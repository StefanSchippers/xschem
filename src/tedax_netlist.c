/* File: tedax_netlist.c
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
static int tedax_netlist(FILE *fd, int tedax_stop )
{
  int err = 0;
  int i;
  char *type=NULL;
  int lvs_ignore = tclgetboolvar("lvs_ignore");

  if(!tedax_stop) {
    xctx->prep_net_structs = 0;
    err |= prepare_netlist_structs(1);
    err |= traverse_node_hash();  /* print all warnings about unconnected floatings etc */
  }
  if(!tedax_stop) {
    for(i=0;i<xctx->instances; ++i) /* print first ipin/opin defs ... */
    {
     if(skip_instance(i, lvs_ignore)) continue;
     my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
     if( type && IS_PIN(type) ) {
       print_tedax_element(fd, i) ;  /* this is the element line  */
     }
    }
    for(i=0;i<xctx->instances; ++i) /* ... then print other lines */
    {
     if(skip_instance(i, lvs_ignore)) continue;
     my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);

     if( type && !IS_LABEL_OR_PIN(type) ) {
       /* already done in global_tedax_netlist */
       if(!strcmp(type,"netlist_commands") && xctx->netlist_count==0) continue;
       if(xctx->netlist_count &&
          !strcmp(get_tok_value(xctx->inst[i].prop_ptr, "only_toplevel", 0), "true")) continue;
       if(!strcmp(type,"netlist_commands")) {
         fprintf(fd,"#**** begin user architecture code\n");
         print_tedax_element(fd, i) ;  /* this is the element line  */
         fprintf(fd,"#**** end user architecture code\n");
       } else {
         print_tedax_element(fd, i) ;  /* this is the element line  */


       }
     }
    }
    my_free(_ALLOC_ID_, &type);
  }
  if(!tedax_stop && !xctx->netlist_count) redraw_hilights(0); /* draw_hilight_net(1); */
  return err;
}

static int tedax_block_netlist(FILE *fd, int i)
{
  int err = 0;
  int tedax_stop=0;
  char filename[PATH_MAX];
  char *extra=NULL;

  if(!strcmp( get_tok_value(xctx->sym[i].prop_ptr,"tedax_stop",0),"true") )
     tedax_stop=1;
  else
     tedax_stop=0;
  get_sch_from_sym(filename, xctx->sym + i, -1);

  fprintf(fd, "\n# expanding   symbol:  %s # of pins=%d\n",
        xctx->sym[i].name,xctx->sym[i].rects[PINLAYER] );
  if(xctx->sym[i].base_name) fprintf(fd, "## sym_path: %s\n", abs_sym_path(xctx->sym[i].base_name, ""));
  else fprintf(fd, "## sym_path: %s\n", sanitized_abs_sym_path(xctx->sym[i].name, ""));
  fprintf(fd, "## sch_path: %s\n", sanitized_abs_sym_path(filename, ""));

  fprintf(fd, "begin netlist v1 %s\n",sanitize(get_cell(xctx->sym[i].name, 0)));
  print_tedax_subckt(fd, i);

  my_strdup(_ALLOC_ID_, &extra, get_tok_value(xctx->sym[i].prop_ptr,"extra",0) );
  /* this is now done in print_spice_subckt */
  /*
   * fprintf(fd, "%s ", extra ? extra : "" );
   */

  /* 20081206 new get_sym_template does not return token=value pairs where token listed in extra */
  fprintf(fd, "%s", get_sym_template(xctx->sym[i].templ, extra));
  my_free(_ALLOC_ID_, &extra);
  fprintf(fd, "\n");
  load_schematic(1,filename, 0, 1);
  get_additional_symbols(1);
  err |= tedax_netlist(fd, tedax_stop);
  xctx->netlist_count++;

  if(xctx->schprop && xctx->schprop[0]) {
    fprintf(fd,"#**** begin user architecture code\n");
    fprintf(fd, "%s\n", xctx->schprop);
    fprintf(fd,"#**** end user architecture code\n");
  }
  fprintf(fd, "end netlist\n\n");
  return err;
}

int global_tedax_netlist(int global)  /* netlister driver */
{
 int err = 0;
 FILE *fd;
 const char *str_tmp;
 int i;
 unsigned int *stored_flags;
 char netl_filename[PATH_MAX]; /* overflow safe 20161122 */
 char tcl_cmd_netlist[PATH_MAX + 100]; /* 20081211 overflow safe 20161122 */
 char cellname[PATH_MAX]; /* 20081211 overflow safe 20161122 */
 char *subckt_name;
 char *abs_path = NULL;
 Str_hashtable subckt_table = {NULL, 0};
 int lvs_ignore = tclgetboolvar("lvs_ignore");

 xctx->push_undo();
 statusmsg("",2);  /* clear infowindow */
 str_hash_init(&subckt_table, HASHSIZE);
 record_global_node(2, NULL, NULL); /* delete list of global nodes */
 bus_char[0] = bus_char[1] = '\0';
 xctx->hiersep[0]='.'; xctx->hiersep[1]='\0';
 str_tmp = tclgetvar("bus_replacement_char");
 if(str_tmp && str_tmp[0] && str_tmp[1]) {
   bus_char[0] = str_tmp[0];
   bus_char[1] = str_tmp[1];
 }
 xctx->netlist_count=0;
 my_snprintf(netl_filename, S(netl_filename), "%s/.%s_%d", 
   tclgetvar("netlist_dir"), get_cell(xctx->sch[xctx->currsch], 0), getpid());
 fd=fopen(netl_filename, "w");
 if(fd==NULL){
   dbg(0, "global_tedax_netlist(): problems opening netlist file\n");
   return 1;
 }
 fprintf(fd, "## sch_path: %s\n", xctx->sch[xctx->currsch]);

 if(xctx->netlist_name[0]) {
   my_snprintf(cellname, S(cellname), "%s", get_cell_w_ext(xctx->netlist_name, 0));
 } else {
   my_snprintf(cellname, S(cellname), "%s.tdx", get_cell(xctx->sch[xctx->currsch], 0));
 }

 /* netlist_options */
 for(i=0;i<xctx->instances; ++i) {
   if(!(xctx->inst[i].ptr+ xctx->sym)->type) continue;
   if( !strcmp((xctx->inst[i].ptr+ xctx->sym)->type,"netlist_options") ) {
     netlist_options(i);
   }
 }

 dbg(1, "global_tedax_netlist(): opening %s for writing\n",netl_filename);
 fprintf(fd,"tEDAx v1\nbegin netlist v1 %s\n", get_cell( xctx->sch[xctx->currsch], 0) );

 tedax_netlist(fd, 0);
 xctx->netlist_count++;

 /*fprintf(fd,"**** begin user architecture code\n"); */
 /*if(xctx->schprop && xctx->schprop[0]) fprintf(fd, "%s\n", xctx->schprop); */
 /*fprintf(fd,"**** end user architecture code\n"); */
 /* /20100217 */

 fprintf(fd, "end netlist\n");

 /* warning if two symbols perfectly overlapped */
 err |= warning_overlapped_symbols(0);
 /* preserve current level instance flags before descending hierarchy for netlisting, restore later */
 stored_flags = my_calloc(_ALLOC_ID_, xctx->instances, sizeof(unsigned int));
 for(i=0;i<xctx->instances; ++i) stored_flags[i] = xctx->inst[i].color;

 if(global) /* was if(global) ... 20180901 no hierarchical tEDAx netlist for now */
 {
   int saved_hilight_nets = xctx->hilight_nets;
   int web_url = is_from_web(xctx->current_dirname);
   char *current_dirname_save = NULL; 

   unselect_all(1);
   remove_symbols(); /* 20161205 ensure all unused symbols purged before descending hierarchy */
   /* reload data without popping undo stack, this populates embedded symbols if any */
   xctx->pop_undo(2, 0);
   /* link_symbols_to_instances(-1); */ /* done in xctx->pop_undo() */
   my_strdup(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], xctx->sch_path[xctx->currsch]);
   my_strcat(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], "->netlisting");
   xctx->sch_path_hash[xctx->currsch+1] = 0;
   xctx->currsch++;
   subckt_name=NULL;
   dbg(2, "global_tedax_netlist(): last defined symbol=%d\n",xctx->symbols);
   get_additional_symbols(1);
   for(i=0;i<xctx->symbols; ++i)
   {
    if(xctx->sym[i].flags & (TEDAX_IGNORE | TEDAX_SHORT)) continue;
    if(lvs_ignore && (xctx->sym[i].flags & LVS_IGNORE)) continue;
    if(!xctx->sym[i].type) continue;
    my_strdup2(_ALLOC_ID_, &abs_path, abs_sym_path(tcl_hook2(xctx->sym[i].name), ""));
    if(strcmp(xctx->sym[i].type,"subcircuit")==0 && check_lib(1, abs_path))
    {
      if(!web_url) {
        tclvareval("get_directory [list ", xctx->sch[xctx->currsch - 1], "]", NULL);
        my_strncpy(xctx->current_dirname, tclresult(),  S(xctx->current_dirname));
      }
      /* xctx->sym can be SCH or SYM, use hash to avoid writing duplicate subckt */
      my_strdup(_ALLOC_ID_, &subckt_name, get_cell(xctx->sym[i].name, 0));
      if (str_hash_lookup(&subckt_table, subckt_name, "", XLOOKUP)==NULL)
      {
        str_hash_lookup(&subckt_table, subckt_name, "", XINSERT);
        err |= tedax_block_netlist(fd, i);
      }
    }
   }
   my_free(_ALLOC_ID_, &abs_path);
   /* can not free additional syms since *_block_netlist() may have loaded additional syms */
   /* get_additional_symbols(0); */
   str_hash_free(&subckt_table);
   my_free(_ALLOC_ID_, &subckt_name);
   /*clear_drawing(); */
   my_strncpy(xctx->sch[xctx->currsch] , "", S(xctx->sch[xctx->currsch]));
   xctx->currsch--;
   unselect_all(1);
   xctx->pop_undo(4, 0);
   if(web_url) {
     my_strncpy(xctx->current_dirname, current_dirname_save, S(xctx->current_dirname));
   } else {
     tclvareval("get_directory [list ", xctx->sch[xctx->currsch], "]", NULL);
     my_strncpy(xctx->current_dirname, tclresult(),  S(xctx->current_dirname));
   }
   my_strncpy(xctx->current_name, rel_sym_path(xctx->sch[xctx->currsch]), S(xctx->current_name));
   err |= prepare_netlist_structs(1); /* so 'lab=...' attributes for unnamed nets are set */

   /* symbol vs schematic pin check, we do it here since now we have ALL symbols loaded */
   err |= sym_vs_sch_pins();
   if(!xctx->hilight_nets) xctx->hilight_nets = saved_hilight_nets;
   my_free(_ALLOC_ID_, &current_dirname_save);
 }
 /* restore hilight flags from errors found analyzing top level before descending hierarchy */
 for(i=0;i<xctx->instances; ++i) if(!xctx->inst[i].color) xctx->inst[i].color = stored_flags[i];

 propagate_hilights(1, 0, XINSERT_NOREPLACE);
 draw_hilight_net(1);
 my_free(_ALLOC_ID_, &stored_flags);

 /* print globals nodes found in netlist 28032003 */
 record_global_node(0,fd,NULL);
 fprintf(fd, "__HIERSEP__ %s\n", xctx->hiersep);

 dbg(1, "global_tedax_netlist(): starting awk on netlist!\n");

 fclose(fd);
 if(tclgetboolvar("netlist_show")) {
  my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} show {%s}", netl_filename, cellname);
  tcleval(tcl_cmd_netlist);
 }
 else {
  my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} noshow {%s}", netl_filename, cellname);
  tcleval(tcl_cmd_netlist);
 }
 if(!debug_var) xunlink(netl_filename);
 xctx->netlist_count = 0;
 return err;
}

