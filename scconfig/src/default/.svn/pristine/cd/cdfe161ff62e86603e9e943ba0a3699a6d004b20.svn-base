#include "ht.h"

int is_dep_known(const char *name);
int require(const char *name, int logdepth, int fatal);
const char *dep_add(const char *name, int (*finder)(const char *name, int logdepth, int fatal));
void require_all(int fatal);

/* Returns if dependency is a wildcard one (ending in / *) */
int is_dep_wild(const char *path);

/* Almost 'basename': returns the last portion of the path, which may
   be '*'. Returns "<unknown>" on error. */
const char *det_list_target(const char *path);

/* Returns 1 if the user asked for detecting a feature; needtodo is
   the first argument passed to the detection function (the target the caller
   wants to get detected), cando is the output path of the test that the
   detector could do next. */
int asked_for(const char *cando, const char *needtodo);

/* for internal use */
void dep_init(void);
void dep_uninit(void);

