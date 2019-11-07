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

#ifndef HAD_CARBON_TRAVERSE_H
#define HAD_CARBON_TRAVERSE_H

#include <jakson/stdinc.h>
#include <jakson/forwdecl.h>

BEGIN_DECL

/* called before traversing is started; used to allocate 'extra' allocated memory */
typedef void (*carbon_traverse_setup_t)(struct carbon_traverse_extra *extra);

/* called when traversing has ended; used to free 'extra' allocated memory */
typedef void (*carbon_traverse_clean_t)(struct carbon_traverse_extra *extra);

/* called when traversing just had started; accesses the record itself */
typedef void (*carbon_visit_record_t)(struct carbon_traverse_extra *extra, struct carbon *record);

/* called when an array was found */
typedef void (*carbon_visit_array_enter_t)(struct carbon_traverse_extra *extra, carbon_array *it);

/* called when an array item was found */
typedef void (*carbon_visit_item_t)(struct carbon_traverse_extra *extra, carbon_item *it);

/* called when an array was was passed */
typedef void (*carbon_visit_array_exit_t)(struct carbon_traverse_extra *extra, carbon_array *it);

/* called when an column was found */
typedef void (*carbon_visit_column_t)(struct carbon_traverse_extra *extra, struct carbon_column *it);

/* called when an object was found */
typedef void (*carbon_visit_object_enter_t)(struct carbon_traverse_extra *extra, struct carbon_object *it);

/* called when an object property item was found */
typedef void (*carbon_visit_prop_t)(struct carbon_traverse_extra *extra, carbon_item *it);

/* called when an object was passed */
typedef void (*carbon_visit_object_exit_t)(struct carbon_traverse_extra *extra, struct carbon_object *it);


/* built-in or user-defined extra data for operation */
enum  carbon_traverse_tag {
        CARBON_TRAVERSE_UNKNOWN,
        CARBON_TRAVERSE_PRINT_JSON
};

enum carbon_print_json_collection_convert {
        CARBON_PRINT_JSON_CONVERT_TO_NULL,
        CARBON_PRINT_JSON_CONVERT_REMAIN,
        CARBON_PRINT_JSON_CONVERT_TO_ELEMENT
};

struct carbon_traverse_extra {
        struct carbon_traverse *parent;
        union {
            struct {
                struct string_buffer *str;
                struct carbon_json_from_opts *config;
                enum carbon_print_json_collection_convert convert;
            } print_json;
            struct {
                void *data;
            } unknown;
        } capture;
};

struct carbon_traverse
{
        enum  carbon_traverse_tag tag;

        bool read_write;

        carbon_traverse_setup_t setup;
        carbon_traverse_clean_t cleanup;

        carbon_visit_record_t visit_record;

        carbon_visit_array_enter_t visit_array_begin;
        carbon_visit_item_t visit_item;
        carbon_visit_array_exit_t visit_array_end;

        carbon_visit_column_t visit_column;

        carbon_visit_object_enter_t visit_object_begin;
        carbon_visit_object_exit_t visit_object_end;

        struct carbon_traverse_extra extra;
};

void carbon_traverse_create(struct carbon_traverse *traverse, carbon_traverse_setup_t begin, carbon_traverse_clean_t end,
                            carbon_visit_record_t visit_record, carbon_visit_array_enter_t visit_array_begin,
                            carbon_visit_array_exit_t visit_array_end, carbon_visit_column_t visit_column,
                            carbon_visit_object_enter_t visit_object_begin, carbon_visit_object_exit_t visit_object_end,
                            enum carbon_traverse_tag tag, bool read_write);

void carbon_traverse_drop(struct carbon_traverse *traverse);

void carbon_traverse_carbon(struct carbon *rev_out, struct carbon_traverse *traverse, struct carbon *record);
void carbon_traverse_array(struct carbon_traverse *traverse, carbon_array *it);
void carbon_traverse_column(struct carbon_traverse *traverse, struct carbon_column *it);
void carbon_traverse_object(struct carbon_traverse *traverse, struct carbon_object *it);

void carbon_traverse_continue_array(struct carbon_traverse_extra *context, carbon_array *it);
void carbon_traverse_continue_column(struct carbon_traverse_extra *context, struct carbon_column *it);
void carbon_traverse_continue_object(struct carbon_traverse_extra *context, struct carbon_object *it);

END_DECL

#endif