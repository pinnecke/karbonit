/*
 * item - a thing that is returned by array and object iterators
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_ITEM_H
#define HAD_ITEM_H

#include <karbonit/stdinc.h>
#include <karbonit/types.h>
#include <karbonit/forwdecl.h>
#include <karbonit/carbon/container.h>
#include <karbonit/carbon/binary.h>
#include <karbonit/carbon/string-field.h>

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
    } value;
} item;

#define INTERNAL_ITEM_IS_TYPE(item, type)                                                                       \
        (ITEM_GET_TYPE(item) == type)

#define INTERNAL_ITEM_GET_VALUE(item, member, default_value)                                                    \
        ((item) ? (item)->value.member : default_value)

#define ITEM_IS_FIELD(item)                                                                                     \
        ((item) ? (item)->parent_type == UNTYPED_ARRAY : false)

#define ITEM_IS_PROP(item)                                                                                      \
        ((item) ? (item)->parent_type == UNTYPED_OBJECT : false)

#define ITEM_GET_TYPE(item)                                                                                     \
        ((item) ? (item)->value_type : ITEM_UNDEF)

#define ITEM_IS_UNDEF(item)                                                                                     \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_UNDEF)

#define ITEM_IS_NULL(item)                                                                                      \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_NULL)

#define ITEM_IS_TRUE(item)                                                                                      \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_TRUE)

#define ITEM_IS_FALSE(item)                                                                                     \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_FALSE)

#define ITEM_IS_BOOLEAN(item)                                                                                   \
        (ITEM_IS_TRUE(item) || ITEM_IS_FALSE(item))

#define ITEM_IS_STRING(item)                                                                                    \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_STRING)

#define ITEM_IS_SIGNED(item)                                                                                    \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_NUMBER_SIGNED)

#define ITEM_IS_UNSIGNED(item)                                                                                  \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_NUMBER_UNSIGNED)

#define ITEM_IS_FLOAT(item)                                                                                     \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_NUMBER_FLOAT)

#define ITEM_IS_NUMBER(item)                                                                                    \
        (ITEM_IS_SIGNED(item) || ITEM_IS_UNSIGNED(item) || ITEM_IS_FLOAT(item))

#define ITEM_IS_BINARY(item)                                                                                    \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_BINARY)

#define ITEM_IS_ARRAY(item)                                                                                     \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_ARRAY)

#define ITEM_IS_COLUMN(item)                                                                                    \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_COLUMN)

#define ITEM_IS_OBJECT(item)                                                                                    \
        INTERNAL_ITEM_IS_TYPE(item, ITEM_OBJECT)

#define ITEM_GET_INDEX(item)                                                                                    \
        ((item) ? (item)->idx : 0)

#define ITEM_GET_NUMBER_SIGNED(item, default_value)                                                             \
        INTERNAL_ITEM_GET_VALUE(item, number_signed, default_value)

#define ITEM_GET_NUMBER_UNSIGNED(item, default_value)                                                           \
        INTERNAL_ITEM_GET_VALUE(item, number_unsigned, default_value)

#define ITEM_GET_NUMBER_FLOAT(item, default_value)                                                              \
        INTERNAL_ITEM_GET_VALUE(item, number_float, default_value)

#define ITEM_GET_STRING(item, default_value)                                                                    \
        INTERNAL_ITEM_GET_VALUE(item, string, default_value)

#define ITEM_GET_BINARY(item, default_value)                                                                    \
        INTERNAL_ITEM_GET_VALUE(item, binary, default_value)

#define INTERNAL_ITEM_GET_ITERATOR(it, item, create_fn)                                                                 \
        if (LIKELY((item)->parent_type == UNTYPED_ARRAY)) {                                                                                   \
                create_fn((it), &((item)->parent.object->file), (item)->parent.array->field_offset);                                                                                        \
        } else {                                                                                   \
                create_fn((it), &((item)->parent.array->file), (item)->parent.object->field.value.start);                                                                                      \
        }

#define ITEM_GET_ARRAY(it, item)                                                                                    \
        INTERNAL_ITEM_GET_ITERATOR(it, item, internal_arr_it_create)

#define ITEM_GET_COLUMN(it, item)                                                                                   \
        INTERNAL_ITEM_GET_ITERATOR(it, item, col_it_create)

#define ITEM_GET_OBJECT(it, item)                                                                                   \
        INTERNAL_ITEM_GET_ITERATOR(it, item, internal_obj_it_create)

#define ITEM_REMOVE(item)                                                                                       \
        ((item)->parent == UNTYPED_ARRAY ? internal_arr_it_remove((item)->parent.array) :                  \
                                                 internal_obj_it_remove((item)->parent.object))

#define ITEM_SET_NULL(item)                                                                                     \
        ((item)->parent == UNTYPED_ARRAY ? internal_arr_it_update_null((item)->parent.array) :             \
                                                 internal_obj_it_update_null((item)->parent.object))            \

#define ITEM_SET_TRUE(item)                                                                                     \
        ((item)->parent == UNTYPED_ARRAY ? internal_arr_it_update_true((item)->parent.array) :             \
                                                 internal_obj_it_update_true((item)->parent.object))

#define ITEM_SET_FALSE(item)                                                                                    \
        ((item)->parent == UNTYPED_ARRAY ? internal_arr_it_update_false((item)->parent.array) :            \
                                                 internal_obj_it_update_false((item)->parent.object)

#define ITEM_SET_NUMBER_FLOAT(item, float_value)                                                                \
        ((item)->parent == UNTYPED_ARRAY ? internal_arr_it_update_float((item)->parent.array, float_value):\
                                                 internal_obj_it_update_float((item)->parent.object, float_value))

#define ITEM_SET_NUMBER_SIGNED(item, i64_value)                                                                 \
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
                        ERROR(ERR_INTERNALERR, NULL)                                                                   \
                break;                                                                                                 \
        }                                                                                                              \
        ret;                                                                                                           \
})

#define ITEM_SET_NUMBER_UNSIGNED(item, u64_value)                                                               \
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
                        ERROR(ERR_INTERNALERR, NULL)                                                                   \
                break;                                                                                                 \
        }                                                                                                              \
        ret;                                                                                                           \
})

#define ITEM_SET_STRING(item, const_char_str)                                                                   \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_string((item)->parent.array, const_char_str) :            \
                                internal_obj_it_update_string((item)->parent.object, const_char_str))

#define ITEM_SET_BINARY(item, const_void_ptr_value, size_t_nbytes, const_char_ptr_file_ext,                     \
                               const_char_ptr_user_type)                                                               \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_binary((item)->parent.array, const_void_ptr_value,        \
                                                                    size_t_nbytes, const_char_ptr_file_ext,            \
                                                                    const_char_ptr_user_type) :                        \
                                internal_obj_it_update_binary((item)->parent.object, const_void_ptr_value,      \
                                                                    size_t_nbytes, const_char_ptr_file_ext,            \
                                                                    const_char_ptr_user_type))

#define ITEM_SET_ARRAY_BEGIN(state, item)                                                                       \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_array_begin((state), (item)->parent.array) :              \
                                internal_obj_it_update_array_begin((state), (item)->parent.object))             \

#define ITEM_SET_ARRAY_END(state)                                                                               \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_array_end((state)) :                                      \
                                internal_obj_it_update_array_end((state)))

#define ITEM_SET_COLUMN_BEGIN(state, item)                                                                      \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_column_begin((state), (item)->parent.array) :             \
                                internal_obj_it_update_column_begin((state), (item)->parent.object))

#define ITEM_SET_COLUMN_END(state)                                                                              \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_column_end((state)) :                                     \
                                internal_obj_it_update_column_end((state)))

#define ITEM_SET_OBJECT_BEGIN(state, item)                                                                      \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_object_begin((state), (item)->parent.array) :             \
                                internal_obj_it_update_object_begin((state), (item)->parent.object))

#define ITEM_SET_OBJECT_END(state)                                                                              \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_object_end((state)) :                                     \
                                internal_obj_it_update_object_end((state))

#define ITEM_SET_FROM_REC(item, const_carbon_ptr_src)                                                        \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_from_carbon((item)->parent.array, const_carbon_ptr_src) : \
                                internal_obj_it_update_from_carbon((item)->parent.object, const_carbon_ptr_src))

#define ITEM_SET_FROM_ARRAY(item, const_arr_it_ptr_src)                                                   \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_from_array((item)->>parent.array,                         \
                                                                        const_arr_it_ptr_src) :                  \
                                internal_obj_it_update_from_array((item)->>parent.object,                       \
                                                                        const_arr_it_ptr_src))

#define ITEM_SET_FROM_OBJECT(item, const_obj_it_ptr_src)                                                 \
        ((item)->parent == UNTYPED_ARRAY ?                                                                       \
                                internal_arr_it_update_from_object((item)->parent.array,                         \
                                                                         const_obj_it_ptr_src) :                \
                                internal_obj_it_update_from_object((item)->parent.object,                       \
                                                                         const_obj_it_ptr_src))

#define ITEM_SET_FROM_COLUMN(item, const_col_it_ptr_src)                                                 \
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
        if (FIELD_IS_SIGNED(field_type) && !FIELD_IS_LIST_OR_SUBTYPE(field_type)) {                                                               \
                (item)->value.number_signed = INTERNAL_FIELD_SIGNED_VALUE((field));                                                               \
                (item)->value_type = ITEM_NUMBER_SIGNED;                                                               \
        } else if (FIELD_IS_UNSIGNED(field_type) && !FIELD_IS_LIST_OR_SUBTYPE(field_type)) {                                                               \
                (item)->value.number_unsigned = INTERNAL_FIELD_UNSIGNED_VALUE((field));                                                               \
                (item)->value_type = ITEM_NUMBER_UNSIGNED;                                                               \
        } else if (FIELD_IS_FLOATING(field_type) && !FIELD_IS_LIST_OR_SUBTYPE(field_type)) {                                                               \
                (item)->value.number_float = INTERNAL_FIELD_FLOAT_VALUE((field));                                                               \
                (item)->value_type = ITEM_NUMBER_FLOAT;                                                               \
        } else if (FIELD_IS_BINARY(field_type)) {                                                               \
                internal_field_binary_value(&(item)->value.binary, (field));                                                               \
                (item)->value_type = ITEM_BINARY;                                                               \
        } else if (FIELD_IS_BOOLEAN(field_type) && !FIELD_IS_LIST_OR_SUBTYPE(field_type)) {                                                               \
                (item)->value_type = field_type == FIELD_TRUE ? ITEM_TRUE : ITEM_FALSE;                                                               \
        } else if (FIELD_IS_ARRAY_OR_SUBTYPE(field_type)) {                                                               \
                (item)->value_type = ITEM_ARRAY;                                                               \
        } else if (FIELD_IS_COLUMN_OR_SUBTYPE(field_type)) {                                                               \
                (item)->value_type = ITEM_COLUMN;                                                               \
        } else if (FIELD_IS_OBJECT_OR_SUBTYPE(field_type)) {                                                               \
                (item)->value_type = ITEM_OBJECT;                                                               \
        } else if (FIELD_IS_BASE_NULL(field_type)) {                                                               \
                (item)->value_type = ITEM_NULL;                                                               \
        } else if (FIELD_IS_STRING(field_type)) {                                                               \
                (item)->value.string.str = internal_field_string_value(&(item)->value.string.len, (field));                                                               \
                (item)->value_type = ITEM_STRING;                                                               \
        } else {                                                               \
                (item)->value_type = ITEM_UNDEF;                                                               \
                status = false;                                                               \
        }                                                               \
        status;                                                               \
})

#define INTERNAL_ITEM_CREATE_FROM_ARRAY(item, parent_arr_it, pos, field, field_type)                                                   \
({                                                                                                       \
        (item)->parent_type = UNTYPED_ARRAY;                                                              \
        (item)->parent.array = parent_arr_it;                                                                  \
        (item)->idx = (pos);                                                                        \
        ITEM_SETUP_VALUE((item), (field_type), (field));                                           \
})

bool internal_item_create_from_object(item *item, obj_it *parent);

#ifdef __cplusplus
}
#endif

#endif

