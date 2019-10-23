/*
 * Copyright 2019 Marcus Pinnecke
 */
#ifndef HAD_CARBON_ITEM_H
#define HAD_CARBON_ITEM_H

#include <jakson/stdinc.h>
#include <jakson/types.h>
#include <jakson/carbon/array.h>
#include <jakson/carbon/object.h>
#include <jakson/carbon/column.h>

BEGIN_DECL

typedef enum carbon_item_type
{
    CARBON_ITEM_UNKNOWN,
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

    carbon_item_type_e value_type;
    union {
        i64 number_signed;
        u64 number_unsigned;
        float number_float;
        struct {
            u64 len;
            const char *base;
        } string;
        carbon_binary binary;
        carbon_array *array;
        carbon_column *column;
        carbon_object *object;
    } value;
} carbon_item;

carbon_item_type_e carbon_item_get_type(const carbon_item *item);

bool carbon_item_remove(const carbon_item *item);

bool carbon_item_is_null(const carbon_item *item);
i64 carbon_item_get_number_signed(const carbon_item *item);
u64 carbon_item_get_number_unsigned(const carbon_item *item);
float carbon_item_get_number_float(const carbon_item *item);
const char *carbon_item_get_string(u64 *str_len, const carbon_item *item);
const carbon_binary *carbon_item_get_binary(const carbon_item *item);
carbon_array *carbon_item_get_array(const carbon_item *item);
carbon_column *carbon_item_get_column(const carbon_item *item);
carbon_object *carbon_item_get_object(const carbon_item *item);

void carbon_item_set_null(const carbon_item *item);
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

