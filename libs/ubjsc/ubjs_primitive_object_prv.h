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
#ifndef HAVE_UBJS_PRIMITIVE_object_PRV
#define HAVE_UBJS_PRIMITIVE_object_PRV

#include "ubjs_primitive_object.h"

typedef struct ubjs_prmtv_object_t ubjs_prmtv_object_t;
typedef struct ubjs_prmtv_object_parser_processor ubjs_prmtv_object_parser_processor;
typedef struct ubjs_prmtv_object_writer ubjs_prmtv_object_writer;
typedef struct ubjs_prmtv_object_printer ubjs_prmtv_object_printer;

enum ubjs_prmtv_object_parser_processor_phase
{
    UPOPPP_INIT,
    UPOPPP_WANT_TYPE_COUNT_KEY_END_MARKER,
    UPOPPP_WANT_TYPE_MARKER,
    UPOPPP_WANT_COUNT_MARKER,
    UPOPPP_WANT_COUNT_PRMTV_MARKER,
    UPOPPP_WANT_COUNT_PRMTV_VALUE,
    UPOPPP_WANT_KEY_LENGTH_PRMTV_MARKER_END,
    UPOPPP_WANT_KEY_LENGTH_PRMTV_MARKER,
    UPOPPP_WANT_KEY_LENGTH_PRMTV_VALUE,
    UPOPPP_GATHERING_KEY_VALUE,
    UPOPPP_WANT_CHILD_PRMTV_MARKER,
    UPOPPP_WANT_CHILD_PRMTV_VALUE,
    UPOPPP_DONE
};

struct ubjs_prmtv_object_t
{
    ubjs_prmtv super;
    ubjs_glue_dict *glue;
};

struct ubjs_object_iterator
{
    ubjs_prmtv_object_t *object;
    ubjs_glue_dict_iterator *glue;
};

struct ubjs_prmtv_object_parser_processor
{
    ubjs_prmtv_marker_parser_processor super;
    ubjs_prmtv *data;
    unsigned int len;
    unsigned int expected_len;
    enum ubjs_prmtv_object_parser_processor_phase phase;

    ubjs_prmtv_marker *type_marker;
    ubjs_prmtv_marker *count_marker;

    unsigned int key_length;
    unsigned int key_at;
    char *key_value;

    ubjs_glue_array *legal_markers_type_count_int64s_end;
    ubjs_glue_array *legal_markers_count;
    ubjs_glue_array *legal_markers_int64s;
    ubjs_glue_array *legal_markers_int64s_end;
    ubjs_glue_array *legal_markers_top;
};

struct ubjs_prmtv_object_writer
{
    ubjs_prmtv_marker_writer super;
    unsigned int len;

    ubjs_prmtv_marker *type_marker;

    ubjs_prmtv_marker_writer_glue *count_writer_glue;
    ubjs_prmtv_marker_writer *count_writer;
    unsigned int count_length;

    char **key_values;
    unsigned int *key_lengths;
    ubjs_prmtv_marker_writer **key_writers;
    ubjs_prmtv_marker_writer_glue **key_writers_glues;

    ubjs_prmtv **value_prmtvs;
    ubjs_bool *value_got_upgraded;
    ubjs_prmtv_marker_writer **value_writers;
    unsigned int *value_lengths;
    ubjs_prmtv_marker_writer_glue **value_writers_glues;
};

struct ubjs_prmtv_object_printer
{
    ubjs_prmtv_marker_printer super;
    unsigned int len;

    ubjs_prmtv_marker *type_marker;

    ubjs_prmtv_marker_printer_glue *count_printer_glue;
    ubjs_prmtv_marker_printer *count_printer;
    unsigned int count_length;

    char **key_values;
    unsigned int *key_lengths;
    ubjs_prmtv_marker_printer **key_printers;
    ubjs_prmtv_marker_printer_glue **key_printers_glues;

    ubjs_prmtv **value_prmtvs;
    ubjs_bool *value_got_upgraded;
    ubjs_prmtv_marker_printer **value_printers;
    unsigned int *value_lengths;
    ubjs_prmtv_marker_printer_glue **value_printers_glues;
};

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_object_free(ubjs_prmtv **);

#ifndef NDEBUG
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_object_debug_string_get_length(ubjs_prmtv *, unsigned int *);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_object_debug_string_copy(ubjs_prmtv *, char *);
#endif

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_object_parser_processor_new(ubjs_library *,
     ubjs_prmtv_marker_parser_glue *, ubjs_prmtv_marker_parser_processor **);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_object_parser_processor_free(
    ubjs_prmtv_marker_parser_processor **);
UBJS_NO_EXPORT void ubjs_prmtv_object_parser_processor_got_child(
    ubjs_prmtv_marker_parser_processor *, ubjs_prmtv *);
UBJS_NO_EXPORT void ubjs_prmtv_object_parser_processor_got_marker(
    ubjs_prmtv_marker_parser_processor *, ubjs_prmtv_marker *);
UBJS_NO_EXPORT void ubjs_prmtv_object_parser_processor_got_control(
    ubjs_prmtv_marker_parser_processor *);
UBJS_NO_EXPORT void ubjs_prmtv_object_parser_processor_read_byte(
    ubjs_prmtv_marker_parser_processor *, uint8_t);

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_object_writer_new(ubjs_library *,
    ubjs_prmtv_marker_writer_glue *, ubjs_prmtv_marker_writer **);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_object_writer_free(ubjs_prmtv_marker_writer **);
UBJS_NO_EXPORT void ubjs_prmtv_object_writer_get_length(ubjs_prmtv_marker_writer *,
    unsigned int *);
UBJS_NO_EXPORT void ubjs_prmtv_object_writer_do(ubjs_prmtv_marker_writer *, uint8_t *);

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_object_printer_new(ubjs_library *,
    ubjs_prmtv_marker_printer_glue *, ubjs_prmtv_marker_printer **);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_object_printer_free(ubjs_prmtv_marker_printer **);
UBJS_NO_EXPORT void ubjs_prmtv_object_printer_get_length(ubjs_prmtv_marker_printer *,
    unsigned int *);
UBJS_NO_EXPORT void ubjs_prmtv_object_printer_do(ubjs_prmtv_marker_printer *, char *);

#endif
/* \endinternal */
