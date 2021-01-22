/*
    scconfig - dependency list of default tests
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

#include "dep.h"
#include "find.h"

void deps_default_init(void)
{
	dep_add("cc/cc",                    find_cc);
	dep_add("cc/argstd/*",              find_cc_argstd);
	dep_add("cc/cflags",                find_cc);
	dep_add("cc/ldflags",               find_cc);
	dep_add("cc/inline",                find_inline);
	dep_add("cc/varargmacro",           find_varargmacro);
	dep_add("cc/funcmacro",             find_funcmacro);
	dep_add("cc/constructor",           find_constructor);
	dep_add("cc/destructor",            find_destructor);
	dep_add("cc/rdynamic",              find_rdynamic);
	dep_add("cc/soname",                find_soname);
	dep_add("cc/so_undefined",          find_so_undefined);
	dep_add("cc/wlrpath",               find_wlrpath);
	dep_add("cc/wloutimplib",           find_cc_wloutimplib);
	dep_add("cc/wloutputdef",           find_cc_wloutputdef);
	dep_add("cc/fpic",                  find_fpic);
	dep_add("cc/fpie/*",                find_cc_fpie);
	dep_add("cc/fnopie/*",              find_cc_fnopie);
	dep_add("cc/fnopic/*",              find_cc_fnopic);
	dep_add("cc/alloca/*",              find_alloca);
	dep_add("cc/_exit/*",               find__exit);
	dep_add("cc/ldflags_dynlib",        find_ldflags_dynlib);
	dep_add("cc/ldflags_dll",           find_ldflags_dll);
	dep_add("cc/ldflags_so",            find_ldflags_so);
	dep_add("cc/func_attr/unused/*",    find_fattr_unused);
	dep_add("cc/declspec/dllimport/*",  find_declspec_dllimport);
	dep_add("cc/declspec/dllexport/*",  find_declspec_dllexport);
	dep_add("cc/argmachine/*",          find_cc_argmachine);
	dep_add("cc/pragma_message/*",      find_cc_pragma_message);
	dep_add("cc/static_libgcc/*",       find_cc_static_libgcc);

	dep_add("libs/ldl",                 find_lib_ldl);
	dep_add("libs/LoadLibrary/*",       find_lib_LoadLibrary);
	dep_add("libs/lpthread",            find_lib_lpthread);
	dep_add("libs/lpthread-recursive",  find_lib_lpthread);
	dep_add("thread/semget/*",          find_thread_semget);
	dep_add("thread/pthread_create/*",  find_thread_pthread_create);
	dep_add("thread/CreateSemaphore/*", find_thread_CreateSemaphore);
	dep_add("thread/CreateThread/*",    find_thread_CreateThread);
	dep_add("libs/errno/*",             find_lib_errno);
	dep_add("libs/printf_x",            find_printf_x);
	dep_add("libs/printf_ptrcast",      find_printf_ptrcast);
	dep_add("libs/snprintf",            find_snprintf);
	dep_add("libs/snprintf_safe",       find_snprintf);
	dep_add("libs/dprintf",             find_dprintf);
	dep_add("libs/vdprintf",            find_vdprintf);
	dep_add("libs/vsnprintf",           find_vsnprintf);
	dep_add("libs/proc/_spawnvp/*",     find_proc__spawnvp);
	dep_add("libs/proc/fork/*",         find_proc_fork);
	dep_add("libs/proc/wait/*",         find_proc_wait);
	dep_add("libs/proc/_getpid/*",      find_proc__getpid);
	dep_add("libs/proc/CreateProcessA/*",find_proc_CreateProcessA);
	dep_add("libs/proc/getexecname/*",  find_proc_getexecname);
	dep_add("libs/proc/GetModuleFileNameA/*",find_proc_GetModuleFileNameA);
	dep_add("libs/proc/shmget/*",       find_proc_shmget);
	dep_add("libs/proc/CreateFileMappingA/*",find_proc_CreateFileMappingA);
	dep_add("libs/fs/realpath/*",       find_fs_realpath);
	dep_add("libs/fs/_fullpath/*",      find_fs__fullpath);
	dep_add("libs/fs/readdir/*",        find_fs_readdir);
	dep_add("libs/fs/findnextfile/*",   find_fs_findnextfile);
	dep_add("libs/fs/stat/macros/*",    find_fs_stat_macros);
	dep_add("libs/fs/stat/fields/*",    find_fs_stat_fields);
	dep_add("libs/fs/access/*",         find_fs_access);
	dep_add("libs/fs/access/macros/*",  find_fs_access_macros);
	dep_add("libs/fs/lstat/*",          find_fs_lstat);
	dep_add("libs/fs/statlstat/*",      find_fs_statlstat);
	dep_add("libs/fs/getcwd/*",         find_fs_getcwd);
	dep_add("libs/fs/_getcwd/*",        find_fs__getcwd);
	dep_add("libs/fs/getwd/*",          find_fs_getwd);
	dep_add("libs/fs/mkdir/*",          find_fs_mkdir);
	dep_add("libs/fs/_mkdir/*",         find_fs__mkdir);
	dep_add("libs/fs/utime/*",          find_fs_utime);
	dep_add("libs/fs/_utime/*",         find_fs__utime);
	dep_add("libs/fs/_utime64/*",       find_fs__utime64);
	dep_add("libs/fs/mkdtemp/*",        find_fs_mkdtemp);
	dep_add("libs/fs/mmap/*",           find_fs_mmap);
	dep_add("libs/fsmount/next_dev/*",  find_fsmount_next_dev);
	dep_add("libs/fsmount/struct_fsstat/*",find_fsmount_fsstat_fields);
	dep_add("libs/fsmount/struct_statfs/*",find_fsmount_statfs_fields);
	dep_add("libs/fsmount/struct_statvfs/*",find_fsmount_statvfs_fields);
	dep_add("libs/fs/ustat/*",          find_fs_ustat);
	dep_add("libs/fs/statfs/*",         find_fs_statfs);
	dep_add("libs/fs/statvfs/*",        find_fs_statvfs);
	dep_add("libs/fs/flock/*",          find_fs_flock);
	dep_add("libs/fs/makedev/*",        find_fs_makedev);

	dep_add("libs/io/pipe/*",           find_io_pipe);
	dep_add("libs/io/pipe2/*",          find_io_pipe2);
	dep_add("libs/io/_pipe/*",          find_io__pipe);
	dep_add("libs/io/dup2/*",           find_io_dup2);
	dep_add("libs/io/fileno/*",         find_io_fileno);
	dep_add("libs/io/lseek/*",          find_io_lseek);
	dep_add("libs/io/popen/*",          find_io_popen);
	dep_add("libs/time/usleep/*",       find_time_usleep);
	dep_add("libs/types/stdint/*",      find_types_stdint);
	dep_add("sys/types/size/*",         find_types_sizes);
	dep_add("libs/time/Sleep/*",        find_time_Sleep);
	dep_add("libs/time/gettimeofday/*", find_time_gettimeofday);
	dep_add("libs/time/ftime/*",        find_time_ftime);
	dep_add("libs/time/timegm/*",       find_time_timegm);
	dep_add("libs/time/_mkgmtime/*",    find_time_mkgmtime);
	dep_add("libs/time/gmtime_r/*",     find_time_gmtime_r);
	dep_add("libs/time/gmtime_s/*",     find_time_gmtime_s);
	dep_add("libs/time/localtime_r/*",  find_time_localtime_r);
	dep_add("libs/time/localtime_s/*",  find_time_localtime_s);
	dep_add("libs/env/main_3arg/*",     find_main_arg3);
	dep_add("libs/env/putenv/*",        find_putenv);
	dep_add("libs/env/setenv/*",        find_setenv);
	dep_add("libs/env/environ/*",       find_environ);
	dep_add("signal/raise/*",           find_signal_raise);
	dep_add("signal/names/*",           find_signal_names);
	dep_add("fstools/cp",               find_fstools_cp);
	dep_add("fstools/ln",               find_fstools_ln);
	dep_add("fstools/mv",               find_fstools_mv);
	dep_add("fstools/rm",               find_fstools_rm);
	dep_add("fstools/mkdir",            find_fstools_mkdir);
	dep_add("fstools/ar",               find_fstools_ar);
	dep_add("fstools/ranlib",           find_fstools_ranlib);
	dep_add("fstools/awk",              find_fstools_awk);
	dep_add("fstools/cat",              find_fstools_cat);
	dep_add("fstools/sed",              find_fstools_sed);
	dep_add("fstools/file_l",           find_fstools_file_l);
	dep_add("fstools/file",             find_fstools_file);
	dep_add("fstools/chmodx",           find_fstools_chmodx);
	dep_add("sys/name",                 find_uname);
	dep_add("sys/uname",                find_uname);
	dep_add("sys/triplet",              find_triplet);
	dep_add("sys/sysid",                find_sysid);
	dep_add("sys/shell",                find_shell);
	dep_add("sys/shell_needs_quote",    find_shell);
	dep_add("sys/tmp",                  find_tmp);
	dep_add("sys/shell_eats_backslash", find_tmp);
	dep_add("sys/ext_exe",              find_uname);
	dep_add("sys/ext_dynlib",           find_uname);
	dep_add("sys/ext_dynlib_native",    find_uname);
	dep_add("sys/ext_stalib",           find_uname);
	dep_add("sys/class",                find_uname);
	dep_add("sys/path_sep",             find_uname);
	dep_add("sys/ptrwidth",             find_sys_ptrwidth);
	dep_add("sys/byte_order",           find_sys_byte_order);
	dep_add("sys/types/size_t/*",       find_types_size_t);
	dep_add("sys/types/off_t/*",        find_types_off_t);
	dep_add("sys/types/off64_t/*",      find_types_off64_t);
	dep_add("sys/types/gid_t/*",        find_types_gid_t);
	dep_add("sys/types/uid_t/*",        find_types_uid_t);
	dep_add("sys/types/pid_t/*",        find_types_pid_t);
	dep_add("sys/types/mode_t/*",       find_types_mode_t);
	dep_add("sys/types/nlink_t/*",      find_types_nlink_t);
	dep_add("sys/types/ptrdiff_t/*",    find_types_ptrdiff_t);
	dep_add("sys/types/dev_t/*",        find_types_dev_t);
	dep_add("sys/types/ino_t/*",        find_types_ino_t);
	dep_add("sys/types/void_ptr/*",     find_types_void_ptr);
	dep_add("str/strcasecmp/*",         find_strcasecmp);
	dep_add("str/strncasecmp/*",        find_strncasecmp);
	dep_add("str/stricmp/*",            find_stricmp);
	dep_add("str/strnicmp/*",           find_strnicmp);

	dep_add("/internal/filelist/cmd",    find_filelist);
	dep_add("/internal/filelist/method", find_filelist);
}
