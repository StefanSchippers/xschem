/* File: spice_netlist.c
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

static Str_hashtable model_table = {NULL, 0}; /* safe even with multiple schematics */

static const char *hier_psprint_mtime(const char *file_name)
{
  static char date[200] = "";
  struct stat time_buf;
  struct tm *tm;
  if(!stat(file_name , &time_buf)) {
    tm=localtime(&(time_buf.st_mtime) );
    strftime(date, sizeof(date), "%Y%m%d_%H%M%S", tm);
  }
  return date;
}

/* 
 * what: 
 * 1 : ps/pdf print
 * 2 : list hierarchy
 */
void hier_psprint(char **res, int what)  /* netlister driver */
{
  int i;
  char *subckt_name;
  char filename[PATH_MAX];
  char *abs_path = NULL;
  struct stat buf;
  Str_hashtable subckt_table = {NULL, 0};
 
  if((what & 1)  && !ps_draw(1)) return; /* prolog */
  xctx->push_undo();
  str_hash_init(&subckt_table, HASHSIZE);
  zoom_full(0, 0, 3, 0.97);
  if(what & 1) ps_draw(2); /* page */
  if(what & 2) { /* print cellname */
    my_strcat(_ALLOC_ID_, res, hier_psprint_mtime(xctx->sch[xctx->currsch]));
    my_strcat(_ALLOC_ID_, res, "  {");
    my_strcat(_ALLOC_ID_, res, xctx->sch[xctx->currsch]);
    my_strcat(_ALLOC_ID_, res, "}\n");
  }
  dbg(1,"--> %s\n", get_cell(xctx->sch[xctx->currsch], 0) );
  unselect_all(1);
  remove_symbols(); /* ensure all unused symbols purged before descending hierarchy */
  /* reload data without popping undo stack, this populates embedded symbols if any */
  xctx->pop_undo(2, 0);
  /* link_symbols_to_instances(-1); */ /* done in xctx->pop_undo() */
  my_strdup(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], xctx->sch_path[xctx->currsch]);
  my_strcat(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], "->netlisting");
  xctx->sch_path_hash[xctx->currsch+1] = 0;
  xctx->currsch++;
  subckt_name=NULL;
  get_additional_symbols(1);
  for(i=0;i<xctx->symbols; ++i)
  {
    int flag;
    /* for printing we process also symbols that have *_ignore attribute */
    if(!xctx->sym[i].type || !xctx->sym[i].name || !xctx->sym[i].name[0]) continue; /* can not descend into */
    my_strdup2(_ALLOC_ID_, &abs_path, abs_sym_path(tcl_hook2(xctx->sym[i].name), ""));
    if(what & 1) flag = check_lib(2, abs_path); /* noprint_libs */
    else flag = check_lib(4, abs_path); /* nolist_libs */
    if(strcmp(xctx->sym[i].type,"subcircuit")==0 && flag)
    {
      /* xctx->sym can be SCH or SYM, use hash to avoid writing duplicate subckt */
      my_strdup(_ALLOC_ID_, &subckt_name, get_cell(xctx->sym[i].name, 0));
      get_sch_from_sym(filename, xctx->sym + i, -1);
      if (str_hash_lookup(&subckt_table, filename, "", XLOOKUP)==NULL)
      {
        str_hash_lookup(&subckt_table, filename, "", XINSERT);
        if(is_generator(filename) || !stat(filename, &buf)) {
          /* for printing we go down to bottom regardless of spice_stop attribute */
          dbg(1, "hier_psprint(): loading file: |%s|\n", filename);
          load_schematic(1,filename, 0, 1);
          get_additional_symbols(1);
          zoom_full(0, 0, 1, 0.97);
          if(what & 1) ps_draw(2); /* page */
          if(what & 2) { /* print cellname */
            my_strcat(_ALLOC_ID_, res, hier_psprint_mtime(xctx->sch[xctx->currsch]));
            my_strcat(_ALLOC_ID_, res, "  {");
            my_strcat(_ALLOC_ID_, res, xctx->sch[xctx->currsch]);
            my_strcat(_ALLOC_ID_, res, "}\n");
          }
          dbg(1,"--> %s\n", get_cell(xctx->sch[xctx->currsch], 0) );
        }
      }
    }
  }
  /* can not free additional syms since *_block_netlist() may have loaded additional syms */
  /* get_additional_symbols(0); */
  my_free(_ALLOC_ID_, &abs_path);
  str_hash_free(&subckt_table);
  my_free(_ALLOC_ID_, &subckt_name);
  my_strncpy(xctx->sch[xctx->currsch] , "", S(xctx->sch[xctx->currsch]));
  xctx->currsch--;
  unselect_all(1);
  xctx->pop_undo(0, 0);
  my_strncpy(xctx->current_name, rel_sym_path(xctx->sch[xctx->currsch]), S(xctx->current_name));
  if(what & 1) ps_draw(4); /* trailer */
  zoom_full(0, 0, 3, 0.97);
  draw();
}

static char *model_name_result = NULL; /* safe even with multiple schematics */

static char *model_name(const char *m)
{
  char *m_lower = NULL;
  char *modelname = NULL;
  int n;
  size_t l = strlen(m) + 1;
  my_strdup(_ALLOC_ID_, &m_lower, m);
  strtolower(m_lower);
  my_realloc(_ALLOC_ID_, &modelname, l);
  my_realloc(_ALLOC_ID_, &model_name_result, l);
  n = sscanf(m_lower, " %s %s", model_name_result, modelname);
  if(n<2) my_strncpy(model_name_result, m_lower, l);
  else {
    /* build a hash key value with no spaces to make device_model attributes with different spaces equivalent*/
    my_strcat(_ALLOC_ID_, &model_name_result, modelname);
  }
  my_free(_ALLOC_ID_, &modelname);
  my_free(_ALLOC_ID_, &m_lower);
  return model_name_result;
}

static int spice_netlist(FILE *fd, int spice_stop )
{
  int err = 0;
  int i, flag = 0;
  char *type=NULL;
  int top_sub;

  top_sub = tclgetboolvar("lvs_netlist");
  if(!spice_stop) {
    dbg(1, "spice_netlist(): invoke prepare_netlist_structs for %s\n", xctx->current_name);
    xctx->prep_net_structs = 0;
    err |= prepare_netlist_structs(1);
    err |= traverse_node_hash();  /* print all warnings about unconnected floatings etc */
    for(i=0;i<xctx->instances; ++i) /* print first ipin/opin defs ... */
    {
     if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue;
     if(xctx->inst[i].ptr<0) continue;
     if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "spice_ignore",0 ), "true") ) {
       continue;
     }
     my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
     if( type && IS_PIN(type) ) {
       if(top_sub && !flag) {
         fprintf(fd, "*.PININFO ");
         flag = 1;
       }
       if(top_sub) {
         int d = 'X';
         if(!strcmp(type, "ipin")) d = 'I';
         if(!strcmp(type, "opin")) d = 'O';
         if(!strcmp(type, "iopin")) d = 'B';
         fprintf(fd, "%s:%c ",xctx->inst[i].lab, d);
       } else {
         print_spice_element(fd, i) ;  /* this is the element line  */
       }
     }
    }
    if(top_sub) fprintf(fd, "\n");
    for(i=0;i<xctx->instances; ++i) /* ... then print other lines */
    {
     if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue;
     if(xctx->inst[i].ptr<0) continue;
     if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "spice_ignore",0 ), "true") ) {
       continue;
     }
     my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
 
     if( type && !IS_LABEL_OR_PIN(type) ) {
       /* already done in global_spice_netlist */
       if(!strcmp(type,"netlist_commands") && xctx->netlist_count==0) continue;
       if(xctx->netlist_count &&
          !strcmp(get_tok_value(xctx->inst[i].prop_ptr, "only_toplevel", 0), "true")) continue;
       if(!strcmp(type,"netlist_commands")) {
         fprintf(fd,"**** begin user architecture code\n");
         print_spice_element(fd, i) ;  /* this is the element line  */
         fprintf(fd,"**** end user architecture code\n");
       } else {
         const char *m;
         if(print_spice_element(fd, i)) fprintf(fd, "**** end_element\n");
         /* hash device_model attribute if any */
         m = get_tok_value(xctx->inst[i].prop_ptr, "device_model", 0);
         if(m[0]) str_hash_lookup(&model_table, model_name(m), m, XINSERT);
         else {
           m = get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "device_model", 0);
           if(m[0]) str_hash_lookup(&model_table, model_name(m), m, XINSERT);
         }
         my_free(_ALLOC_ID_, &model_name_result);
       }
     }
    }
    my_free(_ALLOC_ID_, &type);
  }
  if(!spice_stop && !xctx->netlist_count) redraw_hilights(0); /* draw_hilight_net(1); */
  return err;
}

int global_spice_netlist(int global)  /* netlister driver */
{
 int err = 0;
 int first;
 FILE *fd;
 const char *str_tmp;
 int multip;
 unsigned int *stored_flags;
 int i;
 char *type=NULL;
 char *place=NULL;
 char netl_filename[PATH_MAX]; /* overflow safe 20161122 */
 char tcl_cmd_netlist[PATH_MAX + 100]; /* 20081211 overflow safe 20161122 */
 char cellname[PATH_MAX]; /* 20081211 overflow safe 20161122 */
 char *subckt_name;
 char *abs_path = NULL;
 int top_sub;
 int split_f;
 Str_hashtable subckt_table = {NULL, 0};
 Str_hashentry *model_entry;

 split_f = tclgetboolvar("split_files");
 dbg(1, "global_spice_netlist(): invoking push_undo()\n");
 xctx->push_undo();
 xctx->netlist_unconn_cnt=0; /* unique count of unconnected pins while netlisting */
 statusmsg("",2);  /* clear infowindow */
 str_hash_init(&subckt_table, HASHSIZE);
 str_hash_init(&model_table, HASHSIZE);
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
 dbg(1, "global_spice_netlist(): opening %s for writing\n",netl_filename);
 fd=fopen(netl_filename, "w");
 if(fd==NULL) {
   dbg(0, "global_spice_netlist(): problems opening netlist file\n");
   return 1;
 }
 fprintf(fd, "** sch_path: %s\n", xctx->sch[xctx->currsch]);

 if(xctx->netlist_name[0]) {
   my_snprintf(cellname, S(cellname), "%s", get_cell_w_ext(xctx->netlist_name, 0));
 } else {
   my_snprintf(cellname, S(cellname), "%s.spice", get_cell(xctx->sch[xctx->currsch], 0));
 }

 first = 0;
 for(i=0;i<xctx->instances; ++i) /* print netlist_commands of top level cell with 'place=header' property */
 {
  if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue;
  if(xctx->inst[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "spice_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  my_strdup(_ALLOC_ID_, &place,get_tok_value((xctx->inst[i].ptr+ xctx->sym)->prop_ptr,"place",0));
  if( type && !strcmp(type,"netlist_commands") ) {
   if(!place) {
     my_strdup(_ALLOC_ID_, &place,get_tok_value(xctx->inst[i].prop_ptr,"place",0));
   }
   if(place && !strcmp(place, "header" )) {
     if(first == 0) fprintf(fd,"**** begin user header code\n");
     ++first;
     print_spice_element(fd, i) ;  /* this is the element line  */
   }
  }
 }
 if(first) fprintf(fd,"**** end user header code\n");

 /* netlist_options */
 for(i=0;i<xctx->instances; ++i) {
   if(!(xctx->inst[i].ptr+ xctx->sym)->type) continue;
   if( !strcmp((xctx->inst[i].ptr+ xctx->sym)->type,"netlist_options") ) {
     netlist_options(i);
   }
 }
 top_sub = tclgetboolvar("lvs_netlist");
 if(!top_sub) fprintf(fd,"**");
 fprintf(fd,".subckt %s", get_cell(xctx->sch[xctx->currsch], 0));

 /* print top subckt ipin/opins */
 for(i=0;i<xctx->instances; ++i) {
  if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue;
  if(xctx->inst[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "spice_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  dbg(1, "global_spice_netlist(): |%s|\n", type);
  /* 
  if( type && !strcmp(type,"netlist_options") ) {
    continue;
  }
  */
  if( type && IS_PIN(type)) {
   str_tmp = expandlabel ( (xctx->inst[i].lab ? xctx->inst[i].lab : ""), &multip);
   /*must handle  invalid node names */
   fprintf(fd, " %s", str_tmp ? str_tmp : "(NULL)" );
  }
 }
 fprintf(fd,"\n");

 err |= spice_netlist(fd, 0);

 first = 0;
 for(i=0;i<xctx->instances; ++i) /* print netlist_commands of top level cell with no 'place=end' property
                                   and no place=header */
 {
  if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue;
  if(xctx->inst[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "spice_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  my_strdup(_ALLOC_ID_, &place,get_tok_value((xctx->inst[i].ptr+ xctx->sym)->prop_ptr,"place",0));
  if( type && !strcmp(type,"netlist_commands") ) {
   if(!place) {
     my_strdup(_ALLOC_ID_, &place,get_tok_value(xctx->inst[i].prop_ptr,"place",0));
   }
   if(!place || (strcmp(place, "end") && strcmp(place, "header")) ) {
     if(first == 0) fprintf(fd,"**** begin user architecture code\n");
     ++first;
     print_spice_element(fd, i) ;  /* this is the element line  */
   }
  }
 }

 xctx->netlist_count++;

 if(xctx->schprop && xctx->schprop[0]) {
   if(first == 0) fprintf(fd,"**** begin user architecture code\n");
   ++first;
   fprintf(fd, "%s\n", xctx->schprop);
 }
 if(first) fprintf(fd,"**** end user architecture code\n");
 /* /20100217 */

 if(!top_sub) fprintf(fd,"**");
 fprintf(fd, ".ends\n");


 if(split_f) {
   int save;
   fclose(fd);
   my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} noshow {%s}", netl_filename, cellname);
   save = xctx->netlist_type;
   xctx->netlist_type = CAD_SPICE_NETLIST;
   set_tcl_netlist_type();
   tcleval(tcl_cmd_netlist);
   xctx->netlist_type = save;
   set_tcl_netlist_type();

   if(debug_var==0) xunlink(netl_filename);
 }

 /* warning if two symbols perfectly overlapped */
 err |= warning_overlapped_symbols(0);
 /* preserve current level instance flags before descending hierarchy for netlisting, restore later */
 stored_flags = my_calloc(_ALLOC_ID_, xctx->instances, sizeof(unsigned int));
 for(i=0;i<xctx->instances; ++i) stored_flags[i] = xctx->inst[i].color;
 
 if(global)
 { 
   int saved_hilight_nets = xctx->hilight_nets;
   unselect_all(1);
   remove_symbols(); /* 20161205 ensure all unused symbols purged before descending hierarchy */
   /* reload data without popping undo stack, this populates embedded symbols if any */
   dbg(1, "global_spice_netlist(): invoking pop_undo(2, 0)\n");
   xctx->pop_undo(2, 0);
   /* link_symbols_to_instances(-1); */ /* done in xctx->pop_undo() */
   my_strdup(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], xctx->sch_path[xctx->currsch]);
   my_strcat(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], "->netlisting");
   xctx->sch_path_hash[xctx->currsch+1] = 0;
   xctx->currsch++;
   subckt_name=NULL;
   dbg(2, "global_spice_netlist(): last defined symbol=%d\n",xctx->symbols);
   get_additional_symbols(1);
   for(i=0;i<xctx->symbols; ++i)
   {
    if( strcmp(get_tok_value(xctx->sym[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue;
    if(!xctx->sym[i].type) continue;
    my_strdup(_ALLOC_ID_, &abs_path, abs_sym_path(xctx->sym[i].name, ""));
    if(strcmp(xctx->sym[i].type,"subcircuit")==0 && check_lib(1, abs_path))
    {
      tclvareval("get_directory [list ", xctx->sch[xctx->currsch - 1], "]", NULL);
      my_strncpy(xctx->current_dirname, tclresult(),  S(xctx->current_dirname));
      /* xctx->sym can be SCH or SYM, use hash to avoid writing duplicate subckt */
      my_strdup(_ALLOC_ID_, &subckt_name, get_cell(xctx->sym[i].name, 0));
      dbg(1, "global_spice_netlist(): subckt_name=%s\n", subckt_name);
      if (str_hash_lookup(&subckt_table, subckt_name, "", XLOOKUP)==NULL)
      {
        str_hash_lookup(&subckt_table, subckt_name, "", XINSERT);
        if( split_f && strcmp(get_tok_value(xctx->sym[i].prop_ptr,"vhdl_netlist",0),"true")==0 )
          err |= vhdl_block_netlist(fd, i);
        else if(split_f && strcmp(get_tok_value(xctx->sym[i].prop_ptr,"verilog_netlist",0),"true")==0 )
          err |= verilog_block_netlist(fd, i);
        else
          if( strcmp(get_tok_value(xctx->sym[i].prop_ptr,"spice_primitive",0),"true") )
            err |= spice_block_netlist(fd, i);
      }
    }
   }
   my_free(_ALLOC_ID_, &abs_path);
   /* get_additional_symbols(0); */
   my_free(_ALLOC_ID_, &subckt_name);
   /*clear_drawing(); */
   my_strncpy(xctx->sch[xctx->currsch] , "", S(xctx->sch[xctx->currsch]));
   xctx->currsch--;
   unselect_all(1);
   dbg(1, "global_spice_netlist(): invoking pop_undo(0, 0)\n");
   xctx->pop_undo(0, 0);
   my_strncpy(xctx->current_name, rel_sym_path(xctx->sch[xctx->currsch]), S(xctx->current_name));
   dbg(1, "spice_netlist(): invoke prepare_netlist_structs for %s\n", xctx->current_name);
   err |= prepare_netlist_structs(1); /* so 'lab=...' attributes for unnamed nets are set */
   /* symbol vs schematic pin check, we do it here since now we have ALL symbols loaded */
   err |= sym_vs_sch_pins();
   if(!xctx->hilight_nets) xctx->hilight_nets = saved_hilight_nets;
 }
 /* restore hilight flags from errors found analyzing top level before descending hierarchy */
 for(i=0;i<xctx->instances; ++i) xctx->inst[i].color = stored_flags[i];
 propagate_hilights(1, 0, XINSERT_NOREPLACE);
 draw_hilight_net(1);
 my_free(_ALLOC_ID_, &stored_flags);

 /* print globals nodes found in netlist 28032003 */
 if(!split_f) record_global_node(0,fd,NULL);

 /* =================================== 20121223 */
 first = 0;
 if(!split_f) {
   for(i=0;i<xctx->instances; ++i) /* print netlist_commands of top level cell with 'place=end' property */
   {
    if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue;
    if(xctx->inst[i].ptr<0) continue;
    if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "spice_ignore",0 ), "true") ) {
      continue;
    }
    my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
    my_strdup(_ALLOC_ID_, &place,get_tok_value((xctx->inst[i].ptr+ xctx->sym)->prop_ptr,"place",0));
    if( type && !strcmp(type,"netlist_commands") ) {
     if(place && !strcmp(place, "end" )) {
       if(first == 0) fprintf(fd,"**** begin user architecture code\n");
       ++first;
       print_spice_element(fd, i) ;
     } else {
       my_strdup(_ALLOC_ID_, &place,get_tok_value(xctx->inst[i].prop_ptr,"place",0));
       if(place && !strcmp(place, "end" )) {
         if(first == 0) fprintf(fd,"**** begin user architecture code\n");
         ++first;
         print_spice_element(fd, i) ;
       }
     }
    } /* netlist_commands */
   }

 }
 /* print device_model attributes */
 for(i=0;i<model_table.size; ++i) {
   model_entry=model_table.table[i];
   while(model_entry) {
     if(first == 0) fprintf(fd,"**** begin user architecture code\n");
     ++first;
     fprintf(fd, "%s\n",  model_entry->value);
     model_entry = model_entry->next;
   }
 }
 str_hash_free(&model_table);
 str_hash_free(&subckt_table);
 if(first) fprintf(fd,"**** end user architecture code\n");


 /* 20150922 added split_files check */
 if(!split_f) fprintf(fd, ".end\n");

 dbg(1, "global_spice_netlist(): starting awk on netlist!\n");


 if(!split_f) {
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
 }
 my_free(_ALLOC_ID_, &type);
 my_free(_ALLOC_ID_, &place);
 xctx->netlist_count = 0;
 return err;
}

int spice_block_netlist(FILE *fd, int i)
{
  int err = 0;
  int spice_stop=0;
  char netl_filename[PATH_MAX];
  char tcl_cmd_netlist[PATH_MAX + 100];
  char cellname[PATH_MAX];
  char filename[PATH_MAX];
  /* int j; */
  /* int multip; */
  char *extra=NULL;
  int split_f;
  const char *sym_def;
  char *name = NULL;

  my_strdup(_ALLOC_ID_, &name, tcl_hook2(xctx->sym[i].name));
  split_f = tclgetboolvar("split_files");

  if(!strcmp( get_tok_value(xctx->sym[i].prop_ptr,"spice_stop",0),"true") )
     spice_stop=1;
  else
     spice_stop=0;
  get_sch_from_sym(filename, xctx->sym + i, -1);
  dbg(1, "spice_block_netlist(): filename=%s\n", filename);
  if(split_f) {
    my_snprintf(netl_filename, S(netl_filename), "%s/.%s_%d",
         tclgetvar("netlist_dir"), get_cell(name, 0), getpid());
    dbg(1, "spice_block_netlist(): split_files: netl_filename=%s\n", netl_filename);
    fd=fopen(netl_filename, "w");
    my_snprintf(cellname, S(cellname), "%s.spice", get_cell(name, 0));
  }
  fprintf(fd, "\n* expanding   symbol:  %s # of pins=%d\n", name,xctx->sym[i].rects[PINLAYER] );
  if(xctx->sym[i].base_name) fprintf(fd, "** sym_path: %s\n", abs_sym_path(xctx->sym[i].base_name, ""));
  else fprintf(fd, "** sym_path: %s\n", sanitized_abs_sym_path(name, ""));
  sym_def = get_tok_value(xctx->sym[i].prop_ptr,"spice_sym_def",0);
  if(sym_def[0]) {
    fprintf(fd, "%s\n", sym_def);
  } else {
    const char *s = sanitize(get_cell(name, 0));
    fprintf(fd, "** sch_path: %s\n", sanitized_abs_sym_path(filename, ""));
    fprintf(fd, ".subckt %s ", s);
    print_spice_subckt_nodes(fd, i);
  
    my_strdup(_ALLOC_ID_, &extra, get_tok_value(xctx->sym[i].prop_ptr,"extra",0) );
    /* this is now done in print_spice_subckt_nodes */
    /*
     * fprintf(fd, "%s ", extra ? extra : "" );
     */
   
    /* 20081206 new get_sym_template does not return token=value pairs where token listed in extra */
    fprintf(fd, "%s", get_sym_template(xctx->sym[i].templ, extra));
    my_free(_ALLOC_ID_, &extra);
    fprintf(fd, "\n");
  
    spice_stop ? load_schematic(0,filename, 0, 1) : load_schematic(1,filename, 0, 1);
    get_additional_symbols(1);
    err |= spice_netlist(fd, spice_stop);  /* 20111113 added spice_stop */
  
    if(xctx->schprop && xctx->schprop[0]) {
      fprintf(fd,"**** begin user architecture code\n");
      fprintf(fd, "%s\n", xctx->schprop);
      fprintf(fd,"**** end user architecture code\n");
    }
    fprintf(fd, ".ends\n\n");
  }
  if(split_f) {
    int save;
    fclose(fd);
    my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} noshow {%s}", netl_filename, cellname);
    save = xctx->netlist_type;
    xctx->netlist_type = CAD_SPICE_NETLIST;
    set_tcl_netlist_type();
    tcleval(tcl_cmd_netlist);
    xctx->netlist_type = save;
    set_tcl_netlist_type();
    if(debug_var==0) xunlink(netl_filename);
  }
  xctx->netlist_count++;
  my_free(_ALLOC_ID_, &name);
  return err;
}

/* GENERIC PURPOSE HASH TABLE */


/*    token        value      what    ... what ...
 * --------------------------------------------------------------------------
 * "whatever"    "whatever"  XINSERT     insert in hash table if not in.
 *                                      if already present update value if not NULL,
 *                                      return entry address.
 * "whatever"    "whatever"  XINSERT_NOREPLACE   same as XINSERT but do not replace existing value
 *                                      return NULL if not found.
 * "whatever"    "whatever"  XLOOKUP     lookup in hash table,return entry addr.
 *                                      return NULL if not found
 * "whatever"    "whatever"  XDELETE     delete entry if found,return NULL
 */
Str_hashentry *str_hash_lookup(Str_hashtable *hashtable, const char *token, const char *value, int what)
{
  unsigned int hashcode, idx;
  Str_hashentry *entry, *saveptr, **preventry;
  int s ;
  int size = hashtable->size;
  Str_hashentry **table = hashtable->table;

  if(token==NULL || size == 0 || table == NULL) return NULL;
  hashcode=str_hash(token);
  idx=hashcode % size;
  entry=table[idx];
  preventry=&table[idx];
  while(1)
  {
    if( !entry )          /* empty slot */
    {
      if(what==XINSERT || what == XINSERT_NOREPLACE)            /* insert data */
      {
        s=sizeof( Str_hashentry );
        entry=(Str_hashentry *)my_malloc(_ALLOC_ID_, s);
        entry->next=NULL;
        entry->token=NULL;
        entry->value=NULL;
        my_strdup(_ALLOC_ID_, &entry->token, token);
        my_strdup(_ALLOC_ID_, &entry->value, value);
        entry->hash=hashcode;
        *preventry=entry;
      }
      return NULL; /* if element inserted return NULL since it was not in table */
    }
    if( entry -> hash==hashcode && strcmp(token,entry->token)==0 ) /* found a matching token */
    {
      if(what==XDELETE)             /* remove token from the hash table ... */
      {
        saveptr=entry->next;
        my_free(_ALLOC_ID_, &entry->token);
        my_free(_ALLOC_ID_, &entry->value);
        my_free(_ALLOC_ID_, &entry);
        *preventry=saveptr;
      }
      else if(value && what == XINSERT ) {
        my_strdup(_ALLOC_ID_, &entry->value, value);
      }
      return entry;   /* found matching entry, return the address */
    }
    preventry=&entry->next; /* descend into the list. */
    entry = entry->next;
  }
}

void str_hash_init(Str_hashtable *hashtable, int size)
{
  if(hashtable->size !=0 || hashtable->table != NULL) {
    dbg(0, "str_hash_init(): Warning hash table not empty, possible data leak\n");
  }
  hashtable->size = size;
  hashtable->table = my_calloc(_ALLOC_ID_, size, sizeof(Str_hashentry *));
}

static void str_hash_free_entry(Str_hashentry *entry)
{
  Str_hashentry *tmp;
  while( entry ) {
    tmp = entry -> next;
    my_free(_ALLOC_ID_, &(entry->token));
    my_free(_ALLOC_ID_, &(entry->value));
    my_free(_ALLOC_ID_, &entry);
    entry = tmp;
  }
}


void str_hash_free(Str_hashtable *hashtable)
{
  if(hashtable->table) {
    int i;
    Str_hashentry **table = hashtable->table;
    for(i=0;i < hashtable->size; ++i)
    {
      str_hash_free_entry( table[i] );
      table[i] = NULL;
    }
    if(hashtable->table) my_free(_ALLOC_ID_, &(hashtable->table));
    hashtable->size = 0;
  }
}

/* GENERIC PURPOSE INT HASH TABLE */

/*    token        value      what    ... what ...
 * --------------------------------------------------------------------------
 * "whatever"    "whatever"  XINSERT     insert in hash table if not in.
 *                                       if already present update value if not NULL,
 *                                       return new entry address.
 * "whatever"    "whatever"  XINSERT_NOREPLACE   same as XINSERT but do not replace existing value
 *                                       return NULL if not found.
 * "whatever"    "whatever"  XLOOKUP     lookup in hash table,return entry addr.
 *                                       return NULL if not found
 * "whatever"    "whatever"  XDELETE     delete entry if found,return NULL
 */
Int_hashentry *int_hash_lookup(Int_hashtable *hashtable, const char *token, const int value, int what)
{
  unsigned int hashcode, idx;
  Int_hashentry *entry, *saveptr, **preventry;
  int s ;
  int size = hashtable->size;
  Int_hashentry **table = hashtable->table;

  if(token==NULL || size == 0 || table == NULL) return NULL;
  hashcode=str_hash(token);
  idx=hashcode % size;
  entry=table[idx];
  preventry=&table[idx];
  while(1)
  {
    if( !entry )          /* empty slot */
    {
      if(what==XINSERT || what == XINSERT_NOREPLACE)            /* insert data */
      {
        s=sizeof( Int_hashentry );
        entry=(Int_hashentry *)my_malloc(_ALLOC_ID_, s);
        entry->next=NULL;
        entry->token=NULL;
        my_strdup(_ALLOC_ID_, &entry->token, token);
        entry->value = value;
        entry->hash=hashcode;
        *preventry=entry;
      }
      return NULL; /* if element inserted return NULL since it was not in table */
    }
    if( entry -> hash==hashcode && strcmp(token,entry->token)==0 ) /* found a matching token */
    {
      if(what==XDELETE)             /* remove token from the hash table ... */
      {
        saveptr=entry->next;
        my_free(_ALLOC_ID_, &entry->token);
        my_free(_ALLOC_ID_, &entry);
        *preventry=saveptr;
      }
      else if(what == XINSERT ) {
        entry->value = value;
      }
      return entry;   /* found matching entry, return the address */
    }
    preventry=&entry->next; /* descend into the list. */
    entry = entry->next;
  }
}

void int_hash_init(Int_hashtable *hashtable, int size)
{
  if(hashtable->size !=0 || hashtable->table != NULL) {
    dbg(0, "int_hash_init(): Warning hash table not empty, possible data leak\n");
  }
  hashtable->size = size;
  hashtable->table = my_calloc(_ALLOC_ID_, size, sizeof(Int_hashentry *));
}

static void int_hash_free_entry(Int_hashentry *entry)
{
  Int_hashentry *tmp;
  while( entry ) {
    tmp = entry -> next;
    my_free(_ALLOC_ID_, &(entry->token));
    my_free(_ALLOC_ID_, &entry);
    entry = tmp;
  }
}


void int_hash_free(Int_hashtable *hashtable)
{
  if(hashtable->table) {
    int i;
    Int_hashentry **table = hashtable->table;
    for(i=0;i < hashtable->size; ++i)
    {
      int_hash_free_entry( table[i] );
      table[i] = NULL;
    }
    my_free(_ALLOC_ID_, &(hashtable->table));
    hashtable->size = 0;
  }
}



/* GENERIC PURPOSE PTR HASH TABLE */

/*    token        value      what    ... what ...
 * --------------------------------------------------------------------------
 * "whatever"    "whatever"  XINSERT     insert in hash table if not in.
 *                                       if already present update value if not NULL,
 *                                       return new entry address.
 * "whatever"    "whatever"  XINSERT_NOREPLACE   same as XINSERT but do not replace existing value
 *                                       return NULL if not found.
 * "whatever"    "whatever"  XLOOKUP     lookup in hash table,return entry addr.
 *                                       return NULL if not found
 * "whatever"    "whatever"  XDELETE     delete entry if found,return NULL
 */
Ptr_hashentry *ptr_hash_lookup(Ptr_hashtable *hashtable, const char *token, void *const value, int what)
{
  unsigned int hashcode, idx;
  Ptr_hashentry *entry, *saveptr, **preventry;
  int s ;
  int size = hashtable->size;
  Ptr_hashentry **table = hashtable->table;

  if(token==NULL || size == 0 || table == NULL) return NULL;
  hashcode=str_hash(token);
  idx=hashcode % size;
  entry=table[idx];
  preventry=&table[idx];
  while(1)
  {
    if( !entry )          /* empty slot */
    {
      if(what==XINSERT || what == XINSERT_NOREPLACE)            /* insert data */
      {
        s=sizeof( Ptr_hashentry );
        entry=(Ptr_hashentry *)my_malloc(_ALLOC_ID_, s);
        entry->next=NULL;
        entry->token=NULL;
        my_strdup(_ALLOC_ID_, &entry->token, token);
        entry->value = value;
        entry->hash=hashcode;
        *preventry=entry;
      }
      return NULL; /* if element inserted return NULL since it was not in table */
    }
    if( entry -> hash==hashcode && strcmp(token,entry->token)==0 ) /* found a matching token */
    {
      if(what==XDELETE)             /* remove token from the hash table ... */
      {
        saveptr=entry->next;
        my_free(_ALLOC_ID_, &entry->token);
        my_free(_ALLOC_ID_, &entry);
        *preventry=saveptr;
      }
      else if(what == XINSERT ) {
        entry->value = value;
      }
      return entry;   /* found matching entry, return the address */
    }
    preventry=&entry->next; /* descend into the list. */
    entry = entry->next;
  }
}

void ptr_hash_init(Ptr_hashtable *hashtable, int size)
{
  if(hashtable->size !=0 || hashtable->table != NULL) {
    dbg(0, "ptr_hash_init(): Warning hash table not empty, possible data leak\n");
  }
  hashtable->size = size;
  hashtable->table = my_calloc(_ALLOC_ID_, size, sizeof(Ptr_hashentry *));
}

static void ptr_hash_free_entry(Ptr_hashentry *entry)
{
  Ptr_hashentry *tmp;
  while( entry ) {
    tmp = entry -> next;
    my_free(_ALLOC_ID_, &(entry->token));
    my_free(_ALLOC_ID_, &entry);
    entry = tmp;
  }
}


void ptr_hash_free(Ptr_hashtable *hashtable)
{
  if(hashtable->table) {
    int i;
    Ptr_hashentry **table = hashtable->table;
    for(i=0;i < hashtable->size; ++i)
    {
      ptr_hash_free_entry( table[i] );
      table[i] = NULL;
    }
    my_free(_ALLOC_ID_, &(hashtable->table));
    hashtable->size = 0;
  }
}
