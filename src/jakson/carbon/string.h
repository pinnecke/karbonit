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

#ifndef CARBON_STRING_H
#define CARBON_STRING_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/mem/file.h>

BEGIN_DECL

bool carbon_string_write(struct carbon_memfile *file, const char *string);
bool carbon_string_nchar_write(struct carbon_memfile *file, const char *string, carbon_u64 str_len);
bool carbon_string_nomarker_write(struct carbon_memfile *file, const char *string);
bool carbon_string_nomarker_nchar_write(struct carbon_memfile *file, const char *string, carbon_u64 str_len);
bool carbon_string_nomarker_remove(struct carbon_memfile *file);
bool carbon_string_remove(struct carbon_memfile *file);
bool carbon_string_update(struct carbon_memfile *file, const char *string);
bool carbon_string_update_wnchar(struct carbon_memfile *file, const char *string, size_t str_len);
bool carbon_string_skip(struct carbon_memfile *file);
bool carbon_string_nomarker_skip(struct carbon_memfile *file);
const char *carbon_string_read(carbon_u64 *len, struct carbon_memfile *file);
const char *carbon_string_nomarker_read(carbon_u64 *len, struct carbon_memfile *file);

END_DECL

#endif
