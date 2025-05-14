/*
    scconfig - tty lib detection - ncurses
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

int find_gpm(const char *name, int logdepth, int fatal)
{
	const char *node = "libs/tty/gpm";
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	Gpm_Connect conn;"
		NL "	conn.eventMask = ~GPM_MOVE;"
		NL "	conn.defaultMask = GPM_MOVE;"
		NL "	conn.minMod = conn.maxMod = 0;"
		NL "	Gpm_Open(&conn, 0);"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	if (require("cc/cc", logdepth, fatal))
		return 1;

	report("Checking for gpm... ");
	logprintf(logdepth, "find_gpm:\n");
	logdepth++;

	{
		char *cflags = NULL, *ldflags = NULL;
		/* rely on pkgconfig when available */
		if (run_pkg_config(logdepth, "gpm", &cflags, &ldflags) == 0) {
			if (try_icl(logdepth, node, test_c, "#include <gpm.h>", cflags, ldflags) != 0) {
				free(cflags);
				free(ldflags);
				return 0;
			}
		}
		free(cflags);
		free(ldflags);
	}

	{
		char **incs, *incs_arr[] = {"#include <gpm.h>", NULL } ;
		char **cflags,  *cflags_arr[]  = {"", NULL};
		char **ldflags, *ldflags_arr[] = {"-lgpm", NULL};

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

