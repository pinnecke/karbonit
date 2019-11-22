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

#include <jakson/std/hash.h>
#include <jakson/std/hash/set.h>

#define _HASH_SET_HASHCODE_OF(size, x) HASH_BERNSTEIN(size, x)
#define FIX_MAP_AUTO_REHASH_LOADFACTOR 0.9f

bool hashset_create(hashset *map, size_t key_size, size_t capacity)
{
        int err_code = ERR_INITFAILED;

        map->size = 0;

        SUCCESS_OR_JUMP(vector_create(&map->key_data, key_size, capacity), error_handling);
        SUCCESS_OR_JUMP(vector_create(&map->table, sizeof(hashset_bucket), capacity),
                            cleanup_key_data_and_error);
        SUCCESS_OR_JUMP(vector_enlarge_size_to_capacity(&map->table), cleanup_key_value_table_and_error);
        SUCCESS_OR_JUMP(vector_zero_memory(&map->table), cleanup_key_value_table_and_error);
        SUCCESS_OR_JUMP(spinlock_init(&map->lock), cleanup_key_value_table_and_error);

        return true;

        cleanup_key_value_table_and_error:
        if (!vector_drop(&map->table)) {
                err_code = ERR_DROPFAILED;
        }
        cleanup_key_data_and_error:
        if (!vector_drop(&map->key_data)) {
                err_code = ERR_DROPFAILED;
        }
        error_handling:
        error(err_code, NULL);
        return false;
}

bool hashset_drop(hashset *map)
{
        bool status = true;

        status &= vector_drop(&map->table);
        status &= vector_drop(&map->key_data);

        if (!status) {
                error(ERR_DROPFAILED, NULL);
        }

        return status;
}

vec *hashset_keys(hashset *map)
{
        if (map) {
                vec *result = MALLOC(sizeof(vec));
                vector_create(result, map->key_data.elem_size, map->key_data.num_elems);
                for (u32 i = 0; i < map->table.num_elems; i++) {
                        hashset_bucket *bucket = VECTOR_GET(&map->table, i, hashset_bucket);
                        if (bucket->in_use_flag) {
                                const void *data = vector_at(&map->key_data, bucket->key_idx);
                                vector_push(result, data, 1);
                        }
                }
                return result;
        } else {
                return NULL;
        }
}

hashset *hashset_cpy(hashset *src)
{
        if (src) {
                hashset *cpy = MALLOC(sizeof(hashset));

                hashset_lock(src);

                hashset_create(cpy, src->key_data.elem_size, src->table.cap_elems);

                assert(src->key_data.cap_elems == src->table.cap_elems);
                assert(src->key_data.num_elems <= src->table.num_elems);

                vector_cpy_to(&cpy->key_data, &src->key_data);
                vector_cpy_to(&cpy->table, &src->table);
                cpy->size = src->size;

                assert(cpy->key_data.cap_elems == cpy->table.cap_elems);
                assert(cpy->key_data.num_elems <= cpy->table.num_elems);

                hashset_unlock(src);
                return cpy;
        } else {
                error(ERR_NULLPTR, NULL);
                return NULL;
        }
}

bool hashset_clear(hashset *map)
{
        assert(map->key_data.cap_elems == map->table.cap_elems);
        assert(map->key_data.num_elems <= map->table.num_elems);

        hashset_lock(map);

        bool status = vector_clear(&map->key_data) && vector_zero_memory(&map->table);

        map->size = 0;

        assert(map->key_data.cap_elems == map->table.cap_elems);
        assert(map->key_data.num_elems <= map->table.num_elems);

        if (!status) {
                error(ERR_OPPFAILED, NULL);
        }

        hashset_unlock(map);

        return status;
}

bool hashset_avg_displace(float *displace, const hashset *map)
{
        size_t sum_dis = 0;
        for (size_t i = 0; i < map->table.num_elems; i++) {
                hashset_bucket *bucket = VECTOR_GET(&map->table, i, hashset_bucket);
                sum_dis += abs(bucket->displacement);
        }
        *displace = (sum_dis / (float) map->table.num_elems);

        return true;
}

bool hashset_lock(hashset *map)
{
        spinlock_acquire(&map->lock);
        return true;
}

bool hashset_unlock(hashset *map)
{
        spinlock_release(&map->lock);
        return true;
}

static inline const void *_hash_set_get_bucket_key(const hashset_bucket *bucket, const hashset *map)
{
        return map->key_data.base + bucket->key_idx * map->key_data.elem_size;
}

static void _hash_set_insert(hashset_bucket *bucket, hashset *map, const void *key, i32 displacement)
{
        u64 idx = map->key_data.num_elems;
        void *key_datum = VECTOR_NEW_AND_GET(&map->key_data, void *);
        memcpy(key_datum, key, map->key_data.elem_size);
        bucket->key_idx = idx;
        bucket->in_use_flag = true;
        bucket->displacement = displacement;
        map->size++;
}

static inline uint_fast32_t _hash_set_insert_or_update(hashset *map, const u32 *bucket_idxs, const void *keys,
                                             uint_fast32_t num_pairs)
{
        for (uint_fast32_t i = 0; i < num_pairs; i++) {
                const void *key = keys + i * map->key_data.elem_size;
                u32 intended_bucket_idx = bucket_idxs[i];

                u32 bucket_idx = intended_bucket_idx;

                hashset_bucket *bucket = VECTOR_GET(&map->table, bucket_idx, hashset_bucket);
                if (bucket->in_use_flag && memcmp(_hash_set_get_bucket_key(bucket, map), key, map->key_data.elem_size) != 0) {
                        bool fitting_bucket_found = false;
                        u32 displace_idx;
                        for (displace_idx = bucket_idx + 1; displace_idx < map->table.num_elems; displace_idx++) {
                                hashset_bucket
                                        *bucket = VECTOR_GET(&map->table, displace_idx, hashset_bucket);
                                fitting_bucket_found = !bucket->in_use_flag || (bucket->in_use_flag
                                                                                &&
                                                                                memcmp(_hash_set_get_bucket_key(bucket, map), key,
                                                                                       map->key_data.elem_size) == 0);
                                if (fitting_bucket_found) {
                                        break;
                                } else {
                                        i32 displacement = displace_idx - bucket_idx;
                                        const void *swap_key = _hash_set_get_bucket_key(bucket, map);

                                        if (bucket->displacement < displacement) {
                                                _hash_set_insert(bucket, map, key, displacement);
                                                _hash_set_insert_or_update(map, &displace_idx, swap_key, 1);
                                                goto next_round;
                                        }
                                }
                        }
                        if (!fitting_bucket_found) {
                                for (displace_idx = 0; displace_idx < bucket_idx - 1; displace_idx++) {
                                        const hashset_bucket
                                                *bucket = VECTOR_GET(&map->table, displace_idx, hashset_bucket);
                                        fitting_bucket_found = !bucket->in_use_flag || (bucket->in_use_flag
                                                                                        && memcmp(_hash_set_get_bucket_key(bucket,
                                                                                                                 map),
                                                                                                  key,
                                                                                                  map->key_data.elem_size)
                                                                                           == 0);
                                        if (fitting_bucket_found) {
                                                break;
                                        }
                                }
                        }

                        assert(fitting_bucket_found == true);
                        bucket_idx = displace_idx;
                        bucket = VECTOR_GET(&map->table, bucket_idx, hashset_bucket);
                }

                bool is_update =
                        bucket->in_use_flag && memcmp(_hash_set_get_bucket_key(bucket, map), key, map->key_data.elem_size) == 0;
                if (!is_update) {
                        i32 displacement = intended_bucket_idx - bucket_idx;
                        _hash_set_insert(bucket, map, key, displacement);
                }

                next_round:
                if (map->size >= FIX_MAP_AUTO_REHASH_LOADFACTOR * map->table.cap_elems) {
                        return i + 1; /** tell the caller that pair i was inserted, but it successors not */
                }

        }
        return 0;
}

bool hashset_insert_or_update(hashset *map, const void *keys, uint_fast32_t num_pairs)
{
        assert(map->key_data.cap_elems == map->table.cap_elems);
        assert(map->key_data.num_elems <= map->table.num_elems);

        hashset_lock(map);

        u32 *bucket_idxs = MALLOC(num_pairs * sizeof(u32));
        if (!bucket_idxs) {
                error(ERR_MALLOCERR, NULL);
                return false;
        }

        for (uint_fast32_t i = 0; i < num_pairs; i++) {
                const void *key = keys + i * map->key_data.elem_size;
                hash32_t hash = _HASH_SET_HASHCODE_OF(map->key_data.elem_size, key);
                bucket_idxs[i] = hash % map->table.num_elems;
        }

        uint_fast32_t cont_idx = 0;
        do {
                cont_idx = _hash_set_insert_or_update(map,
                                            bucket_idxs + cont_idx,
                                            keys + cont_idx * map->key_data.elem_size,
                                            num_pairs - cont_idx);
                if (cont_idx != 0) {
                        /** rehashing is required, and [status, num_pairs) are left to be inserted */
                        if (!hashset_rehash(map)) {
                                hashset_unlock(map);
                                return false;
                        }
                }
        } while (cont_idx != 0);

        free(bucket_idxs);
        hashset_unlock(map);

        return true;
}

bool hashset_remove_if_contained(hashset *map, const void *keys, size_t num_pairs)
{
        hashset_lock(map);

        u32 *bucket_idxs = MALLOC(num_pairs * sizeof(u32));
        if (!bucket_idxs) {
                error(ERR_MALLOCERR, NULL);
                hashset_unlock(map);
                return false;
        }

        for (uint_fast32_t i = 0; i < num_pairs; i++) {
                const void *key = keys + i * map->key_data.elem_size;
                bucket_idxs[i] = _HASH_SET_HASHCODE_OF(map->key_data.elem_size, key) % map->table.num_elems;
        }

        for (uint_fast32_t i = 0; i < num_pairs; i++) {
                const void *key = keys + i * map->key_data.elem_size;
                u32 bucket_idx = bucket_idxs[i];
                u32 actual_idx = bucket_idx;
                bool bucket_found = false;

                for (u32 k = bucket_idx; !bucket_found && k < map->table.num_elems; k++) {
                        const hashset_bucket *bucket = VECTOR_GET(&map->table, k, hashset_bucket);
                        bucket_found = bucket->in_use_flag
                                       && memcmp(_hash_set_get_bucket_key(bucket, map), key, map->key_data.elem_size) == 0;
                        actual_idx = k;
                }
                for (u32 k = 0; !bucket_found && k < bucket_idx; k++) {
                        const hashset_bucket *bucket = VECTOR_GET(&map->table, k, hashset_bucket);
                        bucket_found = bucket->in_use_flag
                                       && memcmp(_hash_set_get_bucket_key(bucket, map), key, map->key_data.elem_size) == 0;
                        actual_idx = k;
                }

                if (bucket_found) {
                        hashset_bucket *bucket = VECTOR_GET(&map->table, actual_idx, hashset_bucket);
                        bucket->in_use_flag = false;
                        bucket->key_idx = 0;
                }
        }

        free(bucket_idxs);

        hashset_unlock(map);

        return true;
}

bool hashset_contains_key(hashset *map, const void *key)
{
        bool result = false;

        hashset_lock(map);

        u32 bucket_idx = _HASH_SET_HASHCODE_OF(map->key_data.elem_size, key) % map->table.num_elems;
        bool bucket_found = false;

        for (u32 k = bucket_idx; !bucket_found && k < map->table.num_elems; k++) {
                const hashset_bucket *bucket = VECTOR_GET(&map->table, k, hashset_bucket);
                bucket_found =
                        bucket->in_use_flag && memcmp(_hash_set_get_bucket_key(bucket, map), key, map->key_data.elem_size) == 0;
        }
        for (u32 k = 0; !bucket_found && k < bucket_idx; k++) {
                const hashset_bucket *bucket = VECTOR_GET(&map->table, k, hashset_bucket);
                bucket_found =
                        bucket->in_use_flag && memcmp(_hash_set_get_bucket_key(bucket, map), key, map->key_data.elem_size) == 0;
        }

        result = bucket_found;
        hashset_unlock(map);

        return result;
}

bool hashset_get_load_factor(float *factor, hashset *map)
{
        hashset_lock(map);

        *factor = map->size / (float) map->table.num_elems;

        hashset_unlock(map);

        return true;
}

bool hashset_rehash(hashset *map)
{
        hashset_lock(map);

        hashset *cpy = hashset_cpy(map);
        hashset_clear(map);

        size_t new_cap = (cpy->key_data.cap_elems + 1) * 1.7f;

        vector_grow_to(&map->key_data, new_cap);
        vector_grow_to(&map->table, new_cap);
        vector_enlarge_size_to_capacity(&map->table);
        vector_zero_memory(&map->table);

        assert(map->key_data.cap_elems == map->table.cap_elems);
        assert(map->key_data.num_elems <= map->table.num_elems);

        for (size_t i = 0; i < cpy->table.num_elems; i++) {
                hashset_bucket *bucket = VECTOR_GET(&cpy->table, i, hashset_bucket);
                if (bucket->in_use_flag) {
                        const void *old_key = _hash_set_get_bucket_key(bucket, cpy);
                        if (!hashset_insert_or_update(map, old_key, 1)) {
                                hashset_unlock(map);
                                return error(ERR_REHASH_NOROLLBACK, NULL);
                        }
                }
        }

        hashset_unlock(map);
        return true;
}