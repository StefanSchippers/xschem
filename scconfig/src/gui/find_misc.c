/*
    scconfig - gui lib detection - misc libs
    Copyright (C) 2016  Tibor Palinkas

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

		Project page: http://repo.hu/projects/scconfig
		Contact via email: scconfig [at] igor2.repo.hu
*/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"

int find_libstroke(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stroke.h>"
		NL "int main()"
		NL "{"
		NL "	char msg[256];"
		NL "	int n;"
		NL "	stroke_init();"
		NL "	for(n = 1000; n < 2000; n+=123)"
		NL "		stroke_record(n, 1000);"
		NL "	stroke_trans(msg);"
		NL "	return !(atoi(msg) == 456);"
		NL "}"
		NL;

	const char *node = "libs/gui/libstroke";
	char **cflags,  *cflags_arr[]  = {"", NULL};
	char **ldflags, *ldflags_arr[] = {"-lstroke", NULL};

	if (require("cc/cc", logdepth, fatal))
		return 1;

	report("Checking for libstroke... ");
	logprintf(logdepth, "find_libstroke:\n");
	logdepth++;

	for(cflags = cflags_arr; *cflags != NULL; cflags++) {
		for(ldflags = ldflags_arr; *ldflags != NULL; ldflags++) {
			if (try_icl_norun(logdepth, node, test_c, NULL, *cflags, *ldflags) != 0) {
				return 0;
			}
		}
	}
	return try_fail(logdepth, node);
}

