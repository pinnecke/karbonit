// file: modules.h

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

#ifndef TOOLS_MODULES
#define TOOLS_MODULES

#include <karbonit/std/opt.h>
#include <karbonit/archive/encode_async.h>
#include <karbonit/archive/encoded_doc.h>
#include <karbonit/archive/converter.h>

bool moduleCheckJsInvoke(int argc, char **argv, FILE *file, command_opt_mgr *manager);
bool moduleJs2CabInvoke(int argc, char **argv, FILE *file, command_opt_mgr *manager);
bool moduleViewCabInvoke(int argc, char **argv, FILE *file, command_opt_mgr *manager);
bool moduleInspectInvoke(int argc, char **argv, FILE *file, command_opt_mgr *manager);
bool moduleCab2JsInvoke(int argc, char **argv, FILE *file, command_opt_mgr *manager);
bool moduleListInvoke(int argc, char **argv, FILE *file, command_opt_mgr *manager);


#endif