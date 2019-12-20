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

#define PROP_GET_NAME(prop)                                                                                     \
        (prop ? (prop)->key : NULL_STRING)

#define PROP_GET_TYPE(prop)                                                                                     \
        ((prop) ? ITEM_GET_TYPE(&(prop)->value) : ITEM_UNDEF)

#define PROP_IS_UNDEF(prop)                                                                                     \
        ((prop) ? ITEM_IS_UNDEF(&(prop)->value) : true)

#define PROP_IS_NULL(prop)                                                                                      \
        ((prop) ? ITEM_IS_NULL(&(prop)->value) : false)

#define PROP_IS_TRUE(prop)                                                                                      \
        ((prop) ? ITEM_IS_TRUE(&(prop)->value) : false)

#define PROP_IS_FALSE(prop)                                                                                     \
        ((prop) ? ITEM_IS_FALSE(&(prop)->value) : false)

#define PROP_IS_BOOLEAN(prop)                                                                                   \
        ((prop) ? ITEM_IS_BOOLEAN(&(prop)->value) : false)

#define PROP_IS_STRING(prop)                                                                                    \
        ((prop) ? ITEM_IS_STRING(&(prop)->value) : false)

#define PROP_IS_SIGNED(prop)                                                                                    \
        ((prop) ? ITEM_IS_SIGNED(&(prop)->value) : false)

#define PROP_IS_UNSIGNED(prop)                                                                                  \
        ((prop) ? ITEM_IS_UNSIGNED(&(prop)->value) : false)

#define PROP_IS_FLOAT(prop)                                                                                     \
        ((prop) ? ITEM_IS_FLOAT(&(prop)->value) : false)

#define PROP_IS_NUMBER(prop)                                                                                    \
        ((prop) ? ITEM_IS_NUMBER(&(prop)->value) : false)

#define PROP_IS_BINARY(prop)                                                                                    \
        ((prop) ? ITEM_IS_BINARY(&(prop)->value) : false)

#define PROP_IS_ARRAY(prop)                                                                                     \
        ((prop) ? ITEM_IS_ARRAY(&(prop)->value) : false)

#define PROP_IS_COLUMN(prop)                                                                                    \
        ((prop) ? ITEM_IS_COLUMN(&(prop)->value) : false)

#define PROP_IS_OBJECT(prop)                                                                                    \
        ((prop) ? ITEM_IS_OBJECT(&(prop)->value) : false)

#define PROP_GET_INDEX(prop)                                                                                    \
        ((prop) ? (prop)->idx : 0)

#define PROP_GET_NUMBER_SIGNED(prop, default_value)                                                             \
        ((prop) ? ITEM_GET_NUMBER_SIGNED(&(prop)->value) : default_value)

#define PROP_GET_NUMBER_UNSIGNED(prop, default_value)                                                           \
        ((prop) ? ITEM_GET_NUMBER_UNSIGNED(&(prop)->value) : default_value)

#define PROP_GET_NUMBER_FLOAT(prop, default_value)                                                              \
        ((prop) ? ITEM_GET_NUMBER_FLOAT(&(prop)->value) : default_value)

#define PROP_GET_STRING(prop, default_value)                                                                    \
        ((prop) ? ITEM_GET_STRING(&(prop)->value) : default_value)

#define PROP_GET_BINARY(prop, default_value)                                                                    \
        ((prop) ? ITEM_GET_BINARY(&(prop)->value) : default_value)

#define PROP_GET_ARRAY(it, prop)                                                                                    \
        ITEM_GET_ARRAY((it), &(prop)->value)

#define PROP_GET_COLUMN(it, prop)                                                                    \
        ITEM_GET_COLUMN((it), &(prop)->value)

#define PROP_GET_OBJECT(it, prop)                                                                                   \
        ITEM_GET_OBJECT((it), &(prop)->value)

#define PROP_REMOVE(prop)                                                                                       \
        ((prop) ? internal_obj_it_remove(&(prop)->value) : default_value)

#define PROP_SET_NAME(prop, key_name)                                                                           \
        (internal_obj_it_update_name((prop)->parent, key_name))

#define PROP_SET_NULL(prop)                                                                                     \
        ITEM_SET_NULL(&(prop)->value)

#define PROP_SET_TRUE(prop)                                                                                     \
        ITEM_SET_TRUE(&(prop)->value)

#define PROP_SET_FALSE(prop)                                                                                    \
        ITEM_SET_FALSE(&(prop)->value)

#define PROP_SET_NUMBER_FLOAT(prop, float_value)                                                                \
        ITEM_SET_NUMBER_FLOAT(&(prop)->value, float_value)

#define PROP_SET_NUMBER_SIGNED(prop, i64_value)                                                                 \
        ITEM_SET_NUMBER_SIGNED(&(prop)->value, i64_value)

#define PROP_SET_NUMBER_UNSIGNED(prop, u64_value)                                                               \
        ITEM_SET_NUMBER_UNSIGNED(&(prop)->value, u64_value)

#define PROP_SET_STRING(prop, const_char_str)                                                                   \
        ITEM_SET_STRING(&(prop)->value, const_char_str)

#define PROP_SET_BINARY(prop, const_void_ptr_value, size_t_nbytes, const_char_ptr_file_ext, const_char_ptr_user_type) \
        ITEM_SET_BINARY(&(prop)->value, const_void_ptr_value, size_t_nbytes, const_char_ptr_file_ext, const_char_ptr_user_type)

#define PROP_SET_ARRAY_BEGIN(state, prop)                                                                       \
        ITEM_SET_ARRAY_BEGIN(state, &(prop)->value)

#define PROP_SET_ARRAY_END(state)                                                                               \
        ITEM_SET_ARRAY_END(state)

#define PROP_SET_COLUMN_BEGIN(state, prop)                                                                      \
        ITEM_SET_COLUMN_BEGIN(state, &(prop)->value)

#define PROP_SET_COLUMN_END(state)                                                                              \
        ITEM_SET_COLUMN_END(state)

#define PROP_SET_OBJECT_BEGIN(state, prop)                                                                      \
        ITEM_SET_OBJECT_BEGIN(state, &(prop)->value)

#define PROP_SET_OBJECT_END(state)                                                                              \
        ITEM_SET_OBJECT_END(state)

#define PROP_SET_FROM_REC(prop, const_carbon_ptr_src)                                                        \
        ITEM_SET_FROM_REC(&(prop)->value, const_carbon_ptr_src)

#define PROP_SET_FROM_ARRAY(prop, const_arr_it_ptr_src)                                                         \
        ITEM_SET_FROM_ARRAY(&(prop)->value, const_arr_it_ptr_src)

#define PROP_SET_FROM_OBJECT(prop, const_obj_it_ptr_src)                                                        \
        ITEM_SET_FROM_OBJECT(&(prop)->value, const_obj_it_ptr_src)

#define PROP_SET_FROM_COLUMN(prop, const_col_it_ptr_src)                                                        \
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
