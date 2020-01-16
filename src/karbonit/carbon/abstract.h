/*
 * abstract - abstract container implementations
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_ABSTRACT_H
#define HAD_ABSTRACT_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <karbonit/stdinc.h>
#include <karbonit/types.h>
#include <karbonit/carbon/markers.h>
#include <karbonit/carbon/container.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------------------------------------------------
//  abstract type class (base or derived)
// ---------------------------------------------------------------------------------------------------------------------

/** derivation type of any abstract type */
typedef enum abstract {
        /** Does not need further treatment to guarantee properties (unsorted, and not duplicate-free) */
        ABSTRACT_BASE,
        /** particular abstract type with further properties (such as uniqueness of contained elements), enabling the
        * application to check certain promises and guarantees */
        ABSTRACT_DERIVED,
        ABSTRACT_ERR
} abstract_e;



#define __abstract_is_instanceof(marker, T)                             \
        (abstract_get_container_subtype((marker)) == (T))

#define abstract_type(marker)                                                                                           \
({                                                                                          \
        derived_e abstract_type_derived = abstract_get_derived_type(marker);                                                                                          \
        abstract_e abstract_type_ret;                                                                                          \
        switch (abstract_type_derived) {                                                                                          \
                case UNSORTED_MULTIMAP:                                                                                          \
                case UNSORTED_MULTISET_ARRAY:                                                                                          \
                case UNSORTED_MULTISET_COL_U8:                                                                                          \
                case UNSORTED_MULTISET_COL_U16:                                                                                          \
                case UNSORTED_MULTISET_COL_U32:                                                                                          \
                case UNSORTED_MULTISET_COL_U64:                                                                                          \
                case UNSORTED_MULTISET_COL_I8:                                                                                          \
                case UNSORTED_MULTISET_COL_I16:                                                                                          \
                case UNSORTED_MULTISET_COL_I32:                                                                                          \
                case UNSORTED_MULTISET_COL_I64:                                                                                          \
                case UNSORTED_MULTISET_COL_FLOAT:                                                                                          \
                case UNSORTED_MULTISET_COL_BOOLEAN:                                                                                          \
                        abstract_type_ret = ABSTRACT_BASE;                                                                                          \
                        break;                                                                                          \
                case SORTED_MULTIMAP:                                                                                          \
                case UNSORTED_MAP:                                                                                          \
                case SORTED_MAP:                                                                                          \
                case SORTED_MULTISET_ARRAY:                                                                                          \
                case UNSORTED_SET_ARRAY:                                                                                          \
                case SORTED_SET_ARRAY:                                                                                          \
                case SORTED_MULTISET_COL_U8:                                                                                          \
                case UNSORTED_SET_COL_U8:                                                                                          \
                case SORTED_SET_COL_U8:                                                                                          \
                case SORTED_MULTISET_COL_U16:                                                                                          \
                case UNSORTED_SET_COL_U16:                                                                                          \
                case SORTED_SET_COL_U16:                                                                                          \
                case SORTED_MULTISET_COL_U32:                                                                                          \
                case UNSORTED_SET_COL_U32:                                                                                          \
                case SORTED_SET_COL_U32:                                                                                          \
                case SORTED_MULTISET_COL_U64:                                                                                          \
                case UNSORTED_SET_COL_U64:                                                                                          \
                case SORTED_SET_COL_U64:                                                                                          \
                case SORTED_MULTISET_COL_I8:                                                                                          \
                case UNSORTED_SET_COL_I8:                                                                                          \
                case SORTED_SET_COL_I8:                                                                                          \
                case SORTED_MULTISET_COL_I16:                                                                                          \
                case UNSORTED_SET_COL_I16:                                                                                          \
                case SORTED_SET_COL_I16:                                                                                          \
                case SORTED_MULTISET_COL_I32:                                                                                          \
                case UNSORTED_SET_COL_I32:                                                                                          \
                case SORTED_SET_COL_I32:                                                                                          \
                case SORTED_MULTISET_COL_I64:                                                                                          \
                case UNSORTED_SET_COL_I64:                                                                                          \
                case SORTED_SET_COL_I64:                                                                                          \
                case SORTED_MULTISET_COL_FLOAT:                                                                                          \
                case UNSORTED_SET_COL_FLOAT:                                                                                          \
                case SORTED_SET_COL_FLOAT:                                                                                          \
                case SORTED_MULTISET_COL_BOOLEAN:                                                                                          \
                case UNSORTED_SET_COL_BOOLEAN:                                                                                          \
                case SORTED_SET_COL_BOOLEAN:                                                                                          \
                        abstract_type_ret = ABSTRACT_DERIVED;                                                                                          \
                        break;                                                                                          \
                default: PANIC(ERR_MARKERMAPPING);                                                                                          \
                        abstract_type_ret = ABSTRACT_ERR;                                                                                          \
                        break;                                                                  \
        }                                                                                          \
        abstract_type_ret;                                                                                          \
})

#define abstract_is_multiset(type)                                                                                     \
        (type == TYPE_UNSORTED_MULTISET || type == TYPE_SORTED_MULTISET)

#define abstract_is_base(marker)                                                        \
({                                                        \
        abstract_e abstract_is_type = abstract_type(marker);                                                        \
        (abstract_is_type == ABSTRACT_BASE);                                                        \
})



#define abstract_is_derived(marker)                                                             \
        (!abstract_is_base(marker));


// ---------------------------------------------------------------------------------------------------------------------
//  abstract type (multiset, set, sorted or unsorted)
// ---------------------------------------------------------------------------------------------------------------------

/** class of an abstract type independent of a particular abstract derived container */
typedef enum abstract_type_class {
        /** abstract base types */
        TYPE_UNSORTED_MULTISET,     /** element type: values, distinct elements: no, sorted: no */
        TYPE_UNSORTED_MULTIMAP,     /** element type: pairs, distinct elements: no, sorted: no */

        /** derived abstract types */
        TYPE_SORTED_MULTISET,       /** element type: values, distinct elements: no, sorted: yes */
        TYPE_UNSORTED_SET,          /** element type: values, distinct elements: yes, sorted: no */
        TYPE_SORTED_SET,            /** element type: values, distinct elements: yes, sorted: yes */
        TYPE_SORTED_MAP,            /** element type: pairs, distinct elements: yes, sorted: yes */
        TYPE_SORTED_MULTIMAP,       /** element type: pairs, distinct elements: no, sorted: yes */
        TYPE_UNSORTED_MAP,          /** element type: pairs, distinct elements: yes, sorted: no */

        TYPE_ERR
} abstract_type_class_e;

#define abstract_get_class(marker)                                                                              \
({                                                                              \
        abstract_type_class_e abstract_get_class_ret;                                                                              \
        derived_e derived = abstract_get_derived_type(marker);                                                                              \
        switch (derived) {                                                                              \
                case SORTED_MAP:                                                                              \
                        abstract_get_class_ret = TYPE_SORTED_MAP;                                                                              \
                        break;                                                                              \
                case SORTED_MULTIMAP:                                                                              \
                        abstract_get_class_ret = TYPE_SORTED_MULTIMAP;                                                                              \
                        break;                                                                              \
                case SORTED_MULTISET_ARRAY:                                                                              \
                case SORTED_MULTISET_COL_BOOLEAN:                                                                              \
                case SORTED_MULTISET_COL_FLOAT:                                                                              \
                case SORTED_MULTISET_COL_I16:                                                                              \
                case SORTED_MULTISET_COL_I32:                                                                              \
                case SORTED_MULTISET_COL_I64:                                                                              \
                case SORTED_MULTISET_COL_I8:                                                                              \
                case SORTED_MULTISET_COL_U16:                                                                              \
                case SORTED_MULTISET_COL_U32:                                                                              \
                case SORTED_MULTISET_COL_U64:                                                                              \
                case SORTED_MULTISET_COL_U8:                                                                              \
                        abstract_get_class_ret = TYPE_SORTED_MULTISET;                                                                              \
                        break;                                                                              \
                case SORTED_SET_ARRAY:                                                                              \
                case SORTED_SET_COL_BOOLEAN:                                                                              \
                case SORTED_SET_COL_FLOAT:                                                                              \
                case SORTED_SET_COL_I16:                                                                              \
                case SORTED_SET_COL_I32:                                                                              \
                case SORTED_SET_COL_I64:                                                                              \
                case SORTED_SET_COL_I8:                                                                              \
                case SORTED_SET_COL_U16:                                                                              \
                case SORTED_SET_COL_U32:                                                                              \
                case SORTED_SET_COL_U64:                                                                              \
                case SORTED_SET_COL_U8:                                                                              \
                        abstract_get_class_ret = TYPE_SORTED_SET;                                                                              \
                        break;                                                                              \
                case UNSORTED_MAP:                                                                              \
                        abstract_get_class_ret = TYPE_UNSORTED_MAP;                                                                              \
                        break;                                                                              \
                case UNSORTED_MULTIMAP:                                                                              \
                        abstract_get_class_ret = TYPE_UNSORTED_MULTIMAP;                                                                              \
                        break;                                                                              \
                case UNSORTED_MULTISET_ARRAY:                                                                              \
                case UNSORTED_MULTISET_COL_BOOLEAN:                                                                              \
                case UNSORTED_MULTISET_COL_FLOAT:                                                                              \
                case UNSORTED_MULTISET_COL_I16:                                                                              \
                case UNSORTED_MULTISET_COL_I32:                                                                              \
                case UNSORTED_MULTISET_COL_I64:                                                                              \
                case UNSORTED_MULTISET_COL_I8:                                                                              \
                case UNSORTED_MULTISET_COL_U16:                                                                              \
                case UNSORTED_MULTISET_COL_U32:                                                                              \
                case UNSORTED_MULTISET_COL_U64:                                                                              \
                case UNSORTED_MULTISET_COL_U8:                                                                              \
                        abstract_get_class_ret = TYPE_UNSORTED_MULTISET;                                                                              \
                        break;                                                                              \
                case UNSORTED_SET_ARRAY:                                                                              \
                case UNSORTED_SET_COL_BOOLEAN:                                                                              \
                case UNSORTED_SET_COL_FLOAT:                                                                              \
                case UNSORTED_SET_COL_I16:                                                                              \
                case UNSORTED_SET_COL_I32:                                                                              \
                case UNSORTED_SET_COL_I64:                                                                              \
                case UNSORTED_SET_COL_I8:                                                                              \
                case UNSORTED_SET_COL_U16:                                                                              \
                case UNSORTED_SET_COL_U32:                                                                              \
                case UNSORTED_SET_COL_U64:                                                                              \
                case UNSORTED_SET_COL_U8:                                                                              \
                        abstract_get_class_ret = TYPE_UNSORTED_SET;                                                                              \
                        break;                                                                              \
                default: PANIC(ERR_MARKERMAPPING);                                                                              \
        }                                                                              \
        abstract_get_class_ret;                                 \
})

#define abstract_is_set(type)                                           \
        (type == TYPE_UNSORTED_SET || type == TYPE_SORTED_SET)


#define abstract_is_multimap(type)                                           \
        (type == TYPE_UNSORTED_MULTIMAP || type == TYPE_SORTED_MULTIMAP)

#define abstract_is_map(type)                                           \
        (type == TYPE_SORTED_MAP || type == TYPE_UNSORTED_MAP)


#define abstract_is_sorted(type)                                           \
        (type == TYPE_SORTED_MULTISET || type == TYPE_SORTED_SET ||                                            \
         type == TYPE_SORTED_MAP || type == TYPE_SORTED_MULTIMAP)

#define abstract_is_distinct(type)                                                                                     \
        (type == TYPE_UNSORTED_SET || type == TYPE_SORTED_SET || type == TYPE_SORTED_MAP || type == TYPE_UNSORTED_MAP)

// ---------------------------------------------------------------------------------------------------------------------
//  derived type (actual abstract type and which container is used)
// ---------------------------------------------------------------------------------------------------------------------

/** particular abstract derived type container (with marker) */
typedef enum {
        /** abstract types for object containers */
        UNSORTED_MULTIMAP = MUNSORTED_MULTIMAP,
        SORTED_MULTIMAP = MSORTED_MULTIMAP,
        UNSORTED_MAP = MUNSORTED_MAP,
        SORTED_MAP = MSORTED_MAP,

        /** abstract types for array containers */
        UNSORTED_MULTISET_ARRAY = MUNSORTED_MULTISET_ARR,
        SORTED_MULTISET_ARRAY = MSORTED_MULTISET_ARR,
        UNSORTED_SET_ARRAY = MUNSORTED_SET_ARR,
        SORTED_SET_ARRAY = MSORTED_SET_ARR,

        /** abstract types for column-u8 containers */
        UNSORTED_MULTISET_COL_U8 = MUNSORTED_MULTISET_U8,
        SORTED_MULTISET_COL_U8 = MSORTED_MULTISET_U8,
        UNSORTED_SET_COL_U8 = MUNSORTED_SET_U8,
        SORTED_SET_COL_U8 = MSORTED_SET_U8,

        /** abstract types for column-u16 containers */
        UNSORTED_MULTISET_COL_U16 = MUNSORTED_MULTISET_U16,
        SORTED_MULTISET_COL_U16 = MSORTED_MULTISET_U16,
        UNSORTED_SET_COL_U16 = MUNSORTED_SET_U16,
        SORTED_SET_COL_U16 = MSORTED_SET_U16,

        /** abstract types for column-u32 containers */
        UNSORTED_MULTISET_COL_U32 = MUNSORTED_MULTISET_U32,
        SORTED_MULTISET_COL_U32 = MSORTED_MULTISET_U32,
        UNSORTED_SET_COL_U32 = MUNSORTED_SET_U32,
        SORTED_SET_COL_U32 = MSORTED_SET_U32,

        /** abstract types for column-u64 containers */
        UNSORTED_MULTISET_COL_U64 = MUNSORTED_MULTISET_U64,
        SORTED_MULTISET_COL_U64 = MSORTED_MULTISET_U64,
        UNSORTED_SET_COL_U64 = MUNSORTED_SET_U64,
        SORTED_SET_COL_U64 = MSORTED_SET_U64,

        /** abstract types for column-i8 containers */
        UNSORTED_MULTISET_COL_I8 = MUNSORTED_MULTISET_I8,
        SORTED_MULTISET_COL_I8 = MSORTED_MULTISET_I8,
        UNSORTED_SET_COL_I8 = MUNSORTED_SET_I8,
        SORTED_SET_COL_I8 = MSORTED_SET_I8,

        /** abstract types for column-i16 containers */
        UNSORTED_MULTISET_COL_I16 = MUNSORTED_MULTISET_I16,
        SORTED_MULTISET_COL_I16 = MSORTED_MULTISET_I16,
        UNSORTED_SET_COL_I16 = MUNSORTED_SET_I16,
        SORTED_SET_COL_I16 = MSORTED_SET_I16,

        /** abstract types for column-i32 containers */
        UNSORTED_MULTISET_COL_I32 = MUNSORTED_MULTISET_I32,
        SORTED_MULTISET_COL_I32 = MSORTED_MULTISET_I32,
        UNSORTED_SET_COL_I32 = MUNSORTED_SET_I32,
        SORTED_SET_COL_I32 = MSORTED_SET_I32,

        /** abstract types for column-i64 containers */
        UNSORTED_MULTISET_COL_I64 = MUNSORTED_MULTISET_I64,
        SORTED_MULTISET_COL_I64 = MSORTED_MULTISET_I64,
        UNSORTED_SET_COL_I64 = MUNSORTED_SET_I64,
        SORTED_SET_COL_I64 = MSORTED_SET_I64,

        /** abstract types for column-float containers */
        UNSORTED_MULTISET_COL_FLOAT = MUNSORTED_MULTISET_FLOAT,
        SORTED_MULTISET_COL_FLOAT = MSORTED_MULTISET_FLOAT,
        UNSORTED_SET_COL_FLOAT = MUNSORTED_SET_FLOAT,
        SORTED_SET_COL_FLOAT = MSORTED_SET_FLOAT,

        /** abstract types for column-boolean containers */
        UNSORTED_MULTISET_COL_BOOLEAN = MUNSORTED_MULTISET_BOOLEAN,
        SORTED_MULTISET_COL_BOOLEAN = MSORTED_MULTISET_BOOLEAN,
        UNSORTED_SET_COL_BOOLEAN = MUNSORTED_SET_BOOLEAN,
        SORTED_SET_COL_BOOLEAN = MSORTED_SET_BOOLEAN,

        DERIVED_ERR
} derived_e;

/** derivable types for a list container (column or array) */
typedef enum list_type
{
        /** the container type that implements the list */
        LIST_UNSORTED_MULTISET,
        /** mark list as sorted */
        LIST_SORTED_MULTISET,
        /** mark list as non-distinct */
        LIST_UNSORTED_SET,
        /** mark list as sorted and distinct */
        LIST_SORTED_SET,
        LIST_ERR
} list_type_e;

/** derivable types for a map container (object) */
typedef enum map_type
{
        /** the container type that implements the map */
        MAP_UNSORTED_MULTIMAP = MUNSORTED_MULTIMAP,
        /** mark map as sorted */
        MAP_SORTED_MULTIMAP = MSORTED_MULTIMAP,
        /** mark map as non-distinct */
        MAP_UNSORTED_MAP = MUNSORTED_MAP,
        /** mark map as sorted and non-distinct */
        MAP_SORTED_MAP = MSORTED_MAP
} map_type_e;

#define abstract_class_to_list_derivable(in)                                                            \
({                                                            \
        list_type_e abstract_class_to_list_derivable_ret;                                                            \
        switch (in) {                                                            \
                case TYPE_UNSORTED_MULTISET:                                                            \
                        abstract_class_to_list_derivable_ret = LIST_UNSORTED_MULTISET;                                                            \
                        break;                                                            \
                case TYPE_SORTED_MULTISET:                                                            \
                        abstract_class_to_list_derivable_ret = LIST_SORTED_MULTISET;                                                            \
                        break;                                                            \
                case TYPE_UNSORTED_SET:                                                            \
                        abstract_class_to_list_derivable_ret = LIST_UNSORTED_SET;                                                            \
                        break;                                                            \
                case TYPE_SORTED_SET:                                                            \
                        abstract_class_to_list_derivable_ret = LIST_SORTED_SET;                                                            \
                        break;                                                            \
                default:                                                            \
                        ERROR(ERR_TYPEMISMATCH, "abstract class type does not encode a list type");                                                            \
                        abstract_class_to_list_derivable_ret = LIST_ERR;                                                            \
                        break;                                                            \
        }                                                            \
        abstract_class_to_list_derivable_ret;                                                            \
})

#define abstract_list_derivable_to_class(in)                                                            \
({                                                            \
        abstract_type_class_e abstract_list_derivable_to_class_ret;                                                            \
        switch (in) {                                                            \
                case LIST_UNSORTED_MULTISET:                                                            \
                        abstract_list_derivable_to_class_ret = TYPE_UNSORTED_MULTISET;                                                            \
                        break;                                                            \
                case LIST_SORTED_MULTISET:                                                            \
                        abstract_list_derivable_to_class_ret = TYPE_SORTED_MULTISET;                                                            \
                        break;                                                            \
                case LIST_UNSORTED_SET:                                                            \
                        abstract_list_derivable_to_class_ret = TYPE_UNSORTED_SET;                                                            \
                        break;                                                            \
                case LIST_SORTED_SET:                                                            \
                        abstract_list_derivable_to_class_ret = TYPE_SORTED_SET;                                                            \
                        break;                                                            \
                default:                                                            \
                        ERROR(ERR_TYPEMISMATCH, "abstract class type does not encode a list type");                                                            \
                        abstract_list_derivable_to_class_ret = TYPE_ERR;                                                            \
                        break;                                                            \
        }                                                            \
        abstract_list_derivable_to_class_ret;                                                            \
})

#define abstract_map_derivable_to_class(in)                                                            \
({                                                            \
        abstract_type_class_e abstract_map_derivable_to_class_ret;                                                            \
        switch (in) {                                                            \
                case MAP_UNSORTED_MULTIMAP:                                                            \
                        abstract_map_derivable_to_class_ret = TYPE_UNSORTED_MULTIMAP;                                                            \
                        break;                                                            \
                case MAP_SORTED_MULTIMAP:                                                            \
                        abstract_map_derivable_to_class_ret = TYPE_SORTED_MULTIMAP;                                                            \
                        break;                                                            \
                case MAP_UNSORTED_MAP:                                                            \
                        abstract_map_derivable_to_class_ret = TYPE_UNSORTED_MAP;                                                            \
                        break;                                                            \
                case MAP_SORTED_MAP:                                                            \
                        abstract_map_derivable_to_class_ret = TYPE_SORTED_MAP;                                                            \
                        break;                                                            \
                default:                                                            \
                        ERROR(ERR_TYPEMISMATCH, "abstract class type does not encode a map type");                                                            \
                        abstract_map_derivable_to_class_ret = TYPE_ERR;                                                            \
                        break;                                                            \
        }                                                            \
        abstract_map_derivable_to_class_ret;                                                            \
})

#define abstract_write_base_type(memfile, type)                                                                        \
{                                                                        \
        sub_type_e abstract_write_base_type_type = type;                                                                        \
        MEMFILE_WRITE((memfile), &abstract_write_base_type_type, sizeof(u8));                                                                        \
}

#define abstract_write_derived_type(memfile, type)                      \
{                                                               \
        derived_e abstract_write_derived_type_type = (type);                                                            \
        MEMFILE_WRITE((memfile), &abstract_write_derived_type_type, sizeof(u8));                  \
}

#define abstract_get_container_subtype(marker)                                                              \
({                                                              \
        sub_type_e abstract_get_container_subtype_ret;                                                              \
        switch (marker) {                                                              \
                /** abstract types for object containers */                                                              \
                case UNSORTED_MULTIMAP:                                                              \
                case SORTED_MULTIMAP:                                                              \
                case UNSORTED_MAP:                                                              \
                case SORTED_MAP:                                                              \
                        abstract_get_container_subtype_ret = CONTAINER_OBJECT;                                                              \
                        break;                                                              \
                        /** abstract types for array containers */                                                              \
                case UNSORTED_MULTISET_ARRAY:                                                              \
                case SORTED_MULTISET_ARRAY:                                                              \
                case UNSORTED_SET_ARRAY:                                                              \
                case SORTED_SET_ARRAY:                                                              \
                        abstract_get_container_subtype_ret = CONTAINER_ARRAY;                                                              \
                        break;                                                              \
                        /** abstract types for column-u8 containers */                                                              \
                case UNSORTED_MULTISET_COL_U8:                                                              \
                case SORTED_MULTISET_COL_U8:                                                              \
                case UNSORTED_SET_COL_U8:                                                              \
                case SORTED_SET_COL_U8:                                                              \
                        abstract_get_container_subtype_ret = CONTAINER_COLUMN_U8;                                                              \
                        break;                                                              \
                        /** abstract types for column-u16 containers */                                                              \
                case UNSORTED_MULTISET_COL_U16:                                                              \
                case SORTED_MULTISET_COL_U16:                                                              \
                case UNSORTED_SET_COL_U16:                                                              \
                case SORTED_SET_COL_U16:                                                              \
                        abstract_get_container_subtype_ret = CONTAINER_COLUMN_U16;                                                              \
                        break;                                                              \
                        /** abstract types for column-u32 containers */                                                              \
                case UNSORTED_MULTISET_COL_U32:                                                              \
                case SORTED_MULTISET_COL_U32:                                                              \
                case UNSORTED_SET_COL_U32:                                                              \
                case SORTED_SET_COL_U32:                                                              \
                        abstract_get_container_subtype_ret = CONTAINER_COLUMN_U32;                                                              \
                        break;                                                              \
                        /** abstract types for column-u64 containers */                                                              \
                case UNSORTED_MULTISET_COL_U64:                                                              \
                case SORTED_MULTISET_COL_U64:                                                              \
                case UNSORTED_SET_COL_U64:                                                              \
                case SORTED_SET_COL_U64:                                                              \
                        abstract_get_container_subtype_ret = CONTAINER_COLUMN_U64;                                                              \
                        break;                                                              \
                        /** abstract types for column-i8 containers */                                                              \
                case UNSORTED_MULTISET_COL_I8:                                                              \
                case SORTED_MULTISET_COL_I8:                                                              \
                case UNSORTED_SET_COL_I8:                                                              \
                case SORTED_SET_COL_I8:                                                              \
                        abstract_get_container_subtype_ret = CONTAINER_COLUMN_I8;                                                              \
                        break;                                                              \
                        /** abstract types for column-i16 containers */                                                              \
                case UNSORTED_MULTISET_COL_I16:                                                              \
                case SORTED_MULTISET_COL_I16:                                                              \
                case UNSORTED_SET_COL_I16:                                                              \
                case SORTED_SET_COL_I16:                                                              \
                        abstract_get_container_subtype_ret = CONTAINER_COLUMN_I16;                                                              \
                        break;                                                              \
                        /** abstract types for column-i32 containers */                                                              \
                case UNSORTED_MULTISET_COL_I32:                                                              \
                case SORTED_MULTISET_COL_I32:                                                              \
                case UNSORTED_SET_COL_I32:                                                              \
                case SORTED_SET_COL_I32:                                                              \
                        abstract_get_container_subtype_ret = CONTAINER_COLUMN_I32;                                                              \
                        break;                                                              \
                        /** abstract types for column-i64 containers */                                                              \
                case UNSORTED_MULTISET_COL_I64:                                                              \
                case SORTED_MULTISET_COL_I64:                                                              \
                case UNSORTED_SET_COL_I64:                                                              \
                case SORTED_SET_COL_I64:                                                              \
                        abstract_get_container_subtype_ret = CONTAINER_COLUMN_I64;                                                              \
                        break;                                                              \
                        /** abstract types for column-float containers */                                                              \
                case UNSORTED_MULTISET_COL_FLOAT:                                                              \
                case SORTED_MULTISET_COL_FLOAT:                                                              \
                case UNSORTED_SET_COL_FLOAT:                                                              \
                case SORTED_SET_COL_FLOAT:                                                              \
                        abstract_get_container_subtype_ret = CONTAINER_COLUMN_FLOAT;                                                              \
                        break;                                                              \
                        /** abstract types for column-boolean containers */                                                              \
                case UNSORTED_MULTISET_COL_BOOLEAN:                                                              \
                case SORTED_MULTISET_COL_BOOLEAN:                                                              \
                case UNSORTED_SET_COL_BOOLEAN:                                                              \
                case SORTED_SET_COL_BOOLEAN:                                                              \
                        abstract_get_container_subtype_ret = CONTAINER_COLUMN_BOOLEAN;                                                              \
                        break;                                                              \
                default:                                                              \
                        ERROR(ERR_MARKERMAPPING, "unknown marker encoding an abstract type");                                                              \
                        abstract_get_container_subtype_ret = CONTAINER_ERR;                                                              \
                        break;                                                              \
        }                                                              \
        abstract_get_container_subtype_ret;                                                              \
})



#define abstract_is_instanceof_object(marker)                                           \
        __abstract_is_instanceof(marker, CONTAINER_OBJECT)

#define abstract_is_instanceof_array(marker)                                           \
        __abstract_is_instanceof(marker, CONTAINER_ARRAY)

#define abstract_is_instanceof_column_u8(marker)                                           \
        __abstract_is_instanceof(marker, CONTAINER_COLUMN_U8)

#define abstract_is_instanceof_column_u16(marker)                                           \
        __abstract_is_instanceof(marker, CONTAINER_COLUMN_U16)

#define abstract_is_instanceof_column_u32(marker)                                           \
        __abstract_is_instanceof(marker, CONTAINER_COLUMN_U32)

#define abstract_is_instanceof_column_u64(marker)                                           \
        __abstract_is_instanceof(marker, CONTAINER_COLUMN_U64)

#define abstract_is_instanceof_column_i8(marker)                                           \
        __abstract_is_instanceof(marker, CONTAINER_COLUMN_I8)

#define abstract_is_instanceof_column_i16(marker)                                           \
        __abstract_is_instanceof(marker, CONTAINER_COLUMN_I16)

#define abstract_is_instanceof_column_i32(marker)                                           \
        __abstract_is_instanceof(marker, CONTAINER_COLUMN_I32)

#define abstract_is_instanceof_column_i64(marker)                                           \
        __abstract_is_instanceof(marker, CONTAINER_COLUMN_I64)

#define abstract_is_instanceof_column_float(marker)                                           \
        __abstract_is_instanceof(marker, CONTAINER_COLUMN_FLOAT)

#define abstract_is_instanceof_column_boolean(marker)                                           \
        __abstract_is_instanceof(marker, CONTAINER_COLUMN_BOOLEAN)

#define abstract_is_instanceof_column(marker)                                           \
        (abstract_is_instanceof_column_u8(marker) ||                                           \
         abstract_is_instanceof_column_u16(marker) ||                                           \
         abstract_is_instanceof_column_u32(marker) ||                                           \
         abstract_is_instanceof_column_u64(marker) ||                                           \
         abstract_is_instanceof_column_i8(marker) ||                                           \
         abstract_is_instanceof_column_i16(marker) ||                                           \
         abstract_is_instanceof_column_i32(marker) ||                                           \
         abstract_is_instanceof_column_i64(marker) ||                                           \
         abstract_is_instanceof_column_float(marker) ||                                           \
         abstract_is_instanceof_column_boolean(marker))

#define abstract_is_instanceof_list(marker)                                                             \
        (abstract_is_instanceof_array(marker) ||                                                                \
         abstract_is_instanceof_column(marker))


#define abstract_derive_list_to(is, should)                                                                \
({                                                                \
        derived_e abstract_derive_list_to_ret;                                                                \
        switch (is) {                                                                \
                case LIST_ARRAY:                                                                \
                        switch (should) {                                                                \
                                case LIST_UNSORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_MULTISET_ARRAY;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_MULTISET_ARRAY;                                                                \
                                        break;                                                                \
                                case LIST_UNSORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_SET_ARRAY;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_SET_ARRAY;                                                                \
                                        break;                                                                \
                                default:                                                                \
                                        abstract_derive_list_to_ret = DERIVED_ERR;                                                                \
                                        ERROR(ERR_INTERNALERR, "unhandled switch case");                                                                \
                                        break;                                                                \
                        } break;                                                                \
                case LIST_COLUMN_U8:                                                                \
                        switch (should) {                                                                \
                                case LIST_UNSORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_MULTISET_COL_U8;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_MULTISET_COL_U8;                                                                \
                                        break;                                                                \
                                case LIST_UNSORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_SET_COL_U8;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_SET_COL_U8;                                                                \
                                        break;                                                                \
                                default:                                                                \
                                        abstract_derive_list_to_ret = DERIVED_ERR;                                                                \
                                        ERROR(ERR_INTERNALERR, "unhandled switch case");                                                                \
                                        break;                                                                \
                        } break;                                                                \
                case LIST_COLUMN_U16:                                                                \
                        switch (should) {                                                                \
                                case LIST_UNSORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_MULTISET_COL_U16;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_MULTISET_COL_U16;                                                                \
                                        break;                                                                \
                                case LIST_UNSORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_SET_COL_U16;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_SET_COL_U16;                                                                \
                                        break;                                                                \
                                default:                                                                \
                                        abstract_derive_list_to_ret = DERIVED_ERR;                                                                \
                                        ERROR(ERR_INTERNALERR, "unhandled switch case");                                                                \
                                        break;                                                                \
                        } break;                                                                \
                case LIST_COLUMN_U32:                                                                \
                        switch (should) {                                                                \
                                case LIST_UNSORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_MULTISET_COL_U32;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_MULTISET_COL_U32;                                                                \
                                        break;                                                                \
                                case LIST_UNSORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_SET_COL_U32;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_SET_COL_U32;                                                                \
                                        break;                                                                \
                                default:                                                                \
                                        abstract_derive_list_to_ret = DERIVED_ERR;                                                                \
                                        ERROR(ERR_INTERNALERR, "unhandled switch case");                                                                \
                                        break;                                                                \
                        } break;                                                                \
                case LIST_COLUMN_U64:                                                                \
                        switch (should) {                                                                \
                                case LIST_UNSORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_MULTISET_COL_U64;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_MULTISET_COL_U64;                                                                \
                                        break;                                                                \
                                case LIST_UNSORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_SET_COL_U64;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_SET_COL_U64;                                                                \
                                        break;                                                                \
                                default:                                                                \
                                        abstract_derive_list_to_ret = DERIVED_ERR;                                                                \
                                        ERROR(ERR_INTERNALERR, "unhandled switch case");                                                                \
                                        break;                                                                \
                        } break;                                                                \
                case LIST_COLUMN_I8:                                                                \
                        switch (should) {                                                                \
                                case LIST_UNSORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_MULTISET_COL_I8;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_MULTISET_COL_I8;                                                                \
                                        break;                                                                \
                                case LIST_UNSORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_SET_COL_I8;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_SET_COL_I8;                                                                \
                                        break;                                                                \
                                default:                                                                \
                                        abstract_derive_list_to_ret = DERIVED_ERR;                                                                \
                                        ERROR(ERR_INTERNALERR, "unhandled switch case");                                                                \
                                        break;                                                                \
                        } break;                                                                \
                case LIST_COLUMN_I16:                                                                \
                        switch (should) {                                                                \
                                case LIST_UNSORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_MULTISET_COL_I16;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_MULTISET_COL_I16;                                                                \
                                        break;                                                                \
                                case LIST_UNSORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_SET_COL_I16;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_SET_COL_I16;                                                                \
                                        break;                                                                \
                                default:                                                                \
                                        abstract_derive_list_to_ret = DERIVED_ERR;                                                                \
                                        ERROR(ERR_INTERNALERR, "unhandled switch case");                                                                \
                                        break;                                                                \
                        } break;                                                                \
                case LIST_COLUMN_I32:                                                                \
                        switch (should) {                                                                \
                                case LIST_UNSORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_MULTISET_COL_I32;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_MULTISET_COL_I32;                                                                \
                                        break;                                                                \
                                case LIST_UNSORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_SET_COL_I32;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_SET_COL_I32;                                                                \
                                        break;                                                                \
                                default:                                                                \
                                        abstract_derive_list_to_ret = DERIVED_ERR;                                                                \
                                        ERROR(ERR_INTERNALERR, "unhandled switch case");                                                                \
                                        break;                                                                \
                        } break;                                                                \
                case LIST_COLUMN_I64:                                                                \
                        switch (should) {                                                                \
                                case LIST_UNSORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_MULTISET_COL_I64;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_MULTISET_COL_I64;                                                                \
                                        break;                                                                \
                                case LIST_UNSORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_SET_COL_I64;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_SET_COL_I64;                                                                \
                                        break;                                                                \
                                default:                                                                \
                                        abstract_derive_list_to_ret = DERIVED_ERR;                                                                \
                                        ERROR(ERR_INTERNALERR, "unhandled switch case");                                                                \
                                        break;                                                                \
                        } break;                                                                \
                case LIST_COLUMN_BOOLEAN:                                                                \
                        switch (should) {                                                                \
                                case LIST_UNSORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_MULTISET_COL_BOOLEAN;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_MULTISET_COL_BOOLEAN;                                                                \
                                        break;                                                                \
                                case LIST_UNSORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_SET_COL_BOOLEAN;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_SET_COL_BOOLEAN;                                                                \
                                        break;                                                                \
                                default:                                                                \
                                        abstract_derive_list_to_ret = DERIVED_ERR;                                                                \
                                        ERROR(ERR_INTERNALERR, "unhandled switch case");                                                                \
                                        break;                                                                \
                        } break;                                                                \
                case LIST_COLUMN_FLOAT:                                                                \
                        switch (should) {                                                                \
                                case LIST_UNSORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_MULTISET_COL_FLOAT;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_MULTISET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_MULTISET_COL_FLOAT;                                                                \
                                        break;                                                                \
                                case LIST_UNSORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = UNSORTED_SET_COL_FLOAT;                                                                \
                                        break;                                                                \
                                case LIST_SORTED_SET:                                                                \
                                        abstract_derive_list_to_ret = SORTED_SET_COL_FLOAT;                                                                \
                                        break;                                                                \
                                default:                                                                \
                                        abstract_derive_list_to_ret = DERIVED_ERR;                                                                \
                                        ERROR(ERR_INTERNALERR, "unhandled switch case");                                                                \
                                        break;                                                                \
                        } break;                                                                \
                default:                                                                \
                        abstract_derive_list_to_ret = DERIVED_ERR;                                                                \
                        ERROR(ERR_INTERNALERR, "unhandled switch case");                                                                \
                        break;                                                                \
        }                                                                \
        abstract_derive_list_to_ret;                                                                \
})                                                                \

#define abstract_derive_map_to(should)                                                                \
({                                                                \
        derived_e abstract_derive_map_to_ret;                                                                \
        switch (should) {                                                                \
                case MAP_UNSORTED_MULTIMAP:                                                                \
                        abstract_derive_map_to_ret = UNSORTED_MULTIMAP;                                                                \
                        break;                                                                \
                case MAP_SORTED_MULTIMAP:                                                                \
                        abstract_derive_map_to_ret = SORTED_MULTIMAP;                                                                \
                        break;                                                                \
                case MAP_UNSORTED_MAP:                                                                \
                        abstract_derive_map_to_ret = UNSORTED_MAP;                                                                \
                        break;                                                                \
                case MAP_SORTED_MAP:                                                                \
                        abstract_derive_map_to_ret = SORTED_MAP;                                                                \
                        break;                                                                \
                default:                                                                \
                        ERROR(ERR_INTERNALERR, "unknown list container type");                                                                \
                        abstract_derive_map_to_ret = DERIVED_ERR;                                                                \
                        break;                                                                \
        }                                                                \
        abstract_derive_map_to_ret;                                                                \
})



#define abstract_get_derived_type(marker)                                                                \
({                                                                \
        if (!(marker == MUNSORTED_MULTIMAP || marker == MSORTED_MULTIMAP || marker == MUNSORTED_MAP ||                                                                \
              marker == MSORTED_MAP || marker == MUNSORTED_MULTISET_ARR ||                                                                \
              marker == MSORTED_MULTISET_ARR || marker == MUNSORTED_SET_ARR ||                                                                \
              marker == MSORTED_SET_ARR || marker == MUNSORTED_MULTISET_U8 ||                                                                \
              marker == MSORTED_MULTISET_U8 || marker == MUNSORTED_SET_U8 ||                                                                \
              marker == MSORTED_SET_U8 || marker == MUNSORTED_MULTISET_U16 ||                                                                \
              marker == MSORTED_MULTISET_U16 || marker == MUNSORTED_SET_U16 ||                                                                \
              marker == MSORTED_SET_U16 || marker == MUNSORTED_MULTISET_U32 ||                                                                \
              marker == MSORTED_MULTISET_U32 || marker == MUNSORTED_SET_U32 ||                                                                \
              marker == MSORTED_SET_U32 || marker == MUNSORTED_MULTISET_U64 ||                                                                \
              marker == MSORTED_MULTISET_U64 || marker == MUNSORTED_SET_U64 ||                                                                \
              marker == MSORTED_SET_U64 || marker == MUNSORTED_MULTISET_I8 ||                                                                \
              marker == MSORTED_MULTISET_I8 || marker == MUNSORTED_SET_I8 ||                                                                \
              marker == MSORTED_SET_I8 || marker == MUNSORTED_MULTISET_I16 ||                                                                \
              marker == MSORTED_MULTISET_I16 || marker == MUNSORTED_SET_I16 ||                                                                \
              marker == MSORTED_SET_I16 || marker == MUNSORTED_MULTISET_I32 ||                                                                \
              marker == MSORTED_MULTISET_I32 || marker == MUNSORTED_SET_I32 ||                                                                \
              marker == MSORTED_SET_I32 || marker == MUNSORTED_MULTISET_I64 ||                                                                \
              marker == MSORTED_MULTISET_I64 || marker == MUNSORTED_SET_I64 ||                                                                \
              marker == MSORTED_SET_I64 || marker == MUNSORTED_MULTISET_FLOAT ||                                                                \
              marker == MSORTED_MULTISET_FLOAT || marker == MUNSORTED_SET_FLOAT ||                                                                \
              marker == MSORTED_SET_FLOAT || marker == MUNSORTED_MULTISET_BOOLEAN ||                                                                \
              marker == MSORTED_MULTISET_BOOLEAN || marker == MUNSORTED_SET_BOOLEAN ||                                                                \
              marker == MSORTED_SET_BOOLEAN)) {                                                                \
                ERROR(ERR_MARKERMAPPING, "unknown marker for abstract derived type");                                                                \
                marker = MERR;                                                                \
        }                                                                \
        (derived_e) marker;                                                                \
})

#ifdef __cplusplus
}
#endif

#endif