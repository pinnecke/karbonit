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

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/carbon/commit.h>
#include <jakson/stdx/unique_id.h>
#include <jakson/std/hash.h>

bool commit_create(memfile *file)
{
        u64 init_rev = 0;
        unique_id_create(&init_rev);

        MEMFILE_ENSURE_SPACE(file, sizeof(u64));
        MEMFILE_WRITE(file, &init_rev, sizeof(u64));

        return true;
}

bool commit_skip(memfile *file)
{
        MEMFILE_SKIP(file, sizeof(u64));
        return true;
}

bool commit_read(u64 *commit_hash, memfile *file)
{
        *commit_hash = *MEMFILE_READ_TYPE(file, u64);
        return true;
}

bool commit_peek(u64 *commit_hash, memfile *file)
{
        *commit_hash = *MEMFILE_PEEK_TYPE(file, u64);
        return true;
}

bool commit_update(memfile *file, const char *base, u64 len)
{
        u64 commit_hash;
        commit_compute(&commit_hash, base, len);
        MEMFILE_WRITE(file, &commit_hash, sizeof(u64));
        return true;
}

bool commit_compute(u64 *commit_hash, const void *base, u64 len)
{
        *commit_hash = HASH64_FNV(len, base);
        return true;
}

const char *commit_to_str(str_buf *dst, u64 commit_hash)
{
        if (dst) {
                str_buf_clear(dst);
                str_buf_add_u64_as_hex(dst, commit_hash);
                return str_buf_cstr(dst);
        } else {
                return NULL;
        }
}

bool commit_append_to_str(str_buf *dst, u64 commit_hash)
{
        str_buf_add_u64_as_hex(dst, commit_hash);
        return true;
}

u64 commit_from_str(const char *commit_str)
{
        if (commit_str && strlen(commit_str) == 16) {
                char *illegal_char;
                errno = 0;
                u64 ret = strtoull(commit_str, &illegal_char, 16);
                if (ret == 0 && commit_str == illegal_char) {
                        return 0;
                } else if (ret == ULLONG_MAX && errno) {
                        return 0;
                } else if (*illegal_char) {
                        return 0;
                } else {
                        return ret;
                }
        } else {
                error(ERR_ILLEGALARG, NULL);
                return 0;
        }
}