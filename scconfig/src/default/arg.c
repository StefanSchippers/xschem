/*
    scconfig - command line argument processing
    Copyright (C) 2009..2012  Tibor Palinkas

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
#include "db.h"
#include "arg.h"
#include "dep.h"
#include "log.h"
#include "libs.h"

argtbl_t main_argument_table[] = {
	{"import",      NULL,                   import_args,   "Import saved config (sub)tree"},
	{"target",      "/arg/sys/target",      NULL,          "set cross compilation target (prefix)"},
	{"target-name", "/arg/sys/target-name", NULL,          "set cross compilation target (system name)"},
	{"target-shell","/arg/sys/target-shell",NULL,          "set the shell on cross compilation target"},
	{"emu",         "/arg/sys/emu",         NULL,          "emulator for testing cross compiled executables with"},
	{"pkg-config",  "/arg/sys/pkg-config",  NULL,          "path to pkg-config to use"},
	{"pkg-config-zap","/arg/sys/pkg-config-zap",NULL,      "ignore pkg-config results by this regex pattern"},

/* wildcard rules for icl() control */
	{"^ldflags/",   NULL,                   import_icl,    "force LDFLAGS for a node"},
	{"^cflags/",    NULL,                   import_icl,    "force CFLAGS for a node"},
	{"^includes/",  NULL,                   import_icl,    "force #includes for a node"},
	{"^prefix/",    NULL,                   import_icl,    "force using prefix path for detecting the node"},

/* the followings are autoconf compatibility translations */
	{"CC",          "/arg/cc/cc",           NULL,          "Force using a C compiler (command line)"},
	{"CFLAGS",      "/arg/cc/cflags",       NULL,          "Force using a CFLAGS for C compilation"},
	{"LDFLAGS",     "/arg/cc/ldflags",      NULL,          "Force using a LDFLAGS for linking"},
	{"LDL",         "/arg/libs/ldl",        NULL,          "Force using -ldl string"},

	{"gpmi-prefix", "/arg/gpmi/prefix",     NULL,    NULL},
	{NULL,          NULL,                   NULL,    NULL}
};

void process_args(int argc, char *argv[])
{
	int n;
	char *key, *value;
	argtbl_t *a;
	int found, tainted = 0;

	db_mkdir("/arg");

	logprintf(0, "CLI arg 0: '%s'\n", argv[0]);
	for(n = 1; n < argc; n++) {
		key = argv[n];
		logprintf(0, "CLI arg %d: '%s'\n", n, key);
		while(*key == '-') key++;
		value = str_chr(key, '=');
		found = 0;

		if (value != NULL) {
			*value = '\0';
			value++;

			if (strcmp(key, "without") == 0) {
				char *tmp;
				if (*value != '/') {
					const char **r, *roots[] = {"target", "host", "runtime", NULL};
					for(r = roots; *r != NULL; r++) {
						tmp = str_concat("/", "/arg/without", *r, value, NULL);
						put(tmp, strue);
						free(tmp);
					}
				}
				else {
					tmp = str_concat("/", "/arg/without", value+1, NULL);
					put(tmp, strue);
					free(tmp);
				}
				found = 1;
			}
			else {
				/* Look in the argument translate table */
				for(a = main_argument_table; a->arg != NULL; a++) {
					if (((a->arg[0] == '^') && (strncmp(a->arg+1, key, strlen(a->arg+1)) == 0)) || (strcmp(a->arg, key) == 0)) {
						found = 1;
						if (a->callback != NULL) {
							if (a->callback(key, value) != 0) {
								error("Processing argument '%s' failed in the callback\n", argv[n]);
								abort();
							}
						}
						if (a->path != NULL)
							put(a->path, value);
					}
				}
			}

			/* Look in known deps table or /arg */
			if (found == 0) {
				if ((is_dep_known(key)) || (strncmp(key, "/arg/", 5) == 0)) {
					put(key, value);
					found = 1;
				}
			}
		}

		if (found == 0) {
			if (custom_arg(key, value) == 0) {
				error("Unknown argument '%s'\n", key);
				tainted++;
			}
		}
	}
	if (tainted)
		exit(1);
}

void help_default_args(FILE *f, const char *prefix)
{
	argtbl_t *a;

	if (prefix == NULL)
		prefix = "";

	fprintf(f, "%sscconfig generic command line arguments:\n", prefix);
	for(a = main_argument_table; a->arg != NULL; a++) {
		char *tmp;
		if (a->help == NULL)
			continue;
		if (*a->arg == '^') {
			tmp = str_concat("", a->arg+1, "...=...", NULL);
			fprintf(f, "%s --%-22s  %s\n", prefix, tmp, a->help);
		}
		else {
			tmp = str_concat("", a->arg, "=...", NULL);
			fprintf(f, "%s --%-22s  %s\n", prefix, tmp, a->help);
		}
		free(tmp);
	}
}
