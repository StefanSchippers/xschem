/* File: tedax_netlist.c
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

void global_tedax_netlist(int global)  /* netlister driver */
{
 FILE *fd;
 int i, save_ok;
 unsigned int *stored_flags;
 char netl_filename[PATH_MAX]; /* overflow safe 20161122 */
 char tcl_cmd_netlist[PATH_MAX + 100]; /* 20081211 overflow safe 20161122 */
 char cellname[PATH_MAX]; /* 20081211 overflow safe 20161122 */

 if(modified) {
   save_ok = save_schematic(schematic[currentsch]);
   if(save_ok == -1) return;
 }
 statusmsg("",2);  /* clear infowindow */
 record_global_node(2, NULL, NULL); /* delete list of global nodes */
 netlist_count=0;

 my_snprintf(netl_filename, S(netl_filename), "%s/.%s_%d", netlist_dir, skip_dir(schematic[currentsch]), getpid());
 fd=fopen(netl_filename, "w");

 if(user_top_netl_name[0]) {
   my_snprintf(cellname, S(cellname), "%s", get_cell(user_top_netl_name, 0));
 } else {
   my_snprintf(cellname, S(cellname), "%s.tdx", skip_dir(schematic[currentsch]));
 }

 if(fd==NULL){ 
   dbg(0, "global_tedax_netlist(): problems opening netlist file\n");
   return;
 }
 dbg(1, "global_tedax_netlist(): opening %s for writing\n",netl_filename);
 fprintf(fd,"tEDAx v1\nbegin netlist v1 %s\n", skip_dir( schematic[currentsch]) );

 tedax_netlist(fd, 0);

 /* 20100217 */
 /*fprintf(fd,"**** begin user architecture code\n"); */
 /*netlist_count++; */
 /*if(schprop && schprop[0]) fprintf(fd, "%s\n", schprop); */
 /*fprintf(fd,"**** end user architecture code\n"); */
 /* /20100217 */

 fprintf(fd, "end netlist\n");

 /* preserve current level instance flags before descending hierarchy for netlisting, restore later */
 stored_flags = my_calloc(149, lastinst, sizeof(unsigned int));
 for(i=0;i<lastinst;i++) stored_flags[i] = inst_ptr[i].flags & 4;

 if(0) /* was if(global) ... 20180901 no hierarchical tEDAx netlist for now */
 {
   unselect_all();
   remove_symbols(); /* 20161205 ensure all unused symbols purged before descending hierarchy */
   load_schematic(1, schematic[currentsch], 0); /* 20180927 */

   currentsch++;
    dbg(2, "global_tedax_netlist(): last defined symbol=%d\n",lastinstdef);
   for(i=0;i<lastinstdef;i++)
   {
    if( strcmp(get_tok_value(instdef[i].prop_ptr,"tedax_ignore",0),"true")==0 ) continue; /* 20070726 */
    if(!instdef[i].type) continue;
    if(strcmp(instdef[i].type,"subcircuit")==0 && check_lib(instdef[i].name)) /* 20150409 */
    {
      tedax_block_netlist(fd, i); /* 20081205 */
    }
   }
   /*clear_drawing(); */
   my_strncpy(schematic[currentsch] , "", S(schematic[currentsch]));
   currentsch--;
   unselect_all();
   /* remove_symbols(); */
   load_schematic(1, schematic[currentsch], 0); /* 20180927 */
   prepare_netlist_structs(1); /* so 'lab=...' attributes for unnamed nets are set */

   /* symbol vs schematic pin check, we do it here since now we have ALL symbols loaded */
   sym_vs_sch_pins();

   /* restore hilight flags from errors found analyzing top level before descending hierarchy */
   for(i=0;i<lastinst; i++) inst_ptr[i].flags |= stored_flags[i];

   draw_hilight_net(1);
 }
 my_free(965, &stored_flags);

 /* print globals nodes found in netlist 28032003 */
 record_global_node(0,fd,NULL);

 dbg(1, "global_tedax_netlist(): starting awk on netlist!\n");

 fclose(fd);
 if(netlist_show) {
  my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} show {%s}", netl_filename, cellname);
  tcleval(tcl_cmd_netlist);
 }
 else {
  my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} noshow {%s}", netl_filename, cellname);
  tcleval(tcl_cmd_netlist);
 }
 if(!debug_var) xunlink(netl_filename);
 netlist_count = 0;
}


void tedax_block_netlist(FILE *fd, int i)  /*20081223 */
{
  int j;
  int tedax_stop=0; /* 20111113 */
  char filename[PATH_MAX];
  char netl_filename[PATH_MAX];
  char tcl_cmd_netlist[PATH_MAX + 100];  /* 20081202 */
  char cellname[PATH_MAX];  /* 20081202 */
  const char *str_tmp;
  int mult;
  char *extra=NULL;
  if(!strcmp( get_tok_value(instdef[i].prop_ptr,"tedax_stop",0),"true") )
     tedax_stop=1;
  else
     tedax_stop=0;
  fprintf(fd, "\n* expanding   symbol:  %s # of pins=%d\n\n", 
        instdef[i].name,instdef[i].rects[PINLAYER] );
  fprintf(fd, ".subckt %s ",skip_dir(instdef[i].name));
  for(j=0;j<instdef[i].rects[PINLAYER];j++)
  {
    str_tmp=
      expandlabel(get_tok_value(instdef[i].boxptr[PINLAYER][j].prop_ptr,"name",0), &mult);
    if(str_tmp)
      fprintf(fd,"%s ",str_tmp);
    else 
      fprintf(fd,"<NULL> ");
  }
  my_strdup(420, &extra, get_tok_value(instdef[i].prop_ptr,"extra",0) ); /* 20081206 */
  fprintf(fd, "%s ", extra ? extra : "" );
  /* 20081206 new get_sym_template does not return token=value pairs where token listed in extra */
  fprintf(fd, "%s", get_sym_template(instdef[i].templ, extra)); /* 20150409 */
  my_free(966, &extra);
  fprintf(fd, "\n");
  /*clear_drawing(); */
  if((str_tmp = get_tok_value(instdef[i].prop_ptr, "schematic",0 ))[0]) {
    my_strncpy(filename, abs_sym_path(str_tmp, ""), S(filename));
    load_schematic(1,filename, 0);
  } else {
    load_schematic(1, add_ext(abs_sym_path(instdef[i].name, ""), ".sch"), 0); /* 20190518 */
  }
  tedax_netlist(fd, tedax_stop);  /* 20111113 added tedax_stop */
  netlist_count++;
  fprintf(fd,"**** begin user architecture code\n");
  if(schprop && schprop[0]) fprintf(fd, "%s\n", schprop);
  fprintf(fd,"**** end user architecture code\n");
  fprintf(fd, ".ends\n\n");
  if(split_files) { /* 20081204 */
    fclose(fd);
    my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} noshow {%s}", netl_filename, cellname);
    tcleval(tcl_cmd_netlist);
    if(debug_var==0) xunlink(netl_filename);
  }
}

void tedax_netlist(FILE *fd, int tedax_stop )
{
  int i;
  char *type=NULL;
 
  prepared_netlist_structs = 0;
  prepare_netlist_structs(1);
  /* set_modify(1); */ /* 20160302 prepare_netlist_structs could change schematic (wire node naming for example) */
  traverse_node_hash();  /* print all warnings about unconnected floatings etc */
  if(!tedax_stop) {
    for(i=0;i<lastinst;i++) /* print first ipin/opin defs ... */
    {
     if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"tedax_ignore",0),"true")==0 ) continue; /* 20140416 */
     if(inst_ptr[i].ptr<0) continue;
     if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "tedax_ignore",0 ), "true") ) {
       continue;
     }
     my_strdup(421, &type,(inst_ptr[i].ptr+instdef)->type); /* 20150409 */
     if( type && IS_PIN(type) ) {
       print_tedax_element(fd, i) ;  /* this is the element line  */
     }
    }
    for(i=0;i<lastinst;i++) /* ... then print other lines */
    {
     if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"tedax_ignore",0),"true")==0 ) continue; /* 20140416 */
     if(inst_ptr[i].ptr<0) continue;
     if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "tedax_ignore",0 ), "true") ) {/*20070726 */
       continue;                                                                                   /*20070726 */
     }                                                                                             /*20070726 */
     my_strdup(423, &type,(inst_ptr[i].ptr+instdef)->type); /* 20150409 */
     if( type && !IS_PIN(type) ) {
       if(!strcmp(type,"netlist_commands") && netlist_count==0) continue; /* already done in global_tedax_netlist */
       if(netlist_count && 
          !strcmp(get_tok_value(inst_ptr[i].prop_ptr, "only_toplevel", 0), "true")) continue; /* 20160418 */
       print_tedax_element(fd, i) ;  /* this is the element line  */
     }
    }
  }
  if(!netlist_count) redraw_hilights(); /* draw_hilight_net(1); */
  my_free(967, &type);
}


