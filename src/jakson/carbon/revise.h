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

#ifndef CARBON_REVISE_H
#define CARBON_REVISE_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/stdx/unique_id.h>
#include <jakson/rec.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Acquires a new revision context for the carbon document.
 *
 * In case of an already running revision, the function returns <code>false</code> without blocking.
 * Otherwise, <code>carbon_revise_begin</code> is called internally.
 *
 * @param context non-null pointer to revision context
 * @param doc document that should be revised
 * @return <code>false</code> in case of an already running revision. Otherwise returns value of
 *                            <code>carbon_revise_begin</code>
 */
void carbon_revise_try_begin(rev *context, rec *revised_doc, rec *doc);
void carbon_revise_begin(rev *context, rec *revised_doc, rec *original);
const rec *carbon_revise_end(rev *context);

bool carbon_revise_key_generate(unique_id_t *out, rev *context);

bool carbon_revise_key_set_unsigned(rev *context, u64 key_value);
bool carbon_revise_key_set_signed(rev *context, i64 key_value);
bool carbon_revise_key_set_string(rev *context, const char *key_value);

void carbon_revise_set_list_type(rev *context, carbon_list_derivable_e derivation);

bool carbon_revise_iterator_open(carbon_array *it, rev *context);
void carbon_revise_iterator_close(carbon_array *it);

bool carbon_revise_find_begin(carbon_find *out, const char *dot_path, rev *context);
bool carbon_revise_find_end(carbon_find *find);

bool carbon_revise_remove(const char *dot_path, rev *context);
bool carbon_revise_remove_one(const char *dot_path, rec *rev_doc, rec *doc);

bool carbon_revise_pack(rev *context);
bool carbon_revise_shrink(rev *context);

bool carbon_revise_abort(rev *context);

#ifdef __cplusplus
}
#endif

#endif
