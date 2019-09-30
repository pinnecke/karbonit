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

#include "ubjs_selfemptying_list_prv.h"
#include "ubjs_common_prv.h"
#include "ubjs_library_prv.h"
#include "ubjs_glue_array_array_prv.h"

ubjs_result ubjs_selfemptying_list_new(ubjs_library *lib, ubjs_glue_value_free free_f,
    ubjs_selfemptying_list_callback callback, void *userdata, ubjs_selfemptying_list **pthis)
{
    ubjs_selfemptying_list *this = 0;
    ubjs_glue_array_builder *glue_builder;
    ubjs_library_alloc_f alloc_f;

    ubjs_library_get_alloc_f(lib, &alloc_f);
    this=(ubjs_selfemptying_list *)(alloc_f)(sizeof(struct ubjs_selfemptying_list));
    this->lib=lib;

    this->list = 0;

    ubjs_glue_array_array_builder_new(lib, &glue_builder);
    (glue_builder->set_value_free_f)(glue_builder, free_f);
    (glue_builder->build_f)(glue_builder, &(this->list));
    (glue_builder->free_f)(&glue_builder);

    this->callback=callback;
    this->is_in_callback=UFALSE;
    this->userdata=userdata;

    *pthis=this;
    return UR_OK;
}

ubjs_result ubjs_selfemptying_list_free(ubjs_selfemptying_list **pthis)
{
    ubjs_selfemptying_list *this=*pthis;
    ubjs_library_free_f free_f;

    ubjs_library_get_free_f(this->lib, &free_f);

    (this->list->free_f)(&(this->list));
    (free_f)(this);
    *pthis=0;
    return UR_OK;
}

ubjs_result ubjs_selfemptying_list_add(ubjs_selfemptying_list *this, void *obj)
{
    unsigned int len = 0;
    void *lobj = 0;

    (this->list->add_last_f)(this->list, obj);

    if (UTRUE == this->is_in_callback)
    {
        return UR_OK;
    }

    this->is_in_callback=UTRUE;
    (this->list->get_length_f)(this->list, &len);
    while (0 < len)
    {
        (this->list->get_first_f)(this->list, &lobj);
        (this->callback)(this, lobj);
        (this->list->delete_first_f)(this->list);
        (this->list->get_length_f)(this->list, &len);
    }
    this->is_in_callback=UFALSE;
    return UR_OK;
}
