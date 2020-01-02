#include <stdio.h>
#include <jakson/jakson.h>
#include <libbson-1.0/bson.h>
#include "../../read_bench.h"

struct extra {
        bson_t doc;
        str_buf str;
        u64 array_idx;
};

static void create(struct read_bench_fn *self)
{
        self->extra = malloc(sizeof(struct extra));
}

static void drop(struct read_bench_fn *self)
{
        struct extra *extra = (struct extra *) self->extra;
        free(extra);
}

static void startup(struct read_bench_fn *self, void *arg)
{
        UNUSED(arg);
        struct extra *extra = (struct extra *) self->extra;
        bson_init(&extra->doc);
        str_buf_create(&extra->str);
        extra->array_idx = 0;
}

static void teardown(struct read_bench_fn *self, void *arg)
{
        UNUSED(arg);
        struct extra *extra = (struct extra *) self->extra;
        bson_destroy(&extra->doc);
        str_buf_drop(&extra->str);
}


static void insert_prepare(struct read_bench_fn *self, void *arg)
{
        UNUSED(self)
        UNUSED(arg)
}

static void insert_execute(struct read_bench_fn *self, i16 value, void *arg)
{
        UNUSED(self)
        UNUSED(arg)
        UNUSED(value)

        struct extra *extra = (struct extra *) self->extra;
        str_buf_clear(&extra->str);
        str_buf_add_i32(&extra->str, extra->array_idx++);
        bson_append_int32(&extra->doc, str_buf_cstr(&extra->str), -1, value);
}

static void insert_finalize(struct read_bench_fn *self, void *arg)
{
        UNUSED(arg)
        UNUSED(self)
        //struct extra *extra = (struct extra *) self->extra;
        //char *json = bson_as_json(&extra->doc, NULL);
        //printf(">>> '%s'\n", json);
        //bson_free(json);
}

static void bench_prepare(struct read_bench_fn *self, void *arg)
{
        UNUSED(self)
        UNUSED(arg)
}

static u64 bench_execute(struct read_bench_fn *self, unsigned milliseconds, u64 num_fields, void *arg)
{
        UNUSED(self);
        UNUSED(milliseconds);
        UNUSED(num_fields);
        UNUSED(arg);

        struct extra *extra = (struct extra *) self->extra;
        u64 num_ops = 0;

        u64 start = wallclock();

        while (wallclock() - start < milliseconds) {
                u64 rand_idx = (rand() % num_fields);
                u64 needle_idx = JAK_MAX(0, JAK_MIN((u64) num_fields - 1, rand_idx));

               // str_buf_clear(&extra->str);
                // str_buf_add_u64(&extra->str, needle_idx);

                bson_iter_t it;
                bson_iter_init (&it, &extra->doc);
                for (u64 i = 0; i < needle_idx; i++) {
                        bson_iter_next(&it);
                }
                
                //bool found = bson_iter_find(&it, str_buf_cstr(&extra->str));
                //assert(found);


                num_ops++;
        }

        return num_ops;
}

static void bench_finalize(struct read_bench_fn *self, void *arg)
{
        UNUSED(self)
        UNUSED(arg)
}

static read_bench_fn fn = {
        .driver_name = "libbson-1.16.0-pre",
        .create = create,
        .drop = drop,
        .startup = startup,
        .teardown = teardown,
        .insert_prepare = insert_prepare,
        .insert_execute = insert_execute,
        .insert_finalize = insert_finalize,
        .bench_prepare = bench_prepare,
        .bench_execute = bench_execute,
        .bench_finalize = bench_finalize
};

int main(void) {
        read_bench_run(&fn, NULL);
        return 0;
}

