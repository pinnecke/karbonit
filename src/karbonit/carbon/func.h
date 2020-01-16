/*
 * func - high-level callable functions on records to query, update and remove elements and containers
 *
 * Copyright 2020 Marcus Pinnecke
 */

#ifndef HAD_FUNC_H
#define HAD_FUNC_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <karbonit/stdinc.h>
#include <karbonit/rec.h>
#include <karbonit/carbon/dot.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------------------------------------------------
//  functions on a single record
// ---------------------------------------------------------------------------------------------------------------------

/*!
 * \brief Tests whether the specific dot-notated path <code>path</code> resolves to an existing element in the document
 * <code>doc</code>.
 *
 * The function returns <code>TRUE</code> if (and only if) an non-undefined non-null element exists that is pointed to by
 * <code>path</code>. If the element does not exist, or in case the value of that element is <code>undef</code> or
 * <code>null</code>, <code>FALSE</code> is returned.
 *
 * \note If <code>path</code> is empty, the entire record must be considered. Since <code>doc</code> must be valid,
 *       the function returns <code>TRUE</code> in this case.
 *
 * \param doc non-null pointer to the Carbon record for which <code>path</code> should be evaluated
 * \param path non-null pointer to a dot-notated path that should be evaluated for <code>doc</code>
 * \returns <code>TRUE</code> if <code>path</code> points to an existing element. In all other cases (including errors),
 *          <code>FALSE</code> is returned.
 */
bool func_rec_exists(const rec *doc, const dot *path);

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
const char *func_rec_get(const rec *doc, const dot *path, str_buf *buf);

const char *func_rec_mget(const rec *doc, const dot *path, str_buf *buf);

/*!
 * \brief Sets the value of the element in <code>doc</code> pointed by a dot-notated path <code>path</code> to
 * the contens of <code>import</code>.
 *
 * This function performs an update or create operation in the record <code>doc</code> potentially modifying
 * <code>doc</code>, or writing directly to <code>rev</code>.
 *
 * An <b>update</b> is executed if <code>path</code> evaluates to an existing element in <code>doc</code>. In case
 * the path does not point to an existing element, the elements for this path is <b>created</b> as needed to and its
 * value is set accordingly.
 *
 * The parameter <b>patch</b> controls whether the input record <code>doc</code> is modified or whether a new
 * revision of <code>doc</code> is created leading <code>doc</code> untouched. If <b>patch</b> is turned on,
 * <code>doc</code> is modified and the input parameter <code>rev</code> is unused (and may be <code>NULL</code>).
 * Otherwise, if <b>patch</b> is turned off, a revision of <code>doc</code> is executed and stored in the non-null
 * input parameter <code>rev</code>. In other words, this update resp. create function affects the object
 * <code>doc</code> only if <code>patch</code> is set to <code>FALSE</code>.
 *
 * \note If <b>patch</b> is turned off, this function is thread-safe. In this case, <code>doc</code> is not
 * touched, and <code>rev</code> is a new revision (including a new revision number) of <code>doc</code>. Is
 * <b>patch</b> is turned on, <code>doc</code> is modified on-the-fly without changes to the revision number.
 * In this case, the function is not thread-safe.
 *
 * Update and potential rewrite actions are listed below. Note, that a path that points to a non-existing element
 * resolves to 'undef':
 *
 * Resolved
 * --------
 * (undef)
 * null
 * true
 * false
 * number (u8)
 * number (u16)
 * number (u32)
 * number (u64)
 * number (i8)
 * number (i16)
 * number (i32)
 * number (i64)
 * float
 * string
 * binary
 * object
 * array
 * column
 * null (in column)
 * true (in column)
 * false (in column)
 * number (u8) (in column)
 * number (u16) (in column)
 * number (u32) (in column)
 * number (u64) (in column)
 * number (i8) (in column)
 * number (i16) (in column)
 * number (i32) (in column)
 * number (i64) (in column)
 * float (in column)
 *
 *
 *
 * \param rev A pointer to an uninitialized Carbon record that will contain the revision of <code>doc</code>
 *            having the change applied. The pointer must be non-null if <code>patch</code> is set to <code>FALSE</code>,
 *            and can be null if <code>patch</code> is set to <code>TRUE</code>.
 * \param doc A non-null pointer to an already created Carbon record which is either target of modification (if
 *            <code>patch</code> is turned on), or template for the revision <code>rev</code> (if <code>patch</code>
 *            is turned off).
 * \param path A non-null dot-notated path that points to an (potentially not existing) element in <code>doc</code>
 * \param import A record that contains the new-to-set value for the element pointed by <code>path</code>
 * \param patch A flag controlling whether a patch (<code>patch</code> is turned on), or a revision (<code>patch</code>
 *              is turned off) is executed on <code>doc</code>.
 * \return The pointer to the record which has been modified, or <code>NULL</code> in case of an error. This pointer
 *         points to <code>doc</code> if <code>patch</code> is set to <code>TRUE</code>, and to <code>rev</code>
 *         if <code>patch</code> is set to <code>FALSE</code>.
 */
rec *func_rec_set(rec *rev, rec *doc, const dot *path, const rec *import, bool patch);

rec *func_rec_mset(rec *rev, rec *doc, const dot *path, const rec *import, bool patch);

rec *func_rec_unset(rec *rev, rec *doc, const dot *path, bool patch);

// func_revof

// func_keysof

// func_rename

// func_typeof

// func_isnull

// func_isundef

// func_randkey

// func_randvalue

// func_mv

// func_sort

// func_add

// func_mul

// func_sub

// func_div

// func_push

// func_pop

// func_keysof

// func_insert

// func_lenof

// func_xor

// func_and



// ---------------------------------------------------------------------------------------------------------------------
//  functions on a list container
// ---------------------------------------------------------------------------------------------------------------------

const char *func_list_slice(rec *doc, const dot *path, i64 begin, i64 end, str_buf *buf);

rec *func_list_pushback(rec *rev, rec *doc, const dot *path, const char *json, bool patch);



// func_unset
// func_lenof
// func_reverse
// func_filter
// func_any
// func_all
// func_map
// func_zip
// func_sum
// func_mul
// func_sort
// func_min
// func_max
// func_drop
// func_head
// func_tail
// func_split
// func_cpy
// func_mv
// func_wrap
// func_unwrap
// func_rename
// func_mappingof


#ifdef __cplusplus
}
#endif

#endif
