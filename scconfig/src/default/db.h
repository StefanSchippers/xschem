#include "ht.h"


#define strue  "true"
#define sfalse "false"
#define istrue(s) ((s != NULL) && (*s == 't'))
#define isfalse(s) ((s != NULL) && (*s == 'f'))
/* the 3rd option is "unknown" */

/* accessors */
const char *get(const char *key);
const char *put(const char *key, const char *data);
void append(const char *key, const char *value);
char *concat_nodes(const char *prefix, ...);
int node_istrue(const char *key);


/* init/uninit */
void db_init(void);
void db_uninit(void);

/* export/import */
int export(const char *fn, int export_empty, const char *root);
int import(const char *fn);
int import_args(const char *key, const char *fn);

/* file system features */
extern char *db_cwd;
void db_cd(const char *path);
void db_mkdir(const char *path);
void db_link(const char *existing, const char *new);
void db_rmdir(const char *path);

extern ht_t *DBs;
#define iscross          (ht_get(DBs, "target") != ht_get(DBs, "host"))
#define in_cross_target  (iscross && (strcmp(db_cwd, "/target") == 0))
#define in_cross_host    (iscross && (strcmp(db_cwd, "/host") == 0))
