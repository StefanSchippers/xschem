#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "tmpasm.h"
#include "debug.h"

#define is_space(c)  (((c) == ' ') || ((c) == '\t'))
#define is_sep(c)    (((c) == '\n') || ((c) == '\r') || ((c) == ';'))
#define is_addr(c)   ( (((c) >= '0') && ((c) <= '9')) || (((c) >= 'a') && ((c) <= 'z')) || (((c) >= 'A') && ((c) <= 'Z')) || ((c) == '_') || ((c) == '?') || ((c) == '.') || ((c) == ',')  || ((c) == ',') || ((c) == '-') || ((c) == '/') || ((c) == '&') )

/* this local copy is to make tmpasm compile independently */
static char *strclone(const char *str)
{
	int l;
	char *ret;

	if (str == NULL)
		return NULL;

	l   = strlen(str)+1;
	ret = malloc(l);
	memcpy(ret, str, l);
	return ret;
}


#define TOP ctx->st

static const char *kw_names[] = {"-", "if", "then", "else", "end", "foreach", "in", "switch", "case", "default", "nop", NULL };

static tmpasm_kw_t kw_lookup(const char *str)
{
	const char **k;
	tmpasm_kw_t i;

/* slow linear search is enough: we have only a few keywords */
	for(k = kw_names, i = KW_none; *k != NULL; k++,i++)
		if (strcmp(*k, str) == 0)
			return i;
	return KW_none;
}


tmpasm_exec_t *code_new(tmpasm_kw_t kw)
{
	tmpasm_exec_t *c;
	c = calloc(sizeof(tmpasm_exec_t), 1);
	c->kw = kw;
	return c;
}

/*tmpasm_exec_t *code_end(tmpasm_exec_t *start)
{
	while(start->next != NULL)
		start = start->next;
	return start;
}*/

tmpasm_exec_t *code_append(tmpasm_t *ctx, tmpasm_kw_t kw)
{
	tmpasm_exec_t *c;
/*	c = code_end(TOP->code);*/
	c = TOP->last_code;
	if (TOP->last_code->kw != KW_NOP) {
		c->next = code_new(kw);
		return c->next;
	}
	c->kw = kw;
	return c;
}

static void error(tmpasm_t *ctx, char c, char *msg)
{
	fprintf(stderr, "error: %s at %d:%d\n", msg, ctx->line, ctx->col);
	if (c != 0)
		fprintf(stderr, " character last seen: %c\n", c);
	ctx->dead = 1;
}

static void push(tmpasm_t *ctx, tmpasm_kw_t kw, tmpasm_state_t st, tmpasm_exec_t *code)
{
	tmpasm_stack_t *new;
	new = calloc(sizeof(tmpasm_stack_t), 1);
	new->kw = kw;
	new->state = st;
	new->next = ctx->st;
	new->last_code = code;
	ctx->st = new;
}

static void pop_(tmpasm_t *ctx, int chk_underfl)
{
	tmpasm_stack_t *old;
	old = ctx->st;
	ctx->st = old->next;

	/* stack underflow? */
	if (chk_underfl) {
		if (TOP == NULL) {
			error(ctx, 0, "Excess \"end\"");
			TOP = old;
			return;
		}
	}
	if (old->argv != NULL)
		free(old->argv);
	if (old->argend != NULL)
		free(old->argend);
	if (old->arg_used != NULL)
		free(old->arg_used);
	if (old->arg_alloced != NULL)
		free(old->arg_alloced);
	free(old);
}

static void pop(tmpasm_t *ctx)
{
	pop_(ctx, 1);
}


#define grow(arr, size) arr = realloc(arr, sizeof((arr)[0]) * size)

static void arg_new(tmpasm_t *ctx, int is_addr)
{
	if (TOP->args_used >= TOP->args_alloced) {
		TOP->args_alloced = TOP->args_alloced + 16;
		grow(TOP->argv, TOP->args_alloced);
		grow(TOP->argend, TOP->args_alloced);
		grow(TOP->arg_alloced, TOP->args_alloced);
		grow(TOP->arg_used, TOP->args_alloced);
	}

	TOP->arg_alloced[TOP->args_used] = 64;
	TOP->arg_used[TOP->args_used] = 0;

	TOP->argv[TOP->args_used] = malloc(TOP->arg_alloced[TOP->args_used]+sizeof(tmpasm_arg_t));
	TOP->argv[TOP->args_used]->is_addr = is_addr;
	TOP->argv[TOP->args_used]->next = NULL;
	TOP->argend[TOP->args_used] = TOP->argv[TOP->args_used];

	TOP->args_used++;
}


static void arg_append(tmpasm_t *ctx, char c)
{
	int i = TOP->args_used - 1;

	if (TOP->arg_used[i] >= TOP->arg_alloced[i]) {
		tmpasm_arg_t *prev, *last;

		/* since argend[i] is also in the ->next pointer of the previous item in a block chain, we need to look it up */
		for(prev = NULL, last = TOP->argv[i]; last->next != NULL; last = last->next)
			prev = last;

		TOP->arg_alloced[i] += 64;
		last = realloc(last, TOP->arg_alloced[i]+sizeof(tmpasm_arg_t));

		if (prev == NULL)
			TOP->argv[i] = last;
		else
			prev->next = last;

		TOP->argend[i] = last;
	}
	TOP->argend[i]->data[TOP->arg_used[i]] = c;
	TOP->arg_used[i]++;
}

static void arg_free(tmpasm_arg_t *a)
{
	tmpasm_arg_t *next;
	if (a == NULL)
		return;
	next = a->next;
	free(a);
	if (next != NULL)
		arg_free(next);
}

static void arg_new_next(tmpasm_t *ctx, int is_addr)
{
	tmpasm_arg_t *a;
	int id;

	arg_append(ctx, '\0');

	id = TOP->args_used - 1;
	assert(id>=0);
	TOP->arg_alloced[id] = 64;
	TOP->arg_used[id] = 0;

	a = malloc(TOP->arg_alloced[id]+sizeof(tmpasm_arg_t));
	strcpy(a->data, "QWERT");
	a->is_addr = is_addr;
	a->next = NULL;
	TOP->argend[id]->next = a;
	TOP->argend[id] = a;
}

static void arg_remove(tmpasm_t *ctx)
{
	assert(TOP->args_used == 1);
	TOP->args_used = 0;
	TOP->argv[0] = NULL;
	TOP->argend[0] = NULL;
	TOP->arg_alloced[0] = 0;
	TOP->arg_used[0] = 0;
}

static int arg_is_addr(tmpasm_arg_t *a)
{
	return (a->next == NULL) && (a->is_addr);
}

static void arg_end(tmpasm_t *ctx, int cmd_ctx)
{
	tmpasm_arg_t *a;
	arg_append(ctx, '\0');

	a = TOP->argv[TOP->args_used-1];
	if (cmd_ctx) {
		/* when argument ends in a command context (not in a block inline), we
		   may may need to switch back to command mode; example: after
		   the cond of an "if cond then"*/
		switch(TOP->kw) {
			case KW_IF:
				TOP->state = ST_PRECMD;
				break;
			case KW_FOREACH:
				if (!arg_is_addr(a)) {
					error(ctx, 0, "variable of a foreach must be an address");
					return;
				}
				TOP->last_code->payload.fc_foreach.loop_var = strclone(a->data);
				arg_free(a);
				arg_remove(ctx);
				TOP->state = ST_PRECMD;
				break;
			case KW_IN:
				/* pop will free the argv[] array, but not the elements so "a" is safe to use after this line */
				pop(ctx);
				/* in foreach context, after the IN-data */
				TOP->last_code->payload.fc_foreach.data = a;

				/* we are in the body now, TOP is the foreach context, last_code is body */
				TOP->last_code->payload.fc_foreach.code_body = code_new(KW_NOP);
				push(ctx, KW_none, ST_PRECMD, TOP->last_code->payload.fc_foreach.code_body);
				break;
			case KW_CASE:
				ctx->st->next->last_code->payload.fc_switch.last->data = a;
				arg_remove(ctx);
				push(ctx, KW_none, ST_PRECMD, TOP->last_code);
				break;
			case KW_SWITCH:
				TOP->last_code->payload.fc_switch.cond = a;
				arg_remove(ctx);
				TOP->state = ST_PRECMD;
				break;
			default:
				TOP->state = ST_PREDATA;
		}
	}
}



/* end of statement; update kw state for a composite control kw; for the rest
   just call the lib */
static void end_of_statement(tmpasm_t *ctx)
{
	switch(TOP->kw) {
		case KW_none:
		case KW_THEN:
		case KW_ELSE:
		case KW_CASE:
		case KW_DEFAULT:
			TOP->last_code->payload.instr.argc = TOP->args_used;
			TOP->last_code->payload.instr.argv = TOP->argv;
			TOP->argv = NULL;
			free(TOP->argend);
			TOP->argend = NULL;
			TOP->args_used = 0;
			TOP->args_alloced = 0;
			break;
		default:
			/* don't mess with the payload */
			;
	}
	TOP->state = ST_PRECMD;
}

#define loc_update() \
	do { \
		TOP->last_code->line = TOP->kwline; \
		TOP->last_code->col = TOP->kwcol; \
	} while(0)

static void got_kw(tmpasm_t *ctx, tmpasm_kw_t kw, int terminated)
{
	switch(kw) {
		case KW_END:
			/* then-else threads have their own subcontext within the if subcontext; end needs to pop the innermost subcontext before terminating the if context */
			if (TOP->kw == KW_IF) {
				error(ctx, 0, "unexpected \"end\" in \"if\" - expected \"then\"");
				goto bind_if_cond;
			}
			if ((TOP->kw == KW_ELSE) || (TOP->kw == KW_THEN))
				pop(ctx);


			if (TOP->kw == KW_SWITCH)
				TOP->kw = TOP->old_kw;
			else {
				pop(ctx);

			if ((TOP->kw == KW_CASE) || (TOP->kw == KW_DEFAULT))
				pop(ctx);

			}
			TOP->state = ST_PRECMD;

				/* have to restore context keyword after these */
			if (TOP->kw == KW_FOREACH)
				TOP->kw = TOP->old_kw;

			break;
		case KW_IF:
			if (terminated) {
				error(ctx, 0, "unexpected end of if statement; expected a condition");
				return;
			}
			TOP->last_code = code_append(ctx, KW_IF);
			TOP->last_code->payload.fc_if.code_then = code_new(KW_NOP);
			TOP->last_code->payload.fc_if.code_else = code_new(KW_NOP);
			loc_update();
			TOP->state = ST_PRECMD;
			/* prepare for reading a condition */
			push(ctx, KW_IF, ST_PREDATA, TOP->last_code);
			break;
		case KW_THEN:
			/* we are in an if context, right after reading a condition */
			if (TOP->kw != KW_IF) {
				error(ctx, 0, "unexpected 'then' - must be in an 'if' after the condition");
				return;
			}
			bind_if_cond:;
			TOP->last_code->payload.fc_if.cond = TOP->argv[0];
			loc_update();
			arg_remove(ctx);
			push(ctx, KW_THEN, ST_PRECMD, TOP->last_code->payload.fc_if.code_then);
			break;
		case KW_ELSE:
			/* we are in an if context, after and end  */
			if (TOP->kw != KW_THEN) {
				error(ctx, 0, "unexpected 'else' - must be in a 'then' block before an else");
				return;
			}
			pop(ctx); /* that was the then branch */
			push(ctx, KW_ELSE, ST_PRECMD, TOP->last_code->payload.fc_if.code_else);
			break;
		case KW_FOREACH:
			if (terminated) {
				error(ctx, 0, "unexpected end of if foreach statement; expected an address");
				return;
			}
			TOP->last_code = code_append(ctx, KW_FOREACH);
			loc_update();
			TOP->state = ST_PREDATA;
			TOP->old_kw = TOP->kw;
			TOP->kw = KW_FOREACH;
			break;
		case KW_IN:
			if (TOP->kw != KW_FOREACH)
				error(ctx, 0, "unexpected \"in\"; should be after the address in foreach");
			else
				push(ctx, KW_IN, ST_PREDATA, NULL);
			break;
		case KW_SWITCH:
			if (terminated) {
				error(ctx, 0, "unexpected end of if switch statement; expected a data");
				return;
			}
			TOP->last_code = code_append(ctx, KW_SWITCH);
			TOP->state = ST_PREDATA;
			TOP->old_kw = TOP->kw;
			TOP->kw = KW_SWITCH;
			loc_update();
			break;
		case KW_CASE:
		case KW_DEFAULT:
			if (TOP->kw == KW_SWITCH) {
				tmpasm_case_t *c;
				c = malloc(sizeof(tmpasm_case_t));
				c->body = code_new(KW_NOP);
				c->data = NULL;
				c->next = NULL;
				if (TOP->last_code->payload.fc_switch.last == NULL) {
					TOP->last_code->payload.fc_switch.first = c;
					TOP->last_code->payload.fc_switch.last = c;
				}
				else {
					TOP->last_code->payload.fc_switch.last->next = c;
					TOP->last_code->payload.fc_switch.last = c;
				}
				if (kw == KW_DEFAULT) {
					push(ctx, KW_DEFAULT, ST_PRECMD, c->body);
					push(ctx, KW_none, ST_PRECMD, c->body);
					c->data = NULL;
				}
				else
					push(ctx, KW_CASE, ST_PREDATA, c->body);
			}
			else
				error(ctx, 0, "unexpected \"case\" or \"default\"; should be in a switch (is the last case terminated by an \"end\"?)");
			break;
	default:
		TOP->last_code = code_append(ctx, KW_none);
		TOP->last_code->payload.instr.call_name = strclone(TOP->cmd_buff);
		if (TOP->last_code->payload.instr.call_name != NULL) {
			TOP->last_code->payload.instr.call = ctx->cb->resolve(ctx, TOP->last_code->payload.instr.call_name);
			loc_update();
		}
		if (terminated)
			TOP->state = ST_PRECMD;
		else
			TOP->state = ST_PREDATA;
	}
}

static void comment_start(tmpasm_t *ctx)
{
	push(ctx, KW_none, ST_COMMENT, NULL);
}

int tmpasm_gotchar(tmpasm_t *ctx, char c)
{
	if (ctx->dead)
		return -1;
	switch(TOP->state) {
		case ST_COMMENT:
			if ((c == '\n') || (c == '\r')) {
				pop(ctx);
				if (TOP->state == ST_PREDATA)
					end_of_statement(ctx);
			}
			break;
		case ST_PRECMD:
			if (c == '#') {
				comment_start(ctx);
				break;
			}
			if (is_space(c) || is_sep(c))
				break;
			TOP->cmdi = 0;
			TOP->state = ST_CMD;
			TOP->kwline = ctx->line;
			TOP->kwcol = ctx->col;
			/* fall thru */
		case ST_CMD:
			/* end of command or keyword */
			if (is_space(c) || is_sep(c)) {
				TOP->cmd_buff[TOP->cmdi] = '\0';
				got_kw(ctx, kw_lookup(TOP->cmd_buff), is_sep(c));
			}
			else {
				TOP->cmd_buff[TOP->cmdi] = c;
				TOP->cmdi++;
				if (TOP->cmdi >= sizeof(TOP->cmd_buff))
					error(ctx, 0, "keyword or instruction name is too long");
			}
			break;
		case ST_PREDATA:
			if (c == '#') {
				comment_start(ctx);
				break;
			}
			if (is_space(c))
				break;
			if (is_sep(c))
				end_of_statement(ctx);
			else if (c == '{') {
				TOP->state = ST_STRING;
				arg_new(ctx, 0);
			}
			else if (c == '[') {
				TOP->state = ST_PREBLOCKSEP;
				arg_new(ctx, 0);
			}
			else if (is_addr(c)) {
				TOP->state = ST_ADDRESS;
				arg_new(ctx, 1);
				arg_append(ctx, c);
			}
			else
				error(ctx, c, "unexpected character; expected '{' for starting a string or an address");
			break;
		case ST_PREBLOCKSEP:
			TOP->block_sep = c;
			TOP->state = ST_BLOCK;
			break;
		case ST_BLOCK:
			if (c == TOP->block_sep)
				TOP->state = ST_BLOCKSEP;
			else
				arg_append(ctx, c);
			break;
		case ST_BLOCKSEP:
			if (c != ']') {
				arg_new_next(ctx, 1);
				arg_append(ctx, c);
				TOP->state = ST_BLOCK_INLINE;
			}
			else
				arg_end(ctx, 1);
			break;
		case ST_BLOCK_INLINE:
			if (c == TOP->block_sep) {
				arg_new_next(ctx, 0);
				TOP->state = ST_BLOCK;
			}
			else
				arg_append(ctx, c);
			break;
		case ST_STRING:
			if (c == '}')
				arg_end(ctx, 1);
			else if (c == '\\')
				TOP->state = ST_STRING_ESCAPE;
			else
				arg_append(ctx, c);
			break;
		case ST_STRING_ESCAPE:
			{
			char co;
			switch(c) {
				case 'n':  co = '\n'; break;
				case 'r':  co = '\r'; break;
				case 't':  co = '\t'; break;
				case '\\': co = '\\'; break;
				case 'o':  co = '{';  break;
				case 'c':  co = '}';  break;
				default:   co = c;
			}
			arg_append(ctx, co);
			TOP->state = ST_STRING;
			}
			break;
		case ST_ADDRESS:
			if (is_space(c))
				arg_end(ctx, 1);
			else if (is_sep(c)) {
				arg_end(ctx, 1);
				end_of_statement(ctx);
			}
			else if (is_addr(c))
				arg_append(ctx, c);
			else
				error(ctx, c, "unexpected character; expected next character of the address");
			break;
	}
	if (c == '\n') {
		ctx->line++;
		ctx->col = 1;
	}
	else
		ctx->col++;
	return 0;
}

tmpasm_t *tmpasm_init(const tmpasm_cb_t *cb)
{
	tmpasm_t *ctx;
	ctx = calloc(sizeof(tmpasm_t), 1);
	ctx->line = 1;
	ctx->col = 1;
	ctx->code = code_new(KW_NOP);
	ctx->cb = cb;
	push(ctx, KW_none, ST_PRECMD, ctx->code);
	return ctx;
}

static void free_exec(tmpasm_exec_t *e)
{
	int n;
	tmpasm_case_t *c, *c_next;
	tmpasm_exec_t *e_next;

	for(; e != NULL; e = e_next) {
		e_next = e->next;
		switch(e->kw) {
			case KW_none:
				if (e->payload.instr.call_name != NULL)
					free(e->payload.instr.call_name);
				for(n = 0; n < e->payload.instr.argc; n++)
					arg_free(e->payload.instr.argv[n]);
				free(e->payload.instr.argv);
				break;
			case KW_IF:
				arg_free(e->payload.fc_if.cond);
				free_exec(e->payload.fc_if.code_then);
				free_exec(e->payload.fc_if.code_else);
				break;
			case KW_FOREACH:
				free(e->payload.fc_foreach.loop_var);
				arg_free(e->payload.fc_foreach.data);
				free_exec(e->payload.fc_foreach.code_body);
				break;
			case KW_SWITCH:
				arg_free(e->payload.fc_switch.cond);
				for(c = e->payload.fc_switch.first; c != NULL; c = c_next) {
					c_next = c->next;
					if (c->data != NULL)
						arg_free(c->data);
					free_exec(c->body);
					free(c);
				}
				break;
			default:;
		}
		free(e);
	}
}

void tmpasm_uninit(tmpasm_t *ctx)
{
	free_exec(ctx->code);
	while (ctx->st != NULL)
		pop_(ctx, 0);
	if (ctx->runtime_error_data != NULL)
		free(ctx->runtime_error_data);
	free(ctx);
}

/****************** runtime ********************/

static const char *tmpasm_runtime_error_fmts[] = {
	"success %s",
	"variable '%s' does not exist",
	"empty argument (broken AST)%s",
	"compilation error: control block without an \"end\"; premature end of script%s",
	"attempt to call unresolved instruction '%s'",
	NULL
};

void tmpasm_runtime_error(tmpasm_t *ctx, int code, const char *data)
{
	ctx->runtime_error = code;
	if (ctx->runtime_error_data != NULL)
		free(ctx->runtime_error_data);
	ctx->runtime_error_data = strclone(data);
	if (ctx->executing != NULL) {
		ctx->runtime_error_line = ctx->executing->line;
		ctx->runtime_error_col  = ctx->executing->col;
	}
	else {
		ctx->runtime_error_line = 0;
		ctx->runtime_error_col  = 0;
	}
}

const char *tmpasm_runtime_error_fmt(tmpasm_t *ctx)
{
	if (ctx->runtime_error == 0)
		return NULL;
	if ((ctx->runtime_error < 0) && (ctx->cb->runtime_error_fmt != NULL)) {
		const char *fmt;
		fmt = ctx->cb->runtime_error_fmt(ctx);
		if (fmt != NULL)
			return fmt;
	}
	if ((ctx->runtime_error < 0) || ((size_t)ctx->runtime_error > (sizeof(tmpasm_runtime_error_fmts)/sizeof(char *))))
		return "invalid error code %s";
	return tmpasm_runtime_error_fmts[ctx->runtime_error];
}

char *tmpasm_arg2str(tmpasm_t *ctx, tmpasm_arg_t *a, int keep_addr)
{
	if (a == NULL) {
		tmpasm_runtime_error(ctx, 2, NULL);
		return strclone("");
	}
	if (a->next != NULL) {
		/* block mode */
		int alloced = 0, used = 0;
		char *s = NULL;
		const char *i;

		for(;a != NULL; a = a->next) {
			int l;
			if (a->is_addr) {
				i = ctx->cb->get(ctx, a->data);
				if (i == NULL) {
					i = "";
					tmpasm_runtime_error(ctx, 1, strclone(a->data));
				}
			}
			else
				i = a->data;
			l = strlen(i);
			if (used + l >= alloced) {
				alloced = used + l + 256;
				s = realloc(s, alloced);
			}
			memcpy(s+used, i, l);
			used += l;
		}
		s[used] = '\0';
		return s;
	}

	/* non-block */
	if (a->is_addr) {
		const char *i;
		if (keep_addr)
			i = a->data;
		else
			i = ctx->cb->get(ctx, a->data);
		if (i == NULL) {
			i = "";
			tmpasm_runtime_error(ctx, 1, strclone(a->data));
		}
		return strclone(i);
	}

	return strclone(a->data);
}

static void execute(tmpasm_t *ctx, tmpasm_exec_t *e)
{
	tmpasm_case_t *c;
	void *state;
	char *cond, *list;
	const char *i;

	while((e != NULL) && (ctx->runtime_error == 0) && (ctx->halt == 0)) {
		ctx->executing = e;
		switch(e->kw) {
			case KW_none:
				if (e->payload.instr.call != NULL)
					e->payload.instr.call(ctx, e->payload.instr.call_name, e->payload.instr.argc, e->payload.instr.argv);
				else
					tmpasm_runtime_error(ctx, 4, e->payload.instr.call_name);
				break;
			case KW_IF:
					cond = tmpasm_arg2str(ctx, e->payload.fc_if.cond, 0);
					if (ctx->cb->is_true(ctx, cond))
						execute(ctx, e->payload.fc_if.code_then);
					else
						execute(ctx, e->payload.fc_if.code_else);
					free(cond);
				break;
			case KW_FOREACH:
				list = tmpasm_arg2str(ctx, e->payload.fc_foreach.data, 0);
				for(i = ctx->cb->first(ctx, &state, list); i != NULL; i = ctx->cb->next(ctx, &state)) {
					ctx->cb->set(ctx, e->payload.fc_foreach.loop_var, i);
					execute(ctx, e->payload.fc_foreach.code_body);
				}
				free(list);
				break;
			case KW_SWITCH:
				cond = tmpasm_arg2str(ctx, e->payload.fc_switch.cond, 0);
				for(c = e->payload.fc_switch.first; c != NULL; c = c->next) {
					char *cv = NULL;
					if (c->data != NULL)
						cv = tmpasm_arg2str(ctx, c->data, 0);
					if ((c->data == NULL) || (ctx->cb->match(ctx, cond, cv))) {
						execute(ctx, c->body);
						if (cv != NULL)
							free(cv);
						break;
					}
					if (cv != NULL)
						free(cv);
				}
				free(cond);
				break;
			default:;
		}
	e = e->next;
	}
}

void tmpasm_execute(tmpasm_t *ctx)
{
	if (TOP->next != NULL) {
		ctx->executing = TOP->next->last_code;
		tmpasm_runtime_error(ctx, 3, NULL);
		return;
	}
	if ((TOP->state != ST_PRECMD) || (TOP->kw != KW_none)) {
		ctx->executing = TOP->last_code;
		tmpasm_runtime_error(ctx, 3, NULL);
		return;
	}
	ctx->halt = 0;
	ctx->runtime_error = 0;
	if (ctx->runtime_error_data != NULL) {
		free(ctx->runtime_error_data);
		ctx->runtime_error_data = NULL;
	}
	if (ctx->cb->preexec != NULL)
		ctx->cb->preexec(ctx);
	execute(ctx, ctx->code);
	if (ctx->cb->postexec != NULL)
		ctx->cb->postexec(ctx);
}

