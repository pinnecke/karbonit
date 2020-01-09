/**
 * rewrite - replace a field by another field
 *
 * Copyright 2020 Marcus Pinnecke
 */

#ifndef HAD_REWRITE_H
#define HAD_REWRITE_H

#include <jakson/stdinc.h>
#include <jakson/mem/memfile.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Replaces the field in <code>dst</code> by the field in <code>src</code>
 *
 * @param dst memory file that is positioned to a field marker
 * @param src memory file that is positioned to a field marker
 * @return <code>TRUE</code> is case of success, and <code>FALSE</code> otherwise
 */
bool rewrite_field(memfile *dst, memfile *src);

#ifdef __cplusplus
}
#endif

#endif
