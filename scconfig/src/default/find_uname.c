/*
    scconfig - evaluate uname and classify the system
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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "regex.h"
#include "log.h"
#include "db.h"
#include "libs.h"
#include "dep.h"

static void sys_unix(void)
{
	put("sys/ext_exe",    "");
	put("sys/ext_dynlib", ".so");
	put("sys/ext_stalib", ".a");
	put("sys/ext_dynlib_native", ".so");
}

static void sys_netbsd(void)
{
	sys_unix();
	put("cc/ldflags", "-Wl,-R/usr/pkg/lib -L/usr/pkg/lib"); /* TODO: is this the best way? */
}

static void sys_win32dlc(void)
{
	put("sys/ext_exe",    ".exe");
	put("sys/ext_dynlib", ".dlc");
	put("sys/ext_stalib", ".a");
	put("sys/ext_dynlib_native", ".dll");
}

typedef void (*callback_t)(void);

typedef struct {
	char *uname_regex;
	char *name;
	char *class;

	callback_t callback;
} uname_t;

typedef struct {
	char *file_name;
	char *name;
	char *class;

	callback_t callback;
} magic_file_t;

/* Guess system class by uname; class is informative, nothing important
   should depend on it.
   Order *does* matter */
uname_t unames[] = {
	{"[Nn]et[Bb][Ss][Dd]",   "NetBSD", "UNIX",  sys_netbsd},
	{"[Ll]inux",             "Linux",  "UNIX",  sys_unix},
	{"[Bb][Ss][Dd]",         "BSD",    "UNIX",  sys_unix},
	{"SunOS",                "SunOS",  "UNIX",  sys_unix},
	{"OSF1",                 "OSF",    "UNIX",  sys_unix}, /* TODO: note the difference in cflags for debugging ("-ms -g") */
	{"AIX",                  "AIX",    "UNIX",  sys_unix},
	{"IRIX",                 "IRIX",   "UNIX",  sys_unix},
	{"SunOS",                "SunOS",  "UNIX",  sys_unix},
	{"[Mm]inix",             "Minix",  "UNIX",  sys_unix},
	{"[Aa][Rr][Oo][Ss]",     "Aros",   "UNIX",  sys_unix},
	{"^Darwin",              "MacOSX", "UNIX",  sys_unix},
	{"[Th]hreos",            "Threos", "UNIX",  sys_unix},
	{"[Cc]ygwin",            "cygwin", "WIN32", sys_win32dlc},
	{"[Mm][Ii][Nn][Gg][Ww]", "mingw",  "WIN32", sys_win32dlc},
	{"win32",                "win32",  "WIN32", sys_win32dlc}, /* vanilla windows */
	{NULL, NULL, NULL, NULL}
};

/* Fallback: extract machine name from uname -a if uname -m fails */
static const char *machine_names[] = {
	"i[0-9]86[^ ]*", "x86_[^ ]*", "amd[0-9]*", "armv[0-9][^ ]*", "ppc[0-9]+",
	"sparc[0-9]*", "BePC", "ia64", "x86", "IP[0-9]*", "k1om", "sun4u",
	"RM600", "R4000", "alpha",
	NULL
};

/* Fallback: extract system name from uname -a if uname -s fails */
static const char *system_names[] = {
	"[Ll]inux", "sn5[0-9]*", "CYGWIN_NT[^ ]*", "GNU[^ ]*", "DragonFly",
	"[^ ]*BSD[^ ]*", "Haiku", "HP-UX", "AIX", "OS4000", "Interix",
	"IRIX[0-9]*", "Darwin", "Minix", "MINGW[^ ]*", "ReliantUNIX[^ ]*",
	"SunOS", "OSF1", "ULTRIX", "UWIN-W7", "IS/WB", "OS/390",
	"SCO[^ ]*", "QNX",
	NULL
};

/* Fallback: if uname -a fails, guess system by looking at "magic file names" */
magic_file_t magic_files[] = {
	{"/dev/null",                "UNIX",   "UNIX",  sys_unix},
	{"c:\\config.sys",           "win32",  "WIN32", sys_win32dlc},
	{"c:\\windows\\system.ini",  "win32",  "WIN32", sys_win32dlc},
	{"c:\\windows\\win.ini",     "win32",  "WIN32", sys_win32dlc},
	{"c:\\windows\\notepad.exe", "win32",  "WIN32", sys_win32dlc},
	{NULL, NULL, NULL, NULL}
} ;

static int match(const char *regex, const char *str)
{
	re_comp(regex);
	return re_exec(str);
}

/* match uname against each pattern on the list; on a match, put() the portion
   of the string matched in node and return 1 */
int uname_guess(const char *node, const char *uname, const char *list[])
{
	const char **l;
	if (uname == NULL)
		return 0;
	for(l = list; *l != NULL; l++) {
		if (match(*l, uname)) {
			char *s;
			int len = eopat[0] - bopat[0];
			s = malloc(len+1);
			memcpy(s, bopat[0], len);
			s[len] = '\0';
			put(node, s);
			return 1;
		}
	}
	return 0;
}

/* Don't worry about linear search or matching regexes all the time - this
   function will be run at most two times */
static callback_t lookup_uname(char **uname, const char **name, const char **class)
{
	uname_t *u;
	for(u = unames; u->uname_regex != NULL; u++) {
		if (
		      ((*uname != NULL) && (match(u->uname_regex, *uname)))        /* uname match */
		   || ((*name  != NULL)  && ((strcmp(u->name, *name) == 0)))   /* name match */
		   || ((*class != NULL)  && ((strcmp(u->class, *class) == 0))) /* class match */
		   ) {
			if (*name  == NULL) *name  = u->name;
			if (*class == NULL) *class = u->class;
			return u->callback;
		   }
	}
	return NULL;
}

static callback_t lookup_magic_file(int logdepth, const char **name, const char **class)
{
	magic_file_t *u;
	for(u = magic_files; u->file_name != NULL; u++) {
		if (is_file(u->file_name)) {
			logprintf(logdepth, "%s -> %s\n", u->file_name, u->class);

			if (*name  == NULL) *name  = u->name;
			if (*class == NULL) *class = u->class;
			return u->callback;
		}
	}
	return NULL;
}

int find_uname(const char *rname, int logdepth, int fatal)
{
	const char *name, *class, *tname, *uname_orig;
	char *s, *uname, *mname, *sname;
	void (*callback)(void);

	require("sys/tmp", logdepth, fatal);

	if (istarget(db_cwd))
		require("/target/sys/target", logdepth, fatal);

	report("Checking for system type... ");
	logprintf(logdepth, "[find_uname] checking for sys/name\n");
	logdepth++;

	tname = get("/arg/sys/target-name");
	if (istarget(db_cwd) && (tname != NULL))
		put("sys/name", tname);

	tname = get("/arg/sys/target-uname");
	if (istarget(db_cwd) && (tname != NULL))
		put("sys/uname", tname);

	name       = get("sys/name");
	uname_orig = get("sys/uname");

	if (name == NULL) {
		if (uname_orig == NULL) {
			logprintf(logdepth, "not set, running\n");
			run_shell(logdepth, "uname -a", (char **)&uname);
			if (uname != NULL) {
				for(s = uname; *s != '\0'; s++)
					if ((*s == '\n') || (*s == '\r')) *s = ' ';
				put("sys/uname", uname);
			}
			else
				put("sys/uname", "");

			if (run_shell(logdepth, "uname -m", (char **)&mname) == 0)
				put("sys/machine_name", strip(mname));
			else
				put("sys/machine_name", NULL);

			if (mname != NULL)
				free(mname);

			if (run_shell(logdepth, "uname -o", (char **)&sname) == 0)
				put("sys/system_name", strip(sname));
			else if (run_shell(logdepth, "uname -s", (char **)&sname) == 0)
				put("sys/system_name", strip(sname));
			else
				put("sys/system_name", NULL);
			if (sname != NULL)
				free(sname);
		}

		/* we have uname by now, set sys/name */
		name  = NULL;
		class = NULL;
		callback = lookup_uname(&uname, &name, &class);
		if (name == NULL) {
			/* no uname or unknown system by uname - fallback: check for cross target */
			const char *target = get("/arg/sys/target");
			if ((target != NULL) && (strstr(target, "mingw") != NULL)) {
				name = "WIN32";
				report("(detected mingw cross compilation to WIN32)\n");
			}
			else {
				report("Warning: unknown system\n");
				name = "unknown";
			}
		}
		put("sys/name", name);
	}
	else {
		/* we had sys/name, that should be enough */
		uname = NULL;
		class = name;
		callback = lookup_uname(&uname, &name, &class);
	}

	/* predefined and/or detected uname failed, try magic file method */
	if (callback == NULL) {
		logprintf(logdepth, "System class is unknown by uname, running heuristics...\n");
		report("System class is unknown by uname, running heuristics... ");

		callback = lookup_magic_file(logdepth + 1, &name, &class);
	}


	if (callback == NULL) {
		/* System unknown. */
		error("Unknown system '%s'\n", get("sys/uname"));
		abort();
	}

	callback();
	report("OK (name: %s; class: %s)\n", name, class);
	put("sys/class", class);

	/* fallbacks */
	if (get("sys/machine_name") == NULL)
		uname_guess("sys/machine_name", uname, machine_names);

	if (get("sys/system_name") == NULL)
		uname_guess("sys/system_name", uname, system_names);

	/* on windows, overwrite the path sep with the right amount of \ (the tmp finder may have left / in it) */
	if ((strcmp(class, "WIN32") == 0) || (strcmp(class, "win32") == 0)) {
		int eats = istrue(get("sys/shell_eats_backslash"));

		if (eats)
			put("sys/path_sep", "\\\\\\\\");
		else
			put("sys/path_sep", "\\");

		put("sys/path_sep_escaped", "\\\\");
	}

	return 0;
}

static int find_triplet_(const char *name, int logdepth, int fatal, const char *nodename, int include_vendor, char *sep, char *esc)
{
	const char *machine, *vendor, *os;
	char *triplet, *s;
	char fake_sep[2];

	fake_sep[0] = 1;
	fake_sep[1] = 0;

	require("sys/uname", logdepth, fatal);

	machine = get("sys/machine_name");
	if (machine == NULL)
		machine = "unknown";

	vendor = "unknown";

	os = get("sys/system_name");
	if (os == NULL)
		os = "unknown";

	if (include_vendor)
		triplet = str_concat(fake_sep, machine, vendor, os, NULL);
	else
		triplet = str_concat(fake_sep, machine, os, NULL);

	for(s = triplet; *s != '\0'; s++) {
		if ((esc != NULL) && (*s == *sep))
			*s = *esc;
		if (isalnum(*s))
			*s = tolower(*s);
		else {
			if (*s == *fake_sep)
				*s = *sep;
			else if (esc != NULL)
				*s = *esc;
			else
				*s = '-';
		}
	}
	put(nodename, triplet);
	free(triplet);
	return 0;
}

int find_triplet(const char *name, int logdepth, int fatal)
{
	return find_triplet_(name, logdepth, fatal, "sys/triplet", 1, "-", NULL);
}

int find_sysid(const char *name, int logdepth, int fatal)
{
	return find_triplet_(name, logdepth, fatal, "sys/sysid", 0, "-", "_");
}
