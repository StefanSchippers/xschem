/*
    scconfig - library to query files and directories
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "db.h"
#include "libs.h"
#include "log.h"
#include "dep.h"

int file_size(const char *name)
{
	struct stat buf;
	if (stat(name, &buf) != 0)
		return -1;
	return buf.st_size;
}

char *tempdir_new(int logdepth, const char *suffix)
{
	char s[1024];
	char *cmd;
	const char *tmp;
	const char *mkdir, *emu;
	unsigned int rn, n;

	require("sys/tmp", logdepth+1, 1);
	tmp = get("sys/tmp");

	if (strlen(suffix) > sizeof(s) - strlen(tmp) - 32) {
		fprintf(stderr, "Not enough room for creating temporary file name\n");
		abort();
	}

	require("fstools/mkdir", logdepth+1, 1);
	mkdir = get("fstools/mkdir");

	emu = get("sys/emu");
	if (emu == NULL)
		emu = "";

	for(n = 0; n < 128; n++) {
		rn = rand() % 100000;
		sprintf(s, "%sscc_%u%s", tmp, rn, suffix);
		if (!exists(s)) {
			char *s_esc = shell_escape_dup(s);
			cmd = malloc(strlen(s_esc) + strlen(mkdir) + 16);
			sprintf(cmd, "%s %s", mkdir, s_esc);
			run_shell(logdepth+1, cmd, NULL);
			free(s_esc);
			free(cmd);
			if (is_dir(s))
				return strclone(s);
		}
	}
	error("Couldn't find a suitable temp dir name\n");
	abort();
}

char *tempfile_new_noabort(const char *suffix)
{
	char s[1024];
	const char *tmp;
	unsigned int rn, n;
	FILE *f;


	require("/host/sys/tmp", 0, 1);
	tmp = get("/host/sys/tmp");
	if (strlen(suffix) > sizeof(s) - strlen(tmp) - 32) {
		fprintf(stderr, "tempfile_new_noabort(): not enough room for creating temporary file name\n");
		abort();
	}

	for(n = 0; n < 128; n++) {
		rn = rand() % 100000;
		sprintf(s, "%sscc_%u%s", tmp, rn, suffix);
		if (!is_file(s)) { /* can not test for exists() because that would recurse is_dir */
			f = fopen(s, "w");
			if (f != NULL) {
				fclose(f);
				return strclone(s);
			}
		}
	}
	return NULL;
}

char *tempfile_new(const char *suffix)
{
	char *tmp;

	tmp = tempfile_new_noabort(suffix);
	if (tmp == NULL) {
		error("Couldn't find a suitable temp file name\n");
		abort();
	}
	return tmp;
}

char *tempfile_dump(const char *testcode, const char *suffix)
{
	char *fn;
	FILE *f;

	fn = tempfile_new(suffix);
	f  = fopen(fn, "w");
	fprintf(f, "%s", testcode);
	fclose(f);
	return fn;
}

char *load_file(const char *name)
{
	int size;
	char *content;
	FILE *f;

	size = file_size(name);
	if (size > 0) {
		content = malloc(size+1);
		*content = '\0';
		f = fopen(name, "r");
		if (f != NULL) {
			int len = fread(content, 1, size, f);
			if (len < 0)
				len = 0;
			content[len] = '\0';
			fclose(f);
		}
	}
	else {
		content = malloc(1);
		*content = '\0';
	}
	return content;
}

int is_dir(const char *path)
{
	char *tmp, *path_esc;
	int ret;

	require("sys/shell", 0, 1);

	path_esc = shell_escape_dup(path);
	tmp = malloc(strlen(path_esc) + 16);
	sprintf(tmp, "cd %s", path_esc);
	ret = run_shell(0, tmp, NULL);
	free(tmp);
	free(path_esc);
	return !ret;
}

int is_file(const char *path)
{
	return file_size(path) >= 0;
}

int exists(const char *path)
{
	return is_file(path) || is_dir(path);
}


int exists_in(const char *dir, const char *file)
{
	char *path;
	int ret;

	path = malloc(strlen(dir) + strlen(file) + 5);
	sprintf(path, "%s/%s", dir, file);
	ret = is_file(path) || is_dir(path);
	free(path);
	return ret;
}

const char *file_name_ptr(const char *path)
{
	const char *s;
	s = str_rchr((char *)path, '/');
	if (s == NULL)
		s = str_rchr((char *)path, '\\');
	return s;
}

char *file_name(const char *path)
{
	const char *s;
	s = file_name_ptr(path);
	if (s == NULL)
		return strclone(path);
	s++;
	return strclone(s);
}

char *dir_name(const char *path)
{
	char *s, *r;
	s = strclone(path);
	r = (char *)file_name_ptr(s);
	if (r == NULL) {
		free(s);
		return strclone("");
	}
	*r = '\0';
	return s;
}

int touch_file(const char *path)
{
	FILE *f;
	f = fopen(path, "a");
	if (f == NULL)
		return -1;
	fclose(f);
	return 0;
}
