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
#include <stdlib.h>

#include "ubjs_primitives_prv.h"
#include "ubjs_primitive_array_prv.h"
#include "ubjs_common_prv.h"
#include "ubjs_library_prv.h"
#include "ubjs_glue_array_array.h"

#include "ubjs_primitive_uint8.h"
#include "ubjs_primitive_int8.h"
#include "ubjs_primitive_int16.h"
#include "ubjs_primitive_int32.h"
#include "ubjs_primitive_int64.h"

ubjs_prmtv_marker ubjs_prmtv_array_marker =
{
    91,

    ubjs_prmtv_array_free,

    0,
    0,

#ifndef NDEBUG
    ubjs_prmtv_array_debug_string_get_length,
    ubjs_prmtv_array_debug_string_copy,
#else
    0,
    0,
#endif

    ubjs_prmtv_array_parser_processor_new,
    ubjs_prmtv_array_parser_processor_free,

    ubjs_prmtv_array_parser_processor_got_child,
    ubjs_prmtv_array_parser_processor_got_marker,
    ubjs_prmtv_array_parser_processor_got_control,

    0,

    ubjs_prmtv_array_writer_new,
    ubjs_prmtv_array_writer_free,
    ubjs_prmtv_array_writer_get_length,
    ubjs_prmtv_array_writer_do,

    ubjs_prmtv_array_printer_new,
    ubjs_prmtv_array_printer_free,
    ubjs_prmtv_array_printer_get_length,
    ubjs_prmtv_array_printer_do
};

ubjs_prmtv_marker ubjs_prmtv_array_end_marker =
{
    93,

    0,

    0,
    0,

    0,
    0,

    0,
    0,

    0,
    0,
    0,

    0,

    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0
};

ubjs_prmtv_marker ubjs_prmtv_array_type_marker =
{
    36,

    0,

    0,
    0,

    0,
    0,

    0,
    0,

    0,
    0,
    0,

    0,

    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0
};

ubjs_prmtv_marker ubjs_prmtv_array_count_marker =
{
    35,

    0,

    0,
    0,

    0,
    0,

    0,
    0,

    0,
    0,
    0,

    0,

    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0
};

ubjs_result ubjs_prmtv_array(ubjs_library *lib, ubjs_prmtv **pthis)
{
    ubjs_prmtv_array_t *this;
    ubjs_library_alloc_f alloc_f;
    ubjs_glue_array_builder_new_f glue_array_builder_new_f = 0;
    ubjs_glue_array_builder *glue_builder = 0;

    if (0 == lib || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(lib, &alloc_f);
    this = (ubjs_prmtv_array_t *)(alloc_f)(sizeof(struct ubjs_prmtv_array_t));
    this->super.lib = lib;
    this->super.marker = &ubjs_prmtv_array_marker;

    ubjs_library_get_glue_array_builder(lib, &glue_array_builder_new_f);
    (glue_array_builder_new_f)(lib, &glue_builder);
    (glue_builder->set_value_free_f)(glue_builder, ubjs_prmtv_glue_item_free);
    (glue_builder->build_f)(glue_builder, &(this->glue));
    (glue_builder->free_f)(&glue_builder);

    *pthis = (ubjs_prmtv *)this;
    return UR_OK;
}

ubjs_result ubjs_prmtv_array_with_length(ubjs_library *lib, unsigned int length,
    ubjs_prmtv **pthis)
{
    ubjs_prmtv_array_t *this;
    ubjs_library_alloc_f alloc_f;
    ubjs_glue_array_builder *glue_builder = 0;
    ubjs_glue_array_builder_new_f glue_array_builder_new_f = 0;

    if (0 == lib || 0 == pthis)
    {
        return UR_ERROR;
    }


    ubjs_library_get_alloc_f(lib, &alloc_f);
    this = (ubjs_prmtv_array_t *)(alloc_f)(sizeof(struct ubjs_prmtv_array_t));
    this->super.lib = lib;
    this->super.marker = &ubjs_prmtv_array_marker;

    ubjs_library_get_glue_array_builder(lib, &glue_array_builder_new_f);
    (glue_array_builder_new_f)(lib, &glue_builder);
    (glue_builder->set_value_free_f)(glue_builder, ubjs_prmtv_glue_item_free);
    (glue_builder->set_length_f)(glue_builder, length);
    (glue_builder->build_f)(glue_builder, &(this->glue));
    (glue_builder->free_f)(&glue_builder);

    *pthis=(ubjs_prmtv *)this;
    return UR_OK;
}

ubjs_result ubjs_prmtv_array_get_length(ubjs_prmtv *this, unsigned int *plen)
{
    ubjs_prmtv_array_t *athis;

    if (0 == this || &ubjs_prmtv_array_marker != this->marker || 0 == plen)
    {
        return UR_ERROR;
    }

    athis=(ubjs_prmtv_array_t *)this;
    return (athis->glue->get_length_f)(athis->glue, plen);
}

ubjs_result ubjs_prmtv_array_get_first(ubjs_prmtv *this, ubjs_prmtv **pitem)
{
    ubjs_prmtv_array_t *athis;

    if (0 == this || &ubjs_prmtv_array_marker != this->marker || 0 == pitem)
    {
        return UR_ERROR;
    }

    athis=(ubjs_prmtv_array_t *)this;
    return (athis->glue->get_first_f)(athis->glue, (void **)pitem);
}

ubjs_result ubjs_prmtv_array_get_last(ubjs_prmtv *this, ubjs_prmtv **pitem)
{
    ubjs_prmtv_array_t *athis;

    if (0 == this || &ubjs_prmtv_array_marker != this->marker || 0 == pitem)
    {
        return UR_ERROR;
    }

    athis=(ubjs_prmtv_array_t *)this;
    return (athis->glue->get_last_f)(athis->glue, (void **)pitem);
}

ubjs_result ubjs_prmtv_array_get_at(ubjs_prmtv *this, unsigned int pos, ubjs_prmtv **pitem)
{
    ubjs_prmtv_array_t *athis;

    if (0 == this || &ubjs_prmtv_array_marker != this->marker || 0 == pitem)
    {
        return UR_ERROR;
    }

    athis=(ubjs_prmtv_array_t *)this;
    return (athis->glue->get_at_f)(athis->glue, pos, (void **)pitem);
}

ubjs_result ubjs_prmtv_array_add_first(ubjs_prmtv *this, ubjs_prmtv *item)
{
    ubjs_prmtv_array_t *athis;

    if (0 == this || &ubjs_prmtv_array_marker != this->marker || 0 == item)
    {
        return UR_ERROR;
    }

    athis=(ubjs_prmtv_array_t *)this;
    return (athis->glue->add_first_f)(athis->glue, item);
}

ubjs_result ubjs_prmtv_array_add_last(ubjs_prmtv *this, ubjs_prmtv *item)
{
    ubjs_prmtv_array_t *athis;

    if (0 == this || &ubjs_prmtv_array_marker != this->marker || 0 == item)
    {
        return UR_ERROR;
    }

    athis=(ubjs_prmtv_array_t *)this;
    return (athis->glue->add_last_f)(athis->glue, item);
}

ubjs_result ubjs_prmtv_array_add_at(ubjs_prmtv *this, unsigned int pos, ubjs_prmtv *item)
{
    ubjs_prmtv_array_t *athis;

    if (0 == this || &ubjs_prmtv_array_marker != this->marker || 0 == item)
    {
        return UR_ERROR;
    }

    athis=(ubjs_prmtv_array_t *)this;
    return (athis->glue->add_at_f)(athis->glue, pos, item);
}

ubjs_result ubjs_prmtv_array_delete_first(ubjs_prmtv *this)
{
    ubjs_prmtv_array_t *athis;

    if (0 == this || &ubjs_prmtv_array_marker != this->marker)
    {
        return UR_ERROR;
    }

    athis=(ubjs_prmtv_array_t *)this;
    return (athis->glue->delete_first_f)(athis->glue);
}

ubjs_result ubjs_prmtv_array_delete_last(ubjs_prmtv *this)
{
    ubjs_prmtv_array_t *athis;

    if (0 == this || &ubjs_prmtv_array_marker != this->marker)
    {
        return UR_ERROR;
    }

    athis=(ubjs_prmtv_array_t *)this;
    return (athis->glue->delete_last_f)(athis->glue);
}

ubjs_result ubjs_prmtv_array_delete_at(ubjs_prmtv *this, unsigned int pos)
{
    ubjs_prmtv_array_t *athis;

    if (0 == this || &ubjs_prmtv_array_marker != this->marker)
    {
        return UR_ERROR;
    }

    athis=(ubjs_prmtv_array_t *)this;
    return (athis->glue->delete_at_f)(athis->glue, pos);
}

ubjs_result ubjs_prmtv_array_iterate(ubjs_prmtv *this, ubjs_array_iterator **piterator)
{
    ubjs_prmtv_array_t *athis;
    ubjs_array_iterator *iterator;
    ubjs_library_alloc_f alloc_f;

    if (0 == this || &ubjs_prmtv_array_marker != this->marker || 0 == piterator)
    {
        return UR_ERROR;
    }

    athis=(ubjs_prmtv_array_t *)this;
    ubjs_library_get_alloc_f(athis->super.lib, &alloc_f);
    iterator=(ubjs_array_iterator *)(alloc_f)(sizeof(struct ubjs_array_iterator));
    iterator->array=athis;
    iterator->glue=0;
    (athis->glue->iterate_f)(athis->glue, &(iterator->glue));

    *piterator = iterator;
    return UR_OK;
}

ubjs_result ubjs_array_iterator_next(ubjs_array_iterator *this)
{
    if (0 == this)
    {
        return UR_ERROR;
    }

    return (this->glue->next_f)(this->glue);
}

ubjs_result ubjs_array_iterator_get(ubjs_array_iterator *this, ubjs_prmtv **pitem)
{
    if (0 == this || 0 == pitem)
    {
        return UR_ERROR;
    }

    return (this->glue->get_f)(this->glue, (void **)pitem);
}

ubjs_result ubjs_array_iterator_delete(ubjs_array_iterator *this)
{
    if (0 == this)
    {
        return UR_ERROR;
    }

    return (this->glue->delete_f)(this->glue);
}

ubjs_result ubjs_array_iterator_free(ubjs_array_iterator **pthis)
{
    ubjs_array_iterator *this;
    ubjs_library_free_f free_f;

    if (0 == pthis || 0 == *pthis)
    {
        return UR_ERROR;
    }

    this=*pthis;
    (this->glue->free_f)(&(this->glue));
    ubjs_library_get_free_f(this->array->super.lib, &free_f);
    (free_f)(*pthis);
    *pthis=0;
    return UR_OK;
}

ubjs_result ubjs_prmtv_array_free(ubjs_prmtv **pthis)
{
    ubjs_prmtv_array_t *this;
    ubjs_library_free_f free_f;

    if (0 == pthis || 0 == *pthis)
    {
        return UR_ERROR;
    }

    this = (ubjs_prmtv_array_t *)*pthis;
    ubjs_library_get_free_f(this->super.lib, &free_f);
    (this->glue->free_f)(&(this->glue));
    (free_f)(this);
    *pthis = 0;
    return UR_OK;
}

#ifndef NDEBUG

ubjs_result ubjs_prmtv_array_debug_string_get_length(ubjs_prmtv *this, unsigned int *plen)
{
    ubjs_prmtv_array_t *thisv;
    char tmp[30];
    unsigned int len = 0;
    ubjs_glue_array_iterator *it = 0;

    if (0 == this || 0 == plen)
    {
        return UR_ERROR;
    }

    thisv = (ubjs_prmtv_array_t *)this;
    (thisv->glue->get_length_f)(thisv->glue, &len);

    /* "str(%u, " + %s + ")"*/
    *plen = sprintf(tmp, "array(%u)", len);

    if (0 < len)
    {
        (thisv->glue->iterate_f)(thisv->glue, &it);
        while (UR_OK == (it->next_f)(it))
        {
            unsigned int itlen = 0;
            ubjs_prmtv *child = 0;

            (it->get_f)(it, (void **)&child);
            ubjs_prmtv_debug_string_get_length(child, &itlen);
            *plen += 2 + itlen;
        }
        (it->free_f)(&it);
    }
    return UR_OK;
}

ubjs_result ubjs_prmtv_array_debug_string_copy(ubjs_prmtv *this, char *str)
{
    ubjs_prmtv_array_t *thisv;
    unsigned int len = 0;
    unsigned int at = 0;
    ubjs_glue_array_iterator *it = 0;

    if (0 == this || 0 == str)
    {
        return UR_ERROR;
    }

    thisv = (ubjs_prmtv_array_t *)this;
    (thisv->glue->get_length_f)(thisv->glue, &len);

    at = sprintf(str + at, "array(%u", len);

    if (0 < len)
    {
        (thisv->glue->iterate_f)(thisv->glue, &it);
        while (UR_OK == (it->next_f)(it))
        {
            unsigned int itlen = 0;
            ubjs_prmtv *child = 0;

            (it->get_f)(it, (void **)&child);
            at += sprintf(str + at, ", ");

            ubjs_prmtv_debug_string_get_length(child, &itlen);
            ubjs_prmtv_debug_string_copy(child, str + at);

            at += itlen;
        }
        (it->free_f)(&it);
    }
    at += sprintf(str + at, ")");

    return UR_OK;
}
#endif

ubjs_result ubjs_prmtv_array_parser_processor_new(ubjs_library *lib,
     ubjs_prmtv_marker_parser_glue *glue, ubjs_prmtv_marker_parser_processor **pthis)
{
    ubjs_prmtv_array_parser_processor *this;
    ubjs_library_alloc_f alloc_f;

    if (0 == lib || 0 == glue || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(lib, &alloc_f);
    this = (ubjs_prmtv_array_parser_processor *)(alloc_f)(sizeof(
        struct ubjs_prmtv_array_parser_processor));
    this->super.lib = lib;
    this->super.marker = &ubjs_prmtv_array_marker;
    this->super.name = "array";
    this->super.glue = glue;
    this->data = 0;
    this->phase = UPAPPP_INIT;
    this->len = 0;
    this->expected_len = 0;
    this->type_marker = 0;
    this->count_marker = 0;

    this->legal_markers_type_count_top_end = 0;
    this->legal_markers_count = 0;
    this->legal_markers_int64s = 0;
    this->legal_markers_top = 0;
    this->legal_markers_top_end = 0;

    {
        ubjs_glue_array_builder *glue_builder = 0;

        ubjs_glue_array_array_builder_new(lib, &glue_builder);
        (glue_builder->set_value_free_f)(glue_builder, 0);
        (glue_builder->build_f)(glue_builder, &(this->legal_markers_type_count_top_end));
        (glue_builder->build_f)(glue_builder, &(this->legal_markers_count));
        (glue_builder->build_f)(glue_builder, &(this->legal_markers_int64s));
        (glue_builder->build_f)(glue_builder, &(this->legal_markers_top));
        (glue_builder->build_f)(glue_builder, &(this->legal_markers_top_end));
        (glue_builder->free_f)(&glue_builder);
    }
    {
        ubjs_glue_array_iterator *it = 0;
        ubjs_glue_array *markers = 0;
        ubjs_prmtv_marker *n = 0;

        ubjs_library_get_markers(lib, &markers);
        (markers->iterate_f)(markers, &it);

        while (UR_OK == (it->next_f)(it))
        {
            (it->get_f)(it, (void **)&n);

            (this->legal_markers_type_count_top_end->add_last_f)(
                this->legal_markers_type_count_top_end, n);
            (this->legal_markers_top->add_last_f)(
                this->legal_markers_top, n);
            (this->legal_markers_top_end->add_last_f)(
                this->legal_markers_top_end, n);
            if (0 != n->new_from_int64_f)
            {
                (this->legal_markers_int64s->add_last_f)(this->legal_markers_int64s, n);
            }
        }
        (it->free_f)(&it);

        (this->legal_markers_type_count_top_end->add_last_f)(
            this->legal_markers_type_count_top_end, &ubjs_prmtv_array_type_marker);

        (this->legal_markers_type_count_top_end->add_last_f)(
            this->legal_markers_type_count_top_end, &ubjs_prmtv_array_count_marker);
        (this->legal_markers_count->add_last_f)(
            this->legal_markers_count, &ubjs_prmtv_array_count_marker);

        (this->legal_markers_type_count_top_end->add_last_f)(
            this->legal_markers_type_count_top_end, &ubjs_prmtv_array_end_marker);
        (this->legal_markers_top_end->add_last_f)(
            this->legal_markers_top_end, &ubjs_prmtv_array_end_marker);

    }
    *pthis = (ubjs_prmtv_marker_parser_processor *)this;
    return UR_OK;
}

ubjs_result ubjs_prmtv_array_parser_processor_free(
    ubjs_prmtv_marker_parser_processor **pthis)
{
    ubjs_prmtv_array_parser_processor *this;
    ubjs_library_free_f free_f;

    if (0 == pthis || 0 == *pthis)
    {
        return UR_ERROR;
    }

    this = (ubjs_prmtv_array_parser_processor *)*pthis;
    ubjs_library_get_free_f(this->super.lib, &free_f);
    if (0 != this->data)
    {
        ubjs_prmtv_free(&(this->data));
    }

    (this->legal_markers_type_count_top_end->free_f)(&(this->legal_markers_type_count_top_end));
    (this->legal_markers_count->free_f)(&(this->legal_markers_count));
    (this->legal_markers_int64s->free_f)(&(this->legal_markers_int64s));
    (this->legal_markers_top->free_f)(&(this->legal_markers_top));
    (this->legal_markers_top_end->free_f)(&(this->legal_markers_top_end));

    (free_f)(this);
    *pthis = 0;
    return UR_OK;
}

static void ubjs_prmtv_array_parser_processor_got_child_want_count_prmtv_value(
    ubjs_prmtv_marker_parser_processor *this, ubjs_prmtv *present)
{
    ubjs_prmtv_array_parser_processor *this2 = (ubjs_prmtv_array_parser_processor *)this;
    int64_t len = 0;

    ubjs_prmtv_int_get(present, &len);
    ubjs_prmtv_free(&present);
    if (0 > len)
    {
        this2->phase = UPAPPP_DONE;
        (this->glue->error_f)(this->glue, 14,
            "Invalid length");
        return;
    }

    this2->expected_len = len;
    ubjs_prmtv_free(&present);

    if (this->glue->limit_container_length > 0 &&
        this->glue->limit_container_length < len)
    {
        this2->phase = UPAPPP_DONE;
        (this->glue->error_f)(this->glue, 33,
            "Reached limit of container length");
    }
    else if (0 < len && this->glue->limit_recursion_level > 0 &&
        this->glue->limit_recursion_level == this->glue->recursion_level)
    {
        this2->phase = UPAPPP_DONE;
        (this->glue->error_f)(this->glue, 29,
            "Reached recursion level limit");
    }
    else
    {
        ubjs_prmtv_array_with_length(this->lib, len, &(this2->data));
        if (0 == len)
        {
            ubjs_prmtv *data = this2->data;

            this2->data = 0;
            this2->phase = UPAPPP_DONE;
            (this->glue->return_control_f)(this->glue, data);
        }
        else if (0 != this2->type_marker)
        {
            this2->phase = UPAPPP_WANT_CHILD_PRMTV_VALUE;
            (this->glue->want_child_f)(this->glue, this2->type_marker);
        }
        else
        {
            this2->phase = UPAPPP_WANT_CHILD_PRMTV_MARKER;
            (this->glue->want_marker_f)(this->glue, this2->legal_markers_top);
        }
    }
}

void ubjs_prmtv_array_parser_processor_got_child(
    ubjs_prmtv_marker_parser_processor *this, ubjs_prmtv *present)
{
    ubjs_prmtv_array_parser_processor *this2 = (ubjs_prmtv_array_parser_processor *)this;
    switch (this2->phase)
    {
        case UPAPPP_WANT_COUNT_PRMTV_VALUE:
            ubjs_prmtv_array_parser_processor_got_child_want_count_prmtv_value(this, present);
            break;

        case UPAPPP_WANT_CHILD_PRMTV_VALUE:
            ubjs_prmtv_array_add_last(this2->data, present);
            this2->len += 1;

            if (0 != this2->count_marker && this2->len == this2->expected_len)
            {
                ubjs_prmtv *data = this2->data;

                this2->data = 0;
                this2->phase = UPAPPP_DONE;
                (this->glue->return_control_f)(this->glue, data);
            }
            else
            {
                if (0 != this2->type_marker)
                {
                    this2->phase = UPAPPP_WANT_CHILD_PRMTV_VALUE;
                    (this->glue->want_child_f)(this->glue, this2->type_marker);
                }
                else if (0 != this2->count_marker)
                {
                    this2->phase = UPAPPP_WANT_CHILD_PRMTV_MARKER;
                    (this->glue->want_marker_f)(this->glue, this2->legal_markers_top);
                }
                else
                {
                    this2->phase = UPAPPP_WANT_CHILD_PRMTV_MARKER;
                    (this->glue->want_marker_f)(this->glue, this2->legal_markers_top_end);
                }
            }

            break;

        default:
            this2->phase = UPAPPP_DONE;
            (this->glue->error_f)(this->glue, 22,
                "Unexpected got present");
            break;
    }
}

static void ubjs_prmtv_array_parser_processor_got_marker_want_type_count_top_end_marker(
    ubjs_prmtv_marker_parser_processor *this, ubjs_prmtv_marker *marker)
{
    ubjs_prmtv_array_parser_processor *this2 = (ubjs_prmtv_array_parser_processor *)this;
    if (&ubjs_prmtv_array_type_marker == marker)
    {
        this2->phase = UPAPPP_WANT_TYPE_MARKER;
        (this->glue->want_marker_f)(this->glue, this2->legal_markers_top);
    }
    else if (&ubjs_prmtv_array_end_marker == marker)
    {
        ubjs_prmtv *data = 0;
        ubjs_prmtv_array(this->lib, &(data));
        this2->phase = UPAPPP_DONE;
        (this->glue->return_control_f)(this->glue, data);
    }
    else if (&ubjs_prmtv_array_count_marker == marker)
    {
        this2->phase = UPAPPP_WANT_COUNT_PRMTV_MARKER;
        (this->glue->want_marker_f)(this->glue, this2->legal_markers_int64s);
    }
    else if (this->glue->limit_recursion_level > 0 &&
        this->glue->limit_recursion_level == this->glue->recursion_level)
    {
        this2->phase = UPAPPP_DONE;
        (this->glue->error_f)(this->glue, 29,
            "Reached recursion level limit");
    }
    else
    {
        this2->phase = UPAPPP_WANT_CHILD_PRMTV_VALUE;
        ubjs_prmtv_array(this->lib, &(this2->data));
        (this->glue->want_child_f)(this->glue, marker);
    }

}

void ubjs_prmtv_array_parser_processor_got_marker(
    ubjs_prmtv_marker_parser_processor *this, ubjs_prmtv_marker *marker)
{
    ubjs_prmtv_array_parser_processor *this2 = (ubjs_prmtv_array_parser_processor *)this;

    switch (this2->phase)
    {
        case UPAPPP_WANT_TYPE_COUNT_TOP_END_MARKER:
            ubjs_prmtv_array_parser_processor_got_marker_want_type_count_top_end_marker(
                this, marker);
            break;

        case UPAPPP_WANT_TYPE_MARKER:
            this2->phase = UPAPPP_WANT_COUNT_MARKER;
            this2->type_marker = marker;
            (this->glue->want_marker_f)(this->glue, this2->legal_markers_count);
            break;

        case UPAPPP_WANT_COUNT_MARKER:
            (this->glue->want_marker_f)(this->glue, this2->legal_markers_int64s);
            this2->phase = UPAPPP_WANT_COUNT_PRMTV_MARKER;
            break;

        case UPAPPP_WANT_COUNT_PRMTV_MARKER:
            this2->count_marker = marker;
            this2->phase = UPAPPP_WANT_COUNT_PRMTV_VALUE;
            (this->glue->want_child_f)(this->glue, this2->count_marker);
            break;

        case UPAPPP_WANT_CHILD_PRMTV_MARKER:
            if (&ubjs_prmtv_array_end_marker == marker)
            {
                ubjs_prmtv *data = this2->data;
                this2->data = 0;
                this2->phase = UPAPPP_DONE;
                (this->glue->return_control_f)(this->glue, data);

            }
            else if (this->glue->limit_container_length > 0 &&
                this->glue->limit_container_length == this2->len)
            {
                this2->phase = UPAPPP_DONE;
                (this->glue->error_f)(this->glue, 33,
                    "Reached limit of container length");
            }
            else
            {
                this2->phase = UPAPPP_WANT_CHILD_PRMTV_VALUE;
                (this->glue->want_child_f)(this->glue, marker);
            }
            break;

        default:
            this2->phase = UPAPPP_DONE;
            (this->glue->error_f)(this->glue, 21,
                "Unexpected got marker");
    }
}

void ubjs_prmtv_array_parser_processor_got_control(
    ubjs_prmtv_marker_parser_processor *this)
{
    ubjs_prmtv_array_parser_processor *this2 = (ubjs_prmtv_array_parser_processor *)this;
    switch (this2->phase)
    {
        case UPAPPP_INIT:
            this2->phase = UPAPPP_WANT_TYPE_COUNT_TOP_END_MARKER;
            (this->glue->want_marker_f)(this->glue, this2->legal_markers_type_count_top_end);
            break;

        default:
            this2->phase = UPAPPP_DONE;
            (this->glue->error_f)(this->glue, 22,
                "Unexpected got control");
            break;
    }
}

static void ubjs_prmtv_array_writer_try_upgrades(ubjs_library *lib, unsigned int len,
    ubjs_prmtv **items, ubjs_bool *got_upgraded)
{
    ubjs_library_alloc_f alloc_f = 0;
    unsigned int i = 0;
    unsigned int num_i8 = 0;
    unsigned int num_i16 = 0;
    unsigned int num_i32 = 0;
    unsigned int num_i64 = 0;
    unsigned int num_rest = 0;
    unsigned int digits = ubjs_digits(len);
    ubjs_prmtv_marker *nmarker = 0;
    enum upgrade_to
    {
        DONT,
        TO16,
        TO32,
        TO64
    } upgrade_to;

    if (len < 2)
    {
        return;
    }

    ubjs_library_get_alloc_f(lib, &alloc_f);

    for (i = 0; i < len; i++)
    {
        ubjs_prmtv *child = items[i];
        ubjs_prmtv_marker *marker = 0;

        ubjs_prmtv_get_marker(child, &marker);

        if (&ubjs_prmtv_uint8_marker == marker || &ubjs_prmtv_int8_marker == marker)
        {
            num_i8++;
        }
        else if (&ubjs_prmtv_int16_marker == marker)
        {
            num_i16++;
        }
        else if (&ubjs_prmtv_int32_marker == marker)
        {
            num_i32++;
        }
        else if (&ubjs_prmtv_int64_marker == marker)
        {
            num_i64++;
        }
        else
        {
            num_rest++;
        }
    }

    if (num_rest != 0)
    {
        return;
    }

    if (num_i64 > 0 && num_i64 != len &&
        num_i64 >= 3 + 3 * num_i8 + 5 * num_i16 + 6 * num_i32 + digits)
    {
         upgrade_to = TO64;
         nmarker = &ubjs_prmtv_int64_marker;
    }
    else if (num_i64 > 0)
    {
        return;
    }
    else if (num_i32 > 0 && num_i32 != len &&
        num_i32 >= 3 + num_i8 + 2 * num_i16 + digits)
    {
         upgrade_to = TO32;
         nmarker = &ubjs_prmtv_int32_marker;
    }
    else if (num_i32 > 0)
    {
        return;
    }
    else if (num_i16 > 0 && num_i8 > 0 && num_i16 >= 3 + digits)
    {
         upgrade_to = TO16;
         nmarker = &ubjs_prmtv_int16_marker;
    }
    else
    {
        return;
    }

    for (i = 0; i < len; i++)
    {
        ubjs_prmtv *child = items[i];
        ubjs_prmtv *nchild = 0;
        ubjs_prmtv_marker *marker = 0;
        ubjs_prmtv_get_marker(child, &marker);
        int64_t v = 0;

        ubjs_prmtv_int_get(child, &v);
        if ((upgrade_to == TO64 && &ubjs_prmtv_int64_marker == marker) ||
            (upgrade_to == TO32 && &ubjs_prmtv_int32_marker == marker) ||
            (upgrade_to == TO16 && &ubjs_prmtv_int16_marker == marker))
        {
            continue;
        }

        (nmarker->new_from_int64_f)(lib, v, &nchild);
        items[i] = nchild;
        got_upgraded[i] = UTRUE;
    }
}

static void ubjs_prmtv_array_writer_calculate_lenghts(ubjs_prmtv_array_writer *this)
{
    ubjs_library_alloc_f alloc_f = 0;
    unsigned int i = 0;

    ubjs_library_get_alloc_f(this->super.lib, &alloc_f);

    for (i = 0; i < this->len; i++)
    {
        ubjs_prmtv *child = this->item_prmtvs[i];
        ubjs_prmtv_marker *cmarker = 0;
        ubjs_prmtv_marker_writer *child_writer = 0;
        ubjs_prmtv_marker_writer_glue *child_glue = 0;

        child_glue = (ubjs_prmtv_marker_writer_glue *)(alloc_f)(sizeof(
            struct ubjs_prmtv_marker_writer_glue));
        memset(child_glue, 0, sizeof(struct ubjs_prmtv_marker_writer_glue));
        child_glue->debug_f = 0;
        child_glue->prmtv = child;

        ubjs_prmtv_get_marker(child, &cmarker);
        (cmarker->writer_new_f)(this->super.lib, child_glue, &(child_writer));

        this->item_writers[i] = child_writer;
        this->item_writers_glues[i] = child_glue;
        (cmarker->writer_get_length_f)(child_writer, this->item_lengths + i);

        if (this->len >= 3 && i == 0)
        {
            this->type_marker = cmarker;
        }
        else if (this->type_marker != 0 && cmarker != this->type_marker)
        {
            this->type_marker = 0;
        }
    }

    if (this->len >= 3)
    {
        ubjs_prmtv *count = 0;
        ubjs_prmtv_marker *cmarker = 0;
        ubjs_prmtv_marker_writer_glue *count_glue = 0;
        ubjs_prmtv_marker_writer *count_writer = 0;

        count_glue = (ubjs_prmtv_marker_writer_glue *)(alloc_f)(sizeof(
            struct ubjs_prmtv_marker_writer_glue));
        count_glue->debug_f = 0;

        ubjs_prmtv_int(this->super.lib, i, &count);
        count_glue->prmtv = count;

        ubjs_prmtv_get_marker(count, &cmarker);
        (cmarker->writer_new_f)(this->super.lib, count_glue, &(count_writer));

        this->count_writer = count_writer;
        this->count_writer_glue = count_glue;
        (cmarker->writer_get_length_f)(count_writer, &(this->count_length));
    }
}

ubjs_result ubjs_prmtv_array_writer_new(ubjs_library *lib,
    ubjs_prmtv_marker_writer_glue *glue, ubjs_prmtv_marker_writer **pthis)
{
    ubjs_prmtv_array_writer *this;
    ubjs_library_alloc_f alloc_f = 0;
    ubjs_prmtv_array_t *prmtv = 0;
    unsigned int len = 0;

    if (0 == lib || 0 == glue || 0 == glue->prmtv || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(lib, &alloc_f);
    prmtv = (ubjs_prmtv_array_t *)glue->prmtv;

    this = (ubjs_prmtv_array_writer *)(alloc_f)(sizeof(
        struct ubjs_prmtv_array_writer));
    this->super.lib = lib;
    this->super.marker = &ubjs_prmtv_array_marker;
    this->super.name = "array";
    this->super.glue = glue;

    this->type_marker = 0;
    this->count_writer_glue = 0;
    this->count_writer = 0;

    (prmtv->glue->get_length_f)(prmtv->glue, &len);
    this->len = len;

    this->item_prmtvs = (ubjs_prmtv **)(alloc_f)(sizeof(ubjs_prmtv *) * len);
    this->item_got_upgraded = (ubjs_bool *)(alloc_f)(sizeof(ubjs_bool) * len);
    this->item_writers = (ubjs_prmtv_marker_writer **)(alloc_f)(
        sizeof(struct ubjs_prmtv_marker_writer *) * len);
    this->item_lengths = (unsigned int *)(alloc_f)(
        sizeof(unsigned int) * len);
    this->item_writers_glues = (ubjs_prmtv_marker_writer_glue **)(alloc_f)(
        sizeof(struct ubjs_prmtv_marker_writer_glue *) * len);

    if (0 != this->len)
    {
        ubjs_glue_array_iterator *it = 0;
        unsigned int i = 0;
        memset(this->item_got_upgraded, UFALSE, len * sizeof(ubjs_bool));
        memset(this->item_prmtvs, 0, len * sizeof(struct ubjs_prmtv *));
        (prmtv->glue->iterate_f)(prmtv->glue, &it);
        for (i = 0; i < this->len && UR_OK == (it->next_f)(it); i++)
        {
            (it->get_f)(it, (void *)&(this->item_prmtvs[i]));
        }
        (it->free_f)(&it);

        ubjs_prmtv_array_writer_try_upgrades(this->super.lib, this->len, this->item_prmtvs,
            this->item_got_upgraded);
        ubjs_prmtv_array_writer_calculate_lenghts(this);
    }

    *pthis = (ubjs_prmtv_marker_writer *)this;
    return UR_OK;
}

ubjs_result ubjs_prmtv_array_writer_free(ubjs_prmtv_marker_writer **pthis)
{
    ubjs_prmtv_array_writer *this;
    ubjs_library_free_f free_f;
    ubjs_prmtv_marker *amarker = 0;
    unsigned int i = 0;

    if (0 == pthis || 0 == *pthis)
    {
        return UR_ERROR;
    }

    this = (ubjs_prmtv_array_writer *)*pthis;
    ubjs_library_get_free_f(this->super.lib, &free_f);

    if (0 != this->count_writer)
    {
        ubjs_prmtv_get_marker(this->count_writer->glue->prmtv, &amarker);
        ubjs_prmtv_free(&(this->count_writer->glue->prmtv));
        (free_f)(this->count_writer->glue);
        (amarker->writer_free_f)(&this->count_writer);
    }

    for (i = 0; i < this->len; i++)
    {
        if (UTRUE == this->item_got_upgraded[i])
        {
            ubjs_prmtv_free(&(this->item_prmtvs[i]));
        }
        (free_f)(this->item_writers_glues[i]);
        (this->item_writers[i]->marker->writer_free_f)(&(this->item_writers[i]));
    }

    (free_f)(this->item_prmtvs);
    (free_f)(this->item_got_upgraded);
    (free_f)(this->item_writers);
    (free_f)(this->item_lengths);
    (free_f)(this->item_writers_glues);
    (free_f)(this);
    *pthis = 0;
    return UR_OK;
}

void ubjs_prmtv_array_writer_get_length(ubjs_prmtv_marker_writer *this,
    unsigned int *plen)
{
    ubjs_prmtv_array_writer *this2 = (ubjs_prmtv_array_writer *)this;;
    *plen = 0;

    if (this2->type_marker != 0)
    {
        /* # + marker */
        *plen += 2;
    }

    if (this2->count_writer != 0)
    {
        /* $ + marker + prmtv */
        *plen += 2 + this2->count_length;
    }

    if (this2->type_marker == 0 && this2->count_writer == 0)
    {
        /* ]*/
        *plen += 1;
    }

    {
        unsigned int i = 0;
        for (i = 0; i < this2->len; i++)
        {
            /* marker + prmtv */
            *plen += this2->item_lengths[i] + (this2->type_marker != 0 ? 0 : 1);
        }
    }
}

void ubjs_prmtv_array_writer_do(ubjs_prmtv_marker_writer *this, uint8_t *data)
{
    ubjs_prmtv_array_writer *this2 = (ubjs_prmtv_array_writer *)this;;
    unsigned int at = 0;

    if (this2->type_marker != 0)
    {
        /* # + marker */
        data[at++] = 36;
        data[at++] = this2->type_marker->abyte;
    }

    if (this2->count_writer != 0)
    {
        unsigned int len = 0;
        /* $ + marker + prmtv */

        data[at++] = 35;
        data[at++] = this2->count_writer->marker->abyte;

        (this2->count_writer->marker->writer_get_length_f)(this2->count_writer, &len);
        (this2->count_writer->marker->writer_do_f)(this2->count_writer, data + at);
        at += len;
    }

    {
        unsigned int i = 0;
        for (i = 0; i < this2->len; i++)
        {
            unsigned int len = 0;
            ubjs_prmtv_marker_writer *child_writer = this2->item_writers[i];

            (child_writer->marker->writer_get_length_f)(child_writer, &len);

            if (0 == this2->type_marker)
            {
                data[at++] = child_writer->marker->abyte;
            }
            (child_writer->marker->writer_do_f)(child_writer, data + at);
            at += len;
        }
    }
    if (this2->type_marker == 0 && this2->count_writer == 0)
    {
        /* ]*/
        data[at] = 93;
    }
}

static void ubjs_prmtv_array_printer_calculate_lenghts(ubjs_prmtv_array_printer *this)
{
    ubjs_library_alloc_f alloc_f = 0;
    unsigned int i = 0;

    ubjs_library_get_alloc_f(this->super.lib, &alloc_f);

    for (i = 0; i < this->len; i++)
    {
        ubjs_prmtv *child = this->item_prmtvs[i];
        ubjs_prmtv_marker *cmarker = 0;
        ubjs_prmtv_marker_printer *child_printer = 0;
        ubjs_prmtv_marker_printer_glue *child_glue = 0;

        child_glue = (ubjs_prmtv_marker_printer_glue *)(alloc_f)(sizeof(
            struct ubjs_prmtv_marker_printer_glue));
        memset(child_glue, 0, sizeof(struct ubjs_prmtv_marker_printer_glue));
        child_glue->debug_f = 0;
        child_glue->indent = this->super.glue->indent + 1;

        child_glue->prmtv = child;

        ubjs_prmtv_get_marker(child, &cmarker);
        (cmarker->printer_new_f)(this->super.lib, child_glue, &(child_printer));

        this->item_printers[i] = child_printer;
        this->item_printers_glues[i] = child_glue;
        (cmarker->printer_get_length_f)(child_printer, this->item_lengths + i);

        if (this->len >= 3 && i == 0)
        {
            this->type_marker = cmarker;
        }
        else if (this->type_marker != 0 && cmarker != this->type_marker)
        {
            this->type_marker = 0;
        }
    }


    if (this->len >= 3)
    {
        ubjs_prmtv *count = 0;
        ubjs_prmtv_marker *cmarker = 0;
        ubjs_prmtv_marker_printer_glue *count_glue = 0;
        ubjs_prmtv_marker_printer *count_printer = 0;

        count_glue = (ubjs_prmtv_marker_printer_glue *)(alloc_f)(sizeof(
            struct ubjs_prmtv_marker_printer_glue));
        count_glue->debug_f = 0;

        ubjs_prmtv_int(this->super.lib, i, &count);
        count_glue->prmtv = count;

        ubjs_prmtv_get_marker(count, &cmarker);
        (cmarker->printer_new_f)(this->super.lib, count_glue, &(count_printer));

        this->count_printer = count_printer;
        this->count_printer_glue = count_glue;
        (cmarker->printer_get_length_f)(count_printer, &(this->count_length));
    }
}

ubjs_result ubjs_prmtv_array_printer_new(ubjs_library *lib,
    ubjs_prmtv_marker_printer_glue *glue, ubjs_prmtv_marker_printer **pthis)
{
    ubjs_prmtv_array_printer *this;
    ubjs_library_alloc_f alloc_f;
    ubjs_prmtv_array_t *prmtv = 0;
    unsigned int len;

    if (0 == lib || 0 == glue || 0 == glue->prmtv || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(lib, &alloc_f);
    prmtv = (ubjs_prmtv_array_t *)glue->prmtv;

    this = (ubjs_prmtv_array_printer *)(alloc_f)(sizeof(
        struct ubjs_prmtv_array_printer));
    this->super.lib = lib;
    this->super.marker = &ubjs_prmtv_array_marker;
    this->super.name = "array";
    this->super.glue = glue;
    this->type_marker = 0;
    this->count_printer_glue = 0;
    this->count_printer = 0;
    this->len = 0;
    this->item_printers = 0;
    this->item_lengths = 0;
    this->item_printers_glues = 0;

    (prmtv->glue->get_length_f)(prmtv->glue, &len);
    this->len = len;

    this->item_prmtvs = (ubjs_prmtv **)(alloc_f)(sizeof(ubjs_prmtv *) * len);
    this->item_got_upgraded = (ubjs_bool *)(alloc_f)(sizeof(ubjs_bool) * len);
    this->item_printers = (ubjs_prmtv_marker_printer **)(alloc_f)(
        sizeof(struct ubjs_prmtv_marker_printer *) * len);
    this->item_lengths = (unsigned int *)(alloc_f)(
        sizeof(unsigned int) * len);
    this->item_printers_glues = (ubjs_prmtv_marker_printer_glue **)(alloc_f)(
        sizeof(struct ubjs_prmtv_marker_printer_glue *) * len);

    if (0 != this->len)
    {
        ubjs_glue_array_iterator *it = 0;
        unsigned int i = 0;
        memset(this->item_got_upgraded, UFALSE, len * sizeof(ubjs_bool));
        (prmtv->glue->iterate_f)(prmtv->glue, &it);
        for (i = 0; i < this->len && UR_OK == (it->next_f)(it); i++)
        {
            (it->get_f)(it, (void *)&(this->item_prmtvs[i]));
        }
        (it->free_f)(&it);

        ubjs_prmtv_array_writer_try_upgrades(this->super.lib, len, this->item_prmtvs,
            this->item_got_upgraded);
        ubjs_prmtv_array_printer_calculate_lenghts(this);
    }

    *pthis = (ubjs_prmtv_marker_printer *)this;
    return UR_OK;
}

ubjs_result ubjs_prmtv_array_printer_free(ubjs_prmtv_marker_printer **pthis)
{
    ubjs_prmtv_array_printer *this;
    ubjs_library_free_f free_f;
    ubjs_prmtv_marker *amarker = 0;
    unsigned int i = 0;

    if (0 == pthis || 0 == *pthis)
    {
        return UR_ERROR;
    }

    this = (ubjs_prmtv_array_printer *)*pthis;
    ubjs_library_get_free_f(this->super.lib, &free_f);

    if (0 != this->count_printer)
    {
        ubjs_prmtv_get_marker(this->count_printer->glue->prmtv, &amarker);
        ubjs_prmtv_free(&(this->count_printer->glue->prmtv));
        (free_f)(this->count_printer->glue);
        (amarker->printer_free_f)(&this->count_printer);
    }

    for (i = 0; i < this->len; i++)
    {
        if (UTRUE == this->item_got_upgraded[i])
        {
            ubjs_prmtv_free(&(this->item_prmtvs[i]));
        }
        (free_f)(this->item_printers_glues[i]);
        (this->item_printers[i]->marker->printer_free_f)(&(this->item_printers[i]));
    }
    (free_f)(this->item_prmtvs);
    (free_f)(this->item_got_upgraded);
    (free_f)(this->item_printers);
    (free_f)(this->item_lengths);
    (free_f)(this->item_printers_glues);
    (free_f)(this);
    *pthis = 0;
    return UR_OK;
}

void ubjs_prmtv_array_printer_get_length(ubjs_prmtv_marker_printer *this,
    unsigned int *plen)
{
    ubjs_prmtv_array_writer *this2 = (ubjs_prmtv_array_writer *)this;
    unsigned int i = 0;
    *plen = 0;

    if (this2->type_marker != 0)
    {
        /* [#] + [marker] */
        *plen += 6;
    }

    if (this2->count_writer != 0)
    {
        /* [$] + [marker] + prmtv */
        *plen += 6 + this2->count_length;
    }

    for (i = 0; i < this2->len; i++)
    {
        if (0 < this2->item_lengths[i] || 0 == this2->type_marker)
        {
            /* \n + indent + [marker] + prmtv */
            *plen += 5 + this->glue->indent * 4 + this2->item_lengths[i] +
                (this2->type_marker != 0 ? 0 : 3);
        }
    }

    if (this2->type_marker == 0 && this2->count_writer == 0)
    {
        /* \n + indent + []] */
        *plen += 3 + (this2->len > 0 ? 1 + (this->glue->indent) * 4 : 0);
    }
}

void ubjs_prmtv_array_printer_do(ubjs_prmtv_marker_printer *this, char *data)
{
    ubjs_prmtv_array_printer *this2 = (ubjs_prmtv_array_printer *)this;;
    unsigned int at = 0;
    unsigned int i = 0;

    if (this2->type_marker != 0)
    {
        /* # + marker */
        data[at++] = '[';
        data[at++] = 36;
        data[at++] = ']';
        data[at++] = '[';
        data[at++] = this2->type_marker->abyte;
        data[at++] = ']';
    }

    if (this2->count_printer != 0)
    {
        unsigned int len = 0;
        /* $ + marker + prmtv */

        data[at++] = '[';
        data[at++] = 35;
        data[at++] = ']';
        data[at++] = '[';
        data[at++] = this2->count_printer->marker->abyte;
        data[at++] = ']';

        (this2->count_printer->marker->printer_get_length_f)(this2->count_printer, &len);
        (this2->count_printer->marker->printer_do_f)(this2->count_printer, data + at);
        at += len;
    }

    for (i = 0; i < this2->len; i++)
    {
        unsigned int len = this2->item_lengths[i];
        if (0 < this2->item_lengths[i] || 0 == this2->type_marker)
        {
            unsigned int j = 0;
            ubjs_prmtv_marker_printer *child_printer = this2->item_printers[i];

            data[at++] = '\n';
            for (j = 0; j < (1 + this->glue->indent) * 4; j++, data[at++] = ' ')
            {
            }

            if (0 == this2->type_marker)
            {
                data[at++] = '[';
                data[at++] = child_printer->marker->abyte;
                data[at++] = ']';
            }
            (child_printer->marker->printer_do_f)(child_printer, data + at);
            at += len;
        }
    }
    if (this2->type_marker == 0 && this2->count_printer == 0)
    {
        /* \n + indent + ]*/
        if (0 < this2->len)
        {
            unsigned int j;
            data[at++] = '\n';
            for (j = 0; j < this->glue->indent * 4; j++, data[at++] = ' ')
            {
            }
        }
        data[at++] = '[';
        data[at++] = 93;
        data[at++] = ']';
    }
}
