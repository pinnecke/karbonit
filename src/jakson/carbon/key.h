/*
 * key - low-level key operations
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_KEY_H
#define HAD_KEY_H

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/rec.h>
#include <jakson/mem/memfile.h>

#ifdef __cplusplus
extern "C" {
#endif

bool key_create(memfile *file, key_e type);
bool key_skip(key_e *out, memfile *file);
bool key_read_type(key_e *out, memfile *file);
bool key_write_unsigned(memfile *file, u64 key);
bool key_write_signed(memfile *file, i64 key);
bool key_write_string(memfile *file, const char *key);
bool key_update_string(memfile *file, const char *key);
bool key_update_string_wnchar(memfile *file, const char *key, size_t length);
const void *key_read(u64 *len, key_e *out, memfile *file);
const char *key_type_str(key_e type);

#ifdef __cplusplus
}
#endif

#endif
