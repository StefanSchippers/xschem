/*
    scconfig - dependencies
    Copyright (C) 2009  Tibor Palinkas

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
#include <string.h>
#include "dep.h"
#include "db.h"
#include "log.h"
#include "libs.h"

typedef struct {
	int (*fn)(const char *name, int logdepth, int fatal);
} fn_wrap_t;


static ht_t *deps = NULL;

static const char *USER_WITHOUT = "";

/* find name_ and decide if it was a wildcard request;
   NOTE: there are requests and servers, both can be wildcard independently.
    - if a request ends with a / *, it is an explicit wildcard request (*wild=1)
    - if a request names a "directory" that is wildcard-server, that's a wildcard request (*wild=1)
    - else the request is a normal request (*wild=0).
   For normal requests, a required node was explicitly named; if that node is
   not created by the detection function, that's a failure. For wildcard
   requests we don't look for any specific node to be created.
   TODO: we may still check if at least the directory is created
   */
fn_wrap_t *get_wrap(const char *name_, int *wild, int *missing)
{
	fn_wrap_t *w;
	char *name, *sep, *tmp;
	int len, n;

	len = strlen(name_);
	*wild = name_[len-1] == '*';

	if (*wild) {
		char *pres;
		pres = malloc(len+16);
		memcpy(pres, name_, len-1);
		strcpy(pres+len-1, "presents");
		*missing = get(pres) == NULL;
		if (*missing) { /* if there's no /presents, it may be a non-directory node with an actual non-empty string value */
			const char *val;
			pres[len-2] = '\0';
			val = get(pres);
			if (val != NULL)
				*missing = !strlen(val);
		}
		free(pres);
		if (!(*missing)) /* already detected, won't be detected again */
			return NULL;
	}
	*missing = 1;

	/* check for global --without disable */
	tmp = str_concat("", "/arg/without", db_cwd, "/", name_, NULL);
	sep = tmp + strlen(tmp) - 1;
	for(n = 0; n < 4; n++) {
		const char *d = get(tmp);
		if (sep < tmp+14)
			break;
		if (istrue(d)) {
			free(tmp);
			return (fn_wrap_t *)USER_WITHOUT;
		}
		while(*sep != '/')
			sep--;
		*sep = '\0';
	}
	free(tmp);

	/* try full match first */
	w = ht_get(deps, name_);
	if (w != NULL)
		return w;


	/* try substituting the last part of the path with * for wildcard matches */
	name = malloc(len+3);       /* worst case: ends in a / and we need to append *\0; allocate a bit more */
	memcpy(name, name_, len+1); /* make a copy we can modify */
	if (name[len-1] != '/') {
		name[len] = '/';            /* append a / - if name_ was a "directory", this will result in name/ *  */
		name[len+1] = '\0';
	}

	*wild = 1; /* if we append a / *, then it's a wildcard request */
	for(;;) {
		sep = str_rchr(name, '/');
		if (sep == NULL)
			goto error;
		sep[1] = '*';
		sep[2] = '\0';
		w = ht_get(deps, name);
		if (w != NULL) {
			free(name);
			return w;
		}
		*sep = '\0';
		*wild = 0; /* cutting back the second layer - not wildcard request anymore, but a request to a specific node served by a wildcard */
	}

	/* no match, exit with error  */
	error:;
	*wild = 0;
	free(name);
	return NULL;
}


int require(const char *name, int logdepth, int fatal)
{
	fn_wrap_t *w;
	int wild, missing;

	if (get(name) == NULL) {
		w = get_wrap(name, &wild, &missing);
		if (w == (fn_wrap_t *)USER_WITHOUT) {
			if (fatal) {
				error("Node %s is required by the software but disabled by the user using --without\n", name);
				abort();
			}
			else {
				logprintf(logdepth, "(disabled using --without)");
				return 1;
			}
		}
		if (!missing)
			return 0;
		if ((w == NULL) || (w->fn == NULL)) {
			error("Node %s is required but I don't know how to detect it.\n", name);
			abort();
		}

		logprintf(logdepth, "(Required node: '%s')\n", name);
		if (w->fn(name, logdepth+1, fatal) != 0) {
			if (fatal) {
				error("Node %s is required but provided detection callback fails to find that feature on that system.\n", name);
				abort();
			}
			else {
				logprintf(logdepth, "(Feature not found, but it is not fatal)");
				return 1;
			}
		}
		if ((!wild) && (get(name) == NULL)) {
			error("Node %s is required but provided detection callback didn't create it (looks like an internal error in scconfig).  (db_cwd='%s')\n", name, db_cwd);
			abort();
		}
	}
	return 0;
}

const char *dep_add(const char *name, int (*finder)(const char *name, int logdepth, int fatal))
{
	fn_wrap_t *w;
	w = malloc(sizeof(fn_wrap_t));
	w->fn = finder;
	return ht_set(deps, name, w);
}

int asked_for(const char *cando, const char *needtodo)
{
	int len;

	/* foo/bar/baz matches /foo/bar/baz */
	if (strcmp(cando, needtodo) == 0)
		goto yes;

	len = strlen(needtodo);
	if (len == 0)
		return 0;

	/* foo/bar/baz matches /foo/bar/ * */
	if ((needtodo[len-1] == '*') && (strncmp(cando, needtodo, len-1) == 0))
		goto yes;

	return 0;


	yes:; /* asked for it, but have to see if it's already detected */
	if (get(cando) != NULL)
		return 0;

	return 1;
}

int is_dep_wild(const char *path)
{
	int len = strlen(path);
	if (len == 0)
		return 0;
	return (path[len-1] == '*');
}

const char *det_list_target(const char *path)
{
	const char *res;

	if (path == NULL)
		goto unk;

	res = strrchr(path, '/');
	if (res == NULL)
		goto unk;

	return res + 1;
unk:;
	return "<unknown>";
}


void dep_init(void)
{
	deps = ht_resize(ht_alloc(0), 128);
}

void dep_uninit(void)
{
	ht_free(deps);
}

int is_dep_known(const char *name)
{
	return (ht_get(deps, name) != NULL);
}

void require_all(int fatal)
{
	ht_entry_t *h;

	for(h = ht_first(deps); h != NULL; h = ht_next(deps, h))
		require(h->key, 0, fatal);
}
