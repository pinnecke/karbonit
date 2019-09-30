// file: bench_carbon.h

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

#ifndef JAKSON_BENCH_CARBON_H
#define JAKSON_BENCH_CARBON_H

#include <jakson/stdinc.h>
#include <jakson/carbon.h>
#include <jakson/carbon/find.h>

typedef struct bench_carbon_mgr {
    carbon doc;
    err err;
    carbon_find find;
    string_buffer result;

} bench_carbon_mgr;


#endif
