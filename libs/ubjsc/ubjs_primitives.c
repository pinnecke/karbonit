/*
 * Copyright (c) 2016-2017 Tomasz Sieprawski
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

#include <stdio.h>
#include <string.h>
#include "ubjs_common_prv.h"
#include "ubjs_library_prv.h"
#include "ubjs_primitives_prv.h"

ubjs_result ubjs_prmtv_get_marker(ubjs_prmtv *this, ubjs_prmtv_marker **pmarker)
{
    if (0 == this || 0 == pmarker)
    {
        return UR_ERROR;
    }

    *pmarker = this->marker;
    return UR_OK;
}

ubjs_result ubjs_prmtv_free(ubjs_prmtv **pthis)
{
    ubjs_prmtv *this;

    if (0 == pthis || 0 == *pthis)
    {
        return UR_ERROR;
    }

    this = *pthis;
    return (this->marker->free_f)(pthis);
}

ubjs_result ubjs_prmtv_debug_string_get_length(ubjs_prmtv *this, unsigned int *plen)
{
    /* LCOV_EXCL_START */
#ifndef NDEBUG
    /*
     * Max of them all. See ubjs_writer_noncontainers.c. +20.
     */
#endif

    if (0 == this || 0 == plen)
    {
        return UR_ERROR;
    }

#ifndef NDEBUG
    return (this->marker->debug_string_get_length_f)(this, plen);
#else
    *plen = 0;
#endif
    /* LCOV_EXCL_STOP */

    return UR_OK;
}

ubjs_result ubjs_prmtv_debug_string_copy(ubjs_prmtv *this, char *str)
{
    /* LCOV_EXCL_START */
#ifndef NDEBUG
    if (0 == this || 0 == str)
    {
        return UR_ERROR;
    }

    return (this->marker->debug_string_copy_f)(this, str);
    /* LCOV_EXCL_STOP */
#endif
    return UR_OK;
}

void ubjs_prmtv_glue_item_free(void *item)
{
    ubjs_prmtv_free((ubjs_prmtv **)&item);
}

ubjs_result ubjs_prmtv_int(ubjs_library *lib, int64_t value, ubjs_prmtv **pthis)
{
    ubjs_glue_array *markers = 0;
    ubjs_glue_array_iterator *it = 0;

    if (0 == lib || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_markers(lib, &markers);
    (markers->iterate_f)(markers, &it);
    while (UR_OK == (it->next_f)(it))
    {
        ubjs_prmtv_marker *marker;
        (it->get_f)(it, (void **)&marker);
        if (0 != marker->new_from_int64_f)
        {
            ubjs_result ret = (marker->new_from_int64_f)(lib, value, pthis);
            if (UR_OK == ret)
            {
                (it->free_f)(&it);
                return ret;
            }
        }
    }
    (it->free_f)(&it);
    return UR_ERROR;
}

ubjs_result ubjs_prmtv_uint(ubjs_library *lib, int64_t value, ubjs_prmtv **pthis)
{
    if (0 == lib || 0 > value)
    {
        return UR_ERROR;
    }

    return ubjs_prmtv_int(lib, value, pthis);
}

ubjs_result ubjs_prmtv_int_get(ubjs_prmtv *this, int64_t *pvalue)
{
    if (0 == this || 0 == pvalue)
    {
        return UR_ERROR;
    }

    if (0 != this->marker && 0 != this->marker->get_value_int64_f)
    {
        return (this->marker->get_value_int64_f)(this, pvalue);
    }

    return UR_ERROR;
}
