/*
 * Copyright 2019 Marcus Pinnecke
 */
#ifndef HAD_CARBON_ITEM_H
#define HAD_CARBON_ITEM_H

#include <jakson/stdinc.h>
#include <jakson/types.h>
#include <jakson/forwdecl.h>
#include <jakson/carbon/binary.h>
#include <jakson/carbon/string.h>

BEGIN_DECL

typedef enum carbon_item_type
{
    CARBON_ITEM_UNDEF,
    CARBON_ITEM_NULL,
    CARBON_ITEM_TRUE,
    CARBON_ITEM_FALSE,
    CARBON_ITEM_STRING,
    CARBON_ITEM_NUMBER_SIGNED,
    CARBON_ITEM_NUMBER_UNSIGNED,
    CARBON_ITEM_NUMBER_FLOAT,
    CARBON_ITEM_BINARY,
    CARBON_ITEM_ARRAY,
    CARBON_ITEM_COLUMN,
    CARBON_ITEM_OBJECT
} carbon_item_type_e;

typedef struct carbon_item
{
    carbon_array *parent;
    u64 idx;
    carbon_item_type_e value_type;
    union {
        i64 number_signed;
        u64 number_unsigned;
        float number_float;
        carbon_string_field string;
        carbon_binary binary;
        carbon_array *array;
        carbon_column *column;
        carbon_object *object;
    } value;
} carbon_item;

#define INTERNAL_CARBON_ITEM_IS_TYPE(item, type)                                                                       \
        (carbon_item_get_type(item) == type)

#define INTERNAL_CARBON_ITEM_GET_VALUE(item, member, default_value)                                                    \
        ((item) ? (item)->value.member : default_value)

#define carbon_item_get_type(item)                                                                                     \
        ((item) ? (item)->value_type : CARBON_ITEM_UNDEF)

#define carbon_item_is_undef(item)                                                                                     \
        INTERNAL_CARBON_ITEM_IS_TYPE(item, CARBON_ITEM_UNDEF)

#define carbon_item_is_null(item)                                                                                      \
        INTERNAL_CARBON_ITEM_IS_TYPE(item, CARBON_ITEM_NULL)

#define carbon_item_is_true(item)                                                                                      \
        INTERNAL_CARBON_ITEM_IS_TYPE(item, CARBON_ITEM_TRUE)

#define carbon_item_is_false(item)                                                                                     \
        INTERNAL_CARBON_ITEM_IS_TYPE(item, CARBON_ITEM_FALSE)

#define carbon_item_is_boolean(item)                                                                                   \
        (carbon_item_is_true(item) || carbon_item_is_false(item))

#define carbon_item_is_string(item)                                                                                    \
        INTERNAL_CARBON_ITEM_IS_TYPE(item, CARBON_ITEM_STRING)

#define carbon_item_is_signed(item)                                                                                    \
        INTERNAL_CARBON_ITEM_IS_TYPE(item, CARBON_ITEM_NUMBER_SIGNED)

#define carbon_item_is_unsigned(item)                                                                                  \
        INTERNAL_CARBON_ITEM_IS_TYPE(item, CARBON_ITEM_NUMBER_UNSIGNED)

#define carbon_item_is_float(item)                                                                                     \
        INTERNAL_CARBON_ITEM_IS_TYPE(item, CARBON_ITEM_NUMBER_FLOAT)

#define carbon_item_is_number(item)                                                                                    \
        (carbon_item_is_signed(item) || carbon_item_is_unsigned(item) || carbon_item_is_float(item))

#define carbon_item_is_binary(item)                                                                                    \
        INTERNAL_CARBON_ITEM_IS_TYPE(item, CARBON_ITEM_BINARY)

#define carbon_item_is_array(item)                                                                                     \
        INTERNAL_CARBON_ITEM_IS_TYPE(item, CARBON_ITEM_ARRAY)

#define carbon_item_is_column(item)                                                                                    \
        INTERNAL_CARBON_ITEM_IS_TYPE(item, CARBON_ITEM_COLUMN)

#define carbon_item_is_object(item)                                                                                    \
        INTERNAL_CARBON_ITEM_IS_TYPE(item, CARBON_ITEM_OBJECT)

#define carbon_item_get_index(item)                                                                                    \
        ((item) ? (item)->idx : 0)

#define carbon_item_get_number_signed(item, default_value)                                                             \
        INTERNAL_CARBON_ITEM_GET_VALUE(item, number_signed, default_value)

#define carbon_item_get_number_unsigned(item, default_value)                                                           \
        INTERNAL_CARBON_ITEM_GET_VALUE(item, number_unsigned, default_value)

#define carbon_item_get_number_float(item, default_value)                                                              \
        INTERNAL_CARBON_ITEM_GET_VALUE(item, number_float, default_value)

#define carbon_item_get_string(item, default_value)                                                                    \
        INTERNAL_CARBON_ITEM_GET_VALUE(item, string, default_value)

#define carbon_item_get_binary(item, default_value)                                                                    \
        INTERNAL_CARBON_ITEM_GET_VALUE(item, binary, default_value)

#define carbon_item_get_array(item)                                                                                    \
        INTERNAL_CARBON_ITEM_GET_VALUE(item, array, NULL)

#define carbon_item_get_column(item)                                                                                   \
        INTERNAL_CARBON_ITEM_GET_VALUE(item, column, NULL)

#define carbon_item_get_object(item)                                                                                   \
        INTERNAL_CARBON_ITEM_GET_VALUE(item, object, NULL)

bool carbon_item_remove(const carbon_item *item);

void carbon_item_set_null(const carbon_item *item);
void carbon_item_set_true(const carbon_item *item);
void carbon_item_set_false(const carbon_item *item);
i64 carbon_item_set_number_signed(const carbon_item *item);
u64 carbon_item_set_number_unsigned(const carbon_item *item);
float carbon_item_set_number_float(const carbon_item *item);
const char *carbon_item_set_string(u64 *str_len, const carbon_item *item);
const carbon_binary *carbon_item_set_binary(const carbon_item *item);
carbon_array *carbon_item_set_array(const carbon_item *item);
carbon_column *carbon_item_set_column(const carbon_item *item);
carbon_object *carbon_item_set_object(const carbon_item *item);

bool internal_carbon_item_create(carbon_item *item, carbon_array *parent);

END_DECL

#endif

