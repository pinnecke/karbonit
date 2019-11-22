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

#include <jakson/carbon/traverse.h>
#include <jakson/carbon/revise.h>
#include <jakson/carbon/arr-it.h>
#include <jakson/carbon/col-it.h>
#include <jakson/carbon/obj-it.h>

void carbon_traverse_create(struct carbon_traverse *traverse, carbon_traverse_setup_t begin, carbon_traverse_clean_t end,
                            carbon_visit_record_t visit_record, carbon_visit_array_enter_t visit_array_begin,
                            carbon_visit_array_exit_t visit_array_end, carbon_visit_column_t visit_column,
                            carbon_visit_object_enter_t visit_object_begin, carbon_visit_object_exit_t visit_object_end,
                            enum carbon_traverse_tag tag, bool read_write)
{
        ZERO_MEMORY(traverse, sizeof(struct carbon_traverse))

        traverse->tag = tag;
        traverse->read_write = read_write;

        traverse->setup = begin ? begin : NULL;
        traverse->cleanup = end ? end : NULL;

        traverse->visit_record = visit_record ? visit_record : NULL;
        traverse->visit_array_begin = visit_array_begin ? visit_array_begin : NULL;
        traverse->visit_array_end = visit_array_end ? visit_array_end : NULL;
        traverse->visit_column = visit_column ? visit_column : NULL;
        traverse->visit_object_begin = visit_object_begin ? visit_object_begin : NULL;
        traverse->visit_object_end = visit_object_end ? visit_object_end : NULL;

        if (traverse->setup) {
                traverse->setup(&traverse->extra);
        }
        
}

void carbon_traverse_drop(struct carbon_traverse *traverse)
{
        if (traverse->cleanup) {
                traverse->cleanup(&traverse->extra);
        }
}

void carbon_traverse_carbon(rec *rev_out, struct carbon_traverse *traverse, rec *record)
{
        assert(traverse);
        assert(record);

        arr_it it;

        if (traverse->visit_record) {
                traverse->visit_record(&traverse->extra, record);
        }

        if (traverse->read_write) {
                assert(rev_out);
                
                rev context;
                revise_begin(&context, rev_out, record);
                revise_iterator_open(&it, &context);
                carbon_traverse_array(traverse, &it);
                revise_iterator_close(&it);
                revise_end(&context);
        } else {
                carbon_read_begin(&it, record);
                carbon_traverse_array(traverse, &it);
                carbon_read_end(&it);
        }

}

void carbon_traverse_array(struct carbon_traverse *traverse, arr_it *it)
{
        assert(traverse);
        assert(it);

        if (LIKELY(traverse->visit_array_begin != NULL)) {
                traverse->visit_array_begin(&traverse->extra, it);
        }
        if (LIKELY(traverse->visit_array_end != NULL)) {
                traverse->visit_array_end(&traverse->extra, it);
        }
}

void carbon_traverse_column(struct carbon_traverse *traverse, struct col_it *it)
{
        if (LIKELY(traverse->visit_column != NULL)) {
                traverse->visit_column(&traverse->extra, it);
        }
}

void carbon_traverse_object(struct carbon_traverse *traverse, struct obj_it *it)
{
        if (LIKELY(traverse->visit_object_begin != NULL)) {
                traverse->visit_object_begin(&traverse->extra, it);
        }
        if (LIKELY(traverse->visit_object_end != NULL)) {
                traverse->visit_object_end(&traverse->extra, it);
        }
}

void carbon_traverse_continue_array(struct carbon_traverse_extra *context, arr_it *it)
{
        carbon_traverse_array(context->parent, it);
}

void carbon_traverse_continue_column(struct carbon_traverse_extra *context, struct col_it *it)
{
        carbon_traverse_column(context->parent, it);
}

void carbon_traverse_continue_object(struct carbon_traverse_extra *context, struct obj_it *it)
{
        carbon_traverse_object(context->parent, it);
}