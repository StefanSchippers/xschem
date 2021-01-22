/*
    scconfig - detection of standard library features: file system specific calls
    Copyright (C) 2010  Tibor Palinkas
    Copyright (C) 2018  Aron Barath

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

int find_fs_realpath(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <limits.h>"
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "#ifdef PATH_MAX"
		NL "char out_buf[PATH_MAX];"
		NL "#else"
		NL "char out_buf[32768];"
		NL "#endif"
		NL "int main() {"
		NL "	if (realpath(\".\", out_buf) == out_buf)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for realpath()... ");
	logprintf(logdepth, "find_fs_realpath: trying to find realpath...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/fs/realpath", test_c, NULL, NULL, NULL)) return 0;
	if (try_icl(logdepth, "libs/fs/realpath", test_c, "#define _DEFAULT_SOURCE", NULL, NULL)) return 0;
	if (try_icl(logdepth, "libs/fs/realpath", test_c, "#define _BSD_SOURCE", NULL, NULL)) return 0;
	return try_fail(logdepth, "libs/fs/realpath");
}


int find_fs__fullpath(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stdio.h>"
		NL "#include <conio.h>"
		NL "#include <stdlib.h>"
		NL "#include <direct.h>"
		NL "int main() {"
		NL "	char full[_MAX_PATH];"
		NL "	if (_fullpath(full, \".\", _MAX_PATH) != NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for _fullpath()... ");
	logprintf(logdepth, "find_fs__fullpath: trying to find _fullpath...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/fs/_fullpath", test_c, NULL, NULL, NULL)) return 0;
	return try_fail(logdepth, "libs/fs/_fullpath");
}


int find_fs_readdir(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	DIR *dirp;"
		NL "	struct dirent *dp;"
		NL "	int found = 0;"
		NL "	if ((dirp = opendir(\".\")) == 0)"
		NL "		return -1;"
		NL "	while ((dp = readdir(dirp)) != 0)"
		NL "		if (strcmp(dp->d_name, \"configure\") == 0)"
		NL "			found++;"
		NL "	closedir(dirp);"
		NL "	if (found == 1)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	char *includes[] = {
		"#include <dirent.h>",
		"#include <sys/dir.h>",    /* 4.2BSD */
		NULL
	};
	char **i;

	require("cc/cc", logdepth, fatal);

	report("Checking for readdir()... ");
	logprintf(logdepth, "find_fs_readdir: trying to find readdir...\n");
	logdepth++;

	for (i = includes; *i != NULL; i++)
		if (try_icl(logdepth, "libs/fs/readdir", test_c, *i, NULL, NULL))
			return 0;
	return try_fail(logdepth, "libs/fs/readdir");
}


int find_fs_findnextfile(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "#include <windows.h>"
		NL "int main(int argc, char *argv[]) {"
		NL "	WIN32_FIND_DATA fd;"
		NL "	HANDLE h;"
		NL "	int found=0;"
		NL "	h = FindFirstFile(argv[0], &fd);"
		NL "	if (h == INVALID_HANDLE_VALUE)"
		NL "		return -1;"
		NL "	while (FindNextFile(h, &fd) != 0);"
		NL "		found++;"
		NL "	FindClose(h);"
		NL "	if (found > 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for FindNextFile()... ");
	logprintf(logdepth, "find_fs_findnextfile: trying to find FindNextFile...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/fs/findnextfile", test_c, NULL, NULL, NULL)) return 0;
	return try_fail(logdepth, "libs/fs/findnextfile");
}

int find_fs_access(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "int my_test() { return access(\".\", 0); }"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	if (my_test() == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char* includes[] = { "#include <unistd.h>", "#include <stdlib.h>\n#include <direct.h>", "#include <stdlib.h>", NULL };
	const char** inc;

	require("cc/cc", logdepth, fatal);

	report("Checking for access()... ");
	logprintf(logdepth, "find_fs_access: trying to find access...\n");
	logdepth++;

	for (inc=includes; *inc; ++inc)
		if (try_icl(logdepth, "libs/fs/access", test_c, *inc, NULL, NULL)) return 0;

	return try_fail(logdepth, "libs/fs/access");
}

int find_fs_access_macros(const char *rname, int logdepth, int fatal)
{
	char *test_c_templ =
		NL "%s"
		NL "void my_test() { int a = %s; }"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	my_test();"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	char test_c[256];

	char *names[][3] = {
		{"F_OK",  "F_OK",  NULL},
		{"R_OK",  "R_OK",  NULL},
		{"W_OK",  "W_OK",  NULL},
		{"X_OK",  "X_OK",  NULL},
		{NULL,    NULL,    NULL}
		};
	char **n;
	const char* access_includes;
	int name, pr;
	char nodename[128];

	require("cc/cc", logdepth, fatal);
	if (require("libs/fs/access/*", logdepth, fatal)!=0 ||
		!istrue(get("libs/fs/access/presents"))) {
		put("libs/fs/access/macros/presents", sfalse);
		return 1;
	}
	access_includes = get("libs/fs/access/includes");

	report("Checking for access macros:\n");
	logprintf(logdepth, "find_fs_access_macros: trying to find access macros...\n");
	logdepth++;

	pr = 0;
	for(name = 0; *names[name] != NULL; name++) {
		report(" %s...\t", names[name][0]);
		for(n = &names[name][0]; *n != NULL; n++) {
			sprintf(test_c, test_c_templ, access_includes, *n);
			if (try_icl(logdepth, NULL, test_c, NULL, NULL, NULL)) {
				sprintf(nodename, "libs/fs/access/macros/%s", names[name][0]);
				put(nodename, *n);
				report("found as %s\n", *n);
				pr++;
				goto found;
			}
		}
		report("not found\n");
		found:;
	}

	put("libs/fs/access/macros/presents", ((pr > 0) ? (strue) : (sfalse)));
	return (pr == 0);
}

int find_fs_stat_macros(const char *rname, int logdepth, int fatal)
{
	char *test_c_templ =
		NL "#include <sys/stat.h>"
		NL "#include <sys/types.h>"
		NL "void my_test() { int a = %s(0); }"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	my_test();"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	char test_c[256];

	char *names[][3] = {
		{"S_ISREG",  "S_IFREG",  NULL},
		{"S_ISDIR",  "S_IFDIR",  NULL},
		{"S_ISCHR",  "S_IFCHR",  NULL},
		{"S_ISBLK",  "S_IFBLK",  NULL},
		{"S_ISFIFO", "S_IFFIFO", NULL},
		{"S_ISLNK",  "S_IFLNK",  NULL},
		{"S_ISCHR",  "S_IFCHR",  NULL},
		{"S_ISSOCK", "S_IFSOCK", NULL},
		{NULL,       NULL,       NULL}
		};
	char **n;
	int name, pr;
	char nodename[128];

	require("cc/cc", logdepth, fatal);

	report("Checking for stat macros:\n");
	logprintf(logdepth, "find_fs_stat_macros: trying to find stat macros...\n");
	logdepth++;

	pr = 0;
	for(name = 0; *names[name] != NULL; name++) {
		report(" %s...\t", names[name][0]);
		for(n = &names[name][0]; *n != NULL; n++) {
			sprintf(test_c, test_c_templ, *n);
			if (try_icl(logdepth, NULL, test_c, NULL, NULL, NULL)) {
				sprintf(nodename, "libs/fs/stat/macros/%s", names[name][0]);
				put(nodename, *n);
				report("found as %s\n", *n);
				pr++;
				goto found;
			}
		}
		report("not found\n");
		found:;
	}

	put("libs/fs/stat/macros/presents", ((pr > 0) ? (strue) : (sfalse)));
	return (pr == 0);
}

int find_fs_stat_fields(const char *rname, int logdepth, int fatal)
{
	char *test_c_templ =
		NL "#include <sys/stat.h>"
		NL "#include <sys/types.h>"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	struct stat st;"
		NL "	(void)st.%s;"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	char test_c[256];

	char *names[] = {"st_blksize", "st_blocks", "st_rdev", "st_mtim", "st_mtime", "st_birthtim", "st_birthtime", NULL };
	int name, pr;
	char nodename[128];

	require("cc/cc", logdepth, fatal);

	report("Checking for stat macros:\n");
	logprintf(logdepth, "find_fs_stat_fields: trying to find stat macros...\n");
	logdepth++;

	pr = 0;
	for(name = 0; names[name] != NULL; name++) {
		report(" %s...\t", names[name]);
		sprintf(test_c, test_c_templ, names[name]);
		sprintf(nodename, "libs/fs/stat/fields/%s/presents", names[name]);
		if (try_icl(logdepth, NULL, test_c, NULL, NULL, NULL)) {
			put(nodename, strue);
			report("found\n");
			pr++;
		}
		else {
			report("not found\n");
			put(nodename, sfalse);
		}
	}
	return (pr == 0);
}

static int find_fs_any_lstat(const char *name, int logdepth, int fatal, char *fn)
{
	/* make sure <stdio.h> does not affect our lstat() detection */
	const char *test_c_in =
		NL "void my_puts(const char *s);"
		NL "int main() {"
		NL "	struct stat buf;"
		NL "	if (%s(\".\", &buf) == 0)"
		NL "		my_puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL "#include <stdio.h>"
		NL "void my_puts(const char *s)"
		NL "{"
		NL "	puts(s);"
		NL "}"
		NL;
	char test_c[384], node[64];
	const char *incs[] = {"#include <sys/stat.h>", "#include <unistd.h>", "#include <sys/types.h>\n#include <sys/stat.h>\n#include <unistd.h>", NULL};
	const char **inc;

	require("cc/cc", logdepth, fatal);

	sprintf(node, "libs/fs/%s", fn);
	sprintf(test_c, test_c_in, fn);

	report("Checking for %s... ", fn);
	logprintf(logdepth, "find_fs_%s: trying to find lstat()...\n", fn);
	logdepth++;

	for (inc = incs; *inc; ++inc) {
		if (try_icl(logdepth, node, test_c, *inc, NULL, NULL))
			return 0;
	}

	return try_fail(logdepth, node);
}

int find_fs_lstat(const char *name, int logdepth, int fatal)
{
	return find_fs_any_lstat(name, logdepth, fatal, "lstat");
}

int find_fs_statlstat(const char *name, int logdepth, int fatal)
{
	return find_fs_any_lstat(name, logdepth, fatal, "statlstat");
}


int find_fs_getcwd(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <unistd.h>"
		NL "int main() {"
		NL "	char b[1024];"
		NL "	if (getcwd(b, sizeof(b)) != NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for getcwd... ");
	logprintf(logdepth, "find_fs_getcwd: trying to find getcwd()...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/fs/getcwd", test_c, NULL, NULL, NULL)) return 0;
	return try_fail(logdepth, "libs/fs/getcwd");
}

int find_fs__getcwd(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stdlib.h>"
		NL "int main() {"
		NL "	char b[1024];"
		NL "	if (_getcwd(b, sizeof(b)) != NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for _getcwd... ");
	logprintf(logdepth, "find_fs__getcwd: trying to find _getcwd()...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/fs/_getcwd", test_c, "#include <direct.h>", NULL, NULL)) return 0;
	return try_fail(logdepth, "libs/fs/_getcwd");
}


int find_fs_getwd(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <unistd.h>"
		NL "int main() {"
		NL "	char b[8192];"
		NL "	if (getwd(b) != NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for getwd... ");
	logprintf(logdepth, "find_fs_getwd: trying to find getwd()...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/fs/getwd", test_c, NULL, NULL, NULL)) return 0;
	return try_fail(logdepth, "libs/fs/getwd");
}

int find_fs_mkdir(const char *name, int logdepth, int fatal)
{
	char *dir;
	char test_c[1024];
	char *test_c_in =
		NL "#include <stdio.h>"
		NL "int main() {"
		NL no_implicit(int, "mkdir", "mkdir")
		NL "	if (mkdir(\"%s\"%s) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	dir = tempfile_new("");
	unlink(dir);

	report("Checking for mkdir... ");
	logprintf(logdepth, "find_fs_mkdir: trying to find mkdir()...\n");
	logdepth++;

	/* POSIX, 2 arguments, standard includes */
	sprintf(test_c, test_c_in, dir, ", 0755");
	if (try_icl(logdepth, "libs/fs/mkdir", test_c, "#include <sys/types.h>\n#include <sys/stat.h>\n", NULL, NULL)) {
		if (!is_dir(dir))
			goto oops1;
		put("libs/fs/mkdir/num_args", "2");
		rmdir(dir);
		return 0;
	}

	/* POSIX, 2 arguments, no includes */
	oops1:;
	sprintf(test_c, test_c_in, dir, ", 0755");
	if (try_icl(logdepth, "libs/fs/mkdir", test_c, NULL, NULL, NULL)) {
		if (!is_dir(dir))
			goto oops2;
		put("libs/fs/mkdir/num_args", "2");
		rmdir(dir);
		return 0;
	}

	/* win32, 1 argument, with <direct.h> */
	oops2:;
	sprintf(test_c, test_c_in, dir, "");
	if (try_icl(logdepth, "libs/fs/mkdir", test_c, "#include <direct.h>\n", NULL, NULL)) {
		if (!is_dir(dir))
			goto oops3;
		put("libs/fs/mkdir/num_args", "1");
		rmdir(dir);
		return 0;
	}

	oops3:;
	put("libs/fs/mkdir/includes", "");
	put("libs/fs/mkdir/ldflags", "");
	put("libs/fs/mkdir/cdflags", "");

	rmdir(dir);
	return try_fail(logdepth, "libs/fs/mkdir");
}

int find_fs__mkdir(const char *name, int logdepth, int fatal)
{
	char *dir;
	char test_c[1024];
	char *test_c_in =
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	if (_mkdir(\"%s\"%s) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	dir = tempfile_new("");
	unlink(dir);

	report("Checking for _mkdir... ");
	logprintf(logdepth, "find_fs__mkdir: trying to find _mkdir()...\n");
	logdepth++;

	/* win32, 2 arguments, standard includes */
	sprintf(test_c, test_c_in, dir, ", 0755");
	if (try_icl(logdepth, "libs/fs/_mkdir", test_c, "#include <direct.h>\n", NULL, NULL)) {
		if (!is_dir(dir))
			goto oops1;
		put("libs/fs/_mkdir/num_args", "2");
		rmdir(dir);
		return 0;
	}

	oops1:;
	/* win32, 1 argument, standard includes */
	sprintf(test_c, test_c_in, dir, "");
	if (try_icl(logdepth, "libs/fs/_mkdir", test_c, "#include <direct.h>\n", NULL, NULL)) {
		if (!is_dir(dir))
			goto oops2;
		put("libs/fs/_mkdir/num_args", "1");
		rmdir(dir);
		return 0;
	}

	oops2:;
	put("libs/fs/_mkdir/includes", "");
	put("libs/fs/_mkdir/ldflags", "");
	put("libs/fs/_mkdir/cdflags", "");

	rmdir(dir);
	return try_fail(logdepth, "libs/fs/_mkdir");
}

static int find_utime_impl(const char *name, int logdepth, int fatal,
	const char* key, const char* funcname, const char* typename)
{
	char test_c[1024+4096];
	const char *test_c_templ =
		NL "void puts_OK();"
		NL "int main(int argc, char* argv[])"
		NL "{"
		NL "	struct %s buf;"
		NL "	buf.actime = buf.modtime = 1610958044;"
		NL "	if (%s(\"%s\", &buf) == 0)"
		NL "		puts_OK();"
		NL "	return 0;"
		NL "}"
		NL "#include <stdio.h>"
		NL "void puts_OK()"
		NL "{"
		NL "	puts(\"OK\");"
		NL "}"
		NL;

	const char* includes[] =
	{
		/* *NIX */
		"#include <sys/types.h>\n#include <utime.h>",

		/* windoz */
		"#include <sys/utime.h>",

		NULL
	};
	const char** inc;
	char* tmpf;

	tmpf = tempfile_new(".txt");
	sprintf(test_c, test_c_templ, typename, funcname, tmpf);

	require("cc/cc", logdepth, fatal);

	report("Checking for %s... ", funcname);
	logprintf(logdepth, "find_fs_%s: trying to find %s()...\n", funcname, funcname);
	logdepth++;

	for (inc=includes; *inc; ++inc)
	{
		if (try_icl(logdepth, key, test_c, *inc, NULL, NULL))
		{
			unlink(tmpf);
			free(tmpf);
			return 0;
		}
	}

	unlink(tmpf);
	free(tmpf);

	return try_fail(logdepth, key);
}

int find_fs_utime(const char *name, int logdepth, int fatal)
{
	return find_utime_impl(name, logdepth, fatal,
		"libs/fs/utime", "utime", "utimbuf");
}

int find_fs__utime(const char *name, int logdepth, int fatal)
{
	return find_utime_impl(name, logdepth, fatal,
		"libs/fs/_utime", "_utime", "_utimbuf");
}

int find_fs__utime64(const char *name, int logdepth, int fatal)
{
	return find_utime_impl(name, logdepth, fatal,
		"libs/fs/_utime64", "_utime64", "__utimbuf64");
}

int find_fs_mkdtemp(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stdio.h>"
		NL "#include <unistd.h>"
		NL "#include <string.h>"
		NL "int main() {"
		NL "	char fn[32], *o;"
		NL "	strcpy(fn, \"scc.XXXXXX\");"
		NL "	o = mkdtemp(fn);"
		NL "	if ((o != NULL) && (strstr(o, \"scc.\") != NULL)) {"
		NL "		remove(o);"
		NL "		puts(\"OK\");"
		NL "	}"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for mkdtemp... ");
	logprintf(logdepth, "find_fs_mkdtemp: trying to find mkdtemp()...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/fs/mkdtemp", test_c, "#include <stdlib.h>\n", NULL, NULL)) return 0;
	if (try_icl(logdepth, "libs/fs/mkdtemp", test_c, "#define _BSD_SOURCE\n#include <stdlib.h>\n", NULL, NULL)) return 0;
	return try_fail(logdepth, "libs/fs/mkdtemp");
}

int find_fs_mmap(const char *name, int logdepth, int fatal)
{
	char test_c[1024];
	char *tmp;
	FILE *f;
	char *test_c_in =
		NL "#include <stdio.h>"
		NL "#include <unistd.h>"
		NL "#include <string.h>"
		NL "#include <sys/types.h>"
		NL "#include <sys/stat.h>"
		NL "#include <fcntl.h>"
		NL "int main() {"
		NL "	int fd, size = 11;"
		NL "	void *p;"
		NL "	fd = open(\"%s\", O_RDONLY);"
		NL "	p = mmap(0, size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);"
		NL "	if (p == NULL) {"
		NL "		puts(\"mmap fail\");"
		NL "		return 0;"
		NL "	}"
		NL "	if (strcmp(p, \"hello world\") != 0) {"
		NL "		puts(\"strcmp fail\");"
		NL "		return 0;"
		NL "	}"
		NL "	if (munmap(p, size) != 0) {"
		NL "		puts(\"munmap fail\");"
		NL "		return 0;"
		NL "	}"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	tmp = tempfile_new("");
	f = fopen(tmp, "w");
	fprintf(f, "hello world");
	fclose(f);
	sprintf(test_c, test_c_in, tmp);

	report("Checking for mmap... ");
	logprintf(logdepth, "find_fs_mmap: trying to find mmap()...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/fs/mmap", test_c, "#include <sys/mman.h>\n", NULL, NULL)) {
		unlink(tmp);
		free(tmp);
		return 0;
	}

	unlink(tmp);
	free(tmp);
	return try_fail(logdepth, "libs/fs/mmap");
}

/* Haiku/BeOS next_dev */
int find_fsmount_next_dev(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	int32 pos = 0;"
		NL "	dev_t res = next_dev(&pos);"
		NL "	if (res >= 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for next_dev... ");
	logprintf(logdepth, "find_fsmount_next_dev: trying to find next_dev()...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/fsmount/next_dev", test_c, "#include <fs_info.h>\n", NULL, NULL)) return 0;
	return try_fail(logdepth, "libs/fsmount/next_dev");
}

int find_fsmount_fsstat_fields(const char *name, int logdepth, int fatal)
{
	const char *fields[] = {"f_fstypename", NULL};
	return try_icl_sfields(logdepth, "libs/fsmount/struct_fsstat", "struct fsstat", fields, "#include <sys/fsstat.h>", NULL, NULL, 0);
}

int find_fsmount_statfs_fields(const char *name, int logdepth, int fatal)
{
	const char *fields[] = {"f_fstypename", "f_type", NULL};
	return try_icl_sfields(logdepth, "libs/fsmount/struct_statfs", "struct statfs", fields, "#include <sys/statfs.h>", NULL, NULL, 0);
}

int find_fsmount_statvfs_fields(const char *name, int logdepth, int fatal)
{
	const char *fields[] = {"f_fstypename", "f_type", "f_basetype", NULL};
	return try_icl_sfields(logdepth, "libs/fsmount/struct_statvfs", "struct statvfs", fields, "#include <sys/statvfs.h>", NULL, NULL, 0);
}

int find_fs_ustat(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/fs/ustat";
	const char *test_c =
		NL "#include <stdio.h>"
		NL "#include <sys/stat.h>"
		NL "int main()"
		NL "{"
		NL "	struct stat stat_buf;"
		NL "	struct ustat ustat_buf;"
		NL "	if (stat(\".\", &stat_buf) == 0 &&"
		NL "		ustat(stat_buf.st_dev, &ustat_buf) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for ustat... ");
	logprintf(logdepth, "find_fs_ustat: trying to find ustat()...\n");
	logdepth++;

	if (try_icl(logdepth, key, test_c, "#include <ustat.h>", NULL, NULL)) return 0;
	if (try_icl(logdepth, key, test_c, "#include <unistd.h>", NULL, NULL)) return 0;
	if (try_icl(logdepth, key, test_c, "#include <sys/types.h>\n#include <unistd.h>", NULL, NULL)) return 0;
	if (try_icl(logdepth, key, test_c, "#include <sys/types.h>\n#include <unistd.h>\n#include <ustat.h>", NULL, NULL)) return 0;
	return try_fail(logdepth, key);
}

int find_fs_statfs(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/fs/statfs";
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	struct statfs statfs_buf;"
		NL "	if (statfs(\".\", &statfs_buf) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for statfs... ");
	logprintf(logdepth, "find_fs_statfs: trying to find statfs()...\n");
	logdepth++;

	if (try_icl(logdepth, key, test_c, "#include <sys/statfs.h>", NULL, NULL)) return 0;
	if (try_icl(logdepth, key, test_c, "#include <sys/vfs.h>", NULL, NULL)) return 0;
	return try_fail(logdepth, key);
}

int find_fs_statvfs(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/fs/statvfs";
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	struct statvfs statvfs_buf;"
		NL "	if (statvfs(\".\", &statvfs_buf) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for statvfs... ");
	logprintf(logdepth, "find_fs_statvfs: trying to find statvfs()...\n");
	logdepth++;

	if (try_icl(logdepth, key, test_c, "#include <sys/statvfs.h>", NULL, NULL)) return 0;
	return try_fail(logdepth, key);
}

int find_fs_flock(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/fs/flock";
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	if (flock(1, LOCK_UN) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for flock... ");
	logprintf(logdepth, "find_fs_flock: trying to find flock()...\n");
	logdepth++;

	if (try_icl(logdepth, key, test_c, "#include <sys/file.h>", NULL, NULL)) return 0;
	return try_fail(logdepth, key);
}

int find_fs_makedev(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/fs/makedev";
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	if (1 == major(makedev(1, 2)) && 2 == minor(makedev(1, 2)))"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *includes[] = {
		"#include <sys/sysmacros.h>",
		NULL
	};
	const char **i;

	require("cc/cc", logdepth, fatal);

	report("Checking for makedev()... ");
	logprintf(logdepth, "find_fs_makedev: trying to find makedev...\n");
	logdepth++;

	for (i = includes; *i != NULL; i++)
		if (try_icl(logdepth, key, test_c, *i, NULL, NULL))
			return 0;
	return try_fail(logdepth, key);
}
