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
#include <jakson/schema/generate.h>
#include <jakson/schema/run.h>
#include <regex.h>


fn_result schema_validate(carbon *schemaFile, carbon *fileToVal) {
    FN_FAIL_IF_NULL(schemaFile, fileToVal);

    schema s;
    carbon_array_it ait;
    carbon_field_type_e field_type;

    if (!(FN_IS_OK(schema_init(&s, NULL)))) {
        carbon_array_it_drop(&ait);
        return FN_FAIL_FORWARD();
    }

    carbon_iterator_open(&ait, schemaFile);
    carbon_array_it_next(&ait);

    // a schema always has to be an object.
    carbon_array_it_field_type(&field_type, &ait);
    if (!(carbon_field_type_is_object_or_subtype(field_type))){
        carbon_array_it_drop(&ait);
        return FN_FAIL(ERR_BADTYPE, "schema has to be an object");
    }

    carbon_object_it *oit = carbon_array_it_object_value(&ait);
    if (!(FN_IS_OK(schema_generate(&s, oit)))) {
        carbon_object_it_drop(oit);
        carbon_array_it_drop(&ait);
        schema_drop(&s);
        return FN_FAIL_FORWARD();
    }
    carbon_object_it_drop(oit);
    carbon_array_it_drop(&ait);

    if (!(FN_IS_OK(schema_validate_run_fromFile(&s, fileToVal)))) {
        schema_drop(&s);
        return FN_FAIL_FORWARD();
    }

    return FN_OK();
}


fn_result schema_generate(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    while (carbon_object_it_next(oit)) {
        u64 key_len;
        const char *_keyword = carbon_object_it_prop_name(&key_len, oit);
        const char *keyword = strndup(_keyword, key_len);

        if (!(FN_IS_OK(schema_generate_handleKeyword(s, keyword, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    return FN_OK();
}

fn_result schema_validate_run(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    carbon_array_it_next(ait);
    carbon_field_type_e field_type;
    carbon_array_it_field_type(&field_type, ait);

    if (s->applies.has_type) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_type(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    // -------------- keywords for numbers --------------
    if (s->applies.has_minimum && carbon_field_type_is_number(field_type)) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_minimum(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_maximum && carbon_field_type_is_number(field_type)) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_maximum(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_exclusiveMinimum && carbon_field_type_is_number(field_type)) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_exclusiveMinimum(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_exclusiveMaximum && carbon_field_type_is_number(field_type)) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_exclusiveMaximum(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_multipleOf && carbon_field_type_is_number(field_type)) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_multipleOf(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    // -------------- keywords for strings --------------
    if (s->applies.has_minLength && carbon_field_type_is_string(field_type)) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_minLength(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_maxLength && carbon_field_type_is_string(field_type)) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_maxLength(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_pattern && carbon_field_type_is_string(field_type)) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_pattern(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_format && carbon_field_type_is_string(field_type)) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_format(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_formatMinimum && carbon_field_type_is_string(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_formatMinimum(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_formatMaximum && carbon_field_type_is_string(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_formatMaximum(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_formatExclusiveMinimum && carbon_field_type_is_string(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_formatExclusiveMinimum(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_formatExclusiveMaximum && carbon_field_type_is_string(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_formatExclusiveMaximum(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    // -------------- keywords for arrays --------------
    if (s->applies.has_minItems && carbon_field_type_is_array_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_minItems(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_maxItems && carbon_field_type_is_array_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_maxItems(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_uniqueItems && carbon_field_type_is_array_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_uniqueItems(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_items && carbon_field_type_is_array_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_items(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_contains &&carbon_field_type_is_array_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_contains(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    // -------------- keywords for objects --------------
    if (s->applies.has_minProperties && carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_minProperties(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_maxProperties && carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_maxProperties(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_required && carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_required(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_properties && carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_properties(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_patternProperties && carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_patternProperties(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_additionalProperties && carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_additionalProperties(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_dependencies && carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_dependencies(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_propertyNames && carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_propertyNames(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_patternRequired && carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_patternRequired(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    // -------------- keywords for all types --------------
    if (s->applies.has_enum) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_enum(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_const) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_const(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    // -------------- compound keywords --------------
    if (s->applies.has_not && carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_not(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_oneOf && carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_oneOf(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_anyOf && carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_anyOf(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_allOf && carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_allOf(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_ifThenElse && carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_ifThenElse(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }
    return FN_OK();
}


fn_result schema_validate_run_fromFile(schema *s, carbon *fileToVal) {
    FN_FAIL_IF_NULL(s, fileToVal);

    carbon_array_it ait;
    // FIXME: can't handle JSON files like this: [1,2,3,4,5], need to differentiate between root and sub array
    carbon_iterator_open(&ait, fileToVal);

    if (!(FN_IS_OK(schema_validate_run(s, &ait)))) {
        carbon_array_it_drop(&ait);
        return FN_FAIL_FORWARD();
    }
    return FN_OK();
}


