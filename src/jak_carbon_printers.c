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

#include <jak_carbon_printers.h>
#include <jak_json_printer_compact.h>
#include <jak_json_printer_extended.h>
#include <jak_carbon_object_it.h>
#include <jak_carbon_array_it.h>
#include <jak_carbon_column_it.h>

bool jak_carbon_printer_by_type(jak_carbon_printer *printer, int impl)
{
        JAK_ERROR_IF_NULL(printer)

        switch (impl) {
                case JAK_JSON_EXTENDED:
                        jak_json_extended_printer_create(printer);
                        break;
                case JAK_JSON_COMPACT:
                        jak_json_compact_printer_create(printer);
                        break;
                default: JAK_ERROR_PRINT(JAK_ERR_NOTFOUND)
                        return false;
        }
        return true;
}

bool jak_carbon_printer_drop(jak_carbon_printer *printer)
{
        JAK_ERROR_IF_NULL(printer->drop);
        printer->drop(printer);
        return true;
}

bool jak_carbon_printer_begin(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->record_begin);
        printer->record_begin(printer, str);
        return true;
}

bool jak_carbon_printer_end(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->record_end);
        printer->record_end(printer, str);
        return true;
}

bool jak_carbon_printer_header_begin(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->meta_begin);
        printer->meta_begin(printer, str);
        return true;
}

bool jak_carbon_printer_header_contents(jak_carbon_printer *printer, jak_string *str,
                                    int key_type, const void *key, jak_u64 key_length,
                                    jak_u64 rev)
{
        JAK_ERROR_IF_NULL(printer->drop);
        printer->meta_data(printer, str, key_type, key, key_length, rev);
        return true;
}

bool jak_carbon_printer_header_end(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->meta_end);
        printer->meta_end(printer, str);
        return true;
}

bool jak_carbon_printer_payload_begin(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->doc_begin);
        printer->doc_begin(printer, str);
        return true;
}

bool jak_carbon_printer_payload_end(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->doc_end);
        printer->doc_end(printer, str);
        return true;
}

bool jak_carbon_printer_empty_record(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->empty_record);
        printer->empty_record(printer, str);
        return true;
}

bool jak_carbon_printer_array_begin(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->array_begin);
        printer->array_begin(printer, str);
        return true;
}

bool jak_carbon_printer_array_end(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->array_end);
        printer->array_end(printer, str);
        return true;
}

bool jak_carbon_printer_unit_array_begin(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->unit_array_begin);
        printer->unit_array_begin(printer, str);
        return true;
}

bool jak_carbon_printer_unit_array_end(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->unit_array_end);
        printer->unit_array_end(printer, str);
        return true;
}

bool jak_carbon_printer_object_begin(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->obj_begin);
        printer->obj_begin(printer, str);
        return true;
}

bool jak_carbon_printer_object_end(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->obj_end);
        printer->obj_end(printer, str);
        return true;
}

bool jak_carbon_printer_null(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->const_null);
        printer->const_null(printer, str);
        return true;
}

bool jak_carbon_printer_true(jak_carbon_printer *printer, bool is_null, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->const_true);
        printer->const_true(printer, is_null, str);
        return true;
}

bool jak_carbon_printer_false(jak_carbon_printer *printer, bool is_null, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->const_false);
        printer->const_false(printer, is_null, str);
        return true;
}

bool jak_carbon_printer_comma(jak_carbon_printer *printer, jak_string *str)
{
        JAK_ERROR_IF_NULL(printer->comma);
        printer->comma(printer, str);
        return true;
}

bool jak_carbon_printer_signed_nonull(jak_carbon_printer *printer, jak_string *str, const jak_i64 *value)
{
        JAK_ERROR_IF_NULL(printer->val_signed);
        printer->val_signed(printer, str, value);
        return true;
}

bool jak_carbon_printer_unsigned_nonull(jak_carbon_printer *printer, jak_string *str, const jak_u64 *value)
{
        JAK_ERROR_IF_NULL(printer->val_unsigned);
        printer->val_unsigned(printer, str, value);
        return true;
}

#define delegate_print_call(printer, str, value, null_test_func, print_func, ctype)                                    \
({                                                                                                                     \
        JAK_ERROR_IF_NULL(printer)                                                                                         \
        JAK_ERROR_IF_NULL(str)                                                                                             \
        bool status = false;                                                                                           \
        if (null_test_func(value)) {                                                                                       \
                status = jak_carbon_printer_null(printer, str);                                                            \
        } else {                                                                                                       \
                ctype val = value;                                                                                     \
                status = print_func(printer, str, &val);                                                               \
        }                                                                                                              \
        status;                                                                                                        \
})

bool jak_carbon_printer_u8_or_null(jak_carbon_printer *printer, jak_string *str, jak_u8 value)
{
        return delegate_print_call(printer, str, value, JAK_IS_NULL_U8, jak_carbon_printer_unsigned_nonull, jak_u64);
}

bool jak_carbon_printer_u16_or_null(jak_carbon_printer *printer, jak_string *str, jak_u16 value)
{
        return delegate_print_call(printer, str, value, JAK_IS_NULL_U16, jak_carbon_printer_unsigned_nonull, jak_u64);
}

bool jak_carbon_printer_u32_or_null(jak_carbon_printer *printer, jak_string *str, jak_u32 value)
{
        return delegate_print_call(printer, str, value, JAK_IS_NULL_U32, jak_carbon_printer_unsigned_nonull, jak_u64);
}

bool jak_carbon_printer_u64_or_null(jak_carbon_printer *printer, jak_string *str, jak_u64 value)
{
        return delegate_print_call(printer, str, value, JAK_IS_NULL_U64, jak_carbon_printer_unsigned_nonull, jak_u64);
}

bool jak_carbon_printer_i8_or_null(jak_carbon_printer *printer, jak_string *str, jak_i8 value)
{
        return delegate_print_call(printer, str, value, JAK_IS_NULL_I8, jak_carbon_printer_signed_nonull, jak_i64);
}

bool jak_carbon_printer_i16_or_null(jak_carbon_printer *printer, jak_string *str, jak_i16 value)
{
        return delegate_print_call(printer, str, value, JAK_IS_NULL_I16, jak_carbon_printer_signed_nonull, jak_i64);
}

bool jak_carbon_printer_i32_or_null(jak_carbon_printer *printer, jak_string *str, jak_i32 value)
{
        return delegate_print_call(printer, str, value, JAK_IS_NULL_I32, jak_carbon_printer_signed_nonull, jak_i64);
}

bool jak_carbon_printer_i64_or_null(jak_carbon_printer *printer, jak_string *str, jak_i64 value)
{
        return delegate_print_call(printer, str, value, JAK_IS_NULL_I64, jak_carbon_printer_signed_nonull, jak_i64);
}

bool jak_carbon_printer_float(jak_carbon_printer *printer, jak_string *str, const float *value)
{
        JAK_ERROR_IF_NULL(printer->val_float);
        printer->val_float(printer, str, value);
        return true;
}

bool jak_carbon_printer_string(jak_carbon_printer *printer, jak_string *str, const char *value, jak_u64 strlen)
{
        JAK_ERROR_IF_NULL(printer->val_string);
        printer->val_string(printer, str, value, strlen);
        return true;
}

bool jak_carbon_printer_binary(jak_carbon_printer *printer, jak_string *str, const jak_carbon_binary *binary)
{
        JAK_ERROR_IF_NULL(printer->val_binary);
        printer->val_binary(printer, str, binary);
        return true;
}

bool jak_carbon_printer_prop_null(jak_carbon_printer *printer, jak_string *str,
                              const char *key_name, jak_u64 key_len)
{
        JAK_ERROR_IF_NULL(printer->prop_null);
        printer->prop_null(printer, str, key_name, key_len);
        return true;
}

bool jak_carbon_printer_prop_true(jak_carbon_printer *printer, jak_string *str,
                              const char *key_name, jak_u64 key_len)
{
        JAK_ERROR_IF_NULL(printer->prop_true);
        printer->prop_true(printer, str, key_name, key_len);
        return true;
}

bool jak_carbon_printer_prop_false(jak_carbon_printer *printer, jak_string *str,
                               const char *key_name, jak_u64 key_len)
{
        JAK_ERROR_IF_NULL(printer->prop_false);
        printer->prop_false(printer, str, key_name, key_len);
        return true;
}

bool jak_carbon_printer_prop_signed(jak_carbon_printer *printer, jak_string *str,
                                const char *key_name, jak_u64 key_len, const jak_i64 *value)
{
        JAK_ERROR_IF_NULL(printer->prop_signed);
        printer->prop_signed(printer, str, key_name, key_len, value);
        return true;
}

bool jak_carbon_printer_prop_unsigned(jak_carbon_printer *printer, jak_string *str,
                                  const char *key_name, jak_u64 key_len, const jak_u64 *value)
{
        JAK_ERROR_IF_NULL(printer->prop_unsigned);
        printer->prop_unsigned(printer, str, key_name, key_len, value);
        return true;
}

bool jak_carbon_printer_prop_float(jak_carbon_printer *printer, jak_string *str,
                               const char *key_name, jak_u64 key_len, const float *value)
{
        JAK_ERROR_IF_NULL(printer->prop_float);
        printer->prop_float(printer, str, key_name, key_len, value);
        return true;
}

bool jak_carbon_printer_prop_string(jak_carbon_printer *printer, jak_string *str,
                                const char *key_name, jak_u64 key_len, const char *value, jak_u64 strlen)
{
        JAK_ERROR_IF_NULL(printer->prop_string);
        printer->prop_string(printer, str, key_name, key_len, value, strlen);
        return true;
}

bool jak_carbon_printer_prop_binary(jak_carbon_printer *printer, jak_string *str,
                                const char *key_name, jak_u64 key_len, const jak_carbon_binary *binary)
{
        JAK_ERROR_IF_NULL(printer->prop_binary);
        printer->prop_binary(printer, str, key_name, key_len, binary);
        return true;
}

bool jak_carbon_printer_array_prop_name(jak_carbon_printer *printer, jak_string *str,
                                    const char *key_name, jak_u64 key_len)
{
        JAK_ERROR_IF_NULL(printer->array_prop_name);
        printer->array_prop_name(printer, str, key_name, key_len);
        return true;
}

bool jak_carbon_printer_column_prop_name(jak_carbon_printer *printer, jak_string *str,
                                     const char *key_name, jak_u64 key_len)
{
        JAK_ERROR_IF_NULL(printer->column_prop_name);
        printer->column_prop_name(printer, str, key_name, key_len);
        return true;
}

bool jak_carbon_printer_object_prop_name(jak_carbon_printer *printer, jak_string *str,
                                     const char *key_name, jak_u64 key_len)
{
        JAK_ERROR_IF_NULL(printer->obj_prop_name);
        printer->obj_prop_name(printer, str, key_name, key_len);
        return true;
}

bool jak_carbon_printer_print_object(jak_carbon_object_it *it, jak_carbon_printer *printer, jak_string *builder)
{
        JAK_ASSERT(it);
        JAK_ASSERT(printer);
        JAK_ASSERT(builder);
        bool is_null_value = false;
        bool first_entry = true;
        jak_carbon_printer_object_begin(printer, builder);

        while (jak_carbon_object_it_next(it)) {
                if (JAK_LIKELY(!first_entry)) {
                        jak_carbon_printer_comma(printer, builder);
                }
                JAK_DECLARE_AND_INIT(jak_carbon_field_type_e, type)
                JAK_DECLARE_AND_INIT(jak_u64, key_len)
                const char *key_name = jak_carbon_object_it_prop_name(&key_len, it);

                jak_carbon_object_it_prop_type(&type, it);
                switch (type) {
                        case JAK_CARBON_FIELD_TYPE_NULL:
                                jak_carbon_printer_prop_null(printer, builder, key_name, key_len);
                                break;
                        case JAK_CARBON_FIELD_TYPE_TRUE:
                                /* in an array, there is no TRUE constant that is set to NULL because it will be replaced with
                                 * a constant NULL. In columns, there might be a NULL-encoded value */
                                jak_carbon_printer_prop_true(printer, builder, key_name, key_len);
                                break;
                        case JAK_CARBON_FIELD_TYPE_FALSE:
                                /* in an array, there is no FALSE constant that is set to NULL because it will be replaced with
                                 * a constant NULL. In columns, there might be a NULL-encoded value */
                                jak_carbon_printer_prop_false(printer, builder, key_name, key_len);
                                break;
                        case JAK_CARBON_FIELD_TYPE_NUMBER_U8:
                        case JAK_CARBON_FIELD_TYPE_NUMBER_U16:
                        case JAK_CARBON_FIELD_TYPE_NUMBER_U32:
                        case JAK_CARBON_FIELD_TYPE_NUMBER_U64: {
                                jak_u64 value;
                                jak_carbon_object_it_unsigned_value(&is_null_value, &value, it);
                                jak_carbon_printer_prop_unsigned(printer, builder, key_name, key_len,
                                                             is_null_value ? NULL : &value);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_NUMBER_I8:
                        case JAK_CARBON_FIELD_TYPE_NUMBER_I16:
                        case JAK_CARBON_FIELD_TYPE_NUMBER_I32:
                        case JAK_CARBON_FIELD_TYPE_NUMBER_I64: {
                                jak_i64 value;
                                jak_carbon_object_it_signed_value(&is_null_value, &value, it);
                                jak_carbon_printer_prop_signed(printer, builder, key_name, key_len,
                                                           is_null_value ? NULL : &value);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_NUMBER_FLOAT: {
                                float value;
                                jak_carbon_object_it_float_value(&is_null_value, &value, it);
                                jak_carbon_printer_prop_float(printer, builder, key_name, key_len,
                                                          is_null_value ? NULL : &value);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_STRING: {
                                jak_u64 strlen;
                                const char *value = jak_carbon_object_it_jak_string_value(&strlen, it);
                                jak_carbon_printer_prop_string(printer, builder, key_name, key_len, value, strlen);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_BINARY:
                        case JAK_CARBON_FIELD_TYPE_BINARY_CUSTOM: {
                                jak_carbon_binary binary;
                                jak_carbon_object_it_binary_value(&binary, it);
                                jak_carbon_printer_prop_binary(printer, builder, key_name, key_len, &binary);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_ARRAY: {
                                jak_carbon_array_it *array = jak_carbon_object_it_array_value(it);
                                jak_carbon_printer_array_prop_name(printer, builder, key_name, key_len);
                                jak_carbon_printer_print_array(array, printer, builder, false);
                                jak_carbon_array_it_drop(array);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_COLUMN_U8:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_U16:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_U32:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_U64:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_I8:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_I16:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_I32:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_I64:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_FLOAT:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_BOOLEAN: {
                                jak_carbon_column_it *column = jak_carbon_object_it_column_value(it);
                                jak_carbon_printer_column_prop_name(printer, builder, key_name, key_len);
                                jak_carbon_printer_print_column(column, printer, builder);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_OBJECT: {
                                jak_carbon_object_it *object = jak_carbon_object_it_object_value(it);
                                jak_carbon_printer_object_prop_name(printer, builder, key_name, key_len);
                                jak_carbon_printer_print_object(object, printer, builder);
                                jak_carbon_object_it_drop(object);
                        }
                                break;
                        default:
                                jak_carbon_printer_object_end(printer, builder);
                                JAK_ERROR(&it->err, JAK_ERR_CORRUPTED);
                                return false;
                }
                first_entry = false;
        }

        jak_carbon_printer_object_end(printer, builder);
        return true;
}

bool jak_carbon_printer_print_array(jak_carbon_array_it *it, jak_carbon_printer *printer, jak_string *builder,
                                bool is_record_container)
{
        JAK_ASSERT(it);
        JAK_ASSERT(printer);
        JAK_ASSERT(builder);

        bool first_entry = true;
        bool has_entries = false;
        bool is_single_entry_array = jak_carbon_array_it_is_unit(it);

        while (jak_carbon_array_it_next(it)) {
                bool is_null_value;

                if (JAK_LIKELY(!first_entry)) {
                        jak_carbon_printer_comma(printer, builder);
                } else {
                        if (is_single_entry_array && is_record_container) {
                                jak_carbon_printer_unit_array_begin(printer, builder);
                        } else {
                                jak_carbon_printer_array_begin(printer, builder);
                        }
                        has_entries = true;
                }
                jak_carbon_field_type_e type;
                jak_carbon_array_it_field_type(&type, it);
                switch (type) {
                        case JAK_CARBON_FIELD_TYPE_NULL:
                                jak_carbon_printer_null(printer, builder);
                                break;
                        case JAK_CARBON_FIELD_TYPE_TRUE:
                                /* in an array, there is no TRUE constant that is set to NULL because it will be replaced with
                                 * a constant NULL. In columns, there might be a NULL-encoded value */
                                jak_carbon_printer_true(printer, false, builder);
                                break;
                        case JAK_CARBON_FIELD_TYPE_FALSE:
                                /* in an array, there is no FALSE constant that is set to NULL because it will be replaced with
                                 * a constant NULL. In columns, there might be a NULL-encoded value */
                                jak_carbon_printer_false(printer, false, builder);
                                break;
                        case JAK_CARBON_FIELD_TYPE_NUMBER_U8:
                        case JAK_CARBON_FIELD_TYPE_NUMBER_U16:
                        case JAK_CARBON_FIELD_TYPE_NUMBER_U32:
                        case JAK_CARBON_FIELD_TYPE_NUMBER_U64: {
                                jak_u64 value;
                                jak_carbon_array_it_unsigned_value(&is_null_value, &value, it);
                                jak_carbon_printer_unsigned_nonull(printer, builder, is_null_value ? NULL : &value);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_NUMBER_I8:
                        case JAK_CARBON_FIELD_TYPE_NUMBER_I16:
                        case JAK_CARBON_FIELD_TYPE_NUMBER_I32:
                        case JAK_CARBON_FIELD_TYPE_NUMBER_I64: {
                                jak_i64 value;
                                jak_carbon_array_it_signed_value(&is_null_value, &value, it);
                                jak_carbon_printer_signed_nonull(printer, builder, is_null_value ? NULL : &value);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_NUMBER_FLOAT: {
                                float value;
                                jak_carbon_array_it_float_value(&is_null_value, &value, it);
                                jak_carbon_printer_float(printer, builder, is_null_value ? NULL : &value);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_STRING: {
                                jak_u64 strlen;
                                const char *value = jak_carbon_array_it_jak_string_value(&strlen, it);
                                jak_carbon_printer_string(printer, builder, value, strlen);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_BINARY:
                        case JAK_CARBON_FIELD_TYPE_BINARY_CUSTOM: {
                                jak_carbon_binary binary;
                                jak_carbon_array_it_binary_value(&binary, it);
                                jak_carbon_printer_binary(printer, builder, &binary);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_ARRAY: {
                                jak_carbon_array_it *array = jak_carbon_array_it_array_value(it);
                                jak_carbon_printer_print_array(array, printer, builder, false);
                                jak_carbon_array_it_drop(array);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_COLUMN_U8:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_U16:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_U32:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_U64:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_I8:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_I16:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_I32:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_I64:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_FLOAT:
                        case JAK_CARBON_FIELD_TYPE_COLUMN_BOOLEAN: {
                                jak_carbon_column_it *column = jak_carbon_array_it_column_value(it);
                                jak_carbon_printer_print_column(column, printer, builder);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_OBJECT: {
                                jak_carbon_object_it *object = jak_carbon_array_it_object_value(it);
                                jak_carbon_printer_print_object(object, printer, builder);
                                jak_carbon_object_it_drop(object);
                        }
                                break;
                        default:
                                jak_carbon_printer_array_end(printer, builder);
                                JAK_ERROR(&it->err, JAK_ERR_CORRUPTED);
                                return false;
                }
                first_entry = false;
        }

        if (has_entries) {
                if (is_single_entry_array && is_record_container) {
                        jak_carbon_printer_unit_array_end(printer, builder);
                } else {
                        jak_carbon_printer_array_end(printer, builder);
                }
        } else {
                if (is_record_container) {
                        jak_carbon_printer_empty_record(printer, builder);
                } else {
                        jak_carbon_printer_array_begin(printer, builder);
                        jak_carbon_printer_array_end(printer, builder);
                }
        }

        return true;
}

bool jak_carbon_printer_print_column(jak_carbon_column_it *it, jak_carbon_printer *printer, jak_string *builder)
{
        JAK_ERROR_IF_NULL(it)
        JAK_ERROR_IF_NULL(printer)
        JAK_ERROR_IF_NULL(builder)

        jak_carbon_field_type_e type;
        jak_u32 nvalues;
        const void *values = jak_carbon_column_it_values(&type, &nvalues, it);

        jak_carbon_printer_array_begin(printer, builder);
        for (jak_u32 i = 0; i < nvalues; i++) {
                switch (type) {
                        case JAK_CARBON_FIELD_TYPE_COLUMN_BOOLEAN: {
                                jak_u8 value = ((jak_u8 *) values)[i];
                                if (JAK_IS_NULL_BOOLEAN(value)) {
                                        jak_carbon_printer_null(printer, builder);
                                } else if (value == JAK_CARBON_BOOLEAN_COLUMN_TRUE) {
                                        jak_carbon_printer_true(printer, false, builder);
                                } else {
                                        jak_carbon_printer_false(printer, false, builder);
                                }
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_COLUMN_U8: {
                                jak_u64 number = ((jak_u8 *) values)[i];
                                jak_carbon_printer_unsigned_nonull(printer, builder, JAK_IS_NULL_U8(number) ? NULL : &number);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_COLUMN_U16: {
                                jak_u64 number = ((jak_u16 *) values)[i];
                                jak_carbon_printer_unsigned_nonull(printer, builder, JAK_IS_NULL_U16(number) ? NULL : &number);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_COLUMN_U32: {
                                jak_u64 number = ((jak_u32 *) values)[i];
                                jak_carbon_printer_unsigned_nonull(printer, builder, JAK_IS_NULL_U32(number) ? NULL : &number);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_COLUMN_U64: {
                                jak_u64 number = ((jak_u64 *) values)[i];
                                jak_carbon_printer_unsigned_nonull(printer, builder, JAK_IS_NULL_U64(number) ? NULL : &number);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_COLUMN_I8: {
                                jak_i64 number = ((jak_i8 *) values)[i];
                                jak_carbon_printer_signed_nonull(printer, builder, JAK_IS_NULL_I8(number) ? NULL : &number);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_COLUMN_I16: {
                                jak_i64 number = ((jak_i16 *) values)[i];
                                jak_carbon_printer_signed_nonull(printer, builder, JAK_IS_NULL_I16(number) ? NULL : &number);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_COLUMN_I32: {
                                jak_i64 number = ((jak_i32 *) values)[i];
                                jak_carbon_printer_signed_nonull(printer, builder, JAK_IS_NULL_I32(number) ? NULL : &number);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_COLUMN_I64: {
                                jak_i64 number = ((jak_i64 *) values)[i];
                                jak_carbon_printer_signed_nonull(printer, builder, JAK_IS_NULL_I64(number) ? NULL : &number);
                        }
                                break;
                        case JAK_CARBON_FIELD_TYPE_COLUMN_FLOAT: {
                                float number = ((float *) values)[i];
                                jak_carbon_printer_float(printer, builder, JAK_IS_NULL_FLOAT(number) ? NULL : &number);
                        }
                                break;
                        default:
                                jak_carbon_printer_array_end(printer, builder);
                                JAK_ERROR(&it->err, JAK_ERR_CORRUPTED);
                                return false;
                }
                if (i + 1 < nvalues) {
                        jak_carbon_printer_comma(printer, builder);
                }
        }
        jak_carbon_printer_array_end(printer, builder);

        return true;
}