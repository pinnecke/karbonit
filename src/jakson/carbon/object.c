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

#include <jakson/carbon.h>
#include <jakson/carbon/object_it.h>
#include <jakson/carbon/column.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/string.h>
#include <jakson/carbon/prop.h>
#include <jakson/carbon/object_it.h>

bool carbon_object_create(carbon_object *it, memfile *memfile, err *err,
                             offset_t payload_start)
{
        DEBUG_ERROR_IF_NULL(it);
        DEBUG_ERROR_IF_NULL(memfile);
        DEBUG_ERROR_IF_NULL(err);

        it->object_contents_off = payload_start;
        it->object_start_off = payload_start;
        it->mod_size = 0;
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

bool carbon_object_copy(carbon_object *dst, carbon_object *src)
{
        DEBUG_ERROR_IF_NULL(dst);
        DEBUG_ERROR_IF_NULL(src);
        carbon_object_create(dst, &src->memfile, &src->err, src->object_start_off);
        return true;
}

bool carbon_object_clone(carbon_object *dst, carbon_object *src)
{
        DEBUG_ERROR_IF_NULL(dst);
        DEBUG_ERROR_IF_NULL(src);
        memfile_clone(&dst->memfile, &src->memfile);
        dst->object_contents_off = src->object_contents_off;
        dst->object_start_off = src->object_start_off;
        error_cpy(&dst->err, &src->err);
        dst->mod_size = src->mod_size;
        dst->object_end_reached = src->object_end_reached;
        dst->abstract_type = src->abstract_type;
        vector_cpy(&dst->history, &src->history);
        dst->field.key.name_len = src->field.key.name_len;
        dst->field.key.name = src->field.key.name;
        dst->field.key.offset = src->field.key.offset;
        dst->field.value.offset = src->field.value.offset;
        carbon_int_field_access_clone(&dst->field.value.data, &src->field.value.data);
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
        return memfile_seek(&it->memfile, it->object_contents_off);
}

bool carbon_object_next(carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(it);
        bool is_empty_slot;
        offset_t last_off = memfile_tell(&it->memfile);
        carbon_int_field_access_drop(&it->field.value.data);
        if (carbon_int_object_it_next(&is_empty_slot, &it->object_end_reached, it)) {
                carbon_int_history_push(&it->history, last_off);
                return true;
        } else {
                /** skip remaining zeros until end of array is reached */
                if (!it->object_end_reached) {
                        ERROR_IF(!is_empty_slot, &it->err, ERR_CORRUPTED);

                        while (*memfile_peek(&it->memfile, 1) == 0) {
                                memfile_skip(&it->memfile, 1);
                        }
                }

                JAK_ASSERT(*memfile_peek(&it->memfile, sizeof(char)) == CARBON_MOBJECT_END);
                return false;
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
                memfile_seek(&it->memfile, prev_off);
                return carbon_int_object_it_refresh(NULL, NULL, it);
        } else {
                return false;
        }
}

offset_t carbon_object_memfile_pos(carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(it)
        return memfile_tell(&it->memfile);
}

bool carbon_object_tell(offset_t *key_off, offset_t *value_off, carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(it)
        OPTIONAL_SET(key_off, it->field.key.offset);
        OPTIONAL_SET(value_off, it->field.value.offset);
        return true;
}

const char *carbon_object_prop_name(u64 *key_len, carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(it)
        DEBUG_ERROR_IF_NULL(key_len)
        *key_len = it->field.key.name_len;
        return it->field.key.name;
}

static i64 prop_remove(carbon_object *it, carbon_field_type_e type)
{
        i64 prop_size = carbon_prop_size(&it->memfile);
        carbon_string_nomarker_remove(&it->memfile);
        if (carbon_int_field_remove(&it->memfile, &it->err, type)) {
                carbon_int_object_it_refresh(NULL, NULL, it);
                return prop_size;
        } else {
                return 0;
        }
}

bool carbon_object_remove(carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(it);
        carbon_field_type_e type;
        if (carbon_object_prop_type(&type, it)) {
                offset_t prop_off = carbon_int_history_pop(&it->history);
                memfile_seek(&it->memfile, prop_off);
                it->mod_size -= prop_remove(it, type);
                return true;
        } else {
                ERROR(&it->err, ERR_ILLEGALSTATE);
                return false;
        }
}

bool carbon_object_prop_type(carbon_field_type_e *type, carbon_object *it)
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

bool carbon_object_bool_value(bool *is_true, carbon_object *it)
{
        return carbon_int_field_access_bool_value(is_true, &it->field.value.data, &it->err);
}

bool carbon_object_is_null(bool *is_null, carbon_object *it)
{
        return carbon_int_field_access_is_null(is_null, &it->field.value.data);
}

bool carbon_object_u8_value(u8 *value, carbon_object *it)
{
        return carbon_int_field_access_u8_value(value, &it->field.value.data, &it->err);
}

bool carbon_object_u16_value(u16 *value, carbon_object *it)
{
        return carbon_int_field_access_u16_value(value, &it->field.value.data, &it->err);
}

bool carbon_object_u32_value(u32 *value, carbon_object *it)
{
        return carbon_int_field_access_u32_value(value, &it->field.value.data, &it->err);
}

bool carbon_object_u64_value(u64 *value, carbon_object *it)
{
        return carbon_int_field_access_u64_value(value, &it->field.value.data, &it->err);
}

bool carbon_object_i8_value(i8 *value, carbon_object *it)
{
        return carbon_int_field_access_i8_value(value, &it->field.value.data, &it->err);
}

bool carbon_object_i16_value(i16 *value, carbon_object *it)
{
        return carbon_int_field_access_i16_value(value, &it->field.value.data, &it->err);
}

bool carbon_object_i32_value(i32 *value, carbon_object *it)
{
        return carbon_int_field_access_i32_value(value, &it->field.value.data, &it->err);
}

bool carbon_object_i64_value(i64 *value, carbon_object *it)
{
        return carbon_int_field_access_i64_value(value, &it->field.value.data, &it->err);
}

bool carbon_object_float_value(float *value, carbon_object *it)
{
        return carbon_int_field_access_float_value(value, &it->field.value.data, &it->err);
}

bool carbon_object_float_value_nullable(bool *is_null_in, float *value, carbon_object *it)
{
        return carbon_int_field_access_float_value_nullable(is_null_in, value, &it->field.value.data, &it->err);
}

bool carbon_object_signed_value(bool *is_null_in, i64 *value, carbon_object *it)
{
        return carbon_int_field_access_signed_value(is_null_in, value, &it->field.value.data, &it->err);
}

bool carbon_object_unsigned_value(bool *is_null_in, u64 *value, carbon_object *it)
{
        return carbon_int_field_access_unsigned_value(is_null_in, value, &it->field.value.data, &it->err);
}

const char *carbon_object_string_value(u64 *strlen, carbon_object *it)
{
        return carbon_int_field_access_string_value(strlen, &it->field.value.data, &it->err);
}

bool carbon_object_binary_value(carbon_binary *out, carbon_object *it)
{
        return carbon_int_field_access_binary_value(out, &it->field.value.data, &it->err);
}

carbon_array *carbon_object_array_value(carbon_object *it_in)
{
        return carbon_int_field_access_array_value(&it_in->field.value.data, &it_in->err);
}

carbon_object *carbon_object_object_value(carbon_object *it_in)
{
        return carbon_int_field_access_object_value(&it_in->field.value.data, &it_in->err);
}

carbon_column *carbon_object_column_value(carbon_object *it_in)
{
        return carbon_int_field_access_column_value(&it_in->field.value.data, &it_in->err);
}

bool carbon_object_insert_begin(carbon_insert *inserter, carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(inserter)
        DEBUG_ERROR_IF_NULL(it)
        return carbon_int_insert_create_for_object(inserter, it);
}

fn_result carbon_object_insert_end(carbon_insert *inserter)
{
        FN_FAIL_IF_NULL(inserter)
        return carbon_insert_drop(inserter);
}

bool carbon_object_fast_forward(carbon_object *it)
{
        DEBUG_ERROR_IF_NULL(it);
        while (carbon_object_next(it)) {}

        JAK_ASSERT(*memfile_peek(&it->memfile, sizeof(u8)) == CARBON_MOBJECT_END);
        memfile_skip(&it->memfile, sizeof(u8));
        return true;
}