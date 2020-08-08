#include <stdio.h>
#include "tmpasm.h"

static void indent(FILE *f, int depth)
{
	for(;depth > 0; depth--) fputc(' ', f);
}

static void print_arg(FILE *f, tmpasm_arg_t *a)
{
	if (a == NULL) {
		fprintf(f, "*NULL - broken AST*");
		return;
	}
	if (a->next != NULL) {
		/* block mode */
		fprintf(f, "[~");
		for(;a != NULL; a = a->next) {
			if (a->is_addr)
				fprintf(f, "~%s~", a->data);
			else
				fprintf(f, "%s", a->data);
		}
		fprintf(f, "~]");
	}
	else {
		if (a->is_addr)
			fprintf(f, "%s", a->data);
		else
			fprintf(f, "{%s}", a->data);
	}
}

static void print_loc(FILE *f, tmpasm_exec_t *c)
{
	if ((c->line != 0) || (c->col != 0))
		fprintf(f, " [at %d:%d]\n", c->line, c->col);
	else
		fprintf(f, "\n");
}

static void dump(FILE *f, int depth, tmpasm_exec_t *c)
{
	tmpasm_case_t *cc;
	int n;
	for(; c != NULL; c = c->next) {
		switch(c->kw) {
			case KW_NOP:
				indent(f, depth);
				fprintf(f, "(NOP)");
				print_loc(f, c);
				break;
			case KW_none:
				indent(f, depth);
				fprintf(f, "%s", c->payload.instr.call_name);
				print_loc(f, c);
				for(n = 0; n < c->payload.instr.argc; n++) {
					indent(f, depth+1);
					fprintf(f, "arg: ");
					print_arg(f, c->payload.instr.argv[n]);
					fprintf(f, "\n");
				}
				break;
			case KW_IF:
				indent(f, depth);
				fprintf(f, "if ");
				print_arg(f, c->payload.fc_if.cond);
				print_loc(f, c);
				indent(f, depth);
				fprintf(f, "then:\n");
				dump(f, depth+1, c->payload.fc_if.code_then);
				indent(f, depth);
				fprintf(f, "else:\n");
				dump(f, depth+1, c->payload.fc_if.code_else);
				break;
			case KW_FOREACH:
				indent(f, depth);
				fprintf(f, "foreach %s in ", c->payload.fc_foreach.loop_var);
				print_arg(f, c->payload.fc_foreach.data);
				print_loc(f, c);
				dump(f, depth+1, c->payload.fc_foreach.code_body);
				break;
			case KW_SWITCH:
				indent(f, depth);
				fprintf(f, "switch ");
				print_arg(f, c->payload.fc_switch.cond);
				print_loc(f, c);
				for(cc = c->payload.fc_switch.first; cc != NULL; cc = cc->next) {
					indent(f, depth+1);
					if (cc->data != NULL) {
						fprintf(f, "case ");
						print_arg(f, cc->data);
						fprintf(f, "\n");
					}
					else
						printf("default\n");
					dump(f, depth+2, cc->body);
				}
				break;
			default:
				indent(f, depth);
				fprintf(f, "invalid kw ");
				print_loc(f, c);
		}
	}
}

void tmpasm_dump(tmpasm_t *ctx, FILE *f)
{
	dump(f, 0, ctx->code);
}
