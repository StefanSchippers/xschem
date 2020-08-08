/*
    scconfig - library for making includes on a list unique
    Copyright (C) 2012, 2017  Tibor Palinkas
    Copyright (C) 2017  Aron Barath

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
#include "libs.h"
#include "db.h"
#include "regex.h"

#define grow \
	if (used >= alloced) { \
		alloced += 16; \
		list = realloc(list, alloced * sizeof(char *)); \
	}

char **uniq_inc_arr(const char *includes, int indirect, const char *sep_, int *numlines)
{
	char *node, *next, *cw, *nw, *snode, *orig_node;
	char *sep;
	char **list;
	int alloced, used, n;

	orig_node = strclone(includes);
	node = orig_node;
	if (sep_ == NULL)
		sep = strclone("\r\n");
	else
		sep = strclone(sep_);

	/* reset list */
	alloced = used = 0;
	list = NULL;
	/* take arguments one by one */
	while(node != NULL) {
		if (indirect) {
			while((*node == ' ') || (*node == '\t')) node++;
			next = strpbrk(node, " \t");
		}
		else {
			for(;;) {
				next = strpbrk(node, sep);
				if ((next > node) || (next == NULL))
					break;
				node = next+1;
			}
		}
		if (next != NULL) {
			*next = '\0';
			next++;
		}
		if (indirect)
			snode = str_subsn(get(node));
		else
			snode = node;
		cw = snode;
		/* split node value (s) by sep */
/*		fprintf(stderr, "nodename=%s snode=%s next=%s\n", node, snode, next);*/
		while(cw != NULL) {
			nw = strpbrk(cw, sep);
			if (nw != NULL) {
				*nw = '\0';
				nw++;
			}

			if (*cw != '\0') {
				/* try to find cw in the existing list - this is a slow linear search for now */
				for(n = 0; n < used; n++) {
					if (strcmp(list[n], cw) == 0)
						goto already_on_list;
				}
				/* not found, append */
				grow;
				list[used] = strclone(cw);
				used++;
			}
			already_on_list:;
			cw = nw;
		}
		if (indirect)
			free(snode);
		node = next;
	}
	grow;
	list[used] = NULL;
	if (numlines != NULL)
		*numlines = used;

	free(orig_node);
	free(sep);

	return list;
}

void uniq_inc_free(char **arr)
{
	char **s;
	for(s = arr; *s != NULL; s++)
		free(*s);
	free(arr);
}

static int uniq_inc_str_cmp(const void *a_, const void *b_)
{
	char **a = (char **)a_, **b = (char **)b_;
	return strcmp(*a, *b);
}

static void uniq_inc_assemble_normal(char* const ret, int numelem, char **arr, const char *osep, const int oseplen)
{
	char *end;
	int len;

	for(end = ret; 0 < numelem; ++arr, --numelem) {
		if (!*arr)
			continue;

		len = strlen(*arr);
		memcpy(end, *arr, len);
		end += len;
		memcpy(end, osep, oseplen);
		end += oseplen;
		free(*arr);
	}

	*end = '\0';
}

static void uniq_inc_assemble_groups(char* const ret, int numelem, char **arr, const char *osep, const int oseplen, int eren, char **eres)
{
	char *end = ret;
	int erei, ndx, len;

	/* re_comp() uses a global variable to store the compiler regex! */

	for (erei = 0; erei < eren; ++erei) {
		if (re_comp(eres[erei]))
			abort();

		for (ndx = 0; ndx < numelem; ++ndx) {
			if (!arr[ndx])
				continue;

			if (re_exec(arr[ndx])) {
				len = strlen(arr[ndx]);
				memcpy(end, arr[ndx], len);
				end += len;
				memcpy(end, osep, oseplen);
				end += oseplen;
				free(arr[ndx]);
				arr[ndx] = NULL;
			}
		}
	}

	/* collect remaining elements */

	uniq_inc_assemble_normal(end, numelem, arr, osep, oseplen);
}

char *uniq_inc_str(const char *includes, const char *isep, const char *osep, int sort, int eren, char **eres)
{
	char **arr, **s, *ret;
	int len, numelem, oseplen;

	/* split and uniq */

	oseplen = strlen(osep);
	arr = uniq_inc_arr(includes, 0, isep, NULL);

	/* calculate the required amount of memory */

	len = 4; /* safety margin, for terminator \0, etc. */
	numelem = 0;
	for(s = arr; *s != NULL; s++) {
		len += strlen(*s) + oseplen + 1;
		numelem++;
	}

	/* sort if needed */

	if (sort)
		qsort(arr, numelem, sizeof(char *), uniq_inc_str_cmp);

	/* allocate memory to assemble into */

	ret = malloc(len);

	/* assemble the output */

	if (0>=eren)
		uniq_inc_assemble_normal(ret, numelem, arr, osep, oseplen);
	else
		uniq_inc_assemble_groups(ret, numelem, arr, osep, oseplen, eren, eres);

	/* done */

	free(arr);
	return ret;
}

char *order_inc_str(const char *includes, const char *isep, const char *word1, int dir, const char *word2)
{
	const char *s, *next, *pre, *mid, *post;
	char *out, *end;
	long w1o = -1, w2o = -1;
	long w1len = strlen(word1), w2len = strlen(word2), tlen;
	long pre_len, mid_len, post_len;

	if (dir == 0)
		return NULL;

	if ((w1len == 0) || (w2len == 0))
		return strclone(includes);

	if ((w1len == w2len) && (strcmp(word1, word2) == 0))
		return strclone(includes);

	/* search the starting offset of the first occurence of word1 and word2 */
	for(s = includes; (s != NULL) && ((w1o < 0) || (w2o < 0)); s = next) {
		next = strpbrk(s, isep);
		if (next == NULL)
			tlen = strlen(s);
		else
			tlen = next-s;

		if ((w1o < 0) && (w1len == tlen) && (memcmp(s, word1, tlen) == 0))
			w1o = s - includes;
		if ((w2o < 0) && (w2len == tlen) && (memcmp(s, word2, tlen) == 0))
			w2o = s - includes;

		if (next != NULL)
			next += strspn(next, isep);
	}

	/* one of the words is not on the list, the list is ordered */
	if ((w1o < 0) || (w2o < 0))
		return strclone(includes);

	/* both words are not on the list, but the list is ordered */
	if (((dir < 0) && (w1o < w2o)) || ((dir > 0) && (w1o > w2o)))
		return strclone(includes);

	/* split up the input at word1 and word2 */
	tlen = strlen(includes);
	if (dir < 0) { /* input is: 'pre w2 mid w1 post', goal is mowing w1 before w2 */
		pre = includes;
		pre_len = w2o;
		mid = includes + w2o + w2len + 1;
		mid_len = (includes + w1o) - mid;
		post = includes + w1o + w1len + 1;
		post_len = (includes + tlen) - post + 1;
	}
	else { /* input is: 'pre w1 mid w2 post' goal is moving w1 after w2*/
		pre = includes;
		pre_len = w1o;
		mid = includes + w1o + w1len + 1;
		mid_len = (includes + w2o) - mid;
		post = includes + w2o + w2len + 1;
		post_len = (includes + tlen) - post + 1;
	}

	/* truncate trailing separator, if present */
	if ((pre_len > 0) && (strchr(isep, pre[pre_len-1])))
		pre_len--;

	if ((mid_len > 0) && (strchr(isep, mid[mid_len-1])))
		mid_len--;

	if ((post_len > 0) && (strchr(isep, post[mid_len-1])))
		post_len--;

	/* allocate extra space for a trailing separator and/or \0 */
	end = out = malloc(tlen+2);

	/* build the string by appending the parts */
#define append(str, len) \
	if (len > 0) { \
		memcpy(end, str, len); \
		end += len; \
		*end = *isep; \
		end++; \
	}

	append(pre, pre_len);
	if (dir < 0) {
		append(word1, w1len);
		append(word2, w2len);
	}
	append(mid, mid_len);
	if (dir > 0) {
		append(word2, w2len);
		append(word1, w1len);
	}
	append(post, post_len);

#undef append

	/* replace the last separator with \0 or just add a \0 at the end */
	if ((end > out) && (strchr(isep, end[-1])))
		end[-1] = '\0';
	else
		end[0] = '\0';
	return out;
}
