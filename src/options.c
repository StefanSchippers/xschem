/* File: options.c
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
#define SHORT 1
#define LONG 2

static void check_opt(char *opt, char *optval, int type)
{
    if( (type == SHORT && *opt == 'd') || (type == LONG && !strcmp("debug", opt)) ) {
        if(optval) debug_var=atoi(optval);
        else debug_var = 0;

    } else if( (type == SHORT && *opt == 'S') || (type == LONG && !strcmp("simulate", opt)) ) {
        dbg(1, "process_options(): will do simulation\n");
        cli_opt_do_simulation=1;

    } else if( (type == SHORT && *opt == 'W') || (type == LONG && !strcmp("waves", opt)) ) {
        dbg(1, "process_options(): will show waves\n");
        cli_opt_do_waves=1;

    } else if( (type == SHORT && *opt == 'n') || (type == LONG && !strcmp("netlist", opt)) ) {
        dbg(1, "process_options(): will do netlist\n");
        cli_opt_do_netlist=1;

    } else if( (type == SHORT && *opt == 'f') || (type == LONG && !strcmp("flat_netlist", opt)) ) {
        dbg(1, "process_options(): set flat netlist\n");
        cli_opt_flat_netlist=1;

    } else if( (type == SHORT && *opt == 'r') || (type == LONG && !strcmp("no_readline", opt)) ) {
        cli_opt_no_readline=1;

    } else if( (type == SHORT && *opt == 'p') || (type == LONG && !strcmp("postscript", opt)) ) {
        dbg(1, "process_options(): will print postscript/pdf\n");
        cli_opt_do_print=1;

    } else if( (type == LONG && !strcmp("pdf", opt)) ) {
        dbg(1, "process_options(): will print postscript/pdf\n");
        cli_opt_do_print=1;

    } else if( (type == LONG && !strcmp("plotfile", opt)) ) {
        dbg(1, "process_options(): user plotfile specified: %s\n", optval ? optval : "NULL");
        if(optval) my_strncpy(cli_opt_plotfile, optval, S(cli_opt_plotfile));

    } else if( (type == LONG && !strcmp("rcfile", opt)) ) {
        dbg(1, "process_options(): user rcfile specified: %s\n", optval ? optval : "NULL");
        if(optval) my_strncpy(cli_opt_rcfile, optval, S(cli_opt_rcfile));

    } else if( (type == LONG && !strcmp("png", opt)) ) {
        dbg(1, "process_options(): will print png\n");
        cli_opt_do_print=2;

    } else if( (type == LONG && !strcmp("preinit", opt)) ) {
        dbg(1, "process_options(): passing tcl command to interpreter: %s\n", optval);
        if(optval) my_strdup(_ALLOC_ID_, &cli_opt_preinit_command, optval);

    } else if( (type == LONG && !strcmp("tcl", opt)) ) {
        dbg(1, "process_options(): passing tcl command to interpreter: %s\n", optval);
        if(optval) my_strdup(_ALLOC_ID_, &cli_opt_tcl_command, optval);

    } else if( (type == LONG && !strcmp("script", opt)) ) {
        dbg(1, "process_options(): passing tcl script file  to interpreter: %s\n", optval);
        if(optval) my_strncpy(cli_opt_tcl_script, optval, S(cli_opt_tcl_script));

    } else if( (type == LONG && !strcmp("command", opt)) ) {
        dbg(1, "process_options(): passing tcl command to interpreter: %s\n", optval);
        if(optval)  my_strdup(_ALLOC_ID_, &cli_opt_tcl_post_command, optval);

    } else if( (type == LONG && !strcmp("diff", opt)) ) {
        dbg(1, "process_options(): diff with: %s\n", optval);
        if(optval) my_strncpy(cli_opt_diff, optval, S(cli_opt_diff));

    } else if( (type == LONG && !strcmp("tcp_port", opt)) ) {
        dbg(1, "process_options(): setting tcp port: %s\n", optval);
        if(optval) tcp_port=atoi(optval);
        else tcp_port = 0;

    } else if( (type == LONG && !strcmp("svg", opt)) ) {
        dbg(1, "process_options(): will print png\n");
        cli_opt_do_print=3;

    } else if( (type == SHORT && *opt == 'c') || (type == LONG && !strcmp("color_ps", opt)) ) {
        dbg(1, "process_options(): set color postscript\n");
        color_ps=1;

    } else if( (type == SHORT && *opt == 'i') || (type == LONG && !strcmp("no_rcload", opt)) ) {
        cli_opt_load_initfile=0;

    } else if( (type == SHORT && *opt == 'l') || (type == LONG && !strcmp("log", opt)) ) {
        if(optval) {
          errfp = fopen(optval, "w");
          setvbuf(errfp, NULL, _IOLBF, 0); /* line (_IOLBF) or disable (_IONBF) buffering on error log */
          if(!errfp) {
            errfp=stderr;
            dbg(0, "Problems opening log file: %s\n", optval);
          }
        }
    } else if( (type == SHORT && *opt == 'o') || (type == LONG && !strcmp("netlist_path", opt)) ) {
        if(optval) my_strncpy(cli_opt_netlist_dir, optval, S(cli_opt_netlist_dir));

    } else if( (type == SHORT && *opt == 'N') || (type == LONG && !strcmp("netlist_filename", opt)) ) {
        dbg(1, "process_options(): set netlist name to %s\n", optval);
        if(optval) my_strncpy(cli_opt_initial_netlist_name, optval, S(cli_opt_initial_netlist_name));

    } else if( (type == SHORT && *opt == 's') || (type == LONG && !strcmp("spice", opt)) ) {
        dbg(1, "process_options(): set netlist type to spice\n");
        cli_opt_netlist_type=CAD_SPICE_NETLIST;

    } else if( (type == SHORT && *opt == 'y') || (type == LONG && !strcmp("symbol", opt)) ) {
        dbg(1, "process_options(): set netlist type to symbol\n");
        cli_opt_netlist_type=CAD_SYMBOL_ATTRS;

    } else if( (type == SHORT && *opt == 'V') || (type == LONG && !strcmp("vhdl", opt)) ) {
        dbg(1, "process_options(): set netlist type to vhdl\n");
        cli_opt_netlist_type=CAD_VHDL_NETLIST;

    } else if( (type == SHORT && *opt == 'w') || (type == LONG && !strcmp("verilog", opt)) ) {
        dbg(1, "process_options(): set netlist type to verilog\n");
        cli_opt_netlist_type=CAD_VERILOG_NETLIST;

    } else if( (type == SHORT && *opt == 'b') || (type == LONG && !strcmp("detach", opt)) ) {
        detach = 1;

    } else if( (type == SHORT && *opt == 'v') || (type == LONG && !strcmp("version", opt)) ) {
        print_version();

    } else if( (type == SHORT && *opt == 't') || (type == LONG && !strcmp("tedax", opt)) ) {
        dbg(1, "process_options(): set netlist type to tEDAx\n");
        cli_opt_netlist_type=CAD_TEDAX_NETLIST;

    } else if( (type == SHORT && *opt == 'q') || (type == LONG && !strcmp("quit", opt)) ) {
        quit=1;

    } else if( (type == SHORT && *opt == 'x') || (type == LONG && !strcmp("no_x", opt)) ) {
        has_x=0;

    } else if( (type == SHORT && *opt == 'z') || (type == LONG && !strcmp("rainbow", opt)) ) {
        rainbow_colors=1;

    } else if( (type == SHORT && *opt == 'h') || (type == LONG && !strcmp("help", opt)) ) {
        help=1;

    } else {
        fprintf(errfp, "Unknown option: %s\n", opt);
    }
}

int process_options(int argc, char *argv[])
{
  int i, arg_cnt;
  char *opt, *optval;

  for(arg_cnt = i = 1; i < argc; ++i) {
    opt = argv[i];
    if(*opt == '-') { /* options */
      ++opt;
      if(opt && *opt=='-') { /* long options */
        ++opt;
        if(*opt) {
          optval = strchr(opt, '=');
          if(optval) {
            *optval = '\0';
            ++optval;
          }
          if(!optval && i < argc-1 && argv[i+1][0] != '-') {
            /* options requiring arguments are listed here */
            if(!strcmp("debug", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("tcl", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("preinit", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("script", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("command", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("diff", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("tcp_port", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("netlist_filename", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("log", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("netlist_path", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("rcfile", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("plotfile", opt)) {
              optval = argv[++i];
            }
          }
          check_opt(opt, optval, LONG);
          /* printf("long opt:%s, value: %s\n", opt, optval ? optval : "no value"); */
        }
      }
      else { /* short options */
        while(*opt) {
          optval = NULL;
          /* options requiring arguments are listed here */
          if(*opt == 'N') { /* output top netlist file name in netlist_dir */
            optval = argv[++i];
          }
          if(*opt == 'l') {
            optval = argv[++i];
          }
          else if(*opt == 'd') {
            optval = argv[++i];
          }
          else if(*opt == 'o') {
            optval = argv[++i];
          }
          check_opt(opt, optval, SHORT);
          /* printf("opt: %c, value: %s\n", *opt, optval ? optval : "no value"); */
          ++opt;
        }
      }
    } else { /* arguments */
      /* printf("argument: %s\n", opt); */
      argv[arg_cnt++] = opt;
    }
  }
  if (arg_cnt > 1) {
    dbg(1, "process_option(): file name given: %s\n",argv[1]);
    my_strncpy(cli_opt_filename, argv[1], S(cli_opt_filename));
#ifndef __unix__
    change_to_unix_fn(cli_opt_filename);
#endif
  }
  return arg_cnt;
}

