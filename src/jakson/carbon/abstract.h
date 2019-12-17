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

#include <jakson/stdinc.h>
#include <jakson/types.h>
#include <jakson/carbon/markers.h>
#include <jakson/carbon/container.h>

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

#define abstract_type(marker)                                                                                           \
({                                                                                          \
        derived_e abstract_type_derived;                                                                                          \
        abstract_e abstract_type_ret;                                                                                          \
        if (likely(abstract_get_derived_type(&abstract_type_derived, marker))) {                                                                                          \
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
                        default: panic(ERR_MARKERMAPPING);                                                                                          \
                                abstract_type_ret = ABSTRACT_ERR;                                                                                          \
                }                                                                                          \
        } else {                                                                                          \
                panic(ERR_MARKERMAPPING);                                                                                          \
                abstract_type_ret = ABSTRACT_ERR;                                                                                          \
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
        derived_e derived;                                                                              \
        abstract_type_class_e abstract_get_class_ret;                                                                              \
        if (likely(abstract_get_derived_type(&derived, marker))) {                                                                              \
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
                        default: panic(ERR_MARKERMAPPING);                                                                              \
                }                                                                              \
        } else {                                                                              \
                error(ERR_MARKERMAPPING, "");                                                                              \
                abstract_get_class_ret = TYPE_ERR;                                                                              \
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
        SORTED_SET_COL_BOOLEAN = MSORTED_SET_BOOLEAN
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
                        error(ERR_TYPEMISMATCH, "abstract class type does not encode a list type");                                                            \
                        abstract_class_to_list_derivable_ret = LIST_ERR;                                                            \
                        break;                                                            \
        }                                                            \
        abstract_class_to_list_derivable_ret;                                                            \
})

/** Converts a list derivable type to an abstract type class. In case of error, the function fails. */
bool abstract_list_derivable_to_class(abstract_type_class_e *out, list_type_e in);

bool abstract_map_derivable_to_class(abstract_type_class_e *out, map_type_e in);

/** Writes the marker for a particular base type to the actual position in the memory file, and steps
 * the memory file cursor one byte towards the end. */
void abstract_write_base_type(memfile *memfile, sub_type_e type);

/** Writes the marker for the particular derived abstract type to the actual position in the memory file, and
 * steps the memory file cursor one byte towards the end. */
void abstract_write_derived_type(memfile *memfile, derived_e type);

/** Peeks a byte from the memory file and returns the encoded container sub type. This is either an object
 * container, an array container, or and particular column container. In case a derived type is found, the
 * actual container type that implements that derived type is returned. For instance, if '[1]' is read,
 * a column-u8 container type is returned, and if [SOH] is read (which is MSORTED_MULTISET_U8),
 * a column-u8 container type is returned, too. */
bool abstract_get_container_subtype(sub_type_e *type, u8 marker);

/** Peeks a byte as marker from the memory file without moving the memory file cursor and returns true if this
 * marker sets an object container or a derived type of that container type. */
bool abstract_is_instanceof_object(u8 marker);

/** Peeks a byte as marker from the memory file without moving the memory file cursor and returns true if this
 * marker sets an array container or a derived type of that container type. */
bool abstract_is_instanceof_array(u8 marker);

/** Peeks a byte as marker from the memory file without moving the memory file cursor and returns true if this
 * marker sets an column-u8 container or a derived type of that container type. */
bool abstract_is_instanceof_column_u8(u8 marker);

/** Peeks a byte as marker from the memory file without moving the memory file cursor and returns true if this
 * marker sets an column-u16 container or a derived type of that container type. */
bool abstract_is_instanceof_column_u16(u8 marker);

/** Peeks a byte as marker from the memory file without moving the memory file cursor and returns true if this
 * marker sets an column-u32 container or a derived type of that container type. */
bool abstract_is_instanceof_column_u32(u8 marker);

/** Peeks a byte as marker from the memory file without moving the memory file cursor and returns true if this
 * marker sets an column-u64 container or a derived type of that container type. */
bool abstract_is_instanceof_column_u64(u8 marker);

/** Peeks a byte as marker from the memory file without moving the memory file cursor and returns true if this
 * marker sets an column-i8 container or a derived type of that container type. */
bool abstract_is_instanceof_column_i8(u8 marker);

/** Peeks a byte as marker from the memory file without moving the memory file cursor and returns true if this
 * marker sets an column-i16 container or a derived type of that container type. */
bool abstract_is_instanceof_column_i16(u8 marker);

/** Peeks a byte as marker from the memory file without moving the memory file cursor and returns true if this
 * marker sets an column-i32 container or a derived type of that container type. */
bool abstract_is_instanceof_column_i32(u8 marker);

/** Peeks a byte as marker from the memory file without moving the memory file cursor and returns true if this
 * marker sets an column-i64 container or a derived type of that container type. */
bool abstract_is_instanceof_column_i64(u8 marker);

/** Peeks a byte as marker from the memory file without moving the memory file cursor and returns true if this
 * marker sets an column-float container or a derived type of that container type. */
bool abstract_is_instanceof_column_float(u8 marker);

/** Peeks a byte as marker from the memory file without moving the memory file cursor and returns true if this
 * marker sets an column-boolean container or a derived type of that container type. */
bool abstract_is_instanceof_column_boolean(u8 marker);

/** Peeks a byte as marker from the memory file without moving the memory file cursor and returns true if this
 * marker sets a type of column container (u8, u16,...) or a derived type of that container type. */
bool abstract_is_instanceof_column(u8 marker);

/** Peeks a byte as marker from the memory file without moving the memory file cursor and returns true if this
 * marker sets a type of column container or array container or a derived type of that container type. */
bool abstract_is_instanceof_list(u8 marker);

/** Returns the concrete derived type <code>concrete</code> (e.g., SORTED_SET_COL_BOOLEAN) for a
 * given list type <code>is</code> (e.g., LIST_COLUMN_BOOLEAN) when deriving that
 * list type to a particular abstract type <code>should</code> (e.g., SORTED_SET) */
bool abstract_derive_list_to(derived_e *concrete, list_container_e is,
                                         list_type_e should);

/** Returns the concrete derived type <code>concrete</code> (e.g., MAP_SORTED_MULTIMAP) for a
 * given map when deriving that map type to a particular abstract type <code>should</code>
 * (e.g., SORTED_MULTIMAP) */
bool abstract_derive_map_to(derived_e *concrete, map_type_e should);

/** Reads a marker from the memory file, and returns the particular abstract derived container (including
 * the marker) without moving the memory files cursor. In case of an failure (e.g., the read marker is not known),
 * the function returns an err. */
bool abstract_get_derived_type(derived_e *type, u8 marker);

#ifdef __cplusplus
}
#endif

#endif