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

BEGIN_DECL

typedef struct schema_content {
    const char *key;
    void *value;
} schema_content;

typedef struct schema {
    schema_content *content;
    u64 content_size;
    carbon *fileToVal;
} schema;

bool schema_validate(carbon *schemaCarbon, carbon *fileToVal);
bool schema_createSchema(schema *schema, carbon_object_it *oit);
bool schema_handleKeys(schema *schema);
u64  schema_getSchemaSize(carbon_object_it *oit);

END_DECL

#endif
