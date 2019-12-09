/*
 * arr-it - array iterator implementation
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
#include <jakson/std/vec.h>
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
        field_e type;
        const void *data;
        u64 len;
        const char *mime;
        u64 mime_len;
        nested_it arr_it;
        nested_it obj_it;
        bool col_it_created;
        struct arr_it *array;
        struct col_it *column;
        struct obj_it *object;
} field;

typedef struct arr_it {
        memfile file;
        offset_t begin;
        list_type_e list_type;
        u64 pos;
        item item;
        /** in case of modifications (updates, inserts, deletes), the number of bytes that are added resp. removed */
        i64 mod_size;
        bool eof;
        offset_t last_off;
        field field;
        offset_t field_offset;
} arr_it;

// ---------------------------------------------------------------------------------------------------------------------
//  public interface
// ---------------------------------------------------------------------------------------------------------------------

/**
 * Drops the iterator.
 */
void arr_it_drop(arr_it *it);

/**
 * Positions the iterator at the beginning of this array.
 */
bool arr_it_rewind(arr_it *it);
bool arr_it_length(u64 *len, arr_it *it);
bool arr_it_is_empty(arr_it *it);

/**
 * Positions the iterator to the slot after the current element, potentially pointing to next element.
 * The function returns true, if the slot is non-empty, and false otherwise.
 */
item *arr_it_next(arr_it *it);
bool arr_it_has_next(arr_it *it);
bool arr_it_is_unit(arr_it *it);

/**
 * Inserts a new element at the current position of the iterator.
 */
void arr_it_insert_begin(insert *in, arr_it *it);
void arr_it_insert_end(insert *in);

/** Checks if this array is annotated as a multi set abstract type. Returns true if it is is a multi set, and false if
 * it is a set. In case of any error, a failure is returned. */
bool arr_it_is_multiset(arr_it *it);

/** Checks if this array is annotated as a sorted abstract type. Returns true if this is the case,
 * otherwise false. In case of any error, a failure is returned. */
bool arr_it_is_sorted(arr_it *it);

/** Updates this arrays abstract type to the given abstract type */
void arr_it_update_type(arr_it *it, list_type_e derivation);

// ---------------------------------------------------------------------------------------------------------------------
//  for internal usage
// ---------------------------------------------------------------------------------------------------------------------

/**
 * Constructs a new array iterator in a carbon document, where <code>payload_start</code> is a memory offset
 * that starts with the first (potentially empty) array entry. If there is some data before the array contents
 * (e.g., a header), <code>payload_start</code> must not include this data.
 */
bool internal_arr_it_create(arr_it *it, memfile *memfile, offset_t payload_start);
bool internal_arr_it_copy(arr_it *dst, arr_it *src);
bool internal_arr_it_clone(arr_it *dst, arr_it *src);
bool internal_arr_it_set_mode(arr_it *it, access_mode_e mode);

bool internal_arr_it_remove(arr_it *it);

#define DECLARE_IN_PLACE_UPDATE_FUNCTION(type_name)                                                                    \
bool internal_arr_it_update_##type_name(arr_it *it, type_name value);

DECLARE_IN_PLACE_UPDATE_FUNCTION(u8)
DECLARE_IN_PLACE_UPDATE_FUNCTION(u16)
DECLARE_IN_PLACE_UPDATE_FUNCTION(u32)
DECLARE_IN_PLACE_UPDATE_FUNCTION(u64)
DECLARE_IN_PLACE_UPDATE_FUNCTION(i8)
DECLARE_IN_PLACE_UPDATE_FUNCTION(i16)
DECLARE_IN_PLACE_UPDATE_FUNCTION(i32)
DECLARE_IN_PLACE_UPDATE_FUNCTION(i64)
DECLARE_IN_PLACE_UPDATE_FUNCTION(float)

bool internal_arr_it_update_true(arr_it *it);
bool internal_arr_it_update_false(arr_it *it);
bool internal_arr_it_update_null(arr_it *it);
bool internal_arr_it_update_string(arr_it *it, const char *str);
bool internal_arr_it_update_binary(arr_it *it, const void *base, size_t nbytes, const char *file_ext, const char *type);
insert *internal_arr_it_update_array_begin(arr_state *state, arr_it *it);
bool internal_arr_it_update_array_end(arr_state *state);

insert *internal_arr_it_update_column_begin(col_state *state, arr_it *it);
bool internal_arr_it_update_column_end(col_state *state);

insert *internal_arr_it_update_object_begin(obj_state *state, arr_it *it);
bool internal_arr_it_update_object_end(obj_state *state);

bool internal_arr_it_update_from_carbon(arr_it *it, const rec *src);
bool internal_arr_it_update_from_array(arr_it *it, const arr_it *src);
bool internal_arr_it_update_from_object(arr_it *it, const obj_it *src);
bool internal_arr_it_update_from_column(arr_it *it, const col_it *src);


bool arr_it_field_type(field_e *type, arr_it *it);

offset_t internal_arr_it_memfilepos(arr_it *it);
offset_t internal_arr_it_tell(arr_it *it);
bool internal_arr_it_offset(offset_t *off, arr_it *it);
bool internal_arr_it_fast_forward(arr_it *it);

#ifdef __cplusplus
}
#endif

#endif
