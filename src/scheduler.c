/* File: scheduler.c
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2024 Stefan Frederik Schippers
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
#include <stdarg.h>
/* n=1: messages in status bar
 * n=2: append str in ERC window messages
 * n=3: set ERC messages to str */
void statusmsg(char str[],int n)
{
  if(!str) return;
  if(str[0]== '\0') {
    my_free(_ALLOC_ID_, &xctx->infowindow_text);
    return;
  } else {
    if(n == 3) {
      my_strdup(_ALLOC_ID_, &xctx->infowindow_text, str);
    } else if(n == 2) {
      if(xctx->infowindow_text && xctx->infowindow_text[0]) {
        my_strcat(_ALLOC_ID_, &xctx->infowindow_text, "\n");
      }
      my_strcat(_ALLOC_ID_, &xctx->infowindow_text, str);
    }
  }
  if(!has_x) return;
  if(n == 2 || n == 3) {
    dbg(3, "statusmsg(): n = 2, str = %s\n", str);
  }
  else {
    tclvareval(xctx->top_path, ".statusbar.1 configure -text {", str, "}", NULL);
    dbg(3, "statusmsg(str, %d): -> str = %s\n", n, str);
  }
}

static int get_symbol(const char *s)
{
  int i, found=0;
  if(isonlydigit(s)) {
    i=atoi(s);
    found = 1;
  } else for(i=0;i<xctx->symbols; ++i) {
    if(!strcmp(xctx->sym[i].name, s)) {
      found=1;
      break;
    }
  }
  if(!found || i < 0 || i >= xctx->symbols) return -1;
  return i;
}

int get_instance(const char *s)
{
  int i = -1, found=0;
  Int_hashentry *entry;

  if(isonlydigit(s)) {
     i=atoi(s);
     found = 1;
  }
  else if(xctx->floater_inst_table.table) {
    entry = int_hash_lookup(&xctx->floater_inst_table, s, 0, XLOOKUP);
    if(entry) {
      i = entry->value;
      found = 1;
    }
  } else {
    for(i=0;i<xctx->instances; ++i) {
      if(!strcmp(xctx->inst[i].instname, s)) {
        found=1;
        break;
      }
    }
  }
  if(!found || i < 0 || i >= xctx->instances) return -1;
  return i;
}

static void xschem_cmd_help(int argc, const char **argv)
{
  char prog[PATH_MAX];
  const char *xschem_sharedir=tclgetvar("XSCHEM_SHAREDIR");
  #ifdef __unix__
  int running_in_src_dir = tclgetintvar("running_in_src_dir");
  #endif
  if( get_file_path("x-www-browser")[0] == '/' ) goto done;
  if( get_file_path("firefox")[0] == '/' ) goto done;
  if( get_file_path("chromium")[0] == '/' ) goto done;
  if( get_file_path("chrome")[0] == '/' ) goto done;
  if( get_file_path("xdg-open")[0] == '/' ) goto done;
  #ifndef __unix__
  wchar_t app[MAX_PATH] = {0};
  wchar_t w_url[PATH_MAX];
  char url[PATH_MAX]="", url2[PATH_MAX]="";
  int result = 0;
  if (xschem_sharedir) {
    my_snprintf(url, S(url), "%s/../doc/xschem_man/developer_info.html", xschem_sharedir);
    MultiByteToWideChar(CP_ACP, 0, url, -1, w_url, S(w_url));
    /* The file:// url scheme doesn't have any allowance for HTTP parameters.
     So, use FindExecutable to get the browser app and then ShellExecute */
    result = (int)FindExecutable(w_url, NULL, app);
    if (result > 32) goto done;
  }
  #endif
  if(has_x) tcleval("alert_ { No application to display html documentation} {}");
  else  dbg(0, "No application to display html documentation\n");
  return;
  done:
  my_strncpy(prog, tclresult(), S(prog));
  #ifdef __unix__
  if(running_in_src_dir) {
    tclvareval("launcher {", "file://", xschem_sharedir,
               "/../doc/xschem_man/developer_info.html#cmdref", "} ", prog, NULL);
  } else {
    tclvareval("launcher {", "file://", xschem_sharedir,
               "/../doc/xschem/xschem_man/developer_info.html#cmdref", "} ", prog, NULL);
  }
  #else
  my_snprintf(url2, S(url2), "file://%s#cmdref", url);
  MultiByteToWideChar(CP_ACP, 0, url2, -1, w_url, S(w_url));
  ShellExecute(0, NULL, app, w_url, NULL, SW_SHOWNORMAL);
  #endif
  Tcl_ResetResult(interp);
}

/* can be used to reach C functions from the Tk shell. */
int xschem(ClientData clientdata, Tcl_Interp *interp, int argc, const char * argv[])
{
 int i;
 char name[1024]; /* overflow safe 20161122 */
 int cmd_found = 1;
 char *not_avail = "Not available in this context. If using --tcl consider using --command";

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

    /* add_symbol_pin [x y name dir [draw]]
     *   place a symbol pin.
     *   x,y : pin coordinates
     *   name = pin name
     *   dir = in|out|inout
     *   draw: 1 | 0 (draw or not the added pin immediately, default = 1)
     *   if no parameters given start a GUI placement of a symbol pin */
    else if(!strcmp(argv[1], "add_symbol_pin"))
    {
      int save, draw = 1, linecol = SYMLAYER;
      double x = xctx->mousex_snap;
      double y = xctx->mousey_snap;
      const char *name = NULL;
      const char *dir = NULL;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      xctx->push_undo();
      if(argc > 6) draw = atoi(argv[6]);
      if(argc > 5) {
        char *prop = NULL;
        int flip = 0;
        x = atof(argv[2]);
        y = atof(argv[3]);
        name = argv[4];
        dir = argv[5];
        if(!strcmp(dir, "inout") || !strcmp(dir, "out") ) flip = 1;
        if(!strcmp(dir, "inout")) linecol = 7;
        my_mstrcat(_ALLOC_ID_, &prop, "name=", name, " dir=", dir, NULL);
        storeobject(-1, x - 2.5, y - 2.5, x + 2.5, y + 2.5, xRECT, PINLAYER, 0, prop);
        if(flip) {
          create_text(draw, x - 25, y - 5, 0, 1, name, NULL, 0.2, 0.2);
          storeobject(-1, x - 20, y, x, y, LINE, linecol, 0, NULL);
        } else {
          create_text(draw, x + 25, y - 5, 0, 0, name, NULL, 0.2, 0.2);
          storeobject(-1, x, y, x + 20, y, LINE, linecol, 0, NULL);
        }

        if(draw) {
          save = xctx->draw_window; xctx->draw_window = 1;
          drawrect(PINLAYER, NOW, x - 2.5, y - 2.5, x + 2.5, y + 2.5, 0, -1, -1);
          filledrect(PINLAYER,NOW, x - 2.5, y - 2.5, x + 2.5, y + 2.5, 1, -1, -1);
          if(flip) {
            drawline(linecol, NOW, x -20, y, x, y, 0, NULL);
          } else {
            drawline(linecol, NOW, x, y, x + 20, y, 0, NULL);
          }
          xctx->draw_window = save;
        }
        my_free(_ALLOC_ID_, &prop);
      } else {
        unselect_all(1);
        storeobject(-1, x - 2.5, y - 2.5, x + 2.5, y + 2.5, xRECT, PINLAYER, SELECTED, "name=XXX\ndir=inout");
        xctx->need_reb_sel_arr=1;
        rebuild_selected_array();
        move_objects(START,0,0,0);
        xctx->ui_state |= START_SYMPIN;
      }
      Tcl_ResetResult(interp);
    }

    /* add_graph
     *   Start a GUI placement of a graph object */
    else if(!strcmp(argv[1], "add_graph"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
          "xlabmag=1.0\n"
          "ylabmag=1.0\n"
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

    /* add_image
     *   Ask user to choose a png/jpg file and start a GUI placement of the image */
    else if(!strcmp(argv[1], "add_image"))
    {
      char *f = NULL;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      unselect_all(1);
      tcleval("tk_getOpenFile -filetypes {{{Images} {.jpg .jpeg .png .svg}} {{All files} *} }");

      if(tclresult()[0]) {
        char *str = NULL;
        my_strdup2(_ALLOC_ID_, &f, tclresult());
        my_mstrcat(_ALLOC_ID_, &str, "flags=image,unscaled\nalpha=0.8\nimage=", f, "\n", NULL);

        if(strstr(f, ".svg") == f + strlen(f) - 4 ) {
          if(tcleval("info exists svg_to_png")[0] == '1') {
             my_mstrcat(_ALLOC_ID_, &str, "filter=\"", tclgetvar("svg_to_png"), "\"\n", NULL);
          }
        }
        storeobject(-1, xctx->mousex_snap-100, xctx->mousey_snap-100, xctx->mousex_snap+100, xctx->mousey_snap+100,
                    xRECT, GRIDLAYER, SELECTED, str);

        my_free(_ALLOC_ID_, &str);
        xctx->need_reb_sel_arr=1;
        rebuild_selected_array();
        move_objects(START,0,0,0);
        xctx->ui_state |= START_SYMPIN;
      }
      if(f) my_free(_ALLOC_ID_, &f);
      Tcl_ResetResult(interp);
    }

    /* align
     *   Align currently selected objects to current snap setting */
    else if(!strcmp(argv[1], "align"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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

    /* annotate_op [raw_file] [level]
     *   Annotate operating point data into current schematic.
     *   use <schematic name>.raw or use supplied argument as raw file to open
     *   look for operating point data and annotate voltages/currents into schematic.
     *   The optional 'level' integer specifies the hierarchy level the raw file refers to.
     *   This is necessary if annotate_op is called from a sub schematic at a hierarchy
     *   level > 0 but simulation was done at top level (hierarchy 0, for example)
     */
    else if(!strcmp(argv[1], "annotate_op"))
    {
      int level = -1;
      int res = 0;
      char f[PATH_MAX + 100];
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 3) {
        level = atoi(argv[3]);
        if(level < 0 || level > xctx->currsch) {
          level = -1;
        }
      }
      if(argc > 2) {
        my_snprintf(f, S(f),"regsub {^~/} {%s} {%s/}", argv[2], home_dir);
        tcleval(f);
        my_strncpy(f, tclresult(), S(f));
      } else {
        my_snprintf(f, S(f), "%s/%s.raw",  tclgetvar("netlist_dir"), get_cell(xctx->sch[xctx->currsch], 0));
      }
      tclsetboolvar("live_cursor2_backannotate", 1);
      /* delete previously loaded OP */
      if(xctx->raw && xctx->raw->rawfile && xctx->raw->allpoints == 1 &&
         (!strcmp(xctx->raw->sim_type, "op") || !strcmp(xctx->raw->sim_type, "dc"))) {
        res = extra_rawfile(3, xctx->raw->rawfile, xctx->raw->sim_type, -1.0, -1.0);
      }
      tcleval("array unset ngspice::ngspice_data");
      res = extra_rawfile(1, f, "op", -1.0, -1.0);
      if(res != 1) {
        /* Xyce uses a 1-point DC transfer characteristic for operating point (OP) data */
        res = extra_rawfile(1, f, "dc", -1.0, -1.0);
      }
      if(res == 1) {
        if(level >= 0) {
          xctx->raw->level = level;
          my_strdup2(_ALLOC_ID_, &xctx->raw->schname, xctx->sch[level]);
        }
        update_op();
        draw();
      }
    }

    /* arc
     *   Start a GUI placement of an arc.
     *   User should click 3 unaligned points to define the arc */
    else if(!strcmp(argv[1], "arc"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      xctx->ui_state |= MENUSTART;
      xctx->ui_state2 = MENUSTARTARC;
    }

    /* attach_labels [interactive]
     *   Attach net labels to selected component(s) instance(s)
     *   Optional integer 'interactive' (default: 0) is passed to attach_labels_to_inst().
     *   setting interactive=2 will place lab_show.sym labels on unconnected instance pins */
    else if(!strcmp(argv[1], "attach_labels"))
    {
      int interactive = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      
      if(argc > 2) interactive = atoi(argv[2]);
      attach_labels_to_inst(interactive);
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'b': /*----------------------------------------------*/
    /* bbox begin|end
     *   Start/end bounding box calculation: parameter is either 'begin' or 'end' */
    if(!strcmp(argv[1], "bbox"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        if(!strcmp(argv[2], "end")) {
          bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
        } else if(!strcmp(argv[2], "start")) {
          bbox(START,0.0, 0.0, 0.0, 0.0);
        } else if(!strcmp(argv[2], "set")) {
          bbox(SET,0.0, 0.0, 0.0, 0.0);
        } else if(argc > 6 && !strcmp(argv[2], "add")) {
          bbox(ADD, atof(argv[3]),  atof(argv[4]), atof(argv[5]), atof(argv[6]));
        }
      }
      Tcl_ResetResult(interp);
    }

    /* break_wires [remove]
     *   Break wires at selected instance pins
     *   if '1' is given as 'remove' parameter broken wires that are
     *   all inside selected instances will be deleted */
    else if(!strcmp(argv[1], "break_wires"))
    {
      int remove = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) remove = atoi(argv[2]);
      break_wires_at_pins(remove);
      Tcl_ResetResult(interp);
    }

    /* build_colors
     *   Rebuild color palette using values of tcl vars dim_value and dim_bg */
    else if(!strcmp(argv[1], "build_colors"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      build_colors(tclgetdoublevar("dim_value"), tclgetdoublevar("dim_bg"));
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'c': /*----------------------------------------------*/
    /* callback win_path event mx my key button aux state
     *   Invoke the callback event dispatcher with a software event */
    if(!strcmp(argv[1], "callback") )
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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

    /* change_elem_order n
     *   set selected object (instance, wire, line, rect, ...) to
     *   position 'n' in its respective array */
    else if(!strcmp(argv[1], "change_elem_order"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        int n = atoi(argv[2]);
        if(n >= 0 || n == -1) {
          change_elem_order(n);
        }
      }
    }

    /* change_sch_path n <draw>
     *   if descended into a vector instance change inst number we are into to 'n',
     *   (same rules as 'descend' command) without going up and descending again
     *   if 'draw' string is given redraw screen */
    else if(!strcmp(argv[1], "change_sch_path"))
    {
      int dr = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 3 && !strcmp(argv[3], "draw")) dr = 1;
      if(argc > 2) {
        int n = atoi(argv[2]);
        change_sch_path(n, dr);
      }
    }

    /* check_symbols
     *   List all used symbols in current schematic and warn if some symbol is newer */
    else if(!strcmp(argv[1], "check_symbols"))
    {
      char sympath[PATH_MAX];
      const char *name;
      struct stat buf;
      char *res=NULL;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      for(i=0;i<xctx->symbols; ++i) {
        name = xctx->sym[i].name;
        if(!strcmp(xctx->file_version, "1.0")) {
          my_strncpy(sympath, abs_sym_path(name, ".sym"), S(sympath));
        } else {
          my_strncpy(sympath, abs_sym_path(name, ""), S(sympath));
        }
        if(!stat(sympath, &buf)) { /* file exists */
          if(xctx->time_last_modify < buf.st_mtime) {
            my_mstrcat(_ALLOC_ID_, &res, "Warning: symbol ", sympath, " is newer than schematic\n", NULL);
          }
        } else { /* not found */
            my_mstrcat(_ALLOC_ID_, &res, "Warning: symbol ", sympath, " not found\n", NULL);
        }
        my_mstrcat(_ALLOC_ID_, &res, "symbol ", my_itoa(i), " : ", sympath, "\n", NULL);
      }
      Tcl_SetResult(interp, res, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &res);
    }

    /* check_unique_names [1|0]
     *   Check if all instances have a unique refdes (name attribute in xschem),
     *   highlight such instances. If second parameter is '1' rename duplicates */
    else if(!strcmp(argv[1], "check_unique_names"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2 && !strcmp(argv[2], "1")) {
        check_unique_names(1);
      } else {
        check_unique_names(0);
      }
      Tcl_ResetResult(interp);
    }
    /* closest_object
     *   returns index of closest object to mouse coordinates
     *   index = type layer n
     *   type = wire | text | line | poly | rect | arc | inst
     *   layer is the layer number the object is drawn with
     *   (valid for line, poly, rect, arc)
     *   n is the index of the object in the xschem array
     *   example:
     *      $  after 3000 {set obj [xschem closest_object]}
     *   (after 3s)
     *      $ puts $obj
     *      line 4 19 */
    else if(!strcmp(argv[1], "closest_object"))
    {
      char res[100];
      const char *type=NULL;
      Selected sel;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}

      sel = find_closest_obj(xctx->mousex, xctx->mousey, 0);
      switch(sel.type)
      {
       case WIRE:    type="wire"; break;
       case xTEXT:   type="text"; break;
       case LINE:    type="line"; break;
       case POLYGON: type="poly"; break;
       case xRECT:   type="rect"; break;
       case ARC:     type="arc" ; break;
       case ELEMENT: type="inst"; break;
       default: break;
      } /*end switch */

      if(sel.type) my_snprintf(res, S(res), "%s %d %d", type, sel.col, sel.n);
      else my_snprintf(res, S(res), "nosel");
      Tcl_SetResult(interp, res, TCL_VOLATILE);
    }

    /* circle
     *   Start a GUI placement of a circle.
     *   User should click 3 unaligned points to define the circle */
    else if(!strcmp(argv[1], "circle"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      xctx->ui_state |= MENUSTART;
      xctx->ui_state2 = MENUSTARTCIRCLE;
    }

    /* clear [force] [symbol|schematic]
     *   Clear current schematic window. Resets hierarchy level. Remove symbols
     *   the 'force' parameter will not ask to save existing modified schematic.
     *   the 'schematic' or 'symbol' parameter specifies to default to a schematic
     *   or symbol window (default: schematic) */
    else if(!strcmp(argv[1], "clear"))
    {
      int i, cancel = 1, symbol = 0;

      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      for(i = 2; i < argc; i++) {
        if(!strcmp(argv[i], "force") ) cancel = 0;
        if(!strcmp(argv[i], "symbol")) symbol = 1;
      }
      clear_schematic(cancel, symbol);
      Tcl_ResetResult(interp);
    }

    /* clear_drawing
     *   Clears drawing but does not purge symbols */
    else if(!strcmp(argv[1], "clear_drawing"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      char f[PATH_MAX + 100];
      int ret = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        my_snprintf(f, S(f),"regsub {^~/} {%s} {%s/}", argv[2], home_dir);
        tcleval(f);
        my_strncpy(f, tclresult(), S(f));
        ret = compare_schematics(f);
      }
      else {
        ret = compare_schematics(NULL);
      }
      Tcl_SetResult(interp, my_itoa(ret), TCL_VOLATILE);
    }

    /* connected_nets [0|1|2|3]
     *   Select nets/labels  connected to currently selected instance
     *   if '1' argument is given, stop at wire junctions
     *   if '2' argument is given select only wires directly
     *   attached to selected instance/net
     *   if '3' argument is given combine '1' and '2' */
    else if(!strcmp(argv[1], "connected_nets"))
    {
      int stop_at_junction = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2 ) stop_at_junction = atoi(argv[2]);
      select_connected_nets(stop_at_junction);
      Tcl_ResetResult(interp);
    }

    /* copy
     *   Copy selection to clipboard */
    else if(!strcmp(argv[1], "copy"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      rebuild_selected_array();
      save_selection(2);
      Tcl_ResetResult(interp);
    }

    /* copy_hilights
     *   Copy hilights hash table from previous schematic to new created tab/window */
    else if(!strcmp(argv[1], "copy_hilights"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      copy_hilights();
      Tcl_ResetResult(interp);
    }

    /* copy_hierarchy to from
     *   Copy hierarchy info from tab/window "from" to tab/window "to"
     *   Example: xschem copy_hierarchy .drw .x1.drw */
    else if(!strcmp(argv[1], "copy_hierarchy"))
    {
      int ret = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 3) {
        ret = copy_hierarchy_data(argv[2], argv[3]);
      }
      Tcl_SetResult(interp, my_itoa(ret), TCL_VOLATILE);
    }

    /* copy_objects [dx dy] [kissing] [stretch]
     *   if kissing is given add nets to pins that touch other instances or nets
     *   if stretch is given stretch connected nets to follow instace pins
     *   if dx and dy are given copy selection
     *   to specified offset, otherwise start a GUI copy operation */
    else if(!strcmp(argv[1], "copy_objects"))
    {
      int nparam = 0;
      int kissing= 0;
      int stretch = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        int i;
        for(i = 2; i < argc; i++) {
          if(!strcmp(argv[i], "kissing")) {kissing = 1; nparam++;}
          if(!strcmp(argv[i], "stretch")) {stretch = 1; nparam++;}
        }
      }
      if(stretch) select_attached_nets();
      if(kissing) xctx->connect_by_kissing = 2;
      if(argc > 3 + nparam) {
        copy_objects(START);
        xctx->deltax = atof(argv[2]);
        xctx->deltay = atof(argv[3]);
        copy_objects(END);
      } else {
        xctx->ui_state |= MENUSTART;
        xctx->ui_state2 = MENUSTARTCOPY;
      }
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

    /* create_plot_cmd
     *   Create an xplot file in netlist/simulation directory with
     *   the list of highlighted nodes in a format the selected waveform
     *   viewer understands (bespice, gaw, ngspice) */
    else if(!strcmp(argv[1], "create_plot_cmd") )
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      create_plot_cmd();
      Tcl_ResetResult(interp);
    }

    /* cursor n e
     *   enable or disable cursors.
     *   cursor will be set at 0.0 position. use 'xschem set cursor[12]_x' to set position
     *   n: cursor number (1 or 2, for a or b)
     *   e: enable flag: 1: show, 0: hide */
    else if(!strcmp(argv[1], "cursor"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 3) {
        if(atoi(argv[2]) == 2) { /* cursor 2 */
          if(atoi(argv[3]) == 1)
            xctx->graph_flags |= 4;
          else
            xctx->graph_flags &= ~4;

          if(xctx->graph_flags & 4) {
            xctx->graph_cursor2_x = 0.0;
          }
        } else { /* cursor 1 */
          if(atoi(argv[3]) == 1)
            xctx->graph_flags |= 2;
          else
            xctx->graph_flags &= ~2;
          if(xctx->graph_flags & 2) {
            xctx->graph_cursor1_x = 0.0;
          }
        }
      }
      Tcl_ResetResult(interp);
    }

    /* cut
     *   Cut selection to clipboard */
    else if(!strcmp(argv[1], "cut"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc==2) delete(1/*to_push_undo*/);
      Tcl_ResetResult(interp);
    }

    /* delete_files
     *   Bring up a file selector the user can use to delete files */
    else if(!strcmp(argv[1], "delete_files"))
    {
      delete_files();
    }

    /* descend [n] [notitle]
     *   Descend into selected component instance. Optional number 'n' specifies the
     *   instance number to descend into for vector instances (default: 0).
     *   0 or 1: leftmost instance, 2: second leftmost instance, ...
     *  -1: rightmost instance,-2: second rightmost instance, ...
     *  if integer 'notitle' is given pass it to descend_schematic() */
    else if(!strcmp(argv[1], "descend"))
    {
      int ret=0;
      int set_title = 1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(xctx->semaphore == 0) {

        if(argc > 3 ) {
          set_title = atoi(argv[3]);
        }
        if(argc > 2) {
          int n = atoi(argv[2]);
          ret = descend_schematic(n, 0, 0, set_title);
        } else {
          ret = descend_schematic(0, 0, 0, set_title);
        }
      }
      Tcl_SetResult(interp, dtoa(ret), TCL_VOLATILE);
    }

    /* descend_symbol
     *   Descend into the symbol view of selected component instance */
    else if(!strcmp(argv[1], "descend_symbol"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(xctx->semaphore == 0) descend_symbol();
      Tcl_ResetResult(interp);
    }

    /* destroy_all [force]
     *   Close all additional windows/tabs. If 'force' is given do not ask for
     *   confirmation for changed schematics
     *   Returns the remaining # of windows/tabs in addition to main window/tab */
    else if(!strcmp(argv[1], "destroy_all"))
    {
      int force = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2 && !strcmp(argv[2], "force")) force = 1;
      if(force)
        new_schematic("destroy_all", "force", NULL, 1);
      else
        new_schematic("destroy_all", NULL, NULL, 1);
      Tcl_SetResult(interp, my_itoa(get_window_count()), TCL_VOLATILE);
    }

    /* display_hilights [nets|instances]
     *   Print a list of highlighted objects (nets, net labels/pins, instances)
     *   if 'instances' is specified list only instance highlights
     *   if 'nets' is specified list only net highlights */
    else if(!strcmp(argv[1], "display_hilights"))
    {
      char *str = NULL;
      int what = 3; /* nets and instances */
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        if(!strcmp(argv[2], "instances")) what = 2; /* instances only */
        else if(!strcmp(argv[2], "nets")) what = 1; /* nets only */
      }
      display_hilights(what, &str);
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        int i = atoi(argv[2]);
        setup_graph_data(i, 0,  &xctx->graph_struct);
        if(argc > 3) {
          flags = atoi(argv[3]);
        } else {
          /* 2: draw cursor 1
           * 4: draw cursor 2
           * 128: draw hcursor 1
           * 256: draw hcursor 2 */
          flags = 1 + 8 + (xctx->graph_flags & (2 + 4 + 128 + 256));
        }
        draw_graph(i, flags, &xctx->graph_struct, NULL);
      }
      Tcl_ResetResult(interp);
    }

    /* draw_hilight_net [1|0]
     *   Redraw only hilight colors on nets and instances
     *   the parameter specifies if drawing on window or only on back buffer */
    else if(!strcmp(argv[1], "draw_hilight_net")) {
      int on_window = 1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        on_window = atoi(argv[2]);
      }
      draw_hilight_net(on_window);
      Tcl_ResetResult(interp);
    }

    /* drc_check [i]
     *   Perform DRC rulecheck of instances.
     *   if i is specified do check of specified instance
     *   otherwise check all instances in current schematic. */
    else if(!strcmp(argv[1], "drc_check"))
    {
      int i = -1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2 && (i = get_instance(argv[2])) < 0 ) {
        Tcl_SetResult(interp, "xschem drc_check: instance not found", TCL_STATIC);
        return TCL_ERROR;
      }
      drc_check(i);
    }
    else { cmd_found = 0;}
    break;
    case 'e': /*----------------------------------------------*/
    /* edit_file
     *   Edit xschem file of current schematic if nothing is selected.
     *   Edit .sym file if a component is selected. */
    if(!strcmp(argv[1], "edit_file") )
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      rebuild_selected_array();
      if(xctx->lastsel==0 ) {
        save_schematic(xctx->sch[xctx->currsch], 0); /* sync data with disk file before editing file */
        my_snprintf(name, S(name), "edit_file {%s}",
            abs_sym_path(xctx->sch[xctx->currsch], ""));
        tcleval(name);
      }
      else if(xctx->sel_array[0].type==ELEMENT) {
        my_snprintf(name, S(name), "edit_file {%s}",
            abs_sym_path(tcl_hook2(xctx->inst[xctx->sel_array[0].n].name), ""));
        tcleval(name);
      }
    }
    /* edit_prop
     *   Edit global schematic/symbol attributes or attributes
     *   of currently selected instances */
    else if(!strcmp(argv[1], "edit_prop"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      edit_property(0);
      Tcl_ResetResult(interp);
    }

    /* edit_vi_prop
     *   Edit global schematic/symbol attributes or
     *   attributes of currently selected instances
     *   using a text editor (defined in tcl 'editor' variable) */
    else if(!strcmp(argv[1], "edit_vi_prop"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      edit_property(1);
      Tcl_ResetResult(interp);
    }

    /* embed_rawfile raw_file
     *   Embed base 64 encoded 'raw_file' into currently
     *   selected element as a 'spice_data'
     *   attribute. */
    else if(!strcmp(argv[1], "embed_rawfile"))
    {
      char f[PATH_MAX + 100];
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        my_snprintf(f, S(f),"regsub {^~/} {%s} {%s/}", argv[2], home_dir);
        tcleval(f);
        my_strncpy(f, tclresult(), S(f));
        embed_rawfile(f);
      }
      Tcl_ResetResult(interp);
    }

    /* enable_layers
     *   Enable/disable layers depending on tcl array variable enable_layer() */
    else if(!strcmp(argv[1], "enable_layers"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      enable_layers();
      Tcl_ResetResult(interp);
    }

    /* escape_chars source [charset]
     *   escape tcl special characters with backslash
     *   if charset is given escape characters in charset */
    else if(!strcmp(argv[1], "escape_chars"))
    {
      if(argc > 3) {
        Tcl_SetResult(interp, escape_chars(argv[2], argv[3]), TCL_VOLATILE);
      } else if(argc > 2) {
        Tcl_SetResult(interp, escape_chars(argv[2], ""), TCL_VOLATILE);
      }
    }


    /* eval_expr str
     *   debug function: evaluate arithmetic expression in str */
    else if(!strcmp(argv[1], "eval_expr"))
    {
      if(argc > 2) {
        Tcl_SetResult(interp, eval_expr(argv[2]), TCL_VOLATILE);
      }
    }

    /* exit [exit_code] [closewindow] [force]
     *   Exit the program, ask for confirm if current file modified.
     *   if exit_code is given exit with its value, otherwise use 0 exit code
     *   if 'closewindow' is given close the window, otherwise leave with a blank schematic
     *   when closing the last remaining window
     *   if 'force' is given do not ask before closing modified schematic windows/tabs
     *   This command returns the list of remaining open windows in addition to main window */
    else if(!strcmp(argv[1], "exit"))
    {
      int closewindow = 0;
      int force = 0;
      const char *exit_status = "0";


      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      for(i = 2; i < argc; ++i) {
        if(!strcmp(argv[i], "closewindow")) closewindow = 1;
        if(!strcmp(argv[i], "force")) force = 1;
        if(strpbrk(argv[i], "0123456789-")) exit_status = argv[i];
      }
      if(!strcmp(xctx->current_win_path, ".drw")) {
        /* non tabbed interface */
        if(!tclgetboolvar("tabbed_interface")) {
          int wc = get_window_count();
          dbg(1, "wc=%d\n", wc);
          if(wc > 0 ) {
            if(!force && xctx->modified) {
              tcleval("tk_messageBox -type okcancel  -parent [xschem get topwindow] -message \""
                        "[get_cell [xschem get schname] 0]"
                        ": UNSAVED data: want to exit?\"");
            }
            if(force || !xctx->modified || !strcmp(tclresult(), "ok")) {
              swap_windows(0);
              set_modify(0); /* set modified status to 0 to avoid another confirm in following line */
              new_schematic("destroy", xctx->current_win_path, NULL, 0);
              draw();
            }
          } else {
            if(!force && xctx->modified) {
              tcleval("tk_messageBox -type okcancel  -parent [xschem get topwindow] -message \""
                        "[get_cell [xschem get schname] 0]"
                        ": UNSAVED data: want to exit?\"");
            }
            if(force || !xctx->modified || !strcmp(tclresult(), "ok")) {
               if(closewindow) {
                 char s[40];
                 my_snprintf(s, S(s), "exit %s", exit_status);
                 tcleval(s);
               }
               else clear_schematic(0, 0);
            }
          }
        }
        /* tabbed interface */
        else {
          int wc = get_window_count();
          dbg(1, "wc=%d\n", wc);
          if(wc > 0 ) {
            if(has_x && !force && xctx->modified) {
              tcleval("tk_messageBox -type okcancel  -parent [xschem get topwindow] -message \""
                        "[get_cell [xschem get schname] 0]"
                        ": UNSAVED data: want to exit?\"");
            }
            if(!has_x || force || !xctx->modified || !strcmp(tclresult(), "ok")) {
              swap_tabs();
              set_modify(0);
              new_schematic("destroy", xctx->current_win_path, NULL, 1);
            }
          } else {
            if(has_x && !force && xctx->modified) {
              tcleval("tk_messageBox -type okcancel  -parent [xschem get topwindow] -message \""
                        "[get_cell [xschem get schname] 0]"
                        ": UNSAVED data: want to exit?\"");
            }
            if(!has_x || force || !xctx->modified || !strcmp(tclresult(), "ok")) {
               if(closewindow) {
                 char s[40];
                 my_snprintf(s, S(s), "exit %s", exit_status);
                 tcleval(s);
               }
               else clear_schematic(0, 0);
            }
          }
        }
      } else {
        if(force) set_modify(0); /* avoid ask to save downstream */
        new_schematic("destroy", xctx->current_win_path, NULL, 1);
      }
      Tcl_SetResult(interp, my_itoa(get_window_count()), TCL_VOLATILE);
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
    /* fill_reset [nodraw]
     *   After setting tcl array pixdata(n) reset fill patterns on all layers
     *   If 'nodraw' is given do not redraw window.
     */
    if(!strcmp(argv[1], "fill_reset"))
    {
      int dr = 1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      init_pixdata();
      free_gc();
      create_gc();
      enable_layers();
      build_colors(0.0, 0.0);
      resetwin(1, 0, 1, 0, 0);  /* recreate pixmap. resetwin(create_pixmap, clear_pixmap, force, w, h) */
      if(argc > 2 && !strcmp(argv[2], "nodraw")) dr = 0;
      if(dr) draw();
    }
    /* fill_type n fill_type [nodraw]
     *   Set fill type for layer 'n', fill_type may be 'solid' or 'stipple' or 'empty'
     *   If 'nodraw' is given do not redraw window.
     */
    else if(!strcmp(argv[1], "fill_type"))
    {
      int dr = 1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 3) {
        int n = atoi(argv[2]);
        if(n >=0 && n < cadlayers) {
          if(!strcmp(argv[3], "solid")) xctx->fill_type[n]=1;
          else if(!strcmp(argv[3], "stipple")) xctx->fill_type[n]=2;
          else if(!strcmp(argv[3], "empty")) xctx->fill_type[n]=0;
          free_gc();
          create_gc();
          enable_layers();
          build_colors(0.0, 0.0);
          resetwin(1, 0, 1, 0, 0);  /* recreate pixmap. resetwin(create_pixmap, clear_pixmap, force, w, h) */
          if(argc > 4 && !strcmp(argv[4], "nodraw")) dr = 0;
          if(dr) draw();
        }
      }
    }

    /* find_nth string sep quote keep_quote n
     *   Find n-th field string separated by characters in sep. 1st field is in position 1
     *   do not split quoted fields (if quote characters are given) and return unquoted.
     *   xschem find_nth {aaa,bbb,ccc,ddd} {,} 2  --> bbb
     *   xschem find_nth {aaa, "bbb, ccc" , ddd} { ,} {"} 2  --> bbb, ccc
     */
    else if(!strcmp(argv[1], "find_nth"))
    {
      if(argc > 6) {
        Tcl_SetResult(interp, find_nth(argv[2], argv[3], argv[4], atoi(argv[5]), atoi(argv[6])), TCL_VOLATILE);
      }
    }

    /* flip [x0 y0]
     *   Flip selection horizontally around point x0 y0.
     *   if x0, y0 not given use mouse coordinates */
    else if(!strcmp(argv[1], "flip"))
    {
      double x0 = xctx->mousex_snap;
      double y0 = xctx->mousey_snap;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 3) {
        x0 = atof(argv[2]);
        y0 = atof(argv[3]);
      }
      if(xctx->ui_state & STARTMOVE) move_objects(FLIP,0,0,0);
      else if(xctx->ui_state & STARTCOPY) copy_objects(FLIP);
      else {
        rebuild_selected_array();
        xctx->mx_double_save = xctx->mousex_snap = x0;
        xctx->my_double_save = xctx->mousey_snap = y0;
        move_objects(START,0,0,0);
        move_objects(FLIP,0, 0, 0);
        move_objects(END,0,0,0);
      }
      Tcl_ResetResult(interp);
    }

    /* flip_in_place
     *   Flip selection horizontally, each object around its center */
    else if(!strcmp(argv[1], "flip_in_place"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(xctx->ui_state & STARTMOVE) move_objects(FLIP|ROTATELOCAL,0,0,0);
      else if(xctx->ui_state & STARTCOPY) copy_objects(FLIP|ROTATELOCAL);
      else {
        rebuild_selected_array();
        move_objects(START,0,0,0);
        move_objects(FLIP|ROTATELOCAL,0,0,0);
        move_objects(END,0,0,0);
      }
      Tcl_ResetResult(interp);
    }

    /* flipv [x0 y0]
     *   Flip selection vertically around point x0 y0.
     *   if x0, y0 not given use mouse coordinates */
    else if(!strcmp(argv[1], "flipv"))
    {
      double x0 = xctx->mousex_snap;
      double y0 = xctx->mousey_snap;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 3) {
        x0 = atof(argv[2]);
        y0 = atof(argv[3]);
      }
      if(xctx->ui_state & STARTMOVE) {
        move_objects(ROTATE,0,0,0);
        move_objects(ROTATE,0,0,0);
        move_objects(FLIP,0,0,0);
      }
      else if(xctx->ui_state & STARTCOPY) {
        copy_objects(ROTATE);
        copy_objects(ROTATE);
        copy_objects(FLIP);
      }
      else {
        rebuild_selected_array();
        xctx->mx_double_save = xctx->mousex_snap = x0;
        xctx->my_double_save = xctx->mousey_snap = y0;
        move_objects(START,0,0,0);
        move_objects(ROTATE,0, 0, 0);
        move_objects(ROTATE,0, 0, 0);
        move_objects(FLIP,0, 0, 0);
        move_objects(END,0,0,0);
      }
      Tcl_ResetResult(interp);
    }

    /* flipv_in_place
     *   Flip selection vertically, each object around its center */
    else if(!strcmp(argv[1], "flipv_in_place"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(xctx->ui_state & STARTMOVE) {
        move_objects(ROTATE|ROTATELOCAL,0,0,0);
        move_objects(ROTATE|ROTATELOCAL,0,0,0);
        move_objects(FLIP|ROTATELOCAL,0,0,0);
      }
      else if(xctx->ui_state & STARTCOPY) {
        copy_objects(ROTATE|ROTATELOCAL);
        copy_objects(ROTATE|ROTATELOCAL);
        copy_objects(FLIP|ROTATELOCAL);
      }
      else {
        rebuild_selected_array();
        move_objects(START,0,0,0);
        move_objects(ROTATE|ROTATELOCAL,0,0,0);
        move_objects(ROTATE|ROTATELOCAL,0,0,0);
        move_objects(FLIP|ROTATELOCAL,0,0,0);
        move_objects(END,0,0,0);
      }
      Tcl_ResetResult(interp);
    }

    /* floaters_from_selected_inst
     *   flatten to current level selected instance texts */
    else if(!strcmp(argv[1], "floaters_from_selected_inst"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      floaters_from_selected_inst();
      Tcl_ResetResult(interp);
    }

    /* fullscreen
     *   Toggle fullscreen modes: fullscreen with menu & status, fullscreen, normal */
    else if(!strcmp(argv[1], "fullscreen"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
          else if(!strcmp(argv[2], "bbox")) { /* bounding box schematic */
            xRect boundbox;
            char res[2048];
            calc_drawing_bbox(&boundbox, 0);
            my_snprintf(res, S(res), "%g %g %g %g", boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
            Tcl_SetResult(interp, res, TCL_VOLATILE);
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
          else if(!strcmp(argv[2], "build_date")) { /* time and date this file was built. */
            char date[] =  __DATE__ " : "  __TIME__;
            Tcl_SetResult(interp, date,  TCL_STATIC);
          }
          break;
          case 'c':
          if(!strcmp(argv[2], "cadlayers")) { /* number of layers */
            Tcl_SetResult(interp, my_itoa(cadlayers), TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "case_insensitive")) { /* case_insensitive symbol matching */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, my_itoa(xctx->case_insensitive), TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "color_ps")) { /* color postscript flag */
            if(color_ps != 0 ) Tcl_SetResult(interp, "1",TCL_STATIC);
            else Tcl_SetResult(interp, "0",TCL_STATIC);
          }
          else if(!strcmp(argv[2], "constr_mv")) { /* color postscript flag */
            if(xctx->constr_mv != 0 ) Tcl_SetResult(interp, "1",TCL_STATIC);
            else Tcl_SetResult(interp, "0",TCL_STATIC);
          }
          else if(!strcmp(argv[2], "current_dirname")) { /* directory name of current design */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, xctx->current_dirname, TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "current_name")) { /* name of current design (no library path) */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, xctx->current_name, TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "current_win_path")) { /* path of current tab/window (.drw, .x1.drw, ...) */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, xctx->current_win_path, TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "currsch")) { /* hierarchy level of current schematic (start at 0) */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, my_itoa(xctx->currsch),TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "cursor1_x")) { /* cursor 1 position */
            char c[70];
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_snprintf(c, S(c), "%g", xctx->graph_cursor1_x);
            Tcl_SetResult(interp, c, TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "cursor2_x")) { /* cursor 2 position */
            char c[70];
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_snprintf(c, S(c), "%g", xctx->graph_cursor2_x);
            Tcl_SetResult(interp, c, TCL_VOLATILE);
          }
          break;
          case 'd':
          if(!strcmp(argv[2], "debug_var")) { /* debug level (0 = no debug, 1, 2, 3,...) */
            Tcl_SetResult(interp, my_itoa(debug_var),TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "draw_window")) { /* direct draw into window */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, my_itoa(xctx->draw_window),TCL_VOLATILE);
          }
          break;
          case 'f':
          if(!strcmp(argv[2], "first_sel")) { /* get data about first selected object */
            char res[40];
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_snprintf(res, S(res), "%hu %d %u", xctx->first_sel.type, xctx->first_sel.n, xctx->first_sel.col);
            Tcl_SetResult(interp, res, TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "fix_broken_tiled_fill")) { /* get drawing method setting (for broken GPUs) */
            Tcl_SetResult(interp, my_itoa(fix_broken_tiled_fill),TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "fix_mouse_coord")) { /* get fix_mouse_coord setting (fix for broken RDP)*/
            Tcl_SetResult(interp, my_itoa(fix_mouse_coord),TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "format")) { /* alternate format attribute to use in netlist (or NULL) */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            if(!xctx->custom_format ) Tcl_SetResult(interp, "<NULL>",TCL_STATIC);
            else Tcl_SetResult(interp, xctx->custom_format,TCL_VOLATILE);
          }
          break;
          case 'g':
          if(!strcmp(argv[2], "graph_lastsel")) { /* number of last graph that was clicked */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            if(xctx && xctx->header_text) {
              Tcl_SetResult(interp, xctx->header_text, TCL_VOLATILE);
            } else {
              Tcl_SetResult(interp, "", TCL_VOLATILE);
            }
          }
          break;
          case 'i':
          if(!strcmp(argv[2], "infowindow_text")) { /* ERC messages */
            if(xctx && xctx->infowindow_text)
              Tcl_SetResult(interp, xctx->infowindow_text, TCL_VOLATILE);
            else
              Tcl_SetResult(interp, "", TCL_STATIC);
          }

          else if(!strcmp(argv[2], "instances")) { /* number of instances in schematic */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, my_itoa(xctx->instances), TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "intuitive_interface")) { /* ERC messages */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, my_itoa(xctx->intuitive_interface), TCL_VOLATILE);
          }
          break;
          case 'l':
          if(!strcmp(argv[2], "last_created_window")) { /* return win_path of last created tab or window */
            Tcl_SetResult(interp, get_last_created_window_path(), TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "lastsel")) { /* number of selected objects */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            rebuild_selected_array();
            Tcl_SetResult(interp, my_itoa(xctx->lastsel),TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "line_width")) { /* get line width */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, dtoa(xctx->lw), TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "lines")) { /* (xschem get lines n) number of lines on layer 'n' */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            if(argc > 3) {
              int c = atoi(argv[3]);
              if(c >=0 && c < cadlayers) {
                Tcl_SetResult(interp, my_itoa(xctx->lines[c]),TCL_VOLATILE);
              } else {
                Tcl_SetResult(interp, "xschem get rects n: layer number out of range", TCL_STATIC);
                return TCL_ERROR;
              }
            } else {
              Tcl_SetResult(interp, "xschem get rects n: give a layer number", TCL_STATIC);
              return TCL_ERROR;
            }
          }
          break;
          case 'm':
          if(!strcmp(argv[2], "modified")) { /* schematic is in modified state (needs a save) */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, my_itoa(xctx->modified),TCL_VOLATILE);
          }
          break;
          case 'n':
          if(!strcmp(argv[2], "netlist_name")) { /* netlist name if set. If 'fallback' given get default name */
            if(argc > 3 &&  !strcmp(argv[3], "fallback")) {
              char f[PATH_MAX];
              if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
              if(xctx->netlist_type == CAD_SPICE_NETLIST) {
                my_snprintf(f, S(f), "%s.spice", get_cell(xctx->current_name, 0));
              }
              else if(xctx->netlist_type == CAD_VHDL_NETLIST) {
                my_snprintf(f, S(f), "%s.vhdl", get_cell(xctx->current_name, 0));
              }
              else if(xctx->netlist_type == CAD_VERILOG_NETLIST) {
                my_snprintf(f, S(f), "%s.v", get_cell(xctx->current_name, 0));
              }
              else if(xctx->netlist_type == CAD_SPECTRE_NETLIST) {
                my_snprintf(f, S(f), "%s.spectre", get_cell(xctx->current_name, 0));
              }
              else if(xctx->netlist_type == CAD_TEDAX_NETLIST) {
                my_snprintf(f, S(f), "%s.tdx", get_cell(xctx->current_name, 0));
              }
              else {
                my_snprintf(f, S(f), "%s.unknown", get_cell(xctx->current_name, 0));
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
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            if(xctx->netlist_type == CAD_SPICE_NETLIST) {
              Tcl_SetResult(interp, "spice", TCL_STATIC);
            }
            else if(xctx->netlist_type == CAD_VHDL_NETLIST) {
              Tcl_SetResult(interp, "vhdl", TCL_STATIC);
            }
            else if(xctx->netlist_type == CAD_SPECTRE_NETLIST) {
              Tcl_SetResult(interp, "spectre", TCL_STATIC);
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
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            if(xctx->no_draw != 0 )
              Tcl_SetResult(interp, "1",TCL_STATIC);
            else
              Tcl_SetResult(interp, "0",TCL_STATIC);
          }
          else if(!strcmp(argv[2], "ntabs")) { /* get number of additional tabs (0 = only one tab) */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, my_itoa(get_window_count()), TCL_VOLATILE);
          }
          break;
          case 'p':
          if(!strcmp(argv[2], "pinlayer")) { /* layer number for pins */
            Tcl_SetResult(interp, my_itoa(PINLAYER),TCL_VOLATILE);
          }
          break;
          case 'r':
          if(!strcmp(argv[2], "raw_level")) { /* hierarchy level where raw file was loaded */
            int ret = -1;
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            if(xctx->raw) ret = xctx->raw->level;
            Tcl_SetResult(interp, my_itoa(ret),TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "rectcolor")) { /* current layer number */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, my_itoa(xctx->rectcolor),TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "rects")) { /* (xschem get rects n) number of rectangles on layer 'n' */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            if(argc > 3) {
              int c = atoi(argv[3]);
              if(c >=0 && c < cadlayers) {
                Tcl_SetResult(interp, my_itoa(xctx->rects[c]),TCL_VOLATILE);
              } else {
                Tcl_SetResult(interp, "xschem get rects n: layer number out of range", TCL_STATIC);
                return TCL_ERROR;
              }
            } else {
              Tcl_SetResult(interp, "xschem get rects n: give a layer number", TCL_STATIC);
              return TCL_ERROR;
            }
          }
          break;
          case 's':
          if(!strcmp(argv[2], "sellayer")) { /* layer number for selection */
            Tcl_SetResult(interp, my_itoa(SELLAYER),TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "semaphore")) { /* used for debug */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, my_itoa(xctx->semaphore),TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "schname")) /* get full path of current sch. if 'n' given get sch of level 'n' */
          {
            int x;
            /* allows to retrieve name of n-th parent schematic */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            if(argc > 3) x = atoi(argv[3]);
            else x = xctx->currsch;
            if(x < 0 ) x = xctx->currsch + x;
            if(x<=xctx->currsch && x >= 0) {
              Tcl_SetResult(interp, xctx->sch[x], TCL_VOLATILE); /* if xctx->sch[x]==NULL return empty string */
            }
          }
          else if(!strcmp(argv[2], "schprop")) /* get schematic "spice" global attributes */
          {
             Tcl_SetResult(interp, xctx->schprop ? xctx->schprop : "", TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "schvhdlprop")) /* get schematic "vhdl" global attributes */
          {
             Tcl_SetResult(interp, xctx->schvhdlprop ? xctx->schvhdlprop : "", TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "schverilogprop")) /* get schematic "verilog" global attributes */
          {
             Tcl_SetResult(interp, xctx->schverilogprop ? xctx->schverilogprop : "", TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "schspectreprop")) /* get schematic "spectre" global attributes */
          { 
             Tcl_SetResult(interp, xctx->schspectreprop ? xctx->schspectreprop : "", TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "schsymbolprop")) /* get schematic "symbol" global attributes */
          {
             Tcl_SetResult(interp, xctx->schsymbolprop ? xctx->schsymbolprop : "", TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "schtedaxprop")) /* get schematic "tedax" global attributes */
          {
             Tcl_SetResult(interp, xctx->schtedaxprop ? xctx->schtedaxprop : "", TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "sch_path")) /* get hierarchy path. if 'n' given get hierpath of level 'n' */
          {
            int x;
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            if(argc > 3) x = atoi(argv[3]);
            else x = xctx->currsch;
            if(x < 0 ) x = xctx->currsch + x;
            if(x<=xctx->currsch && x >= 0) {
              Tcl_SetResult(interp, xctx->sch_path[x], TCL_VOLATILE);
            }
          }
          else if(!strcmp(argv[2], "sch_to_compare")) /* if set return schematic current design is compared with */
          {
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, xctx->sch_to_compare, TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "sim_sch_path")) /* get sim hier path. start from level where raw was loaded */
          {
            int x = xctx->currsch;
            char *path = xctx->sch_path[x] + 1;
            int skip = 0;
            int start_level;
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            start_level = sch_waves_loaded();
            /* skip path components that are above the level where raw file was loaded */
            while(*path && skip < start_level) {
              if(*path == '.') skip++;
              ++path;
            }
            Tcl_SetResult(interp, path, TCL_VOLATILE);
          }
          else if(!strcmp(argv[2], "symbols")) { /* number of loaded symbols */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, my_itoa(xctx->symbols), TCL_VOLATILE);
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
                tcleval("exit 1");
              }
              else {
                char s[MAX_PATH];
                size_t num_char_converted;
                int err = wcstombs_s(&num_char_converted, s, MAX_PATH, tmp_buffer_path, MAX_PATH); /*unicode TBD*/
                if(err != 0) {
                  Tcl_SetResult(interp, "xschem get temp_dir conversion failed\n", TCL_STATIC);
                  fprintf(errfp, "xschem get temp_dir: conversion error\n");
                  tcleval("exit 1");
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
          else if(!strcmp(argv[2], "top_path")) { /* get top hier path of current window (always "" for tabbed if) */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, xctx->top_path, TCL_VOLATILE);
          }
          /* same as above but main window returned as "." */
          else if(!strcmp(argv[2], "topwindow")) { /* same as top_path but main window returned as "." */
            char *top_path;
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            top_path =  xctx->top_path[0] ? xctx->top_path : ".";
            Tcl_SetResult(interp, top_path,TCL_VOLATILE);
          }
          break;
          case 'u':
          if(!strcmp(argv[2], "ui_state")) { /* return UI state */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp,  my_itoa(xctx->ui_state), TCL_VOLATILE);
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
          } else if(!strcmp(argv[2], "wires")) { /* number of wires */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            Tcl_SetResult(interp, my_itoa(xctx->wires), TCL_VOLATILE);
          }
          break;
          case 'x':
          if(!strcmp(argv[2], "xschem_web_dirname")) {
            Tcl_SetResult(interp, xschem_web_dirname, TCL_STATIC);
          } else if(!strcmp(argv[2], "xorigin")) { /* x coordinate of origin */
            char s[128];
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_snprintf(s, S(s), "%.16g", xctx->xorigin);
            Tcl_SetResult(interp, s,TCL_VOLATILE);
          }
          break;
          case 'y':
          if(!strcmp(argv[2], "yorigin")) { /* y coordinate of origin */
            char s[128];
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_snprintf(s, S(s), "%.16g", xctx->yorigin);
            Tcl_SetResult(interp, s,TCL_VOLATILE);
          }
          break;
          case 'z':
          if(!strcmp(argv[2], "zoom")) { /* zoom level */
            char s[128];
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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

    /* get_additional_symbols what
     *   create new symbols for instance based implementation selection */
    else if(!strcmp(argv[1], "get_additional_symbols") )
    {
      if(argc > 2) {
        get_additional_symbols(atoi(argv[2]));
      }
      Tcl_ResetResult(interp);
    }

    /* get_cell cell n_dirs
     *   return result of get_cell function */
    else if(!strcmp(argv[1], "get_cell") )
    {
      if(argc > 3) {
        Tcl_SetResult(interp, (char *)get_cell(argv[2], atoi(argv[3])), TCL_VOLATILE);
      }
    }

    /* get_cell_w_ext cell n_dirs
     *   return result of get_cell_w_ext function */
    else if(!strcmp(argv[1], "get_cell_w_ext") )
    {
      if(argc > 3) {
        Tcl_SetResult(interp, (char *)get_cell_w_ext(argv[2], atoi(argv[3])), TCL_VOLATILE);
      }
    }
    /************ end xschem get subcommands *************/


    /* get_fqdevice instname param modelparam
     *   get the full pathname of "instname" device
     *   modelparam: 
     *     0: current, 1: modelparam, 2: modelvoltage
     *   param: device parameter, like ib, gm, vth
     *   set param to {} (empty str) for just branch current of 2 terminal device
     *   for parameters like "vth" modelparam must be 2
     *   for parameters like "ib" modelparam must be 0
     *   for parameters like "gm" modelparam must be 1
     */
    else if(!strcmp(argv[1], "get_fqdevice"))
    {
      char *fqdev;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 4) {
        fqdev = get_fqdevice(argv[3], atoi(argv[4]), argv[2]);
        Tcl_SetResult(interp, fqdev, TCL_VOLATILE);
        my_free(_ALLOC_ID_, &fqdev);
      } else if(argc > 2) {
        fqdev = get_fqdevice("", 0, argv[2]);
        Tcl_SetResult(interp, fqdev, TCL_VOLATILE);
        my_free(_ALLOC_ID_, &fqdev);
      }
    }

    /* getprop instance|instance_pin|symbol|text ref
     *
     * getprop instance inst
     *   Get the full attribute string of 'inst'
     *
     * getprop instance inst attr
     *   Get the value of attribute 'attr'
     *   If 'attr has the form 'cell::sym_attr' look up attribute 'sym_attr'
     *   of the symbol referenced by the instance.
     *
     * getprop instance_notcl inst attr
     *   Same as above but do not perform tcl substitution
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
     *
     * getprop rect layer num attr [with_quotes]
     *   if '1' is given as 'keep' return backslashes and unescaped quotes if present in value
     *   Get attribute 'attr' of rectangle number 'num' on layer 'layer'
     *
     * getprop text num attr
     *   Get attribute 'attr' of text number 'num'
     *   if attribute is 'txt_ptr' return the text
     *
     * getprop wire num attr
     *   Get attribute 'attr' of wire number 'num'
     *
     * ('inst' can be an instance name or instance number)
     * ('pin' can be a pin name or pin number)
     */
    else if(!strcmp(argv[1], "getprop"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc < 3) {
        Tcl_SetResult(interp, "xschem getprop needs instance|instance_pin|wire|symbol|text|rect", TCL_STATIC);
        return TCL_ERROR;
      }
      if(argc > 2 && (!strcmp(argv[2], "instance") || !strcmp(argv[2], "instance_notcl"))) {
        int i;
        int with_quotes = 0;
        const char *tmp;
        if(argc < 4) {
          Tcl_SetResult(interp, "'xschem getprop instance' needs 1 or 2 additional arguments", TCL_STATIC);
          return TCL_ERROR;
        }
        if((i = get_instance(argv[3])) < 0 ) {
          Tcl_AppendResult(interp, "xschem getprop: instance not found:", argv[3], NULL);
          return TCL_ERROR;
        }
        if(!strcmp(argv[2], "instance_notcl")) with_quotes = 2;
        if(argc < 5) {
          Tcl_SetResult(interp, xctx->inst[i].prop_ptr, TCL_VOLATILE);
        } else if(!strcmp(argv[4], "cell::name")) {
          tmp = xctx->inst[i].name;
          Tcl_SetResult(interp, (char *) tmp, TCL_VOLATILE);
        } else if(strstr(argv[4], "cell::") ) {
          tmp = get_tok_value(xctx->sym[xctx->inst[i].ptr].prop_ptr, argv[4]+6, with_quotes);
          dbg(1, "scheduler(): xschem getprop: looking up instance %d prop cell::|%s| : |%s|\n", i, argv[4]+6, tmp);
          Tcl_SetResult(interp, (char *) tmp, TCL_VOLATILE);
        } else {
          Tcl_SetResult(interp, (char *)get_tok_value(xctx->inst[i].prop_ptr, argv[4], with_quotes), TCL_VOLATILE);
        }
      } else if(argc > 2 && !strcmp(argv[2], "instance_pin")) {
        /*   0       1        2         3   4       5     */
        /* xschem getprop instance_pin X10 PLUS [pin_attr]  */
        /* xschem getprop instance_pin X10  1   [pin_attr]  */
        int inst, n;
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
        n = get_inst_pin_number(inst, argv[4]);
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
              value = get_tok_value(xctx->sym[xctx->inst[inst].ptr].rect[PINLAYER][n].prop_ptr,argv[5],0);
            }
            if(value[0] != 0) {
              char *ss;
              int slot;
              if((ss = strchr(xctx->inst[inst].instname, ':')) ) {
                sscanf(ss + 1, "%d", &slot);
                if(strstr(value, ":")) value = find_nth(value, ":", "", 0, slot);
              }
              Tcl_SetResult(interp, (char *)value, TCL_VOLATILE);
            }
            my_free(_ALLOC_ID_, &subtok);
          }
        }
      /* xschem getprop symbol lm358.sym [type] */
      } else if(argc > 2 && !strcmp(argv[2], "symbol")) {
        int i;
        if(argc < 4) {
          Tcl_SetResult(interp, "xschem getprop symbol needs 1 or 2 or 3 additional arguments", TCL_STATIC);
          return TCL_ERROR;
        }

        i = get_symbol(argv[3]);
        if( i == -1) {
          Tcl_SetResult(interp, "Symbol not found", TCL_STATIC);
          return TCL_ERROR;
        }
        if(argc < 5)
          Tcl_SetResult(interp, xctx->sym[i].prop_ptr, TCL_VOLATILE);
        else if(argc == 5)
          Tcl_SetResult(interp, (char *)get_tok_value(xctx->sym[i].prop_ptr, argv[4], 0), TCL_VOLATILE);
        else if(argc > 5)
          Tcl_SetResult(interp, (char *)get_tok_value(xctx->sym[i].prop_ptr, argv[4], atoi(argv[5])), TCL_VOLATILE);

      } else if(argc > 2 && !strcmp(argv[2], "rect")) { /* xschem getprop rect c n token */
        if(argc < 6) {
          Tcl_SetResult(interp, "xschem getprop rect needs <color> <n> <token>", TCL_STATIC);
          return TCL_ERROR;
        } else {
          int with_quotes = 0;
          int c = atoi(argv[3]);
          int n = atoi(argv[4]);
          if(argc > 6) with_quotes = atoi(argv[6]);
          Tcl_SetResult(interp, (char *)get_tok_value(xctx->rect[c][n].prop_ptr, argv[5], with_quotes), TCL_VOLATILE);
        }
      } else if(argc > 2 && !strcmp(argv[2], "text")) { /* xschem getprop text n token */
        if(argc < 5) {
          Tcl_SetResult(interp, "xschem getprop text needs <n> <token>", TCL_STATIC);
          return TCL_ERROR;
        } else {
          int n = atoi(argv[3]);
          if(!strcmp(argv[4], "txt_ptr"))
            Tcl_SetResult(interp, xctx->text[n].txt_ptr, TCL_VOLATILE);
          else
            Tcl_SetResult(interp, (char *)get_tok_value(xctx->text[n].prop_ptr, argv[4], 2), TCL_VOLATILE);
        }
      } else if(argc > 2 && !strcmp(argv[2], "wire")) { /* xschem getprop wire n token */
        if(argc < 5) {
          Tcl_SetResult(interp, "xschem getprop wire needs <n> <token>", TCL_STATIC);
          return TCL_ERROR;
        } else {
          int n = atoi(argv[3]);
          Tcl_SetResult(interp, (char *)get_tok_value(xctx->wire[n].prop_ptr, argv[4], 2), TCL_VOLATILE);
        }
      }
    }

    /* get_sch_from_sym inst [symbol]
     *   get schematic associated with instance 'inst'
     *   if inst==-1 and a 'symbol' name is given get sch associated with symbol */
    else if(!strcmp(argv[1], "get_sch_from_sym") )
    {
      int inst = -1;
      int sym = -1;
      char filename[PATH_MAX];
      my_strncpy(filename,  "", S(filename));
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}

      if(argc > 2) {
        if(argc > 3 && atoi(argv[2]) == -1) {
          sym = get_symbol(argv[3]);
          if(sym < 0) {
            Tcl_SetResult(interp, "xschem get_sch_from_sym: symbol not found", TCL_STATIC);
            return TCL_ERROR;
          }
        }
        else {
          inst = get_instance(argv[2]);
          if(inst < 0) {
            Tcl_SetResult(interp, "xschem get_sch_from_sym: instance not found", TCL_STATIC);
            return TCL_ERROR;
          }
        }
        if( xctx->inst[inst].ptr >= 0  && sym == -1) {
          sym = xctx->inst[inst].ptr;
        }
        if(sym >= 0) get_sch_from_sym(filename, sym + xctx->sym, inst, 0);
      }
      Tcl_SetResult(interp, filename, TCL_VOLATILE);
    }

    /* get_sym_type symname
     *   get "type" value from global attributes of symbol,
     *   looking frst in loaded symbols, then looking in symbol file
     *   symbols that are not already loaded in the design will not be loaded */
    else if(!strcmp(argv[1], "get_sym_type") )
    {
      char *s=NULL;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}

      if(argc < 3) {Tcl_SetResult(interp, "Missing arguments", TCL_STATIC);return TCL_ERROR;}
      get_sym_type(argv[2], &s, NULL, NULL, NULL);

      Tcl_SetResult(interp, s, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &s);
    }

    /* get_tok str tok [with_quotes]
     *   get value of token 'tok' in string 'str'
     *   'with_quotes' (default:0) is an integer passed to get_tok_value() */
    else if(!strcmp(argv[1], "get_tok") )
    {
      char *s=NULL;
      int t;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      Tcl_SetResult(interp, my_itoa((int)xctx->tok_size), TCL_VOLATILE);
    }

    /* globals
     *   Return various global variables used in the program */
    else if(!strcmp(argv[1], "globals"))
    {
      static char res[8192];
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      Tcl_ResetResult(interp);
      my_snprintf(res, S(res), "*******global variables:*******\n"); Tcl_AppendResult(interp, res, NULL);

      my_snprintf(res, S(res), "areax1=%d\n", xctx->areax1); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "areay1=%d\n", xctx->areay1); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "areax2=%d\n", xctx->areax2); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "areay2=%d\n", xctx->areay2); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "areaw=%d\n", xctx->areaw); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "areah=%d\n", xctx->areah); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "xrect[0].width=%d\n",
              xctx->xrect[0].width); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "xrect[0].height=%d\n",
              xctx->xrect[0].height); Tcl_AppendResult(interp, res, NULL);

      my_snprintf(res, S(res), "INT_LINE_W(lw)=%d\n", INT_LINE_W(xctx->lw)); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "lw=%g\n", xctx->lw); Tcl_AppendResult(interp, res, NULL);
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
      my_snprintf(res, S(res), "maxs=%d\n", xctx->maxs); Tcl_AppendResult(interp, res, NULL);
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
      { const char *p, *t;
        my_snprintf(res, S(res), "previous_instance[%d]=%d\n",
            i,xctx->previous_instance[i]); Tcl_AppendResult(interp, res, NULL);
        my_snprintf(res, S(res), "sch_path[%d]=%s\n",i,xctx->sch_path[i]?
            xctx->sch_path[i]:"<NULL>"); Tcl_AppendResult(interp, res, NULL);
        my_snprintf(res, S(res), "sch[%d]=%s\n",i,xctx->sch[i]); Tcl_AppendResult(interp, res, NULL);

        p = xctx->hier_attr[i].prop_ptr ? xctx->hier_attr[i].prop_ptr : "<NULL>";
        t = xctx->hier_attr[i].templ ? xctx->hier_attr[i].templ : "<NULL>";
        my_snprintf(res, S(res), "lcc[%d].prop_ptr=%s\n", i, p);
        Tcl_AppendResult(interp, res, NULL);
        my_snprintf(res, S(res), "lcc[%d].templ=%s\n", i, t);
        Tcl_AppendResult(interp, res, NULL);

      }
      my_snprintf(res, S(res), "modified=%d\n", xctx->modified); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "color_ps=%d\n", color_ps); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "hilight_nets=%d\n", xctx->hilight_nets); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "semaphore=%d\n", xctx->semaphore); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "constr_mv=%d\n", xctx->constr_mv); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "ui_state=%d\n", xctx->ui_state); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "ui_state2=%d\n", xctx->ui_state2); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "drag_elements=%d\n", xctx->drag_elements); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "last_command=%d\n", xctx->last_command); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "prep_net_structs=%d\n", xctx->prep_net_structs); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "prep_hi_structs=%d\n", xctx->prep_hi_structs); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "prep_hash_inst=%d\n", xctx->prep_hash_inst); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "prep_hash_wires=%d\n", xctx->prep_hash_wires); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "need_reb_sel_arr=%d\n", xctx->need_reb_sel_arr); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "undo_type=%d\n", xctx->undo_type); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "******* end global variables:*******\n"); Tcl_AppendResult(interp, res, NULL);

#ifdef __unix__
      my_snprintf(res, S(res), "******* Xserver options: *******\n"); Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "XMaxRequestSize=%ld\n", XMaxRequestSize(display));
      Tcl_AppendResult(interp, res, NULL);
      my_snprintf(res, S(res), "XExtendedMaxRequestSize=%ld\n", XExtendedMaxRequestSize(display));
      Tcl_AppendResult(interp, res, NULL);
#endif

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
      #if HAS_CAIRO==1
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

    /* go_back [what]
     *   Go up one level (pop) in hierarchy
     *   if integer 'what' given pass it to the go_back() function
     *   what = 1: ask confirm save if current schematic modified.
     *   what = 2: do not reset window title */
    else if(!strcmp(argv[1], "go_back"))
    {
      int what = 1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2 ) {
        what = atoi(argv[2]);
      }
      if(xctx->semaphore == 0) go_back(what);
      Tcl_ResetResult(interp);
    }

    /* grabscreen
     *   grab root window */
    else if(!strcmp(argv[1], "grabscreen"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      #if defined(__unix__) && HAS_CAIRO==1
      xctx->ui_state |= GRABSCREEN;
      tclvareval("grab set -global ", xctx->top_path, ".drw", NULL);
      #endif
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      xctx->enable_drill = 0;
      if(argc >=3 && !strcmp(argv[2], "drill")) xctx->enable_drill = 1;
      hilight_net(0);
      redraw_hilights(0);
      Tcl_ResetResult(interp);
    }
    /* hilight_instname [-fast] inst
     *   Highlight instance 'inst'
     * if '-fast' is specified do not redraw
     *   'inst' can be an instance name or number */
    else if(!strcmp(argv[1], "hilight_instname"))
    {
      const char *instname=NULL;
      int i, fast = 0;
      
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      for(i = 2; i < argc; i++) {
        if(argv[i][0] == '-') {
          if(!strcmp(argv[i], "-fast")) {
            fast = 1;
          }
        } else { 
          instname = argv[i];
          break;
        }
      } 
      if(instname) {
        int inst;
        char *type;
        int incr_hi;
        xctx->enable_drill=0;
        incr_hi = tclgetboolvar("incr_hilight");
        prepare_netlist_structs(0);
        if((inst = get_instance(instname)) < 0 ) {
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
            inst_hilight_hash_lookup(inst, xctx->hilight_color, XINSERT_NOREPLACE);
            if(incr_hi) incr_hilight_color();
          }
          dbg(1, "hilight_nets=%d\n", xctx->hilight_nets);
          if(!fast) {
            if(xctx->hilight_nets) propagate_hilights(1, 0, XINSERT_NOREPLACE);
            redraw_hilights(0);
          }
        }
      }
      Tcl_ResetResult(interp);
    }
    /* hilight_netname [-fast] net 
     *   Highlight net name 'net'
     *   if '-fast' is given do not redraw hilights after operation */
    else if(!strcmp(argv[1], "hilight_netname"))
    {
      int ret = 0, fast = 0, i;
      const char *net = NULL;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      for(i = 2; i < argc; i++) {
        if(argv[i][0] == '-') {
          if(!strcmp(argv[i], "-fast")) {
            fast = 1;
          }
        } else {
          net = argv[i];
          break;
        }
      }
      if(net) {
        ret = hilight_netname(net,  fast);
      }
      Tcl_SetResult(interp, ret ? "1" : "0" , TCL_STATIC);
    }
    else { cmd_found = 0;}
    break;
    case 'i': /*----------------------------------------------*/
    #if HAS_CAIRO==1
    /* image [invert|white_transp|black_transp|transp_white|transp_black|write_back|
     *        blend_white|blend_black]
     *   Apply required changes to selected images
     *   invert: invert colors
     *   white_transp: transform white color to transparent (alpha=0)
     *   black_transp: transform black color to transparent (alpha=0)
     *   transp_white: transform transparent to white color
     *   transp_black: transform transparent to black color
     *   blend_white:  blend with white background and remove alpha
     *   blend_black:  blend with black background and remove alpha
     *   write_back:   write resulting image back into `image_data` attribute
     */
    if(!strcmp(argv[1], "image"))
    {
      int n, i, c;
      int what = 0;
      xRect *r;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc < 3) {
        Tcl_SetResult(interp, "Missing arguments", TCL_STATIC);
        return TCL_ERROR;
      }
      if(!strcmp(argv[2], "help")) {
        Tcl_SetResult(interp,
           "xschem image [invert|white_transp|black_transp|transp_white|transp_black|\n"
           "              blend_white|blend_black|write_back]",
            TCL_STATIC);
        return TCL_OK;
      }
      if(xctx->lastsel == 0) {
        Tcl_SetResult(interp, "No images selected", TCL_STATIC);
        return TCL_ERROR;
      }
      for(i = 2; i < argc; i++) {
        if(!strcmp(argv[i], "invert"))       what |=   1;
        if(!strcmp(argv[i], "white_transp")) what |=   2;
        if(!strcmp(argv[i], "black_transp")) what |=   4;
        if(!strcmp(argv[i], "transp_white")) what |=   8;
        if(!strcmp(argv[i], "transp_black")) what |=  16;
        if(!strcmp(argv[i], "blend_white"))  what |=  32;
        if(!strcmp(argv[i], "blend_black"))  what |=  64;
        if(!strcmp(argv[i], "write_back"))   what |= 256;
      }
      if(what) {
        rebuild_selected_array();
        if(what & 256) set_modify(1);
        xctx->push_undo();
        for(n=0; n < xctx->lastsel; ++n) {
          if(xctx->sel_array[n].type == xRECT) {
            i = xctx->sel_array[n].n;
            c = xctx->sel_array[n].col;
            r = &xctx->rect[c][i];
            if(c == GRIDLAYER && r->flags & 1024) {
            edit_image(what, &xctx->rect[c][i]);
            }
          }
        }
        draw();
      }
      Tcl_ResetResult(interp);
    }
    else
    #endif
    /* instance sym_name x y rot flip [prop] [n]
     *   Place a new instance of symbol 'sym_name' at position x,y,
     *   rotation and flip  set to 'rot', 'flip'
     *   if 'prop' is given it is the new instance attribute
     *   string (default: symbol template string)
     *   if 'n' is given it must be 0 on first call
     *   and non zero on following calls
     *   It is used only for efficiency reasons if placing multiple instances */
    if(!strcmp(argv[1], "instance"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc==7) {
       /*           pos sym_name      x                y             rot       */
        place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), (short)atoi(argv[5]),
               /* flip              prop draw first to_push_undo */
               (short)atoi(argv[6]),NULL,  3,   1,      1);
        set_modify(1);
      } else if(argc==8) {
        place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), (short)atoi(argv[5]),
               (short)atoi(argv[6]), argv[7], 3, 1, 1);
        set_modify(1);
      } else if(argc==9) {
        int x = !(atoi(argv[8]));
        place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), (short)atoi(argv[5]),
               (short)atoi(argv[6]), argv[7], 0, x, 1);
        set_modify(1);
      }
    }

    /* instance_bbox inst
     *   return instance and symbol bounding boxes
     *   'inst' can be an instance name or number */
    else if(!strcmp(argv[1], "instance_bbox"))
    {
      int i;
      char s[200];
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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

    /* instance_coord [instance]
     *   Return instance name, symbol name, x placement coord, y placement coord, rotation and flip
     *   of selected instances
     *   if 'instance' is given (instance name or number) return data about specified instance
     *   Example:
     *     xschem [~] xschem instance_coord
     *     {R5} {res.sym} 260 260 0 0
     *     {C1} {capa.sym} 150 150 1 1 */
    else if(!strcmp(argv[1], "instance_coord"))
    {
      xSymbol *symbol;
      short flip, rot;
      double x0,y0;
      int n, i = 0;
      int user_inst = -1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        i = get_instance(argv[2]);
        if(i < 0) {
          Tcl_SetResult(interp, "xschem instance_net: instance not found", TCL_STATIC);
          return TCL_ERROR;
        }
        user_inst = i;
      }

      rebuild_selected_array();
      for(n=0; user_inst >=0 || n < xctx->lastsel; ++n) {
        if(user_inst >=0 || xctx->sel_array[n].type == ELEMENT) {
          char srot[16], sflip[16], sx0[70], sy0[70];
          if(user_inst == -1) i = xctx->sel_array[n].n;
          else i = user_inst;
          x0 = xctx->inst[i].x0;
          y0 = xctx->inst[i].y0;
          rot = xctx->inst[i].rot;
          flip = xctx->inst[i].flip;
          symbol = xctx->sym + xctx->inst[i].ptr;
          my_snprintf(srot, S(srot), "%d", rot);
          my_snprintf(sflip, S(sflip), "%d", flip);
          my_snprintf(sx0, S(sx0), "%g", x0);
          my_snprintf(sy0, S(sy0), "%g", y0);
          Tcl_AppendResult(interp, "{", xctx->inst[i].instname, "} ", "{", symbol->name, "} ",
             sx0, " ", sy0, " ", srot, " ", sflip, "\n", NULL);
          if(user_inst >= 0) break;
        }
      }
    }

    /* instance_list
     *   Return a list of 3-items. Each 3-item is
     *   an instance name followed by the symbol reference and symbol type.
     *   Example: xschem instance_list -->
     *     {x1}  {sky130_tests/bandgap.sym} {subcircuit}
     *     {...} {...}                      {...}
     *     ...
     */
    else if(!strcmp(argv[1], "instance_list"))
    {
      int i;
      char *s = NULL;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      for(i = 0; i < xctx->instances; ++i) {
        const char *name = xctx->inst[i].name ? xctx->inst[i].name : "";
        char *instname = xctx->inst[i].instname ? xctx->inst[i].instname : "";
        char *type = (xctx->inst[i].ptr + xctx->sym)->type;
        type = type ? type : "";
        if(i > 0) my_mstrcat(_ALLOC_ID_, &s, "\n", NULL);
        my_mstrcat(_ALLOC_ID_, &s,  "{", instname, "} {", name, "} {", type, "}", NULL);
      }
      Tcl_SetResult(interp, (char *)s, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &s);
    }
    /* instance_net inst pin
     *   Return the name of the net attached to pin 'pin' of instance 'inst'
     *   Example: xschem instance_net x3 MINUS --> REF */
    else if(!strcmp(argv[1], "instance_net"))
    {
      /* xschem instance_net inst pin */
      int no_of_pins, i, p, multip;
      const char *str_ptr=NULL;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
        if(!strcmp(get_tok_value(xctx->sym[xctx->inst[i].ptr].rect[PINLAYER][p].prop_ptr, "name",0), argv[3])) {
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

    /* instance_nodemap inst [pin]
     *   Return the instance name followed by a list of 'pin net' associations
     *   example:  xschem instance_nodemap x3
     *   --> x3 PLUS LED OUT LEVEL MINUS REF
     *   instance x3 pin PLUS is attached to net LED, pin OUT to net LEVEL and so on...
     *   If 'pin' is given restrict map to only that pin */
    else if(!strcmp(argv[1], "instance_nodemap"))
    {
    /* xschem instance_nodemap [instance_name] */
      int p, no_of_pins;
      int inst = -1, first=1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      prepare_netlist_structs(0);
      if(argc > 2) {
        inst = get_instance(argv[2]);
        if(inst >=0) {
          Tcl_AppendResult(interp,  xctx->inst[inst].instname, " ",  NULL);
          no_of_pins= (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER];
          for(p=0;p<no_of_pins;p++) {
            const char *pin;
            pin = get_tok_value(xctx->sym[xctx->inst[inst].ptr].rect[PINLAYER][p].prop_ptr, "name",0);
            if(!pin[0]) pin = "--ERROR--";
            if(argc > 3 && strcmp(argv[3], pin)) continue;
            if(first == 0) Tcl_AppendResult(interp, " ", NULL);
            Tcl_AppendResult(interp, pin, " ",
                  xctx->inst[inst].node && xctx->inst[inst].node[p] ? xctx->inst[inst].node[p] : "{}", NULL);
            first = 0;
          }
        }
      }
    }

    /* instance_number inst [n]
     *   Return the position of instance 'inst' in the instance array
     *   If 'n' is given set indicated instance position to 'n' */
    else if(!strcmp(argv[1], "instance_number"))
    {
      int i;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc < 3) {
        Tcl_SetResult(interp, "xschem instance_number 1 additional argument", TCL_STATIC);
        return TCL_ERROR;
      }
      if((i = get_instance(argv[2])) < 0 ) {
        Tcl_SetResult(interp, "xschem instance_number: instance not found", TCL_STATIC);
        return TCL_ERROR;
      }

      if(argc > 3) {
        unselect_all(0);
        select_element(i, SELECTED, 1, 1);
        rebuild_selected_array();
        i = atoi(argv[3]);
        change_elem_order(i);
        draw();
      }
      Tcl_SetResult(interp, my_itoa(i), TCL_VOLATILE);
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
      double pinx0, piny0;
      char num[60];
      int p, i, no_of_pins, slot;
      const char *pin;
      char *ss;
      char *tmpstr = NULL;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
        if(slot > 0 && !strcmp(argv[3], "pinnumber") && strstr(pin, ":")) pin = find_nth(pin, ":", "", 0, slot);
        if(!strcmp(pin, argv[4])) break;
      }
      if(p >= no_of_pins) {
        Tcl_SetResult(interp, "", TCL_STATIC);
        return TCL_OK;
      }
      get_inst_pin_coord(i, p, &pinx0, &piny0);
      my_snprintf(num, S(num), "{%s} %g %g", get_tok_value(rct[p].prop_ptr, "name", 0), pinx0, piny0);
      Tcl_SetResult(interp, num, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &tmpstr);
    }

    /* instance_pins inst
     *   Return list of pins of instance 'inst'
     *   'inst can be an instance name or a number */
    else if(!strcmp(argv[1], "instance_pins"))
    {
      char *pins = NULL;
      int p, i, no_of_pins;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        prepare_netlist_structs(0);
        if((i = get_instance(argv[2])) < 0 ) {
          Tcl_SetResult(interp, "xschem instance_pins: instance not found", TCL_STATIC);
          return TCL_ERROR;
        }
        no_of_pins= (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];
        for(p=0;p<no_of_pins;p++) {
          const char *pin;
          pin = get_tok_value(xctx->sym[xctx->inst[i].ptr].rect[PINLAYER][p].prop_ptr, "name",0);
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      xSymbol *symbol;
      xRect *rct;
      double pinx0, piny0;
      char *pins = NULL;
      int p, i, no_of_pins;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      prepare_netlist_structs(0);
      if(argc < 3) {
        Tcl_SetResult(interp, "xschem instances_to_net requires a net name argument", TCL_STATIC);
        return TCL_ERROR;
      }
      for(i = 0;i < xctx->instances; ++i) {
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
            get_inst_pin_coord(i, p, &pinx0, &piny0);
            my_strncpy(xx, dtoa(pinx0), S(xx));
            my_strncpy(yy, dtoa(piny0), S(yy));
            my_mstrcat(_ALLOC_ID_, &pins, "} {", xx, "} {", yy, "} } ", NULL);
          }
        }
      }
      Tcl_SetResult(interp, pins ? pins : "", TCL_VOLATILE);
      my_free(_ALLOC_ID_, &pins);
    }

    /* is_generator symbol
     *   tell if 'symbol' is a generator (symbol(param1,param2,...) */
    else if(!strcmp(argv[1], "is_generator"))
    {
      char s[30];
      if(argc > 2) {
        my_snprintf(s, S(s), "%d", is_generator(argv[2]));
        Tcl_SetResult(interp, s, TCL_VOLATILE);
      }
    }
    else { cmd_found = 0;}
    break;
    case 'l': /*----------------------------------------------*/
    /* line [x1 y1 x2 y2] [pos] [propstring] [draw]
     * line
     * line gui
     *   if 'x1 y1 x2 y2'is given place line on current
     *   layer (rectcolor) at indicated coordinates.
     *   if 'pos' is given insert at given position in line array.
     *   if 'pos' set to -1 append to last element in line array.
     *   'propstring' is the attribute string. Set to empty if not given.
     *   if 'draw' is set to 1 (default) draw the new object, else don't
     *   If no coordinates are given start a GUI operation of line placement
     *   if `gui` argument is given start a line GUI placement with 1st point
     *   set to current mouse coordinates */
    if(!strcmp(argv[1], "line"))
    {
      double x1,y1,x2,y2;
      int pos, save;
      int draw = 1;
      const char *prop_str = NULL;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 5) {
        x1=atof(argv[2]);
        y1=atof(argv[3]);
        x2=atof(argv[4]);
        y2=atof(argv[5]);
        ORDER(x1,y1,x2,y2);
        pos=-1;
        if(argc > 6) pos=atoi(argv[6]);
        if(argc > 7) prop_str = argv[7];
        if(argc > 8) draw = atoi(argv[8]);
        storeobject(pos, x1,y1,x2,y2,LINE,xctx->rectcolor,0,prop_str);
        if(draw) {
          save = xctx->draw_window; xctx->draw_window = 1;
          drawline(xctx->rectcolor,NOW, x1,y1,x2,y2, 0, NULL);
          xctx->draw_window = save;
        }
        set_modify(1);
      }
      else if(argc == 3 && !strcmp(argv[2], "gui")) {
        int prev_state = xctx->ui_state;
        int infix_interface = tclgetboolvar("infix_interface");
        if(infix_interface) {
          start_line(xctx->mousex_snap, xctx->mousey_snap);
          if(prev_state == STARTLINE) {
            tcleval("set constr_mv 0" );
            xctx->constr_mv=0;
          }
        } else {
          xctx->last_command = 0;
          xctx->ui_state |= MENUSTART;
          xctx->ui_state2 = MENUSTARTLINE;
        }
      }
      else {
        xctx->last_command = 0;
        xctx->ui_state |= MENUSTART;
        xctx->ui_state2 = MENUSTARTLINE;
      }
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      Tcl_ResetResult(interp);
      hier_psprint(&res, 2);
      Tcl_SetResult(interp, res, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &res);
    }

    /* list_hilights [sep | all | all_nets | all_inst]
     *    Sorted list of non port or non top level current level highlight nets,
     *    separated by character 'sep' (default: space)
     *    if `all_inst` is given list all instance hilights
     *    if `all_nets` is given list all net hilights
     *    if `all` is given list all hash content */
    else if(!strcmp(argv[1], "list_hilights"))
    {
      const char *sep = "{ }";
      int i, all = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      for(i = 2; i < argc; i++) {
        if(!strcmp(argv[i], "all")) all = 3;
        else if(!strcmp(argv[i], "all_inst")) all = 2;
        else if(!strcmp(argv[i], "all_nets")) all = 1;
        else sep = argv[i];
      }
      list_hilights(all);
      if(!all) tclvareval("join [lsort -decreasing -dictionary {", tclresult(), "}] ", sep, NULL);
    }

    /* list_nets
     *    List all nets with type (in / out / inout / net) */
    else if(!strcmp(argv[1], "list_nets"))
    {
      char *result = NULL;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      list_nets(&result);
      Tcl_SetResult(interp, result, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &result);
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

    /* load [-nosymbols|-gui|-noundoreset|-nofullzoom|-keep_symbols] f
     *   Load a new file 'f'.
     *   '-gui': ask to save modified file or warn if opening an already
     *       open file or opening a new(not existing) file.
     *   '-noundoreset': do not reset the undo history
     *   '-nosymbols': do not load symbols (used if loading a symbol instead of
     *       a schematic)
     *   '-nofullzoom': do not do a full zoom on new schematic.
     *   '-nodraw': do not draw.
     *   '-keep_symbols': retain symbols that are already loaded.
     */
    else if(!strcmp(argv[1], "load") )
    {
      int load_symbols = 1, force = 1, undo_reset = 1, nofullzoom = 0, nodraw = 0;
      int keep_symbols = 0, first;
      int i;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}

      for(i = 2; i < argc; i++) {
        if(argv[i][0] == '-') {
          if(!strcmp(argv[i], "-nosymbols")) { 
            load_symbols = 0 ;
          } else if(!strcmp(argv[i], "-gui")) {
            force = 0;
          } else if(!strcmp(argv[i], "-noundoreset")) {
            undo_reset = 0;
          } else if(!strcmp(argv[i], "-nofullzoom")) {
            nofullzoom = 1;
          } else if(!strcmp(argv[i], "-nodraw")) {
            nofullzoom = 1; nodraw = 1;
          } else if(!strcmp(argv[i], "-keep_symbols")) {
            keep_symbols = 1;
          }
        } else {
          break;
        }
      }
      first = i;
      if(argc==2) {
        if(tclgetboolvar("new_file_browser")) {
          tcleval(
           "insert_symbol $new_file_browser_paths $new_file_browser_depth $new_file_browser_ext load"
          );
        } else {
          ask_new_file(0);
          tcleval("load_additional_files");
        }
      } else
      for(i = first; i < argc; i++) {
        char f[PATH_MAX + 100];
        my_snprintf(f, S(f),"regsub {^~/} {%s} {%s/}", argv[i], home_dir);
        tcleval(f);
        my_strncpy(f, tclresult(), S(f));
        if(force || !has_x || !xctx->modified  || save(1, 0) != -1 ) { /* save(1)==-1 --> user cancel */
          char win_path[WINDOW_PATH_SIZE];
          int skip = 0;
          dbg(1, "scheduler(): load: filename=%s\n", argv[i]);
          my_strncpy(f,  abs_sym_path(f, ""), S(f));
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
            int ret;
            clear_all_hilights();
            unselect_all(1);
            /* no implicit undo: if needed do it before loading */
            /* if(!undo_reset) xctx->push_undo(); */
            if(i == first) {
              if(undo_reset) xctx->currsch = 0;
              if(!keep_symbols) remove_symbols();
              if(!nofullzoom) {
                xctx->zoom=CADINITIALZOOM;
                xctx->mooz=1/CADINITIALZOOM;
                xctx->xorigin=CADINITIALX;
                xctx->yorigin=CADINITIALY;
              }
            }
            dbg(1, "scheduler: undo_reset=%d\n", undo_reset);

            if(i > first) {
              ret = new_schematic("create", "noconfirm", f, 1);
              if(undo_reset) {
                tclvareval("update_recent_file {", f, "}", NULL);
              }
            } else {
              ret = load_schematic(load_symbols, f, undo_reset, !force);
              dbg(1, "xschem load: f=%s, ret=%d\n", f, ret);
              if(undo_reset) {
                tclvareval("update_recent_file {", f, "}", NULL);
                my_strdup(_ALLOC_ID_, &xctx->sch_path[xctx->currsch], ".");
                if(xctx->portmap[xctx->currsch].table) str_hash_free(&xctx->portmap[xctx->currsch]);
                str_hash_init(&xctx->portmap[xctx->currsch], HASHSIZE);
                xctx->sch_path_hash[xctx->currsch] = 0;
                xctx->sch_inst_number[xctx->currsch] = 1;
              }
              if(nofullzoom) {
                if(!nodraw) draw();
              } else zoom_full(1, 0, 1 + 2 * tclgetboolvar("zoom_full_center"), 0.97);
            }
          }
        }
      }
      Tcl_SetResult(interp, xctx->sch[xctx->currsch], TCL_STATIC);
    }

    /* load_new_window [f]
     *   Load schematic in a new tab/window. If 'f' not given prompt user
     *   if 'f' is given as empty '{}' then open untitled.sch */
    else if(!strcmp(argv[1], "load_new_window") )
    {
      char f[PATH_MAX + 100];
      int cancel = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        int i;
        for(i = 2; i < argc; i++) {
          if(!is_from_web(argv[i])) {
            my_snprintf(f, S(f),"regsub {^~/} {%s} {%s/}", argv[i], home_dir);
            tcleval(f);
            /* tclvareval("file normalize {", tclresult(), "}", NULL); */
            my_strncpy(f, abs_sym_path(tclresult(), ""), S(f));
          } else {
            my_strncpy(f, argv[i], S(f));
          }
          if(f[0]) {
           char win_path[WINDOW_PATH_SIZE];
           dbg(1, "f=%s\n", f);
           if(check_loaded(f, win_path)) {
             char msg[PATH_MAX + 100];
             my_snprintf(msg, S(msg),
                "tk_messageBox -type okcancel -icon warning -parent [xschem get topwindow] "
                "-message {Warning: %s already open.}", f);
             tcleval(msg);
             if(strcmp(tclresult(), "ok")) continue;
           }
           new_schematic("create", "noconfirm", f, 1);
           tclvareval("update_recent_file {", f, "}", NULL);
          } else {
            new_schematic("create", NULL, NULL, 1);
          }
        }
      } else {
        tcleval("load_file_dialog {Load file} *.\\{sch,sym,tcl\\} INITIALLOADDIR");
        if(tclresult()[0]) {
          my_snprintf(f, S(f), "%s", tclresult());
        } else {
          cancel = 1;
        }
        if(!cancel) {
          if(f[0]) {
           dbg(1, "f=%s\n", f);
           new_schematic("create", "noconfirm", f, 1);
           tclvareval("update_recent_file {", f, "}", NULL);
          } else {
            new_schematic("create", NULL, NULL, 1);
          }
        }
      }
      Tcl_ResetResult(interp);
    }

    /* log f
     *   If 'f' is given output stderr messages to file 'f'
     *   if 'f' is not given and a file log is open, close log
     *   file and resume logging to stderr */
    else if(!strcmp(argv[1], "log"))
    { /* added check to avoid multiple open */
      if(argc > 2 && errfp == stderr ) {
        char f[PATH_MAX + 100];
        FILE *fp;

        my_snprintf(f, S(f),"regsub {^~/} {%s} {%s/}", argv[2], home_dir);
        tcleval(f);
        my_strncpy(f, tclresult(), S(f));
        fp = fopen(f, "w");
        if(fp) errfp = fp;
        else dbg(0, "xschem log: problems opening file %s\n", f);
    }
      else if(argc==2 && errfp != stderr) { fclose(errfp); errfp=stderr; }
    }

    /* load_symbol [symbol_file]
     *   Load specified symbol_file
     *   Returns:
     *     >= 0: symbol is already loaded or has been loaded
     *     <  0: symbol was not loaded
     */
    else if(!strcmp(argv[1], "load_symbol") )
    {
      int res = -2;
      struct stat buf;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) { 
        int i = get_symbol(rel_sym_path(argv[2]));
        if(i < 0 ) {
          if(!stat(argv[2], &buf)) { /* file exists */
            res = load_sym_def(rel_sym_path(argv[2]), NULL);
            if(res == 0) res = -1;
          } else {
            res = -3;
          }
        } else {
          res = 1;
        }
      }
      Tcl_SetResult(interp, my_itoa(res), TCL_VOLATILE);
    }

    /* log_write text
     *   write given string to log file, so tcl can write messages on the log file
     */
    else if(!strcmp(argv[1], "log_write"))
    {
      if(argc > 2) {
        dbg(0, "%s\n", argv[2]);
      }
    }

    /* logic_get_net net_name
     *   Get logic state of net named 'net_name'
     *   Returns 0, 1, 2, 3 for logic levels 0, 1, X, Z or nothing if no net found.
     */
    else if(!strcmp(argv[1], "logic_get_net"))
    {
      static char s[2];

      my_strncpy(s, "2", S(s));
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
            s[0] = '2'; /* Unknown (X) */
            break;
            case -13:
            s[0] = '3'; /* Hi-Z (Z) */
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      create_sch_from_sym();
      Tcl_ResetResult(interp);
    }

    /* make_sch_from_sel
     *   Create an LCC instance from selection and place it instead of selection
     *   also ask if a symbol (.sym) file needs to be created */
    else if(!strcmp(argv[1], "make_sch_from_sel"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      make_schematic_symbol_from_sel();
      Tcl_ResetResult(interp);
    }

    /* make_symbol
     *   From current schematic (circuit.sch) create a symbol (circuit.sym)
     *   using ipin.sym, opin.sym, iopin.sym in schematic
     *   to deduce symbol interface pins. */
    else if(!strcmp(argv[1], "make_symbol"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(has_x) tcleval("tk_messageBox -type okcancel -parent [xschem get topwindow] "
                        "-message {do you want to make symbol view ?}");
      if(!has_x || strcmp(tclresult(), "ok")==0) {
        save_schematic(xctx->sch[xctx->currsch], 0);
        make_symbol();
      }
      Tcl_ResetResult(interp);
    }

    /* merge [f]
     *   Merge another file. if 'f' not given prompt user. */
    else if(!strcmp(argv[1], "merge"))
    {
      char f[PATH_MAX + 100];
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc < 3) {
        merge_file(0, "");  /* 2nd param not used for merge 25122002 */
      }
      else {
        my_snprintf(f, S(f),"regsub {^~/} {%s} {%s/}", argv[2], home_dir);
        tcleval(f);
        my_strncpy(f, tclresult(), S(f));
        merge_file(0, f);
      }
      Tcl_ResetResult(interp);
    }

    /* move_instance inst x y rot flip [nodraw] [noundo]
     *   resets instance coordinates, and rotaton/flip. A dash will keep existing value
     *   if 'nodraw' is given do not draw the moved instance
     *   if 'noundo' is given operation is not undoable */
    else if(!strcmp(argv[1], "move_instance"))
    {
      int undo = 1, dr = 1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 7) {
        int i;
        for(i = 7; i < argc; i++) {
          if(!strcmp(argv[i], "nodraw")) dr = 0;
          if(!strcmp(argv[i], "noundo")) undo = 0;
        }
      }
      if(argc > 6) {
        int i;
        if((i = get_instance(argv[2])) < 0 ) {
          Tcl_SetResult(interp, "xschem move_instance: instance not found", TCL_STATIC);
          return TCL_ERROR;
        }
        if(undo) xctx->push_undo();
        if(strcmp(argv[3], "-")) xctx->inst[i].x0 = atof(argv[3]);
        if(strcmp(argv[4], "-")) xctx->inst[i].y0 = atof(argv[4]);
        if(strcmp(argv[5], "-")) xctx->inst[i].rot = (unsigned short)atoi(argv[5]);
        if(strcmp(argv[6], "-")) xctx->inst[i].flip = (unsigned short)atoi(argv[6]);
        symbol_bbox(i, &xctx->inst[i].x1, &xctx->inst[i].y1, &xctx->inst[i].x2, &xctx->inst[i].y2);
        xctx->prep_hash_inst=0;
        xctx->prep_net_structs=0;
        xctx->prep_hi_structs=0;
        if(dr) {
          draw();
        }
      }
    }
    /* move_objects [dx dy] [kissing] [stretch]
     *   Start a move operation on selection and let user terminate the operation in the GUI
     *   if kissing is given add nets to pins that touch other instances or nets
     *   if stretch is given stretch connected nets to follow instace pins
     *   if dx and dy are given move by that amount. */
    else if(!strcmp(argv[1], "move_objects"))
    {
      int nparam = 0;
      int kissing= 0;
      int stretch = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        int i;
        for(i = 2; i < argc; i++) {
          if(!strcmp(argv[i], "kissing")) {kissing = 1; nparam++;}
          if(!strcmp(argv[i], "stretch")) {stretch = 1; nparam++;}
        }
      }
      if(stretch) select_attached_nets();
      if(kissing) xctx->connect_by_kissing = 2;
      if(argc > 3 + nparam) {
        move_objects(START,0,0,0);
        move_objects( END,0,atof(argv[2]), atof(argv[3]));
      }
      else {
        xctx->ui_state |= MENUSTART;
        xctx->ui_state2 = MENUSTARTMOVE;
      }
      Tcl_ResetResult(interp);
    }
    /* my_strtok_r str delim quote keep_quote
     * test for my_strtok_r() function */
    else if(!strcmp(argv[1], "my_strtok_r"))
    {
      if(argc > 5) {
        char *strcopy = NULL, *strptr = NULL, *saveptr = NULL, *tok;

        my_strdup(_ALLOC_ID_, &strcopy, argv[2]);
        strptr = strcopy;

        while( (tok = my_strtok_r(strptr, argv[3], argv[4], atoi(argv[5]), &saveptr)) ) {
          strptr = NULL;
          Tcl_AppendResult(interp, "{", tok, "}\n", NULL);
        }
        my_free(_ALLOC_ID_, &strptr);

      }
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

    /* netlist [-keep_symbols|-noalert|-messages|-erc | -nohier] [filename]
     *   do a netlist of current schematic in currently defined netlist format
     *   if 'filename'is given use specified name for the netlist
     *   if 'filename' contains path components place the file in specified path location.
     *   if only a name is given and no path ('/') components are given use the
     *   default netlisting directory.
     *   This means that 'xschem netlist test.spice' and 'xschem netlist ./test.spice'
     *   will create the netlist in different places.
     *   netlisting directory is reset to previous setting after completing this command
     *   If -messages is given return the ERC messages instead of just a fail (1)
     *   or no fail (0) code. 
     *   If -erc is given it means netlister is called from gui, enable show infowindow
     *   If -nohier is given netlist only current level
     *   If -keep_symbols is given no not purge symbols encountered traversing the
     *   design hierarchy */
    else if(!strcmp(argv[1], "netlist") )
    {
      char *saveshow = NULL;
      int err = 0;
      int hier_netlist = 1;
      int i, messages = 0;
      int alert = 1;
      int keep_symbols=0, save_keep;
      int erc = 0;
      const char *fname = NULL;
      const char *path;
      char savedir[PATH_MAX];
      int done_netlist = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      yyparse_error = 0;
      my_strdup(_ALLOC_ID_, &saveshow, tclgetvar("show_infowindow_after_netlist"));
      set_netlist_dir(0, NULL);

      my_strncpy(savedir, tclgetvar("netlist_dir"), S(savedir));
      for(i = 2; i < argc; i++) {
        if(argv[i][0] == '-') {
          if(!strcmp(argv[i], "-messages")) {
            messages = 1;
          } else if(!strcmp(argv[i], "-erc")) {
            erc = 1;
          } else if(!strcmp(argv[i], "-keep_symbols")) {
            keep_symbols = 1;
          } else if(!strcmp(argv[i], "-noalert")) {
            alert = 0;
          } else if(!strcmp(argv[i], "-nohier")) {
            hier_netlist = 0;
          }
        } else {
          fname = argv[i];
          break;
        }
      }
      if(erc == 0) tclsetvar("show_infowindow_after_netlist", "never");
      if(fname) {
        my_strncpy(xctx->netlist_name, get_cell_w_ext(fname, 0), S(xctx->netlist_name));
        tclvareval("file dirname ", fname, NULL);
        path = tclresult();
        if(strchr(fname, '/')) {
          set_netlist_dir(1, path);
        }
      }
      if(set_netlist_dir(0, NULL) ) {
        done_netlist = 1;

        save_keep = tclgetboolvar("keep_symbols");
        if(keep_symbols) tclsetboolvar("keep_symbols", keep_symbols);
        if(xctx->netlist_type == CAD_SPICE_NETLIST)
          err = global_spice_netlist(hier_netlist, alert);
        else if(xctx->netlist_type == CAD_VHDL_NETLIST)
          err = global_vhdl_netlist(hier_netlist, alert);
        else if(xctx->netlist_type == CAD_VERILOG_NETLIST)
          err = global_verilog_netlist(hier_netlist, alert);
        else if(xctx->netlist_type == CAD_SPECTRE_NETLIST)
          err = global_spectre_netlist(hier_netlist, alert);
        else if(xctx->netlist_type == CAD_TEDAX_NETLIST)
          global_tedax_netlist(hier_netlist, alert);
        else
          if(has_x) tcleval("tk_messageBox -type ok -parent [xschem get topwindow] "
                            "-message {Please Set netlisting mode (Options menu)}");
        tclsetboolvar("keep_symbols", save_keep);

        if(erc == 0) {
          my_strncpy(xctx->netlist_name, "", S(xctx->netlist_name));
        }
      }
      else {
         if(has_x) tcleval("alert_ {Can not write into the netlist directory. Please check} {}");
         else dbg(0, "Can not write into the netlist directory. Please check");
         err = 1;
      }
      if(err) {
        if(has_x) {
          tclvareval("catch {", xctx->top_path, ".menubar entryconfigure Netlist -background red}", NULL);
          tclvareval("set tctx::", xctx->current_win_path, "_netlist red", NULL);
        }
      } else {
        if(has_x) {
          tclvareval("catch {", xctx->top_path, ".menubar entryconfigure Netlist -background Green}", NULL);
          tclvareval("set tctx::", xctx->current_win_path, "_netlist Green", NULL);
        }
      }
      tclsetvar("show_infowindow_after_netlist", saveshow);
      tcleval("eval_netlist_postprocess");
      set_netlist_dir(1, savedir);
      if(done_netlist) {
        if(messages) {
          Tcl_SetResult(interp, xctx->infowindow_text, TCL_VOLATILE);
        } else {
         Tcl_SetResult(interp, my_itoa(err), TCL_VOLATILE);
        }
      }
      my_free(_ALLOC_ID_, &saveshow);
    }

    /* new_process [f]
     *   Start a new xschem process for a schematic.
     *   If 'f' is given load specified schematic. */
    else if(!strcmp(argv[1], "new_process"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        char f[PATH_MAX + 100];
        my_snprintf(f, S(f),"regsub {^~/} {%s} {%s/}", argv[2], home_dir);
        tcleval(f);
        my_strncpy(f, tclresult(), S(f));
        new_xschem_process(f, 0);
      } else new_xschem_process("", 0);
      Tcl_ResetResult(interp);
    }

    /* new_schematic create|destroy|destroy_all|switch win_path file [draw]
     *   Open/destroy a new tab or window
     *     create: create new empty window or with 'file' loaded if 'file' given.
     *             The win_path must be given (even {} is ok).
     *             non empty win_path ({1}) will avoid warnings if opening the
     *             same file multiple times.
     *     destroy: destroy tab/window identified by win_path. Example:
     *              xschem new_schematic destroy .x1.drw
     *     destroy_all: close all tabs/additional windows
     *              if the 'force'argument is given do not issue a warning if modified
     *              tabs are about to be closed.
     *     switch: switch context to specified 'win_path' window or specified schematic name
     *              If 'draw' is given and set to 0 do not redraw after switching tab
     *              (only tab i/f)
     *              if win_path set to "previous" switch to previous schematic.
     *   Main window/tab has win_path set to .drw,
     *   Additional windows/tabs have win_path set to .x1.drw, .x2.drw and so on...
     */
    else if(!strcmp(argv[1], "new_schematic"))
    {
      int r = -1;
      int dr = 1;
      char s[20];
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {

        if(argc >= 6 && argv[5][0] == '0') dr = 0;
        if(argc == 3) r = new_schematic(argv[2], NULL, NULL, 1);
        else if(argc == 4) r = new_schematic(argv[2], argv[3], NULL, 1);
        else if(argc >= 5) {
          char f[PATH_MAX + 100];
          my_snprintf(f, S(f),"regsub {^~/} {%s} {%s/}", argv[4], home_dir);
          tcleval(f);
          my_strncpy(f, abs_sym_path(tclresult(), ""), S(f));
          r = new_schematic(argv[2], argv[3], f, dr);
        }
        my_snprintf(s, S(s), "%d", r);
        Tcl_SetResult(interp, s, TCL_VOLATILE);
      }
    }
    else { cmd_found = 0;}
    break;
    case 'o': /*----------------------------------------------*/
    /* only_probes
     * dim schematic to better show highlights */
    if(!strcmp(argv[1], "only_probes"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      xctx->only_probes = !xctx->only_probes;
      tclsetboolvar("only_probes", xctx->only_probes);
      toggle_only_probes();
      Tcl_ResetResult(interp);
    }

    /* origin x y [zoom]
     *   Move origin to 'x, y', optionally changing zoom level to 'zoom'
     *   A dash ('-') given for x or y will keep existing value */
    else if(!strcmp(argv[1], "origin"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 3) {
        if(strcmp(argv[2], "-")) xctx->xorigin = atof(argv[2]);
        if(strcmp(argv[3], "-")) xctx->yorigin = atof(argv[3]);
        if(argc > 4) {
          xctx->zoom = atof(argv[4]);
          xctx->mooz=1/xctx->zoom;
        }
        draw();
      }
      Tcl_ResetResult(interp);
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 3) {
        merge_file(10, ".sch"); /* set bit 3 to avoid doing move_objects(RUBBER,...) */
        xctx->deltax = atof(argv[2]);
        xctx->deltay = atof(argv[3]);
        move_objects(END, 0, 0.0, 0.0);
      } else {
        merge_file(2, ".sch");
      }
      Tcl_ResetResult(interp);
    }

    /* pinlist inst [attr]
     *   List all pins of instance 'inst'
     *   if no 'attr' is given return full attribute string,
     *   else return value for attribute 'attr'.
     *   Example: xschem pinlist x3 name
     *   -->  { {0} {PLUS} } { {1} {OUT} } { {2} {MINUS} }
     *   Example: xschem pinlist x3 dir
     *   -->  { {0} {in} } { {1} {out} } { {2} {in} }
     *   Example: xschem pinlist x3
     *   --> { {0} {name=PLUS dir=in } } { {1} {name=OUT dir=out } }
     *       { {2} {name=MINUS dir=in } }
     */
    else if(!strcmp(argv[1], "pinlist"))
    {
      int i, p, no_of_pins, first = 1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        if((i = get_instance(argv[2])) < 0 ) {
          Tcl_SetResult(interp, "xschem pinlist: instance not found", TCL_STATIC);
          return TCL_ERROR;
        }
        no_of_pins= (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];
        for(p=0;p<no_of_pins;p++) {
          if(first == 0) Tcl_AppendResult(interp, " ", NULL);
          if(argc > 3 && argv[3][0]) {
            Tcl_AppendResult(interp, "{ {", my_itoa(p), "} {",
              get_tok_value(xctx->sym[xctx->inst[i].ptr].rect[PINLAYER][p].prop_ptr, argv[3], 0),
              "} }", NULL);
          } else {
            Tcl_AppendResult(interp, "{ {", my_itoa(p), "} {",
               (xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][p].prop_ptr, "} }", NULL);
          }
          first = 0;
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      xctx->semaphore++;
      rebuild_selected_array();
      if(xctx->lastsel && xctx->sel_array[0].type==ELEMENT) {
        tclvareval("set INITIALINSTDIR [file dirname {",
             abs_sym_path(tcl_hook2(xctx->inst[xctx->sel_array[0].n].name), ""), "}]", NULL);
      }
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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

    /* polygon [gui]
     *   Start a GUI placement of a polygon
     *   if `gui` argument is given start a polygon GUI placement with 1st point
     *   set to current mouse coordinates */
    else if(!strcmp(argv[1], "polygon"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2 && !strcmp(argv[2], "gui")) {
        int infix_interface = tclgetboolvar("infix_interface");
        if(infix_interface) {
          xctx->mx_double_save=xctx->mousex_snap;
          xctx->my_double_save=xctx->mousey_snap;
          xctx->last_command = 0;
          new_polygon(PLACE, xctx->mousex_snap, xctx->mousey_snap);
        } else {
          xctx->ui_state |= MENUSTART;
          xctx->ui_state2 = MENUSTARTPOLYGON;
        }
      } else {
        xctx->ui_state |= MENUSTART;
        xctx->ui_state2 = MENUSTARTPOLYGON;
      }
    }

    /* preview_window create|draw|destroy|close [win_path] [file]
     *   destroy: will delete preview schematic data and destroy container window
     *   close: same as destroy but leave the container window.
     *   Used in fileselector to show a schematic preview.
     */
    else if(!strcmp(argv[1], "preview_window"))
    {
      int res = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc == 3) res = preview_window(argv[2], "", NULL);
      else if(argc == 4) res = preview_window(argv[2], argv[3], NULL);
      else if(argc == 5) {
        char f[PATH_MAX + 100];
        my_snprintf(f, S(f),"regsub {^~/} {%s} {%s/}", argv[4], home_dir);
        tcleval(f);
        my_strncpy(f, tclresult(), S(f));
        res = preview_window(argv[2], argv[3], f);
      }
      Tcl_SetResult(interp, my_itoa(res), TCL_VOLATILE);
    }


    /* print png|svg|ps|pdf|ps_full|pdf_full img_file [img_x img_y] [x1 y1 x2 y2]
     *   If img_x and img_y are set to 0 (recommended for svg and ps/pdf)
     *   they will be calculated by xschem automatically
     *   if img_x and img_y are given they will set the bitmap size, if
     *   area to export is not given then use the selection boundbox if
     *   a selection exists or do a full zoom.
     *   Export current schematic to image.
     *                            img x   y size    xschem area to export
     *      0     1    2    3         4   5             6    7   8   9
     *   xschem print png file.png  [400 300]       [ -300 -200 300 200 ]
     *   xschem print svg file.svg  [400 300]       [ -300 -200 300 200 ]
     *   xschem print ps  file.ps   [400 300]       [ -300 -200 300 200 ]
     *   xschem print eps file.eps  [400 300]       [ -300 -200 300 200 ]
     *   xschem print pdf file.pdf  [400 300]       [ -300 -200 300 200 ]
     *   xschem print ps_full  file.ps
     *   xschem print pdf_full file.pdf
     */
    else if(!strcmp(argv[1], "print") )
    {
      Zoom_info zi;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc < 3) {
        Tcl_SetResult(interp, "xschem print needs at least 1 more arguments: plot_type", TCL_STATIC);
        return TCL_ERROR;
      }
      if(argc > 3) {
        tclvareval("file normalize {", argv[3], "}", NULL);
        my_strncpy(xctx->plotfile, Tcl_GetStringResult(interp), S(xctx->plotfile));
      }
      if(!strcmp(argv[2], "pdf") || !strcmp(argv[2],"ps") || !strcmp(argv[2],"eps")) {
        double save_lw = xctx->lw;
        int fullzoom = 0;
        int w = 0, h = 0;
        int eps = 0;
        double x1, y1, x2, y2;

        if(!strcmp(argv[2],"eps")) eps = 1;
        if(eps && xctx->lastsel == 0) {
          if(has_x) tcleval("alert_ {EPS export works only on a selection} {}");
          else  dbg(0, "EPS export works only on a selection\n");
        } else if(argc == 6 && eps == 0) {
          if(xctx->lastsel) {
            xRect boundbox;
            calc_drawing_bbox(&boundbox, 1);
            x1 =boundbox.x1;
            y1 =boundbox.y1;
            x2 =boundbox.x2;
            y2 =boundbox.y2;
          } else {
            fullzoom = 2; /* 2: set paper size to bounding box instead of a4/letter */
          }
          w = atoi(argv[4]);
          h = atoi(argv[5]);
          if(w == 0) w = xctx->xrect[0].width;
          if(h == 0) h = xctx->xrect[0].height;
          save_restore_zoom(1, &zi);
          set_viewport_size(w, h, xctx->lw);
          if(xctx->lastsel) {
            zoom_box(x1, y1, x2, y2, 1.0);
            unselect_all(0);
          }
          else zoom_full(0, 0, 2 * tclgetboolvar("zoom_full_center"), 0.97);
          resetwin(1, 1, 1, w, h);
          ps_draw(7, fullzoom, eps);
          save_restore_zoom(0, &zi);
          resetwin(1, 1, 1, 0, 0);
          change_linewidth(save_lw);
        } else if(argc == 10 || xctx->lastsel) {
          if(xctx->lastsel) {
            xRect boundbox;
            calc_drawing_bbox(&boundbox, 1);
            unselect_all(0);
            x1 =boundbox.x1;
            y1 =boundbox.y1;
            x2 =boundbox.x2;
            y2 =boundbox.y2;
            w = (int) fabs(x2 - x1);
            h = (int) fabs(y2 - y1);
          } else {
            w = atoi(argv[4]);
            h = atoi(argv[5]);
            x1 = atof(argv[6]);
            y1 = atof(argv[7]);
            x2 = atof(argv[8]);
            y2 = atof(argv[9]);
          }
          fullzoom = 2;
          if(w == 0) w = (int) fabs(x2 - x1);
          if(h == 0) h = (int) fabs(y2 - y1);
          save_restore_zoom(1, &zi);
          set_viewport_size(w, h, xctx->lw);
          zoom_box(x1, y1, x2, y2, 1.0);
          resetwin(1, 1, 1, w, h);
          ps_draw(7, fullzoom, eps);
          save_restore_zoom(0, &zi);
          resetwin(1, 1, 1, 0, 0);
          change_linewidth(save_lw);
        } else {
          fullzoom = 0;
          ps_draw(7, fullzoom, eps);
        }
      }
      else if(!strcmp(argv[2], "pdf_full") || !strcmp(argv[2],"ps_full")) {
        int fullzoom = 1;
        ps_draw(7, fullzoom, 0);
      }
      else if(!strcmp(argv[2], "png")) {
        double save_lw = xctx->lw;
        int w = 0, h = 0;
        double x1, y1, x2, y2;
        if(argc == 6) {
          if(xctx->lastsel) {
            xRect boundbox;
            calc_drawing_bbox(&boundbox, 1);
            x1 =boundbox.x1;
            y1 =boundbox.y1;
            x2 =boundbox.x2;
            y2 =boundbox.y2;
          }
          w = atoi(argv[4]);
          h = atoi(argv[5]);
          if(w == 0) w = xctx->xrect[0].width;
          if(h == 0) h = xctx->xrect[0].height;
          save_restore_zoom(1, &zi);
          set_viewport_size(w, h, xctx->lw);
          if(xctx->lastsel) {
            zoom_box(x1, y1, x2, y2, 1.0);
            unselect_all(0);
          }
          else zoom_full(0, 0, 2 * tclgetboolvar("zoom_full_center"), 0.97);
          resetwin(1, 1, 1, w, h);
          print_image();
          save_restore_zoom(0, &zi);
          resetwin(1, 1, 1, 0, 0);
          change_linewidth(save_lw);
        } else if(argc == 10 || xctx->lastsel) {
          if(xctx->lastsel) {
            xRect boundbox;
            calc_drawing_bbox(&boundbox, 1);
            unselect_all(0);
            x1 =boundbox.x1;
            y1 =boundbox.y1;
            x2 =boundbox.x2;
            y2 =boundbox.y2;
            w = (int) fabs(x2 - x1);
            h = (int) fabs(y2 - y1);
          } else {
            w = atoi(argv[4]);
            h = atoi(argv[5]);
            x1 = atof(argv[6]);
            y1 = atof(argv[7]);
            x2 = atof(argv[8]);
            y2 = atof(argv[9]);
          }
          if(w == 0) w = (int) fabs(x2 - x1);
          if(h == 0) h = (int) fabs(y2 - y1);
          dbg(1, "w=%d h=%d, lw=%g bbox=%g %g %g %g\n", w, h, xctx->lw, x1, y1, x2, y2);
          save_restore_zoom(1, &zi);
          set_viewport_size(w, h, xctx->lw);
          zoom_box(x1, y1, x2, y2, 1.0);
          resetwin(1, 1, 1, w, h);
          print_image();
          save_restore_zoom(0, &zi);
          resetwin(1, 1, 1, 0, 0);
          change_linewidth(save_lw);
        } else {
          print_image();
        }
      }
      else if(!strcmp(argv[2], "svg")) {
        double save_lw = xctx->lw;
        int w = 0, h = 0;
        double x1, y1, x2, y2;
        if(argc == 6) {
          if(xctx->lastsel) {
            xRect boundbox;
            calc_drawing_bbox(&boundbox, 1);
            x1 =boundbox.x1;
            y1 =boundbox.y1;
            x2 =boundbox.x2;
            y2 =boundbox.y2;
          }
          w = atoi(argv[4]);
          h = atoi(argv[5]);
          if(w == 0) w = xctx->xrect[0].width;
          if(h == 0) h = xctx->xrect[0].height;
          save_restore_zoom(1, &zi);
          set_viewport_size(w, h, xctx->lw);
          if(xctx->lastsel) {
            zoom_box(x1, y1, x2, y2, 1.0);
            unselect_all(0);
          }
          else zoom_full(0, 0, 2 * tclgetboolvar("zoom_full_center"), 0.97);
          svg_draw();
          save_restore_zoom(0, &zi);
        } else if(argc == 10 || xctx->lastsel) {
          if(xctx->lastsel) {
            xRect boundbox;
            calc_drawing_bbox(&boundbox, 1);
            unselect_all(0);
            x1 =boundbox.x1;
            y1 =boundbox.y1;
            x2 =boundbox.x2;
            y2 =boundbox.y2;
            w = (int) fabs(x2 - x1);
            h = (int) fabs(y2 - y1);
          } else {
            w = atoi(argv[4]);
            h = atoi(argv[5]);
            x1 = atof(argv[6]);
            y1 = atof(argv[7]);
            x2 = atof(argv[8]);
            y2 = atof(argv[9]);
          }
          if(w == 0) w = (int) fabs(x2 - x1);
          if(h == 0) h = (int) fabs(y2 - y1);
          dbg(1, "w=%d, h=%d\n", w, h);
          save_restore_zoom(1, &zi);
          set_viewport_size(w, h, xctx->lw);
          zoom_box(x1, y1, x2, y2, 1.0);
          svg_draw();
          save_restore_zoom(0, &zi);
        } else {
          svg_draw();
        }
        resetwin(1, 1, 1, 0, 0);
        change_linewidth(save_lw);
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        print_hilight_net(atoi(argv[2]));
      }
    }

    /* print_spice_element inst
     *   Print spice raw netlist line for instance (number or name) 'inst' */
    else if(!strcmp(argv[1], "print_spice_element") )
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      xctx->push_undo();
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'r': /*----------------------------------------------*/

    /* raw what ...
     *     what = add | clear | datasets | index | info | loaded | list |
     *            new | points | rawfile | del | read | set | rename |
     *            sim_type | switch | switch_back | table_read | value | values | pos_at | vars |
     *
     *   xschem raw read filename [type [sweep1 sweep2]]
     *     if sweep1, sweep2 interval is given in 'read' subcommand load only the interval
     *     sweep1 <= sweep_var < sweep2
     *     type is the analysis type to load (tran, dc, ac, op, ...). If not given load first found in
     *     raw file.
     *
     *   xschem raw clear [rawfile [type]]
     *     unload given file and type. If type not given delete all type sfrom rawfile
     *     If a number 'n' is given as 'rawfile' delete the 'nth' raw file
     *     if no file is given unload all raw files.
     *
     *   xschem raw del name
     *     delete named vector from current raw file
     *
     *   xschem raw rename old_name new_name
     *     rename a node in the loaded raw file.
     *
     *   xschem raw info
     *     print information about loaded raw files and show the currently active one.
     *
     *   xschem raw new name type sweepvar start end step
     *     create a new raw file with sweep variable 'sweepvar' with number=(end - start) / step datapoints
     *     from start value 'start' and step 'step'
     *
     *   xschem raw list
     *     get list of saved simulation variables
     *
     *   xschem raw vars
     *     get number of simulation variables
     *
     *   xschem raw switch [n | rawfile type]
     *     make the indicated 'rawfile, type' the active one
     *     else if a number n is specified make the n-th raw data the active one.
     *     if no file or number is specified then switch to the next rawdata in the list.
     *
     *   xschem switch_back
     *     switch to previously active rawdata.
     *
     *   xschem raw datasets
     *     get number of datasets (simulation runs)
     *
     *   xschem raw value node n [dset]
     *     return n-th value of 'node' in raw file
     *     dset is the dataset to look into in case of multiple runs (first run = 0).
     *     if dset = -1 consider n as the absolute position into the whole data file
     *     (all datasets combined).
     *     If n is given as empty string {} return value at cursor b,
     *     dset not used in this case
     *
     *   xschem raw loaded
     *     return hierarchy level where raw file was loaded or -1 if no raw loaded
     *
     *   xschem raw rawfile
     *      return raw filename
     *
     *   xschem raw sim_type
     *      return raw loaded simulation type (ac, op, tran, ...)
     *
     *   xschem raw index node
     *     get index of simulation variable 'node'.
     *     Example:  raw index v(led) --> 46
     *
     *   xschem raw values node [dset]
     *     print all simulation values of 'node' for dataset 'dset' (default dset=0)
     *     dset= -1: print all values for all datasets
     *
     *   xschem raw pos_at node value [dset] [from_start] [to_end]
     *     returns the position, starting from 0 or from_start if given, to the end of dataset
     *     or to_end if given of the first point 'p' where node[p] and node[p+1] bracket value.
     *     If dset not given assume dset 0 (first one)
     *     This is usually done on the sweep (time) variable in transient sims where timestep is
     *     not uniform
     *
     *   xschem raw points [dset]
     *     print simulation points for dataset 'dset' (default: all dataset points combined)
     *
     *   xschem raw set node n value [dset]
     *     change loaded raw file data node[n] to value
     *     dset is the dataset to look into in case of multiple runs (first run = 0)
     *     dset = -1: consider n as the absolute position in the whole raw file
     *     (all datasets combined)
     *
     *   xschem raw table_read tablefile
     *     read a tabular data file.
     *     First line is the header line containing variable names.
     *     data is presented in column format after the header line
     *     First column is sweep (x-axis) variable
     *     Double empty lines start a new dataset
     *     Single empty lines are ignored
     *     Datasets can have different # of lines.
     *     new dataset do not start with a header row.
     *     Lines beginning with '#' are comments and ignored
     *
     *        time    var_a   var_b   var_cnode in the loaded raw file.
     *     # this is a comment, ignored
     *         0.0     0.0     1.8    0.3
     *       <single empty line: ignored>
     *         0.1     0.0     1.5    0.6
     *         ...     ...     ...    ...
     *       <empty line>
     *       <Second empty line: start new dataset>
     *         0.0     0.0     1.8    0.3
     *         0.1     0.0     1.5    0.6
     *         ...     ...     ...    ...
     *
     *   xschem raw add varname [expr] [sweep_var]
     *     add a 'varname' vector with all values set to 0 to loaded raw file if expr not given
     *     otherwise initialize data with values calculated from expr.
     *     if expr is given and also sweep_var is given use indicated sweep_var for expressions
     *     that need it. If sweep_var not given use first raw file variable as sweep variable.
     *     If varname is already existing and expr given recalculate data
     *     Example: xschem raw add power {outm outp - i(@r1[i]) *}
     *
     */
    if(!strcmp(argv[1], "raw") || !strcmp(argv[1], "raw_query"))
    {
      double sweep1 = -1.0, sweep2 = -1.0;
      int err = 0;
      int ret = 0;
      int i;
      Raw *raw = xctx->raw;
      Tcl_ResetResult(interp);
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 3 && !strcmp(argv[2], "table_read")) {
        ret = extra_rawfile(1, argv[3], "table", sweep1, sweep2);
        Tcl_SetResult(interp, my_itoa(ret), TCL_VOLATILE);
      } else if(argc > 3 && !strcmp(argv[2], "read")) {
        if(argc > 6) {
          sweep1 = atof_spice(argv[5]);
          sweep2 = atof_spice(argv[6]);
        }
        if(argc > 4) ret = extra_rawfile(1, argv[3], argv[4], sweep1, sweep2);
        else ret = extra_rawfile(1, argv[3], NULL, sweep1, sweep2);
        Tcl_SetResult(interp, my_itoa(ret), TCL_VOLATILE);
      } else if(argc > 2 && !strcmp(argv[2], "switch")) {
        if(argc > 4) {
          ret = extra_rawfile(2, argv[3], argv[4], -1.0, -1.0);
        } else if(argc > 3) {
          ret = extra_rawfile(2, argv[3], NULL, -1.0, -1.0);
        } else {
          ret = extra_rawfile(2, NULL, NULL, -1.0, -1.0);
        }
        /* only update_op() if switching into a 1-point OP or DC */
        if(ret && raw && raw->rawfile && raw->allpoints == 1 &&
           (!strcmp(xctx->raw->sim_type, "op") || !strcmp(xctx->raw->sim_type, "dc"))) {
          update_op();
        }
        Tcl_SetResult(interp, my_itoa(ret), TCL_VOLATILE);
      } else if(argc ==9 && !strcmp(argv[2], "new")) {
        ret = new_rawfile(argv[3], argv[4], argv[5], atof(argv[6]), atof(argv[7]),atof(argv[8]));
        Tcl_SetResult(interp, my_itoa(ret), TCL_VOLATILE);
      } else if(argc > 2 && !strcmp(argv[2], "info")) {
        ret = extra_rawfile(4, NULL, NULL, -1.0, -1.0);
      } else if(argc > 2 && !strcmp(argv[2], "switch_back")) {
        ret = extra_rawfile(5, NULL, NULL, -1.0, -1.0);
        /* only update_op() if switching into a 1-point OP or DC */
        if(ret && raw && raw->rawfile && raw->allpoints == 1 &&
           (!strcmp(xctx->raw->sim_type, "op") || !strcmp(xctx->raw->sim_type, "dc"))) {
          update_op();
        }
        Tcl_SetResult(interp, my_itoa(ret), TCL_VOLATILE);
      } else if(argc > 2 && !strcmp(argv[2], "clear")) {
        if(argc > 4)  {
          ret = extra_rawfile(3, argv[3], argv[4], -1.0, -1.0);
        } else if(argc > 3)  {
          ret = extra_rawfile(3, argv[3], NULL, -1.0, -1.0);
        } else {
          ret = extra_rawfile(3, NULL, NULL, -1.0, -1.0);
        }
        Tcl_SetResult(interp, my_itoa(ret), TCL_VOLATILE);
      } else if(argc > 2 && !strcmp(argv[2], "loaded")) {
        Tcl_SetResult(interp, my_itoa(sch_waves_loaded()), TCL_VOLATILE);
      } else if(raw && raw->values) {
        /* xschem raw value v(ldcp) 123 */
        if(argc > 4 && !strcmp(argv[2], "value")) {
          int dataset = -1;
          int point = argv[4][0] ? atoi(argv[4]) : -1;
          const char *node = argv[3];
          int idx = -1;
          if(argc > 5) dataset = atoi(argv[5]);
          idx = get_raw_index(node, NULL);
          if(idx >= 0) {
            double val;
            if( (dataset >=0 && point >= 0 && point < raw->npoints[dataset]) ||
                (dataset == -1 && point >= 0 && point < raw->allpoints)
              ) {
              val = get_raw_value(dataset, idx, point);
              Tcl_SetResult(interp, dtoa(val), TCL_VOLATILE);
            } else if(xctx->raw->cursor_b_val) {
              val = xctx->raw->cursor_b_val[idx];
              Tcl_SetResult(interp, dtoa(val), TCL_VOLATILE);
            }
          }
        } else if(argc > 3 && !strcmp(argv[2], "del")) {
          ret = raw_deletevar(argv[3]);
          Tcl_SetResult(interp, my_itoa(ret), TCL_VOLATILE);
        } else if(argc > 4 && !strcmp(argv[2], "rename")) {
          ret = raw_renamevar(argv[3], argv[4]);
          Tcl_SetResult(interp, my_itoa(ret), TCL_VOLATILE);
        } else if(argc > 3 && !strcmp(argv[2], "index")) {
          /* xschem raw index v(ldcp) */
          int idx;
          idx = get_raw_index(argv[3], NULL);
          Tcl_SetResult(interp, my_itoa(idx), TCL_VOLATILE);
        } else if(argc > 3 && !strcmp(argv[2], "values")) {
          /* xschem raw values ldcp [dataset] */
          int idx;
          char n[70];
          int p, dataset = 0;
          idx = get_raw_index(argv[3], NULL);
          if(argc > 4) dataset = atoi(argv[4]);
          if(idx >= 0) {
            int np;
            if(dataset < 0 )
              np = raw->allpoints;
            else
              np = raw->npoints[dataset];
            Tcl_ResetResult(interp);
            for(p = 0; p < np; p++) {
              sprintf(n, "%.16g", get_raw_value(dataset, idx, p));
              Tcl_AppendResult(interp, n, " ", NULL);
            }
          }
        } else if(argc > 4 && !strcmp(argv[2], "pos_at")) {
          /* xschem raw pos_at node value [dset] [from_start] [to_end] */
          int dset = 0;
          int from_start = -1;
          int to_end = -1;
          int pos = -1;
          double value = 0.0;
          if(argc > 5) {
            dset = atoi(argv[5]);
          }
          if(argc > 6) {
            from_start = atoi(argv[6]);
          }
          if(argc > 7) {
            to_end = atoi(argv[7]);
          }
          value = atof_spice(argv[4]);
          pos = raw_get_pos(argv[3], value, dset, from_start, to_end);
          Tcl_SetResult(interp, my_itoa(pos), TCL_VOLATILE);
        } else if(argc > 3 && !strcmp(argv[2], "add")) {
          int res = 0;
          int sweep_idx = 0;
          if(argc > 5) { /* provided sweep variable */
            sweep_idx =  get_raw_index(argv[5], NULL);
            if(sweep_idx <= 0) sweep_idx = 0;
          }
          if(argc > 4) {
            #if 0 /* seems not necessary... */
            int save_datasets = -1, save_npoints = -1;
            /* transform multiple OP points into a dc sweep */
            if(sch_waves_loaded()!= -1 && xctx->raw && xctx->raw->sim_type && !strcmp(xctx->raw->sim_type, "op")
               && xctx->raw->datasets > 1 && xctx->raw->npoints[0] == 1) {
              save_datasets = xctx->raw->datasets;
              xctx->raw->datasets = 1;
              save_npoints = xctx->raw->npoints[0];
              xctx->raw->npoints[0] = xctx->raw->allpoints;
            }
            #endif
            res = raw_add_vector(argv[3], argv[4], sweep_idx);

            #if 0
            if(sch_waves_loaded()!= -1 && save_npoints != -1) { /* restore multiple OP points */
              xctx->raw->datasets = save_datasets;
              xctx->raw->npoints[0] = save_npoints;
            }
            #endif
          } else {
            res = raw_add_vector(argv[3], NULL, 0);
          }
          Tcl_SetResult(interp, my_itoa(res), TCL_VOLATILE);
        } else if(argc > 2 && !strcmp(argv[2], "datasets")) {
          Tcl_SetResult(interp, my_itoa(raw->datasets), TCL_VOLATILE);
        } else if(argc > 2 && !strcmp(argv[2], "points")) {
          int dset = -1;
          if(argc > 3) dset = atoi(argv[3]);
          if(dset == -1) Tcl_SetResult(interp, my_itoa(raw->allpoints), TCL_VOLATILE);
          else {
            if(dset >= 0 && dset <  raw->datasets)
                Tcl_SetResult(interp, my_itoa(raw->npoints[dset]), TCL_VOLATILE);
          }
        } else if(argc > 2 && !strcmp(argv[2], "rawfile")) {
          Tcl_SetResult(interp, raw->rawfile, TCL_VOLATILE);
        } else if(argc > 2 && !strcmp(argv[2], "sim_type")) {
          Tcl_SetResult(interp, raw->sim_type, TCL_VOLATILE);
        } else if(argc > 2 && !strcmp(argv[2], "vars")) {
          Tcl_SetResult(interp, my_itoa(raw->nvars), TCL_VOLATILE);
        } else if(argc > 2 && !strcmp(argv[2], "list")) {
          for(i = 0 ; i < raw->nvars; ++i) {
            if(i > 0) Tcl_AppendResult(interp, "\n", NULL);
            Tcl_AppendResult(interp, raw->names[i], NULL);
          }
        /*    0      1        2   3   4   5       6
         *  xschem raw set node n value [dataset] */
        } else if(argc > 5 && !strcmp(argv[2], "set")) {
          int dataset = -1, ofs = 0;
          int point = atoi(argv[4]);
          const char *node = argv[3];
          int idx = -1;
          if(argc > 6) dataset = atoi(argv[6]);
          idx = get_raw_index(node, NULL);
          if(idx >= 0) {
            if( dataset < xctx->raw->datasets &&
                ( (dataset >=0 && point >= 0 && point < raw->npoints[dataset]) ||
                  (dataset == -1 && point >= 0 && point < raw->allpoints) )
              ) {
              if(dataset != -1) {
                for(i = 0; i < dataset; ++i) {
                  ofs += xctx->raw->npoints[i];
                }
                if(ofs + point < xctx->raw->allpoints) {
                  point += ofs;
                }
              }
              xctx->raw->values[idx][point] = (SPICE_DATA) atof(argv[5]);
              Tcl_SetResult(interp, dtoa(xctx->raw->values[idx][point]), TCL_VOLATILE);
            }
          }
        } else {
          err = 1;
        }
      } else {
        Tcl_SetResult(interp, "No raw file loaded", TCL_STATIC); return TCL_ERROR;
      }
      if(err) {Tcl_SetResult(interp, "Wrong command", TCL_STATIC); return TCL_ERROR;}
    }

    /* raw_clear
     *   Unload all simulation raw files
     *   You can use xschem raw clear as well.
     */
    else if(!strcmp(argv[1], "raw_clear"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      extra_rawfile(3, NULL, NULL, -1.0, -1.0); /* unload additional raw files */
      /* free_rawfile(&xctx->raw, 1, 0); */ /* unload base (current) raw file */
      draw();
      Tcl_ResetResult(interp);
    }

    /* raw_read [file] [sim] [sweep1 sweep2]
     *   If a raw file is already loaded delete from memory
     *   then load specified file and analysis 'sim' (dc, ac, tran, op, ...)
     *   If 'sim' not specified load first section found in raw file.
     *   if sweep1, sweep2 interval is given load only the interval
     *   sweep1 <= sweep_var < sweep2 */
    else if(!strcmp(argv[1], "raw_read"))
    {
      char f[PATH_MAX + 100];
      int res = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      /*
      * if(sch_waves_loaded() >= 0) {
      *   tcleval("array unset ngspice::ngspice_data");
      *   extra_rawfile(3, NULL, NULL, -1.0, -1.0);
      *   free_rawfile(&xctx->raw, 1, 0);
      * } else
      */
      if(argc > 2) {
        double sweep1 = -1.0, sweep2 = -1.0;
        tcleval("array unset ngspice::ngspice_data");
        extra_rawfile(3, NULL, NULL, -1.0, -1.0);
        /* free_rawfile(&xctx->raw, 0, 0); */
        my_snprintf(f, S(f),"regsub {^~/} {%s} {%s/}", argv[2], home_dir);
        tcleval(f);
        my_strncpy(f, tclresult(), S(f));
        if(argc > 5) {
          sweep1 = atof_spice(argv[4]);
          sweep2 = atof_spice(argv[5]);
        }
        if(argc > 3) res = raw_read(f, &xctx->raw, argv[3], 0, sweep1, sweep2);
        else res = raw_read(f, &xctx->raw, NULL, 0, -1.0, -1.0);
        if(sch_waves_loaded() >= 0) {
          draw();
        }
      }
      Tcl_SetResult(interp, my_itoa(res), TCL_VOLATILE);
    }

    /* raw_read_from_attr [sim]
     *   If a simulation raw file is already loaded delete from memory
     *   else read section 'sim' (tran, dc, ac, op, ...)
     *   of base64 encoded data from a 'spice_data'
     *   attribute of selected instance
     *   If sim not given read first section found */
    else if(!strcmp(argv[1], "raw_read_from_attr"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(sch_waves_loaded() >= 0) {
        extra_rawfile(3, NULL, NULL, -1.0, -1.0);
        /* free_rawfile(&xctx->raw, 1, 0); */
        draw();
      } else {
        extra_rawfile(3, NULL, NULL, -1.0, -1.0);
        /* free_rawfile(&xctx->raw, 0, 0); */
        if(argc > 2) raw_read_from_attr(&xctx->raw, argv[2], -1.0, -1.0);
        else  raw_read_from_attr(&xctx->raw, NULL, -1.0, -1.0);
        if(sch_waves_loaded() >= 0) {
          draw();
        }
      }
      Tcl_ResetResult(interp);
    }

    /* rebuild_connectivity
     *   Rebuild logical connectivity abstraction of schematic */
    else if(!strcmp(argv[1], "rebuild_connectivity"))
    {
      int err = 0;
      int n = 1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) n = atoi(argv[2]);
      xctx->prep_hash_inst=0;
      xctx->prep_hash_wires=0;
      xctx->prep_net_structs=0;
      xctx->prep_hi_structs=0;
      err |= prepare_netlist_structs(n);
      Tcl_SetResult(interp, my_itoa(err), TCL_VOLATILE);
    }

    /* rebuild_selection
         Rebuild selection list*/
    else if(!strcmp(argv[1], "rebuild_selection"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      rebuild_selected_array();
    }

    /* record_global_node n node
         call the record_global_node function (list of netlist global nodes) */
    else if(!strcmp(argv[1], "record_global_node"))
    {
      int ret = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        int n = atoi(argv[2]);
        if(n == 4 && argc > 3) ret = record_global_node(4,NULL, argv[3]); /* insert node */
        else if(n == 1 && argc > 3) ret = record_global_node(1,NULL, argv[3]); /* insert node */
        else if(n == 0) ret = record_global_node(0, stdout, NULL);
        else if(n == 2) ret = record_global_node(2, NULL, NULL);
        else if(n == 3 && argc > 3) ret = record_global_node(3, NULL, argv[3]); /* look up node */
      }
      Tcl_SetResult(interp, my_itoa(ret), TCL_VOLATILE);
    }

    /* rect ...
     *   rect [x1 y1 x2 y2] [pos] [propstring] [draw]
     *     if 'x1 y1 x2 y2'is given place recangle on current
     *     layer (rectcolor) at indicated coordinates.
     *     if 'pos' is given insert at given position in rectangle array.
     *     if 'pos' set to -1 append rectangle to last element in rectangle array.
     *     'propstring' is the attribute string. Set to empty if not given.
     *     if 'draw' is set to 1 (default) draw the new object, else don't
     *   rect
     *     If no coordinates are given start a GUI operation of rectangle placement
     *   rect gui
     *     if `gui` argument is given start a GUI placement of a rectangle with 1st
     *     point starting from current mouse coordinates */
    else if(!strcmp(argv[1], "rect"))
    {
      double x1,y1,x2,y2;
      int pos, save;
      int draw = 1;
      const char *prop_str = NULL;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 5) {
        x1=atof(argv[2]);
        y1=atof(argv[3]);
        x2=atof(argv[4]);
        y2=atof(argv[5]);
        RECTORDER(x1,y1,x2,y2);
        pos=-1;
        if(argc > 6) pos=atoi(argv[6]);
        if(argc > 7) prop_str = argv[7];
        if(argc > 8) draw = atoi(argv[8]);
        storeobject(pos, x1,y1,x2,y2,xRECT,xctx->rectcolor,0,prop_str);
        if(draw) {
          int c = xctx->rectcolor;
          int n = xctx->rects[c] - 1;
          int e_a = xctx->rect[c][n].ellipse_a;
          int e_b = xctx->rect[c][n].ellipse_b;
          save = xctx->draw_window; xctx->draw_window = 1;
          drawrect(xctx->rectcolor,NOW, x1,y1,x2,y2, 0, e_a, e_b);
          filledrect(xctx->rectcolor, NOW, x1, y1, x2, y2, 1, -1, -1);
          xctx->draw_window = save;
        }
        set_modify(1);
      } else if(argc > 2 && !strcmp(argv[2], "gui")) {
        int infix_interface = tclgetboolvar("infix_interface");
        if(infix_interface) {
          xctx->mx_double_save=xctx->mousex_snap;
          xctx->my_double_save=xctx->mousey_snap;
          xctx->last_command = 0;
          new_rect(PLACE,xctx->mousex_snap, xctx->mousey_snap);
        } else {
          xctx->ui_state |= MENUSTART;
          xctx->ui_state2 = MENUSTARTRECT;
        } 
      } else {
        xctx->ui_state |= MENUSTART;
        xctx->ui_state2 = MENUSTARTRECT;
      }
    }

    /* redo
     *   Redo last undone action */
    else if(!strcmp(argv[1], "redo"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      xctx->pop_undo(1, 1); /* 2nd param: set_modify_status */
      Tcl_ResetResult(interp);
    }

    /* redraw
     *   redraw window */
    else if(!strcmp(argv[1], "redraw"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      draw();
      Tcl_ResetResult(interp);
    }

    /* reload
     *   Forced (be careful!) Reload current schematic from disk */
    else if(!strcmp(argv[1], "reload"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      unselect_all(1);
      remove_symbols();
      load_schematic(1, xctx->sch[xctx->currsch], 1, 1);
      if(argc > 2 && !strcmp(argv[2], "zoom_full") ) {
        zoom_full(1, 0, 1 + 2 * tclgetboolvar("zoom_full_center"), 0.97);
      } else {
        draw();
      }
      Tcl_ResetResult(interp);
    }

    /* reload_symbols
     *   Reload all used symbols from disk */
    else if(!strcmp(argv[1], "reload_symbols"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      remove_symbols();
      link_symbols_to_instances(-1);
      xctx->prep_hi_structs=0;
      xctx->prep_net_structs=0;
      Tcl_ResetResult(interp);
    }

    /* remove_symbols
     *   Internal command: remove all symbol definitions */
    else if(!strcmp(argv[1], "remove_symbols"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      remove_symbols();
      Tcl_ResetResult(interp);
    }

    /* replace_symbol inst new_symbol [fast]
     *   Replace 'inst' symbol with 'new_symbol'
     *   If doing multiple substitutions set 'fast' to {}
     *    on first call and 'fast' on next calls
     *   for faster operation.
     *   do a 'xschem redraw' at end to update screen
     *   Example: xschem replace_symbol R3 capa.sym */
    else if(!strcmp(argv[1], "replace_symbol"))
    {
      int inst, fast = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 4) {
        argc = 4;
        if(!strcmp(argv[4], "fast")) {
          fast = 1;
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
        int sym_number, prefix;
        char *name=NULL;
        char *ptr=NULL;
        char *sym = NULL;
        my_strncpy(symbol, argv[3], S(symbol));
        if(!fast) {
          xctx->push_undo();
          xctx->prep_hash_inst=0;
          xctx->prep_net_structs=0;
          xctx->prep_hi_structs=0;
        }
        my_strdup(_ALLOC_ID_, &sym, tcl_hook2(symbol));
        sym_number=match_symbol(sym);
        my_free(_ALLOC_ID_, &sym);
        if(sym_number>=0)
        {
          prefix=(get_tok_value(xctx->sym[sym_number].templ , "name",0))[0]; /* get new symbol prefix  */
        }
        else prefix = 'x';
        delete_inst_node(inst); /* 20180208 fix crashing bug: delete node info if changing symbol */
                             /* if number of pins is different we must delete these data *before* */
                             /* changing ysmbol, otherwise i might end up deleting non allocated data. */
        my_strdup2(_ALLOC_ID_, &xctx->inst[inst].name, rel_sym_path(symbol));
        xctx->inst[inst].ptr=sym_number;
        my_strdup(_ALLOC_ID_, &name, xctx->inst[inst].instname);
        if(name && name[0] )
        {
          /* 20110325 only modify prefix if prefix not NUL */
          if(prefix) name[0]=(char)prefix; /* change prefix if changing symbol type; */
          my_strdup(_ALLOC_ID_, &ptr,subst_token(xctx->inst[inst].prop_ptr, "name", name) );
          if(!fast) hash_names(-1, XINSERT);
          hash_names(inst, XDELETE);
          new_prop_string(inst, ptr,           /* sets also inst[].instname */
             tclgetboolvar("disable_unique_names")); /* set new prop_ptr */
          hash_names(inst, XINSERT);
          set_inst_flags(&xctx->inst[inst]);
          my_free(_ALLOC_ID_, &ptr);
        }
        my_free(_ALLOC_ID_, &name);
        set_modify(1);
        /* draw(); */
        Tcl_SetResult(interp, xctx->inst[inst].instname , TCL_VOLATILE);
      }
    }

    /* reset_caches
     *   Reset cached instance and symbol cached flags (inst->flags, sym->flags) */
    else if(!strcmp(argv[1], "reset_caches"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      reset_caches();
      Tcl_ResetResult(interp);
    }

    /* reset_inst_prop inst
     *   Reset instance attribute string taking it from symbol template string */
    else if(!strcmp(argv[1], "reset_inst_prop"))
    {
      char *translated_sym = NULL;
      int sym_number = -1;
      char *subst = NULL;
      int inst;

      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc < 3) {
        Tcl_SetResult(interp, "xschem reset_inst_prop needs 1 more argument", TCL_STATIC);
        return TCL_ERROR;
      }
      if((inst = get_instance(argv[2])) < 0 ) {
        Tcl_SetResult(interp, "xschem reset_inst_prop: instance not found", TCL_STATIC);
        return TCL_ERROR;
      }
      symbol_bbox(inst, &xctx->inst[inst].x1, &xctx->inst[inst].y1, &xctx->inst[inst].x2, &xctx->inst[inst].y2);
      xctx->push_undo();
      xctx->prep_hash_inst=0;
      xctx->prep_net_structs=0;
      xctx->prep_hi_structs=0;

      hash_names(-1, XINSERT);
      hash_names(inst, XDELETE);
      set_inst_prop(inst);

      my_strdup2(_ALLOC_ID_, &translated_sym, translate(inst, xctx->inst[inst].name));
      sym_number=match_symbol(translated_sym);

      if(sym_number > 0) {
        delete_inst_node(inst);
        xctx->inst[inst].ptr=sym_number;
      }
      if(subst) my_free(_ALLOC_ID_, &subst);
      set_inst_flags(&xctx->inst[inst]);
      hash_names(inst, XINSERT);
      /* new symbol bbox after prop changes (may change due to text length) */
      symbol_bbox(inst, &xctx->inst[inst].x1, &xctx->inst[inst].y1, &xctx->inst[inst].x2, &xctx->inst[inst].y2);
      set_modify(-2); /* reset floaters caches */
      draw();
      my_free(_ALLOC_ID_, &translated_sym);
      Tcl_SetResult(interp, xctx->inst[inst].instname , TCL_VOLATILE);
    }

    /* reset_symbol inst symref
     *   This is a low level command, it merely changes the xctx->inst[...].name field.
     *   It is caller responsibility to delete all symbols before and do a reload_symbols
     *   afterward */
    else if(!strcmp(argv[1], "reset_symbol"))
    {
      int inst;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc!=4) {
        Tcl_SetResult(interp, "xschem reset_symbol needs 2 additional arguments", TCL_STATIC);
        return TCL_ERROR;
      }
      if((inst = get_instance(argv[2])) < 0 ) {
        Tcl_SetResult(interp, "xschem reset_symbol: instance not found", TCL_STATIC);
        return TCL_ERROR;
      } else {
        my_strdup(_ALLOC_ID_, &xctx->inst[inst].name, argv[3]);
      }
      Tcl_ResetResult(interp);
    }

    /* resetwin create_pixmap clear_pixmap force w h
     *   internal command: calls resetwin() */
    else if(!strcmp(argv[1], "resetwin"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 6) {
        resetwin(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
      }
      Tcl_ResetResult(interp);
    }

    /* resolved_net [net]
     *   if 'net' is given  return its topmost full hierarchy name
     *   else returns the topmost full hierarchy name of selected net/pin/label.
     *   Nets connected to I/O ports are mapped to upper level recursively */
    else if(!strcmp(argv[1], "resolved_net"))
    {
      const char *net = NULL;
      char  *rn = NULL;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      Tcl_ResetResult(interp);
      prepare_netlist_structs(0);
      if(argc > 2) {
        net = argv[2];
      } else if(xctx->lastsel == 1) {
        if(xctx->sel_array[0].type == ELEMENT) {
          int n=xctx->sel_array[0].n;
          if(xctx->inst[n].ptr >= 0) {
           const  char *type = xctx->sym[xctx->inst[n].ptr].type;
            if(IS_LABEL_SH_OR_PIN(type) && xctx->inst[n].node && xctx->inst[n].node[0]) {
              net = xctx->inst[n].node[0];
            }
          }
        } else if(xctx->sel_array[0].type == WIRE) {
          int n=xctx->sel_array[0].n;
          if(xctx->wire[n].node) {
            net = xctx->wire[n].node;
          }
        }
      }
      rn = resolved_net(net);
      Tcl_AppendResult(interp, rn, NULL);
      my_free(_ALLOC_ID_, &rn);
    }

    /* rotate [x0 y0]
     *   Rotate selection around point x0 y0.
     *   if x0, y0 not given use mouse coordinates */
    else if(!strcmp(argv[1], "rotate"))
    {
      double x0 = xctx->mousex_snap;
      double y0 = xctx->mousey_snap;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 3) {
        x0 = atof(argv[2]);
        y0 = atof(argv[3]);
      }

      if(xctx->ui_state & STARTMOVE) move_objects(ROTATE,0,0,0);
      else if(xctx->ui_state & STARTCOPY) copy_objects(ROTATE);
      else {
        rebuild_selected_array();
        xctx->mx_double_save = xctx->mousex_snap = x0;
        xctx->my_double_save = xctx->mousey_snap = y0;
        move_objects(START,0,0,0);
        move_objects(ROTATE,0,0,0);
        move_objects(END,0,0,0);
      }
      Tcl_ResetResult(interp);
    }

    /* rotate_in_place
     *   Rotate selected objects around their 0,0 coordinate point */
    else if(!strcmp(argv[1], "rotate_in_place"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(xctx->ui_state & STARTMOVE) move_objects(FLIP|ROTATELOCAL,0,0,0);
      else if(xctx->ui_state & STARTCOPY) copy_objects(FLIP|ROTATELOCAL);
      else {
        rebuild_selected_array();
        move_objects(START,0,0,0);
        move_objects(ROTATE|ROTATELOCAL,0,0,0);
        move_objects(END,0,0,0);
      }
      Tcl_ResetResult(interp);
    }

    /* round_to_n_digits i n
     *   round number 'i' to 'n' digits */
    else if(!strcmp(argv[1], "round_to_n_digits"))
    {
      double r;
      if(argc > 3) {
        r = round_to_n_digits(atof(argv[2]), atoi(argv[3]));
        Tcl_SetResult(interp, dtoa(r), TCL_VOLATILE);
      }
    }

    else { cmd_found = 0;}
    break;
    case 's': /*----------------------------------------------*/
    /* save [fast]
     *   Save schematic if modified. Does not ask confirmation!
     *   if 'fast' is given it is passed to save_schematic() to avoid
     *   updating window/tab/sim button states */
    if(!strcmp(argv[1], "save"))
    {
      int fast = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      dbg(1, "scheduler(): saving: current schematic\n");
      for(i = 2; i < argc; i++) {
        if(!strcmp(argv[i], "fast")) fast |= 1;
      }

      if(!strcmp(xctx->sch[xctx->currsch], "")) {   /* check if unnamed schematic, use saveas in this case... */
        saveas(NULL, SCHEMATIC);
      } else {
        save(0, fast);
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
      const char *fptr;
      char f[PATH_MAX + 100];
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}

      if(argc > 2) {
        my_snprintf(f, S(f),"regsub {^~/} {%s} {%s/}", argv[2], home_dir);
        tcleval(f);
        my_strncpy(f, tclresult(), S(f));
      }
      if(argc > 3) {
        fptr = !strcmp(f, "") ? NULL : f;
        if(!strcmp(argv[3], "schematic")) saveas(fptr, SCHEMATIC);
        else if(!strcmp(argv[3], "symbol")) saveas(fptr, SYMBOL);
        else saveas(fptr, SCHEMATIC);
      }
      else if(argc > 2) {
        fptr = !strcmp(f, "") ? NULL : f;
        saveas(fptr, SCHEMATIC);
      }
      else saveas(NULL, SCHEMATIC);
    }

    /* sch_pinlist
     *   List a 2-item list of all pins  and directions of current schematic
     *   Example: xschem sch_pinlist
     *   -->  {PLUS} {in} {OUT} {out} {MINUS} {in} {VCC} {inout} {VSS} {inout}
     */
    else if(!strcmp(argv[1], "sch_pinlist"))
    {
      int i, first = 1;
      char *dir = NULL;
      const char *lab;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      for(i = 0; i < xctx->instances; ++i) {
        if( !strcmp((xctx->inst[i].ptr + xctx->sym)->type, "ipin") ) dir="in";
        else if( !strcmp((xctx->inst[i].ptr + xctx->sym)->type, "opin") ) dir="out";
        else if( !strcmp((xctx->inst[i].ptr + xctx->sym)->type, "iopin") ) dir="inout";
        else dir = NULL;
        if(dir) {
          lab = xctx->inst[i].lab;
          if(first == 0) Tcl_AppendResult(interp, " ", NULL);
          Tcl_AppendResult(interp, "{", lab, "} {", dir, "}", NULL);
          first = 0;

        }
      }
    }

    /* schematic_in_new_window [new_process] [nodraw] [force]
     *   When a symbol is selected edit corresponding schematic
     *   in a new tab/window if not already open.
     *   If nothing selected open another window of the second
     *   schematic (issues a warning).
     *   if 'new_process' is given start a new xschem process
     *   if 'nodraw' is given do not draw loaded schematic
     *   returns '1' if a new schematic was opened, 0 otherwise */
    else if(!strcmp(argv[1], "schematic_in_new_window"))
    {
      int res = 0;
      int new_process = 0;
      int nodraw = 0;
      int force = 0;
      int i;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      for(i = 2; i < argc; i++) {
        if(!strcmp(argv[i], "new_process")) new_process = 1;
        if(!strcmp(argv[i], "nodraw")) nodraw = 1;
        if(!strcmp(argv[i], "force")) force = 1;
      }
      res = schematic_in_new_window(new_process, !nodraw, force);
      Tcl_SetResult(interp, my_itoa(res), TCL_VOLATILE);
    }

    /* search regex|exact select tok val [match_case]
     *   Search instances / wires / rects / texts with attribute string containing 'tok'
     *   and value 'val'
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
     *    match_case:
     *      1 : Match case
     *      0 : Do not match case
     *      If not given assume 1 (Match case)
     */
    else if(!strcmp(argv[1], "search") || !strcmp(argv[1], "searchmenu"))
    {
      /*   0      1         2        3       4   5        6      */
      /*                           select                        */
      /* xschem search regex|exact 0|1|-1   tok val [match_case] */
      int select, r;
      int match_case = 1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 6 && argv[6][0] == '0') match_case = 0;
      if(argc < 6) {
        Tcl_SetResult(interp, "xschem search requires 4 or 5 additional fields.", TCL_STATIC);
        return TCL_ERROR;
      }
      if(argc > 5) {
        select = atoi(argv[3]);
        if(!strcmp(argv[2], "regex") )  r = search(argv[4],argv[5],0,select, match_case);
        else  r = search(argv[4],argv[5],1,select, match_case);
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

    /* select instance|wire|text id [clear] [fast]
     * select rect|line|poly|arc layer id [clear] [fast]
     * Select indicated instance or wire or text, or
     * Select indicated (layer, number) rectangle, line, polygon, arc.
     * For 'instance' 'id' can be the instance name or number
     * for all other objects 'id' is the position in the respective arrays
     * if 'clear' is specified does an unselect operation
     * if 'fast' is specified avoid sending information to infowindow and status bar
     * if 'nodraw' is given on select instance do not draw selection
     * returns 1 if something selected, 0 otherwise */
    else if(!strcmp(argv[1], "select"))
    {
      short unsigned int  sel = SELECTED;
      int fast = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc < 3) {
        Tcl_SetResult(interp, "xschem select: missing arguments.", TCL_STATIC);
        return TCL_ERROR;
      } else if(argc < 5 && (!strcmp(argv[2], "rect") || !strcmp(argv[2], "line") ||
                     !strcmp(argv[2], "poly") || !strcmp(argv[2], "arc"))) {
        Tcl_SetResult(interp, "xschem select: missing arguments.", TCL_STATIC);
        return TCL_ERROR;
      } else if(argc < 4) {
        Tcl_SetResult(interp, "xschem select: missing arguments.", TCL_STATIC);
        return TCL_ERROR;
      }
      if(argc > 4) {
       int i;
       for(i = 4; i < argc; i++) {
         if(!strcmp(argv[i], "clear")) sel = 0;
         if(!strcmp(argv[i], "fast")) fast |= 1;
         if(!strcmp(argv[i], "nodraw") && !strcmp(argv[2], "instance")) fast |= 2;
       }
      }
      if(!strcmp(argv[2], "instance") && argc > 3) {
        int n;
        /* find by instance name  or number*/
        n = get_instance(argv[3]);
        if(n >= 0) {
           select_element(n, sel, fast, 1);
           xctx->ui_state |= SELECTION;
        }
        Tcl_SetResult(interp, (n >= 0) ? "1" : "0" , TCL_STATIC);
      }
      else if(!strcmp(argv[2], "wire") && argc > 3) {
        int n=atoi(argv[3]);
        int valid = n < xctx->wires && n >= 0;
        if(valid) {
          select_wire(n, sel, fast, 1);
          xctx->ui_state |= SELECTION;
        }
        Tcl_SetResult(interp, valid ? "1" : "0" , TCL_STATIC);
      }
      else if(!strcmp(argv[2], "line") && argc > 4) {
        int c=atoi(argv[3]);
        int n=atoi(argv[4]);
        int valid = n < xctx->lines[c] && n >= 0 && c < cadlayers && c >= 0;
        if(valid) {
          select_line(c, n, sel, fast, 0);
          xctx->ui_state |= SELECTION;
        }
        Tcl_SetResult(interp, valid ? "1" : "0" , TCL_STATIC);
      }
      else if(!strcmp(argv[2], "rect") && argc > 4) {
        int c=atoi(argv[3]);
        int n=atoi(argv[4]);
        int valid = n < xctx->rects[c] && n >= 0 && c < cadlayers && c >= 0;
        if(valid) {
          select_box(c, n, sel, fast, 0);
          xctx->ui_state |= SELECTION;
        }
        Tcl_SetResult(interp, valid ? "1" : "0" , TCL_STATIC);
      }
      else if(!strcmp(argv[2], "arc") && argc > 4) {
        int c=atoi(argv[3]);
        int n=atoi(argv[4]);
        int valid = n < xctx->arcs[c] && n >= 0 && c < cadlayers && c >= 0;
        if(valid) {
          select_arc(c, n, sel, fast, 0);
          xctx->ui_state |= SELECTION;
        }
        Tcl_SetResult(interp, valid ? "1" : "0" , TCL_STATIC);
      }
      else if(!strcmp(argv[2], "poly") && argc > 4) {
        int c=atoi(argv[3]);
        int n=atoi(argv[4]);
        int valid = n < xctx->polygons[c] && n >= 0 && c < cadlayers && c >= 0;
        if(valid) {
          select_polygon(c, n, sel, fast, 0);
          xctx->ui_state |= SELECTION;
        }
        Tcl_SetResult(interp, valid ? "1" : "0" , TCL_STATIC);
      }
      else if(!strcmp(argv[2], "text") && argc > 3) {
        int n=atoi(argv[3]);
        int valid = n < xctx->texts && n >= 0;
        if(valid) {
          select_text(n, sel, fast, 0);
          xctx->ui_state |= SELECTION;
        }
        Tcl_SetResult(interp, valid ? "1" : "0" , TCL_STATIC);
      }
      drawtemparc(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
      drawtemprect(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
      drawtempline(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
    }

    /* select_all
     *   Selects all objects in schematic */
    else if(!strcmp(argv[1], "select_all"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      select_all();
      Tcl_ResetResult(interp);
    }

    /* select_dangling_nets
     *   Select all nets/labels that are dangling, ie not attached to any non pin/port/probe components
     *   Returns number of selected items (wires,labels) if danglings found, 0 otherwise */
    else if(!strcmp(argv[1], "select_dangling_nets"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      select_dangling_nets();
      Tcl_SetResult(interp, my_itoa(xctx->lastsel), TCL_VOLATILE);
    }

    /* select_hilight_net
     *   Select all highlight objects (wires, labels, pins, instances) */
    else if(!strcmp(argv[1], "select_hilight_net"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      select_hilight_net();
      Tcl_ResetResult(interp);
    }

    /* select_inside x1 y1 x2 y2 [sel]
     *   Select all objects inside the indicated area
         if [sel] is set to '0' do an unselect operation */
    else if(!strcmp(argv[1], "select_inside"))
    {
      int sel = SELECTED;
      double x1, y1, x2, y2;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 6 && argv[6][0] == '0') sel = 0;
      x1 = atof(argv[2]);
      y1 = atof(argv[3]);
      x2 = atof(argv[4]);
      y2 = atof(argv[5]);
      select_inside(tclgetboolvar("enable_stretch"), x1, y1, x2, y2, sel);
      Tcl_ResetResult(interp);
    }

    /* selected_set [what]
     *   Return a list of selected instance names
     *   If what is not given or set to 'inst' return list of selected instance names
     *   If what set to 'rect' return list of selected rectangles with their coordinates
     *   If what set to 'text' return list of selected texts with their coordinates */
    else if(!strcmp(argv[1], "selected_set"))
    {
      int n, i, first = 1;
      int what = ELEMENT;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}

      if(argc > 2) {
        if(!strcmp(argv[2], "rect")) what = xRECT;
        else if(!strcmp(argv[2], "text")) what = xTEXT;
      }
      rebuild_selected_array();
      for(n=0; n < xctx->lastsel; ++n) {
        if(what == xRECT &&  xctx->sel_array[n].type == xRECT) {
          char col[30], num[30], coord[200];
          int c = xctx->sel_array[n].col;
          i = xctx->sel_array[n].n;
          my_strncpy(col, my_itoa(c), S(col));
          my_strncpy(num, my_itoa(i), S(num));
          my_snprintf(coord, S(coord), "%g %g %g %g",
            xctx->rect[c][i].x1, xctx->rect[c][i].y1, xctx->rect[c][i].x2, xctx->rect[c][i].y2);
          if(first == 0) Tcl_AppendResult(interp, "\n", NULL);
          first = 0;
          Tcl_AppendResult(interp,col, " ", num, " ", coord , NULL);
        } else if(what == xTEXT &&  xctx->sel_array[n].type == xTEXT) {
          char num[30], coord[200];
          i = xctx->sel_array[n].n;
          my_strncpy(num, my_itoa(i), S(num));
          my_snprintf(coord, S(coord), "%g %g %d %d",
          xctx->text[i].x0, xctx->text[i].y0, xctx->text[i].rot, xctx->text[i].flip);
          if(first == 0) Tcl_AppendResult(interp, "\n", NULL);
          first = 0;
          Tcl_AppendResult(interp, num, " ", coord , " {", xctx->text[i].txt_ptr, "}", NULL);
        } else if(what == ELEMENT && xctx->sel_array[n].type == ELEMENT) {
          i = xctx->sel_array[n].n;
          if(first == 0)  Tcl_AppendResult(interp, " ", NULL);
          Tcl_AppendResult(interp, "{", xctx->inst[i].instname, "}", NULL);
          first = 0;
        }
      }
    }

    /* selected_wire
     *  Return list of selected nets */
    else if(!strcmp(argv[1], "selected_wire"))
    {
      int n, i, first = 1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      rebuild_selected_array();
      for(n=0; n < xctx->lastsel; ++n) {
        if(xctx->sel_array[n].type == WIRE) {
          i = xctx->sel_array[n].n;
          if(first == 0)  Tcl_AppendResult(interp, " ", NULL);
          Tcl_AppendResult(interp, "{", get_tok_value(xctx->wire[i].prop_ptr, "lab",0), "}", NULL);
          first = 0;
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            set_grid( atof(argv[3]) );
          }
          else if(!strcmp(argv[2], "cadsnap")) { /* set mouse snap (default: 10) */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            set_snap( atof(argv[3]) );
            change_linewidth(-1.);
            draw();
          }
          else if(!strcmp(argv[2], "color_ps")) { /* set color psoscript (1 or 0) */
            color_ps=atoi(argv[3]);
          }
          else if(!strcmp(argv[2], "crosshair_layer")) { /* set layer for mouse crosshair */
             int c = atoi(argv[3]);
             tclsetintvar("crosshair_layer", c);
             xctx->crosshair_layer = c;
             if(xctx->crosshair_layer < 0 ) xctx->crosshair_layer = 2;
             if(xctx->crosshair_layer >= cadlayers ) xctx->crosshair_layer = 2;
          }
          else if(!strcmp(argv[2], "constr_mv")) { /* set constrained move (1=horiz, 2=vert, 0=none) */
            xctx->constr_mv = atoi(argv[3]);
            if(xctx->constr_mv < 0 || xctx->constr_mv > 2) xctx->constr_mv = 0;
          }
          else if(!strcmp(argv[2], "cursor1_x")) { /* set graph cursor1 position */
            xctx->graph_cursor1_x = atof_spice(argv[3]);

            #if 0
            if(xctx->rects[GRIDLAYER] > 0) {
              Graph_ctx *gr = &xctx->graph_struct;
              xRect *r = &xctx->rect[GRIDLAYER][0];
              if(r->flags & 1) {
                backannotate_at_cursor_b_pos(r, gr);
              }
            }
            #endif
          }
          else if(!strcmp(argv[2], "cursor2_x")) { /* set graph cursor2 position */
            int floaters = there_are_floaters();
            xctx->graph_cursor2_x = atof_spice(argv[3]);

            if(xctx->rects[GRIDLAYER] > 0) {
              Graph_ctx *gr = &xctx->graph_struct;
              xRect *r = &xctx->rect[GRIDLAYER][0];
              if(r->flags & 1) {
                if(xctx->graph_flags & 4) {
                  backannotate_at_cursor_b_pos(r, gr);
                  if(floaters) set_modify(-2); /* update floater caches to reflect actual backannotation */
                }
              }
            }
          }
          else if(!strcmp(argv[2], "draw_window")) { /* set drawing to window (1 or 0) */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            xctx->draw_window=atoi(argv[3]);
          }
          else if(!strcmp(argv[2], "fix_broken_tiled_fill")) { /* alternate drawing method for broken GPUs */
            fix_broken_tiled_fill = atoi(argv[3]);
          }
          else if(!strcmp(argv[2], "fix_mouse_coord")) { /* fix for wrong mouse coords in RDP software */
            fix_mouse_coord = atoi(argv[3]);
          }
          else if(!strcmp(argv[2], "format")) { /* set name of custom format attribute used for netlisting */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_strdup(_ALLOC_ID_, &xctx->custom_format, argv[3]);
          }
          else if(!strcmp(argv[2], "header_text")) { /* set header metadata (used for license info) */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            if(!xctx->header_text || strcmp(xctx->header_text, argv[3])) {
              set_modify(1); xctx->push_undo();
              my_strdup2(_ALLOC_ID_, &xctx->header_text, argv[3]);
            }
          }
          else if(!strcmp(argv[2], "hide_symbols")) { /* set to 0,1,2 for various hiding level of symbols */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            xctx->hide_symbols=atoi(argv[3]);
          }
          else if(!strcmp(argv[2], "hilight_color")) { /* set hilight color for next hilight */
            int c = atoi(argv[3]);
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            if(c >= cadlayers) c = 4;
            xctx->hilight_color= c;
          }

          else if(!strcmp(argv[2], "infowindow_text")) { /* ERC messages */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_strdup(_ALLOC_ID_, &xctx->infowindow_text, argv[3]);
          }
          else if(!strcmp(argv[2], "intuitive_interface")) { /* ERC messages */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            xctx->intuitive_interface = atoi(argv[3]);
          }

        } else { /* argv[2][0] >= 'n' */
          if(!strcmp(argv[2], "netlist_name")) { /* set custom netlist name */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_strncpy(xctx->netlist_name, argv[3], S(xctx->netlist_name));
          }
          else if(!strcmp(argv[2], "netlist_type")) /* set netlisting mode (spice, verilog, vhdl, tedax, symbol) */
          {
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            if(!strcmp(argv[3], "spice")){
              xctx->netlist_type=CAD_SPICE_NETLIST;
            }
            else if(!strcmp(argv[3], "vhdl")) {
              xctx->netlist_type=CAD_VHDL_NETLIST;
            }
            else if(!strcmp(argv[3], "spectre")) {
              xctx->netlist_type=CAD_SPECTRE_NETLIST;
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
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            xctx->no_draw=s;
          }
          else if(!strcmp(argv[2], "no_undo")) { /* set to 1 to disable undo */
            int s = atoi(argv[3]);
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            xctx->no_undo=s;
          }
          else if(!strcmp(argv[2], "raw_level")) { /* set hierarchy level loaded raw file refers to */
            int n = atoi(argv[3]);
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            if(n >= 0 && n <= xctx->currsch) {
              xctx->raw->level = atoi(argv[3]);
              my_strdup2(_ALLOC_ID_, &xctx->raw->schname, xctx->sch[xctx->raw->level]);
              Tcl_SetResult(interp, my_itoa(n), TCL_VOLATILE);
            } else {
              Tcl_SetResult(interp, "-1", TCL_VOLATILE);
            }
          }
          else if(!strcmp(argv[2], "rectcolor")) { /* set current layer (0, 1, .... , cadlayers-1) */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            xctx->rectcolor=atoi(argv[3]);
            if(xctx->rectcolor < 0 ) xctx->rectcolor = 0;
            if(xctx->rectcolor >= cadlayers ) xctx->rectcolor = cadlayers - 1;
            rebuild_selected_array();
            if(xctx->lastsel) {
              change_layer();
            }
          }
          else if(!strcmp(argv[2], "sch_to_compare")) { /* set name of schematic to compare current window with */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_strncpy(xctx->sch_to_compare, abs_sym_path(argv[3], ""), S(xctx->sch_to_compare));
          }
          else if(!strcmp(argv[2], "schsymbolprop")) { /* set global symbol attribute string */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_strdup(_ALLOC_ID_, &xctx->schsymbolprop, argv[3]);
          }
          else if(!strcmp(argv[2], "schprop")) { /* set schematic global spice attribute string */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_strdup(_ALLOC_ID_, &xctx->schprop, argv[3]);
          }
          else if(!strcmp(argv[2], "schverilogprop")) { /* set schematic global verilog attribute string */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_strdup(_ALLOC_ID_, &xctx->schverilogprop, argv[3]);
          }
          else if(!strcmp(argv[2], "schspectreprop")) { /* set schematic global spectre attribute string */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_strdup(_ALLOC_ID_, &xctx->schspectreprop, argv[3]);
          } 
          else if(!strcmp(argv[2], "schvhdlprop")) { /* set schematic global vhdl attribute string */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_strdup(_ALLOC_ID_, &xctx->schvhdlprop, argv[3]);
          }
          else if(!strcmp(argv[2], "schtedaxprop")) { /* set schematic global tedax attribute string */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            my_strdup(_ALLOC_ID_, &xctx->schtedaxprop, argv[3]);
          }
          else if(!strcmp(argv[2], "text_svg")) { /* set to 1 to use svg <text> elements */
            text_svg=atoi(argv[3]);
          }
          else if(!strcmp(argv[2], "semaphore")) { /* debug */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            dbg(1, "scheduler(): set semaphore to %s\n", argv[3]);
            xctx->semaphore=atoi(argv[3]);
          }
          else if(!strcmp(argv[2], "show_hidden_texts")) { /* set to 1 to enable showing texts with attr hide=true */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
            dbg(1, "scheduler(): set show_hidden_texts to %s\n", argv[3]);
            xctx->show_hidden_texts=atoi(argv[3]);
          }
          else if(!strcmp(argv[2], "sym_txt")) { /* set to 0 to hide symbol texts */
            if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc < 5) {Tcl_SetResult(interp, "Missing arguments", TCL_STATIC);return TCL_ERROR;}
      my_strdup(_ALLOC_ID_, &s, argv[2]);
      set_different_token(&s, argv[3], argv[4]);
      Tcl_SetResult(interp, s, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &s);
    }

    /* set_modify [n]
     *   Force modify status on current schematic
     *   integer 'n':
     *   0 : clear modified flag, update title and tab names, upd. simulation button colors.
     *   1 : set modified flag, update title and tab names, upd. simulation button colors, rst floater caches.
     *   2 : clear modified flag, do nothing else.
     *   3 : set modified flag, do nothing else.
     *  -1 : set title, rst floater caches.
     *  -2 : rst floater caches, update simulation button colors (Simulate, Waves, Netlist).
     */
    else if(!strcmp(argv[1], "set_modify"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        set_modify(atoi(argv[2]));
      }
      Tcl_ResetResult(interp);
    }
    /* setprop [-fast|-fastundo] instance|symbol|text|rect ref tok [val]
     *
     * setprop [-fast] instance inst [tok] [val]
     *   set attribute 'tok' of instance (name or number) 'inst' to value 'val'
     *   If 'tok' set to 'allprops' replace whole instance prop_str with 'val'
     *   If 'val' not given (no attribute value) delete attribute from instance
     *   If 'tok' not given clear completely instance attribute string
     *   If '-fast' argument if given does not redraw and is not undoable
     *
     * setprop symbol name tok [val]
     *   Set attribute 'tok' of symbol name 'name' to 'val'
     *   If 'val' not given (no attribute value) delete attribute from symbol
     *   This command is not very useful since changes are not saved into symbol
     *   and netlisters reload symbols, so changes are lost anyway.
     *
     * setprop rect [-fast|-fastundo] lay n tok [val]
     *   Set attribute 'tok' of rectangle number'n' on layer 'lay'
     *   If 'val' not given (no attribute value) delete attribute from rect
     *   If '-fast' argument is given does not redraw and is not undoable
     *   If '-fastundo' s given same as above but action is undoable.
     *
     * setprop rect 2 n fullxzoom
     * setprop rect 2 n fullyzoom
     *   These commands do full x/y zoom of graph 'n' (on layer 2, this is hardcoded).
     *
     * setprop [-fast|-fastundo] text n [tok] [val]
     *   Set attribute 'tok' of text number 'n'
     *   If 'tok' not specified set text string (txt_ptr) to value
     *   If "txt_ptr" is given as token replace the text txt_ptr ("the text")
     *   If 'val' not given (no attribute value) delete attribute from text
     *   If '-fast' argument is given does not redraw and is not undoable
     *   If '-fastundo' is given same as above but action is undoable.
     */
    else if(!strcmp(argv[1], "setprop"))
    {
      int i, fast = 0, shift = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}

      for(i = 2; i < argc; i++) {
        if(argv[i][0] == '-') {
          if(!strcmp(argv[i], "-fast")) {
            fast = 1; shift++;
          } else if(!strcmp(argv[i], "-fastundo")) {
            fast = 3; shift++;
          }
        } else {
          break;
        }
      }
      /* remove option (-xxx) arguments and shift remaining */
      if(shift) for(; i < argc; i++) {
        argv[i - shift] = argv[i];
      }
      argc -= shift;


    /*   0       1       2     3    4     5  
     * xschem setprop instance R4 value [30k]  */
      if(argc > 2 && !strcmp(argv[2], "instance")) {
        int inst;

        if(argc < 4) {
          Tcl_SetResult(interp, "xschem setprop instance needs 1 or more additional arguments", TCL_STATIC);
          return TCL_ERROR;
        }
        if((inst = get_instance(argv[3])) < 0 ) {
          Tcl_SetResult(interp, "xschem setprop: instance not found", TCL_STATIC);
          return TCL_ERROR;
        } else {
          char *translated_sym = NULL;
          int sym_number = -1;
          char *subst = NULL, *old_name = NULL;;
   
          if(!fast) {
            symbol_bbox(inst, &xctx->inst[inst].x1, &xctx->inst[inst].y1, &xctx->inst[inst].x2, &xctx->inst[inst].y2);
            xctx->push_undo();
          }
          xctx->prep_hash_inst=0;
          xctx->prep_net_structs=0;
          xctx->prep_hi_structs=0;
          if(argc > 4 && !strcmp(argv[4], "name")) {
            if(fast == 0) {
              hash_names(-1, XINSERT);
            }
            my_strdup2(_ALLOC_ID_, &old_name, xctx->inst[inst].instname);
          }
          if(argc > 5) {
            if(!strcmp(argv[4], "allprops")) {
              hash_names(-1, XINSERT);
              my_strdup2(_ALLOC_ID_, &subst, argv[5]);
            } else {
              my_strdup2(_ALLOC_ID_, &subst, subst_token(xctx->inst[inst].prop_ptr, argv[4], argv[5]));
            }
          } else if(argc > 4) {/* assume argc == 5 , delete attribute */
            my_strdup2(_ALLOC_ID_, &subst, subst_token(xctx->inst[inst].prop_ptr, argv[4], NULL));
          } else if(argc > 3) {
            /* clear all instance prop_str */
            my_free(_ALLOC_ID_, &subst);

          }
          hash_names(inst, XDELETE);
          new_prop_string(inst, subst, tclgetboolvar("disable_unique_names"));
          if(old_name) {
            update_attached_floaters(old_name, inst, 0);
          }
          my_strdup2(_ALLOC_ID_, &translated_sym, translate(inst, xctx->inst[inst].name));
          sym_number=match_symbol(translated_sym);

          if(sym_number > 0) {
            delete_inst_node(inst);
            xctx->inst[inst].ptr=sym_number;
          }
          if(subst) my_free(_ALLOC_ID_, &subst);
          if(old_name) my_free(_ALLOC_ID_, &old_name);
          set_inst_flags(&xctx->inst[inst]);
          hash_names(inst, XINSERT);
          if(!fast) {
            /* new symbol bbox after prop changes (may change due to text length) */
            symbol_bbox(inst, &xctx->inst[inst].x1, &xctx->inst[inst].y1, &xctx->inst[inst].x2, &xctx->inst[inst].y2);
            set_modify(-2); /* reset floaters caches */
            draw();
          }
          my_free(_ALLOC_ID_, &translated_sym);
          Tcl_SetResult(interp, xctx->inst[inst].instname , TCL_VOLATILE);
        }
      } else if(argc > 2 && !strcmp(argv[2], "symbol")) {
      /*  0       1       2      3    4     5
       * xschem setprop symbol name token [value] */

        int i;
        xSymbol *sym;
        if(argc < 4) {
          Tcl_SetResult(interp, "xschem setprop symbol needs 1 or 2 or 3 additional arguments", TCL_STATIC);
          return TCL_ERROR;
        }
        i = get_symbol(argv[3]);
        if(i == -1) {
          Tcl_SetResult(interp, "Symbol not found", TCL_STATIC);
          return TCL_ERROR;
        }
        sym = &xctx->sym[i];
        if(argc > 5)
          my_strdup2(_ALLOC_ID_, &sym->prop_ptr, subst_token(sym->prop_ptr, argv[4], argv[5]));
        else
          my_strdup2(_ALLOC_ID_, &sym->prop_ptr, subst_token(sym->prop_ptr, argv[4], NULL)); /* delete attr */

      } else if(argc > 5 && !strcmp(argv[2], "rect")) {
      /*  0       1      2   3 4   5    6     
       * xschem setprop rect c n token [value] */
        int change_done = 0;
        xRect *r;
        int c = atoi(argv[3]);
        int n = atoi(argv[4]);
        if(!(c>=0 && c < cadlayers && n >=0 && n < xctx->rects[c]) ) {
          Tcl_SetResult(interp, "xschem setprop rect: wrong layer or rect number", TCL_STATIC);
          return TCL_ERROR;
        }
        r = &xctx->rect[c][n];
        if(!fast) {
          bbox(START,0.0,0.0,0.0,0.0);
        }
        if(argc > 5 && c == 2 && !strcmp(argv[5], "fullxzoom")) {
          Graph_ctx *gr = &xctx->graph_struct;
          int dataset;
          setup_graph_data(n, 0, gr);
          if(xctx->raw && gr->dataset >= 0 && gr->dataset < xctx->raw->datasets) dataset = gr->dataset;
          else dataset = -1;
          graph_fullxzoom(n, gr, dataset);
        }
        if(argc > 5 && c == 2 && !strcmp(argv[5], "fullyzoom")) {
          xRect *r = &xctx->rect[c][n];
          Graph_ctx *gr = &xctx->graph_struct;
          int dataset;
          setup_graph_data(n, 0, gr);
          if(xctx->raw && gr->dataset >= 0 && gr->dataset < xctx->raw->datasets) dataset = gr->dataset;
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
        set_rect_flags(r); /* set cached .flags bitmask from attributes */
        if(argc > 5 && !strcmp(argv[5], "fill")) {
          const char *attr = get_tok_value(r->prop_ptr,"fill", 0);
          if(!strcmp(attr, "full")) xctx->rect[c][n].fill = 2;
          else if(!strboolcmp(attr, "false")) xctx->rect[c][n].fill = 0;
          else xctx->rect[c][n].fill = 1;
        }
        set_rect_extraptr(0, &xctx->rect[c][n]);
        if(!fast) {
          bbox(ADD, r->x1, r->y1, r->x2, r->y2);
          /* redraw rect with new props */
          bbox(SET,0.0,0.0,0.0,0.0);
          draw();
          bbox(END,0.0,0.0,0.0,0.0);
        }
        Tcl_ResetResult(interp);

      } else if(argc > 3 && !strcmp(argv[2], "text")) {
      /*  0       1      2   3   4      5      6
       * xschem setprop text n [token] value [fast|fastundo]
       * if "txt_ptr" is given as token replace the text txt_ptr ("the text") */
        int change_done = 0;
        int tmp;
        double xx1, xx2, yy1, yy2, dtmp;
        xText *t;
        int n = atoi(argv[3]);
        if(!(n >=0 && n < xctx->texts) ) {
          Tcl_SetResult(interp, "xschem setprop text: wrong text number", TCL_STATIC);
          return TCL_ERROR;
        }
        t = &xctx->text[n];

        if(!fast) {
          bbox(START,0.0,0.0,0.0,0.0);
        }
        if(argc > 5) {
         char *estr = NULL;
         if(!fast) {
            estr = my_expand(get_text_floater(n), tclgetintvar("tabstop"));
            text_bbox(estr, t->xscale,
                  t->yscale, t->rot, t->flip, t->hcenter,
                  t->vcenter, t->x0, t->y0,
                  &xx1,&yy1,&xx2,&yy2, &tmp, &dtmp);
            my_free(_ALLOC_ID_, &estr);
            bbox(ADD, xx1, yy1, xx2, yy2);
          }
          /* verify if there is some difference */
          if(!strcmp(argv[4], "txt_ptr")) {
            if(strcmp(argv[5], t->txt_ptr)) {
              change_done = 1;
              if(fast == 3 || fast == 0) xctx->push_undo();
              my_strdup2(_ALLOC_ID_, &t->txt_ptr, argv[5]);
            }
          } else if(strcmp(argv[5], get_tok_value(t->prop_ptr, argv[4], 0))) {
            change_done = 1;
            if(fast == 3 || fast == 0) xctx->push_undo();
            my_strdup2(_ALLOC_ID_, &t->prop_ptr, subst_token(t->prop_ptr, argv[4], argv[5]));
          }
        } else if(argc > 4) {
          get_tok_value(t->prop_ptr, argv[4], 0);
          if(xctx->tok_size) {
            change_done = 1;
            if(fast == 3 || fast == 0) xctx->push_undo();
            my_strdup2(_ALLOC_ID_, &t->prop_ptr, subst_token(t->prop_ptr, argv[4], NULL)); /* delete attr */
          }
        }
        if(change_done) {
          char *estr = NULL;
          set_modify(1);
          set_text_flags(t);
          estr = my_expand(get_text_floater(n), tclgetintvar("tabstop"));
          text_bbox(estr, t->xscale,
                  t->yscale, t->rot, t->flip, t->hcenter,
                  t->vcenter, t->x0, t->y0,
                  &xx1,&yy1,&xx2,&yy2, &tmp, &dtmp);
          my_free(_ALLOC_ID_, &estr);
          if(!fast) bbox(ADD, xx1, yy1, xx2, yy2);
        }
        if(!fast) {
          /* redraw rect with new props */
          bbox(SET,0.0,0.0,0.0,0.0);
          if(change_done) draw();
          bbox(END,0.0,0.0,0.0,0.0);
        }
        Tcl_ResetResult(interp);
      }
    }
    /* show_unconnected_pins
     *   Add a "lab_show.sym" to all instance pins that are not connected to anything */
    else if(!strcmp(argv[1], "show_unconnected_pins") )
    {       
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      show_unconnected_pins();
      Tcl_ResetResult(interp);
    }
    /* simulate [callback]
     *   Run a simulation (start simulator configured as default in
     *   Tools -> Configure simulators and tools)
     *   If 'callback' procedure name is given execute the procedure when simulation
     *   is finished. all execute(..., id) data is available (id = execute(id) )
     *   A callback prodedure is useful if simulation is launched in background mode
     *   ( set sim(spice,1,fg) 0 ) */
    else if(!strcmp(argv[1], "simulate") )
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(set_netlist_dir(0, NULL) ) {
        if(argc > 2) tclvareval("simulate ", argv[2], NULL);
        else tcleval("simulate");
      }
      Tcl_ResetResult(interp);
    }

    /* snap_wire
     *   Start a GUI start snapped wire placement (click to start a
     *   wire to closest pin/net endpoint) */
    else if(!strcmp(argv[1], "snap_wire"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      xctx->ui_state |= MENUSTART;
      xctx->ui_state2 = MENUSTARTSNAPWIRE;
    }

    /* str_replace str rep with [escape] [count]
     *   replace 'rep' with 'with' in string 'str'
     *   if rep not preceeded by an 'escape' character */
    else if(!strcmp(argv[1], "str_replace"))
    {
      int escape = 0, count = -1;
      if(argc > 5) escape = argv[5][0];
      if(argc > 6) count = atoi(argv[6]);
      if(argc > 4) {
        Tcl_AppendResult(interp, str_replace(argv[2], argv[3], argv[4], escape, count), NULL);
      } else {
        Tcl_SetResult(interp, "Missing arguments", TCL_STATIC);
        return TCL_ERROR;
      }
    }

    /* subst_tok str tok newval
     *   Return string 'str' with 'tok' attribute value replaced with 'newval' */
    else if(!strcmp(argv[1], "subst_tok"))
    {
      char *s=NULL;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc < 5) {Tcl_SetResult(interp, "Missing arguments", TCL_STATIC);return TCL_ERROR;}
      my_strdup(_ALLOC_ID_, &s, subst_token(argv[2], argv[3], strcmp(argv[4], "<NULL>") ? argv[4] : NULL));
      Tcl_SetResult(interp, s, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &s);
    }
    /* symbol_base_name n
     *   Return the base_name field of a symbol with name or number `n`
     *   Normally this is empty. It is set for overloaded symbols, that is symbols
     *   derived from the base symbol due to instance based implementation selection
     *   (the instance `schematic` attribute) */
    else if(!strcmp(argv[1], "symbol_base_name"))
    {
      int i = -1, found = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2 && argv[2][0]) {
        i = get_symbol(argv[2]);
        if(i >=0) {
          found = 1;
        }
      }
      if(found) {
        Tcl_AppendResult(interp, xctx->sym[i].base_name, NULL);
      } else {
        Tcl_SetResult(interp, "Missing arguments or symbol not found", TCL_STATIC);
        return TCL_ERROR;
      }
    }


    /* symbol_in_new_window [new_process]
     *   When a symbol is selected edit it in a new tab/window if not already open.
     *   If nothing selected open another window of the second schematic (issues a warning).
     *   if 'new_process' is given start a new xschem process */
    else if(!strcmp(argv[1], "symbol_in_new_window"))
    {
      int new_process = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2 && !strcmp(argv[2], "new_process")) new_process = 1;
      symbol_in_new_window(new_process);
      Tcl_ResetResult(interp);
    }

    /* swap_cursors
     *   swap cursor A (1)  and cursor B (2) positions.
     */
    else if(!strcmp(argv[1], "swap_cursors"))
    {
      xRect *r;
      Graph_ctx *gr;
      double tmp;
      int floaters = there_are_floaters();
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(xctx->rects[GRIDLAYER] > 0) {
        r =  &xctx->rect[GRIDLAYER][0];
        gr = &xctx->graph_struct;
        setup_graph_data(0, 0, gr);
        tmp = xctx->graph_cursor2_x;
        xctx->graph_cursor2_x = xctx->graph_cursor1_x;
        xctx->graph_cursor1_x = tmp;
        if(tclgetboolvar("live_cursor2_backannotate")) {
          if(xctx->graph_flags & 4) {
            backannotate_at_cursor_b_pos(r, gr);
            if(floaters) set_modify(-2); /* update floater caches to reflect actual backannotation */
          }
        }
      }
    }

    /* swap_windows
     *   swap first and second window in window interface (internal command)
     */
    else if(!strcmp(argv[1], "swap_windows"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(!tclgetboolvar("tabbed_interface") && get_window_count()) {
        swap_windows(1);
      }
    }

    /* switch [window_path |schematic_name]
     *   Switch context to indicated window path or schematic name
     *   returns 0 if switch was successfull or 1 in case of errors
     *   if "previous" given as window path switch to previously active tab
     *   (only for tabbed interface)
     *   (no tabs/windows present or no matching win_path / schematic name
     *   found).
     */
    else if(!strcmp(argv[1], "switch"))
    {
      int r = 1; /* error: no switch was done */
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) r = new_schematic("switch", argv[2], NULL, 1);
      Tcl_SetResult(interp, my_itoa(r), TCL_VOLATILE);
    }

    /* symbols [n | 'derived_symbols']
     *   if 'n' given list symbol with name or number 'n', else list all
     *   if 'derived_symbols' is given list also symbols derived from base symbol
     *   due to instance based implementation selection. This option must be used
     *   after a netlist operation with 'keep_symbols' TCL variable set to 1 */
    else if(!strcmp(argv[1], "symbols"))
    {
      int i;
      int derived_symbols = 0;
      int one_symbol = 0;
      char n[100];
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2 && !strcmp(argv[2], "derived_symbols")) derived_symbols = 1;
      else if(argc > 2 && argv[2][0] && isonlydigit(argv[2])) {
        one_symbol = 1;
        i = get_symbol(argv[2]);
        if(i >=0) Tcl_AppendResult(interp,  my_itoa(i), " {", xctx->sym[i].name, "}", NULL);
        else Tcl_SetResult(interp, "", TCL_STATIC);
      }
      if(!one_symbol) {
        for(i=0; i<xctx->symbols; ++i) {
          const char *base_name = xctx->sym[i].base_name;
          if(base_name && !derived_symbols) continue;
          my_snprintf(n , S(n), "%d", i);
          Tcl_AppendResult(interp, "  {", n, " ", "{", xctx->sym[i].name, "}", "}\n", NULL);
        }
      }
    }
    else { cmd_found = 0;}
    break;
    case 't': /*----------------------------------------------*/
    /* tab_list
     *   list all windows / tabs with window pathname and associated filename */
    if(!strcmp(argv[1], "tab_list"))
    {
      int i;
      Xschem_ctx *ctx, **save_xctx = get_save_xctx();
      int found = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      for(i = 0; i < MAX_NEW_WINDOWS; ++i) {
        ctx = save_xctx[i];
        /* if only one schematic it is not yet saved in save_xctx */
        if(get_window_count() == 0 && i == 0)  {
          ctx = xctx;
        }
        if(i == 0 ) {
          if(ctx) Tcl_AppendResult(interp, ".drw", " {", ctx->sch[ctx->currsch], "}\n", NULL);
        }
        else if(ctx) {
          Tcl_AppendResult(interp, get_window_path(i), " {", ctx->sch[ctx->currsch], "}\n", NULL);
        }
      }
      dbg(1, "check_loaded: return %d\n", found);
      return found;
    }

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
    else if(!strcmp(argv[1], "table_read"))
    {
      char f[PATH_MAX + 100];
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(sch_waves_loaded() >= 0) {
        extra_rawfile(3, NULL, NULL, -1.0, -1.0);
        /* free_rawfile(&xctx->raw, 1, 0); */
        draw();
      } else if(argc > 2) {
        my_snprintf(f, S(f),"regsub {^~/} {%s} {%s/}", argv[2], home_dir);
        tcleval(f);
        my_strncpy(f, tclresult(), S(f));
        extra_rawfile(3, NULL, NULL, -1.0, -1.0);
        /* free_rawfile(&xctx->raw, 0, 0); */
        table_read(f);
        if(sch_waves_loaded() >= 0) {
          draw();
        }
      }
      Tcl_ResetResult(interp);
    }

    /* test
     *   Testmode ... */
    else if(1 && !strcmp(argv[1], "test") )
    {
      Iterator_ctx ctx;
      Objectentry *objectptr;
      int type, n, c;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}

      if(argc > 2 && atoi(argv[2]) == 1) {
        hash_objects();
        dbg(0, "n_hash_objects=%d\n", xctx->n_hash_objects);

        for(init_object_iterator(&ctx, -420., -970., 1300., -250.); (objectptr = object_iterator_next(&ctx)) ;) {
          type = objectptr->type;
          n = objectptr->n;
          c = objectptr->c;
          dbg(0, "type=%d, n=%d c=%d\n", type, n, c);
          switch(type) {
            case ELEMENT:
              select_element(n, SELECTED, 1, 1);
              break;
            case WIRE:
              select_wire(n, SELECTED, 1, 1);
              break;
            case xTEXT:
              select_text(n, SELECTED, 1, 1);
              break;
            case xRECT:
              select_box(c, n, SELECTED, 1, 1);
              break;
            case LINE:
              select_line(c, n, SELECTED, 1, 1);
              break;
            case POLYGON:
              select_polygon(c, n, SELECTED, 1, 1);
              break;
            case ARC:
              select_arc(c, n, SELECTED, 1, 1);
              break;
          }
        }
        rebuild_selected_array();
        draw();

        del_object_table();
        Tcl_ResetResult(interp);
      }
      else if(argc > 2 && atoi(argv[2]) == 5) {
        xctx->prep_hash_inst=0;
        hash_instances();
      }
      else if(argc > 2 && atoi(argv[2]) == 4) {
        xctx->prep_hash_object=0;
        hash_objects();
      }
      else if(argc > 2 && atoi(argv[2]) == 6) {
        xctx->prep_hash_wires=0;
        hash_wires();
      }
      else if(argc > 5 && atoi(argv[2]) == 2) {
        /* example: xschem test 2 .xctrl. LDCP_REF 8 */
        prepare_netlist_structs(0);
        hier_hilight_hash_lookup(argv[4], atoi(argv[5]), argv[3], XINSERT);
        propagate_hilights(1, 0, XINSERT_NOREPLACE);
        Tcl_ResetResult(interp);
      }
      else if(argc > 2 && atoi(argv[2]) == 3) {

        char *s = "aa	bb	cc	dd\n"
                  "eee	fff	ggg	hhh";

        char *t = my_expand(s, 8);

        dbg(0, "%s\n----\n", s);
        Tcl_SetResult(interp, t, TCL_VOLATILE);
        my_free(_ALLOC_ID_, &t);
      }
    }

    /* text x y rot flip text props size draw
     *   Create a text object
     *     x, y, rot, flip specify the position and orientation
     *     text is the text string
     *     props is the attribute string
     *     size sets the size
     *     draw is a flag. If set to 1 will draw the created text */
    else if(!strcmp(argv[1], "text") )
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc < 10) {Tcl_SetResult(interp,
          "xschem text requires 8 additional arguments", TCL_STATIC); return TCL_ERROR;}

      create_text(atoi(argv[9]), atof(argv[2]), atof(argv[3]), atoi(argv[4]), atoi(argv[5]),
                    argv[6], argv[7], atof(argv[8]), atof(argv[9]));
      Tcl_ResetResult(interp);
    }


    /* text_string n
     *   get text string of text object 'n' */
    else if(!strcmp(argv[1], "text_string") )
    {
      int n;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc < 3) {Tcl_SetResult(interp,
          "xschem text_string requires 1 additional argument", TCL_STATIC); return TCL_ERROR;}
      n = atoi(argv[2]);
      if(n >= 0 && n < xctx->texts) {
        Tcl_SetResult(interp, xctx->text[n].txt_ptr, TCL_VOLATILE);
      } else {
        Tcl_ResetResult(interp);
      }
    }

    /* toggle_colorscheme
     *   Toggle dark/light colorscheme */
    else if(!strcmp(argv[1], "toggle_colorscheme"))
    {
      int d_c;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      d_c = tclgetboolvar("dark_colorscheme");
      d_c = !d_c;
      tclsetboolvar("dark_colorscheme", d_c);
      tclsetdoublevar("dim_value", 0.0);
      tclsetdoublevar("dim_bg", 0.0);
      build_colors(0.0, 0.0);
      draw();
      Tcl_ResetResult(interp);
    }

    /* toggle_ignore
     *   toggle *_ignore=true attribute on selected instances
     *   * = {spice,verilog,vhdl,tedax} depending on current netlist mode */
    else if(!strcmp(argv[1], "toggle_ignore"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      toggle_ignore();
      Tcl_ResetResult(interp);
    }

    /* touch x1 y1 x2 y2 x0 y0
     *   returns 1 if line {x1 y1 x2 y2} touches point {x0 y0}, 0 otherwise */
    else if(!strcmp(argv[1], "touch") )
    {
      if(argc>7) {
        double x1, y1, x2, y2, x0, y0;
        int r;
        x0 = atof(argv[6]);
        y0 = atof(argv[7]);
        x1 = atof(argv[2]);
        y1 = atof(argv[3]);
        x2 = atof(argv[4]);
        y2 = atof(argv[5]);
        r = touch(x1, y1, x2, y2, x0, y0);
        Tcl_SetResult(interp, my_itoa(r), TCL_VOLATILE);
      }
    }

    /* translate n str
     *   Translate string 'str' replacing @xxx tokens with values in instance 'n' attributes
     *     Example: xschem translate vref {the voltage is @value}
     *     the voltage is 1.8
     *   If -1 is given as the instance number try to translate the string without using any
     *   instance specific data */
    else if(!strcmp(argv[1], "translate") )
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc>3) {
        int i;
        char *s = NULL;
        if(!strcmp(argv[2], "-1")) i = -1;
        else if((i = get_instance(argv[2])) < 0 ) {
          Tcl_SetResult(interp, "xschem translate: instance not found", TCL_STATIC);
          return TCL_ERROR;
        }
        my_strdup2(_ALLOC_ID_, &s, translate(i, argv[3]));
        Tcl_ResetResult(interp);
        Tcl_SetResult(interp, s, TCL_VOLATILE);
        my_free(_ALLOC_ID_, &s);
      }
    }

    /* translate3 str eat_escapes s1 [s2] [s3]
     *   Translate string 'str' replacing @xxx tokens with values in string s1 or if
     *     not found in string s2 or if not found in string s3
     *     eat_escapes should be either 1 (remove backslashes) or 0 (keep them)
     *     Example: xschem translate3 {the voltage is @value} {name=x12} {name=x1 value=1.8}
     *     the voltage is 1.8 */
    else if(!strcmp(argv[1], "translate3") )
    {
      char *s = NULL;
      int eat_escapes = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 3) eat_escapes = atoi(argv[3]);
      if(argc > 6) my_strdup2(_ALLOC_ID_, &s, translate3(argv[2], eat_escapes, argv[4], argv[5], argv[6], NULL));
      else if(argc > 5) my_strdup2(_ALLOC_ID_, &s, translate3(argv[2], eat_escapes, argv[4], argv[5], NULL, NULL));
      else if(argc > 4) my_strdup2(_ALLOC_ID_, &s, translate3(argv[2], eat_escapes, argv[4], NULL, NULL, NULL));
      else {
        Tcl_SetResult(interp, "xschem translate3: missing arguments", TCL_STATIC);
        return TCL_ERROR;
      }
      Tcl_ResetResult(interp);
      Tcl_SetResult(interp, s, TCL_VOLATILE);
      my_free(_ALLOC_ID_, &s);
    }

    /* trim_chars str sep
     *   Remove leading and trailing chars matching any character in 'sep' from str */
    else if(!strcmp(argv[1], "trim_chars"))
    {
      if(argc > 3) {
        char *s = trim_chars(argv[2], argv[3]);
        Tcl_SetResult(interp, s, TCL_VOLATILE);
      }
    }

    /* trim_wires
     *   Remove operlapping wires, join lines, trim wires at intersections */
    else if(!strcmp(argv[1], "trim_wires"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      xctx->push_undo();
      trim_wires();
      draw();
      Tcl_ResetResult(interp);
    }
    else { cmd_found = 0;}
    break;
    case 'u': /*----------------------------------------------*/
    /* undo  [redo [set_modify]]
         Undo last action. Optional integers redo and set_modify are passed to pop_undo() */
    if(!strcmp(argv[1], "undo"))
    {
      int redo = 0, set_modify = 1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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

    /* unhilight_all [fast]
     *   if 'fast' is given do not redraw
     *   Clear all highlights */
    else if(!strcmp(argv[1], "unhilight_all"))
    {
      int fast = 0;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2 && !strcmp(argv[2], "fast")) fast = 1;
      xctx->enable_drill=0;
      clear_all_hilights();
      /* undraw_hilight_net(1); */
      if(!fast) draw();
      Tcl_ResetResult(interp);
    }

    /* unhilight
     *   Unhighlight selected nets/pins */
    else if(!strcmp(argv[1], "unhilight"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      unhilight_net();
    }

    /* unselect_all [draw]
     *   Unselect everything. If draw is given and set to '0' no drawing is done */
    else if(!strcmp(argv[1], "unselect_all"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) unselect_all(atoi(argv[2]));
      else unselect_all(1);
      Tcl_ResetResult(interp);
    }

    /* unselect_attached_floaters
     *   Unselect objects (not symbol instances) attached to some instance with a 
     *   non empty name=... attribute */
    else if(!strcmp(argv[1], "unselect_attached_floaters"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      unselect_attached_floaters();
    }
    /* update_all_sym_bboxes
     *   Update all symbol bounding boxes */
    else if(!strcmp(argv[1], "update_all_sym_bboxes"))
    {
      int i;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      for(i = 0; i < xctx->texts; i++)
      if(xctx->text[i].flags & TEXT_FLOATER) {
        my_free(_ALLOC_ID_, &xctx->text[i].floater_ptr); /* clear floater cached value */
      }
      for(i = 0; i < xctx->instances; ++i) {
        symbol_bbox(i, &xctx->inst[i].x1, &xctx->inst[i].y1, &xctx->inst[i].x2, &xctx->inst[i].y2);
      }
      Tcl_ResetResult(interp);
    }

    /* update_op
     *   update tcl ngspice::ngspice array data from raw file point 0 */
    else if(!strcmp(argv[1], "update_op"))
    {
      int ret;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      ret = update_op();
      Tcl_SetResult(interp, my_itoa(ret), TCL_VOLATILE);
    }

    else { cmd_found = 0;}
    break;
    case 'v': /*----------------------------------------------*/
    /* view_prop
     *   View attributes of selected element (read only)
     *   if multiple selection show the first element (in xschem  array order) */
    if(!strcmp(argv[1], "view_prop"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        warning_overlapped_symbols(atoi(argv[2]));
      } else {
        warning_overlapped_symbols(0);
      }
    }
    /* windowid topwin_path
     *   Used by xschem.tcl for configure events (set icon) */
    else if(!strcmp(argv[1], "windowid"))
    {
      if(argc > 2) {
        windowid(argv[2]);
      }
    }
    /* wire_coord n
     *   return 4 coordinates of wire[n] */
    else if(!strcmp(argv[1], "wire_coord"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      if(argc > 2) {
        char *r = NULL;
        int n = atoi(argv[2]);
        if(n > 0 && n < xctx->wires) {
          xWire * const wire = xctx->wire;
          my_mstrcat(_ALLOC_ID_, &r, dtoa(wire[n].x1), " ", NULL);
          my_mstrcat(_ALLOC_ID_, &r, dtoa(wire[n].y1), " ", NULL);
          my_mstrcat(_ALLOC_ID_, &r, dtoa(wire[n].x2), " ", NULL);
          my_mstrcat(_ALLOC_ID_, &r, dtoa(wire[n].y2), NULL);
          Tcl_SetResult(interp, r, TCL_VOLATILE);
        }
      }
    }
    /* wire [x1 y1 x2 y2] [pos] [prop] [sel]
     *   wire
     *   wire gui
     *   Place a new wire
     *   if no coordinates are given start a GUI wire placement
     *   if `gui` argument is given start a GUI placement of a wire with 1st point
     *   starting from current mouse coordinates */
    else if(!strcmp(argv[1], "wire"))
    {
      double x1,y1,x2,y2;
      int pos = -1, save, sel = 0;
      const char *prop=NULL;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
        set_modify(1);
      }
      else if(argc > 2 && !strcmp(argv[2], "gui")) {
        int prev_state = xctx->ui_state;
        int infix_interface = tclgetboolvar("infix_interface");
        if(infix_interface) {
          start_wire(xctx->mousex_snap, xctx->mousey_snap);
          if(prev_state == STARTWIRE) {
            tcleval("set constr_mv 0" );
            xctx->constr_mv=0;
          }
        } else {
          xctx->last_command = 0;
          xctx->ui_state |= MENUSTART;                   
          xctx->ui_state2 = MENUSTARTWIRE;
        }
      } else {
        xctx->last_command = 0;
        xctx->ui_state |= MENUSTART;
        xctx->ui_state2 = MENUSTARTWIRE;
      }
    }
    /* wire_cut [x y] [noalign]
     *   start a wire cut operation. Point the mouse in the middle of a wire and
     *   Alt-click right button.
     *   if x and y are given cut wire at given point
     *   if noalign is given and is set to 'noalign' do not align the cut point to closest snap point */
    else if(!strcmp(argv[1], "wire_cut"))
    {
      int i, align = 1;
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      for(i = 2; i < argc; i++) {
        if(!strcmp(argv[i], "noalign")) align = 0;
      }
      if(argc > 3) {
        break_wires_at_point(atof(argv[2]), atof(argv[3]), align);
      } else {
        if(align) {
          xctx->ui_state |= MENUSTART;
          xctx->ui_state2 = MENUSTARTWIRECUT;
        } else {
          xctx->ui_state |= MENUSTART;
          xctx->ui_state2 = MENUSTARTWIRECUT2;
        }
      }
      Tcl_ResetResult(interp);
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
        xctx->ui_state |= MENUSTART;
        xctx->ui_state2 = MENUSTARTZOOM;
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
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
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      zoom_full(1, 2, 1 + 2 * tclgetboolvar("zoom_full_center"), 0.97);
      Tcl_ResetResult(interp);
    }

    /* zoom_in
     *   Zoom in drawing */
    else if(!strcmp(argv[1], "zoom_in"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      view_zoom(0.0);
      Tcl_ResetResult(interp);
    }

    /* zoom_out
     *   Zoom out drawing */
    else if(!strcmp(argv[1], "zoom_out"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      view_unzoom(0.0);
      Tcl_ResetResult(interp);
    }

    /* zoom_selected
     *   Zoom to selection */
    else if(!strcmp(argv[1], "zoom_selected"))
    {
      if(!xctx) {Tcl_SetResult(interp, not_avail, TCL_STATIC); return TCL_ERROR;}
      zoom_full(1, 1, 1 + 2 * tclgetboolvar("zoom_full_center"), 0.97);
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
  int res;
  const char *p;
  p = Tcl_GetVar(interp, s, TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);
  if(!p) {
    dbg(0, "%s\n", tclresult());
    return 0;
  }
  if(Tcl_GetBoolean(interp, p, &res) == TCL_ERROR) {
    dbg(0, "%s\n", tclresult());
    return 0;
  }
  return res;
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
  while( (p = va_arg(args, const char *)) ) {
    size = my_strcat(_ALLOC_ID_, &str, p);
    dbg(2, "tclvareval(): p=%s, str=%s, size=%d\n", p, str, size);
  }
  dbg(2, "tclvareval(): script=%s, str=%s, size=%d\n", script, str ? str : "<NULL>", size);
  return_code = Tcl_EvalEx(interp, str, (int)size, TCL_EVAL_GLOBAL);
  va_end(args);
  if(return_code != TCL_OK) {
    dbg(0, "tclvareval(): error executing %s: %s\n", str, tclresult());
    Tcl_ResetResult(interp);
  }
  my_free(_ALLOC_ID_, &str);
  return return_code;
}
