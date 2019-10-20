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

typedef struct bench_carbon_error {
    bench_error *err;
    //carbon_error_t *bError;
} bench_carbon_error;

typedef struct bench_carbon_mgr {
    carbon *doc;
    err *err;
    bench_carbon_error *error;
    //carbon_find *find;
    //string_buffer *result;

} bench_carbon_mgr;

bool bench_carbon_error_create(bench_carbon_error *carbonError, bench_error *benchError);
bool bench_carbon_error_write(bench_carbon_error *error, char *msg, size_t errOffset);
bool bench_carbon_mgr_create_from_file(bench_carbon_mgr *manager, bench_carbon_error *error, const char *filePath);
bool bench_carbon_mgr_create_empty(bench_carbon_mgr *manager, bench_carbon_error *error, bench_error *benchError);
bool bench_carbon_mgr_destroy(bench_carbon_mgr *manager);
bool bench_carbon_get_doc(char *str, bench_carbon_mgr *manager);

bool bench_carbon_insert_int32(bench_carbon_mgr *manager, carbon_object_it *it, carbon_insert *ins, char *key,  int32_t val);

bool bench_carbon_find_int32(bench_carbon_mgr *manager, carbon_object_it *it, char *key, int32_t val);

bool bench_carbon_change_val_int32(bench_carbon_mgr *manager, carbon_object_it *it, char *key, int32_t newVal);

bool bench_carbon_convert_entry_int32(bench_carbon_mgr *manager, carbon_object_it *it, char *key);
bool bench_carbon_convert_entry_int64(bench_carbon_mgr *manager, carbon_object_it *it, char *key);

bool bench_carbon_delete_int32(bench_carbon_mgr *manager, carbon_object_it *it, char *key);

bool bench_carbon_execute_benchmark(bench_carbon_mgr *manager, const char *benchType);


#endif
