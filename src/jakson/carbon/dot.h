/*
 * dot - an ast for dot-notated query paths
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_DOT_H
#define HAD_DOT_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/types.h>
#include <jakson/std/string.h>
#include <jakson/carbon/arr-it.h>
#include <jakson/carbon/col-it.h>
#include <jakson/carbon/obj-it.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dot_node_type {
        DOT_NODE_IDX,
        DOT_NODE_KEY
} dot_node_type_e;

typedef struct dot_node {
    dot_node_type_e type;
        union {
                char *string;
                u32 idx;
        } name;
} dot_node;

typedef struct dot {
        vec ofType(dot_node) nodes;
} dot;

typedef enum pstatus {
        PATH_RESOLVED,
        PATH_EMPTY_DOC,
        PATH_NOSUCHINDEX,
        PATH_NOSUCHKEY,
        PATH_NOTTRAVERSABLE,
        PATH_NOCONTAINER,
        PATH_NOTANOBJECT,
        PATH_NONESTING,
        PATH_INTERNAL
} pstatus_e;

bool dot_create(dot *path);
void dot_clear(dot *path);
bool dot_from_string(dot *path, const char *path_string);
bool dot_drop(dot *path);
void dot_add_key(dot *dst, const char *key);
void dot_add_nkey(dot *dst, const char *key, size_t len);
void dot_add_idx(dot *dst, u32 idx);
bool dot_len(u32 *len, const dot *path);
bool dot_is_empty(const dot *path);
bool dot_type_at(dot_node_type_e *type_out, u32 pos, const dot *path);
bool dot_idx_at(u32 *idx, u32 pos, const dot *path);
const char *dot_key_at(u32 pos, const dot *path);
bool dot_to_str(str_buf *sb, dot *path);
bool dot_fprint(FILE *file, dot *path);
bool dot_print(dot *path);

#ifdef __cplusplus
}
#endif

#endif
