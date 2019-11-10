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

#ifndef GENERATE_H
#define GENERATE_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------
#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/fn_result.h>
#include <jakson/schema.h>
#include <jakson/schema/generate.h>


BEGIN_DECL


fn_result schema_init(schema *s, const char* key_name);
fn_result longDoubleFromAit(bool *is_null, long double *val, carbon_array_it *ait);
fn_result longDoubleFromOit (bool *is_null, long double *val, carbon_object_it *oit);
fn_result schema_generate_handleKeyword(schema *s, const char *keyword, carbon_object_it *oit);
fn_result schema_generate_run(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_minItems(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_maxItems(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_uniqueItems(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_items(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_additionalItems(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_contains(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_enum(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_not(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_oneOf(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_anyOf(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_allOf(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_ifThenElse(schema *s, carbon_object_it *oit);
// TODO: allow for type to be a string
fn_result schema_generate_handleKeyword_type(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_minimum(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_maximum(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_exclusiveMinimum(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_exclusiveMaximum(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_multipleOf(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_properties(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_minProperties(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_maxProperties(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_required(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_patternProperties(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_additionalProperties(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_dependencies(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_propertyNames(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_patternRequired(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_minLength(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_maxLength(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_pattern(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_format(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_formatMinimum(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_formatMaximum(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_formatExclusiveMinimum(schema *s, carbon_object_it *oit);
fn_result schema_generate_handleKeyword_formatExclusiveMaximum(schema *s, carbon_object_it *oit);


END_DECL

#endif
