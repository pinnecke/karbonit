/**
 * Copyright 2020 Marcus Pinnecke
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

#include <karbonit/carbon/traverse.h>

// ---------------------------------------------------------------------------------------------------------------------

#define TRAVERSE_IMPL_SETUP(fn_name)                                                                                   \
static inline void traverse_impl_##fn_name(traverser *t)                                                               \
{                                                                                                                      \
        assert(t);                                                                                                     \
        if (LIKELY(t->impl.fn.fn_name != NULL)) {                                                                      \
                t->impl.fn.fn_name(&t->impl.extra);                                                                    \
        }                                                                                                              \
}

TRAVERSE_IMPL_SETUP(create)
TRAVERSE_IMPL_SETUP(drop)

#define TRAVERSE_IMPL_CALL(fn_name, arg_name)                                                                          \
static inline path_policy_e traverse_impl_##fn_name(traverser *t)                                                      \
{                                                                                                                      \
        assert(t);                                                                                                     \
        if (LIKELY(t->impl.fn.fn_name != NULL)) {                                                                      \
                return t->impl.fn.fn_name((VEC_PEEK(&t->context, container_cntx))->container.arg_name,              \
                                          &t->info, &t->impl.extra);                                                   \
        } else {                                                                                                       \
                return PATH_EXPAND;                                                                                    \
        }                                                                                                              \
}

TRAVERSE_IMPL_CALL(visit_record, record)
TRAVERSE_IMPL_CALL(visit_array, array)
TRAVERSE_IMPL_CALL(visit_object, object)
TRAVERSE_IMPL_CALL(visit_field, item)
TRAVERSE_IMPL_CALL(visit_prop, prop)

// ---------------------------------------------------------------------------------------------------------------------

void traverser_create(traverser *traverse, const traverser_fn *fns, unsigned visit_ops)
{
    traverse->visit_ops = visit_ops;
    traverse->impl.fn = *fns;
    vec_create(&traverse->context, sizeof(container_cntx), 15);
    traverse_impl_create(traverse);
}

static void push_context(traverser *traverse, cntx_type_e type, const void *ptr)
{
        container_cntx cntx = {
                .type = type,
                .container.any = ptr
        };
        vec_push(&traverse->context, &cntx, 1);
}

static void pop_context(traverser *traverse)
{
        assert(!vec_is_empty(&traverse->context));
        vec_pop(&traverse->context);
}

void traverser_run_from_record(traverser *traverse, rec *record, void *arg)
{
        push_context(traverse, CNTX_RECORD, record);
        traverse->impl.extra.arg = arg;

        traverse->info.type = ON_ENTER;
        path_policy_e expand = (traverse->visit_ops & VISIT_CALL_RECORD_ENTER) ? traverse_impl_visit_record(traverse) :
                PATH_EXPAND;

        if (expand == PATH_EXPAND) {
                arr_it it;
                item *item;
                rec_read(&it, (rec *) record);
                while ((item = arr_it_next(&it))) {
                        traverser_run_from_item(traverse, item, arg);
                }
        }

        if (traverse->visit_ops & VISIT_CALL_RECORD_EXIT) {
                traverse->info.type = ON_EXIT;
                traverse_impl_visit_record(traverse);
        }

        pop_context(traverse);
}

void traverser_run_from_array(traverser *traverse, arr_it *it, void *arg)
{
        push_context(traverse, CNTX_ARRAY, it);
        traverse->impl.extra.arg = arg;

        traverse->info.type = ON_ENTER;
        path_policy_e expand = (traverse->visit_ops & VISIT_CALL_ARRAY_ENTER) ? traverse_impl_visit_array(traverse) :
                               PATH_EXPAND;

        if (expand == PATH_EXPAND) {
                item *item;
                while ((item = arr_it_next(it))) {
                        traverser_run_from_item(traverse, item, arg);
                }
        }

        if (traverse->visit_ops & VISIT_CALL_ARRAY_EXIT) {
                traverse->info.type = ON_EXIT;
                traverse_impl_visit_array(traverse);
        }
        pop_context(traverse);
}

void traverser_run_from_object(traverser *traverse, obj_it *it, void *arg)
{
        push_context(traverse, CNTX_OBJECT, it);
        traverse->impl.extra.arg = arg;

        traverse->info.type = ON_ENTER;
        path_policy_e expand = (traverse->visit_ops & VISIT_CALL_OBJECT_ENTER) ? traverse_impl_visit_object(traverse) :
                               PATH_EXPAND;

        if (expand == PATH_EXPAND) {
                prop *p;
                while ((p = obj_it_next(it))) {
                        traverser_run_from_prop(traverse, p, arg);
                }
        }

        if (traverse->visit_ops & VISIT_CALL_OBJECT_EXIT) {
                traverse->info.type = ON_EXIT;
                traverse_impl_visit_object(traverse);
        }

        pop_context(traverse);
}

void traverser_run_from_item(traverser *traverse, item *i, void *arg)
{
        push_context(traverse, CNTX_ITEM, i);
        traverse->impl.extra.arg = arg;

        assert(ITEM_IS_FIELD(i));

        traverse->info.type = ON_ENTER;
        path_policy_e expand = (traverse->visit_ops & VISIT_CALL_FIELD_ENTER) ? traverse_impl_visit_field(traverse) :
                PATH_EXPAND;

        if (expand == PATH_EXPAND) {
                if (ITEM_IS_ARRAY(i)) {
                        arr_it nested_it;
                        ITEM_GET_ARRAY(&nested_it, i);
                        traverser_run_from_array(traverse, &nested_it, arg);
                } else if (ITEM_IS_OBJECT(i)) {
                        obj_it nested_it;
                        ITEM_GET_OBJECT(&nested_it, i);
                        traverser_run_from_object(traverse, &nested_it, arg);
                }
        }

        if (traverse->visit_ops & VISIT_CALL_FIELD_EXIT) {
                traverse->info.type = ON_EXIT;
                traverse_impl_visit_field(traverse);
        }

        pop_context(traverse);
}

void traverser_run_from_prop(traverser *traverse, prop *p, void *arg)
{
        push_context(traverse, CNTX_PROP, p);
        traverse->impl.extra.arg = arg;

        traverse->info.type = ON_ENTER;
        path_policy_e expand = (traverse->visit_ops & VISIT_CALL_PROP_ENTER) ? traverse_impl_visit_prop(traverse) :
                               PATH_EXPAND;

        if (expand == PATH_EXPAND) {
                if (PROP_IS_ARRAY(p)) {
                        arr_it nested_it;
                        PROP_GET_ARRAY(&nested_it, p);
                        traverser_run_from_array(traverse, &nested_it, arg);
                } else if (PROP_IS_OBJECT(p)) {
                        obj_it nested_it;
                        PROP_GET_OBJECT(&nested_it, p);
                        traverser_run_from_object(traverse, &nested_it, arg);
                }
        }

        if (traverse->visit_ops & VISIT_CALL_PROP_EXIT) {
                traverse->info.type = ON_EXIT;
                traverse_impl_visit_prop(traverse);
        }

        pop_context(traverse);
}

void traverser_drop(traverser *traverse)
{
        traverse_impl_drop(traverse);
        vec_drop(&traverse->context);
}
