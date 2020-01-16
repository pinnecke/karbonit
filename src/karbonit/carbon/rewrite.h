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
 * \brief Replaces the field in <code>dst</code> by the field in <code>src</code>
 *
 * \param dst memory file that is positioned to a field marker
 * \param src memory file that is positioned to a field marker
 * \param cpy_arr_bounds flag to indicate whether to copy outer-most '[' and ']'
 */
void rewrite_field(memfile *dst, memfile *src, bool cpy_arr_bounds);

/*!
 * \brief Replaces the element at position <code>idx</code> in the column which is stored in <code>dst</code> by the
 * field in <code>src</code>.
 *
 * Whenever needed, the column might be rewritten to match a small (or larger) value domain, or into an array in case
 * the field in  <code>src</code> cannot be stored inside a column. If a column-to-array rewrite is done.
 *
 * \param dst memory file that is positioned to a field marker
 * \param src memory file that is positioned to a field marker
 * \param cpy_arr_bounds
 */
void rewrite_column(memfile *dst, memfile *src, bool cpy_arr_bounds);

#ifdef __cplusplus
}
#endif

#endif
