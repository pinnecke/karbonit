/**
 * rewrite - replace a field by another field
 *
 * Copyright 2020 Marcus Pinnecke
 */

#include <karbonit/carbon/rewrite.h>
#include <karbonit/carbon/field.h>
#include <karbonit/carbon/find.h>
#include <karbonit/karbonit.h>
#include "dot-eval.h"
#include "item.h"
#include "find.h"

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
//
//static void column_rewrite_domain_i16()
//{
//
//}
//
//static void column_rewrite_domain_i32()
//{
//
//}
//
//static void column_rewrite_domain_i64()
//{
//
//}
//
//static void column_rewrite_domain_u16()
//{
//
//}
//
//static void column_rewrite_domain_u32()
//{
//
//}
//
//static void column_rewrite_domain_u64()
//{
//
//}
//
//static void column_to_array_and_insert(find *find, item *src)
//{
//
//}
//
//static void column_try_insert(find *find, item *src, item_type_e src_type)
//{
//        field_e dst_type = find->eval.result.containers.column.it.field_type;
//        col_it *it = &find->eval.result.containers.column.it;
//        u32 pos = find->eval.result.containers.column.elem_pos;
//
//        switch (src_type) {
//        case ITEM_NULL:
//                /* null is compatible to all column types */
//                col_it_update_set_null(it, pos);
//                break;
//        case ITEM_TRUE:
//        case ITEM_FALSE:
//                /* booleans are compatible to boolean columns only */
//                if (dst_type == FIELD_TRUE ||dst_type == FIELD_FALSE) {
//                        src_type == ITEM_TRUE ? col_it_update_set_true(it, pos) : col_it_update_set_false(it, pos);
//                        return;
//                }
//                break;
//        case ITEM_NUMBER_SIGNED:
//        case ITEM_NUMBER_UNSIGNED:
//        case ITEM_NUMBER_FLOAT:
//                /* numbers are compatible to numbers columns only */
//                if ((dst_type == FIELD_NUMBER_I8) || (dst_type == FIELD_NUMBER_I16) || (dst_type == FIELD_NUMBER_I32) ||
//                    (dst_type == FIELD_NUMBER_I64) || (dst_type == FIELD_NUMBER_U8) || (dst_type == FIELD_NUMBER_U16) ||
//                    (dst_type == FIELD_NUMBER_U32) || (dst_type == FIELD_NUMBER_U64) || (dst_type == FIELD_NUMBER_FLOAT))
//                {
//                        if (dst_type == FIELD_NUMBER_FLOAT) {
//                                if (src_type == ITEM_NUMBER_SIGNED) {
//                                        col_it_update_set_float(it, pos, ITEM_GET_NUMBER_SIGNED(src, NULL_INT64));
//                                } else if (src_type == ITEM_NUMBER_UNSIGNED) {
//                                        col_it_update_set_float(it, pos, ITEM_GET_NUMBER_UNSIGNED(src, NULL_UINT64));
//                                } else {
//                                        col_it_update_set_float(it, pos, ITEM_GET_NUMBER_FLOAT(src, NULL_FLOAT));
//                                }
//                        } else if (dst_type == FIELD_NUMBER_I8 || dst_type == FIELD_NUMBER_I16 ||
//                                   dst_type == FIELD_NUMBER_I32 || dst_type == FIELD_NUMBER_I64)
//                        {
//                                if (src_type == ITEM_NUMBER_SIGNED) {
//                                        i64 value = ITEM_GET_NUMBER_SIGNED(item, NULL_INT64);
//                                        number_min_type_e min = number_min_type_signed(value);
//                                        switch (min) {
//                                        case NUMBER_I8:
//                                                if (col_it_is_i8(it)) {
//                                                        col_it_update_set_i8(it, pos, (i8) value);
//                                                } else if (col_it_is_i16(it)) {
//                                                        col_it_update_set_i16(it, pos, (i16) value);
//                                                } else if (col_it_is_i32(it)) {
//                                                        col_it_update_set_i32(it, pos, (i32) value);
//                                                } else if (col_it_is_i64(it)) {
//                                                        col_it_update_set_i64(it, pos, (i64) value);
//                                                } else {
//                                                        ERROR(ERR_INTERNALERR, "mismatch for column type");
//                                                        return;
//                                                }
//                                        break;
//                                        case NUMBER_I16:
//                                                if (col_it_is_i8(it)) {
//                                                        column_rewrite_domain_i16(xxx);
//                                                        col_it_update_set_i16(it, pos, (i16) value);
//                                                } else if (col_it_is_i16(it)) {
//                                                        col_it_update_set_i16(it, pos, (i16) value);
//                                                } else if (col_it_is_i32(it)) {
//                                                        col_it_update_set_i32(it, pos, (i32) value);
//                                                } else if (col_it_is_i64(it)) {
//                                                        col_it_update_set_i64(it, pos, (i64) value);
//                                                } else {
//                                                        ERROR(ERR_INTERNALERR, "mismatch for column type");
//                                                        return;
//                                                }
//                                        break;
//                                        case NUMBER_I32:
//                                                if (col_it_is_i8(it) || col_it_is_i16(it)) {
//                                                        column_rewrite_domain_i32(xxx);
//                                                        col_it_update_set_i32(it, pos, (i32) value);
//                                                } else if (col_it_is_i32(it)) {
//                                                        col_it_update_set_i32(it, pos, (i32) value);
//                                                } else if (col_it_is_i64(it)) {
//                                                        col_it_update_set_i64(it, pos, (i64) value);
//                                                } else {
//                                                        ERROR(ERR_INTERNALERR, "mismatch for column type");
//                                                        return;
//                                                }
//                                        break;
//                                        case NUMBER_I64:
//                                                if (col_it_is_i8(it) || col_it_is_i16(it) || col_it_is_i64(it)) {
//                                                        column_rewrite_domain_i64(xxx);
//                                                } else {
//                                                        ERROR(ERR_INTERNALERR, "mismatch for column type");
//                                                        return;
//                                                }
//                                                col_it_update_set_i64(it, pos, (i64) value);
//                                        break;
//                                        case NUMBER_U8:
//                                        break;
//                                        case NUMBER_U16:
//                                        break;
//                                        case NUMBER_U32:
//                                        break;
//                                        case NUMBER_U64:
//                                        break;
//                                        default:
//                                                ERROR(ERR_INTERNALERR, "unsupported value for signed type");
//                                                break;
//                                        }
//                                } else if (src_type == ITEM_NUMBER_UNSIGNED) {
//                                        u64 value = ITEM_GET_NUMBER_UNSIGNED(item, NULL_UINT64);
//                                        number_min_type_e min = number_min_type_unsigned(value);
//                                        switch (min) {
//                                                case NUMBER_U8:
//                                                        if (col_it_is_u8(it)) {
//                                                                col_it_update_set_u8(it, pos, (u8) value);
//                                                        } else if (col_it_is_u16(it)) {
//                                                                col_it_update_set_u16(it, pos, (u16) value);
//                                                        } else if (col_it_is_u32(it)) {
//                                                                col_it_update_set_u32(it, pos, (u32) value);
//                                                        } else if (col_it_is_u64(it)) {
//                                                                col_it_update_set_u64(it, pos, (u64) value);
//                                                        } else {
//                                                                ERROR(ERR_INTERNALERR, "mismatch for column type");
//                                                                return;
//                                                        }
//                                                        break;
//                                                case NUMBER_U16:
//                                                        if (col_it_is_u8(it)) {
//                                                                column_rewrite_domain_u16(xxx);
//                                                                col_it_update_set_u16(it, pos, (u16) value);
//                                                        } else if (col_it_is_u16(it)) {
//                                                                col_it_update_set_u16(it, pos, (u16) value);
//                                                        } else if (col_it_is_u32(it)) {
//                                                                col_it_update_set_u32(it, pos, (u32) value);
//                                                        } else if (col_it_is_u64(it)) {
//                                                                col_it_update_set_u64(it, pos, (u64) value);
//                                                        } else {
//                                                                ERROR(ERR_INTERNALERR, "mismatch for column type");
//                                                                return;
//                                                        }
//                                                        break;
//                                                case NUMBER_U32:
//                                                        if (col_it_is_u8(it) || col_it_is_u16(it)) {
//                                                                column_rewrite_domain_u32(xxx);
//                                                                col_it_update_set_u32(it, pos, (u32) value);
//                                                        } else if (col_it_is_u32(it)) {
//                                                                col_it_update_set_u32(it, pos, (u32) value);
//                                                        } else if (col_it_is_u64(it)) {
//                                                                col_it_update_set_u64(it, pos, (u64) value);
//                                                        } else {
//                                                                ERROR(ERR_INTERNALERR, "mismatch for column type");
//                                                                return;
//                                                        }
//                                                        break;
//                                                case NUMBER_U64:
//                                                        if (col_it_is_u8(it) || col_it_is_u16(it) || col_it_is_u64(it)) {
//                                                                column_rewrite_domain_u64(xxx);
//                                                        } else {
//                                                                ERROR(ERR_INTERNALERR, "mismatch for column type");
//                                                                return;
//                                                        }
//                                                        col_it_update_set_u64(it, pos, (u64) value);
//                                                        break;
//                                                default:
//                                                        ERROR(ERR_INTERNALERR, "unsupported value for signed type");
//                                                        break;
//                                        }
//                                } else { /* source type is float */
//
//                                }
//                        } else if (dst_type == FIELD_NUMBER_I8 || dst_type == FIELD_NUMBER_I16 ||
//                                   dst_type == FIELD_NUMBER_I32 || dst_type == FIELD_NUMBER_I64)
//                        {
//                                if (src_type == ITEM_NUMBER_SIGNED) {
//                                        number_min_type_e min = number_min_type_unsigned(ITEM_GET_NUMBER_UNSIGNED(item, NULL_UINT64));
//
//                                } else if (src_type == ITEM_NUMBER_UNSIGNED) {
//
//                                } else {
//
//                                }
//                        } else { /* column contains float typed values */
//
//                        }
//                        number_min_type_e min = number_min_type_signed(ITEM_GET_NUMBER_SIGNED(item, NULL_INT64));
//
//
//                        return;
//                }
//                break;
//        default:
//                PANIC(ERR_INTERNALERR)
//        }
//
//        /* only executed if incompatible values (e.g., 'true') should be stored in the column (e.g., in column-32) */
//        column_to_array_and_insert(find, src);
//}

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
 * a domain Y if both are equal, or X is a true subset of Y.
 *
 * A domain X is embedded in a domain Y if both domains have different domain classes, but X is a subset of Y. The
 * following embedding exists u8 is embedded in i16, u16 is embedded in i32, and u32 is embedded in i64.
 *
 * @return <code>TRUE</code> if <code>item</code> was stored successfully, and <code>FALSE</code> otherwise.
 */
static inline bool column_try_insert(col_it *dst, u32 pos, item *src)
{

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
 * A domain X is covered by a domain Y if the domain of X is partially
 *
 * @return a column iterator managed in <code>buffer</code> to the changed column if the value <code>src</code> was
 * stored successfully, or <code>NULL</code> otherwise. A non-null return value should be used as <code>dst</code> for
 * the next round
 */
static inline col_it* column_try_enlarge(col_it *buffer, col_it *dst, u32 pos, item *src)
{

}

/**
 * Rewrites the column that is pointed by <code>find</code> to an array, and inserts all remaining values of
 * <code>src_it</code> into the new array at position <code>pos</code>, <code>pos + 1</code>, <code>pos + 2</code>,...
 * (potentially moving other elements towards the end).
 */
static inline void cont_as_array(find *find, u32 pos, arr_it *src_it)
{
        /* these types are not supported for column container; rewrite container */
        arr_it dst_it;
        to_array(&dst_it, find->eval.result.containers.column.it);
        array_insert(dst_it, src_it);
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
        item *item;
        internal_arr_it_create(&src_it, src, MEMFILE_TELL(src));
        col_it enlarged_dst_it, *dst_it = &find->eval.result.containers.column.it;
        u32 start_idx = find->eval.result.containers.column.elem_pos;

        while ((item = arr_it_next(&src_it))) {
                item_type_e item_type = ITEM_GET_TYPE(item);
                switch (item_type) {
                        case ITEM_UNDEF:
                                continue;
                        case ITEM_NULL:
                        case ITEM_TRUE:
                        case ITEM_FALSE:
                        case ITEM_NUMBER_SIGNED:
                        case ITEM_NUMBER_UNSIGNED:
                        case ITEM_NUMBER_FLOAT: {
                                /* these types are supported for column container; maybe enlarge domain of column */
                                if (column_try_insert(dst_it, start_idx, item)) {
                                        /* value did fit into column, no rewrite needed */
                                        start_idx++;
                                        break;
                                } else {
                                        if ((dst_it = column_try_enlarge(&enlarged_dst_it, dst_it, start_idx, item))) {
                                                /* value did fit into column after enlarging domain rewrite operation */
                                                start_idx++;
                                                break;
                                        } else {
                                                /* value was incompatible with column type; stop iterating,
                                                 * rewrite container, and continue with insertion into that array */
                                                cont_as_array(find, &src_it);
                                                return;
                                        }
                                }
                        }
                        case ITEM_STRING:
                        case ITEM_BINARY:
                        case ITEM_ARRAY:
                        case ITEM_COLUMN:
                        case ITEM_OBJECT:
                                /* these types are not supported for column container; stop iterating,
                                 * rewrite container, and continue with insertion into that array */
                                cont_as_array(find, start_idx, &src_it);
                                return;
                        default:
                                ERROR(ERR_TYPEMISMATCH, "unknown item type for 'SET' on column container");
                                break;
                }
        }
}