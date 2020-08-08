#include <stdlib.h>
#include <string.h>
#include "regex.h"
#include "libs.h"
#include "db.h"
#include "menulib/scmenu.h"
#include "arg_auto_menu.h"

static const char *get_entry_val(const arg_auto_set_t *as)
{
	char *path;
	const char *v, *res;

	if (as->subvals == arg_lib_nodes) {
		path = str_concat("/", as->subtree, "presents", NULL);
		v = get(path);
		if ((v != NULL) && (strcmp(v, sfalse) == 0))
			res = "disable";
		else
			res = "enable";
		free(path);
		return res;
	}
}

#define next_word(curr, next) \
	do { \
		next = strchr(curr, '|'); \
		if (next != NULL) { \
			*next = '\0'; \
			next++; \
		} \
	} while(0)

void append_settings_auto_set(scm_menu_entry_t *me, int max, const arg_auto_set_t *as, const char *include, const char *exclude, const char *remove_prefix, int entry_type, void *entry_data)
{
	const arg_auto_set_t *a;
	scm_menu_entry_t *e;
	int numa, n, ei;
	char *accept;

	/* count number of all settings, allocate accept[] */
	numa = 0;
	for(a = as; a->arg_key != NULL; a++)
		numa++;
	accept = calloc(numa, 1);

	/* mark entries included in accept[] */
	if (include != NULL) {
		char *all = strclone(include), *next = all, *curr = all;
		do {
			next_word(curr, next);
			re_comp(curr);
			for(a = as, n = 0; a->arg_key != NULL; a++,n++)
				if (re_exec(a->arg_key))
					accept[n] = 1;
			curr = next;
		} while((next != NULL) && (*next != '\0'));
		free(all);
	}
	else
		memset(accept, 1, numa);

	/* mark entries excluded in accept[] */
	if (exclude != NULL) {
		char *all = strclone(exclude), *next = all, *curr = all;
		do {
			next_word(curr, next);
			re_comp(curr);
			for(a = as, n = 0; a->arg_key != NULL; a++,n++)
				if (re_exec(a->arg_key))
					accept[n] = 0;
			curr = next;
		} while((next != NULL) && (*next != '\0'));
		free(all);
	}

	/* find the terminator */
	for(e = me, ei = 0; e->key != SCM_TERMINATOR; e++, ei++) ;

	re_comp(remove_prefix);
	printf("exclude:\n");
	for(n = 0; n < numa; n++) {
		if (accept[n]) {
			char *sd;
			if (re_exec(as[n].arg_key))
				sd = re_subs_dup("");
			else
				sd = strclone(as[n].arg_key);

			e->type = entry_type;
			e->key = sd;
			e->value = get_entry_val(&as[n]);
			e->user_data = &as[n];
			e->auto_data = entry_data;
			e++;
			ei++;
			if (ei > max - 2)
				break;
		}
	}

	e->type = SCM_TERMINATOR;

	free(accept);
}
