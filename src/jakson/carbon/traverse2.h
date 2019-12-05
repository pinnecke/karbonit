/**
 * traverse - traversal framework for Carbon records and containers
 *
 * Copyright 2020 Marcus Pinnecke
 */

#ifndef HAD_TRAVERSE_FRAMEWORK_H
#define HAD_TRAVERSE_FRAMEWORK_H

#include <jakson/stdinc.h>
#include <jakson/forwdecl.h>
#include <jakson/rec.h>
#include <jakson/carbon/arr-it.h>
#include <jakson/carbon/col-it.h>
#include <jakson/carbon/obj-it.h>
#include <jakson/carbon/dot.h>
#include <jakson/carbon/item.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct traverser_fn traverser_fn;

typedef enum path_policy
{
        PATH_PRUNE, /* stop traversal in this branch */
        PATH_EXPAND /* continue traversal if possible */
} path_policy_e;

typedef enum visit_ops
{
        VISIT_CALL_RECORD_ENTER = 0x1 << 0, /* call 'visit_record' when a record was found */
        VISIT_CALL_RECORD_EXIT = 0x1 << 1, /* call 'visit_record' when a record traversal ended */
        VISIT_CALL_ARRAY_ENTER = 0x1 << 2, /* call 'visit_array' when an array was found */
        VISIT_CALL_ARRAY_EXIT = 0x1 << 3, /* call 'visit_array' when an array traversal ended */
        VISIT_CALL_OBJECT_ENTER = 0x1 << 4, /* call 'visit_object' when an object was found */
        VISIT_CALL_OBJECT_EXIT = 0x1 << 5, /* call 'visit_object' when an object traversal ended */
        VISIT_CALL_FIELD_ENTER = 0x1 << 6, /* call 'visit_field' when a field was found */
        VISIT_CALL_FIELD_EXIT = 0x1 << 7, /* call 'visit_field' when a field traversal ended */
        VISIT_CALL_PROP_ENTER = 0x1 << 8, /* call 'visit_prop' when a property was found */
        VISIT_CALL_PROP_EXIT = 0x1 << 9, /* call 'visit_prop' when a property traversal ended */
        VISIT_NO_PATH = 0x1 << 10 /* do not create dot-path information during traversal */
} visit_ops_e;

#define VISIT_ALL   (VISIT_CALL_RECORD_ENTER | VISIT_CALL_RECORD_EXIT |                                                \
                     VISIT_CALL_ARRAY_ENTER | VISIT_CALL_ARRAY_EXIT |                                                  \
                     VISIT_CALL_OBJECT_ENTER | VISIT_CALL_OBJECT_EXIT |                                                \
                     VISIT_CALL_FIELD_ENTER | VISIT_CALL_FIELD_EXIT |                                                  \
                     VISIT_CALL_PROP_ENTER | VISIT_CALL_PROP_EXIT)

typedef enum visit_type
{
        ON_ENTER, /* 'visit_*' function was called because element was found */
        ON_EXIT /* 'visit_*' function was called because element traversal has ended */
} visit_type_e;

typedef struct traverse_context
{
        dot path;
        visit_type_e type;
        rec *record;
} traverse_info;

typedef struct traverse_hidden
{
        void *arg;
        union {
            struct {
                bool record_is_array;
            } json_printer;
            void *extra;
        };
} traverse_hidden;

typedef void (*traverse_fn_create)(traverse_hidden *extra); /* called when traverser is created to setup implementation */

typedef void (*traverse_fn_drop)(traverse_hidden *extra); /* call with live-time end, used for resource releasing */

typedef path_policy_e (*traverse_fn_record)(const rec *record, const traverse_info *context, traverse_hidden *extra);

typedef path_policy_e (*traverse_fn_array)(const arr_it *array, const traverse_info *context, traverse_hidden *extra);

typedef path_policy_e (*traverse_fn_object)(const obj_it *object, const traverse_info *context, traverse_hidden *extra);

typedef path_policy_e (*traverse_fn_field)(const item *field, const traverse_info *context, traverse_hidden *extra);

typedef path_policy_e (*traverse_fn_prop)(const prop *prop, const traverse_info *context, traverse_hidden *extra);

typedef struct traverser_fn
{
        traverse_fn_create create;
        traverse_fn_drop drop;

        traverse_fn_record visit_record;
        traverse_fn_array visit_array;
        traverse_fn_object visit_object;
        traverse_fn_field visit_field;
        traverse_fn_prop visit_prop;

} traverser_fn;

typedef struct traverse_impl
{
        traverser_fn fn;
        traverse_hidden extra;
} traverse_impl;

typedef enum
{
        CNTX_RECORD,
        CNTX_ARRAY,
        CNTX_OBJECT,
        CNTX_COLUMN,
        CNTX_ITEM,
        CNTX_PROP,
} cntx_type_e;

typedef struct {
        cntx_type_e type;
        union {
                rec *record;
                arr_it *array;
                col_it *column;
                obj_it *object;
                item *item;
                prop *prop;
                const void *any;
        } container;
} container_cntx;

typedef struct
{
        vec ofType(container_cntx) context;
        unsigned visit_ops;
        traverse_impl impl;
        traverse_info info;
} traverser;

void traverser_create(traverser *traverse, const traverser_fn *fns, unsigned visit_ops); /* xor visit_ops_e */
void traverser_drop(traverser *traverse);

void traverser_run_from_record(traverser *traverse, rec *record, void *arg);
void traverser_run_from_array(traverser *traverse, arr_it *it, void *arg);
void traverser_run_from_object(traverser *traverse, obj_it *it, void *arg);
void traverser_run_from_item(traverser *traverse, item *i, void *arg);
void traverser_run_from_prop(traverser *traverse, prop *p, void *arg);


#ifdef __cplusplus
}
#endif

#endif