/*
    scconfig - detection of standard library features
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"

static int try_bad(int logdepth, const char *test_c, char *cflags, char *ldflags)
{
	char *out = NULL;

	logprintf(logdepth, "trying signal (neg) with ldflags '%s'\n", ldflags == NULL ? get("cc/ldflags") : ldflags);
	if (compile_run(logdepth+1, test_c, NULL, cflags, ldflags, &out) == 0) {
		if (target_emu_fail(out) || (strncmp(out, "BAD", 3) == 0)) {
			free(out);
			return 0;
		}
		free(out);
	}
	return 1;
}


int find_signal_raise(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stdio.h>"
		NL "#include <signal.h>"
		NL "int main(int argc, char *argv[]) {"
		NL "	printf(\"OK\\n\");"
		NL "	if (argc == 16)"
		NL "		raise(1);"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for raise()... ");
	logprintf(logdepth, "find_signal_raise: trying to find raise()...\n");
	logdepth++;

	if (try_icl(logdepth, "signal/raise", test_c, NULL, NULL, NULL))
		return 0;
	return try_fail(logdepth, "signal/raise");
}


int find_signal_names(const char *rname, int logdepth, int fatal)
{
	char *test_c_exists =
		NL "#include <stdio.h>"
		NL "#include <signal.h>"
		NL "int main(int argc, char *argv[]) {"
		NL "	printf(\"OK\\n\");"
		NL "	if (argc == 16)"
		NL "		raise(%s);"
		NL "	return 0;"
		NL "}"
		NL;
	char *test_c_terms =
		NL "#include <stdio.h>"
		NL "#include <signal.h>"
		NL "int main() {"
		NL "	raise(%s);"
		NL "	printf(\"BAD\\n\");"
		NL "	return 0;"
		NL "}"
		NL;
	char test_c[256];
	const char *names[] = {"SIGINT", "SIGABRT", "SIGKILL", "SIGTERM", "SIGQUIT", "SIGHUP", "SIGFPE", "SIGSEGV", "SIGPIPE", NULL};
	const char **name;
	char path[256], *pathend;
	const char *prefix = "signal/names/";

	require("cc/cc", logdepth, fatal);
	require("signal/raise/*", logdepth, fatal);

	strcpy(path, prefix);
	pathend = path + strlen(prefix);

	for(name = names; *name != NULL; name++) {
		/* check whether it exists */
		report("Checking whether %s exists... ", *name);
		logprintf(logdepth, "find_signal_names: checking whether %s exists\n", *name);
		logdepth++;
		sprintf(test_c, test_c_exists, *name);
		strcpy(pathend, *name);
		if (!try_icl(logdepth, path, test_c, NULL, NULL, NULL)) {
			logdepth--;
			continue;
		}

		/* check whether it exists */
		logdepth--;
		report("Checking whether %s terminates... ", *name);
		logprintf(logdepth, "find_signal_names: checking whether %s terminates\n", *name);
		logdepth++;

		sprintf(test_c, test_c_terms, *name);
		sprintf(pathend, "%s/terminates", *name);
		if (try_bad(logdepth, test_c, NULL, "")) {
			put(path, strue);
			report("terminates\n");
		}
		else {
			report("does not terminate\n");
			put(path, sfalse);
		}
		logdepth--;
	}

/* to avoid redetection */
	put("signal/names/presents", strue);

	return 0;
}

