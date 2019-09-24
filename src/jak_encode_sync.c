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

#include <jak_vector.h>
#include <jak_spinlock.h>
#include <jak_str_hash.h>
#include <jak_encode_sync.h>
#include <jak_str_hash_mem.h>
#include <jak_time.h>
#include <jak_bloom.h>
#include <jak_hash.h>

#define STRING_DIC_SYNC_TAG "string-dic-sync"

struct entry {
        char *str;
        bool in_use;
};

struct sync_extra {
        jak_vector ofType(entry) contents;
        jak_vector ofType(jak_string_id_t_t) freelist;
        jak_str_hash index;
        jak_spinlock _jak_encode_sync_lock;
};

static bool _jak_encode_sync_drop(jak_string_dict *self);

static bool
_jak_encode_sync_insert(jak_string_dict *self, jak_archive_field_sid_t **out, char *const *strings, size_t num_strings,
            size_t num_threads);

static bool _jak_encode_sync_remove(jak_string_dict *self, jak_archive_field_sid_t *strings, size_t num_strings);

static bool
_jak_encode_sync_locate_safe(jak_string_dict *self, jak_archive_field_sid_t **out, bool **found_mask, size_t *num_not_found,
                 char *const *keys, size_t num_keys);

static bool
_jak_encode_sync_locate_fast(jak_string_dict *self, jak_archive_field_sid_t **out, char *const *keys, size_t num_keys);

static char **_jak_encode_sync_extract(jak_string_dict *self, const jak_archive_field_sid_t *ids, size_t num_ids);

static bool _jak_encode_sync_free(jak_string_dict *self, void *ptr);

static bool _jak_encode_sync_reset_counters(jak_string_dict *self);

static bool _jak_encode_sync_counters(jak_string_dict *self, jak_str_hash_counters *counters);

static bool _jak_encode_sync_num_distinct(jak_string_dict *self, size_t *num);

static bool _jak_encode_sync_get_contents(jak_string_dict *self, jak_vector ofType (char *) *strings,
                              jak_vector ofType(jak_archive_field_sid_t) *jak_string_ids);

static void _jak_encode_sync_lock(jak_string_dict *self);

static void _jak_encode_sync_unlock(jak_string_dict *self);

static int
create_extra(jak_string_dict *self, size_t capacity, size_t num_index_buckets, size_t num_index_bucket_cap,
             size_t num_threads);

static struct sync_extra *this_extra(jak_string_dict *self);

static int freelist_pop(jak_archive_field_sid_t *out, jak_string_dict *self);

static int freelist_push(jak_string_dict *self, jak_archive_field_sid_t idx);

int
jak_encode_sync_create(jak_string_dict *dic, size_t capacity, size_t num_indx_buckets, size_t num_index_bucket_cap,
                   size_t num_threads, const jak_allocator *alloc)
{
        JAK_ERROR_IF_NULL(dic);

        JAK_CHECK_SUCCESS(jak_alloc_this_or_std(&dic->alloc, alloc));

        dic->tag = JAK_SYNC;
        dic->drop = _jak_encode_sync_drop;
        dic->insert = _jak_encode_sync_insert;
        dic->remove = _jak_encode_sync_remove;
        dic->locate_safe = _jak_encode_sync_locate_safe;
        dic->locate_fast = _jak_encode_sync_locate_fast;
        dic->extract = _jak_encode_sync_extract;
        dic->free = _jak_encode_sync_free;
        dic->resetCounters = _jak_encode_sync_reset_counters;
        dic->counters = _jak_encode_sync_counters;
        dic->num_distinct = _jak_encode_sync_num_distinct;
        dic->get_contents = _jak_encode_sync_get_contents;

        JAK_CHECK_SUCCESS(create_extra(dic, capacity, num_indx_buckets, num_index_bucket_cap, num_threads));
        return true;
}

static void _jak_encode_sync_lock(jak_string_dict *self)
{
        JAK_ASSERT(self->tag == JAK_SYNC);
        struct sync_extra *extra = this_extra(self);
        jak_spinlock_acquire(&extra->_jak_encode_sync_lock);
}

static void _jak_encode_sync_unlock(jak_string_dict *self)
{
        JAK_ASSERT(self->tag == JAK_SYNC);
        struct sync_extra *extra = this_extra(self);
        jak_spinlock_release(&extra->_jak_encode_sync_lock);
}

static int
create_extra(jak_string_dict *self, size_t capacity, size_t num_index_buckets, size_t num_index_bucket_cap,
             size_t num_threads)
{
        self->extra = jak_alloc_malloc(&self->alloc, sizeof(struct sync_extra));
        struct sync_extra *extra = this_extra(self);
        jak_spinlock_init(&extra->_jak_encode_sync_lock);
        JAK_CHECK_SUCCESS(jak_vector_create(&extra->contents, &self->alloc, sizeof(struct entry), capacity));
        JAK_CHECK_SUCCESS(jak_vector_create(&extra->freelist, &self->alloc, sizeof(jak_archive_field_sid_t), capacity));
        struct entry empty = {.str    = NULL, .in_use = false};
        for (size_t i = 0; i < capacity; i++) {
                JAK_CHECK_SUCCESS(jak_vector_push(&extra->contents, &empty, 1));
                freelist_push(self, i);
        }
        JAK_UNUSED(num_threads);

        jak_allocator jak_hashtable_alloc;
#if defined(JAK_CONFIG_TRACE_STRING_DIC_ALLOC) && !defined(NDEBUG)
        CHECK_SUCCESS(allocatorTrace(&jak_hashtable_alloc));
#else
        JAK_CHECK_SUCCESS(jak_alloc_this_or_std(&jak_hashtable_alloc, &self->alloc));
#endif

        JAK_CHECK_SUCCESS(jak_str_hash_create_inmemory(&extra->index,
                                                  &jak_hashtable_alloc,
                                                  num_index_buckets,
                                                  num_index_bucket_cap));
        return true;
}

static struct sync_extra *this_extra(jak_string_dict *self)
{
        JAK_ASSERT (self->tag == JAK_SYNC);
        return (struct sync_extra *) self->extra;
}

static int freelist_pop(jak_archive_field_sid_t *out, jak_string_dict *self)
{
        JAK_ASSERT (self->tag == JAK_SYNC);
        struct sync_extra *extra = this_extra(self);
        if (JAK_UNLIKELY(jak_vector_is_empty(&extra->freelist))) {
                size_t num_new_pos;
                JAK_CHECK_SUCCESS(jak_vector_grow(&num_new_pos, &extra->freelist));
                JAK_CHECK_SUCCESS(jak_vector_grow(NULL, &extra->contents));
                JAK_ASSERT (extra->freelist.cap_elems == extra->contents.cap_elems);
                struct entry empty = {.in_use = false, .str    = NULL};
                while (num_new_pos--) {
                        size_t new_pos = jak_vector_length(&extra->contents);
                        JAK_CHECK_SUCCESS(jak_vector_push(&extra->freelist, &new_pos, 1));
                        JAK_CHECK_SUCCESS(jak_vector_push(&extra->contents, &empty, 1));
                }
        }
        *out = *(jak_archive_field_sid_t *) jak_vector_pop(&extra->freelist);
        return true;
}

static int freelist_push(jak_string_dict *self, jak_archive_field_sid_t idx)
{
        JAK_ASSERT (self->tag == JAK_SYNC);
        struct sync_extra *extra = this_extra(self);
        JAK_CHECK_SUCCESS(jak_vector_push(&extra->freelist, &idx, 1));
        JAK_ASSERT (extra->freelist.cap_elems == extra->contents.cap_elems);
        return true;
}

static bool _jak_encode_sync_drop(jak_string_dict *self)
{
        JAK_CHECK_TAG(self->tag, JAK_SYNC)

        struct sync_extra *extra = this_extra(self);

        struct entry *entries = (struct entry *) extra->contents.base;
        for (size_t i = 0; i < extra->contents.num_elems; i++) {
                struct entry *entry = entries + i;
                if (entry->in_use) {
                        JAK_ASSERT (entry->str);
                        jak_alloc_free(&self->alloc, entry->str);
                        entry->str = NULL;
                }
        }

        jak_vector_drop(&extra->freelist);
        jak_vector_drop(&extra->contents);
        jak_str_hash_drop(&extra->index);
        jak_alloc_free(&self->alloc, self->extra);

        return true;
}

static bool
_jak_encode_sync_insert(jak_string_dict *self, jak_archive_field_sid_t **out, char *const *strings, size_t num_strings,
            size_t num_threads)
{
        JAK_TRACE(STRING_DIC_SYNC_TAG, "local string dictionary insertion invoked for %zu strings", num_strings);
        jak_timestamp begin = jak_wallclock();

        JAK_UNUSED(num_threads);

        JAK_CHECK_TAG(self->tag, JAK_SYNC)
        _jak_encode_sync_lock(self);

        struct sync_extra *extra = this_extra(self);

        jak_allocator jak_hashtable_alloc;
#if defined(JAK_CONFIG_TRACE_STRING_DIC_ALLOC) && !defined(NDEBUG)
        CHECK_SUCCESS(allocatorTrace(&jak_hashtable_alloc));
#else
        JAK_CHECK_SUCCESS(jak_alloc_this_or_std(&jak_hashtable_alloc, &self->alloc));
#endif

        jak_archive_field_sid_t *ids_out = jak_alloc_malloc(&jak_hashtable_alloc,
                                                            num_strings * sizeof(jak_archive_field_sid_t));
        bool *found_mask;
        jak_archive_field_sid_t *values;
        size_t num_not_found;

        /** query index for strings to get a boolean mask which strings are new and which must be added */
        /** This is for the case that the string dictionary is not empty to skip processing of those new elements
         * which are already contained */
        JAK_TRACE(STRING_DIC_SYNC_TAG, "local string dictionary check for new strings in insertion bulk%s", "...");

        /** NOTE: palatalization of the call to this function decreases performance */
        jak_str_hash_get_bulk_safe(&values, &found_mask, &num_not_found, &extra->index, strings, num_strings);

        /** OPTIMIZATION: use a jak_bitmap to check whether a string (which has not appeared in the
         * dictionary before this batch but might occur multiple times in the current batch) was seen
         * before (with a slight prob. of doing too much work) */
        jak_bitmap bitmap;
        jak_bloom_create(&bitmap, 22 * num_not_found);

        /** copy string ids for already known strings to their result position resp. add those which are new */
        for (size_t i = 0; i < num_strings; i++) {

                if (found_mask[i]) {
                        ids_out[i] = values[i];
                } else {
                        /** This path is taken only for strings that are not already contained in the dictionary. However,
                         * since this insertion batch may contain duplicate string, querying for already inserted strings
                         * must be done anyway for each string in the insertion batch that is inserted. */

                        jak_archive_field_sid_t jak_string_id = 0;
                        const char *key = (const char *) (strings[i]);

                        bool found = false;
                        jak_archive_field_sid_t value;

                        /** Query the jak_bitmap if the keys was already seend. If the filter returns "yes", a lookup
                         * is requried since the filter maybe made a mistake. Of the filter returns "no", the
                         * keys is new for sure. In this case, one can skip the lookup into the buckets. */
                        size_t key_length = strlen(key);
                        hash32_t bloom_key = key_length > 0 ? JAK_HASH_FNV(strlen(key), key)
                                                            : 0; /** using a hash of a keys instead of the string keys itself avoids reading the entire string for computing k hashes inside the jak_bitmap */
                        if (JAK_BLOOM_TEST_AND_SET(&bitmap, &bloom_key, sizeof(hash32_t))) {
                                /** ensure that the string really was seen (due to collisions in the bloom filter the keys might not
                                 * been actually seen) */

                                /** query index for strings to get a boolean mask which strings are new and which must be added */
                                /** This is for the case that the string was not already contained in the string dictionary but may have
                                 * duplicates in this insertion batch that are already inserted */
                                jak_str_hash_get_bulk_safe_exact(&value,
                                                            &found,
                                                            &extra->index,
                                                            key);  /** OPTIMIZATION: use specialized function for "exact" query to avoid unnessecary malloc calls to manage set of results if only a single result is needed */
                        }

                        if (found) {
                                ids_out[i] = value;
                        } else {

                                /** register in contents list */
                                bool pop_result = freelist_pop(&jak_string_id, self);
                                JAK_ERROR_PRINT_AND_DIE_IF(!pop_result, JAK_ERR_SLOTBROKEN)
                                struct entry *entries = (struct entry *) jak_vector_data(&extra->contents);
                                struct entry *entry = entries + jak_string_id;
                                JAK_ASSERT (!entry->in_use);
                                entry->in_use = true;
                                entry->str = strdup(strings[i]);
                                ids_out[i] = jak_string_id;

                                /** add for not yet registered pairs to buffer for fast import */
                                jak_str_hash_put_exact_fast(&extra->index, entry->str, jak_string_id);
                        }
                }
        }

        /** set potential non-null out parameters */
        JAK_OPTIONAL_SET_OR_ELSE(out, ids_out, jak_alloc_free(&self->alloc, ids_out));

        /** cleanup */
        jak_alloc_free(&jak_hashtable_alloc, found_mask);
        jak_alloc_free(&jak_hashtable_alloc, values);
        jak_bloom_drop(&bitmap);

        _jak_encode_sync_unlock(self);

        jak_timestamp end = jak_wallclock();
        JAK_UNUSED(begin);
        JAK_UNUSED(end);
        JAK_INFO(STRING_DIC_SYNC_TAG, "insertion operation done: %f seconds spent here", (end - begin) / 1000.0f)

        return true;

}

static bool _jak_encode_sync_remove(jak_string_dict *self, jak_archive_field_sid_t *strings, size_t num_strings)
{
        JAK_ERROR_IF_NULL(self);
        JAK_ERROR_IF_NULL(strings);
        JAK_ERROR_IF_NULL(num_strings);
        JAK_CHECK_TAG(self->tag, JAK_SYNC)
        _jak_encode_sync_lock(self);

        struct sync_extra *extra = this_extra(self);

        size_t num_strings_to_delete = 0;
        char **jak_string_to_delete = jak_alloc_malloc(&self->alloc, num_strings * sizeof(char *));
        jak_archive_field_sid_t *jak_string_ids_to_delete = jak_alloc_malloc(&self->alloc,
                                                                         num_strings * sizeof(jak_archive_field_sid_t));

        /** remove strings from contents JAK_vector, and skip duplicates */
        for (size_t i = 0; i < num_strings; i++) {
                jak_archive_field_sid_t jak_archive_field_sid_t = strings[i];
                struct entry *entry = (struct entry *) jak_vector_data(&extra->contents) + jak_archive_field_sid_t;
                if (JAK_LIKELY(entry->in_use)) {
                        jak_string_to_delete[num_strings_to_delete] = entry->str;
                        jak_string_ids_to_delete[num_strings_to_delete] = strings[i];
                        entry->str = NULL;
                        entry->in_use = false;
                        num_strings_to_delete++;
                        JAK_CHECK_SUCCESS(freelist_push(self, jak_archive_field_sid_t));
                }
        }

        /** remove from index */
        JAK_CHECK_SUCCESS(jak_str_hash_remove(&extra->index, jak_string_to_delete, num_strings_to_delete));

        /** free up resources for strings that should be removed */
        for (size_t i = 0; i < num_strings_to_delete; i++) {
                free(jak_string_to_delete[i]);
        }

        /** cleanup */
        jak_alloc_free(&self->alloc, jak_string_to_delete);
        jak_alloc_free(&self->alloc, jak_string_ids_to_delete);

        _jak_encode_sync_unlock(self);
        return true;
}

static bool
_jak_encode_sync_locate_safe(jak_string_dict *self, jak_archive_field_sid_t **out, bool **found_mask, size_t *num_not_found,
                 char *const *keys, size_t num_keys)
{
        jak_timestamp begin = jak_wallclock();
        JAK_TRACE(STRING_DIC_SYNC_TAG, "'locate_safe' function invoked for %zu strings", num_keys)

        JAK_ERROR_IF_NULL(self);
        JAK_ERROR_IF_NULL(out);
        JAK_ERROR_IF_NULL(found_mask);
        JAK_ERROR_IF_NULL(num_not_found);
        JAK_ERROR_IF_NULL(keys);
        JAK_ERROR_IF_NULL(num_keys);
        JAK_CHECK_TAG(self->tag, JAK_SYNC)

        _jak_encode_sync_lock(self);
        struct sync_extra *extra = this_extra(self);
        int status = jak_str_hash_get_bulk_safe(out, found_mask, num_not_found, &extra->index, keys, num_keys);
        _jak_encode_sync_unlock(self);

        jak_timestamp end = jak_wallclock();
        JAK_UNUSED(begin);
        JAK_UNUSED(end);
        JAK_TRACE(STRING_DIC_SYNC_TAG, "'locate_safe' function done: %f seconds spent here", (end - begin) / 1000.0f)

        return status;
}

static bool
_jak_encode_sync_locate_fast(jak_string_dict *self, jak_archive_field_sid_t **out, char *const *keys, size_t num_keys)
{
        JAK_CHECK_TAG(self->tag, JAK_SYNC)

        bool *found_mask;
        size_t num_not_found;

        /** use safer but in principle more slower implementation */
        int result = _jak_encode_sync_locate_safe(self, out, &found_mask, &num_not_found, keys, num_keys);

        /** cleanup */
        _jak_encode_sync_free(self, found_mask);

        return result;
}

static char **_jak_encode_sync_extract(jak_string_dict *self, const jak_archive_field_sid_t *ids, size_t num_ids)
{
        if (JAK_UNLIKELY(!self || !ids || num_ids == 0 || self->tag != JAK_SYNC)) {
                return NULL;
        }

        _jak_encode_sync_lock(self);

        jak_allocator jak_hashtable_alloc;
#if defined(JAK_CONFIG_TRACE_STRING_DIC_ALLOC) && !defined(NDEBUG)
        allocatorTrace(&jak_hashtable_alloc);
#else
        jak_alloc_this_or_std(&jak_hashtable_alloc, &self->alloc);
#endif

        struct sync_extra *extra = this_extra(self);
        char **result = jak_alloc_malloc(&jak_hashtable_alloc, num_ids * sizeof(char *));
        struct entry *entries = (struct entry *) jak_vector_data(&extra->contents);

        /** Optimization: notify the kernel that the content list is accessed randomly (since hash based access)*/
        jak_vector_memadvice(&extra->contents, MADV_RANDOM | MADV_WILLNEED);

        for (size_t i = 0; i < num_ids; i++) {
                jak_archive_field_sid_t jak_archive_field_sid_t = ids[i];
                JAK_ASSERT(jak_archive_field_sid_t < jak_vector_length(&extra->contents));
                JAK_ASSERT(
                        jak_archive_field_sid_t == JAK_NULL_ENCODED_STRING || entries[jak_archive_field_sid_t].in_use);
                result[i] = jak_archive_field_sid_t != JAK_NULL_ENCODED_STRING ? entries[jak_archive_field_sid_t].str
                                                                               : JAK_NULL_TEXT;
        }

        _jak_encode_sync_unlock(self);
        return result;
}

static bool _jak_encode_sync_free(jak_string_dict *self, void *ptr)
{
        JAK_UNUSED(self);

        jak_allocator jak_hashtable_alloc;
#if defined(JAK_CONFIG_TRACE_STRING_DIC_ALLOC) && !defined(NDEBUG)
        CHECK_SUCCESS(allocatorTrace(&jak_hashtable_alloc));
#else
        JAK_CHECK_SUCCESS(jak_alloc_this_or_std(&jak_hashtable_alloc, &self->alloc));
#endif

        return jak_alloc_free(&jak_hashtable_alloc, ptr);
}

static bool _jak_encode_sync_reset_counters(jak_string_dict *self)
{
        JAK_CHECK_TAG(self->tag, JAK_SYNC)
        struct sync_extra *extra = this_extra(self);
        JAK_CHECK_SUCCESS(jak_str_hash_reset_counters(&extra->index));
        return true;
}

static bool _jak_encode_sync_counters(jak_string_dict *self, jak_str_hash_counters *counters)
{
        JAK_CHECK_TAG(self->tag, JAK_SYNC)
        struct sync_extra *extra = this_extra(self);
        JAK_CHECK_SUCCESS(jak_str_hash_get_counters(counters, &extra->index));
        return true;
}

static bool _jak_encode_sync_num_distinct(jak_string_dict *self, size_t *num)
{
        JAK_CHECK_TAG(self->tag, JAK_SYNC)
        struct sync_extra *extra = this_extra(self);
        *num = jak_vector_length(&extra->contents);
        return true;
}

static bool _jak_encode_sync_get_contents(jak_string_dict *self, jak_vector ofType (char *) *strings,
                              jak_vector ofType(jak_archive_field_sid_t) *jak_string_ids)
{
        JAK_CHECK_TAG(self->tag, JAK_SYNC);
        struct sync_extra *extra = this_extra(self);

        for (jak_archive_field_sid_t i = 0; i < extra->contents.num_elems; i++) {
                const struct entry *e = JAK_VECTOR_GET(&extra->contents, i, struct entry);
                if (e->in_use) {
                        jak_vector_push(strings, &e->str, 1);
                        jak_vector_push(jak_string_ids, &i, 1);
                }
        }
        return true;
}