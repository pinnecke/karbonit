/**
 * Columnar Binary JSON -- Copyright 2019 Marcus Pinnecke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of
 * the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <jakson/rec.h>
#include <jakson/carbon/revise.h>
#include <jakson/carbon/arr-it.h>
#include <jakson/carbon/internal.h>
#include <jakson/carbon/dot.h>
#include <jakson/carbon/find.h>
#include <jakson/carbon/key.h>
#include <jakson/carbon/commit.h>
#include <jakson/carbon/obj-it.h>

static bool internal_pack_array(arr_it *it);

static bool internal_pack_object(obj_it *it);

static bool internal_pack_column(col_it *it);

static bool internal_commit_update(rec *doc);

static bool carbon_header_rev_inc(rec *doc);

// ---------------------------------------------------------------------------------------------------------------------
void revise_begin(rev *context, rec *revised, rec *original)
{
        context->original = original;
        context->revised = revised;
        rec_clone(context->revised, context->original);
}


static void key_unsigned_set(rec *doc, u64 key)
{
        assert(doc);
        MEMFILE_SAVE_POSITION(&doc->file);
        MEMFILE_SEEK(&doc->file, 0);

        key_write_unsigned(&doc->file, key);

        MEMFILE_RESTORE_POSITION(&doc->file);
}

static void key_signed_set(rec *doc, i64 key)
{
        assert(doc);
        MEMFILE_SAVE_POSITION(&doc->file);
        MEMFILE_SEEK(&doc->file, 0);

        key_write_signed(&doc->file, key);

        MEMFILE_RESTORE_POSITION(&doc->file);
}

static void key_string_set(rec *doc, const char *key)
{
        assert(doc);
        MEMFILE_SAVE_POSITION(&doc->file);
        MEMFILE_SEEK(&doc->file, 0);

        key_update_string(&doc->file, key);

        MEMFILE_RESTORE_POSITION(&doc->file);
}

bool revise_key_generate(unique_id_t *out, rev *context)
{
        key_e type;
        rec_key_type(&type, context->revised);
        if (type == KEY_AUTOKEY) {
                unique_id_t oid;
                unique_id_create(&oid);
                key_unsigned_set(context->revised, oid);
                OPTIONAL_SET(out, oid);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, NULL);
        }
}

bool revise_key_set_unsigned(rev *context, u64 key_value)
{
        key_e type;
        rec_key_type(&type, context->revised);
        if (type == KEY_UKEY) {
                key_unsigned_set(context->revised, key_value);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, NULL);
        }
}

bool revise_key_set_signed(rev *context, i64 key_value)
{
        key_e type;
        rec_key_type(&type, context->revised);
        if (type == KEY_IKEY) {
                key_signed_set(context->revised, key_value);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, NULL);
        }
}

bool revise_key_set_string(rev *context, const char *key_value)
{
        key_e type;
        rec_key_type(&type, context->revised);
        if (type == KEY_SKEY) {
                key_string_set(context->revised, key_value);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, NULL);
        }
}

void revise_set_list_type(rev *context, list_type_e derivation)
{
        arr_it it;
        revise_iterator_open(&it, context);

        MEMFILE_SEEK_FROM_HERE(&it.file, -sizeof(u8));
        derived_e derive_marker;
        abstract_derive_list_to(&derive_marker, LIST_ARRAY, derivation);
        abstract_write_derived_type(&it.file, derive_marker);

        revise_iterator_close(&it);
}

bool revise_iterator_open(arr_it *it, rev *context)
{
        offset_t payload_start = internal_payload_after_header(context->revised);
        if (unlikely(context->revised->file.mode != READ_WRITE)) {
                return error(ERR_PERMISSIONS, "revise iterator on read-only record invoked");
        }
        return internal_arr_it_create(it, &context->revised->file, payload_start);
}

void revise_iterator_close(arr_it *it)
{
        arr_it_drop(it);
}

bool revise_find_begin(find *out, const char *dot, rev *context)
{
        struct dot path;
        dot_from_string(&path, dot);
        bool status = find_exec(out, &path, context->revised);
        dot_drop(&path);
        return status;
}

void revise_find_end(find *find)
{
        find_drop(find);
}

bool revise_remove_one(const char *dot, rec *rev_doc, rec *doc)
{
        rev revise;
        revise_begin(&revise, rev_doc, doc);
        bool status = revise_remove(dot, &revise);
        revise_end(&revise);
        return status;
}

bool revise_remove(const char *path, rev *context)
{
        struct dot dot;
        dot_eval eval;
        bool result;

        if (dot_from_string(&dot, path)) {
                dot_eval_begin_mutable(&eval, &dot, context);

                if (eval.status != PATH_RESOLVED) {
                        result = false;
                } else {
                        switch (eval.result.container) {
                                case ARRAY: {
                                        arr_it *it = &eval.result.containers.array;
                                        result = internal_arr_it_remove(it);
                                }
                                        break;
                                case COLUMN: {
                                        col_it *it = &eval.result.containers.column.it;
                                        u32 elem_pos = eval.result.containers.column.elem_pos;
                                        result = col_it_remove(it, elem_pos);
                                }
                                        break;
                                default: error(ERR_INTERNALERR, NULL);
                                        result = false;
                        }
                }
                dot_eval_end(&eval);
                return result;
        } else {
                error(ERR_DOT_PATH_PARSERR, NULL);
                return false;
        }
}

bool revise_pack(rev *context)
{
        arr_it it;
        revise_iterator_open(&it, context);
        internal_pack_array(&it);
        revise_iterator_close(&it);
        return true;
}

bool revise_shrink(rev *context)
{
        arr_it it;
        revise_iterator_open(&it, context);
        internal_arr_it_fast_forward(&it);
        if (MEMFILE_REMAIN_SIZE(&it.file) > 0) {
                offset_t first_empty_slot = MEMFILE_TELL(&it.file);
                assert(MEMFILE_SIZE(&it.file) > first_empty_slot);
                offset_t shrink_size = MEMFILE_SIZE(&it.file) - first_empty_slot;
                MEMFILE_CUT(&it.file, shrink_size);
        }

        offset_t size;
        MEMBLOCK_SIZE(&size, it.file.memblock);
        revise_iterator_close(&it);
        return true;
}

const rec *revise_end(rev *context)
{
        internal_commit_update(context->revised);
        return context->revised;
}

bool revise_abort(rev *context)
{
        rec_drop(context->revised);
        return true;
}

static bool internal_pack_array(arr_it *it)
{
        assert(it);

        /** shrink this array */
        {
                arr_it this_array;
                bool is_empty_slot, is_array_end;

                internal_arr_it_copy(&this_array, it);
                internal_array_skip_contents(&is_empty_slot, &is_array_end, &this_array);

                if (!is_array_end) {

                        error_if_and_return(!is_empty_slot, ERR_CORRUPTED, NULL);
                        offset_t first_empty_slot_offset = MEMFILE_TELL(&this_array.file);
                        char final;
                        while ((final = *MEMFILE_READ(&this_array.file, sizeof(char))) == 0) {}
                        assert(final == MARRAY_END);
                        offset_t last_empty_slot_offset = MEMFILE_TELL(&this_array.file) - sizeof(char);
                        MEMFILE_SEEK(&this_array.file, first_empty_slot_offset);
                        assert(last_empty_slot_offset > first_empty_slot_offset);

                        MEMFILE_INPLACE_REMOVE(&this_array.file,
                                               last_empty_slot_offset - first_empty_slot_offset);

                        final = *MEMFILE_READ(&this_array.file, sizeof(char));
                        assert(final == MARRAY_END);
                }

                arr_it_drop(&this_array);
        }

        /** shrink contained containers */
        {
                while (arr_it_next(it)) {
                        field_e type;
                        arr_it_field_type(&type, it);
                        switch (type) {
                                case FIELD_NULL:
                                case FIELD_TRUE:
                                case FIELD_FALSE:
                                case FIELD_STRING:
                                case FIELD_NUMBER_U8:
                                case FIELD_NUMBER_U16:
                                case FIELD_NUMBER_U32:
                                case FIELD_NUMBER_U64:
                                case FIELD_NUMBER_I8:
                                case FIELD_NUMBER_I16:
                                case FIELD_NUMBER_I32:
                                case FIELD_NUMBER_I64:
                                case FIELD_NUMBER_FLOAT:
                                case FIELD_BINARY:
                                case FIELD_BINARY_CUSTOM:
                                        /** nothing to shrink, because there are no padded zeros here */
                                        break;
                                case FIELD_ARRAY_UNSORTED_MULTISET:
                                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                                case FIELD_DERIVED_ARRAY_SORTED_SET: {
                                        arr_it array;
                                        internal_arr_it_create(&array, &it->file,
                                                                     it->field.array->begin);
                                        internal_pack_array(&array);
                                        assert(*MEMFILE_PEEK(&array.file, sizeof(char)) ==
                                                   MARRAY_END);
                                        MEMFILE_SKIP(&array.file, sizeof(char));
                                        MEMFILE_SEEK(&it->file, MEMFILE_TELL(&array.file));
                                        arr_it_drop(&array);
                                }
                                        break;
                                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U8_SORTED_SET:
                                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U16_SORTED_SET:
                                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U32_SORTED_SET:
                                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U64_SORTED_SET:
                                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I8_SORTED_SET:
                                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I16_SORTED_SET:
                                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I32_SORTED_SET:
                                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I64_SORTED_SET:
                                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:
                                        col_it_rewind(it->field.column);
                                        internal_pack_column(it->field.column);
                                        MEMFILE_SEEK(&it->file,
                                                     MEMFILE_TELL(&it->field.column->file));
                                        break;
                                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                                case FIELD_DERIVED_OBJECT_SORTED_MAP: {
                                        obj_it object;
                                        internal_obj_it_create(&object, &it->file,
                                                                      it->field.object->content_begin -
                                                                      sizeof(u8));
                                        internal_pack_object(&object);
                                        assert(*MEMFILE_PEEK(&object.file, sizeof(char)) ==
                                                   MOBJECT_END);
                                        MEMFILE_SKIP(&object.file, sizeof(char));
                                        MEMFILE_SEEK(&it->file, MEMFILE_TELL(&object.file));
                                        obj_it_drop(&object);
                                }
                                        break;
                                default: error(ERR_INTERNALERR, NULL);
                                        return false;
                        }
                }
        }

        assert(*MEMFILE_PEEK(&it->file, sizeof(char)) == MARRAY_END);

        return true;
}

static bool internal_pack_object(obj_it *it)
{
        assert(it);

        /** shrink this object */
        {
                obj_it this_object_it;
                bool is_empty_slot, is_object_end;

                internal_obj_it_copy(&this_object_it, it);
                internal_object_skip_contents(&is_empty_slot, &is_object_end, &this_object_it);

                if (!is_object_end) {

                        error_if_and_return(!is_empty_slot, ERR_CORRUPTED, NULL);
                        offset_t first_empty_slot_offset = MEMFILE_TELL(&this_object_it.file);
                        char final;
                        while ((final = *MEMFILE_READ(&this_object_it.file, sizeof(char))) == 0) {}
                        assert(final == MOBJECT_END);
                        offset_t last_empty_slot_offset = MEMFILE_TELL(&this_object_it.file) - sizeof(char);
                        MEMFILE_SEEK(&this_object_it.file, first_empty_slot_offset);
                        assert(last_empty_slot_offset > first_empty_slot_offset);

                        MEMFILE_INPLACE_REMOVE(&this_object_it.file,
                                               last_empty_slot_offset - first_empty_slot_offset);

                        final = *MEMFILE_READ(&this_object_it.file, sizeof(char));
                        assert(final == MOBJECT_END);
                }

                obj_it_drop(&this_object_it);
        }

        /** shrink contained containers */
        {
                while (obj_it_next(it)) {
                        field_e type;
                        internal_obj_it_prop_type(&type, it);
                        switch (type) {
                                case FIELD_NULL:
                                case FIELD_TRUE:
                                case FIELD_FALSE:
                                case FIELD_STRING:
                                case FIELD_NUMBER_U8:
                                case FIELD_NUMBER_U16:
                                case FIELD_NUMBER_U32:
                                case FIELD_NUMBER_U64:
                                case FIELD_NUMBER_I8:
                                case FIELD_NUMBER_I16:
                                case FIELD_NUMBER_I32:
                                case FIELD_NUMBER_I64:
                                case FIELD_NUMBER_FLOAT:
                                case FIELD_BINARY:
                                case FIELD_BINARY_CUSTOM:
                                        /** nothing to shrink, because there are no padded zeros here */
                                        break;
                                case FIELD_ARRAY_UNSORTED_MULTISET:
                                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                                case FIELD_DERIVED_ARRAY_SORTED_SET: {
                                        arr_it array;
                                        internal_arr_it_create(&array, &it->file,
                                                               it->field.value.data.array->begin);
                                        internal_pack_array(&array);
                                        assert(*MEMFILE_PEEK(&array.file, sizeof(char)) ==
                                                   MARRAY_END);
                                        MEMFILE_SKIP(&array.file, sizeof(char));
                                        MEMFILE_SEEK(&it->file, MEMFILE_TELL(&array.file));
                                        arr_it_drop(&array);
                                }
                                        break;
                                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U8_SORTED_SET:
                                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U16_SORTED_SET:
                                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U32_SORTED_SET:
                                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U64_SORTED_SET:
                                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I8_SORTED_SET:
                                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I16_SORTED_SET:
                                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I32_SORTED_SET:
                                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I64_SORTED_SET:
                                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:
                                        col_it_rewind(it->field.value.data.column);
                                        internal_pack_column(it->field.value.data.column);
                                        MEMFILE_SEEK(&it->file,
                                                     MEMFILE_TELL(&it->field.value.data.column->file));
                                        break;
                                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                                case FIELD_DERIVED_OBJECT_SORTED_MAP: {
                                        obj_it object;
                                        internal_obj_it_create(&object, &it->file,
                                                                      it->field.value.data.object->content_begin -
                                                                      sizeof(u8));
                                        internal_pack_object(&object);
                                        assert(*MEMFILE_PEEK(&object.file, sizeof(char)) ==
                                                   MOBJECT_END);
                                        MEMFILE_SKIP(&object.file, sizeof(char));
                                        MEMFILE_SEEK(&it->file, MEMFILE_TELL(&object.file));
                                        obj_it_drop(&object);
                                }
                                        break;
                                default: error(ERR_INTERNALERR, NULL);
                                        return false;
                        }
                }
        }

        assert(*MEMFILE_PEEK(&it->file, sizeof(char)) == MOBJECT_END);

        return true;
}

static bool internal_pack_column(col_it *it)
{
        assert(it);

        u32 free_space = (it->cap - it->num) * internal_get_type_value_size(it->field_type);
        offset_t payload_start = internal_column_get_payload_off(it);
        u64 payload_size = it->num * internal_get_type_value_size(it->field_type);
        MEMFILE_SEEK(&it->file, payload_start);
        MEMFILE_SKIP(&it->file, payload_size);

        if (free_space > 0) {
                MEMFILE_INPLACE_REMOVE(&it->file, free_space);

                MEMFILE_SEEK(&it->file, it->header_begin);
                MEMFILE_SKIP_UINTVAR_STREAM(&it->file); // skip num of elements counter
                MEMFILE_UPDATE_UINTVAR_STREAM(&it->file,
                                              it->num); // update capacity counter to num elems

                MEMFILE_SKIP(&it->file, payload_size);

                return true;
        } else {
                return false;
        }
}

static bool internal_commit_update(rec *doc)
{
        assert(doc);
        return carbon_header_rev_inc(doc);
}

static bool carbon_header_rev_inc(rec *doc)
{
        assert(doc);

        key_e rec_key_type;
        MEMFILE_SAVE_POSITION(&doc->file);
        MEMFILE_SEEK(&doc->file, 0);
        key_read(NULL, &rec_key_type, &doc->file);
        if (rec_has_key(rec_key_type)) {
                u64 raw_data_len = 0;
                const void *raw_data = rec_raw_data(&raw_data_len, doc);
                commit_update(&doc->file, raw_data, raw_data_len);
        }
        MEMFILE_RESTORE_POSITION(&doc->file);

        return true;
}