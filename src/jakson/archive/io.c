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

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/archive.h>
#include <jakson/std/spinlock.h>
#include <jakson/archive/io.h>

typedef struct archive_io_context {
        FILE *file;
        spinlock lock;
        offset_t last_pos;
} archive_io_context;

bool io_context_create(archive_io_context **context, const char *file_path)
{
        archive_io_context *result = MALLOC(sizeof(archive_io_context));

        if (!result) {
                error(ERR_MALLOCERR, NULL);
                return false;
        }

        spinlock_init(&result->lock);

        result->file = fopen(file_path, "r");

        if (!result->file) {
                error(ERR_FOPEN_FAILED, NULL);
                result->file = NULL;
                return false;
        } else {
                *context = result;
                return true;
        }
}

FILE *io_context_lock_and_access(archive_io_context *context)
{
        if (context) {
                spinlock_acquire(&context->lock);
                context->last_pos = ftell(context->file);
                return context->file;
        } else {
                error(ERR_NULLPTR, NULL);
                return NULL;
        }
}

bool io_context_unlock(archive_io_context *context)
{
        if (context) {
                fseek(context->file, context->last_pos, SEEK_SET);
                spinlock_release(&context->lock);
                return true;
        } else {
                error(ERR_NULLPTR, NULL);
                return false;
        }
}

bool io_context_drop(archive_io_context *context)
{
        OPTIONAL(context->file != NULL, fclose(context->file);
                context->file = NULL)
        free(context);
        return true;
}