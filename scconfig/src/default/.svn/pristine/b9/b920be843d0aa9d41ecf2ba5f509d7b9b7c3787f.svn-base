/*
    scconfig - logging
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

#include "log.h"
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

char *spaces  = "                                                                ";
FILE *logfile = NULL;
char *fn_log = "config.log";

void log_init(void)
{
	if (fn_log != NULL) {
		/* double open for truncate - for extreme portability, please do not "fix" */
		logfile = fopen(fn_log, "w");
		assert(logfile != NULL);
		fclose(logfile);
		logfile = fopen(fn_log, "a");
		assert(logfile != NULL);
	}
}

void log_uninit(void)
{
	if (logfile != NULL)
		fclose(logfile);
}

void logprintf(int depth, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);

	if (logfile != NULL) {
		fprintf(logfile, "%s", logprefix(depth));
		vfprintf(logfile, format, ap);
		fflush(logfile);
	}

	va_end(ap);
}

void error(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	va_start(ap, format);
	if (logfile != NULL) {
		fprintf(logfile, "###error### ");
		vfprintf(logfile, format, ap);
		fflush(logfile);
	}
	va_end(ap);

}

void report(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	fflush(stdout);
	va_end(ap);

	va_start(ap, format);
	if (logfile != NULL) {
		fprintf(logfile, "###report### ");
		vfprintf(logfile, format, ap);
		fflush(logfile);
	}
	va_end(ap);
}

void log_merge(int logdepth, const char *fn)
{
	FILE *f;
	char line[2048];
	int lines;

	if (logfile == NULL)
		return;

	f = fopen(fn, "r");
	if (f == NULL) {
		logprintf(logdepth, "scconfig error: couldn't open %s for merging.\n", fn);
		return;
	}
	lines = 0;
	while(!(feof(f))) {
		*line = '\0';
		fgets(line, sizeof(line), f);
		if (*line != '\0') {
			if (lines == 0)
				logprintf(logdepth, "========= output dump start ============\n");
			lines++;
			logprintf(logdepth, "%s", line);
			/* Make sure we have newline at the end of each line */
			if (line[strlen(line)-1] != '\n')
				logprintf(0, "\n");
		}
	}
	if (lines == 0)
		logprintf(logdepth, "========= empty stderr =================\n");
	else
		logprintf(logdepth, "========= output dump end ==============\n");
	fclose(f);
}
