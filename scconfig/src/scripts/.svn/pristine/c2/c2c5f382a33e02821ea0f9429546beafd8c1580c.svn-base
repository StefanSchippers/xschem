/*
    scconfig - tcl lib detection
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
#include <unistd.h>

static int all_vers[] = { 86, 85, 84, 0, -1 };

int find_script_tcl_(const char *name, int logdepth, int fatal, int *vers, int fallback)
{
	char *out, *temp, *temp2, *cmd, *I, *L, *end, **tclsh;
	int *v;

	char *tclshs[] = {
		"tclsh",
		"tclsh86",
		"tclsh85",
		"tclsh84",
		"tclsh8.6",
		"tclsh8.5",
		"tclsh8.4",
		NULL
	};

	char *test_c =
		NL "#include <tcl.h>"
		NL "int main() {"
		NL "	Tcl_Obj *res;"
		NL "	Tcl_Interp *interp;"
		NL "	interp = Tcl_CreateInterp();"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	char *tcl_config =
			NL "proc tclConfigFile {} {"
			NL "	set d [info library]"
			NL "	set f [file join $d \"tclConfig.sh\"]"
			NL "	if {[file exists $f]} {return $f}"
			NL ""
			NL "	set d [file dirname $d]"
			NL "	set f [file join $d \"tclConfig.sh\"]"
			NL "	if {[file exists $f]} {return $f}"
			NL ""
			NL "	set d [file dirname $d]"
			NL "	set f [file join $d \"tclConfig.sh\"]"
			NL "	if {[file exists $f]} {return $f}"
			NL ""
			NL "	set d [file dirname $d]"
			NL "	set f [file join $d \"tclConfig.sh\"]"
			NL "	if {[file exists $f]} {return $f}"
			NL "}"
			NL ""
			NL "puts [tclConfigFile]"
			NL;

	/* Look at some standard places */
	for(v = vers; *v != -1; v++) {
		int major = *v / 10, minor = *v % 10;
		char ifl[64], lfl[64];

		if (*v == 0) {
			report("plain... ");
			if (try_icl(logdepth, "libs/script/tcl", test_c, NULL, NULL, "-ltcl")) return 0;
			continue;
		}

		report("%d.%d... ", major, minor);

		sprintf(lfl, "-ltcl%d", *v);
		if (try_icl(logdepth, "libs/script/tcl", test_c, NULL, NULL, lfl)) return 0;

		sprintf(lfl, "-ltcl%d.%d", major, minor);
		if (try_icl(logdepth, "libs/script/tcl", test_c, NULL, NULL, lfl)) return 0;

		sprintf(lfl, "-ltcl%d", *v);
		sprintf(ifl, "-I/usr/include/tcl%d", *v);
		if (try_icl(logdepth, "libs/script/tcl", test_c, NULL, ifl, lfl)) return 0;

		sprintf(lfl, "-ltcl%d.%d", major, minor);
		sprintf(ifl, "-I/usr/include/tcl%d.%d", major, minor);
		if (try_icl(logdepth, "libs/script/tcl", test_c, NULL, ifl, lfl)) return 0;
	}

	if (!fallback)
		return 1;


	report(" not found by version, trying tclsh.\n");

	/* Try the config script */
	logprintf(logdepth, "running config tcl\n");
	temp = tempfile_dump(tcl_config, ".tcl");
	cmd = malloc(strlen(temp) + 16);
	for(tclsh = tclshs; *tclsh != NULL; tclsh++) {
		sprintf(cmd, "%s %s", *tclsh, temp);
		report("Trying: %s\n", cmd);
		if (run(logdepth+1, cmd, &out) == 0) {
			free(cmd);
			cmd = malloc(strlen(out) + 256);
			sprintf(cmd, "#!/bin/sh\n. %s\necho $TCL_INCLUDE_SPEC\necho $TCL_LIB_SPEC\n", out);
			temp2 = tempfile_dump(cmd, ".sh");
			free(out);
			out = malloc(strlen(temp2) + 32);
			sprintf(out, "chmod +x %s", temp2);
			system(out);
			free(out);
			if (run(logdepth+1, temp2, &out) == 0) {
				remove(temp2);
				I = out;
				L = strchr(I, '\n');
				if (L != NULL) {
					*L = '\0';
					L++;
					end = strchr(L, '\n');
					if (end != NULL)
						*end = '\0';
				}
				if (try_icl(logdepth, "libs/script/tcl", test_c, NULL, I, L)) {
					remove(temp);
					free(cmd);
					if (out != NULL)
						free(out);
					return 0;
				}
			}
			remove(temp2);
		}
	}
	remove(temp);
	free(cmd);
	if (out != NULL)
		free(out);

	return 1;
}

int find_script_tcl(const char *name, int logdepth, int fatal)
{
	int res, rver[2], *vers = all_vers, fallback = 1;
	const char *reqver;


	require("cc/cc", logdepth, fatal);

	report("Checking for tcl... ");
	logprintf(logdepth, "find_tcl: trying to find tcl...\n");
	logdepth++;

	reqver = get("/arg/tcl-version");
	if ((reqver != NULL) && (reqver[0] != '\0')) {
		if (reqver[1] == '.')
			rver[0] = (reqver[0] - '0') * 10 + (reqver[2] - '0');
		else
			rver[0] = atoi(reqver);
		rver[1] = -1;
		vers = rver;
		fallback = 0;
	}

	res = find_script_tcl_(name, logdepth, fatal, vers, fallback);
	if (res == 0)
		return 0;
	return try_fail(logdepth, "libs/script/tcl");
}


static int try_icl_tk(int logdepth, const char *prefix, const char *test_c_in, const char *includes, const char *cflags, const char *ldflags, int ver)
{
	const char *XL = get("libs/gui/xopendisplay/Lflags");
	const char *Xc = get("libs/gui/xopendisplay/cflags");
	const char *tl, *tc;
	char *tmpl, *tmpc;
	int res;

	tl = get("libs/script/tcl/ldflags");
	tc = get("libs/script/tcl/cflags");

	if (XL == NULL) XL = "";
	if (Xc == NULL) Xc = "";
	if (tl == NULL) tl = "";
	if (tc == NULL) tc = "";

	if ((*XL == '\0') && (*Xc == '\0') && (*tl == '\0') && (*tc == '\0'))
		return try_icl(logdepth, prefix, test_c_in, includes, cflags, ldflags);

	tmpl = str_concat(" ", tl, XL, ldflags, NULL);
	tmpc = str_concat(" ", tc, Xc, cflags, NULL);

	res = try_icl(logdepth, prefix, test_c_in, includes, tmpc, tmpl);

	free(tmpl);
	free(tmpc);

	return res;
}


int find_script_tk(const char *name, int logdepth, int fatal)
{
	int *v, rver[2], *vers = all_vers;
	const char *reqver, *tclreqver;

	char *test_c =
		NL "#include <stdio.h>"
		NL "#include <tk.h>"
		NL "int main() {"
		NL "	if (Tk_GetNumMainWindows() == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);
	require("libs/gui/xopendisplay/cflags", logdepth, fatal);
	require("libs/gui/xopendisplay/Lflags", logdepth, fatal);

	report("Checking for tk... ");
	logprintf(logdepth, "find_tk: trying to find tk...\n");
	logdepth++;

	reqver = get("/arg/tk-version");
	if ((reqver != NULL) && (reqver[0] != '\0')) {
		tclreqver = get("/arg/tcl-version");
		if ((tclreqver != NULL) && (tclreqver[0] != '\0')) {
			if (strcmp(tclreqver, reqver) != 0) {
				report("\n\nFatal argument error: if both tcl and tk version are forced, they must match\n\n");
				exit(1);
			}
		}
		if (reqver[1] == '.')
			rver[0] = (reqver[0] - '0') * 10 + (reqver[2] - '0');
		else
			rver[0] = atoi(reqver);
		rver[1] = -1;
		vers = rver;
	}

	/* Look at some standard places, per version */
	for(v = vers; *v != -1; v++) {
		int major = *v / 10, minor = *v % 10;
		char lfl[64];
		int tvers[2];

		tvers[0] = *v;
		tvers[1] = -1;
		if (find_script_tcl_("libs/script/tcl", logdepth+1, 0, tvers, 0) != 0) {
			report("(no tcl) ");
			continue;
		}

		if (*v == 0) {
			if (try_icl_tk(logdepth, "libs/script/tk", test_c, NULL, NULL, "-ltk", 0)) return 0;
			continue;
		}

		sprintf(lfl, "-ltk%d", *v);
		if (try_icl_tk(logdepth, "libs/script/tk", test_c, NULL, NULL, lfl, *v)) return 0;

		sprintf(lfl, "-ltk%d.%d", major, minor);
		if (try_icl_tk(logdepth, "libs/script/tk", test_c, NULL, NULL, lfl, *v)) return 0;
	}

	return try_fail(logdepth, "libs/script/tk");
}
