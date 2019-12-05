/*
 * Copyright 2019 Marcus Pinnecke
 */

#include <jakson/carbon/markers.h>
#include <jakson/carbon/traverse2.h>
#include "commit.h"

static path_policy_e markers_print_fn_record(const rec *record, const traverse_info *context, traverse_hidden *extra)
{
        str_buf *str = (str_buf *) extra->arg;
        if (context->type == ON_ENTER) {
                extra->json_printer.record_is_array = rec_is_array(record);

                key_e tk;
                rec_key_type(&tk, (rec *) record);
                str_buf_add(str, "[");
                switch (tk) {
                        case KEY_AUTOKEY:
                                str_buf_add(str, "autokey");
                                break;
                        case KEY_UKEY:
                                str_buf_add(str, "ukey");
                                break;
                        case KEY_IKEY:
                                str_buf_add(str, "ikey");
                                break;
                        case KEY_SKEY:
                                str_buf_add(str, "skey");
                                break;
                        case KEY_NOKEY:
                                str_buf_add(str, "nokey");
                                break;
                        default:
                                error(ERR_MARKERMAPPING, "unknown key marker detected");
                                return PATH_PRUNE;
                };

                str_buf_add(str, "]");

                switch (tk) {
                        case KEY_AUTOKEY:
                        case KEY_UKEY: {
                                u64 key;
                                rec_key_unsigned_value(&key, (rec *) record);
                                str_buf_add(str, "[key:");
                                str_buf_add_u64(str, key);
                                str_buf_add(str, "]");
                        }
                                break;
                        case KEY_IKEY: {
                                i64 key;
                                rec_key_signed_value(&key, (rec *) record);
                                str_buf_add(str, "[key:");
                                str_buf_add_i64(str, key);
                                str_buf_add(str, "]");
                        }
                                break;
                        case KEY_SKEY: {
                                u64 keylen;
                                const char *key = key_string_value(&keylen, (rec *) record);
                                str_buf_add(str, "[key:");
                                str_buf_add(str, "[string]");
                                str_buf_add(str, "(len:");
                                str_buf_add_u64(str, keylen);
                                str_buf_add(str, ")");
                                str_buf_add(str, "[value:");
                                str_buf_add_nchar(str, key, keylen);
                                str_buf_add(str, "]");
                                str_buf_add(str, "]");
                        }
                                break;
                        case KEY_NOKEY:
                                break;
                        default:
                                error(ERR_MARKERMAPPING, "unknown key marker detected");
                                return PATH_PRUNE;
                }

                switch (tk) {
                        case KEY_AUTOKEY:
                        case KEY_UKEY:
                        case KEY_IKEY:
                        case KEY_SKEY: {
                                u64 hash;
                                str_buf hash_str;
                                str_buf_create(&hash_str);

                                str_buf_add(str, "[commit:");
                                rec_commit_hash(&hash, (rec *) record);
                                str_buf_add(str, commit_to_str(&hash_str, hash));
                                str_buf_add(str, "]");

                                str_buf_drop(&hash_str);
                        }
                                break;
                        case KEY_NOKEY:
                                break;
                        default:
                                error(ERR_CORRUPTED, "commit hash for unsupported key type");
                                return PATH_PRUNE;
                }
        }

        str_buf_add(str, (context->type == ON_ENTER ? " [array-begin] " : "[array-end]"));

        return PATH_EXPAND;
}

static path_policy_e markers_print_fn_array(const arr_it *array, const traverse_info *context, traverse_hidden *extra)
{
        UNUSED(array)
        str_buf *str = (str_buf *) extra->arg;
        str_buf_add(str, context->type == ON_ENTER ? "[array-begin]" : "[array-end]");
        return PATH_EXPAND;
}

static path_policy_e markers_print_fn_object(const obj_it *object, const traverse_info *context, traverse_hidden *extra)
{
        UNUSED(object)
        str_buf *str = (str_buf *) extra->arg;
        str_buf_add(str, context->type == ON_ENTER ? "[object-begin]" : "[object-end]");
        return PATH_EXPAND;
}

static void markers_print_field(str_buf *str, const item *field, const traverse_info *context)
{
        if (context->type == ON_ENTER) {
                if (item_is_null(field)) {
                        str_buf_add(str, "[null]");
                } else if (item_is_true(field)) {
                        str_buf_add(str, "[true]");
                } else if (item_is_false(field)) {
                        str_buf_add(str, "[false]");
                } else if (item_is_string(field)) {
                        string_field sf = item_get_string(field, NULL_STRING);
                        str_buf_add(str, "[string]");
                        str_buf_add(str, "(len:");
                        str_buf_add_u64(str, sf.len);
                        str_buf_add(str, ")");
                        str_buf_add(str, "[value:");
                        str_buf_add_nchar(str, sf.str, sf.len);
                        str_buf_add(str, "]");
                } else if (item_is_signed(field)) {
                        i64 value = item_get_number_signed(field, NULL_INT64);
                        str_buf_add(str, "[signed:");
                        str_buf_add_i64(str, value);
                        str_buf_add_char(str, ']');
                } else if (item_is_unsigned(field)) {
                        u64 value = item_get_number_signed(field, NULL_UINT64);
                        str_buf_add(str, "[unsigned:");
                        str_buf_add_u64(str, value);
                        str_buf_add_char(str, ']');
                } else if (item_is_float(field)) {
                        float value = item_get_number_float(field, NULL_FLOAT);
                        str_buf_add(str, "[float:");
                        str_buf_add_float(str, value);
                        str_buf_add_char(str, ']');
                } else if (item_is_binary(field)) {
                        str_buf_add(str, "[binary:");
                        binary_field value = item_get_binary(field, NULL_BINARY);
                        str_buf_add(str, "[mime:");
                        str_buf_add_nchar(str, value.mime, value.mime_len);
                        str_buf_add(str, "][value:");
                        binary_field_value_print(str, &value);
                        str_buf_add(str, "]]");
                } else if (item_is_column(field)) {
                        str_buf_add(str, "[column:<");
                        col_it *it = item_get_column(field);
                        col_it_print(str, it);
                        str_buf_add(str, ">]");
                }
        }
}

static path_policy_e markers_print_fn_field(const item *item, const traverse_info *context, traverse_hidden *extra)
{
        str_buf *str = (str_buf *) extra->arg;
        markers_print_field(str, item, context);
        return PATH_EXPAND;
}

static path_policy_e markers_print_fn_prop(const prop *prop, const traverse_info *context, traverse_hidden *extra)
{
        str_buf *str = (str_buf *) extra->arg;
        if (context->type == ON_ENTER) {
                str_buf_add(str, "[string]");
                str_buf_add(str, "(len:");
                str_buf_add_u64(str, prop->key.len);
                str_buf_add(str, ")");
                str_buf_add(str, "[value:");
                str_buf_add_nchar(str, prop->key.str, prop->key.len);
                str_buf_add(str, "]");
        }
        markers_print_field(str, &prop->value, context);
        return PATH_EXPAND;
}

traverser_fn markers_print_fn = {
        .create = NULL,
        .drop = NULL,
        .visit_record = markers_print_fn_record,
        .visit_array = markers_print_fn_array,
        .visit_object = markers_print_fn_object,
        .visit_field = markers_print_fn_field,
        .visit_prop = markers_print_fn_prop
};

void markers_print(str_buf *dst, rec *src)
{
        TRAVERSE(dst, &markers_print_fn, VISIT_ALL, traverser_run_from_record, src);
}