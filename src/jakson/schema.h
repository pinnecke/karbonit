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
    BINARY,
    BOOLEAN,
    ARRAY,
    COLUMN,
    OBJECT,
    _NULL
} type;


enum format {
    DATE,
    DATETIME,
    URI,
    EMAIL,
    HOSTNAME,
    IPV4,
    IPV6,
    REGEX
} format;


typedef struct schema {
    const char *key_name;
    struct {
        vector type;
        vector items;
        vector required;
        vector patternRequired;
        vector oneOf;
        vector anyOf;
        vector allOf;
        vector patternProperties;
        vector dependencies;
        vector ifThenElse;
        vector properties;
        carbon _enum;
        long double minimum;
        long double maximum;
        long double exclusiveMinimum;
        long double exclusiveMaximum;
        long double multipleOf;
        u64 minLength;
        u64 maxLength;
        u64 minItems;
        u64 maxItems;
        u64 minProperties;
        u64 maxProperties;
        u8 format;
        char *pattern;
        char *formatMinimum;
        char *formatMaximum;
        char *formatExclusiveMinimum;
        char *formatExclusiveMaximum;
        struct schema *propertyNames;
        struct schema *contains;
        struct schema *_not;
        struct schema *additionalItems;
        bool additionalItemsBool;
        bool additionalProperties;
        bool uniqueItems;
    } data;

    struct {
        // keywords for all types:
        u8 has_type : 1;
        u8 has_enum : 1;
        // compound keywords:
        u8 has_not : 1;
        u8 has_oneOf : 1;
        u8 has_anyOf : 1;
        u8 has_allOf : 1;
        u8 has_ifThenElse : 1;
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
        u8 items_isObject : 1;
        u8 has_additionalItems : 1;
        u8 additionalItemsIsBool : 1;
        u8 has_contains : 1;
        // keywords for objects:
        u8 has_minProperties : 1;
        u8 has_maxProperties : 1;
        u8 has_required : 1;
        u8 has_properties : 1;
        u8 has_patternProperties : 1;
        u8 has_additionalProperties : 1;
        u8 has_dependencies : 1;
        u8 dependencies_isObject : 1;
        u8 has_propertyNames : 1;
        u8 has_patternRequired : 1;
    } applies;
} schema;


fn_result schema_generate_from_json(schema *s, const char *json, carbon_key_e type, const void *key);
fn_result schema_generate_from_carbon(schema *s, carbon *doc);
fn_result schema_validate_carbon(schema *s, carbon *doc);
fn_result schema_validate_json(schema *s, const char *json, carbon_key_e, const void *key);
void schema_drop(schema *s);


END_DECL

#endif
