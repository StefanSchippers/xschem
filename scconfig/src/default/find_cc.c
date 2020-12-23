/*
    scconfig - detection of cc and compiler features
    Copyright (C) 2009..2012  Tibor Palinkas

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
#include <unistd.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"



static int try_flags(int logdepth, const char *cc, const char *test_c, const char *cflags, const char *ldflags, const char *expected)
{
	char *out;

	logprintf(logdepth, "trying cc:try_flags with cc='%s' cflags='%s' ldflags='%s'\n", (cc == NULL ? get("cc/cc") : cc), cflags == NULL ? "" : cflags, ldflags == NULL ? "" : ldflags);

	if (compile_run(logdepth+1, test_c, cc, cflags, ldflags, &out) == 0) {
		if (((out == NULL) && (iscross)) || (strncmp(out, expected, strlen(expected)) == 0)) {
			free(out);
			return 1;
		}
		free(out);
	}
	return 0;
}

static int try_flags_inv(int logdepth, const char *cc, const char *test_c, const char *cflags, const char *ldflags, const char *expected_bad)
{
	char *out;

	logprintf(logdepth, "trying cc:try_flags with cc='%s' cflags='%s' ldflags='%s'\n", (cc == NULL ? get("cc/cc") : cc), cflags == NULL ? "" : cflags, ldflags == NULL ? "" : ldflags);

	if (compile_run(logdepth+1, test_c, cc, cflags, ldflags, &out) == 0) {
		if (((out == NULL) && (iscross)) || (strncmp(out, expected_bad, strlen(expected_bad)) != 0)) {
			free(out);
			return 1;
		}
		free(out);
	}
	return 0;
}

static int try(int logdepth, const char *cc, const char *test_c, const char *expected)
{
	return try_flags(logdepth, cc, test_c, NULL, NULL, expected);
}


static int trycc(int logdepth, const char *cc, const char *test_c)
{
	int ret;

	if (cc == NULL)
		return 0;

	ret = try(logdepth, cc, test_c, "OK");
	if (ret)
		put("cc/cc", cc);
	return ret;
}

int find_cc(const char *name, int logdepth, int fatal)
{
	char *test_c = "#include <stdio.h>\nint main() { printf(\"OK\\n\");\nreturn 0;}\n";
	char *out = NULL, *targetcc;
	const char *cc, *cflags, *ldflags, *target, *sys;
	int len;

	require("sys/name", logdepth, fatal);

	sys = istarget(db_cwd) ? "target" : "host";
	report("Checking for cc (%s)... ", sys);
	logprintf(logdepth, "find_cc: trying to find cc (%s)...\n", sys);
	logdepth++;

	/* cflags */
	cflags = get("/arg/cc/cflags");
	if (cflags != NULL) {
		logprintf(logdepth+1, "using user supplied cflags '%s'\n", cflags);
		put("cc/cflags", cflags);
	}

	/* ldflags */
	ldflags = get("/arg/cc/ldflags");
	if (ldflags != NULL) {
		logprintf(logdepth+1, "using user supplied ldflags '%s'\n", ldflags);
		put("cc/ldflags", ldflags);
	}

	cc = get("/arg/cc/cc");
	if (cc == NULL) {
		target = get("sys/target");
		if (target != NULL) {
			logprintf(logdepth+1, "find_cc: crosscompiling for '%s', looking for target cc\n", target);
			len = strlen(target);
			targetcc = malloc(len + 8);
			memcpy(targetcc, target, len);
			strcpy(targetcc + len, "-gcc");
			if (!trycc(logdepth+1, targetcc, test_c)) {
				strcpy(targetcc + len, "-cc");
				if (!trycc(logdepth+1, targetcc, test_c)) {
					report("FAILED: failed to find crosscompiler for target '%s'\n", target);
					logprintf(logdepth, "find_cc: FAILED to find a crosscompiler for target '%s'\n", target);
					return 1;
				}
			}
			put("cc/cc", targetcc);
		}
		else {
			cc = getenv("CC");
			logprintf(logdepth, "find_cc: Detecting cc (host)\n");
			/* Find a working cc (no arguments) */
			if (!(((cc != NULL) && (trycc(logdepth+1, cc, test_c))) || trycc(logdepth+1, "gcc", test_c) || trycc(logdepth+1, "cc", test_c))) {
				report("FAILED to find a compiler\n");
				logprintf(logdepth, "find_cc: FAILED to find a compiler\n");
				return 1;
			}
		}
	}
	else {
		put("cc/cc", cc);
		logprintf(logdepth+1, "using user supplied '%s' (will test later)\n", cc);
	}

	/* cflags (again) */
	if (cflags == NULL) {
		logprintf(logdepth, "find_cc: Detecting -pipe\n");

		if (compile_run(logdepth+1, test_c, NULL, "-pipe", "", &out) == 0) {
			if (target_emu_fail(out) || (strncmp(out, "OK", 2) == 0)) {
				append("cc/cflags", " -pipe");
			}
			free(out);
		}
	}
	if (get("cc/cflags") == NULL)
		put("cc/cflags", "");

	/* ldflags (again) */
	if (get("cc/ldflags") == NULL)
		put("cc/ldflags", "");

	/* Final test of all arguments together */
	logprintf(logdepth, "find_cc: final test on cc and all flags \n");
	if (compile_run(logdepth+1, test_c, NULL, NULL, NULL, &out) != 0) {
		report("FAILED to get the compiler and all flags to work together\n");
		logprintf(logdepth, "find_cc: the compiler and all the flags don't work well together, aborting\n");
		if (out != NULL)
			free(out);
		return 1;
	}

	report("OK ('%s', '%s', '%s')\n", get("cc/cc"), get("cc/cflags"), get("cc/ldflags"));
	logprintf(logdepth, "find_cc: conclusion: cc='%s' cflags='%s' ldflags='%s'\n", get("cc/cc"), get("cc/cflags"), get("cc/ldflags"));
	if (out != NULL)
		free(out);
	return 0;
}

int find_cc_argstd(const char *det_name, int logdepth, int fatal)
{
	char *test_c = "#include <stdio.h>\nint main() { printf(\"OK\\n\");\nreturn 0;}\n";
	char *out = NULL;
	char **flg, *flags[] = {"-ansi", "-pedantic", "-Wall", "-std=c89", "-std=c99", "-Werror", "-Wextra", "-W", "-pg", "-no-pie", "-static-pie", NULL};
	const char *det_target = det_list_target(det_name);

	require("cc/cc", logdepth, fatal);

	logprintf(logdepth, "find_cc: Detecting CC args %s\n", det_target);
	report("Checking for cc args for std %s... ", det_target);

	for(flg = flags; *flg != NULL; flg++) {
		char name[128], *end;
		const char *found = "";
		sprintf(name, "cc/argstd/%s", (*flg)+1);
		end = strchr(name, '=');
		if (end != NULL)
			*end = '_';
		if (!asked_for(name, det_name))
			continue;
		if (compile_run(logdepth+1, test_c, NULL, *flg, "", &out) == 0) {
			if (target_emu_fail(out) || (strncmp(out, "OK", 2) == 0)) {
				found = *flg;
				report(" ");
				report(found);
			}
			free(out);
		}
		put(name, found);
	}

	if (is_dep_wild(det_name))
		put("cc/argstd/presents", strue); /* to avoid re-detection*/

	report("\n");
	return 0;
}

int find_cc_argmachine(const char *name, int logdepth, int fatal)
{
#define ARGM(flag) "-m" #flag , "-mno-" #flag
	const char *test_c = "#include <stdio.h>\nint main() { printf(\"OK\\n\");\nreturn 0;}\n";
	char *out = NULL;
	const char **flg, *flags[] = { ARGM(mmx), ARGM(sse), ARGM(sse2), ARGM(sse3), ARGM(ssse3), ARGM(sse4), ARGM(sse4.1), ARGM(sse4.2), ARGM(avx), ARGM(avx2), NULL};

	require("cc/cc", logdepth, fatal);

	logprintf(logdepth, "find_cc: Detecting CC machine args\n");
	report("Checking for cc args for machine... ");

	for(flg = flags; *flg != NULL; flg++) {
		char name[128], *end;
		const char *found = "";
		{
			const char* ptr = (*flg) + 1;
			strcpy(name, "cc/argmachine/");
			end = name + strlen(name);
			while(*ptr) {
				if('.'!=*ptr && '-'!=*ptr) *end++ = *ptr;
				++ptr;
			}
			*end = '\0';
		}
		end = strchr(name, '=');
		if (end != NULL)
			*end = '_';
		if (compile_run(logdepth+1, test_c, NULL, *flg, "", &out) == 0) {
			if (target_emu_fail(out) || (strncmp(out, "OK", 2) == 0)) {
				found = *flg;
				report(" ");
				report(found);
			}
			free(out);
		}
		put(name, found);
	}

	report("\n");
	return 0;
#undef ARGM
}

int find_inline(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "static inline void test_inl()"
		NL "{"
		NL "	puts(\"OK\");"
		NL "}"
		NL "int main() {"
		NL "	test_inl();"
		NL "	return 0;"
		NL "}"
		NL ;
	require("cc/cc", logdepth, fatal);

	report("Checking for inline... ");
	logprintf(logdepth, "find_inline: trying to find inline...\n");
	logdepth++;
	if (try(logdepth, NULL, test_c, "OK")) {
		put("cc/inline", strue);
		report("Found.\n");
		return 0;
	}
	put("cc/inline", sfalse);
	report("Not found.\n");
	return 1;
}

int find_varargmacro(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "#define pr(fmt, x...) {printf(\"PR \"); printf(fmt, x); }"
		NL "int main() {"
		NL "	pr(\"%d %d %s\", 42, 8192, \"test\");"
		NL "	puts(\"\");"
		NL "	return 0;"
		NL "}"
		NL ;
	require("cc/cc", logdepth, fatal);

	report("Checking for vararg macro... ");
	logprintf(logdepth, "find_varargmacro: trying to find vararg macro...\n");
	logdepth++;
	if (try(logdepth, NULL, test_c, "PR 42 8192 test")) {
		put("cc/varargmacro", strue);
		report("Found.\n");
		return 0;
	}
	put("cc/varargmacro", sfalse);
	report("Not found.\n");
	return 1;
}

int find_funcmacro(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	printf(\"%s\\n\", __func__);"
		NL "	return 0;"
		NL "}"
		NL ;
	require("cc/cc", logdepth, fatal);

	report("Checking for __func__ macro... ");
	logprintf(logdepth, "find_funcmacro: trying to find __func__ macro...\n");
	logdepth++;
	if (try(logdepth, NULL, test_c, "main")) {
		put("cc/funcmacro", strue);
		report("Found.\n");
		return 0;
	}
	put("cc/funcmacro", sfalse);
	report("Not found.\n");
	return 1;
}

int find_constructor(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "void startup() __attribute__ ((constructor));"
		NL "void startup()"
		NL "{"
		NL "	puts(\"OK\");"
		NL "}"
		NL "int main() {"
		NL "	return 0;"
		NL "}"
		NL ;

	require("cc/cc", logdepth, fatal);

	report("Checking for constructor... ");
	logprintf(logdepth, "find_constructor: trying to find constructor...\n");
	logdepth++;
	if (try(logdepth, NULL, test_c, "OK")) {
		put("cc/constructor", strue);
		report("Found.\n");
		return 0;
	}
	put("cc/constructor", sfalse);
	report("Not found.\n");
	return 1;
}

int find_destructor(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "void startup() __attribute__ ((destructor));"
		NL "void startup()"
		NL "{"
		NL "	puts(\"OK\");"
		NL "}"
		NL "int main() {"
		NL "	return 0;"
		NL "}"
		NL ;

	require("cc/cc", logdepth, fatal);

	report("Checking for destructor... ");
	logprintf(logdepth, "find_destructor: trying to find destructor...\n");
	logdepth++;
	if (try(logdepth, NULL, test_c, "OK")) {
		put("cc/destructor", strue);
		report("Found.\n");
		return 0;
	}
	put("cc/destructor", sfalse);
	report("Not found.\n");
	return 1;
}

static int test_fattr(const char *name, int logdepth, int fatal, const char *fattr)
{
	char path[64];
	char test_c[256];
	const char *test_c_tmp =
		NL "#include <stdio.h>"
		NL "static void test1() __attribute__ ((%s));"
		NL "static void test1()"
		NL "{"
		NL "	puts(\"OK\");"
		NL "}"
		NL "int main() {"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL ;

	require("cc/cc", logdepth, fatal);

	sprintf(test_c, test_c_tmp, fattr);
	sprintf(path, "cc/func_attr/%s/presents", fattr);

	report("Checking for function attribute %s... ", fattr);
	logprintf(logdepth, "test_fattr: trying to find %s...\n", fattr);
	logdepth++;
	if (try(logdepth, NULL, test_c, "OK")) {
		put(path, strue);
		report("Found.\n");
		return 0;
	}
	put(path, sfalse);
	report("Not found.\n");
	return 1;
}

int find_fattr_unused(const char *name, int logdepth, int fatal)
{
	return test_fattr(name, logdepth, fatal, "unused");
}

static int test_declspec(const char *name, int logdepth, int fatal, const char *dspec)
{
	char path[64];
	char test_c[256];
	const char *test_c_tmp =
		NL "#include <stdio.h>"
		NL "void  __declspec (%s) test1();"
		NL "void test1()"
		NL "{"
		NL "	puts(\"OK\");"
		NL "}"
		NL "int main() {"
		NL "	test1();"
		NL "	return 0;"
		NL "}"
		NL ;

	require("cc/cc", logdepth, fatal);

	sprintf(test_c, test_c_tmp, dspec);
	sprintf(path, "cc/declspec/%s/presents", dspec);

	report("Checking for declspec %s... ", dspec);
	logprintf(logdepth, "test_declspec: trying to find %s...\n", dspec);
	logdepth++;
	if (try(logdepth, NULL, test_c, "OK")) {
		put(path, strue);
		report("Found.\n");
		return 0;
	}
	put(path, sfalse);
	report("Not found.\n");
	return 1;
}

int find_declspec_dllimport(const char *name, int logdepth, int fatal)
{
	return test_declspec(name, logdepth, fatal, "dllimport");
}

int find_declspec_dllexport(const char *name, int logdepth, int fatal)
{
	return test_declspec(name, logdepth, fatal, "dllexport");
}

static int test_dll_auxfile(const char *name, int logdepth, int fatal, const char *path, const char *ldflag, const char *filename)
{
	char *ldflags;
	char test_c[256];
	const char *test_c_template =
		NL "#include <stdio.h>"
		NL "void %s test1();"
		NL "void test1()"
		NL "{"
		NL "	puts(\"OK\");"
		NL "}"
		NL "int main() {"
		NL "	test1();"
		NL "	return 0;"
		NL "}"
		NL ;
	const char *dspec;

	require("cc/cc", logdepth, fatal);
	require("cc/declspec/dllexport/*", logdepth, 0);

	if (istrue("cc/declspec/dllexport/presents"))
		dspec = " __declspec(dllexport) ";
	else
		dspec = "";

	sprintf(test_c, test_c_template, dspec);

	report("Checking for DLL flag %s... ", ldflag);
	logprintf(logdepth, "test_dll_auxfile: trying to find %s...\n", ldflag);
	logdepth++;
	ldflags = str_concat("", ldflag, ",", filename, " ", get("cc/ldflags"), NULL);
	if (try_flags(logdepth, NULL, test_c, NULL, ldflags, "OK")) {
		unlink(filename);
		put(path, ldflag);
		free(ldflags);
		report("Found.\n");
		return 0;
	}
	unlink(filename);
	free(ldflags);
	report("Not found.\n");
	return 1;
}

int find_cc_wloutimplib(const char *name, int logdepth, int fatal)
{
	return test_dll_auxfile(name, logdepth, fatal, "cc/wloutimplib", "-Wl,--out-implib", "libscconfig_0.a");
}

int find_cc_wloutputdef(const char *name, int logdepth, int fatal)
{
	return test_dll_auxfile(name, logdepth, fatal, "cc/wloutputdef", "-Wl,--output-def", "libscconfig_0.def");
}

/* Hello world program to test compiler flags */
static const char *test_hello_world =
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL ;

static int try_hello(int logdepth, const char *cflags, const char *ldflags, const char *name, const char *value)
{
	if (try_flags(logdepth, NULL, test_hello_world, cflags, ldflags, "OK")) {
		put(name, value);
		report("OK (%s)\n", value);
		return 1;
	}
	return 0;
}

int find_rdynamic(const char *name, int logdepth, int fatal)
{
	const char *node = "cc/rdynamic";

	require("cc/cc", logdepth, fatal);

	report("Checking for rdynamic... ");
	logprintf(logdepth, "find_rdynamic: trying to find rdynamic...\n");
	logdepth++;

	if (try_hello(logdepth, NULL, "-rdynamic", node, "-rdynamic")) return 0;
	if (try_hello(logdepth, NULL, "-Wl,-export-dynamic", node, "-Wl,-export-dynamic")) return 0;
	if (try_hello(logdepth, NULL, NULL, node, "")) return 0;

	report("Not found.\n");
	return 1;
}

int find_cc_fpie(const char *name, int logdepth, int fatal)
{
	const char *test_c = test_hello_world;

	require("cc/cc", logdepth, fatal);
	/* TODO: what about -fpic? */

	report("Checking for -fpie... ");
	logprintf(logdepth, "find_cc_fpie: trying to find -fpie...\n");
	logdepth++;

	/* NOTE: some gcc configuration might not pass the -pie flag to the linker, so */
	/* try to detect whether we can force it to the linker */
	if (try_icl(logdepth, "cc/fpie", test_c, NULL, "-fpie", "-pie -Wl,-pie")) return 0;
	if (try_icl(logdepth, "cc/fpie", test_c, NULL, "-fPIE", "-pie -Wl,-pie")) return 0;
	if (try_icl(logdepth, "cc/fpie", test_c, NULL, "-fpie", "-pie")) return 0;
	if (try_icl(logdepth, "cc/fpie", test_c, NULL, "-fPIE", "-pie")) return 0;
	if (try_icl(logdepth, "cc/fpie", test_c, NULL, NULL, NULL)) return 0;
	return try_fail(logdepth, "cc/fpie");
}

int find_cc_fnopie(const char *name, int logdepth, int fatal)
{
	const char *test_c = test_hello_world;

	require("cc/cc", logdepth, fatal);

	report("Checking for -fno-pie... ");
	logprintf(logdepth, "find_cc_fnopie: trying to find -fno-pie...\n");
	logdepth++;

	if (try_icl(logdepth, "cc/fnopie", test_c, NULL, "-fno-pie", NULL)) return 0;
	if (try_icl(logdepth, "cc/fnopie", test_c, NULL, "-fno-pie", "-static")) return 0;
	if (try_icl(logdepth, "cc/fnopie", test_c, NULL, NULL, NULL)) return 0;
	return try_fail(logdepth, "cc/fnopie");
}

int find_cc_fnopic(const char *name, int logdepth, int fatal)
{
	const char *test_c = test_hello_world;

	require("cc/cc", logdepth, fatal);

	report("Checking for -fno-pic... ");
	logprintf(logdepth, "find_cc_fnopic: trying to find -fno-pic...\n");
	logdepth++;

	if (try_icl(logdepth, "cc/fnopic", test_c, NULL, "-fno-pic", NULL)) return 0;
	if (try_icl(logdepth, "cc/fnopic", test_c, NULL, "-fno-pic", "-static")) return 0;
	if (try_icl(logdepth, "cc/fnopic", test_c, NULL, NULL, NULL)) return 0;
	return try_fail(logdepth, "cc/fnopic");
}

int find_soname(const char *name, int logdepth, int fatal)
{

	require("cc/cc", logdepth, fatal);

	report("Checking for soname... ");
	logprintf(logdepth, "find_soname: trying to find soname...\n");
	logdepth++;

	if (try_hello(logdepth, NULL, "-Wl,-soname,libscconfig.0", "cc/soname", "-Wl,-soname,")) return 0;
	if (try_hello(logdepth, NULL, NULL,                        "cc/soname", ""))             return 0;

	report("Not found.\n");
	return 1;
}

int find_so_undefined(const char *name, int logdepth, int fatal)
{
	static const char *test_c =
		NL "#include <stdio.h>"
		NL "void intentionally_undefined_symbol(void);"
		NL "int main() {"
		NL "	intentionally_undefined_symbol();"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL ;
	const char **t, *try_ldflags[] = {
		"",
		"-undefined dynamic_lookup", /* OSX + clang */
		NULL
	};

	require("cc/cc", logdepth, fatal);
	require("cc/ldflags_dynlib", logdepth, fatal);

	report("Checking for so_undefined... ");
	logprintf(logdepth, "find_so_undefined: trying to find so_undefined...\n");
	logdepth++;

	for(t = try_ldflags; *t != NULL; t++) {
		const char *fpic;
		char *ldf, *oname = ".o", *libname_dyn, *cflags_c;
		int res1, res2;

		fpic = get("cc/fpic");
		if (fpic == NULL) fpic = "";

		cflags_c = str_concat(" ", "-c", fpic, NULL);

		libname_dyn = (char *)get("sys/ext_dynlib");
		ldf = str_concat(" ", get("cc/ldflags_dynlib"), *t, NULL);
		res1 = compile_code(logdepth, test_c, &oname, NULL, cflags_c, NULL);
		res2 = compile_file(logdepth, oname, &libname_dyn, NULL, NULL, ldf);
		unlink(libname_dyn);
		unlink(oname);
		free(libname_dyn);
		free(oname);
		free(cflags_c);

		if ((res1 == 0) && (res2 == 0)) {
			put(name, *t);
			report("OK (%s)\n", *t);
			return 0;
		}
	}

	report("Not found.\n");
	return 1;
}


int find_wlrpath(const char *name, int logdepth, int fatal)
{

	require("cc/cc", logdepth, fatal);

	report("Checking for rpath... ");
	logprintf(logdepth, "find_wlrpath: trying to find rpath...\n");
	logdepth++;

	if (try_hello(logdepth, NULL, "-Wl,-rpath=.",  "cc/wlrpath", "-Wl,-rpath=")) return 0;

	report("Not found.\n");
	return 1;
}

int find_fpic(const char *name, int logdepth, int fatal)
{

	require("cc/cc", logdepth, fatal);

	report("Checking for -fpic... ");
	logprintf(logdepth, "find_fpic: trying to find -fpic...\n");
	logdepth++;

	if (try_hello(logdepth, NULL, "-fPIC", "cc/fpic", "-fPIC")) return 0;
	if (try_hello(logdepth, NULL, "-fpic", "cc/fpic", "-fpic")) return 0;
	if (try_hello(logdepth, NULL, NULL,    "cc/fpic", ""))      return 0;

	report("Not found.\n");
	return 1;
}

/* Hello world lib... */
static const char *test_lib =
		NL "#include <stdio.h>"
		NL "int hello() {"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL ;

/* ...and the corresponding host application */
static const char *test_host =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "#include %s"
		NL "#ifndef RTLD_NOW"
		NL "#define RTLD_NOW RTLD_LAZY" /* on old BSD and probably on SunOS */
		NL "#endif"
		NL "int main() {"
		NL "	void *handle = NULL;"
		NL "	void (*func)() = NULL;"
		NL "	char *error;"
		NL
		NL "	handle = dlopen(\"%s\", RTLD_NOW);"
		NL "	if (handle == NULL) {"
		NL "		printf(\"dlopen fails: %%s\", dlerror());"
		NL "		return 1;"
		NL "	}"
		NL "	func = dlsym(handle, \"hello\");"
		NL "	if (func == NULL) {"
		NL "		printf(\"dlsym fails: %%s\", dlerror());"
		NL "		return 1;"
		NL "	}"
		NL "	func();"
		NL "	return 0;"
		NL "}"
		NL ;

static int try_dynlib(int logdepth, const char *cflags, char *concated_ldflags, const char *name, const char *value, const char *host_app_cflags, const char *host_app_ldflags)
{
	char test_host_app[1024];
	const char *fpic;
	const char *ld_include;
	const char *dlc;
	char *libname, *libname_dyn;
	char *cflags_c;
	char *oname = ".o";
	int ret = 0;


	dlc = get("libs/dl-compat");
	if ((dlc != NULL) && (strcmp(dlc, strue) == 0))
		ld_include = "<dl-compat.h>";
	else
		ld_include = "<dlfcn.h>";

	fpic = get("cc/fpic");
	if (fpic == NULL) fpic = "";

	if (cflags == NULL)
		cflags="";

	cflags_c = malloc(strlen(cflags) + 8 + strlen(fpic));
	sprintf(cflags_c, "%s -c %s", cflags, fpic);


	libname_dyn = libname = (char *)get("sys/ext_dynlib");
	if ((compile_code(logdepth, test_lib, &oname, NULL, cflags_c, NULL) != 0) ||
			(compile_file(logdepth, oname, &libname_dyn, NULL, NULL, concated_ldflags) != 0)) {
			report("('%s': nope) ", concated_ldflags);
	}
	else {
		sprintf(test_host_app, test_host, ld_include, libname_dyn);
		if (try_flags(logdepth, NULL, test_host_app, host_app_cflags, host_app_ldflags, "OK")) {
			put(name, value);
			report("OK (%s)\n", value);
			ret = 1;
		}
	}
	unlink(libname_dyn);
	unlink(oname);
	if (libname != libname_dyn)
		free(libname_dyn);
	free(oname);
	free(concated_ldflags);
	free(cflags_c);
	return ret;
}


int find_ldflags_dynlib(const char *name, int logdepth, int fatal)
{

	require("cc/cc",       logdepth, fatal);
	require("cc/rdynamic", logdepth, fatal);
	require("cc/fpic",     logdepth, fatal);
	require("libs/ldl",    logdepth, fatal);

	report("Checking for dynamic library ldflags... ");
	logprintf(logdepth, "find_ldflags_dynlib: trying to find dynamic library ldflags...\n");
	logdepth++;

	if (try_dynlib(logdepth, NULL, concat_nodes("-dynamic -shared", "cc/rdynamic", "libs/ldl", NULL), "cc/ldflags_dynlib", "-dynamic -shared", NULL, get("libs/ldl"))) return 0;
	if (try_dynlib(logdepth, NULL, concat_nodes("-shared",          "cc/rdynamic", "libs/ldl", NULL), "cc/ldflags_dynlib", "-shared",          NULL, get("libs/ldl"))) return 0;
	if (try_dynlib(logdepth, NULL, concat_nodes("-G",                              "libs/ldl", NULL), "cc/ldflags_dynlib", "-G",               NULL, get("libs/ldl"))) return 0; /* xlc (on AIX) */
	report("Not found.\n");
	return 1;
}

static int try_dll_or_so(int logdepth, int is_dll, const char *lib_ldflags, const char *name, const char *value,
	const char *dspec_dllexport, const char *dspec_dllimport,
	const char *app_cflags, const char *app_ldflags)
{
	static const char *test_lib_template =
		NL "#include <stdio.h>"
		NL "%s void hello();"
		NL "void hello() {"
		NL "	puts(\"OK\");"
		NL "}"
		NL ;
	static const char *test_app_template =
		NL "%s void hello();"
		NL "int main() {"
		NL "	hello();"
		NL "	return 0;"
		NL "}"
		NL ;
	char test_lib[1024];
	char test_app[1024];
	const char *fpic;
	char *cflags_c;
	char *oname, *oname_ext;
	char *libname, *libname_ext;
	char *appname = NULL, *appname_ext = NULL;
	char *lib_filename = NULL, *lib_dirname = NULL;
	char *lib_ldflags_new = NULL;
	char *app_ldflags_new = NULL;
	size_t len, ii;
	int ret = 0;

	++logdepth;

	require("cc/cc", logdepth, 0);
	require("cc/cflags", logdepth, 0);
	require("cc/ldflags", logdepth, 0);
	require("cc/fpic", logdepth, 0);
	require("sys/ext_exe", logdepth, 0);
	require("sys/ext_dynlib_native", logdepth, 0);

	fpic = get("cc/fpic");
	if (fpic == NULL) fpic = "";

	if (app_cflags == NULL)
		app_cflags = "";

	if (app_ldflags == NULL)
		app_ldflags = "";

	cflags_c = str_concat(" ", get("cc/cflags"), "-c", fpic, NULL);

	oname = oname_ext = ".o";
	libname = libname_ext = (char *)get("sys/ext_dynlib_native");
	sprintf(test_lib, test_lib_template, dspec_dllexport);
	lib_ldflags_new = str_concat(" ", get("cc/ldflags"), lib_ldflags, NULL);
	if ((compile_code(logdepth, test_lib, &oname, NULL, cflags_c, NULL) != 0) ||
			(compile_file(logdepth, oname, &libname, NULL, NULL, lib_ldflags_new) != 0)) {
			report("FAILED (compiling %s)\n", (is_dll?"DLL":"SO"));
	}
	else {
		lib_filename = file_name(libname);
		lib_dirname = dir_name(libname);

		if (!is_dll) {
			len = strlen(lib_filename) - strlen(libname_ext);
			for (ii=3; ii<len; ++ii)
				lib_filename[ii-3] = lib_filename[ii];
			lib_filename[len-3] = 0;
		}

		app_ldflags_new = str_concat("", "-L", lib_dirname, " -l", lib_filename,
			" ", app_ldflags, " ", get("cc/ldflags"), NULL);

		appname = appname_ext = (char *)get("sys/ext_exe");
		sprintf(test_app, test_app_template, dspec_dllimport);
		if (compile_code(logdepth, test_app, &appname, NULL, app_cflags, app_ldflags_new) == 0) {
			put(name, value);
			report("OK (%s)\n", value);
			ret = 1;
		}
	}
	if (oname != oname_ext) {
		unlink(oname);
		free(oname);
	}
	if (libname != libname_ext) {
		unlink(libname);
		free(libname);
	}
	if (appname != appname_ext) {
		unlink(appname);
		free(appname);
	}
	free(cflags_c);
	free(lib_filename);
	free(lib_dirname);
	free(lib_ldflags_new);
	free(app_ldflags_new);
	return ret;
}

int find_ldflags_dll(const char *name, int logdepth, int fatal)
{
	char dll_ldflags[128];
	const char *dspec_dllimport;
	const char *dspec_dllexport;
	static const char *dll_implib_name = "libscconfigdll_0.a";

	require("cc/cc",                   logdepth, 1);
	require("cc/declspec/dllimport/*", logdepth, 0);
	require("cc/declspec/dllexport/*", logdepth, 0);
	require("cc/wloutimplib",          logdepth, 0);

	report("Checking for DLL ldflags... ");
	logprintf(logdepth, "find_ldflags_dll: trying to find DLL ldflags...\n");

	if (istrue(get("cc/declspec/dllimport/presents")))
		dspec_dllimport = " __declspec(dllimport) ";
	else
		dspec_dllimport = "";

	if (istrue(get("cc/declspec/dllexport/presents")))
		dspec_dllexport = " __declspec(dllexport) ";
	else
		dspec_dllexport = "";

	if (try_dll_or_so(logdepth+1, 1, "-shared", "cc/ldflags_dll", "-shared",
		dspec_dllexport, dspec_dllimport, NULL, NULL))
		return 0;

	if (get("cc/wloutimplib")) {
		sprintf(dll_ldflags, "-shared %s,%s", get("cc/wloutimplib"), dll_implib_name);
		if (try_dll_or_so(logdepth+1, 1, dll_ldflags, "cc/ldflags_dll", "-shared",
			dspec_dllexport, dspec_dllimport, NULL, "-L. -lscconfigdll_0")) {
			unlink(dll_implib_name);
			return 0;
		}
		unlink(dll_implib_name);
	}

	report("Not found.\n");
	return 1;
}

static int find_ldflags_so_impl(const char *name, int logdepth, int fatal)
{
	const char *ldflags[] = { "-dynamic -shared", "-shared", NULL };
	const char **ldf;

	require("cc/cc", logdepth, 1);

	report("Checking for SO ldflags... ");
	logprintf(logdepth, "find_ldflags_so: trying to find SO ldflags...\n");

	for (ldf=ldflags; *ldf != NULL; ++ldf)
		if (try_dll_or_so(logdepth+1, 0, *ldf, "cc/ldflags_so", *ldf,
			"", "", NULL, ""))
			return 0;

	report("Not found.\n");
	return 1;
}

int find_ldflags_so(const char *name, int logdepth, int fatal)
{
	int res;
	char *old_tmp;
	char *new_tmp;

	require("/host/sys/tmp", logdepth, 1);
	require("/host/sys/path_sep", logdepth, 1);

	/* HACK: modify */

	old_tmp = strclone(get("/host/sys/tmp"));
	new_tmp = str_concat("", old_tmp, get("/host/sys/path_sep"), "lib", NULL);
	put("/host/sys/tmp", new_tmp);
	free(new_tmp);

	res = find_ldflags_so_impl(name, logdepth, fatal);

	put("/host/sys/tmp", old_tmp);
	free(old_tmp);

	return res;
}

/* Hello world program to test compiler flags */
static const char *test_alloca =
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	char *s;"
		NL "	s = alloca(128);"
		NL "	if (s != NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL ;

static int try_alloca(int logdepth, const char *cflags, const char *ldflags, const char *name, const char *value)
{
	if (try_flags(logdepth, NULL, test_alloca, cflags, ldflags, "OK")) {
		put(name, value);
		report("OK (%s)\n", value);
		return 1;
	}
	return 0;
}

int find_alloca(const char *name, int logdepth, int fatal)
{
	require("cc/cc", logdepth, fatal);

	report("Checking for alloca()... ");
	logprintf(logdepth, "find_alloca: trying to find alloca()...\n");
	logdepth++;

	if (try_alloca(logdepth, NULL, NULL,     "cc/alloca/presents", "true"))      return 0;

	put("cc/alloca/presents", "false");
	report("Not found.\n");
	return 1;
}


int find__exit(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	_exit(0);"
		NL "	puts(\"BAD\");"
		NL "	return 0;"
		NL "}"
		NL ;

	require("cc/cc", logdepth, fatal);

	report("Checking for _exit()... ");
	logprintf(logdepth, "find__exit: trying to find _exit()...\n");
	logdepth++;

	if (try_flags_inv(logdepth, NULL, test_c, NULL, NULL, "BAD")) {
		put("cc/_exit/presents", strue);
		report("found\n");
		return 0;
	}

	put("cc/_exit/presents", sfalse);
	report("Not found.\n");
	return 1;
}

int find_cc_pragma_message(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "#define DO_PRAGMA(arg) _Pragma(#arg)"
		NL "#define TODO(x) DO_PRAGMA(message(\"TODO: \" #x))"
		NL "TODO(test)"
		NL "int main()"
		NL "{"
		NL "	puts(\"OK\");"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for _Pragma(message)... ");
	logprintf(logdepth, "find_cc_pragma_message: trying to find pragma_message...\n");
	logdepth++;
	if (try(logdepth, NULL, test_c, "OK")) {
		put("cc/pragma_message", strue);
		report("Found.\n");
		return 0;
	}
	put("cc/pragma_message", sfalse);
	report("Not found.\n");
	return 1;
}

int find_cc_static_libgcc(const char *name, int logdepth, int fatal)
{
	const char *test_c = test_hello_world;
	const char *key = "cc/static_libgcc";

	require("cc/cc", logdepth, fatal);

	report("Checking for -static-libgcc... ");
	logprintf(logdepth, "find_cc_static_libgcc: trying to find -static-libgcc...\n");
	logdepth++;

	if (try_icl(logdepth, key, test_c, NULL, NULL, "-static-libgcc")) return 0;
	return try_fail(logdepth, key);
}
