/* File: tedax_netlist.c
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

static void tedax_netlist(FILE *fd, int tedax_stop )
{
  int i;
  char *type=NULL;

  if(!tedax_stop) {
    xctx->prep_net_structs = 0;
    prepare_netlist_structs(1);
    traverse_node_hash();  /* print all warnings about unconnected floatings etc */
  }
  if(!tedax_stop) {
    for(i=0;i<xctx->instances;i++) /* print first ipin/opin defs ... */
    {
     if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"tedax_ignore",0),"true")==0 ) continue;
     if(xctx->inst[i].ptr<0) continue;
     if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "tedax_ignore",0 ), "true") ) {
       continue;
     }
     my_strdup(421, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
     if( type && IS_PIN(type) ) {
       print_tedax_element(fd, i) ;  /* this is the element line  */
     }
    }
    for(i=0;i<xctx->instances;i++) /* ... then print other lines */
    {
     if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"tedax_ignore",0),"true")==0 ) continue;
     if(xctx->inst[i].ptr<0) continue;
     if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "tedax_ignore",0 ), "true") ) {
       continue;
     }
     my_strdup(423, &type,(xctx->inst[i].ptr+ xctx->sym)->type);

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
    my_free(967, &type);
  }
  if(!tedax_stop && !xctx->netlist_count) redraw_hilights(0); /* draw_hilight_net(1); */
}

static void tedax_block_netlist(FILE *fd, int i)
{
  int tedax_stop=0;
  char filename[PATH_MAX];
  const char *str_tmp;
  char *extra=NULL;
  char *sch = NULL;

  if(!strcmp( get_tok_value(xctx->sym[i].prop_ptr,"tedax_stop",0),"true") )
     tedax_stop=1;
  else
     tedax_stop=0;
  if((str_tmp = get_tok_value(xctx->sym[i].prop_ptr, "schematic",0 ))[0]) {
    my_strdup2(1256, &sch, str_tmp);
    my_strncpy(filename, abs_sym_path(sch, ""), S(filename));
    my_free(1257, &sch);
  } else {
    my_strncpy(filename, add_ext(abs_sym_path(xctx->sym[i].name, ""), ".sch"), S(filename));
  }
  fprintf(fd, "\n# expanding   symbol:  %s # of pins=%d\n",
        xctx->sym[i].name,xctx->sym[i].rects[PINLAYER] );
  fprintf(fd, "## sym_path: %s\n", abs_sym_path(xctx->sym[i].name, ""));
  fprintf(fd, "## sch_path: %s\n", filename);

  fprintf(fd, "begin netlist v1 %s\n",skip_dir(xctx->sym[i].name));
  print_tedax_subckt(fd, i);

  my_strdup(420, &extra, get_tok_value(xctx->sym[i].prop_ptr,"extra",0) );
  /* this is now done in print_spice_subckt */
  /*
   * fprintf(fd, "%s ", extra ? extra : "" );
   */

  /* 20081206 new get_sym_template does not return token=value pairs where token listed in extra */
  fprintf(fd, "%s", get_sym_template(xctx->sym[i].templ, extra));
  my_free(966, &extra);
  fprintf(fd, "\n");
  load_schematic(1,filename, 0);
  tedax_netlist(fd, tedax_stop);
  xctx->netlist_count++;

  if(xctx->schprop && xctx->schprop[0]) {
    fprintf(fd,"#**** begin user architecture code\n");
    fprintf(fd, "%s\n", xctx->schprop);
    fprintf(fd,"#**** end user architecture code\n");
  }
  fprintf(fd, "end netlist\n\n");
}

void global_tedax_netlist(int global)  /* netlister driver */
{
 FILE *fd;
 const char *str_tmp;
 int i;
 unsigned int *stored_flags;
 char netl_filename[PATH_MAX]; /* overflow safe 20161122 */
 char tcl_cmd_netlist[PATH_MAX + 100]; /* 20081211 overflow safe 20161122 */
 char cellname[PATH_MAX]; /* 20081211 overflow safe 20161122 */
 char *abs_path = NULL;

 xctx->push_undo();
 statusmsg("",2);  /* clear infowindow */
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
   tclgetvar("netlist_dir"), skip_dir(xctx->sch[xctx->currsch]), getpid());
 fd=fopen(netl_filename, "w");
 if(fd==NULL){
   dbg(0, "global_tedax_netlist(): problems opening netlist file\n");
   return;
 }
 fprintf(fd, "## sch_path: %s\n", xctx->sch[xctx->currsch]);

 if(xctx->netlist_name[0]) {
   my_snprintf(cellname, S(cellname), "%s", get_cell_w_ext(xctx->netlist_name, 0));
 } else {
   my_snprintf(cellname, S(cellname), "%s.tdx", skip_dir(xctx->sch[xctx->currsch]));
 }

 /* netlist_options */
 for(i=0;i<xctx->instances;i++) {
   if(!(xctx->inst[i].ptr+ xctx->sym)->type) continue;
   if( !strcmp((xctx->inst[i].ptr+ xctx->sym)->type,"netlist_options") ) {
     netlist_options(i);
   }
 }

 dbg(1, "global_tedax_netlist(): opening %s for writing\n",netl_filename);
 fprintf(fd,"tEDAx v1\nbegin netlist v1 %s\n", skip_dir( xctx->sch[xctx->currsch]) );

 tedax_netlist(fd, 0);
 xctx->netlist_count++;

 /*fprintf(fd,"**** begin user architecture code\n"); */
 /*if(xctx->schprop && xctx->schprop[0]) fprintf(fd, "%s\n", xctx->schprop); */
 /*fprintf(fd,"**** end user architecture code\n"); */
 /* /20100217 */

 fprintf(fd, "end netlist\n");

 /* preserve current level instance flags before descending hierarchy for netlisting, restore later */
 stored_flags = my_calloc(149, xctx->instances, sizeof(unsigned int));
 for(i=0;i<xctx->instances;i++) stored_flags[i] = xctx->inst[i].color;

 if(global) /* was if(global) ... 20180901 no hierarchical tEDAx netlist for now */
 {
   int saved_hilight_nets = xctx->hilight_nets;
   unselect_all();
   remove_symbols(); /* 20161205 ensure all unused symbols purged before descending hierarchy */
   /* reload data without popping undo stack, this populates embedded symbols if any */
   xctx->pop_undo(2, 0);
   /* link_symbols_to_instances(-1); */ /* done in xctx->pop_undo() */
   my_strdup(482, &xctx->sch_path[xctx->currsch+1], xctx->sch_path[xctx->currsch]);
   my_strcat(485, &xctx->sch_path[xctx->currsch+1], "->netlisting");
   xctx->sch_path_hash[xctx->currsch+1] = 0;
   xctx->currsch++;

    dbg(2, "global_tedax_netlist(): last defined symbol=%d\n",xctx->symbols);
   for(i=0;i<xctx->symbols;i++)
   {
    if( strcmp(get_tok_value(xctx->sym[i].prop_ptr,"tedax_ignore",0),"true")==0 ) continue;
    if(!xctx->sym[i].type) continue;
    my_strdup2(1236, &abs_path, abs_sym_path(xctx->sym[i].name, ""));
    if(strcmp(xctx->sym[i].type,"subcircuit")==0 && check_lib(1, abs_path))
    {
      tedax_block_netlist(fd, i);
    }
   }
   my_free(1237, &abs_path);
   /*clear_drawing(); */
   my_strncpy(xctx->sch[xctx->currsch] , "", S(xctx->sch[xctx->currsch]));
   xctx->currsch--;
   unselect_all();
   xctx->pop_undo(0, 0);
   my_strncpy(xctx->current_name, rel_sym_path(xctx->sch[xctx->currsch]), S(xctx->current_name));
   prepare_netlist_structs(1); /* so 'lab=...' attributes for unnamed nets are set */

   /* symbol vs schematic pin check, we do it here since now we have ALL symbols loaded */
   sym_vs_sch_pins();
   if(!xctx->hilight_nets) xctx->hilight_nets = saved_hilight_nets;
 }
 /* restore hilight flags from errors found analyzing top level before descending hierarchy */
 for(i=0;i<xctx->instances; i++) xctx->inst[i].color = stored_flags[i];
 propagate_hilights(1, 0, XINSERT_NOREPLACE);
 draw_hilight_net(1);
 my_free(965, &stored_flags);

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
}


