/**
 * Columnar Binary JSON -- Copyright 2019 Marcus Pinnecke
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

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <ctype.h>
#include <jakson/utils/convert.h>
#include <jakson/carbon/dot.h>
#include <jakson/std/string.h>
#include <jakson/utils/strings.h>

enum dot_token_type {
        TOKEN_DOT,
        TOKEN_STRING,
        TOKEN_NUMBER,
        TOKEN_UNKNOWN,
        TOKEN_EOF
};

struct dot_token {
        enum dot_token_type type;
        const char *str;
        u32 len;
};

static const char *next_token(struct dot_token *token, const char *str)
{
        assert(token);
        assert(str);

        str = strings_skip_blanks(str);
        char c = *str;
        if (c) {
                if (isalpha(c)) {
                        token->type = TOKEN_STRING;
                        token->str = str;
                        bool skip_esc = false;
                        u32 strlen = 0;
                        while (c && (isalpha(c) && (c != '\n') && (c != '\t') && (c != '\r') && (c != ' '))) {
                                if (!skip_esc) {
                                        if (c == '\\') {
                                                skip_esc = true;
                                        }
                                } else {
                                        skip_esc = false;
                                }
                                strlen++;
                                c = *(++str);
                        }
                        token->len = strlen;
                } else if (c == '\"') {
                        token->type = TOKEN_STRING;
                        token->str = str;
                        c = *(++str);
                        bool skip_esc = false;
                        bool end_found = false;
                        u32 strlen = 1;
                        while (c && !end_found) {
                                if (!skip_esc) {
                                        if (c == '\\') {
                                                skip_esc = true;
                                        } else if (c == '\"') {
                                                end_found = true;
                                        }
                                } else {
                                        skip_esc = false;
                                }

                                strlen++;
                                c = *(++str);
                        }
                        token->len = strlen;
                } else if (c == '.') {
                        token->type = TOKEN_DOT;
                        token->str = str;
                        token->len = 1;
                        str++;
                } else if (isdigit(c)) {
                        token->type = TOKEN_NUMBER;
                        token->str = str;
                        u32 strlen = 0;
                        while (c && isdigit(c)) {
                                c = *(++str);
                                strlen++;
                        }
                        token->len = strlen;
                } else {
                        token->type = TOKEN_UNKNOWN;
                        token->str = str;
                        token->len = strlen(str);
                }
        } else {
                token->type = TOKEN_EOF;
        }
        return str;
}

bool dot_create(dot *path)
{
        path->len = 0;
        ZERO_MEMORY(&path->nodes, ARRAY_LENGTH(path->nodes) * sizeof(dot_node));
        return true;
}

bool dot_from_string(dot *path, const char *path_string)
{
        UNUSED(path_string);

        struct dot_token token;
        int status = ERR_NOERR;
        dot_create(path);

        enum path_entry {
                DOT, ENTRY
        } expected_entry = ENTRY;
        path_string = next_token(&token, path_string);
        while (token.type != TOKEN_EOF) {
                expected_entry = token.type == TOKEN_DOT ? DOT : ENTRY;
                switch (token.type) {
                        case TOKEN_DOT:
                                if (expected_entry != DOT) {
                                        status = ERR_PARSE_DOT_EXPECTED;
                                        goto cleanup_and_error;
                                }
                                break;
                        case TOKEN_STRING:
                                if (expected_entry != ENTRY) {
                                        status = ERR_PARSE_ENTRY_EXPECTED;
                                        goto cleanup_and_error;
                                } else {
                                        dot_add_nkey(path, token.str, token.len);
                                }
                                break;
                        case TOKEN_NUMBER:
                                if (expected_entry != ENTRY) {
                                        status = ERR_PARSE_ENTRY_EXPECTED;
                                        goto cleanup_and_error;
                                } else {
                                        u64 num = convert_atoiu64(token.str);
                                        dot_add_idx(path, num);
                                }
                                break;
                        case TOKEN_UNKNOWN:
                                status = ERR_PARSE_UNKNOWN_TOKEN;
                                goto cleanup_and_error;
                        default: error(ERR_INTERNALERR, NULL);
                                break;
                }
                path_string = next_token(&token, path_string);
        }

        return true;

        cleanup_and_error:
        dot_drop(path);
        error(status, NULL);
        return false;
}

bool dot_add_key(dot *dst, const char *key)
{
        return dot_add_nkey(dst, key, strlen(key));
}

bool dot_add_nkey(dot *dst, const char *key, size_t len)
{
        if (LIKELY(dst->len < ARRAY_LENGTH(dst->nodes))) {
                dot_node *node = dst->nodes + dst->len++;
                bool enquoted = strings_is_enquoted_wlen(key, len);
                node->type = DOT_NODE_KEY;
                node->name.string = strndup(enquoted ? key + 1 : key, len);
                if (enquoted) {
                        char *str_wo_rightspaces = strings_remove_tailing_blanks(node->name.string);
                        size_t l = strlen(str_wo_rightspaces);
                        node->name.string[l - 1] = '\0';
                }
                assert(!strings_is_enquoted(node->name.string));
                return true;
        } else {
                return error(ERR_OUTOFBOUNDS, NULL);
        }
}

bool dot_add_idx(dot *dst, u32 idx)
{
        if (LIKELY(dst->len < ARRAY_LENGTH(dst->nodes))) {
                dot_node *node = dst->nodes + dst->len++;
                node->type = DOT_NODE_IDX;
                node->name.idx = idx;
                return true;
        } else {
                return error(ERR_OUTOFBOUNDS, NULL);
        }
}

bool dot_len(u32 *len, const dot *path)
{
        *len = path->len;
        return true;
}

bool dot_is_empty(const dot *path)
{
        return (path->len == 0);
}

bool dot_type_at(dot_node_type_e *type_out, u32 pos, const dot *path)
{
        if (LIKELY(pos < ARRAY_LENGTH(path->nodes))) {
                *type_out = path->nodes[pos].type;
        } else {
                return error(ERR_OUTOFBOUNDS, NULL);
        }
        return true;
}

bool dot_idx_at(u32 *idx, u32 pos, const dot *path)
{
        error_if_and_return(pos >= ARRAY_LENGTH(path->nodes), ERR_OUTOFBOUNDS, NULL);
        error_if_and_return(path->nodes[pos].type != DOT_NODE_IDX, ERR_TYPEMISMATCH, NULL);

        *idx = path->nodes[pos].name.idx;
        return true;
}

const char *dot_key_at(u32 pos, const dot *path)
{
        error_if_and_return(pos >= ARRAY_LENGTH(path->nodes), ERR_OUTOFBOUNDS, NULL);
        error_if_and_return(path->nodes[pos].type != DOT_NODE_KEY, ERR_TYPEMISMATCH, NULL);

        return path->nodes[pos].name.string;
}

bool dot_drop(dot *path)
{
        for (u32 i = 0; i < path->len; i++) {
                dot_node *node = path->nodes + i;
                if (node->type == DOT_NODE_KEY) {
                        free(node->name.string);
                }
        }
        path->len = 0;
        return true;
}

bool dot_to_str(str_buf *sb, dot *path)
{
        for (u32 i = 0; i < path->len; i++) {
                dot_node *node = path->nodes + i;
                switch (node->type) {
                        case DOT_NODE_KEY: {
                                bool empty_str = strlen(node->name.string) == 0;
                                bool quotes_required =
                                        empty_str || strings_contains_blank_char(node->name.string);
                                if (quotes_required) {
                                        str_buf_add_char(sb, '"');
                                }
                                if (!empty_str) {
                                        str_buf_add(sb, node->name.string);
                                }
                                if (quotes_required) {
                                        str_buf_add_char(sb, '"');
                                }
                        }
                                break;
                        case DOT_NODE_IDX:
                                str_buf_add_u32(sb, node->name.idx);
                                break;
                }
                if (i + 1 < path->len) {
                        str_buf_add_char(sb, '.');
                }
        }
        return true;
}

bool dot_fprint(FILE *file, dot *path)
{
        str_buf sb;
        str_buf_create(&sb);
        dot_to_str(&sb, path);
        fprintf(file, "%s", string_cstr(&sb));
        str_buf_drop(&sb);
        return true;
}

bool dot_print(dot *path)
{
        return dot_fprint(stdout, path);
}