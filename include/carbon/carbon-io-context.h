/**
 * Copyright 2019 Marcus Pinnecke
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

#ifndef CARBON_IO_CONTEXT_H
#define CARBON_IO_CONTEXT_H

#include "carbon-common.h"
#include "carbon-error.h"

CARBON_BEGIN_DECL

typedef struct carbon_archive carbon_archive_t; /* forwarded from 'carbon-archive.h' */

/**
 * Thread-safe I/O with an underlying archive file.
 * Locking is implemented using a spinlock. *
 */
typedef struct carbon_io_context carbon_io_context_t;


CARBON_EXPORT(bool)
carbon_io_context_create(carbon_io_context_t **context, carbon_err_t *err, const char *file_path);

CARBON_EXPORT(carbon_err_t *)
carbon_io_context_get_error(carbon_io_context_t *context);

CARBON_EXPORT(FILE *)
carbon_io_context_lock_and_access(carbon_io_context_t *context);

CARBON_EXPORT(bool)
carbon_io_context_unlock(carbon_io_context_t *context);

CARBON_EXPORT(bool)
carbon_io_context_drop(carbon_io_context_t *context);


CARBON_END_DECL

#endif