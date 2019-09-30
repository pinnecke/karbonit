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

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "ubjs_writer_prv.h"
#include "ubjs_common_prv.h"
#include "ubjs_library_prv.h"

ubjs_result ubjs_writer_builder_new(ubjs_library *lib, ubjs_writer_builder **pthis)
{
    ubjs_writer_builder *this;
    ubjs_library_alloc_f alloc_f;

    if (0 == lib || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(lib, &alloc_f);
    this=(ubjs_writer_builder *)(alloc_f)(sizeof(struct ubjs_writer_builder));
    this->lib=lib;
    this->userdata=0;
    this->free_f=0;
    this->would_write_f=0;
    this->would_print_f=0;
    this->free_primitives_early=UFALSE;
    this->debug_f=0;
    *pthis=this;

    return UR_OK;
}

ubjs_result ubjs_writer_builder_free(ubjs_writer_builder **pthis)
{
    ubjs_writer_builder *this;
    ubjs_library_free_f free_f;

    if (0 == pthis || 0 == *pthis)
    {
        return UR_ERROR;
    }

    this=*pthis;
    ubjs_library_get_free_f(this->lib, &free_f);
    (free_f)(this);
    *pthis=0;
    return UR_OK;
}

ubjs_result ubjs_writer_builder_set_userdata(ubjs_writer_builder *this, void *userdata)
{
    if (0 == this || 0 == userdata)
    {
        return UR_ERROR;
    }

    this->userdata = userdata;
    return UR_OK;
}

ubjs_result ubjs_writer_builder_set_would_write_f(ubjs_writer_builder *this,
    ubjs_writer_would_write_f would_write_f)
{
    if (0 == this || 0 == would_write_f)
    {
        return UR_ERROR;
    }

    this->would_write_f = would_write_f;
    return UR_OK;
}

ubjs_result ubjs_writer_builder_set_would_print_f(ubjs_writer_builder *this,
    ubjs_writer_would_print_f would_print_f)
{
    if (0 == this || 0 == would_print_f)
    {
        return UR_ERROR;
    }

    this->would_print_f = would_print_f;
    return UR_OK;
}

ubjs_result ubjs_writer_builder_set_free_primitives_early(ubjs_writer_builder *this,
    ubjs_bool free_primitives_early)
{
    if (0 == this)
    {
        return UR_ERROR;
    }

    this->free_primitives_early = free_primitives_early;
    return UR_OK;
}

ubjs_result ubjs_writer_builder_set_debug_f(ubjs_writer_builder *this,
    ubjs_writer_debug_f debug_f)
{
    if (0 == this || 0 == debug_f)
    {
        return UR_ERROR;
    }

    this->debug_f = debug_f;
    return UR_OK;
}

ubjs_result ubjs_writer_builder_set_free_f(ubjs_writer_builder *this,
    ubjs_writer_free_f free_f)
{
    if (0 == this || 0 == free_f)
    {
        return UR_ERROR;
    }

    this->free_f = free_f;
    return UR_OK;
}

ubjs_result ubjs_writer_builder_build(ubjs_writer_builder *builder, ubjs_writer **pthis)
{
    ubjs_writer *this;
    ubjs_library_alloc_f alloc_f;

    if (0 == builder || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(builder->lib, &alloc_f);
    this=(ubjs_writer *)(alloc_f)(sizeof(struct ubjs_writer));
    this->lib=builder->lib;
    this->userdata=builder->userdata;
    this->would_write_f=builder->would_write_f;
    this->would_print_f=builder->would_print_f;
    this->debug_f=builder->debug_f;
    this->free_primitives_early=builder->free_primitives_early;
    this->free_f=builder->free_f;
    *pthis=this;

    /* LCOV_EXCL_START */
#ifndef NDEBUG
    if (0 != this->debug_f)
    {
        (this->debug_f)(this->userdata, 13, "### ALIVE ###");
    }
#endif
    /* LCOV_EXCL_STOP */

    return UR_OK;
}

ubjs_result ubjs_writer_free(ubjs_writer **pthis)
{
    ubjs_writer *this;
    ubjs_library_free_f free_f;

    if (0 == pthis || 0 == *pthis)
    {
        return UR_ERROR;
    }

    this=*pthis;

    /* LCOV_EXCL_START */
#ifndef NDEBUG
    if (0 != this->debug_f)
    {
        (this->debug_f)(this->userdata, 12, "### DEAD ###");
    }
#endif
    /* LCOV_EXCL_STOP */

    if (0 != this->free_f)
    {
        (this->free_f)(this->userdata);
    }

    ubjs_library_get_free_f(this->lib, &free_f);
    (free_f)(this);

    *pthis=0;
    return UR_OK;
}

ubjs_result ubjs_writer_get_userdata(ubjs_writer *this, void **puserdata)
{
    if (0 == this || 0 == puserdata)
    {
        return UR_ERROR;
    }

    *puserdata = this->userdata;;
    return UR_OK;
}
static void ubjs_writer_writer_glue_debug(ubjs_prmtv_marker_writer_glue *this,
    unsigned int len, char *msg)
{
#ifndef NDEBUG
    ubjs_writer *writer = (ubjs_writer *)this->userdata;
    (writer->debug_f)(writer, len, msg);
#endif
}

static void ubjs_writer_printer_glue_debug(ubjs_prmtv_marker_printer_glue *this,
    unsigned int len, char *msg)
{
#ifndef NDEBUG
    ubjs_writer *writer = (ubjs_writer *)this->userdata;
    (writer->debug_f)(writer, len, msg);
#endif
}

#ifndef NDEBUG
static void ubjs_writer_write_debug(ubjs_writer *this, unsigned int len, uint8_t *data)
{
    char *msg = 0;
    unsigned int mlen = 0;
    unsigned int i;
    unsigned int pages = (len + 7) / 8;
    ubjs_library_free_f free_f;

    ubjs_library_get_free_f(this->lib, &free_f);

    {
        ubjs_compact_sprints(this->lib, &msg, &mlen, 12, "Would write ");
        ubjs_compact_sprintui(this->lib, &msg, &mlen, len);
        ubjs_compact_sprints(this->lib, &msg, &mlen, 8, " bytes:");
        (this->debug_f)(this->userdata, mlen, msg);

        (free_f)(msg);
        msg = 0;
        mlen = 0;
    }

    for (i = 0; i < len; i++)
    {
        unsigned int data_i_len = (data[i] > 99 ? 3 : (data[i] > 9 ? 2 : 1));
        if (0 == (i % 8))
        {
            unsigned int pages_len = 0;
            unsigned int page_len = 0;
            char tmp[10];

            pages_len = sprintf(tmp, "%u", pages);
            page_len = sprintf(tmp, "%u", i / 8 + 1);
            ubjs_compact_sprints(this->lib, &msg, &mlen, pages_len - page_len,
                "                         ");
            ubjs_compact_sprintui(this->lib, &msg, &mlen, i / 8 + 1);
            ubjs_compact_sprints(this->lib, &msg, &mlen, 1, "/");
            ubjs_compact_sprintui(this->lib, &msg, &mlen, pages);
            ubjs_compact_sprints(this->lib, &msg, &mlen, 3, " | ");
        }

        ubjs_compact_sprints(this->lib, &msg, &mlen, 1, "[");
        ubjs_compact_sprintui(this->lib, &msg, &mlen, data[i]);
        ubjs_compact_sprints(this->lib, &msg, &mlen, 1 + 4 - data_i_len, "]   ");

        if (7 == (i % 8))
        {
            (this->debug_f)(this->userdata, mlen, msg);
            (free_f)(msg);
            msg = 0;
            mlen = 0;
        }
    }

    if (0 != msg)
    {
        (this->debug_f)(this->userdata, mlen, msg);
        (free_f)(msg);
    }
}
#endif

ubjs_result ubjs_writer_write(ubjs_writer *this, ubjs_prmtv *object)
{
    unsigned int len = 0;
    uint8_t *data;
    ubjs_prmtv_marker_writer_glue glue;
    ubjs_prmtv_marker_writer *writer = 0;
    ubjs_prmtv_marker *marker = 0;
    ubjs_library_alloc_f alloc_f;
    ubjs_library_free_f free_f;

    if (0 == this || 0 == object || 0 == this->would_write_f)
    {
        return UR_ERROR;
    }

    ubjs_prmtv_get_marker(object, &marker);

    glue.userdata = (void *)this;
    glue.prmtv = object;
    glue.debug_f = ubjs_writer_writer_glue_debug;

    (marker->writer_new_f)(this->lib, &glue, &writer);
    (marker->writer_get_length_f)(writer, &len);
    len += 1;

    ubjs_library_get_alloc_f(this->lib, &alloc_f);
    ubjs_library_get_free_f(this->lib, &free_f);
    data = (uint8_t *)(alloc_f)(sizeof(uint8_t) * (len));

    *(data) = marker->abyte;
    (marker->writer_do_f)(writer, data + 1);
    (marker->writer_free_f)(&writer);

    (this->would_write_f)(this->userdata, data, len);

    /* LCOV_EXCL_START */
#ifndef NDEBUG
    if (0 != this->debug_f)
    {
        ubjs_writer_write_debug(this, len, data);
    }
#endif
    /* LCOV_EXCL_STOP */

    (free_f)(data);

    return UR_OK;
}

ubjs_result ubjs_writer_print(ubjs_writer *this, ubjs_prmtv *object)
{
    unsigned int len = 0;
    char *data;
    ubjs_prmtv_marker_printer_glue glue;
    ubjs_prmtv_marker_printer *printer = 0;
    ubjs_prmtv_marker *marker = 0;
    ubjs_library_alloc_f alloc_f = 0;
    ubjs_library_free_f free_f = 0;

    if (0 == this || 0 == object || 0 == this->would_print_f)
    {
        return UR_ERROR;
    }

    ubjs_prmtv_get_marker(object, &marker);

    glue.userdata = (void *)this;
    glue.prmtv = object;
    glue.debug_f = ubjs_writer_printer_glue_debug;
    glue.indent = 0;

    (marker->printer_new_f)(this->lib, &glue, &printer);
    (marker->printer_get_length_f)(printer, &len);
    len += 3;

    ubjs_library_get_alloc_f(this->lib, &alloc_f);
    ubjs_library_get_free_f(this->lib, &free_f);
    data = (char *)(alloc_f)(sizeof(char) * (len));

    data[0] = '[';
    data[1] = marker->abyte;
    data[2] = ']';
    (marker->printer_do_f)(printer, data + 3);
    (marker->printer_free_f)(&printer);

    (this->would_print_f)(this->userdata, data, len);

    /* LCOV_EXCL_START */
#ifndef NDEBUG
    if (0 != this->debug_f)
    {
        char *msg = 0;
        unsigned int mlen = 0;

        ubjs_compact_sprints(this->lib, &msg, &mlen, 12, "Would print ");
        ubjs_compact_sprintui(this->lib, &msg, &mlen, len);
        ubjs_compact_sprints(this->lib, &msg, &mlen, 8, " bytes:");
        (this->debug_f)(this->userdata, mlen, msg);
        (free_f)(msg);
        msg = 0;
        mlen = 0;

        ubjs_compact_sprints(this->lib, &msg, &mlen, len, data);
        (this->debug_f)(this->userdata, mlen, msg);
        (free_f)(msg);
    }
#endif
    /* LCOV_EXCL_STOP */

    (free_f)(data);
    return UR_OK;
}
