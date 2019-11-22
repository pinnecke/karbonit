/**
 * Copyright 2019 Marcus Pinnecke
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

#include <jakson/mem/file.h>
#include <jakson/carbon/field.h>
#include <jakson/carbon/col_it.h>
#include <jakson/carbon/mime.h>
#include <jakson/carbon/arr_it.h>
#include <jakson/carbon/object.h>
#include <jakson/carbon/abstract.h>

const char *field_type_str(field_type_e type)
{
        switch (type) {
                case FIELD_NULL:
                        return FIELD_TYPE_NULL_STR;
                case FIELD_TRUE:
                        return FIELD_TYPE_TRUE_STR;
                case FIELD_FALSE:
                        return FIELD_TYPE_FALSE_STR;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                        return FIELD_TYPE_OBJECT_UNSORTED_MULTIMAP_STR;
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                        return FIELD_TYPE_OBJECT_SORTED_MULTIMAP_STR;
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                        return FIELD_TYPE_OBJECT_UNSORTED_MAP_STR;
                case FIELD_DERIVED_OBJECT_SORTED_MAP:
                        return FIELD_TYPE_OBJECT_SORTED_MAP_STR;
                case FIELD_ARRAY_UNSORTED_MULTISET:
                        return FIELD_TYPE_ARRAY_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                        return FIELD_TYPE_ARRAY_SORTED_MULTISET_STR;
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                        return FIELD_TYPE_ARRAY_UNSORTED_SET_STR;
                case FIELD_DERIVED_ARRAY_SORTED_SET:
                        return FIELD_TYPE_ARRAY_SORTED_SET_STR;
                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_U8_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_U8_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                        return FIELD_TYPE_COLUMN_U8_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_U8_SORTED_SET:
                        return FIELD_TYPE_COLUMN_U8_SORTED_SET_STR;
                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_U16_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_U16_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                        return FIELD_TYPE_COLUMN_U16_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_U16_SORTED_SET:
                        return FIELD_TYPE_COLUMN_U16_SORTED_SET_STR;
                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_U32_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_U32_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                        return FIELD_TYPE_COLUMN_U32_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_U32_SORTED_SET:
                        return FIELD_TYPE_COLUMN_U32_SORTED_SET_STR;
                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_U64_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_U64_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                        return FIELD_TYPE_COLUMN_U64_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_U64_SORTED_SET:
                        return FIELD_TYPE_COLUMN_U64_SORTED_SET_STR;
                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_I8_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_I8_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                        return FIELD_TYPE_COLUMN_I8_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_I8_SORTED_SET:
                        return FIELD_TYPE_COLUMN_I8_SORTED_SET_STR;
                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_I16_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_I16_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                        return FIELD_TYPE_COLUMN_I16_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_I16_SORTED_SET:
                        return FIELD_TYPE_COLUMN_I16_SORTED_SET_STR;
                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_I32_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_I32_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                        return FIELD_TYPE_COLUMN_I32_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_I32_SORTED_SET:
                        return FIELD_TYPE_COLUMN_I32_SORTED_SET_STR;
                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_I64_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_I64_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                        return FIELD_TYPE_COLUMN_I64_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET:
                        return FIELD_TYPE_COLUMN_I64_SORTED_SET_STR;
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_FLOAT_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_FLOAT_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                        return FIELD_TYPE_COLUMN_FLOAT_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                        return FIELD_TYPE_COLUMN_FLOAT_SORTED_SET_STR;
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_BOOLEAN_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                        return FIELD_TYPE_COLUMN_BOOLEAN_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                        return FIELD_TYPE_COLUMN_BOOLEAN_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:
                        return FIELD_TYPE_COLUMN_BOOLEAN_SORTED_SET_STR;
                case FIELD_STRING:
                        return FIELD_TYPE_STRING_STR;
                case FIELD_NUMBER_U8:
                        return FIELD_TYPE_NUMBER_U8_STR;
                case FIELD_NUMBER_U16:
                        return FIELD_TYPE_NUMBER_U16_STR;
                case FIELD_NUMBER_U32:
                        return FIELD_TYPE_NUMBER_U32_STR;
                case FIELD_NUMBER_U64:
                        return FIELD_TYPE_NUMBER_U64_STR;
                case FIELD_NUMBER_I8:
                        return FIELD_TYPE_NUMBER_I8_STR;
                case FIELD_NUMBER_I16:
                        return FIELD_TYPE_NUMBER_I16_STR;
                case FIELD_NUMBER_I32:
                        return FIELD_TYPE_NUMBER_I32_STR;
                case FIELD_NUMBER_I64:
                        return FIELD_TYPE_NUMBER_I64_STR;
                case FIELD_NUMBER_FLOAT:
                        return FIELD_TYPE_NUMBER_FLOAT_STR;
                case FIELD_BINARY_CUSTOM:
                case FIELD_BINARY:
                        return FIELD_TYPE_BINARY_STR;
                default:
                        error(ERR_NOTFOUND, NULL);
                        return NULL;
        }
}

bool field_type_is_traversable(field_type_e type)
{
        return field_type_is_object_or_subtype(type) ||
                field_type_is_list_or_subtype(type);
}

bool field_type_is_signed(field_type_e type)
{
        return (type == FIELD_NUMBER_I8 || type == FIELD_NUMBER_I16 ||
                type == FIELD_NUMBER_I32 || type == FIELD_NUMBER_I64 ||
                type == FIELD_COLUMN_I8_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I8_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_I8_SORTED_SET ||
                type == FIELD_COLUMN_I16_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I16_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_I16_SORTED_SET ||
                type == FIELD_COLUMN_I32_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I32_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_I32_SORTED_SET ||
                type == FIELD_COLUMN_I64_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I64_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_I64_SORTED_SET);
}

bool field_type_is_unsigned(field_type_e type)
{
        return (type == FIELD_NUMBER_U8 || type == FIELD_NUMBER_U16 ||
                type == FIELD_NUMBER_U32 || type == FIELD_NUMBER_U64 ||
                type == FIELD_COLUMN_U8_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U8_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_U8_SORTED_SET ||
                type == FIELD_COLUMN_U16_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U16_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_U16_SORTED_SET ||
                type == FIELD_COLUMN_U32_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U32_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_U32_SORTED_SET ||
                type == FIELD_COLUMN_U64_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U64_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_U64_SORTED_SET);
}

bool field_type_is_floating(field_type_e type)
{
        return (type == FIELD_NUMBER_FLOAT || type == FIELD_COLUMN_FLOAT_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET);
}

bool field_type_is_number(field_type_e type)
{
        return field_type_is_integer(type) || field_type_is_floating(type);
}

bool field_type_is_integer(field_type_e type)
{
        return field_type_is_signed(type) || field_type_is_unsigned(type);
}

bool field_type_is_binary(field_type_e type)
{
        return (type == FIELD_BINARY || type == FIELD_BINARY_CUSTOM);
}

bool field_type_is_boolean(field_type_e type)
{
        return (type == FIELD_TRUE || type == FIELD_FALSE ||
                type == FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET);
}

bool field_type_is_string(field_type_e type)
{
        return (type == FIELD_STRING);
}

bool field_type_is_constant(field_type_e type)
{
        return (field_type_is_null(type) || field_type_is_boolean(type));
}

bool carbon_field_skip(memfile *file)
{
        u8 type_marker = *MEMFILE_PEEK(file, u8);

        switch (type_marker) {
                case FIELD_NULL:
                        carbon_field_skip_null(file);
                        break;
                case FIELD_TRUE:
                case FIELD_FALSE:
                        carbon_field_skip_boolean(file);
                        break;
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_I8:
                        carbon_field_skip_8(file);
                        break;
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_I16:
                        carbon_field_skip_16(file);
                        break;
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_I32:
                        carbon_field_skip_32(file);
                        break;
                case FIELD_NUMBER_U64:
                case FIELD_NUMBER_I64:
                        carbon_field_skip_64(file);
                        break;
                case FIELD_NUMBER_FLOAT:
                        carbon_field_skip_float(file);
                        break;
                case FIELD_STRING:
                        carbon_field_skip_string(file);
                        break;
                case FIELD_BINARY:
                        carbon_field_skip_binary(file);
                        break;
                case FIELD_BINARY_CUSTOM:
                        carbon_field_skip_custom_binary(file);
                        break;
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET:
                        carbon_field_skip_array(file);
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
                        carbon_field_skip_column(file);
                        break;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP:
                        carbon_field_skip_object(file);
                        break;
                default: error(ERR_CORRUPTED, NULL);
                        return false;
        }
        return true;
}

bool carbon_field_skip_object(memfile *file)
{
        if (abstract_is_instanceof_object(file)) {
                obj_it skip_it;
                internal_carbon_object_create(&skip_it, file, memfile_tell(file));
                internal_carbon_object_fast_forward(&skip_it);
                memfile_seek(file, memfile_tell(&skip_it.memfile));
                carbon_object_drop(&skip_it);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, "marker does not encode an object container or sub type");
        }
}

bool carbon_field_skip_array(memfile *file)
{
        if (abstract_is_instanceof_array(file)) {
                arr_it skip_it;
                internal_arr_it_create(&skip_it, file, memfile_tell(file));
                internal_arr_it_fast_forward(&skip_it);
                memfile_seek(file, memfile_tell(&skip_it.file));
                arr_it_drop(&skip_it);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, "marker does not encode an array container or sub type");
        }
}

bool carbon_field_skip_column(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(!field_type_is_column_or_subtype(type_marker), ERR_TYPEMISMATCH, NULL);

        col_it skip_it;
        col_it_create(&skip_it, file, memfile_tell(file) - sizeof(u8));
        col_it_fast_forward(&skip_it);
        memfile_seek(file, memfile_tell(&skip_it.file));
        return true;
}

bool carbon_field_skip_binary(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_BINARY, ERR_TYPEMISMATCH, NULL);
        /** read and skip mime type with variable-length integer type */
        u64 mime = memfile_read_uintvar_stream(NULL, file);
        UNUSED(mime);

        /** read blob length */
        u64 blob_len = memfile_read_uintvar_stream(NULL, file);

        /** skip blob */
        memfile_skip(file, blob_len);
        return true;
}

bool carbon_field_skip_custom_binary(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_BINARY_CUSTOM, ERR_TYPEMISMATCH, NULL);
        /** read custom type string_buffer length, and skip the type string_buffer */
        u64 custom_type_str_len = memfile_read_uintvar_stream(NULL, file);
        memfile_skip(file, custom_type_str_len);

        /** read blob length, and skip blob data */
        u64 blob_len = memfile_read_uintvar_stream(NULL, file);
        memfile_skip(file, blob_len);
        return true;
}

bool carbon_field_skip_string(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_STRING, ERR_TYPEMISMATCH, NULL);
        u64 strlen = memfile_read_uintvar_stream(NULL, file);
        memfile_skip(file, strlen);
        return true;
}

bool carbon_field_skip_float(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_NUMBER_FLOAT, ERR_TYPEMISMATCH, NULL);
        memfile_skip(file, sizeof(float));
        return true;
}

bool carbon_field_skip_boolean(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_TRUE && type_marker != FIELD_FALSE, ERR_TYPEMISMATCH, NULL);
        return true;
}

bool carbon_field_skip_null(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_NULL, ERR_TYPEMISMATCH, NULL);
        return true;
}

bool carbon_field_skip_8(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_NUMBER_I8 && type_marker != FIELD_NUMBER_U8,
                 ERR_TYPEMISMATCH, NULL);
        JAK_ASSERT(sizeof(u8) == sizeof(i8));
        memfile_skip(file, sizeof(u8));
        return true;
}

bool carbon_field_skip_16(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_NUMBER_I16 && type_marker != FIELD_NUMBER_U16,
                 ERR_TYPEMISMATCH, NULL);
        JAK_ASSERT(sizeof(u16) == sizeof(i16));
        memfile_skip(file, sizeof(u16));
        return true;
}

bool carbon_field_skip_32(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_NUMBER_I32 && type_marker != FIELD_NUMBER_U32,
                 ERR_TYPEMISMATCH, NULL);
        JAK_ASSERT(sizeof(u32) == sizeof(i32));
        memfile_skip(file, sizeof(u32));
        return true;
}

bool carbon_field_skip_64(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_NUMBER_I64 && type_marker != FIELD_NUMBER_U64,
                 ERR_TYPEMISMATCH, NULL);
        JAK_ASSERT(sizeof(u64) == sizeof(i64));
        memfile_skip(file, sizeof(u64));
        return true;
}

field_type_e field_type_for_column(list_type_e derivation, col_it_type_e type)
{
        switch (derivation) {
                case LIST_UNSORTED_MULTISET:
                        switch (type) {
                                case COLUMN_U8:
                                        return FIELD_COLUMN_U8_UNSORTED_MULTISET;
                                case COLUMN_U16:
                                        return FIELD_COLUMN_U16_UNSORTED_MULTISET;
                                case COLUMN_U32:
                                        return FIELD_COLUMN_U32_UNSORTED_MULTISET;
                                case COLUMN_U64:
                                        return FIELD_COLUMN_U64_UNSORTED_MULTISET;
                                case COLUMN_I8:
                                        return FIELD_COLUMN_I8_UNSORTED_MULTISET;
                                case COLUMN_I16:
                                        return FIELD_COLUMN_I16_UNSORTED_MULTISET;
                                case COLUMN_I32:
                                        return FIELD_COLUMN_I32_UNSORTED_MULTISET;
                                case COLUMN_I64:
                                        return FIELD_COLUMN_I64_UNSORTED_MULTISET;
                                case COLUMN_FLOAT:
                                        return FIELD_COLUMN_FLOAT_UNSORTED_MULTISET;
                                case COLUMN_BOOLEAN:
                                        return FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET;
                                default: error(ERR_INTERNALERR, NULL);
                                        return 0;
                        }
                case LIST_SORTED_MULTISET:
                        switch (type) {
                                case COLUMN_U8:
                                        return FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET;
                                case COLUMN_U16:
                                        return FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET;
                                case COLUMN_U32:
                                        return FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET;
                                case COLUMN_U64:
                                        return FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET;
                                case COLUMN_I8:
                                        return FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET;
                                case COLUMN_I16:
                                        return FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET;
                                case COLUMN_I32:
                                        return FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET;
                                case COLUMN_I64:
                                        return FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET;
                                case COLUMN_FLOAT:
                                        return FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET;
                                case COLUMN_BOOLEAN:
                                        return FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET;
                                default: error(ERR_INTERNALERR, NULL);
                                        return 0;
                        }
                case LIST_UNSORTED_SET:
                        switch (type) {
                                case COLUMN_U8:
                                        return FIELD_DERIVED_COLUMN_U8_UNSORTED_SET;
                                case COLUMN_U16:
                                        return FIELD_DERIVED_COLUMN_U16_UNSORTED_SET;
                                case COLUMN_U32:
                                        return FIELD_DERIVED_COLUMN_U32_UNSORTED_SET;
                                case COLUMN_U64:
                                        return FIELD_DERIVED_COLUMN_U64_UNSORTED_SET;
                                case COLUMN_I8:
                                        return FIELD_DERIVED_COLUMN_I8_UNSORTED_SET;
                                case COLUMN_I16:
                                        return FIELD_DERIVED_COLUMN_I16_UNSORTED_SET;
                                case COLUMN_I32:
                                        return FIELD_DERIVED_COLUMN_I32_UNSORTED_SET;
                                case COLUMN_I64:
                                        return FIELD_DERIVED_COLUMN_I64_UNSORTED_SET;
                                case COLUMN_FLOAT:
                                        return FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET;
                                case COLUMN_BOOLEAN:
                                        return FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET;
                                default: error(ERR_INTERNALERR, NULL);
                                        return 0;
                        }
                case LIST_SORTED_SET:
                        switch (type) {
                                case COLUMN_U8:
                                        return FIELD_DERIVED_COLUMN_U8_SORTED_SET;
                                case COLUMN_U16:
                                        return FIELD_DERIVED_COLUMN_U16_SORTED_SET;
                                case COLUMN_U32:
                                        return FIELD_DERIVED_COLUMN_U32_SORTED_SET;
                                case COLUMN_U64:
                                        return FIELD_DERIVED_COLUMN_U64_SORTED_SET;
                                case COLUMN_I8:
                                        return FIELD_DERIVED_COLUMN_I8_SORTED_SET;
                                case COLUMN_I16:
                                        return FIELD_DERIVED_COLUMN_I16_SORTED_SET;
                                case COLUMN_I32:
                                        return FIELD_DERIVED_COLUMN_I32_SORTED_SET;
                                case COLUMN_I64:
                                        return FIELD_DERIVED_COLUMN_I64_SORTED_SET;
                                case COLUMN_FLOAT:
                                        return FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET;
                                case COLUMN_BOOLEAN:
                                        return FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET;
                                default: error(ERR_INTERNALERR, NULL);
                                        return 0;
                        }
                default: error(ERR_INTERNALERR, NULL);
                        return 0;
        }
}

field_type_e
field_type_column_entry_to_regular_type(field_type_e type, bool is_null, bool is_true)
{
        if (is_null) {
                return FIELD_NULL;
        } else {
                switch (type) {
                        case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_U8_SORTED_SET:
                                return FIELD_NUMBER_U8;
                        case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_U16_SORTED_SET:
                                return FIELD_NUMBER_U16;
                        case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_U32_SORTED_SET:
                                return FIELD_NUMBER_U32;
                        case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_U64_SORTED_SET:
                                return FIELD_NUMBER_U64;
                        case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_I8_SORTED_SET:
                                return FIELD_NUMBER_I8;
                        case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_I16_SORTED_SET:
                                return FIELD_NUMBER_I16;
                        case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_I32_SORTED_SET:
                                return FIELD_NUMBER_I32;
                        case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_I64_SORTED_SET:
                                return FIELD_NUMBER_I64;
                        case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                                return FIELD_NUMBER_FLOAT;
                        case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:
                                return is_true ? FIELD_TRUE : FIELD_FALSE;
                        default: error(ERR_INTERNALERR, NULL);
                                return 0;
                }
        }
}

field_class_e field_type_get_class(field_type_e type)
{
        switch (type) {
                case FIELD_NULL:
                case FIELD_TRUE:
                case FIELD_FALSE:
                        return CLASS_CONSTANT;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP:
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET:
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
                        return CLASS_CONTAINER;
                case FIELD_STRING:
                        return CLASS_CHARACTER_STRING;
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_U64:
                case FIELD_NUMBER_I8:
                case FIELD_NUMBER_I16:
                case FIELD_NUMBER_I32:
                case FIELD_NUMBER_I64:
                case FIELD_NUMBER_FLOAT:
                        return CLASS_NUMBER;
                case FIELD_BINARY:
                case FIELD_BINARY_CUSTOM:
                        return CLASS_BINARY_STRING;
                default: error(ERR_INTERNALERR, NULL);
                        return 0;
        }
}

bool field_type_is_array_or_subtype(field_type_e type)
{
        return (type == FIELD_ARRAY_UNSORTED_MULTISET || type == FIELD_DERIVED_ARRAY_SORTED_MULTISET ||
                type == FIELD_DERIVED_ARRAY_UNSORTED_SET || type == FIELD_DERIVED_ARRAY_SORTED_SET);
}

bool field_type_is_column_u8_or_subtype(field_type_e type)
{
        return (type == FIELD_COLUMN_U8_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U8_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_U8_SORTED_SET);
}

bool field_type_is_column_u16_or_subtype(field_type_e type)
{
        return (type == FIELD_COLUMN_U16_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U16_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_U16_SORTED_SET);
}

bool field_type_is_column_u32_or_subtype(field_type_e type)
{
        return (type == FIELD_COLUMN_U32_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U32_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_U32_SORTED_SET);
}

bool field_type_is_column_u64_or_subtype(field_type_e type)
{
        return (type == FIELD_COLUMN_U64_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_U64_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_U64_SORTED_SET);
}

bool field_type_is_column_i8_or_subtype(field_type_e type)
{
        return (type == FIELD_COLUMN_I8_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I8_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_I8_SORTED_SET);
}

bool field_type_is_column_i16_or_subtype(field_type_e type)
{
        return (type == FIELD_COLUMN_I16_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I16_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_I16_SORTED_SET);
}

bool field_type_is_column_i32_or_subtype(field_type_e type)
{
        return (type == FIELD_COLUMN_I32_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I32_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_I32_SORTED_SET);
}

bool field_type_is_column_i64_or_subtype(field_type_e type)
{
        return (type == FIELD_COLUMN_I64_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_I64_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_I64_SORTED_SET);
}

bool field_type_is_column_float_or_subtype(field_type_e type)
{
        return (type == FIELD_COLUMN_FLOAT_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET);
}

bool field_type_is_column_bool_or_subtype(field_type_e type)
{
        return (type == FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET ||
                type == FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET ||
                type == FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET);
}



bool field_type_is_list_or_subtype(field_type_e type)
{
        return field_type_is_array_or_subtype(type) || field_type_is_column_or_subtype(type);
}

bool field_type_is_column_or_subtype(field_type_e type)
{
        return field_type_is_column_u8_or_subtype(type) ||
                field_type_is_column_u16_or_subtype(type) ||
                field_type_is_column_u32_or_subtype(type) ||
                field_type_is_column_u64_or_subtype(type) ||
                field_type_is_column_i8_or_subtype(type) ||
                field_type_is_column_i16_or_subtype(type) ||
                field_type_is_column_i32_or_subtype(type) ||
                field_type_is_column_i64_or_subtype(type) ||
                field_type_is_column_float_or_subtype(type) ||
                field_type_is_column_bool_or_subtype(type);
}

bool field_type_is_object_or_subtype(field_type_e type)
{
        return (type == FIELD_OBJECT_UNSORTED_MULTIMAP || type == FIELD_DERIVED_OBJECT_SORTED_MULTIMAP ||
                        type == FIELD_DERIVED_OBJECT_UNSORTED_MAP ||
                        type == FIELD_DERIVED_OBJECT_SORTED_MAP);
}

bool field_type_is_null(field_type_e type)
{
        return (type == FIELD_NULL);
}