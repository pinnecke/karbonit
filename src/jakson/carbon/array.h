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

#ifndef CARBON_ARRAY_H
#define CARBON_ARRAY_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/forwdecl.h>
#include <jakson/error.h>
#include <jakson/std/vector.h>
#include <jakson/rec.h>
#include <jakson/carbon/field.h>
#include <jakson/carbon/item.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------------------------------------------------
//  public structures
// ---------------------------------------------------------------------------------------------------------------------

typedef struct field_access {
        carbon_field_type_e it_field_type;

        const void *it_field_data;
        u64 it_field_len;

        const char *it_mime_type;
        u64 it_mime_type_strlen;

        bool nested_array_is_created;
        bool nested_array_accessed;

        bool nested_object_it_is_created;
        bool nested_object_it_accessed;

        bool nested_column_it_is_created;

        carbon_array *nested_array;
        carbon_column *nested_column_it;
        carbon_object *nested_object_it;
} field_access;

typedef struct carbon_array {
        memfile memfile;
        offset_t array_begin_off;
        err err;

        carbon_list_derivable_e abstract_type;
        u64 pos;

        carbon_item item;

        /** in case of modifications (updates, inserts, deletes), the number of bytes that are added resp. removed */
        i64 mod_size;
        bool array_end_reached;

        vector ofType(offset_t) history;
        field_access field_access;
        offset_t field_offset;
} carbon_array;

// ---------------------------------------------------------------------------------------------------------------------
//  public interface
// ---------------------------------------------------------------------------------------------------------------------

/**
 * Drops the iterator.
 */
fn_result carbon_array_drop(carbon_array *it);

/**
 * Positions the iterator at the beginning of this array.
 */
bool carbon_array_rewind(carbon_array *it);

bool carbon_array_length(u64 *len, carbon_array *it);

bool carbon_array_is_empty(carbon_array *it);

/**
 * Positions the iterator to the slot after the current element, potentially pointing to next element.
 * The function returns true, if the slot is non-empty, and false otherwise.
 */
carbon_item *carbon_array_next(carbon_array *it);

bool carbon_array_has_next(carbon_array *it);

bool carbon_array_is_unit(carbon_array *it);

bool carbon_array_prev(carbon_array *it);

/**
 * Inserts a new element at the current position of the iterator.
 */
fn_result carbon_array_insert_begin(carbon_insert *inserter, carbon_array *it);
fn_result carbon_array_insert_end(carbon_insert *inserter);

/** Checks if this array is annotated as a multi set abstract type. Returns true if it is is a multi set, and false if
 * it is a set. In case of any error, a failure is returned. */
fn_result ofType(bool) carbon_array_is_multiset(carbon_array *it);

/** Checks if this array is annotated as a sorted abstract type. Returns true if this is the case,
 * otherwise false. In case of any error, a failure is returned. */
fn_result ofType(bool) carbon_array_is_sorted(carbon_array *it);

/** Updates this arrays abstract type to the given abstract type */
fn_result carbon_array_update_type(carbon_array *it, carbon_list_derivable_e derivation);

// ---------------------------------------------------------------------------------------------------------------------
//  for internal usage
// ---------------------------------------------------------------------------------------------------------------------

/**
 * Constructs a new array iterator in a carbon document, where <code>payload_start</code> is a memory offset
 * that starts with the first (potentially empty) array entry. If there is some data before the array contents
 * (e.g., a header), <code>payload_start</code> must not include this data.
 */
fn_result internal_carbon_array_create(carbon_array *it, memfile *memfile, err *err, offset_t payload_start);
bool internal_carbon_array_copy(carbon_array *dst, carbon_array *src);
bool internal_carbon_array_clone(carbon_array *dst, carbon_array *src);
bool internal_carbon_array_set_mode(carbon_array *it, access_mode_e mode);

bool internal_carbon_array_remove(carbon_array *it);

#define DECLARE_IN_PLACE_UPDATE_FUNCTION(type_name)                                                                    \
bool internal_carbon_array_update_##type_name(carbon_array *it, type_name value);

DECLARE_IN_PLACE_UPDATE_FUNCTION(u8)
DECLARE_IN_PLACE_UPDATE_FUNCTION(u16)
DECLARE_IN_PLACE_UPDATE_FUNCTION(u32)
DECLARE_IN_PLACE_UPDATE_FUNCTION(u64)
DECLARE_IN_PLACE_UPDATE_FUNCTION(i8)
DECLARE_IN_PLACE_UPDATE_FUNCTION(i16)
DECLARE_IN_PLACE_UPDATE_FUNCTION(i32)
DECLARE_IN_PLACE_UPDATE_FUNCTION(i64)
DECLARE_IN_PLACE_UPDATE_FUNCTION(float)

bool internal_carbon_array_update_true(carbon_array *it);
bool internal_carbon_array_update_false(carbon_array *it);
bool internal_carbon_array_update_null(carbon_array *it);
bool internal_carbon_array_update_string(carbon_array *it, const char *str);
bool internal_carbon_array_update_binary(carbon_array *it, const void *value, size_t nbytes, const char *file_ext, const char *user_type);
carbon_insert *internal_carbon_array_update_array_begin(carbon_insert_array_state *state, carbon_array *it);
bool internal_carbon_array_update_array_end(carbon_insert_array_state *state);

carbon_insert *internal_carbon_array_update_column_begin(carbon_insert_column_state *state, carbon_array *it);
bool internal_carbon_array_update_column_end(carbon_insert_column_state *state);

carbon_insert *internal_carbon_array_update_object_begin(carbon_insert_object_state *state, carbon_array *it);
bool internal_carbon_array_update_object_end(carbon_insert_object_state *state);

bool internal_carbon_array_update_from_carbon(carbon_array *it, const rec *src);
bool internal_carbon_array_update_from_array(carbon_array *it, const carbon_array *src);
bool internal_carbon_array_update_from_object(carbon_array *it, const carbon_object *src);
bool internal_carbon_array_update_from_column(carbon_array *it, const carbon_column *src);


bool carbon_array_field_type(carbon_field_type_e *type, carbon_array *it);

offset_t internal_carbon_array_memfilepos(carbon_array *it);
offset_t internal_carbon_array_tell(carbon_array *it);
bool internal_carbon_array_offset(offset_t *off, carbon_array *it);
bool internal_carbon_array_fast_forward(carbon_array *it);

#ifdef __cplusplus
}
#endif

#endif
