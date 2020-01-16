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
 * @param dst memory file that is positioned to a field marker
 * @param src memory file that is positioned to a field marker
 * @return <code>TRUE</code> is case of success, and <code>FALSE</code> otherwise
 */
bool rewrite_field(memfile *dst, memfile *src)
{
        u64 dst_start, src_start, dst_end, src_end, dst_span, src_span;

        MEMFILE_SAVE_POSITION(dst);
        MEMFILE_SAVE_POSITION(src);

        dst_start = MEMFILE_TELL(dst);
        src_start = MEMFILE_TELL(src);

        carbon_field_skip(dst);
        carbon_field_skip(src);

        dst_end = MEMFILE_TELL(dst);
        src_end = MEMFILE_TELL(src);

        MEMFILE_RESTORE_POSITION(dst);
        MEMFILE_RESTORE_POSITION(src);

        dst_span = dst_end - dst_start;
        src_span = src_end - src_start;

        if (src_span <= dst_span) { /* new field fit into space of original field */
                MEMFILE_WRITE(dst, MEMFILE_PEEK__FAST(src), src_span);
                u64 num_obsolete = dst_end - MEMFILE_TELL(dst);
                MEMFILE_INPLACE_REMOVE(dst, num_obsolete);
                return true;
        } else { /* new field requires more space that occupied by original field */
                return false;
        }
}

