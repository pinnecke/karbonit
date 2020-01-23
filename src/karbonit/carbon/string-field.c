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

#include <karbonit/std/uintvar/stream.h>
#include <karbonit/carbon/mime.h>
#include <karbonit/carbon/field.h>
#include <karbonit/carbon/string-field.h>

static void write_payload(memfile *file, const char *string, size_t str_len)
{
        MEMFILE_WRITE_UINTVAR_STREAM(NULL, file, str_len);
        MEMFILE_ENSURE_SPACE(file, str_len);
        MEMFILE_WRITE(file, string, str_len);
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
        u64 str_len = MEMFILE_READ_UINTVAR_STREAM(&len_nbytes, file);
        MEMFILE_SKIP(file, -len_nbytes);
        MEMFILE_INPLACE_REMOVE(file, len_nbytes + str_len);
        return true;
}

bool string_field_remove(memfile *file)
{
        u8 marker = *MEMFILE_READ_TYPE(file, u8);
        if (LIKELY(marker == FIELD_STRING)) {
                MEMFILE_INPLACE_REMOVE(file, sizeof(u8));
                return string_field_nomarker_remove(file);
        } else {
                return ERROR(ERR_MARKERMAPPING, NULL);
        }
}

bool string_field_write(memfile *file, const char *string)
{
        return string_field_nchar_write(file, string, strlen(string));
}

bool string_field_nchar_write(memfile *file, const char *string, u64 str_len)
{
        MEMFILE_ENSURE_SPACE(file, sizeof(media_type));
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
        u8 marker = *MEMFILE_READ_TYPE(file, u8);
        if (LIKELY(marker == FIELD_STRING)) {
                offset_t payload_start = MEMFILE_TELL(file);
                u32 old_len = MEMFILE_READ_UINTVAR_STREAM(NULL, file);
                MEMFILE_SKIP(file, old_len);
                offset_t diff = MEMFILE_TELL(file) - payload_start;
                MEMFILE_SEEK(file, payload_start);
                MEMFILE_INPLACE_REMOVE(file, diff);

                write_payload(file, string, str_len);
                return true;
        } else {
                return ERROR(ERR_MARKERMAPPING, NULL);
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
        u8 marker = *MEMFILE_READ_TYPE(file, u8);
        if (LIKELY(marker == FIELD_STRING)) {
                return string_field_nomarker_read(len, file);
        } else {
                ERROR(ERR_MARKERMAPPING, NULL);
                return NULL;
        }
}

const char *string_field_nomarker_read(u64 *len, memfile *file)
{
        u64 str_len = MEMFILE_READ_UINTVAR_STREAM(NULL, file);
        const char *result = MEMFILE_READ(file, str_len);
        OPTIONAL_SET(len, str_len);
        return result;
}