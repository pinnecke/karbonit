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
#include <jakson/mem/file.h>

bool abstract_type(abstract_e *type, memfile *memfile)
{
        derived_e derived;
        if (LIKELY(abstract_get_derived_type(&derived, memfile))) {
                switch (derived) {
                        case UNSORTED_MULTIMAP:
                        case UNSORTED_MULTISET_ARRAY:
                        case UNSORTED_MULTISET_COL_U8:
                        case UNSORTED_MULTISET_COL_U16:
                        case UNSORTED_MULTISET_COL_U32:
                        case UNSORTED_MULTISET_COL_U64:
                        case UNSORTED_MULTISET_COL_I8:
                        case UNSORTED_MULTISET_COL_I16:
                        case UNSORTED_MULTISET_COL_I32:
                        case UNSORTED_MULTISET_COL_I64:
                        case UNSORTED_MULTISET_COL_FLOAT:
                        case UNSORTED_MULTISET_COL_BOOLEAN:
                                OPTIONAL_SET(type, ABSTRACT_BASE);
                                goto return_true;
                        case SORTED_MULTIMAP:
                        case UNSORTED_MAP:
                        case SORTED_MAP:
                        case SORTED_MULTISET_ARRAY:
                        case UNSORTED_SET_ARRAY:
                        case SORTED_SET_ARRAY:
                        case SORTED_MULTISET_COL_U8:
                        case UNSORTED_SET_COL_U8:
                        case SORTED_SET_COL_U8:
                        case SORTED_MULTISET_COL_U16:
                        case UNSORTED_SET_COL_U16:
                        case SORTED_SET_COL_U16:
                        case SORTED_MULTISET_COL_U32:
                        case UNSORTED_SET_COL_U32:
                        case SORTED_SET_COL_U32:
                        case SORTED_MULTISET_COL_U64:
                        case UNSORTED_SET_COL_U64:
                        case SORTED_SET_COL_U64:
                        case SORTED_MULTISET_COL_I8:
                        case UNSORTED_SET_COL_I8:
                        case SORTED_SET_COL_I8:
                        case SORTED_MULTISET_COL_I16:
                        case UNSORTED_SET_COL_I16:
                        case SORTED_SET_COL_I16:
                        case SORTED_MULTISET_COL_I32:
                        case UNSORTED_SET_COL_I32:
                        case SORTED_SET_COL_I32:
                        case SORTED_MULTISET_COL_I64:
                        case UNSORTED_SET_COL_I64:
                        case SORTED_SET_COL_I64:
                        case SORTED_MULTISET_COL_FLOAT:
                        case UNSORTED_SET_COL_FLOAT:
                        case SORTED_SET_COL_FLOAT:
                        case SORTED_MULTISET_COL_BOOLEAN:
                        case UNSORTED_SET_COL_BOOLEAN:
                        case SORTED_SET_COL_BOOLEAN:
                                OPTIONAL_SET(type, ABSTRACT_DERIVED);
                                goto return_true;
                        default:
                                return error(ERR_MARKERMAPPING, "unknown abstract type marker detected");
                }
return_true:
                return true;
        } else {
                return false;
        }
}

bool abstract_is_base(bool *result, memfile *memfile)
{
        abstract_e type;
        if (LIKELY(abstract_type(&type, memfile))) {
                *result = type == ABSTRACT_BASE;
                return true;
        } else {
                return false;
        }
}

bool abstract_is_derived(bool *result, memfile *memfile)
{
        bool ret = false;
        if (abstract_is_base(&ret, memfile)) {
            *result = !ret;
            return true;
        } else {
            return false;
        }
}

bool abstract_get_class(abstract_type_class_e *type, memfile *memfile)
{
        derived_e derived;
        if (LIKELY(abstract_get_derived_type(&derived, memfile))) {
                switch (derived) {
                        case SORTED_MAP:
                                *type = TYPE_SORTED_MAP;
                                goto return_true;
                        case SORTED_MULTIMAP:
                                *type = TYPE_SORTED_MULTIMAP;
                                goto return_true;
                        case SORTED_MULTISET_ARRAY:
                        case SORTED_MULTISET_COL_BOOLEAN:
                        case SORTED_MULTISET_COL_FLOAT:
                        case SORTED_MULTISET_COL_I16:
                        case SORTED_MULTISET_COL_I32:
                        case SORTED_MULTISET_COL_I64:
                        case SORTED_MULTISET_COL_I8:
                        case SORTED_MULTISET_COL_U16:
                        case SORTED_MULTISET_COL_U32:
                        case SORTED_MULTISET_COL_U64:
                        case SORTED_MULTISET_COL_U8:
                                *type = TYPE_SORTED_MULTISET;
                                goto return_true;
                        case SORTED_SET_ARRAY:
                        case SORTED_SET_COL_BOOLEAN:
                        case SORTED_SET_COL_FLOAT:
                        case SORTED_SET_COL_I16:
                        case SORTED_SET_COL_I32:
                        case SORTED_SET_COL_I64:
                        case SORTED_SET_COL_I8:
                        case SORTED_SET_COL_U16:
                        case SORTED_SET_COL_U32:
                        case SORTED_SET_COL_U64:
                        case SORTED_SET_COL_U8:
                                *type = TYPE_SORTED_SET;
                                goto return_true;
                        case UNSORTED_MAP:
                                *type = TYPE_UNSORTED_MAP;
                                goto return_true;
                        case UNSORTED_MULTIMAP:
                                *type = TYPE_UNSORTED_MULTIMAP;
                                goto return_true;
                        case UNSORTED_MULTISET_ARRAY:
                        case UNSORTED_MULTISET_COL_BOOLEAN:
                        case UNSORTED_MULTISET_COL_FLOAT:
                        case UNSORTED_MULTISET_COL_I16:
                        case UNSORTED_MULTISET_COL_I32:
                        case UNSORTED_MULTISET_COL_I64:
                        case UNSORTED_MULTISET_COL_I8:
                        case UNSORTED_MULTISET_COL_U16:
                        case UNSORTED_MULTISET_COL_U32:
                        case UNSORTED_MULTISET_COL_U64:
                        case UNSORTED_MULTISET_COL_U8:
                                *type = TYPE_UNSORTED_MULTISET;
                                goto return_true;
                        case UNSORTED_SET_ARRAY:
                        case UNSORTED_SET_COL_BOOLEAN:
                        case UNSORTED_SET_COL_FLOAT:
                        case UNSORTED_SET_COL_I16:
                        case UNSORTED_SET_COL_I32:
                        case UNSORTED_SET_COL_I64:
                        case UNSORTED_SET_COL_I8:
                        case UNSORTED_SET_COL_U16:
                        case UNSORTED_SET_COL_U32:
                        case UNSORTED_SET_COL_U64:
                        case UNSORTED_SET_COL_U8:
                                *type = TYPE_UNSORTED_SET;
                                goto return_true;
                        default:
                                return error(ERR_MARKERMAPPING, "unknown marker detected");
                }
return_true:
                return true;
        } else {
                return false;
        }
}

bool abstract_is_multiset(abstract_type_class_e type)
{
        switch (type) {
                case TYPE_UNSORTED_MULTISET:
                case TYPE_SORTED_MULTISET:
                        return true;
                default:
                        return false;
        }
}

bool abstract_is_set(abstract_type_class_e type)
{
        switch (type) {
                case TYPE_UNSORTED_SET:
                case TYPE_SORTED_SET:
                        return true;
                default:
                        return false;
        }
}

bool abstract_is_multimap(abstract_type_class_e type)
{
        switch (type) {
                case TYPE_UNSORTED_MULTIMAP:
                case TYPE_SORTED_MULTIMAP:
                        return true;
                default:
                        return false;
        }
}

bool abstract_is_map(abstract_type_class_e type)
{
        switch (type) {
                case TYPE_SORTED_MAP:
                case TYPE_UNSORTED_MAP:
                        return true;
                default:
                        return false;
        }
}

bool abstract_is_sorted(abstract_type_class_e type)
{
        switch (type) {
                case TYPE_SORTED_MULTISET:
                case TYPE_SORTED_SET:
                case TYPE_SORTED_MAP:
                case TYPE_SORTED_MULTIMAP:
                        return true;
                default:
                        return false;
        }
}

bool abstract_is_distinct(abstract_type_class_e type)
{
        switch (type) {
                case TYPE_UNSORTED_SET:
                case TYPE_SORTED_SET:
                case TYPE_SORTED_MAP:
                case TYPE_UNSORTED_MAP:
                        return true;
                default:
                        return false;
        }
}

bool abstract_class_to_list_derivable(list_type_e *out, abstract_type_class_e in)
{
        switch (in) {
                case TYPE_UNSORTED_MULTISET:
                        *out = LIST_UNSORTED_MULTISET;
                        break;
                case TYPE_SORTED_MULTISET:
                        *out = LIST_SORTED_MULTISET;
                        break;
                case TYPE_UNSORTED_SET:
                        *out = LIST_UNSORTED_SET;
                        break;
                case TYPE_SORTED_SET:
                        *out = LIST_SORTED_SET;
                        break;
                default:
                        return error(ERR_TYPEMISMATCH, "abstract class type does not encode a list type");
        }
        return true;
}

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
        memfile_write(memfile, &type, sizeof(u8));
}

void abstract_write_derived_type(memfile *memfile, derived_e type)
{
        memfile_write(memfile, &type, sizeof(u8));
}

bool abstract_get_container_subtype(sub_type_e *type, memfile *memfile)
{
        u8 marker = memfile_peek_byte(memfile);
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

static bool __abstract_is_instanceof(memfile *memfile, sub_type_e T)
{
        sub_type_e type;
        if (LIKELY(abstract_get_container_subtype(&type, memfile))) {
                return type == T;
        } else {
                return false;
        }
}

bool abstract_is_instanceof_object(memfile *memfile)
{
        return __abstract_is_instanceof(memfile, CONTAINER_OBJECT);
}

bool abstract_is_instanceof_array(memfile *memfile)
{
        return __abstract_is_instanceof(memfile, CONTAINER_ARRAY);
}

bool abstract_is_instanceof_column_u8(memfile *memfile)
{
        return __abstract_is_instanceof(memfile, CONTAINER_COLUMN_U8);
}

bool abstract_is_instanceof_column_u16(memfile *memfile)
{
        return __abstract_is_instanceof(memfile, CONTAINER_COLUMN_U16);
}

bool abstract_is_instanceof_column_u32(memfile *memfile)
{
        return __abstract_is_instanceof(memfile, CONTAINER_COLUMN_U32);
}

bool abstract_is_instanceof_column_u64(memfile *memfile)
{
        return __abstract_is_instanceof(memfile, CONTAINER_COLUMN_U64);
}

bool abstract_is_instanceof_column_i8(memfile *memfile)
{
        return __abstract_is_instanceof(memfile, CONTAINER_COLUMN_I8);
}

bool abstract_is_instanceof_column_i16(memfile *memfile)
{
        return __abstract_is_instanceof(memfile, CONTAINER_COLUMN_I16);
}

bool abstract_is_instanceof_column_i32(memfile *memfile)
{
        return __abstract_is_instanceof(memfile, CONTAINER_COLUMN_I32);
}

bool abstract_is_instanceof_column_i64(memfile *memfile)
{
        return __abstract_is_instanceof(memfile, CONTAINER_COLUMN_I64);
}

bool abstract_is_instanceof_column_float(memfile *memfile)
{
        return __abstract_is_instanceof(memfile, CONTAINER_COLUMN_FLOAT);
}

bool abstract_is_instanceof_column_boolean(memfile *memfile)
{
        return __abstract_is_instanceof(memfile, CONTAINER_COLUMN_BOOLEAN);
}

bool abstract_is_instanceof_column(memfile *memfile)
{
       if (abstract_is_instanceof_column_u8(memfile) ||
                abstract_is_instanceof_column_u16(memfile) ||
                abstract_is_instanceof_column_u32(memfile) ||
                abstract_is_instanceof_column_u64(memfile) ||
                abstract_is_instanceof_column_i8(memfile) ||
                abstract_is_instanceof_column_i16(memfile) ||
                abstract_is_instanceof_column_i32(memfile) ||
                abstract_is_instanceof_column_i64(memfile) ||
                abstract_is_instanceof_column_float(memfile) ||
                abstract_is_instanceof_column_boolean(memfile)) {
                return true;
        } else {
                return false;
        }
}

bool abstract_is_instanceof_list(memfile *memfile)
{
        if (abstract_is_instanceof_array(memfile) ||
            abstract_is_instanceof_column(memfile)) {
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

bool abstract_get_derived_type(derived_e *type, memfile *memfile)
{
        u8 c = memfile_peek_byte(memfile);
        if (!(c == MUNSORTED_MULTIMAP || c == MSORTED_MULTIMAP || c == MUNSORTED_MAP ||
                       c == MSORTED_MAP || c == MUNSORTED_MULTISET_ARR ||
                       c == MSORTED_MULTISET_ARR || c == MUNSORTED_SET_ARR ||
                       c == MSORTED_SET_ARR || c == MUNSORTED_MULTISET_U8 ||
                       c == MSORTED_MULTISET_U8 || c == MUNSORTED_SET_U8 ||
                       c == MSORTED_SET_U8 || c == MUNSORTED_MULTISET_U16 ||
                       c == MSORTED_MULTISET_U16 || c == MUNSORTED_SET_U16 ||
                       c == MSORTED_SET_U16 || c == MUNSORTED_MULTISET_U32 ||
                       c == MSORTED_MULTISET_U32 || c == MUNSORTED_SET_U32 ||
                       c == MSORTED_SET_U32 || c == MUNSORTED_MULTISET_U64 ||
                       c == MSORTED_MULTISET_U64 || c == MUNSORTED_SET_U64 ||
                       c == MSORTED_SET_U64 || c == MUNSORTED_MULTISET_I8 ||
                       c == MSORTED_MULTISET_I8 || c == MUNSORTED_SET_I8 ||
                       c == MSORTED_SET_I8 || c == MUNSORTED_MULTISET_I16 ||
                       c == MSORTED_MULTISET_I16 || c == MUNSORTED_SET_I16 ||
                       c == MSORTED_SET_I16 || c == MUNSORTED_MULTISET_I32 ||
                       c == MSORTED_MULTISET_I32 || c == MUNSORTED_SET_I32 ||
                       c == MSORTED_SET_I32 || c == MUNSORTED_MULTISET_I64 ||
                       c == MSORTED_MULTISET_I64 || c == MUNSORTED_SET_I64 ||
                       c == MSORTED_SET_I64 || c == MUNSORTED_MULTISET_FLOAT ||
                       c == MSORTED_MULTISET_FLOAT || c == MUNSORTED_SET_FLOAT ||
                       c == MSORTED_SET_FLOAT || c == MUNSORTED_MULTISET_BOOLEAN ||
                       c == MSORTED_MULTISET_BOOLEAN || c == MUNSORTED_SET_BOOLEAN ||
                       c == MSORTED_SET_BOOLEAN)) {
                return error(ERR_MARKERMAPPING, "unknown marker for abstract derived type");
        }
        *type = c;
        return true;
}