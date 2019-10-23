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

struct carbon_item
{

};

#define CARBON_ITEM_UPDATE_FUNCTION(type_name)                                                                    \
bool carbon_item_update_in_place_##type_name(carbon_array *it, type_name value);

CARBON_ITEM_UPDATE_FUNCTION(u8)
CARBON_ITEM_UPDATE_FUNCTION(u16)
CARBON_ITEM_UPDATE_FUNCTION(u32)
CARBON_ITEM_UPDATE_FUNCTION(u64)
CARBON_ITEM_UPDATE_FUNCTION(i8)
CARBON_ITEM_UPDATE_FUNCTION(i16)
CARBON_ITEM_UPDATE_FUNCTION(i32)
CARBON_ITEM_UPDATE_FUNCTION(i64)
CARBON_ITEM_UPDATE_FUNCTION(float)

bool carbon_item_update_in_place_true(struct carbon_item *item);
bool carbon_item_update_in_place_false(struct carbon_item *item);
bool carbon_item_update_in_place_null(struct carbon_item *item);

bool carbon_item_type(carbon_field_type_e *type, struct carbon_item *item);
bool carbon_item_bool_value(bool *value, struct carbon_item *item);
bool carbon_item_is_null(bool *is_null, struct carbon_item *item);
bool carbon_item_u8_value(u8 *value, struct carbon_item *item);
bool carbon_item_u16_value(u16 *value, struct carbon_item *item);
bool carbon_item_u32_value(u32 *value, struct carbon_item *item);
bool carbon_item_u64_value(u64 *value, struct carbon_item *item);
bool carbon_item_i8_value(i8 *value, struct carbon_item *item);
bool carbon_item_i16_value(i16 *value, struct carbon_item *item);
bool carbon_item_i32_value(i32 *value, struct carbon_item *item);
bool carbon_item_i64_value(i64 *value, struct carbon_item *item);
bool carbon_item_float_value(float *value, struct carbon_item *item);
bool carbon_item_float_value_nullable(bool *is_null_in, float *value, struct carbon_item *item);
bool carbon_item_signed_value(bool *is_null_in, i64 *value, struct carbon_item *item);
bool carbon_item_unsigned_value(bool *is_null_in, u64 *value, struct carbon_item *item);
const char *carbon_item_string_value(u64 *strlen, struct carbon_item *item);
bool carbon_item_binary_value(carbon_binary *out, struct carbon_item *item);
carbon_array *carbon_item_array_value(struct carbon_item *item);
carbon_object_it *carbon_item_object_value(struct carbon_item *item);
carbon_column *carbon_item_column_value(struct carbon_item *item);

/**
 * Inserts a new element at the current position of the iterator.
 */
fn_result carbon_item_insert_begin(carbon_insert *inserter, struct carbon_item *item);
fn_result carbon_item_insert_end(carbon_insert *inserter);
bool carbon_item_remove(struct carbon_item *item);
