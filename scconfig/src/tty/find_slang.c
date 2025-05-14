/*
    scconfig - tty lib detection - slang
    Copyright (C) 2017  Tibor Palinkas

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
    Foundation, Inc., 31 Milk Street, # 960789 Boston, MA 02196 USA

		Project page: http://repo.hu/projects/scconfig
		Contact (email and IRC): http://igor2.repo.hu/contact.html
*/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"

int find_slang(const char *name, int logdepth, int fatal)
{
	const char *node = "libs/tty/slang";
	const char *test_c =
		NL "#include <stdio.h>"
		NL "#include <slang.h>"
		NL "int main()"
		NL "{"
		NL "	if (SLtt_initialize(\"this should not work\") == 0)"
		NL "		puts(\"SLtt_initialize() does not fail on invalid term string\");"
		NL "	else if (SLtt_initialize(\"vt100\") != 0)"
		NL "		puts(\"SLtt_initialize() fails to init on vt100\");"
		NL "	else"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	if (require("cc/cc", logdepth, fatal))
		return 1;

	report("Checking for slang... ");
	logprintf(logdepth, "find_slang:\n");
	logdepth++;

	{
		char *cflags = NULL, *ldflags = NULL;
		/* rely on pkgconfig when available */
		if (run_pkg_config(logdepth, "slang", &cflags, &ldflags) == 0) {
			if (try_icl(logdepth, node, test_c, "#include <slang.h>", cflags, ldflags) != 0) {
				free(cflags);
				free(ldflags);
				return 0;
			}
		}
		free(cflags);
		free(ldflags);
	}

	{
		char **incs, *incs_arr[] = {"#include <slang.h>", "#include <slang/slang.h>", NULL } ;
		char **cflags,  *cflags_arr[]  = {"", NULL};
		char **ldflags, *ldflags_arr[] = {"-lslang", NULL};

		/* fall back on guessing */
		for(incs = incs_arr; *incs != NULL; incs++) {
			for(cflags = cflags_arr; *cflags != NULL; cflags++) {
				for(ldflags = ldflags_arr; *ldflags != NULL; ldflags++) {
					if (try_icl(logdepth, node, test_c, *incs, *cflags, *ldflags) != 0) {
						return 0;
					}
				}
			}
		}
	}

	return try_fail(logdepth, node);
}

