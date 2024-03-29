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

#ifndef COMMON_H
#define COMMON_H

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <inttypes.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

#include <karbonit/forwdecl.h>

#ifndef __cplusplus

# include <stdatomic.h>

#else
# include <atomic>
# define _Atomic(X) std::atomic< X >
#endif

#define MALLOC(size)                \
({                                      \
        void *ptr = malloc(size);       \
        ZERO_MEMORY(ptr, size);     \
        ptr;                            \
})

#define QUERY_LIMIT_NONE -1
#define QUERY_LIMIT_1     1

#define ARRAY_LENGTH(x)                                                                                            \
    sizeof(x)/sizeof(x[0])

typedef uint64_t offset_t;
typedef int64_t signed_offset_t;

typedef unsigned char u_char;

typedef enum archive_field_type {
        ARCHIVE_FIELD_NULL = 0,
        ARCHIVE_FIELD_BOOLEAN = 1,
        ARCHIVE_FIELD_INT8 = 2,
        ARCHIVE_FIELD_INT16 = 3,
        ARCHIVE_FIELD_INT32 = 4,
        ARCHIVE_FIELD_INT64 = 5,
        ARCHIVE_FIELD_UINT8 = 6,
        ARCHIVE_FIELD_UINT16 = 7,
        ARCHIVE_FIELD_UINT32 = 8,
        ARCHIVE_FIELD_UINT64 = 9,
        ARCHIVE_FIELD_FLOAT = 10,
        ARCHIVE_FIELD_STRING = 11,
        ARCHIVE_FIELD_OBJECT = 12,
        ARCHIVE_FIELD_ERR    = 13
} archive_field_e;

typedef enum access_mode_e {
        READ_WRITE,
        READ_ONLY
} access_mode_e;

#define MAYBE_UNUSED __attribute__((unused))

MAYBE_UNUSED static const char *basic_type_to_json_type_str(enum archive_field_type t)
{
        switch (t) {
                case ARCHIVE_FIELD_INT8:
                case ARCHIVE_FIELD_INT16:
                case ARCHIVE_FIELD_INT32:
                case ARCHIVE_FIELD_INT64:
                case ARCHIVE_FIELD_UINT8:
                case ARCHIVE_FIELD_UINT16:
                case ARCHIVE_FIELD_UINT32:
                case ARCHIVE_FIELD_UINT64:
                        return "integer";
                case ARCHIVE_FIELD_FLOAT:
                        return "float";
                case ARCHIVE_FIELD_STRING:
                        return "str_buf";
                case ARCHIVE_FIELD_BOOLEAN:
                        return "boolean";
                case ARCHIVE_FIELD_NULL:
                        return "null";
                case ARCHIVE_FIELD_OBJECT:
                        return "object";
                default:
                        return "(unknown)";
        }
}

MAYBE_UNUSED static const char *basic_type_to_system_type_str(enum archive_field_type t)
{
        switch (t) {
                case ARCHIVE_FIELD_INT8:
                        return "int8";
                case ARCHIVE_FIELD_INT16:
                        return "int16";
                case ARCHIVE_FIELD_INT32:
                        return "int32";
                case ARCHIVE_FIELD_INT64:
                        return "int64";
                case ARCHIVE_FIELD_UINT8:
                        return "uint8";
                case ARCHIVE_FIELD_UINT16:
                        return "uint16";
                case ARCHIVE_FIELD_UINT32:
                        return "uint32";
                case ARCHIVE_FIELD_UINT64:
                        return "uint64";
                case ARCHIVE_FIELD_FLOAT:
                        return "float32";
                case ARCHIVE_FIELD_STRING:
                        return "string64";
                case ARCHIVE_FIELD_BOOLEAN:
                        return "bool8";
                case ARCHIVE_FIELD_NULL:
                        return "void";
                case ARCHIVE_FIELD_OBJECT:
                        return "variable";
                default:
                        return "(unknown)";
        }
}

#if !defined(LOG_TRACE) || defined(NDEBUG)
#define TRACE(tag, msg, ...) { }
#else
#define TRACE(tag, msg, ...)                                                                                    \
{                                                                                                                      \
    char buffer[1024];                                                                                                 \
    sprintf(buffer, "--%d-- [TRACE   : %-10s] %s\n", getpid(), tag, msg);                                              \
    fprintf(stderr, buffer, __VA_ARGS__);                                                                              \
    fflush(stderr);                                                                                                    \
}
#endif

#if !defined(LOG_INFO) || defined(NDEBUG)
#define INFO(tag, msg, ...) { }
#else
#define INFO(tag, msg, ...)                                                                                     \
{                                                                                                                      \
    char buffer[1024];                                                                                                 \
    sprintf(buffer, "--%d-- [INFO    : %-10s] %s\n", getpid(), tag, msg);                                              \
    fprintf(stderr, buffer, __VA_ARGS__);                                                                              \
    fflush(stderr);                                                                                                    \
}
#endif

#if !defined(LOG_DEBUG) || defined(NDEBUG)
#define DEBUG(tag, msg, ...)                                                                                       \
{ }
#else
#define DEBUG(tag, msg, ...)                                                                                    \
{                                                                                                                      \
    char buffer[1024];                                                                                                 \
    sprintf(buffer, "--%d-- [DEBUG   : %-10s] %s\n", getpid(), tag, msg);                                              \
    fprintf(stderr, buffer, __VA_ARGS__);                                                                              \
    fflush(stderr);                                                                                                    \
}
#endif

#if !defined(LOG_WARN) || defined(NDEBUG)
#define WARN(tag, msg, ...) { }
#else
#define WARN(tag, msg, ...)                                                                                     \
    {                                                                                                                  \
        char buffer[1024];                                                                                             \
        sprintf(buffer, "--%d-- [WARNING: %-10s] %s\n", getpid(), tag, msg);                                           \
        fprintf(stderr, buffer, __VA_ARGS__);                                                                          \
        fflush(stderr);                                                                                                \
    }
#endif

#define CARBON_ARCHIVE_MAGIC                "MP/CARBON"
#define CARBON_ARCHIVE_VERSION               1

#define  MARKER_SYMBOL_OBJECT_BEGIN        '{'
#define  MARKER_SYMBOL_OBJECT_END          '}'
#define  MARKER_SYMBOL_PROP_NULL           'n'
#define  MARKER_SYMBOL_PROP_BOOLEAN        'b'
#define  MARKER_SYMBOL_PROP_INT8           'c'
#define  MARKER_SYMBOL_PROP_INT16          's'
#define  MARKER_SYMBOL_PROP_INT32          'i'
#define  MARKER_SYMBOL_PROP_INT64          'l'
#define  MARKER_SYMBOL_PROP_UINT8          'r'
#define  MARKER_SYMBOL_PROP_UINT16         'h'
#define  MARKER_SYMBOL_PROP_UINT32         'e'
#define  MARKER_SYMBOL_PROP_UINT64         'g'
#define  MARKER_SYMBOL_PROP_REAL           'f'
#define  MARKER_SYMBOL_PROP_TEXT           't'
#define  MARKER_SYMBOL_PROP_OBJECT         'o'
#define  MARKER_SYMBOL_PROP_NULL_ARRAY     'N'
#define  MARKER_SYMBOL_PROP_BOOLEAN_ARRAY  'B'
#define  MARKER_SYMBOL_PROP_INT8_ARRAY     'C'
#define  MARKER_SYMBOL_PROP_INT16_ARRAY    'S'
#define  MARKER_SYMBOL_PROP_INT32_ARRAY    'I'
#define  MARKER_SYMBOL_PROP_INT64_ARRAY    'L'
#define  MARKER_SYMBOL_PROP_UINT8_ARRAY    'R'
#define  MARKER_SYMBOL_PROP_UINT16_ARRAY   'H'
#define  MARKER_SYMBOL_PROP_UINT32_ARRAY   'E'
#define  MARKER_SYMBOL_PROP_UINT64_ARRAY   'G'
#define  MARKER_SYMBOL_PROP_REAL_ARRAY     'F'
#define  MARKER_SYMBOL_PROP_TEXT_ARRAY     'T'
#define  MARKER_SYMBOL_PROP_OBJECT_ARRAY   'O'
#define  MARKER_SYMBOL_EMBEDDED_STR_DIC    'D'
#define  MARKER_SYMBOL_EMBEDDED_STR        '-'
#define  MARKER_SYMBOL_COLUMN_GROUP        'X'
#define  MARKER_SYMBOL_COLUMN              'x'
#define  MARKER_SYMBOL_HUFFMAN_DIC_ENTRY   'd'
#define  MARKER_SYMBOL_RECORD_HEADER       'r'
#define  MARKER_SYMBOL_HASHTABLE_HEADER    '#'
#define  MARKER_SYMBOL_VEC_HEADER       '|'

#define DECLARE_AND_INIT(type, name)                                                                               \
        type name;                                                                                                     \
        ZERO_MEMORY(&name, sizeof(type));

#define ZERO_MEMORY(dst, len)                                                                                      \
    memset((char *) (dst), 0, (len));

#define CAST(type, name, src)                                                                                      \
      type name = (type) src

#define UNUSED(x)   (void)(x);

#define BUILT_IN(x)   MAYBE_UNUSED x

#define ofType(x) /** a convenience way to write types for generic containers; no effect than just a visual one */
#define ofMapping(x, y) /** a convenience way to write types for generic containers; no effect than just a visual one */

#define OPTIONAL_CALL(x, func, ...) if((x) && (x)->func) { (x)->func(__VA_ARGS__); }

#define JAK_MAX(a, b)                                                                                                  \
    ((b) > (a) ? (b) : (a))

#define JAK_MIN(a, b)                                                                                                  \
    ((a) < (b) ? (a) : (b))

#define CHECK_SUCCESS(x)                                                                                           \
{                                                                                                                      \
    if (UNLIKELY(!x)) {                                                                                                \
        return x;                                                                                                      \
    }                                                                                                                  \
}

#define SUCCESS_OR_JUMP(expr, label)                                                                               \
{                                                                                                                      \
    if (UNLIKELY(!expr)) {                                                                                             \
        goto label;                                                                                                    \
    }                                                                                                                  \
}

#define LIKELY(x)                                                                                                      \
    __builtin_expect((x), 1)
#define UNLIKELY(x)                                                                                                    \
    __builtin_expect((x), 0)

#define PREFETCH_READ(adr)                                                                                             \
    __builtin_prefetch(adr, 0, 3)

#define PREFETCH_WRITE(adr)                                                                                            \
    __builtin_prefetch(adr, 1, 3)

#define FORWARD_STRUCT_DECL(x) struct x;

#define BIT_NUM_OF(x)             (sizeof(x) * 8)
#define SET_BIT(n)                ( ((u32) 1) << (n) )
#define SET_BITS(x, mask)         ( x |=  (mask) )
#define UNSET_BITS(x, mask)       ( x &= ~(mask) )
#define ARE_BITS_SET(mask, bit)   (((bit) & mask ) == (bit))

#define ERROR_IF_NOT_IMPLEMENTED(err, x, func)                                                                         \
    OPTIONAL(x->func == NULL, ERROR(ERR_NOTIMPLEMENTED, NULL))

#define OPTIONAL(expr, stmt)                                                                                       \
    if (expr) { stmt; }

#define OPTIONAL_SET(x, y)                                                                                         \
     OPTIONAL(x, *x = y)

#define OPTIONAL_SET_OR_ELSE(x, y, stmt)                                                                           \
    if (x) {                                                                                                           \
        *x = y;                                                                                                        \
    } else { stmt; }

bool global_console_enable_output;

#define CONSOLE_OUTPUT_ON()                                                                                        \
    global_console_enable_output = true;

#define CONSOLE_OUTPUT_OFF()                                                                                       \
    global_console_enable_output = false;

#define CONSOLE_WRITE(file, msg, ...)                                                                              \
{                                                                                                                      \
    if (global_console_enable_output) {                                                                                   \
        pid_t pid = getpid();                                                                                          \
        char timeBuffer[2048];                                                                                         \
        char formatBuffer[2048];                                                                                       \
        time_t now = time (0);                                                                                         \
        fflush(file);                                                                                                  \
        strftime (timeBuffer, 2048, "%Y-%m-%d %H:%M:%S", localtime (&now));                                            \
        sprintf (formatBuffer, msg, __VA_ARGS__);                                                                      \
        fprintf(file, "[%d] %s   %-70s", pid, timeBuffer, formatBuffer);                                               \
        fflush(file);                                                                                                  \
    }                                                                                                                  \
}

#define CONSOLE_WRITE_ENDL(file)                                                                                   \
{                                                                                                                      \
    if (global_console_enable_output) {                                                                                   \
        fprintf(file, "\n");                                                                                           \
    }                                                                                                                  \
}

#define CONSOLE_WRITE_CONT(file, msg, ...)                                                                         \
{                                                                                                                      \
    if (global_console_enable_output) {                                                                                   \
        fprintf(file, msg, __VA_ARGS__);                                                                               \
    }                                                                                                                  \
}

#define CONSOLE_WRITELN(file, msg, ...)                                                                            \
{                                                                                                                      \
    if (global_console_enable_output) {                                                                                   \
        CONSOLE_WRITE(file, msg, __VA_ARGS__)                                                                      \
        CONSOLE_WRITE_ENDL(file)                                                                                   \
        fflush(file);                                                                                                  \
    }                                                                                                                  \
}

#endif
