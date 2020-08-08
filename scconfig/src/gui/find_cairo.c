/*
    scconfig - gui lib detection - cairo
    Copyright (C) 2017  Tibor Palinkas

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

int find_cairo(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "#include \"cairo.h\""
		NL "int main() {"
		NL "	cairo_t *ctx;"
		NL "	cairo_surface_t *surf;"
		NL
		NL "	if (cairo_image_surface_create(CAIRO_FORMAT_RGB24, 100, 100) != NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/cairo";
	char *cflags = NULL;
	char *ldflags = NULL;

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for cairo... ");
	logprintf(logdepth, "find_cairo: running pkg-config...\n");
	logdepth++;

	if (run_pkg_config(logdepth, "cairo", &cflags, &ldflags) == 0) {
		if (try_icl(logdepth, node, test_c, NULL, cflags, ldflags) != 0)
			goto success;
	}

	return try_fail(logdepth, node);

	success:;
	if (cflags != NULL)
		free(cflags);
	if (ldflags != NULL)
		free(ldflags);
	return 0;
}


int find_cairo_xcb(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	if (cairo_xcb_device_debug_get_precision(NULL) == -1)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/cairo-xcb", *cflags, *ldflags;

	if (require("libs/gui/cairo", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for cairo-xcb... ");
	logprintf(logdepth, "find_cairo-xcb: \n");
	logdepth++;

	cflags = get("libs/gui/cairo/cflags");
	ldflags = get("libs/gui/cairo/ldflags");

	if (try_icl(logdepth, node, test_c, "#include <cairo-xcb.h>", cflags, ldflags) != 0)
		return 0;

	return try_fail(logdepth, node);
}
