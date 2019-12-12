/*
 * revise - thread-safe modification of records by revise records
 *
 * Copyright 2019 Marcus Pinnecke
 */

#ifndef HAD_REVISE_H
#define HAD_REVISE_H

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
 * Otherwise, <code>revise_begin</code> is called internally.
 *
 * @param context non-null pointer to revision context
 * @param doc document that should be revised
 * @return <code>false</code> in case of an already running revision. Otherwise returns value of
 *                            <code>revise_begin</code>
 */
void revise_begin(rev *context, rec *revised, rec *original);
const rec *revise_end(rev *context);

bool revise_key_generate(unique_id_t *out, rev *context);

bool revise_key_set_unsigned(rev *context, u64 key_value);
bool revise_key_set_signed(rev *context, i64 key_value);
bool revise_key_set_string(rev *context, const char *key_value);

void revise_set_list_type(rev *context, list_type_e derivation);

bool revise_iterator_open(arr_it *it, rev *context);
void revise_iterator_close(arr_it *it);

bool revise_find_begin(find *out, const char *dot, rev *context);
void revise_find_end(find *find);

bool revise_remove(const char *dot, rev *context);
bool revise_remove_one(const char *dot, rec *rev_doc, rec *doc);

bool revise_pack(rev *context);
bool revise_shrink(rev *context);

bool revise_abort(rev *context);

#ifdef __cplusplus
}
#endif

#endif
