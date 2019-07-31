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

#ifndef ARK_COMPRESSOR_HUFFMAN_H
#define ARK_COMPRESSOR_HUFFMAN_H

#include <ark-js/shared/common.h>
#include <ark-js/shared/stdx/vec.h>
#include <ark-js/shared/mem/file.h>

ARK_BEGIN_DECL

ARK_EXPORT(bool) pack_huffman_init(struct packer *self);

ARK_EXPORT(bool) pack_coding_huffman_cpy(const struct packer *self, struct packer *dst);

ARK_EXPORT(bool) pack_coding_huffman_drop(struct packer *self);

ARK_EXPORT(bool) pack_huffman_write_extra(struct packer *self, struct memfile *dst,
        const struct vector ofType (const char *) *strings);

ARK_EXPORT(bool) pack_huffman_read_extra(struct packer *self, FILE *src, size_t nbytes);

ARK_EXPORT(bool) pack_huffman_print_extra(struct packer *self, FILE *file, struct memfile *src);

ARK_EXPORT(bool) pack_huffman_print_encoded(struct packer *self, FILE *file, struct memfile *src,
        u32 decompressed_strlen);

ARK_EXPORT(bool) pack_huffman_encode_string(struct packer *self, struct memfile *dst, struct err *err,
        const char *string);

ARK_EXPORT(bool) pack_huffman_decode_string(struct packer *self, char *dst, size_t strlen, FILE *src);

ARK_END_DECL

#endif
