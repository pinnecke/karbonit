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

#ifndef COMPRESSOR_HUFFMAN_H
#define COMPRESSOR_HUFFMAN_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <karbonit/stdinc.h>
#include <karbonit/std/vec.h>
#include <karbonit/mem/memfile.h>

#ifdef __cplusplus
extern "C" {
#endif

bool pack_huffman_init(packer *self);
bool pack_coding_huffman_cpy(const packer *self, packer *dst);
bool pack_coding_huffman_drop(packer *self);
bool pack_huffman_write_extra(packer *self, memfile *dst, const vec ofType (const char *) *strings);
bool pack_huffman_read_extra(packer *self, FILE *src, size_t nbytes);
bool pack_huffman_print_extra(packer *self, FILE *file, memfile *src);
bool pack_huffman_print_encoded(packer *self, FILE *file, memfile *src, u32 decompressed_strlen);
bool pack_huffman_encode_string(packer *self, memfile *dst, const char *string);
bool pack_huffman_decode_string(packer *self, char *dst, size_t strlen, FILE *src);

#ifdef __cplusplus
}
#endif

#endif
