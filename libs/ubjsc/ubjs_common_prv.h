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
/* \internal */
#ifndef HAVE_UBJS_COMMON_PRV
#define HAVE_UBJS_COMMON_PRV

#include "ubjs_common.h"
#include "ubjs_library.h"

UBJS_NO_EXPORT ubjs_result ubjs_endian_is_big(ubjs_bool *pret);
UBJS_NO_EXPORT void ubjs_endian_convert_big_to_native(uint8_t *in, uint8_t *out,
    unsigned int len);
UBJS_NO_EXPORT void ubjs_endian_convert_native_to_big(uint8_t *in, uint8_t *out,
    unsigned int len);

typedef void (*ubjs_compact_sprint_length_f)(void *userdata, unsigned int *plength);
typedef void (*ubjs_compact_sprint_do_f)(void *userdata, char *value);
UBJS_NO_EXPORT void ubjs_compact_sprint(ubjs_library *lib, char **pthis, unsigned int *plen,
    ubjs_compact_sprint_length_f length_f, ubjs_compact_sprint_do_f do_f,
    void *userdata);

UBJS_NO_EXPORT void ubjs_compact_sprintui_length(void *userdata, unsigned int *plength);
UBJS_NO_EXPORT void ubjs_compact_sprintui_do(void *userdata, char *value);
UBJS_NO_EXPORT void ubjs_compact_sprintui(ubjs_library *lib, char **pthis, unsigned int *plen,
    unsigned int value);

UBJS_NO_EXPORT void ubjs_compact_sprintp_length(void *userdata, unsigned int *plength);
UBJS_NO_EXPORT void ubjs_compact_sprintp_do(void *userdata, char *value);
UBJS_NO_EXPORT void ubjs_compact_sprintp(ubjs_library *lib, char **pthis, unsigned int *plen,
    void *value);

UBJS_NO_EXPORT void ubjs_compact_sprints_length(void *userdata, unsigned int *plength);
UBJS_NO_EXPORT void ubjs_compact_sprints_do(void *userdata, char *value);
UBJS_NO_EXPORT void ubjs_compact_sprints(ubjs_library *lib, char **pthis, unsigned int *plen,
    unsigned int length, char *value);

UBJS_NO_EXPORT unsigned int ubjs_digits(unsigned int);

#endif
