/*
    scconfig - test code for default and scripts
    Copyright (C) 2009..2016  Tibor Palinkas

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
#include "db.h"
#include "find.h"
#include "log.h"
#include "arg.h"
#include "dep.h"
#include "deps_default.h"
#include "libs.h"
#include "hooks.h"
#include "regex.h"
#include "main_custom_args.h"
#include "main_lib.h"

void re_fail(char *s, char c)
{
	fprintf(stderr, "Regex error: %s [opcode %o]\n", s, c);
	abort();
}

int no_autodetect_sys = 0;
int no_save_cache = 0;
int main(int argc, char *argv[])
{
	int blind_save;

	if (main_init() != 0)
		return 1;

	if (main_process_args(argc, argv) != 0)
		return 1;

	if (!no_autodetect_sys) {
		find_target("", 0, 1);
		blind_save = cross_blind;
		cross_blind = 0;
		printf("--- Detecting host\n");

		require("sys/name", 0, 1);
	}

	if (hook_detect_host())  {
		fprintf(stderr, "hook_detect_host failed, exiting\n");
		return 1;
	}

	cross_blind = blind_save;
	if (!no_autodetect_sys) {
		if (!iscross)
			printf("--- Detecting target (same as host)\n");
		else
			printf("--- Detecting target (differs from host)\n");
	}
	db_cd("/target");
	run_custom_reqs();


	if (hook_detect_target()) {
		fprintf(stderr, "hook_detect_target failed, exiting\n");
		return 1;
	}

#ifdef RUNTIME
	if (!no_autodetect_sys) {
		if (!iscross)
			printf("--- Detecting runtime (same as host)\n");
		else
			printf("--- Detecting runtime (differs from host)\n");
	}
	db_cd("/runtime");
	if (hook_detect_runtime()) {
		fprintf(stderr, "hook_detect_runtime failed, exiting\n");
		return 1;
	}
#endif

	if (hook_generate()) {
		fprintf(stderr, "hook_generate failed, exiting\n");
		return 1;
	}

	if (!no_save_cache)
		export("config.cache", 1, "/");

	main_uninit();
	return 0;
}

