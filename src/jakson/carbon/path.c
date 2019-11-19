/**
 * Columnar Binary JSON -- Copyright 2019 Marcus Pinnecke
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

#include <jakson/carbon/path.h>
#include <jakson/carbon/find.h>
#include <jakson/carbon/revise.h>

static inline carbon_path_status_e traverse_column(carbon_path_evaluator *state,
                                                      const carbon_dot_path *path, u32 current_path_pos,
                                                      col_it *it);

static inline carbon_path_status_e traverse_array(carbon_path_evaluator *state,
                                                     const carbon_dot_path *path, u32 current_path_pos,
                                                     arr_it *it, bool is_record);

void carbon_path_evaluator_begin(carbon_path_evaluator *eval, carbon_dot_path *path,
                                 rec *doc)
{
        ZERO_MEMORY(eval, sizeof(carbon_path_evaluator));
        eval->doc = doc;
        carbon_read_begin(&eval->root_it, eval->doc);
        eval->status = traverse_array(eval, path, 0, &eval->root_it, true);
        carbon_read_end(&eval->root_it);
}

bool carbon_path_evaluator_begin_mutable(carbon_path_evaluator *eval, const carbon_dot_path *path,
                                         rev *context)
{
        eval->doc = context->revised_doc;
        if (!carbon_revise_iterator_open(&eval->root_it, context)) {
            return error(ERR_OPPFAILED, "revise iterator cannot be opened");
        }
        eval->status = traverse_array(eval, path, 0, &eval->root_it, true);
        carbon_read_end(&eval->root_it);
        return true;
}

bool carbon_path_evaluator_status(carbon_path_status_e *status, carbon_path_evaluator *state)
{
        *status = state->status;
        return true;
}

bool carbon_path_evaluator_has_result(carbon_path_evaluator *state)
{
        return (state->status == CARBON_PATH_RESOLVED);
}

bool carbon_path_evaluator_end(carbon_path_evaluator *state)
{
        switch (state->result.container_type) {
                case CARBON_OBJECT:
                        carbon_object_drop(&state->result.containers.object.it);
                        break;
                case CARBON_ARRAY:
                        carbon_array_drop(&state->result.containers.array.it);
                        break;
                case CARBON_COLUMN:
                        break;
                default: error(ERR_NOTIMPLEMENTED, NULL);
        }
        return true;
}

bool carbon_path_exists(rec *doc, const char *path)
{
        carbon_find find;
        bool result = carbon_find_begin(&find, path, doc);
        carbon_find_end(&find);
        return result;
}

bool carbon_path_is_array(rec *doc, const char *path)
{
        carbon_find find;
        field_type_e field_type;
        bool result = false;

        if (carbon_find_begin(&find, path, doc)) {
                carbon_find_result_type(&field_type, &find);
                result = carbon_field_type_is_array_or_subtype(field_type);
        }

        carbon_find_end(&find);
        return result;
}

bool carbon_path_is_column(rec *doc, const char *path)
{
        carbon_find find;
        field_type_e field_type;
        bool result = false;

        if (carbon_find_begin(&find, path, doc)) {
                carbon_find_result_type(&field_type, &find);
                result = carbon_field_type_is_column_or_subtype(field_type);
        }

        carbon_find_end(&find);
        return result;
}

bool carbon_path_is_object(rec *doc, const char *path)
{
        carbon_find find;
        field_type_e field_type;
        bool result = false;

        if (carbon_find_begin(&find, path, doc)) {
                carbon_find_result_type(&field_type, &find);
                result = carbon_field_type_is_object_or_subtype(field_type);
        }

        carbon_find_end(&find);
        return result;
}

bool carbon_path_is_container(rec *doc, const char *path)
{
        return (carbon_path_is_array(doc, path) || carbon_path_is_column(doc, path) ||
                carbon_path_is_object(doc, path));
}

bool carbon_path_is_null(rec *doc, const char *path)
{
        carbon_find find;
        field_type_e field_type;
        bool result = false;

        if (carbon_find_begin(&find, path, doc)) {
                carbon_find_result_type(&field_type, &find);
                result = carbon_field_type_is_null(field_type);
        }

        carbon_find_end(&find);
        return result;
}

bool carbon_path_is_number(rec *doc, const char *path)
{
        carbon_find find;
        field_type_e field_type;
        bool result = false;

        if (carbon_find_begin(&find, path, doc)) {
                carbon_find_result_type(&field_type, &find);
                result = carbon_field_type_is_number(field_type);
        }

        carbon_find_end(&find);
        return result;
}

bool carbon_path_is_boolean(rec *doc, const char *path)
{
        carbon_find find;
        field_type_e field_type;
        bool result = false;

        if (carbon_find_begin(&find, path, doc)) {
                carbon_find_result_type(&field_type, &find);
                result = carbon_field_type_is_boolean(field_type);
        }

        carbon_find_end(&find);
        return result;
}

bool carbon_path_is_string(rec *doc, const char *path)
{
        carbon_find find;
        field_type_e field_type;
        bool result = false;

        if (carbon_find_begin(&find, path, doc)) {
                carbon_find_result_type(&field_type, &find);
                result = carbon_field_type_is_string(field_type);
        }

        carbon_find_end(&find);
        return result;
}

static inline carbon_path_status_e traverse_object(carbon_path_evaluator *state,
                                                      const carbon_dot_path *path, u32 current_path_pos,
                                                      obj_it *it)
{
        DECLARE_AND_INIT(carbon_dot_node_type_e, node_type)
        DECLARE_AND_INIT(u32, path_length)
        DECLARE_AND_INIT(bool, status)

        carbon_dot_path_type_at(&node_type, current_path_pos, path);
        JAK_ASSERT(node_type == DOT_NODE_KEY_NAME);

        status = carbon_object_next(it);
        carbon_dot_path_len(&path_length, path);
        const char *needle = carbon_dot_path_key_at(current_path_pos, path);
        u64 needle_len = strlen(needle);
        u32 next_path_pos = current_path_pos + 1;

        if (!status) {
                /** empty document */
                return CARBON_PATH_EMPTY_DOC;
        } else {
                carbon_string_field prop_key;
                do {
                        prop_key = internal_carbon_object_prop_name(it);
                        if (prop_key.length == needle_len && strncmp(prop_key.string, needle, needle_len) == 0) {
                                if (next_path_pos == path_length) {
                                        state->result.container_type = CARBON_OBJECT;
                                        internal_carbon_object_clone(&state->result.containers.object.it, it);
                                        return CARBON_PATH_RESOLVED;
                                } else {
                                        /** path end not reached, traverse further if possible */
                                        JAK_ASSERT(next_path_pos < path_length);

                                        field_type_e prop_type;
                                        internal_carbon_object_prop_type(&prop_type, it);

                                        if (!carbon_field_type_is_traversable(prop_type)) {
                                                return CARBON_PATH_NOTTRAVERSABLE;
                                        } else {
                                                JAK_ASSERT(prop_type == CARBON_FIELD_OBJECT_UNSORTED_MULTIMAP ||
                                                           prop_type == CARBON_FIELD_DERIVED_OBJECT_SORTED_MULTIMAP ||
                                                           prop_type == CARBON_FIELD_DERIVED_OBJECT_UNSORTED_MAP ||
                                                           prop_type == CARBON_FIELD_DERIVED_OBJECT_SORTED_MAP ||
                                                           prop_type == CARBON_FIELD_ARRAY_UNSORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_ARRAY_SORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_ARRAY_UNSORTED_SET ||
                                                           prop_type == CARBON_FIELD_DERIVED_ARRAY_SORTED_SET ||
                                                           prop_type == CARBON_FIELD_COLUMN_U8_UNSORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_U8_UNSORTED_SET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_SET ||
                                                           prop_type == CARBON_FIELD_COLUMN_U16_UNSORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_U16_UNSORTED_SET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_SET ||
                                                           prop_type == CARBON_FIELD_COLUMN_U32_UNSORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_U32_UNSORTED_SET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_SET ||
                                                           prop_type == CARBON_FIELD_COLUMN_U64_UNSORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_U64_UNSORTED_SET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_SET ||
                                                           prop_type == CARBON_FIELD_COLUMN_I8_UNSORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_I8_UNSORTED_SET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_SET ||
                                                           prop_type == CARBON_FIELD_COLUMN_I16_UNSORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_I16_UNSORTED_SET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_SET ||
                                                           prop_type == CARBON_FIELD_COLUMN_I32_UNSORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_I32_UNSORTED_SET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_SET ||
                                                           prop_type == CARBON_FIELD_COLUMN_I64_UNSORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_I64_UNSORTED_SET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_SET ||
                                                           prop_type == CARBON_FIELD_COLUMN_FLOAT_UNSORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET ||
                                                           prop_type == CARBON_FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET ||
                                                           prop_type == CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET);
                                                switch (prop_type) {
                                                        case CARBON_FIELD_OBJECT_UNSORTED_MULTIMAP:
                                                        case CARBON_FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                                                        case CARBON_FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                                                        case CARBON_FIELD_DERIVED_OBJECT_SORTED_MAP: {
                                                                obj_it *sub_it = carbon_item_get_object(&(it->prop.value));
                                                                carbon_path_status_e ret = traverse_object(state,
                                                                                                              path,
                                                                                                              next_path_pos,
                                                                                                              sub_it);
                                                                carbon_object_drop(sub_it);
                                                                return ret;
                                                        }
                                                        case CARBON_FIELD_ARRAY_UNSORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_ARRAY_UNSORTED_SET:
                                                        case CARBON_FIELD_DERIVED_ARRAY_SORTED_SET: {
                                                                arr_it *sub_it = carbon_item_get_array(&(it->prop.value));
                                                                carbon_path_status_e ret = traverse_array(state,
                                                                                                             path,
                                                                                                             next_path_pos,
                                                                                                             sub_it,
                                                                                                             false);
                                                                carbon_array_drop(sub_it);
                                                                return ret;
                                                        }
                                                        case CARBON_FIELD_COLUMN_U8_UNSORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_U8_SORTED_SET:
                                                        case CARBON_FIELD_COLUMN_U16_UNSORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_U16_SORTED_SET:
                                                        case CARBON_FIELD_COLUMN_U32_UNSORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_U32_SORTED_SET:
                                                        case CARBON_FIELD_COLUMN_U64_UNSORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_U64_SORTED_SET:
                                                        case CARBON_FIELD_COLUMN_I8_UNSORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_I8_SORTED_SET:
                                                        case CARBON_FIELD_COLUMN_I16_UNSORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_I16_SORTED_SET:
                                                        case CARBON_FIELD_COLUMN_I32_UNSORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_I32_SORTED_SET:
                                                        case CARBON_FIELD_COLUMN_I64_UNSORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_I64_SORTED_SET:
                                                        case CARBON_FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                                                        case CARBON_FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                                                        case CARBON_FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                                                                col_it *sub_it = carbon_item_get_column(&(it->prop.value));
                                                                return traverse_column(state,
                                                                                       path,
                                                                                       next_path_pos,
                                                                                       sub_it);
                                                        }
                                                        default: error(ERR_UNSUPPORTEDTYPE, NULL);
                                                                return CARBON_PATH_INTERNAL;
                                                }
                                        }
                                }
                        }
                } while (carbon_object_next(it));
        }

        return CARBON_PATH_NOSUCHKEY;
}

static inline carbon_path_status_e traverse_array(carbon_path_evaluator *state,
                                                     const carbon_dot_path *path, u32 current_path_pos,
                                                     arr_it *it, bool is_record)
{
        JAK_ASSERT(state);
        JAK_ASSERT(path);
        JAK_ASSERT(it);
        JAK_ASSERT(current_path_pos < path->path_len);

        DECLARE_AND_INIT(field_type_e, elem_type)
        DECLARE_AND_INIT(carbon_dot_node_type_e, node_type)
        DECLARE_AND_INIT(u32, path_length)
        DECLARE_AND_INIT(carbon_path_status_e, status)
        DECLARE_AND_INIT(u32, requested_array_idx)
        DECLARE_AND_INIT(u32, current_array_idx)
        bool is_unit_array = carbon_array_is_unit(it);

        carbon_dot_path_type_at(&node_type, current_path_pos, path);

        carbon_dot_path_len(&path_length, path);

        if (!carbon_array_next(it)) {
                /** empty document */
                return CARBON_PATH_EMPTY_DOC;
        } else {
                switch (node_type) {
                        case DOT_NODE_ARRAY_IDX:
                                carbon_dot_path_idx_at(&requested_array_idx, current_path_pos, path);
                                while (current_array_idx < requested_array_idx &&
                                        carbon_array_next(it)) { current_array_idx++; }
                                JAK_ASSERT(current_array_idx <= requested_array_idx);
                                if (current_array_idx != requested_array_idx) {
                                        /** root array has too less elements to reach the requested index */
                                        return CARBON_PATH_NOSUCHINDEX;
                                } else {
                                        /** requested index is reached; depending on the subsequent path, lookup may stops */
                                        carbon_array_field_type(&elem_type, it);
                                        u32 next_path_pos = current_path_pos + 1;
                                        if (is_unit_array && is_record &&
                                                carbon_field_type_is_column_or_subtype(elem_type)) {
                                                col_it *sub_it = carbon_item_get_column(&(it->item));
                                                return traverse_column(state,
                                                                       path,
                                                                       next_path_pos,
                                                                       sub_it);
                                        } else {
                                                if (next_path_pos < path_length) {
                                                        /** path must be further evaluated in the next step, which requires a container
                                                         * type (for traversability) */
                                                        carbon_dot_node_type_e next_node_type;
                                                        carbon_dot_path_type_at(&next_node_type, next_path_pos, path);
                                                        if (!carbon_field_type_is_traversable(elem_type)) {
                                                                /** the array element is not a container; path evaluation stops here */
                                                                return CARBON_PATH_NOTTRAVERSABLE;
                                                        } else {
                                                                /** array element is traversable */
                                                                switch (next_node_type) {
                                                                        case DOT_NODE_ARRAY_IDX:
                                                                                /** next node in path is an array index which requires that
                                                                                 * the current array element is an array or column */
                                                                                if (!carbon_field_type_is_list_or_subtype(elem_type)) {
                                                                                        return CARBON_PATH_NOCONTAINER;
                                                                                } else {
                                                                                        if (carbon_field_type_is_array_or_subtype(elem_type)) {
                                                                                                arr_it *sub_it = carbon_item_get_array(&(it->item));
                                                                                                status = traverse_array(
                                                                                                        state,
                                                                                                        path,
                                                                                                        next_path_pos,
                                                                                                        sub_it, false);
                                                                                                carbon_array_drop(
                                                                                                        sub_it);
                                                                                                return status;
                                                                                        } else {
                                                                                                JAK_ASSERT(carbon_field_type_is_column_or_subtype(elem_type));
                                                                                                col_it *sub_it = carbon_item_get_column(&(it->item));
                                                                                                return traverse_column(
                                                                                                        state,
                                                                                                        path,
                                                                                                        next_path_pos,
                                                                                                        sub_it);
                                                                                        }
                                                                                }
                                                                        case DOT_NODE_KEY_NAME:
                                                                                /** next node in path is a key name which requires that
                                                                                 * the current array element is of type object */
                                                                                if (!carbon_field_type_is_object_or_subtype(
                                                                                        elem_type)) {
                                                                                        return CARBON_PATH_NOTANOBJECT;
                                                                                } else {
                                                                                        obj_it *sub_it = carbon_item_get_object(&(it->item));
                                                                                        status = traverse_object(state,
                                                                                                                 path,
                                                                                                                 next_path_pos,
                                                                                                                 sub_it);
                                                                                        carbon_object_drop(sub_it);
                                                                                        return status;
                                                                                }
                                                                        default: error(ERR_INTERNALERR, NULL);
                                                                                return CARBON_PATH_INTERNAL;
                                                                }
                                                        }
                                                } else {
                                                        /** path end is reached */
                                                        state->result.container_type = CARBON_ARRAY;
                                                        internal_carbon_array_clone(&state->result.containers.array.it, it);
                                                        return CARBON_PATH_RESOLVED;
                                                }
                                        }
                                }
                        case DOT_NODE_KEY_NAME:
                                /** first array element exists, which must be of type object */
                                carbon_array_field_type(&elem_type, it);
                                if (!carbon_field_type_is_object_or_subtype(elem_type)) {
                                        /** first array element is not of type object and a key lookup cannot
                                         * be executed, consequentially */
                                        return CARBON_PATH_NOTANOBJECT;
                                } else {
                                        /** next node in path is a key name which requires that
                                                                         * the current array element is of type object */
                                        if (!carbon_field_type_is_object_or_subtype(elem_type)) {
                                                return CARBON_PATH_NOTANOBJECT;
                                        } else {
                                                if (is_unit_array && is_record) {
                                                        obj_it *sub_it = carbon_item_get_object(&(it->item));
                                                        status = traverse_object(state,
                                                                                 path,
                                                                                 current_path_pos,
                                                                                 sub_it);
                                                        carbon_object_drop(sub_it);
                                                        return status;
                                                } else {
                                                        return CARBON_PATH_NOSUCHKEY;
                                                }
                                        }
                                }
                                break;
                        default: error(ERR_INTERNALERR, NULL);
                                return CARBON_PATH_INTERNAL;
                }
        }
}

static inline carbon_path_status_e traverse_column(carbon_path_evaluator *state,
                                                      const carbon_dot_path *path, u32 current_path_pos,
                                                      col_it *it)
{
        DECLARE_AND_INIT(u32, total_path_len)
        DECLARE_AND_INIT(u32, requested_idx)
        DECLARE_AND_INIT(u32, nun_values_contained)
        DECLARE_AND_INIT(carbon_dot_node_type_e, node_type)
        DECLARE_AND_INIT(field_type_e, column_type)
        carbon_dot_path_len(&total_path_len, path);
        if (current_path_pos + 1 != total_path_len) {
                /** a column cannot contain further containers; since the current path node is not
                 * the last one, traversal cannot be continued */
                return CARBON_PATH_NONESTING;
        } else {
                carbon_dot_path_type_at(&node_type, current_path_pos, path);
                JAK_ASSERT(node_type == DOT_NODE_ARRAY_IDX);
                carbon_dot_path_idx_at(&requested_idx, current_path_pos, path);
                carbon_column_values_info(&column_type, &nun_values_contained, it);
                if (requested_idx >= nun_values_contained) {
                        /** requested index does not exists in this column */
                        return CARBON_PATH_NOSUCHINDEX;
                } else {
                        state->result.container_type = CARBON_COLUMN;
                        carbon_column_clone(&state->result.containers.column.it, it);
                        state->result.containers.column.elem_pos = requested_idx;
                        return CARBON_PATH_RESOLVED;
                }
        }
}