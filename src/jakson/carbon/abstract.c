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

#include <jakson/carbon/abstract.h>
#include <jakson/mem/memfile.h>

bool abstract_list_derivable_to_class(abstract_type_class_e *out, list_type_e in)
{
        switch (in) {
                case LIST_UNSORTED_MULTISET:
                        *out = TYPE_UNSORTED_MULTISET;
                        break;
                case LIST_SORTED_MULTISET:
                        *out = TYPE_SORTED_MULTISET;
                        break;
                case LIST_UNSORTED_SET:
                        *out = TYPE_UNSORTED_SET;
                        break;
                case LIST_SORTED_SET:
                        *out = TYPE_SORTED_SET;
                        break;
                default:
                        return error(ERR_TYPEMISMATCH, "abstract class type does not encode a list type");
        }
        return true;
}

bool abstract_map_derivable_to_class(abstract_type_class_e *out, map_type_e in)
{
        switch (in) {
                case MAP_UNSORTED_MULTIMAP:
                        *out = TYPE_UNSORTED_MULTIMAP;
                        break;
                case MAP_SORTED_MULTIMAP:
                        *out = TYPE_SORTED_MULTIMAP;
                        break;
                case MAP_UNSORTED_MAP:
                        *out = TYPE_UNSORTED_MAP;
                        break;
                case MAP_SORTED_MAP:
                        *out = TYPE_SORTED_MAP;
                        break;
                default:
                        return error(ERR_TYPEMISMATCH, "abstract class type does not encode a map type");
        }
        return true;
}

void abstract_write_base_type(memfile *memfile, sub_type_e type)
{
        MEMFILE_WRITE(memfile, &type, sizeof(u8));
}

void abstract_write_derived_type(memfile *memfile, derived_e type)
{
        MEMFILE_WRITE(memfile, &type, sizeof(u8));
}

bool abstract_get_container_subtype(sub_type_e *type, u8 marker)
{
        switch (marker) {
                /** abstract types for object containers */
                case UNSORTED_MULTIMAP:
                case SORTED_MULTIMAP:
                case UNSORTED_MAP:
                case SORTED_MAP:
                        *type = CONTAINER_OBJECT;
                        return true;
                /** abstract types for array containers */
                case UNSORTED_MULTISET_ARRAY:
                case SORTED_MULTISET_ARRAY:
                case UNSORTED_SET_ARRAY:
                case SORTED_SET_ARRAY:
                        *type = CONTAINER_ARRAY;
                        return true;
                /** abstract types for column-u8 containers */
                case UNSORTED_MULTISET_COL_U8:
                case SORTED_MULTISET_COL_U8:
                case UNSORTED_SET_COL_U8:
                case SORTED_SET_COL_U8:
                        *type = CONTAINER_COLUMN_U8;
                        return true;
                /** abstract types for column-u16 containers */
                case UNSORTED_MULTISET_COL_U16:
                case SORTED_MULTISET_COL_U16:
                case UNSORTED_SET_COL_U16:
                case SORTED_SET_COL_U16:
                        *type = CONTAINER_COLUMN_U16;
                        return true;
                /** abstract types for column-u32 containers */
                case UNSORTED_MULTISET_COL_U32:
                case SORTED_MULTISET_COL_U32:
                case UNSORTED_SET_COL_U32:
                case SORTED_SET_COL_U32:
                        *type = CONTAINER_COLUMN_U32;
                        return true;
                /** abstract types for column-u64 containers */
                case UNSORTED_MULTISET_COL_U64:
                case SORTED_MULTISET_COL_U64:
                case UNSORTED_SET_COL_U64:
                case SORTED_SET_COL_U64:
                        *type = CONTAINER_COLUMN_U64;
                        return true;
                /** abstract types for column-i8 containers */
                case UNSORTED_MULTISET_COL_I8:
                case SORTED_MULTISET_COL_I8:
                case UNSORTED_SET_COL_I8:
                case SORTED_SET_COL_I8:
                        *type = CONTAINER_COLUMN_I8;
                        return true;
                /** abstract types for column-i16 containers */
                case UNSORTED_MULTISET_COL_I16:
                case SORTED_MULTISET_COL_I16:
                case UNSORTED_SET_COL_I16:
                case SORTED_SET_COL_I16:
                        *type = CONTAINER_COLUMN_I16;
                        return true;
                /** abstract types for column-i32 containers */
                case UNSORTED_MULTISET_COL_I32:
                case SORTED_MULTISET_COL_I32:
                case UNSORTED_SET_COL_I32:
                case SORTED_SET_COL_I32:
                        *type = CONTAINER_COLUMN_I32;
                        return true;
                /** abstract types for column-i64 containers */
                case UNSORTED_MULTISET_COL_I64:
                case SORTED_MULTISET_COL_I64:
                case UNSORTED_SET_COL_I64:
                case SORTED_SET_COL_I64:
                        *type = CONTAINER_COLUMN_I64;
                        return true;
                /** abstract types for column-float containers */
                case UNSORTED_MULTISET_COL_FLOAT:
                case SORTED_MULTISET_COL_FLOAT:
                case UNSORTED_SET_COL_FLOAT:
                case SORTED_SET_COL_FLOAT:
                        *type = CONTAINER_COLUMN_FLOAT;
                        return true;
                /** abstract types for column-boolean containers */
                case UNSORTED_MULTISET_COL_BOOLEAN:
                case SORTED_MULTISET_COL_BOOLEAN:
                case UNSORTED_SET_COL_BOOLEAN:
                case SORTED_SET_COL_BOOLEAN:
                        *type = CONTAINER_COLUMN_BOOLEAN;
                        return true;
                default:
                        return error(ERR_MARKERMAPPING, "unknown marker encoding an abstract type");
        }
}

static bool __abstract_is_instanceof(u8 marker, sub_type_e T)
{
        sub_type_e type;
        if (likely(abstract_get_container_subtype(&type, marker))) {
                return type == T;
        } else {
                return false;
        }
}

bool abstract_is_instanceof_object(u8 marker)
{
        return __abstract_is_instanceof(marker, CONTAINER_OBJECT);
}

bool abstract_is_instanceof_array(u8 marker)
{
        return __abstract_is_instanceof(marker, CONTAINER_ARRAY);
}

bool abstract_is_instanceof_column_u8(u8 marker)
{
        return __abstract_is_instanceof(marker, CONTAINER_COLUMN_U8);
}

bool abstract_is_instanceof_column_u16(u8 marker)
{
        return __abstract_is_instanceof(marker, CONTAINER_COLUMN_U16);
}

bool abstract_is_instanceof_column_u32(u8 marker)
{
        return __abstract_is_instanceof(marker, CONTAINER_COLUMN_U32);
}

bool abstract_is_instanceof_column_u64(u8 marker)
{
        return __abstract_is_instanceof(marker, CONTAINER_COLUMN_U64);
}

bool abstract_is_instanceof_column_i8(u8 marker)
{
        return __abstract_is_instanceof(marker, CONTAINER_COLUMN_I8);
}

bool abstract_is_instanceof_column_i16(u8 marker)
{
        return __abstract_is_instanceof(marker, CONTAINER_COLUMN_I16);
}

bool abstract_is_instanceof_column_i32(u8 marker)
{
        return __abstract_is_instanceof(marker, CONTAINER_COLUMN_I32);
}

bool abstract_is_instanceof_column_i64(u8 marker)
{
        return __abstract_is_instanceof(marker, CONTAINER_COLUMN_I64);
}

bool abstract_is_instanceof_column_float(u8 marker)
{
        return __abstract_is_instanceof(marker, CONTAINER_COLUMN_FLOAT);
}

bool abstract_is_instanceof_column_boolean(u8 marker)
{
        return __abstract_is_instanceof(marker, CONTAINER_COLUMN_BOOLEAN);
}

bool abstract_is_instanceof_column(u8 marker)
{
       if (abstract_is_instanceof_column_u8(marker) ||
                abstract_is_instanceof_column_u16(marker) ||
                abstract_is_instanceof_column_u32(marker) ||
                abstract_is_instanceof_column_u64(marker) ||
                abstract_is_instanceof_column_i8(marker) ||
                abstract_is_instanceof_column_i16(marker) ||
                abstract_is_instanceof_column_i32(marker) ||
                abstract_is_instanceof_column_i64(marker) ||
                abstract_is_instanceof_column_float(marker) ||
                abstract_is_instanceof_column_boolean(marker)) {
                return true;
        } else {
                return false;
        }
}

bool abstract_is_instanceof_list(u8 marker)
{
        if (abstract_is_instanceof_array(marker) ||
            abstract_is_instanceof_column(marker)) {
                return true;
        } else {
                return false;
        }
}

bool abstract_derive_list_to(derived_e *concrete, list_container_e is,
                                         list_type_e should)
{
        switch (is) {
                case LIST_ARRAY:
                        switch (should) {
                                case LIST_UNSORTED_MULTISET:
                                        *concrete = UNSORTED_MULTISET_ARRAY;
                                        return true;
                                case LIST_SORTED_MULTISET:
                                        *concrete = SORTED_MULTISET_ARRAY;
                                        return true;
                                case LIST_UNSORTED_SET:
                                        *concrete = UNSORTED_SET_ARRAY;
                                        return true;
                                case LIST_SORTED_SET:
                                        *concrete = SORTED_SET_ARRAY;
                                        return true;
                                default:
                                        goto error_case;
                        }
                case LIST_COLUMN_U8:
                        switch (should) {
                                case LIST_UNSORTED_MULTISET:
                                        *concrete = UNSORTED_MULTISET_COL_U8;
                                        return true;
                                case LIST_SORTED_MULTISET:
                                        *concrete = SORTED_MULTISET_COL_U8;
                                        return true;
                                case LIST_UNSORTED_SET:
                                        *concrete = UNSORTED_SET_COL_U8;
                                        return true;
                                case LIST_SORTED_SET:
                                        *concrete = SORTED_SET_COL_U8;
                                        return true;
                                default:
                                        goto error_case;
                        }
                case LIST_COLUMN_U16:
                        switch (should) {
                                case LIST_UNSORTED_MULTISET:
                                        *concrete = UNSORTED_MULTISET_COL_U16;
                                        return true;
                                case LIST_SORTED_MULTISET:
                                        *concrete = SORTED_MULTISET_COL_U16;
                                        return true;
                                case LIST_UNSORTED_SET:
                                        *concrete = UNSORTED_SET_COL_U16;
                                        return true;
                                case LIST_SORTED_SET:
                                        *concrete = SORTED_SET_COL_U16;
                                        return true;
                                default:
                                        goto error_case;
                        }
                case LIST_COLUMN_U32:
                        switch (should) {
                                case LIST_UNSORTED_MULTISET:
                                        *concrete = UNSORTED_MULTISET_COL_U32;
                                        return true;
                                case LIST_SORTED_MULTISET:
                                        *concrete = SORTED_MULTISET_COL_U32;
                                        return true;
                                case LIST_UNSORTED_SET:
                                        *concrete = UNSORTED_SET_COL_U32;
                                        return true;
                                case LIST_SORTED_SET:
                                        *concrete = SORTED_SET_COL_U32;
                                        return true;
                                default:
                                        goto error_case;
                        }
                case LIST_COLUMN_U64:
                        switch (should) {
                                case LIST_UNSORTED_MULTISET:
                                        *concrete = UNSORTED_MULTISET_COL_U64;
                                        return true;
                                case LIST_SORTED_MULTISET:
                                        *concrete = SORTED_MULTISET_COL_U64;
                                        return true;
                                case LIST_UNSORTED_SET:
                                        *concrete = UNSORTED_SET_COL_U64;
                                        return true;
                                case LIST_SORTED_SET:
                                        *concrete = SORTED_SET_COL_U64;
                                        return true;
                                default:
                                        goto error_case;
                        }
                case LIST_COLUMN_I8:
                        switch (should) {
                                case LIST_UNSORTED_MULTISET:
                                        *concrete = UNSORTED_MULTISET_COL_I8;
                                        return true;
                                case LIST_SORTED_MULTISET:
                                        *concrete = SORTED_MULTISET_COL_I8;
                                        return true;
                                case LIST_UNSORTED_SET:
                                        *concrete = UNSORTED_SET_COL_I8;
                                        return true;
                                case LIST_SORTED_SET:
                                        *concrete = SORTED_SET_COL_I8;
                                        return true;
                                default:
                                        goto error_case;
                        }
                case LIST_COLUMN_I16:
                        switch (should) {
                                case LIST_UNSORTED_MULTISET:
                                        *concrete = UNSORTED_MULTISET_COL_I16;
                                        return true;
                                case LIST_SORTED_MULTISET:
                                        *concrete = SORTED_MULTISET_COL_I16;
                                        return true;
                                case LIST_UNSORTED_SET:
                                        *concrete = UNSORTED_SET_COL_I16;
                                        return true;
                                case LIST_SORTED_SET:
                                        *concrete = SORTED_SET_COL_I16;
                                        return true;
                                default:
                                        goto error_case;
                        }
                case LIST_COLUMN_I32:
                        switch (should) {
                                case LIST_UNSORTED_MULTISET:
                                        *concrete = UNSORTED_MULTISET_COL_I32;
                                        return true;
                                case LIST_SORTED_MULTISET:
                                        *concrete = SORTED_MULTISET_COL_I32;
                                        return true;
                                case LIST_UNSORTED_SET:
                                        *concrete = UNSORTED_SET_COL_I32;
                                        return true;
                                case LIST_SORTED_SET:
                                        *concrete = SORTED_SET_COL_I32;
                                        return true;
                                default:
                                        goto error_case;
                        }
                case LIST_COLUMN_I64:
                        switch (should) {
                                case LIST_UNSORTED_MULTISET:
                                        *concrete = UNSORTED_MULTISET_COL_I64;
                                        return true;
                                case LIST_SORTED_MULTISET:
                                        *concrete = SORTED_MULTISET_COL_I64;
                                        return true;
                                case LIST_UNSORTED_SET:
                                        *concrete = UNSORTED_SET_COL_I64;
                                        return true;
                                case LIST_SORTED_SET:
                                        *concrete = SORTED_SET_COL_I64;
                                        return true;
                                default:
                                        goto error_case;
                        }
                case LIST_COLUMN_BOOLEAN:
                        switch (should) {
                                case LIST_UNSORTED_MULTISET:
                                        *concrete = UNSORTED_MULTISET_COL_BOOLEAN;
                                        return true;
                                case LIST_SORTED_MULTISET:
                                        *concrete = SORTED_MULTISET_COL_BOOLEAN;
                                        return true;
                                case LIST_UNSORTED_SET:
                                        *concrete = UNSORTED_SET_COL_BOOLEAN;
                                        return true;
                                case LIST_SORTED_SET:
                                        *concrete = SORTED_SET_COL_BOOLEAN;
                                        return true;
                                default:
                                        goto error_case;
                        }
                case LIST_COLUMN_FLOAT:
                        switch (should) {
                                case LIST_UNSORTED_MULTISET:
                                        *concrete = UNSORTED_MULTISET_COL_FLOAT;
                                        return true;
                                case LIST_SORTED_MULTISET:
                                        *concrete = SORTED_MULTISET_COL_FLOAT;
                                        return true;
                                case LIST_UNSORTED_SET:
                                        *concrete = UNSORTED_SET_COL_FLOAT;
                                        return true;
                                case LIST_SORTED_SET:
                                        *concrete = SORTED_SET_COL_FLOAT;
                                        return true;
                                default:
                                        goto error_case;
                        }
                default:
                        goto error_case;
        }
error_case:
        return error(ERR_INTERNALERR, "unknown list container type");
}

bool abstract_derive_map_to(derived_e *concrete, map_type_e should)
{
        switch (should) {
                case MAP_UNSORTED_MULTIMAP:
                        *concrete = UNSORTED_MULTIMAP;
                        return true;
                case MAP_SORTED_MULTIMAP:
                        *concrete = SORTED_MULTIMAP;
                        return true;
                case MAP_UNSORTED_MAP:
                        *concrete = UNSORTED_MAP;
                        return true;
                case MAP_SORTED_MAP:
                        *concrete = SORTED_MAP;
                        return true;
                default:
                        return error(ERR_INTERNALERR, "unknown list container type");
        }
}

bool abstract_get_derived_type(derived_e *type, u8 marker)
{
        if (!(marker == MUNSORTED_MULTIMAP || marker == MSORTED_MULTIMAP || marker == MUNSORTED_MAP ||
                       marker == MSORTED_MAP || marker == MUNSORTED_MULTISET_ARR ||
                       marker == MSORTED_MULTISET_ARR || marker == MUNSORTED_SET_ARR ||
                       marker == MSORTED_SET_ARR || marker == MUNSORTED_MULTISET_U8 ||
                       marker == MSORTED_MULTISET_U8 || marker == MUNSORTED_SET_U8 ||
                       marker == MSORTED_SET_U8 || marker == MUNSORTED_MULTISET_U16 ||
                       marker == MSORTED_MULTISET_U16 || marker == MUNSORTED_SET_U16 ||
                       marker == MSORTED_SET_U16 || marker == MUNSORTED_MULTISET_U32 ||
                       marker == MSORTED_MULTISET_U32 || marker == MUNSORTED_SET_U32 ||
                       marker == MSORTED_SET_U32 || marker == MUNSORTED_MULTISET_U64 ||
                       marker == MSORTED_MULTISET_U64 || marker == MUNSORTED_SET_U64 ||
                       marker == MSORTED_SET_U64 || marker == MUNSORTED_MULTISET_I8 ||
                       marker == MSORTED_MULTISET_I8 || marker == MUNSORTED_SET_I8 ||
                       marker == MSORTED_SET_I8 || marker == MUNSORTED_MULTISET_I16 ||
                       marker == MSORTED_MULTISET_I16 || marker == MUNSORTED_SET_I16 ||
                       marker == MSORTED_SET_I16 || marker == MUNSORTED_MULTISET_I32 ||
                       marker == MSORTED_MULTISET_I32 || marker == MUNSORTED_SET_I32 ||
                       marker == MSORTED_SET_I32 || marker == MUNSORTED_MULTISET_I64 ||
                       marker == MSORTED_MULTISET_I64 || marker == MUNSORTED_SET_I64 ||
                       marker == MSORTED_SET_I64 || marker == MUNSORTED_MULTISET_FLOAT ||
                       marker == MSORTED_MULTISET_FLOAT || marker == MUNSORTED_SET_FLOAT ||
                       marker == MSORTED_SET_FLOAT || marker == MUNSORTED_MULTISET_BOOLEAN ||
                       marker == MSORTED_MULTISET_BOOLEAN || marker == MUNSORTED_SET_BOOLEAN ||
                       marker == MSORTED_SET_BOOLEAN)) {
                return error(ERR_MARKERMAPPING, "unknown marker for abstract derived type");
        }
        *type = marker;
        return true;
}