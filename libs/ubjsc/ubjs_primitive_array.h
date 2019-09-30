/*
 * Copyright (c) 2016 Tomasz Sieprawski
 *
 * Permission is hereby granted, free of strge, to any person obtaining a copy
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
 *  \brief Array primitive.
 * \since 0.7
 */

#ifndef HAVE_UBJS_PRIMITIVE_ARRAY
#define HAVE_UBJS_PRIMITIVE_ARRAY

#ifdef __cplusplus
extern "C"
{
#endif

#include "ubjs_library.h"
#include "ubjs_primitives.h"


/*! Struct for array's iterator. */
struct ubjs_array_iterator;

/*! Struct for array's iterator. */
typedef struct ubjs_array_iterator ubjs_array_iterator;

/*!
 * Array marker.
 * \since 0.7
 */
UBJS_EXPORT extern ubjs_prmtv_marker ubjs_prmtv_array_marker;

/*!
 * Array end marker.
 * \since 0.7
 */
UBJS_EXPORT extern ubjs_prmtv_marker ubjs_prmtv_array_end_marker;

/*!
 * Array type marker.
 * \since 0.7
 */
UBJS_EXPORT extern ubjs_prmtv_marker ubjs_prmtv_array_type_marker;

/*!
 * Array count marker.
 * \since 0.7
 */
UBJS_EXPORT extern ubjs_prmtv_marker ubjs_prmtv_array_count_marker;

/*! \brief Returns an empty array.
 *
 * \param lib Library,
 * \param pthis Pointer to where put newly created array.
 * \return UR_OK if lib != 0 && 0 != pthis, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_array(ubjs_library *lib, ubjs_prmtv **pthis);

/*! \brief Returns an empty array, preoptimized (hopefully) for given length.
 *
 * \param lib Library,
 * \param length Length.
 * \param pthis Pointer to where put newly created array.
 * \return UR_OK if lib != 0 && 0 != pthis, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_array_with_length(ubjs_library *lib, unsigned int length,
    ubjs_prmtv **pthis);

/*! \brief Gets array's length.
 *
 * If UR_OK is returned, *plen is guavanteed to have new content.
 *
 * \param this Array.
 * \param plen Pointer to where put length.
 * \return UR_OK if lib != 0 && 0 != plen, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_array_get_length(ubjs_prmtv *this, unsigned int *plen);

/*! \brief Gets array's first item.
 *
 * If UR_OK is returned, *pitem is guaranteed to have new content.
 *
 * \param this Array.
 * \param pitem Pointer to where put item.
 * \return UR_OK if lib != 0 && 0 != pitem && array is non-empty, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_array_get_first(ubjs_prmtv *this, ubjs_prmtv **pitem);

/*! \brief Gets array's last item.
 *
 * If UR_OK is returned, *pitem is guaranteed to have new content.
 *
 * \param this Array.
 * \param pitem Pointer to where put item.
 * \return UR_OK if lib != 0 && 0 != pitem && array is non-empty, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_array_get_last(ubjs_prmtv *this, ubjs_prmtv **pitem);

/*! \brief Gets array's first item.
 *
 * If UR_OK is returned, *pitem is guaranteed to have new content.
 *
 * \param this Array.
 * \param pos Position of item.
 * \param pitem Pointer to where put item.
 * \return UR_OK if lib != 0 && 0 != pitem && array is non-empty && pos < length,
 * otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_array_get_at(ubjs_prmtv *this, unsigned int pos,
    ubjs_prmtv **pitem);

/*! \brief Adds item to array at its beginning.
 *
 * \param this Array.
 * \param item Item.
 * \return UR_OK if lib != 0 && 0 != item, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_array_add_first(ubjs_prmtv *this, ubjs_prmtv *item);

/*! \brief Adds item to array at its end.
 *
 * \param this Array.
 * \param item Item.
 * \return UR_OK if lib != 0 && 0 != item &&, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_array_add_last(ubjs_prmtv *this, ubjs_prmtv *item);

/*! \brief Adds item to array at given position.
 *
 * For pos == 0, this is equivalent to ubjs_prmtv_array_add_first().
 * For pos == length, this is equivalent to ubjs_prmtv_array_add_last().
 *
 * \param this Array.
 * \param pos Position.
 * \param item Item.
 * \return UR_OK if lib != 0 && 0 != item && pos <= length, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_array_add_at(ubjs_prmtv *this, unsigned int pos,
    ubjs_prmtv *item);

/*! \brief Deletes array's first item.
 *
 * \param this Array.
 * \return UR_OK if lib != 0 && array is non-empty, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_array_delete_first(ubjs_prmtv *this);

/*! \brief Deletes array's last item.
 *
 * \param this Array.
 * \return UR_OK if lib != 0 && array is non-empty, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_array_delete_last(ubjs_prmtv *this);

/*! \brief Deletes array's item from given position.
 *
 * \param this Array.
 * \param pos Position.
 * \return UR_OK if lib != 0 && array is non-empty && pos < length, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_array_delete_at(ubjs_prmtv *this, unsigned int pos);

/*! \brief Returns an iterator over the array.
 *
 * \param this Array.
 * \param pit Pointer to where put iterator.
 * \return UR_OK if this != 0 && pit != 0, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_array_iterate(ubjs_prmtv *this, ubjs_array_iterator **pit);

/*! \brief Tries to iterate to next item.
 *
 * This method will return UR_OK for length-times calls (if no delete occurs in the meantine)
 * and UR_ERROR for the very next one.
 *
 * \param this Array iterator.
 * \return UR_OK if this != 0 && successfully iterated to next item, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_array_iterator_next(ubjs_array_iterator *this);

/*! \brief Gets the currently iterated item.
 *
 * If previous ubjs_array_iterator_next() returned UR_OK, and UR_OK is returned here,
 * *pitem will be given content.
 *
 * This will not return UR_OK if ubjs_array_iterator_delete() occured.
 *
 * \param this Array iterator.
 * \param pitem Pointer to where put item.
 * \return UR_OK if this != 0 && pitem != 0, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_array_iterator_get(ubjs_array_iterator *this, ubjs_prmtv **pitem);

/*! \brief Deletes the currently iterated item.
 *
 * This will not return UR_OK if ubjs_array_iterator_delete() occured.
 *
 * \param this Array iterator.
 * \return UR_OK if this != 0, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_array_iterator_delete(ubjs_array_iterator *this);

/*! \brief Frees the array iterator.
 *
 * If UR_OK is returned, *pthis == 0.
 *
 * \param pthis Pointer to where lies iterator.
 * \return UR_OK if pthis != 0 && *pthis != 0, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_array_iterator_free(ubjs_array_iterator **pthis);

#ifdef __cplusplus
}
#endif

#endif
