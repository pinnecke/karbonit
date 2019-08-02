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

#ifndef ARK_STRING_BUILDER_H
#define ARK_STRING_BUILDER_H

#include <ark-js/shared/common.h>
#include <ark-js/shared/error.h>

ARK_BEGIN_DECL

struct string_builder
{
        char *data;
        size_t cap;
        size_t end;
        struct err err;
};

ARK_DEFINE_GET_ERROR_FUNCTION(string_builder, struct string_builder, builder);

ARK_EXPORT(bool) string_builder_create(struct string_builder *builder);

ARK_EXPORT(bool) string_builder_create_ex(struct string_builder *builder, size_t capacity);

ARK_EXPORT(bool) string_builder_append(struct string_builder *builder, const char *str);

ARK_EXPORT(bool) string_builder_append_nchar(struct string_builder *builder, const char *str, u64 strlen);

ARK_EXPORT(bool) string_builder_append_char(struct string_builder *builder, char c);

ARK_EXPORT(bool) string_builder_append_u8(struct string_builder *builder, u8 value);

ARK_EXPORT(bool) string_builder_append_u16(struct string_builder *builder, u16 value);

ARK_EXPORT(bool) string_builder_append_u32(struct string_builder *builder, u32 value);

ARK_EXPORT(bool) string_builder_append_u64(struct string_builder *builder, u64 value);

ARK_EXPORT(bool) string_builder_append_i8(struct string_builder *builder, i8 value);

ARK_EXPORT(bool) string_builder_append_i16(struct string_builder *builder, i16 value);

ARK_EXPORT(bool) string_builder_append_i32(struct string_builder *builder, i32 value);

ARK_EXPORT(bool) string_builder_append_i64(struct string_builder *builder, i64 value);

ARK_EXPORT(bool) string_builder_append_float(struct string_builder *builder, float value);

ARK_EXPORT(bool) string_builder_clear(struct string_builder *builder);

ARK_EXPORT(size_t) string_builder_length(struct string_builder *builder);

ARK_EXPORT(bool) string_builder_drop(struct string_builder *builder);

ARK_EXPORT(bool) string_builder_print(struct string_builder *builder);

ARK_EXPORT(bool) string_builder_printf(FILE *file, struct string_builder *builder);

ARK_EXPORT(const char *) string_builder_cstr(struct string_builder *builder);

ARK_END_DECL

#endif