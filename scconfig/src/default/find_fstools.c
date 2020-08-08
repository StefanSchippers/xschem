/*
    scconfig - detection of file system tools
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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"
#include "dep.h"

static int test_cp_ln(int logdepth, const char *command, int link)
{
	char *src, *dst, *src_esc, *dst_esc;
	char *cmd, *result;
	char *test_string = "works.";
	int ret;

	logprintf(logdepth, "trying '%s'\n", command);

	src = tempfile_dump(test_string, "");
	dst = tempfile_new("");
	if (link)
		unlink(dst);

	src_esc = shell_escape_dup(src);
	dst_esc = shell_escape_dup(dst);
	cmd = malloc(strlen(command) + strlen(src_esc) + strlen(dst_esc) + 32);
	sprintf(cmd, "%s %s %s", command, src_esc, dst_esc);
	run_shell(logdepth, cmd, NULL);
	free(cmd);
	free(src_esc);
	free(dst_esc);

	result = load_file(dst);
	ret = !strcmp(result, test_string);
	logprintf(logdepth+1, "result: '%s' == '%s' (%d)\n", result, test_string, ret);
	free(result);

	unlink(src);
	free(src);

	result = load_file(dst);
	if (link) {
		if (strcmp(result, test_string) == 0) {
			report("Warning: link is copy (or hard link). ");
			logprintf(logdepth+1, "Warning: link is copy (or hard link).\n");
		}
	}
	else {
		if (strcmp(result, test_string) != 0) {
			report("Warning: copy is symlink. ");
			logprintf(logdepth+1, "Warning: copy is symlink.\n");
		}
	}
	free(result);

	if (ret) {
		if (link)
			put("fstools/ln", command);
		else
			put("fstools/cp", command);

		report("OK (%s)\n", command);
	}

	unlink(dst);
	free(dst);
	return ret;
}

static int test_mv(int logdepth, const char *command)
{
	char *src, *dst, *src_esc, *dst_esc;
	char *cmd, *result;
	char *test_string = "works.";
	int ret;

	logprintf(logdepth, "trying '%s'\n", command);

	src = tempfile_dump(test_string, "");
	dst = tempfile_new("");
	unlink(dst);

	src_esc = shell_escape_dup(src);
	dst_esc = shell_escape_dup(dst);
	cmd = malloc(strlen(command) + strlen(src_esc) + strlen(dst_esc) + 32);
	sprintf(cmd, "%s %s %s", command, src_esc, dst_esc);
	run_shell(logdepth, cmd, NULL);
	free(cmd);
	free(src_esc);
	free(dst_esc);

	result = load_file(dst);
	ret = !strcmp(result, test_string);
	logprintf(logdepth+1, "result: '%s' == '%s' (%d)\n", result, test_string, ret);
	free(result);

	if (file_size(src) > 0) {
		report("Warning: mv is copy. ");
		logprintf(logdepth+1, "Warning: mv is copy.\n");
	}

	if (ret) {
		put("fstools/mv", command);
		report("OK (%s)\n", command);
	}

	unlink(dst);
	unlink(src);
	free(dst);
	free(src);
	return ret;
}

static int test_mkdir(int logdepth, const char *command)
{
	char *dir, *file;
	char *dir_esc;
	char *cmd, *result;
	char *test_string = "works.";
	int ret = 0, had_p;
	FILE *f;

	logprintf(logdepth, "trying '%s'\n", command);
	dir = tempfile_new("");
	dir_esc = shell_escape_dup(dir);
	unlink(dir);

	had_p = is_dir("-p");

	cmd = malloc(strlen(command) + strlen(dir_esc) + 32);
	sprintf(cmd, "%s %s", command, dir_esc);
	run_shell(logdepth, cmd, NULL);
	free(cmd);

	file = malloc(strlen(dir) + 32);
	sprintf(file, "%s/test", dir);
	f = fopen(file, "w");
	if (f != NULL) {
		fputs(test_string, f);
		fclose(f);
		result = load_file(file);
		if (strcmp(result, test_string) == 0)
			ret = 1;
		free(result);
	}

	unlink(file);
	unlink(dir);

	cmd = malloc(strlen(dir) + 32);
	sprintf(cmd, "rmdir %s", dir_esc);
	run_shell(logdepth, cmd, NULL);
	free(cmd);

	free(file);
	free(dir);
	free(dir_esc);

	/* This is a bit ugly, but on win32 or other systems where mkdir works
	   but -p doesn't have an effect, a directory called -p may be left over... */
	if ((!had_p) && (is_dir("-p"))) {
		unlink("-p");
		return 0;
	}

	if (ret != 0) {
		put("fstools/mkdir", command);
		report("OK (%s)\n", command);
	}

	return ret;
}

static int test_rm(int logdepth, const char *command)
{
	char *src, *src_esc, *cmd, *test_string = "works.";
	int ret;

	logprintf(logdepth, "trying '%s'\n", command);

	src = tempfile_dump(test_string, "");

	if (file_size(src) < 0) {
		report("error: can't create temp file\n");
		free(src);
		return 0;
	}


	src_esc = shell_escape_dup(src);
	cmd = malloc(strlen(command) + strlen(src_esc) + 32);
	sprintf(cmd, "%s %s", command, src_esc);
	run_shell(logdepth, cmd, NULL);
	free(cmd);
	free(src_esc);

	ret = file_size(src) < 0;

	if (ret) {
		put("fstools/rm", command);
		report("OK (%s)\n", command);
	}
	else
		unlink(src);

	free(src);
	return ret;
}

static int test_ar(int logdepth, const char *command)
{
	char *src, *dst, *src_esc, *dst_esc;
	char *cmd, *result, *expected;
	char *test_string = "works.";
	const char *path_sep;
	int ret = 0;

	logprintf(logdepth, "trying '%s'\n", command);
	path_sep = get("sys/path_sep");

	src = tempfile_dump(test_string, "");
	dst = tempfile_new("");
	unlink(dst);

	src_esc = shell_escape_dup(src);
	dst_esc = shell_escape_dup(dst);
	cmd = malloc(strlen(command) + strlen(src_esc) + strlen(dst_esc) + 32);
	sprintf(cmd, "%s ru %s %s", command, dst_esc, src_esc);
	run_shell(logdepth, cmd, NULL);
	sprintf(cmd, "%s t %s", command, dst_esc);
	run_shell(logdepth, cmd, &result);
	free(cmd);
	free(dst_esc);
	free(src_esc);

	if (result != NULL) {
		expected = str_rchr(src, *path_sep);
		if (expected == NULL)
			expected = src;
		else
			expected++;

		logprintf(logdepth, "test_ar path_sep='%s' expected='%s' result='%s'\n", path_sep, expected, result);

		ret = strncmp(expected, result, strlen(expected)) == 0;
		if (ret) {
			put("fstools/ar", command);
			report("OK (%s)\n", command);
		}
		free(result);
	}

	unlink(src);
	unlink(dst);
	free(src);
	free(dst);
	return ret;
}

static int test_ranlib(int logdepth, const char *command, const char *obj)
{
	char *cmd, *archive, *archive_esc, *obj_esc;
	const char *ar;
	int ret;
	ar = get("fstools/ar");
	logprintf(logdepth, "trying '%s'\n", command);

	archive = tempfile_new(".a");
	archive_esc = shell_escape_dup(archive);
	obj_esc = shell_escape_dup(obj);
	cmd = malloc(strlen(command) + strlen(obj_esc) + strlen(archive_esc) + 64);

	sprintf(cmd, "%s r %s %s", ar, archive_esc, obj_esc);
	unlink(archive);
	ret = run_shell(logdepth, cmd, NULL) == 0;
	if (!ret)
		goto fin;

	sprintf(cmd, "%s %s", command, archive_esc);
	ret = run_shell(logdepth, cmd, NULL) == 0;

	if (ret) {
		put("fstools/ranlib", command);
		report("OK (%s)\n", command);
	}

fin:;
	unlink(archive);
	free(archive);
	free(cmd);
	free(archive_esc);
	free(obj_esc);
	return ret;
}

static int test_awk(int logdepth, const char *command)
{
	char cmd[1024];
	char *out;
	int ret = 0;
	char *script, *script_esc;

	/* For some reason windows awk doesn't like the code with NLs */
	char *test_awk =
		"BEGIN {"
		" gsub(\"b\", \"B\", t);"
		" print t;"
		"}";

	logprintf(logdepth, "trying '%s'\n", command);
	script = tempfile_dump(test_awk, ".awk");
	script_esc = shell_escape_dup(script);
	sprintf(cmd, "%s -v \"t=blobb\" -f %s", command, script_esc);
	free(script_esc);
	run_shell(logdepth, cmd, &out);
	unlink(script);
	free(script);

	if ((out != NULL) && (strncmp(out, "BloBB", 5) == 0)) {
		put("fstools/awk", command);
		report("OK (%s)\n", command);
		ret = 1;
	}

	free(out);
	return ret;
}

static int test_cat(int logdepth, const char *command)
{
	char cmd[1024];
	char *out;
	int ret = 0;
	char *fn, *fn_esc;
	const char *test_str = "hello world";

	logprintf(logdepth, "trying '%s'\n", command);
	fn = tempfile_dump(test_str, ".txt");
	fn_esc = shell_escape_dup(fn);
	sprintf(cmd, "%s %s", command, fn_esc);
	run_shell(logdepth, cmd, &out);
	unlink(fn);
	free(fn);
	free(fn_esc);

	if ((out != NULL) && (strncmp(out, test_str, strlen(test_str)) == 0)) {
		put("fstools/cat", command);
		report("OK (%s)\n", command);
		ret = 1;
	}

	free(out);
	return ret;
}

static int test_sed(int logdepth, const char *command)
{
	char cmd[1024];
	char *out;
	int ret = 0;
	char *fn, *fn_esc;
	const char *test_str_in  = "hello world";
	const char *test_str_out = "he11o wor1d";

	logprintf(logdepth, "trying '%s'\n", command);
	fn = tempfile_dump(test_str_in, ".txt");
	fn_esc = shell_escape_dup(fn);
	sprintf(cmd, "%s \"s/l/1/g\" < %s", command, fn_esc);
	run_shell(logdepth, cmd, &out);
	unlink(fn);
	free(fn);
	free(fn_esc);

	if ((out != NULL) && (strncmp(out, test_str_out, strlen(test_str_out)) == 0)) {
		put("fstools/sed", command);
		report("OK (%s)\n", command);
		ret = 1;
	}

	free(out);
	return ret;
}

static int test_chmodx(int logdepth, const char *command)
{
	char *cmd, *tmp, *tmp_esc, *out, *s;
	int ret;

	logprintf(logdepth, "trying '%s'\n", command);
	tmp = tempfile_dump("#!/bin/sh\necho OK\n", ".bat");

	tmp_esc = shell_escape_dup(tmp);
	cmd = malloc(strlen(command) + strlen(tmp_esc) + 16);
	sprintf(cmd, "%s %s", command, tmp_esc);
	ret = run_shell(logdepth, cmd, NULL) == 0;
	free(cmd);
	if (!ret) {
		free(tmp_esc);
		return ret;
	}

	ret = run(logdepth+1, tmp_esc, &out);
	free(tmp_esc);

	if (ret == 0) {
		for(s = out; s != NULL; s = str_chr(s, '\n')) {
			logprintf(logdepth+1, "chmod line to test: '%s'\n", s);
			if ((s[0] == 'O') && (s[1] == 'K')) {
				logprintf(logdepth+2, "(OK)\n");
				ret = 1;
				break;
			}
			s++;
		}
	}
	else
		ret = 0;

	free(out);
	if (ret) {
		put("fstools/chmodx", command);
		logprintf(logdepth, "chmodx command validated: '%s'\n", command);
		report("OK (%s)\n", command);
	}
	unlink(tmp);
	return ret;
}

static int test_file(int logdepth, const char *node, const char *command)
{
	char cmd[1024];
	char *out;
	int ret = 0;
	char *fn, *fn_esc;

	logprintf(logdepth, "trying '%s'\n", command);
	fn = tempfile_dump("plain text file\r\n", ".txt");
	fn_esc = shell_escape_dup(fn);
	sprintf(cmd, "%s %s", command, fn_esc);
	run_shell(logdepth, cmd, &out);
	unlink(fn);
	free(fn);
	free(fn_esc);

	if ((out != NULL) && (strstr(out, "text") != NULL)) {
		put(node, command);
		report("OK (%s)\n", command);
		ret = 1;
	}

	free(out);
	return ret;
}

int find_fstools_cp(const char *name, int logdepth, int fatal)
{
	const char *cp;

	(void) fatal; /* to suppress compiler warnings about not using fatal */

	report("Checking for cp... ");
	logprintf(logdepth, "find_fstools_cp: trying to find cp...\n");
	logdepth++;

	cp = get("/arg/fstools/cp");
	if (cp == NULL) {
		if (test_cp_ln(logdepth, "cp -rp", 0)) return 0;
		if (test_cp_ln(logdepth, "cp -r",  0)) return 0;
		if (test_cp_ln(logdepth, "copy /r",  0)) return 0; /* wine */
	}
	else {
		report(" user provided (%s)...", cp);
		if (test_cp_ln(logdepth, cp, 0)) return 0;
	}
	return 1;
}

int find_fstools_ln(const char *name, int logdepth, int fatal)
{
	const char *ln;

	(void) fatal; /* to suppress compiler warnings about not using fatal */

	report("Checking for ln... ");
	logprintf(logdepth, "find_fstools_ln: trying to find ln...\n");
	logdepth++;

	ln = get("/arg/fstools/ln");
	if (ln == NULL) {
		if (test_cp_ln(logdepth, "ln -sf",1 )) return 0;
		if (test_cp_ln(logdepth, "ln -s",1 ))  return 0;
		if (test_cp_ln(logdepth, "ln",   1))   return 0;
		/* "mklink /H"  ->  win32 equivalent to "ln" */
		/* "cp -s"  ->  same as "ln -s" */
		/* "cp -l"  ->  same as "ln" */
		if (test_cp_ln(logdepth, "cp",   1))   return 0;
	}
	else {
		report(" user provided (%s)...", ln);
		if (test_cp_ln(logdepth, ln, 1)) return 0;
	}
	return 1;
}

int find_fstools_mv(const char *name, int logdepth, int fatal)
{
	const char *mv;

	(void) fatal; /* to suppress compiler warnings about not using fatal */

	report("Checking for mv... ");
	logprintf(logdepth, "find_fstools_mv: trying to find mv...\n");
	logdepth++;

	mv = get("/arg/fstools/mv");
	if (mv == NULL) {
		if (test_mv(logdepth, "mv"))   return 0;
		if (test_mv(logdepth, "move")) return 0; /* win32 */
		if (test_mv(logdepth, "cp"))   return 0;
	}
	else {
		report(" user provided (%s)...", mv);
		if (test_mv(logdepth, mv)) return 0;
	}
	return 1;
}

int find_fstools_rm(const char *name, int logdepth, int fatal)
{
	const char *rm;

	(void) fatal; /* to suppress compiler warnings about not using fatal */

	report("Checking for rm... ");
	logprintf(logdepth, "find_fstools_rm: trying to find rm...\n");
	logdepth++;

	rm = get("/arg/fstools/rm");
	if (rm == NULL) {
		if (test_rm(logdepth, "rm -rf")) return 0;
		if (test_rm(logdepth, "rm -f"))  return 0;
		if (test_rm(logdepth, "rm"))     return 0;
		if (test_rm(logdepth, "del"))    return 0; /* for win32 */
	}
	else {
		report(" user provided (%s)...", rm);
		if (test_rm(logdepth, rm)) return 0;
	}
	return 1;
}

int find_fstools_mkdir(const char *name, int logdepth, int fatal)
{
	const char *mkdir;

	(void) fatal; /* to suppress compiler warnings about not using fatal */

	report("Checking for mkdir... ");
	logprintf(logdepth, "find_fstools_mkdir: trying to find mkdir...\n");
	logdepth++;

	mkdir = get("/arg/fstools/mkdir");
	if (mkdir == NULL) {
		if (test_mkdir(logdepth, "mkdir -p")) return 0;
		if (test_mkdir(logdepth, "md"))       return 0; /* for win32 */
	}
	else {
		report(" user provided (%s)...", mkdir);
		if (test_mkdir(logdepth, mkdir)) return 0;
	}
	return 1;
}

int find_fstools_ar(const char *name, int logdepth, int fatal)
{
	const char *ar, *target;
	char *targetar;
	int len;

	(void) fatal; /* to suppress compiler warnings about not using fatal */

	require("sys/path_sep", logdepth, fatal);


	report("Checking for ar... ");
	logprintf(logdepth, "find_fstools_ar: trying to find ar...\n");
	logdepth++;

	ar = get("/arg/fstools/ar");
	if (ar == NULL) {
		target = get("/arg/sys/target");
		if (target != NULL) {
			logprintf(logdepth+1, "find_ar: crosscompiling for '%s', looking for target ar\n", target);
			len = strlen(target);
			targetar = malloc(len + 8);
			memcpy(targetar, target, len);
			strcpy(targetar + len, "-ar");
			if (test_ar(logdepth, targetar)) {
				free(targetar);
				return 0;
			}
			free(targetar);
		}
		if (test_ar(logdepth, "ar")) return 0;
		if (test_ar(logdepth, "/usr/bin/ar")) return 0;
	}
	else {
		report(" user provided (%s)...", ar);
		if (test_ar(logdepth, ar)) return 0;
	}
	return 1;
}

int find_fstools_ranlib(const char *name, int logdepth, int fatal)
{
	const char *ranlib, *target;
	char *targetranlib;
	int len;
	char *test_code = NL "int zero() { return 0; }" NL;
	char *obj = ".o";

	(void) fatal; /* to suppress compiler warnings about not using fatal */

	require("fstools/ar", logdepth, fatal);
	require("cc/cc", logdepth, fatal);

	report("Checking for ranlib... ");
	logprintf(logdepth, "find_fstools_ranlib: trying to find ranlib...\n");
	logdepth++;

	logprintf(logdepth, "compiling test object...\n");
	if (compile_code(logdepth+1, test_code, &obj, NULL, "-c", NULL) != 0) {
		logprintf(logdepth, "ERROR: Can't compile test object\n");
		report("ERROR: Can't compile test object\n");
		abort();
	}

	ranlib = get("/arg/fstools/ranlib");
	if (ranlib == NULL) {
		target = get("/arg/sys/target");
		if (target != NULL) {
			logprintf(logdepth+1, "find_ranlib: crosscompiling for '%s', looking for target ranlib\n", target);
			len = strlen(target);
			targetranlib = malloc(len + 16);
			memcpy(targetranlib, target, len);
			strcpy(targetranlib + len, "-ranlib");
			if (test_ranlib(logdepth, targetranlib, obj)) {
				free(targetranlib);
				return 0;
			}
			free(targetranlib);
		}
		if (test_ranlib(logdepth, "ranlib", obj)) goto found;
		if (test_ranlib(logdepth, "/usr/bin/ranlib", obj)) goto found;
		if (test_ranlib(logdepth, "ar -s", obj)) goto found;
		if (test_ranlib(logdepth, "/usr/bin/ar -s", obj)) goto found;

		/* some systems (for example IRIX) can't run s without doing
		   something else; t is harmless */
		if (test_ranlib(logdepth, "ar ts", obj)) goto found;
		if (test_ranlib(logdepth, "/usr/bin/ar ts", obj)) goto found;

		/* final fallback: some systems (for example minix3) simply
		   do not have ranlib or ar equivalent; it's easier to detect
		   a dummy command than to force conditions into Makefiles */
		if (test_ranlib(logdepth, "true", obj)) goto found;
	}
	else {
		report(" user provided (%s)...", ranlib);
		if (test_ranlib(logdepth, ranlib, obj)) goto found;
	}
	unlink(obj);
	free(obj);
	return 1;
found:;
	unlink(obj);
	free(obj);
	return 0;
}

int find_fstools_awk(const char *name, int logdepth, int fatal)
{
	const char *awk;

	(void) fatal; /* to suppress compiler warnings about not using fatal */

	report("Checking for awk... ");
	logprintf(logdepth, "find_fstools_awk: trying to find awk...\n");
	logdepth++;

	awk = get("/arg/fstools/awk");
	if (awk == NULL) {
		if (test_awk(logdepth, "awk")) return 0;
		if (test_awk(logdepth, "gawk")) return 0;
		if (test_awk(logdepth, "mawk")) return 0;
		if (test_awk(logdepth, "nawk")) return 0;
	}
	else {
		report(" user provided (%s)...", awk);
		if (test_awk(logdepth, awk)) return 0;
	}
	return 1;
}

int find_fstools_chmodx(const char *name, int logdepth, int fatal)
{
	const char *chmod;

	(void) fatal; /* to suppress compiler warnings about not using fatal */

	report("Checking for chmod to executable... ");
	logprintf(logdepth, "find_fstools_awk: trying to find chmod to executable...\n");
	logdepth++;

	chmod = get("/arg/fstools/chmodx");
	if (chmod == NULL) {
		if (test_chmodx(logdepth, "chmod +x")) return 0;
		if (test_chmodx(logdepth, "chmod 755")) return 0;
		if (test_chmodx(logdepth, "")) return 0; /* on some systems we don't need to do anything */
	}
	else {
		report(" user provided (%s)...", chmod);
		if (test_chmodx(logdepth, chmod)) return 0;
	}
	return 1;
}

int find_fstools_cat(const char *name, int logdepth, int fatal)
{
	const char *cat;

	(void) fatal; /* to suppress compiler warnings about not using fatal */

	report("Checking for cat... ");
	logprintf(logdepth, "find_fstools_cat: trying to find cat...\n");
	logdepth++;

	cat = get("/arg/fstools/cat");
	if (cat == NULL) {
		if (test_cat(logdepth, "cat")) return 0;
		if (test_cat(logdepth, "type")) return 0;
	}
	else {
		report(" user provided (%s)...", cat);
		if (test_cat(logdepth, cat)) return 0;
	}
	return 1;
}

int find_fstools_sed(const char *name, int logdepth, int fatal)
{
	const char *sed;

	(void) fatal; /* to suppress compiler warnings about not using fatal */

	report("Checking for sed... ");
	logprintf(logdepth, "find_fstools_sed: trying to find sed...\n");
	logdepth++;

	sed = get("/arg/fstools/sed");
	if (sed == NULL) {
		if (test_sed(logdepth, "sed")) return 0;
	}
	else {
		report(" user provided (%s)...", sed);
		if (test_sed(logdepth, sed)) return 0;
	}
	return 1;
}

int find_fstools_file_l(const char *name, int logdepth, int fatal)
{
	const char *file;

	(void) fatal; /* to suppress compiler warnings about not using fatal */

	report("Checking for file... ");
	logprintf(logdepth, "find_fstools_file_l: trying to find file -L...\n");
	logdepth++;

	file = get("/arg/fstools/file_l");
	if (file == NULL) {
		if (test_file(logdepth, "fstools/file_l", "file -L")) return 0;
		if (test_file(logdepth, "fstools/file_l", "file")) return 0;
	}
	else {
		report(" user provided (%s)...", file);
		if (test_file(logdepth, "fstools/file_l", file)) return 0;
	}
	return 1;
}

int find_fstools_file(const char *name, int logdepth, int fatal)
{
	const char *file;

	(void) fatal; /* to suppress compiler warnings about not using fatal */

	report("Checking for file... ");
	logprintf(logdepth, "find_fstools_file: trying to find file...\n");
	logdepth++;

	file = get("/arg/fstools/file");
	if (file == NULL) {
		if (test_file(logdepth, "fstools/file", "file")) return 0;
	}
	else {
		report(" user provided (%s)...", file);
		if (test_file(logdepth, "fstools/file", file)) return 0;
	}
	return 1;
}
