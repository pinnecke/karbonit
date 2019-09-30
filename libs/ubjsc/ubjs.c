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

#include "ubjs.h"

void ubjs_get_version(unsigned long *pversion)
{
    (*pversion) = UBJS_VERSION;
}

void ubjs_is_compatible(unsigned long runtime_version, ubjs_bool *pret)
{
    unsigned int compiled_major = (UBJS_VERSION >> 16) & 0xFF;
    unsigned int runtime_major = (runtime_version >> 16) & 0xFF;

    (*pret) = (runtime_major == compiled_major) ? UTRUE : UFALSE;

    /*
     * Because in 0.x API still can change.
     * Let's assume that all different 0.x minors are incompatible.
     */
    if (UTRUE == (*pret) && 0 == runtime_major)
    {
        unsigned int compiled_minor = (UBJS_VERSION >> 8) & 0xFF;
        unsigned int runtime_minor = (runtime_version >> 8) & 0xFF;
        (*pret) = (runtime_minor == compiled_minor) ? UTRUE : UFALSE;
    }
}
