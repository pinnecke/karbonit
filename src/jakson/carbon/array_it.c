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
#include <jakson/carbon.h>
#include <jakson/carbon/array_it.h>
#include <jakson/carbon/column_it.h>
#include <jakson/carbon/object_it.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/mime.h>
#include <jakson/carbon/internal.h>

#define DEFINE_IN_PLACE_UPDATE_FUNCTION(type_name, field_type)                                                         \
bool carbon_int_array_update_##type_name(carbon_array *it, type_name value)                \
{                                                                                                                      \
        offset_t datum = 0;                                                                                                \
        DEBUG_ERROR_IF_NULL(it);                                                                                             \
        if (LIKELY(it->field_access.it_field_type == field_type)) {                                                    \
                memfile_save_position(&it->memfile);                                                                   \
                carbon_int_array_it_offset(&datum, it);                                                                 \
                memfile_seek(&it->memfile, datum + sizeof(u8));                                                        \
                memfile_write(&it->memfile, &value, sizeof(type_name));                                                \
                memfile_restore_position(&it->memfile);                                                                \
                return true;                                                                                           \
        } else {                                                                                                       \
                ERROR(&it->err, ERR_TYPEMISMATCH);                                                                 \
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

static bool update_in_place_constant(carbon_array *it, carbon_constant_e constant)
{
        DEBUG_ERROR_IF_NULL(it);

        memfile_save_position(&it->memfile);

        if (carbon_field_type_is_constant(it->field_access.it_field_type)) {
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
                        default: ERROR(&it->err, ERR_INTERNALERR);
                                break;
                }
                offset_t datum = 0;
                carbon_int_array_it_offset(&datum, it);
                memfile_seek(&it->memfile, datum);
                memfile_write(&it->memfile, &value, sizeof(u8));
        } else {
                carbon_insert ins;
                carbon_array_it_remove(it);
                carbon_array_it_insert_begin(&ins, it);

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
                        default: ERROR(&it->err, ERR_INTERNALERR);
                                break;
                }

                carbon_array_it_insert_end(&ins);
        }

        memfile_restore_position(&it->memfile);
        return true;
}

bool carbon_int_array_update_true(carbon_array *it)
{
        return update_in_place_constant(it, CARBON_CONSTANT_TRUE);
}

bool carbon_int_array_update_false(carbon_array *it)
{
        return update_in_place_constant(it, CARBON_CONSTANT_FALSE);
}

bool carbon_int_array_update_null(carbon_array *it)
{
        return update_in_place_constant(it, CARBON_CONSTANT_NULL);
}

static void __carbon_array_it_load_abstract_type(carbon_array *it)
{
        carbon_abstract_type_class_e type_class;
        carbon_abstract_get_class(&type_class, &it->memfile);
        carbon_abstract_class_to_list_derivable(&it->abstract_type, type_class);
}

fn_result carbon_array_it_create(carbon_array *it, memfile *memfile, err *err,
                            offset_t payload_start)
{
        FN_FAIL_IF_NULL(it, memfile, err);

        ZERO_MEMORY(it, sizeof(carbon_array));

        it->array_begin_off = payload_start;
        it->mod_size = 0;
        it->array_end_reached = false;
        it->field_offset = 0;

        error_init(&it->err);
        vector_create(&it->history, NULL, sizeof(offset_t), 40);
        memfile_open(&it->memfile, memfile->memblock, memfile->mode);
        memfile_seek(&it->memfile, payload_start);

        ERROR_IF(memfile_remain_size(&it->memfile) < sizeof(u8), err, ERR_CORRUPTED);

        fn_result ofType(bool) instance = carbon_abstract_is_instanceof_array(&it->memfile);
        if (!FN_IS_OK(instance)) {
                return FN_FAIL_FORWARD();
        } else {
                if (!FN_BOOL(instance)) {
                        return FN_FAIL(ERR_MARKERMAPPING, "expected array or sub type marker");
                }
        }

        __carbon_array_it_load_abstract_type(it);

        memfile_skip(&it->memfile, sizeof(u8));

        carbon_int_field_access_create(&it->field_access);

        carbon_array_it_rewind(it);

        return FN_OK();
}

bool carbon_array_it_copy(carbon_array *dst, carbon_array *src)
{
        DEBUG_ERROR_IF_NULL(dst);
        DEBUG_ERROR_IF_NULL(src);
        carbon_array_it_create(dst, &src->memfile, &src->err, src->array_begin_off);
        return true;
}

bool carbon_array_it_clone(carbon_array *dst, carbon_array *src)
{
        memfile_clone(&dst->memfile, &src->memfile);
        dst->array_begin_off = src->array_begin_off;
        error_cpy(&dst->err, &src->err);
        dst->mod_size = src->mod_size;
        dst->array_end_reached = src->array_end_reached;
        dst->abstract_type = src->abstract_type;
        vector_cpy(&dst->history, &src->history);
        carbon_int_field_access_clone(&dst->field_access, &src->field_access);
        dst->field_offset = src->field_offset;
        return true;
}

bool carbon_array_it_set_mode(carbon_array *it, access_mode_e mode)
{
        DEBUG_ERROR_IF_NULL(it);
        it->memfile.mode = mode;
        return true;
}

bool carbon_array_it_length(u64 *len, carbon_array *it)
{
        DEBUG_ERROR_IF_NULL(len)
        DEBUG_ERROR_IF_NULL(it)

        u64 num_elem = 0;
        carbon_array_it_rewind(it);
        while (carbon_array_it_next(it)) {
                num_elem++;
        }
        *len = num_elem;

        return true;
}

bool carbon_array_it_is_empty(carbon_array *it)
{
        carbon_array_it_rewind(it);
        return carbon_array_it_next(it);
}

fn_result carbon_array_it_drop(carbon_array *it)
{
        carbon_int_field_auto_close(&it->field_access);
        carbon_int_field_access_drop(&it->field_access);
        vector_drop(&it->history);
        return FN_OK();
}

bool carbon_array_it_rewind(carbon_array *it)
{
        DEBUG_ERROR_IF_NULL(it);
        ERROR_IF(it->array_begin_off >= memfile_size(&it->memfile), &it->err, ERR_OUTOFBOUNDS);
        carbon_int_history_clear(&it->history);
        return memfile_seek(&it->memfile, it->array_begin_off + sizeof(u8));
}

static void auto_adjust_pos_after_mod(carbon_array *it)
{
        if (carbon_int_field_access_object_it_opened(&it->field_access)) {
                memfile_skip(&it->memfile, it->field_access.nested_object_it->mod_size);
        } else if (carbon_int_field_access_array_it_opened(&it->field_access)) {
                //memfile_skip(&it->mem, it->field_access.nested_array_it->mod_size);
                //abort(); // TODO: implement!
        }
}

bool carbon_array_it_has_next(carbon_array *it)
{
        bool has_next = carbon_array_it_next(it);
        carbon_array_it_prev(it);
        return has_next;
}

bool carbon_array_it_is_unit(carbon_array *it)
{
        bool has_next = carbon_array_it_next(it);
        if (has_next) {
                has_next = carbon_array_it_next(it);
                carbon_array_it_prev(it);
                carbon_array_it_prev(it);
                return !has_next;
        }
        carbon_array_it_prev(it);
        return false;
}

bool carbon_array_it_next(carbon_array *it)
{
        DEBUG_ERROR_IF_NULL(it);
        bool is_empty_slot = true;

        auto_adjust_pos_after_mod(it);
        offset_t last_off = memfile_tell(&it->memfile);

        if (carbon_int_array_it_next(&is_empty_slot, &it->array_end_reached, it)) {
                carbon_int_history_push(&it->history, last_off);
                return true;
        } else {
                /** skip remaining zeros until end of array is reached */
                if (!it->array_end_reached) {
                        ERROR_IF(!is_empty_slot, &it->err, ERR_CORRUPTED);

                        while (*memfile_peek(&it->memfile, 1) == 0) {
                                memfile_skip(&it->memfile, 1);
                        }
                }
                JAK_ASSERT(*memfile_peek(&it->memfile, sizeof(char)) == CARBON_MARRAY_END);
                carbon_int_field_auto_close(&it->field_access);
                return false;
        }
}

bool carbon_array_it_prev(carbon_array *it)
{
        DEBUG_ERROR_IF_NULL(it);
        if (carbon_int_history_has(&it->history)) {
                offset_t prev_off = carbon_int_history_pop(&it->history);
                memfile_seek(&it->memfile, prev_off);
                return carbon_int_array_it_refresh(NULL, NULL, it);
        } else {
                return false;
        }
}

offset_t carbon_array_it_memfilepos(carbon_array *it)
{
        if (LIKELY(it != NULL)) {
                return memfile_tell(&it->memfile);
        } else {
                ERROR(&it->err, ERR_NULLPTR);
                return 0;
        }
}

offset_t carbon_array_it_tell(carbon_array *it)
{
        return it ? it->field_offset : 0;
}

bool carbon_int_array_it_offset(offset_t *off, carbon_array *it)
{
        DEBUG_ERROR_IF_NULL(off)
        DEBUG_ERROR_IF_NULL(it)
        if (carbon_int_history_has(&it->history)) {
                *off = carbon_int_history_peek(&it->history);
                return true;
        }
        return false;
}

bool carbon_array_it_fast_forward(carbon_array *it)
{
        DEBUG_ERROR_IF_NULL(it);
        while (carbon_array_it_next(it)) {}

        JAK_ASSERT(*memfile_peek(&it->memfile, sizeof(char)) == CARBON_MARRAY_END);
        memfile_skip(&it->memfile, sizeof(char));
        return true;
}

bool carbon_array_it_field_type(carbon_field_type_e *type, carbon_array *it)
{
        return carbon_int_field_access_field_type(type, &it->field_access);
}

bool carbon_array_it_bool_value(bool *value, carbon_array *it)
{
        return carbon_int_field_access_bool_value(value, &it->field_access, &it->err);
}

bool carbon_array_it_is_null(bool *is_null, carbon_array *it)
{
        return carbon_int_field_access_is_null(is_null, &it->field_access);
}

bool carbon_array_it_u8_value(u8 *value, carbon_array *it)
{
        return carbon_int_field_access_u8_value(value, &it->field_access, &it->err);
}

bool carbon_array_it_u16_value(u16 *value, carbon_array *it)
{
        return carbon_int_field_access_u16_value(value, &it->field_access, &it->err);
}

bool carbon_array_it_u32_value(u32 *value, carbon_array *it)
{
        return carbon_int_field_access_u32_value(value, &it->field_access, &it->err);
}

bool carbon_array_it_u64_value(u64 *value, carbon_array *it)
{
        return carbon_int_field_access_u64_value(value, &it->field_access, &it->err);
}

bool carbon_array_it_i8_value(i8 *value, carbon_array *it)
{
        return carbon_int_field_access_i8_value(value, &it->field_access, &it->err);
}

bool carbon_array_it_i16_value(i16 *value, carbon_array *it)
{
        return carbon_int_field_access_i16_value(value, &it->field_access, &it->err);
}

bool carbon_array_it_i32_value(i32 *value, carbon_array *it)
{
        return carbon_int_field_access_i32_value(value, &it->field_access, &it->err);
}

bool carbon_array_it_i64_value(i64 *value, carbon_array *it)
{
        return carbon_int_field_access_i64_value(value, &it->field_access, &it->err);
}

bool carbon_array_it_float_value(float *value, carbon_array *it)
{
        return carbon_int_field_access_float_value(value, &it->field_access, &it->err);
}

bool carbon_array_it_float_value_nullable(bool *is_null_in, float *value, carbon_array *it)
{
        return carbon_int_field_access_float_value_nullable(is_null_in, value, &it->field_access, &it->err);
}

bool carbon_array_it_signed_value(bool *is_null_in, i64 *value, carbon_array *it)
{
        return carbon_int_field_access_signed_value(is_null_in, value, &it->field_access, &it->err);
}

bool carbon_array_it_unsigned_value(bool *is_null_in, u64 *value, carbon_array *it)
{
        return carbon_int_field_access_unsigned_value(is_null_in, value, &it->field_access, &it->err);
}

const char *carbon_array_it_string_value(u64 *strlen, carbon_array *it)
{
        return carbon_int_field_access_string_value(strlen, &it->field_access, &it->err);
}

bool carbon_array_it_binary_value(carbon_binary *out, carbon_array *it)
{
        return carbon_int_field_access_binary_value(out, &it->field_access, &it->err);
}

carbon_array *carbon_array_it_array_value(carbon_array *it_in)
{
        return carbon_int_field_access_array_value(&it_in->field_access, &it_in->err);
}

carbon_object_it *carbon_array_it_object_value(carbon_array *it_in)
{
        return carbon_int_field_access_object_value(&it_in->field_access, &it_in->err);
}

carbon_column_it *carbon_array_it_column_value(carbon_array *it_in)
{
        return carbon_int_field_access_column_value(&it_in->field_access, &it_in->err);
}

fn_result carbon_array_it_insert_begin(carbon_insert *inserter, carbon_array *it)
{
        FN_FAIL_IF_NULL(inserter, it)
        return carbon_int_insert_create_for_array(inserter, it);
}

fn_result carbon_array_it_insert_end(carbon_insert *inserter)
{
        FN_FAIL_IF_NULL(inserter)
        return carbon_insert_drop(inserter);
}

bool carbon_array_it_remove(carbon_array *it)
{
        DEBUG_ERROR_IF_NULL(it);
        carbon_field_type_e type;
        if (carbon_array_it_field_type(&type, it)) {
                offset_t prev_off = carbon_int_history_pop(&it->history);
                memfile_seek(&it->memfile, prev_off);
                if (carbon_int_field_remove(&it->memfile, &it->err, type)) {
                        carbon_int_array_it_refresh(NULL, NULL, it);
                        return true;
                } else {
                        return false;
                }
        } else {
                ERROR(&it->err, ERR_ILLEGALSTATE);
                return false;
        }
}

/** Checks if this array is annotated as a multi set abstract type. Returns true if it is is a multi set, and false if
 * it is a set. In case of any error, a failure is returned. */
fn_result ofType(bool) carbon_array_it_is_multiset(carbon_array *it)
{
        FN_FAIL_IF_NULL(it)
        carbon_abstract_type_class_e type_class;
        carbon_abstract_list_derivable_to_class(&type_class, it->abstract_type);
        return carbon_abstract_is_multiset(type_class);
}

/** Checks if this array is annotated as a sorted abstract type. Returns true if this is the case,
 * otherwise false. In case of any error, a failure is returned. */
fn_result ofType(bool) carbon_array_it_is_sorted(carbon_array *it)
{
        FN_FAIL_IF_NULL(it)
        carbon_abstract_type_class_e type_class;
        carbon_abstract_list_derivable_to_class(&type_class, it->abstract_type);
        return carbon_abstract_is_sorted(type_class);
}

fn_result carbon_array_it_update_type(carbon_array *it, carbon_list_derivable_e derivation)
{
        FN_FAIL_IF_NULL(it)

        memfile_save_position(&it->memfile);
        memfile_seek(&it->memfile, it->array_begin_off);

        carbon_derived_e derive_marker;
        carbon_abstract_derive_list_to(&derive_marker, CARBON_LIST_CONTAINER_ARRAY, derivation);
        carbon_abstract_write_derived_type(&it->memfile, derive_marker);

        memfile_restore_position(&it->memfile);
        return FN_OK();
}