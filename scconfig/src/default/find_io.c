/*
    scconfig - detect I/O features of the system
    Copyright (C) 2010  Tibor Palinkas

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

int find_io_pipe(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <unistd.h>"
		NL "int main() {"
		NL "	int fd[2];"
		NL "	if (pipe(fd) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for pipe(2)... ");
	logprintf(logdepth, "find_io_pipe: trying to find pipe(2)...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/io/pipe", test_c, "#include <unistd.h>\n", NULL, NULL)) return 0;
	return try_fail(logdepth, "libs/io/pipe");
}

int find_io_pipe2(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <unistd.h>"
		NL "int main() {"
		NL "	int fd[2];"
		NL "	if (pipe2(fd, 0) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for pipe2(2)... ");
	logprintf(logdepth, "find_io_pipe2: trying to find pipe2(2)...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/io/pipe2", test_c, "#include <unistd.h>\n", NULL, NULL)) return 0;
	return try_fail(logdepth, "libs/io/pipe2");
}

int find_io__pipe(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "int main() {"
		NL "	int fd[2];"
		NL "	if (_pipe(fd, 1024, _O_BINARY) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for _pipe()... ");
	logprintf(logdepth, "find_io__pipe: trying to find _pipe()...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/io/_pipe", test_c, "#include <io.h>\n#include <fcntl.h>\n", NULL, NULL)) return 0;
	return try_fail(logdepth, "libs/io/_pipe");
}

int find_io_dup2(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <unistd.h>"
		NL "int main() {"
		NL "	int fd;"
		NL "	if (dup2(1, 4) == 4)"
		NL "		write(4, \"OK\\n\", 3); "
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for dup2(2)... ");
	logprintf(logdepth, "find_io_dup2: trying to find dup2(2)...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/io/dup2", test_c, NULL, NULL, NULL)) return 0;
	return try_fail(logdepth, "libs/io/dup2");
}


int find_io_fileno(const char *name, int logdepth, int fatal)
{
	char test_c[256];
	char *test_c_ =
		NL "#include <stdio.h>"
		NL "int main() {"
		    /* NOTE: can not check for implicit declaration as fileno() may be a macro (e.g. on MINIX3) */
		NL "	if (%s(stdout) >= 0)"
		NL "		puts(\"OK\"); "
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for fileno(3)... ");
	logprintf(logdepth, "find_io_fileno: trying to find fileno(3)...\n");
	logdepth++;

	/* UNIX */
	sprintf(test_c, test_c_, "fileno");
	if (try_icl(logdepth, "libs/io/fileno", test_c, NULL, NULL, NULL)) {
		put("libs/io/fileno/call", "fileno");
		return 0;
	}

	sprintf(test_c, test_c_, "fileno");
	if (try_icl(logdepth, "libs/io/fileno", test_c, "#include <unistd.h>\n", NULL, NULL)) {
		put("libs/io/fileno/call", "fileno");
		return 0;
	}

	sprintf(test_c, test_c_, "fileno");
	if (try_icl(logdepth, "libs/io/fileno", test_c, "#define _XOPEN_SOURCE\n#include <unistd.h>\n", NULL, NULL)) {
		put("libs/io/fileno/call", "fileno");
		return 0;
	}

	/* windows */
	sprintf(test_c, test_c_, "_fileno");
	if (try_icl(logdepth, "libs/io/fileno", test_c, "#include <stdio.h>\n", NULL, NULL)) {
		put("libs/io/fileno/call", "_fileno");
		return 0;
	}

	return try_fail(logdepth, "libs/io/fileno");
}

int find_io_lseek(const char *name, int logdepth, int fatal)
{
#define NODE "libs/io/lseek"

	char test_c[3256];
	const char *test_c_template =
		NL "#include <stdio.h>"
		NL "#include <fcntl.h>"
		NL "int main() {"
		NL "	const char *filename = \"%s\";"
		NL no_implicit(int, "%s", "%s")
		NL "	int fd = open(filename, O_WRONLY);"
		NL "	if (write(fd, \"abc\", 3) == 3 && %s(fd, 1, SEEK_SET) == 1)"
		NL "		puts(\"OK\"); "
		NL "	return 0;"
		NL "}"
		NL;
	char *tmpf;
	const char *incs[] = {"#include <unistd.h>","#include <io.h>",NULL};
	const char *fns[]  = {"lseek",              "_lseek",         NULL};
	const char **inc;
	const char **fn;

	require("cc/cc", logdepth, fatal);

	report("Checking for lseek(2)... ");
	logprintf(logdepth, "find_io_lseek: trying to find lseek(2)...\n");
	logdepth++;

	tmpf = tempfile_new(".psx");

	for (inc = incs, fn = fns; *fn; ++inc, ++fn) {
		sprintf(test_c, test_c_template, tmpf, *fn, *fn, *fn);
		if (try_icl(logdepth, NODE, test_c, *inc, NULL, NULL)) {
			unlink(tmpf);
			free(tmpf);
			put(NODE "/call", *fn);
			return 0;
		}
	}

	unlink(tmpf);
	free(tmpf);
	return try_fail(logdepth, NODE);
#undef NODE
}

int find_io_popen(const char *name, int logdepth, int fatal)
{
	const char **i, *incs[] = {"#define _XOPEN_SOURCE", "#define _BSD_SOURCE", "#define _POSIX_C_SOURCE 2", NULL};
	char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	FILE *f = popen(\"echo OK\", \"r\");"
		NL "	char line[16];"
		NL "	if (f == NULL) return 0;"
		NL "	if (fgets(line, sizeof(line)-1, f) == NULL) return 0;"
		NL "	puts(line);"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for popen(3)... ");
	logprintf(logdepth, "find_io_popen: trying to find popen(3)...\n");
	logdepth++;

	for(i = incs; *i != NULL; i++)
		if (try_icl(logdepth, "libs/io/popen", test_c, *i, NULL, NULL)) return 0;
	return try_fail(logdepth, "libs/io/popen");
}

