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

#ifndef HAVE_UBJS_WRITER_PRV
#define HAVE_UBJS_WRITER_PRV

#include "ubjs_writer.h"

struct ubjs_writer_builder
{
    ubjs_library *lib;

    void *userdata;
    ubjs_writer_would_write_f would_write_f;
    ubjs_writer_would_print_f would_print_f;
    ubjs_writer_debug_f debug_f;
    ubjs_bool free_primitives_early;
    ubjs_writer_free_f free_f;
};

struct ubjs_writer
{
    ubjs_library *lib;

    void *userdata;
    ubjs_writer_would_write_f would_write_f;
    ubjs_writer_would_print_f would_print_f;
    ubjs_writer_debug_f debug_f;
    ubjs_bool free_primitives_early;
    ubjs_writer_free_f free_f;
};

UBJS_NO_EXPORT void ubjs_writer_write_marker_glue_debug(ubjs_prmtv_marker_writer_glue *,
    unsigned int, char *);
UBJS_NO_EXPORT void ubjs_writer_print_marker_glue_debug(ubjs_prmtv_marker_printer_glue *,
    unsigned int, char *);

/* \endinternal */

#endif
