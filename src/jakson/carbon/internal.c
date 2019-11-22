/**
 * Columnar Binary JSON -- Copyright 2019 Marcus Pinnecke
 * This file is for internal usage only; do not call these functions from outside
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

#include <jakson/std/uintvar/stream.h>
#include <jakson/rec.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/mime.h>
#include <jakson/carbon/internal.h>
#include <jakson/carbon/arr-it.h>
#include <jakson/carbon/col-it.h>
#include <jakson/carbon/obj-it.h>
#include <jakson/carbon/key.h>
#include <jakson/carbon/commit.h>
#include <jakson/json/parser.h>
#include <jakson/carbon/obj-it.h>

static void marker_insert(memfile *memfile, u8 marker);

static bool array_is_slot_occupied(bool *is_empty_slot, bool *is_array_end, arr_it *it);

static bool object_it_is_slot_occupied(bool *is_empty_slot, bool *is_object_end, obj_it *it);

static bool is_slot_occupied(bool *is_empty_slot, bool *is_array_end, memfile *file, u8 end_marker);

static bool array_next_no_load(bool *is_empty_slot, bool *is_array_end, arr_it *it);

static bool object_it_next_no_load(bool *is_empty_slot, bool *is_array_end, obj_it *it);

static void int_carbon_from_json_elem(insert *ins, const json_element *elem, bool is_root);

static void int_insert_prop_object(insert *oins, json_object *obj);

static void
insert_embedded_container(memfile *memfile, u8 begin_marker, u8 end_marker, u8 capacity)
{
        memfile_ensure_space(memfile, sizeof(u8));
        marker_insert(memfile, begin_marker);

        memfile_ensure_space(memfile, capacity + sizeof(u8));

        offset_t payload_begin = memfile_tell(memfile);
        memfile_seek(memfile, payload_begin + capacity);

        marker_insert(memfile, end_marker);

        /** seek to first entry in container */
        memfile_seek(memfile, payload_begin);
}

bool internal_insert_object(memfile *file, map_type_e derivation, size_t nbytes)
{
        assert(derivation == MAP_UNSORTED_MULTIMAP || derivation == MAP_SORTED_MULTIMAP ||
               derivation == MAP_UNSORTED_MAP || derivation == MAP_SORTED_MAP);
        derived_e begin_marker;
        abstract_derive_map_to(&begin_marker, derivation);
        insert_embedded_container(file, begin_marker, MOBJECT_END, nbytes);
        return true;
}

bool internal_insert_array(memfile *file, list_type_e derivation, size_t nbytes)
{
        assert(derivation == LIST_UNSORTED_MULTISET || derivation == LIST_SORTED_MULTISET ||
               derivation == LIST_UNSORTED_SET || derivation == LIST_SORTED_SET);
        derived_e begin_marker;
        abstract_derive_list_to(&begin_marker, LIST_ARRAY, derivation);
        insert_embedded_container(file, begin_marker, MARRAY_END, nbytes);
        return true;
}

bool internal_insert_column(memfile *memfile_in, list_type_e derivation, col_it_type_e type,
                              size_t capactity)
{
        assert(derivation == LIST_UNSORTED_MULTISET || derivation == LIST_SORTED_MULTISET ||
               derivation == LIST_UNSORTED_SET || derivation == LIST_SORTED_SET);

        field_e column_type = field_for_column(derivation, type);

        memfile_ensure_space(memfile_in, sizeof(u8));
        marker_insert(memfile_in, column_type);

        u32 num_elements = 0;
        u32 cap_elements = capactity;

        memfile_write_uintvar_stream(NULL, memfile_in, num_elements);
        memfile_write_uintvar_stream(NULL, memfile_in, cap_elements);

        offset_t payload_begin = memfile_tell(memfile_in);

        size_t type_size = internal_get_type_value_size(column_type);

        size_t nbytes = capactity * type_size;
        memfile_ensure_space(memfile_in, nbytes + sizeof(u8) + 2 * sizeof(u32));

        /** seek to first entry in column */
        memfile_seek(memfile_in, payload_begin);

        return true;
}

size_t internal_get_type_size_encoded(field_e type)
{
        size_t type_size = sizeof(media_type); /** at least the media type marker is required */
        switch (type) {
                case FIELD_NULL:
                case FIELD_TRUE:
                case FIELD_FALSE:
                        /** only media type marker is required */
                        break;
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_I8:
                        type_size += sizeof(u8);
                        break;
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_I16:
                        type_size += sizeof(u16);
                        break;
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_I32:
                        type_size += sizeof(u32);
                        break;
                case FIELD_NUMBER_U64:
                case FIELD_NUMBER_I64:
                        type_size += sizeof(u64);
                        break;
                case FIELD_NUMBER_FLOAT:
                        type_size += sizeof(float);
                        break;
                default: error(ERR_INTERNALERR, NULL);
                        return 0;
        }
        return type_size;
}

size_t internal_get_type_value_size(field_e type)
{
        switch (type) {
                case FIELD_NULL:
                case FIELD_TRUE:
                case FIELD_FALSE:
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:
                        return sizeof(media_type); /** these constant values are determined by their media type markers */
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_I8:
                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_SET:
                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_SET:
                        return sizeof(u8);
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_I16:
                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_SET:
                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_SET:
                        return sizeof(u16);
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_I32:
                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_SET:
                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_SET:
                        return sizeof(u32);
                case FIELD_NUMBER_U64:
                case FIELD_NUMBER_I64:
                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_SET:
                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET:
                        return sizeof(u64);
                case FIELD_NUMBER_FLOAT:
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                        return sizeof(float);
                default: error(ERR_INTERNALERR, NULL);
                        return 0;
        }
}

bool internal_array_next(bool *is_empty_slot, bool *is_array_end, arr_it *it)
{
        if (internal_array_refresh(is_empty_slot, is_array_end, it)) {
                carbon_field_skip(&it->file);
                return true;
        } else {
                return false;
        }
}

bool internal_object_it_next(bool *is_empty_slot, bool *is_object_end, obj_it *it)
{
        if (internal_object_it_refresh(is_empty_slot, is_object_end, it)) {
                internal_object_it_prop_value_skip(it);
                return true;
        } else {
                return false;
        }
}

bool internal_object_it_refresh(bool *is_empty_slot, bool *is_object_end, obj_it *it)
{
        if (object_it_is_slot_occupied(is_empty_slot, is_object_end, it)) {
                internal_object_it_prop_key_access(it);
                internal_field_data_access(&it->file, &it->field.value.data);
                return true;
        } else {
                return false;
        }
}

bool internal_object_it_prop_key_access(obj_it *it)
{
        it->field.key.start = memfile_tell(&it->file);
        it->field.key.name_len = memfile_read_uintvar_stream(NULL, &it->file);
        it->field.key.name = memfile_peek(&it->file, it->field.key.name_len);
        memfile_skip(&it->file, it->field.key.name_len);
        it->field.value.start = memfile_tell(&it->file);
        it->field.value.data.type = *MEMFILE_PEEK(&it->file, u8);

        return true;
}

bool internal_object_it_prop_value_skip(obj_it *it)
{
        memfile_seek(&it->file, it->field.value.start);
        return carbon_field_skip(&it->file);
}

bool internal_object_it_prop_skip(obj_it *it)
{
        it->field.key.name_len = memfile_read_uintvar_stream(NULL, &it->file);
        memfile_skip(&it->file, it->field.key.name_len);
        return carbon_field_skip(&it->file);
}

bool internal_object_skip_contents(bool *is_empty_slot, bool *is_array_end, obj_it *it)
{
        while (object_it_next_no_load(is_empty_slot, is_array_end, it)) {}
        return true;
}

bool internal_array_skip_contents(bool *is_empty_slot, bool *is_array_end, arr_it *it)
{
        while (array_next_no_load(is_empty_slot, is_array_end, it)) {}
        return true;
}

bool internal_array_refresh(bool *is_empty_slot, bool *is_array_end, arr_it *it)
{
        internal_field_drop(&it->field);
        if (array_is_slot_occupied(is_empty_slot, is_array_end, it)) {
                internal_array_field_read(it);
                internal_field_data_access(&it->file, &it->field);
                return true;
        } else {
                return false;
        }
}

bool internal_array_field_read(arr_it *it)
{
        error_if_and_return(memfile_remain_size(&it->file) < 1, ERR_ILLEGALOP, NULL);
        memfile_save_position(&it->file);
        it->field_offset = memfile_tell(&it->file);
        u8 media_type = *memfile_read(&it->file, 1);
        error_if_and_return(media_type == 0, ERR_NOTFOUND, NULL)
        error_if_and_return(media_type == MARRAY_END, ERR_OUTOFBOUNDS, NULL)
        it->field.type = media_type;
        memfile_restore_position(&it->file);
        return true;
}

bool internal_field_data_access(memfile *file, field *field)
{
        memfile_save_position(file);
        memfile_skip(file, sizeof(media_type));

        switch (field->type) {
                case FIELD_NULL:
                case FIELD_TRUE:
                case FIELD_FALSE:
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_U64:
                case FIELD_NUMBER_I8:
                case FIELD_NUMBER_I16:
                case FIELD_NUMBER_I32:
                case FIELD_NUMBER_I64:
                case FIELD_NUMBER_FLOAT:
                        break;
                case FIELD_STRING: {
                        u8 nbytes;
                        uintvar_stream_t len = (uintvar_stream_t) memfile_peek(file, 1);
                        field->len = uintvar_stream_read(&nbytes, len);

                        memfile_skip(file, nbytes);
                }
                        break;
                case FIELD_BINARY: {
                        /** read mime type with variable-length integer type */
                        u64 mime_id = memfile_read_uintvar_stream(NULL, file);

                        field->mime = mime_by_id(mime_id);
                        field->mime_len = strlen(field->mime);

                        /** read blob length */
                        field->len = memfile_read_uintvar_stream(NULL, file);

                        /** the mem points now to the actual blob data, which is used by the iterator to set the field */
                }
                        break;
                case FIELD_BINARY_CUSTOM: {
                        /** read mime type str_buf */
                        field->mime_len = memfile_read_uintvar_stream(NULL, file);
                        field->mime = memfile_read(file, field->mime_len);

                        /** read blob length */
                        field->len = memfile_read_uintvar_stream(NULL, file);

                        /** the mem points now to the actual blob data, which is used by the iterator to set the field */
                }
                        break;
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET:
                        internal_field_create(field);
                        field->arr_it.created = true;
                        internal_arr_it_create(field->array, file,
                                               memfile_tell(file) - sizeof(u8));
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
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                        internal_field_create(field);
                        field->col_it_created = true;
                        col_it_create(field->column, file,
                                                memfile_tell(file) - sizeof(u8));
                }
                        break;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP:
                        internal_field_create(field);
                        field->obj_it.created = true;
                        internal_obj_it_create(field->object, file,
                                                      memfile_tell(file) - sizeof(u8));
                        break;
                default:
                        return error(ERR_CORRUPTED, NULL);
        }

        field->data = memfile_peek(file, 1);
        memfile_restore_position(file);
        return true;
}

offset_t internal_column_get_payload_off(col_it *it)
{
        memfile_save_position(&it->file);
        memfile_seek(&it->file, it->header_begin);
        memfile_skip_uintvar_stream(&it->file); // skip num of elements
        memfile_skip_uintvar_stream(&it->file); // skip capacity of elements
        offset_t result = memfile_tell(&it->file);
        memfile_restore_position(&it->file);
        return result;
}

offset_t internal_payload_after_header(rec *doc)
{
        offset_t result = 0;
        key_e key_type;

        memfile_save_position(&doc->file);
        memfile_seek(&doc->file, 0);

        if (LIKELY(key_skip(&key_type, &doc->file))) {
                if (key_type != CARBON_KEY_NOKEY) {
                        commit_skip(&doc->file);
                }
                result = memfile_tell(&doc->file);
        }

        memfile_restore_position(&doc->file);

        return result;
}

u64 internal_header_get_commit_hash(rec *doc)
{
        JAK_ASSERT(doc);
        u64 rev = 0;
        key_e key_type;

        memfile_save_position(&doc->file);
        memfile_seek(&doc->file, 0);

        key_skip(&key_type, &doc->file);
        if (key_type != CARBON_KEY_NOKEY) {
                commit_read(&rev, &doc->file);
        }

        memfile_restore_position(&doc->file);
        return rev;
}

void internal_history_push(vec ofType(offset_t) *vec, offset_t off)
{
        JAK_ASSERT(vec);
        vector_push(vec, &off, 1);
}

void internal_history_clear(vec ofType(offset_t) *vec)
{
        JAK_ASSERT(vec);
        vector_clear(vec);
}

offset_t internal_history_pop(vec ofType(offset_t) *vec)
{
        JAK_ASSERT(vec);
        JAK_ASSERT(internal_history_has(vec));
        return *(offset_t *) vector_pop(vec);
}

offset_t internal_history_peek(vec ofType(offset_t) *vec)
{
        JAK_ASSERT(vec);
        JAK_ASSERT(internal_history_has(vec));
        return *(offset_t *) vector_peek(vec);
}

bool internal_history_has(vec ofType(offset_t) *vec)
{
        JAK_ASSERT(vec);
        return !vector_is_empty(vec);
}

bool internal_field_create(field *field)
{
        field->arr_it.created = false;
        field->arr_it.accessed = false;
        field->obj_it.created = false;
        field->obj_it.accessed = false;
        field->col_it_created = false;
        field->array = MALLOC(sizeof(arr_it));
        field->object = MALLOC(sizeof(obj_it));
        field->column = MALLOC(sizeof(col_it));
        return true;
}

bool internal_field_clone(field *dst, field *src)
{
        dst->type = src->type;
        dst->data = src->data;
        dst->len = src->len;
        dst->mime = src->mime;
        dst->mime_len = src->mime_len;
        dst->arr_it.created = src->arr_it.created;
        dst->arr_it.accessed = src->arr_it.accessed;
        dst->obj_it.created = src->obj_it.created;
        dst->obj_it.accessed = src->obj_it.accessed;
        dst->col_it_created = src->col_it_created;
        dst->array = MALLOC(sizeof(arr_it));
        dst->object = MALLOC(sizeof(obj_it));
        dst->column = MALLOC(sizeof(col_it));

        if (internal_field_object_it_opened(src)) {
                internal_obj_it_clone(dst->object, src->object);
        } else if (internal_field_column_it_opened(src)) {
                col_it_clone(dst->column, src->column);
        } else if (internal_field_array_opened(src)) {
                internal_arr_it_clone(dst->array, src->array);
        }
        return true;
}

bool internal_field_drop(field *field)
{
        internal_field_auto_close(field);
        free(field->array);
        free(field->object);
        free(field->column);
        field->array = NULL;
        field->object = NULL;
        field->column = NULL;
        return true;
}

bool internal_field_object_it_opened(field *field)
{
        JAK_ASSERT(field);
        return field->obj_it.created && field->object != NULL;
}

bool internal_field_array_opened(field *field)
{
        JAK_ASSERT(field);
        return field->arr_it.created && field->array != NULL;
}

bool internal_field_column_it_opened(field *field)
{
        JAK_ASSERT(field);
        return field->col_it_created && field->column != NULL;
}

void internal_auto_close_nested_array(field *field)
{
        if (internal_field_array_opened(field)) {
                arr_it_drop(field->array);
                ZERO_MEMORY(field->array, sizeof(arr_it));
        }
}

void internal_auto_close_nested_object_it(field *field)
{
        if (internal_field_object_it_opened(field)) {
                obj_it_drop(field->object);
                ZERO_MEMORY(field->object, sizeof(obj_it));
        }
}

void internal_auto_close_nested_column_it(field *field)
{
        if (internal_field_column_it_opened(field)) {
                ZERO_MEMORY(field->column, sizeof(col_it));
        }
}

bool internal_field_auto_close(field *field)
{
        if (field->arr_it.created && !field->arr_it.accessed) {
                internal_auto_close_nested_array(field);
                field->arr_it.created = false;
                field->arr_it.accessed = false;
        }
        if (field->obj_it.created && !field->obj_it.accessed) {
                internal_auto_close_nested_object_it(field);
                field->obj_it.created = false;
                field->obj_it.accessed = false;
        }
        if (field->col_it_created) {
                internal_auto_close_nested_column_it(field);
                field->obj_it.created = false;
        }

        return true;
}

bool internal_field_field_type(field_e *type, field *field)
{
        *type = field->type;
        return true;
}

bool internal_field_bool_value(bool *value, field *field)
{
        bool is_true = field->type == FIELD_TRUE;
        bool is_false = field->type == FIELD_FALSE;
        if (LIKELY(is_true || is_false)) {
                *value = is_true;
                return true;
        } else {
                error(ERR_TYPEMISMATCH, NULL);
                return false;
        }
}

bool internal_field_is_null(bool *is_null, field *field)
{
        *is_null = field->type == FIELD_NULL;
        return true;
}

bool internal_field_u8_value(u8 *value, field *field)
{
        error_if_and_return(field->type != FIELD_NUMBER_U8, ERR_TYPEMISMATCH, NULL);
        *value = *(u8 *) field->data;
        return true;
}

bool internal_field_u16_value(u16 *value, field *field)
{
        error_if_and_return(field->type != FIELD_NUMBER_U16, ERR_TYPEMISMATCH, NULL);
        *value = *(u16 *) field->data;
        return true;
}

bool internal_field_u32_value(u32 *value, field *field)
{
        error_if_and_return(field->type != FIELD_NUMBER_U32, ERR_TYPEMISMATCH, NULL);
        *value = *(u32 *) field->data;
        return true;
}

bool internal_field_u64_value(u64 *value, field *field)
{
        error_if_and_return(field->type != FIELD_NUMBER_U64, ERR_TYPEMISMATCH, NULL);
        *value = *(u64 *) field->data;
        return true;
}

bool internal_field_i8_value(i8 *value, field *field)
{
        error_if_and_return(field->type != FIELD_NUMBER_I8, ERR_TYPEMISMATCH, NULL);
        *value = *(i8 *) field->data;
        return true;
}

bool internal_field_i16_value(i16 *value, field *field)
{
        error_if_and_return(field->type != FIELD_NUMBER_I16, ERR_TYPEMISMATCH, NULL);
        *value = *(i16 *) field->data;
        return true;
}

bool internal_field_i32_value(i32 *value, field *field)
{
        error_if_and_return(field->type != FIELD_NUMBER_I32, ERR_TYPEMISMATCH, NULL);
        *value = *(i32 *) field->data;
        return true;
}

bool internal_field_i64_value(i64 *value, field *field)
{
        error_if_and_return(field->type != FIELD_NUMBER_I64, ERR_TYPEMISMATCH, NULL);
        *value = *(i64 *) field->data;
        return true;
}

bool internal_field_float_value(float *value, field *field)
{
        error_if_and_return(field->type != FIELD_NUMBER_FLOAT, ERR_TYPEMISMATCH, NULL);
        *value = *(float *) field->data;
        return true;
}

bool
internal_field_float_value_nullable(bool *is_null_in, float *value, field *field)
{
        error_if_and_return(field->type != FIELD_NUMBER_FLOAT, ERR_TYPEMISMATCH, NULL);
        float read_value = *(float *) field->data;
        OPTIONAL_SET(value, read_value);
        OPTIONAL_SET(is_null_in, IS_NULL_FLOAT(read_value));

        return true;
}

bool internal_field_signed_value_nullable(bool *is_null_in, i64 *value, field *field)
{
        switch (field->type) {
                case FIELD_NUMBER_I8: {
                        i8 read_value;
                        internal_field_i8_value(&read_value, field);
                        OPTIONAL_SET(value, read_value);
                        OPTIONAL_SET(is_null_in, IS_NULL_I8(read_value));
                }
                        break;
                case FIELD_NUMBER_I16: {
                        i16 read_value;
                        internal_field_i16_value(&read_value, field);
                        OPTIONAL_SET(value, read_value);
                        OPTIONAL_SET(is_null_in, IS_NULL_I16(read_value));
                }
                        break;
                case FIELD_NUMBER_I32: {
                        i32 read_value;
                        internal_field_i32_value(&read_value, field);
                        OPTIONAL_SET(value, read_value);
                        OPTIONAL_SET(is_null_in, IS_NULL_I32(read_value));
                }
                        break;
                case FIELD_NUMBER_I64: {
                        i64 read_value;
                        internal_field_i64_value(&read_value, field);
                        OPTIONAL_SET(value, read_value);
                        OPTIONAL_SET(is_null_in, IS_NULL_I64(read_value));
                }
                        break;
                default: error(ERR_TYPEMISMATCH, NULL);
                        return false;
        }
        return true;
}

bool internal_field_unsigned_value_nullable(bool *is_null_in, u64 *value, field *field)
{
        switch (field->type) {
                case FIELD_NUMBER_U8: {
                        u8 read_value;
                        internal_field_u8_value(&read_value, field);
                        OPTIONAL_SET(value, read_value);
                        OPTIONAL_SET(is_null_in, IS_NULL_U8(read_value));
                }
                        break;
                case FIELD_NUMBER_U16: {
                        u16 read_value;
                        internal_field_u16_value(&read_value, field);
                        OPTIONAL_SET(value, read_value);
                        OPTIONAL_SET(is_null_in, IS_NULL_U16(read_value));
                }
                        break;
                case FIELD_NUMBER_U32: {
                        u32 read_value;
                        internal_field_u32_value(&read_value, field);
                        OPTIONAL_SET(value, read_value);
                        OPTIONAL_SET(is_null_in, IS_NULL_U32(read_value));
                }
                        break;
                case FIELD_NUMBER_U64: {
                        u64 read_value;
                        internal_field_u64_value(&read_value, field);
                        OPTIONAL_SET(value, read_value);
                        OPTIONAL_SET(is_null_in, IS_NULL_U64(read_value));
                }
                        break;
                default: error(ERR_TYPEMISMATCH, NULL);
                        return false;
        }
        return true;
}

bool internal_field_signed_value(i64 *value, field *field)
{
        switch (field->type) {
                case FIELD_NUMBER_I8: {
                        i8 read_value;
                        internal_field_i8_value(&read_value, field);
                        *value = read_value;
                }
                        break;
                case FIELD_NUMBER_I16: {
                        i16 read_value;
                        internal_field_i16_value(&read_value, field);
                        *value = read_value;
                }
                        break;
                case FIELD_NUMBER_I32: {
                        i32 read_value;
                        internal_field_i32_value(&read_value, field);
                        *value = read_value;
                }
                        break;
                case FIELD_NUMBER_I64: {
                        i64 read_value;
                        internal_field_i64_value(&read_value, field);
                        *value = read_value;
                }
                        break;
                default: error(ERR_TYPEMISMATCH, NULL);
                        return false;
        }
        return true;
}

bool internal_field_unsigned_value(u64 *value, field *field)
{
        switch (field->type) {
                case FIELD_NUMBER_U8: {
                        u8 read_value;
                        internal_field_u8_value(&read_value, field);
                        *value = read_value;
                }
                        break;
                case FIELD_NUMBER_U16: {
                        u16 read_value;
                        internal_field_u16_value(&read_value, field);
                        *value = read_value;
                }
                        break;
                case FIELD_NUMBER_U32: {
                        u32 read_value;
                        internal_field_u32_value(&read_value, field);
                        *value = read_value;
                }
                        break;
                case FIELD_NUMBER_U64: {
                        u64 read_value;
                        internal_field_u64_value(&read_value, field);
                        *value = read_value;
                }
                        break;
                default: error(ERR_TYPEMISMATCH, NULL);
                        return false;
        }
        return true;
}

const char *internal_field_string_value(u64 *strlen, field *field)
{
        error_if_and_return(field == NULL, ERR_NULLPTR, NULL);
        error_if_and_return(field->type != FIELD_STRING, ERR_TYPEMISMATCH, NULL);
        *strlen = field->len;
        return field->data;
}

bool
internal_field_binary_value(binary_field *out, field *field)
{
        error_if_and_return(field->type != FIELD_BINARY &&
                 field->type != FIELD_BINARY_CUSTOM,
                 ERR_TYPEMISMATCH, NULL);
        out->blob = field->data;
        out->blob_len = field->len;
        out->mime = field->mime;
        out->mime_len = field->mime_len;
        return true;
}

arr_it *internal_field_array_value(field *field)
{
        error_if_and_return(!field, ERR_NULLPTR, NULL);
        error_if_and_return(!field_is_array_or_subtype(field->type), ERR_TYPEMISMATCH, NULL);
        field->arr_it.accessed = true;
        return field->array;
}

obj_it *internal_field_object_value(field *field)
{
        error_if_and_return(!field, ERR_NULLPTR, NULL);
        error_if_and_return(!field_is_object_or_subtype(field->type), ERR_TYPEMISMATCH, NULL);
        field->obj_it.accessed = true;
        return field->object;
}

col_it *internal_field_column_value(field *field)
{
        error_if_and_return(!field, ERR_NULLPTR, NULL);
        error_if_and_return(!field_is_column_or_subtype(field->type), ERR_TYPEMISMATCH, NULL);
        return field->column;
}

bool internal_field_remove(memfile *memfile, field_e type)
{
        JAK_ASSERT((field_e) *memfile_peek(memfile, sizeof(u8)) == type);
        offset_t start_off = memfile_tell(memfile);
        memfile_skip(memfile, sizeof(u8));
        size_t rm_nbytes = sizeof(u8); /** at least the type marker must be removed */
        switch (type) {
                case FIELD_NULL:
                case FIELD_TRUE:
                case FIELD_FALSE:
                        /** nothing to do */
                        break;
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_I8:
                        rm_nbytes += sizeof(u8);
                        break;
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_I16:
                        rm_nbytes += sizeof(u16);
                        break;
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_I32:
                        rm_nbytes += sizeof(u32);
                        break;
                case FIELD_NUMBER_U64:
                case FIELD_NUMBER_I64:
                        rm_nbytes += sizeof(u64);
                        break;
                case FIELD_NUMBER_FLOAT:
                        rm_nbytes += sizeof(float);
                        break;
                case FIELD_STRING: {
                        u8 len_nbytes;  /** number of bytes used to store str_buf length */
                        u64 str_len; /** the number of characters of the str_buf field */

                        str_len = memfile_read_uintvar_stream(&len_nbytes, memfile);

                        rm_nbytes += len_nbytes + str_len;
                }
                        break;
                case FIELD_BINARY: {
                        u8 mime_nbytes; /** number of bytes for mime type */
                        u8 blob_length_nbytes; /** number of bytes to store blob length */
                        u64 blob_nbytes; /** number of bytes to store actual blob data */

                        /** get bytes used for mime type id */
                        memfile_read_uintvar_stream(&mime_nbytes, memfile);

                        /** get bytes used for blob length info */
                        blob_nbytes = memfile_read_uintvar_stream(&blob_length_nbytes, memfile);

                        rm_nbytes += mime_nbytes + blob_length_nbytes + blob_nbytes;
                }
                        break;
                case FIELD_BINARY_CUSTOM: {
                        u8 custom_type_strlen_nbytes; /** number of bytes for type name str_buf length info */
                        u8 custom_type_strlen; /** number of characters to encode type name str_buf */
                        u8 blob_length_nbytes; /** number of bytes to store blob length */
                        u64 blob_nbytes; /** number of bytes to store actual blob data */

                        /** get bytes for custom type str_buf len, and the actual length */
                        custom_type_strlen = memfile_read_uintvar_stream(&custom_type_strlen_nbytes, memfile);
                        memfile_skip(memfile, custom_type_strlen);

                        /** get bytes used for blob length info */
                        blob_nbytes = memfile_read_uintvar_stream(&blob_length_nbytes, memfile);

                        rm_nbytes += custom_type_strlen_nbytes + custom_type_strlen + blob_length_nbytes + blob_nbytes;
                }
                        break;
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET: {
                        arr_it it;

                        offset_t begin_off = memfile_tell(memfile);
                        internal_arr_it_create(&it, memfile, begin_off - sizeof(u8));
                        internal_arr_it_fast_forward(&it);
                        offset_t end_off = internal_arr_it_memfilepos(&it);
                        arr_it_drop(&it);

                        JAK_ASSERT(begin_off < end_off);
                        rm_nbytes += (end_off - begin_off);
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
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                        col_it it;

                        offset_t begin_off = memfile_tell(memfile);
                        col_it_create(&it, memfile, begin_off - sizeof(u8));
                        col_it_fast_forward(&it);
                        offset_t end_off = col_it_memfilepos(&it);

                        JAK_ASSERT(begin_off < end_off);
                        rm_nbytes += (end_off - begin_off);
                }
                        break;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP: {
                        obj_it it;

                        offset_t begin_off = memfile_tell(memfile);
                        internal_obj_it_create(&it, memfile, begin_off - sizeof(u8));
                        internal_obj_it_fast_forward(&it);
                        offset_t end_off = internal_obj_it_memfile_pos(&it);
                        obj_it_drop(&it);

                        JAK_ASSERT(begin_off < end_off);
                        rm_nbytes += (end_off - begin_off);
                }
                        break;
                default:
                        return error(ERR_INTERNALERR, NULL);
        }
        memfile_seek(memfile, start_off);
        memfile_inplace_remove(memfile, rm_nbytes);

        return true;
}

static void int_insert_array_array(insert *array_ins, json_array *array)
{
        arr_state state;
        insert *sub_ins = insert_array_begin(&state, array_ins,
                                                                      array->elements.elements.num_elems * 256);
        for (u32 i = 0; i < array->elements.elements.num_elems; i++) {
                const json_element *elem = VECTOR_GET(&array->elements.elements, i, json_element);
                int_carbon_from_json_elem(sub_ins, elem, false);
        }
        insert_array_end(&state);
}

static void int_insert_array_string(insert *array_ins, json_string *string)
{
        insert_string(array_ins, string->value);
}

static void int_insert_array_number(insert *array_ins, json_number *number)
{
        switch (number->value_type) {
                case JSON_NUMBER_FLOAT:
                        insert_float(array_ins, number->value.float_number);
                        break;
                case JSON_NUMBER_UNSIGNED:
                        insert_unsigned(array_ins, number->value.unsigned_integer);
                        break;
                case JSON_NUMBER_SIGNED:
                        insert_signed(array_ins, number->value.signed_integer);
                        break;
                default: error(ERR_UNSUPPORTEDTYPE, NULL);
        }
}

static void int_insert_array_true(insert *array_ins)
{
        insert_true(array_ins);
}

static void int_insert_array_false(insert *array_ins)
{
        insert_false(array_ins);
}

static void int_insert_array_null(insert *array_ins)
{
        insert_null(array_ins);
}

static void int_insert_array_elements(insert *array_ins, json_array *array)
{
        for (u32 i = 0; i < array->elements.elements.num_elems; i++) {
                json_element *elem = VECTOR_GET(&array->elements.elements, i, json_element);
                switch (elem->value.value_type) {
                        case JSON_VALUE_OBJECT: {
                                obj_state state;
                                insert *sub_obj = insert_object_begin(&state, array_ins,
                                                                                               elem->value.value.object->value->members.num_elems *
                                                                                               256);
                                int_insert_prop_object(sub_obj, elem->value.value.object);
                                insert_object_end(&state);
                        }
                                break;
                        case JSON_VALUE_ARRAY:
                                int_insert_array_array(array_ins, elem->value.value.array);
                                break;
                        case JSON_VALUE_STRING:
                                int_insert_array_string(array_ins, elem->value.value.string);
                                break;
                        case JSON_VALUE_NUMBER:
                                int_insert_array_number(array_ins, elem->value.value.number);
                                break;
                        case JSON_VALUE_TRUE:
                                int_insert_array_true(array_ins);
                                break;
                        case JSON_VALUE_FALSE:
                                int_insert_array_false(array_ins);
                                break;
                        case JSON_VALUE_NULL:
                                int_insert_array_null(array_ins);
                                break;
                        default: error(ERR_UNSUPPORTEDTYPE, NULL);
                                break;
                }
        }
}

#define insert_into_array(ins, elem, ctype, accessor)                                                                  \
{                                                                                                                      \
        for (u32 k = 0; k < elem->value.value.array->elements.elements.num_elems; k++) {                               \
                json_element *array_elem = VECTOR_GET(                                                             \
                        &elem->value.value.array->elements.elements, k, json_element);                          \
                if (array_elem->value.value_type == JSON_VALUE_NULL) {                                                 \
                        insert_null(ins);                                                                       \
                } else {                                                                                               \
                        insert_##ctype(ins, array_elem->value.value.number->value.accessor);                    \
                }                                                                                                      \
        }                                                                                                              \
}

#define insert_into_column(ins, elem, field_type, column_type, ctype, accessor)                                        \
({                                                                                                                     \
        col_state state;                                                                       \
        u64 approx_cap_nbytes = elem->value.value.array->elements.elements.num_elems *                                 \
                                internal_get_type_value_size(field_type);                                            \
        insert *cins = insert_column_begin(&state, ins,                                           \
                                                                column_type, approx_cap_nbytes);                       \
        for (u32 k = 0; k < elem->value.value.array->elements.elements.num_elems; k++) {                               \
                json_element *array_elem = VECTOR_GET(&elem->value.value.array->elements.elements,                 \
                                                          k, json_element);                                     \
                if (array_elem->value.value_type == JSON_VALUE_NULL) {                                                 \
                        insert_null(cins);                                                                      \
                } else {                                                                                               \
                        insert_##ctype(cins, (ctype) array_elem->value.value.number->value.accessor);           \
                }                                                                                                      \
        }                                                                                                              \
        insert_column_end(&state);                                                                              \
})

#define prop_insert_into_column(ins, prop, key, field_type, column_type, ctype, accessor)                                   \
({                                                                                                                     \
        col_state state;                                                                       \
        u64 approx_cap_nbytes = prop->value.value.value.array->elements.elements.num_elems *                                 \
                                internal_get_type_value_size(field_type);                                            \
        insert *cins = insert_prop_column_begin(&state, ins, key,                                          \
                                                                column_type, approx_cap_nbytes);                       \
        for (u32 k = 0; k < prop->value.value.value.array->elements.elements.num_elems; k++) {                               \
                json_element *array_elem = VECTOR_GET(&prop->value.value.value.array->elements.elements,                 \
                                                          k, json_element);                                     \
                if (array_elem->value.value_type == JSON_VALUE_NULL) {                                                 \
                        insert_null(cins);                                                                      \
                } else {                                                                                               \
                        insert_##ctype(cins, (ctype) array_elem->value.value.number->value.accessor);           \
                }                                                                                                      \
        }                                                                                                              \
        insert_prop_column_end(&state);                                                                              \
})

static void int_insert_prop_object(insert *oins, json_object *obj)
{
        for (u32 i = 0; i < obj->value->members.num_elems; i++) {
                json_prop *prop = VECTOR_GET(&obj->value->members, i, json_prop);
                switch (prop->value.value.value_type) {
                        case JSON_VALUE_OBJECT: {
                                obj_state state;
                                insert *sub_obj = insert_prop_object_begin(&state, oins,
                                                                                                    prop->key.value,
                                                                                                    prop->value.value.value.object->value->members.num_elems *
                                                                                                    256);
                                int_insert_prop_object(sub_obj, prop->value.value.value.object);
                                insert_prop_object_end(&state);
                        }
                                break;
                        case JSON_VALUE_ARRAY: {
                                json_list_type_e type;
                                json_array_get_type(&type, prop->value.value.value.array);
                                switch (type) {
                                        case JSON_LIST_EMPTY: {
                                                arr_state state;
                                                insert_prop_array_begin(&state, oins, prop->key.value, 0);
                                                insert_prop_array_end(&state);
                                        }
                                                break;
                                        case JSON_LIST_VARIABLE_OR_NESTED: {
                                                arr_state state;
                                                u64 approx_cap_nbytes =
                                                        prop->value.value.value.array->elements.elements.num_elems *
                                                        256;
                                                insert *array_ins = insert_prop_array_begin(
                                                        &state, oins,
                                                        prop->key.value, approx_cap_nbytes);
                                                int_insert_array_elements(array_ins, prop->value.value.value.array);
                                                insert_prop_array_end(&state);
                                        }
                                                break;
                                        case JSON_LIST_FIXED_U8:
                                                prop_insert_into_column(oins, prop, prop->key.value,
                                                                        FIELD_NUMBER_U8,
                                                                        COLUMN_U8,
                                                                        u8, unsigned_integer);
                                                break;
                                        case JSON_LIST_FIXED_U16:
                                                prop_insert_into_column(oins, prop, prop->key.value,
                                                                        FIELD_NUMBER_U16,
                                                                        COLUMN_U16,
                                                                        u16, unsigned_integer);
                                                break;
                                        case JSON_LIST_FIXED_U32:
                                                prop_insert_into_column(oins, prop, prop->key.value,
                                                                        FIELD_NUMBER_U32,
                                                                        COLUMN_U32,
                                                                        u32, unsigned_integer);
                                                break;
                                        case JSON_LIST_FIXED_U64:
                                                prop_insert_into_column(oins, prop, prop->key.value,
                                                                        FIELD_NUMBER_U64,
                                                                        COLUMN_U64,
                                                                        u64, unsigned_integer);
                                                break;
                                        case JSON_LIST_FIXED_I8:
                                                prop_insert_into_column(oins, prop, prop->key.value,
                                                                        FIELD_NUMBER_I8,
                                                                        COLUMN_I8,
                                                                        i8, signed_integer);
                                                break;
                                        case JSON_LIST_FIXED_I16:
                                                prop_insert_into_column(oins, prop, prop->key.value,
                                                                        FIELD_NUMBER_I16,
                                                                        COLUMN_I16,
                                                                        i16, signed_integer);
                                                break;
                                        case JSON_LIST_FIXED_I32:
                                                prop_insert_into_column(oins, prop, prop->key.value,
                                                                        FIELD_NUMBER_I32,
                                                                        COLUMN_I32,
                                                                        i32, signed_integer);
                                                break;
                                        case JSON_LIST_FIXED_I64:
                                                prop_insert_into_column(oins, prop, prop->key.value,
                                                                        FIELD_NUMBER_I64,
                                                                        COLUMN_I64,
                                                                        i64, signed_integer);
                                                break;
                                        case JSON_LIST_FIXED_FLOAT:
                                                prop_insert_into_column(oins, prop, prop->key.value,
                                                                        FIELD_NUMBER_FLOAT,
                                                                        COLUMN_FLOAT,
                                                                        float, float_number);
                                                break;
                                        case JSON_LIST_FIXED_NULL: {
                                                arr_state state;
                                                u64 approx_cap_nbytes = prop->value.value.value.array->elements.elements.num_elems;
                                                insert *array_ins = insert_prop_array_begin(
                                                        &state, oins, prop->key.value,
                                                        approx_cap_nbytes);
                                                for (u32 k = 0; k <
                                                                    prop->value.value.value.array->elements.elements.num_elems; k++) {
                                                        insert_null(array_ins);
                                                }
                                                insert_prop_array_end(&state);
                                        }
                                                break;
                                        case JSON_LIST_FIXED_BOOLEAN: {
                                                col_state state;
                                                u64 cap_nbytes = prop->value.value.value.array->elements.elements.num_elems;
                                                insert *array_ins = insert_prop_column_begin(
                                                        &state, oins,
                                                        prop->key.value,
                                                        COLUMN_BOOLEAN, cap_nbytes);
                                                for (u32 k = 0; k <
                                                                    prop->value.value.value.array->elements.elements.num_elems; k++) {
                                                        json_element *array_elem = VECTOR_GET(
                                                                &prop->value.value.value.array->elements.elements, k,
                                                                json_element);
                                                        if (array_elem->value.value_type == JSON_VALUE_TRUE) {
                                                                insert_true(array_ins);
                                                        } else if (array_elem->value.value_type == JSON_VALUE_FALSE) {
                                                                insert_false(array_ins);
                                                        } else if (array_elem->value.value_type == JSON_VALUE_NULL) {
                                                                insert_null(array_ins);
                                                        } else {
                                                                error(ERR_UNSUPPORTEDTYPE, NULL);
                                                        }
                                                }
                                                insert_prop_column_end(&state);
                                        }
                                                break;
                                        default: error(ERR_UNSUPPORTEDTYPE, NULL);
                                                break;
                                }
                        }
                                break;
                        case JSON_VALUE_STRING:
                                insert_prop_string(oins, prop->key.value, prop->value.value.value.string->value);
                                break;
                        case JSON_VALUE_NUMBER:
                                switch (prop->value.value.value.number->value_type) {
                                        case JSON_NUMBER_FLOAT:
                                                insert_prop_float(oins, prop->key.value,
                                                                         prop->value.value.value.number->value.float_number);
                                                break;
                                        case JSON_NUMBER_UNSIGNED:
                                                insert_prop_unsigned(oins, prop->key.value,
                                                                            prop->value.value.value.number->value.unsigned_integer);
                                                break;
                                        case JSON_NUMBER_SIGNED:
                                                insert_prop_signed(oins, prop->key.value,
                                                                          prop->value.value.value.number->value.signed_integer);
                                                break;
                                        default: error(ERR_UNSUPPORTEDTYPE, NULL);
                                                break;
                                }
                                break;
                        case JSON_VALUE_TRUE:
                                insert_prop_true(oins, prop->key.value);
                                break;
                        case JSON_VALUE_FALSE:
                                insert_prop_false(oins, prop->key.value);
                                break;
                        case JSON_VALUE_NULL:
                                insert_prop_null(oins, prop->key.value);
                                break;
                        default: error(ERR_UNSUPPORTEDTYPE, NULL);
                                break;
                }
        }
}

static void int_carbon_from_json_elem(insert *ins, const json_element *elem, bool is_root)
{
        switch (elem->value.value_type) {
                case JSON_VALUE_OBJECT: {
                        obj_state state;
                        insert *oins = insert_object_begin(&state, ins,
                                                                                    elem->value.value.object->value->members.num_elems *
                                                                                    256);
                        int_insert_prop_object(oins, elem->value.value.object);
                        insert_object_end(&state);
                }
                        break;
                case JSON_VALUE_ARRAY: {
                        json_list_type_e type;
                        json_array_get_type(&type, elem->value.value.array);
                        switch (type) {
                                case JSON_LIST_EMPTY: {
                                        if (is_root) {
                                                /** nothing to do */
                                        } else {
                                                arr_state state;
                                                insert_array_begin(&state, ins, 0);
                                                insert_array_end(&state);
                                        }
                                }
                                        break;
                                case JSON_LIST_VARIABLE_OR_NESTED: {
                                        u64 approx_cap_nbytes =
                                                elem->value.value.array->elements.elements.num_elems * 256;
                                        if (is_root) {
                                                int_insert_array_elements(ins, elem->value.value.array);
                                        } else {
                                                arr_state state;
                                                insert *array_ins = insert_array_begin(&state,
                                                                                                                ins,
                                                                                                                approx_cap_nbytes);
                                                int_insert_array_elements(array_ins, elem->value.value.array);
                                                insert_array_end(&state);
                                        }
                                }
                                        break;
                                case JSON_LIST_FIXED_U8:
                                        if (is_root) {
                                                insert_into_array(ins, elem, unsigned, unsigned_integer)
                                        } else {
                                                insert_into_column(ins, elem, FIELD_NUMBER_U8,
                                                                   COLUMN_U8,
                                                                   u8, unsigned_integer);
                                        }
                                        break;
                                case JSON_LIST_FIXED_U16:
                                        if (is_root) {
                                                insert_into_array(ins, elem, unsigned, unsigned_integer)
                                        } else {
                                                insert_into_column(ins, elem, FIELD_NUMBER_U16,
                                                                   COLUMN_U16,
                                                                   u16, unsigned_integer);
                                        }
                                        break;
                                case JSON_LIST_FIXED_U32:
                                        if (is_root) {
                                                insert_into_array(ins, elem, unsigned, unsigned_integer)
                                        } else {
                                                insert_into_column(ins, elem, FIELD_NUMBER_U32,
                                                                   COLUMN_U32,
                                                                   u32, unsigned_integer);
                                        }
                                        break;
                                case JSON_LIST_FIXED_U64:
                                        if (is_root) {
                                                insert_into_array(ins, elem, unsigned, unsigned_integer)
                                        } else {
                                                insert_into_column(ins, elem, FIELD_NUMBER_U64,
                                                                   COLUMN_U64,
                                                                   u64, unsigned_integer);
                                        }
                                        break;
                                case JSON_LIST_FIXED_I8:
                                        if (is_root) {
                                                insert_into_array(ins, elem, signed, signed_integer)
                                        } else {
                                                insert_into_column(ins, elem, FIELD_NUMBER_I8,
                                                                   COLUMN_I8,
                                                                   i8, signed_integer);
                                        }
                                        break;
                                case JSON_LIST_FIXED_I16:
                                        if (is_root) {
                                                insert_into_array(ins, elem, signed, signed_integer)
                                        } else {
                                                insert_into_column(ins, elem, FIELD_NUMBER_I16,
                                                                   COLUMN_I16,
                                                                   u16, signed_integer);
                                        }
                                        break;
                                case JSON_LIST_FIXED_I32:
                                        if (is_root) {
                                                insert_into_array(ins, elem, signed, signed_integer)
                                        } else {
                                                insert_into_column(ins, elem, FIELD_NUMBER_I32,
                                                                   COLUMN_I32,
                                                                   u32, signed_integer);
                                        }
                                        break;
                                case JSON_LIST_FIXED_I64:
                                        if (is_root) {
                                                insert_into_array(ins, elem, signed, signed_integer)
                                        } else {
                                                insert_into_column(ins, elem, FIELD_NUMBER_I64,
                                                                   COLUMN_I64,
                                                                   u64, signed_integer);
                                        }
                                        break;
                                case JSON_LIST_FIXED_FLOAT:
                                        if (is_root) {
                                                insert_into_array(ins, elem, float, float_number)
                                        } else {
                                                insert_into_column(ins, elem, FIELD_NUMBER_FLOAT,
                                                                   COLUMN_FLOAT,
                                                                   float, float_number);
                                        }
                                        break;
                                case JSON_LIST_FIXED_NULL: {
                                        u64 approx_cap_nbytes = elem->value.value.array->elements.elements.num_elems;
                                        if (is_root) {
                                                for (u32 i = 0;
                                                     i < elem->value.value.array->elements.elements.num_elems; i++) {
                                                        insert_null(ins);
                                                }
                                        } else {
                                                arr_state state;
                                                insert *array_ins = insert_array_begin(&state,
                                                                                                                ins,
                                                                                                                approx_cap_nbytes);
                                                for (u32 i = 0;
                                                     i < elem->value.value.array->elements.elements.num_elems; i++) {
                                                        insert_null(array_ins);
                                                }
                                                insert_array_end(&state);
                                        }
                                }
                                        break;
                                case JSON_LIST_FIXED_BOOLEAN: {
                                        if (is_root) {
                                                for (u32 i = 0;
                                                     i < elem->value.value.array->elements.elements.num_elems; i++) {
                                                        json_element *array_elem = VECTOR_GET(
                                                                &elem->value.value.array->elements.elements, i,
                                                                json_element);
                                                        if (array_elem->value.value_type == JSON_VALUE_TRUE) {
                                                                insert_true(ins);
                                                        } else if (array_elem->value.value_type == JSON_VALUE_FALSE) {
                                                                insert_false(ins);
                                                        } else if (array_elem->value.value_type == JSON_VALUE_NULL) {
                                                                insert_null(ins);
                                                        } else {
                                                                error(ERR_UNSUPPORTEDTYPE, NULL);
                                                        }
                                                }
                                        } else {
                                                col_state state;
                                                u64 cap_nbytes = elem->value.value.array->elements.elements.num_elems;
                                                insert *array_ins = insert_column_begin(&state,
                                                                                                                 ins,
                                                                                                                 COLUMN_BOOLEAN,
                                                                                                                 cap_nbytes);
                                                for (u32 i = 0;
                                                     i < elem->value.value.array->elements.elements.num_elems; i++) {
                                                        json_element *array_elem = VECTOR_GET(
                                                                &elem->value.value.array->elements.elements, i,
                                                                json_element);
                                                        if (array_elem->value.value_type == JSON_VALUE_TRUE) {
                                                                insert_true(array_ins);
                                                        } else if (array_elem->value.value_type == JSON_VALUE_FALSE) {
                                                                insert_false(array_ins);
                                                        } else if (array_elem->value.value_type == JSON_VALUE_NULL) {
                                                                insert_null(array_ins);
                                                        } else {
                                                                error(ERR_UNSUPPORTEDTYPE, NULL);
                                                        }
                                                }
                                                insert_column_end(&state);
                                        }
                                }
                                        break;
                                default: error(ERR_UNSUPPORTEDTYPE, NULL);
                                        break;
                        }
                }
                        break;
                case JSON_VALUE_STRING:
                        insert_string(ins, elem->value.value.string->value);
                        break;
                case JSON_VALUE_NUMBER:
                        switch (elem->value.value.number->value_type) {
                                case JSON_NUMBER_FLOAT:
                                        insert_float(ins, elem->value.value.number->value.float_number);
                                        break;
                                case JSON_NUMBER_UNSIGNED:
                                        insert_unsigned(ins, elem->value.value.number->value.unsigned_integer);
                                        break;
                                case JSON_NUMBER_SIGNED:
                                        insert_signed(ins, elem->value.value.number->value.signed_integer);
                                        break;
                                default: error(ERR_UNSUPPORTEDTYPE, NULL);
                                        break;
                        }
                        break;
                case JSON_VALUE_TRUE:
                        insert_true(ins);
                        break;
                case JSON_VALUE_FALSE:
                        insert_false(ins);
                        break;
                case JSON_VALUE_NULL:
                        insert_null(ins);
                        break;
                default: error(ERR_UNSUPPORTEDTYPE, NULL);
                        break;
        }
}

void internal_from_json(rec *doc, const json *data,
                          key_e key_type, const void *primary_key, int mode)
{
        UNUSED(data)
        UNUSED(primary_key)

        rec_new context;
        insert *ins = carbon_create_begin(&context, doc, key_type, mode);
        int_carbon_from_json_elem(ins, data->element, true);

        carbon_create_end(&context);
}

static void marker_insert(memfile *memfile, u8 marker)
{
        /** check whether marker can be written, otherwise make space for it */
        char c = *memfile_peek(memfile, sizeof(u8));
        if (c != 0) {
                memfile_inplace_insert(memfile, sizeof(u8));
        }
        memfile_write(memfile, &marker, sizeof(u8));
}

static bool array_is_slot_occupied(bool *is_empty_slot, bool *is_array_end, arr_it *it)
{
        internal_field_auto_close(&it->field);
        return is_slot_occupied(is_empty_slot, is_array_end, &it->file, MARRAY_END);
}

static bool object_it_is_slot_occupied(bool *is_empty_slot, bool *is_object_end, obj_it *it)
{
        internal_field_auto_close(&it->field.value.data);
        return is_slot_occupied(is_empty_slot, is_object_end, &it->file, MOBJECT_END);
}

static bool is_slot_occupied(bool *is_empty_slot, bool *is_end_reached, memfile *file, u8 end_marker)
{
        char c = *memfile_peek(file, 1);
        bool is_empty = c == 0, is_end = c == end_marker;
        OPTIONAL_SET(is_empty_slot, is_empty)
        OPTIONAL_SET(is_end_reached, is_end)
        if (!is_empty && !is_end) {
                return true;
        } else {
                return false;
        }
}

static bool object_it_next_no_load(bool *is_empty_slot, bool *is_array_end, obj_it *it)
{
        if (object_it_is_slot_occupied(is_empty_slot, is_array_end, it)) {
                internal_object_it_prop_skip(it);
                return true;
        } else {
                return false;
        }
}

static bool array_next_no_load(bool *is_empty_slot, bool *is_array_end, arr_it *it)
{
        if (array_is_slot_occupied(is_empty_slot, is_array_end, it)) {
                internal_array_field_read(it);
                carbon_field_skip(&it->file);
                return true;
        } else {
                return false;
        }
}