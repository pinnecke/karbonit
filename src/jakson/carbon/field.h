/*
 * Copyright 2019 Marcus Pinnecke
 */

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/rec.h>
#include <jakson/carbon/abstract.h>

#ifndef HAD_FIELD_H
#define HAD_FIELD_H

typedef enum field_type {
        /** constants */
        CARBON_FIELD_NULL = MNULL, /** null */
        CARBON_FIELD_TRUE = MTRUE, /** true */
        CARBON_FIELD_FALSE = MFALSE, /** false */

        /** containers / abstract base types */
        CARBON_FIELD_OBJECT_UNSORTED_MULTIMAP = UNSORTED_MULTIMAP, /** object */
        CARBON_FIELD_ARRAY_UNSORTED_MULTISET = UNSORTED_MULTISET_ARRAY, /** variable-type array of elements of varying type */
        CARBON_FIELD_COLUMN_U8_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_U8, /** fixed-type array of elements of particular type */
        CARBON_FIELD_COLUMN_U16_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_U16, /** fixed-type array of elements of particular type */
        CARBON_FIELD_COLUMN_U32_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_U32, /** fixed-type array of elements of particular type */
        CARBON_FIELD_COLUMN_U64_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_U64, /** fixed-type array of elements of particular type */
        CARBON_FIELD_COLUMN_I8_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_I8, /** fixed-type array of elements of particular type */
        CARBON_FIELD_COLUMN_I16_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_I16, /** fixed-type array of elements of particular type */
        CARBON_FIELD_COLUMN_I32_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_I32, /** fixed-type array of elements of particular type */
        CARBON_FIELD_COLUMN_I64_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_I64, /** fixed-type array of elements of particular type */
        CARBON_FIELD_COLUMN_FLOAT_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_FLOAT, /** fixed-type array of elements of particular type */
        CARBON_FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_BOOLEAN, /** fixed-type array of elements of particular type */

        /** abstract derived types for object containers */
        CARBON_FIELD_DERIVED_OBJECT_SORTED_MULTIMAP = SORTED_MULTIMAP,
        CARBON_FIELD_DERIVED_OBJECT_UNSORTED_MAP = UNSORTED_MAP,
        CARBON_FIELD_DERIVED_OBJECT_SORTED_MAP = SORTED_MAP,

        /** abstract derived types for array containers */
        CARBON_FIELD_DERIVED_ARRAY_SORTED_MULTISET = SORTED_MULTISET_ARRAY,
        CARBON_FIELD_DERIVED_ARRAY_UNSORTED_SET = UNSORTED_SET_ARRAY,
        CARBON_FIELD_DERIVED_ARRAY_SORTED_SET = SORTED_SET_ARRAY,

        /** abstract derived types for column-u8 containers */
        CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET = SORTED_MULTISET_COL_U8,
        CARBON_FIELD_DERIVED_COLUMN_U8_UNSORTED_SET = UNSORTED_SET_COL_U8,
        CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_SET = SORTED_SET_COL_U8,

        /** abstract derived types for column-u16 containers */
        CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET = SORTED_MULTISET_COL_U16,
        CARBON_FIELD_DERIVED_COLUMN_U16_UNSORTED_SET = UNSORTED_SET_COL_U16,
        CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_SET = SORTED_SET_COL_U16,

        /** abstract derived types for column-u32 containers */
        CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET = SORTED_MULTISET_COL_U32,
        CARBON_FIELD_DERIVED_COLUMN_U32_UNSORTED_SET = UNSORTED_SET_COL_U32,
        CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_SET = SORTED_SET_COL_U32,

        /** abstract derived types for column-u64 containers */
        CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET = SORTED_MULTISET_COL_U64,
        CARBON_FIELD_DERIVED_COLUMN_U64_UNSORTED_SET = UNSORTED_SET_COL_U64,
        CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_SET = SORTED_SET_COL_U64,

        /** abstract derived types for column-i8 containers */
        CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET = SORTED_MULTISET_COL_I8,
        CARBON_FIELD_DERIVED_COLUMN_I8_UNSORTED_SET = UNSORTED_SET_COL_I8,
        CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_SET = SORTED_SET_COL_I8,

        /** abstract derived types for column-i16 containers */
        CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET = SORTED_MULTISET_COL_I16,
        CARBON_FIELD_DERIVED_COLUMN_I16_UNSORTED_SET = UNSORTED_SET_COL_I16,
        CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_SET = SORTED_SET_COL_I16,

        /** abstract derived types for column-i32 containers */
        CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET = SORTED_MULTISET_COL_I32,
        CARBON_FIELD_DERIVED_COLUMN_I32_UNSORTED_SET = UNSORTED_SET_COL_I32,
        CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_SET = SORTED_SET_COL_I32,

        /** abstract derived types for column-i64 containers */
        CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET = SORTED_MULTISET_COL_I64,
        CARBON_FIELD_DERIVED_COLUMN_I64_UNSORTED_SET = UNSORTED_SET_COL_I64,
        CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_SET = SORTED_SET_COL_I64,

        /** abstract derived types for column-float containers */
        CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET = SORTED_MULTISET_COL_FLOAT,
        CARBON_FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET = UNSORTED_SET_COL_FLOAT,
        CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET = SORTED_SET_COL_FLOAT,

        /** abstract derived types for column-boolean containers */
        CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET = SORTED_MULTISET_COL_BOOLEAN,
        CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET = UNSORTED_SET_COL_BOOLEAN,
        CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET = SORTED_SET_COL_BOOLEAN,

        /** character strings */
        CARBON_FIELD_STRING = MSTRING, /** UTF-8 string_buffer */

        /** numbers */
        CARBON_FIELD_NUMBER_U8 = MU8, /** 8bit unsigned integer */
        CARBON_FIELD_NUMBER_U16 = MU16, /** 16bit unsigned integer */
        CARBON_FIELD_NUMBER_U32 = MU32, /** 32bit unsigned integer */
        CARBON_FIELD_NUMBER_U64 = MU64, /** 64bit unsigned integer */
        CARBON_FIELD_NUMBER_I8 = MI8, /** 8bit signed integer */
        CARBON_FIELD_NUMBER_I16 = MI16, /** 16bit signed integer */
        CARBON_FIELD_NUMBER_I32 = MI32, /** 32bit signed integer */
        CARBON_FIELD_NUMBER_I64 = MI64, /** 64bit signed integer */
        CARBON_FIELD_NUMBER_FLOAT = MFLOAT, /** 32bit float */

        /** binary data */
        CARBON_FIELD_BINARY = MBINARY, /** arbitrary binary object with known mime type */
        CARBON_FIELD_BINARY_CUSTOM = MCUSTOM_BINARY, /** arbitrary binary object with unknown mime type*/
} field_type_e;

typedef enum col_it_type {
        COLUMN_TYPE_U8,
        COLUMN_TYPE_U16,
        COLUMN_TYPE_U32,
        COLUMN_TYPE_U64,
        COLUMN_TYPE_I8,
        COLUMN_TYPE_I16,
        COLUMN_TYPE_I32,
        COLUMN_TYPE_I64,
        COLUMN_TYPE_FLOAT,
        COLUMN_TYPE_BOOLEAN
} col_it_type_e;

typedef enum carbon_field_class {
        CARBON_FIELD_CLASS_CONSTANT,
        CARBON_FIELD_CLASS_NUMBER,
        CARBON_FIELD_CLASS_CHARACTER_STRING,
        CARBON_FIELD_CLASS_BINARY_STRING,
        CARBON_FIELD_CLASS_CONTAINER
} carbon_field_class_e;

typedef enum carbon_constant {
        CARBON_CONSTANT_TRUE,
        CARBON_CONSTANT_FALSE,
        CARBON_CONSTANT_NULL
} carbon_constant_e;

#define CARBON_FIELD_TYPE_NULL_STR "null"
#define CARBON_FIELD_TYPE_TRUE_STR "boolean-true"
#define CARBON_FIELD_TYPE_FALSE_STR "boolean-false"
#define CARBON_FIELD_TYPE_OBJECT_UNSORTED_MULTIMAP_STR "unsorted-multimap"
#define CARBON_FIELD_TYPE_OBJECT_SORTED_MULTIMAP_STR "sorted-multimap"
#define CARBON_FIELD_TYPE_OBJECT_UNSORTED_MAP_STR "unsorted-map"
#define CARBON_FIELD_TYPE_OBJECT_SORTED_MAP_STR "sorted-map"
#define CARBON_FIELD_TYPE_ARRAY_UNSORTED_MULTISET_STR "array-unsorted-multiset"
#define CARBON_FIELD_TYPE_ARRAY_SORTED_MULTISET_STR "array-sorted-multiset"
#define CARBON_FIELD_TYPE_ARRAY_UNSORTED_SET_STR "array-unsorted-set"
#define CARBON_FIELD_TYPE_ARRAY_SORTED_SET_STR "array-sorted-set"
#define CARBON_FIELD_TYPE_COLUMN_U8_UNSORTED_MULTISET_STR "column-u8-unsorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_U8_SORTED_MULTISET_STR "column-u8-sorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_U8_UNSORTED_SET_STR "column-u8-unsorted-set"
#define CARBON_FIELD_TYPE_COLUMN_U8_SORTED_SET_STR "column-u8-sorted-set"
#define CARBON_FIELD_TYPE_COLUMN_U16_UNSORTED_MULTISET_STR "column-u16-unsorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_U16_SORTED_MULTISET_STR "column-u16-sorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_U16_UNSORTED_SET_STR "column-u16-unsorted-set"
#define CARBON_FIELD_TYPE_COLUMN_U16_SORTED_SET_STR "column-u16-sorted-set"
#define CARBON_FIELD_TYPE_COLUMN_U32_UNSORTED_MULTISET_STR "column-u32-unsorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_U32_SORTED_MULTISET_STR "column-u32-sorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_U32_UNSORTED_SET_STR "column-u32-unsorted-set"
#define CARBON_FIELD_TYPE_COLUMN_U32_SORTED_SET_STR "column-u32-sorted-set"
#define CARBON_FIELD_TYPE_COLUMN_U64_UNSORTED_MULTISET_STR "column-u64-unsorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_U64_SORTED_MULTISET_STR "column-u64-sorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_U64_UNSORTED_SET_STR "column-u64-unsorted-set"
#define CARBON_FIELD_TYPE_COLUMN_U64_SORTED_SET_STR "column-u64-sorted-set"
#define CARBON_FIELD_TYPE_COLUMN_I8_UNSORTED_MULTISET_STR "column-i8-unsorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_I8_SORTED_MULTISET_STR "column-i8-sorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_I8_UNSORTED_SET_STR "column-i8-unsorted-set"
#define CARBON_FIELD_TYPE_COLUMN_I8_SORTED_SET_STR "column-i8-sorted-set"
#define CARBON_FIELD_TYPE_COLUMN_I16_UNSORTED_MULTISET_STR "column-i16-unsorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_I16_SORTED_MULTISET_STR "column-i16-sorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_I16_UNSORTED_SET_STR "column-i16-unsorted-set"
#define CARBON_FIELD_TYPE_COLUMN_I16_SORTED_SET_STR "column-i16-sorted-set"
#define CARBON_FIELD_TYPE_COLUMN_I32_UNSORTED_MULTISET_STR "column-i32-unsorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_I32_SORTED_MULTISET_STR "column-i32-sorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_I32_UNSORTED_SET_STR "column-i32-unsorted-set"
#define CARBON_FIELD_TYPE_COLUMN_I32_SORTED_SET_STR "column-i32-sorted-set"
#define CARBON_FIELD_TYPE_COLUMN_I64_UNSORTED_MULTISET_STR "column-i64-unsorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_I64_SORTED_MULTISET_STR "column-i64-sorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_I64_UNSORTED_SET_STR "column-i64-unsorted-set"
#define CARBON_FIELD_TYPE_COLUMN_I64_SORTED_SET_STR "column-i64-sorted-set"
#define CARBON_FIELD_TYPE_COLUMN_FLOAT_UNSORTED_MULTISET_STR "column-float-unsorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_FLOAT_SORTED_MULTISET_STR "column-float-sorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_FLOAT_UNSORTED_SET_STR "column-float-unsorted-set"
#define CARBON_FIELD_TYPE_COLUMN_FLOAT_SORTED_SET_STR "column-float-sorted-set"
#define CARBON_FIELD_TYPE_COLUMN_BOOLEAN_UNSORTED_MULTISET_STR "column-boolean-unsorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_BOOLEAN_SORTED_MULTISET_STR "column-boolean-sorted-multiset"
#define CARBON_FIELD_TYPE_COLUMN_BOOLEAN_UNSORTED_SET_STR "column-boolean-unsorted-set"
#define CARBON_FIELD_TYPE_COLUMN_BOOLEAN_SORTED_SET_STR "column-boolean-sorted-set"
#define CARBON_FIELD_TYPE_STRING_STR "string"
#define CARBON_FIELD_TYPE_BINARY_STR "binary"
#define CARBON_FIELD_TYPE_NUMBER_U8_STR "number-u8"
#define CARBON_FIELD_TYPE_NUMBER_U16_STR "number-u16"
#define CARBON_FIELD_TYPE_NUMBER_U32_STR "number-u32"
#define CARBON_FIELD_TYPE_NUMBER_U64_STR "number-u64"
#define CARBON_FIELD_TYPE_NUMBER_I8_STR "number-i8"
#define CARBON_FIELD_TYPE_NUMBER_I16_STR "number-i16"
#define CARBON_FIELD_TYPE_NUMBER_I32_STR "number-i32"
#define CARBON_FIELD_TYPE_NUMBER_I64_STR "number-i64"
#define CARBON_FIELD_TYPE_NUMBER_FLOAT_STR "number-float"

#ifdef __cplusplus
extern "C" {
#endif

const char *field_type_str(field_type_e type);

bool field_type_is_traversable(field_type_e type);
bool field_type_is_signed(field_type_e type);
bool field_type_is_unsigned(field_type_e type);
bool field_type_is_floating(field_type_e type);
bool field_type_is_number(field_type_e type);
bool field_type_is_integer(field_type_e type);
bool field_type_is_binary(field_type_e type);
bool field_type_is_boolean(field_type_e type);
bool field_type_is_list_or_subtype(field_type_e type);
bool field_type_is_array_or_subtype(field_type_e type);
bool field_type_is_column_u8_or_subtype(field_type_e type);
bool field_type_is_column_u16_or_subtype(field_type_e type);
bool field_type_is_column_u32_or_subtype(field_type_e type);
bool field_type_is_column_u64_or_subtype(field_type_e type);
bool field_type_is_column_i8_or_subtype(field_type_e type);
bool field_type_is_column_i16_or_subtype(field_type_e type);
bool field_type_is_column_i32_or_subtype(field_type_e type);
bool field_type_is_column_i64_or_subtype(field_type_e type);
bool field_type_is_column_float_or_subtype(field_type_e type);
bool field_type_is_column_bool_or_subtype(field_type_e type);
bool field_type_is_column_or_subtype(field_type_e type);
bool field_type_is_object_or_subtype(field_type_e type);
bool field_type_is_null(field_type_e type);
bool field_type_is_string(field_type_e type);
bool field_type_is_constant(field_type_e type);

carbon_field_class_e field_type_get_class(field_type_e type);

bool carbon_field_skip(memfile *file);

bool carbon_field_skip_object(memfile *file);
bool carbon_field_skip_array(memfile *file);

bool carbon_field_skip_column(memfile *file);
bool carbon_field_skip_binary(memfile *file);
bool carbon_field_skip_custom_binary(memfile *file);
bool carbon_field_skip_string(memfile *file);
bool carbon_field_skip_float(memfile *file);
bool carbon_field_skip_boolean(memfile *file);
bool carbon_field_skip_null(memfile *file);
bool carbon_field_skip_8(memfile *file);
bool carbon_field_skip_16(memfile *file);
bool carbon_field_skip_32(memfile *file);
bool carbon_field_skip_64(memfile *file);

field_type_e field_type_for_column(list_type_e derivation, col_it_type_e type);
field_type_e field_type_column_entry_to_regular_type(field_type_e type, bool is_null, bool is_true);

#ifdef __cplusplus
}
#endif

#endif
