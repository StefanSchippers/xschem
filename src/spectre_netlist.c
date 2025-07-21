/* File: spice_netlist.c
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

static Str_hashtable model_table = {NULL, 0}; /* safe even with multiple schematics */

static char *model_name_result = NULL; /* safe even with multiple schematics */

static char *model_name(const char *m)
{
  char *m_lower = NULL;
  char *modelname = NULL;
  char *ptr;
  int n;
  size_t l = strlen(m) + 1;
  my_strdup(_ALLOC_ID_, &m_lower, m);
  strtolower(m_lower);
  my_realloc(_ALLOC_ID_, &modelname, l);
  my_realloc(_ALLOC_ID_, &model_name_result, l);
  if((ptr = strstr(m_lower, "subckt"))) {
    n = sscanf(ptr, "subckt %s %s", model_name_result, modelname);
  } else if((ptr = strstr(m_lower, "model"))) {
    n = sscanf(ptr, "model %s %s", model_name_result, modelname);
  } else {
     n = sscanf(m_lower, " %s %s", model_name_result, modelname);
  }
  if(n<2) my_strncpy(model_name_result, m_lower, l);
  else {
    /* build a hash key value with no spaces to make device_model attributes with different spaces equivalent*/
    my_strcat(_ALLOC_ID_, &model_name_result, modelname);
  }
  my_free(_ALLOC_ID_, &modelname);
  my_free(_ALLOC_ID_, &m_lower);
  return model_name_result;
}

static int spectre_netlist(FILE *fd, int spectre_stop )
{
  int err = 0;
  int i, flag = 0;
  const char *type;
  int lvs_netlist =  tclgetboolvar("lvs_netlist");
  int top_sub = lvs_netlist || tclgetboolvar("top_is_subckt");
  int lvs_ignore = tclgetboolvar("lvs_ignore");
  if(lvs_netlist) my_strdup(_ALLOC_ID_, &xctx->format, "lvs_format");
  else my_strdup(_ALLOC_ID_, &xctx->format, xctx->custom_format);
  if(!spectre_stop) {
    dbg(1, "spectre_netlist(): invoke prepare_netlist_structs for %s\n", xctx->current_name);
    xctx->prep_net_structs = 0;
    err |= prepare_netlist_structs(1);
    err |= traverse_node_hash();  /* print all warnings about unconnected floatings etc */
    for(i=0;i<xctx->instances; ++i) /* print first ipin/opin defs ... */
    {
     if(skip_instance(i, 1, lvs_ignore)) continue;
     type = (xctx->inst[i].ptr+ xctx->sym)->type;
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
         print_spectre_element(fd, i) ;  /* this is the element line  */
       }
     }
    }
    if(top_sub) fprintf(fd, "\n");
    for(i=0;i<xctx->instances; ++i) /* ... then print other lines */
    {
     if(skip_instance(i, 1, lvs_ignore)) continue;
     type = (xctx->inst[i].ptr+ xctx->sym)->type;
 
     if( type && !IS_LABEL_OR_PIN(type) ) {
       /* already done in global_spectre_netlist */
       if(!strcmp(type,"netlist_commands") && xctx->netlist_count==0) continue;
       if(xctx->netlist_count &&
          !strboolcmp(get_tok_value(xctx->inst[i].prop_ptr, "only_toplevel", 0), "true")) continue;
       if(!strcmp(type,"netlist_commands")) {
         fprintf(fd,"**** begin user architecture code\n");
         print_spectre_element(fd, i) ;  /* this is the element line  */
         fprintf(fd,"**** end user architecture code\n");
       } else {
         char *val = NULL;
         const char *m;
         if(print_spectre_element(fd, i)) {
           fprintf(fd, "**** end_element\n");
         }
         /* hash device_model attribute if any */
         my_strdup2(_ALLOC_ID_, &val, get_tok_value(xctx->inst[i].prop_ptr, "spectre_device_model", 2));
         m = val;
         if(strchr(val, '@')) m = translate(i, val);
         else m = tcl_hook2(m);
         if(m[0]) str_hash_lookup(&model_table, model_name(m), m, XINSERT);
         else {
           my_strdup2(_ALLOC_ID_, &val,
               get_tok_value(xctx->sym[xctx->inst[i].ptr].prop_ptr, "spectre_device_model", 2));
           m = val;
           if(strchr(val, '@')) m = translate(i, val);
           else m = tcl_hook2(m);
           if(m[0]) str_hash_lookup(&model_table, model_name(m), m, XINSERT);
         }
         my_free(_ALLOC_ID_, &model_name_result);
         my_free(_ALLOC_ID_, &val);
       }
     }
    }
  }
  if(!spectre_stop && !xctx->netlist_count) redraw_hilights(0); /* draw_hilight_net(1); */
  return err;
}

/* alert: if set show alert if file missing */
int global_spectre_netlist(int global, int alert)  /* netlister driver */
{
 int err = 0;
 int first;
 FILE *fd;
 const char *str_tmp;
 int multip;
 unsigned int *stored_flags;
 int i;
 const char *type;
 char *place=NULL;
 char netl_filename[PATH_MAX]; /* overflow safe 20161122 */
 char tcl_cmd_netlist[PATH_MAX + 100]; /* 20081211 overflow safe 20161122 */
 char cellname[PATH_MAX]; /* 20081211 overflow safe 20161122 */
 char *subckt_name;
 char *abs_path = NULL;
 int split_f;
 Str_hashtable subckt_table = {NULL, 0};
 Str_hashentry *model_entry;
 int save_prev_mod = xctx->prev_set_modify;
 struct stat buf;
 char *top_symbol_name = NULL;
 /* if top level has a symbol use it for pin ordering
  * top_symbol_name == 1: a symbol file matching schematic has been found.
  * top_symbol_name == 3: the found symbol has type=subcircuit and has ports */
 int found_top_symbol = 0;
 int npins = 0; /* top schematic number of i/o ports */
 Sch_pin_record *pinnumber_list = NULL; /* list of top sch i/o ports ordered wrt sim_pinnumber attr */
 int uppercase_subckt = tclgetboolvar("uppercase_subckt");
 int lvs_netlist =  tclgetboolvar("lvs_netlist");
 int top_sub = lvs_netlist || tclgetboolvar("top_is_subckt");
 int lvs_ignore = tclgetboolvar("lvs_ignore");

 if(lvs_netlist) my_strdup(_ALLOC_ID_, &xctx->format, "lvs_format");
 else  my_strdup(_ALLOC_ID_, &xctx->format, xctx->custom_format);
 exit_code = 0; /* reset exit code */
 split_f = tclgetboolvar("split_files");
 dbg(1, "global_spectre_netlist(): invoking push_undo()\n");
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
 dbg(1, "global_spectre_netlist(): opening %s for writing\n",netl_filename);
 fd=fopen(netl_filename, "w");
 if(fd==NULL) {
   dbg(0, "global_spectre_netlist(): problems opening netlist file\n");
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
  if(skip_instance(i, 1, lvs_ignore)) continue;
  type = (xctx->inst[i].ptr+ xctx->sym)->type;
  my_strdup(_ALLOC_ID_, &place,get_tok_value(xctx->sym[xctx->inst[i].ptr].prop_ptr,"place",0));
  if( type && !strcmp(type,"netlist_commands") ) {
   if(!place) {
     my_strdup(_ALLOC_ID_, &place,get_tok_value(xctx->inst[i].prop_ptr,"place",0));
   }
   if(place && !strcmp(place, "header" )) {
     if(first == 0) fprintf(fd,"**** begin user header code\n");
     ++first;
     print_spectre_element(fd, i) ;  /* this is the element line  */
   }
  }
 }
 if(first) fprintf(fd,"**** end user header code\n");

 /* netlist_options */
 for(i=0;i<xctx->instances; ++i) {
   if(skip_instance(i, 1, lvs_ignore)) continue;
   if(!(xctx->inst[i].ptr+ xctx->sym)->type) continue;
   if( !strcmp((xctx->inst[i].ptr+ xctx->sym)->type,"netlist_options") ) {
     netlist_options(i);
   }
 }
 if(!top_sub) fprintf(fd,"**");
 if(uppercase_subckt) 
   fprintf(fd,"SUBCKT %s ( ", get_cell(xctx->sch[xctx->currsch], 0));
 else
   fprintf(fd,"subckt %s ( ", get_cell(xctx->sch[xctx->currsch], 0));
 pinnumber_list = sort_schematic_pins(&npins); /* sort pins according to sim_pinnumber attr */

 /* print top subckt ipin/opins */
 my_strdup2(_ALLOC_ID_, &top_symbol_name, abs_sym_path(add_ext(xctx->current_name, ".sym"), ""));
 if(!stat(top_symbol_name, &buf)) { /* if top level has a symbol use the symbol for pin ordering */
   dbg(1, "found top level symbol %s\n", top_symbol_name);
   load_sym_def(top_symbol_name, NULL);
   found_top_symbol = 1;
   if(xctx->sym[xctx->symbols - 1].type != NULL && 
     /* only use the symbol if it has pins and is a subcircuit ? */
     /* !strcmp(xctx->sym[xctx->symbols - 1].type, "subcircuit") && */
       xctx->sym[xctx->symbols - 1].rects[PINLAYER] > 0) {
     fprintf(fd," ");
     print_spectre_subckt_nodes(fd, xctx->symbols - 1);
     found_top_symbol = 3;
     err |= sym_vs_sch_pins(xctx->symbols - 1);
   }
   remove_symbol(xctx->symbols - 1);
 }
 my_free(_ALLOC_ID_, &top_symbol_name);
 if(found_top_symbol != 3) {
   for(i=0;i<npins; ++i) {
     int n = pinnumber_list[i].n;
     str_tmp = expandlabel ( (xctx->inst[n].lab ? xctx->inst[n].lab : ""), &multip);
     /*must handle  invalid node names */
     fprintf(fd, " %s", str_tmp ? str_tmp : "<NULL>" );
   }
 }
 my_free(_ALLOC_ID_, &pinnumber_list);
 fprintf(fd,")\n");

 err |= spectre_netlist(fd, 0);

 first = 0;
 for(i=0;i<xctx->instances; ++i) /* print netlist_commands of top level cell with no 'place=end' property
                                   and no place=header */
 {
  if(skip_instance(i, 1, lvs_ignore)) continue;
  type = (xctx->inst[i].ptr+ xctx->sym)->type;
  my_strdup(_ALLOC_ID_, &place,get_tok_value(xctx->sym[xctx->inst[i].ptr].prop_ptr,"place",0));
  if( type && !strcmp(type,"netlist_commands") ) {
   if(!place) {
     my_strdup(_ALLOC_ID_, &place,get_tok_value(xctx->inst[i].prop_ptr,"place",0));
   }
   if(!place || (strcmp(place, "end") && strcmp(place, "header")) ) {
     if(first == 0) fprintf(fd,"**** begin user architecture code\n");
     ++first;
     print_spectre_element(fd, i) ;  /* this is the element line  */
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
 if(uppercase_subckt) 
   fprintf(fd, "ENDS\n");
 else
   fprintf(fd, "ends\n");


 if(split_f) {
   int save;
   fclose(fd);
   my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} noshow {%s}", netl_filename, cellname);
   save = xctx->netlist_type;
   xctx->netlist_type = CAD_SPECTRE_NETLIST;
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
   int web_url = is_from_web(xctx->current_dirname);
   char *current_dirname_save = NULL;

   my_strdup2(_ALLOC_ID_, &current_dirname_save, xctx->current_dirname);
   unselect_all(1);
   /* ensure all unused symbols purged before descending hierarchy */
   if(!tclgetboolvar("keep_symbols")) remove_symbols();
   /* reload data without popping undo stack, this populates embedded symbols if any */
   dbg(1, "global_spectre_netlist(): invoking pop_undo(2, 0)\n");
   xctx->pop_undo(2, 0);
   /* link_symbols_to_instances(-1); */ /* done in xctx->pop_undo() */
   my_strdup(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], xctx->sch_path[xctx->currsch]);
   my_strcat(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], "->netlisting");
   xctx->sch_path_hash[xctx->currsch+1] = 0;
   xctx->currsch++;
   subckt_name=NULL;
   dbg(2, "global_spectre_netlist(): last defined symbol=%d\n",xctx->symbols);
   get_additional_symbols(1);
   for(i=0;i<xctx->symbols; ++i)
   {
    if(xctx->sym[i].flags & (SPICE_IGNORE | SPICE_SHORT)) continue;
    if(lvs_ignore && (xctx->sym[i].flags & LVS_IGNORE)) continue;
    if(!xctx->sym[i].type) continue;
    /* store parent symbol template attr (before descending into it) and parent instance prop_ptr
     * into xctx->hier_attr[0].templ and xctx->hier_attr[0.prop_ptr,
     * to resolve subschematic instances with model=@modp in format string,
     * modp will be first looked up in instance prop_ptr string, and if not found
     * in parent symbol template string */
    my_strdup(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch - 1].templ,
              tcl_hook2(xctx->sym[i].templ));
    /* only additional symbols (created with instance schematic=... attr) will have this attribute */
    my_strdup(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch - 1].prop_ptr,
              tcl_hook2(xctx->sym[i].parent_prop_ptr));
    my_strdup(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch - 1].sym_extra, 
      get_tok_value(xctx->sym[i].prop_ptr, "extra", 0));
    my_strdup(_ALLOC_ID_, &abs_path, abs_sym_path(xctx->sym[i].name, ""));
    if(strcmp(xctx->sym[i].type,"subcircuit")==0 && check_lib(1, abs_path))
    {
      if(!web_url) {
        tclvareval("get_directory [list ", xctx->sch[xctx->currsch - 1], "]", NULL);
        my_strncpy(xctx->current_dirname, tclresult(),  S(xctx->current_dirname));
      }
      /* xctx->sym can be SCH or SYM, use hash to avoid writing duplicate subckt */
      my_strdup(_ALLOC_ID_, &subckt_name, get_cell(xctx->sym[i].name, 0));
      dbg(1, "global_spectre_netlist(): subckt_name=%s\n", subckt_name);
      if (str_hash_lookup(&subckt_table, subckt_name, "", XLOOKUP)==NULL)
      {
        /* do not insert symbols with default_schematic attribute set to ignore in hash since these symbols
         * will not be processed by *_block_netlist() */
        if(strcmp(get_tok_value(xctx->sym[i].prop_ptr, "default_schematic", 0), "ignore"))
          str_hash_lookup(&subckt_table, subckt_name, "", XINSERT);
        if( split_f && strboolcmp(get_tok_value(xctx->sym[i].prop_ptr,"vhdl_netlist",0),"true")==0 )
          err |= vhdl_block_netlist(fd, i, alert);
        else if(split_f && strboolcmp(get_tok_value(xctx->sym[i].prop_ptr,"verilog_netlist",0),"true")==0 )
          err |= verilog_block_netlist(fd, i, alert);
        else if(split_f && strboolcmp(get_tok_value(xctx->sym[i].prop_ptr,"spice_netlist",0),"true")==0 )
          err |= spice_block_netlist(fd, i, alert);
        else
          if( strboolcmp(get_tok_value(xctx->sym[i].prop_ptr,"spectre_primitive",0),"true") )
            err |= spectre_block_netlist(fd, i, alert);
      }
    }
   }
   if(xctx->hier_attr[xctx->currsch - 1].templ)
     my_free(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch - 1].templ);
   if(xctx->hier_attr[xctx->currsch - 1].prop_ptr)
     my_free(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch - 1].prop_ptr);
   if(xctx->hier_attr[xctx->currsch - 1].sym_extra)
     my_free(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch - 1].sym_extra);
   my_free(_ALLOC_ID_, &abs_path);
   /* get_additional_symbols(0); */
   my_free(_ALLOC_ID_, &subckt_name);
   /*clear_drawing(); */
   my_free(_ALLOC_ID_, &xctx->sch[xctx->currsch]);
   xctx->currsch--;
   unselect_all(1);
   dbg(1, "global_spectre_netlist(): invoking pop_undo(0, 0)\n");
   /* symbol vs schematic pin check, we do it here since now we have ALL symbols loaded */
   err |= sym_vs_sch_pins(-1);
   if(!tclgetboolvar("keep_symbols")) remove_symbols();
   xctx->pop_undo(4, 0);
   xctx->prev_set_modify = save_prev_mod;
   if(web_url) {
     my_strncpy(xctx->current_dirname, current_dirname_save, S(xctx->current_dirname));
   } else {
     tclvareval("get_directory [list ", xctx->sch[xctx->currsch], "]", NULL);
     my_strncpy(xctx->current_dirname, tclresult(),  S(xctx->current_dirname));
   }
   my_strncpy(xctx->current_name, rel_sym_path(xctx->sch[xctx->currsch]), S(xctx->current_name));
   dbg(1, "spectre_netlist(): invoke prepare_netlist_structs for %s\n", xctx->current_name);
   err |= prepare_netlist_structs(1); /* so 'lab=...' attributes for unnamed nets are set */
   if(!xctx->hilight_nets) xctx->hilight_nets = saved_hilight_nets;
   my_free(_ALLOC_ID_, &current_dirname_save);
 }
 /* restore hilight flags from errors found analyzing top level before descending hierarchy */
 for(i=0;i<xctx->instances; ++i) if(!xctx->inst[i].color) xctx->inst[i].color = stored_flags[i];
 propagate_hilights(1, 0, XINSERT_NOREPLACE);
 draw_hilight_net(1);
 my_free(_ALLOC_ID_, &stored_flags);

 /* print globals nodes found in netlist 28032003 */
 if(!split_f) {
   record_global_node(0,fd,NULL);
   /* record_global_node(2, NULL, NULL); */ /* delete list --> do it in xwin_exit() */
 }

 /* =================================== 20121223 */
 first = 0;
 if(!split_f) {
   for(i=0;i<xctx->instances; ++i) /* print netlist_commands of top level cell with 'place=end' property */
   {
    if(skip_instance(i, 1, lvs_ignore)) continue;
    type = (xctx->inst[i].ptr+ xctx->sym)->type;
    my_strdup(_ALLOC_ID_, &place,get_tok_value(xctx->sym[xctx->inst[i].ptr].prop_ptr,"place",0));
    if( type && !strcmp(type,"netlist_commands") ) {
     if(place && !strcmp(place, "end" )) {
       if(first == 0) fprintf(fd,"**** begin user architecture code\n");
       ++first;
       print_spectre_element(fd, i) ;
     } else {
       my_strdup(_ALLOC_ID_, &place,get_tok_value(xctx->inst[i].prop_ptr,"place",0));
       if(place && !strcmp(place, "end" )) {
         if(first == 0) fprintf(fd,"**** begin user architecture code\n");
         ++first;
         print_spectre_element(fd, i) ;
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
 if( !top_sub && !split_f) fprintf(fd, ".end\n");

 dbg(1, "global_spectre_netlist(): starting awk on netlist!\n");


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
 my_free(_ALLOC_ID_, &place);
 xctx->netlist_count = 0;
 tclvareval("show_infotext ", my_itoa(err), NULL); /* critical error: force ERC window showing */
 exit_code = err ? 10 : 0;
 return err;
}

/* alert: if set show alert if file missing */
int spectre_block_netlist(FILE *fd, int i, int alert)
{
  int err = 0;
  int spectre_stop=0;
  char netl_filename[PATH_MAX];
  char tcl_cmd_netlist[PATH_MAX + 100];
  char cellname[PATH_MAX];
  char filename[PATH_MAX];
  /* int j; */
  /* int multip; */
  char *extra=NULL;
  int split_f;
  char *sym_def = NULL;
  char *name = NULL;
  const char *default_schematic;
  int uppercase_subckt = tclgetboolvar("uppercase_subckt");
 
  split_f = tclgetboolvar("split_files");

  if(!strboolcmp( get_tok_value(xctx->sym[i].prop_ptr,"spectre_stop",0),"true") )
     spectre_stop=1;
  else
     spectre_stop=0;
  if(!strcmp(get_tok_value(xctx->sym[i].prop_ptr, "format", 0), "")) {
    return err;
  }
  get_sch_from_sym(filename, xctx->sym + i, -1, 0);
  default_schematic = get_tok_value(xctx->sym[i].prop_ptr, "default_schematic", 0);
  if(!strcmp(default_schematic, "ignore")) {
    return err;
  }
  my_strdup(_ALLOC_ID_, &name, tcl_hook2(xctx->sym[i].name));

  dbg(1, "spectre_block_netlist(): filename=%s\n", filename);
  if(split_f) {
    my_snprintf(netl_filename, S(netl_filename), "%s/.%s_%d",
         tclgetvar("netlist_dir"), get_cell(name, 0), getpid());
    dbg(1, "spectre_block_netlist(): split_files: netl_filename=%s\n", netl_filename);
    fd=fopen(netl_filename, "w");
    if(!fd) {
      dbg(0, "spectre_block_netlist(): unable to write file %s\n", netl_filename);
      err = 1;
      goto err;
    }
    my_snprintf(cellname, S(cellname), "%s.spice", get_cell(name, 0));
  }
  fprintf(fd, "\n* expanding   symbol:  %s # of pins=%d\n", name,xctx->sym[i].rects[PINLAYER] );
  if(xctx->sym[i].base_name) fprintf(fd, "** sym_path: %s\n", abs_sym_path(xctx->sym[i].base_name, ""));
  else fprintf(fd, "** sym_path: %s\n", sanitized_abs_sym_path(name, ""));
  my_strdup(_ALLOC_ID_, &sym_def, get_tok_value(xctx->sym[i].prop_ptr,"spectre_sym_def",0));
  if(sym_def) {
    char *symname_attr = NULL;
    const char *translated_sym_def;
    my_mstrcat(_ALLOC_ID_, &symname_attr, "symname=", get_cell(name, 0), NULL);
    translated_sym_def = translate3(sym_def, 1, xctx->sym[i].templ, symname_attr, NULL, NULL);
    my_free(_ALLOC_ID_, &symname_attr);
    fprintf(fd, "%s\n", translated_sym_def);
    my_free(_ALLOC_ID_, &sym_def);
  } else {
    const char *s = get_cell(sanitize(name), 0);
    fprintf(fd, "** sch_path: %s\n", sanitized_abs_sym_path(filename, ""));
    if(uppercase_subckt)
      fprintf(fd, "SUBCKT %s ( ", s);
     else
      fprintf(fd, "subckt %s ( ", s);
    print_spectre_subckt_nodes(fd, i);
    fprintf(fd, ") ");
    my_strdup(_ALLOC_ID_, &extra, get_tok_value(xctx->sym[i].prop_ptr,"extra",0) );
    /* this is now done in print_spectre_subckt_nodes */
    /*
     * fprintf(fd, "%s ", extra ? extra : "" );
     */
   
    /* 20081206 new get_sym_template does not return token=value pairs where token listed in extra */
    fprintf(fd, "\nparameters %s", get_sym_template(xctx->sym[i].templ, extra));
    my_free(_ALLOC_ID_, &extra);
    fprintf(fd, "\n");
  
    spectre_stop ? load_schematic(0,filename, 0, alert) : load_schematic(1,filename, 0, alert);
    get_additional_symbols(1);
    err |= spectre_netlist(fd, spectre_stop);  /* 20111113 added spectre_stop */
    err |= warning_overlapped_symbols(0);
    if(xctx->schprop && xctx->schprop[0]) {
      fprintf(fd,"**** begin user architecture code\n");
      fprintf(fd, "%s\n", xctx->schprop);
      fprintf(fd,"**** end user architecture code\n");
    }
    if(uppercase_subckt)
      fprintf(fd, "ENDS\n\n");
    else
      fprintf(fd, "ends\n\n");
  }
  if(split_f) {
    int save;
    fclose(fd);
    my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} noshow {%s}", netl_filename, cellname);
    save = xctx->netlist_type;
    xctx->netlist_type = CAD_SPECTRE_NETLIST;
    set_tcl_netlist_type();
    tcleval(tcl_cmd_netlist);
    xctx->netlist_type = save;
    set_tcl_netlist_type();
    if(debug_var==0) xunlink(netl_filename);
  }
  err:
  xctx->netlist_count++;
  my_free(_ALLOC_ID_, &name);
  return err;
}

