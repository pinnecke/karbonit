/*
 * memfile - memory file abstraction for i/o on memory blocks
 *
 * Copyright 2018 Marcus Pinnecke
 */

#ifndef HAD_MEMFILE_H
#define HAD_MEMFILE_H

#include <jakson/stdinc.h>
#include <jakson/std/string.h>
#include <jakson/hexdump.h>
#include <jakson/mem/memblock.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct memfile {
        memblock *memblock;
        offset_t pos;
        offset_t saved_pos[10];
        i8 saved_pos_ptr;
        bool bit_mode;
        size_t current_read_bit, current_write_bit, bytes_completed;
        access_mode_e mode;
} memfile;

#define MEMFILE_PEEK_TYPE(file, type)                                                                                  \
({                                                                                                                     \
        assert (MEMFILE_REMAIN_SIZE((file)) >= sizeof(type));                                                          \
        type* ret = (type*) MEMFILE_PEEK((file), sizeof(type));                                                        \
        ret;                                                                                                           \
})

#define MEMFILE_READ_TYPE(file, type)                                                                                  \
({                                                                                                                     \
        assert (MEMFILE_REMAIN_SIZE(file) >= sizeof(type));                                                            \
        (type *) MEMFILE_READ(file, sizeof(type));                                                                     \
})

#define MEMFILE_READ_TYPE_LIST(file, type, how_many)                                                                   \
     (const type *) MEMFILE_READ(file, how_many * sizeof(type))

#define MEMFILE_TELL(file)                                                                                             \
({                                                                                                                     \
        offset_t __offset = 0;                                                                                         \
        MEMFILE_GET_OFFSET(&__offset, (file));                                                                         \
        __offset;                                                                                                      \
})

#define MEMFILE_SKIP_BYTE(file)                                                                                        \
        MEMFILE_SKIP(file, sizeof(u8))

#define MEMFILE_WRITE(file, data, nbytes)																		       \
({		                                                                                                               \
        bool memfile_write_status = false;																                           \
        if ((file)->mode == READ_WRITE) {																		       \
                if (likely(nbytes != 0)) {																		       \
                        offset_t file_size = 0;																		   \
                        MEMBLOCK_SIZE(&file_size, (file)->memblock);												   \
                        offset_t required_size = (file)->pos + nbytes;												   \
                        if (unlikely(required_size >= file_size)) {													   \
                                MEMBLOCK_RESIZE((file)->memblock, required_size * 1.7f);							   \
                        }																		                       \
																		                                               \
                        if (unlikely(!MEMBLOCK_WRITE((file)->memblock, (file)->pos, ((char *)data), nbytes))) {		   \
                                memfile_write_status = false;																		   \
                        } else {	 																	               \
                                (file)->pos += nbytes;																   \
                        }                                                                                              \
                }																		                               \
                memfile_write_status = true;																		                   \
        } else {																		                               \
                error(ERR_WRITEPROT, NULL);																		       \
                memfile_write_status = false;																		                   \
        }																		                                       \
        memfile_write_status;                                                                                                        \
})

#define MEMFILE_SAVE_POSITION(file)										                                               \
({										                                                                               \
        offset_t pos = MEMFILE_TELL((file));										                                   \
        if (likely((file)->saved_pos_ptr < (i8) (ARRAY_LENGTH((file)->saved_pos)))) {								   \
                (file)->saved_pos[(file)->saved_pos_ptr++] = pos;										               \
        } else {										                                                               \
                error(ERR_STACK_OVERFLOW, NULL);										                               \
                pos = 0;										                                                       \
        }										                                                                       \
        pos;										                                                                   \
})

#define MEMFILE_RESTORE_POSITION(file)												                                   \
({												                                                                       \
	bool memfile_restore_position_status;												                                                   \
        if (likely((file)->saved_pos_ptr >= 0)) {												                       \
                offset_t pos = (file)->saved_pos[--(file)->saved_pos_ptr];											   \
                MEMFILE_SEEK((file), pos);												                               \
                memfile_restore_position_status = true;												                                           \
        } else {												                                                       \
                memfile_restore_position_status = error(ERR_STACK_UNDERFLOW, NULL);												               \
        }												                                                               \
        memfile_restore_position_status;												                                                           \
})

#define MEMFILE_ENSURE_SPACE(file, nbytes)											                                   \
({											                                                                           \
        DECLARE_AND_INIT(offset_t, block_size);											                               \
        MEMBLOCK_SIZE(&block_size, (file)->memblock);											                       \
        assert((file)->pos < block_size);											                                   \
        size_t diff = block_size - (file)->pos;											                               \
        if (diff < (nbytes)) {											                                               \
                MEMFILE_GROW((file), (nbytes) - diff);											                       \
        }											                                                                   \
        MEMFILE_SAVE_POSITION((file));											                                       \
        offset_t current_off = MEMFILE_TELL((file));											                       \
        signed_offset_t shift = 0;											                                           \
        for (u32 i = 0; i < (nbytes); i++) {											                               \
                char c = *MEMFILE_READ((file), 1);											                           \
                if (unlikely(c != 0)) {											                                       \
                        MEMFILE_SEEK((file), current_off);											                   \
                        MEMFILE_INPLACE_INSERT((file), (nbytes) - i);											       \
                        shift += (nbytes) - i;											                               \
                        break;											                                               \
                }											                                                           \
        }											                                                                   \
        MEMFILE_RESTORE_POSITION((file));											                                   \
        shift;											                                                               \
})

#define MEMFILE_READ_UINTVAR_STREAM(_nbytes_, _file_)													                   \
({													                                                                   \
        u64 ___result; UNUSED(___result);													                                               \
        {                                       \
                u8 __nbytes_read_ = 0;             \
                ___result = UINTVAR_STREAM_READ(&__nbytes_read_, (uintvar_stream_t) MEMFILE_PEEK((_file_), sizeof(char)));                   \
                MEMFILE_SKIP((_file_), __nbytes_read_);                                                                                                                                   \
                if ((_nbytes_) != NULL) { u8 *assign_nbytes = (_nbytes_); *assign_nbytes = __nbytes_read_; }                                                   \
        }                               \
        ___result;													                                                       \
})

#define MEMFILE_SKIP_UINTVAR_STREAM(file)						                                                       \
        MEMFILE_READ_UINTVAR_STREAM(NULL, (file));

#define MEMFILE_PEEK_UINTVAR_STREAM(nbytes, file)									                                   \
({									                                                                                   \
        MEMFILE_SAVE_POSITION((file));									                                               \
        u64 result = MEMFILE_READ_UINTVAR_STREAM((nbytes), (file));									                   \
        MEMFILE_RESTORE_POSITION((file));									                                           \
        result;									                                                                       \
})

#define MEMFILE_WRITE_UINTVAR_STREAM(nbytes_moved, file, value)												           \
({												                                                                       \
        u8 required_blocks = UINTVAR_STREAM_REQUIRED_BLOCKS((value));												   \
        signed_offset_t shift = MEMFILE_ENSURE_SPACE((file), required_blocks);										   \
        uintvar_stream_t dst = (uintvar_stream_t) MEMFILE_PEEK((file), sizeof(char));								   \
        uintvar_stream_write(dst, (value));												                               \
        MEMFILE_SKIP((file), required_blocks);												                           \
        if ((nbytes_moved) != NULL) { signed_offset_t *assign = nbytes_moved; *assign = shift; }					   \
        required_blocks;												                                               \
})

#define MEMFILE_UPDATE_UINTVAR_STREAM(file, value)															           \
({															                                                           \
        u8 bytes_used_now, bytes_used_then;															                   \
        MEMFILE_PEEK_UINTVAR_STREAM(&bytes_used_now, (file));														   \
        bytes_used_then = UINTVAR_STREAM_REQUIRED_BLOCKS((value));												   	   \
        if (bytes_used_now < bytes_used_then) {															               \
                u8 inc = bytes_used_then - bytes_used_now;															   \
                MEMFILE_INPLACE_INSERT((file), inc);															       \
        } else if (bytes_used_now > bytes_used_then) {															       \
                u8 dec = bytes_used_now - bytes_used_then;															   \
                MEMFILE_INPLACE_REMOVE((file), dec);															       \
        }															                                                   \
        uintvar_stream_t dst = (uintvar_stream_t) MEMFILE_PEEK((file), sizeof(char));								   \
        u8 required_blocks = uintvar_stream_write(dst, (value));													   \
        MEMFILE_SKIP((file), required_blocks);															               \
        (bytes_used_then - bytes_used_now);															                   \
})


#define MEMFILE_OPEN(file, block, access_mode_e_mode)								                                   \
{								                                                                                       \
        ZERO_MEMORY((file), sizeof(memfile))								                                           \
        (file)->memblock = (block);								                                                       \
        (file)->pos = 0;								                                                               \
        (file)->bit_mode = false;								                                                       \
        (file)->mode = (access_mode_e_mode);								                                           \
        (file)->saved_pos_ptr = 0;								                                                       \
}

#define MEMFILE_CLONE(dst, src)																                           \
{															                                                           \
        MEMFILE_OPEN((dst), (src)->memblock, (src)->mode);															   \
        MEMFILE_SEEK((dst), MEMFILE_TELL((src)));															           \
        (dst)->bit_mode = (src)->bit_mode;															                   \
        (dst)->saved_pos_ptr = (src)->saved_pos_ptr;															       \
        memcpy(&(dst)->saved_pos, &(src)->saved_pos, ARRAY_LENGTH((src)->saved_pos));								   \
}

#define MEMFILE_SEEK(_file_, _position_)												                                   \
({												                                                                       \
        bool ___status = true;												                                               \
        {                                                                                                               \
                offset_t ___file_size = 0;                                                                                                                                           \
                MEMBLOCK_SIZE(&___file_size, (_file_)->memblock);                                                                                                                   \
                if (unlikely((_position_) >= ___file_size)) {                                                                                                                       \
                        if ((_file_)->mode == READ_WRITE) {                                                                                                                       \
                                offset_t new_size = (_position_) + 1;                                                                                                               \
                                MEMBLOCK_RESIZE((_file_)->memblock, new_size);                                                                                                   \
                        } else {                                                                                                                                               \
                                ___status = error(ERR_MEMSTATE, NULL);                                                                                                               \
                        }                                                                                                                                                       \
                }                                                                                                                                                               \
                (_file_)->pos = (_position_);                                                                                                                                       \
        }                                                                                                               \
        ___status;												                                                           \
})

#define MEMFILE_SEEK_FROM_HERE(file, where)				                                                               \
{				                                                                                                       \
        offset_t now = MEMFILE_TELL((file));				                                                           \
        offset_t then = now + (where);				                                                                   \
        MEMFILE_SEEK((file), then);				                                                                       \
}

#define MEMFILE_GROW(file_in, grow_by_bytes)									                                       \
{									                                                                                   \
        if (likely((grow_by_bytes) > 0)) {									                                           \
                offset_t block_size = 0;									                                           \
                MEMBLOCK_SIZE(&block_size, (file_in)->memblock);									                   \
                MEMBLOCK_RESIZE((file_in)->memblock, (block_size + (grow_by_bytes)));								   \
        }									                                                                           \
}

#define MEMFILE_GET_OFFSET(position, file)							                                                   \
{							                                                                                           \
        *(position) = file->pos;							                                                           \
}

#define MEMFILE_SIZE(file)							                                                                   \
({							                                                                                           \
        u64 ret;							                                                                           \
        if (!(file) || !(file)->memblock) {							                                                   \
                ret = 0;							                                                                   \
        } else {							                                                                           \
                MEMBLOCK_SIZE(&ret, (file)->memblock);							                                       \
        }							                                                                                   \
        ret;							                                                                               \
})

#define MEMFILE_CUT(file, how_many_bytes)							                                                   \
({							                                                                                           \
        offset_t block_size = 0;							                                                           \
        MEMBLOCK_SIZE(&block_size, (file)->memblock);							                                       \
        bool memfile_cut_status;							                                                                       \
        if ((how_many_bytes) > 0 && block_size > (how_many_bytes)) {							                       \
                size_t new_block_size = block_size - (how_many_bytes);							                       \
                MEMBLOCK_RESIZE((file)->memblock, new_block_size);							                           \
                (file)->pos = JAK_MIN((file)->pos, new_block_size);							                           \
                memfile_cut_status = true;							                                                               \
        } else {							                                                                           \
                memfile_cut_status = error(ERR_ILLEGALARG, NULL);							                                       \
        }							                                                                                   \
        memfile_cut_status;							                                                                               \
})

#define MEMFILE_REMAIN_SIZE(file)														                               \
({														                                                               \
        assert((file)->pos <= MEMFILE_SIZE((file)));														           \
        (MEMFILE_SIZE((file)) - (file)->pos);														                   \
})

#define MEMFILE_SHRINK(file)														                                   \
({														                                                               \
        bool memfile_shrink_status;														                                           \
        if ((file)->mode == READ_WRITE) {														                       \
                MEMBLOCK_SHRINK((file)->memblock);														               \
                u64 size;														                                       \
                MEMBLOCK_SIZE(&size, (file)->memblock);														           \
                assert(size == (file)->pos);														                   \
                memfile_shrink_status = true;														                                   \
        } else {														                                               \
                memfile_shrink_status = error(ERR_WRITEPROT, NULL);														           \
        }														                                                       \
        memfile_shrink_status;														                                                   \
})

#define MEMFILE_READ(file, nbytes)											                                           \
({											                                                                           \
        const char *memfile_peek_result = MEMFILE_PEEK((file), (nbytes));											               \
        (file)->pos += (nbytes);											                                           \
        memfile_peek_result;											                                                               \
})

#define MEMFILE_READ_BYTE(file)								                                                           \
        *MEMFILE_READ_TYPE((file), u8)

#define MEMFILE_PEEK_BYTE(file)								                                                           \
        *MEMFILE_PEEK_TYPE(file, u8)

#define MEMFILE_READ_U64(file)								                                                           \
        *MEMFILE_READ_TYPE((file), u64)

#define MEMFILE_READ_I64(file)						                                                                   \
		*MEMFILE_READ_TYPE((file), i64)


#define MEMFILE_SKIP(file, nbytes)															                           \
({															                                                           \
        bool memfile_skip_status = true;															                                   \
        {                                                                                               \
                offset_t required_size = (file)->pos + (nbytes);                                                                                                                           \
                (file)->pos += (nbytes);                                                                                                                                                   \
                offset_t file_size = 0;                                                                                                                                                       \
                MEMBLOCK_SIZE(&file_size, (file)->memblock);                                                                                                                               \
                if (unlikely(required_size >= file_size)) {                                                                                                                                   \
                        if ((file)->mode == READ_WRITE) {                                                                                                                                   \
                                MEMBLOCK_RESIZE((file)->memblock, required_size * 1.7f);                                                                           \
                        } else {                                                                                                                                                           \
                                error(ERR_WRITEPROT, NULL);                                                                                                                                   \
                                memfile_skip_status = false;                                                                                                                                               \
                        }                                                                                                                                                                   \
                }                                                                                                                                                                           \
                if (likely(memfile_skip_status)) {                                                                                                                                                       \
                        MEMFILE_UPDATE_LAST_BYTE((file)->memblock, (file)->pos);                                                                                                   \
                }                                                                                                                                                                           \
                assert((file)->pos < MEMFILE_SIZE((file)));                                                                                                                                   \
        }                                                                               \
        memfile_skip_status;															                                               \
})

#define MEMFILE_PEEK(file, nbytes)												                                       \
({												                                                                       \
        const char *result = NULL;												                                       \
        {                                                                                                       \
                offset_t file_size = 0;												                                           \
                MEMBLOCK_SIZE(&file_size, (file)->memblock);												                   \
                if (unlikely((file)->pos + (nbytes) > file_size)) {												               \
                        error(ERR_READOUTOFBOUNDS, NULL);												                       \
                        result = NULL;												                                           \
                } else {												                                                       \
                                char *ptr = (char *) MEMBLOCK_RAW_DATA((file)->memblock);											   \
                                if (ptr) {																				               \
                                result = ptr + (file)->pos;												                           \
                                }														                                               \
                }												                                                               \
        }                                                                       \
        result;												                                                           \
})

#define MEMFILE_WRITE_BYTE(file, data)							                                                       \
{																                                                       \
        u8 byte = data;												                                                   \
        MEMFILE_WRITE((file), &(byte), sizeof(u8));												                       \
}

#define MEMFILE_BEGIN_BIT_MODE(file)													                               \
({													                                                                   \
        bool memfile_begin_bit_mode_status = true;													                                           \
        if ((file)->mode == READ_WRITE) {													                           \
                (file)->bit_mode = true;													                           \
                (file)->current_read_bit = (file)->current_write_bit = (file)->bytes_completed = 0;					   \
                (file)->bytes_completed = 0;													                       \
                offset_t offset;													                                   \
                char empty = '\0';													                                   \
                MEMFILE_GET_OFFSET(&offset, (file));													               \
                MEMFILE_WRITE((file), &empty, sizeof(char));													       \
                MEMFILE_SEEK((file), offset);													                       \
        } else {													                                                   \
                memfile_begin_bit_mode_status = error(ERR_WRITEPROT, NULL);													               \
        }													                                                           \
        memfile_begin_bit_mode_status;													                                                       \
})

static inline bool memfile_write_bit(memfile *file, bool flag)
{
        file->current_read_bit = 0;

        if (file->bit_mode) {
                if (file->current_write_bit < 8) {
                        offset_t offset;
                        MEMFILE_GET_OFFSET(&offset, file);
                        char byte = *MEMFILE_READ(file, sizeof(char));
                        char mask = 1 << file->current_write_bit;
                        if (flag) {
                                SET_BITS(byte, mask);
                        } else {
                                UNSET_BITS(byte, mask);
                        }
                        MEMFILE_SEEK(file, offset);
                        MEMFILE_WRITE(file, &byte, sizeof(char));
                        MEMFILE_SEEK(file, offset);
                        file->current_write_bit++;
                } else {
                    file->current_write_bit = 0;
                    file->bytes_completed++;
                    char empty = '\0';
                    offset_t off;
                    MEMFILE_SKIP(file, 1);
                    MEMFILE_GET_OFFSET(&off, file);
                    MEMFILE_WRITE(file, &empty, sizeof(char));
                    MEMFILE_SEEK(file, off);

                    return memfile_write_bit(file, flag);
                }
                return true;
        } else {
                error(ERR_NOBITMODE, NULL);
                return false;
        }
}

#define MEMFILE_READ_BIT(file)																		                   \
({																		                                               \
        bool memfile_read_bit_status = true;																		                       \
        if (!(file)) {																		                           \
                memfile_read_bit_status = false;																		                   \
        } else {																		                               \
			(file)->current_write_bit = 0;																		       \
			if ((file)->bit_mode) {																		               \
			        if ((file)->current_read_bit < 8) {																   \
			                offset_t offset;																		   \
			                MEMFILE_GET_OFFSET(&offset, (file));													   \
			                char mask = 1 << (file)->current_read_bit;												   \
			                char byte = *MEMFILE_READ((file), sizeof(char));										   \
			                MEMFILE_SEEK((file), offset);															   \
			                bool result = ((byte & mask) >> (file)->current_read_bit) == true;						   \
			                (file)->current_read_bit++;																   \
			                memfile_read_bit_status = result;																		   \
			        } else {																		                   \
			                (file)->current_read_bit = 0;															   \
			                MEMFILE_SKIP((file), sizeof(char));														   \
			                memfile_read_bit_status = MEMFILE_READ_BIT((file));														   \
			        }																		                           \
			} else {																		                           \
			        error(ERR_NOBITMODE, NULL);																		   \
			        memfile_read_bit_status = false;																		               \
			}																		                                   \
        }																		                                       \
        memfile_read_bit_status;																		                                   \
})

#define MEMFILE_SEEK_TO_START(file)											                                           \
        MEMFILE_SEEK((file), 0)

#define MEMFILE_SEEK_TO_END(file)											                                           \
({											                                                                           \
        size_t size = MEMBLOCK_LAST_USED_BYTE((file)->memblock);											           \
        MEMFILE_SEEK((file), size);											                                           \
})

#define MEMFILE_INPLACE_INSERT(file, nbytes)											                               \
        MEMBLOCK_MOVE_RIGHT((file)->memblock, (file)->pos, (nbytes));

#define MEMFILE_INPLACE_REMOVE(file, nbytes_from_here)											                       \
        MEMBLOCK_MOVE_LEFT((file)->memblock, (file)->pos, (nbytes_from_here));

#define MEMFILE_END_BIT_MODE(num_bytes_written, file)											                       \
{											                                                                           \
        (file)->bit_mode = false;											                                           \
        if ((file)->current_write_bit <= 8) {											                               \
                MEMFILE_SKIP((file), 1);											                                   \
                (file)->bytes_completed++;											                                   \
        }											                                                                   \
        if ((num_bytes_written) != NULL) { 											                                   \
        	size_t *assign = (num_bytes_written);											                           \
        	*assign = (file)->bytes_completed;											                               \
        }											                                                                   \
        (file)->current_write_bit = (file)->bytes_completed = 0;											           \
}

#define MEMFILE_CURRENT_POS(file, nbytes)																               \
({																                                                       \
		void *data = NULL;																                               \
        if ((file) && (nbytes) > 0) {																                   \
                offset_t file_size = 0;																                   \
                MEMBLOCK_SIZE(&file_size, (file)->memblock);														   \
                offset_t required_size = (file)->pos + (nbytes);													   \
                if (unlikely((file)->pos + (nbytes) >= file_size)) {												   \
                        if ((file)->mode == READ_WRITE) {															   \
                                MEMBLOCK_RESIZE((file)->memblock, required_size * 1.7f);							   \
                        } else {																                       \
                                error(ERR_WRITEPROT, NULL);															   \
                                data = NULL;																           \
                        }																                               \
                }																                                       \
                data = (void *) MEMFILE_PEEK((file), (nbytes));														   \
        }																                                               \
        data;																                                           \
})

#define MEMFILE_HEXDUMP(str_buf_sb, file)																               \
{																                                                       \
        DECLARE_AND_INIT(offset_t, block_size)																           \
        MEMBLOCK_SIZE(&block_size, (file)->memblock);																   \
        hexdump((str_buf_sb), MEMBLOCK_RAW_DATA((file)->memblock), block_size);										   \
}

#define MEMFILE_HEXDUMP_PRINTF(cfile, memfile)													                       \
({													                                                                   \
        DECLARE_AND_INIT(offset_t, block_size)													                       \
        MEMBLOCK_SIZE(&block_size, (memfile)->memblock);													           \
        hexdump_print((cfile), MEMBLOCK_RAW_DATA((memfile)->memblock), block_size);									   \
})

#define MEMFILE_HEXDUMP_PRINT(memfile)						                                                           \
        MEMFILE_HEXDUMP_PRINTF(stdout, (memfile));

#ifdef __cplusplus
}
#endif

#endif