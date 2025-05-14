/*
    scconfig - tty lib detection - termios
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

int find_tty_ioctl(const char *name, int logdepth, int fatal)
{
	char node[128];
	char* nodeend;
	const char *test_c_template =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	%s buf;"
		NL "	if (sizeof(buf) > 0 && %s > 0) {"
		NL "		puts(\"OK\");"
		NL "		return 0;"
		NL "		}"
		NL "	return 1;"
		NL "}"
		NL;
	char test_c[1024];
	const char **ioc, *ioctls[] = {"TIOCGWINSZ",     "TCGETA",        "TIOCGETP",      NULL};
	const char **typ, *types[]  = {"struct winsize", "struct termio", "struct sgttyb", NULL};
	const char **inc, *includes[] = {"#include <sys/ioctl.h>", "#include <termios.h>\n#include <sys/ioctl.h>", "#include <termio.h>\n#include <sys/ioctl.h>", NULL } ;

	if (require("cc/cc", logdepth, fatal))
		return 1;

	strcpy(node, "libs/tty/ioctl/");
	nodeend = node + strlen(node);

	report("Checking for TTY ioctls...\n");
	logprintf(logdepth, "find_tty_ioctl:\n");
	logdepth++;

	for(ioc = ioctls, typ = types; *ioc != NULL; ++ioc, ++typ) {
		report("  %s... ", *ioc);
		strcpy(nodeend, *ioc);
		sprintf(test_c, test_c_template, *typ, *ioc);
		for(inc = includes; *inc != NULL; ++inc) {
			if (try_icl(logdepth, node, test_c, *inc, NULL, NULL)) {
				goto next_ioc;
			}
		}
		strcat(nodeend, "/presents");
		put(node, sfalse);
		report("not found\n");
		next_ioc:;
	}

	return 0;
}
