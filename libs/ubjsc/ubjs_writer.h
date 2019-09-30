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
 *  \brief Bytestream writer and pretty-printer of ubjson primitives.
 *
 * This file describes ubjs_writer structure, that writes ubjson primitives to bytestreams
 * and also pretty-prints'em.
 *
 * \since 0.2
 */
 
#ifndef HAVE_UBJS_WRITER
#define HAVE_UBJS_WRITER

#ifdef __cplusplus
extern "C"
{
#endif

#include "ubjs_library.h"
#include "ubjs_primitives.h"

/*! \brief Writer itself. */
struct ubjs_writer;

/*! \brief Writer builder. */
struct ubjs_writer_builder;

/*! \brief Writer itself. */
typedef struct ubjs_writer ubjs_writer;

/*! \brief Writer builder. */
typedef struct ubjs_writer_builder ubjs_writer_builder;

/*! \brief Callback when writer produced bytes out of a primitive.
 *
 * \param userdata Userdata.
 * \param data Outgoing data.
 * \param len Length of the data.
 * \since 0.6
 */
typedef void (*ubjs_writer_would_write_f)(void *userdata, uint8_t *data,
    unsigned int length);

/*! \brief Callback when writer produced pretty-printed version of a primitive.
 *
 * \param userdata Userdata.
 * \param data Outgoing printf()-ready data.
 * \param len Length of the data.
 * \since 0.6
 */
typedef void (*ubjs_writer_would_print_f)(void *userdata, char *data,
    unsigned int length);

/*! \brief Callback to be executed just when writer issued a debug message.
 *
 * \param userdata Userdata..
 * \param len Length.
 * \param message Message.
 * \since 0.6
 */
typedef void (*ubjs_writer_debug_f)(void *userdata, unsigned int len, char *message);

/*! \brief Callback when writer is about to be free-d.
 *
 * Here you can cleanup your userdata.
 * \param userdata Userdata.
 * \since 0.6
 */
typedef void (*ubjs_writer_free_f)(void *userdata);

/*! \brief Creates new writer builder.
 * \param lib Library.
 * \param pthis Pointer to where but new builder.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_writer_builder_new(ubjs_library *lib, ubjs_writer_builder **pthis);
/*! \brief Frees the writer builder.
 * \param pthis Pointer to where lies builder.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_writer_builder_free(ubjs_writer_builder **pthis);
/*! \brief Sets the userdata for builder.
 * \param this Builder.
 * \param userdata Userdata.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_writer_builder_set_userdata(ubjs_writer_builder *this,
    void *userdata);
/*! \brief Sets the would-write callback for builder.
 * \param this Builder.
 * \param would_write_f Callback.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_writer_builder_set_would_write_f(ubjs_writer_builder *this,
    ubjs_writer_would_write_f would_write_f);
/*! \brief Sets the would-print callback for builder.
 * \param this Builder.
 * \param would_print_f Callback.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_writer_builder_set_would_print_f(ubjs_writer_builder *this,
    ubjs_writer_would_print_f would_print_f);
/*! \brief Sets whether to free primitives early.
 *
 * If this is on, primitives will be free()-d as soon as they are written or printed.
 * This also means applies to containers' items - first container's items are free()-d,
 * then the container itself.
 *
 * Default is UFALSE.
 *
 * \param this Builder.
 * \param free_primitives_early Whether to free primitives early.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.7
 */
UBJS_EXPORT ubjs_result ubjs_writer_builder_set_free_primitives_early(ubjs_writer_builder *this,
    ubjs_bool free_primitives_early);
/*! \brief Sets the debug callback for builder.
 * \param this Builder.
 * \param debug_f Callback.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_writer_builder_set_debug_f(ubjs_writer_builder *this,
    ubjs_writer_debug_f debug_f);
/*! \brief Sets the userdata free callback for builder.
 * \param this Builder.
 * \param free_f callback.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_writer_builder_set_free_f(ubjs_writer_builder *this,
    ubjs_writer_free_f free_f);

/*! \brief Builds new writer.
 *
 * \param builder Builder.
 * \param pthis Pointer to where put newly created writer.
 * \return UR_ERROR if any of lib/pthis/context/context->free
 * is 0, else UR_OK.
 *
 * \since 0.4
 */
UBJS_EXPORT ubjs_result ubjs_writer_builder_build(ubjs_writer_builder *builder,
    ubjs_writer **pthis);

/*! \brief Frees the writer.
 *
 * The call to context->free will occur here.
 * After this, *pthis will be equal to 0.
 * \param pthis Pointer to the writer.
 * \return UR_ERROR if pthis is 0, else UR_OK.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_writer_free(ubjs_writer **pthis);

/*! \brief Gets userdata from the writer.
 *
 * \param this Writer.
 * \param puserdata Pointer to where put userdata.
 * \return UR_ERROR if any of this/puserdata is 0, else UR_OK.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_writer_get_userdata(ubjs_writer *this, void **puserdata);

/*! \brief Writes a bytestream out of a primitive.
 *
 * This produces the callback to context->would_write with fresh bytestream.
 * \param this Writer.
 * \param object Primitive.
 * \return UR_ERROR if any of this/object is 0, else UR_OK.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_writer_write(ubjs_writer *this, ubjs_prmtv *object);
/*! \brief Pretty-prints out of a primitive.
 *
 * This produces the callback to context->would_print with fresh pretty-printout.
 * \param this Writer.
 * \param object Primitive.
 * \return UR_ERROR if any of this/object is 0, else UR_OK.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_writer_print(ubjs_writer *this, ubjs_prmtv *object);

#ifdef __cplusplus
}
#endif

#endif
