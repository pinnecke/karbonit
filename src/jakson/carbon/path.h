/**
 * Columnar Binary JSON -- Copyright 2019 Marcus Pinnecke
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

#ifndef CARBON_PATH_H
#define CARBON_PATH_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/carbon/dot.h>
#include <jakson/carbon/containers.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct carbon_path_evaluator {
        rec *doc;
        carbon_array root_it;
        carbon_path_status_e status;
        struct {
                container_e container_type;
                union {
                        struct {
                                carbon_array it;
                        } array;

                        struct {
                                carbon_object it;
                        } object;

                        struct {
                                carbon_column it;
                                u32 elem_pos;
                        } column;

                } containers;
        } result;
} carbon_path_evaluator;

void carbon_path_evaluator_begin(carbon_path_evaluator *eval, carbon_dot_path *path, rec *doc);
bool carbon_path_evaluator_begin_mutable(carbon_path_evaluator *eval, const carbon_dot_path *path, rev *context);
bool carbon_path_evaluator_end(carbon_path_evaluator *state);

bool carbon_path_evaluator_status(carbon_path_status_e *status, carbon_path_evaluator *state);
bool carbon_path_evaluator_has_result(carbon_path_evaluator *state);
bool carbon_path_exists(rec *doc, const char *path);

bool carbon_path_is_array(rec *doc, const char *path);
bool carbon_path_is_column(rec *doc, const char *path);
bool carbon_path_is_object(rec *doc, const char *path);
bool carbon_path_is_container(rec *doc, const char *path);
bool carbon_path_is_null(rec *doc, const char *path);
bool carbon_path_is_number(rec *doc, const char *path);
bool carbon_path_is_boolean(rec *doc, const char *path);
bool carbon_path_is_string(rec *doc, const char *path);

#ifdef __cplusplus
}
#endif

#endif
