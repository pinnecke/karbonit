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

#ifndef STRING_H
#define STRING_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct str_buf {
        char *data;
        size_t cap;
        size_t end;
} str_buf;

bool str_buf_create(str_buf *buffer);
bool str_buf_create_ex(str_buf *buffer, size_t capacity);
bool str_buf_drop(str_buf *buffer);

bool str_buf_add(str_buf *buffer, const char *str);
bool str_buf_add_nchar(str_buf *buffer, const char *str, u64 strlen);
bool str_buf_add_char(str_buf *buffer, char c);
bool str_buf_add_u8(str_buf *buffer, u8 value);
bool str_buf_add_u16(str_buf *buffer, u16 value);
bool str_buf_add_u32(str_buf *buffer, u32 value);
bool str_buf_add_u64(str_buf *buffer, u64 value);
bool str_buf_add_i8(str_buf *buffer, i8 value);
bool str_buf_add_i16(str_buf *buffer, i16 value);
bool str_buf_add_i32(str_buf *buffer, i32 value);
bool str_buf_add_i64(str_buf *buffer, i64 value);
bool str_buf_add_u64_as_hex(str_buf *buffer, u64 value);
bool str_buf_add_u64_as_hex_0x_prefix_compact(str_buf *buffer, u64 value);
bool str_buf_add_float(str_buf *buffer, float value);
bool str_buf_add_bool(str_buf *buffer, bool value);
bool str_buf_add_boolean(str_buf *buffer, boolean value);
bool str_buf_clear(str_buf *buffer);
bool str_buf_ensure_capacity(str_buf *buffer, u64 cap);
size_t string_len(str_buf *buffer);
bool str_buf_trim(str_buf *buffer);
bool str_buf_is_empty(str_buf *buffer);

const char *string_cstr(str_buf *buffer);

bool str_buf_print(str_buf *buffer);
bool str_buf_fprint(FILE *file, str_buf *buffer);

#ifdef __cplusplus
}
#endif

#endif
