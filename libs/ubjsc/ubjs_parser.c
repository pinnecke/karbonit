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

#include "ubjs_primitive_noop_prv.h"

ubjs_result ubjs_parser_builder_new(ubjs_library *lib, ubjs_parser_builder **pthis)
{
    ubjs_parser_builder *this = 0;
    ubjs_library_alloc_f alloc_f;

    if (0 == lib || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(lib, &alloc_f);
    this = (ubjs_parser_builder *)(alloc_f)(sizeof(struct ubjs_parser_builder));
    this->lib = lib;
    this->userdata = 0;
    this->parsed_f = 0;
    this->error_f = 0;
    this->free_f = 0;
    this->debug_f = 0;
    this->limit_bytes_since_last_callback = 0;
    this->limit_container_length = 0;
    this->limit_string_length = 0;
    this->limit_recursion_level = 0;
    this->silently_ignore_toplevel_noops = UFALSE;

    *pthis = this;
    return UR_OK;
}

ubjs_result ubjs_parser_builder_free(ubjs_parser_builder **pthis)
{
    ubjs_parser_builder *this = 0;
    ubjs_library_free_f free_f;

    if (0 == pthis || 0 == *pthis)
    {
        return UR_ERROR;
    }

    this = *pthis;
    ubjs_library_get_free_f(this->lib, &free_f);
    (free_f)(this);
    *pthis = 0;
    return UR_OK;
}

ubjs_result ubjs_parser_builder_set_userdata(ubjs_parser_builder *this, void *value)
{
    if (0 == this || 0 == value)
    {
        return UR_ERROR;
    }

    this->userdata = value;
    return UR_OK;
}

ubjs_result ubjs_parser_builder_set_parsed_f(ubjs_parser_builder *this,
    ubjs_parser_parsed_f parsed_f)
{
    if (0 == this || 0 == parsed_f)
    {
        return UR_ERROR;
    }

    this->parsed_f = parsed_f;
    return UR_OK;
}

ubjs_result ubjs_parser_builder_set_error_f(ubjs_parser_builder *this,
    ubjs_parser_error_f error_f)
{
    if (0 == this || 0 == error_f)
    {
        return UR_ERROR;
    }

    this->error_f = error_f;
    return UR_OK;
}

ubjs_result ubjs_parser_builder_set_debug_f(ubjs_parser_builder *this,
    ubjs_parser_debug_f debug_f)
{
    if (0 == this || 0 == debug_f)
    {
        return UR_ERROR;
    }

    this->debug_f = debug_f;
    return UR_OK;
}

ubjs_result ubjs_parser_builder_set_free_f(ubjs_parser_builder *this,
    ubjs_parser_free_f free_f)
{
    if (0 == this || 0 == free_f)
    {
        return UR_ERROR;
    }

    this->free_f = free_f;
    return UR_OK;
}

ubjs_result ubjs_parser_builder_set_limit_bytes_since_last_callback(ubjs_parser_builder *this,
    unsigned int value)
{
    if (0 == this || 0 == value)
    {
        return UR_ERROR;
    }

    this->limit_bytes_since_last_callback = value;
    return UR_OK;
}

ubjs_result ubjs_parser_builder_set_limit_container_length(ubjs_parser_builder *this,
    unsigned int value)
{
    if (0 == this || 0 == value)
    {
        return UR_ERROR;
    }

    this->limit_container_length = value;
    return UR_OK;
}

ubjs_result ubjs_parser_builder_set_limit_string_length(ubjs_parser_builder *this,
    unsigned int value)
{
    if (0 == this || 0 == value)
    {
        return UR_ERROR;
    }

    this->limit_string_length = value;
    return UR_OK;
}

ubjs_result ubjs_parser_builder_set_limit_recursion_level(ubjs_parser_builder *this,
    unsigned int value)
{
    if (0 == this || 0 == value)
    {
        return UR_ERROR;
    }

    this->limit_recursion_level = value;
    return UR_OK;
}

ubjs_result ubjs_parser_builder_set_silently_ignore_toplevel_noops(ubjs_parser_builder *this,
    ubjs_bool value)
{
    if (0 == this)
    {
        return UR_ERROR;
    }

    this->silently_ignore_toplevel_noops = value;
    return UR_OK;
}

ubjs_result ubjs_parser_builder_build(ubjs_parser_builder *builder, ubjs_parser **pthis)
{
    ubjs_parser *this;
    ubjs_library_alloc_f alloc_f;

    if (0 == builder || 0 == pthis || 0 == builder->parsed_f)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(builder->lib, &alloc_f);
    this = (ubjs_parser *)(alloc_f)(sizeof(struct ubjs_parser));
    this->lib = builder->lib;
    this->userdata = builder->userdata;
    this->parsed_f = builder->parsed_f;
    this->error_f = builder->error_f;
    this->debug_f = builder->debug_f;
    this->free_f = builder->free_f;
    this->limit_bytes_since_last_callback = builder->limit_bytes_since_last_callback;
    this->limit_container_length = builder->limit_container_length;
    this->limit_string_length = builder->limit_string_length;
    this->limit_recursion_level = builder->limit_recursion_level;
    this->silently_ignore_toplevel_noops = builder->silently_ignore_toplevel_noops;
    this->errors=0;
    this->counters.bytes_since_last_callback = 0;

    ubjs_selfemptying_list_new(this->lib,
        (ubjs_glue_value_free) ubjs_parser_give_control_request_free,
        ubjs_parser_give_control_fifo_callback,
        (void *)this, &(this->give_control_fifo));

    /* LCOV_EXCL_START */
#ifndef NDEBUG
    if (0 != this->debug_f)
    {
        (this->debug_f)(this->userdata, 13, "### ALIVE ###");
    }
#endif
    /* LCOV_EXCL_STOP */

    ubjs_processor_top(this);

    *pthis=this;
    return UR_OK;
}

ubjs_result ubjs_parser_free(ubjs_parser **pthis)
{
    ubjs_parser *this;
    ubjs_processor *processor;
    ubjs_processor *parent_processor;
    ubjs_library_free_f free_f;

    if (0 == pthis || 0 == *pthis)
    {
        return UR_ERROR;
    }

    this = *pthis;

    for (processor=this->processor; 0 != processor;)
    {
        parent_processor = processor->parent;
        (processor->free)(processor);
        processor=parent_processor;
    }

    ubjs_selfemptying_list_free(&(this->give_control_fifo));

    if (0 != this->free_f)
    {
        (this->free_f)(this->userdata);
    }

    /* LCOV_EXCL_START */
#ifndef NDEBUG
    if (0 != this->debug_f)
    {
        (this->debug_f)(this->userdata, 15, "### D E A D ###");
    }
#endif
    /* LCOV_EXCL_STOP */

    ubjs_library_get_free_f(this->lib, &free_f);
    (free_f)(this);
    (*pthis)=0;

    return UR_OK;
}

ubjs_result ubjs_parser_get_userdata(ubjs_parser *this, void **puserdata)
{
    if (0 == this || 0 == puserdata)
    {
        return UR_ERROR;
    }

    *puserdata=this->userdata;
    return UR_OK;
}

#ifndef NDEBUG
static void ubjs_parser_parse_at_byte(ubjs_parser *this, unsigned int i, unsigned int length,
    uint8_t abyte)
{
    if (0 != this->debug_f)
    {
        char *message = 0;
        unsigned int len = 0;
        ubjs_library_free_f free_f;

        ubjs_compact_sprints(this->lib, &message, &len, 26, "ubjs_parser_parser() byte ");
        ubjs_compact_sprintui(this->lib, &message, &len, i + 1);
        ubjs_compact_sprints(this->lib, &message, &len, 1, "/");
        ubjs_compact_sprintui(this->lib, &message, &len, length);

        ubjs_compact_sprints(this->lib, &message, &len, 2, ": ");
        ubjs_compact_sprintui(this->lib, &message, &len, abyte);

        ubjs_compact_sprints(this->lib, &message, &len, 14, " in processor ");
        ubjs_compact_sprints(this->lib, &message, &len, strlen(this->processor->name),
            this->processor->name);
        (this->debug_f)(this->userdata, len, message);

        ubjs_library_get_free_f(this->lib, &free_f);
        (free_f)(message);
    }
}
#endif

ubjs_result ubjs_parser_parse(ubjs_parser *this, uint8_t *data, unsigned int length)
{
    unsigned int i;

    if (0 == this || 0 == data)
    {
        return UR_ERROR;
    }

    if (0 == length)
    {
        return UR_OK;
    }

    for (i=0; i<length; i++)
    {
        this->errors=0;

#ifndef NDEBUG
        ubjs_parser_parse_at_byte(this, i, length, data[i]);
#endif

        if (0 == this->processor->read_byte)
        {
            ubjs_parser_emit_error(this, 39,
                "Unexpected data cause parser corruption");
            return UR_ERROR;
        }

        (this->processor->read_byte)(this->processor, i, data[i]);
        if (0 < this->errors)
        {
            return UR_ERROR;
        }

        if (this->limit_bytes_since_last_callback > 0)
        {
            this->counters.bytes_since_last_callback++;

            if (this->limit_bytes_since_last_callback <=
                this->counters.bytes_since_last_callback)
            {
                ubjs_parser_emit_error(this, 42, "Reached limit of bytes since last callback");
                return UR_ERROR;
            }
        }
    }

    return UR_OK;
}

void ubjs_parser_give_control_request_free(ubjs_parser_give_control_request *this)
{
    ubjs_library_free_f free_f;
    ubjs_library_get_free_f(this->lib, &free_f);
    (free_f)(this);
}

void ubjs_parser_give_control_fifo_callback(ubjs_selfemptying_list *this, void *obj)
{
    ubjs_parser_give_control_request *robj = (ubjs_parser_give_control_request *)obj;
    ubjs_parser *parser = (ubjs_parser *)this->userdata;

    /* LCOV_EXCL_START */
#ifndef NDEBUG
    if (0 != parser->debug_f)
    {
        char *message = 0;
        unsigned int len = 0;
        ubjs_library_free_f free_f;

        ubjs_library_get_free_f(parser->lib, &free_f);

        ubjs_compact_sprints(parser->lib, &message, &len, 44,
            "ubjs_parser_give_control_fifo_callback() to ");
        ubjs_compact_sprints(parser->lib, &message, &len, strlen(robj->processor->name),
           robj->processor->name);

        if (0 != robj->present)
        {
            char *dtext = 0;
            unsigned int dlen = 0;
            ubjs_library_alloc_f alloc_f;

            ubjs_library_get_alloc_f(parser->lib, &alloc_f);

            ubjs_prmtv_debug_string_get_length(robj->present, &dlen);
            dtext = (char *)(alloc_f)(sizeof(char) * (dlen + 1));
            ubjs_prmtv_debug_string_copy(robj->present, dtext);

            ubjs_compact_sprints(parser->lib, &message, &len, 15,
                ", with present ");
            ubjs_compact_sprints(parser->lib, &message, &len, dlen, dtext);
            (free_f)(dtext);
        }

        if (0 != robj->present)
        {
            ubjs_compact_sprints(this->lib, &message, &len, 14,
                ", with marker ");
            ubjs_compact_sprintp(this->lib, &message, &len, robj->marker);
        }

        ubjs_compact_sprints(this->lib, &message, &len, 21,
            ", at recursion level ");
        ubjs_compact_sprintui(this->lib, &message, &len, robj->processor->recursion_level);
        ubjs_compact_sprints(this->lib, &message, &len, 1,
            "/");
        ubjs_compact_sprintui(this->lib, &message, &len, parser->limit_recursion_level);

        (parser->debug_f)(parser->userdata, len, message);
        (free_f)(message);
    }
#endif
    /* LCOV_EXCL_STOP */

    parser->processor=robj->processor;
    if (0 != robj->processor->got_control)
    {
        (robj->processor->got_control)(robj->processor, robj);
    }
}

void ubjs_parser_give_control(ubjs_parser *this, ubjs_processor *processor,
    ubjs_prmtv *present, ubjs_prmtv_marker *marker)
{
    ubjs_parser_give_control_request *obj;
    ubjs_library_alloc_f alloc_f;

    ubjs_library_get_alloc_f(this->lib, &alloc_f);
    obj = (ubjs_parser_give_control_request *)(alloc_f)(
        sizeof(struct ubjs_parser_give_control_request));
    obj->processor=processor;
    obj->present=present;
    obj->marker=marker;
    obj->lib=this->lib;

    if (0 != obj->present && UTRUE == this->silently_ignore_toplevel_noops)
    {
        ubjs_prmtv_marker *marker = 0;
        ubjs_prmtv_get_marker(obj->present, &marker);
        if (&ubjs_prmtv_noop_marker == marker)
        {
            ubjs_prmtv_free(&(obj->present));
        }
    }

    ubjs_selfemptying_list_add(this->give_control_fifo, obj);
}

void ubjs_parser_emit_error(ubjs_parser *this, unsigned int len, char *message)
{
    /* LCOV_EXCL_START */
#ifndef NDEBUG
    if (0 != this->debug_f)
    {
        char *message2 = 0;
        unsigned int len2 = 0;
        ubjs_library_free_f free_f;

        ubjs_compact_sprints(this->lib, &message2, &len2, 25, "ubjs_parser_emit_error() ");
        ubjs_compact_sprints(this->lib, &message2, &len2, len, message);

        (this->debug_f)(this->userdata, len2, message2);

        ubjs_library_get_free_f(this->lib, &free_f);
        (free_f)(message2);
    }
#endif
    /* LCOV_EXCL_STOP */

    if (0 != this->error_f)
    {
        (this->error_f)(this->userdata, len, message);
    }

    this->errors += 1;
}

void ubjs_processor_top(ubjs_parser *parser)
{
    ubjs_processor *this;
    ubjs_library_alloc_f alloc_f;
    ubjs_library_free_f free_f;

    ubjs_library_get_alloc_f(parser->lib, &alloc_f);
    ubjs_library_get_free_f(parser->lib, &free_f);

    this = (ubjs_processor *)(alloc_f)(
        sizeof(struct ubjs_processor));
    this->name = "top";
    this->parent=0;
    this->parser=parser;
    this->got_control=ubjs_processor_top_got_control;
    this->recursion_level = 0;
    this->read_byte = 0;
    this->free=(ubjs_processor_free)(free_f);
    parser->processor = this;

    /* Always processor_top, they have control */
    ubjs_parser_give_control(this->parser, this, 0, 0);
}

ubjs_result ubjs_processor_top_selected_marker(ubjs_processor *this,
    ubjs_prmtv_marker *marker)
{
    ubjs_processor *next = 0;
    ubjs_processor_marker(this, marker, &next);
    ubjs_parser_give_control(this->parser, next, 0, 0);
    return UR_OK;
}

void ubjs_processor_top_got_control(ubjs_processor *this, ubjs_parser_give_control_request *req)
{
    ubjs_glue_array *markers = 0;

    if (0 != req->present)
    {
        (this->parser->parsed_f)(this->parser->userdata, req->present);
        this->parser->counters.bytes_since_last_callback = 0;
    }

    ubjs_library_get_markers(this->parser->lib, &markers);
    ubjs_processor_next_prmtv(this,
        markers, ubjs_processor_top_selected_marker);
}

void ubjs_processor_next_prmtv(ubjs_processor *parent,
    ubjs_glue_array *markers,
    ubjs_processor_next_prmtv_selected_factory_marker selected_factory_marker)
{
    ubjs_processor_next_prmtv_t *this;
    unsigned int markers_len = 0;
    char name[48];
    unsigned int name_len;
    char *name_template = "next object from %u markers";
    ubjs_library_alloc_f alloc_f;

    (markers->get_length_f)(markers, &markers_len);
    name_len = sprintf(name, name_template, markers_len);

    ubjs_library_get_alloc_f(parent->parser->lib, &alloc_f);
    this = (ubjs_processor_next_prmtv_t *)(alloc_f)(
        sizeof(struct ubjs_processor_next_prmtv_t));

    this->super.name = (char *)(alloc_f)(sizeof(char) * (name_len + 1));
    strncpy(this->super.name, name, name_len + 1);

    this->super.parent=parent;
    this->super.parser=parent->parser;
    this->super.got_control=0;
    this->super.read_byte = ubjs_processor_next_prmtv_read_byte;
    this->super.free=ubjs_processor_next_prmtv_free;
    this->super.recursion_level = parent->recursion_level + 1;

    this->markers=markers;
    this->selected_factory_marker=selected_factory_marker;

    ubjs_parser_give_control(this->super.parser, (ubjs_processor *)this, 0, 0);
}

void ubjs_processor_next_prmtv_free(ubjs_processor *this)
{
    ubjs_library_free_f free_f;
    ubjs_library_get_free_f(this->parser->lib, &free_f);
    (free_f)(this->name);
    (free_f)(this);
}

void ubjs_processor_next_prmtv_read_byte(ubjs_processor *this, unsigned int pos,
    uint8_t c)
{
    ubjs_processor_next_prmtv_t *sub=(ubjs_processor_next_prmtv_t *)this;

    char *message = 0;
    unsigned int message_length = 0;
    ubjs_glue_array_iterator *it = 0;

    (sub->markers->iterate_f)(sub->markers, &it);
    while (UR_OK == (it->next_f)(it))
    {
        ubjs_prmtv_marker *marker = 0;
        (it->get_f)(it, (void **)&marker);

        if (marker->abyte == c)
        {
            (it->free_f)(&it);
            (sub->selected_factory_marker)(this->parent, marker);
            (this->free)(this);
            return;
        }
    }
    (it->free_f)(&it);

    {
        ubjs_library_free_f free_f;
        ubjs_library_get_free_f(this->parser->lib, &free_f);

        ubjs_compact_sprints(this->parser->lib, &message, &message_length, 3, "At ");
        ubjs_compact_sprintui(this->parser->lib, &message, &message_length, pos);
        ubjs_compact_sprints(this->parser->lib, &message, &message_length, 2, " [");
        ubjs_compact_sprintui(this->parser->lib, &message, &message_length, c);
        ubjs_compact_sprints(this->parser->lib, &message, &message_length, 16, "] unknown marker");

        ubjs_parser_emit_error(this->parser, message_length, message);
        (free_f)(message);
    }
}
