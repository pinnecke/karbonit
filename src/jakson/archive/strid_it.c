/**
 * Copyright 2018 Marcus Pinnecke
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

#include <jakson/archive/strid_it.h>

bool strid_iter_open(strid_iter *it, archive *archive)
{
        memset(&it->vec, 0, sizeof(it->vec));
        it->disk_file = fopen(archive->disk_file_path, "r");
        if (!it->disk_file) {
                it->is_open = false;
                return false;
        }
        fseek(it->disk_file, archive->string_table.first_entry_off, SEEK_SET);
        it->is_open = true;
        it->disk_offset = archive->string_table.first_entry_off;
        return true;
}

bool strid_iter_next(bool *success, strid_info **info, size_t *info_length,
                         strid_iter *it)
{
        if (it->disk_offset != 0 && it->is_open) {
                string_entry_header header;
                size_t vector_pos = 0;
                do {
                        fseek(it->disk_file, it->disk_offset, SEEK_SET);
                        int num_read = fread(&header, sizeof(string_entry_header), 1, it->disk_file);
                        if (header.marker != '-') {
                                ERROR(ERR_INTERNALERR, NULL);
                                return false;
                        }
                        if (num_read != 1) {
                                *success = false;
                                return false;
                        } else {
                                it->vec[vector_pos].id = header.string_id;
                                it->vec[vector_pos].offset = ftell(it->disk_file);
                                it->vec[vector_pos].strlen = header.string_len;
                                it->disk_offset = header.next_entry_off;
                                vector_pos++;
                        }
                } while (header.next_entry_off != 0 && vector_pos < ARRAY_LENGTH(it->vec));

                *info_length = vector_pos;
                *success = true;
                *info = &it->vec[0];
                return true;
        } else {
                return false;
        }
}

bool strid_iter_close(strid_iter *it)
{
        if (it->is_open) {
                fclose(it->disk_file);
                it->is_open = false;
        }
        return true;
}
