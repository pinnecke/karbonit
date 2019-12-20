/*
 * json - Java Script Object Notation sub module
 *
 * Copyright 2019 Marcus Pinnecke
 */

#include <jakson/json.h>
#include <jakson/jakson.h>


static const char *_json_from_binary(str_buf *dst, const binary_field *binary)
{
        binary_field_print(dst, binary);
        return str_buf_cstr(dst);
}

static const char *_json_from_string(str_buf *dst, const string_field *binary)
{
        str_buf_add_nquote(dst, binary->str, binary->len);
        return str_buf_cstr(dst);
}

static const char *_json_from_boolean(str_buf *dst, bool value)
{
        str_buf_add(dst, value ? "true" : "false");
        return str_buf_cstr(dst);
}

static const char *_json_from_unsigned(str_buf *dst, u64 value)
{
        str_buf_add_u64(dst, value);
        return str_buf_cstr(dst);
}

static const char *_json_from_signed(str_buf *dst, i64 value)
{
        str_buf_add_i64(dst, value);
        return str_buf_cstr(dst);
}

static const char *_json_from_float(str_buf *dst, float value)
{
        str_buf_add_float(dst, value);
        return str_buf_cstr(dst);
}

static const char *_json_from_null(str_buf *dst)
{
        str_buf_add(dst, "null");
        return str_buf_cstr(dst);
}

static const char *_json_from_undef(str_buf *dst)
{
        str_buf_add(dst, "undef");
        return str_buf_cstr(dst);
}

const char *_json_from_column(str_buf *dst, col_it *src)
{
        col_it_print(dst, src);
        return str_buf_cstr(dst);
}

static path_policy_e json_from_fn_record(const rec *record, const traverse_info *context, traverse_hidden *extra)
{
        str_buf *str = (str_buf *) extra->arg;
        if (context->type == ON_ENTER) {
                extra->json_printer.record_is_array = rec_is_array(record);
        }
        str_buf_add(str, extra->json_printer.record_is_array ?
                              (context->type == ON_ENTER ? "[" : "]") :
                              "");

        return PATH_EXPAND;
}

static path_policy_e json_from_fn_array(const arr_it *array, const traverse_info *context, traverse_hidden *extra)
{
        UNUSED(array)
        str_buf *str = (str_buf *) extra->arg;
        str_buf_add_char(str, context->type == ON_ENTER ? '[' : ']');
        return PATH_EXPAND;
}

static path_policy_e json_from_fn_object(const obj_it *object, const traverse_info *context, traverse_hidden *extra)
{
        UNUSED(object)
        str_buf *str = (str_buf *) extra->arg;
        str_buf_add_char(str, context->type == ON_ENTER ? '{' : '}');
        return PATH_EXPAND;
}

static void json_print_field(str_buf *str, const item *field, const traverse_info *context)
{
        if (context->type == ON_ENTER) {
                if (ITEM_IS_NULL(field)) {
                        _json_from_null(str);
                } else if (ITEM_IS_TRUE(field)) {
                        _json_from_boolean(str, true);
                } else if (ITEM_IS_FALSE(field)) {
                        _json_from_boolean(str, false);
                } else if (ITEM_IS_STRING(field)) {
                        string_field sf = ITEM_GET_STRING(field, NULL_STRING);
                        _json_from_string(str, &sf);
                } else if (ITEM_IS_SIGNED(field)) {
                        i64 value = ITEM_GET_NUMBER_SIGNED(field, NULL_INT64);
                        _json_from_signed(str, value);
                } else if (ITEM_IS_UNSIGNED(field)) {
                        u64 value = ITEM_GET_NUMBER_SIGNED(field, NULL_UINT64);
                        _json_from_unsigned(str, value);
                } else if (ITEM_IS_FLOAT(field)) {
                        float value = ITEM_GET_NUMBER_FLOAT(field, NULL_FLOAT);
                        _json_from_float(str, value);
                } else if (ITEM_IS_BINARY(field)) {
                        binary_field value = ITEM_GET_BINARY(field, NULL_BINARY);
                        _json_from_binary(str, &value);
                } else if (ITEM_IS_COLUMN(field)) {
                        col_it it;
                        ITEM_GET_COLUMN(&it, field);
                        _json_from_column(str, &it);
                }
        }
}

static path_policy_e json_from_fn_field(const item *item, const traverse_info *context, traverse_hidden *extra)
{
        str_buf *str = (str_buf *) extra->arg;
        str_buf_add(str, ITEM_GET_INDEX(item) > 0 && context->type == ON_ENTER ? ", " : "");
        json_print_field(str, item, context);
        return PATH_EXPAND;
}

static path_policy_e json_from_fn_prop(const prop *prop, const traverse_info *context, traverse_hidden *extra)
{
        str_buf *str = (str_buf *) extra->arg;
        if (context->type == ON_ENTER) {
                str_buf_add(str, prop_get_index(prop) > 0 && context->type == ON_ENTER ? ", " : "");
                str_buf_add_nquote(str, prop->key.str, prop->key.len);
                str_buf_add_char(str, ':');
        }
        json_print_field(str, &prop->value, context);
        return PATH_EXPAND;
}

traverser_fn json_from_fn = {
        .create = NULL,
        .drop = NULL,
        .visit_record = json_from_fn_record,
        .visit_array = json_from_fn_array,
        .visit_object = json_from_fn_object,
        .visit_field = json_from_fn_field,
        .visit_prop = json_from_fn_prop
};

const char *json_from_record(str_buf *dst, rec *src)
{
        TRAVERSE(dst, &json_from_fn, VISIT_ALL, traverser_run_from_record, src);
        return str_buf_cstr(dst);
}

const char *json_from_array(str_buf *dst, arr_it *src)
{
        TRAVERSE(dst, &json_from_fn, VISIT_ALL, traverser_run_from_array, src);
        return str_buf_cstr(dst);
}

const char *json_from_object(str_buf *dst, obj_it *src)
{
        TRAVERSE(dst, &json_from_fn, VISIT_ALL, traverser_run_from_object, src);
        return str_buf_cstr(dst);
}

const char *json_from_item(str_buf *dst, item *src)
{
        traverse_info info = { .type = ON_ENTER };
        json_print_field(dst, src, &info);
        info.type = ON_EXIT;
        json_print_field(dst, src, &info);
        return str_buf_cstr(dst);
}

const char *json_from_column(str_buf *dst, col_it *src)
{
        str_buf_clear(dst);
        return _json_from_column(dst, src);
}

const char *json_from_binary(str_buf *dst, const binary_field *binary)
{
        str_buf_clear(dst);
        return _json_from_binary(dst, binary);
}

const char *json_from_string(str_buf *dst, const string_field *str)
{
        str_buf_clear(dst);
        return _json_from_string(dst, str);
}

const char *json_from_boolean(str_buf *dst, bool value)
{
        str_buf_clear(dst);
        return _json_from_boolean(dst, value);
}

const char *json_from_unsigned(str_buf *dst, u64 value)
{
        str_buf_clear(dst);
        return _json_from_unsigned(dst, value);
}

const char *json_from_signed(str_buf *dst, i64 value)
{
        str_buf_clear(dst);
        return _json_from_signed(dst, value);
}

const char *json_from_float(str_buf *dst, float value)
{
        str_buf_clear(dst);
        return _json_from_float(dst, value);
}

const char *json_from_null(str_buf *dst)
{
        str_buf_clear(dst);
        return _json_from_null(dst);
}

const char *json_from_undef(str_buf *dst)
{
        str_buf_clear(dst);
        return _json_from_undef(dst);
}