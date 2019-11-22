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

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/rec.h>
#include <jakson/carbon/col-it.h>
#include <jakson/carbon/arr-it.h>
#include <jakson/carbon/obj-it.h>
#include <jakson/carbon/container.h>
#include <jakson/carbon/dot.h>
#include <jakson/carbon/dot-eval.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct find {
        rec *doc;
        field_e type;
        dot_eval eval;
        union {
                arr_it *array;
                col_it *column;
                obj_it *object;
                bool boolean;
                u64 unsigned_number;
                i64 signed_number;
                float float_number;
                string_field string;
                binary_field binary;
        } value;
} find;

bool find_begin(find *out, const char *dot, rec *doc);
bool find_end(find *find);
bool find_create(find *find, dot *path, rec *doc);
bool find_drop(find *find);

bool find_has_result(find *find);
const char *find_result_to_str(str_buf *dst_str, printer_impl_e print_type, find *find);
const char *find_result_to_json_compact(str_buf *dst_str, find *find);
char *find_result_to_json_compact_dup(find *find);

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
