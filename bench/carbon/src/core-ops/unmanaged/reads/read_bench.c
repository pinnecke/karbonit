#include <stdio.h>
#include <time.h>
#include <jakson/jakson.h>
#include "read_bench.h"

#define READ_BENCH_NAME "core-reads"
#define READ_BENCH_SINGLE_DURATION_MS 10000
#define READ_BENCH_NUM_SAMPLES 1
#define READ_BENCH_N_START 1
#define READ_BENCH_N_MAX 10000
#define READ_BENCH_N_STEP ((READ_BENCH_N_MAX - READ_BENCH_N_START) / 11)

void read_bench_run(read_bench_fn *fn, void *args)
{
        OPTIONAL_CALL(fn, create, fn);

        u64 duration = READ_BENCH_NUM_SAMPLES * ((READ_BENCH_N_MAX/READ_BENCH_N_STEP) * READ_BENCH_SINGLE_DURATION_MS / 1000);
        time_t then;
        then = time(NULL) + duration;
        fprintf(stderr, "** ETA: %s\n\n", asctime(localtime(&then)));

        printf("name,suite,sample,list_len,num_ops,ops/sec,timestamp\n");

        for (u64 n = READ_BENCH_N_START; n <= READ_BENCH_N_MAX; n += READ_BENCH_N_STEP) {
                for (u64 sample = 0; sample < READ_BENCH_NUM_SAMPLES; sample++) {
                        OPTIONAL_CALL(fn, startup, fn, args);
                        OPTIONAL_CALL(fn, insert_prepare, fn, args);
                        for (u64 i = 0; i < n; i++) {
                                i64 field_value = (rand() % 2 == 0 ? 1 : -1) * (rand() % INT32_MAX);
                                fn->insert_execute(fn, field_value, args);
                        }
                        OPTIONAL_CALL(fn, insert_finalize, fn, args);

                        OPTIONAL_CALL(fn, bench_prepare, fn, args);
                        u64 num_ops = fn->bench_execute(fn, READ_BENCH_SINGLE_DURATION_MS, n, args);
                        OPTIONAL_CALL(fn, bench_finalize, fn, args);

                        printf("%s,%s,%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",%f,%jd\n",
                                fn->driver_name, READ_BENCH_NAME, sample, n, num_ops,
                                (float) num_ops / (float) (READ_BENCH_SINGLE_DURATION_MS / 1000.0),
                                ((intmax_t)time(NULL)));
                        fflush(stdout);

                        OPTIONAL_CALL(fn, teardown, fn, args);
                }
        }

        fn->drop(fn);
}