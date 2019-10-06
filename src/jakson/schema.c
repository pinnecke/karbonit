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
#include <jakson/carbon.h>
#include <jakson/carbon/array_it.h>
#include <jakson/carbon/object_it.h>
#include <jakson/schema/keywords.h>


fn_result schema_validate(carbon *schemaFile, carbon *fileToVal) {
    FN_FAIL_IF_NULL(schemaFile, fileToVal);

    schema s;
    carbon_array_it ait;
    carbon_field_type_e field_type;

    if (!(FN_IS_OK(schema_init(&s, NULL)))) {
        return FN_FAIL_FORWARD();
    }

    carbon_iterator_open(&ait, schemaFile);
    carbon_array_it_next(&ait);

    // a schema always has to be an object.
    carbon_array_it_field_type(&field_type, &ait);
    if (!(carbon_field_type_is_object_or_subtype(field_type))){
        return FN_FAIL(ERR_BADTYPE, "schema has to be an object");
    }

    carbon_object_it *oit = carbon_array_it_object_value(&ait);
    if (!(FN_IS_OK(schema_generate(&s, oit)))) {
        return FN_FAIL_FORWARD();
    }

    if (!(FN_IS_OK(schema_validate_run(&s, fileToVal)))) {
        return FN_FAIL_FORWARD();
    }

    return FN_OK();
}


fn_result schema_generate(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s);

    while (carbon_object_it_next(oit)) {
        u64 key_len;
        const char *_keyword = carbon_object_it_prop_name(&key_len, oit);
        const char *keyword = strndup(_keyword, key_len);

        if (!(FN_IS_OK(schema_keyword_handle(s, keyword, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    return FN_OK();
}


fn_result schema_validate_run(schema *s, carbon *fileToVal) {
    FN_FAIL_IF_NULL(s, fileToVal);

    carbon_array_it ait;
    carbon_field_type_e field_type;
    carbon_iterator_open(&ait, fileToVal);

    if (s->applies.has_type) {
        bool passed = false;
        carbon_array_it_next(&ait);
        while (!(vector_is_empty(s->data.type))) {
            carbon_array_it_field_type(&field_type, &ait);
            int constraint = (int)(vector_pop(s->data.type));

            if (constraint == NUMBER && carbon_field_type_is_number(field_type)) {
                passed = true;
            }
            else if (constraint == STRING && carbon_field_type_is_string(field_type)) {
                passed = true;
            }
            else if (constraint == BOOLEAN && carbon_field_type_is_boolean(field_type)) {
                passed = true;
            }
            else if (constraint == BINARY && carbon_field_type_is_binary(field_type)) {
                passed = true;
            }
            else if (constraint == ARRAY && carbon_field_type_is_array_or_subtype(field_type)) {
                passed = true;
            }
            else if (constraint == COLUMN && carbon_field_type_is_column_or_subtype(field_type)) {
                passed = true;
            }
            else if (constraint == OBJECT && carbon_field_type_is_object_or_subtype(field_type)) {
                passed = true;
            }
            else if (constraint == _NULL && carbon_field_type_is_null(field_type)) {
                passed = true;
            }
            if (!(passed)) {
                return RESULT_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "failed \"type\" constraint");
            }
            if (carbon_array_it_has_next(&ait)) {
                return RESULT_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "failed \"type\" constraint, expected atomar element"); 
            }
            if (passed) {
                break;
            }
        }
    }

    return FN_OK();

}
