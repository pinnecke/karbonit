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
#include <jakson/carbon/column.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/string.h>
#include <jakson/carbon/prop.h>
#include <jakson/carbon/object.h>
#include "object.h"

bool internal_carbon_object_create(carbon_object *it, memfile *memfile, err *err,
                                   offset_t payload_start)
{
        DEBUG_ERROR_IF_NULL(it);
        DEBUG_ERROR_IF_NULL(memfile);
        DEBUG_ERROR_IF_NULL(err);

        it->object_contents_off = payload_start;
        it->object_start_off = payload_start;
        it->mod_size = 0;
        it->pos = 0;
        it->object_end_reached = false;

        error_init(&it->err);

        vector_create(&it->history, NULL, sizeof(offset_t), 40);

        memfile_open(&it->memfile, memfile->memblock, memfile->mode);
        memfile_seek(&it->memfile, payload_start);

        ERROR_IF(memfile_remain_size(&it->memfile) < sizeof(u8), err, ERR_CORRUPTED);

        carbon_container_sub_type_e sub_type;
        carbon_abstract_get_container_subtype(&sub_type, &it->memfile);
        ERROR_IF_WDETAILS(sub_type != CARBON_CONTAINER_OBJECT, err, ERR_ILLEGALOP,
                              "object begin marker ('{') or abstract derived type marker for 'map' not found");
        char marker = memfile_read_byte(&it->memfile);
        it->abstract_type = (carbon_map_derivable_e) marker;

        it->object_contents_off += sizeof(u8);

        carbon_int_field_access_create(&it->field.value.data);

        carbon_object_rewind(it);

        return true;
}

bool internal_carbon_object_copy(carbon_object *dst, carbon_object *src)
{
        DEBUG_ERROR_IF_NULL(dst);
        DEBUG_ERROR_IF_NULL(src);
        internal_carbon_object_create(dst, &src->memfile, &src->err, src->object_start_off);
        return true;
}

bool internal_carbon_object_clone(carbon_object *dst, carbon_object *src)
{
        DEBUG_ERROR_IF_NULL(dst);
        DEBUG_ERROR_IF_NULL(src);
        memfile_clone(&dst->memfile, &src->memfile);
        dst->object_contents_off = src->object_contents_off;
        dst->object_start_off = src->object_start_off;
        error_cpy(&dst->err, &src->err);
        dst->mod_size = src->mod_size;
        dst->pos = src->pos;
        dst->object_end_reached = src->object_end_reached;
        dst->abstract_type = src->abstract_type;
        vector_cpy(&dst->history, &src->history);
        dst->field.key.name_len = src->field.key.name_len;
        dst->field.key.name = src->field.key.name;
        dst->field.key.offset = src->field.key.offset;
        dst->field.value.offset = src->field.value.offset;
        carbon_int_field_access_clone(&dst->field.value.data, &src->field.value.data);
        internal_carbon_prop_create(&dst->prop, dst);
        return true;
}

bool carbon_object_drop(carbon_object *it)
{
        carbon_int_field_auto_close(&it->field.value.data);
        carbon_int_field_access_drop(&it->field.value.data);
        vector_drop(&it->history);
        return true;
}

bool carbon_object_rewind(carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(it);
        ERROR_IF(it->object_contents_off >= memfile_size(&it->memfile), &it->err, ERR_OUTOFBOUNDS);
        carbon_int_history_clear(&it->history);
        it->pos = 0;
        return memfile_seek(&it->memfile, it->object_contents_off);
}

carbon_prop *carbon_object_next(carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(it);
        bool is_empty_slot;
        offset_t last_off = memfile_tell(&it->memfile);
        carbon_int_field_access_drop(&it->field.value.data);
        if (carbon_int_object_it_next(&is_empty_slot, &it->object_end_reached, it)) {
                it->pos++;
                carbon_int_history_push(&it->history, last_off);
                internal_carbon_prop_create(&it->prop, it);
                return &it->prop;
        } else {
                /** skip remaining zeros until end of array is reached */
                if (!it->object_end_reached) {
                        ERROR_IF(!is_empty_slot, &it->err, ERR_CORRUPTED);

                        while (*memfile_peek(&it->memfile, 1) == 0) {
                                memfile_skip(&it->memfile, 1);
                        }
                }

                JAK_ASSERT(*memfile_peek(&it->memfile, sizeof(char)) == CARBON_MOBJECT_END);
                return NULL;
        }
}

bool carbon_object_has_next(carbon_object *it)
{
        bool has_next = carbon_object_next(it);
        carbon_object_prev(it);
        return has_next;
}

bool carbon_object_prev(carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(it);
        if (carbon_int_history_has(&it->history)) {
                offset_t prev_off = carbon_int_history_pop(&it->history);
                it->pos--;
                memfile_seek(&it->memfile, prev_off);
                return carbon_int_object_it_refresh(NULL, NULL, it);
        } else {
                return false;
        }
}

offset_t internal_carbon_object_memfile_pos(carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(it)
        return memfile_tell(&it->memfile);
}

bool internal_carbon_object_tell(offset_t *key_off, offset_t *value_off, carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(it)
        OPTIONAL_SET(key_off, it->field.key.offset);
        OPTIONAL_SET(value_off, it->field.value.offset);
        return true;
}

carbon_string_field internal_carbon_object_prop_name(carbon_object *it)
{
        carbon_string_field ret = CARBON_NULL_STRING;
        if (LIKELY(it != NULL)) {
                ret.length = it->field.key.name_len;
                ret.string = it->field.key.name;
        }
        return ret;
}

static i64 prop_remove(carbon_object *it, carbon_field_type_e type)
{
        i64 prop_size = internal_carbon_prop_size(&it->memfile);
        carbon_string_nomarker_remove(&it->memfile);
        if (carbon_int_field_remove(&it->memfile, &it->err, type)) {
                carbon_int_object_it_refresh(NULL, NULL, it);
                return prop_size;
        } else {
                return 0;
        }
}

bool internal_carbon_object_remove(carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(it);
        carbon_field_type_e type;
        if (internal_carbon_object_prop_type(&type, it)) {
                offset_t prop_off = carbon_int_history_pop(&it->history);
                memfile_seek(&it->memfile, prop_off);
                it->mod_size -= prop_remove(it, type);
                return true;
        } else {
                ERROR(&it->err, ERR_ILLEGALSTATE);
                return false;
        }
}

bool internal_carbon_object_prop_type(carbon_field_type_e *type, carbon_object *it)
{
        return carbon_int_field_access_field_type(type, &it->field.value.data);
}

fn_result ofType(bool) carbon_object_is_multimap(carbon_object *it)
{
        FN_FAIL_IF_NULL(it)
        carbon_abstract_type_class_e type_class;
        carbon_abstract_map_derivable_to_class(&type_class, it->abstract_type);
        return carbon_abstract_is_multimap(type_class);
}

fn_result ofType(bool) carbon_object_is_sorted(carbon_object *it)
{
        FN_FAIL_IF_NULL(it)
        carbon_abstract_type_class_e type_class;
        carbon_abstract_map_derivable_to_class(&type_class, it->abstract_type);
        return carbon_abstract_is_sorted(type_class);
}

fn_result carbon_object_update_type(carbon_object *it, carbon_map_derivable_e derivation)
{
        FN_FAIL_IF_NULL(it)

        memfile_save_position(&it->memfile);
        memfile_seek(&it->memfile, it->object_start_off);

        carbon_derived_e derive_marker;
        carbon_abstract_derive_map_to(&derive_marker, derivation);
        carbon_abstract_write_derived_type(&it->memfile, derive_marker);

        memfile_restore_position(&it->memfile);

        return FN_OK();
}

bool internal_carbon_object_insert_begin(carbon_insert *inserter, carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(inserter)
        DEBUG_ERROR_IF_NULL(it)
        return carbon_int_insert_create_for_object(inserter, it);
}

fn_result internal_carbon_object_insert_end(carbon_insert *inserter)
{
        FN_FAIL_IF_NULL(inserter)
        return carbon_insert_drop(inserter);
}

bool internal_carbon_object_fast_forward(carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(it);
        while (carbon_object_next(it)) {}

        JAK_ASSERT(*memfile_peek(&it->memfile, sizeof(u8)) == CARBON_MOBJECT_END);
        memfile_skip(&it->memfile, sizeof(u8));
        return true;
}

bool internal_carbon_object_update_name(carbon_object *it, const char *key)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(key)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_u8(carbon_object *it, u8 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_u16(carbon_object *it, u16 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_u32(carbon_object *it, u32 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_u64(carbon_object *it, u64 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_i8(carbon_object *it, i8 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_i16(carbon_object *it, i16 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_i32(carbon_object *it, i32 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_i64(carbon_object *it, i64 value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_float(carbon_object *it, float value)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_true(carbon_object *it)
{
        // TODO: Implement P1
        UNUSED(it)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_false(carbon_object *it)
{
        // TODO: Implement P1
        UNUSED(it)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_null(carbon_object *it)
{
        // TODO: Implement P1
        UNUSED(it)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_string(carbon_object *it, const char *str)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(str)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_binary(carbon_object *it, const void *value, size_t nbytes, const char *file_ext, const char *user_type)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(value)
        UNUSED(nbytes)
        UNUSED(file_ext)
        UNUSED(user_type)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

carbon_insert *internal_carbon_object_update_array_begin(carbon_insert_array_state *state, carbon_object *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_array_end(carbon_insert_array_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

carbon_insert *internal_carbon_object_update_column_begin(carbon_insert_column_state *state, carbon_object *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_column_end(carbon_insert_column_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

carbon_insert *internal_carbon_object_update_object_begin(carbon_insert_object_state *state, carbon_object *it)
{
        // TODO: Implement P1
        UNUSED(state)
        UNUSED(it)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_object_end(carbon_insert_object_state *state)
{
        // TODO: Implement P1
        UNUSED(state)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_from_carbon(carbon_object *it, const rec *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_from_array(carbon_object *it, const carbon_object *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_from_object(carbon_object *it, const carbon_object *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}

bool internal_carbon_object_update_from_column(carbon_object *it, const carbon_column *src)
{
        // TODO: Implement P1
        UNUSED(it)
        UNUSED(src)
        ERROR_PRINT(ERR_NOTIMPLEMENTED)
        return false;
}
