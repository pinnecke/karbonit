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

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <karbonit/carbon/dot.h>
#include <karbonit/carbon/find.h>
#include <karbonit/stdinc.h>
#include <karbonit/types.h>
#include <stdint.h>
#include <karbonit/json.h>

static void result_from_array(find *find, arr_it *it);

static void result_from_object(find *find, obj_it *it);

static inline bool
result_from_column(find *find, u32 requested_idx, col_it *it);

bool find_from_string(find *out, const char *dot, rec *doc)
{
        struct dot path;
        dot_from_string(&path, dot);
        bool ret = find_from_dot(out, &path, doc);
        dot_drop(&path);
        return ret;
}

bool find_from_dot(find *out, const dot *path, rec *doc)
{
        internal_find_exec(out, path, doc);
        return find_has_result(out);
}

bool internal_find_exec(find *find, const dot *path, rec *doc)
{
        ZERO_MEMORY(find, sizeof(find));
        find->doc = doc;

        dot_eval_begin(&find->eval, path, doc);
        if (find_has_result(find)) {
                switch (find->eval.result.container) {
                        case ARRAY:
                                result_from_array(find, &find->eval.result.containers.array);
                                break;
                        case COLUMN:
                                result_from_column(find, find->eval.result.containers.column.elem_pos,
                                                   &find->eval.result.containers.column.it);
                                break;
                        case OBJECT:
                                result_from_object(find, &find->eval.result.containers.object);
                                break;
                        default:
                                return ERROR(ERR_INTERNALERR, "unknown container type");
                }
        }
        return find_has_result(find);
}

bool find_has_result(find *find)
{
        return DOT_EVAL_HAS_RESULT(&find->eval);
}

const char *find_result_to_str(str_buf *dst_str, find *find)
{
        if (!find_has_result(find)) {
                /* undefined */
                json_from_undef(dst_str);
        } else {
                if (find_result_is_array(find)) {
                        json_from_array(dst_str, find_result_array(find));
                } else if (find_result_is_object(find)) {
                        json_from_object(dst_str, find_result_object(find));
                } else if (find_result_is_column(find)) {
                        json_from_column(dst_str, find_result_column(find));
                } else if (find_result_is_binary(find)) {
                        json_from_binary(dst_str, find_result_binary(find));
                } else if (find_result_is_boolean(find)) {
                        bool val;
                        find_result_boolean(&val, find);
                        json_from_boolean(dst_str, val);
                } else if (find_result_is_unsigned(find)) {
                        u64 val;
                        find_result_unsigned(&val, find);
                        json_from_unsigned(dst_str, val);
                } else if (find_result_is_signed(find)) {
                        i64 val;
                        find_result_signed(&val, find);
                        json_from_signed(dst_str, val);
                } else if (find_result_is_float(find)) {
                        float val;
                        find_result_float(&val, find);
                        json_from_float(dst_str, val);
                } else if (find_result_is_string(find)) {
                        string_field sf;
                        sf.str = find_result_string(&sf.len, find);
                        json_from_string(dst_str, &sf);
                } else if (find_result_is_null(find)) {
                        json_from_null(dst_str);
                } else {
                        /* undefined */
                        json_from_undef(dst_str);
                }
        }
        return str_buf_cstr(dst_str);
}

bool find_result_type(field_e *type, find *find)
{
        if(!DOT_EVAL_HAS_RESULT(&find->eval)) {
            return false;
        } else {
            *type = find->type;
            return true;
        }
}

bool find_update_array_type(find *find, list_type_e derivation)
{
        field_e type;
        find_result_type(&type, find);
        if (FIELD_IS_ARRAY_OR_SUBTYPE(type)) {
                memfile mod;
                arr_it *it = find_result_array(find);
                MEMFILE_CLONE(&mod, &it->file);
                MEMFILE_SEEK_FROM_HERE(&mod, -sizeof(u8));
                derived_e derive_marker = abstract_derive_list_to(LIST_ARRAY, derivation);
                abstract_write_derived_type(&mod, derive_marker);
                return true;

        } else {
                return ERROR(ERR_TYPEMISMATCH, "find: array type update must be invoked on array or sub type");
        }
}

bool find_array_is_multiset(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (FIELD_IS_ARRAY_OR_SUBTYPE(type)) {
                arr_it *it = find_result_array(find);
                return arr_it_is_multiset(it);
        } else {
                return ERROR(ERR_TYPEMISMATCH, "find: array type query must be invoked on array or sub type");
        }
}

bool find_array_is_sorted(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (FIELD_IS_ARRAY_OR_SUBTYPE(type)) {
                arr_it *it = find_result_array(find);
                return arr_it_is_sorted(it);
        } else {
                return ERROR(ERR_TYPEMISMATCH, "find: array type query must be invoked on array or sub type");
        }
}

bool find_update_column_type(find *find, list_type_e derivation)
{
        field_e type;
        find_result_type(&type, find);
        if (FIELD_IS_COLUMN_OR_SUBTYPE(type)) {
                col_it *it = find_result_column(find);
                MEMFILE_SAVE_POSITION(&it->file);
                MEMFILE_SEEK(&it->file, it->begin);

                list_container_e list_container;
                list_by_column_type(&list_container, it->field_type);
                derived_e derive_marker = abstract_derive_list_to(list_container, derivation);
                abstract_write_derived_type(&it->file, derive_marker);

                MEMFILE_RESTORE_POSITION(&it->file);
                return true;
        } else {
                return ERROR(ERR_TYPEMISMATCH, "find: column type update must be invoked on column or sub type");
        }
}

bool find_column_is_multiset(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (FIELD_IS_COLUMN_OR_SUBTYPE(type)) {
                col_it *it = find_result_column(find);
                return col_it_is_multiset(it);
        } else {
                return ERROR(ERR_TYPEMISMATCH, "find: column query must be invoked on column or sub type");
        }
}

bool find_column_is_sorted(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (FIELD_IS_COLUMN_OR_SUBTYPE(type)) {
                col_it *it = find_result_column(find);
                return col_it_is_sorted(it);
        } else {
                return ERROR(ERR_TYPEMISMATCH, "find: column query must be invoked on column or sub type");
        }
}

bool find_update_object_type(find *find, map_type_e derivation)
{
        field_e type;
        find_result_type(&type, find);
        if (FIELD_IS_OBJECT_OR_SUBTYPE(type)) {
                obj_it *it = find_result_object(find);
                MEMFILE_SAVE_POSITION(&it->file);
                MEMFILE_SEEK(&it->file, it->begin);

                derived_e derive_marker = abstract_derive_map_to(derivation);
                abstract_write_derived_type(&it->file, derive_marker);

                MEMFILE_RESTORE_POSITION(&it->file);
                return true;

        } else {
                return ERROR(ERR_TYPEMISMATCH, "find: object type update must be invoked on object or sub type");
        }
}

bool find_object_is_multimap(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (FIELD_IS_OBJECT_OR_SUBTYPE(type)) {
                obj_it *it = find_result_object(find);
                return obj_it_is_multimap(it);
        } else {
                return ERROR(ERR_TYPEMISMATCH, "find: object query must be invoked on object or sub type");
        }
}

bool find_object_is_sorted(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (FIELD_IS_OBJECT_OR_SUBTYPE(type)) {
                obj_it *it = find_result_object(find);
                return obj_it_is_sorted(it);
        } else {
                return ERROR(ERR_TYPEMISMATCH, "find: object query must be invoked on object or sub type");
        }
}

bool find_multimap(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (FIELD_IS_OBJECT_OR_SUBTYPE(type)) {
                return find_object_is_multimap(find);
        } else {
                return false;
        }
}

bool find_multiset(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (FIELD_IS_ARRAY_OR_SUBTYPE(type)) {
                return find_array_is_multiset(find);
        } else if (FIELD_IS_COLUMN_OR_SUBTYPE(type)) {
                return find_column_is_multiset(find);
        } else {
                return false;
        }
}

bool find_sorted(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (FIELD_IS_ARRAY_OR_SUBTYPE(type)) {
                return find_array_is_sorted(find);
        } else if (FIELD_IS_COLUMN_OR_SUBTYPE(type)) {
                return find_column_is_sorted(find);
        } else {
                return find_object_is_sorted(find);
        }
}

bool find_result_is_array(find *find)
{
        return FIELD_IS_ARRAY_OR_SUBTYPE(find->type);
}

bool find_result_is_object(find *find)
{
        return FIELD_IS_OBJECT_OR_SUBTYPE(find->type);
}

bool find_result_is_column(find *find)
{
        return FIELD_IS_COLUMN_OR_SUBTYPE(find->type);
}

bool find_result_is_boolean(find *find)
{
        return FIELD_IS_BOOLEAN(find->type);
}

bool find_result_is_undefined(find *find)
{
        return !find_has_result(find);
}

bool find_result_is_unsigned(find *find)
{
        return FIELD_IS_UNSIGNED(find->type);
}

bool find_result_is_signed(find *find)
{
        return FIELD_IS_SIGNED(find->type);
}

bool find_result_is_float(find *find)
{
        return FIELD_IS_FLOATING(find->type);
}

bool find_result_is_null(find *find)
{
        return FIELD_IS_NULL(find->type);
}

bool find_result_is_string(find *find)
{
        return FIELD_IS_STRING(find->type);
}

bool find_result_is_binary(find *find)
{
        return FIELD_IS_BINARY(find->type);
}

bool __check_path_evaluator_has_result(find *find)
{
        assert(find);
        if (UNLIKELY(!(DOT_EVAL_HAS_RESULT(&find->eval)))) {
                return ERROR(ERR_ILLEGALSTATE, "no path evaluation result available");
        } else {
                return true;
        }
}

arr_it *find_result_array(find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return NULL;
        }

        if (UNLIKELY(!FIELD_IS_ARRAY_OR_SUBTYPE(find->type))) {
                ERROR(ERR_TYPEMISMATCH, "container must be array or sub type");
                return NULL;
        }

        return &find->value.array;
}

obj_it *find_result_object(find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return NULL;
        }

        if (UNLIKELY(!FIELD_IS_OBJECT_OR_SUBTYPE(find->type))) {
                ERROR(ERR_TYPEMISMATCH, "container must be object or sub type");
                return NULL;
        }

        return &find->value.object;
}

col_it *find_result_column(find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return NULL;
        }

        if (UNLIKELY(!FIELD_IS_COLUMN_OR_SUBTYPE(find->type))) {
                ERROR(ERR_TYPEMISMATCH, "container must be column or sub type");
                return NULL;
        }

        return &find->value.column;
}

bool find_result_boolean(bool *result, find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return false;
        }

        if (UNLIKELY(!FIELD_IS_BOOLEAN(find->type))) {
                return ERROR(ERR_TYPEMISMATCH, "result value must be of boolean type");
        }

        *result = find->value.boolean;
        return true;
}

bool find_result_unsigned(u64 *out, find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return false;
        }

        if (UNLIKELY(!FIELD_IS_UNSIGNED(find->type))) {
                return ERROR(ERR_TYPEMISMATCH, "result value must be of unsigned type");
        }

        *out = find->value.unsigned_number;
        return true;
}

bool find_result_signed(i64 *out, find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return false;
        }

        if (UNLIKELY(!FIELD_IS_SIGNED(find->type))) {
                return ERROR(ERR_TYPEMISMATCH, "result value must be of signed type");
        }

        *out = find->value.signed_number;
        return true;
}

bool find_result_float(float *out, find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return false;
        }

        if (UNLIKELY(!FIELD_IS_FLOATING(find->type))) {
                return ERROR(ERR_TYPEMISMATCH, "result value must be of float type");
        }

        *out = find->value.float_number;
        return true;
}

const char *find_result_string(u64 *str_len, find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return NULL;
        }

        if (UNLIKELY(!FIELD_IS_STRING(find->type))) {
                ERROR(ERR_TYPEMISMATCH, "result value must be of string type");
                return NULL;
        }
        *str_len = find->value.string.len;
        return find->value.string.str;
}

binary_field *find_result_binary(find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return NULL;
        }

        if (UNLIKELY(!FIELD_IS_BINARY(find->type))) {
                ERROR(ERR_TYPEMISMATCH, "result value must be of binary type");
                return NULL;
        }

        return &find->value.binary;
}

static void result_from_array(find *find, arr_it *it)
{
        find->type = it->field.type;
        switch (find->type) {
                case FIELD_NULL:
                        /** no value to be stored */
                        break;
                case FIELD_TRUE:
                case FIELD_FALSE:
                        find->value.boolean = find->type == FIELD_TRUE ? true : false;
                        break;
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET:
                        ITEM_GET_ARRAY(&find->value.array, &(it->item));
                        find->value.array.file.mode = find->doc->file.mode;
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
                        ITEM_GET_COLUMN(&find->value.column, &(it->item));
                        find->value.column.file.mode = find->doc->file.mode;
                        break;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP:
                        ITEM_GET_OBJECT(&find->value.object, &(it->item));
                        find->value.object.file.mode = find->doc->file.mode;
                        break;
                case FIELD_STRING:
                        find->value.string = ITEM_GET_STRING(&(it->item), NULL_STRING);
                        break;
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_U64:
                        find->value.unsigned_number = ITEM_GET_NUMBER_UNSIGNED(&(it->item), CARBON_NULL_UNSIGNED);
                        break;
                case FIELD_NUMBER_I8:
                case FIELD_NUMBER_I16:
                case FIELD_NUMBER_I32:
                case FIELD_NUMBER_I64:
                        find->value.signed_number = ITEM_GET_NUMBER_SIGNED(&(it->item), CARBON_NULL_SIGNED);
                        break;
                case FIELD_NUMBER_FLOAT:
                        find->value.float_number = ITEM_GET_NUMBER_FLOAT(&(it->item), CARBON_NULL_FLOAT);
                        break;
                case FIELD_BINARY:
                case FIELD_BINARY_CUSTOM:
                        find->value.binary = ITEM_GET_BINARY(&(it->item), NULL_BINARY);
                        break;
                default: ERROR(ERR_INTERNALERR, NULL);
                        break;
        }
}

static void result_from_object(find *find, obj_it *it)
{
        internal_obj_it_prop_type(&find->type, it);
        switch (find->type) {
                case FIELD_NULL:
                        /** no value to be stored */
                        break;
                case FIELD_TRUE:
                case FIELD_FALSE:
                        find->value.boolean = find->type == FIELD_TRUE ? true : false;
                        break;
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET:
                        ITEM_GET_ARRAY(&find->value.array, &(it->prop.value));
                        find->value.array.file.mode = find->doc->file.mode;
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
                        ITEM_GET_COLUMN(&find->value.column, &(it->prop.value));
                        find->value.column.file.mode = find->doc->file.mode;
                        break;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP:
                        ITEM_GET_OBJECT(&find->value.object, &(it->prop.value));
                        find->value.object.file.mode = find->doc->file.mode;
                        break;
                case FIELD_STRING:
                        find->value.string = ITEM_GET_STRING(&(it->prop.value), NULL_STRING);
                        break;
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_U64:
                        find->value.unsigned_number = ITEM_GET_NUMBER_UNSIGNED(&(it->prop.value), CARBON_NULL_UNSIGNED);
                        break;
                case FIELD_NUMBER_I8:
                case FIELD_NUMBER_I16:
                case FIELD_NUMBER_I32:
                case FIELD_NUMBER_I64:
                        find->value.signed_number = ITEM_GET_NUMBER_SIGNED(&(it->prop.value), CARBON_NULL_SIGNED);
                        break;
                case FIELD_NUMBER_FLOAT:
                        find->value.float_number = ITEM_GET_NUMBER_FLOAT(&(it->prop.value), CARBON_NULL_FLOAT);
                        break;
                case FIELD_BINARY:
                case FIELD_BINARY_CUSTOM:
                        find->value.binary = ITEM_GET_BINARY(&(it->prop.value), NULL_BINARY);
                        break;
                default: ERROR(ERR_INTERNALERR, NULL);
                        break;
        }
}

static inline bool
result_from_column(find *find, u32 requested_idx, col_it *it)
{
#ifndef NDEBUG
        u32 max_idx = COL_IT_VALUES_INFO(&find->type, it);
        assert(requested_idx < max_idx);
#endif

        switch (find->type) {
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                        u8 field_value = COL_IT_BOOLEAN_VALUES(NULL, it)[requested_idx];
                        if (IS_NULL_BOOLEAN(field_value)) {
                                find->type = FIELD_NULL;
                        } else if (field_value == CARBON_BOOLEAN_COLUMN_TRUE) {
                                find->type = FIELD_TRUE;
                        } else if (field_value == CARBON_BOOLEAN_COLUMN_FALSE) {
                                find->type = FIELD_FALSE;
                        } else {
                                ERROR(ERR_INTERNALERR, NULL);
                        }
                }
                        break;
                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_SET: {
                        u8 field_value = COL_IT_U8_VALUES(NULL, it)[requested_idx];
                        if (IS_NULL_U8(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_U8;
                                find->value.unsigned_number = field_value;
                        }
                }
                        break;
                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_SET: {
                        u16 field_value = COL_IT_U16_VALUES(NULL, it)[requested_idx];
                        if (IS_NULL_U16(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_U16;
                                find->value.unsigned_number = field_value;
                        }
                }
                        break;
                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_SET: {
                        u32 field_value = COL_IT_U32_VALUES(NULL, it)[requested_idx];
                        if (IS_NULL_U32(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_U32;
                                find->value.unsigned_number = field_value;
                        }
                }
                        break;
                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_SET: {
                        u64 field_value = COL_IT_U64_VALUES(NULL, it)[requested_idx];
                        if (IS_NULL_U64(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_U64;
                                find->value.unsigned_number = field_value;
                        }
                }
                        break;
                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_SET: {
                        i8 field_value = COL_IT_I8_VALUES(NULL, it)[requested_idx];
                        if (IS_NULL_I8(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_I8;
                                find->value.signed_number = field_value;
                        }
                }
                        break;
                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_SET: {
                        i16 field_value = COL_IT_I16_VALUES(NULL, it)[requested_idx];
                        if (IS_NULL_I16(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_I16;
                                find->value.signed_number = field_value;
                        }
                }
                        break;
                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_SET: {
                        i32 field_value = COL_IT_I32_VALUES(NULL, it)[requested_idx];
                        if (IS_NULL_I32(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_I32;
                                find->value.signed_number = field_value;
                        }
                }
                        break;
                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET: {
                        i64 field_value = COL_IT_I64_VALUES(NULL, it)[requested_idx];
                        if (IS_NULL_I64(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_I64;
                                find->value.signed_number = field_value;
                        }
                }
                        break;
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET: {
                        float field_value = COL_IT_FLOAT_VALUES(NULL, it)[requested_idx];
                        if (IS_NULL_FLOAT(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_FLOAT;
                                find->value.float_number = field_value;
                        }
                }
                        break;
                default:
                        return ERROR(ERR_UNSUPPORTEDTYPE, NULL);
        }
        return true;
}