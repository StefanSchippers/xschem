/* File: hash_iterator.c
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

static int x1a, x2a;
static int y1a, y2a;
static int i, j, counti, countj;
static int tmpi, tmpj;
static struct instentry *instanceptr;
static struct wireentry *wireptr;
static unsigned short *instflag=NULL;
static unsigned short *wireflag=NULL;

void init_inst_iterator(double x1, double y1, double x2, double y2)
{
      dbg(3, "init_inst_iterator(): instances=%d\n", xctx.instances);
      my_realloc(135, &instflag, xctx.instances*sizeof(unsigned short));
      memset(instflag, 0, xctx.instances*sizeof(unsigned short));
      /* calculate square 4 1st corner of drawing area */
      x1a=floor(x1/BOXSIZE) ;
      y1a=floor(y1/BOXSIZE) ;
      /* calculate square 4 2nd corner of drawing area */
      x2a=floor(x2/BOXSIZE);
      y2a=floor(y2/BOXSIZE);
      /* printf("init_inst_iterator(): x1a=%d, y1a=%d\n", x1a, y1a); */
      /* printf("init_inst_iterator(): x2a=%d, y2a=%d\n", x2a, y2a); */
      i = x1a;
      j = y1a;
      tmpi=i%NBOXES; if(tmpi<0) tmpi+=NBOXES;
      tmpj=j%NBOXES; if(tmpj<0) tmpj+=NBOXES;
      counti=0;
      /* printf("init_inst_iterator(): tmpi=%d, tmpj=%d\n", tmpi, tmpj); */
      instanceptr=insttable[tmpi][tmpj];
      countj=0;
}


struct instentry *inst_iterator_next()
{
  struct instentry *ptr;
  dbg(3, "inst_iterator_next(): instances=%d\n", xctx.instances);
  while(1) {
    while(instanceptr) {
      ptr = instanceptr;
      instanceptr = instanceptr -> next;
      if(!instflag[ptr->n]) {
        instflag[ptr->n]=1;
        return ptr;
      }
    }
    if(j<y2a && countj++<NBOXES) {
      j++;
      /* printf("inst_iterator_next(): j=%d\n", j); */
      tmpj=j%NBOXES; if(tmpj<0) tmpj+=NBOXES;
      /* printf("j inst_iterator_next(): tmpi=%d tmpj=%d\n", tmpi, tmpj); */
      instanceptr=insttable[tmpi][tmpj];
    } else if(i<x2a && counti++<NBOXES) {
      i++;
      j=y1a;
      countj=0;
      tmpi=i%NBOXES; if(tmpi<0) tmpi+=NBOXES;
      tmpj=j%NBOXES; if(tmpj<0) tmpj+=NBOXES;
      /* printf("i inst_iterator_next(): tmpi=%d tmpj=%d\n", tmpi, tmpj); */
      instanceptr=insttable[tmpi][tmpj];
    } else {
      my_free(753, &instflag);
      return NULL;
    }
  }
}

void init_wire_iterator(double x1, double y1, double x2, double y2)
{
      dbg(3, "init_wire_iterator(): wires=%d\n", xctx.wires);
      my_realloc(136, &wireflag, xctx.wires*sizeof(unsigned short));
      memset(wireflag, 0, xctx.wires*sizeof(unsigned short));
      /* calculate square 4 1st corner of drawing area */
      x1a=floor(x1/BOXSIZE) ;
      y1a=floor(y1/BOXSIZE) ;
      /* calculate square 4 2nd corner of drawing area */
      x2a=floor(x2/BOXSIZE);
      y2a=floor(y2/BOXSIZE);
      /* printf("init_wire_iterator(): x1a=%d, y1a=%d\n", x1a, y1a); */
      /* printf("init_wire_iterator(): x2a=%d, y2a=%d\n", x2a, y2a); */
      i = x1a;
      j = y1a;
      tmpi=i%NBOXES; if(tmpi<0) tmpi+=NBOXES;
      tmpj=j%NBOXES; if(tmpj<0) tmpj+=NBOXES;
      counti=0;
      /* printf("init_inst_iterator(): tmpi=%d, tmpj=%d\n", tmpi, tmpj); */
      wireptr=wiretable[tmpi][tmpj];
      countj=0;
}


struct wireentry *wire_iterator_next()
{
  struct wireentry *ptr;
  dbg(3, "wire_iterator_next(): wires=%d\n", xctx.wires);
  while(1) {
    while(wireptr) {
      ptr = wireptr;
      wireptr = wireptr -> next;
      if(!wireflag[ptr->n]) {
        wireflag[ptr->n]=1;
        return ptr;
      }
    }
    if(j<y2a && countj++<NBOXES) {
      j++;
      tmpj=j%NBOXES; if(tmpj<0) tmpj+=NBOXES;
      wireptr=wiretable[tmpi][tmpj];
    } else if(i<x2a && counti++<NBOXES) {
      i++;
      j=y1a;
      countj=0;
      tmpi=i%NBOXES; if(tmpi<0) tmpi+=NBOXES;
      tmpj=j%NBOXES; if(tmpj<0) tmpj+=NBOXES;
      wireptr=wiretable[tmpi][tmpj];
    } else {
      my_free(754, &wireflag);
      return NULL;
    }
  }
}


