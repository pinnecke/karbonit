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
#include "ubjs_glue_dict_list_prv.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
ubjs_result ubjs_glue_dict_list_builder_new(ubjs_library *lib, ubjs_glue_dict_builder **pthis)
{
    ubjs_glue_dict_builder *this = 0;
    ubjs_glue_dict_list_builder *data = 0;
    ubjs_library_alloc_f alloc_f;

    ubjs_library_get_alloc_f(lib, &alloc_f);
    this = (ubjs_glue_dict_builder *)(alloc_f)(sizeof(struct ubjs_glue_dict_builder));
    data = (ubjs_glue_dict_list_builder *)(alloc_f)(sizeof(
        struct ubjs_glue_dict_list_builder));
    this->lib = lib;
    this->userdata = data;
    data->value_free = 0;

    this->set_value_free_f = ubjs_glue_dict_list_builder_set_value_free;
    this->set_length_f = ubjs_glue_dict_list_builder_set_length;
    this->free_f = ubjs_glue_dict_list_builder_free;
    this->build_f = ubjs_glue_dict_list_builder_build;

    *pthis = this;
    return UR_OK;
}

ubjs_result ubjs_glue_dict_list_builder_free(ubjs_glue_dict_builder **pthis)
{
    ubjs_glue_dict_builder *this = *pthis;
    ubjs_glue_dict_list_builder *data = (ubjs_glue_dict_list_builder *)this->userdata;
    ubjs_library_free_f free_f;

    ubjs_library_get_free_f(this->lib, &free_f);
    (free_f)(data);
    (free_f)(this);
    *pthis = 0;
    return UR_OK;
}

ubjs_result ubjs_glue_dict_list_builder_set_value_free(ubjs_glue_dict_builder *this,
    ubjs_glue_value_free value_free)
{
    ubjs_glue_dict_list_builder *data = (ubjs_glue_dict_list_builder *)this->userdata;
    data->value_free = value_free;
    return UR_OK;
}

ubjs_result ubjs_glue_dict_list_builder_set_length(ubjs_glue_dict_builder *this,
    unsigned int length)
{
    return UR_OK;
}

ubjs_result ubjs_glue_dict_list_builder_build(ubjs_glue_dict_builder *this,
    ubjs_glue_dict **pdict)
{
    ubjs_glue_dict_list_builder *data = (ubjs_glue_dict_list_builder *)this->userdata;
    ubjs_glue_dict_list *list = 0;
    ubjs_glue_dict *dict = 0;
    ubjs_glue_dict_list_item *sentinel = 0;
    ubjs_library_alloc_f alloc_f;

    ubjs_library_get_alloc_f(this->lib, &alloc_f);

    list = (ubjs_glue_dict_list *)(alloc_f)(sizeof(struct ubjs_glue_dict_list));
    list->value_free = data->value_free;
    list->length = 0;

    sentinel = (ubjs_glue_dict_list_item *)(alloc_f)(sizeof(
        struct ubjs_glue_dict_list_item));
    sentinel->key_length = 0;
    sentinel->key = 0;
    sentinel->value = 0;
    sentinel->next = sentinel;
    sentinel->prev = sentinel;
    list->sentinel=sentinel;

    dict = (ubjs_glue_dict *)(alloc_f)(sizeof(struct ubjs_glue_dict));
    dict->lib = this->lib;
    dict->userdata = (void *)list;

    dict->free_f = ubjs_glue_dict_list_free;
    dict->get_length_f = ubjs_glue_dict_list_get_length;
    dict->get_f = ubjs_glue_dict_list_get;
    dict->set_f = ubjs_glue_dict_list_set;
    dict->delete_f = ubjs_glue_dict_list_delete;
    dict->iterate_f = ubjs_glue_dict_list_iterate;

    *pdict = dict;
    return UR_OK;
}

ubjs_result ubjs_glue_dict_list_free(ubjs_glue_dict **pthis)
{
    ubjs_glue_dict *this = *pthis;
    ubjs_glue_dict_list *list = (ubjs_glue_dict_list *)this->userdata;
    ubjs_glue_dict_list_item *sentinel = list->sentinel;
    ubjs_glue_dict_list_item *at = sentinel->next;
    ubjs_library_free_f free_f;

    ubjs_library_get_free_f(this->lib, &free_f);

    while (at != sentinel)
    {
        sentinel->next = at->next;
        (free_f)(at->key);
        (list->value_free)(at->value);
        (free_f)(at);
        at = sentinel->next;
    }

    (free_f)(sentinel);
    (free_f)(list);
    (free_f)(this);

    *pthis=0;
    return UR_OK;
}

ubjs_result ubjs_glue_dict_list_get_length(ubjs_glue_dict *this, unsigned int *plen)
{
    ubjs_glue_dict_list *list = (ubjs_glue_dict_list *)this->userdata;
    *plen = list->length;
    return UR_OK;
}

ubjs_result ubjs_glue_dict_list_get(ubjs_glue_dict *this, unsigned int klen,
    char *key, void **pvalue)
{
    ubjs_glue_dict_list *list = (ubjs_glue_dict_list *)this->userdata;
    ubjs_glue_dict_list_item *sentinel = list->sentinel;
    ubjs_glue_dict_list_item *at = sentinel->next;

    while (at != sentinel)
    {
        if (at->key_length == klen && 0 == strncmp(key, at->key, klen))
        {
            *pvalue = at->value;
            return UR_OK;
        }
        at = at->next;
    }
    return UR_ERROR;
}

ubjs_result ubjs_glue_dict_list_set(ubjs_glue_dict *this, unsigned int klen,
    char *key, void *value)
{
    ubjs_glue_dict_list *list = (ubjs_glue_dict_list *)this->userdata;
    ubjs_glue_dict_list_item *sentinel = list->sentinel;
    ubjs_glue_dict_list_item *at = sentinel->next;
    ubjs_glue_dict_list_item *anew = 0;
    ubjs_library_alloc_f alloc_f;

    ubjs_library_get_alloc_f(this->lib, &alloc_f);

    while (at != sentinel)
    {
        int ret;
        unsigned int common_length = klen;

        if (at->key_length < klen)
        {
            common_length = at->key_length;
        }

        ret = strncmp(key, at->key, common_length);

        if (ret < 0 || (ret == 0 && klen < common_length))
        {
            anew = (ubjs_glue_dict_list_item *)(alloc_f)(
                sizeof(struct ubjs_glue_dict_list_item));
            anew->key_length = klen;
            anew->key = (char *)(alloc_f)(sizeof(char) * klen);
            memcpy(anew->key, key, sizeof(char) * klen);
            anew->value = value;
            anew->prev = at->prev;
            anew->next = at;
            anew->prev->next=anew;
            anew->next->prev=anew;
            list->length++;
            return UR_OK;
        }
        else if (ret == 0 && at->key_length == klen)
        {
            (list->value_free)(at->value);
            at->value = value;
            return UR_OK;
        }
        at = at->next;
    }

    anew = (ubjs_glue_dict_list_item *)(alloc_f)(sizeof(
        struct ubjs_glue_dict_list_item));
    anew->key_length = klen;
    anew->key = (char *)(alloc_f)(sizeof(char) * klen);
    memcpy(anew->key, key, sizeof(char) * klen);
    anew->value = value;
    anew->prev = sentinel->prev;
    anew->next = sentinel;
    anew->prev->next=anew;
    sentinel->prev=anew;
    list->length++;
    return UR_OK;
}

ubjs_result ubjs_glue_dict_list_delete(ubjs_glue_dict *this, unsigned int klen,
    char *key)
{
    ubjs_glue_dict_list *list = (ubjs_glue_dict_list *)this->userdata;
    ubjs_glue_dict_list_item *sentinel = list->sentinel;
    ubjs_glue_dict_list_item *at = sentinel->next;
    ubjs_library_free_f free_f;

    ubjs_library_get_free_f(this->lib, &free_f);

    while (at != sentinel)
    {
        if (at->key_length == klen && 0 == strncmp(key, at->key, klen))
        {
            at->prev->next = at->next;
            at->next->prev = at->prev;

            (free_f)(at->key);
            (list->value_free)(at->value);
            (free_f)(at);
            list->length--;
            return UR_OK;
        }
        at = at->next;
    }
    return UR_ERROR;
}

ubjs_result ubjs_glue_dict_list_iterate(ubjs_glue_dict *this,
    ubjs_glue_dict_iterator **piterator)
{
    ubjs_glue_dict_list *list = (ubjs_glue_dict_list *)this->userdata;
    ubjs_glue_dict_list_item *sentinel = list->sentinel;
    ubjs_glue_dict_list_iterator *list_iterator = 0;
    ubjs_glue_dict_iterator *iterator = 0;
    ubjs_library_alloc_f alloc_f;

    ubjs_library_get_alloc_f(this->lib, &alloc_f);

    list_iterator = (ubjs_glue_dict_list_iterator *)(alloc_f)(
        sizeof(struct ubjs_glue_dict_list_iterator));
    list_iterator->at = sentinel;
    list_iterator->list = list;

    iterator=(ubjs_glue_dict_iterator *)(alloc_f)(
        sizeof(struct ubjs_glue_dict_iterator));
    iterator->object=this;
    iterator->userdata=(void *)list_iterator;

    iterator->free_f = ubjs_glue_dict_list_iterator_free;
    iterator->next_f = ubjs_glue_dict_list_iterator_next;
    iterator->get_key_length_f = ubjs_glue_dict_list_iterator_get_key_length;
    iterator->copy_key_f = ubjs_glue_dict_list_iterator_copy_key;
    iterator->get_value_f = ubjs_glue_dict_list_iterator_get_value;
    iterator->delete_f = ubjs_glue_dict_list_iterator_delete;

    *piterator=iterator;
    return UR_OK;
}

ubjs_result ubjs_glue_dict_list_iterator_free(ubjs_glue_dict_iterator **pthis)
{
    ubjs_glue_dict_iterator *this = *pthis;
    ubjs_glue_dict_list_iterator *iterator = (ubjs_glue_dict_list_iterator *)this->userdata;
    ubjs_library_free_f free_f;

    ubjs_library_get_free_f(this->object->lib, &free_f);

    (free_f)(iterator);
    (free_f)(this);

    *pthis=0;
    return UR_OK;
}

ubjs_result ubjs_glue_dict_list_iterator_next(ubjs_glue_dict_iterator *this)
{
    ubjs_glue_dict_list_iterator *iterator = (ubjs_glue_dict_list_iterator *)this->userdata;

    iterator->at = iterator->at->next;
    iterator->was_deleted = UFALSE;

    if (iterator->at == iterator->list->sentinel)
    {
        return UR_ERROR;
    }

    return UR_OK;
}

ubjs_result ubjs_glue_dict_list_iterator_get_key_length(ubjs_glue_dict_iterator *this,
    unsigned int *klen)
{
    ubjs_glue_dict_list_iterator *iterator = (ubjs_glue_dict_list_iterator *)this->userdata;
    if (iterator->at == iterator->list->sentinel || UTRUE == iterator->was_deleted)
    {
        return UR_ERROR;
    }

    *klen = iterator->at->key_length;
    return UR_OK;
}

ubjs_result ubjs_glue_dict_list_iterator_copy_key(ubjs_glue_dict_iterator *this, char *key)
{
    ubjs_glue_dict_list_iterator *iterator = (ubjs_glue_dict_list_iterator *)this->userdata;
    if (iterator->at == iterator->list->sentinel || UTRUE == iterator->was_deleted)
    {
        return UR_ERROR;
    }

    memcpy(key, iterator->at->key, sizeof(char) * iterator->at->key_length);
    return UR_OK;
}

ubjs_result ubjs_glue_dict_list_iterator_get_value(ubjs_glue_dict_iterator *this,
    void **pvalue)
{
    ubjs_glue_dict_list_iterator *iterator = (ubjs_glue_dict_list_iterator *)this->userdata;
    if (iterator->at == iterator->list->sentinel || UTRUE == iterator->was_deleted)
    {
        return UR_ERROR;
    }

    *pvalue = iterator->at->value;
    return UR_OK;
}

ubjs_result ubjs_glue_dict_list_iterator_delete(ubjs_glue_dict_iterator *this)
{
    ubjs_glue_dict_list_iterator *iterator = (ubjs_glue_dict_list_iterator *)this->userdata;
    ubjs_glue_dict_list_item *at;
    ubjs_library_free_f free_f;

    if (iterator->at == iterator->list->sentinel || UTRUE == iterator->was_deleted)
    {
        return UR_ERROR;
    }

    at = iterator->at;
    iterator->at = at->prev;

    at->prev->next = at->next;
    at->next->prev = at->prev;

    ubjs_library_get_free_f(this->object->lib, &free_f);
    (free_f)(at->key);
    (iterator->list->value_free)(at->value);
    (free_f)(at);
    iterator->list->length--;
    iterator->was_deleted = UTRUE;
    return UR_OK;

}

#pragma clang diagnostic pop