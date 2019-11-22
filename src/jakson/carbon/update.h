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

#ifndef CARBON_UPDATE_H
#define CARBON_UPDATE_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/mem/block.h>
#include <jakson/mem/file.h>
#include <jakson/std/spinlock.h>
#include <jakson/rec.h>
#include <jakson/carbon/dot.h>
#include <jakson/carbon/path.h>
#include <jakson/carbon/internal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct carbon_update {
        rev *context;
        dot_eval path_evaluater;
        const dot *path;
        bool is_found;
} carbon_update;

bool carbon_update_set_null(rev *context, const char *path);
bool carbon_update_set_true(rev *context, const char *path);
bool carbon_update_set_false(rev *context, const char *path);
bool carbon_update_set_u8(rev *context, const char *path, u8 value);
bool carbon_update_set_u16(rev *context, const char *path, u16 value);
bool carbon_update_set_u32(rev *context, const char *path, u32 value);
bool carbon_update_set_u64(rev *context, const char *path, u64 value);
bool carbon_update_set_i8(rev *context, const char *path, i8 value);
bool carbon_update_set_i16(rev *context, const char *path, i16 value);
bool carbon_update_set_i32(rev *context, const char *path, i32 value);
bool carbon_update_set_i64(rev *context, const char *path, i64 value);
bool carbon_update_set_float(rev *context, const char *path, float value);
bool carbon_update_set_unsigned(rev *context, const char *path, u64 value);
bool carbon_update_set_signed(rev *context, const char *path, i64 value);
bool carbon_update_set_string(rev *context, const char *path, const char *value);
bool carbon_update_set_binary(rev *context, const char *path, const void *value, size_t nbytes, const char *file_ext, const char *user_type);

insert *carbon_update_set_array_begin(rev *context, const char *path, arr_state *state_out, u64 array_capacity);
bool carbon_update_set_array_end(arr_state *state_in);

insert *carbon_update_set_column_begin(rev *context, const char *path, col_state *state_out, field_e type, u64 cap);
bool carbon_update_set_column_end(col_state *state_in);

bool carbon_update_set_null_compiled(rev *context, const dot *path);
bool carbon_update_set_true_compiled(rev *context, const dot *path);
bool carbon_update_set_false_compiled(rev *context, const dot *path);

bool carbon_update_set_u8_compiled(rev *context, const dot *path, u8 value);
bool carbon_update_set_u16_compiled(rev *context, const dot *path, u16 value);
bool carbon_update_set_u32_compiled(rev *context, const dot *path, u32 value);
bool carbon_update_set_u64_compiled(rev *context, const dot *path, u64 value);
bool carbon_update_set_i8_compiled(rev *context, const dot *path, i8 value);
bool carbon_update_set_i16_compiled(rev *context, const dot *path, i16 value);
bool carbon_update_set_i32_compiled(rev *context, const dot *path, i32 value);
bool carbon_update_set_i64_compiled(rev *context, const dot *path, i64 value);
bool carbon_update_set_float_compiled(rev *context, const dot *path, float value);
bool carbon_update_set_unsigned_compiled(rev *context, const dot *path, u64 value);
bool carbon_update_set_signed_compiled(rev *context, const dot *path, i64 value);
bool carbon_update_set_string_compiled(rev *context, const dot *path, const char *value);
bool carbon_update_set_binary_compiled(rev *context, const dot *path, const void *value, size_t nbytes, const char *file_ext, const char *user_type);
insert * carbon_update_set_array_begin_compiled(rev *context, const dot *path, arr_state *state_out, u64 array_capacity);
bool carbon_update_set_array_end_compiled(arr_state *state_in);
insert *carbon_update_set_column_begin_compiled(rev *context, const dot *path, col_state *state_out, field_e type, u64 cap);
bool carbon_update_set_column_end_compiled(col_state *state_in);

bool carbon_update_one_set_null(const char *dot, rec *rev_doc, rec *doc);
bool carbon_update_one_set_true(const char *dot, rec *rev_doc, rec *doc);
bool carbon_update_one_set_false(const char *dot, rec *rev_doc, rec *doc);
bool carbon_update_one_set_u8(const char *dot, rec *rev_doc, rec *doc, u8 value);
bool carbon_update_one_set_u16(const char *dot, rec *rev_doc, rec *doc, u16 value);
bool carbon_update_one_set_u32(const char *dot, rec *rev_doc, rec *doc, u32 value);
bool carbon_update_one_set_u64(const char *dot, rec *rev_doc, rec *doc, u64 value);
bool carbon_update_one_set_i8(const char *dot, rec *rev_doc, rec *doc, i8 value);
bool carbon_update_one_set_i16(const char *dot, rec *rev_doc, rec *doc, i16 value);
bool carbon_update_one_set_i32(const char *dot, rec *rev_doc, rec *doc, i32 value);
bool carbon_update_one_set_i64(const char *dot, rec *rev_doc, rec *doc, i64 value);
bool carbon_update_one_set_float(const char *dot, rec *rev_doc, rec *doc, float value);
bool carbon_update_one_set_unsigned(const char *dot, rec *rev_doc, rec *doc, u64 value);
bool carbon_update_one_set_signed(const char *dot, rec *rev_doc, rec *doc, i64 value);
bool carbon_update_one_set_string(const char *dot, rec *rev_doc, rec *doc, const char *value);
bool carbon_update_one_set_binary(const char *dot, rec *rev_doc, rec *doc, const void *value, size_t nbytes, const char *file_ext, const char *user_type);
insert *carbon_update_one_set_array_begin(arr_state *state_out, const char *dot, rec *rev_doc, rec *doc, u64 array_capacity);
bool carbon_update_one_set_array_end(arr_state *state_in);

insert *carbon_update_one_set_column_begin(col_state *state_out, const char *dot, rec *rev_doc, rec *doc, field_e type, u64 cap);
bool carbon_update_one_set_column_end(col_state *state_in);

bool carbon_update_one_set_null_compiled(const dot *path, rec *rev_doc, rec *doc);
bool carbon_update_one_set_true_compiled(const dot *path, rec *rev_doc, rec *doc);
bool carbon_update_one_set_false_compiled(const dot *path, rec *rev_doc, rec *doc);
bool carbon_update_one_set_u8_compiled(const dot *path, rec *rev_doc, rec *doc, u8 value);
bool carbon_update_one_set_u16_compiled(const dot *path, rec *rev_doc, rec *doc, u16 value);
bool carbon_update_one_set_u32_compiled(const dot *path, rec *rev_doc, rec *doc, u32 value);
bool carbon_update_one_set_u64_compiled(const dot *path, rec *rev_doc, rec *doc, u64 value);
bool carbon_update_one_set_i8_compiled(const dot *path, rec *rev_doc, rec *doc, i8 value);
bool carbon_update_one_set_i16_compiled(const dot *path, rec *rev_doc, rec *doc, i16 value);
bool carbon_update_one_set_i32_compiled(const dot *path, rec *rev_doc, rec *doc, i32 value);
bool carbon_update_one_set_i64_compiled(const dot *path, rec *rev_doc, rec *doc, i64 value);
bool carbon_update_one_set_float_compiled(const dot *path, rec *rev_doc, rec *doc, float value);
bool carbon_update_one_set_unsigned_compiled(const dot *path, rec *rev_doc, rec *doc, u64 value);
bool carbon_update_one_set_signed_compiled(const dot *path, rec *rev_doc, rec *doc, i64 value);
bool carbon_update_one_set_string_compiled(const dot *path, rec *rev_doc, rec *doc, const char *value);
bool carbon_update_one_set_binary_compiled(const dot *path, rec *rev_doc, rec *doc, const void *value, size_t nbytes, const char *file_ext, const char *user_type);
insert *carbon_update_one_set_array_begin_compiled(arr_state *state_out, const dot *path, rec *rev_doc, rec *doc, u64 array_capacity);
bool carbon_update_one_set_array_end_compiled(arr_state *state_in);

insert *carbon_update_one_set_column_begin_compiled(col_state *state_out, const dot *path, rec *rev_doc, rec *doc, field_e type, u64 cap);
bool carbon_update_one_set_column_end_compiled(col_state *state_in);

#ifdef __cplusplus
}
#endif

#endif
