/*
    scconfig - gui lib detection - libsdl2
    Copyright (C) 2022  Tibor Palinkas

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
#include <string.h>
#include <assert.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"

static int accept_sdl2(char *stdout_str)
{
	int major = 0, minor = 0, patch = 0;
	char tmp[32];

	if ((stdout_str[0] != 'O') || (stdout_str[1] != 'K'))
		return 0;

	sscanf(stdout_str + 2, "%d %d %d", &major, &minor, &patch);
	sprintf(tmp, "%d", major); put("libs/gui/sdl2/ver/major", tmp);
	sprintf(tmp, "%d", minor); put("libs/gui/sdl2/ver/minor", tmp);
	sprintf(tmp, "%d", patch);   put("libs/gui/sdl2/ver/patch", tmp);

	return 1;
}


int find_sdl2(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <SDL.h>"
		NL "#include <stdlib.h>"
		NL "int main()"
		NL "{"
		NL "	SDL_version ver = {0};"
		NL "	SDL_GetVersion(&ver);"
		NL "	if (ver.major >= 2)"
		NL "		printf(\"OK %d %d %d\\n\", ver.major, ver.minor, ver.patch);"
		NL "	return 0;"
		NL "}"
		NL;

	const char *node = "libs/gui/sdl2";
	char **cflags,  *cflags_arr[]  = {"-I/usr/include/SDL2", NULL};
	char **ldflags, *ldflags_arr[] = {"-lSDL2", NULL};

	if (require("cc/cc", logdepth, fatal))
		return 1;

	report("Checking for sdl2... ");
	logprintf(logdepth, "find_sdl2:\n");
	logdepth++;

	{
		char *cflags, *ldflags;
		if (run_pkg_config(logdepth, "sdl2", &cflags, &ldflags) == 0)
			if (try_icl_(logdepth, node, test_c, NULL, cflags, ldflags, 1, accept_sdl2) != 0)
				return 0;
	}

	for(cflags = cflags_arr, ldflags = ldflags_arr; *cflags != NULL; cflags++, ldflags++) {
		if (try_icl_(logdepth, node, test_c, NULL, *cflags, *ldflags, 1, accept_sdl2) != 0)
			return 0;
	}

	return try_fail(logdepth, node);
}

int find_sdl2_gfx(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <SDL2_gfxPrimitives.h>"
		NL "#include <stdlib.h>"
		NL "int main()"
		NL "{"
		NL "	aalineColor(NULL, 1, 2, 3, 4, 100);"
		NL "	return 0;"
		NL "}"
		NL;
	char *cflags, *ldflags;
	const char *ocf, *olf;
	const char *node = "libs/gui/sdl2_gfx";
	int succ;

	if (require("cc/cc", logdepth, fatal))
		return 1;

	report("Checking for sdl2_gfx... ");
	logprintf(logdepth, "find_sdl2_gfx:\n");
	logdepth++;

	if (run_pkg_config(logdepth, "SDL2_gfx", &cflags, &ldflags) == 0)
		if (try_icl_norun(logdepth, node, test_c, NULL, cflags, ldflags) != 0)
			return 0;

	/* extend SDL2's cflags and ldflags */
	if (require("libs/gui/sdl2/cflags", logdepth, fatal))
		return try_fail(logdepth, node);
	if (require("libs/gui/sdl2/ldflags", logdepth, fatal))
		return try_fail(logdepth, node);

	ocf = get("libs/gui/sdl2/cflags");
	olf = get("libs/gui/sdl2/ldflags");

	cflags = (char *)ocf;
	ldflags = str_concat(" ", "-lSDL2_gfx", olf, NULL);
	succ = try_icl_norun(logdepth, node, test_c, NULL, cflags, ldflags);
	free(ldflags);

	if (succ != 0)
		return 0;

	return try_fail(logdepth, node);
}

int find_sdl2_ttf(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <SDL_ttf.h>"
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	if (TTF_Init() == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	char *cflags, *ldflags;
	const char *ocf, *olf;
	const char *node = "libs/gui/sdl2_ttf";
	int succ;

	if (require("cc/cc", logdepth, fatal))
		return 1;

	report("Checking for sdl2_ttf... ");
	logprintf(logdepth, "find_sdl2_ttf:\n");
	logdepth++;

	if (run_pkg_config(logdepth, "SDL2_ttf", &cflags, &ldflags) == 0)
		if (try_icl(logdepth, node, test_c, NULL, cflags, ldflags) != 0)
			return 0;

	/* extend SDL2's cflags and ldflags */
	if (require("libs/gui/sdl2/cflags", logdepth, fatal))
		return try_fail(logdepth, node);
	if (require("libs/gui/sdl2/ldflags", logdepth, fatal))
		return try_fail(logdepth, node);

	ocf = get("libs/gui/sdl2/cflags");
	olf = get("libs/gui/sdl2/ldflags");

	cflags = (char *)ocf;
	ldflags = str_concat(" ", "-lSDL2_ttf", olf, NULL);
	succ = try_icl(logdepth, node, test_c, NULL, cflags, ldflags);
	free(ldflags);

	if (succ != 0)
		return 0;

	return try_fail(logdepth, node);
}
