#ifndef SCC_ARG_H
#define SCC_ARG_H

#include <stdio.h>

typedef struct {
	char *arg;
	char *path;
	int (*callback)(const char *key, const char *value);
	char *help;
} argtbl_t;

extern argtbl_t main_argument_table[];



void process_args(int argc, char *argv[]);
void help_default_args(FILE *f, const char *prefix);


/* main.c: */
extern int custom_arg(const char *key, const char *value);
extern int num_custom_reqs;

#endif
