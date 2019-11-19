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
#include <jakson/carbon/array.h>
#include <jakson/carbon/column.h>
#include <jakson/carbon/object.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/mime.h>
#include <jakson/carbon/internal.h>
#include <jakson/carbon/item.h>

#define DEFINE_IN_PLACE_UPDATE_FUNCTION(type_name, field_type)                                                         \
bool internal_carbon_array_update_##type_name(arr_it *it, type_name value)                \
{                                                                                                                      \
        offset_t datum = 0;                                                                                                \
        if (LIKELY(it->field.type == field_type)) {                                                    \
                memfile_save_position(&it->memfile);                                                                   \
                internal_carbon_array_offset(&datum, it);                                                                 \
                memfile_seek(&it->memfile, datum + sizeof(u8));                                                        \
                memfile_write(&it->memfile, &value, sizeof(type_name));                                                \
                memfile_restore_position(&it->memfile);                                                                \
                return true;                                                                                           \
        } else {                                                                                                       \
                error(ERR_TYPEMISMATCH, NULL);                                                                 \
                return false;                                                                                          \
        }                                                                                                              \
}

DEFINE_IN_PLACE_UPDATE_FUNCTION(u8, CARBON_FIELD_NUMBER_U8)

DEFINE_IN_PLACE_UPDATE_FUNCTION(u16, CARBON_FIELD_NUMBER_U16)

DEFINE_IN_PLACE_UPDATE_FUNCTION(u32, CARBON_FIELD_NUMBER_U32)

DEFINE_IN_PLACE_UPDATE_FUNCTION(u64, CARBON_FIELD_NUMBER_U64)

DEFINE_IN_PLACE_UPDATE_FUNCTION(i8, CARBON_FIELD_NUMBER_I8)

DEFINE_IN_PLACE_UPDATE_FUNCTION(i16, CARBON_FIELD_NUMBER_I16)

DEFINE_IN_PLACE_UPDATE_FUNCTION(i32, CARBON_FIELD_NUMBER_I32)

DEFINE_IN_PLACE_UPDATE_FUNCTION(i64, CARBON_FIELD_NUMBER_I64)

DEFINE_IN_PLACE_UPDATE_FUNCTION(float, CARBON_FIELD_NUMBER_FLOAT)

static bool update_in_place_constant(arr_it *it, carbon_constant_e constant)
{
        memfile_save_position(&it->memfile);

        if (carbon_field_type_is_constant(it->field.type)) {
                u8 value;
                switch (constant) {
                        case CARBON_CONSTANT_TRUE:
                                value = CARBON_FIELD_TRUE;
                                break;
                        case CARBON_CONSTANT_FALSE:
                                value = CARBON_FIELD_FALSE;
                                break;
                        case CARBON_CONSTANT_NULL:
                                value = CARBON_FIELD_NULL;
                                break;
                        default: error(ERR_INTERNALERR, NULL);
                                break;
                }
                offset_t datum = 0;
                internal_carbon_array_offset(&datum, it);
                memfile_seek(&it->memfile, datum);
                memfile_write(&it->memfile, &value, sizeof(u8));
        } else {
                carbon_insert ins;
                internal_carbon_array_remove(it);
                carbon_array_insert_begin(&ins, it);

                switch (constant) {
                        case CARBON_CONSTANT_TRUE:
                                carbon_insert_true(&ins);
                                break;
                        case CARBON_CONSTANT_FALSE:
                                carbon_insert_false(&ins);
                                break;
                        case CARBON_CONSTANT_NULL:
                                carbon_insert_null(&ins);
                                break;
                        default: error(ERR_INTERNALERR, NULL);
                                break;
                }

                carbon_array_insert_end(&ins);
        }

        memfile_restore_position(&it->memfile);
        return true;
}

bool internal_carbon_array_update_true(arr_it *it)
{
        return update_in_place_constant(it, CARBON_CONSTANT_TRUE);
}

bool internal_carbon_array_update_false(arr_it *it)
{
        return update_in_place_constant(it, CARBON_CONSTANT_FALSE);
}

bool internal_carbon_array_update_null(arr_it *it)
{
        return update_in_place_constant(it, CARBON_CONSTANT_NULL);
}

bool internal_carbon_array_update_string(arr_it *it, const char *str)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(str)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_array_update_binary(arr_it *it, const void *value, size_t nbytes, const char *file_ext, const char *user_type)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        UNUSED(nbytes)
        UNUSED(file_ext)
        UNUSED(user_type)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

carbon_insert *internal_carbon_array_update_array_begin(carbon_insert_array_state *state, arr_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool internal_carbon_array_update_array_end(carbon_insert_array_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

carbon_insert *internal_carbon_array_update_column_begin(carbon_insert_column_state *state, arr_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool internal_carbon_array_update_column_end(carbon_insert_column_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

carbon_insert *internal_carbon_array_update_object_begin(carbon_insert_object_state *state, arr_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool internal_carbon_array_update_object_end(carbon_insert_object_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_array_update_from_carbon(arr_it *it, const rec *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_array_update_from_array(arr_it *it, const arr_it *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_array_update_from_object(arr_it *it, const carbon_object *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_array_update_from_column(arr_it *it, const carbon_column *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}


static void __carbon_array_load_abstract_type(arr_it *it)
{
        abstract_type_class_e type_class;
        abstract_get_class(&type_class, &it->memfile);
        abstract_class_to_list_derivable(&it->abstract_type, type_class);
}

bool internal_carbon_array_create(arr_it *it, memfile *memfile, offset_t payload_start)
{
        ZERO_MEMORY(it, sizeof(arr_it));

        it->array_begin_off = payload_start;
        it->mod_size = 0;
        it->array_end_reached = false;
        it->field_offset = 0;
        it->pos = (u64) -1;

        vector_create(&it->history, sizeof(offset_t), 40);
        memfile_open(&it->memfile, memfile->memblock, memfile->mode);
        memfile_seek(&it->memfile, payload_start);

        if (memfile_remain_size(&it->memfile) < sizeof(u8)) {
                return error(ERR_CORRUPTED, NULL);
        }

        if (!abstract_is_instanceof_array(&it->memfile)) {
            return error(ERR_MARKERMAPPING, "expected array or sub type marker");
        }

        __carbon_array_load_abstract_type(it);

        memfile_skip(&it->memfile, sizeof(u8));

        carbon_int_field_create(&it->field);

        carbon_array_rewind(it);

        return true;
}

bool internal_carbon_array_copy(arr_it *dst, arr_it *src)
{
        internal_carbon_array_create(dst, &src->memfile, src->array_begin_off);
        return true;
}

bool internal_carbon_array_clone(arr_it *dst, arr_it *src)
{
        memfile_clone(&dst->memfile, &src->memfile);
        dst->array_begin_off = src->array_begin_off;
        dst->mod_size = src->mod_size;
        dst->array_end_reached = src->array_end_reached;
        dst->abstract_type = src->abstract_type;
        vector_cpy(&dst->history, &src->history);
        carbon_int_field_clone(&dst->field, &src->field);
        dst->field_offset = src->field_offset;
        dst->pos = src->pos;
        internal_carbon_item_create_from_array(&dst->item, dst);
        return true;
}

bool internal_carbon_array_set_mode(arr_it *it, access_mode_e mode)
{
        it->memfile.mode = mode;
        return true;
}

bool carbon_array_length(u64 *len, arr_it *it)
{
        u64 num_elem = 0;
        carbon_array_rewind(it);
        while (carbon_array_next(it)) {
                num_elem++;
        }
        *len = num_elem;

        return true;
}

bool carbon_array_is_empty(arr_it *it)
{
        carbon_array_rewind(it);
        return carbon_array_next(it);
}

void carbon_array_drop(arr_it *it)
{
        carbon_int_field_auto_close(&it->field);
        carbon_int_field_drop(&it->field);
        vector_drop(&it->history);
}

bool carbon_array_rewind(arr_it *it)
{
        error_if_and_return(it->array_begin_off >= memfile_size(&it->memfile), ERR_OUTOFBOUNDS, NULL);
        carbon_int_history_clear(&it->history);
        it->pos = (u64) -1;
        return memfile_seek(&it->memfile, it->array_begin_off + sizeof(u8));
}

static void auto_adjust_pos_after_mod(arr_it *it)
{
        if (carbon_int_field_object_it_opened(&it->field)) {
                memfile_skip(&it->memfile, it->field.object->mod_size);
        } else if (carbon_int_field_array_opened(&it->field)) {
                //memfile_skip(&it->mem, it->field.array->mod_size);
                //abort(); // TODO: implement!
        }
}

bool carbon_array_has_next(arr_it *it)
{
        bool has_next = carbon_array_next(it);
        carbon_array_prev(it);
        return has_next;
}

bool carbon_array_is_unit(arr_it *it)
{
        bool has_next = carbon_array_next(it);
        if (has_next) {
                has_next = carbon_array_next(it);
                carbon_array_prev(it);
                carbon_array_prev(it);
                return !has_next;
        }
        carbon_array_prev(it);
        return false;
}

static bool internal_array_next(arr_it *it)
{
        bool is_empty_slot = true;

        auto_adjust_pos_after_mod(it);
        offset_t last_off = memfile_tell(&it->memfile);

        if (carbon_int_array_next(&is_empty_slot, &it->array_end_reached, it)) {
                it->pos++;
                carbon_int_history_push(&it->history, last_off);
                return true;
        } else {
                /** skip remaining zeros until end of array is reached */
                if (!it->array_end_reached) {
                        error_if_and_return(!is_empty_slot, ERR_CORRUPTED, NULL);

                        while (*memfile_peek(&it->memfile, 1) == 0) {
                                memfile_skip(&it->memfile, 1);
                        }
                }
                JAK_ASSERT(*memfile_peek(&it->memfile, sizeof(char)) == CARBON_MARRAY_END);
                carbon_int_field_auto_close(&it->field);
                return false;
        }
}

carbon_item *carbon_array_next(arr_it *it)
{
        if (internal_array_next(it)) {
                internal_carbon_item_create_from_array(&(it->item), it);
                return &(it->item);
        } else {
                return NULL;
        }
}

bool carbon_array_prev(arr_it *it)
{
        if (carbon_int_history_has(&it->history)) {
                offset_t prev_off = carbon_int_history_pop(&it->history);
                memfile_seek(&it->memfile, prev_off);
                it->pos--;
                return carbon_int_array_refresh(NULL, NULL, it);
        } else {
                return false;
        }
}

offset_t internal_carbon_array_memfilepos(arr_it *it)
{
        if (LIKELY(it != NULL)) {
                return memfile_tell(&it->memfile);
        } else {
                error(ERR_NULLPTR, NULL);
                return 0;
        }
}

offset_t internal_carbon_array_tell(arr_it *it)
{
        return it ? it->field_offset : 0;
}

bool internal_carbon_array_offset(offset_t *off, arr_it *it)
{
        if (carbon_int_history_has(&it->history)) {
                *off = carbon_int_history_peek(&it->history);
                return true;
        }
        return false;
}

bool internal_carbon_array_fast_forward(arr_it *it)
{
        while (carbon_array_next(it)) {}

        JAK_ASSERT(*memfile_peek(&it->memfile, sizeof(char)) == CARBON_MARRAY_END);
        memfile_skip(&it->memfile, sizeof(char));
        return true;
}

void carbon_array_insert_begin(carbon_insert *inserter, arr_it *it)
{
        carbon_int_insert_create_for_array(inserter, it);
}

void carbon_array_insert_end(carbon_insert *inserter)
{
        UNUSED(inserter)
        /* nothing to do */
}

bool internal_carbon_array_remove(arr_it *it)
{
        field_type_e type;
        if (carbon_array_field_type(&type, it)) {
                offset_t prev_off = carbon_int_history_pop(&it->history);
                memfile_seek(&it->memfile, prev_off);
                if (carbon_int_field_remove(&it->memfile, type)) {
                        carbon_int_array_refresh(NULL, NULL, it);
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
bool carbon_array_is_multiset(arr_it *it)
{
        abstract_type_class_e type_class;
        abstract_list_derivable_to_class(&type_class, it->abstract_type);
        return abstract_is_multiset(type_class);
}

/** Checks if this array is annotated as a sorted abstract type. Returns true if this is the case,
 * otherwise false. In case of any error, a failure is returned. */
bool carbon_array_is_sorted(arr_it *it)
{
        abstract_type_class_e type_class;
        abstract_list_derivable_to_class(&type_class, it->abstract_type);
        return abstract_is_sorted(type_class);
}

void carbon_array_update_type(arr_it *it, list_derivable_e derivation)
{
        memfile_save_position(&it->memfile);
        memfile_seek(&it->memfile, it->array_begin_off);

        derived_e derive_marker;
        abstract_derive_list_to(&derive_marker, LIST_CONTAINER_ARRAY, derivation);
        abstract_write_derived_type(&it->memfile, derive_marker);

        memfile_restore_position(&it->memfile);
}

bool carbon_array_field_type(field_type_e *type, arr_it *it)
{
        return carbon_int_field_field_type(type, &it->field);
}