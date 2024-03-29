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

#include <karbonit/std/str_hash/mem.h>
#include <karbonit/std/spinlock.h>
#include <karbonit/utils/sort.h>
#include <karbonit/utils/time.h>
#include <karbonit/std/bloom.h>
#include <karbonit/stdx/slicelist.h>
#include <karbonit/std/hash.h>

#define STR_HASH_MEM_HASHCODE_OF(key)      HASH_BERNSTEIN(strlen(key), key)

#define SMART_MAP_TAG "strhash-mem"

struct bucket {
        slice_list_t slice_list;
};

struct mem_extra {
        vec ofType(bucket) buckets;
};

static int _str_hash_mem_drop(str_hash *self);

static int this_put_safe_bulk(str_hash *self, char *const *keys, const archive_field_sid_t *values,
                              size_t num_pairs);

static int this_put_fast_bulk(str_hash *self, char *const *keys, const archive_field_sid_t *values,
                              size_t num_pairs);

static int this_put_safe_exact(str_hash *self, const char *key, archive_field_sid_t value);

static int this_put_fast_exact(str_hash *self, const char *key, archive_field_sid_t value);

static int
this_get_safe(str_hash *self, archive_field_sid_t **out, bool **found_mask, size_t *num_not_found,
              char *const *keys, size_t num_keys);

static int
this_get_safe_exact(str_hash *self, archive_field_sid_t *out, bool *found_mask, const char *key);

static int this_get_fast(str_hash *self, archive_field_sid_t **out, char *const *keys, size_t num_keys);

static int this_update_key_fast(str_hash *self, const archive_field_sid_t *values, char *const *keys,
                                size_t num_keys);

static int _str_hash_mem_remove(str_hash *self, char *const *keys, size_t num_keys);

static int _str_hash_mem_free(str_hash *self, void *ptr);

static int this_insert_bulk(vec ofType(bucket) *buckets, char *const *restrict keys,
                            const archive_field_sid_t *restrict values, size_t *restrict bucket_idxs,
                            size_t num_pairs,
                            str_hash_counters *counter);

static int
this_insert_exact(vec ofType(bucket) *buckets, const char *restrict key, archive_field_sid_t value,
                  size_t bucket_idx, str_hash_counters *counter);

static int
this_fetch_bulk(vec ofType(bucket) *buckets, archive_field_sid_t *values_out, bool *key_found_mask,
                size_t *num_keys_not_found, size_t *bucket_idxs, char *const *keys, size_t num_keys,
                str_hash_counters *counter);

static int
this_fetch_single(vec ofType(bucket) *buckets, archive_field_sid_t *value_out, bool *key_found,
                  const size_t bucket_idx, const char *key, str_hash_counters *counter);

static int _str_hash_mem_create_extra(str_hash *self, size_t num_buckets, size_t cap_buckets);

static struct mem_extra *this_get_exta(str_hash *self);

static int bucket_create(struct bucket *buckets, size_t num_buckets, size_t bucket_cap);

static int bucket_drop(struct bucket *buckets, size_t num_buckets);

static int bucket_insert(struct bucket *bucket, const char *restrict key, archive_field_sid_t value,
                         str_hash_counters *counter);

bool
str_hash_create_inmemory(str_hash *str_hash, size_t num_buckets, size_t cap_buckets)
{
        num_buckets = num_buckets < 1 ? 1 : num_buckets;
        cap_buckets = cap_buckets < 1 ? 1 : cap_buckets;

        str_hash->tag = MEMORY_RESIDENT;
        str_hash->drop = _str_hash_mem_drop;
        str_hash->put_bulk_safe = this_put_safe_bulk;
        str_hash->put_bulk_fast = this_put_fast_bulk;
        str_hash->put_exact_safe = this_put_safe_exact;
        str_hash->put_exact_fast = this_put_fast_exact;
        str_hash->get_bulk_safe = this_get_safe;
        str_hash->get_fast = this_get_fast;
        str_hash->update_key_fast = this_update_key_fast;
        str_hash->remove = _str_hash_mem_remove;
        str_hash->free = _str_hash_mem_free;
        str_hash->get_exact_safe = this_get_safe_exact;

        str_hash_reset_counters(str_hash);
        CHECK_SUCCESS(_str_hash_mem_create_extra(str_hash, num_buckets, cap_buckets));
        return true;
}

static int _str_hash_mem_drop(str_hash *self)
{
        assert(self->tag == MEMORY_RESIDENT);
        struct mem_extra *extra = this_get_exta(self);
        struct bucket *data = (struct bucket *) vec_data(&extra->buckets);
        CHECK_SUCCESS(bucket_drop(data, extra->buckets.cap_elems));
        vec_drop(&extra->buckets);
        free(self->extra);
        return true;
}

static int this_put_safe_bulk(str_hash *self, char *const *keys, const archive_field_sid_t *values,
                              size_t num_pairs)
{
        assert(self->tag == MEMORY_RESIDENT);
        struct mem_extra *extra = this_get_exta(self);
        size_t *bucket_idxs = MALLOC(num_pairs * sizeof(size_t));

        PREFETCH_WRITE(bucket_idxs);

        for (size_t i = 0; i < num_pairs; i++) {
                const char *key = keys[i];
                hash32_t hash = STR_HASH_MEM_HASHCODE_OF(key);
                bucket_idxs[i] = hash % extra->buckets.cap_elems;
        }

        PREFETCH_READ(bucket_idxs);
        PREFETCH_READ(keys);
        PREFETCH_READ(values);

        CHECK_SUCCESS(this_insert_bulk(&extra->buckets,
                                           keys,
                                           values,
                                           bucket_idxs,
                                           num_pairs,
                                           &self->counters));
        free(bucket_idxs);
        return true;
}

static int this_put_safe_exact(str_hash *self, const char *key, archive_field_sid_t value)
{
        assert(self->tag == MEMORY_RESIDENT);
        struct mem_extra *extra = this_get_exta(self);

        hash32_t hash = strcmp("", key) != 0 ? STR_HASH_MEM_HASHCODE_OF(key) : 0;
        size_t bucket_idx = hash % extra->buckets.cap_elems;

        PREFETCH_READ(key);

        CHECK_SUCCESS(this_insert_exact(&extra->buckets,
                                            key,
                                            value,
                                            bucket_idx,
                                            &self->counters));

        return true;
}

static int this_put_fast_exact(str_hash *self, const char *key, archive_field_sid_t value)
{
        return this_put_safe_exact(self, key, value);
}

static int this_put_fast_bulk(str_hash *self, char *const *keys, const archive_field_sid_t *values,
                              size_t num_pairs)
{
        return this_put_safe_bulk(self, keys, values, num_pairs);
}

static int
this_fetch_bulk(vec ofType(bucket) *buckets, archive_field_sid_t *values_out, bool *key_found_mask,
                size_t *num_keys_not_found, size_t *bucket_idxs, char *const *keys, size_t num_keys,
                str_hash_counters *counter)
{
        UNUSED(counter)

        slice_handle result_handle;
        size_t num_not_found = 0;
        struct bucket *data = (struct bucket *) vec_data(buckets);

        PREFETCH_WRITE(values_out);

        for (size_t i = 0; i < num_keys; i++) {
                struct bucket *bucket = data + bucket_idxs[i];
                const char *key = keys[i];
                if (LIKELY(key != NULL)) {
                        slice_list_lookup(&result_handle, &bucket->slice_list, key);
                } else {
                        result_handle.is_contained = true;
                        result_handle.value = NULL_ENCODED_STRING;
                }

                num_not_found += result_handle.is_contained ? 0 : 1;
                key_found_mask[i] = result_handle.is_contained;
                values_out[i] = result_handle.is_contained ? result_handle.value : ((archive_field_sid_t) -1);
        }

        *num_keys_not_found = num_not_found;
        return true;
}

static int
this_fetch_single(vec ofType(bucket) *buckets, archive_field_sid_t *value_out, bool *key_found,
                  const size_t bucket_idx, const char *key, str_hash_counters *counter)
{
        UNUSED(counter);

        slice_handle handle;
        struct bucket *data = (struct bucket *) vec_data(buckets);

        PREFETCH_WRITE(value_out);
        PREFETCH_WRITE(key_found);

        struct bucket *bucket = data + bucket_idx;

        /** Optimization 1/5: EMPTY GUARD (but before "find" call); if this bucket has no occupied slots, do not perform any lookup and comparison */
        slice_list_lookup(&handle, &bucket->slice_list, key);
        *key_found = !slice_list_is_empty(&bucket->slice_list) && handle.is_contained;
        *value_out = (*key_found) ? handle.value : ((archive_field_sid_t) -1);

        return true;
}

static int
this_get_safe(str_hash *self, archive_field_sid_t **out, bool **found_mask, size_t *num_not_found,
              char *const *keys, size_t num_keys)
{
        assert(self->tag == MEMORY_RESIDENT);

        timestamp begin = wallclock();
        TRACE(SMART_MAP_TAG, "'get_safe' function invoked for %zu strings", num_keys)

        struct mem_extra *extra = this_get_exta(self);
        size_t *bucket_idxs = MALLOC(num_keys * sizeof(size_t));
        archive_field_sid_t *values_out = MALLOC(num_keys * sizeof(archive_field_sid_t));
        bool *found_mask_out = MALLOC(num_keys * sizeof(bool));

        assert(bucket_idxs != NULL);
        assert(values_out != NULL);
        assert(found_mask_out != NULL);

        for (register size_t i = 0; i < num_keys; i++) {
                const char *key = keys[i];
                hash32_t hash = key && strcmp("", key) != 0 ? STR_HASH_MEM_HASHCODE_OF(key) : 0;
                bucket_idxs[i] = hash % extra->buckets.cap_elems;
                PREFETCH_READ((struct bucket *) vec_data(&extra->buckets) + bucket_idxs[i]);
        }

        TRACE(SMART_MAP_TAG, "'get_safe' function invoke fetch...for %zu strings", num_keys)
        CHECK_SUCCESS(this_fetch_bulk(&extra->buckets,
                                          values_out,
                                          found_mask_out,
                                          num_not_found,
                                          bucket_idxs,
                                          keys,
                                          num_keys,
                                          &self->counters));
        free(bucket_idxs);
        TRACE(SMART_MAP_TAG, "'get_safe' function invok fetch: done for %zu strings", num_keys)

        assert(values_out != NULL);
        assert(found_mask_out != NULL);

        *out = values_out;
        *found_mask = found_mask_out;

        timestamp end = wallclock();
        UNUSED(begin);
        UNUSED(end);
        TRACE(SMART_MAP_TAG, "'get_safe' function done: %f seconds spent here", (end - begin) / 1000.0f)

        return true;
}

static int
this_get_safe_exact(str_hash *self, archive_field_sid_t *out, bool *found_mask, const char *key)
{
        assert(self->tag == MEMORY_RESIDENT);

        struct mem_extra *extra = this_get_exta(self);

        hash32_t hash = strcmp("", key) != 0 ? STR_HASH_MEM_HASHCODE_OF(key) : 0;
        size_t bucket_idx = hash % extra->buckets.cap_elems;
        PREFETCH_READ((struct bucket *) vec_data(&extra->buckets) + bucket_idx);

        CHECK_SUCCESS(this_fetch_single(&extra->buckets, out, found_mask, bucket_idx, key, &self->counters));

        return true;
}

static int this_get_fast(str_hash *self, archive_field_sid_t **out, char *const *keys, size_t num_keys)
{
        bool *found_mask;
        size_t num_not_found;
        int status = this_get_safe(self, out, &found_mask, &num_not_found, keys, num_keys);
        _str_hash_mem_free(self, found_mask);
        return status;
}

static int this_update_key_fast(str_hash *self, const archive_field_sid_t *values, char *const *keys,
                                size_t num_keys)
{
        UNUSED(self);
        UNUSED(values);
        UNUSED(keys);
        UNUSED(num_keys);
        ERROR(ERR_NOTIMPL, NULL);
        return false;
}

static int simple_map_remove(struct mem_extra *extra, size_t *bucket_idxs, char *const *keys, size_t num_keys,
                             str_hash_counters *counter)
{
        UNUSED(counter)

        slice_handle handle;
        struct bucket *data = (struct bucket *) vec_data(&extra->buckets);

        for (register size_t i = 0; i < num_keys; i++) {
                struct bucket *bucket = data + bucket_idxs[i];
                const char *key = keys[i];

                /** Optimization 1/5: EMPTY GUARD (but before "find" call); if this bucket has no occupied slots, do not perform any lookup and comparison */
                slice_list_lookup(&handle, &bucket->slice_list, key);
                if (LIKELY(handle.is_contained)) {
                        slice_list_remove(&bucket->slice_list, &handle);
                }
        }
        return true;
}

static int _str_hash_mem_remove(str_hash *self, char *const *keys, size_t num_keys)
{
        assert(self->tag == MEMORY_RESIDENT);

        struct mem_extra *extra = this_get_exta(self);
        size_t *bucket_idxs = MALLOC(num_keys * sizeof(size_t));
        for (register size_t i = 0; i < num_keys; i++) {
                const char *key = keys[i];
                hash32_t hash = STR_HASH_MEM_HASHCODE_OF(key);
                bucket_idxs[i] = hash % extra->buckets.cap_elems;
        }

        CHECK_SUCCESS(simple_map_remove(extra, bucket_idxs, keys, num_keys, &self->counters));
        free(bucket_idxs);
        return true;
}

static int _str_hash_mem_free(str_hash *self, void *ptr)
{
        UNUSED(self)
        assert(self->tag == MEMORY_RESIDENT);
        free(ptr);
        return true;
}

MAYBE_UNUSED
static int _str_hash_mem_create_extra(str_hash *self, size_t num_buckets, size_t cap_buckets)
{
        if ((self->extra = MALLOC(sizeof(struct mem_extra))) != NULL) {
                struct mem_extra *extra = this_get_exta(self);
                vec_create(&extra->buckets, sizeof(struct bucket), num_buckets);

                /** Optimization: notify the kernel that the list of buckets are accessed randomly (since hash based access)*/
                vec_madvise(&extra->buckets, MADV_RANDOM | MADV_WILLNEED);

                struct bucket *data = (struct bucket *) vec_data(&extra->buckets);
                CHECK_SUCCESS(bucket_create(data, num_buckets, cap_buckets));
                return true;
        } else {
                ERROR(ERR_MALLOCERR, NULL);
                return false;
        }
}

MAYBE_UNUSED
static struct mem_extra *this_get_exta(str_hash *self)
{
        assert (self->tag == MEMORY_RESIDENT);
        return (struct mem_extra *) (self->extra);
}

MAYBE_UNUSED
static int bucket_create(struct bucket *buckets, size_t num_buckets, size_t bucket_cap)
{
        // TODO: parallize this!
        while (num_buckets--) {
                struct bucket *bucket = buckets++;
                slice_list_create(&bucket->slice_list, bucket_cap);
        }

        return true;
}

static int bucket_drop(struct bucket *buckets, size_t num_buckets)
{
        while (num_buckets--) {
                struct bucket *bucket = buckets++;
                slice_list_drop(&bucket->slice_list);
        }

        return true;
}

static int bucket_insert(struct bucket *bucket, const char *restrict key, archive_field_sid_t value,
                         str_hash_counters *counter)
{
        UNUSED(counter);

        slice_handle handle;

        /** Optimization 1/5: EMPTY GUARD (but before "find" call); if this bucket has no occupied slots, do not perform any lookup and comparison */
        slice_list_lookup(&handle, &bucket->slice_list, key);

        if (handle.is_contained) {
                /** entry found by keys */
                assert(value == handle.value);
                //debug(SMART_MAP_TAG, "debug(SMART_MAP_TAG, \"*** put *** '%s' into bucket [new]\", keys);*** put *** '%s' into bucket [already contained]", keys);
        } else {
                /** no entry found */
                //debug(SMART_MAP_TAG, "*** put *** '%s' into bucket [new]", keys);
                slice_list_insert(&bucket->slice_list, (char **) &key, &value, 1);
        }

        return true;
}

static int this_insert_bulk(vec ofType(bucket) *buckets, char *const *restrict keys,
                            const archive_field_sid_t *restrict values, size_t *restrict bucket_idxs,
                            size_t num_pairs, str_hash_counters *counter)
{
        struct bucket *buckets_data = (struct bucket *) vec_data(buckets);
        int status = true;
        for (register size_t i = 0; status == true && i < num_pairs; i++) {
                size_t bucket_idx = bucket_idxs[i];
                const char *key = keys[i];
                archive_field_sid_t value = values[i];

                struct bucket *bucket = buckets_data + bucket_idx;
                status = bucket_insert(bucket, key, value, counter);
        }

        return status;
}

static int
this_insert_exact(vec ofType(bucket) *buckets, const char *restrict key, archive_field_sid_t value,
                  size_t bucket_idx, str_hash_counters *counter)
{
        struct bucket *buckets_data = (struct bucket *) vec_data(buckets);
        struct bucket *bucket = buckets_data + bucket_idx;
        return bucket_insert(bucket, key, value, counter);
}
