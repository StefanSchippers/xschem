#ifndef ARG_AUTO_SET_H
#define ARG_AUTO_SET_H
/*
	Handle a list of --disable-LIBs automatically.
	The user sets up a table like:
		static const arg_auto_set_t disable_libs[] = {
		{"disable-gtk",       "libs/gui/gtk2",         arg_lib_nodes},
		{"disable-lesstif",   "libs/gui/lesstif2",     arg_lib_nodes},
		{NULL, NULL, NULL}
	};
	and at the end of hook_custom_arg() executes:
	return arg_auto_set(key, value, disable_libs);

	The call will set all nodes listed in arg_lib_nodes to disable gtk or
	lesstif.

	Mechanism: this happens before require()s on these nodes; require() will
	find them already set and won't run the detection. Thus it is suitable for
	disabling features (but not for enabling them).
*/

/* A table of node name-values to be set under a subtree */
typedef struct {
	const char *name;
	const char *value;
} arg_auto_set_node_t;

/* A table of argument->subtree->subtree_values */
typedef struct {
	const char *arg_key;                  /* command line argument without the -- prefix */
	const char *subtree;                  /* subtree path affected, e.g. libs/gui/gtk2 */
	const arg_auto_set_node_t *subvals;   /* a set of values to be put() under the subtree */
	const char *help_txt;
} arg_auto_set_t;

/* node set table for resetting the usual nodes under a library subtree:
   presents, cflags, ldflags */
extern const arg_auto_set_node_t arg_lib_nodes[];

/* set the node true or false */
extern const arg_auto_set_node_t arg_true[];
extern const arg_auto_set_node_t arg_false[];

/* Execute table: find a match on key and set all subvals of the match */
int arg_auto_set(const char *key, const char *value, const arg_auto_set_t *table);

/* Print options help from the table, one entry per line; if help text starts
   with $, replace that with --arg_key and insert padding after that; padding
   should be a string filled with spaces, as long as the longest argument key
   plus the separator spaces. */
void arg_auto_print_options(FILE *fout, const char *line_prefix, const char *padding, const arg_auto_set_t *table);

#endif
