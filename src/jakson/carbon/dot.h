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
#include <jakson/carbon/arr_it.h>
#include <jakson/carbon/col_it.h>
#include <jakson/carbon/object.h>

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

typedef struct dot_path {
        dot_node nodes[256];
        u32 len;
} dot_path;

typedef enum path_status {
        PATH_RESOLVED,
        PATH_EMPTY_DOC,
        PATH_NOSUCHINDEX,
        PATH_NOSUCHKEY,
        PATH_NOTTRAVERSABLE,
        PATH_NOCONTAINER,
        PATH_NOTANOBJECT,
        PATH_NONESTING,
        PATH_INTERNAL
} path_status_e;

bool dot_create(dot_path *path);
bool dot_from_string(dot_path *path, const char *path_string);
bool dot_drop(dot_path *path);
bool dot_add_key(dot_path *dst, const char *key);
bool dot_add_nkey(dot_path *dst, const char *key, size_t len);
bool dot_add_idx(dot_path *dst, u32 idx);
bool dot_len(u32 *len, const dot_path *path);
bool dot_is_empty(const dot_path *path);
bool dot_type_at(dot_node_type_e *type_out, u32 pos, const dot_path *path);
bool dot_idx_at(u32 *idx, u32 pos, const dot_path *path);
const char *dot_key_at(u32 pos, const dot_path *path);

bool dot_to_str(string_buffer *sb, dot_path *path);
bool dot_fprint(FILE *file, dot_path *path);
bool dot_print(dot_path *path);

#ifdef __cplusplus
}
#endif

#endif
