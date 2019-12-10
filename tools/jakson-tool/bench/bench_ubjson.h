// file: bench_ubjson.h

/**
 *  Copyright (C) 2018 Marcus Pinnecke, Marcel Plumbohm
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
#include <libs/ubjsc/ubjs_primitive_str_prv.h>

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

#define BENCH_UBJSON_ERROR_WRITE(error, msg, docOffset) bench_ubjson_error_write(error, msg, __FILE__, __LINE__, docOffset);

typedef struct bench_ubjson_error {
    bench_error *benchErr;
    err *err;
} bench_ubjson_error;

typedef struct bench_ubjson_mgr {
    ubjs_library *lib;
    bench_ubjson_error *error;
    ubjs_prmtv *obj;
    //uint32_t test;
    uint32_t numEntries;
    uint32_t reads;
    uint32_t updates;
} bench_ubjson_mgr;

bool bench_ubjson_error_create(bench_ubjson_error *ubjsonError, bench_error *benchError);
bool bench_ubjson_error_destroy(bench_ubjson_error *error);
bool bench_ubjson_error_write(bench_ubjson_error *error, const char *msg, const char *file, u32 line, size_t docOffset);
bool bench_ubjson_mgr_create_from_file(bench_ubjson_mgr *manager, bench_ubjson_error *ubjsonError, bench_error *benchError, const char *filePath);
bool bench_ubjson_mgr_create_empty(bench_ubjson_mgr *manager, bench_ubjson_error *ubjsonError, bench_error *benchError);
bool bench_ubjson_append_doc(bench_ubjson_mgr *manager, const char *filePath);
bool bench_ubjson_convert_doc(size_t *conv_size, bench_ubjson_mgr *manager, const char *filePath);
bool bench_ubjson_mgr_destroy(bench_ubjson_mgr *manager);
bool bench_ubjson_get_doc(char *str, bench_ubjson_mgr *manager);
size_t bench_ubjson_get_doc_size(bench_ubjson_mgr *manager);
uint32_t bench_ubjson_get_reads(bench_ubjson_mgr *manager);
uint32_t bench_ubjson_get_updates(bench_ubjson_mgr *manager);
/*
bool bench_ubjson_insert_int32(bench_ubjson_mgr *manager, char *key, int32_t val);

bool bench_ubjson_find_int32(bench_ubjson_mgr *manager, ubjs_array_iterator *it, char *key, int32_t val);

bool bench_ubjson_change_val_int32(bench_ubjson_mgr *manager, ubjs_array_iterator *it, char *key, int32_t newVal);

bool bench_ubjson_convert_entry_int32(bench_ubjson_mgr *manager, ubjs_array_iterator *it, char *key);
bool bench_ubjson_convert_entry_int64(bench_ubjson_mgr *manager, ubjs_array_iterator *it, char *key);

bool bench_ubjson_delete_int32(bench_ubjson_mgr *manager, ubjs_array_iterator *it, char *key);
*/
bool bench_ubjson_execute_benchmark_operation(bench_ubjson_mgr *manager, bench_type type, bench_operation_type opType,
                                              uint32_t numOperations, container_type contType);


#endif