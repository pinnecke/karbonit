
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
#include "generate.h"



fn_result schema_init(schema *s, const char* key_name) {
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
    s->applies.has_minItems = false;
    s->applies.has_maxItems = false;
    s->applies.has_uniqueItems = false;
    s->applies.has_items = false;
    s->applies.has_additionalItems = false;
    s->applies.additionalItemsIsBool = false;
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
    s->applies.has_not = false;
    s->applies.has_oneOf = false;
    s->applies.has_anyOf = false;
    s->applies.has_allOf = false;
    s->applies.has_ifThenElse = false;
    s->applies.items_isObject = false;
    s->applies.dependencies_isObject = false;

    return FN_OK();
}


fn_result longDoubleFromAit(bool *is_null, long double *val, carbon_array_it *ait) {
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


fn_result longDoubleFromOit (bool *is_null, long double *val, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword(schema *s, const char *keyword, carbon_object_it *oit) {
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


fn_result schema_generate_run(schema *s, carbon_object_it *oit)
{
    FN_FAIL_IF_NULL(s, oit);

    while (carbon_object_it_next(oit)) {
        u64 key_len;
        const char *_keyword = carbon_object_it_prop_name(&key_len, oit);
        char *keyword = strndup(_keyword, key_len);

        if (!(FN_IS_OK(schema_generate_handleKeyword(s, keyword, oit)))) {
            free(keyword);
            return FN_FAIL_FORWARD();
        }
        free(keyword);
    }
    return FN_OK();
}


fn_result schema_generate_handleKeyword_minItems(schema *s, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword_maxItems(schema *s, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword_uniqueItems(schema *s, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword_items(schema *s, carbon_object_it *oit) {
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
        if (!(FN_IS_OK(schema_generate_run(items, soit)))) {
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
            if (!(FN_IS_OK(schema_generate_run(items, soit)))) {
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


fn_result schema_generate_handleKeyword_additionalItems(schema *s, carbon_object_it *oit) {
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

        if(!(FN_IS_OK(schema_generate_run(s->data.additionalItems, soit)))) {
            carbon_object_it_drop(soit);
            schema_drop(s->data.additionalItems);
            return FN_FAIL_FORWARD();
        }

        s->applies.has_additionalItems = true;
        return FN_OK();
    }
    return FN_FAIL(ERR_BADTYPE, "keyword \"additionalItems\" expects a boolean or an object value");
}


fn_result schema_generate_handleKeyword_contains(schema *s, carbon_object_it *oit) {
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
    if (!(FN_IS_OK(schema_generate_run(contains, soit)))) {
        carbon_object_it_drop(soit);
        free(contains);
        return FN_FAIL_FORWARD();
    }
    carbon_object_it_drop(soit);

    s->applies.has_contains = true;
    s->data.contains = contains;

    return FN_OK();
}

//TODO: needs carbon_from_array_it func
fn_result schema_generate_handleKeyword_enum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    UNUSED(s);
    UNUSED(oit);
    //carbon_field_type_e field_type;
    //carbon_object_it_prop_type(&field_type, oit);

    //if (!(carbon_field_type_is_array_or_subtype(field_type))) {
    //    return FN_FAIL(ERR_BADTYPE, "keyword \"enum\" expects an array");
    //}

    //carbon_array_it *ait = carbon_object_it_array_value(oit);
    //carbon_from_array_it(&(s->data._enum), ait);
    //carbon_array_it_drop(ait);

    //s->applies.has_enum = true;

    //return FN_OK();
    return FN_FAIL(ERR_NOTIMPL, "keyword enum not implemented yet");
}


fn_result schema_generate_handleKeyword_not(schema *s, carbon_object_it *oit) {
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
    if (!(FN_IS_OK(schema_generate_run(_not, soit)))) {
        carbon_object_it_drop(soit);
        free(_not);
        return FN_FAIL_FORWARD();
    }
    carbon_object_it_drop(soit);

    s->applies.has_not = true;
    s->data._not = _not;

    return FN_OK();
}


fn_result schema_generate_handleKeyword_oneOf(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_array_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"oneOf\" expects an array of objects");
    }

    carbon_array_it *ait = carbon_object_it_array_value(oit);
    vector_create(&(s->data.oneOf), NULL, sizeof(schema), 5);

    while (carbon_array_it_next(ait)) {

        carbon_array_it_field_type(&field_type, ait);

        if (!(carbon_field_type_is_object_or_subtype(field_type))) {
            vector_drop(&(s->data.oneOf));
            carbon_array_it_drop(ait);
            return FN_FAIL(ERR_BADTYPE, "keyword \"oneOf\" expects an array of objects");
        }

        carbon_object_it *soit = carbon_array_it_object_value(ait);
        schema *oneOf = VECTOR_NEW_AND_GET(&(s->data.oneOf), schema);

        if (!(FN_IS_OK(schema_init(oneOf , NULL)))) {
            carbon_object_it_drop(soit);
            carbon_array_it_drop(ait);
            vector_drop(&(s->data.oneOf));
            return FN_FAIL_FORWARD();
        }
        if (!(FN_IS_OK(schema_generate_run(oneOf, soit)))) {
            carbon_object_it_drop(soit);
            carbon_array_it_drop(ait);
            vector_drop(&(s->data.oneOf));
            return FN_FAIL_FORWARD();
        }
        carbon_object_it_drop(soit);
    }
    carbon_array_it_drop(ait);

    s->applies.has_oneOf = true;

    return FN_OK();
}


fn_result schema_generate_handleKeyword_anyOf(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_array_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"anyOf\" expects an array of objects");
    }

    carbon_array_it *ait = carbon_object_it_array_value(oit);
    vector_create(&(s->data.anyOf), NULL, sizeof(schema), 5);

    while (carbon_array_it_next(ait)) {

        carbon_array_it_field_type(&field_type, ait);

        if (!(carbon_field_type_is_object_or_subtype(field_type))) {
            vector_drop(&(s->data.anyOf));
            carbon_array_it_drop(ait);
            return FN_FAIL(ERR_BADTYPE, "keyword \"anyOf\" expects an array of objects");
        }

        carbon_object_it *soit = carbon_array_it_object_value(ait);
        schema *anyOf = VECTOR_NEW_AND_GET(&(s->data.anyOf), schema);

        if (!(FN_IS_OK(schema_init(anyOf , NULL)))) {
            carbon_object_it_drop(soit);
            carbon_array_it_drop(ait);
            free(anyOf);
            return FN_FAIL_FORWARD();
        }
        if (!(FN_IS_OK(schema_generate_run(anyOf, soit)))) {
            carbon_object_it_drop(soit);
            carbon_array_it_drop(ait);
            free(anyOf);
            return FN_FAIL_FORWARD();
        }
        carbon_object_it_drop(soit);
    }
    carbon_array_it_drop(ait);

    s->applies.has_anyOf = true;

    return FN_OK();
}


fn_result schema_generate_handleKeyword_allOf(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);

    if (!(carbon_field_type_is_array_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"allOf\" expects an array of objects");
    }

    carbon_array_it *ait = carbon_object_it_array_value(oit);
    vector_create(&(s->data.allOf), NULL, sizeof(schema), 5);

    while (carbon_array_it_next(ait)) {

        carbon_array_it_field_type(&field_type, ait);

        if (!(carbon_field_type_is_object_or_subtype(field_type))) {
            vector_drop(&(s->data.allOf));
            carbon_array_it_drop(ait);
            return FN_FAIL(ERR_BADTYPE, "keyword \"allOf\" expects an array of objects");
        }

        carbon_object_it *soit = carbon_array_it_object_value(ait);
        schema *allOf = VECTOR_NEW_AND_GET(&(s->data.allOf), schema);

        if (!(FN_IS_OK(schema_init(allOf , NULL)))) {
            carbon_object_it_drop(soit);
            carbon_array_it_drop(ait);
            free(allOf);
            return FN_FAIL_FORWARD();
        }
        if (!(FN_IS_OK(schema_generate_run(allOf, soit)))) {
            carbon_object_it_drop(soit);
            carbon_array_it_drop(ait);
            free(allOf);
            return FN_FAIL_FORWARD();
        }
        carbon_object_it_drop(soit);
    }
    carbon_array_it_drop(ait);

    s->applies.has_allOf = true;

    return FN_OK();

}


fn_result schema_generate_handleKeyword_ifThenElse(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    vector_create(&(s->data.ifThenElse), NULL, sizeof(schema), 3);
    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    
    if (!(carbon_field_type_is_object_or_subtype(field_type))) {
        vector_drop(&(s->data.ifThenElse));
        return FN_FAIL(ERR_BADTYPE, "keyword \"if\" expects an object");
    }

    carbon_object_it *soit = carbon_object_it_object_value(oit);
    schema *_if = VECTOR_NEW_AND_GET(&(s->data.ifThenElse), schema); 

    if (!(FN_IS_OK(schema_init(_if, NULL)))) {
        vector_drop(&(s->data.ifThenElse));
        carbon_object_it_drop(soit);
        free(_if);
        return FN_FAIL_FORWARD();
    }

    if (!(FN_IS_OK(schema_generate_run(_if, soit)))) {
        vector_drop(&(s->data.ifThenElse));
        carbon_object_it_drop(soit);
        free(_if);
        return FN_FAIL_FORWARD();
    }
    carbon_object_it_drop(soit);

    const char *keys_needed[2] = { "then", "else" };
    u64 keylen;
    for (int i = 0; i < 2; i++) {
        carbon_object_it_next(oit);
        carbon_object_it_prop_type(&field_type, oit);
        const char *_key = carbon_object_it_prop_name(&keylen, oit);
        char *key = strndup(_key, keylen);

        if (!(carbon_field_type_is_object_or_subtype(field_type)) || strcmp(key, keys_needed[i])) {
            vector_drop(&(s->data.ifThenElse));
            free(key);
            return FN_FAIL(ERR_BADTYPE, "keyword \"if\" must be followed by the keywords \"then\" and \"else\" which have to be objects");
        }

        carbon_object_it *soit = carbon_object_it_object_value(oit);
        schema *val = VECTOR_NEW_AND_GET(&(s->data.ifThenElse), schema);

        if (!(FN_IS_OK(schema_init(val, NULL)))) {
            vector_drop(&(s->data.ifThenElse));
            carbon_object_it_drop(soit);
            free(val);
            free(key);
            return FN_FAIL_FORWARD();
        }

        if (!(FN_IS_OK(schema_generate_run(val, soit)))) {
            vector_drop(&(s->data.ifThenElse));
            carbon_object_it_drop(soit);
            free(val);
            return FN_FAIL_FORWARD();
        }
        carbon_object_it_drop(soit);
        free(key);
    }
    s->applies.has_ifThenElse = true;

    return FN_OK(); 
}

// TODO: allow for type to be a string
fn_result schema_generate_handleKeyword_type(schema *s, carbon_object_it *oit) {
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
        char *str = strndup(_str, strlen);
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
            free(str);
            return FN_FAIL(ERR_UNSUPPORTEDTYPE, "\"type\" keyword contains unsupported constraint");
        }
        vector_push(&(s->data.type), &type, 1);
        free(str);
    }
    carbon_array_it_drop(ait);

    s->applies.has_type = true;

    return FN_OK();
}


fn_result schema_generate_handleKeyword_minimum(schema *s, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword_maximum(schema *s, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword_exclusiveMinimum(schema *s, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword_exclusiveMaximum(schema *s, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword_multipleOf(schema *s, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword_properties(schema *s, carbon_object_it *oit) {
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
        char *key = strndup(_key, keylen);

        schema *item = (schema*) malloc(sizeof(schema));
        if (!(FN_IS_OK(schema_init(item, key)))) {
            carbon_object_it_drop(ssoit);
            carbon_object_it_drop(soit);
            free(key);
            free(item);
            return FN_FAIL_FORWARD();
        }
        if (!(FN_IS_OK(schema_generate_run(item, ssoit)))) {
            carbon_object_it_drop(ssoit);
            carbon_object_it_drop(soit);
            free(key);
            free(item);
            return FN_FAIL_FORWARD();
        }
        carbon_object_it_drop(ssoit);
        vector_push(&(s->data.properties), item, 1);
        free(key);
    }
    carbon_object_it_drop(soit);

    s->applies.has_properties = true;

    return FN_OK();
}


fn_result schema_generate_handleKeyword_minProperties(schema *s, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword_maxProperties(schema *s, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword_required(schema *s, carbon_object_it *oit) {
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
        char *required = strndup(_required, strlen);
        vector_push(&(s->data.required), required, 1);
        free(required);
    }
    carbon_array_it_drop(ait);

    s->applies.has_required = true;

    return FN_OK();
}


fn_result schema_generate_handleKeyword_patternProperties(schema *s, carbon_object_it *oit) {
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
        char *key = strndup(oit->field.key.name, oit->field.key.name_len);

        if (!(FN_IS_OK(schema_init(patternProperty, key)))) {
            carbon_object_it_drop(soit);
            vector_drop(&(s->data.patternProperties));
            free(patternProperty);
            free(key);
            return FN_FAIL_FORWARD();
        }

        carbon_object_it *ssoit = carbon_object_it_object_value(soit);

        if (!(FN_IS_OK(schema_generate_run(patternProperty, ssoit)))) {
            carbon_object_it_drop(ssoit);      
            carbon_object_it_drop(soit);
            vector_drop(&(s->data.patternProperties));
            free(patternProperty);
            return FN_FAIL_FORWARD();
        }
        vector_push(&(s->data.patternProperties), patternProperty, 1);
        carbon_object_it_drop(ssoit);      
        free(key);
    }
    carbon_object_it_drop(soit);

    s->applies.has_patternProperties = true;

    return FN_OK();
}


fn_result schema_generate_handleKeyword_additionalProperties(schema *s, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword_dependencies(schema *s, carbon_object_it *oit) {
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
            char *key = strndup(_key, keylen);

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
                    free(key);
                    carbon_object_it_drop(soit);
                    return FN_FAIL(ERR_BADTYPE, "keyword \"dependencies\" failed: array elem not of type string");
                }

                u64 strlen;
                const char *_str = carbon_array_it_string_value(&strlen, ait);
                char *str = strndup(_str, strlen);
                vector_push(item, str, 1);
                free(str);
            }
            free(key);
            carbon_array_it_drop(ait);
        }

        else if (carbon_field_type_is_object_or_subtype(field_type)) {
            if (isArray) {
                vector_drop(&(s->data.dependencies));
                carbon_object_it_drop(soit);
                return FN_FAIL(ERR_BADTYPE, "keyword \"dependencies\" failed: mixed schemas and arrays");
            }
            s->applies.dependencies_isObject = true;

            schema *dependency = (schema*) malloc(sizeof(schema));
            u64 keylen;
            const char *_key = carbon_object_it_prop_name(&keylen, oit);
            char *key = strndup(_key, keylen);

            if (!(FN_IS_OK(schema_init(dependency, key)))) {
                carbon_object_it_drop(soit);
                free(dependency);
                free(key);
                vector_drop(&(s->data.dependencies));
                return FN_FAIL_FORWARD();
            }

            carbon_object_it *ssoit = carbon_object_it_object_value(soit);

            if (!(FN_IS_OK(schema_generate_run(dependency, ssoit)))) {
                carbon_object_it_drop(ssoit);      
                carbon_object_it_drop(soit);
                free(dependency);
                free(key);
                vector_drop(&(s->data.dependencies));
                return FN_FAIL_FORWARD();
            }
            vector_push(&(s->data.dependencies), dependency, 1);
            carbon_object_it_drop(ssoit);      
            free(key);
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


fn_result schema_generate_handleKeyword_propertyNames(schema *s, carbon_object_it *oit) {
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
    if (!(FN_IS_OK(schema_generate_run(propertyNames, soit)))) {
        carbon_object_it_drop(soit);
        free(propertyNames);
        return FN_FAIL_FORWARD();
    }
    carbon_object_it_drop(soit);

    s->applies.has_propertyNames = true;
    s->data.propertyNames = propertyNames;

    return FN_OK();
}


fn_result schema_generate_handleKeyword_patternRequired(schema *s, carbon_object_it *oit) {
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
        char *pattern = strndup(_pattern, strlen);
        vector_push(&(s->data.patternRequired), pattern, 1);
        free(pattern);
    }
    carbon_array_it_drop(ait);

    s->applies.has_patternRequired = true;

    return FN_OK();
}


fn_result schema_generate_handleKeyword_minLength(schema *s, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword_maxLength(schema *s, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword_pattern(schema *s, carbon_object_it *oit) {
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


fn_result schema_generate_handleKeyword_format(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    carbon_object_it_prop_type(&field_type, oit);
    if (!(carbon_field_type_is_string(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"format\" expects a string value");
    }
    u64 strlen;
    const char* _str = carbon_object_it_string_value(&strlen, oit);
    char *str = strndup(_str, strlen);

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
        free(str);
        return FN_FAIL(ERR_SCHEMA_UNDEF_KEYWORD, "keyword \"format\" defines unknown format"); 
    }
    free(str);
    s->applies.has_format = true;
    s->data.format = format;

    return FN_OK();
}