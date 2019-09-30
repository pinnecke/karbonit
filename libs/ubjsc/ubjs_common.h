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
 *  \brief Definitions of common types, ubjson markers and some utilities.
 *
 * \since 0.2
 */

#ifndef HAVE_UBJS_COMMON
#define HAVE_UBJS_COMMON

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <inttypes.h>
#include "ubjs_exports.h"


/*! \brief Typedef to have consistency in naming all int/float types.
 *
 * \since 0.2
 */
typedef float float32_t;

/*! \brief Typedef to have consistency in naming all int/float types.
 *
 * \since 0.2
 */
typedef double float64_t;

/*! \brief Wrapper around regular C-like booleans.
 *
 * Created for consistency and readibility.
 *
 * \since 0.2
 */
typedef enum ubjs_bool
{
    UFALSE, /*!< False. */
    UTRUE /*!< True. */
} ubjs_bool;

/*! \brief Enum that defines the result of a method.
 *
 * About 99% methods in ubjs API return ubjs_result, with every output being passed
 * by parameters via pointers. This way every method easily tells whether its operation
 * succeded.
 *
 * \since 0.2
 */
typedef enum ubjs_result
{
    UR_OK, /*!< Operation succeded competely. */
    UR_ERROR /*!< Invalid input or otherwise errorness result. */
} ubjs_result;

#ifdef __cplusplus
}
#endif

#endif
