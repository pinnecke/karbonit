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

#include <karbonit/std/vec.h>
#include <karbonit/std/spinlock.h>
#include <karbonit/std/str_hash.h>
#include <karbonit/archive/encode_sync.h>
#include <karbonit/std/str_hash/mem.h>
#include <karbonit/utils/time.h>
#include <karbonit/std/bloom.h>
#include <karbonit/std/hash.h>

#define STRING_DIC_SYNC_TAG "string-dic-sync"

struct entry {
        char *str;
        bool in_use;
};

struct sync_extra {
        vec ofType(entry) contents;
        vec ofType(string_id_t_t) freelist;
        str_hash index;
        spinlock _encode_sync_lock;
};

static bool _encode_sync_drop(string_dict *self);

static bool
_encode_sync_insert(string_dict *self, archive_field_sid_t **out, char *const *strings, size_t num_strings,
            size_t num_threads);

static bool _encode_sync_remove(string_dict *self, archive_field_sid_t *strings, size_t num_strings);

static bool
_encode_sync_locate_safe(string_dict *self, archive_field_sid_t **out, bool **found_mask, size_t *num_not_found,
                 char *const *keys, size_t num_keys);

static bool
_encode_sync_locate_fast(string_dict *self, archive_field_sid_t **out, char *const *keys, size_t num_keys);

static char **_encode_sync_extract(string_dict *self, const archive_field_sid_t *ids, size_t num_ids);

static bool _encode_sync_free(string_dict *self, void *ptr);

static bool _encode_sync_reset_counters(string_dict *self);

static bool _encode_sync_counters(string_dict *self, str_hash_counters *counters);

static bool _encode_sync_num_distinct(string_dict *self, size_t *num);

static bool _encode_sync_get_contents(string_dict *self, vec ofType (char *) *strings,
                              vec ofType(archive_field_sid_t) *string_ids);

static void _encode_sync_lock(string_dict *self);

static void _encode_sync_unlock(string_dict *self);

static int
create_extra(string_dict *self, size_t capacity, size_t num_index_buckets, size_t num_index_bucket_cap,
             size_t num_threads);

static struct sync_extra *this_extra(string_dict *self);

static int freelist_pop(archive_field_sid_t *out, string_dict *self);

static int freelist_push(string_dict *self, archive_field_sid_t idx);

int
encode_sync_create(string_dict *dic, size_t capacity, size_t num_indx_buckets, size_t num_index_bucket_cap,
                   size_t num_threads)
{
        dic->tag = SYNC;
        dic->drop = _encode_sync_drop;
        dic->insert = _encode_sync_insert;
        dic->remove = _encode_sync_remove;
        dic->locate_safe = _encode_sync_locate_safe;
        dic->locate_fast = _encode_sync_locate_fast;
        dic->extract = _encode_sync_extract;
        dic->free = _encode_sync_free;
        dic->resetCounters = _encode_sync_reset_counters;
        dic->counters = _encode_sync_counters;
        dic->num_distinct = _encode_sync_num_distinct;
        dic->get_contents = _encode_sync_get_contents;

        CHECK_SUCCESS(create_extra(dic, capacity, num_indx_buckets, num_index_bucket_cap, num_threads));
        return true;
}

static void _encode_sync_lock(string_dict *self)
{
        assert(self->tag == SYNC);
        struct sync_extra *extra = this_extra(self);
        spinlock_acquire(&extra->_encode_sync_lock);
}

static void _encode_sync_unlock(string_dict *self)
{
        assert(self->tag == SYNC);
        struct sync_extra *extra = this_extra(self);
        spinlock_release(&extra->_encode_sync_lock);
}

static int
create_extra(string_dict *self, size_t capacity, size_t num_index_buckets, size_t num_index_bucket_cap,
             size_t num_threads)
{
        self->extra = MALLOC(sizeof(struct sync_extra));
        struct sync_extra *extra = this_extra(self);
        spinlock_init(&extra->_encode_sync_lock);
        CHECK_SUCCESS(vec_create(&extra->contents, sizeof(struct entry), capacity));
        CHECK_SUCCESS(vec_create(&extra->freelist, sizeof(archive_field_sid_t), capacity));
        struct entry empty = {.str    = NULL, .in_use = false};
        for (size_t i = 0; i < capacity; i++) {
                CHECK_SUCCESS(vec_push(&extra->contents, &empty, 1));
                freelist_push(self, i);
        }
        UNUSED(num_threads);

        CHECK_SUCCESS(str_hash_create_inmemory(&extra->index,
                                                  num_index_buckets,
                                                  num_index_bucket_cap));
        return true;
}

static struct sync_extra *this_extra(string_dict *self)
{
        assert (self->tag == SYNC);
        return (struct sync_extra *) self->extra;
}

static int freelist_pop(archive_field_sid_t *out, string_dict *self)
{
        assert (self->tag == SYNC);
        struct sync_extra *extra = this_extra(self);
        if (UNLIKELY(vec_is_empty(&extra->freelist))) {
                size_t num_new_pos;
                CHECK_SUCCESS(vec_grow(&num_new_pos, &extra->freelist));
                CHECK_SUCCESS(vec_grow(NULL, &extra->contents));
                assert (extra->freelist.cap_elems == extra->contents.cap_elems);
                struct entry empty = {.in_use = false, .str    = NULL};
                while (num_new_pos--) {
                        size_t new_pos = VEC_LENGTH(&extra->contents);
                        CHECK_SUCCESS(vec_push(&extra->freelist, &new_pos, 1));
                        CHECK_SUCCESS(vec_push(&extra->contents, &empty, 1));
                }
        }
        *out = *(archive_field_sid_t *) vec_pop(&extra->freelist);
        return true;
}

static int freelist_push(string_dict *self, archive_field_sid_t idx)
{
        assert (self->tag == SYNC);
        struct sync_extra *extra = this_extra(self);
        CHECK_SUCCESS(vec_push(&extra->freelist, &idx, 1));
        assert (extra->freelist.cap_elems == extra->contents.cap_elems);
        return true;
}

static bool _encode_sync_drop(string_dict *self)
{
        struct sync_extra *extra = this_extra(self);

        struct entry *entries = (struct entry *) extra->contents.base;
        for (size_t i = 0; i < extra->contents.num_elems; i++) {
                struct entry *entry = entries + i;
                if (entry->in_use) {
                        assert (entry->str);
                        free(entry->str);
                        entry->str = NULL;
                }
        }

        vec_drop(&extra->freelist);
        vec_drop(&extra->contents);
        str_hash_drop(&extra->index);
        free(self->extra);

        return true;
}

static bool
_encode_sync_insert(string_dict *self, archive_field_sid_t **out, char *const *strings, size_t num_strings,
            size_t num_threads)
{
        TRACE(STRING_DIC_SYNC_TAG, "local str_buf dictionary insertion invoked for %zu strings", num_strings);
        timestamp begin = wallclock();

        UNUSED(num_threads);

        _encode_sync_lock(self);

        struct sync_extra *extra = this_extra(self);

        archive_field_sid_t *ids_out = MALLOC(num_strings * sizeof(archive_field_sid_t));
        bool *found_mask;
        archive_field_sid_t *values;
        size_t num_not_found;

        /** query index for strings to get a boolean mask which strings are new and which must be added */
        /** This is for the case that the string dictionary is not empty to skip processing of those new elements
         * which are already contained */
        TRACE(STRING_DIC_SYNC_TAG, "local str_buf dictionary check for new strings in insertion bulk%s", "...");

        /** NOTE: palatalization of the call to this function decreases performance */
        str_hash_get_bulk_safe(&values, &found_mask, &num_not_found, &extra->index, strings, num_strings);

        /** OPTIMIZATION: use a bitmap to check whether a string (which has not appeared in the
         * dictionary before this batch but might occur multiple times in the current batch) was seen
         * before (with a slight prob. of doing too much work) */
        bitmap bitmap;
        bloom_create(&bitmap, 22 * num_not_found);

        /** copy string ids for already known strings to their result position resp. add those which are new */
        for (size_t i = 0; i < num_strings; i++) {

                if (found_mask[i]) {
                        ids_out[i] = values[i];
                } else {
                        /** This path is taken only for strings that are not already contained in the dictionary. However,
                         * since this insertion batch may contain duplicate string, querying for already inserted strings
                         * must be done anyway for each string in the insertion batch that is inserted. */

                        archive_field_sid_t string_id = 0;
                        const char *key = (const char *) (strings[i]);

                        bool found = false;
                        archive_field_sid_t value;

                        /** Query the bitmap if the keys was already seend. If the filter returns "yes", a lookup
                         * is requried since the filter maybe made a mistake. Of the filter returns "no", the
                         * keys is new for sure. In this case, one can skip the lookup into the buckets. */
                        size_t key_length = strlen(key);
                        hash32_t bloom_key = key_length > 0 ? HASH_FNV(strlen(key), key)
                                                            : 0; /** using a hash of a keys instead of the string keys itself avoids reading the entire string for computing k hashes inside the bitmap */
                        if (BLOOM_TEST_AND_SET(&bitmap, &bloom_key, sizeof(hash32_t))) {
                                /** ensure that the string really was seen (due to collisions in the bloom filter the keys might not
                                 * been actually seen) */

                                /** query index for strings to get a boolean mask which strings are new and which must be added */
                                /** This is for the case that the string was not already contained in the string dictionary but may have
                                 * duplicates in this insertion batch that are already inserted */
                                str_hash_get_bulk_safe_exact(&value,
                                                            &found,
                                                            &extra->index,
                                                            key);  /** OPTIMIZATION: use specialized function for "exact" query to avoid unnessecary malloc calls to manage set of results if only a single result is needed */
                        }

                        if (found) {
                                ids_out[i] = value;
                        } else {

                                /** register in contents list */
                                bool pop_result = freelist_pop(&string_id, self);
                                ERROR_IF_AND_RETURN(!pop_result, ERR_SLOTBROKEN, NULL)
                                struct entry *entries = (struct entry *) vec_data(&extra->contents);
                                struct entry *entry = entries + string_id;
                                assert (!entry->in_use);
                                entry->in_use = true;
                                entry->str = strdup(strings[i]);
                                ids_out[i] = string_id;

                                /** add for not yet registered pairs to buffer for fast import */
                                str_hash_put_exact_fast(&extra->index, entry->str, string_id);
                        }
                }
        }

        /** set potential non-null out parameters */
        OPTIONAL_SET_OR_ELSE(out, ids_out, free(ids_out));

        /** cleanup */
        free(found_mask);
        free(values);
        bloom_drop(&bitmap);

        _encode_sync_unlock(self);

        timestamp end = wallclock();
        UNUSED(begin);
        UNUSED(end);
        INFO(STRING_DIC_SYNC_TAG, "insertion operation done: %f seconds spent here", (end - begin) / 1000.0f)

        return true;

}

static bool _encode_sync_remove(string_dict *self, archive_field_sid_t *strings, size_t num_strings)
{
        _encode_sync_lock(self);

        struct sync_extra *extra = this_extra(self);

        size_t num_strings_to_delete = 0;
        char **string_to_delete = MALLOC(num_strings * sizeof(char *));
        archive_field_sid_t *string_ids_to_delete = MALLOC(num_strings * sizeof(archive_field_sid_t));

        /** remove strings from contents vec, and skip duplicates */
        for (size_t i = 0; i < num_strings; i++) {
                archive_field_sid_t archive_field_sid_t = strings[i];
                struct entry *entry = (struct entry *) vec_data(&extra->contents) + archive_field_sid_t;
                if (LIKELY(entry->in_use)) {
                        string_to_delete[num_strings_to_delete] = entry->str;
                        string_ids_to_delete[num_strings_to_delete] = strings[i];
                        entry->str = NULL;
                        entry->in_use = false;
                        num_strings_to_delete++;
                        CHECK_SUCCESS(freelist_push(self, archive_field_sid_t));
                }
        }

        /** remove from index */
        CHECK_SUCCESS(str_hash_remove(&extra->index, string_to_delete, num_strings_to_delete));

        /** free up resources for strings that should be removed */
        for (size_t i = 0; i < num_strings_to_delete; i++) {
                free(string_to_delete[i]);
        }

        /** cleanup */
        free(string_to_delete);
        free(string_ids_to_delete);

        _encode_sync_unlock(self);
        return true;
}

static bool
_encode_sync_locate_safe(string_dict *self, archive_field_sid_t **out, bool **found_mask, size_t *num_not_found,
                 char *const *keys, size_t num_keys)
{
        timestamp begin = wallclock();
        TRACE(STRING_DIC_SYNC_TAG, "'locate_safe' function invoked for %zu strings", num_keys)

        _encode_sync_lock(self);
        struct sync_extra *extra = this_extra(self);
        int status = str_hash_get_bulk_safe(out, found_mask, num_not_found, &extra->index, keys, num_keys);
        _encode_sync_unlock(self);

        timestamp end = wallclock();
        UNUSED(begin);
        UNUSED(end);
        TRACE(STRING_DIC_SYNC_TAG, "'locate_safe' function done: %f seconds spent here", (end - begin) / 1000.0f)

        return status;
}

static bool
_encode_sync_locate_fast(string_dict *self, archive_field_sid_t **out, char *const *keys, size_t num_keys)
{
        bool *found_mask;
        size_t num_not_found;

        /** use safer but in principle more slower implementation */
        int result = _encode_sync_locate_safe(self, out, &found_mask, &num_not_found, keys, num_keys);

        /** cleanup */
        _encode_sync_free(self, found_mask);

        return result;
}

static char **_encode_sync_extract(string_dict *self, const archive_field_sid_t *ids, size_t num_ids)
{
        if (UNLIKELY(!self || !ids || num_ids == 0 || self->tag != SYNC)) {
                return NULL;
        }

        _encode_sync_lock(self);

        struct sync_extra *extra = this_extra(self);
        char **result = MALLOC(num_ids * sizeof(char *));
        struct entry *entries = (struct entry *) vec_data(&extra->contents);

        /** Optimization: notify the kernel that the content list is accessed randomly (since hash based access)*/
        vec_madvise(&extra->contents, MADV_RANDOM | MADV_WILLNEED);

        for (size_t i = 0; i < num_ids; i++) {
                archive_field_sid_t archive_field_sid_t = ids[i];
                assert(archive_field_sid_t < VEC_LENGTH(&extra->contents));
                assert(
                        archive_field_sid_t == NULL_ENCODED_STRING || entries[archive_field_sid_t].in_use);
                result[i] = archive_field_sid_t != NULL_ENCODED_STRING ? entries[archive_field_sid_t].str
                                                                               : NULL_TEXT;
        }

        _encode_sync_unlock(self);
        return result;
}

static bool _encode_sync_free(string_dict *self, void *ptr)
{
        UNUSED(self);
        free(ptr);
        return true;
}

static bool _encode_sync_reset_counters(string_dict *self)
{
        struct sync_extra *extra = this_extra(self);
        CHECK_SUCCESS(str_hash_reset_counters(&extra->index));
        return true;
}

static bool _encode_sync_counters(string_dict *self, str_hash_counters *counters)
{
        struct sync_extra *extra = this_extra(self);
        CHECK_SUCCESS(str_hash_get_counters(counters, &extra->index));
        return true;
}

static bool _encode_sync_num_distinct(string_dict *self, size_t *num)
{
        struct sync_extra *extra = this_extra(self);
        *num = VEC_LENGTH(&extra->contents);
        return true;
}

static bool _encode_sync_get_contents(string_dict *self, vec ofType (char *) *strings,
                              vec ofType(archive_field_sid_t) *string_ids)
{
        struct sync_extra *extra = this_extra(self);

        for (archive_field_sid_t i = 0; i < extra->contents.num_elems; i++) {
                const struct entry *e = VEC_GET(&extra->contents, i, struct entry);
                if (e->in_use) {
                        vec_push(strings, &e->str, 1);
                        vec_push(string_ids, &i, 1);
                }
        }
        return true;
}