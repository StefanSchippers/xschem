#define NL "\n"

/* main.c */
extern int no_autodetect_sys;		/* set this to 1 to suppress system and cross detection */
extern int no_save_cache;				/* set this to 1 to avoid saving config.cache */

/* lib_try.c: try to compile and run a test code; save results under prefix, if worked */
/* include, compile-flags, link-flags;
   NULL includes, cflags, *ldflags means don't put anything in the db; cflags
   and ldflags may be prefixed with "+" to include standard flags;
   the test code has to print "OK" if it worked. If prefix is NULL, do not
   modify the db or announce the output, silently return 0 or 1.
   Returns 1 if worked, 0 if not */
int try_icl(int logdepth, const char *prefix, const char *test_c_in, const char *includes, const char *cflags,
						const char *ldflags);

/* same as try_icl(), but does not execute the code, only compiles. Useful
   for test programs with undesirable side effects (e.g. gtk: would open a window) */
int try_icl_norun(int logdepth, const char *prefix, const char *test_c_in, const char *includes, const char *cflags,
									const char *ldflags);

/* same as try_icl, but also insert flags picked up from deps (if not NULL);
   useful for detecting features that depend on other detected features.
   If run is 0, do not run the test program, compile only */
int try_icl_with_deps(int logdepth, const char *prefix, const char *test_c_in, const char *includes, const char *cflags, const char *ldflags, const char *dep_includes, const char *dep_cflags, const char *dep_ldflags, int run);

/* Low level function for the same, giving more control to the caller */
int try_icl_(int logdepth, const char *prefix, const char *test_c_in, const char *includes, const char *cflags, const char *ldflags, int run, int (*accept_res)(char *stdout_str));

/* use try_icl() on a list of packages found by pkg-config. Stick to the version
   required if reqver is non-NULL, else try them in the order pkg-config returned
   them. */
int try_icl_pkg_config(int logdepth, const char *prefix, const char *test_c, char *includes, const char *pkgpat,
											 const char *reqver);

/* call this when failed to find the feature (after multiple try_*() calls);
   always returns 1 (so that return try_fail() does the Right Thing) */
int try_fail(int logdepth, const char *prefix);

/* Import an argument for controlling try_icl() */
int import_icl(const char *key, const char *fn);

/* Determine the sizeof() of a struct field; works the same way as icl() but
   also sets prefix/sizeof */
int try_icl_sfield(int logdepth, const char *prefix, const char *structn, const char *fieldn, const char *includes, const char *cflags, const char *ldflags);
int try_icl_sfields(int logdepth, const char *prefix, const char *structn, const char **fields, const char *includes, const char *cflags, const char *ldflags, int silent_exit_first_fail);


/* lib_compile.c */
extern int cross_blind;					/* 1 if crosscompiling is blind (no emulator to test with) */

char *shell_escape_dup(const char *in);	/* strdup in and escape any special char for the shell */

int compile_file(int logdepth, const char *fn_input, char **fn_output, const char *cc, const char *cflags, const char *ldflags);
int compile_code(int logdepth, const char *testcode, char **fn_output, const char *cc, const char *cflags, const char *ldflags);

/* same as above, but do not add cc/cflags and cc/ldfags */
int compile_file_raw(int logdepth, const char *fn_input, char **fn_output, const char *cc, const char *cflags, const char *ldflags);
int compile_code_raw(int logdepth, const char *testcode, char **fn_output, const char *cc, const char *cflags, const char *ldflags);

int run(int logdepth, const char *cmd_, char **stdout_saved);
int run_shell(int logdepth, const char *cmd_, char **stdout_saved);
int compile_run(int logdepth, const char *testcode, const char *cc, const char *cflags, const char *ldflags,
								char **stdout_saved);
int run_script(int logdepth, const char *interpreter, const char *script, const char *suffix, char **out);

/* lib_file.c */
int file_size(const char *name);
char *tempdir_new(int logdepth, const char *suffix);
char *tempfile_new(const char *suffix);
char *tempfile_dump(const char *testcode, const char *suffix);
char *load_file(const char *name);
int is_dir(const char *path);
int is_file(const char *path);
int exists(const char *path);
int exists_in(const char *dir, const char *file);
char *file_name(const char *path);	/* returns malloc'd buffer */
char *dir_name(const char *path);	/* returns malloc'd buffer */
char *tempfile_new_noabort(const char *suffix);	/* for internal use - returns NULL instead of aborting when temp file can not be created */
int touch_file(const char *path);

/* lib_filelist.c */
void filelist(int logdepth, const char *dir, int *argc, char ***argv);
void filelist_free(int *argc, char ***argv);

/* lib_pkg_config.c */

/** run pkg config on @pkgname:
    - with `--cflags` if cflags  is not NULL, storing the result in cflags  (malloc()'d)
    - with `--libs`   if ldflags is not NULL, storing the result in ldflags (malloc()'d)
   Returns 0 on success.
*/
int run_pkg_config(int logdepth, const char *pkgname, char **cflags, char **ldflags);

/** same as run_pkg_config(), but runs a generic config tool (e.g. gdconfig)
    passed in confname */
int run_gen_config(int logdepth, const char *confname, const char *pkgname, char **cflags, char **ldflags);

int run_pkg_config_modversion(int logdepth, const char *pkgname, char **modversion);
int run_pkg_config_modversion_db(int logdepth, const char *node, const char *pkgname);

/** run pkg-config --list-all and keep lines matching regex pkgpat.

    argc/argv is a filelist output, each item pair is package name returned by
    pkg_config (odd items are full package names, even items are suffixes:
    pkgpath match removed)
*/
void run_pkg_config_lst(int logdepth, const char *pkgpat, int *argc, char ***argv);


/* lib_uniqinc.c */
char **uniq_inc_arr(const char *includes, int indirect, const char *sep, int *numlines);	/* split includes by sep; includes is a list of nodes to get() if indirect is non-zero; return a NULL-terminated array of unique include strings and set *numlines if numlines is not NULL */
void uniq_inc_free(char **arr);	/* free an array returned by uniq_inc_arr() */
char *uniq_inc_str(const char *includes, const char *isep, const char *osep, int sort, int eren, char **eres);	/* take a long list of includes separated by isep and emit an uniq list separated by osep */
char *order_inc_str(const char *includes, const char *isep, const char *word1, int dir, const char *word2); /* take a long list of includes separated by isep and emit a new list where word1 is moved before/after of word2 if dir < 0 or dir > 0 */

/* find_types.c */
int find_types_something_t(const char *name, int logdepth, int fatal, const char* prefix, const char *typ, const char* define, const char *try_include);

/* str.c */
char *strclone(const char *str);
char *trim_left(char *str);
char *trim_right(char *str);
char *strip(char *str);
char *str_chr(char *str, char c);
char *str_rchr(char *str, char c);
char *str_subsn(const char *str);	/* advanced strdup that also interprets \n */
char *str_concat(const char *sep, ...);	/* concat a list of strings into a newly allocated buffer, putting sep between them */
char *esc_interpret(const char *str);
int chr_inset(char c, const char *set);	/* returns whether c is in set */

/* srctree.c */

/* Run svn info on dir and extract the value for key;
   key is case sensitive. The first match is returned or NULL if not found
   or on error. */
char *svn_info(int logdepth, const char *dir, const char *key);

#define isblind(root)  ((strncmp((root), "/target", 7) == 0) && cross_blind)
#define istarget(root) (strncmp((root), "/target", 7) == 0)

#define target_emu_fail(out) ((isblind(db_cwd)) && (out == NULL))

#define safeNULL(s)  ((s) == NULL ? "(NULL)" : (s))
#define str_null(s)  ((s) == NULL ? "" : (s))

/* Test program helper: generate code that ensures a given FUNCT exists
   and is a function; can be turned off by defining SCCONFIG_ACCEPT_IMPLICIT
   on scconfig compilation time */
/* Both FUNCT1 and FUNCT2 argument *must* be used exactly once! In some
   cases FUNCT1 and FUNCT2 is a format string parameter. We expect, however,
   both arguments will substituted to the same value. */
#ifdef SCCONFIG_ACCEPT_IMPLICIT
#	define no_implicit(RET_TYPE, FUNCT1, FUNCT2) \
		"/* accept implicit (" FUNCT1 ", " FUNCT2 ") */\n"
#else
#	define no_implicit(RET_TYPE, FUNCT1, FUNCT2) \
		"#ifndef " FUNCT1 "\n" \
		"{ " #RET_TYPE " (*tmp)() = " FUNCT2 "; if (tmp) {}}\n" \
		"#endif\n"
#endif
