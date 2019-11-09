// file: bench_fwd.h

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

#ifndef JAKSON_BENCH_FWD_H
#define JAKSON_BENCH_FWD_H

#define ERROR_MSG_SIZE 256
#define UINT32_MAX_DIGITS 10
#define BENCH_OPERATION_MIN_TIME 1000000

typedef enum bench_type {
    BENCH_TYPE_TEST = 0x01,
    BENCH_TYPE_INT8 = 0x02,
    BENCH_TYPE_INT16 = 0x03,
    BENCH_TYPE_INT32 = 0x04,
    BENCH_TYPE_INT64 = 0x05,
} bench_type;

typedef enum container_type {
    BENCH_CONTAINER_TYPE_ARRAY = 0x01,
    BENCH_CONTAINER_TYPE_OBJECT = 0x02,
    BENCH_CONTAINER_TYPE_COLUMN = 0x03,
} container_type;

typedef enum bench_operation_type {
    BENCH_OP_TYPE_INSERT = 0x01,
    BENCH_OP_TYPE_READ = 0x02,
    BENCH_OP_TYPE_UPDATE = 0x03,
    BENCH_OP_TYPE_DELETE = 0x04,
} bench_operation_type;

typedef struct bench_error bench_error;

#endif
