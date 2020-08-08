#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct openfile_s {
	FILE *f;

	/* identify the file: */
	dev_t dev;
	ino_t ino;

	char *mode;
} openfile_t;

typedef struct openfiles_s {
	int alloced, used;
	openfile_t *files;
} openfiles_t;

FILE *openfile_open(openfiles_t *of, const char *fn, const char *mode);
void openfile_closeall(openfiles_t *of);
void openfile_free(openfiles_t *of);
