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

#define carbon_prop_remove(prop)                                                                    \
        ((prop) ? carbon_item_get_object((prop)->value) : default_value)



u64 internal_carbon_prop_size(memfile *file);

END_DECL

#endif
