
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
#include "validate.h"
#include <jakson/stdinc.h>
#include <jakson/fn_result.h>
#include <jakson/schema.h>


fn_result schema_keyword_validate_generic_type(schema* s, carbon_array_it* ait)
{
    FN_FAIL_IF_NULL(s, ait);

    if (s->applies.has_type) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_type(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_enum) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_enum(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }
        
    if (s->applies.has_not) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_not(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_oneOf) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_oneOf(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_anyOf) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_anyOf(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_allOf) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_allOf(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_ifThenElse) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_ifThenElse(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }
    return FN_OK();
}


fn_result schema_validate_run(schema *s, carbon_array_it *ait)
{
    FN_FAIL_IF_NULL(s, ait);

    carbon_field_type_e field_type;
    carbon_array_it_field_type(&field_type, ait);

    // -------------- keywords for generic types / compound --------------
    if (!(FN_IS_OK(schema_keyword_validate_generic_type(s, ait)))) {
        return FN_FAIL_FORWARD();
    }

    // -------------- keywords for numbers --------------
    if (carbon_field_type_is_number(field_type)) {
        if (!(FN_IS_OK(schema_keyword_validate_numeric_type(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

        // -------------- keywords for strings --------------
    else if (carbon_field_type_is_string(field_type)) {
        if (!(FN_IS_OK(schema_keyword_validate_string_type(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

        // -------------- keywords for arrays --------------
    else if (carbon_field_type_is_array_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_keyword_validate_array_type(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

        // -------------- keywords for objects --------------
    else if (carbon_field_type_is_object_or_subtype(field_type)) {
        if (!(FN_IS_OK(schema_keyword_validate_object_type(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_minItems(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    u64 numItems;
    carbon_array_it *sait = carbon_array_it_array_value(ait);
    carbon_array_it_length(&numItems, sait);

    if (numItems < s->data.minItems) {
        carbon_array_it_drop(sait);
        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "\"minItems\" constraint not met");
    }
    carbon_array_it_drop(sait);
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_maxItems(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    u64 numItems;
    carbon_array_it *sait = carbon_array_it_array_value(ait);
    carbon_array_it_length(&numItems, sait);

    if (numItems > s->data.maxItems) {
        carbon_array_it_drop(sait);
        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "\"maxItems\" constraint not met");
    }
    carbon_array_it_drop(sait);
    return FN_OK();
}

//fn_result schema_validate_run_handleKeyword_uniqueItems_compColumns(carbon_column_it *cit1, carbon_column_it *cit2) {
//    FN_FAIL_IF_NULL(cit1, cit2);
//
//    carbon_field_type_e type1, type2;
//    u32 nvalues1, nvalues2;
//    carbon_column_it_values_info(&type1, &nvalues1, cit1);
//    carbon_column_it_values_info(&type2, &nvalues2, cit2);
//
//    if (type1 != type2) {
//        return FN_OK();
//    }
//
//    if (nvalues1 != nvalues2) {
//        return FN_OK();
//
//    if (carbon_field_type_is_signed(type1)) {
//        const i64 *vals1 = carbon_column_it_i64_values(&nvalues1, cit1);
//        const i64 *vals2 = carbon_column_it_i64_values(&nvalues2, cit2);
//
//        bool unique = false;
//        i64 val1, val2;
//        for (u32 i = 0; i < nvalues1; i++) {
//            val1 = vals1[i];
//            for (u32 j = 0; j < nvalues2; j++) {
//                val2 = vals2[i];
//                if (val1 == val2) {
//                    break;
//                }
//                if (j == (nvalues2 - 1)) {
//                    return FN_OK();
//                }
//            }
//        }
//    }
//        
//    else if (carbon_field_type_is_unsigned(type1)) {
//        const u64 *vals1 = carbon_column_it_u64_values(&nvalues1, cit1);
//        const u64 *vals2 = carbon_column_it_u64_values(&nvalues2, cit2);
//
//        bool unique = false;
//        u64 val1, val2;
//        for (u32 i = 0; i < nvalues1; i++) {
//            val1 = vals1[i];
//            for (u32 j = 0; j < nvalues2; j++) {
//                val2 = vals2[i];
//                if (val1 == val2) {
//                    break;
//                }
//                if (j == (nvalues2 - 1)) {
//                    return FN_OK();
//                }
//            }
//        }
//    }
//
//    else if (carbon_field_type_is_floating(type1)) {
//        const float *vals1 = carbon_column_it_float_values(&nvalues1, cit1);
//        const float *vals2 = carbon_column_it_float_values(&nvalues2, cit2);
//
//        bool unique = false;
//        float val1, val2;
//        for (u32 i = 0; i < nvalues1; i++) {
//            val1 = vals1[i];
//            for (u32 j = 0; j < nvalues2; j++) {
//                val2 = vals2[i];
//                if (val1 == val2) {
//                    break;
//                }
//                if (j == (nvalues2 - 1)) {
//                    return FN_OK();
//                }
//            }
//        }
//    }
//    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "keyword \"uniqueItems\" not met");
//}
//
//
//fn_result schema_validate_run_handleKeyword_uniqueItems_compObjects(carbon_object_it* oit1, carbon_object_it *oit2) {
//    FN_FAIL_IF_NULL(oit1, oit2);
//
//    u64 len1, len2;
//    carbon_object_it_length(&len1, oit1);
//    carbon_object_it_length(&len2, oit2);
//
//    if (len1 != len2) {
//        return FN_OK();
//    }
//
//    carbon_field_type_e type1;
//    carbon_field_type_e type2;
//
//    while (carbon_object_it_next(oit1)) {
//        carbon_object_it_field_type(&type1, oit1);
//        u64 keylen;
//        bool is_null;
//        field_access data;
//        const char *_key = carbon_object_it_prop_name(&keylen, oit1);
//        const char *key = strndup(_key, keylen);
//        carbon_object_it_data_from_key(&is_null, &type2, &data, key, oit2);
//        free(key); 
//
//        if (type1 != type2 || is_null) {
//            return FN_OK();
//        }
//
//        if (carbon_field_type_is_signed_atom(field_type)) {
//            i64 val1, val2;
//            carbon_int_field_access_signed_value(NULL, &val1, &data, NULL);
//            carbon_int_field_access_signed_value(NULL, &val2, &data, NULL);
//            
//            if (val1 != val2) {
//                return FN_OK();
//            }
//        }
//
//        else if (carbon_field_type_is_unsigned_atom(field_type)) {
//            u64 val1, val2;
//            carbon_int_field_access_unsigned_value(NULL, &val1, &data, NULL);
//            carbon_int_field_access_unsigned_value(NULL, &val2, &data, NULL);
//            
//            if (val1 != val2) {
//                return FN_OK();
//            }
//        }
//
//        else if (carbon_field_type_is_float_atom(field_type)) {
//            float val1, val2;
//            carbon_int_field_access_float_value(NULL, &val1, &data, NULL);
//            carbon_int_field_access_float_value(NULL, &val2, &data, NULL);
//            
//            if (val1 != val2) {
//                return FN_OK();
//            }
//        }
//
//        else if (carbon_field_type_is_string(field_type)) {
//            const char *_str1, _str2;
//            carbon_int_field_access_string_value(
//
//        else if (carbon_field_type_is_null(field_type)) {
//            continue;
//        }
//
//        else if (carbon_field_type_is_boolean_atom(field_type)) {
//            if (val != val) {
//                return FN_OK();
//            }
//        }
//    
//             
//
//
//
//
//
//
//
//
//
//
//    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "keyword \"uniqueItems\" not met");
//}
//
//
//fn_result schema_validate_run_handleKeyword_uniqueItems_compArrays(carbon_array_it* ait1, carbon_array_it *ait2) {
//    FN_FAIL_IF_NULL(ait1, ait2);
//
//    u64 len1, len2;
//    carbon_array_it_length(&len1, ait1);
//    carbon_array_it_length(&len2, ait2);
//
//    if (len1 != len2) {
//        return FN_OK();
//    }
//
//    carbon_field_type_e type1;
//    carbon_field_type_e type2;
//
//    while (carbon_array_it_next(ait1)) {
//        carbon_array_it_field_type(&type1, ait);
//        u64 pos = 0;
//        carbon_array_it_rewind(ait2);
//        while (carbon_array_it_next(ait2)) {
//            carbon_array_it_field_type(&type2, ait2);
//            
//            if (type1 != type2) {
//                continue;
//            }
//
//            if (carbon_field_type_is_signed_atom(type1)) {
//                i64 val1, val2;
//                carbon_array_it_signed_value(&val1, ait);
//                carbon_array_it_signed_value(&val2, ait);
//                if (val1 == val2) {
//                    break;
//                }
//            }
//                
//            else if (carbon_field_type_is_unsigned_atom(type1)) {
//                u64 val1, val2;
//                carbon_array_it_unsigned_value(&val1, ait);
//                carbon_array_it_unsigned_value(&val2, ait);
//                if (val1 == val2) {
//                    break;
//                }
//            }
//
//            else if (carbon_field_type_is_float_atom(type1)) {
//                float val1, val2;
//                carbon_array_it_float_value(&val1, ait);
//                carbon_array_it_float_value(&val2, ait);
//                if (val1 == val2) {
//                    break;
//                }
//            }
//
//            else if (carbon_field_type_is_bool_atom(type1)) {
//                bool val1, val2;
//                carbon_array_it_bool_value(&val1, ait);
//                carbon_array_it_bool_value(&val2, ait);
//                if (val1 == val2) {
//                    break;
//                }
//            }
//
//            else if (carbon_field_type_is_string(type1)) {
//                u64 strlen1, strlen2;
//                char *_val1 = carbon_array_it_string_value(&strlen1, ait);
//                char *_val2 = carbon_array_it_string_value(&strlen2, ait);
//                if (strlen1 != strlen2) {
//                    continue;
//                }
//                char *val1 = strndup(_val1, strlen1);
//                char *val2 = strndup(_val2, strlen2);
//                if (!(strcmp(val1, val2))) {
//                    free(val1);
//                    free(val2);
//                    break;
//                }
//                free(val1);
//                free(val2);
//            }
//
//            else if (carbon_field_type_is_null(type1)) {
//                break;
//            }
//
//            else if (carbon_field_type_is_array_or_subtype(type1)) {
//                carbon_array_it *sait1 = ait1;
//                carbon_array_it *sait2 = ait2;
//                if (!(FN_IS_OK(schema_validate_run_handleKeyword_uniqueItems_compArrays(sait1, sait2)))) {
//                    carbon_array_it_drop(sait1);
//                    carbon_array_it_drop(sait2);
//                    break;
//                }
//                carbon_array_it_drop(sait);
//                carbon_array_it_drop(sait2);
//            }
//
//            else if (carbon_field_type_is_column_or_subtype(type1)) {
//                carbon_array_it *cit1 = ait1;
//                carbon_array_it *cit2 = ait2;
//                if (!(FN_IS_OK(schema_validate_run_handleKeyword_uniqueItems_compColumns(cit1, cit2)))) {
//                    carbon_array_it_drop(cit1);
//                    carbon_array_it_drop(cit2);
//                    break;
//                }
//                carbon_array_it_drop(cit);
//                carbon_array_it_drop(cit2);
//            }
//
//            else if (carbon_field_type_is_object_or_subtype(type1)) {
//                carbon_array_it *oit1 = oit1;
//                carbon_array_it *oit2 = oit2;
//                if (!(FN_IS_OK(schema_validate_run_handleKeyword_uniqueItems_compObjects(oit1, oit2)))) {
//                    carbon_object_it_drop(oit1);
//                    carbon_object_it_drop(oit2);
//                    break;
//                }
//                carbon_object_it_drop(oit1);
//                carbon_object_it_drop(oit2);
//            }
//
//            if (pos == (len1 - 1)) {
//                return FN_OK();
//            }
//            pos++;
//        }
//    }
//    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "keyword \"uniqueItems\" not met");
//}
//
//
fn_result schema_validate_run_handleKeyword_uniqueItems(schema *s, carbon_array_it *ait) {
    UNUSED(s);
    UNUSED(ait);
    return FN_OK();
}

//    FN_FAIL_IF_NULL(s, ait);
//
//    if (!(&(s->data.uniqueItems))) {
//        return FN_OK();
//    }
//
//    u64 array_len;
//    carbon_array_it_length(&array_len, ait);
//    if (array_len < 2) {
//        return FN_OK();
//    }
//
//    carbon_array_it clone;
//    carbon_array_it_clone(&clone, ait);
//    carbon_field_type_e type1; 
//    carbon_field_type_e type2; 
//    
//    carbon_array_it_next(&clone);
//
//    while (carbon_array_it_next(ait)) {
//        carbon_array_it_field_type(&type1, ait);
//        
//        while (carbon_array_it_next(&clone)) {
//            carbon_array_it saved_pos;
//            carbon_array_it_copy(&saved_pos, &clone);
//            carbon_array_it_field_type(&type2, &clone);
//            
//            if (type1 != type2) {
//                continue;
//            }
//            
//            if (carbon_field_type_is_null(type1)) {
//                carbon_array_it_drop(&clone);
//                return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "keyword \"uniqueItems\" not met");
//            }
//            
//            else if (carbon_field_type_is_signed_atom(type1)) {
//                i64 val1, val2;
//                carbon_array_it_signed_value(&val1, ait);
//                carbon_array_it_signed_value(&val2, &clone);
//                if (val1 == val2) {
//                    carbon_array_it_drop(&clone);
//                    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "keyword \"uniqueItems\" not met");
//                }
//            }
//
//            else if (carbon_field_type_is_unsigned_atom(type1)) {
//                u64 val1, val2;
//                carbon_array_it_unsigned_value(&val1, ait);
//                carbon_array_it_unsigned_value(&val2, &clone);
//                if (val1 == val2) {
//                    carbon_array_it_drop(&clone);
//                    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "keyword \"uniqueItems\" not met");
//                }
//            }
//
//            else if (carbon_field_type_is_floating_atom(type1)) {
//                float val1, val2;
//                carbon_array_it_float_value(&val1, ait);
//                carbon_array_it_float_value(&val2, &clone);
//                if (val1 == val2) {
//                    carbon_array_it_drop(&clone);
//                    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "keyword \"uniqueItems\" not met");
//                }
//            }
//
//            else if (carbon_field_type_is_boolean_atom(type1)) {
//                bool val1, val2;
//                carbon_array_it_bool_value(&val1, ait);
//                carbon_array_it_bool_value(&val2, &clone);
//                if (val1 == val2) {
//                    carbon_array_it_drop(&clone);
//                    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "keyword \"uniqueItems\" not met");
//                }
//            }
//
//            else if (carbon_field_type_is_string(type1)) {
//                u64 strlen1, strlen2;
//                const char *_val1 = carbon_array_it_string_value(val1, ait);
//                const char *_val2 = carbon_array_it_string_value(val2, &clone);
//                if (strlen1 != strlen2) {
//                    continue;
//                }
//                const char *val1 = strndup(_val1, strlen1);
//                const char *val2 = strndup(_val2, strlen2);
//                if (!(strcmp(val1, val2))) {
//                    free(val1);
//                    free(val2);
//                    carbon_array_it_drop(&clone);
//                    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "keyword \"uniqueItems\" not met");
//                }
//            }
//
//            else if (carbon_field_type_is_binary(type1)) {
//                carbon_array_it_drop(&clone);
//                return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "keyword \"uniqueItems\" not met. Cannot compare binary types");
//            }
//
//            else if (carbon_field_type_is_array_or_subtype(type1)) {
//                carbon_array_it *sait = carbon_array_it_array_value(ait);
//                carbon_array_it *sclone = carbon_array_it_array_value(&clone);
//                if (!(FN_IS_OK(schema_validate_run_handleKeyword_uniqueItems_compArrays(sait, sclone)))) {
//                    carbon_array_it_drop(sait);
//                    carbon_array_it_drop(sclone);
//                    carbon_array_it_drop(&clone);
//                    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "keyword \"uniqueItems\" not met");
//                }
//                carbon_array_it_drop(sait);
//                carbon_array_it_drop(sclone);
//            }
//
//            else if (carbon_field_type_is_column_or_subtype(type1)) {
//                carbon_column_it *cit = carbon_array_it_column_value(ait);
//                carbon_column_it *cclone = carbon_array_it_column_value(&clone);
//                if (!(FN_IS_OK(schema_validate_run_handleKeyword_uniqueItems_compColumns(cit, cclone)))) {
//                    carbon_column_it_drop(cit);
//                    carbon_column_it_drop(sclone);
//                    carbon_array_it_drop(&clone);
//                    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "keyword \"uniqueItems\" not met");
//                }
//                carbon_column_it_drop(cit);
//                carbon_column_it_drop(sclone);
//            }
//
//            else if (carbon_field_type_is_object_or_subtype(type1)) {
//                carbon_object_it *oit = carbon_array_it_object_value(ait);
//                carbon_object_it *oclone = carbon_array_it_object_value(&clone);
//                if (!(FN_IS_OK(schema_validate_run_handleKeyword_uniqueItems_compObjects(oit, oclone)))) {
//                    carbon_column_it_drop(cit);
//                    carbon_column_it_drop(sclone);
//                    carbon_array_it_drop(&clone);
//                    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "keyword \"uniqueItems\" not met");
//                }
//                carbon_column_it_drop(cit);
//                carbon_column_it_drop(sclone);
//            }
//            //TODO
//            &clone 
//            carbon_array_it_copy(&saved_pos, &clone);
//        }
//    }
//    carbon_array_it_drop(&clone);
//    return FN_OK();
//}

                
fn_result schema_validate_run_handleKeyword_additionalItems(schema *s, carbon_array_it *ait) {
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


fn_result schema_validate_run_handleKeyword_items(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    carbon_array_it *sait = carbon_array_it_array_value(ait);

    if (s->applies.items_isObject) {
        schema *items = (schema*) vector_peek(&(s->data.items));

        while (carbon_array_it_next(sait)) {
            if(!(FN_IS_OK(schema_validate_run(items, sait)))) {
                carbon_array_it_drop(sait);
                return FN_FAIL_FORWARD();
            }
        }
        carbon_array_it_drop(sait);
        return FN_OK();
    }

    else {
        for (size_t i = 0; i < vector_length(&(s->data.items)); i++) {
            if (!(carbon_array_it_next(sait))) {
                carbon_array_it_drop(sait);
                return FN_OK();
            }

            schema *items = (schema*) vector_at(&(s->data.items),i);

            if (!(FN_IS_OK(schema_validate_run(items, sait)))) {
                carbon_array_it_drop(sait);
                return FN_FAIL_FORWARD();
            }
        }

        if (carbon_array_it_has_next(sait)) {
            if (!(s->applies.has_additionalItems)) {
                carbon_array_it_drop(sait);
                return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "more items in array then defined in keyword \"items\" and no keyword \"additionalItems\" given");
            }

            if (!(FN_IS_OK(schema_validate_run_handleKeyword_additionalItems(s, sait)))) {
                carbon_array_it_drop(sait);
                return FN_FAIL_FORWARD();
            }
        }
        carbon_array_it_drop(sait);
        return FN_OK();
    }
}


fn_result schema_validate_run_handleKeyword_contains(schema *s, carbon_array_it *ait) {
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
    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "constraint \"contains\" failed");
}


fn_result schema_keyword_validate_array_type(schema *s, carbon_array_it *ait) 
{
    FN_FAIL_IF_NULL(s, ait);

    if (s->applies.has_minItems) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_minItems(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_maxItems) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_maxItems(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_uniqueItems) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_uniqueItems(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_items) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_items(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_contains) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_contains(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_type(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    bool passed = false;
    carbon_field_type_e field_type;
    carbon_array_it_field_type(&field_type, ait);

    for (size_t i = 0; i < vector_length(&(s->data.type)); i++) {
        int *constraint = (int*)(vector_at(&(s->data.type), i));

        if (*constraint == NUMBER && carbon_field_type_is_number_atom(field_type)) {
            passed = true;
        }
        else if (*constraint == STRING && carbon_field_type_is_string(field_type)) {
            passed = true;
        }
        else if (*constraint == BOOLEAN && carbon_field_type_is_boolean_atom(field_type)) {
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

    return FN_OK();
}


// TODO: implement, deeply equal function needed
fn_result schema_validate_run_handleKeyword_enum(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    UNUSED(s);
    UNUSED(ait);

    return FN_FAIL(ERR_NOTIMPL, "\"enum\" keyword not implemented yet");
}


fn_result schema_validate_run_handleKeyword_not(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    carbon_array_it_rewind(ait);

    if(!(FN_IS_OK(schema_validate_run(s->data._not, ait)))) {
        return FN_OK();
    }

    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "constraint \"not\" was not met");
}


fn_result schema_validate_run_handleKeyword_oneOf(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    bool valid = false;
    for (size_t i = 0; i < vector_length(&(s->data.oneOf)); i++) {
        carbon_array_it_rewind(ait);
        schema *oneOf = (schema*) vector_at(&(s->data.oneOf), i);

        if (FN_IS_OK(schema_validate_run(oneOf, ait))) {
            if (valid) {
                return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "constraint \"oneOf\" was not met: mult. schema match");
            }
            valid = true;
        }
    }
    if (valid) {
        return FN_OK();
    }
    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "constraint \"oneOf\" was not met: no schema match");
}


fn_result schema_validate_run_handleKeyword_anyOf(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    for (size_t i = 0; i < vector_length(&(s->data.anyOf)); i++) {
        carbon_array_it_rewind(ait);
        schema *anyOf = (schema*) vector_at(&(s->data.anyOf), i);

        if(FN_IS_OK(schema_validate_run(anyOf, ait))) {
            return FN_OK();
        }
    }
    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "constraint \"anyOf\" was not met: no schema match");
}


fn_result schema_validate_run_handleKeyword_allOf(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    for (size_t i = 0; i < vector_length(&(s->data.allOf)); i++) {
        carbon_array_it_rewind(ait);
        schema *allOf = (schema*) vector_at(&(s->data.allOf), i);

        if(!(FN_IS_OK(schema_validate_run(allOf, ait)))) {
            return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "constraint \"allOf\" was not met: at least one schema did not match");
        }
    }
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_ifThenElse(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    schema* _if = (schema*) vector_at(&(s->data.ifThenElse), 0);
    schema *then = (schema*) vector_at(&(s->data.ifThenElse), 1);
    schema *_else = (schema*) vector_at(&(s->data.ifThenElse), 2);

    if (FN_IS_OK(schema_validate_run(_if, ait))) {
        if (FN_IS_OK(schema_validate_run(then, ait))) {
            return FN_OK();
        }
        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "contraint \"if/then/else\" was not met.");
    }
    
    if (FN_IS_OK(schema_validate_run(_else, ait))) {
        return FN_OK();
    }
    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "contraint \"if/then/else\" was not met.");
}


fn_result schema_keyword_validate_numeric_type(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    if (s->applies.has_minimum) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_minimum(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_maximum) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_maximum(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_exclusiveMinimum) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_exclusiveMinimum(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_exclusiveMaximum) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_exclusiveMaximum(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_multipleOf) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_multipleOf(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
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


fn_result schema_keyword_validate_object_type(schema* s, carbon_array_it* ait)
{
    FN_FAIL_IF_NULL(s, ait);

    if (s->applies.has_minProperties) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_minProperties(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_maxProperties) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_maxProperties(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_required) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_required(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_properties) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_properties(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_patternProperties) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_patternProperties(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_additionalProperties) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_additionalProperties(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_dependencies) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_dependencies(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_propertyNames) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_propertyNames(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_patternRequired) {
        if (!(FN_IS_OK(schema_validate_run_handleKeyword_patternRequired(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_minProperties(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    u64 len;
    carbon_object_it *oit = carbon_array_it_object_value(ait);
    carbon_object_it_length(&len, oit);
    carbon_object_it_drop(oit);

    if (len < s->data.minProperties) {
        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "constraint \"minProperties\" failed");
    }
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_maxProperties(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    u64 len;
    carbon_object_it *oit = carbon_array_it_object_value(ait);
    carbon_object_it_length(&len, oit);
    carbon_object_it_drop(oit);

    if (len > s->data.maxProperties) {
        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "constraint \"maxProperties\" failed");
    }
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_required(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    carbon_object_it *oit = carbon_array_it_object_value(ait);
    for (u64 i = 0; i < vector_length(&(s->data.required)); i++) {
        const char *key = (const char*) vector_at(&(s->data.required), i);
        if (!(carbon_object_it_has_key(key, oit))) {
            carbon_object_it_drop(oit);
            return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "constraint \"required\" failed");
        }
    }
    carbon_object_it_drop(oit);
    return FN_OK();
}


// TODO: implement
// TODO: need function to create carbon file from object it
fn_result schema_validate_run_handleKeyword_properties(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    UNUSED(s);
    UNUSED(ait);
//    carbon_object_it *oit = carbon_array_it_object_value(ait);
//    for (size_t i = 0; i < vector_length(&(s->data.properties)); i++) {
//        schema *item = (schema*) vector_at(&(s->data.properties), i);
//        while (carbon_object_it_next(oit)) {
//            u64 keylen;
//            const char *_key = carbon_object_it_prop_name(&keylen, oit);
//            const char *key = strncmp(_key, keylen);
//            if (!(strcmp(key, s->key_name))) {
//                if (!(FN_IS_OK

    return FN_FAIL(ERR_NOTIMPL, "\"properties\" keyword not implemented yet");
}


// TODO: implement
fn_result schema_validate_run_handleKeyword_patternProperties(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    UNUSED(s);
    UNUSED(ait);

    return FN_FAIL(ERR_NOTIMPL, "\"patternProperties\" keyword not implemented yet");
}


// TODO: implement
fn_result schema_validate_run_handleKeyword_additionalProperties(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    UNUSED(s);
    UNUSED(ait);

    return FN_FAIL(ERR_NOTIMPL, "\"additionalProperties\" keyword not implemented yet");
}


fn_result schema_validate_run_handleKeyword_dependencies(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    carbon_object_it *oit = carbon_array_it_object_value(ait);

    if (!(s->applies.dependencies_isObject)) {
        for (size_t i = 0; i < vector_length(&(s->data.dependencies)); i++) {
            vector *item = (vector*) vector_at(&(s->data.dependencies), i);
            const char *key = (const char*) vector_at(item, 0);
            if (carbon_object_it_has_key(key, oit)) {
                for (size_t j = 1; j < vector_length(item); j++) {
                    const char *str = (const char*) vector_at(item, j);
                    if (!(carbon_object_it_has_key(str, oit))) {
                        carbon_object_it_drop(oit);
                        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "constraint \"dependencies\" was not met");
                    }
                }
            }
        }
    }
    else {
        for (size_t i = 0; i < vector_length(&(s->data.dependencies)); i++) {
            schema *dependency = (schema*) vector_at(&(s->data.dependencies), i);
            const char *key = dependency->key_name;
            if (carbon_object_it_has_key(key, oit)) {
                if (!(FN_IS_OK(schema_validate_run(dependency, ait)))) {
                    carbon_object_it_drop(oit);
                    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "constraint \"dependencies\" was not met");
                }
            }
        }
    }
    carbon_object_it_drop(oit);
    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_propertyNames(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    carbon_object_it *oit = carbon_array_it_object_value(ait);
    carbon_new context;
    carbon record;
    carbon_insert *ins;
    carbon_array_it sait;

    ins = carbon_create_begin(&context, &record, CARBON_KEY_NOKEY, CARBON_KEEP);

    while (carbon_object_it_next(oit)) {
        u64 keylen;
        const char *_key = carbon_object_it_prop_name(&keylen, oit);
        char *key = strndup(_key, keylen);
        carbon_insert_string(ins, key);
        free(key);
    }
    carbon_create_end(&context);

    carbon_iterator_open(&sait, &record);
    while (carbon_array_it_next(&sait)) {
        if (!(FN_IS_OK(schema_validate_run(s->data.propertyNames, &sait)))) {
            carbon_array_it_drop(&sait);
            carbon_object_it_drop(oit);
            carbon_drop(&record);
            return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "constraint \"propertyNames\" was not met");
        }
    }
    carbon_array_it_drop(&sait);
    carbon_object_it_drop(oit);
    carbon_drop(&record);

    return FN_OK();
}


fn_result schema_validate_run_handleKeyword_patternRequired(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    int reti;
    u64 keylen;
    carbon_object_it *oit = carbon_array_it_object_value(ait);

    for (size_t i = 0; i < vector_length(&(s->data.patternRequired)); i++) {
        carbon_object_it_rewind(oit);

        bool exists = false;
        regex_t regex;
        reti = regcomp(&regex, (const char*) vector_at(&(s->data.patternRequired), i), 0);
        if (reti) {
            carbon_object_it_drop(oit);
            return FN_FAIL(ERR_INITFAILED, "could not initiate \"patternRequired\" constraint. Not a POSIX regexp");
        }

        while (carbon_object_it_next(oit)) {
            const char *_key = carbon_object_it_prop_name(&keylen, oit);
            char *key = strndup(_key, keylen);

            reti = regexec(&regex, key, 0, NULL, 0);
            free(key);
            if (!reti) {
                exists = true;
                break;
            }
        }
        regfree(&regex);
        if (!exists) {
            carbon_object_it_drop(oit);
            return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "constraint \"patternRequired\" was not met");
        }
    }
    carbon_object_it_drop(oit);
    return FN_OK();
}


fn_result schema_keyword_validate_string_type(schema *s, carbon_array_it *ait)
{
    FN_FAIL_IF_NULL(s, ait);

    if (s->applies.has_minLength) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_minLength(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_maxLength) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_maxLength(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_pattern) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_pattern(s, ait)))) {
            return FN_FAIL_FORWARD();
        }
    }

    if (s->applies.has_format) {
        if(!(FN_IS_OK(schema_validate_run_handleKeyword_format(s, ait)))) {
            return FN_FAIL_FORWARD();
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

    reti = regcomp(&regex, s->data.pattern, REG_EXTENDED);
    if (reti) {
        return FN_FAIL(ERR_INITFAILED, "could not initiate \"pattern\" constraint. Not a POSIX regexp");
    }

    reti = regexec(&regex, str, 0, NULL, 0);
    free(str);
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

fn_result schema_validate_regex(u8 *format, const char *str) {
    FN_FAIL_IF_NULL(format);

    const char *regex_in;
    regex_t regex;
    int reti;
    switch(*format) {
        // TODO: there is no POSIX compatible regex for email validation, as email val. needs lookahead. Maybe reimplement as PCRE expr?
        case DATE :
            regex_in = "[0-9]{4}-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])";
            break;
        case DATETIME :
            regex_in = "[0-9]{4}-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1]) (2[0-3]|[01][0-9]):[0-5][0-9]";
            break;
        case URI :
            regex_in = "\\w+:(\\/?\\/?)[^\\s]+";
            break;
        case HOSTNAME :
            regex_in = "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$";
            break;
        case IPV4 :
            regex_in = "^([0-9]{1,3}\\.){3}[0-9]{1,3}(\\/([0-9]|[1-2][0-9]|3[0-2]))?$";
            break;
        case IPV6 :
            regex_in = "(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}\%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))";
            break;
        case REGEX :
            if (str) {
                regex_in = str;
                reti = regcomp(&regex, regex_in, REG_EXTENDED);
                if (reti) {
                    return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "regcomp failed on custom regex. Not a POSIX regex");
                }
                return FN_OK();
            }
            return FN_FAIL(ERR_BADTYPE, "regcomp on custom regex expects a regex. got NULL");
        default :
            return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "unknown format in regex comparison");
    }
    reti = regcomp(&regex, regex_in, REG_EXTENDED);
    if (reti) {
        return FN_FAIL(ERR_ERRINTERNAL, "could not initiate format constraint");
    }
    reti = regexec(&regex, str, 0, NULL, 0);
    if (!reti) {
        regfree(&regex);
        return FN_OK();
    }
    if (reti == REG_NOMATCH) {
        regfree(&regex);
        return FN_FAIL(ERR_SCHEMA_VALIDATION_FAILED, "regex validation failed");
    }
    else {
        char msgbuf[100];
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        return FN_FAIL(ERR_ERRINTERNAL, msgbuf);
    }
}


fn_result schema_validate_run_handleKeyword_format(schema *s, carbon_array_it *ait) {
    FN_FAIL_IF_NULL(s, ait);

    u64 strlen;
    const char *_str = carbon_array_it_string_value(&strlen, ait);
    char *str = strndup(_str, strlen);

    if (!(FN_IS_OK(schema_validate_regex(&(s->data.format), str)))) {
        free(str);
        return FN_FAIL_FORWARD();
    }
    free(str);
    return FN_OK();
}
