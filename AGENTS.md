# Project: Xschem — C89 + Tcl/Tk Linux Desktop App, ASIC / VLSI schematic editor.

## Workspace Boundaries
- **Ignore `XSchemWin`, `tests/` and `scconfig/`:** Never search, read, list, or modify any files in these project directories.

## Overview & Layout
- **Purpose:** ASIC / VLSI schematic editor written in C and using Tcl-Tk as main scripting interface and UI.
- `src/` — All C files, awk helper programs, `xschem.tcl` is the main UI TCL file, yacc (`\*.y`) and flex (`\*.l`)
   files, `xschem.h` is the main header.

# Project Rules: C89 + Tcl/Tk Linux Xschem Desktop App

## Language, Compiler & Memory Standards
- Strict **ANSI C89 / ISO C90** (`gcc -std=c89 -pedantic -Wall -Wextra`).
- **No C99/C11 features**: Do NOT use `//` comments (`/* ... */` only), mixed declarations and code
  (declare all local variables at the top of a block), `for (int i = ...)` loop declarations,
 `<stdbool.h>`, `<stdint.h>`, or designated initializers.
- **Custom Memory Allocators (MANDATORY):** Always use `my_malloc`, `my_calloc`, `my_realloc`, `my_free`,
  `my_strdup`, and `my_strdup2` for all heap allocations/deallocations so debug tracking stays intact.
  **Never** call raw `malloc`, `calloc`, `realloc`, `free`, or `strdup` directly.
- Preserve existing indentation, brace placement, and naming conventions in edited files.

## Tcl/Tk Integration
- Use the classic **string-based Tcl C API** (`Tcl_CreateCommand`, `Tcl_SetResult`, `Tcl_AppendResult`, `Tcl_GetStringResult`) to match the existing codebase; do **not** introduce `Tcl_Obj`-based APIs (`Tcl_CreateObjCommand`, `Tcl_NewStringObj`, etc.).
- **Never** read or write `interp->result` directly—always use `Tcl_SetResult()`, `Tcl_AppendResult()`, or `Tcl_GetStringResult()` with proper memory disposal (`TCL_STATIC`, `TCL_VOLATILE`, `TCL_DYNAMIC`, or custom `Tcl_FreeProc`).
- Return `TCL_OK` or `TCL_ERROR` consistently and populate the interpreter result on errors.
- Keep GUI layout/widget logic in `.tcl` scripts and core logic/bindings in `.c` files unless instructed otherwise.

## Build & Workflow
- Build system: GNU `make -j4` at the project root.
- Do not scan the entire repository or modify unrelated files; edit only the specific `.c`, `.h`, or `.tcl` files
  referenced in the prompt.
- Never alter the `Makefile` unless explicitly asked.
