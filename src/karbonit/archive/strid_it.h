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

#ifndef STRID_ITER_H
#define STRID_ITER_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <karbonit/stdinc.h>
#include <karbonit/types.h>
#include <karbonit/archive.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct strid_info {
        archive_field_sid_t id;
        u32 strlen;
        offset_t offset;
} strid_info;

typedef struct strid_iter {
        FILE *disk_file;
        bool is_open;
        offset_t disk_offset;
        strid_info vec[100000];
} strid_iter;

bool strid_iter_open(strid_iter *it, archive *archive);
bool strid_iter_next(bool *success, strid_info **info, size_t *info_length, strid_iter *it);
bool strid_iter_close(strid_iter *it);

#ifdef __cplusplus
}
#endif

#endif
