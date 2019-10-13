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

#ifndef SCHEMA_H
#define SCHEMA_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------
#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/fn_result.h>
#include <jakson/carbon.h>
#include <jakson/carbon/array_it.h>
#include <jakson/carbon/object_it.h>

BEGIN_DECL


enum type {
    NUMBER,
    STRING,
    BOOLEAN,
    BINARY,
    ARRAY,
    COLUMN,
    OBJECT,
    _NULL
} type;


typedef struct schema {

    const char *key_name;

    struct {

        u8 has_type : 1;
        // keywords for numbers:
        u8 has_minimum : 1;
        u8 has_maximum : 1;
        u8 has_exclusiveMinimum : 1;
        u8 has_exclusiveMaximum : 1;
        u8 has_multipleOf : 1;
        // keywords for strings:
        u8 has_minLength : 1;
        u8 has_maxLength : 1;
        u8 has_pattern : 1;
        u8 has_format : 1;
        u8 has_formatMinimum : 1;
        u8 has_formatMaximum : 1;
        u8 has_formatExclusiveMinimum : 1;
        u8 has_formatExclusiveMaximum : 1;
        // keywords for arrays:
        u8 has_minItems : 1;
        u8 has_maxItems : 1;
        u8 has_uniqueItems : 1;
        u8 has_items : 1;
        u8 has_additionalItems : 1;
        u8 has_contains : 1;
        // keywords for objects:
        u8 has_minProperties : 1;
        u8 has_maxProperties : 1;
        u8 has_required : 1;
        u8 has_properties : 1;
        u8 has_patternProperties : 1;
        u8 has_additionalProperties : 1;
        u8 has_dependencies : 1;
        u8 has_propertyNames : 1;
        u8 has_patternRequired : 1;
        // keywords for all types:
        u8 has_enum : 1;
        u8 has_const : 1;
        // compound keywords:
        u8 has_not : 1;
        u8 has_oneOf : 1;
        u8 has_anyOf : 1;
        u8 has_allOf : 1;
        u8 has_ifThenElse : 1;
    } applies;

    struct {
        vector type;
        long double minimum;
        long double maximum;
        long double exclusiveMinimum;
        long double exclusiveMaximum;
        long double multipleOf;
        u64 minLength;
        u64 maxLength;
        char *pattern;
        char *format;
        char *formatMinimum;
        char *formatMaximum;
        char *formatExclusiveMinimum;
        char *formatExclusiveMaximum;
        u64 minItems;
        u64 maxItems;
        bool uniqueItems;
        vector *items;
        bool additionalItems;
        struct schema *contains;
        u64 minProperties;
        u64 maxProperties;
        vector *required;
        carbon_object_it *properties;
        hashtable *patternProperties;
        bool additionalProperties;
        hashtable *dependencies;
        struct schema *propertyNames;
        vector *patternRequired;
        carbon_array_it *_enum;
        void* _const;
        struct schema *_not;
        vector *oneOf;
        vector *anyOf;
        vector *allOf;
        hashtable *ifThenElse;
    } data;

} schema;

static inline fn_result schema_init(schema *s, const char* key_name) {
    FN_FAIL_IF_NULL(s);

    if (key_name) {
        s->key_name = strdup(key_name);
    }
    else s->key_name = NULL;

    s->applies.has_type = false;
    s->applies.has_minimum = false;
    s->applies.has_maximum = false;
    s->applies.has_exclusiveMinimum = false;
    s->applies.has_exclusiveMaximum = false;
    s->applies.has_multipleOf = false;
    s->applies.has_minLength = false;
    s->applies.has_maxLength = false;
    s->applies.has_pattern = false;
    s->applies.has_format = false;
    s->applies.has_formatMinimum = false;
    s->applies.has_formatMaximum = false;
    s->applies.has_formatExclusiveMinimum = false;
    s->applies.has_formatExclusiveMaximum = false;
    s->applies.has_minItems = false;
    s->applies.has_maxItems = false;
    s->applies.has_uniqueItems = false;
    s->applies.has_items = false;
    s->applies.has_additionalItems = false;
    s->applies.has_contains = false;
    s->applies.has_minProperties = false;
    s->applies.has_maxProperties = false;
    s->applies.has_required = false;
    s->applies.has_properties = false;
    s->applies.has_patternProperties = false;
    s->applies.has_additionalProperties = false;
    s->applies.has_dependencies = false;
    s->applies.has_propertyNames = false;
    s->applies.has_patternRequired = false;
    s->applies.has_enum = false;
    s->applies.has_const = false;
    s->applies.has_not = false;
    s->applies.has_oneOf = false;
    s->applies.has_anyOf = false;
    s->applies.has_allOf = false;
    s->applies.has_ifThenElse = false;

    return FN_OK();
}

static inline fn_result longDoubleFromAit (bool *is_null, long double *val, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(is_null, val, ait);

    carbon_field_type_e field_type;
    carbon_array_it_field_type(&field_type, ait);

    if (carbon_field_type_is_floating(field_type)) {
        float tmp;
        carbon_array_it_float_value(is_null, &tmp, ait);
        if (!(*is_null)) {
            *val = (long double) tmp;
        }
    }
    else if (carbon_field_type_is_signed(field_type)) {
        i64 tmp;
        carbon_array_it_signed_value(is_null, &tmp, ait);
        if (!(*is_null)) {
            *val = (long double) tmp;
        }
    }
    else if (carbon_field_type_is_unsigned(field_type)) {
        u64 tmp;
        carbon_array_it_unsigned_value(is_null, &tmp, ait);
        if (!(*is_null)) {
            *val = (long double) tmp;
        }
    }
    else {
        return FN_FAIL(ERR_BADTYPE, "expected numeric value");
    }
    return FN_OK();
}

static inline fn_result longDoubleFromOit (bool *is_null, long double *val, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(is_null, val, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (carbon_field_type_is_floating(field_type)) {
        float tmp;
        carbon_object_it_float_value(is_null, &tmp, oit);
        if (!(*is_null)) {
            *val = (long double) tmp;
        }
    }
    else if (carbon_field_type_is_signed(field_type)) {
        i64 tmp;
        carbon_object_it_signed_value(is_null, &tmp, oit);
        if (!(*is_null)) {
            *val = (long double) tmp;
        }
    }
    else if (carbon_field_type_is_unsigned(field_type)) {
        u64 tmp;
        carbon_object_it_unsigned_value(is_null, &tmp, oit);
        if (!(*is_null)) {
            *val = (long double) tmp;
        }
    }
    else {
        return FN_FAIL(ERR_BADTYPE, "expected numeric value");
    }
    return FN_OK();
}

fn_result schema_validate(carbon *schemaFile, carbon *fileToVal);
fn_result schema_generate(schema *s, carbon_object_it *oit);
fn_result schema_validate_run(schema *s, carbon *fileToVal);


END_DECL

#endif
