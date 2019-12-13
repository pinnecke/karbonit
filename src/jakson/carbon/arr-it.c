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
        if (likely(it->field.type == field_type)) {                                                    \
                MEMFILE_SAVE_POSITION(&it->file);                                                                   \
                internal_arr_it_offset(&datum, it);                                                                 \
                MEMFILE_SEEK(&it->file, datum + sizeof(u8));                                                        \
                MEMFILE_WRITE(&it->file, &value, sizeof(type_name));                                                \
                MEMFILE_RESTORE_POSITION(&it->file);                                                                \
                return true;                                                                                           \
        } else {                                                                                                       \
                error(ERR_TYPEMISMATCH, NULL);                                                                 \
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

static bool update_in_place_constant(arr_it *it, constant_e constant)
{
        MEMFILE_SAVE_POSITION(&it->file);

        if (field_is_constant(it->field.type)) {
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
                        default: error(ERR_INTERNALERR, NULL);
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
                        default: error(ERR_INTERNALERR, NULL);
                                break;
                }

                arr_it_insert_end(&ins);
        }

        MEMFILE_RESTORE_POSITION(&it->file);
        return true;
}

bool internal_arr_it_update_true(arr_it *it)
{
        return update_in_place_constant(it, CONST_TRUE);
}

bool internal_arr_it_update_false(arr_it *it)
{
        return update_in_place_constant(it, CONST_FALSE);
}

bool internal_arr_it_update_null(arr_it *it)
{
        return update_in_place_constant(it, CONST_NULL);
}

bool internal_arr_it_update_string(arr_it *it, const char *str)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(str)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_arr_it_update_binary(arr_it *it, const void *base, size_t nbytes, const char *file_ext, const char *type)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(base)
        UNUSED(nbytes)
        UNUSED(file_ext)
        UNUSED(type)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

insert *internal_arr_it_update_array_begin(arr_state *state, arr_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool internal_arr_it_update_array_end(arr_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

insert *internal_arr_it_update_column_begin(col_state *state, arr_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool internal_arr_it_update_column_end(col_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

insert *internal_arr_it_update_object_begin(obj_state *state, arr_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool internal_arr_it_update_object_end(obj_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_arr_it_update_from_carbon(arr_it *it, const rec *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_arr_it_update_from_array(arr_it *it, const arr_it *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_arr_it_update_from_object(arr_it *it, const obj_it *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_arr_it_update_from_column(arr_it *it, const col_it *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}


static void __arr_it_load_abstract_type(arr_it *it, u8 marker)
{
        abstract_type_class_e type_class;
        abstract_get_class(&type_class, marker);
        abstract_class_to_list_derivable(&it->list_type, type_class);
}

bool internal_arr_it_create(arr_it *it, memfile *memfile, offset_t payload_start)
{
        ZERO_MEMORY(it, sizeof(arr_it));

        it->begin = payload_start;
        it->mod_size = 0;
        it->eof = false;
        it->field_offset = 0;
        it->pos = (u64) -1;
        it->last_off = 0;

        MEMFILE_OPEN(&it->file, memfile->memblock, memfile->mode);
        MEMFILE_SEEK(&it->file, payload_start);

        if (MEMFILE_REMAIN_SIZE(&it->file) < sizeof(u8)) {
                return error(ERR_CORRUPTED, NULL);
        }

        u8 marker = MEMFILE_READ_BYTE(&it->file);

        if (!abstract_is_instanceof_array(marker)) {
            return error(ERR_MARKERMAPPING, "expected array or sub type marker");
        }

        __arr_it_load_abstract_type(it, marker);

        internal_field_create(&it->field);

        arr_it_rewind(it);

        return true;
}

bool internal_arr_it_copy(arr_it *dst, arr_it *src)
{
        internal_arr_it_create(dst, &src->file, src->begin);
        return true;
}

bool internal_arr_it_clone(arr_it *dst, arr_it *src)
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
        internal_item_create_from_array(&dst->item, dst);
        return true;
}

bool internal_arr_it_set_mode(arr_it *it, access_mode_e mode)
{
        it->file.mode = mode;
        return true;
}

bool arr_it_length(u64 *len, arr_it *it)
{
        u64 num_elem = 0;
        arr_it_rewind(it);
        while (arr_it_next(it)) {
                num_elem++;
        }
        *len = num_elem;

        return true;
}

bool arr_it_is_empty(arr_it *it)
{
        arr_it_rewind(it);
        return arr_it_next(it);
}

void arr_it_drop(arr_it *it)
{
        INTERNAL_FIELD_AUTO_CLOSE(&it->field);
        INTERNAL_FIELD_DROP(&it->field);
}

bool arr_it_rewind(arr_it *it)
{
        error_if_and_return(it->begin >= MEMFILE_SIZE(&it->file), ERR_OUTOFBOUNDS, NULL);
        it->pos = (u64) -1;
        return MEMFILE_SEEK(&it->file, it->begin + sizeof(u8));
}

static void auto_adjust_pos_after_mod(arr_it *it)
{
        if (internal_field_object_it_opened(&it->field)) {
                MEMFILE_SKIP(&it->file, it->field.object->mod_size);
        } else if (internal_field_array_opened(&it->field)) {
                //MEMFILE_SKIP(&it->mem, it->field.array->mod_size);
                //abort(); // TODO: implement!
        }
}

bool arr_it_has_next(arr_it *it)
{
        arr_it cpy;
        internal_arr_it_clone(&cpy, it);
        bool has_next = arr_it_next(&cpy);
        arr_it_drop(&cpy);

        return has_next;
}

bool arr_it_is_unit(arr_it *it)
{
        arr_it cpy;
        internal_arr_it_copy(&cpy, it);
        bool has_next = arr_it_next(&cpy);
        bool ret = false;

        if (has_next) {
                has_next = arr_it_next(&cpy);
                ret = !has_next;
        }

        arr_it_drop(&cpy);
        return ret;
}

static inline bool _internal_array_next__quick(arr_it *it)
{
        bool is_empty_slot = true;

        auto_adjust_pos_after_mod(it);
        offset_t last_off = MEMFILE_TELL(&it->file);

        if (internal_array_next__quick(&is_empty_slot, &it->eof, it)) {
                it->pos++;
                it->last_off = last_off;
                return true;
        } else {
                /** skip remaining zeros until end of array is reached */
                while (!it->eof && *MEMFILE_PEEK__FAST(&it->file) == 0) {
                        MEMFILE_SKIP__UNSAFE(&it->file, 1);
                }

                assert(*MEMFILE_PEEK(&it->file, sizeof(char)) == MARRAY_END);
                INTERNAL_FIELD_AUTO_CLOSE(&it->field);
                return false;
        }
}

item *arr_it_next(arr_it *it)
{
        if (_internal_array_next__quick(it)) {
                internal_item_create_from_array(&(it->item), it);
                return &(it->item);
        } else {
                return NULL;
        }
}

offset_t internal_arr_it_memfilepos(arr_it *it)
{
        if (likely(it != NULL)) {
                return MEMFILE_TELL(&it->file);
        } else {
                error(ERR_NULLPTR, NULL);
                return 0;
        }
}

offset_t internal_arr_it_tell(arr_it *it)
{
        return it ? it->field_offset : 0;
}

bool internal_arr_it_offset(offset_t *off, arr_it *it)
{
        if (it->last_off) {
                *off = it->last_off;
                return true;
        }
        return false;
}

bool internal_arr_it_fast_forward(arr_it *it)
{
        while (arr_it_next(it)) {}

        assert(*MEMFILE_PEEK(&it->file, sizeof(char)) == MARRAY_END);
        MEMFILE_SKIP(&it->file, sizeof(char));
        return true;
}

void arr_it_insert_begin(insert *in, arr_it *it)
{
        internal_insert_create_for_array(in, it);
}

void arr_it_insert_end(insert *in)
{
        UNUSED(in)
        /* nothing to do */
}

bool internal_arr_it_remove(arr_it *it)
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
                error(ERR_ILLEGALSTATE, NULL);
                return false;
        }
}

/** Checks if this array is annotated as a multi set abstract type. Returns true if it is is a multi set, and false if
 * it is a set. In case of any error, a failure is returned. */
bool arr_it_is_multiset(arr_it *it)
{
        abstract_type_class_e type_class;
        abstract_list_derivable_to_class(&type_class, it->list_type);
        return abstract_is_multiset(type_class);
}

/** Checks if this array is annotated as a sorted abstract type. Returns true if this is the case,
 * otherwise false. In case of any error, a failure is returned. */
bool arr_it_is_sorted(arr_it *it)
{
        abstract_type_class_e type_class;
        abstract_list_derivable_to_class(&type_class, it->list_type);
        return abstract_is_sorted(type_class);
}

void arr_it_update_type(arr_it *it, list_type_e derivation)
{
        MEMFILE_SAVE_POSITION(&it->file);
        MEMFILE_SEEK(&it->file, it->begin);

        derived_e derive_marker;
        abstract_derive_list_to(&derive_marker, LIST_ARRAY, derivation);
        abstract_write_derived_type(&it->file, derive_marker);

        MEMFILE_RESTORE_POSITION(&it->file);
}

bool arr_it_field_type(field_e *type, arr_it *it)
{
        return internal_field_field_type(type, &it->field);
}