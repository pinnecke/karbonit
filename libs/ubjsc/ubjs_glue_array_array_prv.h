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
#ifndef HAVE_UBJS_GLUE_ARRAY_ARRAY_PRV
#define HAVE_UBJS_GLUE_ARRAY_ARRAY_PRV

#include "ubjs_glue_array_array.h"

typedef struct ubjs_glue_array_array_builder ubjs_glue_array_array_builder;
typedef struct ubjs_glue_array_array ubjs_glue_array_array;
typedef struct ubjs_glue_array_array_iterator ubjs_glue_array_array_iterator;

struct ubjs_glue_array_array_builder
{
    ubjs_glue_value_free value_free;

    ubjs_bool have_length;
    unsigned int length;
};

struct ubjs_glue_array_array
{
    unsigned int length;
    unsigned int allocated;
    void **values;
    ubjs_glue_value_free value_free;
};

struct ubjs_glue_array_array_iterator
{
    ubjs_glue_array_array *list;
    unsigned int at;
    ubjs_bool was_deleted;
};

UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_expand_if_needed(ubjs_glue_array *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_shrink_if_needed(ubjs_glue_array *);

UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_builder_free(ubjs_glue_array_builder **);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_builder_set_value_free(ubjs_glue_array_builder *,
    ubjs_glue_value_free);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_builder_set_length(ubjs_glue_array_builder *,
    unsigned int);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_builder_build(ubjs_glue_array_builder *,
    ubjs_glue_array **);

UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_free(ubjs_glue_array **);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_get_length(ubjs_glue_array *, unsigned int *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_get_first(ubjs_glue_array *, void **);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_get_last(ubjs_glue_array *, void **);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_get_at(ubjs_glue_array *, unsigned int, void **);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_add_first(ubjs_glue_array *, void *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_add_last(ubjs_glue_array *, void *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_add_at(ubjs_glue_array *, unsigned int, void *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_delete_first(ubjs_glue_array *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_delete_last(ubjs_glue_array *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_delete_at(ubjs_glue_array *, unsigned int);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_iterate(ubjs_glue_array *,
    ubjs_glue_array_iterator **);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_iterator_next(ubjs_glue_array_iterator *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_iterator_get(ubjs_glue_array_iterator *,
    void **);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_iterator_delete(ubjs_glue_array_iterator *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_array_array_iterator_free(ubjs_glue_array_iterator **);

#endif
