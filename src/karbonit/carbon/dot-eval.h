/*
 * dot-eval - dot-notated path evaluator
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_DOT_EVAL_H
#define HAD_DOT_EVAL_H

#include <karbonit/stdinc.h>
#include <karbonit/carbon/dot.h>
#include <karbonit/carbon/container.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dot_eval {
        rec *doc;
        arr_it root_it;
        pstatus_e status;
        struct {
                container_e container;
                union {
                        arr_it array;
                        obj_it object;
                        struct {
                                col_it it;
                                u32 elem_pos;
                        } column;
                } containers;
        } result;
} dot_eval;

void dot_eval_exec(dot_eval *eval, const dot *path, rec *doc);
bool dot_eval_begin_mutable(dot_eval *eval, const dot *path, rev *context);

bool dot_eval_status(pstatus_e *status, dot_eval *state);

#define DOT_EVAL_HAS_RESULT(dot_eval)                                                                                  \
        ((dot_eval)->status == PATH_RESOLVED)

bool carbon_path_exists(rec *doc, const char *path);

bool carbon_path_is_array(rec *doc, const char *path);
bool carbon_path_is_column(rec *doc, const char *path);
bool carbon_path_is_object(rec *doc, const char *path);
bool carbon_path_is_container(rec *doc, const char *path);
bool carbon_path_is_null(rec *doc, const char *path);
bool carbon_path_is_number(rec *doc, const char *path);
bool carbon_path_is_boolean(rec *doc, const char *path);
bool carbon_path_is_string(rec *doc, const char *path);

#ifdef __cplusplus
}
#endif

#endif
