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

#ifndef error_H
#define error_H

#include <stdint.h>

#include <ark-js/shared/common.h>
#include <ark-js/shared/types.h>

ARK_BEGIN_DECL

#define ARK_ERR_NOERR 0                    /** No error */
#define ARK_ERR_NULLPTR 1                  /** Null pointer detected */
#define ARK_ERR_NOTIMPL 2                  /** Function not implemented */
#define ARK_ERR_OUTOFBOUNDS 3              /** Index is out of bounds */
#define ARK_ERR_MALLOCERR 4                /** Memory allocation failed */
#define ARK_ERR_ILLEGALARG 5               /** Illegal arguments */
#define ARK_ERR_INTERNALERR 6              /** Internal error */
#define ARK_ERR_ILLEGALIMPL 7              /** Illegal implementation */
#define ARK_ERR_NOTFOUND 8                 /** Not found */
#define ARK_ERR_NIL 9                      /** Element not in list */
#define ARK_ERR_ARRAYOFARRAYS 10           /** Array index out of bounds */
#define ARK_ERR_ARRAYOFMIXEDTYPES 11       /** Illegal JSON array: mixed types */
#define ARK_ERR_FOPEN_FAILED 12            /** Reading from file failed */
#define ARK_ERR_IO 13                      /** I/O error */
#define ARK_ERR_FORMATVERERR 14            /** Unsupported archive format version */
#define ARK_ERR_CORRUPTED 15               /** Format is corrupted */
#define ARK_ERR_NOCARBONSTREAM 16          /** Stream is not a carbon archive */
#define ARK_ERR_NOBITMODE 17               /** Not in bit writing mode */
#define ARK_ERR_NOTIMPLEMENTED 18          /** Function is not yet implemented */
#define ARK_ERR_NOTYPE 19                  /** Unsupported type found */
#define ARK_ERR_NOCOMPRESSOR 20            /** Unsupported compressor strategy requested */
#define ARK_ERR_NOVALUESTR 21              /** No string representation for type available */
#define ARK_ERR_MARKERMAPPING 22           /** Marker type cannot be mapped to value type */
#define ARK_ERR_PARSETYPE 23               /** Parsing stopped; unknown data type requested */
#define ARK_ERR_NOJSONTOKEN 24             /** Unknown token during parsing JSON detected */
#define ARK_ERR_NOJSONNUMBERT 25           /** Unknown value type for number in JSON property */
#define ARK_ERR_NOARCHIVEFILE 26           /** Stream is not a valid archive file */
#define ARK_ERR_UNSUPFINDSTRAT 27          /** Unsupported strategy requested for key lookup */
#define ARK_ERR_ERRINTERNAL 28             /** Internal error */
#define ARK_ERR_HUFFERR 29                 /** No huffman code table entry found for character */
#define ARK_ERR_MEMSTATE 30                /** Memory file was opened as read-only but requested a modification */
#define ARK_ERR_JSONTYPE 31                /** Unable to import json file: unsupported type */
#define ARK_ERR_WRITEPROT 32               /** Mode set to read-only but modification was requested */
#define ARK_ERR_READOUTOFBOUNDS 33         /** Read outside of memory range bounds */
#define ARK_ERR_SLOTBROKEN 34              /** Slot management broken */
#define ARK_ERR_THREADOOOBJIDS 35          /** Thread run out of object ids: start another one */
#define ARK_ERR_JSONPARSEERR 36            /** JSON parsing error */
#define ARK_ERR_BULKCREATEFAILED 37        /** Document insertion bulk creation failed */
#define ARK_ERR_FOPENWRITE 38              /** File cannot be opened for writing */
#define ARK_ERR_WRITEARCHIVE 39            /** Archive cannot be serialized into file */
#define ARK_ERR_ARCHIVEOPEN 40             /** Archive cannot be deserialized form file */
#define ARK_ERR_FREAD_FAILED 41            /** Unable to read from file */
#define ARK_ERR_SCAN_FAILED 42             /** Unable to perform full scan in archive file */
#define ARK_ERR_DECOMPRESSFAILED 43        /** String decompression from archive failed */
#define ARK_ERR_ITERATORNOTCLOSED 44       /** Closing iterator failed */
#define ARK_ERR_HARDCOPYFAILED 45          /** Unable to construct a hard copy of the source object */
#define ARK_ERR_REALLOCERR 46              /** Memory reallocation failed */
#define ARK_ERR_PREDEVAL_FAILED 47         /** Predicate evaluation failed */
#define ARK_ERR_INITFAILED 48              /** Initialization failed */
#define ARK_ERR_DROPFAILED 49              /** Resource release failed: potentially a memory leak occurred */
#define ARK_ERR_OPPFAILED 50               /** Operation failed */
#define ARK_ERR_REHASH_NOROLLBACK 51       /** Rehashing hash table failed; rollback is not performed */
#define ARK_ERR_MEMFILEOPEN_FAILED 52      /** Unable to open memory file */
#define ARK_ERR_VITEROPEN_FAILED 53        /** Value iterator cannot be initialized */
#define ARK_ERR_MEMFILESKIP_FAILED 54      /** Memfile cannot skip desired amount of bytes */
#define ARK_ERR_MEMFILESEEK_FAILED 55      /** Unable to seek in memory file */
#define ARK_ERR_ITER_NOOBJ 56              /** Unable to get value: type is not non-array object */
#define ARK_ERR_ITER_NOBOOL 57             /** Unable to get value: type is not non-array boolean */
#define ARK_ERR_ITER_NOINT8 58             /** Unable to get value: type is not non-array int8 */
#define ARK_ERR_ITER_NOINT16 59            /** Unable to get value: type is not non-array int16 */
#define ARK_ERR_ITER_NOINT32 60            /** Unable to get value: type is not non-array int32 */
#define ARK_ERR_ITER_NOINT64 61            /** Unable to get value: type is not non-array int64 */
#define ARK_ERR_ITER_NOUINT8 62            /** Unable to get value: type is not non-array uint8 */
#define ARK_ERR_ITER_NOUINT16 63           /** Unable to get value: type is not non-array uint16 */
#define ARK_ERR_ITER_NOUINT32 64           /** Unable to get value: type is not non-array uint32 */
#define ARK_ERR_ITER_NOUINT64 65           /** Unable to get value: type is not non-array uint64 */
#define ARK_ERR_ITER_NONUMBER 66           /** Unable to get value: type is not non-array number */
#define ARK_ERR_ITER_NOSTRING 67           /** Unable to get value: type is not non-array string */
#define ARK_ERR_ITER_OBJECT_NEEDED 68      /** Illegal state: iteration over object issued, but collection found */
#define ARK_ERR_ITER_COLLECTION_NEEDED 69  /** Illegal state: iteration over collection issued, but object found */
#define ARK_ERR_TYPEMISMATCH 70            /** Type mismatch detected */
#define ARK_ERR_INDEXCORRUPTED_OFFSET 71   /** Index is corrupted: requested offset is outside file bounds */
#define ARK_ERR_TMP_FOPENWRITE 72          /** Temporary file cannot be opened for writing */
#define ARK_ERR_FWRITE_FAILED 73           /** Unable to write to file */
#define ARK_ERR_HASTABLE_DESERIALERR 74    /** Unable to deserialize hash table from file */
#define ARK_ERR_UNKNOWN_DIC_TYPE 75        /** Unknown string dictionary implementation requested */
#define ARK_ERR_STACK_OVERFLOW 76          /** Stack overflow */
#define ARK_ERR_STACK_UNDERFLOW 77         /** Stack underflow */
#define ARK_ERR_OUTDATED 78                /** Object was modified but is out of date */
#define ARK_ERR_NOTREADABLE 79             /** Object is currently being updated; no read allowed */
#define ARK_ERR_ILLEGALOP 80               /** Illegal operation */
#define ARK_ERR_BADTYPE 81                 /** Unsupported type */
#define ARK_ERR_UNSUPPCONTAINER 82         /** Unsupported container for data type */
#define ARK_ERR_INSERT_TOO_DANGEROUS 83    /** Adding integers with this function will perform an auto casting to
                                             * the smallest type required to store the integer value. Since you push
                                             * integers with this function into an column container that is bound
                                             * to a specific type, any insertion function call will fail once the
                                             * integer value requires a larger (or smaller) type than the fist value
                                             * added to the container. Use '*_insert_X' instead, where X is u8, u16,...
                                             * , u32 resp. i8, i16,..., i32. */
#define ARK_ERR_PARSE_DOT_EXPECTED 84       /** parsing error: dot ('.') expected */
#define ARK_ERR_PARSE_ENTRY_EXPECTED 85     /** parsing error: key name or array index expected */
#define ARK_ERR_PARSE_UNKNOWN_TOKEN 86      /** parsing error: unknown token */
#define ARK_ERR_DOT_PATH_PARSERR 87         /** dot-notated path could not be parsed */
#define ARK_ERR_ILLEGALSTATE 88             /** Illegal state */
#define ARK_ERR_UNSUPPORTEDTYPE 89          /** Unsupported data type */
#define ARK_ERR_FAILED 90                   /** Operation failed */
#define ARK_ERR_CLEANUP 91                  /** Cleanup operation failed; potentially a memory leak occurred */
#define ARK_ERR_DOT_PATH_COMPILEERR 92      /** dot-notated path could not be compiled */

static const char *const _err_str[] =
        {"No error", "Null pointer detected", "Function not implemented", "Index is out of bounds",
         "Memory allocation failed", "Illegal arguments", "Internal error", "Illegal implementation", "Not found",
         "Element not in list", "Array index out of bounds", "Illegal JSON array: mixed types",
         "Reading from file failed", "I/O error", "Unsupported archive format version", "Format is corrupted",
         "Stream is not a types archive", "Not in bit writing mode", "Function is not yet implemented",
         "Unsupported type found", "Unsupported pack strategy requested", "No string representation for type available",
         "Marker type cannot be mapped to value type", "Parsing stopped; unknown data type requested",
         "Unknown token during parsing JSON detected", "Unknown value type for number in JSON property",
         "Stream is not a valid archive file", "Unsupported strategy requested for key lookup", "Internal error",
         "No huffman code table entry found for character",
         "Memory file was opened as read-only but requested a modification",
         "Unable to import json file: unsupported type", "Mode set to read-only but modification was requested",
         "Read outside of memory range bounds", "Slot management broken",
         "Thread run out of object ids: start another one", "JSON parsing error",
         "Document insertion bulk creation failed", "File cannot be opened for writing",
         "Archive cannot be serialized into file", "Archive cannot be deserialized form file",
         "Unable to read from file", "Unable to perform full scan in archive file",
         "String decompression from archive failed", "Closing iterator failed",
         "Unable to construct a hard copy of the source object", "Memory reallocation failed",
         "Predicate evaluation failed", "Initialization failed",
         "Resource release failed: potentially a memory leak occurred", "Operation failed",
         "Rehashing hash table failed; rollback is not performed", "Unable to open memory file",
         "Value iterator cannot be initialized", "Memfile cannot skip desired amount of bytes",
         "Unable to seek in memory file", "Unable to get value: type is not non-array object",
         "Unable to get value: type is not non-array boolean", "Unable to get value: type is not non-array int8",
         "Unable to get value: type is not non-array int16", "Unable to get value: type is not non-array int32",
         "Unable to get value: type is not non-array int64", "Unable to get value: type is not non-array uint8",
         "Unable to get value: type is not non-array uint16", "Unable to get value: type is not non-array uint32",
         "Unable to get value: type is not non-array uint64", "Unable to get value: type is not non-array number",
         "Unable to get value: type is not non-array string",
         "Illegal state: iteration over object issued, but collection found",
         "Illegal state: iteration over collection issued, but object found", "Type mismatch detected",
         "Index is corrupted: requested offset is outside file bounds", "Temporary file cannot be opened for writing",
         "Unable to write to file", "Unable to deserialize hash table from file",
         "Unknown string dictionary implementation requested", "Stack overflow", "Stack underflow",
         "Object was modified but is out of date", "Object is currently being updated; no read allowed",
         "Illegal operation", "Unsupported type", "Unsupported container for data type",
         "Adding integers with this function will perform an auto casting to the smallest type required to store "
                 "the integer value. Since you push integers with this function into an column container that is bound "
                 "to a specific type, any insertion function call will fail once the integer value requires a larger "
                 "(or smaller) type than the fist value added to the container. Use '*_insert_X' instead, where X is "
                 "u8, u16,..., u32 resp. i8, i16,..., i32. ", "parsing error dot ('.') expected",
        "parsing error key name or array index expected", "parsing error: unknown token",
        "dot-notated path could not be parsed", "Illegal state", "Unsupported data type", "Operation failed",
        "Cleanup operation failed; potentially a memory leak occurred", "dot-notated path could not be compiled"};

#define ARK_ERRSTR_ILLEGAL_CODE "illegal error code"

static const int _nerr_str = ARK_ARRAY_LENGTH(_err_str);

struct err {
        int code;
        const char *file;
        u32 line;
        char *details;
};

ARK_EXPORT(bool) error_init(struct err *err);

ARK_EXPORT(bool) error_cpy(struct err *dst, const struct err *src);

ARK_EXPORT(bool) error_drop(struct err *err);

ARK_EXPORT(bool) error_set(struct err *err, int code, const char *file, u32 line);

ARK_EXPORT(bool) error_set_wdetails(struct err *err, int code, const char *file, u32 line, const char *details);

ARK_EXPORT(bool) error_set_no_abort(struct err *err, int code, const char *file, u32 line);

ARK_EXPORT(bool) error_set_wdetails_no_abort(struct err *err, int code, const char *file, u32 line, const char *details);

ARK_EXPORT(bool) error_str(const char **errstr, const char **file, u32 *line, bool *details, const char **detailsstr,
        const struct err *err);

ARK_EXPORT(bool) error_print_to_stderr(const struct err *err);

ARK_EXPORT(bool) error_print_and_abort(const struct err *err);

#define error_occurred(x)                   ((x)->err.code != ARK_ERR_NOERR)

#define success_else_return(expr, err, code, retval)                                                                   \
{                                                                                                                      \
        error_if(!(expr), err, code);                                                                                  \
        if (!(expr)) { return retval; }                                                                                \
}

#define success_else_null(expr, err)           success_else_return(expr, err, ARK_ERR_FAILED, NULL)
#define success_else_fail(expr, err)           success_else_return(expr, err, ARK_ERR_FAILED, false)



#define error(err, code)                     error_if (true, err, code)
#define error_no_abort(err, code)            error_if (true, err, code)
#define error_if(expr, err, code)            { if (expr) { error_set(err, code, __FILE__, __LINE__); } }
#define error_if_and_return(expr, err, code, retval) \
                                                    { if (expr) { error_set(err, code, __FILE__, __LINE__);            \
                                                                  return retval; } }

#define error_if_with_details(expr, err, code, msg)            { if (expr) { error_with_details(err, code, msg); } }
#define error_with_details(err, code, msg)                     error_set_wdetails(err, code, __FILE__, __LINE__, msg);

#define error_print(code)                    error_print_if(true, code)
#define print_error_and_die(code)            { error_print(code); abort(); }
#define error_print_and_die_if(expr, code)   { if(expr) { print_error_and_die(code) } }
#define error_print_if(expr, code)                                                                                     \
{                                                                                                                      \
    if (expr) {                                                                                                        \
        struct err err;                                                                                                \
        error_init(&err);                                                                                              \
        error(&err, code);                                                                                             \
        error_print_to_stderr(&err);                                                                                   \
    }                                                                                                                  \
}

#define ARK_DEFINE_ERROR_GETTER(type_tag_name)  ARK_DEFINE_GET_ERROR_FUNCTION(type_tag_name, struct type_tag_name, e)

#define ARK_DEFINE_GET_ERROR_FUNCTION(type_name, type, arg)                                                            \
ark_func_unused static bool                                                                                            \
type_name##_get_error(struct err *err, const type *arg)                                                                \
{                                                                                                                      \
    error_if_null(err)                                                                                                 \
    error_if_null(arg)                                                                                                 \
    error_cpy(err, &arg->err);                                                                                         \
    return true;                                                                                                       \
}

ARK_END_DECL

#endif