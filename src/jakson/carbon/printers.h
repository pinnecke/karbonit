/*
 * printers - formatters from Carbon to other formats
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_PRINTERS_H
#define HAD_PRINTERS_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/types.h>
#include <jakson/std/string.h>
#include <jakson/stdx/unique_id.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct printer
{
        void *extra;

        void (*drop)(printer *self);
        void (*record_begin)(printer *self, str_buf *buffer);
        void (*record_end)(printer *self, str_buf *buffer);
        void (*meta_begin)(printer *self, str_buf *buffer);
        /** type is of key_e */
        bool (*meta_data)(printer *self, str_buf *buffer, int key_type, const void *key, u64 key_length, u64 commit_hash);
        void (*meta_end)(printer *self, str_buf *buffer);
        void (*doc_begin)(printer *self, str_buf *buffer);
        void (*doc_end)(printer *self, str_buf *buffer);
        void (*empty_record)(printer *self, str_buf *buffer);
        void (*unit_array_begin)(printer *self, str_buf *buffer);
        void (*unit_array_end)(printer *self, str_buf *buffer);
        void (*array_begin)(printer *self, str_buf *buffer);
        void (*end)(printer *self, str_buf *buffer);
        void (*const_null)(printer *self, str_buf *buffer);
        void (*const_true)(printer *self, bool is_null, str_buf *buffer);
        void (*const_false)(printer *self, bool is_null, str_buf *buffer);
        /** if <code>value</code> is NULL, <code>value</code> is interpreted as null-value'd entry */
        void (*val_signed)(printer *self, str_buf *buffer, const i64 *value);
        /** if <code>value</code> is NULL, <code>value</code> is interpreted as null-value'd entry */
        void (*val_unsigned)(printer *self, str_buf *buffer, const u64 *value);
        /** if <code>value</code> is NULL, <code>value</code> is interpreted as null-value'd entry */
        void (*val_float)(printer *self, str_buf *buffer, const float *value);
        void (*val_string)(printer *self, str_buf *buffer, const char *value, u64 strlen);
        void (*val_binary)(printer *self, str_buf *buffer, const binary_field *binary);
        void (*comma)(printer *self, str_buf *buffer);
        void (*obj_begin)(printer *self, str_buf *buffer);
        void (*obj_end)(printer *self, str_buf *buffer);
        void (*prop_null)(printer *self, str_buf *buffer, const char *key_name, u64 key_len);
        void (*prop_true)(printer *self, str_buf *buffer, const char *key_name, u64 key_len);
        void (*prop_false)(printer *self, str_buf *buffer, const char *key_name, u64 key_len);
        void (*prop_signed)(printer *self, str_buf *buffer, const char *key_name, u64 key_len, const i64 *value);
        void (*prop_unsigned)(printer *self, str_buf *buffer, const char *key_name, u64 key_len, const u64 *value);
        void (*prop_float)(printer *self, str_buf *buffer, const char *key_name, u64 key_len, const float *value);
        void (*prop_string)(printer *self, str_buf *buffer, const char *key_name, u64 key_len, const char *value, u64 strlen);
        void (*prop_binary)(printer *self, str_buf *buffer, const char *key_name, u64 key_len, const binary_field *binary);
        void (*array_prop_name)(printer *self, str_buf *buffer, const char *key_name, u64 key_len);
        void (*column_prop_name)(printer *self, str_buf *buffer, const char *key_name, u64 key_len);
        void (*obj_prop_name)(printer *self, str_buf *buffer, const char *key_name, u64 key_len);
} printer;

/** 'impl' is of printer_impl_e */
bool printer_drop(printer *printer);
bool printer_by_type(printer *printer, int impl);

bool printer_begin(printer *printer, str_buf *str);
bool printer_end(printer *printer, str_buf *str);
bool printer_header_begin(printer *printer, str_buf *str);
/** 'key_type' is of key_e */
bool printer_header_contents(printer *printer, str_buf *str, int key_type, const void *key, u64 key_length, u64 rev);
bool printer_header_end(printer *printer, str_buf *str);
bool printer_payload_begin(printer *printer, str_buf *str);
bool printer_payload_end(printer *printer, str_buf *str);
bool printer_empty_record(printer *printer, str_buf *str);
bool printer_array_begin(printer *printer, str_buf *str);
bool printer_array_end(printer *printer, str_buf *str);
bool printer_unit_array_begin(printer *printer, str_buf *str);
bool printer_unit_array_end(printer *printer, str_buf *str);
bool printer_object_begin(printer *printer, str_buf *str);
bool printer_object_end(printer *printer, str_buf *str);
bool printer_null(printer *printer, str_buf *str);
bool printer_true(printer *printer, bool is_null, str_buf *str);
bool printer_false(printer *printer, bool is_null, str_buf *str);
bool printer_comma(printer *printer, str_buf *str);
bool printer_signed_nonull(printer *printer, str_buf *str, const i64 *value);
bool printer_unsigned_nonull(printer *printer, str_buf *str, const u64 *value);
bool printer_u8_or_null(printer *printer, str_buf *str, u8 value);
bool printer_u16_or_null(printer *printer, str_buf *str, u16 value);
bool printer_u32_or_null(printer *printer, str_buf *str, u32 value);
bool printer_u64_or_null(printer *printer, str_buf *str, u64 value);
bool printer_i8_or_null(printer *printer, str_buf *str, i8 value);
bool printer_i16_or_null(printer *printer, str_buf *str, i16 value);
bool printer_i32_or_null(printer *printer, str_buf *str, i32 value);
bool printer_i64_or_null(printer *printer, str_buf *str, i64 value);
bool printer_float(printer *printer, str_buf *str, const float *value);
bool printer_string(printer *printer, str_buf *str, const char *value, u64 strlen);
bool printer_binary(printer *printer, str_buf *str, const binary_field *binary);
bool printer_prop_null(printer *printer, str_buf *str, const char *key_name, u64 key_len);
bool printer_prop_true(printer *printer, str_buf *str, const char *key_name, u64 key_len);
bool printer_prop_false(printer *printer, str_buf *str, const char *key_name, u64 key_len);
bool printer_prop_signed(printer *printer, str_buf *str, const char *key_name, u64 key_len, const i64 *value);
bool printer_prop_unsigned(printer *printer, str_buf *str, const char *key_name, u64 key_len, const u64 *value);
bool printer_prop_float(printer *printer, str_buf *str, const char *key_name, u64 key_len, const float *value);
bool printer_prop_string(printer *printer, str_buf *str, const char *key_name, u64 key_len, const char *value, u64 strlen);
bool printer_prop_binary(printer *printer, str_buf *str, const char *key_name, u64 key_len, const binary_field *binary);
bool printer_array_prop_name(printer *printer, str_buf *str, const char *key_name, u64 key_len);
bool printer_column_prop_name(printer *printer, str_buf *str, const char *key_name, u64 key_len);
bool printer_object_prop_name(printer *printer, str_buf *str, const char *key_name, u64 key_len);
bool printer_print_object(obj_it *it, printer *printer, str_buf *buffer);
bool printer_print_array(arr_it *it, printer *printer, str_buf *buffer, bool is_record_container);
bool printer_print_column(col_it *it, printer *printer, str_buf *buffer);

#ifdef __cplusplus
}
#endif

#endif
