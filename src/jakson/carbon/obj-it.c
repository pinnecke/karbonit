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
#include <jakson/carbon/obj-it.h>
#include <jakson/carbon/col-it.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/string-field.h>
#include <jakson/carbon/prop.h>

bool internal_obj_it_create(obj_it *it, memfile *memfile, offset_t payload_start)
{
        it->content_begin = payload_start;
        it->begin = payload_start;
        it->mod_size = 0;
        it->pos = 0;
        it->eof = false;

        vector_create(&it->history, sizeof(offset_t), 40);

        MEMFILE_OPEN(&it->file, memfile->memblock, memfile->mode);
        MEMFILE_SEEK(&it->file, payload_start);

        error_if_and_return(MEMFILE_REMAIN_SIZE(&it->file) < sizeof(u8), ERR_CORRUPTED, NULL);

        sub_type_e sub_type;
        abstract_get_container_subtype(&sub_type, &it->file);
        error_if_and_return(sub_type != CONTAINER_OBJECT, ERR_ILLEGALOP,
                              "object begin marker ('{') or abstract derived type marker for 'map' not found");
        char marker = MEMFILE_READ_BYTE(&it->file);
        it->type = (map_type_e) marker;

        it->content_begin += sizeof(u8);

        internal_field_create(&it->field.value.data);

        obj_it_rewind(it);

        return true;
}

bool internal_obj_it_copy(obj_it *dst, obj_it *src)
{
        internal_obj_it_create(dst, &src->file, src->begin);
        return true;
}

bool internal_obj_it_clone(obj_it *dst, obj_it *src)
{
        MEMFILE_CLONE(&dst->file, &src->file);
        dst->content_begin = src->content_begin;
        dst->begin = src->begin;
        dst->mod_size = src->mod_size;
        dst->pos = src->pos;
        dst->eof = src->eof;
        dst->type = src->type;
        vector_cpy(&dst->history, &src->history);
        dst->field.key.name_len = src->field.key.name_len;
        dst->field.key.name = src->field.key.name;
        dst->field.key.start = src->field.key.start;
        dst->field.value.start = src->field.value.start;
        internal_field_clone(&dst->field.value.data, &src->field.value.data);
        internal_prop_create(&dst->prop, dst);
        return true;
}

bool obj_it_drop(obj_it *it)
{
        INTERNAL_FIELD_AUTO_CLOSE(&it->field.value.data);
        INTERNAL_FIELD_DROP(&it->field.value.data);
        vector_drop(&it->history);
        return true;
}

bool obj_it_rewind(obj_it *it)
{
        error_if_and_return(it->content_begin >= MEMFILE_SIZE(&it->file), ERR_OUTOFBOUNDS, NULL);
        internal_history_clear(&it->history);
        it->pos = 0;
        return MEMFILE_SEEK(&it->file, it->content_begin);
}

prop *obj_it_next(obj_it *it)
{
        bool is_empty_slot;
        offset_t last_off = MEMFILE_TELL(&it->file);
        INTERNAL_FIELD_DROP(&it->field.value.data);
        if (internal_object_it_next(&is_empty_slot, &it->eof, it)) {
                internal_history_push(&it->history, last_off);
                internal_prop_create(&it->prop, it);
                it->pos++;
                return &it->prop;
        } else {
                /** skip remaining zeros until end of array is reached */
                if (!it->eof) {
                        error_if_and_return(!is_empty_slot, ERR_CORRUPTED, NULL);

                        while (*MEMFILE_PEEK(&it->file, 1) == 0) {
                                MEMFILE_SKIP(&it->file, 1);
                        }
                }

                assert(*MEMFILE_PEEK(&it->file, sizeof(char)) == MOBJECT_END);
                return NULL;
        }
}

bool obj_it_has_next(obj_it *it)
{
        bool has_next = obj_it_next(it);
        obj_it_prev(it);
        return has_next;
}

bool obj_it_prev(obj_it *it)
{
        if (internal_history_has(&it->history)) {
                offset_t prev_off = internal_history_pop(&it->history);
                it->pos--;
                MEMFILE_SEEK(&it->file, prev_off);
                return internal_object_it_refresh(NULL, NULL, it);
        } else {
                return false;
        }
}

offset_t internal_obj_it_memfile_pos(obj_it *it)
{
        return MEMFILE_TELL(&it->file);
}

bool internal_obj_it_tell(offset_t *key_off, offset_t *value_off, obj_it *it)
{
        OPTIONAL_SET(key_off, it->field.key.start);
        OPTIONAL_SET(value_off, it->field.value.start);
        return true;
}

string_field internal_obj_it_prop_name(obj_it *it)
{
        string_field ret = NULL_STRING;
        if (likely(it != NULL)) {
                ret.len = it->field.key.name_len;
                ret.str = it->field.key.name;
        }
        return ret;
}

static i64 _prop_remove(obj_it *it, field_e type)
{
        i64 prop_size = internal_prop_size(&it->file);
        string_field_nomarker_remove(&it->file);
        if (internal_field_remove(&it->file, type)) {
                internal_object_it_refresh(NULL, NULL, it);
                return prop_size;
        } else {
                return 0;
        }
}

bool internal_obj_it_remove(obj_it *it)
{
        field_e type;
        if (internal_obj_it_prop_type(&type, it)) {
                offset_t prop_off = internal_history_pop(&it->history);
                MEMFILE_SEEK(&it->file, prop_off);
                it->mod_size -= _prop_remove(it, type);
                return true;
        } else {
                error(ERR_ILLEGALSTATE, NULL);
                return false;
        }
}

bool internal_obj_it_prop_type(field_e *type, obj_it *it)
{
        return internal_field_field_type(type, &it->field.value.data);
}

bool obj_it_is_multimap(obj_it *it)
{
        abstract_type_class_e type_class;
        abstract_map_derivable_to_class(&type_class, it->type);
        return abstract_is_multimap(type_class);
}

bool obj_it_is_sorted(obj_it *it)
{
        abstract_type_class_e type_class;
        abstract_map_derivable_to_class(&type_class, it->type);
        return abstract_is_sorted(type_class);
}

void obj_it_update_type(obj_it *it, map_type_e derivation)
{
        MEMFILE_SAVE_POSITION(&it->file);
        MEMFILE_SEEK(&it->file, it->begin);

        derived_e derive_marker;
        abstract_derive_map_to(&derive_marker, derivation);
        abstract_write_derived_type(&it->file, derive_marker);

        MEMFILE_RESTORE_POSITION(&it->file);
}

bool internal_obj_it_insert_begin(insert *in, obj_it *it)
{
        return internal_insert_create_for_object(in, it);
}

void internal_obj_it_insert_end(insert *in)
{
        UNUSED(in)
        /* nothing to do */
}

bool internal_obj_it_fast_forward(obj_it *it)
{
        while (obj_it_next(it)) {}

        assert(*MEMFILE_PEEK(&it->file, sizeof(u8)) == MOBJECT_END);
        MEMFILE_SKIP(&it->file, sizeof(u8));
        return true;
}

bool internal_obj_it_update_name(obj_it *it, const char *key)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(key)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_u8(obj_it *it, u8 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_u16(obj_it *it, u16 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_u32(obj_it *it, u32 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_u64(obj_it *it, u64 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_i8(obj_it *it, i8 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_i16(obj_it *it, i16 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_i32(obj_it *it, i32 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_i64(obj_it *it, i64 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_float(obj_it *it, float value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_true(obj_it *it)
{
        // TODO: Implement P1
        UNUSED(it)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_false(obj_it *it)
{
        // TODO: Implement P1
        UNUSED(it)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_null(obj_it *it)
{
        // TODO: Implement P1
        UNUSED(it)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_string(obj_it *it, const char *str)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(str)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_binary(obj_it *it, const void *value, size_t nbytes, const char *file_ext, const char *user_type)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        UNUSED(nbytes)
        UNUSED(file_ext)
        UNUSED(user_type)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

insert *internal_obj_it_update_array_begin(arr_state *state, obj_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool internal_obj_it_update_array_end(arr_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

insert *internal_obj_it_update_column_begin(col_state *state, obj_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool internal_obj_it_update_column_end(col_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

insert *internal_obj_it_update_object_begin(obj_state *state, obj_it *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        error(ERR_NOTIMPLEMENTED, NULL);
        return NULL;
}

bool internal_obj_it_update_object_end(obj_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_from_carbon(obj_it *it, const rec *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_from_array(obj_it *it, const obj_it *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_from_object(obj_it *it, const obj_it *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}

bool internal_obj_it_update_from_column(obj_it *it, const col_it *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        return error(ERR_NOTIMPLEMENTED, NULL);
}
