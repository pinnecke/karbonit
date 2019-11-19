/**
 * Columnar Binary JSON -- Copyright 2019 Marcus Pinnecke
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

#ifndef CARBON_OBJECT_H
#define CARBON_OBJECT_H

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/mem/file.h>
#include <jakson/carbon/field.h>
#include <jakson/carbon/array.h>
#include <jakson/carbon/prop.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------------------------------------------------
//  public structures
// ---------------------------------------------------------------------------------------------------------------------

typedef struct carbon_object {
        memfile memfile;

        offset_t object_contents_off, object_start_off;
        bool object_end_reached;

        u64 pos;

        vec ofType(offset_t) history;

        struct {
                struct {
                        offset_t offset;
                        const char *name;
                        u64 name_len;
                } key;
                struct {
                        offset_t offset;
                        field data;
                } value;
        } field;

        map_derivable_e abstract_type;
        carbon_prop prop;

        /** in case of modifications (updates, inserts, deletes), the number of bytes that are added resp. removed */
        i64 mod_size;
} carbon_object;

// ---------------------------------------------------------------------------------------------------------------------
//  public interface
// ---------------------------------------------------------------------------------------------------------------------

bool carbon_object_drop(carbon_object *it);

bool carbon_object_rewind(carbon_object *it);
carbon_prop *carbon_object_next(carbon_object *it);
bool carbon_object_has_next(carbon_object *it);
bool carbon_object_prev(carbon_object *it);

bool carbon_object_is_multimap(carbon_object *it);
bool carbon_object_is_sorted(carbon_object *it);
void carbon_object_update_type(carbon_object *it, map_derivable_e derivation);

// ---------------------------------------------------------------------------------------------------------------------
//  for internal usage
// ---------------------------------------------------------------------------------------------------------------------

bool internal_carbon_object_create(carbon_object *it, memfile *memfile, offset_t payload_start);
bool internal_carbon_object_copy(carbon_object *dst, carbon_object *src);
bool internal_carbon_object_clone(carbon_object *dst, carbon_object *src);

bool internal_carbon_object_fast_forward(carbon_object *it);

offset_t internal_carbon_object_memfile_pos(carbon_object *it);
bool internal_carbon_object_tell(offset_t *key_off, offset_t *value_off, carbon_object *it);

carbon_string_field internal_carbon_object_prop_name(carbon_object *it);
bool internal_carbon_object_remove(carbon_object *it);
bool internal_carbon_object_prop_type(field_type_e *type, carbon_object *it);

bool internal_carbon_object_insert_begin(carbon_insert *inserter, carbon_object *it);
void internal_carbon_object_insert_end(carbon_insert *inserter);

bool internal_carbon_object_update_name(carbon_object *it, const char *key);
bool internal_carbon_object_update_u8(carbon_object *it, u8 value);
bool internal_carbon_object_update_u16(carbon_object *it, u16 value);
bool internal_carbon_object_update_u32(carbon_object *it, u32 value);
bool internal_carbon_object_update_u64(carbon_object *it, u64 value);
bool internal_carbon_object_update_i8(carbon_object *it, i8 value);
bool internal_carbon_object_update_i16(carbon_object *it, i16 value);
bool internal_carbon_object_update_i32(carbon_object *it, i32 value);
bool internal_carbon_object_update_i64(carbon_object *it, i64 value);
bool internal_carbon_object_update_float(carbon_object *it, float value);
bool internal_carbon_object_update_true(carbon_object *it);
bool internal_carbon_object_update_false(carbon_object *it);
bool internal_carbon_object_update_null(carbon_object *it);
bool internal_carbon_object_update_string(carbon_object *it, const char *str);
bool internal_carbon_object_update_binary(carbon_object *it, const void *value, size_t nbytes, const char *file_ext, const char *user_type);
carbon_insert *internal_carbon_object_update_array_begin(carbon_insert_array_state *state, carbon_object *it);
bool internal_carbon_object_update_array_end(carbon_insert_array_state *state);

carbon_insert *internal_carbon_object_update_column_begin(carbon_insert_column_state *state, carbon_object *it);
bool internal_carbon_object_update_column_end(carbon_insert_column_state *state);

carbon_insert *internal_carbon_object_update_object_begin(carbon_insert_object_state *state, carbon_object *it);
bool internal_carbon_object_update_object_end(carbon_insert_object_state *state);

bool internal_carbon_object_update_from_carbon(carbon_object *it, const rec *src);
bool internal_carbon_object_update_from_array(carbon_object *it, const carbon_object *src);
bool internal_carbon_object_update_from_object(carbon_object *it, const carbon_object *src);
bool internal_carbon_object_update_from_column(carbon_object *it, const carbon_column *src);

#ifdef __cplusplus
}
#endif

#endif
