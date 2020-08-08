/*
    scconfig - libgpmi detection
    Copyright (C) 2015  Tibor Palinkas

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
#include <unistd.h>


int find_script_gpmi(const char *name, int logdepth, int fatal)
{
	char *out, *cmd, *end, **cfg;
	const char *usr;
	char *cfgs[] = {
		NULL,
		"gpmi-config",
		"/usr/bin/gpmi-config",
		"/usr/local/bin/gpmi-config",
		"~/bin/gpmi-config",
		NULL
	};

	(void) fatal;  /* not used */

	report("Checking for gpmi... ");
	logprintf(logdepth, "find_gpmi: trying to find gpmi...\n");
	logdepth++;

	/* Try the config script */
	logprintf(logdepth, "running config tcl\n");
	cmd = malloc(128);
	usr = get("/arg/gpmi/prefix");
	if (usr != NULL) {
		const char *rp1 = NULL;
		require("cc/wlrpath", logdepth, 0);
		rp1 = get("cc/wlrpath");
		cfgs[0] = str_concat("/", usr, "bin/gpmi-config", NULL);
		put("libs/script/gpmi/ldflags", str_concat("", "-L", usr, "/lib", " -lgpmi ", rp1, usr, "/lib", NULL));
		put("libs/script/gpmi/cflags", str_concat("", "-I", usr, "/include", NULL));
		cfg = cfgs;
	}
	else {
		cfg = cfgs+1;
		put("libs/script/gpmi/cflags", "");
		put("libs/script/gpmi/ldflags", "-lgpmi");
	}
	for(; *cfg != NULL; cfg++) {
		sprintf(cmd, "%s --version", *cfg);
		if (run(logdepth+1, cmd, &out) == 0) {
			put("libs/script/gpmi/gpmi-config", *cfg);
			put("libs/script/gpmi/presents", strue);
			end = strrchr(out, ' ');
			if (end != NULL)
				put("libs/script/gpmi/version", strip(end));
			free(out);
			sprintf(cmd, "%s --id", *cfg);
			if (run(logdepth+1, cmd, &out) == 0) {
				end = strrchr(out, ' ');
				if (end != NULL)
					put("libs/script/gpmi/configapi", strip(end));
				free(out);
			}
			free(cmd);
			if (cfgs[0] != NULL)
				free(cfgs[0]);
			return 0;
		}
	}
	free(cmd);
	if (out != NULL)
		free(out);

	if (cfg[0] != NULL)
		free(cfg[0]);

	return try_fail(logdepth, "libs/script/gpmi");
}
