/*
    scconfig - library functions for compiling and running test code
    Copyright (C) 2009  Tibor Palinkas

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
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "log.h"
#include "libs.h"
#include "db.h"
#include "dep.h"

/*
#define KEEP_TEST_SRCS
*/

int cross_blind = 0;


static char *clone_flags(const char *input, const char *node)
{
	char *output;
	const char *s;
	int len;

	if (input != NULL) {
		if (*input == '+') {
			s = get(node);
			if (s != NULL) {
				len = strlen(s);
				output = malloc(len + strlen(input) + 4);
				memcpy(output, s, len);
				output[len] = ' ';
				strcpy(output + len + 1, input + 1);
			}
			else
				output = strclone(input);
		}
		else
			output = strclone(input);
	}
	else {
		s   = get(node);
		if (s != NULL)
			output = strclone(s);
		else
			output = strclone("");
	}
	return output;
}

int compile_file_raw(int logdepth, const char *fn_input, char **fn_output, const char *cc, const char *cflags, const char *ldflags)
{
	char *cmdline;
	char *cc_esc, *fn_input_esc, *fn_output_esc, *temp_out_esc, *temp_out;
	int ret;

	temp_out = tempfile_new(".out");

	if (*fn_output == NULL)
		*fn_output = tempfile_new(get("sys/ext_exe"));
	else
		*fn_output = tempfile_new(*fn_output);
	unlink(*fn_output);

	cc_esc = shell_escape_dup(cc == NULL ? get("cc/cc") : cc);
	fn_input_esc = shell_escape_dup(fn_input);
	fn_output_esc = shell_escape_dup(*fn_output);
	temp_out_esc = shell_escape_dup(temp_out);

	cmdline = str_concat("",
		get("/host/sys/shell"), " \"", cc_esc, " ", cflags, " ", fn_input_esc, " ", \
		ldflags, " -o ", fn_output_esc, " 2>&1\" >", temp_out_esc, NULL);

	free(cc_esc);
	free(fn_input_esc);
	free(fn_output_esc);
	free(temp_out_esc);

	logprintf(logdepth, "compile: '%s'\n", cmdline);
	ret = system(cmdline);
	free(cmdline);
	log_merge(logdepth + 1, temp_out);
#ifndef KEEP_TEST_SRCS
	unlink(temp_out);
#endif
	free(temp_out);
	logprintf(logdepth, "compile result: %d\n", ret);

	return ret;
}

int compile_file(int logdepth, const char *fn_input, char **fn_output, const char *cc, const char *cflags, const char *ldflags)
{
	int ret;
	char *ldflags_, *cflags_;

	cflags_  = clone_flags(cflags,  "cc/cflags");
	ldflags_ = clone_flags(ldflags, "cc/ldflags");

	ret = compile_file_raw(logdepth, fn_input, fn_output, cc, cflags_, ldflags_);

	free(cflags_);
	free(ldflags_);

	return ret;
}

int compile_code(int logdepth, const char *testcode, char **fn_output, const char *cc, const char *cflags, const char *ldflags)
{
	char *temp_in;
	int ret;

	require("sys/ext_exe", logdepth, 1);

	assert(testcode  != NULL);
	assert(fn_output != NULL);

	temp_in = tempfile_dump(testcode, ".c");
	ret = compile_file(logdepth, temp_in, fn_output, cc, cflags, ldflags);
#ifndef KEEP_TEST_SRCS
	unlink(temp_in);
#endif
	free(temp_in);

	return ret;
}

int compile_code_raw(int logdepth, const char *testcode, char **fn_output, const char *cc, const char *cflags, const char *ldflags)
{
	char *temp_in;
	int ret;

	require("sys/ext_exe", logdepth, 1);

	assert(testcode  != NULL);
	assert(fn_output != NULL);

	temp_in = tempfile_dump(testcode, ".c");
	ret = compile_file_raw(logdepth, temp_in, fn_output, cc, cflags, ldflags);
#ifndef KEEP_TEST_SRCS
	unlink(temp_in);
#endif
	free(temp_in);

	return ret;
}

char *shell_escape_dup(const char *in)
{
	char *o, *out;
	const char *i;
	const char *esc = get("sys/shell_escape_char");

	/* in the early phase, before detecting the shell, this happens */
	if (esc == NULL)
		return strclone(in);

	out = malloc(strlen(in)*2+1);
	for(i = in, o = out; *i != '\0'; i++) {
		if (*i == *esc) {
			*o++ = *esc;
		}
		else if (!isalnum(*i)) {
			switch(*i) {
				case '/':
				case '_':
				case '-':
				case '.':
					break;
				default:
					*o++ = *esc;
			}
		}
		*o++ = *i;
	}
	*o = '\0';
	return out;
}

int run(int logdepth, const char *cmd_, char **stdout_saved)
{
	char *cmd;
	char *fn_out, *temp_out;
	char *fn_out_esc, *temp_out_esc;
	int ret;
	const char *emu;

	assert(cmd_ != NULL);

	/* blind cross compiling mode means we always assume success */
	if (cross_blind) {
		if (stdout_saved != NULL)
			*stdout_saved = NULL;
		return 0;
	}

	emu = get("sys/emu");

	/* emu == NULL means we need an emulator but we don't have one and
	   we should pretend everything went well (and of course can't provide
	   output.) */
	if (emu == NULL) {
		if (stdout_saved != NULL)
			*stdout_saved = NULL;
		return 0;
	}

	/* emu == false means we need an emulator and we don't want to pretend -> fatal */
	if (strcmp(emu, sfalse) == 0) {
		error("Trying to run unavailable emulator (db_cwd='%s')\n", db_cwd);
		abort();
	}

	temp_out = tempfile_new(".out");
	fn_out   = tempfile_new("");

	temp_out_esc = shell_escape_dup(temp_out);
	fn_out_esc = shell_escape_dup(fn_out);
	cmd = malloc(strlen(emu) + strlen(cmd_) + strlen(fn_out_esc) + strlen(temp_out_esc) + 32);
	sprintf(cmd, "%s %s >%s 2>>%s", emu, cmd_, fn_out_esc, temp_out_esc);
	free(temp_out_esc);
	free(fn_out_esc);

	logprintf(logdepth, "run: '%s'\n", cmd);
	ret = system(cmd);
	log_merge(logdepth + 1, temp_out);
	unlink(temp_out);
	free(temp_out);
	logprintf(logdepth, "run result: %d\n", ret);
	free(cmd);

	if (stdout_saved != NULL) {
		if (ret == 0) {
			*stdout_saved = load_file(fn_out);
			logprintf(logdepth, "stdout: '%s'\n", *stdout_saved);
		}
		else
			*stdout_saved = NULL;
	}

	unlink(fn_out);
	free(fn_out);
	return ret;
}

int run_shell(int logdepth, const char *cmd_, char **stdout_saved)
{
	int ret;
	char *cmd, *cmd_esc;
	const char *emu;
	const char *shell;

	emu = get("sys/emulator");
	if (emu == NULL)
		emu = "";

	shell = get("sys/shell");
	if (shell == NULL) {
		error("No shell was specified (db_cwd='%s')\n", db_cwd);
		abort();
	}

	cmd_esc = shell_escape_dup(cmd_);
	cmd = malloc(strlen(emu) + strlen(shell) + strlen(cmd_esc) + 16);
	if (istrue(get("sys/shell_needs_quote")))
		sprintf(cmd, "%s %s \"%s\"", emu, shell, cmd_);
	else
		sprintf(cmd, "%s %s %s", emu, shell, cmd_);
	free(cmd_esc);

	ret = run(logdepth, cmd, stdout_saved);
	free(cmd);
	return ret;
}


int compile_run(int logdepth, const char *testcode, const char *cc, const char *cflags, const char *ldflags, char **stdout_saved)
{
	int ret;
	char *fn_output = NULL;

	ret = compile_code(logdepth+1, testcode, &fn_output, cc, cflags, ldflags);

	if (ret == 0) {
		char *fn_output_esc = shell_escape_dup(fn_output);
		ret = run(logdepth+1, fn_output_esc, stdout_saved);
		free(fn_output_esc);
	}

	if (fn_output != NULL) {
		unlink(fn_output);
		free(fn_output);
	}
	return ret;
}

int run_script(int logdepth, const char *interpreter, const char *script, const char *suffix, char **out)
{
	char *temp, *cmd;
	int res;

	temp = tempfile_dump(script, suffix);
	cmd = malloc(strlen(temp) + strlen(interpreter) + 4);
	sprintf(cmd, "%s %s", interpreter, temp);

	res = run(logdepth, cmd, out);

	unlink(temp);
	free(temp);
	free(cmd);
	return res;
}

