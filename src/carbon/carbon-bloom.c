/**
 * Copyright 2018 Marcus Pinnecke
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

#include "carbon/carbon-bloom.h"

CARBON_EXPORT(bool)
carbon_bloom_create(carbon_bloom_t *filter, size_t size)
{
    return carbon_bitmap_create(filter, size);
}

CARBON_EXPORT(bool)
carbon_bloom_drop(carbon_bloom_t *filter)
{
    return carbon_bitmap_drop(filter);
}

CARBON_EXPORT(bool)
carbon_bloom_clear(carbon_bloom_t *filter)
{
    return carbon_bitmap_clear(filter);
}

size_t carbon_bloom_nbits(carbon_bloom_t *filter)
{
    return carbon_bitmap_nbits(filter);
}

unsigned carbon_bloom_nhashs()
{
    return 4;
}


