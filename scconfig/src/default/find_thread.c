/*
    scconfig - detection of standard library features
    Copyright (C) 2009,2017  Tibor Palinkas

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
#include "db.h"
#include "dep.h"

int find_lib_lpthread(const char *name, int logdepth, int fatal)
{
	const char *lpthread;
	char *s;
	int ret = 0;

	char *test_c_recursive =
		NL "#define _GNU_SOURCE 1 /* Needed for recursive thread-locking */"
		NL "#include <pthread.h>"
		NL "pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;"
		NL "int main() {"
		NL "	pthread_attr_t a;"
		NL "	if (pthread_attr_init(&a) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL ;

	char *test_c_simple =
		NL "#include <pthread.h>"
		NL "int main() {"
		NL "	pthread_attr_t a;"
		NL "	if (pthread_attr_init(&a) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL ;

	require("cc/cc", logdepth, fatal);

	report("Checking for -lpthread... ");
	logprintf(logdepth, "find_lib_lpthread: trying to find lpthread...\n");
	logdepth++;

	lpthread = get("/arg/libs/lpthread");

	if (lpthread != NULL) {
		put("libs/lpthread", lpthread);
		report("User provided... ");
		s = strclone(lpthread);
	}
	else
		s = strclone("-lpthread");

	if (try_icl(logdepth, NULL, test_c_recursive, NULL, NULL, s)) {
		put("libs/lpthread", s);
		put("libs/lpthread-recursive", strue);
		report("OK, recursive (%s)\n", s);
	}
	else if (try_icl(logdepth, NULL, test_c_simple, NULL, NULL, s)) {
		put("libs/lpthread", s);
		put("libs/lpthread-recursive", sfalse);
		report("OK, NOT RECURSIVE (%s)\n", s);
	}
	else
		ret = 1;

	free(s);
	return ret;
}

int find_thread_semget(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	int semid = semget(IPC_PRIVATE, 1, IPC_CREAT);"
		NL "	if (semid < 0) return 0;"
		NL "	if(semctl(semid, 0, IPC_RMID) < 0) return 0;"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "thread/semget";
	char **inc, *incs[] = {"#include <sys/types.h>\n#include <sys/ipc.h>\n#include <sys/sem.h>", "#include <sys/types.h>\n#include <sys/ipc.h>", "#include <sys/types.h>\n#include <sys/sem.h>", NULL};

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for semget... ");
	logprintf(logdepth, "find_semget:\n");
	logdepth++;

	for(inc = incs; *inc != NULL; inc++)
		if (try_icl(logdepth, node, test_c, *inc, NULL, NULL) != 0)
			return 0;

	return try_fail(logdepth, node);
}

int find_thread_pthread_create(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "void* test_thread(void* param)"
		NL "{"
		NL "	return NULL;"
		NL "}"
		NL "int main()"
		NL "{"
		NL "	pthread_t pt;"
		NL "	if (pthread_create(&pt, NULL, test_thread, NULL) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "thread/pthread_create";
	char **inc, *incs[] = {"#include <pthread.h>", "#include <pthread.h>\n#include <signal.h>", NULL};
	const char *lpthread;
	char* s;

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for pthread_create... ");
	logprintf(logdepth, "find_pthread_create:\n");
	logdepth++;

	lpthread = get("/arg/libs/lpthread");

	if (lpthread != NULL) {
		report("User provided... ");
		s = strclone(lpthread);
	}
	else
		s = strclone("-lpthread");

	for(inc = incs; *inc != NULL; inc++)
		if (try_icl(logdepth, node, test_c, *inc, NULL, s) != 0) {
			free(s);
			return 0;
		}

	free(s);
	return try_fail(logdepth, node);
}

int find_thread_CreateSemaphore(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	if (CreateSemaphore(NULL, 1, 1, NULL))"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "thread/CreateSemaphore";

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for CreateSemaphore... ");
	logprintf(logdepth, "find_thread_CreateSemaphore:\n");
	logdepth++;

	if (try_icl(logdepth, node, test_c, "#include <windows.h>", NULL, NULL) != 0)
		return 0;

	return try_fail(logdepth, node);
}

int find_thread_CreateThread(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "DWORD WINAPI test_thread(void* param)"
		NL "{"
		NL "	return 0;"
		NL "}"
		NL "int main()"
		NL "{"
		NL "	if (CreateThread(NULL, 0, test_thread, NULL, 0, NULL))"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "thread/CreateThread";

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for CreateThread... ");
	logprintf(logdepth, "find_thread_CreateThread:\n");
	logdepth++;

	if (try_icl(logdepth, node, test_c, "#include <windows.h>", NULL, NULL) != 0)
		return 0;

	return try_fail(logdepth, node);
}
