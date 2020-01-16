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
 * \return <code>TRUE</code> is case of success, and <code>FALSE</code> otherwise
 */
bool rewrite_field(memfile *dst, memfile *src, bool cpy_arr_bounds);

#ifdef __cplusplus
}
#endif

#endif
