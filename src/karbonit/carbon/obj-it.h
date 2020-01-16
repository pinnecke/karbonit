/*
 * obj_it - object container iterator
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_OBJ_IT_H
#define HAD_OBJ_IT_H

#include <karbonit/stdinc.h>
#include <karbonit/error.h>
#include <karbonit/mem/memfile.h>
#include <karbonit/carbon/field.h>
#include <karbonit/carbon/arr-it.h>
#include <karbonit/carbon/prop.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct obj_it {
        memfile file;
        offset_t content_begin, begin;
        bool eof;
        u64 pos;
        offset_t last_off;
        struct {
                struct {
                        offset_t start;
                        const char *name;
                        u64 name_len;
                } key;
                struct {
                        offset_t start;
                        field data;
                } value;
        } field;
        map_type_e type;
        prop prop;
        i64 mod_size; /** in case of modifications, the number of bytes that are added resp. removed */
} obj_it;

// ---------------------------------------------------------------------------------------------------------------------
//  public interface
// ---------------------------------------------------------------------------------------------------------------------

bool obj_it_rewind(obj_it *it);
prop *obj_it_next(obj_it *it);
bool obj_it_has_next(obj_it *it);
bool obj_it_prev(obj_it *it);

u64 obj_it_length(obj_it *it);

bool obj_it_is_multimap(obj_it *it);
bool obj_it_is_sorted(obj_it *it);
void obj_it_update_type(obj_it *it, map_type_e derivation);

// ---------------------------------------------------------------------------------------------------------------------
//  for internal usage
// ---------------------------------------------------------------------------------------------------------------------

bool internal_obj_it_create(obj_it *it, memfile *memfile, offset_t payload_start);
bool internal_obj_it_copy(obj_it *dst, obj_it *src);
bool internal_obj_it_clone(obj_it *dst, obj_it *src);

bool internal_obj_it_fast_forward(obj_it *it);

void *internal_obj_it_memfile(obj_it *it);
offset_t internal_obj_it_memfile_pos(obj_it *it);
bool internal_obj_it_tell(offset_t *key_off, offset_t *value_off, obj_it *it);

string_field internal_obj_it_prop_name(obj_it *it);
bool internal_obj_it_remove(obj_it *it);
bool internal_obj_it_prop_type(field_e *type, obj_it *it);

bool internal_obj_it_insert_begin(insert *in, obj_it *it);
void internal_obj_it_insert_end(insert *in);

void internal_obj_it_adjust(obj_it *it);

bool internal_obj_it_update_name(obj_it *it, const char *key);
bool internal_obj_it_update_u8(obj_it *it, u8 value);
bool internal_obj_it_update_u16(obj_it *it, u16 value);
bool internal_obj_it_update_u32(obj_it *it, u32 value);
bool internal_obj_it_update_u64(obj_it *it, u64 value);
bool internal_obj_it_update_i8(obj_it *it, i8 value);
bool internal_obj_it_update_i16(obj_it *it, i16 value);
bool internal_obj_it_update_i32(obj_it *it, i32 value);
bool internal_obj_it_update_i64(obj_it *it, i64 value);
bool internal_obj_it_update_float(obj_it *it, float value);
bool internal_obj_it_update_true(obj_it *it);
bool internal_obj_it_update_false(obj_it *it);
bool internal_obj_it_update_null(obj_it *it);
bool internal_obj_it_update_string(obj_it *it, const char *str);
bool internal_obj_it_update_binary(obj_it *it, const void *value, size_t nbytes, const char *file_ext, const char *user_type);
insert *internal_obj_it_update_array_begin(arr_state *state, obj_it *it);
bool internal_obj_it_update_array_end(arr_state *state);

insert *internal_obj_it_update_column_begin(col_state *state, obj_it *it);
bool internal_obj_it_update_column_end(col_state *state);

insert *internal_obj_it_update_object_begin(obj_state *state, obj_it *it);
bool internal_obj_it_update_object_end(obj_state *state);

bool internal_obj_it_update_from_carbon(obj_it *it, const rec *src);
bool internal_obj_it_update_from_array(obj_it *it, const obj_it *src);
bool internal_obj_it_update_from_object(obj_it *it, const obj_it *src);
bool internal_obj_it_update_from_column(obj_it *it, const col_it *src);

#ifdef __cplusplus
}
#endif

#endif
