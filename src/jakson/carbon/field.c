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

#include <jakson/mem/memfile.h>
#include <jakson/carbon/field.h>
#include <jakson/carbon/col-it.h>
#include <jakson/carbon/mime.h>
#include <jakson/carbon/arr-it.h>
#include <jakson/carbon/obj-it.h>
#include <jakson/carbon/abstract.h>

const char *field_str(field_e type)
{
        switch (type) {
                case FIELD_NULL:
                        return NULL_STR;
                case FIELD_TRUE:
                        return TRUE_STR;
                case FIELD_FALSE:
                        return FALSE_STR;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                        return OBJECT_UNSORTED_MULTIMAP_STR;
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                        return OBJECT_SORTED_MULTIMAP_STR;
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                        return OBJECT_UNSORTED_MAP_STR;
                case FIELD_DERIVED_OBJECT_SORTED_MAP:
                        return OBJECT_SORTED_MAP_STR;
                case FIELD_ARRAY_UNSORTED_MULTISET:
                        return ARRAY_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                        return ARRAY_SORTED_MULTISET_STR;
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                        return ARRAY_UNSORTED_SET_STR;
                case FIELD_DERIVED_ARRAY_SORTED_SET:
                        return ARRAY_SORTED_SET_STR;
                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                        return COLUMN_U8_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                        return COLUMN_U8_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                        return COLUMN_U8_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_U8_SORTED_SET:
                        return COLUMN_U8_SORTED_SET_STR;
                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                        return COLUMN_U16_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                        return COLUMN_U16_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                        return COLUMN_U16_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_U16_SORTED_SET:
                        return COLUMN_U16_SORTED_SET_STR;
                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                        return COLUMN_U32_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                        return COLUMN_U32_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                        return COLUMN_U32_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_U32_SORTED_SET:
                        return COLUMN_U32_SORTED_SET_STR;
                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                        return COLUMN_U64_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                        return COLUMN_U64_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                        return COLUMN_U64_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_U64_SORTED_SET:
                        return COLUMN_U64_SORTED_SET_STR;
                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                        return COLUMN_I8_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                        return COLUMN_I8_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                        return COLUMN_I8_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_I8_SORTED_SET:
                        return COLUMN_I8_SORTED_SET_STR;
                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                        return COLUMN_I16_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                        return COLUMN_I16_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                        return COLUMN_I16_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_I16_SORTED_SET:
                        return COLUMN_I16_SORTED_SET_STR;
                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                        return COLUMN_I32_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                        return COLUMN_I32_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                        return COLUMN_I32_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_I32_SORTED_SET:
                        return COLUMN_I32_SORTED_SET_STR;
                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                        return COLUMN_I64_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                        return COLUMN_I64_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                        return COLUMN_I64_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET:
                        return COLUMN_I64_SORTED_SET_STR;
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                        return COLUMN_FLOAT_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                        return COLUMN_FLOAT_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                        return COLUMN_FLOAT_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                        return COLUMN_FLOAT_SORTED_SET_STR;
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                        return COLUMN_BOOLEAN_UNSORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                        return COLUMN_BOOLEAN_SORTED_MULTISET_STR;
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                        return COLUMN_BOOLEAN_UNSORTED_SET_STR;
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:
                        return COLUMN_BOOLEAN_SORTED_SET_STR;
                case FIELD_STRING:
                        return STRING_STR;
                case FIELD_NUMBER_U8:
                        return NUMBER_U8_STR;
                case FIELD_NUMBER_U16:
                        return NUMBER_U16_STR;
                case FIELD_NUMBER_U32:
                        return NUMBER_U32_STR;
                case FIELD_NUMBER_U64:
                        return NUMBER_U64_STR;
                case FIELD_NUMBER_I8:
                        return NUMBER_I8_STR;
                case FIELD_NUMBER_I16:
                        return NUMBER_I16_STR;
                case FIELD_NUMBER_I32:
                        return NUMBER_I32_STR;
                case FIELD_NUMBER_I64:
                        return NUMBER_I64_STR;
                case FIELD_NUMBER_FLOAT:
                        return NUMBER_FLOAT_STR;
                case FIELD_BINARY_CUSTOM:
                case FIELD_BINARY:
                        return BINARY_STR;
                default:
                        error(ERR_NOTFOUND, NULL);
                        return NULL;
        }
}

bool carbon_field_skip(memfile *file)
{
        u8 type_marker = *MEMFILE_PEEK_TYPE(file, u8);

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
                        carbon_field_skip_array(file, type_marker);
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
                        carbon_field_skip_object(file, type_marker);
                        break;
                default: error(ERR_CORRUPTED, NULL);
                        return false;
        }
        return true;
}

void carbon_field_skip_object__fast(memfile *file, const field *field)
{
        obj_it skip_it;
        internal_obj_it_clone(&skip_it, field->object);
        internal_obj_it_fast_forward(&skip_it);
        MEMFILE_SEEK(file, MEMFILE_TELL(&skip_it.file));
        obj_it_drop(&skip_it);
}

void carbon_field_skip_array__fast(memfile *file, const field *field)
{
        arr_it skip_it;
        internal_arr_it_clone(&skip_it, field->array);
        internal_arr_it_fast_forward(&skip_it);
        MEMFILE_SEEK(file, MEMFILE_TELL(&skip_it.file));
        arr_it_drop(&skip_it);
}

void carbon_field_skip_column__fast(memfile *file, const field *field)
{
        internal_col_it_skip__fast(file, field);
}

void carbon_field_skip_binary__fast(memfile *file, const field *field)
{
        /** skip blob */
        MEMFILE_SKIP(file, field->len);
}

void carbon_field_skip_custom_binary__fast(memfile *file, const field *field)
{
        /** skip blob */
        MEMFILE_SKIP(file, field->len);
}

void carbon_field_skip_string__fast(memfile *file, const field *field)
{
        /** skip blob */
        MEMFILE_SKIP(file, field->len);
}

void carbon_field_skip_float__fast(memfile *file)
{
        MEMFILE_SKIP(file, sizeof(float));
}

void carbon_field_skip_8__fast(memfile *file)
{
        MEMFILE_SKIP(file, sizeof(u8));
}

void carbon_field_skip_16__fast(memfile *file)
{
        MEMFILE_SKIP(file, sizeof(u16));
}

void carbon_field_skip_32__fast(memfile *file)
{
        MEMFILE_SKIP(file, sizeof(u32));
}

void carbon_field_skip_64__fast(memfile *file)
{
        MEMFILE_SKIP(file, sizeof(u64));
}

bool carbon_field_skip_object(memfile *file, u8 marker)
{
        if (abstract_is_instanceof_object(marker)) {
                obj_it skip_it;
                internal_obj_it_create(&skip_it, file, MEMFILE_TELL(file));
                internal_obj_it_fast_forward(&skip_it);
                MEMFILE_SEEK(file, MEMFILE_TELL(&skip_it.file));
                obj_it_drop(&skip_it);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, "marker does not encode an object container or sub type");
        }
}

bool carbon_field_skip_array(memfile *file, u8 marker)
{
        if (abstract_is_instanceof_array(marker)) {
                arr_it skip_it;
                internal_arr_it_create(&skip_it, file, MEMFILE_TELL(file));
                internal_arr_it_fast_forward(&skip_it);
                MEMFILE_SEEK(file, MEMFILE_TELL(&skip_it.file));
                arr_it_drop(&skip_it);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, "marker does not encode an array container or sub type");
        }
}

bool carbon_field_skip_column(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(!field_is_column_or_subtype(type_marker), ERR_TYPEMISMATCH, NULL);

        col_it skip_it;
        col_it_create(&skip_it, file, MEMFILE_TELL(file) - sizeof(u8));
        col_it_fast_forward(&skip_it);
        MEMFILE_SEEK(file, MEMFILE_TELL(&skip_it.file));
        return true;
}

bool carbon_field_skip_binary(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_BINARY, ERR_TYPEMISMATCH, NULL);
        /** read and skip mime type with variable-length integer type */
        u64 mime = MEMFILE_READ_UINTVAR_STREAM(NULL, file);
        UNUSED(mime);

        /** read blob length */
        u64 blob_len = MEMFILE_READ_UINTVAR_STREAM(NULL, file);

        /** skip blob */
        MEMFILE_SKIP(file, blob_len);
        return true;
}

bool carbon_field_skip_custom_binary(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_BINARY_CUSTOM, ERR_TYPEMISMATCH, NULL);
        /** read custom type str_buf length, and skip the type str_buf */
        u64 custom_type_str_len = MEMFILE_READ_UINTVAR_STREAM(NULL, file);
        MEMFILE_SKIP(file, custom_type_str_len);

        /** read blob length, and skip blob data */
        u64 blob_len = MEMFILE_READ_UINTVAR_STREAM(NULL, file);
        MEMFILE_SKIP(file, blob_len);
        return true;
}

bool carbon_field_skip_string(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_STRING, ERR_TYPEMISMATCH, NULL);
        u64 strlen = MEMFILE_READ_UINTVAR_STREAM(NULL, file);
        MEMFILE_SKIP(file, strlen);
        return true;
}

bool carbon_field_skip_float(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_NUMBER_FLOAT, ERR_TYPEMISMATCH, NULL);
        MEMFILE_SKIP(file, sizeof(float));
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
        assert(sizeof(u8) == sizeof(i8));
        MEMFILE_SKIP(file, sizeof(u8));
        return true;
}

bool carbon_field_skip_16(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_NUMBER_I16 && type_marker != FIELD_NUMBER_U16,
                 ERR_TYPEMISMATCH, NULL);
        assert(sizeof(u16) == sizeof(i16));
        MEMFILE_SKIP(file, sizeof(u16));
        return true;
}

bool carbon_field_skip_32(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_NUMBER_I32 && type_marker != FIELD_NUMBER_U32,
                 ERR_TYPEMISMATCH, NULL);
        assert(sizeof(u32) == sizeof(i32));
        MEMFILE_SKIP(file, sizeof(u32));
        return true;
}

bool carbon_field_skip_64(memfile *file)
{
        u8 type_marker = *MEMFILE_READ_TYPE(file, u8);

        error_if_and_return(type_marker != FIELD_NUMBER_I64 && type_marker != FIELD_NUMBER_U64,
                 ERR_TYPEMISMATCH, NULL);
        assert(sizeof(u64) == sizeof(i64));
        MEMFILE_SKIP(file, sizeof(u64));
        return true;
}

field_e field_for_column(list_type_e derivation, col_it_type_e type)
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

field_e
field_column_entry_to_regular_type(field_e type, bool is_null, bool is_true)
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

field_class_e field_get_class(field_e type)
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