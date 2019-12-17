#include <stdio.h>
#include <jakson/jakson.h>
#include "../../read_bench.h"

struct extra {
        rec record;
        rec_new record_new;
        insert *ins;
        dot dot_path;
        find find;
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

static void teardown(struct read_bench_fn *self, void *arg)
{
        UNUSED(arg);
        struct extra *extra = (struct extra *) self->extra;
        rec_drop(&extra->record);
}


static void insert_prepare(struct read_bench_fn *self, void *arg)
{
        UNUSED(self)
        UNUSED(arg)

        struct extra *extra = (struct extra *) self->extra;

        extra->ins = rec_create_begin(&extra->record_new, &extra->record, KEY_NOKEY, OPTIMIZE);
}

static void insert_execute(struct read_bench_fn *self, i64 value, void *arg)
{
        UNUSED(self)
        UNUSED(arg)
        UNUSED(value)

        struct extra *extra = (struct extra *) self->extra;
        insert_i64(extra->ins, value);
}

static void insert_finalize(struct read_bench_fn *self, void *arg)
{
        UNUSED(arg)

        struct extra *extra = (struct extra *) self->extra;

        rec_create_end(&extra->record_new);
}

static void bench_prepare(struct read_bench_fn *self, void *arg)
{
        UNUSED(self)
        UNUSED(arg)

        struct extra *extra = (struct extra *) self->extra;
        dot_create(&extra->dot_path);
}

static u64 bench_execute(struct read_bench_fn *self, unsigned milliseconds, u64 num_fields, void *arg)
{
        UNUSED(arg);

        struct extra *extra = (struct extra *) self->extra;
        u64 num_ops = 0;

        arr_it it;


        u64 start = wallclock();

        while (wallclock() - start < milliseconds) {
                u64 rand_idx = (rand() % num_fields);
                u64 needle_idx = JAK_MAX(0, JAK_MIN((u64) num_fields - 1, rand_idx));

                rec_read(&it, &extra->record);

                for (u64 i = 0; i < needle_idx; i++) {
                        arr_it_next(&it);
                }

              //  dot_clear(&extra->dot_path);
                //  dot_add_idx(&extra->dot_path, needle_idx);

                //find_from_dot(&extra->find, &extra->dot_path, &extra->record);
                //assert(find_has_result(&extra->find));

                //find_end(&extra->find);

                num_ops++;
        }



        return num_ops;
}

static void bench_finalize(struct read_bench_fn *self, void *arg)
{
        UNUSED(self)
        UNUSED(arg)

        struct extra *extra = (struct extra *) self->extra;
        dot_drop(&extra->dot_path);
}

static read_bench_fn fn = {
        .driver_name = "libcarbon-improved",
        .create = create,
        .drop = drop,
        .startup = NULL,
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

