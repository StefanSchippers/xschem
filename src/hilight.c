/* File: hilight.c
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

static unsigned int hi_hash(const char *tok)
{
  register unsigned int hash = 5381;
  register char *str;
  register int c;

  if(xctx->sch_path_hash[xctx->currsch] == 0) {
    str=xctx->sch_path[xctx->currsch];
    while ( (c = (unsigned char)*str++) ) {
      hash += (hash << 5) + c;
    }
    xctx->sch_path_hash[xctx->currsch] = hash;
  } else { 
    hash = xctx->sch_path_hash[xctx->currsch];
  }
  while ( (c = (unsigned char)*tok++) ) {
    hash += (hash << 5) + c;
  }
  return hash;
}

static void hilight_hash_free_entry(Hilight_hashentry *entry)
{
  Hilight_hashentry *tmp;
  while(entry) {
    tmp = entry->next;
    my_free(_ALLOC_ID_, &entry->token); 
    my_free(_ALLOC_ID_, &entry->path); 
    my_free(_ALLOC_ID_, &entry);
    entry = tmp;
  }
}

static void hilight_hash_free(void) /* remove the whole hash table  */
{
 int i;

 dbg(2, "hilight_hash_free(): removing hash table\n");
 for(i=0;i<HASHSIZE; ++i)
 {
  hilight_hash_free_entry( xctx->hilight_table[i] );
  xctx->hilight_table[i] = NULL;
 }
}

static Hilight_hashentry *hilight_hash_lookup(const char *token, int value, int what)
/*    token           what       ... what ...
 * --------------------------------------------------------------------------
 * "whatever"         XINSERT    insert in hash table if not in and return NULL. If already present update
 *                               value if not NULL, retun new value, return NULL otherwise
 * "whatever"  XINSERT_NOREPLACE same as XINSERT but do not replace existing value, return NULL if not found.
 * "whatever"         XDELETE    delete entry if found, return NULL
 * "whatever"         XLOOKUP    only look up element, dont insert, return NULL if not found.
 */
{
  unsigned int hashcode, index;
  Hilight_hashentry **preventry;
 
  if(token==NULL) return NULL;
  hashcode=hi_hash(token);
  index=hashcode % HASHSIZE;
  preventry=&xctx->hilight_table[index];
  while(1) {
    if( !(*preventry) ) { /* empty slot */
      size_t lent = strlen(token) + 1;
      size_t lenp = strlen(xctx->sch_path[xctx->currsch]) + 1;
      if( what==XINSERT || what == XINSERT_NOREPLACE) { /* insert data */
        Hilight_hashentry *entry = (Hilight_hashentry *)my_malloc(_ALLOC_ID_, sizeof( Hilight_hashentry ));
        entry->next = NULL;
        entry->token = my_malloc(_ALLOC_ID_, lent);
        memcpy(entry->token, token, lent);
        entry->path = my_malloc(_ALLOC_ID_, lenp);
        memcpy(entry->path, xctx->sch_path[xctx->currsch], lenp);
        entry->oldvalue = value-1000; /* no old value, set different value anyway*/
        entry->value = value;
        entry->time = xctx->hilight_time;
        entry->hash = hashcode;
        *preventry = entry;
        xctx->hilight_nets = 1; /* some nets should be hilighted ....  07122002 */
      }
      return NULL; /* whether inserted or not return NULL since it was not in */
    }
    if( (*preventry) -> hash==hashcode && !strcmp(token,(*preventry)->token) &&
         !strcmp(xctx->sch_path[xctx->currsch], (*preventry)->path)  ) { /* found matching tok */
      if(what==XDELETE) {              /* remove token from the hash table ... */
        Hilight_hashentry *saveptr;
        saveptr=(*preventry)->next;
        my_free(_ALLOC_ID_, &(*preventry)->token);
        my_free(_ALLOC_ID_, &(*preventry)->path);
        my_free(_ALLOC_ID_, &(*preventry));
        *preventry=saveptr;
      } else if(what == XINSERT ) {
        (*preventry)->oldvalue =(*preventry)->value;
        (*preventry)->value = value;
        (*preventry)->time=xctx->hilight_time;
      }
      return (*preventry); /* found matching entry, return the address */
    }
    preventry=&(*preventry)->next; /* descend into the list. */
  }
}

/* wrapper function to hash highlighted instances, avoid clash with net names */
Hilight_hashentry *inst_hilight_hash_lookup(int i,  int value, int what)
{
  const char *token = xctx->inst[i].instname;
  char *inst_tok = NULL;
  size_t len = strlen(token) + 3; /* token plus two more character and \0 */
  int label = 0;
  Hilight_hashentry *entry;
  if(IS_LABEL_SH_OR_PIN( (xctx->inst[i].ptr+xctx->sym)->type )) label = 1;
  dbg(1, "inst_hilight_hash_lookup: token=%s value=%d what=%d\n", token, value, what);
  inst_tok = my_malloc(_ALLOC_ID_, len);
  /* instance name uglyfication: add a space at beginning so it will never match a valid net name */
  /* use 2 spaces for pins/labels to distinguish from other instances */
  if(label) my_snprintf(inst_tok, len, "  %s", token);
  else my_snprintf(inst_tok, len, " %s", token);
  entry = hilight_hash_lookup(inst_tok, value, what);
  my_free(_ALLOC_ID_, &inst_tok);
  return entry;
}

/* warning, in case of buses return only pointer to first found bus element */
Hilight_hashentry *bus_hilight_hash_lookup(const char *token, int value, int what)
{
  char *start, *string_ptr, c;
  char *string=NULL;
  Hilight_hashentry *ptr1=NULL, *ptr2=NULL;
  int mult;
 
  dbg(1, "bus_hilight_hash_lookup(): token=%s value=%d what=%d\n",
       token ? token : "<NULL>", value, what);
  xctx->some_nets_added = 0;
  if(token==NULL) return NULL;
  /* if( token[0] == '#' || !strpbrk(token, "*[],.:")) { */
  if( token[0] == '#' || !strpbrk(token, "*,.:")) {
    dbg(2, "bus_hilight_hash_lookup(): inserting: %s, value:%d\n", token, value);
    ptr1=hilight_hash_lookup(token, value, what);
    if(!ptr1) xctx->some_nets_added = 1;
    return ptr1;
  }
  my_strdup(_ALLOC_ID_, &string, expandlabel(token,&mult));
  if(string==NULL) {
    return NULL;
  }
  string_ptr = start = string;
  while(1) {
    c=(*string_ptr);
    if(c==','|| c=='\0') {
      *string_ptr='\0';  /* set end string at comma position.... */
      /* insert one bus element at a time in hash table */
      dbg(2, "bus_hilight_hash_lookup(): inserting: %s, value:%d\n", start,value);
      ptr1=hilight_hash_lookup(start, value, what);
      if(!ptr1) xctx->some_nets_added = 1;
      if(ptr1 && !ptr2) {
        ptr2=ptr1; /*return first non null entry */
        if(what==XLOOKUP) break; /* 20161221 no need to go any further if only looking up element */
      }
      *string_ptr=c;     /* ....restore original char */
      start=string_ptr+1;
    }
    if(c==0) break;
    string_ptr++;
  }
  /* if something found return first pointer */
  my_free(_ALLOC_ID_, &string);
  return ptr2;
}

/* highlight an arbitrary hierarchic net */
Hilight_hashentry *hier_hilight_hash_lookup(const char *token, int value, const char *path, int what)
{
  Hilight_hashentry *entry;
  char *oldpath = xctx->sch_path[xctx->currsch];
  xctx->sch_path_hash[xctx->currsch] = 0;
  xctx->sch_path[xctx->currsch] = NULL;
  my_strdup2(_ALLOC_ID_, &xctx->sch_path[xctx->currsch], path);
  entry = bus_hilight_hash_lookup(token, value, what);
  my_free(_ALLOC_ID_, &xctx->sch_path[xctx->currsch]);
  xctx->sch_path[xctx->currsch] = oldpath;
  xctx->sch_path_hash[xctx->currsch] = 0;
  return entry;
}

/* what:
 *  1: list only nets
 *  2: list only intances
 *  3: list all
 */
void display_hilights(int what, char **str)
{
  int i;
  int first = 1;
  int instance = 0;
  const char *ptr;
  Hilight_hashentry *entry;
  for(i=0;i<HASHSIZE; ++i) {
    entry = xctx->hilight_table[i];
    while(entry) {
      ptr = entry->token;
      if(ptr[0] == ' ' && ptr[1] == ' ' ) goto skip; /* do not list net labels / pins / net_show */
      if(ptr[0] == ' ') instance = 1;
      else instance = 0;
      dbg(1, "what=%d, instance=%d, token=%s\n", what, instance, ptr);
      if( ((what & 1) && !instance)  || ((what & 2) && instance) ) {
        if(instance) ptr++; /* skip uglyfication space */
        if(!first) my_strcat(_ALLOC_ID_, str, " ");
        my_strcat(_ALLOC_ID_, str,"{");
        my_strcat(_ALLOC_ID_, str, entry->path+1);
        my_strcat(_ALLOC_ID_, str, ptr);
        my_strcat(_ALLOC_ID_, str,"}");
        first = 0;
      }
      skip: 
      entry = entry->next;
    }
  }
}

static int there_are_hilights()
{
  register int i;
  register xInstance * inst =  xctx->inst;
  register Hilight_hashentry **hiptr = xctx->hilight_table;
  for(i=0;i<HASHSIZE; ++i) {
    if(hiptr[i]) return 1;
  }
  for(i = 0; i < xctx->instances; ++i) {
    if(inst[i].color != -10000) return 1; 
  }
  return 0;
}

int hilight_graph_node(const char *node, int col)
{
  int current = 0;
  const char *path;
  const char *path3;
  const char *path_skip;
  int skip = 0;
  char *path2 = NULL;
  char *n = NULL;
  char *nptr, *ptr, *ptr2;
  Hilight_hashentry  *entry;
  int start_level = sch_waves_loaded();

  path_skip = path = xctx->sch_path[xctx->currsch];
  path_skip++; /* skip initial '.' */
  /* skip path components that are above the level where raw file was loaded */
  while(*path_skip && skip < start_level) {
    if(*path_skip == '.') skip++; 
    ++path_skip;
  } 

  my_strdup2(_ALLOC_ID_, &n, node);
  nptr = n;
  
  dbg(1, "hilight_graph_node(): path_skip=%s, %s: %d\n", path_skip, node, col);
  if(strstr(n, "i(v.")) {current = 1; nptr += 4;}
  else if(strstr(n, "I(V.")) {current = 1; nptr += 4;}
  else if(strstr(n, "i(")) {current = 1; nptr += 2;}
  else if(strstr(n, "I(")) {current = 1; nptr += 2;}
  else if(strstr(n, "v(")) {nptr += 2;}
  else if(strstr(n, "V(")) {nptr += 2;}
  if((ptr = strrchr(n, ')'))) *ptr = '\0';

  if((ptr2 = strrchr(nptr, '.'))) {
    *ptr2 = '\0';
    path3 = nptr;
    nptr = ptr2 + 1;
    if(!strstr(path_skip, path3))
      my_mstrcat(_ALLOC_ID_, &path2, path, path3, ".", NULL);
    else 
      my_strdup2(_ALLOC_ID_, &path2, path);
  }
  else {
    my_strdup2(_ALLOC_ID_, &path2, path);
  }
  if(current) {
    nptr--;
    *nptr = ' ';
  }
  entry = hier_hilight_hash_lookup(nptr, -col, path2, XLOOKUP);
  if(!entry || entry->value != -col ) {
    hier_hilight_hash_lookup(nptr, -col, path2, XINSERT);
    dbg(1, "hilight_graph_node(): propagate_hilights(), col=%d\n", col);
    propagate_hilights(1, 0, XINSERT_NOREPLACE);
  }
  my_free(_ALLOC_ID_, &n);
  my_free(_ALLOC_ID_, &path2);
  return 1;
}

/* by default: 
 * xctx->active_layer[0] = 7
 * xctx->active_layer[1] = 8
 * xctx->active_layer[2] = 10  if 9 is disabled it is skipped
 * ...
 * if a layer is disabled (not viewable) it is skipped
 * xctx->n_active_layers is the total number of layers for hilights.
 * standard xschem conf: cadlayers=22, xctx->n_active_layers=15 if no disabled layers.
 */
int get_color(int value)
{
  int x;

  if(value < 0) return (-value) % cadlayers ;
  if(xctx->n_active_layers) {
    x = value%(xctx->n_active_layers);
    return xctx->active_layer[x];
  } else {
    return cadlayers > 5 ? 5 : cadlayers -1; /* desperate attempt to return a decent color */
  }
}

void incr_hilight_color(void)
{
  xctx->hilight_color = (xctx->hilight_color + 1) % (xctx->n_active_layers * cadlayers);
  dbg(1, "incr_hilight_color(): xctx->hilight_color=%d\n", xctx->hilight_color);
}

static void set_rawfile_for_bespice() 
{
  char raw_file[PATH_MAX];
  char netlist_file[PATH_MAX];
  tcleval("file tail [file rootname [xschem get schname 0]].raw");
  my_strncpy(raw_file, tclresult(), S(raw_file));
  tcleval("file tail [file rootname [xschem get schname 0]].spice");
  my_strncpy(netlist_file, tclresult(), S(netlist_file));

  /* (1) make sure that the raw file has been opened */
  tclvareval("puts $bespice_server_getdata(sock) ",
              "{open_file \"", raw_file, "\"}",
              NULL);
  /* (2) create curves for electrically equivalent nodes (pins of spice subcircuits) */
  tclvareval("puts $bespice_server_getdata(sock) ",
              "{create_equivalent_nets \"", raw_file, "\" \"", netlist_file, "\"}",
              NULL);
  /* (3) make sure that the raw file is used for commands plotting voltages and currents 
            this is important if more than one file has been opened. */
  tclvareval("puts $bespice_server_getdata(sock) ",
              "{use_file_for_link_to_schematic \"", raw_file, "\"}",
              NULL);
}

/* print all highlight signals which are not ports (in/out/inout). */
void create_plot_cmd(void)
{
  int i, c, idx, first;
  Hilight_hashentry *entry;
  Node_hashentry *node_entry;
  char *tok;
  char plotfile[PATH_MAX];
  char color_str[30];
  FILE *fd = NULL;
  char *str = NULL;
  int viewer = 0;
  int exists = 0;
  char *viewer_name = NULL;
  char tcl_str[200];
  char rawfile[PATH_MAX];
  int simtype;

  tcleval("sim_is_xyce");
  simtype = atoi( tclresult() );
  tcleval("file tail [file rootname [xschem get schname 0]].raw");
  my_strncpy(rawfile, tclresult(), S(rawfile));
  tcleval("info exists sim");
  if(tclresult()[0] == '1') exists = 1;
  xctx->enable_drill = 0;
  if(exists) {
    viewer = tclgetintvar("sim(spicewave,default)");
    my_snprintf(tcl_str, S(tcl_str), "sim(spicewave,%d,name)", viewer);
    my_strdup(_ALLOC_ID_, &viewer_name, tclgetvar(tcl_str));
    dbg(1,"create_plot_cmd(): viewer_name=%s\n", viewer_name);
    if(strstr(viewer_name, "Gaw")) viewer=GAW;
    else if(strstr(viewer_name, "Bespice")) viewer=BESPICE;
    else if(strstr(viewer_name, "Ngspice")) viewer=NGSPICE;
    my_free(_ALLOC_ID_, &viewer_name);
  }
  if(!exists || !viewer) return;
  my_snprintf(plotfile, S(plotfile), "%s/xplot", tclgetvar("netlist_dir"));
  if(viewer == NGSPICE) {
    if(!(fd = fopen(plotfile, "w"))) {
      fprintf(errfp, "create_plot_cmd(): error opening xplot file for writing\n");
      return;
    }
    fprintf(fd, "*ngspice plot file\n.control\n");
  }
  if(viewer == GAW) tcleval("setup_tcp_gaw");
  if(tclresult()[0] == '0')  return;
  if(viewer == BESPICE) set_rawfile_for_bespice();
  idx = 1;
  first = 1;
  for(i=0;i<HASHSIZE; ++i) /* set ngspice colors */
  {
    entry = xctx->hilight_table[i];
    while(entry) {
      tok = entry->token;
      node_entry = bus_node_hash_lookup(tok, "", XLOOKUP, 0, "", "", "", "");
      if(tok[0] == '#') tok++;
      if(node_entry && !strcmp(xctx->sch_path[xctx->currsch], entry->path) &&
         (node_entry->d.port == 0 || !strcmp(entry->path, ".") )) {
        c = get_color(entry->value);
        ++idx;
        if(viewer == NGSPICE) {
          sprintf(color_str, "%02x/%02x/%02x", 
            xctx->xcolor_array[c].red>>8, xctx->xcolor_array[c].green>>8, xctx->xcolor_array[c].blue>>8);
          if(idx > 9) {
            idx = 2;
            fprintf(fd, "%s", str);
            fprintf(fd, "\n");
            first = 1;
            my_free(_ALLOC_ID_, &str);
          }
          fprintf(fd, "set color%d=rgb:%s\n", idx, color_str);
          if(first) {
            my_strcat(_ALLOC_ID_, &str, "plot ");
            first = 0;
          }
          my_strcat(_ALLOC_ID_, &str, "\"");
          my_strcat(_ALLOC_ID_, &str, (entry->path)+1);
          my_strcat(_ALLOC_ID_, &str, tok);
          my_strcat(_ALLOC_ID_, &str, "\" ");
        }
        if(viewer == GAW) {
          char *t=NULL, *p=NULL;
          sprintf(color_str, "%02x%02x%02x", 
            xctx->xcolor_array[c].red>>8, xctx->xcolor_array[c].green>>8, xctx->xcolor_array[c].blue>>8);
          my_strdup(_ALLOC_ID_, &t, tok);
          my_strdup2(_ALLOC_ID_, &p, (entry->path)+1);
          if(simtype == 0 ) { /* spice */
            tclvareval("puts $gaw_fd {copyvar v(", strtolower(p), strtolower(t),
                        ") sel #", color_str, "}\nvwait gaw_fd\n", NULL);
          } else { /* Xyce */
            char *c=p;
            while(*c){
              if(*c == '.') *c = ':'; /* Xyce uses : as path separator */
              ++c;
            }
            tclvareval("puts $gaw_fd {copyvar ", strtoupper(p), strtoupper(t),
                        " sel #", color_str, "}\nvwait gaw_fd\n", NULL);
          }
          my_free(_ALLOC_ID_, &p);
          my_free(_ALLOC_ID_, &t);
        }
        if(viewer == BESPICE) {
          char *t=NULL, *p=NULL;
          sprintf(color_str, "#%02x%02x%02x", 
            xctx->xcolor_array[c].red>>8, xctx->xcolor_array[c].green>>8, xctx->xcolor_array[c].blue>>8);
          my_strdup(_ALLOC_ID_, &t, tok);
          my_strdup2(_ALLOC_ID_, &p, (entry->path)+1);

          /* bespice command syntax : 
                add_voltage_on_spice_node_to_plot <plot name> <section name> <hierarchical spice node name> <flag clear> [<color>]
                    plot name is "*" => automatic
                    section name is empty => automatic or user defined
          */
          tclvareval(
              "puts $bespice_server_getdata(sock) ",
              "{add_voltage_on_spice_node_to_plot * \"\" \"",
              p, t, "\" 0 ", color_str, "}",
              NULL);
          my_free(_ALLOC_ID_, &p);
          my_free(_ALLOC_ID_, &t);
        }
      }
      entry = entry->next;
    }
  }
  if(viewer == NGSPICE) {
    fprintf(fd, "%s", str);
    fprintf(fd, "\nremcirc\n.endc\n");
    my_free(_ALLOC_ID_, &str);
    fclose(fd);
  }
  if(viewer == GAW) {
     tcleval("vwait gaw_fd; close $gaw_fd; unset gaw_fd\n");
  }
}

void clear_all_hilights(void)
{
  int i;
  xctx->hilight_color=0;
  if(!xctx->hilight_nets) return;
  hilight_hash_free();
  xctx->hilight_nets=0;
  for(i=0;i<xctx->instances; ++i) {
    xctx->inst[i].color = -10000 ;
  }
  dbg(1, "clear_all_hilights(): clearing\n");
}

void hilight_net_pin_mismatches(void)
{
  int i,j,k;
  xSymbol *symbol;
  int npin;
  char *type=NULL;
  char *labname=NULL;
  char *lab=NULL;
  char *netname=NULL;
  int mult;
  xRect *rct;
  int incr_hi;
  
  incr_hi = tclgetboolvar("incr_hilight");
  rebuild_selected_array();
  prepare_netlist_structs(0);
  for(k=0; k<xctx->lastsel; ++k) {
    if(xctx->sel_array[k].type!=ELEMENT) continue;
    j = xctx->sel_array[k].n ;
    my_strdup(_ALLOC_ID_, &type,(xctx->inst[j].ptr+ xctx->sym)->type);
    if( type && IS_LABEL_SH_OR_PIN(type)) break;
    symbol = xctx->sym + xctx->inst[j].ptr;
    npin = symbol->rects[PINLAYER];
    rct=symbol->rect[PINLAYER];
    dbg(1, "hilight_net_pin_mismatches(): \n");
    for(i=0;i<npin; ++i) {
      my_strdup(_ALLOC_ID_, &labname,get_tok_value(rct[i].prop_ptr,"name",0));
      my_strdup(_ALLOC_ID_, &lab, expandlabel(labname, &mult));
      my_strdup(_ALLOC_ID_, &netname, net_name(j,i, &mult, 0, 0));
      dbg(1, "hilight_net_pin_mismatches(): i=%d labname=%s explabname = %s  net = %s\n", i, labname, lab, netname);
      if(netname && strcmp(lab, netname)) {
        dbg(1, "hilight_net_pin_mismatches(): hilight: %s\n", netname);
        bus_hilight_hash_lookup(netname, xctx->hilight_color, XINSERT_NOREPLACE);
        if(incr_hi) incr_hilight_color();
      }
    }
  }
  my_free(_ALLOC_ID_, &type);
  my_free(_ALLOC_ID_, &labname);
  my_free(_ALLOC_ID_, &lab);
  my_free(_ALLOC_ID_, &netname);
  if(xctx->hilight_nets) propagate_hilights(1, 0, XINSERT_NOREPLACE);
  redraw_hilights(0);
}

void hilight_parent_pins(void)
{
 int rects, i, j, k;
 Hilight_hashentry *entry;
 const char *pin_name;
 char *pin_node = NULL;
 char *net_node=NULL;
 int mult, net_mult, inst_number;

 if(!xctx->hilight_nets) return;
 prepare_netlist_structs(0);
 i=xctx->previous_instance[xctx->currsch];
 inst_number = xctx->sch_inst_number[xctx->currsch];

 /* may be set to -1 by descend_symbol to notify we are
  * descending into a smbol from an instance with no embed flag set 
  * this is used when descending into symbols created from generators */
 if(inst_number == -1) inst_number = 1;

 dbg(1, "hilight_parent_pins(): previous_instance=%d\n", xctx->previous_instance[xctx->currsch]);
 dbg(1, "hilight_parent_pins(): inst_number=%d\n", inst_number);

 rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];

 /* propagate global nets */
 for(j=0;j<HASHSIZE; ++j) {
   entry=xctx->hilight_table[j];
   for( entry=xctx->hilight_table[j]; entry; entry = entry->next) { 
     if(entry->token[0] == ' ') continue; /* skip instances, process only nets */
     if(record_global_node(3, NULL, entry->token)) {
       dbg(1, "entry token=%s, value=%d\n", entry->token, entry->value);
       bus_hilight_hash_lookup(entry->token,  entry->value, XINSERT);
     }
   }
 }

 for(j=0;j<rects; ++j)
 {
  char *p_n_s1, *p_n_s2;
  if(!xctx->inst[i].node || !xctx->inst[i].node[j]) continue;
  my_strdup(_ALLOC_ID_, &net_node, expandlabel(xctx->inst[i].node[j], &net_mult));
  dbg(1, "hilight_parent_pins(): net_node=%s\n", net_node);
  pin_name = get_tok_value(xctx->sym[xctx->inst[i].ptr].rect[PINLAYER][j].prop_ptr,"name",0);
  dbg(1, "pin_name=%s\n", pin_name);
  if(!pin_name[0]) continue;
  my_strdup(_ALLOC_ID_, &pin_node, expandlabel(pin_name, &mult));
  dbg(1, "hilight_parent_pins(): pin_node=%s\n", pin_node);

  p_n_s1 = pin_node;
  dbg(1, "p_n_s1=%s\n", p_n_s1);
  for(k = 1; k<=mult; ++k) {
    xctx->currsch++;
    entry = bus_hilight_hash_lookup(my_strtok_r(p_n_s1, ",", "", 0, &p_n_s2), 0, XLOOKUP);
    p_n_s1 = NULL;
    xctx->currsch--;
    if(entry)
    {
      dbg(1, "found hilight entry in child: %s\n", entry->token);
      bus_hilight_hash_lookup(find_nth(net_node, ",", "", 0, 
          ((inst_number - 1) * mult + k - 1) % net_mult + 1), entry->value, XINSERT);
    }
    else
    {
      /* This causes deleting a parent node hilight if two or more child pins
       * (where only one or some of them hilighted) are attached to the same parent net
       * Commenting out below two lines will never unhilight parent nets when going up
       * in the hierarchy. If you want to see how child pins propagate upstream
       * you should unhilight all net probes, hilight the desired child pins and then go up */

      /* 
       * bus_hilight_hash_lookup(find_nth(net_node, ",", "", 0,
       *     ((inst_number - 1) * mult + k - 1) % net_mult + 1), 0, XDELETE);
       */
    }
  }
 }
 my_free(_ALLOC_ID_, &pin_node);
 my_free(_ALLOC_ID_, &net_node);
}

void hilight_child_pins(void)
{
 int j, k, rects;
 const char *pin_name;
 char *pin_node = NULL;
 char *net_node=NULL;
 Hilight_hashentry *entry;
 int mult, net_mult, i, inst_number;

 i = xctx->previous_instance[xctx->currsch-1];
 if(!xctx->hilight_nets) {
   return;
 }
 prepare_netlist_structs(0);
 rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];
 inst_number = xctx->sch_inst_number[xctx->currsch-1];

 /* propagate global nets */
 for(j=0;j<HASHSIZE; ++j) {
   entry=xctx->hilight_table[j];
   for( entry=xctx->hilight_table[j]; entry; entry = entry->next) {
     if(entry->token[0] == ' ') continue; /* skip instances, process only nets */
     if(record_global_node(3, NULL, entry->token)) {
       dbg(1, "entry token=%s, value=%d\n", entry->token, entry->value);
       bus_hilight_hash_lookup(entry->token,  entry->value, XINSERT);
     }
   }
 }

 /* may be set to -1 by descend_symbol to notify we are
  * descending into a smbol from an instance with no embed flag set 
  * this is used when descending into symbols created from generators */
 if(inst_number == -1) inst_number = 1;

 for(j=0;j<rects; ++j)
 {
  char *p_n_s1, *p_n_s2;
  dbg(1, "hilight_child_pins(): inst_number=%d\n", inst_number);

  if(!xctx->inst[i].node || !xctx->inst[i].node[j]) continue;
  my_strdup(_ALLOC_ID_, &net_node, expandlabel(xctx->inst[i].node[j], &net_mult));
  dbg(1, "hilight_child_pins(): net_node=%s\n", net_node);
  pin_name = get_tok_value(xctx->sym[xctx->inst[i].ptr].rect[PINLAYER][j].prop_ptr,"name",0);
  if(!pin_name[0]) continue;
  my_strdup(_ALLOC_ID_, &pin_node, expandlabel(pin_name, &mult));
  dbg(1, "hilight_child_pins(): pin_node=%s\n", pin_node);
  p_n_s1 = pin_node;
  for(k = 1; k<=mult; ++k) {
    dbg(1, "hilight_child_pins(): looking nth net:%d, k=%d, inst_number=%d, mult=%d\n",
                               (inst_number-1)*mult+k, k, inst_number, mult);
    xctx->currsch--;
    entry = bus_hilight_hash_lookup(find_nth(net_node, ",", "", 0,
      ((inst_number - 1) * mult + k - 1) % net_mult + 1), 0, XLOOKUP);
    xctx->currsch++;
    if(entry) {
      bus_hilight_hash_lookup(my_strtok_r(p_n_s1, ",", "", 0, &p_n_s2), entry->value, XINSERT_NOREPLACE);
    }
    else {
      bus_hilight_hash_lookup(my_strtok_r(p_n_s1, ",", "", 0, &p_n_s2), 0, XDELETE);
    }
    p_n_s1 = NULL;
  } /* for(k..) */
 }
 my_free(_ALLOC_ID_, &pin_node);
 my_free(_ALLOC_ID_, &net_node);
}


static int bus_search(const char*s)
{
 int c, bus=0;
 while( (c=*s++) ) {
   if(c=='[')  bus=1;
   if( (c==':') || (c==',') ) {bus=0; break;}
 }
 return bus;
}

#ifndef __unix__
int win_regexec(const char *options, const char *pattern, const char *name) 
{
  if (options!=NULL)
    tclvareval("regexp {", options,"} {", pattern, "} {", name, "}", NULL);
  else
    tclvareval("regexp {", pattern, "} {", name, "}", NULL);
  int ret = atoi(tclresult());
  if (ret > 0)
      return 1;
  return 0;
}
#endif

/* sel: -1 --> unselect
 *       1 --> select
 *       0 --> highlight
 * sub:
 *       0 : regex search
 *       1 : exact search
 */
int search(const char *tok, const char *val, int sub, int sel, int match_case)
{
 int save_draw;
 int i,c, col = 7,tmp,bus=0;
 const char *str;
 char *type;
 size_t has_token;
 const char *empty_string = "";
 char *tmpname=NULL;
 int found = 0;
 int(*comparefn)(const char *,const char *) = strcmp;
 char *(*substrfn)(const char *,const char *) = strstr;
 #ifdef __unix__
 int cflags = REG_NOSUB | REG_EXTENDED;
 regex_t re;
 #else
 char *regexp_options = NULL;
 #endif

 if(!val) {
   fprintf(errfp, "search(): warning: null val key\n");
   return TCL_ERROR;
 }
 save_draw = xctx->draw_window;
 xctx->draw_window=1;
 /* replace strcmp and strstr with my_strcasecmp and my_strcasestr 
  * if SPICE or VHDL (case insensitive) netlist mode is set */
 if(!match_case) {
   comparefn = my_strcasecmp;
   substrfn = my_strcasestr;
 }
 #ifdef __unix__
 if(!match_case) {
   cflags |= REG_ICASE; /* ignore case for Spice and VHDL (these are case insensitive netlists) */
 }
 if(regcomp(&re, val , cflags)) return TCL_ERROR;
 #else 
 if(!match_case) {
   my_strdup(_ALLOC_ID_, &regexp_options, "-nocase");
 }
 #endif
 dbg(1, "search():val=%s\n", val);
 if(!sel) {
   col=xctx->hilight_color;
 }
 has_token = 0;
 prepare_netlist_structs(0);
 bus=bus_search(val); /* searching for a single bit in a bus, like val -> "DATA[13]" */
 for(i=0;i<xctx->instances; ++i) {
   if(!strcmp(tok,"cell::name")) {
     has_token = (xctx->inst[i].name != NULL) && xctx->inst[i].name[0];
     str = xctx->inst[i].name;
   } else if(!strcmp(tok,"cell::propstring")) {
     has_token = (str = (xctx->inst[i].ptr+ xctx->sym)->prop_ptr) ? 1 : 0;
   } else if(!strncmp(tok,"cell::", 6)) { /* cell::xxx looks for xxx in global symbol attributes */
     my_strdup(_ALLOC_ID_, &tmpname,get_tok_value(xctx->sym[xctx->inst[i].ptr].prop_ptr,tok+6,0));
     has_token = xctx->tok_size;
     if(tmpname) {
       str = tmpname;
     } else {
       str = empty_string;
     }
   } else if(!strcmp(tok,"propstring")) {
     has_token = (xctx->inst[i].prop_ptr != NULL) && xctx->inst[i].prop_ptr[0];
     str = xctx->inst[i].prop_ptr;
   } else {
     str = get_tok_value(xctx->inst[i].prop_ptr, tok,0);
     has_token = xctx->tok_size;
   }
   dbg(1, "search(): inst=%d, tok=%s, val=%s \n", i,tok, str);

   if(bus && sub) {
    dbg(1, "search(): doing substr search on bus sig:%s inst=%d tok=%s val=%s\n", str,i,tok,val);
    str=expandlabel(str,&tmp);
   }
   if(str && has_token) {
     #ifdef __unix__
     if( (!sub && !regexec(&re, str,0 , NULL, 0) ) ||           /* 20071120 regex instead of strcmp */
         (sub && !bus && !comparefn(str, val)) || (sub && bus && substrfn(str,val)))
     #else
     if( (!sub && win_regexec(regexp_options, val, str)) ||
         (sub && !bus && !comparefn(str, val)) || (sub && bus && substrfn(str,val)))
     #endif
     {
       if(!sel) {
         type = (xctx->inst[i].ptr+ xctx->sym)->type;
         if( !strcmp(tok, "lab") && type && xctx->inst[i].node && IS_LABEL_SH_OR_PIN(type) ) {
           bus_hilight_hash_lookup(xctx->inst[i].node[0], col, XINSERT_NOREPLACE); /* sets xctx->hilight_nets=1; */
         } else {
           dbg(1, "search(): setting hilight flag on inst %d\n",i);
           /* xctx->hilight_nets=1; */  /* done in hilight_hash_lookup() */
           xctx->inst[i].color = col;
           inst_hilight_hash_lookup(i, col, XINSERT_NOREPLACE);
         }
       }
       if(sel==1) {
         xctx->inst[i].sel = SELECTED;
         set_first_sel(ELEMENT, i, 0);
         xctx->need_reb_sel_arr=1;
       }
       if(sel==-1) { /* 20171211 unselect */
         xctx->inst[i].sel = 0;
         xctx->need_reb_sel_arr=1;
      }
      found  = 1;
     }
   }
 }
 for(i=0;i<xctx->wires; ++i) {
   str = get_tok_value(xctx->wire[i].prop_ptr, tok,0);
   if(xctx->tok_size ) {
     #ifdef __unix__
     if(   (!regexec(&re, str,0 , NULL, 0) && !sub )  ||       /* 20071120 regex instead of strcmp */
           ( !comparefn(str, val) &&  sub ) )
     #else
       if(   (win_regexec(regexp_options, val, str) && !sub )  ||       /* 20071120 regex instead of strcmp */
           ( !comparefn(str, val) &&  sub ) )

     #endif
     {
       if(!sel) {
         bus_hilight_hash_lookup(xctx->wire[i].node, col, XINSERT_NOREPLACE); /* sets xctx->hilight_nets = 1 */
       }
       if(sel==1) {
         xctx->wire[i].sel = SELECTED;
         set_first_sel(WIRE, i, 0);
         xctx->need_reb_sel_arr=1;
       }
       if(sel==-1) {
         xctx->wire[i].sel = 0;
         xctx->need_reb_sel_arr=1;
       }
       found = 1;
     }
     else {
       dbg(2, "search():  not found wire=%d, tok=%s, val=%s search=%s\n", i,tok, str,val);
     }
   }
 }
 if(!sel && xctx->hilight_nets) propagate_hilights(1, 0, XINSERT_NOREPLACE);
 if(sel) for(c = 0; c < cadlayers; ++c) for(i=0;i<xctx->lines[c]; ++i) {
   str = get_tok_value(xctx->line[c][i].prop_ptr, tok,0);
   if(xctx->tok_size) {
     #ifdef __unix__
     if( (!regexec(&re, str,0 , NULL, 0) && !sub ) ||
         ( !comparefn(str, val) &&  sub ))
     #else
     if( (win_regexec(regexp_options, val, str) && !sub ) ||
         ( !comparefn(str, val) &&  sub ))
     #endif
     {
       if(sel==1) {
         xctx->line[c][i].sel = SELECTED;
         set_first_sel(LINE, i, c);
         xctx->need_reb_sel_arr=1;
       }
       if(sel==-1) {
         xctx->line[c][i].sel = 0;
         xctx->need_reb_sel_arr=1;
       }
       found = 1;
     }
     else {
       dbg(2, "search(): not found line=%d col=%d, tok=%s, val=%s search=%s\n",
                           i, c, tok, str, val);
     }
   }
 }
 if(sel) for(c = 0; c < cadlayers; ++c) for(i=0;i<xctx->rects[c]; ++i) {
   str = get_tok_value(xctx->rect[c][i].prop_ptr, tok,0);
   if(xctx->tok_size) {
     #ifdef __unix__
     if( (!regexec(&re, str,0 , NULL, 0) && !sub ) ||
         ( !comparefn(str, val) &&  sub ))
     #else
     if( (win_regexec(regexp_options, val, str) && !sub ) ||
         ( !comparefn(str, val) &&  sub ))
     #endif
     {
         if(sel==1) {
           xctx->rect[c][i].sel = SELECTED;
           set_first_sel(xRECT, i, c);
           xctx->need_reb_sel_arr=1;
         }
         if(sel==-1) {
           xctx->rect[c][i].sel = 0;
           xctx->need_reb_sel_arr=1;
         }
         found = 1;
     }
     else {
       dbg(2, "search(): not found rect=%d col=%d, tok=%s, val=%s search=%s\n",
                           i, c, tok, str, val);
     }
   }
 }

 if(sel) for(c = 0; c < cadlayers; ++c) for(i=0;i<xctx->arcs[c]; ++i) {
   str = get_tok_value(xctx->arc[c][i].prop_ptr, tok,0);
   if(xctx->tok_size) {
     #ifdef __unix__
     if( (!regexec(&re, str,0 , NULL, 0) && !sub ) ||
         ( !comparefn(str, val) &&  sub ))
     #else
     if( (win_regexec(regexp_options, val, str) && !sub ) ||
         ( !comparefn(str, val) &&  sub ))
     #endif
     {
         if(sel==1) {
           xctx->arc[c][i].sel = SELECTED;
           set_first_sel(ARC, i, c);
           xctx->need_reb_sel_arr=1;
         }
         if(sel==-1) {
           xctx->arc[c][i].sel = 0;
           xctx->need_reb_sel_arr=1;
         }
         found = 1;
     }
     else {
       dbg(2, "search(): not found arc=%d col=%d, tok=%s, val=%s search=%s\n",
                           i, c, tok, str, val);
     }
   }
 }

 if(sel) for(c = 0; c < cadlayers; ++c) for(i=0;i<xctx->polygons[c]; ++i) {
   str = get_tok_value(xctx->poly[c][i].prop_ptr, tok,0);
   if(xctx->tok_size) {
     #ifdef __unix__
     if( (!regexec(&re, str,0 , NULL, 0) && !sub ) ||
         ( !comparefn(str, val) &&  sub ))
     #else
     if( (win_regexec(regexp_options, val, str) && !sub ) ||
         ( !comparefn(str, val) &&  sub ))
     #endif
     {
         if(sel==1) {
           xctx->poly[c][i].sel = SELECTED;
           set_first_sel(POLYGON, i, c);
           xctx->need_reb_sel_arr=1;
         }
         if(sel==-1) {
           xctx->poly[c][i].sel = 0;
           xctx->need_reb_sel_arr=1;
         }
         found = 1;
     }
     else {
       dbg(2, "search(): not found arc=%d col=%d, tok=%s, val=%s search=%s\n",
                           i, c, tok, str, val);
     }
   }
 }


 if(sel) for(i=0;i<xctx->texts; ++i) {
   str = get_tok_value(xctx->text[i].prop_ptr, tok,0);
   if(xctx->tok_size) {
     #ifdef __unix__
     if( (!regexec(&re, str,0 , NULL, 0) && !sub ) ||
         ( !comparefn(str, val) &&  sub ))
     #else
     if( (win_regexec(regexp_options, val, str) && !sub ) ||
         ( !comparefn(str, val) &&  sub ))
     #endif
     {
         if(sel==1) {
           xctx->text[i].sel = SELECTED;
           set_first_sel(xTEXT, i, 0);
           xctx->need_reb_sel_arr=1;
         }
         if(sel==-1) {
           xctx->text[i].sel = 0;
           xctx->need_reb_sel_arr=1;
         }
         found = 1;
     }
     else {
       dbg(2, "search(): not found text=%d, tok=%s, val=%s search=%s\n",
                           i, tok, str, val);
     }
   }
 }

 if(found) {
  if(tclgetboolvar("incr_hilight")) incr_hilight_color();
   if(sel == -1) {
     draw();
   }
   if(sel) {
     rebuild_selected_array(); /* sets or clears xctx->ui_state SELECTION flag */
     draw_selection(xctx->gc[SELLAYER], 0);
   }
   else redraw_hilights(0);
 }
 #ifdef __unix__
 regfree(&re);
#else
 my_free(_ALLOC_ID_, &regexp_options);
 #endif
 xctx->draw_window = save_draw;
 my_free(_ALLOC_ID_, &tmpname);
 return found;
}

/* "drill" option (pass through resistors or pass gates or whatever elements with  */
/* 'propag' properties set on pins) */
static void drill_hilight(int mode)
{
  char *netname=NULL, *propagated_net=NULL, *propagate_str = NULL;
  const char *netbitname;
  int found, i, j, k, npin, en_hi, propagate, hilight_connected_inst;
  int /* pinmult = 0, instmult = 0, */ mult = 0, mult2 = 0;
  xSymbol *symbol;
  xRect *rct;
  Hilight_hashentry *entry, *propag_entry;

  en_hi = tclgetboolvar("en_hilight_conn_inst");
  prepare_netlist_structs(0);
  while(1) {
    found=0;
    for(i=0; i<xctx->instances; ++i) {
      /* expandlabel(xctx->inst[i].instname, &instmult); */
      symbol = xctx->inst[i].ptr+xctx->sym;
      npin = symbol->rects[PINLAYER];
      rct=symbol->rect[PINLAYER];
      hilight_connected_inst = en_hi &&
       ((xctx->inst[i].flags & HILIGHT_CONN) || (symbol->flags & HILIGHT_CONN));
      for(j=0; j<npin; ++j) {

        if(xctx->inst[i].node && xctx->inst[i].node[j] &&
           strstr(xctx->inst[i].node[j], "#net") == xctx->inst[i].node[j]) {
          my_strdup2(_ALLOC_ID_, &netname,xctx->inst[i].node[j]);
        } else {
          /* mult here will be set to pin multiplicity */
          my_strdup2(_ALLOC_ID_, &netname, net_name(i, j, &mult, 1, 0));
        }
        /* mult here will be set to net multiplicity */
        expandlabel(netname, &mult);
        dbg(1, "inst=%s, pin=%d, netname=%s, mult=%d\n", xctx->inst[i].instname, j, netname, mult);
        for(k = 1; k <= mult; ++k) {
          netbitname = find_nth(netname, ",", "", 0, k);
          dbg(1, "netbitname=%s\n", netbitname);
          if( (entry=bus_hilight_hash_lookup(netbitname, 0, XLOOKUP)) ) {
            if( hilight_connected_inst || (symbol->type && IS_LABEL_SH_OR_PIN(symbol->type)) ) {
              xctx->inst[i].color = entry->value;
              inst_hilight_hash_lookup(i, entry->value, XINSERT_NOREPLACE); 
            }
            my_strdup(_ALLOC_ID_, &propagate_str, get_tok_value(rct[j].prop_ptr, "propag", 0));
            if(propagate_str) {
              int n = 1;
              const char *propag;
              dbg(1, "drill_hilight(): inst=%d propagate_str=%s\n", i, propagate_str);
              while((propag = find_nth(propagate_str, ",", "", 0, n++))[0]) {
                propagate = atoi(propag);

                if(propagate < 0 || propagate >= npin) {
                   dbg(0, "Error: inst: %s, pin %d, propag set to %s <<%d>>\n",
                     xctx->inst[i].instname, j, propagate_str, propagate);
                     continue;
                }
                /* expandlabel(rct[propagate].name, &pinmult); */
                /* get net to propagate hilight to...*/

                if(xctx->inst[i].node && xctx->inst[i].node[propagate] &&
                   strstr(xctx->inst[i].node[propagate], "#net") == xctx->inst[i].node[propagate]) {
                  my_strdup2(_ALLOC_ID_, &propagated_net,xctx->inst[i].node[propagate]);
                } else {
                  my_strdup2(_ALLOC_ID_, &propagated_net, net_name(i, propagate, &mult2, 1, 0));
                }
                netbitname = find_nth(propagated_net, ",", "", 0, k);
                dbg(1, "netbitname=%s\n", netbitname);
                /* add net to highlight list */
                if(!netbitname[0]) continue;
                propag_entry = bus_hilight_hash_lookup(netbitname, entry->value, mode);
                if(!propag_entry) found=1; /* keep looping until no more nets are found. */
              }
            }
          } /* if(entry) */
        } /* for(k = 1; k <= mult; ++k) */
      } /* for(j...) */
    } /* for(i...) */
    if(!found) break;
  } /* while(1) */
  my_free(_ALLOC_ID_, &netname);
  if(propagated_net) my_free(_ALLOC_ID_, &propagated_net);
  if(propagate_str) my_free(_ALLOC_ID_, &propagate_str);
}

/* if fast is set you need to do a propagate_hilights() at the end to finalize the operation */
int hilight_netname(const char *name, int fast)
{
  Node_hashentry *node_entry;
  prepare_netlist_structs(0);
  dbg(1, "hilight_netname(): entering\n");
  rebuild_selected_array();
  node_entry = bus_node_hash_lookup(name, "", XLOOKUP, 0, "", "", "", "");
                    /* sets xctx->hilight_nets=1 */
  if(node_entry && !bus_hilight_hash_lookup(name, xctx->hilight_color, XINSERT_NOREPLACE)) {
    if(!fast) {
      propagate_hilights(1, 0, XINSERT_NOREPLACE);
      if(tclgetboolvar("incr_hilight")) incr_hilight_color();
      redraw_hilights(0);
    }
  }
  return node_entry ? 1 : 0;
}

static void send_net_to_bespice(int simtype, const char *node)
{
  int c, k, tok_mult;
  Node_hashentry *node_entry;
  const char *expanded_tok;
  const char *tok;
  char color_str[30];

  set_rawfile_for_bespice();

  if(!node || !node[0]) return;
  tok = node;
  node_entry = bus_node_hash_lookup(tok, "", XLOOKUP, 0, "", "", "", "");
  if(tok[0] == '#') tok++;
  if(node_entry  && (node_entry->d.port == 0 || !strcmp(xctx->sch_path[xctx->currsch], ".") )) {
    char *t=NULL, *p=NULL;
    c = get_color(xctx->hilight_color);
    sprintf(color_str, "#%02x%02x%02x", xctx->xcolor_array[c].red>>8, xctx->xcolor_array[c].green>>8,
                                       xctx->xcolor_array[c].blue>>8);
    expanded_tok = expandlabel(tok, &tok_mult);
    my_strdup2(_ALLOC_ID_, &p, xctx->sch_path[xctx->currsch]+1);
    for(k=1; k<=tok_mult; ++k) {
      my_strdup(_ALLOC_ID_, &t, find_nth(expanded_tok, ",", "", 0, k));
      /* bespice command syntax : 
            add_voltage_on_spice_node_to_plot <plot name> <section name> <hierarchical spice node name> <flag clear> [<color>]
                plot name is "*" => automatic
                section name is empty => automatic or user defined
      */
      tclvareval(
              "puts $bespice_server_getdata(sock) ",
              "{add_voltage_on_spice_node_to_plot * \"\" \"",
              p, t, "\" 0 ", color_str, "}",
              NULL);
    }
    my_free(_ALLOC_ID_, &p);
    my_free(_ALLOC_ID_, &t);
  }
}

static void send_net_to_graph(char **s, int simtype, const char *node)
{
  int c, k, tok_mult;
  Node_hashentry *node_entry;
  const char *expanded_tok;
  const char *tok;
  char ss[1024] = "";
  if(!node || !node[0]) return;
  tok = node;
  node_entry = bus_node_hash_lookup(tok, "", XLOOKUP, 0, "", "", "", "");
  if(tok[0] == '#') tok++;
  if(node_entry  && (node_entry->d.port == 0 ||
                     /* !strcmp(xctx->sch_path[xctx->currsch], ".") */
                       xctx->currsch == xctx->raw->level
                    )) {
    char *t=NULL, *p=NULL;
    char *path;
    int start_level;
    c = get_color(xctx->hilight_color);
    expanded_tok = expandlabel(tok, &tok_mult);
    my_strdup2(_ALLOC_ID_, &p, xctx->sch_path[xctx->currsch]+1);
    path = p;
    start_level = sch_waves_loaded();
    if(path) {
      int skip = 0;
      /* skip path components that are above the level where raw file was loaded */
      while(*path && skip < start_level) {
        if(*path == '.') skip++;
        ++path;
      }
    }
    strtolower(path);
    for(k=1; k<=tok_mult; ++k) {
      my_strdup(_ALLOC_ID_, &t, find_nth(expanded_tok, ",", "", 0, k));
      strtolower(t);
      if(simtype == 0 ) { /* ngspice */
        dbg(1, "%s%s color=%d\n", path, t, c);
        my_snprintf(ss, S(ss), "%s%s %d ", path, t, c);
        my_strcat(_ALLOC_ID_, s, ss);
      } else { /* Xyce */
        my_snprintf(ss, S(ss), "%s%s %d", path, t, c);
        my_strcat(_ALLOC_ID_, s, ss);
      }

    }
    my_free(_ALLOC_ID_, &p);
    my_free(_ALLOC_ID_, &t);
  }
}

static void send_net_to_gaw(int simtype, const char *node)
{
  int c, k, tok_mult;
  Node_hashentry *node_entry;
  const char *expanded_tok;
  const char *tok;
  char color_str[8];

  if(!node || !node[0]) return;
  tok = node;
  node_entry = bus_node_hash_lookup(tok, "", XLOOKUP, 0, "", "", "", "");
  if(tok[0] == '#') tok++;
  if(node_entry  && (node_entry->d.port == 0 || !strcmp(xctx->sch_path[xctx->currsch], ".") )) {
    char *t=NULL, *p=NULL;
    char *path;
    c = get_color(xctx->hilight_color);
    sprintf(color_str, "%02x%02x%02x", xctx->xcolor_array[c].red>>8, xctx->xcolor_array[c].green>>8,
                                       xctx->xcolor_array[c].blue>>8);
    expanded_tok = expandlabel(tok, &tok_mult);
    tcleval("setup_tcp_gaw");
    if(tclresult()[0] == '0') return;
    my_strdup2(_ALLOC_ID_, &p, xctx->sch_path[xctx->currsch]+1);
    path = p;
    strtolower(path);
    for(k=1; k<=tok_mult; ++k) {
      my_strdup(_ALLOC_ID_, &t, find_nth(expanded_tok, ",", "", 0, k));
      strtolower(t);
      if(simtype == 0 ) { /* ngspice */
        tclvareval("puts $gaw_fd {copyvar v(", path, t,
                    ") sel #", color_str, "}\nvwait gaw_fd\n", NULL);
      } else { /* Xyce */
        tclvareval("puts $gaw_fd {copyvar v(", path, t,
                    ") sel #", color_str, "}\nvwait gaw_fd\n", NULL);
      }
    }
    my_free(_ALLOC_ID_, &p);
    my_free(_ALLOC_ID_, &t);
  }
}

static void send_current_to_bespice(int simtype, const char *node)
{
  int c, k, tok_mult;
  const char *expanded_tok;
  const char *tok;
  char color_str[30];
  char *t=NULL, *p=NULL;
  
  set_rawfile_for_bespice();

  if(!node || !node[0]) return;
  tok = node;
  /* c = PINLAYER; */
  c = get_color(xctx->hilight_color);
  sprintf(color_str, "#%02x%02x%02x", xctx->xcolor_array[c].red>>8, xctx->xcolor_array[c].green>>8,
                                     xctx->xcolor_array[c].blue>>8);
  expanded_tok = expandlabel(tok, &tok_mult);
  my_strdup2(_ALLOC_ID_, &p, xctx->sch_path[xctx->currsch]+1);
  for(k=1; k<=tok_mult; ++k) {
    my_strdup(_ALLOC_ID_, &t, find_nth(expanded_tok, ",", "", 0, k));
    /* bespice command syntax : 
        add_current_through_spice_device_to_plot <plot name> <section name> <hierarchical spice device name> <flag clear> [<color>]
            plot name is "*" => automatic
            section name is empty => automatic or user defined
    */
    tclvareval(
          "puts $bespice_server_getdata(sock) ",
          "{add_current_through_spice_device_to_plot * \"\" \"",
          p, t, "\" 0 ", color_str, "}",
          NULL);
  }
  my_free(_ALLOC_ID_, &p);
  my_free(_ALLOC_ID_, &t);
}

static void send_current_to_graph(char **s, int simtype, const char *node)
{
  int c, k, tok_mult, start_level, there_is_hierarchy;
  const char *expanded_tok;
  const char *tok;
  char *t=NULL, *p=NULL, *path;
  char ss[1024] = "";

  if(!node || !node[0]) return;
  tok = node;
  c = get_color(xctx->hilight_color);
  expanded_tok = expandlabel(tok, &tok_mult);
  my_strdup2(_ALLOC_ID_, &p, xctx->sch_path[xctx->currsch]+1);
  path = p;
  start_level = sch_waves_loaded();
  if(path) {
    int skip = 0;
    /* skip path components that are above the level where raw file was loaded */
    while(*path && skip < start_level) {
      if(*path == '.') skip++;
      ++path;
    }
  }
  strtolower(path);
  there_is_hierarchy = (strstr(path, ".") != NULL);
  for(k=1; k<=tok_mult; ++k) {
    my_strdup(_ALLOC_ID_, &t, find_nth(expanded_tok, ",", "", 0, k));
    strtolower(t);
    if(!simtype) { /* ngspice */
      my_snprintf(ss, S(ss), "i(%s%s%s) %d", there_is_hierarchy ? "v." : "", path, t, c);
      my_strcat(_ALLOC_ID_, s, ss);
    } else { /* Xyce */
      /* 
      my_snprintf(ss, S(ss), "%s%s%s#branch %d", there_is_hierarchy ? "v." : "",
                  path, (there_is_hierarchy ? t+1 : t) , c);
      */
      my_snprintf(ss, S(ss), "i(%s%s) %d", path, t, c);
      my_strcat(_ALLOC_ID_, s, ss);
    }
  }
  my_free(_ALLOC_ID_, &p);
  my_free(_ALLOC_ID_, &t);
}

static void send_current_to_gaw(int simtype, const char *node)
{
  int c, k, tok_mult, there_is_hierarchy;
  const char *expanded_tok;
  const char *tok;
  char color_str[8];
  char *t=NULL, *p=NULL, *path;

  if(!node || !node[0]) return;
  tok = node;
  /* c = PINLAYER; */
  c = get_color(xctx->hilight_color);
  sprintf(color_str, "%02x%02x%02x", xctx->xcolor_array[c].red>>8, xctx->xcolor_array[c].green>>8,
                                     xctx->xcolor_array[c].blue>>8);
  expanded_tok = expandlabel(tok, &tok_mult);
  tcleval("setup_tcp_gaw");
  if(tclresult()[0] == '0') return;
  my_strdup2(_ALLOC_ID_, &p, xctx->sch_path[xctx->currsch]+1);
  path = p;
  strtolower(path);
  there_is_hierarchy = (xctx->currsch > 0);
  for(k=1; k<=tok_mult; ++k) {
    my_strdup(_ALLOC_ID_, &t, find_nth(expanded_tok, ",", "", 0, k));
    strtolower(t);
    if(!simtype) { /* spice */
      tclvareval("puts $gaw_fd {copyvar i(", there_is_hierarchy ? "v." : "", path, t,
                  ") sel #", color_str, "}\nvwait gaw_fd\n", NULL);
    } else {       /* Xyce */
      char *c=p;
      while(*c){
        if(*c == '.') *c = ':'; /* Xyce uses : as path separator */
        ++c;
      }
      /*
      tclvareval("puts $gaw_fd {copyvar ", there_is_hierarchy ? "V:" : "",
                  path,  there_is_hierarchy ? t+1 : t , "#branch",
                  " sel #", color_str, "}\nvwait gaw_fd\n", NULL);
      */
      tclvareval("puts $gaw_fd {copyvar i(", path, t ,")",
                  " sel #", color_str, "}\nvwait gaw_fd\n", NULL);
    }
  }
  my_free(_ALLOC_ID_, &p);
  my_free(_ALLOC_ID_, &t);
}

/* hilight/clear pin/label instances attached to hilight nets, or instances with "highlight=true"
 * attr if en_hilight_conn_inst option is set
 */
void propagate_hilights(int set, int clear, int mode)
{
  int i, hilight_connected_inst;
  Hilight_hashentry *entry;
  char *type;
  int en_hi;

  dbg(1, "propagate_hilights() for %s\n", xctx->current_name);
  en_hi = tclgetboolvar("en_hilight_conn_inst");
  
  prepare_netlist_structs(0);
  for(i = 0; i < xctx->instances; ++i) {
    if(xctx->inst[i].ptr < 0 ) {
      dbg(0, "propagate_hilights(): .ptr<0, unbound symbol: inst %d, name=%s sch=%s\n",
          i, xctx->inst[i].instname, xctx->current_name);
      continue;
    }
    type = (xctx->inst[i].ptr+ xctx->sym)->type;
    hilight_connected_inst = en_hi && 
      ((xctx->inst[i].flags & HILIGHT_CONN) || ((xctx->inst[i].ptr+ xctx->sym)->flags & HILIGHT_CONN));
    /* hilight/clear instances with highlight=true attr set and en_hilight_conn_inst option is set ... */
    if(type && !IS_LABEL_SH_OR_PIN(type)) {
      if (hilight_connected_inst) {
        int rects, j, nohilight_pins;
        if( (rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER]) > 0 ) {
          nohilight_pins = 1;
          for(j=0;j<rects; ++j) {
            if( xctx->inst[i].node && xctx->inst[i].node[j]) {
              entry=bus_hilight_hash_lookup(xctx->inst[i].node[j], 0, XLOOKUP);
              if(entry) {
                if(set) {
                  xctx->inst[i].color=entry->value;
                  inst_hilight_hash_lookup(i, entry->value, XINSERT_NOREPLACE);
                } else {
                  nohilight_pins = 0; /* at least one connected net is hilighted: keep instance hilighted */
                }
                break;
              }
            }
          }
          if(nohilight_pins && clear) {
            xctx->inst[i].color=-10000;
          }
        }
      }
      else {
        entry=inst_hilight_hash_lookup(i, 0, XLOOKUP);
        if (entry && set) xctx->inst[i].color=entry->value;
      }
    /* ... else hilight/clear pin/label instances attached to hilight nets */
    } else if(type && xctx->inst[i].node && IS_LABEL_SH_OR_PIN(type) ) {
      entry=bus_hilight_hash_lookup( xctx->inst[i].node[0], 0, XLOOKUP);
      if(entry && set) {
        xctx->inst[i].color = entry->value;
        inst_hilight_hash_lookup(i, entry->value, XINSERT_NOREPLACE); 
      }
      else if(!entry && clear) xctx->inst[i].color = -10000;
    }
  }
  xctx->hilight_nets = there_are_hilights();
  if(xctx->hilight_nets && xctx->enable_drill && set) drill_hilight(mode);
}

/* use negative values to bypass the normal hilight color enumeration */
#define LOGIC_0      -12  /* 0 */
#define LOGIC_1      -5   /* 1 */
#define LOGIC_X      -1   /* 2 */
#define LOGIC_Z      -13  /* 3 */
#define LOGIC_NOUP    0   /* 4 don't update */

#define STACKMAX 200

static int get_logic_value(int inst, int n)
{
  int /* mult, */ val = 2; /* LOGIC_X */
  Hilight_hashentry *entry;
  /* char *netname = NULL; */

  if(!xctx->inst[inst].node || !xctx->inst[inst].node[n]) return val;
  /* fast option: dont use net_name() (no expandlabel though) */
  /* THIS MUST BE DONE TO HANDLE VECTOR INSTANCES/BUSES */
  /* my_strdup(xxxx, &netname, net_name(inst, n, &mult, 1, 0)); */
  entry=hilight_hash_lookup(xctx->inst[inst].node[n], 0, XLOOKUP);
  if(entry) {
    val = entry->value;
    val = (val == LOGIC_0) ? 0 : (val == LOGIC_1) ? 1 : (val == LOGIC_Z) ? 3 : 2;
    /* dbg(1, "get_logic_value(): inst=%d pin=%d net=%s val=%d\n", inst, n, netname, val); */
  }
  /* my_free(xxxx, &netname); */
  return val;
}

static int eval_logic_expr(int inst, int output)
{
  int stack[STACKMAX];
  int pos = 0, i, s, sp = 0;
  char *str;
  int res = 0;

  stack[0] = 2; /* default if nothing is calculated: LOGIC_X */
  str = xctx->simdata[inst].pin[output].function;
  dbg(1, "eval_logic_expr(): inst=%d pin=%d function=%s\n", inst, output, str ? str : "<NULL>");
  if(!str) return 2; /* no logic function defined, return LOGIC_X */
  while(str[pos]) {
    switch(str[pos]) {
      case 'd': /* duplicate top element*/
        if(sp > 0 && sp < STACKMAX) {
          stack[sp] = stack[sp - 1];
          ++sp;
        }
        break;
      case 'r': /* rotate down: bottom element goes to top */
        if(sp > 1) {
          s = stack[0];
          for(i = 0 ; i < sp - 1; ++i) stack[i] = stack[i + 1];
          stack[sp - 1] = s;
        }
        break;
      case 'x': /* exchange top 2 operands */
        if(sp > 1) {
           s = stack[sp - 2];
           stack[sp - 2] =  stack[sp - 1];
           stack[sp - 1] = s;
        }
        break;
      case '~': /* negation operator */
        if(sp > 0) {
          sp--;
          if(stack[sp] < 2) stack[sp] = !stack[sp];
          else stack[sp] = 2;
          ++sp;
        }
        break;
      case  'z': /* Tristate driver [signal,enable,'z']-> signal if z==1, Z (3) otherwise */
        if(sp > 1) {
          s = stack[sp - 1];
          stack[sp - 2] = (s > 1) ? 2 : (s == 1 ) ? stack[sp - 2] : 3;
          sp--;
        }
        break;
      case 'R': /* resolution operator, resolve output based on 2 inputs */
        if(sp > 1) {
          s = stack[sp - 1];
          i = stack[sp - 2];
          if(s == 2 || i == 2) res = 2;  /*    s 0 1 X Z   */
          else if(s == 3)      res = i;  /*  i   -------   */
          else if(i == 3)      res = s;  /*  0 | 0 X X 0   */
          else if(i == s)      res = i;  /*  1 | X 1 X 1   */
          else                 res = 2;  /*  X | X X X X   */
          stack[sp - 2] = res;           /*  Z | 0 1 X Z   */
          sp--;
        }
        break;
      case 'M': /* mux operator */
        if(sp > 2) {
          s = stack[sp - 1];
          if(s < 2) {
            stack[sp - 3] = (s == 0) ? stack[sp - 3] : (s == 1) ? stack[sp - 2] : 2;
          }
          else stack[sp - 3] = 2; /* setting to 2 (X) may lead to simulation deadlocks */
          sp -=2;
        }
        break;
      case 'm': /* mux operator , lower priority*/
        if(sp > 2) {
          s = stack[sp - 1];
          stack[sp - 3] = (stack[sp - 3] == 2) ? 4 : stack[sp - 3]; /* if LOGIC_X set to don't update */
          stack[sp - 2] = (stack[sp - 2] == 2) ? 4 : stack[sp - 2]; /* if LOGIC_X set to don't update */
          if(s < 2) {
            stack[sp - 3] = (s == 0) ? stack[sp - 3]  : stack[sp - 2];
          }
          else stack[sp - 3] = 4; /* don't update, to avoid deadlocks */
          sp -=2;
        }
        break;
      case '|': /* or operator */
        if(sp > 1) {
          res = 0;
          for(i = sp - 2; i < sp; ++i) {
            if(stack[i] == 1) {
              res = 1;
              break;
            } else if(stack[i] > 1) {
              res = 2;
            }
          }
          stack[sp - 2] = res;
          sp--;
        }
        break;
      case '&': /* and operator */
        if(sp > 1) {
          res = 1;
          for(i = sp - 2; i < sp; ++i) {
            if(stack[i] == 0) {
              res = 0;
              break;
            } else if(stack[i] > 1) {
              res = 2;
            }
          }
          stack[sp - 2] = res;
          sp--;
        }
        break;
      case '^': /* xor operator */
        if(sp > 1) {
          res = 0;
          for(i = sp - 2; i < sp; ++i) {
            if(stack[i] < 2) {
              res = res ^ stack[i];
            }
            else {
              res = 2;
              break;
            }
          }
          stack[sp - 2] = res;
          sp--;
        }
        break;
      case 'L': /* logic low (0) */
        if(sp < STACKMAX) {
          stack[sp++] = 0;
        }
        break;
      case 'H': /* logic high (1) */
        if(sp < STACKMAX) {
          stack[sp++] = 1;
        }
        break;
      case 'Z': /* logic Z (3) */
        if(sp < STACKMAX) {
          stack[sp++] = 3;
        }
        break;
      case 'U': /* Do not assign to node */
        if(sp < STACKMAX) {
          stack[sp++] = 4;
        }
        break;
      default:
        break;
    } /* switch */
    if(isdigit(str[pos])) {
      if(sp < STACKMAX) {
        char *num = str + pos;
        while(isdigit(str[++pos])) ;
        pos--; /* push back last non digit character */
        stack[sp++] = get_logic_value(inst, atoi(num));
      }
      else dbg(0, "eval_logic_expr(): stack overflow!\n");
    }
    ++pos;
  } /* while */
  dbg(1, "eval_logic_expr(): inst %d output %d, returning %d\n", inst, output, stack[0]);
  return stack[0];
}


/* fast access to symbol "function#" and symbol pin "clock" and "goto" attributes
 * to minimize get_token_value() lookups in simulation loops
 */

static void create_simdata(void)
{
  int i, j;
  const char *str;
  free_simdata();
  my_realloc(_ALLOC_ID_, &xctx->simdata, xctx->instances * sizeof(Simdata));
  xctx->simdata_ninst =  xctx->instances;
  for(i = 0; i < xctx->instances; ++i) {
    xSymbol *symbol = xctx->inst[i].ptr + xctx->sym;
    int npin = symbol->rects[PINLAYER];
    xctx->simdata[i].pin = NULL;
    if(npin) my_realloc(_ALLOC_ID_, &xctx->simdata[i].pin, npin * sizeof(Simdata_pin));
    xctx->simdata[i].npin = npin;
    for(j = 0; j < npin; ++j) {
      char function[20];
      xctx->simdata[i].pin[j].function=NULL;
      xctx->simdata[i].pin[j].go_to=NULL;
      my_snprintf(function, S(function), "function%d", j);
      my_strdup(_ALLOC_ID_, &xctx->simdata[i].pin[j].function, get_tok_value(symbol->prop_ptr, function, 0));
      my_strdup(_ALLOC_ID_, &xctx->simdata[i].pin[j].go_to, 
                get_tok_value(symbol->rect[PINLAYER][j].prop_ptr, "goto", 0));
      str = get_tok_value(symbol->rect[PINLAYER][j].prop_ptr, "clock", 0);
      xctx->simdata[i].pin[j].clock = str[0] ? str[0] - '0' : -1;
    }
  }
}

void free_simdata(void)
{
  int i, j;

  if(xctx->simdata) {
    for(i = 0; i < xctx->simdata_ninst; ++i) { /* can not use xctx->instances if a new sch is loaded */
      int npin = xctx->simdata[i].npin;
      for(j = 0; j < npin; ++j) {
        my_free(_ALLOC_ID_, &xctx->simdata[i].pin[j].function);
        my_free(_ALLOC_ID_, &xctx->simdata[i].pin[j].go_to);
      }
      if(npin) my_free(_ALLOC_ID_, &xctx->simdata[i].pin);
    }
    my_free(_ALLOC_ID_, &xctx->simdata);
  }
  xctx->simdata_ninst = 0;
}

static void propagate_logic()
{
  /* char *propagated_net=NULL; */
  int found, iter = 0 /* , mult */;
  int i, j, npin;
  int propagate;
  Hilight_hashentry  *entry;
  int val, newval;
  static const int map[] = {LOGIC_0, LOGIC_1, LOGIC_X, LOGIC_Z, LOGIC_NOUP};

  prepare_netlist_structs(0);
  if(!xctx->simdata) create_simdata();

  for(i=0; i<xctx->instances; ++i)
    for(j=0;j < xctx->simdata[i].npin; ++j)
      xctx->simdata[i].pin[j].value=-10000;
  
  while(1) {
    dbg(1, "propagate_logic(): main loop iteration\n");
    found=0;
    for(i=0; i<xctx->instances; ++i) {
      npin = xctx->simdata[i].npin;
      for(j=0; j<npin; ++j) {
        if(xctx->simdata && xctx->simdata[i].pin && xctx->simdata[i].pin[j].go_to) {
          int n = 1;
          const char *propag;
          int clock_pin, clock_val, clock_oldval;
          clock_pin = xctx->simdata[i].pin[j].clock;
          if(clock_pin != -1) {
            /* no bus_hilight_lookup --> no bus expansion */
            entry = NULL;
            if(xctx->inst[i].node && xctx->inst[i].node[j]) {
              entry = hilight_hash_lookup(xctx->inst[i].node[j], 0, XLOOKUP); /* clock pin */
            }
            clock_val =  (!entry) ? LOGIC_X : entry->value;
            clock_oldval =  (!entry) ? LOGIC_X : entry->oldvalue;
            if(entry) {
                if(clock_pin == 0) { /* clock falling edge */
                  if( clock_val == clock_oldval) continue;
                  if( clock_val != LOGIC_0) continue;
                  if(entry && (entry->time < xctx->hilight_time)) continue;
                } else if(clock_pin == 1) { /* clock rising edge */
                  if( clock_val == clock_oldval) continue;
                  if( clock_val != LOGIC_1) continue;
                  if(entry && (entry->time < xctx->hilight_time)) continue;
                } else if(clock_pin == 2) { /* set/clear active low */
                  if( clock_val != LOGIC_0) continue;
                } else if(clock_pin == 3) { /* set/clear active high */
                  if( clock_val != LOGIC_1) continue;
                }
            }
          }
          dbg(1, "propagate_logic(): inst=%d pin %d, goto=%s\n", i,j, xctx->simdata[i].pin[j].go_to);
          while(1) {
            propag = find_nth(xctx->simdata[i].pin[j].go_to, ",", "", 0, n);
            ++n;
            if(!propag[0]) break;
            propagate = atoi(propag);
            if(propagate < 0 || propagate >= npin) {
               dbg(1, "Error: inst: %s, pin %d, goto set to %s <<%d>>\n",
                 xctx->inst[i].instname, j, xctx->simdata[i].pin[j].go_to, propagate);
                 continue;
            }
            if(!xctx->inst[i].node[propagate]) {
              dbg(1, "Error: inst %s, output in %d unconnected\n", xctx->inst[i].instname, propagate);
              break;
            }
            /* get net to propagate hilight to...*/
            /* fast option: dont use net_name() (no expandlabel though) */
            /* THIS MUST BE DONE TO HANDLE VECTOR INSTANCES/BUSES */
            /* my_strdup(xxx, &propagated_net, net_name(i, propagate, &mult, 1, 0)); */
            /* dbg(1, "propagate_logic(): inst %d pin %d propag=%s n=%d\n", i, j, propag, n);
             * dbg(1, "propagate_logic(): inst %d pin %d propagate=%d\n", i, j, propagate);
             * dbg(1, "propagate_logic(): propagated_net=%s\n", propagated_net); */
            /* add net to highlight list */
            /* no bus_hilight_lookup --> no bus expansion */
            newval = eval_logic_expr(i, propagate);
            val =  map[newval];

            if(newval != 4 && xctx->simdata[i].pin[propagate].value != val ) {
              found=1; /* keep looping until no more nets are found. */
              xctx->simdata[i].pin[propagate].value = val;
              dbg(1, "propagate_logic(): DRIVERS inst %s pin %d, net %s --> value %d\n", 
                  xctx->inst[i].instname, j, xctx->inst[i].node[propagate], val);
            } 
          } /* while( ith-goto )  */
        } /* if((xctx->simdata && xctx->simdata[i].pin && xctx->simdata[i].pin[j].go_to) */
      } /* for(j...) */
    } /* for(i...) */

    xctx->hilight_time++;

    /* update all values */
    for(i=0; i<xctx->instances; ++i) {
      for(j=0;j < xctx->simdata[i].npin; ++j) {
        if(xctx->simdata[i].pin[j].value != -10000) {
          if(!xctx->inst[i].node || !xctx->inst[i].node[j]) continue;
          entry = hilight_hash_lookup(xctx->inst[i].node[j], 0, XLOOKUP); 
          if(!entry || xctx->hilight_time != entry->time) {
            hilight_hash_lookup(xctx->inst[i].node[j], xctx->simdata[i].pin[j].value, XINSERT);
            dbg(1, "propagate_logic(): UPDATE1 inst %s pin %d, net %s --> value %d\n",
                xctx->inst[i].instname, j, xctx->inst[i].node[j], xctx->simdata[i].pin[j].value);
          } else if(entry->value != xctx->simdata[i].pin[j].value &&
                     xctx->simdata[i].pin[j].value != LOGIC_Z) {
            hilight_hash_lookup(xctx->inst[i].node[j], xctx->simdata[i].pin[j].value, XINSERT);
            dbg(1, "propagate_logic(): UPDATE2 inst %s pin %d, net %s --> value %d\n",
                xctx->inst[i].instname, j, xctx->inst[i].node[j], xctx->simdata[i].pin[j].value);
          } else {
            dbg(1, "propagate_logic(): UPDATE3 inst %s pin %d, net %s --> value %d NOT assigned\n",
                xctx->inst[i].instname, j, xctx->inst[i].node[j], xctx->simdata[i].pin[j].value);
          }
        }
      }
    }
    if(!found) break;
    /* get out from infinite loops (circuit is oscillating) */
    tclvareval("update; expr {$::tclstop == 1}", NULL);
    if( tclresult()[0] == '1') break;
    ++iter;
  } /* while(1) */
  /* my_free(_ALLOC_ID_, &propagated_net); */
}

void logic_set(int value, int num, const char *net_name)
{
  int i, j, n, newval;
  char *type;
  xRect boundbox;
  int big =  xctx->wires> 2000 || xctx->instances > 2000 ;
  static const int map[] = {LOGIC_0, LOGIC_1, LOGIC_X, LOGIC_Z, LOGIC_NOUP};
  Hilight_hashentry  *entry;
 
  tclsetvar("tclstop", "0");
  prepare_netlist_structs(0);
  if(!xctx->simdata) create_simdata();
  rebuild_selected_array();
  newval = value;
  if(!xctx->no_draw && !big) {
    calc_drawing_bbox(&boundbox, 2);
    bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
    bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
  }
  for(j = 0; j < num; ++j) {
    if(net_name) {
      if(value == -1) { /* toggle */
        entry = bus_hilight_hash_lookup(net_name, 0, XLOOKUP);
        if(entry)
          newval = (entry->value == LOGIC_1) ? 0 : (entry->value == LOGIC_0) ? 1 : 2;
        else newval = 2;
      }
      bus_hilight_hash_lookup(net_name, map[newval], XINSERT);
    } else for(i=0;i<xctx->lastsel; ++i)
    {
      char *node = NULL;
      n = xctx->sel_array[i].n;
      switch(xctx->sel_array[i].type) {
        case WIRE:
          node = xctx->wire[n].node;
          break;
        case ELEMENT:
          type = (xctx->inst[n].ptr+ xctx->sym)->type;
          if( type && xctx->inst[n].node && IS_LABEL_SH_OR_PIN(type) ) { /* instance must have a pin! */
            node = xctx->inst[n].node[0];
          }
          break;
        default:
          break;
      }
      if(node) {
        if(value == -1) { /* toggle */
          entry = bus_hilight_hash_lookup(node, 0, XLOOKUP);
          if(entry)
            newval = (entry->value == LOGIC_1) ? 0 : (entry->value == LOGIC_0) ? 1 : 2;
          else newval = 2;
        }
        bus_hilight_hash_lookup(node, map[newval], XINSERT);
      }
    }
    propagate_logic();
    propagate_hilights(1, 0, XINSERT);
  }
  if(!xctx->no_draw && !big) {
    calc_drawing_bbox(&boundbox, 2);
    bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
  }
  draw();
  if(!xctx->no_draw && !big) bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  tcleval("if { [info exists gaw_fd] } {close $gaw_fd; unset gaw_fd}\n");
}


void hilight_net(int viewer)
{
  int i, n;
  char *type;
  int sim_is_xyce;
  int incr_hi;
  char *s = NULL;
  incr_hi = tclgetboolvar("incr_hilight");
  prepare_netlist_structs(0);
  dbg(1, "hilight_net(): entering\n");
  rebuild_selected_array();
  tcleval("sim_is_xyce");
  sim_is_xyce = atoi( tclresult() );
  for(i=0;i<xctx->lastsel; ++i) {
   n = xctx->sel_array[i].n;
   switch(xctx->sel_array[i].type) {
    case WIRE:
         /* sets xctx->hilight_nets=1 */
     if(!xctx->wire[n].node) break;
     dbg(1, "hilight_net(): wire[n].node=%s, incr_hi=%d\n", xctx->wire[n].node, incr_hi);
     if(!bus_hilight_hash_lookup(xctx->wire[n].node, xctx->hilight_color, XINSERT_NOREPLACE)) {
       if(viewer == XSCHEM_GRAPH) {
         send_net_to_graph(&s, sim_is_xyce, xctx->wire[n].node);
         dbg(1, "1 hilight_net(): send_net_to_graph() sets s=%s\n", s);
         dbg(1, "hilight_net(): wire[n].node=%s\n", xctx->wire[n].node);
       } else if(viewer == GAW) send_net_to_gaw(sim_is_xyce, xctx->wire[n].node);
       else if(viewer == BESPICE) send_net_to_bespice(sim_is_xyce, xctx->wire[n].node);
     }
     if(xctx->some_nets_added && incr_hi) {
       incr_hilight_color();
     }
     break;
    case ELEMENT:
     type = (xctx->inst[n].ptr+ xctx->sym)->type;
     if( type && xctx->inst[n].node && IS_LABEL_SH_OR_PIN(type) ) { /* instance must have a pin! */
           /* sets xctx->hilight_nets=1 */
       dbg(1, "hilight_net(): node[0]=%s, incr_hi=%d\n", xctx->inst[n].node[0], incr_hi);
       if(!bus_hilight_hash_lookup(xctx->inst[n].node[0], xctx->hilight_color, XINSERT_NOREPLACE)) {
         if(viewer == XSCHEM_GRAPH) {
           send_net_to_graph(&s, sim_is_xyce, xctx->inst[n].node[0]);
           dbg(1, "2 hilight_net(): send_net_to_graph() sets s=%s\n", s);
           dbg(1, "hilight_net(): inst[n].node[0]=%s\n", xctx->inst[n].node[0]);
         }
         else if(viewer == GAW) send_net_to_gaw(sim_is_xyce, xctx->inst[n].node[0]);
         else if(viewer == BESPICE) send_net_to_bespice(sim_is_xyce, xctx->inst[n].node[0]);
       }
       if(xctx->some_nets_added && incr_hi) {
         incr_hilight_color();
       }
     } else {
       dbg(1, "hilight_net(): setting hilight flag on inst %d\n",n);
       /* xctx->hilight_nets=1; */  /* done in hilight_hash_lookup() */
       xctx->inst[n].color = xctx->hilight_color;
       inst_hilight_hash_lookup(n, xctx->hilight_color, XINSERT_NOREPLACE);
       if(type &&  (!strcmp(type, "ammeter") || !strcmp(type, "vsource")) ) {
         if(viewer == XSCHEM_GRAPH) send_current_to_graph(&s, sim_is_xyce, xctx->inst[n].instname);
         else if(viewer == GAW) send_current_to_gaw(sim_is_xyce, xctx->inst[n].instname);
         else if(viewer == BESPICE) send_current_to_bespice(sim_is_xyce, xctx->inst[n].instname);
       }
       if(incr_hi) incr_hilight_color();
     }
     break;
    default:
     break;
   }
  }
  if( viewer == XSCHEM_GRAPH && s) {
    tclvareval("graph_add_nodes_from_list {", s, "}", NULL);
    my_free(_ALLOC_ID_, &s);
  }
  if(!incr_hi) incr_hilight_color();
  if(xctx->hilight_nets) propagate_hilights(1, 0, XINSERT_NOREPLACE);
  tcleval("if { [info exists gaw_fd] } {close $gaw_fd; unset gaw_fd}\n");
}

void unhilight_net(void)
{
  int i,n;
  char *type;

  rebuild_selected_array();
  prepare_netlist_structs(0);
  dbg(1, "unhilight_net(): entering\n");
  for(i=0;i<xctx->lastsel; ++i) {
   n = xctx->sel_array[i].n;
   switch(xctx->sel_array[i].type) {
    case WIRE:
      bus_hilight_hash_lookup(xctx->wire[n].node, xctx->hilight_color, XDELETE);
     break;
    case ELEMENT:
     type = (xctx->inst[n].ptr+ xctx->sym)->type;
     if( type) {
       if( xctx->inst[n].node && IS_LABEL_SH_OR_PIN(type) ) { /* instance must have a pin! */
         bus_hilight_hash_lookup(xctx->inst[n].node[0], xctx->hilight_color, XDELETE);
       } else {
         inst_hilight_hash_lookup(n, xctx->hilight_color, XDELETE);
       }
     }
     xctx->inst[n].color = -10000;
     break;
    default:
     break;
   }
  }
  propagate_hilights(0, 1, XINSERT_NOREPLACE); /* will also clear xctx->hilight_nets if nothing left hilighted */
  draw();

  unselect_all(1);
}

/* redraws the whole affected rectangle, this avoids artifacts due to antialiased text */
void redraw_hilights(int clear)
{
  if(!has_x) return;
  if(clear) clear_all_hilights();
  draw();
}


void select_hilight_net(void)
{
 char *type=NULL;
 int i;
 Hilight_hashentry *entry;
 int hilight_connected_inst;
 int en_hi;

 if(!xctx->hilight_nets) return;
 en_hi = tclgetboolvar("en_hilight_conn_inst");
 prepare_netlist_structs(0);
 for(i=0;i<xctx->wires; ++i) {
   if( (entry = bus_hilight_hash_lookup(xctx->wire[i].node, 0, XLOOKUP)) ) {
      xctx->wire[i].sel = SELECTED;
      set_first_sel(WIRE, i, 0);
   }
 }
 for(i=0;i<xctx->instances; ++i) {

  type = (xctx->inst[i].ptr+ xctx->sym)->type;
  hilight_connected_inst = en_hi && 
    ((xctx->inst[i].flags & HILIGHT_CONN) || ((xctx->inst[i].ptr+ xctx->sym)->flags & HILIGHT_CONN));
  if( xctx->inst[i].color != -10000) {
    dbg(1, "select_hilight_net(): instance %d flags & HILIGHT_CONN true\n", i);
     xctx->inst[i].sel = SELECTED;
     set_first_sel(ELEMENT, i, 0);
  }
  else if(hilight_connected_inst) {
    int rects, j;
    if( (rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER]) > 0 ) {
      for(j=0;j<rects; ++j) {
        if( xctx->inst[i].node && xctx->inst[i].node[j]) {
          entry=bus_hilight_hash_lookup(xctx->inst[i].node[j], 0, XLOOKUP);
          if(entry) {
            xctx->inst[i].sel = SELECTED;
            set_first_sel(ELEMENT, i, 0);
            break;
          }
        }
      }
    }
  } else if( type && xctx->inst[i].node && IS_LABEL_SH_OR_PIN(type) ) {
    entry=bus_hilight_hash_lookup(xctx->inst[i].node[0], 0, XLOOKUP);
    if(entry) {
      xctx->inst[i].sel = SELECTED;
      set_first_sel(ELEMENT, i, 0);
    }
  }
 }
 xctx->need_reb_sel_arr = 1;
 rebuild_selected_array(); /* sets or clears xctx->ui_state SELECTION flag */
 redraw_hilights(0);
 
}


/* returns the full path name of "net" recursively resolving port connections
 * propagating lower level nets to upper levels.
 * "net" can be a bussed net.
 * caller *MUST* free returned string */
char *resolved_net(const char *net)
{
  char *rnet = NULL;
  Str_hashentry *entry;

  /* global node ? return as is */
  if(net && record_global_node(3, NULL, net)) {
    my_strdup(_ALLOC_ID_, &rnet, net);
    return rnet;
  }
  if(net) {
    char *n_s1, *n_s2;
    int k, mult, skip = 0;
    char *exp_net = NULL;
    char *resolved_net = NULL;
    int level = xctx->currsch;
    int start_level;
    char *path = xctx->sch_path[level] + 1, *path2 = NULL, *path2_ptr = NULL;

    dbg(1, "resolved_net(): net=%s\n", net);
    start_level = sch_waves_loaded();
    if(start_level == -1) start_level = 0;
    if(net[0] == '#') net++;
    if(path) {
      /* skip path components that are above the level where raw file was loaded */
      while(*path && skip < start_level) {
        if(*path == '.') skip++;
        ++path;
      }
    }
    dbg(1, "path=%s\n", path);
    my_strdup2(_ALLOC_ID_, &exp_net, expandlabel(net, &mult));
    n_s1 = exp_net;
    for(k = 0; k < mult; k++) {
      char *net_name = my_strtok_r(n_s1, ",", "", 0, &n_s2);
      level = xctx->currsch;
      n_s1 = NULL;
      my_strdup2(_ALLOC_ID_, &resolved_net, net_name);
      dbg(1, "resolved_net(): resolved_net=%s\n", resolved_net);
      while(level > start_level) { /* check if net passed by attribute instead of by port */
        const char *ptr = get_tok_value(xctx->hier_attr[level - 1].prop_ptr, resolved_net, 0);
        if(ptr && ptr[0]) {
          my_strdup2(_ALLOC_ID_, &resolved_net, ptr);
          dbg(1, "lcc[%d].prop_ptr=%s\n", level - 1, xctx->hier_attr[level - 1].prop_ptr);
          dbg(1, "resolved_net(): resolved_net=%s\n", resolved_net);
        } else {
          break;
        }
        level--;
      }
      while(level > start_level) { /* get net from parent nets attached to port if resolved_net is a port */
        entry = str_hash_lookup(&xctx->portmap[level], resolved_net, NULL, XLOOKUP);
        if(entry) {
          my_strdup2(_ALLOC_ID_, &resolved_net, entry->value);
          dbg(1, "resolved_net(): while loop: resolved_net=%s\n", resolved_net);
        }
        else break;
        level--;
      }
      my_strdup2(_ALLOC_ID_, &path2, path);
      skip = start_level;
      path2_ptr = path2;
      if(level == start_level) path2_ptr[0] = '\0';
      else while(*path2_ptr) {
        if(*path2_ptr == '.') skip++;
        if(skip == level) {
          *(path2_ptr +1) = '\0';
          break;
        }
        path2_ptr++;
      }
      dbg(1, "path2=%s level=%d start_level=%d\n", path2, level, start_level);

      if(record_global_node(3, NULL, resolved_net)) {
        my_strdup2(_ALLOC_ID_, &rnet, resolved_net);
      } else {
        my_mstrcat(_ALLOC_ID_, &rnet, path2, resolved_net, NULL);
      }
      if(k < mult - 1) my_strcat(_ALLOC_ID_, &rnet, ",");
    }
    if(resolved_net) my_free(_ALLOC_ID_, &resolved_net);
    my_free(_ALLOC_ID_, &path2);
    my_free(_ALLOC_ID_, &exp_net);
  }
  dbg(1, "resolved_net(): got %s, return %s\n", net, rnet);
  return rnet;
}

void draw_hilight_net(int on_window)
{
 int save_draw;
 int i,c;
 double x1,y1,x2,y2;
 xSymbol *symptr;
 int use_hash;
 Wireentry *wireptr;
 Instentry *instanceptr;
 Hilight_hashentry *entry;
 Iterator_ctx ctx;

 if(!xctx->hilight_nets) return;
 dbg(3, "draw_hilight_net(): xctx->prep_hi_structs=%d\n", xctx->prep_hi_structs);
 prepare_netlist_structs(0);
 save_draw = xctx->draw_window;
 xctx->draw_window = on_window;
 x1 = X_TO_XSCHEM(xctx->areax1);
 y1 = Y_TO_XSCHEM(xctx->areay1);
 x2 = X_TO_XSCHEM(xctx->areax2);
 y2 = Y_TO_XSCHEM(xctx->areay2);
 use_hash = (xctx->wires> 2000 || xctx->instances > 2000 ) &&  (x2 - x1  < ITERATOR_THRESHOLD);
 if(use_hash) {
   hash_wires();
   hash_instances();
 }
 if(use_hash) init_wire_iterator(&ctx, x1, y1, x2, y2);
 else i = -1;
 while(1) {
   if(use_hash) {
     if( !(wireptr = wire_iterator_next(&ctx))) break;
     i = wireptr->n;
   }
   else {
     ++i;
     if(i >= xctx->wires) break;
   }
   if( (entry = bus_hilight_hash_lookup(xctx->wire[i].node, 0, XLOOKUP)) ) {
     if(xctx->wire[i].bus)
       drawline(get_color(entry->value), THICK,
          xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2, 0, NULL);
     else
       drawline(get_color(entry->value), NOW,
          xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2, 0, NULL);
     if(xctx->cadhalfdotsize*xctx->mooz>=0.7) {
       if( xctx->wire[i].end1 >1 ) {
         filledarc(get_color(entry->value), NOW, xctx->wire[i].x1, xctx->wire[i].y1,
            xctx->cadhalfdotsize, 0, 360);
       }
       if( xctx->wire[i].end2 >1 ) {
         filledarc(get_color(entry->value), NOW, xctx->wire[i].x2, xctx->wire[i].y2,
            xctx->cadhalfdotsize, 0, 360);
       }
     }
   }
 }
 for(c=0;c<cadlayers; ++c) {
   if(xctx->draw_single_layer!=-1 && c != xctx->draw_single_layer) continue;
   if(use_hash) init_inst_iterator(&ctx, x1, y1, x2, y2);
   else i = -1;
   while(1) {
     if(use_hash) {
       if( !(instanceptr = inst_iterator_next(&ctx))) break;
       i = instanceptr->n;
     }
     else {
       ++i;
       if(i >= xctx->instances) break;
     }
     if(xctx->inst[i].color != -10000)
     {
      int col = get_color(xctx->inst[i].color);
      symptr = (xctx->inst[i].ptr+ xctx->sym);
      if( c==0 || /*draw_symbol call is needed on layer 0 to avoid redundant work (outside check) */
          symptr->lines[c] || symptr->rects[c] || symptr->arcs[c] || symptr->polygons[c] ||
          ((c == cadlayers - 1) && symptr->texts)) {
        draw_symbol(ADD, col, i, c, 0, 0, 0.0, 0.0);
        if(c == cadlayers - 1) draw_symbol(ADD, col, i, c + 1, 0, 0, 0.0, 0.0); /* draw texts */
      }
      filledrect(col, END, 0.0, 0.0, 0.0, 0.0, 2, -1, -1); /* last parameter must be 2! */
      drawarc(col, END, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0);
      drawrect(col, END, 0.0, 0.0, 0.0, 0.0, 0, -1, -1);
      drawline(col, END, 0.0, 0.0, 0.0, 0.0, 0, NULL);
     }
   }
 }
 xctx->draw_window = save_draw;
}

/* show == 0   ==> create pins from highlight nets */
/* show == 1   ==> print list of highlight net */
/* show == 2   ==> create labels with i prefix from hilight nets */
/* show == 3   ==> print list of highlight net with path and label expansion  */
/* show == 4   ==> create labels without i prefix from hilight nets */
void print_hilight_net(int show)
{
 int i;
 FILE *fd;
 Hilight_hashentry *entry;
 Node_hashentry *node_entry;
 char cmd[2*PATH_MAX];  /* 20161122 overflow safe */
 char cmd2[2*PATH_MAX];  /* 20161122 overflow safe */
 char cmd3[2*PATH_MAX];  /* 20161122 overflow safe */
 char *a = "create_pins";
 char *b = "add_lab_prefix";
 char *b1 = "add_lab_no_prefix";
 char *filetmp1 = NULL;
 char *filetmp2 = NULL;
 char *filename_ptr;

 prepare_netlist_structs(1); /* use full prepare_netlist_structs(1)  to recognize pin direction */
                             /* when creating pins from hilight nets 20171221 */
 if(!(fd = open_tmpfile("hilight2_", "", &filename_ptr)) ) {
   fprintf(errfp, "print_hilight_net(): can not create tmpfile %s\n", filename_ptr);
   return;
 }
 my_strdup(_ALLOC_ID_, &filetmp2, filename_ptr);
 fclose(fd);
 if(!(fd = open_tmpfile("hilight1_", "", &filename_ptr))) {
   fprintf(errfp, "print_hilight_net(): can not create tmpfile %s\n", filename_ptr);
   my_free(_ALLOC_ID_, &filetmp2);
   return;
 }
 my_strdup(_ALLOC_ID_, &filetmp1, filename_ptr);
 my_snprintf(cmd, S(cmd), "awk -f \"%s/order_labels.awk\"", tclgetvar("XSCHEM_SHAREDIR"));
 my_snprintf(cmd2, S(cmd2), "%s %s > %s", cmd, filetmp1, filetmp2);
 my_snprintf(cmd3, S(cmd3), "awk -f \"%s/sort_labels.awk\" %s", tclgetvar("XSCHEM_SHAREDIR"), filetmp1);
 for(i=0;i<HASHSIZE; ++i) {
   entry=xctx->hilight_table[i];
   while(entry) {
     dbg(1, "print_hilight_net(): (hilight_hashentry *)entry->token=%s\n", entry->token);
     node_entry = bus_node_hash_lookup(entry->token, "", XLOOKUP, 0, "", "", "", "");
     /* 20170926 test for not null node_entry, this may happen if a hilighted net name has been changed */
     /* before invoking this function, in this case --> skip */
     if(node_entry && !strcmp(xctx->sch_path[xctx->currsch], entry->path)) {
       if(show==3) {
         if(xctx->netlist_type == CAD_SPICE_NETLIST) 
           fprintf(fd, ".save v(%s%s)\n", 
              entry->path + 1, 
              entry->token[0] == '#' ? entry->token + 1 : entry->token  );
         else
           fprintf(fd, "%s%s\n", 
              entry->path + 1, 
              entry->token[0] == '#' ? entry->token + 1 : entry->token  );
       } else if(show==1) {
         fprintf(fd, "%s\n",  entry->token);
       } else {
         if(node_entry->d.out==0 && node_entry->d.inout==0 )
           fprintf(fd, "%s   %s\n",  entry->token, "ipin");
         else if(node_entry->d.in==0 && node_entry->d.inout==0 )
           fprintf(fd, "%s   %s\n",  entry->token, "opin");
         else
           fprintf(fd, "%s   %s\n",  entry->token, "iopin");
       }
     }
     entry = entry ->next ;
   }
 }
 fclose(fd);
 if(show != 3) {
   tclsetvar("filetmp",filetmp2);
   if(system(cmd2)==-1) { /* order_labels.awk filetmp1 > filetmp2 */
     fprintf(errfp, "print_hilight_net(): error executing cmd2\n");
   }
   if(show==2) { /* create labels from hilight pins with 'i' prefix */
     tcleval(b); /* add_lab_prefix */
   }
   if(show==4) { /* create labels from hilight pins without 'i' prefix */
     tcleval(b1); /* add_lab_no_prefix */
   }
   if(show==1) {
     my_snprintf(cmd, S(cmd), "set ::retval [ read_data_nonewline %s ]", filetmp2);
     tcleval(cmd);
     tcleval("viewdata $::retval");
   }
 } else { /* show == 3 */
   tclsetvar("filetmp",filetmp1);
   if(system(cmd3)==-1) {  /* sort_labels.awk filetmp1 (writes changes into filetmp1) */
     fprintf(errfp, "print_hilight_net(): error executing cmd3\n");
   }
   my_snprintf(cmd, S(cmd), "set ::retval [ read_data_nonewline %s ]", filetmp1);
   tcleval(cmd);
   tcleval("viewdata $::retval");
 }
 if(show==0)  {
   tcleval(a); /* create_pins */
 }
 if(debug_var == 0 ) {
   xunlink(filetmp2);
   xunlink(filetmp1);
 }
 /* 20170323 this delete_netlist_structs is necessary, without it segfaults when going back (ctrl-e)  */
 /* from a schematic after placing pins (ctrl-j) and changing some pin direction (ipin -->iopin) */
 xctx->prep_hi_structs=0;
 xctx->prep_net_structs=0;

 my_free(_ALLOC_ID_, &filetmp1);
 my_free(_ALLOC_ID_, &filetmp2);
}

void list_hilights(int all)
{
 int i, first = 1;
 Hilight_hashentry *entry;
 Node_hashentry *node_entry;

 Tcl_ResetResult(interp);
 prepare_netlist_structs(1); /* use full prepare_netlist_structs(1)  to recognize pin direction */
                             /* when creating pins from hilight nets 20171221 */
 if(all) {
   for(i=0;i<HASHSIZE; ++i) {
     entry=xctx->hilight_table[i];
     for( entry=xctx->hilight_table[i]; entry; entry = entry->next) {
       if(all == 1 &&  entry->token[0] == ' ') continue;
       if(all == 2 &&  entry->token[0] != ' ') continue;
       Tcl_AppendResult(interp,  entry->path, "  ",
          entry->token, "  ", my_itoa(entry->value), "\n", NULL);
     }
   }
 } else 
 for(i=0;i<HASHSIZE; ++i) {
   entry=xctx->hilight_table[i];
   while(entry) {
     node_entry = bus_node_hash_lookup(entry->token, "", XLOOKUP, 0, "", "", "", "");
     if(node_entry  && (node_entry->d.port == 0 || !strcmp(xctx->sch_path[xctx->currsch], ".") )) {
       if(!first) Tcl_AppendResult(interp, " ", NULL);
       Tcl_AppendResult(interp,  entry->path + 1,
          entry->token[0] == '#' ? entry->token + 1 : entry->token, NULL);
       first = 0;
     }
     entry = entry ->next;
   }
 }
}
