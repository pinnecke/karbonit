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

#ifndef JAK_CARBON_KEY_H
#define JAK_CARBON_KEY_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jak_stdinc.h>
#include <jak_error.h>
#include <jak_carbon.h>
#include <jak_memfile.h>

JAK_BEGIN_DECL

bool jak_carbon_key_create(jak_memfile *file, jak_carbon_key_e type, jak_error *err);
bool jak_carbon_key_skip(jak_carbon_key_e *out, jak_memfile *file);
bool jak_carbon_key_read_type(jak_carbon_key_e *out, jak_memfile *file);
bool jak_carbon_key_write_unsigned(jak_memfile *file, jak_u64 key);
bool jak_carbon_key_write_signed(jak_memfile *file, jak_i64 key);
bool jak_carbon_key_write_string(jak_memfile *file, const char *key);
bool jak_carbon_key_update_string(jak_memfile *file, const char *key);
bool jak_carbon_key_update_jak_string_wnchar(jak_memfile *file, const char *key, size_t length);
const void *jak_carbon_key_read(jak_u64 *len, jak_carbon_key_e *out, jak_memfile *file);
const char *jak_carbon_key_type_str(jak_carbon_key_e type);

JAK_END_DECL

#endif