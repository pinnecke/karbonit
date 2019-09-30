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
#ifndef HAVE_UBJS_GLUE_DICT_LIST_PRV
#define HAVE_UBJS_GLUE_DICT_LIST_PRV

#include "ubjs_glue_dict_list.h"

typedef struct ubjs_glue_dict_list_builder ubjs_glue_dict_list_builder;
typedef struct ubjs_glue_dict_list ubjs_glue_dict_list;
typedef struct ubjs_glue_dict_list_item ubjs_glue_dict_list_item;
typedef struct ubjs_glue_dict_list_iterator ubjs_glue_dict_list_iterator;

struct ubjs_glue_dict_list_builder
{
    ubjs_glue_value_free value_free;
};

struct ubjs_glue_dict_list
{
    unsigned int length;
    ubjs_glue_value_free value_free;

    ubjs_glue_dict_list_item *sentinel;
};

struct ubjs_glue_dict_list_item
{
    unsigned int key_length;
    char *key;
    void *value;

    ubjs_glue_dict_list_item *prev;
    ubjs_glue_dict_list_item *next;
};

struct ubjs_glue_dict_list_iterator
{
    ubjs_glue_dict_list *list;
    ubjs_glue_dict_list_item *at;
    ubjs_bool was_deleted;
};

UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_builder_free(ubjs_glue_dict_builder **);
UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_builder_set_value_free(ubjs_glue_dict_builder *,
    ubjs_glue_value_free);
UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_builder_set_length(ubjs_glue_dict_builder *,
    unsigned int);
UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_builder_build(ubjs_glue_dict_builder *,
   ubjs_glue_dict **);

UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_free(ubjs_glue_dict **);
UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_get_length(ubjs_glue_dict *, unsigned int *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_get(ubjs_glue_dict *, unsigned int,
    char *, void **);
UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_set(ubjs_glue_dict *, unsigned int,
    char *, void *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_delete(ubjs_glue_dict *, unsigned int,
    char *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_iterate(ubjs_glue_dict *,
    ubjs_glue_dict_iterator **);
UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_iterator_next(ubjs_glue_dict_iterator *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_iterator_get_key_length(ubjs_glue_dict_iterator *,
    unsigned int *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_iterator_copy_key(ubjs_glue_dict_iterator *, char *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_iterator_get_value(ubjs_glue_dict_iterator *,
    void **);
UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_iterator_delete(ubjs_glue_dict_iterator *);
UBJS_NO_EXPORT ubjs_result ubjs_glue_dict_list_iterator_free(ubjs_glue_dict_iterator **);

#endif
