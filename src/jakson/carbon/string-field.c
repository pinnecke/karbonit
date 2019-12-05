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

#include <jakson/std/uintvar/stream.h>
#include <jakson/carbon/mime.h>
#include <jakson/carbon/field.h>
#include <jakson/carbon/string-field.h>

static void write_payload(memfile *file, const char *string, size_t str_len)
{
        memfile_write_uintvar_stream(NULL, file, str_len);
        memfile_ensure_space(file, str_len);
        memfile_write(file, string, str_len);
}

bool string_field_nomarker_write(memfile *file, const char *string)
{
        return string_field_nomarker_nchar_write(file, string, strlen(string));
}

bool string_field_nomarker_nchar_write(memfile *file, const char *string, u64 str_len)
{
        write_payload(file, string, str_len);
        return true;
}

bool string_field_nomarker_remove(memfile *file)
{
        u8 len_nbytes;
        u64 str_len = memfile_read_uintvar_stream(&len_nbytes, file);
        memfile_skip(file, -len_nbytes);
        memfile_inplace_remove(file, len_nbytes + str_len);
        return true;
}

bool string_field_remove(memfile *file)
{
        u8 marker = *memfile_read_type(file, u8);
        if (likely(marker == FIELD_STRING)) {
                memfile_inplace_remove(file, sizeof(u8));
                return string_field_nomarker_remove(file);
        } else {
                return error(ERR_MARKERMAPPING, NULL);
        }
}

bool string_field_write(memfile *file, const char *string)
{
        return string_field_nchar_write(file, string, strlen(string));
}

bool string_field_nchar_write(memfile *file, const char *string, u64 str_len)
{
        memfile_ensure_space(file, sizeof(media_type));
        mime_write(file, FIELD_STRING);
        string_field_nomarker_nchar_write(file, string, str_len);
        return true;
}

bool string_field_update(memfile *file, const char *string)
{
        return string_field_update_wnchar(file, string, strlen(string));
}

bool string_field_update_wnchar(memfile *file, const char *string, size_t str_len)
{
        u8 marker = *memfile_read_type(file, u8);
        if (likely(marker == FIELD_STRING)) {
                offset_t payload_start = memfile_tell(file);
                u32 old_len = memfile_read_uintvar_stream(NULL, file);
                memfile_skip(file, old_len);
                offset_t diff = memfile_tell(file) - payload_start;
                memfile_seek(file, payload_start);
                memfile_inplace_remove(file, diff);

                write_payload(file, string, str_len);
                return true;
        } else {
                return error(ERR_MARKERMAPPING, NULL);
        }
}

bool string_field_skip(memfile *file)
{
        return string_field_read(NULL, file);
}

bool string_field_nomarker_skip(memfile *file)
{
        return string_field_nomarker_read(NULL, file);
}

const char *string_field_read(u64 *len, memfile *file)
{
        u8 marker = *memfile_read_type(file, u8);
        if (likely(marker == FIELD_STRING)) {
                return string_field_nomarker_read(len, file);
        } else {
                error(ERR_MARKERMAPPING, NULL);
                return NULL;
        }
}

const char *string_field_nomarker_read(u64 *len, memfile *file)
{
        u64 str_len = memfile_read_uintvar_stream(NULL, file);
        const char *result = memfile_read(file, str_len);
        OPTIONAL_SET(len, str_len);
        return result;
}