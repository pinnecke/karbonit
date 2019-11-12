/**
 * Copyright 2018 Marcus Pinnecke
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

#ifndef MEMBLOCK_H
#define MEMBLOCK_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/error.h>

#ifdef __cplusplus
extern "C" {
#endif

bool memblock_create(area **block, size_t size);
bool memblock_drop(area *block);

bool memblock_from_file(area **block, FILE *file, size_t nbytes);
bool memblock_from_raw_data(area **block, const void *data, size_t nbytes);

bool memblock_get_error(err *out, area *block);

bool memblock_zero_out(area *block);
bool memblock_size(offset_t *size, const area *block);
offset_t memblock_last_used_byte(const area *block);
bool memblock_write_to_file(FILE *file, const area *block);
const char *memblock_raw_data(const area *block);
bool memblock_resize(area *block, size_t size);
bool memblock_write(area *block, offset_t position, const char *data, offset_t nbytes);
bool memblock_cpy(area **dst, area *src);
bool memblock_shrink(area *block);
bool memblock_move_right(area *block, offset_t where, size_t nbytes);
bool memblock_move_left(area *block, offset_t where, size_t nbytes);
bool memblock_move_ex(area *block, offset_t where, size_t nbytes, bool zero_out);
void *memblock_move_contents_and_drop(area *block);
bool memfile_update_last_byte(area *block, size_t where);

#ifdef __cplusplus
}
#endif

#endif
