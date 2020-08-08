#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "log.h"
#include "libs.h"
#include "db.h"
#include "dep.h"
#include "regex.h"

static void zap(char **str)
{
	const char *pat = get("/arg/sys/pkg-config-zap");
	char *n;

	if (pat == NULL)
		return;
	if (re_comp(pat) != NULL)
		return;
	while (re_exec(*str)) {
		n = re_subs_dup("");
		free(*str);
		*str = n;
	}
}

int run_gen_config(int logdepth, const char *confname, const char *pkgname, char **cflags, char **ldflags)
{
	char cmd[256];

	if (strlen(confname) + strlen(pkgname) > sizeof(cmd) - 16) {
		logprintf(logdepth, "run_gen_config(): confname and/or pkgname too long\n");
		return -1;
	}

	if (cflags != NULL) {
		sprintf(cmd, "%s --cflags %s", confname, pkgname);
		if (run(logdepth, cmd, cflags) != 0) {
			report("not found: %s --cflags failed.", confname);
			logprintf(logdepth, "not found: %s --cflags failed.\n", confname);
			return -1;
		}
		if (*cflags != NULL) {
			zap(cflags);
			strip(*cflags);
		}
	}

	if (ldflags != NULL) {
		sprintf(cmd, "%s --libs %s", confname, pkgname);
		if (run(logdepth, cmd, ldflags) != 0) {
			report("not found: %s --libs failed.", confname);
			logprintf(logdepth, "not found: %s --libs failed.\n", confname);
			if (cflags != NULL)
				free(*cflags);
			return -1;
		}
		if (*ldflags != NULL) {
			zap(ldflags);
			strip(*ldflags);
		}
	}

	return 0;
}

const char *pkg_config_name()
{
	const char *name;
	name = get("/arg/sys/pkg-config");
	if (name != NULL)
		return name;
	return "pkg-config";					/* fallback */
}

/** run_pkg_config_modversion:
    run `pkg-config` on @pkgname:
    - with `--modversion` if @modversion is not NULL, storing the result in @modversion (malloc()'d)
    Returns 0 on success.
*/
int run_pkg_config_modversion(int logdepth, const char *pkgname, char **modversion)
{
	char cmd[256];
	const char *confname = pkg_config_name();

	if (strlen(confname) + strlen(pkgname) > sizeof(cmd) - 16) {
		logprintf(logdepth, "run_pkg_config_modversion(): confname and/or pkgname too long\n");
		return -1;
	}

	if (modversion != NULL) {
		sprintf(cmd, "%s --modversion %s", confname, pkgname);
		if (run(logdepth, cmd, modversion) != 0) {
			/*report("Module version not found: %s --modversion %s failed.", confname, pkgname);
			logprintf(logdepth, "Module version not found: %s --modversion %s failed.\n", confname, pkgname); */
			return -1;
		}
		zap(modversion);
		strip(*modversion);
	}

	return 0;
}

/** run_pkg_config_modversion_db:
    run `pkg-config --modversion` on @pkgname to find module (or package) version
    and store the result in @node/modversion
    Returns 0 on success.
*/
int run_pkg_config_modversion_db(int logdepth, const char *node, const char *pkgname /*, char **modversion */ )
{
	char *modversion;
	char *tmp;

	if (run_pkg_config_modversion(logdepth, pkgname, &modversion) != 0) {
		return -1;
	}
	/* Store the module version in node */
	tmp = str_concat("/", node, "modversion", NULL);
	put(tmp, modversion);
	free(tmp);
	free(modversion);

	return 0;
}

int run_pkg_config(int logdepth, const char *pkgname, char **cflags, char **ldflags)
{

	return run_gen_config(logdepth, pkg_config_name(), pkgname, cflags, ldflags);
}

void run_pkg_config_lst(int logdepth, const char *pkgpat, int *argc, char ***argv)
{
	char *end, *s, *next;
	int n = 0, a = 0;
	char **sf = NULL;
	static const char *pkg_cfg_cache = NULL;
	static char no_pkg_cfg;
	char *list;

	if (pkg_cfg_cache == &no_pkg_cfg)
		goto error;

	if (pkg_cfg_cache == NULL) {
		char *cmd = str_concat(" ", pkg_config_name(), "--list-all", NULL);
		run(logdepth, cmd, (char **) &pkg_cfg_cache);
		free(cmd);
		if (pkg_cfg_cache == NULL) {
			pkg_cfg_cache = &no_pkg_cfg;
			goto error;
		}
	}

	if (re_comp(pkgpat) != NULL)
		goto error;

	s = list = strclone(pkg_cfg_cache);
	for (;;) {
		while (isspace(*s))
			s++;
		if (*s == '\0')
			break;
		next = strpbrk(s, "\r\n");
		if (next != NULL)
			*next = '\0';
		if (re_exec(s)) {
			if ((n + 2) >= a) {				/* n+2: make sure there's always room for the NULL at the end */
				a += 16;
				sf = realloc(sf, sizeof(char *) * a);
			}
			end = strpbrk(s, " \t");
			if (end != NULL)
				*end = '\0';

			sf[n] = strclone(s);
			sf[n + 1] = re_subs_dup("");
/*      report("\ns='%s' sf='%s'\n", s, sf[n]);*/
			n += 2;
		}
		s = next + 1;
	}

	if (sf != NULL)
		sf[n] = NULL;

	free(list);
error:;
	*argc = n;
	*argv = sf;
	return;
}
