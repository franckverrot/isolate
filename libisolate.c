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

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <fcntl.h>
#include "whitelist.h"

#ifndef RTLD_NEXT
#define RTLD_NEXT ((void*)(-1l))
#endif

#define DEFAULT_ACL_PATH     ".access-control"
#define ACL_PATH_ENV_NAME    "ACL_PATH"
#define ACL_ISOLATE_ENV_NAME "ACL_INTERACTIVE"
#define EMPTY_STRING(n) (0 == strncmp(n, "", 1))
#define read_flags(flags)  (O_RDONLY == (flags & O_RDONLY)) ? 'y' : 'n'
#define write_flags(flags) (O_WRONLY == (flags & O_WRONLY)) ? 'y' : 'n'
#define read_write_flags(flags)  (O_RDWR   == (flags & O_RDWR)) ? 'y' : 'n'

typedef enum {
    NONE,
    NON_INTERACTIVE,
    INTERACTIVE
} imode;

typedef enum {
  UNSPECIFIED,
  NO_ASK,
  ASK
} user_ask;

static whitelist * list         = NULL;
static user_ask should_ask_user = UNSPECIFIED;
static imode interactive_mode   = NONE;
static char choice;

void retrieve_interactive_mode() {
  if(interactive_mode == NONE) {
    const char * env_value = getenv(ACL_ISOLATE_ENV_NAME);
    int parsed_value = 0;

    sscanf(env_value, "%d", &parsed_value);
    switch(parsed_value) {
      case 1:
        interactive_mode = INTERACTIVE;
        break;
      case 0:
      default:
        interactive_mode = NON_INTERACTIVE;
    }
  }
}

void check_wl(const char *pathname, const char *details) {
  retrieve_interactive_mode();

  if(!list) {
    const char * path = getenv(ACL_PATH_ENV_NAME);
    if(EMPTY_STRING(path)) {
      path = DEFAULT_ACL_PATH;
    }

    list = whitelist_new(path);
    whitelist_reload_from_file(list);
  }

  if((0 != whitelist_allows(list, pathname))) {
    // The file has been added to the white-list
  } else {
    if(interactive_mode == INTERACTIVE) {
      if(UNSPECIFIED == should_ask_user) {
        fprintf(stderr, "[isolate] grant access to %s (%s) ? [*(all files),a(lways),y(es)/N(o)] ", pathname, details);
        choice = getchar();
        getchar();
        puts("");
      } else {
        choice = 'a';
      }

      switch(choice) {
        case '*':
          should_ask_user = NO_ASK;
        case 'a':
        case 'A':
          // Remember this file
          whitelist_append(list, pathname);
          break;
        case 'y':
        case 'Y':
          // Allow one time access
          break;
        default:
          fprintf(stderr, "[isolate] Permission denied!\n");
          exit(1);
      }
    } else {
      fprintf(stderr, "[isolate] Permission denied!\n");
      exit(1);
    }
  }
}

int unlink(const char *path) {
  check_wl(path, "remove");

  int (*real_unlink_with_mode)(const char *);

  real_unlink_with_mode = dlsym(RTLD_NEXT, "unlink");
  return (*real_unlink_with_mode)(path);
}

int unlinkat(int dirfd, const char *pathname, int flags) {
  check_wl(pathname, "remove");

  int (*real_unlink_with_mode)(int, const char *, int);

  real_unlink_with_mode = dlsym(RTLD_NEXT, "unlinkat");
  return (*real_unlink_with_mode)(dirfd, pathname, flags);
}

int open(const char *pathname, int flags, ...) {
  va_list ap;
  mode_t mode;
  char * details;

  asprintf(&details, "Read: %c | Write: %c", read_flags(flags), write_flags(flags));
  check_wl(pathname, details);

  va_start(ap, flags);
  mode = (mode_t)va_arg(ap, int);
  va_end(ap);

  if(0 == mode) {
    int (*real_open)(const char *, int);

    real_open = dlsym(RTLD_NEXT, "open");
    return (*real_open)(pathname, flags);
  } else {
    int (*real_open_with_mode)(const char *, int, mode_t);

    real_open_with_mode = dlsym(RTLD_NEXT, "open");
    return (*real_open_with_mode)(pathname, flags, mode);
  }
}
