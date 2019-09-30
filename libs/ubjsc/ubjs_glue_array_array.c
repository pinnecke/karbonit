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

#include <string.h>
#include "ubjs_library_prv.h"
#include "ubjs_glue_array_array_prv.h"

static unsigned int ubjs_glue_array_array_initial = 10;
static unsigned int ubjs_glue_array_array_expand_multiply = 3;
static unsigned int ubjs_glue_array_array_expand_add = 2;

ubjs_result ubjs_glue_array_array_builder_new(ubjs_library *lib, ubjs_glue_array_builder **pthis)
{
    ubjs_glue_array_builder *this = 0;
    ubjs_glue_array_array_builder *data = 0;
    ubjs_library_alloc_f alloc_f;

    ubjs_library_get_alloc_f(lib, &alloc_f);
    this = (ubjs_glue_array_builder *)(alloc_f)(sizeof(struct ubjs_glue_array_builder));
    data = (ubjs_glue_array_array_builder *)(alloc_f)(sizeof(
        struct ubjs_glue_array_array_builder));
    this->lib = lib;
    this->userdata = data;
    data->value_free = 0;
    data->have_length = UFALSE;
    data->length = 0;

    this->set_value_free_f = ubjs_glue_array_array_builder_set_value_free;
    this->set_length_f = ubjs_glue_array_array_builder_set_length;
    this->free_f = ubjs_glue_array_array_builder_free;
    this->build_f = ubjs_glue_array_array_builder_build;

    *pthis = this;
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_builder_free(ubjs_glue_array_builder **pthis)
{
    ubjs_glue_array_builder *this = *pthis;
    ubjs_glue_array_array_builder *data = (ubjs_glue_array_array_builder *)this->userdata;
    ubjs_library_free_f free_f;

    ubjs_library_get_free_f(this->lib, &free_f);
    (free_f)(data);
    (free_f)(this);
    *pthis = 0;
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_builder_set_value_free(ubjs_glue_array_builder *this,
    ubjs_glue_value_free value_free)
{
    ubjs_glue_array_array_builder *data = (ubjs_glue_array_array_builder *)this->userdata;
    data->value_free = value_free;
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_builder_set_length(ubjs_glue_array_builder *this,
    unsigned int length)
{
    ubjs_glue_array_array_builder *data = (ubjs_glue_array_array_builder *)this->userdata;
    data->have_length = UTRUE;
    data->length = length;
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_builder_build(ubjs_glue_array_builder *this,
    ubjs_glue_array **parr)
{
    ubjs_glue_array_array_builder *data = (ubjs_glue_array_array_builder *)this->userdata;
    ubjs_glue_array_array *list = 0;
    ubjs_glue_array *arr = 0;
    ubjs_library_alloc_f alloc_f;

    ubjs_library_get_alloc_f(this->lib, &alloc_f);
    list = (ubjs_glue_array_array *)(alloc_f)(sizeof(struct ubjs_glue_array_array));
    list->value_free = data->value_free;
    list->length = 0;
    list->allocated = (UTRUE == data->have_length && data->length > ubjs_glue_array_array_initial
        ? data->length : ubjs_glue_array_array_initial);
    list->values = (void **)(alloc_f)(sizeof(void *) * list->allocated);

    arr = (ubjs_glue_array *)(alloc_f)(sizeof(struct ubjs_glue_array));
    arr->lib = this->lib;
    arr->userdata = (void *)list;

    arr->free_f = ubjs_glue_array_array_free;
    arr->get_length_f = ubjs_glue_array_array_get_length;
    arr->get_first_f = ubjs_glue_array_array_get_first;
    arr->get_last_f = ubjs_glue_array_array_get_last;
    arr->get_at_f = ubjs_glue_array_array_get_at;
    arr->add_first_f = ubjs_glue_array_array_add_first;
    arr->add_last_f = ubjs_glue_array_array_add_last;
    arr->add_at_f = ubjs_glue_array_array_add_at;
    arr->delete_first_f = ubjs_glue_array_array_delete_first;
    arr->delete_last_f = ubjs_glue_array_array_delete_last;
    arr->delete_at_f = ubjs_glue_array_array_delete_at;
    arr->iterate_f = ubjs_glue_array_array_iterate;

    *parr = arr;
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_free(ubjs_glue_array **pthis)
{
    ubjs_glue_array *this = *pthis;
    ubjs_glue_array_array *list = (ubjs_glue_array_array *)this->userdata;
    unsigned int i;
    ubjs_library_free_f free_f;

    for (i = 0; i < list->length; i++)
    {
        if (0 != list->value_free)
        {
            (list->value_free)(list->values[i]);
        }
    }

    ubjs_library_get_free_f(this->lib, &free_f);
    (free_f)(list->values);
    (free_f)(list);
    (free_f)(this);

    *pthis=0;
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_expand_if_needed(ubjs_glue_array *this)
{
    ubjs_glue_array_array *list = (ubjs_glue_array_array *)this->userdata;
    void **nvalues;
    ubjs_library_alloc_f alloc_f;
    ubjs_library_free_f free_f;

    if (list->length < list->allocated)
    {
        return UR_OK;
    }

    ubjs_library_get_alloc_f(this->lib, &alloc_f);
    ubjs_library_get_free_f(this->lib, &free_f);

    list->allocated = list->allocated * ubjs_glue_array_array_expand_multiply +
        ubjs_glue_array_array_expand_add;
    nvalues = (void **)(alloc_f)(sizeof(void *) * list->allocated);
    memcpy(nvalues, list->values, sizeof(void *) * list->length);
    (free_f)(list->values);
    list->values = nvalues;
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_shrink_if_needed(ubjs_glue_array *this)
{
    ubjs_glue_array_array *list = (ubjs_glue_array_array *)this->userdata;
    void **nvalues;
    ubjs_library_alloc_f alloc_f;
    ubjs_library_free_f free_f;

    if (list->length * ubjs_glue_array_array_expand_multiply + ubjs_glue_array_array_expand_add
        > list->allocated || list->allocated <= ubjs_glue_array_array_initial)
    {
        return UR_OK;
    }

    ubjs_library_get_alloc_f(this->lib, &alloc_f);
    ubjs_library_get_free_f(this->lib, &free_f);

    list->allocated = (list->allocated - ubjs_glue_array_array_expand_add) /
        ubjs_glue_array_array_expand_multiply;
    nvalues = (void **)(alloc_f)(sizeof(void *) * list->allocated);
    memcpy(nvalues, list->values, sizeof(void *) * list->length);
    (free_f)(list->values);
    list->values = nvalues;
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_get_length(ubjs_glue_array *this, unsigned int *plen)
{
    ubjs_glue_array_array *list = (ubjs_glue_array_array *)this->userdata;
    *plen = list->length;
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_get_first(ubjs_glue_array *this, void **pvalue)
{
    return ubjs_glue_array_array_get_at(this, 0, pvalue);
}

ubjs_result ubjs_glue_array_array_get_last(ubjs_glue_array *this, void **pvalue)
{
    ubjs_glue_array_array *list = (ubjs_glue_array_array *)this->userdata;
    return ubjs_glue_array_array_get_at(this, list->length - 1, pvalue);
}

ubjs_result ubjs_glue_array_array_get_at(ubjs_glue_array *this, unsigned int at,
    void **pvalue)
{
    ubjs_glue_array_array *list = (ubjs_glue_array_array *)this->userdata;

    if (0 == list->length || at >= list->length)
    {
        return UR_ERROR;
    }

    *pvalue = list->values[at];
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_add_first(ubjs_glue_array *this, void *value)
{
    return ubjs_glue_array_array_add_at(this, 0, value);
}

ubjs_result ubjs_glue_array_array_add_last(ubjs_glue_array *this, void *value)
{
    ubjs_glue_array_array *list = (ubjs_glue_array_array *)this->userdata;
    return ubjs_glue_array_array_add_at(this, list->length, value);
}

ubjs_result ubjs_glue_array_array_add_at(ubjs_glue_array *this, unsigned int at,
    void *value)
{
    ubjs_glue_array_array *list = (ubjs_glue_array_array *)this->userdata;
    unsigned int i;

    if (at > list->length)
    {
        return UR_ERROR;
    }

    ubjs_glue_array_array_expand_if_needed(this);

    for (i = list->length; i > at; i--)
    {
        list->values[i] = list->values[i - 1];
    }

    list->values[at] = value;
    list->length++;
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_delete_first(ubjs_glue_array *this)
{
    return ubjs_glue_array_array_delete_at(this, 0);
}

ubjs_result ubjs_glue_array_array_delete_last(ubjs_glue_array *this)
{
    ubjs_glue_array_array *list = (ubjs_glue_array_array *)this->userdata;
    return ubjs_glue_array_array_delete_at(this, list->length - 1);
}

ubjs_result ubjs_glue_array_array_delete_at(ubjs_glue_array *this, unsigned int at)
{
    ubjs_glue_array_array *list = (ubjs_glue_array_array *)this->userdata;
    unsigned int i;

    if (0 == list->length || at >= list->length)
    {
        return UR_ERROR;
    }

    (list->value_free)(list->values[at]);

    for (i = at; i < list->length - 1; i++)
    {
        list->values[i] = list->values[i + 1];
    }

    list->length--;
    ubjs_glue_array_array_shrink_if_needed(this);
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_iterate(ubjs_glue_array *this,
    ubjs_glue_array_iterator **piterator)
{
    ubjs_glue_array_array *list = (ubjs_glue_array_array *)this->userdata;
    ubjs_glue_array_array_iterator *list_iterator = 0;
    ubjs_glue_array_iterator *iterator = 0;
    ubjs_library_alloc_f alloc_f;

    ubjs_library_get_alloc_f(this->lib, &alloc_f);

    list_iterator = (ubjs_glue_array_array_iterator *)(alloc_f)(
        sizeof(struct ubjs_glue_array_array_iterator));
    list_iterator->at = -1;
    list_iterator->was_deleted = UFALSE;
    list_iterator->list = list;

    iterator=(ubjs_glue_array_iterator *)(alloc_f)(
        sizeof(struct ubjs_glue_array_iterator));
    iterator->array=this;
    iterator->userdata=(void *)list_iterator;

    iterator->free_f = ubjs_glue_array_array_iterator_free;
    iterator->next_f = ubjs_glue_array_array_iterator_next;
    iterator->get_f = ubjs_glue_array_array_iterator_get;
    iterator->delete_f = ubjs_glue_array_array_iterator_delete;

    *piterator=iterator;
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_iterator_free(ubjs_glue_array_iterator **pthis)
{
    ubjs_glue_array_iterator *this = *pthis;
    ubjs_glue_array_array_iterator *iterator = (ubjs_glue_array_array_iterator *)this->userdata;
    ubjs_library_free_f free_f;

    ubjs_library_get_free_f(this->array->lib, &free_f);
    (free_f)(iterator);
    (free_f)(this);

    *pthis=0;
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_iterator_next(ubjs_glue_array_iterator *this)
{
    ubjs_glue_array_array_iterator *iterator = (ubjs_glue_array_array_iterator *)this->userdata;

    if (UFALSE == iterator->was_deleted)
    {
        iterator->at++;
    }
    iterator->was_deleted = UFALSE;
    if (iterator->at >= iterator->list->length)
    {
        return UR_ERROR;
    }

    return UR_OK;
}

ubjs_result ubjs_glue_array_array_iterator_get(ubjs_glue_array_iterator *this,
    void **pvalue)
{
    ubjs_glue_array_array_iterator *iterator = (ubjs_glue_array_array_iterator *)this->userdata;
    if (iterator->at >= iterator->list->length || UTRUE == iterator->was_deleted)
    {
        return UR_ERROR;
    }

    *pvalue = iterator->list->values[iterator->at];
    return UR_OK;
}

ubjs_result ubjs_glue_array_array_iterator_delete(ubjs_glue_array_iterator *this)
{
    ubjs_glue_array_array_iterator *iterator = (ubjs_glue_array_array_iterator *)this->userdata;
    ubjs_result ret;

    if (iterator->at >= iterator->list->length || UTRUE == iterator->was_deleted)
    {
        return UR_ERROR;
    }

    ret = ubjs_glue_array_array_delete_at(this->array, iterator->at);
    if (UR_OK == ret)
    {
        iterator->was_deleted = UTRUE;
    }
    return ret;
}
