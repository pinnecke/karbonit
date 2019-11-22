/**
 * Columnar Binary JSON -- Copyright 2019 Marcus Pinnecke
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

#include <jakson/rec.h>
#include <jakson/carbon/object.h>
#include <jakson/carbon/col_it.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/string.h>
#include <jakson/carbon/prop.h>

bool internal_carbon_object_create(obj_it *it, memfile *memfile, offset_t payload_start)
{
        it->object_contents_off = payload_start;
        it->object_start_off = payload_start;
        it->mod_size = 0;
        it->pos = 0;
        it->object_end_reached = false;

        vector_create(&it->history, sizeof(offset_t), 40);

        memfile_open(&it->memfile, memfile->memblock, memfile->mode);
        memfile_seek(&it->memfile, payload_start);

        error_if_and_return(memfile_remain_size(&it->memfile) < sizeof(u8), ERR_CORRUPTED, NULL);

        sub_type_e sub_type;
        abstract_get_container_subtype(&sub_type, &it->memfile);
        error_if_and_return(sub_type != CONTAINER_OBJECT, ERR_ILLEGALOP,
                              "object begin marker ('{') or abstract derived type marker for 'map' not found");
        char marker = memfile_read_byte(&it->memfile);
        it->abstract_type = (map_type_e) marker;

        it->object_contents_off += sizeof(u8);

        carbon_int_field_create(&it->field.value.data);

        carbon_object_rewind(it);

        return true;
}

bool internal_carbon_object_copy(obj_it *dst, obj_it *src)
{
        internal_carbon_object_create(dst, &src->memfile, src->object_start_off);
        return true;
}

bool internal_carbon_object_clone(obj_it *dst, obj_it *src)
{
        memfile_clone(&dst->memfile, &src->memfile);
        dst->object_contents_off = src->object_contents_off;
        dst->object_start_off = src->object_start_off;
        dst->mod_size = src->mod_size;
        dst->pos = src->pos;
        dst->object_end_reached = src->object_end_reached;
        dst->abstract_type = src->abstract_type;
        vector_cpy(&dst->history, &src->history);
        dst->field.key.name_len = src->field.key.name_len;
        dst->field.key.name = src->field.key.name;
        dst->field.key.offset = src->field.key.offset;
        dst->field.value.offset = src->field.value.offset;
        carbon_int_field_clone(&dst->field.value.data, &src->field.value.data);
        internal_carbon_prop_create(&dst->prop, dst);
        return true;
}

bool carbon_object_drop(obj_it *it)
{
        carbon_int_field_auto_close(&it->field.value.data);
        carbon_int_field_drop(&it->field.value.data);
        vector_drop(&it->history);
        return true;
}

bool carbon_object_rewind(obj_it *it)
{
        error_if_and_return(it->object_contents_off >= memfile_size(&it->memfile), ERR_OUTOFBOUNDS, NULL);
        carbon_int_history_clear(&it->history);
        it->pos = 0;
        return memfile_seek(&it->memfile, it->object_contents_off);
}

carbon_prop *carbon_object_next(obj_it *it)
{
        bool is_empty_slot;
        offset_t last_off = memfile_tell(&it->memfile);
        carbon_int_field_drop(&it->field.value.data);
        if (carbon_int_object_it_next(&is_empty_slot, &it->object_end_reached, it)) {
                it->pos++;
                carbon_int_history_push(&it->history, last_off);
                internal_carbon_prop_create(&it->prop, it);
                return &it->prop;
        } else {
                /** skip remaining zeros until end of array is reached */
                if (!it->object_end_reached) {
                        error_if_and_return(!is_empty_slot, ERR_CORRUPTED, NULL);

                        while (*memfile_peek(&it->memfile, 1) == 0) {
                                memfile_skip(&it->memfile, 1);
                        }
                }

                JAK_ASSERT(*memfile_peek(&it->memfile, sizeof(char)) == MOBJECT_END);
                return NULL;
        }
}

bool carbon_object_has_next(obj_it *it)
{
        bool has_next = carbon_object_next(it);
        carbon_object_prev(it);
        return has_next;
}

bool carbon_object_prev(obj_it *it)
{
        if (carbon_int_history_has(&it->history)) {
                offset_t prev_off = carbon_int_history_pop(&it->history);
                it->pos--;
                memfile_seek(&it->memfile, prev_off);
                return carbon_int_object_it_refresh(NULL, NULL, it);
        } else {
                return false;
        }
}

offset_t internal_carbon_object_memfile_pos(obj_it *it)
{
        return memfile_tell(&it->memfile);
}

bool internal_carbon_object_tell(offset_t *key_off, offset_t *value_off, obj_it *it)
{
        OPTIONAL_SET(key_off, it->field.key.offset);
        OPTIONAL_SET(value_off, it->field.value.offset);
        return true;
}

carbon_string_field internal_carbon_object_prop_name(obj_it *it)
{
        carbon_string_field ret = CARBON_NULL_STRING;
        if (LIKELY(it != NULL)) {
                ret.length = it->field.key.name_len;
                ret.string = it->field.key.name;
        }
        return ret;
}

static i64 prop_remove(obj_it *it, field_e type)
{
        i64 prop_size = internal_carbon_prop_size(&it->memfile);
        carbon_string_nomarker_remove(&it->memfile);
        if (carbon_int_field_remove(&it->memfile, type)) {
                carbon_int_object_it_refresh(NULL, NULL, it);
                return prop_size;
        } else {
                return 0;
        }
}

bool internal_carbon_object_remove(obj_it *it)
{
        field_e type;
        if (internal_carbon_object_prop_type(&type, it)) {
                offset_t prop_off = carbon_int_history_pop(&it->history);
                memfile_seek(&it->memfile, prop_off);
                it->mod_size -= prop_remove(it, type);
                return true;
        } else {
                error(ERR_ILLEGALSTATE, NULL);
                return false;
        }
}

bool internal_carbon_object_prop_type(field_e *type, obj_it *it)
{
        return carbon_int_field_field_type(type, &it->field.value.data);
}

bool carbon_object_is_multimap(obj_it *it)
{
        abstract_type_class_e type_class;
        abstract_map_derivable_to_class(&type_class, it->abstract_type);
        return abstract_is_multimap(type_class);
}

bool carbon_object_is_sorted(obj_it *it)
{
        abstract_type_class_e type_class;
        abstract_map_derivable_to_class(&type_class, it->abstract_type);
        return abstract_is_sorted(type_class);
}

void carbon_object_update_type(obj_it *it, map_type_e derivation)
{
        memfile_save_position(&it->memfile);
        memfile_seek(&it->memfile, it->object_start_off);

        derived_e derive_marker;
        abstract_derive_map_to(&derive_marker, derivation);
        abstract_write_derived_type(&it->memfile, derive_marker);

        memfile_restore_position(&it->memfile);
}

bool internal_carbon_object_insert_begin(carbon_insert *inserter, obj_it *it)
{
        return carbon_int_insert_create_for_object(inserter, it);
}

void internal_carbon_object_insert_end(carbon_insert *inserter)
{
        UNUSED(inserter)
        /* nothing to do */
}

bool internal_carbon_object_fast_forward(obj_it *it)
{
        while (carbon_object_next(it)) {}

        JAK_ASSERT(*memfile_peek(&it->memfile, sizeof(u8)) == MOBJECT_END);
        memfile_skip(&it->memfile, sizeof(u8));
        return true;
}

bool internal_carbon_object_update_name(obj_it *it, const char *key)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(key)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_u8(obj_it *it, u8 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_u16(obj_it *it, u16 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_u32(obj_it *it, u32 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_u64(obj_it *it, u64 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_i8(obj_it *it, i8 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_i16(obj_it *it, i16 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_i32(obj_it *it, i32 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_i64(obj_it *it, i64 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_float(obj_it *it, float value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_true(obj_it *it)
{
        // TODO: Implement P1
        UNUSED(it)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_false(obj_it *it)
{
        // TODO: Implement P1
        UNUSED(it)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_null(obj_it *it)
{
        // TODO: Implement P1
        UNUSED(it)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_string(obj_it *it, const char *str)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(str)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_binary(obj_it *it, const void *value, size_t nbytes, const char *file_ext, const char *user_type)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        UNUSED(nbytes)
        UNUSED(file_ext)
        UNUSED(user_type)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

carbon_insert *internal_carbon_object_update_array_begin(carbon_insert_array_state *state, obj_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool internal_carbon_object_update_array_end(carbon_insert_array_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

carbon_insert *internal_carbon_object_update_column_begin(carbon_insert_column_state *state, obj_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool internal_carbon_object_update_column_end(carbon_insert_column_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

carbon_insert *internal_carbon_object_update_object_begin(carbon_insert_object_state *state, obj_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool internal_carbon_object_update_object_end(carbon_insert_object_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_from_carbon(obj_it *it, const rec *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_from_array(obj_it *it, const obj_it *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_from_object(obj_it *it, const obj_it *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_carbon_object_update_from_column(obj_it *it, const col_it *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}
