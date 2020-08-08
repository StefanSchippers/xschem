/*
    scconfig - detection of standard library features (processes)
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
#include "db.h"
#include "dep.h"

int find_proc__spawnvp(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stdlib.h>"
		NL "int main() {"
		NL "	const char *a[3] = {\"/c\", \"echo OK\", NULL};"
		NL "	_spawnvp(_P_WAIT, \"cmd\", a);"
		NL "	return 0;"
		NL "}"
		NL ;

	require("cc/cc", logdepth, fatal);

	report("Checking for _spawnvp... ");
	logprintf(logdepth, "find_proc__spawnvp: trying to find _spawnvp...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/proc/_spawnvp", test_c, "#include <process.h>", NULL, NULL)) return 0;

	return try_fail(logdepth, "libs/proc/_spawnvp");
}

int find_proc_CreateProcessA(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/proc/CreateProcessA";
	char *test_c =
		NL "#include <stdlib.h>"
		NL "char buf[2000];"
		NL "int main() {"
		NL "	const char *cmd;"
		NL "	STARTUPINFOA si;"
		NL "	PROCESS_INFORMATION pi;"
		NL "	cmd = getenv(\"COMSPEC\");"
		NL "	if (cmd == NULL) { /* fallback to guessing */"
		NL "		UINT len = GetWindowsDirectoryA(buf, sizeof(buf));"
		NL "		strcpy(buf+len, \"\\\\system32\\\\cmd.exe\");"
		NL "		cmd = buf;"
		NL "	}"
		NL "	memset(&si, 0, sizeof(si)); si.cb = sizeof(si);"
		NL "	if (!CreateProcessA(cmd, \"/c \\\"echo OK\\\"\","
		NL "		NULL, NULL, TRUE,"
		NL "		0, NULL, NULL, &si, &pi))"
		NL "		return 1;"
		NL "	if (WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_OBJECT_0)"
		NL "		abort();"
		NL "	return 0;"
		NL "}"
		NL ;

	require("cc/cc", logdepth, fatal);

	report("Checking for CreateProcessA... ");
	logprintf(logdepth, "find_proc_CreateProcessA: trying to find CreateProcessA...\n");
	logdepth++;

	if (try_icl(logdepth, key, test_c, "#include <windows.h>", NULL, NULL)) return 0;

	return try_fail(logdepth, key);
}



int find_proc_fork(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	if (fork() == 0) { return 0; }"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL ;

	/* NOTE: can't print OK from the child process because of a possible race
	   with the parent immediately exiting without wait(). */

	require("cc/cc", logdepth, fatal);

	report("Checking for fork... ");
	logprintf(logdepth, "find_proc_fork: trying to find fork...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/proc/fork", test_c, "#include <unistd.h>", NULL, NULL)) return 0;

	return try_fail(logdepth, "libs/proc/fork");
}


int find_proc_wait(const char *name, int logdepth, int fatal)
{
	char *inc;
	const char *inc1;
	char test_c[1024];
	char *test_c_in =
		NL "%s\n"
		NL "#include <stdlib.h>"
		NL "#include <stdio.h>"
		NL "int main() {"
		NL "	int st = 0;"
		NL "	if (fork() == 0) {"
		NL "		printf(\"O\");"
		NL "		return 42;"
		NL "	}"
		NL "	wait(&st);"
		NL "	if (WIFEXITED(st) && (WEXITSTATUS(st) == 42))"
		NL "		printf(\"K\");"
		NL "	else"
		NL "		printf(\"%%d\", st);"
		NL "	printf(\"\\n\");"
		NL "	return 0;"
		NL "}"
		NL ;

	require("cc/cc", logdepth, fatal);
	if (require("libs/proc/fork", logdepth, fatal))
		return try_fail(logdepth, "libs/proc/wait");

	report("Checking for wait... ");
	logprintf(logdepth, "find_proc_wait: trying to find wait...\n");
	logdepth++;

	inc1 = get("libs/proc/fork/includes");
	if (inc1 != NULL) {
		char *i, *o;
		inc = strclone(inc1);
		for(i = o = inc; *i != '\0'; i++,o++) {
			if ((i[0] == '\\') && (i[1] == 'n')) {
				*o = '\n';
				i++;
			}
			else
				*o = *i;
		}
		*o = '\0';
		sprintf(test_c, test_c_in, inc);
		free(inc);
	}
	else
		sprintf(test_c, test_c_in, "");

	if (try_icl(logdepth, "libs/proc/wait", test_c, "#include <sys/types.h>\n#include <sys/wait.h>", NULL, NULL)) return 0;

	return try_fail(logdepth, "libs/proc/wait");
}

int find_proc__getpid(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/proc/_getpid";
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL no_implicit(int, "_getpid", "_getpid")
		NL "	if (_getpid() != (-1))"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for _getpid()... ");
	logprintf(logdepth, "find_proc__getpid: trying to find _getpid()...\n");
	logdepth++;

	if (try_icl(logdepth, key, test_c, "#include <process.h>", NULL, NULL))
		return 0;

	return try_fail(logdepth, key);
}

int find_proc_getexecname(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/proc/getexecname";
	const char *test_c =
		NL "void my_puts(const char *s);"
		NL "int main()"
		NL "{"
		NL no_implicit(const char*, "getexecname", "getexecname")
		NL "	getexecname();"
		NL "	my_puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL "#include <stdio.h>"
		NL "void my_puts(const char *s)"
		NL "{"
		NL "	puts(s);"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for getexecname()... ");
	logprintf(logdepth, "find_proc_getexecname: trying to find getexecname()...\n");
	logdepth++;

	if (try_icl(logdepth, key, test_c, "#include <stdlib.h>", NULL, NULL))
		return 0;

	return try_fail(logdepth, key);
}

int find_proc_GetModuleFileNameA(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/proc/GetModuleFileNameA";
	const char *test_c =
		NL "void my_puts(const char *s);"
		NL "char path_buffer[5000];"
		NL "int main()"
		NL "{"
		NL "	HMODULE hModule = GetModuleHandleA(NULL);"
		NL "	if (GetModuleFileNameA(hModule, path_buffer, sizeof(path_buffer)) != 0)"
		NL "		my_puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL "#include <stdio.h>"
		NL "void my_puts(const char *s)"
		NL "{"
		NL "	puts(s);"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for GetModuleFileNameA()... ");
	logprintf(logdepth, "find_proc_GetModuleFileNameA: trying to find GetModuleFileNameA()...\n");
	logdepth++;

	if (try_icl(logdepth, key, test_c, "#include <windows.h>", NULL, NULL))
		return 0;
	if (try_icl(logdepth, key, test_c, "#include <windows.h>", NULL, "-lkernel32"))
		return 0;

	return try_fail(logdepth, key);
}

int find_proc_shmget(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/proc/shmget";
	const char *test_c =
		NL "void my_puts(const char *s);"
		NL "int main()"
		NL "{"
		NL "	int shmid;"
		NL "	char *addr;"
		NL "	shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT|IPC_EXCL|0600);"
		NL "	if (shmid < 0)"
		NL "		return 1;"
		NL "	addr = (char *)shmat(shmid, (void *)0, 0);"
		NL "	if (addr == (char *)0) {"
		NL "		shmctl(shmid, IPC_RMID, (void *)0);"
		NL "		return 1;"
		NL "	}"
		NL "	if (shmctl(shmid, IPC_RMID, (void *)0) != 0)"
		NL "		return 1;"
		NL "	if (addr[2] != 0)"
		NL "		return 1;"
		NL "	addr[0] = 'O'; addr[1] = 'K';"
		NL "	my_puts(addr);"
		NL "	return 0;"
		NL "}"
		NL "#include <stdio.h>"
		NL "void my_puts(const char *s)"
		NL "{"
		NL "	puts(s);"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for shmget()... ");
	logprintf(logdepth, "find_proc_shmget: trying to find shmget()...\n");
	logdepth++;

	if (try_icl(logdepth, key, test_c, "#include <sys/ipc.h>\n#include <sys/shm.h>", NULL, NULL))
		return 0;

	return try_fail(logdepth, key);
}

int find_proc_CreateFileMappingA(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/proc/CreateFileMappingA";
	const char *test_c =
		NL "void my_puts(const char *s);"
		NL "#define MAP_BUF_SIZE 4096"
		NL "int main()"
		NL "{"
		NL "	char *addr;"
		NL "	HANDLE hMap = CreateFileMappingA("
		NL "		INVALID_HANDLE_VALUE,"
		NL "		NULL,"
		NL "		PAGE_READWRITE,"
		NL "		0,"
		NL "		MAP_BUF_SIZE,"
		NL "		NULL);"
		NL "	if (hMap == NULL)"
		NL "		return 1;"
		NL "	addr = (char *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS,"
		NL "		0, 0, MAP_BUF_SIZE);"
		NL "	if (addr == NULL) {"
		NL "		CloseHandle(hMap);"
		NL "		return 1;"
		NL "	}"
		NL "	if (addr[2] != 0) {"
		NL "		UnmapViewOfFile(addr);"
		NL "		CloseHandle(hMap);"
		NL "		return 1;"
		NL "	}"
		NL "	addr[0] = 'O'; addr[1] = 'K';"
		NL "	my_puts(addr);"
		NL "	UnmapViewOfFile(addr);"
		NL "	CloseHandle(hMap);"
		NL "	return 0;"
		NL "}"
		NL "#include <stdio.h>"
		NL "void my_puts(const char *s)"
		NL "{"
		NL "	puts(s);"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for CreateFileMappingA()... ");
	logprintf(logdepth, "find_proc_CreateFileMappingA: trying to find CreateFileMappingA()...\n");
	logdepth++;

	if (try_icl(logdepth, key, test_c, "#include <windows.h>", NULL, NULL))
		return 0;
	if (try_icl(logdepth, key, test_c, "#include <windows.h>", NULL, "-lkernel32"))
		return 0;

	return try_fail(logdepth, key);
}
