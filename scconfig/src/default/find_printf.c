/*
    scconfig - detection of printf-related features
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

static int tryx(int logdepth, const char *test_c, const char *trying, const char *expected)
{
	char *out = NULL;
	char buff[512];

	logprintf(logdepth, "trying '%s'\n", trying);
	sprintf(buff, test_c, trying, trying);
	if (compile_run(logdepth+1, buff, NULL, NULL, NULL, &out) == 0) {
		if (strncmp(out, expected, strlen(expected)) == 0) {
			free(out);
			return 1;
		}
		free(out);
	}
	return 0;
}

static int tryc(int logdepth, const char *test_c, const char *trying)
{
	char *out = NULL;
	char buff[512];
	char *spc, *end;

	logprintf(logdepth, "trying '%s'\n", trying);
	sprintf(buff, test_c, trying);
	if (compile_run(logdepth+1, buff, NULL, NULL, NULL, &out) == 0) {
		spc = str_chr(out, ' ');
		if (spc == NULL)
			return 0;
		*spc = '\0';
		spc++;
		end = str_chr(spc, ' ');
		if (end == NULL)
			return 0;
		*end = '\0';
		if (strcmp(out, spc) == 0) {
			free(out);
			put("libs/printf_ptrcast", trying);
			report("OK (%s)\n", trying);
			return 1;
		}
		free(out);
	}
	return 0;
}

int find_printf_x(const char *name, int logdepth, int fatal)
{
	const char *pfx;
	char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	printf(\"'%s%%x'/'%s%%x'\\n\", (size_t)0x1234, NULL);"
		NL "	return 0;"
		NL "}"
		NL;
	char *expected = "'0x1234'/'0x0'";

	require("cc/cc", logdepth, fatal);

	report("Checking for printf %%x prefix... ");
	logprintf(logdepth, "find_printf_x: trying to find printf %%x prefix...\n");
	logdepth++;

	pfx = get("/arg/libs/printf_x");
	if (pfx == NULL) {

		if (tryx(logdepth, test_c, "", expected)) {
			put("libs/printf_x", "");
			report("OK ()\n");
			return 0;
		}
		if (tryx(logdepth, test_c, "0x", expected)) {
			put("libs/printf_x", "0x");
			report("OK (0x)\n");
			return 0;
		}
	}
	else {
		report("User provided... ");
		if (tryx(logdepth, test_c, pfx, expected)) {
			put("libs/printf_x", pfx);
			report("OK (%s)\n", pfx);
			return 0;
		}
	}
	return 1;
}

int find_printf_ptrcast(const char *name, int logdepth, int fatal)
{
	const char *cast;
	char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	printf(\"%%d %%d \\n\", sizeof(void *), sizeof(%s));"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for printf %%x pointer cast... ");
	logprintf(logdepth, "find_printf_ptrcast: trying to find printf %%x pointer cast...\n");
	logdepth++;

	cast = get("/arg/libs/printf_ptrcast");
	if (cast == NULL) {
		if (tryc(logdepth, test_c, "unsigned int"))        return 0;
		if (tryc(logdepth, test_c, "unsigned long"))       return 0;
		if (tryc(logdepth, test_c, "unsigned long long"))  return 0;
	}
	else {
		report("User provided... ");
		if (tryc(logdepth, test_c, cast)) return 0;
	}
	return 1;
}

int find_snprintf(const char *name, int logdepth, int fatal)
{
	char *out;
	char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	char buff[9];"
		NL "	char *s = buff+2;"
		NL
		NL "	/* build a fence */"
		NL "	buff[0] = 0;"
		NL "	buff[1] = 65;"
		NL "	buff[7] = 66;"
		NL "	buff[8] = 0;"
		NL
		NL "	snprintf(s, 4, \"%d\", 123456);"
		NL "	if ((buff[0] == 0) && (buff[1] == 65) && (buff[7] == 65) && (buff[8] == 0))"
		NL "	printf(\"%s\\n\", s);"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for snprintf... ");
	logprintf(logdepth, "find_snprintf_works: trying to find snprintf...\n");
	logdepth++;
	logprintf(logdepth, "trying snprintf...\n");

	if (compile_run(logdepth+1, test_c, NULL, NULL, NULL, &out) == 0) {
		if (cross_blind) {
			put("libs/snprintf", strue);
			report("OK (can't check if safe)\n");
			free(out);
			return 0;
		}
		if (strcmp(out, "123")) {
			put("libs/snprintf", strue);
			put("libs/snprintf_safe", strue);
			report("OK (safe)\n");
			free(out);
			return 0;
		}
		if (strcmp(out, "1234")) {
			put("libs/snprintf", strue);
			put("libs/snprintf_safe", sfalse);
			report("OK (UNSAFE)\n");
			free(out);
			return 0;
		}
		free(out);
		report("not found (broken output).\n");
	}
	else {
		report("not found (no output).\n");
	}
	put("libs/snprintf", sfalse);
	return 1;
}

int find_dprintf(const char *name, int logdepth, int fatal)
{
	char *out;
	char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	dprintf(1, \"OK\\n\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for dprintf... ");
	logprintf(logdepth, "find_dprintf: trying to find dprintf...\n");
	logdepth++;
	logprintf(logdepth, "trying dprintf...\n");

	if ((compile_run(logdepth+1, test_c, NULL, NULL, NULL, &out) == 0) && (strcmp(out, "OK"))) {
		put("libs/dprintf", strue);
		report("found\n");
		free(out);
		return 0;
	}
	put("libs/dprintf", sfalse);
		report("not found\n");
	return 1;
}

int find_vdprintf(const char *name, int logdepth, int fatal)
{
	char *out;
	char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "#include <stdarg.h>"
		NL "void local_dprintf(int fd, const char *fmt, ...)"
		NL "{"
		NL "	va_list ap;"
		NL "	va_start(ap, fmt);"
		NL "	vdprintf(fd, fmt, ap);"
		NL "}"
		NL "int main() {"
		NL "	local_dprintf(1, \"OK\\n\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for vdprintf... ");
	logprintf(logdepth, "find_vdprintf: trying to find vdprintf...\n");
	logdepth++;
	logprintf(logdepth, "trying vdprintf...\n");

	if ((compile_run(logdepth+1, test_c, NULL, NULL, NULL, &out) == 0) && (strcmp(out, "OK"))) {
		put("libs/vdprintf", strue);
		report("found\n");
		free(out);
		return 0;
	}
	put("libs/vdprintf", sfalse);
		report("not found\n");
	return 1;
}

int find_vsnprintf(const char *name, int logdepth, int fatal)
{
	char *out;
	char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "#include <stdarg.h>"
		NL "void local_vsnprintf(char *s, int len, const char *fmt, ...)"
		NL "{"
		NL "	va_list ap;"
		NL "	va_start(ap, fmt);"
		NL "	vsnprintf(s, len, fmt, ap);"
		NL "}"
		NL "int main() {"
		NL "	char s[16];"
		NL "	*s = '\\0';"
		NL "	local_vsnprintf(s, 14, \"OK\\n\");"
		NL "	printf(\"%s\", s);"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for vsnprintf... ");
	logprintf(logdepth, "find_vsnprintf: trying to find vsnprintf...\n");
	logdepth++;
	logprintf(logdepth, "trying vsnprintf...\n");

	if ((compile_run(logdepth+1, test_c, NULL, NULL, NULL, &out) == 0) && (strcmp(out, "OK"))) {
		put("libs/vsnprintf", strue);
		report("found\n");
		free(out);
		return 0;
	}
	put("libs/vsnprintf", sfalse);
		report("not found\n");
	return 1;
}
