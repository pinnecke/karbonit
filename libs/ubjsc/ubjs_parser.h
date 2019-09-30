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
 *  \brief Streaing parser of bytestream to ubjson primitives.
 *
 * This file describes ubjs_parser structure, that parses incoming bytes and produces
 * ubjs_prmtv objects asynchronously.
 *
 * \since 0.2
 */

#ifndef HAVE_UBJS_PARSER
#define HAVE_UBJS_PARSER

#ifdef __cplusplus
extern "C"
{
#endif

#include "ubjs_library.h"
#include "ubjs_primitives.h"

/*! \brief Parser itself. */
struct ubjs_parser;

/*! \brief Parser builder. */
struct ubjs_parser_builder;

/*! \brief Holder for parser errors. */
struct ubjs_parser_error;

/*! \brief Parser itself. */
typedef struct ubjs_parser ubjs_parser;

/*! \brief Parser builder. */
typedef struct ubjs_parser_builder ubjs_parser_builder;

/*! \brief Holder for parser errors. */
typedef struct ubjs_parser_error ubjs_parser_error;

/*! \brief Callback to be executed just when parser parsed a first-citizen primitive.
 *
 * You should ubjs_prmtv_free(&primitive) by yourself.
 * \param userdata Userdata.
 * \param primitive Newly parsed primitive.
 * \since 0.6
 */
typedef void (*ubjs_parser_parsed_f)(void *userdata, ubjs_prmtv *primitive);
/*! \brief Callback to be executed just when parser issued a debug message.
 *
 * \param userdata Userdata.
 * \param len Length.
 * \param message Message.
 * \since 0.6
 */
typedef void (*ubjs_parser_debug_f)(void *userdata, unsigned int len, char *message);
/*! \brief Callback when encountered a parsing error.
 *
 * The current call to ubjs_parser_parse will return UR_ERROR. The parser is still functional,
 * but it remains at last valid state, before the error appeared.
 * \param userdata Userdata.
 * \param error Error.
 * \since 0.7
 */
typedef void (*ubjs_parser_error_f)(void *userdata, unsigned int len, char *message);
/*! \brief Callback when parser is about to be free-d.
 *
 * Here you can cleanup your userdata.
 * \param userdata Userdata.
 * \since 0.6
 */
typedef void (*ubjs_parser_free_f)(void *userdata);

/*! \brief Gets the error message's length.
 * \param this Error.
 * \param plength Pointer to where set the value.
 * \return UR_ERROR if any of this/presult is 0,  else UR_OK.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_error_get_message_length(ubjs_parser_error *this,
    unsigned int *plength);

/*! \brief Copies the error message's content to provided array.
 *
 * Target array must be preallocated. Before the call, you may want to
 * ubjs_parser_error_get_message_length and allocate the target array.
 * \param this Error.
 * \param text Target array.
 * \return UR_ERROR if any of this/text is 0,  else UR_OK.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_error_get_message_text(ubjs_parser_error *this, char *text);

/*! \brief Creates new builder.
 * \param lib Library.
 * \param pbuilder Pointer to where put builder.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_builder_new(ubjs_library *lib,
    ubjs_parser_builder **pbuilder);

/*! \brief Frees the builder.
 * \param pbuilder Pointer to where lies builder.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_builder_free(ubjs_parser_builder **pbuilder);

/*! \brief Sets parser's userdata.
 * \param builder Builder.
 * \param userdata Userdata.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_builder_set_userdata(ubjs_parser_builder *builder,
    void *userdata);

/*! \brief Sets parsed callback for parser's userdata.
 * \param builder Builder.
 * \param parsed_f Parsed callback.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_builder_set_parsed_f(ubjs_parser_builder *builder,
    ubjs_parser_parsed_f parsed_f);

/*! \brief Sets debug callback for parser's userdata.
 * If this is set, parser will callback with lots of
 * internal messages about how and what is being parsed.
 *
 * \param builder Builder.
 * \param debug_f Debug callback.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_builder_set_debug_f(ubjs_parser_builder *builder,
    ubjs_parser_debug_f debug_f);

/*! \brief Sets error callback for parser's userdata.
 * \param builder Builder.
 * \param error_f Error callback.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_builder_set_error_f(ubjs_parser_builder *builder,
    ubjs_parser_error_f error_f);

/*! \brief Sets free callback for parser's userdata.
 * \param builder Builder.
 * \param free_f Free callback.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_builder_set_free_f(ubjs_parser_builder *builder,
    ubjs_parser_free_f free_f);

/*! \brief Sets max number of bytes to process since last callback.
 * If this is 0, limit is effectively off.
 * Else parser errors will occur once you process this-th byte.
 *
 * This setting is global to whole parser.
 * \param builder Builder.
 * \param value Value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_builder_set_limit_bytes_since_last_callback(
    ubjs_parser_builder *builder, unsigned int value);

/*! \brief Max number of items in container.
 * This applies to arrays and objects.
 * If this is 0, limit is effectively off.
 * Else parser errors when:
 * - for unoptimized containers, n+1-th item is not an end marker,
 * - for optimized containers, when received length and it exceeds the limit.
 *
 * This setting is global to whole parser.
 * \param builder Builder.
 * \param value Value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_builder_set_limit_container_length(ubjs_parser_builder *builder,
    unsigned int value);

/*! \brief Max number of characters in string or HPN.
 * If this is 0, limit is effectively off.
 * Else parser errors when expected length exceeds the limit.
 *
 * This setting is global to whole parser.
 * \param builder Builder.
 * \param value Value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_builder_set_limit_string_length(ubjs_parser_builder *builder,
    unsigned int value);

/*! \brief Max recursion level for containers.
 * If this is 0, limit is effectively off.
 * Else parser errors when recursion level exceeds the limit.
 *
 * This setting is global to whole parser.
 * \param builder Builder.
 * \param value Value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_builder_set_limit_recursion_level(ubjs_parser_builder *builder,
    unsigned int value);

/*! \brief Whether to silently ignore no-op primitives.
 * If this is UTRUE:
 * - any top-level no-op does not result in callback,
 * - no-ops found during parsing of a container are not added to it's content,
 * - they also do not count in count-optimized containers.
 *   This means it is absolutely legal to have a 12-optimized array, and between elements there
 *   happen to be no-ops.
 *
 * This setting is global to whole parser.
 * \param builder Builder.
 * \param value Value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_builder_set_silently_ignore_toplevel_noops(
    ubjs_parser_builder *builder, ubjs_bool value);

/*! \brief Creates new parser.
 *
 * \param builder Builder.
 * \param pthis Pointer to where put newly created parser.
 * \return UR_ERROR if error occured, else UR_OK.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_builder_build(ubjs_parser_builder *builder,
    ubjs_parser **pthis);

/*! \brief Frees the parser.
 *
 * The call to context->free will occur here.
 * After this, *pthis will be equal to 0.
 * \param pthis Pointer to the parser.
 * \return UR_ERROR if pthis is 0, else UR_OK.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_free(ubjs_parser **pthis);

/*! \brief Gets userdata from the parser.
 *
 * \param this Parser.
 * \param puserdata Pointer to where put userdata.
 * \return UR_ERROR if any of this/pcontext is 0, else UR_OK.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_get_userdata(ubjs_parser *this, void **puserdata);

/*! \brief Parses the incoming data.
 *
 * This may produce the call (or calls) to context->parsed with just-parsed primitive,
 * or context->error, or none of them.
 *
 * In case there are many primitives parsed in single ubjs_parser_parse call, these many
 * calls to context->parsed will be issues for each new primitive.
 *
 * In case there is too little data to issue any callback, this method will not issue any callback.
 *
 * If len == 0, this method does nothing and simply returns UR_OK.
 * \param this Parser.
 * \param data Incoming data.
 * \param len Length of the data.
 * \return UR_ERROR if any of this/data is 0, else UR_OK.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_parser_parse(ubjs_parser *this, uint8_t *data, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif
