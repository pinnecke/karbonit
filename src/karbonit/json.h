/*
 * json - Java Script Object Notation sub module
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_JSON_H
#define HAD_JSON_H

#include <karbonit/stdinc.h>
#include <karbonit/types.h>
#include <karbonit/forwdecl.h>

#ifdef __cplusplus
extern "C" {
#endif

const char *json_from_record(str_buf *dst, rec *src);
const char *json_from_array(str_buf *dst, arr_it *src);
const char *json_from_column(str_buf *dst, col_it *src);
const char *json_from_object(str_buf *dst, obj_it *src);
const char *json_from_item(str_buf *dst, item *src);

const char *json_from_binary(str_buf *dst, const binary_field *binary);
const char *json_from_string(str_buf *dst, const string_field *str);
const char *json_from_boolean(str_buf *dst, bool value);
const char *json_from_unsigned(str_buf *dst, u64 value);
const char *json_from_signed(str_buf *dst, i64 value);
const char *json_from_float(str_buf *dst, float value);
const char *json_from_null(str_buf *dst);
const char *json_from_undef(str_buf *dst);

#ifdef __cplusplus
}
#endif

#endif
