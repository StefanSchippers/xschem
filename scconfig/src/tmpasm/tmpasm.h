#ifndef TMPASM_H
#define TMPASM_H
#ifndef TMPASM_INSTR_MAXLEN
#define TMPASM_INSTR_MAXLEN 32
#endif

typedef struct tmpasm_s tmpasm_t;

typedef struct tmpasm_arg_s tmpasm_arg_t;

struct tmpasm_arg_s {
	tmpasm_arg_t *next;    /* block: the resulting string is a list of strings and addresses */
	char is_addr;          /* 1: arg is a node address; 0: arg is a string immediate */
	char data[1];          /* arg string - obviously longer than 1 char (but there's not special hack for that in C89), \0 terminated */
};

/* user specified instruction prototype */
typedef void tmpasm_instr(tmpasm_t *ctx, char *iname, int argc, tmpasm_arg_t *argv[]);


typedef struct tmpasm_cb_s {
	/* return the value of a node at addr - NULL is an error */
	const char *(*get)(tmpasm_t *ctx, const char *addr);

	/* set the value of a node at addr to data; data may be NULL */
	void (*set)(tmpasm_t *ctx, const char *addr, const char *data);

	/* return 1 if data is true, 0 otherwise; data may be NULL (if an unknown variable is referenced) */
	int (*is_true)(tmpasm_t *ctx, const char *data);

	/* return 1 if str matches pat, 0 otherwise; str and pat may be NULL */
	int (*match)(tmpasm_t *ctx, const char *str, const char *pat);

	/* first iteration over list; return the first element (or NULL to end); the string returned is not free'd by the caller */
	const char *(*first)(tmpasm_t *ctx, void **state, char *list);

	/* return next element of a list or NULL on end (in which case state shall be also free'd by the caller); the string returned is not free'd by the caller */
	const char *(*next)(tmpasm_t *ctx, void **state);

	/* resolve an instruction name to a function pointer */
	tmpasm_instr *(*resolve)(tmpasm_t *ctx, const char *name);

	/* optional: called once before execution of a context starts */
	void (*preexec)(tmpasm_t *ctx);

	/* optional: called once before execution of a context starts */
	void (*postexec)(tmpasm_t *ctx);

	/* optional: resolve the current runtime error, called only for negative
	   error codes; should return a format string with exactly one %s in it
	   or NULL. */
	const char *(*runtime_error_fmt)(tmpasm_t *ctx);
} tmpasm_cb_t;

int tmpasm_gotchar(tmpasm_t *ctx, char c);

tmpasm_t *tmpasm_init(const tmpasm_cb_t *cb);
void tmpasm_uninit(tmpasm_t *ctx);

/* return the string version of an arg in a newly malloc()'d string
   if keep_addr is non-zero and a is a single address, no get() is run
   but the address is returned as a string */
char *tmpasm_arg2str(tmpasm_t *ctx, tmpasm_arg_t *a, int keep_addr);

/* execute the code recursively until it exits */
void tmpasm_execute(tmpasm_t *ctx);

/* Set or get the runtime error of a context. 0 means no error, negative
   codes are user errors handled by the runtime_error_fmt() callback
   and positive codes are internal error. */
void tmpasm_runtime_error(tmpasm_t *ctx, int code, const char *data);
const char *tmpasm_runtime_error_fmt(tmpasm_t *ctx);

/* --- internals: not required for normal use --- */
typedef enum {
	ST_PRECMD,          /* waiting for a command to start - ignore whitespace */
	ST_CMD,
	ST_PREDATA,         /* waiting for data */
	ST_PREBLOCKSEP,     /* waiting for a block sep when opening a block */
	ST_BLOCKSEP,        /* found a block sep within the block - either an address or a termination follows */
	ST_BLOCK,           /* in [@ @] block, text part */
	ST_BLOCK_INLINE,    /* in [@ @] block, within inline @@ part */
	ST_STRING,          /* in {} string  */
	ST_STRING_ESCAPE,   /* in {} string, right after a \ */
	ST_ADDRESS,         /* shifting address bytes */
	ST_COMMENT          /* after #, until the next newline */
} tmpasm_state_t;

typedef enum {
	KW_none,
	KW_IF,
	KW_THEN,
	KW_ELSE,
	KW_END,
	KW_FOREACH,
	KW_IN,
	KW_SWITCH,
	KW_CASE,
	KW_DEFAULT,

	KW_NOP     /* virtual instruction */
} tmpasm_kw_t;

/* execution structs */
typedef struct tmpasm_exec_s tmpasm_exec_t;
typedef struct tmpasm_case_s tmpasm_case_t;

struct tmpasm_case_s {
	tmpasm_arg_t *data;
	tmpasm_exec_t *body;
	tmpasm_case_t *next;
};


struct tmpasm_exec_s {
	tmpasm_kw_t kw;        /* kw_none means a hook instruction */
	union {
		struct {      /* normal instruction */
			tmpasm_instr *call;
			char *call_name; /* temporary */
			int argc;
			tmpasm_arg_t **argv;
		} instr;
		struct {
			tmpasm_arg_t *cond;
			tmpasm_exec_t *code_then;
			tmpasm_exec_t *code_else;
		} fc_if;
		struct {
			char *loop_var;        /* must be a single address */
			tmpasm_arg_t *data;    /* what to loop in */
			tmpasm_exec_t *code_body;
		} fc_foreach;
		struct {
			tmpasm_arg_t *cond;
			tmpasm_case_t *first;
			tmpasm_case_t *last;
		} fc_switch;
	} payload;
	int line, col;
	tmpasm_exec_t *next;
};


/* parser structs */
typedef struct stack_s tmpasm_stack_t;
struct stack_s {
	tmpasm_state_t state;
/*	tmpasm_state_t kwstate; internal states of composite keywords like switch */
	char cmd_buff[TMPASM_INSTR_MAXLEN+1];
	unsigned int cmdi;
	tmpasm_kw_t kw, old_kw;
	char block_sep;
	int kwcol, kwline;

	int args_used, args_alloced; /* number of arguments in argv[] */

	tmpasm_arg_t **argv;   /* an array of linked lists */
	tmpasm_arg_t **argend; /* each argv[] is a linked list (for blocks); argend points to the tail */
	int *arg_alloced;      /* of argend */
	int *arg_used;         /* of argend */

	tmpasm_exec_t *last_code;     /* tail of the code list */

	tmpasm_stack_t *next;
};

struct tmpasm_s {
	tmpasm_stack_t *st;
	int dead;
	int col, line;
	tmpasm_exec_t *code;
	tmpasm_exec_t *executing; /* points to the code most recently executed (or being executed when in callbacks) */
	const tmpasm_cb_t *cb;
	int halt;
	int runtime_error;
	char *runtime_error_data;
	int runtime_error_line;
	int runtime_error_col;
	void *user_data;
};
#endif
