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

BEGIN_DECL

// ---------------------------------------------------------------------------------------------------------------------
//  public structures
// ---------------------------------------------------------------------------------------------------------------------

typedef struct carbon_object {
        memfile memfile;
        err err;

        offset_t object_contents_off, object_start_off;
        bool object_end_reached;

        vector ofType(offset_t) history;

        struct {
                struct {
                        offset_t offset;
                        const char *name;
                        u64 name_len;
                } key;
                struct {
                        offset_t offset;
                        field_access data;
                } value;
        } field;

        carbon_map_derivable_e abstract_type;

        /** in case of modifications (updates, inserts, deletes), the number of bytes that are added resp. removed */
        i64 mod_size;
} carbon_object;

// ---------------------------------------------------------------------------------------------------------------------
//  public interface
// ---------------------------------------------------------------------------------------------------------------------

bool carbon_object_drop(carbon_object *it);

bool carbon_object_rewind(carbon_object *it);
bool carbon_object_next(carbon_object *it);
bool carbon_object_has_next(carbon_object *it);
bool carbon_object_prev(carbon_object *it);

fn_result ofType(bool) carbon_object_is_multimap(carbon_object *it);
fn_result ofType(bool) carbon_object_is_sorted(carbon_object *it);
fn_result carbon_object_update_type(carbon_object *it, carbon_map_derivable_e derivation);

// ---------------------------------------------------------------------------------------------------------------------
//  for internal usage
// ---------------------------------------------------------------------------------------------------------------------

bool internal_carbon_object_create(carbon_object *it, memfile *memfile, err *err, offset_t payload_start);
bool internal_carbon_object_copy(carbon_object *dst, carbon_object *src);
bool internal_carbon_object_clone(carbon_object *dst, carbon_object *src);

bool internal_carbon_object_fast_forward(carbon_object *it);

offset_t internal_carbon_object_memfile_pos(carbon_object *it);
bool internal_carbon_object_tell(offset_t *key_off, offset_t *value_off, carbon_object *it);

const char *internal_carbon_object_prop_name(u64 *key_len, carbon_object *it);
bool internal_carbon_object_remove(carbon_object *it);
bool internal_carbon_object_prop_type(carbon_field_type_e *type, carbon_object *it);

bool internal_carbon_object_insert_begin(carbon_insert *inserter, carbon_object *it);
fn_result internal_carbon_object_insert_end(carbon_insert *inserter);

bool internal_carbon_object_bool_value(bool *is_true, carbon_object *it);
bool internal_carbon_object_is_null(bool *is_null, carbon_object *it);

bool internal_carbon_object_u8_value(u8 *value, carbon_object *it);
bool internal_carbon_object_u16_value(u16 *value, carbon_object *it);
bool internal_carbon_object_u32_value(u32 *value, carbon_object *it);
bool internal_carbon_object_u64_value(u64 *value, carbon_object *it);
bool internal_carbon_object_i8_value(i8 *value, carbon_object *it);
bool internal_carbon_object_i16_value(i16 *value, carbon_object *it);
bool internal_carbon_object_i32_value(i32 *value, carbon_object *it);
bool internal_carbon_object_i64_value(i64 *value, carbon_object *it);
bool internal_carbon_object_float_value(float *value, carbon_object *it);
bool internal_carbon_object_float_value_nullable(bool *is_null_in, float *value, carbon_object *it);
bool internal_carbon_object_signed_value(bool *is_null_in, i64 *value, carbon_object *it);
bool carbon_object_unsigned_value(bool *is_null_in, u64 *value, carbon_object *it);
const char *internal_carbon_object_string_value(u64 *strlen, carbon_object *it);
bool internal_carbon_object_binary_value(carbon_binary *out, carbon_object *it);
carbon_array *internal_carbon_object_array_value(carbon_object *it_in);
carbon_object *internal_carbon_object_object_value(carbon_object *it_in);
carbon_column *internal_carbon_object_column_value(carbon_object *it_in);

END_DECL

#endif
