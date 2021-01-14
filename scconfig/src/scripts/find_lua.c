/*
    scconfig - lua lib detection
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

int find_script_lua(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <lua.h>"
		NL "#include <lauxlib.h>"
		NL "int main() {"
		NL "	lua_State *state;"
		NL "	state = luaL_newstate();"
		NL "	luaL_loadfile(state, \"nothing\");"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;


	require("cc/cc", logdepth, fatal);
	require("/internal/filelist/method", logdepth, fatal);

	report("Checking for lua... ");
	logprintf(logdepth, "find_lua: trying to find lua...\n");
	logdepth++;

	/* Look at some standard places */
	/* TODO: do we need -ldl? */
	if (try_icl(logdepth, "libs/script/lua", test_c, NULL, NULL, "-llua -llualib -lm")) return 0;

	/* lualib doesn't exist in lua 5.1.1 */
	if (try_icl(logdepth, "libs/script/lua", test_c, NULL, NULL, "-llua -lm")) return 0;

	/* OS specific include dir */
	if (try_icl(logdepth, "libs/script/lua", test_c, NULL, "-I/usr/include", "-llua -llualib -lm")) return 0;
	if (try_icl(logdepth, "libs/script/lua", test_c, NULL, "-I/usr/include/lua5.3", "-llua5.3 -lm")) return 0;
	if (try_icl(logdepth, "libs/script/lua", test_c, NULL, "-I/usr/include/lua5.2", "-llua5.2 -lm")) return 0;
	if (try_icl(logdepth, "libs/script/lua", test_c, NULL, "-I/usr/local/include", "-llua -llualib -lm")) return 0;
	if (try_icl(logdepth, "libs/script/lua", test_c, NULL, "-I/usr/include", "-llua -lm")) return 0;
	if (try_icl(logdepth, "libs/script/lua", test_c, NULL, "-I/usr/local/include", "-llua -lm")) return 0;

	/* This one is for OSX (by Bjarni) */
	if (try_icl(logdepth, "libs/script/lua", test_c, NULL, "-I/sw/include", "-llua -llualib -lm")) return 0;

	/* no luck - try to find by brute force, listing directory names */
	if (brute_force_include(logdepth, "lua", test_c, NULL, "/usr/include") ||
	    brute_force_include(logdepth, "lua", test_c, NULL, "/usr/local/include")) {
	    return 0;
	}

	return try_fail(logdepth, "libs/script/lua");
}
