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

#ifndef CARBON_ARRAY_IT_H
#define CARBON_ARRAY_IT_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/std/vector.h>
#include <jakson/carbon.h>
#include <jakson/carbon/field.h>

BEGIN_DECL

// ---------------------------------------------------------------------------------------------------------------------
//  public interface
// ---------------------------------------------------------------------------------------------------------------------

struct carbon_array {
        memfile memfile;
        offset_t begin;
        err err;

        carbon_list_derivable_e abstract_type;

        /** in case of modifications (updates, inserts, deletes), the number of bytes that are added resp. removed */
        i64 mod_size;
        bool array_end_reached;

        vector ofType(offset_t) history;
        struct carbon_int_field_access field_access;
        offset_t field_offset;
};

/**
 * Constructs a new array iterator in a carbon document, where <code>payload_start</code> is a memory offset
 * that starts with the first (potentially empty) array entry. If there is some data before the array contents
 * (e.g., a header), <code>payload_start</code> must not include this data.
 */
fn_result carbon_array_it_create(struct carbon_array *it, memfile *memfile, err *err, offset_t payload_start);

/**
 * Drops the iterator.
 */
fn_result carbon_array_it_drop(struct carbon_array *it);

bool carbon_array_it_copy(struct carbon_array *dst, struct carbon_array *src);

bool carbon_array_it_clone(struct carbon_array *dst, struct carbon_array *src);

bool carbon_array_it_length(u64 *len, struct carbon_array *it);

bool carbon_array_it_is_empty(struct carbon_array *it);

/**
 * Positions the iterator at the beginning of this array.
 */
bool carbon_array_it_rewind(struct carbon_array *it);

/**
 * Positions the iterator to the slot after the current element, potentially pointing to next element.
 * The function returns true, if the slot is non-empty, and false otherwise.
 */
bool carbon_array_it_next(struct carbon_array *it);
bool carbon_array_it_has_next(struct carbon_array *it);
bool carbon_array_it_is_unit(struct carbon_array *it);
bool carbon_array_it_prev(struct carbon_array *it);

/**
 * Inserts a new element at the current position of the iterator.
 */
fn_result carbon_array_it_insert_begin(carbon_insert *inserter, struct carbon_array *it);
fn_result carbon_array_it_insert_end(carbon_insert *inserter);

bool carbon_array_it_remove(struct carbon_array *it);

/** Checks if this array is annotated as a multi set abstract type. Returns true if it is is a multi set, and false if
 * it is a set. In case of any error, a failure is returned. */
fn_result ofType(bool) carbon_array_it_is_multiset(struct carbon_array *it);

/** Checks if this array is annotated as a sorted abstract type. Returns true if this is the case,
 * otherwise false. In case of any error, a failure is returned. */
fn_result ofType(bool) carbon_array_it_is_sorted(struct carbon_array *it);

/** Updates this arrays abstract type to the given abstract type */
fn_result carbon_array_it_update_type(struct carbon_array *it, carbon_list_derivable_e derivation);

// ---------------------------------------------------------------------------------------------------------------------
//  for internal usage
// ---------------------------------------------------------------------------------------------------------------------

#define DECLARE_IN_PLACE_UPDATE_FUNCTION(type_name)                                                                    \
bool carbon_int_array_update_##type_name(struct carbon_array *it, type_name value);

DECLARE_IN_PLACE_UPDATE_FUNCTION(u8)
DECLARE_IN_PLACE_UPDATE_FUNCTION(u16)
DECLARE_IN_PLACE_UPDATE_FUNCTION(u32)
DECLARE_IN_PLACE_UPDATE_FUNCTION(u64)
DECLARE_IN_PLACE_UPDATE_FUNCTION(i8)
DECLARE_IN_PLACE_UPDATE_FUNCTION(i16)
DECLARE_IN_PLACE_UPDATE_FUNCTION(i32)
DECLARE_IN_PLACE_UPDATE_FUNCTION(i64)
DECLARE_IN_PLACE_UPDATE_FUNCTION(float)

bool carbon_int_array_update_true(struct carbon_array *it);
bool carbon_int_array_update_false(struct carbon_array *it);
bool carbon_int_array_update_null(struct carbon_array *it);

offset_t carbon_array_it_memfilepos(struct carbon_array *it);
offset_t carbon_array_it_tell(struct carbon_array *it);
bool carbon_int_array_it_offset(offset_t *off, struct carbon_array *it);
bool carbon_array_it_fast_forward(struct carbon_array *it);

bool carbon_array_it_field_type(carbon_field_type_e *type, struct carbon_array *it);
bool carbon_array_it_bool_value(bool *value, struct carbon_array *it);
bool carbon_array_it_is_null(bool *is_null, struct carbon_array *it);
bool carbon_array_it_u8_value(u8 *value, struct carbon_array *it);
bool carbon_array_it_u16_value(u16 *value, struct carbon_array *it);
bool carbon_array_it_u32_value(u32 *value, struct carbon_array *it);
bool carbon_array_it_u64_value(u64 *value, struct carbon_array *it);
bool carbon_array_it_i8_value(i8 *value, struct carbon_array *it);
bool carbon_array_it_i16_value(i16 *value, struct carbon_array *it);
bool carbon_array_it_i32_value(i32 *value, struct carbon_array *it);
bool carbon_array_it_i64_value(i64 *value, struct carbon_array *it);
bool carbon_array_it_float_value(float *value, struct carbon_array *it);
bool carbon_array_it_float_value_nullable(bool *is_null_in, float *value, struct carbon_array *it);
bool carbon_array_it_signed_value(bool *is_null_in, i64 *value, struct carbon_array *it);
bool carbon_array_it_unsigned_value(bool *is_null_in, u64 *value, struct carbon_array *it);
const char *carbon_array_it_string_value(u64 *strlen, struct carbon_array *it);
bool carbon_array_it_binary_value(carbon_binary *out, struct carbon_array *it);
struct carbon_array *carbon_array_it_array_value(struct carbon_array *it_in);
carbon_object_it *carbon_array_it_object_value(struct carbon_array *it_in);
carbon_column_it *carbon_array_it_column_value(struct carbon_array *it_in);
bool carbon_int_array_set_mode(struct carbon_array *it, access_mode_e mode);

END_DECL

#endif
