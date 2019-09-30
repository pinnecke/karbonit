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

#include "ubjs_library_prv.h"
#include "ubjs_primitives_prv.h"
#include "ubjs_primitive_true_prv.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
ubjs_prmtv_marker ubjs_prmtv_true_marker =
{
    84,

    ubjs_prmtv_true_free,

    0,
    0,

#ifndef NDEBUG
    ubjs_prmtv_true_debug_string_get_length,
    ubjs_prmtv_true_debug_string_copy,
#else
    0,
    0,
#endif

    ubjs_prmtv_true_parser_processor_new,
    ubjs_prmtv_true_parser_processor_free,

    0,
    0,
    ubjs_prmtv_true_parser_processor_got_control,

    0,

    ubjs_prmtv_true_writer_new,
    ubjs_prmtv_true_writer_free,
    ubjs_prmtv_true_writer_get_length,
    ubjs_prmtv_true_writer_do,

    ubjs_prmtv_true_printer_new,
    ubjs_prmtv_true_printer_free,
    ubjs_prmtv_true_printer_get_length,
    ubjs_prmtv_true_printer_do
};

ubjs_prmtv __ubjs_prmtv_true = {0, &ubjs_prmtv_true_marker};

ubjs_prmtv *ubjs_prmtv_true(void)
{
    return &__ubjs_prmtv_true;
}

ubjs_result ubjs_prmtv_true_free(ubjs_prmtv **pthis)
{
    if (0 == pthis || 0 == *pthis)
    {
        return UR_ERROR;
    }

    *pthis = 0;
    return UR_OK;
}

#ifndef NDEBUG
ubjs_result ubjs_prmtv_true_debug_string_get_length(ubjs_prmtv *this, unsigned int *plen)
{
    if (0 == this || 0 == plen)
    {
        return UR_ERROR;
    }

    *plen = 4;
    return UR_OK;
}

ubjs_result ubjs_prmtv_true_debug_string_copy(ubjs_prmtv *this, char *str)
{
    if (0 == this || 0 == str)
    {
        return UR_ERROR;
    }

    memcpy(str, "true", sizeof(char) * 4);
    return UR_OK;
}
#endif

ubjs_result ubjs_prmtv_true_parser_processor_new(ubjs_library *lib,
     ubjs_prmtv_marker_parser_glue *glue, ubjs_prmtv_marker_parser_processor **pthis)
{
    ubjs_prmtv_marker_parser_processor *this;
    ubjs_library_alloc_f alloc_f;

    if (0 == lib || 0 == glue || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(lib, &alloc_f);
    this = (ubjs_prmtv_marker_parser_processor *)(alloc_f)(sizeof(
        struct ubjs_prmtv_marker_parser_processor));
    this->lib = lib;
    this->marker = &ubjs_prmtv_true_marker;
    this->name = "true";
    this->glue = glue;
    *pthis = this;
    return UR_OK;
}

ubjs_result ubjs_prmtv_true_parser_processor_free(
    ubjs_prmtv_marker_parser_processor **pthis)
{
    ubjs_prmtv_marker_parser_processor *this;
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

void ubjs_prmtv_true_parser_processor_got_control(
    ubjs_prmtv_marker_parser_processor *this)
{
    (this->glue->return_control_f)(this->glue, ubjs_prmtv_true());
}

ubjs_result ubjs_prmtv_true_writer_new(ubjs_library *lib,
    ubjs_prmtv_marker_writer_glue *glue, ubjs_prmtv_marker_writer **pthis)
{
    ubjs_prmtv_marker_writer *this;
    ubjs_library_alloc_f alloc_f;

    if (0 == lib || 0 == glue || &__ubjs_prmtv_true != glue->prmtv || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(lib, &alloc_f);
    this = (ubjs_prmtv_marker_writer *)(alloc_f)(sizeof(
        struct ubjs_prmtv_marker_writer));
    this->lib = lib;
    this->marker = &ubjs_prmtv_true_marker;
    this->name = "true";
    this->glue = glue;
    *pthis = this;
    return UR_OK;
}

ubjs_result ubjs_prmtv_true_writer_free(ubjs_prmtv_marker_writer **pthis)
{
    ubjs_prmtv_marker_writer *this;
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

void ubjs_prmtv_true_writer_get_length(ubjs_prmtv_marker_writer *this,
    unsigned int *plen)
{
    *plen = 0;
}

void ubjs_prmtv_true_writer_do(ubjs_prmtv_marker_writer *this, uint8_t *data)
{
}

ubjs_result ubjs_prmtv_true_printer_new(ubjs_library *lib,
    ubjs_prmtv_marker_printer_glue *glue,
    ubjs_prmtv_marker_printer **pthis)
{
    ubjs_prmtv_marker_printer *this;
    ubjs_library_alloc_f alloc_f;

    if (0 == lib || 0 == glue || &__ubjs_prmtv_true != glue->prmtv || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(lib, &alloc_f);
    this = (ubjs_prmtv_marker_printer *)(alloc_f)(sizeof(
        struct ubjs_prmtv_marker_printer));
    this->lib = lib;
    this->marker = &ubjs_prmtv_true_marker;
    this->name = "true";
    this->glue = glue;
    *pthis = this;
    return UR_OK;
}

ubjs_result ubjs_prmtv_true_printer_free(ubjs_prmtv_marker_printer **pthis)
{
    ubjs_prmtv_marker_printer *this;
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

void ubjs_prmtv_true_printer_get_length(ubjs_prmtv_marker_printer *this,
    unsigned int *plen)
{
    *plen = 0;
}

void ubjs_prmtv_true_printer_do(ubjs_prmtv_marker_printer *this, char *data)
{
}

#pragma clang diagnostic pop