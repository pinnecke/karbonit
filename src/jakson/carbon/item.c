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
#include <jakson/carbon/arr_it.h>
#include <jakson/carbon/object.h>
#include <jakson/carbon/internal.h>
#include <jakson/utils/numbers.h>

static bool carbon_item_setup_value(item *item, field_type_e field_type, field *field)
{
        if (field_type_is_signed(field_type) && !field_type_is_list_or_subtype(field_type)) {
                carbon_int_field_signed_value(&item->value.number_signed, field);
                item->value_type = CARBON_ITEM_NUMBER_SIGNED;
        } else if (field_type_is_unsigned(field_type) && !field_type_is_list_or_subtype(field_type)) {
                carbon_int_field_unsigned_value(&item->value.number_unsigned, field);
                item->value_type = CARBON_ITEM_NUMBER_UNSIGNED;
        } else if (field_type_is_floating(field_type) && !field_type_is_list_or_subtype(field_type)) {
                carbon_int_field_float_value(&item->value.number_float, field);
                item->value_type = CARBON_ITEM_NUMBER_FLOAT;
        } else if (field_type_is_binary(field_type)) {
                carbon_int_field_binary_value(&item->value.binary, field);
                item->value_type = CARBON_ITEM_BINARY;
        } else if (field_type_is_boolean(field_type) && !field_type_is_list_or_subtype(field_type)) {
                item->value_type = field_type == FIELD_TRUE ? CARBON_ITEM_TRUE : CARBON_ITEM_FALSE;
        } else if (field_type_is_array_or_subtype(field_type)) {
                item->value.array = carbon_int_field_array_value(field);
                item->value_type = CARBON_ITEM_ARRAY;
        } else if (field_type_is_column_or_subtype(field_type)) {
                item->value.column = carbon_int_field_column_value(field);
                item->value_type = CARBON_ITEM_COLUMN;
        } else if (field_type_is_object_or_subtype(field_type)) {
                item->value.object = carbon_int_field_object_value(field);
                item->value_type = CARBON_ITEM_OBJECT;
        } else if (field_type_is_null(field_type)) {
                item->value_type = CARBON_ITEM_NULL;
        } else if (field_type_is_string(field_type)) {
                item->value.string.string = carbon_int_field_string_value(&item->value.string.length, field);
                item->value_type = CARBON_ITEM_STRING;
        } else {
                item->value_type = CARBON_ITEM_UNDEF;
                return false;
        }
        return true;
}

bool internal_carbon_item_create_from_array(item *item, arr_it *parent)
{
        item->parent_type = CARBON_PARENT_ARRAY;
        item->parent.array = parent;
        item->idx = parent->pos;
        field_type_e field_type = parent->field.type;

        return carbon_item_setup_value(item, field_type, &parent->field);
}

bool internal_carbon_item_create_from_object(item *item, obj_it *parent)
{
        item->parent_type = CARBON_PARENT_OBJECT;
        item->parent.object = parent;
        item->idx = parent->pos;
        field_type_e field_type = parent->field.value.data.type;

        return carbon_item_setup_value(item, field_type, &parent->field.value.data);
}