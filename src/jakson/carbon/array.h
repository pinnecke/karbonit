/*
 * arr_it - array iterator implementation
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_ARR_IT_H
#define HAD_ARR_IT_H

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

typedef struct {
        bool created;
        bool accessed;
} nested_it;

typedef struct field {
        field_type_e type;
        const void *data;
        u64 len;
        const char *mime;
        u64 mime_len;
        nested_it arr_it;
        nested_it obj_it;
        bool col_it_created;
        carbon_array *array;
        carbon_column *column;
        carbon_object *object;
} field;

typedef struct carbon_array {
        memfile memfile;
        offset_t array_begin_off;

        list_derivable_e abstract_type;
        u64 pos;

        carbon_item item;

        /** in case of modifications (updates, inserts, deletes), the number of bytes that are added resp. removed */
        i64 mod_size;
        bool array_end_reached;

        vector ofType(offset_t) history;
        field field;
        offset_t field_offset;
} carbon_array;

// ---------------------------------------------------------------------------------------------------------------------
//  public interface
// ---------------------------------------------------------------------------------------------------------------------

/**
 * Drops the iterator.
 */
void carbon_array_drop(carbon_array *it);

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
void carbon_array_insert_begin(carbon_insert *inserter, carbon_array *it);
void carbon_array_insert_end(carbon_insert *inserter);

/** Checks if this array is annotated as a multi set abstract type. Returns true if it is is a multi set, and false if
 * it is a set. In case of any error, a failure is returned. */
bool carbon_array_is_multiset(carbon_array *it);

/** Checks if this array is annotated as a sorted abstract type. Returns true if this is the case,
 * otherwise false. In case of any error, a failure is returned. */
bool carbon_array_is_sorted(carbon_array *it);

/** Updates this arrays abstract type to the given abstract type */
void carbon_array_update_type(carbon_array *it, list_derivable_e derivation);

// ---------------------------------------------------------------------------------------------------------------------
//  for internal usage
// ---------------------------------------------------------------------------------------------------------------------

/**
 * Constructs a new array iterator in a carbon document, where <code>payload_start</code> is a memory offset
 * that starts with the first (potentially empty) array entry. If there is some data before the array contents
 * (e.g., a header), <code>payload_start</code> must not include this data.
 */
bool internal_carbon_array_create(carbon_array *it, memfile *memfile, offset_t payload_start);
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


bool carbon_array_field_type(field_type_e *type, carbon_array *it);

offset_t internal_carbon_array_memfilepos(carbon_array *it);
offset_t internal_carbon_array_tell(carbon_array *it);
bool internal_carbon_array_offset(offset_t *off, carbon_array *it);
bool internal_carbon_array_fast_forward(carbon_array *it);

#ifdef __cplusplus
}
#endif

#endif
