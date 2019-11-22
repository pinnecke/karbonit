/**
 * traverse - traversal framework for Carbon records and containers
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_TRAVERSE_H
#define HAD_TRAVERSE_H

#include <jakson/stdinc.h>
#include <jakson/forwdecl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* called before traversing is started; used to allocate 'extra' allocated memory */
typedef void (*traverse_setup_t)(struct traverse_extra *extra);

/* called when traversing has ended; used to free 'extra' allocated memory */
typedef void (*traverse_clean_t)(struct traverse_extra *extra);

/* called when traversing just had started; accesses the record itself */
typedef void (*visit_record_t)(struct traverse_extra *extra, struct rec *record);

/* called when an array was found */
typedef void (*visit_array_enter_t)(struct traverse_extra *extra, arr_it *it);

/* called when an array item was found */
typedef void (*visit_item_t)(struct traverse_extra *extra, item *it);

/* called when an array was was passed */
typedef void (*visit_array_exit_t)(struct traverse_extra *extra, arr_it *it);

/* called when an column was found */
typedef bool (*visit_column_t)(struct traverse_extra *extra, struct col_it *it);

/* called when an object was found */
typedef void (*visit_object_enter_t)(struct traverse_extra *extra, struct obj_it *it);

/* called when an object property item was found */
typedef void (*visit_prop_t)(struct traverse_extra *extra, item *it);

/* called when an object was passed */
typedef void (*visit_object_exit_t)(struct traverse_extra *extra, struct obj_it *it);


/* built-in or user-defined extra data for operation */
enum  traverse_tag {
        TRAVERSE_UNKNOWN,
        TRAVERSE_PRINT_JSON
};

enum json_convert {
        JSON_TO_NULL,
        JSON_REMAIN,
        JSON_ELEMENT
};

struct traverse_extra {
        struct carbon_traverse *parent;
        union {
            struct {
                struct str_buf *str;
                struct carbon_json_from_opts *config;
                enum json_convert convert;
            } print_json;
            struct {
                void *data;
            } unknown;
        } capture;
};

struct carbon_traverse
{
        enum  traverse_tag tag;

        bool read_write;

        traverse_setup_t setup;
        traverse_clean_t cleanup;

        visit_record_t visit_record;

        visit_array_enter_t visit_array_begin;
        visit_item_t visit_item;
        visit_array_exit_t visit_array_end;

        visit_column_t visit_column;

        visit_object_enter_t visit_object_begin;
        visit_object_exit_t visit_object_end;

        struct traverse_extra extra;
};

void traverse_create(struct carbon_traverse *traverse, traverse_setup_t begin, traverse_clean_t end,
                            visit_record_t visit_record, visit_array_enter_t visit_array_begin,
                            visit_array_exit_t visit_array_end, visit_column_t visit_column,
                            visit_object_enter_t visit_object_begin, visit_object_exit_t visit_object_end,
                            enum traverse_tag tag, bool read_write);

void traverse_drop(struct carbon_traverse *traverse);

void traverse_carbon(struct rec *rev_out, struct carbon_traverse *traverse, struct rec *record);
void traverse_array(struct carbon_traverse *traverse, arr_it *it);
void traverse_column(struct carbon_traverse *traverse, struct col_it *it);
void traverse_object(struct carbon_traverse *traverse, struct obj_it *it);

void traverse_continue_array(struct traverse_extra *context, arr_it *it);
void traverse_continue_column(struct traverse_extra *context, struct col_it *it);
void traverse_continue_object(struct traverse_extra *context, struct obj_it *it);

#ifdef __cplusplus
}
#endif

#endif