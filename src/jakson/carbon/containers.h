/*
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_CONTAINERS_H
#define HAD_CONTAINERS_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/carbon/markers.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------------------------------------------------
//  container types (lists/maps)
// ---------------------------------------------------------------------------------------------------------------------

typedef enum container {
    OBJECT, ARRAY, COLUMN
} container_e;

// ---------------------------------------------------------------------------------------------------------------------
//  implementation types (and marker) for list/map container types
// ---------------------------------------------------------------------------------------------------------------------

typedef enum sub_type {
    CARBON_CONTAINER_OBJECT = CARBON_MOBJECT_BEGIN,
    CARBON_CONTAINER_ARRAY = CARBON_MARRAY_BEGIN,
    CARBON_CONTAINER_COLUMN_U8 = CARBON_MCOLUMN_U8,
    CARBON_CONTAINER_COLUMN_U16 = CARBON_MCOLUMN_U16,
    CARBON_CONTAINER_COLUMN_U32 = CARBON_MCOLUMN_U32,
    CARBON_CONTAINER_COLUMN_U64 = CARBON_MCOLUMN_U64,
    CARBON_CONTAINER_COLUMN_I8 = CARBON_MCOLUMN_I8,
    CARBON_CONTAINER_COLUMN_I16 = CARBON_MCOLUMN_I16,
    CARBON_CONTAINER_COLUMN_I32 = CARBON_MCOLUMN_I32,
    CARBON_CONTAINER_COLUMN_I64 = CARBON_MCOLUMN_I64,
    CARBON_CONTAINER_COLUMN_BOOLEAN = CARBON_MCOLUMN_BOOLEAN,
    CARBON_CONTAINER_COLUMN_FLOAT = CARBON_MCOLUMN_FLOAT
} sub_type_e;

// ---------------------------------------------------------------------------------------------------------------------
//  implementation types (and marker) for list-only container types
// ---------------------------------------------------------------------------------------------------------------------

typedef enum carbon_list_container {
    LIST_CONTAINER_ARRAY = CARBON_MARRAY_BEGIN,
    LIST_CONTAINER_COLUMN_U8 = CARBON_MCOLUMN_U8,
    LIST_CONTAINER_COLUMN_U16 = CARBON_MCOLUMN_U16,
    LIST_CONTAINER_COLUMN_U32 = CARBON_MCOLUMN_U32,
    LIST_CONTAINER_COLUMN_U64 = CARBON_MCOLUMN_U64,
    LIST_CONTAINER_COLUMN_I8 = CARBON_MCOLUMN_I8,
    LIST_CONTAINER_COLUMN_I16 = CARBON_MCOLUMN_I16,
    LIST_CONTAINER_COLUMN_I32 = CARBON_MCOLUMN_I32,
    LIST_CONTAINER_COLUMN_I64 = CARBON_MCOLUMN_I64,
    LIST_CONTAINER_COLUMN_BOOLEAN = CARBON_MCOLUMN_BOOLEAN,
    LIST_CONTAINER_COLUMN_FLOAT = CARBON_MCOLUMN_FLOAT
} carbon_list_container_e;

bool carbon_list_container_by_column_type(carbon_list_container_e *out, int field_type);

#ifdef __cplusplus
}
#endif

#endif