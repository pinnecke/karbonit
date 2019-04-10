/**
 * Copyright 2018 Marcus Pinnecke
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

#include <math.h>
#include <inttypes.h>
#include "json/json.h"
#include "json/doc.h"
#include "json/columndoc.h"
#include "json/json.h"
#include "stdx/sort.h"

char VALUE_NULL = '\0';

static void create_doc(carbon_doc_obj_t *model, carbon_doc_t *doc);

static void create_typed_vector(carbon_doc_entries_t *entry);

static void entries_drop(carbon_doc_entries_t *entry);

static bool print_value(FILE *file, field_e type, const vec_t ofType(<T>) *values);

static void print_object(FILE *file, const carbon_doc_obj_t *model);

static bool import_json_object(carbon_doc_obj_t *target, struct err *err, const carbon_json_ast_node_object_t *json_obj);

static void sort_columndoc_entries(columndoc_obj_t *columndoc);

CARBON_EXPORT(bool)
carbon_doc_bulk_create(carbon_doc_bulk_t *bulk, carbon_strdic_t *dic)
{
    CARBON_NON_NULL_OR_ERROR(bulk)
    CARBON_NON_NULL_OR_ERROR(dic)
    bulk->dic = dic;
    carbon_vec_create(&bulk->keys, NULL, sizeof(char *), 500);
    carbon_vec_create(&bulk->values, NULL, sizeof(char *), 1000);
    carbon_vec_create(&bulk->models, NULL, sizeof(carbon_doc_t), 50);
    return true;
}

carbon_doc_obj_t *carbon_doc_bulk_new_obj(carbon_doc_t *model)
{
    if (!model) {
        return NULL;
    } else {
        carbon_doc_obj_t *retval = VECTOR_NEW_AND_GET(&model->obj_model, carbon_doc_obj_t);
        create_doc(retval, model);
        return retval;
    }
}

CARBON_EXPORT(bool)
carbon_doc_bulk_get_dic_contents(vec_t ofType (const char *) **strings,
                                 vec_t ofType(carbon_string_id_t) **string_ids,
                                 const carbon_doc_bulk_t *context)
{
    CARBON_NON_NULL_OR_ERROR(context)

    size_t num_distinct_values;
    carbon_strdic_num_distinct(&num_distinct_values, context->dic);
    vec_t ofType (const char *) *result_strings = malloc(sizeof(vec_t));
    vec_t ofType (carbon_string_id_t) *resultcarbon_string_id_ts = malloc(sizeof(vec_t));
    carbon_vec_create(result_strings, NULL, sizeof(const char *), num_distinct_values);
    carbon_vec_create(resultcarbon_string_id_ts, NULL, sizeof(carbon_string_id_t), num_distinct_values);

    int status = carbon_strdic_get_contents(result_strings, resultcarbon_string_id_ts, context->dic);
    CARBON_CHECK_SUCCESS(status);
    *strings = result_strings;
    *string_ids = resultcarbon_string_id_ts;

    return status;
}

carbon_doc_t *carbon_doc_bulk_new_doc(carbon_doc_bulk_t *context, field_e type)
{
    if (!context) {
        return NULL;
    }

    carbon_doc_t template, *model;
    size_t idx = carbon_vec_length(&context->models);
    carbon_vec_push(&context->models, &template, 1);
    model = vec_get(&context->models, idx, carbon_doc_t);
    model->context = context;
    model->type = type;

    carbon_vec_create(&model->obj_model, NULL, sizeof(carbon_doc_obj_t), 500);

    return model;
}

CARBON_EXPORT(bool)
carbon_doc_bulk_Drop(carbon_doc_bulk_t *bulk)
{
    CARBON_NON_NULL_OR_ERROR(bulk)
    for (size_t i = 0; i < bulk->keys.num_elems; i++) {
        char *string = *vec_get(&bulk->keys, i, char *);
        free (string);
    }
    for (size_t i = 0; i < bulk->values.num_elems; i++) {
        char *string = *vec_get(&bulk->values, i, char *);
        free (string);
    }
    for (size_t i = 0; i < bulk->models.num_elems; i++) {
        carbon_doc_t *model = vec_get(&bulk->models, i, carbon_doc_t);
        for (size_t j = 0; j < model->obj_model.num_elems; j++) {
            carbon_doc_obj_t *doc = vec_get(&model->obj_model, j, carbon_doc_obj_t);
            carbon_doc_drop(doc);
        }
        carbon_vec_drop(&model->obj_model);
    }

    carbon_vec_drop(&bulk->keys);
    carbon_vec_drop(&bulk->values);
    carbon_vec_drop(&bulk->models);
    return true;
}

CARBON_EXPORT(bool)
carbon_doc_bulk_shrink(carbon_doc_bulk_t *bulk)
{
    CARBON_NON_NULL_OR_ERROR(bulk)
    VectorShrink(&bulk->keys);
    VectorShrink(&bulk->values);
    return true;
}

CARBON_EXPORT(bool)
carbon_doc_bulk_print(FILE *file, carbon_doc_bulk_t *bulk)
{
    CARBON_NON_NULL_OR_ERROR(file)
    CARBON_NON_NULL_OR_ERROR(bulk)

    fprintf(file, "{");
    char **key_strings = vec_all(&bulk->keys, char *);
    fprintf(file, "\"Key Strings\": [");
    for (size_t i = 0; i < bulk->keys.num_elems; i++) {
        fprintf(file, "\"%s\"%s", key_strings[i], i + 1 < bulk->keys.num_elems ? ", " : "");
    }
    fprintf(file, "], ");

    char **valueStrings = vec_all(&bulk->values, char *);
    fprintf(file, "\"Value Strings\": [");
    for (size_t i = 0; i < bulk->values.num_elems; i++) {
        fprintf(file, "\"%s\"%s", valueStrings[i], i + 1 < bulk->values.num_elems ? ", " : "");
    }
    fprintf(file, "]}");

    return true;
}

CARBON_EXPORT(bool)
carbon_doc_print(FILE *file, const carbon_doc_t *doc)
{
    CARBON_NON_NULL_OR_ERROR(file)
    CARBON_NON_NULL_OR_ERROR(doc)

    if (doc->obj_model.num_elems == 0) {
        fprintf(file, "{ }");
    }

    if (doc->obj_model.num_elems > 1) {
        fprintf(file, "[");
    }

    for (size_t num_entries = 0; num_entries < doc->obj_model.num_elems; num_entries++) {
        carbon_doc_obj_t *object = vec_get(&doc->obj_model, num_entries, carbon_doc_obj_t);
        print_object(file, object);
        fprintf(file, "%s", num_entries + 1 < doc->obj_model.num_elems ? ", " : "");
    }


    if (doc->obj_model.num_elems > 1) {
        fprintf(file, "]");
    }

    return true;
}

const vec_t ofType(carbon_doc_entries_t) *carbon_doc_get_entries(const carbon_doc_obj_t *model)
{
    return &model->entries;
}

void carbon_doc_print_entries(FILE *file, const carbon_doc_entries_t *entries)
{
    fprintf(file, "{\"Key\": \"%s\"", entries->key);
}

void carbon_doc_drop(carbon_doc_obj_t *model)
{
    for (size_t i = 0; i < model->entries.num_elems; i++) {
        carbon_doc_entries_t *entry = vec_get(&model->entries, i, carbon_doc_entries_t);
        entries_drop(entry);
    }
    carbon_vec_drop(&model->entries);
}

bool carbon_doc_obj_add_key(carbon_doc_entries_t **out,
                            carbon_doc_obj_t *obj,
                            const char *key,
                            field_e type)
{
    CARBON_NON_NULL_OR_ERROR(out)
    CARBON_NON_NULL_OR_ERROR(obj)
    CARBON_NON_NULL_OR_ERROR(key)

    size_t entry_idx;
    char *key_dup = strdup(key);

    carbon_doc_entries_t entry_model = {
        .type = type,
        .key = key_dup,
        .context = obj
    };

    create_typed_vector(&entry_model);
    carbon_vec_push(&obj->doc->context->keys, &key_dup, 1);

    entry_idx = carbon_vec_length(&obj->entries);
    carbon_vec_push(&obj->entries, &entry_model, 1);

    *out = vec_get(&obj->entries, entry_idx, carbon_doc_entries_t);

    return true;
}

bool carbon_doc_obj_push_primtive(carbon_doc_entries_t *entry, const void *value)
{
    CARBON_NON_NULL_OR_ERROR(entry)
    CARBON_NON_NULL_OR_ERROR((entry->type == field_null) || (value != NULL))

    switch(entry->type) {
        case field_null:
            carbon_vec_push(&entry->values, &VALUE_NULL, 1);
        break;
        case field_string: {
            char *string = value ? strdup((char *) value) : NULL;
            carbon_vec_push(&entry->context->doc->context->values, &string, 1);
            carbon_vec_push(&entry->values, &string, 1);
        }
        break;
    default:
            carbon_vec_push(&entry->values, value, 1);
        break;
    }
    return true;
}

bool carbon_doc_obj_push_object(carbon_doc_obj_t **out, carbon_doc_entries_t *entry)
{
    CARBON_NON_NULL_OR_ERROR(out);
    CARBON_NON_NULL_OR_ERROR(entry);

    assert(entry->type == field_object);

    carbon_doc_obj_t objectModel;

    create_doc(&objectModel, entry->context->doc);
    size_t length = carbon_vec_length(&entry->values);
    carbon_vec_push(&entry->values, &objectModel, 1);

    *out = vec_get(&entry->values, length, carbon_doc_obj_t);

    return true;
}

static field_e value_type_for_json_number(bool *success, struct err *err, const carbon_json_ast_node_number_t *number)
{
    *success = true;
    switch(number->value_type) {
    case CARBON_JSON_AST_NODE_NUMBER_VALUE_TYPE_REAL_NUMBER:
        return field_float;
    case CARBON_JSON_AST_NODE_NUMBER_VALUE_TYPE_UNSIGNED_INTEGER: {
        u64 test = number->value.unsigned_integer;
        if (test <= CARBON_LIMITS_UINT8_MAX) {
            return field_uint8;
        } else if (test <= CARBON_LIMITS_UINT16_MAX) {
            return field_uint16;
        } else if (test <= CARBON_LIMITS_UINT32_MAX) {
            return field_uint32;
        } else {
            return field_uint64;
        }
    }
    case CARBON_JSON_AST_NODE_NUMBER_VALUE_TYPE_SIGNED_INTEGER: {
        i64 test = number->value.signed_integer;
        if (test >= CARBON_LIMITS_INT8_MIN && test <= CARBON_LIMITS_INT8_MAX) {
            return field_int8;
        } else if (test >= CARBON_LIMITS_INT16_MIN && test <= CARBON_LIMITS_INT16_MAX) {
            return field_int16;
        } else if (test >= CARBON_LIMITS_INT32_MIN && test <= CARBON_LIMITS_INT32_MAX) {
            return field_int32;
        } else {
            return field_int64;
        }
    }
    default:
        error(err, CARBON_ERR_NOJSONNUMBERT);
        *success = false;
	return field_int8;
    }
}

static void import_json_object_string_prop(carbon_doc_obj_t *target, const char *key, const carbon_json_ast_node_string_t *string)
{
    carbon_doc_entries_t *entry;
    carbon_doc_obj_add_key(&entry, target, key, field_string);
    carbon_doc_obj_push_primtive(entry, string->value);
}

static bool import_json_object_number_prop(carbon_doc_obj_t *target, struct err *err, const char *key, const carbon_json_ast_node_number_t *number)
{
    carbon_doc_entries_t *entry;
    bool success;
    field_e number_type = value_type_for_json_number(&success, err, number);
    if (!success) {
        return false;
    }
    carbon_doc_obj_add_key(&entry, target, key, number_type);
    carbon_doc_obj_push_primtive(entry, &number->value);
    return true;
}

static void import_json_object_bool_prop(carbon_doc_obj_t *target, const char *key, carbon_boolean_t value)
{
    carbon_doc_entries_t *entry;
    carbon_doc_obj_add_key(&entry, target, key, field_bool);
    carbon_doc_obj_push_primtive(entry, &value);
}

static void import_json_object_null_prop(carbon_doc_obj_t *target, const char *key)
{
    carbon_doc_entries_t *entry;
    carbon_doc_obj_add_key(&entry, target, key, field_null);
    carbon_doc_obj_push_primtive(entry, NULL);
}

static bool import_json_object_object_prop(carbon_doc_obj_t *target, struct err *err, const char *key, const carbon_json_ast_node_object_t *object)
{
    carbon_doc_entries_t *entry;
    carbon_doc_obj_t *nested_object = NULL;
    carbon_doc_obj_add_key(&entry, target, key, field_object);
    carbon_doc_obj_push_object(&nested_object, entry);
    return import_json_object(nested_object, err, object);
}

static bool import_json_object_array_prop(carbon_doc_obj_t *target, struct err *err, const char *key, const carbon_json_ast_node_array_t *array)
{
    carbon_doc_entries_t *entry;

    if (!carbon_vec_is_empty(&array->elements.elements))
    {
        size_t num_elements = array->elements.elements.num_elems;

        /** Find first type that is not null unless the entire array is of type null */
        carbon_json_ast_node_value_type_e array_data_type = CARBON_JSON_AST_NODE_VALUE_TYPE_NULL;
        field_e field_type;

        for (size_t i = 0; i < num_elements && array_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_NULL; i++) {
            const carbon_json_ast_node_element_t *element = vec_get(&array->elements.elements, i, carbon_json_ast_node_element_t);
            array_data_type = element->value.value_type;
        }

        switch (array_data_type) {
        case CARBON_JSON_AST_NODE_VALUE_TYPE_OBJECT:
            field_type = field_object;
            break;
        case CARBON_JSON_AST_NODE_VALUE_TYPE_STRING:
            field_type = field_string;
            break;
        case CARBON_JSON_AST_NODE_VALUE_TYPE_NUMBER: {
            /** find smallest fitting physical number type */
            field_e array_number_type = field_null;
            for (size_t i = 0; i < num_elements; i++) {
                const carbon_json_ast_node_element_t *element = vec_get(&array->elements.elements, i, carbon_json_ast_node_element_t);
                if (CARBON_UNLIKELY(element->value.value_type == CARBON_JSON_AST_NODE_VALUE_TYPE_NULL)) {
                    continue;
                } else {
                    bool success;
                    field_e element_number_type = value_type_for_json_number(&success, err, element->value.value.number);
                    if (!success) {
                        return false;
                    }
                    assert(element_number_type == field_int8 || element_number_type == field_int16 ||
                           element_number_type == field_int32 || element_number_type == field_int64 ||
                           element_number_type == field_uint8 || element_number_type == field_uint16 ||
                           element_number_type == field_uint32 || element_number_type == field_uint64 ||
                           element_number_type == field_float);
                    if (CARBON_UNLIKELY(array_number_type == field_null)) {
                        array_number_type = element_number_type;
                    } else {
                        if (array_number_type == field_int8) {
                            array_number_type = element_number_type;
                        } else if (array_number_type == field_int16) {
                            if (element_number_type != field_int8) {
                                array_number_type = element_number_type;
                            }
                        } else if (array_number_type == field_int32) {
                            if (element_number_type != field_int8 && element_number_type != field_int16) {
                                array_number_type = element_number_type;
                            }
                        } else if (array_number_type == field_int64) {
                            if (element_number_type != field_int8 && element_number_type != field_int16 &&
                                element_number_type != field_int32) {
                                array_number_type = element_number_type;
                            }
                        } else if (array_number_type == field_uint8) {
                            array_number_type = element_number_type;
                        } else if (array_number_type == field_uint16) {
                            if (element_number_type != field_uint16) {
                                array_number_type = element_number_type;
                            }
                        } else if (array_number_type == field_uint32) {
                            if (element_number_type != field_uint8 && element_number_type != field_uint16) {
                                array_number_type = element_number_type;
                            }
                        } else if (array_number_type == field_uint64) {
                            if (element_number_type != field_uint8 && element_number_type != field_uint16 &&
                                element_number_type != field_uint32) {
                                array_number_type = element_number_type;
                            }
                        } else if (array_number_type == field_float) {
                            break;
                        }
                    }
                }
            }
            assert(array_number_type != field_null);
            field_type = array_number_type;
        } break;
        case CARBON_JSON_AST_NODE_VALUE_TYPE_FALSE:
        case CARBON_JSON_AST_NODE_VALUE_TYPE_TRUE:
            field_type = field_bool;
            break;
        case CARBON_JSON_AST_NODE_VALUE_TYPE_NULL:
            field_type = field_null;
            break;
        case CARBON_JSON_AST_NODE_VALUE_TYPE_ARRAY:
            error(err, CARBON_ERR_ERRINTERNAL) /** array type is illegal here */
            return false;
        default:
            error(err, CARBON_ERR_NOTYPE)
            return false;
        }

        carbon_doc_obj_add_key(&entry, target, key, field_type);

        for (size_t i = 0; i < num_elements; i++)
        {
            const carbon_json_ast_node_element_t *element = vec_get(&array->elements.elements, i, carbon_json_ast_node_element_t);
            carbon_json_ast_node_value_type_e ast_node_data_type = element->value.value_type;

            switch (field_type) {
            case field_object: {
                carbon_doc_obj_t *nested_object = NULL;
                carbon_doc_obj_push_object(&nested_object, entry);
                if (ast_node_data_type != CARBON_JSON_AST_NODE_VALUE_TYPE_NULL) {
                    /** the object is null by definition, if no entries are contained */
                    if (!import_json_object(nested_object, err, element->value.value.object)) {
                        return false;
                    }
                }
            } break;
            case field_string: {
                assert(ast_node_data_type == array_data_type || ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_NULL);
                carbon_doc_obj_push_primtive(entry, ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_NULL ?
                                                    CARBON_NULL_ENCODED_STRING :
                                                    element->value.value.string->value);
            } break;
            case field_int8:
            case field_int16:
            case field_int32:
            case field_int64:
            case field_uint8:
            case field_uint16:
            case field_uint32:
            case field_uint64:
            case field_float: {
                assert(ast_node_data_type == array_data_type || ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_NULL);
                switch(field_type) {
                case field_int8: {
                    carbon_i8 value = ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_NULL ? CARBON_NULL_INT8 :
                                      (carbon_i8) element->value.value.number->value.signed_integer;
                    carbon_doc_obj_push_primtive(entry, &value);
                } break;
                case field_int16: {
                    carbon_i16 value = ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_NULL ? CARBON_NULL_INT16 :
                                       (carbon_i16) element->value.value.number->value.signed_integer;
                    carbon_doc_obj_push_primtive(entry, &value);
                } break;
                case field_int32: {
                    carbon_i32 value = ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_NULL ? CARBON_NULL_INT32 :
                                       (carbon_i32) element->value.value.number->value.signed_integer;
                    carbon_doc_obj_push_primtive(entry, &value);
                } break;
                case field_int64: {
                    carbon_i64 value = ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_NULL ? CARBON_NULL_INT64 :
                                       (carbon_i64) element->value.value.number->value.signed_integer;
                    carbon_doc_obj_push_primtive(entry, &value);
                } break;
                case field_uint8: {
                    carbon_u8 value = ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_NULL ? CARBON_NULL_UINT8 :
                                       (carbon_u8) element->value.value.number->value.unsigned_integer;
                    carbon_doc_obj_push_primtive(entry, &value);
                } break;
                case field_uint16: {
                    carbon_u16 value = ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_NULL ? CARBON_NULL_UINT16 :
                                        (carbon_u16) element->value.value.number->value.unsigned_integer;
                    carbon_doc_obj_push_primtive(entry, &value);
                } break;
                case field_uint32: {
                    carbon_u32 value = ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_NULL ? CARBON_NULL_UINT32 :
                                        (carbon_u32) element->value.value.number->value.unsigned_integer;
                    carbon_doc_obj_push_primtive(entry, &value);
                } break;
                case field_uint64: {
                    carbon_u64 value = ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_NULL ? CARBON_NULL_UINT64 :
                                        (carbon_u64) element->value.value.number->value.unsigned_integer;
                    carbon_doc_obj_push_primtive(entry, &value);
                } break;
                case field_float: {
                    carbon_number_t value = CARBON_NULL_FLOAT;
                    if (ast_node_data_type != CARBON_JSON_AST_NODE_VALUE_TYPE_NULL) {
                        carbon_json_ast_node_number_value_type_e element_number_type = element->value.value.number->value_type;
                        if (element_number_type == CARBON_JSON_AST_NODE_NUMBER_VALUE_TYPE_REAL_NUMBER) {
                            value = element->value.value.number->value.float_number;
                        } else if (element_number_type == CARBON_JSON_AST_NODE_NUMBER_VALUE_TYPE_UNSIGNED_INTEGER) {
                            value = element->value.value.number->value.unsigned_integer;
                        } else if (element_number_type == CARBON_JSON_AST_NODE_NUMBER_VALUE_TYPE_SIGNED_INTEGER) {
                            value = element->value.value.number->value.signed_integer;
                        } else {
                            carbon_print_error_and_die(CARBON_ERR_INTERNALERR) /** type mismatch */
                            return false;
                        }
                    }
                    carbon_doc_obj_push_primtive(entry, &value);
                } break;
                default:
                    carbon_print_error_and_die(CARBON_ERR_INTERNALERR) /** not a number type  */
                    return false;
                }
            } break;
            case field_bool:
                if (CARBON_LIKELY(ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_TRUE ||
                                  ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_FALSE)) {
                    carbon_boolean_t value = ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_TRUE ?
                                         CARBON_BOOLEAN_TRUE : CARBON_BOOLEAN_FALSE;
                    carbon_doc_obj_push_primtive(entry, &value);
                } else {
                    assert(ast_node_data_type == CARBON_JSON_AST_NODE_VALUE_TYPE_NULL);
                    carbon_boolean_t value = CARBON_NULL_BOOLEAN;
                    carbon_doc_obj_push_primtive(entry, &value);
                }
                break;
            case field_null:
                assert(ast_node_data_type == array_data_type);
                carbon_doc_obj_push_primtive(entry, NULL);
                break;
            default:
                error(err, CARBON_ERR_NOTYPE)
                return false;
            }
        }
    } else {
        import_json_object_null_prop(target, key);
    }
    return true;
}

static bool import_json_object(carbon_doc_obj_t *target, struct err *err, const carbon_json_ast_node_object_t *json_obj)
{
    for (size_t i = 0; i < json_obj->value->members.num_elems; i++) {
        carbon_json_ast_node_member_t *member = vec_get(&json_obj->value->members, i, carbon_json_ast_node_member_t);
        carbon_json_ast_node_value_type_e value_type = member->value.value.value_type;
        switch (value_type) {
        case CARBON_JSON_AST_NODE_VALUE_TYPE_STRING:
            import_json_object_string_prop(target, member->key.value, member->value.value.value.string);
            break;
        case CARBON_JSON_AST_NODE_VALUE_TYPE_NUMBER:
            if (!import_json_object_number_prop(target, err, member->key.value, member->value.value.value.number)) {
                return false;
            }
            break;
        case CARBON_JSON_AST_NODE_VALUE_TYPE_TRUE:
        case CARBON_JSON_AST_NODE_VALUE_TYPE_FALSE: {
            carbon_boolean_t value = value_type == CARBON_JSON_AST_NODE_VALUE_TYPE_TRUE ? CARBON_BOOLEAN_TRUE : CARBON_BOOLEAN_FALSE;
            import_json_object_bool_prop(target, member->key.value, value);
        } break;
        case CARBON_JSON_AST_NODE_VALUE_TYPE_NULL:
            import_json_object_null_prop(target, member->key.value);
            break;
        case CARBON_JSON_AST_NODE_VALUE_TYPE_OBJECT:
            if (!import_json_object_object_prop(target, err, member->key.value, member->value.value.value.object)) {
                return false;
            }
            break;
        case CARBON_JSON_AST_NODE_VALUE_TYPE_ARRAY:
            if (!import_json_object_array_prop(target, err, member->key.value, member->value.value.value.array)) {
                return false;
            }
            break;
        default:
            error(err, CARBON_ERR_NOTYPE);
            return false;
        }
    }
    return true;
}

static bool import_json(carbon_doc_obj_t *target, struct err *err, const carbon_json_t *json, carbon_doc_entries_t *partition)
{
    carbon_json_ast_node_value_type_e value_type = json->element->value.value_type;
    switch (value_type) {
    case CARBON_JSON_AST_NODE_VALUE_TYPE_OBJECT:
        if (!import_json_object(target, err, json->element->value.value.object)) {
            return false;
        }
        break;
    case CARBON_JSON_AST_NODE_VALUE_TYPE_ARRAY: {
        const vec_t ofType(carbon_json_ast_node_element_t) *arrayContent = &json->element->value.value.array->elements.elements;
        if (!carbon_vec_is_empty(arrayContent)) {
            const carbon_json_ast_node_element_t *first = vec_get(arrayContent, 0, carbon_json_ast_node_element_t);
            switch (first->value.value_type) {
            case CARBON_JSON_AST_NODE_VALUE_TYPE_OBJECT:
                if (!import_json_object(target, err, first->value.value.object)) {
                    return false;
                }
                for (size_t i = 1; i < arrayContent->num_elems; i++) {
                    const carbon_json_ast_node_element_t *element = vec_get(arrayContent, i, carbon_json_ast_node_element_t);
                    carbon_doc_obj_t *nested;
                    carbon_doc_obj_push_object(&nested, partition);
                    if (!import_json_object(nested, err, element->value.value.object)) {
                        return false;
                    }
                }
                break;
            case CARBON_JSON_AST_NODE_VALUE_TYPE_ARRAY:
            case CARBON_JSON_AST_NODE_VALUE_TYPE_STRING:
            case CARBON_JSON_AST_NODE_VALUE_TYPE_NUMBER:
            case CARBON_JSON_AST_NODE_VALUE_TYPE_TRUE:
            case CARBON_JSON_AST_NODE_VALUE_TYPE_FALSE:
            case CARBON_JSON_AST_NODE_VALUE_TYPE_NULL:
            default:
                carbon_print_error_and_die(CARBON_ERR_INTERNALERR) /** Unsupported operation in arrays */
                break;
            }
        }
    } break;
    case CARBON_JSON_AST_NODE_VALUE_TYPE_STRING:
    case CARBON_JSON_AST_NODE_VALUE_TYPE_NUMBER:
    case CARBON_JSON_AST_NODE_VALUE_TYPE_TRUE:
    case CARBON_JSON_AST_NODE_VALUE_TYPE_FALSE:
    case CARBON_JSON_AST_NODE_VALUE_TYPE_NULL:
    default:
        error(err, CARBON_ERR_JSONTYPE);
        return false;
    }
    return true;
}

carbon_doc_obj_t *carbon_doc_bulk_add_json(carbon_doc_entries_t *partition, carbon_json_t *json)
{
    if (!partition || !json) {
        return NULL;
    }

    carbon_doc_obj_t *converted_json;
    carbon_doc_obj_push_object(&converted_json, partition);
    if (!import_json(converted_json, &json->err, json, partition)) {
        return NULL;
    }

    return converted_json;
}

carbon_doc_obj_t *carbon_doc_entries_get_root(const carbon_doc_entries_t *partition)
{
    return partition ? partition->context : NULL;
}

carbon_doc_entries_t *carbon_doc_bulk_new_entries(carbon_doc_bulk_t *dst)
{
    carbon_doc_entries_t *partition = NULL;
    carbon_doc_t *model = carbon_doc_bulk_new_doc(dst, field_object);
    carbon_doc_obj_t *object = carbon_doc_bulk_new_obj(model);
    carbon_doc_obj_add_key(&partition, object, "/", field_object);
    return partition;
}

#define DEFINE_CARBON_TYPE_LQ_FUNC(type)                                                                               \
static bool compare##type##LessEqFunc(const void *lhs, const void *rhs)                                                \
{                                                                                                                      \
    type a = *(type *) lhs;                                                                                            \
    type b = *(type *) rhs;                                                                                            \
    return (a <= b);                                                                                                   \
}

DEFINE_CARBON_TYPE_LQ_FUNC(carbon_boolean_t)
DEFINE_CARBON_TYPE_LQ_FUNC(carbon_number_t)
DEFINE_CARBON_TYPE_LQ_FUNC(carbon_i8)
DEFINE_CARBON_TYPE_LQ_FUNC(carbon_i16)
DEFINE_CARBON_TYPE_LQ_FUNC(carbon_i32)
DEFINE_CARBON_TYPE_LQ_FUNC(carbon_i64)
DEFINE_CARBON_TYPE_LQ_FUNC(carbon_u8)
DEFINE_CARBON_TYPE_LQ_FUNC(carbon_u16)
DEFINE_CARBON_TYPE_LQ_FUNC(carbon_u32)
DEFINE_CARBON_TYPE_LQ_FUNC(carbon_u64)

static bool compare_encoded_string_less_eq_func(const void *lhs, const void *rhs, void *args)
{
    carbon_strdic_t *dic = (carbon_strdic_t *) args;
    carbon_string_id_t *a = (carbon_string_id_t *) lhs;
    carbon_string_id_t *b = (carbon_string_id_t *) rhs;
    char **a_string = carbon_strdic_extract(dic, a, 1);
    char **b_string = carbon_strdic_extract(dic, b, 1);
    bool lq = strcmp(*a_string, *b_string) <= 0;
    carbon_strdic_free(dic, a_string);
    carbon_strdic_free(dic, b_string);
    return lq;
}

static void sort_nested_primitive_object(columndoc_obj_t *columndoc)
{
    if (columndoc->parent->read_optimized) {
        for (size_t i = 0; i < columndoc->obj_prop_vals.num_elems; i++) {
            columndoc_obj_t *nestedModel = vec_get(&columndoc->obj_prop_vals, i, columndoc_obj_t);
            sort_columndoc_entries(nestedModel);
        }
    }
}

#define DEFINE_CARBON_ARRAY_TYPE_LQ_FUNC(type)                                                                         \
static bool compare##type##ArrayLessEqFunc(const void *lhs, const void *rhs)                                           \
{                                                                                                                      \
    vec_t ofType(type) *a = (vec_t *) lhs;                                                               \
    vec_t ofType(type) *b = (vec_t *) rhs;                                                               \
    const type *aValues = vec_all(a, type);                                                                  \
    const type *bValues = vec_all(b, type);                                                                  \
    size_t max_compare_idx = a->num_elems < b->num_elems ? a->num_elems : b->num_elems;                                \
    for (size_t i = 0; i < max_compare_idx; i++) {                                                                     \
        if (aValues[i] > bValues[i]) {                                                                                 \
            return false;                                                                                              \
        }                                                                                                              \
    }                                                                                                                  \
    return true;                                                                                                       \
}

DEFINE_CARBON_ARRAY_TYPE_LQ_FUNC(carbon_boolean_t)
DEFINE_CARBON_ARRAY_TYPE_LQ_FUNC(carbon_i8)
DEFINE_CARBON_ARRAY_TYPE_LQ_FUNC(carbon_i16)
DEFINE_CARBON_ARRAY_TYPE_LQ_FUNC(carbon_i32)
DEFINE_CARBON_ARRAY_TYPE_LQ_FUNC(carbon_i64)
DEFINE_CARBON_ARRAY_TYPE_LQ_FUNC(carbon_u8)
DEFINE_CARBON_ARRAY_TYPE_LQ_FUNC(carbon_u16)
DEFINE_CARBON_ARRAY_TYPE_LQ_FUNC(carbon_u32)
DEFINE_CARBON_ARRAY_TYPE_LQ_FUNC(carbon_u64)
DEFINE_CARBON_ARRAY_TYPE_LQ_FUNC(carbon_number_t)

static bool compare_encoded_string_array_less_eq_func(const void *lhs, const void *rhs, void *args)
{
    carbon_strdic_t *dic = (carbon_strdic_t *) args;
    vec_t ofType(carbon_string_id_t) *a = (vec_t *) lhs;
    vec_t ofType(carbon_string_id_t) *b = (vec_t *) rhs;
    const carbon_string_id_t *aValues = vec_all(a, carbon_string_id_t);
    const carbon_string_id_t *bValues = vec_all(b, carbon_string_id_t);
    size_t max_compare_idx = a->num_elems < b->num_elems ? a->num_elems : b->num_elems;
    for (size_t i = 0; i < max_compare_idx; i++) {
        char **aString = carbon_strdic_extract(dic, aValues + i, 1);
        char **bString = carbon_strdic_extract(dic, bValues + i, 1);
        bool greater = strcmp(*aString, *bString) > 0;
        carbon_strdic_free(dic, aString);
        carbon_strdic_free(dic, bString);
        if (greater) {
            return false;
        }
    }
    return true;
}

static void sorted_nested_array_objects(columndoc_obj_t *columndoc)
{
    if (columndoc->parent->read_optimized) {
        for (size_t i = 0; i < columndoc->obj_array_props.num_elems; i++) {
            carbon_columndoc_columngroup_t *array_columns = vec_get(&columndoc->obj_array_props, i, carbon_columndoc_columngroup_t);
            for (size_t j = 0; j < array_columns->columns.num_elems; j++) {
                carbon_columndoc_column_t *column = vec_get(&array_columns->columns, j, carbon_columndoc_column_t);
                vec_t ofType(u32) *array_indices = &column->array_positions;
                vec_t ofType(vec_t ofType(<T>)) *values_for_indicies = &column->values;
                assert (array_indices->num_elems == values_for_indicies->num_elems);

                for (size_t k = 0; k < array_indices->num_elems; k++) {
                    vec_t ofType(<T>) *values_for_index = vec_get(values_for_indicies, k, vec_t);
                    if (column->type == field_object) {
                        for (size_t l = 0; l < values_for_index->num_elems; l++) {
                            columndoc_obj_t *nested_object = vec_get(values_for_index, l, columndoc_obj_t);
                            sort_columndoc_entries(nested_object);
                        }
                    }
                }
            }
        }
    }
}

#define SORT_META_MODEL_VALUES(key_vector, value_vector, value_type, compareValueFunc)                                 \
{                                                                                                                      \
    size_t num_elements = carbon_vec_length(&key_vector);                                                              \
                                                                                                                       \
    if (num_elements > 0) {                                                                                            \
        size_t *value_indicies = malloc(sizeof(size_t) * num_elements);                                                \
        for (size_t i = 0; i < num_elements; i++) {                                                                    \
            value_indicies[i] = i;                                                                                     \
        }                                                                                                              \
                                                                                                                       \
        vec_t ofType(carbon_string_id_t) key_cpy;                                                               \
        vec_t ofType(value_type) value_cpy;                                                                     \
                                                                                                                       \
        carbon_vec_cpy(&key_cpy, &key_vector);                                                                         \
        carbon_vec_cpy(&value_cpy, &value_vector);                                                                     \
                                                                                                                       \
        value_type *values = vec_all(&value_cpy, value_type);                                                \
                                                                                                                       \
        carbon_sort_qsort_indicies(value_indicies, values, sizeof(value_type), compareValueFunc, num_elements,         \
                      key_vector.allocator);                                                                           \
                                                                                                                       \
        for (size_t i = 0; i < num_elements; i++) {                                                                    \
            carbon_vec_set(&key_vector, i, vec_get(&key_cpy, value_indicies[i], carbon_string_id_t));        \
            carbon_vec_set(&value_vector, i, vec_get(&value_cpy, value_indicies[i], value_type));            \
        }                                                                                                              \
                                                                                                                       \
                                                                                                                       \
        free(value_indicies);                                                                                          \
        carbon_vec_drop(&key_cpy);                                                                                     \
        carbon_vec_drop(&value_cpy);                                                                                   \
    }                                                                                                                  \
}

static void sort_meta_model_string_values(vec_t ofType(carbon_string_id_t) *key_vector, vec_t ofType(carbon_string_id_t) *value_vector,
                                      carbon_strdic_t *dic)
{
    size_t num_elements = carbon_vec_length(key_vector);

    if (num_elements > 0) {
        size_t *value_indicies = malloc(sizeof(size_t) * num_elements);
        for (size_t i = 0; i < num_elements; i++) {
            value_indicies[i] = i;
        }

        vec_t ofType(carbon_string_id_t) key_cpy;
        vec_t ofType(carbon_string_id_t) value_cpy;

        carbon_vec_cpy(&key_cpy, key_vector);
        carbon_vec_cpy(&value_cpy, value_vector);

        carbon_string_id_t *values = vec_all(&value_cpy, carbon_string_id_t);

        carbon_sort_qsort_indicies_wargs(value_indicies,
                                         values,
                                         sizeof(carbon_string_id_t),
                                         compare_encoded_string_less_eq_func,
                                         num_elements,
                                         key_vector->allocator,
                                         dic);

        for (size_t i = 0; i < num_elements; i++) {
            carbon_vec_set(key_vector, i, vec_get(&key_cpy, value_indicies[i], carbon_string_id_t));
            carbon_vec_set(value_vector, i, vec_get(&value_cpy, value_indicies[i], carbon_string_id_t));
        }

        free(value_indicies);
        carbon_vec_drop(&key_cpy);
        carbon_vec_drop(&value_cpy);
    }
}

#define SORT_META_MODEL_ARRAYS(key_vector, value_array_vector, compare_func)                                           \
{                                                                                                                      \
    size_t num_elements = carbon_vec_length(&key_vector);                                                              \
                                                                                                                       \
    if (num_elements > 0) {                                                                                            \
        size_t *value_indicies = malloc(sizeof(size_t) * num_elements);                                                \
        for (size_t i = 0; i < num_elements; i++) {                                                                    \
            value_indicies[i] = i;                                                                                     \
        }                                                                                                              \
                                                                                                                       \
        vec_t ofType(carbon_string_id_t) key_cpy;                                                               \
        vec_t ofType(vec_t) value_cpy;                                                                   \
                                                                                                                       \
        carbon_vec_cpy(&key_cpy, &key_vector);                                                                         \
        carbon_vec_cpy(&value_cpy, &value_array_vector);                                                               \
                                                                                                                       \
        const vec_t *values = vec_all(&value_array_vector, vec_t);                             \
                                                                                                                       \
        carbon_sort_qsort_indicies(value_indicies, values, sizeof(vec_t), compare_func, num_elements,           \
                      key_vector.allocator);                                                                           \
                                                                                                                       \
        for (size_t i = 0; i < num_elements; i++) {                                                                    \
            carbon_vec_set(&key_vector, i, vec_get(&key_cpy, value_indicies[i], carbon_string_id_t));        \
            carbon_vec_set(&value_array_vector, i, vec_get(&value_cpy, value_indicies[i], vec_t));    \
        }                                                                                                              \
                                                                                                                       \
        free(value_indicies);                                                                                          \
        carbon_vec_drop(&key_cpy);                                                                                     \
        carbon_vec_drop(&value_cpy);                                                                                   \
    }                                                                                                                  \
}

static void sort_columndoc_strings_arrays(vec_t ofType(carbon_string_id_t) *key_vector, vec_t ofType(carbon_string_id_t) *value_array_vector,
                                      carbon_strdic_t *dic)
{
    size_t num_elements = carbon_vec_length(key_vector);

    if (num_elements > 0) {
        size_t *value_indicies = malloc(sizeof(size_t) * num_elements);
        for (size_t i = 0; i < num_elements; i++) {
            value_indicies[i] = i;
        }

        vec_t ofType(carbon_string_id_t) key_cpy;
        vec_t ofType(vec_t) value_cpy;

        carbon_vec_cpy(&key_cpy, key_vector);
        carbon_vec_cpy(&value_cpy, value_array_vector);

        const vec_t *values = vec_all(value_array_vector, vec_t);

        carbon_sort_qsort_indicies_wargs(value_indicies,
                                         values,
                                         sizeof(vec_t),
                                         compare_encoded_string_array_less_eq_func,
                                         num_elements,
                                         key_vector->allocator,
                                         dic);

        for (size_t i = 0; i < num_elements; i++) {
            carbon_vec_set(key_vector, i, vec_get(&key_cpy, value_indicies[i], carbon_string_id_t));
            carbon_vec_set(value_array_vector, i, vec_get(&value_cpy, value_indicies[i], vec_t));
        }

        free(value_indicies);
        carbon_vec_drop(&key_cpy);
        carbon_vec_drop(&value_cpy);
    }
}


static bool compare_object_array_key_columns_less_eq_func(const void *lhs, const void *rhs, void *args)
{
    carbon_strdic_t *dic = (carbon_strdic_t *) args;
    carbon_columndoc_columngroup_t *a = (carbon_columndoc_columngroup_t *) lhs;
    carbon_columndoc_columngroup_t *b = (carbon_columndoc_columngroup_t *) rhs;
    char **a_column_name = carbon_strdic_extract(dic, &a->key, 1);
    char **b_column_name = carbon_strdic_extract(dic, &b->key, 1);
    bool column_name_leq = strcmp(*a_column_name, *b_column_name) <= 0;
    carbon_strdic_free(dic, a_column_name);
    carbon_strdic_free(dic, b_column_name);
    return column_name_leq;
}

static bool compare_object_array_key_column_less_eq_func(const void *lhs, const void *rhs, void *args)
{
    carbon_strdic_t *dic = (carbon_strdic_t *) args;
    carbon_columndoc_column_t *a = (carbon_columndoc_column_t *) lhs;
    carbon_columndoc_column_t *b = (carbon_columndoc_column_t *) rhs;
    char **a_column_name = carbon_strdic_extract(dic, &a->key_name, 1);
    char **b_column_name = carbon_strdic_extract(dic, &b->key_name, 1);
    int cmpResult = strcmp(*a_column_name, *b_column_name);
    bool column_name_leq = cmpResult < 0 ? true : (cmpResult == 0 ? (a->type <= b->type) : false);
    carbon_strdic_free(dic, a_column_name);
    carbon_strdic_free(dic, b_column_name);
    return column_name_leq;
}

typedef struct {
    carbon_strdic_t *dic;
    field_e value_type;
} compare_column_less_eq_func_arg;

#define ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, type, valueVectorAPtr, valueVectorBPtr)                                 \
{                                                                                                                      \
    for (size_t i = 0; i < max_num_elem; i++) {                                                                        \
        type o1 = *vec_get(valueVectorAPtr, i, type);                                                        \
        type o2 = *vec_get(valueVectorBPtr, i, type);                                                        \
        if (o1 > o2) {                                                                                                 \
            return false;                                                                                              \
        }                                                                                                              \
    }                                                                                                                  \
    return true;                                                                                                       \
}

static bool compare_column_less_eq_func(const void *lhs, const void *rhs, void *args)
{
    vec_t ofType(<T>) *a = (vec_t *) lhs;
    vec_t ofType(<T>) *b = (vec_t *) rhs;
    compare_column_less_eq_func_arg *func_arg = (compare_column_less_eq_func_arg *) args;

    size_t max_num_elem = CARBON_MIN(a->num_elems, b->num_elems);

    switch (func_arg->value_type) {
    case field_null:
        return (a->num_elems <= b->num_elems);
        break;
    case field_bool:
        ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, carbon_boolean_t, a, b);
        break;
    case field_int8:
        ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, carbon_i8, a, b);
        break;
    case field_int16:
        ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, carbon_i16, a, b);
        break;
    case field_int32:
        ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, carbon_i32, a, b);
        break;
    case field_int64:
        ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, carbon_i64, a, b);
        break;
    case field_uint8:
        ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, carbon_u8, a, b);
        break;
    case field_uint16:
        ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, carbon_u16, a, b);
        break;
    case field_uint32:
        ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, carbon_u32, a, b);
        break;
    case field_uint64:
        ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, carbon_u64, a, b);
        break;
    case field_float:
        ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, carbon_number_t, a, b);
        break;
    case field_string:
        for (size_t i = 0; i < max_num_elem; i++) {
            carbon_string_id_t o1 = *vec_get(a, i, carbon_string_id_t);
            carbon_string_id_t o2 = *vec_get(b, i, carbon_string_id_t);
            char **o1_string = carbon_strdic_extract(func_arg->dic, &o1, 1);
            char **o2_string = carbon_strdic_extract(func_arg->dic, &o2, 1);
            bool greater = strcmp(*o1_string, *o2_string) > 0;
            carbon_strdic_free(func_arg->dic, o1_string);
            carbon_strdic_free(func_arg->dic, o2_string);
            if (greater) {
                return false;
            }
        }
        return true;
    case field_object:
        return true;
        break;
    default:
        carbon_print_error_and_die(CARBON_ERR_NOTYPE)
        return false;
    }
}

static void sort_columndoc_column(carbon_columndoc_column_t *column, carbon_strdic_t *dic)
{
    /** Sort column by its value, and re-arrange the array position list according this new order */
    vec_t ofType(u32) array_position_cpy;
    vec_t ofType(vec_t ofType(<T>)) values_cpy;

    carbon_vec_cpy(&array_position_cpy, &column->array_positions);
    carbon_vec_cpy(&values_cpy, &column->values);

    assert(column->array_positions.num_elems == column->values.num_elems);
    assert(array_position_cpy.num_elems == values_cpy.num_elems);
    assert(values_cpy.num_elems == column->array_positions.num_elems);

    size_t *indices = malloc(values_cpy.num_elems * sizeof(size_t));
    for (size_t i = 0; i < values_cpy.num_elems; i++) {
        indices[i] = i;
    }

    compare_column_less_eq_func_arg func_arg = {
        .dic = dic,
        .value_type = column->type
    };

    carbon_sort_qsort_indicies_wargs(indices,
                                     values_cpy.base,
                                     values_cpy.elem_size,
                                     compare_column_less_eq_func,
                                     values_cpy.num_elems,
                                     values_cpy.allocator,
                                     &func_arg);

    for (size_t i = 0; i < values_cpy.num_elems; i++) {
        carbon_vec_set(&column->values, i, carbon_vec_at(&values_cpy, indices[i]));
        carbon_vec_set(&column->array_positions, i, carbon_vec_at(&array_position_cpy, indices[i]));
    }

    free (indices);
    carbon_vec_drop(&array_position_cpy);
    carbon_vec_drop(&values_cpy);
}

static void sort_columndoc_column_arrays(columndoc_obj_t *columndoc)
{
    vec_t ofType(carbon_columndoc_columngroup_t) cpy;
    carbon_vec_cpy(&cpy, &columndoc->obj_array_props);
    size_t *indices = malloc(cpy.num_elems * sizeof(size_t));
    for (size_t i = 0; i < cpy.num_elems; i++) {
        indices[i] = i;
    }
    carbon_sort_qsort_indicies_wargs(indices,
                                     cpy.base,
                                     sizeof(carbon_columndoc_columngroup_t),
                                     compare_object_array_key_columns_less_eq_func,
                                     cpy.num_elems,
                                     cpy.allocator,
                                     columndoc->parent->dic);
    for (size_t i = 0; i < cpy.num_elems; i++) {
        carbon_vec_set(&columndoc->obj_array_props, i, vec_get(&cpy, indices[i], carbon_columndoc_columngroup_t));
    }
    free(indices);

    for (size_t i = 0; i < cpy.num_elems; i++) {
        carbon_columndoc_columngroup_t *key_columns = vec_get(&columndoc->obj_array_props, i, carbon_columndoc_columngroup_t);
        size_t *columnIndices = malloc(key_columns->columns.num_elems * sizeof(size_t));
        vec_t ofType(carbon_columndoc_column_t) columnCpy;
        carbon_vec_cpy(&columnCpy, &key_columns->columns);
        for (size_t i = 0; i < key_columns->columns.num_elems; i++) {
            columnIndices[i] = i;
        }

        /** First, sort by column name; Then, sort by columns with same name by type */
        carbon_sort_qsort_indicies_wargs(columnIndices, columnCpy.base, sizeof(carbon_columndoc_column_t),
                                         compare_object_array_key_column_less_eq_func, key_columns->columns.num_elems,
                                         key_columns->columns.allocator, columndoc->parent->dic);
        for (size_t i = 0; i < key_columns->columns.num_elems; i++) {
            carbon_vec_set(&key_columns->columns, i, vec_get(&columnCpy, columnIndices[i], carbon_columndoc_column_t));
            carbon_columndoc_column_t *column = vec_get(&key_columns->columns, i, carbon_columndoc_column_t);
            sort_columndoc_column(column, columndoc->parent->dic);
        }

        carbon_vec_drop(&columnCpy);
        free(columnIndices);
    }
    carbon_vec_drop(&cpy);
}

static void sort_columndoc_values(columndoc_obj_t *columndoc)
{
    if (columndoc->parent->read_optimized) {
        SORT_META_MODEL_VALUES(columndoc->bool_prop_keys, columndoc->bool_prop_vals, carbon_boolean_t,
                               comparecarbon_boolean_tLessEqFunc);
        SORT_META_MODEL_VALUES(columndoc->int8_prop_keys, columndoc->int8_prop_vals, carbon_i8,
                               comparecarbon_i8LessEqFunc);
        SORT_META_MODEL_VALUES(columndoc->int16_prop_keys, columndoc->int16_prop_vals, carbon_i16,
                               comparecarbon_i16LessEqFunc);
        SORT_META_MODEL_VALUES(columndoc->int32_prop_keys, columndoc->int32_prop_vals, carbon_i32,
                               comparecarbon_i32LessEqFunc);
        SORT_META_MODEL_VALUES(columndoc->int64_prop_keys, columndoc->int64_prop_vals, carbon_i64,
                               comparecarbon_i64LessEqFunc);
        SORT_META_MODEL_VALUES(columndoc->uint8_prop_keys, columndoc->uint8_prop_vals, carbon_u8,
                               comparecarbon_u8LessEqFunc);
        SORT_META_MODEL_VALUES(columndoc->uint16_prop_keys, columndoc->uint16_prop_vals, carbon_u16,
                               comparecarbon_u16LessEqFunc);
        SORT_META_MODEL_VALUES(columndoc->uin32_prop_keys, columndoc->uint32_prop_vals, carbon_u32,
                               comparecarbon_u32LessEqFunc);
        SORT_META_MODEL_VALUES(columndoc->uint64_prop_keys, columndoc->uint64_prop_vals, carbon_u64,
                               comparecarbon_u64LessEqFunc);
        SORT_META_MODEL_VALUES(columndoc->float_prop_keys, columndoc->float_prop_vals, carbon_number_t,
                               comparecarbon_number_tLessEqFunc);
        sort_meta_model_string_values(&columndoc->string_prop_keys, &columndoc->string_prop_vals,
                                  columndoc->parent->dic);

        SORT_META_MODEL_ARRAYS(columndoc->bool_array_prop_keys, columndoc->bool_array_prop_vals,
                               comparecarbon_boolean_tArrayLessEqFunc);
        SORT_META_MODEL_ARRAYS(columndoc->int8_array_prop_keys, columndoc->int8_array_prop_vals,
                               comparecarbon_i8ArrayLessEqFunc);
        SORT_META_MODEL_ARRAYS(columndoc->int16_array_prop_keys, columndoc->int16_array_prop_vals,
                               comparecarbon_i16ArrayLessEqFunc);
        SORT_META_MODEL_ARRAYS(columndoc->int32_array_prop_keys, columndoc->int32_array_prop_vals,
                               comparecarbon_i32ArrayLessEqFunc);
        SORT_META_MODEL_ARRAYS(columndoc->int64_array_prop_keys, columndoc->int64_array_prop_vals,
                               comparecarbon_i64ArrayLessEqFunc);
        SORT_META_MODEL_ARRAYS(columndoc->uint8_array_prop_keys, columndoc->uint8_array_prop_vals,
                               comparecarbon_u8ArrayLessEqFunc);
        SORT_META_MODEL_ARRAYS(columndoc->uint16_array_prop_keys, columndoc->uint16_array_prop_vals,
                               comparecarbon_u16ArrayLessEqFunc);
        SORT_META_MODEL_ARRAYS(columndoc->uint32_array_prop_keys, columndoc->uint32_array_prop_vals,
                               comparecarbon_u32ArrayLessEqFunc);
        SORT_META_MODEL_ARRAYS(columndoc->uint64_array_prop_keys, columndoc->ui64_array_prop_vals,
                               comparecarbon_u64ArrayLessEqFunc);
        SORT_META_MODEL_ARRAYS(columndoc->float_array_prop_keys, columndoc->float_array_prop_vals,
                               comparecarbon_number_tArrayLessEqFunc);
        sort_columndoc_strings_arrays(&columndoc->string_array_prop_keys, &columndoc->string_array_prop_vals,
                                  columndoc->parent->dic);

        sort_columndoc_column_arrays(columndoc);
    }
}

static void sort_columndoc_entries(columndoc_obj_t *columndoc)
{
    if (columndoc->parent->read_optimized) {
        sort_columndoc_values(columndoc);
        sort_nested_primitive_object(columndoc);
        sorted_nested_array_objects(columndoc);
    }
}

carbon_columndoc_t *carbon_doc_entries_to_columndoc(const carbon_doc_bulk_t *bulk,
                                                          const carbon_doc_entries_t *partition,
                                                          bool read_optimized)
{
    if (!bulk || !partition) {
        return NULL;
    }

    // Step 1: encode all strings at once in a bulk
    char *const* key_strings = vec_all(&bulk->keys, char *);
    char *const* valueStrings = vec_all(&bulk->values, char *);
    carbon_strdic_insert(bulk->dic, NULL, key_strings, carbon_vec_length(&bulk->keys), 0);
    carbon_strdic_insert(bulk->dic, NULL, valueStrings, carbon_vec_length(&bulk->values), 0);

    // Step 2: for each document doc, create a meta doc, and construct a binary compressed document
    const carbon_doc_t *models = vec_all(&bulk->models, carbon_doc_t);
    assert (bulk->models.num_elems == 1);

    const carbon_doc_t *model = models;

    // TODO: DEBUG remove these lines
   // fprintf(stdout, "\nDocument Model:\n");
   // DocumentModelPrint(stdout, doc);

    carbon_columndoc_t *columndoc = malloc(sizeof(carbon_columndoc_t));
    columndoc->read_optimized = read_optimized;
    struct err err;
    if (!carbon_columndoc_create(columndoc, &err, model, bulk, partition, bulk->dic)) {
        carbon_error_print_and_abort(&err);
    }


    if (columndoc->read_optimized) {
        sort_columndoc_entries(&columndoc->columndoc);
    }


  //  fprintf(stdout, "\nDocument Meta Model:\n");
  //  DocumentMetaModelPrint(stdout, columndoc);

    return columndoc;
}

CARBON_EXPORT(bool)
carbon_doc_entries_drop(carbon_doc_entries_t *partition)
{
    CARBON_UNUSED(partition);
    return true;
}

static void create_doc(carbon_doc_obj_t *model, carbon_doc_t *doc)
{
    carbon_vec_create(&model->entries, NULL, sizeof(carbon_doc_entries_t), 50);
    model->doc = doc;
}

static void create_typed_vector(carbon_doc_entries_t *entry)
{
    size_t size;
    switch (entry->type) {
    case field_null:
        size = sizeof(carbon_null_t);
        break;
    case field_bool:
        size = sizeof(carbon_boolean_t);
        break;
    case field_int8:
        size = sizeof(carbon_i8);
        break;
    case field_int16:
        size = sizeof(carbon_i16);
        break;
    case field_int32:
        size = sizeof(carbon_i32);
        break;
    case field_int64:
        size = sizeof(carbon_i64);
        break;
    case field_uint8:
        size = sizeof(carbon_u8);
        break;
    case field_uint16:
        size = sizeof(carbon_u16);
        break;
    case field_uint32:
        size = sizeof(carbon_u32);
        break;
    case field_uint64:
        size = sizeof(carbon_u64);
        break;
    case field_float:
        size = sizeof(carbon_number_t);
        break;
    case field_string:
        size = sizeof(carbon_cstring_t);
        break;
    case field_object:
        size = sizeof(carbon_doc_obj_t);
        break;
    default:
        carbon_print_error_and_die(CARBON_ERR_INTERNALERR) /** unknown type */
        return;
    }
    carbon_vec_create(&entry->values, NULL, size, 50);
}

static void entries_drop(carbon_doc_entries_t *entry)
{
    if (entry->type == field_object) {
        for (size_t i = 0; i < entry->values.num_elems; i++)
        {
            carbon_doc_obj_t *model = vec_get(&entry->values, i, carbon_doc_obj_t);
            carbon_doc_drop(model);
        }
    }
    carbon_vec_drop(&entry->values);
}

static bool print_value(FILE *file, field_e type, const vec_t ofType(<T>) *values)
{
    size_t num_values = values->num_elems;
    if (num_values == 0) {
        fprintf(file, "null");
        return true;
    }
    if (num_values > 1) {
        fprintf(file, "[");
    }
    switch (type) {
    case field_null:
    {
        for (size_t i = 0; i < num_values; i++) {
            fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
        }
    } break;
    case field_bool:
    {
        for (size_t i = 0; i < num_values; i++) {
            carbon_boolean_t value = *(vec_get(values, i, carbon_boolean_t));
            if (value != CARBON_NULL_BOOLEAN) {
                fprintf(file, "%s%s", value == 0 ? "false" : "true", i + 1 < num_values ? ", " : "");
            } else {
                fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
            }
        }
    } break;
    case field_int8:
    {
        for (size_t i = 0; i < num_values; i++) {
            carbon_i8 value = *(vec_get(values, i, carbon_i8));
            if (value != CARBON_NULL_INT8) {
                fprintf(file, "%d%s", value, i + 1 < num_values ? ", " : "");
            } else {
                fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
            }
        }
    } break;
    case field_int16:
    {
        for (size_t i = 0; i < num_values; i++) {
            carbon_i16 value = *(vec_get(values, i, carbon_i16));
            if (value != CARBON_NULL_INT16) {
                fprintf(file, "%d%s", value, i + 1 < num_values ? ", " : "");
            } else {
                fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
            }
        }
    } break;
    case field_int32:
    {
        for (size_t i = 0; i < num_values; i++) {
            carbon_i32 value = *(vec_get(values, i, carbon_i32));
            if (value != CARBON_NULL_INT32) {
                fprintf(file, "%d%s", value, i + 1 < num_values ? ", " : "");
            } else {
                fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
            }
        }
    } break;
    case field_int64:
    {
        for (size_t i = 0; i < num_values; i++) {
            carbon_i64 value = *(vec_get(values, i, carbon_i64));
            if (value != CARBON_NULL_INT64) {
                fprintf(file, "%" PRIi64 "%s", value, i + 1 < num_values ? ", " : "");
            } else {
                fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
            }
        }
    } break;
    case field_uint8:
    {
        for (size_t i = 0; i < num_values; i++) {
            carbon_u8 value = *(vec_get(values, i, carbon_u8));
            if (value != CARBON_NULL_UINT8) {
                fprintf(file, "%d%s", value, i + 1 < num_values ? ", " : "");
            } else {
                fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
            }
        }
    } break;
    case field_uint16:
    {
        for (size_t i = 0; i < num_values; i++) {
            carbon_u16 value = *(vec_get(values, i, carbon_u16));
            if (value != CARBON_NULL_UINT16) {
                fprintf(file, "%d%s", value, i + 1 < num_values ? ", " : "");
            } else {
                fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
            }
        }
    } break;
    case field_uint32:
    {
        for (size_t i = 0; i < num_values; i++) {
            carbon_u32 value = *(vec_get(values, i, carbon_u32));
            if (value != CARBON_NULL_UINT32) {
                fprintf(file, "%d%s", value, i + 1 < num_values ? ", " : "");
            } else {
                fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
            }
        }
    } break;
    case field_uint64:
    {
        for (size_t i = 0; i < num_values; i++) {
            carbon_u64 value = *(vec_get(values, i, carbon_u64));
            if (value != CARBON_NULL_UINT64) {
                fprintf(file, "%" PRIu64 "%s", value, i + 1 < num_values ? ", " : "");
            } else {
                fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
            }
        }
    } break;
    case field_float:
    {
        for (size_t i = 0; i < num_values; i++) {
            carbon_number_t value = *(vec_get(values, i, carbon_number_t));
            if (!isnan(value)) {
                fprintf(file, "%f%s", value, i + 1 < num_values ? ", " : "");
            } else {
                fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
            }
        }
    } break;
    case field_string:
    {
        for (size_t i = 0; i < num_values; i++) {
            carbon_cstring_t value = *(vec_get(values, i, carbon_cstring_t));
            if (value) {
                fprintf(file, "\"%s\"%s", value, i + 1 < num_values ? ", " : "");
            } else {
                fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
            }
        }
    } break;
    case field_object:
    {
        for (size_t i = 0; i < num_values; i++) {
            carbon_doc_obj_t *obj = vec_get(values, i, carbon_doc_obj_t);
            if (!CARBON_NULL_OBJECT_MODEL(obj)) {
                print_object(file, obj);
            } else {
                fprintf(file, "null");
            }
            fprintf(file, "%s", i + 1 < num_values ? ", " : "");
        }
    } break;
    default:
        CARBON_NOT_IMPLEMENTED;
    }
    if (num_values > 1) {
        fprintf(file, "]");
    }
    return true;
}

static void print_object(FILE *file, const carbon_doc_obj_t *model)
{
    fprintf(file, "{");
    for (size_t i = 0; i < model->entries.num_elems; i++) {
        carbon_doc_entries_t *entry = vec_get(&model->entries, i, carbon_doc_entries_t);
        fprintf(file, "\"%s\": ", entry->key);
        print_value(file, entry->type, &entry->values);
        fprintf(file, "%s", i + 1 < model->entries.num_elems ? ", " : "");
    }
    fprintf(file, "}");
}
