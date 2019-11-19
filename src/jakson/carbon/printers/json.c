/**
 * Copyright 2019 Marcus Pinnecke
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

#include <stdint.h>

#include <jakson/carbon/printers/json.h>
#include <jakson/carbon/traverse.h>
#include <jakson/carbon/array.h>
#include <jakson/carbon/column.h>
#include <jakson/carbon/object.h>
#include <jakson/jakson.h>

#define CARBON_PRINT_JSON_NULL "null"
#define CARBON_PRINT_JSON_TRUE "true"
#define CARBON_PRINT_JSON_FALSE "false"

// ---------------------------------------------------------------------------------------------------------------------
//
//  general-purpose formatter
//
// ---------------------------------------------------------------------------------------------------------------------

static void __carbon_print_json_record(struct carbon_traverse_extra *extra, rec *record)
{
        UNUSED(extra)
        UNUSED(record)
}

static void __carbon_print_json_enter_array(struct carbon_traverse_extra *extra, carbon_array *it)
{
        UNUSED(extra)
        UNUSED(it)
}

static void __carbon_print_json_exit_array(struct carbon_traverse_extra *extra, carbon_array *it)
{
        UNUSED(extra)
        UNUSED(it)
}

static bool __carbon_print_json_column(struct carbon_traverse_extra *extra, struct carbon_column *it)
{
        UNUSED(extra)
        UNUSED(it)
        return false;
}

static void __carbon_print_json_enter_object(struct carbon_traverse_extra *extra, struct carbon_object *it)
{
        UNUSED(extra)
        UNUSED(it)
}

static void __carbon_print_json_exit_object(struct carbon_traverse_extra *extra, struct carbon_object *it)
{
        UNUSED(extra)
        UNUSED(it)
}

// ---------------------------------------------------------------------------------------------------------------------
//
//  specialized printer for fast printing with minimal logic
//
// ---------------------------------------------------------------------------------------------------------------------

static inline void __carbon_print_json_constant(struct string_buffer *restrict buf, const char *restrict literal)
{
        string_buffer_add(buf, literal);
}

static inline void __carbon_print_json_string(struct string_buffer *restrict buf, const char *restrict string, u64 len)
{
        string_buffer_add_char(buf, '"');
        string_buffer_add_nchar(buf, string, len);
        string_buffer_add_char(buf, '"');
}

static inline void __carbon_print_json_binary(struct string_buffer *restrict buf, const void *restrict data, u64 nbyte)
{
        string_buffer_add_char(buf, '[');
        for (register uint_fast64_t i = 0; i < nbyte; i++) {
                const u8 byte = *(const u8 *)(data + nbyte);
                string_buffer_add_u8(buf, byte);
                if (LIKELY(i + 1 < nbyte)) {
                        string_buffer_add_char(buf, ',');
                }
        }
        string_buffer_add_char(buf, ']');
}

//#define DEFINE_CARBON_PRINT_JSON_TYPE_FROM_ARRAY(type)                                                                 \
//static inline void __carbon_print_json_##type##_from_array(struct string_buffer *restrict buf,                         \
//                                                           carbon_array *restrict it)                        \
//{                                                                                                                      \
//        type val;                                                                                                      \
//        internal_carbon_array_##type##_value(&val, it);                                                                      \
//        string_buffer_add_##type(buf, val);                                                                            \
//}
//
//DEFINE_CARBON_PRINT_JSON_TYPE_FROM_ARRAY(u8)
//DEFINE_CARBON_PRINT_JSON_TYPE_FROM_ARRAY(u16)
//DEFINE_CARBON_PRINT_JSON_TYPE_FROM_ARRAY(u32)
//DEFINE_CARBON_PRINT_JSON_TYPE_FROM_ARRAY(u64)
//DEFINE_CARBON_PRINT_JSON_TYPE_FROM_ARRAY(i8)
//DEFINE_CARBON_PRINT_JSON_TYPE_FROM_ARRAY(i16)
//DEFINE_CARBON_PRINT_JSON_TYPE_FROM_ARRAY(i32)
//DEFINE_CARBON_PRINT_JSON_TYPE_FROM_ARRAY(i64)
//DEFINE_CARBON_PRINT_JSON_TYPE_FROM_ARRAY(float)

#define DEFINE_CARBON_PRINT_JSON_TYPE_FROM_PROP_VALUE(type, clazz, default_value)                                                            \
static inline void __carbon_print_json_##type##_from_prop_value(struct string_buffer *restrict buf,                    \
                                                                struct carbon_object *restrict it)                     \
{                                                                                                                      \
        type val = (type) carbon_item_get_##clazz(&(it->prop.value), default_value);                                                    \
        string_buffer_add_##type(buf, val);                                                                            \
}

DEFINE_CARBON_PRINT_JSON_TYPE_FROM_PROP_VALUE(u8, number_unsigned, CARBON_NULL_UNSIGNED)
DEFINE_CARBON_PRINT_JSON_TYPE_FROM_PROP_VALUE(u16, number_unsigned, CARBON_NULL_UNSIGNED)
DEFINE_CARBON_PRINT_JSON_TYPE_FROM_PROP_VALUE(u32, number_unsigned, CARBON_NULL_UNSIGNED)
DEFINE_CARBON_PRINT_JSON_TYPE_FROM_PROP_VALUE(u64, number_unsigned, CARBON_NULL_UNSIGNED)
DEFINE_CARBON_PRINT_JSON_TYPE_FROM_PROP_VALUE(i8, number_signed, CARBON_NULL_SIGNED)
DEFINE_CARBON_PRINT_JSON_TYPE_FROM_PROP_VALUE(i16, number_signed, CARBON_NULL_SIGNED)
DEFINE_CARBON_PRINT_JSON_TYPE_FROM_PROP_VALUE(i32, number_signed, CARBON_NULL_SIGNED)
DEFINE_CARBON_PRINT_JSON_TYPE_FROM_PROP_VALUE(i64, number_signed, CARBON_NULL_SIGNED)
DEFINE_CARBON_PRINT_JSON_TYPE_FROM_PROP_VALUE(float, number_float, CARBON_NULL_FLOAT)

static inline void __carbon_print_json_enter_array_fast(struct carbon_traverse_extra *restrict extra,
                                                        carbon_array *restrict it)
{
        struct string_buffer *str_buf = extra->capture.print_json.str;
        field_type_e type;
        //const char *string;
        //carbon_binary binary;
        //u64 string_len;

        if (UNLIKELY(!carbon_array_has_next(it))) {
                __carbon_print_json_constant(str_buf, CARBON_PRINT_JSON_NULL);
                extra->capture.print_json.convert = CARBON_PRINT_JSON_CONVERT_TO_NULL;
                return;
        } else if (LIKELY(!carbon_array_is_unit(it))) {
                extra->capture.print_json.convert = CARBON_PRINT_JSON_CONVERT_REMAIN;
                string_buffer_add(str_buf, "[");
        } else {
                extra->capture.print_json.convert = CARBON_PRINT_JSON_CONVERT_TO_ELEMENT;
        }

        char sep = '\0';

        while (carbon_array_next(it)) {

                string_buffer_add_char(str_buf, sep);
                sep = ',';

                carbon_array_field_type(&type, it);

//                switch (type) {
//                        case CARBON_FIELD_NULL:
//                                __carbon_print_json_constant(str_buf, CARBON_PRINT_JSON_NULL);
//                                break;
//                        case CARBON_FIELD_TRUE:
//                                __carbon_print_json_constant(str_buf, CARBON_PRINT_JSON_TRUE);
//                                break;
//                        case CARBON_FIELD_FALSE:
//                                __carbon_print_json_constant(str_buf, CARBON_PRINT_JSON_FALSE);
//                                break;
//                        case CARBON_FIELD_STRING:
//                                string = internal_carbon_array_string_value(&string_len, it);
//                                __carbon_print_json_string(str_buf, string, string_len);
//                                break;
//                        case CARBON_FIELD_NUMBER_U8:
//                                __carbon_print_json_u8_from_array(str_buf, it);
//                                break;
//                        case CARBON_FIELD_NUMBER_U16:
//                                __carbon_print_json_u16_from_array(str_buf, it);
//                                break;
//                        case CARBON_FIELD_NUMBER_U32:
//                                __carbon_print_json_u32_from_array(str_buf, it);
//                                break;
//                        case CARBON_FIELD_NUMBER_U64:
//                                __carbon_print_json_u64_from_array(str_buf, it);
//                                break;
//                        case CARBON_FIELD_NUMBER_I8:
//                                __carbon_print_json_i8_from_array(str_buf, it);
//                                break;
//                        case CARBON_FIELD_NUMBER_I16:
//                                __carbon_print_json_i16_from_array(str_buf, it);
//                                break;
//                        case CARBON_FIELD_NUMBER_I32:
//                                __carbon_print_json_i32_from_array(str_buf, it);
//                                break;
//                        case CARBON_FIELD_NUMBER_I64:
//                                __carbon_print_json_i64_from_array(str_buf, it);
//                                break;
//                        case CARBON_FIELD_NUMBER_FLOAT:
//                                __carbon_print_json_float_from_array(str_buf, it);
//                                break;
//                        case CARBON_FIELD_BINARY:
//                        case CARBON_FIELD_BINARY_CUSTOM:
//                                internal_carbon_array_binary_value(&binary, it);
//                                __carbon_print_json_binary(str_buf, binary.blob, binary.blob_len);
//                                break;
//                        default:
//                                break;
//                }

                if (carbon_field_type_is_object_or_subtype(type)) {
                        carbon_object *sub = carbon_item_get_object(&(it->item));
                        carbon_traverse_continue_object(extra, sub);
                } else if (carbon_field_type_is_column_or_subtype(type)) {
                        carbon_column *sub = carbon_item_get_column(&(it->item));
                        carbon_traverse_continue_column(extra, sub);
                } else if (carbon_field_type_is_array_or_subtype(type)) {
                        carbon_array *sub = carbon_item_get_array(&(it->item));
                        carbon_traverse_continue_array(extra, sub);
                }
        }
}

static inline void __carbon_print_json_exit_array_fast(struct carbon_traverse_extra *restrict extra,
                                                       carbon_array *restrict it)
{
        UNUSED(it)
        struct string_buffer *str_buf = extra->capture.print_json.str;

        if (extra->capture.print_json.convert == CARBON_PRINT_JSON_CONVERT_REMAIN) {
                string_buffer_add(str_buf, "]");
        }
}

#define CARBON_PRINT_JSON_COLUMN_VALUES(str_buf, type, base, num_elems, sep, null_test)                                \
{                                                                                                                      \
        const type *values = (const type *) base;                                                                      \
        for (register uint_fast32_t i = 0; i < num_elems; i++) {                                                       \
                string_buffer_add_char(str_buf, sep);                                                                  \
                const type elem = values[i];                                                                           \
                if (UNLIKELY(null_test(elem))) {                                                                       \
                        string_buffer_add(str_buf, CARBON_PRINT_JSON_NULL);                                            \
                } else {                                                                                               \
                        string_buffer_add_##type(str_buf, values[i]);                                                  \
                }                                                                                                      \
                sep = ',';                                                                                             \
        }                                                                                                              \
}

static inline bool __carbon_print_json_column_fast(struct carbon_traverse_extra *restrict extra,
                                                   struct carbon_column *restrict it)
{
        struct string_buffer *str_buf = extra->capture.print_json.str;

        uint_fast32_t num_elems;
        enum carbon_field_type type;
        carbon_list_container_e container_type;

        char sep = '\0';
        const void *base = carbon_column_values(&type, &num_elems, it);
        carbon_list_container_type_by_column_type(&container_type, type);

        if (UNLIKELY(num_elems == 0)) {
                __carbon_print_json_constant(str_buf, CARBON_PRINT_JSON_NULL);
                return false;
        } else if (LIKELY(num_elems > 1)) {
                string_buffer_add(str_buf, "[");
        }

        switch (container_type) {
                case LIST_CONTAINER_COLUMN_U8:
                        CARBON_PRINT_JSON_COLUMN_VALUES(str_buf, u8, base, num_elems, sep, IS_NULL_U8)
                        break;
                case LIST_CONTAINER_COLUMN_U16:
                        CARBON_PRINT_JSON_COLUMN_VALUES(str_buf, u16, base, num_elems, sep, IS_NULL_U16)
                        break;
                case LIST_CONTAINER_COLUMN_U32:
                        CARBON_PRINT_JSON_COLUMN_VALUES(str_buf, u32, base, num_elems, sep, IS_NULL_U32)
                        break;
                case LIST_CONTAINER_COLUMN_U64:
                        CARBON_PRINT_JSON_COLUMN_VALUES(str_buf, u64, base, num_elems, sep, IS_NULL_U64)
                        break;
                case LIST_CONTAINER_COLUMN_I8:
                        CARBON_PRINT_JSON_COLUMN_VALUES(str_buf, i8, base, num_elems, sep, IS_NULL_I8)
                        break;
                case LIST_CONTAINER_COLUMN_I16:
                        CARBON_PRINT_JSON_COLUMN_VALUES(str_buf, i16, base, num_elems, sep, IS_NULL_I16)
                        break;
                case LIST_CONTAINER_COLUMN_I32:
                        CARBON_PRINT_JSON_COLUMN_VALUES(str_buf, i32, base, num_elems, sep, IS_NULL_I32)
                        break;
                case LIST_CONTAINER_COLUMN_I64:
                        CARBON_PRINT_JSON_COLUMN_VALUES(str_buf, i64, base, num_elems, sep, IS_NULL_I64)
                        break;
                case LIST_CONTAINER_COLUMN_BOOLEAN:
                        CARBON_PRINT_JSON_COLUMN_VALUES(str_buf, boolean, base, num_elems, sep, IS_NULL_BOOLEAN)
                        break;
                case LIST_CONTAINER_COLUMN_FLOAT:
                        CARBON_PRINT_JSON_COLUMN_VALUES(str_buf, float, base, num_elems, sep, IS_NULL_FLOAT)
                        break;
                default:
                        return error(ERR_INTERNALERR, NULL);
        }

        if (LIKELY(num_elems > 1)) {
                string_buffer_add(str_buf, "]");
        }
        return true;
}

static inline void __carbon_print_json_enter_object_fast(struct carbon_traverse_extra *restrict extra,
                                                         struct carbon_object *restrict it)
{
        struct string_buffer *str_buf = extra->capture.print_json.str;
        field_type_e type;

        string_buffer_add(str_buf, "{");
        char sep = '\0';

        while (carbon_object_next(it)) {
                internal_carbon_object_prop_type(&type, it);

                string_buffer_add_char(str_buf, sep);
                sep = ',';

                carbon_string_field prop_key = internal_carbon_object_prop_name(it);
                __carbon_print_json_string(str_buf, prop_key.string, prop_key.length);
                string_buffer_add_char(str_buf, ':');

                internal_carbon_object_prop_type(&type, it);
                switch (type) {
                        case CARBON_FIELD_NULL:
                                __carbon_print_json_constant(str_buf, CARBON_PRINT_JSON_NULL);
                                break;
                        case CARBON_FIELD_TRUE:
                                __carbon_print_json_constant(str_buf, CARBON_PRINT_JSON_TRUE);
                                break;
                        case CARBON_FIELD_FALSE:
                                __carbon_print_json_constant(str_buf, CARBON_PRINT_JSON_FALSE);
                                break;
                        case CARBON_FIELD_STRING: {
                                carbon_string_field string = carbon_item_get_string(&(it->prop.value), CARBON_NULL_STRING);
                                __carbon_print_json_string(str_buf, string.string, string.length);
                        } break;
                        case CARBON_FIELD_NUMBER_U8:
                                __carbon_print_json_u8_from_prop_value(str_buf, it);
                                break;
                        case CARBON_FIELD_NUMBER_U16:
                                __carbon_print_json_u16_from_prop_value(str_buf, it);
                                break;
                        case CARBON_FIELD_NUMBER_U32:
                                __carbon_print_json_u32_from_prop_value(str_buf, it);
                                break;
                        case CARBON_FIELD_NUMBER_U64:
                                __carbon_print_json_u64_from_prop_value(str_buf, it);
                                break;
                        case CARBON_FIELD_NUMBER_I8:
                                __carbon_print_json_i8_from_prop_value(str_buf, it);
                                break;
                        case CARBON_FIELD_NUMBER_I16:
                                __carbon_print_json_i16_from_prop_value(str_buf, it);
                                break;
                        case CARBON_FIELD_NUMBER_I32:
                                __carbon_print_json_i32_from_prop_value(str_buf, it);
                                break;
                        case CARBON_FIELD_NUMBER_I64:
                                __carbon_print_json_i64_from_prop_value(str_buf, it);
                                break;
                        case CARBON_FIELD_NUMBER_FLOAT:
                                __carbon_print_json_float_from_prop_value(str_buf, it);
                                break;
                        case CARBON_FIELD_BINARY:
                        case CARBON_FIELD_BINARY_CUSTOM: {
                                carbon_binary binary = carbon_item_get_binary(&(it->prop.value), CARBON_NULL_BINARY);
                                __carbon_print_json_binary(str_buf, binary.blob, binary.blob_len);
                        } break;
                        default:
                                break;
                }

                if (carbon_field_type_is_object_or_subtype(type)) {
                        carbon_object *sub = carbon_item_get_object(&(it->prop.value));
                        carbon_traverse_continue_object(extra, sub);
                } else if (carbon_field_type_is_column_or_subtype(type)) {
                        carbon_column *sub = carbon_item_get_column(&(it->prop.value));
                        carbon_traverse_continue_column(extra, sub);
                } else if (carbon_field_type_is_array_or_subtype(type)) {
                        carbon_array *sub = carbon_item_get_array(&(it->prop.value));
                        carbon_traverse_continue_array(extra, sub);
                }
        }
}

static inline void __carbon_print_json_exit_object_fast(struct carbon_traverse_extra *restrict extra,
                                                        struct carbon_object *it)
{
        UNUSED(it)
        struct string_buffer *str_buf = extra->capture.print_json.str;
        string_buffer_add(str_buf, "}");
}

// ---------------------------------------------------------------------------------------------------------------------
//
//  bindings
//
// ---------------------------------------------------------------------------------------------------------------------

static void __carbon_print_json_traverse_create(struct carbon_traverse *traverse, struct string_buffer *str,
                                                struct carbon_json_from_opts *config)
{
        bool use_fast_formatter = !config || config->force_fast_formatter;
        carbon_visit_record_t visit_record = use_fast_formatter ? NULL /* no meta information for fast printing */:
                                             __carbon_print_json_record;
        carbon_visit_array_enter_t enter_array = use_fast_formatter ? __carbon_print_json_enter_array_fast :
                                           __carbon_print_json_enter_array;
        carbon_visit_array_enter_t exit_array = use_fast_formatter ? __carbon_print_json_exit_array_fast :
                                                 __carbon_print_json_exit_array;
        carbon_visit_column_t visit_column = use_fast_formatter ? __carbon_print_json_column_fast :
                                             __carbon_print_json_column;
        carbon_visit_object_enter_t enter_object = use_fast_formatter ? __carbon_print_json_enter_object_fast :
                                             __carbon_print_json_enter_object;
        carbon_visit_object_exit_t exit_object = use_fast_formatter ? __carbon_print_json_exit_object_fast :
                                             __carbon_print_json_exit_object;

        carbon_traverse_create(traverse,
                               NULL /* no setup required since built-in function */,
                               NULL /* no cleanup required since built-in function */,
                               visit_record, enter_array, exit_array, visit_column, enter_object, exit_object,
                               CARBON_TRAVERSE_PRINT_JSON, false);

        string_buffer_clear(str);
        traverse->extra.parent = traverse;
        traverse->extra.capture.print_json.str = str;
        traverse->extra.capture.print_json.config = config ? config : &CARBON_PRINT_JSON_PRETTY;
}

// ---------------------------------------------------------------------------------------------------------------------
//
//  interface functions
//
// ---------------------------------------------------------------------------------------------------------------------

void carbon_json_from_carbon(struct string_buffer *str, rec *record,
                             struct carbon_json_from_opts *config)
{
        assert(str);
        assert(record);

        struct carbon_traverse traverse;

        string_buffer_ensure_capacity(str, 2 * memfile_size(&record->file));
        __carbon_print_json_traverse_create(&traverse, str, config);
        carbon_traverse_carbon(NULL, &traverse, record);
        carbon_traverse_drop(&traverse);

}

void carbon_json_from_array(struct string_buffer *str, carbon_array *it,
                            struct carbon_json_from_opts *config)
{
        assert(str);
        assert(it);

        struct carbon_traverse traverse;

        __carbon_print_json_traverse_create(&traverse, str, config);
        carbon_traverse_array(&traverse, it);
        carbon_traverse_drop(&traverse);
}

void carbon_json_from_column(struct string_buffer *str, struct carbon_column *it,
                             struct carbon_json_from_opts *config)
{
        assert(str);
        assert(it);

        struct carbon_traverse traverse;

        __carbon_print_json_traverse_create(&traverse, str, config);
        carbon_traverse_column(&traverse, it);
        carbon_traverse_drop(&traverse);
}

void carbon_json_from_object(struct string_buffer *str, struct carbon_object *it,
                             struct carbon_json_from_opts *config)
{
        assert(str);
        assert(it);

        struct carbon_traverse traverse;

        __carbon_print_json_traverse_create(&traverse, str, config);
        carbon_traverse_object(&traverse, it);
        carbon_traverse_drop(&traverse);
}