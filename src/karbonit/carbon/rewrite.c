/**
 * rewrite - replace a field by another field
 *
 * Copyright 2020 Marcus Pinnecke
 */

#include <karbonit/carbon/rewrite.h>
#include <karbonit/carbon/field.h>
#include <karbonit/carbon/find.h>
#include <karbonit/karbonit.h>
#include <karbonit/carbon/dot-eval.h>
#include <karbonit/carbon/item.h>
#include <karbonit/carbon/find.h>

/*!
 * \brief Replaces the field pointed by <code>find</code> to the value in <code>src</code>
 *
 * \param find is a valid evaluated dot path that points to a field either inside an array or object container
 * \param src memory file that is positioned to a field marker
 * \param cpy_arr_bounds flag to indicate whether to copy outer-most '[' and ']'
 */
void rewrite_field(find *find, memfile *src, bool cpy_arr_bounds)
{
        u64 dst_start, src_start, dst_end, src_end, dst_span, src_span;
        int skip_marker = !cpy_arr_bounds; /* skip outer-most '[' and ']' */
        memfile *dst = &(find->doc->file);

        MEMFILE_SAVE_POSITION(dst);
        MEMFILE_SEEK__UNSAFE(dst, find->field_offset);

        MEMFILE_SAVE_POSITION(src);

        dst_start = MEMFILE_TELL(dst);
        src_start = MEMFILE_TELL(src) + skip_marker; /* skip outer-most '[' if needed */

        carbon_field_skip(dst);
        carbon_field_skip(src);

        dst_end = MEMFILE_TELL(dst);
        src_end = MEMFILE_TELL(src) - skip_marker; /* ignore outer-most ']' if needed */;

        MEMFILE_RESTORE_POSITION(dst);
        MEMFILE_RESTORE_POSITION(src);
        MEMFILE_SEEK_FROM_HERE(src, skip_marker);  /* reposition to skip outer-most '[' if needed */

        dst_span = dst_end - dst_start;
        src_span = src_end - src_start;

        if (src_span > dst_span) {
                /* new field requires more space that occupied by original field */
                MEMFILE_ENSURE_SPACE(dst, (src_span - dst_span));
        }

        MEMFILE_WRITE(dst, MEMFILE_PEEK__FAST(src), src_span);

        if (src_span <= dst_span) {
                /* new field requires less space that occupied by original field; remove that extra space */
                u64 num_obsolete = dst_end - MEMFILE_TELL(dst);
                MEMFILE_INPLACE_REMOVE(dst, num_obsolete);
        }
}

typedef struct
{
    num_type_e min_type_unsigned, min_type_signed;
    bool dst_is_boolean;
    bool dst_is_u8;
    bool dst_is_u16;
    bool dst_is_u32;
    bool dst_is_u64;
    bool dst_is_i8;
    bool dst_is_i16;
    bool dst_is_i32;
    bool dst_is_i64;
    bool dst_is_float;
    bool dst_is_signed;
    bool dst_is_unsigned;
    bool src_is_null;
    bool src_is_true;
    bool src_is_false;
    bool src_is_unsigned;
    bool src_is_u8;
    bool src_is_u16;
    bool src_is_u32;
    bool src_is_u64;
    bool src_is_signed;
    bool src_is_i8;
    bool src_is_i16;
    bool src_is_i32;
    bool src_is_i64;
    bool src_is_float;
}  rewrite_match_info;

static inline void rewrite_match_info_init(rewrite_match_info *info, col_it *dst, item *src)
{
        info->dst_is_boolean = col_it_is_boolean(dst);
        info->dst_is_u8 = col_it_is_u8(dst);
        info->dst_is_u16 = col_it_is_u16(dst);
        info->dst_is_u32 = col_it_is_u32(dst);
        info->dst_is_u64 = col_it_is_u64(dst);
        info->dst_is_i8 = col_it_is_i8(dst);
        info->dst_is_i16 = col_it_is_i16(dst);
        info->dst_is_i32 = col_it_is_i32(dst);
        info->dst_is_i64 = col_it_is_i64(dst);
        info->dst_is_float = col_it_is_float(dst);
        info->dst_is_signed = info->dst_is_i8 || info->dst_is_i16 || info->dst_is_i32 || info->dst_is_i64;
        info->dst_is_unsigned = info->dst_is_u8 || info->dst_is_u16 || info->dst_is_u32 || info->dst_is_u64;

        info->src_is_null = ITEM_IS_NULL(src);
        info->src_is_true = ITEM_IS_TRUE(src);
        info->src_is_false = ITEM_IS_FALSE(src);
        info->src_is_unsigned = ITEM_IS_UNSIGNED(src);
        info->min_type_unsigned = num_min_type_unsigned(ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
        info->src_is_u8 = info->src_is_unsigned && info->min_type_unsigned == NUM_U8;
        info->src_is_u16 = info->src_is_unsigned && info->min_type_unsigned == NUM_U16;
        info->src_is_u32 = info->src_is_unsigned && info->min_type_unsigned == NUM_U32;
        info->src_is_u64 = info->src_is_unsigned && info->min_type_unsigned == NUM_U64;
        info->src_is_signed = ITEM_IS_SIGNED(src);
        info->min_type_signed = num_min_type_signed(ITEM_GET_SIGNED(src, NULL_SIGNED));
        info->src_is_i8 = info->src_is_signed && info->min_type_signed == NUM_I8;
        info->src_is_i16 = info->src_is_signed && info->min_type_signed == NUM_I16;
        info->src_is_i32 = info->src_is_signed && info->min_type_signed == NUM_I32;
        info->src_is_i64 = info->src_is_signed && info->min_type_signed == NUM_I64;
        info->src_is_float = ITEM_IS_FLOAT(src);
}

/**
 * Tries to store the value <code>src</code> at <code>pos</code> in column accessed by <code>dst</code>. The column
 * type for <code>dst</code> is <u>not</u> changed.
 *
 * The value of <code>src</code> can be successfully stored if <code>src</code>s value domain is compatible to,
 * dominated by, or embedded in the value domain of <code>dst</code>.
 *
 * A domain X is compatible to a domain Y if the values in X can be stored in Y (without any rewrite operation).
 * The following domain compatibility holds: <code>null</code> is compatible to all domains, <code>true</code> and
 * <code>false</code> are compatible to the boolean domain, and (un-)signed numbers are compatible to the float domain.
 * Trivially, any domain is compatible to itself.
 *
 * A domain X is dominated by a domain Y if both domains have the same domain class and X is smaller or equal to Y.
 * The following domain classes exists: the signed class and the unsigned class. A domain X is smaller or equal to
 * a domain Y if both are equal.
 *
 * A domain X is embedded in a domain Y if both domains have different domain classes, but X is a subset of Y. The
 * following embedding exists u8 is embedded in i16 (i32, or i64), u16 is embedded in i32 (or i64), and u32 is embedded in i64.
 *
 * @return <code>TRUE</code> if <code>item</code> was stored successfully, and <code>FALSE</code> otherwise.
 */
static inline bool column_try_insert(col_it *dst, u32 pos, item *src, const rewrite_match_info *info) {

        /* test for compatibility */
        {
                if (info->src_is_null) {
                        col_it_set_null(dst, pos);
                        return true;
                } else if (info->dst_is_boolean && (info->src_is_true || info->src_is_false)) {
                        info->src_is_true ? col_it_set_true(dst, pos) : col_it_set_false(dst, pos);
                        return true;
                } else if (info->dst_is_u8 && info->src_is_u8) {
                        col_it_set_u8(dst, pos, (u8) ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
                        return true;
                } else if (info->dst_is_u16 && info->src_is_u16) {
                        col_it_set_u16(dst, pos, (u16) ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
                        return true;
                } else if (info->dst_is_u32 && info->src_is_u32) {
                        col_it_set_u32(dst, pos, (u32) ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
                        return true;
                } else if (info->dst_is_u64 && info->src_is_u64) {
                        col_it_set_u64(dst, pos, (u64) ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
                        return true;
                } else if (info->dst_is_i8 && info->src_is_i8) {
                        col_it_set_i8(dst, pos, (i8) ITEM_GET_SIGNED(src, NULL_SIGNED));
                        return true;
                } else if (info->dst_is_i16 && info->src_is_i16) {
                        col_it_set_i16(dst, pos, (i16) ITEM_GET_SIGNED(src, NULL_SIGNED));
                        return true;
                } else if (info->dst_is_i32 && info->src_is_i32) {
                        col_it_set_i32(dst, pos, (i32) ITEM_GET_SIGNED(src, NULL_SIGNED));
                        return true;
                } else if (info->dst_is_i64 && info->src_is_i64) {
                        col_it_set_i64(dst, pos, (i64) ITEM_GET_SIGNED(src, NULL_SIGNED));
                        return true;
                } else if (info->dst_is_float && info->src_is_float) {
                        col_it_set_float(dst, pos, (float) ITEM_GET_FLOAT(src, NULL_FLOAT));
                        return true;
                }
        }

        /* test for domination */
        {
                if (info->dst_is_signed && info->src_is_signed) {
                        switch (info->min_type_signed) {
                                case NUM_I8:
                                        if (info->dst_is_i8)
                                                col_it_set_i8(dst, pos, (i8) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                        if (info->dst_is_i16)
                                                col_it_set_i16(dst, pos, (i16) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                        if (info->dst_is_i32)
                                                col_it_set_i32(dst, pos, (i32) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                        if (info->dst_is_i64)
                                                col_it_set_i64(dst, pos, (i64) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                        return true;
                                        break;
                                case NUM_I16:
                                        if (info->dst_is_i16) {
                                                col_it_set_i16(dst, pos, (i16) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                                return true;
                                        }
                                        if (info->dst_is_i32) {
                                                col_it_set_i32(dst, pos, (i32) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                                return true;
                                        }
                                        if (info->dst_is_i64) {
                                                col_it_set_i64(dst, pos, (i64) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                                return true;
                                        }
                                        break;
                                case NUM_I32:
                                        if (info->dst_is_i32) {
                                                col_it_set_i32(dst, pos, (i32) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                                return true;
                                        }
                                        if (info->dst_is_i64) {
                                                col_it_set_i64(dst, pos, (i64) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                                return true;
                                        }
                                        break;
                                case NUM_I64:
                                        if (info->dst_is_i64) {
                                                col_it_set_i64(dst, pos, (i64) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                                return true;
                                        }
                                        break;

                                default:
                                        break;
                        }
                }

                if (info->dst_is_unsigned && info->src_is_unsigned) {
                        switch (info->min_type_unsigned) {
                                case NUM_U8:
                                        if (info->dst_is_u8)
                                                col_it_set_u8(dst, pos, (u8) ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
                                        if (info->dst_is_u16)
                                                col_it_set_u16(dst, pos, (u16) ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
                                        if (info->dst_is_u32)
                                                col_it_set_u32(dst, pos, (u32) ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
                                        if (info->dst_is_u64)
                                                col_it_set_u64(dst, pos, (u64) ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
                                        return true;
                                        break;
                                case NUM_U16:
                                        if (info->dst_is_u16) {
                                                col_it_set_u16(dst, pos, (u16) ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
                                                return true;
                                        }
                                        if (info->dst_is_u32) {
                                                col_it_set_u32(dst, pos, (u32) ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
                                                return true;
                                        }
                                        if (info->dst_is_u64) {
                                                col_it_set_u64(dst, pos, (u64) ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
                                                return true;
                                        }
                                        break;
                                case NUM_U32:
                                        if (info->dst_is_u32) {
                                                col_it_set_u32(dst, pos, (u32) ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
                                                return true;
                                        }
                                        if (info->dst_is_u64) {
                                                col_it_set_u64(dst, pos, (u64) ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
                                                return true;
                                        }
                                        break;
                                case NUM_U64:
                                        if (info->dst_is_u64) {
                                                col_it_set_u64(dst, pos, (u64) ITEM_GET_UNSIGNED(src, NULL_UNSIGNED));
                                                return true;
                                        }
                                        break;

                                default:
                                        break;
                        }
                }
        }

        /* test for embedding */
        {
                // u8 is embedded in i16, u16 is embedded in i32, and u32 is embedded in i64
                if (info->src_is_u8 && (info->dst_is_i16 || info->dst_is_i32 || info->dst_is_i64)) {
                        if (info->dst_is_i16) {
                                col_it_set_i16(dst, pos, (i16) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                return true;
                        } else if (info->dst_is_i32) {
                                col_it_set_i32(dst, pos, (i32) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                return true;
                        } else {
                                col_it_set_i64(dst, pos, (i32) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                return true;
                        }
                }
                if (info->src_is_u16 && (info->dst_is_i32 || info->dst_is_i64)) {
                        if (info->dst_is_i32) {
                                col_it_set_i32(dst, pos, (i32) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                return true;
                        } else {
                                col_it_set_i64(dst, pos, (i32) ITEM_GET_SIGNED(src, NULL_SIGNED));
                                return true;
                        }
                }
                if (info->src_is_u32 && info->dst_is_i64) {
                        col_it_set_i64(dst, pos, (i32) ITEM_GET_SIGNED(src, NULL_SIGNED));
                        return true;
                }
        }

        return false;
}

static inline void rewrite_column_and_set(col_it *dst, num_type_e new_type, u32 pos, item *src)
{
        // TODO: try to avoid realloc by using the records tail buffer!

        /* compute a fitting size for the new temporary memory block */

        col_it_type_e new_col_type;

        u64 max_header_size = sizeof(u8) /* marker */ + 2 * 10 /* max. varstream blocks for 'cap' and 'num_elem' */;
        u64 elem_cap = dst->cap;

        u64 old_elem_size = INTERNAL_GET_TYPE_VALUE_SIZE(dst->field_type);
        u64 new_elem_size = 0;

        switch (new_type) {
                case NUM_U16:
                        new_elem_size = INTERNAL_GET_TYPE_VALUE_SIZE(FIELD_NUMBER_U16);
                        new_col_type = COLUMN_U16;
                        break;
                case NUM_U32:
                        new_elem_size = INTERNAL_GET_TYPE_VALUE_SIZE(FIELD_NUMBER_U32);
                        new_col_type = COLUMN_U32;
                        break;
                case NUM_U64:
                        new_elem_size = INTERNAL_GET_TYPE_VALUE_SIZE(FIELD_NUMBER_U64);
                        new_col_type = COLUMN_U64;
                        break;
                case NUM_I8:
                        new_elem_size = INTERNAL_GET_TYPE_VALUE_SIZE(FIELD_NUMBER_I8);
                        new_col_type = COLUMN_I8;
                        break;
                case NUM_I16:
                        new_elem_size = INTERNAL_GET_TYPE_VALUE_SIZE(FIELD_NUMBER_I16);
                        new_col_type = COLUMN_I16;
                        break;
                case NUM_I32:
                        new_elem_size = INTERNAL_GET_TYPE_VALUE_SIZE(FIELD_NUMBER_I32);
                        new_col_type = COLUMN_I32;
                        break;
                case NUM_I64:
                        new_elem_size = INTERNAL_GET_TYPE_VALUE_SIZE(FIELD_NUMBER_I64);
                        new_col_type = COLUMN_I64;
                        break;
                default:
                        ERROR(ERR_INTERNALERR, "unsupported target type for column rewrite operation");
                        return;
        }

        u64 old_column_size = max_header_size + elem_cap * old_elem_size;
        u64 new_column_size = max_header_size + elem_cap * new_elem_size;

        /* setup new column container in temporary memory block */
        memblock *tmp_block;
        memfile tmp_file;
        col_it tmp_dst_it;
        MEMBLOCK_CREATE(&tmp_block, new_column_size + 1);
        MEMFILE_OPEN(&tmp_file, tmp_block, READ_WRITE);

        internal_insert_column(&tmp_file, LIST_UNSORTED_MULTISET, new_col_type, new_column_size);
        MEMFILE_SEEK_TO_START(&tmp_file);
        col_it_create(&tmp_dst_it, &tmp_file, 0);

        /* copy elements from the old container to the new one and change the size */
        u32 num_elems;
        void *new_values = (void *) COL_IT_VALUES(NULL, NULL, &tmp_dst_it);
        void *old_values = (void *) COL_IT_VALUES(NULL, &num_elems, dst);
        while (num_elems--) {
                memcpy(new_values, old_values, old_elem_size);
                new_values += new_elem_size;
                old_values += old_elem_size;
        }

        /* update element count in 'tmp_dst_it' which is missing due to the direct raw data access from above */
        MEMFILE_SEEK__UNSAFE(&tmp_file, tmp_dst_it.header_begin);
        MEMFILE_UPDATE_UINTVAR_STREAM(&tmp_file, num_elems);
        tmp_dst_it.num = num_elems;

        /* replace the old column container in the dst memory file with the contents of the temporary one */
        // TODO: try to reuse memory instead of just removing everything!

        MEMFILE_SEEK__UNSAFE(&dst->file, dst->begin);
        MEMFILE_SEEK__UNSAFE(&tmp_file, 0);
        MEMFILE_CUT(&dst->file, old_column_size);
        MEMFILE_ENSURE_SPACE(&dst->file, new_column_size);
        MEMFILE_WRITE(&dst->file, MEMFILE_RAW_DATA(&tmp_file), new_column_size);
        col_it_create(dst, &dst->file, dst->begin);

        /* cleanup */
        MEMBLOCK_DROP(tmp_block);
}

/**
 * Tries to store the value <code>src</code> at <code>pos</code> in column accessed by <code>dst</code>. The column
 * type for <code>dst</code> <u>might be</u> changed.
 *
 * The value of <code>src</code> can be successfully stored if the value domain of <code>dst</code> can be enlarged and
 * the enlarged domain dominates the value domain of <code>dst</code>, if the domain <code>dst</code> embeds
 * <code>src</code> after enlargement of <code>dst</code>, or if the domain of <code>dst</code> is rewritten to match
 * the domain of <code>src</code>.
 *
 * A domain X is enlarged to a domain X' if X and X' have the same domain class and X' is a true superset of X.
 * The following domains can be enlarged: u8 to u16 to u32 to u64, and i8 to i16 to i32 to i64. A value of a domain
 * Y is contained in an enlarged domain X' if Y is a subset of X'.
 *
 * A domain X embeds a domain Y after enlargement, if both domains have a different domain class, but X is a superset
 * of Y and X was enlarged. The following <code>src</code> domains are embedded by the following <code>dst</code> domains
 * after enlargement (<code>src</code> <i>expands</code> <code>dst</code> to a particular domain):
 * u8 expands i8 to i16, u16 expands i16 to i32, and i32 expands u32 to i64.
 *
 * A domain X is rewritten to a domain X' to match another domain Y if X' and Y are equal, Y is the float domain, and
 * X is an signed or unsigned domain.
 *
 * @return a boolean indicating whether the value was stored. <code>TRUE</code> is case of success, otherwise
 * <code>FALSE</code>
 */
static inline bool column_try_enlarge(col_it *dst, u32 pos, item *src, const rewrite_match_info *info)
{
        /* try enlargement */
        {
                if (info->dst_is_signed && info->src_is_signed) {
                        if (info->dst_is_i8) {
                                if (info->src_is_i16) {
                                        rewrite_column_and_set(dst, NUM_I16, pos, src);
                                        return true;
                                } else if (info->src_is_i32) {
                                        rewrite_column_and_set(dst, NUM_I32, pos, src);
                                        return true;
                                } else if (info->src_is_i64) {
                                        rewrite_column_and_set(dst, NUM_I64, pos, src);
                                        return true;
                                }
                        } else if (info->dst_is_i16) {
                                if (info->src_is_i32) {
                                        rewrite_column_and_set(dst, NUM_I32, pos, src);
                                        return true;
                                } else if (info->src_is_i64) {
                                        rewrite_column_and_set(dst, NUM_I64, pos, src);
                                        return true;
                                }
                        } else if (info->dst_is_i32) {
                                if (info->src_is_i64) {
                                        rewrite_column_and_set(dst, NUM_I64, pos, src);
                                        return true;
                                }
                        }
                } else if (info->dst_is_unsigned && info->src_is_unsigned) {
                        if (info->dst_is_u8) {
                                if (info->src_is_u16) {
                                        rewrite_column_and_set(dst, NUM_U16, pos, src);
                                        return true;
                                } else if (info->src_is_u32) {
                                        rewrite_column_and_set(dst, NUM_U32, pos, src);
                                        return true;
                                } else if (info->src_is_u64) {
                                        rewrite_column_and_set(dst, NUM_U64, pos, src);
                                        return true;
                                }
                        } else if (info->dst_is_u16) {
                                if (info->src_is_u32) {
                                        rewrite_column_and_set(dst, NUM_U32, pos, src);
                                        return true;
                                } else if (info->src_is_u64) {
                                        rewrite_column_and_set(dst, NUM_U64, pos, src);
                                        return true;
                                }
                        } else if (info->dst_is_u32) {
                                if (info->src_is_u64) {
                                        rewrite_column_and_set(dst, NUM_U64, pos, src);
                                        return true;
                                }
                        }
                }
        }

        /* try embedding after enlargement */
        {
                // TODO: try embedding after enlargement
        }

        /* try rewrite to match */
        {
                // TODO: try rewrite to match
        }

        return false;
}

/*!
 * \brief Replaces the element at the position in the column resolved via <code>find</code> by the
 * field in <code>src</code>.
 *
 * Whenever needed, the column might be rewritten to match a larger value domain, or into an array in case
 * the field in <code>src</code> cannot be stored inside a column.
 *
 * \note Even if the domain of the column could be reset to a smaller domain (e.g., from u16 to u8), a rewrite
 * to a smaller domain is never executed by this function (and must be manually triggered). However, if the domain
 * of the column is too small to contain the new value, the column is rewritten to the larger domain. The reason for
 * this different behavior (i.e., rewrite to larger domain but never to smaller) is that a write should not introduce
 * too much overhead (i.e., don't rewrite the column if not required to hold the new datum) and that a write should
 * always succeed.
 *
 * \note If a value should be stored into the column that cannot be stored (e.g., an object cannot be stored in a
 * column), then the column is rewritten to an array to match the user 'set' request. However, this comes by a notable
 * drop in the read performance since columns are way more faster iterated than arrays. However, the same design
 * decision holds: the 'set' request should succeed.
 *
 * \param find a path evaluation that points to the element that should be overwritten.
 * \param src memory file that is positioned to a field marker
 */
void rewrite_column(find *find, memfile *src)
{
        memfile *dst = &(find->doc->file);

        MEMFILE_SAVE_POSITION(dst);
        MEMFILE_SEEK__UNSAFE(dst, find->field_offset);

        MEMFILE_SAVE_POSITION(src);

        arr_it src_it;

        internal_arr_it_create(&src_it, src, MEMFILE_TELL(src));
        col_it *dst_it = &find->eval.result.containers.column.it;
        u32 dst_elem_pos = find->eval.result.containers.column.elem_pos;
        item *item = arr_it_next(&src_it);
        if (!arr_it_is_unit(&src_it)) {
                ERROR(ERR_INTERNALERR, "rewrite column at first level only supports single elements rather than "
                                       "multi-element containers");
        } else {
                if (item) {
                        item_type_e item_type = ITEM_GET_TYPE(item);
                        switch (item_type) {
                        case ITEM_UNDEF:
                                break;
                        case ITEM_NULL:
                        case ITEM_TRUE:
                        case ITEM_FALSE:
                        case ITEM_NUMBER_SIGNED:
                        case ITEM_NUMBER_UNSIGNED:
                        case ITEM_NUMBER_FLOAT: {
                                rewrite_match_info info;
                                rewrite_match_info_init(&info, dst_it, item);

                                /* these types are supported for column container; maybe enlarge domain of column */
                                if (column_try_insert(dst_it, dst_elem_pos, item, &info)) {
                                        /* value did fit into column, no rewrite needed */
                                        /* nothing further to do */
                                } else {
                                        if ((column_try_enlarge(dst_it, dst_elem_pos, item, &info))) {
                                                /* value did fit into column after enlarging domain rewrite operation */
                                                /* nothing further to do */
                                        } else {
                                                /* value was incompatible with column type; stop here for columns,
                                                 * rewrite container to an array, and continue with insertion into that */
                                                rewrite_column_to_array(find);
                                                rewrite_field(find, src, false);
                                                MEMFILE_RESTORE_POSITION(src);
                                                MEMFILE_RESTORE_POSITION(dst);
                                        }
                                }
                        } break;
                        case ITEM_STRING:
                        case ITEM_BINARY:
                        case ITEM_COLUMN:
                        case ITEM_OBJECT:
                                /* these types are not supported for column container; stop insertion into column,
                                 * rewrite container to array, and continue with insertion into that */
                                rewrite_column_to_array(find);
                                rewrite_field(find, src, false);
                                MEMFILE_RESTORE_POSITION(src);
                                MEMFILE_RESTORE_POSITION(dst);
                                break;
                        case ITEM_ARRAY:
                                /* nested arrays are not supported for columns; stop iterating,
                                 * rewrite container to array, and continue with insertion of an nested array into that */
                                rewrite_column_to_array(find);
                                rewrite_field(find, src, true); /* note here, that array bounds are copied, too */
                                MEMFILE_RESTORE_POSITION(src);
                                MEMFILE_RESTORE_POSITION(dst);
                                break;
                        default:
                                ERROR(ERR_TYPEMISMATCH, "unknown item type for 'SET' on column container");
                                break;
                        }
                }
        }
}

/*!
 * Rewrites a column container in the memfile <code>subj</code> to an array container and updates references in
 * <code>find</code> to that new container.
 *
 * \param find find a path evaluation that points to a column container that should be rewritten to an array container
 */
void rewrite_column_to_array(find *find)
{
        memfile *dst = &(find->doc->file);

        MEMFILE_SAVE_POSITION(dst);
        MEMFILE_SEEK__UNSAFE(dst, find->field_offset);

        col_it *it = &find->eval.result.containers.column.it;

        col_it working_col_it;

        col_it_clone(&working_col_it, it); /* cloning is required since memory of 'it' in 'find' will be recycled */
        u32 needle_pos = find->eval.result.containers.column.elem_pos;

        col_it_fast_forward(&working_col_it);
        offset_t begin_off = find->field_offset, end_off = col_it_memfilepos(&working_col_it);

        /* TODO: try to avoid malloc here by using the tail puffer of a record */
        memblock *block;
        memfile tmp_file;
        arr_it tmp_arr_it;
        offset_t column_size = end_off - begin_off;
        MEMBLOCK_CREATE(&block, 2.5 * column_size);
        MEMFILE_OPEN(&tmp_file, block, READ_WRITE)

        // set up new array iterator in temporary puffer and copy elements
        insert ins;
        u32 nvalues;
        internal_insert_array(&tmp_file, LIST_UNSORTED_MULTISET, 2 * column_size);
        internal_arr_it_create(&tmp_arr_it, &tmp_file, 0);
        arr_it_insert_begin(&ins, &tmp_arr_it);

        col_it_rewind(&working_col_it);
        if (col_it_is_boolean(&working_col_it)) {
                const boolean *vals = COL_IT_BOOLEAN_VALUES(&nvalues, &working_col_it);
                while (nvalues--) {
                        boolean val = *(vals++);
                        if (IS_NULL_BOOLEAN(val)) {
                                insert_null(&ins);
                        } else if (val) {
                                insert_true(&ins);
                        } else {
                                insert_false(&ins);
                        }
                }
        } else if (col_it_is_u8(&working_col_it)) {
                const u8 *vals = COL_IT_U8_VALUES(&nvalues, &working_col_it);
                while (nvalues--) {
                        u8 val = *(vals++);
                        if (IS_NULL_U8(val)) {
                                insert_null(&ins);
                        } else {
                                insert_u8(&ins, val);
                        }
                }
        } else if (col_it_is_u16(&working_col_it)) {
                const u16 *vals = COL_IT_U16_VALUES(&nvalues, &working_col_it);
                while (nvalues--) {
                        u16 val = *(vals++);
                        if (IS_NULL_U16(val)) {
                                insert_null(&ins);
                        } else {
                                insert_u16(&ins, val);
                        }
                }
        } else if (col_it_is_u32(&working_col_it)) {
                const u32 *vals = COL_IT_U32_VALUES(&nvalues, &working_col_it);
                while (nvalues--) {
                        u32 val = *(vals++);
                        if (IS_NULL_U32(val)) {
                                insert_null(&ins);
                        } else {
                                insert_u32(&ins, val);
                        }
                }
        } else if (col_it_is_u64(&working_col_it)) {
                const u64 *vals = COL_IT_U64_VALUES(&nvalues, &working_col_it);
                while (nvalues--) {
                        u64 val = *(vals++);
                        if (IS_NULL_U64(val)) {
                                insert_null(&ins);
                        } else {
                                insert_u64(&ins, val);
                        }
                }
        } else if (col_it_is_i8(&working_col_it)) {
                const i8 *vals = COL_IT_I8_VALUES(&nvalues, &working_col_it);
                while (nvalues--) {
                        i8 val = *(vals++);
                        if (IS_NULL_I8(val)) {
                                insert_null(&ins);
                        } else {
                                insert_i8(&ins, val);
                        }
                }
        } else if (col_it_is_i16(&working_col_it)) {
                const i16 *vals = COL_IT_I16_VALUES(&nvalues, &working_col_it);
                while (nvalues--) {
                        i16 val = *(vals++);
                        if (IS_NULL_I16(val)) {
                                insert_null(&ins);
                        } else {
                                insert_i16(&ins, val);
                        }
                }
        } else if (col_it_is_i32(&working_col_it)) {
                const i32 *vals = COL_IT_I32_VALUES(&nvalues, &working_col_it);
                while (nvalues--) {
                        i32 val = *(vals++);
                        if (IS_NULL_I32(val)) {
                                insert_null(&ins);
                        } else {
                                insert_i32(&ins, val);
                        }
                }
        } else if (col_it_is_i64(&working_col_it)) {
                const i64 *vals = COL_IT_I64_VALUES(&nvalues, &working_col_it);
                while (nvalues--) {
                        i64 val = *(vals++);
                        if (IS_NULL_I64(val)) {
                                insert_null(&ins);
                        } else {
                                insert_i64(&ins, val);
                        }
                }
        } else if (col_it_is_float(&working_col_it)) {
                const float *vals = COL_IT_FLOAT_VALUES(&nvalues, &working_col_it);
                while (nvalues--) {
                        float val = *(vals++);
                        if (IS_NULL_FLOAT(val)) {
                                insert_null(&ins);
                        } else {
                                insert_float(&ins, val);
                        }
                }
        } else {
                ERROR(ERR_TYPEMISMATCH, "column type not recognized");
                goto cleanup;
        }

        arr_it_insert_end(&ins);

        internal_arr_it_create(&tmp_arr_it, &tmp_file, 0);
        internal_arr_it_fast_forward(&tmp_arr_it);
        offset_t arr_len = internal_arr_it_tell(&tmp_arr_it);

        /* overwrite column container with temporary array container */
        MEMFILE_SAVE_POSITION(&find->eval.doc->file);
        MEMFILE_SEEK__UNSAFE(&find->eval.doc->file, begin_off);
        MEMFILE_CUT(&find->eval.doc->file, (end_off - begin_off)); /* TODO: try to reuse memory! */
        MEMFILE_ENSURE_SPACE(&find->eval.doc->file, arr_len);
        MEMFILE_SEEK__UNSAFE(&find->eval.doc->file, begin_off);
        MEMFILE_WRITE(&find->eval.doc->file, MEMFILE_RAW_DATA(&tmp_file), arr_len);

        find->eval.result.container = ARRAY;

        /* position the array iterator to the element that was evaluated */
        internal_arr_it_create(&find->eval.result.containers.array, &find->eval.doc->file, begin_off);
        while (needle_pos--) {
                assert(arr_it_has_next(&find->eval.result.containers.array));
                arr_it_next(&find->eval.result.containers.array);
        }

cleanup:
        MEMBLOCK_DROP(block)

        MEMFILE_RESTORE_POSITION(dst);
}