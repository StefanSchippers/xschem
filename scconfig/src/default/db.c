/*
    scconfig - database
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
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include "db.h"
#include "log.h"
#include "libs.h"

ht_t *DBs = NULL;
char *db_cwd = NULL;

void append(const char *key, const char *value)
{
	const char *orig;
	char *new;
	int l1, l2;

	assert(key   != NULL);
	assert(value != NULL);
	if (*value == '\0')
		return;

	orig = get(key);
	if (orig == NULL) {
		put(key, value);
		return;
	}

	l1   = strlen(orig);
	l2   = strlen(value);
	new  = malloc(l1 + l2 + 1);
	memcpy(new,      orig,  l1);
	memcpy(new + l1, value, l2);
	new[l1+l2] = '\0';
	put(key, new);
}


static const char *db_split_path(const char *key, ht_t **ht, char **fld)
{
	size_t fld_len;
	const char *path;
	char first_level_dir[32];

	path = str_chr((char *)(key+1), '/');
	assert(path != NULL);
	fld_len = path - key;
	path++;
	if (*path == '\0') {
		*ht = NULL;
		if (fld != NULL)
			*fld = NULL;
		return NULL;
	}
	assert(fld_len < sizeof(first_level_dir));
	strncpy(first_level_dir, key, fld_len);
	first_level_dir[fld_len] = '\0';
	*ht = ht_get(DBs, first_level_dir+1);
	if (fld != NULL)
		*fld = first_level_dir;
	return path;
}

static void export_qs(FILE *f, const char *s)
{
	fputc('"', f);
	for(;*s != '\0';s++) {
		switch(*s) {
			case '"':  fputc('\\', f); fputc('"', f); break;
			case '\n': fputc('\\', f); fputc('n', f); break;
			case '\r': fputc('\\', f); fputc('r', f); break;
			case '\t': fputc('\\', f); fputc('t', f); break;
			default: fputc(*s, f);
		}
	}

	fputc('"', f);
	fputc('\n', f);
}

static int needs_quote(const char *s) {
	for(; *s != '\0'; s++)
		if ((*s < 32) || (*s > 126) || (*s == '"')) return 1;
	return 0;
}

int export_(FILE *f, int export_empty, ht_t *table, const char *fld)
{
	ht_entry_t *h;

	for(h = ht_first(table); h != NULL; h = ht_next(table, h))
		if (export_empty || ((h->value != NULL) && (*(char *)h->value != '\0'))) {
			fprintf(f, "/%s/%s=", fld, h->key);
			if (h->value != NULL) {
				if (needs_quote((char *)h->value))
					export_qs(f, (const char *)h->value);
				else
					fprintf(f, "%s\n", (char *)h->value);
			}
			else
				fprintf(f, "\n");
		}
	return 0;
}

int export(const char *fn, int export_empty, const char *root)
{
	FILE *f;
	int ret = 0;
/*	ht_t *table; */
	ht_entry_t *h;

	if (fn != NULL) {
		f = fopen(fn, "w");
		if (f == NULL)
			return -1;
	}
	else
		f = stdout;

	if ((root == NULL) || ((root[0] == '/') && (root[1] == '\0'))) {
		/* export all directories */
		for(h = ht_first(DBs); h != NULL; h = ht_next(DBs, h))
			ret += export_(f, export_empty, h->value, h->key);
	}
	else {
		error("not yet implemented\n");
		abort();
		/* db_split_path(); */
	}

	if (f != stdout)
		fclose(f);

	return ret;
}

/* append a single character, grow the buffer as needed */
#define qappend(chr) \
do { \
	if (used >= alloced) { \
		alloced += 256; \
		res = realloc(res, alloced); \
	} \
	res[used] = chr; \
	used++; \
} while(0)

/* read until end of quote and interpret backslash sequences if do_esc is non-zero */
static char *readq(FILE *f, char *str, long strmax, int quote, int do_esc, int *num_lines, const char *fn)
{
	int bs = 0;
	long used = 0, alloced = 0;
	char *res = NULL, *s;

	for(;;) {
		for(s = str; *s != '\0'; s++) {
			if (*s == '\n') (*num_lines)++;

			if (bs) { /* character escaped by backslash */
				switch(*s) {
					case '\\': qappend('\\'); break;
					case 'n': qappend('\n'); break;
					case 'r': qappend('\r'); break;
					case 't': qappend('\t'); break;
					default: qappend(*s); break;
				}
				bs = 0;
			}
			else if (*s == quote) { /* end */
				qappend('\0');
				if ((s[1] != '\r') && (s[1] != '\n') && (s[1] != '\0'))
					fprintf(stderr, "Warning: trailing text after quote ignored in %s:%d\n", fn, (*num_lines)+1);
				return res;
			}
			else if (do_esc && (*s == '\\')) bs = 1; /* backslash start */
			else qappend(*s); /* plain character */
		}

		/* get the next chunk */
		fgets(str, strmax, f);
	}

	return NULL; /* can't get here */
}

int import(const char *fn)
{
	char line[1024];
	char *key, *value, *nl, *slash;
	int num_records, num_lines;
	FILE *f;

	f = fopen(fn, "r");
	if (f == NULL)
		return -1;

	for(num_records = 0, num_lines = 0; !feof(f); num_lines++) {
		*line = '\0';
		fgets(line, sizeof(line) - 1, f);
		if ((*line != '#') && (*line != '\n') && (*line != '\r') && (*line != '\0')) {
			int quote, do_esc=0;
			key = line;
			value = str_chr(key, '=');
			if (value == NULL) {
				error("Error importing: missing '=' in line %d in file %s.\n", num_lines, fn);
				abort();
			}
			num_records++;
			*value = '\0';
			value++;
			if (*value == '"') {
				quote=*value;
				value++;
				do_esc=1;
			}
			else if (*value == '\'') {
				quote=*value;
				value++;
			}
			else
				quote=0;

			if (!quote) {
				nl = str_chr(value, '\n');
				if (nl != NULL)
					*nl = '\0';
			}
			else
				value = readq(f, value, sizeof(line) - (value - line) - 4, quote, do_esc, &num_lines, fn);

			slash = str_chr(key+1, '/');
			if (slash == NULL) {
				error("Error importing: no directory name for %s.\n", key);
				abort();
			}
			*slash = '\0';
			db_mkdir(key);
			*slash = '/';
			put(key, value);
			logprintf(0, "(Import from '%s': '%s'='%s')\n", fn, key, value);
			if (quote)
				free(value);
		}
	}

	fclose(f);
	return num_records;
}

int import_args(const char *key, const char *fn)
{
	(void) key; /* suppress compiler warnings for unused key; needed because function pointers to this function from arg.c */
	db_mkdir("/target");
	db_mkdir("/host");

	return import(fn) < 0;
}


static const char *db_get(const char *key)
{
	const char *path;
	ht_t *ht;

	path = db_split_path(key, &ht, NULL);
	if (ht == NULL)
		return NULL;
	return ht_get(ht, path);
}

static const char *db_put(const char *key, const char *value)
{
	const char *path;
	ht_t *ht;

	path = db_split_path(key, &ht, NULL);
	if (ht == NULL) {
		error("db_put: can't find top level hash for '%s'\n", key);
		abort();
	}
	return ht_set(ht, path, (void *)value);
}

#define assamble_path \
	assert(strlen(key) + strlen(db_cwd) < sizeof(tmp)-1); \
	sprintf(tmp, "%s/%s", db_cwd, key);

const char *get(const char *key)
{
	char tmp[256];

	if (*key == '/')
		return db_get(key);
	assamble_path;
	return db_get(tmp);
}

const char *put(const char *key, const char *value)
{
	char tmp[256];

	if (*key == '/')
		return db_put(key, value);
	assamble_path;
	return db_put(tmp, value);
}

void db_init(void)
{
	DBs = ht_resize(ht_alloc(0), 16);
}

void db_uninit(void)
{
	ht_entry_t *h;
	ht_t *dir;

	for(h = ht_first(DBs); h != NULL; h = ht_next(DBs, h)) {
		dir = h->value;
		dir->refcount--;
		if (dir->refcount == 0)
			ht_free(dir);
	}
	ht_free(DBs);
	if (db_cwd != NULL)
		free(db_cwd);
/* Just in case someone calls db_init again... */
	db_cwd = NULL;
	DBs    = NULL;
}

void db_cd(const char *path)
{
	assert(*path == '/');
	if (db_cwd != NULL)
		free(db_cwd);
	db_cwd = strclone(path);
}

void db_mkdir(const char *path)
{
	ht_t *ht, *target;
	assert(*path == '/');
	target = ht_get(DBs, path+1);
	if (target == NULL) {
		ht = ht_resize(ht_alloc(1), 256);
		ht_set(DBs, path+1, ht);
	}
}

void db_rmdir(const char *path)
{
	ht_t *ht;
	assert(*path == '/');
	ht = ht_get(DBs, path+1);
	if (ht == NULL)
		return;
	ht_del(DBs, path+1);
/*	ht_free(ht); */
}

void db_link(const char *existing, const char *new)
{
	ht_t *ht;

	assert(*new == '/');
	ht = ht_get(DBs, existing+1);
	assert(ht != NULL);
	ht_set(DBs, new+1, ht);
	ht->refcount++;
}

char *concat_nodes(const char *prefix, ...)
{
	char *buff;
	const char *node, *value;
	int allocated = 256, len, totallen;

	va_list ap;
	va_start(ap, prefix);
	buff = malloc(allocated);
	if (prefix != NULL) {
		strcpy(buff, prefix);
		totallen = strlen(prefix);
		buff[totallen] = ' ';
		totallen++;
	}
	else
		totallen = 0;

	while((node = va_arg(ap, const char *)) != NULL) {
		value = get(node);
		if (value != NULL) {
			len = strlen(value);
			if (totallen + len >= allocated) {
				allocated = totallen + len + 256;
				buff = realloc(buff, allocated);
			}
			memcpy(buff + totallen, value, len);
			totallen += len;
			buff[totallen] = ' ';
			totallen++;

			buff[totallen] = '\0';
		}
	}

	buff[totallen - 1] = '\0';
	va_end(ap);
	return buff;
}

int node_istrue(const char *key)
{
	const char *s = get(key);
	if (s == NULL)
		return 0;
	return istrue(s);
}
