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

#ifndef VECTOR_H
#define VECTOR_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <sys/mman.h>

#include <jakson/stdinc.h>
#include <jakson/mem/file.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DECLARE_PRINTER_FUNC(type)                                                                                     \
    void vector_##type##_printer_func(memfile *dst, void ofType(T) *values, size_t num_elems);

DECLARE_PRINTER_FUNC(u_char)

DECLARE_PRINTER_FUNC(i8)

DECLARE_PRINTER_FUNC(i16)

DECLARE_PRINTER_FUNC(i32)

DECLARE_PRINTER_FUNC(i64)

DECLARE_PRINTER_FUNC(u8)

DECLARE_PRINTER_FUNC(u16)

DECLARE_PRINTER_FUNC(u32)

DECLARE_PRINTER_FUNC(u64)

DECLARE_PRINTER_FUNC(size_t)

#define VECTOR_PRINT_UCHAR  vector_u_char_printer_func
#define VECTOR_PRINT_UINT8  vector_u8_printer_func
#define VECTOR_PRINT_UINT16 vector_u16_printer_func
#define VECTOR_PRINT_UINT32 vector_u32_printer_func
#define VECTOR_PRINT_UINT64 vector_u64_printer_func
#define VECTOR_PRINT_INT8   vector_i8_printer_func
#define VECTOR_PRINT_INT16  vector_i16_printer_func
#define VECTOR_PRINT_INT32  vector_i32_printer_func
#define VECTOR_PRINT_INT64  vector_i64_printer_func
#define VECTOR_PRINT_SIZE_T vector_size_t_printer_func

/**
 * An implementation of the concrete data type Vector, a resizeable dynamic array.
 */
typedef struct vec_t {
        /**
         *  Fixed number of bytes for a single element that should be stored in the vec_t
         */
        size_t elem_size;

        /**
         *  The number of elements currently stored in the vec_t
         */
        u32 num_elems;

        /**
         *  The number of elements for which currently memory is reserved
         */
        u32 cap_elems;

        /**
        * The grow factor considered for resize operations
        */
        float grow_factor;

        /**
         * A pointer to a memory address managed by 'allocator' that contains the user data
         */
        void *base;
} vec_t;

/**
 * Utility implementation of generic vec_t to specialize for type of 'char *'
 */
typedef vec_t ofType(const char *) string_vector_t;

/**
 * Constructs a new vec_t for elements of size 'elem_size', reserving memory for 'cap_elems' elements using
 * the allocator 'alloc'.
 *
 * @param out non-null vec_t that should be constructed
 * @param alloc an allocator
 * @param elem_size fixed-length element size
 * @param cap_elems number of elements for which memory should be reserved
 * @return STATUS_OK if success, and STATUS_NULLPTR in case of NULL pointer parameters
 */
bool vector_create(vec_t *out, size_t elem_size, size_t cap_elems);

bool vector_serialize(FILE *file, vec_t *vec);

bool vector_deserialize(vec_t *vec, FILE *file);

/**
 * Provides hints on the OS kernel how to deal with memory inside this vec_t.
 *
 * @param vec non-null vec_t
 * @param madviseAdvice value to give underlying <code>madvise</code> syscall and advice, see man page
 * of <code>madvise</code>
 * @return STATUS_OK if success, otherwise a value indicating the ERROR
 */
bool vector_memadvice(vec_t *vec, int madviseAdvice);

/**
 * Sets the factor for determining the reallocation size in case of a resizing operation.
 *
 * Note that <code>factor</code> must be larger than one.
 *
 * @param vec non-null vec_t for which the grow factor should be changed
 * @param factor a positive real number larger than 1
 * @return STATUS_OK if success, otherwise a value indicating the ERROR
 */
bool vector_set_grow_factor(vec_t *vec, float factor);

/**
 * Frees up memory requested via the allocator.
 *
 * Depending on the allocator implementation, dropping the reserved memory might not take immediately effect.
 * The pointer 'vec' itself gets not freed.
 *
 * @param vec vec_t to be freed
 * @return STATUS_OK if success, and STATUS_NULL_PTR in case of NULL pointer to 'vec'
 */
bool vector_drop(vec_t *vec);

/**
 * Returns information on whether elements are stored in this vec_t or not.
 * @param vec non-null pointer to the vec_t
 * @return Returns <code>STATUS_TRUE</code> if <code>vec</code> is empty. Otherwise <code>STATUS_FALSE</code> unless
 *         an ERROR occurs. In case an ERROR is occured, the return value is neither <code>STATUS_TRUE</code> nor
 *         <code>STATUS_FALSE</code> but an value indicating that ERROR.
 */
bool vector_is_empty(const vec_t *vec);

/**
 * Appends 'num_elems' elements stored in 'data' into the vec_t by copying num_elems * vec->elem_size into the
 * vectors memory block.
 *
 * In case the capacity is not sufficient, the vec_t gets automatically resized.
 *
 * @param vec the vec_t in which the data should be pushed
 * @param data non-null pointer to data that should be appended. Must be at least size of 'num_elems' * vec->elem_size.
 * @param num_elems number of elements stored in data
 * @return STATUS_OK if success, and STATUS_NULLPTR in case of NULL pointer parameters
 */
bool vector_push(vec_t *vec, const void *data, size_t num_elems);

const void *vector_peek(vec_t *vec);

#define VECTOR_PEEK(vec, type) (type *)(vector_peek(vec))

/**
 * Appends 'how_many' elements of the same source stored in 'data' into the vec_t by copying how_many * vec->elem_size
 * into the vectors memory block.
 *
 * In case the capacity is not sufficient, the vec_t gets automatically resized.
 *
 * @param vec the vec_t in which the data should be pushed
 * @param data non-null pointer to data that should be appended. Must be at least size of one vec->elem_size.
 * @param num_elems number of elements stored in data
 * @return STATUS_OK if success, and STATUS_NULLPTR in case of NULL pointer parameters
 */
bool vector_repeated_push(vec_t *vec, const void *data, size_t how_often);

/**
 * Returns a pointer to the last element in this vec_t, or <code>NULL</code> is the vec_t is already empty.
 * The number of elements contained in that vec_t is decreased, too.
 *
 * @param vec non-null pointer to the vec_t
 * @return Pointer to last element, or <code>NULL</code> if vec_t is empty
 */
const void *vector_pop(vec_t *vec);

bool vector_clear(vec_t *vec);

/**
 * Shinks the vec_t's internal data block to fits its real size, i.e., remove reserved memory
 *
 * @param vec
 * @return
 */
bool vector_shrink(vec_t *vec);

/**
 * Increases the capacity of that vec_t according the internal grow factor
 * @param numNewSlots a pointer to a value that will store the number of newly created slots in that vec_t if
 *                      <code>num_new_slots</code> is non-null. If this parameter is <code>NULL</code>, it is ignored.
 * @param vec non-null pointer to the vec_t that should be grown
 * @return STATUS_OK in case of success, and another value indicating an ERROR otherwise.
 */
bool vector_grow(size_t *numNewSlots, vec_t *vec);

bool vector_grow_to(vec_t *vec, size_t capacity);

/**
 * Returns the number of elements currently stored in the vec_t
 *
 * @param vec the vec_t for which the operation is started
 * @return 0 in case of NULL pointer to 'vec', or the number of elements otherwise.
 */
size_t vector_length(const vec_t *vec);

#define VECTOR_GET(vec, pos, type) (type *) vector_at(vec, pos)

#define VECTOR_NEW_AND_GET(vec, type)                                                                              \
({                                                                                                                     \
    type obj;                                                                                                          \
    size_t vectorLength = vector_length(vec);                                                                      \
    vector_push(vec, &obj, 1);                                                                                     \
    VECTOR_GET(vec, vectorLength, type);                                                                           \
})

const void *vector_at(const vec_t *vec, size_t pos);

/**
 * Returns the number of elements for which memory is currently reserved in the vec_t
 *
 * @param vec the vec_t for which the operation is started
 * @return 0 in case of NULL pointer to 'vec', or the number of elements otherwise.
 */
size_t vector_capacity(const vec_t *vec);

/**
 * Set the internal size of <code>vec</code> to its capacity.
 */
bool vector_enlarge_size_to_capacity(vec_t *vec);

bool vector_zero_memory(vec_t *vec);

bool vector_zero_memory_in_range(vec_t *vec, size_t from, size_t to);

bool vector_set(vec_t *vec, size_t pos, const void *data);

bool vector_cpy(vec_t *dst, const vec_t *src);

bool vector_cpy_to(vec_t *dst, vec_t *src);

/**
 * Gives raw data access to data stored in the vec_t; do not manipulate this data since otherwise the vec_t
 * might get corrupted.
 *
 * @param vec the vec_t for which the operation is started
 * @return pointer to user-data managed by this vec_t
 */
const void *vector_data(const vec_t *vec);

char *vector_string(const vec_t ofType(T) *vec,
                    void (*printerFunc)(memfile *dst, void ofType(T) *values, size_t num_elems));

#define VECTOR_ALL(vec, type) (type *) vector_data(vec)

#ifdef __cplusplus
}
#endif

#endif
