/**
 * BISON Adaptive Binary JSON -- Copyright 2019 Marcus Pinnecke
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

#ifndef BISON_UPDATE_H
#define BISON_UPDATE_H


#include "shared/common.h"
#include "shared/error.h"
#include "core/mem/block.h"
#include "core/mem/file.h"
#include "core/async/spin.h"
#include "core/bison/bison.h"
#include "core/bison/bison-dot.h"
#include "core/bison/bison-path.h"

NG5_BEGIN_DECL

struct bison_update
{
        struct bison_revise *context;
        struct bison_path_evaluator path_evaluater;
        struct bison_dot_path path;
        struct err err;
        bool is_found;
};

NG5_DEFINE_ERROR_GETTER(bison_update)

NG5_EXPORT(bool) bison_update_null(struct bison_revise *context, const char *path);

NG5_EXPORT(bool) bison_update_true(struct bison_revise *context, const char *path);

NG5_EXPORT(bool) bison_update_false(struct bison_revise *context, const char *path);

NG5_EXPORT(bool) bison_update_u8(struct bison_revise *context, const char *path, u8 value);

NG5_EXPORT(bool) bison_update_u16(struct bison_revise *context, const char *path, u16 value);

NG5_EXPORT(bool) bison_update_u32(struct bison_revise *context, const char *path, u32 value);

NG5_EXPORT(bool) bison_update_u64(struct bison_revise *context, const char *path, u64 value);

NG5_EXPORT(bool) bison_update_i8(struct bison_revise *context, const char *path, i8 value);

NG5_EXPORT(bool) bison_update_i16(struct bison_revise *context, const char *path, i16 value);

NG5_EXPORT(bool) bison_update_i32(struct bison_revise *context, const char *path, i32 value);

NG5_EXPORT(bool) bison_update_i64(struct bison_revise *context, const char *path, i64 value);

NG5_EXPORT(bool) bison_update_float(struct bison_revise *context, const char *path, float value);





NG5_EXPORT(bool) bison_update_unsigned(struct bison_update *updater, u64 value);

NG5_EXPORT(bool) bison_update_signed(struct bison_update *updater, i64 value);



NG5_EXPORT(bool) bison_update_string(struct bison_update *updater, const char *value);

NG5_EXPORT(bool) bison_update_binary(struct bison_update *updater, const void *value, size_t nbytes,
        const char *file_ext, const char *user_type);

NG5_END_DECL

#endif
