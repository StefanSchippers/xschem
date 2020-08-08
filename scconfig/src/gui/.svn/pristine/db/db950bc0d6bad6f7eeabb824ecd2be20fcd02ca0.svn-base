/*
    scconfig - gui lib detection - gtk3
    Copyright (C) 2017  Alain Vigne

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

int find_gtk3(const char *name, int logdepth, int fatal, const char *call, const char *arg)
{
	const char *test_c =
		NL "#include <gtk/gtk.h>"
		NL
		NL "static void"
		NL "activate (GtkApplication* app,"
		NL "          gpointer        user_data)"
		NL "{"
		NL "  GtkWidget *window;"
		NL
		NL "  window = gtk_application_window_new (app);"
		NL "  gtk_window_set_title (GTK_WINDOW (window), \"Window\");"
		NL "  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);"
		NL "  gtk_widget_show_all (window);"
		NL "}"
		NL
		NL "int"
		NL "main (int    argc,"
		NL "      char **argv)"
		NL "{"
		NL "  GtkApplication *app;"
		NL "  int status;"
		NL
		NL "  app = gtk_application_new (\"org.gtk.example\", G_APPLICATION_FLAGS_NONE);"
		NL "  g_signal_connect (app, \"activate\", G_CALLBACK (activate), NULL);"
		NL "  status = g_application_run (G_APPLICATION (app), argc, argv);"
		NL "  g_object_unref (app);"
		NL
		NL "  return status;"
		NL "}"
		NL;
	const char *node = "libs/gui/gtk3";
	const char *pkgname = "gtk+-3.0";
	char *cflags;
	char *ldflags;
	(void) call;									/* not used */
	(void) arg;										/* not used */

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for gtk+3... ");
	logprintf(logdepth, "find_gtk3: running pkg-config...\n");
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
