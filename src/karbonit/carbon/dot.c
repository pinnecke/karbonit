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
#include <karbonit/utils/convert.h>
#include <karbonit/carbon/dot.h>
#include <karbonit/std/string.h>
#include <karbonit/utils/strings.h>

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
        vec_create(&path->nodes, sizeof(dot_node), 256);
        return true;
}

void dot_clear(dot *path)
{
        vec_clear(&path->nodes);
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
                        default: ERROR(ERR_INTERNALERR, NULL);
                                break;
                }
                path_string = next_token(&token, path_string);
        }

        return true;

        cleanup_and_error:
        dot_drop(path);
        ERROR(status, NULL);
        return false;
}

void dot_add_key(dot *dst, const char *key)
{
        dot_add_nkey(dst, key, strlen(key));
}

void dot_add_nkey(dot *dst, const char *key, size_t len)
{
        dot_node *node = VEC_NEW_AND_GET(&dst->nodes, dot_node);
        bool enquoted = strings_is_enquoted_wlen(key, len);
        node->type = DOT_NODE_KEY;
        node->name.string = strndup(enquoted ? key + 1 : key, len);
        if (enquoted) {
                char *str_wo_rightspaces = strings_remove_tailing_blanks(node->name.string);
                size_t l = strlen(str_wo_rightspaces);
                node->name.string[l - 1] = '\0';
        }
        assert(!strings_is_enquoted(node->name.string));
}

void dot_add_idx(dot *dst, u32 idx)
{
        dot_node *node = VEC_NEW_AND_GET(&dst->nodes, dot_node);
        node->type = DOT_NODE_IDX;
        node->name.idx = idx;
}

bool dot_len(u32 *len, const dot *path)
{
        *len = VEC_LENGTH(&path->nodes);
        return true;
}

bool dot_is_empty(const dot *path)
{
        return (VEC_LENGTH(&path->nodes) == 0);
}

bool dot_type_at(dot_node_type_e *type_out, u32 pos, const dot *path)
{
        if (LIKELY(pos < VEC_LENGTH(&path->nodes))) {
                *type_out = VEC_GET(&path->nodes, pos, dot_node)->type;
        } else {
                return ERROR(ERR_OUTOFBOUNDS, NULL);
        }
        return true;
}

bool dot_idx_at(u32 *idx, u32 pos, const dot *path)
{
        ERROR_IF_AND_RETURN(pos >= VEC_LENGTH(&path->nodes), ERR_OUTOFBOUNDS, NULL);
        ERROR_IF_AND_RETURN(VEC_GET(&path->nodes, pos, dot_node)->type != DOT_NODE_IDX, ERR_TYPEMISMATCH, NULL);

        *idx = VEC_GET(&path->nodes, pos, dot_node)->name.idx;
        return true;
}

const char *dot_key_at(u32 pos, const dot *path)
{
        ERROR_IF_AND_RETURN(pos >= VEC_LENGTH(&path->nodes), ERR_OUTOFBOUNDS, NULL);
        ERROR_IF_AND_RETURN(VEC_GET(&path->nodes, pos, dot_node)->type != DOT_NODE_KEY, ERR_TYPEMISMATCH, NULL);

        return VEC_GET(&path->nodes, pos, dot_node)->name.string;
}

bool dot_drop(dot *path)
{
        for (u32 i = 0; i < VEC_LENGTH(&path->nodes); i++) {
                dot_node *node = VEC_GET(&path->nodes, i, dot_node);
                if (node->type == DOT_NODE_KEY) {
                        free(node->name.string);
                }
        }
        vec_drop(&path->nodes);
        return true;
}

bool dot_to_str(str_buf *sb, dot *path)
{
        for (u32 i = 0; i < VEC_LENGTH(&path->nodes); i++) {
                dot_node *node = VEC_GET(&path->nodes, i, dot_node);
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
                if (i + 1 < VEC_LENGTH(&path->nodes)) {
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
        fprintf(file, "%s", str_buf_cstr(&sb));
        str_buf_drop(&sb);
        return true;
}

bool dot_print(dot *path)
{
        return dot_fprint(stdout, path);
}