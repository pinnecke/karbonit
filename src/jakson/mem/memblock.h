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

#define MEMBLOCK_CREATE(block, size)						                                                           \
({												                                                                       \
        bool memblock_create_status = true;                                                                                            \
        if (unlikely(size == 0)) {							                                                           \
            *(block) = NULL;                                                                                           \
            memblock_create_status = error(ERR_ILLEGALARG, NULL);		                                                               \
        } else {									                                                                   \
            struct memblock *result = (struct memblock *) MALLOC(sizeof(struct memblock));	                           \
            ZERO_MEMORY(result, sizeof(memblock));			                                                           \
            result->blockLength = size;						                                                           \
            result->last_byte = 0;							                                                           \
            result->base = MALLOC(size);					                                                           \
            *(block) = result;								                                                           \
        }                                                                                                              \
        memblock_create_status;									                                                                       \
})

#define MEMBLOCK_DROP(block)                                                                                           \
{                                                                                                                      \
    free((block)->base);                                                                                               \
    free((block));                                                                                                     \
}

#define MEMBLOCK_FROM_FILE(block, file, nbytes)							                                               \
({																		                                               \
        MEMBLOCK_CREATE((block), nbytes);								                                               \
        size_t numRead = fread((*(block))->base, 1, nbytes, file);							                           \
        ((size_t) numRead == (size_t) nbytes);												                           \
})

#define MEMBLOCK_FROM_RAW_DATA(block, data, nbytes)													                   \
{																									                   \
        struct memblock *result = (struct memblock *) MALLOC(sizeof(struct memblock));				                   \
        result->blockLength = nbytes;																                   \
        result->last_byte = nbytes;																	                   \
        result->base = MALLOC(nbytes);																                   \
        memcpy(result->base, data, nbytes);															                   \
}

#define MEMBLOCK_RAW_DATA(block)									                                                   \
        ((block) && (block)->base ? (block)->base : NULL)

#define MEMBLOCK_WRITE(block, position, data, nbytes)										                           \
({										                                                                               \
        bool memblock_write_status;										                                                           \
        if (likely(position + nbytes < block->blockLength)) {										                   \
                memcpy(((char *)block->base) + position, data, nbytes);										           \
                block->last_byte = JAK_MAX(block->last_byte, position + nbytes);									   \
                memblock_write_status = true;										                                                   \
        } else {										                                                               \
                memblock_write_status = false;										                                                   \
        }										                                                                       \
        memblock_write_status;										                                                                   \
})

#define MEMBLOCK_CPY(dst, src)										                                                   \
{										                                                                               \
        MEMBLOCK_CREATE((dst), (src)->blockLength);										                               \
        memcpy((*(dst))->base, (src)->base, (src)->blockLength);										               \
        assert((*(dst))->base);										                                                   \
        assert((*(dst))->blockLength == (src)->blockLength);										                   \
        assert(memcmp((*(dst))->base, (src)->base, (src)->blockLength) == 0);										   \
        (*(dst))->last_byte = (src)->last_byte;										                                   \
}

#define MEMBLOCK_SHRINK(block)																		                   \
{																		                                               \
        (block)->blockLength = (block)->last_byte;																	   \
        (block)->base = realloc((block)->base, (block)->blockLength);												   \
}

#define MEMBLOCK_MOVE_RIGHT(block, where, nbytes)		                                                               \
        MEMBLOCK_MOVE_EX((block), (where), nbytes, true);

#define MEMBLOCK_MOVE_LEFT(block, where, nbytes)															           \
({															                                                           \
        bool status = true;															                                   \
        if (unlikely((where) + (nbytes) >= (block)->blockLength)) {													   \
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

#define MEMBLOCK_MOVE_EX(block, where, nbytes, zero_out)															   \
({																                                                       \
        bool status = true;																                               \
        if (unlikely(where >= (block)->blockLength)) {																   \
            error(ERR_OUTOFBOUNDS, NULL);																               \
            status = false;																                               \
        } else {																                                       \
            if (unlikely(nbytes == 0)) {																               \
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

#define MEMBLOCK_ZERO_OUT(block)										                                               \
        ZERO_MEMORY((block)->base, (block)->blockLength)

#define MEMBLOCK_SIZE(size, block)										                                               \
{										                                                                               \
        *(size) = (block)->blockLength;										                                           \
}

#define MEMBLOCK_LAST_USED_BYTE(block)										                                           \
        ((block) ? (block)->last_byte : 0)

#define MEMBLOCK_WRITE_TO_FILE(cfile, block)												                           \
({												                                                                       \
        size_t nwritten = fwrite((block)->base, (block)->blockLength, 1, (cfile));									   \
        (nwritten == 1);												                                               \
})

#define MEMBLOCK_RESIZE(block, size)							                                                       \
({							                                                                                           \
        bool memblock_resize_status = true;							                                                                   \
        if (unlikely((size) == 0)) {							                                                       \
                memblock_resize_status = error(ERR_ILLEGALARG, NULL);							                                       \
        } else {							                                                                           \
            (block)->base = realloc((block)->base, (size));							                                   \
            if ((size) > (block)->blockLength) {							                                           \
                    ZERO_MEMORY(((char *)(block)->base) + (block)->blockLength, ((size) - (block)->blockLength));	   \
            }							                                                                               \
            (block)->blockLength = (size);							                                                   \
        }							                                                                                   \
        memblock_resize_status;							                                                                               \
})

#define MEMBLOCK_MOVE_CONTENTS_AND_DROP(block)					                                                       \
({					                                                                                                   \
        void *result = (block)->base;					                                                               \
        (block)->base = NULL;					                                                                       \
        free((block));					                                                                               \
        result;					                                                                                       \
})

#define MEMFILE_UPDATE_LAST_BYTE(block, where)							                                               \
{							                                                                                           \
        bool memfile_update_last_byte_status = true;							                                                                   \
        if (unlikely((where) >= (block)->blockLength)) {							                                   \
            memfile_update_last_byte_status = error(ERR_ILLEGALSTATE, NULL);							                                           \
        } else {							                                                                           \
            (block)->last_byte = JAK_MAX((block)->last_byte, (where));							                       \
        }							                                                                                   \
        memfile_update_last_byte_status;							                                                                               \
}

#ifdef __cplusplus
}
#endif

#endif
