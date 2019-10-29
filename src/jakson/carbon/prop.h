/*
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_CARBON_PROP_H
#define HAD_CARBON_PROP_H

#include <jakson/stdinc.h>
#include <jakson/carbon/field.h>
#include <jakson/carbon/item.h>

BEGIN_DECL

typedef struct carbon_prop
{
        carbon_object       *parent;
        u64                  idx;
        carbon_string_field  key;
        carbon_item          value;
} carbon_prop;

#define carbon_prop_get_name(prop)                                                                                     \
        (prop ? (prop)->key : CARBON_NULL_STRING)

#define carbon_prop_get_type(prop)                                                                                     \
        ((prop) ? carbon_item_get_type((prop)->value) : CARBON_ITEM_UNDEF)

#define carbon_prop_is_undef(prop)                                                                                     \
        ((prop) ? carbon_item_is_undef((prop)->value) : true)

#define carbon_prop_is_null(prop)                                                                                      \
        ((prop) ? carbon_item_is_null((prop)->value) : false)

#define carbon_prop_is_true(prop)                                                                                      \
        ((prop) ? carbon_item_is_true((prop)->value) : false)

#define carbon_prop_is_false(prop)                                                                                     \
        ((prop) ? carbon_item_is_false((prop)->value) : false)

#define carbon_prop_is_boolean(prop)                                                                                   \
        ((prop) ? carbon_item_is_boolean((prop)->value) : false)

#define carbon_prop_is_string(prop)                                                                                    \
        ((prop) ? carbon_item_is_string((prop)->value) : false)

#define carbon_prop_is_signed(prop)                                                                                    \
        ((prop) ? carbon_item_is_signed((prop)->value) : false)

#define carbon_prop_is_unsigned(prop)                                                                                  \
        ((prop) ? carbon_item_is_unsigned((prop)->value) : false)

#define carbon_prop_is_float(prop)                                                                                     \
        ((prop) ? carbon_item_is_float((prop)->value) : false)

#define carbon_prop_is_number(prop)                                                                                    \
        ((prop) ? carbon_item_is_number((prop)->value) : false)

#define carbon_prop_is_binary(prop)                                                                                    \
        ((prop) ? carbon_item_is_binary((prop)->value) : false)

#define carbon_prop_is_array(prop)                                                                                     \
        ((prop) ? carbon_item_is_array((prop)->value) : false)

#define carbon_prop_is_column(prop)                                                                                    \
        ((prop) ? carbon_item_is_column((prop)->value) : false)

#define carbon_prop_is_object(prop)                                                                                    \
        ((prop) ? carbon_item_is_object((prop)->value) : false)

#define carbon_prop_get_index(prop)                                                                                    \
        ((prop) ? (prop)->idx : 0)

#define carbon_prop_get_number_signed(prop, default_value)                                                             \
        ((prop) ? carbon_item_get_number_signed((prop)->value) : default_value)

#define carbon_prop_get_number_unsigned(prop, default_value)                                                           \
        ((prop) ? carbon_item_get_number_unsigned((prop)->value) : default_value)

#define carbon_prop_get_number_float(prop, default_value)                                                              \
        ((prop) ? carbon_item_get_number_float((prop)->value) : default_value)

#define carbon_prop_get_string(prop, default_value)                                                                    \
        ((prop) ? carbon_item_get_string((prop)->value) : default_value)

#define carbon_prop_get_binary(prop, default_value)                                                                    \
        ((prop) ? carbon_item_get_binary((prop)->value) : default_value)

#define carbon_prop_get_array(prop, default_value)                                                                     \
        ((prop) ? carbon_item_get_array((prop)->value) : default_value)

#define carbon_prop_get_column(prop, default_value)                                                                    \
        ((prop) ? carbon_item_get_column((prop)->value) : default_value)

#define carbon_prop_get_object(prop, default_value)                                                                    \
        ((prop) ? carbon_item_get_object((prop)->value) : default_value)

#define carbon_prop_remove(prop)                                                                                       \
        ((prop) ? internal_carbon_object_remove((prop)->value) : default_value)

#define carbon_prop_set_name(prop, key_name)                                                                           \
        (internal_carbon_object_update_name((prop)->parent, key_name))

#define carbon_prop_set_null(prop)                                                                                     \
        carbon_item_set_null((prop)->value)

#define carbon_prop_set_true(prop)                                                                                     \
        carbon_item_set_true((prop)->value)

#define carbon_prop_set_false(prop)                                                                                    \
        carbon_item_set_false((prop)->value)

#define carbon_prop_set_number_float(prop, float_value)                                                                \
        carbon_item_set_number_float((prop)->value, float_value)

#define carbon_prop_set_number_signed(prop, i64_value)                                                                 \
        carbon_item_set_number_signed((prop)->value, i64_value)

#define carbon_prop_set_number_unsigned(prop, u64_value)                                                               \
        carbon_item_set_number_unsigned((prop)->value, u64_value)

#define carbon_prop_set_string(prop, const_char_str)                                                                   \
        carbon_item_set_string((prop)->value, const_char_str)

#define carbon_prop_set_binary(prop, const_void_ptr_value, size_t_nbytes, const_char_ptr_file_ext, const_char_ptr_user_type) \
        carbon_item_set_binary((prop)->value, const_void_ptr_value, size_t_nbytes, const_char_ptr_file_ext, const_char_ptr_user_type)

#define carbon_prop_set_array_begin(state, prop)                                                                       \
        carbon_item_set_array_begin(state, (prop)->value)

#define carbon_prop_set_array_end(state)                                                                               \
        carbon_item_set_array_end

#define carbon_prop_set_column_begin(state, prop)                                                                      \
        carbon_item_set_column_begin(state, (prop)->value)

#define carbon_prop_set_column_end(state)                                                                              \
        carbon_item_set_column_end(state)

#define carbon_prop_set_object_begin(state, prop)                                                                      \
        carbon_item_set_object_begin(state, (prop)->value)

#define carbon_prop_set_object_end(state)                                                                              \
        carbon_item_set_object_end(state)

#define carbon_prop_set_from_carbon(prop, const_carbon_ptr_src)                                                        \
        carbon_item_set_from_carbon((prop)->value, const_carbon_ptr_src)

#define carbon_prop_set_from_array(prop, const_carbon_array_ptr_src)                                                   \
        carbon_item_set_from_array((prop)->value, const_carbon_array_ptr_src)

#define carbon_prop_set_from_object(prop, const_carbon_object_ptr_src)                                                 \
        carbon_item_set_from_object((prop)->value, const_carbon_object_ptr_src)

#define carbon_prop_set_from_column(prop, const_carbon_column_ptr_src)                                                 \
        carbon_item_set_from_column((prop)->value, const_carbon_column_ptr_src)

// ---------------------------------------------------------------------------------------------------------------------
//  for internal usage
// ---------------------------------------------------------------------------------------------------------------------

u64 internal_carbon_prop_size(memfile *file);

END_DECL

#endif
