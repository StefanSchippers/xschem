/*
    scconfig - python lib detection
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

#include "scripts.h"

static int find_script_python_(const char *name, int logdepth, int fatal, int vermajor, const char *nodedir)
{
	char *ldflags_base, *cflags, *ldflags, *nodebn;
	int iswin32, res;
	char test_c[1024];
	char *test_c_in =
		NL "#include <stdio.h>"
		NL "#include <Python.h>"
		NL "int main() {"
		NL "  char *s;"
		NL "	Py_Initialize();"
		NL
		NL "	s = PY_VERSION;"
		NL "	if (s[0] != '%d') return 1;"
		NL "	if ((s[2] >= '0') && (s[2] <= '9')) puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	char *inc_py =
		NL "import distutils.sysconfig;"
		NL "print '-I' + distutils.sysconfig.get_python_inc().replace('\\\\','/')"
		NL;
	char *lib_py =
		NL "import distutils.sysconfig;"
		NL "print '-L' + distutils.sysconfig.PREFIX.replace('\\\\','/') + '/libs',;"
		NL "import sys;"
		NL "print '-lpython' + str(sys.version_info[0]) + str(sys.version_info[1])"
		NL;


	require("sys/class", logdepth, fatal);
	iswin32 = strcmp(get("sys/class"), "win32") == 0;
	if (iswin32)
		require("libs/lpthread", logdepth, fatal);
	require("cc/cc", logdepth, fatal);
	require("/internal/filelist/method", logdepth, fatal);

	sprintf(test_c, test_c_in, vermajor);

	report("Checking for python (%d)... ", vermajor);
	logprintf(logdepth, "find_python: trying to find python %d...\n", vermajor);
	logdepth++;

	if (iswin32)
		ldflags_base = strclone(get("libs/lpthread"));
	else
		ldflags_base = strclone("-L/usr/bin -L/usr/local/bin");

	/* Look at some standard places */
	if (try_icl(logdepth, nodedir, test_c, NULL, NULL, "-lpython")) return 0;

	/* Ask python using the python executables on path; use + so both runs and can free out of both */
	if (run_script(logdepth, "python", inc_py, ".py", &cflags) + run_script(logdepth, "python", lib_py, ".py", &ldflags) == 0)
		res = try_icl(logdepth, nodedir, test_c, NULL, strip(cflags), strip(ldflags));
	else
		res = 0;

	free(cflags);
	free(ldflags);
	if (res)
		return 0;

	/* no luck - try to find by brute force, listing directory names */
	nodebn = strrchr(nodedir, '/');
	nodebn++;
	if (brute_force_include(logdepth, nodebn, test_c, ldflags_base, "/usr/include") ||
	    brute_force_include(logdepth, nodebn, test_c, ldflags_base, "/usr/local/include")) {
	    free(ldflags_base);
	    return 0;
	}

	free(ldflags_base);

	return try_fail(logdepth, nodedir);
}

int find_script_python(const char *name, int logdepth, int fatal)
{
	return find_script_python_(name, logdepth, fatal, 2, "libs/script/python");
}

int find_script_python3(const char *name, int logdepth, int fatal)
{
	return find_script_python_(name, logdepth, fatal, 3, "libs/script/python3");
}
