/* File: hilight.c
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

static struct hilight_hashentry *hilight_table[HASHSIZE];
static int nelements=0;
static int *inst_color=NULL;

static unsigned int hash(const char *tok)
{
  unsigned int hash = 0;
  char *str;
  int c;

  str=xctx->sch_path[xctx->currsch];
  while ( (c = *tok++) )
      hash = c + (hash << 6) + (hash << 16) - hash;
  while ( (c = *str++) )
      hash = c + (hash << 6) + (hash << 16) - hash;
  return hash;
}

static struct hilight_hashentry *free_hilight_entry(struct hilight_hashentry *entry)
{
  struct hilight_hashentry *tmp;
  while(entry) {
    tmp = entry->next;
    my_free(755, &entry->token);
    my_free(756, &entry->path);
    my_free(757, &entry);
    entry = tmp;
  }
  return NULL;
}

void display_hilights(char **str)
{
  int i;
  int first = 1;
  struct hilight_hashentry *entry;
  for(i=0;i<HASHSIZE;i++) {
    entry = hilight_table[i];
    while(entry) {
      if(!first) my_strcat(93, str, ",");
      my_strcat(562, str, entry->path+1);
      my_strcat(1160, str, entry->token);
      first = 0;
      entry = entry->next;
    }
  }
}

void free_hilight_hash(void) /* remove the whole hash table  */
{
 int i;

 dbg(2, "free_hilight_hash(): removing hash table\n");
 for(i=0;i<HASHSIZE;i++)
 {
  hilight_table[i] = free_hilight_entry( hilight_table[i] );
 }
 dbg(2, "free_hilight_hash(): : nelements=%d\n", nelements);
 nelements=0;
}


int get_color(int value)
{
  int x;

  x = value%(n_active_layers);
  return active_layer[x];
}

/* print all highlight signals which are not ports (in/out/inout). */
void create_plot_cmd(int viewer)
{
  int i, c, idx, first;
  struct hilight_hashentry *entry;
  struct node_hashentry *node_entry;
  char *tok;
  char plotfile[PATH_MAX];
  char color_str[8];
  FILE *fd = NULL;
  char *str = NULL;

  my_snprintf(plotfile, S(plotfile), "%s/xplot", netlist_dir);
  if(viewer == NGSPICE) {
    if(!(fd = fopen(plotfile, "w"))) {
      fprintf(errfp, "create_plot_cmd(): error opening xplot file for writing\n");
      return;
    }
    fprintf(fd, "*ngspice plot file\n.control\n");
  }
  if(viewer == GAW) tcleval("if { ![info exists gaw_fd] } { gaw_setup_tcp }\n");
  idx = 1;
  first = 1;
  for(i=0;i<HASHSIZE;i++) /* set ngspice colors */
  {
    entry = hilight_table[i];
    while(entry) {
      tok = entry->token;
      node_entry = bus_hash_lookup(tok, "", XLOOKUP, 0, "", "", "", "");
      if(tok[0] == '#') tok++;
      if(node_entry && !strcmp(xctx->sch_path[xctx->currsch], entry->path) &&
         (node_entry->d.port == 0 || !strcmp(entry->path, ".") )) {
        c = get_color(entry->value);
        sprintf(color_str, "%02x%02x%02x", xcolor_array[c].red>>8, xcolor_array[c].green>>8, xcolor_array[c].blue>>8);
        idx++;
        if(viewer == NGSPICE) {
          if(idx > 9) {
            idx = 2;
            fprintf(fd, str);
            fprintf(fd, "\n");
            first = 1;
            my_free(758, &str);
          }
          fprintf(fd, "set color%d=rgb:%s\n", idx, color_str);
          if(first) {
            my_strcat(164, &str, "plot ");
            first = 0;
          }
          my_strcat(160, &str, "\"");
          my_strcat(161, &str, (entry->path)+1);
          my_strcat(162, &str, tok);
          my_strcat(163, &str, "\" ");
        }
        if(viewer == GAW) {
          char *t=NULL, *p=NULL;
          my_strdup(241, &t, tok);
          my_strdup2(245, &p, (entry->path)+1);
          Tcl_VarEval(interp, "puts $gaw_fd {copyvar v(", strtolower(p), strtolower(t),
                      ") p0 #", color_str, "}\nvwait gaw_fd\n", NULL);
          my_free(759, &p);
          my_free(760, &t);
        }
      }
      entry = entry->next;
    }
  }
  if(viewer == NGSPICE) {
    fprintf(fd, str);
    fprintf(fd, "\n.endc\n");
    my_free(761, &str);
    fclose(fd);
  }
  if(viewer == GAW) {
     tcleval("vwait gaw_fd; close $gaw_fd; unset gaw_fd\n");
  }
}

struct hilight_hashentry *hilight_lookup(const char *token, int value, int remove)
/*    token           remove    ... what ... */
/* -------------------------------------------------------------------------- */
/* "whatever"         0,XINSERT insert in hash table if not in and return NULL */
/*                             if already present just return entry address  */
/*                             return NULL otherwise */
/* */
/* "whatever"         1,XDELETE delete entry if found return NULL */
/* "whatever"         2,XLOOKUP only look up element, dont insert */
{
 unsigned int hashcode, index;
 struct hilight_hashentry *entry, *saveptr, **preventry;
 char *ptr;
 int s ;
 int depth=0;

 if(token==NULL) return NULL;
 hashcode=hash(token);
 index=hashcode % HASHSIZE;
 entry=hilight_table[index];
 preventry=&hilight_table[index];
 depth=0;
 while(1)
 {
  if( !entry )                  /* empty slot */
  {
   if( remove==XINSERT )              /* insert data */
   {
    s=sizeof( struct hilight_hashentry );
    ptr= my_malloc(137, s );
    entry=(struct hilight_hashentry *)ptr;
    entry->next = NULL;
    entry->token = NULL;
    my_strdup(138, &(entry->token),token);
    entry->path = NULL;
    my_strdup(139, &(entry->path),xctx->sch_path[xctx->currsch]);
    entry->value=value;
    entry->hash=hashcode;
    *preventry=entry;
    hilight_nets=1; /* some nets should be hilighted ....  07122002 */
    nelements++;
   }
   return NULL; /* whether inserted or not return NULL since it was not in */
  }
  if( entry -> hash==hashcode && !strcmp(token,entry->token) &&
      !strcmp(xctx->sch_path[xctx->currsch], entry->path)  ) /* found matching tok */
  {
   if(remove==XDELETE)                /* remove token from the hash table ... */
   {
    saveptr=entry->next;
    my_free(762, &entry->token);
    my_free(763, &entry->path);
    my_free(764, &entry);
    *preventry=saveptr;
    return NULL;
   }
   else /* found matching entry, return the address */
   {
    return entry;
   }
  }
  preventry=&entry->next; /* descend into the list. */
  entry = entry->next;
  depth++;
  if(debug_var>=2)
    if(depth>200)
      fprintf(errfp, "hilight_lookup(): deep into the list: %d, index=%d, token=%s, hashcode=%d\n",
              depth, index, token, hashcode);
 }
}

/* warning, in case of buses return only pointer to first bus element */
struct hilight_hashentry *bus_hilight_lookup(const char *token, int value, int remove)
{
 char *start, *string_ptr, c;
 char *string=NULL;
 struct hilight_hashentry *ptr1=NULL, *ptr2=NULL;
 int mult;

 if(token==NULL) return NULL;
 if( token[0] == '#') {
   my_strdup(140, &string, token);
 }
 else {
   my_strdup(141, &string, expandlabel(token,&mult));
 }

 if(string==NULL) {
   return NULL;
 }
 string_ptr = start = string;
 while(1) {
  c=(*string_ptr);
  if(c==','|| c=='\0')
  {
    *string_ptr='\0';  /* set end string at comma position.... */
    /* insert one bus element at a time in hash table */
    dbg(2, "bus_hilight_lookup: inserting: %s, value:%d\n", start,value);
    ptr1=hilight_lookup(start, value, remove);
    if(ptr1 && !ptr2) {
      ptr2=ptr1; /*return first non null entry */
      if(remove==2) break; /* 20161221 no need to go any further if only looking up element */
    }
    *string_ptr=c;     /* ....restore original char */
    start=string_ptr+1;
  }
  if(c==0) break;
  string_ptr++;
 }
 /* if something found return first pointer */
 my_free(765, &string);
 return ptr2;
}

void delete_hilight_net(void)
{
 int i;
 if(!hilight_nets) return;
 free_hilight_hash();
 if(event_reporting) {
   printf("xschem clear_hilights\n");
   fflush(stdout);
 }

 hilight_nets=0;
 for(i=0;i<xctx->instances;i++) {
   xctx->inst[i].flags &= ~4 ;
 }
 dbg(1, "delete_hilight_net(): clearing\n");
 my_free(766, &inst_color);
 hilight_color=0;
}

void hilight_parent_pins(void)
{
 int rects, i, j, k;
 struct hilight_hashentry *entry;
 const char *pin_name;
 char *pin_node = NULL;
 char *net_node=NULL;
 int mult, net_mult, inst_number;

 if(!hilight_nets) return;
 prepare_netlist_structs(0);
 i=xctx->previous_instance[xctx->currsch];
 inst_number = xctx->sch_inst_number[xctx->currsch+1];
 dbg(1, "hilight_parent_pins(): previous_instance=%d\n", xctx->previous_instance[xctx->currsch]);
 dbg(1, "hilight_parent_pins(): inst_number=%d\n", inst_number);

 rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];

 for(j=0;j<rects;j++)
 {
  if(!xctx->inst[i].node[j]) continue;
  my_strdup(445, &net_node, expandlabel(xctx->inst[i].node[j], &net_mult));
  dbg(1, "hilight_parent_pins(): net_node=%s\n", net_node);
  pin_name = get_tok_value((xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][j].prop_ptr,"name",0);
  if(!pin_name[0]) continue;
  my_strdup(450, &pin_node, expandlabel(pin_name, &mult));
  dbg(1, "hilight_parent_pins(): pin_node=%s\n", pin_node);

  for(k = 1; k<=mult; k++) {
    xctx->currsch++;
    entry = bus_hilight_lookup(find_nth(pin_node, ',', k), 0, XLOOKUP);
    xctx->currsch--;
    if(entry)
    {
      bus_hilight_lookup(find_nth(net_node, ',',
          ((inst_number - 1) * mult + k - 1) % net_mult + 1), entry->value, XINSERT);
    }
    else
    {
      bus_hilight_lookup(find_nth(net_node, ',',
          ((inst_number - 1) * mult + k - 1) % net_mult + 1), 0, XDELETE);
    }
   }
 }
 my_free(767, &pin_node);
 my_free(768, &net_node);
}

void hilight_child_pins(void)
{
 int j, k, rects;
 const char *pin_name;
 char *pin_node = NULL;
 char *net_node=NULL;
 struct hilight_hashentry *entry;
 int mult, net_mult, i, inst_number;

 i = xctx->previous_instance[xctx->currsch-1];
 if(!hilight_nets) return;
 prepare_netlist_structs(0);
 rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];
 inst_number = xctx->sch_inst_number[xctx->currsch];
 for(j=0;j<rects;j++)
 {
  dbg(1, "inst_number=%d\n", inst_number);

  if(!xctx->inst[i].node[j]) continue;
  my_strdup(508, &net_node, expandlabel(xctx->inst[i].node[j], &net_mult));
  dbg(1, "hilight_child_pins(): net_node=%s\n", net_node);
  pin_name = get_tok_value((xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][j].prop_ptr,"name",0);
  if(!pin_name[0]) continue;
  my_strdup(521, &pin_node, expandlabel(pin_name, &mult));
  dbg(1, "hilight_child_pins(): pin_node=%s\n", pin_node);
  for(k = 1; k<=mult; k++) {
    dbg(1, "hilight_child_pins():looking nth net:%d, k=%d, inst_number=%d, mult=%d\n",
                               (inst_number-1)*mult+k, k, inst_number, mult);
    dbg(1, "hilight_child_pins():looking net:%s\n",  find_nth(net_node, ',',
        ((inst_number - 1) * mult + k - 1) % net_mult + 1));
    xctx->currsch--;
    entry = bus_hilight_lookup(find_nth(net_node, ',', ((inst_number - 1) * mult + k - 1) % net_mult + 1), 0, XLOOKUP);
    xctx->currsch++;
    if(entry) {
      bus_hilight_lookup(find_nth(pin_node, ',', k), entry->value, XINSERT);
      dbg(1, "hilight_child_pins(): inserting: %s\n", find_nth(pin_node, ',', k));
    }
    else {
      bus_hilight_lookup(find_nth(pin_node, ',', k), 0, XDELETE);
      dbg(1, "hilight_child_pins(): deleting: %s\n", find_nth(pin_node, ',', k));
    }
  } /* for(k..) */
 }
 my_free(769, &pin_node);
 my_free(770, &net_node);
}


int bus_search(const char*s)
{
 int c, bus=0;
 while( (c=*s++) ) {
   if(c=='[')  bus=1;
   if( (c==':') || (c==',') ) {bus=0; break;}
 }
 return bus;
}

int search(const char *tok, const char *val, int sub, int sel, int what)
{
 int save_draw, hilight_layer = 7;
 int i,c, col = 7,tmp,bus=0;
 const char *str;
 char *type;
 int has_token;
 const char empty_string[] = "";
 char *tmpname=NULL;
 int found = 0;
#ifdef __unix__
 regex_t re;
#endif

 if(!val) {
   fprintf(errfp, "search(): warning: null val key\n");
   return TCL_ERROR;
 }
 save_draw = draw_window;
 draw_window=1;
#ifdef __unix__
 if(regcomp(&re, val , REG_EXTENDED)) return TCL_ERROR;
#endif
 dbg(1, "search():val=%s\n", val);
 if(what==ADD || what==NOW) {

    if(!sel) {
      col=hilight_color;
      hilight_layer = get_color(col);
      if(incr_hilight) hilight_color++;
    }
    has_token = 0;
    prepare_netlist_structs(0);
    bus=bus_search(val);
    for(i=0;i<xctx->instances;i++) {
      if(!strcmp(tok,"cell::name")) {
        has_token = (xctx->inst[i].name != NULL) && xctx->inst[i].name[0];
        str = xctx->inst[i].name;
      } else if(!strncmp(tok,"cell::", 6)) { /* cell::xxx looks for xxx in global symbol attributes */
        my_strdup(142, &tmpname,get_tok_value((xctx->inst[i].ptr+ xctx->sym)->prop_ptr,tok+6,0));
        has_token = get_tok_size;
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
        has_token = get_tok_size;
      }
      dbg(1, "search(): inst=%d, tok=%s, val=%s \n", i,tok, str);

      if(bus && sub) {
       dbg(1, "search(): doing substr search on bus sig:%s inst=%d tok=%s val=%s\n", str,i,tok,val);
       str=expandlabel(str,&tmp);
      }
      if(str && has_token) {
#ifdef __unix__
        if( (!regexec(&re, str,0 , NULL, 0) && !sub) ||           /* 20071120 regex instead of strcmp */
            (!strcmp(str, val) && sub && !bus) || (strstr(str,val) && sub && bus))
#else
        if ((!strcmp(str, val) && sub && !bus) || (strstr(str,val) && sub && bus))
#endif
        {
          if(!sel) {
            type = (xctx->inst[i].ptr+ xctx->sym)->type;
            if( type && IS_LABEL_SH_OR_PIN(type) ) {
              if(!bus_hilight_lookup(xctx->inst[i].node[0], col, XINSERT)) hilight_nets = 1;
              if(what==NOW) for(c=0;c<cadlayers;c++)
                draw_symbol(NOW, hilight_layer, i,c,0,0,0.0,0.0);
            }
            else {
              dbg(1, "search(): setting hilight flag on inst %d\n",i);
              hilight_nets=1;
              xctx->inst[i].flags |= 4;
              if(what==NOW) for(c=0;c<cadlayers;c++)
                draw_symbol(NOW, hilight_layer, i,c,0,0,0.0,0.0);
            }
          }
          if(sel==1) {
            select_element(i, SELECTED, 1, 0);
            ui_state|=SELECTION;
          }

          if(sel==-1) { /* 20171211 unselect */
            select_element(i, 0, 1, 0);
         }
         found  = 1;
        }
      }

    }
    for(i=0;i<xctx->wires;i++) {
      str = get_tok_value(xctx->wire[i].prop_ptr, tok,0);
      if(get_tok_size ) {
#ifdef __unix__
        if(   (!regexec(&re, str,0 , NULL, 0) && !sub )  ||       /* 20071120 regex instead of strcmp */
              ( !strcmp(str, val) &&  sub ) )
#else
        if (!strcmp(str, val) && sub)
#endif
        {
            dbg(2, "search(): wire=%d, tok=%s, val=%s \n", i,tok, xctx->wire[i].node);
            if(!sel) {
              bus_hilight_lookup(xctx->wire[i].node, col, XINSERT);
              if(what == NOW) {
                if(xctx->wire[i].bus)
                  drawline(hilight_layer, THICK,
                     xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2, 0);
                else
                  drawline(hilight_layer, NOW,
                     xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2, 0);
                if(cadhalfdotsize*xctx->mooz>=0.7) {
                  if( xctx->wire[i].end1 >1 ) {
                    filledarc(hilight_layer, NOW, xctx->wire[i].x1, xctx->wire[i].y1, cadhalfdotsize, 0, 360);
                  }
                  if( xctx->wire[i].end2 >1 ) {
                    filledarc(hilight_layer, NOW, xctx->wire[i].x2, xctx->wire[i].y2, cadhalfdotsize, 0, 360);
                  }
                }
              }
            }
            if(sel==1) {
              select_wire(i,SELECTED, 1);
              ui_state|=SELECTION;
            }
            if(sel==-1) {
              select_wire(i,0, 1);
            }
            found = 1;
        }
        else {
          dbg(2, "search():  not found wire=%d, tok=%s, val=%s search=%s\n", i,tok, str,val);
        }
      }
    }
    if(sel) for(c = 0; c < cadlayers; c++) for(i=0;i<xctx->lines[c];i++) {
      str = get_tok_value(xctx->line[c][i].prop_ptr, tok,0);
      if(get_tok_size) {
#ifdef __unix__
        if( (!regexec(&re, str,0 , NULL, 0) && !sub ) ||
            ( !strcmp(str, val) &&  sub ))
#else
        if ((!strcmp(str, val) && sub))
#endif
        {
            if(sel==1) {
              select_line(c, i,SELECTED, 1);
              ui_state|=SELECTION;
            }
            if(sel==-1) {
              select_line(c, i,0, 1);
            }
            found = 1;
        }
        else {
          dbg(2, "search(): not found line=%d col=%d, tok=%s, val=%s search=%s\n",
                              i, c, tok, str, val);
        }
      }
    }
    if(sel) for(c = 0; c < cadlayers; c++) for(i=0;i<xctx->rects[c];i++) {
      str = get_tok_value(xctx->rect[c][i].prop_ptr, tok,0);
      if(get_tok_size) {
#ifdef __unix__
        if( (!regexec(&re, str,0 , NULL, 0) && !sub ) ||
            ( !strcmp(str, val) &&  sub ))
#else
        if ((!strcmp(str, val) && sub))
#endif
        {
            if(sel==1) {
              select_box(c, i,SELECTED, 1);
              ui_state|=SELECTION;
            }
            if(sel==-1) {
              select_box(c, i,0, 1);
            }
            found = 1;
        }
        else {
          dbg(2, "search(): not found rect=%d col=%d, tok=%s, val=%s search=%s\n",
                              i, c, tok, str, val);
        }
      }
    }
 }
 else if(what==END) {
   redraw_hilights(); /* draw_hilight_net(1); */
   found = 1; /* no error flagging */
 }
 if(sel) {
   drawtemparc(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
   drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
   drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
 }
#ifdef __unix__
 regfree(&re);
#endif
 draw_window = save_draw;

 if(event_reporting) {
   printf("xschem search %s %d %s %s\n", (sub ? "exact" : "regex"), sel, tok, val);
   fflush(stdout);
 }
 my_free(771, &tmpname);
 if(found) return 1; else return 0;
}




/* "drill" option (pass through resistors or pass gates or whatever elements with  */
/* 'propagate_to' properties set on pins) */
void drill_hilight(void)
{
  char *netname=NULL, *propagated_net=NULL;
  int mult=0;
  int found;
  xSymbol *symbol;
  xRect *rct;
  int i, j, npin;
  const char *propagate_str;
  int propagate;
  struct hilight_hashentry *entry, *propag_entry;
  int count;

  prepare_netlist_structs(0);
  count=0;
  while(1) {
    found=0;
    count++;
    for(i=0; i<xctx->instances;i++) {
      symbol = xctx->inst[i].ptr+xctx->sym;
      npin = symbol->rects[PINLAYER];
      rct=symbol->rect[PINLAYER];
      for(j=0; j<npin;j++) {
        my_strdup(143, &netname, net_name(i, j, &mult, 1));
        propagate_str=get_tok_value(rct[j].prop_ptr, "propagate_to", 0);
        if(propagate_str[0] && (entry=bus_hilight_lookup(netname, 0, XLOOKUP))) {
          propagate = atoi(propagate_str);
          my_strdup(144, &propagated_net, net_name(i, propagate, &mult, 1)); /* get net to propagate hilight to...*/
          propag_entry = bus_hilight_lookup(propagated_net, entry->value, XINSERT); /* add net to highlight list */
          if(!propag_entry) {
            found=1; /* keep looping until no more nets are found. */
          }

        }
      } /* for(j...) */
    } /* for(i...) */
    if(!found) break;
  } /* while(1) */
  my_free(772, &netname);
  my_free(773, &propagated_net);
}

int hilight_netname(const char *name)
{
  int ret = 0;
  struct node_hashentry *node_entry;
  prepare_netlist_structs(0);
  dbg(1, "hilight_netname(): entering\n");
  rebuild_selected_array();
  node_entry = bus_hash_lookup(name, "", XLOOKUP, 0, "", "", "", "");
  ret = node_entry ? 1 : 0;
  if(ret && !bus_hilight_lookup(name, hilight_color, XINSERT)) {
    hilight_nets=1;
    if(incr_hilight) hilight_color++;
  }
  redraw_hilights();
  return ret;
}

static void send_net_to_gaw(int simtype, const char *node)
{
  int c, k, tok_mult;
  struct node_hashentry *node_entry;
  const char *expanded_tok;
  const char *tok;
  char color_str[8];

  if(!node || !node[0]) return;
  tok = node;
  node_entry = bus_hash_lookup(tok, "", XLOOKUP, 0, "", "", "", "");
  if(tok[0] == '#') tok++;
  if(node_entry  && (node_entry->d.port == 0 || !strcmp(xctx->sch_path[xctx->currsch], ".") )) {
    char *t=NULL, *p=NULL;
    c = get_color(hilight_color);
    sprintf(color_str, "%02x%02x%02x", xcolor_array[c].red>>8, xcolor_array[c].green>>8, xcolor_array[c].blue>>8);
    expanded_tok = expandlabel(tok, &tok_mult);
    tcleval("if { ![info exists gaw_fd] } { gaw_setup_tcp }\n");
    for(k=1; k<=tok_mult; k++) {
      my_strdup(246, &t, find_nth(expanded_tok, ',', k));
      my_strdup2(254, &p, xctx->sch_path[xctx->currsch]+1);
      if(simtype == 0 ) { /* spice */
        Tcl_VarEval(interp, "puts $gaw_fd {copyvar v(", strtolower(p), strtolower(t),
                    ") p0 #", color_str, "}\nvwait gaw_fd\n", NULL);
      } else { /* Xyce */
        char *c=p;
        while(*c){
          if(*c == '.') *c = ':'; /* Xyce uses : as path separator */
          c++;
        }
        Tcl_VarEval(interp, "puts $gaw_fd {copyvar ", strtoupper(p), strtoupper(t),
                    " p0 #", color_str, "}\nvwait gaw_fd\n", NULL);
      }
    }
    my_free(774, &p);
    my_free(775, &t);
  }
}

static void send_current_to_gaw(int simtype, const char *node)
{
  int c, k, tok_mult;
  const char *expanded_tok;
  const char *tok;
  char color_str[8];
  char *t=NULL, *p=NULL;

  if(!node || !node[0]) return;
  tok = node;
  /* c = get_color(hilight_color); */
  c = PINLAYER;
  sprintf(color_str, "%02x%02x%02x", xcolor_array[c].red>>8, xcolor_array[c].green>>8, xcolor_array[c].blue>>8);
  expanded_tok = expandlabel(tok, &tok_mult);
  tcleval("if { ![info exists gaw_fd] } { gaw_setup_tcp }\n");
  for(k=1; k<=tok_mult; k++) {
    my_strdup(1179, &t, find_nth(expanded_tok, ',', k));
    my_strdup2(1180, &p, xctx->sch_path[xctx->currsch]+1);
    if(!simtype) { /* spice */
      Tcl_VarEval(interp, "puts $gaw_fd {copyvar i(", xctx->currsch>0 ? "v." : "",
                  strtolower(p), strtolower(t),
                  ") p0 #", color_str, "}\nvwait gaw_fd\n", NULL);
    } else {       /* Xyce */
      char *c=p;
      while(*c){
        if(*c == '.') *c = ':'; /* Xyce uses : as path separator */
        c++;
      }
      Tcl_VarEval(interp, "puts $gaw_fd {copyvar ", xctx->currsch>0 ? "V:" : "",
                  strtoupper(p), strtoupper( xctx->currsch>0 ? t+1 : t ), "#branch",
                  " p0 #", color_str, "}\nvwait gaw_fd\n", NULL);

    }
  }
  my_free(1181, &p);
  my_free(1182, &t);

}


void hilight_net(int to_waveform)
{
  int i, n;
  char *type;
  int sim_is_xyce;

  prepare_netlist_structs(0);
  dbg(1, "hilight_net(): entering\n");
  rebuild_selected_array();
  tcleval("sim_is_xyce");
  sim_is_xyce = atoi( tclresult() );
  for(i=0;i<lastselected;i++)
  {
   n = selectedgroup[i].n;
   switch(selectedgroup[i].type)
   {
    case WIRE:
      if(event_reporting) {
        char s[PATH_MAX];
        printf("xschem search exact %d lab %s\n", 0, escape_chars(s, xctx->wire[n].node, PATH_MAX));
        fflush(stdout);
      }
      hilight_nets=1;
      if(!bus_hilight_lookup(xctx->wire[n].node, hilight_color, XINSERT)) {
        if(to_waveform == GAW) send_net_to_gaw(sim_is_xyce, xctx->wire[n].node);
        if(incr_hilight) hilight_color++;
      }
      break;
    case ELEMENT:
     type = (xctx->inst[n].ptr+ xctx->sym)->type;
     if( type && xctx->inst[n].node && IS_LABEL_SH_OR_PIN(type) ) { /* instance must have a pin! */
       if(event_reporting) {
         char s[PATH_MAX];
         printf("xschem search exact %d lab %s\n", 0, escape_chars(s, xctx->inst[n].node[0], PATH_MAX));
         fflush(stdout);
       }
       if(!bus_hilight_lookup(xctx->inst[n].node[0], hilight_color, XINSERT)) {
         if(to_waveform == GAW) send_net_to_gaw(sim_is_xyce, xctx->inst[n].node[0]);
         hilight_nets=1;
         if(incr_hilight) hilight_color++;
       }
     } else {
       if(event_reporting) {
         char s[PATH_MAX];
         printf("xschem search exact %d name %s\n", 0, escape_chars(s, xctx->inst[n].instname, PATH_MAX));
         fflush(stdout);
       }
       dbg(1, "hilight_net(): setting hilight flag on inst %d\n",n);
       hilight_nets=1;
       xctx->inst[n].flags |= 4;
     }
     if(type &&  (!strcmp(type, "current_probe") || !strcmp(type, "vsource")) ) {
       if(to_waveform == GAW) send_current_to_gaw(sim_is_xyce, xctx->inst[n].instname);
     }
     break;
    default:
     break;
   }
  }
  if(!incr_hilight) hilight_color++;
  if(enable_drill) {
    drill_hilight();
    /*traverse_schematic(); */
  }
  tcleval("if { [info exists gaw_fd] } {close $gaw_fd; unset gaw_fd}\n");
}

void unhilight_net(void)
{
  int i,n;
  char *type;

  prepare_netlist_structs(0);
  dbg(1, "unhilight_net(): entering\n");
  rebuild_selected_array();
  for(i=0;i<lastselected;i++)
  {
   n = selectedgroup[i].n;
   switch(selectedgroup[i].type)
   {
    case WIRE:
      if(event_reporting) {
        char s[PATH_MAX];
        printf("xschem search exact %d lab %s\n", 1, escape_chars(s, xctx->wire[n].node, PATH_MAX));
        printf("xschem unhilight\n");
        fflush(stdout);
      }
      bus_hilight_lookup(xctx->wire[n].node, hilight_color, XDELETE);
     break;
    case ELEMENT:
     type = (xctx->inst[n].ptr+ xctx->sym)->type;
     if( type &&
         xctx->inst[n].node && IS_LABEL_SH_OR_PIN(type) ) {  /* instance must have a pin! */
      if(event_reporting) {
        printf("xschem unhilight\n");
        fflush(stdout);
      }
      bus_hilight_lookup(xctx->inst[n].node[0], hilight_color, XDELETE);
     } else {
      if(event_reporting) {
        char s[PATH_MAX];
        printf("xschem search exact %d name %s\n", 1, escape_chars(s, xctx->inst[n].instname, PATH_MAX));
        printf("xschem unhilight\n");
        fflush(stdout);
      }
     }
     xctx->inst[n].flags &= ~4;
     break;
    default:
     break;
   }
  }
  unselect_all();
}

void redraw_hilights(void)
{
  xRect boundbox;
  int big =  xctx->wires> 2000 || xctx->instances > 2000 ;
  if(!has_x) return;
  if(!big) {
    calc_drawing_bbox(&boundbox, 2);
    bbox(BEGIN, 0.0 , 0.0 , 0.0 , 0.0);
    bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
  }
  draw();
  if(!big) bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
}

void draw_hilight_net(int on_window)
{
 int save_draw;
 char *type=NULL;
 int i,c;
 struct hilight_hashentry *entry;
 register double x1,y1,x2,y2;
 xSymbol *symptr;
 int use_hash;
 struct wireentry *wireptr;
 int hilight_connected_inst;

 if(!hilight_nets) return;
 prepare_netlist_structs(0);
 save_draw = draw_window;
 draw_window = on_window;
 x1 = X_TO_XSCHEM(areax1);
 y1 = Y_TO_XSCHEM(areay1);
 x2 = X_TO_XSCHEM(areax2);
 y2 = Y_TO_XSCHEM(areay2);
 use_hash = (xctx->wires> 2000 || xctx->instances > 2000 ) &&  (x2 - x1  < ITERATOR_THRESHOLD);
 if(use_hash) {
   hash_wires();
 }
 if(!use_hash) for(i=0;i<xctx->wires;i++)
 {
      if( (entry = bus_hilight_lookup(xctx->wire[i].node, 0, XLOOKUP)) ) {
        if(xctx->wire[i].bus)
          drawline(get_color(entry->value), THICK,
             xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2, 0);
        else
          drawline(get_color(entry->value), NOW,
             xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2, 0);
        if(cadhalfdotsize*xctx->mooz>=0.7) {
          if( xctx->wire[i].end1 >1 ) { /* 20150331 draw_dots */
            filledarc(get_color(entry->value), NOW, xctx->wire[i].x1, xctx->wire[i].y1, cadhalfdotsize, 0, 360);
          }
          if( xctx->wire[i].end2 >1 ) { /* 20150331 draw_dots */
            filledarc(get_color(entry->value), NOW, xctx->wire[i].x2, xctx->wire[i].y2, cadhalfdotsize, 0, 360);
          }
        }
      }
 } else for(init_wire_iterator(x1, y1, x2, y2); ( wireptr = wire_iterator_next() ) ;) {
    i = wireptr->n;
      if( (entry = bus_hilight_lookup(xctx->wire[i].node, 0, XLOOKUP)) ) {
        if(xctx->wire[i].bus)
          drawline(get_color(entry->value), THICK,
             xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2, 0);
        else
          drawline(get_color(entry->value), NOW,
             xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2, 0);
        if(cadhalfdotsize*xctx->mooz>=0.7) {
          if( xctx->wire[i].end1 >1 ) { /* 20150331 draw_dots */
            filledarc(get_color(entry->value), NOW, xctx->wire[i].x1, xctx->wire[i].y1, cadhalfdotsize, 0, 360);
          }
          if( xctx->wire[i].end2 >1 ) { /* 20150331 draw_dots */
            filledarc(get_color(entry->value), NOW, xctx->wire[i].x2, xctx->wire[i].y2, cadhalfdotsize, 0, 360);
          }
        }
      }
 }
 dbg(1, "draw_hilight_net() : allocating inst_color %d bytes \n", xctx->instances*sizeof(int));
 my_realloc(145, &inst_color,xctx->instances*sizeof(int));
 for(i=0;i<xctx->instances;i++)
 {

   x1=X_TO_SCREEN(xctx->inst[i].x1);
   x2=X_TO_SCREEN(xctx->inst[i].x2);
   y1=Y_TO_SCREEN(xctx->inst[i].y1);
   y2=Y_TO_SCREEN(xctx->inst[i].y2);
   inst_color[i]=0;
   if(OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2)) continue;
   /* /20150409 */

  type = (xctx->inst[i].ptr+ xctx->sym)->type;

  hilight_connected_inst = !strcmp(get_tok_value((xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "highlight", 0), "true") ||
                           !strcmp(get_tok_value(xctx->inst[i].prop_ptr, "highlight", 0), "true");
  if( xctx->inst[i].flags & 4) {
    dbg(1, "draw_hilight_net(): instance %d flags &4 true\n", i);
    inst_color[i]=PINLAYER;
  }
  else if(hilight_connected_inst) {
    int rects, j;
    dbg(2, "draw_hilight_net(): hilight_connected_inst inst=%d, node=%s\n", i, xctx->inst[i].node[0]);
    if( (rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER]) > 0 ) {
      for(j=0;j<rects;j++) {
        if( xctx->inst[i].node && xctx->inst[i].node[j]) {
          entry=bus_hilight_lookup(xctx->inst[i].node[j], 0, XLOOKUP);
          if(entry) {
            inst_color[i]=get_color(entry->value);
            break;
          }
        }
      }
    }
  } else if( type && IS_LABEL_SH_OR_PIN(type) ) {
   entry=bus_hilight_lookup( get_tok_value(xctx->inst[i].prop_ptr,"lab",0) , 0, XLOOKUP);
   if(entry) inst_color[i]=get_color(entry->value);
  }
 }

 for(c=0;c<cadlayers;c++) {
   /* 20160414 from draw() */
   if(draw_single_layer!=-1 && c != draw_single_layer) continue;

   for(i=0;i<xctx->instances;i++)
   {
     if(inst_color[i] )
     {

      x1=X_TO_SCREEN(xctx->inst[i].x1);
      x2=X_TO_SCREEN(xctx->inst[i].x2);
      y1=Y_TO_SCREEN(xctx->inst[i].y1);
      y2=Y_TO_SCREEN(xctx->inst[i].y2);
      if(OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2)) continue;
      dbg(1, "draw_hilight_net(): instance:%d\n",i);
      /* 20160414 from draw() */
      symptr = (xctx->inst[i].ptr+ xctx->sym);
      if( c==0 || /*draw_symbol call is needed on layer 0 to avoid redundant work (outside check) */
          symptr->lines[c] ||
          symptr->rects[c] ||
          symptr->arcs[c] ||
          symptr->polygons[c] ||
          ((c==TEXTWIRELAYER || c==TEXTLAYER) && symptr->texts)) {
        draw_symbol(ADD, inst_color[i], i,c,0,0,0.0,0.0);
      }
      filledrect(inst_color[i], END, 0.0, 0.0, 0.0, 0.0);
      drawarc(inst_color[i], END, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0);
      drawrect(inst_color[i], END, 0.0, 0.0, 0.0, 0.0, 0);
      drawline(inst_color[i], END, 0.0, 0.0, 0.0, 0.0, 0);
     }
   }
 }
 draw_window = save_draw;
 /* display_hilights(); */
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
 struct hilight_hashentry *entry;
 struct node_hashentry *node_entry;
 char cmd[2*PATH_MAX];  /* 20161122 overflow safe */
 char cmd2[2*PATH_MAX];  /* 20161122 overflow safe */
 char cmd3[2*PATH_MAX];  /* 20161122 overflow safe */
 char a[] = "create_pins";
 char b[] = "add_lab_prefix";
 char b1[] = "add_lab_no_prefix";
 char *filetmp1 = NULL;
 char *filetmp2 = NULL;
 char *filename_ptr;


 /* 20111116 20111201 */
 prepare_netlist_structs(1); /* use full prepare_netlist_structs(1)  to recognize pin direction */
                             /* when creating pins from hilight nets 20171221 */


 if(!(fd = open_tmpfile("hilight_", &filename_ptr)) ) {
   fprintf(errfp, "print_hilight_net(): can not create tmpfile %s\n", filename_ptr);
   return;
 }
 my_strdup(147, &filetmp2, filename_ptr);
 fclose(fd);
 if(!(fd = open_tmpfile("hilight_", &filename_ptr))) {
   fprintf(errfp, "print_hilight_net(): can not create tmpfile %s\n", filename_ptr);
   my_free(776, &filetmp2);
   return;
 }
 my_strdup(148, &filetmp1, filename_ptr);

 if(show == 3) {
   tclsetvar("filetmp2",filetmp1);
 } else {
   tclsetvar("filetmp2",filetmp2);
 }
 tclsetvar("filetmp1",filetmp1);

 if(  filetmp1[0] == 0 || filetmp2[0] == 0 ) {
   dbg(1, "print_hilight_net(): problems creating tmpfiles\n");
   my_free(777, &filetmp1);
   my_free(778, &filetmp2);
   return;
 }
 if(fd==NULL){
   dbg(1, "print_hilight_net(): problems opening netlist file\n");
   my_free(779, &filetmp1);
   my_free(780, &filetmp2);
   return;
 }
 my_snprintf(cmd, S(cmd), "\"%s/order_labels.awk\"", tclgetvar("XSCHEM_SHAREDIR"));
 my_snprintf(cmd2, S(cmd2), "%s %s > %s", cmd, filetmp1, filetmp2);
 my_snprintf(cmd3, S(cmd3), "\"%s/sort_labels.awk\" %s", tclgetvar("XSCHEM_SHAREDIR"), filetmp1);
 for(i=0;i<HASHSIZE;i++) {
   entry=hilight_table[i];
   while(entry) {

     node_entry = bus_hash_lookup(entry->token, "", XLOOKUP, 0, "", "", "", "");

     /* 20170926 test for not null node_entry, this may happen if a hilighted net name has been changed */
     /* before invoking this function, in this case --> skip */
     if(node_entry && !strcmp(xctx->sch_path[xctx->currsch], entry->path)) {
       if(show==3) {
         if(netlist_type == CAD_SPICE_NETLIST) 
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
 if(system(cmd2)==-1) {
   fprintf(errfp, "print_hilight_net(): error executing cmd2\n");
 }
 if(show==2) {
   tcleval(b);
 }
 if(show==4) { /* 20120913 create labels from hilight pins without 'i' prefix */
   tcleval(b1);
 }
 if(show==1) {
   my_snprintf(cmd, S(cmd), "set ::retval [ read_data_nonewline %s ]", filetmp2);
   tcleval(cmd);
   tcleval("viewdata $::retval");
 }
 if(show==3) {
   if(system(cmd3)==-1) {
     fprintf(errfp, "print_hilight_net(): error executing cmd3\n");
   }

   my_snprintf(cmd, S(cmd), "set ::retval [ read_data_nonewline %s ]", filetmp1);
   tcleval(cmd);
   tcleval("viewdata $::retval");
 }
 if(show==0)  {
   tcleval(a);
 }
 xunlink(filetmp2);
 xunlink(filetmp1);

 /* 20170323 this delete_netlist_structs is necessary, without it segfaults when going back (ctrl-e)  */
 /* from a schematic after placing pins (ctrl-j) and changing some pin direction (ipin -->iopin) */
 prepared_hilight_structs=0;
 prepared_netlist_structs=0;

 /* delete_netlist_structs(); */
 my_free(781, &filetmp1);
 my_free(782, &filetmp2);
}

