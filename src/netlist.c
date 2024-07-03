/* File: netlist.c
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2023 Stefan Frederik Schippers
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

static int for_netlist = 0;
static int netlist_lvs_ignore = 0;
static void instdelete(int n, int x, int y)
{
  Instentry *saveptr, **prevptr, *ptr;

  prevptr = &xctx->inst_spatial_table[x][y];
  ptr = *prevptr;
  while(ptr) {
    if(ptr->n == n) {
      saveptr = ptr->next;
      my_free(_ALLOC_ID_, &ptr);
      *prevptr = saveptr;
      return;
    }
    prevptr = &ptr->next;
    ptr = *prevptr;
  }
}

static void instinsert(int n, int x, int y)
{
  Instentry *ptr, *newptr;
  ptr=xctx->inst_spatial_table[x][y];
  newptr=my_malloc(_ALLOC_ID_, sizeof(Instentry));
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
    my_free(_ALLOC_ID_, &t);
    t = tmp;
  }
  return NULL;
}

void del_inst_table(void)
{
  int i,j;

  for(i=0;i<NBOXES; ++i)
    for(j=0;j<NBOXES; ++j)
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
  for(i=x1a; i<=x2a && counti < NBOXES; ++i)
  {
   ++counti;
   tmpi=i%NBOXES; if(tmpi<0) tmpi+=NBOXES;
   countj=0;
   for(j=y1a; j<=y2a && countj < NBOXES; ++j)
   {
    ++countj;
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
 for(n=0; n<xctx->instances; ++n) {
   hash_inst(XINSERT, n);
 }
 xctx->prep_hash_inst=1;
}


/* START HASH ALL OBJECTS */

static Objectentry *delobjectentry(Objectentry *t)
{
  Objectentry *tmp;
  while( t ) {
    tmp = t->next;
    my_free(_ALLOC_ID_, &t);
    t = tmp;
  }
  return NULL;
}

void del_object_table(void)
{
  int i,j;

  for(i=0;i<NBOXES; ++i)
    for(j=0;j<NBOXES; ++j)
      xctx->object_spatial_table[i][j] = delobjectentry(xctx->object_spatial_table[i][j]);
  xctx->prep_hash_object=0;
  xctx->n_hash_objects = 0;
  dbg(1, "del_object_table(): cleared object hash table\n");
}



static void objectdelete(int type, int n, int c, int x, int y)
{
  Objectentry *saveptr, **prevptr, *ptr;
  
  prevptr = &xctx->object_spatial_table[x][y];
  ptr = *prevptr;
  while(ptr) {
    if(ptr->n == n && ptr->type == type && ptr->c == c ) {
      saveptr = ptr->next;
      my_free(_ALLOC_ID_, &ptr);
      *prevptr = saveptr;
      return;
    }
    prevptr = &ptr->next;
    ptr = *prevptr;
  }
} 
  
static void objectinsert(int type, int n, int c, int x, int y)
{
  Objectentry *ptr, *newptr;
  ptr=xctx->object_spatial_table[x][y];
  newptr=my_malloc(_ALLOC_ID_, sizeof(Instentry));
  newptr->next=ptr;
  newptr->type=type;
  newptr->n=n;
  newptr->c=c;
  xctx->object_spatial_table[x][y]=newptr;
  dbg(2, "objectnsert(): inserting object %d %d %d at %d,%d\n",type, n, c, x, y);
}

/* what:
 * 0, XINSERT : add to hash
 * 1, XDELETE : remove from hash
 */
void hash_object(int what, int type, int n, int c)
{
  int tmpi,tmpj, counti,countj,i,j;
  double tmpd;
  double x1, y1, x2, y2;
  int x1a, x2a, y1a, y2a;
  int skip = 0;
  char *estr = NULL;

  switch(type) {
    case ELEMENT:
      x1=xctx->inst[n].x1;
      x2=xctx->inst[n].x2;
      y1=xctx->inst[n].y1;
      y2=xctx->inst[n].y2;
      break;
    case xRECT:
      x1 = xctx->rect[c][n].x1;
      y1 = xctx->rect[c][n].y1;
      x2 = xctx->rect[c][n].x2;
      y2 = xctx->rect[c][n].y2;
      break;
    case WIRE:
      x1 = xctx->wire[n].x1;
      y1 = xctx->wire[n].y1;
      x2 = xctx->wire[n].x2;
      y2 = xctx->wire[n].y2;
      break;
    case LINE:
      x1 = xctx->line[c][n].x1;
      y1 = xctx->line[c][n].y1;
      x2 = xctx->line[c][n].x2;
      y2 = xctx->line[c][n].y2;
      break;
    case POLYGON:
      polygon_bbox(xctx->poly[c][n].x, xctx->poly[c][n].y, xctx->poly[c][n].points, &x1, &y1, &x2, &y2);
      break;
    case ARC:
      arc_bbox(xctx->arc[c][n].x, xctx->arc[c][n].y, xctx->arc[c][n].r,
              xctx->arc[c][n].a, xctx->arc[c][n].b, &x1, &y1, &x2, &y2);
      break;
    case xTEXT:
      estr = my_expand(get_text_floater(n), tclgetintvar("tabstop"));
      text_bbox(estr,
           xctx->text[n].xscale, xctx->text[n].yscale, xctx->text[n].rot, xctx->text[n].flip,
           xctx->text[n].hcenter, xctx->text[n].vcenter,
           xctx->text[n].x0, xctx->text[n].y0,
           &x1,&y1, &x2,&y2, &tmpi, &tmpd);
      my_free(_ALLOC_ID_, &estr);
      break;
    default:
      skip = 1;
      x1 = 0.0;
      y1 = 0.0;
      x2 = 0.0;
      y2 = 0.0;
      break;
  }
  if(skip) return;
  xctx->n_hash_objects++; /* total number of objects in spatial hash table */
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
  for(i=x1a; i<=x2a && counti < NBOXES; ++i)
  {
   ++counti;
   tmpi=i%NBOXES; if(tmpi<0) tmpi+=NBOXES;
   countj=0;
   for(j=y1a; j<=y2a && countj < NBOXES; ++j)
   {
    ++countj;
    tmpj=j%NBOXES; if(tmpj<0) tmpj+=NBOXES;
    /* insert object_ptr[n] in region [tmpi, tmpj] */
    if(what == XINSERT) objectinsert(type, n, c, tmpi, tmpj);
    else objectdelete(type, n, c, tmpi, tmpj);
   }
  }
}

void hash_objects(void) /* 20171203 insert object bbox in spatial hash table */
{
  int n, c;

  if(xctx->prep_hash_object) return;
  del_object_table();
  for(n=0; n<xctx->instances; ++n) {
    hash_object(XINSERT, ELEMENT, n, 0);
  }
  for(n=0; n<xctx->wires; ++n) {
    hash_object(XINSERT, WIRE, n, 0);
  }
  for(n=0; n<xctx->texts; ++n) {
    hash_object(XINSERT, xTEXT, n, 0);
  }
  for(c=0;c<cadlayers; ++c)
  {   
    for(n=0; n<xctx->rects[c]; n++) {
      hash_object(XINSERT, xRECT, n, c);
    }
    for(n=0; n<xctx->lines[c]; n++) {
      hash_object(XINSERT, LINE, n, c);
    }
    for(n=0; n<xctx->arcs[c]; n++) {
      hash_object(XINSERT, ARC, n, c);
    }
    for(n=0; n<xctx->polygons[c]; n++) {
      hash_object(XINSERT, POLYGON, n, c);
    }

  }
  xctx->prep_hash_object=1;
}

/* END HASH ALL OBJECTS */

static void instpindelete(int n,int pin, int x, int y)
{
  Instpinentry *saveptr, **prevptr, *ptr;

  prevptr = &xctx->instpin_spatial_table[x][y];
  ptr = *prevptr;
  while(ptr) {
    if(ptr->n == n && ptr->pin == pin) {
      saveptr = ptr->next;
      my_free(_ALLOC_ID_, &ptr);
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
 newptr=my_malloc(_ALLOC_ID_, sizeof(Instpinentry));
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
    my_free(_ALLOC_ID_, &t);
    t = tmp;
  }
  return NULL;
}

static void del_inst_pin_table(void)
{
 int i,j;

 for(i=0;i<NBOXES; ++i)
  for(j=0;j<NBOXES; ++j)
   xctx->instpin_spatial_table[i][j] = delinstpinentry(xctx->instpin_spatial_table[i][j]);
}


static void wiredelete(int n, int x, int y)
{
  Wireentry *saveptr, **prevptr, *ptr;

  prevptr = &xctx->wire_spatial_table[x][y];
  ptr = *prevptr;
  while(ptr) {
    if(ptr->n == n) {
      saveptr = ptr->next;
      my_free(_ALLOC_ID_, &ptr);
      *prevptr = saveptr;
      return;
    }
    prevptr = &ptr->next;
    ptr = *prevptr;
  }
}

static void wireinsert(int n, int x, int y)
{
  Wireentry *ptr, *newptr;

  ptr=xctx->wire_spatial_table[x][y];
  newptr=my_malloc(_ALLOC_ID_, sizeof(Wireentry));
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
    my_free(_ALLOC_ID_, &t);
    t = tmp;
  }
  return NULL;
}

void del_wire_table(void)
{
 int i,j;

 for(i=0;i<NBOXES; ++i)
  for(j=0;j<NBOXES; ++j)
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
 *
 *                                 inst   pin */
static int hash_inst_pin(int what, int i, int j)
{
  int err = 0;
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


  if(xctx->netlist_count && for_netlist && j<rects &&
     (!prop_ptr || !get_tok_value(prop_ptr, "name",0)[0] || !get_tok_value(prop_ptr, "dir",0)[0]) ) {
    char str[2048];
    my_snprintf(str, S(str), "Error: instance %s, symbol %s: missing all or name or dir attributes on pin "
        " %d\n"
        "attribute string:\n%s",
        xctx->inst[i].instname, xctx->inst[i].name, j, prop_ptr ? prop_ptr : "<NULL>");
    statusmsg(str,2);
    err |= 1;
    xctx->inst[i].color = -PINLAYER;
    xctx->hilight_nets=1;
  }
  rot=xctx->inst[i].rot;
  flip=xctx->inst[i].flip;
  ROTATION(rot, flip, 0.0,0.0,x0,y0,rx1,ry1);
  x0=xctx->inst[i].x0+rx1;
  y0=xctx->inst[i].y0+ry1;
  get_square(x0, y0, &sqx, &sqy);
  if( what == XINSERT ) instpininsert(i, j, x0, y0, sqx, sqy);
  else                 instpindelete(i, j, sqx, sqy);
  return err;
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

  dbg(1, "hash_wire(): what=%d n=%d incremental=%d\n",  what, n, incremental);
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
  for(i=x1a; i<=x2a && counti < NBOXES; ++i)
  {
   ++counti;
   tmpi=i%NBOXES; if(tmpi<0) tmpi+=NBOXES;
   countj=0;
   for(j=y1a; j<=y2a && countj < NBOXES; ++j)
   {
    ++countj;
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

 for(n=0; n<xctx->wires; ++n) hash_wire(XINSERT, n, 0);
 xctx->prep_hash_wires=1;
}

/* return 0 if library path of s matches any lib name in tcl variable $xschem_libs */
/* what: 
 * 1: netlist exclude pattern
 * 2: hierarchical print exclude pattern
 * 4: hierarchical cell listing exclude pattern 
 */
int check_lib(int what, const char *s)
{
 int range,i, found;
 char str[PATH_MAX + 512]; /* overflow safe 20161122 */

 found=0;
 if(what & 1) tcleval("llength $xschem_libs");
 if(what & 2) tcleval("llength $noprint_libs");
 if(what & 4) tcleval("llength $nolist_libs");
 range = atoi(tclresult());
 dbg(1, "check_lib(): s=%s, range=%d\n", s, range);

 for(i=0;i<range; ++i){
  if(what & 1 ) my_snprintf(str, S(str), "lindex $xschem_libs %d",i);
  if(what & 2 ) my_snprintf(str, S(str), "lindex $noprint_libs %d",i);
  if(what & 4 ) my_snprintf(str, S(str), "lindex $nolist_libs %d",i);
  tcleval(str);
  dbg(1, "check_lib(): %s -> %s\n", str, tclresult());
  my_snprintf(str, S(str), "regexp {%s} {%s}", tclresult(), s);
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

  str = get_tok_value(xctx->inst[i].prop_ptr, "lvs_ignore", 0);
  if(str[0]) {
    /* fprintf(errfp, "netlist_options(): prop_ptr=%s\n", xctx->inst[i].prop_ptr); */
    if(!strboolcmp(str, "true")) tclsetintvar("lvs_ignore", 1);
    else tclsetintvar("lvs_ignore", 0);
  }
  str = get_tok_value(xctx->inst[i].prop_ptr, "lvs_netlist", 0);
  if(str[0]) {
    /* fprintf(errfp, "netlist_options(): prop_ptr=%s\n", xctx->inst[i].prop_ptr); */
    if(!strboolcmp(str, "true")) tclsetintvar("lvs_netlist", 1);
    else tclsetintvar("lvs_netlist", 0);
  }
  str = get_tok_value(xctx->inst[i].prop_ptr, "spiceprefix", 0);
  if(str[0]) {
    /* fprintf(errfp, "netlist_options(): prop_ptr=%s\n", xctx->inst[i].prop_ptr); */
    if(!strboolcmp(str, "false")) tclsetvar("spiceprefix", "0");
    else tclsetvar("spiceprefix", "1");
  }

  str = get_tok_value(xctx->inst[i].prop_ptr, "hiersep", 0);
  if(str[0]) {
    my_snprintf(xctx->hiersep, S(xctx->hiersep), "%s", str);
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

/* used only for debug */
#if 0
static void print_wires(void)
{
 int i,j;
 Wireentry *ptr;
 for(i=0;i<NBOXES; ++i) {
   for(j=0;j<NBOXES; ++j)
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

/* store list of global nodes (global=1 set in symbol props) to be printed in netlist 28032003 */
/* what: */
/*      0: print list of global nodes and delete list */
/*      1: add entry */
/*      2: delete list only, no print */
/*      3: look if node is a global */
int record_global_node(int what, FILE *fp, const char *node)
{
 int i;

 if( what == 1 || what == 3) {
    if(!node) return 0;
    if(!strcmp(node, "0")) return 1;
    for(i = 0;i < xctx->max_globals; ++i) {
      if( !strcmp(node, xctx->globals[i] )) return 1; /* node is a global */
    }
    if(what == 3) return 0; /* node is not a global */
    if(xctx->max_globals >= xctx->size_globals) {
       xctx->size_globals+=CADCHUNKALLOC;
       my_realloc(_ALLOC_ID_, &xctx->globals, xctx->size_globals*sizeof(char *) );
    }
    xctx->globals[xctx->max_globals]=NULL;
    my_strdup(_ALLOC_ID_, &xctx->globals[xctx->max_globals], node);
    xctx->max_globals++;
 } else if(what == 0) {
    for(i = 0;i < xctx->max_globals; ++i) {
       if(xctx->netlist_type == CAD_SPICE_NETLIST) fprintf(fp, ".GLOBAL %s\n", xctx->globals[i]);
       if(xctx->netlist_type == CAD_TEDAX_NETLIST) fprintf(fp, "__GLOBAL__ %s\n", xctx->globals[i]);
    }
 } else if(what == 2) {
    for(i=0;i<xctx->max_globals; ++i) {
       my_free(_ALLOC_ID_, &xctx->globals[i]);
    }
    my_free(_ALLOC_ID_, &xctx->globals);
    xctx->size_globals = xctx->max_globals=0;

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

/* what==0 -> initialize  */
/* what==1 -> get new node name, net##   */
/* what==2 -> update multiplicity   */
/* what==3 -> get node multiplicity */
int get_unnamed_node(int what, int mult,int node)
{
  dbg(2, "get_unnamed_node(): what=%d mult=%d node=%d\n", what, mult, node);
  if(what==0) { /* initialize unnamed node data structures */
    xctx->new_node=0;
    my_free(_ALLOC_ID_, &xctx->node_mult);
    xctx->node_mult_size=0;
    return 0;
  }
  else if(what==1) { /* get a new unique unnamed node */
    char tmp_str[30];
    do {
      ++xctx->new_node;
      my_snprintf(tmp_str, S(tmp_str), "net%d", xctx->new_node);
    /* JL avoid autonamed nets clash with user defined 'net#' names */
    } while (bus_node_hash_lookup(tmp_str, "", XLOOKUP, 0, "", "", "", "")!=NULL);
    if(xctx->new_node >= xctx->node_mult_size) { /* enlarge array and zero it */
      int oldsize = xctx->node_mult_size;
      xctx->node_mult_size = xctx->new_node + CADCHUNKALLOC;
      my_realloc(_ALLOC_ID_, &xctx->node_mult, sizeof(xctx->node_mult[0]) * xctx->node_mult_size );
      memset(xctx->node_mult + oldsize, 0, (xctx->node_mult_size - oldsize) * sizeof(xctx->node_mult[0]));
    }
    xctx->node_mult[xctx->new_node]=mult;
    return xctx->new_node;
  }
  else if(what==2) { /* update node multiplicity if given mult is lower */
    if(xctx->node_mult[node]==0) xctx->node_mult[node]=mult;
    else if(mult < xctx->node_mult[node]) xctx->node_mult[node]=mult;
    return 0;
  }
  else { /* what=3 , return node multiplicity */
    dbg(2, "get_unnamed_node(): returning mult=%d\n", xctx->node_mult[node]);
    return xctx->node_mult[node];
  }
}

static void name_generics()
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
  dbg(2, "name_generics(): naming generics from attached labels\n");
  if(for_netlist) for (i=0;i<instances; ++i) { /* ... assign node fields on all (non label) instances */
    if(skip_instance(i, 0, netlist_lvs_ignore)) continue;
    my_strdup(_ALLOC_ID_, &type,(inst[i].ptr+ xctx->sym)->type);
    if(type && !IS_LABEL_OR_PIN(type) ) {
      if((generic_rects = (inst[i].ptr+ xctx->sym)->rects[GENERICLAYER]) > 0) {
        rects = (inst[i].ptr+ xctx->sym)->rects[PINLAYER];
        for (j=rects;j<rects+generic_rects; ++j) {
          if(!inst[i].node) continue;
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
            int p = iptr->pin;
            int n = iptr->n;
            if(n == i) continue;
            if(!inst[n].node) continue;
            if((iptr->x0==x0) && (iptr->y0==y0)) {
              if((inst[n].ptr+ xctx->sym)->type && inst[n].node[p] != NULL &&
                 !strcmp((inst[n].ptr+ xctx->sym)->type, "label")) {
                dbg(2, "name_generics(): naming generic %s\n", inst[n].node[p]);
                my_strdup(_ALLOC_ID_,  &inst[i].node[j], get_tok_value(inst[n].prop_ptr,"value",0) );
                if(!for_netlist) {
                  my_strdup(_ALLOC_ID_, &sig_type,"");
                  bus_node_hash_lookup(inst[n].node[p],"", XINSERT, 1, sig_type,"", "","");
                } else {
                  my_strdup(_ALLOC_ID_, &sig_type,
                    get_tok_value((inst[i].ptr+ xctx->sym)->rect[GENERICLAYER][j-rects].prop_ptr, "sig_type", 0));
                  /* insert generic label in hash table as a port so it will not */
                  /* be declared as a signal in the vhdl netlist. this is a workaround */
                  /* that should be fixed 25092001 */
                  bus_node_hash_lookup(inst[n].node[p],
                    get_tok_value((inst[i].ptr+ xctx->sym)->rect[GENERICLAYER][j-rects].prop_ptr, "dir",0),
                    XINSERT, 1, sig_type,"", "","");
                }
              } /* end if(inst[iptr->n].node[iptr->pin] != NULL) */
            } /* end if( (iptr->x0==x0) && (iptr->y0==y0) ) */
          } /* end for(iptr=xctx->instpin_spatial_table[sqx][sqy]; iptr; iptr = iptr ->next) */
        } /* end for(j=0;j<rects; ++j) */
      } /* end if( rects=...>0) */
    } /* end if(type not a label nor pin)... */
  } /* end for(i...) */
  if(type) my_free(_ALLOC_ID_, &type);
}

static int signal_short( const char *tag, const char *n1, const char *n2)
{
 int err = 0;
 char str[2048];
 if( xctx->netlist_count && n1 && n2 && strcmp( n1, n2) )
 {
   err |= 1;
   my_snprintf(str, S(str), "Error: %s shorted: %s - %s", tag, n1, n2);
   dbg(1, "signal_short(): signal_short: shorted: %s - %s", n1, n2);
   statusmsg(str,2);
   if(!xctx->netlist_count) {
      bus_hilight_hash_lookup(n1, xctx->hilight_color, XINSERT);
      if(tclgetboolvar("incr_hilight")) incr_hilight_color();
      bus_hilight_hash_lookup(n2, xctx->hilight_color, XINSERT);
      if(tclgetboolvar("incr_hilight")) incr_hilight_color();
   }
 }
 return err;
}

static void set_inst_node(int i, int j, const char *node)
{
  xInstance * const inst = xctx->inst;
  int inst_mult;
  xRect *rect = (inst[i].ptr + xctx->sym)->rect[PINLAYER];
  int skip;

  if(!inst[i].node) return;
  dbg(1, "set_inst_node(): inst %s pin %d <-- %s\n", inst[i].instname, j, node);
  expandlabel(inst[i].instname, &inst_mult);
  my_strdup(_ALLOC_ID_,  &inst[i].node[j], node);
  skip = skip_instance(i, 1, netlist_lvs_ignore);
  if(!for_netlist || skip) {
    bus_node_hash_lookup(inst[i].node[j],"", XINSERT, 0,"","","","");
  } else {
    const char *dir = get_tok_value(rect[j].prop_ptr, "dir",0);
    bus_node_hash_lookup(inst[i].node[j], dir, XINSERT, 0,"","","","");
  }
  if(node && node[0] == '#') { /* update multilicity of unnamed node */
    int pin_mult; 
    expandlabel(get_tok_value(rect[j].prop_ptr, "name", 0), &pin_mult);
    get_unnamed_node(2, pin_mult * inst_mult, atoi((inst[i].node[j]) + 4));
  }
}

static int instcheck(int n, int p);

static int name_attached_inst_to_net(int k, int sqx, int sqy)
{
  int err = 0;
  xInstance * const inst = xctx->inst;
  xWire * const wire = xctx->wire;
  Instpinentry *iptr;
  for(iptr = xctx->instpin_spatial_table[sqx][sqy]; iptr; iptr = iptr->next) {
    int n = iptr->n;
    int p = iptr->pin;
    double x0 = iptr->x0;
    double y0 = iptr->y0;
    int rects=(inst[n].ptr+ xctx->sym)->rects[PINLAYER];
    if(p >= rects) continue; /* generic pins are handled in name_generics() */
    if(!inst[n].node) continue;
    if(touch(wire[k].x1, wire[k].y1, wire[k].x2, wire[k].y2, x0, y0)) {
      if(!inst[n].node[p]) {
        dbg(1, "name_attached_inst_to_net(): inst %s, pin %d <-- %s\n", 
              inst[n].instname, p, wire[k].node ? wire[k].node : "NULL");
        set_inst_node(n, p, wire[k].node);
        err |= instcheck(n, p);
      } else {
        if(for_netlist>0) err |= signal_short("net to named instance pin", wire[k].node, inst[n].node[p]);
      }
    }
  }
  return err;
}

static int wirecheck(int k)    /* recursive routine */
{
  int err = 0;
  int tmpi, tmpj, counti, countj, i, j, touches, x1a, x2a, y1a, y2a;
  double x1, y1, x2, y2;
  Wireentry *wptr;
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
  for(i = x1a; i <= x2a && counti < NBOXES; ++i) {
    ++counti;
    tmpi = i % NBOXES; if(tmpi < 0) tmpi += NBOXES;
    countj = 0;
    for(j = y1a; j <= y2a && countj < NBOXES; ++j) {
      ++countj;
      tmpj = j % NBOXES; if(tmpj < 0) tmpj += NBOXES;
      /*check if wire[k]  touches wires in square [tmpi, tmpj] */
      for(wptr = xctx->wire_spatial_table[tmpi][tmpj]; wptr; wptr = wptr->next) {
        int n = wptr->n;
        if(n == k) { /* itself */
          err |= name_attached_inst_to_net(k, tmpi, tmpj);
          continue;
        } 
        touches = 
          touch(wire[k].x1, wire[k].y1, wire[k].x2, wire[k].y2, wire[n].x1, wire[n].y1) ||
          touch(wire[k].x1, wire[k].y1, wire[k].x2, wire[k].y2, wire[n].x2, wire[n].y2) ||
          touch(wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2, wire[k].x1, wire[k].y1) ||
          touch(wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2, wire[k].x2, wire[k].y2);
        if( touches ) {
          if(!wire[n].node) {
            my_strdup(_ALLOC_ID_, &wire[n].node, wire[k].node);
            my_strdup(_ALLOC_ID_, &wire[n].prop_ptr, subst_token(wire[n].prop_ptr, "lab", wire[n].node));
            err |= name_attached_inst_to_net(n, tmpi, tmpj);
            err |= wirecheck(n); /* recursive check */
          } else {
            if(for_netlist>0) err |= signal_short("Net to net", wire[n].node, wire[k].node);
          }
        }
      }
    }
  }
  return err;
}

static int name_attached_nets(double x0, double y0, int sqx, int sqy, const char *node)
{
  int err = 0;
  xWire * const wire = xctx->wire;
  Wireentry *wptr;
  for(wptr = xctx->wire_spatial_table[sqx][sqy]; wptr; wptr = wptr->next) {
    int n = wptr->n;
    if(touch(wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2, x0,y0)) {
      if(!wire[n].node) {
        my_strdup(_ALLOC_ID_,  &wire[n].node, node);
        my_strdup(_ALLOC_ID_, &wire[n].prop_ptr, subst_token(wire[n].prop_ptr, "lab", wire[n].node));
        err |= wirecheck(n);
      } else {
        if(for_netlist>0) err |= signal_short("Net", wire[n].node, node);
      }
    }
  }
  return err;
}

static int name_attached_inst(int i, double x0, double y0, int sqx, int sqy, const char *node)
{
  int err = 0;
  xInstance * const inst = xctx->inst;
  Instpinentry *iptr;
  for(iptr = xctx->instpin_spatial_table[sqx][sqy]; iptr; iptr = iptr->next) {
    int n = iptr->n;
    int p = iptr->pin;
    int rects=(inst[n].ptr+ xctx->sym)->rects[PINLAYER];
    if(i == n) continue; /* itself -> skip */
    if(p >= rects) continue; /* generic pins are handled in name_generics() */
    if(!inst[n].node) continue;
    if(iptr->x0 == x0 && iptr->y0 == y0 ) {
      if(!inst[n].node[p]) {
        set_inst_node(n, p, node);
        err |= instcheck(n, p);
      } else {
        if(for_netlist>0) err |= signal_short("Instance pin", inst[n].node[p], node);
      }
    }
  }
  return err;
}

int shorted_instance(int i, int lvs_ignore)
{
  xInstance * const inst = xctx->inst;
  xSymbol * const sym = xctx->sym;
  int shorted = 0;

  if(inst[i].ptr < 0) shorted = 0;
  else if(lvs_ignore) {
    if((inst[i].flags & LVS_IGNORE_SHORT) || (sym[inst[i].ptr].flags & LVS_IGNORE_SHORT) ) shorted = 1;
  } else if(xctx->netlist_type == CAD_SPICE_NETLIST) {
    if((inst[i].flags & SPICE_SHORT) || (sym[inst[i].ptr].flags & SPICE_SHORT) ) shorted = 1;
  } else if(xctx->netlist_type == CAD_VERILOG_NETLIST) {
    if((inst[i].flags & VERILOG_SHORT) || (sym[inst[i].ptr].flags & VERILOG_SHORT) ) shorted = 1;
  } else if(xctx->netlist_type == CAD_VHDL_NETLIST) {
    if((inst[i].flags & VHDL_SHORT) || (sym[inst[i].ptr].flags & VHDL_SHORT) ) shorted = 1;
  } else if(xctx->netlist_type == CAD_TEDAX_NETLIST) 
    if((inst[i].flags & TEDAX_SHORT) || (sym[inst[i].ptr].flags & TEDAX_SHORT) ) shorted = 1;

  return shorted;
}

static int skip_instance2(int i, int lvs_ignore, int mask)
{
  int skip = 0;
  if(xctx->inst[i].flags & mask) skip = 1;
  else if(xctx->sym[xctx->inst[i].ptr].flags & mask) skip = 1;
  else if(lvs_ignore && (xctx->inst[i].flags & LVS_IGNORE)) skip = 1;
  else if(lvs_ignore && (xctx->sym[xctx->inst[i].ptr].flags & LVS_IGNORE)) skip = 1;
  return skip;
}

int skip_instance(int i, int skip_short, int lvs_ignore)
{
  int skip = 0;
  if(xctx->inst[i].ptr < 0) skip = 1;
  else if(xctx->netlist_type == CAD_SPICE_NETLIST)
      skip =  skip_instance2(i, lvs_ignore, (skip_short ? SPICE_SHORT : 0) | SPICE_IGNORE);
  else if(xctx->netlist_type == CAD_VERILOG_NETLIST)
      skip =  skip_instance2(i, lvs_ignore, (skip_short ? VERILOG_SHORT : 0) | VERILOG_IGNORE);
  else if(xctx->netlist_type == CAD_VHDL_NETLIST)
      skip =  skip_instance2(i, lvs_ignore, (skip_short ? VHDL_SHORT : 0) | VHDL_IGNORE);
  else if(xctx->netlist_type == CAD_TEDAX_NETLIST)
      skip =  skip_instance2(i, lvs_ignore, (skip_short ? TEDAX_SHORT : 0) | TEDAX_IGNORE);
  else skip = 0;

  dbg(1, "skip_instance(): instance %d skip=%d\n", i, skip);
  return skip;
}


/* what: 
 * Determine if given "ninst" instance has pass-through pins 
 * 0: initialize
 * 1: query
 * 2: cleanup
 */
static int find_pass_through_symbols(int what, int ninst)
{
  int i, j, k;
  xInstance * const inst = xctx->inst;
  int const instances = xctx->instances;
  Int_hashtable table = {NULL, 0};
  static int *pt_symbol = NULL; /* pass-through symbols, symbols with duplicated ports */
  int *symtable = NULL;

  if(what == 0 ) { /* initialize */
    pt_symbol = my_calloc(_ALLOC_ID_, xctx->symbols, sizeof(int));
    symtable = my_calloc(_ALLOC_ID_,  xctx->symbols, sizeof(int));
    for(i = 0; i < instances; ++i) {
      k = inst[i].ptr;
      if( k < 0 || symtable[k] ) continue;
      symtable[k] =1;
      int_hash_init(&table, 37);
      for(j = 0; j < xctx->sym[k].rects[PINLAYER]; ++j) {
        const char *pin_name = get_tok_value(xctx->sym[k].rect[PINLAYER][j].prop_ptr, "name", 0);
        if(int_hash_lookup(&table, pin_name, j, XINSERT_NOREPLACE)) {
          dbg(1, "   pass thru symbol found\n");
          pt_symbol[k] = 1;
        }
      }
      int_hash_free(&table);
      if(pt_symbol[k]) dbg(1, "duplicated pins: %s\n", xctx->sym[k].name);
    }
    my_free(_ALLOC_ID_, &symtable);
  } else if(what ==1) { /* query */
    k = inst[ninst].ptr;
    if(k >=0 && pt_symbol[k]) return 1;
    return 0;
  } else if(what ==2) { /* cleanup */
    my_free(_ALLOC_ID_, &pt_symbol);
  }
  return -1;
}

/* 
 * Given an instance pin (inst n, pin p) propagate electrical information through 
 * other pins with identical "name" attribute (pass-through symbols)
 */
static int instcheck(int n, int p)
{
  int err = 0;
  xInstance * const inst = xctx->inst;
  xSymbol * const sym = xctx->sym;
  int j, sqx, sqy;
  double x0, y0;
  int rects = xctx->sym[inst[n].ptr].rects[PINLAYER];
  int bus_tap = !strcmp(xctx->sym[inst[n].ptr].type, "bus_tap");
  int k = inst[n].ptr;
  int shorted_inst = shorted_instance(n, netlist_lvs_ignore);

  if(!inst[n].node) return 0;

  if( xctx->netlist_type == CAD_VERILOG_NETLIST &&
       ((inst[n].flags & VERILOG_IGNORE) || 
       (k >= 0 && (sym[k].flags & VERILOG_IGNORE))) ) return 0;
       
  if( xctx->netlist_type == CAD_SPICE_NETLIST &&
       ((inst[n].flags & SPICE_IGNORE) || 
       (k >= 0 && (sym[k].flags & SPICE_IGNORE))) ) return 0;
       
  if( xctx->netlist_type == CAD_VHDL_NETLIST &&
       ((inst[n].flags & VHDL_IGNORE) || 
       (k >= 0 &&(sym[k].flags & VHDL_IGNORE))) ) return 0;
       
  if( xctx->netlist_type == CAD_TEDAX_NETLIST &&
       ((inst[n].flags & TEDAX_IGNORE) || 
       (k >= 0 && (sym[k].flags & TEDAX_IGNORE))) ) return 0;

  if( netlist_lvs_ignore &&
       ((inst[n].flags & LVS_IGNORE_OPEN) || 
       (k >= 0 && (sym[k].flags & LVS_IGNORE_OPEN))) ) return 0;

  /* process bus taps : type = bus_tap 
   * node 1 connects to bus (DATA[15:0]) , node 0 is the tap ([2:0]) */
  if(rects > 1 && bus_tap && p == 1) {
    char *node_base_name = NULL;
    const char *tap;
    if(inst[n].node && inst[n].node[0] && inst[n].node[0][0] == '#') {
      my_free(_ALLOC_ID_, &inst[n].node[0]); /* bus tap forces net name on pin 0 (the tap) */
    }                                        /* delete any previously set unnamed net */
    if(!inst[n].node[0]) { /* still unnamed */
      /* tap = get_tok_value(inst[n].prop_ptr, "lab", 0); */
      tap = inst[n].lab;
      /* Check if this is a bus slice and must be appended to bus base name */
      if(tap[0] == '[' || isonlydigit(tap)) {
        /* find bus basename, from beginning or first character after ',' and ' ' */
        char *nptr = strchr(inst[n].node[p], '[');
        if(nptr) {
           while(nptr > inst[n].node[p]) {
             nptr--;
             if(*nptr == ',') {
               while(*(++nptr) ==' ');
               break;
             }
           }
        } else {
          nptr = inst[n].node[p];
        }
        node_base_name = my_malloc(_ALLOC_ID_, strlen(inst[n].node[p]) + 1);
        sscanf(nptr, "%[^[]", node_base_name);
        my_strcat(_ALLOC_ID_, &node_base_name, tap);
      }
      else {
        my_strdup2(_ALLOC_ID_, &node_base_name, tap);
      }
      set_inst_node(n, 0, node_base_name);
      get_inst_pin_coord(n, 0, &x0, &y0);
      get_square(x0, y0, &sqx, &sqy);
      err |= name_attached_nets(x0, y0, sqx, sqy, inst[n].node[0]);
      err |= name_attached_inst(n, x0, y0, sqx, sqy, inst[n].node[0]);
    } else {
      if(for_netlist>0) err |= signal_short("Bus tap", inst[n].node[p], inst[n].node[0]);
    }
    dbg(1, "instcheck: bus tap node: p=%d, %s, tap=%s\n", p, inst[n].node[p], inst[n].node[0]);
    my_free(_ALLOC_ID_, &node_base_name);
  }


  /* should process only symbols with pass-through pins */
  else if(shorted_inst || find_pass_through_symbols(1, n)) {
    int k = inst[n].ptr;
    char *pin_name = NULL;
    my_strdup(_ALLOC_ID_, &pin_name, get_tok_value(xctx->sym[k].rect[PINLAYER][p].prop_ptr, "name", 0));
    if(p >= rects) return 1;
    for(j = 0; j < rects; ++j) {
      const char *other_pin;
      if(j == p) continue;
      if(shorted_inst) {
        other_pin = get_tok_value(xctx->sym[k].rect[PINLAYER][p].prop_ptr, "name", 0);
      } else {
        other_pin = get_tok_value(xctx->sym[k].rect[PINLAYER][j].prop_ptr, "name", 0);
      }
      if(!strcmp(other_pin, pin_name)) {
        dbg(1, "instcheck: inst %s pin %s(%d) <--> pin %s(%d)\n", inst[n].instname, pin_name, p, other_pin, j);
        dbg(1, "instcheck: node: %s\n", inst[n].node[p]);
        if(!inst[n].node[j]) {
          set_inst_node(n, j, inst[n].node[p]);
          get_inst_pin_coord(n, j, &x0, &y0);
          get_square(x0, y0, &sqx, &sqy);
          err |= name_attached_nets(x0, y0, sqx, sqy, inst[n].node[j]);
          err |= name_attached_inst(n, x0, y0, sqx, sqy, inst[n].node[j]);
        } else {
          if(for_netlist>0) err |= signal_short("Pass_through symbol", inst[n].node[p], inst[n].node[j]);
        }
      }
    }
    my_free(_ALLOC_ID_, &pin_name);
  }
  return err;
}

/* starting from labels, ipins, opins, iopins propagate electrical
 * nodes to attached nets and instances
 */
static int name_nodes_of_pins_labels_and_propagate()
{
  int err = 0;
  int i, sqx, sqy;
  double x0, y0;
  int port;
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
  dbg(2, "name_nodes_of_pins_labels_and_propagate(): naming pins from attrs\n");
  /* print_erc is 1 the first time prepare_netlist_structs() is called on top level while
   * doing the netlist, when netlist of sub blocks is completed and toplevel is reloaded
   * a second prepare_netlist_structs() is called to name unnamed nets, in this second call
   * print_erc must be set to 0 to avoid double erc printing
   */
  print_erc =  (xctx->netlist_count == 0 || startlevel < xctx->currsch) && for_netlist;
  for (i=0;i<instances; ++i) {
    /* name ipin opin label node fields from prop_ptr attributes */
    if(skip_instance(i, 0, netlist_lvs_ignore)) continue;
    my_strdup(_ALLOC_ID_, &type,(inst[i].ptr+ xctx->sym)->type);
    if(print_erc && (!inst[i].instname || !inst[i].instname[0]) &&
      !get_tok_value((inst[i].ptr+ xctx->sym)->templ, "name", 0)[0]
        ) {
      char str[2048];
      if(  type &&  /* list of devices that do not have a name= in template attribute */
           strcmp(type, "package") &&  
           strcmp(type, "port_attributes") &&
           strcmp(type, "architecture") &&
           strcmp(type, "arch_declarations") &&
           strcmp(type, "attributes") &&
           strcmp(type, "netlist_options") &&
           strcmp(type, "use")) {
        my_snprintf(str, S(str), "Warning: instance: %s (%s): no name attribute set",
          inst[i].instname, inst[i].name);
        statusmsg(str,2);
        inst[i].color = -PINLAYER;
        xctx->hilight_nets=1;
      }
    }
    if(print_erc && (!type || !type[0]) ) {
      char str[2048];
      my_snprintf(str, S(str), "Warning: Symbol: %s: no type attribute set", inst[i].name);
      statusmsg(str,2);
      inst[i].color = -PINLAYER;
      xctx->hilight_nets=1;
    }
    if(type && inst[i].node && IS_LABEL_OR_PIN(type) ) { /* instance must have a pin! */
      #if 0
      if(for_netlist) {
        /* 20150918 skip labels / pins if ignore property specified on instance */
        if( xctx->netlist_type == CAD_VERILOG_NETLIST && (inst[i].flags & VERILOG_IGNORE)) continue;
        if( xctx->netlist_type == CAD_SPICE_NETLIST && (inst[i].flags & SPICE_IGNORE)) continue;
        if( xctx->netlist_type == CAD_VHDL_NETLIST && (inst[i].flags & VHDL_IGNORE)) continue;
        if( xctx->netlist_type == CAD_TEDAX_NETLIST && (inst[i].flags & TEDAX_IGNORE)) continue;
        if( netlist_lvs_ignore && (inst[i].flags & LVS_IGNORE_OPEN)) continue;
      }
      #endif
      port=0;
      my_strdup2(_ALLOC_ID_, &dir, "");
      if(strcmp(type,"label")) {  /* instance is a port (not a label) */
        port=1;
        /* 20071204 only define a dir property if instance is not a label */
        if(for_netlist)
          my_strdup2(_ALLOC_ID_, &dir,
              get_tok_value( (inst[i].ptr+ xctx->sym)->rect[PINLAYER][0].prop_ptr, "dir",0));
      }
      else {
        /* handle global nodes (global=1 set as symbol property) 28032003 */
        my_strdup(_ALLOC_ID_, &global_node,get_tok_value(inst[i].prop_ptr,"global",0));
        if(!xctx->tok_size) {
          my_strdup(_ALLOC_ID_, &global_node,get_tok_value((inst[i].ptr+ xctx->sym)->prop_ptr,"global",0));
        }
        /*20071204 if instance is a label dont define a dir property for more precise erc checking */
      }
      /* obtain ipin/opin/label signal type (default: std_logic) */
      if(for_netlist) {
        my_strdup(_ALLOC_ID_, &sig_type,get_tok_value(inst[i].prop_ptr,"sig_type",0));
        my_strdup(_ALLOC_ID_, &verilog_type,get_tok_value(inst[i].prop_ptr,"verilog_type",0));
        my_strdup(_ALLOC_ID_, &value,get_tok_value(inst[i].prop_ptr,"value",0));
        my_strdup(_ALLOC_ID_, &class,get_tok_value(inst[i].prop_ptr,"class",0));
      }
      my_strdup(_ALLOC_ID_, &inst[i].node[0], inst[i].lab);
      if(!(inst[i].node[0])) {
        my_strdup(_ALLOC_ID_, &inst[i].node[0], get_tok_value((inst[i].ptr+ xctx->sym)->templ, "lab",0));
        dbg(1, "name_nodes_of_pins_labels_and_propagate(): no lab attr on instance, pick from symbol: %s\n",
                inst[i].node[0]);
      }
      /* handle global nodes (global=1 set as symbol property) 28032003 */
      if(!strcmp(type,"label") && global_node && !strboolcmp(global_node, "true")) {
        dbg(1, "name_nodes_of_pins_labels_and_propagate(): global node: %s\n",inst[i].node[0]);
        record_global_node(1,NULL, inst[i].node[0]);
      }

      /* do not count multiple labels/pins with same name */
      bus_node_hash_lookup(inst[i].node[0],    /* insert node in hash table */
         dir, XINSERT, port, sig_type, verilog_type, value, class);

      get_inst_pin_coord(i, 0, &x0, &y0);
      get_square(x0, y0, &sqx, &sqy);
      /* name nets that touch ioin opin alias instances */
      err |= name_attached_nets(x0, y0, sqx, sqy, inst[i].node[0]);
      /* name instances that touch ioin opin alias instances */
      err |= name_attached_inst(i, x0, y0, sqx, sqy, inst[i].node[0]);
    } /* if(type && ... */
  } /* for(i=0;i<instances... */
  if(dir) my_free(_ALLOC_ID_, &dir);
  if(type) my_free(_ALLOC_ID_, &type);
  if(global_node) my_free(_ALLOC_ID_, &global_node);
  if(for_netlist) {
    if(sig_type) my_free(_ALLOC_ID_, &sig_type);
    if(verilog_type) my_free(_ALLOC_ID_, &verilog_type);
    if(value) my_free(_ALLOC_ID_, &value);
    if(class) my_free(_ALLOC_ID_, &class);
  }
  return err;
}

static int set_unnamed_net(int i)
{
  int err = 0;
  char tmp_str[30];
  my_snprintf(tmp_str, S(tmp_str), "#net%d", get_unnamed_node(1,0,0));
  my_strdup(_ALLOC_ID_, &xctx->wire[i].node, tmp_str);
  my_strdup(_ALLOC_ID_, &xctx->wire[i].prop_ptr, subst_token(xctx->wire[i].prop_ptr, "lab", tmp_str));
  /* insert unnamed wire name in hash table */
  bus_node_hash_lookup(tmp_str, "", XINSERT, 0,"","","","");
  err |= wirecheck(i);
  return err;
}

static int name_unlabeled_nets()
{
  int err = 0;
  int i;
  /* name nets that do not touch ipin opin alias instances */
  dbg(2, "name_unlabeled_nets(): naming nets that dont touch labels\n");
  for (i = 0; i < xctx->wires; ++i)
  {
    if(xctx->wire[i].node == NULL)
    {
      err |= set_unnamed_net(i);
    }
  }
  return err;
}

static int set_unnamed_inst(int i, int j)
{
  int err = 0;
  char tmp_str[30];
  xInstance * const inst = xctx->inst;
  int sqx, sqy;
  double x0, y0;
  my_snprintf(tmp_str, S(tmp_str), "#net%d", get_unnamed_node(1,0,0));
  dbg(1, "set_unnamed_inst(): inst %s pin %d, net %s\n", inst[i].instname, j, tmp_str);
  set_inst_node(i, j, tmp_str);
  get_inst_pin_coord(i, j, &x0, &y0);
  get_square(x0, y0, &sqx, &sqy);
  if(inst[i].node && inst[i].node[j]) err |= name_attached_inst(i, x0, y0, sqx, sqy, inst[i].node[j]);
  return err;
}

static int name_unlabeled_instances()
{ 
  int err = 0;
  int i, j;
  xInstance * const inst = xctx->inst;
  int const instances = xctx->instances;
  int rects;
    
  /* name nets that do not touch ipin opin alias instances */
  dbg(2, "name_unlabeled_instances(): naming nets that dont touch labels\n");
  for (i = 0; i < instances; ++i)
  {
    if(!inst[i].node) continue;
    if(skip_instance(i, 0, netlist_lvs_ignore)) continue;
    if(inst[i].ptr != -1) {
      rects=(inst[i].ptr+ xctx->sym)->rects[PINLAYER];
      for(j = 0; j < rects; ++j) {
        if(inst[i].node[j] == NULL)
        {
          err |= set_unnamed_inst(i, j);
        }
      }
    }
  }
  return err;
} 

static int reset_node_data_and_rehash()
{
  int err = 0;
  int i,j, rects;
  xInstance * const inst = xctx->inst;
  int const instances = xctx->instances;

  /* reset wire & inst node labels */
  dbg(2, "reset_node_data_and_rehash(): rehashing wires and instance pins in spatial hash table\n");
  hash_wires();
  for (i=0;i<instances; ++i)
  {
    if(inst[i].ptr<0) continue;
    rects=(inst[i].ptr+ xctx->sym)->rects[PINLAYER] +
          (inst[i].ptr+ xctx->sym)->rects[GENERICLAYER];
    if(rects > 0)
    {
      inst[i].node = my_malloc(_ALLOC_ID_, sizeof(char *) * rects);
      for (j=0;j<rects; ++j)
      {
        inst[i].node[j]=NULL;
        err |= hash_inst_pin(XINSERT, i, j);
      }
    }
  }
  return err;
}

int prepare_netlist_structs(int for_netl)
{
  int err = 0;
  char nn[PATH_MAX+30];
  netlist_lvs_ignore=tclgetboolvar("lvs_ignore");
  for_netlist = for_netl;
  if(for_netlist>0 && xctx->prep_net_structs) return 0;
  else if(!for_netlist && xctx->prep_hi_structs) return 0;
  
  dbg(1, "prepare_netlist_structs(): extraction: %s\n", xctx->sch[xctx->currsch]);

  reset_caches(); /* update cached flags: necessary if some tcleval() is used for cached attrs */

  set_modify(-2); /* to reset floater cached values */
  /* delete instance pins spatial hash, wires spatial hash, node_hash, wires and inst nodes.*/
  if(for_netlist) {
    my_snprintf(nn, S(nn), "-----------%s", xctx->sch[xctx->currsch]);
    statusmsg(nn,2);
  }
  delete_netlist_structs();
  free_simdata(); /* invalidate simulation cache */
  err |= reset_node_data_and_rehash();
  get_unnamed_node(0,0,0); /*initializes node multiplicity data struct */
  find_pass_through_symbols(0, 0); /* initialize data struct to quickly find pass-through syms */
  err |= name_nodes_of_pins_labels_and_propagate();
  err |= name_unlabeled_nets();
  err |= name_unlabeled_instances();
  name_generics();
  /* name_non_label_inst_pins(); */

  find_pass_through_symbols(2, 0); /* cleanup data */
  rebuild_selected_array();
  if(for_netlist>0) {
    xctx->prep_net_structs=1;
    xctx->prep_hi_structs=1;
  } else xctx->prep_hi_structs=1;
  dbg(1, "prepare_netlist_structs(): returning\n");
  /* avoid below call: it in turn calls prepare_netlist_structs(), too many side effects */
  /* propagate_hilights(1, 0, XINSERT_NOREPLACE);*/
  return err;
}

void delete_inst_node(int i)
{
   int j, rects;
   if( xctx->inst[i].ptr == -1 || !xctx->inst[i].node) return;
   rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER] +
           (xctx->inst[i].ptr+ xctx->sym)->rects[GENERICLAYER];
   if( rects > 0 )
   {
     for(j=0;j< rects ; ++j)
       my_free(_ALLOC_ID_, &xctx->inst[i].node[j]);
     my_free(_ALLOC_ID_, &xctx->inst[i].node );
   }
}

void delete_netlist_structs(void)
{
 int i;
  /* erase node data structures */
   dbg(1, "delete_netlist_structs(): begin erasing\n");
  for(i=0;i<xctx->instances; ++i)
  {
   delete_inst_node(i);
  }
  for(i=0;i<xctx->wires; ++i)
  {
    my_free(_ALLOC_ID_, &xctx->wire[i].node);
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
  int err = 0;
  int i;
  Int_hashtable table = {NULL, 0};
  Int_hashentry *found;
  char str[2048];
  char s[512];

  int_hash_init(&table, HASHSIZE);
  for(i = 0; i < xctx->instances; ++i) {
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
        set_first_sel(ELEMENT, i, 0);
        xctx->need_reb_sel_arr = 1;
      }
      my_snprintf(str, S(str), "Error: overlapped instance found: %s(%s) -> %s\n",
            xctx->inst[i].instname, xctx->inst[i].name, xctx->inst[found->value].instname);
      statusmsg(str,2);
      err |= 1;
    }
  }
  int_hash_free(&table);
  if(sel && xctx->need_reb_sel_arr) rebuild_selected_array();
  return err;
}

/* all: -1: check all symbols, otherwise check only indicated symbol */
int sym_vs_sch_pins(int all)
{
  int err = 0;
  char **lab_array =NULL;
  int lab_array_size = 0;
  int i, j, k, symbol, pin_cnt=0, pin_match, mult;
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
  char f_version[100];
  int start = 0;
  int end = xctx->symbols;
  int n_syms = xctx->symbols;
  if(all >= 0 && all < xctx->symbols) {
    start = all;
    end = all + 1;
  }
  for(i = start; i < end; ++i)
  {
    if( xctx->sym[i].type && !strcmp(xctx->sym[i].type,"subcircuit")) {
      int rects = xctx->sym[i].rects[PINLAYER];
      /* Determine if symbol has pass-through pins (pins with identical name) 
       * if any is found do not check with schematic */
      Int_hashtable pin_table = {NULL, 0};
      int p;
      int unique_pins = 0;
      int_hash_init(&pin_table, HASHSIZE);
      for(p = 0; p < rects; p++) {
        const char *pname = get_tok_value(xctx->sym[i].rect[PINLAYER][p].prop_ptr, "name", 0);
        if(!int_hash_lookup(&pin_table, pname, i, XINSERT_NOREPLACE)) {
          unique_pins++;
        }
      }
      dbg(1, "%s: rects=%d, unique_pins=%d\n", xctx->sym[i].name, rects, unique_pins);
      int_hash_free(&pin_table);
      /* pass through symbols, duplicated pins: do not check with schematic */
      if(rects > unique_pins) continue;
      get_sch_from_sym(filename, xctx->sym + i, -1, 0);
      if(!stat(filename, &buf)) {
        fd = fopen(filename, "r");
        pin_cnt = 0;
        endfile = 0;
        f_version[0] = '\0';
        while(!endfile) {
          if(fscanf(fd," %c",tag)==EOF) break;
          switch(tag[0]) {
            case 'v':
             load_ascii_string(&tmp, fd);
             my_snprintf(f_version, S(f_version), "%s", 
                         get_tok_value(tmp, "file_version", 0));
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
              for(j=0;j<tmpi; ++j) {
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

              if(!strcmp(f_version,"1.0") ) {
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

              /* generators or names with @ characters in general need translate() to process
               * arguments. This can not be done in this context (the current schematic we are looking
               * into is not loaded), so skip test */
              if( strchr(name, '@')) {
                dbg(1, "sym_vs_sch_pins(): symbol reference %s skipped (need to translate() tokens)\n", 
                   name);
                break;
              }

              symbol = match_symbol(name);
              my_strdup(_ALLOC_ID_, &type, xctx->sym[symbol].type);
              if(type && IS_PIN(type)) {
                my_strdup(_ALLOC_ID_, &lab, expandlabel(get_tok_value(tmp, "lab", 0), &mult));
                if(pin_cnt >= lab_array_size) {
                  lab_array_size += CADCHUNKALLOC;
                  my_realloc(_ALLOC_ID_, &lab_array, lab_array_size * sizeof(char *));
                }
                lab_array[pin_cnt] = NULL;
                my_strdup(_ALLOC_ID_, &(lab_array[pin_cnt]), lab);
                pin_cnt++;
                pin_match = 0;
                for(j=0; j < rects; ++j) {
                  my_strdup(_ALLOC_ID_, &pin_name, 
                    expandlabel(get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr, "name", 0), &mult));
                  my_strdup(_ALLOC_ID_, &pin_dir, get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr, "dir", 0));


                  if( pin_name && !strcmp(pin_name, lab)) {
                    if(!pin_dir) {
                      char str[2048];
                      my_snprintf(str, S(str), "Error: Symbol %s: No direction given for pin %s",
                                xctx->sym[i].name, lab);
                      statusmsg(str,2);
                      my_snprintf(str, S(str), "    %s <--> ???", type);
                      statusmsg(str,2);
                      err |= 1;
                    }
                    else if(!(
                          ( !strcmp(type, "ipin") && !strcmp(pin_dir, "in") ) ||
                          ( !strcmp(type, "opin") && !strcmp(pin_dir, "out") ) ||
                          ( !strcmp(type, "iopin") && !strcmp(pin_dir, "inout") )
                        )
                      ) {
                      char str[2048];
                      my_snprintf(str, S(str), "Error: Symbol %s: Unmatched subcircuit schematic pin direction: %s",
                                  xctx->sym[i].name, lab);
                      statusmsg(str,2);
                      my_snprintf(str, S(str), "    %s <--> %s", type, pin_dir);
                      statusmsg(str,2);
                      err |= 1;
                      for(j = 0; j < xctx->instances; ++j) {
                        if(!xctx->x_strcmp(get_sym_name(j, 9999, 1, 0), xctx->sym[i].name)) {
                          xctx->inst[j].color = -PINLAYER;
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
                  my_snprintf(str, S(str), "Error: Symbol %s: schematic pin: %s not in symbol",
                              xctx->sym[i].name, lab);
                  statusmsg(str,2);
                  err |= 1;
                  for(j = 0; j < xctx->instances; ++j) {
                    dbg(1, "inst.name=%s, sym.name=%s\n", tcl_hook2(xctx->inst[j].name), xctx->sym[i].name);
                    if(!xctx->x_strcmp(get_sym_name(j, 9999, 1, 0), xctx->sym[i].name)) {
                      xctx->inst[j].color = -PINLAYER;
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
          if(!f_version[0]) {
            my_snprintf(f_version, S(f_version), "1.0");
            dbg(1, "sym_vs_sch_pins(): no file_version, assuming file_version=%s\n", f_version);
          }
        } /* while(!endfile) */
        fclose(fd);
        if(pin_cnt != rects) {
          char str[2048];
          my_snprintf(str, S(str), "Error: Symbol %s has %d pins, its schematic has %d pins",
                      xctx->sym[i].name, rects, pin_cnt);
          statusmsg(str,2);
          err |= 1;
          for(j = 0; j < xctx->instances; ++j) {
            if(!xctx->x_strcmp(get_sym_name(j, 9999, 1, 0), xctx->sym[i].name)) {
              xctx->inst[j].color = -PINLAYER;
              xctx->hilight_nets=1;
            }
          }
        }
        for(j=0; j < rects; ++j) {
          my_strdup(_ALLOC_ID_, &pin_name,
             expandlabel(get_tok_value(xctx->sym[i].rect[PINLAYER][j].prop_ptr, "name", 0), &mult));
          pin_match = 0;
          for(k=0; k<pin_cnt; ++k) {
            if(pin_name && !strcmp(lab_array[k], pin_name)) {
              pin_match++;
              break;
            }
          }
          if(!pin_match) {
            char str[2048];
            /* fprintf(errfp, "  unmatched sch / sym pin: %s\n", lab); */
            my_snprintf(str, S(str), "Error: Symbol %s: symbol pin: %s not in schematic",
                        xctx->sym[i].name, pin_name ? pin_name : "<NULL>");
            statusmsg(str,2);
            err |= 1;
            for(k = 0; k < xctx->instances; ++k) {
              if(!xctx->x_strcmp(get_sym_name(k, 9999, 1, 0), xctx->sym[i].name)) {
                xctx->inst[k].color = -PINLAYER;
                xctx->hilight_nets=1;
              }
            }
          }
        }
      }
      if(lab_array_size) {
        for(k=0;k<pin_cnt; ++k) {
          my_free(_ALLOC_ID_, &(lab_array[k]));
        }
        my_free(_ALLOC_ID_, &lab_array);
        lab_array_size = 0;
        pin_cnt=0;
      }
    } /* if( ... "subcircuit"... ) */
    my_free(_ALLOC_ID_, &type);
    my_free(_ALLOC_ID_, &tmp);
    my_free(_ALLOC_ID_, &lab);
    my_free(_ALLOC_ID_, &pin_name);
    my_free(_ALLOC_ID_, &pin_dir);
  } /* for(i=0;i<n_syms; ++i) */

  if(all == -1) while(xctx->symbols > n_syms) remove_symbol(xctx->symbols - 1);
  return err;
}
