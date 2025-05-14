/*
    scconfig - gui lib detection - lesstif
    Copyright (C) 2015,2021  Tibor Palinkas

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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"

int find_lesstif2(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <Xm/MainW.h>"
		NL "int main(int argc, char *argv[])"
		NL "{"
		NL "	XtAppContext context;"
		NL "	Display *dsp;"
		NL "	Colormap cmap;"
		NL "	XColor color;"
		NL "	Widget toplevel;"
		NL "	toplevel = XtAppInitialize(& context, \"\", NULL, 0, &argc, argv, NULL, NULL, 0);"
		NL "	XAllocColor(dsp, cmap, &color);"
		NL "	return toplevel != NULL;"
		NL "}"
		NL;

	const char *node = "libs/gui/lesstif2";
	char **cflags,  *cflags_arr[]  = {"", "-I/opt/X11/include", NULL};
	char **ldflags, *ldflags_arr[] = {"-lXm -lX11", "-lXm -lXt", "-L/opt/X11/lib -lXm -lXt  -lX11", NULL}; /* note: -lXt must be after -lXm else lesstif fails to init with runtime error */

	if (require("cc/cc", logdepth, fatal))
		return 1;

	report("Checking for lesstif2... ");
	logprintf(logdepth, "find_lesstif:\n");
	logdepth++;

	for(cflags = cflags_arr; *cflags != NULL; cflags++) {
		for(ldflags = ldflags_arr; *ldflags != NULL; ldflags++) {
			if (try_icl_norun(logdepth, node, test_c, NULL, *cflags, *ldflags) != 0) {
				return 0;
			}
		}
	}
	return try_fail(logdepth, node);

}

int find_lesstif2_exthi(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <Xm/MainW.h>"
		NL "int main(int argc, char *argv[])"
		NL "{"
		NL "	return XmEXTERNAL_HIGHLIGHT;"
		NL "}"
		NL;

	const char *node = "libs/gui/lesstif2/exthi";
	const char *cflags, *ldflags;

	if (require("cc/cc", logdepth, fatal) || require("libs/gui/lesstif2", logdepth, fatal))
		return 1;

	report("Checking for lesstif2 XmEXTERNAL_HIGHLIGHT... ");
	logprintf(logdepth, "find_lesstif_exthi:\n");
	logdepth++;

	cflags = get("libs/gui/lesstif2/cflags");
	ldflags = get("libs/gui/lesstif2/ldflags");

	if (try_icl_norun(logdepth, node, test_c, NULL, cflags, ldflags) != 0)
		return 0;

	return try_fail(logdepth, node);
}
