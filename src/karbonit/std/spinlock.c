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

#include <karbonit/std/spinlock.h>
#include <karbonit/utils/time.h>

#define SPINLOCK_TAG "spinlock"

bool spinlock_init(spinlock *spinlock)
{
        atomic_flag_clear(&spinlock->lock);
        memset(&spinlock->owner, 0, sizeof(pthread_t));
        return true;
}

bool spinlock_acquire(spinlock *spinlock)
{
        timestamp begin = wallclock();
        if (!pthread_equal(spinlock->owner, pthread_self())) {
                while (atomic_flag_test_and_set(&spinlock->lock)) {}
                /** remeber the thread that aquires this lock */
                spinlock->owner = pthread_self();
        }
        timestamp end = wallclock();
        float duration = (end - begin) / 1000.0f;
        if (duration > 0.01f) {
                WARN(SPINLOCK_TAG, "spin lock acquisition took exceptionally long: %f seconds", duration);
        }

        return true;
}

bool spinlock_release(spinlock *spinlock)
{
        atomic_flag_clear(&spinlock->lock);
        memset(&spinlock->owner, 0, sizeof(pthread_t));
        return true;
}