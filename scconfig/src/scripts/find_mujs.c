/*
    scconfig - mujs lib detection
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

int find_script_mujs(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stdio.h>"
		NL "#include <mujs.h>"
		NL "int main() {"
		NL "	js_State *J = js_newstate(NULL, NULL, JS_STRICT);\n"
		NL "	if (J != NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;


	require("cc/cc", logdepth, fatal);

	report("Checking for mujs... ");
	logprintf(logdepth, "find_mujs: trying to find mujs...\n");
	logdepth++;

	/* Look at the standard place */
	if (try_icl(logdepth, "libs/script/mujs", test_c, NULL, NULL, "-lmujs -lm")) return 0;

	return try_fail(logdepth, "libs/script/mujs");
}

int find_script_mujs_isboolean(const char *name, int logdepth, int fatal)
{
	const char *inc, *cflags, *ldflags;
	char *test_c =
		NL "#include <stdio.h>"
		NL "#include <mujs.h>"
		NL "int main() {"
		NL "	if (JS_ISBOOLEAN != JS_ISSTRING)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;


	require("cc/cc", logdepth, fatal);
	require("libs/script/mujs", logdepth, fatal);

	report("Checking for mujs JS_ISBOOLEAN... ");
	logprintf(logdepth, "find_mujs: trying to find mujs JS_ISBOOLEAN...\n");
	logdepth++;

	inc = get("libs/script/mujs/includes");
	cflags = get("libs/script/mujs/cflags");
	ldflags = get("libs/script/mujs/ldflags");

	/* Look at the standard place */
	if (try_icl(logdepth, "libs/script/mujs_isboolean", test_c, inc, cflags, ldflags))
		return 0;

	return try_fail(logdepth, "libs/script/mujs_isboolean");
}
