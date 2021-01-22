/* cc */
int find_cc(const char *name, int logdepth, int fatal);
int find_cc_argstd(const char *name, int logdepth, int fatal);
int find_cc_argmachine(const char *name, int logdepth, int fatal);
int find_cc_fpie(const char *name, int logdepth, int fatal);
int find_cc_fnopie(const char *name, int logdepth, int fatal);
int find_cc_fnopic(const char *name, int logdepth, int fatal);
int find_inline(const char *name, int logdepth, int fatal);
int find_varargmacro(const char *name, int logdepth, int fatal);
int find_funcmacro(const char *name, int logdepth, int fatal);
int find_constructor(const char *name, int logdepth, int fatal);
int find_destructor(const char *name, int logdepth, int fatal);
int find_fattr_unused(const char *name, int logdepth, int fatal);
int find_declspec_dllimport(const char *name, int logdepth, int fatal);
int find_declspec_dllexport(const char *name, int logdepth, int fatal);
int find_rdynamic(const char *name, int logdepth, int fatal);
int find_soname(const char *name, int logdepth, int fatal);
int find_so_undefined(const char *name, int logdepth, int fatal);
int find_wlrpath(const char *name, int logdepth, int fatal);
int find_cc_wloutimplib(const char *name, int logdepth, int fatal);
int find_cc_wloutputdef(const char *name, int logdepth, int fatal);
int find_fpic(const char *name, int logdepth, int fatal);
int find_ldflags_dynlib(const char *name, int logdepth, int fatal);
int find_ldflags_dll(const char *name, int logdepth, int fatal);
int find_ldflags_so(const char *name, int logdepth, int fatal);
int find_alloca(const char *name, int logdepth, int fatal);
int find__exit(const char *name, int logdepth, int fatal);
int find_cc_pragma_message(const char *name, int logdepth, int fatal);
int find_cc_static_libgcc(const char *name, int logdepth, int fatal);

/* libs */
int find_lib_ldl(const char *name, int logdepth, int fatal);
int find_lib_LoadLibrary(const char *name, int logdepth, int fatal);
int find_lib_errno(const char *name, int logdepth, int fatal);

/* thread */
int find_lib_lpthread(const char *name, int logdepth, int fatal);
int find_thread_semget(const char *name, int logdepth, int fatal);
int find_thread_pthread_create(const char *name, int logdepth, int fatal);
int find_thread_CreateSemaphore(const char *name, int logdepth, int fatal);
int find_thread_CreateThread(const char *name, int logdepth, int fatal);

/* fscalls */
int find_fs_realpath(const char *name, int logdepth, int fatal);
int find_fs__fullpath(const char *name, int logdepth, int fatal);
int find_fs_readdir(const char *name, int logdepth, int fatal);
int find_fs_findnextfile(const char *name, int logdepth, int fatal);
int find_fs_access(const char *name, int logdepth, int fatal);
int find_fs_access_macros(const char *name, int logdepth, int fatal);
int find_fs_stat_macros(const char *name, int logdepth, int fatal);
int find_fs_stat_fields(const char *name, int logdepth, int fatal);
int find_fs_lstat(const char *name, int logdepth, int fatal);
int find_fs_statlstat(const char *name, int logdepth, int fatal);
int find_fs_getcwd(const char *name, int logdepth, int fatal);
int find_fs__getcwd(const char *name, int logdepth, int fatal);
int find_fs_getwd(const char *name, int logdepth, int fatal);
int find_fs_mkdir(const char *name, int logdepth, int fatal);
int find_fs__mkdir(const char *name, int logdepth, int fatal);
int find_fs_utime(const char *name, int logdepth, int fatal);
int find_fs__utime(const char *name, int logdepth, int fatal);
int find_fs__utime64(const char *name, int logdepth, int fatal);
int find_fs_mkdtemp(const char *name, int logdepth, int fatal);
int find_fs_mmap(const char *name, int logdepth, int fatal);
int find_fsmount_next_dev(const char *name, int logdepth, int fatal);
int find_fsmount_fsstat_fields(const char *name, int logdepth, int fatal);
int find_fsmount_statfs_fields(const char *name, int logdepth, int fatal);
int find_fsmount_statvfs_fields(const char *name, int logdepth, int fatal);
int find_fs_ustat(const char *name, int logdepth, int fatal);
int find_fs_statfs(const char *name, int logdepth, int fatal);
int find_fs_statvfs(const char *name, int logdepth, int fatal);
int find_fs_flock(const char *name, int logdepth, int fatal);
int find_fs_makedev(const char *name, int logdepth, int fatal);

/* printf */
int find_printf_x(const char *name, int logdepth, int fatal);
int find_printf_ptrcast(const char *name, int logdepth, int fatal);
int find_snprintf(const char *name, int logdepth, int fatal);
int find_dprintf(const char *name, int logdepth, int fatal);
int find_vdprintf(const char *name, int logdepth, int fatal);
int find_vsnprintf(const char *name, int logdepth, int fatal);

/* proc */
int find_proc__spawnvp(const char *name, int logdepth, int fatal);
int find_proc_fork(const char *name, int logdepth, int fatal);
int find_proc_wait(const char *name, int logdepth, int fatal);
int find_proc__getpid(const char *name, int logdepth, int fatal);
int find_proc_CreateProcessA(const char *name, int logdepth, int fatal);
int find_proc_getexecname(const char *name, int logdepth, int fatal);
int find_proc_GetModuleFileNameA(const char *name, int logdepth, int fatal);
int find_proc_shmget(const char *name, int logdepth, int fatal);
int find_proc_CreateFileMappingA(const char *name, int logdepth, int fatal);

/* fstools */
int find_fstools_cp(const char *name, int logdepth, int fatal);
int find_fstools_ln(const char *name, int logdepth, int fatal);
int find_fstools_mv(const char *name, int logdepth, int fatal);
int find_fstools_rm(const char *name, int logdepth, int fatal);
int find_fstools_mkdir(const char *name, int logdepth, int fatal);
int find_fstools_ar(const char *name, int logdepth, int fatal);
int find_fstools_ranlib(const char *name, int logdepth, int fatal);
int find_fstools_awk(const char *name, int logdepth, int fatal);
int find_fstools_cat(const char *name, int logdepth, int fatal);
int find_fstools_sed(const char *name, int logdepth, int fatal);
int find_fstools_file(const char *name, int logdepth, int fatal);
int find_fstools_file_l(const char *name, int logdepth, int fatal);
int find_fstools_chmodx(const char *name, int logdepth, int fatal);

/* uname */
int find_uname(const char *name, int logdepth, int fatal);
int find_triplet(const char *name, int logdepth, int fatal);
int find_sysid(const char *name, int logdepth, int fatal);

/* find_target */
int find_target(const char *name, int logdepth, int fatal);

/* filelist */
int find_filelist(const char *name, int logdepth, int fatal);

/* find_str.c */
int find_strcasecmp(const char *name, int logdepth, int fatal);
int find_strncasecmp(const char *name, int logdepth, int fatal);
int find_stricmp(const char *name, int logdepth, int fatal);
int find_strnicmp(const char *name, int logdepth, int fatal);

/* find_sys.c */
int find_sys_ptrwidth(const char *name, int logdepth, int fatal);
int find_sys_byte_order(const char *name, int logdepth, int fatal);
int find_tmp(const char *name, int logdepth, int fatal);
int find_shell(const char *name, int logdepth, int fatal);

/* find_io.c */
int find_io_pipe(const char *name, int logdepth, int fatal);
int find_io_pipe2(const char *name, int logdepth, int fatal);
int find_io__pipe(const char *name, int logdepth, int fatal);
int find_io_dup2(const char *name, int logdepth, int fatal);
int find_io_fileno(const char *name, int logdepth, int fatal);
int find_io_lseek(const char *name, int logdepth, int fatal);
int find_io_popen(const char *name, int logdepth, int fatal);

/* find_time.c */
int find_time_usleep(const char *name, int logdepth, int fatal);
int find_time_Sleep(const char *name, int logdepth, int fatal);
int find_time_gettimeofday(const char *name, int logdepth, int fatal);
int find_time_ftime(const char *name, int logdepth, int fatal);
int find_time_timegm(const char *name, int logdepth, int fatal);
int find_time_mkgmtime(const char *name, int logdepth, int fatal);
int find_time_gmtime_s(const char *name, int logdepth, int fatal);
int find_time_gmtime_r(const char *name, int logdepth, int fatal);
int find_time_localtime_s(const char *name, int logdepth, int fatal);
int find_time_localtime_r(const char *name, int logdepth, int fatal);

/* find_types.c */
int find_types_stdint(const char *name, int logdepth, int fatal);
int find_types_sizes(const char *name, int logdepth, int fatal);
int find_types_size_t(const char *name, int logdepth, int fatal);
int find_types_off_t(const char *name, int logdepth, int fatal);
int find_types_off64_t(const char *name, int logdepth, int fatal);
int find_types_gid_t(const char *name, int logdepth, int fatal);
int find_types_uid_t(const char *name, int logdepth, int fatal);
int find_types_pid_t(const char *name, int logdepth, int fatal);
int find_types_dev_t(const char *name, int logdepth, int fatal);
int find_types_dev_t(const char *name, int logdepth, int fatal);
int find_types_mode_t(const char *name, int logdepth, int fatal);
int find_types_nlink_t(const char *name, int logdepth, int fatal);
int find_types_ptrdiff_t(const char *name, int logdepth, int fatal);
int find_types_dev_t(const char *name, int logdepth, int fatal);
int find_types_ino_t(const char *name, int logdepth, int fatal);
int find_types_void_ptr(const char *name, int logdepth, int fatal);

/* find_signal.c */
int find_signal_names(const char *name, int logdepth, int fatal);
int find_signal_raise(const char *name, int logdepth, int fatal);

/* environ.c */
int find_main_arg3(const char *name, int logdepth, int fatal);
int find_putenv(const char *name, int logdepth, int fatal);
int find_setenv(const char *name, int logdepth, int fatal);
int find_environ(const char *name, int logdepth, int fatal);
