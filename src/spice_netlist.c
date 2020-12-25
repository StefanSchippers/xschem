/* File: spice_netlist.c
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

static struct hashentry *model_table[HASHSIZE];
static struct hashentry *model_entry;
static struct hashentry *subckt_table[HASHSIZE];

void global_spice_netlist(int global)  /* netlister driver */
{
 int first;
 FILE *fd;
 const char *str_tmp;
 int multip;
 unsigned int *stored_flags;
 int i, save_ok;
 char *type=NULL;
 char *place=NULL;
 char netl_filename[PATH_MAX]; /* overflow safe 20161122 */
 char tcl_cmd_netlist[PATH_MAX + 100]; /* 20081211 overflow safe 20161122 */
 char cellname[PATH_MAX]; /* 20081211 overflow safe 20161122 */
 char *subckt_name;

 if(xctx->modified) {
   save_ok = save_schematic(xctx->sch[xctx->currsch]);
   if(save_ok == -1) return;
 }
 free_hash(subckt_table);
 free_hash(model_table);
 statusmsg("",2);  /* clear infowindow */
 record_global_node(2, NULL, NULL); /* delete list of global nodes */
 top_subckt = 0;
 spiceprefix=1;
 bus_char[0] = bus_char[1] = '\0';
 hiersep[0]='.'; hiersep[1]='\0';
 str_tmp = tclgetvar("bus_replacement_char");
 if(str_tmp && str_tmp[0] && str_tmp[1]) {
   bus_char[0] = str_tmp[0];
   bus_char[1] = str_tmp[1];
 }
 netlist_count=0;
 my_snprintf(netl_filename, S(netl_filename), "%s/.%s_%d", 
   netlist_dir, skip_dir(xctx->sch[xctx->currsch]), getpid());
 dbg(1, "global_spice_netlist(): opening %s for writing\n",netl_filename);
 fd=fopen(netl_filename, "w");

 if(xctx->netlist_name[0]) {
   my_snprintf(cellname, S(cellname), "%s", get_cell(xctx->netlist_name, 0));
 } else {
   my_snprintf(cellname, S(cellname), "%s.spice", skip_dir(xctx->sch[xctx->currsch]));
 }

 if(fd==NULL) {
   dbg(0, "global_spice_netlist(): problems opening netlist file\n");
   return;
 }
 /* netlist_options */
 for(i=0;i<xctx->instances;i++) {
   if(!(xctx->inst[i].ptr+ xctx->sym)->type) continue;
   if( !strcmp((xctx->inst[i].ptr+ xctx->sym)->type,"netlist_options") ) {
     netlist_options(i);
   }
 }
 if(!strcmp(tclgetvar("top_subckt"), "1")) top_subckt = 1;
 if(!strcmp(tclgetvar("spiceprefix"), "0")) spiceprefix = 0;

 if(!top_subckt) fprintf(fd,"**");
 fprintf(fd,".subckt %s", skip_dir( xctx->sch[xctx->currsch]) );

 /* print top subckt ipin/opins */
 for(i=0;i<xctx->instances;i++) {
  if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue;
  if(xctx->inst[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "spice_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(380, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  dbg(1, "global_spice_netlist(): |%s|\n", type);
  if( type && !strcmp(type,"netlist_options") ) {
    continue;
  }
  if( type && IS_PIN(type)) {
   str_tmp = expandlabel ( (xctx->inst[i].lab ? xctx->inst[i].lab : ""), &multip);
   /*must handle  invalid node names */
   fprintf(fd, " %s", str_tmp ? str_tmp : "(NULL)" );
  }
 }
 fprintf(fd,"\n");

 spice_netlist(fd, 0);

 first = 0;
 for(i=0;i<xctx->instances;i++) /* print netlist_commands of top level cell with no 'place=end' property */
 {
  if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue;
  if(xctx->inst[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "spice_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(381, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
  my_strdup(382, &place,get_tok_value((xctx->inst[i].ptr+ xctx->sym)->prop_ptr,"place",0));
  if( type && !strcmp(type,"netlist_commands") ) {
   if(!place || strcmp(place, "end" )) {
     my_strdup(383, &place,get_tok_value(xctx->inst[i].prop_ptr,"place",0));
     if(!place || strcmp(place, "end" )) {
       if(first == 0) fprintf(fd,"**** begin user architecture code\n");
       first++;
       print_spice_element(fd, i) ;  /* this is the element line  */
     }
   }
  }
 }

 netlist_count++;

 if(xctx->schprop && xctx->schprop[0]) {
   if(first == 0) fprintf(fd,"**** begin user architecture code\n");
   first++;
   fprintf(fd, "%s\n", xctx->schprop);
 }
 if(first) fprintf(fd,"**** end user architecture code\n");
 /* /20100217 */

 if(!top_subckt) fprintf(fd,"**");
 fprintf(fd, ".ends\n");


 if(split_files) {
   fclose(fd);
   my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} noshow {%s}", netl_filename, cellname);
   override_netlist_type(CAD_SPICE_NETLIST);
   tcleval(tcl_cmd_netlist);
   override_netlist_type(-1); /* restore to netlist_type default */
   if(debug_var==0) xunlink(netl_filename);
 }

 /* preserve current level instance flags before descending hierarchy for netlisting, restore later */
 stored_flags = my_calloc(146, xctx->instances, sizeof(unsigned int));
 for(i=0;i<xctx->instances;i++) stored_flags[i] = xctx->inst[i].color;
 
 if(global)
 { 
   int saved_hilight_nets = xctx->hilight_nets;
   unselect_all();
   remove_symbols(); /* 20161205 ensure all unused symbols purged before descending hierarchy */
   load_schematic(1, xctx->sch[xctx->currsch], 0);

   my_strdup(469, &xctx->sch_path[xctx->currsch+1], xctx->sch_path[xctx->currsch]);
   my_strcat(481, &xctx->sch_path[xctx->currsch+1], "->netlisting");
   xctx->currsch++;
   
    dbg(1, "global_spice_netlist(): last defined symbol=%d\n",xctx->symbols);
   subckt_name=NULL;
   for(i=0;i<xctx->symbols;i++)
   {
    if( strcmp(get_tok_value(xctx->sym[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue;
    if(!xctx->sym[i].type) continue;
    if(strcmp(xctx->sym[i].type,"subcircuit")==0 && check_lib(xctx->sym[i].name))
    {
      /* xctx->sym can be SCH or SYM, use hash to avoid writing duplicate subckt */
      my_strdup(391, &subckt_name, get_cell(xctx->sym[i].name, 0));
      if (str_hash_lookup(subckt_table, subckt_name, "", XLOOKUP)==NULL)
      {
        str_hash_lookup(subckt_table, subckt_name, "", XINSERT);
        if( split_files && strcmp(get_tok_value(xctx->sym[i].prop_ptr,"vhdl_netlist",0),"true")==0 )
          vhdl_block_netlist(fd, i);
        else if(split_files && strcmp(get_tok_value(xctx->sym[i].prop_ptr,"verilog_netlist",0),"true")==0 )
          verilog_block_netlist(fd, i);
        else
          if( strcmp(get_tok_value(xctx->sym[i].prop_ptr,"spice_primitive",0),"true") )
            spice_block_netlist(fd, i);
      }
    }
   }
   free_hash(subckt_table);
   my_free(944, &subckt_name);
   /*clear_drawing(); */
   my_strncpy(xctx->sch[xctx->currsch] , "", S(xctx->sch[xctx->currsch]));
   xctx->currsch--;
   unselect_all();
   /* remove_symbols(); */
   load_schematic(1, xctx->sch[xctx->currsch], 0);
   prepare_netlist_structs(1); /* so 'lab=...' attributes for unnamed nets are set */
   /* symbol vs schematic pin check, we do it here since now we have ALL symbols loaded */
   sym_vs_sch_pins();
   if(!xctx->hilight_nets) xctx->hilight_nets = saved_hilight_nets;
 }
 /* restore hilight flags from errors found analyzing top level before descending hierarchy */
 for(i=0;i<xctx->instances; i++) xctx->inst[i].color = stored_flags[i];
 propagate_hilights(1, 0, XINSERT_NOREPLACE);
 draw_hilight_net(1);
 my_free(945, &stored_flags);

 /* print globals nodes found in netlist 28032003 */
 record_global_node(0,fd,NULL);

 /* =================================== 20121223 */
 first = 0;
 if(!split_files) {
   for(i=0;i<xctx->instances;i++) /* print netlist_commands of top level cell with 'place=end' property */
   {
    if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue;
    if(xctx->inst[i].ptr<0) continue;
    if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "spice_ignore",0 ), "true") ) {
      continue;
    }
    my_strdup(384, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
    my_strdup(385, &place,get_tok_value((xctx->inst[i].ptr+ xctx->sym)->prop_ptr,"place",0));
    if( type && !strcmp(type,"netlist_commands") ) {
     if(place && !strcmp(place, "end" )) {
       if(first == 0) fprintf(fd,"**** begin user architecture code\n");
       first++;
       print_spice_element(fd, i) ;
     } else {
       my_strdup(386, &place,get_tok_value(xctx->inst[i].prop_ptr,"place",0));
       if(place && !strcmp(place, "end" )) {
         if(first == 0) fprintf(fd,"**** begin user architecture code\n");
         first++;
         print_spice_element(fd, i) ;
       }
     }
    } /* netlist_commands */
   }

 }
 /* print device_model attributes */
 for(i=0;i<HASHSIZE; i++) {
   model_entry=model_table[i];
   while(model_entry) {
     if(first == 0) fprintf(fd,"**** begin user architecture code\n");
     first++;
     fprintf(fd, "%s\n",  model_entry->value);
     model_entry = model_entry->next;
   }
 }
 free_hash(model_table);
 if(first) fprintf(fd,"**** end user architecture code\n");


 /* 20150922 added split_files check */
 if(!split_files) fprintf(fd, ".end\n");

 dbg(1, "global_spice_netlist(): starting awk on netlist!\n");


 if(!split_files) {
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
 }
 my_free(946, &type);
 my_free(947, &place);
 netlist_count = 0;
}

static char *model_name_result = NULL;

static char *model_name(const char *m)
{
  char *m_lower = NULL;
  char *modelname = NULL;
  int n;
  int l = strlen(m) + 1;
  my_strdup(255, &m_lower, m);
  strtolower(m_lower);
  my_realloc(256, &modelname, l);
  my_realloc(257, &model_name_result, l);
  n = sscanf(m_lower, " %s %s", model_name_result, modelname);
  if(n<2) my_strncpy(model_name_result, m_lower, l);
  else {
    /* build a hash key value with no spaces to make device_model attributes with different spaces equivalent*/
    my_strcat(296, &model_name_result, modelname);
  }
  my_free(948, &modelname);
  my_free(949, &m_lower);
  return model_name_result;
}

void spice_block_netlist(FILE *fd, int i)
{
  int spice_stop=0;
  char netl_filename[PATH_MAX];
  char tcl_cmd_netlist[PATH_MAX + 100];
  char cellname[PATH_MAX];
  char filename[PATH_MAX];
  const char *str_tmp;
  /* int j; */
  /* int multip; */
  char *extra=NULL;

  if(!strcmp( get_tok_value(xctx->sym[i].prop_ptr,"spice_stop",0),"true") )
     spice_stop=1;
  else
     spice_stop=0;

  if(split_files) {
    my_snprintf(netl_filename, S(netl_filename), "%s/.%s_%d", netlist_dir, skip_dir(xctx->sym[i].name), getpid());
    dbg(1, "spice_block_netlist(): split_files: netl_filename=%s\n", netl_filename);
    fd=fopen(netl_filename, "w");
    my_snprintf(cellname, S(cellname), "%s.spice", skip_dir(xctx->sym[i].name));
  }

  fprintf(fd, "\n* expanding   symbol:  %s # of pins=%d\n\n",
        xctx->sym[i].name,xctx->sym[i].rects[PINLAYER] );

  fprintf(fd, ".subckt %s ",skip_dir(xctx->sym[i].name));
  print_spice_subckt(fd, i);

  my_strdup(387, &extra, get_tok_value(xctx->sym[i].prop_ptr,"extra",0) );
  /* this is now done in print_spice_subckt */
  /*
   * fprintf(fd, "%s ", extra ? extra : "" );
   */
 
  /* 20081206 new get_sym_template does not return token=value pairs where token listed in extra */
  fprintf(fd, "%s", get_sym_template(xctx->sym[i].templ, extra));
  my_free(950, &extra);
  fprintf(fd, "\n");

  if((str_tmp = get_tok_value(xctx->sym[i].prop_ptr, "schematic",0 ))[0]) {
    my_strncpy(filename, abs_sym_path(str_tmp, ""), S(filename));
    spice_stop? load_schematic(0,filename, 0) : 
                load_schematic(1,filename, 0);
  } else {
    spice_stop? load_schematic(0, add_ext(abs_sym_path(xctx->sym[i].name, ""), ".sch") ,0) : 
                load_schematic(1, add_ext(abs_sym_path(xctx->sym[i].name, ""), ".sch") ,0);
  }
  spice_netlist(fd, spice_stop);  /* 20111113 added spice_stop */
  netlist_count++;

  if(xctx->schprop && xctx->schprop[0]) {
    fprintf(fd,"**** begin user architecture code\n");
    fprintf(fd, "%s\n", xctx->schprop);
    fprintf(fd,"**** end user architecture code\n");
  }
  fprintf(fd, ".ends\n\n");
  if(split_files) {
    fclose(fd);
    my_snprintf(tcl_cmd_netlist, S(tcl_cmd_netlist), "netlist {%s} noshow {%s}", netl_filename, cellname);
    override_netlist_type(CAD_SPICE_NETLIST);
    tcleval(tcl_cmd_netlist);
    override_netlist_type(-1); /* restore to netlist_type default */
    if(debug_var==0) xunlink(netl_filename);
  }
}

void spice_netlist(FILE *fd, int spice_stop )
{
  int i;
  char *type=NULL;
 
  if(!spice_stop) {
    xctx->prep_net_structs = 0;
    prepare_netlist_structs(1);
    traverse_node_hash();  /* print all warnings about unconnected floatings etc */
    for(i=0;i<xctx->instances;i++) /* print first ipin/opin defs ... */
    {
     if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue;
     if(xctx->inst[i].ptr<0) continue;
     if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "spice_ignore",0 ), "true") ) {
       continue;
     }
     my_strdup(388, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
     if( type && IS_PIN(type) ) {
       print_spice_element(fd, i) ;  /* this is the element line  */
     }
    }
    for(i=0;i<xctx->instances;i++) /* ... then print other lines */
    {
     if( strcmp(get_tok_value(xctx->inst[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue;
     if(xctx->inst[i].ptr<0) continue;
     if(!strcmp(get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "spice_ignore",0 ), "true") ) {
       continue;
     }
     my_strdup(390, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
 
     if( type && !IS_LABEL_OR_PIN(type) ) {
       /* already done in global_spice_netlist */
       if(!strcmp(type,"netlist_commands") && netlist_count==0) continue;
       if(netlist_count &&
          !strcmp(get_tok_value(xctx->inst[i].prop_ptr, "only_toplevel", 0), "true")) continue;
       if(!strcmp(type,"netlist_commands")) {
         fprintf(fd,"**** begin user architecture code\n");
         print_spice_element(fd, i) ;  /* this is the element line  */
         fprintf(fd,"**** end user architecture code\n");
       } else {
         const char *m;
         print_spice_element(fd, i) ;  /* this is the element line  */
         fprintf(fd, "**** end_element\n");
         /* hash device_model attribute if any */
         m = get_tok_value(xctx->inst[i].prop_ptr, "device_model", 0);
         if(m[0]) str_hash_lookup(model_table, model_name(m), m, XINSERT);
         else {
           m = get_tok_value( (xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "device_model", 0);
           if(m[0]) str_hash_lookup(model_table, model_name(m), m, XINSERT);
         }
         my_free(951, &model_name_result);
       }
     }
    }
    my_free(952, &type);
  }
  if(!spice_stop && !netlist_count) redraw_hilights(); /* draw_hilight_net(1); */
}

/* calculate the hash function relative to string s */
static unsigned int str_hash(const char *tok)
{
  unsigned int hash = 0;
  int c;

  while ( (c = *tok++) )
      hash = c + (hash << 6) + (hash << 16) - hash;
  return hash;
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
struct hashentry *str_hash_lookup(struct hashentry **table, const char *token, const char *value, int what)
{
  unsigned int hashcode, idx;
  struct hashentry *entry, *saveptr, **preventry;
  int s ;

  if(token==NULL) return NULL;
  hashcode=str_hash(token);
  idx=hashcode % HASHSIZE;
  entry=table[idx];
  preventry=&table[idx];
  while(1)
  {
    if( !entry )          /* empty slot */
    {
      if(what==XINSERT || what == XINSERT_NOREPLACE)            /* insert data */
      {
        s=sizeof( struct hashentry );
        entry=(struct hashentry *)my_malloc(313, s);
        entry->next=NULL;
        entry->token=NULL;
        entry->value=NULL;
        my_strdup(297, &entry->token, token);
        my_strdup(307, &entry->value, value);
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
        my_free(953, &entry->token);
        my_free(954, &entry->value);
        my_free(955, &entry);
        *preventry=saveptr;
      }
      else if(value && what == XINSERT ) {
        my_strdup(308, &entry->value, value);
      }
      return entry;   /* found matching entry, return the address */
    }
    preventry=&entry->next; /* descend into the list. */
    entry = entry->next;
  }
}

static struct hashentry *str_free_hash_entry(struct hashentry *entry)
{
  struct hashentry *tmp;
  while( entry ) {
    tmp = entry -> next;
    my_free(956, &(entry->token));
    my_free(957, &(entry->value));
    my_free(958, &entry);
    entry = tmp;
  }
  return NULL;
}


void free_hash(struct hashentry **table)
{
  int i;

  for(i=0;i<HASHSIZE;i++)
  {
    table[i] = str_free_hash_entry( table[i] );
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
struct int_hashentry *int_hash_lookup(struct int_hashentry **table, const char *token, const int value, int what)
{
  unsigned int hashcode, idx;
  struct int_hashentry *entry, *saveptr, **preventry;
  int s ;

  if(token==NULL) return NULL;
  hashcode=str_hash(token);
  idx=hashcode % HASHSIZE;
  entry=table[idx];
  preventry=&table[idx];
  while(1)
  {
    if( !entry )          /* empty slot */
    {
      if(what==XINSERT || what == XINSERT_NOREPLACE)            /* insert data */
      {
        s=sizeof( struct int_hashentry );
        entry=(struct int_hashentry *)my_malloc(659, s);
        entry->next=NULL;
        entry->token=NULL;
        my_strdup(658, &entry->token, token);
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
        my_free(896, &entry->token);
        my_free(897, &entry);
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

static struct int_hashentry *free_int_hash_entry(struct int_hashentry *entry)
{
  struct int_hashentry *tmp;
  while( entry ) {
    tmp = entry -> next;
    my_free(1171, &(entry->token));
    my_free(1172, &entry);
    entry = tmp;
  }
  return NULL;
}


void free_int_hash(struct int_hashentry **table)
{
  int i;

  for(i=0;i<HASHSIZE;i++)
  {
    table[i] = free_int_hash_entry( table[i] );
  }
}

