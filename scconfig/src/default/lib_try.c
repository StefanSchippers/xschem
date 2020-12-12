#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"

/* Returns true if the first 2 characters of the output is OK */
static int try_icl_accept_ok(char *stdout_str)
{
	return (strncmp(stdout_str, "OK", 2) == 0);
}

#define is_ctrl_prefix(ch) (((ch) == '!') || ((ch) == '^'))

static int try_icl__(int logdepth, const char *prefix, const char *test_c_in, const char *includes, const char *cflags, const char *ldflags, const char *db_includes, const char *db_cflags, const char *db_ldflags, int run, int (*accept_res)(char *stdout_str))
{
	char *out = NULL;
	char *tmp, *inc;
	const char *test_c;
	char c[1024];
	int l, compres;

	if (includes != NULL) {
		l = strlen(includes);
		memcpy(c, includes, l);
		c[l] = '\n';
		l++;
		strcpy(c+l, test_c_in);
		test_c = c;
	}
	else
		test_c = test_c_in;

	logprintf(logdepth, "trying '%s' and '%s' and '%s', %s\n", str_null(db_includes), str_null(db_cflags), str_null(db_ldflags), run ? "with a run" : "with no run");

	if (run)
		compres = compile_run(logdepth+1, test_c, NULL, cflags, ldflags, &out);
	else {
		char *fn_output = NULL;
		compres = compile_code(logdepth+1, test_c, &fn_output, NULL, cflags, ldflags);
		if (fn_output != NULL) {
			unlink(fn_output);
			free(fn_output);
		}
	}


	if (compres == 0) {
		if (!run || target_emu_fail(out) || accept_res(out)) {
			free(out);

			/* no prefix: don't modify the database, the caller will do that */
			if (prefix == NULL)
				return 1;

			tmp = malloc(strlen(prefix) + 32);

			if ((db_includes == NULL) || (*db_includes == '\0'))
				inc = strclone("");
			else
				inc = uniq_inc_str(db_includes, NULL, "\\n", 0, 0, NULL);
			sprintf(tmp, "%s/includes", prefix);
			put(tmp, inc);

			if (db_cflags == NULL)
				db_cflags = "";

			sprintf(tmp, "%s/cflags", prefix);
			put(tmp, db_cflags);


			if (db_ldflags == NULL)
				db_ldflags = "";
			sprintf(tmp, "%s/ldflags", prefix);
			put(tmp, db_ldflags);

			if (inc != NULL) {
				report("OK ('%s', '%s' and '%s')\n", str_null(inc), str_null(db_cflags), str_null(db_ldflags));
				free(inc);
			}
			else
				report("OK ('%s' and '%s')\n", str_null(db_cflags), str_null(db_ldflags));

			sprintf(tmp, "%s/presents", prefix);
			put(tmp, strue);
			free(tmp);
			return 1;
		}
		free(out);
	}
	return 0;
}

#define LOAD(node) \
do { \
	if (u ## node != NULL) break; \
	strcpy(apath_end, #node); \
	u ## node = get(apath); \
} while(0)

#define SET(dst, src, is_flag) \
do { \
	char *__sep__ = is_flag ? " " : "\n"; \
	const char *__dst__ = dst == NULL ? "" : dst; \
	char *__out__; \
	if (is_ctrl_prefix(*__dst__)) __dst__++; \
	if (*src == '!') \
		__out__ = strclone(src+1); \
	else if (*src == '^') {\
		if (src[1] != '\0') \
			__out__ = str_concat("", src+1, __sep__, __dst__, NULL); \
		else \
			__out__ = strclone(__dst__); \
	} \
	else { \
		if (*__dst__ != '\0') \
			__out__ = str_concat("", __dst__, __sep__, src, NULL); \
		else \
			__out__ = strclone(src); \
	} \
	free(dst); \
	dst = __out__; \
	if (is_flag) { \
		char *__s__; \
		for(__s__ = dst; *__s__ != '\0'; __s__++) \
			if ((*__s__ == '\n') || (*__s__ == '\r')) \
				*__s__ = ' '; \
	} \
} while(0)


/* Figure user overrides and call try_icl__() accordingly */
int try_icl_(int logdepth, const char *prefix, const char *test_c_in, const char *includes, const char *cflags, const char *ldflags, int run, int (*accept_res)(char *stdout_str))
{
	char apath[1024], *apath_end;
	int l, res;
	const char *uincludes = NULL, *ucflags = NULL, *uldflags = NULL, *uprefix = NULL; /* user specified */
	char *rincludes, *rcflags, *rldflags; /* real */
	char *dbincludes = NULL, *dbcflags = NULL, *dbldflags = NULL; /* what to add in the db at the end */

	if ((prefix == NULL) ? 0 : strlen(prefix) + strlen(db_cwd) > sizeof(apath)-32) {
		report("ERROR: no room for try_icl_() - prefix is probably too long.\n");
		return -1;
	}

	/* load uincludes, uclfags, uldflags and uprefix - LOAD() inserts the u */
	l = sprintf(apath, "/arg/icl/%s/", prefix); apath_end = apath+l;
	LOAD(includes);
	LOAD(cflags);
	LOAD(ldflags);
	LOAD(prefix);
	l = sprintf(apath, "/arg/icl/%s/%s/", db_cwd, prefix); apath_end = apath+l;
	LOAD(includes);
	LOAD(cflags);
	LOAD(ldflags);
	LOAD(prefix);

	/* special case: all three specified by the user - ignore what the detector wanted, but run only once per node prefix */
	if ((uincludes != NULL) && (ucflags != NULL) && (uldflags != NULL)) {
		const char *am;
		sprintf(apath, "%s/icl/all_manual_result", prefix);
		am = get(apath);
		if (am != NULL)
			return istrue(am); /* return cached result if available */
		res = try_icl__(logdepth, prefix, test_c_in, uincludes, ucflags, uldflags, uincludes, ucflags, uldflags, run, accept_res);
		put(apath, res ? strue : sfalse);
		return res;
	}

	/* TODO: get default cflags here */
	rincludes = NULL;
	rcflags   = strclone(get("cc/cflags"));
	rldflags  = strclone(get("cc/ldflags"));

	/* override base/default values with detection requested ones */
	if (includes != NULL) SET(rincludes, includes, 0);
	if (cflags != NULL)   SET(rcflags,   cflags, 1);
	if (ldflags != NULL)  SET(rldflags,  ldflags, 1);

	if (includes != NULL) SET(dbincludes, includes, 0);
	if (cflags != NULL)   SET(dbcflags,   cflags, 1);
	if (ldflags != NULL)  SET(dbldflags,  ldflags, 1);

	/* override detection with user specified ICL values */
	if (uincludes != NULL) SET(rincludes, uincludes, 0);
	if (ucflags != NULL)   SET(rcflags,   ucflags, 1);
	if (uldflags != NULL)  SET(rldflags,  uldflags, 1);

	if (uincludes != NULL) SET(dbincludes, uincludes, 0);
	if (ucflags != NULL)   SET(dbcflags,   ucflags, 1);
	if (uldflags != NULL)  SET(dbldflags,  uldflags, 1);

	/* insert prefix as needed */
	if (uprefix != NULL) {
		char *old, *prfx;

		old = rcflags;
		if ((rcflags != NULL) && (*rcflags == '^')) {
			rcflags++;
			prfx = "^";
		}
		else
			prfx = "";
		rcflags = str_concat("", prfx, "-I", uprefix, "/include ", rcflags, NULL);
		if (old != cflags) free(old);

		/* add -I to the db too */
		old = dbcflags;
		dbcflags = str_concat("", "-I", uprefix, "/include ", dbcflags, NULL);
		free(old);


		old = rldflags;
		if ((rldflags != NULL) && (*rldflags == '^')) {
			rldflags++;
			prfx = "^";
		}
		else
			prfx = "";
		rldflags = str_concat("", prfx, "-L", uprefix, "/lib ", rldflags, NULL);
		if (old != ldflags) free(old);

		/* add -L to the db too */
		old = dbldflags;
		dbldflags = str_concat("", "-L", uprefix, "/lib ", dbldflags, NULL);
		free(old);
	}

	res = try_icl__(logdepth, prefix, test_c_in, rincludes, rcflags, rldflags, dbincludes, dbcflags, dbldflags, run, accept_res);

	/* if we had to alloc, free here */
	free(rincludes);
	free(rcflags);
	free(rldflags);
	free(dbincludes);
	free(dbcflags);
	free(dbldflags);

	return res;
}

#undef LOAD
#undef SET

int try_icl(int logdepth, const char *prefix, const char *test_c_in, const char *includes, const char *cflags, const char *ldflags)
{
	return try_icl_(logdepth, prefix, test_c_in, includes, cflags, ldflags, 1, try_icl_accept_ok);
}

int try_icl_with_deps(int logdepth, const char *prefix, const char *test_c_in, const char *includes, const char *cflags, const char *ldflags, const char *dep_includes, const char *dep_cflags, const char *dep_ldflags, int run)
{
	int res;

	if ((dep_includes != NULL) && (*dep_includes == '\0')) dep_includes = NULL;
	if ((dep_cflags != NULL) &&   (*dep_cflags == '\0'))   dep_cflags = NULL;
	if ((dep_ldflags != NULL) &&  (*dep_ldflags == '\0'))  dep_ldflags = NULL;

	if (dep_includes != NULL) includes = str_concat(" ", dep_includes, includes, NULL);
	if (dep_cflags != NULL)   cflags   = str_concat(" ", dep_cflags, cflags, NULL);
	if (dep_ldflags != NULL)  ldflags  = str_concat(" ", dep_ldflags, ldflags, NULL);

	res = try_icl_(logdepth, prefix, test_c_in, includes, cflags, ldflags, run, try_icl_accept_ok);

	if (dep_includes != NULL) free((char *)includes);
	if (dep_cflags != NULL)   free((char *)cflags);
	if (dep_ldflags != NULL)  free((char *)ldflags);

	return res;
}

int try_icl_norun(int logdepth, const char *prefix, const char *test_c_in, const char *includes, const char *cflags, const char *ldflags)
{
	return try_icl_(logdepth, prefix, test_c_in, includes, cflags, ldflags, 0, try_icl_accept_ok);
}


int try_fail(int logdepth, const char *prefix)
{
	char *tmp;
	tmp = malloc(strlen(prefix) + 32);
	sprintf(tmp, "%s/presents", prefix);
	put(tmp, sfalse);
	free(tmp);
	report("not found\n");
	logprintf(logdepth, "NOT FOUND.");
	return 1;
}

static int try_pkg_config_(int logdepth, char *pkgname, const char *prefix, const char *test_c)
{
	char *cflags, *ldflags;
	int res;

	logprintf(logdepth, "Trying pkg-config %s\n", pkgname);
	if (run_pkg_config(logdepth+1, pkgname, &cflags, &ldflags) == 0)
		res = try_icl(logdepth+1, prefix, test_c, NULL, cflags, ldflags);
	else
		res = 0;
	free(cflags);
	free(ldflags);

	return res;
}

int try_icl_pkg_config(int logdepth, const char *prefix, const char *test_c, char *includes, const char *pkgpat, const char *reqver)
{
	char **pkg_ver, **s;
	int num_pkg_ver;
	int res = 0;
	(void) includes;  /* not used */

	run_pkg_config_lst(logdepth, pkgpat, &num_pkg_ver, &pkg_ver);
	if (pkg_ver == NULL)
		return 0;

	if (reqver != NULL) {
		/* search the list for the preferred version */
		for(s = pkg_ver; *s != NULL; s+=2) {
			if (strcmp(s[1], reqver) == 0) {
				if (try_pkg_config_(logdepth, s[0], prefix, test_c)) {
					res = 1;
					report("Found version required (%s) using pkg_config.\n", reqver);
					goto out;
				}
				else {
					report("The version required (%s) is found (via pkg_config) but does not work\n", reqver);
					goto out;
				}
			}
		}
		goto out;
	}

	for(s = pkg_ver; *s != NULL; s+=2) {
		if (try_pkg_config_(logdepth, s[0], prefix, test_c)) {
			res = 1;
			goto out;
		}
	}

out:;
	filelist_free(&num_pkg_ver, &pkg_ver);
	return res;
}


int import_icl(const char *key, const char *fn)
{
	char path[1024];

	if (strlen(key) > sizeof(path)-32) {
		report("ERROR: no room for import_icl() - key is probably too long.\n");
		return -1;
	}

	switch(*key) {
		case 'l': sprintf(path, "/arg/icl/%s/ldflags", key+8); break;
		case 'c': sprintf(path, "/arg/icl/%s/cflags", key+7); break;
		case 'i': sprintf(path, "/arg/icl/%s/includes", key+9); break;
		case 'p': sprintf(path, "/arg/icl/%s/prefix", key+7); break;
		default:
			return 1;
	}
	return put(path, fn) == NULL;
}


static long field_accept_len;
static int field_accept_res(char *stdout_str)
{
	char *end;
	field_accept_len = strtol(stdout_str, &end, 10);
	if (((*end == '\0') || (*end == '\r') || (*end == '\n')) && (field_accept_len > 0))
		return 1;
	return 0;
}

int try_icl_sfield(int logdepth, const char *prefix, const char *structn, const char *fieldn, const char *includes, const char *cflags, const char *ldflags)
{
	int res;
	char test_c[512];
	char ls[16];
	const char *test_c_in =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	%s s;"
		NL "	printf(\"%%ld\\n\", (long)sizeof(s.%s));"
		NL "}"
		NL;

	if (strlen(fieldn) + strlen(structn) + strlen(test_c_in) + 32 >= sizeof(test_c)) {
		report("ERROR: no room for try_icl_sfield() - struct or field name is probably too long.\n");
		return -1;
	}

	sprintf(test_c, test_c_in, structn, fieldn);

	res = try_icl_(logdepth, prefix, test_c, includes, cflags, ldflags, 1, field_accept_res);
	if (res) {
		sprintf(test_c, "%s/sizeof", prefix);
		sprintf(ls, "%ld", field_accept_len);
		put(test_c, ls);
	}
	return res;
}

int try_icl_sfields(int logdepth, const char *prefix, const char *structn, const char **fields, const char *includes, const char *cflags, const char *ldflags, int silent_exit_first_fail)
{
	int succ = 0, first = 1;
	require("cc/cc", logdepth, 1);

	for(; *fields != NULL; fields++) {
		report("Checking for %s.%s... ", structn, *fields);
		logprintf(logdepth, "%s: checking for field %s...\n", structn, *fields);

		logdepth++;
		if (try_icl_sfield(logdepth, prefix, structn, *fields, includes, cflags, ldflags)) {
			succ = 1;
		}
		else if ((silent_exit_first_fail) && (first)) {
			return 1;
		}
		logdepth--;
		first = 0;
	}


	if (!succ)
		try_fail(logdepth, "libs/fsmount/next_dev");

	return 0;
}
