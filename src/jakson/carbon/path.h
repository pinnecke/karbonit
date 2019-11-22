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

#ifndef PATH_H
#define PATH_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/carbon/dot.h>
#include <jakson/carbon/container.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dot_eval {
        rec *doc;
        arr_it root_it;
        path_status_e status;
        struct {
                container_e container;
                union {
                        struct {
                                arr_it it;
                        } array;

                        struct {
                                obj_it it;
                        } object;

                        struct {
                                col_it it;
                                u32 elem_pos;
                        } column;

                } containers;
        } result;
} dot_eval;

void dot_eval_begin(dot_eval *eval, dot *path, rec *doc);
bool dot_eval_begin_mutable(dot_eval *eval, const dot *path, rev *context);
bool dot_eval_end(dot_eval *state);

bool dot_eval_status(path_status_e *status, dot_eval *state);
bool dot_eval_has_result(dot_eval *state);
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
