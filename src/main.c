/* File: main.c
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2020 Stefan Frederik Schippers
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

void sig_handler(int s){
#ifndef IN_MEMORY_UNDO
  char emergency_prefix[PATH_MAX];
  const char *emergency_dir;
#endif


  if(s==SIGINT) {
    fprintf(errfp, "Use 'exit' to close the program\n");
    return;
  }

#ifndef IN_MEMORY_UNDO
  /* 20180923 no more mkdtemp */
  my_snprintf(emergency_prefix, S(emergency_prefix), "xschem_emergencysave_%s_",
           skip_dir(xctx.sch[xctx.currsch]));
  if( !(emergency_dir = create_tmpdir(emergency_prefix)) ) {
    fprintf(errfp, "xinit(): problems creating emergency save dir\n");
    /* tcleval( "exit"); */
    tcleval("exit");
  }

  if(rename(undo_dirname, emergency_dir)) {
    fprintf(errfp, "rename dir %s to %s failed\n", undo_dirname, emergency_dir);
  }
  fprintf(errfp, "EMERGENCY SAVE DIR: %s\n", emergency_dir);
#endif
  fprintf(errfp, "\nFATAL: signal %d\n", s);
  fprintf(errfp, "while editing: %s\n", skip_dir(xctx.sch[xctx.currsch]));
  exit(EXIT_FAILURE);
}

void child_handler(int signum)
{
    /* fprintf(errfp, "SIGCHLD received\n"); */
#ifdef __unix__
    wait(NULL);
#endif
}

int main(int argc, char **argv)
{
  my_strdup(45, &xschem_executable, argv[0]);
  signal(SIGINT, sig_handler);
  signal(SIGSEGV, sig_handler);
  signal(SIGILL, sig_handler);
  signal(SIGTERM, sig_handler);
  signal(SIGFPE, sig_handler);
  /* signal(SIGCHLD, child_handler); */  /* avoid zombies 20180925 --> conflicts with tcl exec */

  errfp=stderr;
  /* 20181013 check for empty or non existing DISPLAY *before* calling Tk_Main or Tcl_Main */
#ifdef __unix__
  if(!getenv("DISPLAY") || !getenv("DISPLAY")[0]) has_x=0;
#endif
  process_options(argc, argv);
  if(debug_var>=1 && !has_x)
    fprintf(errfp, "main(): no DISPLAY set, assuming no X available\n");

/* detach from console (fork a child and close std file descriptors) */
#ifdef __unix__
  if(batch_mode) {
    pid_t pid = fork();
    if(pid < 0) {
      fprintf(errfp, "main(): fork() failed\n");
      exit(EXIT_FAILURE);
    }
    if(pid == 0) {
      /* The child becomes the daemon. */
      /* Detach all standard I/O descriptors */
      close(0); /* stdin */
      close(1); /* stdout */
      close(2); /* stderr */
      setsid(); /* new session */
      /* Ok, now detached */
    }
    else {
      /* terminate parent */
      exit(0);
    }
  }
#endif

  if(has_x) Tk_Main(1, argv, Tcl_AppInit);
  else     Tcl_Main(1, argv, Tcl_AppInit);
  return 0;
}

