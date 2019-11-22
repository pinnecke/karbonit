/*
 * commit - commit hash functions
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_COMMIT_H
#define HAD_COMMIT_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/mem/file.h>
#include <jakson/std/string.h>

#ifdef __cplusplus
extern "C" {
#endif

bool commit_create(memfile *file);
bool commit_skip(memfile *file);
bool commit_read(u64 *commit_hash, memfile *file);
bool commit_peek(u64 *commit_hash, memfile *file);
bool commit_update(memfile *file, const char *base, u64 len);
bool commit_compute(u64 *commit_hash, const void *base, u64 len);
const char *commit_to_str(str_buf *dst, u64 commit_hash);
bool commit_append_to_str(str_buf *dst, u64 commit_hash);
u64 commit_from_str(const char *commit_str);

#ifdef __cplusplus
}
#endif

#endif
