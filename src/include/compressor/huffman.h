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

#ifndef CARBON_COMPRESSOR_HUFFMAN_H
#define CARBON_COMPRESSOR_HUFFMAN_H

#include "shared/common.h"
#include "stdx/vec.h"
#include "mem/memfile.h"

CARBON_BEGIN_DECL

typedef struct carbon_compressor carbon_compressor_t; /* forwarded from 'types-compressor.h' */

CARBON_EXPORT(bool)
carbon_compressor_huffman_init(carbon_compressor_t *self);

CARBON_EXPORT(bool)
carbon_compressor_huffman_cpy(const carbon_compressor_t *self, carbon_compressor_t *dst);

CARBON_EXPORT(bool)
carbon_compressor_huffman_drop(carbon_compressor_t *self);

CARBON_EXPORT(bool)
carbon_compressor_huffman_write_extra(carbon_compressor_t *self, memfile_t *dst,
                                      const vec_t ofType (const char *) *strings);

CARBON_EXPORT(bool)
carbon_compressor_huffman_read_extra(carbon_compressor_t *self, FILE *src, size_t nbytes);

CARBON_EXPORT(bool)
carbon_compressor_huffman_print_extra(carbon_compressor_t *self, FILE *file, memfile_t *src);

CARBON_EXPORT(bool)
carbon_compressor_huffman_print_encoded(carbon_compressor_t *self, FILE *file, memfile_t *src,
                                        u32 decompressed_strlen);

CARBON_EXPORT(bool)
carbon_compressor_huffman_encode_string(carbon_compressor_t *self, memfile_t *dst, struct err *err,
                                        const char *string);

CARBON_EXPORT(bool)
carbon_compressor_huffman_decode_string(carbon_compressor_t *self, char *dst, size_t strlen, FILE *src);


CARBON_END_DECL

#endif