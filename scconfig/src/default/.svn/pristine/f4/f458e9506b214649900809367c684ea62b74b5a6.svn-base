/*
    scconfig - library for listing files in a directory
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
#include "db.h"
#include "libs.h"
#include "log.h"
#include "dep.h"

static void destroy_testdir(int logdepth, char *dir)
{
	const char *rm;
	char *cmd, *dir_esc;

	rm = get("fstools/rm");
	if (rm == NULL) {
		logprintf(logdepth, "CAN NOT delete test directory '%s': no rm available\n", dir);
		return;
	}

	if (dir == NULL)
		return;

	logprintf(logdepth, "deleting test directory '%s'\n", dir);

	cmd = malloc(strlen(dir) + strlen(rm) + 4);
	dir_esc = shell_escape_dup(dir);
	sprintf(cmd, "%s %s", rm, dir_esc);
	run_shell(0, cmd, NULL);
	free(cmd);
	free(dir);
	free(dir_esc);
}

static char *create_testdir(int logdepth)
{
	char *dir, *fn, *cmd;
	const char *mkdir;
	int n;
	logprintf(logdepth, "creating test directory\n");

	dir = tempdir_new(logdepth+1, "");
	logprintf(logdepth, "sandbox is: '%s'\n", dir);

	fn = malloc(strlen(dir) + 32);
	for(n = 0; n < 2; n++) {
		FILE *f;
		sprintf(fn, "%s%sfile%d", dir, get("sys/path_sep"), n+1);
		f = fopen(fn, "w");
		if (f != NULL) {
			fclose(f);
			if (!is_file(fn)) {
				logprintf(logdepth, "Can not create file %s\n", fn);
				free(fn);
				destroy_testdir(logdepth, dir);
				return NULL;
			}
		}
	}

	mkdir = get("fstools/mkdir");

	cmd = malloc(strlen(dir) + 64);
	for(n = 0; n < 2; n++) {
		char *fn_esc;
		sprintf(fn, "%s%sdir%d", dir, get("sys/path_sep"), n+1);
		fn_esc = shell_escape_dup(fn);
		sprintf(cmd, "%s %s", mkdir, fn_esc);
		free(fn_esc);
		if (run_shell(logdepth+1, cmd, NULL) || (!is_dir(fn))) {
			logprintf(logdepth, "Can not create directory %s\n", fn);
			free(fn);
			free(cmd);
			destroy_testdir(logdepth, dir);
			return NULL;
		}
	}
	free(cmd);
	free(fn);
	return dir;
}

static int test(int logdepth, int argc, char *argv[])
{
	int dir[2], file[2], n;
	int *arr, idx;

	for(n = 0; n < 2; n++) {
		dir[n] = 0;
		file[n] = 0;
	}

	/* count the list of files, increase arrays by hit */
	for(n = 0; n < argc; n++) {
		arr =  NULL;
		if (strncmp(argv[n], "dir", 3) == 0)  { arr = dir;  idx = atoi(argv[n]+3); }
		if (strncmp(argv[n], "file", 4) == 0) { arr = file; idx = atoi(argv[n]+4); }
		if (arr == NULL) {
			logprintf(logdepth, "test fails: unknown existing file on the list: '%s'\n", argv[n]);
			return 0;
		}
		idx--;
		if ((idx < 0) || (idx > 1)) {
			logprintf(logdepth, "test fails: file name changed: '%s'\n", argv[n]);
			return 0;
		}
		arr[idx]++;
	}

	/* check if every item was found exactly once */
	for(n = 0; n < 2; n++) {
		if ((dir[n] != 1) || (file[n] != 1)) {
			logprintf(logdepth, "test fails: %s%d not found \n", dir[n] ? "file" : "dir", n);
			return 0;
		}
	}

	return 1;
}

static void filelist_extract(char *out, const char *dir, const char *method, int *argc, char ***argv)
{
	char *s, sep, *start, *end;
	int len, allocated = 0, count = 0;
	char **arr = NULL;
	const char *psep;

	psep = get("sys/path_sep");

	len = strlen(dir);

	/* uniform separator */
	if (*method == 'w') {
		/* if word splitting then convert newlines to spaces and convert tabs to spaces */
		for(s = out; *s != '\0'; s++) {
			if ((*s == '\n') || (*s == '\r') || (*s == '\t'))
				*s = ' ';
		}
		sep = ' ';
	}
	else {
		for(s = out; *s != '\0'; s++) {
			if (*s == '\r')
				*s = '\n';
		}
		sep = '\n';
	}

	start = out;
	while((s = str_chr(start, sep)) != NULL) {
		*s = '\0';
		if (strncmp(dir, start, len) == 0)
			start += len;
		while(*start == *psep)
			start++;

		if (*start != '\0') {
			end = str_chr(start, *psep);
			if (end != NULL)
				*end = '\0';

			/* add only if not the same as previous and exists */
			if ((!((count > 0) && (strcmp(arr[count - 1], start) == 0))) && (exists_in(dir, start))) {

				if (count >= allocated) {
					allocated = count + 32;
					arr = realloc(arr, sizeof(char *) * allocated);
				}
				arr[count] = strclone(start);
				count++;
			}
		}

		start = s+1;
		while(*start == sep) start++;
	}
	*argc = count;
	*argv = arr;
}

void filelist_free(int *argc, char ***argv)
{
	int n;

	if (*argv == NULL)
		return;

	for(n = 0; n < *argc; n++)
		free((*argv)[n]);
	free(*argv);
	*argc = 0;
}

static char *filelist_asmcmd(const char *dir, const char *list_cmd)
{
	char *cmd;

	cmd = malloc(strlen(dir) + strlen(list_cmd) + 32);
	sprintf(cmd, list_cmd, dir);
	return cmd;
}

static int try(int logdepth, const char *dir, const char *list_cmd, const char *method)
{
	char *cmd, *out, *dir_esc;
	int argc, res;
	char **argv;

	dir_esc = shell_escape_dup(dir);
	cmd = filelist_asmcmd(dir_esc, list_cmd);
	free(dir_esc);
	logprintf(logdepth, "trying '%s'...\n", cmd);

	run_shell(logdepth+1, cmd, &out);
	if (out != NULL) {
		filelist_extract(out, dir, method, &argc, &argv);
		res = test(logdepth+1, argc, argv);
		filelist_free(&argc, &argv);
		free(out);
	}

	if (res) {
		logprintf(logdepth+1, "Works.", cmd);
		put("/internal/filelist/cmd", list_cmd);
		put("/internal/filelist/method", method);
		report("OK ('%s' with %s split)\n", list_cmd, method);
	}

	free(cmd);
	return res;
}

int find_filelist(const char *name, int logdepth, int fatal)
{
	char *dir;
	char *old_cwd;
	int ret;

	old_cwd = strclone(db_cwd);
	db_cd("/host");

	require("fstools/mkdir", logdepth, fatal);
	require("fstools/rm", logdepth, fatal);


	report("Checking for filelist... ");
	logprintf(logdepth, "find_filelist: trying to find file listing...\n");
	logdepth++;


	dir = create_testdir(logdepth);
	if (dir == NULL) {
		report("Failed to creat sandbox\n");
		ret = 1;
		goto end;
	}

	if (
			try(logdepth, dir, "ls %s", "line") || /* should return one file name per line since the output is redirected */
			try(logdepth, dir, "ls -1 %s", "line") || /* try to force one file name per line */
			try(logdepth, dir, "ls --format=single-column %s", "line") || /* for gnu ls */
			try(logdepth, dir, "find %s", "line") || /* if ls fails, we try find */
	    try(logdepth, dir, "ls %s", "word") ||   /* if that fails too, ls may still have a list in multiple columns */
	    try(logdepth, dir, "dir %s", "word") ||  /* or we are on windows where we need to use dir maybe */
	    try(logdepth, dir, "echo %s/*", "word")) { /* or on a system without ls, dir or anything alike, but shell globbing may still work */

		destroy_testdir(logdepth, dir);
		ret = 0;
		goto end;
	}

	destroy_testdir(logdepth, dir);
	ret = 1;
	end:;
	db_cd(old_cwd);
	free(old_cwd);
	return ret;
}


void filelist(int logdepth, const char *dir, int *argc, char ***argv)
{
	const char *list_cmd, *method;
	char *cmd, *out, *dir_esc;
	char *old_cwd;

	old_cwd = strclone(db_cwd);
	db_cd("/host");

	/* make sure these are set to invalid for easier return in case we fail anywhere later */
	*argc = -1;
	*argv = NULL;

	if (!is_dir(dir))
		goto end;

	require("/internal/filelist/cmd", logdepth, 1);
	require("/internal/filelist/method", logdepth, 1);

	list_cmd = get("/internal/filelist/cmd");
	method   = get("/internal/filelist/method");

	dir_esc = shell_escape_dup(dir);
	cmd = filelist_asmcmd(dir_esc, list_cmd);
	free(dir_esc);
	run_shell(logdepth+1, cmd, &out);
	if (out != NULL) {
		filelist_extract(out, dir, method, argc, argv);
		logprintf(logdepth, "filelist: Getting list of files in %s\n", dir);
		free(out);
	}

	free(cmd);
	end:;
	db_cd(old_cwd);
	free(old_cwd);
}
