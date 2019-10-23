/**
 * Copyright 2019 Marcus Pinnecke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of
 * the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef HAD_JSON_FROM_CARBON_H
#define HAD_JSON_FROM_CARBON_H

#include <jakson/stdinc.h>
#include <jakson/std/string.h>

BEGIN_DECL

struct array;
struct column_it;
struct object_it;
struct carbon_json_from_opts;

/* built-in json formatter configuration */
static struct carbon_json_from_opts
{
    /* ignores all format options below and uses a specialized, fast formatter */
    bool force_fast_formatter: 1;

    /* print record key type */
    bool print_key_type : 1;
    /* print record key value */
    bool print_key_value : 1;
    /* print record commit hash value */
    bool print_commit_hash : 1;
    /* intention of blocks in number of spaces */
    unsigned block_intent;
    /* intention before '{' in number of spaces */
    unsigned object_begin_before_intent;
    /* intention after '{' in number of spaces */
    unsigned object_begin_after_intent;
    /* intention before '}' in number of spaces */
    unsigned object_end_before_intent;
    /* intention after '}' in number of spaces */
    unsigned object_end_after_intent;
    /* intention before '[' in number of spaces */
    unsigned array_begin_before_intent;
    /* intention after '[' in number of spaces */
    unsigned array_begin_after_intent;
    /* intention before ']' in number of spaces */
    unsigned array_end_before_intent;
    /* intention after ']' in number of spaces */
    unsigned array_end_after_intent;
    /* intention before a key name in number of spaces */
    unsigned key_before_intent;
    /* intention after a key name in number of spaces */
    unsigned key_after_intent;
    /* intention before a value in number of spaces */
    unsigned value_before_intent;
    /* intention after a value in number of spaces */
    unsigned value_after_intent;
    /* intention before an element in numeration in number of spaces */
    unsigned item_before_intent;
    /* intention after an element in numeration in number of spaces */
    unsigned item_after_intent;
    /* intention before a comma in numeration in number of spaces */
    unsigned comma_before_intent;
    /* intention after a comma in numeration in number of spaces */
    unsigned comma_after_intent;
    /* do a line break after opening an array, e.g., [... -> [\n... */
    bool array_begin_linebreak: 1;
    /* do a line break before closing an array, e.g., ...] -> ...\n] */
    bool array_end_linebreak: 1;
    /* do a line break after opening an object, e.g., {... -> {\n... */
    bool object_begin_linebreak: 1;
    /* do a line break before closing an object, e.g., ...} -> ...\n} */
    bool object_end_linebreak: 1;
    /* do not write JSON outer-most '{' and '}' */
    bool ignore_root_object_begin_end: 1;
    /* do not write JSON outer-most '[' and ']' */
    bool ignore_root_array_begin_end: 1;
    /* force JSON compatible structure for carbon records by potentially removing outer-most '[' and ']' */
    bool force_json_compatible_structure: 1;
    /* use Windows compatible line breaks, i.e., use \r\n instead of \n */
    bool windows_linebreaks: 1;
    /* do a line break after element in numeration, e.g., [1, 2] -> [1,\n2] */
    bool item_linebreak : 1;
    /* always enquote key-names even if they are C identifiers */
    bool force_equote_keys : 1;
    /* always enquote value strings even if they are C identifiers */
    bool force_equote_value : 1;
    /* precision of floating point number strings in number of digits after dot */
    unsigned floating_num_of_dots;
}
        CARBON_PRINT_JSON_PRETTY = {
        .force_fast_formatter = false,

        .print_key_type = false,
        .print_key_value = false,
        .print_commit_hash = false,
        .block_intent = 4,
        .object_begin_before_intent = 1,
        .object_begin_after_intent = 1,
        .object_end_before_intent = 1,
        .object_end_after_intent = 1,
        .array_begin_before_intent = 1,
        .array_begin_after_intent = 1,
        .array_end_before_intent = 1,
        .array_end_after_intent = 1,
        .key_before_intent = 1,
        .key_after_intent = 1,
        .value_before_intent = 1,
        .value_after_intent = 1,
        .item_before_intent = 1,
        .item_after_intent = 1,
        .comma_before_intent = 1,
        .comma_after_intent = 1,
        .array_begin_linebreak = 1,
        .array_end_linebreak = 1,
        .object_begin_linebreak = 1,
        .object_end_linebreak = 1,
        .force_json_compatible_structure = true,
        .ignore_root_object_begin_end = false,
        .ignore_root_array_begin_end = false,
        .windows_linebreaks = true,
        .item_linebreak = true,
        .force_equote_keys = true,
        .force_equote_value = true,
        .floating_num_of_dots = 2
},
        CARBON_PRINT_JSON_FAST = {      /* note that for this config, a specialized faster formatter is used*/
        .force_fast_formatter = true
};

/* Setup a custom formatter by setting 'config' to any non-null format configuration, or set 'config' to NULL
 * for using a built-in fast formatter. Use built-in formatter 'CARBON_PRINT_JSON_PRETTY' for 'config' to format
 * into a nice human readable form. */

void carbon_json_from_carbon(struct string_buffer *str, struct carbon *record,
                             struct carbon_json_from_opts *config);

void carbon_json_from_array(struct string_buffer *str, carbon_array *it,
                            struct carbon_json_from_opts *config);

void carbon_json_from_column(struct string_buffer *str, struct carbon_column_it *it,
                             struct carbon_json_from_opts *config);

void carbon_json_from_object(struct string_buffer *str, struct carbon_object_it *it,
                             struct carbon_json_from_opts *config);

END_DECL

#endif