/* Runs when a custom command line argument is found
 returns true if no further argument processing should be done */
int hook_custom_arg(const char *key, const char *value);

/* If any of the int hooks return non-zero, that means failure and stops
   the whole process */

/* Runs before anything else */
int hook_preinit(void);

/* Runs after initialization */
int hook_postinit(void);

/* Runs after all arguments are read and parsed */
int hook_postarg(void);

/* Runs when things should be detected for the host system (tools compiled for and/or run on compilation host) */
int hook_detect_host(void);

/* Runs when things should be detected for the target system (tools compiled on the compilation host but running on the target)*/
int hook_detect_target(void);

/* Runs when things should be detected for the runtime system (tools that will run only on the target, production runtime, not during compilation or installation) */
int hook_detect_runtime(void);

/* Runs after detection hooks, should generate the output (Makefiles, etc.) */
int hook_generate(void);

/* Runs before everything is uninitialized */
void hook_preuninit(void);

/* Runs at the very end, when everything is already uninitialized */
void hook_postuninit(void);
