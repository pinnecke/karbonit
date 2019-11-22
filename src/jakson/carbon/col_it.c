/**
 * Columnar Binary JSON -- Copyright 2019 Marcus Pinnecke
 * This file implements an (read-/write) iterator for (JSON) arrays in carbon
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

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/carbon/col_it.h>
#include <jakson/carbon/arr_it.h>
#include <jakson/carbon/mime.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/internal.h>

#define safe_cast(builtin_type, nvalues, it, field_expr)                                                          \
({                                                                                                                     \
        field_e type;                                                                                  \
        const void *raw = col_it_values(&type, nvalues, it);                                             \
        error_if_and_return(!(field_expr), ERR_TYPEMISMATCH, NULL);                                              \
        (const builtin_type *) raw;                                                                                    \
})

bool col_it_create(col_it *it, memfile *memfile, offset_t begin)
{
        it->begin = begin;
        it->mod_size = 0;

        memfile_open(&it->file, memfile->memblock, memfile->mode);
        memfile_seek(&it->file, begin);

        error_if_and_return(memfile_remain_size(&it->file) < sizeof(u8) + sizeof(media_type), ERR_CORRUPTED, NULL);

        if (!abstract_is_instanceof_column(&it->file)) {
            return error(ERR_ILLEGALOP, "column begin marker or sub type expected");
        }

        abstract_type_class_e type_class;
        abstract_get_class(&type_class, &it->file);
        abstract_class_to_list_derivable(&it->list_type, type_class);

        u8 marker = *memfile_read(&it->file, sizeof(u8));

        field_e type = (field_e) marker;
        it->field_type = type;

        it->header_begin = memfile_tell(&it->file);
        it->num = (u32) memfile_read_uintvar_stream(NULL, &it->file);
        it->cap = (u32) memfile_read_uintvar_stream(NULL, &it->file);

        col_it_rewind(it);

        return true;
}

bool col_it_clone(col_it *dst, col_it *src)
{
        memfile_clone(&dst->file, &src->file);
        dst->header_begin = src->header_begin;
        dst->begin = src->begin;
        dst->field_type = src->field_type;
        dst->list_type = src->list_type;
        dst->mod_size = src->mod_size;
        dst->cap = src->cap;
        dst->num = src->num;
        return true;
}

bool col_it_insert(carbon_insert *inserter, col_it *it)
{
        return carbon_int_insert_create_for_column(inserter, it);
}

bool col_it_fast_forward(col_it *it)
{
        col_it_values(NULL, NULL, it);
        return true;
}

offset_t col_it_memfilepos(col_it *it)
{
        if (LIKELY(it != NULL)) {
                return memfile_tell(&it->file);
        } else {
                error(ERR_NULLPTR, NULL);
                return 0;
        }
}

offset_t col_it_tell(col_it *it, u32 elem_idx)
{
        if (it) {
                memfile_save_position(&it->file);
                memfile_seek(&it->file, it->header_begin);
                u32 num_elements = (u32) memfile_read_uintvar_stream(NULL, &it->file);
                memfile_read_uintvar_stream(NULL, &it->file);
                offset_t payload_start = memfile_tell(&it->file);
                error_if_and_return(elem_idx >= num_elements, ERR_OUTOFBOUNDS, NULL);
                offset_t ret = payload_start + elem_idx * carbon_int_get_type_value_size(it->field_type);
                memfile_restore_position(&it->file);
                return ret;
        } else {
                error(ERR_NULLPTR, NULL);
                return 0;
        }
}

bool col_it_values_info(field_e *type, u32 *nvalues, col_it *it)
{
        if (nvalues) {
                memfile_seek(&it->file, it->header_begin);
                u32 num_elements = (u32) memfile_read_uintvar_stream(NULL, &it->file);
                *nvalues = num_elements;
        }

        OPTIONAL_SET(type, it->field_type);

        return true;
}

bool col_it_value_is_null(col_it *it, u32 pos)
{
        field_e type;
        u32 nvalues = 0;
        col_it_values_info(&type, &nvalues, it);
        error_if_and_return(pos >= nvalues, ERR_OUTOFBOUNDS, NULL);
        switch (type) {
                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_SET:
                        return IS_NULL_U8(col_it_u8_values(NULL, it)[pos]);
                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_SET:
                        return IS_NULL_U16(col_it_u16_values(NULL, it)[pos]);
                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_SET:
                        return IS_NULL_U32(col_it_u32_values(NULL, it)[pos]);
                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_SET:
                        return IS_NULL_U64(col_it_u64_values(NULL, it)[pos]);
                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_SET:
                        return IS_NULL_I8(col_it_i8_values(NULL, it)[pos]);
                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_SET:
                        return IS_NULL_I16(col_it_i16_values(NULL, it)[pos]);
                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_SET:
                        return IS_NULL_I32(col_it_i32_values(NULL, it)[pos]);
                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET:
                        return IS_NULL_I64(col_it_i64_values(NULL, it)[pos]);
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                        return IS_NULL_FLOAT(col_it_float_values(NULL, it)[pos]);
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:
                        return IS_NULL_BOOLEAN(col_it_boolean_values(NULL, it)[pos]);
                default:
                        return error(ERR_UNSUPPCONTAINER, NULL);
        }
}

const void *col_it_values(field_e *type, u32 *nvalues, col_it *it)
{
        memfile_seek(&it->file, it->header_begin);
        u32 num_elements = (u32) memfile_read_uintvar_stream(NULL, &it->file);
        u32 cap_elements = (u32) memfile_read_uintvar_stream(NULL, &it->file);
        offset_t payload_start = memfile_tell(&it->file);

        const void *result = memfile_peek(&it->file, sizeof(void));

        OPTIONAL_SET(type, it->field_type);
        OPTIONAL_SET(nvalues, num_elements);

        u32 skip = cap_elements * carbon_int_get_type_value_size(it->field_type);
        memfile_seek(&it->file, payload_start + skip);

        return result;
}

const boolean *col_it_boolean_values(u32 *nvalues, col_it *it)
{
        return safe_cast(boolean, nvalues, it, field_is_column_bool_or_subtype(type));
}

const u8 *col_it_u8_values(u32 *nvalues, col_it *it)
{
        return safe_cast(u8, nvalues, it, field_is_column_u8_or_subtype(type));
}

const u16 *col_it_u16_values(u32 *nvalues, col_it *it)
{
        return safe_cast(u16, nvalues, it, field_is_column_u16_or_subtype(type));
}

const u32 *col_it_u32_values(u32 *nvalues, col_it *it)
{
        return safe_cast(u32, nvalues, it, field_is_column_u32_or_subtype(type));
}

const u64 *col_it_u64_values(u32 *nvalues, col_it *it)
{
        return safe_cast(u64, nvalues, it, field_is_column_u64_or_subtype(type));
}

const i8 *col_it_i8_values(u32 *nvalues, col_it *it)
{
        return safe_cast(i8, nvalues, it, field_is_column_i8_or_subtype(type));
}

const i16 *col_it_i16_values(u32 *nvalues, col_it *it)
{
        return safe_cast(i16, nvalues, it, field_is_column_i16_or_subtype(type));
}

const i32 *col_it_i32_values(u32 *nvalues, col_it *it)
{
        return safe_cast(i32, nvalues, it, field_is_column_i32_or_subtype(type));
}

const i64 *col_it_i64_values(u32 *nvalues, col_it *it)
{
        return safe_cast(i64, nvalues, it, field_is_column_i64_or_subtype(type));
}

const float *col_it_float_values(u32 *nvalues, col_it *it)
{
        return safe_cast(float, nvalues, it, field_is_column_float_or_subtype(type));
}

bool col_it_remove(col_it *it, u32 pos)
{
        error_if_and_return(pos >= it->num, ERR_OUTOFBOUNDS, NULL);
        memfile_save_position(&it->file);

        offset_t payload_start = carbon_int_column_get_payload_off(it);

        /** remove element */
        size_t elem_size = carbon_int_get_type_value_size(it->field_type);
        memfile_seek(&it->file, payload_start + pos * elem_size);
        memfile_inplace_remove(&it->file, elem_size);

        /** add an empty element at the end to restore the column capacity property */
        memfile_seek(&it->file, payload_start + it->num * elem_size);
        memfile_inplace_insert(&it->file, elem_size);

        /** update element counter */
        memfile_seek(&it->file, it->header_begin);
        u32 num_elems = memfile_peek_uintvar_stream(NULL, &it->file);
        JAK_ASSERT(num_elems > 0);
        num_elems--;
        signed_offset_t shift = memfile_update_uintvar_stream(&it->file, num_elems);
        it->num = num_elems;

        memfile_restore_position(&it->file);
        memfile_seek_from_here(&it->file, shift);

        return true;
}

bool col_it_is_multiset(col_it *it)
{
        abstract_type_class_e type_class;
        abstract_list_derivable_to_class(&type_class, it->list_type);
        return abstract_is_multiset(type_class);
}

bool col_it_is_sorted(col_it *it)
{
        abstract_type_class_e type_class;
        abstract_list_derivable_to_class(&type_class, it->list_type);
        return abstract_is_sorted(type_class);
}

bool col_it_update_type(col_it *it, list_type_e derivation)
{
        if (!field_is_column_or_subtype(it->field_type)) {
                return false;
        }

        memfile_save_position(&it->file);
        memfile_seek(&it->file, it->begin);

        derived_e derive_marker;
        list_container_e container;
        list_by_column_type(&container, it->field_type);
        abstract_derive_list_to(&derive_marker, container, derivation);
        abstract_write_derived_type(&it->file, derive_marker);

        memfile_restore_position(&it->file);

        return true;
}

bool col_it_update_set_null(col_it *it, u32 pos)
{
        error_if_and_return(pos >= it->num, ERR_OUTOFBOUNDS, NULL)

        memfile_save_position(&it->file);

        offset_t payload_start = carbon_int_column_get_payload_off(it);
        memfile_seek(&it->file, payload_start + pos * carbon_int_get_type_value_size(it->field_type));

        switch (it->field_type) {
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                        u8 null_value = CARBON_BOOLEAN_COLUMN_NULL;
                        memfile_write(&it->file, &null_value, sizeof(u8));
                }
                        break;
                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_SET: {
                        u8 null_value = U8_NULL;
                        memfile_write(&it->file, &null_value, sizeof(u8));
                }
                        break;
                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_SET: {
                        u16 null_value = U16_NULL;
                        memfile_write(&it->file, &null_value, sizeof(u16));
                }
                        break;
                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_SET: {
                        u32 null_value = U32_NULL;
                        memfile_write(&it->file, &null_value, sizeof(u32));
                }
                        break;
                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_SET: {
                        u64 null_value = U64_NULL;
                        memfile_write(&it->file, &null_value, sizeof(u64));
                }
                        break;
                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_SET: {
                        i8 null_value = I8_NULL;
                        memfile_write(&it->file, &null_value, sizeof(i8));
                }
                        break;
                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_SET: {
                        i16 null_value = I16_NULL;
                        memfile_write(&it->file, &null_value, sizeof(i16));
                }
                        break;
                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_SET: {
                        i32 null_value = I32_NULL;
                        memfile_write(&it->file, &null_value, sizeof(i32));
                }
                        break;
                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET: {
                        i64 null_value = I64_NULL;
                        memfile_write(&it->file, &null_value, sizeof(i64));
                }
                        break;
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET: {
                        float null_value = CARBON_NULL_FLOAT;
                        memfile_write(&it->file, &null_value, sizeof(float));
                }
                        break;
                case FIELD_NULL:
                case FIELD_TRUE:
                case FIELD_FALSE:
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP:
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET:
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
                        memfile_restore_position(&it->file);
                        return error(ERR_UNSUPPCONTAINER, NULL);
                default:
                        memfile_restore_position(&it->file);
                        error(ERR_INTERNALERR, NULL);
                        return false;
        }

        memfile_restore_position(&it->file);

        return true;
}

#define push_array_element(num_values, data, data_cast_type, null_check, insert_func)                                  \
for (u32 i = 0; i < num_values; i++) {                                                                                 \
        data_cast_type datum = ((data_cast_type *)data)[i];                                                            \
        if (LIKELY(null_check(datum) == false)) {                                                                      \
                insert_func(&array_ins);                                                                               \
        } else {                                                                                                       \
                carbon_insert_null(&array_ins);                                                                         \
        }                                                                                                              \
}

#define push_array_element_wvalue(num_values, data, data_cast_type, null_check, insert_func)                           \
for (u32 i = 0; i < num_values; i++) {                                                                                 \
        data_cast_type datum = ((data_cast_type *)data)[i];                                                            \
        if (LIKELY(null_check(datum) == false)) {                                                                      \
                insert_func(&array_ins, datum);                                                                        \
        } else {                                                                                                       \
                carbon_insert_null(&array_ins);                                                                         \
        }                                                                                                              \
}

static bool rewrite_column_to_array(col_it *it)
{
        abstract_type_class_e type_class;
        list_type_e list_type;
        arr_it array;
        carbon_insert array_ins;

        memfile_save_position(&it->file);
        assert(field_is_column_or_subtype(memfile_peek_byte(&it->file)));

        abstract_get_class(&type_class, &it->file);
        abstract_class_to_list_derivable(&list_type, type_class);

        /** Potentially tailing space after the last ']' marker of the outer most array is used for temporary space */
        memfile_seek_to_end(&it->file);
        offset_t array_marker_begin = memfile_tell(&it->file);

        size_t capacity = it->num * carbon_int_get_type_value_size(it->field_type);
        carbon_int_insert_array(&it->file, list_type, capacity);
        internal_arr_it_create(&array, &it->file, array_marker_begin);
        arr_it_insert_begin(&array_ins, &array);

        field_e type;
        u32 num_values;
        const void *data = col_it_values(&type, &num_values, it);
        switch (type) {
                case FIELD_NULL:
                        while (num_values--) {
                                carbon_insert_null(&array_ins);
                        }
                        break;
                case FIELD_TRUE:
                        push_array_element(num_values, data, u8, IS_NULL_BOOLEAN, carbon_insert_true);
                        break;
                case FIELD_FALSE:
                        push_array_element(num_values, data, u8, IS_NULL_BOOLEAN, carbon_insert_false);
                        break;
                case FIELD_NUMBER_U8:
                        push_array_element_wvalue(num_values, data, u8, IS_NULL_U8, carbon_insert_u8);
                        break;
                case FIELD_NUMBER_U16:
                        push_array_element_wvalue(num_values, data, u16, IS_NULL_U16, carbon_insert_u16);
                        break;
                case FIELD_NUMBER_U32:
                        push_array_element_wvalue(num_values, data, u32, IS_NULL_U32, carbon_insert_u32);
                        break;
                case FIELD_NUMBER_U64:
                        push_array_element_wvalue(num_values, data, u64, IS_NULL_U64, carbon_insert_u64);
                        break;
                case FIELD_NUMBER_I8:
                        push_array_element_wvalue(num_values, data, i8, IS_NULL_I8, carbon_insert_i8);
                        break;
                case FIELD_NUMBER_I16:
                        push_array_element_wvalue(num_values, data, i16, IS_NULL_I16, carbon_insert_i16);
                        break;
                case FIELD_NUMBER_I32:
                        push_array_element_wvalue(num_values, data, i32, IS_NULL_I32, carbon_insert_i32);
                        break;
                case FIELD_NUMBER_I64:
                        push_array_element_wvalue(num_values, data, i64, IS_NULL_I64, carbon_insert_i64);
                        break;
                case FIELD_NUMBER_FLOAT:
                        push_array_element_wvalue(num_values, data, float, IS_NULL_FLOAT, carbon_insert_float);
                        break;
                default: error(ERR_UNSUPPORTEDTYPE, NULL);
                        return false;
        }

        arr_it_insert_end(&array_ins);
        JAK_ASSERT(array_marker_begin < internal_arr_it_memfilepos(&array));
        arr_it_drop(&array);

        memfile_restore_position(&it->file);
        return true;
}

bool col_it_update_set_true(col_it *it, u32 pos)
{
        error_if_and_return(pos >= it->num, ERR_OUTOFBOUNDS, NULL)

        memfile_save_position(&it->file);

        offset_t payload_start = carbon_int_column_get_payload_off(it);
        memfile_seek(&it->file, payload_start + pos * carbon_int_get_type_value_size(it->field_type));

        switch (it->field_type) {
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                        u8 value = CARBON_BOOLEAN_COLUMN_TRUE;
                        memfile_write(&it->file, &value, sizeof(u8));
                }
                        break;
                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_SET: {
                        u8 null_value = U8_NULL;
                        memfile_write(&it->file, &null_value, sizeof(u8));
                }
                        break;
                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_SET: {
                        u16 null_value = U16_NULL;
                        memfile_write(&it->file, &null_value, sizeof(u16));
                }
                        break;
                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_SET: {
                        //u32 null_value = U32_NULL;
                        //memfile_write(&it->mem, &null_value, sizeof(u32));
                        rewrite_column_to_array(it);


                }
                        break;
                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_SET: {
                        u64 null_value = U64_NULL;
                        memfile_write(&it->file, &null_value, sizeof(u64));
                }
                        break;
                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_SET: {
                        i8 null_value = I8_NULL;
                        memfile_write(&it->file, &null_value, sizeof(i8));
                }
                        break;
                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_SET: {
                        i16 null_value = I16_NULL;
                        memfile_write(&it->file, &null_value, sizeof(i16));
                }
                        break;
                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_SET: {
                        i32 null_value = I32_NULL;
                        memfile_write(&it->file, &null_value, sizeof(i32));
                }
                        break;
                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET: {
                        i64 null_value = I64_NULL;
                        memfile_write(&it->file, &null_value, sizeof(i64));
                }
                        break;
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET: {
                        float null_value = CARBON_NULL_FLOAT;
                        memfile_write(&it->file, &null_value, sizeof(float));
                }
                        break;
                case FIELD_NULL:
                case FIELD_TRUE:
                case FIELD_FALSE:
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP:
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET:
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
                        memfile_restore_position(&it->file);
                        return error(ERR_UNSUPPCONTAINER, NULL);
                default:
                        memfile_restore_position(&it->file);
                        error(ERR_INTERNALERR, NULL);
                        return false;
        }

        memfile_restore_position(&it->file);

        return true;
}

bool col_it_update_set_false(col_it *it, u32 pos)
{
        UNUSED(it)
        UNUSED(pos)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_u8(col_it *it, u32 pos, u8 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_u16(col_it *it, u32 pos, u16 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_u32(col_it *it, u32 pos, u32 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_u64(col_it *it, u32 pos, u64 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_i8(col_it *it, u32 pos, i8 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_i16(col_it *it, u32 pos, i16 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_i32(col_it *it, u32 pos, i32 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_i64(col_it *it, u32 pos, i64 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_float(col_it *it, u32 pos, float value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_rewind(col_it *it)
{
        offset_t playload_start = carbon_int_column_get_payload_off(it);
        error_if_and_return(playload_start >= memfile_size(&it->file), ERR_OUTOFBOUNDS, NULL);
        return memfile_seek(&it->file, playload_start);
}