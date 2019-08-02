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
#include <ark-js/shared/stdx/string_builder.h>

ARK_EXPORT(bool) string_builder_create(struct string_builder *builder)
{
        return string_builder_create_ex(builder, 1024);
}

ARK_EXPORT(bool) string_builder_create_ex(struct string_builder *builder, size_t capacity)
{
        error_if_null(builder)
        error_if_null(capacity)
        error_init(&builder->err);
        builder->cap = capacity;
        builder->end = 0;
        builder->data = malloc(capacity);
        error_if_and_return(!builder->data, &builder->err, ARK_ERR_MALLOCERR, false);
        ark_zero_memory(builder->data, builder->cap);
        return true;
}

ARK_EXPORT(bool) string_builder_append(struct string_builder *builder, const char *str)
{
        error_if_null(builder)
        error_if_null(str)
        u64 len = strlen(str);
        return string_builder_append_nchar(builder, str, len);
}

ARK_EXPORT(bool) string_builder_append_nchar(struct string_builder *builder, const char *str, u64 strlen)
{
        error_if_null(builder)
        error_if_null(str)

        /* resize if needed */
        if (unlikely(builder->end + strlen >= builder->cap)) {
                size_t new_cap = (builder->end + strlen) * 1.7f;
                builder->data = realloc(builder->data, new_cap);
                error_if_and_return(!builder->data, &builder->err, ARK_ERR_REALLOCERR, false);
                ark_zero_memory(builder->data + builder->cap, (new_cap - builder->cap));
                builder->cap = new_cap;
        }

        /* append string */
        memcpy(builder->data + builder->end, str, strlen);
        builder->end += strlen;

        return true;
}

ARK_EXPORT(bool) string_builder_append_char(struct string_builder *builder, char c)
{
        error_if_null(builder)
        char buffer[2];
        sprintf(buffer, "%c", c);
        return string_builder_append(builder, buffer);
        return true;
}

ARK_EXPORT(bool) string_builder_append_u8(struct string_builder *builder, u8 value)
{
        char buffer[21];
        ark_zero_memory(buffer, ARK_ARRAY_LENGTH(buffer));
        sprintf(buffer, "%u", value);
        return string_builder_append(builder, buffer);
}

ARK_EXPORT(bool) string_builder_append_u16(struct string_builder *builder, u16 value)
{
        char buffer[21];
        sprintf(buffer, "%u", value);
        return string_builder_append(builder, buffer);
}

ARK_EXPORT(bool) string_builder_append_u32(struct string_builder *builder, u32 value)
{
        char buffer[21];
        sprintf(buffer, "%u", value);
        return string_builder_append(builder, buffer);
}

ARK_EXPORT(bool) string_builder_append_u64(struct string_builder *builder, u64 value)
{
        char buffer[21];
        sprintf(buffer, "%" PRIu64, value);
        return string_builder_append(builder, buffer);
}

ARK_EXPORT(bool) string_builder_append_i8(struct string_builder *builder, i8 value)
{
        char buffer[21];
        sprintf(buffer, "%d", value);
        return string_builder_append(builder, buffer);
}

ARK_EXPORT(bool) string_builder_append_i16(struct string_builder *builder, i16 value)
{
        char buffer[21];
        sprintf(buffer, "%d", value);
        return string_builder_append(builder, buffer);
}

ARK_EXPORT(bool) string_builder_append_i32(struct string_builder *builder, i32 value)
{
        char buffer[21];
        sprintf(buffer, "%d", value);
        return string_builder_append(builder, buffer);
}

ARK_EXPORT(bool) string_builder_append_i64(struct string_builder *builder, i64 value)
{
        char buffer[21];
        sprintf(buffer, "%" PRIi64, value);
        return string_builder_append(builder, buffer);
}

ARK_EXPORT(bool) string_builder_append_float(struct string_builder *builder, float value)
{
        char buffer[2046];
        sprintf(buffer, "%0.2f", value);
        return string_builder_append(builder, buffer);
}

ARK_EXPORT(bool) string_builder_clear(struct string_builder *builder)
{
        error_if_null(builder)
        ark_zero_memory(builder->data, builder->cap);
        builder->end = 0;
        return true;
}

ARK_EXPORT(size_t) string_builder_length(struct string_builder *builder)
{
        error_if_null(builder)
        return builder->end;
}

ARK_EXPORT(bool) string_builder_drop(struct string_builder *builder)
{
        error_if_null(builder)
        free (builder->data);
        return true;
}

ARK_EXPORT(bool) string_builder_print(struct string_builder *builder)
{
        return string_builder_printf(stdout, builder);
}

ARK_EXPORT(bool) string_builder_printf(FILE *file, struct string_builder *builder)
{
        error_if_null(file)
        error_if_null(builder)
        fprintf(file, "%s\n", string_builder_cstr(builder));
        return true;
}

ARK_EXPORT(const char *) string_builder_cstr(struct string_builder *builder)
{
        return builder->data;
}