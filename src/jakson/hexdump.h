/*
 * Copyright 2018 Marcus Pinnecke
 */

#ifndef HAD_HEXDUMP_H
#define HAD_HEXDUMP_H

#include <jakson/stdinc.h>
#include <jakson/std/string.h>

#ifdef __cplusplus
extern "C" {
#endif

bool hexdump(str_buf *dst, const void *base, u64 nbytes);
bool hexdump_print(FILE *file, const void *base, u64 nbytes);

#ifdef __cplusplus
}
#endif

#endif
