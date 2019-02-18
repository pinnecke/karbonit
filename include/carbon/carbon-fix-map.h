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

#ifndef CARBON_FIX_MAP_H
#define CARBON_FIX_MAP_H

#include "carbon-common.h"
#include "carbon-vector.h"
#include "carbon-spinlock.h"

CARBON_BEGIN_DECL

typedef struct
{
    bool     in_use_flag;  /* flag indicating if bucket is in use */
    int32_t  displacement; /* difference between intended position during insert, and actual position in table */
    uint32_t num_probs;    /* number of probe calls to this bucket */
    uint64_t data_idx;      /* position of key element in owning carbon_fix_map_t structure */
} carbon_bucket_t;

/**
 * Hash table implementation specialized for key and value types of fixed-length size, and where comparision
 * for equals is byte-compare. With this, calling a (type-dependent) compare function becomes obsolete.
 *
 * Example: mapping of u64 to u32.
 *
 * This hash table is optimized to reduce access time to elements. Internally, a robin-hood hashing technique is used.
 *
 * Note: this implementation does not support string or pointer types. The structure is thread-safe by a spinlock
 * lock implementation.
 */
typedef struct
{
    carbon_vec_t key_data;
    carbon_vec_t value_data;
    carbon_vec_t ofType(carbon_bucket_t) table;
    carbon_spinlock_t lock;
    uint32_t size;
    carbon_err_t err;
} carbon_fix_map_t;

CARBON_DEFINE_GET_ERROR_FUNCTION(fix_map, carbon_fix_map_t, map);

CARBON_EXPORT(bool)
carbon_fix_map_create(carbon_fix_map_t *map, carbon_err_t *err, size_t key_size, size_t value_size, size_t capacity);

CARBON_EXPORT(carbon_fix_map_t *)
carbon_fix_map_cpy(carbon_fix_map_t *src);

CARBON_EXPORT(bool)
carbon_fix_map_drop(carbon_fix_map_t *map);

CARBON_EXPORT(bool)
carbon_fix_map_clear(carbon_fix_map_t *map);

CARBON_EXPORT(bool)
carbon_fix_map_lock(carbon_fix_map_t *map);

CARBON_EXPORT(bool)
carbon_fix_map_unlock(carbon_fix_map_t *map);

CARBON_EXPORT(bool)
carbon_fix_map_insert_or_update(carbon_fix_map_t *map, const void *keys, const void *values, uint_fast32_t num_pairs);

CARBON_EXPORT(bool)
carbon_fix_map_remove_if_contained(carbon_fix_map_t *map, const void *keys, size_t num_pairs);

CARBON_EXPORT(const void *)
carbon_fix_map_get_value(carbon_fix_map_t *map, const void *key);

CARBON_EXPORT(bool)
carbon_fix_map_get_fload_factor(float *factor, carbon_fix_map_t *map);

CARBON_EXPORT(bool)
carbon_fix_map_rehash(carbon_fix_map_t *map);

CARBON_END_DECL

#endif
