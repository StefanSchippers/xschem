/*
    scconfig - m4 binary detection
    Copyright (C) 2015  Tibor Palinkas

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
#include <unistd.h>

static int test_m4(const char *name, int logdepth, int fatal, char *bin)
{
	char *script =
		NL "define(baz, foo)"
		NL "baz bar"
		NL;
	char *out, *cmd, *tmpf, *tmpf_esc;
	(void) fatal;  /* not used */

	tmpf = tempfile_dump(script, ".m4");
	tmpf_esc = shell_escape_dup(tmpf);
	cmd = str_concat(" ", bin, tmpf_esc, NULL);
	free(tmpf_esc);

	run(logdepth, cmd, &out);
	if (out != NULL) {
		char *s = out;

		while((*s == ' ') || (*s == '\r') || (*s == '\n'))
			s++;

		if (strncmp(s, "foo bar", 7) == 0) {
			unlink(tmpf);
			free(tmpf);
			free(out);
			report("found (%s)\n", bin);
			logprintf(logdepth, "found (%s)", bin);
			put("libs/script/m4/bin/presents", strue);
			put("libs/script/m4/bin/path", bin);
			return 1;
		}

		free(out);
	}

	unlink(tmpf);
	free(tmpf);
	free(cmd);
	return 0;
}

int find_script_m4(const char *name, int logdepth, int fatal)
{
	char *m4_paths[] = {
		"/usr/bin/m4",
		"/usr/local/bin/m4",
		"/bin/m4",
		"/usr/bin/gm4",
		"/usr/local/bin/gm4",
		"/bin/gm4",
		NULL
	};
	char **s;

	report("Checking for m4 binary... ");
	logprintf(logdepth, "find_m4: trying to find m4 binary...\n");
	logdepth++;

	for(s = m4_paths; *s != NULL; s++)
		if (test_m4(name, logdepth, fatal, *s))
			return 0;

	return try_fail(logdepth, "libs/script/m4/bin");
}
