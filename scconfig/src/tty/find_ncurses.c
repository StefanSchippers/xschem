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
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

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

/* Need to look for a unique string because ncurses may write arbitrary
   terminal control sequences to stdout during initialization */

#define SAFE_OK "{{{scconfig:OK}}}"

int end_ok(char *stdout_str)
{
	return (strstr(stdout_str, SAFE_OK) != NULL);
}

static const char *nucrses_test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	WINDOW *mainwin = initscr();"
		NL "	endwin();"
		NL "	if (mainwin != NULL)"
		NL "		puts(\"" SAFE_OK "\");"
		NL "	return 0;"
		NL "}"
		NL;


int find_ncurses(const char *name, int logdepth, int fatal)
{
	const char *node = "libs/tty/ncurses";
	const char *inc = "#include <curses.h>";

	if (require("cc/cc", logdepth, fatal))
		return 1;

	report("Checking for ncurses... ");
	logprintf(logdepth, "find_ncurses:\n");
	logdepth++;

	{
		char *cflags = NULL, *ldflags = NULL;
		/* rely on pkgconfig when available */
		if (run_pkg_config(logdepth, "ncurses", &cflags, &ldflags) == 0) {
			if (try_icl_(logdepth, node, nucrses_test_c, inc, cflags, ldflags, 1, end_ok) != 0) {
				free(cflags);
				free(ldflags);
				return 0;
			}
		}
		free(cflags);
		free(ldflags);
	}

	{
		char **cflags,  *cflags_arr[]  = {"", NULL};
		char **ldflags, *ldflags_arr[] = {"-lncurses", NULL};

		/* fall back on guessing */
		for(cflags = cflags_arr; *cflags != NULL; cflags++) {
			for(ldflags = ldflags_arr; *ldflags != NULL; ldflags++) {
				if (try_icl_(logdepth, node, nucrses_test_c, inc, *cflags, *ldflags, 1, end_ok) != 0) {
					return 0;
				}
			}
		}
	}

	return try_fail(logdepth, node);
}

int find_ncursesw(const char *name, int logdepth, int fatal)
{
	const char *node = "libs/tty/ncursesw";
	const char *inc = "#include <ncursesw/curses.h>";

	if (require("cc/cc", logdepth, fatal))
		return 1;

	report("Checking for ncursesw... ");
	logprintf(logdepth, "find_ncursesw:\n");
	logdepth++;

	{
		char *cflags = NULL, *ldflags = NULL;
		/* rely on pkgconfig when available */
		if (run_pkg_config(logdepth, "ncursesw", &cflags, &ldflags) == 0) {
			if (try_icl_(logdepth, node, nucrses_test_c, inc, cflags, ldflags, 1, end_ok) != 0) {
				free(cflags);
				free(ldflags);
				return 0;
			}
		}
		free(cflags);
		free(ldflags);
	}

	{
		char **cflags,  *cflags_arr[]  = {"", NULL};
		char **ldflags, *ldflags_arr[] = {"-lncursesw", NULL};

		/* fall back on guessing */
		for(cflags = cflags_arr; *cflags != NULL; cflags++) {
			for(ldflags = ldflags_arr; *ldflags != NULL; ldflags++) {
				if (try_icl_(logdepth, node, nucrses_test_c, inc, *cflags, *ldflags, 1, end_ok) != 0) {
					return 0;
				}
			}
		}
	}

	return try_fail(logdepth, node);
}


int find_ncurses_escdelay(const char *name, int logdepth, int fatal)
{
	const char *node = "libs/tty/ncurses/escdelay";
	const char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	if (ESCDELAY >= 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	if (require("cc/cc", logdepth, fatal) || require("libs/tty/ncurses/presents", logdepth, fatal))
		return 1;

	report("Checking for ncurses ESCDELAY... ");
	logprintf(logdepth, "find_ncurses ESCDELAY:\n");
	logdepth++;

	if (try_icl(logdepth, node, test_c, get("libs/tty/ncurses/includes"), get("libs/tty/ncurses/cflags"), get("libs/tty/ncurses/ldflags")) != 0)
		return 0;

	return try_fail(logdepth, node);
}


int find_ncurses_cur_term(const char *name, int logdepth, int fatal)
{
	const char *node = "libs/tty/ncurses/cur_term";
	char *inc, *inc1;
	const char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	if (&cur_term != NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	if (require("cc/cc", logdepth, fatal) || require("libs/tty/ncurses/presents", logdepth, fatal))
		return 1;

	report("Checking for ncurses cur_term... ");
	logprintf(logdepth, "find_ncurses cur_term:\n");
	logdepth++;

	if (try_icl(logdepth, node, test_c, get("libs/tty/ncurses/includes"), get("libs/tty/ncurses/cflags"), get("libs/tty/ncurses/ldflags")) != 0)
		return 0;

	inc1 = str_subsn(get("libs/tty/ncurses/includes"));

	inc = str_concat("\n", inc1, "#include <ncurses/term.h>", NULL);
	if (try_icl(logdepth, node, test_c, inc, get("libs/tty/ncurses/cflags"), get("libs/tty/ncurses/ldflags")) != 0) {
		free(inc);
		free(inc1);
		return 0;
	}
	free(inc);

	inc = str_concat("\n", inc1, "#include <term.h>", NULL);
	if (try_icl(logdepth, node, test_c, inc, get("libs/tty/ncurses/cflags"), get("libs/tty/ncurses/ldflags")) != 0) {
		free(inc);
		free(inc1);
		return 0;
	}
	free(inc);

	free(inc1);
	return try_fail(logdepth, node);
}


int find_ncurses_resizeterm(const char *name, int logdepth, int fatal)
{
	const char *node = "libs/tty/ncurses/resizeterm";
	const char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	if (0) resizeterm(80, 25);"
		NL "	if (resizeterm != NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	if (require("cc/cc", logdepth, fatal) || require("libs/tty/ncurses/presents", logdepth, fatal))
		return 1;

	report("Checking for ncurses resizeterm... ");
	logprintf(logdepth, "find_ncurses resizeterm:\n");
	logdepth++;

	if (try_icl(logdepth, node, test_c, get("libs/tty/ncurses/includes"), get("libs/tty/ncurses/cflags"), get("libs/tty/ncurses/ldflags")) != 0)
		return 0;

	return try_fail(logdepth, node);
}

