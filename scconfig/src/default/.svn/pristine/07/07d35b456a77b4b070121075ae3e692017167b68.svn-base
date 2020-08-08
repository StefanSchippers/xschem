/*
    scconfig - detection of standard library features: strings
    Copyright (C) 2017  Tibor Palinkas

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

int find_strcasecmp(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <string.h>"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	if ((strcasecmp(\"foo\", \"FoO\") == 0) && (strcasecmp(\"foo\", \"bar\") != 0))"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for strcasecmp()... ");
	logprintf(logdepth, "find_fs_strcasecmp: trying to find strcasecmp...\n");
	logdepth++;

	if (try_icl(logdepth, "str/strcasecmp", test_c, NULL, NULL, NULL)) return 0;
	return try_fail(logdepth, "str/strcasecmp");
}


int find_strncasecmp(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <string.h>"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	if ((strncasecmp(\"foo1\", \"FoO2\", 3) == 0) && (strncasecmp(\"foo1\", \"bar2\", 3) != 0))"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for strncasecmp()... ");
	logprintf(logdepth, "find_fs_strncasecmp: trying to find strncasecmp...\n");
	logdepth++;

	if (try_icl(logdepth, "str/strncasecmp", test_c, NULL, NULL, NULL)) return 0;
	return try_fail(logdepth, "str/strncasecmp");
}


int find_stricmp(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <string.h>"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	if ((stricmp(\"foo\", \"FoO\") == 0) && (stricmp(\"foo\", \"bar\") != 0))"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for stricmp()... ");
	logprintf(logdepth, "find_fs_stricmp: trying to find stricmp...\n");
	logdepth++;

	if (try_icl(logdepth, "str/stricmp", test_c, NULL, NULL, NULL)) return 0;
	return try_fail(logdepth, "str/stricmp");
}


int find_strnicmp(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <string.h>"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	if ((strnicmp(\"foo1\", \"FoO2\", 3) == 0) && (strnicmp(\"foo1\", \"bar2\", 3) != 0))"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for strnicmp()... ");
	logprintf(logdepth, "find_fs_strnicmp: trying to find strnicmp...\n");
	logdepth++;

	if (try_icl(logdepth, "str/strnicmp", test_c, NULL, NULL, NULL)) return 0;
	return try_fail(logdepth, "str/strnicmp");
}

