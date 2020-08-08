#include <stdlib.h>
#include <string.h>
#include "openfiles.h"
#include "libs.h"

static openfile_t *find_file_by_name(openfiles_t *of, const char *name, int alloc, const char *mode, int recursion)
{
	int n;
	struct stat buf;
	FILE *f;

	if (recursion > 4) {
		fprintf(stderr, "scconfig internal error: openfiles infinite recursion for %s\n", name);
		abort();
	}

	if (stat(name, &buf) != 0) {
		/* File does not exist - try to create it or return NULL */
		if (*mode == 'w') {
			f = fopen(name, "w");
			if (f == NULL)
				return NULL;
			fclose(f);
			return find_file_by_name(of, name, alloc, mode, recursion + 1);
		}
		return NULL;
	}

	/* look for an existing open file in the list */
	for(n = 0; n < of->used; n++)
		if ((of->files[n].dev == buf.st_dev) && (of->files[n].ino == buf.st_ino) && (strcmp(of->files[n].mode, mode) == 0))
			return &(of->files[n]);

	if (!alloc)
		return NULL;

	/* File exists but not on the list yet, allocate a new slot for it */
	/* TODO: try to find an empty slot first */
	if (of->used >= of->alloced) {
		of->alloced += 16;
		of->files = realloc(of->files, sizeof(openfile_t) * of->alloced);
	}

	n = of->used;
	of->files[n].dev  = buf.st_dev;
	of->files[n].ino  = buf.st_ino;
	of->files[n].f    = NULL;
	of->files[n].mode = strclone(mode);
	of->used++;
	return &(of->files[n]);
}

void release(openfile_t *o)
{
	if (o->mode != NULL) {
		free(o->mode);
		o->mode = NULL;
	}
	if (o->f != NULL) {
		fclose(o->f);
		o->f = NULL;
	}
	o->dev = -1;
	o->ino = -1;
}

FILE *openfile_open(openfiles_t *of, const char *fn, const char *mode)
{
	openfile_t *o;
	o = find_file_by_name(of, fn, 1, mode, 0);
	if (o == NULL)
		return NULL;
	o->f = fopen(fn, mode);
	if (o->f == NULL) {
		release(o);
		return NULL;
	}
	return o->f;
}

void openfile_closeall(openfiles_t *of)
{
	int n;
	if (of->files == NULL)
		return;
	for(n = 0; n < of->used; n++)
		release(&(of->files[n]));
}

void openfile_free(openfiles_t *of)
{
	openfile_closeall(of);
	if (of->files != NULL)
		free(of->files);
}
