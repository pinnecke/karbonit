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

#ifndef HAVE_UBJS_PARSER_PRV
#define HAVE_UBJS_PARSER_PRV

#include "ubjs_parser.h"
#include "ubjs_common.h"
#include "ubjs_selfemptying_list_prv.h"
#include "ubjs_primitives_prv.h"

typedef struct ubjs_parser_give_control_request ubjs_parser_give_control_request;
typedef struct ubjs_processor ubjs_processor;
typedef struct ubjs_processor_next_prmtv_t ubjs_processor_next_prmtv_t;
typedef struct ubjs_processor_marker_t ubjs_processor_marker_t;
typedef struct ubjs_parser_counters ubjs_parser_counters;

typedef void (*ubjs_processor_free)(ubjs_processor *);
typedef void (*ubjs_processor_got_control)(ubjs_processor *, ubjs_parser_give_control_request *);
typedef void (*ubjs_processor_read_byte)(ubjs_processor *, unsigned int, uint8_t);
typedef ubjs_result (*ubjs_processor_next_prmtv_selected_factory_marker)(ubjs_processor *,
    ubjs_prmtv_marker *);

struct ubjs_parser_give_control_request
{
    ubjs_library *lib;
    ubjs_processor *processor;

    ubjs_prmtv *present;
    ubjs_prmtv_marker *marker;
};

struct ubjs_processor
{
    char *name;
    ubjs_processor *parent;
    ubjs_parser *parser;
    unsigned int recursion_level;

    ubjs_processor_got_control got_control;
    ubjs_processor_read_byte read_byte;
    ubjs_processor_free free;
};

struct ubjs_parser_counters
{
    unsigned int bytes_since_last_callback;
};

struct ubjs_parser_builder
{
    ubjs_library *lib;

    void *userdata;
    ubjs_parser_parsed_f parsed_f;
    ubjs_parser_error_f error_f;
    ubjs_parser_free_f free_f;
    ubjs_parser_debug_f debug_f;
    unsigned int limit_bytes_since_last_callback;
    unsigned int limit_container_length;
    unsigned int limit_string_length;
    unsigned int limit_recursion_level;
    ubjs_bool silently_ignore_toplevel_noops;
};

struct ubjs_parser
{
    ubjs_library *lib;

    void *userdata;
    ubjs_parser_parsed_f parsed_f;
    ubjs_parser_error_f error_f;
    ubjs_parser_free_f free_f;
    ubjs_parser_debug_f debug_f;
    unsigned int limit_bytes_since_last_callback;
    unsigned int limit_container_length;
    unsigned int limit_string_length;
    unsigned int limit_recursion_level;
    ubjs_bool silently_ignore_toplevel_noops;

    ubjs_selfemptying_list *give_control_fifo;
    unsigned int errors;

    ubjs_processor *processor;
    ubjs_parser_counters counters;
};

struct ubjs_processor_next_prmtv_t
{
    ubjs_processor super;
    ubjs_glue_array *markers;
    ubjs_processor_next_prmtv_selected_factory_marker selected_factory_marker;
};

struct ubjs_processor_marker_t
{
    ubjs_processor super;
    ubjs_prmtv_marker *marker;
    ubjs_prmtv_marker_parser_glue glue;
    ubjs_prmtv_marker_parser_processor *processor;
};

UBJS_NO_EXPORT void ubjs_parser_give_control_request_free(ubjs_parser_give_control_request *);
UBJS_NO_EXPORT void ubjs_parser_give_control_fifo_callback(ubjs_selfemptying_list *, void *);
UBJS_NO_EXPORT void ubjs_parser_give_control(ubjs_parser *, ubjs_processor *, ubjs_prmtv *,
    ubjs_prmtv_marker *);

UBJS_NO_EXPORT void ubjs_parser_emit_error(ubjs_parser *, unsigned int, char *);

UBJS_NO_EXPORT void ubjs_processor_top(ubjs_parser *);
UBJS_NO_EXPORT void ubjs_processor_next_prmtv(ubjs_processor *,
    ubjs_glue_array *, ubjs_processor_next_prmtv_selected_factory_marker);

UBJS_NO_EXPORT ubjs_result ubjs_processor_marker(ubjs_processor *, ubjs_prmtv_marker *,
    ubjs_processor **);
UBJS_NO_EXPORT void ubjs_processor_marker_got_control(ubjs_processor *,
    ubjs_parser_give_control_request *);
UBJS_NO_EXPORT void ubjs_processor_marker_free(ubjs_processor *);
UBJS_NO_EXPORT void ubjs_processor_marker_read_byte(ubjs_processor *, unsigned int, uint8_t);
UBJS_NO_EXPORT void ubjs_processor_marker_return_control(ubjs_prmtv_marker_parser_glue *,
    void *);
UBJS_NO_EXPORT void ubjs_processor_marker_want_marker(ubjs_prmtv_marker_parser_glue *,
    ubjs_glue_array *);
UBJS_NO_EXPORT void ubjs_processor_marker_want_child(ubjs_prmtv_marker_parser_glue *,
    ubjs_prmtv_marker *);
UBJS_NO_EXPORT void ubjs_processor_marker_debug(ubjs_prmtv_marker_parser_glue *,
    unsigned int, char *);
UBJS_NO_EXPORT void ubjs_processor_marker_error(ubjs_prmtv_marker_parser_glue *,
    unsigned int, char *);

UBJS_NO_EXPORT void ubjs_processor_top_got_control(ubjs_processor *,
    ubjs_parser_give_control_request *);
UBJS_NO_EXPORT ubjs_result ubjs_processor_top_selected_marker(ubjs_processor *,
    ubjs_prmtv_marker *);

UBJS_NO_EXPORT void ubjs_processor_next_prmtv_read_byte(ubjs_processor *, unsigned int, uint8_t);
UBJS_NO_EXPORT void ubjs_processor_next_prmtv_free(ubjs_processor *);

/* \endinternal */

#endif
