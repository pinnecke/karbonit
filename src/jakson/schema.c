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

bool schema_validate(carbon *schemaCarbon, carbon *fileToVal) {
    ERROR_IF_NULL(schemaCarbon);
    ERROR_IF_NULL(fileToVal);

    carbon_array_it it;
    carbon_field_type_e field_type;

    carbon_iterator_open(&it, schemaCarbon);
    carbon_array_it_next(&it);
    carbon_array_it_field_type(&field_type, &it);

    // a schema always has to be an object.
    if (!(carbon_field_type_is_object_or_subtype(field_type))){
        ERROR_WDETAILS(&it.err, ERR_BADTYPE, "schema has to be an object.");
        carbon_iterator_close(&it);
        //TODO: cleanup?
        return false;
    }

    schema schema;
    schema.fileToVal = fileToVal;

    if(!(schema_createSchema(&schema, carbon_array_it_object_value(&it)))) {
        //TODO: error handling
        return false;
    }
    carbon_iterator_close(&it);

    return true;   
}

bool schema_createSchema(schema *schema, carbon_object_it *oit) {

    // get schema size to avoid unnecessary reallocs
    u64 schemaSize = schema_getSchemaSize(oit);
    schema_content *content = malloc(schemaSize * sizeof(schema_content*));
    schema->content = content;
    schema->content_size = schemaSize;
    
    u64 pos = 0;
    while(carbon_object_it_next(oit)) {
        u64 key_len;
        const char *prop_name = carbon_object_it_prop_name(&key_len,oit);
        content[pos].key = strndup(prop_name,key_len);
        content[pos].value = oit; 
    }
    if(!(schema_handleKeys(schema))) {
        //TODO: error handling
        free(content);
        return false;
    }
    free(content);
    return true;
}


bool schema_handleKeys(schema *schema) {
    bool status = true;
    for (unsigned int i = 0; i < schema->content_size; i++) {
        if (strcmp(schema->content[i].key, "type")==0) {
            status = schema_keywords_type(schema->content[i].value, carbonFiles, &(schema->err));
        }
        else if (strcmp(schema->content[i].key, "properties")==0) {
                status = schema_keywords_properties(schema->content[i].value, carbonFiles, &(schema->err));
        }
        else {
            //TODO: error handling
            return false;
        }
        if (status != true) {
            //TODO: error handling
            return false;
        }
    }
    return true;
}


unsigned int schema_getSchemaSize(carbon_object_it *oit) {
    unsigned int size = 0;
    while (carbon_object_it_next(oit)) {
        size++;
    }
    carbon_object_it_rewind(oit);
    return size;
}
