/*
 * Copyright (c) 2016 Tomasz Sieprawski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
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
 *  \brief Float32.
 * \since 0.7
 */

#ifndef HAVE_UBJS_PRIMITIVE_FLOAT32
#define HAVE_UBJS_PRIMITIVE_FLOAT32

#ifdef __cplusplus
extern "C"
{
#endif

#include "ubjs_library.h"
#include "ubjs_primitives.h"

/*!
 * \since 0.7
 */
UBJS_EXPORT extern ubjs_prmtv_marker ubjs_prmtv_float32_marker;

/*! \brief Returns float32 primitive.
 *
 * This is a singleton and ubj_prmtv_free do nothing.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_float32(ubjs_library *, float32_t,
    ubjs_prmtv **);

/*!
 * \since 0.7
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_float32_get(ubjs_prmtv *, float32_t *);

#ifdef __cplusplus
}
#endif

#endif
