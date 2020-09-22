/*
    scconfig - detection of types and type sizes
    Copyright (C) 2012  Tibor Palinkas
    Copyright (C) 2017-2018  Aron Barath

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

/* assume there is no integer that is at least this wide, in bytes */
#define MAX_INT_WIDTH 9
/* ('long double' can be 16 bytes; see https://en.wikipedia.org/wiki/Long_double) */
#define MAX_FLT_WIDTH 17

static int try_size(int logdepth, char *cflags, char *ldflags, const char *type, int use_stdint, const char *path, const char **sizearr, unsigned char *inc_stdint, const int max_width)
{
	char *out = NULL;
	const char *test_c_template =
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	printf(\"OK %%d\\n\", sizeof(%s));"
		NL "	return 0;"
		NL "}"
		NL;
	char test_c[512], *start;
	const char *inc = "#include <stdint.h>\n";
	int size;

	if (use_stdint) {
		strcpy(test_c, inc);
		start = test_c + strlen(inc);
	}
	else
		start = test_c;
	sprintf(start, test_c_template, type);

	report("Testing size of type %25s... ", type);

	logprintf(logdepth, "trying size with ldflags '%s'\n", ldflags == NULL ? get("cc/ldflags") : ldflags);
	if (compile_run(logdepth+1, test_c, NULL, cflags, ldflags, &out) == 0) {
		if (target_emu_fail(out)) {
			report(" FAIL (emulator)\n");
			free(out);
			return -1;
		}

		if (strncmp(out, "OK", 2) == 0) {
			size = atoi(out+3);
			if ((size > 0) && (size < max_width)) {
				sprintf(test_c, "%d", size);
				put(path, test_c);
				sizearr[size] = type;
				if (inc_stdint != NULL)
					inc_stdint[size] = use_stdint;
				report(" OK, size %d byte%s\n", size, (size > 1) ? "s" : "");
			}
			else {
				report(" FAIL, size %d bytes\n", size);
				size = -1;
			}
			free(out);
			return size;
		}
		free(out);
	}
	report(" FAIL (compile)\n");
	return -1;
}

int find_types_stdint(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stdint.h>"
		NL "int main() {"
		NL "	if (sizeof(uint8_t) == 1)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for stdint.h... ");
	logprintf(logdepth, "find_types_stdint: trying to find stdint.h...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/types/stdint", test_c, NULL, NULL, NULL))
		return 0;
	return try_fail(logdepth, "libs/types/stdint");
}

int find_types_sizes(const char *name, int logdepth, int fatal)
{
	const char *stdint;
	const char *sizearr_u[MAX_INT_WIDTH];
	const char *sizearr_s[MAX_INT_WIDTH];
	const char *sizearr_f[MAX_FLT_WIDTH];
	unsigned char inc_stdint_u[MAX_INT_WIDTH];
	unsigned char inc_stdint_s[MAX_INT_WIDTH];
	int n;
	const char *includes = "";
	const char *path_template = "sys/types/size/%d_%c_int";
	const char *path_template_f = "sys/types/size/%d_float";
	char path[64];

	require("cc/cc", logdepth, fatal);
	require("libs/types/stdint/presents", logdepth, 0);
	stdint = get("libs/types/stdint/presents");

	for(n = 0; n < MAX_INT_WIDTH; n++) {
		sizearr_u[n] = NULL;
		sizearr_s[n] = NULL;
		inc_stdint_u[n] = 0;
		inc_stdint_s[n] = 0;
	}
	for(n = 0; n < MAX_FLT_WIDTH; n++)
		sizearr_f[n] = NULL;

	try_size(logdepth+1, NULL, NULL, "unsigned long long int", 0, "sys/types/size/unsigned_long_long_int", sizearr_u, NULL, MAX_INT_WIDTH);
	try_size(logdepth+1, NULL, NULL, "unsigned char", 0, "sys/types/size/unsigned_char", sizearr_u, NULL, MAX_INT_WIDTH);
	try_size(logdepth+1, NULL, NULL, "unsigned short int", 0, "sys/types/size/unsigned_short_int", sizearr_u, NULL, MAX_INT_WIDTH);
	try_size(logdepth+1, NULL, NULL, "unsigned int", 0, "sys/types/size/unsigned_int", sizearr_u, NULL, MAX_INT_WIDTH);
	try_size(logdepth+1, NULL, NULL, "unsigned long int", 0, "sys/types/size/unsigned_long_int", sizearr_u, NULL, MAX_INT_WIDTH);

	try_size(logdepth+1, NULL, NULL, "signed long long int", 0, "sys/types/size/signed_long_long_int", sizearr_s, NULL, MAX_INT_WIDTH);
	try_size(logdepth+1, NULL, NULL, "signed char", 0, "sys/types/size/signed_char", sizearr_s, NULL, MAX_INT_WIDTH);
	try_size(logdepth+1, NULL, NULL, "signed short int", 0, "sys/types/size/signed_short_int", sizearr_s, NULL, MAX_INT_WIDTH);
	try_size(logdepth+1, NULL, NULL, "signed int", 0, "sys/types/size/signed_int", sizearr_s, NULL, MAX_INT_WIDTH);
	try_size(logdepth+1, NULL, NULL, "signed long int", 0, "sys/types/size/signed_long_int", sizearr_s, NULL, MAX_INT_WIDTH);

	if ((stdint != NULL) && (istrue(stdint))) {
		try_size(logdepth+1, NULL, NULL, "uint8_t", 1, "sys/types/size/uint8_t", sizearr_u, inc_stdint_u, MAX_INT_WIDTH);
		try_size(logdepth+1, NULL, NULL, "uint16_t", 1, "sys/types/size/uint16_t", sizearr_u, inc_stdint_u, MAX_INT_WIDTH);
		try_size(logdepth+1, NULL, NULL, "uint32_t", 1, "sys/types/size/uint32_t", sizearr_u, inc_stdint_u, MAX_INT_WIDTH);
		try_size(logdepth+1, NULL, NULL, "uint64_t", 1, "sys/types/size/uint64_t", sizearr_u, inc_stdint_u, MAX_INT_WIDTH);
		try_size(logdepth+1, NULL, NULL, "int8_t", 1, "sys/types/size/int8_t", sizearr_s, inc_stdint_s, MAX_INT_WIDTH);
		try_size(logdepth+1, NULL, NULL, "int16_t", 1, "sys/types/size/int16_t", sizearr_s, inc_stdint_s, MAX_INT_WIDTH);
		try_size(logdepth+1, NULL, NULL, "int32_t", 1, "sys/types/size/int32_t", sizearr_s, inc_stdint_s, MAX_INT_WIDTH);
		try_size(logdepth+1, NULL, NULL, "int64_t", 1, "sys/types/size/int64_t", sizearr_s, inc_stdint_s, MAX_INT_WIDTH);
	}

	try_size(logdepth+1, NULL, NULL, "float", 0, "sys/types/size/float", sizearr_f, NULL, MAX_FLT_WIDTH);
	try_size(logdepth+1, NULL, NULL, "double", 0, "sys/types/size/double", sizearr_f, NULL, MAX_FLT_WIDTH);
	try_size(logdepth+1, NULL, NULL, "long double", 0, "sys/types/size/long_double", sizearr_f, NULL, MAX_FLT_WIDTH);

	for(n = 0; n < MAX_INT_WIDTH; n++) {
		if (sizearr_u[n] != NULL) {
			report("Found best fit %d bytes wide uint: %s\n", n, sizearr_u[n]);
			sprintf(path, path_template, n, 'u');
			put(path, sizearr_u[n]);
			if (inc_stdint_u[n])
				includes = "#include <stdint.h>";
		}
		if (sizearr_s[n] != NULL) {
			report("Found best fit %d bytes wide sint: %s\n", n, sizearr_s[n]);
			sprintf(path, path_template, n, 's');
			put(path, sizearr_s[n]);
			if (inc_stdint_s[n])
				includes = "#include <stdint.h>";
		}
	}
	for(n = 0; n < MAX_FLT_WIDTH; n++) {
		if (sizearr_f[n] != NULL) {
			report("Found best fit %d bytes wide float: %s\n", n, sizearr_f[n]);
			sprintf(path, path_template_f, n);
			put(path, sizearr_f[n]);
		}
	}

	put("sys/types/size/presents", strue); /* to avoid redetection */
	put("sys/types/size/includes", includes);

	return 0;
}


int find_types_something_t(const char *name, int logdepth, int fatal, const char *prefix, const char *typ, const char *define, const char *try_include)
{
	char *out = NULL;
	int res;
	char test_c[512];
	char node[256], *nodeend;
	const char **include, *includes[] = {"", "#include <stddef.h>", "#include <sys/types.h>", "#include <sys/types/types.h>", "#include <stdlib.h>", "#include <unistd.h>", "#include <stdio.h>", NULL};
	const char ** const first_include = (try_include && *try_include) ? includes : (includes+1);

	char *test_c_include =
		NL "%s"
		NL "int my_puts(const char *s);"
		NL "%s"
		NL "int main() {"
		NL "	%s s;"
		NL "	my_puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL "#include <stdio.h>"
		NL "int my_puts(const char *s)"
		NL "{"
		NL "	return puts(s);"
		NL "}"
		NL;

	char *test_c_size =
		NL "%s"
		NL "#include <stdio.h>"
		NL "%s"
		NL "int main() {"
		NL "	printf(\"%%d\", sizeof(%s));"
		NL "	return 0;"
		NL "}"
		NL;

	char *test_c_broken =
		NL "%s"
		NL "int my_puts(const char *s);"
		NL "%s"
		NL "int main() {"
		NL "	%s s;"
		NL "	void *v;"
		NL "	if (sizeof(v) != sizeof(s)) my_puts(\"yes\");"
		NL "	else my_puts(\"no\");"
		NL "	return 0;"
		NL "}"
		NL "#include <stdio.h>"
		NL "int my_puts(const char *s)"
		NL "{"
		NL "	return puts(s);"
		NL "}"
		NL;

	includes[0] = try_include;

	require("cc/cc", logdepth, fatal);

	report("Checking for type %s... ", typ);
	logprintf(logdepth, "find_types_something_t: Checking for %s...\n", typ);
	logdepth++;

	sprintf(node, "%s/%s", prefix, typ);
	nodeend = node + strlen(node);

	/* replace '*' at the end of the node path with _ptr so it can be saved in the tree */
	if (nodeend[-1] == '*') {
		nodeend--;
		while((nodeend > node) && (*nodeend == ' ')) nodeend--;
		strcpy(nodeend-1, "_ptr");
		nodeend+=4;
	}

	nodeend[0] = '/';
	nodeend[1] = '\0';
	nodeend++;

	if (define == NULL)
		define = "";

	for(include = first_include; *include != NULL; include++) {
		sprintf(test_c, test_c_include, define, *include, typ);
		if ((compile_run(logdepth, test_c, NULL, NULL, NULL, &out) == 0) && out != NULL && (strncmp(out, "OK", 2) == 0)) {
			report("Found; ");
			logprintf(logdepth+1, "include %s works\n", *include);
			sprintf(nodeend, "includes");
			if (define) {
				put(node, define);
				append(node, "\\n");
				append(node, *include);
			} else
				put(node, *include);
			break;
		}
		logprintf(logdepth+1, "include %s fails\n", *include);
		if (out != NULL)
			free(out);
	}
	if (*include == NULL) {
		report("Not found\n");
		return 1;
	}

	sprintf(nodeend, "presents");
	put(node, strue);

	/* check if typ is broken (smaller than void *) */
	sprintf(test_c, test_c_broken, define, *include, typ);
	if (compile_run(logdepth, test_c, NULL, NULL, NULL, &out) == 0) {
		if ((out != NULL) && (strncmp(out, "yes", 3) == 0)) {
			report("(%s is narrower than void *)\n", typ);
			sprintf(nodeend, "broken");
			put(node, strue);
			res = 0;
		}
		else if ((out != NULL) && (strncmp(out, "no", 2) == 0)) {
			report("(%s is not narrower than void *)\n", typ);
			sprintf(nodeend, "broken");
			put(node, sfalse);
			res = 0;
		}
		else {
			report("ERROR: test failed (%s)\n", out);
			res = 1;
		}
	}
	if (out != NULL)
		free(out);

	if (res == 0) {
		report("Checking for size of %s... ", typ);
		sprintf(test_c, test_c_size, define, *include, typ);
		if (compile_run(logdepth, test_c, NULL, NULL, NULL, &out) == 0) {
			if (out != NULL) {
				report("(sizeof %s is %s)\n", typ, out);
				sprintf(nodeend, "size");
				put(node, out);
			}
		}
		if (out != NULL)
			free(out);
	}

	return res;
}


int find_types_size_t(const char *name, int logdepth, int fatal)
{
	return find_types_something_t(name, logdepth, fatal, "sys/types", "size_t", NULL, NULL);
}

int find_types_off_t(const char *name, int logdepth, int fatal)
{
	return find_types_something_t(name, logdepth, fatal, "sys/types", "off_t", NULL, NULL);
}

int find_types_off64_t(const char *name, int logdepth, int fatal)
{
	return find_types_something_t(name, logdepth, fatal, "sys/types", "off64_t", NULL, NULL) &&
	       find_types_something_t(name, logdepth, fatal, "sys/types", "off64_t", "#define _LARGEFILE64_SOURCE", NULL);
}

int find_types_gid_t(const char *name, int logdepth, int fatal)
{
	return find_types_something_t(name, logdepth, fatal, "sys/types", "gid_t", NULL, NULL);
}

int find_types_uid_t(const char *name, int logdepth, int fatal)
{
	return find_types_something_t(name, logdepth, fatal, "sys/types", "uid_t", NULL, NULL);
}

int find_types_pid_t(const char *name, int logdepth, int fatal)
{
	return find_types_something_t(name, logdepth, fatal, "sys/types", "pid_t", NULL, NULL);
}

int find_types_mode_t(const char *name, int logdepth, int fatal)
{
	return find_types_something_t(name, logdepth, fatal, "sys/types", "mode_t", NULL, NULL);
}

int find_types_nlink_t(const char *name, int logdepth, int fatal)
{
	return find_types_something_t(name, logdepth, fatal, "sys/types", "nlink_t", NULL, NULL);
}

int find_types_ptrdiff_t(const char *name, int logdepth, int fatal)
{
	return find_types_something_t(name, logdepth, fatal, "sys/types", "ptrdiff_t", NULL, NULL);
}

int find_types_dev_t(const char *name, int logdepth, int fatal)
{
	return find_types_something_t(name, logdepth, fatal, "sys/types", "dev_t", NULL, NULL);
}

int find_types_ino_t(const char *name, int logdepth, int fatal)
{
	return find_types_something_t(name, logdepth, fatal, "sys/types", "ino_t", NULL, NULL);
}

int find_types_void_ptr(const char *name, int logdepth, int fatal)
{
	return find_types_something_t(name, logdepth, fatal, "sys/types", "void *", NULL, NULL);
}
