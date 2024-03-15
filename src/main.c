/* File: main.c
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2023 Stefan Frederik Schippers
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "xschem.h"
#ifdef __unix__
#include <sys/wait.h>
#endif
#include <locale.h>

/* can not install a child handler as the tcl-tk toolkit probably already uses it */
/* #define HANDLE_SIGCHLD */

static void sig_handler(int s){
  char emergency_prefix[PATH_MAX];
  const char *emergency_dir;

  if(s==SIGINT) {
    fprintf(errfp, "Use 'exit' to close the program\n");
    return;
  }

  if(xctx->undo_type == 0 ) { /* on disk undo */
    my_snprintf(emergency_prefix, S(emergency_prefix), "xschem_emergencysave_%s_",
             get_cell(xctx->sch[xctx->currsch], 0));
    if( !(emergency_dir = create_tmpdir(emergency_prefix)) ) {
      fprintf(errfp, "xinit(): problems creating emergency save dir\n");
      tcleval("exit 1");
    }
  
    if(rename(xctx->undo_dirname, emergency_dir)) {
      fprintf(errfp, "rename dir %s to %s failed\n", xctx->undo_dirname, emergency_dir);
    }
    fprintf(errfp, "EMERGENCY SAVE DIR: %s\n", emergency_dir);
  }




  fprintf(errfp, "\nFATAL: signal %d\n", s);
  fprintf(errfp, "while editing: %s\n", get_cell(xctx->sch[xctx->currsch], 0));
  exit(EXIT_FAILURE);
}

#ifdef HANDLE_SIGCHLD
static void child_handler(int signum)
{
  fprintf(errfp, "SIGCHLD received\n");
  #ifdef __unix__
  wait(NULL);
  #endif
}
#endif

int main(int argc, char **argv)
{
  int i;
  #ifdef __unix__
  int stdin_is_a_fifo = 0;
  struct stat statbuf;
  #endif
  Display *display;
  signal(SIGINT, sig_handler);
  signal(SIGSEGV, sig_handler);
  signal(SIGILL, sig_handler);
  signal(SIGTERM, sig_handler);
  signal(SIGFPE, sig_handler);

  #ifdef HANDLE_SIGCHLD
  signal(SIGCHLD, child_handler); /* avoid zombies 20180925 --> conflicts with tcl exec */
  #endif

  errfp=stderr;
  /* 20181013 check for empty or non existing DISPLAY *before* calling Tk_Main or Tcl_Main */
#ifdef __unix__
  if(!getenv("DISPLAY") || !getenv("DISPLAY")[0]) has_x=0;
  else {
    display = XOpenDisplay(NULL);
    if(!display) {
      has_x=0;
      fprintf(errfp, "\n   X server connection failed, although DISPLAY shell variable is set.\n"
                     "   A possible reason is that the X server is not running or DISPLAY shell variable\n"
                     "   is incorrectly set.\n"
                     "   Starting Xschem in text only mode.\n\n");
    } else XCloseDisplay(display);
  }
#endif
  argc = process_options(argc, argv);

  #ifdef __unix__
  /* if invoked in background (and not invoked from a command pipeline) detach from console */
  if(!fstat(0, &statbuf)) {
     setvbuf(stdout, NULL, _IOLBF, 0); /* set to line buffer mode */
     if(statbuf.st_mode & S_IFIFO) stdin_is_a_fifo = 1; /* input coming from a command pipe */
  }
  
  if(!stdin_is_a_fifo && getpgrp() != tcgetpgrp(STDOUT_FILENO) && !cli_opt_no_readline) {
    cli_opt_detach = 1;
  }
  #endif

  my_strdup(_ALLOC_ID_, &xschem_executable, argv[0]);
  if(debug_var>=1 && !has_x)
    fprintf(errfp, "main(): no DISPLAY set, assuming no X available\n");
  /* if cli_opt_detach is 1 no interactive command shell is created ...
   * using cli_opt_detach if no windowing exists (has_x == 0) is non sense so do nothing
   */



  cli_opt_argc = argc;
  cli_opt_argv = my_malloc(_ALLOC_ID_, cli_opt_argc * sizeof(char *));
  for(i = 0; i < cli_opt_argc; ++i) {
    cli_opt_argv[i] = NULL;
    my_strdup(_ALLOC_ID_, &cli_opt_argv[i], argv[i]);
  }


  if(cli_opt_detach) {
    fclose(stdin);
    #ifdef __unix__
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);
    #else
    freopen("nul", "w", stdout);
    freopen("nul", "w", stderr);
    #endif
  }
  if(cli_opt_detach && has_x) {
    Tcl_FindExecutable(argv[0]); /* tcl stores executable name for its internal usage */
    interp = Tcl_CreateInterp(); /* create the tcl interpreter */
    Tcl_AppInit(interp); /* execute our init function */
    Tk_MainLoop(); /* ok, now all done go into the event loop */
  } else { /* ... else start tcl or tk main loop and enter interactive mode (tcl shell) */
    if(has_x) Tk_Main(1, argv, Tcl_AppInit);
    else     Tcl_Main(1, argv, Tcl_AppInit);
  }
  return(0);
}

