/*
    scconfig - set nodes from tables upon user CLI arguments
    Copyright (C) 2015   Tibor Palinkas

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

#include <stdio.h>
#include <string.h>
#include "log.h"
#include "libs.h"
#include "db.h"
#include "arg_auto_set.h"

const arg_auto_set_node_t arg_lib_nodes[] = {
	{"presents",    sfalse},
	{"cflags",      ""},
	{"ldflags",     ""},
	{NULL, NULL}
};

const arg_auto_set_node_t arg_true[] = {
	{"",    strue},
	{NULL, NULL}
};

const arg_auto_set_node_t arg_false[] = {
	{"",    sfalse},
	{NULL, NULL}
};


int arg_auto_set(const char *key, const char *value, const arg_auto_set_t *table)
{
	const arg_auto_set_t *lib;
	const arg_auto_set_node_t *node;

	for(lib = table; lib->arg_key != NULL; lib++) {
		if (strcmp(key, lib->arg_key) == 0) {
			report("Executing lib table: %s\n", lib->arg_key);
			for(node = lib->subvals; node->name != NULL; node++) {
				char *s;
				const char *setval;

				setval = node->value;
				if (strcmp(setval, "$") == 0)
					setval = value;

				if (*node->name != '\0') {
					s = str_concat("/", lib->subtree, node->name, NULL);
					put(s, setval);
					free(s);
				}
				else
					put(lib->subtree, setval);
			}
			return 1;
		}
	}
	return 0;
}

void arg_auto_print_options(FILE *fout, const char *line_prefix, const char *padding, const arg_auto_set_t *table)
{
	const arg_auto_set_t *t;
	int pl;

	pl = strlen(padding);

	for(t = table; t->arg_key != NULL; t++) {
		if (t->help_txt == NULL)
			continue;
		if (*t->help_txt == '$') {
			int kl = strlen(t->arg_key);
			if (kl > pl)
				kl = pl;
			fprintf(fout, "%s--%s%s%s\n", line_prefix, t->arg_key, padding+kl, t->help_txt+1);
		}
		else
			fprintf(fout, "%s%s\n", line_prefix, t->help_txt);
	}
}
