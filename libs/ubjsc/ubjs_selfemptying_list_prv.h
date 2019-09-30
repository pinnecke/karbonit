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

#ifndef HAVE_UBJS_SELFEMPTYING_LIST
#define HAVE_UBJS_SELFEMPTYING_LIST

#ifdef __cplusplus
extern "C"
{
#endif

#include "ubjs_library.h"

typedef struct ubjs_selfemptying_list ubjs_selfemptying_list;
typedef void (*ubjs_selfemptying_list_callback)(ubjs_selfemptying_list *, void *);

struct ubjs_selfemptying_list
{
    ubjs_library *lib;
    ubjs_glue_array *list;

    ubjs_selfemptying_list_callback callback;
    ubjs_bool is_in_callback;
    void *userdata;
};

UBJS_NO_EXPORT ubjs_result ubjs_selfemptying_list_new(ubjs_library *lib, ubjs_glue_value_free,
    ubjs_selfemptying_list_callback, void *, ubjs_selfemptying_list **);
UBJS_NO_EXPORT ubjs_result ubjs_selfemptying_list_free(ubjs_selfemptying_list **);
UBJS_NO_EXPORT ubjs_result ubjs_selfemptying_list_add(ubjs_selfemptying_list *, void *);

#ifdef __cplusplus
}
#endif

#endif

/* \endinternal */
