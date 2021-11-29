/* File: scheduler.c
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2021 Stefan Frederik Schippers
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
  if(!has_x) return;
  tclsetvar("infowindow_text", str);
  if(n==2) {
    dbg(3, "statusmsg(): n = 2, str = %s\n", str);
    tcleval("infowindow");
  }
  else {
    Tcl_VarEval(interp, xctx->top_path, ".statusbar.1 configure -text $infowindow_text", NULL);
    dbg(3, "statusmsg(str, %d): -> $infowindow_text = %s\n", n, tclgetvar("infowindow_text"));
  }
}

int get_instance(const char *s)
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
    i=atol(s);
  }
  if(i<0 || i>xctx->instances) {
    return -1;
  }
  return i;
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
       unselect_all();
       storeobject(-1, xctx->mousex_snap-2.5, xctx->mousey_snap-2.5, xctx->mousex_snap+2.5, xctx->mousey_snap+2.5,
                   xRECT, PINLAYER, SELECTED, "name=XXX\ndir=inout");
       xctx->need_reb_sel_arr=1;
       rebuild_selected_array();
       move_objects(START,0,0,0);
       xctx->ui_state |= START_SYMPIN;
       Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"align"))
    {
       cmd_found = 1;
       xctx->push_undo();
       round_schematic_to_grid(tclgetdoublevar("cadsnap"));
       if(tclgetvar("autotrim_wires")) trim_wires();
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
   
    else if(!strcmp(argv[1],"attach_pins")) /* attach pins to selected component 20171005 */
    {
      cmd_found = 1;
      attach_labels_to_inst();
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
   
    else if(!strcmp(argv[1],"build_colors"))
    {
      cmd_found = 1;
      build_colors(tclgetdoublevar("dim_value"), tclgetdoublevar("dim_bg"));
      Tcl_ResetResult(interp);
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
      int cancel;
   
      cmd_found = 1;
      cancel=save(1);
      if(!cancel){
        char name[PATH_MAX];
        struct stat buf;
        int i;
   
        xctx->currsch = 0;
        unselect_all();
        remove_symbols();
        clear_drawing();
        if(argc>=3 && !strcmp(argv[2],"SYMBOL")) {
          xctx->netlist_type = CAD_SYMBOL_ATTRS;
          set_tcl_netlist_type();
          for(i=0;;i++) {
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
          char *top_path;
          top_path =  xctx->top_path[0] ? xctx->top_path : ".";
          Tcl_VarEval(interp, "wm title ", top_path, " \"xschem - [file tail [xschem get schname]]\"", NULL);
          Tcl_VarEval(interp, "wm iconname ", top_path, " \"xschem - [file tail [xschem get schname]]\"", NULL);
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
      cmd_found = 1;
      if(argc >=3) {
        int n = atoi(argv[2]);
        descend_schematic(n);
      } else {
        descend_schematic(0);
      }
      Tcl_ResetResult(interp);
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
   
    else if(!strcmp(argv[1],"enable_layers"))
    {
      cmd_found = 1;
      enable_layers();
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"exit"))
    {
      char * top_path;
      cmd_found = 1;
      top_path =  xctx->top_path[0] ? xctx->top_path : ".";
      if(!strcmp(top_path, ".")) {
        if(has_x) {
          tcleval("new_window destroy_all"); /* close child schematics */
          if(tclresult()[0] == '1') {
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
        Tcl_VarEval(interp, "xschem new_schematic destroy ", top_path, " ", xctx->top_path, ".drw {}" , NULL);
      }
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"expandlabel") && argc == 3) 
    {
      int tmp, llen;
      char *result=NULL;
      const char *l;
   
      cmd_found = 1;
      l = expandlabel(argv[2], &tmp);
      llen = strlen(l);
      result = my_malloc(378, llen + 30);
      my_snprintf(result, llen + 30, "%s %d", l, tmp);
      Tcl_SetResult(interp, result, TCL_VOLATILE);
      my_free(927, &result);
    }
  }

  else if(argv[1][0] == 'f') {   
    if(!strcmp(argv[1],"find_nth") )
    {
      cmd_found = 1;
      if(argc>4) {
        char *r = NULL;
        my_strdup(1202, &r, find_nth(argv[2], argv[3][0], atoi(argv[4])));
        Tcl_SetResult(interp, r ? r : "<NULL>", TCL_VOLATILE);
        my_free(1203, &r);
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
      toggle_fullscreen(argv[2]);
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
    else if(!strcmp(argv[1],"get") && argc==3)
    {
     cmd_found = 1;
     if(!strcmp(argv[2],"backlayer")) {
       char s[30]; /* overflow safe 20161122 */
       my_snprintf(s, S(s), "%d",BACKLAYER);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
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
       char s[30]; /* overflow safe 20161212 */
       my_snprintf(s, S(s), "%d",cadlayers);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"color_ps")) {
       if( color_ps != 0 )
         Tcl_SetResult(interp, "1",TCL_STATIC);
       else
         Tcl_SetResult(interp, "0",TCL_STATIC);
     }
     else if(!strcmp(argv[2],"current_dirname")) {
       Tcl_SetResult(interp, xctx->current_dirname, TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"currsch")) {
       char s[30]; /* overflow safe 20161122 */
       my_snprintf(s, S(s), "%d",xctx->currsch);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"debug_var")) {
       char s[30]; /* overflow safe 20161122 */
       my_snprintf(s, S(s), "%d",debug_var);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"draw_window")) {
       char s[30]; /* overflow safe 20161122 */
       my_snprintf(s, S(s), "%d",xctx->draw_window);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"flat_netlist")) {
       if( xctx->flat_netlist != 0 )
         Tcl_SetResult(interp, "1",TCL_STATIC);
       else
         Tcl_SetResult(interp, "0",TCL_STATIC);
     }
     else if(!strcmp(argv[2],"gridlayer")) {
       char s[30]; /* overflow safe 20161122 */
       my_snprintf(s, S(s), "%d",GRIDLAYER);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"help")) {
       if( help != 0 )
         Tcl_SetResult(interp, "1",TCL_STATIC);
       else
         Tcl_SetResult(interp, "0",TCL_STATIC);
     }
     else if(!strcmp(argv[2],"instances")) {
       char s[30]; /* overflow safe 20161122 */
       my_snprintf(s, S(s), "%d",xctx->instances);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"lastsel")) {
       rebuild_selected_array();
       if( xctx->lastsel != 0 )
         Tcl_SetResult(interp, "1",TCL_STATIC);
       else
         Tcl_SetResult(interp, "0",TCL_STATIC);
     }
     else if(!strcmp(argv[2],"line_width")) {
       char s[40];
       my_snprintf(s, S(s), "%g", xctx->lw);
       Tcl_SetResult(interp, s, TCL_VOLATILE);
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
     else if(!strcmp(argv[2],"pinlayer")) {
       char s[30]; /* overflow safe 20161122 */
       my_snprintf(s, S(s), "%d",PINLAYER);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"rectcolor")) {
       char s[30]; /* overflow safe 20161122 */
       my_snprintf(s, S(s), "%d",xctx->rectcolor);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"sellayer")) {
       char s[30]; /* overflow safe 20161122 */
       my_snprintf(s, S(s), "%d",SELLAYER);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"semaphore")) {
       char s[30]; /* overflow safe 20161122 */
       my_snprintf(s, S(s), "%d",xctx->semaphore);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
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
             int slen = strlen(s);
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
       char s[30]; /* overflow safe 20161122 */
       my_snprintf(s, S(s), "%d",TEXTLAYER);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"topwindow")) {
       char *top_path;
       top_path =  xctx->top_path[0] ? xctx->top_path : ".";
       Tcl_SetResult(interp, top_path,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"version")) {
       char s[30]; /* overflow safe 20161122 */
       my_snprintf(s, S(s), "%s",XSCHEM_VERSION);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else if(!strcmp(argv[2],"wirelayer")) {
       char s[30]; /* overflow safe 20161122 */
       my_snprintf(s, S(s), "%d",WIRELAYER);
       Tcl_SetResult(interp, s,TCL_VOLATILE);
     }
     else {
       fprintf(errfp, "xschem get %s: invalid command.\n", argv[2]);
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
      char s[30];
      cmd_found = 1;
      my_snprintf(s, S(s), "%d", (int)xctx->get_tok_size);
      Tcl_SetResult(interp, s, TCL_VOLATILE);
    }
   
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
        int inst, n=-1, tmp;
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
                if(strstr(value, ":")) value = find_nth(value, ':', slot);
              }
              Tcl_SetResult(interp, (char *)value, TCL_VOLATILE);
            }
            my_free(924, &subtok);
          }
        }
      /* xschem getprop symbol lm358.sym [type] */
      } else if( !strcmp(argv[2],"symbol")) {
        int i, found=0;
        if(argc!=5 && argc !=4) {
          Tcl_SetResult(interp, "xschem getprop needs 2 or 3 additional arguments", TCL_STATIC);
          return TCL_ERROR;
        }
        for(i=0; i<xctx->symbols; i++) {
          if(!strcmp(xctx->sym[i].name,argv[3])){
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
        else
          Tcl_SetResult(interp, (char *)get_tok_value(xctx->sym[i].prop_ptr, argv[4], 0), TCL_VOLATILE);
      }
    }
   
    else if(!strcmp(argv[1],"globals"))
    {
     cmd_found = 1;
     printf("*******global variables:*******\n");
     printf("INT_WIDTH(lw)=%d\n", INT_WIDTH(xctx->lw));
     printf("wires=%d\n", xctx->wires);
     printf("instances=%d\n", xctx->instances);
     printf("symbols=%d\n", xctx->symbols);
     printf("lastsel=%d\n", xctx->lastsel);
     printf("texts=%d\n", xctx->texts);
     printf("maxt=%d\n", xctx->maxt);
     printf("maxw=%d\n", xctx->maxw);
     printf("maxi=%d\n", xctx->maxi);
     printf("maxsel=%d\n", xctx->maxsel);
     printf("zoom=%.16g\n", xctx->zoom);
     printf("xorigin=%.16g\n", xctx->xorigin);
     printf("yorigin=%.16g\n", xctx->yorigin);
     for(i=0;i<8;i++)
     {
       printf("rects[%d]=%d\n", i, xctx->rects[i]);
       printf("lines[%d]=%d\n", i, xctx->lines[i]);
       printf("maxr[%d]=%d\n", i, xctx->maxr[i]);
       printf("maxl[%d]=%d\n", i, xctx->maxl[i]);
     }
     printf("current_name=%s\n", xctx->current_name);
     printf("currsch=%d\n", xctx->currsch);
     for(i=0;i<=xctx->currsch;i++)
     {
       printf("previous_instance[%d]=%d\n",i,xctx->previous_instance[i]);
       printf("sch_path[%d]=%s\n",i,xctx->sch_path[i]? xctx->sch_path[i]:"<NULL>");
       printf("sch[%d]=%s\n",i,xctx->sch[i]);
     }
     printf("modified=%d\n", xctx->modified);
     printf("areaw=%d\n", xctx->areaw);
     printf("areah=%d\n", xctx->areah);
     printf("color_ps=%d\n", color_ps);
     printf("hilight_nets=%d\n", xctx->hilight_nets);
     printf("semaphore=%d\n", xctx->semaphore);
     printf("prep_net_structs=%d\n", xctx->prep_net_structs);
     printf("prep_hi_structs=%d\n", xctx->prep_hi_structs);
     printf("prep_hash_inst=%d\n", xctx->prep_hash_inst);
     printf("prep_hash_wires=%d\n", xctx->prep_hash_wires);
     printf("need_reb_sel_arr=%d\n", xctx->need_reb_sel_arr);
     printf("undo_type=%d\n", xctx->undo_type);
     printf("******* end global variables:*******\n");
    }
   
    else if(!strcmp(argv[1],"go_back"))
    {
      cmd_found = 1;
      go_back(1);
      Tcl_ResetResult(interp);
    }
  }

  else if(argv[1][0] == 'h') {   
    if(!strcmp(argv[1],"help"))
    {
     cmd_found = 1;
     printf("xschem : function used to communicate with the C program\n");
     printf("Usage:\n");
     printf("      xschem callback X-event_type mousex mousey Xkeysym mouse_button Xstate\n");
     printf("                   can be used to send any event to the application\n");
     printf("      xschem netlist\n");
     printf("                   generates a netlist in the selected format for the current schematic\n");
     printf("      xschem simulate\n");
     printf("                   launches the currently set simulator on the generated netlist\n");
     printf("      xschem redraw\n");
     printf("                   Redraw the window\n");
     printf("      xschem new_window library/cell\n");
     printf("                   start a new window optionally with specified cell\n");
     printf("      xschem schematic_in_new_window \n");
     printf("                   start a new window with selected element schematic\n");
     printf("      xschem symbol_in_new_window \n");
     printf("                   start a new window with selected element schematic\n");
     printf("      xschem globals\n");
     printf("                   print information about global variables\n");
     printf("      xschem inst_ptr n\n");
     printf("                   return inst_ptr of inst[n]\n");
     printf("      xschem netlist\n");
     printf("                   perform a global netlist on current schematic\n");
     printf("      xschem netlist_type type\n");
     printf("                   set netlist type to <type>, currently spice, vhdl, verilog or tedax\n");
     printf("      xschem save [library/name]\n");
     printf("                   save current schematic, optionally a lib/name can be given\n");
     printf("      xschem saveas\n");
     printf("                   save current schematic, asking for a filename\n");
     printf("      xschem load library/cell\n");
     printf("                   load specified cell from library\n");
     printf("      xschem load_symbol library/cell\n");
     printf("                   load specified cell symbol view  from library\n");
     printf("      xschem reload\n");
     printf("                   reload current cell from library\n");
     printf("      xschem instance library/cell x y rot flip [property string]\n");
     printf("                   place instance cell of the given library at x,y, rot, flip\n");
     printf("                   can also be given a property string\n");
     printf("      xschem rect x1 y1 x2 y2 [pos]\n");
     printf("                   place rectangle, optionally at pos (position in database)\n");
     printf("      xschem line x1 y1 x2 y2 [pos]\n");
     printf("                   place line, optionally at pos (position in database)\n");
     printf("      xschem wire x1 y1 x2 y2 [pos]\n");
     printf("                   place wire, optionally at pos (position in database)\n");
     printf("      xschem select instance|wire|text n\n");
     printf("                   select instance or text or wire number n\n");
     printf("      xschem select_all\n");
     printf("                   select all objects\n");
     printf("      xschem descend\n");
     printf("                   descend into schematic of selected element\n");
     printf("      xschem descend_symbol\n");
     printf("                   descend into symbol of selected element\n");
     printf("      xschem go_back\n");
     printf("                   back from selected element\n");
     printf("      xschem unselect\n");
     printf("                   unselect selected objects\n");
     printf("      xschem zoom_out\n");
     printf("                   zoom out\n");
     printf("      xschem zoom_in\n");
     printf("                   zoom in\n");
     printf("      xschem zoom_full\n");
     printf("                   zoom full\n");
     printf("      xschem zoom_box\n");
     printf("                   zoom box\n");
     printf("      xschem paste\n");
     printf("                   paste selection from clipboard\n");
     printf("      xschem merge\n");
     printf("                   merge external file into current schematic\n");
     printf("      xschem cut\n");
     printf("                   cut selection to clipboard\n");
     printf("      xschem copy\n");
     printf("                   copy selection to clipboard\n");
     printf("      xschem copy_objects\n");
     printf("                   duplicate selected objects\n");
     printf("      xschem move_objects [deltax deltay]\n");
     printf("                   move selected objects\n");
     printf("      xschem line_width n\n");
     printf("                   set line width to (float) n\n");
     printf("      xschem delete\n");
     printf("                   delete selected objects\n");
     printf("      xschem unhilight\n");
     printf("                   unlight selected nets/pins\n");
     printf("      xschem hilight\n");
     printf("                   hilight selected nets/pins\n");
     printf("      xschem clear_hilights\n");
     printf("                   unhilight  all nets/pins\n");
     printf("      xschem print [color]\n");
     printf("                   print schematic (optionally in color)\n");
     printf("      xschem search regex|exact <select> token value\n");
     printf("                   hilight instances which match tok=val property,\n");
     printf("                   exact search or regex\n");
     printf("                   select: 0-> highlight, 1-> select, -1-> unselect\n");
     printf("      xschem log file\n");
     printf("                   open a log file to write messages to\n");
     printf("      xschem get variable\n");
     printf("                   return  global variable\n");
     printf("      xschem set variable value\n");
     printf("                   set global variable\n");
     printf("      xschem clear\n");
     printf("                   clear current schematic\n");
     printf("      xschem exit\n");
     printf("                   exit program gracefully\n");
     printf("      xschem view_prop\n");
     printf("                   view properties of currently selected element\n");
     printf("      xschem edit_prop\n");
     printf("                   edit properties of currently selected element\n");
     printf("      xschem edit_vi_prop\n");
     printf("                   edit properties of currently selected element in a vim window\n");
     printf("      xschem place_symbol\n");
     printf("                   place new symbol, asking filename\n");
     printf("      xschem make_symbol\n");
     printf("                   make symbol view from current schematic\n");
     printf("      xschem make_sch_from_sel\n");
     printf("                   make schematic view from selected components\n");
     printf("      xschem place_text\n");
     printf("                   place new text\n");
     printf("      xschem debug  n\n");
     printf("                   set debug level to n: 1, 2, 3 for C Program \n");
     printf("                                        -1,-2,-3 for Tcl frontend\n");
     #ifndef __unix__
     printf("      xschem temp_dir\n");
     printf("                   get a valid temp folder path\n");
     #endif
    }
   
    else if(!strcmp(argv[1],"hier_psprint"))
    {
      cmd_found = 1;
      hier_psprint();
      Tcl_ResetResult(interp);
    }
    else if(!strcmp(argv[1],"hilight"))
    {
      cmd_found = 1;
      xctx->enable_drill = 0;
      if(argc >=3 && !strcmp(argv[2], "drill")) xctx->enable_drill = 1;
      hilight_net(0);
      /* draw_hilight_net(1); */
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
       /*           pos sym_name      x                y             rot          flip      prop draw first */
        place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]),NULL, 3, 1, 1/*to_push_undo*/);
      else if(argc==8)
        place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), argv[7], 3, 1, 1/*to_push_undo*/);
      else if(argc==9) {
        int x = !(atoi(argv[8]));
        place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), argv[7], 0, x, 1/*to_push_undo*/);
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
      char *pins = NULL;
      int p, i, no_of_pins;
      int inst = -1;

      cmd_found = 1;
      prepare_netlist_structs(0);
      if(argc>=3) inst = get_instance(argv[2]);
      for(i=0;i<xctx->instances;i++) {
        if(inst>=0 && i != inst) continue;
        my_strcat(573, &pins, "{ {");
        my_strcat(574, &pins,  xctx->inst[i].instname);
        my_strcat(575, &pins, "} ");
        no_of_pins= (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];
        for(p=0;p<no_of_pins;p++) {
          const char *pin;
          pin = get_tok_value((xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][p].prop_ptr,"name",0);
          if(!pin[0]) pin = "--ERROR--";
          if(argc>=4 && strcmp(argv[3], pin)) continue;
          my_strcat(576, &pins, "{ ");
          my_strcat(655, &pins, "{");
          my_strcat(662, &pins, pin);
          my_strcat(663, &pins, "} {");
          my_strcat(664, &pins, xctx->inst[i].node[p] ? xctx->inst[i].node[p] : "");
          my_strcat(665, &pins, "} ");
          my_strcat(1155, &pins, "} ");
        }
        my_strcat(1188, &pins, "} ");
      }
      Tcl_SetResult(interp, pins, TCL_VOLATILE);
      my_free(1189, &pins);
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
        if(slot > 0 && !strcmp(argv[3], "pinnumber") && strstr(pin,":")) pin = find_nth(pin, ':', slot);
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
        my_strcat(376, &pins, "{");
        my_strcat(523, &pins, pin);
        my_strcat(533, &pins, "}");
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
      char num[40];
   
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
            my_strcat(534, &pins, "{ {");
            my_strcat(535, &pins,  xctx->inst[i].instname);
            my_strcat(536, &pins, "} {");
            my_strcat(537, &pins, pin);
   
            pinx0 = (rct[p].x1+rct[p].x2)/2;
            piny0 = (rct[p].y1+rct[p].y2)/2;
            ROTATION(rot, flip, 0.0, 0.0, pinx0, piny0, pinx0, piny0);
            pinx0 += x0;
            piny0 += y0;
   
            my_strcat(538, &pins, "} {");
            my_snprintf(num, S(num), "%g", pinx0);
            my_strcat(539, &pins, num);
            my_strcat(540, &pins, "} {");
            my_snprintf(num, S(num), "%g", piny0);
            my_strcat(541, &pins, num);
            my_strcat(542, &pins, "} } ");
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
        if(argc==7) pos=atol(argv[6]);
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
   
    else if(!strcmp(argv[1], "list_tokens") && argc == 4)
    {
      cmd_found = 1;
      Tcl_ResetResult(interp);
      Tcl_SetResult(interp, (char *)list_tokens(argv[2], atoi(argv[3])), TCL_VOLATILE);
    }
   
    else if(!strcmp(argv[1],"load") )
    {
      cmd_found = 1;
      tcleval("catch { ngspice::resetdata }");
      if(argc==3) {
        if(!has_x || !xctx->modified  || !save(1) ) { /* save(1)==1 --> user cancel */
          dbg(1, "scheduler(): load: filename=%s\n", argv[2]);
          clear_all_hilights();
          xctx->currsch = 0;
          unselect_all();
          remove_symbols();
          load_schematic(1, abs_sym_path(argv[2], ""), 1);
          Tcl_VarEval(interp, "update_recent_file {", abs_sym_path(argv[2], ""), "}", NULL);
          my_strdup(375, &xctx->sch_path[xctx->currsch],".");
          xctx->sch_path_hash[xctx->currsch] = 0;
          xctx->sch_inst_number[xctx->currsch] = 1;
          zoom_full(1, 0, 1, 0.97);
        }
      }
      else if(argc==2) {
        ask_new_file();
      }
    }
    else if(!strcmp(argv[1],"load_new_window") )
    {
      char fullname[PATH_MAX];

      cmd_found = 1;
      tcleval("catch { ngspice::resetdata }");
      if(argc>=3) {
        my_snprintf(fullname, S(fullname),"%s", argv[2]);
      } else {
        tcleval("load_file_dialog {Load Schematic} .sch.sym INITIALLOADDIR");
        my_snprintf(fullname, S(fullname),"%s", tclresult());
      }
      if( fullname[0] ) {
       Tcl_VarEval(interp, "new_window create ", fullname, NULL);
       Tcl_VarEval(interp, "update_recent_file {", fullname, "}", NULL);
      }
    }
   
    else if(!strcmp(argv[1], "load_symbol")) 
    { 
      int save, missing = 0;
      cmd_found = 1;
      if(argc > 2) { 
         save = xctx->symbols;
         match_symbol(argv[2]);
         if( xctx->symbols != save && !strcmp( xctx->sym[xctx->symbols - 1].type, "missing") ) {
           missing = 1;
           remove_symbol( xctx->symbols - 1);
         }
      }
      Tcl_SetResult(interp, missing ? "0" : "1", TCL_STATIC);
    }
   
    else if(!strcmp(argv[1],"load_symbol") )
    {
      cmd_found = 1;
      if(argc==3) {
        dbg(1, "scheduler(): load: filename=%s\n", argv[2]);
        clear_all_hilights();
        xctx->currsch = 0;
        unselect_all();
        remove_symbols();
        /* load_symbol(argv[2]); */
        load_schematic(0, abs_sym_path(argv[2], ""), 1);
        my_strdup(374, &xctx->sch_path[xctx->currsch],".");
        xctx->sch_path_hash[xctx->currsch] = 0;
        xctx->sch_inst_number[xctx->currsch] = 1;
        zoom_full(1, 0, 1, 0.97);
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
      if(has_x) tcleval("tk_messageBox -type okcancel -message {do you want to make symbol view ?}");
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
          if(has_x) tcleval("tk_messageBox -type ok -message {Please Set netlisting mode (Options menu)}");
      }
    }
   
    else if(!strcmp(argv[1],"new_schematic"))
    {
      cmd_found = 1;
      if(argc == 4) new_schematic(argv[2], argv[3], "{}","{}");
      else if(argc == 5) new_schematic(argv[2], argv[3], argv[4], "{}");
      else if(argc == 6) new_schematic(argv[2], argv[3], argv[4], argv[5]);
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"new_symbol_window"))
    {
      cmd_found = 1;
      if(argc==2) new_window("",1);
      else new_window(argv[2],1);
      Tcl_ResetResult(interp);
    }

    else if(!strcmp(argv[1],"new_window"))
    {
      cmd_found = 1;
      if(argc==2) new_window("",0);
      else new_window(argv[2],0);
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
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1],"pinlist"))
    {
      int i, p, no_of_pins;
      cmd_found = 1;
      if( (i = get_instance(argv[2])) < 0 ) {
        Tcl_SetResult(interp, "xschem getprop: instance not found", TCL_STATIC);
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
      xctx->mx_double_save = xctx->mousex_snap;
      xctx->my_double_save = xctx->mousey_snap;
      if(argc == 4) {
        ret = place_symbol(-1,argv[2],xctx->mousex_snap, xctx->mousey_snap, 0, 0, argv[3], 4, 1, 1/*to_push_undo*/);
      } else if(argc == 3) {
        ret = place_symbol(-1,argv[2],xctx->mousex_snap, xctx->mousey_snap, 0, 0, NULL, 4, 1, 1/*to_push_undo*/);
      } else {
        xctx->last_command = 0;
        rebuild_selected_array();
        if(xctx->lastsel && xctx->sel_array[0].type==ELEMENT) {
          Tcl_VarEval(interp, "set INITIALINSTDIR [file dirname {",
             abs_sym_path(xctx->inst[xctx->sel_array[0].n].name, ""), "}]", NULL);
        } 
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
        Tcl_VarEval(interp, "file normalize \"", argv[3], "\"", NULL);
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
        if(argc==7) pos=atol(argv[6]);
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
      unselect_all();
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
      char *newname = NULL;
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
        my_strdup2(510, &newname, name);
        if(name && name[0] )
        {
          /* 20110325 only modify prefix if prefix not NUL */
          if(prefix) name[0]=prefix; /* change prefix if changing symbol type; */
   
          my_strdup(371, &ptr,subst_token(xctx->inst[inst].prop_ptr, "name", name) );
          hash_all_names(inst);
          new_prop_string(inst, ptr,0, tclgetboolvar("disable_unique_names")); /* set new prop_ptr */
          my_strdup2(517, &newname, get_tok_value(xctx->inst[inst].prop_ptr, "name",0));
          my_strdup2(372, &xctx->inst[inst].instname, newname);
   
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
        Tcl_SetResult(interp, newname , TCL_VOLATILE);
        my_free(528, &newname);
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
            tcleval("tk_messageBox -type ok -message {Not found.}");
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
        int n=atol(argv[3]);
        if(n<xctx->wires && n >= 0) select_wire(atol(argv[3]), SELECTED, 0);
      }
      else if(!strcmp(argv[2],"text") && argc==4) {
        int n=atol(argv[3]);
        if(n<xctx->texts && n >= 0) select_text(atol(argv[3]), SELECTED, 0);
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
      char *res = NULL;
      cmd_found = 1;
      rebuild_selected_array();
      for(n=0; n < xctx->lastsel; n++) {
        if(xctx->sel_array[n].type == ELEMENT) {
          i = xctx->sel_array[n].n;
          my_strcat(1191, &res, "{");
          my_strcat(1192, &res, xctx->inst[i].instname);
          my_strcat(645, &res, "}");
          if(n < xctx->lastsel-1) my_strcat(646, &res, " ");
        }
      }
      Tcl_SetResult(interp, res, TCL_VOLATILE);
      my_free(925, &res);
    }
   
    else if(!strcmp(argv[1],"selected_wire"))
    {
      int n, i;
      char *res = NULL;
      cmd_found = 1;
      rebuild_selected_array();
      for(n=0; n < xctx->lastsel; n++) {
        if(xctx->sel_array[n].type == WIRE) {
          i = xctx->sel_array[n].n;
          my_strcat(434, &res, get_tok_value(xctx->wire[i].prop_ptr,"lab",0));
          if(n < xctx->lastsel-1) my_strcat(442, &res, " ");
        }
      }
      Tcl_SetResult(interp, res, TCL_VOLATILE);
      my_free(453, &res);
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
        viewer = atol(tclgetvar("sim(spicewave,default)"));
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
            tclsetvar("netlist_type", "spice");
          }
          else if(!strcmp(argv[3],"vhdl")) {
            xctx->netlist_type=CAD_VHDL_NETLIST;
            tclsetvar("netlist_type", "vhdl");
          }
          else if(!strcmp(argv[3],"verilog")) {
            xctx->netlist_type=CAD_VERILOG_NETLIST;
            tclsetvar("netlist_type", "verilog");
          }
          else if(!strcmp(argv[3],"tedax")) {
            xctx->netlist_type=CAD_TEDAX_NETLIST;
            tclsetvar("netlist_type", "tedax");
          }
          else if(!strcmp(argv[3],"symbol")) {
            xctx->netlist_type=CAD_SYMBOL_ATTRS;
            tclsetvar("netlist_type", "symbol");
          }
          else {
            tclsetvar("netlist_type", "unknown");
          }
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
         rebuild_selected_array();
         if(xctx->lastsel) {
           change_layer();
         }
      }
      else if(!strcmp(argv[2],"text_svg")) {
            text_svg=atoi(argv[3]);
      }
      else if(!strcmp(argv[2],"semaphore")) {
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
      if(argc == 3 && argv[2][0] == '0') set_modify(0);
      set_modify(1);
      Tcl_ResetResult(interp);
    }
   
    else if(!strcmp(argv[1], "setprop"))
    {
    /*   0       1    2   3      4     5
     * xschem setprop R4 value [30k] [fast] */
      int inst, fast=0;
   
      cmd_found = 1;
      if(argc >= 6) {
        if(!strcmp(argv[5], "fast")) {
          fast = 1;
          argc = 5;
        }
      }
      else if(argc >= 5) {
        if(!strcmp(argv[4], "fast")) {
          fast = 1;
          argc = 4;
        }
      }
      if(argc < 4) {
        Tcl_SetResult(interp, "xschem setprop needs 2 or 3 additional arguments", TCL_STATIC);
        return TCL_ERROR;
      }
      if( (inst = get_instance(argv[2])) < 0 ) {
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
        if(!strcmp(argv[3], "name")) hash_all_names(inst);
        if(argc >= 5) {
          new_prop_string(inst, subst_token(xctx->inst[inst].prop_ptr, argv[3], argv[4]),fast, 
            tclgetboolvar("disable_unique_names"));
        } else {/* assume argc == 4 , delete attribute */
          new_prop_string(inst, subst_token(xctx->inst[inst].prop_ptr, argv[3], NULL),fast, 
            tclgetboolvar("disable_unique_names"));
        }
        my_strdup2(367, &xctx->inst[inst].instname, get_tok_value(xctx->inst[inst].prop_ptr, "name",0));

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
      }
      Tcl_SetResult(interp, xctx->inst[inst].instname , TCL_VOLATILE);
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
      unselect_all();
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
      int pos, save;
      const char *prop;
      cmd_found = 1;
      if(argc>=6) {
        x1=atof(argv[2]);
        y1=atof(argv[3]);
        x2=atof(argv[4]);
        y2=atof(argv[5]);
        ORDER(x1,y1,x2,y2);
        pos=-1;
        if(argc >= 7) pos=atol(argv[6]);
        if(argc == 8) prop = argv[7];
        else prop = NULL;
        xctx->push_undo();
        storeobject(pos, x1,y1,x2,y2,WIRE,0,0,prop);
        xctx->prep_hi_structs=0;
        xctx->prep_net_structs=0;
        xctx->prep_hash_wires=0;
        save = xctx->draw_window; xctx->draw_window = 1;
        drawline(WIRELAYER,NOW, x1,y1,x2,y2, 0);
        xctx->draw_window = save;
        if(tclgetvar("autotrim_wires")) trim_wires();
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
  return atof(Tcl_GetVar(interp,s, TCL_GLOBAL_ONLY));
}

int tclgetintvar(const char *s)
{
  return atoi(Tcl_GetVar(interp,s, TCL_GLOBAL_ONLY));
}

int tclgetboolvar(const char *s)
{
  return Tcl_GetVar(interp,s, TCL_GLOBAL_ONLY)[0] == '1' ? 1 : 0;
}

const char *tclgetvar(const char *s)
{
  return Tcl_GetVar(interp,s, TCL_GLOBAL_ONLY);
}

const char *tcleval(const char str[])
{
  if( Tcl_GlobalEval(interp, str) != TCL_OK) {
    fprintf(errfp, "tcleval(): evaluation of script: %s failed\n", str);
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


