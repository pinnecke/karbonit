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

#include <libs/libb64/libb64.h>
#include <jakson/carbon/printers/extended.h>
#include <jakson/carbon/arr-it.h>

// ---------------------------------------------------------------------------------------------------------------------

#define INIT_BUFFER_LEN 1024
#define NULL_STR "null"

// ---------------------------------------------------------------------------------------------------------------------

struct extra {
        char *buffer;
        size_t buffer_size;
};

// ---------------------------------------------------------------------------------------------------------------------

#define INIT_BUFFER_LEN 1024
#define NULL_STR "null"

// ---------------------------------------------------------------------------------------------------------------------

struct json_compact_extra {
        char *buffer;
        size_t buffer_size;
};

// ---------------------------------------------------------------------------------------------------------------------

static void _json_printer_compact_nop(printer *self, str_buf *buffer)
{
        UNUSED(self)
        UNUSED(buffer)
}

static void _json_printer_compact_drop(printer *self)
{
        struct json_compact_extra *extra = (struct json_compact_extra *) self->extra;
        free(extra->buffer);
        free(self->extra);
}

static void _json_printer_compact_obj_begin(printer *self, str_buf *buffer)
{
        UNUSED(self);
        str_buf_add(buffer, "{");
}

static void _json_printer_compact_obj_end(printer *self, str_buf *buffer)
{
        UNUSED(self);
        str_buf_add(buffer, "}");
}

static bool meta_data_nop(printer *self, str_buf *buffer, int key_type, const void *key,
                          u64 key_length, u64 rev)
{
        UNUSED(self)
        UNUSED(buffer)
        UNUSED(key_type)
        UNUSED(key)
        UNUSED(key_length)
        UNUSED(rev)
        return false;
}

static void _json_printer_compact_empty_record(printer *self, str_buf *buffer)
{
        UNUSED(self);
        str_buf_add(buffer, "{}");
}

static void _json_printer_compact_array_begin(printer *self, str_buf *buffer)
{
        UNUSED(self);

        str_buf_add(buffer, "[");
}

static void _json_printer_compact_array_end(printer *self, str_buf *buffer)
{
        UNUSED(self);
        str_buf_add(buffer, "]");
}

static void _json_printer_compact_const_null(printer *self, str_buf *buffer)
{
        UNUSED(self);
        str_buf_add(buffer, "null");
}

static void _json_printer_compact_const_true(printer *self, bool is_null, str_buf *buffer)
{
        UNUSED(self);
        str_buf_add(buffer, is_null ? "null" : "true");
}

static void _json_printer_compact_const_false(printer *self, bool is_null, str_buf *buffer)
{
        UNUSED(self);
        str_buf_add(buffer, is_null ? "null" : "false");
}

static void _json_printer_compact_val_signed(printer *self, str_buf *buffer, const i64 *value)
{
        UNUSED(self);
        if (LIKELY(value != NULL)) {
                str_buf_add_i64(buffer, *value);
        } else {
                str_buf_add(buffer, NULL_STR);
        }

}

static void _json_printer_compact_val_unsigned(printer *self, str_buf *buffer, const u64 *value)
{
        UNUSED(self);
        if (LIKELY(value != NULL)) {
                str_buf_add_u64(buffer, *value);
        } else {
                str_buf_add(buffer, NULL_STR);
        }
}

static void _json_printer_compact_val_float(printer *self, str_buf *buffer, const float *value)
{
        UNUSED(self);
        if (LIKELY(value != NULL)) {
                str_buf_add_float(buffer, *value);
        } else {
                str_buf_add(buffer, NULL_STR);
        }
}

static void _json_printer_compact_val_string(printer *self, str_buf *buffer, const char *value, u64 strlen)
{
        UNUSED(self);
        str_buf_add_char(buffer, '"');
        str_buf_add_nchar(buffer, value, strlen);
        str_buf_add_char(buffer, '"');
}

#define code_of(x, data_len)      (x + data_len + 2)
#define data_of(x)      (x)

static bool _json_printer_compact_print_binary(printer *self, str_buf *buffer, const binary_field *binary)
{
        /** base64 code will be written into the extra's buffer after a null-terminated copy of the binary data */
        struct json_compact_extra *extra = (struct json_compact_extra *) self->extra;
        /** buffer of at least 2x data length for base64 code + 1x data length to hold the null-terminated value */
        size_t required_buff_size = 3 * (binary->blob_len + 1);
        /** increase buffer capacity if needed */
        if (extra->buffer_size < required_buff_size) {
                extra->buffer_size = required_buff_size * 1.7f;
                extra->buffer = realloc(extra->buffer, extra->buffer_size);
                error_if_and_return(!extra->buffer, ERR_REALLOCERR, NULL);
        }
        /** decrease buffer capacity if needed */
        if (extra->buffer_size * 0.3f > required_buff_size) {
                extra->buffer_size = required_buff_size;
                extra->buffer = realloc(extra->buffer, extra->buffer_size);
                error_if_and_return(!extra->buffer, ERR_REALLOCERR, NULL);
        }

        JAK_ASSERT(extra->buffer_size >= required_buff_size);
        ZERO_MEMORY(extra->buffer, extra->buffer_size);
        /** copy binary data into buffer, and leave one (zero'd) byte free; null-termination is required by libb64 */
        memcpy(data_of(extra->buffer), binary->blob, binary->blob_len);

        str_buf_add(buffer, "{ ");
        str_buf_add(buffer, "\"type\": \"");
        str_buf_add_nchar(buffer, binary->mime, binary->mime_len);
        str_buf_add(buffer, "\", \"encoding\": \"base64\", \"binary-str_buf\": \"");

        base64_encodestate state;
        base64_init_encodestate(&state);

        u64 code_len = base64_encode_block(data_of(extra->buffer), binary->blob_len + 2,
                                               code_of(extra->buffer, binary->blob_len), &state);
        base64_encode_blockend(code_of(extra->buffer, binary->blob_len), &state);
        str_buf_add_nchar(buffer, code_of(extra->buffer, binary->blob_len), code_len);
        str_buf_add(buffer, "\" }");
        return true;
}

static void _json_printer_compact_val_binary(printer *self, str_buf *buffer, const binary_field *binary)
{
        _json_printer_compact_print_binary(self, buffer, binary);
}

static void _json_printer_compact_comma(printer *self, str_buf *buffer)
{
        UNUSED(self);
        str_buf_add(buffer, ", ");
}

static void print_key(str_buf *buffer, const char *key_name, u64 key_len)
{
        str_buf_add_char(buffer, '"');
        str_buf_add_nchar(buffer, key_name, key_len);
        str_buf_add(buffer, "\": ");
}

static void _json_printer_compact_prop_null(printer *self, str_buf *buffer,
                      const char *key_name, u64 key_len)
{
        UNUSED(self);
        print_key(buffer, key_name, key_len);
        str_buf_add(buffer, "null");
}

static void _json_printer_compact_prop_true(printer *self, str_buf *buffer,
                      const char *key_name, u64 key_len)
{
        UNUSED(self);
        print_key(buffer, key_name, key_len);
        str_buf_add(buffer, "true");
}

static void _json_printer_compact_prop_false(printer *self, str_buf *buffer,
                       const char *key_name, u64 key_len)
{
        UNUSED(self);
        print_key(buffer, key_name, key_len);
        str_buf_add(buffer, "false");
}

static void _json_printer_compact_prop_signed(printer *self, str_buf *buffer,
                        const char *key_name, u64 key_len, const i64 *value)
{
        UNUSED(self);
        print_key(buffer, key_name, key_len);
        str_buf_add_i64(buffer, *value);
}

static void _json_printer_compact_prop_unsigned(printer *self, str_buf *buffer,
                          const char *key_name, u64 key_len, const u64 *value)
{
        UNUSED(self);
        print_key(buffer, key_name, key_len);
        str_buf_add_u64(buffer, *value);
}

static void _json_printer_compact_prop_float(printer *self, str_buf *buffer,
                       const char *key_name, u64 key_len, const float *value)
{
        UNUSED(self);
        print_key(buffer, key_name, key_len);
        str_buf_add_float(buffer, *value);
}

static void _json_printer_compact_prop_string(printer *self, str_buf *buffer,
                        const char *key_name, u64 key_len, const char *value, u64 strlen)
{
        UNUSED(self);
        print_key(buffer, key_name, key_len);
        str_buf_add_char(buffer, '"');
        str_buf_add_nchar(buffer, value, strlen);
        str_buf_add_char(buffer, '"');
}

static void _json_printer_compact_prop_binary(printer *self, str_buf *buffer,
                        const char *key_name, u64 key_len, const binary_field *binary)
{
        print_key(buffer, key_name, key_len);
        _json_printer_compact_print_binary(self, buffer, binary);
}

static void _json_printer_compact_array_prop_name(printer *self, str_buf *buffer,
                            const char *key_name, u64 key_len)
{
        UNUSED(self)
        print_key(buffer, key_name, key_len);
}

static void _json_printer_compact_obj_prop_name(printer *self, str_buf *buffer,
                          const char *key_name, u64 key_len)
{
        UNUSED(self)
        print_key(buffer, key_name, key_len);
}

// ---------------------------------------------------------------------------------------------------------------------

bool json_compact_printer_create(printer *printer)
{
        printer->drop = _json_printer_compact_drop;

        printer->record_begin = _json_printer_compact_nop;
        printer->record_end = _json_printer_compact_nop;

        printer->meta_begin = _json_printer_compact_nop;
        printer->meta_data = meta_data_nop;
        printer->meta_end = _json_printer_compact_nop;

        printer->doc_begin = _json_printer_compact_nop;
        printer->doc_end = _json_printer_compact_nop;

        printer->empty_record = _json_printer_compact_empty_record;

        printer->unit_array_begin = _json_printer_compact_nop;
        printer->unit_array_end = _json_printer_compact_nop;

        printer->array_begin = _json_printer_compact_array_begin;
        printer->end = _json_printer_compact_array_end;

        printer->const_null = _json_printer_compact_const_null;
        printer->const_true = _json_printer_compact_const_true;
        printer->const_false = _json_printer_compact_const_false;

        printer->val_signed = _json_printer_compact_val_signed;
        printer->val_unsigned = _json_printer_compact_val_unsigned;
        printer->val_float = _json_printer_compact_val_float;
        printer->val_string = _json_printer_compact_val_string;
        printer->val_binary = _json_printer_compact_val_binary;

        printer->comma = _json_printer_compact_comma;

        printer->prop_null = _json_printer_compact_prop_null;
        printer->prop_true = _json_printer_compact_prop_true;
        printer->prop_false = _json_printer_compact_prop_false;
        printer->prop_signed = _json_printer_compact_prop_signed;
        printer->prop_unsigned = _json_printer_compact_prop_unsigned;
        printer->prop_float = _json_printer_compact_prop_float;
        printer->prop_string = _json_printer_compact_prop_string;
        printer->prop_binary = _json_printer_compact_prop_binary;
        printer->array_prop_name = _json_printer_compact_array_prop_name;
        printer->column_prop_name = _json_printer_compact_array_prop_name;
        printer->obj_prop_name = _json_printer_compact_obj_prop_name;
        printer->obj_begin = _json_printer_compact_obj_begin;
        printer->obj_end = _json_printer_compact_obj_end;

        printer->extra = MALLOC(sizeof(struct json_compact_extra));
        struct json_compact_extra *extra = (struct json_compact_extra *) printer->extra;
        *extra = (struct json_compact_extra) {
                .buffer_size = INIT_BUFFER_LEN,
                .buffer = MALLOC(INIT_BUFFER_LEN)
        };
        ZERO_MEMORY(extra->buffer, extra->buffer_size);

        return true;
}
