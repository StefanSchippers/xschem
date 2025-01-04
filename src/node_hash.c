/* File: node_hash.c
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
static Node_hashentry *node_hash_lookup(const char *token, const char *dir,int what,int port,
       char *sig_type, char *verilog_type, char *value, char *class, const char *orig_tok)
/*    token        dir et all      what           ... action ...
 * --------------------------------------------------------------------------
 * "whatever"     "in"/"out"    0,XINSERT insert in hash table if not in and return NULL
 *                                        if already present just return entry address
 *                                        and update in/out fields sum up port field
 *                                        return NULL otherwise
 *
 * "whatever"     whatever      2,XDELETE  delete entry if found return NULL
 * "whatever"     whatever      1,XLOOKUP  only look up element, dont insert */
{
 unsigned int hashcode, index;
 Node_hashentry **preventry;
 Drivers d;

 if(token==NULL || token[0]==0 ) return NULL;
 dbg(3, "node_hash_lookup(): called with: %s dir=%s what=%d port=%d\n",
        token, dir, what, port);
 d.in=d.out=d.inout=0;
 if(dir) {
   if(!strcmp(dir,"in") )  d.in=1;
   else if(!strcmp(dir,"out") ) d.out=1;
   else if(!strcmp(dir,"inout") ) d.inout=1;
 }
 d.port=port;
 hashcode=str_hash(token);
 index=hashcode % HASHSIZE;
 preventry=&xctx->node_table[index];
 while(1)
 {
  if( !(*preventry) )                  /* empty slot */
  {
   if( what==XINSERT )          /* insert data */
   {
    Node_hashentry *entry = (Node_hashentry *)my_malloc(_ALLOC_ID_, sizeof( Node_hashentry ));
    entry->next = NULL;
    entry->token = entry->sig_type = entry->verilog_type =
                   entry->value = entry->class = entry->orig_tok = NULL;
    my_strdup(_ALLOC_ID_, &(entry->token),token);
    if(sig_type &&sig_type[0]) my_strdup(_ALLOC_ID_,  &(entry->sig_type), sig_type);
    if(verilog_type &&verilog_type[0]) my_strdup(_ALLOC_ID_,  &(entry->verilog_type), verilog_type);
    if(class && class[0]) my_strdup(_ALLOC_ID_,  &(entry->class), class);
    if(orig_tok && orig_tok[0]) my_strdup(_ALLOC_ID_,  &(entry->orig_tok), orig_tok);
    if(value && value[0]) my_strdup(_ALLOC_ID_,  &(entry->value), value);
    entry->d.port=d.port;
    entry->d.in=d.in;
    entry->d.out=d.out;
    entry->d.inout=d.inout;
    entry->hash=hashcode;
    *preventry=entry;
    dbg(3, "node_hash_lookup(): hashing %s : value=%s\n\n",
           entry->token, entry->value? entry->value:"<NULL>");
    dbg(3, "node_hash_lookup(): hashing %s in=%d out=%d inout=%d port=%d\n",
                token, d.in, d.out, d.inout, d.port);
   }
   return NULL; /* whether inserted or not return NULL since it was not in */
  }
  if( (*preventry) -> hash==hashcode && strcmp(token,(*preventry)->token)==0 ) /* found matching tok */
  {
   if(what==XDELETE)                /* remove token from the hash table ... */
   {
    Node_hashentry *saveptr=(*preventry)->next, *entry = *preventry;
    my_free(_ALLOC_ID_, &entry->token);
    my_free(_ALLOC_ID_, &entry->verilog_type);
    my_free(_ALLOC_ID_, &entry->sig_type);
    my_free(_ALLOC_ID_, &entry->class);
    my_free(_ALLOC_ID_, &entry->orig_tok);
    my_free(_ALLOC_ID_, &entry->value);
    my_free(_ALLOC_ID_, &entry);
    *preventry=saveptr;
    return NULL;
   }
   else /* found matching entry, return the address and update in/out count */
   {
    Node_hashentry *entry = *preventry;
    entry->d.port+=port;
    entry->d.in+=d.in;
    entry->d.out+=d.out;
    entry->d.inout+=d.inout;
    if(sig_type && sig_type[0] !='\0')
      my_strdup(_ALLOC_ID_,  &(entry->sig_type), sig_type);
    if(verilog_type && verilog_type[0] !='\0')
      my_strdup(_ALLOC_ID_,  &(entry->verilog_type), verilog_type);
    if(value && value[0] !='\0')
      my_strdup(_ALLOC_ID_,  &(entry->value), value);
    dbg(3, "node_hash_lookup(): hashing %s : value=%s\n\n",
           entry->token, entry->value? entry->value:"<NULL>");
    return entry;
   }
  }
  preventry=&(*preventry)->next; /* descend into the list. */
 }
}

/* wrapper to node_hash_lookup that handles buses */
/* warning, in case of buses return only pointer to first bus element */
Node_hashentry *bus_node_hash_lookup(const char *token, const char *dir, int what, int port,
       char *sig_type,char *verilog_type, char *value, char *class)
{
 char *start, *string_ptr, c;
 int mult;
 char *string=NULL;
 Node_hashentry *ptr1=NULL, *ptr2=NULL;

 if(token==NULL || token[0] == 0) return NULL;
 if( token[0] == '#')
 {
   my_strdup(_ALLOC_ID_, &string, token);
 }
 else
 {
   dbg(3, "bus_node_hash_lookup(): expanding node: %s\n", token);
   my_strdup(_ALLOC_ID_, &string, expandlabel(token,&mult));
   dbg(3, "bus_node_hash_lookup(): done expanding node: %s\n", token);
 }
 if(string==NULL) return NULL;
 string_ptr = start = string;
 while(1)
 {
  c=(*string_ptr);
  if(c==','|| c=='\0')
  {
    *string_ptr='\0';  /* set end string at comma position.... */
    /* insert one bus element at a time in hash table */
    ptr1=node_hash_lookup(start, dir, what,port, sig_type, verilog_type, value, class, token);
    if(!ptr2) ptr2=ptr1;
    dbg(3, "bus_node_hash_lookup(): processing node: %s\n", start);
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

static void node_hash_free_entry(Node_hashentry *entry)
{
  Node_hashentry *tmp;

  while(entry) {
    tmp = entry->next;
    my_free(_ALLOC_ID_, &entry->token);
    my_free(_ALLOC_ID_, &entry->verilog_type);
    my_free(_ALLOC_ID_, &entry->sig_type);
    my_free(_ALLOC_ID_, &entry->class);
    my_free(_ALLOC_ID_, &entry->orig_tok);
    my_free(_ALLOC_ID_, &entry->value);
    my_free(_ALLOC_ID_, &entry);
    entry = tmp;
  }
}

void node_hash_free(void) /* remove the whole hash table  */
{
 int i;

 dbg(2, "node_hash_free(): removing hash table\n");
 for(i=0;i<HASHSIZE; ++i)
 {
  node_hash_free_entry( xctx->node_table[i] );
  xctx->node_table[i] = NULL;
 }
}

int traverse_node_hash()
{
 int i;
 int err = 0;
 Node_hashentry *entry;
 char str[2048]; /* 20161122 overflow safe */
 int incr_hi;
 
 incr_hi = tclgetboolvar("incr_hilight");
 for(i=0;i<HASHSIZE; ++i)
 {
  entry = xctx->node_table[i];
  while(entry)
  {
   if( !record_global_node(3, NULL, entry->token)) {
     if(entry->d.out ==0  && entry->d.inout == 0)
     {
       my_snprintf(str, S(str), "Error: undriven node: %s", entry->token);
       if(!xctx->netlist_count) bus_hilight_hash_lookup(entry->token, xctx->hilight_color, XINSERT_NOREPLACE);
       if(incr_hi) incr_hilight_color();
       statusmsg(str, 2);
       err |= 1;
     }
     else if(entry->d.out + entry->d.inout + entry->d.in == 1)
     {
       my_snprintf(str, S(str), "Warning: open net: %s", entry->token);
       if(!xctx->netlist_count) bus_hilight_hash_lookup(entry->token, xctx->hilight_color, XINSERT_NOREPLACE);
       if(incr_hi) incr_hilight_color();
       statusmsg(str,2);
     }
     else if(entry->d.out >=2 && entry->d.port>=0)  /*  era d.port>=2   03102001 */
     {
       my_snprintf(str, S(str), "Warning: shorted output node: %s", entry->token);
       if(!xctx->netlist_count) bus_hilight_hash_lookup(entry->token, xctx->hilight_color, XINSERT_NOREPLACE);
       if(incr_hi) incr_hilight_color();
       statusmsg(str,2);
     }
     else if(entry->d.in ==0 && entry->d.inout == 0)
     {
       my_snprintf(str, S(str), "Warning: node: %s goes nowhere", entry->token);
       if(!xctx->netlist_count) bus_hilight_hash_lookup(entry->token, xctx->hilight_color, XINSERT_NOREPLACE);
       if(incr_hi) incr_hilight_color();
       statusmsg(str,2);
     }
     else if(entry->d.out >=2 && entry->d.inout == 0 && entry->d.port>=0)  /*  era d.port>=2   03102001 */
     {
       my_snprintf(str, S(str), "Warning: shorted output node: %s", entry->token);
       if(!xctx->netlist_count) bus_hilight_hash_lookup(entry->token, xctx->hilight_color, XINSERT_NOREPLACE);
       if(incr_hi) incr_hilight_color();
       statusmsg(str,2);
     }
   }
   dbg(1, "traverse_node_hash(): node: %s in=%d out=%d inout=%d port=%d\n",
        entry->token, entry->d.in, entry->d.out, entry->d.inout, entry->d.port);

   entry = entry->next;
  }
 }
 return err;
}

void print_vhdl_signals(FILE *fd)
{
  Node_hashentry *ptr;
  int i, found;
  int mult,j;
  char *class=NULL;
 
  found=0;
  for(i=0;i<HASHSIZE; ++i) {
    ptr = xctx->node_table[i];
    while(ptr) {
      if(strstr(ptr->token, ".")) {
        dbg(2, "print_vhdl_signals(): record field, skipping: %s\n", ptr->token);
        ptr = ptr->next;
        continue; /* signal is a record field, no declaration */
      }
      if(ptr->d.port == 0 ) {
        found = 1;
        if(ptr->token[0]=='#') {
          mult=get_unnamed_node(3, 0,  atoi((ptr->token)+4) );
        }
        else {
          mult=1;
        }
        dbg(2, "print_vhdl_signals(): node: %s mult: %d value=%s \n\n",
              ptr->token,mult, ptr->value?ptr->value:"<NULL>");
        if( ptr->class && ptr->class[0] )
          my_strdup(_ALLOC_ID_, &class, ptr->class);
        else
          my_strdup(_ALLOC_ID_, &class, "signal");
        if(mult>1) {
          for(j=mult-1;j>=0;j--) {
            fprintf(fd, "%s %s[%d] : ", class, ptr->token[0]=='#' ? ptr->token+1 : ptr->token,j);
            if(ptr->sig_type && ptr->sig_type[0]) {
              fprintf(fd, "%s", ptr->sig_type);
            }
            else {
             fprintf(fd, "std_logic");
            }
            if(ptr->value && ptr->value[0]) fprintf(fd, " := %s ", ptr->value);
            fprintf(fd, " ; %s\n", ptr->orig_tok);
          }
        }
        else {
          fprintf(fd, "%s %s : ", class, ptr->token[0]=='#' ? ptr->token+1 : ptr->token);
          if(ptr->sig_type && ptr->sig_type[0]) {
            fprintf(fd, "%s", ptr->sig_type);
          }
          else {
            fprintf(fd, "std_logic");
          }
          if(ptr->value && ptr->value[0]) fprintf(fd, " := %s ", ptr->value);
          fprintf(fd, " ; %s\n", ptr->orig_tok);
        }
      }
      ptr = ptr->next;
    }
  }
  if(found) fprintf(fd, "\n" );
  my_free(_ALLOC_ID_, &class);
}



void print_verilog_signals(FILE *fd)
{
  Node_hashentry *ptr;
  int i, found;
  int mult,j;
 
  dbg(2, " print_verilog_signals(): entering routine\n");
  found=0;
  for(i=0;i<HASHSIZE; ++i) {
    ptr = xctx->node_table[i];
    while(ptr) {
      if(ptr->d.port == 0 ) {
        found = 1;
        if(ptr->token[0]=='#') {
         mult=get_unnamed_node(3, 0,  atoi((ptr->token)+4) );
        }
        else {
         mult=1;
        }
        dbg(2, " print_verilog_signals(): node: %s mult: %d value=%s \n\n",
               ptr->token,mult, ptr->value?ptr->value:"<NULL>");
        if(mult>1) {
          for(j=mult-1;j>=0;j--) {
            if(ptr->verilog_type && ptr->verilog_type[0]) {
              fprintf(fd, "%s ", ptr->verilog_type);
            }
            else {
              fprintf(fd, "wire ");
            }
            fprintf(fd, "%s[%d] ", ptr->token[0]=='#' ? ptr->token+1 : ptr->token,j);
            if(ptr->value && ptr->value[0]) fprintf(fd, "= %s ", ptr->value);
            fprintf(fd, "; // %s\n", ptr->orig_tok);
          }
        }
        else {
          if(ptr->verilog_type && ptr->verilog_type[0]) {
            fprintf(fd, "%s ", ptr->verilog_type);
          } else {
            fprintf(fd, "wire ");
          }
          fprintf(fd, "%s ", ptr->token[0]=='#' ? ptr->token+1 : ptr->token);
          if(ptr->value && ptr->value[0]) fprintf(fd, "= %s ", ptr->value);
          fprintf(fd, "; // %s\n", ptr->orig_tok);
        }
      }
      ptr = ptr->next;
    }
  }
  if(found) fprintf(fd, "\n" );
}

void list_nets(char **result)
{
  Node_hashentry *ptr;
  char *type = NULL;
  int i;
  int netlist_lvs_ignore=tclgetboolvar("lvs_ignore");

  prepare_netlist_structs(1);
  for(i = 0; i < xctx->instances; i++) {
    if(skip_instance(i, 0, netlist_lvs_ignore)) continue;
    my_strdup(_ALLOC_ID_, &type,(xctx->inst[i].ptr+ xctx->sym)->type);
    if(type && xctx->inst[i].node && IS_PIN(type)) {
      /* 
       * my_mstrcat(_ALLOC_ID_, result, 
       *  "{", get_tok_value(xctx->inst[i].prop_ptr, "lab", 0), " ", type, "}\n", NULL);
       */
      my_mstrcat(_ALLOC_ID_, result, 
        "{", xctx->inst[i].lab, " ", type, "}\n", NULL);
    }
  }
  if(type) my_free(_ALLOC_ID_, &type);
  for(i=0;i<HASHSIZE; ++i) {
    ptr = xctx->node_table[i];
    while(ptr) {
      if(!ptr->d.port) {
        my_mstrcat(_ALLOC_ID_, result, 
          "{", ptr->token, " ", "net", "}\n", NULL);
      }
      ptr = ptr->next;
    }
  }
}

