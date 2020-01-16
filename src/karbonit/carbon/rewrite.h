/**
 * rewrite - replace a field by another field
 *
 * Copyright 2020 Marcus Pinnecke
 */

#ifndef HAD_REWRITE_H
#define HAD_REWRITE_H

#include <karbonit/stdinc.h>
#include <karbonit/mem/memfile.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Replaces the field pointed by <code>find</code> to the value in <code>src</code>
 *
 * \param find is a valid evaluated dot path that points to a field either inside an array or object container
 * \param src memory file that is positioned to a field marker
 * \param cpy_arr_bounds flag to indicate whether to copy outer-most '[' and ']'
 */
void rewrite_field(find *find, memfile *src, bool cpy_arr_bounds);

/*!
 * \brief Replaces the element at the position in the column resolved via <code>find</code> by the
 * field in <code>src</code>.
 *
 * Whenever needed, the column might be rewritten to match a larger value domain, or into an array in case
 * the field in <code>src</code> cannot be stored inside a column.
 *
 * \note Even if the domain of the column could be reset to a smaller domain (e.g., from u16 to u8), a rewrite
 * to a smaller domain is never executed by this function (and must be manually triggered). However, if the domain
 * of the column is too small to contain the new value, the column is rewritten to the larger domain. The reason for
 * this different behavior (i.e., rewrite to larger domain but never to smaller) is that a write should not introduce
 * too much overhead (i.e., don't rewrite the column if not required to hold the new datum) and that a write should
 * always succeed.
 *
 * \note If a value should be stored into the column that cannot be stored (e.g., an object cannot be stored in a
 * column), then the column is rewritten to an array to match the user 'set' request. However, this comes by a notable
 * drop in the read performance since columns are way more faster iterated than arrays. However, the same design
 * decision holds: the 'set' request should succeed.
 *
 * \param find a path evaluation that points to the element that should be overwritten.
 * \param src memory file that is positioned to a field marker
 */
void rewrite_column(find *find, memfile *src);

#ifdef __cplusplus
}
#endif

#endif
