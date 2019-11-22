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

#define memfile_peek_type(file, type)                                                                                  \
({                                                                                                                     \
        assert (memfile_remain_size((file)) >= sizeof(type));                                                          \
        type* ret = (type*) memfile_peek((file), sizeof(type));                                                        \
        ret;                                                                                                           \
})

#define memfile_read_type(file, type)                                                                                  \
({                                                                                                                     \
        assert (memfile_remain_size(file) >= sizeof(type));                                                            \
        (type *) memfile_read(file, sizeof(type));                                                                     \
})

#define memfile_read_type_list(file, type, how_many)                                                                   \
     (const type *) memfile_read(file, how_many * sizeof(type))

#define memfile_tell(file)                                                                                             \
({                                                                                                                     \
        offset_t __offset = 0;                                                                                         \
        memfile_get_offset(&__offset, (file));                                                                         \
        __offset;                                                                                                      \
})

#define MEMFILE_SKIP_BYTE(file)                                                                                        \
        memfile_skip(file, sizeof(u8))

#define memfile_write(file, data, nbytes)																		       \
({		                                                                                                               \
        bool status = false;																                           \
        if ((file)->mode == READ_WRITE) {																		       \
                if (LIKELY(nbytes != 0)) {																		       \
                        offset_t file_size = 0;																		   \
                        memblock_size(&file_size, (file)->memblock);												   \
                        offset_t required_size = (file)->pos + nbytes;												   \
                        if (UNLIKELY(required_size >= file_size)) {													   \
                                memblock_resize((file)->memblock, required_size * 1.7f);							   \
                        }																		                       \
																		                                               \
                        if (UNLIKELY(!memblock_write((file)->memblock, (file)->pos, ((char *)data), nbytes))) {		   \
                                status = false;																		   \
                        } else {	 																	               \
                                (file)->pos += nbytes;																   \
                        }                                                                                              \
                }																		                               \
                status = true;																		                   \
        } else {																		                               \
                error(ERR_WRITEPROT, NULL);																		       \
                status = false;																		                   \
        }																		                                       \
        status;                                                                                                        \
})

#define memfile_save_position(file)										                                               \
({										                                                                               \
        offset_t pos = memfile_tell((file));										                                   \
        if (LIKELY((file)->saved_pos_ptr < (i8) (ARRAY_LENGTH((file)->saved_pos)))) {								   \
                (file)->saved_pos[(file)->saved_pos_ptr++] = pos;										               \
        } else {										                                                               \
                error(ERR_STACK_OVERFLOW, NULL);										                               \
                pos = 0;										                                                       \
        }										                                                                       \
        pos;										                                                                   \
})

#define memfile_restore_position(file)												                                   \
({												                                                                       \
		bool status;												                                                   \
        if (LIKELY((file)->saved_pos_ptr >= 0)) {												                       \
                offset_t pos = (file)->saved_pos[--(file)->saved_pos_ptr];											   \
                memfile_seek((file), pos);												                               \
                status = true;												                                           \
        } else {												                                                       \
                status = error(ERR_STACK_UNDERFLOW, NULL);												               \
        }												                                                               \
        status;												                                                           \
})

#define memfile_ensure_space(file, nbytes)											                                   \
({											                                                                           \
        DECLARE_AND_INIT(offset_t, block_size);											                               \
        memblock_size(&block_size, (file)->memblock);											                       \
        assert((file)->pos < block_size);											                                   \
        size_t diff = block_size - (file)->pos;											                               \
        if (diff < (nbytes)) {											                                               \
                memfile_grow((file), (nbytes) - diff);											                       \
        }											                                                                   \
        memfile_save_position((file));											                                       \
        offset_t current_off = memfile_tell((file));											                       \
        signed_offset_t shift = 0;											                                           \
        for (u32 i = 0; i < (nbytes); i++) {											                               \
                char c = *memfile_read((file), 1);											                           \
                if (UNLIKELY(c != 0)) {											                                       \
                        memfile_seek((file), current_off);											                   \
                        memfile_inplace_insert((file), (nbytes) - i);											       \
                        shift += (nbytes) - i;											                               \
                        break;											                                               \
                }											                                                           \
        }											                                                                   \
        memfile_restore_position((file));											                                   \
        shift;											                                                               \
})

#define memfile_read_uintvar_stream(nbytes, file)													                   \
({													                                                                   \
        u8 nbytes_read;													                                               \
        u64 result = uintvar_stream_read(&nbytes_read, (uintvar_stream_t) memfile_peek((file), sizeof(char)));		   \
        memfile_skip((file), nbytes_read);													                           \
        if ((nbytes) != NULL) { u8 *assign_nbytes = (nbytes); *assign_nbytes = nbytes_read; }						   \
        result;													                                                       \
})

#define memfile_skip_uintvar_stream(file)						                                                       \
        memfile_read_uintvar_stream(NULL, (file));

#define memfile_peek_uintvar_stream(nbytes, file)									                                   \
({									                                                                                   \
        memfile_save_position((file));									                                               \
        u64 result = memfile_read_uintvar_stream((nbytes), (file));									                   \
        memfile_restore_position((file));									                                           \
        result;									                                                                       \
})

#define memfile_write_uintvar_stream(nbytes_moved, file, value)												           \
({												                                                                       \
        u8 required_blocks = UINTVAR_STREAM_REQUIRED_BLOCKS((value));												   \
        signed_offset_t shift = memfile_ensure_space((file), required_blocks);										   \
        uintvar_stream_t dst = (uintvar_stream_t) memfile_peek((file), sizeof(char));								   \
        uintvar_stream_write(dst, (value));												                               \
        memfile_skip((file), required_blocks);												                           \
        if ((nbytes_moved) != NULL) { signed_offset_t *assign = nbytes_moved; *assign = shift; }					   \
        required_blocks;												                                               \
})

#define memfile_update_uintvar_stream(file, value)															           \
({															                                                           \
        u8 bytes_used_now, bytes_used_then;															                   \
        memfile_peek_uintvar_stream(&bytes_used_now, (file));														   \
        bytes_used_then = UINTVAR_STREAM_REQUIRED_BLOCKS((value));												   	   \
        if (bytes_used_now < bytes_used_then) {															               \
                u8 inc = bytes_used_then - bytes_used_now;															   \
                memfile_inplace_insert((file), inc);															       \
        } else if (bytes_used_now > bytes_used_then) {															       \
                u8 dec = bytes_used_now - bytes_used_then;															   \
                memfile_inplace_remove((file), dec);															       \
        }															                                                   \
        uintvar_stream_t dst = (uintvar_stream_t) memfile_peek((file), sizeof(char));								   \
        u8 required_blocks = uintvar_stream_write(dst, (value));													   \
        memfile_skip((file), required_blocks);															               \
        (bytes_used_then - bytes_used_now);															                   \
})


#define memfile_open(file, block, access_mode_e_mode)								                                   \
{								                                                                                       \
        ZERO_MEMORY((file), sizeof(memfile))								                                           \
        (file)->memblock = (block);								                                                       \
        (file)->pos = 0;								                                                               \
        (file)->bit_mode = false;								                                                       \
        (file)->mode = (access_mode_e_mode);								                                           \
        (file)->saved_pos_ptr = 0;								                                                       \
}

#define memfile_clone(dst, src)																                           \
{															                                                           \
        memfile_open((dst), (src)->memblock, (src)->mode);															   \
        memfile_seek((dst), memfile_tell((src)));															           \
        (dst)->bit_mode = (src)->bit_mode;															                   \
        (dst)->saved_pos_ptr = (src)->saved_pos_ptr;															       \
        memcpy(&(dst)->saved_pos, &(src)->saved_pos, ARRAY_LENGTH((src)->saved_pos));								   \
}

#define memfile_seek(file, position)												                                   \
({												                                                                       \
        offset_t file_size = 0;												                                           \
        bool status = true;												                                               \
        memblock_size(&file_size, (file)->memblock);												                   \
        if (UNLIKELY((position) >= file_size)) {												                       \
                if ((file)->mode == READ_WRITE) {												                       \
                        offset_t new_size = (position) + 1;												               \
                        memblock_resize((file)->memblock, new_size);												   \
                } else {												                                               \
                        status = error(ERR_MEMSTATE, NULL);												               \
                }												                                                       \
        }												                                                               \
        (file)->pos = (position);												                                       \
        status;												                                                           \
})

#define memfile_seek_from_here(file, where)				                                                               \
{				                                                                                                       \
        offset_t now = memfile_tell((file));				                                                           \
        offset_t then = now + (where);				                                                                   \
        memfile_seek((file), then);				                                                                       \
}

#define memfile_rewind(file)					                                                                       \
        (file)->pos = 0;

#define memfile_grow(file_in, grow_by_bytes)									                                       \
{									                                                                                   \
        if (LIKELY((grow_by_bytes) > 0)) {									                                           \
                offset_t block_size = 0;									                                           \
                memblock_size(&block_size, (file_in)->memblock);									                   \
                memblock_resize((file_in)->memblock, (block_size + (grow_by_bytes)));								   \
        }									                                                                           \
}

#define memfile_get_offset(position, file)							                                                   \
{							                                                                                           \
        *(position) = file->pos;							                                                           \
}

#define memfile_size(file)							                                                                   \
({							                                                                                           \
		u64 ret;							                                                                           \
        if (!(file) || !(file)->memblock) {							                                                   \
                ret = 0;							                                                                   \
        } else {							                                                                           \
                memblock_size(&ret, (file)->memblock);							                                       \
        }							                                                                                   \
        ret;							                                                                               \
})

#define memfile_cut(file, how_many_bytes)							                                                   \
({							                                                                                           \
        offset_t block_size = 0;							                                                           \
        memblock_size(&block_size, (file)->memblock);							                                       \
        bool status;							                                                                       \
        if ((how_many_bytes) > 0 && block_size > (how_many_bytes)) {							                       \
                size_t new_block_size = block_size - (how_many_bytes);							                       \
                memblock_resize((file)->memblock, new_block_size);							                           \
                (file)->pos = JAK_MIN((file)->pos, new_block_size);							                           \
                status = true;							                                                               \
        } else {							                                                                           \
                status = error(ERR_ILLEGALARG, NULL);							                                       \
        }							                                                                                   \
        status;							                                                                               \
})

#define memfile_remain_size(file)														                               \
({														                                                               \
        assert((file)->pos <= memfile_size((file)));														           \
        (memfile_size((file)) - (file)->pos);														                   \
})

#define memfile_shrink(file)														                                   \
({														                                                               \
		bool status;														                                           \
        if ((file)->mode == READ_WRITE) {														                       \
                memblock_shrink((file)->memblock);														               \
                u64 size;														                                       \
                memblock_size(&size, (file)->memblock);														           \
                assert(size == (file)->pos);														                   \
                status = true;														                                   \
        } else {														                                               \
                status = error(ERR_WRITEPROT, NULL);														           \
        }														                                                       \
        status;														                                                   \
})

#define memfile_read(file, nbytes)											                                           \
({											                                                                           \
        const char *result = memfile_peek((file), (nbytes));											               \
        (file)->pos += (nbytes);											                                           \
        result;											                                                               \
})

#define memfile_read_byte(file)								                                                           \
        *memfile_read_type((file), u8)

#define memfile_peek_byte(file)								                                                           \
        *memfile_peek_type(file, u8)

#define memfile_read_u64(file)								                                                           \
        *memfile_read_type((file), u64)

#define memfile_read_i64(file)						                                                                   \
		*memfile_read_type((file), i64)


#define memfile_skip(file, nbytes)															                           \
({															                                                           \
        offset_t required_size = (file)->pos + (nbytes);															\
        (file)->pos += (nbytes);															\
        offset_t file_size = 0;															\
        memblock_size(&file_size, (file)->memblock);															\
        bool status = true;															\
        if (UNLIKELY(required_size >= file_size)) {															\
                if ((file)->mode == READ_WRITE) {															\
                        memblock_resize((file)->memblock, required_size * 1.7f);															\
                } else {															\
                        error(ERR_WRITEPROT, NULL);															\
                        status = false;															\
                }															\
        }															\
        if (LIKELY(status)) {															\
        	memfile_update_last_byte((file)->memblock, (file)->pos);															\
        }															\
        assert((file)->pos < memfile_size((file)));															\
        status;															\
})

#define memfile_peek(file, nbytes)												\
({												\
        offset_t file_size = 0;												\
        memblock_size(&file_size, (file)->memblock);												\
        const char *result = NULL;												\
        if (UNLIKELY((file)->pos + (nbytes) > file_size)) {												\
                error(ERR_READOUTOFBOUNDS, NULL);												\
                result = NULL;												\
        } else {												\
        		char *ptr = (char *) memblock_raw_data((file)->memblock);												\
        		if (ptr) {																				\
                	result = ptr + (file)->pos;												\
        		}														\
        }												\
        result;												\
})

#define memfile_write_byte(file, data)							\
{																\
		u8 byte = data;												\
		memfile_write((file), &(byte), sizeof(u8));												\
}


#define memfile_write_zero(file, how_many)									\
{									\
        char empty = 0;									\
        while ((how_many)--) {									\
                memfile_write((file), &empty, sizeof(char));									\
        }									\
}

#define memfile_begin_bit_mode(file)													\
({													\
		bool status = true;													\
        if ((file)->mode == READ_WRITE) {													\
                (file)->bit_mode = true;													\
                (file)->current_read_bit = (file)->current_write_bit = (file)->bytes_completed = 0;													\
                (file)->bytes_completed = 0;													\
                offset_t offset;													\
                char empty = '\0';													\
                memfile_get_offset(&offset, (file));													\
                memfile_write((file), &empty, sizeof(char));													\
                memfile_seek((file), offset);													\
        } else {													\
                status = error(ERR_WRITEPROT, NULL);													\
        }													\
        status;													\
})

static inline bool memfile_write_bit(memfile *file, bool flag)
{
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
                error(ERR_NOBITMODE, NULL);
                return false;
        }
}


#define memfile_read_bit(file)																		\
({																		\
		bool status = true;																		\
        if (!(file)) {																		\
                status = false;																		\
        } else {																		\
			(file)->current_write_bit = 0;																		\
			if ((file)->bit_mode) {																		\
			        if ((file)->current_read_bit < 8) {																		\
			                offset_t offset;																		\
			                memfile_get_offset(&offset, (file));																		\
			                char mask = 1 << (file)->current_read_bit;																		\
			                char byte = *memfile_read((file), sizeof(char));																		\
			                memfile_seek((file), offset);																		\
			                bool result = ((byte & mask) >> (file)->current_read_bit) == true;																		\
			                (file)->current_read_bit++;																		\
			                status = result;																		\
			        } else {																		\
			                (file)->current_read_bit = 0;																		\
			                memfile_skip((file), sizeof(char));																		\
			                status = memfile_read_bit((file));																		\
			        }																		\
			} else {																		\
			        error(ERR_NOBITMODE, NULL);																		\
			        status = false;																		\
			}																		\
        }																		\
        status;																		\
})

#define memfile_seek_to_start(file)											\
        memfile_seek((file), 0)

#define memfile_seek_to_end(file)											\
({											\
        size_t size = memblock_last_used_byte((file)->memblock);											\
        memfile_seek((file), size);											\
})

#define memfile_inplace_insert(file, nbytes)											\
        memblock_move_right((file)->memblock, (file)->pos, (nbytes));

#define memfile_inplace_remove(file, nbytes_from_here)											\
        memblock_move_left((file)->memblock, (file)->pos, (nbytes_from_here));

#define memfile_end_bit_mode(num_bytes_written, file)											\
{											\
        (file)->bit_mode = false;											\
        if ((file)->current_write_bit <= 8) {											\
                memfile_skip((file), 1);											\
                (file)->bytes_completed++;											\
        }											\
        if ((num_bytes_written) != NULL) { 											\
        	size_t *assign = (num_bytes_written);											\
        	*assign = (file)->bytes_completed;											\
        }											\
        (file)->current_write_bit = (file)->bytes_completed = 0;											\
}

#define memfile_current_pos(file, nbytes)																\
({																\
		void *data = NULL;																\
        if ((file) && (nbytes) > 0) {																\
                offset_t file_size = 0;																\
                memblock_size(&file_size, (file)->memblock);																\
                offset_t required_size = (file)->pos + (nbytes);																\
                if (UNLIKELY((file)->pos + (nbytes) >= file_size)) {																\
                        if ((file)->mode == READ_WRITE) {																\
                                memblock_resize((file)->memblock, required_size * 1.7f);																\
                        } else {																\
                                error(ERR_WRITEPROT, NULL);																\
                                data = NULL;																\
                        }																\
                }																\
                data = (void *) memfile_peek((file), (nbytes));																\
        }																\
        data;																\
})

#define memfile_hexdump(str_buf_sb, file)																\
{																\
        DECLARE_AND_INIT(offset_t, block_size)																\
        memblock_size(&block_size, (file)->memblock);																\
        hexdump((str_buf_sb), memblock_raw_data((file)->memblock), block_size);																\
}

#define memfile_hexdump_printf(cfile, memfile)													\
({													\
        DECLARE_AND_INIT(offset_t, block_size)													\
        memblock_size(&block_size, (memfile)->memblock);													\
        hexdump_print((cfile), memblock_raw_data((memfile)->memblock), block_size);													\
})

#define memfile_hexdump_print(memfile)						\
        memfile_hexdump_printf(stdout, (memfile));


#ifdef __cplusplus
}
#endif

#endif