/*
 * pindex - path index for speeding up dot path evaluation and traversals
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_PATH_INDEX_H
#define HAD_PATH_INDEX_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <karbonit/stdinc.h>
#include <karbonit/mem/memfile.h>
#include <karbonit/mem/memblock.h>
#include <karbonit/error.h>
#include <karbonit/rec.h>
#include <karbonit/carbon/find.h>
#include <karbonit/carbon/dot.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------------------------------------------------
//  types
// ---------------------------------------------------------------------------------------------------------------------

typedef struct pindex {
        memblock *memblock;
        memfile memfile;
} pindex;

typedef struct pindex_it {
        rec *doc;
        memfile memfile;
        container_e container;
        u64 pos;
} pindex_it;

typedef enum {
        PINDEX_ROOT, PINDEX_PROP_KEY, PINDEX_ARRAY_INDEX, PINDEX_COLUMN_INDEX
} pindex_node_e;

// ---------------------------------------------------------------------------------------------------------------------
//  construction and deconstruction
// ---------------------------------------------------------------------------------------------------------------------

bool pindex_create(pindex *index, rec *doc);
bool pindex_drop(pindex *index);

// ---------------------------------------------------------------------------------------------------------------------
//  index data access and meta information
// ---------------------------------------------------------------------------------------------------------------------

const void *pindex_raw_data(u64 *size, pindex *index);
bool pindex_commit_hash(u64 *commit_hash, pindex *index);
bool pindex_key_type(key_e *rec_key_type, pindex *index);
bool pindex_key_unsigned_value(u64 *key, pindex *index);
bool pindex_key_signed_value(i64 *key, pindex *index);
const char *pindex_key_string_value(u64 *str_len, pindex *index);
bool pindex_indexes_doc(pindex *index, rec *doc);

// ---------------------------------------------------------------------------------------------------------------------
//  index access and type information
// ---------------------------------------------------------------------------------------------------------------------

bool pindex_it_open(pindex_it *it, pindex *index, rec *doc);
bool pindex_it_type(container_e *type, pindex_it *it);

// ---------------------------------------------------------------------------------------------------------------------
//  array and column container functions
// ---------------------------------------------------------------------------------------------------------------------

bool pindex_it_list_length(u64 *key_len, pindex_it *it);
bool pindex_it_list_goto(u64 pos, pindex_it *it);
bool pindex_it_list_pos(u64 *pos, pindex_it *it);
bool pindex_it_list_can_enter(pindex_it *it);
bool pindex_it_list_enter(pindex_it *it);

// ---------------------------------------------------------------------------------------------------------------------
//  object container functions
// ---------------------------------------------------------------------------------------------------------------------

bool pindex_it_obj_num_props(u64 *num_props, pindex_it *it);
bool pindex_it_obj_goto(const char *key_name, pindex_it *it);
const char *pindex_it_key_name(u64 *name_len, pindex_it *it);
bool pindex_it_obj_can_enter(pindex_it *it);
bool pindex_it_obj_enter(pindex_it *it);

// ---------------------------------------------------------------------------------------------------------------------
//  field access
// ---------------------------------------------------------------------------------------------------------------------

bool pindex_type(field_e *type, pindex_it *it);
bool pindex_it_field_u8_value(u8 *value, pindex_it *it);
bool pindex_it_field_u16_value(u16 *value, pindex_it *it);
bool pindex_it_field_u32_value(u32 *value, pindex_it *it);
bool pindex_it_field_u64_value(u64 *value, pindex_it *it);
bool pindex_it_field_i8_value(i8 *value, pindex_it *it);
bool pindex_it_field_i16_value(i16 *value, pindex_it *it);
bool pindex_it_field_i32_value(i32 *value, pindex_it *it);
bool pindex_it_field_i64_value(i64 *value, pindex_it *it);
bool pindex_it_field_float_value(bool *is_null_in, float *value, pindex_it *it);
bool pindex_it_field_signed_value(bool *is_null_in, i64 *value, pindex_it *it);
bool pindex_it_field_unsigned_value(bool *is_null_in, u64 *value, pindex_it *it);
const char *pindex_it_field_string_value(u64 *strlen, pindex_it *it);
bool pindex_it_field_binary_value(binary_field *out, arr_it *it);
bool pindex_it_field_array_value(arr_it *it_out, pindex_it *it_in);
bool pindex_it_field_object_value(obj_it *it_out, pindex_it *it_in);
bool pindex_it_field_column_value(col_it *it_out, pindex_it *it_in);

// ---------------------------------------------------------------------------------------------------------------------
//  diagnostics
// ---------------------------------------------------------------------------------------------------------------------

bool pindex_hexdump(FILE *file, pindex *index);
void pindex_to_record(rec *doc, pindex *index);
const char *pindex_to_str(str_buf *str, pindex *index);
bool pindex_print(FILE *file, pindex *index);

#ifdef __cplusplus
}
#endif

#endif
