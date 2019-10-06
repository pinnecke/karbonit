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
        u8 has_keyword_for_number : 1;
        u8 has_keyword_for_string : 1;
        u8 has_keyword_for_array : 1;
        u8 has_keyword_for_objects : 1;

        u8 has_type : 1;
        // keywords for numbers:
        u8 has_min : 1;
        u8 has_max : 1;
        u8 has_exclMin : 1;
        u8 has_exclMax : 1;
        u8 has_multOf : 1;
        // keywords for strings:
        u8 has_minLen : 1;
        u8 has_maxLen : 1;
        u8 has_pattern : 1;
        u8 has_format : 1;
        u8 has_formatMin : 1;
        u8 has_formatMax : 1;
        u8 has_formatExclMin : 1;
        u8 has_formatExclMax : 1;
        // keywords for arrays:
        u8 has_minItems : 1;
        u8 has_maxItems : 1;
        u8 has_uniqueItems : 1;
        u8 has_items : 1;
        u8 has_addItems : 1;
        u8 has_contains : 1;
        // keywords for objects:
        u8 has_minProp : 1;
        u8 has_maxProp : 1;
        u8 has_required : 1;
        u8 has_properties : 1;
        u8 has_patternProp : 1;
        u8 has_addProp : 1;
        u8 has_depends : 1;
        u8 has_propNames : 1;
        u8 has_patternReq : 1;
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
        vector *type;
        float min;
        float max;
        float exclMin;
        float exclMax;
        float multOf;
        unsigned int minLen;
        unsigned int maxLen;
        char *pattern;
        char *format;
        char *formatMin;
        char *formatMax;
        char *formatExclMin;
        char *formatExclMax;
        unsigned int minItems;
        unsigned int maxItems;
        bool uniqueItems;
        vector *items;
        bool addItems;
        struct schema *contains;
        unsigned int minProp;
        unsigned int maxProp;
        vector *required;
        carbon_object_it *properties;
        hashtable *patternProp;
        struct schema *addProperties;
        hashtable *depends;
        struct schema *propertyNames;
        vector *patternReq;
        carbon_array_it *_enum;
        void* _const;
        struct schema *_not;
        vector *oneOf;
        vector *anyOf;
        vector *allOf;
        hashtable *ifThenElse;
    } data;

} schema;

inline fn_result schema_init(schema *s, const char* key_name) {
    FN_FAIL_IF_NULL(s);

    // TODO: what happens when I strdup a nullptr?
    s->key_name = strdup(key_name);

    s->applies.has_keyword_for_number = false;
    s->applies.has_keyword_for_string = false;
    s->applies.has_keyword_for_array = false;
    s->applies.has_keyword_for_objects = false;

    s->applies.has_type = false;
    s->applies.has_min = false;
    s->applies.has_max = false;
    s->applies.has_exclMin = false;
    s->applies.has_exclMax = false;
    s->applies.has_multOf = false;
    s->applies.has_minLen = false;
    s->applies.has_maxLen = false;
    s->applies.has_pattern = false;
    s->applies.has_format = false;
    s->applies.has_formatMin = false;
    s->applies.has_formatMax = false;
    s->applies.has_formatExclMin = false;
    s->applies.has_formatExclMax = false;
    s->applies.has_minItems = false;
    s->applies.has_maxItems = false;
    s->applies.has_uniqueItems = false;
    s->applies.has_items = false;
    s->applies.has_addItems = false;
    s->applies.has_contains = false;
    s->applies.has_minProp = false;
    s->applies.has_maxProp = false;
    s->applies.has_required = false;
    s->applies.has_properties = false;
    s->applies.has_patternProp = false;
    s->applies.has_addProp = false;
    s->applies.has_depends = false;
    s->applies.has_propNames = false;
    s->applies.has_patternReq = false;
    s->applies.has_enum = false;
    s->applies.has_const = false;
    s->applies.has_not = false;
    s->applies.has_oneOf = false;
    s->applies.has_anyOf = false;
    s->applies.has_allOf = false;
    s->applies.has_ifThenElse = false;

    return FN_OK();
}


fn_result schema_validate(carbon *schemaFile, carbon *fileToVal);
fn_result schema_generate(schema *s, carbon_object_it *oit);
fn_result schema_validate_run(schema *s, carbon *fileToVal);


END_DECL

#endif
