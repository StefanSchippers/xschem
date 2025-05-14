/*
    scconfig - picol detection
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
    Foundation, Inc., 31 Milk Street, # 960789 Boston, MA 02196 USA

		Project page: http://repo.hu/projects/scconfig
		Contact (email and IRC): http://igor2.repo.hu/contact.html
*/

#include "scripts.h"
#include <unistd.h>

int find_script_picol(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#define PICOL_IMPLEMENTATION"
		NL "#include \"picol.h\""
		NL "int main() {"
		NL "	picolInterp *interp = picolCreateInterp();"
		NL "	picolEval(interp, \"puts {OK}\");"
		NL "	picolFreeInterp(interp);"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for picol... ");
	logprintf(logdepth, "find_picol: trying to find picol...\n");
	logdepth++;

	/* Look at the standard places: /usr/include and /usr/local/include and whatever the C compiler sees fit */
	if (try_icl(logdepth, "libs/script/picol", test_c, NULL, NULL, "")) return 0;

	return try_fail(logdepth, "libs/script/picol");
}
