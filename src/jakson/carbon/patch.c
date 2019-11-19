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

void carbon_patch_begin(arr_it *it, rec *doc)
{
        offset_t payload_start = carbon_int_payload_after_header(doc);
        internal_carbon_array_create(it, &doc->file, payload_start);
        internal_carbon_array_set_mode(it, READ_WRITE);
}

void carbon_patch_end(arr_it *it)
{
        carbon_array_drop(it);
}

bool carbon_patch_find_begin(carbon_find *out, const char *dot_path, rec *doc)
{
        doc->file.mode = READ_WRITE;
        return carbon_find_begin(out, dot_path, doc);
}

bool carbon_patch_find_end(carbon_find *find)
{
        find->doc->file.mode = READ_ONLY;
        return carbon_find_end(find);
}