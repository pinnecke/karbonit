/*
 * Copyright 2018 Marcus Pinnecke
 */

#ifndef HAD_JSON_PARSER_H
#define HAD_JSON_PARSER_H

#include <karbonit/stdinc.h>
#include <karbonit/std/vec.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum json_token_type {
        OBJECT_OPEN,
        OBJECT_CLOSE,
        LITERAL_STRING,
        LITERAL_INT,
        LITERAL_FLOAT,
        LITERAL_TRUE,
        LITERAL_FALSE,
        LITERAL_NULL,
        COMMA,
        ASSIGN,
        ARRAY_OPEN,
        ARRAY_CLOSE,
        JSON_UNKNOWN
} json_token_e;

typedef struct json_token {
        json_token_e type;
        const char *string;
        unsigned line;
        unsigned column;
        unsigned length;
} json_token;

typedef struct json_err {
        const json_token *token;
        const char *token_type_str;
        const char *msg;
} json_err;

typedef struct json_tokenizer {
        const char *cursor;
        json_token token;
} json_tokenizer;

typedef struct json_parser {
        json_tokenizer tokenizer;
} json_parser;

typedef enum json_parent {
        JSON_PARENT_OBJECT, JSON_PARENT_MEMBER, JSON_PARENT_ELEMENTS
} json_parent_e;

typedef enum json_value_type_e {
        JSON_VALUE_OBJECT,
        JSON_VALUE_ARRAY,
        JSON_VALUE_STRING,
        JSON_VALUE_NUMBER,
        JSON_VALUE_TRUE,
        JSON_VALUE_FALSE,
        JSON_VALUE_NULL
} json_value_type_e;

typedef enum json_list_type_e {
        JSON_LIST_EMPTY,
        JSON_LIST_VARIABLE_OR_NESTED,
        JSON_LIST_FIXED_U8,
        JSON_LIST_FIXED_U16,
        JSON_LIST_FIXED_U32,
        JSON_LIST_FIXED_U64,
        JSON_LIST_FIXED_I8,
        JSON_LIST_FIXED_I16,
        JSON_LIST_FIXED_I32,
        JSON_LIST_FIXED_I64,
        JSON_LIST_FIXED_FLOAT,
        JSON_LIST_FIXED_NULL,
        JSON_LIST_FIXED_BOOLEAN
} json_list_type_e;

typedef struct json {
        json_element *element;
} json;

typedef struct json_node_value {
        json_element *parent;
        json_value_type_e value_type;
        union {
                json_object *object;
                json_array *array;
                json_string *string;
                json_number *number;
                void *ptr;
        } value;
} json_node_value;

typedef struct json_object {
        json_node_value *parent;
        json_members *value;
} json_object;

typedef struct json_element {
        json_parent_e parent_type;
        union {
                json *json;
                json_prop *member;
                json_elements *elements;
                void *ptr;
        } parent;
        json_node_value value;
} json_element;

typedef struct json_string {
        json_prop *parent;
        char *value;
} json_string;

typedef struct json_prop {
        json_members *parent;
        json_string key;
        json_element value;
} json_prop;

typedef struct json_members {
        json_object *parent;
        vec ofType(json_prop) members;
} json_members;

typedef struct json_elements {
        json_array *parent;
        vec ofType(json_element) elements;
} json_elements;

typedef struct json_array {
        json_node_value *parent;
        json_elements elements;
} json_array;

typedef enum json_number_type {
        JSON_NUMBER_FLOAT, JSON_NUMBER_UNSIGNED, JSON_NUMBER_SIGNED
} json_number_type_e;

typedef struct json_number {
        json_node_value *parent;
        json_number_type_e value_type;
        union {
                float float_number;
                i64 signed_integer;
                u64 unsigned_integer;
        } value;
} json_number;

bool json_tokenizer_init(json_tokenizer *tokenizer, const char *input);
const json_token *json_tokenizer_next(json_tokenizer *tokenizer);
void json_token_dup(json_token *dst, const json_token *src);
void json_token_print(FILE *file, const json_token *token);
bool json_parse(json *json, json_err *error_desc, json_parser *parser, const char *input);
bool json_test(json *json);
bool json_drop(json *json);
bool json_print(FILE *file, json *json);
bool json_list_is_empty(const json_elements *elements);
bool json_list_length(u32 *len, const json_elements *elements);
json_list_type_e json_fitting_type(json_list_type_e current, json_list_type_e to_add);
bool json_array_get_type(json_list_type_e *type, const json_array *array);

#ifdef __cplusplus
}
#endif

#endif