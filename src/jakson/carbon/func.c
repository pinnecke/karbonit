/*
 * func - high-level callable functions on records to query, update and remove elements and containers
 *
 * Copyright 2020 Marcus Pinnecke
 */

#include <jakson/carbon/func.h>
#include <jakson/carbon/find.h>
#include <jakson/json.h>

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
        return (path && dot_is_empty(path)) || (doc && path && find_from_dot(&eval, path, (rec *) doc) &&
               !find_result_is_null(&eval) && !find_result_is_undefined(&eval));
}

/*!
 * \brief Gets the value of the element which is pointed to by a given dot-notated path.
 *
 * The function invokes internally a dot-path evaluation and prints the result into the string buffer <code>buf</code>,
 * which c-string is the return value of this function.
 *
 * \attention The buffer <code>buf</code> is cleaned up before used. In case the buffer is non-empty before the call,
 *      its content is deleted.
 *
 * \note In case the path evaluation fails (e.g., the path points to an not existing element), <code>undef</code> (rather
 * than <code>null</code>) is returned.
 *
 * \note In case <code>path</code> is empty, the entire record, formatted as json, is returned.
 *
 * \param doc non-null pointer to the Carbon record for which <code>path</code> should be evaluated
 * \param path non-null pointer to a dot-notated path that should be evaluated for <code>doc</code>
 * \param buf non-null pointer to an already created string buffer
 * \return the string buffers c-string which is the json formatted result of the path evaluation. In case of an error,
 *         <code>undef</code> is returned.
 */
inline const char *func_get(const rec *doc, const dot *path, str_buf *buf)
{
        find eval;
        str_buf_clear(buf);
        if (!doc || !path || !buf) {
                return json_from_undef(buf);
        } else {
                if (dot_is_empty(path)) {
                        return json_from_record(buf, (rec *) doc);
                } else {
                        find_from_dot(&eval, path, (rec *) doc);
                        return find_result_to_str(buf, &eval);
                }
        }
}