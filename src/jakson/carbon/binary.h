/*
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_BINARY_H
#define HAD_BINARY_H

#include <jakson/types.h>

typedef struct binary_field {
    const char *mime;
    u64 mime_len;
    const void *blob;
    u64 blob_len;
} binary_field;

static binary_field NULL_BINARY = {
        .mime = NULL,
        .mime_len = 0,
        .blob = 0,
        .blob_len = 0
};

#endif
