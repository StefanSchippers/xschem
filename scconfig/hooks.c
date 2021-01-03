#include <stdio.h>
#include <string.h>
#include "arg.h"
#include "db.h"
#include "libs.h"
#include "log.h"
#include "dep.h"
#include "tmpasm_scconfig.h"

#define version "2.0.1"

static void help(void)
{
	printf("./configure: configure xschem.\n");
	printf("\n");
	printf("Usage: ./configure [options]\n");
	printf("\n");
	printf("xschem-specific options:\n");
	printf(" --prefix=path             change installation prefix from /usr/local to path\n");
	printf(" --debug                   build full debug version (-g -O0)\n");
	printf(" --profile                 build profiling version if available (-pg)\n");
	printf(" --symbols                 include symbols (add -g, but no -O0)\n");
	printf(" --user-conf-dir           change the user conf dir (e.g. ~/.xschem)\n");
	printf(" --user-lib-path           set the user library path\n");
	printf(" --sys-lib-path            set the system library path\n");
	printf(" --xschem-lib-path         overrides the final list of library search paths\n");
	printf(" /arg/tk-version=8.x       force detecting a specific version of tcl/tk\n");

	printf("\n");
	help_default_args(stdout, "");
}

/* Runs when a custom command line argument is found
   returns true if no further argument processing should be done */
int hook_custom_arg(const char *key, const char *value)
{
	if (strcmp(key, "prefix") == 0) {
		report("Setting prefix to '%s'\n", value);
		put("/local/xschem/prefix", strclone(value));
		return 1;
	}
	if (strcmp(key, "debug") == 0) {
		put("/local/xschem/debug", strue);
		return 1;
	}
	if (strcmp(key, "profile") == 0) {
		put("/local/xschem/profile", strue);
		return 1;
	}
	if (strcmp(key, "symbols") == 0) {
		put("/local/xschem/symbols", strue);
		return 1;
	}
	if (strcmp(key, "user-conf-dir") == 0) {
		put("/local/xschem/user-conf-dir", value);
		return 1;
	}
	if (strcmp(key, "user-lib-path") == 0) {
		put("/local/xschem/user-lib-path", value);
		return 1;
	}
	if (strcmp(key, "sys-lib-path") == 0) {
		put("/local/xschem/sys-lib-path", value);
		return 1;
	}
	if (strcmp(key, "xschem-lib-path") == 0) {
		put("/local/xschem/xschem-lib-path", value);
		return 1;
	}
	if (strcmp(key, "help") == 0) {
		help();
		exit(0);
	}

	return 0;
}


/* Runs before anything else */
int hook_preinit()
{
	return 0;
}

/* Runs after initialization */
int hook_postinit()
{
	db_mkdir("/local");
	db_mkdir("/local/xschem");

	/* DEFAULTS */
	put("/local/xschem/prefix", "/usr/local");
	put("/local/xschem/debug", sfalse);
	put("/local/xschem/profile", sfalse);
	put("/local/xschem/symbols", sfalse);
	put("/local/xschem/user-conf-dir", "~/.xschem");

	return 0;
}

/* Runs after all arguments are read and parsed */
int hook_postarg()
{

	if (get("/local/xschem/user-lib-path") == NULL) {
		put("/local/xschem/user-lib-path", get("/local/xschem/user-conf-dir"));
		append("/local/xschem/user-lib-path", "/xschem_library");
	}

	if (get("/local/xschem/sys-lib-path") == NULL) {
		put("/local/xschem/sys-lib-path", get("/local/xschem/prefix"));
		append("/local/xschem/sys-lib-path", "/");
		append("/local/xschem/sys-lib-path", "share/xschem/xschem_library/devices");
	}

	if (get("/local/xschem/xschem-lib-path") == NULL) {
		put("/local/xschem/xschem-lib-path", get("/local/xschem/user-lib-path"));
		append("/local/xschem/xschem-lib-path", ":");
		append("/local/xschem/xschem-lib-path", get("/local/xschem/sys-lib-path"));

		append("/local/xschem/xschem-lib-path", ":");
		append("/local/xschem/xschem-lib-path", get("/local/xschem/prefix"));
		append("/local/xschem/xschem-lib-path", "/");
		append("/local/xschem/xschem-lib-path", "share/doc/xschem/examples");

		append("/local/xschem/xschem-lib-path", ":");
		append("/local/xschem/xschem-lib-path", get("/local/xschem/prefix"));
		append("/local/xschem/xschem-lib-path", "/");
		append("/local/xschem/xschem-lib-path", "share/doc/xschem/ngspice");

		append("/local/xschem/xschem-lib-path", ":");
		append("/local/xschem/xschem-lib-path", get("/local/xschem/prefix"));
		append("/local/xschem/xschem-lib-path", "/");
		append("/local/xschem/xschem-lib-path", "share/doc/xschem/logic");

		append("/local/xschem/xschem-lib-path", ":");
		append("/local/xschem/xschem-lib-path", get("/local/xschem/prefix"));
		append("/local/xschem/xschem-lib-path", "/");
		append("/local/xschem/xschem-lib-path", "share/doc/xschem/xschem_simulator");

		append("/local/xschem/xschem-lib-path", ":");
		append("/local/xschem/xschem-lib-path", get("/local/xschem/prefix"));
		append("/local/xschem/xschem-lib-path", "/");
		append("/local/xschem/xschem-lib-path", "share/doc/xschem/binto7seg");

		append("/local/xschem/xschem-lib-path", ":");
		append("/local/xschem/xschem-lib-path", get("/local/xschem/prefix"));
		append("/local/xschem/xschem-lib-path", "/");
		append("/local/xschem/xschem-lib-path", "share/doc/xschem/pcb");

		append("/local/xschem/xschem-lib-path", ":");
		append("/local/xschem/xschem-lib-path", get("/local/xschem/prefix"));
		append("/local/xschem/xschem-lib-path", "/");
		append("/local/xschem/xschem-lib-path", "share/doc/xschem/rom8k");

	}
	return 0;
}

/* Runs when things should be detected for the compilation host system (commands
   that will be executed on host and will produce files to be used on host) */
int hook_detect_host()
{
	return 0;
}

static void disable_xcb(void)
{
	put("libs/gui/xcb/presents", "");
	put("libs/gui/xcb/includes", "");
	put("libs/gui/xcb/cflags", "");
	put("libs/gui/xcb/ldflags", "");
	put("libs/gui/xcb_render/presents", "");
	put("libs/gui/xcb_render/includes", "");
	put("libs/gui/xcb_render/cflags", "");
	put("libs/gui/xcb_render/ldflags", "");
	put("libs/gui/xgetxcbconnection/presents", "");
	put("libs/gui/xgetxcbconnection/includes", "");
	put("libs/gui/xgetxcbconnection/cflags", "");
	put("libs/gui/xgetxcbconnection/ldflags", "");
}

/* Runs when things should be detected for the host->target system (commands
   that will be executed on host but will produce files to be used on target) */
int hook_detect_target()
{
	require("cc/fpic",  0, 0);

	{ /* need to set debug flags here to make sure libs are detected with the modified cflags; -ansi matters in what #defines we need for some #includes */
		const char *tmp, *fpic, *debug;
		fpic = get("/target/cc/fpic");
		if (fpic == NULL) fpic = "";
		debug = get("/arg/debug");
		if (debug == NULL) debug = "";
		tmp = str_concat(" ", fpic, debug, NULL);
		put("/local/global_cflags", tmp);

		/* for --debug mode, use -ansi -pedantic for all detection */
		if (istrue(get("/local/xschem/debug"))) {
			append("cc/cflags", " -g -O0");
			if (require("cc/argstd/Wall",  0, 0) == 0) {
				append("cc/cflags", " ");
				append("cc/cflags", get("cc/argstd/Wall"));
			}
			if (require("cc/argstd/std_c89",  0, 0) == 0) {
				append("cc/cflags", " ");
				append("cc/cflags", get("cc/argstd/std_c89"));
			}
			if (require("cc/argstd/pedantic",  0, 0) == 0) {
				append("cc/cflags", " ");
				append("cc/cflags", get("cc/argstd/pedantic"));
			}
		}
		else {
			append("cc/cflags", " -O2");
			if (istrue(get("/local/xschem/symbols")))
				append("cc/cflags", " -g");
		}

		if (istrue(get("/local/xschem/profile"))) {
			if (require("cc/argstd/pg",  0, 0) == 0) {
				append("cc/cflags", " ");
				append("cc/cflags", get("cc/argstd/pg"));
			}
			if (require("cc/argstd/no-pie",  0, 0) == 0) {
				append("cc/cflags", " ");
				append("cc/cflags", get("cc/argstd/no-pie"));
			}
		}
	}

	if (require("libs/io/popen/*",  0, 0) != 0) {
		if (require("libs/proc/fork/*",  0, 0) == 0) /* pipe is used together with fork, both needed */
			require("libs/io/pipe/*",  0, 0);
	}

	require("parsgen/flex/presents",  0, 1);
	require("parsgen/bison/presents",  0, 1);
	require("libs/script/tk/*",  0, 1); /* this will also bring libs/script/tcl/* */
	require("fstools/awk",  0, 1);
	require("libs/gui/xpm/*",  0, 1);
	require("libs/gui/cairo/*",  0, 0);
	require("libs/gui/xrender/*",  0, 0);

	if (require("libs/gui/cairo-xcb/*",  0, 0) != 0) {
		put("libs/gui/xcb/presents", sfalse);
	}
	else if (require("libs/gui/xcb/*",  0, 0) == 0) {
		/* if xcb is used, the code requires these: */
		require("libs/gui/xgetxcbconnection/*", 0, 0);
		if (!istrue(get("libs/gui/xgetxcbconnection/presents"))) {
			report("Disabling xcb because xgetxcbconnection is not found...\n");
			disable_xcb();
		}
		else {
			require("libs/gui/xcb_render/*", 0, 0);
			if (!istrue(get("libs/gui/xcb_render/presents"))) {
				report("Disabling xcb because xcb_render is not found...\n");
				disable_xcb();
			}
		}
	}

	return 0;
}

/* Runs when things should be detected for the runtime system  (commands
   that will be executed only on the target, never on host) */
int hook_detect_runtime()
{
	return 0;
}

static const char *isok(int retval, int *accumulator)
{
	*accumulator |= retval;
	return (retval == 0) ? "ok" : "ERROR";
}

/* Runs after detection hooks, should generate the output (Makefiles, etc.) */
int hook_generate()
{
	int generr = 0;

	printf("\n--- Generating build and config files\n");
	printf("config.h:              %s\n", isok(tmpasm("..", "config.h.in", "config.h"), &generr));
	printf("Makefile.conf:         %s\n", isok(tmpasm("..", "Makefile.conf.in", "Makefile.conf"), &generr));
	printf("doc/manpages/xschem.1: %s\n", isok(tmpasm("../doc/manpages", "xschem.1.in", "xschem.1"), &generr));
	printf("src/Makefile:          %s\n", isok(tmpasm("../src", "Makefile.in", "Makefile"), &generr));

	if (!generr) {
		printf("\n\n");
		printf("=====================\n");
		printf("Configuration summary\n");
		printf("=====================\n");

		printf("\nCompilation:\n");
		printf(" CC:        %s\n", get("/target/cc/cc"));
		printf(" debug:     %s\n", istrue(get("/local/xschem/debug")) ? "yes" : "no");
		printf(" profiling: %s\n", istrue(get("/local/xschem/profile")) ? "yes" : "no");

		printf("\nPaths:\n");
		printf(" prefix:        %s\n", get("/local/xschem/prefix"));
		printf(" user-conf-dir: %s\n", get("/local/xschem/user-conf-dir"));
		printf(" user-lib-path: %s\n", get("/local/xschem/user-lib-path"));
		printf(" sys-lib-path:  %s\n", get("/local/xschem/sys-lib-path"));

		printf("\nLibs & features:\n");
		printf(" tcl:       %s\n", get("/target/libs/script/tcl/ldflags"));
		printf(" tk:        %s\n", get("/target/libs/script/tk/ldflags"));
		printf(" cairo:     %s\n", istrue(get("/target/libs/gui/cairo/presents")) ? "yes" : "no");
		printf(" xrender:   %s\n", istrue(get("/target/libs/gui/xrender/presents")) ? "yes" : "no");
		printf(" xcb:       %s\n", istrue(get("/target/libs/gui/xcb/presents")) ? "yes" : "no");

		printf("\nConfiguration complete, ready to compile.\n\n");
	}

	return 0;
}

/* Runs before everything is uninitialized */
void hook_preuninit()
{
}

/* Runs at the very end, when everything is already uninitialized */
void hook_postuninit()
{
}

