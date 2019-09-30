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
#ifndef HAVE_UBJS_PRIMITIVE_STR_PRV
#define HAVE_UBJS_PRIMITIVE_STR_PRV

#include "ubjs_primitive_str.h"

typedef struct ubjs_prmtv_str_t ubjs_prmtv_str_t;
typedef struct ubjs_prmtv_str_parser_processor ubjs_prmtv_str_parser_processor;
typedef struct ubjs_prmtv_str_writer ubjs_prmtv_str_writer;
typedef struct ubjs_prmtv_str_printer ubjs_prmtv_str_printer;

enum ubjs_prmtv_str_parser_processor_phase
{
    UPSPPP_INIT,
    UPSPPP_WANT_NUMBER_MARKER,
    UPSPPP_WANT_NUMBER_CHILD,
    UPSPPP_GATHERING_BYTES,
    UPSPPP_DONE
};

struct ubjs_prmtv_str_t
{
    ubjs_prmtv super;

    unsigned int len;
    char *value;
};

struct ubjs_prmtv_str_parser_processor
{
    ubjs_prmtv_marker_parser_processor super;
    unsigned int len;
    unsigned int read;
    char *data;
    enum ubjs_prmtv_str_parser_processor_phase phase;
    ubjs_prmtv_marker *number_marker;
    ubjs_glue_array *legal_number_markers;
};

struct ubjs_prmtv_str_writer
{
    ubjs_prmtv_marker_writer super;
    ubjs_prmtv_marker_writer *length_writer;
    ubjs_prmtv_marker_writer_glue *length_glue;
};

struct ubjs_prmtv_str_printer
{
    ubjs_prmtv_marker_printer super;
    ubjs_prmtv_marker_printer *length_printer;
    ubjs_prmtv_marker_printer_glue *length_glue;
};

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_str_free(ubjs_prmtv **);
#ifndef NDEBUG
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_str_debug_string_get_length(ubjs_prmtv *, unsigned int *);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_str_debug_string_copy(ubjs_prmtv *, char *);
#endif
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_str_parser_processor_new(ubjs_library *,
     ubjs_prmtv_marker_parser_glue *, ubjs_prmtv_marker_parser_processor **);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_str_parser_processor_free(
    ubjs_prmtv_marker_parser_processor **);
UBJS_NO_EXPORT void ubjs_prmtv_str_parser_processor_got_child(
    ubjs_prmtv_marker_parser_processor *, ubjs_prmtv *);
UBJS_NO_EXPORT void ubjs_prmtv_str_parser_processor_got_marker(
    ubjs_prmtv_marker_parser_processor *, ubjs_prmtv_marker *);
UBJS_NO_EXPORT void ubjs_prmtv_str_parser_processor_got_control(
    ubjs_prmtv_marker_parser_processor *);
UBJS_NO_EXPORT void ubjs_prmtv_str_parser_processor_read_byte(
    ubjs_prmtv_marker_parser_processor *, uint8_t);

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_str_writer_new(ubjs_library *,
    ubjs_prmtv_marker_writer_glue *, ubjs_prmtv_marker_writer **);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_str_writer_free(ubjs_prmtv_marker_writer **);
UBJS_NO_EXPORT void ubjs_prmtv_str_writer_get_length(ubjs_prmtv_marker_writer *,
    unsigned int *);
UBJS_NO_EXPORT void ubjs_prmtv_str_writer_do(ubjs_prmtv_marker_writer *, uint8_t *);

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_str_printer_new(ubjs_library *,
    ubjs_prmtv_marker_printer_glue *, ubjs_prmtv_marker_printer **);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_str_printer_free(ubjs_prmtv_marker_printer **);
UBJS_NO_EXPORT void ubjs_prmtv_str_printer_get_length(ubjs_prmtv_marker_printer *,
    unsigned int *);
UBJS_NO_EXPORT void ubjs_prmtv_str_printer_do(ubjs_prmtv_marker_printer *, char *);

#endif
/* \endinternal */
