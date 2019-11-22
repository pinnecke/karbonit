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

#include <inttypes.h>
#include <jakson/std/string.h>
#include <ctype.h>

bool str_buf_create(str_buf *buffer)
{
        return str_buf_create_ex(buffer, 1024);
}

bool str_buf_create_ex(str_buf *buffer, size_t capacity)
{
        buffer->cap = capacity;
        buffer->end = 0;
        buffer->data = MALLOC(capacity);
        error_if_and_return(!buffer->data, ERR_MALLOCERR, false);
        ZERO_MEMORY(buffer->data, buffer->cap);
        return true;
}

bool str_buf_add(str_buf *buffer, const char *str)
{
        u64 len = strlen(str);
        return str_buf_add_nchar(buffer, str, len);
}

bool str_buf_add_nchar(str_buf *buffer, const char *str, u64 strlen)
{
        /** resize if needed */
        if (unlikely(buffer->end + strlen >= buffer->cap)) {
                size_t new_cap = (buffer->end + strlen) * 1.7f;
                buffer->data = realloc(buffer->data, new_cap);
                error_if_and_return(!buffer->data, ERR_REALLOCERR, false);
                ZERO_MEMORY(buffer->data + buffer->cap, (new_cap - buffer->cap));
                buffer->cap = new_cap;
        }

        /** append str_buf */
        memcpy(buffer->data + buffer->end, str, strlen);
        buffer->end += strlen;

        return true;
}

bool str_buf_add_char(str_buf *buffer, char c)
{
        char buff[2];
        sprintf(buff, "%c", c);
        return str_buf_add(buffer, buff);
        return true;
}

bool str_buf_add_u8(str_buf *buffer, u8 value)
{
        char buff[21];
        ZERO_MEMORY(buff, ARRAY_LENGTH(buff));
        sprintf(buff, "%u", value);
        return str_buf_add(buffer, buff);
}

bool str_buf_add_u16(str_buf *buffer, u16 value)
{
        char buff[21];
        sprintf(buff, "%u", value);
        return str_buf_add(buffer, buff);
}

bool str_buf_add_u32(str_buf *buffer, u32 value)
{
        char buff[21];
        sprintf(buff, "%u", value);
        return str_buf_add(buffer, buff);
}

bool str_buf_add_u64(str_buf *buffer, u64 value)
{
        char buff[21];
        sprintf(buff, "%" PRIu64, value);
        return str_buf_add(buffer, buff);
}

bool str_buf_add_i8(str_buf *buffer, i8 value)
{
        char buff[21];
        sprintf(buff, "%d", value);
        return str_buf_add(buffer, buff);
}

bool str_buf_add_i16(str_buf *buffer, i16 value)
{
        char buff[21];
        sprintf(buff, "%d", value);
        return str_buf_add(buffer, buff);
}

bool str_buf_add_i32(str_buf *buffer, i32 value)
{
        char buff[21];
        sprintf(buff, "%d", value);
        return str_buf_add(buffer, buff);
}

bool str_buf_add_i64(str_buf *buffer, i64 value)
{
        char buff[21];
        sprintf(buff, "%" PRIi64, value);
        return str_buf_add(buffer, buff);
}

bool str_buf_add_u64_as_hex(str_buf *buffer, u64 value)
{
        char buff[17];
        sprintf(buff, "%016"PRIx64, value);
        return str_buf_add(buffer, buff);
}

bool str_buf_add_u64_as_hex_0x_prefix_compact(str_buf *buffer, u64 value)
{
        char buff[17];
        sprintf(buff, "0x%"PRIx64, value);
        return str_buf_add(buffer, buff);
}

bool str_buf_add_float(str_buf *buffer, float value)
{
        char buff[2046];
        sprintf(buff, "%0.2f", value);
        return str_buf_add(buffer, buff);
}

bool str_buf_add_bool(str_buf *buffer, bool value)
{
        char buff[6];
        sprintf(buff, "%s", value ? "true" : "false");
        return str_buf_add(buffer, buff);
}

bool str_buf_add_boolean(str_buf *buffer, boolean value)
{
        char buff[6];
        if (IS_NULL_BOOLEAN(value)) {
                sprintf(buff, "null");
        } else {
                sprintf(buff, "%s", value ? "true" : "false");
        }

        return str_buf_add(buffer, buff);
}

bool str_buf_clear(str_buf *buffer)
{
        ZERO_MEMORY(buffer->data, buffer->cap);
        buffer->end = 0;
        return true;
}

bool str_buf_ensure_capacity(str_buf *buffer, u64 cap)
{
        /** resize if needed */
        if (unlikely(cap > buffer->cap)) {
                size_t new_cap = cap * 1.7f;
                buffer->data = realloc(buffer->data, new_cap);
                error_if_and_return(!buffer->data, ERR_REALLOCERR, false);
                ZERO_MEMORY(buffer->data + buffer->cap, (new_cap - buffer->cap));
                buffer->cap = new_cap;
        }
        return true;
}

size_t string_len(str_buf *buffer)
{
        return buffer->end;
}

bool str_buf_trim(str_buf *buffer)
{
        if (buffer->end > 0) {
                char *string = buffer->data;
                int len = strlen(string);

                while (isspace(string[len - 1])) {
                        string[--len] = '\0';
                }
                while (*string && isspace(*string)) {
                        ++string;
                        --len;
                }

                buffer->end = len + 1;
                memmove(buffer->data, string, buffer->end);
        }
        return true;
}

bool str_buf_is_empty(str_buf *buffer)
{
        return buffer ? (buffer->end == 0 || (buffer->end == 1 && buffer->data[0] == '\0')) : true;
}

bool str_buf_drop(str_buf *buffer)
{
        free(buffer->data);
        return true;
}

bool str_buf_print(str_buf *buffer)
{
        return str_buf_fprint(stdout, buffer);
}

bool str_buf_fprint(FILE *file, str_buf *buffer)
{
        fprintf(file, "%s\n", string_cstr(buffer));
        return true;
}

const char *string_cstr(str_buf *buffer)
{
        return buffer->data;
}