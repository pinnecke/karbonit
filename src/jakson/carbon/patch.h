/**
 * patch - modification of records within a revision (not thread-safe)
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_PATCH_H
#define HAD_PATCH_H

#include <jakson/stdinc.h>
#include <jakson/forwdecl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opens a read-write enabled iterator for patching a record revision without creating a new one. */
void patch_begin(arr_it *it, rec *doc);

void patch_find_begin(find *out, const char *dot, rec *doc);

void patch_find_end(find *find);

#ifdef __cplusplus
}
#endif

#endif
