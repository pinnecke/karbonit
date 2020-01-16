/*
 * col_it - column iterator implementation
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_COL_IT_H
#define HAD_COL_IT_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <karbonit/stdinc.h>
#include <karbonit/error.h>
#include <karbonit/carbon/field.h>
#include <karbonit/carbon/internal.h>
#include <karbonit/rec.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CAST_COL_VALUES(builtin_type, __nvalues__, iterator, field_expr)                                                   \
        ((const builtin_type *) (COL_IT_VALUES(NULL, __nvalues__, iterator)))

typedef struct col_it {
        memfile file;
        offset_t header_begin;
        offset_t begin;
        field_e field_type;
        list_type_e list_type;
        i64 mod_size; /* in case of modifications, the number of bytes that are added resp. removed */
        u32 cap;
        u32 num;
} col_it;

bool col_it_create(col_it *it, memfile *memfile, offset_t begin);
void internal_col_it_skip__fast(col_it *col);
bool col_it_clone(col_it *dst, col_it *src);
bool col_it_insert(insert *in, col_it *it);
bool col_it_fast_forward(col_it *it);
offset_t col_it_memfilepos(col_it *it);
void *col_it_memfile_raw(col_it *it);
offset_t col_it_tell(col_it *it, u32 elem_idx);


#define COL_IT_VALUES(field_e_ptr, _nvalues_u32_ptr_, _col_it_ptr_it_)                                                        \
({                                                                                                                     \
        const char *col_it_values_result = NULL;                                                                                     \
        {                                                                                                                       \
                col_it *it_ptr = (_col_it_ptr_it_);                                                                                 \
                MEMFILE_SEEK(&it_ptr->file, it_ptr->header_begin);                                                                     \
                u32 num_elements = (u32) MEMFILE_READ_UINTVAR_STREAM(NULL, &it_ptr->file); UNUSED(num_elements);                                     \
                u32 cap_elements = (u32) MEMFILE_READ_UINTVAR_STREAM(NULL, &it_ptr->file); UNUSED(cap_elements);                                     \
                offset_t payload_start = MEMFILE_TELL(&it_ptr->file);                                                          \
                col_it_values_result = MEMFILE_PEEK(&it_ptr->file, sizeof(char));                                                            \
                field_e *ptr_type = (field_e_ptr); UNUSED(ptr_type);                                                                            \
                u32 *ptr_nvalues = (_nvalues_u32_ptr_); UNUSED(ptr_nvalues);                                                                          \
                OPTIONAL_SET(ptr_type, it_ptr->field_type);                                                                    \
                OPTIONAL_SET(ptr_nvalues, num_elements);                                                                       \
                u32 skip = cap_elements * INTERNAL_GET_TYPE_VALUE_SIZE(it_ptr->field_type);                                    \
                MEMFILE_SEEK(&it_ptr->file, payload_start + skip);                                                             \
        }                                                                                                                       \
        col_it_values_result;                                                                                                        \
})

#define COL_IT_VALUES_INFO(_field_e_, _col_it_)                                                                        \
({                                                                                                                     \
        u32 ___nvalues = 0; UNUSED(___nvalues);                                                                       \
        {                                                                                                              \
                MEMFILE_SEEK(&(_col_it_)->file, (_col_it_)->header_begin);                                             \
                ___nvalues = (u32) MEMFILE_READ_UINTVAR_STREAM(NULL, &(_col_it_)->file);                           \
                OPTIONAL_SET((_field_e_), (_col_it_)->field_type);                                                     \
        }                                                                                                              \
        ___nvalues;                                                                                                    \
})

bool col_it_is_null(col_it *it, u32 pos);

bool col_it_is_boolean(col_it *it);
bool col_it_is_u8(col_it *it);
bool col_it_is_u16(col_it *it);
bool col_it_is_u32(col_it *it);
bool col_it_is_u64(col_it *it);
bool col_it_is_i8(col_it *it);
bool col_it_is_i16(col_it *it);
bool col_it_is_i32(col_it *it);
bool col_it_is_i64(col_it *it);
bool col_it_is_float(col_it *it);

const boolean *internal_col_it_boolean_values(u32 *nvalues, col_it *it);
const u8 *internal_col_it_u8_values(u32 *nvalues, col_it *it);
const u16 *internal_col_it_u16_values(u32 *nvalues, col_it *it);
const u32 *internal_col_it_u32_values(u32 *nvalues, col_it *it);
const u64 *internal_col_it_u64_values(u32 *nvalues, col_it *it);
const i8 *internal_col_it_i8_values(u32 *nvalues, col_it *it);
const i16 *internal_col_it_i16_values(u32 *nvalues, col_it *it);
const i32 *internal_col_it_i32_values(u32 *nvalues, col_it *it);
const i64 *internal_col_it_i64_values(u32 *nvalues, col_it *it);
const float *internal_col_it_float_values(u32 *nvalues, col_it *it);

#define COL_IT_BOOLEAN_VALUES(_nvalues_u32_ptr_, _col_it_ptr_it_)                                                             \
        (CAST_COL_VALUES(boolean, _nvalues_u32_ptr_, _col_it_ptr_it_, FIELD_IS_COLUMN_BOOL_OR_SUBTYPE(type)))

#define COL_IT_U8_VALUES(_nvalues_u32_ptr_, _col_it_ptr_it_)                                                          \
        (CAST_COL_VALUES(u8, _nvalues_u32_ptr_, _col_it_ptr_it_, FIELD_IS_COLUMN_U8_OR_SUBTYPE(type)))

#define COL_IT_U16_VALUES(_nvalues_u32_ptr_, _col_it_ptr_it_)                                                         \
        (CAST_COL_VALUES(u16, _nvalues_u32_ptr_, _col_it_ptr_it_, FIELD_IS_COLUMN_U16_OR_SUBTYPE(type)))

#define COL_IT_U32_VALUES(_nvalues_u32_ptr_, _col_it_ptr_it_)                                                         \
        (CAST_COL_VALUES(u32, _nvalues_u32_ptr_, _col_it_ptr_it_, FIELD_IS_COLUMN_U32_OR_SUBTYPE(type)))

#define COL_IT_U64_VALUES(_nvalues_u32_ptr_, _col_it_ptr_it_)                                                         \
        (CAST_COL_VALUES(u64, _nvalues_u32_ptr_, _col_it_ptr_it_, FIELD_IS_COLUMN_U64_OR_SUBTYPE(type)))

#define COL_IT_I8_VALUES(_nvalues_u32_ptr_, _col_it_ptr_it_)                                                          \
        (CAST_COL_VALUES(i8, _nvalues_u32_ptr_, _col_it_ptr_it_, FIELD_IS_COLUMN_I8_OR_SUBTYPE(type)))

#define COL_IT_I16_VALUES(_nvalues_u32_ptr_, _col_it_ptr_it_)                                                         \
        (CAST_COL_VALUES(i16, _nvalues_u32_ptr_, _col_it_ptr_it_, FIELD_IS_COLUMN_I16_OR_SUBTYPE(type)))

#define COL_IT_I32_VALUES(_nvalues_u32_ptr_, _col_it_ptr_it_)                                                         \
        (CAST_COL_VALUES(i32, _nvalues_u32_ptr_, _col_it_ptr_it_, FIELD_IS_COLUMN_I32_OR_SUBTYPE(type)))

#define COL_IT_I64_VALUES(_nvalues_u32_ptr_, _col_it_ptr_it_)                                                         \
        (CAST_COL_VALUES(i64, _nvalues_u32_ptr_, _col_it_ptr_it_, FIELD_IS_COLUMN_I64_OR_SUBTYPE(type)))

#define COL_IT_FLOAT_VALUES(_nvalues_u32_ptr_, _col_it_ptr_it_)                                                       \
        (CAST_COL_VALUES(float, _nvalues_u32_ptr_, _col_it_ptr_it_, FIELD_IS_COLUMN_FLOAT_OR_SUBTYPE(type)))

bool col_it_remove(col_it *it, u32 pos);
bool col_it_is_multiset(col_it *it);
bool col_it_is_sorted(col_it *it);
bool col_it_update_type(col_it *it, list_type_e derivation);
bool col_it_update_set_null(col_it *it, u32 pos);
bool col_it_update_set_true(col_it *it, u32 pos);
bool col_it_update_set_false(col_it *it, u32 pos);
bool col_it_update_set_u8(col_it *it, u32 pos, u8 value);
bool col_it_update_set_u16(col_it *it, u32 pos, u16 value);
bool col_it_update_set_u32(col_it *it, u32 pos, u32 value);
bool col_it_update_set_u64(col_it *it, u32 pos, u64 value);
bool col_it_update_set_i8(col_it *it, u32 pos, i8 value);
bool col_it_update_set_i16(col_it *it, u32 pos, i16 value);
bool col_it_update_set_i32(col_it *it, u32 pos, i32 value);
bool col_it_update_set_i64(col_it *it, u32 pos, i64 value);
bool col_it_update_set_float(col_it *it, u32 pos, float value);
bool col_it_rewind(col_it *it);
bool col_it_print(str_buf *dst, col_it *it);

#ifdef __cplusplus
}
#endif

#endif
