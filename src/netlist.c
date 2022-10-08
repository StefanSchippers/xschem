/* File: netlist.c
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

static void instdelete(int n, int x, int y)
{
  Instentry *saveptr, **prevptr;

  prevptr = &xctx->inst_spatial_table[x][y];
  while( (*prevptr)->n != n) prevptr = &(*prevptr)->next;
  saveptr = (*prevptr)->next;
  my_free(821, prevptr);
  *prevptr = saveptr;
}

static void instinsert(int n, int x, int y)
{
  Instentry *ptr, *newptr;
  ptr=xctx->inst_spatial_table[x][y];
  newptr=my_malloc(236, sizeof(Instentry));
  newptr->next=ptr;
  newptr->n=n;
  xctx->inst_spatial_table[x][y]=newptr;
  dbg(2, "instinsert(): inserting object %d at %d,%d\n",n,x,y);
}

static Instentry *delinstentry(Instentry *t)
{
  Instentry *tmp;
  while( t ) {
    tmp = t->next;
    my_free(822, &t);
    t = tmp;
  }
  return NULL;
}

void del_inst_table(void)
{
  int i,j;

  for(i=0;i<NBOXES;i++)
    for(j=0;j<NBOXES;j++)
      xctx->inst_spatial_table[i][j] = delinstentry(xctx->inst_spatial_table[i][j]);
  xctx->prep_hash_inst=0;
  dbg(1, "del_inst_table(): cleared object hash table\n");
}

/* what:
 * 0, XINSERT : add to hash
 * 1, XDELETE : remove from hash
 */
void hash_inst(int what, int n) /* 20171203 insert object bbox in spatial hash table */
{
  int tmpi,tmpj, counti,countj,i,j;
  double tmpd;
  double x1, y1, x2, y2;
  int x1a, x2a, y1a, y2a;

  x1=xctx->inst[n].x1;
  x2=xctx->inst[n].x2;
  y1=xctx->inst[n].y1;
  y2=xctx->inst[n].y2;
  /* ordered bbox */
  if( x2 < x1) { tmpd=x2;x2=x1;x1=tmpd;}
  if( y2 < y1) { tmpd=y2;y2=y1;y1=tmpd;}

  /* calculate square 4 1st bbox point of object[k] */
  x1a=(int)floor(x1/BOXSIZE);
  y1a=(int)floor(y1/BOXSIZE);

  /* calculate square 4 2nd bbox point of object[k] */
  x2a=(int)floor(x2/BOXSIZE);
  y2a=(int)floor(y2/BOXSIZE);

  /*loop thru all squares that intersect bbox of object[k] */
  counti=0;
  for(i=x1a; i<=x2a && counti < NBOXES; i++)
  {
   counti++;
   tmpi=i%NBOXES; if(tmpi<0) tmpi+=NBOXES;
   countj=0;
   for(j=y1a; j<=y2a && countj < NBOXES; j++)
   {
    countj++;
    tmpj=j%NBOXES; if(tmpj<0) tmpj+=NBOXES;
    /* insert object_ptr[n] in region [tmpi, tmpj] */
    if(what == XINSERT) instinsert(n, tmpi, tmpj);
    else instdelete(n, tmpi, tmpj);
   }
  }
}
void hash_instances(void) /* 20171203 insert object bbox in spatial hash table */
{
 int n;

 if(xctx->prep_hash_inst) return;
 del_inst_table();
 for(n=0; n<xctx->instances; n++) {
   hash_inst(XINSERT, n);
 }
 xctx->prep_hash_inst=1;
}

static void instpindelete(int n,int pin, int x, int y)
{
  Instpinentry *saveptr, **prevptr, *ptr;

  prevptr = &xctx->instpin_spatial_table[x][y];
  ptr = *prevptr;
  while(ptr) {
    if(ptr->n == n && ptr->pin == pin) {
      saveptr = ptr->next;
      my_free(823, &ptr);
      *prevptr = saveptr;
      return;
    }
    prevptr = &ptr->next;
    ptr = *prevptr;
  }
}

/*                                      --pin coordinates--  -square coord- */
static void instpininsert(int n,int pin, double x0, double y0, int x, int y)
{
 Instpinentry *ptr, *newptr;

 ptr=xctx->instpin_spatial_table[x][y];
 newptr=my_malloc(237, sizeof(Instpinentry));
 newptr->next=ptr;
 newptr->n=n;
 newptr->x0=x0;
 newptr->y0=y0;
 newptr->pin=pin;
 xctx->instpin_spatial_table[x][y]=newptr;
 dbg(2, "instpininsert(): inserting inst %d at %d,%d\n",n,x,y);
}


static Instpinentry *delinstpinentry(Instpinentry *t)
{
  Instpinentry *tmp;

  while(t) {
    tmp = t->next;
    my_free(824, &t);
    t = tmp;
  }
  return NULL;
}

static void del_inst_pin_table(void)
{
 int i,j;

 for(i=0;i<NBOXES;i++)
  for(j=0;j<NBOXES;j++)
   xctx->instpin_spatial_table[i][j] = delinstpinentry(xctx->instpin_spatial_table[i][j]);
}


static void wiredelete(int n, int x, int y)
{
  Wireentry *saveptr, **prevptr;

  prevptr = &xctx->wire_spatial_table[x][y];
  while( (*prevptr)->n != n) prevptr = &(*prevptr)->next;
  saveptr = (*prevptr)->next;
  my_free(825, prevptr);
  *prevptr = saveptr;
}

static void wireinsert(int n, int x, int y)
{
  Wireentry *ptr, *newptr;

  ptr=xctx->wire_spatial_table[x][y];
  newptr=my_malloc(238, sizeof(Wireentry));
  newptr->next=ptr;
  newptr->n=n;
  xctx->wire_spatial_table[x][y]=newptr;
  dbg(2, "wireinsert(): inserting wire %d at %d,%d\n",n,x,y);
}

static Wireentry *delwireentry(Wireentry *t)
{
  Wireentry *tmp;

  while( t ) {
    tmp = t->next;
    my_free(826, &t);
    t = tmp;
  }
  return NULL;
}

void del_wire_table(void)
{
 int i,j;

 for(i=0;i<NBOXES;i++)
  for(j=0;j<NBOXES;j++)
   xctx->wire_spatial_table[i][j] = delwireentry(xctx->wire_spatial_table[i][j]);
 xctx->prep_hash_wires=0;
}

void get_square(double x, double y, int *xx, int *yy)
{
 int xa, xb, ya, yb;

 xa=(int)floor(x/BOXSIZE) ;
 xb=xa % NBOXES; if(xb<0) xb+=NBOXES;
 ya=(int)floor(y/BOXSIZE) ;
 yb=ya % NBOXES; if(yb<0) yb+=NBOXES;

 *xx=xb;
 *yy=yb;
}

/* what:
 * 0, XINSERT : add to hash
 * 1, XDELETE : remove from hash
 */
static void hash_inst_pin(int for_netlist, int what, int i, int j)
/*                                                    inst   pin */

{
  xRect *rct;
  char *prop_ptr;
  double x0, y0, rx1, ry1;
  short rot, flip;
  int sqx, sqy;
  int rects;

  rects=(xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER] ;
  if(j>=rects)  /* generic pins */
  {
    rct=(xctx->inst[i].ptr+ xctx->sym)->rect[GENERICLAYER];
    x0=(rct[j-rects].x1+rct[j-rects].x2)/2;
    y0=(rct[j-rects].y1+rct[j-rects].y2)/2;
    prop_ptr = rct[j-rects].prop_ptr;
  }
  else
  {
    rct=(xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER];
    x0=(rct[j].x1+rct[j].x2)/2;
    y0=(rct[j].y1+rct[j].y2)/2;
    prop_ptr = rct[j].prop_ptr;
  }


  if(for_netlist && j<rects &&
     (!prop_ptr || !get_tok_value(prop_ptr, "name",0)[0] || !get_tok_value(prop_ptr, "dir",0)[0]) ) {
    char str[2048];
    my_snprintf(str, S(str), "symbol %s: missing all or name or dir attributes on pin %d\n  %s",
        xctx->inst[i].name, j, prop_ptr);
    statusmsg(str,2);
    tcleval("show_infotext"); /* critical error: force ERC window showing */
    if(!xctx->netlist_count) {
      xctx->inst[i].color = -PINLAYER;
      xctx->hilight_nets=1;
    }
  }
  rot=xctx->inst[i].rot;
  flip=xctx->inst[i].flip;
  ROTATION(rot, flip, 0.0,0.0,x0,y0,rx1,ry1);
  x0=xctx->inst[i].x0+rx1;
  y0=xctx->inst[i].y0+ry1;
  get_square(x0, y0, &sqx, &sqy);
  if( what == XINSERT ) instpininsert(i, j, x0, y0, sqx, sqy);
  else                 instpindelete(i, j, sqx, sqy);
}

/* what:
 * 0, XINSERT : add to hash
 * 1, XDELETE : remove from hash
 */
void hash_wire(int what, int n, int incremental)
{
  int tmpi,tmpj, counti,countj,i,j;
  double tmpd;
  double x1, y1, x2, y2;
  int x1a, x2a, y1a, y2a;
  Wireentry *wptr;
  xWire * const wire = xctx->wire;

  wire[n].end1 = wire[n].end2=-1;
  x1=wire[n].x1;
  x2=wire[n].x2;
  y1=wire[n].y1;
  y2=wire[n].y2;
  /* ordered bbox */
  if( x2 < x1) { tmpd=x2;x2=x1;x1=tmpd;}
  if( y2 < y1) { tmpd=y2;y2=y1;y1=tmpd;}

  /* calculate square 4 1st bbox point of wire[k] */
  x1a=(int)floor(x1/BOXSIZE) ;
  y1a=(int)floor(y1/BOXSIZE) ;

  /* calculate square 4 2nd bbox point of wire[k] */
  x2a=(int)floor(x2/BOXSIZE);
  y2a=(int)floor(y2/BOXSIZE);

  /*loop thru all squares that intersect bbox of wire[k] */
  counti=0;
  for(i=x1a; i<=x2a && counti < NBOXES; i++)
  {
   counti++;
   tmpi=i%NBOXES; if(tmpi<0) tmpi+=NBOXES;
   countj=0;
   for(j=y1a; j<=y2a && countj < NBOXES; j++)
   {
    countj++;
    tmpj=j%NBOXES; if(tmpj<0) tmpj+=NBOXES;
    /* insert wire[n] in region [tmpi, tmpj] */
    if(what==XINSERT) wireinsert(n, tmpi, tmpj);
    else  wiredelete(n, tmpi, tmpj);

    /* reset ends of all wires that *could* touch wire[n] */
    if(incremental) for(wptr = xctx->wire_spatial_table[tmpi][tmpj] ; wptr ; wptr = wptr->next) {
      wire[wptr->n].end1 = wire[wptr->n].end2 = -1;
    }
   }
  }
}

void hash_wires(void)
{
 int n;
 if(xctx->prep_hash_wires) return;
 del_wire_table();

 for(n=0; n<xctx->wires; n++) hash_wire(XINSERT, n, 0);
 xctx->prep_hash_wires=1;
}

/* return 0 if library path of s matches any lib name in tcl variable $xschem_libs */
/* what: 1: netlist exclude lib, 2: hierarchical print exclude lib */
int check_lib(int what, const char *s)
{
 int range,i, found;
 char str[PATH_MAX + 512]; /* overflow safe 20161122 */

 found=0;
 if(what & 1) tcleval("llength $xschem_libs");
 if(what & 2) tcleval("llength $noprint_libs");
 range = atoi(tclresult());
 dbg(1, "check_lib(): s=%s, range=%d\n", s, range);

 for(i=0;i<range;i++){
  if(what & 1 ) my_snprintf(str, S(str), "lindex $xschem_libs %d",i);
  if(what & 2 ) my_snprintf(str, S(str), "lindex $noprint_libs %d",i);
  tcleval(str);
  dbg(1, "check_lib(): %s -> %s\n", str, tclresult());
  my_snprintf(str, S(str), "regexp {%s} %s", tclresult(), s);
  dbg(1, "check_lib(): str=%s\n", str);
  tcleval(str);
  if( tclresult()[0] == '1') found=1;
 }
 if(found) return 0;
 else return 1;
}

void netlist_options(int i)
{
  const char * str;
  str = get_tok_value(xctx->inst[i].prop_ptr, "bus_replacement_char", 0);
  if(str[0] && str[1] && strlen(str) ==2) {
    bus_char[0] = str[0];
    bus_char[1] = str[1];
    /* tclsetvar("bus_replacement_char", str); */
  }
  /* fprintf(errfp, "netlist_options(): bus_char=%s\n", str); */

  str = get_tok_value(xctx->inst[i].prop_ptr, "top_subckt", 0);
  if(str[0]) {
    /* fprintf(errfp, "netlist_options(): prop_ptr=%s\n", xctx->inst[i].prop_ptr); */
    if(!strcmp(str, "true")) tclsetintvar("top_subckt", 1);
    else tclsetintvar("top_subckt", 0);
  }
  str = get_tok_value(xctx->inst[i].prop_ptr, "spiceprefix", 0);
  if(str[0]) {
    /* fprintf(errfp, "netlist_options(): prop_ptr=%s\n", xctx->inst[i].prop_ptr); */
    if(!strcmp(str, "false")) tclsetvar("spiceprefix", "0");
    else tclsetvar("spiceprefix", "1");
  }

  str = get_tok_value(xctx->inst[i].prop_ptr, "hiersep", 0);
  if(str[0]) {
    my_snprintf(xctx->hiersep, S(xctx->hiersep), "%s", str);
  }
}

/* used only for debug */
#if 0
static void print_wires(void)
{
 int i,j;
 Wireentry *ptr;
 for(i=0;i<NBOXES;i++) {
   for(j=0;j<NBOXES;j++)
   {
    dbg(1, "print_wires(): %4d%4d :\n",i,j);
    ptr=xctx->wire_spatial_table[i][j];
    while(ptr)
    {
     dbg(1, "print_wires(): %6d\n", ptr->n);
     ptr=ptr->next;
    }
    dbg(1, "print_wires(): \n");
   }
 }
 ptr=xctx->wire_spatial_table[0][1];
 while(ptr)
 {
  select_wire(ptr->n,SELECTED, 1);
  rebuild_selected_array();
  ptr=ptr->next;
 }
 draw();
}
#endif

static void signal_short( char *n1, char *n2)
{
 char str[2048];
 if( n1 && n2 && strcmp( n1, n2) )
 {
   my_snprintf(str, S(str), "shorted: %s - %s", n1, n2);
   dbg(1, "signal_short(): signal_short: shorted: %s - %s", n1, n2);
   statusmsg(str,2);
   tcleval("show_infotext"); /* critical error: force ERC window showing */
   if(!xctx->netlist_count) {
      bus_hilight_hash_lookup(n1, xctx->hilight_color, XINSERT);
      if(tclgetboolvar("incr_hilight")) incr_hilight_color();
      bus_hilight_hash_lookup(n2, xctx->hilight_color, XINSERT);
      if(tclgetboolvar("incr_hilight")) incr_hilight_color();
   }
 }
}

static void wirecheck(int k)    /* recursive routine */
{
  int tmpi, tmpj, counti, countj, i, j, touches, x1a, x2a, y1a, y2a;
  double x1, y1, x2, y2;
  Wireentry *ptr2;
  xWire * const wire = xctx->wire;
 
  x1 = wire[k].x1; y1 = wire[k].y1;
  x2 = wire[k].x2; y2 = wire[k].y2;
  /* ordered bbox */
  RECTORDER(x1, y1, x2, y2);
  /* calculate square 4 1st bbox point of wire[k] */
  x1a = (int)floor(x1 / BOXSIZE);
  y1a = (int)floor(y1 / BOXSIZE);
  /* calculate square 4 2nd bbox point of wire[k] */
  x2a = (int)floor(x2 / BOXSIZE);
  y2a = (int)floor(y2 / BOXSIZE);
  /*loop thru all squares that intersect bbox of wire[k] */
  counti = 0;
  for(i = x1a; i <= x2a && counti < NBOXES; i++) {
    counti++;
    tmpi = i % NBOXES; if(tmpi < 0) tmpi += NBOXES;
    countj = 0;
    for(j = y1a; j <= y2a && countj < NBOXES; j++) {
      countj++;
      tmpj = j % NBOXES; if(tmpj < 0) tmpj += NBOXES;
      /*check if wire[k]  touches wires in square [tmpi, tmpj] */
      for(ptr2 = xctx->wire_spatial_table[tmpi][tmpj]; ptr2; ptr2 = ptr2->next) {
        if(ptr2->n == k || wire[ptr2->n].node) {continue;} /* itself or net already checked. Move on */
        touches = 
          touch(wire[k].x1,wire[k].y1,wire[k].x2,wire[k].y2, wire[ptr2->n].x1,wire[ptr2->n].y1) ||
          touch(wire[k].x1,wire[k].y1,wire[k].x2,wire[k].y2, wire[ptr2->n].x2,wire[ptr2->n].y2) ||
          touch(wire[ptr2->n].x1,wire[ptr2->n].y1,wire[ptr2->n].x2, wire[ptr2->n].y2, wire[k].x1,wire[k].y1) ||
          touch(wire[ptr2->n].x1,wire[ptr2->n].y1,wire[ptr2->n].x2, wire[ptr2->n].y2, wire[k].x2,wire[k].y2);
        if( touches ) {
          my_strdup(239, &wire[ptr2->n].node, wire[k].node);
          my_strdup(240, &wire[ptr2->n].prop_ptr,
            subst_token(wire[ptr2->n].prop_ptr, "lab", wire[ptr2->n].node));
          wirecheck(ptr2->n); /* recursive check */
        }
      }
    }
  }
}

void set_tcl_netlist_type(void)
{
    if(xctx->netlist_type == CAD_SPICE_NETLIST) {
      tclsetvar("netlist_type", "spice");
    }  else if(xctx->netlist_type == CAD_VERILOG_NETLIST) {
      tclsetvar("netlist_type", "verilog");
    } else if(xctx->netlist_type == CAD_VHDL_NETLIST) {
      tclsetvar("netlist_type", "vhdl");
    } else if(xctx->netlist_type == CAD_TEDAX_NETLIST) {
      tclsetvar("netlist_type", "tedax");
    } else if(xctx->netlist_type == CAD_SYMBOL_ATTRS) {
      tclsetvar("netlist_type", "symbol");
    } else {
      tclsetvar("netlist_type", "unknown");
    }
}

static void set_unnamed_net(int i)
{
  char tmp_str[30];
  my_snprintf(tmp_str, S(tmp_str), "#net%d", get_unnamed_node(1,0,0));
  my_strdup(265, &xctx->wire[i].node, tmp_str);
  my_strdup(266, &xctx->wire[i].prop_ptr, subst_token(xctx->wire[i].prop_ptr, "lab", tmp_str));
  /* insert unnamed wire name in hash table */
  bus_node_hash_lookup(tmp_str, "", XINSERT, 0,"","","","");
  wirecheck(i);
}

/* what==0 -> initialize  */
/* what==1 -> get new node name, net##   */
/* what==2 -> update multiplicity   */
/* what==3 -> get node multiplicity */
int get_unnamed_node(int what, int mult,int node)
{
  int i;

  dbg(2, "get_unnamed_node(): what=%d mult=%d node=%d\n", what, mult, node);
  if(what==0)  /* initialize unnamed node data structures */
  {
    xctx->new_node=0;
    my_free(828, &xctx->node_mult);
    xctx->node_mult_size=0;
    return 0;
  }
  else if(what==1) /* get a new unique unnamed node */
  {
    char tmp_str[30];
    do {
      ++xctx->new_node;
      my_snprintf(tmp_str, S(tmp_str), "net%d", xctx->new_node);
    /* JL avoid autonamed nets clash with user defined 'net#' names */
    } while (bus_node_hash_lookup(tmp_str, "", XLOOKUP, 0, "", "", "", "")!=NULL);

    while(xctx->new_node>= xctx->node_mult_size)  /* enlarge array and zero it */
    {
      xctx->node_mult_size += CADCHUNKALLOC;
      my_realloc(242, &xctx->node_mult, sizeof(int) * xctx->node_mult_size );
      for (i=xctx->node_mult_size-CADCHUNKALLOC;i<xctx->node_mult_size;i++) xctx->node_mult[i]=0;
    }
    xctx->node_mult[xctx->new_node]=mult;
    return xctx->new_node;
  }
  else if(what==2)    /* update node multiplicity if given mult is lower */
  {
    if(xctx->node_mult[node]==0) xctx->node_mult[node]=mult;
    else if(mult < xctx->node_mult[node]) xctx->node_mult[node]=mult;
    return 0;
  }
  else /* what=3 , return node multiplicity */
  {
    dbg(2, "get_unnamed_node(): returning mult=%d\n", xctx->node_mult[node]);
    return xctx->node_mult[node];
  }
}
/*------------ */

/* store list of global nodes (global=1 set in symbol props) to be printed in netlist 28032003 */
/* what: */
/*      0: print list of global nodes and delete list */
/*      1: add entry */
/*      2: delete list only, no print */
/*      3: look if node is a global */
int record_global_node(int what, FILE *fp, char *node)
{
 static int max_globals=0; /* safe to keep even with multiple schematics, netlist code always resets data */
 static int size_globals=0; /* safe to keep even with multiple schematics, netlist code always resets data */
 static char **globals=NULL; /* safe to keep even with multiple schematics, netlist code always resets data */
 int i;

 if( what==1 || what==3) {
    if(!node) return 0;
    if(!strcmp(node, "0")) return 1;
    for(i=0;i<max_globals;i++) {
      if( !strcmp(node, globals[i] )) return 1; /* node is a global */
    }
    if(what == 3) return 0; /* node is not a global */
    if(max_globals>=size_globals) {
       size_globals+=CADCHUNKALLOC;
       my_realloc(243, &globals, size_globals*sizeof(char *) );
    }
    globals[max_globals]=NULL;
    my_strdup(244, &globals[max_globals], node);
    max_globals++;
 } else if(what == 0 || what == 2) {
    for(i=0;i<max_globals;i++) {
       if(what == 0 && xctx->netlist_type == CAD_SPICE_NETLIST) fprintf(fp, ".GLOBAL %s\n", globals[i]);
       if(what == 0 && xctx->netlist_type == CAD_TEDAX_NETLIST) fprintf(fp, "__GLOBAL__ %s\n", globals[i]);
       my_free(829, &globals[i]);
    }
    my_free(830, &globals);
    size_globals=max_globals=0;
 }
 return 0;
}

void get_inst_pin_coord(int i, int j, double *x, double *y)
{
  xRect *rct;
  int rects, rot, flip;
  double x0, y0, rx1, ry1;
  xInstance * const inst = xctx->inst;
  rects = (inst[i].ptr+ xctx->sym)->rects[PINLAYER];
  if(j < rects) {
    rct=(inst[i].ptr+ xctx->sym)->rect[PINLAYER];
    x0=(rct[j].x1+rct[j].x2)/2;
    y0=(rct[j].y1+rct[j].y2)/2;
    rot=inst[i].rot;
    flip=inst[i].flip;
    ROTATION(rot, flip, 0.0,0.0,x0,y0,rx1,ry1);
    *x=inst[i].x0+rx1;
    *y=inst[i].y0+ry1;
  } else {
    *x = 0;
    *y = 0;
  }
}

static void set_inst_node(int i, int j, const char *node, int for_netlist)
{
  xInstance * const inst = xctx->inst;
  my_strdup(275,  &inst[i].node[j], node);
  if(!for_netlist) {
    bus_node_hash_lookup(inst[i].node[j],"", XINSERT, 0,"","","","");
  } else {
    bus_node_hash_lookup(inst[i].node[j],
      get_tok_value( (inst[i].ptr+ xctx->sym)->rect[PINLAYER][j].prop_ptr, "dir",0),
      XINSERT, 0,"","","","");
  }
}

static void name_non_label_inst_pins(for_netlist)
{
  char tmp_str[30]; /* overflow safe */
  double x0, y0;
  int sqx, sqy;
  int touches=0;
  int touches_unnamed=0;
  Wireentry *wptr;
  Instpinentry *iptr;
  int i,j, rects;
  char *type=NULL;
  int inst_mult, pin_mult;
  xInstance * const inst = xctx->inst;
  int const instances = xctx->instances;

  /* name instance pins  of non (label,pin) instances */
  dbg(2, "prepare_netlist_structs(): assigning node names on instance pins\n");
  for (i=0;i<instances;i++) { /* ... assign node fields on all (non label) instances */
    if(inst[i].ptr<0) continue;
    my_strdup(272, &type,(inst[i].ptr+ xctx->sym)->type);
    if(type && !IS_LABEL_OR_PIN(type) ) {
      rects = (inst[i].ptr+ xctx->sym)->rects[PINLAYER];
      expandlabel(inst[i].instname, &inst_mult);
      for (j=0;j<rects;j++) {
        touches=0;
        if(inst[i].node[j]) continue; /* already named node */
        get_inst_pin_coord(i, j, &x0, &y0);
        get_square(x0, y0, &sqx, &sqy);
        /* name instance nodes that touch named nets */
        dbg(2, "prepare_netlist_structs():           from attached nets\n");
        for(wptr=xctx->wire_spatial_table[sqx][sqy]; wptr; wptr = wptr->next) {
          if(touch(xctx->wire[wptr->n].x1, xctx->wire[wptr->n].y1,
            xctx->wire[wptr->n].x2, xctx->wire[wptr->n].y2, x0,y0)) {
            /* short circuit check */
            if(touches) {
              if(for_netlist>0) signal_short(inst[i].node[j],  xctx->wire[wptr->n].node);
            }
            if(!touches) {
              set_inst_node(i, j,  xctx->wire[wptr->n].node, for_netlist);
              if(xctx->wire[wptr->n].node[0]=='#') { /* unnamed node, update its multiplicity */
                expandlabel(get_tok_value(
                  (inst[i].ptr+ xctx->sym)->rect[PINLAYER][j].prop_ptr,"name",0),&pin_mult);
                get_unnamed_node(2, pin_mult * inst_mult, atoi((inst[i].node[j])+4) );
              }
            } /* end if(!touches) */
            touches=1;
          }
        }
        dbg(2, "prepare_netlist_structs():           from other instances\n");
        touches_unnamed=0;
        for(iptr=xctx->instpin_spatial_table[sqx][sqy]; iptr; iptr= iptr->next) {
          if(iptr->n == i ||  inst[iptr->n].ptr < 0) continue;
          if((iptr->x0==x0) && (iptr->y0==y0)) {
            if(inst[iptr->n].node && inst[iptr->n].node[iptr->pin] != NULL) {
              /* short circuit check */
              if(touches) {
                if(for_netlist>0) signal_short(inst[i].node[j],  inst[iptr->n].node[iptr->pin]);
              }
              if(!touches) {
                set_inst_node(i, j, inst[iptr->n].node[iptr->pin], for_netlist);
                if((inst[i].node[j])[0] == '#') {
                  expandlabel(get_tok_value(
                    (inst[i].ptr+ xctx->sym)->rect[PINLAYER][j].prop_ptr,"name",0),&pin_mult );
                  get_unnamed_node(2, pin_mult * inst_mult, atoi((inst[i].node[j])+4));
                }
              }
              touches=1;
            } /* end if(inst[iptr->n].node[iptr->pin] != NULL) */
            else { /* touches instance with unnamed pins */
              touches_unnamed=1;
            }
          } /* end if( (iptr->x0==x0) && (iptr->y0==y0) ) */
        }
        /*   pin did not touch named pins or nets so we name it now */
        dbg(2, "prepare_netlist_structs():           naming the other pins\n");
        if(!touches) { /* pin does not touch any other instance pin with non NULL node[] field ...*/
          if(touches_unnamed) { /* ... but touches some other instance pin, so create unnamed net */
            expandlabel(get_tok_value(
            (inst[i].ptr+ xctx->sym)->rect[PINLAYER][j].prop_ptr,"name",0), &pin_mult);
            /* done at beginning of for(i) loop 20171210 */
            /* expandlabel(get_tok_value( */
            /* inst[i].prop_ptr,"name",0), &inst_mult); */
            my_snprintf( tmp_str, S(tmp_str), "#net%d", get_unnamed_node(1, pin_mult * inst_mult, 0));
            set_inst_node(i, j, tmp_str, for_netlist);
          }
        }
      } /* for(j=0;j<rects;j++) */
    } /* if(type && !IS_LABEL_OR_PIN(type) ) */
  } /* for(i...) */
  /* END name instance pins  of non (label,pin) instances */
}

static void name_generics(int for_netlist)
{
  xRect *rct;
  double x0, y0;
  short rot = 0;
  short flip = 0;
  int sqx, sqy;
  double rx1,ry1;
  Instpinentry *iptr;
  int i,j, rects, generic_rects;
  char *type=NULL;
  char *sig_type=NULL;
  xInstance * const inst = xctx->inst;
  int const instances = xctx->instances;

  /* name generic pins from attached labels */
  dbg(2, "prepare_netlist_structs(): naming generics from attached labels\n");
  if(for_netlist) for (i=0;i<instances;i++) { /* ... assign node fields on all (non label) instances */
    if(inst[i].ptr<0) continue;
    my_strdup(267, &type,(inst[i].ptr+ xctx->sym)->type);
    if(type && !IS_LABEL_OR_PIN(type) ) {
      if((generic_rects = (inst[i].ptr+ xctx->sym)->rects[GENERICLAYER]) > 0) {
        rects = (inst[i].ptr+ xctx->sym)->rects[PINLAYER];
        for (j=rects;j<rects+generic_rects;j++) {
          if(inst[i].node[j]) continue; /* already named node */
          rct=(inst[i].ptr+ xctx->sym)->rect[GENERICLAYER];
          x0=(rct[j-rects].x1+rct[j-rects].x2)/2;
          y0=(rct[j-rects].y1+rct[j-rects].y2)/2;
          rot=inst[i].rot;
          flip=inst[i].flip;
          ROTATION(rot, flip, 0.0,0.0,x0,y0,rx1,ry1);
          x0=inst[i].x0+rx1;
          y0=inst[i].y0+ry1;
          get_square(x0, y0, &sqx, &sqy);

          for(iptr=xctx->instpin_spatial_table[sqx][sqy]; iptr; iptr = iptr ->next) {
            if(iptr->n == i) continue;
            if((iptr->x0==x0) && (iptr->y0==y0)) {
              if((inst[iptr->n].ptr+ xctx->sym)->type && inst[iptr->n].node[iptr->pin] != NULL &&
                 !strcmp((inst[iptr->n].ptr+ xctx->sym)->type, "label")) {
                dbg(2, "prepare_netlist_structs(): naming generic %s\n",
                  inst[iptr->n].node[iptr->pin]);
                my_strdup(268,  &inst[i].node[j], get_tok_value(inst[iptr->n].prop_ptr,"value",0) );
                if(!for_netlist) {
                  my_strdup(270, &sig_type,"");
                  bus_node_hash_lookup(inst[iptr->n].node[iptr->pin],"",
                    XINSERT, 1, sig_type,"", "","");
                } else {
                  my_strdup(271, &sig_type,get_tok_value(
                    (inst[i].ptr+ xctx->sym)->rect[GENERICLAYER][j-rects].prop_ptr, "sig_type",0));
                  /* insert generic label in hash table as a port so it will not */
                  /* be declared as a signal in the vhdl netlist. this is a workaround */
                  /* that should be fixed 25092001 */
                  bus_node_hash_lookup(inst[iptr->n].node[iptr->pin],
                    get_tok_value((inst[i].ptr+ xctx->sym)->rect[GENERICLAYER][j-rects].prop_ptr, "dir",0),
                    XINSERT, 1, sig_type,"", "","");
                }
              } /* end if(inst[iptr->n].node[iptr->pin] != NULL) */
            } /* end if( (iptr->x0==x0) && (iptr->y0==y0) ) */
          } /* end for(iptr=xctx->instpin_spatial_table[sqx][sqy]; iptr; iptr = iptr ->next) */
        } /* end for(j=0;j<rects;j++) */
      } /* end if( rects=...>0) */
    } /* end if(type not a label nor pin)... */
  } /* end for(i...) */
}

static void name_nodes_of_pins_labels_and_attached_nets(for_netlist)
{
  double x0, y0;
  int i, sqx, sqy;
  int port;
  Wireentry *wptr;
  char *dir=NULL;
  char *type=NULL;
  char *sig_type=NULL;
  char *verilog_type=NULL;
  char *value=NULL;
  char *class=NULL;
  char *global_node=NULL;
  int print_erc;
  xInstance * const inst = xctx->inst;
  int const instances = xctx->instances;
  static int startlevel = 0; /* safe to keep even with multiple schematic windows, netlist is not interruptable */

  if(xctx->netlist_count == 0 ) startlevel = xctx->currsch;
  dbg(2, "prepare_netlist_structs(): naming pins from attrs\n");
  /* print_erc is 1 the first time prepare_netlist_structs() is called on top level while
   * doing the netlist, when netlist of sub blocks is completed and toplevel is reloaded
   * a second prepare_netlist_structs() is called to name unnamed nets, in this second call
   * print_erc must be set to 0 to avoid double erc printing
   */
  print_erc =  (xctx->netlist_count == 0 || startlevel < xctx->currsch) && for_netlist;
  for (i=0;i<instances;i++) {
    /* name ipin opin label node fields from prop_ptr attributes */
    if(inst[i].ptr<0) continue;

    my_strdup(248, &type,(inst[i].ptr+ xctx->sym)->type);
    if(print_erc && (!inst[i].instname || !inst[i].instname[0]) &&
      !get_tok_value((inst[i].ptr+ xctx->sym)->templ, "name", 0)[0]
        ) {
      char str[2048];
      if(  type && 
           strcmp(type, "package") &&
           strcmp(type, "port_attributes") &&
           strcmp(type, "architecture") &&
           strcmp(type, "arch_declarations") &&
           strcmp(type, "attributes") &&
           strcmp(type, "netlist_options") &&
           strcmp(type, "use")) {
        my_snprintf(str, S(str), "instance: %d (%s): no name attribute set", i, inst[i].name);
        statusmsg(str,2);
        inst[i].color = -PINLAYER;
        xctx->hilight_nets=1;
      }
    }
    if(print_erc && (!type || !type[0]) ) {
      char str[2048];
      my_snprintf(str, S(str), "Symbol: %s: no type attribute set", inst[i].name);
      statusmsg(str,2);
      inst[i].color = -PINLAYER;
      xctx->hilight_nets=1;
    }
    if(type && inst[i].node && IS_LABEL_OR_PIN(type) ) { /* instance must have a pin! */
      if(for_netlist>0) {
        /* 20150918 skip labels / pins if ignore property specified on instance */
        if( xctx->netlist_type == CAD_VERILOG_NETLIST &&
          strcmp(get_tok_value(inst[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue;
        if( xctx->netlist_type == CAD_SPICE_NETLIST &&
          strcmp(get_tok_value(inst[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue;
        if( xctx->netlist_type == CAD_VHDL_NETLIST &&
          strcmp(get_tok_value(inst[i].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue;
        if( xctx->netlist_type == CAD_TEDAX_NETLIST &&
          strcmp(get_tok_value(inst[i].prop_ptr,"tedax_ignore",0),"true")==0 ) continue;
      }
      port=0;
      my_strdup2(249, &dir, "");
      if(strcmp(type,"label")) {  /* instance is a port (not a label) */
        port=1;
        /* 20071204 only define a dir property if instance is not a label */
        if(for_netlist)
          my_strdup2(250, &dir, get_tok_value( (inst[i].ptr+ xctx->sym)->rect[PINLAYER][0].prop_ptr, "dir",0));
      }
      else {
        /* handle global nodes (global=1 set as symbol property) 28032003 */
        my_strdup(251, &global_node,get_tok_value((inst[i].ptr+ xctx->sym)->prop_ptr,"global",0));
        /*20071204 if instance is a label dont define a dir property for more precise erc checking */
      }
      /* obtain ipin/opin/label signal type (default: std_logic) */
      if(for_netlist) {
        my_strdup(258, &sig_type,get_tok_value(inst[i].prop_ptr,"sig_type",0));
        my_strdup(259, &verilog_type,get_tok_value(inst[i].prop_ptr,"verilog_type",0));
        my_strdup(260, &value,get_tok_value(inst[i].prop_ptr,"value",0));
        my_strdup(261, &class,get_tok_value(inst[i].prop_ptr,"class",0));
      }
      my_strdup(262, &inst[i].node[0], inst[i].lab);
      if(!(inst[i].node[0])) {
        my_strdup(65, &inst[i].node[0], get_tok_value((inst[i].ptr+ xctx->sym)->templ, "lab",0));
        dbg(1, "prepare_netlist_structs(): no lab attr on instance, pick from symbol: %s\n", inst[i].node[0]);
      }
      /* handle global nodes (global=1 set as symbol property) 28032003 */
      if(!strcmp(type,"label") && global_node && !strcmp(global_node, "true")) {
        dbg(1, "prepare_netlist_structs(): global node: %s\n",inst[i].node[0]);
        record_global_node(1,NULL, inst[i].node[0]);
      }

      /* do not count multiple labels/pins with same name */
      bus_node_hash_lookup(inst[i].node[0],    /* insert node in hash table */
        dir, XINSERT, port, sig_type, verilog_type, value, class);

      dbg(2, "prepare_netlist_structs(): pin=%s\n",
        get_tok_value( (inst[i].ptr+ xctx->sym)->rect[PINLAYER][0].prop_ptr, "name",0));
      dbg(2, "prepare_netlist_structs(): dir=%s\n",
        get_tok_value( (inst[i].ptr+ xctx->sym)->rect[PINLAYER][0].prop_ptr, "dir",0));

      /* name nets that touch ioin opin alias instances */
      get_inst_pin_coord(i, 0, &x0, &y0);
      get_square(x0, y0, &sqx, &sqy);
      if(inst[i].node[0]) for(wptr = xctx->wire_spatial_table[sqx][sqy]; wptr; wptr = wptr->next)
      {
        if(touch(xctx->wire[wptr->n].x1, xctx->wire[wptr->n].y1,
          xctx->wire[wptr->n].x2, xctx->wire[wptr->n].y2, x0,y0))
        {
          /* short circuit check */
          if(for_netlist>0) signal_short(xctx->wire[wptr->n].node, inst[i].node[0]);
          my_strdup(263,  &xctx->wire[wptr->n].node, inst[i].node[0]);
          my_strdup(264, &xctx->wire[wptr->n].prop_ptr,
          subst_token(xctx->wire[wptr->n].prop_ptr, "lab", xctx->wire[wptr->n].node));
          wirecheck(wptr->n);
        }
      }
    } /* if(type && ... */
  } /* for(i=0;i<instances... */
  my_free(835, &dir);
  my_free(836, &type);
  my_free(841, &global_node);
  if(for_netlist) {
    my_free(837, &sig_type);
    my_free(838, &verilog_type);
    my_free(839, &value);
    my_free(840, &class);
  }
}

static void name_unlabeled_nets()
{
  int i;

  /* name nets that do not touch ipin opin alias instances */
  dbg(2, "prepare_netlist_structs(): naming nets that dont touch labels\n");
  for (i=0;i<xctx->wires;i++)
  {
    if(xctx->wire[i].node == NULL)
    {
      set_unnamed_net(i);
    }
  }
}

static void reset_node_data_and_rehash(int for_netlist)
{
  int i,j, rects;
  xInstance * const inst = xctx->inst;
  int const instances = xctx->instances;

  /* reset wire & inst node labels */
  dbg(2, "prepare_netlist_structs(): rehashing wires and instance pins in spatial hash table\n");
  hash_wires();
  for (i=0;i<instances;i++)
  {
    if(inst[i].ptr<0) continue;
    rects=(inst[i].ptr+ xctx->sym)->rects[PINLAYER] +
          (inst[i].ptr+ xctx->sym)->rects[GENERICLAYER];
    if(rects > 0)
    {
      inst[i].node = my_malloc(247, sizeof(char *) * rects);
      for (j=0;j<rects;j++)
      {
        inst[i].node[j]=NULL;
        hash_inst_pin(for_netlist, XINSERT, i, j);
      }
    }
  }
}

/* name nets that are attached to symbols with duplicated pins.
 * if another duplicated pin is attached to a named net/label/pin get name from there */
static void name_pass_through_nets(int for_netlist)
{
  int i, j, k, rects, rot, flip, sqx, sqy, changed = 0;
  double x0, y0, rx1, ry1;
  char *type = NULL, *type2 = NULL;
  xRect *rct;
  Wireentry *wptr;
  Instpinentry *iptr;
  xInstance * const inst = xctx->inst;
  int const instances = xctx->instances;
  Str_hashtable table = {NULL, 0};
  Str_hashentry *entry;
  int *symtable = NULL;
  const char *pin_name;
  int *pt_symbol = NULL; /* pass-through symbols, symbols with duplicated ports */
  int there_are_pt = 0;
  int lev = 1;
  
  dbg(1, "name_pass_through_nets() start...\n");
  pt_symbol = my_calloc(0, xctx->symbols, sizeof(int));
  symtable = my_calloc(0,  xctx->symbols, sizeof(int));
  /* we can not loop over xctx->symbols since we keep symbols of parent circuit while netlisting */
  for(i = 0; i < xctx->instances; i++) {
    k = xctx->inst[i].ptr;
    if( k < 0 || symtable[k] ) continue;
    symtable[k] =1;
    dbg(lev, "name_pass_through_nets(): inst=%s sym=%s\n", xctx->inst[i].instname, xctx->sym[k].name);
    
    str_hash_init(&table, 37);
    for(j = 0; j < xctx->sym[k].rects[PINLAYER]; j++) {
      const char *pin_name = get_tok_value(xctx->sym[k].rect[PINLAYER][j].prop_ptr, "name", 0);
      entry = str_hash_lookup(&table, pin_name, "1", XINSERT_NOREPLACE);
      if(entry) {
        dbg(lev, "   pass thru symbol found\n");
        pt_symbol[k] = 1;
        there_are_pt = 1;
        break;
      }
    }
    str_hash_free(&table);
    if(pt_symbol[k]) dbg(1, "duplicated pins: %s\n", xctx->sym[i].name);
  }
  my_free(0, &symtable);
  if(!there_are_pt) { /* nothing to do: no pass through symbols */
    my_free(0, &pt_symbol);
    return;
  }
  do { /* keep looping until propagation of nets occurs */
    dbg(lev, "name_pass_through_nets(): do loop\n");
    changed = 0;
    for (i=0;i<instances;i++) {
      dbg(1, "instance %d: %s\n", i, inst[i].instname);
      if(inst[i].ptr<0) continue;
      if(!pt_symbol[ inst[i].ptr ]) continue;
      str_hash_init(&table, 37);
      my_strdup(0, &type, (inst[i].ptr + xctx->sym)->type);
      if (type && !IS_LABEL_OR_PIN(type) ) {
        if ((rects = (inst[i].ptr+ xctx->sym)->rects[PINLAYER]) > 0) {
          /* 1st loop: hash symbol pins that are attached to named nets/pins/labels */
          for (j=0;j<rects;j++) {
            rct=(inst[i].ptr+ xctx->sym)->rect[PINLAYER];
            x0=(rct[j].x1+rct[j].x2)/2;
            y0=(rct[j].y1+rct[j].y2)/2;
            rot=inst[i].rot;
            flip=inst[i].flip;
            ROTATION(rot, flip, 0.0,0.0,x0,y0,rx1,ry1);
            x0=inst[i].x0+rx1;
            y0=inst[i].y0+ry1;
            get_square(x0, y0, &sqx, &sqy);
            pin_name = get_tok_value(rct[j].prop_ptr, "name", 0);
            /* find attached nets */
            wptr = xctx->wire_spatial_table[sqx][sqy];
            while(wptr) {
              if (touch(xctx->wire[wptr->n].x1, xctx->wire[wptr->n].y1,
                xctx->wire[wptr->n].x2, xctx->wire[wptr->n].y2, x0,y0)) {
                if(xctx->wire[wptr->n].node) {
                  dbg(1, "pin_name=%s, node=%s\n", pin_name, xctx->wire[wptr->n].node);
                  entry = str_hash_lookup(&table, pin_name, NULL, XLOOKUP);
                  if(entry) {
                    if(entry->value[0] != '#') {
                      if(xctx->wire[wptr->n].node[0] != '#') {
                        /* named net attached and another named net on duplicated pin -> check short */
                        if(for_netlist) signal_short(xctx->wire[wptr->n].node, entry->value);
                      }
                    } else { /* pin begins with '#' */
                        if(xctx->wire[wptr->n].node[0] != '#') {
                          /* named net has precedence over auto-named net */
                          str_hash_lookup(&table, pin_name, xctx->wire[wptr->n].node, XINSERT);
                        } else { /* pin and wire both begin with '#' */
                          int n, p;
                          sscanf(xctx->wire[wptr->n].node, "#net%d", &n);
                          sscanf(entry->value, "#net%d", &p);
                          if(n < p) { /* lower numbered #nets have precedence */
                            dbg(lev, "inst: %s pin %s <--- net %s\n",
                                inst[i].instname, entry->value, xctx->wire[wptr->n].node);
                            str_hash_lookup(&table, pin_name, xctx->wire[wptr->n].node, XINSERT);
                          }
                        }
                    }
                  } else {
                    /* pin unconnected. Assign from net */
                    entry = str_hash_lookup(&table, pin_name, xctx->wire[wptr->n].node, XINSERT);
                  }
                }
              }
              wptr=wptr->next;
            }
            /* find attached pins/labels */
            iptr=xctx->instpin_spatial_table[sqx][sqy];
            while (iptr) {
              if (iptr->n == i || inst[iptr->n].ptr < 0) {
                iptr=iptr->next;
                continue;
              }
              my_strdup(0, &type2, (inst[iptr->n].ptr + xctx->sym)->type);
              if (!type2 || !IS_LABEL_OR_PIN(type2) ) {
                iptr=iptr->next;
                continue;
              }
              if ((iptr->x0==x0) && (iptr->y0==y0)) {
                if(xctx->inst[iptr->n].node[0]) {
                   dbg(1, "pin_name=%s, node=%s\n", pin_name, xctx->inst[iptr->n].node[0]);
                   entry = str_hash_lookup(&table, pin_name, xctx->inst[iptr->n].node[0], XLOOKUP);
                   if(entry && entry->value[0] != '#') {
                     if(for_netlist) signal_short(xctx->inst[iptr->n].node[0], entry->value);
                   } else {
                     str_hash_lookup(&table, pin_name, xctx->inst[iptr->n].node[0], XINSERT);
                  }
                }
              }
              iptr=iptr->next;
            }
          } /* for (j=0;j<rects;j++) */
          /* 2nd loop: if unnamed nets, get name from duplicated pins */
          for (j=0;j<rects;j++) {
            rct=(inst[i].ptr+ xctx->sym)->rect[PINLAYER];
            x0=(rct[j].x1+rct[j].x2)/2;
            y0=(rct[j].y1+rct[j].y2)/2;
            rot=inst[i].rot;
            flip=inst[i].flip;
            ROTATION(rot, flip, 0.0,0.0,x0,y0,rx1,ry1);
            x0=inst[i].x0+rx1;
            y0=inst[i].y0+ry1;
            get_square(x0, y0, &sqx, &sqy);
            wptr = xctx->wire_spatial_table[sqx][sqy];
            pin_name = get_tok_value(rct[j].prop_ptr, "name", 0);
            while(wptr) {
              int assign = 0;
              if (touch(xctx->wire[wptr->n].x1, xctx->wire[wptr->n].y1,
                xctx->wire[wptr->n].x2, xctx->wire[wptr->n].y2, x0,y0)) {
                entry = str_hash_lookup(&table, pin_name, NULL, XLOOKUP);
                if(entry) {
                  if(xctx->wire[wptr->n].node) {
                    if(strcmp(xctx->wire[wptr->n].node, entry->value)) { /* net node name is different */
                      if(xctx->wire[wptr->n].node[0] == '#') { /* autonamed net */
                        if(entry->value[0] == '#') { /* autonamed pin */
                          int n, p;
                          sscanf(xctx->wire[wptr->n].node, "#net%d", &n);
                          sscanf(entry->value, "#net%d", &p);
                          dbg(lev, "2nd loop: net=%s pin=%s\n", xctx->wire[wptr->n].node, entry->value);
                          if(n > p) assign = 1; /* lower numbered #net names on pins have precedence */
                        } else { /* not autonamed pin */
                          assign = 1;
                        }
                      } else { /* not autonamed net */
                        if(entry->value[0] != '#') { /* not autonamed pin */
                          if(for_netlist) signal_short(xctx->wire[wptr->n].node, entry->value);
                        }
                      }
                    }
                  } else { /* wire .node is NULL */
                    assign = 1;
                  }
                }
    
                if(assign) {
                  dbg(lev, "inst %s pin %s(%s) --> net %s\n",
                      inst[i].instname, entry->token, entry->value, 
                      xctx->wire[wptr->n].node ? xctx->wire[wptr->n].node : "<NULL>");
                  my_strdup(0,  &xctx->wire[wptr->n].node, entry->value);
                  my_strdup(0, &xctx->wire[wptr->n].prop_ptr,
                  subst_token(xctx->wire[wptr->n].prop_ptr, "lab", entry->value));
                  wirecheck(wptr->n);
                  changed = 1;
                }
              } /* if(touch...) */
              wptr=wptr->next;
            }
          } /* for (j=0;j<rects;j++) */
        } /* if ((rects = (inst[i].ptr+ xctx->sym)->rects[PINLAYER]) > 0) */
      } /* if (type && !IS_LABEL_OR_PIN(type) ) */
      str_hash_free(&table);
    } /* for (i=0;i<instances;i++) */
  } while(changed);
  my_free(0, &type);
  my_free(0, &type2);
  my_free(0, &pt_symbol);
  dbg(lev, "name_pass_through_nets() end...\n");
}

static void name_nets_from_pass_through_syms(int for_netlist)
{
  int i, j, k, sqx, sqy;
  double x0, y0;
  Int_hashtable table = {NULL, 0};
  int *symtable = NULL, *pt_symbol = NULL;
  Wireentry *wptr;
  Instpinentry *iptr;
  char *type = NULL;
  int there_are_pt = 0;
  xWire * const wire = xctx->wire;
  xInstance * const inst = xctx->inst;
  int changed = 0;

  pt_symbol = my_calloc(973, xctx->symbols, sizeof(int));
  symtable = my_calloc(1581,  xctx->symbols, sizeof(int));
  /* we can not loop over xctx->symbols since we keep symbols of parent circuit while netlisting */
  for(i = 0; i < xctx->instances; i++) {
    k = inst[i].ptr;
    if( k < 0 || symtable[k] ) continue;
    symtable[k] =1;
    dbg(1, "name_pass_through_nets(): inst=%s sym=%s\n", inst[i].instname, xctx->sym[k].name);
    int_hash_init(&table, 37);
    for(j = 0; j < xctx->sym[k].rects[PINLAYER]; j++) {
      const char *pin_name = get_tok_value(xctx->sym[k].rect[PINLAYER][j].prop_ptr, "name", 0);
      if(int_hash_lookup(&table, pin_name, j, XINSERT_NOREPLACE)) {
        dbg(1, "   pass thru symbol found\n");
        pt_symbol[k] = 1;
        there_are_pt = 1;
      }
    }
    int_hash_free(&table);
    if(pt_symbol[k]) dbg(1, "duplicated pins: %s\n", xctx->sym[i].name);
  }
  my_free(831, &symtable);
  if(!there_are_pt) { /* nothing to do: no pass through symbols */
    my_free(832, &pt_symbol);
    return;
  }
  dbg(0, "\nname_pass_through_nets() start...\n");

  do {
    dbg(0, "\n\n    do loop\n\n");
    changed = 0;
    for(i = 0; i < xctx->instances; i++) {
      Str_hashtable table = {NULL, 0};
      Str_hashentry *entry;
      str_hash_init(&table, 37);
      k = inst[i].ptr;
      if( k < 0 ) continue;
      if(!inst[i].node) continue;
      if(!pt_symbol[k]) continue;
      for(j = 0; j < xctx->sym[k].rects[PINLAYER]; j++) {
        const char *pin_name = get_tok_value(xctx->sym[k].rect[PINLAYER][j].prop_ptr, "name", 0);
        get_inst_pin_coord(i, j, &x0, &y0);
        get_square(x0, y0, &sqx, &sqy);
        for(wptr = xctx->wire_spatial_table[sqx][sqy]; wptr; wptr = wptr->next) {
          if(!wire[wptr->n].node) continue;
          if(touch(wire[wptr->n].x1, wire[wptr->n].y1, wire[wptr->n].x2, wire[wptr->n].y2, x0,y0)) {
            dbg(0, "check inst %s pin %s(%d) wire %d(%s)\n", inst[i].instname, pin_name, j,
                wptr->n, wire[wptr->n].node ? wire[wptr->n].node : "<NULL>");
            str_hash_lookup(&table, pin_name, wire[wptr->n].node, XINSERT_NOREPLACE);
          }
        }
        for(iptr=xctx->instpin_spatial_table[sqx][sqy]; iptr; iptr = iptr ->next) {
          if(iptr->n == i || inst[iptr->n].ptr < 0) continue;
          if((iptr->x0==x0) && (iptr->y0==y0)) {
            my_strdup(1572, &type,(inst[iptr->n].ptr+ xctx->sym)->type);
            if(inst[iptr->n].node && type && (IS_LABEL_OR_PIN(type) || pt_symbol[inst[iptr->n].ptr])) {
              if(inst[iptr->n].node[iptr->pin]) {
                dbg(0, "inst %s pin %s(%d) hash_lookup %s\n",
                    inst[i].instname, pin_name, j, inst[iptr->n].node[iptr->pin]);
                str_hash_lookup(&table, pin_name, inst[iptr->n].node[iptr->pin], XINSERT_NOREPLACE);
              } 
            }
          }
        }
        my_free(1570, &type);
      } /* for(j = 0; j < xctx->sym[k].rects[PINLAYER]; j++) */

      for(j = 0; j < xctx->sym[k].rects[PINLAYER]; j++) {
        const char *pin_name = get_tok_value(xctx->sym[k].rect[PINLAYER][j].prop_ptr, "name", 0);
        get_inst_pin_coord(i, j, &x0, &y0);
        get_square(x0, y0, &sqx, &sqy);
        entry = str_hash_lookup(&table, pin_name, NULL, XLOOKUP);
        if(!entry) dbg(0, "inst %s pin %s(%d) is unset\n", inst[i].instname, pin_name, j);
        if(!entry) continue;
        if(!inst[i].node[j]) set_inst_node(i, j, entry->value, for_netlist);
        dbg(0, "inst %s pin %s(%d) <-- %s\n", inst[i].instname, pin_name, j, entry->value);
        for(wptr = xctx->wire_spatial_table[sqx][sqy]; wptr; wptr = wptr->next) {
          if(touch(wire[wptr->n].x1, wire[wptr->n].y1, wire[wptr->n].x2, wire[wptr->n].y2, x0,y0)) {
            if(!wire[wptr->n].node) {
              dbg(0, "wire %d <-- inst %s pin %s net %s\n", wptr->n, inst[i].instname, pin_name, entry->value);
              my_strdup(1565, &wire[wptr->n].node, entry->value);
              my_strdup(834, &wire[wptr->n].prop_ptr,
                subst_token(wire[wptr->n].prop_ptr, "lab", wire[wptr->n].node));
              wirecheck(wptr->n); /* recursive check */
              changed = 1;
            } else {
              dbg(0, "wire %d has already node: %s\n", wptr->n, wire[wptr->n].node);
            }
          }
        } /*  for(wptr=...) */
        for(iptr=xctx->instpin_spatial_table[sqx][sqy]; iptr; iptr= iptr->next) {
          if(iptr->n == i ||  inst[iptr->n].ptr < 0) continue;
          my_strdup(1569, &type,(inst[i].ptr+ xctx->sym)->type);
          if(type && !IS_LABEL_OR_PIN(type) && pt_symbol[iptr->n] && inst[iptr->n].node) {
            if((iptr->x0==x0) && (iptr->y0==y0)) {
              if(!inst[iptr->n].node[iptr->pin]) {
                set_inst_node(iptr->n, iptr->pin, entry->value, for_netlist);
              } else {
                dbg(0, "inst %s pin %s(%d) has already node: %s\n",
                  inst[iptr->n].instname, pin_name, iptr->pin, inst[iptr->n].node[iptr->pin]);
              }
            }
          }
        }
      } /* for(j = 0; j < xctx->sym[k].rects[PINLAYER]; j++) */
      str_hash_free(&table);
    } /* for(i = 0; i < xctx->instances; i++) */
  } while(changed);
  my_free(833, &pt_symbol);
}

void prepare_netlist_structs(int for_netlist)
{
  char nn[PATH_MAX+30];

  if(for_netlist>0 && xctx->prep_net_structs) return;
  else if(!for_netlist && xctx->prep_hi_structs) return;
  /* delete instance pins spatial hash, wires spatial hash, node_hash, wires and inst nodes.*/
  if(for_netlist) {
    my_snprintf(nn, S(nn), "-----------%s", xctx->sch[xctx->currsch]);
    statusmsg(nn,2);
  }
  dbg(1, "prepare_netlist_structs(): extraction: %s\n", xctx->sch[xctx->currsch]);
  delete_netlist_structs();
  free_simdata(); /* invalidate simulation cache */
  reset_node_data_and_rehash(for_netlist);
  get_unnamed_node(0,0,0); /*initializes node multiplicity data struct */
  name_nodes_of_pins_labels_and_attached_nets(for_netlist);
  /* name_nets_from_pass_through_syms(for_netlist); */
  name_unlabeled_nets();
  name_pass_through_nets(for_netlist);
  name_generics(for_netlist);
  name_non_label_inst_pins(for_netlist);

  rebuild_selected_array();
  if(for_netlist>0) {
    xctx->prep_net_structs=1;
    xctx->prep_hi_structs=1;
  } else xctx->prep_hi_structs=1;
  dbg(1, "prepare_netlist_structs(): returning\n");
  /* avoid below call: it in turn calls prepare_netlist_structs(), too many side effects */
  /* propagate_hilights(1, 0, XINSERT_NOREPLACE);*/
}

void delete_inst_node(int i)
{
   int j, rects;
   if( xctx->inst[i].ptr == -1 || !xctx->inst[i].node) return;
   rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER] +
           (xctx->inst[i].ptr+ xctx->sym)->rects[GENERICLAYER];
   if( rects > 0 )
   {
     for(j=0;j< rects ;j++)
       my_free(849, &xctx->inst[i].node[j]);
     my_free(850, &xctx->inst[i].node );
   }
}

void delete_netlist_structs(void)
{
 int i;
  /* erase node data structures */
   dbg(1, "delete_netlist_structs(): begin erasing\n");
  for(i=0;i<xctx->instances;i++)
  {
   delete_inst_node(i);
  }
  for(i=0;i<xctx->wires;i++)
  {
    my_free(851, &xctx->wire[i].node);
  }
  /* erase inst and wire topological hash tables */
  del_inst_pin_table();
  node_hash_free();
  dbg(1, "delete_netlist_structs(): end erasing\n");
  xctx->prep_net_structs=0;
  xctx->prep_hi_structs=0;
}

int warning_overlapped_symbols(int sel)
{
  int i;
  Int_hashtable table = {NULL, 0};
  Int_hashentry *found;
  char str[2048];
  char s[512];

  int_hash_init(&table, HASHSIZE);
  for(i = 0; i < xctx->instances; i++) {
    dbg(1, "instance:%s: %s\n", xctx->inst[i].instname, xctx->inst[i].name);
    my_snprintf(s, S(s), "%g %g %g %g",
       xctx->inst[i].xx1, xctx->inst[i].yy1, xctx->inst[i].xx2, xctx->inst[i].yy2);

    dbg(1, "  bbox: %g %g %g %g\n", xctx->inst[i].xx1, xctx->inst[i].yy1, xctx->inst[i].xx2, xctx->inst[i].yy2);
    dbg(1, "  s=%s\n", s);
    found =  int_hash_lookup(&table, s, i, XINSERT_NOREPLACE);
    if(found) {
      if(sel == 0) {
        xctx->inst[i].color = -PINLAYER;
        xctx->hilight_nets=1;
      } else {
        xctx->inst[i].sel = SELECTED;
        xctx->need_reb_sel_arr = 1;
      }
      my_snprintf(str, S(str), "Warning: overlapped instance found: %s(%s) -> %s\n",
            xctx->inst[i].instname, xctx->inst[i].name, xctx->inst[found->value].instname);
      statusmsg(str,2);
      tcleval("show_infotext"); /* critical error: force ERC window showing */
    }
  }
  int_hash_free(&table);
  if(sel && xctx->need_reb_sel_arr) rebuild_selected_array();
  return 0;
}

int sym_vs_sch_pins()
{
  char **lab_array =NULL;
  int lab_array_size = 0;
  int i, j, k, symbol, n_syms, rects, pin_cnt=0, pin_match, mult;
  struct stat buf;
  char name[PATH_MAX];
  char *type = NULL;
  char *tmp = NULL;
  char *lab=NULL;
  char *pin_name=NULL;
  char *pin_dir=NULL;
  double tmpd;
  FILE *fd;
  int tmpi;
  short tmps;
  int endfile;
  char tag[1];
  char filename[PATH_MAX];
  n_syms = xctx->symbols;
  for(i=0;i<n_syms;i++)
  {
    if( xctx->sym[i].type && !strcmp(xctx->sym[i].type,"subcircuit")) {
      rects = xctx->sym[i].rects[PINLAYER];

      get_sch_from_sym(filename, xctx->sym + i);
      if(!stat(filename, &buf)) {
        fd = fopen(filename, "r");
        pin_cnt = 0;
        endfile = 0;
        xctx->file_version[0] = '\0';
        while(!endfile) {
          if(fscanf(fd," %c",tag)==EOF) break;
          switch(tag[0]) {
            case 'v':
             load_ascii_string(&xctx->version_string, fd);
             my_snprintf(xctx->file_version, S(xctx->file_version), "%s", 
                         get_tok_value(xctx->version_string, "file_version", 0));
            break;

            case 'E':
            case 'S':
            case 'V':
            case 'K':
            case 'G':
              load_ascii_string(&tmp, fd);
              break;
            case '#':
              read_line(fd, 1);
              break;
            case 'F': /* extension for future symbol floater labels */
              read_line(fd, 1);
              break;
            case 'L':
            case 'B':
              if(fscanf(fd, "%d",&tmpi)< 1) {
                 fprintf(errfp,"sym_vs_sch_pins(): WARNING:  missing fields for LINE/BOX object, ignoring\n");
                 read_line(fd, 0);
                 break;
              }
            case 'N':
              if(fscanf(fd, "%lf %lf %lf %lf ",&tmpd, &tmpd, &tmpd, &tmpd) < 4) {
                 fprintf(errfp,"sym_vs_sch_pins(): WARNING:  missing fields for LINE/BOX object, ignoring\n");
                 read_line(fd, 0);
                 break;
               }
               load_ascii_string(&tmp, fd);
              break;
            case 'P':
              if(fscanf(fd, "%d %d",&tmpi, &tmpi)<2) {
                fprintf(errfp,"sym_vs_sch_pins(): WARNING: missing fields for POLYGON object, ignoring.\n");
                read_line(fd, 0);
                break;
              }
              for(j=0;j<tmpi;j++) {
                if(fscanf(fd, "%lf %lf ",&tmpd, &tmpd)<2) {
                  fprintf(errfp,"sym_vs_sch_pins(): WARNING: missing fields for POLYGON points, ignoring.\n");
                  read_line(fd, 0);
                }
              }
              load_ascii_string( &tmp, fd);
              break;
            case 'A':
              if(fscanf(fd, "%d",&tmpi)< 1) {
                 fprintf(errfp,"sym_vs_sch_pins(): WARNING:  missing fields for ARC object, ignoring\n");
                 read_line(fd, 0);
                 break;
              }
              if(fscanf(fd, "%lf %lf %lf %lf %lf ",&tmpd, &tmpd, &tmpd, &tmpd, &tmpd) < 5) {
                fprintf(errfp,"sym_vs_sch_pins(): WARNING:  missing fields for ARC object, ignoring\n");
                read_line(fd, 0);
                break;
              }
              load_ascii_string(&tmp, fd);
              break;
            case 'T':
              load_ascii_string(&tmp,fd);
              if(fscanf(fd, "%lf %lf %hd %hd %lf %lf ", &tmpd, &tmpd, &tmps, &tmps, &tmpd, &tmpd) < 6 ) {
                fprintf(errfp,"sym_vs_sch_pins(): WARNING:  missing fields for TEXT object, ignoring\n");
                read_line(fd, 0);
                break;
              }
              load_ascii_string(&tmp,fd);
              break;
            case 'C':
              load_ascii_string(&tmp, fd);
              my_strncpy(name, tmp, S(name));

              if(!strcmp(xctx->file_version,"1.0") ) {
                dbg(1, "sym_vs_sch_pins(): add_ext(name,\".sym\") = %s\n", add_ext(name, ".sym") );
                my_strncpy(name, add_ext(name, ".sym"), S(name));
              }

              if(fscanf(fd, "%lf %lf %hd %hd", &tmpd, &tmpd, &tmps, &tmps) < 4) {
                fprintf(errfp,"sym_vs_sch_pins() WARNING: missing fields for INST object, filename=%s\n",
                  filename);
                read_line(fd, 0);
                break;
              }
              load_ascii_string(&tmp,fd);
              symbol = match_symbol(name);
              my_strdup(276, &type, xctx->sym[symbol].type);
              if(type && IS_PIN(type)) {
                my_strdup(292, &lab, expandlabel(get_tok_value(tmp, "lab", 0), &mult));
                if(pin_cnt >= lab_array_size) {
                  lab_array_size += CADCHUNKALLOC;
                  my_realloc(154, &lab_array, lab_array_size * sizeof(char *));
                }
                lab_array[pin_cnt] = NULL;
                my_strdup(155, &(lab_array[pin_cnt]), lab);
                pin_cnt++;
                pin_match = 0;
                for(j=0; j < rects; j++) {
                  my_strdup(293, &pin_name, 
                    expandlabel(get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr, "name", 0), &mult));
                  my_strdup(294, &pin_dir, get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr, "dir", 0));
                  if( pin_name && !strcmp(pin_name, lab)) {
                    if(!(
                          ( !strcmp(type, "ipin") && !strcmp(pin_dir, "in") ) ||
                          ( !strcmp(type, "opin") && !strcmp(pin_dir, "out") ) ||
                          ( !strcmp(type, "iopin") && !strcmp(pin_dir, "inout") )
                        )
                      ) {
                      char str[2048];
                      my_snprintf(str, S(str), "Symbol %s: Unmatched subcircuit schematic pin direction: %s",
                                  xctx->sym[i].name, lab);
                      statusmsg(str,2);
                      my_snprintf(str, S(str), "    %s <--> %s", type, pin_dir);
                      statusmsg(str,2);
                      tcleval("show_infotext"); /* critical error: force ERC window showing */
                      for(j = 0; j < xctx->instances; j++) {
                        if(!xctx->x_strcmp(xctx->inst[j].name, xctx->sym[i].name)) {
                          xctx->inst[i].color = -PINLAYER;
                          xctx->hilight_nets=1;
                        }
                      }
                    }
                    pin_match++;
                    break;
                  }
                }
                if(!pin_match) {
                  char str[2048];
                  /* fprintf(errfp, "  unmatched sch / sym pin: %s\n", lab); */
                  my_snprintf(str, S(str), "Symbol %s: schematic pin: %s not in symbol", xctx->sym[i].name, lab);
                  statusmsg(str,2);
                  tcleval("show_infotext"); /* critical error: force ERC window showing */
                  for(j = 0; j < xctx->instances; j++) {
                    if(!xctx->x_strcmp(xctx->inst[j].name, xctx->sym[i].name)) {
                      xctx->inst[i].color = -PINLAYER;
                      xctx->hilight_nets=1;
                    }
                  }
                }
              }
              break;
            case '[':
              load_sym_def(name, fd);
              break;
            case ']':
              read_line(fd, 0);
              endfile=1;
              break;
            default:
              if( tag[0] == '{' ) ungetc(tag[0], fd);
              read_record(tag[0], fd, 0);
              break;
          }
          read_line(fd, 0); /* discard any remaining characters till (but not including) newline */
          if(!xctx->file_version[0]) {
            my_snprintf(xctx->file_version, S(xctx->file_version), "1.0");
            dbg(1, "sym_vs_sch_pins(): no file_version, assuming file_version=%s\n", xctx->file_version);
          }
        } /* while(!endfile) */
        fclose(fd);
        if(pin_cnt != rects) {
          char str[2048];
          my_snprintf(str, S(str), "Symbol %s has %d pins, its schematic has %d pins",
                      xctx->sym[i].name, rects, pin_cnt);
          statusmsg(str,2);
          tcleval("show_infotext"); /* critical error: force ERC window showing */
          for(j = 0; j < xctx->instances; j++) {
            if(!xctx->x_strcmp(xctx->inst[j].name, xctx->sym[i].name)) {
              xctx->inst[i].color = -PINLAYER;
              xctx->hilight_nets=1;
            }
          }
        }
        for(j=0; j < rects; j++) {
          my_strdup(295, &pin_name,
             expandlabel(get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr, "name", 0), &mult));
          pin_match = 0;
          for(k=0; k<pin_cnt; k++) {
            if(pin_name && !strcmp(lab_array[k], pin_name)) {
              pin_match++;
              break;
            }
          }
          if(!pin_match) {
            char str[2048];
            /* fprintf(errfp, "  unmatched sch / sym pin: %s\n", lab); */
            my_snprintf(str, S(str), "Symbol %s: symbol pin: %s not in schematic",
                        xctx->sym[i].name, pin_name ? pin_name : "<NULL>");
            statusmsg(str,2);
            tcleval("show_infotext"); /* critical error: force ERC window showing */
            for(k = 0; k < xctx->instances; k++) {
              if(!xctx->x_strcmp(xctx->inst[k].name, xctx->sym[i].name)) {
                xctx->inst[i].color = -PINLAYER;
                xctx->hilight_nets=1;
              }
            }
          }
        }
      }
      if(lab_array_size) {
        for(k=0;k<pin_cnt;k++) {
          my_free(842, &(lab_array[k]));
        }
        my_free(843, &lab_array);
        lab_array_size = 0;
        pin_cnt=0;
      }
    } /* if( ... "subcircuit"... ) */
    my_free(844, &type);
    my_free(845, &tmp);
    my_free(846, &lab);
    my_free(847, &pin_name);
    my_free(848, &pin_dir);
  } /* for(i=0;i<n_syms;i++) */

  while(xctx->symbols > n_syms) remove_symbol(xctx->symbols - 1);
  return 0;
}
