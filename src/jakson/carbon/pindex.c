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

// ---------------------------------------------------------------------------------------------------------------------
//  includes
// ---------------------------------------------------------------------------------------------------------------------

#include <jakson/hexdump.h>
#include <jakson/carbon/pindex.h>
#include <jakson/carbon/key.h>
#include <jakson/carbon/internal.h>
#include <jakson/carbon/string-field.h>
#include <jakson/carbon/insert.h>
#include <jakson/carbon/commit.h>

// ---------------------------------------------------------------------------------------------------------------------
//  config
// ---------------------------------------------------------------------------------------------------------------------

#define pindex_CAPACITY 1024

#define PATH_MARKER_PROP_NODE 'P'
#define PATH_MARKER_ARRAY_NODE 'a'
#define PATH_MARKER_COLUMN_NODE 'A'

// ---------------------------------------------------------------------------------------------------------------------
//  types
// ---------------------------------------------------------------------------------------------------------------------

struct pindex_node {
        pindex_node_e type;

        union {
                u64 pos;
                struct {
                        const char *name;
                        u64 name_len;
                        offset_t offset;
                } key;
        } entry;

        field_e field_type;
        offset_t field_offset;

        vec ofType(struct pindex_node) sub_entries;
};

// ---------------------------------------------------------------------------------------------------------------------
//  helper prototypes
// ---------------------------------------------------------------------------------------------------------------------

static void
array_to_str(str_buf *str, pindex *index, bool is_root, unsigned intent_level);

static void array_into_record(insert *ins, pindex *index, bool is_root);

static void prop_to_str(str_buf *str, pindex *index, unsigned intent_level);

static void prop_into_record(insert *ins, pindex *index);

static void column_to_str(str_buf *str, pindex *index, unsigned intent_level);

static void column_into_record(insert *ins, pindex *index);

static void object_build_index(struct pindex_node *parent, obj_it *elem_it);

static void array_build_index(struct pindex_node *parent, arr_it *elem_it);

static void node_flat(memfile *file, struct pindex_node *node);

// ---------------------------------------------------------------------------------------------------------------------
//  helper
// ---------------------------------------------------------------------------------------------------------------------

static void intent(str_buf *str, unsigned intent)
{
        str_buf_add_char(str, '\n');
        for (unsigned i = 0; i < intent; i++) {
                str_buf_add(str, "    ");
        }
}

static void pindex_node_init(struct pindex_node *node)
{
        ZERO_MEMORY(node, sizeof(struct pindex_node));
        vector_create(&node->sub_entries, sizeof(struct pindex_node), 10);
        node->type = PINDEX_ROOT;
}

static void pindex_node_drop(struct pindex_node *node)
{
        for (u32 i = 0; i < node->sub_entries.num_elems; i++) {
                struct pindex_node *sub = VECTOR_GET(&node->sub_entries, i, struct pindex_node);
                pindex_node_drop(sub);
        }
        vector_drop(&node->sub_entries);
}

static void pindex_node_new_array_element(struct pindex_node *node, u64 pos, offset_t value_off)
{
        pindex_node_init(node);
        node->type = PINDEX_ARRAY_INDEX;
        node->entry.pos = pos;
        node->field_offset = value_off;
}

static void pindex_node_new_column_element(struct pindex_node *node, u64 pos, offset_t value_off)
{
        pindex_node_init(node);
        node->type = PINDEX_COLUMN_INDEX;
        node->entry.pos = pos;
        node->field_offset = value_off;
}

static void pindex_node_new_object_prop(struct pindex_node *node, offset_t key_off, const char *name,
                                            u64 name_len, offset_t value_off)
{
        pindex_node_init(node);
        node->type = PINDEX_PROP_KEY;
        node->entry.key.offset = key_off;
        node->entry.key.name = name;
        node->entry.key.name_len = name_len;
        node->field_offset = value_off;
}

static void pindex_node_set_field_type(struct pindex_node *node, field_e field_type)
{
        node->field_type = field_type;
}

static struct pindex_node *
pindex_node_add_array_elem(struct pindex_node *parent, u64 pos, offset_t value_off)
{
        /** For elements in array, the type marker (e.g., [c]) is contained. That is needed since the element might
         * be a container */
        struct pindex_node *sub = VECTOR_NEW_AND_GET(&parent->sub_entries, struct pindex_node);
        pindex_node_new_array_element(sub, pos, value_off);
        return sub;
}

static struct pindex_node *
pindex_node_add_column_elem(struct pindex_node *parent, u64 pos, offset_t value_off)
{
        /** For elements in column, there is no type marker since no value is allowed to be a container */
        struct pindex_node *sub = VECTOR_NEW_AND_GET(&parent->sub_entries, struct pindex_node);
        pindex_node_new_column_element(sub, pos, value_off);
        return sub;
}

static struct pindex_node *pindex_node_add_key_elem(struct pindex_node *parent, offset_t key_off,
                                                            const char *name, u64 name_len, offset_t value_off)
{
        struct pindex_node *sub = VECTOR_NEW_AND_GET(&parent->sub_entries, struct pindex_node);
        pindex_node_new_object_prop(sub, key_off, name, name_len, value_off);
        return sub;
}

//static void pindex_node_print_level(FILE *file, struct pindex_node *node, unsigned level)
//{
//        for (unsigned i = 0; i < level; i++) {
//                fprintf(file, " ");
//        }
//        if (node->type == PINDEX_ROOT) {
//                fprintf(file, "root");
//        } else if (node->type == PINDEX_ARRAY_INDEX) {
//                fprintf(file, "array_idx(%"PRIu64"), ", node->entry.pos);
//        } else if (node->type == PINDEX_COLUMN_INDEX) {
//                fprintf(file, "column_idx(%"PRIu64"), ", node->entry.pos);
//        } else {
//                fprintf(file, "key('%*.*s', offset: 0x%x), ", 0, (int) node->entry.key.name_len, node->entry.key.name,
//                        (unsigned) node->entry.key.offset);
//        }
//        if (node->type != PINDEX_ROOT) {
//                fprintf(file, "field(type: %s, offset: 0x%x)\n", field_str(node->field_type),
//                        (unsigned) node->field_offset);
//        } else {
//                fprintf(file, "\n");
//        }
//
//        for (u32 i = 0; i < node->sub_entries.num_elems; i++) {
//                struct pindex_node *sub = VECTOR_GET(&node->sub_entries, i, struct pindex_node);
//                pindex_node_print_level(file, sub, level + 1);
//        }
//}

static const void *
record_ref_read(key_e *rec_key_type, u64 *key_length, u64 *commit_hash, memfile *memfile)
{
        MEMFILE_SAVE_POSITION(memfile);
        MEMFILE_SEEK(memfile, 0);
        const void *ret = key_read(key_length, rec_key_type, memfile);
        u64 *hash = MEMFILE_READ_TYPE(memfile, u64);
        OPTIONAL_SET(commit_hash, *hash);
        MEMFILE_RESTORE_POSITION(memfile);
        return ret;
}

static void record_ref_create(memfile *memfile, rec *doc)
{
        key_e type;
        u64 commit_hash;
        rec_key_type(&type, doc);
        rec_commit_hash(&commit_hash, doc);

        /** write record key */
        MEMFILE_SEEK(memfile, 0);
        key_create(memfile, type);
        switch (type) {
                case KEY_NOKEY: {
                        /** nothing to do */
                }
                        break;
                case KEY_AUTOKEY:
                case KEY_UKEY: {
                        u64 key;
                        rec_key_unsigned_value(&key, doc);
                        MEMFILE_SEEK(memfile, 0);
                        key_write_unsigned(memfile, key);
                }
                        break;
                case KEY_IKEY: {
                        DECLARE_AND_INIT(i64, key)
                        rec_key_signed_value(&key, doc);
                        MEMFILE_SEEK(memfile, 0);
                        key_write_signed(memfile, key);
                }
                        break;
                case KEY_SKEY: {
                        u64 len;
                        const char *key = key_string_value(&len, doc);
                        MEMFILE_SEEK(memfile, 0);
                        key_update_string_wnchar(memfile, key, len);
                }
                        break;
                default: error(ERR_TYPEMISMATCH, NULL);
        }

        /** write record version */
        MEMFILE_WRITE(memfile, &commit_hash, sizeof(u64));
}

static void array_traverse(struct pindex_node *parent, arr_it *it)
{
        u64 sub_elem_pos = 0;
        while (arr_it_next(it)) {
                offset_t sub_elem_off = internal_arr_it_tell(it);
                struct pindex_node *elem_node = pindex_node_add_array_elem(parent, sub_elem_pos, sub_elem_off);
                array_build_index(elem_node, it);

                sub_elem_pos++;
        }
}

static void column_traverse(struct pindex_node *parent, col_it *it)
{
        field_e column_type;
        field_e entry_type;
        u32 nvalues = COL_IT_VALUES_INFO(&column_type, it);

        for (u32 i = 0; i < nvalues; i++) {
                bool is_null = col_it_is_null(it, i);
                bool is_true = false;
                if (field_is_column_bool_or_subtype(column_type)) {
                        is_true = COL_IT_BOOLEAN_VALUES(NULL, it)[i];
                }
                entry_type = field_column_entry_to_regular_type(column_type, is_null, is_true);
                offset_t sub_elem_off = col_it_tell(it, i);

                struct pindex_node *node = pindex_node_add_column_elem(parent, i, sub_elem_off);
                pindex_node_set_field_type(node, entry_type);
        }
}

static void object_traverse(struct pindex_node *parent, obj_it *it)
{
        while (obj_it_next(it)) {
                offset_t key_off = 0, value_off = 0;
                internal_obj_it_tell(&key_off, &value_off, it);
                string_field prop_key = internal_obj_it_prop_name(it);
                struct pindex_node *elem_node = pindex_node_add_key_elem(parent, key_off,
                                                                         prop_key.str, prop_key.len, value_off);
                object_build_index(elem_node, it);
        }
}

static void object_build_index(struct pindex_node *parent, obj_it *elem_it)
{
        field_e field_type = 0;;
        internal_obj_it_prop_type(&field_type, elem_it);
        pindex_node_set_field_type(parent, field_type);

        switch (field_type) {
                case FIELD_NULL:
                case FIELD_TRUE:
                case FIELD_FALSE:
                case FIELD_STRING:
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_U64:
                case FIELD_NUMBER_I8:
                case FIELD_NUMBER_I16:
                case FIELD_NUMBER_I32:
                case FIELD_NUMBER_I64:
                case FIELD_NUMBER_FLOAT:
                case FIELD_BINARY:
                case FIELD_BINARY_CUSTOM:
                        /** path ends here */
                        break;
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:
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
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET: {
                        col_it it;
                        item_get_column(&it, &(elem_it->prop.value));
                        column_traverse(parent, &it);

                }
                        break;
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET: {
                        arr_it it;
                        item_get_array(&it, &(elem_it->prop.value));
                        array_traverse(parent, &it);
                }
                        break;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP: {
                        obj_it it;
                        item_get_object(&it, &(elem_it->prop.value));
                        object_traverse(parent, &it);
                }
                        break;
                default: error(ERR_INTERNALERR, NULL);
        }
}

static void array_build_index(struct pindex_node *parent, arr_it *elem_it)
{
        field_e field_type;
        arr_it_field_type(&field_type, elem_it);
        pindex_node_set_field_type(parent, field_type);

        switch (field_type) {
                case FIELD_NULL:
                case FIELD_TRUE:
                case FIELD_FALSE:
                case FIELD_STRING:
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_U64:
                case FIELD_NUMBER_I8:
                case FIELD_NUMBER_I16:
                case FIELD_NUMBER_I32:
                case FIELD_NUMBER_I64:
                case FIELD_NUMBER_FLOAT:
                case FIELD_BINARY:
                case FIELD_BINARY_CUSTOM:
                        /** path ends here */
                        break;
                case FIELD_COLUMN_FLOAT_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_FLOAT_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_FLOAT_SORTED_SET:
                case FIELD_COLUMN_BOOLEAN_UNSORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_MULTISET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_UNSORTED_SET:
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:
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
                case FIELD_DERIVED_COLUMN_I64_SORTED_SET: {
                        col_it it;
                        item_get_column(&it, &elem_it->item);
                        column_traverse(parent, &it);
                }
                        break;
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET: {
                        arr_it it;
                        item_get_array(&it, &elem_it->item);
                        array_traverse(parent, &it);
                }
                        break;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP: {
                        obj_it it;
                        item_get_object(&it, &elem_it->item);
                        object_traverse(parent, &it);
                }
                        break;
                default: error(ERR_INTERNALERR, NULL);
        }
}

static void field_ref_write(memfile *file, struct pindex_node *node)
{
        MEMFILE_WRITE_BYTE(file, node->field_type);
        if (node->field_type != FIELD_NULL && node->field_type != FIELD_TRUE &&
            node->field_type != FIELD_FALSE) {
                /** only in case of field type that is not null, true, or false, there is more information behind
                 * the field offset */
                MEMFILE_WRITE_UINTVAR_STREAM(NULL, file, node->field_offset);
        }
}

static void container_contents_flat(memfile *file, struct pindex_node *node)
{
        MEMFILE_WRITE_UINTVAR_STREAM(NULL, file, node->sub_entries.num_elems);

        /** write position offsets */
        offset_t position_off_latest = MEMFILE_TELL(file);
        for (u32 i = 0; i < node->sub_entries.num_elems; i++) {
                MEMFILE_WRITE_UINTVAR_STREAM(NULL, file, 0);
        }

        for (u32 i = 0; i < node->sub_entries.num_elems; i++) {
                offset_t node_off = MEMFILE_TELL(file);
                struct pindex_node *sub = VECTOR_GET(&node->sub_entries, i, struct pindex_node);
                node_flat(file, sub);
                MEMFILE_SAVE_POSITION(file);
                MEMFILE_SEEK(file, position_off_latest);
                signed_offset_t shift = MEMFILE_UPDATE_UINTVAR_STREAM(file, node_off);
                position_off_latest = MEMFILE_TELL(file);
                MEMFILE_RESTORE_POSITION(file);
                MEMFILE_SEEK_FROM_HERE(file, shift);
        }
}

static void container_field_flat(memfile *file, struct pindex_node *node)
{
        switch (node->field_type) {
                case FIELD_NULL:
                case FIELD_TRUE:
                case FIELD_FALSE:
                case FIELD_STRING:
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_U64:
                case FIELD_NUMBER_I8:
                case FIELD_NUMBER_I16:
                case FIELD_NUMBER_I32:
                case FIELD_NUMBER_I64:
                case FIELD_NUMBER_FLOAT:
                case FIELD_BINARY:
                case FIELD_BINARY_CUSTOM:
                        /** any path will end with this kind of field, and therefore no subsequent elements exists */
                        assert(node->sub_entries.num_elems == 0);
                        break;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP:
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET:
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
                case FIELD_DERIVED_COLUMN_BOOLEAN_SORTED_SET:
                        /** each of these field types allows for further path traversals, and therefore at least one
                         * subsequent path element must exist */
                        container_contents_flat(file, node);
                        break;
                default: error(ERR_INTERNALERR, NULL);
        }
}

static void prop_flat(memfile *file, struct pindex_node *node)
{
        MEMFILE_WRITE_BYTE(file, PATH_MARKER_PROP_NODE);
        field_ref_write(file, node);
        MEMFILE_WRITE_UINTVAR_STREAM(NULL, file, node->entry.key.offset);
        container_field_flat(file, node);
}

static void array_flat(memfile *file, struct pindex_node *node)
{
        MEMFILE_WRITE_BYTE(file, PATH_MARKER_ARRAY_NODE);
        field_ref_write(file, node);
        if (unlikely(node->type == PINDEX_ROOT)) {
                container_contents_flat(file, node);
        } else {
                container_field_flat(file, node);
        }
}

MAYBE_UNUSED
static void node_into_record(insert *ins, pindex *index)
{
        u8 next = MEMFILE_PEEK_BYTE(&index->memfile);
        switch (next) {
                case PATH_MARKER_PROP_NODE:
                        prop_into_record(ins, index);
                        break;
                case PATH_MARKER_ARRAY_NODE:
                        array_into_record(ins, index, false);
                        break;
                case PATH_MARKER_COLUMN_NODE:
                        column_into_record(ins, index);
                        break;
                default: error(ERR_CORRUPTED, NULL);
        }
}

static void node_to_str(str_buf *str, pindex *index, unsigned intent_level)
{
        u8 next = MEMFILE_PEEK_BYTE(&index->memfile);
        intent_level++;

        switch (next) {
                case PATH_MARKER_PROP_NODE:
                        prop_to_str(str, index, intent_level);
                        break;
                case PATH_MARKER_ARRAY_NODE:
                        array_to_str(str, index, false, intent_level);
                        break;
                case PATH_MARKER_COLUMN_NODE:
                        column_to_str(str, index, intent_level);
                        break;
                default: error(ERR_CORRUPTED, NULL);
        }
}

static u8 field_ref_into_record(insert *ins, pindex *index, bool is_root)
{
        u8 field_type = MEMFILE_READ_BYTE(&index->memfile);

        if (is_root) {
                insert_prop_null(ins, "container");
        } else {
                insert_prop_string(ins, "container", field_str(field_type));
        }


        if (field_type != FIELD_NULL && field_type != FIELD_TRUE &&
            field_type != FIELD_FALSE) {
                /** only in case of field type that is not null, true, or false, there is more information behind
                 * the field offset */
                u64 field_offset = MEMFILE_READ_UINTVAR_STREAM(NULL, &index->memfile);
                if (is_root) {
                        insert_prop_null(ins, "offset");
                } else {
                        str_buf str;
                        str_buf_create(&str);
                        str_buf_add_u64_as_hex_0x_prefix_compact(&str, field_offset);
                        insert_prop_string(ins, "offset", str_buf_cstr(&str));
                        str_buf_drop(&str);
                }
        } else {
                insert_prop_null(ins, "offset");
        }
        return field_type;
}

static u8 field_ref_to_str(str_buf *str, pindex *index)
{
        u8 field_type = MEMFILE_READ_BYTE(&index->memfile);

        str_buf_add_char(str, '[');
        str_buf_add_char(str, field_type);
        str_buf_add_char(str, ']');

        if (field_type != FIELD_NULL && field_type != FIELD_TRUE &&
            field_type != FIELD_FALSE) {
                /** only in case of field type that is not null, true, or false, there is more information behind
                 * the field offset */
                u64 field_offset = MEMFILE_READ_UINTVAR_STREAM(NULL, &index->memfile);
                str_buf_add_char(str, '(');
                str_buf_add_u64_as_hex_0x_prefix_compact(str, field_offset);
                str_buf_add_char(str, ')');
        }

        return field_type;
}

static void column_to_str(str_buf *str, pindex *index, unsigned intent_level)
{
        intent(str, intent_level);
        u8 marker = MEMFILE_READ_BYTE(&index->memfile);
        str_buf_add_char(str, '[');
        str_buf_add_char(str, marker);
        str_buf_add_char(str, ']');

        field_ref_to_str(str, index);
}

static u8 _insert_field_ref(insert *ins, pindex *index, bool is_root)
{
        obj_state object;
        insert *oins = insert_prop_object_begin(&object, ins, "record-reference", 1024);
        u8 ret = field_ref_into_record(oins, index, is_root);
        insert_prop_object_end(&object);
        return ret;
}

MAYBE_UNUSED
static void column_into_record(insert *ins, pindex *index)
{
        MEMFILE_SKIP_BYTE(&index->memfile);
        insert_prop_string(ins, "type", "column");
        _insert_field_ref(ins, index, false);
}

static void container_contents_into_record(insert *ins, pindex *index)
{
        u64 num_elems = MEMFILE_READ_UINTVAR_STREAM(NULL, &index->memfile);
        insert_prop_unsigned(ins, "element-count", num_elems);

        arr_state array;
        insert *ains = insert_prop_array_begin(&array, ins, "element-offsets", 1024);

        str_buf str;
        str_buf_create(&str);
        for (u32 i = 0; i < num_elems; i++) {
                u64 pos_offs = MEMFILE_READ_UINTVAR_STREAM(NULL, &index->memfile);
                str_buf_clear(&str);
                str_buf_add_u64_as_hex_0x_prefix_compact(&str, pos_offs);
                insert_string(ains, str_buf_cstr(&str));
        }
        str_buf_drop(&str);

        insert_prop_array_end(&array);

        ains = insert_prop_array_begin(&array, ins, "elements", 1024);
        UNUSED(ains)
        for (u32 i = 0; i < num_elems; i++) {
                obj_state node_obj;
                insert *node_obj_ins = insert_object_begin(&node_obj, ains, 1024);
                node_into_record(node_obj_ins, index);
                insert_object_end(&node_obj);
        }
        insert_prop_array_end(&array);

}

static void
container_contents_to_str(str_buf *str, pindex *index, unsigned intent_level)
{
        u64 num_elems = MEMFILE_READ_UINTVAR_STREAM(NULL, &index->memfile);
        str_buf_add_char(str, '(');
        str_buf_add_u64(str, num_elems);
        str_buf_add_char(str, ')');

        for (u32 i = 0; i < num_elems; i++) {
                u64 pos_offs = MEMFILE_READ_UINTVAR_STREAM(NULL, &index->memfile);
                str_buf_add_char(str, '(');
                str_buf_add_u64_as_hex_0x_prefix_compact(str, pos_offs);
                str_buf_add_char(str, ')');
        }

        for (u32 i = 0; i < num_elems; i++) {
                node_to_str(str, index, intent_level);
        }
}

static void
container_to_str(str_buf *str, pindex *index, u8 field_type, unsigned intent_level)
{
        switch (field_type) {
                case FIELD_NULL:
                case FIELD_TRUE:
                case FIELD_FALSE:
                case FIELD_STRING:
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_U64:
                case FIELD_NUMBER_I8:
                case FIELD_NUMBER_I16:
                case FIELD_NUMBER_I32:
                case FIELD_NUMBER_I64:
                case FIELD_NUMBER_FLOAT:
                case FIELD_BINARY:
                case FIELD_BINARY_CUSTOM:
                        /** nothing to do */
                        break;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP:
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET:
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
                        /** subsequent path elements to be printed */
                        container_contents_to_str(str, index, ++intent_level);
                }
                        break;
                default: error(ERR_INTERNALERR, NULL);
        }
}

static void container_into_record(insert *ins, pindex *index, u8 field_type)
{
        switch (field_type) {
                case FIELD_NULL:
                case FIELD_TRUE:
                case FIELD_FALSE:
                case FIELD_STRING:
                case FIELD_NUMBER_U8:
                case FIELD_NUMBER_U16:
                case FIELD_NUMBER_U32:
                case FIELD_NUMBER_U64:
                case FIELD_NUMBER_I8:
                case FIELD_NUMBER_I16:
                case FIELD_NUMBER_I32:
                case FIELD_NUMBER_I64:
                case FIELD_NUMBER_FLOAT:
                case FIELD_BINARY:
                case FIELD_BINARY_CUSTOM:
                        /** nothing to do */
                        break;
                case FIELD_OBJECT_UNSORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_SORTED_MULTIMAP:
                case FIELD_DERIVED_OBJECT_UNSORTED_MAP:
                case FIELD_DERIVED_OBJECT_SORTED_MAP:
                case FIELD_ARRAY_UNSORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_SORTED_MULTISET:
                case FIELD_DERIVED_ARRAY_UNSORTED_SET:
                case FIELD_DERIVED_ARRAY_SORTED_SET:
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
                        /** subsequent path elements to be printed */
                        container_contents_into_record(ins, index);
                }
                        break;
                default: error(ERR_INTERNALERR, NULL);
        }
}

static void prop_to_str(str_buf *str, pindex *index, unsigned intent_level)
{
        intent(str, intent_level++);

        u8 marker = MEMFILE_READ_BYTE(&index->memfile);
        str_buf_add_char(str, '[');
        str_buf_add_char(str, marker);
        str_buf_add_char(str, ']');

        u8 field_type = field_ref_to_str(str, index);

        u64 key_offset = MEMFILE_READ_UINTVAR_STREAM(NULL, &index->memfile);

        str_buf_add_char(str, '(');
        str_buf_add_u64_as_hex_0x_prefix_compact(str, key_offset);
        str_buf_add_char(str, ')');

        container_to_str(str, index, field_type, intent_level);
}

MAYBE_UNUSED
static void prop_into_record(insert *ins, pindex *index)
{
        MEMFILE_SKIP_BYTE(&index->memfile);
        insert_prop_string(ins, "type", "key");
        u8 field_type = _insert_field_ref(ins, index, false);

        str_buf str;
        str_buf_create(&str);

        u64 key_offset = MEMFILE_READ_UINTVAR_STREAM(NULL, &index->memfile);
        str_buf_add_u64_as_hex_0x_prefix_compact(&str, key_offset);
        insert_prop_string(ins, "key", str_buf_cstr(&str));
        str_buf_drop(&str);

        container_into_record(ins, index, field_type);
}

static void array_into_record(insert *ins, pindex *index, bool is_root)
{
        MEMFILE_SKIP_BYTE(&index->memfile);
        u8 field_type;

        insert_prop_string(ins, "parent", is_root ? "record" : "array");
        field_type = _insert_field_ref(ins, index, is_root);

        obj_state object;
        insert *oins = insert_prop_object_begin(&object, ins, "nodes", 1024);
        if (unlikely(is_root)) {
                container_contents_into_record(oins, index);
        } else {
                container_into_record(oins, index, field_type);
        }
        insert_prop_object_end(&object);
}

static void
array_to_str(str_buf *str, pindex *index, bool is_root, unsigned intent_level)
{
        intent(str, intent_level++);

        u8 marker = MEMFILE_READ_BYTE(&index->memfile);
        str_buf_add_char(str, '[');
        str_buf_add_char(str, marker);
        str_buf_add_char(str, ']');

        u8 field_type = field_ref_to_str(str, index);

        if (unlikely(is_root)) {
                container_contents_to_str(str, index, intent_level);
        } else {
                container_to_str(str, index, field_type, intent_level);
        }
}

static void column_flat(memfile *file, struct pindex_node *node)
{
        MEMFILE_WRITE_BYTE(file, PATH_MARKER_COLUMN_NODE);
        field_ref_write(file, node);
        assert(node->sub_entries.num_elems == 0);
}

static void node_flat(memfile *file, struct pindex_node *node)
{
        switch (node->type) {
                case PINDEX_PROP_KEY:
                        prop_flat(file, node);
                        break;
                case PINDEX_ARRAY_INDEX:
                        array_flat(file, node);
                        break;
                case PINDEX_COLUMN_INDEX:
                        column_flat(file, node);
                        break;
                default: error(ERR_INTERNALERR, NULL);
                        return;
        }
}

static void index_flat(memfile *file, struct pindex_node *root_array)
{
        array_flat(file, root_array);
}

static void index_build(memfile *file, rec *doc)
{
        struct pindex_node root_array;

        /** init */
        pindex_node_init(&root_array);

        arr_it it;
        u64 array_pos = 0;
        rec_read_begin(&it, doc);

        /** build index as tree structure */
        while (arr_it_next(&it)) {
                offset_t entry_offset = internal_arr_it_tell(&it);
                struct pindex_node *node = pindex_node_add_array_elem(&root_array, array_pos, entry_offset);
                array_build_index(node, &it);
                array_pos++;
        }

        index_flat(file, &root_array);
        MEMFILE_SHRINK(file);

        /** cleanup */
        pindex_node_drop(&root_array);
}

static void record_ref_to_str(str_buf *str, pindex *index)
{
        u8 rec_key_type = MEMFILE_READ_BYTE(&index->memfile);
        str_buf_add_char(str, '[');
        str_buf_add_char(str, rec_key_type);
        str_buf_add_char(str, ']');

        switch (rec_key_type) {
                case KEY_NOKEY:
                        /** nothing to do */
                        break;
                case KEY_AUTOKEY:
                case KEY_UKEY: {
                        u64 key = MEMFILE_READ_U64(&index->memfile);
                        str_buf_add_char(str, '[');
                        str_buf_add_u64(str, key);
                        str_buf_add_char(str, ']');
                }
                        break;
                case KEY_IKEY: {
                        i64 key = MEMFILE_READ_I64(&index->memfile);
                        str_buf_add_char(str, '[');;
                        str_buf_add_i64(str, key);
                        str_buf_add_char(str, ']');
                }
                        break;
                case KEY_SKEY: {
                        u64 key_len;
                        const char *key = string_field_read(&key_len, &index->memfile);
                        str_buf_add_char(str, '(');
                        str_buf_add_nchar(str, key, key_len);
                        str_buf_add_char(str, ')');
                }
                        break;
                default: error(ERR_INTERNALERR, NULL);
        }
        u64 commit_hash = MEMFILE_READ_U64(&index->memfile);
        str_buf_add_char(str, '[');
        str_buf_add_u64(str, commit_hash);
        str_buf_add_char(str, ']');
}

static void record_ref_to_record(insert *roins, pindex *index)
{
        char rec_key_type = MEMFILE_READ_BYTE(&index->memfile);
        insert_prop_string(roins, "key-type", key_type_str(rec_key_type));

        switch (rec_key_type) {
                case KEY_NOKEY:
                        /** nothing to do */
                        break;
                case KEY_AUTOKEY:
                case KEY_UKEY: {
                        u64 key = MEMFILE_READ_U64(&index->memfile);
                        insert_prop_unsigned(roins, "key-value", key);
                }
                        break;
                case KEY_IKEY: {
                        i64 key = MEMFILE_READ_I64(&index->memfile);
                        insert_prop_signed(roins, "key-value", key);
                }
                        break;
                case KEY_SKEY: {
                        u64 key_len;
                        const char *key = string_field_read(&key_len, &index->memfile);
                        insert_prop_nchar(roins, "key-value", key, key_len);
                }
                        break;
                default: error(ERR_INTERNALERR, NULL);
        }
        u64 commit_hash = MEMFILE_READ_U64(&index->memfile);
        str_buf str;
        str_buf_create(&str);
        commit_to_str(&str, commit_hash);
        insert_prop_string(roins, "commit-hash", str_buf_cstr(&str));
        str_buf_drop(&str);
}

// ---------------------------------------------------------------------------------------------------------------------
//  construction and deconstruction
// ---------------------------------------------------------------------------------------------------------------------

bool pindex_create(pindex *index, rec *doc)
{
        MEMBLOCK_CREATE(&index->memblock, pindex_CAPACITY);
        MEMFILE_OPEN(&index->memfile, index->memblock, READ_WRITE);
        record_ref_create(&index->memfile, doc);
        index_build(&index->memfile, doc);
        return true;
}

bool pindex_drop(pindex *index)
{
        UNUSED(index)
        return false;
}

// ---------------------------------------------------------------------------------------------------------------------
//  index data access and meta information
// ---------------------------------------------------------------------------------------------------------------------

const void *pindex_raw_data(u64 *size, pindex *index)
{
        if (size && index) {
                const char *raw = MEMBLOCK_RAW_DATA(index->memfile.memblock);
                MEMBLOCK_SIZE(size, index->memfile.memblock);
                return raw;
        } else {
                return NULL;
        }
}

bool pindex_commit_hash(u64 *commit_hash, pindex *index)
{
        record_ref_read(NULL, NULL, commit_hash, &index->memfile);
        return true;
}

bool pindex_key_type(key_e *rec_key_type, pindex *index)
{
        record_ref_read(rec_key_type, NULL, NULL, &index->memfile);
        return true;
}

bool pindex_key_unsigned_value(u64 *key, pindex *index)
{
        key_e rec_key_type;
        u64 ret = *(u64 *) record_ref_read(&rec_key_type, NULL, NULL, &index->memfile);
        error_if_and_return(rec_key_type != KEY_AUTOKEY && rec_key_type != KEY_UKEY, ERR_TYPEMISMATCH, NULL);
        *key = ret;
        return true;
}

bool pindex_key_signed_value(i64 *key, pindex *index)
{
        key_e rec_key_type;
        i64 ret = *(i64 *) record_ref_read(&rec_key_type, NULL, NULL, &index->memfile);
        error_if_and_return(rec_key_type != KEY_IKEY, ERR_TYPEMISMATCH, NULL);
        *key = ret;
        return true;
}

const char *pindex_key_string_value(u64 *str_len, pindex *index)
{
        if (str_len && index) {
                key_e rec_key_type;
                const char *ret = (const char *) record_ref_read(&rec_key_type, str_len, NULL, &index->memfile);
                error_if_and_return(rec_key_type != KEY_SKEY, ERR_TYPEMISMATCH, NULL);
                return ret;
        } else {
                error(ERR_NULLPTR, NULL);
                return NULL;
        }
}

bool pindex_indexes_doc(pindex *index, rec *doc)
{
        u64 index_hash = 0, doc_hash = 0;
        pindex_commit_hash(&index_hash, index);
        rec_commit_hash(&doc_hash, doc);
        if (likely(index_hash == doc_hash)) {
                key_e index_key_type, doc_key_type;
                pindex_key_type(&index_key_type, index);
                rec_key_type(&doc_key_type, doc);
                if (likely(index_key_type == doc_key_type)) {
                        switch (index_key_type) {
                                case KEY_NOKEY:
                                        return true;
                                case KEY_AUTOKEY:
                                case KEY_UKEY: {
                                        u64 index_key, doc_key;
                                        pindex_key_unsigned_value(&index_key, index);
                                        rec_key_unsigned_value(&doc_key, doc);
                                        return index_key == doc_key;
                                }
                                case KEY_IKEY: {
                                        i64 index_key, doc_key;
                                        pindex_key_signed_value(&index_key, index);
                                        rec_key_signed_value(&doc_key, doc);
                                        return index_key == doc_key;
                                }
                                case KEY_SKEY: {
                                        u64 index_key_len, doc_key_len;
                                        const char *index_key = pindex_key_string_value(&index_key_len,
                                                                                                   index);
                                        const char *doc_key = key_string_value(&doc_key_len, doc);
                                        return (index_key_len == doc_key_len) && (strcmp(index_key, doc_key) == 0);
                                }
                                default:
                                        return error(ERR_TYPEMISMATCH, NULL);
                        }
                } else {
                        return false;
                }
        } else {
                return false;
        }
}

// ---------------------------------------------------------------------------------------------------------------------
//  index access and type information
// ---------------------------------------------------------------------------------------------------------------------

bool pindex_it_open(pindex_it *it, pindex *index,
                               rec *doc)
{
        if (pindex_indexes_doc(index, doc)) {
                ZERO_MEMORY(it, sizeof(pindex_it));
                MEMFILE_OPEN(&it->memfile, index->memfile.memblock, READ_ONLY);
                it->doc = doc;
                it->container = ARRAY;
                return true;
        } else {
                return error(ERR_NOTINDEXED, NULL);
        }
}

// ---------------------------------------------------------------------------------------------------------------------
//  diagnostics
// ---------------------------------------------------------------------------------------------------------------------

bool pindex_hexdump(FILE *file, pindex *index)
{
        return MEMFILE_HEXDUMP_PRINTF(file, &index->memfile);
}

void pindex_to_record(rec *doc, pindex *index)
{
        rec_new context;
        obj_state object;

        MEMFILE_SEEK_TO_START(&index->memfile);

        insert *ins = rec_create_begin(&context, doc, KEY_NOKEY, OPTIMIZE);
        insert *oins = insert_object_begin(&object, ins, 1024);

        {
                obj_state ref_object;
                insert *roins = insert_prop_object_begin(&ref_object, oins,
                                                                                  "record-association", 1024);
                record_ref_to_record(roins, index);
                insert_prop_object_end(&ref_object);
        }
        {
                obj_state root_object;
                insert *roins = insert_prop_object_begin(&root_object, oins, "index", 1024);
                array_into_record(roins, index, true);
                insert_prop_object_end(&root_object);
        }

        insert_object_end(&object);
        rec_create_end(&context);
}

const char *pindex_to_str(str_buf *str, pindex *index)
{
        MEMFILE_SEEK_TO_START(&index->memfile);
        record_ref_to_str(str, index);
        array_to_str(str, index, true, 0);
        return str_buf_cstr(str);
}

bool pindex_print(FILE *file, pindex *index)
{
        str_buf str;
        str_buf_create(&str);
        MEMFILE_SAVE_POSITION(&index->memfile);
        MEMFILE_SEEK_TO_START(&index->memfile);
        fprintf(file, "%s", pindex_to_str(&str, index));
        MEMFILE_RESTORE_POSITION(&index->memfile);
        str_buf_drop(&str);
        return true;
}
