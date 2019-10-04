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

enum constraint_type {
    MEMBER,
    INTEGER,
    STRING,
    BOOLEAN,
    ARRAY,
    OBJECT,
    NULL
}

struct property_constraint {

    char *key_name;

    struct {
        u8 has_type_constraint : 1;
        u8 has_min : 1;
        u8 has_object_pro_sub : 1;
    } applied;
    
    constraint_type type;

    float min;

    struct property_constraint sub;

}

struct properties_constraint {
    
    jak_vector ofType(struct property_constraint) properties;

}

fn_result property_constraint_create(struct property_constraint *p, const char *key_name) {
    
    p->key_name = strdup(key_name);
    p->applied.has_type_costraint = false;
    p->applied.has_min = false;

}

fn_result property_constraint_set_type(struct property_constraint *p, enum constraint_type type) {
    p->type = type;
}

fn_result property_constraint_set_min(struct property_constraint *p, float min) {
    p->min = min;
}

fn_result properties_constaint_create(struct properties_constraint *p) {
    jak_vector_create(&p->properties, sizeof(struct property_constraint), ...);
}

fn_result properties_constaint_add_entry(struct properties_constraint *p, struct property_constraint *line) {
    jak_vector_get_new(p, line);
}

fn_result properties_constaint_validate(jak_carbon *file, struct properties_constraint *p) {
    
    carbon_iterator_open(it, file)
    carbon_array_it_next)(

}



END_DECL

#endif
