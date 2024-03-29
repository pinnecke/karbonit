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
#include <karbonit/json/json-parser.h>
#include <karbonit/archive/doc.h>
#include <karbonit/archive/column_doc.h>
#include <karbonit/json/json-parser.h>
#include <karbonit/utils/sort.h>

char VALUE_NULL = '\0';

static void create_doc(doc_obj *model, doc *doc);

static void create_typed_vector(doc_entries *entry);

static void entries_drop(doc_entries *entry);

static bool print_value(FILE *file, archive_field_e type, const vec ofType(<T>) *values);

static void _doc_print_object(FILE *file, const doc_obj *model);

static bool
import_json_object(doc_obj *target, const json_object *json_obj);

static void sort_columndoc_entries(column_doc_obj *columndoc);

bool doc_bulk_create(doc_bulk *bulk, string_dict *dic)
{
        bulk->dic = dic;
        vec_create(&bulk->keys, sizeof(char *), 500);
        vec_create(&bulk->values, sizeof(char *), 1000);
        vec_create(&bulk->models, sizeof(doc), 50);
        return true;
}

doc_obj *doc_bulk_new_obj(doc *model)
{
        if (!model) {
                return NULL;
        } else {
                doc_obj *retval = VEC_NEW_AND_GET(&model->obj_model, doc_obj);
                create_doc(retval, model);
                return retval;
        }
}

bool doc_bulk_get_dic_contents(vec ofType (const char *) **strings,
                               vec ofType(archive_field_sid_t) **string_ids,
                               const doc_bulk *context)
{
        size_t num_distinct_values;
        string_dict_num_distinct(&num_distinct_values, context->dic);
        vec ofType (const char *) *result_strings = MALLOC(sizeof(vec));
        vec ofType (archive_field_sid_t) *resultstring_id_ts = MALLOC(sizeof(vec));
        vec_create(result_strings, sizeof(const char *), num_distinct_values);
        vec_create(resultstring_id_ts, sizeof(archive_field_sid_t), num_distinct_values);

        int status = string_dict_get_contents(result_strings, resultstring_id_ts, context->dic);
        CHECK_SUCCESS(status);
        *strings = result_strings;
        *string_ids = resultstring_id_ts;

        return status;
}

doc *doc_bulk_new_doc(doc_bulk *context, archive_field_e type)
{
        if (!context) {
                return NULL;
        }

        doc template, *model;
        size_t idx = VEC_LENGTH(&context->models);
        vec_push(&context->models, &template, 1);
        model = VEC_GET(&context->models, idx, doc);
        model->context = context;
        model->type = type;

        vec_create(&model->obj_model, sizeof(doc_obj), 500);

        return model;
}

bool doc_bulk_drop(doc_bulk *bulk)
{
        for (size_t i = 0; i < bulk->keys.num_elems; i++) {
                char *string = *VEC_GET(&bulk->keys, i, char *);
                free(string);
        }
        for (size_t i = 0; i < bulk->values.num_elems; i++) {
                char *string = *VEC_GET(&bulk->values, i, char *);
                free(string);
        }
        for (size_t i = 0; i < bulk->models.num_elems; i++) {
                doc *model = VEC_GET(&bulk->models, i, doc);
                for (size_t j = 0; j < model->obj_model.num_elems; j++) {
                        doc_obj *doc = VEC_GET(&model->obj_model, j, doc_obj);
                        doc_drop(doc);
                }
                vec_drop(&model->obj_model);
        }

        vec_drop(&bulk->keys);
        vec_drop(&bulk->values);
        vec_drop(&bulk->models);
        return true;
}

bool doc_bulk_shrink(doc_bulk *bulk)
{
        vec_shrink(&bulk->keys);
        vec_shrink(&bulk->values);
        return true;
}

bool doc_bulk_print(FILE *file, doc_bulk *bulk)
{
        fprintf(file, "{");
        char **key_strings = VEC_ALL(&bulk->keys, char *);
        fprintf(file, "\"Key Strings\": [");
        for (size_t i = 0; i < bulk->keys.num_elems; i++) {
                fprintf(file, "\"%s\"%s", key_strings[i], i + 1 < bulk->keys.num_elems ? ", " : "");
        }
        fprintf(file, "], ");

        char **valueStrings = VEC_ALL(&bulk->values, char *);
        fprintf(file, "\"Value Strings\": [");
        for (size_t i = 0; i < bulk->values.num_elems; i++) {
                fprintf(file, "\"%s\"%s", valueStrings[i], i + 1 < bulk->values.num_elems ? ", " : "");
        }
        fprintf(file, "]}");

        return true;
}

bool doc_print(FILE *file, const doc *doc)
{
        if (doc->obj_model.num_elems == 0) {
                fprintf(file, "{ }");
        }

        if (doc->obj_model.num_elems > 1) {
                fprintf(file, "[");
        }

        for (size_t num_entries = 0; num_entries < doc->obj_model.num_elems; num_entries++) {
                doc_obj *object = VEC_GET(&doc->obj_model, num_entries, doc_obj);
                _doc_print_object(file, object);
                fprintf(file, "%s", num_entries + 1 < doc->obj_model.num_elems ? ", " : "");
        }

        if (doc->obj_model.num_elems > 1) {
                fprintf(file, "]");
        }

        return true;
}

const vec ofType(doc_entries) *doc_get_entries(const doc_obj *model)
{
        return &model->entries;
}

void doc_print_entries(FILE *file, const doc_entries *entries)
{
        fprintf(file, "{\"Key\": \"%s\"", entries->key);
}

void doc_drop(doc_obj *model)
{
        for (size_t i = 0; i < model->entries.num_elems; i++) {
                doc_entries *entry = VEC_GET(&model->entries, i, doc_entries);
                entries_drop(entry);
        }
        vec_drop(&model->entries);
}

bool doc_obj_add_key(doc_entries **out, doc_obj *obj, const char *key, archive_field_e type)
{
        size_t entry_idx;
        char *key_dup = strdup(key);

        doc_entries entry_model = {.type = type, .key = key_dup, .context = obj};

        create_typed_vector(&entry_model);
        vec_push(&obj->doc->context->keys, &key_dup, 1);

        entry_idx = VEC_LENGTH(&obj->entries);
        vec_push(&obj->entries, &entry_model, 1);

        *out = VEC_GET(&obj->entries, entry_idx, doc_entries);

        return true;
}

bool doc_obj_push_primtive(doc_entries *entry, const void *value)
{
        switch (entry->type) {
                case ARCHIVE_FIELD_NULL:
                        vec_push(&entry->values, &VALUE_NULL, 1);
                        break;
                case ARCHIVE_FIELD_STRING: {
                        char *string = value ? strdup((char *) value) : NULL;
                        vec_push(&entry->context->doc->context->values, &string, 1);
                        vec_push(&entry->values, &string, 1);
                }
                        break;
                default:
                        vec_push(&entry->values, value, 1);
                        break;
        }
        return true;
}

bool doc_obj_push_object(doc_obj **out, doc_entries *entry)
{
        assert(entry->type == ARCHIVE_FIELD_OBJECT);

        doc_obj objectModel;

        create_doc(&objectModel, entry->context->doc);
        size_t length = VEC_LENGTH(&entry->values);
        vec_push(&entry->values, &objectModel, 1);

        *out = VEC_GET(&entry->values, length, doc_obj);

        return true;
}

static archive_field_e
value_type_for_json_number(bool *success, const json_number *number)
{
        *success = true;
        switch (number->value_type) {
                case JSON_NUMBER_FLOAT:
                        return ARCHIVE_FIELD_FLOAT;
                case JSON_NUMBER_UNSIGNED: {
                        u64 test = number->value.unsigned_integer;
                        if (test <= LIMITS_UINT8_MAX) {
                                return ARCHIVE_FIELD_UINT8;
                        } else if (test <= LIMITS_UINT16_MAX) {
                                return ARCHIVE_FIELD_UINT16;
                        } else if (test <= LIMITS_UINT32_MAX) {
                                return ARCHIVE_FIELD_UINT32;
                        } else {
                                return ARCHIVE_FIELD_UINT64;
                        }
                }
                case JSON_NUMBER_SIGNED: {
                        i64 test = number->value.signed_integer;
                        if (test >= LIMITS_INT8_MIN && test <= LIMITS_INT8_MAX) {
                                return ARCHIVE_FIELD_INT8;
                        } else if (test >= LIMITS_INT16_MIN && test <= LIMITS_INT16_MAX) {
                                return ARCHIVE_FIELD_INT16;
                        } else if (test >= LIMITS_INT32_MIN && test <= LIMITS_INT32_MAX) {
                                return ARCHIVE_FIELD_INT32;
                        } else {
                                return ARCHIVE_FIELD_INT64;
                        }
                }
                default: ERROR(ERR_NOJSONNUMBERT, NULL);
                        *success = false;
                        return ARCHIVE_FIELD_INT8;
        }
}

static void
import_json_object_string_prop(doc_obj *target, const char *key, const json_string *string)
{
        doc_entries *entry;
        doc_obj_add_key(&entry, target, key, ARCHIVE_FIELD_STRING);
        doc_obj_push_primtive(entry, string->value);
}

static bool import_json_object_number_prop(doc_obj *target, const char *key,
                                           const json_number *number)
{
        doc_entries *entry;
        bool success;
        archive_field_e number_type = value_type_for_json_number(&success, number);
        if (!success) {
                return false;
        }
        doc_obj_add_key(&entry, target, key, number_type);
        doc_obj_push_primtive(entry, &number->value);
        return true;
}

static void import_json_object_bool_prop(doc_obj *target, const char *key, archive_field_boolean_t value)
{
        doc_entries *entry;
        doc_obj_add_key(&entry, target, key, ARCHIVE_FIELD_BOOLEAN);
        doc_obj_push_primtive(entry, &value);
}

static void import_json_object_null_prop(doc_obj *target, const char *key)
{
        doc_entries *entry;
        doc_obj_add_key(&entry, target, key, ARCHIVE_FIELD_NULL);
        doc_obj_push_primtive(entry, NULL);
}

static bool import_json_object_object_prop(doc_obj *target, const char *key,
                                           const json_object *object)
{
        doc_entries *entry;
        doc_obj *nested_object = NULL;
        doc_obj_add_key(&entry, target, key, ARCHIVE_FIELD_OBJECT);
        doc_obj_push_object(&nested_object, entry);
        return import_json_object(nested_object, object);
}

static bool import_json_object_array_prop(doc_obj *target, const char *key, const json_array *array)
{
        doc_entries *entry;

        if (!vec_is_empty(&array->elements.elements)) {
                size_t num_elements = array->elements.elements.num_elems;

                /** Find first type that is not null unless the entire array is of type null */
                json_value_type_e array_data_type = JSON_VALUE_NULL;
                archive_field_e field_type;

                for (size_t i = 0; i < num_elements && array_data_type == JSON_VALUE_NULL; i++) {
                        const json_element *element = VEC_GET(&array->elements.elements, i,
                                                                         json_element);
                        array_data_type = element->value.value_type;
                }

                switch (array_data_type) {
                        case JSON_VALUE_OBJECT:
                                field_type = ARCHIVE_FIELD_OBJECT;
                                break;
                        case JSON_VALUE_STRING:
                                field_type = ARCHIVE_FIELD_STRING;
                                break;
                        case JSON_VALUE_NUMBER: {
                                /** find smallest fitting physical number type */
                                archive_field_e array_number_type = ARCHIVE_FIELD_NULL;
                                for (size_t i = 0; i < num_elements; i++) {
                                        const json_element
                                                *element = VEC_GET(&array->elements.elements, i,
                                                                   json_element);
                                        if (UNLIKELY(element->value.value_type == JSON_VALUE_NULL)) {
                                                continue;
                                        } else {
                                                bool success;
                                                archive_field_e element_number_type =
                                                        value_type_for_json_number(&success,
                                                                                   element->value.value.number);
                                                if (!success) {
                                                        return false;
                                                }
                                                assert(element_number_type == ARCHIVE_FIELD_INT8 ||
                                                           element_number_type == ARCHIVE_FIELD_INT16
                                                           || element_number_type == ARCHIVE_FIELD_INT32
                                                           || element_number_type == ARCHIVE_FIELD_INT64
                                                           || element_number_type == ARCHIVE_FIELD_UINT8
                                                           || element_number_type == ARCHIVE_FIELD_UINT16
                                                           || element_number_type == ARCHIVE_FIELD_UINT32
                                                           || element_number_type == ARCHIVE_FIELD_UINT64
                                                           || element_number_type == ARCHIVE_FIELD_FLOAT);
                                                if (UNLIKELY(array_number_type == ARCHIVE_FIELD_NULL)) {
                                                        array_number_type = element_number_type;
                                                } else {
                                                        if (array_number_type == ARCHIVE_FIELD_INT8) {
                                                                array_number_type = element_number_type;
                                                        } else if (array_number_type == ARCHIVE_FIELD_INT16) {
                                                                if (element_number_type != ARCHIVE_FIELD_INT8) {
                                                                        array_number_type = element_number_type;
                                                                }
                                                        } else if (array_number_type == ARCHIVE_FIELD_INT32) {
                                                                if (element_number_type != ARCHIVE_FIELD_INT8
                                                                    && element_number_type != ARCHIVE_FIELD_INT16) {
                                                                        array_number_type = element_number_type;
                                                                }
                                                        } else if (array_number_type == ARCHIVE_FIELD_INT64) {
                                                                if (element_number_type != ARCHIVE_FIELD_INT8
                                                                    && element_number_type != ARCHIVE_FIELD_INT16
                                                                    && element_number_type != ARCHIVE_FIELD_INT32) {
                                                                        array_number_type = element_number_type;
                                                                }
                                                        } else if (array_number_type == ARCHIVE_FIELD_UINT8) {
                                                                array_number_type = element_number_type;
                                                        } else if (array_number_type == ARCHIVE_FIELD_UINT16) {
                                                                if (element_number_type != ARCHIVE_FIELD_UINT16) {
                                                                        array_number_type = element_number_type;
                                                                }
                                                        } else if (array_number_type == ARCHIVE_FIELD_UINT32) {
                                                                if (element_number_type != ARCHIVE_FIELD_UINT8
                                                                    && element_number_type != ARCHIVE_FIELD_UINT16) {
                                                                        array_number_type = element_number_type;
                                                                }
                                                        } else if (array_number_type == ARCHIVE_FIELD_UINT64) {
                                                                if (element_number_type != ARCHIVE_FIELD_UINT8
                                                                    && element_number_type != ARCHIVE_FIELD_UINT16
                                                                    && element_number_type != ARCHIVE_FIELD_UINT32) {
                                                                        array_number_type = element_number_type;
                                                                }
                                                        } else if (array_number_type == ARCHIVE_FIELD_FLOAT) {
                                                                break;
                                                        }
                                                }
                                        }
                                }
                                assert(array_number_type != ARCHIVE_FIELD_NULL);
                                field_type = array_number_type;
                        }
                                break;
                        case JSON_VALUE_FALSE:
                        case JSON_VALUE_TRUE:
                                field_type = ARCHIVE_FIELD_BOOLEAN;
                                break;
                        case JSON_VALUE_NULL:
                                field_type = ARCHIVE_FIELD_NULL;
                                break;
                        case JSON_VALUE_ARRAY:
                                return ERROR(ERR_ERRINTERNAL, NULL) /** array type is illegal here */;
                        default:
                                return ERROR(ERR_NOTYPE, NULL);
                }

                doc_obj_add_key(&entry, target, key, field_type);

                for (size_t i = 0; i < num_elements; i++) {
                        const json_element *element = VEC_GET(&array->elements.elements, i,
                                                                         json_element);
                        json_value_type_e ast_node_data_type = element->value.value_type;

                        switch (field_type) {
                                case ARCHIVE_FIELD_OBJECT: {
                                        doc_obj *nested_object = NULL;
                                        doc_obj_push_object(&nested_object, entry);
                                        if (ast_node_data_type != JSON_VALUE_NULL) {
                                                /** the object is null by definition, if no entries are contained */
                                                if (!import_json_object(nested_object, element->value.value.object)) {
                                                        return false;
                                                }
                                        }
                                }
                                        break;
                                case ARCHIVE_FIELD_STRING: {
                                        assert(ast_node_data_type == array_data_type ||
                                                   ast_node_data_type == JSON_VALUE_NULL);
                                        doc_obj_push_primtive(entry,
                                                              ast_node_data_type == JSON_VALUE_NULL
                                                              ? NULL_ENCODED_STRING : element->value
                                                                      .value.string->value);
                                }
                                        break;
                                case ARCHIVE_FIELD_INT8:
                                case ARCHIVE_FIELD_INT16:
                                case ARCHIVE_FIELD_INT32:
                                case ARCHIVE_FIELD_INT64:
                                case ARCHIVE_FIELD_UINT8:
                                case ARCHIVE_FIELD_UINT16:
                                case ARCHIVE_FIELD_UINT32:
                                case ARCHIVE_FIELD_UINT64:
                                case ARCHIVE_FIELD_FLOAT: {
                                        assert(ast_node_data_type == array_data_type ||
                                                   ast_node_data_type == JSON_VALUE_NULL);
                                        switch (field_type) {
                                                case ARCHIVE_FIELD_INT8: {
                                                        archive_field_i8_t value =
                                                                ast_node_data_type == JSON_VALUE_NULL ? NULL_INT8
                                                                                                      : (archive_field_i8_t) element
                                                                        ->value.value.number->value.signed_integer;
                                                        doc_obj_push_primtive(entry, &value);
                                                }
                                                        break;
                                                case ARCHIVE_FIELD_INT16: {
                                                        archive_field_i16_t value =
                                                                ast_node_data_type == JSON_VALUE_NULL ? NULL_INT16
                                                                                                      : (archive_field_i16_t) element
                                                                        ->value.value.number->value.signed_integer;
                                                        doc_obj_push_primtive(entry, &value);
                                                }
                                                        break;
                                                case ARCHIVE_FIELD_INT32: {
                                                        archive_field_i32_t value =
                                                                ast_node_data_type == JSON_VALUE_NULL ? NULL_INT32
                                                                                                      : (archive_field_i32_t) element
                                                                        ->value.value.number->value.signed_integer;
                                                        doc_obj_push_primtive(entry, &value);
                                                }
                                                        break;
                                                case ARCHIVE_FIELD_INT64: {
                                                        archive_field_i64_t value =
                                                                ast_node_data_type == JSON_VALUE_NULL ? NULL_INT64
                                                                                                      : (archive_field_i64_t) element
                                                                        ->value.value.number->value.signed_integer;
                                                        doc_obj_push_primtive(entry, &value);
                                                }
                                                        break;
                                                case ARCHIVE_FIELD_UINT8: {
                                                        archive_field_u8_t value =
                                                                ast_node_data_type == JSON_VALUE_NULL ? NULL_UINT8
                                                                                                      : (archive_field_u8_t) element
                                                                        ->value.value.number->value.unsigned_integer;
                                                        doc_obj_push_primtive(entry, &value);
                                                }
                                                        break;
                                                case ARCHIVE_FIELD_UINT16: {
                                                        archive_field_u16_t value =
                                                                ast_node_data_type == JSON_VALUE_NULL ? NULL_UINT16
                                                                                                      : (archive_field_u16_t) element
                                                                        ->value.value.number->value.unsigned_integer;
                                                        doc_obj_push_primtive(entry, &value);
                                                }
                                                        break;
                                                case ARCHIVE_FIELD_UINT32: {
                                                        archive_field_u32_t value =
                                                                ast_node_data_type == JSON_VALUE_NULL ? NULL_UINT32
                                                                                                      : (archive_field_u32_t) element
                                                                        ->value.value.number->value.unsigned_integer;
                                                        doc_obj_push_primtive(entry, &value);
                                                }
                                                        break;
                                                case ARCHIVE_FIELD_UINT64: {
                                                        archive_field_u64_t value =
                                                                ast_node_data_type == JSON_VALUE_NULL ? NULL_UINT64
                                                                                                      : (archive_field_u64_t) element
                                                                        ->value.value.number->value.unsigned_integer;
                                                        doc_obj_push_primtive(entry, &value);
                                                }
                                                        break;
                                                case ARCHIVE_FIELD_FLOAT: {
                                                        archive_field_number_t value = NULL_FLOAT;
                                                        if (ast_node_data_type != JSON_VALUE_NULL) {
                                                                json_number_type_e
                                                                        element_number_type = element->value.value.number->value_type;
                                                                if (element_number_type == JSON_NUMBER_FLOAT) {
                                                                        value = element->value.value.number->value.float_number;
                                                                } else if (element_number_type ==
                                                                           JSON_NUMBER_UNSIGNED) {
                                                                        value = element->value.value.number->value.unsigned_integer;
                                                                } else if (element_number_type == JSON_NUMBER_SIGNED) {
                                                                        value = element->value.value.number->value.signed_integer;
                                                                } else {
                                                                        return ERROR(ERR_INTERNALERR, NULL) /** type mismatch */;
                                                                }
                                                        }
                                                        doc_obj_push_primtive(entry, &value);
                                                }
                                                        break;
                                                default:
                                                        return ERROR(ERR_INTERNALERR, NULL) /** not a number type  */;
                                        }
                                }
                                        break;
                                case ARCHIVE_FIELD_BOOLEAN:
                                        if (LIKELY(ast_node_data_type == JSON_VALUE_TRUE
                                                       || ast_node_data_type == JSON_VALUE_FALSE)) {
                                                archive_field_boolean_t value =
                                                        ast_node_data_type == JSON_VALUE_TRUE ? BOOLEAN_TRUE
                                                                                              : BOOLEAN_FALSE;
                                                doc_obj_push_primtive(entry, &value);
                                        } else {
                                                assert(ast_node_data_type == JSON_VALUE_NULL);
                                                archive_field_boolean_t value = NULL_BOOLEAN;
                                                doc_obj_push_primtive(entry, &value);
                                        }
                                        break;
                                case ARCHIVE_FIELD_NULL:
                                        assert(ast_node_data_type == array_data_type);
                                        doc_obj_push_primtive(entry, NULL);
                                        break;
                                default:
                                        return ERROR(ERR_NOTYPE, NULL);
                        }
                }
        } else {
                import_json_object_null_prop(target, key);
        }
        return true;
}

static bool
import_json_object(doc_obj *target, const json_object *json_obj)
{
        for (size_t i = 0; i < json_obj->value->members.num_elems; i++) {
                json_prop *member = VEC_GET(&json_obj->value->members, i, json_prop);
                json_value_type_e value_type = member->value.value.value_type;
                switch (value_type) {
                        case JSON_VALUE_STRING:
                                import_json_object_string_prop(target, member->key.value,
                                                               member->value.value.value.string);
                                break;
                        case JSON_VALUE_NUMBER:
                                if (!import_json_object_number_prop(target,
                                                                    member->key.value,
                                                                    member->value.value.value.number)) {
                                        return false;
                                }
                                break;
                        case JSON_VALUE_TRUE:
                        case JSON_VALUE_FALSE: {
                                archive_field_boolean_t value =
                                        value_type == JSON_VALUE_TRUE ? BOOLEAN_TRUE : BOOLEAN_FALSE;
                                import_json_object_bool_prop(target, member->key.value, value);
                        }
                                break;
                        case JSON_VALUE_NULL:
                                import_json_object_null_prop(target, member->key.value);
                                break;
                        case JSON_VALUE_OBJECT:
                                if (!import_json_object_object_prop(target,
                                                                    member->key.value,
                                                                    member->value.value.value.object)) {
                                        return false;
                                }
                                break;
                        case JSON_VALUE_ARRAY:
                                if (!import_json_object_array_prop(target,
                                                                   member->key.value,
                                                                   member->value.value.value.array)) {
                                        return false;
                                }
                                break;
                        default: ERROR(ERR_NOTYPE, NULL);
                                return false;
                }
        }
        return true;
}

static bool
import_json(doc_obj *target, const json *json,
            doc_entries *partition)
{
        json_value_type_e value_type = json->element->value.value_type;
        switch (value_type) {
                case JSON_VALUE_OBJECT:
                        if (!import_json_object(target, json->element->value.value.object)) {
                                return false;
                        }
                        break;
                case JSON_VALUE_ARRAY: {
                        const vec ofType(json_element)
                                *arrayContent = &json->element->value.value.array->elements.elements;
                        if (!vec_is_empty(arrayContent)) {
                                const json_element *first = VEC_GET(arrayContent, 0,
                                                                               json_element);
                                switch (first->value.value_type) {
                                        case JSON_VALUE_OBJECT:
                                                if (!import_json_object(target, first->value.value.object)) {
                                                        return false;
                                                }
                                                for (size_t i = 1; i < arrayContent->num_elems; i++) {
                                                        const json_element
                                                                *element = VEC_GET(arrayContent, i,
                                                                                   json_element);
                                                        doc_obj *nested;
                                                        doc_obj_push_object(&nested, partition);
                                                        if (!import_json_object(nested, element->value.value.object)) {
                                                                return false;
                                                        }
                                                }
                                                break;
                                        case JSON_VALUE_ARRAY:
                                        case JSON_VALUE_STRING:
                                        case JSON_VALUE_NUMBER:
                                        case JSON_VALUE_TRUE:
                                        case JSON_VALUE_FALSE:
                                        case JSON_VALUE_NULL:
                                        default: return ERROR(ERR_INTERNALERR, NULL); /** Unsupported operation in arrays */
                                }
                        }
                }
                        break;
                case JSON_VALUE_STRING:
                case JSON_VALUE_NUMBER:
                case JSON_VALUE_TRUE:
                case JSON_VALUE_FALSE:
                case JSON_VALUE_NULL:
                default: return ERROR(ERR_JSONTYPE, NULL);
        }
        return true;
}

doc_obj *doc_bulk_add_json(doc_entries *partition, json *json)
{
        if (!partition || !json) {
                return NULL;
        }

        doc_obj *converted_json;
        doc_obj_push_object(&converted_json, partition);
        if (!import_json(converted_json, json, partition)) {
                return NULL;
        }

        return converted_json;
}

doc_obj *doc_entries_get_root(const doc_entries *partition)
{
        return partition ? partition->context : NULL;
}

doc_entries *doc_bulk_new_entries(doc_bulk *dst)
{
        doc_entries *partition = NULL;
        doc *model = doc_bulk_new_doc(dst, ARCHIVE_FIELD_OBJECT);
        doc_obj *object = doc_bulk_new_obj(model);
        doc_obj_add_key(&partition, object, "/", ARCHIVE_FIELD_OBJECT);
        return partition;
}

#define DEFINE_TYPE_LQ_FUNC(type)                                                                               \
static bool compare_##type##_leq(const void *lhs, const void *rhs)                                                \
{                                                                                                                      \
    archive_##type a = *(archive_##type *) lhs;                                                                                            \
    archive_##type b = *(archive_##type *) rhs;                                                                                            \
    return (a <= b);                                                                                                   \
}

DEFINE_TYPE_LQ_FUNC(field_boolean_t)

DEFINE_TYPE_LQ_FUNC(field_number_t)

DEFINE_TYPE_LQ_FUNC(field_i8_t)

DEFINE_TYPE_LQ_FUNC(field_i16_t)

DEFINE_TYPE_LQ_FUNC(field_i32_t)

DEFINE_TYPE_LQ_FUNC(field_i64_t)

DEFINE_TYPE_LQ_FUNC(field_u8_t)

DEFINE_TYPE_LQ_FUNC(field_u16_t)

DEFINE_TYPE_LQ_FUNC(field_u32_t)

DEFINE_TYPE_LQ_FUNC(field_u64_t)

static bool compare_encoded_string_less_eq_func(const void *lhs, const void *rhs, void *args)
{
        string_dict *dic = (string_dict *) args;
        archive_field_sid_t *a = (archive_field_sid_t *) lhs;
        archive_field_sid_t *b = (archive_field_sid_t *) rhs;
        char **a_string = string_dict_extract(dic, a, 1);
        char **b_string = string_dict_extract(dic, b, 1);
        bool lq = strcmp(*a_string, *b_string) <= 0;
        string_dict_free(dic, a_string);
        string_dict_free(dic, b_string);
        return lq;
}

static void sort_nested_primitive_object(column_doc_obj *columndoc)
{
        if (columndoc->parent->read_optimized) {
                for (size_t i = 0; i < columndoc->obj_prop_vals.num_elems; i++) {
                        column_doc_obj *nestedModel = VEC_GET(&columndoc->obj_prop_vals, i,
                                                                         column_doc_obj);
                        sort_columndoc_entries(nestedModel);
                }
        }
}

#define DEFINE_ARRAY_TYPE_LQ_FUNC(type)                                                                         \
static bool compare_##type##_array_leq(const void *lhs, const void *rhs)                                           \
{                                                                                                                      \
    vec ofType(archive_##type) *a = (vec *) lhs;                                                               \
    vec ofType(archive_##type) *b = (vec *) rhs;                                                               \
    const archive_##type *aValues = VEC_ALL(a, archive_##type);                                                                  \
    const archive_##type *bValues = VEC_ALL(b, archive_##type);                                                                  \
    size_t max_compare_idx = a->num_elems < b->num_elems ? a->num_elems : b->num_elems;                                \
    for (size_t i = 0; i < max_compare_idx; i++) {                                                                     \
        if (aValues[i] > bValues[i]) {                                                                                 \
            return false;                                                                                              \
        }                                                                                                              \
    }                                                                                                                  \
    return true;                                                                                                       \
}

DEFINE_ARRAY_TYPE_LQ_FUNC(field_boolean_t)

DEFINE_ARRAY_TYPE_LQ_FUNC(field_i8_t)

DEFINE_ARRAY_TYPE_LQ_FUNC(field_i16_t)

DEFINE_ARRAY_TYPE_LQ_FUNC(field_i32_t)

DEFINE_ARRAY_TYPE_LQ_FUNC(field_i64_t)

DEFINE_ARRAY_TYPE_LQ_FUNC(field_u8_t)

DEFINE_ARRAY_TYPE_LQ_FUNC(field_u16_t)

DEFINE_ARRAY_TYPE_LQ_FUNC(field_u32_t)

DEFINE_ARRAY_TYPE_LQ_FUNC(field_u64_t)

DEFINE_ARRAY_TYPE_LQ_FUNC(field_number_t)

static bool compare_encoded_string_array_less_eq_func(const void *lhs, const void *rhs, void *args)
{
        string_dict *dic = (string_dict *) args;
        vec ofType(archive_field_sid_t) *a = (vec *) lhs;
        vec ofType(archive_field_sid_t) *b = (vec *) rhs;
        const archive_field_sid_t *aValues = VEC_ALL(a, archive_field_sid_t);
        const archive_field_sid_t *bValues = VEC_ALL(b, archive_field_sid_t);
        size_t max_compare_idx = a->num_elems < b->num_elems ? a->num_elems : b->num_elems;
        for (size_t i = 0; i < max_compare_idx; i++) {
                char **aString = string_dict_extract(dic, aValues + i, 1);
                char **bString = string_dict_extract(dic, bValues + i, 1);
                bool greater = strcmp(*aString, *bString) > 0;
                string_dict_free(dic, aString);
                string_dict_free(dic, bString);
                if (greater) {
                        return false;
                }
        }
        return true;
}

static void sorted_nested_array_objects(column_doc_obj *columndoc)
{
        if (columndoc->parent->read_optimized) {
                for (size_t i = 0; i < columndoc->obj_array_props.num_elems; i++) {
                        column_doc_group
                                *array_columns = VEC_GET(&columndoc->obj_array_props, i, column_doc_group);
                        for (size_t j = 0; j < array_columns->columns.num_elems; j++) {
                                column_doc_column
                                        *column = VEC_GET(&array_columns->columns, j, column_doc_column);
                                vec ofType(u32) *array_indices = &column->array_positions;
                                vec ofType(
                                        vec ofType(<T>)) *values_for_indicies = &column->values;
                                assert (array_indices->num_elems == values_for_indicies->num_elems);

                                for (size_t k = 0; k < array_indices->num_elems; k++) {
                                        vec ofType(<T>)
                                                *values_for_index = VEC_GET(values_for_indicies, k, vec);
                                        if (column->type == ARCHIVE_FIELD_OBJECT) {
                                                for (size_t l = 0; l < values_for_index->num_elems; l++) {
                                                        column_doc_obj *nested_object =
                                                                VEC_GET(values_for_index, l, column_doc_obj);
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
    size_t num_elements = VEC_LENGTH(&key_vector);                                                              \
                                                                                                                       \
    if (num_elements > 0) {                                                                                            \
        size_t *value_indicies = MALLOC(sizeof(size_t) * num_elements);                                                \
        for (size_t i = 0; i < num_elements; i++) {                                                                    \
            value_indicies[i] = i;                                                                                     \
        }                                                                                                              \
                                                                                                                       \
        vec ofType(archive_field_sid_t) key_cpy;                                                               \
        vec ofType(value_type) value_cpy;                                                                     \
                                                                                                                       \
        vec_cpy(&key_cpy, &key_vector);                                                                         \
        vec_cpy(&value_cpy, &value_vector);                                                                     \
                                                                                                                       \
        value_type *values = VEC_ALL(&value_cpy, value_type);                                                \
                                                                                                                       \
        sort_qsort_indicies(value_indicies, values, sizeof(value_type), compareValueFunc, num_elements);                                                                           \
                                                                                                                       \
        for (size_t i = 0; i < num_elements; i++) {                                                                    \
            vec_set(&key_vector, i, VEC_GET(&key_cpy, value_indicies[i], archive_field_sid_t));        \
            vec_set(&value_vector, i, VEC_GET(&value_cpy, value_indicies[i], value_type));            \
        }                                                                                                              \
                                                                                                                       \
                                                                                                                       \
        free(value_indicies);                                                                                          \
        vec_drop(&key_cpy);                                                                                     \
        vec_drop(&value_cpy);                                                                                   \
    }                                                                                                                  \
}

static void sort_meta_model_string_values(vec ofType(archive_field_sid_t) *key_vector,
                                          vec ofType(archive_field_sid_t) *value_vector,
                                          string_dict *dic)
{
        size_t num_elements = VEC_LENGTH(key_vector);

        if (num_elements > 0) {
                size_t *value_indicies = MALLOC(sizeof(size_t) * num_elements);
                for (size_t i = 0; i < num_elements; i++) {
                        value_indicies[i] = i;
                }

                vec ofType(archive_field_sid_t) key_cpy;
                vec ofType(archive_field_sid_t) value_cpy;

                vec_cpy(&key_cpy, key_vector);
                vec_cpy(&value_cpy, value_vector);

                archive_field_sid_t *values = VEC_ALL(&value_cpy, archive_field_sid_t);

                sort_qsort_indicies_wargs(value_indicies,
                                          values,
                                          sizeof(archive_field_sid_t),
                                          compare_encoded_string_less_eq_func,
                                          num_elements,
                                          dic);

                for (size_t i = 0; i < num_elements; i++) {
                        vec_set(key_vector, i, VEC_GET(&key_cpy, value_indicies[i], archive_field_sid_t));
                        vec_set(value_vector, i, VEC_GET(&value_cpy, value_indicies[i], archive_field_sid_t));
                }

                free(value_indicies);
                vec_drop(&key_cpy);
                vec_drop(&value_cpy);
        }
}

#define SORT_META_MODEL_ARRAYS(key_vector, value_array_vector, compare_func)                                           \
{                                                                                                                      \
    size_t num_elements = VEC_LENGTH(&key_vector);                                                              \
                                                                                                                       \
    if (num_elements > 0) {                                                                                            \
        size_t *value_indicies = MALLOC(sizeof(size_t) * num_elements);                                                \
        for (size_t i = 0; i < num_elements; i++) {                                                                    \
            value_indicies[i] = i;                                                                                     \
        }                                                                                                              \
                                                                                                                       \
        vec ofType(archive_field_sid_t) key_cpy;                                                               \
        vec ofType(vec) value_cpy;                                                                   \
                                                                                                                       \
        vec_cpy(&key_cpy, &key_vector);                                                                         \
        vec_cpy(&value_cpy, &value_array_vector);                                                               \
                                                                                                                       \
        const vec *values = VEC_ALL(&value_array_vector, vec);                             \
                                                                                                                       \
        sort_qsort_indicies(value_indicies, values, sizeof(vec), compare_func, num_elements);                                                                           \
                                                                                                                       \
        for (size_t i = 0; i < num_elements; i++) {                                                                    \
            vec_set(&key_vector, i, VEC_GET(&key_cpy, value_indicies[i], archive_field_sid_t));        \
            vec_set(&value_array_vector, i, VEC_GET(&value_cpy, value_indicies[i], vec));    \
        }                                                                                                              \
                                                                                                                       \
        free(value_indicies);                                                                                          \
        vec_drop(&key_cpy);                                                                                     \
        vec_drop(&value_cpy);                                                                                   \
    }                                                                                                                  \
}

static void sort_columndoc_strings_arrays(vec ofType(archive_field_sid_t) *key_vector,
                                          vec ofType(archive_field_sid_t) *value_array_vector,
                                          string_dict *dic)
{
        size_t num_elements = VEC_LENGTH(key_vector);

        if (num_elements > 0) {
                size_t *value_indicies = MALLOC(sizeof(size_t) * num_elements);
                for (size_t i = 0; i < num_elements; i++) {
                        value_indicies[i] = i;
                }

                vec ofType(archive_field_sid_t) key_cpy;
                vec ofType(vec) value_cpy;

                vec_cpy(&key_cpy, key_vector);
                vec_cpy(&value_cpy, value_array_vector);

                const vec *values = VEC_ALL(value_array_vector, vec);

                sort_qsort_indicies_wargs(value_indicies,
                                          values,
                                          sizeof(vec),
                                          compare_encoded_string_array_less_eq_func,
                                          num_elements,
                                          dic);

                for (size_t i = 0; i < num_elements; i++) {
                        vec_set(key_vector, i, VEC_GET(&key_cpy, value_indicies[i], archive_field_sid_t));
                        vec_set(value_array_vector, i, VEC_GET(&value_cpy, value_indicies[i], vec));
                }

                free(value_indicies);
                vec_drop(&key_cpy);
                vec_drop(&value_cpy);
        }
}

static bool compare_object_array_key_columns_less_eq_func(const void *lhs, const void *rhs, void *args)
{
        string_dict *dic = (string_dict *) args;
        column_doc_group *a = (column_doc_group *) lhs;
        column_doc_group *b = (column_doc_group *) rhs;
        char **a_column_name = string_dict_extract(dic, &a->key, 1);
        char **b_column_name = string_dict_extract(dic, &b->key, 1);
        bool column_name_leq = strcmp(*a_column_name, *b_column_name) <= 0;
        string_dict_free(dic, a_column_name);
        string_dict_free(dic, b_column_name);
        return column_name_leq;
}

static bool compare_object_array_key_column_less_eq_func(const void *lhs, const void *rhs, void *args)
{
        string_dict *dic = (string_dict *) args;
        column_doc_column *a = (column_doc_column *) lhs;
        column_doc_column *b = (column_doc_column *) rhs;
        char **a_column_name = string_dict_extract(dic, &a->key_name, 1);
        char **b_column_name = string_dict_extract(dic, &b->key_name, 1);
        int cmpResult = strcmp(*a_column_name, *b_column_name);
        bool column_name_leq = cmpResult < 0 ? true : (cmpResult == 0 ? (a->type <= b->type) : false);
        string_dict_free(dic, a_column_name);
        string_dict_free(dic, b_column_name);
        return column_name_leq;
}

struct com_column_leq_arg {
        string_dict *dic;
        archive_field_e value_type;
};

#define ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, type, valueVectorAPtr, valueVectorBPtr)                                 \
{                                                                                                                      \
    for (size_t i = 0; i < max_num_elem; i++) {                                                                        \
        type o1 = *VEC_GET(valueVectorAPtr, i, type);                                                        \
        type o2 = *VEC_GET(valueVectorBPtr, i, type);                                                        \
        if (o1 > o2) {                                                                                                 \
            return false;                                                                                              \
        }                                                                                                              \
    }                                                                                                                  \
    return true;                                                                                                       \
}

static bool compare_column_less_eq_func(const void *lhs, const void *rhs, void *args)
{
        vec ofType(<T>) *a = (vec *) lhs;
        vec ofType(<T>) *b = (vec *) rhs;
        struct com_column_leq_arg *func_arg = (struct com_column_leq_arg *) args;

        size_t max_num_elem = JAK_MIN(a->num_elems, b->num_elems);

        switch (func_arg->value_type) {
                case ARCHIVE_FIELD_NULL:
                        return (a->num_elems <= b->num_elems);
                        break;
                case ARCHIVE_FIELD_BOOLEAN: ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, archive_field_boolean_t, a, b);
                        break;
                case ARCHIVE_FIELD_INT8: ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, archive_field_i8_t, a, b);
                        break;
                case ARCHIVE_FIELD_INT16: ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, archive_field_i16_t, a, b);
                        break;
                case ARCHIVE_FIELD_INT32: ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, archive_field_i32_t, a, b);
                        break;
                case ARCHIVE_FIELD_INT64: ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, archive_field_i64_t, a, b);
                        break;
                case ARCHIVE_FIELD_UINT8: ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, archive_field_u8_t, a, b);
                        break;
                case ARCHIVE_FIELD_UINT16: ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, archive_field_u16_t, a, b);
                        break;
                case ARCHIVE_FIELD_UINT32: ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, archive_field_u32_t, a, b);
                        break;
                case ARCHIVE_FIELD_UINT64: ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, archive_field_u64_t, a, b);
                        break;
                case ARCHIVE_FIELD_FLOAT: ARRAY_LEQ_PRIMITIVE_FUNC(max_num_elem, archive_field_number_t, a, b);
                        break;
                case ARCHIVE_FIELD_STRING:
                        for (size_t i = 0; i < max_num_elem; i++) {
                                archive_field_sid_t o1 = *VEC_GET(a, i, archive_field_sid_t);
                                archive_field_sid_t o2 = *VEC_GET(b, i, archive_field_sid_t);
                                char **o1_string = string_dict_extract(func_arg->dic, &o1, 1);
                                char **o2_string = string_dict_extract(func_arg->dic, &o2, 1);
                                bool greater = strcmp(*o1_string, *o2_string) > 0;
                                string_dict_free(func_arg->dic, o1_string);
                                string_dict_free(func_arg->dic, o2_string);
                                if (greater) {
                                        return false;
                                }
                        }
                        return true;
                case ARCHIVE_FIELD_OBJECT:
                        return true;
                        break;
                default:
                        return ERROR(ERR_NOTYPE, NULL);
        }
}

static void sort_columndoc_column(column_doc_column *column, string_dict *dic)
{
        /** Sort column by its value, and re-arrange the array position list according this new order */
        vec ofType(u32) array_position_cpy;
        vec ofType(vec ofType(<T>)) values_cpy;

        vec_cpy(&array_position_cpy, &column->array_positions);
        vec_cpy(&values_cpy, &column->values);

        assert(column->array_positions.num_elems == column->values.num_elems);
        assert(array_position_cpy.num_elems == values_cpy.num_elems);
        assert(values_cpy.num_elems == column->array_positions.num_elems);

        size_t *indices = MALLOC(values_cpy.num_elems * sizeof(size_t));
        for (size_t i = 0; i < values_cpy.num_elems; i++) {
                indices[i] = i;
        }

        struct com_column_leq_arg func_arg = {.dic = dic, .value_type = column->type};

        sort_qsort_indicies_wargs(indices,
                                  values_cpy.base,
                                  values_cpy.elem_size,
                                  compare_column_less_eq_func,
                                  values_cpy.num_elems,
                                  &func_arg);

        for (size_t i = 0; i < values_cpy.num_elems; i++) {
                vec_set(&column->values, i, vec_at(&values_cpy, indices[i]));
                vec_set(&column->array_positions, i, vec_at(&array_position_cpy, indices[i]));
        }

        free(indices);
        vec_drop(&array_position_cpy);
        vec_drop(&values_cpy);
}

static void sort_columndoc_column_arrays(column_doc_obj *columndoc)
{
        vec ofType(column_doc_group) cpy;
        vec_cpy(&cpy, &columndoc->obj_array_props);
        size_t *indices = MALLOC(cpy.num_elems * sizeof(size_t));
        for (size_t i = 0; i < cpy.num_elems; i++) {
                indices[i] = i;
        }
        sort_qsort_indicies_wargs(indices,
                                  cpy.base,
                                  sizeof(column_doc_group),
                                  compare_object_array_key_columns_less_eq_func,
                                  cpy.num_elems,
                                  columndoc->parent->dic);
        for (size_t i = 0; i < cpy.num_elems; i++) {
                vec_set(&columndoc->obj_array_props, i, VEC_GET(&cpy, indices[i], column_doc_group));
        }
        free(indices);

        for (size_t i = 0; i < cpy.num_elems; i++) {
                column_doc_group *key_columns = VEC_GET(&columndoc->obj_array_props, i,
                                                                   column_doc_group);
                size_t *columnIndices = MALLOC(key_columns->columns.num_elems * sizeof(size_t));
                vec ofType(column_doc_column) columnCpy;
                vec_cpy(&columnCpy, &key_columns->columns);
                for (size_t i = 0; i < key_columns->columns.num_elems; i++) {
                        columnIndices[i] = i;
                }

                /** First, sort by column name; Then, sort by columns with same name by type */
                sort_qsort_indicies_wargs(columnIndices,
                                          columnCpy.base,
                                          sizeof(column_doc_column),
                                          compare_object_array_key_column_less_eq_func,
                                          key_columns->columns.num_elems,
                                          columndoc->parent->dic);
                for (size_t i = 0; i < key_columns->columns.num_elems; i++) {
                        vec_set(&key_columns->columns,
                                i,
                                VEC_GET(&columnCpy, columnIndices[i], column_doc_column));
                        column_doc_column *column = VEC_GET(&key_columns->columns, i,
                                                                       column_doc_column);
                        sort_columndoc_column(column, columndoc->parent->dic);
                }

                vec_drop(&columnCpy);
                free(columnIndices);
        }
        vec_drop(&cpy);
}

static void sort_columndoc_values(column_doc_obj *columndoc)
{
        if (columndoc->parent->read_optimized) {
                SORT_META_MODEL_VALUES(columndoc->bool_prop_keys,
                                       columndoc->bool_prop_vals,
                                       archive_field_boolean_t,
                                       compare_field_boolean_t_leq);
                SORT_META_MODEL_VALUES(columndoc->int8_prop_keys,
                                       columndoc->int8_prop_vals,
                                       archive_field_i8_t,
                                       compare_field_i8_t_leq);
                SORT_META_MODEL_VALUES(columndoc->int16_prop_keys,
                                       columndoc->int16_prop_vals,
                                       archive_field_i16_t,
                                       compare_field_i16_t_leq);
                SORT_META_MODEL_VALUES(columndoc->int32_prop_keys,
                                       columndoc->int32_prop_vals,
                                       archive_field_i32_t,
                                       compare_field_i32_t_leq);
                SORT_META_MODEL_VALUES(columndoc->int64_prop_keys,
                                       columndoc->int64_prop_vals,
                                       archive_field_i64_t,
                                       compare_field_i64_t_leq);
                SORT_META_MODEL_VALUES(columndoc->uint8_prop_keys,
                                       columndoc->uint8_prop_vals,
                                       archive_field_u8_t,
                                       compare_field_u8_t_leq);
                SORT_META_MODEL_VALUES(columndoc->uint16_prop_keys,
                                       columndoc->uint16_prop_vals,
                                       archive_field_u16_t,
                                       compare_field_u16_t_leq);
                SORT_META_MODEL_VALUES(columndoc->uin32_prop_keys,
                                       columndoc->uint32_prop_vals,
                                       archive_field_u32_t,
                                       compare_field_u32_t_leq);
                SORT_META_MODEL_VALUES(columndoc->uint64_prop_keys,
                                       columndoc->uint64_prop_vals,
                                       archive_field_u64_t,
                                       compare_field_u64_t_leq);
                SORT_META_MODEL_VALUES(columndoc->float_prop_keys,
                                       columndoc->float_prop_vals,
                                       archive_field_number_t,
                                       compare_field_number_t_leq);
                sort_meta_model_string_values(&columndoc->string_prop_keys,
                                              &columndoc->string_prop_vals,
                                              columndoc->parent->dic);

                SORT_META_MODEL_ARRAYS(columndoc->bool_array_prop_keys,
                                       columndoc->bool_array_prop_vals,
                                       compare_field_boolean_t_array_leq);
                SORT_META_MODEL_ARRAYS(columndoc->int8_array_prop_keys,
                                       columndoc->int8_array_prop_vals,
                                       compare_field_i8_t_array_leq);
                SORT_META_MODEL_ARRAYS(columndoc->int16_array_prop_keys,
                                       columndoc->int16_array_prop_vals,
                                       compare_field_i16_t_array_leq);
                SORT_META_MODEL_ARRAYS(columndoc->int32_array_prop_keys,
                                       columndoc->int32_array_prop_vals,
                                       compare_field_i32_t_array_leq);
                SORT_META_MODEL_ARRAYS(columndoc->int64_array_prop_keys,
                                       columndoc->int64_array_prop_vals,
                                       compare_field_i64_t_array_leq);
                SORT_META_MODEL_ARRAYS(columndoc->uint8_array_prop_keys,
                                       columndoc->uint8_array_prop_vals,
                                       compare_field_u8_t_array_leq);
                SORT_META_MODEL_ARRAYS(columndoc->uint16_array_prop_keys,
                                       columndoc->uint16_array_prop_vals,
                                       compare_field_u16_t_array_leq);
                SORT_META_MODEL_ARRAYS(columndoc->uint32_array_prop_keys,
                                       columndoc->uint32_array_prop_vals,
                                       compare_field_u32_t_array_leq);
                SORT_META_MODEL_ARRAYS(columndoc->uint64_array_prop_keys,
                                       columndoc->ui64_array_prop_vals,
                                       compare_field_u64_t_array_leq);
                SORT_META_MODEL_ARRAYS(columndoc->float_array_prop_keys,
                                       columndoc->float_array_prop_vals,
                                       compare_field_number_t_array_leq);
                sort_columndoc_strings_arrays(&columndoc->string_array_prop_keys,
                                              &columndoc->string_array_prop_vals,
                                              columndoc->parent->dic);

                sort_columndoc_column_arrays(columndoc);
        }
}

static void sort_columndoc_entries(column_doc_obj *columndoc)
{
        if (columndoc->parent->read_optimized) {
                sort_columndoc_values(columndoc);
                sort_nested_primitive_object(columndoc);
                sorted_nested_array_objects(columndoc);
        }
}

column_doc *doc_entries_columndoc(const doc_bulk *bulk, const doc_entries *partition,
                                             bool read_optimized)
{
        if (!bulk || !partition) {
                return NULL;
        }

        // Step 1: encode all strings at once in a bulk
        char *const *key_strings = VEC_ALL(&bulk->keys, char *);
        char *const *valueStrings = VEC_ALL(&bulk->values, char *);
        string_dict_insert(bulk->dic, NULL, key_strings, VEC_LENGTH(&bulk->keys), 0);
        string_dict_insert(bulk->dic, NULL, valueStrings, VEC_LENGTH(&bulk->values), 0);

        // Step 2: for each document doc, create a meta doc, and construct a binary compressed document
        const doc *models = VEC_ALL(&bulk->models, doc);
        assert (bulk->models.num_elems == 1);

        const doc *model = models;

        column_doc *columndoc = MALLOC(sizeof(column_doc));
        columndoc->read_optimized = read_optimized;
        if (!columndoc_create(columndoc, model, bulk, partition, bulk->dic)) {
                error_print(stderr);
        }

        if (columndoc->read_optimized) {
                sort_columndoc_entries(&columndoc->columndoc);
        }

        return columndoc;
}

bool doc_entries_drop(doc_entries *partition)
{
        UNUSED(partition);
        return true;
}

static void create_doc(doc_obj *model, doc *doc)
{
        vec_create(&model->entries, sizeof(doc_entries), 50);
        model->doc = doc;
}

static void create_typed_vector(doc_entries *entry)
{
        size_t size;
        switch (entry->type) {
                case ARCHIVE_FIELD_NULL:
                        size = sizeof(field_null_t);
                        break;
                case ARCHIVE_FIELD_BOOLEAN:
                        size = sizeof(archive_field_boolean_t);
                        break;
                case ARCHIVE_FIELD_INT8:
                        size = sizeof(archive_field_i8_t);
                        break;
                case ARCHIVE_FIELD_INT16:
                        size = sizeof(archive_field_i16_t);
                        break;
                case ARCHIVE_FIELD_INT32:
                        size = sizeof(archive_field_i32_t);
                        break;
                case ARCHIVE_FIELD_INT64:
                        size = sizeof(archive_field_i64_t);
                        break;
                case ARCHIVE_FIELD_UINT8:
                        size = sizeof(archive_field_u8_t);
                        break;
                case ARCHIVE_FIELD_UINT16:
                        size = sizeof(archive_field_u16_t);
                        break;
                case ARCHIVE_FIELD_UINT32:
                        size = sizeof(archive_field_u32_t);
                        break;
                case ARCHIVE_FIELD_UINT64:
                        size = sizeof(archive_field_u64_t);
                        break;
                case ARCHIVE_FIELD_FLOAT:
                        size = sizeof(archive_field_number_t);
                        break;
                case ARCHIVE_FIELD_STRING:
                        size = sizeof(field_string_t);
                        break;
                case ARCHIVE_FIELD_OBJECT:
                        size = sizeof(doc_obj);
                        break;
                default: ERROR(ERR_INTERNALERR, "unknown type"); /** unknown type */
                        return;
        }
        vec_create(&entry->values, size, 50);
}

static void entries_drop(doc_entries *entry)
{
        if (entry->type == ARCHIVE_FIELD_OBJECT) {
                for (size_t i = 0; i < entry->values.num_elems; i++) {
                        doc_obj *model = VEC_GET(&entry->values, i, doc_obj);
                        doc_drop(model);
                }
        }
        vec_drop(&entry->values);
}

static bool print_value(FILE *file, archive_field_e type, const vec ofType(<T>) *values)
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
                case ARCHIVE_FIELD_NULL: {
                        for (size_t i = 0; i < num_values; i++) {
                                fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
                        }
                }
                        break;
                case ARCHIVE_FIELD_BOOLEAN: {
                        for (size_t i = 0; i < num_values; i++) {
                                archive_field_boolean_t value = *(VEC_GET(values, i, archive_field_boolean_t));
                                if (value != NULL_BOOLEAN) {
                                        fprintf(file, "%s%s", value == 0 ? "false" : "true",
                                                i + 1 < num_values ? ", " : "");
                                } else {
                                        fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
                                }
                        }
                }
                        break;
                case ARCHIVE_FIELD_INT8: {
                        for (size_t i = 0; i < num_values; i++) {
                                archive_field_i8_t value = *(VEC_GET(values, i, archive_field_i8_t));
                                if (value != NULL_INT8) {
                                        fprintf(file, "%d%s", value, i + 1 < num_values ? ", " : "");
                                } else {
                                        fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
                                }
                        }
                }
                        break;
                case ARCHIVE_FIELD_INT16: {
                        for (size_t i = 0; i < num_values; i++) {
                                archive_field_i16_t value = *(VEC_GET(values, i, archive_field_i16_t));
                                if (value != NULL_INT16) {
                                        fprintf(file, "%d%s", value, i + 1 < num_values ? ", " : "");
                                } else {
                                        fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
                                }
                        }
                }
                        break;
                case ARCHIVE_FIELD_INT32: {
                        for (size_t i = 0; i < num_values; i++) {
                                archive_field_i32_t value = *(VEC_GET(values, i, archive_field_i32_t));
                                if (value != NULL_INT32) {
                                        fprintf(file, "%d%s", value, i + 1 < num_values ? ", " : "");
                                } else {
                                        fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
                                }
                        }
                }
                        break;
                case ARCHIVE_FIELD_INT64: {
                        for (size_t i = 0; i < num_values; i++) {
                                archive_field_i64_t value = *(VEC_GET(values, i, archive_field_i64_t));
                                if (value != NULL_INT64) {
                                        fprintf(file, "%" PRIi64 "%s", value, i + 1 < num_values ? ", " : "");
                                } else {
                                        fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
                                }
                        }
                }
                        break;
                case ARCHIVE_FIELD_UINT8: {
                        for (size_t i = 0; i < num_values; i++) {
                                archive_field_u8_t value = *(VEC_GET(values, i, archive_field_u8_t));
                                if (value != NULL_UINT8) {
                                        fprintf(file, "%d%s", value, i + 1 < num_values ? ", " : "");
                                } else {
                                        fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
                                }
                        }
                }
                        break;
                case ARCHIVE_FIELD_UINT16: {
                        for (size_t i = 0; i < num_values; i++) {
                                archive_field_u16_t value = *(VEC_GET(values, i, archive_field_u16_t));
                                if (value != NULL_UINT16) {
                                        fprintf(file, "%d%s", value, i + 1 < num_values ? ", " : "");
                                } else {
                                        fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
                                }
                        }
                }
                        break;
                case ARCHIVE_FIELD_UINT32: {
                        for (size_t i = 0; i < num_values; i++) {
                                archive_field_u32_t value = *(VEC_GET(values, i, archive_field_u32_t));
                                if (value != NULL_UINT32) {
                                        fprintf(file, "%d%s", value, i + 1 < num_values ? ", " : "");
                                } else {
                                        fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
                                }
                        }
                }
                        break;
                case ARCHIVE_FIELD_UINT64: {
                        for (size_t i = 0; i < num_values; i++) {
                                archive_field_u64_t value = *(VEC_GET(values, i, archive_field_u64_t));
                                if (value != NULL_UINT64) {
                                        fprintf(file, "%" PRIu64 "%s", value, i + 1 < num_values ? ", " : "");
                                } else {
                                        fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
                                }
                        }
                }
                        break;
                case ARCHIVE_FIELD_FLOAT: {
                        for (size_t i = 0; i < num_values; i++) {
                                archive_field_number_t value = *(VEC_GET(values, i, archive_field_number_t));
                                if (!isnan(value)) {
                                        fprintf(file, "%f%s", value, i + 1 < num_values ? ", " : "");
                                } else {
                                        fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
                                }
                        }
                }
                        break;
                case ARCHIVE_FIELD_STRING: {
                        for (size_t i = 0; i < num_values; i++) {
                                field_string_t value = *(VEC_GET(values, i, field_string_t));
                                if (value) {
                                        fprintf(file, "\"%s\"%s", value, i + 1 < num_values ? ", " : "");
                                } else {
                                        fprintf(file, "null%s", i + 1 < num_values ? ", " : "");
                                }
                        }
                }
                        break;
                case ARCHIVE_FIELD_OBJECT: {
                        for (size_t i = 0; i < num_values; i++) {
                                doc_obj *obj = VEC_GET(values, i, doc_obj);
                                if (!NULL_OBJECT_MODEL(obj)) {
                                        _doc_print_object(file, obj);
                                } else {
                                        fprintf(file, "null");
                                }
                                fprintf(file, "%s", i + 1 < num_values ? ", " : "");
                        }
                }
                        break;
                default: ERROR(ERR_NOTIMPLEMENTED, NULL);
        }
        if (num_values > 1) {
                fprintf(file, "]");
        }
        return true;
}

static void _doc_print_object(FILE *file, const doc_obj *model)
{
        fprintf(file, "{");
        for (size_t i = 0; i < model->entries.num_elems; i++) {
                doc_entries *entry = VEC_GET(&model->entries, i, doc_entries);
                fprintf(file, "\"%s\": ", entry->key);
                print_value(file, entry->type, &entry->values);
                fprintf(file, "%s", i + 1 < model->entries.num_elems ? ", " : "");
        }
        fprintf(file, "}");
}

