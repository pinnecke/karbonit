/**
 * Columnar Binary JSON -- Copyright 2019 Marcus Pinnecke
 * This file is for internal usage only; do not call these functions from outside
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

#ifndef CARBON_INT_H
#define CARBON_INT_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/mem/file.h>
#include <jakson/std/uintvar/stream.h>
#include <jakson/json/parser.h>
#include <jakson/carbon/internal.h>
#include <jakson/carbon/containers.h>
#include <jakson/carbon/field.h>
#include <jakson/carbon/array.h>
#include <jakson/carbon/abstract.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct carbon_insert {
        container_e context_type;
        union {
                carbon_array *array;
                carbon_column *column;
                carbon_object *object;
        } context;

        memfile memfile;
        offset_t position;
} carbon_insert;

typedef struct carbon_insert_array_state {
        carbon_insert *parent_inserter;
        carbon_array *nested_array;
        carbon_insert nested_inserter;
        offset_t array_begin, array_end;
} carbon_insert_array_state;

typedef struct carbon_insert_object_state {
        carbon_insert *parent_inserter;
        carbon_object *it;
        carbon_insert inserter;
        offset_t object_begin, object_end;
} carbon_insert_object_state;

typedef struct carbon_insert_column_state {
        carbon_insert *parent_inserter;
        field_type_e type;
        carbon_column *nested_column;
        carbon_insert nested_inserter;
        offset_t column_begin, column_end;
} carbon_insert_column_state;

bool carbon_int_insert_object(memfile *memfile, map_derivable_e derivation, size_t nbytes);
bool carbon_int_insert_array(memfile *memfile, list_derivable_e derivation, size_t nbytes);
bool carbon_int_insert_column(memfile *memfile_in, list_derivable_e derivation, carbon_column_type_e type, size_t capactity);

/**
 * Returns the number of bytes required to store a field type including its type marker in a byte sequence.
 */
size_t carbon_int_get_type_size_encoded(field_type_e type);

/**
 * Returns the number of bytes required to store a field value of a particular type exclusing its type marker.
 */
size_t carbon_int_get_type_value_size(field_type_e type);

bool carbon_int_array_next(bool *is_empty_slot, bool *is_array_end, carbon_array *it);
bool carbon_int_array_refresh(bool *is_empty_slot, bool *is_array_end, carbon_array *it);
bool carbon_int_array_field_type_read(carbon_array *it);
bool carbon_int_array_skip_contents(bool *is_empty_slot, bool *is_array_end, carbon_array *it);

bool carbon_int_object_it_next(bool *is_empty_slot, bool *is_object_end, carbon_object *it);
bool carbon_int_object_it_refresh(bool *is_empty_slot, bool *is_object_end, carbon_object *it);
bool carbon_int_object_it_prop_key_access(carbon_object *it);
bool carbon_int_object_it_prop_value_skip(carbon_object *it);
bool carbon_int_object_it_prop_skip(carbon_object *it);
bool carbon_int_object_skip_contents(bool *is_empty_slot, bool *is_array_end, carbon_object *it);
bool carbon_int_field_data_access(memfile *file, field_access *field_access);

offset_t carbon_int_column_get_payload_off(carbon_column *it);
offset_t carbon_int_payload_after_header(rec *doc);

u64 carbon_int_header_get_commit_hash(rec *doc);

void carbon_int_history_push(vector ofType(offset_t) *vec, offset_t off);
void carbon_int_history_clear(vector ofType(offset_t) *vec);
offset_t carbon_int_history_pop(vector ofType(offset_t) *vec);
offset_t carbon_int_history_peek(vector ofType(offset_t) *vec);
bool carbon_int_history_has(vector ofType(offset_t) *vec);

bool carbon_int_field_access_create(field_access *field);
bool carbon_int_field_access_clone(field_access *dst, field_access *src);
bool carbon_int_field_access_drop(field_access *field);
bool carbon_int_field_auto_close(field_access *it);
bool carbon_int_field_access_object_it_opened(field_access *field);
bool carbon_int_field_access_array_opened(field_access *field);
bool carbon_int_field_access_column_it_opened(field_access *field);
bool carbon_int_field_access_field_type(field_type_e *type, field_access *field);
bool carbon_int_field_access_bool_value(bool *value, field_access *field);
bool carbon_int_field_access_is_null(bool *is_null, field_access *field);
bool carbon_int_field_access_u8_value(u8 *value, field_access *field);
bool carbon_int_field_access_u16_value(u16 *value, field_access *field);
bool carbon_int_field_access_u32_value(u32 *value, field_access *field);
bool carbon_int_field_access_u64_value(u64 *value, field_access *field);
bool carbon_int_field_access_i8_value(i8 *value, field_access *field);
bool carbon_int_field_access_i16_value(i16 *value, field_access *field);
bool carbon_int_field_access_i32_value(i32 *value, field_access *field);
bool carbon_int_field_access_i64_value(i64 *value, field_access *field);
bool carbon_int_field_access_float_value(float *value, field_access *field);
bool carbon_int_field_access_float_value_nullable(bool *is_null_in, float *value, field_access *field);
bool carbon_int_field_access_signed_value_nullable(bool *is_null_in, i64 *value, field_access *field);
bool carbon_int_field_access_unsigned_value_nullable(bool *is_null_in, u64 *value, field_access *field);
bool carbon_int_field_access_signed_value(i64 *value, field_access *field);
bool carbon_int_field_access_unsigned_value(u64 *value, field_access *field);
const char *carbon_int_field_access_string_value(u64 *strlen, field_access *field);
bool carbon_int_field_access_binary_value(carbon_binary *out, field_access *field);
carbon_array *carbon_int_field_access_array_value(field_access *field);
carbon_object *carbon_int_field_access_object_value(field_access *field);
carbon_column *carbon_int_field_access_column_value(field_access *field);

void carbon_int_auto_close_nested_array(field_access *field);
void carbon_int_auto_close_nested_object_it(field_access *field);
void carbon_int_auto_close_nested_column_it(field_access *field);

bool carbon_int_field_remove(memfile *memfile, field_type_e type);

/**
 * For <code>mode</code>, see <code>carbon_create_begin</code>
 */
void carbon_int_from_json(rec *doc, const json *data, carbon_key_e key_type, const void *primary_key, int mode);

#ifdef __cplusplus
}
#endif

#endif
