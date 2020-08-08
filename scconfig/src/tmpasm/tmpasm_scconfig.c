#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tmpasm.h"
#include "db.h"
#include "regex.h"
#include "openfiles.h"
#include "libs.h"
#include "tmpasm_scconfig.h"
#include "log.h"
#include "regex.h"

#ifndef TMPASM_PATH
#define TMPASM_PATH "/tmpasm"
#endif

#ifndef IFS_PATH
#define IFS_PATH TMPASM_PATH "/IFS"
#endif

#ifndef IFS_DEFAULT
#define IFS_DEFAULT " \t\r\n"
#endif

#ifndef OFS_PATH
#define OFS_PATH TMPASM_PATH "/OFS"
#endif

#ifndef OFS_DEFAULT
#define OFS_DEFAULT "\n"
#endif

typedef struct scc_s {
	openfiles_t ofl;
	FILE *fout, *default_fout;
	const char *cwd;
} scc_t;

static const char *scc_runtime_error_fmts[] = {
	/* -0  */  "success scc %s",
	/* -1  */  "\"put\" requires exactly two arguments (got %s)",
	/* -2  */  "not enough arguments for sub; should be \"sub node pattern str\"%s",
	/* -3  */  "regex syntax error: %s",
	/* -4  */  "not enough arguments for uniq; should be \"uniq destnode\" or \"uniq destnode src\"%s",
	/* -5  */  "redir: too many arguments%s",
	/* -6  */  "redir: can't open %s",
	/* -7  */  "exiting due to a previous runtime error occurred in included file %s",
	/* -8  */  "can't include '%s': can't open file",
	/* -9  */  "\"put\" requires two or three arguments (got %s)",
	/* -10 */  "\"order\" requires 4 or 5 arguments: \"order destnode word before|after word\" or \"order destnode src word before|after word\"",
	/* -11 */  "\"uniq\" got too many grouping regular expressions",
	NULL
};

static int print_runtime_error(tmpasm_t *ctx, const char *ifn)
{
	if (ctx->runtime_error != 0) {
		const char *fmt = tmpasm_runtime_error_fmt(ctx);
		fprintf(stderr, "Runtime error at %s %d:%d: ", ifn, ctx->runtime_error_line, ctx->runtime_error_col);
		fprintf(stderr, fmt, (ctx->runtime_error_data == NULL ? "" : ctx->runtime_error_data));
		fprintf(stderr, "\n");
		return -1;
	}
	return 0;
}

/* allocate and build a full path using ud->cwd and fn */
static char *scc_path(scc_t *ud, const char *fn)
{
	if (ud->cwd == NULL)
		return strclone(fn);
	return str_concat("", ud->cwd, "/", fn, NULL);
}

/******** db binding ********/

static const char *scc_get(tmpasm_t *ctx, const char *addr)
{
	(void) ctx;  /* not used */

	if (*addr == '&') { /* return whether exists */
		if (get(addr+1) != NULL)
			return strue;
		return sfalse;
	}

	if (*addr == '?') { /* safe get: return "" instead of NULL to avoid runtime error */
		const char *res = get(addr+1);
		if (res == NULL)
			return "";
		return res;
	}
	return get(addr);
}

static void scc_set(tmpasm_t *ctx, const char *addr, const char *data)
{
	(void) ctx;  /* not used */
	put(addr, data);
}

static int scc_is_true(tmpasm_t *ctx, const char *data)
{
	(void) ctx;  /* not used */
	return ((strcmp(data, "1") == 0) || istrue(data));
}

static int scc_match(tmpasm_t *ctx, const char *str, const char *pat)
{
	(void) ctx;  /* not used */
	re_comp(pat);
	return re_exec(str);
}

static const char *scc_ifs(tmpasm_t *ctx)
{
	const char *ifs = get(IFS_PATH);
	(void) ctx;  /* not used */
	if (ifs == NULL)
		return IFS_DEFAULT;
	return ifs;
}

static const char *scc_ofs(tmpasm_t *ctx)
{
	const char *ofs = get(OFS_PATH);
	(void) ctx;  /* not used */
	if (ofs == NULL)
		return OFS_DEFAULT;
	return ofs;
}

static const char *scc_next(tmpasm_t *ctx, void **state)
{
	char **s = (char **)state;
	char *start;
	const char *IFS;

	IFS = scc_ifs(ctx);

	/* strip leading whitespace */
	while(chr_inset(**s, IFS)) (*s)++;

	/* at the end of the string, no more tokens */
	if (**s == '\0')
		return NULL;

	start = *s;

	/* skip non-whitespace */
	while(!(chr_inset(**s, IFS)) && (**s != '\0')) (*s)++;

	if (**s != '\0') {
		**s = '\0';
		(*s)++;
	}
	return start;
}


static const char *scc_first(tmpasm_t *ctx, void **state, char *list)
{
	*state = list;

	return scc_next(ctx, state);
}


/******** instructions ********/
static void instr_put(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[])
{
	char *addr, *val;
	(void) iname;  /* not used */
	if (argc != 2) {
		char str[16];
		sprintf(str, "%d", argc);
		tmpasm_runtime_error(ctx, -1, str);
		return;
	}
	addr = tmpasm_arg2str(ctx, argv[0], 1);
	val  = tmpasm_arg2str(ctx, argv[1], 0);
	if (*addr != '\0')
		put(addr, val);
	free(addr);
	free(val);
}

static void instr_resolve(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[])
{
	char *dst, *srca;
	const char *src;
	(void) iname;  /* not used */
	if (argc != 2) {
		char str[16];
		sprintf(str, "%d", argc);
		tmpasm_runtime_error(ctx, -1, str);
		return;
	}

	dst = tmpasm_arg2str(ctx, argv[0], 1);
	srca = tmpasm_arg2str(ctx, argv[1], 0);
	src = scc_get(ctx, srca);
	if (*dst != '\0')
		put(dst, src);
	free(dst);
	free(srca);
}


static void instr_append(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[])
{
	char *addr, *val;
	char *sep;
	(void) iname;  /* not used */

	if ((argc < 2) || (argc > 3)) {
		char str[16];
		sprintf(str, "%d", argc);
		tmpasm_runtime_error(ctx, -9, str);
		return;
	}
	addr = tmpasm_arg2str(ctx, argv[0], 1);
	val  = tmpasm_arg2str(ctx, argv[1], 0);
	if (argc >= 3)
		sep = tmpasm_arg2str(ctx, argv[2], 0);
	else
		sep = strclone(scc_ofs(ctx));
	if (*addr != '\0') {
		append(addr, sep);
		append(addr, val);
	}
	free(addr);
	free(val);
	free(sep);
}

static void instr_report(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[])
{
	int n;
	(void) iname;  /* not used */
	for(n = 0; n < argc; n++) {
		char *val;
		val  = tmpasm_arg2str(ctx, argv[n], 0);
		report("%s", val);
		free(val);
	}
}

static void instr_abort(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[])
{
	scc_t *ud = (scc_t *)ctx->user_data;
	(void) iname;  /* not used */
	(void) argc;  /* not used */
	(void) argv;  /* not used */
	report("Abort requested by template.\n");
	if (ud->fout) fflush(ud->fout);
	fflush(stdout);
	fflush(stderr);
	abort();
}

static void instr_halt(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[])
{
	(void) iname;  /* not used */
	(void) argc;  /* not used */
	(void) argv;  /* not used */
	ctx->halt = 1;
}

static void instr_sub(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[])
{
	char *node, *pat, *err, *csub, *buff, *end;
	const char *start;
	const char *val;
	int score, slen, global;

	if (argc < 3) {
		tmpasm_runtime_error(ctx, -2, NULL);
		return;
	}

	node = tmpasm_arg2str(ctx, argv[0], 1);
	pat  = tmpasm_arg2str(ctx, argv[1], 0);
	csub = tmpasm_arg2str(ctx, argv[2], 0);
	global = (*iname == 'g');

	val = get(node);
	if (val == NULL)
		val="";
	err = re_comp(pat);
	if (err != NULL) {
		tmpasm_runtime_error(ctx, -3, err);
		return;
	}

	slen = strlen(csub);
	if (global)
		buff = malloc(strlen(val)*(slen+3)+32); /* big enough for worst case, when every letter and $ and ^ are replaced with sub */
	else
		buff = malloc(strlen(val)+slen+32);  /* only one replacement will be done */
	strcpy(buff, val);

	start = buff;
	do {
		score = re_exec(start);
		if (score == 0)
			break;
		end = buff + strlen(buff);
		if (eopat[0] - bopat[0] != slen) {
			int mlen = end - eopat[0]+1;
			if (mlen > 0)
				memmove((char *)(bopat[0] + slen), eopat[0], mlen);
		}
		memcpy((char *)bopat[0], csub, slen);
		start = bopat[0] + slen;
	} while(global);

	buff = realloc(buff, strlen(buff)+1);
	put(node, buff);
	free(buff);
	free(node);
	free(pat);
	free(csub);
}

#define UNIQ_ERE_MAX 16
static char *uniq_eres[UNIQ_ERE_MAX];

static void instr_uniq(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[])
{
	char *node, *strlist, *buff;
	int eres = 0;

	if (argc < 1) {
		tmpasm_runtime_error(ctx, -4, NULL);
		return;
	}
	node = tmpasm_arg2str(ctx, argv[0], 1);
	if (argc > 1) {
		int offs = 2;

		strlist = tmpasm_arg2str(ctx, argv[1], 0);
		if ((argc-offs) >= UNIQ_ERE_MAX) {
			tmpasm_runtime_error(ctx, -11, NULL);
			return;
		}
		while(argc > offs)
			uniq_eres[eres++] = tmpasm_arg2str(ctx, argv[offs++], 0);
		if (eres > 0)
			uniq_eres[eres++] = ".*";
	}
	else
		strlist = strclone(get(node));
	buff = uniq_inc_str(strlist, scc_ifs(ctx), scc_ofs(ctx), (*iname == 's'), eres, uniq_eres);
	put(node, buff);
	free(buff);
	free(strlist);
	free(node);
}

static void instr_order(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[])
{
	char *node, *strlist, *buff, *w1, *dirs, *w2;
	int offs, dir;

	if ((argc != 4) && (argc != 5)) {
		tmpasm_runtime_error(ctx, -10, NULL);
		return;
	}
	node = tmpasm_arg2str(ctx, argv[0], 1);
	if (argc > 4) {
		strlist = tmpasm_arg2str(ctx, argv[1], 0);
		offs = 2;
	}
	else {
		strlist = strclone(get(node));
		offs = 1;
	}

	w1   = tmpasm_arg2str(ctx, argv[offs], 0);
	dirs = tmpasm_arg2str(ctx, argv[offs+1], 0);
	w2   = tmpasm_arg2str(ctx, argv[offs+2], 0);

	if (strcmp(dirs, "before") == 0)
		dir = -1;
	else if (strcmp(dirs, "after") == 0)
		dir = +1;
	else {
		tmpasm_runtime_error(ctx, -10, NULL);
		return;
	}

	buff = order_inc_str(strlist, scc_ifs(ctx), w1, dir, w2);
	put(node, buff);
	free(buff);
	free(strlist);
	free(node);
}

static void instr_print(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[])
{
	int n;
	scc_t *ud = (scc_t *)ctx->user_data;
	(void) iname;  /* not used */

	for(n = 0; n < argc; n++) {
		char *val;
		val  = tmpasm_arg2str(ctx, argv[n], 0);
		fprintf(ud->fout, "%s", val);
		free(val);
	}
}

static void instr_print_ternary(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[])
{
	char *s_cond, *s;
	scc_t *ud = (scc_t *)ctx->user_data;
	(void) iname;  /* not used */

	if ((argc < 2) || (argc > 3)) {
		char str[16];
		sprintf(str, "%d", argc);
		tmpasm_runtime_error(ctx, -1, str);
		return;
	}

	s_cond  = tmpasm_arg2str(ctx, argv[0], 0);

	if (ctx->cb->is_true(ctx, s_cond))
		s  = tmpasm_arg2str(ctx, argv[1], 0);
	else
		s  = tmpasm_arg2str(ctx, argv[2], 0);

	fprintf(ud->fout, "%s", s);

	free(s_cond);
	free(s);
}

static void scc_tmpasm_parse_(tmpasm_t *ctx, const char *cwd, FILE *fin, FILE *default_fout, FILE *fout)
{
	scc_t *ud = malloc(sizeof(scc_t));
	memset(&ud->ofl, 0, sizeof(ud->ofl));
	ctx->user_data = ud;
	ud->default_fout = default_fout;
	ud->fout = fout;
	ud->cwd = cwd;

	for(;;) {
		int c;
		c = fgetc(fin);
		if (c == EOF)
			break;
		tmpasm_gotchar(ctx, c);
	}

}

void scc_tmpasm_parse(tmpasm_t *ctx, const char *cwd, FILE *fin, FILE *fout)
{
	scc_tmpasm_parse_(ctx, cwd, fin, fout, fout);
}

#ifndef NO_FILE_IO
static void instr_include(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[])
{
	scc_t *ud = (scc_t *)ctx->user_data;
	int n;
	(void) iname;  /* not used */

	for(n = 0; n < argc; n++) {
		char *fn, *path;
		FILE *fin;
		tmpasm_t *child;

		fn = tmpasm_arg2str(ctx, argv[n], 0);
		path = scc_path(ud, fn);
		fin = fopen(path, "r");
		if (fin == NULL) {
			tmpasm_runtime_error(ctx, -8, path);
			free(fn);
			free(path);
			return;
		}
		child = tmpasm_init(ctx->cb);
		scc_tmpasm_parse_(child, ud->cwd, fin, ud->default_fout, ud->fout);
		tmpasm_execute(child);
		if (print_runtime_error(child, path) != 0)
			tmpasm_runtime_error(ctx, -7, path);
		tmpasm_uninit(child);
		fclose(fin);
		free(fn);
		free(path);
	}
}


static void instr_redir(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[])
{
	char *path, *fn, *mode;
	scc_t *ud = (scc_t *)ctx->user_data;
	(void) iname;  /* not used */
	fflush(ud->fout);
	switch(argc) {
		case 0: ud->fout = ud->default_fout; return;  /* set redirection to default */
		case 1: mode = strclone("w"); break;
		case 2: mode = tmpasm_arg2str(ctx, argv[1], 0); break;
		default:
			tmpasm_runtime_error(ctx, -5, NULL);
			return;
	}

	fn = tmpasm_arg2str(ctx, argv[0], 0);
	path = scc_path(ud, fn);
	ud->fout = openfile_open(&ud->ofl, path, mode);
	if (ud->fout == NULL) {
		char *err = malloc(strlen(fn) + strlen(path) + strlen(mode) + 16);
		sprintf(err, "%s (%s) for %s", path, fn, mode);
		tmpasm_runtime_error(ctx, -6, err);
		free(err);
		free(path);
		return;
	}
	free(fn);
	free(mode);
	free(path);
}
#endif

#ifdef TMPASM_TESTER
static void instr_unknown(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[])
{
	printf("ERROR: unknown instruction '%s'\n", iname);
}
#endif


/******** interface ********/

tmpasm_instr *scc_resolve(tmpasm_t *ctx, const char *name)
{
	(void) ctx;  /* not used */
/* TODO: make this a hash */
	if (strcmp(name, "put") == 0)
		return instr_put;
	if (strcmp(name, "resolve") == 0)
		return instr_resolve;
	if (strcmp(name, "append") == 0)
		return instr_append;
	if (strcmp(name, "print") == 0)
		return instr_print;
	if (strcmp(name, "print_ternary") == 0)
		return instr_print_ternary;
#ifndef TMPASM_NO_FILE_IO
	if (strcmp(name, "redir") == 0)
		return instr_redir;
	if (strcmp(name, "include") == 0)
		return instr_include;
#endif
	if (strcmp(name, "report") == 0)
		return instr_report;
	if (strcmp(name, "abort") == 0)
		return instr_abort;
	if (strcmp(name, "halt") == 0)
		return instr_halt;
	if (strcmp(name, "uniq") == 0)
		return instr_uniq;
	if (strcmp(name, "order") == 0)
		return instr_order;
	if (strcmp(name, "sortuniq") == 0)
		return instr_uniq;
	if ((strcmp(name, "sub") == 0) || (strcmp(name, "gsub") == 0))
		return instr_sub;

#ifndef TMPASM_TESTER
	return NULL;
#else
	return instr_unknown;
#endif
}


static const char *scc_err_fmt(tmpasm_t *ctx)
{
	int code;
	code = -ctx->runtime_error;

	if ((code < 0) || ((size_t)code > (sizeof(scc_runtime_error_fmts)/sizeof(char *))))
		return NULL;
	return scc_runtime_error_fmts[code];
}


static void scc_preexec(tmpasm_t *ctx)
{
	(void) ctx;  /* not used */
	db_mkdir(TMPASM_PATH);
}

static void scc_postexec(tmpasm_t *ctx)
{
	scc_t *ud = (scc_t *)ctx->user_data;
	openfile_free(&ud->ofl);
	free(ud);
}

tmpasm_cb_t scc_cb = {
	scc_get, scc_set, scc_is_true, scc_match, scc_first, scc_next,
	scc_resolve, scc_preexec, scc_postexec, scc_err_fmt
};

int tmpasm(const char *wdir, const char *input, const char *output)
{
	tmpasm_t *ctx;
	FILE *fin, *fout;
	int ret;
	scc_t ud_tmp;
	char *path;

	ud_tmp.cwd = wdir;

	path = scc_path(&ud_tmp, input);
	fin = fopen(path, "r");
	if (fin == NULL) {
		fprintf(stderr, "ERROR: tmpasm: can not open script '%s' (%s in %s)\n", path, input, wdir);
		free(path);
		return -1;
	}
	free(path);

	path = scc_path(&ud_tmp, output);
	fout = fopen(path, "w");
	if (fout == NULL) {
		fprintf(stderr, "ERROR: tmpasm: can not open output '%s' (%s in %s)\n", path, output, wdir);
		free(path);
		return -1;
	}
	free(path);

	ctx = tmpasm_init(&scc_cb);
	scc_tmpasm_parse_(ctx, wdir, fin, fout, fout);
	if (!ctx->dead)
		tmpasm_execute(ctx);
	fclose(fin);
	fclose(fout);

	ret = print_runtime_error(ctx, input);

	tmpasm_uninit(ctx);
	return ret;
}

FILE *tmpasm_fout(tmpasm_t *ctx)
{
	scc_t *ud = (scc_t *)ctx->user_data;
	return ud->fout;
}
