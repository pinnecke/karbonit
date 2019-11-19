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

#include <jakson/carbon/column.h>
#include <jakson/carbon/array.h>
#include <jakson/carbon/mime.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/internal.h>

#define safe_cast(builtin_type, nvalues, it, field_type_expr)                                                          \
({                                                                                                                     \
        carbon_field_type_e type;                                                                                  \
        const void *raw = carbon_column_values(&type, nvalues, it);                                             \
        error_if_and_return(!(field_type_expr), ERR_TYPEMISMATCH, NULL);                                              \
        (const builtin_type *) raw;                                                                                    \
})

bool carbon_column_create(carbon_column *it, memfile *memfile, offset_t column_start_offset)
{
        it->column_start_offset = column_start_offset;
        it->mod_size = 0;

        memfile_open(&it->memfile, memfile->memblock, memfile->mode);
        memfile_seek(&it->memfile, column_start_offset);

        error_if_and_return(memfile_remain_size(&it->memfile) < sizeof(u8) + sizeof(media_type), ERR_CORRUPTED, NULL);

        if (!abstract_is_instanceof_column(&it->memfile)) {
            return error(ERR_ILLEGALOP, "column begin marker or sub type expected");
        }

        abstract_type_class_e type_class;
        abstract_get_class(&type_class, &it->memfile);
        abstract_class_to_list_derivable(&it->abstract_type, type_class);

        u8 marker = *memfile_read(&it->memfile, sizeof(u8));

        carbon_field_type_e type = (carbon_field_type_e) marker;
        it->type = type;

        it->num_and_capacity_start_offset = memfile_tell(&it->memfile);
        it->column_num_elements = (u32) memfile_read_uintvar_stream(NULL, &it->memfile);
        it->column_capacity = (u32) memfile_read_uintvar_stream(NULL, &it->memfile);

        carbon_column_rewind(it);

        return true;
}

bool carbon_column_clone(carbon_column *dst, carbon_column *src)
{
        memfile_clone(&dst->memfile, &src->memfile);
        dst->num_and_capacity_start_offset = src->num_and_capacity_start_offset;
        dst->column_start_offset = src->column_start_offset;
        dst->type = src->type;
        dst->abstract_type = src->abstract_type;
        dst->mod_size = src->mod_size;
        dst->column_capacity = src->column_capacity;
        dst->column_num_elements = src->column_num_elements;
        return true;
}

bool carbon_column_insert(carbon_insert *inserter, carbon_column *it)
{
        return carbon_int_insert_create_for_column(inserter, it);
}

bool carbon_column_fast_forward(carbon_column *it)
{
        carbon_column_values(NULL, NULL, it);
        return true;
}

offset_t carbon_column_memfilepos(carbon_column *it)
{
        if (LIKELY(it != NULL)) {
                return memfile_tell(&it->memfile);
        } else {
                error(ERR_NULLPTR, NULL);
                return 0;
        }
}

offset_t carbon_column_tell(carbon_column *it, u32 elem_idx)
{
        if (it) {
                memfile_save_position(&it->memfile);
                memfile_seek(&it->memfile, it->num_and_capacity_start_offset);
                u32 num_elements = (u32) memfile_read_uintvar_stream(NULL, &it->memfile);
                memfile_read_uintvar_stream(NULL, &it->memfile);
                offset_t payload_start = memfile_tell(&it->memfile);
                error_if_and_return(elem_idx >= num_elements, ERR_OUTOFBOUNDS, NULL);
                offset_t ret = payload_start + elem_idx * carbon_int_get_type_value_size(it->type);
                memfile_restore_position(&it->memfile);
                return ret;
        } else {
                error(ERR_NULLPTR, NULL);
                return 0;
        }
}

bool carbon_column_values_info(carbon_field_type_e *type, u32 *nvalues, carbon_column *it)
{
        if (nvalues) {
                memfile_seek(&it->memfile, it->num_and_capacity_start_offset);
                u32 num_elements = (u32) memfile_read_uintvar_stream(NULL, &it->memfile);
                *nvalues = num_elements;
        }

        OPTIONAL_SET(type, it->type);

        return true;
}

bool carbon_column_value_is_null(carbon_column *it, u32 pos)
{
        carbon_field_type_e type;
        u32 nvalues = 0;
        carbon_column_values_info(&type, &nvalues, it);
        error_if_and_return(pos >= nvalues, ERR_OUTOFBOUNDS, NULL);
        switch (type) {
                case CARBON_FIELD_COLUMN_U8_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_SET:
                        return IS_NULL_U8(carbon_column_u8_values(NULL, it)[pos]);
                case CARBON_FIELD_COLUMN_U16_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_SET:
                        return IS_NULL_U16(carbon_column_u16_values(NULL, it)[pos]);
                case CARBON_FIELD_COLUMN_U32_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_SET:
                        return IS_NULL_U32(carbon_column_u32_values(NULL, it)[pos]);
                case CARBON_FIELD_COLUMN_U64_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_SET:
                        return IS_NULL_U64(carbon_column_u64_values(NULL, it)[pos]);
                case CARBON_FIELD_COLUMN_I8_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_SET:
                        return IS_NULL_I8(carbon_column_i8_values(NULL, it)[pos]);
                case CARBON_FIELD_COLUMN_I16_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_SET:
                        return IS_NULL_I16(carbon_column_i16_values(NULL, it)[pos]);
                case CARBON_FIELD_COLUMN_I32_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_SET:
                        return IS_NULL_I32(carbon_column_i32_values(NULL, it)[pos]);
                case CARBON_FIELD_COLUMN_I64_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_SET:
                        return IS_NULL_I64(carbon_column_i64_values(NULL, it)[pos]);
                case CARBON_FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                        return IS_NULL_FLOAT(carbon_column_float_values(NULL, it)[pos]);
                case CARBON_FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:
                        return IS_NULL_BOOLEAN(carbon_column_boolean_values(NULL, it)[pos]);
                default:
                        return error(ERR_UNSUPPCONTAINER, NULL);
        }
}

const void *carbon_column_values(carbon_field_type_e *type, u32 *nvalues, carbon_column *it)
{
        memfile_seek(&it->memfile, it->num_and_capacity_start_offset);
        u32 num_elements = (u32) memfile_read_uintvar_stream(NULL, &it->memfile);
        u32 cap_elements = (u32) memfile_read_uintvar_stream(NULL, &it->memfile);
        offset_t payload_start = memfile_tell(&it->memfile);

        const void *result = memfile_peek(&it->memfile, sizeof(void));

        OPTIONAL_SET(type, it->type);
        OPTIONAL_SET(nvalues, num_elements);

        u32 skip = cap_elements * carbon_int_get_type_value_size(it->type);
        memfile_seek(&it->memfile, payload_start + skip);

        return result;
}

const boolean *carbon_column_boolean_values(u32 *nvalues, carbon_column *it)
{
        return safe_cast(boolean, nvalues, it, carbon_field_type_is_column_bool_or_subtype(type));
}

const u8 *carbon_column_u8_values(u32 *nvalues, carbon_column *it)
{
        return safe_cast(u8, nvalues, it, carbon_field_type_is_column_u8_or_subtype(type));
}

const u16 *carbon_column_u16_values(u32 *nvalues, carbon_column *it)
{
        return safe_cast(u16, nvalues, it, carbon_field_type_is_column_u16_or_subtype(type));
}

const u32 *carbon_column_u32_values(u32 *nvalues, carbon_column *it)
{
        return safe_cast(u32, nvalues, it, carbon_field_type_is_column_u32_or_subtype(type));
}

const u64 *carbon_column_u64_values(u32 *nvalues, carbon_column *it)
{
        return safe_cast(u64, nvalues, it, carbon_field_type_is_column_u64_or_subtype(type));
}

const i8 *carbon_column_i8_values(u32 *nvalues, carbon_column *it)
{
        return safe_cast(i8, nvalues, it, carbon_field_type_is_column_i8_or_subtype(type));
}

const i16 *carbon_column_i16_values(u32 *nvalues, carbon_column *it)
{
        return safe_cast(i16, nvalues, it, carbon_field_type_is_column_i16_or_subtype(type));
}

const i32 *carbon_column_i32_values(u32 *nvalues, carbon_column *it)
{
        return safe_cast(i32, nvalues, it, carbon_field_type_is_column_i32_or_subtype(type));
}

const i64 *carbon_column_i64_values(u32 *nvalues, carbon_column *it)
{
        return safe_cast(i64, nvalues, it, carbon_field_type_is_column_i64_or_subtype(type));
}

const float *carbon_column_float_values(u32 *nvalues, carbon_column *it)
{
        return safe_cast(float, nvalues, it, carbon_field_type_is_column_float_or_subtype(type));
}

bool carbon_column_remove(carbon_column *it, u32 pos)
{
        error_if_and_return(pos >= it->column_num_elements, ERR_OUTOFBOUNDS, NULL);
        memfile_save_position(&it->memfile);

        offset_t payload_start = carbon_int_column_get_payload_off(it);

        /** remove element */
        size_t elem_size = carbon_int_get_type_value_size(it->type);
        memfile_seek(&it->memfile, payload_start + pos * elem_size);
        memfile_inplace_remove(&it->memfile, elem_size);

        /** add an empty element at the end to restore the column capacity property */
        memfile_seek(&it->memfile, payload_start + it->column_num_elements * elem_size);
        memfile_inplace_insert(&it->memfile, elem_size);

        /** update element counter */
        memfile_seek(&it->memfile, it->num_and_capacity_start_offset);
        u32 num_elems = memfile_peek_uintvar_stream(NULL, &it->memfile);
        JAK_ASSERT(num_elems > 0);
        num_elems--;
        signed_offset_t shift = memfile_update_uintvar_stream(&it->memfile, num_elems);
        it->column_num_elements = num_elems;

        memfile_restore_position(&it->memfile);
        memfile_seek_from_here(&it->memfile, shift);

        return true;
}

bool carbon_column_is_multiset(carbon_column *it)
{
        abstract_type_class_e type_class;
        abstract_list_derivable_to_class(&type_class, it->abstract_type);
        return abstract_is_multiset(type_class);
}

bool carbon_column_is_sorted(carbon_column *it)
{
        abstract_type_class_e type_class;
        abstract_list_derivable_to_class(&type_class, it->abstract_type);
        return abstract_is_sorted(type_class);
}

bool carbon_column_update_type(carbon_column *it, list_derivable_e derivation)
{
        if (!carbon_field_type_is_column_or_subtype(it->type)) {
                return false;
        }

        memfile_save_position(&it->memfile);
        memfile_seek(&it->memfile, it->column_start_offset);

        derived_e derive_marker;
        carbon_list_container_e container_type;
        carbon_list_container_type_by_column_type(&container_type, it->type);
        abstract_derive_list_to(&derive_marker, container_type, derivation);
        abstract_write_derived_type(&it->memfile, derive_marker);

        memfile_restore_position(&it->memfile);

        return true;
}

bool carbon_column_update_set_null(carbon_column *it, u32 pos)
{
        error_if_and_return(pos >= it->column_num_elements, ERR_OUTOFBOUNDS, NULL)

        memfile_save_position(&it->memfile);

        offset_t payload_start = carbon_int_column_get_payload_off(it);
        memfile_seek(&it->memfile, payload_start + pos * carbon_int_get_type_value_size(it->type));

        switch (it->type) {
                case CARBON_FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                        u8 null_value = CARBON_BOOLEAN_COLUMN_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(u8));
                }
                        break;
                case CARBON_FIELD_COLUMN_U8_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_SET: {
                        u8 null_value = U8_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(u8));
                }
                        break;
                case CARBON_FIELD_COLUMN_U16_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_SET: {
                        u16 null_value = U16_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(u16));
                }
                        break;
                case CARBON_FIELD_COLUMN_U32_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_SET: {
                        u32 null_value = U32_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(u32));
                }
                        break;
                case CARBON_FIELD_COLUMN_U64_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_SET: {
                        u64 null_value = U64_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(u64));
                }
                        break;
                case CARBON_FIELD_COLUMN_I8_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_SET: {
                        i8 null_value = I8_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(i8));
                }
                        break;
                case CARBON_FIELD_COLUMN_I16_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_SET: {
                        i16 null_value = I16_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(i16));
                }
                        break;
                case CARBON_FIELD_COLUMN_I32_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_SET: {
                        i32 null_value = I32_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(i32));
                }
                        break;
                case CARBON_FIELD_COLUMN_I64_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_SET: {
                        i64 null_value = I64_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(i64));
                }
                        break;
                case CARBON_FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET: {
                        float null_value = CARBON_NULL_FLOAT;
                        memfile_write(&it->memfile, &null_value, sizeof(float));
                }
                        break;
                case CARBON_FIELD_NULL:
                case CARBON_FIELD_TRUE:
                case CARBON_FIELD_FALSE:
                case CARBON_FIELD_OBJECT_UNSORTED_MULTIMAP:
                case CARBON_FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case CARBON_FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case CARBON_FIELD_DERIVED_OBJECT_SORTED_MAP:
                case CARBON_FIELD_ARRAY_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_ARRAY_SORTED_SET:
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
                        memfile_restore_position(&it->memfile);
                        return error(ERR_UNSUPPCONTAINER, NULL);
                default:
                        memfile_restore_position(&it->memfile);
                        error(ERR_INTERNALERR, NULL);
                        return false;
        }

        memfile_restore_position(&it->memfile);

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

static bool rewrite_column_to_array(carbon_column *it)
{
        abstract_type_class_e type_class;
        list_derivable_e list_type;
        carbon_array array;
        carbon_insert array_ins;

        memfile_save_position(&it->memfile);
        assert(carbon_field_type_is_column_or_subtype(memfile_peek_byte(&it->memfile)));

        abstract_get_class(&type_class, &it->memfile);
        abstract_class_to_list_derivable(&list_type, type_class);

        /** Potentially tailing space after the last ']' marker of the outer most array is used for temporary space */
        memfile_seek_to_end(&it->memfile);
        offset_t array_marker_begin = memfile_tell(&it->memfile);

        size_t capacity = it->column_num_elements * carbon_int_get_type_value_size(it->type);
        carbon_int_insert_array(&it->memfile, list_type, capacity);
        internal_carbon_array_create(&array, &it->memfile, array_marker_begin);
        carbon_array_insert_begin(&array_ins, &array);

        carbon_field_type_e type;
        u32 num_values;
        const void *data = carbon_column_values(&type, &num_values, it);
        switch (type) {
                case CARBON_FIELD_NULL:
                        while (num_values--) {
                                carbon_insert_null(&array_ins);
                        }
                        break;
                case CARBON_FIELD_TRUE:
                        push_array_element(num_values, data, u8, IS_NULL_BOOLEAN, carbon_insert_true);
                        break;
                case CARBON_FIELD_FALSE:
                        push_array_element(num_values, data, u8, IS_NULL_BOOLEAN, carbon_insert_false);
                        break;
                case CARBON_FIELD_NUMBER_U8:
                        push_array_element_wvalue(num_values, data, u8, IS_NULL_U8, carbon_insert_u8);
                        break;
                case CARBON_FIELD_NUMBER_U16:
                        push_array_element_wvalue(num_values, data, u16, IS_NULL_U16, carbon_insert_u16);
                        break;
                case CARBON_FIELD_NUMBER_U32:
                        push_array_element_wvalue(num_values, data, u32, IS_NULL_U32, carbon_insert_u32);
                        break;
                case CARBON_FIELD_NUMBER_U64:
                        push_array_element_wvalue(num_values, data, u64, IS_NULL_U64, carbon_insert_u64);
                        break;
                case CARBON_FIELD_NUMBER_I8:
                        push_array_element_wvalue(num_values, data, i8, IS_NULL_I8, carbon_insert_i8);
                        break;
                case CARBON_FIELD_NUMBER_I16:
                        push_array_element_wvalue(num_values, data, i16, IS_NULL_I16, carbon_insert_i16);
                        break;
                case CARBON_FIELD_NUMBER_I32:
                        push_array_element_wvalue(num_values, data, i32, IS_NULL_I32, carbon_insert_i32);
                        break;
                case CARBON_FIELD_NUMBER_I64:
                        push_array_element_wvalue(num_values, data, i64, IS_NULL_I64, carbon_insert_i64);
                        break;
                case CARBON_FIELD_NUMBER_FLOAT:
                        push_array_element_wvalue(num_values, data, float, IS_NULL_FLOAT, carbon_insert_float);
                        break;
                default: error(ERR_UNSUPPORTEDTYPE, NULL);
                        return false;
        }

        carbon_array_insert_end(&array_ins);
        JAK_ASSERT(array_marker_begin < internal_carbon_array_memfilepos(&array));
        carbon_array_drop(&array);

        memfile_restore_position(&it->memfile);
        return true;
}

bool carbon_column_update_set_true(carbon_column *it, u32 pos)
{
        error_if_and_return(pos >= it->column_num_elements, ERR_OUTOFBOUNDS, NULL)

        memfile_save_position(&it->memfile);

        offset_t payload_start = carbon_int_column_get_payload_off(it);
        memfile_seek(&it->memfile, payload_start + pos * carbon_int_get_type_value_size(it->type));

        switch (it->type) {
                case CARBON_FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                        u8 value = CARBON_BOOLEAN_COLUMN_TRUE;
                        memfile_write(&it->memfile, &value, sizeof(u8));
                }
                        break;
                case CARBON_FIELD_COLUMN_U8_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_SET: {
                        u8 null_value = U8_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(u8));
                }
                        break;
                case CARBON_FIELD_COLUMN_U16_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_SET: {
                        u16 null_value = U16_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(u16));
                }
                        break;
                case CARBON_FIELD_COLUMN_U32_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_SET: {
                        //u32 null_value = U32_NULL;
                        //memfile_write(&it->mem, &null_value, sizeof(u32));
                        rewrite_column_to_array(it);


                }
                        break;
                case CARBON_FIELD_COLUMN_U64_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_SET: {
                        u64 null_value = U64_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(u64));
                }
                        break;
                case CARBON_FIELD_COLUMN_I8_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_SET: {
                        i8 null_value = I8_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(i8));
                }
                        break;
                case CARBON_FIELD_COLUMN_I16_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_SET: {
                        i16 null_value = I16_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(i16));
                }
                        break;
                case CARBON_FIELD_COLUMN_I32_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_SET: {
                        i32 null_value = I32_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(i32));
                }
                        break;
                case CARBON_FIELD_COLUMN_I64_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_SET: {
                        i64 null_value = I64_NULL;
                        memfile_write(&it->memfile, &null_value, sizeof(i64));
                }
                        break;
                case CARBON_FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET: {
                        float null_value = CARBON_NULL_FLOAT;
                        memfile_write(&it->memfile, &null_value, sizeof(float));
                }
                        break;
                case CARBON_FIELD_NULL:
                case CARBON_FIELD_TRUE:
                case CARBON_FIELD_FALSE:
                case CARBON_FIELD_OBJECT_UNSORTED_MULTIMAP:
                case CARBON_FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case CARBON_FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case CARBON_FIELD_DERIVED_OBJECT_SORTED_MAP:
                case CARBON_FIELD_ARRAY_UNSORTED_MULTISET:
                case CARBON_FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case CARBON_FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case CARBON_FIELD_DERIVED_ARRAY_SORTED_SET:
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
                        memfile_restore_position(&it->memfile);
                        return error(ERR_UNSUPPCONTAINER, NULL);
                default:
                        memfile_restore_position(&it->memfile);
                        error(ERR_INTERNALERR, NULL);
                        return false;
        }

        memfile_restore_position(&it->memfile);

        return true;
}

bool carbon_column_update_set_false(carbon_column *it, u32 pos)
{
        UNUSED(it)
        UNUSED(pos)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool carbon_column_update_set_u8(carbon_column *it, u32 pos, u8 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool carbon_column_update_set_u16(carbon_column *it, u32 pos, u16 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool carbon_column_update_set_u32(carbon_column *it, u32 pos, u32 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool carbon_column_update_set_u64(carbon_column *it, u32 pos, u64 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool carbon_column_update_set_i8(carbon_column *it, u32 pos, i8 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool carbon_column_update_set_i16(carbon_column *it, u32 pos, i16 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool carbon_column_update_set_i32(carbon_column *it, u32 pos, i32 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool carbon_column_update_set_i64(carbon_column *it, u32 pos, i64 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool carbon_column_update_set_float(carbon_column *it, u32 pos, float value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        error(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool carbon_column_rewind(carbon_column *it)
{
        offset_t playload_start = carbon_int_column_get_payload_off(it);
        error_if_and_return(playload_start >= memfile_size(&it->memfile), ERR_OUTOFBOUNDS, NULL);
        return memfile_seek(&it->memfile, playload_start);
}