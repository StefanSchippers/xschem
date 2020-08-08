/*
    scconfig - detect features of the system or the host/target computer
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
#include <ctype.h>
#include <unistd.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"

int find_sys_ptrwidth(const char *name, int logdepth, int fatal)
{
	char *end, W[32];
	char *out = NULL;
	int w;

	char *test_c =
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	void *ptr;"
		NL "	printf(\"%d\\n\", sizeof(ptr));"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for pointer width... ");
	logprintf(logdepth, "find_sys_ptrwidth: trying to find pointer width...\n");
	logdepth++;

	if (compile_run(logdepth, test_c, NULL, NULL, NULL, &out) == 0) {
		w = strtol(out, &end, 10);
		if ((*end != '\0') && (*end != '\n') && (*end != '\r')) {
			report("FAILED (test code failed)\n");
			logprintf(logdepth+1, "FAILED: returned '%s' which is not a valid decimal number (at '%s')\n", out, end);
			return 1;
		}
		sprintf(W, "%d", w * 8);
		report("OK (%s bits)\n", W);
		put("sys/ptrwidth", W);
		logprintf(logdepth+1, "OK (%s bits)\n", W);
	}
	return 0;
}

int find_sys_byte_order(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	long int i = 8;"
		NL "	char *s = (char *)&i;"
		NL "	printf(\"%d\\n\", s[0]);"
		NL "	return 0;"
		NL "}"
		NL;
	const char* test_c_blind_template =
		NL "#include <stdio.h>"
		NL "#include <endian.h>"
		NL "#ifndef __BYTE_ORDER"
		NL "#error \"ERROR 1\""
		NL "void void *;"
		NL "#endif"
		NL "#ifndef __%s_ENDIAN"
		NL "#error \"ERROR 2\""
		NL "char char *;"
		NL "#endif"
		NL "#if __BYTE_ORDER != __%s_ENDIAN"
		NL "#error \"ERROR 3\""
		NL "int int *;"
		NL "#endif"
		NL "int main() {"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *key = "sys/byte_order";
	const char *endians1[] = { "LITTLE", "BIG", NULL };
	const char *endians2[] = { "LSB",    "MSB", NULL };
	int index;
	char test_c_blind[1024];
	char *end;
	const char *W;
	char *out = NULL;
	int w;

	require("cc/cc", logdepth, fatal);

	report("Checking for byte order... ");
	logprintf(logdepth, "find_sys_byte_order: trying to find byte order...\n");
	logdepth++;

	if ((!isblind(db_cwd)) && compile_run(logdepth, test_c, NULL, NULL, NULL, &out) == 0) {
		w = strtol(out, &end, 10);
		if (((*end != '\0') && (*end != '\n') && (*end != '\r')) || ((w != 0) && (w != 8))) {
			report("FAILED (test code failed)\n");
			logprintf(logdepth+1, "FAILED: returned '%s' which is not a valid decimal number (at '%s')\n", out, end);
			return 1;
		}
		if (w == 0)
			W = "MSB";
		else
			W = "LSB";

		report("OK (%s first)\n", W);
		put("sys/byte_order", W);
		logprintf(logdepth+1, "OK (%s first)\n", W);
		return 0;
	}

	for (index=0; endians1[index]!=NULL; ++index)
	{
		sprintf(test_c_blind, test_c_blind_template, endians1[index], endians1[index]);

		if (compile_run(logdepth, test_c_blind, NULL, NULL, NULL, NULL) == 0)
		{
			W = endians2[index];
			report("OK (%s first)\n", W);
			put(key, W);
			return 0;
		}
	}

	report("FAILED (cannot determine byte order, you must supply it)\n");
	return try_fail(logdepth, key);
}

static int test_shell_eats_backslash(int logdepth)
{
	char *test = "echo c:\\n";
	char *out;


	logprintf(logdepth, "testing if shell eats \\...\n");
	run_shell(logdepth+1, test, &out);
	if (out == NULL) {
		logprintf(logdepth+1, "oops, couldn't run shell?! (returned NULL)\n");
		report("ERROR: shell fails.");
		abort();
	}
	if (out[2] == '\\') {
		logprintf(logdepth, "shell does NOT eat \\...\n");
		put("sys/shell_eats_backslash", sfalse);
		free(out);
		return 0;
	}

	free(out);
	logprintf(logdepth, "shell eats \\...\n");
	put("sys/shell_eats_backslash", strue);
	return 1;
}


static int try_get_cwd(int logdepth, const char *cmd)
{
	char *cwd, *end;
	run_shell(logdepth+1, cmd, &cwd);
	if (cwd != NULL) {
		end = strpbrk(cwd, "\r\n");
		if (end != NULL)
			*end = '\0';
		if (*cwd != '\0') {
			end = cwd + strlen(cwd) - 1;
			while((*end == ' ') || (*end == '\t')) { *end = '\0'; end--; }
			put("sys/tmp", cwd);
			/* ugly hack for win32: paths there start as c:\ */
			if ((cwd[1] == ':') && (cwd[2] == '\\'))
				append("sys/tmp", "\\");
			else
				append("sys/tmp", "/");
			logprintf(logdepth, "cwd is '%s'\n", get("sys/tmp"));
			free(cwd);
			return 1;
		}
		else
			free(cwd);
	}
	return 0;
}

static int try_tmp(int logdepth)
{
	char *fn;

	logprintf(logdepth, "validating temp dir '%s'\n", get("sys/tmp"));
	fn = tempfile_new_noabort("");
	if (fn != NULL) {
		unlink(fn);
		free(fn);
		logprintf(logdepth, "temp dir works!\n");
		return 1;
	}

	logprintf(logdepth, "temp dir fails\n");
	return 0;
}

/* test temp dir with all sort of workarounds */
static int try_tmp_all(int logdepth)
{
	const char *tmp, *si;
	char c;
	char *t, *so, *old_tmp;
	int eats, n;

	tmp = get("sys/tmp");

	/* path must end in path separator */
	c = tmp[strlen(tmp)-1];
	if ((c != '/') && (c != '\\')) {
		append("sys/tmp", "/");
		tmp = get("sys/tmp");
	}

	logprintf(logdepth, "trying detected temp dir '%s'\n", tmp);
	if (try_tmp(logdepth+1)) return 1;

	/* try msys-on-windows hack: if path starts with /d/something, try d:/ instead */
	if ((tmp[0] == '/') && (isalpha(tmp[1])) && (tmp[2] == '/')) {
		/* for the next test we shouldn't use our half-detected tmp path but go with . */
		old_tmp = strclone(tmp);
		put("sys/tmp", "");
		eats = istrue(get("sys/shell_eats_backslash"));
		tmp = old_tmp;
		logprintf(logdepth, "tmp2='%s' eats=%d\n", tmp, eats);
		t = malloc(strlen(tmp) * 2);
		t[0] = tmp[1];
		t[1] = ':';
		for(si = tmp + 2, so = t + 2; *si != '\0'; si++, so++) {
			if (*si == '/') {
				*so = '\\';
				if (eats) {
					for(n = 0; n < 3; n++) {
						so++;
						*so = '\\';
					}
				}
			}
			else
				*so = *si;
		}
		*so = '\0';
		free(old_tmp);

		logprintf(logdepth, "trying windows fix: '%s'\n", t);
		put("sys/tmp", t);
		free(t);
		if (try_tmp(logdepth+1)) {
			if (eats)
				put("sys/path_sep", "\\\\\\\\");
			else
				put("sys/path_sep", "\\");
			put("sys/path_sep_escaped", "\\\\");
			return 1;
		}
		tmp = get("sys/tmp");
	}

	/* fail. Set back tmp to empty so next command has a chance to run */
	put("sys/tmp", "");
	return 0;
}

int find_tmp(const char *name, int logdepth, int fatal)
{
	const char *usertmp;

	if (in_cross_target) {
		report("Temp dir for cross compilation target is the same as for host...");
		logprintf(logdepth, "Copying temp dir from host to target\n");
		require("/host/sys/tmp", logdepth, fatal);
		usertmp = get("/host/sys/tmp");
		if (usertmp == NULL) {
			report("Host temp dir not found.\n");
			logprintf(logdepth, "Host temp dir not found.\n");
			return 1;
		}
		put("sys/tmp", usertmp);
		return 0;
	}

	/* we need shell for later tests; do this detection in . */
	put("sys/tmp", "");
	require("sys/shell", logdepth, fatal);

	put("sys/path_sep", "/");
	put("sys/path_sep_escaped", "/");

	report("Detecting temp dir...");
	logprintf(logdepth, "Finding temp dir (current working directory)...\n");

	usertmp = get("/arg/sys/tmp");

	/* . as tmp would fail for commands including a "cd" - this would cause
	   temporary files left in the target dir. We start out with empty
	   string (which is ., but on windows ./ would fail), and run
	   pwd (without cd) to find out the current directory (as getcwd() is not
	   portable). If pwd fails, we stay with ./ */
	put("sys/tmp", "");

	/* we need to know about shell backslash problem regardless of how
	   we end up with tmp - currently tmp is ., where the test could run
	   safely */
	test_shell_eats_backslash(logdepth+1);

	/* Special case: cross-compilation with emulator; we can not assume
	   the emulator uses the same paths as the host system, while we mix
	   accessing files from host and emu. If we stay in ., both emulator
	   and host system should be (more or less) happy. */
	if (istarget(db_cwd) && iscross) {
		if (usertmp == NULL) {
			report("using temp dir . for cross-compilation\n");
			logprintf(logdepth, "staying with . for cross-compilation\n");
		}
		else {
			put("sys/tmp", usertmp);
			report("using user supplied temp dir '%s' for cross-compilation\n", usertmp);
			logprintf(logdepth, "using user supplied temp dir '%s' for cross-compilation\n", usertmp);
			logprintf(logdepth, "Path sep: '%s'\n", get("sys/path_sep"));
		}
		return 0;
	}

	if ((usertmp != NULL))
			put("sys/tmp", usertmp);

	if (
	   ((usertmp != NULL) && (try_tmp_all(logdepth+2))) ||                      /* try user supplied temp dir */
	   ((try_get_cwd(logdepth+1, "pwd")) && (try_tmp_all(logdepth+2))) ||       /* try pwd for finding out cwd */
	   ((try_get_cwd(logdepth+1, "echo %cd%") && (try_tmp_all(logdepth+2))))) { /* try windows-specific way for finding out cwd */

		report(" validated %s\n", get("sys/tmp"));
		logprintf(logdepth, "Detected temp dir '%s'\n", get("sys/tmp"));
		logprintf(logdepth, "Path sep: '%s'\n", get("sys/path_sep"));
		return 0;
	}

	put("sys/tmp", "");
	report("using temp dir fallback .\n");
	logprintf(logdepth, "all temp directories failed, using . as tmp\n");
	logprintf(logdepth, "Path sep: '%s'\n", get("sys/path_sep"));
	return 0;
}

int test_shell(const char *shell, int logdepth, int quote)
{
	char *test = "echo hello";
	char *cmd;
	char *out;
	char *q;

	if (quote)
		q = "\"";
	else
		q = "";

	logprintf(logdepth, "testing '%s' as shell\n", shell);
	cmd = malloc(strlen(test) + strlen(shell) + 8);
	sprintf(cmd, "%s %s%s%s", shell, q, test, q);

	run(logdepth+1, cmd, &out);

	free(cmd);

	if ((out != NULL) && (strncmp(out, "hello", 5) == 0)) {
		put("sys/shell", shell);
		if (quote)
			put("sys/shell_needs_quote", strue);
		else
			put("sys/shell_needs_quote", sfalse);
		logprintf(logdepth, "accepted.\n");
		free(out);
		return 1;
	}

	logprintf(logdepth, "refused.\n");
	free(out);
	return 0;
}

static int find_shell_escape(const char *name, int logdepth, int fatal, const char *shell)
{
	char cmdline[256];
	char **t;
	char *tests[] = {
		"\\", "\\ {}&;|",
		"^",  "^ &",
		NULL, NULL
	};
	(void) fatal;  /* not used */
	(void) shell;  /* not used */

	report("Looking for a shell escape character... ");
	logprintf(logdepth, "finding shell escape character...\n");

	for(t = tests; *t != NULL; t += 2) {
		char *s, *end, *out, *start;
		strcpy(cmdline, "echo ");
		end = cmdline+5;
		for(s = t[1]; *s != '\0'; s++) {
			*end++ = *t[0];
			*end++ = *s;
		}
		*end = '\0';
		run(logdepth+1, cmdline, &out);
		if (out != NULL) {
			int res;
			if (*out == '\"') /* wine likes to wrap the output in quotes for some reason */
				start = out+1;
			else
				start = out;

			res = strncmp(start, t[1], strlen(t[1]));
			free(out);
			if (res == 0) {
				report("found: '%s'\n", t[0]);
				logprintf(logdepth, "found shell escape char '%s'\n", t[0]);
				put("sys/shell_escape_char", t[0]);
				return 0;
			}
		}
	}
	report("NOT FOUND\n");
	logprintf(logdepth, "shell escape character not found\n");

	return 1;
}

int find_shell(const char *name, int logdepth, int fatal)
{
	const char *shells[] = {
		"/bin/sh -c",
		"/bin/bash -c",
		"bash -c",
		"cmd.exe /c",
		"sh -c",
		"/bin/dash -c",
		"dash -c",
		"/bin/ksh -c",
		"ksh -c",
		NULL
	};
	const char **s;

	if (cross_blind) {
		const char *shell = get("/arg/sys/target-shell");
		if (shell == NULL) {
			report("Need to specify sys/target-shell in blind cross compiling mode, because the shell cannot be detected (note: scconfig will not attempt to run the target shell)\n");
			exit(1);
		}

		put("sys/shell", shell);
		report("Blind cross compiling: accepting '%s' as shell\n", shell);
		logprintf(logdepth, "Blind cross compiling: accepting '%s' as shell\n", shell);
		return 0;
	}

	report("Looking for a shell... ");
	logprintf(logdepth, "finding a shell\n");

	for(s = shells; *s != NULL; s++) {
		if ((test_shell(*s, logdepth+1, 0)) || (test_shell(*s, logdepth+1, 1))) {
			report("%s\n", *s);
			logprintf(logdepth, "found a shell '%s', need quote: %s\n", *s, get("sys/shell_needs_quote"));
			return find_shell_escape(name, logdepth, fatal, *s);
		}
	}

	report("NOT FOUND\n");
	logprintf(logdepth, "shell not found\n");
	return 1;
}
