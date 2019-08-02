/**
 * Columnar Binary JSON -- Copyright 2019 Marcus Pinnecke
 * This file implements the document format itself
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

#ifndef CARBON_LITERAL_INSERTER_H
#define CARBON_LITERAL_INSERTER_H

#include <ark-js/shared/common.h>
#include <ark-js/shared/error.h>
#include <ark-js/shared/mem/block.h>
#include <ark-js/shared/mem/file.h>
#include <ark-js/shared/async/spin.h>
#include <ark-js/carbon/carbon.h>
#include <ark-js/carbon/carbon-int.h>

ARK_BEGIN_DECL

struct carbon_array_it; /* forwarded from carbon-array-it.h */
struct carbon_column_it; /* forwarded from carbon-column-it.h */

ARK_EXPORT(bool) carbon_int_insert_create_for_array(struct carbon_insert *inserter, struct carbon_array_it *context);

ARK_EXPORT(bool) carbon_int_insert_create_for_column(struct carbon_insert *inserter, struct carbon_column_it *context);

ARK_EXPORT(bool) carbon_int_insert_create_for_object(struct carbon_insert *inserter, struct carbon_object_it *context);

ARK_EXPORT(bool) carbon_insert_null(struct carbon_insert *inserter);

ARK_EXPORT(bool) carbon_insert_true(struct carbon_insert *inserter);

ARK_EXPORT(bool) carbon_insert_false(struct carbon_insert *inserter);

ARK_EXPORT(bool) carbon_insert_u8(struct carbon_insert *inserter, u8 value);

ARK_EXPORT(bool) carbon_insert_u16(struct carbon_insert *inserter, u16 value);

ARK_EXPORT(bool) carbon_insert_u32(struct carbon_insert *inserter, u32 value);

ARK_EXPORT(bool) carbon_insert_u64(struct carbon_insert *inserter, u64 value);

ARK_EXPORT(bool) carbon_insert_i8(struct carbon_insert *inserter, i8 value);

ARK_EXPORT(bool) carbon_insert_i16(struct carbon_insert *inserter, i16 value);

ARK_EXPORT(bool) carbon_insert_i32(struct carbon_insert *inserter, i32 value);

ARK_EXPORT(bool) carbon_insert_i64(struct carbon_insert *inserter, i64 value);

ARK_EXPORT(bool) carbon_insert_unsigned(struct carbon_insert *inserter, u64 value);

ARK_EXPORT(bool) carbon_insert_signed(struct carbon_insert *inserter, i64 value);

ARK_EXPORT(bool) carbon_insert_float(struct carbon_insert *inserter, float value);

ARK_EXPORT(bool) carbon_insert_string(struct carbon_insert *inserter, const char *value);

/**
 * Inserts a user-defined binary string <code>value</code> of <code>nbytes</code> bytes along with a (mime) type annotation.
 * The type annotation is automatically found if <code>file_ext</code> is non-null and known to the system. If it is
 * not known or null, the non-empty <code>user_type</code> string is used to encode the mime annotation. In case
 * <code>user_type</code> is null (or empty) and <code>file_ext</code> is null (or not known), the mime type is set to
 * <code>application/octet-stream</code>, which encodes arbitrary binary data.
 */
ARK_EXPORT(bool) carbon_insert_binary(struct carbon_insert *inserter, const void *value, size_t nbytes,
        const char *file_ext, const char *user_type);

ARK_EXPORT(struct carbon_insert *) carbon_insert_object_begin(struct carbon_insert_object_state *out,
        struct carbon_insert *inserter, u64 object_capacity);

ARK_EXPORT(bool) carbon_insert_object_end(struct carbon_insert_object_state *state);

ARK_EXPORT(struct carbon_insert *) carbon_insert_array_begin(struct carbon_insert_array_state *state_out,
        struct carbon_insert *inserter_in, u64 array_capacity);

ARK_EXPORT(bool) carbon_insert_array_end(struct carbon_insert_array_state *state_in);

ARK_EXPORT(struct carbon_insert *) carbon_insert_column_begin(struct carbon_insert_column_state *state_out,
        struct carbon_insert *inserter_in, enum carbon_column_type type, u64 column_capacity);

ARK_EXPORT(bool) carbon_insert_column_end(struct carbon_insert_column_state *state_in);







ARK_EXPORT(bool) carbon_insert_prop_null(struct carbon_insert *inserter, const char *key);

ARK_EXPORT(bool) carbon_insert_prop_true(struct carbon_insert *inserter, const char *key);

ARK_EXPORT(bool) carbon_insert_prop_false(struct carbon_insert *inserter, const char *key);

ARK_EXPORT(bool) carbon_insert_prop_u8(struct carbon_insert *inserter, const char *key, u8 value);

ARK_EXPORT(bool) carbon_insert_prop_u16(struct carbon_insert *inserter, const char *key, u16 value);

ARK_EXPORT(bool) carbon_insert_prop_u32(struct carbon_insert *inserter, const char *key, u32 value);

ARK_EXPORT(bool) carbon_insert_prop_u64(struct carbon_insert *inserter, const char *key, u64 value);

ARK_EXPORT(bool) carbon_insert_prop_i8(struct carbon_insert *inserter, const char *key, i8 value);

ARK_EXPORT(bool) carbon_insert_prop_i16(struct carbon_insert *inserter, const char *key, i16 value);

ARK_EXPORT(bool) carbon_insert_prop_i32(struct carbon_insert *inserter, const char *key, i32 value);

ARK_EXPORT(bool) carbon_insert_prop_i64(struct carbon_insert *inserter, const char *key, i64 value);

ARK_EXPORT(bool) carbon_insert_prop_unsigned(struct carbon_insert *inserter, const char *key, u64 value);

ARK_EXPORT(bool) carbon_insert_prop_signed(struct carbon_insert *inserter, const char *key, i64 value);

ARK_EXPORT(bool) carbon_insert_prop_float(struct carbon_insert *inserter, const char *key, float value);

ARK_EXPORT(bool) carbon_insert_prop_string(struct carbon_insert *inserter, const char *key, const char *value);

ARK_EXPORT(bool) carbon_insert_prop_binary(struct carbon_insert *inserter, const char *key, const void *value,
        size_t nbytes, const char *file_ext, const char *user_type);

ARK_EXPORT(struct carbon_insert *) carbon_insert_prop_object_begin(struct carbon_insert_object_state *out,
        struct carbon_insert *inserter, const char *key, u64 object_capacity);

ARK_EXPORT(u64) carbon_insert_prop_object_end(struct carbon_insert_object_state *state);

ARK_EXPORT(struct carbon_insert *) carbon_insert_prop_array_begin(struct carbon_insert_array_state *state,
        struct carbon_insert *inserter, const char *key, u64 array_capacity);

ARK_EXPORT(u64) carbon_insert_prop_array_end(struct carbon_insert_array_state *state);

ARK_EXPORT(struct carbon_insert *) carbon_insert_prop_column_begin(struct carbon_insert_column_state *state_out,
        struct carbon_insert *inserter_in, const char *key, enum carbon_column_type type, u64 column_capacity);

ARK_EXPORT(u64) carbon_insert_prop_column_end(struct carbon_insert_column_state *state_in);




ARK_EXPORT(bool) carbon_insert_drop(struct carbon_insert *inserter);

ARK_END_DECL

#endif