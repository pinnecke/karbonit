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

#include <jakson/carbon/dot.h>
#include <jakson/carbon/find.h>
#include <jakson/stdinc.h>
#include <jakson/types.h>
#include <stdint.h>

static void result_from_array(find *find, arr_it *it);

static void result_from_object(find *find, obj_it *it);

static inline bool
result_from_column(find *find, u32 requested_idx, col_it *it);

bool find_begin(find *out, const char *dot, rec *doc)
{
        struct dot path;
        dot_from_string(&path, dot);
        find_create(out, &path, doc);
        dot_drop(&path);
        return true;
}

bool find_end(find *find)
{
        if (find_has_result(find)) {
                field_e type;
                find_result_type(&type, find);
                switch (type) {
                        case FIELD_OBJECT_UNSORTED_MULTIMAP:
                        case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                        case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                        case FIELD_DERIVED_OBJECT_SORTED_MAP:
                                carbon_object_drop(find->value.object);
                                break;
                        case FIELD_ARRAY_UNSORTED_MULTISET:
                        case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                        case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                        case FIELD_DERIVED_ARRAY_SORTED_SET:
                                arr_it_drop(find->value.array);
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
                                break;
                        default:
                                break;
                }
                return find_drop(find);
        }
        return true;
}

bool find_create(find *find, dot *path, rec *doc)
{
        ZERO_MEMORY(find, sizeof(find));
        find->doc = doc;

        dot_eval_begin(&find->eval, path, doc);
        if (dot_eval_has_result(&find->eval)) {
                switch (find->eval.result.container) {
                        case ARRAY:
                                result_from_array(find, &find->eval.result.containers.array.it);
                                break;
                        case COLUMN:
                                result_from_column(find, find->eval.result.containers.column.elem_pos,
                                                   &find->eval.result.containers.column.it);
                                break;
                        case OBJECT:
                                result_from_object(find, &find->eval.result.containers.object.it);
                                break;
                        default:
                                return error(ERR_INTERNALERR, "unknown container type");
                }
        }
        return true;
}

bool find_has_result(find *find)
{
        return dot_eval_has_result(&find->eval);
}

const char *find_result_to_str(string_buffer *dst_str, carbon_printer_impl_e print_type, find *find)
{
        string_buffer_clear(dst_str);

        carbon_printer printer;
        carbon_printer_by_type(&printer, print_type);

        if (find_has_result(find)) {
                field_e result_type;
                find_result_type(&result_type, find);
                switch (result_type) {
                        case FIELD_NULL:
                                carbon_printer_null(&printer, dst_str);
                                break;
                        case FIELD_TRUE:
                                carbon_printer_true(&printer, false, dst_str);
                                break;
                        case FIELD_FALSE:
                                carbon_printer_false(&printer, false, dst_str);
                                break;
                        case FIELD_OBJECT_UNSORTED_MULTIMAP:
                        case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                        case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                        case FIELD_DERIVED_OBJECT_SORTED_MAP: {
                                obj_it *sub_it = find_result_object(find);
                                carbon_printer_print_object(sub_it, &printer, dst_str);
                        }
                                break;
                        case FIELD_ARRAY_UNSORTED_MULTISET:
                        case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                        case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                        case FIELD_DERIVED_ARRAY_SORTED_SET: {
                                arr_it *sub_it = find_result_array(find);
                                carbon_printer_print_array(sub_it, &printer, dst_str, false);
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
                                col_it *sub_it = find_result_column(find);
                                carbon_printer_print_column(sub_it, &printer, dst_str);
                        }
                                break;
                        case FIELD_STRING: {
                                u64 str_len = 0;
                                const char *str = find_result_string(&str_len, find);
                                carbon_printer_string(&printer, dst_str, str, str_len);
                        }
                                break;
                        case FIELD_NUMBER_U8: {
                                u64 val = 0;
                                find_result_unsigned(&val, find);
                                carbon_printer_u8_or_null(&printer, dst_str, (u8) val);
                        }
                                break;
                        case FIELD_NUMBER_U16: {
                                u64 val = 0;
                                find_result_unsigned(&val, find);
                                carbon_printer_u16_or_null(&printer, dst_str, (u16) val);
                        }
                                break;
                        case FIELD_NUMBER_U32: {
                                u64 val = 0;
                                find_result_unsigned(&val, find);
                                carbon_printer_u32_or_null(&printer, dst_str, (u32) val);
                        }
                                break;
                        case FIELD_NUMBER_U64: {
                                u64 val = 0;
                                find_result_unsigned(&val, find);
                                carbon_printer_u64_or_null(&printer, dst_str, (u64) val);
                        }
                                break;
                        case FIELD_NUMBER_I8: {
                                i64 val = 0;
                                find_result_signed(&val, find);
                                carbon_printer_i8_or_null(&printer, dst_str, (i8) val);
                        }
                                break;
                        case FIELD_NUMBER_I16: {
                                i64 val = 0;
                                find_result_signed(&val, find);
                                carbon_printer_i16_or_null(&printer, dst_str, (i16) val);
                        }
                                break;
                        case FIELD_NUMBER_I32: {
                                i64 val = 0;
                                find_result_signed(&val, find);
                                carbon_printer_i32_or_null(&printer, dst_str, (i32) val);
                        }
                                break;
                        case FIELD_NUMBER_I64: {
                                i64 val = 0;
                                find_result_signed(&val, find);
                                carbon_printer_i64_or_null(&printer, dst_str, (i64) val);
                        }
                                break;
                        case FIELD_NUMBER_FLOAT: {
                                float val = 0;
                                find_result_float(&val, find);
                                carbon_printer_float(&printer, dst_str, &val);
                        }
                                break;
                        case FIELD_BINARY:
                        case FIELD_BINARY_CUSTOM: {
                                const binary_field *val = find_result_binary(find);
                                carbon_printer_binary(&printer, dst_str, val);
                        }
                                break;
                        default:
                                error(ERR_INTERNALERR, "unknown field type");
                                return NULL;
                }

        } else {
                string_buffer_add(dst_str, CARBON_NIL_STR);
        }
        carbon_printer_drop(&printer);

        return string_cstr(dst_str);
}

const char *find_result_to_json_compact(string_buffer *dst_str, find *find)
{
        return find_result_to_str(dst_str, JSON_COMPACT, find);
}

char *find_result_to_json_compact_dup(find *find)
{
        string_buffer str;
        string_buffer_create(&str);
        char *ret = strdup(find_result_to_json_compact(&str, find));
        string_buffer_drop(&str);
        return ret;
}

bool find_result_type(field_e *type, find *find)
{
        if(!dot_eval_has_result(&find->eval)) {
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
        if (field_is_array_or_subtype(type)) {
                memfile mod;
                arr_it *it = find_result_array(find);
                memfile_clone(&mod, &it->file);
                memfile_seek_from_here(&mod, -sizeof(u8));
                derived_e derive_marker;
                abstract_derive_list_to(&derive_marker, LIST_ARRAY, derivation);
                abstract_write_derived_type(&mod, derive_marker);
                return true;

        } else {
                return error(ERR_TYPEMISMATCH, "find: array type update must be invoked on array or sub type");
        }
}

bool find_array_is_multiset(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (field_is_array_or_subtype(type)) {
                arr_it *it = find_result_array(find);
                return arr_it_is_multiset(it);
        } else {
                return error(ERR_TYPEMISMATCH, "find: array type query must be invoked on array or sub type");
        }
}

bool find_array_is_sorted(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (field_is_array_or_subtype(type)) {
                arr_it *it = find_result_array(find);
                return arr_it_is_sorted(it);
        } else {
                return error(ERR_TYPEMISMATCH, "find: array type query must be invoked on array or sub type");
        }
}

bool find_update_column_type(find *find, list_type_e derivation)
{
        field_e type;
        find_result_type(&type, find);
        if (field_is_column_or_subtype(type)) {
                col_it *it = find_result_column(find);
                memfile_save_position(&it->file);
                memfile_seek(&it->file, it->begin);

                derived_e derive_marker;
                list_container_e list_container;
                list_by_column_type(&list_container, it->field_type);
                abstract_derive_list_to(&derive_marker, list_container, derivation);
                abstract_write_derived_type(&it->file, derive_marker);

                memfile_restore_position(&it->file);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, "find: column type update must be invoked on column or sub type");
        }
}

bool find_column_is_multiset(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (field_is_column_or_subtype(type)) {
                col_it *it = find_result_column(find);
                return col_it_is_multiset(it);
        } else {
                return error(ERR_TYPEMISMATCH, "find: column query must be invoked on column or sub type");
        }
}

bool find_column_is_sorted(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (field_is_column_or_subtype(type)) {
                col_it *it = find_result_column(find);
                return col_it_is_sorted(it);
        } else {
                return error(ERR_TYPEMISMATCH, "find: column query must be invoked on column or sub type");
        }
}

bool find_update_object_type(find *find, map_type_e derivation)
{
        field_e type;
        find_result_type(&type, find);
        if (field_is_object_or_subtype(type)) {
                obj_it *it = find_result_object(find);
                memfile_save_position(&it->memfile);
                memfile_seek(&it->memfile, it->object_start_off);

                derived_e derive_marker;
                abstract_derive_map_to(&derive_marker, derivation);
                abstract_write_derived_type(&it->memfile, derive_marker);

                memfile_restore_position(&it->memfile);
                return true;

        } else {
                return error(ERR_TYPEMISMATCH, "find: object type update must be invoked on object or sub type");
        }
}

bool find_object_is_multimap(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (field_is_object_or_subtype(type)) {
                obj_it *it = find_result_object(find);
                return carbon_object_is_multimap(it);
        } else {
                return error(ERR_TYPEMISMATCH, "find: object query must be invoked on object or sub type");
        }
}

bool find_object_is_sorted(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (field_is_object_or_subtype(type)) {
                obj_it *it = find_result_object(find);
                return carbon_object_is_sorted(it);
        } else {
                return error(ERR_TYPEMISMATCH, "find: object query must be invoked on object or sub type");
        }
}

bool find_multimap(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (field_is_object_or_subtype(type)) {
                return find_object_is_multimap(find);
        } else {
                return false;
        }
}

bool find_multiset(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (field_is_array_or_subtype(type)) {
                return find_array_is_multiset(find);
        } else if (field_is_column_or_subtype(type)) {
                return find_column_is_multiset(find);
        } else {
                return false;
        }
}

bool find_sorted(find *find)
{
        field_e type;
        find_result_type(&type, find);
        if (field_is_array_or_subtype(type)) {
                return find_array_is_sorted(find);
        } else if (field_is_column_or_subtype(type)) {
                return find_column_is_sorted(find);
        } else {
                return find_object_is_sorted(find);
        }
}


bool __check_path_evaluator_has_result(find *find)
{
        assert(find);
        if (UNLIKELY(!(dot_eval_has_result(&find->eval)))) {
                return error(ERR_ILLEGALSTATE, "no path evaluation result available");
        } else {
                return true;
        }
}

arr_it *find_result_array(find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return NULL;
        }

        if (UNLIKELY(!field_is_array_or_subtype(find->type))) {
                error(ERR_TYPEMISMATCH, "container must be array or sub type");
                return NULL;
        }

        return find->value.array;
}

obj_it *find_result_object(find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return NULL;
        }

        if (UNLIKELY(!field_is_object_or_subtype(find->type))) {
                error(ERR_TYPEMISMATCH, "container must be object or sub type");
                return NULL;
        }

        return find->value.object;
}

col_it *find_result_column(find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return NULL;
        }

        if (UNLIKELY(!field_is_column_or_subtype(find->type))) {
                error(ERR_TYPEMISMATCH, "container must be column or sub type");
                return NULL;
        }

        return find->value.column;
}

bool find_result_boolean(bool *result, find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return false;
        }

        if (UNLIKELY(!field_is_boolean(find->type))) {
                return error(ERR_TYPEMISMATCH, "result value must be of boolean type");
        }

        *result = find->value.boolean;
        return true;
}

bool find_result_unsigned(u64 *out, find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return false;
        }

        if (UNLIKELY(!field_is_unsigned(find->type))) {
                return error(ERR_TYPEMISMATCH, "result value must be of unsigned type");
        }

        *out = find->value.unsigned_number;
        return true;
}

bool find_result_signed(i64 *out, find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return false;
        }

        if (UNLIKELY(!field_is_signed(find->type))) {
                return error(ERR_TYPEMISMATCH, "result value must be of signed type");
        }

        *out = find->value.signed_number;
        return true;
}

bool find_result_float(float *out, find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return false;
        }

        if (UNLIKELY(!field_is_floating(find->type))) {
                return error(ERR_TYPEMISMATCH, "result value must be of float type");
        }

        *out = find->value.float_number;
        return true;
}

const char *find_result_string(u64 *str_len, find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return NULL;
        }

        if (UNLIKELY(!field_is_string(find->type))) {
                error(ERR_TYPEMISMATCH, "result value must be of string type");
                return NULL;
        }
        *str_len = find->value.string.length;
        return find->value.string.string;
}

binary_field *find_result_binary(find *find)
{
        if (!__check_path_evaluator_has_result(find)) {
            return NULL;
        }

        if (UNLIKELY(!field_is_binary(find->type))) {
                error(ERR_TYPEMISMATCH, "result value must be of binary type");
                return NULL;
        }

        return &find->value.binary;
}

bool find_drop(find *find)
{
        return dot_eval_end(&find->eval);
}

static void result_from_array(find *find, arr_it *it)
{
        find->type = it->field.type;
        switch (find->type) {
                case FIELD_NULL:
                case FIELD_TRUE:
                case FIELD_FALSE:
                        /** no value to be stored */
                        break;
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET:
                        find->value.array = item_get_array(&(it->item));
                        find->value.array->file.mode = find->doc->file.mode;
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
                        find->value.column = item_get_column(&(it->item));
                        find->value.column->file.mode = find->doc->file.mode;
                        break;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP:
                        find->value.object = item_get_object(&(it->item));
                        find->value.object->memfile.mode = find->doc->file.mode;
                        break;
                case FIELD_STRING:
                        find->value.string = item_get_string(&(it->item), CARBON_NULL_STRING);
                        break;
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_U64:
                        find->value.unsigned_number = item_get_number_unsigned(&(it->item), CARBON_NULL_UNSIGNED);
                        break;
                case FIELD_NUMBER_I8:
                case FIELD_NUMBER_I16:
                case FIELD_NUMBER_I32:
                case FIELD_NUMBER_I64:
                        find->value.signed_number = item_get_number_signed(&(it->item), CARBON_NULL_SIGNED);
                        break;
                case FIELD_NUMBER_FLOAT:
                        find->value.float_number = item_get_number_float(&(it->item), CARBON_NULL_FLOAT);
                        break;
                case FIELD_BINARY:
                case FIELD_BINARY_CUSTOM:
                        find->value.binary = item_get_binary(&(it->item), NULL_BINARY);
                        break;
                default: error(ERR_INTERNALERR, NULL);
                        break;
        }
}

static void result_from_object(find *find, obj_it *it)
{
        internal_carbon_object_prop_type(&find->type, it);
        switch (find->type) {
                case FIELD_NULL:
                case FIELD_TRUE:
                case FIELD_FALSE:
                        /** no value to be stored */
                        break;
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET:
                        find->value.array = item_get_array(&(it->prop.value));
                        find->value.array->file.mode = find->doc->file.mode;
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
                        find->value.column = item_get_column(&(it->prop.value));
                        find->value.column->file.mode = find->doc->file.mode;
                        break;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP:
                        find->value.object = item_get_object(&(it->prop.value));
                        find->value.object->memfile.mode = find->doc->file.mode;
                        break;
                case FIELD_STRING:
                        find->value.string = item_get_string(&(it->prop.value), CARBON_NULL_STRING);
                        break;
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_U64:
                        find->value.unsigned_number = item_get_number_unsigned(&(it->prop.value), CARBON_NULL_UNSIGNED);
                        break;
                case FIELD_NUMBER_I8:
                case FIELD_NUMBER_I16:
                case FIELD_NUMBER_I32:
                case FIELD_NUMBER_I64:
                        find->value.signed_number = item_get_number_signed(&(it->prop.value), CARBON_NULL_SIGNED);
                        break;
                case FIELD_NUMBER_FLOAT:
                        find->value.float_number = item_get_number_float(&(it->prop.value), CARBON_NULL_FLOAT);
                        break;
                case FIELD_BINARY:
                case FIELD_BINARY_CUSTOM:
                        find->value.binary = item_get_binary(&(it->prop.value), NULL_BINARY);
                        break;
                default: error(ERR_INTERNALERR, NULL);
                        break;
        }
}

static inline bool
result_from_column(find *find, u32 requested_idx, col_it *it)
{
        u32 num_contained_values;
        col_it_values_info(&find->type, &num_contained_values, it);
        JAK_ASSERT(requested_idx < num_contained_values);

        switch (find->type) {
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                        u8 field_value = col_it_boolean_values(NULL, it)[requested_idx];
                        if (IS_NULL_BOOLEAN(field_value)) {
                                find->type = FIELD_NULL;
                        } else if (field_value == CARBON_BOOLEAN_COLUMN_TRUE) {
                                find->type = FIELD_TRUE;
                        } else if (field_value == CARBON_BOOLEAN_COLUMN_FALSE) {
                                find->type = FIELD_FALSE;
                        } else {
                                error(ERR_INTERNALERR, NULL);
                        }
                }
                        break;
                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U8_SORTED_SET: {
                        u8 field_value = col_it_u8_values(NULL, it)[requested_idx];
                        if (IS_NULL_U8(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_U8;
                                find->value.unsigned_number = col_it_u8_values(NULL, it)[requested_idx];
                        }
                }
                        break;
                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U16_SORTED_SET: {
                        u16 field_value = col_it_u16_values(NULL, it)[requested_idx];
                        if (IS_NULL_U16(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_U16;
                                find->value.unsigned_number = col_it_u16_values(NULL, it)[requested_idx];
                        }
                }
                        break;
                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U32_SORTED_SET: {
                        u32 field_value = col_it_u32_values(NULL, it)[requested_idx];
                        if (IS_NULL_U32(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_U32;
                                find->value.unsigned_number = col_it_u32_values(NULL, it)[requested_idx];
                        }
                }
                        break;
                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_U64_SORTED_SET: {
                        u64 field_value = col_it_u64_values(NULL, it)[requested_idx];
                        if (IS_NULL_U64(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_U64;
                                find->value.unsigned_number = col_it_u64_values(NULL, it)[requested_idx];
                        }
                }
                        break;
                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I8_SORTED_SET: {
                        i8 field_value = col_it_i8_values(NULL, it)[requested_idx];
                        if (IS_NULL_I8(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_I8;
                                find->value.signed_number = col_it_i8_values(NULL, it)[requested_idx];
                        }
                }
                        break;
                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I16_SORTED_SET: {
                        i16 field_value = col_it_i16_values(NULL, it)[requested_idx];
                        if (IS_NULL_I16(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_I16;
                                find->value.signed_number = col_it_i16_values(NULL, it)[requested_idx];
                        }
                }
                        break;
                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I32_SORTED_SET: {
                        i32 field_value = col_it_i32_values(NULL, it)[requested_idx];
                        if (IS_NULL_I32(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_I32;
                                find->value.signed_number = col_it_i32_values(NULL, it)[requested_idx];
                        }
                }
                        break;
                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET: {
                        i64 field_value = col_it_i64_values(NULL, it)[requested_idx];
                        if (IS_NULL_I64(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_I64;
                                find->value.signed_number = col_it_i64_values(NULL, it)[requested_idx];
                        }
                }
                        break;
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET: {
                        float field_value = col_it_float_values(NULL, it)[requested_idx];
                        if (IS_NULL_FLOAT(field_value)) {
                                find->type = FIELD_NULL;
                        } else {
                                find->type = FIELD_NUMBER_FLOAT;
                                find->value.float_number = col_it_float_values(NULL, it)[requested_idx];
                        }
                }
                        break;
                default:
                        return error(ERR_UNSUPPORTEDTYPE, NULL);
        }
        return true;
}