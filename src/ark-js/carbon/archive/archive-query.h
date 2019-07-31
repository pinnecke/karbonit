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

#ifndef ARK_QUERY_H
#define ARK_QUERY_H

#include <ark-js/shared/common.h>
#include <ark-js/carbon/archive/archive.h>
#include <ark-js/carbon/archive/archive-strid-iter.h>
#include <ark-js/carbon/archive/archive-pred.h>
#include <ark-js/shared/stdx/hash_table.h>

ARK_BEGIN_DECL

struct archive_query {
        struct archive *archive;
        struct io_context *context;
        struct err err;
};

ARK_DEFINE_GET_ERROR_FUNCTION(query, struct archive_query, query)

ARK_EXPORT(bool) query_create(struct archive_query *query, struct archive *archive);

ARK_EXPORT(bool) query_drop(struct archive_query *query);

ARK_EXPORT(bool) query_scan_strids(struct strid_iter *it, struct archive_query *query);

ARK_EXPORT(bool) query_create_index_string_id_to_offset(struct sid_to_offset **index, struct archive_query *query);

ARK_EXPORT(void) query_drop_index_string_id_to_offset(struct sid_to_offset *index);

ARK_EXPORT(bool) query_index_id_to_offset_serialize(FILE *file, struct err *err, struct sid_to_offset *index);

ARK_EXPORT(bool) query_index_id_to_offset_deserialize(struct sid_to_offset **index, struct err *err,
        const char *file_path, offset_t offset);

ARK_EXPORT(char *) query_fetch_string_by_id(struct archive_query *query, field_sid_t id);

ARK_EXPORT(char *) query_fetch_string_by_id_nocache(struct archive_query *query, field_sid_t id);

ARK_EXPORT(char **) query_fetch_strings_by_offset(struct archive_query *query, offset_t *offs, u32 *strlens,
        size_t num_offs);

ARK_EXPORT(field_sid_t *)query_find_ids(size_t *num_found, struct archive_query *query,
        const struct string_pred_t *pred, void *capture, i64 limit);

ARK_END_DECL

#endif
