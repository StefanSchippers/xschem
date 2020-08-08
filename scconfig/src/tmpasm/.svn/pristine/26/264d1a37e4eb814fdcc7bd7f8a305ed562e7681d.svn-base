#include <stdlib.h>
#include <stdio.h>
#include "tmpasm.h"
#include "tmpasm_scconfig.h"
#include "debug.h"
#include "db.h"

tmpasm_t *ctx;

void re_fail(char *s, char c)
{
	fprintf(stderr, "Regex error: %s [opcode %o]\n", s, c);
	abort();
}

static void do_dump()
{
	tmpasm_dump(ctx, stdout);
}

static void do_exec()
{
	if (ctx->dead)
		fprintf(stderr, "Can not execute the script due to the above compilation error.\n");
	else
		tmpasm_execute(ctx);
}

static void scc_init(void)
{
	db_init();
	db_mkdir("/local");
	db_cd("/local");
}

int main(int argc, char *argv[])
{
	scc_init();
	ctx = tmpasm_init(&scc_cb);
	scc_tmpasm_parse(ctx, NULL, stdin, stdout);

	if (argc > 1) {
		char *cmd;
		cmd = argv[1];
		while(*cmd == '-') cmd++;
		switch(*cmd) {
			case 'd': do_dump(); break;
			case 'e': do_exec(); break;
		}
	}
	else
		do_dump();

	if (ctx->runtime_error != 0) {
		const char *fmt = tmpasm_runtime_error_fmt(ctx);
		fprintf(stderr, "Runtime error at %d:%d: ", ctx->runtime_error_line, ctx->runtime_error_col);
		fprintf(stderr, fmt, (ctx->runtime_error_data == NULL ? "" : ctx->runtime_error_data));
		fprintf(stderr, "\n");
	}

	tmpasm_uninit(ctx);
	db_uninit();
	return 0;
}
