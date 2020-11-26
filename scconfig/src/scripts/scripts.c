/*
    scconfig - helper functions for detecting script libs
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
#include <assert.h>
#include "find.h"

void deps_scripts_init()
{
	dep_add("libs/script/gpmi/*",           find_script_gpmi);
	dep_add("libs/script/tcl/*",            find_script_tcl);
	dep_add("libs/script/tk/*",             find_script_tk);
	dep_add("libs/script/ruby/*",           find_script_ruby);
	dep_add("libs/script/mruby/*",          find_script_mruby);
	dep_add("libs/script/python/*",         find_script_python);
	dep_add("libs/script/python3/*",        find_script_python3);
	dep_add("libs/script/perl/*",           find_script_perl);
	dep_add("libs/script/mawk/*",           find_script_mawk);
	dep_add("libs/script/lua/*",            find_script_lua);
	dep_add("libs/script/guile/*",          find_script_guile);
	dep_add("libs/script/stutter/*",        find_script_stutter);
	dep_add("libs/script/estutter/*",       find_script_estutter);
	dep_add("libs/script/funlisp/*",        find_script_funlisp);
	dep_add("libs/script/duktape/*",        find_script_duktape);
	dep_add("libs/script/m4/bin/*",         find_script_m4);

	dep_add("libs/script/fungw/presents",         find_script_fungw);
	dep_add("libs/script/fungw/user_call_ctx/*",  find_script_fungw_user_call_ctx);
	dep_add("libs/script/fungw/cfg_pupdir/*",     find_script_fungw_cfg_pupdir);
	dep_add("libs/script/fungw/*",                find_script_fungw_all);
}

int brute_force_include(int logdepth, const char *language, const char *test_c, const char *ldflags_base, const char *basedir)
{
	char **files, *cflags, *ldflags;
	char nodename[1024], deflink[sizeof(nodename)];
	int fileno, n, res;
	size_t llen;

	if (ldflags_base == NULL)
		ldflags_base = "";

	llen = strlen(language);
	assert(llen < sizeof(nodename) - 16);
	sprintf(nodename, "libs/script/%s", language);
	sprintf(deflink, "-l%s", language);

	res = 0;
	filelist(logdepth, basedir, &fileno, &files);
	if (fileno > 0) {
		for(n = 0; (n < fileno) && !res; n++) {
			if (strncmp(files[n], language, llen) == 0) {
				ldflags = malloc(strlen(files[n]) + strlen(ldflags_base) + 16);
				sprintf(ldflags, "%s -l%s", ldflags_base, files[n]);
				cflags = malloc(strlen(files[n]) + strlen(basedir) + 16);
				sprintf(cflags, "-I%s/%s", basedir, files[n]);
				if (try_icl(logdepth, nodename, test_c, NULL, cflags, ldflags) || try_icl(logdepth, nodename, test_c, NULL, cflags, deflink)) {
					filelist_free(&fileno, &files);
					free(cflags);
					free(ldflags);
					return 1;
				}
				free(cflags);
				free(ldflags);
			}
		}
		filelist_free(&fileno, &files);
	}

	return res;
}
