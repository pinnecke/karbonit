/**
 * Copyright (c) 2020 Marcus Pinnecke
 */

#ifndef HAD_BENCH_READ_H
#define HAD_BENCH_READ_H

#include <jakson/types.h>

typedef struct read_bench_fn
{
        const char *driver_name;
        void (*create)(struct read_bench_fn *self);
        void (*drop)(struct read_bench_fn *self);

        void (*startup)(struct read_bench_fn *self, void *arg);
        void (*teardown)(struct read_bench_fn *self, void *arg);

        void (*insert_prepare)(struct read_bench_fn *self, void *arg);
        void (*insert_execute)(struct read_bench_fn *self, i64 value, void *arg);
        void (*insert_finalize)(struct read_bench_fn *self, void *arg);

        void (*bench_prepare)(struct read_bench_fn *self, void *arg);
        u64 (*bench_execute)(struct read_bench_fn *self, unsigned milliseconds, u64 num_fields, void *arg);
        void (*bench_finalize)(struct read_bench_fn *self, void *arg);

        void *extra;
} read_bench_fn;

void read_bench_run(read_bench_fn *fn, void *args);



#endif
