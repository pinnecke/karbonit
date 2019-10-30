// file: bench_format_handler.h

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

#ifndef JAKSON_BENCH_FORMAT_HANDLER_H
#define JAKSON_BENCH_FORMAT_HANDLER_H

#include <sys/time.h>
#include <sys/resource.h>

#include <jakson/stdinc.h>
#include <jakson/error.h>
#include <jakson/carbon/find.h>
#include <jakson/carbon/array_it.h>

#include <libs/bson/bson.h>

#include "bench_carbon.h"
#include "bench_bson.h"
#include "bench_ubjson.h"




#define BENCH_FORMAT_BSON "BSON"
#define BENCH_FORMAT_CARBON "CARBON"
#define BENCH_FORMAT_UBJSON "UBJSON"

#define BENCH_TYPE_TEST "test"

typedef struct bench_bson_error bench_bson_error;

typedef struct bench_error {
    size_t line;
    char *msg;
    char *file;
    size_t offset;
} bench_error;

typedef struct bench_format_ops bench_format_ops;

/*
struct bench_format_ops {
    bool (*create_doc)(bench_format_mgr *, char* file);
    bool (*insert_uint32)(bench_format_mgr *, uint32_t value, unsigned int pos);
    bool (*revise_uint32)(bench_format_mgr *, uint32_t value, unsigned int pos);
    bool (*delete_uint32)(bench_format_mgr *, unsigned int pos);
    // TODO: Add more operations
};
*/

typedef struct bench_format_handler {
    bench_error *error;
    void *manager;
    char *format_name;
} bench_format_handler;

bool bench_format_handler_create_error(bench_error *error);
//bool bench_format_handler_destroy_error(bench_error *error);
bool bench_format_handler_create_handler(bench_format_handler *handler, bench_error *error, const char *filePath, const char *benchType);
bool bench_format_handler_create_carbon_handler(bench_format_handler *handler, bench_error *error, const char* filePath);
bool bench_format_handler_create_bson_handler(bench_format_handler *handler, bench_error *error, const char* filePath);
bool bench_format_handler_create_ubjson_handler(bench_format_handler *handler, bench_error *error, const char* filePath);
bool bench_format_handler_append_doc(bench_format_handler *handler, const char *filePath);
bool bench_format_handler_convert_doc(size_t *conv_size, bench_format_handler *handler, const char *filePath);
bool bench_format_handler_destroy(bench_format_handler *handler);
bool bench_format_handler_get_doc(char *str, bench_format_handler *handler);
size_t bench_format_handler_get_doc_size(bench_format_handler *handler);
bool bench_format_handler_get_process_status(char *buffer);
size_t bench_format_handler_get_process_size();
bool bench_format_handler_get_file_content(unsigned char *json_content, FILE *file, long file_size);

/*
bool bench_format_handler_insert_int8(bench_format_handler *handler, char *key, int8_t val);
bool bench_format_handler_insert_int16(bench_format_handler *handler, char *key, int16_t val);
bool bench_format_handler_insert_int32(bench_format_handler *handler, char *key, int32_t val);
bool bench_format_handler_insert_int64(bench_format_handler *handler, char *key, int64_t val);
bool bench_format_handler_insert_string(bench_format_handler *handler, char *key, const char *val);

bool bench_format_handler_find_int8(bench_format_handler *handler, char *key, int8_t val);
bool bench_format_handler_find_int16(bench_format_handler *handler, char *key, int16_t val);
bool bench_format_handler_find_int32(bench_format_handler *handler, char *key, int32_t val);
bool bench_format_handler_find_int64(bench_format_handler *handler, char *key, int64_t val);
bool bench_format_handler_find_string(bench_format_handler *handler, char *key, const char *val);

bool bench_format_handler_change_val_int8(bench_format_handler *handler, char *key, int8_t newVal);
bool bench_format_handler_change_val_int16(bench_format_handler *handler, char *key, int16_t newVal);
bool bench_format_handler_change_val_int32(bench_format_handler *handler, char *key, int32_t newVal);
bool bench_format_handler_change_val_int64(bench_format_handler *handler, char *key, int64_t newVal);
bool bench_format_handler_change_val_string(bench_format_handler *handler, char *key, const char *newVal);

bool bench_format_handler_convert_entry_int8(bench_format_handler *handler, char *key);
bool bench_format_handler_convert_entry_int16(bench_format_handler *handler, char *key);
bool bench_format_handler_convert_entry_int32(bench_format_handler *handler, char *key);
bool bench_format_handler_convert_entry_int64(bench_format_handler *handler, char *key);
bool bench_format_handler_convert_entry_string(bench_format_handler *handler, char *key, size_t size);
bool bench_format_handler_convert_entry_object(bench_format_handler *handler, char *key);

bool bench_format_handler_delete_int8(bench_format_handler *handler, char *key);
bool bench_format_handler_delete_int16(bench_format_handler *handler, char *key);
bool bench_format_handler_delete_int32(bench_format_handler *handler, char *key);
bool bench_format_handler_delete_int64(bench_format_handler *handler, char *key);
bool bench_format_handler_delete_string(bench_format_handler *handler, char *key);
*/
// TODO : Write centralized benchmark access point
bool bench_format_handler_execute_benchmark(bench_format_handler *handler, const char *benchType);

#endif
