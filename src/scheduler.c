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

extern char yytext[];
extern int yylex (void);
typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE yy_scan_string ( const char *yy_str  );

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
  for(i=0;i<xctx->instances;i++) {
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
  Tcl_ResetResult(interp);
  Tcl_AppendResult(interp, 
    "Xschem command language:\n", 
    "  xschem subcommand [args]\n",
    "\"xschem\" subcommands available:\n",
    /* "add_symbol_pin\n", */
    "align\n",
    "  Align selected part of schematic to current gid snap setting\n",
    /* "arc\n", */
    "attach_labels\n",
    "  Attach labels to selected instance pins\n",
    "bbox [begin | end]\n",
    "  bbox begin: start a bounding box drawing area setting\n",
    "  bbox end: end a bounding box drawing area setting, draw stuff and reset bbox\n",
    "break_wires\n",
    "build_colors\n",
    "callback\n",
    "check_symbols\n",
    "check_unique_names\n",
    "circle\n",
    "clear\n",
    "clear_drawing\n",
    "color_dim\n",
    "connected_nets\n",
    "copy\n",
    "copy_objects\n",
    "create_plot_cmd\n",
    "cut\n",
    "debug\n",
    "delete\n",
    "delete_files\n",
    "descend\n",
    "descend_symbol\n",
    "display_hilights\n",
    "edit_file\n",
    "edit_prop\n",
    "edit_vi_prop\n",
    "enable_layers\n",
    "exit\n",
    "expandlabel\n",
    "find_nth\n",
    "flip\n",
    "fullscreen\n",
    "get var\n",
    "  where \"var\" is one of:\n",
    "  schname [n]\n",
    "    get name of schematic, optionally of upper levels if (negative) n is provided\n",
    "  sch_path [n]\n",
    "    get pathname of schematic, optionally of upper levels if (negative) n is provided\n",
    "  backlayer\n",
    "  bbox_hilighted\n",
    "  bbox_selected\n",
    "  cadlayers\n",
    "  color_ps\n",
    "  current_dirname\n",
    "  currsch\n",
    "  debug_var\n",
    "  draw_window\n",
    "  flat_netlist\n",
    "  gridlayer\n",
    "  help\n",
    "  instances\n",
    "  lastsel\n",
    "  line_width\n",
    "  netlist_name\n",
    "  netlist_type\n",
    "  no_draw\n",
    "  pinlayer\n",
    "  rectcolor\n",
    "  sellayer\n",
    "  semaphore\n",
    "  temp_dir\n",
    "  text_svg\n",
    "  textlayer\n",
    "  topwindow\n",
    "  version\n",
    "  wirelayer\n",
    "  xorigin\n",
    "  yorigin\n",
    "  zoom\n",
    "get_tok\n",
    "get_tok_size\n",
    "getprop\n",
    "globals\n",
    "go_back\n",
    "help\n",
    "hier_psprint\n",
    "hilight\n",
    "hilight_netname\n",
    "instance\n",
    "instance_bbox\n",
    "instance_net\n",
    "instance_nodemap\n",
    "instance_pin_coord\n",
    "instance_pins\n",
    "instance_pos\n",
    "instances_to_net\n",
    "line\n",
    "line_width\n",
    "list_tokens\n",
    "load\n",
    "load_new_window\n",
    "load_symbol\n",
    "log\n",
    "logic_set\n",
    "make_sch\n",
    "make_symbol\n",
    "merge\n",
    "move_objects\n",
    "net_label\n",
    "net_pin_mismatch\n",
    "netlist\n",
    "new_schematic\n",
    "new_symbol_window\n",
    "new_window\n",
    "only_probes\n",
    "origin\n",
    "parselabel\n",
    "paste\n",
    "pinlist\n",
    "place_symbol\n",
    "place_text\n",
    "polygon\n",
    "preview_window\n",
    "print\n",
    "print_hilight_net\n",
    "print_spice_element\n",
    "propagate_hilights\n",
    "push_undo\n",
    "rebuild_connectivity\n",
    "rect\n",
    "redo\n",
    "redraw\n",
    "reload\n",
    "reload_symbols\n",
    "remove_symbols\n",
    "replace_symbol\n",
    "rotate\n",
    "save\n",
    "saveas\n",
    "schematic_in_new_window\n",
    "search\n",
    "searchmenu\n",
    "select\n",
    "select_all\n",
    "select_hilight_net\n",
    "selected_set\n",
    "selected_wire\n",
    "send_to_viewer\n",
    "set var value\n",
    "  where \"var\" is one of:\n",
    "  cadgrid\n",
    "  cadsnap\n",
    "  color_ps\n",
    "  constrained_move\n",
    "  draw_window\n",
    "  flat_netlist\n",
    "  hide_symbols\n",
    "  netlist_name\n",
    "  netlist_type\n",
    "  no_draw\n",
    "  no_undo\n",
    "  rectcolor\n",
    "  text_svg\n",
    "  semaphore\n",
    "  sym_txt\n",
    "set_different_tok\n",
    "set_modify\n",
    "setprop\n",
    "show_pin_net_names\n",
    "simulate\n",
    "snap_wire\n",
    "subst_tok\n",
    "symbol_in_new_window\n",
    "symbols\n",
    "test\n",
    "toggle_colorscheme\n",
    "translate\n",
    "trim_wires\n",
    "undo\n",
    "undo_type\n",
    "unhilight_all\n",
    "unhilight\n",
    "unselect_all\n",
    "view_prop\n",
    /* "windowid\n", */
    "windows\n",
    "wire\n",
    "zoom_box\n",
    "zoom_full\n",
    "zoom_hilighted\n",
    "zoom_in\n",
    "zoom_out\n",
    "zoom_selected\n",
    NULL);
}

/* can be used to reach C functions from the Tk shell. */
int xschem(ClientData clientdata, Tcl_Interp *interp, int argc, const char * argv[])
{
 int i;
 char name[1024]; /* overflow safe 20161122 */
 int cmd_found = 0;

 Tcl_ResetResult(interp);
 if(argc<2) {
   Tcl_SetResult(interp, "Missing arguments.", TCL_STATIC);
   return TCL_ERROR;
 }
 if(debug_var>=2) {
   int i;
   fprintf(errfp, "xschem():");
   for(i=0; i<argc; i++) {
     fprintf(errfp, "%s ", argv[i]);
   }
   fprintf(errfp, "\n");
 }

 /*
  * ********** xschem commands  IN SORTED ORDER !!! *********
  */

  if(argv[1][0] == 'a') {   
    if(!strcmp(argv[1],"add_symbol_pin"))
    {
       cmd_found = 1;
       unselect_all(1);
       storeobject(-1, xctx->mousex_snap-2.5, xctx->mousey_snap-2.5, xctx->mousex_snap+2.5, xctx->mousey_snap+2.5,
                   xRECT, PINLAYER, SELECTED, "name=XXX\ndir=inout");
       xctx->need_reb_sel_arr=1;
       rebuild_selected_array();
       move_objects(START,0,0,0);
       xctx->ui_state |= START_SYMPIN;
       Tcl_ResetResult(interp);
    }

    if(!strcmp(argv[1],"add_graph"))
    {
       cmd_found = 1;
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
           "dataset=0\n"
           "unitx=u\n"
           "logx=0\n"
           "logy=0\n"
         );
       xctx->need_reb_sel_arr=1;
       rebuild_selected_array();
       move_objects(START,0,0,0);
       xctx->ui_state |= START_SYMPIN;
       Tcl_ResetResult(interp);
    }

    if(!strcmp(argv[1],"add_png"))
    {
       char str[PATH_MAX+100];
       cmd_found = 1;
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

    else if(!strcmp(argv[1],"align"))
    {
       cmd_found = 1;
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
   
    else if(!strcmp(argv[1],"arc"))
    {
      cmd_found = 1;
      xctx->ui_state |= MENUSTARTARC;
    }
   
    else if(!strcmp(argv[1],"attach_labels")) /* attach pins to selected component 20171005 */
    {
      cmd_found = 1;
      attach_labels_to_inst(0);
      Tcl_ResetResult(interp);
    }
  }

  else if(argv[1][0] == 'b') {   
    if(!strcmp(argv[1], "bbox"))
    {
      cmd_found = 1;
      if(argc == 3) {
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
   
    else if(!strcmp(argv[1],"break_wires"))
    {
      cmd_found = 1;
      break_wires_at_pins();
      Tcl_ResetResult(interp);
    }

    else if(!strcmp(argv[1],"build_colors"))
    {
      cmd_found = 1;
      build_colors(tclgetdoublevar("dim_value"), tclgetdoublevar("dim_bg"));
      Tcl_ResetResult(interp);
    }
  }

  else if(argv[1][0] == 'c') {
    if(!strcmp(argv[1],"callback") )
    {
      cmd_found = 1;
      callback( argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), (KeySym)atol(argv[6]),
               atoi(argv[7]), atoi(argv[8]), atoi(argv[9]) );
      dbg(2, "callback %s %s %s %s %s %s %s %s\n", argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9]);
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"case_insensitive"))
    {
      cmd_found = 1;
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
    else if(!strcmp(argv[1],"check_symbols"))
    {
      char sympath[PATH_MAX];
      const char *name;
      struct stat buf;
      cmd_found = 1;
      for(i=0;i<xctx->symbols;i++) {
        name = xctx->sym[i].name;
        if(!strcmp(xctx->file_version,"1.0")) {
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

    else if(!strcmp(argv[1],"check_unique_names"))
    {
      cmd_found = 1;
      if(!strcmp(argv[2],"1")) {
        check_unique_names(1);
      } else {
        check_unique_names(0);
      }
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"circle"))
    {
      cmd_found = 1;
      xctx->ui_state |= MENUSTARTCIRCLE;
    }
   
    else if(!strcmp(argv[1],"clear"))
    {
      int cancel = 0;
   
      cmd_found = 1;
      if( argc < 3 || strcmp(argv[2], "force") ) cancel=save(1);
      if(cancel != -1) { /* -1 means user cancel save request */
        char name[PATH_MAX];
        struct stat buf;
        int i;
   
        xctx->currsch = 0;
        unselect_all(1);
        remove_symbols();
        clear_drawing();
        if(argc>=3 && !strcmp(argv[2],"SYMBOL")) {
          xctx->netlist_type = CAD_SYMBOL_ATTRS;
          set_tcl_netlist_type();
          for(i=0;;i++) { /* find a non-existent untitled[-n].sym */
            if(i == 0) my_snprintf(name, S(name), "%s.sym", "untitled");
            else my_snprintf(name, S(name), "%s-%d.sym", "untitled", i);
            if(stat(name, &buf)) break;
          }
          my_snprintf(xctx->sch[xctx->currsch], S(xctx->sch[xctx->currsch]), "%s/%s", pwd_dir, name);
          my_strncpy(xctx->current_name, name, S(xctx->current_name));
        } else {
          xctx->netlist_type = CAD_SPICE_NETLIST;
          set_tcl_netlist_type();
          for(i=0;;i++) {
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
   
    else if(!strcmp(argv[1],"clear_drawing"))
    {
      cmd_found = 1;
      if(argc==2) clear_drawing();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1], "color_dim"))
    {
      cmd_found = 1;
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

    else if(!strcmp(argv[1], "compare_schematics"))
    {
      int ret = 0;
      cmd_found = 1;
      if(argc > 2) {
        ret = compare_schematics(argv[2]);
      }
      else {
        ret = compare_schematics(NULL); 
      }
      Tcl_SetResult(interp, my_itoa(ret), TCL_VOLATILE);
    }

    else if(!strcmp(argv[1],"connected_nets")) /* selected nets connected to currently selected ones */
    {
      int stop_at_junction = 0;
      cmd_found = 1;
      if(argc>=3 && argv[2][0] == '1') stop_at_junction = 1;
      select_connected_wires(stop_at_junction);
      Tcl_ResetResult(interp);
    }

    else if(!strcmp(argv[1],"copy"))
    {
      cmd_found = 1;
      rebuild_selected_array();
      save_selection(2);
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"copy_objects"))
    {
      cmd_found = 1;
      copy_objects(START);
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"count_items"))
    {
      if(argc > 4) {
        Tcl_SetResult(interp, my_itoa(count_items(argv[2], argv[3], argv[4])), TCL_VOLATILE);
      }
      cmd_found = 1;
    }
    else if(!strcmp(argv[1],"create_plot_cmd") )
    {
      cmd_found = 1;
      create_plot_cmd();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"cut"))
    {
      cmd_found = 1;
      rebuild_selected_array();
      save_selection(2);
      delete(1/*to_push_undo*/);
      Tcl_ResetResult(interp);
    }
  }

  else if(argv[1][0] == 'd') {   
    if(!strcmp(argv[1],"debug"))
    {
      cmd_found = 1;
      if(argc==3) {
         debug_var=atoi(argv[2]);
         tclsetvar("debug_var",argv[2]);
      }
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"delete"))
    {
      cmd_found = 1;
      if(argc==2) delete(1/*to_push_undo*/);
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"delete_files"))
    {
      cmd_found = 1;
      delete_files();
    }
   
    else if(!strcmp(argv[1],"descend"))
    {
      int ret=0;
      cmd_found = 1;
      if(argc >=3) {
        int n = atoi(argv[2]);
        ret = descend_schematic(n);
      } else {
        ret = descend_schematic(0);
      }
      Tcl_SetResult(interp, dtoa(ret), TCL_VOLATILE);
    }
   
    else if(!strcmp(argv[1],"descend_symbol"))
    {
      cmd_found = 1;
      descend_symbol();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1], "display_hilights"))
    {
      char *str = NULL;
      cmd_found = 1;
      display_hilights(&str);
      Tcl_SetResult(interp, str, TCL_VOLATILE);
      my_free(1161, &str);
    }
    else if(!strcmp(argv[1], "draw_graph"))
    {
      int flags;
      cmd_found = 1;
      if(argc > 2) {
        int i = atoi(argv[2]);
        if(argc > 3) {
          flags = atoi(argv[3]);
        } else {
          flags = 1 + 8 + (xctx->graph_flags & 6);
        }
        setup_graph_data(i, xctx->graph_flags, 0,  &xctx->graph_struct);
        draw_graph(i, flags, &xctx->graph_struct);
      }
      Tcl_ResetResult(interp);
    }

  }

  else if(argv[1][0] == 'e') {   
    if(!strcmp(argv[1],"edit_file") )
    {
      cmd_found = 1;
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
   
    else if(!strcmp(argv[1],"edit_prop"))
    {
      cmd_found = 1;
      edit_property(0);
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"edit_vi_prop"))
    {
      cmd_found = 1;
      edit_property(1);
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"embed_rawfile"))
    {
      cmd_found = 1;
      if(argc > 2) {
        embed_rawfile(argv[2]);
      }
      Tcl_ResetResult(interp);
    }
    else if(!strcmp(argv[1],"enable_layers"))
    {
      cmd_found = 1;
      enable_layers();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"exit"))
    {
      cmd_found = 1;
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
            if(!xctx->modified || !strcmp(tclresult(),"ok")) tcleval("exit");
          }
        }
        else tcleval("exit"); /* if has_x == 0 there are no additional windows to close */
      } else {
        new_schematic("destroy", xctx->current_win_path, NULL);
      }
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"expandlabel") && argc == 3) 
    {
      int tmp;
      size_t llen;
      char *result=NULL;
      const char *l;
   
      cmd_found = 1;
      l = expandlabel(argv[2], &tmp);
      llen = strlen(l);
      dbg(0, "l=%s\n", l ? l : "<NULL>");
      result = my_malloc(378, llen + 30);
      my_snprintf(result, llen + 30, "%s %d", l, tmp);
      Tcl_SetResult(interp, result, TCL_VOLATILE);
      my_free(927, &result);
    }
  }

  else if(argv[1][0] == 'f') {   
    if(!strcmp(argv[1],"find_nth"))
    {
      cmd_found = 1;
      if(argc > 4) {
        Tcl_SetResult(interp, find_nth(argv[2], argv[3], atoi(argv[4])), TCL_VOLATILE);
      }
    }
    else if(!strcmp(argv[1],"flip"))
    {
      cmd_found = 1;
      if(! (xctx->ui_state & (STARTMOVE | STARTCOPY) ) ) { 
        rebuild_selected_array();
        move_objects(START,0,0,0);
        move_objects(FLIP|ROTATELOCAL,0,0,0);
        move_objects(END,0,0,0);
      }
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"fullscreen"))
    {
      cmd_found = 1;
      if(argc > 2) toggle_fullscreen(argv[2]);
      else toggle_fullscreen(".drw");
      Tcl_ResetResult(interp);
    }
  }

  else if(argv[1][0] == 'g') {   
    /*
     * ********** xschem get subcommands
     */
   
    if(argc >= 3 && !strcmp(argv[1],"get") && !strcmp(argv[2],"schname"))
    {
    /* allows to retrieve name of n-th parent schematic */
      int x;
      cmd_found = 1;
      if(argc >= 4) x = atoi(argv[3]);
      else x = xctx->currsch;
      if(x<0 && xctx->currsch+x>=0) {
        Tcl_SetResult(interp, xctx->sch[xctx->currsch+x], TCL_VOLATILE);
      } else if(x<=xctx->currsch) {
        Tcl_SetResult(interp, xctx->sch[x], TCL_VOLATILE);
      }
    }

    else if( argc >= 3 && !strcmp(argv[1],"get") && !strcmp(argv[2],"sch_path")) 
    {
      int x;
      cmd_found = 1;
      if(argc == 4) x = atoi(argv[3]);
      else x = xctx->currsch;
      if(x<0 && xctx->currsch+x>=0) {
        Tcl_SetResult(interp, xctx->sch_path[xctx->currsch+x], TCL_VOLATILE);
      } else if(x<=xctx->currsch) {
        Tcl_SetResult(interp, xctx->sch_path[x], TCL_VOLATILE);
      }
    }
    else if(argc >= 4 && !strcmp(argv[1],"get") && !strcmp(argv[2],"netlist_name") &&
            !strcmp(argv[3], "fallback")) {
      char f[PATH_MAX];
      cmd_found = 1;

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
    }
    else if(!strcmp(argv[1],"get") && argc==3)
    {
     cmd_found = 1;
     if(!strcmp(argv[2],"backlayer")) {
       Tcl_SetResult(interp, my_itoa(BACKLAYER), TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"bbox_hilighted")) {
       xRect boundbox;
       char res[2048];
       calc_drawing_bbox(&boundbox, 2);
       my_snprintf(res, S(res), "%g %g %g %g", boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
       Tcl_SetResult(interp, res, TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"bbox_selected")) {
       xRect boundbox;
       char res[2048];
       calc_drawing_bbox(&boundbox, 1);
       my_snprintf(res, S(res), "%g %g %g %g", boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
       Tcl_SetResult(interp, res, TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"cadlayers")) {
       Tcl_SetResult(interp, my_itoa(cadlayers), TCL_VOLATILE);
     }
     else if(!strcmp(argv[2], "case_insensitive")) {
       Tcl_SetResult(interp, my_itoa(xctx->case_insensitive), TCL_VOLATILE);
     }
     else if(!strcmp(argv[2], "color_ps")) {
       if( color_ps != 0 )
         Tcl_SetResult(interp, "1",TCL_STATIC);
       else
         Tcl_SetResult(interp, "0",TCL_STATIC);
     }
     else if(!strcmp(argv[2],"current_dirname")) {
       Tcl_SetResult(interp, xctx->current_dirname, TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"current_name")) {
       Tcl_SetResult(interp, xctx->current_name, TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"current_win_path")) {
       Tcl_SetResult(interp, xctx->current_win_path, TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"currsch")) {
       Tcl_SetResult(interp, my_itoa(xctx->currsch),TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"debug_var")) {
       Tcl_SetResult(interp, my_itoa(debug_var),TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"draw_window")) {
       Tcl_SetResult(interp, my_itoa(xctx->draw_window),TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"flat_netlist")) {
       if( xctx->flat_netlist != 0 )
         Tcl_SetResult(interp, "1",TCL_STATIC);
       else
         Tcl_SetResult(interp, "0",TCL_STATIC);
     }
     else if(!strcmp(argv[2],"format")) {
       if( !xctx->format )
         Tcl_SetResult(interp, "<NULL>",TCL_STATIC);
       else
         Tcl_SetResult(interp, xctx->format,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"graph_lastsel")) {
       Tcl_SetResult(interp, my_itoa(xctx->graph_lastsel),TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"gridlayer")) {
       Tcl_SetResult(interp, my_itoa(GRIDLAYER),TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"help")) {
       if( help != 0 )
         Tcl_SetResult(interp, "1",TCL_STATIC);
       else
         Tcl_SetResult(interp, "0",TCL_STATIC);
     }
     else if(!strcmp(argv[2],"instances")) {
       Tcl_SetResult(interp, my_itoa(xctx->instances), TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"lastsel")) {
       rebuild_selected_array();
       Tcl_SetResult(interp, my_itoa(xctx->lastsel),TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"line_width")) {
       Tcl_SetResult(interp, dtoa(xctx->lw), TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"netlist_name")) {
       Tcl_SetResult(interp, xctx->netlist_name, TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"netlist_type"))
     {
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
     else if(!strcmp(argv[2],"no_draw")) {
       if( xctx->no_draw != 0 )
         Tcl_SetResult(interp, "1",TCL_STATIC);
       else
         Tcl_SetResult(interp, "0",TCL_STATIC);
     }
     else if(!strcmp(argv[2],"ntabs")) {
       Tcl_SetResult(interp, my_itoa(new_schematic("ntabs", NULL, NULL)),TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"pinlayer")) {
       Tcl_SetResult(interp, my_itoa(PINLAYER),TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"rectcolor")) {
       Tcl_SetResult(interp, my_itoa(xctx->rectcolor),TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"sellayer")) {
       Tcl_SetResult(interp, my_itoa(SELLAYER),TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"semaphore")) {
       Tcl_SetResult(interp, my_itoa(xctx->semaphore),TCL_VOLATILE);
     }
     #ifndef __unix__
     else if(!strcmp(argv[2], "temp_dir")) {
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
     #endif
     else if(!strcmp(argv[2],"text_svg")) {
       if( text_svg != 0 )
         Tcl_SetResult(interp, "1",TCL_STATIC);
       else
         Tcl_SetResult(interp, "0",TCL_STATIC);
     }
     else if(!strcmp(argv[2],"textlayer")) {
       Tcl_SetResult(interp, my_itoa(TEXTLAYER), TCL_VOLATILE);
     }
     /* top_path="" for main window, ".x1", ".x2", ... for additional windows.
      * always "" in tabbed interface */
     else if(!strcmp(argv[2],"top_path")) {
       Tcl_SetResult(interp, xctx->top_path, TCL_VOLATILE);
     }
     /* same as above but main window returned as "." */
     else if(!strcmp(argv[2],"topwindow")) {
       char *top_path;
       top_path =  xctx->top_path[0] ? xctx->top_path : ".";
       Tcl_SetResult(interp, top_path,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"version")) {
       Tcl_SetResult(interp, XSCHEM_VERSION, TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"wirelayer")) {
       Tcl_SetResult(interp, my_itoa(WIRELAYER), TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"xorigin")) {
       char s[128];
       my_snprintf(s, S(s), "%.16g", xctx->xorigin);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"yorigin")) {
       char s[128];
       my_snprintf(s, S(s), "%.16g", xctx->yorigin);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"zoom")) {
       char s[128];
       my_snprintf(s, S(s), "%.16g", xctx->zoom);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else {
       fprintf(errfp, "xschem get %s: invalid command.\n", argv[2]);
     }
    } /*  else if(!strcmp(argv[1],"get") && argc==3) */

    else if(!strcmp(argv[1],"getprop"))
    {
      cmd_found = 1;
      if( argc > 2 && !strcmp(argv[2], "instance")) {
        int i;
        const char *tmp;
        if(argc!=5 && argc !=4) {
          Tcl_SetResult(interp, "'xschem getprop instance' needs 1 or 2 additional arguments", TCL_STATIC);
          return TCL_ERROR;
        }
        if( (i = get_instance(argv[3])) < 0 ) {
          Tcl_SetResult(interp, "xschem getprop: instance not found", TCL_STATIC);
          return TCL_ERROR;
        }
        if(argc == 4) {
          Tcl_SetResult(interp, xctx->inst[i].prop_ptr, TCL_VOLATILE);
        } else if(!strcmp(argv[4],"cell::name")) {
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
        if(argc != 6 && argc != 5) {
          Tcl_SetResult(interp, "xschem getprop instance_pin needs 2 or 3 additional arguments", TCL_STATIC);
          return TCL_ERROR;
        }
        if( (inst = get_instance(argv[3])) < 0 ) {
          Tcl_SetResult(interp, "xschem getprop: instance not found", TCL_STATIC);
          return TCL_ERROR;
        }
        if(isonlydigit(argv[4])) {
          n = atoi(argv[4]);
        }
        else {
          xSymbol *ptr = xctx->inst[inst].ptr+ xctx->sym;
          for(n = 0; n < ptr->rects[PINLAYER]; n++) {
            char *prop = ptr->rect[PINLAYER][n].prop_ptr;
            if(!strcmp(get_tok_value(prop, "name",0), argv[4])) break;
          }
        }
        if(n>=0  && n < (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER]) {
          if(argc == 5) {
           Tcl_SetResult(interp, (xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][n].prop_ptr, TCL_VOLATILE);
          } else {
            tmp = 100 + strlen(argv[4]) + strlen(argv[5]);
            subtok = my_malloc(83,tmp);
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
              if( (ss = strchr(xctx->inst[inst].instname, ':')) ) {
                sscanf(ss + 1, "%d", &slot);
                if(strstr(value, ":")) value = find_nth(value, ":", slot);
              }
              Tcl_SetResult(interp, (char *)value, TCL_VOLATILE);
            }
            my_free(924, &subtok);
          }
        }
      /* xschem getprop symbol lm358.sym [type] */
      } else if( !strcmp(argv[2],"symbol")) {
        int i, found=0;
        if(argc!=5 && argc !=4 && argc !=6) {
          Tcl_SetResult(interp, "xschem getprop symbol needs 1 or 2 or 3 additional arguments", TCL_STATIC);
          return TCL_ERROR;
        }
        for(i=0; i<xctx->symbols; i++) {
          if(!xctx->x_strcmp(xctx->sym[i].name,argv[3])){
            found=1;
            break;
          }
        }
        if(!found) {
          Tcl_SetResult(interp, "Symbol not found", TCL_STATIC);
          return TCL_ERROR;
        }
        if(argc == 4)
          Tcl_SetResult(interp, xctx->sym[i].prop_ptr, TCL_VOLATILE);
        else if(argc == 5) 
          Tcl_SetResult(interp, (char *)get_tok_value(xctx->sym[i].prop_ptr, argv[4], 0), TCL_VOLATILE);
        else if(argc > 5) 
          Tcl_SetResult(interp, (char *)get_tok_value(xctx->sym[i].prop_ptr, argv[4], atoi(argv[5])), TCL_VOLATILE);
 
      } else if (!strcmp(argv[2],"rect")) {
        if(argc <=5) {
          Tcl_SetResult(interp, "xschem getprop rect needs <color> <n> <token>", TCL_STATIC);
          return TCL_ERROR;
        } else {
          int c = atoi(argv[3]);
          int n = atoi(argv[4]);
          Tcl_SetResult(interp, (char *)get_tok_value(xctx->rect[c][n].prop_ptr, argv[5], 0), TCL_VOLATILE);
        }
      }
    }
   
    else if(!strcmp(argv[1],"get_tok") )
    {
      char *s=NULL;
      int t;
      cmd_found = 1;
      if(argc < 4) {Tcl_SetResult(interp, "Missing arguments", TCL_STATIC);return TCL_ERROR;}
      if(argc == 5) t = atoi(argv[4]);
      else t = 0;
      my_strdup(648, &s, get_tok_value(argv[2], argv[3], t));
      Tcl_SetResult(interp, s, TCL_VOLATILE);
      my_free(649, &s);
    }
   
    else if(!strcmp(argv[1],"get_tok_size") )
    {
      Tcl_SetResult(interp, my_itoa((int)xctx->tok_size), TCL_VOLATILE);
    }
   
    else if(!strcmp(argv[1],"globals"))
    {
     static char res[8192];
     cmd_found = 1;

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
     for(i=0;i<8;i++)
     {
       my_snprintf(res, S(res), "rects[%d]=%d\n", i, xctx->rects[i]); Tcl_AppendResult(interp, res, NULL);
       my_snprintf(res, S(res), "lines[%d]=%d\n", i, xctx->lines[i]); Tcl_AppendResult(interp, res, NULL);
       my_snprintf(res, S(res), "maxr[%d]=%d\n", i, xctx->maxr[i]); Tcl_AppendResult(interp, res, NULL);
       my_snprintf(res, S(res), "maxl[%d]=%d\n", i, xctx->maxl[i]); Tcl_AppendResult(interp, res, NULL);
     }
     my_snprintf(res, S(res), "current_name=%s\n", xctx->current_name); Tcl_AppendResult(interp, res, NULL);
     my_snprintf(res, S(res), "currsch=%d\n", xctx->currsch); Tcl_AppendResult(interp, res, NULL);
     for(i=0;i<=xctx->currsch;i++)
     {
       my_snprintf(res, S(res), "previous_instance[%d]=%d\n",i,xctx->previous_instance[i]); Tcl_AppendResult(interp, res, NULL);
       my_snprintf(res, S(res), "sch_path[%d]=%s\n",i,xctx->sch_path[i]? xctx->sch_path[i]:"<NULL>"); Tcl_AppendResult(interp, res, NULL);
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
   
    else if(!strcmp(argv[1],"go_back"))
    {
      cmd_found = 1;
      go_back(1);
      Tcl_ResetResult(interp);
    }
  }

  else if(argv[1][0] == 'h') {   
    if(!strcmp(argv[1],"hash_file"))
    {
     unsigned int h;
     char s[40];
     cmd_found = 1;
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
   
    else if(!strcmp(argv[1],"help"))
    {
     cmd_found = 1;
     xschem_cmd_help(argc, argv);
    }
   
    else if(!strcmp(argv[1],"hier_psprint"))
    {
      cmd_found = 1;
      if(argc > 2) {
        my_strncpy(xctx->plotfile, argv[2], S(xctx->plotfile));
      }
      hier_psprint();
      Tcl_ResetResult(interp);
    }
    else if(!strcmp(argv[1],"hilight"))
    {
      cmd_found = 1;
      xctx->enable_drill = 0;
      if(argc >=3 && !strcmp(argv[2], "drill")) xctx->enable_drill = 1;
      hilight_net(0);
      redraw_hilights(0);
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"hilight_netname"))
    {
      int ret = 0;
      cmd_found = 1;
      if(argc>=3) {
        ret = hilight_netname(argv[2]);
      }
      Tcl_SetResult(interp,ret ? "1" : "0" , TCL_STATIC);
    }
  }

  else if(argv[1][0] == 'i') {   
    if(!strcmp(argv[1],"instance"))
    {
      cmd_found = 1;
      if(argc==7)
       /*           pos sym_name      x                y             rot       */
        place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), (short)atoi(argv[5]), 
               /* flip              prop draw first ito_push_undo */
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
   
    else if(!strcmp(argv[1],"instance_bbox"))
    {
      int i;
      char s[200];
      cmd_found = 1;
      for(i=0;i<xctx->instances;i++) {
        if(!strcmp(xctx->inst[i].instname, argv[2])) {
          break;
        }
      }
      if(i<xctx->instances) {
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
   
    else if(!strcmp(argv[1],"instance_net"))
    {
      /* xschem instance_net inst pin */
      int no_of_pins, i, p, multip;
      const char *str_ptr=NULL;
   
      cmd_found = 1;
      if( argc <4) {
        Tcl_SetResult(interp, "xschem instance_net needs 2 additional arguments", TCL_STATIC);
        return TCL_ERROR;
      }
      if( (i = get_instance(argv[2])) < 0 ) {
        Tcl_SetResult(interp, "xschem instance_net: instance not found", TCL_STATIC);
        return TCL_ERROR;
      }
      prepare_netlist_structs(0);
      no_of_pins= (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];
      for(p=0;p<no_of_pins;p++) {
        if(!strcmp( get_tok_value((xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][p].prop_ptr,"name",0), argv[3])) {
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
   
    else if(!strcmp(argv[1],"instance_nodemap"))
    {
    /* xschem instance_nodemap [instance_name] */
      int p, no_of_pins;
      int inst = -1;

      cmd_found = 1;
      prepare_netlist_structs(0);
      if(argc > 2) {
        inst = get_instance(argv[2]);
        if(inst >=0) {
          Tcl_AppendResult(interp,  xctx->inst[inst].instname, " ",  NULL);
          no_of_pins= (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER];
          for(p=0;p<no_of_pins;p++) {
            const char *pin;
            pin = get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][p].prop_ptr,"name",0);
            if(!pin[0]) pin = "--ERROR--";
            if(argc>=4 && strcmp(argv[3], pin)) continue;
            Tcl_AppendResult(interp, pin, " ",
                  xctx->inst[inst].node && xctx->inst[inst].node[p] ? xctx->inst[inst].node[p] : "{}", " ", NULL);
          }
        }
      }
    }
   
    else if(!strcmp(argv[1],"instance_pin_coord"))
    {
    /*   0            1           2       3     4
     * xschem instance_pin_coord m12  pinnumber 2
     * xschem instance_pin_coord U3:2 pinnumber 5
     * xschem instance_pin_coord m12 name p
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
   
      cmd_found = 1;
      if(argc < 5) {
        Tcl_SetResult(interp,
          "xschem instance_pin_coord requires an instance, a pin attribute and a value", TCL_STATIC);
        return TCL_ERROR;
      }
      i = get_instance(argv[2]);
      if(i < 0) {
        Tcl_SetResult(interp,"", TCL_STATIC);
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
      tmpstr = my_malloc(529, sizeof(xctx->inst[i].instname));
      if( (ss=strchr(xctx->inst[i].instname, ':')) ) {
        sscanf(ss+1, "%s", tmpstr);
        if(isonlydigit(tmpstr)) {
          slot = atoi(tmpstr);
        }
      }
      for(p = 0;p < no_of_pins; p++) {
        pin = get_tok_value(rct[p].prop_ptr,argv[3],0);
        if(slot > 0 && !strcmp(argv[3], "pinnumber") && strstr(pin,":")) pin = find_nth(pin, ":", slot);
        if(!strcmp(pin, argv[4])) break;
      }
      if(p >= no_of_pins) {
        Tcl_SetResult(interp,"", TCL_STATIC);
        return TCL_OK;
      }
      pinx0 = (rct[p].x1+rct[p].x2)/2;
      piny0 = (rct[p].y1+rct[p].y2)/2;
      ROTATION(rot, flip, 0.0, 0.0, pinx0, piny0, pinx0, piny0);
      pinx0 += x0;
      piny0 += y0;
      my_snprintf(num, S(num), "{%s} %g %g", get_tok_value(rct[p].prop_ptr, "name", 0), pinx0, piny0);
      Tcl_SetResult(interp, num, TCL_VOLATILE);
      my_free(530, &tmpstr);
    }
   
    else if(!strcmp(argv[1],"instance_pins"))
    {
      char *pins = NULL;
      int p, i, no_of_pins;

      cmd_found = 1;
      prepare_netlist_structs(0);
      if( (i = get_instance(argv[2])) < 0 ) {
        Tcl_SetResult(interp, "xschem instance_pins: instance not found", TCL_STATIC);
        return TCL_ERROR;
      }
      no_of_pins= (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];
      for(p=0;p<no_of_pins;p++) {
        const char *pin;
        pin = get_tok_value((xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][p].prop_ptr,"name",0);
        if(!pin[0]) pin = "--ERROR--";
        my_mstrcat(655, &pins, "{", pin, "}", NULL);
        if(p< no_of_pins-1) my_strcat(377, &pins, " ");
      }
      Tcl_SetResult(interp, pins, TCL_VOLATILE);
      my_free(1195, &pins);
    }
   
    else if(!strcmp(argv[1],"instance_pos"))
    {
      int i;
      char s[30];
      cmd_found = 1;
      for(i=0;i<xctx->instances;i++) {
        if(!strcmp(xctx->inst[i].instname, argv[2])) {
          break;
        }
      }
      if(i==xctx->instances) i = -1;
      my_snprintf(s, S(s), "%d", i);
      Tcl_SetResult(interp, s, TCL_VOLATILE);
    }
   
    else if(!strcmp(argv[1],"instances_to_net"))
    {
    /* xschem instances_to_net PLUS */
      xSymbol *symbol;
      xRect *rct;
      short flip, rot;
      double x0,y0, pinx0, piny0;
   
      char *pins = NULL;
      int p, i, no_of_pins;
      cmd_found = 1;
      prepare_netlist_structs(0);
      if(argc < 3) {
        Tcl_SetResult(interp,"xschem instances_to_net requires a net name argument", TCL_STATIC);
        return TCL_ERROR;
      }
      for(i = 0;i < xctx->instances; i++) {
        x0 = xctx->inst[i].x0;
        y0 = xctx->inst[i].y0;
        rot = xctx->inst[i].rot;
        flip = xctx->inst[i].flip;
        symbol = xctx->sym + xctx->inst[i].ptr;
        no_of_pins= symbol->rects[PINLAYER];
        rct=symbol->rect[PINLAYER];
        for(p = 0;p < no_of_pins; p++) {
          const char *pin;
          pin = get_tok_value(rct[p].prop_ptr,"name",0);
          if(!pin[0]) pin = "--ERROR--";
          if(xctx->inst[i].node[p] && !strcmp(xctx->inst[i].node[p], argv[2]) &&
             !IS_LABEL_SH_OR_PIN( (xctx->inst[i].ptr+xctx->sym)->type )) {
            my_mstrcat(534, &pins, "{ {", xctx->inst[i].instname, "} {", pin, NULL);
   
            pinx0 = (rct[p].x1+rct[p].x2)/2;
            piny0 = (rct[p].y1+rct[p].y2)/2;
            ROTATION(rot, flip, 0.0, 0.0, pinx0, piny0, pinx0, piny0);
            pinx0 += x0;
            piny0 += y0;
   
            my_mstrcat(538, &pins, "} {", dtoa(pinx0), "} {", dtoa(piny0), "} } ", NULL);
          }
        }
      }
      Tcl_SetResult(interp, pins ? pins : "", TCL_VOLATILE);
      my_free(926, &pins);
    }
  }

  else if(argv[1][0] == 'l') {   
    if(!strcmp(argv[1],"line"))
    {
      double x1,y1,x2,y2;
      int pos, save;
      cmd_found = 1;
      if(argc>=6) {
        x1=atof(argv[2]);
        y1=atof(argv[3]);
        x2=atof(argv[4]);
        y2=atof(argv[5]);
        ORDER(x1,y1,x2,y2);
        pos=-1;
        if(argc==7) pos=atoi(argv[6]);
        storeobject(pos, x1,y1,x2,y2,LINE,xctx->rectcolor,0,NULL);
        save = xctx->draw_window; xctx->draw_window = 1;
        drawline(xctx->rectcolor,NOW, x1,y1,x2,y2, 0);
        xctx->draw_window = save;
      }
      else xctx->ui_state |= MENUSTARTLINE;
    }
   
    else if(!strcmp(argv[1],"line_width") && argc==3)
    {
      cmd_found = 1;
      change_linewidth(atof(argv[2]));
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1], "list_hilights"))
    {
      const char *sep;
      cmd_found = 1;
      if(argc > 2) {
        sep = argv[2];
      } else {
        sep = "{ }";
      }
      list_hilights();
      tclvareval("join [lsort -decreasing -dictionary {", tclresult(), "}] ", sep, NULL);
    }

    else if(!strcmp(argv[1], "list_tokens") && argc == 4)
    {
      cmd_found = 1;
      Tcl_ResetResult(interp);
      Tcl_SetResult(interp, (char *)list_tokens(argv[2], atoi(argv[3])), TCL_VOLATILE);
    }
   
    else if(!strcmp(argv[1],"load") )
    {
      int load_symbols = 1, force = 0, undo_reset = 1;
      size_t i;
      cmd_found = 1;
      if(argc > 3) {
        for(i = 3; i < argc; i++) {
          if(!strcmp(argv[i], "symbol")) load_symbols = 0;
          if(!strcmp(argv[i], "force")) force = 1;
          if(!strcmp(argv[i], "noundoreset")) undo_reset = 0;
        }
      }
      if(argc>2) {
        i = strlen(argv[2]);
        if(i > 4 && !strcmp(argv[2] + i - 4, ".sym")) {
          load_symbols = 0;
        }
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
            my_strdup(375, &xctx->sch_path[xctx->currsch],".");
            xctx->sch_path_hash[xctx->currsch] = 0;
            xctx->sch_inst_number[xctx->currsch] = 1;
            zoom_full(1, 0, 1, 0.97);
          }
        }
      }
      else if(argc==2) {
        ask_new_file();
      }
      Tcl_SetResult(interp, xctx->sch[xctx->currsch], TCL_STATIC);
    }
    else if(!strcmp(argv[1],"load_new_window") )
    {
      char fullname[PATH_MAX];
      cmd_found = 1;
      if(has_x) {
        if(argc>=3) {
          my_snprintf(fullname, S(fullname),"%s", argv[2]);
        } else {
          tcleval("load_file_dialog {Load file} .sch.sym INITIALLOADDIR");
          my_snprintf(fullname, S(fullname),"%s", tclresult());
        }
        if( fullname[0] ) {
         new_schematic("create", NULL, fullname);
         tclvareval("update_recent_file {", fullname, "}", NULL);
        }
      } else {
        Tcl_ResetResult(interp);
      }
    }
    else if(!strcmp(argv[1],"log"))
    {
      cmd_found = 1;
      if(argc==3 && errfp == stderr ) { errfp = fopen(argv[2], "w"); } /* added check to avoid multiple open */
      else if(argc==2 && errfp != stderr) { fclose(errfp); errfp=stderr; }
    }
    else if(!strcmp(argv[1],"logic_set"))
    {
      int num =  1;
      cmd_found = 1;
      if(argc > 3 ) num = atoi(argv[3]);
      if(argc > 2) {
        int n = atoi(argv[2]);
        if(n == 4) n = -1;
        logic_set(n, num);
      }
      Tcl_ResetResult(interp);
    }
  }

  else if(argv[1][0] == 'm') {   
    if(!strcmp(argv[1],"make_sch")) /* make schematic from selected symbol 20171004 */
    {
      cmd_found = 1;
      create_sch_from_sym();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"make_symbol"))
    {
      cmd_found = 1;
      if(has_x) tcleval("tk_messageBox -type okcancel -parent [xschem get topwindow] "
                        "-message {do you want to make symbol view ?}");
      if(!has_x || strcmp(tclresult(),"ok")==0) {
        save_schematic(xctx->sch[xctx->currsch]);
        make_symbol();
      }
      Tcl_ResetResult(interp);
    }

    else if (!strcmp(argv[1], "make_sch_from_sel"))
    {
      cmd_found = 1;
      make_schematic_symbol_from_sel();
      Tcl_ResetResult(interp);
    }

    else if(!strcmp(argv[1],"merge"))
    {
      cmd_found = 1;
      if(argc<3) {
        merge_file(0,"");  /* 2nd param not used for merge 25122002 */
      }
      else {
        merge_file(0,argv[2]);
      }
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"move_objects"))
    {
      cmd_found = 1;
      if(argc==4) {
        move_objects(START,0,0,0);
        move_objects( END,0,atof(argv[2]), atof(argv[3]));
      }
      else move_objects(START,0,0,0);
      Tcl_ResetResult(interp);
    }
  }

  else if(argv[1][0] == 'n') {   
    if(!strcmp(argv[1],"net_label"))
    {
      cmd_found = 1;
      unselect_all(1);
      if(argc>=3) place_net_label(atoi(argv[2]));
    }
   
    else if(!strcmp(argv[1],"net_pin_mismatch"))
    {
      cmd_found = 1;
      hilight_net_pin_mismatches();
    }
   
    else if(!strcmp(argv[1],"netlist") )
    {
      cmd_found = 1;
      yyparse_error = 0;
      if( set_netlist_dir(0, NULL) ) {
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
      }
    }
   
    else if(!strcmp(argv[1],"new_schematic"))
    {
      int r = -1;
      char s[20];
      cmd_found = 1;
      if(argc == 3) r = new_schematic(argv[2], NULL, NULL);
      else if(argc == 4) r = new_schematic(argv[2], argv[3], NULL);
      else if(argc == 5) r = new_schematic(argv[2], argv[3], argv[4]);
      my_snprintf(s, S(s), "%d", r);
      Tcl_SetResult(interp, s, TCL_VOLATILE);
    }
   
    else if(!strcmp(argv[1],"new_symbol_window"))
    {
      cmd_found = 1;
      if(argc==2) new_xschem_process("",1);
      else new_xschem_process(argv[2],1);
      Tcl_ResetResult(interp);
    }

    else if(!strcmp(argv[1],"new_window"))
    {
      cmd_found = 1;
      if(argc==2) new_xschem_process("",0);
      else new_xschem_process(argv[2],0);
      Tcl_ResetResult(interp);
    }
  }

  else if(argv[1][0] == 'o') {   
    if(!strcmp(argv[1],"only_probes"))
    {
      cmd_found = 1;
      toggle_only_probes();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"origin"))
    {
      cmd_found = 1;
      if(argc == 5) xctx->zoom = atof(argv[4]);
      if(argc >= 4) {
        xctx->xorigin = atof(argv[2]);
        xctx->yorigin = atof(argv[3]);
      }
      xctx->mooz=1/xctx->zoom;
      draw();
    }
  }

  else if(argv[1][0] == 'p') {   
    if(!strcmp(argv[1],"parselabel") && argc ==3) 
    {
      cmd_found = 1;
      parse( argv[2]);
    }
   
    else if(!strcmp(argv[1],"paste"))
    {
      cmd_found = 1;
      merge_file(2,".sch");
      if(argc > 3) {
        xctx->deltax = atof(argv[2]);
        xctx->deltay = atof(argv[3]);
        move_objects(END, 0, 0.0, 0.0);
      }
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"pinlist"))
    {
      int i, p, no_of_pins;
      cmd_found = 1;
      if( (i = get_instance(argv[2])) < 0 ) {
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
   
    else if(!strcmp(argv[1],"place_symbol"))
    {
      int ret;
      cmd_found = 1;
      xctx->semaphore++;
      rebuild_selected_array();
      if(xctx->lastsel && xctx->sel_array[0].type==ELEMENT) {
        tclvareval("set INITIALINSTDIR [file dirname {",
             abs_sym_path(xctx->inst[xctx->sel_array[0].n].name, ""), "}]", NULL);
      }
      unselect_all(1);
      xctx->mx_double_save = xctx->mousex_snap;
      xctx->my_double_save = xctx->mousey_snap;
      if(argc == 4) {
        ret = place_symbol(-1,argv[2],xctx->mousex_snap, xctx->mousey_snap, 0, 0, argv[3], 4, 1, 1/*to_push_undo*/);
      } else if(argc == 3) {
        ret = place_symbol(-1,argv[2],xctx->mousex_snap, xctx->mousey_snap, 0, 0, NULL, 4, 1, 1/*to_push_undo*/);
      } else {
        xctx->last_command = 0;
        ret = place_symbol(-1,NULL,xctx->mousex_snap, xctx->mousey_snap, 0, 0, NULL, 4, 1, 1/*to_push_undo*/);
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
   
    else if(!strcmp(argv[1],"place_text"))
    {
      cmd_found = 1;

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
   
    else if(!strcmp(argv[1],"polygon"))
    {
      cmd_found = 1;
      xctx->ui_state |= MENUSTARTPOLYGON;
    }
   
    else if(!strcmp(argv[1],"preview_window"))
    {
      cmd_found = 1;
      if(argc == 3) preview_window(argv[2], "{}","{}");
      else if(argc == 4) preview_window(argv[2], argv[3], "{}");
      else if(argc == 5) preview_window(argv[2], argv[3], argv[4]);
      Tcl_ResetResult(interp);
    }
   
    /*                          img x   y size    xschem area to export
     *    0     1    2    3         4   5             6    7   8   9
     * xschem print png file.png   400 300        [ -300 -200 300 200 ]
     * xschem print svg file.svg   400 300        [ -300 -200 300 200 ]
     * xschem print ps  file.ps
     * xschem print pdf file.pdf
     */
    else if(!strcmp(argv[1],"print") ) {
      cmd_found = 1;
      if(argc < 3) {
        Tcl_SetResult(interp, "xschem print needs at least 1 more arguments: plot_type", TCL_STATIC);
        return TCL_ERROR;
      }
      if(argc >= 4) {
        tclvareval("file normalize {", argv[3], "}", NULL);
        my_strncpy(xctx->plotfile, Tcl_GetStringResult(interp), S(xctx->plotfile));
      }

      if(!strcmp(argv[2],"pdf") || !strcmp(argv[2],"ps")) {
        ps_draw(7);
      }
      else if(!strcmp(argv[2],"png")) {
        int w = 0, h = 0;
        double x1, y1, x2, y2;
        if(argc == 6) {
          w = atoi(argv[4]);
          h = atoi(argv[5]);
          save_restore_zoom(1);
          set_viewport_size(w, h, 0.8);
          zoom_full(0, 0, 2, 0.97);
          resetwin(1, 1, 1, w, h);
          print_image();
          save_restore_zoom(0);
          resetwin(1, 1, 1, 0, 0);
          change_linewidth(-1.);
          draw();
        } else if( argc == 10) {
          w = atoi(argv[4]);
          h = atoi(argv[5]);
          x1 = atof(argv[6]);
          y1 = atof(argv[7]);
          x2 = atof(argv[8]);
          y2 = atof(argv[9]);
          save_restore_zoom(1);
          set_viewport_size(w, h, 0.8);
          zoom_box(x1, y1, x2, y2, 1.0);
          resetwin(1, 1, 1, w, h);
          print_image();
          save_restore_zoom(0);
          resetwin(1, 1, 1, 0, 0);
          change_linewidth(-1.);
          draw();
        } else {
          print_image();
        }
      }
      else if(!strcmp(argv[2],"svg")) {
        int w = 0, h = 0;
        double x1, y1, x2, y2;
        if(argc == 6) {
          w = atoi(argv[4]);
          h = atoi(argv[5]);
          save_restore_zoom(1);
          set_viewport_size(w, h, 0.8);
          zoom_full(0, 0, 2, 0.97);
          svg_draw();
          save_restore_zoom(0);
        } else if( argc == 10) {
          w = atoi(argv[4]);
          h = atoi(argv[5]);
          x1 = atof(argv[6]);
          y1 = atof(argv[7]);
          x2 = atof(argv[8]);
          y2 = atof(argv[9]);
          save_restore_zoom(1);
          set_viewport_size(w, h, 0.8);
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
   
    else if(!strcmp(argv[1], "print_hilight_net") && argc == 3)
    {
      cmd_found = 1;
      print_hilight_net(atoi(argv[2]));
    }
   
    else if(!strcmp(argv[1],"print_spice_element") )
    {
      cmd_found = 1;
      if(argc>2) print_spice_element(stderr, atoi(argv[2]));
    }
   
    else if(!strcmp(argv[1],"propagate_hilights"))
    {
      int set = 1, clear = 0;
      cmd_found = 1;
      if(argc>=4) {
         set = atoi(argv[2]);
         clear = atoi(argv[3]);
      }
      propagate_hilights(set, clear, XINSERT_NOREPLACE);
    }

    else if(!strcmp(argv[1],"push_undo"))
    {
      cmd_found = 1;
      xctx->push_undo();
      Tcl_ResetResult(interp);
    }
  }

  else if(argv[1][0] == 'r') {   
    if(!strcmp(argv[1], "raw_clear"))
    {
      cmd_found = 1;
      tclsetvar("rawfile_loaded", "0");
      free_rawfile(1);
      Tcl_ResetResult(interp);
    }

    if(!strcmp(argv[1], "raw_query"))
    {
      int i;
      cmd_found = 1;
      Tcl_ResetResult(interp);
      if(argc > 2 && !strcmp(argv[2], "loaded")) {
        Tcl_SetResult(interp, schematic_waves_loaded() ? "1" : "0", TCL_STATIC);
      } else if(xctx->graph_values) {
        /* xschem rawfile_query value v(ldcp) 123 */
        if(argc > 4 && !strcmp(argv[2], "value")) {
          int dataset = -1;
          int point = atoi(argv[4]);
          const char *node = argv[3];
          int idx = -1;
          if(argc > 5) dataset = atoi(argv[5]);
          if( (dataset >= 0 && point >= 0 && point < xctx->graph_npoints[dataset]) ||
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
          Int_hashentry *entry; 
          int idx;
          entry = int_hash_lookup(xctx->graph_raw_table, argv[3], 0, XLOOKUP);
          idx = entry ? entry->value : -1;
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
          for(i = 0 ; i < xctx->graph_nvars; i++) {
            if(i > 0) Tcl_AppendResult(interp, "\n", NULL);
            Tcl_AppendResult(interp, xctx->graph_names[i], NULL);
          }
        }
      }
    }

    if(!strcmp(argv[1], "raw_read"))
    {
      cmd_found = 1;
      if(schematic_waves_loaded()) {
        free_rawfile(1);
        tclsetvar("rawfile_loaded", "0");
      } else if(argc > 2) {
        free_rawfile(0);
        read_rawfile(argv[2]);
        if(schematic_waves_loaded()) tclsetvar("rawfile_loaded", "1");
        else  tclsetvar("rawfile_loaded", "0");
      }
      Tcl_ResetResult(interp);
    }
    if(!strcmp(argv[1], "raw_read_from_attr"))
    {
      cmd_found = 1;
      if(schematic_waves_loaded()) {
        free_rawfile(1);
      } else {
        free_rawfile(0);
        read_embedded_rawfile();
        if(schematic_waves_loaded()) tclsetvar("rawfile_loaded", "1");
        else  tclsetvar("rawfile_loaded", "0");
      }
      Tcl_ResetResult(interp);
    }
    if(!strcmp(argv[1], "rebuild_connectivity"))
    {
      cmd_found = 1;
      xctx->prep_hash_inst=0;
      xctx->prep_hash_wires=0;
      xctx->prep_net_structs=0;
      xctx->prep_hi_structs=0;
      prepare_netlist_structs(1);
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"rect"))
    {
      double x1,y1,x2,y2;
      int pos, save;
      cmd_found = 1;
      if(argc>=6) {
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
   
    else if(!strcmp(argv[1],"redo"))
    {
      cmd_found = 1;
      xctx->pop_undo(1, 1); /* 2nd param: set_modify_status */
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"redraw"))
    {
      cmd_found = 1;
      draw();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"reload"))
    {
      cmd_found = 1;
      unselect_all(1);
      remove_symbols();
      load_schematic(1, xctx->sch[xctx->currsch], 1);
      if(argc >= 3 && !strcmp(argv[2], "zoom_full") ) {
        zoom_full(1, 0, 1, 0.97);
      } else {
        draw();
      }
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"reload_symbols"))
    {
      cmd_found = 1;
      remove_symbols();
      link_symbols_to_instances(-1);
      draw();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"remove_symbols"))
    {
      cmd_found = 1;
      remove_symbols();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1], "replace_symbol"))
    {
    /* xschem replace_symbol R3 capa.sym */
      int inst, fast = 0;
      cmd_found = 1;
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
      if( (inst = get_instance(argv[2])) < 0 ) {
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
        my_strdup2(369, &xctx->inst[inst].name, rel_sym_path(symbol));
        xctx->inst[inst].ptr=sym_number;
        bbox(ADD, xctx->inst[inst].x1, xctx->inst[inst].y1, xctx->inst[inst].x2, xctx->inst[inst].y2);
   
        my_strdup(370, &name, xctx->inst[inst].instname);
        if(name && name[0] )
        {
          /* 20110325 only modify prefix if prefix not NUL */
          if(prefix) name[0]=(char)prefix; /* change prefix if changing symbol type; */
   
          my_strdup(371, &ptr,subst_token(xctx->inst[inst].prop_ptr, "name", name) );
          hash_all_names(inst);
          new_prop_string(inst, ptr,0, tclgetboolvar("disable_unique_names")); /* set new prop_ptr */
          type=xctx->sym[xctx->inst[inst].ptr].type;
          cond= !type || !IS_LABEL_SH_OR_PIN(type);
          if(cond) xctx->inst[inst].flags|=2;
          else {
            xctx->inst[inst].flags &=~2;
            my_strdup(872, &xctx->inst[inst].lab, get_tok_value(xctx->inst[inst].prop_ptr, "lab", 0));
          }
          my_free(922, &ptr);
        }
        my_free(923, &name);
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
   
    else if(!strcmp(argv[1],"rotate"))
    {
      cmd_found = 1;
      if(! (xctx->ui_state & (STARTMOVE | STARTCOPY) ) ) { 
        rebuild_selected_array();
        move_objects(START,0,0,0);
        move_objects(ROTATE|ROTATELOCAL,0,0,0);
        move_objects(END,0,0,0);
      }
      Tcl_ResetResult(interp);
    }
  }

  else if(argv[1][0] == 's') {   
    if(!strcmp(argv[1],"save"))
    {
      cmd_found = 1;
      dbg(1, "scheduler(): saving: current schematic\n");
   
      if(!strcmp(xctx->sch[xctx->currsch],"")) {   /* check if unnamed schematic, use saveas in this case... */
        saveas(NULL, SCHEMATIC);
      } else {
        save(0);
      }
    }
   
    else if(!strcmp(argv[1],"saveas"))
    {
      cmd_found = 1;
      if(argc == 4) {
        const char *f;
        f = !strcmp(argv[2],"") ? NULL : argv[2];
        if(!strcmp(argv[3], "SCHEMATIC")) saveas(f, SCHEMATIC);
        else if(!strcmp(argv[3], "SYMBOL")) saveas(f, SYMBOL);
        else saveas(f, SCHEMATIC);
      }
      else if(argc == 3) {
        const char *f;
        f = !strcmp(argv[2],"") ? NULL : argv[2];
        saveas(f, SCHEMATIC);
      }
      else saveas(NULL, SCHEMATIC);
    }
   
    else if(!strcmp(argv[1],"schematic_in_new_window"))
    {
      cmd_found = 1;
      schematic_in_new_window();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"search") || !strcmp(argv[1],"searchmenu"))
    {
      /*   0      1         2        3       4   5   */
      /*                           select            */
      /* xschem search regex|exact 0|1|-1   tok val  */
      int select, r;
      cmd_found = 1;
      if(argc < 6) {
        Tcl_SetResult(interp,"xschem search requires 4 or 5 additional fields.", TCL_STATIC);
        return TCL_ERROR;
      }
      if(argc==6) {
        select = atoi(argv[3]);
        if( !strcmp(argv[2],"regex") )  r = search(argv[4],argv[5],0,select);
        else  r = search(argv[4],argv[5],1,select);
        if(r == 0) {
          if(has_x && !strcmp(argv[1],"searchmenu")) 
            tcleval("tk_messageBox -type ok -parent [xschem get topwindow] -message {Not found.}");
          Tcl_SetResult(interp,"0", TCL_STATIC);
        } else {
          Tcl_SetResult(interp,"1", TCL_STATIC);
        }
        return TCL_OK;
      }
    }
   
    else if(!strcmp(argv[1],"select"))
    {
      cmd_found = 1;
      if(argc<3) {
        Tcl_SetResult(interp, "xschem select: missing arguments.", TCL_STATIC);
        return TCL_ERROR;
      }
   
      if(!strcmp(argv[2],"instance") && argc>=4) {
        int i;
        /* find by instance name  or number*/
        i = get_instance(argv[3]);
        if(i >= 0) {
          if(argc>=5 && !strcmp(argv[4], "clear"))
            select_element(i, 0, 0, 0);
          else
            select_element(i, SELECTED, 0, 0);
        }
        Tcl_SetResult(interp, (i >= 0) ? "1" : "0" , TCL_STATIC);
      }
      else if(!strcmp(argv[2],"wire") && argc==4) {
        int n=atoi(argv[3]);
        if(n<xctx->wires && n >= 0) select_wire(atoi(argv[3]), SELECTED, 0);
      }
      else if(!strcmp(argv[2],"text") && argc==4) {
        int n=atoi(argv[3]);
        if(n<xctx->texts && n >= 0) select_text(atoi(argv[3]), SELECTED, 0);
      }
      drawtemparc(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
      drawtemprect(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
      drawtempline(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
    }
   
    else if(!strcmp(argv[1],"select_all"))
    {
      cmd_found = 1;
      select_all();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"select_hilight_net"))
    {
      cmd_found = 1;
      select_hilight_net();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"selected_set"))
    {
      int n, i;
      cmd_found = 1;
      rebuild_selected_array();
      for(n=0; n < xctx->lastsel; n++) {
        if(xctx->sel_array[n].type == ELEMENT) {
          i = xctx->sel_array[n].n;
          Tcl_AppendResult(interp, /* "{", */ xctx->inst[i].instname, " ", /* "} ", */ NULL);
        }
      }
    }
   
    else if(!strcmp(argv[1],"selected_wire"))
    {
      int n, i;
      cmd_found = 1;
      rebuild_selected_array();
      for(n=0; n < xctx->lastsel; n++) {
        if(xctx->sel_array[n].type == WIRE) {
          i = xctx->sel_array[n].n;
          Tcl_AppendResult(interp, get_tok_value(xctx->wire[i].prop_ptr,"lab",0), " ", NULL);
        }
      }
    }
   
    else if(!strcmp(argv[1],"send_to_viewer"))
    {
      int viewer = 0;
      int exists = 0;
      char *viewer_name = NULL;
      char tcl_str[200];
      
      cmd_found = 1;
      tcleval("info exists sim");
      if(tclresult()[0] == '1') exists = 1;
      xctx->enable_drill = 0;
      if(exists) {
        viewer = atoi(tclgetvar("sim(spicewave,default)"));
        my_snprintf(tcl_str, S(tcl_str), "sim(spicewave,%d,name)", viewer);
        my_strdup(1267, &viewer_name, tclgetvar(tcl_str));
        dbg(1,"send_to_viewer: viewer_name=%s\n", viewer_name);
        if(strstr(viewer_name, "Gaw")) viewer=GAW;
        else if(strstr(viewer_name, "Bespice")) viewer=BESPICE;
        if(viewer) {
          hilight_net(viewer);
          redraw_hilights(0);
        }
        my_free(1268, &viewer_name);
      }
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"set") && argc==4)
    {
      /*
       * ********** xschem  set subcommands
       */
      cmd_found = 1;
      if(!strcmp(argv[2],"cadgrid")) {
            set_grid( atof(argv[3]) );
      }
      else if(!strcmp(argv[2],"cadsnap")) {
            set_snap( atof(argv[3]) );
      }
      else if(!strcmp(argv[2],"color_ps")) {
            color_ps=atoi(argv[3]);
      }
      else if(!strcmp(argv[2],"constrained_move")) {
        constrained_move = atoi(argv[3]);
      }
      else if(!strcmp(argv[2],"draw_window")) {
         xctx->draw_window=atoi(argv[3]);
      }
      else if(!strcmp(argv[2],"flat_netlist")) {
            xctx->flat_netlist=atoi(argv[3]);
      }
      else if(!strcmp(argv[2],"format")) {
            my_strdup(1542, &xctx->format, argv[3]);
      }
      else if(!strcmp(argv[2],"hide_symbols")) {
            xctx->hide_symbols=atoi(argv[3]);
      }
      else if(!strcmp(argv[2],"netlist_name")) {
        my_strncpy(xctx->netlist_name, argv[3], S(xctx->netlist_name));
      }
      else if(!strcmp(argv[2],"netlist_type"))
      {
        if(argc > 3) {
          if(!strcmp(argv[3],"spice")){
            xctx->netlist_type=CAD_SPICE_NETLIST;
          }
          else if(!strcmp(argv[3],"vhdl")) {
            xctx->netlist_type=CAD_VHDL_NETLIST;
          }
          else if(!strcmp(argv[3],"verilog")) {
            xctx->netlist_type=CAD_VERILOG_NETLIST;
          }
          else if(!strcmp(argv[3],"tedax")) {
            xctx->netlist_type=CAD_TEDAX_NETLIST;
          }
          else if(!strcmp(argv[3],"symbol")) {
            xctx->netlist_type=CAD_SYMBOL_ATTRS;
          }
          else {
            dbg(0, "Warning: undefined netlist format: %s\n", argv[3]);
          }
          set_tcl_netlist_type();
        }
      }
      else if(!strcmp(argv[2],"no_draw")) {
        int s = atoi(argv[3]);
        xctx->no_draw=s;
      }
      else if(!strcmp(argv[2],"no_undo")) {
        int s = atoi(argv[3]);
        xctx->no_undo=s;
      }
      else if(!strcmp(argv[2],"rectcolor")) {
         xctx->rectcolor=atoi(argv[3]);
         if(xctx->rectcolor < 0 ) xctx->rectcolor = 0;
         if(xctx->rectcolor >= cadlayers ) xctx->rectcolor = cadlayers - 1;
         rebuild_selected_array();
         if(xctx->lastsel) {
           change_layer();
         }
      }
      else if(!strcmp(argv[2],"text_svg")) {
            text_svg=atoi(argv[3]);
      }
      else if(!strcmp(argv[2],"semaphore")) {
            dbg(1, "scheduler(): set semaphore to %s\n", argv[3]);
            xctx->semaphore=atoi(argv[3]);
      }
      else if(!strcmp(argv[2],"sym_txt")) {
            xctx->sym_txt=atoi(argv[3]);
      }
      else {
        Tcl_AppendResult(interp, "xschem set ", argv[1], argv[3], ": invalid command.", NULL);
        return TCL_ERROR;
      }
    }
   
    else if(!strcmp(argv[1],"set_different_tok") )
    {
      char *s = NULL;
      cmd_found = 1;
      if(argc < 5) {Tcl_SetResult(interp, "Missing arguments", TCL_STATIC);return TCL_ERROR;}
      my_strdup(459, &s, argv[2]);
      set_different_token(&s, argv[3], argv[4], 0, 0);
      Tcl_SetResult(interp, s, TCL_VOLATILE);
      my_free(1156, &s);
    }
   
    else if(!strcmp(argv[1],"set_modify"))
    {
      cmd_found = 1;
      if(argc > 2) {
        set_modify(atoi(argv[2]));
      }
      Tcl_ResetResult(interp);
    }
    /* 'fast' argument if given does not redraw and is not undoable */
    else if(!strcmp(argv[1], "setprop"))
    {
    /*   0       1       2     3    4     5    6
     * xschem setprop instance R4 value [30k] [fast] */
   
      cmd_found = 1;
      
      if(argc > 2 && !strcmp(argv[2], "instance")) {
        int inst, fast=0;
        if(argc >= 7) {
          if(!strcmp(argv[6], "fast")) {
            fast = 1;
            argc = 6;
          }
        }
        else if(argc >= 6) {
          if(!strcmp(argv[5], "fast")) {
            fast = 1;
            argc = 5;
          }
        }
        if(argc < 5) {
          Tcl_SetResult(interp, "xschem setprop instance needs 2 or 3 additional arguments", TCL_STATIC);
          return TCL_ERROR;
        }
        if( (inst = get_instance(argv[3])) < 0 ) {
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
          if(!strcmp(argv[4], "name")) hash_all_names(inst);
          if(argc >= 6) {
            new_prop_string(inst, subst_token(xctx->inst[inst].prop_ptr, argv[4], argv[5]),fast, 
              tclgetboolvar("disable_unique_names"));
          } else {/* assume argc == 5 , delete attribute */
            new_prop_string(inst, subst_token(xctx->inst[inst].prop_ptr, argv[3], NULL),fast, 
              tclgetboolvar("disable_unique_names"));
          }
          type=xctx->sym[xctx->inst[inst].ptr].type;
          cond= !type || !IS_LABEL_SH_OR_PIN(type);
          if(cond) xctx->inst[inst].flags|=2;
          else {
            xctx->inst[inst].flags &=~2;
            my_strdup(1215, &xctx->inst[inst].lab, get_tok_value(xctx->inst[inst].prop_ptr, "lab", 0));
          }
  
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
       * xschem setprop rect c n token value [fast] */
        int change_done = 0;
        int fast = 0;
        xRect *r;
        int c = atoi(argv[3]);
        int n = atoi(argv[4]);

        if (!(c>=0 && c < cadlayers && n >=0 && n < xctx->rects[c]) ) {
          Tcl_SetResult(interp, "xschem setprop rect: wrong layer or rect number", TCL_STATIC);
          return TCL_ERROR;
        }
        r = &xctx->rect[c][n];
        if(argc >= 8) {
          if(!strcmp(argv[7], "fast")) {
            fast = 1;
            argc = 7;
          }
          if(!strcmp(argv[7], "fastundo")) {
            fast = 3;
            argc = 7;
          }
        }
        else if(argc >= 7) {
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
        if(argc > 6) {
          /* verify if there is some difference */
          if(strcmp(argv[6], get_tok_value(r->prop_ptr, argv[5], 0))) {
            change_done = 1;
            if(fast == 3 || fast == 0) xctx->push_undo();
            my_strdup2(1486, &r->prop_ptr, subst_token(r->prop_ptr, argv[5], argv[6]));
          }
        } else {
          get_tok_value(r->prop_ptr, argv[5], 0);
          if(xctx->tok_size) {
            change_done = 1;
            if(fast == 3 || fast == 0) xctx->push_undo();
            my_strdup2(1478, &r->prop_ptr, subst_token(r->prop_ptr, argv[5], NULL)); /* delete attr */
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
      }
    }
    else if(!strcmp(argv[1],"show_pin_net_names")) {
      int i;
      cmd_found = 1;
      for(i = 0; i < xctx->instances; i++) {
        symbol_bbox(i, &xctx->inst[i].x1, &xctx->inst[i].y1, &xctx->inst[i].x2, &xctx->inst[i].y2);
      }
    }
    else if(!strcmp(argv[1],"simulate") )
    {
      cmd_found = 1;
      if( set_netlist_dir(0, NULL) ) {
        tcleval("simulate");
      }
    }
   
    else if(!strcmp(argv[1],"snap_wire"))
    {
      cmd_found = 1;
      xctx->ui_state |= MENUSTARTSNAPWIRE;
    }
   
    else if(!strcmp(argv[1],"subst_tok") )
    {
      char *s=NULL;
      cmd_found = 1;
      if(argc < 5) {Tcl_SetResult(interp, "Missing arguments", TCL_STATIC);return TCL_ERROR;}
      my_strdup(894, &s, subst_token(argv[2], argv[3], strcmp(argv[4], "NULL") ? argv[4] : NULL));
      Tcl_SetResult(interp, s, TCL_VOLATILE);
      my_free(1150, &s);
    }
   
    else if(!strcmp(argv[1],"symbol_in_new_window"))
    {
      cmd_found = 1;
      symbol_in_new_window();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"symbols"))
    {
      int i;
      char n[100];
      cmd_found = 1;
      Tcl_SetResult(interp, "\n", TCL_STATIC);
      for(i=0; i<xctx->symbols; i++) {
        my_snprintf(n , S(n), "%d", i);
        Tcl_AppendResult(interp, "  {", n, " ", "{", xctx->sym[i].name, "}", "}\n", NULL);
      }
      Tcl_AppendResult(interp, "\n", NULL);
    }
  }

  else if(argv[1][0] == 't') {   
    if(!strcmp(argv[1],"test"))
    {
      cmd_found = 1;
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"toggle_colorscheme"))
    {
      int d_c;
      cmd_found = 1;
      d_c = tclgetboolvar("dark_colorscheme");
      d_c = !d_c;
      tclsetboolvar("dark_colorscheme", d_c);
      tclsetdoublevar("dim_value", 0.0);
      tclsetdoublevar("dim_bg", 0.0);
      build_colors(0.0, 0.0);
      draw();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"translate") )
    {
      cmd_found = 1;
      if(argc>3) {
        Tcl_ResetResult(interp);
        Tcl_AppendResult(interp, translate(atoi(argv[2]), argv[3]), NULL);
      }
    }
   
    else if(!strcmp(argv[1],"trim_wires"))
    {
      cmd_found = 1;
      xctx->push_undo();
      trim_wires();
      draw();
      Tcl_ResetResult(interp);
    }
  }

  else if(argv[1][0] == 'u') {   
    if(!strcmp(argv[1],"undo"))
    {
      int redo = 0, set_modify = 1;
      cmd_found = 1;
      if(argc > 2) {
        redo = atoi(argv[2]);
      }
      if(argc > 3) {
        set_modify = atoi(argv[3]);
      }
      xctx->pop_undo(redo, set_modify); /* 2nd param: set_modify_status */
      Tcl_ResetResult(interp);
    }

    else if(!strcmp(argv[1],"undo_type")) {
      cmd_found = 1;
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

    else if(!strcmp(argv[1],"unhilight_all"))
    {
      xRect boundbox;
      int big =  xctx->wires> 2000 || xctx->instances > 2000 ;
      cmd_found = 1;
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
    else if(!strcmp(argv[1],"unhilight"))
    {
      cmd_found = 1;
      unhilight_net();
    }
   
    else if(!strcmp(argv[1],"unselect_all"))
    {
      cmd_found = 1;
      if(argc > 2) unselect_all(atoi(argv[2]));
      else unselect_all(1);
      Tcl_ResetResult(interp);
    }
  }

  else if(argv[1][0] == 'v') {   
    if(!strcmp(argv[1],"view_prop"))
    {
      cmd_found = 1;
      edit_property(2);
      Tcl_ResetResult(interp);
    }
  }

  else if(argv[1][0] == 'w') {   
    if(!strcmp(argv[1],"windowid")) /* used by xschem.tcl for configure events */
    {
      cmd_found = 1;
      if(argc >= 3) {
        windowid(argv[2]);
      }
    }
   
    else if(!strcmp(argv[1],"windows"))
    {
      cmd_found = 1;
      printf("top win:%lx\n", Tk_WindowId(Tk_Parent(Tk_MainWindow(interp))));
    }
   
    else if(!strcmp(argv[1],"wire"))
    {
      double x1,y1,x2,y2;
      int pos = -1, save, sel = 0;
      const char *prop=NULL;
      cmd_found = 1;
      if(argc>=6) {
        x1=atof(argv[2]);
        y1=atof(argv[3]);
        x2=atof(argv[4]);
        y2=atof(argv[5]);
        ORDER(x1,y1,x2,y2);
        if(argc >= 7) pos=atoi(argv[6]);
        if(argc >= 8) prop = argv[7];
        if(argc >= 9) sel = atoi(argv[8]);
        xctx->push_undo();
        storeobject(pos, x1,y1,x2,y2,WIRE,0,(short)sel,prop);
        xctx->prep_hi_structs=0;
        xctx->prep_net_structs=0;
        xctx->prep_hash_wires=0;
        save = xctx->draw_window; xctx->draw_window = 1;
        drawline(WIRELAYER,NOW, x1,y1,x2,y2, 0);
        xctx->draw_window = save;
        if(tclgetboolvar("autotrim_wires")) trim_wires();
      }
      else xctx->ui_state |= MENUSTARTWIRE;
    }
  }

  else if(argv[1][0] == 'z') {      
    if(!strcmp(argv[1],"zoom_box"))
    {
      double x1, y1, x2, y2, factor;
      cmd_found = 1;
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
   
    /* xschem zoom_full [center | nodraw | nolinewidth] */
    else if(!strcmp(argv[1],"zoom_full"))
    {
      int i, flags = 1;
      int draw = 1;
      double shrink = 0.97;
      char * endptr;

      cmd_found = 1;
      for(i = 2; i < argc; i++) {
        if(!strcmp(argv[i], "center")) flags  |= 2;
        else if(!strcmp(argv[i], "nodraw")) draw = 0;
        else if(!strcmp(argv[i], "nolinewidth")) flags &= ~1;
        else {
          shrink = strtod(argv[i], &endptr);
          if(endptr == argv[i]) shrink = 1.0;
        }
      }
      zoom_full(draw, 0, flags, shrink);
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"zoom_hilighted"))
    {
      cmd_found = 1;
      zoom_full(1, 2, 1, 0.97);
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"zoom_in"))
    {
      cmd_found = 1;
      view_zoom(0.0);
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"zoom_out"))
    {
      cmd_found = 1;
      view_unzoom(0.0);
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"zoom_selected"))
    {
      cmd_found = 1;
      zoom_full(1, 1, 1, 0.97);
      Tcl_ResetResult(interp);
    }
  }

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
  return atof(p);
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
  if( Tcl_GlobalEval(interp, str) != TCL_OK) {
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
  size = my_strcat(1379, &str, script);
  while( (p = va_arg(args, const char *)) ) {
    size = my_strcat(1380, &str, p);
  }
  return_code = Tcl_EvalEx(interp, str, (int)size, TCL_EVAL_GLOBAL);
  va_end(args);
  if(return_code != TCL_OK) {
    dbg(0, "tclvareval(): error executing %s: %s\n", str, tclresult());
    Tcl_ResetResult(interp);
  }
  my_free(1381, &str);
  return return_code;
}
