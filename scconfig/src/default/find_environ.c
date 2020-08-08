/*
    scconfig - detection of environmental variable access features
    Copyright (C) 2014  Tibor Palinkas

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

int find_main_arg3(const char *name, int logdepth, int fatal)
{
	char *out;
	char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main(int argc, char *argv[], char *env[])"
		NL "{"
		NL "	char **e;"
		NL "	int cnt;"
		NL "	for(e = env, cnt = 0; *e != NULL; e++, cnt++) ;"
		NL "	printf(\"%d\\n\", cnt);"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for main() with 3 arguments... ");
	logprintf(logdepth, "find_main_3args: checking for main() with 3 arguments\n");
	if (compile_run(logdepth+1, test_c, NULL, NULL, NULL, &out) == 0) {
		if (atoi(out) > 1) {
			put("libs/env/main_3arg", strue);
			report("OK\n");
			free(out);
			return 0;
		}
		free(out);
		report("not found (broken output).\n");
	}
	else {
		report("not found (no output).\n");
	}
	put("libs/env/main_3arg", sfalse);
	return 1;
}

int find_environ(const char *name, int logdepth, int fatal)
{
	char *out;
	char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "extern char **environ;"
		NL "int main(int argc, char *argv[])"
		NL "{"
		NL "	char **e;"
		NL "	int cnt;"
		NL "	for(e = environ, cnt = 0; *e != NULL; e++, cnt++) ;"
		NL "	printf(\"%d\\n\", cnt);"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for extern environ... ");
	logprintf(logdepth, "find_environ: checking for extern environ\n");
	if (compile_run(logdepth+1, test_c, NULL, NULL, NULL, &out) == 0) {
		if (atoi(out) > 1) {
			put("libs/env/environ", strue);
			report("OK\n");
			free(out);
			return 0;
		}
		free(out);
		report("not found (broken output).\n");
	}
	else {
		report("not found (no output).\n");
	}
	put("libs/env/environ", sfalse);
	return 1;
}

int find_putenv(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main(int argc, char *argv[])"
		NL "{"
		NL "	putenv(\"SCCONFIG_TEST=bad\");"
		NL "	putenv(\"SCCONFIG_TEST=OK\");"
		NL "	printf(\"%s\\n\", getenv(\"SCCONFIG_TEST\"));"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for putenv()... ");
	logprintf(logdepth, "find_putenv: trying to find putenv...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/env/putenv", test_c, "", NULL, NULL))
		return 0;
	if (try_icl(logdepth, "libs/env/putenv", test_c, "#define _XOPEN_SOURCE", NULL, NULL))
		return 0;
	if (try_icl(logdepth, "libs/env/putenv", test_c, "#define _SVID_SOURCE", NULL, NULL))
		return 0;
	return try_fail(logdepth, "libs/env/putenv");
}


int find_setenv(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main(int argc, char *argv[])"
		NL "{"
		NL "	setenv(\"SCCONFIG_TEST\", \"bad\", 1);"
		NL "	setenv(\"SCCONFIG_TEST\", \"OK\", 1);"
		NL "	printf(\"%s\\n\", getenv(\"SCCONFIG_TEST\"));"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for setenv()... ");
	logprintf(logdepth, "find_setenv: trying to find setenv...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/env/setenv", test_c, "", NULL, NULL))
		return 0;
	if (try_icl(logdepth, "libs/env/setenv", test_c, "#define _BSD_SOURCE", NULL, NULL))
		return 0;
	if (try_icl(logdepth, "libs/env/setenv", test_c, "#define _POSIX_C_SOURCE 200112L", NULL, NULL))
		return 0;
	if (try_icl(logdepth, "libs/env/setenv", test_c, "#define _XOPEN_SOURCE 600", NULL, NULL))
		return 0;
	return try_fail(logdepth, "libs/env/setenv");
}

