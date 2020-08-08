/*
    scconfig - gui lib detection - gtk2
    Copyright (C) 2013  Tibor Palinkas

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

static const char *node = "libs/gui/gtk2";
static const char *nodegl = "libs/gui/gtk2gl";
static const char *pkgname = "gtk+-2.0";
static const char *pkgnamegl = "gtkglext-x11-1.0";

int find_gtk2(const char *name, int logdepth, int fatal, const char *call, const char *arg)
{
	const char *test_c =
		NL "#include <gtk/gtk.h>"
		NL "#include <stdlib.h>"
		NL
		NL "int main(int argc, char *argv[])"
		NL "{"
		NL "	GtkWidget* mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);"
		NL "	gtk_window_set_default_size(GTK_WINDOW(mainWindow), 500, 500);"
		NL "	gtk_window_set_title(GTK_WINDOW(mainWindow), \"foobar\");"
		NL "	gtk_widget_show_all(mainWindow);"
		NL "	gtk_main();"
		NL "	return EXIT_SUCCESS;"
		NL "}"
		NL;
	char *cflags;
	char *ldflags;
	(void) call;									/* not used */
	(void) arg;										/* not used */

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for gtk+2... ");
	logprintf(logdepth, "find_gtk2: running pkg-config...\n");
	logdepth++;

	if (run_pkg_config(logdepth, pkgname, &cflags, &ldflags) != 0) {
		return try_fail(logdepth, node);
	}

	if (try_icl_norun(logdepth, node, test_c, NULL, cflags, ldflags) == 0) {
		free(cflags);
		free(ldflags);
		return try_fail(logdepth, node);
	}

	free(cflags);
	free(ldflags);
	return 0;
}

int find_gtk2gl(const char *name, int logdepth, int fatal, const char *call, const char *arg)
{
	const char *test_c =
		NL "#include <gtk/gtk.h>"
		NL "#include <gtk/gtkgl.h>"
		NL "#include <stdlib.h>"
		NL
		NL "int main(int argc, char *argv[])"
		NL "{"
		NL "	gtk_gl_init(&argc, &argv);"
		NL "	return EXIT_SUCCESS;"
		NL "}"
		NL;
	char *cflags;
	char *ldflags;
	(void) call;									/* not used */
	(void) arg;										/* not used */

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, nodegl);

	report("Checking for gtk+2 with GL... ");
	logprintf(logdepth, "find_gtk2gl: running pkg-config...\n");
	logdepth++;

	if (run_pkg_config(logdepth, pkgnamegl, &cflags, &ldflags) != 0) {
		return try_fail(logdepth, nodegl);
	}

	if (try_icl_norun(logdepth, nodegl, test_c, NULL, cflags, ldflags) == 0) {
		free(cflags);
		free(ldflags);
		return try_fail(logdepth, nodegl);
	}

	free(cflags);
	free(ldflags);
	return 0;
}

int find_gtk2_key_prefix(const char *name, int logdepth, int fatal)
{
	const char *node = "libs/gui/gtk2/key_prefix";
	char test_c[512];
	const char *test_c_ =
		NL "#include <gdk/gdkkeysyms.h>"
		NL "#include <stdlib.h>"
		NL
		NL "int main(int argc, char *argv[])"
		NL "{"
		NL "	if (%s != 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *includes, *cflags, *ldflags;


	if (require("libs/gui/gtk2/cflags", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for gtk+2 key prefix... ");
	logprintf(logdepth, "Checking for gtk+2 key prefix...\n");
	logdepth++;

	includes = get("libs/gui/gtk2/includes");
	cflags = get("libs/gui/gtk2/cflags");
	ldflags = get("libs/gui/gtk2/ldflags");

	sprintf(test_c, test_c_, "GDK_KEY_Tab");
	if (try_icl(logdepth, node, test_c, includes, cflags, ldflags)) {
		report("GDK_KEY_\n");
		put(node, "GDK_KEY_");
		return 0;
	}

	sprintf(test_c, test_c_, "GDK_Tab");
	if (try_icl(logdepth, node, test_c, includes, cflags, ldflags)) {
		report("GDK_\n");
		put(node, "GDK_");
		return 0;
	}

	return try_fail(logdepth, node);
}

int find_gtk2_modversion(const char *name, int logdepth, int fatal)
{
	if (run_pkg_config_modversion_db(logdepth, node, pkgname) != 0)
		return try_fail(logdepth, node);
	return 0;
}


