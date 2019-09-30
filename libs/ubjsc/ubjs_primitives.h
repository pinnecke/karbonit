/*
 * Copyright (c) 2016 Tomasz Sieprawski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
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
/*! \file
 *  \brief Common methods and interfaces over ubjson primitive types.
 *
 *  Common operations:
 *
 *  - construction.
 *
 *    For "valued" types, especially containers, ubjs_prmtv_<type> construct new structs.
 *
 *    For integer values, there is also a helper ubjs_prmtv_int that decides the type
 *    of final wrapper in runtime, depending on the value. This ensures that the least memory
 *    is taken for the wrapper.
 *
 *  - deconstruction.
 *
 *    Call ubjs_prmtv_free(&value). This works on all types, both valueless and "valued".
 *
 *  - checking type
 *
 *    ubjs_prmtv_get_marker returns the wrapper's type.
 *
 *    This applies also to ubjs_prmtv_is_int that returns UTRUE when the wrapper is of
 *    any int type.
 *
 *  - getters/setters
 *
 *    Most valued types has its own ubjs_prmtv_<type>_get and ubjs_prmtv_<type>_set methods.
 *
 *    String, array and object wrappers also have ubjs_prmtv_<type>_get_length methods for getting
 *    the number of items (or, in case of str, characters) in the value.
 *
 *  - manipulating container's content
 *
 *    Strings are mutable. Arrays have methods for inserting, getting and
 *    removing items, and same for objects.
 *
 *  - iterating over container
 *
 *    Array and object have their own iterators. Arrays are iterated from front (from 0
 *    to ubjs_array_get_length() - 1), objects are iterated in-order (from the lowest key
 *    to the highest).
 *
 * \since 0.7
 */

#ifndef HAVE_UBJS_PRIMITIVES
#define HAVE_UBJS_PRIMITIVES

#ifdef __cplusplus
extern "C"
{
#endif

#include "ubjs_library.h"

/*! Generic struct for all ubjson primitives. */
typedef struct ubjs_prmtv ubjs_prmtv;

/*!
 * \brief Generic struct describing primitive's type.
 * /since 0.7
 */
typedef struct ubjs_prmtv_marker ubjs_prmtv_marker;

/*! \brief Struct for array's iterator. */
typedef struct ubjs_array_iterator ubjs_array_iterator;

/*! \brief Struct for objects's iterator. */
typedef struct ubjs_object_iterator ubjs_object_iterator;

/*!
 * \brief Glue that glues primitive parser and parser itself.
 * /since 0.7
 */
typedef struct ubjs_prmtv_marker_parser_glue ubjs_prmtv_marker_parser_glue;
/*!
 * \brief Primitive parser processor.
 * /since 0.7
 */
typedef struct ubjs_prmtv_marker_parser_processor ubjs_prmtv_marker_parser_processor;

/*!
 * \brief Glue that glues primitive writer and writer itself.
 * /since 0.7
 */
typedef struct ubjs_prmtv_marker_writer_glue ubjs_prmtv_marker_writer_glue;

/*!
 * \brief Primitive writer.
 * /since 0.7
 */
typedef struct ubjs_prmtv_marker_writer ubjs_prmtv_marker_writer;

/*!
 * \brief Glue that glues primitive printer and printer itself.
 * /since 0.7
 */
typedef struct ubjs_prmtv_marker_printer_glue ubjs_prmtv_marker_printer_glue;

/*!
 * \brief Primitive printer.
 * /since 0.7
 */
typedef struct ubjs_prmtv_marker_printer ubjs_prmtv_marker_printer;

/*!
 * \brief Constructs a primitive using given int64 value.
 *
 * This is intended for primitives wrapping integer types. Primitive that defines
 * new_from_int64 must also define get_value_int64.
 *
 * Not every int64 value can be supported - method can accept only given range of values
 * and return UR_ERROR for every other.
 *
 * If this returns UR_OK, *pthis != 0.
 *
 * \param lib Library.
 * \param v Value.
 * \param pthis Pointer to where put newly created primitive.
 * \return UR_OK if lib != 0 && pthis != 0 && value lies within this primitive marker's
 * intended range.
 * /since 0.7
 */
typedef ubjs_result (*ubjs_prmtv_marker_new_from_int64_f)(ubjs_library *lib, int64_t v,
    ubjs_prmtv **pthis);

/*!
 * \brief Frees the primitive.
 *
 * If this returns UR_OK, *pthis == 0.
 *
 * \param pthis Pointer to where lies primitive.
 * \return UR_OK if pthis != 0 && *pthis != 0, else UR_ERROR.
 * /since 0.7
 */
typedef ubjs_result (*ubjs_prmtv_marker_free_f)(ubjs_prmtv **);

/*!
 * \brief Gets length of debug string produced by this primitive.
 *
 * If this returns UR_OK, *plen >= 0.
 * \param this Primitive.
 * \param plen Pointer to where put. length.
 * \return UR_OK if this != 0 && plen != 0, else UR_ERROR.
 * /since 0.7
 */
typedef ubjs_result (*ubjs_prmtv_marker_debug_string_get_length_f)(ubjs_prmtv *this,
    unsigned int *plen);
/*!
 * \brief Copies debug string produced by this primitive to given string.
 *
 * If this returns UR_OK, str is filled up to length byte.
 * \param this Primitive.
 * \param str Already allocated string.
 * \return UR_OK if this != 0 && str != 0, else UR_ERROR.
 * /since 0.7
 */
typedef ubjs_result (*ubjs_prmtv_marker_debug_string_copy_f)(ubjs_prmtv *this, char *str);

/*!
 * \brief Gets the int64 value from this primitive.
 * This is intended for primitives wrapping integer types.
 *
 * If this returns UR_OK, *pvalue is given value.
 *
 * \param this Primitive.
 * \param pvalue Pointer to where put value.
 * \return UR_OK if this != 0 && pvalue != 0, otherwise UR_ERROR.
 * /since 0.7
 */
typedef ubjs_result (*ubjs_prmtv_marker_get_value_int64_f)(ubjs_prmtv *this, int64_t *pvalue);

/*!
 * \brief Constructs a new parser processor using given glue.
 *
 * If this returns UR_OK, *pthis != 0.
 * \param lib Library,
 * \param glue Glue.
 * \param pthis Pointer to where put processor.
 * \return UR_OK if lib != 0 && glue != 0 && pthis != 0, otherwise UR_ERROR.
 * /since 0.7
 */
typedef ubjs_result (*ubjs_prmtv_marker_parser_processor_new_f)(ubjs_library *lib,
     ubjs_prmtv_marker_parser_glue *glue, ubjs_prmtv_marker_parser_processor **pthis);

/*!
 * \brief Frees the parser processor.
 *
 * If this returns UR_OK, *pthis == 0.
 * \param pthis Pointer to where is processor.
 * \return UR_OK if pthis != 0 && *pthis != 0, otherwise UR_ERROR.
 * /since 0.7
 */
typedef ubjs_result (*ubjs_prmtv_marker_parser_processor_free_f)(
    ubjs_prmtv_marker_parser_processor **pthis);

/*!
 * \brief Callback from the parser glue that the processor received a previously requested
 * child primitive.
 *
 * \param this Processor.
 * \param child Child primitive. Will not be ubjs_prmtv_free()-d by the glue.
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_parser_processor_got_child_f)
    (ubjs_prmtv_marker_parser_processor *this, ubjs_prmtv *child);

/*!
 * \brief Callback from the parser glue that the processor received a previously requested
 * marker.
 *
 * \param this Processor.
 * \param marker Marker.
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_parser_processor_got_marker_f)
    (ubjs_prmtv_marker_parser_processor *this, ubjs_prmtv_marker *marker);

/*!
 * \brief Callback from the parser glue that the processor received a control
 * and will receive read_byte() callbacks from now.
 *
 * \param this Processor.
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_parser_processor_got_control_f)
    (ubjs_prmtv_marker_parser_processor *this);
/*!
 * \brief Callback from the parser glue that the processor received a byte.
 * \param this Processor.
 * \param abyte Byte read.
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_parser_processor_read_byte_f)
    (ubjs_prmtv_marker_parser_processor *this, uint8_t abyte);

/*!
 * \param Request to the parser glue from the processor to return produced primitive
 * to its parent.
 * \param glue Glue.
 * \param prmtv Primitive.
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_parser_glue_return_control_f)(ubjs_prmtv_marker_parser_glue *this,
    void *prmtv);

/*!
 * \param Request to the parser glue from the processor to read a marker
 * and return it.
 * \param glue Glue.
 * \param markers Array of legal markers. Read marker will be from this array.
 * This should contain items from ubjs_library_get_markers().
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_parser_glue_want_marker_f)(ubjs_prmtv_marker_parser_glue *this,
    ubjs_glue_array *markers);

/*!
 * \param Request to the parser glue from the processor to read a primitive
 * and return it.
 * \param glue Glue.
 * \param marker Already known marker type of the primitive.
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_parser_glue_want_child_f)(ubjs_prmtv_marker_parser_glue *this,
    ubjs_prmtv_marker *marker);

/*!
 * \brief Debugging callback to the parser glue from the processor.
 * \param glue Glue.
 * \param len Length of the message.
 * \param msg Message.
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_parser_glue_debug_f)(ubjs_prmtv_marker_parser_glue *this,
    unsigned int len, char *msg);

/*!
 * \param Parser error callback to the parser glue from the processor.
 * \param glue Glue.
 * \param len Length of the message.
 * \param msg Message.
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_parser_glue_error_f)(ubjs_prmtv_marker_parser_glue *this,
    unsigned int len, char *msg);

/*!
 * \brief Constructs a new writer for given writer glue.
 *
 * The writer writes the primitive from the glue.
 *
 * If this returns UR_OK, *pthis != 0.
 * \param lib Library,
 * \param glue Glue.
 * \param pthis Pointer to where put writer.
 * \return UR_OK if lib != 0 && glue != 0 && glue->prmtv != 0 && pthis != 0, otherwise UR_ERROR.
 * /since 0.7
 */
typedef ubjs_result (*ubjs_prmtv_marker_writer_new_f)(ubjs_library *lib,
    ubjs_prmtv_marker_writer_glue *glue, ubjs_prmtv_marker_writer **pthis);

/*!
 * \brief Frees the writer.
 *
 * If this returns UR_OK, *pthis == 0.
 * \param pthis Pointer to where is writer.
 * \return UR_OK if pthis != 0 && *pthis != 0, otherwise UR_ERROR.
 * /since 0.7
 */
typedef ubjs_result (*ubjs_prmtv_marker_writer_free_f)(ubjs_prmtv_marker_writer **);

/*!
 * \brief Gets the length of data to be written by this writer
 *
 * If this returns UR_OK, *plen >= 0;
 * \param this Writer.
 * \param plen Pointer to where put length.
 * \param UR_OK if this != 0 && plen != 0, otherwise UR_ERROR.
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_writer_get_length_f)(ubjs_prmtv_marker_writer *this,
    unsigned int *plen);

/*!
 * \brief Writes the primitive data to given memory.
 *
 * The data length to be written must be same as got from ubjs_prmtv_marker_writer_get_length_f().
 * This should write data in range [0, length - 1].
 *
 * \param this Writer.
 * \param data Data memory to write to.
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_writer_do_f)(ubjs_prmtv_marker_writer *this, uint8_t *data);

/*!
 * \brief Debugging callback to the parser glue from the writer.
 * \param glue Glue.
 * \param len Length of the message.
 * \param msg Message.
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_writer_glue_debug_f)(ubjs_prmtv_marker_writer_glue *this,
    unsigned int len, char *msg);

/*!
 * \brief Constructs a new printer for given printer glue.
 *
 * Printer differs from the writer, that writer writes a UBJSON-compatible
 * bytestream, where printer prints human-readable according to the notation.
 *
 * The printer prints the primitive from the glue.
 *
 * If this returns UR_OK, *pthis != 0.
 * \param lib Library,
 * \param glue Glue.
 * \param pthis Pointer to where put printer.
 * \return UR_OK if lib != 0 && glue != 0 && glue->prmtv != 0 && pthis != 0, otherwise UR_ERROR.
 * /since 0.7
 */
typedef ubjs_result (*ubjs_prmtv_marker_printer_new_f)(ubjs_library *lib,
    ubjs_prmtv_marker_printer_glue *glue, ubjs_prmtv_marker_printer **pthis);

/*!
 * \brief Frees the printer.
 *
 * If this returns UR_OK, *pthis == 0.
 * \param pthis Pointer to where is printer.
 * \return UR_OK if pthis != 0 && *pthis != 0, otherwise UR_ERROR.
 * /since 0.7
 */
typedef ubjs_result (*ubjs_prmtv_marker_printer_free_f)(ubjs_prmtv_marker_printer **);

/*!
 * \brief Gets the length of data to be written by this printer
 *
 * If the primitive to be printed is a container, the returned length should respect
 * correct indentation of children.
 *
 * If this returns UR_OK, *plen >= 0;
 * \param this printer.
 * \param plen Pointer to where put length.
 * \param UR_OK if this != 0 && plen != 0, otherwise UR_ERROR.
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_printer_get_length_f)(ubjs_prmtv_marker_printer *,
    unsigned int *);

/*!
 * \brief Prints the primitive data to given memory.
 *
 * If the primitive to be printed is a container, the printed data should contain
 * correct indentation of children.
 *
 * The data length to be written must be same as got from ubjs_prmtv_marker_printer_get_length_f().
 * This should write data in range [0, length - 1].
 *
 * \param this printer.
 * \param data Data memory to write to.
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_printer_do_f)(ubjs_prmtv_marker_printer *, char *);

/*!
 * \brief Debugging callback to the parser glue from the printer.
 * \param glue Glue.
 * \param len Length of the message.
 * \param msg Message.
 * /since 0.7
 */
typedef void (*ubjs_prmtv_marker_printer_glue_debug_f)(ubjs_prmtv_marker_printer_glue *,
    unsigned int, char *);

/*! \brief Generic struct for all ubjson primitives. */
struct ubjs_prmtv
{
    /*! \brief Library. */
    ubjs_library *lib;

    /*! \brief Ntype. */
    ubjs_prmtv_marker *marker;
};

/*!
 * \brief Generic struct describing primitive's type.
 * /since 0.7
 */
struct ubjs_prmtv_marker
{
    /*! \brief Marker byte value. */
    uint8_t abyte;

    /*! \brief Primitive free callback. Required. */
    ubjs_prmtv_marker_free_f free_f;

    /*! \brief Primitive constructor basing on int64 value.
     * This is for all integer types.
     *
     * Optional.
     */
    ubjs_prmtv_marker_new_from_int64_f new_from_int64_f;

    /*! \brief Primitive value getter basing on int64 value.
     * This is for all integer types.
     *
     * Optional.
     */
    ubjs_prmtv_marker_get_value_int64_f get_value_int64_f;

    /*! \brief Primitive debug string length getter.
     *
     * Optional.
     */
    ubjs_prmtv_marker_debug_string_get_length_f debug_string_get_length_f;

    /*! \brief Primitive debug string getter.
     *
     * Optional.
     */
    ubjs_prmtv_marker_debug_string_copy_f debug_string_copy_f;

    /*! \brief Parser constructor.
     *
     * Required.
     */
    ubjs_prmtv_marker_parser_processor_new_f parser_processor_new_f;

    /*! \brief Parser destructor.
     *
     * Required.
     */
    ubjs_prmtv_marker_parser_processor_free_f parser_processor_free_f;

    /*! \brief Parser callback when got requested child primitive.
     *
     * Optional.
     */
    ubjs_prmtv_marker_parser_processor_got_child_f parser_processor_got_child_f;

    /*! \brief Parser callback when got requested marker.
     *
     * Optional.
     */
    ubjs_prmtv_marker_parser_processor_got_marker_f parser_processor_got_marker_f;

    /*! \brief Parser callback when got control.
     *
     * Optional.
     */
    ubjs_prmtv_marker_parser_processor_got_control_f parser_processor_got_control_f;

    /*! \brief Parser callback when read a byte.
     *
     * Optional.
     */
    ubjs_prmtv_marker_parser_processor_read_byte_f parser_processor_read_byte_f;

    /*! \brief Writer constructor.
     *
     * Required.
     */
    ubjs_prmtv_marker_writer_new_f writer_new_f;

    /*! \brief Writer destructor.
     *
     * Required.
     */
    ubjs_prmtv_marker_writer_free_f writer_free_f;

    /*! \brief Writer to-be-written length getter.
     *
     * Required.
     */
    ubjs_prmtv_marker_writer_get_length_f writer_get_length_f;

    /*! \brief Writer actual writer.
     *
     * Required.
     */
    ubjs_prmtv_marker_writer_do_f writer_do_f;

    /*! \brief Printer constructor.
     *
     * Required.
     */
    ubjs_prmtv_marker_printer_new_f printer_new_f;

    /*! \brief Printer destructor.
     *
     * Required.
     */
    ubjs_prmtv_marker_printer_free_f printer_free_f;

    /*! \brief Printer to-be-written length getter.
     *
     * Required.
     */
    ubjs_prmtv_marker_printer_get_length_f printer_get_length_f;

    /*! \brief Printer actual printer.
     *
     * Required.
     */
    ubjs_prmtv_marker_printer_do_f printer_do_f;
};

/*!
 * \brief Parser processor.
 * /since 0.7
 */
struct ubjs_prmtv_marker_parser_processor
{
    /*! \brief Library. */
    ubjs_library *lib;

    /*! \brief Primitive marker. */
    ubjs_prmtv_marker *marker;

    /*! \brief Human-readable processor name. */
    char *name;

    /*! \brief Glue. */
    ubjs_prmtv_marker_parser_glue *glue;
};

/*!
 * \brief Parser glue.
 * /since 0.7
 */
struct ubjs_prmtv_marker_parser_glue
{
    /*! \brief Userdata. */
    void *userdata;
    /*! \brief Parent processor, if any. */
    void *parent;

    /*! \brief Return control callback. */
    ubjs_prmtv_marker_parser_glue_return_control_f return_control_f;

    /*! \brief Want marker callback. */
    ubjs_prmtv_marker_parser_glue_want_marker_f want_marker_f;

    /*! \brief Want child callback. */
    ubjs_prmtv_marker_parser_glue_want_child_f want_child_f;

    /*! \brief Debug callback. */
    ubjs_prmtv_marker_parser_glue_debug_f debug_f;

    /*! \brief Error callback. */
    ubjs_prmtv_marker_parser_glue_error_f error_f;

    /*! \brief Container length limit, if any.
     * If >0, container parsers should error if about to produce primitive
     * that would have more elements than the limit.
     */
    unsigned int limit_container_length;

    /*! \brief String length limit, if any.
     * If >0, string parsers should error if about to produce primitive
     * that would have be longer than the limit.
     */
    unsigned int limit_string_length;

    /*! \brief Container recursion limit, if any.
     * If >0, container parsers should error if limit == recursion_level.
     */
    unsigned int limit_recursion_level;

    /*! \brief Current recursion level. >= 1. */
    unsigned int recursion_level;
};

/*!
 * \brief Writer.
 * /since 0.7
 */
struct ubjs_prmtv_marker_writer
{
    /*! Library. */
    ubjs_library *lib;

    /*! Primitive to be written. */
    ubjs_prmtv *prmtv;

    /*! Primitive marker. */
    ubjs_prmtv_marker *marker;

    /*! \brief Human-readable writer name. */
    char *name;

    /*! \brief Glue. */
    ubjs_prmtv_marker_writer_glue *glue;
};

/*!
 * \brief Writer glue.
 * /since 0.7
 */
struct ubjs_prmtv_marker_writer_glue
{
    /*! \brief Userdata. */
    void *userdata;

    /*! \brief Primitive to be written. */
    ubjs_prmtv *prmtv;

    /*! \brief Debug callback. */
    ubjs_prmtv_marker_writer_glue_debug_f debug_f;
};

/*!
 * /since 0.7
 */
struct ubjs_prmtv_marker_printer
{
    /*! Library. */
    ubjs_library *lib;

    /*! Primitive to be written. */
    ubjs_prmtv *prmtv;

    /*! Primitive marker. */
    ubjs_prmtv_marker *marker;

    /*! \brief Human-readable writer name. */
    char *name;

    /*! \brief Glue. */
    ubjs_prmtv_marker_printer_glue *glue;
};

/*!
 * /since 0.7
 */
struct ubjs_prmtv_marker_printer_glue
{
    /*! \brief Userdata. */
    void *userdata;
    /*! \brief Primitive to be printed. */
    ubjs_prmtv *prmtv;
    /*! \brief Current indentation level. >= 0. */
    unsigned int indent;

    /*! \brief Debug callback. */
    ubjs_prmtv_marker_printer_glue_debug_f debug_f;
};

/*! \brief Returns the best int primitive wrapping given value.
 *
 * After this returns UR_OK, *pthis points to a valid (u)int8/int16/int32/int64 primitive.
 * \param lib Library handle.
 * \param value The value.
 * \param pthis Pointer to where put newly created primitive.
 * \return UR_ERROR if any of lib/pthis is 0, else UR_OK.
 * \since 0.4
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_int(ubjs_library *lib, int64_t value, ubjs_prmtv **pthis);
/*! \brief Returns the best unsigned int primitive wrapping given value.
 *
 * After this returns UR_OK, *pthis points to a valid uint8/int16/int32/int64 primitive.
 * \param lib Library handle.
 * \param value The value.
 * \param pthis Pointer to where put newly created primitive.
 * \return UR_ERROR if lib is 0, else UR_OK.
 * \since 0.4
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_uint(ubjs_library *lib, int64_t value, ubjs_prmtv **pthis);
/*! \brief Gets the value of the integer primitive, regardless of its type.
 *
 * Recognized primitive types are (u)int8/int16/int32/int64.
 * The output value is upcasted to int64_t.
 * \param this Primitive.
 * \param pvalue Pointer to where set the value.
 * \return UR_ERROR if any of this/pvalue is 0, else UR_OK.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_int_get(ubjs_prmtv *this, int64_t *pvalue);

/*!
 * \brief Gets marker for given primitive.
 * \param this Primitive.
 * \param pmarker Pointer to where put marker.
 * \since 0.7
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_get_marker(ubjs_prmtv *this, ubjs_prmtv_marker **pmarker);

/*! \brief Calculates the length of would-be-serialized debug string for the primitive.
 * After this returns UR_OK, *this gets a dynamically allocated null-terminated string.
 * \param this Existing primitive.
 * \param plen Pointer to where put debug string's length.
 * \return UR_ERROR if any of this/pstr is 0, else UR_OK.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_debug_string_get_length(ubjs_prmtv *this, unsigned int *plen);

/*! \brief Serializes the primitive and copies the debug string to provided array.
 *
 * Target array must be preallocated. Before the call, you may want to
 * ubjs_prmtv_debug_string_get_length and allocate the target array.
 * \param this Existing primitive.
 * \param str Pointer to where put debug string.
 * \return UR_ERROR if any of this/str is 0, else UR_OK.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_debug_string_copy(ubjs_prmtv *this, char *str);

/*! \brief Frees the primitive.
 * After this returns UR_OK, *pthis is equal to 0.
 * \param pthis Pointer to existing primitive.
 * \return UR_ERROR if any of pthis / *pthis is 0, else UR_OK.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_free(ubjs_prmtv **pthis);

#ifdef __cplusplus
}
#endif

#endif
