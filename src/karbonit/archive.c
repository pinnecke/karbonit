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

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <inttypes.h>

#include <karbonit/forwdecl.h>
#include <karbonit/stdx/unique_id.h>
#include <karbonit/archive/encode_async.h>
#include <karbonit/archive/pack.h>
#include <karbonit/archive/strid_it.h>
#include <karbonit/archive/internal.h>
#include <karbonit/archive/query.h>
#include <karbonit/archive/cache.h>
#include <karbonit/archive.h>
#include <karbonit/archive/encode_sync.h>
#include <karbonit/stdinc.h>
#include <karbonit/mem/memblock.h>
#include <karbonit/mem/memfile.h>
#include <karbonit/archive/huffman.h>
#include <karbonit/archive.h>

#define WRITE_PRIMITIVE_VALUES(memfile, values_vec, type)                                                              \
{                                                                                                                      \
    type *values = VEC_ALL(values_vec, type);                                                                \
    MEMFILE_WRITE(memfile, values, values_vec->num_elems * sizeof(type));                                       \
}

#define WRITE_ARRAY_VALUES(memfile, values_vec, type)                                                                  \
{                                                                                                                      \
    for (u32 i = 0; i < values_vec->num_elems; i++) {                                                             \
        vec ofType(type) *nested_values = VEC_GET(values_vec, i, vec);                     \
        WRITE_PRIMITIVE_VALUES(memfile, nested_values, type);                                                          \
    }                                                                                                                  \
}

#define PRINT_SIMPLE_PROPS(file, memfile, offset, nesting_level, value_type, type_string, format_string)               \
{                                                                                                                      \
    prop_header *prop_header = MEMFILE_READ_TYPE(memfile, struct prop_header);             \
    archive_field_sid_t *keys = (archive_field_sid_t *) MEMFILE_READ(memfile, prop_header->num_entries *          \
                                   sizeof(archive_field_sid_t));                                                        \
    value_type *values = (value_type *) MEMFILE_READ(memfile, prop_header->num_entries * sizeof(value_type));   \
    fprintf(file, "0x%04x ", (unsigned) offset);                                                                       \
    INTENT_LINE(nesting_level)                                                                                         \
    fprintf(file, "[marker: %c (" type_string ")] [num_entries: %d] [", entryMarker, prop_header->num_entries);        \
    for (u32 i = 0; i < prop_header->num_entries; i++) {                                                          \
        fprintf(file, "key: %"PRIu64"%s", keys[i], i + 1 < prop_header->num_entries ? ", " : "");                      \
    }                                                                                                                  \
    fprintf(file, "] [");                                                                                              \
    for (u32 i = 0; i < prop_header->num_entries; i++) {                                                          \
      fprintf(file, "value: "format_string"%s", values[i], i + 1 < prop_header->num_entries ? ", " : "");              \
    }                                                                                                                  \
    fprintf(file, "]\n");                                                                                              \
}

#define PRINT_ARRAY_PROPS(memfile, offset, nesting_level, entryMarker, type, type_string, format_string)               \
{                                                                                                                      \
    prop_header *prop_header = MEMFILE_READ_TYPE(memfile, struct prop_header);             \
                                                                                                                       \
    archive_field_sid_t *keys = (archive_field_sid_t *) MEMFILE_READ(memfile, prop_header->num_entries *          \
                                        sizeof(archive_field_sid_t));                                                   \
    u32 *array_lengths;                                                                                           \
                                                                                                                       \
    fprintf(file, "0x%04x ", (unsigned) offset);                                                                       \
    INTENT_LINE(nesting_level)                                                                                         \
    fprintf(file, "[marker: %c ("type_string")] [num_entries: %d] [", entryMarker, prop_header->num_entries);          \
                                                                                                                       \
    for (u32 i = 0; i < prop_header->num_entries; i++) {                                                          \
        fprintf(file, "key: %"PRIu64"%s", keys[i], i + 1 < prop_header->num_entries ? ", " : "");                      \
    }                                                                                                                  \
    fprintf(file, "] [");                                                                                              \
                                                                                                                       \
    array_lengths = (u32 *) MEMFILE_READ(memfile, prop_header->num_entries * sizeof(u32));            \
                                                                                                                       \
    for (u32 i = 0; i < prop_header->num_entries; i++) {                                                          \
        fprintf(file, "num_entries: %d%s", array_lengths[i], i + 1 < prop_header->num_entries ? ", " : "");            \
    }                                                                                                                  \
                                                                                                                       \
    fprintf(file, "] [");                                                                                              \
                                                                                                                       \
    for (u32 array_idx = 0; array_idx < prop_header->num_entries; array_idx++) {                                  \
        type *values = (type *) MEMFILE_READ(memfile, array_lengths[array_idx] * sizeof(type));                 \
        fprintf(file, "[");                                                                                            \
        for (u32 i = 0; i < array_lengths[array_idx]; i++) {                                                      \
            fprintf(file, "value: "format_string"%s", values[i], i + 1 < array_lengths[array_idx] ? ", " : "");        \
        }                                                                                                              \
        fprintf(file, "]%s", array_idx + 1 < prop_header->num_entries ? ", " : "");                                    \
    }                                                                                                                  \
                                                                                                                       \
    fprintf(file, "]\n");                                                                                              \
}

#define INTENT_LINE(nesting_level)                                                                                     \
{                                                                                                                      \
    for (unsigned nest_level = 0; nest_level < nesting_level; nest_level++) {                                          \
        fprintf(file, "   ");                                                                                          \
    }                                                                                                                  \
}

#define PRINT_VALUE_ARRAY(type, memfile, header, format_string)                                                        \
{                                                                                                                      \
    u32 num_elements = *MEMFILE_READ_TYPE(memfile, u32);                                              \
    const type *values = (const type *) MEMFILE_READ(memfile, num_elements * sizeof(type));                     \
    fprintf(file, "0x%04x ", (unsigned) offset);                                                                       \
    INTENT_LINE(nesting_level);                                                                                        \
    fprintf(file, "   [num_elements: %d] [values: [", num_elements);                                                   \
    for (size_t i = 0; i < num_elements; i++) {                                                                        \
        fprintf(file, "value: "format_string"%s", values[i], i + 1 < num_elements ? ", " : "");                        \
    }                                                                                                                  \
    fprintf(file, "]\n");                                                                                              \
}

static offset_t skip_record_header(memfile *memfile);

static void
update_record_header(memfile *memfile, offset_t root_object_header_offset, column_doc *model,
                     u64 record_size);

static bool __serialize(offset_t *offset, memfile *memfile,
                        column_doc_obj *columndoc,
                        offset_t root_object_header_offset);

static object_flags_u *get_flags(object_flags_u *flags, column_doc_obj *columndoc);

static void update_file_header(memfile *memfile, offset_t root_object_header_offset);

static void skip_file_header(memfile *memfile);

static bool serialize_string_dic(memfile *memfile, const doc_bulk *context,
                                 packer_e compressor);

static bool print_archive_from_memfile(FILE *file, memfile *memfile);

bool archive_from_json(archive *out, const char *file, const char *json_string,
                           packer_e compressor, str_dict_tag_e dictionary,
                           size_t num_async_dic_threads,
                           bool read_optimized,
                           bool bake_string_id_index, archive_callback *callback)
{
        OPTIONAL_CALL(callback, begin_create_from_json);

        memblock *stream;
        FILE *out_file;

        if (!archive_stream_from_json(&stream,
                                          json_string,
                                          compressor,
                                          dictionary,
                                          num_async_dic_threads,
                                          read_optimized,
                                          bake_string_id_index,
                                          callback)) {
                return false;
        }

        OPTIONAL_CALL(callback, begin_write_archive_file_to_disk);

        if ((out_file = fopen(file, "w")) == NULL) {
                ERROR(ERR_FOPENWRITE, NULL);
                MEMBLOCK_DROP(stream);
                return false;
        }

        if (!archive_write(out_file, stream)) {
                ERROR(ERR_WRITEARCHIVE, NULL);
                fclose(out_file);
                MEMBLOCK_DROP(stream);
                return false;
        }

        fclose(out_file);

        OPTIONAL_CALL(callback, end_write_archive_file_to_disk);

        OPTIONAL_CALL(callback, begin_load_archive);

        if (!archive_open(out, file)) {
                ERROR(ERR_ARCHIVEOPEN, NULL);
                return false;
        }

        OPTIONAL_CALL(callback, end_load_archive);

        MEMBLOCK_DROP(stream);

        OPTIONAL_CALL(callback, end_create_from_json);

        return true;
}

bool archive_stream_from_json(memblock **stream, const char *json_string,
                              packer_e compressor, str_dict_tag_e dictionary,
                              size_t num_async_dic_threads,
                              bool read_optimized,
                              bool bake_id_index, archive_callback *callback)
{
        string_dict dic;
        json_parser parser;
        json_err error_desc;
        doc_bulk bulk;
        doc_entries *partition;
        column_doc *columndoc;
        json json;

        OPTIONAL_CALL(callback, begin_archive_stream_from_json)

        OPTIONAL_CALL(callback, begin_setup_string_dict_ionary);
        if (dictionary == SYNC) {
                encode_sync_create(&dic, 1000, 1000, 1000, 0);
        } else if (dictionary == ASYNC) {
                encode_async_create(&dic, 1000, 1000, 1000, num_async_dic_threads);
        } else {
                ERROR(ERR_UNKNOWN_DIC_TYPE, NULL);
        }

        OPTIONAL_CALL(callback, end_setup_string_dict_ionary);

        OPTIONAL_CALL(callback, begin_parse_json);
        if (!(json_parse(&json, &error_desc, &parser, json_string))) {
                char buffer[2048];
                if (error_desc.token) {
                        sprintf(buffer,
                                "%s. Token %s was found in line %u column %u",
                                error_desc.msg,
                                error_desc.token_type_str,
                                error_desc.token->line,
                                error_desc.token->column);
                        ERROR(ERR_JSONPARSEERR, &buffer[0]);
                } else {
                        sprintf(buffer, "%s", error_desc.msg);
                        ERROR(ERR_JSONPARSEERR, &buffer[0]);
                }
                return false;
        }
        OPTIONAL_CALL(callback, end_parse_json);

        OPTIONAL_CALL(callback, begin_test_json);
        if (!json_test(&json)) {
                return false;
        }
        OPTIONAL_CALL(callback, end_test_json);

        OPTIONAL_CALL(callback, begin_import_json);
        if (!doc_bulk_create(&bulk, &dic)) {
                ERROR(ERR_BULKCREATEFAILED, NULL);
                return false;
        }

        partition = doc_bulk_new_entries(&bulk);
        doc_bulk_add_json(partition, &json);

        json_drop(&json);

        doc_bulk_shrink(&bulk);

        columndoc = doc_entries_columndoc(&bulk, partition, read_optimized);

        if (!archive_from_model(stream, columndoc, compressor, bake_id_index, callback)) {
                return false;
        }

        OPTIONAL_CALL(callback, end_import_json);

        OPTIONAL_CALL(callback, begin_cleanup);
        string_dict_drop(&dic);
        doc_bulk_drop(&bulk);
        doc_entries_drop(partition);
        columndoc_free(columndoc);
        free(columndoc);
        OPTIONAL_CALL(callback, end_cleanup);

        OPTIONAL_CALL(callback, end_archive_stream_from_json)

        return true;
}

static bool run_string_id_baking(memblock **stream)
{
        archive archive;
        char tmp_file_name[512];
        unique_id_t rand_part;
        unique_id_create(&rand_part);
        sprintf(tmp_file_name, "/tmp/carbon-tool-temp-%"
                               PRIu64
                               ".carbon-tool", rand_part);
        FILE *tmp_file;

        if ((tmp_file = fopen(tmp_file_name, "w")) == NULL) {
                ERROR(ERR_TMP_FOPENWRITE, NULL);
                return false;
        }

        if (!archive_write(tmp_file, *stream)) {
                ERROR(ERR_WRITEARCHIVE, NULL);
                fclose(tmp_file);
                remove(tmp_file_name);
                return false;
        }

        fflush(tmp_file);
        fclose(tmp_file);

        if (!archive_open(&archive, tmp_file_name)) {
                ERROR(ERR_ARCHIVEOPEN, NULL);
                return false;
        }

        bool has_index;
        archive_has_query_index_string_id_to_offset(&has_index, &archive);
        if (has_index) {
                ERROR(ERR_INTERNALERR, NULL);
                remove(tmp_file_name);
                return false;
        }

        struct sid_to_offset *index;
        query query;
        query_create(&query, &archive);
        query_create_index_string_id_to_offset(&index, &query);
        query_drop(&query);
        archive_close(&archive);

        if ((tmp_file = fopen(tmp_file_name, "rb+")) == NULL) {
                ERROR(ERR_TMP_FOPENWRITE, NULL);
                return false;
        }

        fseek(tmp_file, 0, SEEK_END);
        offset_t index_pos = ftell(tmp_file);
        query_index_id_to_offset_serialize(tmp_file, index);
        offset_t file_length = ftell(tmp_file);
        fseek(tmp_file, 0, SEEK_SET);

        archive_header header;
        size_t nread = fread(&header, sizeof(archive_header), 1, tmp_file);
        ERROR_IF_AND_RETURN(nread != 1, ERR_FREAD_FAILED, NULL);
        header.string_id_to_offset_index_offset = index_pos;
        fseek(tmp_file, 0, SEEK_SET);
        int nwrite = fwrite(&header, sizeof(archive_header), 1, tmp_file);
        ERROR_IF_AND_RETURN(nwrite != 1, ERR_FWRITE_FAILED, NULL);
        fseek(tmp_file, 0, SEEK_SET);

        query_drop_index_string_id_to_offset(index);

        MEMBLOCK_DROP(*stream);
        MEMBLOCK_FROM_FILE(stream, tmp_file, file_length);

        remove(tmp_file_name);

        return true;
}

bool archive_from_model(memblock **stream, column_doc *model, packer_e compressor,
                        bool bake_string_id_index, archive_callback *callback)
{
        OPTIONAL_CALL(callback, begin_create_from_model)

        MEMBLOCK_CREATE(stream, 1024 * 1024 * 1024);
        memfile memfile;
        MEMFILE_OPEN(&memfile, *stream, READ_WRITE);

        OPTIONAL_CALL(callback, begin_write_string_table);
        skip_file_header(&memfile);
        if (!serialize_string_dic(&memfile, model->bulk, compressor)) {
                return false;
        }
        OPTIONAL_CALL(callback, end_write_string_table);

        OPTIONAL_CALL(callback, begin_write_record_table);
        offset_t record_header_offset = skip_record_header(&memfile);
        update_file_header(&memfile, record_header_offset);
        offset_t root_object_header_offset = MEMFILE_TELL(&memfile);
        if (!__serialize(NULL, &memfile, &model->columndoc, root_object_header_offset)) {
                return false;
        }
        u64 record_size = MEMFILE_TELL(&memfile) - (record_header_offset + sizeof(record_header));
        update_record_header(&memfile, record_header_offset, model, record_size);
        OPTIONAL_CALL(callback, end_write_record_table);

        MEMFILE_SHRINK(&memfile);

        if (bake_string_id_index) {
                /** create str_buf id to offset index, and append it to the CARBON file */
                OPTIONAL_CALL(callback, begin_string_id_index_baking);
                if (!run_string_id_baking(stream)) {
                        return false;
                }
                OPTIONAL_CALL(callback, end_string_id_index_baking);
        } else {
                OPTIONAL_CALL(callback, skip_string_id_index_baking);
        }

        OPTIONAL_CALL(callback, end_create_from_model)

        return true;
}

archive_io_context *archive_io_context_create(archive *archive)
{
        archive_io_context *context;
        if (io_context_create(&context, archive->disk_file_path)) {
                return context;
        } else {
                ERROR(ERR_IO, NULL);
                return NULL;
        }
}

bool archive_write(FILE *file, const memblock *stream)
{
        return MEMBLOCK_WRITE_TO_FILE(file, stream);
}

bool archive_load(memblock **stream, FILE *file)
{
        long start = ftell(file);
        fseek(file, 0, SEEK_END);
        long end = ftell(file);
        fseek(file, start, SEEK_SET);
        long fileSize = (end - start);

        return MEMBLOCK_FROM_FILE(stream, file, fileSize);
}

bool archive_print(FILE *file, memblock *stream)
{
        memfile memfile;
        MEMFILE_OPEN(&memfile, stream, READ_ONLY);
        if (MEMFILE_SIZE(&memfile)
            < sizeof(archive_header) + sizeof(string_table_header) +
              sizeof(object_header)) {
                ERROR(ERR_NOCARBONSTREAM, NULL);
                return false;
        } else {
                return print_archive_from_memfile(file, &memfile);
        }
}

bool _archive_print_object(FILE *file, memfile *memfile, unsigned nesting_level);

static u32 flags_to_int32(object_flags_u *flags)
{
        return *((i32 *) flags);
}

static const char *array_value_type_to_string(archive_field_e type)
{
        switch (type) {
                case ARCHIVE_FIELD_NULL:
                        return "Null Array";
                case ARCHIVE_FIELD_BOOLEAN:
                        return "Boolean Array";
                case ARCHIVE_FIELD_INT8:
                        return "Int8 Array";
                case ARCHIVE_FIELD_INT16:
                        return "Int16 Array";
                case ARCHIVE_FIELD_INT32:
                        return "Int32 Array";
                case ARCHIVE_FIELD_INT64:
                        return "Int64 Array";
                case ARCHIVE_FIELD_UINT8:
                        return "UInt8 Array";
                case ARCHIVE_FIELD_UINT16:
                        return "UInt16 Array";
                case ARCHIVE_FIELD_UINT32:
                        return "UInt32 Array";
                case ARCHIVE_FIELD_UINT64:
                        return "UInt64 Array";
                case ARCHIVE_FIELD_FLOAT:
                        return "UIntFloat Array";
                case ARCHIVE_FIELD_STRING:
                        return "Text Array";
                case ARCHIVE_FIELD_OBJECT:
                        return "Object Array";
                default: {
                        ERROR(ERR_NOVALUESTR, NULL);
                        return NULL;
                }
        }
}

static void
write_primitive_key_column(memfile *memfile, vec ofType(archive_field_sid_t) *keys)
{
        archive_field_sid_t *string_ids = VEC_ALL(keys, archive_field_sid_t);
        MEMFILE_WRITE(memfile, string_ids, keys->num_elems * sizeof(archive_field_sid_t));
}

static offset_t skip_var_value_offset_column(memfile *memfile, size_t num_keys)
{
        offset_t result = MEMFILE_TELL(memfile);
        MEMFILE_SKIP(memfile, num_keys * sizeof(offset_t));
        return result;
}

static void write_var_value_offset_column(memfile *file, offset_t where, offset_t after,
                                          const offset_t *values,
                                          size_t n)
{
        MEMFILE_SEEK(file, where);
        MEMFILE_WRITE(file, values, n * sizeof(offset_t));
        MEMFILE_SEEK(file, after);
}

static bool
write_primitive_fixed_value_column(memfile *memfile, archive_field_e type,
                                   vec ofType(T) *values_vec)
{
        assert (type != ARCHIVE_FIELD_OBJECT); /** use 'write_primitive_var_value_column' instead */

        switch (type) {
                case ARCHIVE_FIELD_NULL:
                        break;
                case ARCHIVE_FIELD_BOOLEAN: WRITE_PRIMITIVE_VALUES(memfile, values_vec, archive_field_boolean_t);
                        break;
                case ARCHIVE_FIELD_INT8: WRITE_PRIMITIVE_VALUES(memfile, values_vec, archive_field_i8_t);
                        break;
                case ARCHIVE_FIELD_INT16: WRITE_PRIMITIVE_VALUES(memfile, values_vec, archive_field_i16_t);
                        break;
                case ARCHIVE_FIELD_INT32: WRITE_PRIMITIVE_VALUES(memfile, values_vec, archive_field_i32_t);
                        break;
                case ARCHIVE_FIELD_INT64: WRITE_PRIMITIVE_VALUES(memfile, values_vec, archive_field_i64_t);
                        break;
                case ARCHIVE_FIELD_UINT8: WRITE_PRIMITIVE_VALUES(memfile, values_vec, archive_field_u8_t);
                        break;
                case ARCHIVE_FIELD_UINT16: WRITE_PRIMITIVE_VALUES(memfile, values_vec, archive_field_u16_t);
                        break;
                case ARCHIVE_FIELD_UINT32: WRITE_PRIMITIVE_VALUES(memfile, values_vec, archive_field_u32_t);
                        break;
                case ARCHIVE_FIELD_UINT64: WRITE_PRIMITIVE_VALUES(memfile, values_vec, archive_field_u64_t);
                        break;
                case ARCHIVE_FIELD_FLOAT: WRITE_PRIMITIVE_VALUES(memfile, values_vec, archive_field_number_t);
                        break;
                case ARCHIVE_FIELD_STRING: WRITE_PRIMITIVE_VALUES(memfile, values_vec, archive_field_sid_t);
                        break;
                default: ERROR(ERR_NOTYPE, NULL);
                        return false;
        }
        return true;
}

static offset_t *__write_primitive_column(memfile *memfile,
                                              vec ofType(column_doc_obj) *values_vec,
                                              offset_t root_offset)
{
        offset_t *result = MALLOC(values_vec->num_elems * sizeof(offset_t));
        column_doc_obj *mapped = VEC_ALL(values_vec, column_doc_obj);
        for (u32 i = 0; i < values_vec->num_elems; i++) {
                column_doc_obj *obj = mapped + i;
                result[i] = MEMFILE_TELL(memfile) - root_offset;
                if (!__serialize(NULL, memfile, obj, root_offset)) {
                        return NULL;
                }
        }
        return result;
}

static bool __write_array_len_column(memfile *memfile, archive_field_e type,
                                     vec ofType(...) *values)
{
        switch (type) {
                case ARCHIVE_FIELD_NULL:
                        break;
                case ARCHIVE_FIELD_BOOLEAN:
                case ARCHIVE_FIELD_INT8:
                case ARCHIVE_FIELD_INT16:
                case ARCHIVE_FIELD_INT32:
                case ARCHIVE_FIELD_INT64:
                case ARCHIVE_FIELD_UINT8:
                case ARCHIVE_FIELD_UINT16:
                case ARCHIVE_FIELD_UINT32:
                case ARCHIVE_FIELD_UINT64:
                case ARCHIVE_FIELD_FLOAT:
                case ARCHIVE_FIELD_STRING:
                        for (u32 i = 0; i < values->num_elems; i++) {
                                vec *arrays = VEC_GET(values, i, vec);
                                MEMFILE_WRITE(memfile, &arrays->num_elems, sizeof(u32));
                        }
                        break;
                case ARCHIVE_FIELD_OBJECT:
                        return ERROR(ERR_ILLEGALIMPL, NULL);
                default:
                        return ERROR(ERR_NOTYPE, NULL);
        }
        return true;
}

static bool write_array_value_column(memfile *memfile, archive_field_e type,
                                     vec ofType(...) *values_vec)
{

        switch (type) {
                case ARCHIVE_FIELD_NULL: WRITE_PRIMITIVE_VALUES(memfile, values_vec, u32);
                        break;
                case ARCHIVE_FIELD_BOOLEAN: WRITE_ARRAY_VALUES(memfile, values_vec, archive_field_boolean_t);
                        break;
                case ARCHIVE_FIELD_INT8: WRITE_ARRAY_VALUES(memfile, values_vec, archive_field_i8_t);
                        break;
                case ARCHIVE_FIELD_INT16: WRITE_ARRAY_VALUES(memfile, values_vec, archive_field_i16_t);
                        break;
                case ARCHIVE_FIELD_INT32: WRITE_ARRAY_VALUES(memfile, values_vec, archive_field_i32_t);
                        break;
                case ARCHIVE_FIELD_INT64: WRITE_ARRAY_VALUES(memfile, values_vec, archive_field_i64_t);
                        break;
                case ARCHIVE_FIELD_UINT8: WRITE_ARRAY_VALUES(memfile, values_vec, archive_field_u64_t);
                        break;
                case ARCHIVE_FIELD_UINT16: WRITE_ARRAY_VALUES(memfile, values_vec, archive_field_u16_t);
                        break;
                case ARCHIVE_FIELD_UINT32: WRITE_ARRAY_VALUES(memfile, values_vec, archive_field_u32_t);
                        break;
                case ARCHIVE_FIELD_UINT64: WRITE_ARRAY_VALUES(memfile, values_vec, archive_field_u64_t);
                        break;
                case ARCHIVE_FIELD_FLOAT: WRITE_ARRAY_VALUES(memfile, values_vec, archive_field_number_t);
                        break;
                case ARCHIVE_FIELD_STRING: WRITE_ARRAY_VALUES(memfile, values_vec, archive_field_sid_t);
                        break;
                case ARCHIVE_FIELD_OBJECT:
                        return ERROR(ERR_NOTIMPL, NULL);
                default:
                        return ERROR(ERR_NOTYPE, NULL);
        }
        return true;
}

static bool write_array_prop(offset_t *offset, memfile *memfile,
                             vec ofType(archive_field_sid_t) *keys, archive_field_e type,
                             vec ofType(...) *values,
                             offset_t root_object_header_offset)
{
        assert(keys->num_elems == values->num_elems);

        if (keys->num_elems > 0) {
                prop_header header =
                        {.marker = global_marker_symbols[global_value_array_marker_mapping[type].marker].symbol, .num_entries = keys
                                ->num_elems};
                offset_t prop_ofOffset = MEMFILE_TELL(memfile);
                MEMFILE_WRITE(memfile, &header, sizeof(prop_header));

                write_primitive_key_column(memfile, keys);
                if (!__write_array_len_column(memfile, type, values)) {
                        return false;
                }
                if (!write_array_value_column(memfile, type, values)) {
                        return false;
                }
                *offset = (prop_ofOffset - root_object_header_offset);
        } else {
                *offset = 0;
        }
        return true;
}

static bool write_array_props(memfile *memfile, column_doc_obj *columndoc,
                              archive_prop_offs *offsets, offset_t root_object_header_offset)
{
        if (!write_array_prop(&offsets->null_arrays,
                              memfile,
                              &columndoc->null_array_prop_keys,
                              ARCHIVE_FIELD_NULL,
                              &columndoc->null_array_prop_vals,
                              root_object_header_offset)) {
                return false;
        }
        if (!write_array_prop(&offsets->bool_arrays,
                              memfile,
                              &columndoc->bool_array_prop_keys,
                              ARCHIVE_FIELD_BOOLEAN,
                              &columndoc->bool_array_prop_vals,
                              root_object_header_offset)) {
                return false;
        }
        if (!write_array_prop(&offsets->int8_arrays,
                              memfile,
                              &columndoc->int8_array_prop_keys,
                              ARCHIVE_FIELD_INT8,
                              &columndoc->int8_array_prop_vals,
                              root_object_header_offset)) {
                return false;
        }
        if (!write_array_prop(&offsets->int16_arrays,
                              memfile,
                              &columndoc->int16_array_prop_keys,
                              ARCHIVE_FIELD_INT16,
                              &columndoc->int16_array_prop_vals,
                              root_object_header_offset)) {
                return false;
        }
        if (!write_array_prop(&offsets->int32_arrays,
                              memfile,
                              &columndoc->int32_array_prop_keys,
                              ARCHIVE_FIELD_INT32,
                              &columndoc->int32_array_prop_vals,
                              root_object_header_offset)) {
                return false;
        }
        if (!write_array_prop(&offsets->int64_arrays,
                              memfile,
                              &columndoc->int64_array_prop_keys,
                              ARCHIVE_FIELD_INT64,
                              &columndoc->int64_array_prop_vals,
                              root_object_header_offset)) {
                return false;
        }
        if (!write_array_prop(&offsets->uint8_arrays,
                              memfile,
                              &columndoc->uint8_array_prop_keys,
                              ARCHIVE_FIELD_UINT8,
                              &columndoc->uint8_array_prop_vals,
                              root_object_header_offset)) {
                return false;
        }
        if (!write_array_prop(&offsets->uint16_arrays,
                              memfile,
                              &columndoc->uint16_array_prop_keys,
                              ARCHIVE_FIELD_UINT16,
                              &columndoc->uint16_array_prop_vals,
                              root_object_header_offset)) {
                return false;
        }
        if (!write_array_prop(&offsets->uint32_arrays,
                              memfile,
                              &columndoc->uint32_array_prop_keys,
                              ARCHIVE_FIELD_UINT32,
                              &columndoc->uint32_array_prop_vals,
                              root_object_header_offset)) {
                return false;
        }
        if (!write_array_prop(&offsets->uint64_arrays,
                              memfile,
                              &columndoc->uint64_array_prop_keys,
                              ARCHIVE_FIELD_UINT64,
                              &columndoc->ui64_array_prop_vals,
                              root_object_header_offset)) {
                return false;
        }
        if (!write_array_prop(&offsets->float_arrays,
                              memfile,
                              &columndoc->float_array_prop_keys,
                              ARCHIVE_FIELD_FLOAT,
                              &columndoc->float_array_prop_vals,
                              root_object_header_offset)) {
                return false;
        }
        if (!write_array_prop(&offsets->string_arrays,
                              memfile,
                              &columndoc->string_array_prop_keys,
                              ARCHIVE_FIELD_STRING,
                              &columndoc->string_array_prop_vals,
                              root_object_header_offset)) {
                return false;
        }
        return true;
}

/** Fixed-length property lists; value position can be determined by size of value and position of key in key column.
 * In contrast, variable-length property list require an additional offset column (see 'write_var_props') */
static bool write_fixed_props(offset_t *offset, memfile *memfile,
                              vec ofType(archive_field_sid_t) *keys, archive_field_e type,
                              vec ofType(T) *values)
{
        assert(!values || keys->num_elems == values->num_elems);
        assert(type != ARCHIVE_FIELD_OBJECT); /** use 'write_var_props' instead */

        if (keys->num_elems > 0) {
                prop_header header =
                        {.marker = global_marker_symbols[valueMarkerMapping[type].marker].symbol, .num_entries = keys
                                ->num_elems};

                offset_t prop_ofOffset = MEMFILE_TELL(memfile);
                MEMFILE_WRITE(memfile, &header, sizeof(prop_header));

                write_primitive_key_column(memfile, keys);
                if (!write_primitive_fixed_value_column(memfile, type, values)) {
                        return false;
                }
                *offset = prop_ofOffset;
        } else {
                *offset = 0;
        }
        return true;
}

/** Variable-length property lists; value position cannot be determined by position of key in key column, since single
 * value has unknown size. Hence, a dedicated offset column is added to these properties allowing to seek directly
 * to a particular property. Due to the move of strings (i.e., variable-length values) to a dedicated string table,
 * the only variable-length value for properties are "JSON objects".
 * In contrast, fixed-length property list doesn't require an additional offset column (see 'write_fixed_props') */
static bool write_var_props(offset_t *offset, memfile *memfile,
                            vec ofType(archive_field_sid_t) *keys,
                            vec ofType(column_doc_obj) *objects,
                            offset_t root_object_header_offset)
{
        assert(!objects || keys->num_elems == objects->num_elems);

        if (keys->num_elems > 0) {
                prop_header header = {.marker = MARKER_SYMBOL_PROP_OBJECT, .num_entries = keys->num_elems};

                offset_t prop_ofOffset = MEMFILE_TELL(memfile);
                MEMFILE_WRITE(memfile, &header, sizeof(prop_header));

                write_primitive_key_column(memfile, keys);
                offset_t value_offset = skip_var_value_offset_column(memfile, keys->num_elems);
                offset_t *value_offsets = __write_primitive_column(memfile, objects,
                                                                       root_object_header_offset);
                if (!value_offsets) {
                        return false;
                }

                offset_t last = MEMFILE_TELL(memfile);
                write_var_value_offset_column(memfile, value_offset, last, value_offsets, keys->num_elems);
                free(value_offsets);
                *offset = prop_ofOffset;
        } else {
                *offset = 0;
        }
        return true;
}

static bool
write_primitive_props(memfile *memfile, column_doc_obj *columndoc,
                      archive_prop_offs *offsets, offset_t root_object_header_offset)
{
        if (!write_fixed_props(&offsets->nulls, memfile, &columndoc->null_prop_keys, ARCHIVE_FIELD_NULL, NULL)) {
                return false;
        }
        if (!write_fixed_props(&offsets->bools,
                               memfile,
                               &columndoc->bool_prop_keys,
                               ARCHIVE_FIELD_BOOLEAN,
                               &columndoc->bool_prop_vals)) {
                return false;
        }
        if (!write_fixed_props(&offsets->int8s,
                               memfile,
                               &columndoc->int8_prop_keys,
                               ARCHIVE_FIELD_INT8,
                               &columndoc->int8_prop_vals)) {
                return false;
        }
        if (!write_fixed_props(&offsets->int16s,
                               memfile,
                               &columndoc->int16_prop_keys,
                               ARCHIVE_FIELD_INT16,
                               &columndoc->int16_prop_vals)) {
                return false;
        }
        if (!write_fixed_props(&offsets->int32s,
                               memfile,
                               &columndoc->int32_prop_keys,
                               ARCHIVE_FIELD_INT32,
                               &columndoc->int32_prop_vals)) {
                return false;
        }
        if (!write_fixed_props(&offsets->int64s,
                               memfile,
                               &columndoc->int64_prop_keys,
                               ARCHIVE_FIELD_INT64,
                               &columndoc->int64_prop_vals)) {
                return false;
        }
        if (!write_fixed_props(&offsets->uint8s,
                               memfile,
                               &columndoc->uint8_prop_keys,
                               ARCHIVE_FIELD_UINT8,
                               &columndoc->uint8_prop_vals)) {
                return false;
        }
        if (!write_fixed_props(&offsets->uint16s,
                               memfile,
                               &columndoc->uint16_prop_keys,
                               ARCHIVE_FIELD_UINT16,
                               &columndoc->uint16_prop_vals)) {
                return false;
        }
        if (!write_fixed_props(&offsets->uint32s,
                               memfile,
                               &columndoc->uin32_prop_keys,
                               ARCHIVE_FIELD_UINT32,
                               &columndoc->uint32_prop_vals)) {
                return false;
        }
        if (!write_fixed_props(&offsets->uint64s,
                               memfile,
                               &columndoc->uint64_prop_keys,
                               ARCHIVE_FIELD_UINT64,
                               &columndoc->uint64_prop_vals)) {
                return false;
        }
        if (!write_fixed_props(&offsets->floats,
                               memfile,
                               &columndoc->float_prop_keys,
                               ARCHIVE_FIELD_FLOAT,
                               &columndoc->float_prop_vals)) {
                return false;
        }
        if (!write_fixed_props(&offsets->strings,
                               memfile,
                               &columndoc->string_prop_keys,
                               ARCHIVE_FIELD_STRING,
                               &columndoc->string_prop_vals)) {
                return false;
        }
        if (!write_var_props(&offsets->objects,
                             memfile,
                             &columndoc->obj_prop_keys,
                             &columndoc->obj_prop_vals,
                             root_object_header_offset)) {
                return false;
        }

        offsets->nulls -= root_object_header_offset;
        offsets->bools -= root_object_header_offset;
        offsets->int8s -= root_object_header_offset;
        offsets->int16s -= root_object_header_offset;
        offsets->int32s -= root_object_header_offset;
        offsets->int64s -= root_object_header_offset;
        offsets->uint8s -= root_object_header_offset;
        offsets->uint16s -= root_object_header_offset;
        offsets->uint32s -= root_object_header_offset;
        offsets->uint64s -= root_object_header_offset;
        offsets->floats -= root_object_header_offset;
        offsets->strings -= root_object_header_offset;
        offsets->objects -= root_object_header_offset;
        return true;
}

static bool write_column_entry(memfile *memfile, archive_field_e type,
                               vec ofType(<T>) *column, offset_t root_object_header_offset)
{
        MEMFILE_WRITE(memfile, &column->num_elems, sizeof(u32));
        switch (type) {
                case ARCHIVE_FIELD_NULL:
                        MEMFILE_WRITE(memfile, column->base, column->num_elems * sizeof(u32));
                        break;
                case ARCHIVE_FIELD_BOOLEAN:
                case ARCHIVE_FIELD_INT8:
                case ARCHIVE_FIELD_INT16:
                case ARCHIVE_FIELD_INT32:
                case ARCHIVE_FIELD_INT64:
                case ARCHIVE_FIELD_UINT8:
                case ARCHIVE_FIELD_UINT16:
                case ARCHIVE_FIELD_UINT32:
                case ARCHIVE_FIELD_UINT64:
                case ARCHIVE_FIELD_FLOAT:
                case ARCHIVE_FIELD_STRING:
                        MEMFILE_WRITE(memfile, column->base, column->num_elems * GET_TYPE_SIZE(type));
                        break;
                case ARCHIVE_FIELD_OBJECT: {
                        offset_t preObjectNext = 0;
                        for (size_t i = 0; i < column->num_elems; i++) {
                                column_doc_obj *object = VEC_GET(column, i, column_doc_obj);
                                if (LIKELY(preObjectNext != 0)) {
                                        offset_t continuePos = MEMFILE_TELL(memfile);
                                        offset_t relativeContinuePos = continuePos - root_object_header_offset;
                                        MEMFILE_SEEK(memfile, preObjectNext);
                                        MEMFILE_WRITE(memfile, &relativeContinuePos, sizeof(offset_t));
                                        MEMFILE_SEEK(memfile, continuePos);
                                }
                                if (!__serialize(&preObjectNext, memfile, object, root_object_header_offset)) {
                                        return false;
                                }
                        }
                }
                        break;
                default:
                        return ERROR(ERR_NOTYPE, NULL);
        }
        return true;
}

static bool write_column(memfile *memfile, column_doc_column *column,
                         offset_t root_object_header_offset)
{
        assert(column->array_positions.num_elems == column->values.num_elems);

        column_header header = {.marker = global_marker_symbols[MARKER_TYPE_COLUMN].symbol, .column_name = column
                ->key_name, .value_type = global_marker_symbols[global_value_array_marker_mapping[column->type].marker]
                .symbol, .num_entries = column->values.num_elems};

        MEMFILE_WRITE(memfile, &header, sizeof(column_header));

        /** skip offset column to value entry points */
        offset_t value_entry_offsets = MEMFILE_TELL(memfile);
        MEMFILE_SKIP(memfile, column->values.num_elems * sizeof(offset_t));

        MEMFILE_WRITE(memfile, column->array_positions.base, column->array_positions.num_elems * sizeof(u32));

        for (size_t i = 0; i < column->values.num_elems; i++) {
                vec ofType(<T>) *column_data = VEC_GET(&column->values, i, vec);
                offset_t column_entry_offset = MEMFILE_TELL(memfile);
                offset_t relative_entry_offset = column_entry_offset - root_object_header_offset;
                MEMFILE_SEEK(memfile, value_entry_offsets + i * sizeof(offset_t));
                MEMFILE_WRITE(memfile, &relative_entry_offset, sizeof(offset_t));
                MEMFILE_SEEK(memfile, column_entry_offset);
                if (!write_column_entry(memfile, column->type, column_data, root_object_header_offset)) {
                        return false;
                }
        }
        return true;
}

static bool write_object_array_props(memfile *memfile,
                                     vec ofType(column_doc_group) *object_key_columns,
                                     archive_prop_offs *offsets,
                                     offset_t root_object_header_offset)
{
        if (object_key_columns->num_elems > 0) {
                object_array_header header = {.marker = global_marker_symbols[MARKER_TYPE_PROP_OBJECT_ARRAY]
                        .symbol, .num_entries = object_key_columns->num_elems};

                offsets->object_arrays = MEMFILE_TELL(memfile) - root_object_header_offset;
                MEMFILE_WRITE(memfile, &header, sizeof(object_array_header));

                for (size_t i = 0; i < object_key_columns->num_elems; i++) {
                        column_doc_group *column_group = VEC_GET(object_key_columns, i,
                                                                            column_doc_group);
                        MEMFILE_WRITE(memfile, &column_group->key, sizeof(archive_field_sid_t));
                }

                // skip offset column to column groups
                offset_t column_offsets = MEMFILE_TELL(memfile);
                MEMFILE_SKIP(memfile, object_key_columns->num_elems * sizeof(offset_t));

                for (size_t i = 0; i < object_key_columns->num_elems; i++) {
                        column_doc_group *column_group = VEC_GET(object_key_columns, i,
                                                                            column_doc_group);
                        offset_t this_column_offset_relative = MEMFILE_TELL(memfile) - root_object_header_offset;

                        /** write an object-id for each position number */
                        size_t max_pos = 0;
                        for (size_t k = 0; k < column_group->columns.num_elems; k++) {
                                column_doc_column
                                        *column = VEC_GET(&column_group->columns, k, column_doc_column);
                                const u32 *array_pos = VEC_ALL(&column->array_positions, u32);
                                for (size_t m = 0; m < column->array_positions.num_elems; m++) {
                                        max_pos = JAK_MAX(max_pos, array_pos[m]);
                                }
                        }
                        column_group_header column_group_header =
                                {.marker = global_marker_symbols[MARKER_TYPE_COLUMN_GROUP].symbol, .num_columns = column_group
                                        ->columns.num_elems, .num_objects = max_pos + 1};
                        MEMFILE_WRITE(memfile, &column_group_header, sizeof(column_group_header));

                        for (size_t i = 0; i < column_group_header.num_objects; i++) {
                                unique_id_t oid;
                                if (!unique_id_create(&oid)) {
                                        ERROR(ERR_THREADOOOBJIDS, NULL);
                                        return false;
                                }
                                MEMFILE_WRITE(memfile, &oid, sizeof(unique_id_t));
                        }

                        offset_t continue_write = MEMFILE_TELL(memfile);
                        MEMFILE_SEEK(memfile, column_offsets + i * sizeof(offset_t));
                        MEMFILE_WRITE(memfile, &this_column_offset_relative, sizeof(offset_t));
                        MEMFILE_SEEK(memfile, continue_write);

                        offset_t offset_column_to_columns = continue_write;
                        MEMFILE_SKIP(memfile, column_group->columns.num_elems * sizeof(offset_t));

                        for (size_t k = 0; k < column_group->columns.num_elems; k++) {
                                column_doc_column
                                        *column = VEC_GET(&column_group->columns, k, column_doc_column);
                                offset_t continue_write = MEMFILE_TELL(memfile);
                                offset_t column_off = continue_write - root_object_header_offset;
                                MEMFILE_SEEK(memfile, offset_column_to_columns + k * sizeof(offset_t));
                                MEMFILE_WRITE(memfile, &column_off, sizeof(offset_t));
                                MEMFILE_SEEK(memfile, continue_write);
                                if (!write_column(memfile, column, root_object_header_offset)) {
                                        return false;
                                }
                        }

                }
        } else {
                offsets->object_arrays = 0;
        }

        return true;
}

static offset_t skip_record_header(memfile *memfile)
{
        offset_t offset = MEMFILE_TELL(memfile);
        MEMFILE_SKIP(memfile, sizeof(record_header));
        return offset;
}

static void
update_record_header(memfile *memfile, offset_t root_object_header_offset, column_doc *model,
                     u64 record_size)
{
        record_flags flags = {.bits.is_sorted = model->read_optimized};
        record_header
                header = {.marker = MARKER_SYMBOL_RECORD_HEADER, .flags = flags.value, .record_size = record_size};
        offset_t offset;
        MEMFILE_GET_OFFSET(&offset, memfile);
        MEMFILE_SEEK(memfile, root_object_header_offset);
        MEMFILE_WRITE(memfile, &header, sizeof(record_header));
        MEMFILE_SEEK(memfile, offset);
}

static void propOffsetsWrite(memfile *memfile, const object_flags_u *flags,
                             archive_prop_offs *prop_offsets)
{
        if (flags->bits.has_null_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->nulls, sizeof(offset_t));
        }
        if (flags->bits.has_bool_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->bools, sizeof(offset_t));
        }
        if (flags->bits.has_int8_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->int8s, sizeof(offset_t));
        }
        if (flags->bits.has_int16_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->int16s, sizeof(offset_t));
        }
        if (flags->bits.has_int32_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->int32s, sizeof(offset_t));
        }
        if (flags->bits.has_int64_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->int64s, sizeof(offset_t));
        }
        if (flags->bits.has_uint8_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->uint8s, sizeof(offset_t));
        }
        if (flags->bits.has_uint16_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->uint16s, sizeof(offset_t));
        }
        if (flags->bits.has_uint32_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->uint32s, sizeof(offset_t));
        }
        if (flags->bits.has_uint64_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->uint64s, sizeof(offset_t));
        }
        if (flags->bits.has_float_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->floats, sizeof(offset_t));
        }
        if (flags->bits.has_string_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->strings, sizeof(offset_t));
        }
        if (flags->bits.has_object_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->objects, sizeof(offset_t));
        }
        if (flags->bits.has_null_array_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->null_arrays, sizeof(offset_t));
        }
        if (flags->bits.has_bool_array_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->bool_arrays, sizeof(offset_t));
        }
        if (flags->bits.has_int8_array_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->int8_arrays, sizeof(offset_t));
        }
        if (flags->bits.has_int16_array_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->int16_arrays, sizeof(offset_t));
        }
        if (flags->bits.has_int32_array_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->int32_arrays, sizeof(offset_t));
        }
        if (flags->bits.has_int64_array_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->int64_arrays, sizeof(offset_t));
        }
        if (flags->bits.has_uint8_array_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->uint8_arrays, sizeof(offset_t));
        }
        if (flags->bits.has_uint16_array_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->uint16_arrays, sizeof(offset_t));
        }
        if (flags->bits.has_uint32_array_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->uint32_arrays, sizeof(offset_t));
        }
        if (flags->bits.has_uint64_array_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->uint64_arrays, sizeof(offset_t));
        }
        if (flags->bits.has_float_array_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->float_arrays, sizeof(offset_t));
        }
        if (flags->bits.has_string_array_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->string_arrays, sizeof(offset_t));
        }
        if (flags->bits.has_object_array_props) {
                MEMFILE_WRITE(memfile, &prop_offsets->object_arrays, sizeof(offset_t));
        }
}

static void prop_offsets_skip_write(memfile *memfile, const object_flags_u *flags)
{
        unsigned num_skip_offset_bytes = 0;
        if (flags->bits.has_null_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_bool_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_int8_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_int16_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_int32_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_int64_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_uint8_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_uint16_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_uint32_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_uint64_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_float_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_string_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_object_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_null_array_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_bool_array_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_int8_array_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_int16_array_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_int32_array_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_int64_array_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_uint8_array_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_uint16_array_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_uint32_array_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_uint64_array_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_float_array_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_string_array_props) {
                num_skip_offset_bytes++;
        }
        if (flags->bits.has_object_array_props) {
                num_skip_offset_bytes++;
        }

        MEMFILE_SKIP(memfile, num_skip_offset_bytes * sizeof(offset_t));
}

static bool __serialize(offset_t *offset, memfile *memfile,
                        column_doc_obj *columndoc,
                        offset_t root_object_header_offset)
{
        object_flags_u flags;
        archive_prop_offs prop_offsets;
        get_flags(&flags, columndoc);

        offset_t header_offset = MEMFILE_TELL(memfile);
        MEMFILE_SKIP(memfile, sizeof(object_header));

        prop_offsets_skip_write(memfile, &flags);
        offset_t next_offset = MEMFILE_TELL(memfile);
        offset_t default_next_nil = 0;
        MEMFILE_WRITE(memfile, &default_next_nil, sizeof(offset_t));

        if (!write_primitive_props(memfile, columndoc, &prop_offsets, root_object_header_offset)) {
                return false;
        }
        if (!write_array_props(memfile, columndoc, &prop_offsets, root_object_header_offset)) {
                return false;
        }
        if (!write_object_array_props(memfile,
                                      &columndoc->obj_array_props,
                                      &prop_offsets,
                                      root_object_header_offset)) {
                return false;
        }

        MEMFILE_WRITE(memfile, &global_marker_symbols[MARKER_TYPE_OBJECT_END].symbol, 1);

        offset_t object_end_offset = MEMFILE_TELL(memfile);
        MEMFILE_SEEK(memfile, header_offset);

        unique_id_t oid;
        if (!unique_id_create(&oid)) {
                ERROR(ERR_THREADOOOBJIDS, NULL);
                return false;
        }

        object_header header =
                {.marker = global_marker_symbols[MARKER_TYPE_OBJECT_BEGIN].symbol, .oid = oid, .flags = flags_to_int32(
                        &flags),

                };

        MEMFILE_WRITE(memfile, &header, sizeof(object_header));

        propOffsetsWrite(memfile, &flags, &prop_offsets);

        MEMFILE_SEEK(memfile, object_end_offset);
        OPTIONAL_SET(offset, next_offset);
        return true;
}

static char *embedded_dic_flags_to_string(const string_tab_flags_u *flags)
{
        size_t max = 2048;
        char *string = MALLOC(max + 1);
        size_t length = 0;

        if (flags->value == 0) {
                strcpy(string, " uncompressed");
                length = strlen(string);
                assert(length <= max);
        } else {

                for (size_t i = 0; i < ARRAY_LENGTH(global_pack_strategy_register); i++) {
                        if (flags->value & global_pack_strategy_register[i].flag_bit) {
                                strcpy(string + length, global_pack_strategy_register[i].name);
                                length = strlen(string);
                                strcpy(string + length, " ");
                                length = strlen(string);
                        }
                }
        }
        string[length] = '\0';
        return string;
}

static char *record_header_flags_to_string(const record_flags *flags)
{
        size_t max = 2048;
        char *string = MALLOC(max + 1);
        size_t length = 0;

        if (flags->value == 0) {
                strcpy(string, " none");
                length = strlen(string);
                assert(length <= max);
        } else {
                if (flags->bits.is_sorted) {
                        strcpy(string + length, " sorted");
                        length = strlen(string);
                        assert(length <= max);
                }
        }
        string[length] = '\0';
        return string;
}

static bool serialize_string_dic(memfile *memfile, const doc_bulk *context, packer_e compressor)
{
        string_tab_flags_u flags;
        packer strategy;
        string_table_header header;

        vec ofType (const char *) *strings;
        vec ofType(archive_field_sid_t) *string_ids;

        doc_bulk_get_dic_contents(&strings, &string_ids, context);

        assert(strings->num_elems == string_ids->num_elems);

        flags.value = 0;
        if (!pack_by_type(&strategy, compressor)) {
                return false;
        }
        u8 flag_bit = pack_flagbit_by_type(compressor);
        SET_BITS(flags.value, flag_bit);

        offset_t header_pos = MEMFILE_TELL(memfile);
        MEMFILE_SKIP(memfile, sizeof(string_table_header));

        offset_t extra_begin_off = MEMFILE_TELL(memfile);
        pack_write_extra(&strategy, memfile, strings);
        offset_t extra_end_off = MEMFILE_TELL(memfile);

        header = (string_table_header) {.marker = global_marker_symbols[MARKER_TYPE_EMBEDDED_STR_DIC]
                .symbol, .flags = flags.value, .num_entries = strings
                ->num_elems, .first_entry = MEMFILE_TELL(memfile), .compressor_extra_size = (extra_end_off
                                                                                             - extra_begin_off)};

        for (size_t i = 0; i < strings->num_elems; i++) {
                archive_field_sid_t id = *VEC_GET(string_ids, i, archive_field_sid_t);
                const char *string = *VEC_GET(strings, i, char *);

                string_entry_header header = {.marker = global_marker_symbols[MARKER_TYPE_EMBEDDED_UNCOMP_STR]
                        .symbol, .next_entry_off = 0, .string_id = id, .string_len = strlen(string)};

                offset_t header_pos_off = MEMFILE_TELL(memfile);
                MEMFILE_SKIP(memfile, sizeof(string_entry_header));

                if (!pack_encode(&strategy, memfile, string)) {
                        error_print(stderr);
                        return false;
                }
                offset_t continue_off = MEMFILE_TELL(memfile);
                MEMFILE_SEEK(memfile, header_pos_off);
                header.next_entry_off = i + 1 < strings->num_elems ? continue_off : 0;
                MEMFILE_WRITE(memfile, &header, sizeof(string_entry_header));
                MEMFILE_SEEK(memfile, continue_off);
        }

        offset_t continue_pos = MEMFILE_TELL(memfile);
        MEMFILE_SEEK(memfile, header_pos);
        MEMFILE_WRITE(memfile, &header, sizeof(string_table_header));
        MEMFILE_SEEK(memfile, continue_pos);

        vec_drop(strings);
        vec_drop(string_ids);
        free(strings);
        free(string_ids);

        return pack_drop(&strategy);
}

static void skip_file_header(memfile *memfile)
{
        MEMFILE_SKIP(memfile, sizeof(archive_header));
}

static void update_file_header(memfile *memfile, offset_t record_header_offset)
{
        offset_t current_pos;
        MEMFILE_GET_OFFSET(&current_pos, memfile);
        MEMFILE_SEEK(memfile, 0);
        memcpy(&this_file_header.magic, CARBON_ARCHIVE_MAGIC, strlen(CARBON_ARCHIVE_MAGIC));
        this_file_header.root_object_header_offset = record_header_offset;
        this_file_header.string_id_to_offset_index_offset = 0;
        MEMFILE_WRITE(memfile, &this_file_header, sizeof(archive_header));
        MEMFILE_SEEK(memfile, current_pos);
}

static bool
print_column_form_memfile(FILE *file, memfile *memfile, unsigned nesting_level)
{
        offset_t offset;
        MEMFILE_GET_OFFSET(&offset, memfile);
        column_header *header = MEMFILE_READ_TYPE(memfile, column_header);
        if (header->marker != MARKER_SYMBOL_COLUMN) {
                char buffer[256];
                sprintf(buffer, "expected marker [%c] but found [%c]", MARKER_SYMBOL_COLUMN, header->marker);
                return ERROR(ERR_CORRUPTED, buffer);
        }
        fprintf(file, "0x%04x ", (unsigned) offset);
        INTENT_LINE(nesting_level);
        const char *type_name = array_value_type_to_string(int_marker_to_field_type(header->value_type));
        if (!type_name) {
                return false;
        }

        fprintf(file,
                "[marker: %c (Column)] [column_name: '%"PRIu64"'] [value_type: %c (%s)] [nentries: %d] [",
                header->marker,
                header->column_name,
                header->value_type,
                type_name,
                header->num_entries);

        for (size_t i = 0; i < header->num_entries; i++) {
                offset_t entry_off = *MEMFILE_READ_TYPE(memfile, offset_t);
                fprintf(file, "offset: 0x%04x%s", (unsigned) entry_off, i + 1 < header->num_entries ? ", " : "");
        }

        u32 *positions = (u32 *) MEMFILE_READ(memfile, header->num_entries * sizeof(u32));
        fprintf(file, "] [positions: [");
        for (size_t i = 0; i < header->num_entries; i++) {
                fprintf(file, "%d%s", positions[i], i + 1 < header->num_entries ? ", " : "");
        }

        fprintf(file, "]]\n");

        archive_field_e data_type = int_marker_to_field_type(header->value_type);

        //fprintf(file, "[");
        for (size_t i = 0; i < header->num_entries; i++) {
                switch (data_type) {
                        case ARCHIVE_FIELD_NULL: {
                                PRINT_VALUE_ARRAY(u32, memfile, header, "%d");
                        }
                                break;
                        case ARCHIVE_FIELD_BOOLEAN: {
                                PRINT_VALUE_ARRAY(archive_field_boolean_t, memfile, header, "%d");
                        }
                                break;
                        case ARCHIVE_FIELD_INT8: {
                                PRINT_VALUE_ARRAY(archive_field_i8_t, memfile, header, "%d");
                        }
                                break;
                        case ARCHIVE_FIELD_INT16: {
                                PRINT_VALUE_ARRAY(archive_field_i16_t, memfile, header, "%d");
                        }
                                break;
                        case ARCHIVE_FIELD_INT32: {
                                PRINT_VALUE_ARRAY(archive_field_i32_t, memfile, header, "%d");
                        }
                                break;
                        case ARCHIVE_FIELD_INT64: {
                                PRINT_VALUE_ARRAY(archive_field_i64_t, memfile, header, "%"
                                        PRIi64);
                        }
                                break;
                        case ARCHIVE_FIELD_UINT8: {
                                PRINT_VALUE_ARRAY(archive_field_u8_t, memfile, header, "%d");
                        }
                                break;
                        case ARCHIVE_FIELD_UINT16: {
                                PRINT_VALUE_ARRAY(archive_field_u16_t, memfile, header, "%d");
                        }
                                break;
                        case ARCHIVE_FIELD_UINT32: {
                                PRINT_VALUE_ARRAY(archive_field_u32_t, memfile, header, "%d");
                        }
                                break;
                        case ARCHIVE_FIELD_UINT64: {
                                PRINT_VALUE_ARRAY(archive_field_u64_t, memfile, header, "%"
                                        PRIu64);
                        }
                                break;
                        case ARCHIVE_FIELD_FLOAT: {
                                PRINT_VALUE_ARRAY(archive_field_number_t, memfile, header, "%f");
                        }
                                break;
                        case ARCHIVE_FIELD_STRING: {
                                PRINT_VALUE_ARRAY(archive_field_sid_t, memfile, header, "%"
                                        PRIu64
                                        "");
                        }
                                break;
                        case ARCHIVE_FIELD_OBJECT: {
                                u32 num_elements = *MEMFILE_READ_TYPE(memfile, u32);
                                INTENT_LINE(nesting_level);
                                fprintf(file, "   [num_elements: %d] [values: [\n", num_elements);
                                for (size_t i = 0; i < num_elements; i++) {
                                        if (!_archive_print_object(file, memfile, nesting_level + 2)) {
                                                return false;
                                        }
                                }
                                INTENT_LINE(nesting_level);
                                fprintf(file, "   ]\n");
                        }
                                break;
                        default:
                                return ERROR(ERR_NOTYPE, NULL);
                }
        }
        return true;
}

static bool _archive_print_object_array_from_memfile(FILE *file, memfile *mem_file,
                                            unsigned nesting_level)
{
        unsigned offset = (unsigned) MEMFILE_TELL(mem_file);
        object_array_header *header = MEMFILE_READ_TYPE(mem_file, object_array_header);
        if (header->marker != MARKER_SYMBOL_PROP_OBJECT_ARRAY) {
                char buffer[256];
                sprintf(buffer, "expected marker [%c] but found [%c]", MARKER_SYMBOL_PROP_OBJECT_ARRAY,
                        header->marker);
                ERROR(ERR_CORRUPTED, buffer);
                return false;
        }

        fprintf(file, "0x%04x ", offset);
        INTENT_LINE(nesting_level);
        fprintf(file, "[marker: %c (Object Array)] [nentries: %d] [", header->marker, header->num_entries);

        for (size_t i = 0; i < header->num_entries; i++) {
                archive_field_sid_t string_id = *MEMFILE_READ_TYPE(mem_file, archive_field_sid_t);
                fprintf(file, "key: %"PRIu64"%s", string_id, i + 1 < header->num_entries ? ", " : "");
        }
        fprintf(file, "] [");
        for (size_t i = 0; i < header->num_entries; i++) {
                offset_t columnGroupOffset = *MEMFILE_READ_TYPE(mem_file, offset_t);
                fprintf(file,
                        "offset: 0x%04x%s",
                        (unsigned) columnGroupOffset,
                        i + 1 < header->num_entries ? ", " : "");
        }

        fprintf(file, "]\n");
        nesting_level++;

        for (size_t i = 0; i < header->num_entries; i++) {
                offset = MEMFILE_TELL(mem_file);
                column_group_header
                        *column_group_header = MEMFILE_READ_TYPE(mem_file, struct column_group_header);
                if (column_group_header->marker != MARKER_SYMBOL_COLUMN_GROUP) {
                        char buffer[256];
                        sprintf(buffer,
                                "expected marker [%c] but found [%c]",
                                MARKER_SYMBOL_COLUMN_GROUP,
                                column_group_header->marker);
                        ERROR(ERR_CORRUPTED, buffer);
                        return false;
                }
                fprintf(file, "0x%04x ", offset);
                INTENT_LINE(nesting_level);
                fprintf(file,
                        "[marker: %c (Column Group)] [num_columns: %d] [num_objects: %d] [object_ids: ",
                        column_group_header->marker,
                        column_group_header->num_columns,
                        column_group_header->num_objects);
                const unique_id_t
                        *oids = MEMFILE_READ_TYPE_LIST(mem_file, unique_id_t,
                                                           column_group_header->num_objects);
                for (size_t k = 0; k < column_group_header->num_objects; k++) {
                        fprintf(file, "%"PRIu64"%s", oids[k], k + 1 < column_group_header->num_objects ? ", " : "");
                }
                fprintf(file, "] [offsets: ");
                for (size_t k = 0; k < column_group_header->num_columns; k++) {
                        offset_t column_off = *MEMFILE_READ_TYPE(mem_file, offset_t);
                        fprintf(file,
                                "0x%04x%s",
                                (unsigned) column_off,
                                k + 1 < column_group_header->num_columns ? ", " : "");
                }

                fprintf(file, "]\n");

                for (size_t k = 0; k < column_group_header->num_columns; k++) {
                        if (!print_column_form_memfile(file, mem_file, nesting_level + 1)) {
                                return false;
                        }
                }

                fprintf(file, "0x%04x ", offset);
                INTENT_LINE(nesting_level);
                fprintf(file, "]\n");
        }
        return true;
}

static void print_prop_offsets(FILE *file, const object_flags_u *flags,
                               const archive_prop_offs *prop_offsets)
{
        if (flags->bits.has_null_props) {
                fprintf(file, " nulls: 0x%04x", (unsigned) prop_offsets->nulls);
        }
        if (flags->bits.has_bool_props) {
                fprintf(file, " bools: 0x%04x", (unsigned) prop_offsets->bools);
        }
        if (flags->bits.has_int8_props) {
                fprintf(file, " int8s: 0x%04x", (unsigned) prop_offsets->int8s);
        }
        if (flags->bits.has_int16_props) {
                fprintf(file, " int16s: 0x%04x", (unsigned) prop_offsets->int16s);
        }
        if (flags->bits.has_int32_props) {
                fprintf(file, " int32s: 0x%04x", (unsigned) prop_offsets->int32s);
        }
        if (flags->bits.has_int64_props) {
                fprintf(file, " int64s: 0x%04x", (unsigned) prop_offsets->int64s);
        }
        if (flags->bits.has_uint8_props) {
                fprintf(file, " uint8s: 0x%04x", (unsigned) prop_offsets->uint8s);
        }
        if (flags->bits.has_uint16_props) {
                fprintf(file, " uint16s: 0x%04x", (unsigned) prop_offsets->uint16s);
        }
        if (flags->bits.has_uint32_props) {
                fprintf(file, " uint32s: 0x%04x", (unsigned) prop_offsets->uint32s);
        }
        if (flags->bits.has_uint64_props) {
                fprintf(file, " uint64s: 0x%04x", (unsigned) prop_offsets->uint64s);
        }
        if (flags->bits.has_float_props) {
                fprintf(file, " floats: 0x%04x", (unsigned) prop_offsets->floats);
        }
        if (flags->bits.has_string_props) {
                fprintf(file, " texts: 0x%04x", (unsigned) prop_offsets->strings);
        }
        if (flags->bits.has_object_props) {
                fprintf(file, " objects: 0x%04x", (unsigned) prop_offsets->objects);
        }
        if (flags->bits.has_null_array_props) {
                fprintf(file, " nullArrays: 0x%04x", (unsigned) prop_offsets->null_arrays);
        }
        if (flags->bits.has_bool_array_props) {
                fprintf(file, " boolArrays: 0x%04x", (unsigned) prop_offsets->bool_arrays);
        }
        if (flags->bits.has_int8_array_props) {
                fprintf(file, " int8Arrays: 0x%04x", (unsigned) prop_offsets->int8_arrays);
        }
        if (flags->bits.has_int16_array_props) {
                fprintf(file, " int16Arrays: 0x%04x", (unsigned) prop_offsets->int16_arrays);
        }
        if (flags->bits.has_int32_array_props) {
                fprintf(file, " int32Arrays: 0x%04x", (unsigned) prop_offsets->int32_arrays);
        }
        if (flags->bits.has_int64_array_props) {
                fprintf(file, " int16Arrays: 0x%04x", (unsigned) prop_offsets->int64_arrays);
        }
        if (flags->bits.has_uint8_array_props) {
                fprintf(file, " uint8Arrays: 0x%04x", (unsigned) prop_offsets->uint8_arrays);
        }
        if (flags->bits.has_uint16_array_props) {
                fprintf(file, " uint16Arrays: 0x%04x", (unsigned) prop_offsets->uint16_arrays);
        }
        if (flags->bits.has_uint32_array_props) {
                fprintf(file, " uint32Arrays: 0x%04x", (unsigned) prop_offsets->uint32_arrays);
        }
        if (flags->bits.has_uint64_array_props) {
                fprintf(file, " uint64Arrays: 0x%04x", (unsigned) prop_offsets->uint64_arrays);
        }
        if (flags->bits.has_float_array_props) {
                fprintf(file, " floatArrays: 0x%04x", (unsigned) prop_offsets->float_arrays);
        }
        if (flags->bits.has_string_array_props) {
                fprintf(file, " textArrays: 0x%04x", (unsigned) prop_offsets->string_arrays);
        }
        if (flags->bits.has_object_array_props) {
                fprintf(file, " objectArrays: 0x%04x", (unsigned) prop_offsets->object_arrays);
        }
}

bool _archive_print_object(FILE *file, memfile *memfile, unsigned nesting_level)
{
        unsigned offset = (unsigned) MEMFILE_TELL(memfile);
        object_header *header = MEMFILE_READ_TYPE(memfile, object_header);

        archive_prop_offs prop_offsets;
        object_flags_u flags = {.value = header->flags};

        int_read_prop_offsets(&prop_offsets, memfile, &flags);
        offset_t nextObjectOrNil = *MEMFILE_READ_TYPE(memfile, offset_t);

        if (header->marker != MARKER_SYMBOL_OBJECT_BEGIN) {
                char buffer[256];
                sprintf(buffer, "Parsing ERROR: expected object marker [{] but found [%c]\"", header->marker);
                ERROR(ERR_CORRUPTED, buffer);
                return false;
        }

        fprintf(file, "0x%04x ", offset);
        INTENT_LINE(nesting_level);
        nesting_level++;
        fprintf(file,
                "[marker: %c (BeginObject)] [object-id: %"PRIu64"] [flags: %u] [propertyOffsets: [",
                header->marker,
                header->oid,
                header->flags);
        print_prop_offsets(file, &flags, &prop_offsets);
        fprintf(file, " ] [next: 0x%04x] \n", (unsigned) nextObjectOrNil);

        bool continue_read = true;
        while (continue_read) {
                offset = MEMFILE_TELL(memfile);
                char entryMarker = *MEMFILE_PEEK_TYPE(memfile, char);

                switch (entryMarker) {
                        case MARKER_SYMBOL_PROP_NULL: {
                                prop_header *prop_header = MEMFILE_READ_TYPE(memfile,
                                                                                                struct prop_header);
                                archive_field_sid_t *keys = (archive_field_sid_t *) MEMFILE_READ(memfile,
                                                                                                             prop_header->num_entries *
                                                                                                             sizeof(archive_field_sid_t));
                                fprintf(file, "0x%04x ", offset);
                                INTENT_LINE(nesting_level)
                                fprintf(file, "[marker: %c (null)] [nentries: %d] [", entryMarker,
                                        prop_header->num_entries);

                                for (u32 i = 0; i < prop_header->num_entries; i++) {
                                        fprintf(file, "%"PRIu64"%s", keys[i],
                                                i + 1 < prop_header->num_entries ? ", " : "");
                                }
                                fprintf(file, "]\n");
                        }
                                break;
                        case MARKER_SYMBOL_PROP_BOOLEAN: {
                                prop_header *prop_header = MEMFILE_READ_TYPE(memfile, struct prop_header);
                                archive_field_sid_t *keys = (archive_field_sid_t *) MEMFILE_READ(memfile,
                                                                                                             prop_header->num_entries *
                                                                                                             sizeof(archive_field_sid_t));
                                archive_field_boolean_t *values = (archive_field_boolean_t *) MEMFILE_READ(
                                        memfile,
                                        prop_header->num_entries *
                                        sizeof(archive_field_boolean_t));
                                fprintf(file, "0x%04x ", offset);
                                INTENT_LINE(nesting_level)
                                fprintf(file, "[marker: %c (boolean)] [nentries: %d] [", entryMarker,
                                        prop_header->num_entries);
                                for (u32 i = 0; i < prop_header->num_entries; i++) {
                                        fprintf(file, "%"PRIu64"%s", keys[i],
                                                i + 1 < prop_header->num_entries ? ", " : "");
                                }
                                fprintf(file, "] [");
                                for (u32 i = 0; i < prop_header->num_entries; i++) {
                                        fprintf(file,
                                                "%s%s",
                                                values[i] ? "true" : "false",
                                                i + 1 < prop_header->num_entries ? ", " : "");
                                }
                                fprintf(file, "]\n");
                        }
                                break;
                        case MARKER_SYMBOL_PROP_INT8: PRINT_SIMPLE_PROPS(file,
                                                                             memfile,
                                                                             MEMFILE_TELL(memfile),
                                                                             nesting_level,
                                                                             archive_field_i8_t,
                                                                             "Int8",
                                                                             "%d");
                                break;
                        case MARKER_SYMBOL_PROP_INT16: PRINT_SIMPLE_PROPS(file,
                                                                              memfile,
                                                                              MEMFILE_TELL(memfile),
                                                                              nesting_level,
                                                                              archive_field_i16_t,
                                                                              "Int16",
                                                                              "%d");
                                break;
                        case MARKER_SYMBOL_PROP_INT32: PRINT_SIMPLE_PROPS(file,
                                                                              memfile,
                                                                              MEMFILE_TELL(memfile),
                                                                              nesting_level,
                                                                              archive_field_i32_t,
                                                                              "Int32",
                                                                              "%d");
                                break;
                        case MARKER_SYMBOL_PROP_INT64: PRINT_SIMPLE_PROPS(file,
                                                                              memfile,
                                                                              MEMFILE_TELL(memfile),
                                                                              nesting_level,
                                                                              archive_field_i64_t,
                                                                              "Int64",
                                                                              "%"
                                                                                      PRIi64);
                                break;
                        case MARKER_SYMBOL_PROP_UINT8: PRINT_SIMPLE_PROPS(file,
                                                                              memfile,
                                                                              MEMFILE_TELL(memfile),
                                                                              nesting_level,
                                                                              archive_field_u8_t,
                                                                              "UInt8",
                                                                              "%d");
                                break;
                        case MARKER_SYMBOL_PROP_UINT16: PRINT_SIMPLE_PROPS(file,
                                                                               memfile,
                                                                               MEMFILE_TELL(memfile),
                                                                               nesting_level,
                                                                               archive_field_u16_t,
                                                                               "UInt16",
                                                                               "%d");
                                break;
                        case MARKER_SYMBOL_PROP_UINT32: PRINT_SIMPLE_PROPS(file,
                                                                               memfile,
                                                                               MEMFILE_TELL(memfile),
                                                                               nesting_level,
                                                                               archive_field_u32_t,
                                                                               "UInt32",
                                                                               "%d");
                                break;
                        case MARKER_SYMBOL_PROP_UINT64: PRINT_SIMPLE_PROPS(file,
                                                                               memfile,
                                                                               MEMFILE_TELL(memfile),
                                                                               nesting_level,
                                                                               archive_field_u64_t,
                                                                               "UInt64",
                                                                               "%"
                                                                                       PRIu64);
                                break;
                        case MARKER_SYMBOL_PROP_REAL: PRINT_SIMPLE_PROPS(file,
                                                                             memfile,
                                                                             MEMFILE_TELL(memfile),
                                                                             nesting_level,
                                                                             archive_field_number_t,
                                                                             "Float",
                                                                             "%f");
                                break;
                        case MARKER_SYMBOL_PROP_TEXT: PRINT_SIMPLE_PROPS(file,
                                                                             memfile,
                                                                             MEMFILE_TELL(memfile),
                                                                             nesting_level,
                                                                             archive_field_sid_t,
                                                                             "Text",
                                                                             "%"
                                                                                     PRIu64
                                                                                     "");
                                break;
                        case MARKER_SYMBOL_PROP_OBJECT: {
                                var_prop prop;
                                int_embedded_var_props_read(&prop, memfile);
                                fprintf(file, "0x%04x ", offset);
                                INTENT_LINE(nesting_level)
                                fprintf(file, "[marker: %c (Object)] [nentries: %d] [", entryMarker,
                                        prop.header->num_entries);
                                for (u32 i = 0; i < prop.header->num_entries; i++) {
                                        fprintf(file,
                                                "key: %"PRIu64"%s",
                                                prop.keys[i],
                                                i + 1 < prop.header->num_entries ? ", " : "");
                                }
                                fprintf(file, "] [");
                                for (u32 i = 0; i < prop.header->num_entries; i++) {
                                        fprintf(file,
                                                "offsets: 0x%04x%s",
                                                (unsigned) prop.offsets[i],
                                                i + 1 < prop.header->num_entries ? ", " : "");
                                }
                                fprintf(file, "] [\n");

                                char nextEntryMarker;
                                do {
                                        if (!_archive_print_object(file, memfile, nesting_level + 1)) {
                                                return false;
                                        }
                                        nextEntryMarker = *MEMFILE_PEEK_TYPE(memfile, char);
                                } while (nextEntryMarker == MARKER_SYMBOL_OBJECT_BEGIN);

                        }
                                break;
                        case MARKER_SYMBOL_PROP_NULL_ARRAY: {
                                prop_header *prop_header = MEMFILE_READ_TYPE(memfile, struct prop_header);

                                archive_field_sid_t *keys = (archive_field_sid_t *) MEMFILE_READ(memfile,
                                                                                                             prop_header->num_entries *
                                                                                                             sizeof(archive_field_sid_t));
                                u32 *nullArrayLengths;

                                fprintf(file, "0x%04x ", offset);
                                INTENT_LINE(nesting_level)
                                fprintf(file,
                                        "[marker: %c (Null Array)] [nentries: %d] [",
                                        entryMarker,
                                        prop_header->num_entries);

                                for (u32 i = 0; i < prop_header->num_entries; i++) {
                                        fprintf(file, "%"PRIu64"%s", keys[i],
                                                i + 1 < prop_header->num_entries ? ", " : "");
                                }
                                fprintf(file, "] [");

                                nullArrayLengths = (u32 *) MEMFILE_READ(memfile,
                                                                                prop_header->num_entries *
                                                                                sizeof(u32));

                                for (u32 i = 0; i < prop_header->num_entries; i++) {
                                        fprintf(file,
                                                "nentries: %d%s",
                                                nullArrayLengths[i],
                                                i + 1 < prop_header->num_entries ? ", " : "");
                                }

                                fprintf(file, "]\n");
                        }
                                break;
                        case MARKER_SYMBOL_PROP_BOOLEAN_ARRAY: {
                                prop_header *prop_header = MEMFILE_READ_TYPE(memfile, struct prop_header);

                                archive_field_sid_t *keys = (archive_field_sid_t *) MEMFILE_READ(memfile,
                                                                                                             prop_header->num_entries *
                                                                                                             sizeof(archive_field_sid_t));
                                u32 *array_lengths;

                                fprintf(file, "0x%04x ", offset);
                                INTENT_LINE(nesting_level)
                                fprintf(file,
                                        "[marker: %c (Boolean Array)] [nentries: %d] [",
                                        entryMarker,
                                        prop_header->num_entries);

                                for (u32 i = 0; i < prop_header->num_entries; i++) {
                                        fprintf(file, "%"PRIu64"%s", keys[i],
                                                i + 1 < prop_header->num_entries ? ", " : "");
                                }
                                fprintf(file, "] [");

                                array_lengths = (u32 *) MEMFILE_READ(memfile,
                                                                             prop_header->num_entries *
                                                                             sizeof(u32));

                                for (u32 i = 0; i < prop_header->num_entries; i++) {
                                        fprintf(file,
                                                "arrayLength: %d%s",
                                                array_lengths[i],
                                                i + 1 < prop_header->num_entries ? ", " : "");
                                }

                                fprintf(file, "] [");

                                for (u32 array_idx = 0; array_idx < prop_header->num_entries; array_idx++) {
                                        archive_field_boolean_t *values = (archive_field_boolean_t *) MEMFILE_READ(
                                                memfile,
                                                array_lengths[array_idx] *
                                                sizeof(archive_field_boolean_t));
                                        fprintf(file, "[");
                                        for (u32 i = 0; i < array_lengths[array_idx]; i++) {
                                                fprintf(file,
                                                        "value: %s%s",
                                                        values[i] ? "true" : "false",
                                                        i + 1 < array_lengths[array_idx] ? ", " : "");
                                        }
                                        fprintf(file, "]%s", array_idx + 1 < prop_header->num_entries ? ", " : "");
                                }

                                fprintf(file, "]\n");
                        }
                                break;
                                break;
                        case MARKER_SYMBOL_PROP_INT8_ARRAY: {
                                PRINT_ARRAY_PROPS(memfile,
                                                  MEMFILE_TELL(memfile),
                                                  nesting_level,
                                                  entryMarker,
                                                  archive_field_i8_t,
                                                  "Int8 Array",
                                                  "%d");
                        }
                                break;
                        case MARKER_SYMBOL_PROP_INT16_ARRAY: PRINT_ARRAY_PROPS(memfile,
                                                                                   MEMFILE_TELL(memfile),
                                                                                   nesting_level,
                                                                                   entryMarker,
                                                                                   archive_field_i16_t,
                                                                                   "Int16 Array",
                                                                                   "%d");
                                break;
                        case MARKER_SYMBOL_PROP_INT32_ARRAY: PRINT_ARRAY_PROPS(memfile,
                                                                                   MEMFILE_TELL(memfile),
                                                                                   nesting_level,
                                                                                   entryMarker,
                                                                                   archive_field_i32_t,
                                                                                   "Int32 Array",
                                                                                   "%d");
                                break;
                        case MARKER_SYMBOL_PROP_INT64_ARRAY: PRINT_ARRAY_PROPS(memfile,
                                                                                   MEMFILE_TELL(memfile),
                                                                                   nesting_level,
                                                                                   entryMarker,
                                                                                   archive_field_i64_t,
                                                                                   "Int64 Array",
                                                                                   "%"
                                                                                           PRIi64);
                                break;
                        case MARKER_SYMBOL_PROP_UINT8_ARRAY: PRINT_ARRAY_PROPS(memfile,
                                                                                   MEMFILE_TELL(memfile),
                                                                                   nesting_level,
                                                                                   entryMarker,
                                                                                   archive_field_u8_t,
                                                                                   "UInt8 Array",
                                                                                   "%d");
                                break;
                        case MARKER_SYMBOL_PROP_UINT16_ARRAY: PRINT_ARRAY_PROPS(memfile,
                                                                                    MEMFILE_TELL(memfile),
                                                                                    nesting_level,
                                                                                    entryMarker,
                                                                                    archive_field_u16_t,
                                                                                    "UInt16 Array",
                                                                                    "%d");
                                break;
                        case MARKER_SYMBOL_PROP_UINT32_ARRAY: PRINT_ARRAY_PROPS(memfile,
                                                                                    MEMFILE_TELL(memfile),
                                                                                    nesting_level,
                                                                                    entryMarker,
                                                                                    archive_field_u32_t,
                                                                                    "UInt32 Array",
                                                                                    "%d");
                                break;
                        case MARKER_SYMBOL_PROP_UINT64_ARRAY: PRINT_ARRAY_PROPS(memfile,
                                                                                    MEMFILE_TELL(memfile),
                                                                                    nesting_level,
                                                                                    entryMarker,
                                                                                    archive_field_u64_t,
                                                                                    "UInt64 Array",
                                                                                    "%"
                                                                                            PRIu64);
                                break;
                        case MARKER_SYMBOL_PROP_REAL_ARRAY: PRINT_ARRAY_PROPS(memfile,
                                                                                  MEMFILE_TELL(memfile),
                                                                                  nesting_level,
                                                                                  entryMarker,
                                                                                  archive_field_number_t,
                                                                                  "Float Array",
                                                                                  "%f");
                                break;
                        case MARKER_SYMBOL_PROP_TEXT_ARRAY: PRINT_ARRAY_PROPS(memfile,
                                                                                  MEMFILE_TELL(memfile),
                                                                                  nesting_level,
                                                                                  entryMarker,
                                                                                  archive_field_sid_t,
                                                                                  "Text Array",
                                                                                  "%"
                                                                                          PRIu64
                                                                                          "");
                                break;
                        case MARKER_SYMBOL_PROP_OBJECT_ARRAY:
                                if (!_archive_print_object_array_from_memfile(file, memfile, nesting_level)) {
                                        return false;
                                }
                                break;
                        case MARKER_SYMBOL_OBJECT_END:
                                continue_read = false;
                                break;
                        default: {
                                char buffer[256];
                                sprintf(buffer,
                                        "Parsing ERROR: unexpected marker [%c] was detected in file %p",
                                        entryMarker,
                                        memfile);
                                ERROR(ERR_CORRUPTED, buffer);
                                return false;
                        }
                }
        }

        offset = MEMFILE_TELL(memfile);
        char end_marker = *MEMFILE_READ_TYPE(memfile, char);
        assert (end_marker == MARKER_SYMBOL_OBJECT_END);
        nesting_level--;
        fprintf(file, "0x%04x ", offset);
        INTENT_LINE(nesting_level);
        fprintf(file, "[marker: %c (EndObject)]\n", end_marker);
        return true;
}

static bool is_valid_file(const archive_header *header)
{
        if (ARRAY_LENGTH(header->magic) != strlen(CARBON_ARCHIVE_MAGIC)) {
                return false;
        } else {
                for (size_t i = 0; i < ARRAY_LENGTH(header->magic); i++) {
                        if (header->magic[i] != CARBON_ARCHIVE_MAGIC[i]) {
                                return false;
                        }
                }
                if (header->version != CARBON_ARCHIVE_VERSION) {
                        return false;
                }
                if (header->root_object_header_offset == 0) {
                        return false;
                }
                return true;
        }
}

static void print_record_header_from_memfile(FILE *file, memfile *memfile)
{
        unsigned offset = MEMFILE_TELL(memfile);
        record_header *header = MEMFILE_READ_TYPE(memfile, record_header);
        record_flags flags;
        memset(&flags, 0, sizeof(record_flags));
        flags.value = header->flags;
        char *flags_string = record_header_flags_to_string(&flags);
        fprintf(file, "0x%04x ", offset);
        fprintf(file,
                "[marker: %c] [flags: %s] [record_size: 0x%04x]\n",
                header->marker,
                flags_string,
                (unsigned) header->record_size);
        free(flags_string);
}

static bool print_header_from_memfile(FILE *file, memfile *memfile)
{
        unsigned offset = MEMFILE_TELL(memfile);
        assert(MEMFILE_SIZE(memfile) > sizeof(archive_header));
        archive_header *header = MEMFILE_READ_TYPE(memfile, archive_header);
        if (!is_valid_file(header)) {
                return ERROR(ERR_NOARCHIVEFILE, NULL);
        }

        fprintf(file, "0x%04x ", offset);
        fprintf(file,
                "[magic: " CARBON_ARCHIVE_MAGIC "] [version: %d] [recordOffset: 0x%04x] [str_buf-id-offset-index: 0x%04x]\n",
                header->version,
                (unsigned) header->root_object_header_offset,
                (unsigned) header->string_id_to_offset_index_offset);
        return true;
}

static bool print_embedded_dic_from_memfile(FILE *file, memfile *memfile)
{
        packer strategy;
        string_tab_flags_u flags;

        unsigned offset = MEMFILE_TELL(memfile);
        string_table_header *header = MEMFILE_READ_TYPE(memfile, string_table_header);
        if (header->marker != global_marker_symbols[MARKER_TYPE_EMBEDDED_STR_DIC].symbol) {
                char buffer[256];
                sprintf(buffer,
                        "expected [%c] marker, but found [%c]",
                        global_marker_symbols[MARKER_TYPE_EMBEDDED_STR_DIC].symbol,
                        header->marker);
                ERROR(ERR_CORRUPTED, buffer);
                return false;
        }
        flags.value = header->flags;

        char *flagsStr = embedded_dic_flags_to_string(&flags);
        fprintf(file, "0x%04x ", offset);
        fprintf(file,
                "[marker: %c] [nentries: %d] [flags: %s] [first-entry-off: 0x%04x] [extra-size: %" PRIu64 "]\n",
                header->marker,
                header->num_entries,
                flagsStr,
                (unsigned) header->first_entry,
                header->compressor_extra_size);
        free(flagsStr);

        if (pack_by_flags(&strategy, flags.value) != true) {
                return ERROR(ERR_NOCOMPRESSOR, NULL);
        }

        pack_print_extra(&strategy, file, memfile);

        while ((*MEMFILE_PEEK_TYPE(memfile, char)) == global_marker_symbols[MARKER_TYPE_EMBEDDED_UNCOMP_STR].symbol) {
                unsigned offset = MEMFILE_TELL(memfile);
                string_entry_header header = *MEMFILE_READ_TYPE(memfile, string_entry_header);
                fprintf(file,
                        "0x%04x    [marker: %c] [next-entry-off: 0x%04zx] [str_buf-id: %"PRIu64"] [str_buf-length: %"PRIu32"]",
                        offset,
                        header.marker,
                        (size_t) header.next_entry_off,
                        header.string_id,
                        header.string_len);
                pack_print_encoded(&strategy, file, memfile, header.string_len);
                fprintf(file, "\n");
        }

        return pack_drop(&strategy);
}

static bool print_archive_from_memfile(FILE *file, memfile *memfile)
{
        if (!print_header_from_memfile(file, memfile)) {
                return false;
        }
        if (!print_embedded_dic_from_memfile(file, memfile)) {
                return false;
        }
        print_record_header_from_memfile(file, memfile);
        if (!_archive_print_object(file, memfile, 0)) {
                return false;
        }
        return true;
}

static object_flags_u *get_flags(object_flags_u *flags, column_doc_obj *columndoc)
{
        ZERO_MEMORY(flags, sizeof(object_flags_u));
        flags->bits.has_null_props = (columndoc->null_prop_keys.num_elems > 0);
        flags->bits.has_bool_props = (columndoc->bool_prop_keys.num_elems > 0);
        flags->bits.has_int8_props = (columndoc->int8_prop_keys.num_elems > 0);
        flags->bits.has_int16_props = (columndoc->int16_prop_keys.num_elems > 0);
        flags->bits.has_int32_props = (columndoc->int32_prop_keys.num_elems > 0);
        flags->bits.has_int64_props = (columndoc->int64_prop_keys.num_elems > 0);
        flags->bits.has_uint8_props = (columndoc->uint8_prop_keys.num_elems > 0);
        flags->bits.has_uint16_props = (columndoc->uint16_prop_keys.num_elems > 0);
        flags->bits.has_uint32_props = (columndoc->uin32_prop_keys.num_elems > 0);
        flags->bits.has_uint64_props = (columndoc->uint64_prop_keys.num_elems > 0);
        flags->bits.has_float_props = (columndoc->float_prop_keys.num_elems > 0);
        flags->bits.has_string_props = (columndoc->string_prop_keys.num_elems > 0);
        flags->bits.has_object_props = (columndoc->obj_prop_keys.num_elems > 0);
        flags->bits.has_null_array_props = (columndoc->null_array_prop_keys.num_elems > 0);
        flags->bits.has_bool_array_props = (columndoc->bool_array_prop_keys.num_elems > 0);
        flags->bits.has_int8_array_props = (columndoc->int8_array_prop_keys.num_elems > 0);
        flags->bits.has_int16_array_props = (columndoc->int16_array_prop_keys.num_elems > 0);
        flags->bits.has_int32_array_props = (columndoc->int32_array_prop_keys.num_elems > 0);
        flags->bits.has_int64_array_props = (columndoc->int64_array_prop_keys.num_elems > 0);
        flags->bits.has_uint8_array_props = (columndoc->uint8_array_prop_keys.num_elems > 0);
        flags->bits.has_uint16_array_props = (columndoc->uint16_array_prop_keys.num_elems > 0);
        flags->bits.has_uint32_array_props = (columndoc->uint32_array_prop_keys.num_elems > 0);
        flags->bits.has_uint64_array_props = (columndoc->uint64_array_prop_keys.num_elems > 0);
        flags->bits.has_float_array_props = (columndoc->float_array_prop_keys.num_elems > 0);
        flags->bits.has_string_array_props = (columndoc->string_array_prop_keys.num_elems > 0);
        flags->bits.has_object_array_props = (columndoc->obj_array_props.num_elems > 0);
        //assert(flags->value != 0);
        return flags;
}

static bool init_decompressor(packer *strategy, u8 flags);

static bool read_stringtable(string_table *table, FILE *disk_file);

static bool read_record(record_header *header_read, archive *archive, FILE *disk_file,
                        offset_t record_header_offset);

static bool read_string_id_to_offset_index(archive *archive, const char *file_path,
                                           offset_t string_id_to_offset_index_offset);

bool archive_open(archive *out, const char *file_path)
{
        int status;
        FILE *disk_file;

        out->disk_file_path = strdup(file_path);
        disk_file = fopen(out->disk_file_path, "r");
        if (!disk_file) {
                str_buf sb;
                char cwd[PATH_MAX];
                str_buf_create(&sb);

                str_buf_add(&sb, "File '");
                str_buf_add(&sb, file_path);
                str_buf_add(&sb, "' not found in current working directory ('");
                str_buf_add(&sb, getcwd(cwd, sizeof(cwd)));
                str_buf_add(&sb, "')");
                ERROR(ERR_FOPEN_FAILED, str_buf_cstr(&sb));
                str_buf_drop(&sb);
                return false;
        } else {
                archive_header header;
                size_t nread = fread(&header, sizeof(archive_header), 1, disk_file);
                if (nread != 1) {
                        fclose(disk_file);
                        ERROR(ERR_IO, NULL);
                        return false;
                } else {
                        if (!is_valid_file(&header)) {
                                ERROR(ERR_FORMATVERERR, NULL);
                                return false;
                        } else {
                                out->query_index_string_id_to_offset = NULL;
                                out->string_id_cache = NULL;

                                record_header record_header;

                                if ((status = read_stringtable(&out->string_table, disk_file)) != true) {
                                        return status;
                                }
                                if ((status = read_record(&record_header,
                                                          out,
                                                          disk_file,
                                                          header.root_object_header_offset)) != true) {
                                        return status;
                                }

                                if (header.string_id_to_offset_index_offset != 0) {
                                        if ((status = read_string_id_to_offset_index(out,
                                                                                     file_path,
                                                                                     header.string_id_to_offset_index_offset)) !=
                                            true) {
                                                ERROR(ERR_NOTFOUND, NULL);
                                                return status;
                                        }
                                }

                                fseek(disk_file, sizeof(archive_header), SEEK_SET);

                                offset_t data_start = ftell(disk_file);
                                fseek(disk_file, 0, SEEK_END);
                                offset_t file_size = ftell(disk_file);

                                fclose(disk_file);

                                size_t string_table_size = header.root_object_header_offset - data_start;
                                size_t record_table_size = record_header.record_size;
                                size_t string_id_index = file_size - header.string_id_to_offset_index_offset;

                                out->info.string_table_size = string_table_size;
                                out->info.record_table_size = record_table_size;
                                out->info.num_embeddded_strings = out->string_table.num_embeddded_strings;
                                out->info.string_id_index_size = string_id_index;
                                out->default_query = MALLOC(sizeof(query));
                                query_create(out->default_query, out);

                        }
                }
        }

        return true;
}

bool archive_get_info(archive_info *info, const archive *archive)
{
        *info = archive->info;
        return true;
}

bool archive_close(archive *archive)
{
        archive_drop_indexes(archive);
        archive_drop_query_string_id_cache(archive);
        free(archive->disk_file_path);
        MEMBLOCK_DROP(archive->record_table.record_db);
        query_drop(archive->default_query);
        free(archive->default_query);
        return true;
}

bool archive_drop_indexes(archive *archive)
{
        if (archive->query_index_string_id_to_offset) {
                query_drop_index_string_id_to_offset(archive->query_index_string_id_to_offset);
                archive->query_index_string_id_to_offset = NULL;
        }
        return true;
}

bool archive_query_run(query *query, archive *archive)
{
        if (query_create(query, archive)) {
                bool has_index = false;
                archive_has_query_index_string_id_to_offset(&has_index, archive);
                if (!has_index) {
                        query_create_index_string_id_to_offset(&archive->query_index_string_id_to_offset, query);
                }
                bool has_cache = false;
                archive_hash_query_string_id_cache(&has_cache, archive);
                if (!has_cache) {
                        string_id_cache_create_lru(&archive->string_id_cache, archive);
                }
                return true;
        } else {
                return false;
        }
}

bool archive_has_query_index_string_id_to_offset(bool *state, archive *archive)
{
        *state = (archive->query_index_string_id_to_offset != NULL);
        return true;
}

bool archive_hash_query_string_id_cache(bool *has_cache, archive *archive)
{
        *has_cache = archive->string_id_cache != NULL;
        return true;
}

bool archive_drop_query_string_id_cache(archive *archive)
{
        if (archive->string_id_cache) {
                string_id_cache_drop(archive->string_id_cache);
                archive->string_id_cache = NULL;
        }
        return true;
}

struct string_cache *archive_get_query_string_id_cache(archive *archive)
{
        return archive->string_id_cache;
}

query *archive_query_default(archive *archive)
{
        return archive ? archive->default_query : NULL;
}

static bool init_decompressor(packer *strategy, u8 flags)
{
        if (pack_by_flags(strategy, flags) != true) {
                return false;
        }
        return true;
}

static bool read_stringtable(string_table *table, FILE *disk_file)
{
        assert(disk_file);

        string_table_header header;
        string_tab_flags_u flags;

        size_t num_read = fread(&header, sizeof(string_table_header), 1, disk_file);
        if (num_read != 1) {
                return ERROR(ERR_IO, NULL);
        }
        if (header.marker != global_marker_symbols[MARKER_TYPE_EMBEDDED_STR_DIC].symbol) {
                return ERROR(ERR_CORRUPTED, NULL);
        }

        flags.value = header.flags;
        table->first_entry_off = header.first_entry;
        table->num_embeddded_strings = header.num_entries;

        if ((init_decompressor(&table->compressor, flags.value)) != true) {
                return false;
        }
        if ((pack_read_extra(&table->compressor, disk_file, header.compressor_extra_size)) != true) {
                return false;
        }
        return true;
}

static bool read_record(record_header *header_read, archive *archive, FILE *disk_file,
                        offset_t record_header_offset)
{
        fseek(disk_file, record_header_offset, SEEK_SET);
        record_header header;
        if (fread(&header, sizeof(record_header), 1, disk_file) != 1) {
                ERROR(ERR_CORRUPTED, NULL);
                return false;
        } else {
                archive->record_table.flags.value = header.flags;
                bool status = MEMBLOCK_FROM_FILE(&archive->record_table.record_db, disk_file, header.record_size);
                if (!status) {
                        return false;
                }

                memfile memfile;
                MEMFILE_OPEN(&memfile, archive->record_table.record_db, READ_ONLY);
                if (*MEMFILE_PEEK_TYPE(&memfile, char) != MARKER_SYMBOL_OBJECT_BEGIN) {
                        ERROR(ERR_CORRUPTED, NULL);
                        status = false;
                }

                *header_read = header;
                return true;
        }
}

static bool read_string_id_to_offset_index(archive *archive, const char *file_path,
                                           offset_t string_id_to_offset_index_offset)
{
        return query_index_id_to_offset_deserialize(&archive->query_index_string_id_to_offset,
                                                        file_path,
                                                        string_id_to_offset_index_offset);
}







