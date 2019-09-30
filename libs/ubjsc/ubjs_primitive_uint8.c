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
#include "ubjs_primitive_uint8_prv.h"
#include "ubjs_common_prv.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
ubjs_prmtv_marker ubjs_prmtv_uint8_marker =
{
    85,

    ubjs_prmtv_uint8_free,

    ubjs_prmtv_uint8_new_from_int64,
    ubjs_prmtv_uint8_get_value_int64,

#ifndef NDEBUG
    ubjs_prmtv_uint8_debug_string_get_length,
    ubjs_prmtv_uint8_debug_string_copy,
#else
    0,
    0,
#endif

    ubjs_prmtv_uint8_parser_processor_new,
    ubjs_prmtv_uint8_parser_processor_free,

    0,
    0,
    ubjs_prmtv_uint8_parser_processor_got_control,

    ubjs_prmtv_uint8_parser_processor_read_byte,

    ubjs_prmtv_uint8_writer_new,
    ubjs_prmtv_uint8_writer_free,
    ubjs_prmtv_uint8_writer_get_length,
    ubjs_prmtv_uint8_writer_do,

    ubjs_prmtv_uint8_printer_new,
    ubjs_prmtv_uint8_printer_free,
    ubjs_prmtv_uint8_printer_get_length,
    ubjs_prmtv_uint8_printer_do
};

ubjs_result ubjs_prmtv_uint8(ubjs_library *lib, uint8_t value, ubjs_prmtv **pthis)
{
    ubjs_prmtv_uint8_t *this;
    ubjs_library_alloc_f alloc_f;

    if (0 == lib || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(lib, &alloc_f);
    this = (ubjs_prmtv_uint8_t *)(alloc_f)(sizeof(struct ubjs_prmtv_uint8_t));
    this->super.lib = lib;
    this->super.marker = &ubjs_prmtv_uint8_marker;
    this->value = value;

    *pthis = (ubjs_prmtv *)this;
    return UR_OK;
}

ubjs_result ubjs_prmtv_uint8_new_from_int64(ubjs_library *lib, int64_t value, ubjs_prmtv **pthis)
{
    if (0 == lib || 0 == pthis || value < 0 || value > UINT8_MAX)
    {
        return UR_ERROR;
    }
    return ubjs_prmtv_uint8(lib, (uint8_t)value, pthis);
}

ubjs_result ubjs_prmtv_uint8_free(ubjs_prmtv **pthis)
{
    ubjs_prmtv *this;
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

#ifndef NDEBUG
ubjs_result ubjs_prmtv_uint8_debug_string_get_length(ubjs_prmtv *this, unsigned int *plen)
{
    ubjs_prmtv_uint8_t *thisv;
    char tmp[11];

    if (0 == this || 0 == plen)
    {
        return UR_ERROR;
    }

    thisv = (ubjs_prmtv_uint8_t *)this;
    *plen = sprintf(tmp, "uint8(%u)", thisv->value);
    return UR_OK;
}

ubjs_result ubjs_prmtv_uint8_debug_string_copy(ubjs_prmtv *this, char *str)
{
    ubjs_prmtv_uint8_t *thisv;

    if (0 == this || 0 == str)
    {
        return UR_ERROR;
    }

    thisv = (ubjs_prmtv_uint8_t *)this;
    sprintf(str, "uint8(%u)", thisv->value);
    return UR_OK;
}
#endif

ubjs_result ubjs_prmtv_uint8_parser_processor_new(ubjs_library *lib,
     ubjs_prmtv_marker_parser_glue *glue, ubjs_prmtv_marker_parser_processor **pthis)
{
    ubjs_prmtv_uint8_parser_processor *this;
    ubjs_library_alloc_f alloc_f;

    if (0 == lib || 0 == glue || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(lib, &alloc_f);
    this = (ubjs_prmtv_uint8_parser_processor *)(alloc_f)(sizeof(
        struct ubjs_prmtv_uint8_parser_processor));
    this->super.lib = lib;
    this->super.marker = &ubjs_prmtv_uint8_marker;
    this->super.name = "uint8";
    this->super.glue = glue;
    this->did_read = UFALSE;
    *pthis = (ubjs_prmtv_marker_parser_processor *)this;
    return UR_OK;
}

ubjs_result ubjs_prmtv_uint8_parser_processor_free(
    ubjs_prmtv_marker_parser_processor **pthis)
{
    ubjs_prmtv_uint8_parser_processor *this;
    ubjs_library_free_f free_f;

    if (0 == pthis || 0 == *pthis)
    {
        return UR_ERROR;
    }

    this = (ubjs_prmtv_uint8_parser_processor *)*pthis;
    ubjs_library_get_free_f(this->super.lib, &free_f);
    (free_f)(this);
    *pthis = 0;
    return UR_OK;
}

void ubjs_prmtv_uint8_parser_processor_got_control(
    ubjs_prmtv_marker_parser_processor *this)
{
}

void ubjs_prmtv_uint8_parser_processor_read_byte(
    ubjs_prmtv_marker_parser_processor *this, uint8_t achr)
{
    ubjs_prmtv_uint8_parser_processor *this2;
    uint8_t value[1];
    uint8_t value2[1];
    ubjs_prmtv *ret;

    this2 = (ubjs_prmtv_uint8_parser_processor *)this;
    if (UTRUE == this2->did_read)
    {
        (this->glue->error_f)(this->glue, 19,
            "Too much bytes read");
        return;
    }

    this2->did_read = UTRUE;

    value[0] = achr;
    ubjs_endian_convert_big_to_native(value, value2, 1);
    ubjs_prmtv_uint8(this->lib, *((uint8_t *)value2), &ret);
    (this->glue->return_control_f)(this->glue, ret);
}

ubjs_result ubjs_prmtv_uint8_writer_new(ubjs_library *lib,
    ubjs_prmtv_marker_writer_glue *glue, ubjs_prmtv_marker_writer **pthis)
{
    ubjs_prmtv_marker_writer *this;
    ubjs_library_alloc_f alloc_f;

    if (0 == lib || 0 == glue || 0 == glue->prmtv || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(lib, &alloc_f);
    this = (ubjs_prmtv_marker_writer *)(alloc_f)(sizeof(
        struct ubjs_prmtv_marker_writer));
    this->lib = lib;
    this->marker = &ubjs_prmtv_uint8_marker;
    this->name = "uint8";
    this->glue = glue;
    *pthis = this;
    return UR_OK;
}

ubjs_result ubjs_prmtv_uint8_writer_free(ubjs_prmtv_marker_writer **pthis)
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

void ubjs_prmtv_uint8_writer_get_length(ubjs_prmtv_marker_writer *this,
    unsigned int *plen)
{
    *plen = 1;
}

void ubjs_prmtv_uint8_writer_do(ubjs_prmtv_marker_writer *this, uint8_t *data)
{
    ubjs_prmtv_uint8_t *thisv;
    uint8_t value1[1];
    uint8_t value2[1];

    thisv = (ubjs_prmtv_uint8_t *)this->glue->prmtv;
    memcpy(value1, (void *)&(thisv->value), sizeof(uint8_t));
    ubjs_endian_convert_native_to_big(value1, value2, 1);
    memcpy(data, value2, 1);
}

ubjs_result ubjs_prmtv_uint8_printer_new(ubjs_library *lib,
    ubjs_prmtv_marker_printer_glue *glue,
    ubjs_prmtv_marker_printer **pthis)
{
    ubjs_prmtv_marker_printer *this;
    ubjs_library_alloc_f alloc_f;

    if (0 == lib || 0 == glue || 0 == glue->prmtv || 0 == pthis)
    {
        return UR_ERROR;
    }

    ubjs_library_get_alloc_f(lib, &alloc_f);
    this = (ubjs_prmtv_marker_printer *)(alloc_f)(sizeof(
        struct ubjs_prmtv_marker_printer));
    this->lib = lib;
    this->marker = &ubjs_prmtv_uint8_marker;
    this->name = "uint8";
    this->glue = glue;
    *pthis = this;
    return UR_OK;
}

ubjs_result ubjs_prmtv_uint8_printer_free(ubjs_prmtv_marker_printer **pthis)
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

void ubjs_prmtv_uint8_printer_get_length(ubjs_prmtv_marker_printer *this,
    unsigned int *plen)
{
    ubjs_prmtv_uint8_t *thisv;
    char tmp[6];

    thisv = (ubjs_prmtv_uint8_t *)this->glue->prmtv;
    *plen = sprintf(tmp, "[%u]", thisv->value);
}

void ubjs_prmtv_uint8_printer_do(ubjs_prmtv_marker_printer *this, char *data)
{
    ubjs_prmtv_uint8_t *thisv;
    char tmp[6];
    thisv = (ubjs_prmtv_uint8_t *)this->glue->prmtv;
    sprintf(tmp, "[%u]", thisv->value);
    memcpy(data, tmp, strlen(tmp) * sizeof(char));
}

ubjs_result ubjs_prmtv_uint8_get(ubjs_prmtv *this, uint8_t *pvalue)
{
    ubjs_prmtv_uint8_t *thisv;

    if (0 == this || 0 == pvalue || &ubjs_prmtv_uint8_marker != this->marker)
    {
        return UR_ERROR;
    }

    thisv = (ubjs_prmtv_uint8_t *)this;
    *pvalue = thisv->value;
    return UR_OK;
}

ubjs_result ubjs_prmtv_uint8_get_value_int64(ubjs_prmtv *this, int64_t *pvalue)
{
    ubjs_prmtv_uint8_t *thisv;

    if (0 == this || 0 == pvalue || &ubjs_prmtv_uint8_marker != this->marker)
    {
        return UR_ERROR;
    }

    thisv = (ubjs_prmtv_uint8_t *)this;
    *pvalue = (int64_t) thisv->value;
    return UR_OK;
}

#pragma clang diagnostic pop