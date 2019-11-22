/**
 * Columnar Binary JSON -- Copyright 2019 Marcus Pinnecke
 * This file implements the document format itself
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

#include <jakson/forwdecl.h>
#include <jakson/std/uintvar/stream.h>
#include <jakson/rec.h>
#include <jakson/carbon/arr_it.h>
#include <jakson/carbon/col_it.h>
#include <jakson/carbon/object.h>
#include <jakson/carbon/printers.h>
#include <jakson/carbon/internal.h>
#include <jakson/carbon/dot.h>
#include <jakson/carbon/find.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/revise.h>
#include <jakson/carbon/string.h>
#include <jakson/carbon/key.h>
#include <jakson/carbon/commit.h>
#include <jakson/carbon/patch.h>
#include <jakson/carbon/printers/compact.h>
#include <jakson/carbon/printers/extended.h>

#define MIN_DOC_CAPACITY 17 /** minimum number of bytes required to store header and empty document array */

static bool internal_drop(rec *doc);

static void carbon_header_init(rec *doc, carbon_key_e key_type);

// ---------------------------------------------------------------------------------------------------------------------

carbon_insert * carbon_create_begin(rec_new *context, rec *doc,
                                           carbon_key_e type, int options)
{
        if (context && doc) {
                context->content_it = MALLOC(sizeof(arr_it));
                context->inserter = MALLOC(sizeof(carbon_insert));
                context->mode = options;

                /** get the annotation type for that records outer-most array from options*/
                list_type_e derivation;

                if (context->mode & SORTED_MULTISET) {
                        derivation = LIST_SORTED_MULTISET;
                } else if (context->mode & UNSORTED_SET) {
                        derivation = LIST_UNSORTED_SET;
                } else if (context->mode & SORTED_SET) {
                        derivation = LIST_SORTED_SET;
                } else { /** UNSORTED_MULTISET is default */
                        derivation = LIST_UNSORTED_MULTISET;
                }

                carbon_create_empty(&context->original, derivation, type);
                carbon_revise_begin(&context->revision_context, doc, &context->original);
                if (!carbon_revise_iterator_open(context->content_it, &context->revision_context)) {
                    error(ERR_OPPFAILED, "cannot open revision iterator");
                    return NULL;
                }
                arr_it_insert_begin(context->inserter, context->content_it);
                return context->inserter;
        } else {
                return NULL;
        }
}

void carbon_create_end(rec_new *context)
{
        arr_it_insert_end(context->inserter);
        carbon_revise_iterator_close(context->content_it);
        if (context->mode & CARBON_COMPACT) {
                carbon_revise_pack(&context->revision_context);
        }
        if (context->mode & CARBON_SHRINK) {
                carbon_revise_shrink(&context->revision_context);
        }
        carbon_revise_end(&context->revision_context);
        free(context->content_it);
        free(context->inserter);
        carbon_drop(&context->original);
}

void carbon_create_empty(rec *doc, list_type_e derivation, carbon_key_e type)
{
        carbon_create_empty_ex(doc, derivation, type, 1024, 1);
}

void carbon_create_empty_ex(rec *doc, list_type_e derivation, carbon_key_e type,
                                u64 doc_cap, u64 array_cap)
{
        doc_cap = JAK_MAX(MIN_DOC_CAPACITY, doc_cap);

        memblock_create(&doc->area, doc_cap);
        memblock_zero_out(doc->area);
        memfile_open(&doc->file, doc->area, READ_WRITE);

        carbon_header_init(doc, type);
        internal_insert_array(&doc->file, derivation, array_cap);
}

bool carbon_from_json(rec *doc, const char *json, carbon_key_e type,
                      const void *key)
{
        struct json data;
        json_err status;
        json_parser parser;

        if (!(json_parse(&data, &status, &parser, json))) {
                error(ERR_JSONPARSEERR, "parsing JSON file failed");
                return false;
        } else {
                internal_from_json(doc, &data, type, key, CARBON_OPTIMIZE);
                json_drop(&data);
                return true;
        }
}

bool carbon_from_raw_data(rec *doc, const void *data, u64 len)
{
        memblock_from_raw_data(&doc->area, data, len);
        memfile_open(&doc->file, doc->area, READ_WRITE);

        return true;
}

bool carbon_drop(rec *doc)
{
        return internal_drop(doc);
}

const void *carbon_raw_data(u64 *len, rec *doc)
{
        if (len && doc) {
                memblock_size(len, doc->file.memblock);
                return memblock_raw_data(doc->file.memblock);
        } else {
                return NULL;
        }
}

bool carbon_key_type(carbon_key_e *out, rec *doc)
{
        memfile_save_position(&doc->file);
        carbon_key_skip(out, &doc->file);
        memfile_restore_position(&doc->file);
        return true;
}

const void *carbon_key_raw_value(u64 *len, carbon_key_e *type, rec *doc)
{
        memfile_save_position(&doc->file);
        memfile_seek(&doc->file, 0);
        const void *result = carbon_key_read(len, type, &doc->file);
        memfile_restore_position(&doc->file);
        return result;
}

bool carbon_key_signed_value(i64 *key, rec *doc)
{
        carbon_key_e type;
        memfile_save_position(&doc->file);
        memfile_seek(&doc->file, 0);
        const void *result = carbon_key_read(NULL, &type, &doc->file);
        memfile_restore_position(&doc->file);
        if (LIKELY(carbon_key_is_signed(type))) {
                *key = *((const i64 *) result);
                return true;
        } else {
                error(ERR_TYPEMISMATCH, NULL);
                return false;
        }
}

bool carbon_key_unsigned_value(u64 *key, rec *doc)
{
        carbon_key_e type;
        memfile_save_position(&doc->file);
        memfile_seek(&doc->file, 0);
        const void *result = carbon_key_read(NULL, &type, &doc->file);
        memfile_restore_position(&doc->file);
        if (LIKELY(carbon_key_is_unsigned(type))) {
                *key = *((const u64 *) result);
                return true;
        } else {
                error(ERR_TYPEMISMATCH, NULL);
                return false;
        }
}

const char *carbon_key_string_value(u64 *len, rec *doc)
{
        carbon_key_e type;
        memfile_save_position(&doc->file);
        memfile_seek(&doc->file, 0);
        const void *result = carbon_key_read(len, &type, &doc->file);
        memfile_restore_position(&doc->file);
        if (LIKELY(carbon_key_is_string(type))) {
                return result;
        } else {
                error(ERR_TYPEMISMATCH, NULL);
                return false;
        }
}

bool carbon_key_is_unsigned(carbon_key_e type)
{
        return type == CARBON_KEY_UKEY || type == CARBON_KEY_AUTOKEY;
}

bool carbon_key_is_signed(carbon_key_e type)
{
        return type == CARBON_KEY_IKEY;
}

bool carbon_key_is_string(carbon_key_e type)
{
        return type == CARBON_KEY_SKEY;
}

bool carbon_has_key(carbon_key_e type)
{
        return type != CARBON_KEY_NOKEY;
}

bool carbon_clone(rec *clone, rec *doc)
{
        CHECK_SUCCESS(memblock_cpy(&clone->area, doc->area));
        CHECK_SUCCESS(memfile_open(&clone->file, clone->area, READ_WRITE));
        return true;
}

bool carbon_commit_hash(u64 *hash, rec *doc)
{
        *hash = internal_header_get_commit_hash(doc);
        return true;
}

bool carbon_is_multiset(rec *doc)
{
        arr_it it;
        carbon_read_begin(&it, doc);
        bool ret = arr_it_is_multiset(&it);
        carbon_read_end(&it);
        return ret;
}

bool carbon_is_sorted(rec *doc)
{
        arr_it it;
        carbon_read_begin(&it, doc);
        bool ret = arr_it_is_sorted(&it);
        carbon_read_end(&it);
        return ret;
}

void carbon_update_list_type(rec *revised_doc, rec *doc, list_type_e derivation)
{
        rev context;
        carbon_revise_begin(&context, revised_doc, doc);
        carbon_revise_set_list_type(&context, derivation);
        carbon_revise_end(&context);
}

bool carbon_to_str(string_buffer *dst, carbon_printer_impl_e printer, rec *doc)
{
        carbon_printer p;
        string_buffer b;
        carbon_key_e key_type;
        u64 key_len;
        u64 rev;

        string_buffer_clear(dst);
        string_buffer_ensure_capacity(dst, 2 * memfile_size(&doc->file));

        memfile_save_position(&doc->file);

        ZERO_MEMORY(&p, sizeof(carbon_printer));
        string_buffer_create(&b);

        carbon_commit_hash(&rev, doc);

        carbon_printer_by_type(&p, printer);

        carbon_printer_begin(&p, &b);
        carbon_printer_header_begin(&p, &b);

        const void *key = carbon_key_raw_value(&key_len, &key_type, doc);
        carbon_printer_header_contents(&p, &b, key_type, key, key_len, rev);

        carbon_printer_header_end(&p, &b);
        carbon_printer_payload_begin(&p, &b);

        arr_it it;
        carbon_read_begin(&it, doc);

        carbon_printer_print_array(&it, &p, &b, true);
        arr_it_drop(&it);

        carbon_printer_payload_end(&p, &b);
        carbon_printer_end(&p, &b);

        carbon_printer_drop(&p);
        string_buffer_add(dst, string_cstr(&b));
        string_buffer_drop(&b);

        memfile_restore_position(&doc->file);
        return true;
}

const char *carbon_to_json_extended(string_buffer *dst, rec *doc)
{
        carbon_to_str(dst, JSON_EXTENDED, doc);
        return string_cstr(dst);
}

const char *carbon_to_json_compact(string_buffer *dst, rec *doc)
{
        carbon_to_str(dst, JSON_COMPACT, doc);
        return string_cstr(dst);
}

char *carbon_to_json_extended_dup(rec *doc)
{
        string_buffer sb;
        string_buffer_create(&sb);
        char *result = strdup(carbon_to_json_extended(&sb, doc));
        string_buffer_drop(&sb);
        return result;
}

char *carbon_to_json_compact_dup(rec *doc)
{
        string_buffer sb;
        string_buffer_create(&sb);
        char *result = strdup(carbon_to_json_compact(&sb, doc));
        string_buffer_drop(&sb);
        return result;
}

void carbon_read_begin(arr_it *it, rec *doc)
{
        carbon_patch_begin(it, doc);
        internal_arr_it_set_mode(it, READ_ONLY);
}

void carbon_read_end(arr_it *it)
{
        carbon_patch_end(it);
}

bool carbon_print(FILE *file, carbon_printer_impl_e printer, rec *doc)
{
        string_buffer builder;
        string_buffer_create(&builder);
        carbon_to_str(&builder, printer, doc);
        fprintf(file, "%s\n", string_cstr(&builder));
        string_buffer_drop(&builder);

        return true;
}

bool carbon_hexdump_print(FILE *file, rec *doc)
{
        memfile_save_position(&doc->file);
        memfile_seek(&doc->file, 0);
        bool status = hexdump_print(file, memfile_peek(&doc->file, 1), memfile_size(&doc->file));
        memfile_restore_position(&doc->file);
        return status;
}

// ---------------------------------------------------------------------------------------------------------------------

static bool internal_drop(rec *doc)
{
        JAK_ASSERT(doc);
        memblock_drop(doc->area);
        return true;
}

static void carbon_header_init(rec *doc, carbon_key_e key_type)
{
        JAK_ASSERT(doc);

        memfile_seek(&doc->file, 0);
        carbon_key_create(&doc->file, key_type);

        if (key_type != CARBON_KEY_NOKEY) {
                commit_create(&doc->file);
        }
}