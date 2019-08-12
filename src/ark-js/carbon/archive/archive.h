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

#ifndef ARK_ARCHIVE_H
#define ARK_ARCHIVE_H

#include <ark-js/shared/common.h>
#include <ark-js/shared/mem/block.h>
#include <ark-js/shared/mem/file.h>
#include <ark-js/carbon/coding/pack.h>
#include <ark-js/shared/json/columndoc.h>
#include <ark-js/carbon/archive/archive-io.h>
#include <ark-js/carbon/archive/archive-int.h>

ARK_BEGIN_DECL

struct archive_query; /* forwarded from 'types-query.h' */

struct archive {
    struct archive_info info;
    char *diskFilePath;
    struct string_table string_table;
    struct record_table record_table;
    struct err err;
    struct sid_to_offset *query_index_string_id_to_offset;
    struct string_cache *string_id_cache;
    struct archive_query *default_query;
};

struct archive_callback {
    void (*begin_create_from_model)();

    void (*end_create_from_model)();

    void (*begin_create_from_json)();

    void (*end_create_from_json)();

    void (*begin_archive_stream_from_json)();

    void (*end_archive_stream_from_json)();

    void (*begin_write_archive_file_to_disk)();

    void (*end_write_archive_file_to_disk)();

    void (*begin_load_archive)();

    void (*end_load_archive)();

    void (*begin_setup_string_dictionary)();

    void (*end_setup_string_dictionary)();

    void (*begin_parse_json)();

    void (*end_parse_json)();

    void (*begin_test_json)();

    void (*end_test_json)();

    void (*begin_import_json)();

    void (*end_import_json)();

    void (*begin_cleanup)();

    void (*end_cleanup)();

    void (*begin_write_string_table)();

    void (*end_write_string_table)();

    void (*begin_write_record_table)();

    void (*end_write_record_table)();

    void (*skip_string_id_index_baking)();

    void (*begin_string_id_index_baking)();

    void (*end_string_id_index_baking)();
};

bool archive_from_json(struct archive *out, const char *file, struct err *err, const char *json_string,
                       enum packer_type compressor, enum strdic_tag dictionary, size_t num_async_dic_threads,
                       bool read_optimized,
                       bool bake_string_id_index, struct archive_callback *callback);

bool archive_stream_from_json(struct memblock **stream, struct err *err, const char *json_string,
                              enum packer_type compressor, enum strdic_tag dictionary, size_t num_async_dic_threads,
                              bool read_optimized,
                              bool bake_id_index, struct archive_callback *callback);

bool archive_from_model(struct memblock **stream, struct err *err, struct columndoc *model,
                        enum packer_type compressor, bool bake_string_id_index, struct archive_callback *callback);

bool archive_write(FILE *file, const struct memblock *stream);

bool archive_load(struct memblock **stream, FILE *file);

bool archive_print(FILE *file, struct err *err, struct memblock *stream);

bool archive_open(struct archive *out, const char *file_path);

bool archive_get_info(struct archive_info *info, const struct archive *archive);

ARK_DEFINE_GET_ERROR_FUNCTION(archive, struct archive, archive);

bool archive_close(struct archive *archive);

bool archive_drop_indexes(struct archive *archive);

bool archive_query(struct archive_query *query, struct archive *archive);

bool archive_has_query_index_string_id_to_offset(bool *state, struct archive *archive);

bool archive_hash_query_string_id_cache(bool *has_cache, struct archive *archive);

bool archive_drop_query_string_id_cache(struct archive *archive);

struct string_cache *archive_get_query_string_id_cache(struct archive *archive);

struct archive_query *archive_query_default(struct archive *archive);

/**
 * Creates a new <code>struct io_context</code> to access the archives underlying file for unsafe operations.
 *
 * An unsafe operation directly seeks randomly in the underlying file. To avoid creation of multiple file
 * descriptors while at the same time allow to access unsafe operations in a multi-threading environment, an
 * <code>struct io_context</code> is used. Roughly, such a context is a regular FILE that is protected by a lock.
 *
 * @param archive The archive
 * @return a heap-allocated instance of <code>struct io_context</code>, or NULL if not successful
 */
struct io_context *archive_io_context_create(struct archive *archive);

ARK_END_DECL

#endif