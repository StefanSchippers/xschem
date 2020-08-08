/*
    scconfig - stutter lib detection
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
#include <unistd.h>


int find_script_stutter(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stutter.h>"
		NL "int main() {"
		NL "	varctx *global;"
		NL "	global = varctx_create(NULL, 256);"
		NL
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for stutter... ");
	logprintf(logdepth, "find_stutter: trying to find stutter...\n");
	logdepth++;

	/* TODO: do we need -ldl? */
	if (
			try_icl(logdepth, "libs/script/stutter", test_c, NULL, NULL, "-lstutter") ||
			try_icl(logdepth, "libs/script/stutter", test_c, NULL, "-I/usr/include/stutter", "-lstutter") ||
			try_icl(logdepth, "libs/script/stutter", test_c, NULL, "-I/usr/local/include/stutter", "-lstutter")
			)
		return 0;

	return try_fail(logdepth, "libs/script/stutter");
}

int find_script_estutter(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <estutter/stutter.h>"
		NL "int main() {"
		NL "	stt_ctx_t stt;"
		NL "	stt.next_serial = 4242;"
		NL "	stt_init(&stt);"
		NL "	if (stt.next_serial != 4242)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for estutter... ");
	logprintf(logdepth, "find_estutter: trying to find estutter...\n");
	logdepth++;

	/* TODO: do we need -ldl? */
	if (
			try_icl(logdepth, "libs/script/estutter", test_c, NULL, NULL, "-lestutter") ||
			try_icl(logdepth, "libs/script/estutter", test_c, NULL, "-I/usr/include/estutter", "-lestutter") ||
			try_icl(logdepth, "libs/script/estutter", test_c, NULL, "-I/usr/local/include/estutter", "-lestutter")
			)
		return 0;

	return try_fail(logdepth, "libs/script/estutter");
}
