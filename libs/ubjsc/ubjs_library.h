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
 *  \brief Library handle, allocation callbacks, glues.
 *
 * \since 0.2
 */

#ifndef HAVE_UBJS_LIBRARY
#define HAVE_UBJS_LIBRARY

#ifdef __cplusplus
extern "C"
{
#endif

#include "ubjs_common.h"

/*! \brief Allocation functor.
 *
 * This roughly follows the prototype for malloc().
 *  \param len Length.
 *  \return Allocated pointer.
 *
 * \since 0.4
 */
typedef void *(*ubjs_library_alloc_f)(unsigned long len);

/*! \brief Deallocation functor.
 *
 * This roughly follows the prototype for free().
 *  \param ptr Allocated pointer.
 *
 * \since 0.4
 */
typedef void (*ubjs_library_free_f)(void *ptr);

/*! \brief Library handle builder.
 *
 * \since 0.5
 */
typedef struct ubjs_library_builder ubjs_library_builder;

/*! \brief Library handle.
 *
 * \since 0.4
 */
typedef struct ubjs_library ubjs_library;

/*! \brief Glue to a array.
 *
 * \since 0.5
 */
typedef struct ubjs_glue_array ubjs_glue_array;

/*! \brief Builder for array glues.
 *
 * \since 0.5
 */
typedef struct ubjs_glue_array_builder ubjs_glue_array_builder;

/*! \brief Glue to a array iterator.
 *
 * \since 0.5
 */
typedef struct ubjs_glue_array_iterator ubjs_glue_array_iterator;

/*! \brief Glue to a dictionary.
 *
 * \since 0.5
 */
typedef struct ubjs_glue_dict ubjs_glue_dict;

/*! \brief Builder for dictionary glues.
 *
 * \since 0.5
 */
typedef struct ubjs_glue_dict_builder ubjs_glue_dict_builder;

/*! \brief Glue to a dictionary iterator.
 *
 * \since 0.5
 */
typedef struct ubjs_glue_dict_iterator ubjs_glue_dict_iterator;

/*! \brief Frees the value held in a glue.
 * Note that this is a generic callback, not related to actual
 * ubjs_prmtv_object()-s.
 * \param pvalue Pointer to value to be freed.
 * \since 0.5
 */
typedef void (*ubjs_glue_value_free)(void *pvalue);

/*! \brief Creates a new array glue builder.
 * \param lib Library handle.
 * \param pthis Pointer to where put new builder.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_builder_new_f)(ubjs_library *lib,
    ubjs_glue_array_builder **pthis);

/*! \brief Frees the array glue builder.
 *
 *  After this returns UR_OK, it is guaranteed that pthis points to 0.
 *
 * \param pthis Pointer to existing builder.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_builder_free_f)(ubjs_glue_array_builder **);

/*! \brief Sets the value free callback method.
 *
 * This is required.
 *
 * Default is stdlib's free().
 * \param this Builder.
 * \param value_free Callback.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_builder_set_value_free_f)(ubjs_glue_array_builder *this,
    ubjs_glue_value_free value_free);

/*! \brief Sets the predicted length for the array.
 *
 * If this is called, this gives the clue for implementation that n items will be added
 * to the array after creation. If this is called, it is called exactly 1 time.
 *
 * \warning This does not mean that the array's size will always remain fixed. Implementation
 * must be ready for any new item to be added, or existing to be removed - the size can change.
 *
 * If this is not called, implementation still must be prepared for any array size.
 *
 * Default is stdlib's free().
 * \param this Builder.
 * \param value_free Callback.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_builder_set_length_f)(ubjs_glue_array_builder *this,
    unsigned int length);

/*! \brief Callback that creates a new array glue based on what was passed to builder.
 * \param this Builder..
 * \param parr Pointer to where put new array glue.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_builder_build_f)(ubjs_glue_array_builder *this,
    ubjs_glue_array **parr);

/*! \brief Frees the array glue.
 *
 *  After this returns UR_OK, it is guaranteed that pthis points to 0.
 *
 * \param pthis Pointer to where glue is.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_free)(ubjs_glue_array **);

/*! \brief Gets the length of the array under a glue.
 *
 *  After this returns UR_OK, it is guaranteed that pvalue contains the length.
 *
 * \param this Glue.
 * \param pvalue Pointer to where put value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_get_length)(ubjs_glue_array *this, unsigned int *pvalue);

/*! \brief Gets the value of first item.
 *
 *  After this returns UR_OK, it is guaranteed that pvalue contains the value.
 *
 * \param this Glue.
 * \param pvalue Pointer to where put value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_get_first)(ubjs_glue_array *this, void **pvalue);

/*! \brief Gets the value of last item.
 *
 *  After this returns UR_OK, it is guaranteed that pvalue contains the value.
 *
 * \param this Glue.
 * \param pvalue Pointer to where put value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_get_last)(ubjs_glue_array *this, void **pvalue);

/*! \brief Gets the value of item under an index.
 *
 *  After this returns UR_OK, it is guaranteed that pvalue contains the value.
 *
 * \param this Glue.
 * \param index Index.
 * \param pvalue Pointer to where put value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_get_at)(ubjs_glue_array *this, unsigned int index,
    void **pvalue);

/*! \brief Add the value at beginning.
 *
 * \param this Glue.
 * \param value Value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_add_first)(ubjs_glue_array *this, void *value);

/*! \brief Add the value at end.
 *
 * \param this Glue.
 * \param value Value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_add_last)(ubjs_glue_array *this, void *value);

/*! \brief Add the value of item at an index.
 *
 * \param this Glue.
 * \param index Index.
 * \param value Value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_add_at)(ubjs_glue_array *this, unsigned int index,
    void *value);

/*! \brief Delete the first item.
 *
 * \param this Glue.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_delete_first)(ubjs_glue_array *this);

/*! \brief Delete the last item.
 *
 * \param this Glue.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_delete_last)(ubjs_glue_array *this);

/*! \brief Delete the item at index.
 *
 * \param this Glue.
 * \param index Index.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_delete_at)(ubjs_glue_array *this, unsigned int index);


/*! \brief Creates an iterator over glued array.
 *
 * \param this Glue.
 * \param piterator Pointer to where put iterator.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_iterate)(ubjs_glue_array *this,
    ubjs_glue_array_iterator **piterator);

/*! \brief Tries to iterate to next item.
 *
 * \param this Glue.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_iterator_next)(ubjs_glue_array_iterator *this);

/*! \brief Gets current item's value.
 *
 * \param this Glue.
 * \param pvalue Pointer to where put the value.
 * \return UR_OK if succedeed, and _delete() was not executed before on this item,
 * otherwise UR_ERROR.
 * \since 0.7
 */
typedef ubjs_result (*ubjs_glue_array_iterator_get)(ubjs_glue_array_iterator *this,
    void **pvalue);

/*! \brief Deletes item at current iterator.
 *
 * After this, you still need to _next() to advance to next item - the one that would be
 * behind the removed one.
 *
 * Aka this is how the array looks before _delete():
 *
 *     [PREV] -> [AT] -> [NEXT]
 *
 * And this is after:
 *
 *     [PREV] -> [NEXT]
 * \param this Glue.
 * \return UR_OK if succedeed, and _delete() was not executed before on this item,
 * otherwise UR_ERROR.
 * \since 0.7
 */
typedef ubjs_result (*ubjs_glue_array_iterator_delete)(ubjs_glue_array_iterator *this);

/*! \brief Frees the array iterator glue.
 *
 *  After this returns UR_OK, it is guaranteed that pthis points to 0.
 *
 * \param pthis Pointer to where glue is.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_array_iterator_free)(ubjs_glue_array_iterator **pthis);

/*! \brief Creates a new dictionary glue builder.
 * \param lib Library handle.
 * \param pthis Pointer to where put new builder.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_dict_builder_new_f)(ubjs_library *lib,
    ubjs_glue_dict_builder **pthis);

/*! \brief Frees the dictionary glue builder.
 *
 *  After this returns UR_OK, it is guaranteed that pthis points to 0.
 *
 * \param pthis Pointer to existing builder.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_dict_builder_free_f)(ubjs_glue_dict_builder **);

/*! \brief Sets the value free callback method.
 *
 * This is required.
 *
 * Default is stdlib's free().
 * \param this Builder.
 * \param value_free Callback.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_dict_builder_set_value_free_f)(ubjs_glue_dict_builder *this,
    ubjs_glue_value_free value_free);

/*! \brief Sets the predicted length for the dictionary.
 *
 * If this is called, this gives the clue for implementation that n items will be added
 * to the dictionary after creation. If this is called, it is called exactly 1 time.
 *
 * \warning This does not mean that the dictionary's size will always remain fixed. Implementation
 * must be ready for any new item to be added, or existing to be removed - the size can change.
 *
 * If this is not called, implementation still must be prepared for any dictionary size.
 *
 * Default is stdlib's free().
 * \param this Builder.
 * \param value_free Callback.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_dict_builder_set_length_f)(ubjs_glue_dict_builder *this,
    unsigned int length);

/*! \brief Callback that creates a new dictionary glue based on what was passed to builder.
 * \param this Builder..
 * \param pdict Pointer to where put new dictionary glue.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_dict_builder_build_f)(ubjs_glue_dict_builder *this,
    ubjs_glue_dict **pdict);

/*! \brief Frees the dictionary glue.
 *
 *  After this returns UR_OK, it is guaranteed that pthis points to 0.
 *
 * \param pthis Pointer to where glue is.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_dict_free)(ubjs_glue_dict **);

/*! \brief Gets the length of the dictionary under a glue
 *
 *  After this returns UR_OK, it is guaranteed that pvalue contains the length.
 *
 * \param this Glue.
 * \param pvalue Pointer to where put value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_dict_get_length)(ubjs_glue_dict *this, unsigned int *pvalue);

/*! \brief Gets the value under a key.
 *
 *  After this returns UR_OK, it is guaranteed that pvalue contains the value.
 *
 * \param this Glue.
 * \param key_length Key length.
 * \param key Key.
 * \param pvalue Pointer to where put value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_dict_get)(ubjs_glue_dict *this, unsigned int key_length,
    char *key, void **pvalue);

/*! \brief Sets the value under a key.
 *
 * \param this Glue.
 * \param key_length Key length.
 * \param key Key.
 * \param value Value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_dict_set)(ubjs_glue_dict *this, unsigned int key_length,
    char *key, void *value);

/*! \brief Deletes a key, with its value.
 *
 * \param this Glue.
 * \param key_length Key length.
 * \param key Key.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_dict_delete)(ubjs_glue_dict *this, unsigned int key_length,
    char *key);

/*! \brief Creates an iterator over glued dictionary.
 *
 * \param this Glue.
 * \param piterator Pointer to where put iterator.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_dict_iterate)(ubjs_glue_dict *this,
    ubjs_glue_dict_iterator **piterator);

/*! \brief Tries to iterate to next item.
 *
 * \param this Glue.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_dict_iterator_next)(ubjs_glue_dict_iterator *this);

/*! \brief Gets current key's length.
 *
 * _get_key_value() after _delete() will return UR_ERROR;
 *
 * \param this Glue.
 * \param plen Pointer to where put key's length.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.7
 */
typedef ubjs_result (*ubjs_glue_dict_iterator_get_key_length)(ubjs_glue_dict_iterator *this,
    unsigned int *plen);

/*! \brief Copies the key to specified array.
 *
 * _copy_key() after _delete() will return UR_ERROR;
 *
 * \param this Glue.
 * \param text Allocated array.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.7
 */
typedef ubjs_result (*ubjs_glue_dict_iterator_copy_key)(ubjs_glue_dict_iterator *this, char *text);

/*! \brief Gets current value.
 *
 * _get_value() after _delete() will return UR_ERROR;
 *
 * \param this Glue.
 * \param pvalue Pointer to where put the value.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.7
 */
typedef ubjs_result (*ubjs_glue_dict_iterator_get_value)(ubjs_glue_dict_iterator *this,
    void **pvalue);

/*! \brief Deletes current item.
  *
 * After this, you still need to _next() to advance to next item - the one that would be
 * behind the removed one.
 *
 * Aka this is how the array looks before _delete():
 *
 *     [PREV] -> [AT] -> [NEXT]
 *
 * And this is after:
 *
 *     [PREV] -> [NEXT]
 *
 * _delete() after _delete() will return UR_ERROR;
 * \param this Glue.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.7
 */
typedef ubjs_result (*ubjs_glue_dict_iterator_delete)(ubjs_glue_dict_iterator *this);

/*! \brief Frees the dictionary iterator glue.
 *
 *  After this returns UR_OK, it is guaranteed that pthis points to 0.
 *
 * \param pthis Pointer to where glue is.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.5
 */
typedef ubjs_result (*ubjs_glue_dict_iterator_free)(ubjs_glue_dict_iterator **pthis);

/*! \brief Builder for array glues.
 *
 * \since 0.5
 */
struct ubjs_glue_array_builder
{
    /*! Library. */
    ubjs_library *lib;
    /*! Userdata, possibly with actual implementation. */
    void *userdata;

    /*! Free callback */
    ubjs_glue_array_builder_free_f free_f;

    /*! Set value free callback */
    ubjs_glue_array_builder_set_value_free_f set_value_free_f;

    /*! Set length callback */
    ubjs_glue_array_builder_set_length_f set_length_f;

    /*! Build callback */
    ubjs_glue_array_builder_build_f build_f;
};

/*! \brief Glue to an array.
 *
 * \since 0.5
 */
struct ubjs_glue_array
{
    /*! Library. */
    ubjs_library *lib;

    /*! Userdata, possibly with actual implementation. */
    void *userdata;

    /*! Free callback */
    ubjs_glue_array_free free_f;

    /*! Get length callback. */
    ubjs_glue_array_get_length get_length_f;

    /*! Get first callback. */
    ubjs_glue_array_get_first get_first_f;

    /*! Get last callback. */
    ubjs_glue_array_get_last get_last_f;

    /*! Get at callback. */
    ubjs_glue_array_get_at get_at_f;

    /*! Add first callback. */
    ubjs_glue_array_add_first add_first_f;

    /*! Add last callback. */
    ubjs_glue_array_add_last add_last_f;

    /*! Add at callback. */
    ubjs_glue_array_add_at add_at_f;

    /*! Delete first callback. */
    ubjs_glue_array_delete_first delete_first_f;

    /*! Delete last callback. */
    ubjs_glue_array_delete_last delete_last_f;

    /*! Delete at callback. */
    ubjs_glue_array_delete_at delete_at_f;

    /*! Iterate callback. */
    ubjs_glue_array_iterate iterate_f;
};


/*! \brief Glue to array iterator.
 *
 * \since 0.5
 */
struct ubjs_glue_array_iterator
{
    /*! Parent glue. */
    ubjs_glue_array *array;

    /*! Userdata, probably with iterator implementation. */
    void *userdata;

    /*! Free callback. */
    ubjs_glue_array_iterator_free free_f;

    /*! Next calback. */
    ubjs_glue_array_iterator_next next_f;

    /*! Get callback. */
    ubjs_glue_array_iterator_get get_f;

    /*! Delete callback. */
    ubjs_glue_array_iterator_delete delete_f;
};

/*! \brief Builder for dictionary glues.
 *
 * \since 0.5
 */
struct ubjs_glue_dict_builder
{
    /*! Library. */
    ubjs_library *lib;
    /*! Userdata, possibly with actual implementation. */
    void *userdata;

    /*! Free callback */
    ubjs_glue_dict_builder_free_f free_f;

    /*! Set value free callback */
    ubjs_glue_dict_builder_set_value_free_f set_value_free_f;

    /*! Set length callback */
    ubjs_glue_dict_builder_set_length_f set_length_f;

    /*! Build callback */
    ubjs_glue_dict_builder_build_f build_f;
};

/*! \brief Glue to a dictionary.
 *
 * \since 0.5
 */
struct ubjs_glue_dict
{
    /*! Library. */
    ubjs_library *lib;

    /*! Userdata, possibly with actual implementation. */
    void *userdata;

    /*! Free callback */
    ubjs_glue_dict_free free_f;

    /*! Get length callback. */
    ubjs_glue_dict_get_length get_length_f;

    /*! Get callback. */
    ubjs_glue_dict_get get_f;

    /*! Set callback. */
    ubjs_glue_dict_set set_f;

    /*! Delete callback. */
    ubjs_glue_dict_delete delete_f;

    /*! Iterate callback. */
    ubjs_glue_dict_iterate iterate_f;
};

/*! \brief Glue to dictionary iterator.
 *
 * \since 0.5
 */
struct ubjs_glue_dict_iterator
{
    /*! Parent glue. */
    ubjs_glue_dict *object;

    /*! Userdata, probably with iterator implementation. */
    void *userdata;

    /*! Free callback. */
    ubjs_glue_dict_iterator_free free_f;

    /*! Next calback. */
    ubjs_glue_dict_iterator_next next_f;

    /*! Get key length callback. */
    ubjs_glue_dict_iterator_get_key_length get_key_length_f;

    /*! Copy callback. */
    ubjs_glue_dict_iterator_copy_key copy_key_f;

    /*! Get value callback. */
    ubjs_glue_dict_iterator_get_value get_value_f;

    /*! Delete callback. */
    ubjs_glue_dict_iterator_delete delete_f;
};

/*! \brief Library handle builder.
 *
 * \since 0.5
 */
struct ubjs_library_builder
{
    /*! \brief Alloc callback. */
    ubjs_library_alloc_f alloc_f;

    /*! \brief Free callback. */
    ubjs_library_free_f free_f;

    /*! \brief Glue array builder. */
    ubjs_glue_array_builder_new_f glue_array_builder;

    /*! \brief Glue dict builder. */
    ubjs_glue_dict_builder_new_f glue_dict_builder;
};

/*! \brief Initializes the library handle builder.
 *
 *  Required once to initialize library.
 *  \param this builder.
 *  \return UR_ERROR if universe exploded, otherwise UR_OK.
 *
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_library_builder_init(ubjs_library_builder *this);

/*! \brief Sets the allocation functor for to use for everything.
 *
 *  By default, ubjs uses stdlib's malloc().
 *
 *  \param this Builder.
 *  \param alloc_f Allocation functor.
 *  \return UR_ERROR if universe exploded, otherwise UR_OK.
 *
 * \since 0.5
 */
UBJS_EXPORT ubjs_result ubjs_library_builder_set_alloc_f(ubjs_library_builder *this,
    ubjs_library_alloc_f alloc_f);

/*! \brief Sets the deallocation functor for to use for everything.
 *
 *  By default, ubjs uses stdlib's free().
 *
 *  \param this Builder.
 *  \param free_f Deallocation functor.
 *  \return UR_ERROR if universe exploded, otherwise UR_OK.
 *
 * \since 0.5
 */
UBJS_EXPORT ubjs_result ubjs_library_builder_set_free_f(
    ubjs_library_builder *this,
    ubjs_library_free_f free_f);

/*! \brief Sets the array glue builder.
 *  This allows to select a different implementation of actual key-value
 *  store used in arrays.
 *
 *  By default, ubjs uses ubjs_glue_array_array_builder - built-in
 *  doubly-linked list.
 *
 *  \param this Builder.
 *  \param builder Glue array builder.
 *  \return UR_ERROR if universe exploded, otherwise UR_OK.
 *
 * \since 0.5
 */
UBJS_EXPORT ubjs_result ubjs_library_builder_set_glue_array_builder(
    ubjs_library_builder *this, ubjs_glue_array_builder_new_f builder);

/*! \brief Sets the dict glue builder.
 *  This allows to select a different implementation of actual key-value
 *  store used in dictionaries.
 *
 *  By default, ubjs uses ubjs_glue_dict_list_builder - built-in
 *  doubly-linked list.
 *
 *  \param this Builder.
 *  \param builder Glue dict builder.
 *  \return UR_ERROR if universe exploded, otherwise UR_OK.
 *
 * \since 0.5
 */
UBJS_EXPORT ubjs_result ubjs_library_builder_set_glue_dict_builder(
    ubjs_library_builder *this,
    ubjs_glue_dict_builder_new_f builder);

/*! \brief Builds the library and returns it.
 *
 *  After this returns UR_OK, it is guaranteed that plib points to newly
 *  initialized library handle.
 *
 *  \param this Builder.
 *  \param plib Pointer to where put newly created library handle.
 *  \return UR_ERROR if universe exploded, otherwise UR_OK.
 *
 * \since 0.5
 */
UBJS_EXPORT ubjs_result ubjs_library_builder_build(ubjs_library_builder *this,
    ubjs_library **plib);

/*! \brief Library handle.
 *
 * \since 0.5
 */
struct ubjs_library;

/*! \ Brief Gets library's alloc callback.
 * \param this Library.
 * \param palloc_f Pointer to where put alloc callback.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_library_get_alloc_f(ubjs_library *this,
    ubjs_library_alloc_f *palloc_f);

/*! \ Brief Gets library's free callback.
 * \param this Library.
 * \param pfree_f Pointer to where put free callback.
 * \return UR_OK if succedeed, otherwise UR_ERROR.
 * \since 0.6
 */
UBJS_EXPORT ubjs_result ubjs_library_get_free_f(ubjs_library *this, ubjs_library_free_f *pfree_f);

/*! \brief Deinitializes the library handle.
 *
 *  After this returns UR_OK, it is guaranteed that pthis points to already
 *  allocated library handle.
 *  \param pthis Pointer to library handle.
 *  \return UR_ERROR if any of pthis is not 0, otherwise UR_OK.
 *
 * \since 0.4
 */
UBJS_EXPORT ubjs_result ubjs_library_free(ubjs_library **pthis);

#ifdef __cplusplus
}
#endif

#endif
