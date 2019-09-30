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
 *  \brief Object primitive.
 * \since 0.7
 */

#ifndef HAVE_UBJS_PRIMITIVE_OBJECT
#define HAVE_UBJS_PRIMITIVE_OBJECT

#ifdef __cplusplus
extern "C"
{
#endif

#include "ubjs_library.h"
#include "ubjs_primitives.h"


/*! Struct for object's iterator. */
struct ubjs_object_iterator;

/*! Struct for object's iterator. */
typedef struct ubjs_object_iterator ubjs_object_iterator;

/*!
 * Object marker.
 * \since 0.7
 */
UBJS_EXPORT extern ubjs_prmtv_marker ubjs_prmtv_object_marker;

/*!
 * Object end marker.
 * \since 0.7
 */
UBJS_EXPORT extern ubjs_prmtv_marker ubjs_prmtv_object_end_marker;

/*!
 * Object type marker.
 * \since 0.7
 */
UBJS_EXPORT extern ubjs_prmtv_marker ubjs_prmtv_object_type_marker;

/*!
 * Object count marker.
 * \since 0.7
 */
UBJS_EXPORT extern ubjs_prmtv_marker ubjs_prmtv_object_count_marker;

/*! \brief Returns an empty object.
 *
 * \param lib Library,
 * \param pthis Pointer to where put newly created object.
 * \return UR_OK if lib != 0 && 0 != pthis, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_object(ubjs_library *lib, ubjs_prmtv **pthis);

/*! \brief Returns an empty object, preoptimized (hopefully) for given length.
 *
 * \param lib Library,
 * \param len Length.
 * \param pthis Pointer to where put newly created object.
 * \return UR_OK if lib != 0 && 0 != pthis, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_object_with_length(ubjs_library *lib, unsigned int len,
    ubjs_prmtv **pthis);

/*! \brief Gets object's length.
 *
 * If UR_OK is returned, *plen is guaranteed to have new content.
 *
 * \param this Object.
 * \param plen Pointer to where put length.
 * \return UR_OK if lib != 0 && 0 != plen, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_object_get_length(ubjs_prmtv *this, unsigned int *plen);

/*! \brief Gets the value for given key in the object.
 *
 * If UR_OK is returned, *pitem is guaranteed to have new content.
 *
 * \param this Object.
 * \param klen Key length.
 * \param key Key.
 * \param pitem Pointer to where put item.
 * \return UR_OK if lib != 0 && 0 != key && 0 != pitem && object is non-empty
 * && contains given key, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_object_get(ubjs_prmtv *this, unsigned int klen, char * key,
    ubjs_prmtv **pitem);

/*! \brief Adds (or updates) the value for given key in the object.
 *
 * If key already exists in the object, it is replaced - old value is freed.
 *
 * \param this Object.
 * \param klen Key length.
 * \param key Key.
 * \param item Item.
 * \return UR_OK if lib != 0 && 0 != key && 0 != item, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_object_set(ubjs_prmtv *this, unsigned int klen, char * key,
    ubjs_prmtv *item);

/*! \brief Deletes the pair for given key in the object.
 *
 * \param this Object.
 * \param klen Key length.
 * \param key Key.
 * \return UR_OK if lib != 0 && key != 0 && object is non-empty && contains given key,
 * otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_object_delete(ubjs_prmtv *this, unsigned int klen, char * key);

/*! \brief Returns an iterator over the object.
 *
 * \param this Object.
 * \param pit Pointer to where put iterator.
 * \return UR_OK if this != 0 && pit != 0, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_prmtv_object_iterate(ubjs_prmtv *this,
    ubjs_object_iterator **pit);

/*! \brief Tries to iterate to next item.
 *
 * This method will return UR_OK for length-times calls (if no delete occurs in the meantine)
 * and UR_ERROR for the very next one.
 *
 * \param this Object iterator.
 * \return UR_OK if this != 0 && successfully iterated to next item, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_object_iterator_next(ubjs_object_iterator *this);

/*! \brief Gets the currently iterated pair value.
 *
 * If previous ubjs_object_iterator_next() returned UR_OK, and UR_OK is returned here,
 * *pitem will be given content.
 *
 * This will not return UR_OK if ubjs_object_iterator_delete() occured.
 *
 * \param this Object iterator.
 * \param pitem Pointer to where put item.
 * \return UR_OK if this != 0 && pitem != 0, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_object_iterator_get_value(ubjs_object_iterator *this,
    ubjs_prmtv **pitem);

/*! \brief Gets the currently iterated pair key's length.
 *
 * If previous ubjs_object_iterator_next() returned UR_OK, and UR_OK is returned here,
 * *pklen will be given content.
 *
 * This will not return UR_OK if ubjs_object_iterator_delete() occured.
 *
 * \param this Object iterator.
 * \param pklen Pointer to where put key's length.
 * \return UR_OK if this != 0 && pklen != 0, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_object_iterator_get_key_length(ubjs_object_iterator *this,
    unsigned int *pklen);

/*! \brief Copies the currently iterater pair key to given string.
 *
 * If previous ubjs_object_iterator_next() returned UR_OK, and UR_OK is returned here,
 * key will be filled.
 *
 * This will not return UR_OK if ubjs_object_iterator_delete() occured.
 *
 * \param this Object iterator.
 * \param key Already allocated string.
 * Must be >= ubjs_object_iterator_get_key_length().
 * \return UR_OK if this != 0 && key != 0, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_object_iterator_copy_key(ubjs_object_iterator *this, char *key);

/*! \brief Deletes the currently iterated pair.
 *
 * This will not return UR_OK if ubjs_object_iterator_delete() occured.
 *
 * \param this Object iterator.
 * \return UR_OK if this != 0, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_object_iterator_delete(ubjs_object_iterator *this);

/*! \brief Frees the object iterator.
 *
 * If UR_OK is returned, *pthis == 0.
 *
 * \param pthis Pointer to where lies iterator.
 * \return UR_OK if pthis != 0 && *pthis != 0, otherwise UR_ERROR.
 */
UBJS_EXPORT ubjs_result ubjs_object_iterator_free(ubjs_object_iterator **pthis);

#ifdef __cplusplus
}
#endif

#endif
