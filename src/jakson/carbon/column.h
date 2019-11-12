/**
 * Columnar Binary JSON -- Copyright 2019 Marcus Pinnecke
 * This file implements an (read-/write) iterator for (JSON) arrays in carbon
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

#ifndef CARBON_COLUMN_H
#define CARBON_COLUMN_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/carbon/field.h>
#include <jakson/rec.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct carbon_column {
        memfile memfile;

        offset_t num_and_capacity_start_offset;
        offset_t column_start_offset;

        err err;
        carbon_field_type_e type;

        carbon_list_derivable_e abstract_type;

        /** in case of modifications (updates, inserts, deletes), the number of bytes that are added resp. removed */
        i64 mod_size;

        u32 column_capacity;
        u32 column_num_elements;

} carbon_column;

bool carbon_column_create(carbon_column *it, memfile *memfile, err *err, offset_t column_start_offset);
bool carbon_column_clone(carbon_column *dst, carbon_column *src);

bool carbon_column_insert(carbon_insert *inserter, carbon_column *it);
bool carbon_column_fast_forward(carbon_column *it);
offset_t carbon_column_memfilepos(carbon_column *it);
offset_t carbon_column_tell(carbon_column *it, u32 elem_idx);

const void *carbon_column_values(carbon_field_type_e *type, u32 *nvalues, carbon_column *it);
bool carbon_column_values_info(carbon_field_type_e *type, u32 *nvalues, carbon_column *it);

bool carbon_column_value_is_null(carbon_column *it, u32 pos);

const boolean *carbon_column_boolean_values(u32 *nvalues, carbon_column *it);
const u8 *carbon_column_u8_values(u32 *nvalues, carbon_column *it);
const u16 *carbon_column_u16_values(u32 *nvalues, carbon_column *it);
const u32 *carbon_column_u32_values(u32 *nvalues, carbon_column *it);
const u64 *carbon_column_u64_values(u32 *nvalues, carbon_column *it);
const i8 *carbon_column_i8_values(u32 *nvalues, carbon_column *it);
const i16 *carbon_column_i16_values(u32 *nvalues, carbon_column *it);
const i32 *carbon_column_i32_values(u32 *nvalues, carbon_column *it);
const i64 *carbon_column_i64_values(u32 *nvalues, carbon_column *it);
const float *carbon_column_float_values(u32 *nvalues, carbon_column *it);

bool carbon_column_remove(carbon_column *it, u32 pos);

fn_result ofType(bool) carbon_column_is_multiset(carbon_column *it);
fn_result ofType(bool) carbon_column_is_sorted(carbon_column *it);
fn_result carbon_column_update_type(carbon_column *it, carbon_list_derivable_e derivation);

bool carbon_column_update_set_null(carbon_column *it, u32 pos);
bool carbon_column_update_set_true(carbon_column *it, u32 pos);
bool carbon_column_update_set_false(carbon_column *it, u32 pos);
bool carbon_column_update_set_u8(carbon_column *it, u32 pos, u8 value);
bool carbon_column_update_set_u16(carbon_column *it, u32 pos, u16 value);
bool carbon_column_update_set_u32(carbon_column *it, u32 pos, u32 value);
bool carbon_column_update_set_u64(carbon_column *it, u32 pos, u64 value);
bool carbon_column_update_set_i8(carbon_column *it, u32 pos, i8 value);
bool carbon_column_update_set_i16(carbon_column *it, u32 pos, i16 value);
bool carbon_column_update_set_i32(carbon_column *it, u32 pos, i32 value);
bool carbon_column_update_set_i64(carbon_column *it, u32 pos, i64 value);
bool carbon_column_update_set_float(carbon_column *it, u32 pos, float value);

/**
 * Positions the iterator at the beginning of this array.
 */
bool carbon_column_rewind(carbon_column *it);

#ifdef __cplusplus
}
#endif

#endif
