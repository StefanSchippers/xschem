/* File: scheduler.c
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2022 Stefan Frederik Schippers
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

void statusmsg(char str[],int n)
{
  tclsetvar("infowindow_text", str);
  if(!has_x) return;
  if(n==2) {
    dbg(3, "statusmsg(): n = 2, str = %s\n", str);
    tcleval("infowindow");
  }
  else {
    tclvareval(xctx->top_path, ".statusbar.1 configure -text $infowindow_text", NULL);
    dbg(3, "statusmsg(str, %d): -> $infowindow_text = %s\n", n, tclgetvar("infowindow_text"));
  }
}

static int get_instance(const char *s)
{
  int i, found=0;
  for(i=0;i<xctx->instances; ++i) {
    if(!strcmp(xctx->inst[i].instname, s)) {
      found=1;
      break;
    }
  }
  dbg(1, "get_instance(): found=%d, i=%d\n", found, i);
  if(!found) {
    if(!isonlydigit(s)) return -1;
    i=atoi(s);
  }
  if(i<0 || i>xctx->instances) {
    return -1;
  }
  return i;
}

static void xschem_cmd_help(int argc, const char **argv)
{
  
  char prog[PATH_MAX];
  if( get_file_path("x-www-browser")[0] == '/' ) goto done;
  if( get_file_path("firefox")[0] == '/' ) goto done;
  if( get_file_path("chromium")[0] == '/' ) goto done;
  if( get_file_path("chrome")[0] == '/' ) goto done;
  if( get_file_path("xdg-open")[0] == '/' ) goto done;
  done: 
  my_strncpy(prog, tclresult(), S(prog));
  tclvareval("launcher {", "file://", XSCHEM_SHAREDIR,
             "/../doc/xschem/xschem_man/developer_info.html#cmdref", "} ", prog, NULL);
  Tcl_ResetResult(interp);
}

/* can be used to reach C functions from the Tk shell. */
int xschem(ClientData clientdata, Tcl_Interp *interp, int argc, const char * argv[])
{
 int i;
 char name[1024]; /* overflow safe 20161122 */
 int cmd_found = 1;

 Tcl_ResetResult(interp);
 if(argc < 2) {
   Tcl_SetResult(interp, "Missing arguments.", TCL_STATIC);
   return TCL_ERROR;
 }
 if(debug_var>=2) {
   int i;
   fprintf(errfp, "xschem():");
   for(i=0; i<argc; ++i) {
     fprintf(errfp, "%s ", argv[i]);
   }
   fprintf(errfp, "\n");
 }
 /*
  * ********** xschem commands  IN SORTED ORDER !!! *********
  */
  switch(argv[1][0]) {
    case 'a': /*----------------------------------------------*/
    /* abort_operation
     *   Resets UI state, unselect all and abort any pending operation */
    if(!strcmp(argv[1], "abort_operation"))
    {
      abort_operation();
    }

    /* add_symbol_pin
     *   Start a GUI placement of a symbol pin */
    else if(!strcmp(argv[1], "add_symbol_pin"))
    {
      unselect_all(1);
      storeobject(-1, xctx->mousex_snap-2.5, xctx->mousey_snap-2.5, xctx->mousex_snap+2.5, xctx->mousey_snap+2.5,
                  xRECT, PINLAYER, SELECTED, "name=XXX\ndir=inout");
      xctx->need_reb_sel_arr=1;
      rebuild_selected_array();
      move_objects(START,0,0,0);
      xctx->ui_state |= START_SYMPIN;
      Tcl_ResetResult(interp);
    }

    /* add_graph
     *   Start a GUI placement of a graph object */
    else if(!strcmp(argv[1], "add_graph"))
    {
      unselect_all(1);
      xctx->graph_lastsel = xctx->rects[GRIDLAYER];
      storeobject(-1, xctx->mousex_snap-400, xctx->mousey_snap-200, xctx->mousex_snap+400, xctx->mousey_snap+200,
                  xRECT, GRIDLAYER, SELECTED,
          "flags=graph\n"
          "y1=0\n"
          "y2=2\n"
          "ypos1=0\n"
          "ypos2=2\n"
          "divy=5\n"
          "subdivy=1\n"
          "unity=1\n"
          "x1=0\n"
          "x2=10e-6\n"
          "divx=5\n"
          "subdivx=1\n"
          "node=\"\"\n"
          "color=\"\"\n"
          "dataset=-1\n"
          "unitx=1\n"
          "logx=0\n"
          "logy=0\n"
        );
      xctx->need_reb_sel_arr=1;
      rebuild_selected_array();
      move_objects(START,0,0,0);
      xctx->ui_state |= START_SYMPIN;
      Tcl_ResetResult(interp);
    }

    /* add_png
     *   Ask user to choose a png file and start a GUI placement of the image */
    else if(!strcmp(argv[1], "add_png"))
    {
      char str[PATH_MAX+100];
      unselect_all(1);
      tcleval("tk_getOpenFile -filetypes { {{Png} {.png}}   {{All files} *} }");
      if(tclresult()[0]) {
        my_snprintf(str, S(str), "flags=image,unscaled\nalpha=0.8\nimage=%s\n", tclresult());
        storeobject(-1, xctx->mousex_snap-100, xctx->mousey_snap-100, xctx->mousex_snap+100, xctx->mousey_snap+100,
                    xRECT, GRIDLAYER, SELECTED, str);
        xctx->need_reb_sel_arr=1;
        rebuild_selected_array();
        move_objects(START,0,0,0);
        xctx->ui_state |= START_SYMPIN;
      }
      Tcl_ResetResult(interp);
    }

    /* align
     *   Align currently selected objects to current snap setting */
    else if(!strcmp(argv[1], "align"))
    {
      xctx->push_undo();
      round_schematic_to_grid(tclgetdoublevar("cadsnap"));
      if(tclgetboolvar("autotrim_wires")) trim_wires();
      set_modify(1);
      xctx->prep_hash_inst=0;
      xctx->prep_hash_wires=0;
      xctx->prep_net_structs=0;
      xctx->prep_hi_structs=0;
      draw();
    }

    /* annotate_op [raw_file]
     *   Annotate operating point data into current schematic. 
     *   use <schematic name>.raw or use supplied argument as raw file to open
     *   look for operating point data and annotate voltages/currents into schematic */
    else if(!strcmp(argv[1], "annotate_op"))
    {
      int i;
      char f[PATH_MAX];
      if(argc > 2) {
        my_snprintf(f, S(f), "%s", argv[2]);
      } else {
        my_snprintf(f, S(f), "%s/%s.raw",  tclgetvar("netlist_dir"), skip_dir(xctx->sch[xctx->currsch]));
      }
      tclsetvar("rawfile_loaded", "0");
      free_rawfile(1);
      tcleval("array unset ngspice::ngspice_data");
      raw_read(f, "op");
      if(xctx->graph_values) {
        xctx->graph_annotate_p = 0;
        for(i = 0; i < xctx->graph_nvars; ++i) {
          char s[100];
          int p = 0;
          my_snprintf(s, S(s), "%.4g", xctx->graph_values[i][p]);
          dbg(1, "%s = %g\n", xctx->graph_names[i], xctx->graph_values[i][p]);
          tclvareval("array set ngspice::ngspice_data [list {",  xctx->graph_names[i], "} ", s, "]", NULL);
        }
        tclvareval("set ngspice::ngspice_data(n\\ vars) ", my_itoa( xctx->graph_nvars), NULL);
        tclvareval("set ngspice::ngspice_data(n\\ points) 1", NULL);
        draw();
      }
    }

    /* arc
     *   Start a GUI placement of an arc.
     *   User should click 3 unaligned points to define the arc */
    else if(!strcmp(argv[1], "arc"))
    {
      xctx->ui_state |= MENUSTARTARC;
    }

    /* attach_labels
     *   Attach net labels to selected component(s) instance(s) */
    else if(!strcmp(argv[1], "attach_labels"))
    {
      attach_labels_to_inst(0);
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'b': /*----------------------------------------------*/
    /* bbox begin|end
     *   Start/end bounding box calculation: parameter is either 'begin' or 'end' */
    if(!strcmp(argv[1], "bbox"))
    {
      if(argc > 2) {
        if(!strcmp(argv[2], "end")) {
          bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
          draw();
          bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
        } else if(!strcmp(argv[2], "begin")) {
          bbox(START,0.0, 0.0, 0.0, 0.0);
        }
      }
      Tcl_ResetResult(interp);
    }

    /* break_wires
     *   Break wires at selected instance pins */
    else if(!strcmp(argv[1], "break_wires"))
    {
      break_wires_at_pins();
      Tcl_ResetResult(interp);
    }

    /* build_colors
     *   Rebuild color palette using values of tcl vars dim_value and dim_bg */
    else if(!strcmp(argv[1], "build_colors"))
    {
      build_colors(tclgetdoublevar("dim_value"), tclgetdoublevar("dim_bg"));
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'c': /*----------------------------------------------*/
    /* callback winpath event mx my key button aux state
     *   Invoke the callback event dispatcher with a software event */
    if(!strcmp(argv[1], "callback") )
    {
      callback( argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), (KeySym)atol(argv[6]),
               atoi(argv[7]), atoi(argv[8]), atoi(argv[9]) );
      dbg(2, "callback %s %s %s %s %s %s %s %s\n",
          argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9]);
      Tcl_ResetResult(interp);
    }

    /* case_insensitive 1|0
     *   Set case insensitive symbol lookup. Use only on case insensitive filesystems */
    else if(!strcmp(argv[1], "case_insensitive"))
    {
      if(argc > 2) {
        int n = atoi(argv[2]);
        if(n) {
          xctx->case_insensitive = 1;
          xctx->x_strcmp = my_strcasecmp;
        } else {
          xctx->case_insensitive = 0;
          xctx->x_strcmp = strcmp;
        }
      }
    }

    /* check_symbols
     *   List all used symbols in current schematic and warn if some symbol is newer */
    else if(!strcmp(argv[1], "check_symbols"))
    {
      char sympath[PATH_MAX];
      const char *name;
      struct stat buf;
      for(i=0;i<xctx->symbols; ++i) {
        name = xctx->sym[i].name;
        if(!strcmp(xctx->file_version, "1.0")) {
          my_strncpy(sympath, abs_sym_path(name, ".sym"), S(sympath));
        } else {
          my_strncpy(sympath, abs_sym_path(name, ""), S(sympath));
        }
        if(!stat(sympath, &buf)) { /* file exists */
          if(xctx->time_last_modify < buf.st_mtime) {
            dbg(0, "Warning: symbol %s is newer than schematic\n", sympath);
          }
        } else { /* not found */
            dbg(0, "Warning: symbol %s not found\n", sympath);
        }
        dbg(0, "symbol %d: %s\n", i, sympath);
      }
      Tcl_ResetResult(interp);
    }

    /* check_unique_names [1|0]
     *   Check if all instances have a unique refdes (name attribute in xschem), 
     *   highlight such instances. If second parameter is '1' rename duplicates */
    else if(!strcmp(argv[1], "check_unique_names"))
    {
      if(argc > 2 && !strcmp(argv[2], "1")) {
        check_unique_names(1);
      } else {
        check_unique_names(0);
      }
      Tcl_ResetResult(interp);
    }

    /* circle
     *   Start a GUI placement of a circle.
     *   User should click 3 unaligned points to define the circle */
    else if(!strcmp(argv[1], "circle"))
    {
      xctx->ui_state |= MENUSTARTCIRCLE;
    }

    /* clear [force] [symbol|schematic]
     *   Clear current schematic window. Resets hierarchy level. Remove symbols
     *   the 'force' parameter will not ask to save existing modified schematic.
     *   the 'schematic' or 'symbol' parameter specifies to default to a schematic
     *   or symbol window (default: schematic) */
    else if(!strcmp(argv[1], "clear"))
    {
      int i, cancel = 1, symbol = 0;;

      for(i = 2; i < argc; i++) {
        if(!strcmp(argv[i], "force") ) cancel = 0;
        if(!strcmp(argv[i], "symbol")) symbol = 1;
      }
      if(cancel == 1) cancel=save(1);
      if(cancel != -1) { /* -1 means user cancel save request */
        char name[PATH_MAX];
        struct stat buf;
        int i;
        xctx->currsch = 0;
        unselect_all(1);
        remove_symbols();
        clear_drawing();
        if(symbol == 1) {
          xctx->netlist_type = CAD_SYMBOL_ATTRS;
          set_tcl_netlist_type();
          for(i=0;; ++i) { /* find a non-existent untitled[-n].sym */
            if(i == 0) my_snprintf(name, S(name), "%s.sym", "untitled");
            else my_snprintf(name, S(name), "%s-%d.sym", "untitled", i);
            if(stat(name, &buf)) break;
          }
          my_snprintf(xctx->sch[xctx->currsch], S(xctx->sch[xctx->currsch]), "%s/%s", pwd_dir, name);
          my_strncpy(xctx->current_name, name, S(xctx->current_name));
        } else {
          xctx->netlist_type = CAD_SPICE_NETLIST;
          set_tcl_netlist_type();
          for(i=0;; ++i) {
            if(i == 0) my_snprintf(name, S(name), "%s.sch", "untitled");
            else my_snprintf(name, S(name), "%s-%d.sch", "untitled", i);
            if(stat(name, &buf)) break;
          }
          my_snprintf(xctx->sch[xctx->currsch], S(xctx->sch[xctx->currsch]), "%s/%s", pwd_dir, name);
          my_strncpy(xctx->current_name, name, S(xctx->current_name));
        }
        draw();
        set_modify(0);
        xctx->prep_hash_inst=0;
        xctx->prep_hash_wires=0;
        xctx->prep_net_structs=0;
        xctx->prep_hi_structs=0;
        if(has_x) {
          set_modify(-1);
        }
      }
      Tcl_ResetResult(interp);
    }

    /* clear_drawing
     *   Clears drawing but does not purge symbols */
    else if(!strcmp(argv[1], "clear_drawing"))
    {
      if(argc==2) {
        unselect_all(1);
        clear_drawing();
      }
      Tcl_ResetResult(interp);
    }

    /* color_dim value
     *   Dim colors or brite colors depending on value parameter: -5 <= value <= 5 */
    else if(!strcmp(argv[1], "color_dim"))
    {
      if(argc > 2) {
        tclsetvar("dim_value", argv[2]);
        if(tclgetboolvar("enable_dim_bg") ) {
          tclsetvar("dim_bg", argv[2]);
        }
      }
      build_colors(tclgetdoublevar("dim_value"), tclgetdoublevar("dim_bg"));
      draw();
      Tcl_ResetResult(interp);
    }

    /* compare_schematics [sch_file]
     *   Compare currently loaded schematic with another 'sch_file' schematic.
     *   if no file is given prompt user to choose one */
    else if(!strcmp(argv[1], "compare_schematics"))
    {
      int ret = 0;
      if(argc > 2) {
        /* ret = compare_schematics(abs_sym_path(argv[2], "")); */
        ret = compare_schematics(argv[2]);
      }
      else {
        ret = compare_schematics(NULL); 
      }
      Tcl_SetResult(interp, my_itoa(ret), TCL_VOLATILE);
    }

    /* connected_nets [1|0]
     *   Selected nets connected to currently selected net or net label/pin.
     *   if '1' argument is given, stop at wire junctions */
    else if(!strcmp(argv[1], "connected_nets"))
    {
      int stop_at_junction = 0;
      if(argc > 2 && argv[2][0] == '1') stop_at_junction = 1;
      select_connected_wires(stop_at_junction);
      Tcl_ResetResult(interp);
    }

    /* copy
     *   Copy selection to clipboard */
    else if(!strcmp(argv[1], "copy"))
    {
      rebuild_selected_array();
      save_selection(2);
      Tcl_ResetResult(interp);
    }

    /* copy_objects
     *   Start a GUI copy operation */
    else if(!strcmp(argv[1], "copy_objects"))
    {
      copy_objects(START);
      Tcl_ResetResult(interp);
    }

    /* count_items string separator quoting_chars
         Debug command */
    else if(!strcmp(argv[1], "count_items"))
    {
      if(argc > 4) {
        Tcl_SetResult(interp, my_itoa(count_items(argv[2], argv[3], argv[4])), TCL_VOLATILE);
      }
    }
    /* Create_plot_cmd
     *   Create an xplot file in netlist/simulation directory with
     *   the list of highlighted nodes in a format the selected waveform
     *   viewer understands (bespice, gaw, ngspice) */
    else if(!strcmp(argv[1], "create_plot_cmd") )
    {
      create_plot_cmd();
      Tcl_ResetResult(interp);
    }

    /* cut
     *   Cut selection to clipboard */
    else if(!strcmp(argv[1], "cut"))
    {
      rebuild_selected_array();
      save_selection(2);
      delete(1/*to_push_undo*/);
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'd': /*----------------------------------------------*/
    /* debug n
     *   Set xschem in debug mode.'n' is the debug level 
     *   (0=no debug). Higher levels yield more debug info.*/
    if(!strcmp(argv[1], "debug"))
    {
      if(argc > 2) {
         debug_var=atoi(argv[2]);
         tclsetvar("debug_var",argv[2]);
      }
      Tcl_ResetResult(interp);
    }

    /* delete
     *   Delete selection */
    else if(!strcmp(argv[1], "delete"))
    {
      if(argc==2) delete(1/*to_push_undo*/);
      Tcl_ResetResult(interp);
    }

    /* delete_files
     *   Bring up a file selector the user can use to delete files */
    else if(!strcmp(argv[1], "delete_files"))
    {
      delete_files();
    }

    /* descend [n]
     *   Descend into selected component instance. Optional number 'n' specifies the 
     *   instance number to descend into for vector instances (default: 0). */
    else if(!strcmp(argv[1], "descend"))
    {
      int ret=0;
      if(argc > 2) {
        int n = atoi(argv[2]);
        ret = descend_schematic(n);
      } else {
        ret = descend_schematic(0);
      }
      Tcl_SetResult(interp, dtoa(ret), TCL_VOLATILE);
    }

    /* descend_symbol
     *   Descend into the symbol view of selected component instance */
    else if(!strcmp(argv[1], "descend_symbol"))
    {
      descend_symbol();
      Tcl_ResetResult(interp);
    }

    /* display_hilights
     *   Print a list of highlighted objects (nets, net labels/pins, instances) */
    else if(!strcmp(argv[1], "display_hilights"))
    {
      char *str = NULL;
      display_hilights(&str);
      Tcl_SetResult(interp, str, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &str);
    }

    /* draw_graph [n] [flags]
     *   Redraw graph rectangle number 'n'.
     *   If the optional 'flags' integer is given it will be used as the 
     *   flags bitmask to use while drawing (can be used to restrict what to redraw) */
    else if(!strcmp(argv[1], "draw_graph"))
    {
      int flags;
      if(argc > 2) {
        int i = atoi(argv[2]);
        if(argc > 3) {
          flags = atoi(argv[3]);
        } else {
          flags = 1 + 8 + (xctx->graph_flags & 6);
        }
        setup_graph_data(i, 0,  &xctx->graph_struct);
        draw_graph(i, flags, &xctx->graph_struct, NULL);
      }
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'e': /*----------------------------------------------*/
    /* edit_file
     *   Edit xschem file of current schematic if nothing is selected.
     *   Edit .sym file if a component is selected. */
    if(!strcmp(argv[1], "edit_file") )
    {
      rebuild_selected_array();
      if(xctx->lastsel==0 ) {
        save_schematic(xctx->sch[xctx->currsch]); /* sync data with disk file before editing file */
        my_snprintf(name, S(name), "edit_file {%s}",
            abs_sym_path(xctx->sch[xctx->currsch], ""));
        tcleval(name);
      }
      else if(xctx->sel_array[0].type==ELEMENT) {
        my_snprintf(name, S(name), "edit_file {%s}",
            abs_sym_path(xctx->inst[xctx->sel_array[0].n].name, ""));
        tcleval(name);
      }
    }
    /* edit_prop
     *   Edit global schematic/symbol attributes or attributes
     *   of currently selected instances */
    else if(!strcmp(argv[1], "edit_prop"))
    {
      edit_property(0);
      Tcl_ResetResult(interp);
    }

    /* edit_prop
     *   Edit global schematic/symbol attributes or
     *   attributes of currently selected instances
     *   using a text editor (defined in tcl 'editor' variable) */
    else if(!strcmp(argv[1], "edit_vi_prop"))
    {
      edit_property(1);
      Tcl_ResetResult(interp);
    }

    /* embed_rawfile raw_file
     *   Embed base 64 encoded 'raw_file' into currently
     *   selected element as a 'spice_data'
     *   attribute. */
    else if(!strcmp(argv[1], "embed_rawfile"))
    {
      if(argc > 2) {
        embed_rawfile(argv[2]);
      }
      Tcl_ResetResult(interp);
    }

    /* enable_layers
     *   Enable/disable layers depending on tcl array variable enable_layer() */
    else if(!strcmp(argv[1], "enable_layers"))
    {
      enable_layers();
      Tcl_ResetResult(interp);
    }

    /* exit
     *   Exit the program, ask for confirm if current file modified. */
    else if(!strcmp(argv[1], "exit"))
    {
      if(!strcmp(xctx->current_win_path, ".drw")) {
        if(has_x) {
          int remaining;
          remaining = new_schematic("destroy_all", NULL, NULL);
          if(!remaining) {
            if(xctx->modified) {
              tcleval("tk_messageBox -type okcancel  -parent [xschem get topwindow] -message \""
                      "[get_cell [xschem get schname] 0]"
                      ": UNSAVED data: want to exit?\"");
            }
            if(!xctx->modified || !strcmp(tclresult(), "ok")) tcleval("exit");
          }
        }
        else tcleval("exit"); /* if has_x == 0 there are no additional windows to close */
      } else {
        new_schematic("destroy", xctx->current_win_path, NULL);
      }
      Tcl_ResetResult(interp);
    }

    /* expandlabel lab
     *   Expand vectored labels/instance names:
     *   xschem expandlabel {2*A[3:0]} --> A[3],A[2],A[1],A[0],A[3],A[2],A[1],A[0] 8
     *   last field is the number of bits
     *   since [ and ] are TCL special characters argument must be quoted with { and } */
    else if(!strcmp(argv[1], "expandlabel")) 
    {
      int tmp;
      size_t llen;
      char *result=NULL;
      const char *l;
      if(argc > 2) {
        l = expandlabel(argv[2], &tmp);
        llen = strlen(l);
        dbg(1, "l=%s\n", l ? l : "<NULL>");
        result = my_malloc(_ALLOC_ID_, llen + 30);
        my_snprintf(result, llen + 30, "%s %d", l, tmp);
        Tcl_SetResult(interp, result, TCL_VOLATILE);
        my_free(_ALLOC_ID_, &result);
      }
    }
    else { cmd_found = 0;}
    break;
    case 'f': /*----------------------------------------------*/
    /* find_nth string sep n
     *   Find n-th field string separated by characters in sep. 1st field is in position 1
     *   xschem find_nth {aaa,bbb,ccc,ddd} {,} 2  --> bbb */
    if(!strcmp(argv[1], "find_nth"))
    {
      if(argc > 4) {
        Tcl_SetResult(interp, find_nth(argv[2], argv[3], atoi(argv[4])), TCL_VOLATILE);
      }
    }

    /* flip
     *   Flip selection horizontally */
    else if(!strcmp(argv[1], "flip"))
    {
      if(! (xctx->ui_state & (STARTMOVE | STARTCOPY) ) ) { 
        rebuild_selected_array();
        xctx->mx_double_save=xctx->mousex_snap;
        xctx->my_double_save=xctx->mousey_snap;
        move_objects(START,0,0,0);
        if(xctx->lastsel>1) move_objects(FLIP,0, 0, 0);
        else                move_objects(FLIP|ROTATELOCAL,0,0,0);
        xctx->deltax = -xctx->mx_double_save;
        xctx->deltay = 0;
        move_objects(END,0,0,0);
      }
      Tcl_ResetResult(interp);
    }

    /* flip_in_place
     *   Flip selection horizontally, each object around its center */
    else if(!strcmp(argv[1], "flip_in_place"))
    {
      if(! (xctx->ui_state & (STARTMOVE | STARTCOPY) ) ) {
        rebuild_selected_array();
        move_objects(START,0,0,0);
        move_objects(FLIP|ROTATELOCAL,0,0,0);
        move_objects(END,0,0,0);
      }
      Tcl_ResetResult(interp);
    }

    /* fullscreen
     *   Toggle fullscreen modes: fullscreen with menu & status, fullscreen, normal */
    else if(!strcmp(argv[1], "fullscreen"))
    {
      if(argc > 2) toggle_fullscreen(argv[2]);
      else toggle_fullscreen(".drw");
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'g': /*----------------------------------------------*/
    /************ xschem get subcommands *************/
    /* get var
     *   Get C variable/constant 'var' */
    if(!strcmp(argv[1], "get"))
    {
      if(argc > 2) {
        switch(argv[2][0]) {
          case 'b':
          if(!strcmp(argv[2], "backlayer")) { /* number of background layer */
            Tcl_SetResult(interp, my_itoa(BACKLAYER), TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "bbox_hilighted")) { /* bounding box of highlinhted objects */
            xRect boundbox;
            char res[2048];
            calc_drawing_bbox(&boundbox, 2);
            my_snprintf(res, S(res), "%g %g %g %g", boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
            Tcl_SetResult(interp, res, TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "bbox_selected")) { /* bounding box of selected objects */
            xRect boundbox;
            char res[2048];
            calc_drawing_bbox(&boundbox, 1);
            my_snprintf(res, S(res), "%g %g %g %g", boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
            Tcl_SetResult(interp, res, TCL_VOLATILE);
          }
          break;
          case 'c':
          if(!strcmp(argv[2], "cadlayers")) { /* number of layers */
            Tcl_SetResult(interp, my_itoa(cadlayers), TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "case_insensitive")) { /* case_insensitive symbol matching */
            Tcl_SetResult(interp, my_itoa(xctx->case_insensitive), TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "color_ps")) { /* color postscript flag */
            if(color_ps != 0 ) Tcl_SetResult(interp, "1",TCL_STATIC);
            else Tcl_SetResult(interp, "0",TCL_STATIC);
          }
          else if(!strcmp(argv[2], "current_dirname")) { /* directory name of current design */
            Tcl_SetResult(interp, xctx->current_dirname, TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "current_name")) { /* name of current design (no library path) */
            Tcl_SetResult(interp, xctx->current_name, TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "current_win_path")) { /* path of current tab/window (.drw, .x1.drw, ...) */
            Tcl_SetResult(interp, xctx->current_win_path, TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "currsch")) { /* hierarchy level of current schematic (start at 0) */
            Tcl_SetResult(interp, my_itoa(xctx->currsch),TCL_VOLATILE);
          }
          break;
          case 'd':
          if(!strcmp(argv[2], "debug_var")) { /* debug level (0 = no debug, 1, 2, 3,...) */
            Tcl_SetResult(interp, my_itoa(debug_var),TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "draw_window")) { /* direct draw into window */
            Tcl_SetResult(interp, my_itoa(xctx->draw_window),TCL_VOLATILE);
          }
          break;
          case 'f':
          if(!strcmp(argv[2], "format")) { /* alternate format attribute to use in netlist (or NULL) */
            if(!xctx->format ) Tcl_SetResult(interp, "<NULL>",TCL_STATIC);
            else Tcl_SetResult(interp, xctx->format,TCL_VOLATILE);
          }
          break;
          case 'g':
          if(!strcmp(argv[2], "graph_lastsel")) { /* number of last graph that was clicked */
            Tcl_SetResult(interp, my_itoa(xctx->graph_lastsel),TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "gridlayer")) { /* layer number for grid */
            Tcl_SetResult(interp, my_itoa(GRIDLAYER),TCL_VOLATILE);
          }
          break;
          case 'h':
          if(!strcmp(argv[2], "help")) { /* command help */
            if(help != 0 ) Tcl_SetResult(interp, "1",TCL_STATIC);
            else Tcl_SetResult(interp, "0",TCL_STATIC);
          }
          else if(!strcmp(argv[2], "header_text")) { /* header metadata (license info etc) present in schematic */
            if(xctx && xctx->header_text) {
              Tcl_SetResult(interp, xctx->header_text, TCL_VOLATILE);
            } else {
              Tcl_SetResult(interp, "", TCL_VOLATILE);
            }
          }
          break;
          case 'i':
          if(!strcmp(argv[2], "instances")) { /* number of instances in schematic */
            Tcl_SetResult(interp, my_itoa(xctx->instances), TCL_VOLATILE);
          }
          break;
          case 'l':
          if(!strcmp(argv[2], "lastsel")) { /* number of selected objects */
            rebuild_selected_array();
            Tcl_SetResult(interp, my_itoa(xctx->lastsel),TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "line_width")) { /* get line width */
            Tcl_SetResult(interp, dtoa(xctx->lw), TCL_VOLATILE);
          }
          break;
          case 'n':
          if(!strcmp(argv[2], "netlist_name")) { /* netlist name if set. If 'fallback' given get default name */
            if(argc > 3 &&  !strcmp(argv[3], "fallback")) {
              char f[PATH_MAX];
              if(xctx->netlist_type == CAD_SPICE_NETLIST) {
                my_snprintf(f, S(f), "%s.spice", skip_dir(xctx->current_name));
              }
              else if(xctx->netlist_type == CAD_VHDL_NETLIST) {
                my_snprintf(f, S(f), "%s.vhdl", skip_dir(xctx->current_name));
              }
              else if(xctx->netlist_type == CAD_VERILOG_NETLIST) {
                my_snprintf(f, S(f), "%s.v", skip_dir(xctx->current_name));
              }
              else if(xctx->netlist_type == CAD_TEDAX_NETLIST) {
                my_snprintf(f, S(f), "%s.tdx", skip_dir(xctx->current_name));
              }
              else {
                my_snprintf(f, S(f), "%s.unknown", skip_dir(xctx->current_name));
              }
              if(xctx->netlist_name[0] == '\0') {
                Tcl_SetResult(interp, f, TCL_VOLATILE);
              } else {
                Tcl_SetResult(interp, xctx->netlist_name, TCL_VOLATILE);
              }
            } else {
              Tcl_SetResult(interp, xctx->netlist_name, TCL_VOLATILE);
            }
          }
          else if(!strcmp(argv[2], "netlist_type")) { /* get current netlist type (spice/vhdl/verilog/tedax) */
            if(xctx->netlist_type == CAD_SPICE_NETLIST) {
              Tcl_SetResult(interp, "spice", TCL_STATIC);
            }
            else if(xctx->netlist_type == CAD_VHDL_NETLIST) {
              Tcl_SetResult(interp, "vhdl", TCL_STATIC);
            }
            else if(xctx->netlist_type == CAD_VERILOG_NETLIST) {
              Tcl_SetResult(interp, "verilog", TCL_STATIC);
            }
            else if(xctx->netlist_type == CAD_TEDAX_NETLIST) {
              Tcl_SetResult(interp, "tedax", TCL_STATIC);
            }
            else if(xctx->netlist_type == CAD_SYMBOL_ATTRS) {
              Tcl_SetResult(interp, "symbol", TCL_STATIC);
            }
            else {
              Tcl_SetResult(interp, "unknown", TCL_STATIC);
            }
          }
          else if(!strcmp(argv[2], "no_draw")) { /* disable drawing */
            if(xctx->no_draw != 0 )
              Tcl_SetResult(interp, "1",TCL_STATIC);
            else
              Tcl_SetResult(interp, "0",TCL_STATIC);
          }
          else if(!strcmp(argv[2], "ntabs")) { /* get number of additional tabs (0 = only one tab) */
            Tcl_SetResult(interp, my_itoa(new_schematic("ntabs", NULL, NULL)),TCL_VOLATILE);
          }
          break;
          case 'p':
          if(!strcmp(argv[2], "pinlayer")) { /* layer number for pins */
            Tcl_SetResult(interp, my_itoa(PINLAYER),TCL_VOLATILE);
          }
          break;
          case 'r':
          if(!strcmp(argv[2], "rectcolor")) { /* current layer number */
            Tcl_SetResult(interp, my_itoa(xctx->rectcolor),TCL_VOLATILE);
          }
          break;
          case 's':
          if(!strcmp(argv[2], "sellayer")) { /* layer number for selection */
            Tcl_SetResult(interp, my_itoa(SELLAYER),TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "semaphore")) { /* used for debug */
            Tcl_SetResult(interp, my_itoa(xctx->semaphore),TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "schname")) /* get full path of current sch. if 'n' given get sch of level 'n' */
          {
            int x;
            /* allows to retrieve name of n-th parent schematic */
            if(argc > 3) x = atoi(argv[3]);
            else x = xctx->currsch;
            if(x<0 && xctx->currsch+x>=0) {
              Tcl_SetResult(interp, xctx->sch[xctx->currsch+x], TCL_VOLATILE);
            } else if(x<=xctx->currsch) {
              Tcl_SetResult(interp, xctx->sch[x], TCL_VOLATILE);
            }
          }
          else if(!strcmp(argv[2], "sch_path")) /* get hierarchy path. if 'n' given get hierpath of level 'n' */
          {
            int x;
            if(argc > 3) x = atoi(argv[3]);
            else x = xctx->currsch;
            if(x<0 && xctx->currsch+x>=0) {
              Tcl_SetResult(interp, xctx->sch_path[xctx->currsch+x], TCL_VOLATILE);
            } else if(x<=xctx->currsch) {
              Tcl_SetResult(interp, xctx->sch_path[x], TCL_VOLATILE);
            }
          }
          else if(!strcmp(argv[2], "sch_to_compare")) /* if set return schematic current design is compared with */
          {
            Tcl_SetResult(interp, xctx->sch_to_compare, TCL_VOLATILE);
          }
          break;
          case 't':
          #ifndef __unix__
          if(!strcmp(argv[2], "temp_dir")) { /* get windows temporary dir */
            if(win_temp_dir[0] != '\0') Tcl_SetResult(interp, win_temp_dir, TCL_VOLATILE);
            else {
              TCHAR tmp_buffer_path[MAX_PATH];
              DWORD ret_val = GetTempPath(MAX_PATH, tmp_buffer_path);
              if(ret_val > MAX_PATH || (ret_val == 0)) {
                Tcl_SetResult(interp, "xschem get temp_dir failed\n", TCL_STATIC);
                fprintf(errfp, "xschem get temp_dir: path error\n");
                tcleval("exit");
              }
              else {
                char s[MAX_PATH];
                size_t num_char_converted;
                int err = wcstombs_s(&num_char_converted, s, MAX_PATH, tmp_buffer_path, MAX_PATH); /*unicode TBD*/
                if(err != 0) {
                  Tcl_SetResult(interp, "xschem get temp_dir conversion failed\n", TCL_STATIC);
                  fprintf(errfp, "xschem get temp_dir: conversion error\n");
                  tcleval("exit");
                }
                else {
                  change_to_unix_fn(s);
                  size_t slen = strlen(s);
                  if(s[slen - 1] == '/') s[slen - 1] = '\0';
                  my_strncpy(win_temp_dir, s, S(win_temp_dir));
                  dbg(2, "scheduler(): win_temp_dir is %s\n", win_temp_dir);
                  Tcl_SetResult(interp, s, TCL_VOLATILE);
                }
              }
            }
          }
          else 
          #endif
          if(!strcmp(argv[2], "text_svg")) { /* return 1 if using <text> elements in svg export */
            if(text_svg != 0 )
              Tcl_SetResult(interp, "1",TCL_STATIC);
            else
              Tcl_SetResult(interp, "0",TCL_STATIC);
          }
          else if(!strcmp(argv[2], "textlayer")) { /* layer number for texts */
            Tcl_SetResult(interp, my_itoa(TEXTLAYER), TCL_VOLATILE);
          }
          /* top_path="" for main window, ".x1", ".x2", ... for additional windows.
           * always "" in tabbed interface */
          else if(!strcmp(argv[2], "top_path")) { /* get top hier path of current window (always "") for tabbed if */
            Tcl_SetResult(interp, xctx->top_path, TCL_VOLATILE);
          }
          /* same as above but main window returned as "." */
          else if(!strcmp(argv[2], "topwindow")) { /* same as top_path but main window returned as "." */
            char *top_path;
            top_path =  xctx->top_path[0] ? xctx->top_path : ".";
            Tcl_SetResult(interp, top_path,TCL_VOLATILE);
          }
          break;
          case 'v':
          if(!strcmp(argv[2], "version")) { /* return xschem version */
            Tcl_SetResult(interp, XSCHEM_VERSION, TCL_VOLATILE);
          }
          break;
          case 'w':
          if(!strcmp(argv[2], "wirelayer")) { /* layer used for wires */
            Tcl_SetResult(interp, my_itoa(WIRELAYER), TCL_VOLATILE);
          }
          break;
          case 'x':
          if(!strcmp(argv[2], "xorigin")) { /* x coordinate of origin */
            char s[128];
            my_snprintf(s, S(s), "%.16g", xctx->xorigin);
            Tcl_SetResult(interp, s,TCL_VOLATILE);
          }
          break;
          case 'y':
          if(!strcmp(argv[2], "yorigin")) { /* y coordinate of origin */
            char s[128];
            my_snprintf(s, S(s), "%.16g", xctx->yorigin);
            Tcl_SetResult(interp, s,TCL_VOLATILE);
          }
          break;
          case 'z':
          if(!strcmp(argv[2], "zoom")) { /* zoom level */
            char s[128];
            my_snprintf(s, S(s), "%.16g", xctx->zoom);
            Tcl_SetResult(interp, s,TCL_VOLATILE);
          }
          break;
          default:
          cmd_found = 0;
          break;
        } /* switch */
      }
    }
    /************ end xschem get subcommands *************/
    /* getprop instance inst 
     *   Get the full attribute string of 'inst'
     *
     * getprop instance inst attr
     *   Get the value of attribute 'attr'
     *   If 'attr has the form 'cell::sym_attr' look up attribute 'sym_attr'
     *   of the symbol referenced by the instance.
     *
     * getprop instance_pin inst pin
     *   Get the full attribute string of pin 'pin' of instance 'inst'
     *   Example: xschem getprop instance_pin x3 MINUS --> name=MINUS dir=in
     *
     * getprop instance_pin inst pin pin_attr
     *   Get attribute 'pin_attr' of pin 'pin' of instance 'inst'
     *   Example: xschem getprop instance_pin x3 MINUS dir --> in
     *
     * getprop symbol sym_name
     *   Get full attribute string of symbol 'sym_name'
     *   example:
     *   xschem getprop symbol comp_ngspice.sym -->
     *     type=subcircuit
     *     format="@name @pinlist @symname
     *        OFFSET=@OFFSET AMPLITUDE=@AMPLITUDE GAIN=@GAIN ROUT=@ROUT COUT=@COUT"
     *     template="name=x1 OFFSET=0 AMPLITUDE=5 GAIN=100 ROUT=1000 COUT=1p"
     *
     * getprop symbol sym_name sym_attr [with_quotes]
     *   Get value of attribute 'sym_attr' of symbol 'sym_name'
     *   'with_quotes' (default:0) is an integer passed to get_tok_value()
     * getprop rect layer num attr
     *   Get attribute 'attr' of rectangle number 'num' on layer 'layer'
     *
     * getprop text num attr
     *   Get attribute 'attr' of text number 'num'
     *
     * ('inst' can be an instance name or instance number)
     * ('pin' can be a pin name or pin number)
     */
    else if(!strcmp(argv[1], "getprop"))
    {
      if(argc > 2 && !strcmp(argv[2], "instance")) {
        int i;
        const char *tmp;
        if(argc < 4) {
          Tcl_SetResult(interp, "'xschem getprop instance' needs 1 or 2 additional arguments", TCL_STATIC);
          return TCL_ERROR;
        }
        if((i = get_instance(argv[3])) < 0 ) {
          Tcl_SetResult(interp, "xschem getprop: instance not found", TCL_STATIC);
          return TCL_ERROR;
        }
        if(argc < 5) {
          Tcl_SetResult(interp, xctx->inst[i].prop_ptr, TCL_VOLATILE);
        } else if(!strcmp(argv[4], "cell::name")) {
          tmp = xctx->inst[i].name;
          Tcl_SetResult(interp, (char *) tmp, TCL_VOLATILE);
        } else if(strstr(argv[4], "cell::") ) {
          tmp = get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, argv[4]+6, 0);
          dbg(1, "scheduler(): xschem getprop: looking up instance %d prop cell::|%s| : |%s|\n", i, argv[4]+6, tmp);
          Tcl_SetResult(interp, (char *) tmp, TCL_VOLATILE);
        } else {
          Tcl_SetResult(interp, (char *)get_tok_value(xctx->inst[i].prop_ptr, argv[4], 0), TCL_VOLATILE);
        }
      } else if(argc > 2 && !strcmp(argv[2], "instance_pin")) {
        /*   0       1        2         3   4       5     */
        /* xschem getprop instance_pin X10 PLUS [pin_attr]  */
        /* xschem getprop instance_pin X10  1   [pin_attr]  */
        int inst, n=-1;
        size_t tmp;
        char *subtok=NULL;
        const char *value=NULL;
        if(argc < 5) {
          Tcl_SetResult(interp, "xschem getprop instance_pin needs 2 or 3 additional arguments", TCL_STATIC);
          return TCL_ERROR;
        }
        if((inst = get_instance(argv[3])) < 0 ) {
          Tcl_SetResult(interp, "xschem getprop: instance not found", TCL_STATIC);
          return TCL_ERROR;
        }
        if(isonlydigit(argv[4])) {
          n = atoi(argv[4]);
        }
        else {
          xSymbol *ptr = xctx->inst[inst].ptr+ xctx->sym;
          for(n = 0; n < ptr->rects[PINLAYER]; ++n) {
            char *prop = ptr->rect[PINLAYER][n].prop_ptr;
            if(!strcmp(get_tok_value(prop, "name",0), argv[4])) break;
          }
        }
        if(n>=0  && n < (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER]) {
          if(argc < 6) {
           Tcl_SetResult(interp, (xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][n].prop_ptr, TCL_VOLATILE);
          } else {
            tmp = 100 + strlen(argv[4]) + strlen(argv[5]);
            subtok = my_malloc(_ALLOC_ID_,tmp);
            my_snprintf(subtok, tmp, "%s(%s)", argv[5], argv[4]);
            value = get_tok_value(xctx->inst[inst].prop_ptr,subtok,0);
            if(!value[0]) {
              my_snprintf(subtok, tmp, "%s(%d)", argv[5], n);
              value = get_tok_value(xctx->inst[inst].prop_ptr,subtok,0);
            }
            if(!value[0]) {
              value = get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][n].prop_ptr,argv[5],0);
            }
            if(value[0] != 0) {
              char *ss;
              int slot;
              if((ss = strchr(xctx->inst[inst].instname, ':')) ) {
                sscanf(ss + 1, "%d", &slot);
                if(strstr(value, ":")) value = find_nth(value, ":", slot);
              }
              Tcl_SetResult(interp, (char *)value, TCL_VOLATILE);
            }
            my_free(_ALLOC_ID_, &subtok);
          }
        }
      /* xschem getprop symbol lm358.sym [type] */
      } else if(!strcmp(argv[2], "symbol")) {
        int i, found=0;
        if(argc < 4) {
          Tcl_SetResult(interp, "xschem getprop symbol needs 1 or 2 or 3 additional arguments", TCL_STATIC);
          return TCL_ERROR;
        }
        for(i=0; i<xctx->symbols; ++i) {
          if(!xctx->x_strcmp(xctx->sym[i].name,argv[3])){
            found=1;
            break;
          }
        }
        if(!found) {
          Tcl_SetResult(interp, "Symbol not found", TCL_STATIC);
          return TCL_ERROR;
        }
        if(argc < 5)
          Tcl_SetResult(interp, xctx->sym[i].prop_ptr, TCL_VOLATILE);
        else if(argc == 5) 
          Tcl_SetResult(interp, (char *)get_tok_value(xctx->sym[i].prop_ptr, argv[4], 0), TCL_VOLATILE);
        else if(argc > 5) 
          Tcl_SetResult(interp, (char *)get_tok_value(xctx->sym[i].prop_ptr, argv[4], atoi(argv[5])), TCL_VOLATILE);

      } else if(!strcmp(argv[2], "rect")) { /* xschem getprop rect c n token */
        if(argc < 6) {
          Tcl_SetResult(interp, "xschem getprop rect needs <color> <n> <token>", TCL_STATIC);
          return TCL_ERROR;
        } else {
          int c = atoi(argv[3]);
          int n = atoi(argv[4]);
          Tcl_SetResult(interp, (char *)get_tok_value(xctx->rect[c][n].prop_ptr, argv[5], 2), TCL_VOLATILE);
        }
      } else if(!strcmp(argv[2], "text")) { /* xschem getprop text n token */
        if(argc < 5) {
          Tcl_SetResult(interp, "xschem getprop text needs <n> <token>", TCL_STATIC);
          return TCL_ERROR;
        } else {
          int n = atoi(argv[3]);
          Tcl_SetResult(interp, (char *)get_tok_value(xctx->text[n].prop_ptr, argv[4], 2), TCL_VOLATILE);
        }
      }
    }
    
    /* get_tok str tok [with_quotes]
     *   get value of token 'tok' in string 'str'
     *  'with_quotes' (default:0) is an integer passed to get_tok_value() */
    else if(!strcmp(argv[1], "get_tok") )
    {
      char *s=NULL;
      int t;
      if(argc < 4) {Tcl_SetResult(interp, "Missing arguments", TCL_STATIC);return TCL_ERROR;}
      if(argc == 5) t = atoi(argv[4]);
      else t = 0;
      my_strdup(_ALLOC_ID_, &s, get_tok_value(argv[2], argv[3], t));
      Tcl_SetResult(interp, s, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &s);
    }

    /* get_tok_size
     *   Get length of last looked up attribute name (not its value) 
     *   if returned value is 0 it means that last searched attribute did not exist */
    else if(!strcmp(argv[1], "get_tok_size") )
    {
      Tcl_SetResult(interp, my_itoa((int)xctx->tok_size), TCL_VOLATILE);
    }

    /* globals
     *   Return various global variables used in the program */
    else if(!strcmp(argv[1], "globals"))
    {
      static char res[8192];
      Tcl_ResetResult(interp);
      my_snprintf(res, S(res), "*******global variables:*******\n"); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "INT_WIDTH(lw)=%d\n", INT_WIDTH(xctx->lw)); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "wires=%d\n", xctx->wires); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "instances=%d\n", xctx->instances); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "symbols=%d\n", xctx->symbols); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "lastsel=%d\n", xctx->lastsel); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "texts=%d\n", xctx->texts); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "maxt=%d\n", xctx->maxt); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "maxw=%d\n", xctx->maxw); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "maxi=%d\n", xctx->maxi); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "maxsel=%d\n", xctx->maxsel); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "zoom=%.16g\n", xctx->zoom); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "xorigin=%.16g\n", xctx->xorigin); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "yorigin=%.16g\n", xctx->yorigin); Tcl_AppendResult(interp, res, NULL);
      for(i=0;i<8; ++i)
      {
        my_snprintf(res, S(res), "rects[%d]=%d\n", i, xctx->rects[i]); Tcl_AppendResult(interp, res, NULL);
        my_snprintf(res, S(res), "lines[%d]=%d\n", i, xctx->lines[i]); Tcl_AppendResult(interp, res, NULL);
        my_snprintf(res, S(res), "maxr[%d]=%d\n", i, xctx->maxr[i]); Tcl_AppendResult(interp, res, NULL);
        my_snprintf(res, S(res), "maxl[%d]=%d\n", i, xctx->maxl[i]); Tcl_AppendResult(interp, res, NULL);
      }
      my_snprintf(res, S(res), "current_dirname=%s\n", xctx->current_dirname); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "current_name=%s\n", xctx->current_name); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "currsch=%d\n", xctx->currsch); Tcl_AppendResult(interp, res, NULL);
      for(i=0;i<=xctx->currsch; ++i)
      {
        my_snprintf(res, S(res), "previous_instance[%d]=%d\n",
            i,xctx->previous_instance[i]); Tcl_AppendResult(interp, res, NULL);
        my_snprintf(res, S(res), "sch_path[%d]=%s\n",i,xctx->sch_path[i]? 
            xctx->sch_path[i]:"<NULL>"); Tcl_AppendResult(interp, res, NULL);
        my_snprintf(res, S(res), "sch[%d]=%s\n",i,xctx->sch[i]); Tcl_AppendResult(interp, res, NULL);
      }
      my_snprintf(res, S(res), "modified=%d\n", xctx->modified); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "areaw=%d\n", xctx->areaw); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "areah=%d\n", xctx->areah); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "color_ps=%d\n", color_ps); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "hilight_nets=%d\n", xctx->hilight_nets); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "semaphore=%d\n", xctx->semaphore); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "ui_state=%d\n", xctx->ui_state); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "prep_net_structs=%d\n", xctx->prep_net_structs); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "prep_hi_structs=%d\n", xctx->prep_hi_structs); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "prep_hash_inst=%d\n", xctx->prep_hash_inst); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "prep_hash_wires=%d\n", xctx->prep_hash_wires); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "need_reb_sel_arr=%d\n", xctx->need_reb_sel_arr); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "undo_type=%d\n", xctx->undo_type); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "******* end global variables:*******\n"); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "******* Compile options:*******\n"); Tcl_AppendResult(interp, res, NULL);
      #ifdef HAS_DUP2
      my_snprintf(res, S(res), "HAS_DUP2=%d\n", HAS_DUP2); Tcl_AppendResult(interp, res, NULL); 
      #endif
      #ifdef HAS_PIPE
      my_snprintf(res, S(res), "HAS_PIPE=%d\n", HAS_PIPE); Tcl_AppendResult(interp, res, NULL); 
      #endif
      #ifdef HAS_POPEN
      my_snprintf(res, S(res), "HAS_POPEN=%d\n", HAS_POPEN); Tcl_AppendResult(interp, res, NULL); 
      #endif
      #ifdef HAS_CAIRO
      my_snprintf(res, S(res), "HAS_CAIRO=%d\n", HAS_CAIRO); Tcl_AppendResult(interp, res, NULL); 
      #endif
      #ifdef XSCHEM_SHAREDIR
      my_snprintf(res, S(res), "XSCHEM_SHAREDIR=%s\n", XSCHEM_SHAREDIR); Tcl_AppendResult(interp, res, NULL); 
      #endif
      #ifdef PREFIX
      my_snprintf(res, S(res), "PREFIX=%s\n", PREFIX); Tcl_AppendResult(interp, res, NULL); 
      #endif
      #ifdef USER_CONF_DIR
      my_snprintf(res, S(res), "USER_CONF_DIR=%s\n", USER_CONF_DIR); Tcl_AppendResult(interp, res, NULL); 
      #endif
      #ifdef XSCHEM_LIBRARY_PATH
      my_snprintf(res, S(res), "XSCHEM_LIBRARY_PATH=%s\n", XSCHEM_LIBRARY_PATH); Tcl_AppendResult(interp, res, NULL); 
      #endif
      #ifdef HAS_SNPRINTF
      my_snprintf(res, S(res), "HAS_SNPRINTF=%s\n", HAS_SNPRINTF); Tcl_AppendResult(interp, res, NULL); 
      #endif
    }

    /* go_back
     *   Go up one level (pop) in hierarchy */
    else if(!strcmp(argv[1], "go_back"))
    {
      go_back(1);
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'h': /*----------------------------------------------*/
    /* hash_file file [skip_path_lines]
     *   Do a simple hash of 'file'
     *   'skip_path_lines' is an integer (default: 0) passed to hash_file() */
    if(!strcmp(argv[1], "hash_file"))
    {
      unsigned int h;
      char s[40];
      if(argc > 2) {
        if(argc > 3) {
          h = hash_file(argv[2], atoi(argv[3]));
        } else {
          h = hash_file(argv[2], 0);
        }
        my_snprintf(s, S(s), "%u", h);
        Tcl_SetResult(interp, s, TCL_VOLATILE);
      }
    }

    /* hash_string str
     *   Do a simple hashing of string 'str' */
    else if(!strcmp(argv[1], "hash_string"))
    {
      if(argc > 2) {
        unsigned int h;
        char s[50];
        h = str_hash(argv[2]);
        my_snprintf(s, S(s), "%u", h);
        Tcl_SetResult(interp, s, TCL_VOLATILE);
      }
    }

   
    /* help
     *  Print command help */
    else if(!strcmp(argv[1], "help"))
    {
      xschem_cmd_help(argc, argv);
    }

    /* hier_psprint [file]
     *   Hierarchical postscript / pdf print
     *   if 'file' is not given show a fileselector dialog box */
    else if(!strcmp(argv[1], "hier_psprint"))
    {
      if(argc > 2) {
        my_strncpy(xctx->plotfile, argv[2], S(xctx->plotfile));
      }
      hier_psprint(NULL, 1);
      Tcl_ResetResult(interp);
    }

    /* hilight [drill]
     *   Highlight selected element/pins/labels/nets
     *   if 'drill' is given propagate net highlights through conducting elements
     *   (elements that have the 'propag' attribute on pins ) */
    else if(!strcmp(argv[1], "hilight"))
    {
      xctx->enable_drill = 0;
      if(argc >=3 && !strcmp(argv[2], "drill")) xctx->enable_drill = 1;
      hilight_net(0);
      redraw_hilights(0);
      Tcl_ResetResult(interp);
    }
    /* hilight_instname inst
     *   Highlight instance 'inst' 
     *   'inst' can be an instance name or number */
    else if(!strcmp(argv[1], "hilight_instname"))
    {
      if(argc > 2) {
        int inst;
        char *type;
        int incr_hi;
        xctx->enable_drill=0;
        incr_hi = tclgetboolvar("incr_hilight");
        prepare_netlist_structs(0);
        if((inst = get_instance(argv[2])) < 0 ) {
          Tcl_SetResult(interp, "xschem hilight_instname: instance not found", TCL_STATIC);
          return TCL_ERROR;
        } else {
          type = (xctx->inst[inst].ptr+ xctx->sym)->type;
          if( type && xctx->inst[inst].node && IS_LABEL_SH_OR_PIN(type) ) { /* instance must have a pin! */
                /* sets xctx->hilight_nets=1 */
            if(!bus_hilight_hash_lookup(xctx->inst[inst].node[0], xctx->hilight_color, XINSERT_NOREPLACE)) {
              dbg(1, "xschem hilight_instname: node=%s\n", xctx->inst[inst].node[0]);
              if(incr_hi) incr_hilight_color();
            }
          } else {
            dbg(1, "xschem hilight_instname: setting hilight flag on inst %d\n",inst);
            /* xctx->hilight_nets=1; */  /* done in hilight_hash_lookup() */
            xctx->inst[inst].color = xctx->hilight_color;
            inst_hilight_hash_lookup(xctx->inst[inst].instname, xctx->hilight_color, XINSERT_NOREPLACE);
            if(incr_hi) incr_hilight_color();
          }
          dbg(1, "hilight_nets=%d\n", xctx->hilight_nets);
          if(xctx->hilight_nets) propagate_hilights(1, 0, XINSERT_NOREPLACE);
          redraw_hilights(0);
        }
      }
      Tcl_ResetResult(interp);
    }
    /* hilight_netname net
     *   Highlight net name 'net' */
    else if(!strcmp(argv[1], "hilight_netname"))
    {
      int ret = 0;
      if(argc > 2) {
        ret = hilight_netname(argv[2]);
      }
      Tcl_SetResult(interp, ret ? "1" : "0" , TCL_STATIC);
    }
    else { cmd_found = 0;}
    break;
    case 'i': /*----------------------------------------------*/
    /* instance sym_name x y rot flip [prop] [first_call]
     *   Place a new instance of symbol 'sym_name' at position x,y,
     *   rotation and flip  set to 'rot', 'flip'
     *   if 'prop' is given it is the new instance attribute
     *   string (default: symbol template string)
     *   if 'first_call' is given it must be 1 on first call
     *   and zero on following calls
     *   It is used only for efficiency reasons if placing multiple instances */
    if(!strcmp(argv[1], "instance"))
    {
      if(argc==7)
       /*           pos sym_name      x                y             rot       */
        place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), (short)atoi(argv[5]), 
               /* flip              prop draw first to_push_undo */
               (short)atoi(argv[6]),NULL,  3,   1,      1);
      else if(argc==8)
        place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), (short)atoi(argv[5]),
               (short)atoi(argv[6]), argv[7], 3, 1, 1);
      else if(argc==9) {
        int x = !(atoi(argv[8]));
        place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), (short)atoi(argv[5]),
               (short)atoi(argv[6]), argv[7], 0, x, 1);
      }
    }

    /* instance_bbox inst
     *   return instance and symbol bounding boxes
     *   'inst' can be an instance name or number */
    else if(!strcmp(argv[1], "instance_bbox"))
    {
      int i;
      char s[200];

      if(argc > 2) {
        if((i = get_instance(argv[2])) < 0 ) {
          Tcl_SetResult(interp, "xschem instance_bbox: instance not found", TCL_STATIC);
          return TCL_ERROR;
        }   
  
        my_snprintf(s, S(s), "Instance: %g %g %g %g", xctx->inst[i].x1, xctx->inst[i].y1,
                                                       xctx->inst[i].x2, xctx->inst[i].y2);
        Tcl_AppendResult(interp, s, NULL);
        my_snprintf(s, S(s), "\nSymbol: %g %g %g %g",
   	  (xctx->inst[i].ptr+ xctx->sym)->minx,
   	  (xctx->inst[i].ptr+ xctx->sym)->miny,
   	  (xctx->inst[i].ptr+ xctx->sym)->maxx,
   	  (xctx->inst[i].ptr+ xctx->sym)->maxy);
        Tcl_AppendResult(interp, s, NULL);
      }
    }

    /* instance_net inst pin
     *   Return the name of the net attached to pin 'pin' of instance 'inst'
     *   Example: xschem instance_net x3 MINUS --> REF */
    else if(!strcmp(argv[1], "instance_net"))
    {
      /* xschem instance_net inst pin */
      int no_of_pins, i, p, multip;
      const char *str_ptr=NULL;
      if(argc < 4) {
        Tcl_SetResult(interp, "xschem instance_net needs 2 additional arguments", TCL_STATIC);
        return TCL_ERROR;
      }
      if((i = get_instance(argv[2])) < 0 ) {
        Tcl_SetResult(interp, "xschem instance_net: instance not found", TCL_STATIC);
        return TCL_ERROR;
      }
      prepare_netlist_structs(0);
      no_of_pins= (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];
      for(p=0;p<no_of_pins;p++) {
        if(!strcmp(get_tok_value((xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][p].prop_ptr, "name",0), argv[3])) {
          str_ptr =  net_name(i,p, &multip, 0, 1);
          break;
        }
      } /* /20171029 */
      if(p>=no_of_pins) {
        Tcl_SetResult(interp, "Pin not found", TCL_STATIC);
        return TCL_ERROR;
      }
      Tcl_SetResult(interp, (char *)str_ptr, TCL_VOLATILE);
    }

    /* instance_nodemap inst
     *   Return the instance name followed by a list of 'pin net' associations
     *   example:  xschem instance_nodemap x3
     *   --> x3 PLUS LED OUT LEVEL MINUS REF
     *   instance x3 pin PLUS is attached to net LED, pin OUT to net LEVEL and so on... */
    else if(!strcmp(argv[1], "instance_nodemap"))
    {
    /* xschem instance_nodemap [instance_name] */
      int p, no_of_pins;
      int inst = -1;
      prepare_netlist_structs(0);
      if(argc > 2) {
        inst = get_instance(argv[2]);
        if(inst >=0) {
          Tcl_AppendResult(interp,  xctx->inst[inst].instname, " ",  NULL);
          no_of_pins= (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER];
          for(p=0;p<no_of_pins;p++) {
            const char *pin;
            pin = get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][p].prop_ptr, "name",0);
            if(!pin[0]) pin = "--ERROR--";
            if(argc > 3 && strcmp(argv[3], pin)) continue;
            Tcl_AppendResult(interp, pin, " ",
                  xctx->inst[inst].node && xctx->inst[inst].node[p] ? xctx->inst[inst].node[p] : "{}", " ", NULL);
          }
        }
      }
    }

    /* instance_pin_coord inst attr value
     *   Return the name and coordinates of pin with 
     *   attribute 'attr' set to 'value' of instance 'inst'
     *   'inst can be an instance name or a number
     *   Example: xschem instance_pin_coord x3 name MINUS --> {MINUS} 600 -840 */
    else if(!strcmp(argv[1], "instance_pin_coord"))
    {
    /*   0            1           2       3     4
     * xschem instance_pin_coord m12  pinnumber 2
     * xschem instance_pin_coord U3:2 pinnumber 5
     * xschem instance_pin_coord m12 name d
     * returns pin_name x y */
      xSymbol *symbol;
      xRect *rct;
      short flip, rot;
      double x0,y0, pinx0, piny0;
      char num[60];
      int p, i, no_of_pins, slot;
      const char *pin;
      char *ss;
      char *tmpstr = NULL;
      if(argc < 5) {
        Tcl_SetResult(interp,
          "xschem instance_pin_coord requires an instance, a pin attribute and a value", TCL_STATIC);
        return TCL_ERROR;
      }
      i = get_instance(argv[2]);
      if(i < 0) {
        Tcl_SetResult(interp, "", TCL_STATIC);
        return TCL_OK;
      }
      x0 = xctx->inst[i].x0;
      y0 = xctx->inst[i].y0;
      rot = xctx->inst[i].rot;
      flip = xctx->inst[i].flip;
      symbol = xctx->sym + xctx->inst[i].ptr;
      no_of_pins= symbol->rects[PINLAYER];
      rct=symbol->rect[PINLAYER];
      /* slotted devices: name= U1:2, pinnumber=2:5 */
      slot = -1;
      tmpstr = my_malloc(_ALLOC_ID_, sizeof(xctx->inst[i].instname));
      if((ss=strchr(xctx->inst[i].instname, ':')) ) {
        sscanf(ss+1, "%s", tmpstr);
        if(isonlydigit(tmpstr)) {
          slot = atoi(tmpstr);
        }
      }
      for(p = 0;p < no_of_pins; p++) {
        pin = get_tok_value(rct[p].prop_ptr,argv[3],0);
        if(slot > 0 && !strcmp(argv[3], "pinnumber") && strstr(pin, ":")) pin = find_nth(pin, ":", slot);
        if(!strcmp(pin, argv[4])) break;
      }
      if(p >= no_of_pins) {
        Tcl_SetResult(interp, "", TCL_STATIC);
        return TCL_OK;
      }
      pinx0 = (rct[p].x1+rct[p].x2)/2;
      piny0 = (rct[p].y1+rct[p].y2)/2;
      ROTATION(rot, flip, 0.0, 0.0, pinx0, piny0, pinx0, piny0);
      pinx0 += x0;
      piny0 += y0;
      my_snprintf(num, S(num), "{%s} %g %g", get_tok_value(rct[p].prop_ptr, "name", 0), pinx0, piny0);
      Tcl_SetResult(interp, num, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &tmpstr);
    }

    /* instance_pins inst
         Return list of pins of instance 'inst'
         'inst can be an instance name or a number */
    else if(!strcmp(argv[1], "instance_pins"))
    {
      char *pins = NULL;
      int p, i, no_of_pins;
      if(argc > 2) {
        prepare_netlist_structs(0);
        if((i = get_instance(argv[2])) < 0 ) {
          Tcl_SetResult(interp, "xschem instance_pins: instance not found", TCL_STATIC);
          return TCL_ERROR;
        }
        no_of_pins= (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];
        for(p=0;p<no_of_pins;p++) {
          const char *pin;
          pin = get_tok_value((xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][p].prop_ptr, "name",0);
          if(!pin[0]) pin = "--ERROR--";
          my_mstrcat(_ALLOC_ID_, &pins, "{", pin, "}", NULL);
          if(p< no_of_pins-1) my_strcat(_ALLOC_ID_, &pins, " ");
        }
        Tcl_SetResult(interp, pins, TCL_VOLATILE);
        my_free(_ALLOC_ID_, &pins);
      }
    }

    /* instance_pos inst
     *   Get number (position) of instance name 'inst' */
    else if(!strcmp(argv[1], "instance_pos"))
    {
      int i;
      char s[30];
      if(argc > 2) {
        i = get_instance(argv[2]);
        my_snprintf(s, S(s), "%d", i);
        Tcl_SetResult(interp, s, TCL_VOLATILE);
      }
    }

    /* instances_to_net net
     *   Return list of instances names and pins attached to net 'net' 
     *   Example: xschem instances_to_net PANEL
     *    --> { {Vsw} {plus} {580} {-560} } { {p2} {p} {660} {-440} }
     *        { {Vpanel1} {minus} {600} {-440} } */
    else if(!strcmp(argv[1], "instances_to_net"))
    {
    /* xschem instances_to_net PLUS */
      xSymbol *symbol;
      xRect *rct;
      short flip, rot;
      double x0,y0, pinx0, piny0, rpinx0, rpiny0;
      char *pins = NULL;
      int p, i, no_of_pins;
      prepare_netlist_structs(0);
      if(argc < 3) {
        Tcl_SetResult(interp, "xschem instances_to_net requires a net name argument", TCL_STATIC);
        return TCL_ERROR;
      }
      for(i = 0;i < xctx->instances; ++i) {
        x0 = xctx->inst[i].x0;
        y0 = xctx->inst[i].y0;
        rot = xctx->inst[i].rot;
        flip = xctx->inst[i].flip;
        symbol = xctx->sym + xctx->inst[i].ptr;
        no_of_pins= symbol->rects[PINLAYER];
        rct=symbol->rect[PINLAYER];
        for(p = 0;p < no_of_pins; p++) {
          const char *pin;
          char xx[70], yy[70];
          pin = get_tok_value(rct[p].prop_ptr, "name",0);
          if(!pin[0]) pin = "--ERROR--";
          if(xctx->inst[i].node[p] && !strcmp(xctx->inst[i].node[p], argv[2]) &&
             !IS_LABEL_SH_OR_PIN( (xctx->inst[i].ptr+xctx->sym)->type )) {
            my_mstrcat(_ALLOC_ID_, &pins, "{ {", xctx->inst[i].instname, "} {", pin, NULL);
            pinx0 = (rct[p].x1+rct[p].x2)/2;
            piny0 = (rct[p].y1+rct[p].y2)/2;
            ROTATION(rot, flip, 0.0, 0.0, pinx0, piny0, rpinx0, rpiny0);
            rpinx0 += x0;
            rpiny0 += y0;
            my_strncpy(xx, dtoa(rpinx0), S(xx));
            my_strncpy(yy, dtoa(rpiny0), S(yy));
            my_mstrcat(_ALLOC_ID_, &pins, "} {", xx, "} {", yy, "} } ", NULL);
          }
        }
      }
      Tcl_SetResult(interp, pins ? pins : "", TCL_VOLATILE);
      my_free(_ALLOC_ID_, &pins);
    }
    else { cmd_found = 0;}
    break;
    case 'l': /*----------------------------------------------*/
    /* line x1 y1 x2 y2 [pos]
     *   Place a line on current layer (rectcolor) 
     *   if integer number 'pos' is given place line at indicated
     *   position in the line array. */
    if(!strcmp(argv[1], "line"))
    {
      double x1,y1,x2,y2;
      int pos, save;
      if(argc > 5) {
        x1=atof(argv[2]);
        y1=atof(argv[3]);
        x2=atof(argv[4]);
        y2=atof(argv[5]);
        ORDER(x1,y1,x2,y2);
        pos=-1;
        if(argc==7) pos=atoi(argv[6]);
        storeobject(pos, x1,y1,x2,y2,LINE,xctx->rectcolor,0,NULL);
        save = xctx->draw_window; xctx->draw_window = 1;
        drawline(xctx->rectcolor,NOW, x1,y1,x2,y2, 0, NULL);
        xctx->draw_window = save;
      }
      else xctx->ui_state |= MENUSTARTLINE;
    }

    /* line_width n
     *   set line width to floating point number 'n' */
    else if(!strcmp(argv[1], "line_width"))
    {
      if(argc > 2) {
        change_linewidth(atof(argv[2]));
        Tcl_ResetResult(interp);
      }
    }

    /* list_hierarchy
     *   List all schematics at or below current hierarchy with modification times.
     *   Example: xschem list_hiearchy
     *   -->
     *   20230302_003134  {/home/.../ngspice/solar_panel.sch}
     *   20230211_010031  {/home/.../ngspice/pv_ngspice.sch}
     *   20221011_175308  {/home/.../ngspice/diode_ngspice.sch}
     *   20221014_091945  {/home/.../ngspice/comp_ngspice.sch}
     */
    else if(!strcmp(argv[1], "list_hierarchy"))
    {
      char *res = NULL;
      Tcl_ResetResult(interp);
      hier_psprint(&res, 2);
      Tcl_SetResult(interp, res, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &res);
    }

    /* list_hilights [sep]
     *    Sorted list of highlight nets, separated by character 'sep' (default: space) */
    else if(!strcmp(argv[1], "list_hilights"))
    {
      const char *sep;
      if(argc > 2) {
        sep = argv[2];
      } else {
        sep = "{ }";
      }
      list_hilights();
      tclvareval("join [lsort -decreasing -dictionary {", tclresult(), "}] ", sep, NULL);
    }

    /* list_tokens str with_quotes
     *   List tokens in string 'str'
     *   with_quotes:
     *   0: eat non escaped quotes (")
     *   1: return unescaped quotes as part of the token value if they are present
     *   2: eat backslashes */
    else if(!strcmp(argv[1], "list_tokens"))
    {
      if(argc > 3) {
        Tcl_ResetResult(interp);
        Tcl_SetResult(interp, (char *)list_tokens(argv[2], atoi(argv[3])), TCL_VOLATILE);
      }
    }

    /* load f [symbol|force|noundoreset|nofullzoom]
     *   Load a new file 'f'.
     *   'force': do not ask to save modified file or warn if opening an already open file
     *   'noundoreset': do not reset the undo history
     *   'symbol': do not load symbols (used if loading a symbol instead of a schematic)
     *   'nofullzoom': do not do a fll zoom on new schematic.
     */
    else if(!strcmp(argv[1], "load") )
    {
      int load_symbols = 1, force = 0, undo_reset = 1, nofullzoom = 0;
      size_t i;
      if(argc > 3) {
        for(i = 3; i < argc; ++i) {
          if(!strcmp(argv[i], "symbol")) load_symbols = 0;
          if(!strcmp(argv[i], "force")) force = 1;
          if(!strcmp(argv[i], "noundoreset")) undo_reset = 0;
          if(!strcmp(argv[i], "nofullzoom")) nofullzoom = 1;
        }
      }
      if(argc>2) {
        if(force || !has_x || !xctx->modified  || save(1) != -1 ) { /* save(1)==-1 --> user cancel */
          char f[PATH_MAX];
          char win_path[WINDOW_PATH_SIZE];
          int skip = 0;
          dbg(1, "scheduler(): load: filename=%s\n", argv[2]);
          my_strncpy(f,  abs_sym_path(argv[2], ""), S(f));
          if(!force && f[0] && check_loaded(f, win_path) ) {
            char msg[PATH_MAX + 100];
            my_snprintf(msg, S(msg),
               "tk_messageBox -type okcancel -icon warning -parent [xschem get topwindow] "
               "-message {Warning: %s already open.}", f);
            if(has_x) {
              tcleval(msg);
              if(strcmp(tclresult(), "ok")) skip = 1;
            }
            else dbg(0, "xschem load: %s already open: %s\n", f, win_path);
          }
          if(!skip) {
            clear_all_hilights();
            unselect_all(1);
            if(!undo_reset) xctx->push_undo();
            xctx->currsch = 0;
            remove_symbols();
            dbg(1, "scheduler: undo_reset=%d\n", undo_reset);
            load_schematic(load_symbols, f, undo_reset);
            tclvareval("update_recent_file {", f, "}", NULL);
            my_strdup(_ALLOC_ID_, &xctx->sch_path[xctx->currsch], ".");
            xctx->sch_path_hash[xctx->currsch] = 0;
            xctx->sch_inst_number[xctx->currsch] = 1;
            if(nofullzoom) draw();
            else zoom_full(1, 0, 1, 0.97);
          }
        }
      }
      else if(argc==2) {
        ask_new_file();
      }
      Tcl_SetResult(interp, xctx->sch[xctx->currsch], TCL_STATIC);
    }
 
    /* load_new_window [f]
     *   Load schematic in a new tab/window. If 'f' not given prompt user */
    else if(!strcmp(argv[1], "load_new_window") )
    {
      char fullname[PATH_MAX];
      if(has_x) {
        if(argc > 2) {
          my_snprintf(fullname, S(fullname), "%s", argv[2]);
        } else {
          tcleval("load_file_dialog {Load file} *.\\{sch,sym\\} INITIALLOADDIR");
          my_snprintf(fullname, S(fullname), "%s", tclresult());
        }
        if(fullname[0] ) {
         new_schematic("create", NULL, fullname);
         tclvareval("update_recent_file {", fullname, "}", NULL);
        }
      } else {
        Tcl_ResetResult(interp);
      }
    }
    
    /* log f
     *   If 'f' is given output stderr messages to file 'f'
     *   if 'f' is not given and a file log is open, close log
     *   file and resume logging to stderr */
    else if(!strcmp(argv[1], "log"))
    {
      if(argc==3 && errfp == stderr ) { errfp = fopen(argv[2], "w"); } /* added check to avoid multiple open */
      else if(argc==2 && errfp != stderr) { fclose(errfp); errfp=stderr; }
    }

    /* logic_get_net net_name
     *   Get logic state of net named 'net_name'
     *   Returns 0, 1, 2, 3 for logic levels 0, 1, X, Z or nothing if no net found.
     */
    else if(!strcmp(argv[1], "logic_get_net"))
    {
      static char s[2]="X";
      Tcl_ResetResult(interp);
      if(argc > 2) {
        Hilight_hashentry  *entry;
        entry = bus_hilight_hash_lookup(argv[2], 0, XLOOKUP);
        if(entry) {
          switch(entry->value) {
            case -5:
            s[0] = '1';
            break;
            case -12:
            s[0] = '0';
            break;
            case -1:
            s[0] = '2';
            break;
            case -13:
            s[0] = '3';
            break;
            default:
            s[0] = '2';
            break;
          }
        }
        Tcl_SetResult(interp, s, TCL_VOLATILE);
      }
    }

    /* logic_set_net net_name n [num]
     *   set 'net_name' to logic level 'n' 'num' times.
     *   'n': 
     *       0  set to logic value 0
     *       1  set to logic value 1
     *       2  set to logic value X
     *       3  set to logic value Z
     *      -1  toggle logic valie (1->0, 0->1)
     *   the 'num' parameter is essentially useful only with 'toggle' (-1)  value
     */
    else if(!strcmp(argv[1], "logic_set_net"))
    {
      int num =  1;
      if(argc > 4 ) num = atoi(argv[4]);
      if(argc > 3) {
        int n = atoi(argv[3]);
        if(n == 4) n = -1;
        logic_set(n, num, argv[2]);
      }
      Tcl_ResetResult(interp);
    }

    /* logic_set n [num]
     *   set selected nets, net labels or pins to logic level 'n' 'num' times.
     *   'n': 
     *       0  set to logic value 0
     *       1  set to logic value 1
     *       2  set to logic value X
     *       3  set to logic value Z
     *      -1  toggle logic valie (1->0, 0->1)
     *   the 'num' parameter is essentially useful only with 'toggle' (-1)  value
     */
    else if(!strcmp(argv[1], "logic_set"))
    {
      int num =  1;
      if(argc > 3 ) num = atoi(argv[3]);
      if(argc > 2) {
        int n = atoi(argv[2]);
        if(n == 4) n = -1;
        logic_set(n, num, NULL);
      }
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'm': /*----------------------------------------------*/
    /* make_sch
     *   Make a schematic from selected symbol */
    if(!strcmp(argv[1], "make_sch")) /* make schematic from selected symbol 20171004 */
    {
      create_sch_from_sym();
      Tcl_ResetResult(interp);
    }

    /* make_sch_from_sel 
     *   Create an LCC instance from selection and place it instead of selection
     *   also ask if a symbol (.sym) file needs to be created */
    else if(!strcmp(argv[1], "make_sch_from_sel"))
    {
      make_schematic_symbol_from_sel();
      Tcl_ResetResult(interp);
    }

    /* make_symbol
     *   From current schematic (circuit.sch) create a symbol (circuit.sym) 
     *   using ipin.sym, opin.sym, iopin.sym in schematic
     *   to deduce symbol interface pins. */
    else if(!strcmp(argv[1], "make_symbol"))
    {
      if(has_x) tcleval("tk_messageBox -type okcancel -parent [xschem get topwindow] "
                        "-message {do you want to make symbol view ?}");
      if(!has_x || strcmp(tclresult(), "ok")==0) {
        save_schematic(xctx->sch[xctx->currsch]);
        make_symbol();
      }
      Tcl_ResetResult(interp);
    }

    /* merge [f]
     *   Merge another file. if 'f' not given prompt user. */
    else if(!strcmp(argv[1], "merge"))
    {
      if(argc < 3) {
        merge_file(0, "");  /* 2nd param not used for merge 25122002 */
      }
      else {
        merge_file(0,argv[2]);
      }
      Tcl_ResetResult(interp);
    }

    /* move_objects [dx dy]
     *   Start a move operation on selection and let user terminate the operation in the GUI
     *   if dx and dy are given move by that amount. */
    else if(!strcmp(argv[1], "move_objects"))
    {
      if(argc==4) {
        move_objects(START,0,0,0);
        move_objects( END,0,atof(argv[2]), atof(argv[3]));
      }
      else move_objects(START,0,0,0);
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'n': /*----------------------------------------------*/
    /* net_label [type]
     *   Place a new net label
     *   'type': 1: place a 'lab_pin.sym' label
     *           0: place a 'lab_wire.sym' label
     *   User should complete the placement in the GUI. */
    if(!strcmp(argv[1], "net_label"))
    {
      if(argc > 2) {
        unselect_all(1);
        place_net_label(atoi(argv[2]));
      }
    }

    /* net_pin_mismatch
     *   Highlight nets attached to selected symbols with
     *   a different name than symbol pin */
    else if(!strcmp(argv[1], "net_pin_mismatch"))
    {
      hilight_net_pin_mismatches();
    }

    /* netlist
     * do a netlist of current schematic in currently defined netlist format */
    else if(!strcmp(argv[1], "netlist") )
    {
      yyparse_error = 0;
      if(set_netlist_dir(0, NULL) ) {
        if(xctx->netlist_type == CAD_SPICE_NETLIST)
          global_spice_netlist(1);                  /* 1 means global netlist */
        else if(xctx->netlist_type == CAD_VHDL_NETLIST)
          global_vhdl_netlist(1);
        else if(xctx->netlist_type == CAD_VERILOG_NETLIST)
          global_verilog_netlist(1);
        else if(xctx->netlist_type == CAD_TEDAX_NETLIST)
          global_tedax_netlist(1);
        else
          if(has_x) tcleval("tk_messageBox -type ok -parent [xschem get topwindow] "
                            "-message {Please Set netlisting mode (Options menu)}");
        Tcl_ResetResult(interp);
      }
    }

    /* new_schematic create|destroy|destroy_all|switch_win winpath file
     *   Open/destroy a new tab or window 
     *     create: create new empty window or with 'file' loaded if 'file' given.
     *             The winpath must be given (even {} is ok) but not used.
     *     destroy: destroy tab/window identified by winpath. Example:
     *              xschem new_schematic destroy .x1.drw
     *     destroy_all: close all tabs/additional windows
     *     switch_win: switch context to specified 'winpath' window
     *     switch_tab: switch context to specified 'winpath' tab
     *   Main window/tab has winpath set to .drw,
     *   Additional windows/tabs have winpath set to .x1.drw, .x2.drw and so on...
     */
    else if(!strcmp(argv[1], "new_schematic"))
    {
      int r = -1;
      char s[20];
      if(argc > 2) {
        if(argc == 3) r = new_schematic(argv[2], NULL, NULL);
        else if(argc == 4) r = new_schematic(argv[2], argv[3], NULL);
        else if(argc == 5) r = new_schematic(argv[2], argv[3], argv[4]);
        my_snprintf(s, S(s), "%d", r);
        Tcl_SetResult(interp, s, TCL_VOLATILE);
      }
    }

    /* new_symbol_window [f]
     *   Start a new xschem process for a symbol.
     *   If 'f' is given load specified symbol. */
    else if(!strcmp(argv[1], "new_symbol_window"))
    {
      if(argc > 2)  new_xschem_process(argv[2], 1);
      else new_xschem_process("", 1);
      Tcl_ResetResult(interp);
    }

    /* new_window [f]
     *   Start a new xschem process for a schematic.
     *   If 'f' is given load specified schematic. */
    else if(!strcmp(argv[1], "new_window"))
    {
      if(argc > 2)  new_xschem_process(argv[2],0);
      else new_xschem_process("",0);
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'o': /*----------------------------------------------*/
    /* only_probes
     * dim schematic to better show highlights */
    if(!strcmp(argv[1], "only_probes"))
    {
      xctx->only_probes = !xctx->only_probes;
      tclsetboolvar("only_probes", xctx->only_probes);
      toggle_only_probes();
      Tcl_ResetResult(interp);
    }

    /* origin x y [zoom]
     *   Move origin to 'x, y', optionally changing zoom level to 'zoom' */
    else if(!strcmp(argv[1], "origin"))
    {
      if(argc > 3) {
        xctx->xorigin = atof(argv[2]);
        xctx->yorigin = atof(argv[3]);
        if(argc == 5) {
          xctx->zoom = atof(argv[4]);
          xctx->mooz=1/xctx->zoom;
        }
        draw();
      }
    }
    else { cmd_found = 0;}
    break;
    case 'p': /*----------------------------------------------*/
    /* parse_cmd
     *   debug command to test parse_cmd_string()
     *   splits a command string into argv-like arguments
     *   return # of args in *argc
     *   argv[*argc] is always set to NULL */
    if(!strcmp(argv[1], "parse_cmd")) 
    {
      if(argc > 2) {
        int c, i;
        char **av;
        av = parse_cmd_string(argv[2], &c);
        for(i = 0; i < c; ++i) {
          dbg(0, "--> %s\n", av[i]);
        }
      }
    }
  
    /* parselabel str
     *   Debug command to test vector net syntax parser */
    else if(!strcmp(argv[1], "parselabel")) 
    {
      if(argc > 2) {
        parse(argv[2]);
      }
    }

    /* paste [x y]
     *   Paste clipboard. If 'x y' not given user should complete placement in the GUI */
    else if(!strcmp(argv[1], "paste"))
    {
      merge_file(2, ".sch");
      if(argc > 3) {
        xctx->deltax = atof(argv[2]);
        xctx->deltay = atof(argv[3]);
        move_objects(END, 0, 0.0, 0.0);
      }
      Tcl_ResetResult(interp);
    }

    /* pinlist inst
     *   List all pins of instance 'inst' 
     *   Example: xschem pinlist x3
     *   -->  { {0} {name=PLUS dir=in } } { {1} {name=OUT dir=out } }
     *        { {2} {name=MINUS dir=in } }
     */
    else if(!strcmp(argv[1], "pinlist"))
    {
      int i, p, no_of_pins;
      if(argc > 2) {
        if((i = get_instance(argv[2])) < 0 ) {
          Tcl_SetResult(interp, "xschem pinlist: instance not found", TCL_STATIC);
          return TCL_ERROR;
        }
        no_of_pins= (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];
        for(p=0;p<no_of_pins;p++) {
          char s[10];
          my_snprintf(s, S(s), "%d", p);
          if(argc == 4 && argv[3][0]) {
            Tcl_AppendResult(interp, "{ {", s, "} {",
              get_tok_value((xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][p].prop_ptr, argv[3], 0),
              "} } ", NULL);
          } else {
            Tcl_AppendResult(interp, "{ {", s, "} {", 
               (xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][p].prop_ptr, "} } ", NULL);
          }
        }
      }
    }

    /* place_symbol [sym_name] [prop]
     *   Start a GUI placement operation of specified 'sym_name' symbol.
     *   If 'sym_name' not given prompt user
     *   'prop' is the attribute string of the symbol.
     *   If not given take from symbol template attribute.
     */
    else if(!strcmp(argv[1], "place_symbol"))
    {
      int ret;
      xctx->semaphore++;
      rebuild_selected_array();
      if(xctx->lastsel && xctx->sel_array[0].type==ELEMENT) {
        tclvareval("set INITIALINSTDIR [file dirname {",
             abs_sym_path(xctx->inst[xctx->sel_array[0].n].name, ""), "}]", NULL);
      }
      unselect_all(1);
      xctx->mx_double_save = xctx->mousex_snap;
      xctx->my_double_save = xctx->mousey_snap;
      if(argc > 3) {
        /*               pos  name     x                  y               rot flip prop   draw first to_push_undo */
        ret = place_symbol(-1,argv[2],xctx->mousex_snap, xctx->mousey_snap, 0, 0,  argv[3], 4,   1,    1);
      } else if(argc > 2) {
        ret = place_symbol(-1,argv[2],xctx->mousex_snap, xctx->mousey_snap, 0, 0,  NULL,    4,   1,    1);
      } else {
        xctx->last_command = 0;
        ret = place_symbol(-1,NULL,   xctx->mousex_snap, xctx->mousey_snap, 0, 0,  NULL,    4,   1,    1);
      }
      if(ret) {
        xctx->mousey_snap = xctx->my_double_save;
        xctx->mousex_snap = xctx->mx_double_save;
        move_objects(START,0,0,0);
        xctx->ui_state |= PLACE_SYMBOL;
      }
      xctx->semaphore--;
      Tcl_ResetResult(interp);
    }

    /* place_text 
     *   Start a GUI placement of a text object */
    else if(!strcmp(argv[1], "place_text"))
    {
      xctx->semaphore++;
      xctx->last_command = 0;
      unselect_all(1);
      xctx->mx_double_save = xctx->mousex_snap;
      xctx->my_double_save = xctx->mousey_snap;
      if(place_text(0, xctx->mousex_snap, xctx->mousey_snap)) { /* 1 = draw text 24122002 */
        xctx->mousey_snap = xctx->my_double_save;
        xctx->mousex_snap = xctx->mx_double_save;
        move_objects(START,0,0,0);
        xctx->ui_state |= PLACE_TEXT;
      }
      xctx->semaphore--;
      Tcl_ResetResult(interp);
    }

    /* polygon
     *   Start a GUI placement of a polygon */
    else if(!strcmp(argv[1], "polygon"))
    {
      xctx->ui_state |= MENUSTARTPOLYGON;
    }

    /* preview_window create|draw|destroy [winpath] [file]
     *   Used in fileselector to show a schematic preview.
     */
    else if(!strcmp(argv[1], "preview_window"))
    {
      if(argc == 3) preview_window(argv[2], "{}", "{}");
      else if(argc == 4) preview_window(argv[2], argv[3], "{}");
      else if(argc == 5) preview_window(argv[2], argv[3], argv[4]);
      Tcl_ResetResult(interp);
    }


    /* print png|svg|ps|pdf img_file img_x img_y [x1 y1 x2 y2]
     *   Export current schematic to image.
     *                            img x   y size    xschem area to export
     *      0     1    2    3         4   5             6    7   8   9
     *   xschem print png file.png   400 300        [ -300 -200 300 200 ]
     *   xschem print svg file.svg   400 300        [ -300 -200 300 200 ]
     *   xschem print ps  file.ps
     *   xschem print pdf file.pdf
     */
    else if(!strcmp(argv[1], "print") )
    {
      if(argc < 3) {
        Tcl_SetResult(interp, "xschem print needs at least 1 more arguments: plot_type", TCL_STATIC);
        return TCL_ERROR;
      }
      if(argc > 3) {
        tclvareval("file normalize {", argv[3], "}", NULL);
        my_strncpy(xctx->plotfile, Tcl_GetStringResult(interp), S(xctx->plotfile));
      }
      if(!strcmp(argv[2], "pdf") || !strcmp(argv[2],"ps")) {
        ps_draw(7);
      }
      else if(!strcmp(argv[2], "png")) {
        int w = 0, h = 0;
        double x1, y1, x2, y2;
        if(argc == 6) {
          w = atoi(argv[4]);
          h = atoi(argv[5]);
          if(w == 0) w = xctx->xrect[0].width;
          if(h == 0) h = xctx->xrect[0].height;
          save_restore_zoom(1);
          set_viewport_size(w, h, 1.0);
          zoom_full(0, 0, 2, 0.97);
          resetwin(1, 1, 1, w, h);
          print_image();
          save_restore_zoom(0);
          resetwin(1, 1, 1, 0, 0);
          change_linewidth(-1.);
        } else if(argc == 10) {
          w = atoi(argv[4]);
          h = atoi(argv[5]);
          x1 = atof(argv[6]);
          y1 = atof(argv[7]);
          x2 = atof(argv[8]);
          y2 = atof(argv[9]);
          if(w == 0) w = (int) fabs(x2 - x1);
          if(h == 0) h = (int) fabs(y2 - y1);
          save_restore_zoom(1);
          set_viewport_size(w, h, 1.0); 
          zoom_box(x1, y1, x2, y2, 1.0);
          resetwin(1, 1, 1, w, h);
          print_image();
          save_restore_zoom(0);
          resetwin(1, 1, 1, 0, 0);
          change_linewidth(-1.);
        } else {
          print_image();
        }
      }
      else if(!strcmp(argv[2], "svg")) {
        int w = 0, h = 0;
        double x1, y1, x2, y2;
        if(argc == 6) {
          w = atoi(argv[4]);
          h = atoi(argv[5]);
          save_restore_zoom(1);
          set_viewport_size(w, h, 1.0);
          zoom_full(0, 0, 2, 0.97);
          svg_draw();
          save_restore_zoom(0);
        } else if(argc == 10) {
          w = atoi(argv[4]);
          h = atoi(argv[5]);
          x1 = atof(argv[6]);
          y1 = atof(argv[7]);
          x2 = atof(argv[8]);
          y2 = atof(argv[9]);
          save_restore_zoom(1);
          set_viewport_size(w, h, 1.0);
          zoom_box(x1, y1, x2, y2, 1.0);
          svg_draw();
          save_restore_zoom(0);
        } else {
          svg_draw();
        }
      }
      draw();
      Tcl_ResetResult(interp);
    }

    /* print_hilight_net show
     *   from highlighted nets/pins/labels:
     *   show == 0   ==> create pins from highlight nets
     *   show == 1   ==> show list of highlight net in a dialog box
     *   show == 2   ==> create labels with i prefix from hilight nets
     *   show == 3   ==> show list of highlight net with path and label
     *                  expansion in a dialog box
     *   show == 4   ==> create labels without i prefix from hilight nets
     *   for show = 0, 2, 4 user should complete GUI placement
     *   of created objects */
    else if(!strcmp(argv[1], "print_hilight_net"))
    {
      if(argc > 2) {
        print_hilight_net(atoi(argv[2]));
      }
    }

    /* print_spice_element inst
     *   Print spice raw netlist line for instance (number or name) 'inst' */
    else if(!strcmp(argv[1], "print_spice_element") )
    {
      if(argc > 2) {
        int inst;
        if((inst = get_instance(argv[2])) < 0 ) {
          Tcl_SetResult(interp, "xschem replace_symbol: instance not found", TCL_STATIC);
          return TCL_ERROR;
        }
        print_spice_element(stderr, inst);
      }
    }

    
    /* propagate_hilights [set clear] 
     *   Debug: wrapper to propagate_hilights() function */
    else if(!strcmp(argv[1], "propagate_hilights"))
    {
      int set = 1, clear = 0;
      if(argc > 3) {
         set = atoi(argv[2]);
         clear = atoi(argv[3]);
      }
      propagate_hilights(set, clear, XINSERT_NOREPLACE);
    }

    /* push_undo
     *   Push current state on undo stack */
    else if(!strcmp(argv[1], "push_undo"))
    {
      xctx->push_undo();
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'r': /*----------------------------------------------*/
    /* raw_clear 
     *   Delete loaded simulation raw file */
    if(!strcmp(argv[1], "raw_clear"))
    {
      tclsetvar("rawfile_loaded", "0");
      free_rawfile(1);
      Tcl_ResetResult(interp);
    }

    /* raw_query loaded|value|index|values|datasets|vars|list 
     *   xschem raw_query list: get list of saved simulation variables
     *   xschem raw_query vars: get number of simulation variables
     *   xschem raw_query datasets: get number of datasets (simulation runs)
     *   xschem raw_query value node n: return n-th value of 'node' in raw file
     *   xschem raw_query loaded: return hierarchy level
     *   where raw file was loaded or -1 if no raw loaded
     *   xschem raw_query index node: get index of simulation variable 'node'. 
     *     Example:  raw_query index v(led) --> 46
     *   xschem raw_query values node [dset] : print all simulation
     *   values of 'node' for dataset 'dset' (default dset=0)
     *   xschem raw_query points [dset] : print simulation points for
     *   dataset 'dset' (default dset=0)
     */
    else if(!strcmp(argv[1], "raw_query"))
    {
      int i;
      Tcl_ResetResult(interp);
      if(argc > 2 && !strcmp(argv[2], "loaded")) {
        Tcl_SetResult(interp, my_itoa(sch_waves_loaded()), TCL_VOLATILE);
      } else if(xctx->graph_values) {
        /* xschem rawfile_query value v(ldcp) 123 */
        if(argc > 4 && !strcmp(argv[2], "value")) {
          int dataset = -1;
          int point = atoi(argv[4]);
          const char *node = argv[3];
          int idx = -1;
          if(argc > 5) dataset = atoi(argv[5]);
          if((dataset >= 0 && point >= 0 && point < xctx->graph_npoints[dataset]) ||
              (point >= 0 && point < xctx->graph_allpoints)) {
            if(isonlydigit(node)) {
              int i = atoi(node);
              if(i >= 0 && i < xctx->graph_nvars) {
                idx = i;
              }
            } else {
              idx = get_raw_index(node);
            }
            if(idx >= 0) {
              double val = get_raw_value(dataset, idx, point);
              Tcl_SetResult(interp, dtoa(val), TCL_VOLATILE);
            }
          }
        } else if(argc > 3 && !strcmp(argv[2], "index")) {
          /* xschem rawfile_query index v(ldcp) */
          int idx;
          idx = get_raw_index(argv[3]);
          Tcl_SetResult(interp, my_itoa(idx), TCL_VOLATILE);
        } else if(argc > 3 && !strcmp(argv[2], "values")) {
          /* xschem raw_query values ldcp [dataset] */
          int idx;
          char n[70];
          int p, dataset = 0;
          idx = get_raw_index(argv[3]);
          if(argc > 4) dataset = atoi(argv[4]);
          if(idx >= 0) {
            int np =  xctx->graph_npoints[dataset];
            Tcl_ResetResult(interp);
            for(p = 0; p < np; p++) {
              sprintf(n, "%.10e", get_raw_value(dataset, idx, p));
              Tcl_AppendResult(interp, n, " ", NULL);
            }
          }
        } else if(argc > 2 && !strcmp(argv[2], "datasets")) {
          Tcl_SetResult(interp, my_itoa(xctx->graph_datasets), TCL_VOLATILE); 
        } else if(argc > 2 && !strcmp(argv[2], "points")) {
          int dset = -1;
          if(argc > 3) dset = atoi(argv[3]);
          if(dset == -1) Tcl_SetResult(interp, my_itoa(xctx->graph_allpoints), TCL_VOLATILE);
          else {
            if(dset >= 0 && dset <  xctx->graph_datasets) 
                Tcl_SetResult(interp, my_itoa(xctx->graph_npoints[dset]), TCL_VOLATILE);
          }
        } else if(argc > 2 && !strcmp(argv[2], "vars")) {
          Tcl_SetResult(interp, my_itoa(xctx->graph_nvars), TCL_VOLATILE);
        } else if(argc > 2 && !strcmp(argv[2], "list")) {
          for(i = 0 ; i < xctx->graph_nvars; ++i) {
            if(i > 0) Tcl_AppendResult(interp, "\n", NULL);
            Tcl_AppendResult(interp, xctx->graph_names[i], NULL);
          }
        }
      }
    }

    /* raw_read [file] [sim]
     *   If a raw file is already loaded delete from memory
     *   else load specified file and analysis 'sim' (dc, ac, tran, op, ...)
     *   If 'sim' not specified load first section found in raw file. */
    else if(!strcmp(argv[1], "raw_read"))
    {
      if(sch_waves_loaded() >= 0) {
        free_rawfile(1);
        tclsetvar("rawfile_loaded", "0");
      } else if(argc > 2) {
        free_rawfile(0);
        if(argc > 3) raw_read(argv[2], argv[3]);
        else raw_read(argv[2], NULL);
        if(sch_waves_loaded() >= 0) {
          tclsetvar("rawfile_loaded", "1");
          draw();
        }
        else  tclsetvar("rawfile_loaded", "0");
      }
      Tcl_ResetResult(interp);
    }

    /* raw_read_from_attr [sim]
     *   If a simulation raw file is already loaded delete from memory
     *   else read section 'sim' (tran, dc, ac, op, ...)
     *   of base64 encoded data from a 'spice_data'
     *   attribute of selected instance
     *   If sim not given read first section found */
    else if(!strcmp(argv[1], "raw_read_from_attr"))
    {
      if(sch_waves_loaded() >= 0) {
        free_rawfile(1);
      } else {
        free_rawfile(0);
        if(argc > 2) raw_read_from_attr(argv[2]);
        else  raw_read_from_attr(NULL);
        if(sch_waves_loaded() >= 0) {
          tclsetvar("rawfile_loaded", "1");
          draw();
        }
        else  tclsetvar("rawfile_loaded", "0");
      }
      Tcl_ResetResult(interp);
    }

    /* rebuild_connectivity 
         Rebuild logical connectivity abstraction of schematic */
    else if(!strcmp(argv[1], "rebuild_connectivity"))
    {
      xctx->prep_hash_inst=0;
      xctx->prep_hash_wires=0;
      xctx->prep_net_structs=0;
      xctx->prep_hi_structs=0;
      prepare_netlist_structs(1);
      Tcl_ResetResult(interp);
    }

    /* rebuild_selection 
         Rebuild selection list*/
    else if(!strcmp(argv[1], "rebuild_selection"))
    {
      rebuild_selected_array();
    }

    /* rect [x1 y1 x2 y2] [pos]
     *   if 'x1 y1 x2 y2'is given place recangle on current
     *   layer (rectcolor) at indicated coordinates.
     *   if 'pos' is given insert at given position in rectangle array.
     *   If no coordinates are given start a GUI operation of rectangle placement */
    else if(!strcmp(argv[1], "rect"))
    {
      double x1,y1,x2,y2;
      int pos, save;
      if(argc > 5) {
        x1=atof(argv[2]);
        y1=atof(argv[3]);
        x2=atof(argv[4]);
        y2=atof(argv[5]);
        ORDER(x1,y1,x2,y2);
        pos=-1;
        if(argc==7) pos=atoi(argv[6]);
        storeobject(pos, x1,y1,x2,y2,xRECT,xctx->rectcolor,0,NULL);
        save = xctx->draw_window; xctx->draw_window = 1;
        drawrect(xctx->rectcolor,NOW, x1,y1,x2,y2, 0);
        xctx->draw_window = save;
      }
      else xctx->ui_state |= MENUSTARTRECT;
    }

    /* redo
     *   Redo last undone action */
    else if(!strcmp(argv[1], "redo"))
    {
      xctx->pop_undo(1, 1); /* 2nd param: set_modify_status */
      Tcl_ResetResult(interp);
    }

    /* redraw
     *   redraw window */
    else if(!strcmp(argv[1], "redraw"))
    {
      draw();
      Tcl_ResetResult(interp);
    }

    /* reload
     *   Forced (be careful!) Reload current schematic from disk */
    else if(!strcmp(argv[1], "reload"))
    {
      unselect_all(1);
      remove_symbols();
      load_schematic(1, xctx->sch[xctx->currsch], 1);
      if(argc > 2 && !strcmp(argv[2], "zoom_full") ) {
        zoom_full(1, 0, 1, 0.97);
      } else {
        draw();
      }
      Tcl_ResetResult(interp);
    }

    /* reload_symbols
     *   Reload all used symbols from disk */
    else if(!strcmp(argv[1], "reload_symbols"))
    {
      remove_symbols();
      link_symbols_to_instances(-1);
      draw();
      Tcl_ResetResult(interp);
    }

    /* remove_symbols
     *   Internal command: remove all symbol definitions */
    else if(!strcmp(argv[1], "remove_symbols"))
    {
      remove_symbols();
      Tcl_ResetResult(interp);
    }

    /* replace_symbol inst new_symbol [fast]
     *   Replace 'inst' symbol with 'new_symbol'
     *   If doing multiple substitutions set 'fast' to 0
     *    on first call and non zero on next calls
     *   for faster operation
     *   Example: xschem replace_symbol R3 capa.sym */
    else if(!strcmp(argv[1], "replace_symbol"))
    {
      int inst, fast = 0;
      if(argc == 5) {
        if(!strcmp(argv[4], "fast")) {
          fast = 1;
          argc = 4;
        }
      }
      if(argc!=4) {
        Tcl_SetResult(interp, "xschem replace_symbol needs 2 additional arguments", TCL_STATIC);
        return TCL_ERROR;
      }
      if((inst = get_instance(argv[2])) < 0 ) {
        Tcl_SetResult(interp, "xschem replace_symbol: instance not found", TCL_STATIC);
        return TCL_ERROR;
      } else {
        char symbol[PATH_MAX];
        int sym_number, prefix, cond;
        char *type;
        char *name=NULL;
        char *ptr=NULL;
        bbox(START,0.0,0.0,0.0,0.0);
        my_strncpy(symbol, argv[3], S(symbol));
        xctx->push_undo();
        set_modify(1);
        if(!fast) {
          xctx->prep_hash_inst=0;
          xctx->prep_net_structs=0;
          xctx->prep_hi_structs=0;
        }
        sym_number=match_symbol(symbol);
        if(sym_number>=0)
        {
          prefix=(get_tok_value( (xctx->sym+sym_number)->templ , "name",0))[0]; /* get new symbol prefix  */
        }
        else prefix = 'x';
        delete_inst_node(inst); /* 20180208 fix crashing bug: delete node info if changing symbol */
                             /* if number of pins is different we must delete these data *before* */
                             /* changing ysmbol, otherwise i might end up deleting non allocated data. */
        my_strdup2(_ALLOC_ID_, &xctx->inst[inst].name, rel_sym_path(symbol));
        xctx->inst[inst].ptr=sym_number;
        symbol_bbox(inst, &xctx->inst[inst].x1, &xctx->inst[inst].y1, &xctx->inst[inst].x2, &xctx->inst[inst].y2);
        bbox(ADD, xctx->inst[inst].x1, xctx->inst[inst].y1, xctx->inst[inst].x2, xctx->inst[inst].y2);
        my_strdup(_ALLOC_ID_, &name, xctx->inst[inst].instname);
        if(name && name[0] )
        {
          /* 20110325 only modify prefix if prefix not NUL */
          if(prefix) name[0]=(char)prefix; /* change prefix if changing symbol type; */
          my_strdup(_ALLOC_ID_, &ptr,subst_token(xctx->inst[inst].prop_ptr, "name", name) );
          hash_all_names();
          new_prop_string(inst, ptr,0, tclgetboolvar("disable_unique_names")); /* set new prop_ptr */
          type=xctx->sym[xctx->inst[inst].ptr].type;
          cond= !type || !IS_LABEL_SH_OR_PIN(type);
          if(cond) xctx->inst[inst].flags|=2;
          else {
            xctx->inst[inst].flags &=~2;
            my_strdup(_ALLOC_ID_, &xctx->inst[inst].lab, get_tok_value(xctx->inst[inst].prop_ptr, "lab", 0));
          }
          xctx->inst[inst].embed = !strcmp(get_tok_value(xctx->inst[inst].prop_ptr, "embed", 2), "true");
          my_free(_ALLOC_ID_, &ptr);
        }
        my_free(_ALLOC_ID_, &name);
        /* new symbol bbox after prop changes (may change due to text length) */
        symbol_bbox(inst, &xctx->inst[inst].x1, &xctx->inst[inst].y1, &xctx->inst[inst].x2, &xctx->inst[inst].y2);
        bbox(ADD, xctx->inst[inst].x1, xctx->inst[inst].y1, xctx->inst[inst].x2, xctx->inst[inst].y2);
        /* redraw symbol */
        bbox(SET,0.0,0.0,0.0,0.0);
        draw();
        bbox(END,0.0,0.0,0.0,0.0);
        Tcl_SetResult(interp, xctx->inst[inst].instname , TCL_VOLATILE);
      }
    }

    /* rotate
     *   Rotate selected objects around their centers */
    else if(!strcmp(argv[1], "rotate"))
    {
      if(! (xctx->ui_state & (STARTMOVE | STARTCOPY) ) ) { 
        rebuild_selected_array();
        move_objects(START,0,0,0);
        move_objects(ROTATE|ROTATELOCAL,0,0,0);
        move_objects(END,0,0,0);
      }
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 's': /*----------------------------------------------*/
    /* save
     *   Save schematic if modified. Does not ask confirmation! */
    if(!strcmp(argv[1], "save"))
    {
      dbg(1, "scheduler(): saving: current schematic\n");
      if(!strcmp(xctx->sch[xctx->currsch], "")) {   /* check if unnamed schematic, use saveas in this case... */
        saveas(NULL, SCHEMATIC);
      } else {
        save(0);
      }
    }

    /* saveas [file] [type]
     *   save current schematic as 'file'
     *   if file is empty ({}) use current schematic name
     *   as defalt and prompt user with file selector
     *   'type' is used used to set/change file extension:
     *     schematic: save as schematic (*.sch)
     *     symbol: save as symbol (*.sym)
     *     If not specified default to schematic (*.sch)
     *   Does not ask confirmation if file name given
     */
    else if(!strcmp(argv[1], "saveas"))
    {
      const char *f;
      if(argc > 3) {
        f = !strcmp(argv[2], "") ? NULL : argv[2];
        if(!strcmp(argv[3], "schematic")) saveas(f, SCHEMATIC);
        else if(!strcmp(argv[3], "symbol")) saveas(f, SYMBOL);
        else saveas(f, SCHEMATIC);
      }
      else if(argc > 2) {
        f = !strcmp(argv[2], "") ? NULL : argv[2];
        saveas(f, SCHEMATIC);
      }
      else saveas(NULL, SCHEMATIC);
    }

    /* schematic_in_new_window [new_process]
     *   When a symbol is selected edit corresponding schematic
     *   in a new tab/window if not already open.
     *   If nothing selected open another window of the second
     *   schematic (issues a warning).
     *   if 'new_process' is given start a new xschem process */
    else if(!strcmp(argv[1], "schematic_in_new_window"))
    {
      int new_process = 0;
      if(argc > 2 && !strcmp(argv[2], "new_process")) new_process = 1;
      schematic_in_new_window(new_process);
      Tcl_ResetResult(interp);
    }

    /* search regex|exact select tok val
     *   Search instances with attribute string containing 'tok'
     *   attribute and value 'val'
     *   search can be exact ('exact') or as a regular expression ('regex')
     *   select: 
     *      0 : highlight matching instances
     *      1 : select matching instances
     *     -1 : unselect matching instances 
     *   'tok' set as:
     *       propstring : will search for 'val' in the entire 
     *       *instance* attribute string.
     *       cell::propstring : will search for 'val' in the entire
     *       *symbol* attribute string. 
     *       cell::name : will search for 'val' in the symbol name
     *       cell::<attr> will search for 'val' in symbol attribute 'attr'
     *         example: xschem search regex 0 cell::template GAIN=100
     */
    else if(!strcmp(argv[1], "search") || !strcmp(argv[1], "searchmenu"))
    {
      /*   0      1         2        3       4   5   */
      /*                           select            */
      /* xschem search regex|exact 0|1|-1   tok val  */
      int select, r;
      if(argc < 6) {
        Tcl_SetResult(interp, "xschem search requires 4 or 5 additional fields.", TCL_STATIC);
        return TCL_ERROR;
      }
      if(argc > 5) {
        select = atoi(argv[3]);
        if(!strcmp(argv[2], "regex") )  r = search(argv[4],argv[5],0,select);
        else  r = search(argv[4],argv[5],1,select);
        if(r == 0) {
          if(has_x && !strcmp(argv[1], "searchmenu")) 
            tcleval("tk_messageBox -type ok -parent [xschem get topwindow] -message {Not found.}");
          Tcl_SetResult(interp, "0", TCL_STATIC);
        } else {
          Tcl_SetResult(interp, "1", TCL_STATIC);
        }
        return TCL_OK;
      }
    }

    /* select instance|wire|text id [clear]
     * Select indicated instance or wire or text.
     * For 'instance' 'id' can be the instance name or number
     * for 'wire' or 'text' 'id' is the position in the respective arrays
     * if 'clear' is specified does an unselect operation */
    else if(!strcmp(argv[1], "select"))
    {
      short unsigned int  sel = SELECTED;
      if(argc < 3) {
        Tcl_SetResult(interp, "xschem select: missing arguments.", TCL_STATIC);
        return TCL_ERROR;
      }
      if(argc > 4 && !strcmp(argv[4], "clear")) sel = 0;
      if(!strcmp(argv[2], "instance") && argc > 3) {
        int i;
        /* find by instance name  or number*/
        i = get_instance(argv[3]);
        if(i >= 0) {
           select_element(i, sel, 0, 0);
        }
        Tcl_SetResult(interp, (i >= 0) ? "1" : "0" , TCL_STATIC);
      }
      else if(!strcmp(argv[2], "wire") && argc > 3) {
        int n=atoi(argv[3]);
        if(n<xctx->wires && n >= 0) select_wire(atoi(argv[3]), sel, 0);
      }
      else if(!strcmp(argv[2], "text") && argc > 3) {
        int n=atoi(argv[3]);
        if(n<xctx->texts && n >= 0) select_text(atoi(argv[3]), sel, 0);
      }
      drawtemparc(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
      drawtemprect(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
      drawtempline(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
    }

    /* select_all
     *   Selects all objects in schematic */
    else if(!strcmp(argv[1], "select_all"))
    {
      select_all();
      Tcl_ResetResult(interp);
    }

    /* select_hilight_net
     *   Select all highlight objects (wires, labels, pins, instances) */
    else if(!strcmp(argv[1], "select_hilight_net"))  
    {
      select_hilight_net();
      Tcl_ResetResult(interp);
    }

    /* selected_set
     *   Return a list of selected instance names */
    else if(!strcmp(argv[1], "selected_set"))
    {
      int n, i;
      rebuild_selected_array();
      for(n=0; n < xctx->lastsel; ++n) {
        if(xctx->sel_array[n].type == ELEMENT) {
          i = xctx->sel_array[n].n;
          Tcl_AppendResult(interp, /* "{", */ xctx->inst[i].instname, " ", /* "} ", */ NULL);
        }
      }
    }

    /* selected_wire
     *  Return list of selected nets */
    else if(!strcmp(argv[1], "selected_wire"))
    {
      int n, i;
      rebuild_selected_array();
      for(n=0; n < xctx->lastsel; ++n) {
        if(xctx->sel_array[n].type == WIRE) {
          i = xctx->sel_array[n].n;
          Tcl_AppendResult(interp, get_tok_value(xctx->wire[i].prop_ptr, "lab",0), " ", NULL);
        }
      }
    }

    /* send_to_viewer
     *   Send selected wires/net labels/pins/voltage source or ammeter currents to current
     *   open viewer (gaw or bespice) */
    else if(!strcmp(argv[1], "send_to_viewer"))
    {
      int viewer = 0;
      int exists = 0;
      char *viewer_name = NULL;
      char tcl_str[200];
      tcleval("info exists sim");
      if(tclresult()[0] == '1') exists = 1;
      xctx->enable_drill = 0;
      if(exists) {
        viewer = atoi(tclgetvar("sim(spicewave,default)"));
        my_snprintf(tcl_str, S(tcl_str), "sim(spicewave,%d,name)", viewer);
        my_strdup(_ALLOC_ID_, &viewer_name, tclgetvar(tcl_str));
        dbg(1, "send_to_viewer: viewer_name=%s\n", viewer_name);
        if(strstr(viewer_name, "Gaw")) viewer=GAW;
        else if(strstr(viewer_name, "Bespice")) viewer=BESPICE;
        if(viewer) {
          hilight_net(viewer);
          redraw_hilights(0);
        }
        my_free(_ALLOC_ID_, &viewer_name);
      }
      Tcl_ResetResult(interp);
    }

    /* set var value
     *   Set C variable 'var' to 'value' */
    else if(!strcmp(argv[1], "set"))
    {
      if(argc > 3) {
        if(argv[2][0] < 'n') {
          if(!strcmp(argv[2], "cadgrid")) { /* set cad grid (default: 20) */
            set_grid( atof(argv[3]) );
          }
          else if(!strcmp(argv[2], "cadsnap")) { /* set mouse snap (default: 10) */
            set_snap( atof(argv[3]) );
          }
          else if(!strcmp(argv[2], "color_ps")) { /* set color psoscript (1 or 0) */
            color_ps=atoi(argv[3]);
          }
          else if(!strcmp(argv[2], "constrained_move")) { /* set constrained move (1=horiz, 2=vert, 0=none) */
            constrained_move = atoi(argv[3]);
          }
          else if(!strcmp(argv[2], "draw_window")) { /* set drawing to window (1 or 0) */
            xctx->draw_window=atoi(argv[3]);
          }
          else if(!strcmp(argv[2], "format")) { /* set name of custom format attribute used for netlisting */
            my_strdup(_ALLOC_ID_, &xctx->format, argv[3]);
          }
          else if(!strcmp(argv[2], "header_text")) { /* set header metadata (used for license info) */
            if(!xctx->header_text || strcmp(xctx->header_text, argv[3])) {
              set_modify(1); xctx->push_undo();
              my_strdup2(_ALLOC_ID_, &xctx->header_text, argv[3]);
            } 
          }
          else if(!strcmp(argv[2], "hide_symbols")) { /* set to 0,1,2 for various hiding level of symbols */
            xctx->hide_symbols=atoi(argv[3]);
          }
        } else { /* argv[2][0] >= 'n' */
          if(!strcmp(argv[2], "netlist_name")) { /* set custom netlist name */
            my_strncpy(xctx->netlist_name, argv[3], S(xctx->netlist_name));
          }
          else if(!strcmp(argv[2], "netlist_type")) /* set netlisting mode (spice, verilog, vhdl, tedax, symbol) */
          {
            if(!strcmp(argv[3], "spice")){
              xctx->netlist_type=CAD_SPICE_NETLIST;
            }
            else if(!strcmp(argv[3], "vhdl")) {
              xctx->netlist_type=CAD_VHDL_NETLIST;
            }
            else if(!strcmp(argv[3], "verilog")) {
              xctx->netlist_type=CAD_VERILOG_NETLIST;
            }
            else if(!strcmp(argv[3], "tedax")) {
              xctx->netlist_type=CAD_TEDAX_NETLIST;
            }
            else if(!strcmp(argv[3], "symbol")) {
              xctx->netlist_type=CAD_SYMBOL_ATTRS;
            }
            else {
              dbg(0, "Warning: undefined netlist format: %s\n", argv[3]);
            }
            set_tcl_netlist_type();
          }
          else if(!strcmp(argv[2], "no_draw")) { /* set no drawing flag (0 or 1) */
            int s = atoi(argv[3]);
            xctx->no_draw=s;
          }
          else if(!strcmp(argv[2], "no_undo")) { /* set to 1 to disable undo */
            int s = atoi(argv[3]);
            xctx->no_undo=s;
          }
          else if(!strcmp(argv[2], "rectcolor")) { /* set current layer (0, 1, .... , cadlayers-1) */
            xctx->rectcolor=atoi(argv[3]);
            if(xctx->rectcolor < 0 ) xctx->rectcolor = 0;
            if(xctx->rectcolor >= cadlayers ) xctx->rectcolor = cadlayers - 1;
            rebuild_selected_array();
            if(xctx->lastsel) {
              change_layer();
            }
          }
          else if(!strcmp(argv[2], "sch_to_compare")) { /* st name of schematic to compare current window with */
            my_strncpy(xctx->sch_to_compare, abs_sym_path(argv[3], ""), S(xctx->sch_to_compare));
          }
          else if(!strcmp(argv[2], "text_svg")) { /* set to 1 to use svg <text> elements */
            text_svg=atoi(argv[3]);
          }
          else if(!strcmp(argv[2], "semaphore")) { /* debug */
            dbg(1, "scheduler(): set semaphore to %s\n", argv[3]);
            xctx->semaphore=atoi(argv[3]);
          }
          else if(!strcmp(argv[2], "show_hidden_texts")) { /* set to 1 to enable showing texts with attr hide=true */
            dbg(1, "scheduler(): set show_hidden_texts to %s\n", argv[3]);
            xctx->show_hidden_texts=atoi(argv[3]);
          }
          else if(!strcmp(argv[2], "sym_txt")) { /* set to 0 to hide symbol texts */
            xctx->sym_txt=atoi(argv[3]);
          }
          else {
            cmd_found = 0;
          }
        } /* argv[2][0] >= 'n' */
      } /* if(argc > 3 */
    }
    /************ end xschem set subcommands *************/
    /* set_different_tok str new_str old_str
     *   Return string 'str' replacing/adding/removing tokens that are
     *   different between 'new_str' and 'old_str' */
    else if(!strcmp(argv[1], "set_different_tok") )
    {
      char *s = NULL;
      if(argc < 5) {Tcl_SetResult(interp, "Missing arguments", TCL_STATIC);return TCL_ERROR;}
      my_strdup(_ALLOC_ID_, &s, argv[2]);
      set_different_token(&s, argv[3], argv[4]);
      Tcl_SetResult(interp, s, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &s);
    }

    /* set_modify
     *   Force modify status on current schematic */
    else if(!strcmp(argv[1], "set_modify"))
    {
      if(argc > 2) {
        set_modify(atoi(argv[2]));
      }
      Tcl_ResetResult(interp);
    }
    /* setprop instance inst tok [val] [fast]
     *   set attribute 'tok' of instance (name or number) 'inst' to value 'val'
     *   If 'val' not given (no attribute value) delete attribute from instance
     *   If 'fast' argument if given does not redraw and is not undoable 
     * setprop rect lay n tok [val] [fast|fastundo]
     *   Set attribute 'tok' of rectangle number'n' on layer 'lay'
     *   If 'val' not given (no attribute value) delete attribute from rect
     *   If 'fast' argument is given does not redraw and is not undoable
     *   If 'fastundo' s given same as above but action is undoable.
     * setprop rect 2 n fullxzoom
     * setprop rect 2 n fullyzoom
     *   These commands do full x/y zoom of graph 'n' (on layer 2, this is hardcoded).
     * setprop text n tok [val] [fast|fastundo]
     *   Set attribute 'tok' of text number 'n'
     *   If 'val' not given (no attribute value) delete attribute from text
     *   If 'fast' argument is given does not redraw and is not undoable
     *   If 'fastundo' s given same as above but action is undoable.
     */
    else if(!strcmp(argv[1], "setprop"))
    {
    /*   0       1       2     3    4     5    6
     * xschem setprop instance R4 value [30k] [fast] */
      if(argc > 2 && !strcmp(argv[2], "instance")) {
        int inst, fast=0;
        if(argc > 6) {
          if(!strcmp(argv[6], "fast")) {
            fast = 1;
            argc = 6;
          }
        }
        else if(argc > 5) {
          if(!strcmp(argv[5], "fast")) {
            fast = 1;
            argc = 5;
          }
        }
        if(argc < 5) {
          Tcl_SetResult(interp, "xschem setprop instance needs 2 or 3 additional arguments", TCL_STATIC);
          return TCL_ERROR;
        }
        if((inst = get_instance(argv[3])) < 0 ) {
          Tcl_SetResult(interp, "xschem setprop: instance not found", TCL_STATIC);
          return TCL_ERROR;
        } else {
          char *type;
          int cond;
          if(!fast) {
            bbox(START,0.0,0.0,0.0,0.0);
            symbol_bbox(inst, &xctx->inst[inst].x1, &xctx->inst[inst].y1, &xctx->inst[inst].x2, &xctx->inst[inst].y2);
            bbox(ADD, xctx->inst[inst].x1, xctx->inst[inst].y1, xctx->inst[inst].x2, xctx->inst[inst].y2);
            xctx->push_undo();
          }
          set_modify(1);
          xctx->prep_hash_inst=0;
          xctx->prep_net_structs=0;
          xctx->prep_hi_structs=0;
          if(!strcmp(argv[4], "name")) hash_all_names();
          if(argc > 5) {
            new_prop_string(inst, subst_token(xctx->inst[inst].prop_ptr, argv[4], argv[5]),fast, 
              tclgetboolvar("disable_unique_names"));
          } else {/* assume argc == 5 , delete attribute */
            new_prop_string(inst, subst_token(xctx->inst[inst].prop_ptr, argv[4], NULL),fast, 
              tclgetboolvar("disable_unique_names"));
          }
          type=xctx->sym[xctx->inst[inst].ptr].type;
          cond= !type || !IS_LABEL_SH_OR_PIN(type);
          if(cond) xctx->inst[inst].flags|=2;
          else {
            xctx->inst[inst].flags &=~2;
            my_strdup(_ALLOC_ID_, &xctx->inst[inst].lab, get_tok_value(xctx->inst[inst].prop_ptr, "lab", 0));
          }

          if(!strcmp(get_tok_value(xctx->inst[inst].prop_ptr,"highlight",0), "true"))
                xctx->inst[inst].flags |= HILIGHT_CONN;
          else  xctx->inst[inst].flags &= ~HILIGHT_CONN;
          if(!strcmp(get_tok_value(xctx->inst[inst].prop_ptr,"hide",0), "true"))
                xctx->inst[inst].flags |= HIDE_INST;
          else  xctx->inst[inst].flags &= ~HIDE_INST;
          xctx->inst[inst].embed = !strcmp(get_tok_value(xctx->inst[inst].prop_ptr, "embed", 2), "true");
          if(!fast) {
            /* new symbol bbox after prop changes (may change due to text length) */
            symbol_bbox(inst, &xctx->inst[inst].x1, &xctx->inst[inst].y1, &xctx->inst[inst].x2, &xctx->inst[inst].y2);
            bbox(ADD, xctx->inst[inst].x1, xctx->inst[inst].y1, xctx->inst[inst].x2, xctx->inst[inst].y2);
            /* redraw symbol with new props */
            bbox(SET,0.0,0.0,0.0,0.0);
            draw();
            bbox(END,0.0,0.0,0.0,0.0);
          }
          Tcl_SetResult(interp, xctx->inst[inst].instname , TCL_VOLATILE);
        }
      } else if(argc > 5 && !strcmp(argv[2], "rect")) {
      /*  0       1      2   3 4   5    6      7
       * xschem setprop rect c n token [value] [fast|fastundo] */
        int change_done = 0;
        int fast = 0;
        xRect *r;
        int c = atoi(argv[3]);
        int n = atoi(argv[4]);
        if(!(c>=0 && c < cadlayers && n >=0 && n < xctx->rects[c]) ) {
          Tcl_SetResult(interp, "xschem setprop rect: wrong layer or rect number", TCL_STATIC);
          return TCL_ERROR;
        }
        r = &xctx->rect[c][n];
        if(argc > 7) {
          if(!strcmp(argv[7], "fast")) {
            fast = 1;
            argc = 7;
          }
          if(!strcmp(argv[7], "fastundo")) {
            fast = 3;
            argc = 7;
          }
        }
        else if(argc > 6) {
          if(!strcmp(argv[6], "fast")) {
            fast = 1;
            argc = 6;
          }
          if(!strcmp(argv[6], "fastundo")) {
            fast = 3;
            argc = 6;
          }
        }
        if(!fast) {
          bbox(START,0.0,0.0,0.0,0.0);
        }
        if(argc > 5 && c == 2 && !strcmp(argv[5], "fullxzoom")) {
          xRect *r = &xctx->rect[c][n];
          Graph_ctx *gr = &xctx->graph_struct;
          int dataset;
          setup_graph_data(n, 0, gr);
          if(gr->dataset >= 0 && gr->dataset < xctx->graph_datasets) dataset = gr->dataset;
          else dataset = -1;
          graph_fullxzoom(r, gr, dataset);
        } 
        if(argc > 5 && c == 2 && !strcmp(argv[5], "fullyzoom")) {
          xRect *r = &xctx->rect[c][n];
          Graph_ctx *gr = &xctx->graph_struct;
          int dataset;
          setup_graph_data(n, 0, gr);
          if(gr->dataset >= 0 && gr->dataset < xctx->graph_datasets) dataset = gr->dataset;
          else dataset = -1;
          graph_fullyzoom(r, gr, dataset);
        }
        else if(argc > 6) {
          /* verify if there is some difference */
          if(strcmp(argv[6], get_tok_value(r->prop_ptr, argv[5], 0))) {
            change_done = 1;
            if(fast == 3 || fast == 0) xctx->push_undo();
            my_strdup2(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, argv[5], argv[6]));
          }
        } else {
          get_tok_value(r->prop_ptr, argv[5], 0);
          if(xctx->tok_size) {
            change_done = 1;
            if(fast == 3 || fast == 0) xctx->push_undo();
            my_strdup2(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, argv[5], NULL)); /* delete attr */
          }
        }
        if(change_done) set_modify(1);
        set_rect_flags(r); /* set cached .flags bitmask from on attributes */
        if(!fast) {
          bbox(ADD, r->x1, r->y1, r->x2, r->y2);
          /* redraw rect with new props */
          bbox(SET,0.0,0.0,0.0,0.0);
          draw();
          bbox(END,0.0,0.0,0.0,0.0);
        }
        Tcl_ResetResult(interp);

      } else if(argc > 4 && !strcmp(argv[2], "text")) {
      /*  0       1      2   3   4    5      6
       * xschem setprop text n token value [fast] */
        int change_done = 0;
        int tmp, fast = 0;
        double xx1, xx2, yy1, yy2, dtmp;
        xText *t;
        int n = atoi(argv[3]);
        if(!(n >=0 && n < xctx->texts) ) {
          Tcl_SetResult(interp, "xschem setprop text: wrong text number", TCL_STATIC);
          return TCL_ERROR;
        }
        t = &xctx->text[n];
        if(argc > 6) {
          if(!strcmp(argv[6], "fast")) {
            fast = 1;
            argc = 6;
          }
          if(!strcmp(argv[6], "fastundo")) {
            fast = 3;
            argc = 6;
          }
        }
        else if(argc > 5) {
          if(!strcmp(argv[5], "fast")) {
            fast = 1;
            argc = 5;
          }
          if(!strcmp(argv[5], "fastundo")) {
            fast = 3;
            argc = 5;
          }
        }
        if(!fast) {
          bbox(START,0.0,0.0,0.0,0.0);
        }
        if(argc > 5) {
          /* verify if there is some difference */
          if(strcmp(argv[5], get_tok_value(t->prop_ptr, argv[4], 0))) {
            change_done = 1;
            if(fast == 3 || fast == 0) xctx->push_undo();
            my_strdup2(_ALLOC_ID_, &t->prop_ptr, subst_token(t->prop_ptr, argv[4], argv[5]));
          }
        } else {
          get_tok_value(t->prop_ptr, argv[4], 0);
          if(xctx->tok_size) {
            change_done = 1;
            if(fast == 3 || fast == 0) xctx->push_undo();
            my_strdup2(_ALLOC_ID_, &t->prop_ptr, subst_token(t->prop_ptr, argv[4], NULL)); /* delete attr */
          }
        }
        if(change_done) set_modify(1);
        set_text_flags(t);
        text_bbox(t->txt_ptr, t->xscale,
                  t->yscale, t->rot, t->flip, t->hcenter,
                  t->vcenter, t->x0, t->y0,
                  &xx1,&yy1,&xx2,&yy2, &tmp, &dtmp);

        if(!fast) {
          bbox(ADD, xx1, yy1, xx2, yy2);
          /* redraw rect with new props */
          bbox(SET,0.0,0.0,0.0,0.0);
          draw();
          bbox(END,0.0,0.0,0.0,0.0);
        }
        Tcl_ResetResult(interp);
      }
    }
    /* simulate
     *   Run a simulation (start simulator configured as default in
     *   Tools -> Configure simulators and tools) */
    else if(!strcmp(argv[1], "simulate") )
    {
      if(set_netlist_dir(0, NULL) ) {
        tcleval("simulate");
      }
    }

    /* snap_wire 
     *   Start a GUI start snapped wire placement (click to start a
     *   wire to closest pin/net endpoint) */
    else if(!strcmp(argv[1], "snap_wire"))
    {
      xctx->ui_state |= MENUSTARTSNAPWIRE;
    }

    /* subst_tok str tok newval
     *   Return string 'str' with 'tok' attribute value replaced with 'newval' */
    else if(!strcmp(argv[1], "subst_tok"))
    {
      char *s=NULL;
      if(argc < 5) {Tcl_SetResult(interp, "Missing arguments", TCL_STATIC);return TCL_ERROR;}
      my_strdup(_ALLOC_ID_, &s, subst_token(argv[2], argv[3], strcmp(argv[4], "NULL") ? argv[4] : NULL));
      Tcl_SetResult(interp, s, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &s);
    }

    /* symbol_in_new_window [new_process]
     *   When a symbol is selected edit it in a new tab/window if not already open.
     *   If nothing selected open another window of the second schematic (issues a warning).
     *   if 'new_process' is given start a new xschem process */
    else if(!strcmp(argv[1], "symbol_in_new_window"))
    {
      int new_process = 0;
      if(argc > 2 && !strcmp(argv[2], "new_process")) new_process = 1;
      symbol_in_new_window(new_process);
      Tcl_ResetResult(interp);
    }

    /* symbols
     *   List all used symbols */
    else if(!strcmp(argv[1], "symbols"))
    {
      int i;
      char n[100];
      Tcl_SetResult(interp, "\n", TCL_STATIC);
      for(i=0; i<xctx->symbols; ++i) {
        my_snprintf(n , S(n), "%d", i);
        Tcl_AppendResult(interp, "  {", n, " ", "{", xctx->sym[i].name, "}", "}\n", NULL);
      }
      Tcl_AppendResult(interp, "\n", NULL);
    }
    else { cmd_found = 0;}
    break;
    case 't': /*----------------------------------------------*/
    /* table_read [table_file]
     *   If a simulation raw file is lodaded unload from memory.
     *   else read a tabular file 'table_file'
     *   First line is the header line containing variable names.
     *   data is presented in column format after the header line
     *   First column is sweep (x-axis) variable
     *   Double empty lines start a new dataset
     *   Single empty lines are ignored
     *   Datasets can have different # of lines.
     *   new dataset do not start with a header row.
     *   Lines beginning with '#' are comments and ignored
     *      
     *      time    var_a   var_b   var_c
     *   # this is a comment, ignored
     *       0.0     0.0     1.8    0.3
     *     <single empty line: ignored>
     *       0.1     0.0     1.5    0.6
     *       ...     ...     ...    ...
     *     <empty line>
     *     <Second empty line: start new dataset>
     *       0.0     0.0     1.8    0.3
     *       0.1     0.0     1.5    0.6
     *       ...     ...     ...    ...
     */
    if(!strcmp(argv[1], "table_read"))
    {
      if(sch_waves_loaded() >= 0) {
        free_rawfile(1);
        tclsetvar("rawfile_loaded", "0");
      } else if(argc > 2) {
        free_rawfile(0);
        table_read(argv[2]);
        if(sch_waves_loaded() >= 0) {
          tclsetvar("rawfile_loaded", "1");
          draw();
        }
        else  tclsetvar("rawfile_loaded", "0");
      } 
      Tcl_ResetResult(interp);
    }
    /* test 
     *   testmode */
    else if(!strcmp(argv[1], "test"))
    {
      Ptr_hashtable table = {NULL, 0};
      Ptr_hashentry *entry;
      double a = 10.1;
      ptr_hash_init(&table, 37);
      ptr_hash_lookup(&table, "val", &a, XINSERT);
    
      entry = ptr_hash_lookup(&table, "val", NULL, XLOOKUP);
      dbg(0, "val=%g\n", *(double *)entry->value);
      ptr_hash_free(&table);
    
      Tcl_ResetResult(interp);
    }

    /* toggle_colorscheme
     *   Toggle dark/light colorscheme */
    else if(!strcmp(argv[1], "toggle_colorscheme"))
    {
      int d_c;
      d_c = tclgetboolvar("dark_colorscheme");
      d_c = !d_c;
      tclsetboolvar("dark_colorscheme", d_c);
      tclsetdoublevar("dim_value", 0.0);
      tclsetdoublevar("dim_bg", 0.0);
      build_colors(0.0, 0.0);
      draw();
      Tcl_ResetResult(interp);
    }

    /* translate n str
     *   Translate string 'str' replacing @xxx tokens with values in instance 'n' attributes */
    else if(!strcmp(argv[1], "translate") )
    {
      if(argc>3) {
        char *s = NULL;
        my_strdup2(_ALLOC_ID_, &s, translate(atoi(argv[2]), argv[3]));
        Tcl_ResetResult(interp);
        Tcl_SetResult(interp, s, TCL_VOLATILE);
        my_free(_ALLOC_ID_, &s);
      }
    }

    /* trim_wires
     *   Remove operlapping wires, join lines, trim wires at intersections */
    else if(!strcmp(argv[1], "trim_wires"))
    {
      xctx->push_undo();
      trim_wires();
      draw();
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'u': /*----------------------------------------------*/
    /* undo
         Undo last action */
    if(!strcmp(argv[1], "undo"))
    {
      int redo = 0, set_modify = 1;
      if(argc > 2) {
        redo = atoi(argv[2]);
      }
      if(argc > 3) {
        set_modify = atoi(argv[3]);
      }
      xctx->pop_undo(redo, set_modify); /* 2nd param: set_modify_status */
      Tcl_ResetResult(interp);
    }

    /* undo_type disk|memory
     *   Use disk file ('disk') or RAM ('memory') for undo bufer
     */
    else if(!strcmp(argv[1], "undo_type"))
    {
      if(argc > 2) {
        dbg(1, "xschem undo_type %s\n", argv[2]);
        if(!strcmp(argv[2], "disk")) {
          if(xctx->undo_type == 1) {
            mem_delete_undo(); /*reset memory undo */
          }
          /* redefine undo function pointers */
          xctx->push_undo = push_undo;
          xctx->pop_undo = pop_undo;
          xctx->delete_undo = delete_undo;
          xctx->clear_undo = clear_undo;
          xctx->undo_type = 0; /* disk */
        } else { /* "memory" */
          if(xctx->undo_type == 0) {
            delete_undo(); /*reset disk undo */
          }
          /* redefine undo function pointers */
          xctx->push_undo = mem_push_undo;
          xctx->pop_undo = mem_pop_undo;
          xctx->delete_undo = mem_delete_undo;
          xctx->clear_undo = mem_clear_undo;
          xctx->undo_type = 1; /* memory */
        }
      }
    }

    /* unhilight_all
     *   Clear all highlights */
    else if(!strcmp(argv[1], "unhilight_all"))
    {
      xRect boundbox;
      int big =  xctx->wires> 2000 || xctx->instances > 2000 ;
      xctx->enable_drill=0;
      if(!big) calc_drawing_bbox(&boundbox, 2);
      clear_all_hilights();
      /* undraw_hilight_net(1); */
      if(!big) {
        bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
        bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
        bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
      }
      draw();
      if(!big) bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
      Tcl_ResetResult(interp);
    }
    
    /* unhilight
     *   Unhighlight selected nets/pins */
    else if(!strcmp(argv[1], "unhilight"))
    {
      unhilight_net();
    }

    /* unselect_all
     *   Unselect everything */
    else if(!strcmp(argv[1], "unselect_all"))
    {
      if(argc > 2) unselect_all(atoi(argv[2]));
      else unselect_all(1);
      Tcl_ResetResult(interp);
    }

    /* update_all_sym_bboxes
     *   Update all symbol bounding boxes (useful if show_pin_net_names is set) */
    else if(!strcmp(argv[1], "update_all_sym_bboxes"))
    {
      int i;
      for(i = 0; i < xctx->instances; ++i) {
        symbol_bbox(i, &xctx->inst[i].x1, &xctx->inst[i].y1, &xctx->inst[i].x2, &xctx->inst[i].y2);
      }
    }

    else { cmd_found = 0;}
    break;
    case 'v': /*----------------------------------------------*/
    /* view_prop
     *   View attributes of selected element (read only)
     *   if multiple selection show the first element (in xschem  array order) */
    if(!strcmp(argv[1], "view_prop"))
    {
      edit_property(2);
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'w': /*----------------------------------------------*/
    /* warning_overlapped_symbols [sel]
         Highlight or select (if 'sel' set to 1) perfectly overlapped instances
         this is usually an error and difficult to grasp visually */
    if(!strcmp(argv[1], "warning_overlapped_symbols"))
    {
      if(argc > 2) {
        warning_overlapped_symbols(atoi(argv[2]));
      } else {
        warning_overlapped_symbols(0);
      }
    }
    /* windowid
     *   Used by xschem.tcl for configure events */
    else if(!strcmp(argv[1], "windowid"))
    {
      if(argc > 2) {
        windowid(argv[2]);
      }
    }

    /* wire [x1 y1 x2 y2] [pos] [prop] [sel]
     *   Place a new wire
     *   if no coordinates are given start a GUI wire placement */
    else if(!strcmp(argv[1], "wire"))
    {
      double x1,y1,x2,y2;
      int pos = -1, save, sel = 0;
      const char *prop=NULL;
      if(argc > 5) {
        x1=atof(argv[2]);
        y1=atof(argv[3]);
        x2=atof(argv[4]);
        y2=atof(argv[5]);
        ORDER(x1,y1,x2,y2);
        if(argc > 6) pos=atoi(argv[6]);
        if(argc > 7) prop = argv[7];
        if(argc > 8) sel = atoi(argv[8]);
        xctx->push_undo();
        storeobject(pos, x1,y1,x2,y2,WIRE,0,(short)sel,prop);
        if(sel) xctx->need_reb_sel_arr=1;
        xctx->prep_hi_structs=0;
        xctx->prep_net_structs=0;
        xctx->prep_hash_wires=0;
        save = xctx->draw_window; xctx->draw_window = 1;
        drawline(WIRELAYER,NOW, x1,y1,x2,y2, 0, NULL);
        xctx->draw_window = save;
        if(tclgetboolvar("autotrim_wires")) trim_wires();
      }
      else xctx->ui_state |= MENUSTARTWIRE;
    }
    else { cmd_found = 0;}
    break;
    case 'x': /*----------------------------------------------*/
    #ifdef HAS_XCB
    /* xcb_info
     *   For debug */
    if(!strcmp(argv[1], "xcb_info"))
    {
      dbg(0, "maximum xcb req length=%u\n", xcb_get_maximum_request_length(xcb_conn));
    }
    else { cmd_found = 0;}
    #endif
    break;
    case 'z': /*----------------------------------------------*/
    /* zoom_box [x1 y1 x2 y2] [factor]
     *   Zoom to specified coordinates, if 'factor' is given reduce view (factor < 1.0) 
     *   or add border (factor > 1.0)
     *   If no coordinates are given start GUI zoom box operation */
    if(!strcmp(argv[1], "zoom_box"))
    {
      double x1, y1, x2, y2, factor;
      dbg(1, "scheduler(): xschem zoom_box: argc=%d, argv[2]=%s\n", argc, argv[2]);
      if(argc==6 || argc == 7) {
        x1 = atof(argv[2]);
        y1 = atof(argv[3]);
        x2 = atof(argv[4]);
        y2 = atof(argv[5]);
        if(argc == 7) factor = atof(argv[6]);
        else          factor = 1.;
        if(factor == 0.) factor = 1.;
        zoom_box(x1, y1, x2, y2, factor);
        change_linewidth(-1.);
        draw();
      }
      else {
        xctx->ui_state |=MENUSTARTZOOM;
      }
      Tcl_ResetResult(interp);
    }

    /* zoom_full [center|nodraw|nolinewidth]
     *   Set full view. 
     *   If 'center' is given center vire instead of lower-left align
     *   If 'nodraw' is given don't redraw
     *   If 'nolinewidth]' is given don't reset line widths. */
    else if(!strcmp(argv[1], "zoom_full"))
    {
      int i, flags = 1;
      int draw = 1;
      double shrink = 0.97;
      char * endptr;
      for(i = 2; i < argc; ++i) {
        if(!strcmp(argv[i], "center")) flags  |= 2;
        else if(!strcmp(argv[i], "nodraw")) draw = 0;
        else if(!strcmp(argv[i], "nolinewidth")) flags &= ~1;
        else {
          shrink = strtod(argv[i], &endptr);
          if(endptr == argv[i]) shrink = 1.0;
        }
      }
      if(tclgetboolvar("zoom_full_center")) flags |= 2;
      zoom_full(draw, 0, flags, shrink);
      Tcl_ResetResult(interp);
    }

    /* zoom_hilighted 
     *   Zoom to highlighted objects */
    else if(!strcmp(argv[1], "zoom_hilighted"))
    {
      zoom_full(1, 2, 1, 0.97);
      Tcl_ResetResult(interp);
    }

    /* zoom_in
     *   Zoom in drawing */
    else if(!strcmp(argv[1], "zoom_in"))
    {
      view_zoom(0.0);
      Tcl_ResetResult(interp);
    }

    /* zoom_out
     *   Zoom out drawing */
    else if(!strcmp(argv[1], "zoom_out"))
    {
      view_unzoom(0.0);
      Tcl_ResetResult(interp);
    }

    /* zoom_selected
     *   Zoom to selection */
    else if(!strcmp(argv[1], "zoom_selected"))
    {
      zoom_full(1, 1, 1, 0.97);
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;

    default:
    cmd_found = 0;
    break;
  } /* switch */
  if(!cmd_found) {
    Tcl_AppendResult(interp, "xschem ", argv[1], ": invalid command.", NULL);
    return TCL_ERROR;
  }
  return TCL_OK;
}

double tclgetdoublevar(const char *s)
{
  const char *p;
  p = Tcl_GetVar(interp, s, TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);
  if(!p) {
    dbg(0, "%s\n", tclresult());
    return 0.0;
  }
  return atof_spice(p);
}

int tclgetintvar(const char *s)
{
  const char *p;
  p = Tcl_GetVar(interp, s, TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);
  if(!p) {
    dbg(0, "%s\n", tclresult());
    return 0;
  }
  return atoi(p);
}

int tclgetboolvar(const char *s)
{
  const char *p;
  p = Tcl_GetVar(interp, s, TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);
  if(!p) {
    dbg(0, "%s\n", tclresult());
    return 0;
  }
  return p[0] == '1' ? 1 : 0;
}

const char *tclgetvar(const char *s)
{
  const char *p;
  p = Tcl_GetVar(interp, s, TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);
  if(!p) {
    dbg(1, "%s\n", tclresult());
    return NULL;
  }
  return p;
}

const char *tcleval(const char str[])
{
  if(Tcl_GlobalEval(interp, str) != TCL_OK) {
    fprintf(errfp, "tcleval(): evaluation of script: %s failed\n", str);
    fprintf(errfp, "         : %s\n", Tcl_GetStringResult(interp));
    Tcl_ResetResult(interp);
  }
  return Tcl_GetStringResult(interp);
}
const char *tclresult(void)
{
  return Tcl_GetStringResult(interp);
}

void tclsetvar(const char *s, const char *value)
{
  if(!Tcl_SetVar(interp, s, value, TCL_GLOBAL_ONLY)) {
    fprintf(errfp, "tclsetvar(): error setting variable %s to %s\n", s, value);
  }
}

void tclsetdoublevar(const char *s, const double value)
{
  char str[80];
  sprintf(str, "%.16g", value);
  if(!Tcl_SetVar(interp, s, str, TCL_GLOBAL_ONLY)) {
    fprintf(errfp, "tclsetdoublevar(): error setting variable %s to %g\n", s, value);
  }
}

void tclsetintvar(const char *s, const int value)
{
  char str[80];
  sprintf(str, "%d", value);
  if(!Tcl_SetVar(interp, s, str, TCL_GLOBAL_ONLY)) {
    fprintf(errfp, "tclsetintvar(): error setting variable %s to %d\n", s, value);
  }
}

void tclsetboolvar(const char *s, const int value)
{
  if(!Tcl_SetVar(interp, s, (value ? "1" : "0"), TCL_GLOBAL_ONLY)) {
    fprintf(errfp, "tclsetboolvar(): error setting variable %s to %d\n", s, value);
  }
}

/* Replacement for Tcl_VarEval, which despite being very useful is deprecated */
int tclvareval(const char *script, ...)
{
  char *str = NULL;
  int return_code;
  size_t size;
  const char *p;
  va_list args;

  va_start(args, script);
  size = my_strcat(_ALLOC_ID_, &str, script);
  dbg(1, "tclvareval(): script=%s, str=%s, size=%d\n", script, str, size);
  while( (p = va_arg(args, const char *)) ) {
    size = my_strcat(_ALLOC_ID_, &str, p);
    dbg(1, "tclvareval(): p=%s, str=%s, size=%d\n", p, str, size);
  }
  return_code = Tcl_EvalEx(interp, str, (int)size, TCL_EVAL_GLOBAL);
  va_end(args);
  if(return_code != TCL_OK) {
    dbg(0, "tclvareval(): error executing %s: %s\n", str, tclresult());
    Tcl_ResetResult(interp);
  }
  my_free(_ALLOC_ID_, &str);
  return return_code;
}
