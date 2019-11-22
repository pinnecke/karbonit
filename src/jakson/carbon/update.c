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

#include <jakson/carbon/path.h>
#include <jakson/carbon/path.h>
#include <jakson/carbon/update.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/revise.h>
#include <jakson/utils/numbers.h>

#define try_array_update(type_match, in_place_update_fn, insert_fn)                                                    \
({                                                                                                                     \
        field_e type_is = 0;                                                                            \
        arr_it_field_type(&type_is, it);                                                                      \
        bool status = false;                                                                                           \
        switch (type_is) {                                                                                             \
                case type_match:                                                                                       \
                        status = in_place_update_fn(it, value);                                                        \
                break;                                                                                                 \
                default: {                                                                                             \
                        insert in;                                                                 \
                        internal_arr_it_remove(it);                                                                    \
                        arr_it_next(it);                                                                      \
                        arr_it_insert_begin(&in, it);                                                   \
                        status = insert_fn(&in, value);                                                          \
                        arr_it_insert_end(&in);                                                         \
                break;                                                                                                 \
                }                                                                                                      \
        }                                                                                                              \
        status;                                                                                                        \
})

#define DEFINE_ARRAY_UPDATE_FUNCTION(type_name, type_match, in_place_update_fn, insert_fn)                             \
static bool array_update_##type_name(arr_it *it, type_name value)                                       \
{                                                                                                                      \
        return try_array_update(type_match, in_place_update_fn, insert_fn);                                            \
}

DEFINE_ARRAY_UPDATE_FUNCTION(u8, FIELD_NUMBER_U8, internal_arr_it_update_u8, insert_u8)

DEFINE_ARRAY_UPDATE_FUNCTION(u16, FIELD_NUMBER_U16, internal_arr_it_update_u16,
                             insert_u16)

DEFINE_ARRAY_UPDATE_FUNCTION(u32, FIELD_NUMBER_U32, internal_arr_it_update_u32,
                             insert_u32)

DEFINE_ARRAY_UPDATE_FUNCTION(u64, FIELD_NUMBER_U64, internal_arr_it_update_u64,
                             insert_u64)

DEFINE_ARRAY_UPDATE_FUNCTION(i8, FIELD_NUMBER_I8, internal_arr_it_update_i8, insert_i8)

DEFINE_ARRAY_UPDATE_FUNCTION(i16, FIELD_NUMBER_I16, internal_arr_it_update_i16,
                             insert_i16)

DEFINE_ARRAY_UPDATE_FUNCTION(i32, FIELD_NUMBER_I32, internal_arr_it_update_i32,
                             insert_i32)

DEFINE_ARRAY_UPDATE_FUNCTION(i64, FIELD_NUMBER_I64, internal_arr_it_update_i64,
                             insert_i64)

DEFINE_ARRAY_UPDATE_FUNCTION(float, FIELD_NUMBER_FLOAT, internal_arr_it_update_float,
                             insert_float)

#define try_update_generic(context, path, array_exec, column_exec)                                                     \
({                                                                                                                     \
        carbon_update updater;                                                                                   \
        if (create(&updater, context, path)) {                                                                         \
                if (resolve_path(&updater) && path_resolved(&updater)) {                                               \
                                                                                                                       \
                        switch (updater.path_evaluater.result.container) {                                        \
                        case ARRAY:                                                                              \
                                array_exec;                                                                            \
                                break;                                                                                 \
                        case COLUMN: {                                                                           \
                                u32 elem_pos;                                                                          \
                                col_it *it = column_iterator(&elem_pos, &updater);                     \
                                column_exec;                                                                           \
                        } break;                                                                                       \
                        default:                                                                                       \
                                return error(ERR_INTERNALERR, "unknown container type for update operation");                                            \
                        }                                                                                              \
                }                                                                                                      \
                dot_eval_end(&updater.path_evaluater);                                                    \
                }                                                                                                              \
        true;                                                                                                        \
})

#define try_update_value(context, path, value, array_update_fn, column_update_fn)                                      \
        try_update_generic(context, path, (array_update_fn(arrayerator(&updater), value)),                          \
                           (column_update_fn(it, elem_pos, value)) )

#define try_update(context, path, array_update_fn, column_update_fn)                                                   \
        try_update_generic(context, path, (array_update_fn(arrayerator(&updater))), (column_update_fn(it, elem_pos)))


static bool
create(carbon_update *updater, rev *context, const dot *path)
{
        updater->context = context;
        updater->path = path;

        return true;
}

static bool compile_path(dot *out, const char *in)
{
        return dot_from_string(out, in);
}

static bool resolve_path(carbon_update *updater)
{
        return dot_eval_begin_mutable(&updater->path_evaluater, updater->path, updater->context);
}

static bool path_resolved(carbon_update *updater)
{
        return dot_eval_has_result(&updater->path_evaluater);
}

static bool column_update_u8(col_it *it, u32 pos, u8 value)
{
        UNUSED(it);
        UNUSED(pos);
        UNUSED(value);
        error(ERR_NOTIMPLEMENTED, NULL);    // TODO: Implement
        return false;
}

static bool column_update_u16(col_it *it, u32 pos, u16 value)
{
        UNUSED(it);
        UNUSED(pos);
        UNUSED(value);
        error(ERR_NOTIMPLEMENTED, NULL);    // TODO: Implement
        return false;
}

static bool column_update_u32(col_it *it, u32 pos, u32 value)
{
        UNUSED(it);
        UNUSED(pos);
        UNUSED(value);
        error(ERR_NOTIMPLEMENTED, NULL);    // TODO: Implement
        return false;
}

static bool column_update_u64(col_it *it, u32 pos, u64 value)
{
        UNUSED(it);
        UNUSED(pos);
        UNUSED(value);
        error(ERR_NOTIMPLEMENTED, NULL);    // TODO: Implement
        return false;
}

static bool column_update_i8(col_it *it, u32 pos, i8 value)
{
        UNUSED(it);
        UNUSED(pos);
        UNUSED(value);
        error(ERR_NOTIMPLEMENTED, NULL);    // TODO: Implement
        return false;
}

static bool column_update_i16(col_it *it, u32 pos, i16 value)
{
        UNUSED(it);
        UNUSED(pos);
        UNUSED(value);
        error(ERR_NOTIMPLEMENTED, NULL);    // TODO: Implement
        return false;
}

static bool column_update_i32(col_it *it, u32 pos, i32 value)
{
        UNUSED(it);
        UNUSED(pos);
        UNUSED(value);
        error(ERR_NOTIMPLEMENTED, NULL);    // TODO: Implement
        return false;
}

static bool column_update_i64(col_it *it, u32 pos, i64 value)
{
        UNUSED(it);
        UNUSED(pos);
        UNUSED(value);
        error(ERR_NOTIMPLEMENTED, NULL);    // TODO: Implement
        return false;
}

static bool column_update_float(col_it *it, u32 pos, float value)
{
        UNUSED(it);
        UNUSED(pos);
        UNUSED(value);
        error(ERR_NOTIMPLEMENTED, NULL);    // TODO: Implement
        return false;
}


static inline arr_it *arrayerator(carbon_update *updater)
{
        return &updater->path_evaluater.result.containers.array.it;
}

static inline col_it *column_iterator(u32 *elem_pos, carbon_update *updater)
{
        *elem_pos = updater->path_evaluater.result.containers.column.elem_pos;
        return &updater->path_evaluater.result.containers.column.it;
}

#define compile_path_and_delegate(context, path, func)                                                                 \
({                                                                                                                     \
        bool status;                                                                                                               \
        dot compiled_path;                                                                           \
        if (compile_path(&compiled_path, path)) {                                                                      \
                status = func(context, &compiled_path);                                                                \
                dot_drop(&compiled_path);                                                                   \
        } else {                                                                                                       \
                return error(ERR_DOT_PATH_PARSERR, "path string parsing failed");                             \
        }                                                                                                              \
        status;                                                                                                        \
})

#define compile_path_and_delegate_wargs(context, path, func, ...)                                                      \
({                                                                                                                     \
        dot compiled_path;                                                                           \
        bool status;                                                                                                   \
        if (compile_path(&compiled_path, path)) {                                                                      \
                status = func(context, &compiled_path, __VA_ARGS__);                                                   \
                dot_drop(&compiled_path);                                                                   \
        } else {                                                                                                       \
                return error(ERR_DOT_PATH_PARSERR, "path string parsing failed");                             \
        }                                                                                                              \
        status;                                                                                                        \
})


bool carbon_update_set_null(rev *context, const char *path)
{
        return compile_path_and_delegate(context, path, carbon_update_set_null_compiled);
}

bool carbon_update_set_true(rev *context, const char *path)
{
        return compile_path_and_delegate(context, path, carbon_update_set_true_compiled);
}

bool carbon_update_set_false(rev *context, const char *path)
{
        return compile_path_and_delegate(context, path, carbon_update_set_false_compiled);
}

bool carbon_update_set_u8(rev *context, const char *path, u8 value)
{
        return compile_path_and_delegate_wargs(context, path, carbon_update_set_u8_compiled, value);
}

bool carbon_update_set_u16(rev *context, const char *path, u16 value)
{
        return compile_path_and_delegate_wargs(context, path, carbon_update_set_u16_compiled, value);
}

bool carbon_update_set_u32(rev *context, const char *path, u32 value)
{
        return compile_path_and_delegate_wargs(context, path, carbon_update_set_u32_compiled, value);
}

bool carbon_update_set_u64(rev *context, const char *path, u64 value)
{
        return compile_path_and_delegate_wargs(context, path, carbon_update_set_u64_compiled, value);
}

bool carbon_update_set_i8(rev *context, const char *path, i8 value)
{
        return compile_path_and_delegate_wargs(context, path, carbon_update_set_i8_compiled, value);
}

bool carbon_update_set_i16(rev *context, const char *path, i16 value)
{
        return compile_path_and_delegate_wargs(context, path, carbon_update_set_i16_compiled, value);
}

bool carbon_update_set_i32(rev *context, const char *path, i32 value)
{
        return compile_path_and_delegate_wargs(context, path, carbon_update_set_i32_compiled, value);
}

bool carbon_update_set_i64(rev *context, const char *path, i64 value)
{
        return compile_path_and_delegate_wargs(context, path, carbon_update_set_i64_compiled, value);
}

bool carbon_update_set_float(rev *context, const char *path, float value)
{
        return compile_path_and_delegate_wargs(context, path, carbon_update_set_float_compiled, value);
}

bool carbon_update_set_unsigned(rev *context, const char *path, u64 value)
{
        switch (number_min_type_unsigned(value)) {
                case NUMBER_U8:
                        return carbon_update_set_u8(context, path, (u8) value);
                case NUMBER_U16:
                        return carbon_update_set_u16(context, path, (u16) value);
                case NUMBER_U32:
                        return carbon_update_set_u32(context, path, (u32) value);
                case NUMBER_U64:
                        return carbon_update_set_u64(context, path, (u64) value);
                default:
                        return error(ERR_INTERNALERR, "update unsigned value failed: limit exeeded");
        }
}

bool carbon_update_set_signed(rev *context, const char *path, i64 value)
{
        switch (number_min_type_signed(value)) {
                case NUMBER_I8:
                        return carbon_update_set_i8(context, path, (i8) value);
                case NUMBER_I16:
                        return carbon_update_set_i16(context, path, (i16) value);
                case NUMBER_I32:
                        return carbon_update_set_i32(context, path, (i32) value);
                case NUMBER_I64:
                        return carbon_update_set_i64(context, path, (i64) value);
                default:
                        return error(ERR_INTERNALERR, "update signed value failed: limit exeeded");
        }
}

bool carbon_update_set_string(rev *context, const char *path, const char *value)
{
        // TODO: Implement
        UNUSED(context);
        UNUSED(path);
        UNUSED(value);

        return error(ERR_NOTIMPLEMENTED, "carbon_update_set_string");
}

bool carbon_update_set_binary(rev *context, const char *path, const void *value, size_t nbytes,
                              const char *file_ext, const char *user_type)
{
        // TODO: Implement
        UNUSED(context);
        UNUSED(value);
        UNUSED(nbytes);
        UNUSED(file_ext);
        UNUSED(user_type);
        UNUSED(path);
        return error(ERR_NOTIMPLEMENTED, "carbon_update_set_binary");
}

insert *carbon_update_set_array_begin(rev *context, const char *path,
                                                        insert_array_state *state_out,
                                                        u64 array_capacity)
{
        // TODO: Implement
        UNUSED(context);
        UNUSED(state_out);
        UNUSED(array_capacity);
        UNUSED(path);
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool carbon_update_set_array_end(insert_array_state *state_in)
{
        // TODO: Implement
        UNUSED(state_in);
        return error(ERR_NOTIMPLEMENTED, NULL);
}

insert *carbon_update_set_column_begin(rev *context, const char *path,
                                                         insert_column_state *state_out,
                                                         field_e type, u64 cap)
{
        // TODO: Implement
        UNUSED(state_out);
        UNUSED(context);
        UNUSED(type);
        UNUSED(cap);
        UNUSED(path);
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool carbon_update_set_column_end(insert_column_state *state_in)
{
        // TODO: Implement
        UNUSED(state_in);
        return error(ERR_NOTIMPLEMENTED, NULL);
}

// ---------------------------------------------------------------------------------------------------------------------

bool carbon_update_set_null_compiled(rev *context, const dot *path)
{
        return try_update(context, path, internal_arr_it_update_null, col_it_update_set_null);
}

bool carbon_update_set_true_compiled(rev *context, const dot *path)
{
        return try_update(context, path, internal_arr_it_update_true, col_it_update_set_true);
}

bool carbon_update_set_false_compiled(rev *context, const dot *path)
{
        return try_update(context, path, internal_arr_it_update_false, col_it_update_set_false);
}

bool carbon_update_set_u8_compiled(rev *context, const dot *path,
                                   u8 value)
{
        return try_update_value(context, path, value, array_update_u8, column_update_u8);
}

bool carbon_update_set_u16_compiled(rev *context, const dot *path,
                                    u16 value)
{
        return try_update_value(context, path, value, array_update_u16, column_update_u16);
}

bool carbon_update_set_u32_compiled(rev *context, const dot *path,
                                    u32 value)
{
        return try_update_value(context, path, value, array_update_u32, column_update_u32);
}

bool carbon_update_set_u64_compiled(rev *context, const dot *path,
                                    u64 value)
{
        return try_update_value(context, path, value, array_update_u64, column_update_u64);
}

bool carbon_update_set_i8_compiled(rev *context, const dot *path,
                                   i8 value)
{
        return try_update_value(context, path, value, array_update_i8, column_update_i8);
}

bool carbon_update_set_i16_compiled(rev *context, const dot *path,
                                    i16 value)
{
        return try_update_value(context, path, value, array_update_i16, column_update_i16);
}

bool carbon_update_set_i32_compiled(rev *context, const dot *path,
                                    i32 value)
{
        return try_update_value(context, path, value, array_update_i32, column_update_i32);
}

bool carbon_update_set_i64_compiled(rev *context, const dot *path,
                                    i64 value)
{
        return try_update_value(context, path, value, array_update_i64, column_update_i64);
}

bool carbon_update_set_float_compiled(rev *context, const dot *path,
                                      float value)
{
        return try_update_value(context, path, value, array_update_float, column_update_float);
}

bool carbon_update_set_unsigned_compiled(rev *context, const dot *path,
                                         u64 value)
{
        switch (number_min_type_unsigned(value)) {
                case NUMBER_U8:
                        return carbon_update_set_u8_compiled(context, path, (u8) value);
                case NUMBER_U16:
                        return carbon_update_set_u16_compiled(context, path, (u16) value);
                case NUMBER_U32:
                        return carbon_update_set_u32_compiled(context, path, (u32) value);
                case NUMBER_U64:
                        return carbon_update_set_u64_compiled(context, path, (u64) value);
                default:
                        return error(ERR_INTERNALERR, "unknown type for container update operation");
        }
}

bool carbon_update_set_signed_compiled(rev *context, const dot *path,
                                       i64 value)
{
        switch (number_min_type_signed(value)) {
                case NUMBER_I8:
                        return carbon_update_set_i8_compiled(context, path, (i8) value);
                case NUMBER_I16:
                        return carbon_update_set_i16_compiled(context, path, (i16) value);
                case NUMBER_I32:
                        return carbon_update_set_i32_compiled(context, path, (i32) value);
                case NUMBER_I64:
                        return carbon_update_set_i64_compiled(context, path, (i64) value);
                default:
                        return error(ERR_INTERNALERR, "unknown type for container update operation");
        }
}

bool carbon_update_set_string_compiled(rev *context, const dot *path,
                                       const char *value)
{
        // TODO: Implement
        UNUSED(context);
        UNUSED(path);
        UNUSED(value);

        return error(ERR_NOTIMPLEMENTED, "carbon_update_set_binary_compiled");
}

bool carbon_update_set_binary_compiled(rev *context, const dot *path,
                                       const void *value, size_t nbytes, const char *file_ext, const char *user_type)
{
        // TODO: Implement
        UNUSED(context);
        UNUSED(value);
        UNUSED(nbytes);
        UNUSED(file_ext);
        UNUSED(user_type);
        UNUSED(path);

        return error(ERR_NOTIMPLEMENTED, "carbon_update_set_binary_compiled");
}

insert *carbon_update_set_array_begin_compiled(rev *context,
                                                                 const dot *path,
                                                                 insert_array_state *state_out,
                                                                 u64 array_capacity)
{
        // TODO: Implement
        UNUSED(context);
        UNUSED(state_out);
        UNUSED(array_capacity);
        UNUSED(path);
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool carbon_update_set_array_end_compiled(insert_array_state *state_in)
{
        // TODO: Implement
        UNUSED(state_in);
        return error(ERR_NOTIMPLEMENTED, NULL);
}

insert *carbon_update_set_column_begin_compiled(rev *context,
                                                                  const dot *path,
                                                                  insert_column_state *state_out,
                                                                  field_e type,
                                                                  u64 cap)
{
        // TODO: Implement
        UNUSED(state_out);
        UNUSED(context);
        UNUSED(type);
        UNUSED(cap);
        UNUSED(path);
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool carbon_update_set_column_end_compiled(insert_column_state *state_in)
{
        // TODO: Implement
        UNUSED(state_in);
        return error(ERR_NOTIMPLEMENTED, NULL);
}

// ---------------------------------------------------------------------------------------------------------------------

#define revision_context_delegate_func(rev_doc, doc, func, ...)                                                        \
({                                                                                                                     \
        rev revise;                                                                                    \
        carbon_revise_begin(&revise, rev_doc, doc);                                                                     \
        bool status = func(&revise, __VA_ARGS__);                                                                      \
        carbon_revise_end(&revise);                                                                                     \
        status;                                                                                                        \
})

bool carbon_update_one_set_null(const char *dot, rec *rev_doc, rec *doc)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_null, dot);
}

bool carbon_update_one_set_true(const char *dot, rec *rev_doc, rec *doc)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_true, dot);
}

bool carbon_update_one_set_false(const char *dot, rec *rev_doc, rec *doc)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_false, dot);
}

bool carbon_update_one_set_u8(const char *dot, rec *rev_doc, rec *doc, u8 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_u8, dot, value);
}

bool carbon_update_one_set_u16(const char *dot, rec *rev_doc, rec *doc, u16 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_u16, dot, value);
}

bool carbon_update_one_set_u32(const char *dot, rec *rev_doc, rec *doc, u32 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_u32, dot, value);
}

bool carbon_update_one_set_u64(const char *dot, rec *rev_doc, rec *doc, u64 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_u64, dot, value);
}

bool carbon_update_one_set_i8(const char *dot, rec *rev_doc, rec *doc, i8 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_i8, dot, value);
}

bool carbon_update_one_set_i16(const char *dot, rec *rev_doc, rec *doc, i16 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_i16, dot, value);
}

bool carbon_update_one_set_i32(const char *dot, rec *rev_doc, rec *doc, i32 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_i32, dot, value);
}

bool carbon_update_one_set_i64(const char *dot, rec *rev_doc, rec *doc, i64 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_i64, dot, value);
}

bool carbon_update_one_set_float(const char *dot, rec *rev_doc, rec *doc,
                                 float value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_float, dot, value);
}

bool carbon_update_one_set_unsigned(const char *dot, rec *rev_doc, rec *doc,
                                    u64 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_unsigned, dot, value);
}

bool carbon_update_one_set_signed(const char *dot, rec *rev_doc, rec *doc, i64 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_signed, dot, value);
}

bool carbon_update_one_set_string(const char *dot, rec *rev_doc, rec *doc,
                                  const char *value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_string, dot, value);
}

bool carbon_update_one_set_binary(const char *dot, rec *rev_doc, rec *doc,
                                  const void *value, size_t nbytes, const char *file_ext, const char *user_type)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_binary, dot, value, nbytes,
                                              file_ext, user_type);
}

insert *carbon_update_one_set_array_begin(insert_array_state *state_out,
                                                            const char *dot, rec *rev_doc,
                                                            rec *doc, u64 array_capacity)
{
        rev revise;
        carbon_revise_begin(&revise, rev_doc, doc);
        insert *result = carbon_update_set_array_begin(&revise, dot, state_out, array_capacity);
        // ... TODO: add revision to context
        return result;
}

bool carbon_update_one_set_array_end(insert_array_state *state_in)
{
        bool status = carbon_update_set_array_end(state_in);
        // ... TODO: drop revision from context
        return status;
}

insert *carbon_update_one_set_column_begin(insert_column_state *state_out,
                                                             const char *dot, rec *rev_doc,
                                                             rec *doc, field_e type,
                                                             u64 cap)
{
        rev revise;
        carbon_revise_begin(&revise, rev_doc, doc);
        insert *result = carbon_update_set_column_begin(&revise, dot, state_out, type,
                                                                          cap);
        // ... TODO: add revision to context
        return result;
}

bool carbon_update_one_set_column_end(insert_column_state *state_in)
{
        bool status = carbon_update_set_column_end(state_in);
        // ... TODO: drop revision from context
        return status;
}

// ---------------------------------------------------------------------------------------------------------------------

bool carbon_update_one_set_null_compiled(const dot *path, rec *rev_doc,
                                         rec *doc)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_null_compiled, path);
}

bool carbon_update_one_set_true_compiled(const dot *path, rec *rev_doc,
                                         rec *doc)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_true_compiled, path);
}

bool carbon_update_one_set_false_compiled(const dot *path, rec *rev_doc,
                                          rec *doc)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_false_compiled, path);
}

bool carbon_update_one_set_u8_compiled(const dot *path, rec *rev_doc,
                                       rec *doc, u8 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_u8_compiled, path, value);
}

bool carbon_update_one_set_u16_compiled(const dot *path, rec *rev_doc,
                                        rec *doc, u16 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_u16_compiled, path, value);
}

bool carbon_update_one_set_u32_compiled(const dot *path, rec *rev_doc,
                                        rec *doc, u32 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_u32_compiled, path, value);
}

bool carbon_update_one_set_u64_compiled(const dot *path, rec *rev_doc,
                                        rec *doc, u64 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_u64_compiled, path, value);
}

bool carbon_update_one_set_i8_compiled(const dot *path, rec *rev_doc,
                                       rec *doc, i8 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_i8_compiled, path, value);
}

bool carbon_update_one_set_i16_compiled(const dot *path, rec *rev_doc,
                                        rec *doc, i16 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_i16_compiled, path, value);
}

bool carbon_update_one_set_i32_compiled(const dot *path, rec *rev_doc,
                                        rec *doc, i32 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_i32_compiled, path, value);
}

bool carbon_update_one_set_i64_compiled(const dot *path, rec *rev_doc,
                                        rec *doc, i64 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_i64_compiled, path, value);
}

bool carbon_update_one_set_float_compiled(const dot *path, rec *rev_doc,
                                          rec *doc, float value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_float_compiled, path, value);
}

bool carbon_update_one_set_unsigned_compiled(const dot *path, rec *rev_doc,
                                             rec *doc, u64 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_unsigned_compiled, path, value);
}

bool carbon_update_one_set_signed_compiled(const dot *path, rec *rev_doc,
                                           rec *doc, i64 value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_signed_compiled, path, value);
}

bool carbon_update_one_set_string_compiled(const dot *path, rec *rev_doc,
                                           rec *doc, const char *value)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_string_compiled, path, value);
}

bool carbon_update_one_set_binary_compiled(const dot *path, rec *rev_doc,
                                           rec *doc, const void *value, size_t nbytes,
                                           const char *file_ext,
                                           const char *user_type)
{
        return revision_context_delegate_func(rev_doc, doc, carbon_update_set_binary_compiled, path, value, nbytes,
                                              file_ext, user_type);
}

insert *carbon_update_one_set_array_begin_compiled(insert_array_state *state_out,
                                                                     const dot *path,
                                                                     rec *rev_doc, rec *doc,
                                                                     u64 array_capacity)
{
        rev revise;
        carbon_revise_begin(&revise, rev_doc, doc);
        insert *result = carbon_update_set_array_begin_compiled(&revise, path, state_out,
                                                                                  array_capacity);
        // ... TODO: add revision to context
        return result;
}

bool carbon_update_one_set_array_end_compiled(insert_array_state *state_in)
{
        bool status = carbon_update_set_array_end_compiled(state_in);
        // ... TODO: drop revision from context
        return status;
}

insert *carbon_update_one_set_column_begin_compiled(
        insert_column_state *state_out, const dot *path,
        rec *rev_doc,
        rec *doc, field_e type, u64 cap)
{
        rev revise;
        carbon_revise_begin(&revise, rev_doc, doc);
        insert *result = carbon_update_set_column_begin_compiled(&revise, path, state_out, type,
                                                                                   cap);
        // ... TODO: add revision to context
        return result;
}

bool carbon_update_one_set_column_end_compiled(insert_column_state *state_in)
{
        bool status = carbon_update_set_column_end_compiled(state_in);
        // ... TODO: drop revision from context
        return status;
}