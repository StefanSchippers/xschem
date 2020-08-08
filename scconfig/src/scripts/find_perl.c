/*
    scconfig - perl lib detection
    Copyright (C) 2009  Tibor Palinkas

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

#include "scripts.h"

int find_script_perl(const char *name, int logdepth, int fatal)
{
	char *cflags, *ldflags, *s;
	int res;
	char *test_c =
		NL "#include <EXTERN.h>"
		NL "#include <perl.h>"
		NL "int main() {"
		NL "	PerlInterpreter *interp;"
		NL
		NL "	interp = perl_alloc();"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("sys/class", logdepth, fatal);
	require("cc/cc", logdepth, fatal);
	require("/internal/filelist/method", logdepth, fatal);

	report("Checking for perl... ");
	logprintf(logdepth, "find_perl: trying to find perl...\n");
	logdepth++;

	res = run(logdepth, "perl -MExtUtils::Embed -e ccopts", &cflags);
	if (res) {
		logprintf(logdepth, "perl executable not found or broken (%d) at cflags\n", res);
		report("FAILED (perl exec fail)\n");
		return 1;
	}

	res = run(logdepth, "perl -MExtUtils::Embed -e ldopts", &ldflags);
	if (res) {
		logprintf(logdepth, "perl executable not found or broken (%d) aat ldflags\n", res);
		report("FAILED (perl exec fail)\n");
		free(cflags);
		return 1;
	}

	/* workarounds for windows [TODO] */
	if (strcmp(get("sys/class"), "win32") == 0) {
		for(s = cflags; *s != '\0'; s++)
			if (*s == '\\') *s = '/';

		for(s = ldflags; *s != '\0'; s++)
			if (*s == '\\') *s = '/';

		/* TODO: do we need to remove double quotes as well? */
	}

	res = try_icl(logdepth, "libs/script/perl", test_c, NULL, strip(cflags), strip(ldflags));

	free(cflags);
	free(ldflags);

	if (res)
		return 0;

	return try_fail(logdepth, "libs/script/perl");
}
