/*
 * Copyright 2019 Marcus Pinnecke
 */
#ifndef HAD_CARBON_BINARY_H
#define HAD_CARBON_BINARY_H

#include <jakson/types.h>

typedef struct carbon_binary {
    const char *mime_type;
    u64 mime_type_strlen;
    const void *blob;
    u64 blob_len;
} carbon_binary;

static carbon_binary CARBON_NULL_BINARY = {
        .mime_type = NULL,
        .mime_type_strlen = 0,
        .blob = 0,
        .blob_len = 0
};

#endif

