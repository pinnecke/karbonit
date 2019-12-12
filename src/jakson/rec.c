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
#include <jakson/carbon/arr-it.h>
#include <jakson/carbon/col-it.h>
#include <jakson/carbon/obj-it.h>
#include <jakson/carbon/internal.h>
#include <jakson/carbon/dot.h>
#include <jakson/carbon/find.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/revise.h>
#include <jakson/carbon/string-field.h>
#include <jakson/carbon/key.h>
#include <jakson/carbon/commit.h>
#include <jakson/carbon/patch.h>
#include <jakson/json.h>

#define MIN_DOC_CAPACITY 17 /** minimum number of bytes required to store header and empty document array */

static bool internal_drop(rec *doc);

static void carbon_header_init(rec *doc, key_e rec_key_type);

// ---------------------------------------------------------------------------------------------------------------------

insert * rec_create_begin(rec_new *context, rec *doc,
                                           key_e type, int options)
{
        if (context && doc) {
                context->array = MALLOC(sizeof(arr_it));
                context->in = MALLOC(sizeof(insert));
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

                rec_create_empty(&context->original, derivation, type);
                revise_begin(&context->context, doc, &context->original);
                if (!revise_iterator_open(context->array, &context->context)) {
                    error(ERR_OPPFAILED, "cannot open revision iterator");
                    return NULL;
                }
                arr_it_insert_begin(context->in, context->array);
                return context->in;
        } else {
                return NULL;
        }
}

void rec_create_end(rec_new *context)
{
        arr_it_insert_end(context->in);
        revise_iterator_close(context->array);
        if (context->mode & COMPACT) {
                revise_pack(&context->context);
        }
        if (context->mode & SHRINK) {
                revise_shrink(&context->context);
        }
        revise_end(&context->context);
        free(context->array);
        free(context->in);
        rec_drop(&context->original);
}

void rec_create_empty(rec *doc, list_type_e derivation, key_e type)
{
        rec_create_empty_ex(doc, derivation, type, 1024, 1);
}

void rec_create_empty_ex(rec *doc, list_type_e derivation, key_e type,
                                u64 doc_cap, u64 array_cap)
{
        doc_cap = JAK_MAX(MIN_DOC_CAPACITY, doc_cap);

        memblock_create(&doc->block, doc_cap);
        memblock_zero_out(doc->block);
        MEMFILE_OPEN(&doc->file, doc->block, READ_WRITE);

        carbon_header_init(doc, type);
        internal_insert_array(&doc->file, derivation, array_cap);
}

bool rec_from_json(rec *doc, const char *json, key_e type,
                      const void *key)
{
        struct json data;
        json_err status;
        json_parser parser;

        if (!(json_parse(&data, &status, &parser, json))) {
                error(ERR_JSONPARSEERR, "parsing JSON file failed");
                return false;
        } else {
                internal_from_json(doc, &data, type, key, OPTIMIZE);
                json_drop(&data);
                return true;
        }
}

bool rec_from_raw_data(rec *doc, const void *data, u64 len)
{
        memblock_from_raw_data(&doc->block, data, len);
        MEMFILE_OPEN(&doc->file, doc->block, READ_WRITE);

        return true;
}

bool rec_drop(rec *doc)
{
        return internal_drop(doc);
}

const void *rec_raw_data(u64 *len, rec *doc)
{
        if (len && doc) {
                memblock_size(len, doc->file.memblock);
                return memblock_raw_data(doc->file.memblock);
        } else {
                return NULL;
        }
}

bool rec_key_type(key_e *out, rec *doc)
{
        MEMFILE_SAVE_POSITION(&doc->file);
        key_skip(out, &doc->file);
        MEMFILE_RESTORE_POSITION(&doc->file);
        return true;
}

const void *rec_key_raw_value(u64 *len, key_e *type, rec *doc)
{
        MEMFILE_SAVE_POSITION(&doc->file);
        MEMFILE_SEEK(&doc->file, 0);
        const void *result = key_read(len, type, &doc->file);
        MEMFILE_RESTORE_POSITION(&doc->file);
        return result;
}

bool rec_key_signed_value(i64 *key, rec *doc)
{
        key_e type;
        MEMFILE_SAVE_POSITION(&doc->file);
        MEMFILE_SEEK(&doc->file, 0);
        const void *result = key_read(NULL, &type, &doc->file);
        MEMFILE_RESTORE_POSITION(&doc->file);
        if (likely(rec_key_is_signed(type))) {
                *key = *((const i64 *) result);
                return true;
        } else {
                error(ERR_TYPEMISMATCH, NULL);
                return false;
        }
}

bool rec_key_unsigned_value(u64 *key, rec *doc)
{
        key_e type;
        MEMFILE_SAVE_POSITION(&doc->file);
        MEMFILE_SEEK(&doc->file, 0);
        const void *result = key_read(NULL, &type, &doc->file);
        MEMFILE_RESTORE_POSITION(&doc->file);
        if (likely(rec_key_is_unsigned(type))) {
                *key = *((const u64 *) result);
                return true;
        } else {
                error(ERR_TYPEMISMATCH, NULL);
                return false;
        }
}

const char *key_string_value(u64 *len, rec *doc)
{
        key_e type;
        MEMFILE_SAVE_POSITION(&doc->file);
        MEMFILE_SEEK(&doc->file, 0);
        const void *result = key_read(len, &type, &doc->file);
        MEMFILE_RESTORE_POSITION(&doc->file);
        if (likely(rec_key_is_string(type))) {
                return result;
        } else {
                error(ERR_TYPEMISMATCH, NULL);
                return false;
        }
}

bool rec_key_is_unsigned(key_e type)
{
        return type == KEY_UKEY || type == KEY_AUTOKEY;
}

bool rec_key_is_signed(key_e type)
{
        return type == KEY_IKEY;
}

bool rec_key_is_string(key_e type)
{
        return type == KEY_SKEY;
}

bool rec_has_key(key_e type)
{
        return type != KEY_NOKEY;
}

void rec_clone(rec *clone, rec *doc)
{
        memblock_cpy(&clone->block, doc->block);
        MEMFILE_OPEN(&clone->file, clone->block, READ_WRITE);
}

bool rec_commit_hash(u64 *hash, rec *doc)
{
        *hash = internal_header_get_commit_hash(doc);
        return true;
}

bool rec_is_multiset(rec *doc)
{
        arr_it it;
        rec_read_begin(&it, doc);
        bool ret = arr_it_is_multiset(&it);
        rec_read_end(&it);
        return ret;
}

bool rec_is_sorted(rec *doc)
{
        arr_it it;
        rec_read_begin(&it, doc);
        bool ret = arr_it_is_sorted(&it);
        rec_read_end(&it);
        return ret;
}

void rec_update_list_type(rec *revised, rec *doc, list_type_e derivation)
{
        rev context;
        revise_begin(&context, revised, doc);
        revise_set_list_type(&context, derivation);
        revise_end(&context);
}

const char *rec_to_json(str_buf *dst, rec *doc)
{
        return json_from_record(dst, doc);
}

void rec_read_begin(arr_it *it, rec *doc)
{
        patch_begin(it, doc);
        internal_arr_it_set_mode(it, READ_ONLY);
}

void rec_read_end(arr_it *it)
{
        patch_end(it);
}

bool rec_is_array(const rec *doc)
{
        arr_it it;
        rec_read_begin(&it, (rec *) doc);
        bool ret = !arr_it_is_unit(&it);
        rec_read_end(&it);
        return ret;
}

bool rec_hexdump_print(FILE *file, rec *doc)
{
        MEMFILE_SAVE_POSITION(&doc->file);
        MEMFILE_SEEK(&doc->file, 0);
        bool status = hexdump_print(file, MEMFILE_PEEK(&doc->file, 1), MEMFILE_SIZE(&doc->file));
        MEMFILE_RESTORE_POSITION(&doc->file);
        return status;
}

// ---------------------------------------------------------------------------------------------------------------------

static bool internal_drop(rec *doc)
{
        assert(doc);
        memblock_drop(doc->block);
        return true;
}

static void carbon_header_init(rec *doc, key_e rec_key_type)
{
        assert(doc);

        MEMFILE_SEEK(&doc->file, 0);
        key_create(&doc->file, rec_key_type);

        if (rec_key_type != KEY_NOKEY) {
                commit_create(&doc->file);
        }
}