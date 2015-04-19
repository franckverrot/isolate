# isolate - Prevent processes from accessing specific resources
# Copyright (C) 2015 Franck Verrot <franck@verrot.fr>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

CC=gcc

all: test

UNAME=$(shell uname)

ifeq ($(UNAME),Linux)
	OPTS=
	LIBISOLATE_FILENAME=libisolate.so
endif

ifeq ($(UNAME),Darwin)
	OPTS=-dynamiclib
	LIBISOLATE_FILENAME=libisolate.dylib
endif

libisolate:
	$(CC) -Wall -shared $(OPTS) -o $(LIBISOLATE_FILENAME) libisolate.c whitelist.c -I. -ldl -fPIC

clean:
	rm -rf tmp
	mkdir tmp

test: clean libisolate
	./test/test.sh

install: libisolate
	ln -s $LIBISOLATE_FILENAME /usr/lib/
	cp isolate /usr/local/bin

uninstall: libisolate
	rm -f /usr/lib/$LIBISOLATE_FILENAME /usr/local/bin/isolate
