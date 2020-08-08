/*
    scconfig - helpers for a main()
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

#ifdef PLUGIN_SCRIPTS
#include "../scripts/INIT.h"
#endif

#ifdef PLUGIN_PARSER
#include "../parser/INIT.h"
#endif

#ifdef PLUGIN_C99
#include "../c99/INIT.h"
#endif

#ifdef PLUGIN_PARSGEN
#include "../parsgen/INIT.h"
#endif

#ifdef PLUGIN_MATH
#include "../math/INIT.h"
#endif

#ifdef PLUGIN_SOCKET
#include "../socket/INIT.h"
#endif

#ifdef PLUGIN_USERPASS
#include "../userpass/INIT.h"
#endif

#ifdef PLUGIN_GUI
#include "../gui/INIT.h"
#endif

#ifdef PLUGIN_TTY
#include "../tty/INIT.h"
#endif

#ifdef PLUGIN_SUL
#include "../sul/INIT.h"
#endif

#ifdef PLUGIN_POSIX
#include "../posix/INIT.h"
#endif

#ifdef PLUGIN_GENERATOR
#include "generator.h"
#endif


void init(void)
{
	db_init();
	log_init();
	dep_init();
	deps_default_init();

	/* common internal directory */
	db_mkdir("/internal");

	/* We have a host system for sure - also make it the default */
	db_mkdir("/host");
	db_cd("/host");

	/* emulator for the host system is empty string */
	put("sys/emu", "");

#ifdef PLUGIN_SCRIPTS
#include "../scripts/INIT.c"
#endif

#ifdef PLUGIN_PARSER
#include "../parser/INIT.c"
#endif

#ifdef PLUGIN_C99
#include "../c99/INIT.c"
#endif

#ifdef PLUGIN_PARSGEN
#include "../parsgen/INIT.c"
#endif

#ifdef PLUGIN_MATH
#include "../math/INIT.c"
#endif

#ifdef PLUGIN_SOCKET
#include "../socket/INIT.c"
#endif

#ifdef PLUGIN_USERPASS
#include "../userpass/INIT.c"
#endif

#ifdef PLUGIN_GUI
#include "../gui/INIT.c"
#endif

#ifdef PLUGIN_TTY
#include "../tty/INIT.c"
#endif

#ifdef PLUGIN_SUL
#include "../sul/INIT.c"
#endif

#ifdef PLUGIN_POSIX
#include "../posix/INIT.c"
#endif

#ifdef PLUGIN_GENERATOR
#include "../generator/INIT.c"
#endif
}

void uninit(void)
{
	log_uninit();
	dep_uninit();
	db_uninit();
}

void run_custom_reqs(void)
{
	int n;
	if (num_custom_reqs > 0) {
		printf("Running custom requirements\n");
		for(n = 0; n < num_custom_reqs; n++) {
			if (custom_reqs[n] == NULL) {
				fprintf(stderr, "Error: requested detection of empty string - please check your command line, syntax is --detect=node\n");
				exit(1);
			}
			require(custom_reqs[n], 1, 1);
		}
	}
}

int main_init(void)
{
	re_modw("./\\");
	if (hook_preinit()) {
		fprintf(stderr, "hook_preinit failed, exiting\n");
		return 1;
	}
	init();
	if (hook_postinit())  {
		fprintf(stderr, "hook_postinit failed, exiting\n");
		return 1;
	}
	return 0;
}

int main_process_args(int argc, char *argv[])
{
	process_args(argc, argv);
	if (hook_postarg()) {
		fprintf(stderr, "hook_postarg failed, exiting\n");
		return 1;
	}
	return 0;
}

void main_uninit(void)
{
	hook_preuninit();
	uninit();
	hook_postuninit();
}
