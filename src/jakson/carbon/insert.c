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

#include <jakson/std/uintvar/stream.h>
#include <jakson/carbon/arr-it.h>
#include <jakson/carbon/col-it.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/mime.h>
#include <jakson/carbon/container.h>
#include <jakson/carbon/internal.h>
#include <jakson/carbon/string-field.h>
#include <jakson/carbon/obj-it.h>
#include <jakson/carbon/internal.h>
#include <jakson/utils/numbers.h>

#define check_type_if_container_is_column(in, expr)                                                              \
if (unlikely(in->context_type == COLUMN && !(expr))) {                                            \
        error(ERR_TYPEMISMATCH, "Element type does not match container type");        \
}

#define check_type_range_if_container_is_column(in, expected1, expected2, expected3)                             \
if (unlikely(in->context_type == COLUMN && in->context.column->type != expected1 &&                 \
        in->context.column->type != expected2 && in->context.column->type != expected3)) {                 \
        ERROR_WDETAILS(&in->err, ERR_TYPEMISMATCH, "Element type does not match container type");        \
}

static bool
write_field_data(insert *in, u8 field_marker, const void *base, u64 nbytes);

static bool push_in_column(insert *in, const void *base, field_e type);

static bool push_media_type_for_array(insert *in, field_e type);

static void internal_create(insert *in, memfile *src, offset_t pos);

static void write_binary_blob(insert *in, const void *value, size_t nbytes);

void internal_insert_create_for_array(insert *in, arr_it *context)
{
        in->context_type = ARRAY;
        in->context.array = context;
        in->position = 0;

        offset_t pos = 0;
        if (context->eof) {
                pos = MEMFILE_TELL(&context->file);
        } else {
                pos = context->last_off ? context->last_off : 0;
        }

        internal_create(in, &context->file, pos);
}

bool internal_insert_create_for_column(insert *in, col_it *context)
{
        in->context_type = COLUMN;
        in->context.column = context;
        internal_create(in, &context->file, MEMFILE_TELL(&context->file));
        return true;
}

bool internal_insert_create_for_object(insert *in, obj_it *context)
{
        in->context_type = OBJECT;
        in->context.object = context;

        offset_t pos;
        if (context->eof) {
                pos = MEMFILE_TELL(&context->file);
        } else {
                pos = internal_history_has(&context->history) ? internal_history_peek(&context->history) : 0;
        }

        internal_create(in, &context->file, pos);
        return true;
}

bool insert_null(insert *in)
{
        if (unlikely(in->context_type == COLUMN &&
                !field_is_column_or_subtype(in->context.column->field_type))) {
                error(ERR_TYPEMISMATCH, "Element type does not match container type");
        }

        switch (in->context_type) {
                case ARRAY:
                        return push_media_type_for_array(in, FIELD_NULL);
                case COLUMN: {
                        switch (in->context.column->field_type) {
                                case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U8_SORTED_SET: {
                                        u8 value = U8_NULL;
                                        return push_in_column(in, &value, in->context.column->field_type);
                                }
                                case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U16_SORTED_SET: {
                                        u16 value = U16_NULL;
                                        return push_in_column(in, &value, in->context.column->field_type);
                                }
                                case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U32_SORTED_SET: {
                                        u32 value = U32_NULL;
                                        return push_in_column(in, &value, in->context.column->field_type);
                                }
                                case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_U64_SORTED_SET: {
                                        u64 value = U64_NULL;
                                        return push_in_column(in, &value, in->context.column->field_type);
                                }
                                case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I8_SORTED_SET: {
                                        i8 value = I8_NULL;
                                        return push_in_column(in, &value, in->context.column->field_type);
                                }
                                case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I16_SORTED_SET: {
                                        i16 value = I16_NULL;
                                        return push_in_column(in, &value, in->context.column->field_type);
                                }
                                case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I32_SORTED_SET: {
                                        i32 value = I32_NULL;
                                        return push_in_column(in, &value, in->context.column->field_type);
                                }
                                case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_I64_SORTED_SET: {
                                        i64 value = I64_NULL;
                                        return push_in_column(in, &value, in->context.column->field_type);
                                }
                                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET: {
                                        float value = CARBON_NULL_FLOAT;
                                        return push_in_column(in, &value, in->context.column->field_type);
                                }
                                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                                        u8 value = CARBON_BOOLEAN_COLUMN_NULL;
                                        return push_in_column(in, &value, in->context.column->field_type);
                                }
                                default:
                                        return error(ERR_INTERNALERR, NULL);
                        }
                }
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
}

bool insert_true(insert *in)
{
        check_type_if_container_is_column(in, field_is_column_bool_or_subtype(in->context.column->field_type));
        switch (in->context_type) {
                case ARRAY:
                        return push_media_type_for_array(in, FIELD_TRUE);
                case COLUMN: {
                        u8 value = CARBON_BOOLEAN_COLUMN_TRUE;
                        return push_in_column(in, &value, FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET);
                }
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
}

bool insert_false(insert *in)
{
        check_type_if_container_is_column(in, field_is_column_bool_or_subtype(in->context.column->field_type));
        switch (in->context_type) {
                case ARRAY:
                        return push_media_type_for_array(in, FIELD_FALSE);
                case COLUMN: {
                        u8 value = CARBON_BOOLEAN_COLUMN_FALSE;
                        return push_in_column(in, &value, FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET);
                }
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
}

bool insert_u8(insert *in, u8 value)
{
        check_type_if_container_is_column(in, field_is_column_u8_or_subtype(in->context.column->field_type));
        switch (in->context_type) {
                case ARRAY:
                        write_field_data(in, FIELD_NUMBER_U8, &value, sizeof(u8));
                        break;
                case COLUMN:
                        push_in_column(in, &value, FIELD_COLUMN_U8_UNSORTED_MULTISET);
                        break;
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
        return true;
}

bool insert_u16(insert *in, u16 value)
{
        check_type_if_container_is_column(in, field_is_column_u16_or_subtype(in->context.column->field_type));
        switch (in->context_type) {
                case ARRAY:
                        write_field_data(in, FIELD_NUMBER_U16, &value, sizeof(u16));
                        break;
                case COLUMN:
                        push_in_column(in, &value, FIELD_COLUMN_U16_UNSORTED_MULTISET);
                        break;
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
        return true;
}

bool insert_u32(insert *in, u32 value)
{
        check_type_if_container_is_column(in, field_is_column_u32_or_subtype(in->context.column->field_type));
        switch (in->context_type) {
                case ARRAY:
                        write_field_data(in, FIELD_NUMBER_U32, &value, sizeof(u32));
                        break;
                case COLUMN:
                        push_in_column(in, &value, FIELD_COLUMN_U32_UNSORTED_MULTISET);
                        break;
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
        return true;
}

bool insert_u64(insert *in, u64 value)
{
        check_type_if_container_is_column(in, field_is_column_u64_or_subtype(in->context.column->field_type));
        switch (in->context_type) {
                case ARRAY:
                        write_field_data(in, FIELD_NUMBER_U64, &value, sizeof(u64));
                        break;
                case COLUMN:
                        push_in_column(in, &value, FIELD_COLUMN_U64_UNSORTED_MULTISET);
                        break;
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
        return true;
}

bool insert_i8(insert *in, i8 value)
{
        check_type_if_container_is_column(in, field_is_column_i8_or_subtype(in->context.column->field_type));
        switch (in->context_type) {
                case ARRAY:
                        write_field_data(in, FIELD_NUMBER_I8, &value, sizeof(i8));
                        break;
                case COLUMN:
                        push_in_column(in, &value, FIELD_COLUMN_I8_UNSORTED_MULTISET);
                        break;
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
        return true;
}

bool insert_i16(insert *in, i16 value)
{
        check_type_if_container_is_column(in, field_is_column_i16_or_subtype(in->context.column->field_type));
        switch (in->context_type) {
                case ARRAY:
                        write_field_data(in, FIELD_NUMBER_I16, &value, sizeof(i16));
                        break;
                case COLUMN:
                        push_in_column(in, &value, FIELD_COLUMN_I16_UNSORTED_MULTISET);
                        break;
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
        return true;
}

bool insert_i32(insert *in, i32 value)
{
        check_type_if_container_is_column(in, field_is_column_i32_or_subtype(in->context.column->field_type));
        switch (in->context_type) {
                case ARRAY:
                        write_field_data(in, FIELD_NUMBER_I32, &value, sizeof(i32));
                        break;
                case COLUMN:
                        push_in_column(in, &value, FIELD_COLUMN_I32_UNSORTED_MULTISET);
                        break;
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
        return true;
}

bool insert_i64(insert *in, i64 value)
{
        check_type_if_container_is_column(in, field_is_column_i64_or_subtype(in->context.column->field_type));
        switch (in->context_type) {
                case ARRAY:
                        write_field_data(in, FIELD_NUMBER_I64, &value, sizeof(i64));
                        break;
                case COLUMN:
                        push_in_column(in, &value, FIELD_COLUMN_I64_UNSORTED_MULTISET);
                        break;
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
        return true;
}

bool insert_unsigned(insert *in, u64 value)
{
        error_if_and_return(in->context_type == COLUMN, ERR_INSERT_TOO_DANGEROUS, NULL)

        switch (number_min_type_signed(value)) {
                case NUMBER_I8:
                        return insert_u8(in, (u8) value);
                case NUMBER_I16:
                        return insert_u16(in, (u16) value);
                case NUMBER_I32:
                        return insert_u32(in, (u32) value);
                case NUMBER_I64:
                        return insert_u64(in, (u64) value);
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
}

bool insert_signed(insert *in, i64 value)
{
        error_if_and_return(in->context_type == COLUMN, ERR_INSERT_TOO_DANGEROUS, NULL)

        switch (number_min_type_signed(value)) {
                case NUMBER_I8:
                        return insert_i8(in, (i8) value);
                case NUMBER_I16:
                        return insert_i16(in, (i16) value);
                case NUMBER_I32:
                        return insert_i32(in, (i32) value);
                case NUMBER_I64:
                        return insert_i64(in, (i64) value);
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
}

bool insert_float(insert *in, float value)
{
        check_type_if_container_is_column(in, field_is_column_float_or_subtype(in->context.column->field_type));
        switch (in->context_type) {
                case ARRAY:
                        write_field_data(in, FIELD_NUMBER_FLOAT, &value, sizeof(float));
                        break;
                case COLUMN:
                        push_in_column(in, &value, FIELD_COLUMN_FLOAT_UNSORTED_MULTISET);
                        break;
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
        return true;
}

bool insert_string(insert *in, const char *value)
{
        return insert_nchar(in, value, strlen(value));
}

bool insert_nchar(insert *in, const char *value, u64 value_len)
{
        UNUSED(in);
        UNUSED(value);
        error_if_and_return(in->context_type != ARRAY, ERR_UNSUPPCONTAINER, NULL);

        return string_field_nchar_write(&in->file, value, value_len);
}

static void _insert_binary(insert *in, const void *value, size_t nbytes,
                          const char *file_ext, const char *user_type)
{
        if (user_type && strlen(user_type) > 0) {
                /** write media type 'user binary' */
                push_media_type_for_array(in, FIELD_BINARY_CUSTOM);

                /** write length of 'user_type' str_buf with variable-length integer type */
                u64 user_type_strlen = strlen(user_type);

                memfile_write_uintvar_stream(NULL, &in->file, user_type_strlen);

                /** write 'user_type' str_buf */
                memfile_ensure_space(&in->file, user_type_strlen);
                MEMFILE_WRITE(&in->file, user_type, user_type_strlen);

                /** write binary blob */
                write_binary_blob(in, value, nbytes);

        } else {
                /** write media type 'binary' */
                push_media_type_for_array(in, FIELD_BINARY);

                /** write mime type with variable-length integer type */
                u64 mime_id = mime_by_ext(file_ext);

                /** write mime type id */
                memfile_write_uintvar_stream(NULL, &in->file, mime_id);

                /** write binary blob */
                write_binary_blob(in, value, nbytes);
        }
}

bool insert_binary(insert *in, const void *value, size_t nbytes,
                          const char *file_ext, const char *user_type)
{
        error_if_and_return(in->context_type != ARRAY, ERR_UNSUPPCONTAINER, NULL);
        _insert_binary(in, value, nbytes, file_ext, user_type);
        return true;
}

insert *__insert_map_begin(obj_state *out,
                                                  insert *in, map_type_e derivation,
                                                  u64 object_capacity)
{
        error_if_and_return(!out, ERR_NULLPTR, NULL);
        if (!in) {
                error(ERR_NULLPTR, NULL);
                return false;
        }

        *out = (obj_state) {
                .parent = in,
                .it = MALLOC(sizeof(obj_it)),
                .begin = MEMFILE_TELL(&in->file),
                .end = 0
        };


        internal_insert_object(&in->file, derivation, object_capacity);
        u64 payload_start = MEMFILE_TELL(&in->file) - 1;

        internal_obj_it_create(out->it, &in->file, payload_start);
        internal_obj_it_insert_begin(&out->in, out->it);

        return &out->in;
}

insert *insert_object_begin(obj_state *out,
                                                  insert *in,
                                                  u64 object_capacity)
{
        return __insert_map_begin(out, in, MAP_UNSORTED_MULTIMAP, object_capacity);
}

bool insert_object_end(obj_state *state)
{
        obj_it scan;
        internal_obj_it_create(&scan, &state->parent->file, MEMFILE_TELL(&state->parent->file) - 1);
        while (obj_it_next(&scan)) {}

        assert(*MEMFILE_PEEK(&scan.file, sizeof(char)) == MOBJECT_END);
        memfile_read(&scan.file, sizeof(char));

        state->end = MEMFILE_TELL(&scan.file);

        memfile_skip(&scan.file, 1);

        MEMFILE_SEEK(&state->parent->file, MEMFILE_TELL(&scan.file) - 1);
        obj_it_drop(&scan);
        obj_it_drop(state->it);
        free(state->it);
        return true;
}

insert *insert_object_map_begin(obj_state *out, insert *in,
                                              map_type_e derivation, u64 object_capacity)
{
        return __insert_map_begin(out, in, derivation, object_capacity);
}

bool insert_object_map_end(obj_state *state)
{
        return insert_object_end(state);
}

insert *__insert_array_list_begin(arr_state *state_out,
                                                 insert *inserter_in, list_type_e derivation, u64 array_capacity)
{
        error_if_and_return(!state_out, ERR_NULLPTR, NULL);
        if (!inserter_in) {
                error(ERR_NULLPTR, NULL);
                return false;
        }

        error_if_and_return(inserter_in->context_type != ARRAY && inserter_in->context_type != OBJECT,
                     ERR_UNSUPPCONTAINER, NULL);

        *state_out = (arr_state) {
                .parent = inserter_in,
                .array = MALLOC(sizeof(arr_it)),
                .begin = MEMFILE_TELL(&inserter_in->file),
                .end = 0
        };

        internal_insert_array(&inserter_in->file, derivation, array_capacity);
        u64 payload_start = MEMFILE_TELL(&inserter_in->file) - 1;

        internal_arr_it_create(state_out->array, &inserter_in->file, payload_start);
        arr_it_insert_begin(&state_out->nested, state_out->array);

        return &state_out->nested;
}

insert *insert_array_begin(arr_state *state_out,
                                                    insert *inserter_in, u64 array_capacity)
{
        return __insert_array_list_begin(state_out, inserter_in, LIST_UNSORTED_MULTISET, array_capacity);
}

bool insert_array_end(arr_state *state_in)
{
        arr_it scan;
        internal_arr_it_create(&scan, &state_in->parent->file,
                               MEMFILE_TELL(&state_in->parent->file) - 1);

        internal_arr_it_fast_forward(&scan);

        state_in->end = MEMFILE_TELL(&scan.file);
        memfile_skip(&scan.file, 1);

        MEMFILE_SEEK(&state_in->parent->file, MEMFILE_TELL(&scan.file) - 1);
        arr_it_drop(&scan);
        arr_it_drop(state_in->array);
        free(state_in->array);
        return true;
}

insert *insert_array_list_begin(arr_state *state_out, insert *inserter_in, list_type_e derivation, u64 array_capacity)
{
        return __insert_array_list_begin(state_out, inserter_in, derivation, array_capacity);
}

bool insert_array_list_end(arr_state *state_in)
{
        return insert_array_end(state_in);
}


insert *__insert_column_list_begin(col_state *state_out,
                                                     insert *inserter_in,
                                                     list_type_e derivation,
                                                     col_it_type_e type,
                                                     u64 cap)
{
        error_if_and_return(!state_out, ERR_NULLPTR, NULL);
        error_if_and_return(!inserter_in, ERR_NULLPTR, NULL);
        error_if_and_return(inserter_in->context_type != ARRAY && inserter_in->context_type != OBJECT,
                     ERR_UNSUPPCONTAINER, NULL);

        field_e field_type = field_for_column(derivation, type);

        *state_out = (col_state) {
                .parent = inserter_in,
                .nested_column = MALLOC(sizeof(col_it)),
                .type = field_type,
                .begin = MEMFILE_TELL(&inserter_in->file),
                .end = 0
        };

        u64 container_start_off = MEMFILE_TELL(&inserter_in->file);
        internal_insert_column(&inserter_in->file, derivation, type, cap);

        col_it_create(state_out->nested_column, &inserter_in->file,
                                    container_start_off);
        col_it_insert(&state_out->nested, state_out->nested_column);

        return &state_out->nested;
}

insert *insert_column_begin(col_state *state_out,
                                                     insert *inserter_in,
                                                     col_it_type_e type,
                                                     u64 cap)
{
        return __insert_column_list_begin(state_out, inserter_in, LIST_UNSORTED_MULTISET, type, cap);
}

bool insert_column_end(col_state *state_in)
{
        col_it scan;
        col_it_create(&scan, &state_in->parent->file,
                                state_in->nested_column->begin);
        col_it_fast_forward(&scan);

        state_in->end = MEMFILE_TELL(&scan.file);
        MEMFILE_SEEK(&state_in->parent->file, MEMFILE_TELL(&scan.file));

        free(state_in->nested_column);
        return true;
}

insert *insert_column_list_begin(col_state *state_out, insert *inserter_in, list_type_e derivation, col_it_type_e type, u64 cap)
{
        return __insert_column_list_begin(state_out, inserter_in, derivation, type, cap);
}

bool insert_column_list_end(col_state *state_in)
{
        return insert_column_end(state_in);
}

static bool inserter_refresh_mod_size(insert *in, i64 mod_size)
{
        assert(mod_size > 0);

        i64 *target = NULL;
        switch (in->context_type) {
                case OBJECT:
                        target = &in->context.object->mod_size;
                        break;
                case ARRAY:
                        target = &in->context.array->mod_size;
                        break;
                case COLUMN:
                        target = &in->context.column->mod_size;
                        break;
                default: error(ERR_UNSUPPCONTAINER, NULL);
                        return false;
        }
        *target += mod_size;
        return true;
}

bool insert_prop_null(insert *in, const char *key)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        offset_t prop_start = MEMFILE_TELL(&in->file);
        string_field_nomarker_write(&in->file, key);
        push_media_type_for_array(in, FIELD_NULL);
        offset_t prop_end = MEMFILE_TELL(&in->file);
        inserter_refresh_mod_size(in, prop_end - prop_start);
        return true;
}

bool insert_prop_true(insert *in, const char *key)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        offset_t prop_start = MEMFILE_TELL(&in->file);
        string_field_nomarker_write(&in->file, key);
        push_media_type_for_array(in, FIELD_TRUE);
        offset_t prop_end = MEMFILE_TELL(&in->file);
        inserter_refresh_mod_size(in, prop_end - prop_start);
        return true;
}

bool insert_prop_false(insert *in, const char *key)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        offset_t prop_start = MEMFILE_TELL(&in->file);
        string_field_nomarker_write(&in->file, key);
        push_media_type_for_array(in, FIELD_FALSE);
        offset_t prop_end = MEMFILE_TELL(&in->file);
        inserter_refresh_mod_size(in, prop_end - prop_start);
        return true;
}

bool insert_prop_u8(insert *in, const char *key, u8 value)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        offset_t prop_start = MEMFILE_TELL(&in->file);
        string_field_nomarker_write(&in->file, key);
        write_field_data(in, FIELD_NUMBER_U8, &value, sizeof(u8));
        offset_t prop_end = MEMFILE_TELL(&in->file);
        inserter_refresh_mod_size(in, prop_end - prop_start);
        return true;
}

bool insert_prop_u16(insert *in, const char *key, u16 value)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        offset_t prop_start = MEMFILE_TELL(&in->file);
        string_field_nomarker_write(&in->file, key);
        write_field_data(in, FIELD_NUMBER_U16, &value, sizeof(u16));
        offset_t prop_end = MEMFILE_TELL(&in->file);
        inserter_refresh_mod_size(in, prop_end - prop_start);
        return true;
}

bool insert_prop_u32(insert *in, const char *key, u32 value)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        offset_t prop_start = MEMFILE_TELL(&in->file);
        string_field_nomarker_write(&in->file, key);
        write_field_data(in, FIELD_NUMBER_U32, &value, sizeof(u32));
        offset_t prop_end = MEMFILE_TELL(&in->file);
        inserter_refresh_mod_size(in, prop_end - prop_start);
        return true;
}

bool insert_prop_u64(insert *in, const char *key, u64 value)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        offset_t prop_start = MEMFILE_TELL(&in->file);
        string_field_nomarker_write(&in->file, key);
        write_field_data(in, FIELD_NUMBER_U64, &value, sizeof(u64));
        offset_t prop_end = MEMFILE_TELL(&in->file);
        inserter_refresh_mod_size(in, prop_end - prop_start);
        return true;
}

bool insert_prop_i8(insert *in, const char *key, i8 value)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        offset_t prop_start = MEMFILE_TELL(&in->file);
        string_field_nomarker_write(&in->file, key);
        write_field_data(in, FIELD_NUMBER_I8, &value, sizeof(i8));
        offset_t prop_end = MEMFILE_TELL(&in->file);
        inserter_refresh_mod_size(in, prop_end - prop_start);
        return true;
}

bool insert_prop_i16(insert *in, const char *key, i16 value)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        offset_t prop_start = MEMFILE_TELL(&in->file);
        string_field_nomarker_write(&in->file, key);
        write_field_data(in, FIELD_NUMBER_I16, &value, sizeof(i16));
        offset_t prop_end = MEMFILE_TELL(&in->file);
        inserter_refresh_mod_size(in, prop_end - prop_start);
        return true;
}

bool insert_prop_i32(insert *in, const char *key, i32 value)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        offset_t prop_start = MEMFILE_TELL(&in->file);
        string_field_nomarker_write(&in->file, key);
        write_field_data(in, FIELD_NUMBER_I32, &value, sizeof(i32));
        offset_t prop_end = MEMFILE_TELL(&in->file);
        inserter_refresh_mod_size(in, prop_end - prop_start);
        return true;
}

bool insert_prop_i64(insert *in, const char *key, i64 value)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        offset_t prop_start = MEMFILE_TELL(&in->file);
        string_field_nomarker_write(&in->file, key);
        write_field_data(in, FIELD_NUMBER_I64, &value, sizeof(i64));
        offset_t prop_end = MEMFILE_TELL(&in->file);
        inserter_refresh_mod_size(in, prop_end - prop_start);
        return true;
}

bool insert_prop_unsigned(insert *in, const char *key, u64 value)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL)

        switch (number_min_type_unsigned(value)) {
                case NUMBER_U8:
                        return insert_prop_u8(in, key, (u8) value);
                case NUMBER_U16:
                        return insert_prop_u16(in, key, (u16) value);
                case NUMBER_U32:
                        return insert_prop_u32(in, key, (u32) value);
                case NUMBER_U64:
                        return insert_prop_u64(in, key, (u64) value);
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
}

bool insert_prop_signed(insert *in, const char *key, i64 value)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL)

        switch (number_min_type_signed(value)) {
                case NUMBER_I8:
                        return insert_prop_i8(in, key, (i8) value);
                case NUMBER_I16:
                        return insert_prop_i16(in, key, (i16) value);
                case NUMBER_I32:
                        return insert_prop_i32(in, key, (i32) value);
                case NUMBER_I64:
                        return insert_prop_i64(in, key, (i64) value);
                default: error(ERR_INTERNALERR, NULL);
                        return false;
        }
}

bool insert_prop_float(insert *in, const char *key, float value)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        offset_t prop_start = MEMFILE_TELL(&in->file);
        string_field_nomarker_write(&in->file, key);
        write_field_data(in, FIELD_NUMBER_FLOAT, &value, sizeof(float));
        offset_t prop_end = MEMFILE_TELL(&in->file);
        inserter_refresh_mod_size(in, prop_end - prop_start);
        return true;
}

bool insert_prop_string(insert *in, const char *key, const char *value)
{
        return insert_prop_nchar(in, key, value, strlen(value));
}

bool insert_prop_nchar(insert *in, const char *key, const char *value, u64 value_len)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        offset_t prop_start = MEMFILE_TELL(&in->file);
        string_field_nomarker_write(&in->file, key);
        string_field_nchar_write(&in->file, value, value_len);
        offset_t prop_end = MEMFILE_TELL(&in->file);
        inserter_refresh_mod_size(in, prop_end - prop_start);
        return true;
}

bool insert_prop_binary(insert *in, const char *key, const void *value,
                               size_t nbytes, const char *file_ext, const char *user_type)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        offset_t prop_start = MEMFILE_TELL(&in->file);
        string_field_nomarker_write(&in->file, key);
        _insert_binary(in, value, nbytes, file_ext, user_type);
        offset_t prop_end = MEMFILE_TELL(&in->file);
        inserter_refresh_mod_size(in, prop_end - prop_start);
        return true;
}

static insert *__insert_prop_object_container_begin(obj_state *out,
                                                       insert *in, map_type_e derivation, const char *key,
                                                       u64 object_capacity)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        string_field_nomarker_write(&in->file, key);
        return insert_object_map_begin(out, in, derivation, object_capacity);
}

insert *insert_prop_object_begin(obj_state *out,
                                                          insert *in, const char *key,
                                                          u64 object_capacity)
{
        return __insert_prop_object_container_begin(out, in, MAP_UNSORTED_MULTIMAP,
                                                           key, object_capacity);
}

u64 insert_prop_object_end(obj_state *state)
{
        insert_object_end(state);
        return state->end - state->begin;
}

insert *insert_prop_map_begin(obj_state *out, insert *in, map_type_e derivation, const char *key, u64 object_capacity)
{
        return __insert_prop_object_container_begin(out, in, derivation,
                                                           key, object_capacity);
}

u64 insert_prop_map_end(obj_state *state)
{
        return insert_prop_object_end(state);
}

insert *insert_prop_array_begin(arr_state *state,
                                                         insert *in, const char *key,
                                                         u64 array_capacity)
{
        error_if_and_return(in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        string_field_nomarker_write(&in->file, key);
        return insert_array_begin(state, in, array_capacity);
}

u64 insert_prop_array_end(arr_state *state)
{
        insert_array_end(state);
        return state->end - state->begin;
}

insert *insert_prop_column_begin(col_state *state_out,
                                                          insert *inserter_in, const char *key,
                                                          col_it_type_e type, u64 cap)
{
        error_if_and_return(inserter_in->context_type != OBJECT, ERR_UNSUPPCONTAINER, NULL);
        string_field_nomarker_write(&inserter_in->file, key);
        return insert_column_begin(state_out, inserter_in, type, cap);
}

u64 insert_prop_column_end(col_state *state_in)
{
        insert_column_end(state_in);
        return state_in->end - state_in->begin;
}

static bool
write_field_data(insert *in, u8 field_marker, const void *base, u64 nbytes)
{
        assert(in->context_type == ARRAY || in->context_type == OBJECT);

        memfile_ensure_space(&in->file, sizeof(u8) + nbytes);
        MEMFILE_WRITE(&in->file, &field_marker, sizeof(u8));
        return MEMFILE_WRITE(&in->file, base, nbytes);
}

static bool push_in_column(insert *in, const void *base, field_e type)
{
        assert(in->context_type == COLUMN);

        size_t type_size = internal_get_type_value_size(type);

        memfile_save_position(&in->file);

        // Increase element counter
        MEMFILE_SEEK(&in->file, in->context.column->header_begin);
        u32 num_elems = memfile_peek_uintvar_stream(NULL, &in->file);
        num_elems++;
        memfile_update_uintvar_stream(&in->file, num_elems);
        in->context.column->num = num_elems;

        u32 capacity = MEMFILE_READ_UINTVAR_STREAM(NULL, &in->file);

        if (unlikely(num_elems > capacity)) {
                memfile_save_position(&in->file);

                u32 new_capacity = (capacity + 1) * 1.7f;

                // Update capacity counter
                MEMFILE_SEEK(&in->file, in->context.column->header_begin);
                memfile_skip_uintvar_stream(&in->file); // skip num element counter
                memfile_update_uintvar_stream(&in->file, new_capacity);
                in->context.column->cap = new_capacity;

                size_t payload_start = internal_column_get_payload_off(in->context.column);
                MEMFILE_SEEK(&in->file, payload_start + (num_elems - 1) * type_size);
                memfile_ensure_space(&in->file, (new_capacity - capacity) * type_size);

                memfile_restore_position(&in->file);
        }

        size_t payload_start = internal_column_get_payload_off(in->context.column);
        MEMFILE_SEEK(&in->file, payload_start + (num_elems - 1) * type_size);
        MEMFILE_WRITE(&in->file, base, type_size);

        memfile_restore_position(&in->file);
        return true;
}

static bool push_media_type_for_array(insert *in, field_e type)
{
        memfile_ensure_space(&in->file, sizeof(media_type));
        return mime_write(&in->file, type);
}

static void internal_create(insert *in, memfile *src, offset_t pos)
{
        memfile_clone(&in->file, src);
        in->position = pos ? pos : MEMFILE_TELL(src);
        MEMFILE_SEEK(&in->file, in->position);
}

static void write_binary_blob(insert *in, const void *value, size_t nbytes)
{
        /** write blob length */
        memfile_write_uintvar_stream(NULL, &in->file, nbytes);

        /** write blob */
        memfile_ensure_space(&in->file, nbytes);
        MEMFILE_WRITE(&in->file, value, nbytes);
}