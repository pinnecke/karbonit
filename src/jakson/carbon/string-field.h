/**
 * string-field - low-level string field for Carbon records
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_STRING_FIELD_H
#define HAD_STRING_FIELD_H

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/mem/memfile.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct string_field
{
        const char *str;
        u64 len;
} string_field;

static string_field NULL_STRING = {
        .str = NULL,
        .len = 0
};

bool string_field_write(memfile *file, const char *string);
bool string_field_nchar_write(memfile *file, const char *string, u64 str_len);
bool string_field_nomarker_write(memfile *file, const char *string);
bool string_field_nomarker_nchar_write(memfile *file, const char *string, u64 str_len);
bool string_field_nomarker_remove(memfile *file);
bool string_field_remove(memfile *file);
bool string_field_update(memfile *file, const char *string);
bool string_field_update_wnchar(memfile *file, const char *string, size_t str_len);
bool string_field_skip(memfile *file);
bool string_field_nomarker_skip(memfile *file);
const char *string_field_read(u64 *len, memfile *file);
const char *string_field_nomarker_read(u64 *len, memfile *file);

#ifdef __cplusplus
}
#endif

#endif
