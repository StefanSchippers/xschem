#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"

int brute_force_include(int logdepth, const char *language, const char *test_c, const char *ldflags_base, const char *basedir);


/* script detection */
int find_script_gpmi(const char *name, int logdepth, int fatal);
int find_script_tcl(const char *name, int logdepth, int fatal);
int find_script_tk(const char *name, int logdepth, int fatal);
int find_script_ruby(const char *name, int logdepth, int fatal);
int find_script_mruby(const char *name, int logdepth, int fatal);
int find_script_python(const char *name, int logdepth, int fatal);
int find_script_python3(const char *name, int logdepth, int fatal);
int find_script_perl(const char *name, int logdepth, int fatal);
int find_script_mawk(const char *name, int logdepth, int fatal);
int find_script_lua(const char *name, int logdepth, int fatal);
int find_script_guile(const char *name, int logdepth, int fatal);
int find_script_stutter(const char *name, int logdepth, int fatal);
int find_script_estutter(const char *name, int logdepth, int fatal);
int find_script_funlisp(const char *name, int logdepth, int fatal);
int find_script_duktape(const char *name, int logdepth, int fatal);
int find_script_fungw(const char *name, int logdepth, int fatal);
int find_script_fungw_user_call_ctx(const char *name, int logdepth, int fatal);
int find_script_fungw_cfg_pupdir(const char *name, int logdepth, int fatal);
int find_script_fungw_all(const char *name, int logdepth, int fatal);
int find_script_m4(const char *name, int logdepth, int fatal);
