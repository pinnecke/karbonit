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

#include <karbonit/stdinc.h>
#include <karbonit/mem/memfile.h>
#include <karbonit/std/uintvar/stream.h>
#include <karbonit/json/json-parser.h>
#include <karbonit/carbon/internal.h>
#include <karbonit/carbon/container.h>
#include <karbonit/carbon/field.h>
#include <karbonit/carbon/arr-it.h>
#include <karbonit/carbon/abstract.h>

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
#define INTERNAL_GET_TYPE_SIZE_ENCODED(field_e)                                                                        \
({                                                                                                                     \
        size_t type_size = sizeof(u8); /** at least the media type marker is required */                               \
        switch (field_e) {                                                                                             \
                case FIELD_NULL:                                                                                       \
                case FIELD_TRUE:                                                                                       \
                case FIELD_FALSE:                                                                                      \
                        /** only media type marker is required */                                                      \
                        break;                                                                                         \
                case FIELD_NUMBER_U8:                                                                                  \
                case FIELD_NUMBER_I8:                                                                                  \
                        type_size += sizeof(u8);                                                                       \
                        break;                                                                                         \
                case FIELD_NUMBER_U16:                                                                                 \
                case FIELD_NUMBER_I16:                                                                                 \
                        type_size += sizeof(u16);                                                                      \
                        break;                                                                                         \
                case FIELD_NUMBER_U32:                                                                                 \
                case FIELD_NUMBER_I32:                                                                                 \
                        type_size += sizeof(u32);                                                                      \
                        break;                                                                                         \
                case FIELD_NUMBER_U64:                                                                                 \
                case FIELD_NUMBER_I64:                                                                                 \
                        type_size += sizeof(u64);                                                                      \
                        break;                                                                                         \
                case FIELD_NUMBER_FLOAT:                                                                               \
                        type_size += sizeof(float);                                                                    \
                        break;                                                                                         \
                default: ERROR(ERR_INTERNALERR, NULL);                                                                 \
                        type_size = 0;                                                                                 \
                        break;                                                                                         \
        }                                                                                                              \
        type_size;                                                                                                     \
})

/**
 * Returns the number of bytes required to store a field value of a particular type excluding its type marker.
 */
#define INTERNAL_GET_TYPE_VALUE_SIZE(field_e)                                                                          \
({                                                                         \
        size_t internal_get_type_value_size_result = 0;                                                                         \
        switch (field_e) {                                                                         \
                case FIELD_NULL:                                                                         \
                case FIELD_TRUE:                                                                         \
                case FIELD_FALSE:                                                                         \
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:                                                                         \
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:                                                                         \
                        /** these constant values are determined by their media type markers */                                                                         \
                        internal_get_type_value_size_result = sizeof(u8);                                                                         \
                        break;                                                                         \
                case FIELD_NUMBER_U8:                                                                         \
                case FIELD_NUMBER_I8:                                                                         \
                case FIELD_COLUMN_U8_UNSORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:                                                                         \
                case FIELD_DERIVED_COLUMN_U8_SORTED_SET:                                                                         \
                case FIELD_COLUMN_I8_UNSORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:                                                                         \
                case FIELD_DERIVED_COLUMN_I8_SORTED_SET:                                                                         \
                        internal_get_type_value_size_result = sizeof(u8);                                                                         \
                        break;                                                                         \
                case FIELD_NUMBER_U16:                                                                         \
                case FIELD_NUMBER_I16:                                                                         \
                case FIELD_COLUMN_U16_UNSORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:                                                                         \
                case FIELD_DERIVED_COLUMN_U16_SORTED_SET:                                                                         \
                case FIELD_COLUMN_I16_UNSORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:                                                                         \
                case FIELD_DERIVED_COLUMN_I16_SORTED_SET:                                                                         \
                        internal_get_type_value_size_result = sizeof(u16);                                                                         \
                        break;                                                                         \
                case FIELD_NUMBER_U32:                                                                         \
                case FIELD_NUMBER_I32:                                                                         \
                case FIELD_COLUMN_U32_UNSORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:                                                                         \
                case FIELD_DERIVED_COLUMN_U32_SORTED_SET:                                                                         \
                case FIELD_COLUMN_I32_UNSORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:                                                                         \
                case FIELD_DERIVED_COLUMN_I32_SORTED_SET:                                                                         \
                        internal_get_type_value_size_result = sizeof(u32);                                                                         \
                        break;                                                                         \
                case FIELD_NUMBER_U64:                                                                         \
                case FIELD_NUMBER_I64:                                                                         \
                case FIELD_COLUMN_U64_UNSORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:                                                                         \
                case FIELD_DERIVED_COLUMN_U64_SORTED_SET:                                                                         \
                case FIELD_COLUMN_I64_UNSORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:                                                                         \
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET:                                                                         \
                        internal_get_type_value_size_result = sizeof(u64);                                                                         \
                        break;                                                                         \
                case FIELD_NUMBER_FLOAT:                                                                         \
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:                                                                         \
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:                                                                         \
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:                                                                         \
                        internal_get_type_value_size_result = sizeof(float);                                                                         \
                        break;                                                                         \
                default:                                                                         \
                        ERROR(ERR_INTERNALERR, NULL);                                                                         \
                        break;                                                                         \
        }                                                                         \
        internal_get_type_value_size_result;                                                                         \
})

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

#define INTERNAL_PAYLOAD_AFTER_HEADER(rec)                                                                             \
        ((rec)->data_off)


u64 internal_header_get_commit_hash(rec *doc);

void internal_history_push(vec ofType(offset_t) *vec, offset_t off);
void internal_history_clear(vec ofType(offset_t) *vec);
offset_t internal_history_pop(vec ofType(offset_t) *vec);
offset_t internal_history_peek(vec ofType(offset_t) *vec);
bool internal_history_has(vec ofType(offset_t) *vec);

bool internal_field_clone(field *dst, field *src);

#define INTERNAL_FIELD_AUTO_CLOSE(field)                                                                               \
{                                                                                                                      \
        if ((field)->arr_it.created && !(field)->arr_it.accessed) {                                                    \
                internal_auto_close_nested_array(field);                                                               \
                (field)->arr_it.created = false;                                                                       \
                (field)->arr_it.accessed = false;                                                                      \
        }                                                                                                              \
        if ((field)->obj_it.created && !(field)->obj_it.accessed) {                                                    \
                internal_auto_close_nested_object_it(field);                                                           \
                (field)->obj_it.created = false;                                                                       \
                (field)->obj_it.accessed = false;                                                                      \
        }                                                                                                              \
        if ((field)->col_it_created) {                                                                                 \
                internal_auto_close_nested_column_it((field));                                                         \
                (field)->obj_it.created = false;                                                                       \
        }                                                                                                              \
}

bool internal_field_field_type(field_e *type, field *field);
bool internal_field_bool_value(bool *value, field *field);
bool internal_field_is_null(bool *is_null, field *field);

#define INTERNAL_FIELD_U8_VALUE(field)                                                                             \
        (*(u8 *) field->data)

#define INTERNAL_FIELD_U16_VALUE(field)                                                                             \
        (*(u16 *) field->data)

#define INTERNAL_FIELD_U32_VALUE(field)                                                                             \
        (*(u32 *) field->data)

#define INTERNAL_FIELD_U64_VALUE(field)                                                                             \
        (*(u64 *) field->data)

#define INTERNAL_FIELD_I8_VALUE(field)                                                                             \
        (*(i8 *) field->data)

#define INTERNAL_FIELD_I16_VALUE(field)                                                                             \
        (*(i16 *) field->data)

#define INTERNAL_FIELD_I32_VALUE(field)                                                                             \
        (*(i32 *) field->data)

#define INTERNAL_FIELD_I64_VALUE(field)                                                                             \
        (*(i64 *) field->data)

#define INTERNAL_FIELD_FLOAT_VALUE(field)                                                                             \
        (*(float *) field->data)


#define INTERNAL_FIELD_SIGNED_VALUE(field)                                                                            \
({                                                                                                               \
        i64 ret = 0;                                                                            \
        switch (field->type) {                                                                            \
                case FIELD_NUMBER_I8:   ret = INTERNAL_FIELD_I8_VALUE(field);                                                                            \
                        break;                                                                            \
                case FIELD_NUMBER_I16:  ret = INTERNAL_FIELD_I16_VALUE(field);                                                                            \
                        break;                                                                            \
                case FIELD_NUMBER_I32:  ret = INTERNAL_FIELD_I32_VALUE(field);                                                                            \
                        break;                                                                            \
                case FIELD_NUMBER_I64:  ret = INTERNAL_FIELD_I64_VALUE(field);                                                                            \
                        break;                                                                            \
                default: ERROR(ERR_TYPEMISMATCH, NULL);                                                                            \
                        ret = 0;                                                                            \
                        break;                                                                            \
        }                                                                                                        \
        ret;                                                                                                     \
})

#define INTERNAL_FIELD_UNSIGNED_VALUE(field)                                                                            \
({                                                                            \
        u64 ret = 0;                                                                            \
        switch (field->type) {                                                                            \
                case FIELD_NUMBER_U8:  ret = INTERNAL_FIELD_U8_VALUE(field);                                                                            \
                        break;                                                                            \
                case FIELD_NUMBER_U16: ret = INTERNAL_FIELD_U16_VALUE(field);                                                                            \
                        break;                                                                            \
                case FIELD_NUMBER_U32: ret = INTERNAL_FIELD_U32_VALUE(field);                                                                            \
                        break;                                                                            \
                case FIELD_NUMBER_U64: ret = INTERNAL_FIELD_U64_VALUE(field);                                                                            \
                        break;                                                                            \
                default: ERROR(ERR_TYPEMISMATCH, NULL);                                                                            \
                        ret = 0;                                                                            \
                        break;                                                                            \
        }                                                                            \
        ret;                                                                            \
})

const char *internal_field_string_value(u64 *strlen, field *field);
bool internal_field_binary_value(binary_field *out, field *field);

bool internal_field_remove(memfile *memfile, field_e type);

/**
 * For <code>mode</code>, see <code>rec_create_begin</code>
 */
void internal_from_json(rec *doc, const json *data, key_e rec_key_type, const void *primary_key, int mode);

#ifdef __cplusplus
}
#endif

#endif
