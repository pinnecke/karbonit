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
#include <jakson/carbon/arr_it.h>
#include <jakson/carbon/internal.h>
#include <jakson/carbon/dot.h>
#include <jakson/carbon/find.h>
#include <jakson/carbon/key.h>
#include <jakson/carbon/commit.h>
#include <jakson/carbon/object.h>

static bool internal_pack_array(arr_it *it);

static bool internal_pack_object(obj_it *it);

static bool internal_pack_column(col_it *it);

static bool internal_commit_update(rec *doc);

static bool carbon_header_rev_inc(rec *doc);

// ---------------------------------------------------------------------------------------------------------------------

void carbon_revise_try_begin(rev *context, rec *revised_doc, rec *doc)
{
        carbon_revise_begin(context, revised_doc, doc);
}

void carbon_revise_begin(rev *context, rec *revised_doc, rec *original)
{
        context->original = original;
        context->revised_doc = revised_doc;
        carbon_clone(context->revised_doc, context->original);
}


static void key_unsigned_set(rec *doc, u64 key)
{
        JAK_ASSERT(doc);
        memfile_save_position(&doc->file);
        memfile_seek(&doc->file, 0);

        carbon_key_write_unsigned(&doc->file, key);

        memfile_restore_position(&doc->file);
}

static void key_signed_set(rec *doc, i64 key)
{
        JAK_ASSERT(doc);
        memfile_save_position(&doc->file);
        memfile_seek(&doc->file, 0);

        carbon_key_write_signed(&doc->file, key);

        memfile_restore_position(&doc->file);
}

static void key_string_set(rec *doc, const char *key)
{
        JAK_ASSERT(doc);
        memfile_save_position(&doc->file);
        memfile_seek(&doc->file, 0);

        carbon_key_update_string(&doc->file, key);

        memfile_restore_position(&doc->file);
}

bool carbon_revise_key_generate(unique_id_t *out, rev *context)
{
        carbon_key_e key_type;
        carbon_key_type(&key_type, context->revised_doc);
        if (key_type == CARBON_KEY_AUTOKEY) {
                unique_id_t oid;
                unique_id_create(&oid);
                key_unsigned_set(context->revised_doc, oid);
                OPTIONAL_SET(out, oid);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, NULL);
        }
}

bool carbon_revise_key_set_unsigned(rev *context, u64 key_value)
{
        carbon_key_e key_type;
        carbon_key_type(&key_type, context->revised_doc);
        if (key_type == CARBON_KEY_UKEY) {
                key_unsigned_set(context->revised_doc, key_value);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, NULL);
        }
}

bool carbon_revise_key_set_signed(rev *context, i64 key_value)
{
        carbon_key_e key_type;
        carbon_key_type(&key_type, context->revised_doc);
        if (key_type == CARBON_KEY_IKEY) {
                key_signed_set(context->revised_doc, key_value);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, NULL);
        }
}

bool carbon_revise_key_set_string(rev *context, const char *key_value)
{
        carbon_key_e key_type;
        carbon_key_type(&key_type, context->revised_doc);
        if (key_type == CARBON_KEY_SKEY) {
                key_string_set(context->revised_doc, key_value);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, NULL);
        }
}

void carbon_revise_set_list_type(rev *context, list_type_e derivation)
{
        arr_it it;
        carbon_revise_iterator_open(&it, context);

        memfile_seek_from_here(&it.file, -sizeof(u8));
        derived_e derive_marker;
        abstract_derive_list_to(&derive_marker, LIST_ARRAY, derivation);
        abstract_write_derived_type(&it.file, derive_marker);

        carbon_revise_iterator_close(&it);
}

bool carbon_revise_iterator_open(arr_it *it, rev *context)
{
        offset_t payload_start = carbon_int_payload_after_header(context->revised_doc);
        if (UNLIKELY(context->revised_doc->file.mode != READ_WRITE)) {
                return error(ERR_PERMISSIONS, "revise iterator on read-only record invoked");
        }
        return internal_arr_it_create(it, &context->revised_doc->file, payload_start);
}

void carbon_revise_iterator_close(arr_it *it)
{
        arr_it_drop(it);
}

bool carbon_revise_find_begin(carbon_find *out, const char *dot, rev *context)
{
        struct dot path;
        dot_from_string(&path, dot);
        bool status = carbon_find_create(out, &path, context->revised_doc);
        dot_drop(&path);
        return status;
}

bool carbon_revise_find_end(carbon_find *find)
{
        return carbon_find_drop(find);
}

bool carbon_revise_remove_one(const char *dot, rec *rev_doc, rec *doc)
{
        rev revise;
        carbon_revise_begin(&revise, rev_doc, doc);
        bool status = carbon_revise_remove(dot, &revise);
        carbon_revise_end(&revise);
        return status;
}

bool carbon_revise_remove(const char *path, rev *context)
{
        struct dot dot;
        carbon_path_evaluator eval;
        bool result;

        if (dot_from_string(&dot, path)) {
                carbon_path_evaluator_begin_mutable(&eval, &dot, context);

                if (eval.status != PATH_RESOLVED) {
                        result = false;
                } else {
                        switch (eval.result.container) {
                                case ARRAY: {
                                        arr_it *it = &eval.result.containers.array.it;
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
                carbon_path_evaluator_end(&eval);
                return result;
        } else {
                error(ERR_DOT_PATH_PARSERR, NULL);
                return false;
        }
}

bool carbon_revise_pack(rev *context)
{
        arr_it it;
        carbon_revise_iterator_open(&it, context);
        internal_pack_array(&it);
        carbon_revise_iterator_close(&it);
        return true;
}

bool carbon_revise_shrink(rev *context)
{
        arr_it it;
        carbon_revise_iterator_open(&it, context);
        internal_arr_it_fast_forward(&it);
        if (memfile_remain_size(&it.file) > 0) {
                offset_t first_empty_slot = memfile_tell(&it.file);
                JAK_ASSERT(memfile_size(&it.file) > first_empty_slot);
                offset_t shrink_size = memfile_size(&it.file) - first_empty_slot;
                memfile_cut(&it.file, shrink_size);
        }

        offset_t size;
        memblock_size(&size, it.file.memblock);
        carbon_revise_iterator_close(&it);
        return true;
}

const rec *carbon_revise_end(rev *context)
{
        internal_commit_update(context->revised_doc);
        return context->revised_doc;
}

bool carbon_revise_abort(rev *context)
{
        carbon_drop(context->revised_doc);
        return true;
}

static bool internal_pack_array(arr_it *it)
{
        JAK_ASSERT(it);

        /** shrink this array */
        {
                arr_it this_array;
                bool is_empty_slot, is_array_end;

                internal_arr_it_copy(&this_array, it);
                carbon_int_array_skip_contents(&is_empty_slot, &is_array_end, &this_array);

                if (!is_array_end) {

                        error_if_and_return(!is_empty_slot, ERR_CORRUPTED, NULL);
                        offset_t first_empty_slot_offset = memfile_tell(&this_array.file);
                        char final;
                        while ((final = *memfile_read(&this_array.file, sizeof(char))) == 0) {}
                        JAK_ASSERT(final == MARRAY_END);
                        offset_t last_empty_slot_offset = memfile_tell(&this_array.file) - sizeof(char);
                        memfile_seek(&this_array.file, first_empty_slot_offset);
                        JAK_ASSERT(last_empty_slot_offset > first_empty_slot_offset);

                        memfile_inplace_remove(&this_array.file,
                                               last_empty_slot_offset - first_empty_slot_offset);

                        final = *memfile_read(&this_array.file, sizeof(char));
                        JAK_ASSERT(final == MARRAY_END);
                }

                arr_it_drop(&this_array);
        }

        /** shrink contained containers */
        {
                while (arr_it_next(it)) {
                        field_type_e type;
                        arr_it_field_type(&type, it);
                        switch (type) {
                                case CARBON_FIELD_NULL:
                                case CARBON_FIELD_TRUE:
                                case CARBON_FIELD_FALSE:
                                case CARBON_FIELD_STRING:
                                case CARBON_FIELD_NUMBER_U8:
                                case CARBON_FIELD_NUMBER_U16:
                                case CARBON_FIELD_NUMBER_U32:
                                case CARBON_FIELD_NUMBER_U64:
                                case CARBON_FIELD_NUMBER_I8:
                                case CARBON_FIELD_NUMBER_I16:
                                case CARBON_FIELD_NUMBER_I32:
                                case CARBON_FIELD_NUMBER_I64:
                                case CARBON_FIELD_NUMBER_FLOAT:
                                case CARBON_FIELD_BINARY:
                                case CARBON_FIELD_BINARY_CUSTOM:
                                        /** nothing to shrink, because there are no padded zeros here */
                                        break;
                                case CARBON_FIELD_ARRAY_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_ARRAY_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_ARRAY_SORTED_SET: {
                                        arr_it array;
                                        internal_arr_it_create(&array, &it->file,
                                                                     it->field.array->begin);
                                        internal_pack_array(&array);
                                        JAK_ASSERT(*memfile_peek(&array.file, sizeof(char)) ==
                                                   MARRAY_END);
                                        memfile_skip(&array.file, sizeof(char));
                                        memfile_seek(&it->file, memfile_tell(&array.file));
                                        arr_it_drop(&array);
                                }
                                        break;
                                case CARBON_FIELD_COLUMN_U8_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_SET:
                                case CARBON_FIELD_COLUMN_U16_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_SET:
                                case CARBON_FIELD_COLUMN_U32_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_SET:
                                case CARBON_FIELD_COLUMN_U64_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_SET:
                                case CARBON_FIELD_COLUMN_I8_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_SET:
                                case CARBON_FIELD_COLUMN_I16_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_SET:
                                case CARBON_FIELD_COLUMN_I32_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_SET:
                                case CARBON_FIELD_COLUMN_I64_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_SET:
                                case CARBON_FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                                case CARBON_FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:
                                        col_it_rewind(it->field.column);
                                        internal_pack_column(it->field.column);
                                        memfile_seek(&it->file,
                                                     memfile_tell(&it->field.column->file));
                                        break;
                                case CARBON_FIELD_OBJECT_UNSORTED_MULTIMAP:
                                case CARBON_FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                                case CARBON_FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                                case CARBON_FIELD_DERIVED_OBJECT_SORTED_MAP: {
                                        obj_it object;
                                        internal_carbon_object_create(&object, &it->file,
                                                                      it->field.object->object_contents_off -
                                                                      sizeof(u8));
                                        internal_pack_object(&object);
                                        JAK_ASSERT(*memfile_peek(&object.memfile, sizeof(char)) ==
                                                   MOBJECT_END);
                                        memfile_skip(&object.memfile, sizeof(char));
                                        memfile_seek(&it->file, memfile_tell(&object.memfile));
                                        carbon_object_drop(&object);
                                }
                                        break;
                                default: error(ERR_INTERNALERR, NULL);
                                        return false;
                        }
                }
        }

        JAK_ASSERT(*memfile_peek(&it->file, sizeof(char)) == MARRAY_END);

        return true;
}

static bool internal_pack_object(obj_it *it)
{
        JAK_ASSERT(it);

        /** shrink this object */
        {
                obj_it this_object_it;
                bool is_empty_slot, is_object_end;

                internal_carbon_object_copy(&this_object_it, it);
                carbon_int_object_skip_contents(&is_empty_slot, &is_object_end, &this_object_it);

                if (!is_object_end) {

                        error_if_and_return(!is_empty_slot, ERR_CORRUPTED, NULL);
                        offset_t first_empty_slot_offset = memfile_tell(&this_object_it.memfile);
                        char final;
                        while ((final = *memfile_read(&this_object_it.memfile, sizeof(char))) == 0) {}
                        JAK_ASSERT(final == MOBJECT_END);
                        offset_t last_empty_slot_offset = memfile_tell(&this_object_it.memfile) - sizeof(char);
                        memfile_seek(&this_object_it.memfile, first_empty_slot_offset);
                        JAK_ASSERT(last_empty_slot_offset > first_empty_slot_offset);

                        memfile_inplace_remove(&this_object_it.memfile,
                                               last_empty_slot_offset - first_empty_slot_offset);

                        final = *memfile_read(&this_object_it.memfile, sizeof(char));
                        JAK_ASSERT(final == MOBJECT_END);
                }

                carbon_object_drop(&this_object_it);
        }

        /** shrink contained containers */
        {
                while (carbon_object_next(it)) {
                        field_type_e type;
                        internal_carbon_object_prop_type(&type, it);
                        switch (type) {
                                case CARBON_FIELD_NULL:
                                case CARBON_FIELD_TRUE:
                                case CARBON_FIELD_FALSE:
                                case CARBON_FIELD_STRING:
                                case CARBON_FIELD_NUMBER_U8:
                                case CARBON_FIELD_NUMBER_U16:
                                case CARBON_FIELD_NUMBER_U32:
                                case CARBON_FIELD_NUMBER_U64:
                                case CARBON_FIELD_NUMBER_I8:
                                case CARBON_FIELD_NUMBER_I16:
                                case CARBON_FIELD_NUMBER_I32:
                                case CARBON_FIELD_NUMBER_I64:
                                case CARBON_FIELD_NUMBER_FLOAT:
                                case CARBON_FIELD_BINARY:
                                case CARBON_FIELD_BINARY_CUSTOM:
                                        /** nothing to shrink, because there are no padded zeros here */
                                        break;
                                case CARBON_FIELD_ARRAY_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_ARRAY_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_ARRAY_SORTED_SET: {
                                        arr_it array;
                                        internal_arr_it_create(&array, &it->memfile,
                                                               it->field.value.data.array->begin);
                                        internal_pack_array(&array);
                                        JAK_ASSERT(*memfile_peek(&array.file, sizeof(char)) ==
                                                   MARRAY_END);
                                        memfile_skip(&array.file, sizeof(char));
                                        memfile_seek(&it->memfile, memfile_tell(&array.file));
                                        arr_it_drop(&array);
                                }
                                        break;
                                case CARBON_FIELD_COLUMN_U8_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_SET:
                                case CARBON_FIELD_COLUMN_U16_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_SET:
                                case CARBON_FIELD_COLUMN_U32_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_SET:
                                case CARBON_FIELD_COLUMN_U64_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_SET:
                                case CARBON_FIELD_COLUMN_I8_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_SET:
                                case CARBON_FIELD_COLUMN_I16_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_SET:
                                case CARBON_FIELD_COLUMN_I32_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_SET:
                                case CARBON_FIELD_COLUMN_I64_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_SET:
                                case CARBON_FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                                case CARBON_FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:
                                        col_it_rewind(it->field.value.data.column);
                                        internal_pack_column(it->field.value.data.column);
                                        memfile_seek(&it->memfile,
                                                     memfile_tell(&it->field.value.data.column->file));
                                        break;
                                case CARBON_FIELD_OBJECT_UNSORTED_MULTIMAP:
                                case CARBON_FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                                case CARBON_FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                                case CARBON_FIELD_DERIVED_OBJECT_SORTED_MAP: {
                                        obj_it object;
                                        internal_carbon_object_create(&object, &it->memfile,
                                                                      it->field.value.data.object->object_contents_off -
                                                                      sizeof(u8));
                                        internal_pack_object(&object);
                                        JAK_ASSERT(*memfile_peek(&object.memfile, sizeof(char)) ==
                                                   MOBJECT_END);
                                        memfile_skip(&object.memfile, sizeof(char));
                                        memfile_seek(&it->memfile, memfile_tell(&object.memfile));
                                        carbon_object_drop(&object);
                                }
                                        break;
                                default: error(ERR_INTERNALERR, NULL);
                                        return false;
                        }
                }
        }

        JAK_ASSERT(*memfile_peek(&it->memfile, sizeof(char)) == MOBJECT_END);

        return true;
}

static bool internal_pack_column(col_it *it)
{
        JAK_ASSERT(it);

        u32 free_space = (it->cap - it->num) * carbon_int_get_type_value_size(it->field_type);
        offset_t payload_start = carbon_int_column_get_payload_off(it);
        u64 payload_size = it->num * carbon_int_get_type_value_size(it->field_type);
        memfile_seek(&it->file, payload_start);
        memfile_skip(&it->file, payload_size);

        if (free_space > 0) {
                memfile_inplace_remove(&it->file, free_space);

                memfile_seek(&it->file, it->header_begin);
                memfile_skip_uintvar_stream(&it->file); // skip num of elements counter
                memfile_update_uintvar_stream(&it->file,
                                              it->num); // update capacity counter to num elems

                memfile_skip(&it->file, payload_size);

                return true;
        } else {
                return false;
        }
}

static bool internal_commit_update(rec *doc)
{
        JAK_ASSERT(doc);
        return carbon_header_rev_inc(doc);
}

static bool carbon_header_rev_inc(rec *doc)
{
        JAK_ASSERT(doc);

        carbon_key_e key_type;
        memfile_save_position(&doc->file);
        memfile_seek(&doc->file, 0);
        carbon_key_read(NULL, &key_type, &doc->file);
        if (carbon_has_key(key_type)) {
                u64 raw_data_len = 0;
                const void *raw_data = carbon_raw_data(&raw_data_len, doc);
                commit_update(&doc->file, raw_data, raw_data_len);
        }
        memfile_restore_position(&doc->file);

        return true;
}