/*
    scconfig - default way to handle custom args (save them in an array)
    Copyright (C) 2016  Tibor Palinkas

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
#include "libs.h"
#include "log.h"
#include "hooks.h"
#include "main_custom_args.h"

char *custom_reqs[MAX_CUSTOM_REQS];
int num_custom_reqs = 0;

int custom_arg(const char *key, const char *value)
{
	if (hook_custom_arg(key, value))
		return 1;
	if (strcmp(key, "detect") == 0) {
		printf("Will detect: %s\n", value);
		if (num_custom_reqs >= MAX_CUSTOM_REQS) {
			report("Too many custom reqs from the command line, exiting\n");
			exit(1);
		}
		custom_reqs[num_custom_reqs] = strclone(value);
		num_custom_reqs++;
		return 1;
	}
	return 0;
}
