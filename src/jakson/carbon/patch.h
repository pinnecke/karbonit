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

/* Closes a read-write ennabled iterator , which was previously opened via 'patch_end' */
void patch_end(arr_it *it);

bool patch_find_begin(find *out, const char *dot, rec *doc);

bool patch_find_end(find *find);

#ifdef __cplusplus
}
#endif

#endif
