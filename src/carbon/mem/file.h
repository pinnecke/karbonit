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

#ifndef MEMFILE_H
#define MEMFILE_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <carbon/stdinc.h>
#include <carbon/std/string.h>
#include <carbon/hexdump.h>
#include <carbon/mem/block.h>

BEGIN_DECL

struct carbon_memfile {
        memblock *memblock;
        offset_t pos;
        offset_t saved_pos[10];
        carbon_i8 saved_pos_ptr;
        bool bit_mode;
        size_t current_read_bit, current_write_bit, bytes_completed;
        access_mode_e mode;
        err err;
};

#define MEMFILE_PEEK(file, type)                                                                                   \
({                                                                                                                     \
    JAK_ASSERT (memfile_remain_size(file) >= sizeof(type));                                                                \
    (type*) memfile_peek(file, sizeof(type));                                                                          \
})

#define MEMFILE_READ_TYPE(file, type)                                                                              \
({                                                                                                                     \
    JAK_ASSERT (memfile_remain_size(file) >= sizeof(type));                                                                \
    (type *) memfile_read(file, sizeof(type));                                                                          \
})

#define MEMFILE_READ_TYPE_LIST(file, type, how_many)                                                               \
    (const type *) MEMFILE_READ(file, how_many * sizeof(type))

#define MEMFILE_READ(file, nbytes)                                                                                 \
({                                                                                                                     \
    JAK_ASSERT (memfile_remain_size(file) >= nbytes);                                                                      \
    memfile_read(file, nbytes);                                                                                        \
})

#define memfile_tell(file)                                                                                             \
({                                                                                                                     \
    offset_t offset = 0;                                                                                               \
    memfile_get_offset(&offset, file);                                                                                 \
    offset;                                                                                                            \
})

bool memfile_open(struct carbon_memfile *file, memblock *block, access_mode_e mode);
bool memfile_clone(struct carbon_memfile *dst, struct carbon_memfile *src);

bool memfile_seek(struct carbon_memfile *file, offset_t pos);
bool memfile_seek_from_here(struct carbon_memfile *file, signed_offset_t where);
bool memfile_rewind(struct carbon_memfile *file);
bool memfile_grow(struct carbon_memfile *file_in, size_t grow_by_bytes);
bool memfile_get_offset(offset_t *pos, const struct carbon_memfile *file);
size_t memfile_size(struct carbon_memfile *file);
bool memfile_cut(struct carbon_memfile *file, size_t how_many_bytes);
size_t memfile_remain_size(struct carbon_memfile *file);
bool memfile_shrink(struct carbon_memfile *file);
const char *memfile_read(struct carbon_memfile *file, offset_t nbytes);
carbon_u8 memfile_read_byte(struct carbon_memfile *file);
carbon_u8 memfile_peek_byte(struct carbon_memfile *file);
carbon_u64 memfile_read_u64(struct carbon_memfile *file);
carbon_i64 memfile_read_i64(struct carbon_memfile *file);
bool memfile_skip(struct carbon_memfile *file, signed_offset_t nbytes);
#define MEMFILE_SKIP_BYTE(file) memfile_skip(file, sizeof(carbon_u8))
const char *memfile_peek(struct carbon_memfile *file, offset_t nbytes);
bool memfile_write_byte(struct carbon_memfile *file, carbon_u8 data);
bool memfile_write(struct carbon_memfile *file, const void *data, offset_t nbytes);
bool memfile_write_zero(struct carbon_memfile *file, size_t how_many);
bool memfile_begin_bit_mode(struct carbon_memfile *file);
bool memfile_write_bit(struct carbon_memfile *file, bool flag);
bool memfile_read_bit(struct carbon_memfile *file);
offset_t memfile_save_position(struct carbon_memfile *file);
bool memfile_restore_position(struct carbon_memfile *file);
signed_offset_t memfile_ensure_space(struct carbon_memfile *memfile, carbon_u64 nbytes);
carbon_u64 memfile_read_uintvar_stream(carbon_u8 *nbytes, struct carbon_memfile *memfile);
bool memfile_skip_uintvar_stream(struct carbon_memfile *memfile);
carbon_u64 memfile_peek_uintvar_stream(carbon_u8 *nbytes, struct carbon_memfile *memfile);
carbon_u64 memfile_write_uintvar_stream(carbon_u64 *nbytes_moved, struct carbon_memfile *memfile, carbon_u64 value);
signed_offset_t memfile_update_uintvar_stream(struct carbon_memfile *memfile, carbon_u64 value);
bool memfile_seek_to_start(struct carbon_memfile *file);
bool memfile_seek_to_end(struct carbon_memfile *file);

/**
 * Moves the contents of the underlying memory block <code>nbytes</code> towards the end of the file.
 * The offset in the memory block from where this move is done is the current position stored in this file.
 * In case of not enough space, the underlying memory block is resized.
 */
bool memfile_inplace_insert(struct carbon_memfile *file, size_t nbytes);
bool memfile_inplace_remove(struct carbon_memfile *file, size_t nbytes_from_here);
bool memfile_end_bit_mode(size_t *num_bytes_written, struct carbon_memfile *file);
void *memfile_current_pos(struct carbon_memfile *file, offset_t nbytes);

bool memfile_hexdump(string_buffer *sb, struct carbon_memfile *file);
bool memfile_hexdump_printf(FILE *file, struct carbon_memfile *memfile);
bool memfile_hexdump_print(struct carbon_memfile *memfile);

END_DECL

#endif