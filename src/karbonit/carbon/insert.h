/*
 * insert - container insert operations
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_INSERT_H
#define HAD_INSERT_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <karbonit/stdinc.h>
#include <karbonit/error.h>
#include <karbonit/mem/memblock.h>
#include <karbonit/mem/memfile.h>
#include <karbonit/std/spinlock.h>
#include <karbonit/rec.h>
#include <karbonit/carbon/internal.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------------------------------------------------
//  public interface
// ---------------------------------------------------------------------------------------------------------------------

bool insert_null(insert *in);
bool insert_true(insert *in);
bool insert_false(insert *in);
bool insert_u8(insert *in, u8 value);
bool insert_u16(insert *in, u16 value);
bool insert_u32(insert *in, u32 value);
bool insert_u64(insert *in, u64 value);
bool insert_i8(insert *in, i8 value);
bool insert_i16(insert *in, i16 value);
bool insert_i32(insert *in, i32 value);
bool insert_i64(insert *in, i64 value);
bool insert_unsigned(insert *in, u64 value);
bool insert_signed(insert *in, i64 value);
bool insert_float(insert *in, float value);
bool insert_string(insert *in, const char *value);
bool insert_nchar(insert *in, const char *value, u64 value_len);

bool insert_from_carbon(); // TODO: Implement P2
bool insert_from_array(); // TODO: Implement P2
bool insert_from_object(); // TODO: Implement P2
bool insert_from_column(); // TODO: Implement P2

/**
 * Inserts a user-defined binary string <code>value</code> of <code>nbytes</code> bytes along with a (mime) type annotation.
 * The type annotation is automatically found if <code>file_ext</code> is non-null and known to the system. If it is
 * not known or null, the non-empty <code>user_type</code> string is used to encode the mime annotation. In case
 * <code>user_type</code> is null (or empty) and <code>file_ext</code> is null (or not known), the mime type is set to
 * <code>application/octet-stream</code>, which encodes arbitrary binary data.
 */
bool insert_binary(insert *in, const void *value, size_t nbytes, const char *file_ext, const char *user_type);

insert *insert_object_begin(obj_state *out, insert *in, u64 object_capacity);
bool insert_object_end(obj_state *state);

insert *insert_object_map_begin(obj_state *out, insert *in,
                                              map_type_e derivation, u64 object_capacity);
bool insert_object_map_end(obj_state *state);

insert *insert_array_begin(arr_state *state_out, insert *inserter_in, u64 array_capacity);
bool insert_array_end(arr_state *state_in);

insert *insert_array_list_begin(arr_state *state_out, insert *inserter_in, list_type_e derivation, u64 array_capacity);
bool insert_array_list_end(arr_state *state_in);

insert *insert_column_begin(col_state *state_out, insert *inserter_in, col_it_type_e type, u64 cap);
bool insert_column_end(col_state *state_in);

insert *insert_column_list_begin(col_state *state_out, insert *inserter_in, list_type_e derivation, col_it_type_e type, u64 cap);
bool insert_column_list_end(col_state *state_in);

bool insert_prop_null(insert *in, const char *key);
bool insert_prop_true(insert *in, const char *key);
bool insert_prop_false(insert *in, const char *key);
bool insert_prop_u8(insert *in, const char *key, u8 value);
bool insert_prop_u16(insert *in, const char *key, u16 value);
bool insert_prop_u32(insert *in, const char *key, u32 value);
bool insert_prop_u64(insert *in, const char *key, u64 value);
bool insert_prop_i8(insert *in, const char *key, i8 value);
bool insert_prop_i16(insert *in, const char *key, i16 value);
bool insert_prop_i32(insert *in, const char *key, i32 value);
bool insert_prop_i64(insert *in, const char *key, i64 value);
bool insert_prop_unsigned(insert *in, const char *key, u64 value);
bool insert_prop_signed(insert *in, const char *key, i64 value);
bool insert_prop_float(insert *in, const char *key, float value);
bool insert_prop_string(insert *in, const char *key, const char *value);
bool insert_prop_nchar(insert *in, const char *key, const char *value, u64 value_len);
bool insert_prop_binary(insert *in, const char *key, const void *value, size_t nbytes, const char *file_ext, const char *user_type);

bool insert_prop_from_carbon(); // TODO: Implement P2
bool insert_prop_from_array(); // TODO: Implement P2
bool insert_prop_from_object(); // TODO: Implement P2
bool insert_prop_from_column(); // TODO: Implement P2

insert *insert_prop_object_begin(obj_state *out, insert *in, const char *key, u64 object_capacity);
u64 insert_prop_object_end(obj_state *state);

insert *insert_prop_map_begin(obj_state *out, insert *in, map_type_e derivation, const char *key, u64 object_capacity);
u64 insert_prop_map_end(obj_state *state);

insert *insert_prop_array_begin(arr_state *state, insert *in, const char *key, u64 array_capacity);
u64 insert_prop_array_end(arr_state *state);

insert *insert_prop_column_begin(col_state *state_out, insert *inserter_in, const char *key, col_it_type_e type, u64 cap);
u64 insert_prop_column_end(col_state *state_in);

// ---------------------------------------------------------------------------------------------------------------------
//  for internal usage only
// ---------------------------------------------------------------------------------------------------------------------

void internal_insert_create_for_array(insert *in, arr_it *context);
bool internal_insert_create_for_column(insert *in, col_it *context);
bool internal_insert_create_for_object(insert *in, obj_it *context);


#ifdef __cplusplus
}
#endif

#endif
