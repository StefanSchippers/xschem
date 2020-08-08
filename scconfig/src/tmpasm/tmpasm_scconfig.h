#include <stdio.h>
#include "tmpasm.h"

int tmpasm(const char *wdir, const char *input, const char *output);

void scc_tmpasm_parse(tmpasm_t *ctx, const char *cwd, FILE *fin, FILE *fout);

FILE *tmpasm_fout(tmpasm_t *ctx);

tmpasm_instr *scc_resolve(tmpasm_t *ctx, const char *name);

extern tmpasm_cb_t scc_cb;


