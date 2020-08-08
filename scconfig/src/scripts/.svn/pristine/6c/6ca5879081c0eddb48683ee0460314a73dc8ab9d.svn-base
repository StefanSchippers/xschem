/*
    scconfig - duktape lib detection
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

int find_script_duktape(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <duktape.h>"
		NL "int main() {"
		NL "	duk_context *ctx = duk_create_heap_default();"
		NL "	if (ctx != NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;


	require("cc/cc", logdepth, fatal);

	report("Checking for duktape... ");
	logprintf(logdepth, "find_duktape: trying to find duktape...\n");
	logdepth++;

	/* Look at the standard place */
	if (try_icl(logdepth, "libs/script/duktape", test_c, NULL, NULL, "-lduktape -lm")) return 0;

	return try_fail(logdepth, "libs/script/duktape");
}
