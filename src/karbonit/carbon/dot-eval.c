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

#include <karbonit/carbon/dot-eval.h>
#include <karbonit/carbon/find.h>
#include <karbonit/carbon/revise.h>

static inline pstatus_e _dot_eval_traverse_column(dot_eval *state,
                                                  const dot *path, u32 current_path_pos,
                                                  col_it *it);

static inline pstatus_e _dot_eval_traverse_array(dot_eval *state,
                                                 const dot *path, u32 current_path_pos,
                                                 arr_it *it, bool is_record);

void dot_eval_exec(dot_eval *eval, const dot *path,
                   rec *doc)
{
        ZERO_MEMORY(eval, sizeof(dot_eval));
        eval->doc = doc;
        rec_read(&eval->root_it, eval->doc);
        eval->status = _dot_eval_traverse_array(eval, path, 0, &eval->root_it, true);
}

bool dot_eval_begin_mutable(dot_eval *eval, const dot *path,
                                         rev *context)
{
        eval->doc = context->revised;
        if (!revise_iterator_open(&eval->root_it, context)) {
            return ERROR(ERR_OPPFAILED, "revise iterator cannot be opened");
        }
        eval->status = _dot_eval_traverse_array(eval, path, 0, &eval->root_it, true);
        return true;
}

bool dot_eval_status(pstatus_e *status, dot_eval *state)
{
        *status = state->status;
        return true;
}

bool carbon_path_exists(rec *doc, const char *path)
{
        find find;
        find_from_string(&find, path, doc);
        bool result = find_has_result(&find);
        return result;
}

bool carbon_path_is_array(rec *doc, const char *path)
{
        find find;
        field_e field_type;
        bool result = false;

        if (find_from_string(&find, path, doc)) {
                find_result_type(&field_type, &find);
                result = FIELD_IS_ARRAY_OR_SUBTYPE(field_type);
        }

        return result;
}

bool carbon_path_is_column(rec *doc, const char *path)
{
        find find;
        field_e field_type;
        bool result = false;

        if (find_from_string(&find, path, doc)) {
                find_result_type(&field_type, &find);
                result = FIELD_IS_COLUMN_OR_SUBTYPE(field_type);
        }

        return result;
}

bool carbon_path_is_object(rec *doc, const char *path)
{
        find find;
        field_e field_type;
        bool result = false;

        if (find_from_string(&find, path, doc)) {
                find_result_type(&field_type, &find);
                result = FIELD_IS_OBJECT_OR_SUBTYPE(field_type);
        }

        return result;
}

bool carbon_path_is_container(rec *doc, const char *path)
{
        return (carbon_path_is_array(doc, path) || carbon_path_is_column(doc, path) ||
                carbon_path_is_object(doc, path));
}

bool carbon_path_is_null(rec *doc, const char *path)
{
        find find;
        field_e field_type;
        bool result = false;

        if (find_from_string(&find, path, doc)) {
                find_result_type(&field_type, &find);
                result = FIELD_IS_BASE_NULL(field_type);
        }

        return result;
}

bool carbon_path_is_number(rec *doc, const char *path)
{
        find find;
        field_e field_type;
        bool result = false;

        if (find_from_string(&find, path, doc)) {
                find_result_type(&field_type, &find);
                result = FIELD_IS_NUMBER(field_type);
        }

        return result;
}

bool carbon_path_is_boolean(rec *doc, const char *path)
{
        find find;
        field_e field_type;
        bool result = false;

        if (find_from_string(&find, path, doc)) {
                find_result_type(&field_type, &find);
                result = FIELD_IS_BOOLEAN(field_type);
        }

        return result;
}

bool carbon_path_is_string(rec *doc, const char *path)
{
        find find;
        field_e field_type;
        bool result = false;

        if (find_from_string(&find, path, doc)) {
                find_result_type(&field_type, &find);
                result = FIELD_IS_STRING(field_type);
        }

        return result;
}

static inline pstatus_e _dot_eval_traverse_object(dot_eval *state,
                                                  const dot *path, u32 current_path_pos,
                                                  obj_it *it)
{
        DECLARE_AND_INIT(dot_node_type_e, node_type)
        DECLARE_AND_INIT(u32, length)
        DECLARE_AND_INIT(bool, status)

        dot_type_at(&node_type, current_path_pos, path);
        assert(node_type == DOT_NODE_KEY);

        status = obj_it_next(it);
        dot_len(&length, path);
        const char *needle = dot_key_at(current_path_pos, path);
        u64 needle_len = strlen(needle);
        u32 next_path_pos = current_path_pos + 1;

        if (!status) {
                /** empty document */
                return PATH_EMPTY_DOC;
        } else {
                string_field prop_key;
                do {
                        prop_key = internal_obj_it_prop_name(it);
                        if (prop_key.len == needle_len && strncmp(prop_key.str, needle, needle_len) == 0) {
                                if (next_path_pos == length) {
                                        state->result.container = OBJECT;
                                        internal_obj_it_clone(&state->result.containers.object, it);
                                        return PATH_RESOLVED;
                                } else {
                                        /** path end not reached, traverse further if possible */
                                        assert(next_path_pos < length);

                                        field_e prop_type;
                                        internal_obj_it_prop_type(&prop_type, it);

                                        if (!FIELD_IS_TRAVERSABLE(prop_type)) {
                                                return PATH_NOTTRAVERSABLE;
                                        } else {
                                                assert(prop_type == FIELD_OBJECT_UNSORTED_MULTIMAP ||
                                                           prop_type == FIELD_DERIVED_OBJECT_SORTED_MULTIMAP ||
                                                           prop_type == FIELD_DERIVED_OBJECT_UNSORTED_MAP ||
                                                           prop_type == FIELD_DERIVED_OBJECT_SORTED_MAP ||
                                                           prop_type == FIELD_ARRAY_UNSORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_ARRAY_SORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_ARRAY_UNSORTED_SET ||
                                                           prop_type == FIELD_DERIVED_ARRAY_SORTED_SET ||
                                                           prop_type == FIELD_COLUMN_U8_UNSORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_U8_UNSORTED_SET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_U8_SORTED_SET ||
                                                           prop_type == FIELD_COLUMN_U16_UNSORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_U16_UNSORTED_SET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_U16_SORTED_SET ||
                                                           prop_type == FIELD_COLUMN_U32_UNSORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_U32_UNSORTED_SET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_U32_SORTED_SET ||
                                                           prop_type == FIELD_COLUMN_U64_UNSORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_U64_UNSORTED_SET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_U64_SORTED_SET ||
                                                           prop_type == FIELD_COLUMN_I8_UNSORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_I8_UNSORTED_SET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_I8_SORTED_SET ||
                                                           prop_type == FIELD_COLUMN_I16_UNSORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_I16_UNSORTED_SET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_I16_SORTED_SET ||
                                                           prop_type == FIELD_COLUMN_I32_UNSORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_I32_UNSORTED_SET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_I32_SORTED_SET ||
                                                           prop_type == FIELD_COLUMN_I64_UNSORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_I64_UNSORTED_SET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_I64_SORTED_SET ||
                                                           prop_type == FIELD_COLUMN_FLOAT_UNSORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET ||
                                                           prop_type == FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET ||
                                                           prop_type == FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET);
                                                switch (prop_type) {
                                                        case FIELD_OBJECT_UNSORTED_MULTIMAP:
                                                        case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                                                        case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                                                        case FIELD_DERIVED_OBJECT_SORTED_MAP: {
                                                                obj_it sub_it;
                                                                ITEM_GET_OBJECT(&sub_it, &(it->prop.value));
                                                                pstatus_e ret = _dot_eval_traverse_object(state,
                                                                                                          path,
                                                                                                          next_path_pos,
                                                                                                          &sub_it);
                                                                return ret;
                                                        }
                                                        case FIELD_ARRAY_UNSORTED_MULTISET:
                                                        case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                                                        case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                                                        case FIELD_DERIVED_ARRAY_SORTED_SET: {
                                                                arr_it sub_it;
                                                                ITEM_GET_ARRAY(&sub_it, &(it->prop.value));
                                                                pstatus_e ret = _dot_eval_traverse_array(state,
                                                                                                         path,
                                                                                                         next_path_pos,
                                                                                                         &sub_it,
                                                                                                         false);
                                                                return ret;
                                                        }
                                                        case FIELD_COLUMN_U8_UNSORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_U8_SORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_U8_UNSORTED_SET:
                                                        case FIELD_DERIVED_COLUMN_U8_SORTED_SET:
                                                        case FIELD_COLUMN_U16_UNSORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_U16_SORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_U16_UNSORTED_SET:
                                                        case FIELD_DERIVED_COLUMN_U16_SORTED_SET:
                                                        case FIELD_COLUMN_U32_UNSORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_U32_SORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_U32_UNSORTED_SET:
                                                        case FIELD_DERIVED_COLUMN_U32_SORTED_SET:
                                                        case FIELD_COLUMN_U64_UNSORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_U64_SORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_U64_UNSORTED_SET:
                                                        case FIELD_DERIVED_COLUMN_U64_SORTED_SET:
                                                        case FIELD_COLUMN_I8_UNSORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_I8_SORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_I8_UNSORTED_SET:
                                                        case FIELD_DERIVED_COLUMN_I8_SORTED_SET:
                                                        case FIELD_COLUMN_I16_UNSORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_I16_SORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_I16_UNSORTED_SET:
                                                        case FIELD_DERIVED_COLUMN_I16_SORTED_SET:
                                                        case FIELD_COLUMN_I32_UNSORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_I32_SORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_I32_UNSORTED_SET:
                                                        case FIELD_DERIVED_COLUMN_I32_SORTED_SET:
                                                        case FIELD_COLUMN_I64_UNSORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_I64_SORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_I64_UNSORTED_SET:
                                                        case FIELD_DERIVED_COLUMN_I64_SORTED_SET:
                                                        case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                                                        case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                                                        case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                                                        case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                                                        case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET: {
                                                                col_it sub_it;
                                                                ITEM_GET_COLUMN(&sub_it, &(it->prop.value));
                                                                return _dot_eval_traverse_column(state,
                                                                                                 path,
                                                                                                 next_path_pos,
                                                                                                 &sub_it);
                                                        }
                                                        default: ERROR(ERR_UNSUPPORTEDTYPE, NULL);
                                                                return PATH_INTERNAL;
                                                }
                                        }
                                }
                        }
                } while (obj_it_next(it));
        }

        return PATH_NOSUCHKEY;
}

static inline pstatus_e _dot_eval_traverse_array(dot_eval *state,
                                                     const dot *path, u32 current_path_pos,
                                                     arr_it *it, bool is_record)
{
        assert(state);
        assert(path);
        assert(it);
        assert(current_path_pos < VEC_LENGTH(&path->nodes));

        DECLARE_AND_INIT(field_e, elem_type)
        DECLARE_AND_INIT(dot_node_type_e, node_type)
        DECLARE_AND_INIT(u32, length)
        DECLARE_AND_INIT(pstatus_e, status)
        DECLARE_AND_INIT(u32, requested_array_idx)
        DECLARE_AND_INIT(u32, current_array_idx)

        dot_type_at(&node_type, current_path_pos, path);

        dot_len(&length, path);

        if (!arr_it_next(it)) {
                /** empty document */
                return PATH_EMPTY_DOC;
        } else {
                bool is_unit_array = arr_it_is_unit(it);
                switch (node_type) {
                        case DOT_NODE_IDX:
                                dot_idx_at(&requested_array_idx, current_path_pos, path);

                                while (current_array_idx < requested_array_idx && arr_it_next(it)) {
                                        current_array_idx++;
                                }
                                assert(current_array_idx <= requested_array_idx);
                                if (current_array_idx != requested_array_idx) {
                                        /** root array has too less elements to reach the requested index */
                                        return PATH_NOSUCHINDEX;
                                }

                                /** requested index is reached; depending on the subsequent path, lookup may stops */
                                arr_it_field_type(&elem_type, it);
                                u32 next_path_pos = current_path_pos + 1;
                                if (is_unit_array && is_record &&
                                        FIELD_IS_COLUMN_OR_SUBTYPE(elem_type)) {
                                        col_it sub_it;
                                        ITEM_GET_COLUMN(&sub_it, &(it->item));
                                        return _dot_eval_traverse_column(state,
                                                                         path,
                                                                         next_path_pos,
                                                                         &sub_it);
                                } else {
                                        if (next_path_pos < length) {
                                                /** path must be further evaluated in the next step, which requires a container
                                                 * type (for traversability) */
                                                dot_node_type_e next_node_type;
                                                dot_type_at(&next_node_type, next_path_pos, path);
                                                if (!FIELD_IS_TRAVERSABLE(elem_type)) {
                                                        /** the array element is not a container; path evaluation stops here */
                                                        return PATH_NOTTRAVERSABLE;
                                                } else {
                                                        /** array element is traversable */
                                                        switch (next_node_type) {
                                                                case DOT_NODE_IDX:
                                                                        /** next node in path is an array index which requires that
                                                                         * the current array element is an array or column */
                                                                        if (!FIELD_IS_LIST_OR_SUBTYPE(elem_type)) {
                                                                                return PATH_NOCONTAINER;
                                                                        } else {
                                                                                if (FIELD_IS_ARRAY_OR_SUBTYPE(elem_type)) {
                                                                                        arr_it sub_it;
                                                                                        ITEM_GET_ARRAY(&sub_it, &(it->item));
                                                                                        status = _dot_eval_traverse_array(
                                                                                                state,
                                                                                                path,
                                                                                                next_path_pos,
                                                                                                &sub_it, false);
                                                                                        return status;
                                                                                } else {
                                                                                        assert(FIELD_IS_COLUMN_OR_SUBTYPE(elem_type));
                                                                                        col_it sub_it;
                                                                                        ITEM_GET_COLUMN(&sub_it, &(it->item));
                                                                                        return _dot_eval_traverse_column(
                                                                                                state,
                                                                                                path,
                                                                                                next_path_pos,
                                                                                                &sub_it);
                                                                                }
                                                                        }
                                                                case DOT_NODE_KEY:
                                                                        /** next node in path is a key name which requires that
                                                                         * the current array element is of type object */
                                                                        if (!FIELD_IS_OBJECT_OR_SUBTYPE(
                                                                                elem_type)) {
                                                                                return PATH_NOTANOBJECT;
                                                                        } else {
                                                                                obj_it sub_it;
                                                                                ITEM_GET_OBJECT(&sub_it, &(it->item));
                                                                                status = _dot_eval_traverse_object(
                                                                                        state,
                                                                                        path,
                                                                                        next_path_pos,
                                                                                        &sub_it);
                                                                                return status;
                                                                        }
                                                                default: ERROR(ERR_INTERNALERR, NULL);
                                                                        return PATH_INTERNAL;
                                                        }
                                                }
                                        } else {
                                                /** path end is reached */
                                                state->result.container = ARRAY;
                                                internal_arr_it_clone(&state->result.containers.array, it);
                                                return PATH_RESOLVED;
                                        }
                                }

                case DOT_NODE_KEY:
                        /** first array element exists, which must be of type object */
                        arr_it_field_type(&elem_type, it);
                        if (!FIELD_IS_OBJECT_OR_SUBTYPE(elem_type)) {
                                /** first array element is not of type object and a key lookup cannot
                                 * be executed, consequentially */
                                return PATH_NOTANOBJECT;
                        } else {
                                /** next node in path is a key name which requires that
                                                                 * the current array element is of type object */
                                if (!FIELD_IS_OBJECT_OR_SUBTYPE(elem_type)) {
                                        return PATH_NOTANOBJECT;
                                } else {
                                        if (is_unit_array && is_record) {
                                                obj_it sub_it;
                                                ITEM_GET_OBJECT(&sub_it, &(it->item));
                                                status = _dot_eval_traverse_object(state,
                                                                                   path,
                                                                                   current_path_pos,
                                                                                   &sub_it);
                                                return status;
                                        } else {
                                                return PATH_NOSUCHKEY;
                                        }
                                }
                        }
                                break;
                        default: ERROR(ERR_INTERNALERR, NULL);
                                return PATH_INTERNAL;
                }
        }
}

static inline pstatus_e _dot_eval_traverse_column(dot_eval *state,
                                                      const dot *path, u32 current_path_pos,
                                                      col_it *it)
{
        DECLARE_AND_INIT(u32, total_len)
        DECLARE_AND_INIT(u32, requested_idx)
        DECLARE_AND_INIT(u32, nun_values_contained)
        DECLARE_AND_INIT(dot_node_type_e, node_type)
        DECLARE_AND_INIT(field_e, column_type)
        dot_len(&total_len, path);
        if (current_path_pos + 1 != total_len) {
                /** a column cannot contain further containers; since the current path node is not
                 * the last one, traversal cannot be continued */
                return PATH_NONESTING;
        } else {
                dot_type_at(&node_type, current_path_pos, path);
                assert(node_type == DOT_NODE_IDX);
                dot_idx_at(&requested_idx, current_path_pos, path);
                nun_values_contained = COL_IT_VALUES_INFO(&column_type, it);
                if (requested_idx >= nun_values_contained) {
                        /** requested index does not exists in this column */
                        return PATH_NOSUCHINDEX;
                } else {
                        state->result.container = COLUMN;
                        col_it_clone(&state->result.containers.column.it, it);
                        state->result.containers.column.elem_pos = requested_idx;
                        return PATH_RESOLVED;
                }
        }
}