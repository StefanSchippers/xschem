/*
    scconfig - lua lib detection
    Copyright (C) 2018  Tibor Palinkas

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

int find_script_fungw(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <libfungw/fungw.h>"
		NL "int main() {"
		NL "	fgw_ctx_t ctx;"
		NL "	fgw_obj_t *obj;"
		NL "	fgw_init(&ctx, \"host\");"
		NL "	obj = fgw_obj_reg(&ctx, \"foo\");"
		NL "	if (obj != NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;


	require("cc/cc", logdepth, fatal);

	report("Checking for fungw... ");
	logprintf(logdepth, "find_fungw: trying to find fungw...\n");
	logdepth++;

	/* Look at some standard place */
	if (try_icl(logdepth, "libs/script/fungw", test_c, NULL, NULL, "-lfungw -lm -lgenht")) return 0;
	if (try_icl(logdepth, "libs/script/fungw", test_c, NULL, "-I/usr/local/include", "-L/usr/local/lib -lfungw -lm -lgenht")) return 0;

	return try_fail(logdepth, "libs/script/fungw");
}

int find_script_fungw_user_call_ctx(const char *name, int logdepth, int fatal)
{
	const char *lf, *cf, *inc;
	char *test_c =
		NL "#include <libfungw/fungw.h>"
		NL "int main() {"
		NL "	fgw_value_t val;"
		NL "	val.argv0.user_call_ctx = &val;"
		NL "	if (0) fgw_uvcall(NULL, NULL, NULL, \"funcname\", NULL);"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;


	require("cc/cc", logdepth, fatal);
	require("libs/script/fungw/presents", logdepth, fatal);

	report("Checking for fungw user_call_ctx... ");
	logprintf(logdepth, "find_fungw_user_call_ctx: trying to find fungw...\n");
	logdepth++;

	inc = get("libs/script/fungw/includes");
	lf  = get("libs/script/fungw/ldflags");
	cf  = get("libs/script/fungw/cflags");

	if (try_icl(logdepth, "libs/script/fungw/user_call_ctx", test_c, inc, cf, lf)) return 0;

	return try_fail(logdepth, "libs/script/fungw/user_call_ctx");
}

int find_script_fungw_cfg_pupdir(const char *name, int logdepth, int fatal)
{
	const char *lf, *cf;
	char *out;
	char *test_c =
		NL "#include <libfungw/fungw.h>"
		NL "int main() {"
		NL "	puts(FGW_CFG_PUPDIR);"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);
	require("libs/script/fungw/presents", logdepth, fatal);

	report("Checking for fungw configured pupdir... ");
	logprintf(logdepth, "find_fungw_cfg_pupdir: trying to find fungw pupdir...\n");
	logdepth++;

/*	inc = get("libs/script/fungw/includes");*/
	lf  = get("libs/script/fungw/ldflags");
	cf  = get("libs/script/fungw/cflags");

	if ((compile_run(logdepth, test_c, NULL, cf, lf, &out) == 0) && (out != NULL)) {
		char *end = strpbrk(out, "\r\n");
		if (end != NULL) *end = '\0';
		report("Found: %s\n", out);
		logprintf(logdepth+1, "found: %s\n", out);
		put("libs/script/fungw/cfg_pupdir/path", out);
		put("libs/script/fungw/cfg_pupdir/presents", strue);
		free(out);
		return 1;
	}
	report("Not found\n");
	logprintf(logdepth+1, "not found\n");
	put("libs/script/fungw/cfg_pupdir/presents", sfalse);
	return 0;
}


int find_script_fungw_all(const char *name, int logdepth, int fatal)
{
	if (require("libs/script/fungw/presents", logdepth, fatal) != 0)
		return 1;
	require("libs/script/fungw/user_call_ctx/*", logdepth, 0);
	return 0;
}