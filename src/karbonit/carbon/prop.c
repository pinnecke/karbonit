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

#include <karbonit/carbon/prop.h>
#include <karbonit/carbon/string-field.h>
#include <karbonit/carbon/field.h>
#include <karbonit/carbon/obj-it.h>

bool internal_prop_create(prop *dst, obj_it *parent)
{
        dst->parent = parent;
        dst->idx = parent->pos;
        dst->key.str = parent->field.key.name;
        dst->key.len = parent->field.key.name_len;
        internal_item_create_from_object(&dst->value, parent);
        return true;
}

u64 internal_prop_size(memfile *file)
{
        offset_t prop_start = MEMFILE_SAVE_POSITION(file);
        string_field_nomarker_skip(file);
        carbon_field_skip(file);
        offset_t prop_end = MEMFILE_TELL(file);
        MEMFILE_RESTORE_POSITION(file);
        return prop_end - prop_start;
}