
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

#ifndef VALIDATE_H
#define VALIDATE_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------
#include <jakson/schema.h>
#include <jakson/schema/generate.h>
#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/fn_result.h>
#include <regex.h>
#include "jakson/carbon/insert.h"


BEGIN_DECL


fn_result schema_keyword_validate_generic_type(schema* s, carbon_array_it* ait);
fn_result schema_validate_run(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_minItems(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_maxItems(schema *s, carbon_array_it *ait);
// TODO: implement - question: is e.g. [[1,1],[2,2]] valid?
fn_result schema_validate_run_handleKeyword_uniqueItems(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_additionalItems(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_items(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_contains(schema *s, carbon_array_it *ait);
fn_result schema_keyword_validate_array_type(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_type(schema *s, carbon_array_it *ait);
// TODO: implement, deeply equal function needed
fn_result schema_validate_run_handleKeyword_enum(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_not(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_oneOf(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_anyOf(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_allOf(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_ifThenElse(schema *s, carbon_array_it *ait);
fn_result schema_keyword_validate_numeric_type(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_minimum(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_maximum(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_exclusiveMinimum(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_exclusiveMaximum(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_multipleOf(schema *s, carbon_array_it *ait);
fn_result schema_keyword_validate_object_type(schema* s, carbon_array_it* ait);
fn_result schema_validate_run_handleKeyword_minProperties(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_maxProperties(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_required(schema *s, carbon_array_it *ait);
// TODO: implement
// TODO: need function to create carbon file from object it
fn_result schema_validate_run_handleKeyword_properties(schema *s, carbon_array_it *ait);
// TODO: implement
fn_result schema_validate_run_handleKeyword_patternProperties(schema *s, carbon_array_it *ait);
// TODO: implement
fn_result schema_validate_run_handleKeyword_additionalProperties(schema *s, carbon_array_it *ait);
// TODO: implement
fn_result schema_validate_run_handleKeyword_dependencies(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_propertyNames(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_patternRequired(schema *s, carbon_array_it *ait);
fn_result schema_keyword_validate_string_type(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_minLength(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_maxLength(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_pattern(schema *s, carbon_array_it *ait);
fn_result schema_validate_run_handleKeyword_format(schema *s, carbon_array_it *ait);


END_DECL

#endif
