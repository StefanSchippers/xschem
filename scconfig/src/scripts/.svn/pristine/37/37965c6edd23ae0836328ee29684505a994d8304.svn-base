/*
    scconfig - ruby lib detection
    Copyright (C) 2015  Tibor Palinkas

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

int find_script_mruby(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <mruby.h>"
		NL "int main() {"
		NL "	mrb_state *ctx = mrb_open();"
		NL "	mrb_close(ctx);"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for mruby... ");
	logprintf(logdepth, "find_mruby: trying to find mruby...\n");
	logdepth++;

	if (try_icl_pkg_config(logdepth, "libs/script/mruby", test_c, NULL, "mruby-*", get("/arg/mruby-version")))
		return 0;

	/* Look at the most standard place */
	if (try_icl(logdepth, "libs/script/mruby", test_c, NULL, NULL, "-lmruby -lm"))
		return 0;

	return try_fail(logdepth, "libs/script/ruby");
}
