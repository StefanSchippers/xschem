/*
    scconfig - gui lib detection - libgd
    Copyright (C) 2013  Tibor Palinkas

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
#include <string.h>
#include <assert.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"

int find_gd(const char *name, int logdepth, int fatal, const char *call, const char *arg)
{
	const char *test_c =
		NL "#include \"gd.h\""
		NL "int main() {"
		NL "	gdImagePtr imtype;"
		NL
		NL "	if (gdImageCreateTrueColor(32, 32) != NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/gd";
	char *cflags = NULL;
	char *ldflags = NULL;
	(void) call;  /* not used */
	(void) arg;  /* not used */

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for gd... ");
	logprintf(logdepth, "find_gd: running pkg-config...\n");
	logdepth++;

	if (run_pkg_config(logdepth, "gdlib", &cflags, &ldflags) == 0) {
		if (try_icl(logdepth, node, test_c, NULL, cflags, ldflags) != 0)
			goto success;
	}

	if ((run_gen_config(logdepth, "gdlib-config", "", &cflags, &ldflags) == 0) || (run_pkg_config(logdepth, "gdlib", &cflags, &ldflags) == 0)) {
		char *tmp;

		if (try_icl(logdepth, node, test_c, NULL, cflags, ldflags) != 0)
			goto success;

		/* Some versions of gdlib-config --libs is broken and does not return -lgd */
		tmp = ldflags;
		ldflags = str_concat(" ", ldflags, "-lgd", NULL);
		free(tmp);

		if (try_icl(logdepth, node, test_c, NULL, cflags, ldflags) != 0)
			goto success;

		/* none of the above worked, fail */
		free(cflags);
		free(ldflags);
		return try_fail(logdepth, node);
	}

	return try_fail(logdepth, node);

	success:;
	if (cflags != NULL)
		free(cflags);
	if (ldflags != NULL)
		free(ldflags);
	return 0;
}

int find_gdimagepng(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "#include \"gd.h\""
		NL "int main() {"
		NL "	gdImagePtr img;"
		NL
		NL "	if ((img = gdImageCreateTrueColor(32, 32)) == NULL)"
		NL "		return 1;"
		NL "	gdImagePng(img, stderr);"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/gd/gdImagePng";
	const char *cflags, *ldflags;

	if (require("cc/cc", logdepth, fatal))
		return 1;
	if (require("libs/gui/gd/presents", logdepth, fatal))
		return 1;
	if (!istrue(get("libs/gui/gd/presents")))
		return 1;

	cflags = get("libs/gui/gd/cflags");
	ldflags = get("libs/gui/gd/ldflags");

	report("Checking for gdImagePng... ");
	logprintf(logdepth, "find_gdimagepng: running pkg-config...\n");
	logdepth++;

	if (try_icl(logdepth, node, test_c, NULL, cflags, ldflags) == 0)
		return try_fail(logdepth, node);

	return 0;
}

int find_gdimagejpeg(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "#include \"gd.h\""
		NL "int main() {"
		NL "	gdImagePtr img;"
		NL
		NL "	if ((img = gdImageCreateTrueColor(32, 32)) == NULL)"
		NL "		return 1;"
		NL "	gdImageJpeg(img, stderr, 0.5);"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/gd/gdImageJpeg";
	const char *cflags, *ldflags;

	if (require("cc/cc", logdepth, fatal))
		return 1;
	if (require("libs/gui/gd/presents", logdepth, fatal))
		return 1;
	if (!istrue(get("libs/gui/gd/presents")))
		return 1;

	cflags = get("libs/gui/gd/cflags");
	ldflags = get("libs/gui/gd/ldflags");

	report("Checking for gdImageJpeg... ");
	logprintf(logdepth, "find_gdimagejpeg: running pkg-config...\n");
	logdepth++;

	if (try_icl(logdepth, node, test_c, NULL, cflags, ldflags) == 0)
		return try_fail(logdepth, node);

	return 0;
}

int find_gdimagegif(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "#include \"gd.h\""
		NL "int main() {"
		NL "	gdImagePtr img;"
		NL
		NL "	if ((img = gdImageCreateTrueColor(32, 32)) == NULL)"
		NL "		return 1;"
		NL "	gdImageGif(img, stderr);"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/gd/gdImageGif";
	const char *cflags, *ldflags;

	if (require("cc/cc", logdepth, fatal))
		return 1;
	if (require("libs/gui/gd/presents", logdepth, fatal))
		return 1;
	if (!istrue(get("libs/gui/gd/presents")))
		return 1;

	cflags = get("libs/gui/gd/cflags");
	ldflags = get("libs/gui/gd/ldflags");

	report("Checking for gdImageGif... ");
	logprintf(logdepth, "find_gdimagesetresolution: running pkg-config...\n");
	logdepth++;

	if (try_icl(logdepth, node, test_c, NULL, cflags, ldflags) == 0)
		return try_fail(logdepth, node);

	return 0;
}

int find_gdimagesetresolution(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "#include \"gd.h\""
		NL "int main() {"
		NL "	gdImagePtr img;"
		NL
		NL "	if ((img = gdImageCreateTrueColor(32, 32)) == NULL)"
		NL "		return 1;"
		NL "	gdImageSetResolution(img, 100, 100);"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/gd/gdImageSetResolution";
	const char *cflags, *ldflags;

	if (require("cc/cc", logdepth, fatal))
		return 1;
	if (require("libs/gui/gd/presents", logdepth, fatal))
		return 1;
	if (!istrue(get("libs/gui/gd/presents")))
		return 1;

	cflags = get("libs/gui/gd/cflags");
	ldflags = get("libs/gui/gd/ldflags");

	report("Checking for gdImageSetResolution... ");
	logprintf(logdepth, "find_gdimagesetresolution: running pkg-config...\n");
	logdepth++;

	if (try_icl(logdepth, node, test_c, NULL, cflags, ldflags) == 0)
		return try_fail(logdepth, node);

	return 0;
}
