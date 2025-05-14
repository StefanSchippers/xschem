/*
    scconfig - tty lib detection - readline
    Copyright (C) 2017  Tibor Palinkas
    Copyright (C) 2018  Aron Barath

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

int find_tty_readline(const char *name, int logdepth, int fatal)
{
	const char *node = "libs/tty/readline";
	const char *test_c =
		NL "int my_puts(const char* s);"
		NL "int main()"
		NL "{"
		NL "	if (rl_set_prompt(\"x\") == 0)"
		NL "		my_puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL "#include <stdio.h>"
		NL "int my_puts(const char* s) {"
		NL "	puts(s);"
		NL "}"
		NL;
	const char **ldflag, *ldflags[] = {"-lreadline", NULL };
	const char **inc, *includes[] = {"#include <readline/readline.h>", "#include <stdio.h>\n#include <readline/readline.h>", NULL };

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for readline library... ");
	logprintf(logdepth, "find_tty_readline: check for readline library\n");
	logdepth++;

	for(ldflag = ldflags; *ldflag != NULL; ++ldflag) {
		for(inc = includes; *inc != NULL; ++inc) {
			if (try_icl(logdepth, node, test_c, *inc, NULL, *ldflag)) {
				return 0;
			}
		}
	}

	return try_fail(logdepth, node);
}

int find_tty_history(const char *name, int logdepth, int fatal)
{
	const char *node = "libs/tty/history";
	const char *test_c =
		NL "int my_puts(const char* s);"
		NL "int main()"
		NL "{"
		NL "	HIST_ENTRY **list;"
		NL "	using_history();"
		NL "	add_history(\"thing\");"
		NL "	list = history_list();"
		NL "	if (list != NULL && (*list) != NULL)"
		NL "		my_puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL "#include <stdio.h>"
		NL "int my_puts(const char* s) {"
		NL "	puts(s);"
		NL "}"
		NL;
	const char **ldflag, *ldflags[] = {"-lhistory", NULL };
	const char **inc, *includes[] = {"#include <readline/history.h>", "#include <stdio.h>\n#include <readline/history.h>", NULL };

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for history library... ");
	logprintf(logdepth, "find_tty_history: check for history library\n");
	logdepth++;

	for(ldflag = ldflags; *ldflag != NULL; ++ldflag) {
		for(inc = includes; *inc != NULL; ++inc) {
			if (try_icl(logdepth, node, test_c, *inc, NULL, *ldflag)) {
				return 0;
			}
		}
	}

	return try_fail(logdepth, node);
}
