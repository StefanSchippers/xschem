/* File: scheduler.c
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

void statusmsg(char str[],int n)
{
 static char s[2048];

 if(!has_x) return;
 tclsetvar("infowindow_text", str);
 if(n==2)
 {
  dbg(3, "statusmsg(): n = 2, s = %s\n", s);
  tcleval("infowindow");
 }
 else
 {
  my_snprintf(s, S(s), ".statusbar.1 configure -text $infowindow_text", str);
  dbg(3, "statusmsg(): n = %d, %s\n", n, s);
  dbg(3, "statusmsg(): -> $infowindow_text = %s\n", tclgetvar("infowindow_text"));

  tcleval(s);
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
       Tcl_AppendResult(interp, "Index out of range", NULL);
       return -1;
     }
     return i;
}


/* can be used to reach C functions from the Tk shell. */
int xschem(ClientData clientdata, Tcl_Interp *interp, int argc, const char * argv[])
{
 int i;
 char name[1024]; /* overflow safe 20161122 */

 Tcl_ResetResult(interp);
 if(argc<2) {
   Tcl_AppendResult(interp, "Missing arguments.", NULL);
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
  * ********** xschem commands
  */

 if(!strcmp(argv[1],"callback") )
 {
  callback( atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), (KeySym)atol(argv[5]),
            atoi(argv[6]), atoi(argv[7]), atoi(argv[8]) );
  Tcl_ResetResult(interp);

 }

 else if(!strcmp(argv[1],"set_netlist_dir") && argc==3) {
   dbg(1, "scheduler(): xschem set_netlist_dir: argv[2] = %s\n", argv[2]);
   my_strdup(0, &netlist_dir, argv[2]);
 }

 else if(!strcmp(argv[1],"copy"))
 {
   rebuild_selected_array();
   save_selection(2);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"change_colors"))
 {
   build_colors(color_dim);
   draw();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"toggle_colorscheme"))
 {
   dark_colorscheme=!dark_colorscheme;
   tclsetvar("dark_colorscheme", dark_colorscheme ? "1" : "0");
   color_dim=0.0;
   build_colors(color_dim);
   draw();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"color_dim"))
 {
   double d;

   if(argc==3) {
     d = atof(argv[2]);
     build_colors(d);
     color_dim = d;
     draw();
     Tcl_ResetResult(interp);
   }
 }

 else if(!strcmp(argv[1],"cut"))
 {
   rebuild_selected_array();
   save_selection(2);
   delete();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"only_probes")) {
   toggle_only_probes();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"fullscreen"))
 {
   dbg(1, "scheduler(): xschem fullscreen, fullscreen=%d\n", fullscreen);
   toggle_fullscreen();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"windowid")) /* used by xschem.tcl for configure events */
 {
   windowid();
 }

 else if(!strcmp(argv[1],"preview_window"))
 {
   if(argc == 3) preview_window(argv[2], "{}","{}");
   else if(argc == 4) preview_window(argv[2], argv[3], "{}");
   else if(argc == 5) preview_window(argv[2], argv[3], argv[4]);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"paste"))
 {
   merge_file(2,".sch");
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"merge"))
 {
   if(argc<3) {
     merge_file(0,"");  /* 2nd param not used for merge 25122002 */
   }
   else {
     merge_file(0,argv[2]);
   }
   Tcl_ResetResult(interp);
 }
 else if(!strcmp(argv[1],"attach_pins")) /* attach pins to selected component 20171005 */
 {
   attach_labels_to_inst();
   Tcl_ResetResult(interp);
 }
 else if(!strcmp(argv[1],"make_sch")) /* make schematic from selected symbol 20171004 */
 {
   create_sch_from_sym();
   Tcl_ResetResult(interp);
 }
 else if(!strcmp(argv[1],"add_symbol_pin")) {
    unselect_all();
    storeobject(-1, mousex_snap-2.5, mousey_snap-2.5, mousex_snap+2.5, mousey_snap+2.5,
                xRECT, PINLAYER, SELECTED, "name=XXX\ndir=inout");
    need_rebuild_selected_array=1;
    rebuild_selected_array();
    move_objects(START,0,0,0);
    ui_state |= START_SYMPIN;
    Tcl_ResetResult(interp);
 }
 else if(!strcmp(argv[1],"make_symbol"))
 {
   if(has_x) tcleval("tk_messageBox -type okcancel -message {do you want to make symbol view ?}");
   if(!has_x || strcmp(tclresult(),"ok")==0) {
     save_schematic(xctx->sch[xctx->currsch]);
     make_symbol();
   }
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"descend"))
 {
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
   descend_symbol();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"go_back"))
 {
   go_back(1);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"zoom_full"))
 {
   zoom_full(1, 0);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"zoom_selected"))
 {
   zoom_full(1, 1);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"zoom_hilighted"))
 {
   zoom_full(1, 2);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"zoom_box"))
 {
   double x1, y1, x2, y2, yy1, factor;
   dbg(1, "scheduler(): xschem zoom_box: argc=%d, argv[2]=%s\n", argc, argv[2]);
   if(argc==6 || argc == 7) {
     x1 = atof(argv[2]);
     y1 = atof(argv[3]);
     x2 = atof(argv[4]);
     y2 = atof(argv[5]);
     if(argc == 7) factor = atof(argv[6]);
     else          factor = 1.;
     if(factor == 0.) factor = 1.;
     RECTORDER(x1,y1,x2,y2);
     xctx->xorigin=-x1;xctx->yorigin=-y1;
     xctx->zoom=(x2-x1)/(areaw-4*INT_WIDTH(xctx->lw));
     yy1=(y2-y1)/(areah-4*INT_WIDTH(xctx->lw));
     if(yy1>xctx->zoom) xctx->zoom=yy1;
     xctx->mooz=1/xctx->zoom;
     xctx->xorigin=xctx->xorigin+areaw*xctx->zoom*(1-1/factor)/2;
     xctx->yorigin=xctx->yorigin+areah*xctx->zoom*(1-1/factor)/2;
     xctx->zoom*= factor;
     xctx->mooz=1/xctx->zoom;
     change_linewidth(-1.);
     draw();
   }
   else {
     ui_state |=MENUSTARTZOOM;
   }
   Tcl_ResetResult(interp);
 }
 else if(!strcmp(argv[1], "load_symbol")) 
 { 
   int save, missing = 0;
   if(argc > 2) { 
      save = xctx->symbols;
      match_symbol(argv[2]);
      if( xctx->symbols != save && !strcmp( xctx->sym[xctx->symbols - 1].type, "missing") )  {
        missing = 1;
        remove_symbol( xctx->symbols - 1);
      }
   }
   Tcl_ResetResult(interp);
   Tcl_AppendResult(interp, missing ? "0" : "1", NULL);
 }
 else if(!strcmp(argv[1],"place_symbol"))
 {
   int ret;
   semaphore++;
   mx_double_save = mousex_snap;
   my_double_save = mousey_snap;
   if(argc == 4) {
     ret = place_symbol(-1,argv[2],mousex_snap, mousey_snap, 0, 0, argv[3], 4, 1);
   } else if(argc == 3) {
     ret = place_symbol(-1,argv[2],mousex_snap, mousey_snap, 0, 0, NULL, 4, 1);
   } else {
     ret = place_symbol(-1,NULL,mousex_snap, mousey_snap, 0, 0, NULL, 4, 1);
   }

   if(ret) {
     mousey_snap = my_double_save;
     mousex_snap = mx_double_save;
     move_objects(START,0,0,0);
     ui_state |= PLACE_SYMBOL;
   }
   semaphore--;
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"place_text"))
 {
   ui_state |= MENUSTARTTEXT;
   /* place_text(0,mousex_snap, mousey_snap); */
   /* move_objects(START,0,0,0); */
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"zoom_out"))
 {
   view_unzoom(0.0);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"line_width") && argc==3)
 {
   change_linewidth(atof(argv[2]));
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"select_all"))
 {
   select_all();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"zoom_in"))
 {
   view_zoom(0.0);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"copy_objects"))
 {
   copy_objects(START);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"move_objects"))
 {
   if(argc==4) {
     move_objects(START,0,0,0);
     move_objects( END,0,atof(argv[2]), atof(argv[3]));
   }
   else move_objects(START,0,0,0);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"log"))
 {
   static int opened=0;
   if(argc==3 && opened==0  )  { errfp = fopen(argv[2], "w");opened=1; } /* added check to avoid multiple open */
   else if(argc==2 && opened==1) { fclose(errfp); errfp=stderr;opened=0; }
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"exit"))
 {
   if(modified && has_x) {
     tcleval("tk_messageBox -type okcancel -message {UNSAVED data: want to exit?}");
     if(strcmp(tclresult(),"ok")==0) tcleval( "exit");
   }
   else tcleval( "exit");
   Tcl_ResetResult(interp);
 }
 else if(!strcmp(argv[1], "rebuild_connectivity")) {
   prepared_hash_instances=0;
   prepared_hash_wires=0;
   prepared_netlist_structs=0;
   prepared_hilight_structs=0;
   prepare_netlist_structs(0);
   Tcl_ResetResult(interp);
 }
 else if(!strcmp(argv[1],"clear"))
 {
   int cancel;

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
       netlist_type = CAD_SYMBOL_ATTRS;
       tclsetvar("netlist_type","symbol");
       for(i=0;;i++) {
         if(i == 0) my_snprintf(name, S(name), "%s.sym", "untitled");
         else my_snprintf(name, S(name), "%s-%d.sym", "untitled", i);
         if(stat(name, &buf)) break;
       }
       my_snprintf(xctx->sch[xctx->currsch], S(xctx->sch[xctx->currsch]), "%s/%s", pwd_dir, name);
       my_strncpy(xctx->current_name, name, S(xctx->current_name));
     } else {
       netlist_type = CAD_SPICE_NETLIST;
       tclsetvar("netlist_type","spice");
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
     prepared_hash_instances=0;
     prepared_hash_wires=0;
     prepared_netlist_structs=0;
     prepared_hilight_structs=0;
     if(has_x) {
       tcleval( "wm title . \"xschem - [file tail [xschem get schname]]\""); /* 20150417 set window and icon title */
       tcleval( "wm iconname . \"xschem - [file tail [xschem get schname]]\"");
     }
   }
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"set_modify"))
 {
   set_modify(1);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"reload"))
 {
   unselect_all();
   remove_symbols();
   load_schematic(1, xctx->sch[xctx->currsch], 1);
   if(argc >= 3 && !strcmp(argv[2], "zoom_full") ) {
     zoom_full(1, 0);
   } else {
     draw();
   }
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"debug"))
 {
   if(argc==3)  {
      debug_var=atoi(argv[2]);
      tclsetvar("tcl_debug",argv[2]);
   }
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1], "bbox")) {
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

 else if(!strcmp(argv[1], "setprop")) {
   int inst, fast=0;

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
     Tcl_AppendResult(interp, "xschem setprop needs 2 or 3 additional arguments", NULL);
     return TCL_ERROR;
   }
   if( (inst = get_instance(argv[2])) < 0 ) {
     Tcl_AppendResult(interp, "xschem setprop: instance not found", NULL);
     return TCL_ERROR;
   } else {
     if(!fast) {
       bbox(START,0.0,0.0,0.0,0.0);
       symbol_bbox(inst, &xctx->inst[inst].x1, &xctx->inst[inst].y1, &xctx->inst[inst].x2, &xctx->inst[inst].y2);
       bbox(ADD, xctx->inst[inst].x1, xctx->inst[inst].y1, xctx->inst[inst].x2, xctx->inst[inst].y2);
       push_undo();
     }
     set_modify(1);
     prepared_hash_instances=0;
     prepared_netlist_structs=0;
     prepared_hilight_structs=0;
     if(!strcmp(argv[3], "name")) hash_all_names(inst);
     if(argc >= 5) {
       new_prop_string(inst, subst_token(xctx->inst[inst].prop_ptr, argv[3], argv[4]),fast, dis_uniq_names);
     } else {/* assume argc == 4 */
       new_prop_string(inst, subst_token(xctx->inst[inst].prop_ptr, argv[3], NULL),fast, dis_uniq_names);
     }
     my_strdup2(367, &xctx->inst[inst].instname, get_tok_value(xctx->inst[inst].prop_ptr, "name",0));

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
   Tcl_ResetResult(interp);

 } else if(!strcmp(argv[1], "replace_symbol")) {
   int inst, fast = 0;
   if(argc == 6) {
     if(!strcmp(argv[5], "fast")) {
       fast = 1;
       argc = 5;
     }
   }
   if(argc!=5) {
     Tcl_AppendResult(interp, "xschem replace_symbol needs 3 additional arguments", NULL);
     return TCL_ERROR;
   }
   if(!strcmp(argv[2],"instance")) {
     if( (inst = get_instance(argv[3])) < 0 ) {
       Tcl_AppendResult(interp, "xschem replace_symbol: instance not found", NULL);
       return TCL_ERROR;
     } else {
       char symbol[PATH_MAX];
       int sym_number, prefix, cond;
       char *type;
       char *name=NULL;
       char *ptr=NULL;

       bbox(START,0.0,0.0,0.0,0.0);
       my_strncpy(symbol, argv[4], S(symbol));
       push_undo();
       set_modify(1);
       if(!fast) {
         prepared_hash_instances=0;
         prepared_netlist_structs=0;
         prepared_hilight_structs=0;
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
       my_strdup(369, &xctx->inst[inst].name,symbol);
       xctx->inst[inst].ptr=sym_number;
       bbox(ADD, xctx->inst[inst].x1, xctx->inst[inst].y1, xctx->inst[inst].x2, xctx->inst[inst].y2);

       my_strdup(370, &name, xctx->inst[inst].instname);
       if(name && name[0] )
       {
         /* 20110325 only modify prefix if prefix not NUL */
         if(prefix) name[0]=prefix; /* change prefix if changing symbol type; */

         my_strdup(371, &ptr,subst_token(xctx->inst[inst].prop_ptr, "name", name) );
         hash_all_names(inst);
         new_prop_string(inst, ptr,0, dis_uniq_names); /* set new prop_ptr */
         my_strdup2(372, &xctx->inst[inst].instname, get_tok_value(xctx->inst[inst].prop_ptr, "name",0));

         type=xctx->sym[xctx->inst[inst].ptr].type;
         cond= !type || !IS_LABEL_SH_OR_PIN(type);
         if(cond) xctx->inst[inst].flags|=2;
         else xctx->inst[inst].flags &=~2;
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
     }
   }
   Tcl_ResetResult(interp);
 }

 else if( !strcmp(argv[1],"symbols")) {
   int i;
   char n[100];
   Tcl_AppendResult(interp, "\n", NULL);
   for(i=0; i<xctx->symbols; i++) {
     my_snprintf(n , S(n), "%d", i);
     Tcl_AppendResult(interp, "  {", n, " ", "{", xctx->sym[i].name, "}", "}\n", NULL);
   }
   Tcl_AppendResult(interp, "\n", NULL);
 }

 else if( !strcmp(argv[1],"getprop")) {
   if( argc > 2 && !strcmp(argv[2], "instance")) {
     int i;
     const char *tmp;
     if(argc!=5 && argc !=4) {
       Tcl_AppendResult(interp, "'xschem getprop instance' needs 1 or 2 additional arguments", NULL);
       return TCL_ERROR;
     }
     if( (i = get_instance(argv[3])) < 0 ) {
       Tcl_AppendResult(interp, "xschem getprop: instance not found", NULL);
       return TCL_ERROR;
     }
     if(argc == 4) {
       Tcl_AppendResult(interp, xctx->inst[i].prop_ptr, NULL);
     } else if(!strcmp(argv[4],"cell::name")) {
       tmp = xctx->inst[i].name;
       Tcl_AppendResult(interp, tmp, NULL);
     } else if(strstr(argv[4], "cell::") ) {
       tmp = get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, argv[4]+6, 0);
       dbg(1, "scheduler(): xschem getprop: looking up instance %d prop cell::|%s| : |%s|\n", i, argv[4]+6, tmp);
       Tcl_AppendResult(interp, tmp, NULL);
     } else {
       Tcl_AppendResult(interp, get_tok_value(xctx->inst[i].prop_ptr, argv[4], 0), NULL);
     }
   } else if(argc > 2 && !strcmp(argv[2], "instance_pin")) {
     /*   0       1        2         3   4       5     */
     /* xschem getprop instance_pin X10 PLUS pin_attr  */
     /* xschem getprop instance_pin X10  1   pin_attr  */
     int inst, n=-1, tmp;
     char *subtok=NULL;
     const char *value=NULL;
     if(argc != 6 && argc != 5) {
       Tcl_AppendResult(interp, "xschem getprop instance_pin needs 2 or 3 additional arguments", NULL);
       return TCL_ERROR;
     }

     if( (inst = get_instance(argv[3])) < 0 ) {
       Tcl_AppendResult(interp, "xschem getprop: instance not found", NULL);
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
        Tcl_AppendResult(interp, (xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][n].prop_ptr, NULL);
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
           Tcl_AppendResult(interp, value, NULL);
         }
         my_free(924, &subtok);
       }
     }
   } else if( !strcmp(argv[2],"symbol")) {
     int i, found=0;
     if(argc!=5 && argc !=4) {
       Tcl_AppendResult(interp, "xschem getprop needs 2 or 3 additional arguments", NULL);
       return TCL_ERROR;
     }
     for(i=0; i<xctx->symbols; i++) {
       if(!strcmp(xctx->sym[i].name,argv[3])){
         found=1;
         break;
       }
     }
     if(!found) {
       Tcl_AppendResult(interp, "Symbol not found", NULL);
       return TCL_ERROR;
     }
     if(argc == 4)
       Tcl_AppendResult(interp, xctx->sym[i].prop_ptr, NULL);
     else
       Tcl_AppendResult(interp, get_tok_value(xctx->sym[i].prop_ptr, argv[4], 0), NULL);
   }
 } else if(!strcmp(argv[1],"pinlist")) {
   int i, p, no_of_pins;
   if( (i = get_instance(argv[2])) < 0 ) {
     Tcl_AppendResult(interp, "xschem getprop: instance not found", NULL);
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
 } else if(!strcmp(argv[1],"instance_net")) {
   int no_of_pins, i, p, mult;
   const char *str_ptr=NULL;

   if( argc <4) {
     Tcl_AppendResult(interp, "xschem instance_net needs 2 additional arguments", NULL);
     return TCL_ERROR;
   }
   if( (i = get_instance(argv[2])) < 0 ) {
     Tcl_AppendResult(interp, "xschem instance_net: instance not found", NULL);
     return TCL_ERROR;
   }
   prepare_netlist_structs(0);
   no_of_pins= (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];
   for(p=0;p<no_of_pins;p++) {
     if(!strcmp( get_tok_value((xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][p].prop_ptr,"name",0), argv[3])) {
       str_ptr =  net_name(i,p,&mult, 0, 1);
       break;
     }
   } /* /20171029 */
   if(p>=no_of_pins) {
     Tcl_AppendResult(interp, "Pin not found", NULL);
     return TCL_ERROR;
   }
   Tcl_AppendResult(interp, str_ptr, NULL);
 } else if(!strcmp(argv[1],"selected_set")) {
   int n, i;
   char *res = NULL;
   rebuild_selected_array();
   for(n=0; n < lastselected; n++) {
     if(selectedgroup[n].type == ELEMENT) {
       i = selectedgroup[n].n;
       my_strcat(645, &res, xctx->inst[i].instname);
       if(n < lastselected-1) my_strcat(646, &res, " ");
     }
   }
   Tcl_AppendResult(interp, res, NULL);
   my_free(925, &res);
 } else if(!strcmp(argv[1],"select")) {
   if(argc<3) {
     Tcl_AppendResult(interp, "xschem select: missing arguments.", NULL);
     return TCL_ERROR;
   }

   if(!strcmp(argv[2],"instance") && argc==4) {
     char *endptr;
     int i,found=0;
     int n;
     n=strtol(argv[3], &endptr, 10);

     /* 20171006 find by instance name */
     for(i=0;i<xctx->instances;i++) {
       if(!strcmp(xctx->inst[i].instname, argv[3])) {
         select_element(i, SELECTED, 0, 0);
         found=1;
         break;
       }
     }
     if(!found && !(endptr == argv[3]) && n<xctx->instances && n >= 0) {
       select_element(n, SELECTED, 0, 0);
       found = 1;
     }
     if(!found) {
       Tcl_ResetResult(interp);
       Tcl_AppendResult(interp, "xschem select instance: instance not found", NULL);
       return TCL_ERROR;
     }

   }
   else if(!strcmp(argv[2],"wire") && argc==4) {
     int n=atol(argv[3]);
     if(n<xctx->wires && n >= 0) select_wire(atol(argv[3]), SELECTED, 0);
   }
   else if(!strcmp(argv[2],"text") && argc==4) {
     int n=atol(argv[3]);
     if(n<xctx->texts && n >= 0) select_text(atol(argv[3]), SELECTED, 0);
   }
   drawtemparc(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
   drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
   drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
 } else if(!strcmp(argv[1],"instance")) {
   if(argc==7)
     place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]),NULL, 3, 1);
   else if(argc==8)
     place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), argv[7], 3, 1);
   else if(argc==9) {
     int x = !(atoi(argv[8]));
     place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), argv[7], 0, x);
   }
 } else if(!strcmp(argv[1],"arc")) {
   ui_state |= MENUSTARTARC;
 } else if(!strcmp(argv[1],"circle")) {
   ui_state |= MENUSTARTCIRCLE;
 } else if(!strcmp(argv[1],"snap_wire")) {
   ui_state |= MENUSTARTSNAPWIRE;
 } else if(!strcmp(argv[1],"wire")) {
   double x1,y1,x2,y2;
   int pos, save;
   const char *prop;
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
     push_undo();
     storeobject(pos, x1,y1,x2,y2,WIRE,0,0,prop);
     prepared_hilight_structs=0;
     prepared_netlist_structs=0;
     prepared_hash_wires=0;

     save = draw_window; draw_window = 1;
     drawline(WIRELAYER,NOW, x1,y1,x2,y2, 0);
     draw_window = save;
   }
   else ui_state |= MENUSTARTWIRE;
 } else if(!strcmp(argv[1],"line")) {
   double x1,y1,x2,y2;
   int pos, save;
   if(argc>=6) {
     x1=atof(argv[2]);
     y1=atof(argv[3]);
     x2=atof(argv[4]);
     y2=atof(argv[5]);
     ORDER(x1,y1,x2,y2);
     pos=-1;
     if(argc==7) pos=atol(argv[6]);
     storeobject(pos, x1,y1,x2,y2,LINE,rectcolor,0,NULL);
     save = draw_window; draw_window = 1;
     drawline(rectcolor,NOW, x1,y1,x2,y2, 0);
     draw_window = save;
   }
   else ui_state |= MENUSTARTLINE;
 } else if(!strcmp(argv[1],"rect")) {
   double x1,y1,x2,y2;
   int pos, save;
   if(argc>=6) {
     x1=atof(argv[2]);
     y1=atof(argv[3]);
     x2=atof(argv[4]);
     y2=atof(argv[5]);
     ORDER(x1,y1,x2,y2);
     pos=-1;
     if(argc==7) pos=atol(argv[6]);
     storeobject(pos, x1,y1,x2,y2,xRECT,rectcolor,0,NULL);
     save = draw_window; draw_window = 1;
     drawrect(rectcolor,NOW, x1,y1,x2,y2, 0);
     draw_window = save;
   }
   else ui_state |= MENUSTARTRECT;
 } else if(!strcmp(argv[1],"polygon")) {
   ui_state |= MENUSTARTPOLYGON;
 } else if(!strcmp(argv[1],"align")) {
    push_undo();
    round_schematic_to_grid(cadsnap);
    set_modify(1);
    prepared_hash_instances=0;
    prepared_hash_wires=0;
    prepared_netlist_structs=0;
    prepared_hilight_structs=0;
    draw();
 } else if(!strcmp(argv[1],"saveas")) {
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
 } else if(!strcmp(argv[1],"save")) {
   dbg(1, "scheduler(): saving: current schematic\n");

   if(!strcmp(xctx->sch[xctx->currsch],"")) {   /* 20170622 check if unnamed schematic, use saveas in this case... */
     saveas(NULL, SCHEMATIC);
   } else {
     save(0);
   }
 } else if(!strcmp(argv[1],"windows")) {
  printf("top win:%lx\n", Tk_WindowId(Tk_Parent(Tk_MainWindow(interp))));
 } else if(!strcmp(argv[1],"globals")) {
  printf("*******global variables:*******\n");
  printf("netlist_dir=%s\n", netlist_dir? netlist_dir: "<NULL>");
  printf("INT_WIDTH(xctx->lw)=%d\n", INT_WIDTH(xctx->lw));
  printf("wires=%d\n", xctx->wires);
  printf("instances=%d\n", xctx->instances);
  printf("symbols=%d\n", xctx->symbols);
  printf("lastselected=%d\n", lastselected);
  printf("texts=%d\n", xctx->texts);
  printf("maxt=%d\n", xctx->maxt);
  printf("maxw=%d\n", xctx->maxw);
  printf("maxi=%d\n", xctx->maxi);
  printf("max_selected=%d\n", max_selected);
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
  printf("modified=%d\n", modified);
  printf("color_ps=%d\n", color_ps);
  printf("a3page=%d\n", a3page);
  printf("hilight_nets=%d\n", hilight_nets);
  printf("need_rebuild_selected_array=%d\n", need_rebuild_selected_array);
  printf("******* end global variables:*******\n");
 } else if(!strcmp(argv[1],"help")) {
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
  printf("      xschem netlist_show yes|no\n");
  printf("                   show or not netlist in a window\n");
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

 else if(!strcmp(argv[1],"netlist") ) {
    yyparse_error = 0;
    if( set_netlist_dir(0, NULL) ) {
      if(netlist_type == CAD_SPICE_NETLIST)
        global_spice_netlist(1);                  /* 1 means global netlist */
      else if(netlist_type == CAD_VHDL_NETLIST)
        global_vhdl_netlist(1);
      else if(netlist_type == CAD_VERILOG_NETLIST)
        global_verilog_netlist(1);
      else if(netlist_type == CAD_TEDAX_NETLIST)
        global_tedax_netlist(1);
      else
        if(has_x) tcleval("tk_messageBox -type ok -message {Please Set netlisting mode (Options menu)}");
    }
 }

 else if(!strcmp(argv[1],"simulate") ) {
   if( set_netlist_dir(0, NULL) ) {
     tcleval("simulate");
   }
 }

 else if(!strcmp(argv[1],"create_plot_cmd") ) {
   if(argc>2) {
     if(!strcmp(argv[2], "gaw")) {
       create_plot_cmd(GAW);
     } else {
       create_plot_cmd(NGSPICE);
     }
   } else {
     create_plot_cmd(NGSPICE);
   }
 }

 else if(!strcmp(argv[1], "print_hilight_net") && argc == 3) {
   print_hilight_net(atoi(argv[2]));
 }

 else if(!strcmp(argv[1], "display_hilights")) {
   char *str = NULL;
   display_hilights(&str);
   Tcl_ResetResult(interp);
   Tcl_AppendResult(interp, str, NULL);
   my_free(1161, &str);
 }

 else if(!strcmp(argv[1],"clear_netlist_dir") ) {
   my_strdup(373, &netlist_dir, "");
 }

 else if(!strcmp(argv[1],"edit_file") ) {
    rebuild_selected_array();
    if(lastselected==0 ) {
      save_schematic(xctx->sch[xctx->currsch]); /* sync data with disk file before editing file */
      my_snprintf(name, S(name), "edit_file {%s}",
          abs_sym_path(xctx->sch[xctx->currsch], ""));
      tcleval(name);
    }
    else if(selectedgroup[0].type==ELEMENT) {
      my_snprintf(name, S(name), "edit_file {%s}",
          abs_sym_path(xctx->inst[selectedgroup[0].n].name, ""));
      tcleval(name);

    }
 }

 else if(!strcmp(argv[1],"print") ) { /* 20171022 added png, svg */
   if(argc >= 4) {
     my_strncpy(plotfile, argv[3], S(plotfile));
   } else {
     plotfile[0] = '\0';
   }
   if(argc==2 || (argc>=3 && !strcmp(argv[2],"pdf")) ) {
     ps_draw();
   }
   else if(argc>=3 && !strcmp(argv[2],"png") ) {
     print_image();
   }
   else if(argc>=3 && !strcmp(argv[2],"svg") ) {
     svg_draw();
   }
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"translate") )
 {
   if(argc>3) {
     Tcl_ResetResult(interp);
     Tcl_AppendResult(interp, translate(atoi(argv[2]), argv[3]), NULL);
   }
 }
 else if(!strcmp(argv[1],"print_spice_element") )
 {
   if(argc>2) print_spice_element(stderr, atoi(argv[2]));
 }
 else if(!strcmp(argv[1],"subst_tok") )
 {
   char *s=NULL;
   Tcl_ResetResult(interp);
   if(argc < 5) {Tcl_AppendResult(interp, "Missing arguments", NULL);return TCL_ERROR;}
   my_strdup(894, &s, subst_token(argv[2], argv[3], strcmp(argv[4], "NULL") ? argv[4] : NULL));
   Tcl_AppendResult(interp, s, NULL);
   my_free(1150, &s);
 }

 else if(!strcmp(argv[1],"get_tok_size") )
 {
   char s[30];
   my_snprintf(s, S(s), "%d", (int)get_tok_size);
   Tcl_ResetResult(interp);
   Tcl_AppendResult(interp, s, NULL);
 }
 else if(!strcmp(argv[1],"set_different_tok") )
 {
   char *s = NULL;
   Tcl_ResetResult(interp);
   if(argc < 5) {Tcl_AppendResult(interp, "Missing arguments", NULL);return TCL_ERROR;}
   my_strdup(459, &s, argv[2]);
   set_different_token(&s, argv[3], argv[4], 0, 0);
   Tcl_AppendResult(interp, s, NULL);
   my_free(1156, &s);
 }
 else if(!strcmp(argv[1],"get_tok") )
 {
   char *s=NULL;
   int t;
   Tcl_ResetResult(interp);
   if(argc < 4) {Tcl_AppendResult(interp, "Missing arguments", NULL);return TCL_ERROR;}
   if(argc == 5) t = atoi(argv[4]);
   else t = 0;
   my_strdup(648, &s, get_tok_value(argv[2], argv[3], t));
   Tcl_AppendResult(interp, s, NULL);
   my_free(649, &s);
 }
 else if(!strcmp(argv[1],"load_symbol") )
 {
    if(argc==3) {
      dbg(1, "scheduler(): load: filename=%s\n", argv[2]);
      delete_hilight_net();
      xctx->currsch = 0;
      unselect_all();
      remove_symbols();
      /* load_symbol(argv[2]); */
      load_schematic(0, abs_sym_path(argv[2], ""), 1);
      my_strdup(374, &xctx->sch_path[xctx->currsch],".");
      xctx->sch_inst_number[xctx->currsch] = 1;
      zoom_full(1, 0);
    }
 }

 else if(!strcmp(argv[1],"load") )
 {
    tcleval("catch { ngspice::resetdata }");
    if(argc==3) {
      if(!has_x || !modified  || !save(1) ) { /* save(1)==1 --> user cancel */
        dbg(1, "scheduler(): load: filename=%s\n", argv[2]);
        delete_hilight_net();
        xctx->currsch = 0;
        unselect_all();
        remove_symbols();
        load_schematic(1, abs_sym_path(argv[2], ""), 1);
        Tcl_VarEval(interp, "update_recent_file {", abs_sym_path(argv[2], ""), "}", NULL);
        my_strdup(375, &xctx->sch_path[xctx->currsch],".");
        xctx->sch_inst_number[xctx->currsch] = 1;
        zoom_full(1, 0);
      }
    }
    else if(argc==2) {
      ask_new_file();
    }
 }

 else if(!strcmp(argv[1],"test"))
 {
  /*XSetWMHints(display, topwindow, hints_ptr); */
 }

 else if(!strcmp(argv[1],"redraw"))
 {
   draw();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"break_wires"))
 {
   break_wires_at_pins();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"push_undo"))
 {
   push_undo();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"check_unique_names")) {
   if(!strcmp(argv[2],"1")) {
     check_unique_names(1);
   } else {
     check_unique_names(0);
   }
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"new_window"))
 {
   if(argc==2) new_window("",0);
   else new_window(argv[2],0);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"symbol_in_new_window"))
 {
   symbol_in_new_window();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"schematic_in_new_window"))
 {
   schematic_in_new_window();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"netlist_type"))
 {
  if(!strcmp(argv[2],"vhdl")) {
    netlist_type=CAD_VHDL_NETLIST;
    tclsetvar("netlist_type",argv[2]);
  }
  else if(!strcmp(argv[2],"verilog")) {
    netlist_type=CAD_VERILOG_NETLIST;
    tclsetvar("netlist_type",argv[2]);
  }
  else if(!strcmp(argv[2],"tedax")) {
    netlist_type=CAD_TEDAX_NETLIST;
    tclsetvar("netlist_type",argv[2]);
  }
  else if(!strcmp(argv[2],"symbol")) {
    netlist_type=CAD_SYMBOL_ATTRS;
    tclsetvar("netlist_type",argv[2]);
  }
  else if(!strcmp(argv[2],"spice")){
    netlist_type=CAD_SPICE_NETLIST;
    tclsetvar("netlist_type",argv[2]);
  }
 }

 else if(!strcmp(argv[1],"unselect_all"))
 {
   unselect_all();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"clear_drawing"))
 {
   if(argc==2) clear_drawing();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"remove_symbols"))
 {
   if(argc==2) remove_symbols();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"view_prop"))
 {
   edit_property(2);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"undo"))
 {
   pop_undo(0);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"redo"))
 {
   pop_undo(1);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"edit_prop"))
 {
   edit_property(0);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"edit_vi_prop"))
 {
   edit_property(1);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"trim_wires"))
 {
   push_undo();
   trim_wires();
   draw();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"delete"))
 {
   if(argc==2) delete();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"enable_layers"))
 {
   enable_layers();
   Tcl_ResetResult(interp);
 }
 else if(argc == 4 && !strcmp(argv[1], "list_tokens")) {
   Tcl_ResetResult(interp);
   Tcl_AppendResult(interp, list_tokens(argv[2], atoi(argv[3])), NULL);
 }

 else if(!strcmp(argv[1],"select_hilight_net"))
 {
   select_hilight_net();
 }
 else if(!strcmp(argv[1],"unhilight"))
 {
   xRect boundbox;
   int big =  xctx->wires> 2000 || xctx->instances > 2000 ;
   enable_drill=0;
   if(!big) calc_drawing_bbox(&boundbox, 2);
   delete_hilight_net();
   /* undraw_hilight_net(1); */
   if(!big) {
     bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
     bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
     bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
   }
   draw();
   if(!big) bbox(END , 0.0 , 0.0 , 0.0 , 0.0);

   /*
   enable_drill = 0;
   unhilight_net();
   draw();
   */
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"hilight"))
 {
   enable_drill = 0;
   hilight_net(0);
   /* draw_hilight_net(1); */
   redraw_hilights();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"hilight_netname"))
 {
   int ret = 0;
   if(argc>=3) {
     ret = hilight_netname(argv[2]);
   }
   /* 
    * Tcl_ResetResult(interp);
    * Tcl_AppendResult(interp,ret ? "1" : "0" , NULL); */
    Tcl_SetResult(interp,ret ? "1" : "0" , TCL_STATIC);
 }

 else if(!strcmp(argv[1],"flip"))
 {
   if(! (ui_state & (STARTMOVE | STARTCOPY) ) ) { 
     rebuild_selected_array();
     move_objects(START,0,0,0);
     move_objects(FLIP|ROTATELOCAL,0,0,0);
     move_objects(END,0,0,0);
   } 
   Tcl_ResetResult(interp);
 }
 else if(!strcmp(argv[1],"delete_files"))
 {
   delete_files();
 }
 else if(!strcmp(argv[1],"rotate"))
 {


   if(! (ui_state & (STARTMOVE | STARTCOPY) ) ) { 
     rebuild_selected_array();
     move_objects(START,0,0,0);
     move_objects(ROTATE|ROTATELOCAL,0,0,0);
     move_objects(END,0,0,0);
   } 
   Tcl_ResetResult(interp);
 }
 else if(!strcmp(argv[1],"net_label"))
 {
   if(argc>=3) place_net_label(atoi(argv[2]));
 }
 else if(!strcmp(argv[1],"net_pin_mismatch"))
 {
   hilight_net_pin_mismatches();
 }
 else if(!strcmp(argv[1],"send_to_gaw"))
 {
   enable_drill = 0;
   hilight_net(GAW);
   /* draw_hilight_net(1); */
   redraw_hilights();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"clear_hilights"))
 {
   delete_hilight_net();
   draw();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"search") || !strcmp(argv[1],"searchmenu"))
 {
   /*   0      1         2        3       4   5      6(opt)   */
   /*                           select                        */
   /* xschem search regex|exact 0|1|-1   tok val  ADD/END/NOW */
  int select, what, r;
  what = NOW;
  if(argc < 6) {
    Tcl_SetResult(interp,"xschem search requires 4 or 5 additional fields.", TCL_STATIC);
    return TCL_ERROR;
  }
  if(argc == 7) {
    if(!strcmp(argv[6], "ADD")) what = ADD;
    else if(!strcmp(argv[6], "END")) what = END;
    else if(!strcmp(argv[6], "NOW")) what = NOW;
    argc = 6;
  }
  if(argc==6) {
    select = atoi(argv[3]);
    if( !strcmp(argv[2],"regex") )  r = search(argv[4],argv[5],0,select, what);
    else  r = search(argv[4],argv[5],1,select, what);
    Tcl_ResetResult(interp);
    if(r == 0) {
      if(has_x && !strcmp(argv[1],"searchmenu")) tcleval("tk_messageBox -type ok -message {Not found.}");
      Tcl_SetResult(interp,"0", TCL_STATIC);
    } else {
      Tcl_SetResult(interp,"1", TCL_STATIC);
    }
    return TCL_OK;
  }
 }
 else if(!strcmp(argv[1],"instance_bbox")) {
   int i;
   char s[200];
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
	(xctx->inst[i].ptr+ xctx->sym)->maxy
              );
   Tcl_AppendResult(interp, s, NULL);
   }
 }

 else if(!strcmp(argv[1],"instance_pos")) {
   int i;
   char s[30];
   for(i=0;i<xctx->instances;i++) {
     if(!strcmp(xctx->inst[i].instname, argv[2])) {
       break;
     }
   }
   if(i==xctx->instances) i = -1;
   my_snprintf(s, S(s), "%d", i);
   Tcl_AppendResult(interp, s, NULL);
 }
 else if(!strcmp(argv[1],"instance_pins")) {
   char *pins = NULL;
   int p, i, no_of_pins;
   prepare_netlist_structs(0);

   if( (i = get_instance(argv[2])) < 0 ) {
     Tcl_AppendResult(interp, "xschem instance_pins: instance not found", NULL);
     return TCL_ERROR;
   }
   no_of_pins= (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];
   for(p=0;p<no_of_pins;p++) {
     const char *pin;
     pin = get_tok_value((xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][p].prop_ptr,"name",0);
     if(!pin[0]) pin = "--ERROR--";
     my_strcat(376, &pins, pin);
     if(p< no_of_pins-1) my_strcat(377, &pins, " ");
   }
   Tcl_AppendResult(interp, pins, NULL);
   my_free(926, &pins);
 }

 /*
  * ********** xschem get subcommands
  */

 /* 20171010 allows to retrieve name of n-th parent schematic */
 else if(argc >= 3 && !strcmp(argv[1],"get") && !strcmp(argv[2],"schname") ) {
   int x;
   if(argc == 4) x = atoi(argv[3]);
   else x = xctx->currsch;
   if(x<0 && xctx->currsch+x>=0) {
     Tcl_AppendResult(interp, xctx->sch[xctx->currsch+x], NULL);
   } else if(x<=xctx->currsch) {
     Tcl_AppendResult(interp, xctx->sch[x], NULL);
   }
 }
 else if(argc >=3 && !strcmp(argv[1],"get") && !strcmp(argv[2],"sch_path"))  {
   int x;
   if(argc == 4) x = atoi(argv[3]);
   else x = xctx->currsch;
   if(x<0 && xctx->currsch+x>=0) {
     Tcl_AppendResult(interp, xctx->sch_path[xctx->currsch+x], NULL);
   } else if(x<=xctx->currsch) {
     Tcl_AppendResult(interp, xctx->sch_path[x], NULL);
   }
 }
 else if(argc == 4 && !strcmp(argv[1],"get") && !strcmp(argv[2],"expandlabel"))  {
   int tmp, llen;
   char *result=NULL;
   const char *l;

   l = expandlabel(argv[3], &tmp);
   llen = strlen(l);
   result = my_malloc(378, llen + 30);
   my_snprintf(result, llen + 30, "%s %d", l, tmp);
   Tcl_AppendResult(interp, result, NULL);
   my_free(927, &result);
 }
 else if(!strcmp(argv[1],"get") && argc==3)
 {
  if(!strcmp(argv[2],"incr_hilight"))  {
     if( incr_hilight != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"auto_hilight"))  {
     if( auto_hilight != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"netlist_show"))  {
     if( netlist_show != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"show_pin_net_names"))  {
     if( show_pin_net_names != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"flat_netlist"))  {
     if( flat_netlist != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"split_files"))  {
     if( split_files != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"bbox_selected"))  {
    xRect boundbox;
    char res[2048];
    calc_drawing_bbox(&boundbox, 1);
    my_snprintf(res, S(res), "%g %g %g %g", boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
    Tcl_AppendResult(interp, res, NULL);
  }
  else if(!strcmp(argv[2],"bbox_hilighted"))  {
    xRect boundbox;
    char res[2048];
    calc_drawing_bbox(&boundbox, 2);
    my_snprintf(res, S(res), "%g %g %g %g", boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
    Tcl_AppendResult(interp, res, NULL);
  }
  else if(!strcmp(argv[2],"enable_stretch"))  {
     if( enable_stretch != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"help"))  {
     if( help != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"color_ps"))  {
     if( color_ps != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"only_probes"))  {
     if( only_probes != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"event_reporting"))  {
     if( event_reporting != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"a3page"))  {
     if( a3page != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"draw_grid"))  {
     if( draw_grid != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"text_svg"))  {
     if( text_svg != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"sym_txt"))  {
     if( sym_txt != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"cadsnap_default"))  {
        Tcl_AppendResult(interp, tclgetvar("snap"),NULL);
  }
  else if(!strcmp(argv[2],"user_top_netl_name"))  {
    Tcl_AppendResult(interp, user_top_netl_name);
  }
  else if(!strcmp(argv[2],"cadsnap"))  {
        char s[30]; /* overflow safe 20161212 */
        my_snprintf(s, S(s), "%.9g",cadsnap);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"rectcolor"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",rectcolor);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"debug_var"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",debug_var);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"currsch"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",xctx->currsch);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"semaphore"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",semaphore);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"change_lw"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",change_lw);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"draw_window"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",draw_window);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"ui_state"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",ui_state);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"netlist_dir"))  {
        Tcl_AppendResult(interp, netlist_dir,NULL);
  }
  else if(!strcmp(argv[2],"dim"))  {
        char s[40];
        my_snprintf(s, S(s), "%.2g", color_dim);
        Tcl_AppendResult(interp, s, NULL);
  }
  else if(!strcmp(argv[2],"instances"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",xctx->instances);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"pinlayer"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",PINLAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"wirelayer"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",WIRELAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"textlayer"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",TEXTLAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"sellayer"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",SELLAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"gridlayer"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",GRIDLAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"backlayer"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",BACKLAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"version"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "XSCHEM V%s",XSCHEM_VERSION);
        Tcl_AppendResult(interp, s,NULL);
  }
#ifndef __unix__
  else if (!strcmp(argv[2], "temp_dir")) {
    if (win_temp_dir[0] != '\0') Tcl_AppendResult(interp, win_temp_dir, NULL);
    else {
      TCHAR tmp_buffer_path[MAX_PATH];
      DWORD ret_val = GetTempPath(MAX_PATH, tmp_buffer_path);
      if (ret_val > MAX_PATH || (ret_val == 0)) {
        Tcl_AppendResult(interp, "xschem get temp_dir failed\n", NULL);
        fprintf(errfp, "xschem get temp_dir: path error\n");
        tcleval("exit");
      }
      else {
        char s[MAX_PATH];
        size_t num_char_converted;
        int err = wcstombs_s(&num_char_converted, s, MAX_PATH, tmp_buffer_path, MAX_PATH); /*unicode TBD*/
        if (err != 0) {
          Tcl_AppendResult(interp, "xschem get temp_dir conversion failed\n", NULL);
          fprintf(errfp, "xschem get temp_dir: conversion error\n");
          tcleval("exit");
        }
        else {
          change_to_unix_fn(s);
          int slen = strlen(s);
          if (s[slen - 1] == '/') s[slen - 1] = '\0';
          strcpy(win_temp_dir, s);
          dbg(2, "scheduler(): win_temp_dir is %s\n", win_temp_dir);
          Tcl_AppendResult(interp, s, NULL);
        }
      }
    }
 }
#endif
  else {
    fprintf(errfp, "xschem get %s: invalid command.\n", argv[2]);
  }
 }

 /*
  * ********** xschem  set subcommands
  */

 else if(!strcmp(argv[1],"set") && argc==3) {
   if(!strcmp(argv[2],"horizontal_move"))  {
     horizontal_move = atoi(tclgetvar("horizontal_move"));
     if(horizontal_move) {
       vertical_move=0;
       tcleval("set vertical_move 0" );
     }
   }
   else if(!strcmp(argv[2],"vertical_move"))  {
     vertical_move = atoi(tclgetvar("vertical_move"));
     if(vertical_move) {
       horizontal_move=0;
       tcleval("set horizontal_move 0" );
     }
   }
   else {
     fprintf(errfp, "xschem set %s : invalid command.\n", argv[2]);
   }
 }

 else if(!strcmp(argv[1],"set") && argc==4)
 {
   if(!strcmp(argv[2],"svg_font_name"))  {
     if( strlen(argv[3]) < sizeof(svg_font_name) ) {
       my_strncpy(svg_font_name, argv[3], S(svg_font_name));
     }
   } else
   #ifdef HAS_CAIRO
   if(!strcmp(argv[2],"cairo_font_name"))  {
     if( strlen(argv[3]) < sizeof(cairo_font_name) ) {
       my_strncpy(cairo_font_name, argv[3], S(cairo_font_name));
       cairo_select_font_face (cairo_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
       cairo_select_font_face (cairo_save_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
     }
   } else
   #endif
   if(!strcmp(argv[2],"no_undo"))  {
     int s = atoi(argv[3]);
     no_undo=s;
   }
   else if(!strcmp(argv[2],"no_draw"))  {
     int s = atoi(argv[3]);
     no_draw=s;
   }
   else if(!strcmp(argv[2],"hide_symbols"))  {
     int s = atoi(argv[3]);
     hide_symbols=s;
   }
   else if(!strcmp(argv[2],"show_pin_net_names"))  {
     int s = atoi(argv[3]);
     show_pin_net_names=s;
   }
   else if(!strcmp(argv[2],"user_top_netl_name"))  {
     my_strncpy(user_top_netl_name, argv[3], S(user_top_netl_name));
   }
   else if(!strcmp(argv[2],"dim"))  {
     double s = atof(argv[3]);
     build_colors(s);
     color_dim = s;
     draw();
     Tcl_ResetResult(interp);
   }
   else if(!strcmp(argv[2],"cairo_font_scale"))  {
     double s = atof(argv[3]);
     if(s>0.1 && s<10.0) cairo_font_scale = s;
   }
   else if(!strcmp(argv[2],"nocairo_font_xscale"))  {
     double s = atof(argv[3]);
     if(s>0.1 && s<10.0) nocairo_font_xscale = s;
   }
   else if(!strcmp(argv[2],"nocairo_font_yscale"))  {
     double s = atof(argv[3]);
     if(s>0.1 && s<10.0) nocairo_font_yscale = s;
   }
   else if(!strcmp(argv[2],"cairo_font_line_spacing"))  {
     double s = atof(argv[3]);
     if(s>0.1 && s<10.0) cairo_font_line_spacing = s;
   }
   else if(!strcmp(argv[2],"cairo_vert_correct"))  {
     double s = atof(argv[3]);
     if(s>-20. && s<20.) cairo_vert_correct = s;
   }
   else if(!strcmp(argv[2],"nocairo_vert_correct"))  {
     double s = atof(argv[3]);
     if(s>-20. && s<20.) nocairo_vert_correct = s;
   }
   else if(!strcmp(argv[2],"persistent_command")) {
     if(!strcmp(argv[3],"1")) {
       persistent_command=1;
     } else {
       persistent_command=0;
     }
   }
   else if(!strcmp(argv[2],"disable_unique_names")) {
     if(!strcmp(argv[3],"1")) {
       dis_uniq_names=1;
     } else {
       dis_uniq_names=0;
     }
   }
   else if(!strcmp(argv[2],"incr_hilight"))  {
         incr_hilight=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"auto_hilight"))  {
         auto_hilight=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"netlist_show"))  {
         netlist_show=atoi(argv[3]);
         tclsetvar("netlist_show", netlist_show ? "1" : "0");
   }
   else if(!strcmp(argv[2],"semaphore"))  {
         semaphore=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"cadsnap"))  {
         set_snap( atof(argv[3]) );
   }
   else if(!strcmp(argv[2],"spiceprefix"))  {
         spiceprefix=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"cadgrid"))  {
         set_grid( atof(argv[3]) );
   }
   else if(!strcmp(argv[2],"flat_netlist"))  {
         flat_netlist=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"split_files"))  {
         split_files=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"enable_stretch"))  {
         enable_stretch=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"a3page"))  {
         a3page=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"color_ps"))  {
         color_ps=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"only_probes"))  {
         only_probes=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"event_reporting"))  {
         event_reporting=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"draw_grid"))  {
         draw_grid=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"text_svg"))  {
         text_svg=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"sym_txt"))  {
         sym_txt=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"rectcolor"))  {
      rectcolor=atoi(argv[3]);
      tcleval("reconfigure_layers_button");
      rebuild_selected_array();
      if(lastselected) {
        change_layer();
      }
   }
   else if(!strcmp(argv[2],"change_lw"))  {
      change_lw=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"draw_window"))  {
      draw_window=atoi(argv[3]);
   }
   else if(!strcmp(argv[2],"ui_state"))  {
      ui_state=atoi(argv[3]);
   }
   else {
     Tcl_AppendResult(interp, "xschem set ", argv[1], argv[3], ": invalid command.", NULL);
     return TCL_ERROR;
   }
 }

 else {
   Tcl_AppendResult(interp, "xschem ", argv[1], ": invalid command.", NULL);
   return TCL_ERROR;
 }
 return TCL_OK;
}

const char *tclgetvar(const char *s)
{
  dbg(2, "tclgetvar(): %s\n", s);
  return Tcl_GetVar(interp,s, TCL_GLOBAL_ONLY);
}

const char *tcleval(const char str[])
{
  dbg(2, "tcleval(): %s\n", str);
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
  dbg(2, "tclsetvar(): %s to %s\n", s, value);
  if(!Tcl_SetVar(interp, s, value, TCL_GLOBAL_ONLY)) {
    fprintf(errfp, "tclsetvar(): error setting variable %s to %s\n", s, value);
  }
}
