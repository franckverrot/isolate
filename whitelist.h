/**
  isolate - Prevent processes from accessing specific resources
  Copyright (C) 2015 Franck Verrot <franck@verrot.fr>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _WHITELIST__H_
#define _WHITELIST__H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct whitelist_t {
  FILE * acl_file;
  char * acl_path;
  char ** files;
  int files_count;
} whitelist;

whitelist * whitelist_new(const char * path);
whitelist * whitelist_destroy(whitelist * list);
int whitelist_reload_from_file(whitelist * list);
int whitelist_append(whitelist * list, const char * path);
int whitelist_allows(whitelist * list, const char * path);

#endif
