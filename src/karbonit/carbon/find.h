/*
 * find - finding fields inside a carbon file by a dot-notated query path
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_FIND_H
#define HAD_FIND_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <karbonit/stdinc.h>
#include <karbonit/error.h>
#include <karbonit/rec.h>
#include <karbonit/carbon/col-it.h>
#include <karbonit/carbon/arr-it.h>
#include <karbonit/carbon/obj-it.h>
#include <karbonit/carbon/container.h>
#include <karbonit/carbon/dot.h>
#include <karbonit/carbon/dot-eval.h>
#include <karbonit/std/uintvar/stream.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct find {
        rec *doc;
        field_e type;
        dot_eval eval;
        union {
                arr_it array;
                col_it column;
                obj_it object;
                bool boolean;
                u64 unsigned_number;
                i64 signed_number;
                float float_number;
                string_field string;
                binary_field binary;
        } value;
} find;

bool find_from_string(find *out, const char *dot, rec *doc);
bool find_from_dot(find *out, const dot *path, rec *doc);
bool internal_find_exec(find *find, const dot *path, rec *doc);

bool find_has_result(find *find);
const char *find_result_to_str(str_buf *dst_str, find *find);

bool find_result_type(field_e *type, find *find);

bool find_update_array_type(find *find, list_type_e derivation);
bool find_array_is_multiset(find *find);
bool find_array_is_sorted(find *find);

bool find_update_column_type(find *find, list_type_e derivation);
bool find_column_is_multiset(find *find);
bool find_column_is_sorted(find *find);

bool find_update_object_type(find *find, map_type_e derivation);
bool find_object_is_multimap(find *find);
bool find_object_is_sorted(find *find);

bool find_multimap(find *find);
bool find_multiset(find *find);
bool find_sorted(find *find);

bool find_result_is_array(find *find);
bool find_result_is_object(find *find);
bool find_result_is_column(find *find);
bool find_result_is_boolean(find *find);
bool find_result_is_undefined(find *find);
bool find_result_is_unsigned(find *find);
bool find_result_is_signed(find *find);
bool find_result_is_float(find *find);
bool find_result_is_null(find *find);
bool find_result_is_string(find *find);
bool find_result_is_binary(find *find);

arr_it *find_result_array(find *find);
obj_it *find_result_object(find *find);
col_it *find_result_column(find *find);
bool find_result_boolean(bool *result, find *find);
bool find_result_unsigned(u64 *out, find *find);
bool find_result_signed(i64 *out, find *find);
bool find_result_float(float *out, find *find);
const char *find_result_string(u64 *str_len, find *find);
binary_field *find_result_binary(find *find);

#ifdef __cplusplus
}
#endif

#endif
