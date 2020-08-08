/*
    scconfig - library to explore the source tree
    Copyright (C) 2015  Tibor Palinkas

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


char *svn_info(int logdepth, const char *dir, const char *key)
{
	char *cmd, *stdo = NULL;
	char *res = NULL;
	int keylen = strlen(key);

	cmd = str_concat(" ", "svn info", dir, NULL);
	if (run_shell(logdepth, cmd, &stdo) == 0) {
		char *line, *nline;

		/* check key against each line */
		for(line = stdo; line != NULL; line = nline) {
			/* split line */
			nline = strpbrk(line, "\r\n");
			if (nline != NULL) {
				*nline = '\0';
				nline++;
				while((*nline == '\n') || (*nline == '\r')) nline++;
			}

			/* compare key */
			if (strncmp(line, key, keylen) == 0) {
				char *val;

				/* extract value */
				val = strchr(line, ':');
				if (val != NULL) {
					val++;
					while((*val == ' ') || (*val == '\t')) val++;
				}
				else
					val = line;

				/* produce output */
				res = strclone(val);
				goto found;
			}
		}
	}

	found:;
	if (stdo != NULL)
		free(stdo);
	free(cmd);
	return res;
}
