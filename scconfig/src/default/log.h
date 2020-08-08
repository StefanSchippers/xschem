#include <stdlib.h>
#include <stdio.h>

#define max_spaces 64
extern char *spaces;

#define logprefix(n) (((n) > max_spaces) ? spaces : (spaces+max_spaces-(n)))

void logprintf(int depth, const char *format, ...);
void error(const char *format, ...);
void report(const char *format, ...);

void log_merge(int logdepth, const char *fn);

extern FILE *logfile;
extern void log_init(void);
void log_uninit(void);

extern char *fn_log;
