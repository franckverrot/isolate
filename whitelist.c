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

#include "whitelist.h"
#define MAX_LINE_LEN 2048

whitelist * whitelist_new(const char * path) {
  whitelist * list = (whitelist*)malloc(sizeof(whitelist));

  list->acl_path = strndup(path,strlen(path));
  list->acl_file = fopen(list->acl_path,"a+");
  list->files       = NULL;
  list->files_count = 0;

  return (list->acl_file > 0) ? list : NULL;
}

whitelist * whitelist_destroy(whitelist * list) {
  if(list) {
    fclose(list->acl_file);
    free(list->acl_path);
  }
  free(list);
  return list;
}

int whitelist_reload_from_file(whitelist * list) {
  char * line;
  char ch;
  int position = 0;

  if(NULL != list->files) {
    free(list->files);
  }

  fseek(list->acl_file, 0L, SEEK_SET);
  list->files_count = 0;
  list->files = (char**)malloc(sizeof(char*) * 10000);

  while(!feof(list->acl_file)) {
    line = (char*)malloc(MAX_LINE_LEN * sizeof(char));
    while((ch = getc(list->acl_file)) &&  ch != '\n' && ch != EOF) {
      line[position] = ch;
      ++position;
    }
    line[position] = 0;
    position = 0;
    list->files[list->files_count] = line;
    ++(list->files_count);
  }

  return 0;
}

int whitelist_append(whitelist * list, const char * path) {
  fseek(list->acl_file, 0L, SEEK_END);
  int res = fputs(path, list->acl_file) + fputs("\n", list->acl_file);
  whitelist_reload_from_file(list);
  return res == strlen(path);
}

int whitelist_allows(whitelist * list, const char * path) {
  int i;
  int val = 1000;

  for(i = 0; i < list->files_count; ++i) {
    char * allowed_path = list->files[i];

    if(0 == (val = strcmp(allowed_path, path))) { // path allowed
      // Path allowed
      return 1;
    }
    // else { Comparison failed, jumping to next line }
  }
  // Path not found
  return 0;
}

