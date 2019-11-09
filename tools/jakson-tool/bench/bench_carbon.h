// file: bench_carbon.h

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

#ifndef JAKSON_BENCH_CARBON_H
#define JAKSON_BENCH_CARBON_H

#include <jakson/stdinc.h>
#include <jakson/carbon.h>
#include <jakson/carbon/find.h>
#include <jakson/jakson.h>

#include "bench_fwd.h"

#define BENCH_CARBON_ERROR_WRITE(error, msg, docOffset) bench_carbon_error_write(error, msg, __FILE__, __LINE__, docOffset);

typedef struct bench_carbon_error {
    bench_error *benchErr;
    err *err;
} bench_carbon_error;

typedef struct bench_carbon_mgr {
    carbon *doc;
    bench_carbon_error *error;
    uint32_t numEntries;
    uint32_t reads;
    uint32_t updates;
} bench_carbon_mgr;

bool bench_carbon_error_create(bench_carbon_error *carbonError, bench_error *benchError);
bool bench_carbon_error_destroy(bench_carbon_error *error);
bool bench_carbon_error_write(bench_carbon_error *error, const char *msg, const char *file, u32 line, size_t docOffset);
bool bench_carbon_mgr_create_from_file(bench_carbon_mgr *manager, bench_carbon_error *carbonError, bench_error *benchError, const char *filePath);
bool bench_carbon_mgr_create_empty(bench_carbon_mgr *manager, bench_carbon_error *carbonError, bench_error *benchError);
bool bench_carbon_append_doc(bench_carbon_mgr *manager, const char *filePath);
bool bench_carbon_convert_doc(size_t *conv_size, bench_carbon_mgr *manager, const char *filePath);
bool bench_carbon_mgr_destroy(bench_carbon_mgr *manager);
size_t bench_carbon_get_doc_size(bench_carbon_mgr *manager);
bool bench_carbon_to_file(bench_carbon_mgr *manager, const char *filePath);
uint32_t bench_carbon_get_reads(bench_carbon_mgr *manager);
uint32_t bench_carbon_get_updates(bench_carbon_mgr *manager);
/*
bool bench_carbon_insert_int32(bench_carbon_mgr *manager, carbon_object_it *it, carbon_insert *ins, char *key,  int32_t val);

bool bench_carbon_find_int32(bench_carbon_mgr *manager, carbon_object_it *it, char *key, int32_t val);

bool bench_carbon_change_val_int32(bench_carbon_mgr *manager, carbon_object_it *it, char *key, int32_t newVal);

bool bench_carbon_convert_entry_int32(bench_carbon_mgr *manager, carbon_object_it *it, char *key);
bool bench_carbon_convert_entry_int64(bench_carbon_mgr *manager, carbon_object_it *it, char *key);

bool bench_carbon_delete_int32(bench_carbon_mgr *manager, carbon_object_it *it, char *key);
*/
bool bench_carbon_execute_benchmark_operation(bench_carbon_mgr *manager, bench_type type, bench_operation_type opType,
        uint32_t numOperations, container_type contType);


#endif
