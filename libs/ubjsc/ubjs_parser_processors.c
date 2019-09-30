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
#include "ubjs_parser_prv.h"
#include "ubjs_glue_array_array.h"
#include "ubjs_primitive_str.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
ubjs_result ubjs_processor_marker(ubjs_processor *parent, ubjs_prmtv_marker *marker,
    ubjs_processor **pthis)
{
    ubjs_processor_marker_t *this;
    ubjs_library_alloc_f alloc_f;

    ubjs_library_get_alloc_f(parent->parser->lib, &alloc_f);
    this = (ubjs_processor_marker_t *)(alloc_f)(sizeof(
        struct ubjs_processor_marker_t));
    this->marker = marker;
    this->glue.userdata = (void *)this;
    this->glue.parent = parent;
    this->glue.return_control_f = ubjs_processor_marker_return_control;
    this->glue.want_marker_f = ubjs_processor_marker_want_marker;
    this->glue.want_child_f = ubjs_processor_marker_want_child;
    this->glue.debug_f = ubjs_processor_marker_debug;
    this->glue.error_f = ubjs_processor_marker_error;
    this->glue.limit_container_length = parent->parser->limit_container_length;
    this->glue.limit_string_length = parent->parser->limit_string_length;
    this->glue.limit_recursion_level = parent->parser->limit_recursion_level;
    this->glue.recursion_level = parent->recursion_level + 1;
    (marker->parser_processor_new_f)(parent->parser->lib, &(this->glue), &(this->processor));

    this->super.name = this->processor->name;
    this->super.parent = parent;
    this->super.parser = parent->parser;
    this->super.got_control = ubjs_processor_marker_got_control;
    this->super.read_byte = ubjs_processor_marker_read_byte;
    this->super.free = ubjs_processor_marker_free;
    this->super.recursion_level = parent->recursion_level + 1;

    *pthis = (ubjs_processor *)this;
    return UR_OK;
}

void ubjs_processor_marker_got_control(ubjs_processor *this, ubjs_parser_give_control_request *req)
{
    ubjs_processor_marker_t *this2 = (ubjs_processor_marker_t *)this;

    if (0 != req->present)
    {
        if (0 == this2->marker->parser_processor_got_child_f)
        {
            char *msg = 0;
            unsigned int len = 0;
            char *dtext = 0;
            unsigned int dlen = 0;
            ubjs_library_alloc_f alloc_f;
            ubjs_library_free_f free_f;

            ubjs_library_get_alloc_f(this->parser->lib, &alloc_f);
            ubjs_library_get_free_f(this->parser->lib, &free_f);

            ubjs_prmtv_debug_string_get_length(req->present, &dlen);
            dtext = (char *)(alloc_f)(sizeof(char) * (dlen + 1));
            ubjs_prmtv_debug_string_copy(req->present, dtext);

            ubjs_compact_sprints(this->parser->lib, &msg, &len, 22, "Got unexpected present ");
            ubjs_compact_sprints(this->parser->lib, &msg, &len, dlen, dtext);
            (free_f)(dtext);

            ubjs_compact_sprints(this->parser->lib, &msg, &len, 21, " in parser processor ");
            ubjs_compact_sprints(this->parser->lib, &msg, &len, strlen(this->name),
                this->name);
            ubjs_processor_marker_error(&(this2->glue), len, msg);
            (free_f)(msg);
        }
        else
        {
            (this2->marker->parser_processor_got_child_f)(this2->processor, req->present);
        }
        return;
    }

    if (0 != req->marker)
    {
        if (0 == this2->marker->parser_processor_got_marker_f)
        {
            char *msg = 0;
            unsigned int len = 0;
            ubjs_library_free_f free_f;

            ubjs_library_get_free_f(this->parser->lib, &free_f);

            ubjs_compact_sprints(this->parser->lib, &msg, &len, 21, "Got unexpected marker ");
            ubjs_compact_sprintp(this->parser->lib, &msg, &len, (unsigned int *)req->marker);

            ubjs_compact_sprints(this->parser->lib, &msg, &len, 21, " in parser processor ");
            ubjs_compact_sprints(this->parser->lib, &msg, &len, strlen(this->name),
                this->name);
            ubjs_processor_marker_error(&(this2->glue), len, msg);
            (free_f)(msg);
        }
        else
        {
            (this2->marker->parser_processor_got_marker_f)(this2->processor, req->marker);
        }
        return;
    }

    (this2->marker->parser_processor_got_control_f)(this2->processor);
}

void ubjs_processor_marker_free(ubjs_processor *this)
{
    ubjs_processor_marker_t *this2 = (ubjs_processor_marker_t *)this;
    ubjs_library_free_f free_f;

    ubjs_library_get_free_f(this->parser->lib, &free_f);

#ifndef NDEBUG
    {
        char *msg = 0;
        unsigned int len = 0;

        ubjs_compact_sprints(this->parser->lib, &msg, &len, 29,
            "ubjs_processor_marker_free(): ");
        ubjs_compact_sprints(this->parser->lib, &msg, &len, strlen(this->name),
                this->name);
        ubjs_processor_marker_debug(&(this2->glue), len, msg);
        (free_f)(msg);
    }
#endif

    (this2->marker->parser_processor_free_f)(&(this2->processor));
    (free_f)(this2);
}

void ubjs_processor_marker_read_byte(ubjs_processor *this, unsigned int pos, uint8_t c)
{
    ubjs_processor_marker_t *this2 = (ubjs_processor_marker_t *)this;

#ifndef NDEBUG
    {
        char *msg = 0;
        unsigned int len = 0;
        ubjs_library_free_f free_f;

        ubjs_library_get_free_f(this->parser->lib, &free_f);

        ubjs_compact_sprints(this->parser->lib, &msg, &len, 52,
            "ubjs_processor_marker_read_byte()  ");
        ubjs_compact_sprintui(this->parser->lib, &msg, &len, c);
        ubjs_processor_marker_debug(&(this2->glue), len, msg);
        (free_f)(msg);
    }
#endif

    (this2->marker->parser_processor_read_byte_f)(this2->processor, c);
}

void ubjs_processor_marker_return_control(ubjs_prmtv_marker_parser_glue *this,
    void *present)
{
    ubjs_processor *this2 = (ubjs_processor *)this->userdata;
    ubjs_parser_give_control(this2->parser, this2->parent, present, 0);
    ubjs_processor_marker_free(this2);
}

ubjs_result ubjs_processor_marker_want_marker_selected_factory_marker(ubjs_processor *this,
    ubjs_prmtv_marker *marker)
{
    ubjs_parser_give_control(this->parser, this, 0, marker);
    return UR_OK;
}

void ubjs_processor_marker_want_marker(ubjs_prmtv_marker_parser_glue *this,
    ubjs_glue_array *markers)
{
    ubjs_processor_marker_t *this2 = (ubjs_processor_marker_t *)this->userdata;
    ubjs_processor_marker_debug(&(this2->glue), 11, "Want marker");
    ubjs_processor_next_prmtv((ubjs_processor *)this2,
        markers,
        ubjs_processor_marker_want_marker_selected_factory_marker);
}

void ubjs_processor_marker_want_child(ubjs_prmtv_marker_parser_glue *this,
    ubjs_prmtv_marker *marker)
{
    ubjs_processor_marker_t *this2 = (ubjs_processor_marker_t *)this->userdata;

    ubjs_processor_marker_debug(&(this2->glue), 10, "Want child");
    ubjs_processor_top_selected_marker((ubjs_processor *)this2, marker);
}

void ubjs_processor_marker_debug(ubjs_prmtv_marker_parser_glue *this,
    unsigned int len, char *msg)
{
    /* LCOV_EXCL_START */
#ifndef NDEBUG
    ubjs_processor *this2 = (ubjs_processor *)this->userdata;
    if (0 != this2->parser->debug_f)
    {
        char *msg2 = 0;
        unsigned int len2 = 0;
        ubjs_library_free_f free_f;

        ubjs_library_get_free_f(this2->parser->lib, &free_f);

        ubjs_compact_sprints(this2->parser->lib, &msg2, &len2, 20, "In parser processor ");
        ubjs_compact_sprints(this2->parser->lib, &msg2, &len2, strlen(this2->name),
            this2->name);
        ubjs_compact_sprints(this2->parser->lib, &msg2, &len2, 2, ": ");
        ubjs_compact_sprints(this2->parser->lib, &msg2, &len2, len, msg);
        (this2->parser->debug_f)(this2->parser->userdata, len2, msg2);
        (free_f)(msg2);
    }
#endif
    /* LCOV_EXCL_STOP */
}

void ubjs_processor_marker_error(ubjs_prmtv_marker_parser_glue *this,
    unsigned int len, char *msg)
{
    ubjs_processor *this2 = (ubjs_processor *)this->userdata;
    char *msg2 = 0;
    unsigned int len2 = 0;
    ubjs_library_free_f free_f;

    ubjs_library_get_free_f(this2->parser->lib, &free_f);

    ubjs_compact_sprints(this2->parser->lib, &msg2, &len2, 20, "In parser processor ");
    ubjs_compact_sprints(this2->parser->lib, &msg2, &len2, strlen(this2->name),
        this2->name);
    ubjs_compact_sprints(this2->parser->lib, &msg2, &len2, 2, ": ");
    ubjs_compact_sprints(this2->parser->lib, &msg2, &len2, len, msg);
    ubjs_parser_emit_error(this2->parser, len2, msg2);
    (free_f)(msg2);
}

#pragma clang diagnostic pop