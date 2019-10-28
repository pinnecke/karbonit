/**
 * Copyright 2019 Marcus Pinnecke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of
 * the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <jakson/carbon/item.h>
#include <jakson/carbon/array.h>
#include <jakson/carbon/internal.h>

bool carbon_item_remove(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

void carbon_item_set_null(const carbon_item *item)
{
        UNUSED(item);
}

void carbon_item_set_true(const carbon_item *item)
{
        UNUSED(item);
}

void carbon_item_set_false(const carbon_item *item)
{
        UNUSED(item);
}

i64 carbon_item_set_number_signed(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

u64 carbon_item_set_number_unsigned(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

float carbon_item_set_number_float(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

const char *carbon_item_set_string(u64 *str_len, const carbon_item *item)
{
        UNUSED(item);
        UNUSED(str_len);
        return 0;
}

const carbon_binary *carbon_item_set_binary(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

carbon_array *carbon_item_set_array(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

carbon_column *carbon_item_set_column(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

carbon_object *carbon_item_set_object(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

bool internal_carbon_item_create(carbon_item *item, carbon_array *parent)
{
        item->parent = parent;
        item->idx = parent->pos;
        carbon_field_type_e field_type = parent->field_access.it_field_type;

        if (carbon_field_type_is_signed(field_type)) {
                carbon_int_field_access_signed_value(&item->value.number_signed, &parent->field_access, &parent->err);
                item->value_type = CARBON_ITEM_NUMBER_SIGNED;
        } else if (carbon_field_type_is_unsigned(field_type)) {
                carbon_int_field_access_unsigned_value(&item->value.number_unsigned, &parent->field_access, &parent->err);
                item->value_type = CARBON_ITEM_NUMBER_UNSIGNED;
        } else if (carbon_field_type_is_floating(field_type)) {
                carbon_int_field_access_float_value(&item->value.number_float, &parent->field_access, &parent->err);
                item->value_type = CARBON_ITEM_NUMBER_FLOAT;
        } else if (carbon_field_type_is_binary(field_type)) {
                carbon_int_field_access_binary_value(&item->value.binary, &parent->field_access, &parent->err);
                item->value_type = CARBON_ITEM_BINARY;
        } else if (carbon_field_type_is_boolean(field_type)) {
                item->value_type = field_type == CARBON_FIELD_TRUE ? CARBON_ITEM_TRUE : CARBON_ITEM_FALSE;
        } else if (carbon_field_type_is_array_or_subtype(field_type)) {
                item->value.array = carbon_int_field_access_array_value(&parent->field_access, &parent->err);
                item->value_type = CARBON_ITEM_ARRAY;
        } else if (carbon_field_type_is_column_or_subtype(field_type)) {
                item->value.column = carbon_int_field_access_column_value(&parent->field_access, &parent->err);
                item->value_type = CARBON_ITEM_COLUMN;
        } else if (carbon_field_type_is_object_or_subtype(field_type)) {
                item->value.object = carbon_int_field_access_object_value(&parent->field_access, &parent->err);
                item->value_type = CARBON_ITEM_OBJECT;
        } else if (carbon_field_type_is_null(field_type)) {
                item->value_type = CARBON_ITEM_NULL;
        } else if (carbon_field_type_is_string(field_type)) {
                item->value.string.string = carbon_int_field_access_string_value(&item->value.string.length,
                                                                                 &parent->field_access, &parent->err);
                item->value_type = CARBON_ITEM_STRING;
        } else {
                item->value_type = CARBON_ITEM_UNDEF;
                return false;
        }
        return true;
}