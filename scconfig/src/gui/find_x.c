/*
    scconfig - gui lib detection - lesstif
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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"


int find_xopendisplay(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <X11/Xlib.h>"
		NL "#include <stdlib.h>"
		NL "int main()"
		NL "{"
		NL "	Display *d = XOpenDisplay(NULL);"
		NL "	return 0;"
		NL "}"
		NL;

	const char *node = "libs/gui/xopendisplay";
	char **cflags,  *cflags_arr[]  = {"", "-I/opt/X11/include", "-I/usr/X11R6/include", NULL};
	char **ldflags, *ldflags_arr[] = {"-lX11", "-L/opt/X11/lib -lX11", "-L/usr/X11R6/lib -lX11", NULL};
	char **Lflags, *Lflags_arr[] =   {"",      "-L/opt/X11/lib",       "-L/usr/X11R6/lib", NULL};

	if (require("cc/cc", logdepth, fatal))
		return 1;

	report("Checking for XOpenDisplay... ");
	logprintf(logdepth, "find_xopendisplay:\n");
	logdepth++;

	for(cflags = cflags_arr; *cflags != NULL; cflags++) {
		for(ldflags = ldflags_arr, Lflags = Lflags_arr; *ldflags != NULL; ldflags++,Lflags++) {
			if (try_icl_norun(logdepth, node, test_c, NULL, *cflags, *ldflags) != 0) {
				put("libs/gui/xopendisplay/Lflags", *Lflags);
				return 0;
			}
		}
	}
	return try_fail(logdepth, node);
}



int find_xinerama(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <X11/Xlib.h>"
		NL "#include <stdlib.h>"
		NL "int main()"
		NL "{"
		NL "	Display *d = XOpenDisplay(NULL);"
		NL "	if (d != NULL)"
		NL "		XineramaIsActive(d);"
		NL "	return 0;"
		NL "}"
		NL;

	const char *node = "libs/gui/xinerama";
	char **cflags,  *cflags_arr[]  = {"", NULL};
	char **ldflags, *ldflags_arr[] = {"-lXinerama", NULL};
	const char *xincludes, *xcflags, *xldflags;

	if (require("cc/cc", logdepth, fatal))
		return 1;

	if (require("libs/gui/xopendisplay/*", logdepth, fatal))
		return 1;

	xincludes = get("libs/gui/xopendisplay/includes");
	xcflags = get("libs/gui/xopendisplay/cflags");
	xldflags = get("libs/gui/xopendisplay/ldflags");

	report("Checking for Xinerama... ");
	logprintf(logdepth, "find_xinerama:\n");
	logdepth++;

	for(cflags = cflags_arr; *cflags != NULL; cflags++) {
		for(ldflags = ldflags_arr; *ldflags != NULL; ldflags++) {
			if (try_icl_with_deps(logdepth, node, test_c, NULL, *cflags, *ldflags, xincludes, xcflags, xldflags, 0) != 0) {
				return 0;
			}
		}
	}
	return try_fail(logdepth, node);
}

int find_xrender(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <X11/Xlib.h>"
		NL "#include <X11/extensions/Xrender.h>"
		NL "#include <stdlib.h>"
		NL "int main()"
		NL "{"
		NL "	Display *d = XOpenDisplay(NULL);"
		NL "	XRenderFreePicture (d, 0);"
		NL "	return 0;"
		NL "}"
		NL;

	const char *node = "libs/gui/xrender";
	char **cflags,  *cflags_arr[]  = {"", NULL};
	char **ldflags, *ldflags_arr[] = {"-lXrender", NULL};
	const char *xincludes, *xcflags, *xldflags;

	if (require("cc/cc", logdepth, fatal))
		return 1;

	if (require("libs/gui/xopendisplay/*", logdepth, fatal))
		return 1;

	xincludes = get("libs/gui/xopendisplay/includes");
	xcflags = get("libs/gui/xopendisplay/cflags");
	xldflags = get("libs/gui/xopendisplay/ldflags");

	report("Checking for Xrender... ");
	logprintf(logdepth, "find_xrender:\n");
	logdepth++;

	for(cflags = cflags_arr; *cflags != NULL; cflags++) {
		for(ldflags = ldflags_arr; *ldflags != NULL; ldflags++) {
			if (try_icl_with_deps(logdepth, node, test_c, NULL, *cflags, *ldflags, xincludes, xcflags, xldflags, 0) != 0) {
				return 0;
			}
		}
	}
	return try_fail(logdepth, node);
}

int find_xcb(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "#include <string.h>"
		NL "#include <X11/Xlib-xcb.h>"
		NL "int main()"
		NL "{"
		NL "	char *host;"
		NL "	int display, screen;"
		NL "	if (xcb_parse_display(\"ford:42.8\", &host, &display, &screen) == 0) return 0;"
		NL "	if ((strcmp(host, \"ford\") == 0) && (display == 42) && (screen == 8))"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/xcb";
	char **cflags,  *cflags_arr[]  = {"", NULL};
	char **ldflags, *ldflags_arr[] = {"-lxcb", NULL};
	const char *xincludes, *xcflags, *xldflags;

	if (require("cc/cc", logdepth, fatal))
		return 1;

	if (require("libs/gui/xopendisplay/*", logdepth, fatal))
		return 1;

	xincludes = get("libs/gui/xopendisplay/includes");
	xcflags = get("libs/gui/xopendisplay/cflags");
	xldflags = get("libs/gui/xopendisplay/ldflags");

	report("Checking for xcb... ");
	logprintf(logdepth, "find_xcb:\n");
	logdepth++;

	for(cflags = cflags_arr; *cflags != NULL; cflags++) {
		for(ldflags = ldflags_arr; *ldflags != NULL; ldflags++) {
			if (try_icl_with_deps(logdepth, node, test_c, NULL, *cflags, *ldflags, xincludes, xcflags, xldflags, 1) != 0) {
				return 0;
			}
		}
	}
	return try_fail(logdepth, node);
}

int find_xgetxcbconnection(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "#include <string.h>"
		NL "#include <X11/Xlib-xcb.h>"
		NL "int main()"
		NL "{"
		NL "	XGetXCBConnection(NULL);"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/xgetxcbconnection";
	char **cflags,  *cflags_arr[]  = {"", NULL};
	char **ldflags, *ldflags_arr[] = {"-lX11-xcb", NULL};
	const char *xincludes, *xcflags, *xldflags;

	if (require("cc/cc", logdepth, fatal))
		return 1;

	if (require("libs/gui/xopendisplay/*", logdepth, fatal))
		return 1;

	xincludes = get("libs/gui/xopendisplay/includes");
	xcflags = get("libs/gui/xopendisplay/cflags");
	xldflags = get("libs/gui/xopendisplay/ldflags");

	report("Checking for xgetxcbconnection... ");
	logprintf(logdepth, "find_xgetxcbconnection:\n");
	logdepth++;

	for(cflags = cflags_arr; *cflags != NULL; cflags++) {
		for(ldflags = ldflags_arr; *ldflags != NULL; ldflags++) {
			if (try_icl_with_deps(logdepth, node, test_c, NULL, *cflags, *ldflags, xincludes, xcflags, xldflags, 0) != 0) {
				return 0;
			}
		}
	}
	return try_fail(logdepth, node);
}

int find_xcb_render(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "#include <string.h>"
		NL "#include <xcb/render.h>"
		NL "int main()"
		NL "{"
		NL "	xcb_render_query_pict_formats_formats(NULL);"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/xcb_render";
	char **cflags,  *cflags_arr[]  = {"", NULL};
	char **ldflags, *ldflags_arr[] = {"-lxcb-render", NULL};
	const char *xincludes, *xcflags, *xldflags;

	if (require("cc/cc", logdepth, fatal))
		return 1;

	if (require("libs/gui/xopendisplay/*", logdepth, fatal))
		return 1;

	xincludes = get("libs/gui/xopendisplay/includes");
	xcflags = get("libs/gui/xopendisplay/cflags");
	xldflags = get("libs/gui/xopendisplay/ldflags");

	report("Checking for xcb_render... ");
	logprintf(logdepth, "find_xcb_render:\n");
	logdepth++;

	for(cflags = cflags_arr; *cflags != NULL; cflags++) {
		for(ldflags = ldflags_arr; *ldflags != NULL; ldflags++) {
			if (try_icl_with_deps(logdepth, node, test_c, NULL, *cflags, *ldflags, xincludes, xcflags, xldflags, 0) != 0) {
				return 0;
			}
		}
	}
	return try_fail(logdepth, node);
}

int find_xpm(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "#include <X11/xpm.h>"
		NL "int main()"
		NL "{"
		NL "	if (XpmLibraryVersion() == XpmIncludeVersion)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/xpm";
	char **cflags,  *cflags_arr[]  = {"", NULL};
	char **ldflags, *ldflags_arr[] = {"-lXpm", NULL};
	const char *xincludes, *xcflags, *xldflags;

	if (require("cc/cc", logdepth, fatal))
		return 1;

	if (require("libs/gui/xopendisplay/*", logdepth, fatal))
		return 1;

	xincludes = get("libs/gui/xopendisplay/includes");
	xcflags = get("libs/gui/xopendisplay/cflags");
	xldflags = get("libs/gui/xopendisplay/ldflags");

	report("Checking for xpm... ");
	logprintf(logdepth, "find_xpm:\n");
	logdepth++;

	for(cflags = cflags_arr; *cflags != NULL; cflags++) {
		for(ldflags = ldflags_arr; *ldflags != NULL; ldflags++) {
			if (try_icl_with_deps(logdepth, node, test_c, NULL, *cflags, *ldflags, xincludes, xcflags, xldflags, 1) != 0) {
				return 0;
			}
		}
	}
	return try_fail(logdepth, node);
}
