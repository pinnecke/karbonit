/*
 * item - a thing that is returned by array and object iterators
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_ITEM_H
#define HAD_ITEM_H

#include <jakson/stdinc.h>
#include <jakson/types.h>
#include <jakson/forwdecl.h>
#include <jakson/carbon/container.h>
#include <jakson/carbon/binary.h>
#include <jakson/carbon/string-field.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum item_type
{
    ITEM_UNDEF,
    ITEM_NULL,
    ITEM_TRUE,
    ITEM_FALSE,
    ITEM_STRING,
    ITEM_NUMBER_SIGNED,
    ITEM_NUMBER_UNSIGNED,
    ITEM_NUMBER_FLOAT,
    ITEM_BINARY,
    ITEM_ARRAY,
    ITEM_COLUMN,
    ITEM_OBJECT
} item_type_e;

typedef struct item
{
    untyped_e parent_type;
    union {
        arr_it *array;
        obj_it *object;
    } parent;
    u64 idx;

    item_type_e value_type;
    union {
        i64 number_signed;
        u64 number_unsigned;
        float number_float;
        string_field string;
        binary_field binary;
        arr_it *array;
        col_it *column;
        obj_it *object;
    } value;
} item;

#define INTERNAL_ITEM_IS_TYPE(item, type)                                                                       \
        (item_get_type(item) == type)

#define INTERNAL_ITEM_GET_VALUE(item, member, default_value)                                                    \
        ((item) ? (item)->value.member : default_value)

#define item_is_field(item)                                                                                     \
        ((item) ? (item)->parent_type == UNTYPED_ARRAY : false)

#define item_is_prop(item)                                                                                      \
        ((item) ? (item)->parent_type == UNTYPED_OBJECT : false)

#define item_get_type(item)                                                                                     \
        ((item) ? (item)->value_type : ITEM_UNDEF)

#define item_is_undef(item)                                                                                     \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_UNDEF)

#define item_is_null(item)                                                                                      \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_NULL)

#define item_is_true(item)                                                                                      \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_TRUE)

#define item_is_false(item)                                                                                     \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_FALSE)

#define item_is_boolean(item)                                                                                   \
        (item_is_true(item) || item_is_false(item))

#define item_is_string(item)                                                                                    \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_STRING)

#define item_is_signed(item)                                                                                    \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_NUMBER_SIGNED)

#define item_is_unsigned(item)                                                                                  \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_NUMBER_UNSIGNED)

#define item_is_float(item)                                                                                     \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_NUMBER_FLOAT)

#define item_is_number(item)                                                                                    \
        (item_is_signed(item) || item_is_unsigned(item) || item_is_float(item))

#define item_is_binary(item)                                                                                    \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_BINARY)

#define item_is_array(item)                                                                                     \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_ARRAY)

#define item_is_column(item)                                                                                    \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_COLUMN)

#define item_is_object(item)                                                                                    \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_OBJECT)

#define item_get_index(item)                                                                                    \
        ((item) ? (item)->idx : 0)

#define item_get_number_signed(item, default_value)                                                             \
        INTERNAL_ITEM_GET_VALUE(item, number_signed, default_value)

#define item_get_number_unsigned(item, default_value)                                                           \
        INTERNAL_ITEM_GET_VALUE(item, number_unsigned, default_value)

#define item_get_number_float(item, default_value)                                                              \
        INTERNAL_ITEM_GET_VALUE(item, number_float, default_value)

#define item_get_string(item, default_value)                                                                    \
        INTERNAL_ITEM_GET_VALUE(item, string, default_value)

#define item_get_binary(item, default_value)                                                                    \
        INTERNAL_ITEM_GET_VALUE(item, binary, default_value)

#define item_get_array(item)                                                                                    \
        INTERNAL_ITEM_GET_VALUE(item, array, NULL)

#define item_get_column(item)                                                                                   \
        INTERNAL_ITEM_GET_VALUE(item, column, NULL)

#define item_get_object(item)                                                                                   \
        INTERNAL_ITEM_GET_VALUE(item, object, NULL)

#define item_remove(item)                                                                                       \
        ((item)->parent == UNTYPED_ARRAY ? internal_arr_it_remove((item)->parent.array) :                  \
                                                 internal_obj_it_remove((item)->parent.object))

#define item_set_null(item)                                                                                     \
        ((item)->parent == UNTYPED_ARRAY ? internal_arr_it_update_null((item)->parent.array) :             \
                                                 internal_obj_it_update_null((item)->parent.object))            \

#define item_set_true(item)                                                                                     \
        ((item)->parent == UNTYPED_ARRAY ? internal_arr_it_update_true((item)->parent.array) :             \
                                                 internal_obj_it_update_true((item)->parent.object))

#define item_set_false(item)                                                                                    \
        ((item)->parent == UNTYPED_ARRAY ? internal_arr_it_update_false((item)->parent.array) :            \
                                                 internal_obj_it_update_false((item)->parent.object)

#define item_set_number_float(item, float_value)                                                                \
        ((item)->parent == UNTYPED_ARRAY ? internal_arr_it_update_float((item)->parent.array, float_value):\
                                                 internal_obj_it_update_float((item)->parent.object, float_value))

#define item_set_number_signed(item, i64_value)                                                                 \
({                                                                                                                     \
        bool ret = false;                                                                                              \
        switch (number_min_type_signed(value)) {                                                                       \
                case NUMBER_I8:                                                                                        \
                        ret = ((item)->parent == UNTYPED_ARRAY ?                                                 \
                                        internal_arr_it_update_i8(subj->parent.array, (i8) i64_value) :          \
                                        internal_obj_it_update_i8(subj->parent.object, (i8) i64_value));        \
                break;                                                                                                 \
                case NUMBER_I16:                                                                                       \
                        ret = ((item)->parent == UNTYPED_ARRAY ?                                                 \
                                        internal_arr_it_update_i16(subj->parent.array, (i16) i64_value) :        \
                                        internal_obj_it_update_i16(subj->parent.object, (i16) i64_value));      \
                break;                                                                                                 \
                case NUMBER_I32:                                                                                       \
                        ret = ((item)->parent == UNTYPED_ARRAY ?                                                 \
                                        internal_arr_it_update_i32(subj->parent.array, (i32) i64_value) :        \
                                        internal_obj_it_update_i32(subj->parent.object, (i32) i64_value));      \
                break;                                                                                                 \
                case NUMBER_I64:                                                                                       \
                        ret = ((item)->parent == UNTYPED_ARRAY ?                                                 \
                                        internal_arr_it_update_i64(subj->parent.array, i64_value) :              \
                                        internal_obj_it_update_i64(subj->parent.ojbect, i64_value) :            \
                break;                                                                                                 \
                default:                                                                                               \
                        error(ERR_INTERNALERR, NULL)                                                                   \
                break;                                                                                                 \
        }                                                                                                              \
        ret;                                                                                                           \
})

#define item_set_number_unsigned(item, u64_value)                                                               \
({                                                                                                                     \
        bool ret = false;                                                                                              \
        switch (number_min_type_unsigned(value)) {                                                                     \
                case NUMBER_U8:                                                                                        \
                        ret = ((item)->parent == UNTYPED_ARRAY ?                                                 \
                                        internal_arr_it_update_u8(subj->parent.array, (u8) u64_value) :          \
                                        internal_obj_it_update_u8(subj->parent.object, (u8) u64_value));        \
                break;                                                                                                 \
                case NUMBER_U16:                                                                                       \
                        ret = ((item)->parent == UNTYPED_ARRAY ?                                                 \
                                        internal_arr_it_update_u16(subj->parent.array, (u16) u64_value) :        \
                                        internal_obj_it_update_u16(subj->parent.object, (u16) u64_value));      \
                break;                                                                                                 \
                case NUMBER_U32:                                                                                       \
                        ret = ((item)->parent == UNTYPED_ARRAY ?                                                 \
                                        internal_arr_it_update_u32(subj->parent.array, (u32) u64_value) :        \
                                        internal_obj_it_update_u32(subj->parent.object, (u32) u64_value));      \
                break;                                                                                                 \
                case NUMBER_U64:                                                                                       \
                        ret = ((item)->parent == UNTYPED_ARRAY ?                                                 \
                                        internal_arr_it_update_u64(subj->parent.array, u64_value) :              \
                                        internal_obj_it_update_u64(subj->parent.object, u64_value) :            \
                break;                                                                                                 \
                default:                                                                                               \
                        error(ERR_INTERNALERR, NULL)                                                                   \
                break;                                                                                                 \
        }                                                                                                              \
        ret;                                                                                                           \
})

#define item_set_string(item, const_char_str)                                                                   \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_string((item)->parent.array, const_char_str) :            \
                                internal_obj_it_update_string((item)->parent.object, const_char_str))

#define item_set_binary(item, const_void_ptr_value, size_t_nbytes, const_char_ptr_file_ext,                     \
                               const_char_ptr_user_type)                                                               \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_binary((item)->parent.array, const_void_ptr_value,        \
                                                                    size_t_nbytes, const_char_ptr_file_ext,            \
                                                                    const_char_ptr_user_type) :                        \
                                internal_obj_it_update_binary((item)->parent.object, const_void_ptr_value,      \
                                                                    size_t_nbytes, const_char_ptr_file_ext,            \
                                                                    const_char_ptr_user_type))

#define item_set_array_begin(state, item)                                                                       \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_array_begin((state), (item)->parent.array) :              \
                                internal_obj_it_update_array_begin((state), (item)->parent.object))             \

#define item_set_array_end(state)                                                                               \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_array_end((state)) :                                      \
                                internal_obj_it_update_array_end((state)))

#define item_set_column_begin(state, item)                                                                      \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_column_begin((state), (item)->parent.array) :             \
                                internal_obj_it_update_column_begin((state), (item)->parent.object))

#define item_set_column_end(state)                                                                              \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_column_end((state)) :                                     \
                                internal_obj_it_update_column_end((state)))

#define item_set_object_begin(state, item)                                                                      \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_object_begin((state), (item)->parent.array) :             \
                                internal_obj_it_update_object_begin((state), (item)->parent.object))

#define item_set_object_end(state)                                                                              \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_object_end((state)) :                                     \
                                internal_obj_it_update_object_end((state))

#define item_set_from_record(item, const_carbon_ptr_src)                                                        \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_from_carbon((item)->parent.array, const_carbon_ptr_src) : \
                                internal_obj_it_update_from_carbon((item)->parent.object, const_carbon_ptr_src))

#define item_set_from_array(item, const_arr_it_ptr_src)                                                   \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_from_array((item)->>parent.array,                         \
                                                                        const_arr_it_ptr_src) :                  \
                                internal_obj_it_update_from_array((item)->>parent.object,                       \
                                                                        const_arr_it_ptr_src))

#define item_set_from_object(item, const_obj_it_ptr_src)                                                 \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_from_object((item)->parent.array,                         \
                                                                         const_obj_it_ptr_src) :                \
                                internal_obj_it_update_from_object((item)->parent.object,                       \
                                                                         const_obj_it_ptr_src))

#define item_set_from_column(item, const_col_it_ptr_src)                                                 \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_from_column((item)->parent.array,                         \
                                                                         const_col_it_ptr_src) :                \
                                internal_obj_it_update_from_column((item)->parent.object,                       \
                                                                         const_col_it_ptr_src))

// ---------------------------------------------------------------------------------------------------------------------
//  for internal usage
// ---------------------------------------------------------------------------------------------------------------------


#define ITEM_SETUP_VALUE(item, field_type, field)                                                               \
({                                                               \
        bool status = true;                                                            \
        if (field_is_signed(field_type) && !field_is_list_or_subtype(field_type)) {                                                               \
                internal_field_signed_value(&item->value.number_signed, field);                                                               \
                item->value_type = ITEM_NUMBER_SIGNED;                                                               \
        } else if (field_is_unsigned(field_type) && !field_is_list_or_subtype(field_type)) {                                                               \
                internal_field_unsigned_value(&item->value.number_unsigned, field);                                                               \
                item->value_type = ITEM_NUMBER_UNSIGNED;                                                               \
        } else if (field_is_floating(field_type) && !field_is_list_or_subtype(field_type)) {                                                               \
                internal_field_float_value(&item->value.number_float, field);                                                               \
                item->value_type = ITEM_NUMBER_FLOAT;                                                               \
        } else if (field_is_binary(field_type)) {                                                               \
                internal_field_binary_value(&item->value.binary, field);                                                               \
                item->value_type = ITEM_BINARY;                                                               \
        } else if (field_is_boolean(field_type) && !field_is_list_or_subtype(field_type)) {                                                               \
                item->value_type = field_type == FIELD_TRUE ? ITEM_TRUE : ITEM_FALSE;                                                               \
        } else if (field_is_array_or_subtype(field_type)) {                                                               \
                item->value.array = internal_field_array_value(field);                                                               \
                item->value_type = ITEM_ARRAY;                                                               \
        } else if (field_is_column_or_subtype(field_type)) {                                                               \
                item->value.column = internal_field_column_value(field);                                                               \
                item->value_type = ITEM_COLUMN;                                                               \
        } else if (field_is_object_or_subtype(field_type)) {                                                               \
                item->value.object = internal_field_object_value(field);                                                               \
                item->value_type = ITEM_OBJECT;                                                               \
        } else if (field_is_null(field_type)) {                                                               \
                item->value_type = ITEM_NULL;                                                               \
        } else if (field_is_string(field_type)) {                                                               \
                item->value.string.str = internal_field_string_value(&item->value.string.len, field);                                                               \
                item->value_type = ITEM_STRING;                                                               \
        } else {                                                               \
                item->value_type = ITEM_UNDEF;                                                               \
                status = false;                                                               \
        }                                                               \
        status;                                                               \
})

#define INTERNAL_ITEM_CREATE_FROM_ARRAY(item, arr_it)                                                   \
({                                                                                                       \
        (item)->parent_type = UNTYPED_ARRAY;                                                              \
        (item)->parent.array = (arr_it);                                                                    \
        (item)->idx = (arr_it)->pos;                                                                        \
        field_e field_type = (arr_it)->field.type;                                                        \
        (ITEM_SETUP_VALUE((item), field_type, &(arr_it)->field));                                           \
})

bool internal_item_create_from_object(item *item, obj_it *parent);

#ifdef __cplusplus
}
#endif

#endif

