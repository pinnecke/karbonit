/*
 * shared internal usage - do not use for public purposes
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_INTERNAL_H
#define HAD_INTERNAL_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/mem/memfile.h>
#include <jakson/std/uintvar/stream.h>
#include <jakson/json/json-parser.h>
#include <jakson/carbon/internal.h>
#include <jakson/carbon/container.h>
#include <jakson/carbon/field.h>
#include <jakson/carbon/arr-it.h>
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
        memfile file;
        offset_t position;
} insert;

typedef struct arr_state {
        insert *parent;
        arr_it *array;
        insert nested;
        offset_t begin, end;
} arr_state;

typedef struct obj_state {
        insert *parent;
        obj_it *it;
        insert in;
        offset_t begin, end;
} obj_state;

typedef struct col_state {
        insert *parent;
        field_e type;
        col_it *nested_column;
        insert nested;
        offset_t begin, end;
} col_state;

bool internal_insert_object(memfile *file, map_type_e derivation, size_t nbytes);
bool internal_insert_array(memfile *file, list_type_e derivation, size_t nbytes);
bool internal_insert_column(memfile *file, list_type_e derivation, col_it_type_e type, size_t capactity);

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
void internal_from_json(rec *doc, const json *data, key_e key_type, const void *primary_key, int mode);

#ifdef __cplusplus
}
#endif

#endif
