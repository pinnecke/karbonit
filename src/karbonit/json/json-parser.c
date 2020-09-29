/**
 * Copyright 2018 Marcus Pinnecke
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

#include <inttypes.h>
#include <ctype.h>
#include <locale.h>
#include <fcntl.h>
#include <karbonit/json/json-parser.h>
#include <karbonit/archive/doc.h>
#include <karbonit/utils/convert.h>
#include <karbonit/utils/numbers.h>
#include <karbonit/json/json-parser.h>
#include <karbonit/rec.h>
#include <karbonit/carbon/internal.h>
#include <karbonit/karbonit.h>
#include "json-parser.h"
#include <karbonit/std/hash.h>

static struct {
        json_token_e token;
        const char *string;
} JSON_TOKEN_STRINGS[] = {{.token = OBJECT_OPEN, .string = "OBJECT_OPEN"},
                              {.token = OBJECT_CLOSE, .string = "OBJECT_CLOSE"},
                              {.token = LITERAL_STRING, .string = "JSON_TOKEN_STRING"},
                              {.token = LITERAL_INT, .string = "LITERAL_INT"},
                              {.token = LITERAL_FLOAT, .string = "LITERAL_FLOAT"},
                              {.token = LITERAL_TRUE, .string = "LITERAL_TRUE"},
                              {.token = LITERAL_FALSE, .string = "LITERAL_FALSE"},
                              {.token = LITERAL_NULL, .string = "LITERAL_NULL"},
                              {.token = COMMA, .string = "COMMA"},
                              {.token = ASSIGN, .string = "JSON_TOKEN_ASSIGMENT"},
                              {.token = ARRAY_OPEN, .string = "ARRAY_OPEN"},
                              {.token = ARRAY_CLOSE, .string = "ARRAY_CLOSE"},
                              {.token = JSON_UNKNOWN, .string = "JSON_UNKNOWN"}};

struct token_memory {
        json_token_e type;
        bool init;
};

static int process_token(json_err *error_desc, const json_token *token,
                         vec ofType(json_token_e) *brackets, struct token_memory *token_mem);

static int set_error(json_err *error_desc, const json_token *token, const char *msg);

bool json_tokenizer_init(json_tokenizer *tokenizer, const char *input)
{
        tokenizer->cursor = input;
        tokenizer->token =
                (json_token) {.type = JSON_UNKNOWN, .length = 0, .column = 0, .line = 1, .string = NULL};
        tokenizer->charcount = strlen(tokenizer->cursor);
        return true;
}

bool json_tokenizer_init_limited(json_tokenizer *tokenizer, const char *input, size_t charcount)
{
    tokenizer->cursor = input;
    tokenizer->token =
            (json_token) {.type = JSON_UNKNOWN, .length = 0, .column = 0, .line = 1, .string = NULL};
    tokenizer->charcount = charcount;
    return true;
}

static void
parse_string_token(json_tokenizer *tokenizer, char c, char delimiter, char delimiter2, char delimiter3,
                   bool include_start, bool include_end)
{
        size_t step = 0;

        tokenizer->token.type = LITERAL_STRING;
        if (!include_start) {
            tokenizer->token.string++;
        }
        tokenizer->token.column++;
        c = *(++tokenizer->cursor);
        tokenizer->charcount--;

        while ((c != delimiter && c != delimiter2 && c != delimiter3) && c != '\r' && c != '\n') {
            if(c == '\\')
            {
                //skip next char
                tokenizer->token.length++;
                c = *(++tokenizer->cursor);
                tokenizer->charcount--;
            }
            tokenizer->token.length++;
            c = *(++tokenizer->cursor);
            tokenizer->charcount--;
        }

        if (include_end) {
            tokenizer->token.length++;
        } else {
            tokenizer->cursor++;
            tokenizer->charcount--;
        }

        step = (c == '\r' || c == '\n') ? 1 : 0;
        tokenizer->cursor += step;
        tokenizer->charcount -= step;
}

const json_token *json_tokenizer_next(json_tokenizer *tokenizer)
{
        size_t step = 0;

        if (LIKELY(tokenizer->charcount != 0)) {
                char c = *tokenizer->cursor;
                tokenizer->token.string = tokenizer->cursor;
                tokenizer->token.column += tokenizer->token.length;
                tokenizer->token.length = 0;
                if (c == '\n' || c == '\r') {
                        tokenizer->token.line += c == '\n' ? 1 : 0;
                        tokenizer->token.column = c == '\n' ? 0 : tokenizer->token.column;
                        tokenizer->cursor++;
                        tokenizer->charcount--;
                        return json_tokenizer_next(tokenizer);
                } else if (isspace(c)) {
                        do {
                                tokenizer->cursor++;
                                tokenizer->charcount--;
                                tokenizer->token.column++;
                        } while (isspace(c = *tokenizer->cursor) && c != '\n');
                        return json_tokenizer_next(tokenizer);
                } else if (c == '{' || c == '}' || c == '[' || c == ']' || c == ':' || c == ',') {
                        tokenizer->token.type =
                                c == '{' ? OBJECT_OPEN : c == '}' ? OBJECT_CLOSE : c == '[' ? ARRAY_OPEN : c == ']'
                                                                                                           ? ARRAY_CLOSE
                                                                                                           : c == ':'
                                                                                                             ? ASSIGN
                                                                                                             : COMMA;
                        tokenizer->token.column++;
                        tokenizer->token.length = 1;
                        tokenizer->cursor++;
                        tokenizer->charcount--;
                } else if (c != '"' && (isalpha(c) || c == '_') &&
                           (tokenizer->charcount >= 4 && (strncmp(tokenizer->cursor, "null", 4) != 0 &&
                                                               strncmp(tokenizer->cursor, "true", 4) != 0)) &&
                           (tokenizer->charcount >= 5 && strncmp(tokenizer->cursor, "false", 5) != 0)) {
                        parse_string_token(tokenizer, c, ' ', ':', ',', true, true);
                } else if (c == '"') {
                        parse_string_token(tokenizer, c, '"', '"', '"', false, false);
                } else if (c == 't' || c == 'f' || c == 'n') {
                        const unsigned lenTrueNull = 4;
                        const unsigned lenFalse = 5;
                        const unsigned cursorLen = tokenizer->charcount;
                        if (cursorLen >= lenTrueNull && strncmp(tokenizer->cursor, "true", lenTrueNull) == 0) {
                                tokenizer->token.type = LITERAL_TRUE;
                                tokenizer->token.length = lenTrueNull;
                        } else if (cursorLen >= lenFalse && strncmp(tokenizer->cursor, "false", lenFalse) == 0) {
                                tokenizer->token.type = LITERAL_FALSE;
                                tokenizer->token.length = lenFalse;
                        } else if (cursorLen >= lenTrueNull && strncmp(tokenizer->cursor, "null", lenTrueNull) == 0) {
                                tokenizer->token.type = LITERAL_NULL;
                                tokenizer->token.length = lenTrueNull;
                        } else {
                                goto caseTokenUnknown;
                        }
                        tokenizer->token.column++;
                        tokenizer->cursor += tokenizer->token.length;
                        tokenizer->charcount -= tokenizer->token.length;
                } else if (c == '-' || isdigit(c)) {
                        unsigned fracFound = 0, expFound = 0, plusMinusFound = 0;
                        bool plusMinusAllowed = false;
                        bool onlyDigitsAllowed = false;
                        do {
                                onlyDigitsAllowed |= plusMinusAllowed;
                                plusMinusAllowed = (expFound == 1);
                                c = *(++tokenizer->cursor);
                                tokenizer->charcount--;
                                fracFound += c == '.';
                                expFound += (c == 'e') || (c == 'E');
                                plusMinusFound += plusMinusAllowed && ((c == '+') || (c == '-')) ? 1 : 0;
                                tokenizer->token.length++;
                        } while ((((isdigit(c)) || (c == '.' && fracFound <= 1)
                                   || (plusMinusAllowed && (plusMinusFound <= 1) && ((c == '+') || (c == '-')))
                                   || ((c == 'e' || c == 'E') && expFound <= 1))) && c != '\n' && c != '\r');

                        if (!isdigit(*(tokenizer->cursor - 1))) {
                                tokenizer->token.column -= tokenizer->token.length;
                                goto caseTokenUnknown;
                        }

                        step = (c == '\r' || c == '\n') ? 1 : 0;
                        tokenizer->cursor += step;
                        tokenizer->charcount -= step;
                        tokenizer->token.type = fracFound ? LITERAL_FLOAT : LITERAL_INT;
                } else {
                        caseTokenUnknown:
                        tokenizer->token.type = JSON_UNKNOWN;
                        tokenizer->token.column++;
                        tokenizer->token.length = tokenizer->charcount;
                        tokenizer->cursor += tokenizer->token.length;
                        tokenizer->charcount -= tokenizer->token.length;
                }
                return &tokenizer->token;
        } else {
                return NULL;
        }
}

void json_token_dup(json_token *dst, const json_token *src)
{
        assert(dst);
        assert(src);
        memcpy(dst, src, sizeof(json_token));
}

void json_token_print(FILE *file, const json_token *token)
{
        char *string = MALLOC(token->length + 1);
        strncpy(string, token->string, token->length);
        string[token->length] = '\0';
        fprintf(file,
                "{\"type\": \"%s\", \"line\": %d, \"column\": %d, \"length\": %d, \"text\": \"%s\"}",
                JSON_TOKEN_STRINGS[token->type].string,
                token->line,
                token->column,
                token->length,
                string);
        free(string);
}

static bool parse_object(json_object *object, vec ofType(json_token) *token_stream, size_t *token_idx);

static bool parse_array(json_array *array, vec ofType(json_token) *token_stream, size_t *token_idx);

static void parse_string(json_string *string, vec ofType(json_token) *token_stream,
                         size_t *token_idx);

static void parse_number(json_number *number, vec ofType(json_token) *token_stream,
                         size_t *token_idx);

static bool parse_object_exp(json_parser * parser, json_object *object, json_err *error_desc, struct token_memory *token_mem, size_t cap_elems);

static bool parse_array_exp(json_parser * parser, json_array *array, json_err *error_desc, struct token_memory *token_mem, size_t cap_elems);

static void parse_string_exp(json_string *string, json_token token);

static void parse_number_exp(json_number *number, json_token token);

static bool parse_element(json_element *element, vec ofType(json_token) *token_stream, size_t *token_idx);

static bool parse_elements(json_elements *elements, vec ofType(json_token) *token_stream, size_t *token_idx);

static bool parse_token_stream(json *json, vec ofType(json_token) *token_stream);

static json_token get_token(vec ofType(json_token) *token_stream, size_t token_idx);

static void connect_child_and_parents_member(json_prop *member);

static void connect_child_and_parents_object(json_object *object);

static void connect_child_and_parents_array(json_array *array);

static void connect_child_and_parents_value(json_node_value *value);

static void connect_child_and_parents_element(json_element *element);

static void connect_child_and_parents(json *json);

static bool json_ast_node_member_print(FILE *file, json_prop *member);

static bool json_ast_node_object_print(FILE *file, json_object *object);

static bool json_ast_node_array_print(FILE *file, json_array *array);

static void json_ast_node_string_print(FILE *file, json_string *string);

static bool json_ast_node_number_print(FILE *file, json_number *number);

static bool json_ast_node_value_print(FILE *file, json_node_value *value);

static bool json_ast_node_element_print(FILE *file, json_element *element);

void init_parseStats(parseStats* stats);
static void insert_statsElement(parseStats* stats, char* key, size_t count);
static statsElement* get_statsElement(parseStats* stats, char* key);
static size_t stats_get_prediction(parseStats* stats, char* key);
static void update_statsElement(parseStats* stats, char* key, statsElement* elem, size_t count);
static void update_or_insert_statsElement(parseStats* stats, char* key, size_t count);
void build_parseStats(parseStats* stats, const char* input, size_t input_size);
static void drop_parseStats(parseStats* stats);

#define NEXT_TOKEN(x) { *x = *x + 1; }
#define PREV_TOKEN(x) { *x = *x - 1; }

static void task_routine_lbl(void *args)
{
    parser_task_args* task_args = (parser_task_args*) args;
    json_parse_split(task_args->start, task_args->size, NULL, "");
}

static void task_routine_lbl_combined_tok_int(void *args)
{
    parser_task_args* task_args = (parser_task_args*) args;
    json_parse_split_exp(task_args->start, task_args->size, NULL, "", NULL);
}

static void task_routine_stats(void *args)
{
    parser_task_args* task_args = (parser_task_args*) args;

    build_parseStats(task_args->stats, task_args->start, task_args->size);
}

static void task_routine_lbl_stats(void *args)
{
    parser_task_args* task_args = (parser_task_args*) args;
    json_parse_split_exp(task_args->start, task_args->size, NULL, "", task_args->stats);
}

bool
json_parse_split_parallel(const char *input, size_t size_input, size_t num_threads, size_t num_parts, bool parse_combined_tok_int, int statmode)
{
    parseStats stats;

    //statistic only useable, if tokenizer and interpreter are combined
    if(!parse_combined_tok_int)
    {
        statmode = 0;
    }

    //if stat
    if(statmode != 0)
    {
        init_parseStats(&stats);
    }

    if(statmode == 1)
    {
        build_parseStats(&stats, input, size_input);
        //madvise((void *) input, size_input, MADV_WILLNEED);
    }

    if(num_threads > num_parts)
    {
        num_threads = num_parts;
    }

    size_t size_part = 0;
    size_t current_char_pos = 0;
    size_t current_size = 0;
    size_t i = 0;
    const char* start_of_part = input;

    thread_pool *pool = thread_pool_create(num_threads, 0);

    //create Array of num_parts tasks
    //task_handle hndl[num_parts];
    thread_task tasks[num_parts];
    parser_task_args task_args[num_parts];
    task_handle task_hndl[num_parts];

    //divide size by num_parts
    size_part = size_input / num_parts;
    size_part++;

    if(statmode == 2)
    {
        parser_task_args stats_args;
        stats_args.stats = &stats;
        stats_args.start = input;
        stats_args.size = size_input;

        thread_task stats_task;
        stats_task.args = (void *) &stats_args;
        stats_task.routine = task_routine_stats;

        task_handle stats_hndl;
        thread_pool_enqueue_task(&stats_task, pool, &stats_hndl);
    }

    while (i < num_parts)
    {
        //if out of bounds
        if((current_size + size_part) > size_input)
        {
            current_char_pos = size_input - current_size;
        }
        //else skip size_part chars
        else
        {
            current_char_pos = size_part;

            //search for next EOL or End of String
            for (; start_of_part[current_char_pos] != '\n' && LIKELY(start_of_part[current_char_pos] != '\0'); current_char_pos++) {}
        }

        //start_of_part - pointer to start of part
        //end_of_part - position of end of part
        task_args[i].start = start_of_part;
        task_args[i].size = current_char_pos;
        task_args[i].count = i+1;

        if(statmode != 0)
        {
            task_args[i].stats = &stats;
        }

        current_size += current_char_pos;

        tasks[i].args = (void *) &task_args[i];

        if(parse_combined_tok_int)
        {
            if(statmode != 0)
            {
                tasks[i].routine = task_routine_lbl_stats;
            }
            else
            {
                tasks[i].routine = task_routine_lbl_combined_tok_int;
            }
        }
        else
        {
            tasks[i].routine = task_routine_lbl;
        }

        if(statmode == 2)
        {
            thread_pool_enqueue_task(&tasks[i], pool, &task_hndl[i]);
        }

        start_of_part = start_of_part + (current_char_pos + 1);

        i++;
    }
    if(statmode == 2)
    {
        thread_pool_wait_for_all(pool);
    }
    else
    {
        thread_pool_enqueue_tasks_wait(tasks, pool, num_parts);
    }

    thread_pool_free(pool);

    //if stats were used
    if(statmode != 0)
    {
        drop_parseStats(&stats);
    }

    return true;
}

bool
json_parse_split(const char *input, size_t size_input, const char* destdir, const char* filename)
{
    size_t i = 0;
    size_t lastPart = 0;
    int l = 0;
    const char* currentPart;

    bool end_parse = false;

    while (!end_parse)
    {
        if((input[i] == '\n') || (input[i] == '\0'))
        {
            if((input[i] == '\0') || (i == size_input))
            {
                end_parse = true;
            }

            //set pointer to the beginning of current part
            currentPart = input + lastPart;

            l++;

            if(destdir == NULL)
            {
                struct json data;
                json_err err;
                json_parser parser;
                if(json_parse_limited(&data, &err, &parser, currentPart, i-lastPart)) {
                    json_drop(&data);
                }
            }
            else
            {
                struct json data;
                json_err err;
                json_parser parser;

                if(json_parse_limited(&data, &err, &parser, currentPart, i-lastPart)) {
                    size_t filepathsize = strlen(destdir) + strlen(filename) + 28;

                    char filepath[filepathsize];
                    snprintf(filepath, filepathsize, "%s%s%u%s", destdir, filename, l, ".carbon");

                    FILE *file;
                    file = fopen(filepath, "w");
                    if(file == NULL)
                    {
                        file = NULL;
                    }
                    json_print(file, &data);
                    fclose(file);
                    json_drop(&data);
                }
            }
            lastPart = i+1;
        }
        i++;
    }
    
    return true;
}

static bool json_parse_check_input(json_err *error_desc, const char *input, size_t charcount)
{
    size_t pos = 0;
    size_t endpos = 0;

    if (charcount == 0)
    {
        charcount = strlen(input);
    }

    if(charcount == 0)
    {
        set_error(error_desc, NULL, "input str_buf is empty");
        return false;
    }

    endpos = (charcount / 2) +1;

    while (pos < endpos)
    {
        if (((!isspace(input[pos])) && (input[pos] != '\n'))
            || (!isspace(input[charcount - pos]) && (input[charcount - pos] != '\n') && (input[charcount - pos] != '\0')))
        {
            return true;
        }

        pos++;
    }

    set_error(error_desc, NULL, "input str_buf is empty");
    return false;
}

static bool json_parse_input(json *json, json_err *error_desc, json_parser *parser)
{
    vec ofType(json_token_e) brackets;
    vec ofType(json_token) token_stream;

    struct json retval;
    ZERO_MEMORY(&retval, sizeof(json))
    retval.element = MALLOC(sizeof(json_element));
    const json_token *token;
    int status;

    vec_create(&brackets, sizeof(json_token_e), 15);
    vec_create(&token_stream, sizeof(json_token), 200);

    struct token_memory token_mem = {.init = true, .type = JSON_UNKNOWN};

    while ((token = json_tokenizer_next(&parser->tokenizer))) {
        if (LIKELY(
                (status = process_token(error_desc, token, &brackets, &token_mem)) == true)) {
            json_token *newToken = VEC_NEW_AND_GET(&token_stream, json_token);
            json_token_dup(newToken, token);
        } else {
            goto cleanup;
        }
    }
    if (!vec_is_empty(&brackets)) {
        json_token_e type = *VEC_PEEK(&brackets, json_token_e);
        char buffer[1024];
        sprintf(&buffer[0],
                "Unexpected end of file: missing '%s' to match unclosed '%s' (if any)",
                type == OBJECT_OPEN ? "}" : "]",
                type == OBJECT_OPEN ? "{" : "[");
        status = set_error(error_desc, token, &buffer[0]);
        goto cleanup;
    }

    if (!parse_token_stream(&retval, &token_stream)) {
        status = false;
        goto cleanup;
    }

    OPTIONAL_SET_OR_ELSE(json, retval, json_drop(json));
    status = true;

    cleanup:
    vec_drop(&brackets);
    vec_drop(&token_stream);
    return status;
}

bool
json_parse(json *json, json_err *error_desc, json_parser *parser, const char *input)
{
        if(!json_parse_check_input(error_desc, input, 0))
        {
            return false;
        }

        json_tokenizer_init(&parser->tokenizer, input);

        return json_parse_input(json, error_desc, parser);
}

bool
json_parse_limited(json *json, json_err *error_desc, json_parser *parser, const char *input, size_t charcount)
{
    if(!json_parse_check_input(error_desc, input, charcount))
    {
        return false;
    }

    json_tokenizer_init_limited(&parser->tokenizer, input, charcount);

    return json_parse_input(json, error_desc, parser);
}

bool test_condition_value(json_node_value *value)
{
        switch (value->value_type) {
                case JSON_VALUE_OBJECT:
                        for (size_t i = 0; i < value->value.object->value->members.num_elems; i++) {
                                json_prop *member = VEC_GET(&value->value.object->value->members, i,
                                                                       json_prop);
                                if (!test_condition_value(&member->value.value)) {
                                        return false;
                                }
                        }
                        break;
                case JSON_VALUE_ARRAY: {
                        json_elements *elements = &value->value.array->elements;
                        json_value_type_e value_type = JSON_VALUE_NULL;

                        for (size_t i = 0; i < elements->elements.num_elems; i++) {
                                json_element *element = VEC_GET(&elements->elements, i,
                                                                           json_element);
                                value_type =
                                        ((i == 0 || value_type == JSON_VALUE_NULL) ? element->value.value_type
                                                                                   : value_type);

                                /** Test "All elements in array of same type" condition */
                                if ((element->value.value_type != JSON_VALUE_NULL) && (value_type == JSON_VALUE_TRUE
                                                                                       && (element->value.value_type !=
                                                                                           JSON_VALUE_TRUE
                                                                                           ||
                                                                                           element->value.value_type !=
                                                                                           JSON_VALUE_FALSE))
                                    && (value_type == JSON_VALUE_FALSE && (element->value.value_type != JSON_VALUE_TRUE
                                                                           || element->value.value_type !=
                                                                              JSON_VALUE_FALSE))
                                    && ((value_type != JSON_VALUE_TRUE && value_type != JSON_VALUE_FALSE)
                                        && value_type != element->value.value_type)) {
                                        char message[] = "JSON file constraint broken: arrays of mixed types detected";
                                        char *result = MALLOC(strlen(message) + 1);
                                        strcpy(result, &message[0]);
                                        ERROR(ERR_ARRAYOFMIXEDTYPES, result);
                                        free(result);
                                        return false;
                                }

                                switch (element->value.value_type) {
                                        case JSON_VALUE_OBJECT: {
                                                json_object *object = element->value.value.object;
                                                for (size_t i = 0; i < object->value->members.num_elems; i++) {
                                                        json_prop
                                                                *member = VEC_GET(&object->value->members, i,
                                                                                  json_prop);
                                                        if (!test_condition_value(&member->value.value)) {
                                                                return false;
                                                        }
                                                }
                                        }
                                                break;
                                        case JSON_VALUE_ARRAY: {/** Test "No Array of Arrays" condition */
                                                char message[] = "JSON file constraint broken: arrays of arrays detected";
                                                char *result = MALLOC(strlen(message) + 1);
                                                strcpy(result, &message[0]);
                                                ERROR(ERR_ARRAYOFARRAYS, result);
                                                free(result);
                                                return false;
                                        }
                                        default:
                                                break;
                                }
                        }
                }
                        break;
                default:
                        break;
        }
        return true;
}

bool json_test(json *json)
{
        return (test_condition_value(&json->element->value));
}

static json_token get_token(vec ofType(json_token) *token_stream, size_t token_idx)
{
        return *(json_token *) vec_at(token_stream, token_idx);
}

static bool has_next_token(size_t token_idx, vec ofType(json_token) *token_stream)
{
        return token_idx < token_stream->num_elems;
}

bool parse_members(json_members *members,
                   vec ofType(json_token) *token_stream,
                   size_t *token_idx)
{
        vec_create(&members->members, sizeof(json_prop), 20);
        json_token delimiter_token;

        do {
                json_prop *member = VEC_NEW_AND_GET(&members->members, json_prop);
                json_token keyNameToken = get_token(token_stream, *token_idx);

                member->key.value = MALLOC(keyNameToken.length + 1);
                strncpy(member->key.value, keyNameToken.string, keyNameToken.length);
                member->key.value[keyNameToken.length] = '\0';

                /** assignment token */
                NEXT_TOKEN(token_idx);
                if (!has_next_token(*token_idx, token_stream)) {
                        return false;
                }
                json_token assignment_token = get_token(token_stream, *token_idx);
                if (assignment_token.type != ASSIGN) {
                        return false;
                }

                /** value assignment token */
                NEXT_TOKEN(token_idx);
                if (!has_next_token(*token_idx, token_stream)) {
                        return false;
                }

                json_token valueToken = get_token(token_stream, *token_idx);

                switch (valueToken.type) {
                        case OBJECT_OPEN:
                                member->value.value.value_type = JSON_VALUE_OBJECT;
                                member->value.value.value.object = MALLOC(sizeof(json_object));
                                if (!parse_object(member->value.value.value.object, token_stream, token_idx)) {
                                        return false;
                                }
                                break;
                        case ARRAY_OPEN:
                                member->value.value.value_type = JSON_VALUE_ARRAY;
                                member->value.value.value.array = MALLOC(sizeof(json_array));
                                if (!parse_array(member->value.value.value.array, token_stream, token_idx)) {
                                        return false;
                                }
                                break;
                        case LITERAL_STRING:
                                member->value.value.value_type = JSON_VALUE_STRING;
                                member->value.value.value.string = MALLOC(sizeof(json_string));
                                parse_string(member->value.value.value.string, token_stream, token_idx);
                                break;
                        case LITERAL_INT:
                        case LITERAL_FLOAT:
                                member->value.value.value_type = JSON_VALUE_NUMBER;
                                member->value.value.value.number = MALLOC(sizeof(json_number));
                                parse_number(member->value.value.value.number, token_stream, token_idx);
                                break;
                        case LITERAL_TRUE:
                                member->value.value.value_type = JSON_VALUE_TRUE;
                                NEXT_TOKEN(token_idx);
                                break;
                        case LITERAL_FALSE:
                                member->value.value.value_type = JSON_VALUE_FALSE;
                                NEXT_TOKEN(token_idx);
                                break;
                        case LITERAL_NULL:
                                member->value.value.value_type = JSON_VALUE_NULL;
                                NEXT_TOKEN(token_idx);
                                break;
                        default:
                                return ERROR(ERR_PARSETYPE, NULL);
                }

                delimiter_token = get_token(token_stream, *token_idx);
                NEXT_TOKEN(token_idx);
        } while (delimiter_token.type == COMMA);
        PREV_TOKEN(token_idx);
        return true;
}

static bool parse_object(json_object *object, vec ofType(json_token) *token_stream, size_t *token_idx)
{
        assert(get_token(token_stream, *token_idx).type == OBJECT_OPEN);
        NEXT_TOKEN(token_idx);  /** Skip '{' */
        object->value = MALLOC(sizeof(json_members));

        /** test whether this is an empty object */
        json_token token = get_token(token_stream, *token_idx);

        if (token.type != OBJECT_CLOSE) {
                if (!parse_members(object->value, token_stream, token_idx)) {
                        return false;
                }
        } else {
                vec_create(&object->value->members, sizeof(json_prop), 20);
        }

        NEXT_TOKEN(token_idx);  /** Skip '}' */
        return true;
}

static bool parse_array(json_array *array, vec ofType(json_token) *token_stream, size_t *token_idx)
{
        json_token token = get_token(token_stream, *token_idx);
        UNUSED(token);
        assert(token.type == ARRAY_OPEN);
        NEXT_TOKEN(token_idx); /** Skip '[' */

        vec_create(&array->elements.elements, sizeof(json_element), 250);
        if (!parse_elements(&array->elements, token_stream, token_idx)) {
                return false;
        }

        NEXT_TOKEN(token_idx); /** Skip ']' */
        return true;
}

static void parse_string(json_string *string, vec ofType(json_token) *token_stream,
                         size_t *token_idx)
{
        json_token token = get_token(token_stream, *token_idx);
        assert(token.type == LITERAL_STRING);

        string->value = MALLOC(token.length + 1);
        if (LIKELY(token.length > 0)) {
                strncpy(string->value, token.string, token.length);
        }
        string->value[token.length] = '\0';
        NEXT_TOKEN(token_idx);
}

static void parse_number(json_number *number, vec ofType(json_token) *token_stream,
                         size_t *token_idx)
{
        json_token token = get_token(token_stream, *token_idx);
        assert(token.type == LITERAL_FLOAT || token.type == LITERAL_INT);

        char *value = MALLOC(token.length + 1);
        strncpy(value, token.string, token.length);
        value[token.length] = '\0';

        if (token.type == LITERAL_INT) {
                i64 assumeSigned = convert_atoi64(value);
                if (value[0] == '-') {
                        number->value_type = JSON_NUMBER_SIGNED;
                        number->value.signed_integer = assumeSigned;
                } else {
                        u64 assumeUnsigned = convert_atoiu64(value);
                        if (assumeUnsigned >= (u64) assumeSigned) {
                                number->value_type = JSON_NUMBER_UNSIGNED;
                                number->value.unsigned_integer = assumeUnsigned;
                        } else {
                                number->value_type = JSON_NUMBER_SIGNED;
                                number->value.signed_integer = assumeSigned;
                        }
                }
        } else {
                number->value_type = JSON_NUMBER_FLOAT;
                setlocale(LC_ALL | ~LC_NUMERIC, "");
                number->value.float_number = strtof(value, NULL);
        }

        free(value);
        NEXT_TOKEN(token_idx);
}

static bool parse_element(json_element *element, vec ofType(json_token) *token_stream, size_t *token_idx)
{
        if (!has_next_token(*token_idx, token_stream)) {
                return false;
        }
        json_token token = get_token(token_stream, *token_idx);

        if (token.type == OBJECT_OPEN) { /** Parse object */
                element->value.value_type = JSON_VALUE_OBJECT;
                element->value.value.object = MALLOC(sizeof(json_object));
                if (!parse_object(element->value.value.object, token_stream, token_idx)) {
                        return false;
                }
        } else if (token.type == ARRAY_OPEN) { /** Parse array */
                element->value.value_type = JSON_VALUE_ARRAY;
                element->value.value.array = MALLOC(sizeof(json_array));
                if (!parse_array(element->value.value.array, token_stream, token_idx)) {
                        return false;
                }
        } else if (token.type == LITERAL_STRING) { /** Parse string */
                element->value.value_type = JSON_VALUE_STRING;
                element->value.value.string = MALLOC(sizeof(json_string));
                parse_string(element->value.value.string, token_stream, token_idx);
        } else if (token.type == LITERAL_FLOAT || token.type == LITERAL_INT) { /** Parse number */
                element->value.value_type = JSON_VALUE_NUMBER;
                element->value.value.number = MALLOC(sizeof(json_number));
                parse_number(element->value.value.number, token_stream, token_idx);
        } else if (token.type == LITERAL_TRUE) {
                element->value.value_type = JSON_VALUE_TRUE;
                NEXT_TOKEN(token_idx);
        } else if (token.type == LITERAL_FALSE) {
                element->value.value_type = JSON_VALUE_FALSE;
                NEXT_TOKEN(token_idx);
        } else if (token.type == LITERAL_NULL) {
                element->value.value_type = JSON_VALUE_NULL;
                NEXT_TOKEN(token_idx);
        } else {
                element->value.value_type = JSON_VALUE_NULL;
        }
        return true;
}

static bool parse_elements(json_elements *elements, vec ofType(json_token) *token_stream, size_t *token_idx)
{
        json_token delimiter;
        do {
                json_token current = get_token(token_stream, *token_idx);
                if (current.type != ARRAY_CLOSE && current.type != OBJECT_CLOSE) {
                        if (!parse_element(VEC_NEW_AND_GET(&elements->elements, json_element),
                                           token_stream,
                                           token_idx)) {
                                return false;
                        }
                }
                delimiter = get_token(token_stream, *token_idx);
                NEXT_TOKEN(token_idx);
        } while (delimiter.type == COMMA);
        PREV_TOKEN(token_idx);
        return true;
}

static bool parse_token_stream(json *json, vec ofType(json_token) *token_stream)
{
        size_t token_idx = 0;
        if (!parse_element(json->element, token_stream, &token_idx)) {
                return false;
        }
        connect_child_and_parents(json);
        return true;
}

static void connect_child_and_parents_member(json_prop *member)
{
        connect_child_and_parents_element(&member->value);
}

static void connect_child_and_parents_object(json_object *object)
{
        object->value->parent = object;
        for (size_t i = 0; i < object->value->members.num_elems; i++) {
                json_prop *member = VEC_GET(&object->value->members, i, json_prop);
                member->parent = object->value;

                member->key.parent = member;

                member->value.parent_type = JSON_PARENT_MEMBER;
                member->value.parent.member = member;

                connect_child_and_parents_member(member);
        }
}

static void connect_child_and_parents_array(json_array *array)
{
        array->elements.parent = array;
        for (size_t i = 0; i < array->elements.elements.num_elems; i++) {
                json_element *element = VEC_GET(&array->elements.elements, i, json_element);
                element->parent_type = JSON_PARENT_ELEMENTS;
                element->parent.elements = &array->elements;
                connect_child_and_parents_element(element);
        }
}

static void connect_child_and_parents_value(json_node_value *value)
{
        switch (value->value_type) {
                case JSON_VALUE_OBJECT:
                        connect_child_and_parents_object(value->value.object);
                        break;
                case JSON_VALUE_ARRAY:
                        connect_child_and_parents_array(value->value.array);
                        break;
                default:
                        break;
        }
}

static void connect_child_and_parents_element(json_element *element)
{
        element->value.parent = element;
        connect_child_and_parents_value(&element->value);
}

static void connect_child_and_parents(json *json)
{
        json->element->parent_type = JSON_PARENT_OBJECT;
        json->element->parent.json = json;
        connect_child_and_parents_element(json->element);
}

static bool isValue(json_token_e token)
{
        return (token == LITERAL_STRING || token == LITERAL_FLOAT || token == LITERAL_INT || token == LITERAL_TRUE
                || token == LITERAL_FALSE || token == LITERAL_NULL);
}

static int process_token(json_err *error_desc, const json_token *token,
                         vec ofType(json_token_e) *brackets, struct token_memory *token_mem)
{
        switch (token->type) {
                case OBJECT_OPEN:
                case ARRAY_OPEN:
                        vec_push(brackets, &token->type, 1);
                        break;
                case OBJECT_CLOSE:
                case ARRAY_CLOSE: {
                        if (!vec_is_empty(brackets)) {
                                json_token_e bracket = *VEC_PEEK(brackets, json_token_e);
                                if ((token->type == ARRAY_CLOSE && bracket == ARRAY_OPEN)
                                    || (token->type == OBJECT_CLOSE && bracket == OBJECT_OPEN)) {
                                        vec_pop(brackets);
                                } else {
                                        goto pushEntry;
                                }
                        } else {
                                pushEntry:
                                vec_push(brackets, &token->type, 1);
                        }
                }
                        break;
                default:
                        break;
        }

        switch (token_mem->type) {
                case OBJECT_OPEN:
                        switch (token->type) {
                                case LITERAL_STRING:
                                case OBJECT_CLOSE:
                                        break;
                                default:
                                        return set_error(error_desc, token, "Expected key name or '}'");
                        }
                        break;
                case LITERAL_STRING:
                        switch (token->type) {
                                case ASSIGN:
                                case COMMA:
                                case ARRAY_CLOSE:
                                case OBJECT_CLOSE:
                                        break;
                                default:
                                        return set_error(error_desc, token,
                                                         "Expected key name (missing ':'), enumeration (','), "
                                                         "end of enumeration (']'), or end of object ('}')");
                        }
                        break;
                case OBJECT_CLOSE:
                case LITERAL_INT:
                case LITERAL_FLOAT:
                case LITERAL_TRUE:
                case LITERAL_FALSE:
                case LITERAL_NULL:
                        switch (token->type) {
                                case COMMA:
                                case ARRAY_CLOSE:
                                case OBJECT_CLOSE:
                                        break;
                                default:
                                        return set_error(error_desc, token,
                                                         "Expected enumeration (','), end of enumeration (']'), "
                                                         "or end of object ('})");
                        }
                        break;
                case ASSIGN:
                case COMMA:
                        switch (token->type) {
                                case LITERAL_STRING:
                                case LITERAL_FLOAT:
                                case LITERAL_INT:
                                case OBJECT_OPEN:
                                case ARRAY_OPEN:
                                case LITERAL_TRUE:
                                case LITERAL_FALSE:
                                case LITERAL_NULL:
                                        break;
                                default:
                                        return set_error(error_desc,
                                                         token,
                                                         "Expected key name, or value (str_buf, number, object, enumeration, true, "
                                                         "false, or null).");
                        }
                        break;
                case ARRAY_OPEN:
                        switch (token->type) {
                                case ARRAY_CLOSE:
                                case LITERAL_STRING:
                                case LITERAL_FLOAT:
                                case LITERAL_INT:
                                case OBJECT_OPEN:
                                case ARRAY_OPEN:
                                case LITERAL_TRUE:
                                case LITERAL_FALSE:
                                case LITERAL_NULL:
                                        break;
                                default:
                                        return set_error(error_desc, token,
                                                         "End of enumeration (']'), enumeration (','), or "
                                                         "end of enumeration (']')");
                        }
                        break;
                case ARRAY_CLOSE:
                        switch (token->type) {
                                case COMMA:
                                case ARRAY_CLOSE:
                                case OBJECT_CLOSE:
                                        break;
                                default:
                                        return set_error(error_desc, token,
                                                         "End of enumeration (']'), enumeration (','), or "
                                                         "end of object ('}')");
                        }
                        break;
                case JSON_UNKNOWN:
                        if (token_mem->init) {
                                if (token->type != OBJECT_OPEN && token->type != ARRAY_OPEN && !isValue(token->type)) {
                                        return set_error(error_desc, token,
                                                         "Expected JSON document: missing '{' or '['");
                                }
                                token_mem->init = false;
                        } else {
                                return set_error(error_desc, token, "Unexpected token");
                        }
                        break;
                default:
                        return ERROR(ERR_NOJSONTOKEN, NULL);
        }

        token_mem->type = token->type;
        return true;
}

static int set_error(json_err *error_desc, const json_token *token, const char *msg)
{
        if (error_desc) {
                error_desc->token = token;
                error_desc->token_type_str = token ? JSON_TOKEN_STRINGS[token->type].string : "(no token)";
                error_desc->msg = msg;
        }
        return false;
}

static bool json_ast_node_member_print(FILE *file, json_prop *member)
{
        fprintf(file, "\"%s\": ", member->key.value);
        return json_ast_node_value_print(file, &member->value.value);
}

static bool json_ast_node_object_print(FILE *file, json_object *object)
{
        fprintf(file, "{");
        for (size_t i = 0; i < object->value->members.num_elems; i++) {
                json_prop *member = VEC_GET(&object->value->members, i, json_prop);
                if (!json_ast_node_member_print(file, member)) {
                        return false;
                }
                fprintf(file, "%s", i + 1 < object->value->members.num_elems ? ", " : "");
        }
        fprintf(file, "}");
        return true;
}

static bool json_ast_node_array_print(FILE *file, json_array *array)
{
        fprintf(file, "[");
        for (size_t i = 0; i < array->elements.elements.num_elems; i++) {
                json_element *element = VEC_GET(&array->elements.elements, i, json_element);
                if (!json_ast_node_element_print(file, element)) {
                        return false;
                }
                fprintf(file, "%s", i + 1 < array->elements.elements.num_elems ? ", " : "");
        }
        fprintf(file, "]");
        return true;
}

static void json_ast_node_string_print(FILE *file, json_string *string)
{
        fprintf(file, "\"%s\"", string->value);
}

static bool json_ast_node_number_print(FILE *file, json_number *number)
{
        switch (number->value_type) {
                case JSON_NUMBER_FLOAT:
                        fprintf(file, "%f", number->value.float_number);
                        break;
                case JSON_NUMBER_UNSIGNED:
                        fprintf(file, "%" PRIu64, number->value.unsigned_integer);
                        break;
                case JSON_NUMBER_SIGNED:
                        fprintf(file, "%" PRIi64, number->value.signed_integer);
                        break;
                default: ERROR(ERR_NOJSONNUMBERT, NULL);
                        return false;
        }
        return true;
}

static bool json_ast_node_value_print(FILE *file, json_node_value *value)
{
        switch (value->value_type) {
                case JSON_VALUE_OBJECT:
                        if (!json_ast_node_object_print(file, value->value.object)) {
                                return false;
                        }
                        break;
                case JSON_VALUE_ARRAY:
                        if (!json_ast_node_array_print(file, value->value.array)) {
                                return false;
                        }
                        break;
                case JSON_VALUE_STRING:
                        json_ast_node_string_print(file, value->value.string);
                        break;
                case JSON_VALUE_NUMBER:
                        if (!json_ast_node_number_print(file, value->value.number)) {
                                return false;
                        }
                        break;
                case JSON_VALUE_TRUE:
                        fprintf(file, "true");
                        break;
                case JSON_VALUE_FALSE:
                        fprintf(file, "false");
                        break;
                case JSON_VALUE_NULL:
                        fprintf(file, "null");
                        break;
                default: ERROR(ERR_NOTYPE, NULL);
                        return false;
        }
        return true;
}

static bool json_ast_node_element_print(FILE *file, json_element *element)
{
        return json_ast_node_value_print(file, &element->value);
}

static bool json_ast_node_value_drop(json_node_value *value);

static bool json_ast_node_element_drop(json_element *element)
{
        return json_ast_node_value_drop(&element->value);
}

static bool json_ast_node_member_drop(json_prop *member)
{
        free(member->key.value);
        return json_ast_node_element_drop(&member->value);
}

static bool json_ast_node_members_drop(json_members *members)
{
        for (size_t i = 0; i < members->members.num_elems; i++) {
                json_prop *member = VEC_GET(&members->members, i, json_prop);
                if (!json_ast_node_member_drop(member)) {
                        return false;
                }
        }
        vec_drop(&members->members);
        return true;
}

static bool json_ast_node_elements_drop(json_elements *elements)
{
        for (size_t i = 0; i < elements->elements.num_elems; i++) {
                json_element *element = VEC_GET(&elements->elements, i, json_element);
                if (!json_ast_node_element_drop(element)) {
                        return false;
                }
        }
        vec_drop(&elements->elements);
        return true;
}

static bool json_ast_node_object_drop(json_object *object)
{
        if (!json_ast_node_members_drop(object->value)) {
                return false;
        } else {
                free(object->value);
                return true;
        }
}

static bool json_ast_node_array_drop(json_array *array)
{
        return json_ast_node_elements_drop(&array->elements);
}

static void json_ast_node_string_drop(json_string *string)
{
        free(string->value);
}

static void json_ast_node_number_drop(json_number *number)
{
        UNUSED(number);
}

static bool json_ast_node_value_drop(json_node_value *value)
{
        switch (value->value_type) {
                case JSON_VALUE_OBJECT:
                        if (!json_ast_node_object_drop(value->value.object)) {
                                return false;
                        } else {
                                free(value->value.object);
                        }
                        break;
                case JSON_VALUE_ARRAY:
                        if (!json_ast_node_array_drop(value->value.array)) {
                                return false;
                        } else {
                                free(value->value.array);
                        }
                        break;
                case JSON_VALUE_STRING:
                        json_ast_node_string_drop(value->value.string);
                        free(value->value.string);
                        break;
                case JSON_VALUE_NUMBER:
                        json_ast_node_number_drop(value->value.number);
                        free(value->value.number);
                        break;
                case JSON_VALUE_TRUE:
                case JSON_VALUE_FALSE:
                case JSON_VALUE_NULL:
                        break;
                default:
                        return ERROR(ERR_NOTYPE, NULL);

        }
        return true;
}

bool json_drop(json *json)
{
        json_element *element = json->element;
        if (!json_ast_node_value_drop(&element->value)) {
                return false;
        } else {
                free(json->element);
                return true;
        }
}

bool json_print(FILE *file, json *json)
{
        return json_ast_node_element_print(file, json->element);
}

bool json_list_is_empty(const json_elements *elements)
{
        return elements->elements.num_elems == 0;
}

bool json_list_length(u32 *len, const json_elements *elements)
{
        *len = elements->elements.num_elems;
        return true;
}

json_list_type_e json_fitting_type(json_list_type_e current, json_list_type_e to_add)
{
        if (current == JSON_LIST_VARIABLE_OR_NESTED || to_add == JSON_LIST_VARIABLE_OR_NESTED) {
                return JSON_LIST_VARIABLE_OR_NESTED;
        }
        if (current == JSON_LIST_EMPTY || current == JSON_LIST_FIXED_BOOLEAN ||
            to_add == JSON_LIST_EMPTY || to_add == JSON_LIST_FIXED_BOOLEAN) {
                if (current == to_add) {
                        return current;
                } else {
                        if ((to_add == JSON_LIST_FIXED_BOOLEAN && current == JSON_LIST_FIXED_NULL) ||
                            (to_add == JSON_LIST_FIXED_NULL && current == JSON_LIST_FIXED_BOOLEAN)) {
                                return JSON_LIST_FIXED_BOOLEAN;
                        } else {
                                if (to_add == JSON_LIST_EMPTY) {
                                        return current;
                                } else if (current == JSON_LIST_EMPTY) {
                                        return to_add;
                                } else {

                                        return JSON_LIST_VARIABLE_OR_NESTED;
                                }
                        }
                }
        } else {
                switch (current) {
                        case JSON_LIST_FIXED_NULL:
                                switch (to_add) {
                                        case JSON_LIST_FIXED_NULL:
                                        case JSON_LIST_FIXED_FLOAT:
                                        case JSON_LIST_FIXED_BOOLEAN:
                                        case JSON_LIST_FIXED_U8:
                                        case JSON_LIST_FIXED_U16:
                                        case JSON_LIST_FIXED_U32:
                                        case JSON_LIST_FIXED_U64:
                                        case JSON_LIST_FIXED_I8:
                                        case JSON_LIST_FIXED_I16:
                                        case JSON_LIST_FIXED_I32:
                                        case JSON_LIST_FIXED_I64:
                                                return to_add;
                                        default:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                }
                        case JSON_LIST_FIXED_FLOAT:
                                switch (to_add) {
                                        case JSON_LIST_FIXED_NULL:
                                        case JSON_LIST_FIXED_FLOAT:
                                        case JSON_LIST_FIXED_U8:
                                        case JSON_LIST_FIXED_U16:
                                        case JSON_LIST_FIXED_U32:
                                        case JSON_LIST_FIXED_U64:
                                        case JSON_LIST_FIXED_I8:
                                        case JSON_LIST_FIXED_I16:
                                        case JSON_LIST_FIXED_I32:
                                        case JSON_LIST_FIXED_I64:
                                                return JSON_LIST_FIXED_FLOAT;
                                        default:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                }
                        case JSON_LIST_FIXED_U8:
                                switch (to_add) {
                                        case JSON_LIST_FIXED_NULL:
                                                return JSON_LIST_FIXED_U8;
                                        case JSON_LIST_FIXED_U8:
                                                return JSON_LIST_FIXED_U8;
                                        case JSON_LIST_FIXED_U16:
                                                return JSON_LIST_FIXED_U16;
                                        case JSON_LIST_FIXED_U32:
                                                return JSON_LIST_FIXED_U32;
                                        case JSON_LIST_FIXED_U64:
                                                return JSON_LIST_FIXED_U64;
                                        case JSON_LIST_FIXED_I8:
                                                return JSON_LIST_FIXED_I16;
                                        case JSON_LIST_FIXED_I16:
                                                return JSON_LIST_FIXED_I32;
                                        case JSON_LIST_FIXED_I32:
                                                return JSON_LIST_FIXED_I64;
                                        case JSON_LIST_FIXED_I64:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                        default:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                }
                        case JSON_LIST_FIXED_U16:
                                switch (to_add) {
                                        case JSON_LIST_FIXED_NULL:
                                                return JSON_LIST_FIXED_U16;
                                        case JSON_LIST_FIXED_U8:
                                        case JSON_LIST_FIXED_U16:
                                                return JSON_LIST_FIXED_U16;
                                        case JSON_LIST_FIXED_U32:
                                                return JSON_LIST_FIXED_U32;
                                        case JSON_LIST_FIXED_U64:
                                                return JSON_LIST_FIXED_U64;
                                        case JSON_LIST_FIXED_I8:
                                                return JSON_LIST_FIXED_I32;
                                        case JSON_LIST_FIXED_I16:
                                                return JSON_LIST_FIXED_I32;
                                        case JSON_LIST_FIXED_I32:
                                                return JSON_LIST_FIXED_I64;
                                        case JSON_LIST_FIXED_I64:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                        default:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                }
                        case JSON_LIST_FIXED_U32:
                                switch (to_add) {
                                        case JSON_LIST_FIXED_NULL:
                                                return JSON_LIST_FIXED_U32;
                                        case JSON_LIST_FIXED_U8:
                                        case JSON_LIST_FIXED_U16:
                                        case JSON_LIST_FIXED_U32:
                                                return JSON_LIST_FIXED_U32;
                                        case JSON_LIST_FIXED_U64:
                                                return JSON_LIST_FIXED_U64;
                                        case JSON_LIST_FIXED_I8:
                                        case JSON_LIST_FIXED_I16:
                                        case JSON_LIST_FIXED_I32:
                                                return JSON_LIST_FIXED_I64;
                                        case JSON_LIST_FIXED_I64:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                        default:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                }
                        case JSON_LIST_FIXED_U64:
                                switch (to_add) {
                                        case JSON_LIST_FIXED_NULL:
                                                return JSON_LIST_FIXED_U64;
                                        case JSON_LIST_FIXED_U8:
                                        case JSON_LIST_FIXED_U16:
                                        case JSON_LIST_FIXED_U32:
                                        case JSON_LIST_FIXED_U64:
                                                return JSON_LIST_FIXED_U64;
                                        case JSON_LIST_FIXED_I8:
                                        case JSON_LIST_FIXED_I16:
                                        case JSON_LIST_FIXED_I32:
                                        case JSON_LIST_FIXED_I64:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                        default:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                }
                        case JSON_LIST_FIXED_I8:
                                switch (to_add) {
                                        case JSON_LIST_FIXED_NULL:
                                                return JSON_LIST_FIXED_I8;
                                        case JSON_LIST_FIXED_U8:
                                                return JSON_LIST_FIXED_I16;
                                        case JSON_LIST_FIXED_U16:
                                                return JSON_LIST_FIXED_I32;
                                        case JSON_LIST_FIXED_U32:
                                                return JSON_LIST_FIXED_I64;
                                        case JSON_LIST_FIXED_U64:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                        case JSON_LIST_FIXED_I8:
                                                return JSON_LIST_FIXED_I8;
                                        case JSON_LIST_FIXED_I16:
                                                return JSON_LIST_FIXED_I16;
                                        case JSON_LIST_FIXED_I32:
                                                return JSON_LIST_FIXED_I32;
                                        case JSON_LIST_FIXED_I64:
                                                return JSON_LIST_FIXED_I64;
                                        default:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                }
                        case JSON_LIST_FIXED_I16:
                                switch (to_add) {
                                        case JSON_LIST_FIXED_NULL:
                                                return JSON_LIST_FIXED_I16;
                                        case JSON_LIST_FIXED_U8:
                                        case JSON_LIST_FIXED_U16:
                                                return JSON_LIST_FIXED_I32;
                                        case JSON_LIST_FIXED_U32:
                                                return JSON_LIST_FIXED_I64;
                                        case JSON_LIST_FIXED_U64:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                        case JSON_LIST_FIXED_I8:
                                        case JSON_LIST_FIXED_I16:
                                                return JSON_LIST_FIXED_I16;
                                        case JSON_LIST_FIXED_I32:
                                                return JSON_LIST_FIXED_I32;
                                        case JSON_LIST_FIXED_I64:
                                                return JSON_LIST_FIXED_I64;
                                        default:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                }
                        case JSON_LIST_FIXED_I32:
                                switch (to_add) {
                                        case JSON_LIST_FIXED_NULL:
                                                return JSON_LIST_FIXED_I32;
                                        case JSON_LIST_FIXED_U8:
                                        case JSON_LIST_FIXED_U16:
                                        case JSON_LIST_FIXED_U32:
                                                return JSON_LIST_FIXED_I64;
                                        case JSON_LIST_FIXED_U64:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                        case JSON_LIST_FIXED_I8:
                                        case JSON_LIST_FIXED_I16:
                                        case JSON_LIST_FIXED_I32:
                                                return JSON_LIST_FIXED_I32;
                                        case JSON_LIST_FIXED_I64:
                                                return JSON_LIST_FIXED_I64;
                                        default:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                }
                        case JSON_LIST_FIXED_I64:
                                switch (to_add) {
                                        case JSON_LIST_FIXED_NULL:
                                                return JSON_LIST_FIXED_I64;
                                        case JSON_LIST_FIXED_U8:
                                        case JSON_LIST_FIXED_U16:
                                        case JSON_LIST_FIXED_U32:
                                        case JSON_LIST_FIXED_U64:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                        case JSON_LIST_FIXED_I8:
                                        case JSON_LIST_FIXED_I16:
                                        case JSON_LIST_FIXED_I32:
                                        case JSON_LIST_FIXED_I64:
                                                return JSON_LIST_FIXED_I64;
                                        default:
                                                return JSON_LIST_VARIABLE_OR_NESTED;
                                }
                        default:
                                return JSON_LIST_VARIABLE_OR_NESTED;
                }
        }
}

static json_list_type_e number_type_to_list_type(number_min_type_e type)
{
        switch (type) {
                case NUMBER_U8:
                        return JSON_LIST_FIXED_U8;
                case NUMBER_U16:
                        return JSON_LIST_FIXED_U16;
                case NUMBER_U32:
                        return JSON_LIST_FIXED_U32;
                case NUMBER_U64:
                        return JSON_LIST_FIXED_U64;
                case NUMBER_I8:
                        return JSON_LIST_FIXED_I8;
                case NUMBER_I16:
                        return JSON_LIST_FIXED_I16;
                case NUMBER_I32:
                        return JSON_LIST_FIXED_I32;
                case NUMBER_I64:
                        return JSON_LIST_FIXED_I64;
                default: ERROR(ERR_UNSUPPORTEDTYPE, NULL);
                        return JSON_LIST_EMPTY;

        }
}

bool json_array_get_type(json_list_type_e *type, const json_array *array)
{
        json_list_type_e list_type = JSON_LIST_EMPTY;
        for (u32 i = 0; i < array->elements.elements.num_elems; i++) {
                const json_element *elem = VEC_GET(&array->elements.elements, i, json_element);
                switch (elem->value.value_type) {
                        case JSON_VALUE_OBJECT:
                        case JSON_VALUE_ARRAY:
                        case JSON_VALUE_STRING:
                                list_type = JSON_LIST_VARIABLE_OR_NESTED;
                                goto return_result;
                        case JSON_VALUE_NUMBER: {
                                json_list_type_e elem_type;
                                switch (elem->value.value.number->value_type) {
                                        case JSON_NUMBER_FLOAT:
                                                elem_type = JSON_LIST_FIXED_FLOAT;
                                                break;
                                        case JSON_NUMBER_UNSIGNED:
                                                elem_type = number_type_to_list_type(number_min_type_unsigned(
                                                        elem->value.value.number->value.unsigned_integer));
                                                break;
                                        case JSON_NUMBER_SIGNED:
                                                elem_type = number_type_to_list_type(number_min_type_signed(
                                                        elem->value.value.number->value.signed_integer));
                                                break;
                                        default: ERROR(ERR_UNSUPPORTEDTYPE, NULL);
                                                continue;
                                }

                                list_type = json_fitting_type(list_type, elem_type);
                                if (list_type == JSON_LIST_VARIABLE_OR_NESTED) {
                                        goto return_result;
                                }
                                break;
                        }
                        case JSON_VALUE_TRUE:
                        case JSON_VALUE_FALSE:
                                list_type = json_fitting_type(list_type, JSON_LIST_FIXED_BOOLEAN);
                                if (list_type == JSON_LIST_VARIABLE_OR_NESTED) {
                                        goto return_result;
                                }
                                break;
                        case JSON_VALUE_NULL:
                                list_type = json_fitting_type(list_type, JSON_LIST_FIXED_NULL);
                                if (list_type == JSON_LIST_VARIABLE_OR_NESTED) {
                                        goto return_result;
                                }
                                break;
                        default: ERROR(ERR_UNSUPPORTEDTYPE, NULL);
                                break;
                }
        }
        return_result:
        *type = list_type;
        return true;

}

static void parse_number_exp(json_number *number, json_token token)
{
    assert(token.type == LITERAL_FLOAT || token.type == LITERAL_INT);

    char *value = MALLOC(token.length + 1);
    strncpy(value, token.string, token.length);
    value[token.length] = '\0';

    if (token.type == LITERAL_INT) {
        i64 assumeSigned = convert_atoi64(value);
        if (value[0] == '-') {
            number->value_type = JSON_NUMBER_SIGNED;
            number->value.signed_integer = assumeSigned;
        } else {
            u64 assumeUnsigned = convert_atoiu64(value);
            if (assumeUnsigned >= (u64) assumeSigned) {
                number->value_type = JSON_NUMBER_UNSIGNED;
                number->value.unsigned_integer = assumeUnsigned;
            } else {
                number->value_type = JSON_NUMBER_SIGNED;
                number->value.signed_integer = assumeSigned;
            }
        }
    } else {
        number->value_type = JSON_NUMBER_FLOAT;
        setlocale(LC_ALL | ~LC_NUMERIC, "");
        number->value.float_number = strtof(value, NULL);
    }

    free(value);
}

static void parse_string_exp(json_string *string, json_token token)
{
    assert(token.type == LITERAL_STRING);

    string->value = MALLOC(token.length + 1);
    if (LIKELY(token.length > 0)) {
        strncpy(string->value, token.string, token.length);
    }
    string->value[token.length] = '\0';
}

static int process_token_exp(json_err *error_desc, const json_token *token, struct token_memory *token_mem)
{
    switch (token_mem->type) {
        case OBJECT_OPEN:
            switch (token->type) {
                case LITERAL_STRING:
                case OBJECT_CLOSE:
                    break;
                default:
                    return set_error(error_desc, token, "Expected key name or '}'");
            }
            break;
        case LITERAL_STRING:
            switch (token->type) {
                case ASSIGN:
                case COMMA:
                case ARRAY_CLOSE:
                case OBJECT_CLOSE:
                    break;
                default:
                    return set_error(error_desc, token,
                                     "Expected key name (missing ':'), enumeration (','), "
                                     "end of enumeration (']'), or end of object ('}')");
            }
            break;
        case OBJECT_CLOSE:
        case LITERAL_INT:
        case LITERAL_FLOAT:
        case LITERAL_TRUE:
        case LITERAL_FALSE:
        case LITERAL_NULL:
            switch (token->type) {
                case COMMA:
                case ARRAY_CLOSE:
                case OBJECT_CLOSE:
                    break;
                default:
                    return set_error(error_desc, token,
                                     "Expected enumeration (','), end of enumeration (']'), "
                                     "or end of object ('})");
            }
            break;
        case ASSIGN:
        case COMMA:
            switch (token->type) {
                case LITERAL_STRING:
                case LITERAL_FLOAT:
                case LITERAL_INT:
                case OBJECT_OPEN:
                case ARRAY_OPEN:
                case LITERAL_TRUE:
                case LITERAL_FALSE:
                case LITERAL_NULL:
                    break;
                default:
                    return set_error(error_desc,
                                     token,
                                     "Expected key name, or value (str_buf, number, object, enumeration, true, "
                                     "false, or null).");
            }
            break;
        case ARRAY_OPEN:
            switch (token->type) {
                case ARRAY_CLOSE:
                case LITERAL_STRING:
                case LITERAL_FLOAT:
                case LITERAL_INT:
                case OBJECT_OPEN:
                case ARRAY_OPEN:
                case LITERAL_TRUE:
                case LITERAL_FALSE:
                case LITERAL_NULL:
                    break;
                default:
                    return set_error(error_desc, token,
                                     "End of enumeration (']'), enumeration (','), or "
                                     "end of enumeration (']')");
            }
            break;
        case ARRAY_CLOSE:
            switch (token->type) {
                case COMMA:
                case ARRAY_CLOSE:
                case OBJECT_CLOSE:
                    break;
                default:
                    return set_error(error_desc, token,
                                     "End of enumeration (']'), enumeration (','), or "
                                     "end of object ('}')");
            }
            break;
        case JSON_UNKNOWN:
            if (token_mem->init) {
                if (token->type != OBJECT_OPEN && token->type != ARRAY_OPEN && !isValue(token->type)) {
                    return set_error(error_desc, token,
                                     "Expected JSON document: missing '{' or '['");
                }
                token_mem->init = false;
            } else {
                return set_error(error_desc, token, "Unexpected token");
            }
            break;
        default:
            return ERROR(ERR_NOJSONTOKEN, NULL);
    }

    token_mem->type = token->type;
    return true;
}

const json_token *json_tokenizer_next_exp(json_tokenizer *tokenizer)
{
    size_t step = 0;

    if (LIKELY(tokenizer->charcount != 0)) {
        char c = *tokenizer->cursor;
        tokenizer->token.string = tokenizer->cursor;
        tokenizer->token.column += tokenizer->token.length;
        tokenizer->token.length = 0;
        if (c == '\n' || c == '\r') {
            tokenizer->token.line += c == '\n' ? 1 : 0;
            tokenizer->token.column = c == '\n' ? 0 : tokenizer->token.column;
            tokenizer->cursor++;
            tokenizer->charcount--;
            return json_tokenizer_next(tokenizer);
        } else if (isspace(c)) {
            do {
                tokenizer->cursor++;
                tokenizer->charcount--;
                tokenizer->token.column++;
            } while (isspace(c = *tokenizer->cursor) && c != '\n');
            return json_tokenizer_next(tokenizer);
        } else if (c == '{' || c == '}' || c == '[' || c == ']' || c == ':' || c == ',') {
            tokenizer->token.type =
                    c == '{' ? OBJECT_OPEN : c == '}' ? OBJECT_CLOSE : c == '[' ? ARRAY_OPEN : c == ']'
                                                                                               ? ARRAY_CLOSE
                                                                                               : c == ':'
                                                                                                 ? ASSIGN
                                                                                                 : COMMA;
            tokenizer->token.column++;
            tokenizer->token.length = 1;
            tokenizer->cursor++;
            tokenizer->charcount--;
        } else if (c != '"' && (isalpha(c) || c == '_') &&
                   (tokenizer->charcount >= 4 && (strncmp(tokenizer->cursor, "null", 4) != 0 &&
                                                  strncmp(tokenizer->cursor, "true", 4) != 0)) &&
                   (tokenizer->charcount >= 5 && strncmp(tokenizer->cursor, "false", 5) != 0)) {
            parse_string_token(tokenizer, c, ' ', ':', ',', true, true);
        } else if (c == '"') {
            parse_string_token(tokenizer, c, '"', '"', '"', false, false);
        } else if (c == 't' || c == 'f' || c == 'n') {
            const unsigned lenTrueNull = 4;
            const unsigned lenFalse = 5;
            const unsigned cursorLen = tokenizer->charcount;
            if (cursorLen >= lenTrueNull && strncmp(tokenizer->cursor, "true", lenTrueNull) == 0) {
                tokenizer->token.type = LITERAL_TRUE;
                tokenizer->token.length = lenTrueNull;
            } else if (cursorLen >= lenFalse && strncmp(tokenizer->cursor, "false", lenFalse) == 0) {
                tokenizer->token.type = LITERAL_FALSE;
                tokenizer->token.length = lenFalse;
            } else if (cursorLen >= lenTrueNull && strncmp(tokenizer->cursor, "null", lenTrueNull) == 0) {
                tokenizer->token.type = LITERAL_NULL;
                tokenizer->token.length = lenTrueNull;
            } else {
                goto caseTokenUnknown;
            }
            tokenizer->token.column++;
            tokenizer->cursor += tokenizer->token.length;
            tokenizer->charcount -= tokenizer->token.length;
        } else if (c == '-' || isdigit(c)) {
            unsigned fracFound = 0, expFound = 0, plusMinusFound = 0;
            bool plusMinusAllowed = false;
            bool onlyDigitsAllowed = false;
            do {
                onlyDigitsAllowed |= plusMinusAllowed;
                plusMinusAllowed = (expFound == 1);
                c = *(++tokenizer->cursor);
                tokenizer->charcount--;
                fracFound += c == '.';
                expFound += (c == 'e') || (c == 'E');
                plusMinusFound += plusMinusAllowed && ((c == '+') || (c == '-')) ? 1 : 0;
                tokenizer->token.length++;
            } while ((((isdigit(c)) || (c == '.' && fracFound <= 1)
                       || (plusMinusAllowed && (plusMinusFound <= 1) && ((c == '+') || (c == '-')))
                       || ((c == 'e' || c == 'E') && expFound <= 1))) && c != '\n' && c != '\r');

            if (!isdigit(*(tokenizer->cursor - 1))) {
                tokenizer->token.column -= tokenizer->token.length;
                goto caseTokenUnknown;
            }

            step = (c == '\r' || c == '\n') ? 1 : 0;
            tokenizer->cursor += step;
            tokenizer->charcount -= step;
            tokenizer->token.type = fracFound ? LITERAL_FLOAT : LITERAL_INT;
        } else {
            caseTokenUnknown:
            tokenizer->token.type = JSON_UNKNOWN;
            tokenizer->token.column++;
            tokenizer->token.length = tokenizer->charcount;
            tokenizer->cursor += tokenizer->token.length;
            tokenizer->charcount -= tokenizer->token.length;
        }
        return &tokenizer->token;
    } else {
        return NULL;
    }
}

static bool parse_element_exp(json_parser *parser, json_element *element, json_token token, json_err *error_desc, struct token_memory *token_mem)
{
    switch(token.type) {
        case OBJECT_OPEN: /** Parse object */
            element->value.value_type = JSON_VALUE_OBJECT;
            element->value.value.object = MALLOC(sizeof(json_object));

            parse_object_exp(parser, element->value.value.object, error_desc, token_mem, 20);
            break;
        case ARRAY_OPEN: /** Parse array */
            element->value.value_type = JSON_VALUE_ARRAY;
            element->value.value.array = MALLOC(sizeof(json_array));

            parse_array_exp(parser, element->value.value.array, error_desc, token_mem, 150);
            break;
        case LITERAL_STRING: /** Parse string */
            element->value.value_type = JSON_VALUE_STRING;
            element->value.value.string = MALLOC(sizeof(json_string));
            parse_string_exp(element->value.value.string, token);
            break;
        case LITERAL_FLOAT:
        case LITERAL_INT: /** Parse number */
            element->value.value_type = JSON_VALUE_NUMBER;
            element->value.value.number = MALLOC(sizeof(json_number));
            parse_number_exp(element->value.value.number, token);
            break;
        case LITERAL_TRUE:
            element->value.value_type = JSON_VALUE_TRUE;
            break;
        case LITERAL_FALSE:
            element->value.value_type = JSON_VALUE_FALSE;
            break;
        default:
            element->value.value_type = JSON_VALUE_NULL;
            break;
    }
    return true;
}

static bool parse_elements_exp(json_elements *elements, json_parser *parser, json_err *error_desc, struct token_memory *token_mem)
{
    const json_token* delimiter;
    do {
        json_token current = *json_tokenizer_next_exp(&parser->tokenizer);
        if (current.type != ARRAY_CLOSE && current.type != OBJECT_CLOSE) {
            if (!parse_element_exp(parser, VEC_NEW_AND_GET(&elements->elements, json_element), current, error_desc, token_mem)) {
                //TODO set error
                return false;
            }
        }
        delimiter = json_tokenizer_next_exp(&parser->tokenizer);
    } while (delimiter != NULL && delimiter->type == COMMA);

    return true;
}

bool parse_members_exp(json_members *members, json_parser *parser, json_err *error_desc, struct token_memory *token_mem, size_t cap_elems)
{
    vec_create(&members->members, sizeof(json_prop), cap_elems);
    //vec_create(&members->members, sizeof(json_prop), 20);
    const json_token* delimiter_token;

    do {
        json_prop *member = VEC_NEW_AND_GET(&members->members, json_prop);
        json_token keyNameToken = *json_tokenizer_next_exp(&parser->tokenizer);

        member->key.value = MALLOC(keyNameToken.length + 1);
        strncpy(member->key.value, keyNameToken.string, keyNameToken.length);
        member->key.value[keyNameToken.length] = '\0';

        json_token assignment_token = *json_tokenizer_next_exp(&parser->tokenizer);
        if (assignment_token.type != ASSIGN) {
            //TODO set error
            return false;
        }

        json_token valueToken = *json_tokenizer_next_exp(&parser->tokenizer);
        size_t pred_count = 0;
        switch (valueToken.type) {
            case OBJECT_OPEN:
                member->value.value.value_type = JSON_VALUE_OBJECT;
                member->value.value.value.object = MALLOC(sizeof(json_object));
                pred_count = stats_get_prediction(parser->stats, member->key.value);

                parse_object_exp(parser, member->value.value.value.object, error_desc, token_mem, pred_count);
                break;
            case ARRAY_OPEN:
                member->value.value.value_type = JSON_VALUE_ARRAY;
                member->value.value.value.array = MALLOC(sizeof(json_array));
                pred_count = stats_get_prediction(parser->stats, member->key.value);
                parse_array_exp(parser, member->value.value.value.array, error_desc, token_mem, pred_count);
                break;
            case LITERAL_STRING:
                member->value.value.value_type = JSON_VALUE_STRING;
                member->value.value.value.string = MALLOC(sizeof(json_string));
                parse_string_exp(member->value.value.value.string, valueToken);
                break;
            case LITERAL_INT:
            case LITERAL_FLOAT:
                member->value.value.value_type = JSON_VALUE_NUMBER;
                member->value.value.value.number = MALLOC(sizeof(json_number));
                parse_number_exp(member->value.value.value.number, valueToken);
                break;
            case LITERAL_TRUE:
                member->value.value.value_type = JSON_VALUE_TRUE;
                break;
            case LITERAL_FALSE:
                member->value.value.value_type = JSON_VALUE_FALSE;
                break;
            case LITERAL_NULL:
                member->value.value.value_type = JSON_VALUE_NULL;
                break;
            default:
                return ERROR(ERR_PARSETYPE, NULL);
        }

        delimiter_token = json_tokenizer_next_exp(&parser->tokenizer);
    } while (delimiter_token != NULL && delimiter_token->type == COMMA);

    return true;
}

static bool parse_array_exp(json_parser *parser, json_array *array, json_err *error_desc, struct token_memory *token_mem, size_t cap_elems)
{
    vec_create(&array->elements.elements, sizeof(json_element), cap_elems);
    //vec_create(&array->elements.elements, sizeof(json_element), 250);
    return parse_elements_exp(&array->elements, parser, error_desc, token_mem);

}

static bool parse_object_exp(json_parser *parser, json_object *object, json_err *error_desc, struct token_memory *token_mem, size_t cap_elems)
{
    object->value = MALLOC(sizeof(json_members));

    return parse_members_exp(object->value, parser, error_desc, token_mem, cap_elems);
}

static bool json_parse_input_exp(json *json, json_err *error_desc, json_parser *parser)
{

    struct json retval;
    ZERO_MEMORY(&retval, sizeof(json))
    retval.element = MALLOC(sizeof(json_element));
    const json_token *token;
    int status;

    struct token_memory token_mem = {.init = true, .type = JSON_UNKNOWN};

    token = json_tokenizer_next_exp(&parser->tokenizer);
    if (LIKELY((status = process_token_exp(error_desc, token, &token_mem)) == true)) {
        switch(token->type){
            case OBJECT_OPEN:
                retval.element->value.value_type = JSON_VALUE_OBJECT;
                retval.element->value.value.object = MALLOC(sizeof(json_object));

                parse_object_exp(parser, retval.element->value.value.object, error_desc, &token_mem,20);
                break;
            case ARRAY_OPEN:
                retval.element->value.value_type = JSON_VALUE_ARRAY;
                retval.element->value.value.array = MALLOC(sizeof(json_array));

                parse_array_exp(parser, retval.element->value.value.array, error_desc, &token_mem,150);
                break;
            default:
                //TODO set error
                goto cleanup;
        }
    } else {
        //TODO set error
        goto cleanup;
    }

    OPTIONAL_SET_OR_ELSE(json, retval, json_drop(json));
    status = true;

    cleanup:
    return status;
}


bool
json_parse_exp(json *json, json_err *error_desc, json_parser *parser, const char *input)
{
    if(!json_parse_check_input(error_desc, input, 0))
    {
        //TODO set error
        return false;
    }

    json_tokenizer_init(&parser->tokenizer, input);

    /*const json_token *token;
    while ((token = json_tokenizer_next_exp(&parser->tokenizer)) != NULL){}
    json_tokenizer_init(&parser->tokenizer, input);*/
    return json_parse_input_exp(json, error_desc, parser);
}

bool
json_parse_limited_exp(json *json, json_err *error_desc, json_parser *parser, const char *input, size_t charcount)
{
    if(!json_parse_check_input(error_desc, input, charcount))
    {
        return false;
    }

    json_tokenizer_init_limited(&parser->tokenizer, input, charcount);

    return json_parse_input_exp(json, error_desc, parser);
}

bool
json_parse_split_exp(const char *input, size_t size_input, const char* destdir, const char* filename, parseStats* stats)
{
    size_t i = 0;
    size_t lastPart = 0;
    int l = 0;
    const char* currentPart;

    bool end_parse = false;

    while (!end_parse)
    {
        if((input[i] == '\n') || (input[i] == '\0'))
        {
            if((input[i] == '\0') || (i == size_input))
            {
                end_parse = true;
            }

            //set pointer to the beginning of current part
            currentPart = input + lastPart;

            //rec doc;
            l++;

            if(destdir == NULL)
            {
                struct json data;
                json_err err;
                json_parser parser;
                parser.stats = stats;

                if(json_parse_limited_exp(&data, &err, &parser, currentPart, i-lastPart)) {
                    json_drop(&data);
                }
            }
            else
            {
                /*if(rec_from_json_limited(&doc, currentPart, KEY_NOKEY, NULL, i-lastPart))
                {
                    size_t filepathsize = strlen(destdir) + strlen(filename) + 28;

                    char filepath[filepathsize];
                    snprintf(filepath, filepathsize, "%s%s%u%s", destdir, filename, l, ".carbon");

                    FILE *file;
                    file = fopen(filepath, "w");

                    MEMFILE_SAVE_POSITION(&doc.file);
                    MEMFILE_SEEK(&doc.file, 0);
                    fwrite(MEMFILE_PEEK(&doc.file, 1), MEMFILE_SIZE(&doc.file), 1, file);
                    MEMFILE_RESTORE_POSITION(&doc.file);
                    fclose(file);
                    remove(filepath);
                    rec_drop(&doc);
                }*/

                struct json data;
                json_err err;
                json_parser parser;
                parser.stats = stats;

                if(json_parse_limited_exp(&data, &err, &parser, currentPart, i-lastPart)) {
                    size_t filepathsize = strlen(destdir) + strlen(filename) + 28;

                    char filepath[filepathsize];
                    snprintf(filepath, filepathsize, "%s%s%u%s", destdir, filename, l, ".carbon");

                    FILE *file;
                    file = fopen(filepath, "w");
                    if(file == NULL)
                    {
                        file = NULL;
                    }
                    json_print(file, &data);
                    fclose(file);
                    json_drop(&data);
                }
            }
            lastPart = i+1;
        }
        i++;
    }

    return true;
}

/*-----------------------------------------------------------------------------------------------
 * start hashtable*/

void init_parseStats(parseStats* stats)
{
    stats->size = 50;
    stats->count = 0;

    size_t i = 0;
    while (i < stats->size)
    {
        stats->arr[i] = NULL;
        i++;
    }
}

static void insert_statsElement(parseStats* stats, char* key, size_t count)
{
    if(stats->count == stats->size)
    {
        //TODO ERROR or realloc with more elements
        return;
    }

    size_t key_hash = HASH_ADDITIVE(strlen(key), key);
    size_t pos = key_hash % stats->size;

    while (stats->arr[pos] != NULL)
    {
        pos++;
        pos %= stats->size;
    }

    statsElement* item = malloc(sizeof(statsElement));
    item->key = key;
    item->count = count;
    item->sample_size = 1;
    item->max_count = count;

    stats->arr[pos] = item;
    stats->count++;
}

static statsElement* get_statsElement(parseStats* stats, char* key)
{
    //size_t a = strlen(key);
    size_t key_hash = HASH_ADDITIVE(strlen(key), key);
    size_t pos = key_hash % stats->size;

    if(stats->count != stats->size)
    {
        while((stats->arr[pos] != NULL) && (strcmp((stats->arr[pos])->key, key) != 0))
        {
            pos++;
            pos %= stats->size;
        }
    }
    else
    {
        size_t orig_pos = pos;

        while((stats->arr[pos] != NULL) && (strcmp((stats->arr[pos])->key, key) != 0))
        {
            pos++;
            pos %= stats->size;

            if(pos == orig_pos)
            {
                return NULL;
            }
        }
    }

    return stats->arr[pos];
}

static size_t stats_get_prediction(parseStats* stats, char* key)
{
    if(stats == NULL)
    {
        return 20;
    }

    statsElement* elem = get_statsElement(stats, key);

    if(elem == NULL)
    {
        return 20;
    }

    return ((elem->count/elem->sample_size) + elem->max_count)/2;
}

static void update_statsElement(parseStats* stats, char* key, statsElement* elem, size_t count)
{
    if(elem == NULL)
    {
        elem = get_statsElement(stats, key);
    }

    elem->count += count;
    elem->sample_size++;

    if(elem->max_count < count)
    {
        elem->max_count = count;
    }
}

static void update_or_insert_statsElement(parseStats* stats, char* key, size_t count)
{
    statsElement* elem = get_statsElement(stats, key);

    if(elem == NULL)
    {
        insert_statsElement(stats, key, count);
    }
    else
    {
        update_statsElement(stats, key, elem, count);
    }

    return;
}

static bool build_parseStats_get_key_check_escaped(const char* input, size_t pos) {
    char c = input[--pos];
    bool escaped = false;

    while(c == '\\')
    {
        escaped = !escaped;
        c = input[--pos];
    }

    return escaped;
}

static char* build_parseStats_get_key(const char* input, size_t pos) {
    size_t start = 0;
    size_t end = 0;

    char c = input[--pos];

    while (isspace(c)) {
        if (pos == 0) {
            return NULL;
        }
        c = input[--pos];
    }

    //check if possible end of String
    if (c != '"') {
        return NULL;
    }

    if(!build_parseStats_get_key_check_escaped(input, pos))
    {
        end = pos;
        c = input[--pos];

        while((c != '"') || (build_parseStats_get_key_check_escaped(input, pos)))
        {
            c = input[--pos];
        }

        start = pos+1;
    }

    char* res = malloc(end-start+1);
    strncpy(res, input+start, end-start);
    res[end-start+1] = '\0';

    return res;
}

static size_t build_parseStats_get_count(const char* input, size_t pos, size_t input_size) {
    size_t depth = 0;
    char c = input[++pos];
    bool escaped = false;
    bool in_string = false;
    char start;
    char end;
    size_t count = 0;

    while(isspace(c))
    {
        c = input[++pos];
    }

    //if no object or array is opened
    if((c != '{') && (c != '['))
    {
        //return 0, which indicates, that it was no object or array
        return 0;
    }

    start = c;

    if(start == '{')
    {
        end = '}';
    } else {
        end = ']';
    }
    pos++;
    count++;
    while(((c != end) || depth != 0 || (escaped)) && (pos < input_size))
    {
        c = input[pos++];

        if(isspace(c))
        {
            continue;
        }

        //toggle in string and out of string, if not escaped
        if((c == '"') && (!escaped))
        {
            in_string = !in_string;
        }
        else if((c == start) && (!escaped) && (!in_string))
        {
            depth++;
        }
        else if((c == end) && (!escaped) && (!in_string))
        {
            if(depth == 0)
            {
                return count+1;
            }
            depth--;
        }
        else if((c == ',') && !escaped && !in_string && (depth == 0))
        {
            count++;
        }


        if(c == '\\')
        {
            escaped = !escaped;
        }
        else
        {
            escaped = false;
        }
    }

    //increase count by 1, because 0 means it is neither an array, nor object
    return count+1;
}

void build_parseStats(parseStats* stats, const char* input, size_t input_size)
{
    bool escaped = false;
    bool in_string = false;

    size_t i = 0;
    size_t count = 0;

    char* key;

    char c;
    //madvise((void*) input, input_size, MADV_SEQUENTIAL);
    while(i < input_size)
    {
        c = input[i];
        i++;


        if(isspace(c))
        {
            continue;
        }

        //toggle in string and out of string, if not escaped
        if((c == '"') && (!escaped))
        {
            in_string = !in_string;
        }

        if((c == ':') && (!in_string) && (!escaped))
        {
            key = build_parseStats_get_key(input, i-1);
            if(key != NULL)
            {
                count = build_parseStats_get_count(input, i - 1, input_size);

                if (count != 0)
                {
                    update_or_insert_statsElement(stats, key, count-1);
                }
            }
        }


        if(c == '\\')
        {
            escaped = !escaped;
        }
        else
        {
            escaped = false;
        }
    }
}

static void drop_parseStats(parseStats* stats)
{
    size_t i = 0;

    while ((stats->count != 0) && (i < stats->size))
    {
        if(stats->arr[i] != NULL)
        {
            free(stats->arr[i]);
            stats->arr[i] = NULL;
            stats->count--;
        }
        i++;
    }
}
