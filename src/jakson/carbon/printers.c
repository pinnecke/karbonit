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

#include <jakson/carbon/printers.h>
#include <jakson/carbon/printers/compact.h>
#include <jakson/carbon/printers/extended.h>
#include <jakson/carbon/obj-it.h>
#include <jakson/carbon/arr-it.h>
#include <jakson/carbon/col-it.h>

bool printer_by_type(printer *printer, int impl)
{
        switch (impl) {
                case JSON_EXTENDED:
                        json_extended_printer_create(printer);
                        break;
                case JSON_COMPACT:
                        json_compact_printer_create(printer);
                        break;
                default:
                        return error(ERR_NOTFOUND, NULL);
        }
        return true;
}

bool printer_drop(printer *printer)
{
        printer->drop(printer);
        return true;
}

bool printer_begin(printer *printer, str_buf *str)
{
        printer->record_begin(printer, str);
        return true;
}

bool printer_end(printer *printer, str_buf *str)
{
        printer->record_end(printer, str);
        return true;
}

bool printer_header_begin(printer *printer, str_buf *str)
{
        printer->meta_begin(printer, str);
        return true;
}

bool printer_header_contents(printer *printer, str_buf *str,
                                    int key_type, const void *key, u64 key_length,
                                    u64 rev)
{
        printer->meta_data(printer, str, key_type, key, key_length, rev);
        return true;
}

bool printer_header_end(printer *printer, str_buf *str)
{
        printer->meta_end(printer, str);
        return true;
}

bool printer_payload_begin(printer *printer, str_buf *str)
{
        printer->doc_begin(printer, str);
        return true;
}

bool printer_payload_end(printer *printer, str_buf *str)
{
        printer->doc_end(printer, str);
        return true;
}

bool printer_empty_record(printer *printer, str_buf *str)
{
        printer->empty_record(printer, str);
        return true;
}

bool printer_array_begin(printer *printer, str_buf *str)
{
        printer->array_begin(printer, str);
        return true;
}

bool printer_array_end(printer *printer, str_buf *str)
{
        printer->end(printer, str);
        return true;
}

bool printer_unit_array_begin(printer *printer, str_buf *str)
{
        printer->unit_array_begin(printer, str);
        return true;
}

bool printer_unit_array_end(printer *printer, str_buf *str)
{
        printer->unit_array_end(printer, str);
        return true;
}

bool printer_object_begin(printer *printer, str_buf *str)
{
        printer->obj_begin(printer, str);
        return true;
}

bool printer_object_end(printer *printer, str_buf *str)
{
        printer->obj_end(printer, str);
        return true;
}

bool printer_null(printer *printer, str_buf *str)
{
        printer->const_null(printer, str);
        return true;
}

bool printer_true(printer *printer, bool is_null, str_buf *str)
{
        printer->const_true(printer, is_null, str);
        return true;
}

bool printer_false(printer *printer, bool is_null, str_buf *str)
{
        printer->const_false(printer, is_null, str);
        return true;
}

bool printer_comma(printer *printer, str_buf *str)
{
        printer->comma(printer, str);
        return true;
}

bool printer_signed_nonull(printer *printer, str_buf *str, const i64 *value)
{
        printer->val_signed(printer, str, value);
        return true;
}

bool printer_unsigned_nonull(printer *printer, str_buf *str, const u64 *value)
{
        printer->val_unsigned(printer, str, value);
        return true;
}

#define delegate_print_call(printer, str, value, null_test_func, print_func, ctype)                                    \
({                                                                                                                     \
        bool status = false;                                                                                           \
        if (null_test_func(value)) {                                                                                       \
                status = printer_null(printer, str);                                                            \
        } else {                                                                                                       \
                ctype val = value;                                                                                     \
                status = print_func(printer, str, &val);                                                               \
        }                                                                                                              \
        status;                                                                                                        \
})

bool printer_u8_or_null(printer *printer, str_buf *str, u8 value)
{
        return delegate_print_call(printer, str, value, IS_NULL_U8, printer_unsigned_nonull, u64);
}

bool printer_u16_or_null(printer *printer, str_buf *str, u16 value)
{
        return delegate_print_call(printer, str, value, IS_NULL_U16, printer_unsigned_nonull, u64);
}

bool printer_u32_or_null(printer *printer, str_buf *str, u32 value)
{
        return delegate_print_call(printer, str, value, IS_NULL_U32, printer_unsigned_nonull, u64);
}

bool printer_u64_or_null(printer *printer, str_buf *str, u64 value)
{
        return delegate_print_call(printer, str, value, IS_NULL_U64, printer_unsigned_nonull, u64);
}

bool printer_i8_or_null(printer *printer, str_buf *str, i8 value)
{
        return delegate_print_call(printer, str, value, IS_NULL_I8, printer_signed_nonull, i64);
}

bool printer_i16_or_null(printer *printer, str_buf *str, i16 value)
{
        return delegate_print_call(printer, str, value, IS_NULL_I16, printer_signed_nonull, i64);
}

bool printer_i32_or_null(printer *printer, str_buf *str, i32 value)
{
        return delegate_print_call(printer, str, value, IS_NULL_I32, printer_signed_nonull, i64);
}

bool printer_i64_or_null(printer *printer, str_buf *str, i64 value)
{
        return delegate_print_call(printer, str, value, IS_NULL_I64, printer_signed_nonull, i64);
}

bool printer_float(printer *printer, str_buf *str, const float *value)
{
        printer->val_float(printer, str, value);
        return true;
}

bool printer_string(printer *printer, str_buf *str, const char *value, u64 strlen)
{
        printer->val_string(printer, str, value, strlen);
        return true;
}

bool printer_binary(printer *printer, str_buf *str, const binary_field *binary)
{
        printer->val_binary(printer, str, binary);
        return true;
}

bool printer_prop_null(printer *printer, str_buf *str,
                              const char *key_name, u64 key_len)
{
        printer->prop_null(printer, str, key_name, key_len);
        return true;
}

bool printer_prop_true(printer *printer, str_buf *str,
                              const char *key_name, u64 key_len)
{
        printer->prop_true(printer, str, key_name, key_len);
        return true;
}

bool printer_prop_false(printer *printer, str_buf *str,
                               const char *key_name, u64 key_len)
{
        printer->prop_false(printer, str, key_name, key_len);
        return true;
}

bool printer_prop_signed(printer *printer, str_buf *str,
                                const char *key_name, u64 key_len, const i64 *value)
{
        printer->prop_signed(printer, str, key_name, key_len, value);
        return true;
}

bool printer_prop_unsigned(printer *printer, str_buf *str,
                                  const char *key_name, u64 key_len, const u64 *value)
{
        printer->prop_unsigned(printer, str, key_name, key_len, value);
        return true;
}

bool printer_prop_float(printer *printer, str_buf *str,
                               const char *key_name, u64 key_len, const float *value)
{
        printer->prop_float(printer, str, key_name, key_len, value);
        return true;
}

bool printer_prop_string(printer *printer, str_buf *str,
                                const char *key_name, u64 key_len, const char *value, u64 strlen)
{
        printer->prop_string(printer, str, key_name, key_len, value, strlen);
        return true;
}

bool printer_prop_binary(printer *printer, str_buf *str,
                                const char *key_name, u64 key_len, const binary_field *binary)
{
        printer->prop_binary(printer, str, key_name, key_len, binary);
        return true;
}

bool printer_array_prop_name(printer *printer, str_buf *str,
                                    const char *key_name, u64 key_len)
{
        printer->array_prop_name(printer, str, key_name, key_len);
        return true;
}

bool printer_column_prop_name(printer *printer, str_buf *str,
                                     const char *key_name, u64 key_len)
{
        printer->column_prop_name(printer, str, key_name, key_len);
        return true;
}

bool printer_object_prop_name(printer *printer, str_buf *str,
                                     const char *key_name, u64 key_len)
{
        printer->obj_prop_name(printer, str, key_name, key_len);
        return true;
}

bool printer_print_object(obj_it *it, printer *printer, str_buf *buffer)
{
        JAK_ASSERT(it);
        JAK_ASSERT(printer);
        JAK_ASSERT(buffer);
        bool is_null_value = false;
        bool first_entry = true;
        printer_object_begin(printer, buffer);

        while (obj_it_next(it)) {
                if (LIKELY(!first_entry)) {
                        printer_comma(printer, buffer);
                }
                DECLARE_AND_INIT(field_e, type)
                string_field prop_key = internal_obj_it_prop_name(it);

                internal_obj_it_prop_type(&type, it);
                switch (type) {
                        case FIELD_NULL:
                                printer_prop_null(printer, buffer, prop_key.str, prop_key.len);
                                break;
                        case FIELD_TRUE:
                                /** in an array, there is no TRUE constant that is set to NULL because it will be replaced with
                                 * a constant NULL. In columns, there might be a NULL-encoded value */
                                printer_prop_true(printer, buffer, prop_key.str, prop_key.len);
                                break;
                        case FIELD_FALSE:
                                /** in an array, there is no FALSE constant that is set to NULL because it will be replaced with
                                 * a constant NULL. In columns, there might be a NULL-encoded value */
                                printer_prop_false(printer, buffer, prop_key.str, prop_key.len);
                                break;
                        case FIELD_NUMBER_U8:
                        case FIELD_NUMBER_U16:
                        case FIELD_NUMBER_U32:
                        case FIELD_NUMBER_U64: {
                                u64 value = item_get_number_unsigned(&(it->prop.value), CARBON_NULL_UNSIGNED);
                                is_null_value = IS_NULL_UNSIGNED(value);
                                printer_prop_unsigned(printer, buffer, prop_key.str, prop_key.len,
                                                             is_null_value ? NULL : &value);
                        }
                                break;
                        case FIELD_NUMBER_I8:
                        case FIELD_NUMBER_I16:
                        case FIELD_NUMBER_I32:
                        case FIELD_NUMBER_I64: {
                                i64 value = item_get_number_signed(&(it->prop.value), CARBON_NULL_SIGNED);
                                is_null_value = IS_NULL_SIGNED(value);
                                printer_prop_signed(printer, buffer, prop_key.str, prop_key.len,
                                                           is_null_value ? NULL : &value);
                        }
                                break;
                        case FIELD_NUMBER_FLOAT: {
                                float value = item_get_number_float(&(it->prop.value), CARBON_NULL_FLOAT);
                                is_null_value = IS_NULL_FLOAT(value);
                                printer_prop_float(printer, buffer, prop_key.str, prop_key.len,
                                                          is_null_value ? NULL : &value);
                        }
                                break;
                        case FIELD_STRING: {
                                string_field value = item_get_string(&(it->prop.value), NULL_STRING);
                                printer_prop_string(printer, buffer, prop_key.str, prop_key.len, value.str, value.len);
                        }
                                break;
                        case FIELD_BINARY:
                        case FIELD_BINARY_CUSTOM: {
                                binary_field binary = item_get_binary(&(it->prop.value), NULL_BINARY);
                                printer_prop_binary(printer, buffer, prop_key.str, prop_key.len, &binary);
                        }
                                break;
                        case FIELD_ARRAY_UNSORTED_MULTISET:
                        case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                        case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                        case FIELD_DERIVED_ARRAY_SORTED_SET: {
                                arr_it *array = item_get_array(&(it->prop.value));
                                printer_array_prop_name(printer, buffer, prop_key.str, prop_key.len);
                                printer_print_array(array, printer, buffer, false);
                                arr_it_drop(array);
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
                                col_it *column = item_get_column(&(it->prop.value));
                                printer_column_prop_name(printer, buffer, prop_key.str, prop_key.len);
                                printer_print_column(column, printer, buffer);
                        }
                                break;
                        case FIELD_OBJECT_UNSORTED_MULTIMAP:
                        case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                        case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                        case FIELD_DERIVED_OBJECT_SORTED_MAP: {
                                obj_it *object = item_get_object(&(it->prop.value));
                                printer_object_prop_name(printer, buffer, prop_key.str, prop_key.len);
                                printer_print_object(object, printer, buffer);
                                obj_it_drop(object);
                        }
                                break;
                        default:
                                printer_object_end(printer, buffer);
                                error(ERR_CORRUPTED, NULL);
                                return false;
                }
                first_entry = false;
        }

        printer_object_end(printer, buffer);
        return true;
}

bool printer_print_array(arr_it *it, printer *printer, str_buf *buffer,
                                bool is_record_container)
{
        JAK_ASSERT(it);
        JAK_ASSERT(printer);
        JAK_ASSERT(buffer);

        bool first_entry = true;
        bool has_entries = false;
        bool is_single_entry_array = arr_it_is_unit(it);

        while (arr_it_next(it)) {
                bool is_null_value;

                if (LIKELY(!first_entry)) {
                        printer_comma(printer, buffer);
                } else {
                        if (is_single_entry_array && is_record_container) {
                                printer_unit_array_begin(printer, buffer);
                        } else {
                                printer_array_begin(printer, buffer);
                        }
                        has_entries = true;
                }
                field_e type;
                arr_it_field_type(&type, it);
                switch (type) {
                        case FIELD_NULL:
                                printer_null(printer, buffer);
                                break;
                        case FIELD_TRUE:
                                /** in an array, there is no TRUE constant that is set to NULL because it will be replaced with
                                 * a constant NULL. In columns, there might be a NULL-encoded value */
                                printer_true(printer, false, buffer);
                                break;
                        case FIELD_FALSE:
                                /** in an array, there is no FALSE constant that is set to NULL because it will be replaced with
                                 * a constant NULL. In columns, there might be a NULL-encoded value */
                                printer_false(printer, false, buffer);
                                break;
                        case FIELD_NUMBER_U8:
                        case FIELD_NUMBER_U16:
                        case FIELD_NUMBER_U32:
                        case FIELD_NUMBER_U64: {
                                u64 value = item_get_number_unsigned(&(it->item), CARBON_NULL_UNSIGNED);
                                is_null_value = false;
                                printer_unsigned_nonull(printer, buffer, is_null_value ? NULL : &value);
                        }
                                break;
                        case FIELD_NUMBER_I8:
                        case FIELD_NUMBER_I16:
                        case FIELD_NUMBER_I32:
                        case FIELD_NUMBER_I64: {
                                i64 value = item_get_number_signed(&(it->item), CARBON_NULL_SIGNED);
                                is_null_value = false;
                                printer_signed_nonull(printer, buffer, is_null_value ? NULL : &value);
                        }
                                break;
                        case FIELD_NUMBER_FLOAT: {
                                float value = item_get_number_float(&(it->item), CARBON_NULL_FLOAT);
                                is_null_value = false;
                                printer_float(printer, buffer, is_null_value ? NULL : &value);
                        }
                                break;
                        case FIELD_STRING: {
                                string_field string = item_get_string(&(it->item), NULL_STRING);
                                printer_string(printer, buffer, string.str, string.len);
                        }
                                break;
                        case FIELD_BINARY:
                        case FIELD_BINARY_CUSTOM: {
                                binary_field binary = item_get_binary(&(it->item), NULL_BINARY);
                                printer_binary(printer, buffer, &binary);
                        }
                                break;
                        case FIELD_ARRAY_UNSORTED_MULTISET:
                        case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                        case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                        case FIELD_DERIVED_ARRAY_SORTED_SET: {
                                arr_it *array = item_get_array(&(it->item));
                                printer_print_array(array, printer, buffer, false);
                                arr_it_drop(array);
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
                                col_it *column = item_get_column(&(it->item));
                                printer_print_column(column, printer, buffer);
                        }
                                break;
                        case FIELD_OBJECT_UNSORTED_MULTIMAP:
                        case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                        case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                        case FIELD_DERIVED_OBJECT_SORTED_MAP: {
                                obj_it *object = item_get_object(&(it->item));
                                printer_print_object(object, printer, buffer);
                                obj_it_drop(object);
                        }
                                break;
                        default:
                                printer_array_end(printer, buffer);
                                error(ERR_CORRUPTED, NULL);
                                return false;
                }
                first_entry = false;
        }

        if (has_entries) {
                if (is_single_entry_array && is_record_container) {
                        printer_unit_array_end(printer, buffer);
                } else {
                        printer_array_end(printer, buffer);
                }
        } else {
                if (is_record_container) {
                        printer_empty_record(printer, buffer);
                } else {
                        printer_array_begin(printer, buffer);
                        printer_array_end(printer, buffer);
                }
        }

        return true;
}

bool printer_print_column(col_it *it, printer *printer, str_buf *buffer)
{
        field_e type;
        u32 nvalues;
        const void *values = col_it_values(&type, &nvalues, it);

        printer_array_begin(printer, buffer);
        for (u32 i = 0; i < nvalues; i++) {
                switch (type) {
                        case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                                u8 value = ((u8 *) values)[i];
                                if (IS_NULL_BOOLEAN(value)) {
                                        printer_null(printer, buffer);
                                } else if (value == CARBON_BOOLEAN_COLUMN_TRUE) {
                                        printer_true(printer, false, buffer);
                                } else {
                                        printer_false(printer, false, buffer);
                                }
                        }
                                break;
                        case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_U8_SORTED_SET: {
                                u64 number = ((u8 *) values)[i];
                                printer_unsigned_nonull(printer, buffer, IS_NULL_U8(number) ? NULL : &number);
                        }
                                break;
                        case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_U16_SORTED_SET: {
                                u64 number = ((u16 *) values)[i];
                                printer_unsigned_nonull(printer, buffer, IS_NULL_U16(number) ? NULL : &number);
                        }
                                break;
                        case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_U32_SORTED_SET: {
                                u64 number = ((u32 *) values)[i];
                                printer_unsigned_nonull(printer, buffer, IS_NULL_U32(number) ? NULL : &number);
                        }
                                break;
                        case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_U64_SORTED_SET: {
                                u64 number = ((u64 *) values)[i];
                                printer_unsigned_nonull(printer, buffer, IS_NULL_U64(number) ? NULL : &number);
                        }
                                break;
                        case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_I8_SORTED_SET: {
                                i64 number = ((i8 *) values)[i];
                                printer_signed_nonull(printer, buffer, IS_NULL_I8(number) ? NULL : &number);
                        }
                                break;
                        case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_I16_SORTED_SET: {
                                i64 number = ((i16 *) values)[i];
                                printer_signed_nonull(printer, buffer, IS_NULL_I16(number) ? NULL : &number);
                        }
                                break;
                        case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_I32_SORTED_SET: {
                                i64 number = ((i32 *) values)[i];
                                printer_signed_nonull(printer, buffer, IS_NULL_I32(number) ? NULL : &number);
                        }
                                break;
                        case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_I64_SORTED_SET: {
                                i64 number = ((i64 *) values)[i];
                                printer_signed_nonull(printer, buffer, IS_NULL_I64(number) ? NULL : &number);
                        }
                                break;
                        case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                        case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                        case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET: {
                                float number = ((float *) values)[i];
                                printer_float(printer, buffer, IS_NULL_FLOAT(number) ? NULL : &number);
                        }
                                break;
                        default:
                                printer_array_end(printer, buffer);
                                error(ERR_CORRUPTED, NULL);
                                return false;
                }
                if (i + 1 < nvalues) {
                        printer_comma(printer, buffer);
                }
        }
        printer_array_end(printer, buffer);

        return true;
}