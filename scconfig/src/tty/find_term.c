/*
    scconfig - tty lib detection - terminal related functionality
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

int find_tty_cfmakeraw(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/tty/cfmakeraw";
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	struct termios t;"
		NL no_implicit(void, "cfmakeraw", "cfmakeraw")
		NL "	cfmakeraw(&t);"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char **inc;
	const char *incs[] = {
		"#include <termios.h>",
		"#include <termios.h>\n#include <unistd.h>",
		"#define _DEFAULT_SOURCE\n#define _BSD_SOURCE\n#include <termios.h>",
		"#define _DEFAULT_SOURCE\n#define _BSD_SOURCE\n#include <termios.h>\n#include <unistd.h>",
		NULL
	};

	require("cc/cc", logdepth, fatal);

	report("Checking for cfmakeraw()... ");
	logprintf(logdepth, "find_tty_cfmakeraw: trying to find cfmakeraw()...\n");
	logdepth++;

	for (inc = incs; *inc != NULL; ++inc )
		if (try_icl(logdepth, key, test_c, *inc, NULL, NULL))
			return 0;

	return try_fail(logdepth, key);
}

int find_tty_cfsetspeed(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/tty/cfsetspeed";
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	struct termios t;"
		NL no_implicit(int, "cfsetspeed", "cfsetspeed")
		NL "	if (cfsetspeed(&t, B9600) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char **inc;
	const char *incs[] = {
		"#include <termios.h>",
		"#include <termios.h>\n#include <unistd.h>",
		"#define _DEFAULT_SOURCE\n#define _BSD_SOURCE\n#include <termios.h>",
		"#define _DEFAULT_SOURCE\n#define _BSD_SOURCE\n#include <termios.h>\n#include <unistd.h>",
		NULL
	};

	require("cc/cc", logdepth, fatal);

	report("Checking for cfsetspeed()... ");
	logprintf(logdepth, "find_tty_cfsetspeed: trying to find cfsetspeed()...\n");
	logdepth++;

	for (inc = incs; *inc != NULL; ++inc )
		if (try_icl(logdepth, key, test_c, *inc, NULL, NULL))
			return 0;

	return try_fail(logdepth, key);
}
