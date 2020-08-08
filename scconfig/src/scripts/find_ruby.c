/*
    scconfig - ruby lib detection
    Copyright (C) 2009..2015  Tibor Palinkas

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

static int brute_force(int logdepth, const char *test_c, const char *basedir)
{
	char **files, **ifiles, *tmp, *ldflags;
	int fileno, n, m, ifileno, res;

	res = 0;
	filelist(logdepth, basedir, &fileno, &files);
	if (fileno <= 0)
		return res;

	for(n = 0; (n < fileno) && !res; n++) {
		tmp = malloc(strlen(basedir) + strlen(files[n]) + 4);
		sprintf(tmp, "%s/%s", basedir, files[n]);
		filelist(logdepth+1, tmp, &ifileno, &ifiles);
		free(tmp);
		for(m = 0; (m < ifileno) && !res; m++) {
			tmp = malloc(strlen(basedir) + strlen(files[n]) + strlen(ifiles[m]) + 16);
			sprintf(tmp, "%s/%s/%s/ruby.h", basedir, files[n], ifiles[m]);

			if (is_file(tmp)) {
				sprintf(tmp, "-I%s/%s/%s", basedir, files[n], ifiles[m]);
				ldflags = malloc(strlen(files[n]) + 16);
				sprintf(ldflags, "-lruby%s", files[n]);
				res = try_icl(logdepth, "libs/script/ruby", test_c, NULL, tmp, ldflags);
				free(ldflags);
			}
			free(tmp);
		}
		filelist_free(&ifileno, &ifiles);
	}
	filelist_free(&fileno, &files);

	return res;
}

int find_script_ruby(const char *name, int logdepth, int fatal)
{
	int require18;
	const char *require18s;

	char *test_c =
		NL "#include <ruby.h>"
		NL "int main() {"
		NL "	ruby_init();"
		NL "	ruby_finalize();"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	char *test_c18 =
		NL "#include <ruby.h>"
		NL "#include <env.h>"
		NL "int main() {"
		NL "	ruby_init();"
		NL "	ruby_frame->orig_func;"
		NL "	ruby_finalize();"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for ruby... ");
	logprintf(logdepth, "find_ruby: trying to find ruby...\n");
	logdepth++;

	require18s = get("libs/script/ruby/require_18");
	if (require18s != NULL)
		require18 = istrue(require18s);
	else
		require18 = 0;

	if ((!require18) && (try_icl_pkg_config(logdepth, "libs/script/ruby", test_c, NULL, "ruby-*", get("/arg/ruby-version"))))
		return 0;

	if (try_icl_pkg_config(logdepth, "libs/script/ruby", test_c18, NULL, "ruby-*", get("/arg/ruby-version")))
		return 0;

	/* Look at some standard places */
	if ((!require18) && (try_icl(logdepth, "libs/script/ruby", test_c, NULL, NULL, "-lruby")))
		return 0;
	if (try_icl(logdepth, "libs/script/ruby", test_c18, NULL, NULL, "-lruby"))
		return 0;

	require("/internal/filelist/method", logdepth, fatal);

	/* no luck - try to find by brute force, listing directory names */
	if ((!require18) && (brute_force(logdepth, test_c, "/usr/lib/ruby") ||
	    brute_force(logdepth, test_c, "/usr/local/lib/ruby"))) {
	    return 0;
	}
	if (brute_force(logdepth, test_c18, "/usr/lib/ruby") ||
	    brute_force(logdepth, test_c18, "/usr/local/lib/ruby")) {
	    return 0;
	}

	return try_fail(logdepth, "libs/script/ruby");
}
