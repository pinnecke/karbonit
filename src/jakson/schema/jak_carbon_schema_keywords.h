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

#ifndef JAK_CARBON_SCHEMA_KEYWORDS_H
#define JAK_CARBON_SCHEMA_KEYWORDS_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------
#include <jak_stdinc.h>
#include <jak_error.h>

JAK_BEGIN_DECL

static inline bool jak_carbon_schema_keywords_type(void *value, jak_carbon_schema_input *carbonFiles, jak_error *err) {
    JAK_UNUSED(err);

    const char **allowed_types;
    jak_u64 allowed_types_len;
    jak_u64 key_len;
    jak_carbon_field_type_e keyword_field_type;
    jak_carbon_object_it *schema_oit = (jak_carbon_object_it*)value;
    jak_carbon_object_it_prop_type(&keyword_field_type, schema_oit);

    // build array of allowed types
    // if only one allowed type passed as string -> create a size 1 const char* array for compatibility
    if (jak_carbon_field_type_is_string(keyword_field_type)) {
        allowed_types = (const char**)malloc(sizeof(char*));
        const char *val = jak_carbon_object_it_jak_string_value(&key_len, schema_oit);
        allowed_types[0] = strndup(val, key_len);
        allowed_types_len = 1;
    }

    // if array of keywords is given create size X const char* array
    else if (jak_carbon_field_type_is_array(keyword_field_type)) {
        jak_carbon_array_it *schema_keyword_it = jak_carbon_object_it_array_value(schema_oit);
        jak_carbon_array_it_length(&allowed_types_len, schema_keyword_it);
        allowed_types = (const char**)malloc(allowed_types_len*sizeof(char*));
        for (jak_u64 i = 0; i < allowed_types_len; i++) {
            jak_carbon_array_it_next(schema_keyword_it);
            allowed_types[i] = jak_carbon_array_it_jak_string_value(&(schema_keyword_it->field_access.it_field_len), schema_keyword_it);
        }
        jak_carbon_iterator_close(schema_keyword_it);
    }

    // only "type":["foo","bar"] or "type":"foo" allowed 
    else {
        //TODO: Error handling
        return false;
    }

    //verify all input files
    for (unsigned int i = 0; i < carbonFiles->num_files; i++){
        bool status = false;
        jak_carbon_field_type_e input_type;
        jak_carbon_array_it input_ait;
        jak_carbon_iterator_open(&input_ait, carbonFiles->files[i].data);
        // as the type keyword always defines the type of only one element, the carbonFile is only valid if EXACT one Element is passed. JSON File: [] or {} is NOT valid! 
        if(!(jak_carbon_array_it_next(&input_ait))){
            //TODO: Error handling
            carbonFiles->files[i].passed = status;
            continue;
        }

        jak_carbon_array_it_field_type(&input_type, &input_ait);
        for (jak_u64 j = 0; j < allowed_types_len; j++) {
            if (strcmp(allowed_types[j], "string") == 0 && jak_carbon_field_type_is_string(input_type)) {
                status = true;
            }
            else if (strcmp(allowed_types[j], "number") == 0 && jak_carbon_field_type_is_number(input_type)) {
                status = true;
            }
            else if (strcmp(allowed_types[j], "boolean") == 0 && jak_carbon_field_type_is_boolean(input_type)) {
                status = true;
            }
            else if (strcmp(allowed_types[j], "binary") == 0 && jak_carbon_field_type_is_binary(input_type)) {
                status = true;
            }
            else if (strcmp(allowed_types[j], "null") == 0 && jak_carbon_field_type_is_null(input_type)) {
                status = true;
            }
            else if (strcmp(allowed_types[j], "column") == 0 && jak_carbon_field_type_is_column(input_type)) {
                status = true;
            }
            else if (strcmp(allowed_types[j], "array") == 0 && jak_carbon_field_type_is_array(input_type)) {
                status = true;
            }
            else if (strcmp(allowed_types[j], "object") == 0 && jak_carbon_field_type_is_object(input_type)) {
                status = true;
            }
        }
        carbonFiles->files[i].passed = status;

        // if we encounter an unexpected additional item, file is invalid. This means there is either an issue with the schema syntax or an internal error.
        if (jak_carbon_array_it_has_next(&input_ait)) {
            //TODO: Error handling
            return false;
        }
        jak_carbon_iterator_close(&input_ait);
    }
    return true;
}


static inline bool jak_carbon_schema_keywords_properties(void *value, jak_carbon_schema_input *carbonFiles, jak_error *err) {
    JAK_UNUSED(value);
    JAK_UNUSED(carbonFiles);
    JAK_UNUSED(err);
    return true;
}


JAK_END_DECL

#endif
