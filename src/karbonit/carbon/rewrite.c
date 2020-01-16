/**
 * rewrite - replace a field by another field
 *
 * Copyright 2020 Marcus Pinnecke
 */

#include <karbonit/carbon/rewrite.h>
#include <karbonit/carbon/field.h>

/*!
 * \brief Replaces the field in <code>dst</code> by the field in <code>src</code>
 *
 * \param dst memory file that is positioned to a field marker
 * \param src memory file that is positioned to a field marker
 * \param cpy_arr_bounds flag to indicate whether to copy outer-most '[' and ']'
 * \return <code>TRUE</code> is case of success, and <code>FALSE</code> otherwise
 */
void rewrite_field(memfile *dst, memfile *src, bool cpy_arr_bounds)
{
        u64 dst_start, src_start, dst_end, src_end, dst_span, src_span;
        int skip_marker = !cpy_arr_bounds; /* skip outer-most '[' and ']' */

        MEMFILE_SAVE_POSITION(dst);
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

/*!
 * \brief Replaces the element at position <code>idx</code> in the column which is stored in <code>dst</code> by the
 * field in <code>src</code>.
 *
 * Whenever needed, the column might be rewritten to match a small (or larger) value domain, or into an array in case
 * the field in  <code>src</code> cannot be stored inside a column. If a column-to-array rewrite is done.
 *
 * \param dst memory file that is positioned to a field marker
 * \param src memory file that is positioned to a field marker
 * \param cpy_arr_bounds
 */
void rewrite_column(memfile *dst, memfile *src, bool cpy_arr_bounds)
{
        UNUSED(dst)
        UNUSED(src)
        UNUSED(cpy_arr_bounds)
}