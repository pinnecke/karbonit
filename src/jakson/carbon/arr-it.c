/**
 * Columnar Binary JSON -- Copyright 2019 Marcus Pinnecke
 * This file implements an (read-/write) iterator for (JSON) arrays in carbon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of
 * the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/forwdecl.h>
#include <jakson/std/uintvar/stream.h>
#include <jakson/rec.h>
#include <jakson/carbon/arr-it.h>
#include <jakson/carbon/col-it.h>
#include <jakson/carbon/obj-it.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/mime.h>
#include <jakson/carbon/internal.h>
#include <jakson/carbon/item.h>

#define DEFINE_IN_PLACE_UPDATE_FUNCTION(type_name, field_type)                                                         \
bool internal_arr_it_update_##type_name(arr_it *it, type_name value)                \
{                                                                                                                      \
        offset_t datum = 0;                                                                                                \
        if (LIKELY(it->field.type == field_type)) {                                                    \
                MEMFILE_SAVE_POSITION(&it->file);                                                                   \
                internal_arr_it_offset(&datum, it);                                                                 \
                MEMFILE_SEEK(&it->file, datum + sizeof(u8));                                                        \
                MEMFILE_WRITE(&it->file, &value, sizeof(type_name));                                                \
                MEMFILE_RESTORE_POSITION(&it->file);                                                                \
                return true;                                                                                           \
        } else {                                                                                                       \
                ERROR(ERR_TYPEMISMATCH, NULL);                                                                 \
                return false;                                                                                          \
        }                                                                                                              \
}

DEFINE_IN_PLACE_UPDATE_FUNCTION(u8, FIELD_NUMBER_U8)

DEFINE_IN_PLACE_UPDATE_FUNCTION(u16, FIELD_NUMBER_U16)

DEFINE_IN_PLACE_UPDATE_FUNCTION(u32, FIELD_NUMBER_U32)

DEFINE_IN_PLACE_UPDATE_FUNCTION(u64, FIELD_NUMBER_U64)

DEFINE_IN_PLACE_UPDATE_FUNCTION(i8, FIELD_NUMBER_I8)

DEFINE_IN_PLACE_UPDATE_FUNCTION(i16, FIELD_NUMBER_I16)

DEFINE_IN_PLACE_UPDATE_FUNCTION(i32, FIELD_NUMBER_I32)

DEFINE_IN_PLACE_UPDATE_FUNCTION(i64, FIELD_NUMBER_I64)

DEFINE_IN_PLACE_UPDATE_FUNCTION(float, FIELD_NUMBER_FLOAT)

static inline bool update_in_place_constant(arr_it *it, constant_e constant)
{
        MEMFILE_SAVE_POSITION(&it->file);

        if (FIELD_IS_CONSTANT(it->field.type)) {
                u8 value;
                switch (constant) {
                        case CONST_TRUE:
                                value = FIELD_TRUE;
                                break;
                        case CONST_FALSE:
                                value = FIELD_FALSE;
                                break;
                        case CONST_NULL:
                                value = FIELD_NULL;
                                break;
                        default: ERROR(ERR_INTERNALERR, NULL);
                                break;
                }
                offset_t datum = 0;
                internal_arr_it_offset(&datum, it);
                MEMFILE_SEEK(&it->file, datum);
                MEMFILE_WRITE(&it->file, &value, sizeof(u8));
        } else {
                insert ins;
                internal_arr_it_remove(it);
                arr_it_insert_begin(&ins, it);

                switch (constant) {
                        case CONST_TRUE:
                                insert_true(&ins);
                                break;
                        case CONST_FALSE:
                                insert_false(&ins);
                                break;
                        case CONST_NULL:
                                insert_null(&ins);
                                break;
                        default: ERROR(ERR_INTERNALERR, NULL);
                                break;
                }

                arr_it_insert_end(&ins);
        }

        MEMFILE_RESTORE_POSITION(&it->file);
        return true;
}

inline bool internal_arr_it_update_true(arr_it *it)
{
        return update_in_place_constant(it, CONST_TRUE);
}

inline bool internal_arr_it_update_false(arr_it *it)
{
        return update_in_place_constant(it, CONST_FALSE);
}

inline bool internal_arr_it_update_null(arr_it *it)
{
        return update_in_place_constant(it, CONST_NULL);
}

inline bool internal_arr_it_update_string(arr_it *it, const char *str)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(str)
        return ERROR(ERR_NOTIMPLEMENTED, NULL);
}

inline bool internal_arr_it_update_binary(arr_it *it, const void *base, size_t nbytes, const char *file_ext, const char *type)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(base)
        UNUSED(nbytes)
        UNUSED(file_ext)
        UNUSED(type)
        return ERROR(ERR_NOTIMPLEMENTED, NULL);
}

inline insert *internal_arr_it_update_array_begin(arr_state *state, arr_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        ERROR(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

inline bool internal_arr_it_update_array_end(arr_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return ERROR(ERR_NOTIMPLEMENTED, NULL);
}

inline insert *internal_arr_it_update_column_begin(col_state *state, arr_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        ERROR(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

inline bool internal_arr_it_update_column_end(col_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return ERROR(ERR_NOTIMPLEMENTED, NULL);
}

inline insert *internal_arr_it_update_object_begin(obj_state *state, arr_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        ERROR(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

inline bool internal_arr_it_update_object_end(obj_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return ERROR(ERR_NOTIMPLEMENTED, NULL);
}

inline bool internal_arr_it_update_from_carbon(arr_it *it, const rec *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return ERROR(ERR_NOTIMPLEMENTED, NULL);
}

inline bool internal_arr_it_update_from_array(arr_it *it, const arr_it *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return ERROR(ERR_NOTIMPLEMENTED, NULL);
}

inline bool internal_arr_it_update_from_object(arr_it *it, const obj_it *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return ERROR(ERR_NOTIMPLEMENTED, NULL);
}

inline bool internal_arr_it_update_from_column(arr_it *it, const col_it *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return ERROR(ERR_NOTIMPLEMENTED, NULL);
}

inline bool internal_arr_it_create(arr_it *it, memfile *memfile, offset_t payload_start)
{
        //ZERO_MEMORY(it, sizeof(arr_it));

        it->begin = payload_start;
        it->mod_size = 0;
        it->eof = false;
        it->field_offset = 0;
        it->pos = 0;
        it->last_off = 0;

        MEMFILE_OPEN(&it->file, memfile->memblock, memfile->mode);
        MEMFILE_SEEK__UNSAFE(&it->file, payload_start);

        u8 marker = *(u8*) MEMFILE_READ_UNSAEF(&it->file, 1);

//#ifndef NDEBUG
//        if (!abstract_is_instanceof_array(marker)) {
//            return ERROR(ERR_MARKERMAPPING, "expected array or sub type marker");
//        }
//#endif

        abstract_type_class_e type_class = abstract_get_class(marker);
        it->list_type = abstract_class_to_list_derivable(type_class);

        ARR_IT_REWIND(it);

        return true;
}

inline bool internal_arr_it_copy(arr_it *dst, arr_it *src)
{
        internal_arr_it_create(dst, &src->file, src->begin);
        return true;
}

inline bool internal_arr_it_clone(arr_it *dst, arr_it *src)
{
        MEMFILE_CLONE(&dst->file, &src->file);
        dst->begin = src->begin;
        dst->mod_size = src->mod_size;
        dst->eof = src->eof;
        dst->list_type = src->list_type;
        dst->last_off = src->last_off;
        internal_field_clone(&dst->field, &src->field);
        dst->field_offset = src->field_offset;
        dst->pos = src->pos;
        INTERNAL_ITEM_CREATE_FROM_ARRAY(&dst->item, dst, src->pos, &dst->field, dst->field.type);
        return true;
}

inline bool arr_it_length(u64 *len, arr_it *it)
{
        arr_it dup;
        internal_arr_it_clone(&dup, it);

        u64 num_elem = 0;
        while (arr_it_next(&dup)) {
                num_elem++;
        }
        *len = num_elem;

        return true;
}

inline bool arr_it_is_empty(arr_it *it)
{
        ARR_IT_REWIND(it);
        return arr_it_next(it);
}

inline bool arr_it_has_next(arr_it *it)
{
        arr_it cpy;
        internal_arr_it_clone(&cpy, it);
        bool has_next = arr_it_next(&cpy);
        return has_next;
}

inline bool arr_it_is_unit(arr_it *it)
{
        arr_it cpy;
        internal_arr_it_copy(&cpy, it);
        bool has_next = arr_it_next(&cpy);
        bool ret = false;

        if (has_next) {
                has_next = arr_it_next(&cpy);
                ret = !has_next;
        }

        return ret;
}

inline void internal_arr_it_adjust(arr_it *it)
{
        MEMFILE_SEEK_FROM_HERE(&it->file, it->mod_size);
        it->mod_size = 0;
}

inline item *arr_it_next(arr_it *it)
{
        offset_t begin_off = MEMFILE_TELL(&it->file);
        u8 *raw_begin = MEMFILE_RAW_DATA(&it->file);
        u8 *raw_it = raw_begin;

        uint_fast8_t c;

        /** skip remaining zeros until end of array is reached */
        while ((c = *raw_it) == 0) {
                raw_it++;
        }

        if (c != MARRAY_END) {
                // read array field read fast
                {
                        it->field_offset = begin_off + (raw_it - raw_begin);
                        it->field.type = *(raw_it++);
                }
                // access field data
                {
                        field *field = &it->field;

                        switch (field->type) {
                                case FIELD_NULL:
                                case FIELD_TRUE:
                                case FIELD_FALSE:
                                        /* nothing to do */
                                        break;
                                case FIELD_NUMBER_U8:
                                case FIELD_NUMBER_I8:
                                        field->data = raw_it;
                                        raw_it += sizeof(u8);
                                        break;
                                case FIELD_NUMBER_U16:
                                case FIELD_NUMBER_I16:
                                        field->data = raw_it;
                                        raw_it += sizeof(u16);
                                        break;
                                case FIELD_NUMBER_U32:
                                case FIELD_NUMBER_I32:
                                        field->data = raw_it;
                                        raw_it += sizeof(u32);
                                        break;
                                case FIELD_NUMBER_U64:
                                case FIELD_NUMBER_I64:
                                        field->data = raw_it;
                                        raw_it += sizeof(u64);
                                        break;
                                case FIELD_NUMBER_FLOAT:
                                        field->data = raw_it;
                                        raw_it += sizeof(float);
                                        break;
                                case FIELD_STRING: {
                                        u8 nbytes;
                                        uintvar_stream_t len = (uintvar_stream_t) raw_it;
                                        field->len = UINTVAR_STREAM_READ(&nbytes, len);
                                        raw_it += nbytes;
                                        field->data = raw_it;
                                        raw_it += field->len; /* skip string value */
                                }
                                        break;
                                case FIELD_BINARY: {
                                        u8 nbytes;
                                        /** read mime type with variable-length integer type */
                                        uintvar_stream_t id = (uintvar_stream_t) raw_it;
                                        u64 mime_id = UINTVAR_STREAM_READ(&nbytes, id);
                                        raw_it += nbytes;
                                        field->mime = mime_by_id(mime_id);
                                        field->mime_len = strlen(field->mime);

                                        /** read blob length */
                                        uintvar_stream_t len = (uintvar_stream_t) raw_it;
                                        field->len = UINTVAR_STREAM_READ(&nbytes, len);
                                        raw_it += nbytes;

                                        /** the mem points now to the actual blob data, which is used by the iterator to set the field */
                                        field->data = raw_it;
                                        raw_it += field->len;
                                }
                                        break;
                                case FIELD_BINARY_CUSTOM: {
                                        u8 nbytes;
                                        /** read mime type str_buf */
                                        uintvar_stream_t mlen = (uintvar_stream_t) raw_it;
                                        field->mime_len = UINTVAR_STREAM_READ(&nbytes, mlen);
                                        raw_it += nbytes;

                                        field->mime = (const char *) raw_it;
                                        raw_it += field->mime_len;

                                        /** read blob length */
                                        uintvar_stream_t len = (uintvar_stream_t) raw_it;
                                        field->len = UINTVAR_STREAM_READ(&nbytes, len);
                                        raw_it += nbytes;

                                        /** the mem points now to the actual blob data, which is used by the iterator to set the field */
                                        field->data = raw_it;
                                        raw_it += field->len;
                                }
                                        break;
                                case FIELD_ARRAY_UNSORTED_MULTISET:
                                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                                case FIELD_DERIVED_ARRAY_SORTED_SET: {
                                        arr_it forward_it;
                                        field->data = raw_begin;

                                        internal_arr_it_create(&forward_it, &it->file, begin_off);
                                        internal_arr_it_fast_forward(&forward_it);
                                        raw_it = internal_arr_it_memfile(&forward_it);
                                } break;
                                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U8_SORTED_SET:
                                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U16_SORTED_SET:
                                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U32_SORTED_SET:
                                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U64_SORTED_SET:
                                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I8_SORTED_SET:
                                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I16_SORTED_SET:
                                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I32_SORTED_SET:
                                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I64_SORTED_SET:
                                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                                        col_it forward_it;
                                        field->data = raw_begin;

                                        col_it_create(&forward_it, &it->file, begin_off);
                                        internal_col_it_skip__fast(&forward_it);
                                        raw_it = col_it_memfile_raw(&forward_it);
                                } break;
                                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                                case FIELD_DERIVED_OBJECT_SORTED_MAP: {
                                        obj_it forward_it;
                                        field->data = raw_it;

                                        internal_obj_it_create(&forward_it, &it->file, begin_off);
                                        internal_obj_it_fast_forward(&forward_it);
                                        raw_it = internal_obj_it_memfile(&forward_it);
                                        obj_it_drop(&forward_it);
                                } break;
                                default:
                                        ERROR(ERR_CORRUPTED, NULL);
                                        break;
                        }
                }

                INTERNAL_ITEM_CREATE_FROM_ARRAY(&(it->item), it, it->pos, &(it->field),it->field.type);
                it->pos++;

                it->last_off = begin_off;
                MEMFILE_SEEK__UNSAFE(&it->file, begin_off + (raw_it - raw_begin));

                return &(it->item);
        } else {
                MEMFILE_SEEK__UNSAFE(&it->file, begin_off + (raw_it - raw_begin));
                assert(*MEMFILE_PEEK(&it->file, sizeof(char)) == MARRAY_END);
                it->eof = true;
                return NULL;
        }
}

inline void *internal_arr_it_memfile(arr_it *it)
{
        return MEMFILE_RAW_DATA(&it->file);
}

inline offset_t internal_arr_it_memfilepos(arr_it *it)
{
        if (LIKELY(it != NULL)) {
                return MEMFILE_TELL(&it->file);
        } else {
                ERROR(ERR_NULLPTR, NULL);
                return 0;
        }
}

inline offset_t internal_arr_it_tell(arr_it *it)
{
        return it ? it->field_offset : 0;
}

inline bool internal_arr_it_offset(offset_t *off, arr_it *it)
{
        if (it->last_off) {
                *off = it->last_off;
                return true;
        }
        return false;
}

inline bool internal_arr_it_fast_forward(arr_it *it)
{
        while (arr_it_next(it))
                { }

        assert(*MEMFILE_PEEK(&it->file, sizeof(char)) == MARRAY_END);
        MEMFILE_SKIP(&it->file, sizeof(char));
        return true;
}

inline void arr_it_insert_begin(insert *in, arr_it *it)
{
        internal_insert_create_for_array(in, it);
}

inline void arr_it_insert_end(insert *in)
{
        UNUSED(in)
        /* nothing to do */
}

inline bool internal_arr_it_remove(arr_it *it)
{
        field_e type;
        if (arr_it_field_type(&type, it)) {
                offset_t prev_off = it->last_off;
                MEMFILE_SEEK(&it->file, prev_off);
                if (internal_field_remove(&it->file, type)) {
                        internal_array_refresh(NULL, NULL, it);
                        return true;
                } else {
                        return false;
                }
        } else {
                ERROR(ERR_ILLEGALSTATE, NULL);
                return false;
        }
}

/** Checks if this array is annotated as a multi set abstract type. Returns true if it is is a multi set, and false if
 * it is a set. In case of any ERROR, a failure is returned. */
inline bool arr_it_is_multiset(arr_it *it)
{
        abstract_type_class_e type_class = abstract_list_derivable_to_class(it->list_type);
        return abstract_is_multiset(type_class);
}

/** Checks if this array is annotated as a sorted abstract type. Returns true if this is the case,
 * otherwise false. In case of any ERROR, a failure is returned. */
inline bool arr_it_is_sorted(arr_it *it)
{
        abstract_type_class_e type_class = abstract_list_derivable_to_class(it->list_type);
        return abstract_is_sorted(type_class);
}

inline void arr_it_update_type(arr_it *it, list_type_e derivation)
{
        MEMFILE_SAVE_POSITION(&it->file);
        MEMFILE_SEEK(&it->file, it->begin);

        derived_e derive_marker = abstract_derive_list_to(LIST_ARRAY, derivation);
        abstract_write_derived_type(&it->file, derive_marker);

        MEMFILE_RESTORE_POSITION(&it->file);
}

inline bool arr_it_field_type(field_e *type, arr_it *it)
{
        return internal_field_field_type(type, &it->field);
}