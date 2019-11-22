/*
 * prop - object container property
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_PROP_H
#define HAD_PROP_H

#include <jakson/stdinc.h>
#include <jakson/carbon/field.h>
#include <jakson/carbon/item.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct prop
{
        obj_it *parent;
        u64 idx;
        string_field  key;
        item value;
} prop;

#define prop_get_name(prop)                                                                                     \
        (prop ? (prop)->key : CARBON_NULL_STRING)

#define prop_get_type(prop)                                                                                     \
        ((prop) ? item_get_type((prop)->value) : ITEM_UNDEF)

#define prop_is_undef(prop)                                                                                     \
        ((prop) ? item_is_undef((prop)->value) : true)

#define prop_is_null(prop)                                                                                      \
        ((prop) ? item_is_null((prop)->value) : false)

#define prop_is_true(prop)                                                                                      \
        ((prop) ? item_is_true((prop)->value) : false)

#define prop_is_false(prop)                                                                                     \
        ((prop) ? item_is_false((prop)->value) : false)

#define prop_is_boolean(prop)                                                                                   \
        ((prop) ? item_is_boolean((prop)->value) : false)

#define prop_is_string(prop)                                                                                    \
        ((prop) ? item_is_string((prop)->value) : false)

#define prop_is_signed(prop)                                                                                    \
        ((prop) ? item_is_signed((prop)->value) : false)

#define prop_is_unsigned(prop)                                                                                  \
        ((prop) ? item_is_unsigned((prop)->value) : false)

#define prop_is_float(prop)                                                                                     \
        ((prop) ? item_is_float((prop)->value) : false)

#define prop_is_number(prop)                                                                                    \
        ((prop) ? item_is_number((prop)->value) : false)

#define prop_is_binary(prop)                                                                                    \
        ((prop) ? item_is_binary((prop)->value) : false)

#define prop_is_array(prop)                                                                                     \
        ((prop) ? item_is_array((prop)->value) : false)

#define prop_is_column(prop)                                                                                    \
        ((prop) ? item_is_column((prop)->value) : false)

#define prop_is_object(prop)                                                                                    \
        ((prop) ? item_is_object((prop)->value) : false)

#define prop_get_index(prop)                                                                                    \
        ((prop) ? (prop)->idx : 0)

#define prop_get_number_signed(prop, default_value)                                                             \
        ((prop) ? item_get_number_signed((prop)->value) : default_value)

#define prop_get_number_unsigned(prop, default_value)                                                           \
        ((prop) ? item_get_number_unsigned((prop)->value) : default_value)

#define prop_get_number_float(prop, default_value)                                                              \
        ((prop) ? item_get_number_float((prop)->value) : default_value)

#define prop_get_string(prop, default_value)                                                                    \
        ((prop) ? item_get_string((prop)->value) : default_value)

#define prop_get_binary(prop, default_value)                                                                    \
        ((prop) ? item_get_binary((prop)->value) : default_value)

#define prop_get_array(prop, default_value)                                                                     \
        ((prop) ? item_get_array((prop)->value) : default_value)

#define prop_get_column(prop, default_value)                                                                    \
        ((prop) ? item_get_column((prop)->value) : default_value)

#define prop_get_object(prop, default_value)                                                                    \
        ((prop) ? item_get_object((prop)->value) : default_value)

#define prop_remove(prop)                                                                                       \
        ((prop) ? internal_obj_it_remove((prop)->value) : default_value)

#define prop_set_name(prop, key_name)                                                                           \
        (internal_obj_it_update_name((prop)->parent, key_name))

#define prop_set_null(prop)                                                                                     \
        item_set_null((prop)->value)

#define prop_set_true(prop)                                                                                     \
        item_set_true((prop)->value)

#define prop_set_false(prop)                                                                                    \
        item_set_false((prop)->value)

#define prop_set_number_float(prop, float_value)                                                                \
        item_set_number_float((prop)->value, float_value)

#define prop_set_number_signed(prop, i64_value)                                                                 \
        item_set_number_signed((prop)->value, i64_value)

#define prop_set_number_unsigned(prop, u64_value)                                                               \
        item_set_number_unsigned((prop)->value, u64_value)

#define prop_set_string(prop, const_char_str)                                                                   \
        item_set_string((prop)->value, const_char_str)

#define prop_set_binary(prop, const_void_ptr_value, size_t_nbytes, const_char_ptr_file_ext, const_char_ptr_user_type) \
        item_set_binary((prop)->value, const_void_ptr_value, size_t_nbytes, const_char_ptr_file_ext, const_char_ptr_user_type)

#define prop_set_array_begin(state, prop)                                                                       \
        item_set_array_begin(state, (prop)->value)

#define prop_set_array_end(state)                                                                               \
        item_set_array_end(state)

#define prop_set_column_begin(state, prop)                                                                      \
        item_set_column_begin(state, (prop)->value)

#define prop_set_column_end(state)                                                                              \
        item_set_column_end(state)

#define prop_set_object_begin(state, prop)                                                                      \
        item_set_object_begin(state, (prop)->value)

#define prop_set_object_end(state)                                                                              \
        item_set_object_end(state)

#define prop_set_from_record(prop, const_carbon_ptr_src)                                                        \
        item_set_from_record((prop)->value, const_carbon_ptr_src)

#define prop_set_from_array(prop, const_arr_it_ptr_src)                                                   \
        item_set_from_array((prop)->value, const_arr_it_ptr_src)

#define prop_set_from_object(prop, const_obj_it_ptr_src)                                                 \
        item_set_from_object((prop)->value, const_obj_it_ptr_src)

#define prop_set_from_column(prop, const_col_it_ptr_src)                                                 \
        item_set_from_column((prop)->value, const_col_it_ptr_src)

// ---------------------------------------------------------------------------------------------------------------------
//  for internal usage
// ---------------------------------------------------------------------------------------------------------------------

bool internal_prop_create(prop *dst, obj_it *parent);

u64 internal_prop_size(memfile *file);

#ifdef __cplusplus
}
#endif

#endif
