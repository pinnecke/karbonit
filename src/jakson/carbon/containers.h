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
    CONTAINER_OBJECT = CARBON_MOBJECT_BEGIN,
    CONTAINER_ARRAY = CARBON_MARRAY_BEGIN,
    CONTAINER_COLUMN_U8 = CARBON_MCOLUMN_U8,
    CONTAINER_COLUMN_U16 = CARBON_MCOLUMN_U16,
    CONTAINER_COLUMN_U32 = CARBON_MCOLUMN_U32,
    CONTAINER_COLUMN_U64 = CARBON_MCOLUMN_U64,
    CONTAINER_COLUMN_I8 = CARBON_MCOLUMN_I8,
    CONTAINER_COLUMN_I16 = CARBON_MCOLUMN_I16,
    CONTAINER_COLUMN_I32 = CARBON_MCOLUMN_I32,
    CONTAINER_COLUMN_I64 = CARBON_MCOLUMN_I64,
    CONTAINER_COLUMN_BOOLEAN = CARBON_MCOLUMN_BOOLEAN,
    CONTAINER_COLUMN_FLOAT = CARBON_MCOLUMN_FLOAT
} sub_type_e;

// ---------------------------------------------------------------------------------------------------------------------
//  implementation types (and marker) for list-only container types
// ---------------------------------------------------------------------------------------------------------------------

typedef enum list_container {
    LIST_ARRAY = CARBON_MARRAY_BEGIN,
    LIST_COLUMN_U8 = CARBON_MCOLUMN_U8,
    LIST_COLUMN_U16 = CARBON_MCOLUMN_U16,
    LIST_COLUMN_U32 = CARBON_MCOLUMN_U32,
    LIST_COLUMN_U64 = CARBON_MCOLUMN_U64,
    LIST_COLUMN_I8 = CARBON_MCOLUMN_I8,
    LIST_COLUMN_I16 = CARBON_MCOLUMN_I16,
    LIST_COLUMN_I32 = CARBON_MCOLUMN_I32,
    LIST_COLUMN_I64 = CARBON_MCOLUMN_I64,
    LIST_COLUMN_BOOLEAN = CARBON_MCOLUMN_BOOLEAN,
    LIST_COLUMN_FLOAT = CARBON_MCOLUMN_FLOAT
} list_container_e;

bool list_container_by_column_type(list_container_e *out, int field_type);

#ifdef __cplusplus
}
#endif

#endif