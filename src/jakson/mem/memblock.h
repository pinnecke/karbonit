/**
 * memblock - memory block as abstraction of resizable continuous memory area
 *
 * Copyright 2018 Marcus Pinnecke
 */

#ifndef HAD_MEMBLOCK_H
#define HAD_MEMBLOCK_H

#include <jakson/stdinc.h>
#include <jakson/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct memblock {
    offset_t blockLength;
    offset_t last_byte;
    void *base;
} memblock;

#define memblock_create(block, size)						                                                           \
({												                                                                       \
        bool status = true;                                                                                            \
        if (UNLIKELY(size == 0)) {							                                                           \
            *(block) = NULL;                                                                                           \
            status = error(ERR_ILLEGALARG, NULL);		                                                               \
        } else {									                                                                   \
            struct memblock *result = (struct memblock *) MALLOC(sizeof(struct memblock));	                           \
            ZERO_MEMORY(result, sizeof(memblock));			                                                           \
            result->blockLength = size;						                                                           \
            result->last_byte = 0;							                                                           \
            result->base = MALLOC(size);					                                                           \
            *(block) = result;								                                                           \
        }                                                                                                              \
        status;									                                                                       \
})

#define memblock_drop(block)                                                                                           \
{                                                                                                                      \
    free((block)->base);                                                                                               \
    free((block));                                                                                                     \
}

#define memblock_from_file(block, file, nbytes)							                                               \
({																		                                               \
        memblock_create((block), nbytes);								                                               \
        size_t numRead = fread((*(block))->base, 1, nbytes, file);							                           \
        ((size_t) numRead == (size_t) nbytes);												                           \
})

#define memblock_from_raw_data(block, data, nbytes)													                   \
{																									                   \
        struct memblock *result = (struct memblock *) MALLOC(sizeof(struct memblock));				                   \
        result->blockLength = nbytes;																                   \
        result->last_byte = nbytes;																	                   \
        result->base = MALLOC(nbytes);																                   \
        memcpy(result->base, data, nbytes);															                   \
}

#define memblock_raw_data(block)									                                                   \
        ((block) && (block)->base ? (block)->base : NULL)

#define memblock_write(block, position, data, nbytes)										                           \
({										                                                                               \
		bool status;										                                                           \
        if (LIKELY(position + nbytes < block->blockLength)) {										                   \
                memcpy(((char *)block->base) + position, data, nbytes);										           \
                block->last_byte = JAK_MAX(block->last_byte, position + nbytes);									   \
                status = true;										                                                   \
        } else {										                                                               \
                status = false;										                                                   \
        }										                                                                       \
        status;										                                                                   \
})

#define memblock_cpy(dst, src)										                                                   \
{										                                                                               \
        memblock_create((dst), (src)->blockLength);										                               \
        memcpy((*(dst))->base, (src)->base, (src)->blockLength);										               \
        assert((*(dst))->base);										                                                   \
        assert((*(dst))->blockLength == (src)->blockLength);										                   \
        assert(memcmp((*(dst))->base, (src)->base, (src)->blockLength) == 0);										   \
        (*(dst))->last_byte = (src)->last_byte;										                                   \
}

#define memblock_shrink(block)																		                   \
{																		                                               \
        (block)->blockLength = (block)->last_byte;																	   \
        (block)->base = realloc((block)->base, (block)->blockLength);												   \
}

#define memblock_move_right(block, where, nbytes)		                                                               \
		memblock_move_ex((block), (where), nbytes, true);

#define memblock_move_left(block, where, nbytes)															           \
({															                                                           \
		bool status = true;															                                   \
		if (UNLIKELY((where) + (nbytes) >= (block)->blockLength)) {													   \
			status = error(ERR_OUTOFBOUNDS, NULL);															           \
		} else {															                                           \
			size_t remainder = (block)->blockLength - (where) - (nbytes);											   \
	        if (remainder > 0) {															                           \
	                memmove((block)->base + (where), (block)->base + (where) + (nbytes), remainder);				   \
	                assert((block)->last_byte >= (nbytes));															   \
	                (block)->last_byte -= (nbytes);															           \
	                ZERO_MEMORY((block)->base + (block)->blockLength - (nbytes), (nbytes))							   \
	        } else {															                                       \
	                status = false;															                           \
	        }															                                               \
		}															                                                   \
        status;															                                               \
})

#define memblock_move_ex(block, where, nbytes, zero_out)															   \
({																                                                       \
		bool status = true;																                               \
		if (UNLIKELY(where >= (block)->blockLength)) {																   \
			error(ERR_OUTOFBOUNDS, NULL);																               \
			status = false;																                               \
		} else {																                                       \
			if (UNLIKELY(nbytes == 0)) {																               \
				error(ERR_ILLEGALARG, NULL);																           \
				status = false;																                           \
			} else {																                                   \
		        if ((block)->last_byte + nbytes > (block)->blockLength) {											   \
		                size_t new_length = ((block)->last_byte + nbytes);											   \
		                (block)->base = realloc((block)->base, new_length);											   \
		                if (zero_out) {																                   \
		                        ZERO_MEMORY((block)->base + (block)->blockLength, (new_length - (block)->blockLength));\
		                }																                               \
		                (block)->blockLength = new_length;															   \
		        }																                                       \
		        memmove((block)->base + where + nbytes, (block)->base + where, (block)->last_byte - where);			   \
		        if (zero_out) {																                           \
		                ZERO_MEMORY((block)->base + where, nbytes);													   \
		        }																                                       \
		        (block)->last_byte += nbytes;																           \
			}																                                           \
		}																                                               \
        status;																                                           \
})

#define memblock_zero_out(block)										                                               \
        ZERO_MEMORY((block)->base, (block)->blockLength)

#define memblock_size(size, block)										                                               \
{										                                                                               \
        *(size) = (block)->blockLength;										                                           \
}

#define memblock_last_used_byte(block)										                                           \
        ((block) ? (block)->last_byte : 0)

#define memblock_write_to_file(cfile, block)												                           \
({												                                                                       \
        size_t nwritten = fwrite((block)->base, (block)->blockLength, 1, (cfile));									   \
        (nwritten == 1);												                                               \
})

#define memblock_resize(block, size)							                                                       \
({							                                                                                           \
		bool status = true;							                                                                   \
        if (UNLIKELY((size) == 0)) {							                                                       \
                status = error(ERR_ILLEGALARG, NULL);							                                       \
        } else {							                                                                           \
        	(block)->base = realloc((block)->base, (size));							                                   \
	        if ((size) > (block)->blockLength) {							                                           \
	                ZERO_MEMORY(((char *)(block)->base) + (block)->blockLength, ((size) - (block)->blockLength));	   \
	        }							                                                                               \
	        (block)->blockLength = (size);							                                                   \
        }							                                                                                   \
        status;							                                                                               \
})

#define memblock_move_contents_and_drop(block)					                                                       \
({					                                                                                                   \
        void *result = (block)->base;					                                                               \
        (block)->base = NULL;					                                                                       \
        free((block));					                                                                               \
        result;					                                                                                       \
})

#define memfile_update_last_byte(block, where)							                                               \
{							                                                                                           \
		bool status = true;							                                                                   \
        if (UNLIKELY((where) >= (block)->blockLength)) {							                                   \
        	status = error(ERR_ILLEGALSTATE, NULL);							                                           \
        } else {							                                                                           \
        	(block)->last_byte = JAK_MAX((block)->last_byte, (where));							                       \
        }							                                                                                   \
        status;							                                                                               \
}

#ifdef __cplusplus
}
#endif

#endif
