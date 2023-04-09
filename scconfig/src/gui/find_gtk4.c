/*
    scconfig - gui lib detection - gtk4
    Copyright (C) 2017  Alain Vigne
    Copyright (C) 2021  Tibor 'Igor2' Palinkas

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
		Contact (email and IRC): http://igor2.repo.hu/contact.html
*/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"

static const char *node = "libs/gui/gtk4";
static const char *pkgname = "gtk4";

int find_gtk4(const char *name, int logdepth, int fatal, const char *call, const char *arg)
{
	const char *test_c =
		NL "#include <gtk/gtk.h>"
		NL
		NL "static void activate (GtkApplication* app, gpointer user_data)"
		NL "{"
		NL "  GtkWidget *window;"
		NL
		NL "  window = gtk_application_window_new(app);"
		NL "  gtk_window_set_title(GTK_WINDOW(window), \"Window\");"
		NL "  gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);"
		NL "  gtk_widget_show(window);"
		NL "}"
		NL
		NL "int main (int    argc, char **argv)"
		NL "{"
		NL "  GtkApplication *app;"
		NL "  int status;"
		NL
		NL "  app = gtk_application_new(\"org.gtk.example\", G_APPLICATION_FLAGS_NONE);"
		NL "  g_signal_connect(app, \"activate\", G_CALLBACK(activate), NULL);"
		NL "  status = g_application_run(G_APPLICATION(app), argc, argv);"
		NL "  g_object_unref(app);"
		NL
		NL "  return status;"
		NL "}"
		NL;
	char *cflags;
	char *ldflags;
	(void) call;									/* not used */
	(void) arg;										/* not used */

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for gtk-4... ");
	logprintf(logdepth, "find_gtk4: running pkg-config...\n");
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

int find_gtk4_modversion(const char *name, int logdepth, int fatal)
{
	if (run_pkg_config_modversion_db(logdepth, node, pkgname) != 0)
		return try_fail(logdepth, node);
	return 0;
}


int find_epoxy(const char *name, int logdepth, int fatal)
{
	static const char *node = "libs/gui/epoxy";
	static const char *pkgname = "epoxy";
	const char *test_c =
		NL "#include <stdio.h>"
		NL "#include <epoxy/gl.h>"
		NL "int main() {"
		NL "	const char *list = \"foo bar baz\";"
		NL "	if (epoxy_extension_in_string(list, \"bar\") && !epoxy_extension_in_string(list, \"foobar\"))"
		NL "		printf(\"OK\\n\");"
		NL "	return 0;"
		NL "}"
		NL;

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for epoxy... ");
	logprintf(logdepth, "find_epoxy: running pkg-config...\n");
	logdepth++;

	if (try_icl_pkg_config(logdepth, node, test_c, NULL, "epoxy", NULL))
		return 0;

	if (try_icl(logdepth, node, test_c, NULL, "", "-lepoxy"))
		return 0;

	return try_fail(logdepth, node);
}
