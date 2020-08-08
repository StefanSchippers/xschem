/*
    scconfig - parser generator detection
    Copyright (C) 2009,2020  Tibor Palinkas

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
#include <stdio.h>
#include <string.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"

int find_parsgen_flex(const char *name, int logdepth, int fatal)
{
	const char *test_flex =
		NL "%%"
		NL "foo { return 1; }"
		NL "%%"
		NL ;
	char *out, *temp_in, *temp_in_esc, *cmd;
	int ret;
	char *lexfile = "lex.yy.c";
	(void) fatal;  /* not used */

	report("Checking for flex... ");
	logprintf(logdepth, "find_flex: trying to find flex...\n");
	logdepth++;

	if (is_file(lexfile)) {
		report("ERROR: %s exists, and I don't dare to delete it. Can't test flex, please remove the file by hand.\n", lexfile);
		logprintf(logdepth, "ERROR: %s exists, and I don't dare to delete it. Can't test flex, please remove the file by hand.\n", lexfile);
		exit(1);
	}
	temp_in = tempfile_dump(test_flex, ".lex");
	temp_in_esc = shell_escape_dup(temp_in);
	cmd = malloc(strlen(temp_in_esc) + 16);
	sprintf(cmd, "flex %s", temp_in_esc);
	free(temp_in_esc);
	ret = run(logdepth, cmd, &out);
	remove(temp_in);
	free(temp_in);
	if (out != NULL)
		free(out);

	if (is_file(lexfile)) {
		remove(lexfile);
		if (ret == 0) {
			put("parsgen/flex", "flex");
			put("parsgen/flex/presents", strue);
			report("Found.\n");
			return 0;
		}
	}

	put("parsgen/flex/presents", sfalse);
	report("Not found.\n");
	return 1;
}

static const char *test_yacc =
		NL "%union { char *str; double num;}"
		NL "%token <str> TOK1;"
		NL "%token <num> TOK2;"
		NL "%%"
		NL "root: one | two;"
		NL "one: TOK1;"
		NL "two: TOK2;"
		NL ;

int find_parsgen_bison(const char *name, int logdepth, int fatal)
{
	char *out, *temp_in, *temp_in_esc, *cmd;
	int ret;
	char *bisfile, *s;
	(void) fatal;  /* not used */

	report("Checking for bison... ");
	logprintf(logdepth, "find_bison: trying to find bison...\n");
	logdepth++;

	temp_in = tempfile_dump(test_yacc, ".y");
	bisfile = malloc(strlen(temp_in) + 32);
	strcpy(bisfile, temp_in);
	s = strrchr(bisfile+1, '.');
	strcpy(s, ".tab.c");
	if (is_file(bisfile)) {
		report("ERROR: %s exists, and I don't dare to delete it. Can't test bison, please remove the file by hand.\n", bisfile);
		logprintf(logdepth, "ERROR: %s exists, and I don't dare to delete it. Can't test bison, please remove the file by hand.\n", bisfile);
		exit(1);
	}
	temp_in_esc = shell_escape_dup(temp_in);
	cmd = malloc(strlen(temp_in_esc) + 16);
	sprintf(cmd, "bison %s", temp_in_esc);
	free(temp_in_esc);


	ret = run(logdepth, cmd, &out);
	remove(temp_in);
	free(temp_in);
	if (out != NULL)
		free(out);

	if (is_file(bisfile)) {
		remove(bisfile);
		if (ret == 0) {
			put("parsgen/bison", "bison");
			put("parsgen/bison/presents", strue);
			report("Found.\n");
			return 0;
		}
	}

	put("parsgen/bison/presents", sfalse);
	report("Not found.\n");
	return 1;
}

int find_parsgen_byaccic(const char *name, int logdepth, int fatal)
{
	char *out, *temp_in, *temp_in_esc, *cmd;
	int ret;
	char *bisfile, *s;
	(void) fatal;  /* not used */

	report("Checking for byaccic... ");
	logprintf(logdepth, "find_byaccic: trying to find byaccic...\n");
	logdepth++;

	temp_in = tempfile_dump(test_yacc, ".y");
	bisfile = malloc(strlen(temp_in) + 32);
	strcpy(bisfile, temp_in);
	s = strrchr(bisfile+1, '.');
	strcpy(s, ".tab.c");
	if (is_file(bisfile)) {
		report("ERROR: %s exists, and I don't dare to delete it. Can't test byaccic, please remove the file by hand.\n", bisfile);
		logprintf(logdepth, "ERROR: %s exists, and I don't dare to delete it. Can't test byaccic, please remove the file by hand.\n", bisfile);
		exit(1);
	}
	temp_in_esc = shell_escape_dup(temp_in);
	cmd = malloc(strlen(temp_in_esc)*2 + 32);
	sprintf(cmd, "byaccic -o %s %s", bisfile, temp_in_esc);
	free(temp_in_esc);

	ret = run(logdepth, cmd, &out);
	remove(temp_in);
	free(temp_in);
	if (out != NULL)
		free(out);

	if (is_file(bisfile)) {
		remove(bisfile);
		if (ret == 0) {
			put("parsgen/byaccic", "byaccic");
			put("parsgen/byaccic/presents", strue);
			report("Found.\n");
			return 0;
		}
	}

	put("parsgen/byaccic/presents", sfalse);
	report("Not found.\n");
	return 1;
}

int find_parsgen_ureglex(const char *name, int logdepth, int fatal)
{
	const char *test_ureglex =
		NL "rule blank"
		NL "regex [ \t\r]+"
		NL "code"
		NL "	ULX_IGNORE;"
		NL "rulestring 3D_DXF            return 0;"
		NL ;
	char *out, *temp_in, *temp_in_esc, *cmd;
	int ret;
	char *lexfile = "lex.yy.c";
	(void) fatal;  /* not used */

	report("Checking for ureglex... ");
	logprintf(logdepth, "find_ureglex: trying to find ureglex...\n");
	logdepth++;

	if (is_file(lexfile)) {
		report("ERROR: %s exists, and I don't dare to delete it. Can't test ureglex, please remove the file by hand.\n", lexfile);
		logprintf(logdepth, "ERROR: %s exists, and I don't dare to delete it. Can't test ureglex, please remove the file by hand.\n", lexfile);
		exit(1);
	}
	temp_in = tempfile_dump(test_ureglex, ".lex");
	temp_in_esc = shell_escape_dup(temp_in);
	cmd = malloc(strlen(temp_in_esc) + 32);
	sprintf(cmd, "ureglex -l %s -c %s", temp_in_esc, lexfile);
	free(temp_in_esc);
	ret = run(logdepth, cmd, &out);
	remove(temp_in);
	free(temp_in);
	if (out != NULL)
		free(out);

	if (is_file(lexfile)) {
		remove(lexfile);
		if (ret == 0) {
			put("parsgen/ureglex", "ureglex");
			put("parsgen/ureglex/presents", strue);
			report("Found.\n");
			return 0;
		}
	}

	put("parsgen/ureglex/presents", sfalse);
	report("Not found.\n");
	return 1;
}

void deps_parsgen_init()
{
	dep_add("parsgen/flex/*",                     find_parsgen_flex);
	dep_add("parsgen/bison/*",                    find_parsgen_bison);
	dep_add("parsgen/byaccic/*",                  find_parsgen_byaccic);
	dep_add("parsgen/ureglex/*",                  find_parsgen_ureglex);
}
