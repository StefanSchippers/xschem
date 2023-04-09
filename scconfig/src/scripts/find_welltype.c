/*
    scconfig - welltype lib detection
    Copyright (C) 2021  Tibor Palinkas

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

#include "scripts.h"
#include <unistd.h>

#define SAVE_VER(name) \
	next = ver; \
	while(*next != ' ') next++; \
	*next = '\0'; \
	put("libs/script/welltype/ver/" name, ver); \
	ver = next + 1; \
	while(*ver == ' ') ver++;

static int accept_wt_res(char *ver)
{
	char *next;

	if (strncmp(ver, "OK ", 3) == 0)
		return 0;

	ver += 3;
	SAVE_VER("compacted");
	SAVE_VER("major");
	SAVE_VER("minor");
	SAVE_VER("build");
	SAVE_VER("revsn");
	return 1;
}

int find_script_welltype(const char *name, int logdepth, int fatal)
{
	/* The test script compares runtime lib version to compile time header
	   version and accepts proposed compilation flags only if they match */
	char *test_c =
		NL "#include <stdio.h>"
		NL "#include <wtruntime/version.h>"
		NL "int main() {"
		NL "	wt_u32 ver = wtrGetSuiteVersion();"
		NL "	if (ver == WTR_VERSION)"
		NL "		printf(\"OK %lu %d %d %d %d \", ver, WT_GETMAJOR(ver), WT_GETMINOR(ver), WT_GETBUILD(ver), WT_GETREVSN(ver));"
		NL "	return 0;"
		NL "}"
		NL;


	require("cc/cc", logdepth, fatal);

	report("Checking for welltype... ");
	logprintf(logdepth, "find_welltype: trying to find welltype...\n");
	logdepth++;

	/* Look at some standard places */
	if (try_icl_(logdepth, "libs/script/welltype", test_c, NULL, NULL, "-lwtruntime", 1, accept_wt_res)) return 0;

	return try_fail(logdepth, "libs/script/welltype");
}

int find_script_wtc(const char *name, int logdepth, int fatal)
{
	char cmd[] = "wtc --version";
	char *out = NULL;
	int ret = 1;

	report("Checking for welltype wtc... ");
	logprintf(logdepth, "find_welltype wtc: trying to find welltype wtc...\n");
	logdepth++;

	run_shell(logdepth, cmd, &out);

	if ((out != NULL) && (strncmp(out, "Welltype compiler v", 19) == 0)) {
		put("libs/script/welltype/wtc/presents", strue);
		put("libs/script/welltype/wtc/cmd", "wtc");
		report("OK (wtc)\n");
		ret = 0;
	}
	else
		report("not found\n");

	free(out);
	return ret;
}
