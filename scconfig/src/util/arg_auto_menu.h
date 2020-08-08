#include "util/arg_auto_set.h"

/* An optional bridge between auto_set and menulib: create a submenu from an
   arg_auto_set_t table using regex key matching. */
void append_settings_auto_set(scm_menu_entry_t *me, int max, const arg_auto_set_t *as, const char *include, const char *exclude, const char *remove_prefix, int entry_type, void *entry_data);
