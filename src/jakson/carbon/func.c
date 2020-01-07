/*
 * func - high-level callable functions on records to query, update and remove elements and containers
 *
 * Copyright 2020 Marcus Pinnecke
 */

#include <jakson/carbon/func.h>
#include <jakson/carbon/find.h>

/*!
 * \brief Tests whether the specific dot-notated path <code>path</code> resolves to an existing element in the document
 * <code>doc</code>.
 *
 * The function returns <code>TRUE</code> if (and only if) an non-undefined non-null element exists that is pointed to by
 * <code>path</code>. If the element does not exist, or in case the value of that element is <code>undef</code> or
 * <code>null</code>, <code>FALSE</code> is returned.
 *
 * \param doc non-null pointer to the Carbon record for which <code>path</code> should be evaluated
 * \param path non-null pointer to a dot-notated path that should be evaluated for <code>doc</code>
 * \returns <code>TRUE</code> if <code>path</code> points to an existing element. In all other cases (including errors),
 *          <code>FALSE</code> is returned.
 */
inline bool func_exists(const rec *doc, const dot *path)
{
        find eval;
        return !dot_is_empty(path) && find_from_dot(&eval, path, (rec *) doc) &&
               !find_result_is_null(&eval) && !find_result_is_undefined(&eval);
}