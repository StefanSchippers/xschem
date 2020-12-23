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

static int trydlc(int logdepth, const char *test_c_dlc, const char *cflagsf, const char *ldflagsf, const char *dlc)
{
	char *cflags, *ldflags;

	cflags = malloc(strlen(dlc) + 64);
	ldflags = malloc(strlen(dlc)*2 + 256);
	sprintf(cflags,  cflagsf, dlc);
	sprintf(ldflags, ldflagsf, dlc, dlc);
	if (try_icl(logdepth, NULL, test_c_dlc, NULL, cflags, ldflags)) {
		*cflags  = ' ';
		append("cc/cflags", cflags);
		put("libs/ldl", ldflags);
		put("libs/dl-compat", strue);
		report("OK (%s and %s)\n", cflags, ldflags);
		free(cflags);
		free(ldflags);
		return 1;
	}
	free(cflags);
	free(ldflags);
	return 0;
}

int find_lib_ldl(const char *name, int logdepth, int fatal)
{
	const char *ldl, *dlc;
	char *s;

	char *test_c =
		NL "#include <stdio.h>"
		NL "#include <dlfcn.h>"
		NL "#ifndef RTLD_NOW"
		NL "#define RTLD_NOW RTLD_LAZY" /* on old BSD and probably on SunOS */
		NL "#endif"
		NL "int main() {"
		NL "	void *handle;"
		NL "	handle = dlopen(\"/this file does not exist.\", RTLD_NOW);"
		NL "	if (handle == NULL) printf(\"OK\\n\");"
		NL "	return 0;"
		NL "}"
		NL;

	char *test_c_dlc =
		NL "#include <stdio.h>"
		NL "#include <dl-compat.h>"
		NL "int main() {"
		NL "	void *handle;"
		NL "	handle = dlopen(\"/this file does not exist.\", RTLD_NOW);"
		NL "	if (handle == NULL) printf(\"OK\\n\");"
		NL "	return 0;"
		NL "}"
		NL;

	s = (char *)get("libs/ldl/presents");
	if (s != NULL)
		return !istrue(s);

	require("cc/cc", logdepth, fatal);

	report("Checking for -ldl... ");
	logprintf(logdepth, "find_lib_ldl: trying to find ldl...\n");
	logdepth++;

	ldl = get("/arg/libs/ldl");
	if (ldl == NULL) {
		dlc = get("/arg/libs/dl-compat");

		if (dlc == NULL) {
		/* If dlc is not explicitly requested by the user, try standard
		   dl (see whether we need -ldl for dlopen()) */
			if (try_icl(logdepth, NULL, test_c, NULL, NULL, NULL)) {
				put("libs/ldl", "");
				put("libs/ldl/includes", "#include <dlfcn.h>\\n");
				put("libs/ldl/presents", strue);
				report("OK ()\n");
				return 0;
			}
			if (try_icl(logdepth, NULL, test_c, NULL, NULL, "-ldl")) {
				put("libs/ldl", "-ldl");
				put("libs/ldl/includes", "#include <dlfcn.h>\\n");
				put("libs/ldl/presents", strue);
				report("OK (-ldl)\n");
				return 0;
			}
		}
		/* try dl-compat (dl compatibility lib) */
		if (dlc != NULL) {
			/* test at user supplied dlc prefix:
			   - first assume the linker will find it
			   - next assume gcc and pass rpath to the linker
			   - finally try static linking */
			if (trydlc(logdepth, test_c_dlc, "-I%s/include", "-L%s/lib -ldl-compat\000%s", dlc)) {
				put("libs/ldl/includes", "#include <dl-compat.h>\\n");
				return 0;
			}
			if (trydlc(logdepth, test_c_dlc, "-I%s/include", "-L%s/lib -Wl,-rpath=%s/lib -ldl-compat", dlc)) {
				put("libs/ldl/includes", "#include <dl-compat.h>\\n");
				return 0;
			}
			if (trydlc(logdepth, test_c_dlc, "-I%s/include", "%s/lib/libdl-compat.a\000%s", dlc)) {
				put("libs/ldl/includes", "#include <dl-compat.h>\\n");
				return 0;
			}
		}
		else if (try_icl(logdepth, NULL, test_c_dlc, NULL, NULL, "-ldl-compat")) {
			/* check at normal system installation */
			put("libs/ldl", "-ldl-compat");
			put("libs/ldl/includes", "#include <dl-compat.h>\\n");
			put("libs/ldl/presents", strue);
			report("OK (-ldl-compat)\n");
			return 0;
		}
	}
	else {
		report("User provided... ");
		s = strclone(ldl);
		if (try_icl(logdepth, NULL, test_c, NULL, NULL, s)) {
			put("libs/ldl", ldl);
			put("libs/ldl/includes", "#include <dlfcn.h>\\n");
			put("libs/ldl/presents", strue);
			report("OK (%s)\n", ldl);
			free(s);
			return 0;
		}
		free(s);
	}
	put("libs/ldl/presents", sfalse);
	report("Not found\n");
	return 1;
}

int find_lib_LoadLibrary(const char *name, int logdepth, int fatal)
{
	/*char *s;*/

	char *test_c =
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	void *handle;"
		NL "	handle = LoadLibrary(\"/this file does not exist.\");"
		NL "	if (handle == NULL) printf(\"OK\\n\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for LoadLibrary... ");
	logprintf(logdepth, "find_lib_LoadLibrary: trying to find LoadLibrary...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/LoadLibrary", test_c, "#include <windows.h>", NULL, NULL))
		return 0;
	return try_fail(logdepth, "libs/LoadLibrary");
}

int find_lib_errno(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <errno.h>"
		NL "int main() {"
		NL "	errno = 0;"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL ;

	require("cc/cc", logdepth, fatal);

	report("Checking for errno.h... ");
	logprintf(logdepth, "find_lib_errno: trying to find errno...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/errno", test_c, NULL, NULL, NULL)) return 0;
	return try_fail(logdepth, "libs/errno");
}
