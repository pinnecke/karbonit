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

carbon_item_type_e carbon_item_get_type(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

bool carbon_item_remove(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

bool carbon_item_is_null(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

i64 carbon_item_get_number_signed(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

u64 carbon_item_get_number_unsigned(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

float carbon_item_get_number_float(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

const char *carbon_item_get_string(u64 *str_len, const carbon_item *item)
{
        UNUSED(item);
        UNUSED(str_len);
        return 0;
}

const carbon_binary *carbon_item_get_binary(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

carbon_array *carbon_item_get_array(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

carbon_column *carbon_item_get_column(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

carbon_object *carbon_item_get_object(const carbon_item *item)
{
        UNUSED(item);
        return 0;
}

void carbon_item_set_null(const carbon_item *item)
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
        UNUSED(item);
        UNUSED(parent);
        return 0;
}