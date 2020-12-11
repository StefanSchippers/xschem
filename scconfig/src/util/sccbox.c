/*
    scconfig - sccbox: portable copy, link, mkdir, remove, etc.
    Copyright (C) 2016, 2017, 2020  Tibor Palinkas

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
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>

/*********************** DOCUMENTATION **************************/

static void help_generic(const char *prg)
{
	printf("sccbox - Makefile helper\n\n");
	printf("Invocation: %s cmd [switches] [paths]\n\n", prg);
	printf("sccbox and provides a basic set of file-system utilities as a single,\n");
	printf("static linkable executable with dependency only on libc. It is implemented\n");
	printf("in C89 and uses only a very thin layer of non-C89 OS calls that trace\n");
	printf("back to early BSD days and are widely available on all platforms.\n\n");
	printf("The intention is to replace 'mkdir -p' and 'rm -f' and similar,\n");
	printf("non-portable commands with a less efficient, but more portable local\n");
	printf("implementation. Using sccbox for mkdir/install/uninstall exclusively also\n");
	printf("simplifies the Makefile rules because the semantics of uninstall\n");
	printf("matches the semantics of install and the same list of source files\n");
	printf("and destination can be passed and source files are not removed.\n\n");
	printf("For more info read the following help topics using --help topic:\n");
	printf("  error       error handling\n");
	printf("  rm          remove files (e.g. for make clean)\n");
	printf("  mkdir       create directories (e.g. for make *install)\n");
	printf("  mktemp      create a temporary file/directory\n");
	printf("  ln          create symlink (e.g. for make *install)\n");
	printf("  install     copy-install files (e.g. for make install)\n");
	printf("  linstall    symlink-install files (e.g. for make linstall)\n");
	printf("  uninstall   remove files (e.g. for make uninstall)\n");
	printf("  touch       touch files (open for append)\n");
	printf("  touchnew    touch non-existing files, ignore existing files\n");
	printf("\n");
}

static void help_error(const char *prg)
{
	printf("sccbox - error handling (all commands)\n\n");
	printf("Any sccbox command by default will try to carry out the requested\n");
	printf("operation assuming no errors, e.g. an mkdir will assume the target\n");
	printf("directory does not exists but all parent directories are in place.\n");
	printf("If such a command fails, the exit status is non-zero and error messages\n");
	printf("are printed to stderr.\n\n");
	printf("If the --quiet switch is specified, the command will try to do the same\n");
	printf("but never prints error messages and always returns 0. This is useful\n");
	printf("in situations when failure is expected (note: 2>/dev/null is not portable)\n\n");
	printf("If the --force switch is specified, the command will make extre efforts,\n");
	printf("sometimes even destructive ones, to fulfill the request. If it fails,\n");
	printf("error messages are printed (unless --quiet).\n\n");
}

static void help_rm_common(void)
{
	printf("  --quiet  don't print error messages and set exit status to 0\n");
	printf("  --force  a non-existing file is not an error\n");
}

static void help_rm(const char *prg)
{
	printf("sccbox rm [switches] paths\n\n");
	printf("Remove one or more paths from the file system. Paths can be\n");
	printf("files, symlinks and empty directories. Recursive removal is not\n");
	printf("supported. If multiple paths specified, attempt to remove them all\n");
	printf("even if some can not be removed.\n\n");
	printf("Switches:\n");
	help_rm_common();
	printf("\n");
}

static void help_mkdir(const char *prg)
{
	printf("sccbox mkdir [switches] paths\n\n");
	printf("Create one or more directories.\n\n");
	printf("Switches:\n");
	printf("  --quiet  don't print error messages and set exit status to 0\n");
	printf("  --force  ignored (there's nothing to force)\n");
	printf("  -p       create parent directories automatically\n");
	printf("  -i       ignored\n");
	printf("  -l       ignored\n");
	printf("  -c       ignored\n");
	printf("  -u       do not do anyhting, exit 0\n");
	printf("  -r       do not do anyhting, exit 0\n\n");
}

static void help_mktemp(const char *prg)
{
	printf("sccbox mktemp [switches]\n\n");
	printf("Create a temporary file or directory and print its path to stdout\n\n");
	printf("Switches:\n");
	printf("  --quiet  don't print error messages and set exit status to 0\n");
	printf("  --force  ignored (there's nothing to force)\n");
	printf("  -d       create a directory, not a file\n");
	printf("  -p tmp   path to tmpdir\n");
	printf("  -t tpl   use tpl as file name template\n\n");
}

static void help_ln_common(void)
{
	printf("  --quiet     don't print error messages and set exit status to 0\n");
	printf("  --force     attempt to remove target if it exists\n");
}

static void help_ln(const char *prg)
{
	printf("sccbox ln [switches] existing new\n");
	printf("sccbox ln [switches] existing1 existing2 ... dir\n\n");
	printf("Create one or more symlinks. Intended for linking installed paths\n");
	printf("to other installed paths.\n\n");
	printf("Switches:\n");
	help_ln_common();
	printf("  --absolute  convert existing paths to absolute paths\n\n");
	printf("  --relative  make single symlink point to relative path\n\n");
}

static void help_install(const char *prg)
{
	printf("sccbox install [switches] src_file dst_file\n");
	printf("sccbox install [switches] src_file1 src_file2 ... dir\n\n");
	printf("Copies (or links or removes) one or more files. Intended to install\n");
	printf("files from the source or build tree to the installation root.\n\n");
	printf("Switches:\n");
	printf("  --quiet     don't print error messages and set exit status to 0\n");
	printf("  --force     install: attempt to remove destination\n");
	printf("  --absolute  convert existing paths to absolute paths\n");
	printf("  --relative  make single symlink point to relative path\n\n");
	printf("  -d          force destination to be a directory (even remove old dest)\n");
	printf("  -i          install (copy) files\n");
	printf("  -c          install (copy) files\n");
	printf("  -u          uninstall (remove) files - see --help uninstall\n");
	printf("  -r          uninstall (remove) files - see --help uninstall\n");
	printf("  -l          linstall (symlink) files - see --help linstall\n\n");
}

static void help_linstall(const char *prg) {
	printf("sccbox linstall [switches] src_file dst_file\n");
	printf("sccbox linstall [switches] src_file1 src_file2 ... dir\n\n");
	printf("Installs (a) file(s) using symlinks. Automatically convert src_file\n");
	printf("paths to absolute paths. Intended for developers: if a package is\n");
	printf("symlink-installed, it can be run from the installation after a\n");
	printf("recompilation without needing to reinstall.\n\n");
	printf("Switches:\n");
	help_ln_common();
	printf("  --preserve  do not convert source paths to absolute\n\n");
}

static void help_uninstall(const char *prg) {
	printf("sccbox uninstall [switches] src_file dst_file\n");
	printf("sccbox uninstall [switches] src_file1 src_file2 ... dir\n\n");
	printf("Remove (an) installed file(s). In the first form, src_file is ignored.\n");
	printf("In the second form dir is used to calculate destination paths.\n\n");
	printf("The purpose of this command is to unify install and uninstall rules:\n");
	printf("unlike cp and rm, an 'sccbox install' and 'sccbox uninstall' can\n");
	printf("be used with the same parameter list to install and uninstall a set of files\n\n");
	printf("Switches:\n");
	help_rm_common();
	printf("\n");
}

static void help_touch(const char *prg)
{
	printf("sccbox touch file1 file2 ... fileN\n");
	printf("sccbox touchnew file1 file2 ... fileN\n\n");
	printf("Open all files listed, for append, and close them immedately.\n");
	printf("This makes sure the files exist and also bumps their modification date.\n");
	printf("Command touchnew ignores existing files; its primary use is to make\n");
	printf("sure a file exists without making changes to already existing files.\n");
}

static int help(const char *prg, const char *topic)
{
	if (topic == NULL) help_generic(prg);
	else if (strcmp(topic, "error") == 0) help_error(prg);
	else if (strcmp(topic, "rm") == 0) help_rm(prg);
	else if (strcmp(topic, "mkdir") == 0) help_mkdir(prg);
	else if (strcmp(topic, "mktemp") == 0) help_mktemp(prg);
	else if (strcmp(topic, "ln") == 0) help_ln(prg);
	else if (strcmp(topic, "install") == 0) help_install(prg);
	else if (strcmp(topic, "linstall") == 0) help_linstall(prg);
	else if (strcmp(topic, "uninstall") == 0) help_uninstall(prg);
	else if (strcmp(topic, "touch") == 0) help_touch(prg);
	else if (strcmp(topic, "touchnew") == 0) help_touch(prg);
	else {
		printf("No such help topic: %s\n", topic);
		return 1;
	}
	return 0;
}



/*********************** IMPLEMENTATION: low level **************************/
#ifdef PATH_MAX
#	define MY_PATH_MAX PATH_MAX
#else
#	define MY_PATH_MAX 32768
#endif

typedef enum {
	INSTALL,     /* install with cp */
	LINSTALL,    /* install with symlinks */
	UNINSTALL    /* remove installed files */
} scc_mode_t;

#define kill_flag  argv[n] = NULL
#define kill_flag2  do { argv[n] = NULL; n++; argv[n] = NULL; } while(0)

#define load_flags(code) \
	do { \
		int n, flags = 1; \
		for(n = 1; n < argc; n++) { \
			char *arg = argv[n]; \
			char *payload = argv[n+1]; \
			(void)payload; \
			if ((*arg == '-') && (flags)) { \
				if ((arg[1] == '-') && (arg[2] == '\0')) { \
					kill_flag; \
					flags = 0; \
					continue; \
				} \
				while(*arg == '-') arg++; \
				{ code; } \
			} \
		} \
	} while(0) \

#define load_args_minus(delta, code) \
	do { \
		int n; \
		for(n = 1; n < argc-delta; n++) {\
			char *arg = argv[n]; \
			if (arg != NULL) \
				{ code; } \
		} \
	} while(0);

#define load_args(code) load_args_minus(0, code)\


static int safe_isdir(const char *path)
{
	struct stat st;

	if (stat(path, &st) != 0)
		return 0;
	
	return S_ISDIR(st.st_mode);
}

static int safe_isfile(const char *path)
{
	struct stat st;

	if (stat(path, &st) != 0)
		return 0;
	
	return S_ISREG(st.st_mode);
}

static int safe_remove(const char *src, int need_existing_fd, int quiet)
{
	struct stat st;
	int exists;

	if (quiet)
		return remove(src);

	exists = stat(src, &st) == 0;
	if ((need_existing_fd) && (!exists)) {
		fprintf(stderr, "sccbox: Can't remove %s: no such file or directory\n", src);
		return 1;
	}
	

	if (exists && (remove(src) != 0)) {
		perror("sccbox");
		fprintf(stderr, "sccbox: Can't remove %s\n", src);
		return 1;
	}
	return 0;
}

#define issep(chr) (((chr) == '/') || ((chr) == '\\'))

static char *my_basename(const char *path)
{
	const char *s = path + strlen(path);
	for(s--; s >= path; s--) {
		if (issep(*s))
			return (char *)s+1;
	}
	return (char *)path;
}

static int path_concat(char *out, size_t out_len, const char *dir, const char *file)
{
	char *bn = my_basename(file);
	size_t dlen = strlen(dir), blen = strlen(bn);

	if (dlen+blen+2 > out_len)
		return 1;

	memcpy(out, dir, dlen);
	out[dlen] = '/'; dlen++;
	memcpy(out+dlen, bn, blen+1);
	return 0;
}

static int safe_mkdir_p(const char *path);


static int safe_copy(const char *src, const char *dst, int force, int quiet, int dst_is_dir)
{
	char buff[16384], dst2[MY_PATH_MAX];
	const char *dst_name = NULL;
	FILE *fd, *fs;
	struct stat st, dst_st;
	int err = 0, remove_dst = 1;

	if (stat(src, &st) != 0) {
		if (!quiet)
			fprintf(stderr, "sccbox: can't stat %s\n", src);
		return 1;
	}

	/* make sure symlinks/sockets/etc. are not copied into: remove any dst that's not a dir or a regular file */
	if (stat(dst, &dst_st) == 0) {
		int try_remove = dst_is_dir ? (!S_ISDIR(dst_st.st_mode)) : (!S_ISDIR(dst_st.st_mode) || !S_ISREG(dst_st.st_mode));
		if (try_remove) {
			if (remove(dst) != 0) {
				if (!quiet)
					fprintf(stderr, "sccbox: filed to remove non-regular/non-directory target %s\n", dst);
				return 1;
			}
		}
	}

	if (dst_is_dir) {
		remove_dst = 0;
		if (safe_mkdir_p(dst) != 0) {
			if (!quiet)
				fprintf(stderr, "sccbox: filed to create target directory %s\n", dst);
			return 1;
		}
	}

	fs = fopen(src, "rb");
	if (fs == NULL) {
		if (!quiet)
			fprintf(stderr, "sccbox: can't copy %s to %s: can not open source for write\n", src, dst2);
		return 1;
	}

	if ((force) && (remove_dst))
		remove(dst);

	fd = fopen(dst, "wb");

	if (fd == NULL) {
		/* no luck opening the dest file, maybe dst is a directory, try to create a file under it */
		if (path_concat(dst2, sizeof(dst2), dst, src) != 0) {
			if (!quiet)
				fprintf(stderr, "sccbox: can't copy %s to %s: resulting path is too long\n", src, dst);
			fclose(fs);
			return 1;
		}


		fd = fopen(dst2, "wb");
		if ((fd == NULL) && (force)) {
			remove(dst2);
			fd = fopen(dst2, "wb");
			if (fd == NULL) {
				/* dest was not a directory or couldn't host our new file; if force, try to
				   overwrite whatever dst was */
				remove(dst);
				fd = fopen(dst, "wb");
				if (fd == NULL) {
					if (!quiet)
						fprintf(stderr, "sccbox: can't copy %s to %s: destination can not be overwritten\n", src, dst2);
					fclose(fs);
					return 1;
				}
				dst_name = dst;
			}
		}
		if (fd == NULL) {
			if (!quiet)
				fprintf(stderr, "sccbox: can't copy %s to %s: can not open destination for write\n", src, dst2);
			fclose(fs);
			return 1;
		}
		if (dst_name == NULL)
			dst_name = dst2;
	}
	else
		dst_name = dst;

	/* manual copy - the only portable way */
	for(;;) {
		size_t len = fread(buff, 1, sizeof(buff), fs);
		if (len == 0)
			break;
		if (fwrite(buff, 1, len, fd) != len) {
			if (!quiet) {
				perror("sccbox");
				fprintf(stderr, "sccbox: can't copy %s to %s\n", src, dst_name);
			}
			err = 1;
			if (!force)
				break;
		}
	}

	chmod(dst_name, st.st_mode); /* this may fail on windows or on strange FS, don't check the return value */

	fclose(fs);
	fclose(fd);
	return err;
}

static const char *safe_get_pwd(int quiet)
{
	static char pwd_buff[MY_PATH_MAX];
	static int valid = 0;
	if (!valid) {
		FILE *f;
		char *end;
		f = popen("pwd", "r");

		if (f == NULL) {
			if (!quiet)
				perror("sccbox: running pwd");
			return NULL;
		}
		if (fgets(pwd_buff, sizeof(pwd_buff), f) == NULL) {
			if (!quiet)
				perror("sccbox: reading pwd's output");
			pclose(f);
			return NULL;
		}
		pclose(f);
		end = pwd_buff + strlen(pwd_buff) - 1;
		while((end >= pwd_buff) && ((*end == '/') || (*end == '\n')  || (*end == '\r'))) {
			*end = '\0';
			end--;
		}
		if (*pwd_buff != '/') {
			if (!quiet)
				fprintf(stderr, "sccbox: invalid pwd: '%s'\n", pwd_buff);
			return NULL;
		}
		valid = 1;
	}
	return pwd_buff;
}

/* Both 'path' and 'from' are absolute paths; recalculate path to be a relative
   path from 'from' and copy the result to buf. Returns buf. */
char *relativize(char *buf, const char *path, const char *from)
{
	const char *p, *f, *c, *lastp, *lastf;
	char *end = buf;
	int cnt = 0, rem = MY_PATH_MAX-1, len;

	for(lastp = p = path, lastf = f = from; *p == *f; p++,f++) {
		/* skip double separators */
		if (issep(p[0])) {
			while(issep(p[0]) && issep(p[1])) p++;
			lastp = p+1;
		}
		if (issep(f[0])) {
			while(issep(f[0]) && issep(f[1])) f++;
			lastf = f+1;
		}
	}

	p = lastp;
	f = lastf;

	for(c = f; *c != '\0'; c++) {
		if (issep(c[0])) {
			cnt++;
			while(issep(c[0]) && issep(c[1])) c++;
		}
	}

	while(cnt > 0) {
		if (rem < 3)
			return NULL;
		strcpy(end, "../");
		end += 3;
		rem -= 3;
		cnt--;
	}

	len = strlen(p);
	if (len >= rem)
		return NULL;
	strcpy(end, p);

	return buf;
}

/* create a symlink - needs to work only on UNIX but needs to compile on win32,
   so use system("ln -s") and avoid link-related syscalls */
static int safe_link(const char *src, const char *dst, int absolute, int relative, int force, int quiet, int dst_is_dir)
{
	char cmd[MY_PATH_MAX*2+16], full_src_tmp[MY_PATH_MAX], rel_src_tmp[MY_PATH_MAX];
	const char *full_src, *supp;
	int remove_dst = 1;

	if ((absolute) && (*src != '/')) {
		/* fix up relative source paths */
		const char *pwd = safe_get_pwd(quiet);
		int pwdlen, srclen;
		if (pwd == NULL) {
			if (!quiet)
				fprintf(stderr, "sccbox: can't figure current working directory for relative symlink %s to %s\n", src, dst);
			return 1;
		}
		if ((src[0] == '.') && (src[1] == '/'))
			src += 2;
		pwdlen = strlen(pwd);
		srclen = strlen(src);
	
		if ((pwdlen + srclen + 2) >= sizeof(full_src_tmp)) {
			if (!quiet)
				fprintf(stderr, "sccbox: can't link %s to %s: resulting path is too long\n", src, dst);
			return 1;
		}
		memcpy(full_src_tmp, pwd, pwdlen);
		full_src_tmp[pwdlen] = '/';
		memcpy(full_src_tmp+pwdlen+1, src, srclen+1);
		full_src = full_src_tmp;
	}
	else
		full_src = src;

	if (dst_is_dir) {
		struct stat dst_st;
		if ((stat(dst, &dst_st) == 0) && (!S_ISDIR(dst_st.st_mode))) {
			if (remove(dst) != 0) {
				if (!quiet)
					fprintf(stderr, "sccbox: filed to remove non-directory target %s\n", dst);
				return 1;
			}
		}

		remove_dst = 0;
		if (safe_mkdir_p(dst) != 0) {
			if (!quiet)
				fprintf(stderr, "sccbox: filed to create target directory %s\n", dst);
			return 1;
		}
	}

	if (force)
		remove(dst);

	supp = quiet ? " 2>/dev/null" : "";

	if (relative) {
		const char *fdst;
		char full_dst_tmp[MY_PATH_MAX];

		if (*dst != '/') {
			const char *pwd = safe_get_pwd(quiet);
			if (pwd == NULL) {
				if (!quiet)
					fprintf(stderr, "sccbox: can't figure current working directory for relative path '%s'\n", dst);
				return 1;
			}
			if (path_concat(full_dst_tmp, sizeof(full_dst_tmp), pwd, dst) != 0) {
				if (!quiet)
					fprintf(stderr, "sccbox: can't link %s to %s: resulting path is too long\n", src, dst);
				return 1;
			}
			fdst = full_dst_tmp;
		}
		else
			fdst = dst;
		full_src = relativize(rel_src_tmp, full_src, fdst);
	}

	sprintf(cmd, "ln -s \"%s\" \"%s\"%s", full_src, dst, supp);
	return system(cmd);
}

static int safe_mkdir_p(const char *path)
{
	char *curr, *next, *s;
	int res = 1, len = strlen(path);
	char *p = malloc(len+1);
	memcpy(p, path, len+1);

	/* do not create existing directories */
	if (safe_isdir(path))
			return 0;

	curr = p;
	if ((isalpha(p[0]) && (p[1] == ':') && issep(p[2]))) {
		/* windows special case: c:\ */
		curr += 3;
	}

	/* remove trailing path separators so we don't create empty dirs at the end */
	s = p+len-1;
	while((s >= p) && (issep(*s))) {
		*s = '\0';
		s--;
	}

	for(next = curr; next != NULL; curr = next) {
		char old;
		next = strpbrk(curr, "/\\");
		if (next != NULL) {
			old = *next;
			*next = '\0';
		}
		res = mkdir(p, 0755);
		if (next != NULL) {
			*next = old;
			next++;
		}
	}

	if (res != 0) {
		perror("sccbox");
		fprintf(stderr, "sccbox: failed to make directory with parents: %s\n", path);
	}

	free(p);
	return res; /* return the result of the last mkdir only, previous failures probably meant existing parents */
}

static int safe_touch(const char *fn)
{
	FILE *f;
	f = fopen(fn, "a");
	if (f == NULL)
		return -1;
	fclose(f);
	return 0;
}

/*********************** IMPLEMENTATION: high level **************************/
int cmd_rm(int argc, char *argv[])
{
	int err = 0, quiet = 0, need_existing_fd = 1;
	load_flags(
		switch(*arg) {
			case 'f': need_existing_fd = 0; kill_flag; break; /* nop: can't do more than remove() */
			case 'q': quiet = 1; kill_flag; break;
		}
	);
	load_args(
		err |= safe_remove(arg, need_existing_fd, quiet);
	);
	

	if (quiet)
		return 0;

	return err;
}


int cmd_install(int argc, char *argv[], scc_mode_t mode, int absolute)
{
	int force = 0, err = 0, quiet = 0, dst_is_dir, force_dst_dir = 0, relative = 0;
	const char *last;

	load_flags(
		if ((arg[0] == 'r') && (arg[1] == 'e')) arg[0] = 'R'; /* --relative */
		switch(*arg) {
			case 'c': /* --copy */
			case 'i': /* --install */
				mode = INSTALL;
				kill_flag;
				break;
			case 'l': /* --ln or --linstall */
				mode = LINSTALL;
				kill_flag;
				break;
			case 'r': /* --rm */
			case 'u': /* --uninstall */
				mode = UNINSTALL;
				kill_flag;
				break;
			case 'f': /* --force */
				force = 1;
				kill_flag;
				break;
			case 'd': /* --directory */
				force_dst_dir = 1;
				kill_flag;
				break;
			case 'q': /* --quiet */
				quiet = 1;
				kill_flag;
				break;
			case 'p': /* --preserve */
				absolute = 0;
				kill_flag;
				break;
			case 'a': /* --absolute */
				absolute = 1;
				kill_flag;
				break;
			case 'R': /* --relative */
				relative = 1;
				kill_flag;
				break;
		}
	);

	last = argv[argc-1];
	dst_is_dir = safe_isdir(last);

	load_args_minus(1,
/*printf("arg=%s last=%s force=%d q=%d d=%d\n", arg, last, force, quiet, force_dst_dir);*/
		switch(mode) {
			case INSTALL:   err |= safe_copy(arg, last, force, quiet, force_dst_dir); break;
			case LINSTALL:  err |= safe_link(arg, last, absolute, relative, force, quiet, force_dst_dir); break;
			case UNINSTALL:
				if (dst_is_dir) {
					char path[MY_PATH_MAX];
					if (path_concat(path, sizeof(path), last, arg) != 0) {
						if (!quiet) {
							fprintf(stderr, "sccbox: can't remove %s/%s: resulting path is too long\n", last, arg);
							err |= 1;
						}
					}
					else
						err |= safe_remove(path, !force, quiet);
				}
				break;
		}
		if ((mode == UNINSTALL) && (!dst_is_dir))
			err |= safe_remove(last, !force, quiet);
	);

	if (quiet)
		return 0;
	return err;
}

int cmd_mkdir(int argc, char *argv[])
{
	int parents = 0, err = 0, quiet = 0;

	load_flags(
		switch(*arg) {
			case 'q': /* --quiet */
				quiet = 1; kill_flag; break;
			case 'f':
				kill_flag; break;
			case 'i':
			case 'l':
			case 'c':
				kill_flag; break; /* ignore these for compatibility with install */
			case 'r': /* --rm */
			case 'u': /* --uninstall */
				return 0; /* don't do anything for uninstall */
			case 'p': parents = 1; kill_flag; break;
		}
	);

	if (!parents) {
		load_args(
			if ((mkdir(arg, 0755) != 0) && (!quiet)) {
				perror("sccbox");
				fprintf(stderr, "sccbox: failed to make directory %s\n", arg);
				err |= 1;
			}
		)
	}
	else {
		load_args(
			err |= safe_mkdir_p(arg);
		);
	}

	if (quiet)
		return 0;

	return err;
}

int rand_chr(void)
{
	static int seeded = 0;
	static const char map[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_";

	if (!seeded) {
		FILE *f;
		long seed;

		f = fopen("/dev/urandom", "rb");
		if (f != NULL) {
			int n;
			for(n = 0; n < 4; n++) {
				seed <<= 8;
				seed |= fgetc(f);
			}
			fclose(f);
		}
		else
			seed = time(NULL);
		srand(seed);
		seeded = 1;
	}
	return map[(int)((double)rand() / (double)RAND_MAX * (sizeof(map)-1))];
}

int cmd_mktemp(int argc, char *argv[])
{
	int want_dir = 0, err = 0, quiet = 0, retry;
	const char *template = "scctmp.^^^^^^^^";
	const char *tmpdir = "."; /* assume . is the safe place to create temp files the unsafe way */
	char path[MY_PATH_MAX];

	load_flags(
		switch(*arg) {
			case 'q': /* --quiet */
				quiet = 1; kill_flag; break;
			case 'd': /* --quiet */
				want_dir = 1; kill_flag; break;
			case 't': /* --template */
				template = payload; kill_flag2; break;
			case 'p': /* --path */
				tmpdir = payload; kill_flag2; break;
			case 'f':
				kill_flag; break; /* ignore */
		}
	);

	for(retry = 0; retry < 8; retry++) {
		int dlen = strlen(tmpdir), rc = 0;
		const char *t;
		char *end;

		/* generate a path */
		memcpy(path, tmpdir, dlen);
		end = path+dlen;
		for(*end++ = '/', t = template; *t != '\0'; end++, t++) {
			if (*t == '^') {
				*end = rand_chr();
				rc++;
			}
			else
				*end = *t;
		}
		while(rc < 8) { /* make sure there are at least 8 random characters */
			*end++ = rand_chr();
			rc++;
		}
		*end = '\0';

		if (want_dir) {
			if (mkdir(path, 0755) == 0) {
				printf("%s\n", path);
				return 0;
			}
		}
		else {
			FILE *f = fopen(path, "w");
			if (f != NULL) {
				fclose(f);
				printf("%s\n", path);
				return 0;
			}
		}
	}

	if (quiet)
		return 0;

	fprintf(stderr, "sccbox: failed to create temporary %s\n", want_dir ? "directory" : "file");

	return err;
}

int cmd_touch(int argc, char *argv[], int only_new)
{
	int n, res = 0;

	for(n = 1; n < argc; n++) {
		if ((only_new) && (safe_isfile(argv[n])))
			continue; /* skip existing file in touchnew */

		if (safe_touch(argv[n]) != 0) {
			res = 1;
			fprintf(stderr, "sccbox: failed to touch %s\n", argv[n]);
		}
	}
	return res;
}


int main(int argc, char *argv[])
{
	const char *prg = argv[0];
	if (argc > 1) {
		if (strstr(argv[0], "sccbox") != NULL) {
			argv++;
			argc--;
		}

		if (strcmp(argv[0], "--help") == 0) return help(prg, argv[1]);
		if (strcmp(argv[0], "help") == 0)   return help(prg, argv[1]);
		if (strcmp(argv[0], "-h") == 0)     return help(prg, argv[1]);

		if (strcmp(argv[0], "rm") == 0)        return cmd_rm(argc, argv);
		if (strcmp(argv[0], "install") == 0)   return cmd_install(argc, argv, INSTALL, 1);
		if (strcmp(argv[0], "linstall") == 0)  return cmd_install(argc, argv, LINSTALL, 1);
		if (strcmp(argv[0], "uninstall") == 0) return cmd_install(argc, argv, UNINSTALL, 1);
		if (strcmp(argv[0], "mkdir") == 0)     return cmd_mkdir(argc, argv);
		if (strcmp(argv[0], "mktemp") == 0)    return cmd_mktemp(argc, argv);
		if (strcmp(argv[0], "ln") == 0)        return cmd_install(argc, argv, LINSTALL, 0);
		if (strcmp(argv[0], "touch") == 0)     return cmd_touch(argc, argv, 0);
		if (strcmp(argv[0], "touchnew") == 0)  return cmd_touch(argc, argv, 1);
		fprintf(stderr, "sccbox: unknown command %s\n", argv[0]);
	}
	else
		fprintf(stderr, "sccbox: need arguments\n");
	fprintf(stderr, "sccbox: try --help\n");
	return 1;
}
