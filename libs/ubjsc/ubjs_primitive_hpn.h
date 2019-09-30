/*
 * Copyright (c) 2016 Tomasz Sieprawski
 *
 * Permission is hereby granted, free of hpnge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without rehpniction, including without limitation the rights
 * to use, copy, modify, merge, publish, dihpnibute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/
/*! \file
 *  \brief High-precision number.
 * \since 0.7
 */

#ifndef HAVE_UBJS_PRIMITIVE_HPN
#define HAVE_UBJS_PRIMITIVE_HPN

#ifdef __cplusplus
extern "C"
{
#endif

#include "ubjs_library.h"
#include "ubjs_primitives.h"

/*!
 * \since 0.7
 */
UBJS_EXPORT extern ubjs_prmtv_marker ubjs_prmtv_hpn_marker;

/*! \brief Returns hpn primitive.
 *
 * This is a singleton and ubj_prmtv_free do nothing.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_hpn(ubjs_library *, unsigned int, char *,
    ubjs_prmtv **);

/*!
 * \since 0.7
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_hpn_get_length(ubjs_prmtv *, unsigned int *);
/*!
 * \since 0.7
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_hpn_copy_text(ubjs_prmtv *, char *);

#ifdef __cplusplus
}
#endif

#endif
