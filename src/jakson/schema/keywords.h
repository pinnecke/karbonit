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

BEGIN_DECL


inline fn_result schema_keyword_handle_properties(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    
    carbon_object_it_prop_type(&field_type, oit);
    if (!(carbon_field_type_is_object_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"properties\" expects an object");
    }
    carbon_object_it p_oit;
    carbon_object_it_clone(&p_oit, carbon_object_it_object_value(oit));
    s->data.properties = &p_oit;        
    s->applies.has_properties = true;
    s->applies.has_keyword_for_objects = true;
    
    return FN_OK();
}


inline fn_result schema_keyword_handle_type(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);

    carbon_field_type_e field_type;
    vector ofType(int) vec;
    vector_create(&vec, NULL, sizeof(int), 8);
    
    carbon_object_it_prop_type(&field_type, oit);
    if (!(carbon_field_type_is_array_or_subtype(field_type))) {
        return FN_FAIL(ERR_BADTYPE, "keyword \"type\" expects an array");
    }

    carbon_array_it *ait = carbon_object_it_array_value(oit);

    while (carbon_array_it_next(ait)) {
        u64 strlen;
        const char* _str = carbon_array_it_string_value(&strlen, ait);
        const char* str = strndup(_str, strlen);
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
            return FN_FAIL(ERR_UNSUPPORTEDTYPE, "\"type\" keyword contains unsupported constraint");
        }
        vector_push(&vec, &type, 1);
    }

    s->applies.has_type = true;
    s->data.type = &vec;

    return FN_OK();
}


inline fn_result schema_keyword_handle_minimum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_maximum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_exclusiveMinimum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_exclusiveMaximum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_multipleOf(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_minLength(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_maxLength(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_pattern(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_format(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_formatMinimum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_formatMaximum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_formatExclusiveMinimum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_formatExclusiveMaximum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_minItems(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_maxItems(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_uniqueItems(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_items(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_additionalItems(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_contains(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_minProperties(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_maxProperties(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_required(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_patternProperties(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_additionalProperties(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_dependencies(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_propertyNames(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_patternRequired(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_enum(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_const(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_not(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_oneOf(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_anyOf(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_allOf(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}


inline fn_result schema_keyword_handle_ifThenElse(schema *s, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, oit);
    UNUSED(s);
    UNUSED(oit);

    return FN_FAIL(ERR_NOTIMPL, "handler function for schema keyword not yet implemented");
}

inline fn_result schema_keyword_handle(schema *s, const char *keyword, carbon_object_it *oit) {
    FN_FAIL_IF_NULL(s, keyword, oit);

    if (strcmp(keyword, "type") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_type(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "minimum") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_minimum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "maximum") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_maximum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "exclusiveMinimum") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_exclusiveMinimum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "exclusiveMaximum") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_exclusiveMaximum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "multipleOf") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_multipleOf(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "minLength") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_minLength(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "maxLength") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_maxLength(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "pattern") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_pattern(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "format") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_format(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "formatMinimum") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_formatMinimum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "formatMaximum") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_formatMaximum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "formatExclusiveMinimum") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_formatExclusiveMinimum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "formatExclusiveMaximum") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_formatExclusiveMaximum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "minItems") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_minItems(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "maxItems") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_maxItems(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "uniqueItems") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_uniqueItems(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "items") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_items(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "additionalItems") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_additionalItems(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "contains") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_contains(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "minProperties") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_minProperties(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "maxProperties") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_maxProperties(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "required") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_required(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "properties") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_properties(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "patternProperties") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_patternProperties(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "additionalProperties") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_additionalProperties(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "dependencies") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_dependencies(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "propertyNames") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_propertyNames(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "patternRequired") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_patternRequired(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "enum") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_enum(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "const") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_const(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "not") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_not(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "oneOf") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_oneOf(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "anyOf") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_anyOf(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "allOf") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_allOf(s, oit)))) {
            return FN_FAIL_FORWARD();
        }
    }
    else if (strcmp(keyword, "if") == 0) {
        if (!(FN_IS_OK(schema_keyword_handle_ifThenElse(s, oit)))) {
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
