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

#include <karbonit/carbon/col-it.h>
#include <karbonit/carbon/arr-it.h>
#include <karbonit/carbon/mime.h>
#include <karbonit/carbon/insert.h>
#include <karbonit/carbon/internal.h>

bool col_it_create(col_it *it, memfile *memfile, offset_t begin)
{
        it->begin = begin;
        it->mod_size = 0;

        MEMFILE_OPEN(&it->file, memfile->memblock, memfile->mode);
        MEMFILE_SEEK(&it->file, begin);

#ifndef NDEBUG
        ERROR_IF_AND_RETURN(MEMFILE_REMAIN_SIZE(&it->file) < sizeof(u8) + sizeof(media_type), ERR_CORRUPTED, NULL);
#endif

        u8 marker = *MEMFILE_READ(&it->file, sizeof(u8));

        if (!abstract_is_instanceof_column(marker)) {
            return ERROR(ERR_ILLEGALOP, "column begin marker or sub type expected");
        }

        abstract_type_class_e type_class = abstract_get_class(marker);
        it->list_type = abstract_class_to_list_derivable(type_class);


        field_e type = (field_e) marker;
        it->field_type = type;

        it->header_begin = MEMFILE_TELL(&it->file);
        it->num = (u32) MEMFILE_READ_UINTVAR_STREAM(NULL, &it->file);
        it->cap = (u32) MEMFILE_READ_UINTVAR_STREAM(NULL, &it->file);

        col_it_rewind(it);

        return true;
}

void internal_col_it_skip__fast(col_it *col)
{
        u32 skip = col->cap * INTERNAL_GET_TYPE_VALUE_SIZE(col->field_type);
        MEMFILE_SEEK__UNSAFE(&col->file, col->header_begin +
                                       UINTVAR_STREAM_REQUIRED_BLOCKS(col->num) +
                                       UINTVAR_STREAM_REQUIRED_BLOCKS(col->cap) + skip);
}

bool col_it_clone(col_it *dst, col_it *src)
{
        MEMFILE_CLONE(&dst->file, &src->file);
        dst->header_begin = src->header_begin;
        dst->begin = src->begin;
        dst->field_type = src->field_type;
        dst->list_type = src->list_type;
        dst->mod_size = src->mod_size;
        dst->cap = src->cap;
        dst->num = src->num;
        return true;
}

bool col_it_insert(insert *in, col_it *it)
{
        return internal_insert_create_for_column(in, it);
}

bool col_it_fast_forward(col_it *it)
{
        COL_IT_VALUES(NULL, NULL, it);
        return true;
}

offset_t col_it_memfilepos(col_it *it)
{
        if (LIKELY(it != NULL)) {
                return MEMFILE_TELL(&it->file);
        } else {
                ERROR(ERR_NULLPTR, NULL);
                return 0;
        }
}

void *col_it_memfile_raw(col_it *it)
{
        return MEMFILE_RAW_DATA(&it->file);
}

offset_t col_it_tell(col_it *it, u32 elem_idx)
{
        if (it) {
                MEMFILE_SAVE_POSITION(&it->file);
                MEMFILE_SEEK(&it->file, it->header_begin);
                u32 num_elements = (u32) MEMFILE_READ_UINTVAR_STREAM(NULL, &it->file);
                MEMFILE_READ_UINTVAR_STREAM(NULL, &it->file);
                offset_t payload_start = MEMFILE_TELL(&it->file);
                ERROR_IF_AND_RETURN(elem_idx >= num_elements, ERR_OUTOFBOUNDS, NULL);
                offset_t ret = payload_start + elem_idx * INTERNAL_GET_TYPE_VALUE_SIZE(it->field_type);
                MEMFILE_RESTORE_POSITION(&it->file);
                return ret;
        } else {
                ERROR(ERR_NULLPTR, NULL);
                return 0;
        }
}

bool col_it_is_null(col_it *it, u32 pos)
{
        field_e type;
        u32 nvalues = COL_IT_VALUES_INFO(&type, it);
        ERROR_IF_AND_RETURN(pos >= nvalues, ERR_OUTOFBOUNDS, NULL);
        switch (type) {
                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_SET: {
                        const u8 *values = COL_IT_U8_VALUES(NULL, it);
                        return IS_NULL_U8(values[pos]);
                }
                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_SET: {
                        const u16 *values = COL_IT_U16_VALUES(NULL, it);
                        return IS_NULL_U16(values[pos]);
                }
                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_SET: {
                        const u32 *values = COL_IT_U32_VALUES(NULL, it);
                        return IS_NULL_U32(values[pos]);
                }
                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_SET: {
                        const u64 *values = COL_IT_U64_VALUES(NULL, it);
                        return IS_NULL_U64(values[pos]);
                }
                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_SET: {
                        const i8 *values = COL_IT_I8_VALUES(NULL, it);
                        return IS_NULL_I8(values[pos]);
                }
                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_SET: {
                        const i16 *values = COL_IT_I16_VALUES(NULL, it);
                        return IS_NULL_I16(values[pos]);
                }
                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_SET: {
                        const i32 *values = COL_IT_I32_VALUES(NULL, it);
                        return IS_NULL_I32(values[pos]);
                }
                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET: {
                        const i64 *values = COL_IT_I64_VALUES(NULL, it);
                        return IS_NULL_I64(values[pos]);
                }
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET: {
                        const float *values = COL_IT_FLOAT_VALUES(NULL, it);
                        return IS_NULL_FLOAT(values[pos]);
                }
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                        const boolean *values = COL_IT_BOOLEAN_VALUES(NULL, it);
                        return IS_NULL_BOOLEAN(values[pos]);
                }
                default:
                        return ERROR(ERR_UNSUPPCONTAINER, NULL);
        }
}

bool col_it_is_boolean(col_it *it)
{
        switch (it->field_type) {
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:
                        return true;
                default:
                        return false;
        }
}

bool col_it_is_u8(col_it *it)
{
        switch (it->field_type) {
                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_SET:
                        return true;
                default:
                        return false;
        }
}

bool col_it_is_u16(col_it *it)
{
        switch (it->field_type) {
                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_SET:
                        return true;
                default:
                        return false;
        }
}

bool col_it_is_u32(col_it *it)
{
        switch (it->field_type) {
                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_SET:
                        return true;
                default:
                        return false;
        }
}

bool col_it_is_u64(col_it *it)
{
        {
                switch (it->field_type) {
                        case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_U64_SORTED_SET:
                                return true;
                        default:
                                return false;
                }
        }
}

bool col_it_is_i8(col_it *it)
{
        switch (it->field_type) {
                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_SET:
                        return true;
                default:
                        return false;
        }
}

bool col_it_is_i16(col_it *it)
{
        switch (it->field_type) {
                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_SET:
                        return true;
                default:
                        return false;
        }
}

bool col_it_is_i32(col_it *it)
{
        switch (it->field_type) {
                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_SET:
                        return true;
                default:
                        return false;
        }
}

bool col_it_is_i64(col_it *it)
{
        switch (it->field_type) {
                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET:
                        return true;
                default:
                        return false;
        }
}

bool col_it_is_float(col_it *it)
{
        switch (it->field_type) {
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                        return true;
                default:
                        return false;
        }
}

const boolean *internal_col_it_boolean_values(u32 *nvalues, col_it *it)
{
        return COL_IT_BOOLEAN_VALUES(nvalues, it);
}

const u8 *internal_col_it_u8_values(u32 *nvalues, col_it *it)
{
        return COL_IT_U8_VALUES(nvalues, it);
}

const u16 *internal_col_it_u16_values(u32 *nvalues, col_it *it)
{
        return COL_IT_U16_VALUES(nvalues, it);
}

const u32 *internal_col_it_u32_values(u32 *nvalues, col_it *it)
{
        return COL_IT_U32_VALUES(nvalues, it);
}

const u64 *internal_col_it_u64_values(u32 *nvalues, col_it *it)
{
        return COL_IT_U64_VALUES(nvalues, it);
}

const i8 *internal_col_it_i8_values(u32 *nvalues, col_it *it)
{
        return COL_IT_I8_VALUES(nvalues, it);
}

const i16 *internal_col_it_i16_values(u32 *nvalues, col_it *it)
{
        return COL_IT_I16_VALUES(nvalues, it);
}

const i32 *internal_col_it_i32_values(u32 *nvalues, col_it *it)
{
        return COL_IT_I32_VALUES(nvalues, it);
}

const i64 *internal_col_it_i64_values(u32 *nvalues, col_it *it)
{
        return COL_IT_I64_VALUES(nvalues, it);
}

const float *internal_col_it_float_values(u32 *nvalues, col_it *it)
{
        return COL_IT_FLOAT_VALUES(nvalues, it);
}

bool col_it_remove(col_it *it, u32 pos)
{
        ERROR_IF_AND_RETURN(pos >= it->num, ERR_OUTOFBOUNDS, NULL);
        MEMFILE_SAVE_POSITION(&it->file);

        offset_t payload_start = internal_column_get_payload_off(it);

        /** remove element */
        size_t elem_size = INTERNAL_GET_TYPE_VALUE_SIZE(it->field_type);
        MEMFILE_SEEK(&it->file, payload_start + pos * elem_size);
        MEMFILE_INPLACE_REMOVE(&it->file, elem_size);

        /** add an empty element at the end to restore the column capacity property */
        MEMFILE_SEEK(&it->file, payload_start + it->num * elem_size);
        MEMFILE_INPLACE_INSERT(&it->file, elem_size);

        /** update element counter */
        MEMFILE_SEEK(&it->file, it->header_begin);
        u32 num_elems = MEMFILE_PEEK_UINTVAR_STREAM(NULL, &it->file);
        assert(num_elems > 0);
        num_elems--;
        signed_offset_t shift = MEMFILE_UPDATE_UINTVAR_STREAM(&it->file, num_elems);
        it->num = num_elems;

        MEMFILE_RESTORE_POSITION(&it->file);
        MEMFILE_SEEK_FROM_HERE(&it->file, shift);

        return true;
}

bool col_it_is_multiset(col_it *it)
{
        abstract_type_class_e type_class = abstract_list_derivable_to_class(it->list_type);
        return abstract_is_multiset(type_class);
}

bool col_it_is_sorted(col_it *it)
{
        abstract_type_class_e type_class = abstract_list_derivable_to_class(it->list_type);
        return abstract_is_sorted(type_class);
}

bool col_it_update_type(col_it *it, list_type_e derivation)
{
        if (!FIELD_IS_COLUMN_OR_SUBTYPE(it->field_type)) {
                return false;
        }

        MEMFILE_SAVE_POSITION(&it->file);
        MEMFILE_SEEK(&it->file, it->begin);

        list_container_e container;
        list_by_column_type(&container, it->field_type);
        derived_e derive_marker = abstract_derive_list_to(container, derivation);
        abstract_write_derived_type(&it->file, derive_marker);

        MEMFILE_RESTORE_POSITION(&it->file);

        return true;
}

bool col_it_update_set_null(col_it *it, u32 pos)
{
        ERROR_IF_AND_RETURN(pos >= it->num, ERR_OUTOFBOUNDS, NULL)

        MEMFILE_SAVE_POSITION(&it->file);

        offset_t payload_start = internal_column_get_payload_off(it);
        MEMFILE_SEEK(&it->file, payload_start + pos * INTERNAL_GET_TYPE_VALUE_SIZE(it->field_type));

        switch (it->field_type) {
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                        u8 null_value = CARBON_BOOLEAN_COLUMN_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(u8));
                }
                        break;
                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_SET: {
                        u8 null_value = U8_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(u8));
                }
                        break;
                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_SET: {
                        u16 null_value = U16_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(u16));
                }
                        break;
                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_SET: {
                        u32 null_value = U32_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(u32));
                }
                        break;
                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_SET: {
                        u64 null_value = U64_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(u64));
                }
                        break;
                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_SET: {
                        i8 null_value = I8_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(i8));
                }
                        break;
                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_SET: {
                        i16 null_value = I16_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(i16));
                }
                        break;
                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_SET: {
                        i32 null_value = I32_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(i32));
                }
                        break;
                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET: {
                        i64 null_value = I64_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(i64));
                }
                        break;
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET: {
                        float null_value = CARBON_NULL_FLOAT;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(float));
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
                        MEMFILE_RESTORE_POSITION(&it->file);
                        return ERROR(ERR_UNSUPPCONTAINER, NULL);
                default:
                        MEMFILE_RESTORE_POSITION(&it->file);
                        ERROR(ERR_INTERNALERR, NULL);
                        return false;
        }

        MEMFILE_RESTORE_POSITION(&it->file);

        return true;
}

#define push_array_element(num_values, data, data_cast_type, null_check, insert_func)                                  \
for (u32 i = 0; i < num_values; i++) {                                                                                 \
        data_cast_type datum = ((data_cast_type *)data)[i];                                                            \
        if (LIKELY(null_check(datum) == false)) {                                                                      \
                insert_func(&array_ins);                                                                               \
        } else {                                                                                                       \
                insert_null(&array_ins);                                                                         \
        }                                                                                                              \
}

#define push_array_element_wvalue(num_values, data, data_cast_type, null_check, insert_func)                           \
for (u32 i = 0; i < num_values; i++) {                                                                                 \
        data_cast_type datum = ((data_cast_type *)data)[i];                                                            \
        if (LIKELY(null_check(datum) == false)) {                                                                      \
                insert_func(&array_ins, datum);                                                                        \
        } else {                                                                                                       \
                insert_null(&array_ins);                                                                         \
        }                                                                                                              \
}

static bool rewrite_column_to_array(col_it *it)
{
        abstract_type_class_e type_class;
        list_type_e list_type;
        arr_it array;
        insert array_ins;

        MEMFILE_SAVE_POSITION(&it->file);
        u8 marker = MEMFILE_PEEK_BYTE(&it->file);
        assert(FIELD_IS_COLUMN_OR_SUBTYPE(marker));

        type_class = abstract_get_class(marker);
        list_type = abstract_class_to_list_derivable(type_class);

        /** Potentially tailing space after the last ']' marker of the outer most array is used for temporary space */
        MEMFILE_SEEK_TO_END(&it->file);
        offset_t array_marker_begin = MEMFILE_TELL(&it->file);

        size_t capacity = it->num * INTERNAL_GET_TYPE_VALUE_SIZE(it->field_type);
        internal_insert_array(&it->file, list_type, capacity);
        internal_arr_it_create(&array, &it->file, array_marker_begin);
        arr_it_insert_begin(&array_ins, &array);

        field_e type;
        u32 num_values;
        const void *data = COL_IT_VALUES(&type, &num_values, it);
        switch (type) {
                case FIELD_NULL:
                        while (num_values--) {
                                insert_null(&array_ins);
                        }
                        break;
                case FIELD_TRUE:
                        push_array_element(num_values, data, u8, IS_NULL_BOOLEAN, insert_true);
                        break;
                case FIELD_FALSE:
                        push_array_element(num_values, data, u8, IS_NULL_BOOLEAN, insert_false);
                        break;
                case FIELD_NUMBER_U8:
                        push_array_element_wvalue(num_values, data, u8, IS_NULL_U8, insert_u8);
                        break;
                case FIELD_NUMBER_U16:
                        push_array_element_wvalue(num_values, data, u16, IS_NULL_U16, insert_u16);
                        break;
                case FIELD_NUMBER_U32:
                        push_array_element_wvalue(num_values, data, u32, IS_NULL_U32, insert_u32);
                        break;
                case FIELD_NUMBER_U64:
                        push_array_element_wvalue(num_values, data, u64, IS_NULL_U64, insert_u64);
                        break;
                case FIELD_NUMBER_I8:
                        push_array_element_wvalue(num_values, data, i8, IS_NULL_I8, insert_i8);
                        break;
                case FIELD_NUMBER_I16:
                        push_array_element_wvalue(num_values, data, i16, IS_NULL_I16, insert_i16);
                        break;
                case FIELD_NUMBER_I32:
                        push_array_element_wvalue(num_values, data, i32, IS_NULL_I32, insert_i32);
                        break;
                case FIELD_NUMBER_I64:
                        push_array_element_wvalue(num_values, data, i64, IS_NULL_I64, insert_i64);
                        break;
                case FIELD_NUMBER_FLOAT:
                        push_array_element_wvalue(num_values, data, float, IS_NULL_FLOAT, insert_float);
                        break;
                default: ERROR(ERR_UNSUPPORTEDTYPE, NULL);
                        return false;
        }

        arr_it_insert_end(&array_ins);
        assert(array_marker_begin < internal_arr_it_memfilepos(&array));

        MEMFILE_RESTORE_POSITION(&it->file);
        return true;
}

bool col_it_update_set_true(col_it *it, u32 pos)
{
        ERROR_IF_AND_RETURN(pos >= it->num, ERR_OUTOFBOUNDS, NULL)

        MEMFILE_SAVE_POSITION(&it->file);

        offset_t payload_start = internal_column_get_payload_off(it);
        MEMFILE_SEEK(&it->file, payload_start + pos * INTERNAL_GET_TYPE_VALUE_SIZE(it->field_type));

        switch (it->field_type) {
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                        u8 value = CARBON_BOOLEAN_COLUMN_TRUE;
                        MEMFILE_WRITE(&it->file, &value, sizeof(u8));
                }
                        break;
                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_SET: {
                        u8 null_value = U8_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(u8));
                }
                        break;
                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_SET: {
                        u16 null_value = U16_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(u16));
                }
                        break;
                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_SET: {
                        //u32 null_value = U32_NULL;
                        //MEMFILE_WRITE(&it->mem, &null_value, sizeof(u32));
                        rewrite_column_to_array(it);


                }
                        break;
                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_SET: {
                        u64 null_value = U64_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(u64));
                }
                        break;
                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_SET: {
                        i8 null_value = I8_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(i8));
                }
                        break;
                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_SET: {
                        i16 null_value = I16_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(i16));
                }
                        break;
                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_SET: {
                        i32 null_value = I32_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(i32));
                }
                        break;
                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET: {
                        i64 null_value = I64_NULL;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(i64));
                }
                        break;
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET: {
                        float null_value = CARBON_NULL_FLOAT;
                        MEMFILE_WRITE(&it->file, &null_value, sizeof(float));
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
                        MEMFILE_RESTORE_POSITION(&it->file);
                        return ERROR(ERR_UNSUPPCONTAINER, NULL);
                default:
                        MEMFILE_RESTORE_POSITION(&it->file);
                        ERROR(ERR_INTERNALERR, NULL);
                        return false;
        }

        MEMFILE_RESTORE_POSITION(&it->file);

        return true;
}

bool col_it_update_set_false(col_it *it, u32 pos)
{
        UNUSED(it)
        UNUSED(pos)
        ERROR(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_u8(col_it *it, u32 pos, u8 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        ERROR(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_u16(col_it *it, u32 pos, u16 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        ERROR(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_u32(col_it *it, u32 pos, u32 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        ERROR(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_u64(col_it *it, u32 pos, u64 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        ERROR(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_i8(col_it *it, u32 pos, i8 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        ERROR(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_i16(col_it *it, u32 pos, i16 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        ERROR(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_i32(col_it *it, u32 pos, i32 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        ERROR(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_i64(col_it *it, u32 pos, i64 value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        ERROR(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_update_set_float(col_it *it, u32 pos, float value)
{
        UNUSED(it)
        UNUSED(pos)
        UNUSED(value)
        ERROR(ERR_NOTIMPLEMENTED, NULL); // TODO: implement
        return false;
}

bool col_it_rewind(col_it *it)
{
        offset_t playload_start = internal_column_get_payload_off(it);
        ERROR_IF_AND_RETURN(playload_start >= MEMFILE_SIZE(&it->file), ERR_OUTOFBOUNDS, NULL);
        return MEMFILE_SEEK(&it->file, playload_start);
}

#define COL_IT_PRINT(it, nvalues, type, null_check)                                             \
{                                                                                               \
        const type *value = internal_col_it_##type##_values(&nvalues, it);                               \
        for (u32 i = 0; i < nvalues; i++) {                                                     \
                type x = value[i];                                                              \
                if (null_check(x)) {                                                            \
                        str_buf_add(dst, "null");                                               \
                } else {                                                                        \
                        str_buf_add_##type(dst, value[i]);                                      \
                }                                                                               \
                if (i + 1 < nvalues) {                                                          \
                        str_buf_add(dst, ", ");                                                 \
                }                                                                               \
        }                                                                                       \
}

bool col_it_print(str_buf *dst, col_it *it)
{
        u32 nvalues;

        str_buf_add_char(dst, '[');

        if (col_it_is_boolean(it)) {
                COL_IT_PRINT(it, nvalues, boolean, IS_NULL_BOOLEAN)
        } else if (col_it_is_i8(it)) {
                COL_IT_PRINT(it, nvalues, i8, IS_NULL_I8)
        } else if (col_it_is_i16(it)) {
                COL_IT_PRINT(it, nvalues, i16, IS_NULL_I16)
        } else if (col_it_is_i32(it)) {
                COL_IT_PRINT(it, nvalues, i32, IS_NULL_I32)
        } else if (col_it_is_i64(it)) {
                COL_IT_PRINT(it, nvalues, i64, IS_NULL_I64)
        } else if (col_it_is_u8(it)) {
                COL_IT_PRINT(it, nvalues, u8, IS_NULL_U8)
        } else if (col_it_is_u16(it)) {
                COL_IT_PRINT(it, nvalues, u16, IS_NULL_U16)
        } else if (col_it_is_u32(it)) {
                COL_IT_PRINT(it, nvalues, u32, IS_NULL_U32)
        } else if (col_it_is_u64(it)) {
                COL_IT_PRINT(it, nvalues, u64, IS_NULL_U64)
        } else if (col_it_is_float(it)) {
                COL_IT_PRINT(it, nvalues, float, IS_NULL_FLOAT)
        } else {
                str_buf_add_char(dst, ']');
                return ERROR(ERR_UNSUPPORTEDTYPE, "column has unsupported type");
        }

        str_buf_add_char(dst, ']');
        return true;
}