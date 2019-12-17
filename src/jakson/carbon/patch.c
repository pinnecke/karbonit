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

#include <jakson/carbon/patch.h>
#include <jakson/carbon/internal.h>
#include <jakson/carbon/find.h>

void patch_begin(arr_it *it, rec *doc)
{
        offset_t payload_start = INTERNAL_PAYLOAD_AFTER_HEADER(doc);
        internal_arr_it_create(it, &doc->file, payload_start);
        INTERNAL_ARR_IT_SET_MODE(it, READ_WRITE);
}

void patch_find_begin(find *out, const char *dot, rec *doc)
{
        doc->file.mode = READ_WRITE;
        find_begin_from_string(out, dot, doc);
}

void patch_find_end(find *find)
{
        find->doc->file.mode = READ_ONLY;
}