/*
    scconfig - stutter lib detection
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

int find_script_funlisp(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <funlisp.h>"
		NL "int main() {"
		NL "	lisp_runtime *rt = lisp_runtime_new();"
		NL "	if (rt != NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for funlisp... ");
	logprintf(logdepth, "find_funlisp: trying to find funlisp...\n");
	logdepth++;

	if (
			try_icl(logdepth, "libs/script/funlisp", test_c, NULL, NULL, "/usr/local/lib/libfunlisp.a") ||
			try_icl(logdepth, "libs/script/funlisp", test_c, NULL, NULL, "/usr/lib/libfunlisp.a")
			)
		return 0;

	return try_fail(logdepth, "libs/script/funlisp");
}
