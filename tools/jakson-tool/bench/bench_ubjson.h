// file: bench_ubjson.h

/**
 *  Copyright (C) 2018 Marcus Pinnecke
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef JAKSON_BENCH_UBJSON_H
#define JAKSON_BENCH_UBJSON_H

//#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <libs/ubjsc/ubjs.h>

#include "bench_fwd.h"

#define UNUSED(x)   (void)(x);
#define ERROR_IF_NULL(x)                                                                                               \
{                                                                                                                      \
    if (!(x)) {                                                                                                        \
        struct err err;                                                                                                \
        error_init(&err);                                                                                              \
        ERROR(&err, ERR_NULLPTR);                                                                                  \
        error_print_to_stderr(&err);                                                                                   \
        return false;                                                                                                  \
    }                                                                                                                  \
}


typedef struct bench_ubjson_error {
    bench_error *err;
    //ubjson_error_t *bError;
} bench_ubjson_error;

typedef struct bench_ubjson_mgr {
    ubjs_library *lib;
    bench_ubjson_error *error;
    //ubjs_array_iterator *itArr;
    //ubjs_object_iterator *itObj;
} bench_ubjson_mgr;

typedef struct json_t json_t;
static void js2ubj_main_encode_json_to_ubjson(json_t *jsoned, ubjs_library *lib, ubjs_prmtv **pobj);

bool bench_ubjson_error_create(bench_ubjson_error *ubjsonError, bench_error *benchError);
bool bench_ubjson_error_write(bench_ubjson_error *error, char *msg, size_t errOffset);
bool bench_ubjson_mgr_create_from_file(bench_ubjson_mgr *manager, const char *filePath, bench_ubjson_error *error);
bool bench_ubjson_mgr_create_empty(bench_ubjson_mgr *manager, bench_ubjson_error *error);
bool bench_ubjson_mgr_destroy(bench_ubjson_mgr *manager);
bool bench_ubjson_get_doc(char *str, bench_ubjson_mgr *manager);

bool bench_ubjson_insert_int32(bench_ubjson_mgr *manager, const char *key, int32_t val);

bool bench_ubjson_find_int32(bench_ubjson_mgr *manager, ubjs_array_iterator *it, const char *key, int32_t val);

bool bench_ubjson_change_val_int32(bench_ubjson_mgr *manager, ubjs_array_iterator *it, const char *key, int32_t newVal);

bool bench_ubjson_convert_entry_int32(bench_ubjson_mgr *manager, ubjs_array_iterator *it, const char *key);
bool bench_ubjson_convert_entry_int64(bench_ubjson_mgr *manager, ubjs_array_iterator *it, const char *key);

bool bench_ubjson_delete_int32(bench_ubjson_mgr *manager, ubjs_array_iterator *it, const char *key);

bool bench_ubjson_execute_benchmark(bench_ubjson_mgr *manager, const char *benchType);


#endif