/*
 * Copyright 2019 Marcus Pinnecke, Jasper Orschulko
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

#ifndef KEYWORDS_H
#define KEYWORDS_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------
#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/fn_result.h>

BEGIN_DECL


static inline fn_result schema_generate_handleKeyword_properties(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;

    carbon_object_it_prop_type(&field_type, oit);
    if (!(carbon_field_type_is_object_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"properties\" expects an object");
    }

    vector_create(&(s->data.properties), NULL, sizeof(schema), 5);
    carbon_object_it *soit = carbon_object_it_object_value(oit);

    while (carbon_object_it_next(soit)) {
        carbon_object_it_prop_type(&field_type, soit);
        if (!(carbon_field_type_is_object_or_subtype(field_type))) {
            carbon_object_it_drop(soit);
            vector_drop(&(s->data.properties));
            return FN_FAIL(ERR_BADTYPE, "keyword \"properties\" expects nested objects");
        }

        u64 keylen;
        carbon_object_it *ssoit = carbon_object_it_object_value(soit);
        const char *_key = carbon_object_it_prop_name(&keylen, soit);
        const char *key = (const char*) malloc(sizeof(char) * (keylen + 1));
        key = strndup(_key, keylen);

        schema *item = (schema*) malloc(sizeof(schema));
        if (!(FN_IS_OK(schema_init(item, key)))) {
            carbon_object_it_drop(ssoit);
            carbon_object_it_drop(soit);
            free((void*) key);
            free(item);
            return FN_FAIL_FORWARD();
        }
        if (!(FN_IS_OK(schema_generate(item, ssoit)))) {
            carbon_object_it_drop(ssoit);
            carbon_object_it_drop(soit);
            free((void*) key);
            free(item);
            return FN_FAIL_FORWARD();
        }
        carbon_object_it_drop(ssoit);
        vector_push(&(s->data.properties), item, 1);
    }
    carbon_object_it_drop(soit);

    s->applies.has_properties = true;

    return FN_OK();
}


// TODO: allow for type to be a string
static inline fn_result schema_generate_handleKeyword_type(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    vector_create(&(s->data.type), NULL, sizeof(int), 8);

    carbon_object_it_prop_type(&field_type, oit);
    if (!(carbon_field_type_is_array_or_subtype(field_type))) {
        vector_drop(&(s->data.type));
        return FN_FAIL(ERR_BADTYPE, "keyword \"type\" expects an array");
    }

    carbon_array_it *ait = carbon_object_it_array_value(oit);

    while (carbon_array_it_next(ait)) {
        carbon_array_it_field_type(&field_type, ait);
        if (!(carbon_field_type_is_string(field_type))) {
            vector_drop(&(s->data.type));
            carbon_array_it_drop(ait);
            return FN_FAIL(ERR_BADTYPE, "keyword \"type\" expects an array of strings");
        }
        u64 strlen;
        const char *_str = carbon_array_it_string_value(&strlen, ait);
        const char *str = strndup(_str, strlen);
        if (strcmp(str, "number") == 0) {
            type = NUMBER;
        }
        else if (strcmp(str, "string") == 0) {
            type = STRING;
        }
        else if (strcmp(str, "boolean") == 0) {
            type = BOOLEAN;
        }
        else if (strcmp(str, "binary") == 0) {
            type = BINARY;
        }
        else if (strcmp(str, "array") == 0) {
            type = ARRAY;
        }
        else if (strcmp(str, "column") == 0) {
            type = COLUMN;
        }
        else if (strcmp(str, "object") == 0) {
            type = OBJECT;
        }
        else if (strcmp(str, "null") == 0) {
            type = _NULL;
        }
        else {
            vector_drop(&(s->data.type));
            carbon_array_it_drop(ait);
            return FN_FAIL(ERR_UNSUPPORTEDTYPE, "\"type\" keyword contains unsupported constraint");
        }
        vector_push(&(s->data.type), &type, 1);
    }
    carbon_array_it_drop(ait);

    s->applies.has_type = true;

    return FN_OK();
}

static inline fn_result schema_generate_handleKeyword_minimum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    bool isnull;
    long double val;

    if (!(FN_IS_OK(longDoubleFromOit(&isnull, &val, oit)))) {
        return FN_FAIL_FORWARD();
    }
    if(isnull) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"minimum\" expects a numeric value. Got NULL");
    }
    s->applies.has_minimum = true;
    s->data.minimum = val;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_maximum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    bool isnull;
    long double val;

    if (!(FN_IS_OK(longDoubleFromOit(&isnull, &val, oit)))) {
        return FN_FAIL_FORWARD();
    }
    if (isnull) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"maximum\" expects a numeric value. Got NULL");
    }
    s->applies.has_maximum = true;
    s->data.maximum = val;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_exclusiveMinimum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    bool isnull;
    long double val;

    if (!(FN_IS_OK(longDoubleFromOit(&isnull, &val, oit)))) {
        return FN_FAIL_FORWARD();
    }
    if (isnull) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"exclusiveMinimum\" expects a numeric value. Got NULL");
    }
    s->applies.has_exclusiveMinimum = true;
    s->data.exclusiveMinimum = val;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_exclusiveMaximum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    bool isnull;
    long double val;

    if (!(FN_IS_OK(longDoubleFromOit(&isnull, &val, oit)))) {
        return FN_FAIL_FORWARD();
    }
    if (isnull) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"exclusiveMaximum\" expects a numeric value. Got NULL");
    }
    s->applies.has_exclusiveMaximum = true;
    s->data.exclusiveMaximum = val;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_multipleOf(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    bool isnull;
    long double val;

    if (!(FN_IS_OK(longDoubleFromOit(&isnull, &val, oit)))) {
        return FN_FAIL_FORWARD();
    }
    if (isnull) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"multipleOf\" expects a numeric value. Got NULL");
    }
    s->applies.has_multipleOf = true;
    s->data.multipleOf = val;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_minLength(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    bool isnull;
    u64 val;
    if (!(carbon_field_type_is_unsigned(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"minLength\" expects a positive numeric value");
    }
    carbon_object_it_unsigned_value(&isnull, &val, oit);
    if (isnull) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"minLength\" expects a positive numeric value. Got NULL");
    }
    s->applies.has_minLength = true;
    s->data.minLength = val;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_maxLength(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    bool isnull;
    u64 val;
    if (!(carbon_field_type_is_number(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"maxLength\" expects a numeric value");
    }
    carbon_object_it_unsigned_value(&isnull, &val, oit);
    if (isnull) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"maxLength\" expects a numeric value. Got NULL");
    }
    s->applies.has_maxLength = true;
    s->data.maxLength = val;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_pattern(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    if (!(carbon_field_type_is_string(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"pattern\" expects a string value");
    }
    u64 strlen;
    const char* _str = carbon_object_it_string_value(&strlen, oit);
    s->applies.has_pattern = true;
    s->data.pattern = strndup(_str, strlen);

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_format(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    if (!(carbon_field_type_is_string(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"format\" expects a string value");
    }
    u64 strlen;
    const char* _str = carbon_object_it_string_value(&strlen, oit);
    const char *str = strndup(_str, strlen);

    if (strcmp(str, "date") == 0) {
        format = DATE;
    }
    else if (strcmp(str, "date-time") == 0) {
        format = DATETIME;
    }
    else if (strcmp(str, "uri") == 0) {
        format = URI;
    }
    else if (strcmp(str, "email") == 0) {
        format = EMAIL;
    }
    else if (strcmp(str, "hostname") == 0) {
        format = HOSTNAME;
    }
    else if (strcmp(str, "ipv4") == 0) {
        format = IPV4;
    }
    else if (strcmp(str, "ipv6") == 0) {
        format = IPV6;
    }
    else if (strcmp(str, "regex") == 0) {
        format = REGEX;
    }
    else {
        return FN_FAIL(ERR_SCHEMA_UNDEF_KEYWORD, "keyword \"format\" defines unknown format"); 
    }
    s->applies.has_format = true;
    s->data.format = format;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_formatMinimum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    if (!(carbon_field_type_is_string(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"formatMinimum\" expects a string value");
    }
    u64 strlen;
    const char* _str = carbon_object_it_string_value(&strlen, oit);

    s->applies.has_formatMinimum = true;
    s->data.formatMinimum = strndup(_str, strlen);

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_formatMaximum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    if (!(carbon_field_type_is_string(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"formatMaximum\" expects a string value");
    }
    u64 strlen;
    const char* _str = carbon_object_it_string_value(&strlen, oit);

    s->applies.has_formatMaximum = true;
    s->data.formatMaximum = strndup(_str, strlen);

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_formatExclusiveMinimum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    if (!(carbon_field_type_is_string(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"formatMaximum\" expects a string value");
    }
    u64 strlen;
    const char* _str = carbon_object_it_string_value(&strlen, oit);

    s->applies.has_formatExclusiveMinimum = true;
    s->data.formatExclusiveMinimum = strndup(_str, strlen);

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_formatExclusiveMaximum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    if (!(carbon_field_type_is_string(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"formatExclusiveMaximum\" expects a string value");
    }
    u64 strlen;
    const char* _str = carbon_object_it_string_value(&strlen, oit);

    s->applies.has_formatExclusiveMaximum = true;
    s->data.formatExclusiveMaximum = strndup(_str, strlen);

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_minItems(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    bool isnull;
    u64 val;
    if (!(carbon_field_type_is_unsigned(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"minItems\" expects a positive numeric value");
    }
    carbon_object_it_unsigned_value(&isnull, &val, oit);
    if (isnull) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"minItems\" expects a positive numeric value. Got NULL");
    }

    s->applies.has_minItems = true;
    s->data.minItems = val;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_maxItems(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    bool isnull;
    u64 val;
    if (!(carbon_field_type_is_unsigned(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"maxItems\" expects a positive numeric value");
    }
    carbon_object_it_unsigned_value(&isnull, &val, oit);
    if (isnull) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"maxItems\" expects a postive numeric value. Got NULL");
    }

    s->applies.has_maxItems = true;
    s->data.maxItems = val;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_uniqueItems(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_boolean(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"uniqueItems\" expects a boolean value");
    }

    s->applies.has_uniqueItems = true;
    carbon_object_it_bool_value(&(s->data.uniqueItems), oit);

    return FN_OK();
}

static inline fn_result schema_generate_handleKeyword_items(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    vector_create(&(s->data.items), NULL, sizeof(schema), 5);

    // if keyword only holds one schema object, every element in array has to conform to the schema
    if (carbon_field_type_is_object_or_subtype(field_type)) {
        
        schema *items = VECTOR_NEW_AND_GET(&(s->data.items), schema);
        carbon_object_it *soit = carbon_object_it_object_value(oit);

        s->applies.items_isObject = true;

        if (!(FN_IS_OK(schema_init(items, NULL)))) {
            carbon_object_it_drop(soit);
            free(items);
            vector_drop(&(s->data.items));
            return FN_FAIL_FORWARD();
        }
        if (!(FN_IS_OK(schema_generate(items, soit)))) {
            carbon_object_it_drop(soit);
            free(items);
            vector_drop(&(s->data.items));
            return FN_FAIL_FORWARD();
        }
        carbon_object_it_drop(soit);

        s->applies.has_items = true;

        return FN_OK();
    }

    // if keyword consists of an array of schema, element x in data array has to conform to schema x in schema array
    else if (carbon_field_type_is_array_or_subtype(field_type)) {
        carbon_array_it *ait = carbon_object_it_array_value(oit);
        while (carbon_array_it_next(ait)) {

            carbon_array_it_field_type(&field_type, ait);

            if (!(carbon_field_type_is_object_or_subtype(field_type))) {
                carbon_array_it_drop(ait);
                vector_drop(&(s->data.items));
                return FN_FAIL(ERR_BADTYPE, "keyword \"items\" expects an object or an array of objects");
            }

            schema *items = VECTOR_NEW_AND_GET(&(s->data.items), schema);
            carbon_object_it *soit = carbon_array_it_object_value(ait);

            if (!(FN_IS_OK(schema_init(items, NULL)))) {
                carbon_object_it_drop(soit);
                carbon_array_it_drop(ait);
                schema_drop(items);
                return FN_FAIL_FORWARD();
            }
            if (!(FN_IS_OK(schema_generate(items, soit)))) {
                carbon_object_it_drop(soit);
                carbon_array_it_drop(ait);
                schema_drop(items);
                return FN_FAIL_FORWARD();
            }
            carbon_object_it_drop(soit);
        }
        carbon_array_it_drop(ait);

        s->applies.has_items = true;

        return FN_OK();
    }

    vector_drop(&(s->data.items));
    return FN_FAIL(ERR_BADTYPE, "keyword \"items\" expects an object or an array of objects");
}


static inline fn_result schema_generate_handleKeyword_additionalItems(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (carbon_field_type_is_boolean(field_type)) {
        s->applies.additionalItemsIsBool = true;
        bool is_true;
        carbon_object_it_bool_value(&is_true, oit);
        if (is_true) {
            s->data.additionalItemsBool = true;
        }
        else {
            s->data.additionalItemsBool = false;

        }
        s->applies.has_additionalItems = true;
        return FN_OK();
    }

    if (carbon_field_type_is_object_or_subtype(field_type)) {

        carbon_object_it *soit = carbon_object_it_object_value(oit);
        schema *additionalItems = (schema*) malloc(sizeof(schema));
        s->data.additionalItems = additionalItems;

        if(!(FN_IS_OK(schema_init(s->data.additionalItems, NULL)))) {
            free(additionalItems);
            return FN_FAIL_FORWARD();
        }

        if(!(FN_IS_OK(schema_generate(s->data.additionalItems, soit)))) {
            carbon_object_it_drop(soit);
            schema_drop(s->data.additionalItems);
            return FN_FAIL_FORWARD();
        }

        s->applies.has_additionalItems = true;
        return FN_OK();
    }
    return FN_FAIL(ERR_BADTYPE, "keyword \"additionalItems\" expects a boolean or an object value");
}


static inline fn_result schema_generate_handleKeyword_contains(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_object_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"contains\" expects an object");
    }
    carbon_object_it *soit = carbon_object_it_object_value(oit);
    schema *contains = (schema*) malloc(sizeof(schema));

    if (!(FN_IS_OK(schema_init(contains, NULL)))) {
        carbon_object_it_drop(soit);
        free(contains);
        return FN_FAIL_FORWARD();
    }
    if (!(FN_IS_OK(schema_generate(contains, soit)))) {
        carbon_object_it_drop(soit);
        free(contains);
        return FN_FAIL_FORWARD();
    }
    carbon_object_it_drop(soit);

    s->applies.has_contains = true;
    s->data.contains = contains;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_minProperties(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    bool isnull;
    u64 val;
    if (!(carbon_field_type_is_number(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"minProperties\" expects a numeric value");
    }
    carbon_object_it_unsigned_value(&isnull, &val, oit);
    if (isnull) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"minProperties\" expects a numeric value. Got NULL");
    }

    s->applies.has_minProperties = true;
    s->data.minProperties = val;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_maxProperties(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    bool isnull;
    u64 val;
    if (!(carbon_field_type_is_number(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"maxProperties\" expects a numeric value");
    }
    carbon_object_it_unsigned_value(&isnull, &val, oit);
    if (isnull) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"maxProperties\" expects a numeric value. Got NULL");
    }

    s->applies.has_maxProperties = true;
    s->data.maxProperties = val;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_required(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_array_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"required\" expects an array of strings");
    }

    vector_create(&(s->data.required), NULL, sizeof(char*), 5);
    carbon_array_it *ait = carbon_object_it_array_value(oit);

    while (carbon_array_it_next(ait)) {
        carbon_array_it_field_type(&field_type, ait);
        if (!(carbon_field_type_is_string(field_type))) {
            vector_drop(&(s->data.required));
            carbon_array_it_drop(ait);
            return FN_FAIL(ERR_BADTYPE, "keyword \"required\" expects an array of strings");
        }
        u64 strlen;
        const char *_required = carbon_array_it_string_value(&strlen, ait); 
        const char *required = (const char*) malloc(sizeof(char) * (strlen + 1));
        required = strndup(_required, strlen);
        vector_push(&(s->data.required), required, 1);
    }
    carbon_array_it_drop(ait);

    s->applies.has_required = true;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_patternProperties(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_object_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"patternProperties\" expects an object");
    }

    carbon_object_it *soit = carbon_object_it_object_value(oit);
    vector_create(&(s->data.patternProperties), NULL, sizeof(schema*), 5);

    while(carbon_object_it_next(soit)) {
        carbon_object_it_prop_type(&field_type, soit);

        if(!(carbon_field_type_is_object_or_subtype(field_type))) {
            vector_drop(&(s->data.patternProperties));
            carbon_object_it_drop(soit);
            return FN_FAIL(ERR_BADTYPE, "keyword \"patternProperties\" expects each property to be an object");
        }

        schema *patternProperty = (schema*) malloc(sizeof(schema));
        char *key = (char*) malloc(sizeof(char) * (oit->field.key.name_len + 1));
        key = strndup(oit->field.key.name, oit->field.key.name_len);

        if (!(FN_IS_OK(schema_init(patternProperty, key)))) {
            carbon_object_it_drop(soit);
            vector_drop(&(s->data.patternProperties));
            free(patternProperty);
            return FN_FAIL_FORWARD();
        }

        carbon_object_it *ssoit = carbon_object_it_object_value(soit);

        if (!(FN_IS_OK(schema_generate(patternProperty, ssoit)))) {
            carbon_object_it_drop(ssoit);      
            carbon_object_it_drop(soit);
            vector_drop(&(s->data.patternProperties));
            free(patternProperty);
            return FN_FAIL_FORWARD();
        }
        vector_push(&(s->data.patternProperties), patternProperty, 1);
        carbon_object_it_drop(ssoit);      
    }
    carbon_object_it_drop(soit);

    s->applies.has_patternProperties = true;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_additionalProperties(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_boolean(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"additionalProperties\" expects a boolean value");
    }

    s->applies.has_additionalProperties = true;
    carbon_object_it_bool_value(&(s->data.additionalProperties), oit);

    return FN_OK();
}

static inline fn_result schema_generate_handleKeyword_dependencies(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_object_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"dependencies\"expects an object");
    }

    carbon_object_it *soit = carbon_object_it_object_value(oit);
    vector_create(&(s->data.dependencies), NULL, sizeof(struct vector), 5);
    bool isArray = false;

    while (carbon_object_it_next(soit)) {
        carbon_object_it_prop_type(&field_type, soit);

        if (carbon_field_type_is_array_or_subtype(field_type)) {
            isArray = true;
            if(s->applies.dependencies_isObject) {
                vector_drop(&(s->data.dependencies));
                carbon_object_it_drop(soit);
                return FN_FAIL(ERR_BADTYPE, "keyword \"dependencies\" failed: mixed schemas and arrays");
            }

            u64 keylen;
            const char *_key = carbon_object_it_prop_name(&keylen, soit);
            const char *key = (const char*) malloc(sizeof(const char) * (keylen + 1));
            key = strndup(_key, keylen);

            vector *item = VECTOR_NEW_AND_GET(&(s->data.dependencies), vector);
            vector_create(item, NULL, sizeof(const char*), 5);
            vector_push(item, key, 1);

            carbon_array_it *ait = carbon_object_it_array_value(soit); 
            while (carbon_array_it_next(ait)) {
                carbon_array_it_field_type(&field_type, ait);
                if (!(carbon_field_type_is_string(field_type))) {
                    carbon_array_it_drop(ait);
                    vector_drop(item);
                    vector_drop(&s->data.dependencies);
                    carbon_object_it_drop(soit);
                    return FN_FAIL(ERR_BADTYPE, "keyword \"dependencies\" failed: array elem not of type string");
                }

                u64 strlen;
                const char *_str = carbon_array_it_string_value(&strlen, ait);
                const char *str = (const char*) malloc(sizeof(const char) * strlen + 1);
                str = strndup(_str, strlen);
                vector_push(item, str, 1);
            }
            carbon_array_it_drop(ait);
        }

        else if (carbon_field_type_is_object_or_subtype(field_type)) {
            if (isArray) {
                // FIXME: might be a memory leak. does drop free memory recursive?
                vector_drop(&(s->data.dependencies));
                carbon_object_it_drop(soit);
                return FN_FAIL(ERR_BADTYPE, "keyword \"dependencies\" failed: mixed schemas and arrays");
            }
            s->applies.dependencies_isObject = true;

            schema *dependency = (schema*) malloc(sizeof(schema));
            u64 keylen;
            const char *_key = carbon_object_it_prop_name(&keylen, oit);
            const char *key = (const char*) malloc(sizeof(const char) * keylen + 1);
            key = strndup(_key, keylen);

            if (!(FN_IS_OK(schema_init(dependency, key)))) {
                carbon_object_it_drop(soit);
                free(dependency);
                vector_drop(&(s->data.dependencies));
                return FN_FAIL_FORWARD();
            }

            carbon_object_it *ssoit = carbon_object_it_object_value(soit);

            if (!(FN_IS_OK(schema_generate(dependency, ssoit)))) {
                carbon_object_it_drop(ssoit);      
                carbon_object_it_drop(soit);
                free(dependency);
                vector_drop(&(s->data.dependencies));
                return FN_FAIL_FORWARD();
            }
            vector_push(&(s->data.dependencies), dependency, 1);
            carbon_object_it_drop(ssoit);      
        }
        else {
            vector_drop(&(s->data.dependencies));
            carbon_object_it_drop(soit);
            vector_drop(&(s->data.dependencies));
            return FN_FAIL(ERR_BADTYPE, "keyword \"dependencies\" expects a nested schemas or nested string arrays");
        }
    }
    carbon_object_it_drop(soit);

    s->applies.has_dependencies = true;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_propertyNames(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_object_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"propertyNames\" expects each dependency to be an object");
    }

    schema *propertyNames = (schema*) malloc(sizeof(schema)); 
    carbon_object_it *soit = carbon_object_it_object_value(oit);

    if (!(FN_IS_OK(schema_init(propertyNames, NULL)))) {
        carbon_object_it_drop(soit);
        free(propertyNames);
        return FN_FAIL_FORWARD();
    }
    if (!(FN_IS_OK(schema_generate(propertyNames, soit)))) {
        carbon_object_it_drop(soit);
        free(propertyNames);
        return FN_FAIL_FORWARD();
    }
    carbon_object_it_drop(soit);

    s->applies.has_propertyNames = true;
    s->data.propertyNames = propertyNames;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_patternRequired(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_array_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"patternRequired\" expects an array of strings");
    }

    carbon_array_it *ait = carbon_object_it_array_value(oit);
    vector_create(&(s->data.patternRequired), NULL, sizeof(char*), 5);

    while (carbon_array_it_next(ait)) {
        carbon_array_it_field_type(&field_type, ait);

        if (!(carbon_field_type_is_string(field_type))) {
            carbon_array_it_drop(ait);
            vector_drop(&(s->data.patternRequired));
            return FN_FAIL(ERR_BADTYPE, "keyword \"patternRequired\" expects an array of strings");
        }
        u64 strlen;
        const char *_pattern = carbon_array_it_string_value(&strlen, ait);
        char *pattern = (char*) malloc(sizeof(char) *(strlen + 1));
        pattern = strndup(_pattern, strlen);

        vector_push(&(s->data.patternRequired), pattern, 1);
    }
    carbon_array_it_drop(ait);

    s->applies.has_patternRequired = true;

    return FN_OK();
}

// TODO: implement - a bit tricky. carbon from array func would be useful!
static inline fn_result schema_generate_handleKeyword_enum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    //carbon_field_type_e field_type;
    //carbon_object_it_prop_type(&field_type, oit);

    //if (!(carbon_field_type_is_array_or_subtype(field_type))) {
    //    return FN_FAIL(ERR_BADTYPE, "keyword \"enum\" expects an array");
    //}

    //carbon_array_it *ait = carbon_object_it_array_value(oit);

    //carbon_new context;
    //carbon_insert *ins, *nested_ins;

    //ins = carbon_create_begin(&context, &(s->data._enum), CARBON_KEY_NOKEY, CARBON_KEEP);
    //while (carbon_object_it_next(oit)) {
    //    carbon_object_it_prop_




    //vector_create(&(s->data._enum), NULL, sizeof(schema_tuple*), 5);
    //schema_tuple *tuple = (schema_tuple*) malloc(sizeof(schema_tuple));
    //carbon_array_it *ait = carbon_object_it_array_value(oit);

    //while (carbon_array_it_next(ait)) {
    //    carbon_array_it_field_type(&field_type, ait);

    //    if (carbon_field_type_is_number(field_type)) {
    //        bool isnull;
    //        long double *val = (long double*) malloc(sizeof(long double));
    //        longDoubleFromAit (&isnull, val, ait);
    //       
    //        tuple->type = NUMBER;
    //        tuple->value = val;
    //    }

    //    else if (carbon_field_type_is_string(field_type) || carbon_field_type_is_binary(field_type)) {
    //        u64 strlen;
    //        const char *_str = carbon_array_it_string_value(&strlen, ait);
    //        char *str = (char*) malloc(sizeof(char) *(strlen + 1));
    //        str = strndup(_str, strlen);
    //        if (carbon_field_type_is_string(field_type)) {
    //            tuple->type = STRING;
    //        }
    //        else {
    //            tuple->type = BINARY;
    //        }
    //        tuple->value = str;
    //    }

    //    else if (carbon_field_type_is_boolean(field_type)) {
    //        bool *val = (bool*) malloc(sizeof(bool));
    //        carbon_array_it_bool_value(val, ait);
    //        tuple->type = BOOLEAN;
    //        tuple->value = val;
    //    }

    //// ...

    //        

    //}


    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}

// TODO: implement - a bit tricky!
static inline fn_result schema_generate_handleKeyword_const(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


static inline fn_result schema_generate_handleKeyword_not(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_object_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"not\" expects an object");
    }

    carbon_object_it *soit = carbon_object_it_object_value(oit); 
    schema *_not = (schema*) malloc(sizeof(schema));

    if (!(FN_IS_OK(schema_init(_not, NULL)))) {
        carbon_object_it_drop(soit);
        free(_not);
        return FN_FAIL_FORWARD();
    }
    if (!(FN_IS_OK(schema_generate(_not, soit)))) {
        carbon_object_it_drop(soit);
        free(_not);
        return FN_FAIL_FORWARD();
    }
    carbon_object_it_drop(soit);

    s->applies.has_not = true;
    s->data._not = _not;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_oneOf(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_array_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"oneOf\" expects an array of objects");
    }

    carbon_array_it *ait = carbon_object_it_array_value(oit);
    vector_create(&(s->data.oneOf), NULL, sizeof(schema*), 5);

    while (carbon_array_it_next(ait)) {

        carbon_array_it_field_type(&field_type, ait);

        if (!(carbon_field_type_is_object_or_subtype(field_type))) {
            vector_drop(&(s->data.oneOf));
            carbon_array_it_drop(ait);
            return FN_FAIL(ERR_BADTYPE, "keyword \"oneOf\" expects an array of objects");
        }

        carbon_object_it *soit = carbon_array_it_object_value(ait);
        schema *oneOf = (schema*) malloc(sizeof(schema));

        if (!(FN_IS_OK(schema_init(oneOf , NULL)))) {
            carbon_object_it_drop(soit);
            carbon_array_it_drop(ait);
            free(oneOf);
            return FN_FAIL_FORWARD();
        }
        if (!(FN_IS_OK(schema_generate(oneOf, soit)))) {
            carbon_object_it_drop(soit);
            carbon_array_it_drop(ait);
            free(oneOf);
            return FN_FAIL_FORWARD();
        }
        carbon_object_it_drop(soit);
        vector_push(&(s->data.oneOf), oneOf, 1);
    }
    carbon_array_it_drop(ait);

    s->applies.has_oneOf = true;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_anyOf(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_array_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"anyOf\" expects an array of objects");
    }

    carbon_array_it *ait = carbon_object_it_array_value(oit);
    vector_create(&(s->data.anyOf), NULL, sizeof(schema*), 5);

    while (carbon_array_it_next(ait)) {

        carbon_array_it_field_type(&field_type, ait);

        if (!(carbon_field_type_is_object_or_subtype(field_type))) {
            vector_drop(&(s->data.anyOf));
            carbon_array_it_drop(ait);
            return FN_FAIL(ERR_BADTYPE, "keyword \"anyOf\" expects an array of objects");
        }

        carbon_object_it *soit = carbon_array_it_object_value(ait);
        schema *anyOf = (schema*) malloc(sizeof(schema));

        if (!(FN_IS_OK(schema_init(anyOf , NULL)))) {
            carbon_object_it_drop(soit);
            carbon_array_it_drop(ait);
            free(anyOf);
            return FN_FAIL_FORWARD();
        }
        if (!(FN_IS_OK(schema_generate(anyOf, soit)))) {
            carbon_object_it_drop(soit);
            carbon_array_it_drop(ait);
            free(anyOf);
            return FN_FAIL_FORWARD();
        }
        carbon_object_it_drop(soit);
        vector_push(&(s->data.anyOf), anyOf, 1);
    }
    carbon_array_it_drop(ait);

    s->applies.has_anyOf = true;

    return FN_OK();
}


static inline fn_result schema_generate_handleKeyword_allOf(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_array_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"allOf\" expects an array of objects");
    }

    carbon_array_it *ait = carbon_object_it_array_value(oit);
    vector_create(&(s->data.allOf), NULL, sizeof(schema*), 5);

    while (carbon_array_it_next(ait)) {

        carbon_array_it_field_type(&field_type, ait);

        if (!(carbon_field_type_is_object_or_subtype(field_type))) {
            vector_drop(&(s->data.allOf));
            carbon_array_it_drop(ait);
            return FN_FAIL(ERR_BADTYPE, "keyword \"allOf\" expects an array of objects");
        }

        carbon_object_it *soit = carbon_array_it_object_value(ait);
        schema *allOf = (schema*) malloc(sizeof(schema));

        if (!(FN_IS_OK(schema_init(allOf , NULL)))) {
            carbon_object_it_drop(soit);
            carbon_array_it_drop(ait);
            free(allOf);
            return FN_FAIL_FORWARD();
        }
        if (!(FN_IS_OK(schema_generate(allOf, soit)))) {
            carbon_object_it_drop(soit);
            carbon_array_it_drop(ait);
            free(allOf);
            return FN_FAIL_FORWARD();
        }
        carbon_object_it_drop(soit);
        vector_push(&(s->data.allOf), allOf, 1);
    }
    carbon_array_it_drop(ait);

    s->applies.has_allOf = true;

    return FN_OK();

}


static inline fn_result schema_generate_handleKeyword_ifThenElse(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    vector_create(&(s->data.ifThenElse), NULL, sizeof(schema*), 3);
    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    
    if (!(carbon_field_type_is_object_or_subtype(field_type))) {
        vector_drop(&(s->data.ifThenElse));
        return FN_FAIL(ERR_BADTYPE, "keyword \"if\" expects an object");
    }

    carbon_object_it *soit = carbon_object_it_object_value(oit);
    schema *_if = (schema*) malloc(sizeof(schema));

    if (!(FN_IS_OK(schema_init(_if, NULL)))) {
        vector_drop(&(s->data.ifThenElse));
        carbon_object_it_drop(soit);
        free(_if);
        return FN_FAIL_FORWARD();
    }

    if (!(FN_IS_OK(schema_generate(_if, soit)))) {
        vector_drop(&(s->data.ifThenElse));
        carbon_object_it_drop(soit);
        free(_if);
        return FN_FAIL_FORWARD();
    }
    carbon_object_it_drop(soit);

    vector_push(&(s->data.ifThenElse), _if, 1);
    
    const char *keys_needed[2] = { "then", "else" };
    u64 keylen;

    for (int i = 0; i < 2; i++) {
        carbon_object_it_next(oit);
        carbon_object_it_prop_type(&field_type, oit);
        const char *_key = carbon_object_it_prop_name(&keylen, oit);
        const char *key = strndup(_key, keylen);

        if (!(carbon_field_type_is_object_or_subtype(field_type)) || strcmp(key, keys_needed[i])) {
            vector_drop(&(s->data.ifThenElse));
            return FN_FAIL(ERR_BADTYPE, "keyword \"if\" must be followed by the keywords \"then\" and \"else\" which have to be objects");
        }

        carbon_object_it *soit = carbon_object_it_object_value(oit);
        schema *val = (schema*) malloc(sizeof(schema));

        if (!(FN_IS_OK(schema_init(val, NULL)))) {
            vector_drop(&(s->data.ifThenElse));
            carbon_object_it_drop(soit);
            free(val);
            return FN_FAIL_FORWARD();
        }

        if (!(FN_IS_OK(schema_generate(val, soit)))) {
            vector_drop(&(s->data.ifThenElse));
            carbon_object_it_drop(soit);
            free(val);
            return FN_FAIL_FORWARD();
        }
        carbon_object_it_drop(soit);

        vector_push(&(s->data.ifThenElse), val, 1);
    }
    return FN_OK(); 
}


static inline fn_result schema_generate_handleKeyword(schema *s, const char *keyword, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, keyword, oit);
    if (strcmp(keyword, "type") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_type(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "minimum") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_minimum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "maximum") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_maximum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "exclusiveMinimum") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_exclusiveMinimum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "exclusiveMaximum") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_exclusiveMaximum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "multipleOf") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_multipleOf(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "minLength") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_minLength(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "maxLength") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_maxLength(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "pattern") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_pattern(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "format") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_format(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "formatMinimum") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_formatMinimum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "formatMaximum") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_formatMaximum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "formatExclusiveMinimum") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_formatExclusiveMinimum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "formatExclusiveMaximum") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_formatExclusiveMaximum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "minItems") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_minItems(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "maxItems") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_maxItems(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "uniqueItems") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_uniqueItems(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "items") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_items(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "additionalItems") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_additionalItems(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "contains") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_contains(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "minProperties") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_minProperties(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "maxProperties") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_maxProperties(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "required") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_required(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "properties") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_properties(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "patternProperties") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_patternProperties(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "additionalProperties") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_additionalProperties(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "dependencies") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_dependencies(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "propertyNames") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_propertyNames(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "patternRequired") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_patternRequired(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "enum") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_enum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "const") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_const(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "not") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_not(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "oneOf") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_oneOf(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "anyOf") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_anyOf(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "allOf") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_allOf(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "if") == 0) {
        if (!(FN_IS_OK(schema_generate_handleKeyword_ifThenElse(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else {
        //TODO: more error details
        return FN_FAIL(ERR_SCHEMA_UNDEF_KEYWORD, "");
    }
    return FN_OK();
}


END_DECL

#endif
