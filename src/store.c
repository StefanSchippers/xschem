/* File: store.c
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


void check_wire_storage(void)
{
 if(xctx->wires >= xctx->maxw)
 {
  xctx->maxw=(1+xctx->wires / CADMAXWIRES)*CADMAXWIRES;
  my_realloc(_ALLOC_ID_, &xctx->wire, sizeof(xWire)*xctx->maxw);
 }
}

void check_selected_storage(void)
{
 if(xctx->lastsel >= xctx->maxsel)
 {
  xctx->maxsel=(1+xctx->lastsel / MAXGROUP) * MAXGROUP;
  my_realloc(_ALLOC_ID_, &xctx->sel_array, sizeof(Selected)*xctx->maxsel);
 }
}

void check_text_storage(void)
{
 if(xctx->texts >= xctx->maxt)
 {
  xctx->maxt=(1 + xctx->texts / CADMAXTEXT) * CADMAXTEXT;
  my_realloc(_ALLOC_ID_, &xctx->text, sizeof(xText)*xctx->maxt);
 }
}

void check_symbol_storage(void)
{
 if(xctx->symbols >= xctx->maxs)
 {
  dbg(1, "check_symbol_storage(): more than maxs, %s\n",
        xctx->sch[xctx->currsch] );
  xctx->maxs=(1 + xctx->symbols / ELEMDEF) * ELEMDEF;
  my_realloc(_ALLOC_ID_, &xctx->sym, sizeof(xSymbol)*xctx->maxs);
 }

}

#undef ZERO_REALLOC

void check_inst_storage(void)
{
 if(xctx->instances >= xctx->maxi)
 {
  int i, old = xctx->maxi;
  
  xctx->maxi=(1 + xctx->instances / ELEMINST) * ELEMINST;
  my_realloc(_ALLOC_ID_, &xctx->inst, sizeof(xInstance)*xctx->maxi);
  #ifdef ZERO_REALLOC
  memset(xctx->inst + xctx->instances, 0, sizeof(xInstance) * (xctx->maxi - xctx->instances));
  #endif
  /* clear all flag bits (to avoid random data in bit 8, that can not be cleraed 
   * by set_inst_flags() */
  for(i = old; i < xctx->maxi; i++) xctx->inst[i].flags = 0;
 }
}

void check_arc_storage(int c)
{
 if(xctx->arcs[c] >= xctx->maxa[c])
 {
  xctx->maxa[c]=(1 + xctx->arcs[c] / CADMAXOBJECTS) * CADMAXOBJECTS;
  my_realloc(_ALLOC_ID_, &xctx->arc[c], sizeof(xArc)*xctx->maxa[c]);
  #ifdef ZERO_REALLOC
  memset(xctx->arc[c] + xctx->arcs[c], 0, sizeof(xArc) * (xctx->maxa[c] - xctx->arcs[c]));
  #endif
 }
}

void check_box_storage(int c)
{
 if(xctx->rects[c] >= xctx->maxr[c])
 {
  xctx->maxr[c]=(1 + xctx->rects[c] / CADMAXOBJECTS) * CADMAXOBJECTS;
  my_realloc(_ALLOC_ID_, &xctx->rect[c], sizeof(xRect)*xctx->maxr[c]);
  #ifdef ZERO_REALLOC
  memset(xctx->rect[c] + xctx->rects[c], 0, sizeof(xRect) * (xctx->maxr[c] - xctx->rects[c]));
  #endif
 }
}

void check_line_storage(int c)
{
 if(xctx->lines[c] >= xctx->maxl[c])
 {
  xctx->maxl[c]=(1 + xctx->lines[c] / CADMAXOBJECTS) * CADMAXOBJECTS;
  my_realloc(_ALLOC_ID_, &xctx->line[c], sizeof(xLine)*xctx->maxl[c]);
  #ifdef ZERO_REALLOC
  memset(xctx->line[c] + xctx->lines[c], 0, sizeof(xLine) * (xctx->maxl[c] - xctx->lines[c]));
  #endif
 }
}

void check_polygon_storage(int c)
{
 if(xctx->polygons[c] >= xctx->maxp[c])
 {
  xctx->maxp[c]=(1 + xctx->polygons[c] / CADMAXOBJECTS) * CADMAXOBJECTS;
  my_realloc(_ALLOC_ID_, &xctx->poly[c], sizeof(xPoly)*xctx->maxp[c]);
  #ifdef ZERO_REALLOC
  memset(xctx->poly[c] + xctx->polygons[c], 0, sizeof(xPoly) * (xctx->maxp[c] - xctx->polygons[c]));
  #endif
 }
}

void store_arc(int pos, double x, double y, double r, double a, double b,
               unsigned int rectc, unsigned short sel, const char *prop_ptr)
{
  int n, j;
  const char *dash, *fill_ptr;
  check_arc_storage(rectc);
  if(pos==-1) n=xctx->arcs[rectc];
  else
  {
   for(j=xctx->arcs[rectc];j>pos;j--)
   {
    xctx->arc[rectc][j]=xctx->arc[rectc][j-1];
   }
   n=pos;
  }
  xctx->arc[rectc][n].x = x;
  xctx->arc[rectc][n].y = y;
  xctx->arc[rectc][n].r = r;
  xctx->arc[rectc][n].a = a;
  xctx->arc[rectc][n].b = b;
  xctx->arc[rectc][n].prop_ptr = NULL;
  my_strdup(_ALLOC_ID_, &xctx->arc[rectc][n].prop_ptr, prop_ptr);
  xctx->arc[rectc][n].sel = sel;
  if(sel == SELECTED) set_first_sel(ARC, n, rectc);

  fill_ptr = get_tok_value(xctx->arc[rectc][n].prop_ptr,"fill",0);
  if(!strcmp(fill_ptr, "full") )
    xctx->arc[rectc][n].fill = 2; /* bit 1: solid fill (not stippled) */
  else if(!strboolcmp(fill_ptr, "true") )
    xctx->arc[rectc][n].fill = 1;
  else
    xctx->arc[rectc][n].fill = 0;
  dash = get_tok_value(xctx->arc[rectc][n].prop_ptr,"dash",0);
  if( strcmp(dash, "") ) {
    int d = atoi(dash);
    xctx->arc[rectc][n].dash = (char) (d >= 0 ? d : 0);
  } else
    xctx->arc[rectc][n].dash = 0;

  xctx->arcs[rectc]++;
}

void store_poly(int pos, double *x, double *y, int points, unsigned int rectc, 
                unsigned short sel, char *prop_ptr)
{
  int n, j;
  const char *dash, *fill_ptr;
  check_polygon_storage(rectc);
  if(pos==-1) n=xctx->polygons[rectc];
  else
  {
   for(j=xctx->polygons[rectc];j>pos;j--)
   {
    xctx->poly[rectc][j]=xctx->poly[rectc][j-1];
   }
   n=pos;
  }
  dbg(2, "store_poly(): storing POLYGON %d\n",n);

  xctx->poly[rectc][n].x=NULL;
  xctx->poly[rectc][n].y=NULL;
  xctx->poly[rectc][n].selected_point=NULL;
  xctx->poly[rectc][n].prop_ptr=NULL;
  xctx->poly[rectc][n].x= my_calloc(_ALLOC_ID_, points, sizeof(double));
  xctx->poly[rectc][n].y= my_calloc(_ALLOC_ID_, points, sizeof(double));
  xctx->poly[rectc][n].selected_point= my_calloc(_ALLOC_ID_, points, sizeof(unsigned short));
  my_strdup(_ALLOC_ID_, &xctx->poly[rectc][n].prop_ptr, prop_ptr);
  for(j=0;j<points; ++j) {
    xctx->poly[rectc][n].x[j] = x[j];
    xctx->poly[rectc][n].y[j] = y[j];
  }
  xctx->poly[rectc][n].points = points;
  xctx->poly[rectc][n].sel = sel;
  if(sel == SELECTED) set_first_sel(POLYGON, n, rectc);

  fill_ptr = get_tok_value(xctx->poly[rectc][n].prop_ptr,"fill",0);
  if(!strcmp(fill_ptr, "full") )
    xctx->poly[rectc][n].fill = 2; /* bit 1: solid fill (not stippled) */
  else if(!strboolcmp(fill_ptr, "true") )
    xctx->poly[rectc][n].fill = 1;
  else
    xctx->poly[rectc][n].fill = 0;
  dash = get_tok_value(xctx->poly[rectc][n].prop_ptr,"dash",0);
  if( strcmp(dash, "") ) {
    int d = atoi(dash);
    xctx->poly[rectc][n].dash = (char) (d >= 0 ? d : 0);
  } else
    xctx->poly[rectc][n].dash = 0;


  xctx->polygons[rectc]++;
}

int storeobject(int pos, double x1,double y1,double x2,double y2,
                 unsigned short type, unsigned int rectc,
                 unsigned short sel, const char *prop_ptr)
{
 int n, j, modified = 0;
 const char *attr, *fill_ptr;
    if(type == LINE)
    {
     check_line_storage(rectc);

     if(pos==-1) n=xctx->lines[rectc];
     else
     {
      for(j=xctx->lines[rectc];j>pos;j--)
      {
       xctx->line[rectc][j]=xctx->line[rectc][j-1];
      }
      n=pos;
     }
     dbg(2, "storeobject(): storing LINE %d\n",n);
     xctx->line[rectc][n].x1=x1;
     xctx->line[rectc][n].x2=x2;
     xctx->line[rectc][n].y1=y1;
     xctx->line[rectc][n].y2=y2;
     xctx->line[rectc][n].prop_ptr=NULL;
     my_strdup(_ALLOC_ID_, &xctx->line[rectc][n].prop_ptr, prop_ptr);
     xctx->line[rectc][n].sel=sel;
     if(sel == SELECTED) set_first_sel(LINE, n, rectc);
     xctx->line[rectc][n].bus = 0;
     if(prop_ptr) {
        xctx->line[rectc][n].bus = get_attr_val(get_tok_value(prop_ptr, "bus", 0));
     }
     if(prop_ptr && (attr = get_tok_value(prop_ptr,"dash",0))[0]) {
       int d = atoi(attr);
       xctx->line[rectc][n].dash = (char) (d >= 0 ? d : 0);
     } else
       xctx->line[rectc][n].dash = 0;
     xctx->lines[rectc]++;
     modified = 1;
    }
    if(type == xRECT)
    {
     check_box_storage(rectc);
     if(pos==-1) n=xctx->rects[rectc];
     else
     {
      for(j=xctx->rects[rectc];j>pos;j--)
      {
       xctx->rect[rectc][j]=xctx->rect[rectc][j-1];
      }
      n=pos;
     }
     dbg(2, "storeobject(): storing RECT %d\n",n);
     xctx->rect[rectc][n].x1=x1;
     xctx->rect[rectc][n].x2=x2;
     xctx->rect[rectc][n].y1=y1;
     xctx->rect[rectc][n].y2=y2;
     xctx->rect[rectc][n].prop_ptr=NULL;
     xctx->rect[rectc][n].extraptr=NULL;
     my_strdup(_ALLOC_ID_, &xctx->rect[rectc][n].prop_ptr, prop_ptr);
     xctx->rect[rectc][n].sel=sel;
     if(sel == SELECTED) set_first_sel(xRECT, n, rectc);

     if(prop_ptr && (attr = get_tok_value(prop_ptr,"dash",0))[0]) {
       int d = atoi(attr);
       xctx->rect[rectc][n].dash = (char) (d >= 0 ? d : 0);
     } else
       xctx->rect[rectc][n].dash = 0;

     if(prop_ptr && (attr = get_tok_value(prop_ptr,"ellipse",0))[0]) {
       int a;
       int b;
       if(sscanf(attr, "%d%*[ ,]%d", &a, &b) != 2) {
         a = 0;
         b = 360;
       }
       xctx->rect[rectc][n].ellipse_a = a;
       xctx->rect[rectc][n].ellipse_b = b;
     } else {
       xctx->rect[rectc][n].ellipse_a = -1;
       xctx->rect[rectc][n].ellipse_b = -1;
     }

     fill_ptr = get_tok_value(xctx->rect[rectc][n].prop_ptr, "fill", 0);
     if(!strcmp(fill_ptr, "full") )
       xctx->rect[rectc][n].fill = 2;
     else if(!strboolcmp(fill_ptr,"false") )
       xctx->rect[rectc][n].fill = 0;
     else
       xctx->rect[rectc][n].fill = 1;
     set_rect_flags(&xctx->rect[rectc][n]); /* set cached .flags bitmask from on attributes */
     if(rectc == GRIDLAYER && (xctx->rect[rectc][n].flags & 1024)) {
        xRect *r = &xctx->rect[GRIDLAYER][n];
        draw_image(0, r, &r->x1, &r->y1, &r->x2, &r->y2, 0, 0);
     }
     xctx->rects[rectc]++;
     modified = 1;
    }
    if(type == WIRE)
    {
     check_wire_storage();
     if(pos==-1) n=xctx->wires;
     else
     {
      for(j=xctx->wires;j>pos;j--)
      {
       xctx->wire[j]=xctx->wire[j-1];
      }
      n=pos;
     }
     dbg(2, "storeobject(): storing WIRE %d\n",n);
     xctx->wire[n].x1=x1;
     xctx->wire[n].y1=y1;
     xctx->wire[n].x2=x2;
     xctx->wire[n].y2=y2;
     xctx->wire[n].prop_ptr=NULL;
     xctx->wire[n].node=NULL;
     xctx->wire[n].end1=0;
     xctx->wire[n].end2=0;
     my_strdup(_ALLOC_ID_, &xctx->wire[n].prop_ptr, prop_ptr);
     xctx->wire[n].bus = 0;
     if(prop_ptr) {
       xctx->wire[n].bus = get_attr_val(get_tok_value(prop_ptr,"bus",0));
     }
     xctx->wire[n].sel=sel;
     if(sel == SELECTED) set_first_sel(WIRE, n, 0);
     xctx->wires++;
     modified = 1;
    }
    return modified;
}
