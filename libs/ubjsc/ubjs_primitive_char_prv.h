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
#ifndef HAVE_UBJS_PRIMITIVE_CHAR_PRV
#define HAVE_UBJS_PRIMITIVE_CHAR_PRV

#include "ubjs_primitive_char.h"

typedef struct ubjs_prmtv_char_t ubjs_prmtv_char_t;
typedef struct ubjs_prmtv_char_parser_processor ubjs_prmtv_char_parser_processor;
struct ubjs_prmtv_char_t
{
    ubjs_prmtv super;
    unsigned char value;
};
struct ubjs_prmtv_char_parser_processor
{
    ubjs_prmtv_marker_parser_processor super;
    ubjs_bool did_read;
};

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_char_free(ubjs_prmtv **);

#ifndef NDEBUG
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_char_debug_string_get_length(ubjs_prmtv *, unsigned int *);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_char_debug_string_copy(ubjs_prmtv *, char *);
#endif

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_char_parser_processor_new(ubjs_library *,
     ubjs_prmtv_marker_parser_glue *, ubjs_prmtv_marker_parser_processor **);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_char_parser_processor_free(
    ubjs_prmtv_marker_parser_processor **);
UBJS_NO_EXPORT void ubjs_prmtv_char_parser_processor_got_control(
    ubjs_prmtv_marker_parser_processor *);
UBJS_NO_EXPORT void ubjs_prmtv_char_parser_processor_read_byte(
    ubjs_prmtv_marker_parser_processor *, uint8_t);

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_char_writer_new(ubjs_library *,
    ubjs_prmtv_marker_writer_glue *, ubjs_prmtv_marker_writer **);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_char_writer_free(ubjs_prmtv_marker_writer **);
UBJS_NO_EXPORT void ubjs_prmtv_char_writer_get_length(ubjs_prmtv_marker_writer *,
    unsigned int *);
UBJS_NO_EXPORT void ubjs_prmtv_char_writer_do(ubjs_prmtv_marker_writer *, uint8_t *);

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_char_printer_new(ubjs_library *,
    ubjs_prmtv_marker_printer_glue *, ubjs_prmtv_marker_printer **);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_char_printer_free(ubjs_prmtv_marker_printer **);
UBJS_NO_EXPORT void ubjs_prmtv_char_printer_get_length(ubjs_prmtv_marker_printer *,
    unsigned int *);
UBJS_NO_EXPORT void ubjs_prmtv_char_printer_do(ubjs_prmtv_marker_printer *, char *);

#endif
/* \endinternal */
