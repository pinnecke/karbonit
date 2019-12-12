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

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/carbon/field.h>
#include <jakson/rec.h>

#ifdef __cplusplus
extern "C" {
#endif

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
bool col_it_clone(col_it *dst, col_it *src);
bool col_it_insert(insert *in, col_it *it);
bool col_it_fast_forward(col_it *it);
offset_t col_it_memfilepos(col_it *it);
offset_t col_it_tell(col_it *it, u32 elem_idx);


#define col_it_values(field_e_ptr, u32_ptr_nvalues, col_it_ptr)                                                        \
({                                                                                                                     \
        const void *result = NULL;                                                                                     \
        col_it *it_ptr = (col_it_ptr);                                                                                 \
        memfile_seek(&it->file, it->header_begin);                                                                     \
        u32 num_elements = (u32) memfile_read_uintvar_stream(NULL, &it_ptr->file);                                     \
        u32 cap_elements = (u32) memfile_read_uintvar_stream(NULL, &it_ptr->file);                                     \
        offset_t payload_start = memfile_tell(&it_ptr->file);                                                          \
        result = memfile_peek(&it_ptr->file, sizeof(void));                                                            \
        field_e *ptr_type = (field_e_ptr);                                                                             \
        u32 *ptr_nvalues = (u32_ptr_nvalues);                                                                          \
        OPTIONAL_SET(ptr_type, it_ptr->field_type);                                                                    \
        OPTIONAL_SET(ptr_nvalues, num_elements);                                                                       \
        u32 skip = cap_elements * internal_get_type_value_size(it_ptr->field_type);                                    \
        memfile_seek(&it_ptr->file, payload_start + skip);                                                             \
        result;                                                                                                        \
})

#define col_it_values_info(field_e, col_it)                                                                            \
({                                                                                                                     \
        memfile_seek(&(col_it)->file, (col_it)->header_begin);                                                         \
        u32 nvalues = (u32) memfile_read_uintvar_stream(NULL, &(col_it)->file);                                        \
        OPTIONAL_SET((field_e), (col_it)->field_type);                                                                 \
        nvalues;                                                                                                       \
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

const boolean *col_it_boolean_values(u32 *nvalues, col_it *it);
const u8 *col_it_u8_values(u32 *nvalues, col_it *it);
const u16 *col_it_u16_values(u32 *nvalues, col_it *it);
const u32 *col_it_u32_values(u32 *nvalues, col_it *it);
const u64 *col_it_u64_values(u32 *nvalues, col_it *it);
const i8 *col_it_i8_values(u32 *nvalues, col_it *it);
const i16 *col_it_i16_values(u32 *nvalues, col_it *it);
const i32 *col_it_i32_values(u32 *nvalues, col_it *it);
const i64 *col_it_i64_values(u32 *nvalues, col_it *it);
const float *col_it_float_values(u32 *nvalues, col_it *it);
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
