/* File: verilog_netlist.c
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
static Str_hashentry *subckt_table[HASHSIZE]; /* safe even with multiple schematics */

static void verilog_netlist(FILE *fd , int verilog_stop)
{
 int i;
 char *type=NULL;

 if(!verilog_stop) {
   xctx->prep_net_structs = 0;
   prepare_netlist_structs(1);
   dbg(2, "verilog_netlist(): end prepare_netlist_structs\n");
   traverse_node_hash();  /* print all warnings about unconnected floatings etc */
   dbg(2, "verilog_netlist(): end traverse_node_hash\n");
 }

 fprintf(fd,"---- begin signal list\n"); /* these are needed even if signal list empty */
 if(!verilog_stop) print_verilog_signals(fd);
 fprintf(fd,"---- end signal list\n");   /* these are needed even if signal list empty */


 if(!verilog_stop)
 {
   for(i=0;i<xctx->instances;i++) /* ... print all element except ipin opin labels use package */
   {
    if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue;
    if(xctx->inst[i].ptr<0) continue;
    if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "verilog_ignore",0 ), "true") ) {
      continue;
    }

    dbg(2, "verilog_netlist():       into the netlisting loop\n");
    my_strdup(570, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
    if( type &&
       ( !IS_LABEL_OR_PIN(type) &&
         strcmp(type,"netlist_commands")&&
         strcmp(type,"timescale")&&
         strcmp(type,"verilog_preprocessor")
       ))
    {
     if(xctx->lastsel)
     {
      if(xctx->inst[i].sel==SELECTED) print_verilog_element(fd, i) ;
     }
     else print_verilog_element(fd, i) ;  /* this is the element line  */
    }
   }
   my_free(1084, &type);
 }
 dbg(1, "verilog_netlist():       end\n");
 if(!verilog_stop && !xctx->netlist_count) redraw_hilights(0); /*draw_hilight_net(1); */
}

void global_verilog_netlist(int global)  /* netlister driver */
{
 FILE *fd;
 const char *str_tmp;
 char *sig_type = NULL;
 char *port_value = NULL;
 char *tmp_string=NULL;
 unsigned int *stored_flags;
 int i, tmp;
 char netl_filename[PATH_MAX];  /* overflow safe 20161122 */
 char tcl_cmd_netlist[PATH_MAX + 100]; /* 20081203  overflow safe 20161122 */
 char cellname[PATH_MAX]; /* 20081203  overflow safe 20161122 */
 char *type=NULL;
 struct stat buf;
 char *subckt_name;
 char *abs_path = NULL;
 int split_f;
 const char *fmt_attr = NULL;

 xctx->hash_size = HASHSIZE;
 split_f = tclgetboolvar("split_files");
 xctx->push_undo();
 xctx->netlist_unconn_cnt=0; /* unique count of unconnected pins while netlisting */
 statusmsg("",2);  /* clear infowindow */
 str_hash_free(subckt_table);
 xctx->netlist_count=0;
 /* top sch properties used for library use declarations and type definitions */
 /* to be printed before any entity declarations */

 my_snprintf(netl_filename, S(netl_filename), "%s/.%s_%d", 
   tclgetvar("netlist_dir"), skip_dir(xctx->sch[xctx->currsch]),getpid());
 fd=fopen(netl_filename, "w");
 if(fd==NULL){
   dbg(0, "global_verilog_netlist(): problems opening netlist file\n");
   return;
 }
 fprintf(fd, "// sch_path: %s\n", xctx->sch[xctx->currsch]);

 if(xctx->netlist_name[0]) {
   my_snprintf(cellname, S(cellname), "%s", get_cell_w_ext(xctx->netlist_name, 0));
 } else {
   my_snprintf(cellname, S(cellname), "%s.v", skip_dir(xctx->sch[xctx->currsch]));
 }

 dbg(1, "global_verilog_netlist(): opening %s for writing\n",netl_filename);



/* print verilog timescale 10102004 */
 fmt_attr = xctx->format ? xctx->format : "verilog_format";
 for(i=0;i<xctx->instances;i++)
 {
  if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue;
  if(xctx->inst[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "verilog_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(105, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  if( type && (strcmp(type,"timescale")==0 || strcmp(type,"verilog_preprocessor")==0) )
  {
   str_tmp = get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr , fmt_attr, 2);
   my_strdup(106, &tmp_string, str_tmp);
   fprintf(fd, "%s\n", str_tmp ? translate(i, tmp_string) : "(NULL)");
  }
 }



 dbg(1, "global_verilog_netlist(): printing top level entity\n");
 fprintf(fd,"module %s (\n", skip_dir( xctx->sch[xctx->currsch]) );
 /* flush data structures (remove unused symbols) */
 unselect_all(1);
 remove_symbols();  /* removed 25122002, readded 04112003 */
 /* reload data without popping undo stack, this populates embedded symbols if any */
 xctx->pop_undo(2, 0);
 /* link_symbols_to_instances(-1); */ /* done in xctx->pop_undo() */
 dbg(1, "global_verilog_netlist(): sch[currsch]=%s\n", xctx->sch[xctx->currsch]);

 /* print top subckt port directions */
 dbg(1, "global_verilog_netlist(): printing top level out pins\n");
 tmp=0;
 for(i=0;i<xctx->instances;i++)
 {
  if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue;
  if(xctx->inst[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "verilog_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(546, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  if( type && (strcmp(type,"opin"))==0)
  {
   if(tmp) fprintf(fd, " ,\n");
   tmp++;
   str_tmp = xctx->inst[i].lab ? xctx->inst[i].lab : "";
   fprintf(fd, "  %s", str_tmp ? str_tmp : "(NULL)");
  }
 }

 dbg(1, "global_verilog_netlist(): printing top level inout pins\n");
 for(i=0;i<xctx->instances;i++)
 {
  if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue;
  if(xctx->inst[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "verilog_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(547, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  if( type && (strcmp(type,"iopin"))==0)
  {
   if(tmp) fprintf(fd, " ,\n");
   tmp++;
   str_tmp = xctx->inst[i].lab ? xctx->inst[i].lab : "";
   fprintf(fd, "  %s", str_tmp ? str_tmp : "(NULL)");
  }
 }

 dbg(1, "global_verilog_netlist(): printing top level input pins\n");
 for(i=0;i<xctx->instances;i++)
 {
  if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue;
  if(xctx->inst[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "verilog_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(548, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  if( type && (strcmp(type,"ipin"))==0)
  {
   if(tmp) fprintf(fd, " ,\n");
   tmp++;
   str_tmp = xctx->inst[i].lab ? xctx->inst[i].lab : "";
   fprintf(fd, "  %s", str_tmp ? str_tmp : "<NULL>");
  }
 }

 fprintf(fd,"\n);\n");

 /* 20071006 print top level params if defined in symbol */
 str_tmp = add_ext(xctx->sch[xctx->currsch], ".sym");
 if(!stat(str_tmp, &buf)) {
   load_sym_def(str_tmp, NULL );
   print_verilog_param(fd,xctx->symbols-1);  /* added print top level params */
   remove_symbol(xctx->symbols - 1);
 }
 /* 20071006 end */



 /* print top subckt port types */
 dbg(1, "global_verilog_netlist(): printing top level out pins\n");
 for(i=0;i<xctx->instances;i++)
 {
  if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue;
  if(xctx->inst[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "verilog_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(549, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  if( type && (strcmp(type,"opin"))==0)
  {
   my_strdup(550, &port_value,get_tok_value(xctx->inst[i].prop_ptr,"value",0));
   my_strdup(551, &sig_type,get_tok_value(xctx->inst[i].prop_ptr,"verilog_type",0));
   if(!sig_type || sig_type[0]=='\0') my_strdup(552, &sig_type,"wire"); /* 20070720 changed reg to wire */
   str_tmp = xctx->inst[i].lab ? xctx->inst[i].lab : "";
   fprintf(fd, "  output %s ;\n", str_tmp ? str_tmp : "(NULL)");
   fprintf(fd, "  %s %s ", sig_type, str_tmp ? str_tmp : "(NULL)");

   if(port_value && port_value[0]) fprintf(fd," = %s", port_value);
   fprintf(fd, ";\n");
  }
 }

 dbg(1, "global_verilog_netlist(): printing top level inout pins\n");
 for(i=0;i<xctx->instances;i++)
 {
  if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue;
  if(xctx->inst[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "verilog_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(553, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  if( type && (strcmp(type,"iopin"))==0)
  {
   my_strdup(554, &port_value,get_tok_value(xctx->inst[i].prop_ptr,"value",0));
   my_strdup(555, &sig_type,get_tok_value(xctx->inst[i].prop_ptr,"verilog_type",0));
   if(!sig_type || sig_type[0]=='\0') my_strdup(556, &sig_type,"wire");
   str_tmp = xctx->inst[i].lab ? xctx->inst[i].lab : "";
   fprintf(fd, "  inout %s ;\n", str_tmp ? str_tmp : "(NULL)");
   fprintf(fd, "  %s %s ", sig_type, str_tmp ? str_tmp : "(NULL)");

   if(port_value && port_value[0]) fprintf(fd," = %s", port_value);
   fprintf(fd, ";\n");
  }
 }

 dbg(1, "global_verilog_netlist(): printing top level input pins\n");
 for(i=0;i<xctx->instances;i++)
 {
  if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue;
  if(xctx->inst[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "verilog_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(557, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  if( type && (strcmp(type,"ipin"))==0)
  {
   my_strdup(558, &port_value,get_tok_value(xctx->inst[i].prop_ptr,"value",0));
   my_strdup(559, &sig_type,get_tok_value(xctx->inst[i].prop_ptr,"verilog_type",0));
   if(!sig_type || sig_type[0]=='\0') my_strdup(560, &sig_type,"wire");
   str_tmp = xctx->inst[i].lab ? xctx->inst[i].lab : "";
   fprintf(fd, "  input %s ;\n", str_tmp ? str_tmp : "<NULL>");
   fprintf(fd, "  %s %s ", sig_type, str_tmp ? str_tmp : "<NULL>");

   if(port_value && port_value[0]) fprintf(fd," = %s", port_value);
   fprintf(fd, ";\n");
  }
 }

 dbg(1, "global_verilog_netlist(): netlisting  top level\n");
 verilog_netlist(fd, 0);
 xctx->netlist_count++;
 fprintf(fd,"---- begin user architecture code\n");

 for(i=0;i<xctx->instances;i++) {
   if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue;
   if(xctx->inst[i].ptr<0) continue;
   if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "verilog_ignore",0 ), "true") ) {
     continue;
   }
   my_strdup(561, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
   if(type && !strcmp(type,"netlist_commands")) {
     fprintf(fd, "%s\n", get_tok_value(xctx->inst[i].prop_ptr,"value", 0));
   }
 }


 if(xctx->schverilogprop && xctx->schverilogprop[0]) {
   fprintf(fd, "%s\n", xctx->schverilogprop);
 }
 fprintf(fd,"---- end user architecture code\n");
 fprintf(fd, "endmodule\n");

 if(split_f) {
   int save;
   fclose(fd);
   my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} noshow {%s}", netl_filename, cellname);
   save = xctx->netlist_type;
   xctx->netlist_type = CAD_VERILOG_NETLIST;
   set_tcl_netlist_type();
   tcleval(tcl_cmd_netlist);
   xctx->netlist_type = save;
   set_tcl_netlist_type();
   if(debug_var==0) xunlink(netl_filename);
 }

 /* warning if two symbols perfectly overlapped */
 warning_overlapped_symbols(0);
 /* preserve current level instance flags before descending hierarchy for netlisting, restore later */
 stored_flags = my_calloc(150, xctx->instances, sizeof(unsigned int));
 for(i=0;i<xctx->instances;i++) stored_flags[i] = xctx->inst[i].color;

 if(global)
 {
   int saved_hilight_nets = xctx->hilight_nets;
   unselect_all(1);
   remove_symbols(); /* 20161205 ensure all unused symbols purged before descending hierarchy */
   /* reload data without popping undo stack, this populates embedded symbols if any */
   xctx->pop_undo(2, 0);
   /* link_symbols_to_instances(-1); */ /* done in xctx->pop_undo() */
   my_strdup(487, &xctx->sch_path[xctx->currsch+1], xctx->sch_path[xctx->currsch]);
   my_strcat(496, &xctx->sch_path[xctx->currsch+1], "->netlisting");
   xctx->sch_path_hash[xctx->currsch+1] = 0;
   xctx->currsch++;

   dbg(2, "global_verilog_netlist(): last defined symbol=%d\n",xctx->symbols);
   subckt_name=NULL;
   for(i=0;i<xctx->symbols;i++)
   {
    if( strcmp(get_tok_value(xctx->sym[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue;
    if(!xctx->sym[i].type) continue;
    my_strdup2(1234, &abs_path, abs_sym_path(xctx->sym[i].name, ""));
    if(strcmp(xctx->sym[i].type,"subcircuit")==0 && check_lib(1, abs_path)) {
      /* xctx->sym can be SCH or SYM, use hash to avoid writing duplicate subckt */
      my_strdup(328, &subckt_name, get_cell(xctx->sym[i].name, 0));
      if (str_hash_lookup(subckt_table, subckt_name, "", XLOOKUP)==NULL)
      {
        str_hash_lookup(subckt_table, subckt_name, "", XINSERT);
        if( split_f && strcmp(get_tok_value(xctx->sym[i].prop_ptr,"vhdl_netlist",0),"true")==0 )
          vhdl_block_netlist(fd, i);
        else if(split_f && strcmp(get_tok_value(xctx->sym[i].prop_ptr,"spice_netlist",0),"true")==0 )
          spice_block_netlist(fd, i);
        else
          if( strcmp(get_tok_value(xctx->sym[i].prop_ptr,"verilog_primitive",0), "true"))
            verilog_block_netlist(fd, i);
      }
    }
   }
   my_free(1235, &abs_path);
   str_hash_free(subckt_table);
   my_free(1073, &subckt_name);
   my_strncpy(xctx->sch[xctx->currsch] , "", S(xctx->sch[xctx->currsch]));
   xctx->currsch--;
   unselect_all(1);
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
 my_free(1074, &stored_flags);

 dbg(1, "global_verilog_netlist(): starting awk on netlist!\n");
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
   if(debug_var == 0 ) xunlink(netl_filename);
 }
 my_free(1075, &sig_type);
 my_free(1076, &port_value);
 my_free(1077, &tmp_string);
 my_free(1078, &type);
 xctx->netlist_count = 0;
}


void verilog_block_netlist(FILE *fd, int i)
{
  int j, l, tmp;
  int verilog_stop=0;
  char *dir_tmp = NULL;
  char *sig_type = NULL;
  char *port_value = NULL;
  char *type = NULL;
  char *tmp_string = NULL;
  char filename[PATH_MAX];
  char netl_filename[PATH_MAX];
  char tcl_cmd_netlist[PATH_MAX + 100];
  char cellname[PATH_MAX];
  const char *str_tmp, *fmt_attr = NULL;
  int split_f;
  const char *sym_def;
  char *extra_ptr, *saveptr1, *extra_token, *extra = NULL, *extra2=NULL;


  split_f = tclgetboolvar("split_files");
  if(!strcmp( get_tok_value(xctx->sym[i].prop_ptr,"verilog_stop",0),"true") )
     verilog_stop=1;
  else
     verilog_stop=0;
  get_sch_from_sym(filename, xctx->sym + i);
  if(split_f) {
    my_snprintf(netl_filename, S(netl_filename), "%s/.%s_%d",
       tclgetvar("netlist_dir"),  skip_dir(xctx->sym[i].name), getpid());
    dbg(1, "global_vhdl_netlist(): split_files: netl_filename=%s\n", netl_filename);
    fd=fopen(netl_filename, "w");
    my_snprintf(cellname, S(cellname), "%s.v", skip_dir(xctx->sym[i].name) );

  }
  dbg(1, "verilog_block_netlist(): expanding %s\n",  xctx->sym[i].name);
  fprintf(fd, "\n// expanding   symbol:  %s # of pins=%d\n",
        xctx->sym[i].name,xctx->sym[i].rects[PINLAYER] );
  fprintf(fd, "// sym_path: %s\n", abs_sym_path(xctx->sym[i].name, ""));
  sym_def = get_tok_value(xctx->sym[i].prop_ptr,"verilog_sym_def",0);
  if(sym_def[0]) {
    fprintf(fd, "%s\n", sym_def);
  } else {
    my_strdup(1040, &extra, get_tok_value(xctx->sym[i].prop_ptr, "verilog_extra", 0));
    my_strdup(1563, &extra2, get_tok_value(xctx->sym[i].prop_ptr, "verilog_extra", 0));
    fprintf(fd, "// sch_path: %s\n", filename);
    verilog_stop? load_schematic(0,filename, 0) : load_schematic(1,filename, 0);
    /* print verilog timescale  and preprocessor directives 10102004 */
    fmt_attr = xctx->format ? xctx->format : "verilog_format";
    for(j=0;j<xctx->instances;j++)
    {
     if( strcmp(get_tok_value(xctx->inst[j].prop_ptr,"verilog_ignore",0),"true")==0 ) continue;
     if(xctx->inst[j].ptr<0) continue;
     if(!strcmp(get_tok_value( (xctx->inst[j].ptr+ xctx->sym)->prop_ptr, "verilog_ignore",0 ), "true") ) {
       continue;
     }
     my_strdup(544, &type,(xctx->inst[j].ptr+ xctx->sym)->type);
     if( type && ( strcmp(type,"timescale")==0  || strcmp(type,"verilog_preprocessor")==0) )
     {
      str_tmp = get_tok_value( (xctx->inst[j].ptr+ xctx->sym)->prop_ptr, fmt_attr, 2);
      my_strdup(545, &tmp_string, str_tmp);
      fprintf(fd, "%s\n", str_tmp ? translate(j, tmp_string) : "(NULL)");
     }
    }
  
    fprintf(fd, "module %s (\n", skip_dir(xctx->sym[i].name));
    /*print_generic(fd, "entity", i); */
  
    dbg(1, "verilog_block_netlist():       entity ports\n");
  
    /* print port list */
    tmp=0;
    for(j=0;j<xctx->sym[i].rects[PINLAYER];j++)
    {
      if(strcmp(get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr,"verilog_ignore",0), "true")) {
        str_tmp = get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr,"name",0);
        if(tmp) fprintf(fd, " ,\n");
        tmp++;
        fprintf(fd,"  %s", str_tmp ? str_tmp : "<NULL>");
      }
    }
  
    if(extra) {
      for(extra_ptr = extra; ; extra_ptr=NULL) {
        extra_token=my_strtok_r(extra_ptr, " ", "", &saveptr1);
        if(!extra_token) break;
        if(tmp) fprintf(fd, " ,\n");
        fprintf(fd, "  %s", extra_token);
        tmp++;
      }
    }
    fprintf(fd, "\n);\n");

    dbg(1, "verilog_block_netlist():       entity generics\n");
    /* print module  default parameters */
    print_verilog_param(fd,i);
    /* print port types */
    for(j=0;j<xctx->sym[i].rects[PINLAYER];j++)
    {
      if(strcmp(get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr,"verilog_ignore",0), "true")) {
        my_strdup(564, &sig_type,get_tok_value(
                  xctx->sym[i].rect[PINLAYER][j].prop_ptr,"verilog_type",0));
        my_strdup(565, &port_value, 
          get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr,"value", 0) );
        my_strdup(566, &dir_tmp, get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr,"dir",0) );
        if(strcmp(dir_tmp,"in")){
           if(!sig_type || sig_type[0]=='\0') my_strdup(567, &sig_type,"wire"); /* 20070720 changed reg to wire */
        } else {
           if(!sig_type || sig_type[0]=='\0') my_strdup(568, &sig_type,"wire");
        }
        str_tmp = get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr,"name",0);
        fprintf(fd,"  %s %s ;\n",
          strcmp(dir_tmp,"in")? ( strcmp(dir_tmp,"out")? "inout" :"output"  ) : "input",
          str_tmp ? str_tmp : "<NULL>");
        fprintf(fd,"  %s %s",
          sig_type,
          str_tmp ? str_tmp : "<NULL>");
        if(port_value &&port_value[0])
          fprintf(fd," = %s", port_value);
        fprintf(fd," ;\n");
      }
    }
    if(extra2) {
      saveptr1 = NULL;
      for(extra_ptr = extra2; ; extra_ptr=NULL) {
        extra_token=my_strtok_r(extra_ptr, " ", "", &saveptr1);
        if(!extra_token) break;
        fprintf(fd, "  inout %s ;\n", extra_token);
        fprintf(fd, "  wire %s ;\n", extra_token);
      }
    }
    dbg(1, "verilog_block_netlist():       netlisting %s\n", skip_dir( xctx->sch[xctx->currsch]));
    verilog_netlist(fd, verilog_stop);
    fprintf(fd,"---- begin user architecture code\n");
    for(l=0;l<xctx->instances;l++) {
      if( strcmp(get_tok_value(xctx->inst[l].prop_ptr,"verilog_ignore",0),"true")==0 ) continue;
      if(xctx->inst[l].ptr<0) continue;
      if(!strcmp(get_tok_value( (xctx->inst[l].ptr+ xctx->sym)->prop_ptr, "verilog_ignore",0 ), "true") ) {
        continue;
      }
      if(xctx->netlist_count &&
        !strcmp(get_tok_value(xctx->inst[l].prop_ptr, "only_toplevel", 0), "true")) continue;
  
      my_strdup(569, &type,(xctx->inst[l].ptr+ xctx->sym)->type);
      if(type && !strcmp(type,"netlist_commands")) {
        fprintf(fd, "%s\n", get_tok_value(xctx->inst[l].prop_ptr,"value", 0));
      }
    }
  
    if(xctx->schverilogprop && xctx->schverilogprop[0]) {
      fprintf(fd, "%s\n", xctx->schverilogprop);
    }
    fprintf(fd,"---- end user architecture code\n");
    fprintf(fd, "endmodule\n");
    my_free(1079, &dir_tmp);
    my_free(1080, &sig_type);
    my_free(1081, &port_value);
    my_free(1082, &type);
    my_free(1083, &tmp_string);
    my_free(1561, &extra);
    my_free(1564, &extra2);
  } /* if(!sym_def[0]) */
  if(split_f) {
    int save;
    fclose(fd);
    my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} noshow {%s}", netl_filename, cellname);
    save = xctx->netlist_type;
    xctx->netlist_type = CAD_VERILOG_NETLIST;
    set_tcl_netlist_type();
    tcleval(tcl_cmd_netlist);
    xctx->netlist_type = save;
    set_tcl_netlist_type();
    if(debug_var==0) xunlink(netl_filename);
  }
  xctx->netlist_count++;
}

