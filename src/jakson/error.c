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

#include <jakson/error.h>

_Thread_local struct err_info g_err = {
        .line = 0,
        .file = NULL,
        .code = ERR_NOERR,
        .details = NULL,
        .noabort = false
};

void error_abort_enable()
{
        g_err.noabort = false;
}

void error_abort_disable()
{
        g_err.noabort = true;
}

void error_set(int code, const char *file, u32 line, const char *details)
{
        error_clear();
        g_err.code = code;
        g_err.file = file;
        g_err.line = line;
        g_err.details = details ? strdup(details) : NULL;
}

void error_clear()
{
        if (g_err.details) {
                free(g_err.details);
                g_err.details = NULL;
        }
        g_err.line = 0;
        g_err.code = ERR_NOERR;
        g_err.file = NULL;
}

const char *error_get_str()
{
        return global_err_str[g_err.code];
}

int error_get_code()
{
        return g_err.code;
}

const char *error_get_file(u32 *line)
{
        *line = g_err.line;
        return g_err.file;
}

const char *error_get_details()
{
        return g_err.details;
}

void error_print(FILE *file)
{
        fprintf(file, "** ERROR **  %s (%d)\n", error_get_str(), g_err.code);
        fprintf(file, "             in %s:%d\n", g_err.file, g_err.line);
        fprintf(file, "             %s\n", g_err.details ? g_err.details : "no details");
}