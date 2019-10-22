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

#include <jakson/mem/file.h>
#include <jakson/std/uintvar/stream.h>

bool memfile_open(struct carbon_memfile *file, memblock *block, access_mode_e mode)
{
        DEBUG_ERROR_IF_NULL(file)
        DEBUG_ERROR_IF_NULL(block)
        ZERO_MEMORY(file, sizeof(struct carbon_memfile))
        file->memblock = block;
        file->pos = 0;
        file->bit_mode = false;
        file->mode = mode;
        file->saved_pos_ptr = 0;
        error_init(&file->err);
        return true;
}

bool memfile_clone(struct carbon_memfile *dst, struct carbon_memfile *src)
{
        DEBUG_ERROR_IF_NULL(dst)
        DEBUG_ERROR_IF_NULL(src)
        memfile_open(dst, src->memblock, src->mode);
        memfile_seek(dst, memfile_tell(src));
        dst->bit_mode = src->bit_mode;
        dst->saved_pos_ptr = src->saved_pos_ptr;
        error_cpy(&dst->err, &src->err);
        memcpy(&dst->saved_pos, &src->saved_pos, ARRAY_LENGTH(src->saved_pos));
        return true;
}

bool memfile_seek(struct carbon_memfile *file, offset_t pos)
{
        DEBUG_ERROR_IF_NULL(file)
        offset_t file_size = 0;
        memblock_size(&file_size, file->memblock);
        if (UNLIKELY(pos >= file_size)) {
                if (file->mode == READ_WRITE) {
                        offset_t new_size = pos + 1;
                        memblock_resize(file->memblock, new_size);
                } else {
                        ERROR(&file->err, ERR_MEMSTATE)
                        return false;
                }
        }
        file->pos = pos;
        return true;
}

bool memfile_seek_from_here(struct carbon_memfile *file, signed_offset_t where)
{
        offset_t now = memfile_tell(file);
        offset_t then = now + where;
        return memfile_seek(file, then);
}

bool memfile_rewind(struct carbon_memfile *file)
{
        DEBUG_ERROR_IF_NULL(file)
        file->pos = 0;
        return true;
}

bool memfile_grow(struct carbon_memfile *file_in, size_t grow_by_bytes)
{
        DEBUG_ERROR_IF_NULL(file_in)
        if (LIKELY(grow_by_bytes > 0)) {
                offset_t block_size = 0;
                memblock_size(&block_size, file_in->memblock);
                memblock_resize(file_in->memblock, (block_size + grow_by_bytes));
        }
        return true;
}

bool memfile_get_offset(offset_t *pos, const struct carbon_memfile *file)
{
        DEBUG_ERROR_IF_NULL(pos)
        DEBUG_ERROR_IF_NULL(file)
        *pos = file->pos;
        return true;
}

size_t memfile_size(struct carbon_memfile *file)
{
        if (!file || !file->memblock) {
                return 0;
        } else {
                carbon_u64 size;
                memblock_size(&size, file->memblock);
                return size;
        }
}

bool memfile_cut(struct carbon_memfile *file, size_t how_many_bytes)
{
        DEBUG_ERROR_IF_NULL(file);
        offset_t block_size = 0;
        memblock_size(&block_size, file->memblock);

        if (how_many_bytes > 0 && block_size > how_many_bytes) {
                size_t new_block_size = block_size - how_many_bytes;
                memblock_resize(file->memblock, new_block_size);
                file->pos = JAK_MIN(file->pos, new_block_size);
                return true;
        } else {
                ERROR(&file->err, ERR_ILLEGALARG);
                return false;
        }
}

size_t memfile_remain_size(struct carbon_memfile *file)
{
        JAK_ASSERT(file->pos <= memfile_size(file));
        return memfile_size(file) - file->pos;
}

bool memfile_shrink(struct carbon_memfile *file)
{
        DEBUG_ERROR_IF_NULL(file);
        if (file->mode == READ_WRITE) {
                int status = memblock_shrink(file->memblock);
                carbon_u64 size;
                memblock_size(&size, file->memblock);
                JAK_ASSERT(size == file->pos);
                return status;
        } else {
                ERROR(&file->err, ERR_WRITEPROT)
                return false;
        }
}

const char *memfile_read(struct carbon_memfile *file, offset_t nbytes)
{
        const char *result = memfile_peek(file, nbytes);
        file->pos += nbytes;
        return result;
}

carbon_u8 memfile_read_byte(struct carbon_memfile *file)
{
        return *MEMFILE_READ_TYPE(file, carbon_u8);
}

carbon_u8 memfile_peek_byte(struct carbon_memfile *file)
{
        return *MEMFILE_PEEK(file, carbon_u8);
}

carbon_u64 memfile_read_u64(struct carbon_memfile *file)
{
        return *MEMFILE_READ_TYPE(file, carbon_u64);
}

carbon_i64 memfile_read_i64(struct carbon_memfile *file)
{
        return *MEMFILE_READ_TYPE(file, carbon_i64);
}

bool memfile_skip(struct carbon_memfile *file, signed_offset_t nbytes)
{
        offset_t required_size = file->pos + nbytes;
        file->pos += nbytes;
        offset_t file_size = 0;
        memblock_size(&file_size, file->memblock);

        if (UNLIKELY(required_size >= file_size)) {
                if (file->mode == READ_WRITE) {
                        memblock_resize(file->memblock, required_size * 1.7f);
                } else {
                        ERROR(&file->err, ERR_WRITEPROT);
                        return false;
                }
        }
        memfile_update_last_byte(file->memblock, file->pos);
        JAK_ASSERT(file->pos < memfile_size(file));
        return true;
}

const char *memfile_peek(struct carbon_memfile *file, offset_t nbytes)
{
        offset_t file_size = 0;
        memblock_size(&file_size, file->memblock);
        if (UNLIKELY(file->pos + nbytes > file_size)) {
                ERROR(&file->err, ERR_READOUTOFBOUNDS);
                return NULL;
        } else {
                const char *result = memblock_raw_data(file->memblock) + file->pos;
                return result;
        }
}

bool memfile_write_byte(struct carbon_memfile *file, carbon_u8 data)
{
        return memfile_write(file, &data, sizeof(carbon_u8));
}

bool memfile_write(struct carbon_memfile *file, const void *data, offset_t nbytes)
{
        DEBUG_ERROR_IF_NULL(file)
        DEBUG_ERROR_IF_NULL(data)
        if (file->mode == READ_WRITE) {
                if (LIKELY(nbytes != 0)) {
                        offset_t file_size = 0;
                        memblock_size(&file_size, file->memblock);
                        offset_t required_size = file->pos + nbytes;
                        if (UNLIKELY(required_size >= file_size)) {
                                memblock_resize(file->memblock, required_size * 1.7f);
                        }

                        if (UNLIKELY(!memblock_write(file->memblock, file->pos, data, nbytes))) {
                                return false;
                        }
                        file->pos += nbytes;
                }
                return true;
        } else {
                ERROR(&file->err, ERR_WRITEPROT);
                return false;
        }
}

bool memfile_write_zero(struct carbon_memfile *file, size_t how_many)
{
        DEBUG_ERROR_IF_NULL(file);
        DEBUG_ERROR_IF_NULL(how_many);
        char empty = 0;
        while (how_many--) {
                memfile_write(file, &empty, sizeof(char));
        }
        return true;
}

bool memfile_begin_bit_mode(struct carbon_memfile *file)
{
        DEBUG_ERROR_IF_NULL(file);
        if (file->mode == READ_WRITE) {
                file->bit_mode = true;
                file->current_read_bit = file->current_write_bit = file->bytes_completed = 0;
                file->bytes_completed = 0;
                offset_t offset;
                char empty = '\0';
                memfile_get_offset(&offset, file);
                memfile_write(file, &empty, sizeof(char));
                memfile_seek(file, offset);
        } else {
                ERROR(&file->err, ERR_WRITEPROT);
                return false;
        }

        return true;
}

bool memfile_write_bit(struct carbon_memfile *file, bool flag)
{
        DEBUG_ERROR_IF_NULL(file);
        file->current_read_bit = 0;

        if (file->bit_mode) {
                if (file->current_write_bit < 8) {
                        offset_t offset;
                        memfile_get_offset(&offset, file);
                        char byte = *memfile_read(file, sizeof(char));
                        char mask = 1 << file->current_write_bit;
                        if (flag) {
                                SET_BITS(byte, mask);
                        } else {
                                UNSET_BITS(byte, mask);
                        }
                        memfile_seek(file, offset);
                        memfile_write(file, &byte, sizeof(char));
                        memfile_seek(file, offset);
                        file->current_write_bit++;
                } else {
                        file->current_write_bit = 0;
                        file->bytes_completed++;
                        char empty = '\0';
                        offset_t off;
                        memfile_skip(file, 1);
                        memfile_get_offset(&off, file);
                        memfile_write(file, &empty, sizeof(char));
                        memfile_seek(file, off);

                        return memfile_write_bit(file, flag);
                }
                return true;
        } else {
                ERROR(&file->err, ERR_NOBITMODE);
                return false;
        }
}

bool memfile_read_bit(struct carbon_memfile *file)
{
        if (!file) {
                return false;
        }

        file->current_write_bit = 0;

        if (file->bit_mode) {
                if (file->current_read_bit < 8) {
                        offset_t offset;
                        memfile_get_offset(&offset, file);

                        char mask = 1 << file->current_read_bit;
                        char byte = *memfile_read(file, sizeof(char));
                        memfile_seek(file, offset);
                        bool result = ((byte & mask) >> file->current_read_bit) == true;
                        file->current_read_bit++;
                        return result;
                } else {
                        file->current_read_bit = 0;
                        memfile_skip(file, sizeof(char));
                        return memfile_read_bit(file);
                }
        } else {
                ERROR(&file->err, ERR_NOBITMODE);
                return false;
        }
}

offset_t memfile_save_position(struct carbon_memfile *file)
{
        DEBUG_ERROR_IF_NULL(file);
        offset_t pos = memfile_tell(file);
        if (LIKELY(file->saved_pos_ptr < (carbon_i8) (ARRAY_LENGTH(file->saved_pos)))) {
                file->saved_pos[file->saved_pos_ptr++] = pos;
        } else {
                ERROR(&file->err, ERR_STACK_OVERFLOW)
        }
        return pos;
}

bool memfile_restore_position(struct carbon_memfile *file)
{
        DEBUG_ERROR_IF_NULL(file);
        if (LIKELY(file->saved_pos_ptr >= 0)) {
                offset_t pos = file->saved_pos[--file->saved_pos_ptr];
                memfile_seek(file, pos);
                return true;
        } else {
                ERROR(&file->err, ERR_STACK_UNDERFLOW)
                return false;
        }
}

signed_offset_t memfile_ensure_space(struct carbon_memfile *memfile, carbon_u64 nbytes)
{
        DEBUG_ERROR_IF_NULL(memfile)

        DECLARE_AND_INIT(offset_t, block_size);

        memblock_size(&block_size, memfile->memblock);
        JAK_ASSERT(memfile->pos < block_size);
        size_t diff = block_size - memfile->pos;
        if (diff < nbytes) {
                memfile_grow(memfile, nbytes - diff);
        }

        memfile_save_position(memfile);
        offset_t current_off = memfile_tell(memfile);
        signed_offset_t shift = 0;
        for (carbon_u32 i = 0; i < nbytes; i++) {
                char c = *memfile_read(memfile, 1);
                if (UNLIKELY(c != 0)) {
                        /** not enough space; enlarge container */
                        memfile_seek(memfile, current_off);
                        memfile_inplace_insert(memfile, nbytes - i);
                        shift += nbytes - i;
                        break;
                }
        }
        memfile_restore_position(memfile);

        return shift;
}

carbon_u64 memfile_read_uintvar_stream(carbon_u8 *nbytes, struct carbon_memfile *memfile)
{
        carbon_u8 nbytes_read;
        carbon_u64 result = uintvar_stream_read(&nbytes_read, (uintvar_stream_t) memfile_peek(memfile, sizeof(char)));
        memfile_skip(memfile, nbytes_read);
        OPTIONAL_SET(nbytes, nbytes_read);
        return result;
}

bool memfile_skip_uintvar_stream(struct carbon_memfile *memfile)
{
        DEBUG_ERROR_IF_NULL(memfile)
        memfile_read_uintvar_stream(NULL, memfile);
        return true;
}

carbon_u64 memfile_peek_uintvar_stream(carbon_u8 *nbytes, struct carbon_memfile *memfile)
{
        memfile_save_position(memfile);
        carbon_u64 result = memfile_read_uintvar_stream(nbytes, memfile);
        memfile_restore_position(memfile);
        return result;
}

carbon_u64 memfile_write_uintvar_stream(carbon_u64 *nbytes_moved, struct carbon_memfile *memfile, carbon_u64 value)
{
        carbon_u8 required_blocks = UINTVAR_STREAM_REQUIRED_BLOCKS(value);
        signed_offset_t shift = memfile_ensure_space(memfile, required_blocks);
        uintvar_stream_t dst = (uintvar_stream_t) memfile_peek(memfile, sizeof(char));
        uintvar_stream_write(dst, value);
        memfile_skip(memfile, required_blocks);
        OPTIONAL_SET(nbytes_moved, shift);
        return required_blocks;
}

signed_offset_t memfile_update_uintvar_stream(struct carbon_memfile *memfile, carbon_u64 value)
{
        DEBUG_ERROR_IF_NULL(memfile);

        carbon_u8 bytes_used_now, bytes_used_then;

        memfile_peek_uintvar_stream(&bytes_used_now, memfile);
        bytes_used_then = UINTVAR_STREAM_REQUIRED_BLOCKS(value);

        if (bytes_used_now < bytes_used_then) {
                carbon_u8 inc = bytes_used_then - bytes_used_now;
                memfile_inplace_insert(memfile, inc);
        } else if (bytes_used_now > bytes_used_then) {
                carbon_u8 dec = bytes_used_now - bytes_used_then;
                memfile_inplace_remove(memfile, dec);
        }

        uintvar_stream_t dst = (uintvar_stream_t) memfile_peek(memfile, sizeof(char));
        carbon_u8 required_blocks = uintvar_stream_write(dst, value);
        memfile_skip(memfile, required_blocks);

        return bytes_used_then - bytes_used_now;
}

bool memfile_seek_to_start(struct carbon_memfile *file)
{
        return memfile_seek(file, 0);
}

bool memfile_seek_to_end(struct carbon_memfile *file)
{
        DEBUG_ERROR_IF_NULL(file)
        size_t size = memblock_last_used_byte(file->memblock);
        return memfile_seek(file, size);
}

bool memfile_inplace_insert(struct carbon_memfile *file, size_t nbytes)
{
        DEBUG_ERROR_IF_NULL(file);
        return memblock_move_right(file->memblock, file->pos, nbytes);
}

bool memfile_inplace_remove(struct carbon_memfile *file, size_t nbytes_from_here)
{
        DEBUG_ERROR_IF_NULL(file);
        return memblock_move_left(file->memblock, file->pos, nbytes_from_here);
}

bool memfile_end_bit_mode(size_t *num_bytes_written, struct carbon_memfile *file)
{
        DEBUG_ERROR_IF_NULL(file);
        file->bit_mode = false;
        if (file->current_write_bit <= 8) {
                memfile_skip(file, 1);
                file->bytes_completed++;
        }
        OPTIONAL_SET(num_bytes_written, file->bytes_completed);
        file->current_write_bit = file->bytes_completed = 0;
        return true;
}

void *memfile_current_pos(struct carbon_memfile *file, offset_t nbytes)
{
        if (file && nbytes > 0) {
                offset_t file_size = 0;
                memblock_size(&file_size, file->memblock);
                offset_t required_size = file->pos + nbytes;
                if (UNLIKELY(file->pos + nbytes >= file_size)) {
                        if (file->mode == READ_WRITE) {
                                memblock_resize(file->memblock, required_size * 1.7f);
                        } else {
                                ERROR(&file->err, ERR_WRITEPROT);
                                return NULL;
                        }
                }
                void *data = (void *) memfile_peek(file, nbytes);
                return data;
        } else {
                return NULL;
        }
}

bool memfile_hexdump(string_buffer *sb, struct carbon_memfile *file)
{
        DEBUG_ERROR_IF_NULL(sb);
        DEBUG_ERROR_IF_NULL(file);
        DECLARE_AND_INIT(offset_t, block_size)
        memblock_size(&block_size, file->memblock);
        hexdump(sb, memblock_raw_data(file->memblock), block_size);
        return true;
}

bool memfile_hexdump_printf(FILE *file, struct carbon_memfile *memfile)
{
        DEBUG_ERROR_IF_NULL(file)
        DEBUG_ERROR_IF_NULL(memfile)
        DECLARE_AND_INIT(offset_t, block_size)
        memblock_size(&block_size, memfile->memblock);
        hexdump_print(file, memblock_raw_data(memfile->memblock), block_size);
        return true;
}

bool memfile_hexdump_print(struct carbon_memfile *memfile)
{
        return memfile_hexdump_printf(stdout, memfile);
}