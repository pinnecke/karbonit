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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ubjs_common_prv.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-compare"
ubjs_result ubjs_endian_is_big(ubjs_bool *pret)
{
    volatile uint32_t i=0x01234567;
    (*pret)=((*((uint8_t*)(&i))) == 0x67) ? UTRUE : UFALSE;
    return UR_OK;
}

/* LCOV_EXCL_START */
static void swap(uint8_t *in, uint8_t *out, unsigned int len)
{
    int i;

    for (i=0; len>i; i++)
    {
        out[len - i - 1] = in[i];
    }
}

static void copy(uint8_t *in, uint8_t *out, unsigned int len)
{
    int i;

    for (i=0; len>i; i++)
    {
        out[i] = in[i];
    }
}
/* LCOV_EXCL_STOP */

void ubjs_endian_convert_big_to_native(uint8_t *in, uint8_t *out, unsigned int len)
{
    ubjs_bool big;

    ubjs_endian_is_big(&big);

    /* LCOV_EXCL_START */
    if (UTRUE == big)
    {
        copy(in, out, len);
    }
    else
    {
        swap(in, out, len);
    }
    /* LCOV_EXCL_STOP */
}

void ubjs_endian_convert_native_to_big(uint8_t *in, uint8_t *out, unsigned int len)
{
    ubjs_bool big;

    ubjs_endian_is_big(&big);

    /* LCOV_EXCL_START */
    if (UTRUE == big)
    {
        copy(in, out, len);
    }
    else
    {
        swap(in, out, len);
    }
    /* LCOV_EXCL_STOP */
}

void ubjs_compact_sprintf(ubjs_library *lib, char **pthis, unsigned int *plen,
    ubjs_compact_sprint_length_f length_f, ubjs_compact_sprint_do_f do_f,
    void *userdata)
{
    char *now = 0;
    unsigned int add_length = 0;
    unsigned int length;
    unsigned int offset = 0;
    char *othis = 0;
    unsigned int olen = 0;
    ubjs_library_alloc_f alloc_f;
    ubjs_library_free_f free_f;

    /* LCOV_EXCL_START */
    if (0 != *pthis)
    {
        othis = *pthis;
        olen = *plen;
        offset = olen;
    }
    /* LCOV_EXCL_STOP */

    (length_f)(userdata, &add_length);

    length=offset + add_length;
    ubjs_library_get_alloc_f(lib, &alloc_f);
    now=(char *)(alloc_f)(sizeof(char) * (length + 1));

    /* LCOV_EXCL_START */
    if (0 != othis)
    {
        memcpy(now, othis, olen * sizeof(char));

        ubjs_library_get_free_f(lib, &free_f);
        (free_f)(othis);
    }
    /* LCOV_EXCL_STOP */

    (do_f)(userdata, now + offset);

    now[length] = 0;

    *plen=length;
    *pthis=now;
}

void ubjs_compact_sprintui_length(void *userdata, unsigned int *plength)
{
    char buf[14];
    unsigned int v = *((unsigned int *)userdata);
    unsigned int len = sprintf(buf, "%u", v);
    *plength = len;
}

void ubjs_compact_sprintui_do(void *userdata, char *value)
{
    unsigned int v = *((unsigned int *)userdata);
    sprintf(value, "%u", v);
}

void ubjs_compact_sprintui(ubjs_library *lib, char **pthis, unsigned int *plen,
    unsigned int value)
{
    ubjs_compact_sprintf(lib, pthis, plen,
        ubjs_compact_sprintui_length,
        ubjs_compact_sprintui_do,
        (void *)&value);
}

void ubjs_compact_sprintp_length(void *userdata, unsigned int *plength)
{
    char buf[14];
    void *v = userdata;
    unsigned int len = sprintf(buf, "%p", v);
    *plength = len;
}

void ubjs_compact_sprintp_do(void *userdata, char *value)
{
    void *v = userdata;
    sprintf(value, "%p", v);
}

void ubjs_compact_sprintp(ubjs_library *lib, char **pthis, unsigned int *plen,
    void *value)
{
    ubjs_compact_sprintf(lib, pthis, plen,
        ubjs_compact_sprintp_length,
        ubjs_compact_sprintp_do,
        value);
}

typedef struct ubjs_compact_sprints_value
{
    unsigned int len;
    char *value;
} ubjs_compact_sprints_value;

void ubjs_compact_sprints_length(void *userdata, unsigned int *plength)
{
    ubjs_compact_sprints_value *v = (ubjs_compact_sprints_value *)userdata;
    *plength = v->len;
}

void ubjs_compact_sprints_do(void *userdata, char *value)
{
    ubjs_compact_sprints_value *v = (ubjs_compact_sprints_value *)userdata;
    memcpy(value, v->value, sizeof(char) * v->len);
}

void ubjs_compact_sprints(ubjs_library *lib, char **pthis, unsigned int *plen,
    unsigned int len, char *value)
{
    ubjs_compact_sprints_value v = {len, value};
    ubjs_compact_sprintf(lib, pthis, plen,
        ubjs_compact_sprints_length,
        ubjs_compact_sprints_do,
        (void *)&v);
}

unsigned int ubjs_digits(unsigned int in)
{
    unsigned int a = 0;
    do
    {
        a++;
        in >>= 8;
    }
    while (in > 0);
    return a;
}

#pragma clang diagnostic pop