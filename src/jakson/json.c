/*
 * json - Java Script Object Notation sub module
 *
 * Copyright 2019 Marcus Pinnecke
 */

#include <jakson/json.h>
#include <jakson/jakson.h>

static path_policy_e json_from_fn_record(const rec *record, const traverse_info *context, traverse_hidden *extra)
{
        UNUSED(record)
        UNUSED(context)
        UNUSED(extra)

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
                if (item_is_null(field)) {
                        str_buf_add(str, "null");
                } else if (item_is_true(field)) {
                        str_buf_add(str, "true");
                } else if (item_is_false(field)) {
                        str_buf_add(str, "false");
                } else if (item_is_string(field)) {
                        string_field sf = item_get_string(field, NULL_STRING);
                        str_buf_add_nquote(str, sf.str, sf.len);
                } else if (item_is_signed(field)) {
                        i64 value = item_get_number_signed(field, NULL_INT64);
                        str_buf_add_i64(str, value);
                } else if (item_is_unsigned(field)) {
                        u64 value = item_get_number_signed(field, NULL_UINT64);
                        str_buf_add_u64(str, value);
                } else if (item_is_float(field)) {
                        float value = item_get_number_float(field, NULL_FLOAT);
                        str_buf_add_float(str, value);
                } else if (item_is_binary(field)) {
                        binary_field value = item_get_binary(field, NULL_BINARY);
                        binary_field_print(str, &value);
                } else if (item_is_column(field)) {
                        col_it *it = item_get_column(field);
                        col_it_print(str, it);
                }
        }
}

static path_policy_e json_from_fn_field(const item *item, const traverse_info *context, traverse_hidden *extra)
{
        str_buf *str = (str_buf *) extra->arg;
        str_buf_add(str, item_get_index(item) > 0 && context->type == ON_ENTER ? ", " : "");
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

void json_from_record(str_buf *dst, rec *src)
{
        traverser traverser;
        str_buf_clear(dst);
        traverser_create(&traverser, &json_from_fn, VISIT_ALL);
        traverser_run_from_record(&traverser, src, dst);
        traverser_drop(&traverser);
}
//
//void json_from_array(str_buf *dst, const arr_it *src)
//{
//
//}
//
//void json_from_column(str_buf *dst, const col_it *src)
//{
//
//}
//
//void json_from_object(str_buf *dst, const obj_it *src)
//{
//
//}
//
//void json_from_item(str_buf *dst, const item *src)
//{
//
//}
