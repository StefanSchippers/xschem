/* File: vhdl_netlist.c
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

static int vhdl_netlist(FILE *fd , int vhdl_stop)
{
 int err = 0;
 int i,l;
 char *type=NULL;
 int lvs_ignore = tclgetboolvar("lvs_ignore");

 if(!vhdl_stop) {
   xctx->prep_net_structs = 0;
   err |= prepare_netlist_structs(1);
   err |= traverse_node_hash();  /* print all warnings about unconnected floatings etc */
 }

 dbg(1, "vhdl_netlist():       architecture declarations\n");
 fprintf(fd, "//// begin user declarations\n");
 for(l=0;l<xctx->instances; ++l)
 {
  if(skip_instance(l, 1, lvs_ignore)) continue;
  if(!(xctx->inst[l].ptr+ xctx->sym)->type) continue;

  if(!strcmp((xctx->inst[l].ptr+ xctx->sym)->type, "arch_declarations") )
   fprintf(fd, "%s\n", xctx->inst[l].prop_ptr?  xctx->inst[l].prop_ptr: "");
 }
 fprintf(fd, "//// end user declarations\n");

 dbg(1, "vhdl_netlist():       print erc checks\n");
 if(!vhdl_stop) print_vhdl_signals(fd);
 dbg(1, "vhdl_netlist():       done print erc checks\n");

 dbg(1, "vhdl_netlist():       attributes\n");
 fprintf(fd, "//// begin user attributes\n");
 for(l=0;l<xctx->instances; ++l)
 {
  if(skip_instance(l, 1, lvs_ignore)) continue;
  my_strdup(_ALLOC_ID_, &type,(xctx->inst[l].ptr+ xctx->sym)->type);
  if( type && (strcmp(type,"attributes"))==0)
  {
   if(xctx->inst[l].prop_ptr) fprintf(fd, "\n%s\n", xctx->inst[l].prop_ptr);
  }
 }
 fprintf(fd, "//// end user attributes\n");


 fprintf(fd, "begin\n"); /* begin reintroduced 09122003 */
 if(!vhdl_stop)
 {
   for(i=0;i<xctx->instances; ++i) /* ... print all element except ipin opin labels use package */
   {                       /* dont print elements with vhdl_ignore=true set in symbol */
    if(skip_instance(i, 1, lvs_ignore)) continue;
    dbg(2, "vhdl_netlist():       into the netlisting loop\n");
    my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
    if( type &&
       ( !IS_LABEL_SH_OR_PIN(type) &&
         strcmp(type,"generic")&&
         strcmp(type,"use")&&
         strcmp(type,"netlist_commands")&&
         strcmp(type,"package")  &&
         strcmp(type,"attributes")  &&
         strcmp(type,"port_attributes")  &&
         strcmp(type,"arch_declarations")
       ))
    {
     if(xctx->lastsel)
     {
      if(xctx->inst[i].sel==SELECTED) {
        print_vhdl_element(fd, i);
      }
     } else {
        print_vhdl_element(fd, i);
     }
    }
   }
 }
 if(type) my_free(_ALLOC_ID_, &type);
 dbg(1, "vhdl_netlist():       end\n");
 if(!vhdl_stop && !xctx->netlist_count) redraw_hilights(0); /* draw_hilight_net(1); */
 return err;
}

int global_vhdl_netlist(int global)  /* netlister driver */
{
 int err = 0;
 FILE *fd;
 const char *str_tmp;
 char *dir_tmp = NULL;
 char *sig_type = NULL;
 char *port_value = NULL;
 int i,j, tmp;
 unsigned int *stored_flags;
 char netl_filename[PATH_MAX];   /* overflow safe 20161122 */
 char tcl_cmd_netlist[PATH_MAX + 100];  /* 20081202 overflow safe 20161122 */
 char cellname[PATH_MAX];  /* 20081202 overflow safe 20161122 */
 char *type=NULL;
 struct stat buf;
 char *subckt_name;
 char *abs_path = NULL;
 int split_f;
 Str_hashtable subckt_table = {NULL, 0};
 int lvs_ignore = tclgetboolvar("lvs_ignore");
 int save_prev_mod = xctx->prev_set_modify;

 exit_code = 0; /* reset exit code */
 split_f = tclgetboolvar("split_files");
 xctx->push_undo();
 xctx->netlist_unconn_cnt=0; /* unique count of unconnected pins while netlisting */
 statusmsg("",2);  /* clear infowindow */
 /* top sch properties used for library use declarations and type definitions */
 /* to be printed before any entity declarations */

 xctx->netlist_count=0;
 str_hash_init(&subckt_table, HASHSIZE);
 my_snprintf(netl_filename, S(netl_filename), "%s/.%s_%d", 
   tclgetvar("netlist_dir"), get_cell(xctx->sch[xctx->currsch], 0), getpid());
 fd=fopen(netl_filename, "w");

 if(fd==NULL){
   dbg(0, "global_vhdl_netlist(): problems opening netlist file\n");
   return 1;
 }
 fprintf(fd, "-- sch_path: %s\n", xctx->sch[xctx->currsch]);

 if(xctx->netlist_name[0]) {
   my_snprintf(cellname, S(cellname), "%s", get_cell_w_ext(xctx->netlist_name, 0));
 } else {
   my_snprintf(cellname, S(cellname), "%s.vhdl", get_cell(xctx->sch[xctx->currsch], 0));
 }

 dbg(1, "global_vhdl_netlist(): opening %s for writing\n",netl_filename);

 dbg(1, "global_vhdl_netlist(): printing top level packages\n");
  for(i=0;i<xctx->instances; ++i)
  {
   if(skip_instance(i, 1, lvs_ignore)) continue;
   my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
   if( type && (strcmp(type,"package"))==0)
   {
    if(xctx->inst[i].prop_ptr) {                          /* */
      fprintf(fd, "//// begin package\n");              /* 20080213 mark start of packages */
      fprintf(fd, "%s\n", xctx->inst[i].prop_ptr);
      fprintf(fd, "//// end package\n");                /* 20080213 mark end of packages */
    }
   }
  }

 dbg(1, "global_vhdl_netlist(): printing top level use statements\n");
  for(i=0;i<xctx->instances; ++i)
  {
   if(skip_instance(i, 1, lvs_ignore)) continue;
   my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
   if( type && (strcmp(type,"use"))==0)
   {
    if(xctx->inst[i].prop_ptr) fprintf(fd, "%s\n", xctx->inst[i].prop_ptr);
   }
  }

 dbg(1, "global_vhdl_netlist(): printing top level entity\n");

 /* 20071015 already done in print_generic() */
 /* fprintf(fd," --- entity %s is\n", get_cell( xctx->sch[xctx->currsch], 0) ); */
 /* 20071015 end */

 /* flush data structures (remove unused symbols) */
 unselect_all(1);
 if(!tclgetboolvar("keep_symbols")) remove_symbols();
 /* reload data without popping undo stack, this populates embedded symbols if any */
 xctx->pop_undo(2, 0);
 /* link_symbols_to_instances(-1); */ /* done in xctx->pop_undo() */

 /* 20071009 print top level generics if defined in symbol */
 str_tmp = add_ext(xctx->sch[xctx->currsch], ".sym");
 if(!stat(str_tmp, &buf)) {
   load_sym_def(str_tmp, NULL);
   print_generic(fd,"entity", xctx->symbols-1);  /* added print top level params */
   remove_symbol(xctx->symbols - 1);
 } else {
    fprintf(fd,"entity %s is\n", get_cell( xctx->sch[xctx->currsch], 0) );
 }
 /* 20071009 end */


 /* 20071009 get generics from symbol */
 /* print top subckt generics */
 /* dbg(1, "global_vhdl_netlist(): printing top level generic pins\n"); */
 /* tmp=0; */
 /* for(i=0;i<xctx->instances; ++i) */
 /* { */
 /*  if(xctx->inst[i].ptr<0) continue; */
 /*  my_strdup(xxx, &type,(xctx->inst[i].ptr+ xctx->sym)->type); */
 /*  my_strdup(xxx, &sig_type,get_tok_value(xctx->inst[i].prop_ptr,"generic_type",0)); */
 /*  if(!sig_type || sig_type[0]=='\0') my_strdup(xxx, &sig_type,"std_logic"); */
 /*  if( type && (strcmp(type,"generic"))==0) */
 /*  { */
 /*   my_strdup(xxx, &port_value,get_tok_value(xctx->inst[i].prop_ptr,"value", 0)); */
 /*   str_tmp = xctx->inst[i].lab ? xctx->inst[i].lab : ""; */
 /*   if(!tmp)  fprintf(fd,"generic (\n"); */
 /*   if(tmp) fprintf(fd," ;\n"); */
 /*   fprintf(fd, "  %s : %s", str_tmp ? str_tmp : "(NULL)", sig_type ); */
 /*   if(port_value &&port_value[0]) */
 /*      fprintf(fd," := %s", port_value); */
 /* */
 /*   tmp=1; */
 /*  } */
 /* } */
 /* if(tmp) fprintf(fd, "\n);\n"); */



 /* print top subckt ipin/opins */
 dbg(1, "global_vhdl_netlist(): printing top level out pins\n");
 tmp=0;
 for(i=0;i<xctx->instances; ++i)
 {
  if(skip_instance(i, 1, lvs_ignore)) continue;
  my_strdup(_ALLOC_ID_, &sig_type,get_tok_value(xctx->inst[i].prop_ptr,"sig_type",0));
  if(!sig_type || sig_type[0]=='\0') my_strdup(_ALLOC_ID_, &sig_type,"std_logic");
  my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  if( type && (strcmp(type,"opin"))==0)
  {
   str_tmp = xctx->inst[i].lab ? xctx->inst[i].lab : "";
   if(!tmp)  fprintf(fd,"port(\n");
   if(tmp) fprintf(fd," ;\n");
   fprintf(fd, "  %s : out %s", str_tmp ? str_tmp : "(NULL)", sig_type );
   tmp=1;
  }
 }

 dbg(1, "global_vhdl_netlist(): printing top level inout pins\n");
 for(i=0;i<xctx->instances; ++i)
 {
  if(skip_instance(i, 1, lvs_ignore)) continue;
  my_strdup(_ALLOC_ID_, &sig_type,get_tok_value(xctx->inst[i].prop_ptr,"sig_type",0));
  if(!sig_type || sig_type[0]=='\0') my_strdup(_ALLOC_ID_, &sig_type,"std_logic");
  my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  if( type && (strcmp(type,"iopin"))==0)
  {
   str_tmp = xctx->inst[i].lab ? xctx->inst[i].lab : "";
   if(!tmp)  fprintf(fd,"port(\n");
   if(tmp) fprintf(fd," ;\n");
   fprintf(fd, "  %s : inout %s", str_tmp ? str_tmp : "(NULL)", sig_type );
   tmp=1;
  }
 }

 dbg(1, "global_vhdl_netlist(): printing top level input pins\n");
 for(i=0;i<xctx->instances; ++i)
 {
  if(skip_instance(i, 1, lvs_ignore)) continue;
  my_strdup(_ALLOC_ID_, &sig_type,get_tok_value(xctx->inst[i].prop_ptr,"sig_type",0));
  if(!sig_type || sig_type[0]=='\0') my_strdup(_ALLOC_ID_, &sig_type,"std_logic");
  my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  if( type && (strcmp(type,"ipin"))==0)
  {
   str_tmp = xctx->inst[i].lab ? xctx->inst[i].lab : "";
   if(!tmp)  fprintf(fd,"port(\n");
   if(tmp) fprintf(fd," ;\n");
   fprintf(fd, "  %s :  in %s", str_tmp ? str_tmp : "<NULL>", sig_type );
   tmp=1;
  }
 }
 if(tmp) fprintf(fd,"\n);\n");

 dbg(1, "global_vhdl_netlist(): printing top level port attributes\n");
  for(i=0;i<xctx->instances; ++i)
  {
   if(skip_instance(i, 1, lvs_ignore)) continue;
   my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
   if( type && (strcmp(type,"port_attributes"))==0)
   {
    if(xctx->inst[i].prop_ptr) fprintf(fd, "%s\n", xctx->inst[i].prop_ptr);
   }
  }

 fprintf(fd,"end %s ;\n\n", get_cell( xctx->sch[xctx->currsch], 0) );
 fprintf(fd,"architecture arch_%s of %s is\n\n",
        sanitize(get_cell( xctx->sch[xctx->currsch], 0)),
        sanitize(get_cell( xctx->sch[xctx->currsch], 0)));

 dbg(1, "global_vhdl_netlist(): printing top level used components\n");
 /* print all components */
 subckt_name=NULL;
 get_additional_symbols(1);
 for(j=0;j<xctx->symbols; ++j)
 {
  if( strboolcmp(get_tok_value(xctx->sym[j].prop_ptr,"vhdl_primitive",0),"true")==0 ) continue;
  if(xctx->sym[j].flags & (VHDL_IGNORE | VHDL_SHORT)) continue;
  if(lvs_ignore && (xctx->sym[j].flags & LVS_IGNORE)) continue;
  if(!xctx->sym[j].type || (strcmp(xctx->sym[j].type,"primitive")!=0 &&
     strcmp(xctx->sym[j].type,"subcircuit")!=0)) continue;
  my_strdup(_ALLOC_ID_, &abs_path, abs_sym_path(tcl_hook2(xctx->sym[j].name), ""));
  if((
      strcmp(xctx->sym[j].type,"subcircuit")==0 ||
      strcmp(xctx->sym[j].type,"primitive")==0
     ) && check_lib(1, abs_path)
    )
  {
   /* xctx->sym can be SCH or SYM, use hash to avoid writing duplicate subckt */
   my_strdup(_ALLOC_ID_, &subckt_name, get_cell(xctx->sym[j].name, 0));
   if (str_hash_lookup(&subckt_table, subckt_name, "", XLOOKUP)==NULL) {
     Int_hashtable table = {NULL, 0};
     str_hash_lookup(&subckt_table, subckt_name, "", XINSERT);
     /* component generics */
     print_generic(fd,"component", j);

     /* component ports */
     tmp=0;
     int_hash_init(&table, 37);
     for(i=0;i<xctx->sym[j].rects[PINLAYER]; ++i)
     {
       if(strboolcmp(get_tok_value(xctx->sym[j].rect[PINLAYER][i].prop_ptr,"vhdl_ignore",0), "true")) {
         my_strdup(_ALLOC_ID_, &sig_type,
            get_tok_value( xctx->sym[j].rect[PINLAYER][i].prop_ptr,"sig_type",0));
         my_strdup(_ALLOC_ID_, &port_value,
            get_tok_value(xctx->sym[j].rect[PINLAYER][i].prop_ptr,"value", 0) );
         if(!sig_type || sig_type[0]=='\0') my_strdup(_ALLOC_ID_, &sig_type,"std_logic");
         my_strdup(_ALLOC_ID_, &dir_tmp, get_tok_value(xctx->sym[j].rect[PINLAYER][i].prop_ptr,"dir",0) );
         str_tmp = get_tok_value(xctx->sym[j].rect[PINLAYER][i].prop_ptr,"name",0);
         if(!int_hash_lookup(&table, str_tmp, 1, XINSERT_NOREPLACE)) {
           if(!tmp) fprintf(fd, "port (\n");
           if(tmp) fprintf(fd, " ;\n");
           fprintf(fd,"  %s : %s %s",str_tmp, dir_tmp ? dir_tmp : "<NULL>", sig_type);
           if(port_value &&port_value[0])
             fprintf(fd," := %s", port_value);
           tmp=1;
         }
         my_free(_ALLOC_ID_, &dir_tmp);
       }
     }
     int_hash_free(&table);
     if(tmp) fprintf(fd, "\n);\n");
     fprintf(fd, "end component ;\n\n");
   }
  }
  my_free(_ALLOC_ID_, &abs_path);
 }
 get_additional_symbols(0);
 str_hash_free(&subckt_table);
 my_free(_ALLOC_ID_, &subckt_name);

 dbg(1, "global_vhdl_netlist(): netlisting  top level\n");
 err |= vhdl_netlist(fd, 0);
 fprintf(fd,"//// begin user architecture code\n");

 for(i=0;i<xctx->instances; ++i) {
   if(skip_instance(i, 1, lvs_ignore)) continue;
   my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
   if(type && !strcmp(type,"netlist_commands")) {
     fprintf(fd, "%s\n", get_tok_value(xctx->inst[i].prop_ptr,"value", 0));
   }
 }

 if(xctx->schvhdlprop && xctx->schvhdlprop[0]) {
   fprintf(fd, "%s\n", xctx->schvhdlprop);
 }
 fprintf(fd, "end arch_%s ;\n\n", get_cell( xctx->sch[xctx->currsch], 0) );

 if(split_f) {
   int save;
   fclose(fd);
   my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} noshow {%s}", netl_filename, cellname);
   save = xctx->netlist_type;
   xctx->netlist_type = CAD_VHDL_NETLIST;
   set_tcl_netlist_type();
   tcleval(tcl_cmd_netlist);
   xctx->netlist_type = save;
   set_tcl_netlist_type();
   if(debug_var==0) xunlink(netl_filename);
 }
 xctx->netlist_count++;

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

   str_hash_init(&subckt_table, HASHSIZE);
   unselect_all(1);
   /* ensure all unused symbols purged before descending hierarchy */
   if(!tclgetboolvar("keep_symbols")) remove_symbols();
   /* reload data without popping undo stack, this populates embedded symbols if any */
   xctx->pop_undo(2, 0);
   /* link_symbols_to_instances(-1); */ /* done in xctx->pop_undo() */
   my_strdup(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], xctx->sch_path[xctx->currsch]);
   my_strcat(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], "->netlisting");
   xctx->sch_path_hash[xctx->currsch+1] = 0;
   xctx->currsch++;

    dbg(2, "global_vhdl_netlist(): last defined symbol=%d\n",xctx->symbols);
   subckt_name=NULL;
   get_additional_symbols(1);
   for(i=0;i<xctx->symbols; ++i)
   {
    if(xctx->sym[i].flags & (VHDL_IGNORE | VHDL_SHORT)) continue;
    if(lvs_ignore && (xctx->sym[i].flags & LVS_IGNORE)) continue;
    if(!xctx->sym[i].type) continue;
    my_strdup(_ALLOC_ID_, &abs_path, abs_sym_path(xctx->sym[i].name, ""));
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
        /* do not insert symbols with default_schematic attribute set to ignore in hash since these symbols
         * will not be processed by *_block_netlist() */
        if(strcmp(get_tok_value(xctx->sym[i].prop_ptr, "default_schematic", 0), "ignore"))
          str_hash_lookup(&subckt_table, subckt_name, "", XINSERT);
        if( split_f && strboolcmp(get_tok_value(xctx->sym[i].prop_ptr,"verilog_netlist",0),"true")==0 )
          err |= verilog_block_netlist(fd, i);
        else if( split_f && strboolcmp(get_tok_value(xctx->sym[i].prop_ptr,"spice_netlist",0),"true")==0 )
          err |= spice_block_netlist(fd, i);
        else if( strboolcmp(get_tok_value(xctx->sym[i].prop_ptr,"vhdl_primitive",0),"true"))
          err |= vhdl_block_netlist(fd, i);
      }
    }
   }
   my_free(_ALLOC_ID_, &abs_path);
   /* can not free additional syms since *_block_netlist() may have loaded additional syms */
   /* get_additional_symbols(0); */
   str_hash_free(&subckt_table);
   my_free(_ALLOC_ID_, &subckt_name);
   my_free(_ALLOC_ID_, &xctx->sch[xctx->currsch]);
   xctx->currsch--;
   unselect_all(1);
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
   err |= prepare_netlist_structs(1); /* so 'lab=...' attributes for unnamed nets are set */
   if(!xctx->hilight_nets) xctx->hilight_nets = saved_hilight_nets;
   my_free(_ALLOC_ID_, &current_dirname_save);
 }
 /* restore hilight flags from errors found analyzing top level before descending hierarchy */
 for(i=0;i<xctx->instances; ++i) if(!xctx->inst[i].color) xctx->inst[i].color = stored_flags[i];

 propagate_hilights(1, 0, XINSERT_NOREPLACE);
 draw_hilight_net(1);
 my_free(_ALLOC_ID_, &stored_flags);
 dbg(1, "global_vhdl_netlist(): starting awk on netlist!\n");
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
 my_free(_ALLOC_ID_, &sig_type);
 my_free(_ALLOC_ID_, &type);
 my_free(_ALLOC_ID_, &port_value);
 xctx->netlist_count = 0;
 tclvareval("show_infotext ", my_itoa(err), NULL); /* critical error: force ERC window showing */
 exit_code = err ? 10 : 0;
 return err;
}

int vhdl_block_netlist(FILE *fd, int i)
{
  int err = 0;
  int j,k,l, tmp, found;
  int vhdl_stop=0;
  char *dir_tmp = NULL;
  char *sig_type = NULL;
  char *port_value = NULL;
  char *type=NULL;
  char filename[PATH_MAX];
  char netl_filename[PATH_MAX];
  char tcl_cmd_netlist[PATH_MAX + 100];
  char cellname[PATH_MAX];
  char *abs_path = NULL;
  const char *str_tmp;
  int split_f;
  const char *sym_def;
  int lvs_ignore = tclgetboolvar("lvs_ignore");
  const char *default_schematic;

  split_f = tclgetboolvar("split_files");
  if(!strboolcmp( get_tok_value(xctx->sym[i].prop_ptr,"vhdl_stop",0),"true") )
    vhdl_stop=1;
  else
    vhdl_stop=0;
  get_sch_from_sym(filename, xctx->sym + i, -1, 0);

  default_schematic = get_tok_value(xctx->sym[i].prop_ptr, "default_schematic", 0);
  if(!strcmp(default_schematic, "ignore")) {
    return err;
  }

  if(split_f) {
    my_snprintf(netl_filename, S(netl_filename), "%s/.%s_%d",
       tclgetvar("netlist_dir"), get_cell(xctx->sym[i].name, 0), getpid());
    dbg(1, "vhdl_block_netlist(): split_files: netl_filename=%s\n", netl_filename);
    fd=fopen(netl_filename, "w");
    my_snprintf(cellname, S(cellname), "%s.vhdl", get_cell(xctx->sym[i].name, 0) );
  }

  dbg(1, "vhdl_block_netlist(): expanding %s\n",  xctx->sym[i].name);
  fprintf(fd, "\n-- expanding   symbol:  %s # of pins=%d\n",
        xctx->sym[i].name,xctx->sym[i].rects[PINLAYER] );
  if(xctx->sym[i].base_name) fprintf(fd, "-- sym_path: %s\n", abs_sym_path(xctx->sym[i].base_name, ""));
  else fprintf(fd, "-- sym_path: %s\n", sanitized_abs_sym_path(xctx->sym[i].name, ""));
  sym_def = get_tok_value(xctx->sym[i].prop_ptr,"vhdl_sym_def",0);
  if(sym_def[0]) {
    fprintf(fd, "%s\n", sym_def);
  } else {
    Int_hashtable table = {NULL, 0};
    fprintf(fd, "-- sch_path: %s\n", sanitized_abs_sym_path(filename, ""));
    load_schematic(1,filename, 0, 1);
    get_additional_symbols(1);
    dbg(1, "vhdl_block_netlist():       packages\n");
    for(l=0;l<xctx->instances; ++l)
    {
     if(skip_instance(l, 1, lvs_ignore)) continue;
     if(!(xctx->inst[l].ptr+ xctx->sym)->type) continue;
     if( !strcmp((xctx->inst[l].ptr+ xctx->sym)->type, "package") )
      fprintf(fd, "%s\n", xctx->inst[l].prop_ptr);
    }
  
    dbg(1, "vhdl_block_netlist():       use statements\n");
    for(l=0;l<xctx->instances; ++l)
    {
     if(skip_instance(l, 1, lvs_ignore)) continue;
     if(!(xctx->inst[l].ptr+ xctx->sym)->type) continue;
     if( !strcmp((xctx->inst[l].ptr+ xctx->sym)->type, "use") )
      fprintf(fd, "%s\n", xctx->inst[l].prop_ptr);
    }
  
    dbg(1, "vhdl_block_netlist():       entity generics\n");
    /* print entity generics */
    print_generic(fd, "entity", i);
  
    dbg(1, "vhdl_block_netlist():       entity ports\n");
    /* print entity ports */
    tmp=0;
    int_hash_init(&table, 37);
    for(j=0;j<xctx->sym[i].rects[PINLAYER]; ++j)
    {
      if(strboolcmp(get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr,"vhdl_ignore",0), "true")) {
        my_strdup(_ALLOC_ID_, &sig_type,
           get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr,"sig_type",0));
        my_strdup(_ALLOC_ID_, &port_value,
           get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr,"value", 0) );
        if(!sig_type || sig_type[0]=='\0') my_strdup(_ALLOC_ID_, &sig_type,"std_logic");
        my_strdup(_ALLOC_ID_, &dir_tmp, get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr,"dir",0) );
        str_tmp = get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr,"name",0);
        if(!int_hash_lookup(&table, str_tmp, 1, XINSERT_NOREPLACE)) {
          if(tmp) fprintf(fd, " ;\n");
          if(!tmp)  fprintf(fd,"port (\n");
          fprintf(fd,"  %s : %s %s",str_tmp ? str_tmp : "<NULL>",
                                             dir_tmp ? dir_tmp : "<NULL>", sig_type);
          if(port_value &&port_value[0])
            fprintf(fd," := %s", port_value);
          tmp=1;
        }
        my_free(_ALLOC_ID_, &dir_tmp);
      }
    }
    int_hash_free(&table);
    if(tmp) fprintf(fd, "\n);\n");
  
    dbg(1, "vhdl_block_netlist():       port attributes\n");
    for(l=0;l<xctx->instances; ++l)
    {
     if(skip_instance(l, 1, lvs_ignore)) continue;
     my_strdup(_ALLOC_ID_, &type,(xctx->inst[l].ptr+ xctx->sym)->type);
     if( type && (strcmp(type,"port_attributes"))==0)
     {
      if(xctx->inst[l].prop_ptr) fprintf(fd, "%s\n", xctx->inst[l].prop_ptr);
     }
    }
    fprintf(fd,"end %s ;\n\n", sanitize(get_cell(xctx->sym[i].name, 0)) );
  
    dbg(1, "vhdl_block_netlist():       architecture\n");
    fprintf(fd,"architecture arch_%s of %s is\n\n",
       sanitize(get_cell(xctx->sym[i].name, 0)),
       sanitize(get_cell(xctx->sym[i].name, 0)) );
    /*    get_cell( xctx->sch[xctx->currsch], 0), get_cell( xctx->sch[xctx->currsch], 0)); */
    /* load current schematic to print used components */
  
    dbg(1, "vhdl_block_netlist():       used components\n");
    /* print all components */
    if(!vhdl_stop) {
      get_additional_symbols(1);
      for(j=0;j<xctx->symbols; ++j)
      {
        const char *default_schematic;
        if( strboolcmp(get_tok_value(xctx->sym[j].prop_ptr,"vhdl_primitive",0),"true")==0 ) continue;
        default_schematic = get_tok_value(xctx->sym[i].prop_ptr, "default_schematic", 0);
        if(!strcmp(default_schematic, "ignore")) continue;

        if(!xctx->sym[j].type || (strcmp(xctx->sym[j].type,"primitive")!=0 && 
           strcmp(xctx->sym[j].type,"subcircuit")!=0))
             continue;
        my_strdup2(_ALLOC_ID_, &abs_path, abs_sym_path(xctx->sym[i].name, ""));
        if(( strcmp(xctx->sym[j].type,"subcircuit")==0 || strcmp(xctx->sym[j].type,"primitive")==0) && 
            check_lib(1, abs_path)
          ) {
   
          /* only print component declaration if used in current subcircuit */
          found=0;
          for(l=0;l<xctx->instances; ++l)
          {
            if(skip_instance(l, 1, lvs_ignore)) continue;
            if(!xctx->x_strcmp(xctx->sym[j].name, tcl_hook2(xctx->inst[l].name)))
            {
              found=1; break;
            }
          }
          if(!found) continue;
          /* component generics */
          print_generic(fd, "component",j);
          /* component ports */
          tmp=0;
          int_hash_init(&table, 37);
          for(k=0;k<xctx->sym[j].rects[PINLAYER]; ++k)
          {
            if(strboolcmp(get_tok_value(xctx->sym[j].rect[PINLAYER][k].prop_ptr,"vhdl_ignore",0), "true")) {
              my_strdup(_ALLOC_ID_, &sig_type,get_tok_value(
                        xctx->sym[j].rect[PINLAYER][k].prop_ptr,"sig_type",0));
              my_strdup(_ALLOC_ID_, &port_value,
                 get_tok_value(xctx->sym[j].rect[PINLAYER][k].prop_ptr,"value", 0) );
   
              if(!sig_type || sig_type[0]=='\0') my_strdup(_ALLOC_ID_, &sig_type,"std_logic");
              my_strdup(_ALLOC_ID_, &dir_tmp, get_tok_value(xctx->sym[j].rect[PINLAYER][k].prop_ptr,"dir",0) );
              str_tmp = get_tok_value(xctx->sym[j].rect[PINLAYER][k].prop_ptr,"name",0);
              if(!int_hash_lookup(&table, str_tmp, 1, XINSERT_NOREPLACE)) {
                if(!tmp) fprintf(fd, "port (\n");
                if(tmp) fprintf(fd, " ;\n");
                fprintf(fd,"  %s : %s %s",str_tmp, dir_tmp ? dir_tmp : "<NULL>", sig_type);
                my_free(_ALLOC_ID_, &dir_tmp);
                if(port_value &&port_value[0]) fprintf(fd," := %s", port_value);
                tmp=1;
              }
            }
          }
          int_hash_free(&table);
          if(tmp) fprintf(fd, "\n);\n");
          fprintf(fd, "end component ;\n\n");
        }
      } /* for(j...) */
      get_additional_symbols(0);
    } /* if(!vhdl_stop) */
    my_free(_ALLOC_ID_, &abs_path);
    dbg(1, "vhdl_block_netlist():  netlisting %s\n", get_cell( xctx->sch[xctx->currsch], 0));
    err |= vhdl_netlist(fd, vhdl_stop);
    fprintf(fd,"//// begin user architecture code\n");
  
    for(l=0;l<xctx->instances; ++l) {
      if(skip_instance(l, 1, lvs_ignore)) continue;
      if(xctx->netlist_count &&
        !strboolcmp(get_tok_value(xctx->inst[l].prop_ptr, "only_toplevel", 0), "true")) continue;
  
      my_strdup(_ALLOC_ID_, &type,(xctx->inst[l].ptr+ xctx->sym)->type);
      if(type && !strcmp(type,"netlist_commands")) {
        fprintf(fd, "%s\n", get_tok_value(xctx->inst[l].prop_ptr,"value", 0));
      }
    }
  
    if(xctx->schvhdlprop && xctx->schvhdlprop[0]) fprintf(fd, "%s\n", xctx->schvhdlprop);
    fprintf(fd, "end arch_%s ;\n\n", sanitize(get_cell(xctx->sym[i].name, 0)) );
    my_free(_ALLOC_ID_, &sig_type);
    my_free(_ALLOC_ID_, &port_value);
    my_free(_ALLOC_ID_, &type);
  } /* if(!sym_def[0]) */
  if(split_f) {
    int save;
    fclose(fd);
    my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} noshow {%s}", netl_filename, cellname);
    save = xctx->netlist_type;
    xctx->netlist_type = CAD_VHDL_NETLIST;
    set_tcl_netlist_type();
    tcleval(tcl_cmd_netlist);
    xctx->netlist_type = save;
    set_tcl_netlist_type();
    if(debug_var==0) xunlink(netl_filename);
  }
  xctx->netlist_count++;
  return err;
}

