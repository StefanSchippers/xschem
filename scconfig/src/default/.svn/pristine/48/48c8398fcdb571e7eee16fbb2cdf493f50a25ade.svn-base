/*
    scconfig - detection of standard library features: time/date/sleep related calls
    Copyright (C) 2011..2012  Tibor Palinkas

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

int find_time_usleep(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <unistd.h>"
		NL "int main() {"
		NL "	if (usleep(1) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for usleep()... ");
	logprintf(logdepth, "find_time_usleep: trying to find usleep...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/time/usleep", test_c, NULL, NULL, NULL))
		return 0;
	return try_fail(logdepth, "libs/time/usleep");
}

int find_time_Sleep(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "int main() {"
		NL "	Sleep(1);"
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for Sleep()... ");
	logprintf(logdepth, "find_time_Sleep: trying to find Sleep...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/time/Sleep", test_c, "#include <windows.h>", NULL, NULL))
		return 0;
	return try_fail(logdepth, "libs/time/Sleep");
}

int find_time_gettimeofday(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "#include <stdlib.h>"
		NL "int main() {"
		NL "	struct timeval tv;"
		NL "	if (gettimeofday(&tv, NULL) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for gettimeofday()... ");
	logprintf(logdepth, "find_time_gettimeofday: trying to find gettimeofday...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/time/gettimeofday", test_c, "#include <sys/time.h>", NULL, NULL))
		return 0;
	return try_fail(logdepth, "libs/time/gettimeofday");
}


int find_time_ftime(const char *name, int logdepth, int fatal)
{
	char *test_c =
		NL "int main() {"
		NL "	struct timeb tb;"
		NL "	if (ftime(&tb) == 0)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for ftime()... ");
	logprintf(logdepth, "find_time_ftime: trying to find ftime...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/time/ftime", test_c, "#include <sys/timeb.h>", NULL, NULL))
		return 0;
	return try_fail(logdepth, "libs/time/ftime");
}

static const char timegm_test_c_template[] =
	NL "void my_puts(const char *s);"
	NL "int main() {"
	NL "	struct tm tm;"
	NL "	tm.tm_sec  = 50;"
	NL "	tm.tm_min  = 30;"
	NL "	tm.tm_hour = 6;"
	NL "	tm.tm_mday = 1;"
	NL "	tm.tm_mon  = 11;"
	NL "	tm.tm_year = 2018 - 1900;"
	NL "	tm.tm_wday = 0;"
	NL "	tm.tm_yday = 0;"
	NL "	if (%s(&tm) != (time_t)(-1))"
	NL "		my_puts(\"OK\");"
	NL "	return 0;"
	NL "}"
	NL "#include <stdio.h>"
	NL "void my_puts(const char *s)"
	NL "{"
	NL "	puts(s);"
	NL "}"
	NL;

int find_time_timegm(const char *name, int logdepth, int fatal)
{
	char test_c[1000];
	sprintf(test_c, timegm_test_c_template, "timegm");

	require("cc/cc", logdepth, fatal);

	report("Checking for timegm()... ");
	logprintf(logdepth, "find_time_timegm: trying to find timegm...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/time/timegm", test_c, "#include <time.h>", NULL, NULL))
		return 0;
	return try_fail(logdepth, "libs/time/timegm");
}

int find_time_mkgmtime(const char *name, int logdepth, int fatal)
{
	char test_c[1000];
	const char *ldflags[] = {"","-lmsvcr120","-lmsvcr110","-lmsvcr100","-lmsvcr90","-lmsvcr80","-lmsvcr71","-lmsvcr70",NULL};
	const char **ldf;

	sprintf(test_c, timegm_test_c_template, "_mkgmtime");

	require("cc/cc", logdepth, fatal);

	report("Checking for _mkgmtime()... ");
	logprintf(logdepth, "find_time_mkgmtime: trying to find _mkgmtime...\n");
	logdepth++;

	for (ldf = ldflags; *ldf; ++ldf)
		if (try_icl(logdepth, "libs/time/_mkgmtime", test_c, "#include <time.h>", NULL, *ldf))
			return 0;
	return try_fail(logdepth, "libs/time/_mkgmtime");
}

int find_time_gmtime_r(const char *name, int logdepth, int fatal)
{
	const char test_c[] =
		NL "void my_puts(const char *s);"
		NL "int main() {"
		NL "	time_t tim = 1543645850;"
		NL "	struct tm tm;"
		NL "	if (gmtime_r(&tim, &tm)" /* returns '&tm' */
		NL "		&& 50==tm.tm_sec"
		NL "		&& 30==tm.tm_min"
		NL "		&& 6==tm.tm_hour"
		NL "		&& 1==tm.tm_mday"
		NL "		&& 11==tm.tm_mon"
		NL "		&& (2018-1900)==tm.tm_year)"
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

	report("Checking for gmtime_r()... ");
	logprintf(logdepth, "find_time_gmtime_r: trying to find gmtime_r...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/time/gmtime_r", test_c, "#include <time.h>", NULL, NULL))
		return 0;
	return try_fail(logdepth, "libs/time/gmtime_r");
}

int find_time_gmtime_s(const char *name, int logdepth, int fatal)
{
	const char test_c[] =
		NL "void my_puts(const char *s);"
		NL "int main() {"
		NL "	time_t tim = 1543645850;"
		NL "	struct tm tm;"
		NL "	if (0==gmtime_s(&tm, &tim)" /* returns errno */
		NL "		&& 50==tm.tm_sec"
		NL "		&& 30==tm.tm_min"
		NL "		&& 6==tm.tm_hour"
		NL "		&& 1==tm.tm_mday"
		NL "		&& 11==tm.tm_mon"
		NL "		&& (2018-1900)==tm.tm_year)"
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

	report("Checking for gmtime_s()... ");
	logprintf(logdepth, "find_time_gmtime_s: trying to find gmtime_s...\n");
	logdepth++;

	if (try_icl(logdepth, "libs/time/gmtime_s", test_c, "#include <time.h>", NULL, NULL))
		return 0;
	return try_fail(logdepth, "libs/time/gmtime_s");
}

int find_time_localtime_r(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/time/localtime_r";
	const char test_c[] =
		NL "void my_puts(const char *s);"
		NL "int main() {"
		NL "	time_t tim = 1543645850;"
		NL "	struct tm tm;"
		NL "	if (localtime_r(&tim, &tm)" /* returns '&tm' */
		NL "		&& 0!=tm.tm_sec" /* depends on TZ: sadly we can't sure in anything */
		NL "		&& 0!=tm.tm_min"
		NL "		&& 0!=tm.tm_hour)"
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

	report("Checking for localtime_r()... ");
	logprintf(logdepth, "find_time_localtime_r: trying to find localtime_r...\n");
	logdepth++;

	if (try_icl(logdepth, key, test_c, "#include <time.h>", NULL, NULL))
		return 0;
	return try_fail(logdepth, key);
}

int find_time_localtime_s(const char *name, int logdepth, int fatal)
{
	const char *key = "libs/time/localtime_s";
	const char test_c[] =
		NL "void my_puts(const char *s);"
		NL "int main() {"
		NL "	time_t tim = 1543645850;"
		NL "	struct tm tm;"
		NL "	if (0==localtime_s(&tm, &tim)" /* returns errno */
		NL "		&& 0!=tm.tm_sec" /* depends on TZ: sadly we can't sure in anything */
		NL "		&& 0!=tm.tm_min"
		NL "		&& 0!=tm.tm_hour)"
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

	report("Checking for localtime_s()... ");
	logprintf(logdepth, "find_time_localtime_s: trying to find localtime_s...\n");
	logdepth++;

	if (try_icl(logdepth, key, test_c, "#include <time.h>", NULL, NULL))
		return 0;
	return try_fail(logdepth, key);
}
