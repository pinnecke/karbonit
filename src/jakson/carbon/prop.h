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
        (prop ? (prop)->key : NULL_STRING)

#define prop_get_type(prop)                                                                                     \
        ((prop) ? ITEM_GET_TYPE(&(prop)->value) : ITEM_UNDEF)

#define prop_is_undef(prop)                                                                                     \
        ((prop) ? ITEM_IS_UNDEF(&(prop)->value) : true)

#define prop_is_null(prop)                                                                                      \
        ((prop) ? ITEM_IS_NULL(&(prop)->value) : false)

#define prop_is_true(prop)                                                                                      \
        ((prop) ? ITEM_IS_TRUE(&(prop)->value) : false)

#define prop_is_false(prop)                                                                                     \
        ((prop) ? ITEM_IS_FALSE(&(prop)->value) : false)

#define prop_is_boolean(prop)                                                                                   \
        ((prop) ? ITEM_IS_BOOLEAN(&(prop)->value) : false)

#define prop_is_string(prop)                                                                                    \
        ((prop) ? ITEM_IS_STRING(&(prop)->value) : false)

#define prop_is_signed(prop)                                                                                    \
        ((prop) ? ITEM_IS_SIGNED(&(prop)->value) : false)

#define prop_is_unsigned(prop)                                                                                  \
        ((prop) ? ITEM_IS_UNSIGNED(&(prop)->value) : false)

#define prop_is_float(prop)                                                                                     \
        ((prop) ? ITEM_IS_FLOAT(&(prop)->value) : false)

#define prop_is_number(prop)                                                                                    \
        ((prop) ? ITEM_IS_NUMBER(&(prop)->value) : false)

#define prop_is_binary(prop)                                                                                    \
        ((prop) ? ITEM_IS_BINARY(&(prop)->value) : false)

#define prop_is_array(prop)                                                                                     \
        ((prop) ? ITEM_IS_ARRAY(&(prop)->value) : false)

#define prop_is_column(prop)                                                                                    \
        ((prop) ? ITEM_IS_COLUMN(&(prop)->value) : false)

#define prop_is_object(prop)                                                                                    \
        ((prop) ? ITEM_IS_OBJECT(&(prop)->value) : false)

#define prop_get_index(prop)                                                                                    \
        ((prop) ? (prop)->idx : 0)

#define prop_get_number_signed(prop, default_value)                                                             \
        ((prop) ? ITEM_GET_NUMBER_SIGNED(&(prop)->value) : default_value)

#define prop_get_number_unsigned(prop, default_value)                                                           \
        ((prop) ? ITEM_GET_NUMBER_UNSIGNED(&(prop)->value) : default_value)

#define prop_get_number_float(prop, default_value)                                                              \
        ((prop) ? ITEM_GET_NUMBER_FLOAT(&(prop)->value) : default_value)

#define prop_get_string(prop, default_value)                                                                    \
        ((prop) ? ITEM_GET_STRING(&(prop)->value) : default_value)

#define prop_get_binary(prop, default_value)                                                                    \
        ((prop) ? ITEM_GET_BINARY(&(prop)->value) : default_value)

#define prop_get_array(it, prop)                                                                                    \
        ITEM_GET_ARRAY((it), &(prop)->value)

#define prop_get_column(it, prop)                                                                    \
        ITEM_GET_COLUMN((it), &(prop)->value)

#define prop_get_object(it, prop)                                                                                   \
        ITEM_GET_OBJECT((it), &(prop)->value)

#define prop_remove(prop)                                                                                       \
        ((prop) ? internal_obj_it_remove(&(prop)->value) : default_value)

#define prop_set_name(prop, key_name)                                                                           \
        (internal_obj_it_update_name((prop)->parent, key_name))

#define prop_set_null(prop)                                                                                     \
        ITEM_SET_NULL(&(prop)->value)

#define prop_set_true(prop)                                                                                     \
        ITEM_SET_TRUE(&(prop)->value)

#define prop_set_false(prop)                                                                                    \
        ITEM_SET_FALSE(&(prop)->value)

#define prop_set_number_float(prop, float_value)                                                                \
        ITEM_SET_NUMBER_FLOAT(&(prop)->value, float_value)

#define prop_set_number_signed(prop, i64_value)                                                                 \
        ITEM_SET_NUMBER_SIGNED(&(prop)->value, i64_value)

#define prop_set_number_unsigned(prop, u64_value)                                                               \
        ITEM_SET_NUMBER_UNSIGNED(&(prop)->value, u64_value)

#define prop_set_string(prop, const_char_str)                                                                   \
        ITEM_SET_STRING(&(prop)->value, const_char_str)

#define prop_set_binary(prop, const_void_ptr_value, size_t_nbytes, const_char_ptr_file_ext, const_char_ptr_user_type) \
        ITEM_SET_BINARY(&(prop)->value, const_void_ptr_value, size_t_nbytes, const_char_ptr_file_ext, const_char_ptr_user_type)

#define prop_set_array_begin(state, prop)                                                                       \
        ITEM_SET_ARRAY_BEGIN(state, &(prop)->value)

#define prop_set_array_end(state)                                                                               \
        ITEM_SET_ARRAY_END(state)

#define prop_set_column_begin(state, prop)                                                                      \
        ITEM_SET_COLUMN_BEGIN(state, &(prop)->value)

#define prop_set_column_end(state)                                                                              \
        ITEM_SET_COLUMN_END(state)

#define prop_set_object_begin(state, prop)                                                                      \
        ITEM_SET_OBJECT_BEGIN(state, &(prop)->value)

#define prop_set_object_end(state)                                                                              \
        ITEM_SET_OBJECT_END(state)

#define prop_set_from_record(prop, const_carbon_ptr_src)                                                        \
        ITEM_SET_FROM_REC(&(prop)->value, const_carbon_ptr_src)

#define prop_set_from_array(prop, const_arr_it_ptr_src)                                                         \
        ITEM_SET_FROM_ARRAY(&(prop)->value, const_arr_it_ptr_src)

#define prop_set_from_object(prop, const_obj_it_ptr_src)                                                        \
        ITEM_SET_FROM_OBJECT(&(prop)->value, const_obj_it_ptr_src)

#define prop_set_from_column(prop, const_col_it_ptr_src)                                                        \
        ITEM_SET_FROM_COLUMN(&(prop)->value, const_col_it_ptr_src)

// ---------------------------------------------------------------------------------------------------------------------
//  for internal usage
// ---------------------------------------------------------------------------------------------------------------------

bool internal_prop_create(prop *dst, obj_it *parent);

u64 internal_prop_size(memfile *file);

#ifdef __cplusplus
}
#endif

#endif
