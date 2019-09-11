/**
 * A variable-length unsigned integer type that encodes the number of used bytes by a preceding marker byte
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

#ifndef JAK_UINTVAR_MARKER_H
#define JAK_UINTVAR_MARKER_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jak_stdinc.h>
#include <jak_types.h>

/**
 * This type is for variable-length unsigned integer types.
 *
 * The encoding uses a dedicated byte (called marker) to identify the number of subsequent bytes that holding the
 * actual value: if the first byte read is...
 *      - ... 'c', then the next byte contains an unsigned integer value of 8bit.
 *      - ... 'd', then the next 2 bytes contain an unsigned integer value of 16bit.
 *      - ... 'i', then the next 4 bytes contain an unsigned integer value of 32bit.
 *      - ... 'l', then the next 8 bytes contain an unsigned integer value of 64bit.
 *
 * This implementation supports variable-length encoding to the maximum of unsigned integers of
 * 64bit (fixed-sized) requiring constant 1 byte more than the standard C type.
 *
 * Note that size requirements for this kind of variable-length encoding is (relatively) huge;
 * the encoding requires as least 12.5% additional storage (to encode 64bit integers) and at most
 * 100.0% (!) additional storage (to encode 8bit integers). The benefit of marker-based variable-length encoding is that
 * read-/write performance is superior to byte-stream based variable-length encoding (see <code>uintvar_stream</code>),
 * and that size requirements payoff for values larger than 65536. Faster read/write performance compared to byte-stream
 * based variable-length encoding comes by the fact that after determination of actual number of bytes to reads
 * (i.e., the marker), there is no interpretation overhead to read the actual value while in byte-stream based encoding
 * each subsequent byte must be inspect (on whether it is the last byte in the stream) before reading its contained
 * value (after some byte shift operations).
 *
 * Rule of thumb:
 *      - if fixed-length types are a good choice, and...
 *          - ... if speed matters, use fast-types of the C library (e.g., <code>uint_fast32_t</code>)
 *          - ... if space matters, use fix-types of the C library (e.g., <code>uint32_t</code>)
 *      - if variable-length types are a good choice, and...
 *          - ... if space shall be minimized in exchange of read/write performance, use <code>jak_uintvar_stream_t</code>
 *          - ... if read/write performance shall be maximized in exchange of space, use <code>jak_uintvar_marker_t</code>
 */

JAK_BEGIN_DECL

// ---------------------------------------------------------------------------------------------------------------------
//  markers
// ---------------------------------------------------------------------------------------------------------------------

#define UINT_VAR_MARKER_8 'c'
#define UINT_VAR_MARKER_16 'd'
#define UINT_VAR_MARKER_32 'i'
#define UINT_VAR_MARKER_64 'l'

// ---------------------------------------------------------------------------------------------------------------------
//  types
// ---------------------------------------------------------------------------------------------------------------------

typedef void *jak_uintvar_marker_t;

typedef enum jak_uintvar_marker_marker_type {
        JAK_UINTVAR_8 = 0,
        JAK_UINTVAR_16 = 1,
        JAK_UINTVAR_32 = 2,
        JAK_UINTVAR_64 = 3
} jak_uintvar_marker_marker_type_e;

static const jak_marker_t jak_uintvar_marker_strs[] = {
        UINT_VAR_MARKER_8, UINT_VAR_MARKER_16, UINT_VAR_MARKER_32, UINT_VAR_MARKER_64
};

static const struct {
        jak_marker_t marker;
        jak_u8 size;
} jak_global_uintvar_marker_reg[] = {
        { UINT_VAR_MARKER_8, sizeof(jak_u8) },
        { UINT_VAR_MARKER_16, sizeof(jak_u16) },
        { UINT_VAR_MARKER_32, sizeof(jak_u32) },
        { UINT_VAR_MARKER_64, sizeof(jak_u64) }
};

static const int jak_global_uintvar_marker_nreg = JAK_ARRAY_LENGTH(jak_global_uintvar_marker_reg);

// ---------------------------------------------------------------------------------------------------------------------
//  read/write
// ---------------------------------------------------------------------------------------------------------------------

bool jak_uintvar_marker_write(jak_uintvar_marker_t dst, jak_u64 value);
jak_u64 jak_uintvar_marker_read(jak_u8 *nbytes_read, jak_uintvar_marker_t src);

jak_u8 jak_uintvar_marker_write_marker_for(jak_uintvar_marker_marker_type_e *marker_out, void *dst, jak_u64 value);
void jak_uintvar_marker_write_marker(void *dst, jak_uintvar_marker_marker_type_e type);

jak_u8 jak_uintvar_marker_write_value_only(void *dst, jak_uintvar_marker_marker_type_e type, jak_u64 value);

// ---------------------------------------------------------------------------------------------------------------------
//  information
// ---------------------------------------------------------------------------------------------------------------------

#define JAK_UINTVAR_MARKER_BYTES_NEEDED_FOR_MARKER()      sizeof(jak_marker_t)

jak_uintvar_marker_marker_type_e jak_uintvar_marker_type_for(jak_u64 value);
jak_uintvar_marker_marker_type_e jak_uintvar_marker_peek_marker(jak_uintvar_marker_t data);
jak_u8 jak_uintvar_marker_sizeof(jak_uintvar_marker_t varuint);
jak_u8 jak_uintvar_marker_bytes_needed_value(jak_u64 value);
jak_u8 jak_uintvar_marker_bytes_needed_for_value_by_type(jak_uintvar_marker_marker_type_e type);
jak_u8 jak_uintvar_marker_bytes_needed_complete(jak_u64 value);
char jak_uintvar_marker_type_str(jak_uintvar_marker_marker_type_e marker);
int jak_uintvar_marker_compare(jak_uintvar_marker_marker_type_e lhs, jak_uintvar_marker_marker_type_e rhs);

JAK_END_DECL

#endif
