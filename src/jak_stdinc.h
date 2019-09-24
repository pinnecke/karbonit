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

#ifndef JAK_COMMON_H
#define JAK_COMMON_H

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

#include "jak_forwdecl.h"

#ifndef __cplusplus

# include <stdatomic.h>

#else
# include <atomic>
# define _Atomic(X) std::atomic< X >
#endif

#ifdef __cplusplus
#define JAK_BEGIN_DECL  extern "C" {
#define JAK_END_DECL    }
#else
#define JAK_BEGIN_DECL
#define JAK_END_DECL
#endif

#define JAK_MALLOC(size)                \
({                                      \
        void *ptr = malloc(size);       \
        JAK_ZERO_MEMORY(ptr, size);     \
        ptr;                            \
})

#define JAK_QUERY_LIMIT_NONE -1
#define JAK_QUERY_LIMIT_1     1

#define JAK_ARRAY_LENGTH(x)                                                                                            \
    sizeof(x)/sizeof(x[0])

typedef uint64_t jak_offset_t;
typedef int64_t signed_offset_t;

typedef unsigned char u_char;

typedef enum jak_archive_field_type {
        JAK_FIELD_NULL = 0,
        JAK_FIELD_BOOLEAN = 1,
        JAK_FIELD_INT8 = 2,
        JAK_FIELD_INT16 = 3,
        JAK_FIELD_INT32 = 4,
        JAK_FIELD_INT64 = 5,
        JAK_FIELD_UINT8 = 6,
        JAK_FIELD_UINT16 = 7,
        JAK_FIELD_UINT32 = 8,
        JAK_FIELD_UINT64 = 9,
        JAK_FIELD_FLOAT = 10,
        JAK_FIELD_STRING = 11,
        JAK_FIELD_OBJECT = 12
} jak_archive_field_e;

typedef enum jak_access_mode_e {
        JAK_READ_WRITE,
        JAK_READ_ONLY
} jak_access_mode_e;

#define JAK_FUNC_UNUSED __attribute__((unused))

JAK_FUNC_UNUSED static const char *jak_basic_type_to_json_type_str(enum jak_archive_field_type t)
{
        switch (t) {
                case JAK_FIELD_INT8:
                case JAK_FIELD_INT16:
                case JAK_FIELD_INT32:
                case JAK_FIELD_INT64:
                case JAK_FIELD_UINT8:
                case JAK_FIELD_UINT16:
                case JAK_FIELD_UINT32:
                case JAK_FIELD_UINT64:
                        return "integer";
                case JAK_FIELD_FLOAT:
                        return "float";
                case JAK_FIELD_STRING:
                        return "string";
                case JAK_FIELD_BOOLEAN:
                        return "boolean";
                case JAK_FIELD_NULL:
                        return "null";
                case JAK_FIELD_OBJECT:
                        return "object";
                default:
                        return "(unknown)";
        }
}

JAK_FUNC_UNUSED static const char *jak_basic_type_to_system_type_str(enum jak_archive_field_type t)
{
        switch (t) {
                case JAK_FIELD_INT8:
                        return "int8";
                case JAK_FIELD_INT16:
                        return "int16";
                case JAK_FIELD_INT32:
                        return "int32";
                case JAK_FIELD_INT64:
                        return "int64";
                case JAK_FIELD_UINT8:
                        return "uint8";
                case JAK_FIELD_UINT16:
                        return "uint16";
                case JAK_FIELD_UINT32:
                        return "uint32";
                case JAK_FIELD_UINT64:
                        return "uint64";
                case JAK_FIELD_FLOAT:
                        return "float32";
                case JAK_FIELD_STRING:
                        return "string64";
                case JAK_FIELD_BOOLEAN:
                        return "bool8";
                case JAK_FIELD_NULL:
                        return "void";
                case JAK_FIELD_OBJECT:
                        return "variable";
                default:
                        return "(unknown)";
        }
}

#define JAK_NOT_IMPLEMENTED                                                                                            \
{                                                                                                                      \
    jak_error err;                                                                                                    \
    jak_error_init(&err);                                                                                                  \
    JAK_ERROR(&err, JAK_ERR_NOTIMPLEMENTED)                                                                                \
    jak_error_print_and_abort(&err);                                                                                       \
    return false;                                                                                                      \
};

#define __JAK_CPP_VA_ARGS_11(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, ...) a11
#define JAK_VA_ARGS_LENGTH(...) __JAK_CPP_VA_ARGS_11(_, ## __VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#ifndef NDEBUG
#define JAK_CHECK_TAG(is, expected)                                                                                 \
{                                                                                                                      \
    if (is != expected) {                                                                                              \
        JAK_ERROR_PRINT(JAK_ERR_ERRINTERNAL)                                                                     \
        return false;                                                                                                  \
    }                                                                                                                  \
}
#else
#define JAK_CHECK_TAG(is, expected) { }
#endif

#if !defined(JAK_LOG_TRACE) || defined(NDEBUG)
#define JAK_TRACE(tag, msg, ...) { }
#else
#define JAK_TRACE(tag, msg, ...)                                                                                    \
{                                                                                                                      \
    char buffer[1024];                                                                                                 \
    sprintf(buffer, "--%d-- [TRACE   : %-10s] %s\n", getpid(), tag, msg);                                              \
    fprintf(stderr, buffer, __VA_ARGS__);                                                                              \
    fflush(stderr);                                                                                                    \
}
#endif

#if !defined(JAK_LOG_INFO) || defined(NDEBUG)
#define JAK_INFO(tag, msg, ...) { }
#else
#define JAK_INFO(tag, msg, ...)                                                                                     \
{                                                                                                                      \
    char buffer[1024];                                                                                                 \
    sprintf(buffer, "--%d-- [INFO    : %-10s] %s\n", getpid(), tag, msg);                                              \
    fprintf(stderr, buffer, __VA_ARGS__);                                                                              \
    fflush(stderr);                                                                                                    \
}
#endif

#if !defined(JAK_LOG_DEBUG) || defined(NDEBUG)
#define JAK_DEBUG(tag, msg, ...)                                                                                       \
{ }
#else
#define JAK_DEBUG(tag, msg, ...)                                                                                    \
{                                                                                                                      \
    char buffer[1024];                                                                                                 \
    sprintf(buffer, "--%d-- [DEBUG   : %-10s] %s\n", getpid(), tag, msg);                                              \
    fprintf(stderr, buffer, __VA_ARGS__);                                                                              \
    fflush(stderr);                                                                                                    \
}
#endif

#if !defined(JAK_LOG_WARN) || defined(NDEBUG)
#define JAK_WARN(tag, msg, ...) { }
#else
#define JAK_WARN(tag, msg, ...)                                                                                     \
    {                                                                                                                  \
        char buffer[1024];                                                                                             \
        sprintf(buffer, "--%d-- [WARNING: %-10s] %s\n", getpid(), tag, msg);                                           \
        fprintf(stderr, buffer, __VA_ARGS__);                                                                          \
        fflush(stderr);                                                                                                \
    }
#endif

#define JAK_ASSERT(x) assert(x);

#define JAK_CARBON_ARCHIVE_MAGIC                "MP/CARBON"
#define JAK_CARBON_ARCHIVE_VERSION               1

#define  JAK_MARKER_SYMBOL_OBJECT_BEGIN        '{'
#define  JAK_MARKER_SYMBOL_OBJECT_END          '}'
#define  JAK_MARKER_SYMBOL_PROP_NULL           'n'
#define  JAK_MARKER_SYMBOL_PROP_BOOLEAN        'b'
#define  JAK_MARKER_SYMBOL_PROP_INT8           'c'
#define  JAK_MARKER_SYMBOL_PROP_INT16          's'
#define  JAK_MARKER_SYMBOL_PROP_INT32          'i'
#define  JAK_MARKER_SYMBOL_PROP_INT64          'l'
#define  JAK_MARKER_SYMBOL_PROP_UINT8          'r'
#define  JAK_MARKER_SYMBOL_PROP_UINT16         'h'
#define  JAK_MARKER_SYMBOL_PROP_UINT32         'e'
#define  JAK_MARKER_SYMBOL_PROP_UINT64         'g'
#define  JAK_MARKER_SYMBOL_PROP_REAL           'f'
#define  JAK_MARKER_SYMBOL_PROP_TEXT           't'
#define  JAK_MARKER_SYMBOL_PROP_OBJECT         'o'
#define  JAK_MARKER_SYMBOL_PROP_NULL_ARRAY     'N'
#define  JAK_MARKER_SYMBOL_PROP_BOOLEAN_ARRAY  'B'
#define  JAK_MARKER_SYMBOL_PROP_INT8_ARRAY     'C'
#define  JAK_MARKER_SYMBOL_PROP_INT16_ARRAY    'S'
#define  JAK_MARKER_SYMBOL_PROP_INT32_ARRAY    'I'
#define  JAK_MARKER_SYMBOL_PROP_INT64_ARRAY    'L'
#define  JAK_MARKER_SYMBOL_PROP_UINT8_ARRAY    'R'
#define  JAK_MARKER_SYMBOL_PROP_UINT16_ARRAY   'H'
#define  JAK_MARKER_SYMBOL_PROP_UINT32_ARRAY   'E'
#define  JAK_MARKER_SYMBOL_PROP_UINT64_ARRAY   'G'
#define  JAK_MARKER_SYMBOL_PROP_REAL_ARRAY     'F'
#define  JAK_MARKER_SYMBOL_PROP_TEXT_ARRAY     'T'
#define  JAK_MARKER_SYMBOL_PROP_OBJECT_ARRAY   'O'
#define  JAK_MARKER_SYMBOL_EMBEDDED_STR_DIC    'D'
#define  JAK_MARKER_SYMBOL_EMBEDDED_STR        '-'
#define  JAK_MARKER_SYMBOL_COLUMN_GROUP        'X'
#define  JAK_MARKER_SYMBOL_COLUMN              'x'
#define  JAK_MARKER_SYMBOL_HUFFMAN_DIC_ENTRY   'd'
#define  JAK_MARKER_SYMBOL_RECORD_HEADER       'r'
#define  JAK_MARKER_SYMBOL_HASHTABLE_HEADER    '#'
#define  JAK_MARKER_SYMBOL_VECTOR_HEADER       '|'

#define JAK_DECLARE_AND_INIT(type, name)                                                                               \
        type name;                                                                                                     \
        JAK_ZERO_MEMORY(&name, sizeof(type));

#define JAK_ZERO_MEMORY(dst, len)                                                                                      \
    memset((void *) dst, 0, len);

#define JAK_cast(type, name, src)                                                                                      \
      type name = (type) src

#define JAK_UNUSED(x)   (void)(x);

#define JAK_BUILT_IN(x)   JAK_FUNC_UNUSED x

#define ofType(x) /** a convenience way to write types for generic containers; no effect than just a visual one */
#define ofMapping(x, y) /** a convenience way to write types for generic containers; no effect than just a visual one */

#define JAK_OPTIONAL_CALL(x, func, ...) if((x) && (x)->func) { (x)->func(__VA_ARGS__); }

#define JAK_MAX(a, b)                                                                                                  \
    ((b) > (a) ? (b) : (a))

#define JAK_MIN(a, b)                                                                                                  \
    ((a) < (b) ? (a) : (b))

#define JAK_ERROR_IF_NULL(x)                                                                                               \
{                                                                                                                      \
    if (!(x)) {                                                                                                        \
        jak_error err;                                                                                                \
        jak_error_init(&err);                                                                                              \
        JAK_ERROR(&err, JAK_ERR_NULLPTR);                                                                                  \
        jak_error_print_to_stderr(&err);                                                                                   \
        return false;                                                                                                  \
    }                                                                                                                  \
}

#define JAK_CHECK_SUCCESS(x)                                                                                           \
{                                                                                                                      \
    if (JAK_UNLIKELY(!x)) {                                                                                                \
        return x;                                                                                                      \
    }                                                                                                                  \
}

#define JAK_SUCCESS_OR_JUMP(expr, label)                                                                               \
{                                                                                                                      \
    if (JAK_UNLIKELY(!expr)) {                                                                                             \
        goto label;                                                                                                    \
    }                                                                                                                  \
}

#define JAK_LIKELY(x)                                                                                                      \
    __builtin_expect((x), 1)
#define JAK_UNLIKELY(x)                                                                                                    \
    __builtin_expect((x), 0)

#define JAK_PREFETCH_READ(adr)                                                                                             \
    __builtin_prefetch(adr, 0, 3)

#define JAK_PREFETCH_WRITE(adr)                                                                                            \
    __builtin_prefetch(adr, 1, 3)

#define JAK_FORWARD_STRUCT_DECL(x) struct x;

#define JAK_BIT_NUM_OF(x)             (sizeof(x) * 8)
#define JAK_SET_BIT(n)                ( ((jak_u32) 1) << (n) )
#define JAK_SET_BITS(x, mask)         ( x |=  (mask) )
#define JAK_UNSET_BITS(x, mask)       ( x &= ~(mask) )
#define JAK_ARE_BITS_SET(mask, bit)   (((bit) & mask ) == (bit))

#define JAK_ERROR_IF_NOT_IMPLEMENTED(err, x, func)                                                                         \
    JAK_OPTIONAL(x->func == NULL, JAK_ERROR(err, JAK_ERR_NOTIMPLEMENTED))

#define JAK_OPTIONAL(expr, stmt)                                                                                       \
    if (expr) { stmt; }

#define JAK_OPTIONAL_SET(x, y)                                                                                         \
     JAK_OPTIONAL(x, *x = y)

#define JAK_OPTIONAL_SET_OR_ELSE(x, y, stmt)                                                                           \
    if (x) {                                                                                                           \
        *x = y;                                                                                                        \
    } else { stmt; }

bool jak_global_console_enable_output;

#define JAK_CONSOLE_OUTPUT_ON()                                                                                        \
    jak_global_console_enable_output = true;

#define JAK_CONSOLE_OUTPUT_OFF()                                                                                       \
    jak_global_console_enable_output = false;

#define JAK_CONSOLE_WRITE(file, msg, ...)                                                                              \
{                                                                                                                      \
    if (jak_global_console_enable_output) {                                                                                   \
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

#define JAK_CONSOLE_WRITE_ENDL(file)                                                                                   \
{                                                                                                                      \
    if (jak_global_console_enable_output) {                                                                                   \
        fprintf(file, "\n");                                                                                           \
    }                                                                                                                  \
}

#define JAK_CONSOLE_WRITE_CONT(file, msg, ...)                                                                         \
{                                                                                                                      \
    if (jak_global_console_enable_output) {                                                                                   \
        fprintf(file, msg, __VA_ARGS__);                                                                               \
    }                                                                                                                  \
}

#define JAK_CONSOLE_WRITELN(file, msg, ...)                                                                            \
{                                                                                                                      \
    if (jak_global_console_enable_output) {                                                                                   \
        JAK_CONSOLE_WRITE(file, msg, __VA_ARGS__)                                                                      \
        JAK_CONSOLE_WRITE_ENDL(file)                                                                                   \
        fflush(file);                                                                                                  \
    }                                                                                                                  \
}

#endif