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

#include <jakson/carbon/key.h>
#include <jakson/carbon/string-field.h>
#include <jakson/stdx/unique_id.h>

static void write_nokey(memfile *file)
{
        u8 marker = MNOKEY;
        memfile_write(file, &marker, sizeof(u8));
}

static void write_autokey(memfile *file)
{
        u8 marker = MAUTOKEY;
        unique_id_t key;
        unique_id_create(&key);
        memfile_write(file, &marker, sizeof(u8));
        memfile_write(file, &key, sizeof(unique_id_t));
}

static void write_ukey(memfile *file)
{
        u8 marker = MUKEY;
        u64 key = 0;
        memfile_write(file, &marker, sizeof(u8));
        memfile_write(file, &key, sizeof(u64));
}

static void write_ikey(memfile *file)
{
        u8 marker = MIKEY;
        i64 key = 0;
        memfile_write(file, &marker, sizeof(u8));
        memfile_write(file, &key, sizeof(u64));
}

static void write_skey(memfile *file)
{
        u8 marker = MSKEY;
        const char *key = "";
        memfile_write(file, &marker, sizeof(u8));
        string_field_write(file, key);
}

bool key_create(memfile *file, key_e type)
{
        switch (type) {
                case CARBON_KEY_NOKEY:
                        write_nokey(file);
                        break;
                case CARBON_KEY_AUTOKEY:
                        write_autokey(file);
                        break;
                case CARBON_KEY_UKEY:
                        write_ukey(file);
                        break;
                case CARBON_KEY_IKEY:
                        write_ikey(file);
                        break;
                case CARBON_KEY_SKEY:
                        write_skey(file);
                        break;
                default:
                        return false;
        }
        return true;
}

bool key_skip(key_e *out, memfile *file)
{
        key_read(NULL, out, file);
        return true;
}

bool key_write_unsigned(memfile *file, u64 key)
{
        DECLARE_AND_INIT(key_e, key_type)

        key_read_type(&key_type, file);
        if (key_is_unsigned(key_type)) {
                memfile_write(file, &key, sizeof(u64));
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, NULL);
        }
}

bool key_write_signed(memfile *file, i64 key)
{
        DECLARE_AND_INIT(key_e, key_type)

        key_read_type(&key_type, file);
        if (key_is_signed(key_type)) {
                memfile_write(file, &key, sizeof(i64));
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, NULL);
        }
}

bool key_update_string(memfile *file, const char *key)
{
        return key_update_string_wnchar(file, key, strlen(key));
}

bool key_update_string_wnchar(memfile *file, const char *key, size_t length)
{
        DECLARE_AND_INIT(key_e, key_type)
        key_read_type(&key_type, file);
        if (key_is_string(key_type)) {
                string_field_update_wnchar(file, key, length);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, NULL);
        }
}

bool key_write_string(memfile *file, const char *key)
{
        DECLARE_AND_INIT(key_e, key_type)

        key_read_type(&key_type, file);
        if (key_is_string(key_type)) {
                string_field_write(file, key);
                return true;
        } else {
                return error(ERR_TYPEMISMATCH, NULL);
        }
}

bool key_read_type(key_e *out, memfile *file)
{
        u8 marker = *memfile_read_type(file, u8);

        assert(marker == MNOKEY || marker == MAUTOKEY || marker ==
                                                                                                       MUKEY ||
                   marker == MIKEY || marker == MSKEY);

        switch (marker) {
                case MNOKEY:
                        OPTIONAL_SET(out, CARBON_KEY_NOKEY)
                        break;
                case MAUTOKEY:
                        OPTIONAL_SET(out, CARBON_KEY_AUTOKEY)
                        break;
                case MUKEY:
                        OPTIONAL_SET(out, CARBON_KEY_UKEY)
                        break;
                case MIKEY:
                        OPTIONAL_SET(out, CARBON_KEY_IKEY)
                        break;
                case MSKEY:
                        OPTIONAL_SET(out, CARBON_KEY_SKEY)
                        break;
                default:
                        return error(ERR_INTERNALERR, NULL);
        }
        return true;
}

const void *key_read(u64 *len, key_e *out, memfile *file)
{
        key_e key_type = 0;
        key_read_type(&key_type, file);

        OPTIONAL_SET(out, key_type)

        switch (key_type) {
                case CARBON_KEY_NOKEY:
                        OPTIONAL_SET(len, 0)
                        return NULL;
                case CARBON_KEY_AUTOKEY:
                        OPTIONAL_SET(len, sizeof(unique_id_t))
                        return memfile_read_type(file, unique_id_t);
                case CARBON_KEY_UKEY:
                        OPTIONAL_SET(len, sizeof(u64))
                        return memfile_read_type(file, u64);
                case CARBON_KEY_IKEY:
                        OPTIONAL_SET(len, sizeof(i64))
                        return memfile_read_type(file, i64);
                case CARBON_KEY_SKEY:
                        return string_field_read(len, file);
                default: error(ERR_INTERNALERR, NULL);
                        return NULL;
        }
}

const char *key_type_str(key_e type)
{
        switch (type) {
                case CARBON_KEY_NOKEY:
                        return "nokey";
                case CARBON_KEY_AUTOKEY:
                        return "autokey";
                case CARBON_KEY_UKEY:
                        return "ukey";
                case CARBON_KEY_IKEY:
                        return "ikey";
                case CARBON_KEY_SKEY:
                        return "skey";
                default: error(ERR_INTERNALERR, NULL);
                        return NULL;
        }
}