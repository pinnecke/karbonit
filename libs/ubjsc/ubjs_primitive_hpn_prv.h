/*
 * Copyright (c) 2016 Tomasz Sieprawski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without rehpniction, including without limitation the rights
 * to use, copy, modify, merge, publish, dihpnibute, sublicense, and/or sell
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
#ifndef HAVE_UBJS_PRIMITIVE_HPN_PRV
#define HAVE_UBJS_PRIMITIVE_HPN_PRV

#include "ubjs_primitive_hpn.h"

typedef struct ubjs_prmtv_hpn_t ubjs_prmtv_hpn_t;
typedef struct ubjs_prmtv_hpn_parser_processor ubjs_prmtv_hpn_parser_processor;
typedef struct ubjs_prmtv_hpn_writer ubjs_prmtv_hpn_writer;
typedef struct ubjs_prmtv_hpn_printer ubjs_prmtv_hpn_printer;

enum ubjs_prmtv_hpn_parser_processor_phase
{
    UPSPPP_INIT,
    UPSPPP_WANT_NUMBER_MARKER,
    UPSPPP_WANT_NUMBER_CHILD,
    UPSPPP_GATHERING_BYTES,
    UPSPPP_DONE
};

struct ubjs_prmtv_hpn_t
{
    ubjs_prmtv super;

    unsigned int len;
    char *value;
};

struct ubjs_prmtv_hpn_parser_processor
{
    ubjs_prmtv_marker_parser_processor super;
    unsigned int len;
    unsigned int read;
    char *data;
    enum ubjs_prmtv_hpn_parser_processor_phase phase;
    ubjs_prmtv_marker *number_marker;
    ubjs_glue_array *legal_number_markers;
};

struct ubjs_prmtv_hpn_writer
{
    ubjs_prmtv_marker_writer super;
    ubjs_prmtv_marker_writer *length_writer;
    ubjs_prmtv_marker_writer_glue *length_glue;
};

struct ubjs_prmtv_hpn_printer
{
    ubjs_prmtv_marker_printer super;
    ubjs_prmtv_marker_printer *length_printer;
    ubjs_prmtv_marker_printer_glue *length_glue;
};

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_hpn_free(ubjs_prmtv **);
#ifndef NDEBUG
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_hpn_debug_string_get_length(ubjs_prmtv *, unsigned int *);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_hpn_debug_string_copy(ubjs_prmtv *, char *);
#endif
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_hpn_parser_processor_new(ubjs_library *,
     ubjs_prmtv_marker_parser_glue *, ubjs_prmtv_marker_parser_processor **);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_hpn_parser_processor_free(
    ubjs_prmtv_marker_parser_processor **);
UBJS_NO_EXPORT void ubjs_prmtv_hpn_parser_processor_got_child(
    ubjs_prmtv_marker_parser_processor *, ubjs_prmtv *);
UBJS_NO_EXPORT void ubjs_prmtv_hpn_parser_processor_got_marker(
    ubjs_prmtv_marker_parser_processor *, ubjs_prmtv_marker *);
UBJS_NO_EXPORT void ubjs_prmtv_hpn_parser_processor_got_control(
    ubjs_prmtv_marker_parser_processor *);
UBJS_NO_EXPORT void ubjs_prmtv_hpn_parser_processor_read_byte(
    ubjs_prmtv_marker_parser_processor *, uint8_t);

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_hpn_writer_new(ubjs_library *,
    ubjs_prmtv_marker_writer_glue *, ubjs_prmtv_marker_writer **);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_hpn_writer_free(ubjs_prmtv_marker_writer **);
UBJS_NO_EXPORT void ubjs_prmtv_hpn_writer_get_length(ubjs_prmtv_marker_writer *,
    unsigned int *);
UBJS_NO_EXPORT void ubjs_prmtv_hpn_writer_do(ubjs_prmtv_marker_writer *, uint8_t *);

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_hpn_printer_new(ubjs_library *,
    ubjs_prmtv_marker_printer_glue *, ubjs_prmtv_marker_printer **);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_hpn_printer_free(ubjs_prmtv_marker_printer **);
UBJS_NO_EXPORT void ubjs_prmtv_hpn_printer_get_length(ubjs_prmtv_marker_printer *,
    unsigned int *);
UBJS_NO_EXPORT void ubjs_prmtv_hpn_printer_do(ubjs_prmtv_marker_printer *, char *);

enum ubjs_prmtv_is_valid_hpn_state
{
    PMIVHS_BEGIN,
    PMIVHS_AFTER_MINUS,
    PMIVHS_AFTER_DIGIT,
    PMIVHS_AFTER_DIGITS,
    PMIVHS_AFTER_DOT_BEFORE_DIGITS,
    PMIVHS_AFTER_NUMBER,
    PMIVHS_AFTER_E,
    PMIVHS_AFTER_E_PLUS_MINUS,
    PMIVHS_AFTER_E_DIGIT,
    PMIVHS_END
};

typedef ubjs_result (*ubjs_prmtv_is_valid_hpn_state_processor_f)(char,
    enum ubjs_prmtv_is_valid_hpn_state *);

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_is_valid_hpn_state_processor_begin(char,
    enum ubjs_prmtv_is_valid_hpn_state *);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_is_valid_hpn_state_processor_after_minus(char,
    enum ubjs_prmtv_is_valid_hpn_state *);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_is_valid_hpn_state_processor_after_digit(char,
    enum ubjs_prmtv_is_valid_hpn_state *);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_is_valid_hpn_state_processor_after_digits(char,
    enum ubjs_prmtv_is_valid_hpn_state *);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_is_valid_hpn_state_processor_after_dot_before_digits(char,
    enum ubjs_prmtv_is_valid_hpn_state *);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_is_valid_hpn_state_processor_after_number(char,
    enum ubjs_prmtv_is_valid_hpn_state *);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_is_valid_hpn_state_processor_after_e(char,
    enum ubjs_prmtv_is_valid_hpn_state *);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_is_valid_hpn_state_processor_after_e_plus_minus(char,
    enum ubjs_prmtv_is_valid_hpn_state *);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_is_valid_hpn_state_processor_after_e_digit(char,
    enum ubjs_prmtv_is_valid_hpn_state *);
UBJS_NO_EXPORT ubjs_result ubjs_prmtv_is_valid_hpn_state_processor_after_end(char,
    enum ubjs_prmtv_is_valid_hpn_state *);

extern unsigned int \
    ubjs_prmtv_is_valid_hpn_state_processor_matrix_length;
extern ubjs_prmtv_is_valid_hpn_state_processor_f \
    ubjs_prmtv_is_valid_hpn_state_processor_matrix[];

UBJS_NO_EXPORT ubjs_result ubjs_prmtv_is_valid_hpn(unsigned int, char *, ubjs_bool *);

#endif
/* \endinternal */
