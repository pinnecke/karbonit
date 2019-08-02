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

#ifndef ARK_MEMFILE_H
#define ARK_MEMFILE_H

#include <ark-js/shared/common.h>
#include <ark-js/shared/stdx/string_builder.h>
#include <ark-js/shared/utils/hexdump.h>
#include <ark-js/shared/mem/block.h>

ARK_BEGIN_DECL

struct memfile {
        struct memblock *memblock;
        offset_t pos;
        offset_t saved_pos[10];
        i8 saved_pos_ptr;
        bool bit_mode;
        size_t current_read_bit, current_write_bit, bytes_completed;
        enum access_mode mode;
        struct err err;
};

#define ARK_MEMFILE_PEEK(file, type)                                                                                   \
({                                                                                                                     \
    assert (memfile_remain_size(file) >= sizeof(type));                                                                \
    (type*) memfile_peek(file, sizeof(type));                                                                          \
})

#define ARK_MEMFILE_READ_TYPE(file, type)                                                                              \
({                                                                                                                     \
    assert (memfile_remain_size(file) >= sizeof(type));                                                                \
    (type*) memfile_read(file, sizeof(type));                                                                          \
})

#define ARK_MEMFILE_READ_TYPE_LIST(file, type, how_many)                                                               \
    (const type *) ARK_MEMFILE_READ(file, how_many * sizeof(type))

#define ARK_MEMFILE_READ(file, nbytes)                                                                                 \
({                                                                                                                     \
    assert (memfile_remain_size(file) >= nbytes);                                                                      \
    memfile_read(file, nbytes);                                                                                        \
})

#define memfile_tell(file)                                                                                             \
({                                                                                                                     \
    offset_t offset;                                                                                                   \
    memfile_get_offset(&offset, file);                                                                                 \
    offset;                                                                                                            \
})

ARK_EXPORT(bool) memfile_open(struct memfile *file, struct memblock *block, enum access_mode mode);

ARK_EXPORT(bool) memfile_dup(struct memfile *dst, struct memfile *src);

ARK_EXPORT(bool) memfile_seek(struct memfile *file, offset_t pos);

ARK_EXPORT(bool) memfile_rewind(struct memfile *file);

ARK_EXPORT(bool) memfile_grow(struct memfile *file_in, size_t grow_by_bytes, bool zero_out);

ARK_EXPORT(bool) memfile_get_offset(offset_t *pos, const struct memfile *file);

ARK_EXPORT(size_t) memfile_size(struct memfile *file);

ARK_EXPORT(bool) memfile_cut(struct memfile *file, size_t how_many_bytes);

ARK_EXPORT(size_t) memfile_remain_size(struct memfile *file);

ARK_EXPORT(bool) memfile_shrink(struct memfile *file);

ARK_EXPORT(const char *)memfile_read(struct memfile *file, offset_t nbytes);

ARK_EXPORT(bool) memfile_skip(struct memfile *file, signed_offset_t nbytes);

ARK_EXPORT(const char *)memfile_peek(struct memfile *file, offset_t nbytes);

ARK_EXPORT(bool) memfile_write(struct memfile *file, const void *data, offset_t nbytes);

ARK_EXPORT(bool) memfile_write_zero(struct memfile *file, size_t how_many);

ARK_EXPORT(bool) memfile_begin_bit_mode(struct memfile *file);

ARK_EXPORT(bool) memfile_write_bit(struct memfile *file, bool flag);

ARK_EXPORT(bool) memfile_read_bit(struct memfile *file);

ARK_EXPORT(offset_t) memfile_save_position(struct memfile *file);

ARK_EXPORT(bool) memfile_restore_position(struct memfile *file);

ARK_EXPORT(bool) memfile_ensure_space(struct memfile *memfile, u64 nbytes);

ARK_EXPORT(u64) memfile_read_varuint(u8 *nbytes, struct memfile *memfile);

ARK_EXPORT(bool) memfile_skip_varuint(struct memfile *memfile);

ARK_EXPORT(u64) memfile_peek_varuint(u8 *nbytes, struct memfile *memfile);

ARK_EXPORT(u64) memfile_write_varuint(struct memfile *memfile, u64 value);

ARK_EXPORT(bool) memfile_update_varuint(struct memfile *memfile, u64 value);

ARK_EXPORT(bool) memfile_seek_to_end(struct memfile *file);

/**
 * Moves the contents of the underlying memory block <code>nbytes</code> towards the end of the file.
 * The offset in the memory block from where this move is done is the current position stored in this file.
 * In case of not enough space, the underlying memory block is resized.
 */
ARK_EXPORT(bool) memfile_inplace_insert(struct memfile *file, size_t nbytes);

ARK_EXPORT(bool) memfile_inplace_remove(struct memfile *file, size_t nbytes_from_here);

ARK_EXPORT(bool) memfile_end_bit_mode(size_t *num_bytes_written, struct memfile *file);

ARK_EXPORT(void *) memfile_current_pos(struct memfile *file, offset_t nbytes);

ARK_EXPORT(bool) memfile_hexdump(struct string_builder *sb, struct memfile *file);

ARK_EXPORT(bool) memfile_hexdump_printf(FILE *file, struct memfile *memfile);

ARK_EXPORT(bool) memfile_hexdump_print(struct memfile *memfile);

ARK_END_DECL

#endif