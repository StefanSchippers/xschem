/*
    scconfig - non-standard string manipulation routines
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
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

char *strclone(const char *str)
{
	int l;
	char *ret;

	if (str == NULL)
		return NULL;

	l   = strlen(str)+1;
	ret = malloc(l);
	memcpy(ret, str, l);
	return ret;
}

#define SPACE(c) (((c) == '\r') || ((c) == '\n') || ((c) == '\t') || ((c) == ' '))

char *trim_left(char *str)
{
	while(SPACE(*str)) str++;
	return str;
}

char *trim_right(char *str)
{
	char *end;

	end = str + strlen(str) - 1;
	while((end >= str) && SPACE(*end)) { *end = '\0'; end--; }
	return str;
}

char *strip(char *str)
{
	return trim_left(trim_right(str));
}

char *str_chr(char *str, char c)
{
	char *s;

	for(s = str; *s != '\0'; s++)
		if (*s == c)
			return s;
	return NULL;
}

char *str_rchr(char *str, char c)
{
	char *s, *last;

	last = NULL;
	for(s = str; *s != '\0'; s++)
		if (*s == c)
			last = s;
	return last;
}

char *str_subsn(const char *str)
{
	char *out, *o;
	const char *i;
	if (str == NULL)
		return strclone("");
	o = out = malloc(strlen(str)+1);
	for(i = str; *i != '\0'; i++, o++) {
		if ((i[0] == '\\') && (i[1] == 'n')) {
			i++;
			*o = '\n';
		}
		else
			*o = *i;
	}
	*o = '\0';
	return out;
}

char *str_concat(const char *sep, ...)
{
#	define CONCAT_MAX 64
	int len[CONCAT_MAX];
	const char *str[CONCAT_MAX];
	int n, v, sum, sl;
	char *out, *o;
	va_list ap;
	va_start(ap, sep);

	if (sep == NULL)
		sep = "";

	/* load all strings into an array, measure their lengths */
	sum = 0;
	for(v = 0; ;v++) {
		if (v >= CONCAT_MAX) {
			fprintf(stderr, "Internal error: str_concat got more strings than CONCAT_MAX\n");
			abort();
		}
		str[v] = va_arg(ap, const char *);
		if (str[v] == NULL) {
			len[v] = 0;
			break;
		}
		len[v] = strlen(str[v]);
		sum += len[v];
	}

	/* first string is NULL; return a new allocation that is a simple \0, empty string to avoid a nasty corner case */
	if (sum == 0)
		return calloc(1, 1);

	sl = strlen(sep);
	sum += (v-1) * sl + 1; /* + a sep between each two strings and a terminator at the end */
	o = out = malloc(sum);
	for(n = 0; n < v; n++) {
		if ((n > 0) && (sl > 0)) {
			memcpy(o, sep, sl);
			o += sl;
		}
		if (len[n] > 0) {
			memcpy(o, str[n], len[n]);
			o += len[n];
		}
	}
	*o = '\0';
	va_end(ap);
	return out;
}

char *esc_interpret(const char *str)
{
	char *out, *si, *so;

	out = strclone(str);
	/* replace (interpret) \ sequences in seq */
	for(si = so = out; *si != '\0'; si++,so++) {
		if (si[0] == '\\') {
			switch(si[1]) {
				case 'n': *so = '\n'; break;
				case 't': *so = '\t'; break;
				case 's': *so = ' '; break;
				case '\\': *so = '\\'; break;
			}
			si++;
		}
		else
			*so = *si;
	}
	*so = '\0';
	return out;
}

int chr_inset(char c, const char *set)
{
	while (*set != '\0') {
		if (c == *set)
			return 1;
		set++;
	}
	return 0;
}

