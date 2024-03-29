/*
 * update - update operations on records, containers and fields
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_UPDATE_H
#define HAD_UPDATE_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <karbonit/stdinc.h>
#include <karbonit/error.h>
#include <karbonit/mem/memblock.h>
#include <karbonit/mem/memfile.h>
#include <karbonit/std/spinlock.h>
#include <karbonit/rec.h>
#include <karbonit/carbon/dot.h>
#include <karbonit/carbon/dot-eval.h>
#include <karbonit/carbon/internal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct update {
        rev *context;
        dot_eval eval;
        const dot *path;
} update;

bool update_set_null(rev *context, const char *path);
bool update_set_true(rev *context, const char *path);
bool update_set_false(rev *context, const char *path);
bool update_set_u8(rev *context, const char *path, u8 value);
bool update_set_u16(rev *context, const char *path, u16 value);
bool update_set_u32(rev *context, const char *path, u32 value);
bool update_set_u64(rev *context, const char *path, u64 value);
bool update_set_i8(rev *context, const char *path, i8 value);
bool update_set_i16(rev *context, const char *path, i16 value);
bool update_set_i32(rev *context, const char *path, i32 value);
bool update_set_i64(rev *context, const char *path, i64 value);
bool update_set_float(rev *context, const char *path, float value);
bool update_set_unsigned(rev *context, const char *path, u64 value);
bool update_set_signed(rev *context, const char *path, i64 value);
bool update_set_string(rev *context, const char *path, const char *value);
bool update_set_binary(rev *context, const char *path, const void *value, size_t nbytes, const char *file_ext, const char *user_type);

insert *update_set_array_begin(rev *context, const char *path, arr_state *state_out, u64 array_capacity);
bool update_set_array_end(arr_state *state_in);

insert *update_set_column_begin(rev *context, const char *path, col_state *state_out, field_e type, u64 cap);
bool update_set_column_end(col_state *state_in);

bool update_set_null_compiled(rev *context, const dot *path);
bool update_set_true_compiled(rev *context, const dot *path);
bool update_set_false_compiled(rev *context, const dot *path);

bool update_set_u8_compiled(rev *context, const dot *path, u8 value);
bool update_set_u16_compiled(rev *context, const dot *path, u16 value);
bool update_set_u32_compiled(rev *context, const dot *path, u32 value);
bool update_set_u64_compiled(rev *context, const dot *path, u64 value);
bool update_set_i8_compiled(rev *context, const dot *path, i8 value);
bool update_set_i16_compiled(rev *context, const dot *path, i16 value);
bool update_set_i32_compiled(rev *context, const dot *path, i32 value);
bool update_set_i64_compiled(rev *context, const dot *path, i64 value);
bool update_set_float_compiled(rev *context, const dot *path, float value);
bool update_set_unsigned_compiled(rev *context, const dot *path, u64 value);
bool update_set_signed_compiled(rev *context, const dot *path, i64 value);
bool update_set_string_compiled(rev *context, const dot *path, const char *value);
bool update_set_binary_compiled(rev *context, const dot *path, const void *value, size_t nbytes, const char *file_ext, const char *user_type);
insert * update_set_array_begin_compiled(rev *context, const dot *path, arr_state *state_out, u64 array_capacity);
bool update_set_array_end_compiled(arr_state *state_in);
insert *update_set_column_begin_compiled(rev *context, const dot *path, col_state *state_out, field_e type, u64 cap);
bool update_set_column_end_compiled(col_state *state_in);

bool update_one_set_null(const char *dot, rec *rev_doc, rec *doc);
bool update_one_set_true(const char *dot, rec *rev_doc, rec *doc);
bool update_one_set_false(const char *dot, rec *rev_doc, rec *doc);
bool update_one_set_u8(const char *dot, rec *rev_doc, rec *doc, u8 value);
bool update_one_set_u16(const char *dot, rec *rev_doc, rec *doc, u16 value);
bool update_one_set_u32(const char *dot, rec *rev_doc, rec *doc, u32 value);
bool update_one_set_u64(const char *dot, rec *rev_doc, rec *doc, u64 value);
bool update_one_set_i8(const char *dot, rec *rev_doc, rec *doc, i8 value);
bool update_one_set_i16(const char *dot, rec *rev_doc, rec *doc, i16 value);
bool update_one_set_i32(const char *dot, rec *rev_doc, rec *doc, i32 value);
bool update_one_set_i64(const char *dot, rec *rev_doc, rec *doc, i64 value);
bool update_one_set_float(const char *dot, rec *rev_doc, rec *doc, float value);
bool update_one_set_unsigned(const char *dot, rec *rev_doc, rec *doc, u64 value);
bool update_one_set_signed(const char *dot, rec *rev_doc, rec *doc, i64 value);
bool update_one_set_string(const char *dot, rec *rev_doc, rec *doc, const char *value);
bool update_one_set_binary(const char *dot, rec *rev_doc, rec *doc, const void *value, size_t nbytes, const char *file_ext, const char *user_type);
insert *update_one_set_array_begin(arr_state *state_out, const char *dot, rec *rev_doc, rec *doc, u64 array_capacity);
bool update_one_set_array_end(arr_state *state_in);

insert *update_one_set_column_begin(col_state *state_out, const char *dot, rec *rev_doc, rec *doc, field_e type, u64 cap);
bool update_one_set_column_end(col_state *state_in);

bool update_one_set_null_compiled(const dot *path, rec *rev_doc, rec *doc);
bool update_one_set_true_compiled(const dot *path, rec *rev_doc, rec *doc);
bool update_one_set_false_compiled(const dot *path, rec *rev_doc, rec *doc);
bool update_one_set_u8_compiled(const dot *path, rec *rev_doc, rec *doc, u8 value);
bool update_one_set_u16_compiled(const dot *path, rec *rev_doc, rec *doc, u16 value);
bool update_one_set_u32_compiled(const dot *path, rec *rev_doc, rec *doc, u32 value);
bool update_one_set_u64_compiled(const dot *path, rec *rev_doc, rec *doc, u64 value);
bool update_one_set_i8_compiled(const dot *path, rec *rev_doc, rec *doc, i8 value);
bool update_one_set_i16_compiled(const dot *path, rec *rev_doc, rec *doc, i16 value);
bool update_one_set_i32_compiled(const dot *path, rec *rev_doc, rec *doc, i32 value);
bool update_one_set_i64_compiled(const dot *path, rec *rev_doc, rec *doc, i64 value);
bool update_one_set_float_compiled(const dot *path, rec *rev_doc, rec *doc, float value);
bool update_one_set_unsigned_compiled(const dot *path, rec *rev_doc, rec *doc, u64 value);
bool update_one_set_signed_compiled(const dot *path, rec *rev_doc, rec *doc, i64 value);
bool update_one_set_string_compiled(const dot *path, rec *rev_doc, rec *doc, const char *value);
bool update_one_set_binary_compiled(const dot *path, rec *rev_doc, rec *doc, const void *value, size_t nbytes, const char *file_ext, const char *user_type);
insert *update_one_set_array_begin_compiled(arr_state *state_out, const dot *path, rec *rev_doc, rec *doc, u64 array_capacity);
bool update_one_set_array_end_compiled(arr_state *state_in);

insert *update_one_set_column_begin_compiled(col_state *state_out, const dot *path, rec *rev_doc, rec *doc, field_e type, u64 cap);
bool update_one_set_column_end_compiled(col_state *state_in);

#ifdef __cplusplus
}
#endif

#endif
