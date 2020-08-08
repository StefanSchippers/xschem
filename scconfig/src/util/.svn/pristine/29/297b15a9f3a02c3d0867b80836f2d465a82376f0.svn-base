/*
    scconfig - ls built on dirent
    Copyright (C) 2009  Szabolcs Nagy

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

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
static int ls(char *arg) {
	WIN32_FIND_DATA fd;
	HANDLE h;
	char p[MAX_PATH];
	int i;

	for (i = 0; i+2 < MAX_PATH; i++)
		if (arg[i])
			p[i] = arg[i];
	if (i+2 < MAX_PATH) {
		p[i] = '\\';
		p[i+1] = '*';
		p[i+2] = 0;
	} else
		return -1;

	h = FindFirstFile(p, &fd);
	if (h == INVALID_HANDLE_VALUE)
		return -1;
	printf("%s\n", fd.cFileName);

	while (FindNextFile(h, &fd) != 0);
		printf("%s\n", fd.cFileName);

	FindClose(h);
	return 0;
}
#else
#include <dirent.h>
static int ls(char *arg) {
	DIR *dirp;
	struct dirent *dp;

	if ((dirp = opendir(arg)) == 0)
		return -1;

	while ((dp = readdir(dirp)) != 0)
		printf("%s\n", dp->d_name);

	closedir(dirp);
	return 0;
}
#endif

int main(int argc, char *argv[]) {
	int i;
	char *p = ".";

	if (argc > 1)
		p = argv[1];
	return ls(p);
}

