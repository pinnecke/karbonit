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
#include <jakson/carbon/arr_it.h>
#include <jakson/carbon/abstract.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct insert {
        container_e context_type;
        union {
                arr_it *array;
                col_it *column;
                obj_it *object;
        } context;

        memfile memfile;
        offset_t position;
} insert;

typedef struct insert_array_state {
        insert *parent_inserter;
        arr_it *array;
        insert nested_inserter;
        offset_t array_begin, array_end;
} insert_array_state;

typedef struct insert_object_state {
        insert *parent_inserter;
        obj_it *it;
        insert in;
        offset_t object_begin, object_end;
} insert_object_state;

typedef struct insert_column_state {
        insert *parent_inserter;
        field_e type;
        col_it *nested_column;
        insert nested_inserter;
        offset_t column_begin, column_end;
} insert_column_state;

bool internal_insert_object(memfile *memfile, map_type_e derivation, size_t nbytes);
bool internal_insert_array(memfile *memfile, list_type_e derivation, size_t nbytes);
bool internal_insert_column(memfile *memfile_in, list_type_e derivation, col_it_type_e type, size_t capactity);

/**
 * Returns the number of bytes required to store a field type including its type marker in a byte sequence.
 */
size_t internal_get_type_size_encoded(field_e type);

/**
 * Returns the number of bytes required to store a field value of a particular type exclusing its type marker.
 */
size_t internal_get_type_value_size(field_e type);

bool internal_array_next(bool *is_empty_slot, bool *is_array_end, arr_it *it);
bool internal_array_refresh(bool *is_empty_slot, bool *is_array_end, arr_it *it);
bool internal_array_field_read(arr_it *it);
bool internal_array_skip_contents(bool *is_empty_slot, bool *is_array_end, arr_it *it);

bool internal_object_it_next(bool *is_empty_slot, bool *is_object_end, obj_it *it);
bool internal_object_it_refresh(bool *is_empty_slot, bool *is_object_end, obj_it *it);
bool internal_object_it_prop_key_access(obj_it *it);
bool internal_object_it_prop_value_skip(obj_it *it);
bool internal_object_it_prop_skip(obj_it *it);
bool internal_object_skip_contents(bool *is_empty_slot, bool *is_array_end, obj_it *it);
bool internal_field_data_access(memfile *file, field *field);

offset_t internal_column_get_payload_off(col_it *it);
offset_t internal_payload_after_header(rec *doc);

u64 internal_header_get_commit_hash(rec *doc);

void internal_history_push(vec ofType(offset_t) *vec, offset_t off);
void internal_history_clear(vec ofType(offset_t) *vec);
offset_t internal_history_pop(vec ofType(offset_t) *vec);
offset_t internal_history_peek(vec ofType(offset_t) *vec);
bool internal_history_has(vec ofType(offset_t) *vec);

bool internal_field_create(field *field);
bool internal_field_clone(field *dst, field *src);
bool internal_field_drop(field *field);
bool internal_field_auto_close(field *it);
bool internal_field_object_it_opened(field *field);
bool internal_field_array_opened(field *field);
bool internal_field_column_it_opened(field *field);
bool internal_field_field_type(field_e *type, field *field);
bool internal_field_bool_value(bool *value, field *field);
bool internal_field_is_null(bool *is_null, field *field);
bool internal_field_u8_value(u8 *value, field *field);
bool internal_field_u16_value(u16 *value, field *field);
bool internal_field_u32_value(u32 *value, field *field);
bool internal_field_u64_value(u64 *value, field *field);
bool internal_field_i8_value(i8 *value, field *field);
bool internal_field_i16_value(i16 *value, field *field);
bool internal_field_i32_value(i32 *value, field *field);
bool internal_field_i64_value(i64 *value, field *field);
bool internal_field_float_value(float *value, field *field);
bool internal_field_float_value_nullable(bool *is_null_in, float *value, field *field);
bool internal_field_signed_value_nullable(bool *is_null_in, i64 *value, field *field);
bool internal_field_unsigned_value_nullable(bool *is_null_in, u64 *value, field *field);
bool internal_field_signed_value(i64 *value, field *field);
bool internal_field_unsigned_value(u64 *value, field *field);
const char *internal_field_string_value(u64 *strlen, field *field);
bool internal_field_binary_value(binary_field *out, field *field);
arr_it *internal_field_array_value(field *field);
obj_it *internal_field_object_value(field *field);
col_it *internal_field_column_value(field *field);

void internal_auto_close_nested_array(field *field);
void internal_auto_close_nested_object_it(field *field);
void internal_auto_close_nested_column_it(field *field);

bool internal_field_remove(memfile *memfile, field_e type);

/**
 * For <code>mode</code>, see <code>carbon_create_begin</code>
 */
void internal_from_json(rec *doc, const json *data, carbon_key_e key_type, const void *primary_key, int mode);

#ifdef __cplusplus
}
#endif

#endif
