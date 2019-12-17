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
#include <jakson/mem/memblock.h>
#include <jakson/std/uintvar/stream.h>

#ifndef HAD_FIELD_H
#define HAD_FIELD_H

typedef enum field_type {
        /** constants */
        FIELD_NULL = MNULL, /** null */
        FIELD_TRUE = MTRUE, /** true */
        FIELD_FALSE = MFALSE, /** false */

        /** containers / abstract base types */
        FIELD_OBJECT_UNSORTED_MULTIMAP = UNSORTED_MULTIMAP, /** object */
        FIELD_ARRAY_UNSORTED_MULTISET = UNSORTED_MULTISET_ARRAY, /** variable-type array of elements of varying type */
        FIELD_COLUMN_U8_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_U8, /** fixed-type array of elements of particular type */
        FIELD_COLUMN_U16_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_U16, /** fixed-type array of elements of particular type */
        FIELD_COLUMN_U32_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_U32, /** fixed-type array of elements of particular type */
        FIELD_COLUMN_U64_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_U64, /** fixed-type array of elements of particular type */
        FIELD_COLUMN_I8_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_I8, /** fixed-type array of elements of particular type */
        FIELD_COLUMN_I16_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_I16, /** fixed-type array of elements of particular type */
        FIELD_COLUMN_I32_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_I32, /** fixed-type array of elements of particular type */
        FIELD_COLUMN_I64_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_I64, /** fixed-type array of elements of particular type */
        FIELD_COLUMN_FLOAT_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_FLOAT, /** fixed-type array of elements of particular type */
        FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET = UNSORTED_MULTISET_COL_BOOLEAN, /** fixed-type array of elements of particular type */

        /** abstract derived types for object containers */
        FIELD_DERIVED_OBJECT_SORTED_MULTIMAP = SORTED_MULTIMAP,
        FIELD_DERIVED_OBJECT_UNSORTED_MAP = UNSORTED_MAP,
        FIELD_DERIVED_OBJECT_SORTED_MAP = SORTED_MAP,

        /** abstract derived types for array containers */
        FIELD_DERIVED_ARRAY_SORTED_MULTISET = SORTED_MULTISET_ARRAY,
        FIELD_DERIVED_ARRAY_UNSORTED_SET = UNSORTED_SET_ARRAY,
        FIELD_DERIVED_ARRAY_SORTED_SET = SORTED_SET_ARRAY,

        /** abstract derived types for column-u8 containers */
        FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET = SORTED_MULTISET_COL_U8,
        FIELD_DERIVED_COLUMN_U8_UNSORTED_SET = UNSORTED_SET_COL_U8,
        FIELD_DERIVED_COLUMN_U8_SORTED_SET = SORTED_SET_COL_U8,

        /** abstract derived types for column-u16 containers */
        FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET = SORTED_MULTISET_COL_U16,
        FIELD_DERIVED_COLUMN_U16_UNSORTED_SET = UNSORTED_SET_COL_U16,
        FIELD_DERIVED_COLUMN_U16_SORTED_SET = SORTED_SET_COL_U16,

        /** abstract derived types for column-u32 containers */
        FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET = SORTED_MULTISET_COL_U32,
        FIELD_DERIVED_COLUMN_U32_UNSORTED_SET = UNSORTED_SET_COL_U32,
        FIELD_DERIVED_COLUMN_U32_SORTED_SET = SORTED_SET_COL_U32,

        /** abstract derived types for column-u64 containers */
        FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET = SORTED_MULTISET_COL_U64,
        FIELD_DERIVED_COLUMN_U64_UNSORTED_SET = UNSORTED_SET_COL_U64,
        FIELD_DERIVED_COLUMN_U64_SORTED_SET = SORTED_SET_COL_U64,

        /** abstract derived types for column-i8 containers */
        FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET = SORTED_MULTISET_COL_I8,
        FIELD_DERIVED_COLUMN_I8_UNSORTED_SET = UNSORTED_SET_COL_I8,
        FIELD_DERIVED_COLUMN_I8_SORTED_SET = SORTED_SET_COL_I8,

        /** abstract derived types for column-i16 containers */
        FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET = SORTED_MULTISET_COL_I16,
        FIELD_DERIVED_COLUMN_I16_UNSORTED_SET = UNSORTED_SET_COL_I16,
        FIELD_DERIVED_COLUMN_I16_SORTED_SET = SORTED_SET_COL_I16,

        /** abstract derived types for column-i32 containers */
        FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET = SORTED_MULTISET_COL_I32,
        FIELD_DERIVED_COLUMN_I32_UNSORTED_SET = UNSORTED_SET_COL_I32,
        FIELD_DERIVED_COLUMN_I32_SORTED_SET = SORTED_SET_COL_I32,

        /** abstract derived types for column-i64 containers */
        FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET = SORTED_MULTISET_COL_I64,
        FIELD_DERIVED_COLUMN_I64_UNSORTED_SET = UNSORTED_SET_COL_I64,
        FIELD_DERIVED_COLUMN_I64_SORTED_SET = SORTED_SET_COL_I64,

        /** abstract derived types for column-float containers */
        FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET = SORTED_MULTISET_COL_FLOAT,
        FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET = UNSORTED_SET_COL_FLOAT,
        FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET = SORTED_SET_COL_FLOAT,

        /** abstract derived types for column-boolean containers */
        FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET = SORTED_MULTISET_COL_BOOLEAN,
        FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET = UNSORTED_SET_COL_BOOLEAN,
        FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET = SORTED_SET_COL_BOOLEAN,

        /** character strings */
        FIELD_STRING = MSTRING, /** UTF-8 str_buf */

        /** numbers */
        FIELD_NUMBER_U8 = MU8, /** 8bit unsigned integer */
        FIELD_NUMBER_U16 = MU16, /** 16bit unsigned integer */
        FIELD_NUMBER_U32 = MU32, /** 32bit unsigned integer */
        FIELD_NUMBER_U64 = MU64, /** 64bit unsigned integer */
        FIELD_NUMBER_I8 = MI8, /** 8bit signed integer */
        FIELD_NUMBER_I16 = MI16, /** 16bit signed integer */
        FIELD_NUMBER_I32 = MI32, /** 32bit signed integer */
        FIELD_NUMBER_I64 = MI64, /** 64bit signed integer */
        FIELD_NUMBER_FLOAT = MFLOAT, /** 32bit float */

        /** binary data */
        FIELD_BINARY = MBINARY, /** arbitrary binary object with known mime type */
        FIELD_BINARY_CUSTOM = MCUSTOM_BINARY, /** arbitrary binary object with unknown mime type*/
} field_e;

typedef enum col_it_type {
        COLUMN_U8,
        COLUMN_U16,
        COLUMN_U32,
        COLUMN_U64,
        COLUMN_I8,
        COLUMN_I16,
        COLUMN_I32,
        COLUMN_I64,
        COLUMN_FLOAT,
        COLUMN_BOOLEAN
} col_it_type_e;

typedef enum field_class {
        CLASS_CONSTANT,
        CLASS_NUMBER,
        CLASS_CHARACTER_STRING,
        CLASS_BINARY_STRING,
        CLASS_CONTAINER
} field_class_e;

typedef enum constant {
        CONST_TRUE,
        CONST_FALSE,
        CONST_NULL
} constant_e;

#define NULL_STR "null"
#define TRUE_STR "boolean-true"
#define FALSE_STR "boolean-false"
#define OBJECT_UNSORTED_MULTIMAP_STR "unsorted-multimap"
#define OBJECT_SORTED_MULTIMAP_STR "sorted-multimap"
#define OBJECT_UNSORTED_MAP_STR "unsorted-map"
#define OBJECT_SORTED_MAP_STR "sorted-map"
#define ARRAY_UNSORTED_MULTISET_STR "array-unsorted-multiset"
#define ARRAY_SORTED_MULTISET_STR "array-sorted-multiset"
#define ARRAY_UNSORTED_SET_STR "array-unsorted-set"
#define ARRAY_SORTED_SET_STR "array-sorted-set"
#define COLUMN_U8_UNSORTED_MULTISET_STR "column-u8-unsorted-multiset"
#define COLUMN_U8_SORTED_MULTISET_STR "column-u8-sorted-multiset"
#define COLUMN_U8_UNSORTED_SET_STR "column-u8-unsorted-set"
#define COLUMN_U8_SORTED_SET_STR "column-u8-sorted-set"
#define COLUMN_U16_UNSORTED_MULTISET_STR "column-u16-unsorted-multiset"
#define COLUMN_U16_SORTED_MULTISET_STR "column-u16-sorted-multiset"
#define COLUMN_U16_UNSORTED_SET_STR "column-u16-unsorted-set"
#define COLUMN_U16_SORTED_SET_STR "column-u16-sorted-set"
#define COLUMN_U32_UNSORTED_MULTISET_STR "column-u32-unsorted-multiset"
#define COLUMN_U32_SORTED_MULTISET_STR "column-u32-sorted-multiset"
#define COLUMN_U32_UNSORTED_SET_STR "column-u32-unsorted-set"
#define COLUMN_U32_SORTED_SET_STR "column-u32-sorted-set"
#define COLUMN_U64_UNSORTED_MULTISET_STR "column-u64-unsorted-multiset"
#define COLUMN_U64_SORTED_MULTISET_STR "column-u64-sorted-multiset"
#define COLUMN_U64_UNSORTED_SET_STR "column-u64-unsorted-set"
#define COLUMN_U64_SORTED_SET_STR "column-u64-sorted-set"
#define COLUMN_I8_UNSORTED_MULTISET_STR "column-i8-unsorted-multiset"
#define COLUMN_I8_SORTED_MULTISET_STR "column-i8-sorted-multiset"
#define COLUMN_I8_UNSORTED_SET_STR "column-i8-unsorted-set"
#define COLUMN_I8_SORTED_SET_STR "column-i8-sorted-set"
#define COLUMN_I16_UNSORTED_MULTISET_STR "column-i16-unsorted-multiset"
#define COLUMN_I16_SORTED_MULTISET_STR "column-i16-sorted-multiset"
#define COLUMN_I16_UNSORTED_SET_STR "column-i16-unsorted-set"
#define COLUMN_I16_SORTED_SET_STR "column-i16-sorted-set"
#define COLUMN_I32_UNSORTED_MULTISET_STR "column-i32-unsorted-multiset"
#define COLUMN_I32_SORTED_MULTISET_STR "column-i32-sorted-multiset"
#define COLUMN_I32_UNSORTED_SET_STR "column-i32-unsorted-set"
#define COLUMN_I32_SORTED_SET_STR "column-i32-sorted-set"
#define COLUMN_I64_UNSORTED_MULTISET_STR "column-i64-unsorted-multiset"
#define COLUMN_I64_SORTED_MULTISET_STR "column-i64-sorted-multiset"
#define COLUMN_I64_UNSORTED_SET_STR "column-i64-unsorted-set"
#define COLUMN_I64_SORTED_SET_STR "column-i64-sorted-set"
#define COLUMN_FLOAT_UNSORTED_MULTISET_STR "column-float-unsorted-multiset"
#define COLUMN_FLOAT_SORTED_MULTISET_STR "column-float-sorted-multiset"
#define COLUMN_FLOAT_UNSORTED_SET_STR "column-float-unsorted-set"
#define COLUMN_FLOAT_SORTED_SET_STR "column-float-sorted-set"
#define COLUMN_BOOLEAN_UNSORTED_MULTISET_STR "column-boolean-unsorted-multiset"
#define COLUMN_BOOLEAN_SORTED_MULTISET_STR "column-boolean-sorted-multiset"
#define COLUMN_BOOLEAN_UNSORTED_SET_STR "column-boolean-unsorted-set"
#define COLUMN_BOOLEAN_SORTED_SET_STR "column-boolean-sorted-set"
#define STRING_STR "string"
#define BINARY_STR "binary"
#define NUMBER_U8_STR "number-u8"
#define NUMBER_U16_STR "number-u16"
#define NUMBER_U32_STR "number-u32"
#define NUMBER_U64_STR "number-u64"
#define NUMBER_I8_STR "number-i8"
#define NUMBER_I16_STR "number-i16"
#define NUMBER_I32_STR "number-i32"
#define NUMBER_I64_STR "number-i64"
#define NUMBER_FLOAT_STR "number-float"

#ifdef __cplusplus
extern "C" {
#endif

const char *field_str(field_e type);

field_class_e field_get_class(field_e type);

bool carbon_field_skip(memfile *file);

bool carbon_field_skip_object(memfile *file, u8 marker);
bool carbon_field_skip_array(memfile *file, u8 marker);
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

#define CARBON_FIELD_SKIP_BINARY__FAST(memfile, field)                                                                \
        /** skip blob */                                                                \
        MEMFILE_SKIP__UNSAFE((memfile), (field)->len);

#define CARBON_FIELD_SKIP_CUSTOM_BINARY__FAST(memfile, field)                                                                \
        /** skip blob */                                                                \
        MEMFILE_SKIP__UNSAFE((memfile), (field)->len);

#define CARBON_FIELD_SKIP_STRING__FAST(memfile, field)                                                                \
        /** skip blob */                                                                \
        MEMFILE_SKIP__UNSAFE((memfile), (field)->len);

#define CARBON_FIELD_SKIP_FLOAT__FAST(memfile)                                                                \
        MEMFILE_SKIP__UNSAFE((memfile), sizeof(float));

#define CARBON_FIELD_SKIP_8__FAST(memfile)                                                                \
        MEMFILE_SKIP__UNSAFE((memfile), sizeof(u8));

#define CARBON_FIELD_SKIP_16__FAST(memfile)                                                                \
        MEMFILE_SKIP__UNSAFE((memfile), sizeof(u16));

#define CARBON_FIELD_SKIP_32__FAST(memfile)                                                                \
        MEMFILE_SKIP__UNSAFE((memfile), sizeof(u32));

#define CARBON_FIELD_SKIP_64__FAST(memfile)                                                                 \
        MEMFILE_SKIP__UNSAFE((memfile), sizeof(u64));

field_e field_for_column(list_type_e derivation, col_it_type_e type);
field_e field_column_entry_to_regular_type(field_e type, bool is_null, bool is_true);



#define field_is_traversable(_field_e_type_)                                                                             \
        (field_is_object_or_subtype(_field_e_type_) ||                                                                   \
         field_is_list_or_subtype(_field_e_type_))

#define field_is_signed(_field_e_type_)                                                                                  \
        ((_field_e_type_ == FIELD_NUMBER_I8 || _field_e_type_ == FIELD_NUMBER_I16 ||                                       \
          _field_e_type_ == FIELD_NUMBER_I32 || _field_e_type_ == FIELD_NUMBER_I64 ||                                      \
          _field_e_type_ == FIELD_COLUMN_I8_UNSORTED_MULTISET ||                                                         \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET ||                                                   \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I8_UNSORTED_SET ||                                                      \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I8_SORTED_SET ||                                                        \
          _field_e_type_ == FIELD_COLUMN_I16_UNSORTED_MULTISET ||                                                        \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET ||                                                  \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I16_UNSORTED_SET ||                                                     \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I16_SORTED_SET ||                                                       \
          _field_e_type_ == FIELD_COLUMN_I32_UNSORTED_MULTISET ||                                                        \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET ||                                                  \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I32_UNSORTED_SET ||                                                     \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I32_SORTED_SET ||                                                       \
          _field_e_type_ == FIELD_COLUMN_I64_UNSORTED_MULTISET ||                                                        \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET ||                                                  \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I64_UNSORTED_SET ||                                                     \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I64_SORTED_SET))

#define field_is_unsigned(_field_e_type_)                                                                                \
        ((_field_e_type_ == FIELD_NUMBER_U8 || _field_e_type_ == FIELD_NUMBER_U16 ||                                       \
          _field_e_type_ == FIELD_NUMBER_U32 || _field_e_type_ == FIELD_NUMBER_U64 ||                                      \
          _field_e_type_ == FIELD_COLUMN_U8_UNSORTED_MULTISET ||                                                         \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET ||                                                   \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U8_UNSORTED_SET ||                                                      \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U8_SORTED_SET ||                                                        \
          _field_e_type_ == FIELD_COLUMN_U16_UNSORTED_MULTISET ||                                                        \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET ||                                                  \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U16_UNSORTED_SET ||                                                     \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U16_SORTED_SET ||                                                       \
          _field_e_type_ == FIELD_COLUMN_U32_UNSORTED_MULTISET ||                                                        \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET ||                                                  \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U32_UNSORTED_SET ||                                                     \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U32_SORTED_SET ||                                                       \
          _field_e_type_ == FIELD_COLUMN_U64_UNSORTED_MULTISET ||                                                        \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET ||                                                  \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U64_UNSORTED_SET ||                                                     \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U64_SORTED_SET))

#define field_is_floating(_field_e_type_)                                                                                \
        ((_field_e_type_ == FIELD_NUMBER_FLOAT || _field_e_type_ == FIELD_COLUMN_FLOAT_UNSORTED_MULTISET ||                \
          _field_e_type_ == FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET ||                                                \
          _field_e_type_ == FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET ||                                                   \
          _field_e_type_ == FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET))

#define field_is_number(_field_e_type_)                                                                                  \
        (field_is_integer(_field_e_type_) || field_is_floating(_field_e_type_))

#define field_is_integer(_field_e_type_)                                                                                 \
        (field_is_signed(_field_e_type_) || field_is_unsigned(_field_e_type_))

#define field_is_binary(_field_e_type_)                                                                                  \
        ((_field_e_type_ == FIELD_BINARY || _field_e_type_ == FIELD_BINARY_CUSTOM))

#define field_is_boolean(_field_e_type_)                                                                                 \
        ((_field_e_type_ == FIELD_TRUE || _field_e_type_ == FIELD_FALSE ||                                                 \
          _field_e_type_ == FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET ||                                                    \
          _field_e_type_ == FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET ||                                              \
          _field_e_type_ == FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET ||                                                 \
          _field_e_type_ == FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET))

#define field_is_string(_field_e_type_)                                                                                  \
        (_field_e_type_ == FIELD_STRING)

#define field_is_constant(_field_e_type_)                                                                                \
        ((field_is_null(_field_e_type_) || field_is_boolean(_field_e_type_)))

#define field_is_array_or_subtype(_field_e_type_)                                                                        \
        ((_field_e_type_ == FIELD_ARRAY_UNSORTED_MULTISET || _field_e_type_ == FIELD_DERIVED_ARRAY_SORTED_MULTISET ||      \
          _field_e_type_ == FIELD_DERIVED_ARRAY_UNSORTED_SET || _field_e_type_ == FIELD_DERIVED_ARRAY_SORTED_SET))

#define field_is_column_u8_or_subtype(_field_e_type_)                                                                    \
        ((_field_e_type_ == FIELD_COLUMN_U8_UNSORTED_MULTISET ||                                                         \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET ||                                                   \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U8_UNSORTED_SET ||                                                      \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U8_SORTED_SET))

#define field_is_column_u16_or_subtype(_field_e_type_)                                                                   \
        ((_field_e_type_ == FIELD_COLUMN_U16_UNSORTED_MULTISET ||                                                        \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET ||                                                  \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U16_UNSORTED_SET ||                                                     \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U16_SORTED_SET))

#define field_is_column_u32_or_subtype(_field_e_type_)                                                                   \
        ((_field_e_type_ == FIELD_COLUMN_U32_UNSORTED_MULTISET ||                                                        \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET ||                                                  \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U32_UNSORTED_SET ||                                                     \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U32_SORTED_SET))

#define field_is_column_u64_or_subtype(_field_e_type_)                                                                   \
        ((_field_e_type_ == FIELD_COLUMN_U64_UNSORTED_MULTISET ||                                                        \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET ||                                                  \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U64_UNSORTED_SET ||                                                     \
          _field_e_type_ == FIELD_DERIVED_COLUMN_U64_SORTED_SET))

#define field_is_column_i8_or_subtype(_field_e_type_)                                                                    \
        ((_field_e_type_ == FIELD_COLUMN_I8_UNSORTED_MULTISET ||                                                         \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET ||                                                   \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I8_UNSORTED_SET ||                                                      \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I8_SORTED_SET))

#define field_is_column_i16_or_subtype(_field_e_type_)                                                                   \
        ((_field_e_type_ == FIELD_COLUMN_I16_UNSORTED_MULTISET ||                                                        \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET ||                                                  \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I16_UNSORTED_SET ||                                                     \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I16_SORTED_SET))

#define field_is_column_i32_or_subtype(_field_e_type_)                                                                   \
        ((_field_e_type_ == FIELD_COLUMN_I32_UNSORTED_MULTISET ||                                                        \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET ||                                                  \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I32_UNSORTED_SET ||                                                     \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I32_SORTED_SET))

#define field_is_column_i64_or_subtype(_field_e_type_)                                                                   \
        ((_field_e_type_ == FIELD_COLUMN_I64_UNSORTED_MULTISET ||                                                        \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET ||                                                  \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I64_UNSORTED_SET ||                                                     \
          _field_e_type_ == FIELD_DERIVED_COLUMN_I64_SORTED_SET))

#define field_is_column_float_or_subtype(_field_e_type_)                                                                 \
        ((_field_e_type_ == FIELD_COLUMN_FLOAT_UNSORTED_MULTISET ||                                                      \
          _field_e_type_ == FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET ||                                                \
          _field_e_type_ == FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET ||                                                   \
          _field_e_type_ == FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET))

#define field_is_column_bool_or_subtype(_field_e_type_)                                                                  \
        ((_field_e_type_ == FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET ||                                                    \
          _field_e_type_ == FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET ||                                              \
          _field_e_type_ == FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET ||                                                 \
          _field_e_type_ == FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET))

#define field_is_list_or_subtype(_field_e_type_)                                                                         \
        (field_is_array_or_subtype(_field_e_type_) || field_is_column_or_subtype(_field_e_type_))

#define field_is_column_or_subtype(_field_e_type_)                                                                       \
        (field_is_column_u8_or_subtype(_field_e_type_) ||                                                                \
         field_is_column_u16_or_subtype(_field_e_type_) ||                                                               \
         field_is_column_u32_or_subtype(_field_e_type_) ||                                                               \
         field_is_column_u64_or_subtype(_field_e_type_) ||                                                               \
         field_is_column_i8_or_subtype(_field_e_type_) ||                                                                \
         field_is_column_i16_or_subtype(_field_e_type_) ||                                                               \
         field_is_column_i32_or_subtype(_field_e_type_) ||                                                               \
         field_is_column_i64_or_subtype(_field_e_type_) ||                                                               \
         field_is_column_float_or_subtype(_field_e_type_) ||                                                             \
         field_is_column_bool_or_subtype(_field_e_type_))

#define field_is_object_or_subtype(_field_e_type_)                                                                       \
        ((_field_e_type_ == FIELD_OBJECT_UNSORTED_MULTIMAP || _field_e_type_ == FIELD_DERIVED_OBJECT_SORTED_MULTIMAP ||    \
          _field_e_type_ == FIELD_DERIVED_OBJECT_UNSORTED_MAP ||                                                         \
          _field_e_type_ == FIELD_DERIVED_OBJECT_SORTED_MAP))

#define field_is_null(_field_e_type_)                                                                                    \
        (_field_e_type_ == FIELD_NULL)

#ifdef __cplusplus
}
#endif

#endif
