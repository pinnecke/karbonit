/*
 * json - Java Script Object Notation sub module
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_JSON_H
#define HAD_JSON_H

#include <jakson/rec.h>
#include <jakson/carbon/arr-it.h>
#include <jakson/carbon/col-it.h>
#include <jakson/carbon/obj-it.h>
#include <jakson/carbon/item.h>
#include <jakson/std/string.h>

#ifdef __cplusplus
extern "C" {
#endif

void json_from_record(str_buf *dst, rec *src);
void json_from_array(str_buf *dst, arr_it *src);
void json_from_column(str_buf *dst, col_it *src);
void json_from_object(str_buf *dst, obj_it *src);
void json_from_item(str_buf *dst, item *src);

#ifdef __cplusplus
}
#endif

#endif
