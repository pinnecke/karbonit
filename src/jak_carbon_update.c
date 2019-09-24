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

#include <jak_carbon_path.h>
#include <jak_carbon_path.h>
#include <jak_carbon_update.h>
#include <jak_carbon_insert.h>
#include <jak_carbon_revise.h>
#include <jak_utils_numbers.h>

#define try_array_update(type_match, in_place_update_fn, insert_fn)                                                    \
({                                                                                                                     \
        jak_carbon_field_type_e type_is = 0;                                                                            \
        jak_carbon_array_it_field_type(&type_is, it);                                                                      \
        bool status = false;                                                                                           \
        switch (type_is) {                                                                                             \
                case type_match:                                                                                       \
                        status = in_place_update_fn(it, value);                                                        \
                break;                                                                                                 \
                default: {                                                                                             \
                        jak_carbon_insert inserter;                                                                 \
                        jak_carbon_array_it_remove(it);                                                                    \
                        jak_carbon_array_it_next(it);                                                                      \
                        jak_carbon_array_it_insert_begin(&inserter, it);                                                   \
                        status = insert_fn(&inserter, value);                                                          \
                        jak_carbon_array_it_insert_end(&inserter);                                                         \
                break;                                                                                                 \
                }                                                                                                      \
        }                                                                                                              \
        status;                                                                                                        \
})

#define DEFINE_ARRAY_UPDATE_FUNCTION(type_name, type_match, in_place_update_fn, insert_fn)                             \
static bool array_update_##type_name(jak_carbon_array_it *it, jak_##type_name value)                                       \
{                                                                                                                      \
        return try_array_update(type_match, in_place_update_fn, insert_fn);                                            \
}

DEFINE_ARRAY_UPDATE_FUNCTION(u8, JAK_CARBON_FIELD_TYPE_NUMBER_U8, jak_carbon_array_it_update_in_place_u8, jak_carbon_insert_u8)

DEFINE_ARRAY_UPDATE_FUNCTION(u16, JAK_CARBON_FIELD_TYPE_NUMBER_U16, jak_carbon_array_it_update_in_place_u16,
                             jak_carbon_insert_u16)

DEFINE_ARRAY_UPDATE_FUNCTION(u32, JAK_CARBON_FIELD_TYPE_NUMBER_U32, jak_carbon_array_it_update_in_place_u32,
                             jak_carbon_insert_u32)

DEFINE_ARRAY_UPDATE_FUNCTION(u64, JAK_CARBON_FIELD_TYPE_NUMBER_U64, jak_carbon_array_it_update_in_place_u64,
                             jak_carbon_insert_u64)

DEFINE_ARRAY_UPDATE_FUNCTION(i8, JAK_CARBON_FIELD_TYPE_NUMBER_I8, jak_carbon_array_it_update_in_place_i8, jak_carbon_insert_i8)

DEFINE_ARRAY_UPDATE_FUNCTION(i16, JAK_CARBON_FIELD_TYPE_NUMBER_I16, jak_carbon_array_it_update_in_place_i16,
                             jak_carbon_insert_i16)

DEFINE_ARRAY_UPDATE_FUNCTION(i32, JAK_CARBON_FIELD_TYPE_NUMBER_I32, jak_carbon_array_it_update_in_place_i32,
                             jak_carbon_insert_i32)

DEFINE_ARRAY_UPDATE_FUNCTION(i64, JAK_CARBON_FIELD_TYPE_NUMBER_I64, jak_carbon_array_it_update_in_place_i64,
                             jak_carbon_insert_i64)

DEFINE_ARRAY_UPDATE_FUNCTION(float, JAK_CARBON_FIELD_TYPE_NUMBER_FLOAT, jak_carbon_array_it_update_in_place_float,
                             jak_carbon_insert_float)

#define try_update_generic(context, path, array_exec, column_exec)                                                     \
({                                                                                                                     \
        JAK_ERROR_IF_NULL(context)                                                                                         \
        JAK_ERROR_IF_NULL(path)                                                                                            \
        bool status = false;                                                                                           \
        jak_carbon_update updater;                                                                                   \
        if (create(&updater, context, path)) {                                                                         \
                if (resolve_path(&updater) && path_resolved(&updater)) {                                               \
                                                                                                                       \
                        switch (updater.path_evaluater.result.container_type) {                                        \
                        case JAK_CARBON_ARRAY:                                                                              \
                                array_exec;                                                                            \
                                status = true;                                                                         \
                                break;                                                                                 \
                        case JAK_CARBON_COLUMN: {                                                                           \
                                jak_u32 elem_pos;                                                                          \
                                jak_carbon_column_it *it = column_iterator(&elem_pos, &updater);                     \
                                column_exec;                                                                           \
                                status = true;                                                                         \
                        } break;                                                                                       \
                        default:                                                                                       \
                        JAK_ERROR(&context->original->err, JAK_ERR_INTERNALERR)                                            \
                        }                                                                                              \
                }                                                                                                      \
                jak_carbon_path_evaluator_end(&updater.path_evaluater);                                                    \
                }                                                                                                              \
        status;                                                                                                        \
})

#define try_update_value(context, path, value, array_update_fn, column_update_fn)                                      \
        try_update_generic(context, path, (array_update_fn(array_iterator(&updater), value)),                          \
                           (column_update_fn(it, elem_pos, value)) )

#define try_update(context, path, array_update_fn, column_update_fn)                                                   \
        try_update_generic(context, path, (array_update_fn(array_iterator(&updater))), (column_update_fn(it, elem_pos)))


static bool
create(jak_carbon_update *updater, jak_carbon_revise *context, const jak_carbon_dot_path *path)
{
        JAK_ERROR_IF_NULL(updater)
        JAK_ERROR_IF_NULL(context)
        JAK_ERROR_IF_NULL(path)

        jak_error_init(&updater->err);
        updater->context = context;
        updater->path = path;

        return true;
}

static bool compile_path(jak_carbon_dot_path *out, const char *in)
{
        return jak_carbon_dot_path_from_string(out, in);
}

static bool resolve_path(jak_carbon_update *updater)
{
        return jak_carbon_path_evaluator_begin_mutable(&updater->path_evaluater, updater->path, updater->context);
}

static bool path_resolved(jak_carbon_update *updater)
{
        return jak_carbon_path_evaluator_has_result(&updater->path_evaluater);
}

static bool column_update_u8(jak_carbon_column_it *it, jak_u32 pos, jak_u8 value)
{
        JAK_UNUSED(it);
        JAK_UNUSED(pos);
        JAK_UNUSED(value);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED);    // TODO: Implement
        return false;
}

static bool column_update_u16(jak_carbon_column_it *it, jak_u32 pos, jak_u16 value)
{
        JAK_UNUSED(it);
        JAK_UNUSED(pos);
        JAK_UNUSED(value);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED);    // TODO: Implement
        return false;
}

static bool column_update_u32(jak_carbon_column_it *it, jak_u32 pos, jak_u32 value)
{
        JAK_UNUSED(it);
        JAK_UNUSED(pos);
        JAK_UNUSED(value);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED);    // TODO: Implement
        return false;
}

static bool column_update_u64(jak_carbon_column_it *it, jak_u32 pos, jak_u64 value)
{
        JAK_UNUSED(it);
        JAK_UNUSED(pos);
        JAK_UNUSED(value);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED);    // TODO: Implement
        return false;
}

static bool column_update_i8(jak_carbon_column_it *it, jak_u32 pos, jak_i8 value)
{
        JAK_UNUSED(it);
        JAK_UNUSED(pos);
        JAK_UNUSED(value);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED);    // TODO: Implement
        return false;
}

static bool column_update_i16(jak_carbon_column_it *it, jak_u32 pos, jak_i16 value)
{
        JAK_UNUSED(it);
        JAK_UNUSED(pos);
        JAK_UNUSED(value);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED);    // TODO: Implement
        return false;
}

static bool column_update_i32(jak_carbon_column_it *it, jak_u32 pos, jak_i32 value)
{
        JAK_UNUSED(it);
        JAK_UNUSED(pos);
        JAK_UNUSED(value);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED);    // TODO: Implement
        return false;
}

static bool column_update_i64(jak_carbon_column_it *it, jak_u32 pos, jak_i64 value)
{
        JAK_UNUSED(it);
        JAK_UNUSED(pos);
        JAK_UNUSED(value);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED);    // TODO: Implement
        return false;
}

static bool column_update_float(jak_carbon_column_it *it, jak_u32 pos, float value)
{
        JAK_UNUSED(it);
        JAK_UNUSED(pos);
        JAK_UNUSED(value);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED);    // TODO: Implement
        return false;
}


static inline jak_carbon_array_it *array_iterator(jak_carbon_update *updater)
{
        return &updater->path_evaluater.result.containers.array.it;
}

static inline jak_carbon_column_it *column_iterator(jak_u32 *elem_pos, jak_carbon_update *updater)
{
        *elem_pos = updater->path_evaluater.result.containers.column.elem_pos;
        return &updater->path_evaluater.result.containers.column.it;
}

#define compile_path_and_delegate(context, path, func)                                                                 \
({                                                                                                                     \
        JAK_ERROR_IF_NULL(context)                                                                                         \
        JAK_ERROR_IF_NULL(path)                                                                                            \
                                                                                                                       \
        jak_carbon_dot_path compiled_path;                                                                           \
        bool status;                                                                                                   \
        if (compile_path(&compiled_path, path)) {                                                                      \
                status = func(context, &compiled_path);                                                                \
                jak_carbon_dot_path_drop(&compiled_path);                                                                   \
        } else {                                                                                                       \
                JAK_ERROR(&context->err, JAK_ERR_DOT_PATH_PARSERR)                                                         \
                status = false;                                                                                        \
        }                                                                                                              \
        status;                                                                                                        \
})

#define compile_path_and_delegate_wargs(context, path, func, ...)                                                      \
({                                                                                                                     \
        JAK_ERROR_IF_NULL(context)                                                                                         \
        JAK_ERROR_IF_NULL(path)                                                                                            \
                                                                                                                       \
        jak_carbon_dot_path compiled_path;                                                                           \
        bool status;                                                                                                   \
        if (compile_path(&compiled_path, path)) {                                                                      \
                status = func(context, &compiled_path, __VA_ARGS__);                                                   \
                jak_carbon_dot_path_drop(&compiled_path);                                                                   \
        } else {                                                                                                       \
                JAK_ERROR(&context->err, JAK_ERR_DOT_PATH_PARSERR)                                                         \
                status = false;                                                                                        \
        }                                                                                                              \
        status;                                                                                                        \
})


bool jak_carbon_update_set_null(jak_carbon_revise *context, const char *path)
{
        return compile_path_and_delegate(context, path, jak_carbon_update_set_null_compiled);
}

bool jak_carbon_update_set_true(jak_carbon_revise *context, const char *path)
{
        return compile_path_and_delegate(context, path, jak_carbon_update_set_true_compiled);
}

bool jak_carbon_update_set_false(jak_carbon_revise *context, const char *path)
{
        return compile_path_and_delegate(context, path, jak_carbon_update_set_false_compiled);
}

bool jak_carbon_update_set_u8(jak_carbon_revise *context, const char *path, jak_u8 value)
{
        return compile_path_and_delegate_wargs(context, path, jak_carbon_update_set_u8_compiled, value);
}

bool jak_carbon_update_set_u16(jak_carbon_revise *context, const char *path, jak_u16 value)
{
        return compile_path_and_delegate_wargs(context, path, jak_carbon_update_set_u16_compiled, value);
}

bool jak_carbon_update_set_u32(jak_carbon_revise *context, const char *path, jak_u32 value)
{
        return compile_path_and_delegate_wargs(context, path, jak_carbon_update_set_u32_compiled, value);
}

bool jak_carbon_update_set_u64(jak_carbon_revise *context, const char *path, jak_u64 value)
{
        return compile_path_and_delegate_wargs(context, path, jak_carbon_update_set_u64_compiled, value);
}

bool jak_carbon_update_set_i8(jak_carbon_revise *context, const char *path, jak_i8 value)
{
        return compile_path_and_delegate_wargs(context, path, jak_carbon_update_set_i8_compiled, value);
}

bool jak_carbon_update_set_i16(jak_carbon_revise *context, const char *path, jak_i16 value)
{
        return compile_path_and_delegate_wargs(context, path, jak_carbon_update_set_i16_compiled, value);
}

bool jak_carbon_update_set_i32(jak_carbon_revise *context, const char *path, jak_i32 value)
{
        return compile_path_and_delegate_wargs(context, path, jak_carbon_update_set_i32_compiled, value);
}

bool jak_carbon_update_set_i64(jak_carbon_revise *context, const char *path, jak_i64 value)
{
        return compile_path_and_delegate_wargs(context, path, jak_carbon_update_set_i64_compiled, value);
}

bool jak_carbon_update_set_float(jak_carbon_revise *context, const char *path, float value)
{
        return compile_path_and_delegate_wargs(context, path, jak_carbon_update_set_float_compiled, value);
}

bool jak_carbon_update_set_unsigned(jak_carbon_revise *context, const char *path, jak_u64 value)
{
        switch (jak_number_min_type_unsigned(value)) {
                case JAK_NUMBER_U8:
                        return jak_carbon_update_set_u8(context, path, (jak_u8) value);
                case JAK_NUMBER_U16:
                        return jak_carbon_update_set_u16(context, path, (jak_u16) value);
                case JAK_NUMBER_U32:
                        return jak_carbon_update_set_u32(context, path, (jak_u32) value);
                case JAK_NUMBER_U64:
                        return jak_carbon_update_set_u64(context, path, (jak_u64) value);
                default: JAK_ERROR(&context->err, JAK_ERR_INTERNALERR);
                        return false;
        }
}

bool jak_carbon_update_set_signed(jak_carbon_revise *context, const char *path, jak_i64 value)
{
        switch (jak_number_min_type_signed(value)) {
                case JAK_NUMBER_I8:
                        return jak_carbon_update_set_i8(context, path, (jak_i8) value);
                case JAK_NUMBER_I16:
                        return jak_carbon_update_set_i16(context, path, (jak_i16) value);
                case JAK_NUMBER_I32:
                        return jak_carbon_update_set_i32(context, path, (jak_i32) value);
                case JAK_NUMBER_I64:
                        return jak_carbon_update_set_i64(context, path, (jak_i64) value);
                default: JAK_ERROR(&context->err, JAK_ERR_INTERNALERR);
                        return false;
        }
}

bool jak_carbon_update_set_string(jak_carbon_revise *context, const char *path, const char *value)
{
        // TODO: Implement
        JAK_UNUSED(context);
        JAK_UNUSED(path);
        JAK_UNUSED(value);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED)
        return false;
}

bool jak_carbon_update_set_binary(jak_carbon_revise *context, const char *path, const void *value, size_t nbytes,
                              const char *file_ext, const char *user_type)
{
        // TODO: Implement
        JAK_UNUSED(context);
        JAK_UNUSED(value);
        JAK_UNUSED(nbytes);
        JAK_UNUSED(file_ext);
        JAK_UNUSED(user_type);
        JAK_UNUSED(path);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED)
        return false;
}

jak_carbon_insert *jak_carbon_update_set_array_begin(jak_carbon_revise *context, const char *path,
                                                        jak_carbon_insert_array_state *state_out,
                                                        jak_u64 array_capacity)
{
        // TODO: Implement
        JAK_UNUSED(context);
        JAK_UNUSED(state_out);
        JAK_UNUSED(array_capacity);
        JAK_UNUSED(path);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED)
        return false;
}

bool jak_carbon_update_set_array_end(jak_carbon_insert_array_state *state_in)
{
        // TODO: Implement
        JAK_UNUSED(state_in);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED)
        return false;
}

jak_carbon_insert *jak_carbon_update_set_column_begin(jak_carbon_revise *context, const char *path,
                                                         jak_carbon_insert_column_state *state_out,
                                                         jak_carbon_field_type_e type, jak_u64 column_capacity)
{
        // TODO: Implement
        JAK_UNUSED(state_out);
        JAK_UNUSED(context);
        JAK_UNUSED(type);
        JAK_UNUSED(column_capacity);
        JAK_UNUSED(path);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED)
        return false;
}

bool jak_carbon_update_set_column_end(jak_carbon_insert_column_state *state_in)
{
        // TODO: Implement
        JAK_UNUSED(state_in);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED)
        return false;
}

// ---------------------------------------------------------------------------------------------------------------------

bool jak_carbon_update_set_null_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path)
{
        return try_update(context, path, jak_carbon_array_it_update_in_place_null, jak_carbon_column_it_update_set_null);
}

bool jak_carbon_update_set_true_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path)
{
        return try_update(context, path, jak_carbon_array_it_update_in_place_true, jak_carbon_column_it_update_set_true);
}

bool jak_carbon_update_set_false_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path)
{
        return try_update(context, path, jak_carbon_array_it_update_in_place_false, jak_carbon_column_it_update_set_false);
}

bool jak_carbon_update_set_u8_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path,
                                   jak_u8 value)
{
        return try_update_value(context, path, value, array_update_u8, column_update_u8);
}

bool jak_carbon_update_set_u16_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path,
                                    jak_u16 value)
{
        return try_update_value(context, path, value, array_update_u16, column_update_u16);
}

bool jak_carbon_update_set_u32_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path,
                                    jak_u32 value)
{
        return try_update_value(context, path, value, array_update_u32, column_update_u32);
}

bool jak_carbon_update_set_u64_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path,
                                    jak_u64 value)
{
        return try_update_value(context, path, value, array_update_u64, column_update_u64);
}

bool jak_carbon_update_set_i8_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path,
                                   jak_i8 value)
{
        return try_update_value(context, path, value, array_update_i8, column_update_i8);
}

bool jak_carbon_update_set_i16_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path,
                                    jak_i16 value)
{
        return try_update_value(context, path, value, array_update_i16, column_update_i16);
}

bool jak_carbon_update_set_i32_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path,
                                    jak_i32 value)
{
        return try_update_value(context, path, value, array_update_i32, column_update_i32);
}

bool jak_carbon_update_set_i64_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path,
                                    jak_i64 value)
{
        return try_update_value(context, path, value, array_update_i64, column_update_i64);
}

bool jak_carbon_update_set_float_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path,
                                      float value)
{
        return try_update_value(context, path, value, array_update_float, column_update_float);
}

bool jak_carbon_update_set_unsigned_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path,
                                         jak_u64 value)
{
        switch (jak_number_min_type_unsigned(value)) {
                case JAK_NUMBER_U8:
                        return jak_carbon_update_set_u8_compiled(context, path, (jak_u8) value);
                case JAK_NUMBER_U16:
                        return jak_carbon_update_set_u16_compiled(context, path, (jak_u16) value);
                case JAK_NUMBER_U32:
                        return jak_carbon_update_set_u32_compiled(context, path, (jak_u32) value);
                case JAK_NUMBER_U64:
                        return jak_carbon_update_set_u64_compiled(context, path, (jak_u64) value);
                default: JAK_ERROR(&context->err, JAK_ERR_INTERNALERR);
                        return false;
        }
}

bool jak_carbon_update_set_signed_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path,
                                       jak_i64 value)
{
        switch (jak_number_min_type_signed(value)) {
                case JAK_NUMBER_I8:
                        return jak_carbon_update_set_i8_compiled(context, path, (jak_i8) value);
                case JAK_NUMBER_I16:
                        return jak_carbon_update_set_i16_compiled(context, path, (jak_i16) value);
                case JAK_NUMBER_I32:
                        return jak_carbon_update_set_i32_compiled(context, path, (jak_i32) value);
                case JAK_NUMBER_I64:
                        return jak_carbon_update_set_i64_compiled(context, path, (jak_i64) value);
                default: JAK_ERROR(&context->err, JAK_ERR_INTERNALERR);
                        return false;
        }
}

bool jak_carbon_update_set_jak_string_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path,
                                       const char *value)
{
        // TODO: Implement
        JAK_UNUSED(context);
        JAK_UNUSED(path);
        JAK_UNUSED(value);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED)
        return false;
}

bool jak_carbon_update_set_binary_compiled(jak_carbon_revise *context, const jak_carbon_dot_path *path,
                                       const void *value, size_t nbytes, const char *file_ext, const char *user_type)
{
        // TODO: Implement
        JAK_UNUSED(context);
        JAK_UNUSED(value);
        JAK_UNUSED(nbytes);
        JAK_UNUSED(file_ext);
        JAK_UNUSED(user_type);
        JAK_UNUSED(path);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED)
        return false;
}

jak_carbon_insert *jak_carbon_update_set_array_begin_compiled(jak_carbon_revise *context,
                                                                 const jak_carbon_dot_path *path,
                                                                 jak_carbon_insert_array_state *state_out,
                                                                 jak_u64 array_capacity)
{
        // TODO: Implement
        JAK_UNUSED(context);
        JAK_UNUSED(state_out);
        JAK_UNUSED(array_capacity);
        JAK_UNUSED(path);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED)
        return false;
}

bool jak_carbon_update_set_array_end_compiled(jak_carbon_insert_array_state *state_in)
{
        // TODO: Implement
        JAK_UNUSED(state_in);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED)
        return false;
}

jak_carbon_insert *jak_carbon_update_set_column_begin_compiled(jak_carbon_revise *context,
                                                                  const jak_carbon_dot_path *path,
                                                                  jak_carbon_insert_column_state *state_out,
                                                                  jak_carbon_field_type_e type,
                                                                  jak_u64 column_capacity)
{
        // TODO: Implement
        JAK_UNUSED(state_out);
        JAK_UNUSED(context);
        JAK_UNUSED(type);
        JAK_UNUSED(column_capacity);
        JAK_UNUSED(path);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED)
        return false;
}

bool jak_carbon_update_set_column_end_compiled(jak_carbon_insert_column_state *state_in)
{
        // TODO: Implement
        JAK_UNUSED(state_in);
        JAK_ERROR_PRINT(JAK_ERR_NOTIMPLEMENTED)
        return false;
}

// ---------------------------------------------------------------------------------------------------------------------

#define revision_context_delegate_func(rev_doc, doc, func, ...)                                                        \
({                                                                                                                     \
        jak_carbon_revise revise;                                                                                    \
        jak_carbon_revise_begin(&revise, rev_doc, doc);                                                                     \
        bool status = func(&revise, __VA_ARGS__);                                                                      \
        jak_carbon_revise_end(&revise);                                                                                     \
        status;                                                                                                        \
})

bool carbon_update_one_set_null(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_null, dot_path);
}

bool carbon_update_one_set_true(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_true, dot_path);
}

bool carbon_update_one_set_false(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_false, dot_path);
}

bool carbon_update_one_set_u8(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc, jak_u8 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_u8, dot_path, value);
}

bool carbon_update_one_set_u16(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc, jak_u16 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_u16, dot_path, value);
}

bool carbon_update_one_set_u32(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc, jak_u32 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_u32, dot_path, value);
}

bool carbon_update_one_set_u64(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc, jak_u64 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_u64, dot_path, value);
}

bool carbon_update_one_set_i8(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc, jak_i8 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_i8, dot_path, value);
}

bool carbon_update_one_set_i16(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc, jak_i16 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_i16, dot_path, value);
}

bool carbon_update_one_set_i32(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc, jak_i32 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_i32, dot_path, value);
}

bool carbon_update_one_set_i64(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc, jak_i64 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_i64, dot_path, value);
}

bool carbon_update_one_set_float(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc,
                                 float value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_float, dot_path, value);
}

bool carbon_update_one_set_unsigned(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc,
                                    jak_u64 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_unsigned, dot_path, value);
}

bool
carbon_update_one_set_signed(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc, jak_i64 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_signed, dot_path, value);
}

bool carbon_update_one_set_string(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc,
                                  const char *value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_string, dot_path, value);
}

bool carbon_update_one_set_binary(const char *dot_path, jak_carbon *rev_doc, jak_carbon *doc,
                                  const void *value, size_t nbytes, const char *file_ext, const char *user_type)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_binary, dot_path, value, nbytes,
                                              file_ext, user_type);
}

jak_carbon_insert *carbon_update_one_set_array_begin(jak_carbon_insert_array_state *state_out,
                                                            const char *dot_path, jak_carbon *rev_doc,
                                                            jak_carbon *doc, jak_u64 array_capacity)
{
        jak_carbon_revise revise;
        jak_carbon_revise_begin(&revise, rev_doc, doc);
        jak_carbon_insert *result = jak_carbon_update_set_array_begin(&revise, dot_path, state_out, array_capacity);
        // ... TODO: add revision to context
        return result;
}

bool carbon_update_one_set_array_end(jak_carbon_insert_array_state *state_in)
{
        bool status = jak_carbon_update_set_array_end(state_in);
        // ... TODO: drop revision from context
        return status;
}

jak_carbon_insert *carbon_update_one_set_column_begin(jak_carbon_insert_column_state *state_out,
                                                             const char *dot_path, jak_carbon *rev_doc,
                                                             jak_carbon *doc, jak_carbon_field_type_e type,
                                                             jak_u64 column_capacity)
{
        jak_carbon_revise revise;
        jak_carbon_revise_begin(&revise, rev_doc, doc);
        jak_carbon_insert *result = jak_carbon_update_set_column_begin(&revise, dot_path, state_out, type,
                                                                          column_capacity);
        // ... TODO: add revision to context
        return result;
}

bool carbon_update_one_set_column_end(jak_carbon_insert_column_state *state_in)
{
        bool status = jak_carbon_update_set_column_end(state_in);
        // ... TODO: drop revision from context
        return status;
}

// ---------------------------------------------------------------------------------------------------------------------

bool carbon_update_one_set_null_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                         jak_carbon *doc)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_null_compiled, path);
}

bool carbon_update_one_set_true_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                         jak_carbon *doc)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_true_compiled, path);
}

bool carbon_update_one_set_false_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                          jak_carbon *doc)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_false_compiled, path);
}

bool carbon_update_one_set_u8_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                       jak_carbon *doc, jak_u8 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_u8_compiled, path, value);
}

bool carbon_update_one_set_u16_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                        jak_carbon *doc, jak_u16 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_u16_compiled, path, value);
}

bool carbon_update_one_set_u32_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                        jak_carbon *doc, jak_u32 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_u32_compiled, path, value);
}

bool carbon_update_one_set_u64_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                        jak_carbon *doc, jak_u64 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_u64_compiled, path, value);
}

bool carbon_update_one_set_i8_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                       jak_carbon *doc, jak_i8 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_i8_compiled, path, value);
}

bool carbon_update_one_set_i16_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                        jak_carbon *doc, jak_i16 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_i16_compiled, path, value);
}

bool carbon_update_one_set_i32_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                        jak_carbon *doc, jak_i32 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_i32_compiled, path, value);
}

bool carbon_update_one_set_i64_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                        jak_carbon *doc, jak_i64 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_i64_compiled, path, value);
}

bool carbon_update_one_set_float_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                          jak_carbon *doc, float value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_float_compiled, path, value);
}

bool carbon_update_one_set_unsigned_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                             jak_carbon *doc, jak_u64 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_unsigned_compiled, path, value);
}

bool carbon_update_one_set_signed_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                           jak_carbon *doc, jak_i64 value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_signed_compiled, path, value);
}

bool carbon_update_one_set_jak_string_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                           jak_carbon *doc, const char *value)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_jak_string_compiled, path, value);
}

bool carbon_update_one_set_binary_compiled(const jak_carbon_dot_path *path, jak_carbon *rev_doc,
                                           jak_carbon *doc, const void *value, size_t nbytes,
                                           const char *file_ext,
                                           const char *user_type)
{
        return revision_context_delegate_func(rev_doc, doc, jak_carbon_update_set_binary_compiled, path, value, nbytes,
                                              file_ext, user_type);
}

jak_carbon_insert *carbon_update_one_set_array_begin_compiled(jak_carbon_insert_array_state *state_out,
                                                                     const jak_carbon_dot_path *path,
                                                                     jak_carbon *rev_doc, jak_carbon *doc,
                                                                     jak_u64 array_capacity)
{
        jak_carbon_revise revise;
        jak_carbon_revise_begin(&revise, rev_doc, doc);
        jak_carbon_insert *result = jak_carbon_update_set_array_begin_compiled(&revise, path, state_out,
                                                                                  array_capacity);
        // ... TODO: add revision to context
        return result;
}

bool carbon_update_one_set_array_end_compiled(jak_carbon_insert_array_state *state_in)
{
        bool status = jak_carbon_update_set_array_end_compiled(state_in);
        // ... TODO: drop revision from context
        return status;
}

jak_carbon_insert *carbon_update_one_set_column_begin_compiled(
        jak_carbon_insert_column_state *state_out, const jak_carbon_dot_path *path,
        jak_carbon *rev_doc,
        jak_carbon *doc, jak_carbon_field_type_e type, jak_u64 column_capacity)
{
        jak_carbon_revise revise;
        jak_carbon_revise_begin(&revise, rev_doc, doc);
        jak_carbon_insert *result = jak_carbon_update_set_column_begin_compiled(&revise, path, state_out, type,
                                                                                   column_capacity);
        // ... TODO: add revision to context
        return result;
}

bool carbon_update_one_set_column_end_compiled(jak_carbon_insert_column_state *state_in)
{
        bool status = jak_carbon_update_set_column_end_compiled(state_in);
        // ... TODO: drop revision from context
        return status;
}