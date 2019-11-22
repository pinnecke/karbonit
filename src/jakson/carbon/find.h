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
#include <jakson/carbon/col_it.h>
#include <jakson/carbon/arr_it.h>
#include <jakson/carbon/object.h>
#include <jakson/carbon/containers.h>
#include <jakson/carbon/dot.h>
#include <jakson/carbon/path.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct carbon_find {
        rec *doc;
        field_e type;
        carbon_path_evaluator path_evaluater;

        bool value_is_nulled;

        union {
                arr_it *array;
                col_it *column_it;
                obj_it *object_it;
                bool boolean;
                u64 unsigned_number;
                i64 signed_number;
                float float_number;

                carbon_string_field string;
                binary binary;
        } value;
} carbon_find;

bool carbon_find_begin(carbon_find *out, const char *dot, rec *doc);
bool carbon_find_end(carbon_find *find);
bool carbon_find_create(carbon_find *find, dot *path, rec *doc);
bool carbon_find_drop(carbon_find *find);

bool carbon_find_has_result(carbon_find *find);
const char *carbon_find_result_to_str(string_buffer *dst_str, carbon_printer_impl_e print_type, carbon_find *find);
const char *carbon_find_result_to_json_compact(string_buffer *dst_str, carbon_find *find);
char *carbon_find_result_to_json_compact_dup(carbon_find *find);

bool carbon_find_result_type(field_e *type, carbon_find *find);

bool carbon_find_update_array_type(carbon_find *find, list_type_e derivation);
bool carbon_find_array_is_multiset(carbon_find *find);
bool carbon_find_array_is_sorted(carbon_find *find);

bool carbon_find_update_column_type(carbon_find *find, list_type_e derivation);
bool carbon_find_column_is_multiset(carbon_find *find);
bool carbon_find_column_is_sorted(carbon_find *find);

bool carbon_find_update_object_type(carbon_find *find, map_type_e derivation);
bool carbon_find_object_is_multimap(carbon_find *find);
bool carbon_find_object_is_sorted(carbon_find *find);

bool carbon_find_multimap(carbon_find *find);
bool carbon_find_multiset(carbon_find *find);
bool carbon_find_sorted(carbon_find *find);

arr_it *carbon_find_result_array(carbon_find *find);
obj_it *carbon_find_result_object(carbon_find *find);
col_it *carbon_find_result_column(carbon_find *find);
bool carbon_find_result_boolean(bool *result, carbon_find *find);
bool carbon_find_result_unsigned(u64 *out, carbon_find *find);
bool carbon_find_result_signed(i64 *out, carbon_find *find);
bool carbon_find_result_float(float *out, carbon_find *find);
const char *carbon_find_result_string(u64 *str_len, carbon_find *find);
binary *carbon_find_result_binary(carbon_find *find);

#ifdef __cplusplus
}
#endif

#endif
