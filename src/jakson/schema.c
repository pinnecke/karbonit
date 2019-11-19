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

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------
#include "schema.h"
#include <jakson/schema/validate.h>
#include <jakson/schema/generate.h>

fn_result schema_generate_from_json(schema *s, const char *json, carbon_key_e type, const void *key)
{
    FN_FAIL_IF_NULL(s, json);

    carbon doc;
    err err;

    if (!(carbon_from_json(&doc, json, type, key, &err))) {
        return FN_FAIL(ERR_JSONPARSEERR, err.details);
    }
    if (!(FN_IS_OK(schema_generate_from_carbon(s, &doc)))) {
        return FN_FAIL_FORWARD();
    }
    return FN_OK();
}


fn_result schema_generate_from_carbon(schema *s, carbon *doc)
{
    FN_FAIL_IF_NULL(s, doc);

    carbon_array_it ait;
    carbon_field_type_e field_type;

    if (!(FN_IS_OK(schema_init(s, NULL)))) {
        carbon_array_it_drop(&ait);
        return FN_FAIL_FORWARD();
    }

    carbon_iterator_open(&ait, doc);
    carbon_array_it_next(&ait);

    carbon_array_it_field_type(&field_type, &ait);
    if (!(carbon_field_type_is_object_or_subtype(field_type))){
        carbon_array_it_drop(&ait);
        return FN_FAIL(ERR_BADTYPE, "schema has to be an object");
    }

    carbon_object_it *oit = carbon_array_it_object_value(&ait);

    if (!(FN_IS_OK(schema_generate_run(s, oit)))) {
        carbon_object_it_drop(oit);
        carbon_array_it_drop(&ait);
        schema_drop(s);
        return FN_FAIL_FORWARD();
    }

    carbon_object_it_drop(oit);
    carbon_array_it_drop(&ait);
    return FN_OK();
}


fn_result schema_validate_json(schema *s, const char *json, carbon_key_e type, const void *key)
{
    FN_FAIL_IF_NULL(s, json);

    carbon doc;
    err err;

    if (!(carbon_from_json(&doc, json, type, key, &err))) {
        return FN_FAIL(ERR_JSONPARSEERR, err.details);
    }

    if (!(FN_IS_OK(schema_validate_carbon(s, &doc)))) {
        schema_drop(s);
        carbon_drop(&doc);
        return FN_FAIL_FORWARD();
    }
    return FN_OK();
}


fn_result schema_validate_carbon(schema *s, carbon *doc)
{
    FN_FAIL_IF_NULL(s, doc);
    
    carbon_array_it ait;

    carbon_iterator_open(&ait, doc);
    carbon_array_it_next(&ait);

    if (!(FN_IS_OK(schema_validate_run(s, &ait)))) {
        carbon_array_it_drop(&ait);
        return FN_FAIL_FORWARD();
    }
    return FN_OK();
}


void schema_drop(schema *s) {

    if (s->key_name) {
        free((void*)s->key_name);
    }
    if (s->applies.has_type) {
        vector_drop(&(s->data.type));
    }
    if (s->applies.has_items) {
        for (size_t i = 0; i < vector_length(&(s->data.items)); i++) {
            schema_drop((schema*)vector_at(&(s->data.items), i));
        }
        vector_drop(&(s->data.items));
    }
    if (s->applies.has_required) {
        for (size_t i = 0; i < vector_length(&(s->data.required)); i++) {
        }
        vector_drop(&(s->data.required));
    }
    if (s->applies.has_patternRequired) {
        for (size_t i = 0; i < vector_length(&(s->data.patternRequired)); i++) {
        }
        vector_drop(&(s->data.patternRequired));
    }
    if (s->applies.has_oneOf) {
        for (size_t i = 0; i < vector_length(&(s->data.oneOf)); i++) {
            schema_drop((schema*)vector_at(&(s->data.oneOf), i));
        }
        vector_drop(&(s->data.oneOf));
    }
    if (s->applies.has_anyOf) {
        for (size_t i = 0; i < vector_length(&(s->data.anyOf)); i++) {
            schema_drop((schema*)vector_at(&(s->data.anyOf), i));
        }
        vector_drop(&(s->data.anyOf));
    }
    if (s->applies.has_allOf) {
        for (size_t i = 0; i < vector_length(&(s->data.allOf)); i++) {
            schema_drop((schema*)vector_at(&(s->data.allOf), i));
        }
        vector_drop(&(s->data.allOf));
    }
    if (s->applies.has_patternProperties) {
        //for (size_t i = 0; i < vector_length(&(s->data.patternProperties)); i++) {
        //    schema_drop((schema*)vector_at(&(s->data.patternProperties), i));
        //    free((void*)vector_at(&(s->data.patternProperties), i));
        //}
        //vector_drop(&(s->data.patternProperties));
    }
    if (s->applies.has_dependencies) {
            for (size_t i = 0; i < vector_length(&(s->data.dependencies)); i++) {
                if (s->applies.dependencies_isObject) {
                    schema_drop((schema*)vector_at(&(s->data.dependencies), i));
                }
                else {
                    vector_drop((vector*)vector_at(&(s->data.dependencies), i));
                }
            }
        vector_drop(&(s->data.dependencies));
    }
    // TODO: implement
    if (s->applies.has_ifThenElse) {
    }
    //TODO: implement
    if (s->applies.has_enum) {
        carbon_drop(&(s->data._enum));
    }
    if (s->applies.has_pattern) {
        free(s->data.pattern);
    }
    if (s->applies.has_propertyNames) {
        free(s->data.propertyNames);
    }
    if (s->applies.has_contains) {
        free(s->data.contains);
    }
    if (s->applies.has_not) {
        free(s->data._not);
    }
    if (s->applies.has_additionalItems && !(s->applies.additionalItemsIsBool)) {
        free(s->data.additionalItems);
    }
    //TODO: implement
    //if (s->applies.has_properties) {
    //}
}
