
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

#ifndef SCHEMA_RUN_H
#define SCHEMA_RUN_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------
#include <regex.h>

BEGIN_DECL


fn_result schema_validate_run_handleKeyword_type(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    bool passed = false;
    carbon_field_type_e field_type;
    carbon_array_it_field_type(&field_type, ait);

    for (size_t i = 0; i < vector_length(&(s->data.type)); i++) {
        int *constraint = (int*)(vector_at(&(s->data.type), i));

        if (*constraint == NUMBER && carbon_field_type_is_number(field_type)) {
            passed = true;
        }
        else if (*constraint == STRING && carbon_field_type_is_string(field_type)) {
            passed = true;
        }
        else if (*constraint == BOOLEAN && carbon_field_type_is_boolean(field_type)) {
            passed = true;
        }
        else if (*constraint == BINARY && carbon_field_type_is_binary(field_type)) {
            passed = true;
        }
        else if (*constraint == ARRAY && carbon_field_type_is_array_or_subtype(field_type)) {
            passed = true;
        }
        else if (*constraint == COLUMN && carbon_field_type_is_column_or_subtype(field_type)) {
            passed = true;
        }
        else if (*constraint == OBJECT && carbon_field_type_is_object_or_subtype(field_type)) {
            passed = true;
        }
        else if (*constraint == _NULL && carbon_field_type_is_null(field_type)) {
            passed = true;
        }
        if (passed) {
            break;
        }
    }
    if (!(passed)) {
        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "failed \"type\" constraint");
    }
    if (carbon_array_it_has_next(ait)) {
        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "failed \"type\" constraint, expected atomar element"); 
    }
    return FN_OK();
}
    

fn_result schema_validate_run_handleKeyword_minimum(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    bool isnull;
    long double val;
    longDoubleFromAit(&isnull, &val, ait);
    if (!isnull) {
        if (val < s->data.minimum) {
            return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "\"minimum\" constraint not met");
        }
    }
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_maximum(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    bool isnull;
    long double val;
    longDoubleFromAit(&isnull, &val, ait);
    if (!isnull) {
        if (val > s->data.maximum) {
            return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "\"maximum\" constraint not met");
        }
    }
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_exclusiveMinimum(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    bool isnull;
    long double val;
    longDoubleFromAit(&isnull, &val, ait);
    if (!isnull) {
        if (val <= s->data.exclusiveMinimum) {
            return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "\"exclusiveMinimum\" constraint not met");
        }
    }
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_exclusiveMaximum(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);
    
    bool isnull;
    long double val;
    longDoubleFromAit(&isnull, &val, ait);
    if (!isnull) {
        if (val >= s->data.exclusiveMaximum) {
            return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "\"exclusiveMaximum\" constraint not met");
        }
    }
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_multipleOf(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    bool isnull;
    long double val;
    longDoubleFromAit(&isnull, &val, ait);
    if (!isnull) {
        if (!(fmod(val, s->data.multipleOf) == 0)) {
            return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "\"multipleOf\" constraint not met");
        }
    }
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_minLength(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    u64 strlen;
    const char *_ = carbon_array_it_string_value(&strlen, ait);
    UNUSED(_);

    if (s->data.minLength > strlen) {
        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "\"minLength\" constraint not met");
    }
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_maxLength(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    u64 strlen;
    const char *_ = carbon_array_it_string_value(&strlen, ait);
    UNUSED(_);

    if (s->data.maxLength < strlen) {
        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "\"maxLength\" constraint not met");
    }
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_pattern(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    regex_t regex;
    int reti;
    u64 strlen;
    const char *_str = carbon_array_it_string_value(&strlen, ait);
    char *str = strndup(_str, strlen);

    reti = regcomp(&regex, s->data.pattern, 0);
    if (reti) {
        return FN_FAIL(ERR_INITFAILED, "could not initiate \"pattern\" constraint. Not a POSIX regexp");
    }

    reti = regexec(&regex, str, 0, NULL, 0);
    if (!reti) {
        regfree(&regex);
        return FN_OK();
    }
    if (reti == REG_NOMATCH) {
        regfree(&regex);
        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "\"pattern\" constraint not met");
    }
    else {
        char msgbuf[100];
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        return FN_FAIL(ERR_ERRINTERNAL, msgbuf);
    }
}


fn_result schema_validate_run_handleKeyword_format(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    regex_t regex;
    int reti;
    const char *format;
    u64 strlen;
    const char *_str = carbon_array_it_string_value(&strlen, ait);
    const char *str = strndup(_str, strlen);
    
    switch(s->data.format) {
        case DATE :
            format = "[0-9]{4}-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])";
            break;
        case DATETIME :
            format = "[0-9]{4}-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1]) (2[0-3]|[01][0-9]):[0-5][0-9]";
            break;
        case URI :
            // https://tools.ietf.org/html/rfc3986#appendix-B
            format = "^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?";
            break;
        case EMAIL : 
            format = "^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$";
            break;
        case HOSTNAME :
            format = "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$";
            break;
        case IPV4 :
            format = "^([0-9]{1,3}\\.){3}[0-9]{1,3}(\\/([0-9]|[1-2][0-9]|3[0-2]))?$";
            break;
        case IPV6 :
            format = "(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}\%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))";
            break;
        case REGEX :
            format = str;
            reti = regcomp(&regex, format, 0);
            if (reti) {
                return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "keyword \"format\" failed. Not a POSIX regex");
            }
            return FN_OK();    
        default :
            return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "unknown \"format\" keyword");
    }

    reti = regcomp(&regex, format, 0);
    if (reti) {
        return FN_FAIL(ERR_ERRINTERNAL, "could not initiate \"format\" constraint");
    }
    reti = regexec(&regex, str, 0, NULL, 0);
    if (!reti) {
        regfree(&regex);
        return FN_OK();
    }
    if (reti == REG_NOMATCH) {
        regfree(&regex);
        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "\"format\" constraint not met");
    }
    else {
        char msgbuf[100];
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        return FN_FAIL(ERR_ERRINTERNAL, msgbuf);
    }
}


//TODO: implement
static inline fn_result schema_validate_run_handleKeyword_formatMinimum(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);
    UNUSED(s);
    UNUSED(ait);
    return FN_FAIL(ERR_NOTIMPL, "function formatMinimum not implemented yet");
}


//TODO: implement
static inline fn_result schema_validate_run_handleKeyword_formatMaximum(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);
    UNUSED(s);
    UNUSED(ait);
    return FN_FAIL(ERR_NOTIMPL, "function formatMaximum not implemented yet");
}


//TODO: implement
static inline fn_result schema_validate_run_handleKeyword_formatExclusiveMinimum(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);
    UNUSED(s);
    UNUSED(ait);
    return FN_FAIL(ERR_NOTIMPL, "function formatExclusiveMinimum not implemented yet");
}


//TODO: implement
static inline fn_result schema_validate_run_handleKeyword_formatExclusiveMaximum(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);
    UNUSED(s);
    UNUSED(ait);
    return FN_FAIL(ERR_NOTIMPL, "function formatExclusiveMaximum not implemented yet");
}
END_DECL


static inline fn_result schema_validate_run_handleKeyword_minItems(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);
    
    u64 numItems;
    carbon_array_it_length(&numItems, ait);
    
    if (numItems < s->data.minItems) {
        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "\"minItems\" constraint not met");
    }
    return FN_OK();
}


static inline fn_result schema_validate_run_handleKeyword_maxItems(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);
    
    u64 numItems;
    carbon_array_it_length(&numItems, ait);
    
    if (numItems > s->data.maxItems) {
        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "\"maxItems\" constraint not met");
    }
    return FN_OK();
}

// TODO: implement - question: is e.g. [[1,1],[2,2]] valid?
static inline fn_result schema_validate_run_handleKeyword_uniqueItems(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);
    
    UNUSED(s);
    UNUSED(ait);
    
    return FN_FAIL(ERR_NOTIMPL, "function exclusiveMaximum not implemented yet");

//    carbon_field_type_e field_type;
//
//    while (carbon_array_it_next(ait)) {
//        carbon_array_it_field_type(&field_type, &ait);
//
//        if (carbon_field_type_is_array_or_subtype(field_type)) {
//            carbon_array_it *sait = carbon_array_it_array_value(ait);
//            if (!(FN_IS_OK(schema_validate_run_handleKeyword_uniqueItems(s, sait)))) {
//                carbon_array_it_drop(sait);
//                return FN_FAIL_FORWARD();
//            }
//            carbon_array_it_drop(sait);
//        }
//
//        else if (carbon_field_type_is_object_or_subtype(field_type)) {
//            carbon_object_it *oit = carbon_array_it_object_value(ait);
//            if (!(FN_IS_OK(schema_validate_run_handleKeyword_uniqueItems_oit(s, oit)))) {
//                carbon_object_it_drop(oit);
//                return FN_FAIL_FORWARD();
//            }
//            carbon_object_it_drop(oit);
//        }
//
//        else if (carbon_field_type_is_number(field_type)) {
//            bool isnull;
//            long double value;
//            if (!(FN_IS_OK(longDoubleFromAit(&isnull, &value, ait)))) {
//                return FN_FAIL_FORWARD();
//            }
//        }
//
//        else if (carbon_field_type_is_string(field_type)) {
//
//            char *value
//
//            
//
//    }

}


static inline fn_result schema_validate_run_handleKeyword_additionalItems(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    if(s->applies.additionalItemsIsBool) {
        if(!(s->data.additionalItemsBool)) {
            return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "more items in array then defined in keyword \"items\" and keyword \"additionalItems\" set to false");
        }
        return FN_OK();
    }

    if(!(FN_IS_OK(schema_validate_run(s->data.additionalItems, ait)))) {
        return FN_FAIL_FORWARD();
    }
    return FN_OK();
}
    

static inline fn_result schema_validate_run_handleKeyword_items(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    carbon_array_it *sait = carbon_array_it_array_value(ait);

    if (s->applies.items_isObject) {
        schema *items = (schema*) vector_peek(&(s->data.items));

        while (carbon_array_it_next(sait)) {
            if(!(FN_IS_OK(schema_validate_run(items, sait)))) {
                carbon_array_it_drop(sait);
                //TODO: free schema memory
                return FN_FAIL_FORWARD();
            }
        }
        carbon_array_it_drop(sait);
        return FN_OK();
    }

    else {
        for (size_t i = 0; i < vector_length(&(s->data.items)); i++) {
            if (!(carbon_array_it_has_next(sait))) {
                carbon_array_it_drop(sait);
                return FN_OK();
            }

            schema *items = (schema*) vector_at(&(s->data.items),i);

            if (!(FN_IS_OK(schema_validate_run(items, sait)))) {
                carbon_array_it_drop(sait);
                //TODO: free schema memory!
                return FN_FAIL_FORWARD();
            }
        }

        if (carbon_array_it_has_next(sait) && (!(s->applies.has_additionalItems))) {
            carbon_array_it_drop(sait);
            //TODO: free schema memory!
            return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "more items in array then defined in keyword \"items\" and no keyword \"additionalItems\" given");
        }

        if (!(FN_IS_OK(schema_validate_run_handleKeyword_additionalItems(s, sait)))) {
            carbon_array_it_drop(sait);
            return FN_FAIL_FORWARD();
        }
        carbon_array_it_drop(sait);
        return FN_OK();
    }
}


static inline fn_result schema_validate_run_handleKeyword_contains(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);
   
    carbon_array_it *sait = carbon_array_it_array_value(ait);
    bool contains = false;

    while (carbon_array_it_next(sait)) {
        if (FN_IS_OK(schema_validate_run(s->data.contains, sait))) {
            contains = true;
        }
    }
    carbon_array_it_drop(sait);

    if (contains) {
        return FN_OK();
    }
    return FN_OK();
}



#endif
