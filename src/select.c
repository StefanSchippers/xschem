/* File: select.c
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

/* select all nets and pins/labels that are *physically* connected to current selected wire segments
 * stop_at_junction==1 --> stop selecting wires at 'T' junctions
 * stop_at_junction==2 --> select only wires directly attached to selected net/instance
 * Recursive routine
 */
static void check_connected_nets(int stop_at_junction, int n)
{ 
  int k, touches;
  xWire * const wire = xctx->wire;
  Wireentry *wireptr;
  Instentry *instptr;
  char *type;
  double x1, y1, x2, y2;
  Iterator_ctx ctx;
  
  x1 = wire[n].x1;
  y1 = wire[n].y1;
  x2 = wire[n].x2;
  y2 = wire[n].y2;
  RECTORDER(x1, y1, x2, y2);
  dbg(1, "check_connected_nets(): n=%d, %g %g %g %g\n", n, x1, y1, x2, y2);
  for(init_inst_iterator(&ctx, x1, y1, x2, y2); (instptr = inst_iterator_next(&ctx)) ;) {
    k = instptr->n;
    type = (xctx->inst[k].ptr+ xctx->sym)->type;
    if( type && (IS_LABEL_SH_OR_PIN(type) || !strcmp(type, "probe") )) {
      double x0, y0;
      xRect *rct;
      rct=(xctx->inst[k].ptr+ xctx->sym)->rect[PINLAYER];
      if(rct) {
        get_inst_pin_coord(k, 0, &x0, &y0);
        touches = touch(wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2, x0, y0);
        if(touches) {
          xctx->need_reb_sel_arr=1;
          xctx->inst[k].sel = SELECTED;
        }
      }
    }
  }
  if(!(stop_at_junction & 2)) {
    for(init_wire_iterator(&ctx, x1, y1, x2, y2); (wireptr = wire_iterator_next(&ctx)) ;) {
      k = wireptr->n;
      if(n == k || xctx->wire[k].sel == SELECTED) continue;




      if((stop_at_junction  & 1)== 0) {
        touches = touch(wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2, wire[k].x1, wire[k].y1) ||
                  touch(wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2, wire[k].x2, wire[k].y2) ||
                  touch(wire[k].x1, wire[k].y1, wire[k].x2, wire[k].y2, wire[n].x1, wire[n].y1) ||
                  touch(wire[k].x1, wire[k].y1, wire[k].x2, wire[k].y2, wire[n].x2, wire[n].y2);
      } else {
        touches = (wire[n].x1 == wire[k].x1 && wire[n].y1 == wire[k].y1 && wire[n].end1 < 2 && wire[k].end1 < 2) ||
                  (wire[n].x1 == wire[k].x2 && wire[n].y1 == wire[k].y2 && wire[n].end1 < 2 && wire[k].end2 < 2) ||
                  (wire[n].x2 == wire[k].x1 && wire[n].y2 == wire[k].y1 && wire[n].end2 < 2 && wire[k].end1 < 2) ||
                  (wire[n].x2 == wire[k].x2 && wire[n].y2 == wire[k].y2 && wire[n].end2 < 2 && wire[k].end2 < 2);
      } 
      if(touches) {
        xctx->wire[k].sel = SELECTED;
        check_connected_nets(stop_at_junction, k); /* recursive check */
      }
    }
  }
}

/* stop_at_junction==1 --> stop selecting wires at 'T' junctions 
 * stop_at_junction==2 --> select only wires directly attached to selected net/instance
 */
void select_connected_nets(int stop_at_junction)
{
  int i, n;
  int netlist_lvs_ignore=tclgetboolvar("lvs_ignore");

  if(stop_at_junction & 1) trim_wires();
  hash_wires();
  hash_instances();
  rebuild_selected_array(); /* does nothing as already done in most of use cases */
  for(n=0; n<xctx->lastsel; ++n) {
    i = xctx->sel_array[n].n;
    switch(xctx->sel_array[n].type) {
      double x0, y0;
      int sqx, sqy, p, rects;
      Wireentry *wptr;
      Instentry *iptr;

      case WIRE:
        if(xctx->wire[i].sel == SELECTED) check_connected_nets(stop_at_junction, i);
        break;
      case ELEMENT:
        rects = (xctx->inst[i].ptr + xctx->sym)->rects[PINLAYER];
        for(p = 0; p < rects; p++)
        {
          get_inst_pin_coord(i, p, &x0, &y0);
          get_square(x0, y0, &sqx, &sqy);
          for(wptr = xctx->wire_spatial_table[sqx][sqy]; wptr; wptr = wptr->next) {
             dbg(1, "select_connected_nets(): x0=%g y0=%g wire[%d]=%g %g %g %g\n",
                 x0, y0, wptr->n, xctx->wire[wptr->n].x1, xctx->wire[wptr->n].y1,
                                  xctx->wire[wptr->n].x2, xctx->wire[wptr->n].y2);
             if (touch(xctx->wire[wptr->n].x1, xctx->wire[wptr->n].y1,
                 xctx->wire[wptr->n].x2, xctx->wire[wptr->n].y2, x0,y0)) {
               xctx->wire[wptr->n].sel = SELECTED;
               check_connected_nets(stop_at_junction, wptr->n);
             }
          }
          for(iptr = xctx->inst_spatial_table[sqx][sqy]; iptr; iptr = iptr->next) {
            int rects, p, touches;
            char *type;
            xRect *rct;
            int n = iptr->n;
            double x1, y1;
  
            if(n == i) continue;
            type = (xctx->inst[n].ptr+ xctx->sym)->type;
            if( type && (!strcmp(type, "label") || !strcmp(type, "probe")) ) {
              if(skip_instance(n, 0, netlist_lvs_ignore)) continue;
              rct = (xctx->inst[n].ptr+ xctx->sym)->rect[PINLAYER];
              if(!rct) continue;
              rects = (xctx->inst[n].ptr + xctx->sym)->rects[PINLAYER];
              for(p = 0; p < rects; p++)
              {
                get_inst_pin_coord(n, p, &x1, &y1);
                touches = (x0 == x1 && y0 == y1);
                if(touches) {
                  xctx->inst[n].sel = SELECTED;
                }
              }
            }
          }
        } /* for(p...) */
        break;
      default:
        break;
    } /* switch(...) */
  } /* for(... lastsel ...) */
  xctx->need_reb_sel_arr=1;
  rebuild_selected_array();
  draw_selection(xctx->gc[SELLAYER], 0);
}

int select_dangling_nets(void)
{
  int netlist_lvs_ignore=tclgetboolvar("lvs_ignore");
  int i, p, w, touches, rects, done;
  int ret = 0; /* return code: 1: dangling elements found */
  int *table = NULL;
  xRect *rct;
  xWire * const wire = xctx->wire;
  Instentry *instptr;
  Wireentry *wireptr;
  Iterator_ctx ctx;
  char *type;
  double x0, y0, x1, y1, x2, y2;

  table = my_calloc(_ALLOC_ID_, xctx->wires, sizeof(int));
  
  hash_instances();
  hash_wires();

  /* Mark nets connected to non pin/label/probe components as NOT dangling (table[w] = 1) */
  for(w = 0; w < xctx->wires; w++) {
    x1 = xctx->wire[w].x1;
    y1 = xctx->wire[w].y1;
    x2 = xctx->wire[w].x2;
    y2 = xctx->wire[w].y2;
    RECTORDER(x1, y1, x2, y2);
    for(init_inst_iterator(&ctx, x1, y1, x2, y2); (instptr = inst_iterator_next(&ctx)) ;) {
      i = instptr->n;
      if(skip_instance(i, 0, netlist_lvs_ignore)) continue;
      type = (xctx->inst[i].ptr+ xctx->sym)->type;
      rct=(xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER];
      if(!rct) continue;
      if( type && (!strcmp(type, "label") || !strcmp(type, "probe") )) {
        continue;
      }
      rects = (xctx->inst[i].ptr + xctx->sym)->rects[PINLAYER];
      for(p = 0; p < rects; p++)
      {
        get_inst_pin_coord(i, p, &x0, &y0);
        touches = touch(xctx->wire[w].x1, xctx->wire[w].y1, xctx->wire[w].x2, xctx->wire[w].y2, x0, y0);
        if(touches) {
          table[w] = 1; /* wire[w] is NOT dangling */
          dbg(1, "wire %d touches inst %d\n", w, i);
        }
      }
    }
  }

  /* Propagate NOT dangling nets to other nets */
  done = 0;
  while(!done) {
    done = 1;
    for(w = 0; w < xctx->wires; w++) {
      if(table[w] == 0) continue;
      /* wire[w] is not dangling */
      x1 = xctx->wire[w].x1;
      y1 = xctx->wire[w].y1;
      x2 = xctx->wire[w].x2;
      y2 = xctx->wire[w].y2;
      RECTORDER(x1, y1, x2, y2);
      for(init_wire_iterator(&ctx, x1, y1, x2, y2); (wireptr = wire_iterator_next(&ctx)) ;) {
        i = wireptr->n;
        if(i == w) continue;
        if(table[i]) continue;
        touches = touch(wire[w].x1, wire[w].y1, wire[w].x2, wire[w].y2, wire[i].x1, wire[i].y1) ||
                  touch(wire[w].x1, wire[w].y1, wire[w].x2, wire[w].y2, wire[i].x2, wire[i].y2) ||
                  touch(wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2, wire[w].x1, wire[w].y1) ||
                  touch(wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2, wire[w].x2, wire[w].y2);
        if(touches) { 
          table[i] = 1; /* wire[i] also not dangling */
          done = 0;
        }
      }
    }
  }

  /* Select all nets that are dangling */
  for(w = 0; w < xctx->wires; w++) {
    if(!table[w]) {
      xctx->wire[w].sel = SELECTED;
      ret = 1;
    }
  }

  /* select pins/labels/probes attached to dangling nets */
  for(w = 0; w < xctx->wires; w++) {
    if(table[w]) continue;
    x1 = xctx->wire[w].x1;
    y1 = xctx->wire[w].y1;
    x2 = xctx->wire[w].x2;
    y2 = xctx->wire[w].y2;
    RECTORDER(x1, y1, x2, y2);
    for(init_inst_iterator(&ctx, x1, y1, x2, y2); (instptr = inst_iterator_next(&ctx)) ;) {
      i = instptr->n;
      type = (xctx->inst[i].ptr+ xctx->sym)->type;
      if( type && (!strcmp(type, "label") || !strcmp(type, "probe") )) {
        rct = (xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER];
        if(!rct) continue;
        get_inst_pin_coord(i, 0, &x0, &y0);
        touches = touch(xctx->wire[w].x1, xctx->wire[w].y1, xctx->wire[w].x2, xctx->wire[w].y2, x0, y0);
        if(touches) {
          xctx->inst[i].sel = SELECTED;
          ret = 1;
        }
      }
    }
  }

  /* select dangling labels/probes (not connected to anything) */
  for(i = 0; i < xctx->instances; i++) {
    int dangling = 1;
    if(xctx->inst[i].sel == SELECTED) continue;
    type = (xctx->inst[i].ptr+ xctx->sym)->type;
    if( type && (!strcmp(type, "label") || !strcmp(type, "probe")) ) {
      int sqx, sqy;
      rct = (xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER];
      if(!rct) continue;
      get_inst_pin_coord(i, 0, &x0, &y0);
      get_square(x0, y0, &sqx, &sqy);
      for(wireptr = xctx->wire_spatial_table[sqx][sqy]; wireptr; wireptr = wireptr->next) {
        int n = wireptr->n;
        if (touch(xctx->wire[n].x1, xctx->wire[n].y1, xctx->wire[n].x2, xctx->wire[n].y2, x0, y0)) {
          dangling = 0; /* inst[i] connected to a wire */
        }
      }
      for(instptr = xctx->inst_spatial_table[sqx][sqy]; instptr; instptr = instptr->next) {
        int n = instptr->n;
        if(n == i) continue;
        type = (xctx->inst[n].ptr+ xctx->sym)->type;
        if( type && (!strcmp(type, "label") || !strcmp(type, "probe")) ) continue;
        if(skip_instance(n, 0, netlist_lvs_ignore)) continue;
        rct = (xctx->inst[n].ptr+ xctx->sym)->rect[PINLAYER];
        if(!rct) continue;
        rects = (xctx->inst[n].ptr + xctx->sym)->rects[PINLAYER];
        for(p = 0; p < rects; p++)
        { 
          get_inst_pin_coord(n, p, &x1, &y1);
          touches = (x0 == x1 && y0 == y1);
          if(touches) {
            dangling = 0; /* inst[i] connected to non label/probe inst[n] */
          }
        }
      }
      if(dangling) {
        xctx->inst[i].sel = SELECTED;
        ret = 1;
      }
    }
  }

  /* draw selection */
  xctx->need_reb_sel_arr = 1;
  rebuild_selected_array();
  draw_selection(xctx->gc[SELLAYER], 0);
  my_free(_ALLOC_ID_, &table);
  return ret;
}

void symbol_bbox(int i, double *x1,double *y1, double *x2, double *y2)
{
   int j, tmp;
   xText text;
   const char *tmp_txt;
   short rot,flip;
   double x0, y0, dtmp;
   double text_x0, text_y0;
   short sym_rot, sym_flip;
   double xx1,yy1,xx2,yy2;
   #if HAS_CAIRO==1
   int customfont;
   #endif
   /* symbol bbox */
   flip = xctx->inst[i].flip;
   rot = xctx->inst[i].rot;
   x0=xctx->inst[i].x0;
   y0=xctx->inst[i].y0;
   ROTATION(rot, flip, 0.0,0.0,(xctx->inst[i].ptr+ xctx->sym)->minx,
                    (xctx->inst[i].ptr+ xctx->sym)->miny,*x1,*y1);
   ROTATION(rot, flip, 0.0,0.0,(xctx->inst[i].ptr+ xctx->sym)->maxx,
                    (xctx->inst[i].ptr+ xctx->sym)->maxy,*x2,*y2);
   RECTORDER(*x1,*y1,*x2,*y2);
   *x1+=x0;*y1+=y0;
   *x2+=x0;*y2+=y0;
   xctx->inst[i].xx1 = *x1;               /* 20070314 added bbox without text */
   xctx->inst[i].yy1 = *y1;               /* for easier select */
   xctx->inst[i].xx2 = *x2;
   xctx->inst[i].yy2 = *y2;
   dbg(2, "symbol_bbox(): instance=%s %.16g %.16g %.16g %.16g\n",xctx->inst[i].instname,*x1, *y1, *x2, *y2);
   /* strings bbox */
   for(j=0;j< (xctx->inst[i].ptr+ xctx->sym)->texts; ++j)
   {
     text = (xctx->inst[i].ptr+ xctx->sym)->text[j];
     if(!xctx->show_hidden_texts && (text.flags & HIDE_TEXT)) continue;
     sym_flip = flip;
     sym_rot = rot;
     /* dbg(2, "symbol_bbox(): instance %d text n: %d text str=%s\n", i,j, text.txt_ptr? text.txt_ptr:"NULL"); */
     tmp_txt = translate(i, text.txt_ptr);
     /* dbg(2, "symbol_bbox(): translated text: %s\n", tmp_txt); */
     ROTATION(rot, flip, 0.0,0.0,text.x0, text.y0,text_x0,text_y0);
     #if HAS_CAIRO==1
     customfont=set_text_custom_font(&text);
     #endif
     text_bbox(tmp_txt, text.xscale, text.yscale,
       (text.rot + ( (sym_flip && (text.rot & 1) ) ? sym_rot+2 : sym_rot)) &0x3,
       sym_flip ^ text.flip, text.hcenter, text.vcenter,
       x0+text_x0,y0+text_y0, &xx1,&yy1,&xx2,&yy2, &tmp, &dtmp);
     dbg(1, "symbol bbox: text bbox: %s, %g %g %g %g\n", tmp_txt, xx1, yy1, xx2, yy2);
     dbg(1, "symbol bbox: text bbox: zoom=%g, lw=%g\n", xctx->zoom, xctx->lw);
     #if HAS_CAIRO==1
     if(customfont) {
       cairo_restore(xctx->cairo_ctx);
     }
     #endif
     if(xx1<*x1) *x1=xx1;
     if(yy1<*y1) *y1=yy1;
     if(xx2>*x2) *x2=xx2;
     if(yy2>*y2) *y2=yy2;
   }
   dbg(1, "symbol_bbox(): instance=%s %.16g %.16g %.16g %.16g\n", xctx->inst[i].instname, *x1, *y1, *x2, *y2);
}


static void del_rect_line_arc_poly()
{
 xRect tmp;
 int c, j, i;
 int deleted = 0;

 for(c=0;c<cadlayers; ++c)
 {
  j = 0;
  for(i=0;i<xctx->rects[c]; ++i)
  {
   if(xctx->rect[c][i].sel == SELECTED)
   {
    if(c == GRIDLAYER) xctx->graph_lastsel = -1; /* invalidate last selected graph */
    ++j;
    my_free(_ALLOC_ID_, &xctx->rect[c][i].prop_ptr);
    set_rect_extraptr(0, &xctx->rect[c][i]);
    deleted = 1;
    continue;
   }
   if(j)
   {
    xctx->rect[c][i-j] = xctx->rect[c][i];
   }
  }
  xctx->rects[c] -= j;
  j = 0;
  for(i=0;i<xctx->lines[c]; ++i)
  {
   if(xctx->line[c][i].sel == SELECTED)
   {
    ++j;
    deleted = 1;
    my_free(_ALLOC_ID_, &xctx->line[c][i].prop_ptr);
    continue;
   }
   if(j)
   {
    xctx->line[c][i-j] = xctx->line[c][i];
   }
  }
  xctx->lines[c] -= j;

  j = 0;
  for(i=0;i<xctx->arcs[c]; ++i)
  {
   if(xctx->arc[c][i].sel == SELECTED)
   {
    ++j;

    if(xctx->arc[c][i].fill)
      arc_bbox(xctx->arc[c][i].x, xctx->arc[c][i].y, xctx->arc[c][i].r, 0, 360,
               &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
    else
      arc_bbox(xctx->arc[c][i].x, xctx->arc[c][i].y, xctx->arc[c][i].r, xctx->arc[c][i].a, xctx->arc[c][i].b,
               &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
    my_free(_ALLOC_ID_, &xctx->arc[c][i].prop_ptr);
    deleted = 1;
    continue;
   }
   if(j)
   {
    xctx->arc[c][i-j] = xctx->arc[c][i];
   }
  }
  xctx->arcs[c] -= j;

  j = 0;
  for(i=0;i<xctx->polygons[c]; ++i)
  {
   if(xctx->poly[c][i].sel == SELECTED)
   {
    int k;
    double x1=0., y1=0., x2=0., y2=0.;
    for(k=0; k<xctx->poly[c][i].points; ++k) {
      if(k==0 || xctx->poly[c][i].x[k] < x1) x1 = xctx->poly[c][i].x[k];
      if(k==0 || xctx->poly[c][i].y[k] < y1) y1 = xctx->poly[c][i].y[k];
      if(k==0 || xctx->poly[c][i].x[k] > x2) x2 = xctx->poly[c][i].x[k];
      if(k==0 || xctx->poly[c][i].y[k] > y2) y2 = xctx->poly[c][i].y[k];
    }
    ++j;
    my_free(_ALLOC_ID_, &xctx->poly[c][i].prop_ptr);
    my_free(_ALLOC_ID_, &xctx->poly[c][i].x);
    my_free(_ALLOC_ID_, &xctx->poly[c][i].y);
    my_free(_ALLOC_ID_, &xctx->poly[c][i].selected_point);
    /*fprintf(errfp, "bbox: %.16g %.16g %.16g %.16g\n", x1, y1, x2, y2); */
    deleted = 1;
    continue;
   }
   if(j)
   {
    xctx->poly[c][i-j] = xctx->poly[c][i];
   }
  }
  xctx->polygons[c] -= j;
 }
 if(deleted) set_modify(1);
}

int delete_wires(int selected_flag)
{
  int i, j = 0, deleted = 0;
  for(i=0;i<xctx->wires; ++i)
  {
    if(xctx->wire[i].sel == selected_flag) {
      ++j;
      hash_wire(XDELETE, i, 0);
      my_free(_ALLOC_ID_, &xctx->wire[i].prop_ptr);
      my_free(_ALLOC_ID_, &xctx->wire[i].node);

      deleted = 1;
      continue;
    }
    if(j) {
      xctx->wire[i-j] = xctx->wire[i];
    }
  }
  xctx->wires -= j;
  if(j) {
    xctx->prep_hash_wires=0;
    xctx->prep_net_structs=0;
    xctx->prep_hi_structs=0;
  }
  if(xctx->hilight_nets) {
    propagate_hilights(1, 1, XINSERT_NOREPLACE);
  }
  return deleted;
}

void delete(int to_push_undo)
{
  int i, j, deleted = 0;
  #if HAS_CAIRO==1
  int customfont;
  #endif

  dbg(3, "delete(): start\n");
  j = 0;
  rebuild_selected_array();
  if(to_push_undo && xctx->lastsel) xctx->push_undo();
  del_rect_line_arc_poly();

  for(i=0;i<xctx->texts; ++i)
  {
    if(xctx->text[i].sel == SELECTED)
    {
      #if HAS_CAIRO==1
      customfont = set_text_custom_font(&xctx->text[i]);
      #endif
      #if HAS_CAIRO==1
      if(customfont) {
        cairo_restore(xctx->cairo_ctx);
      }
      #endif
      my_free(_ALLOC_ID_, &xctx->text[i].prop_ptr);
      my_free(_ALLOC_ID_, &xctx->text[i].font);
      my_free(_ALLOC_ID_, &xctx->text[i].floater_instname);
      my_free(_ALLOC_ID_, &xctx->text[i].floater_ptr);
      my_free(_ALLOC_ID_, &xctx->text[i].txt_ptr);
      deleted = 1;
      ++j;
      continue;
    }
    if(j)
    {
      dbg(1, "select(); deleting string %d\n",i-j);
      xctx->text[i-j] = xctx->text[i];
      dbg(1, "select(); new string %d = %s\n",i-j,xctx->text[i-j].txt_ptr);
    }
  }
  xctx->texts -= j;
  j = 0;

  for(i=0;i<xctx->instances; ++i)
  {
    if(xctx->inst[i].sel == SELECTED)
    {
      deleted = 1;
      if(xctx->inst[i].prop_ptr != NULL)
      {
        my_free(_ALLOC_ID_, &xctx->inst[i].prop_ptr);
      }
      delete_inst_node(i);
      my_free(_ALLOC_ID_, &xctx->inst[i].name);
      my_free(_ALLOC_ID_, &xctx->inst[i].instname);
      my_free(_ALLOC_ID_, &xctx->inst[i].lab);
      ++j;
      continue;
    }
    if(j)
    {
      xctx->inst[i-j] = xctx->inst[i];
    }
  }
  xctx->instances-=j;

  if(j) {
    xctx->prep_hash_inst=0;
    xctx->prep_net_structs=0;
    xctx->prep_hi_structs=0;
  }

  if(delete_wires(SELECTED)) {
    deleted = 1;
    if(tclgetboolvar("autotrim_wires")) trim_wires();
    update_conn_cues(WIRELAYER, 0, 0);
  }
  if(xctx->hilight_nets) {
    propagate_hilights(1, 1, XINSERT_NOREPLACE);
  }
  if(deleted) set_modify(1);
  xctx->lastsel = 0;
  draw();
  xctx->ui_state &= ~SELECTION;
  set_first_sel(0, -1, 0);
}

void delete_only_rect_line_arc_poly(void)
{
 del_rect_line_arc_poly();
 xctx->lastsel = 0;
 draw();
 xctx->ui_state &= ~SELECTION;
 set_first_sel(0, -1, 0);
}


void bbox(int what,double x1,double y1, double x2, double y2)
{
 dbg(1, "bbox: what=%d\n", what);
 switch(what)
 {
  case START:
   if(xctx->bbox_set==1) {
     fprintf(errfp, "ERROR: rentrant bbox() call\n");
     tcleval("alert_ {ERROR: reentrant bbox() call} {}");
   }
   xctx->bbx1 = 300000000; /* screen coordinates */
   xctx->bbx2 = 0;
   xctx->bby1 = 300000000;
   xctx->bby2 = 0;
   xctx->savex1 = xctx->areax1;
   xctx->savex2 = xctx->areax2;
   xctx->savey1 = xctx->areay1;
   xctx->savey2 = xctx->areay2;
   xctx->savew = xctx->areaw;
   xctx->saveh = xctx->areah;
   xctx->savexrect = xctx->xrect[0];
   xctx->bbox_set=1;
   break;
  case ADD:
   if(xctx->bbox_set==0) {
     fprintf(errfp, "ERROR: bbox(ADD) call before bbox(START)\n");
     tcleval("alert_ {ERROR: bbox(ADD) call before bbox(START)} {}");
   }
   dbg(2, "bbox(ADD): %.16g %.16g %.16g %.16g\n", x1, y1, x2, y2);
   x1=X_TO_SCREEN(x1);
   y1=Y_TO_SCREEN(y1);
   x2=X_TO_SCREEN(x2);
   y2=Y_TO_SCREEN(y2);
   x1=CLIP(x1,xctx->savex1,xctx->savex2);
   x2=CLIP(x2,xctx->savex1,xctx->savex2);
   y1=CLIP(y1,xctx->savey1,xctx->savey2);
   y2=CLIP(y2,xctx->savey1,xctx->savey2);
   if(x1 < xctx->bbx1) xctx->bbx1 = (int) x1;
   if(x2 > xctx->bbx2) xctx->bbx2 = (int) x2;
   if(y1 < xctx->bby1) xctx->bby1 = (int) y1;
   if(y2 > xctx->bby2) xctx->bby2 = (int) y2;
   if(y2 < xctx->bby1) xctx->bby1 = (int) y2;
   if(y1 > xctx->bby2) xctx->bby2 = (int) y1;
   break;
  case END:
   if(xctx->bbox_set) {
     xctx->areax1 = xctx->savex1;
     xctx->areax2 = xctx->savex2;
     xctx->areay1 = xctx->savey1;
     xctx->areay2 = xctx->savey2;
     xctx->areaw =  xctx->savew;
     xctx->areah =  xctx->saveh;
     xctx->xrect[0] = xctx->savexrect;
     if(has_x) {
       dbg(2, "bbox(END): resetting clip area: %d %d %d %d\n",
          xctx->xrect[0].x, xctx->xrect[0].y, xctx->xrect[0].width, xctx->xrect[0].height);
       set_clip_mask(END);
     }
     xctx->bbox_set=0;
   }
   break;
  case SET:
   if(xctx->bbox_set==0) {
     fprintf(errfp, "ERROR: bbox(SET) call before bbox(START)\n");
     tcleval("alert_ {ERROR: bbox(SET) call before bbox(START)} {}");
   }
   xctx->areax1 = xctx->bbx1-2*INT_WIDTH(xctx->lw);
   xctx->areax2 = xctx->bbx2+2*INT_WIDTH(xctx->lw);
   xctx->areay1 = xctx->bby1-2*INT_WIDTH(xctx->lw);
   xctx->areay2 = xctx->bby2+2*INT_WIDTH(xctx->lw);
   xctx->areaw = (xctx->areax2-xctx->areax1);
   xctx->areah = (xctx->areay2-xctx->areay1);

   xctx->xrect[0].x = (short)(xctx->bbx1-INT_WIDTH(xctx->lw));
   xctx->xrect[0].y = (short)(xctx->bby1-INT_WIDTH(xctx->lw));
   xctx->xrect[0].width = (unsigned short)(xctx->bbx2-xctx->bbx1+2*INT_WIDTH(xctx->lw));
   xctx->xrect[0].height = (unsigned short)(xctx->bby2-xctx->bby1+2*INT_WIDTH(xctx->lw));
   if(has_x) {
     set_clip_mask(SET);
       dbg(2, "bbox(SET): setting clip area: %d %d %d %d\n",
          xctx->xrect[0].x, xctx->xrect[0].y, xctx->xrect[0].width, xctx->xrect[0].height);
   }
   break;



  case SET_INSIDE: /* do not add line widths to clip rectangle so everything remains inside */
   if(xctx->bbox_set==0) {
     fprintf(errfp, "ERROR: bbox(SET_INSIDE) call before bbox(START)\n");
     tcleval("alert_ {ERROR: bbox(SET_INSIDE) call before bbox(START)} {}");
   }
   xctx->areax1 = xctx->bbx1-2*INT_WIDTH(xctx->lw);
   xctx->areax2 = xctx->bbx2+2*INT_WIDTH(xctx->lw);
   xctx->areay1 = xctx->bby1-2*INT_WIDTH(xctx->lw);
   xctx->areay2 = xctx->bby2+2*INT_WIDTH(xctx->lw);
   xctx->areaw = (xctx->areax2-xctx->areax1);
   xctx->areah = (xctx->areay2-xctx->areay1);

   xctx->xrect[0].x = (short)(xctx->bbx1+INT_WIDTH(xctx->lw));
   xctx->xrect[0].y = (short)(xctx->bby1+INT_WIDTH(xctx->lw));
   xctx->xrect[0].width = (unsigned short)(xctx->bbx2-xctx->bbx1-2*INT_WIDTH(xctx->lw));
   xctx->xrect[0].height = (unsigned short)(xctx->bby2-xctx->bby1-2*INT_WIDTH(xctx->lw));
   if(has_x) {
     set_clip_mask(SET);
       dbg(2, "bbox(SET): setting clip area: %d %d %d %d\n",
          xctx->xrect[0].x, xctx->xrect[0].y, xctx->xrect[0].width, xctx->xrect[0].height);
   }
   break;


  default:
   break;
 }
}

void set_first_sel(unsigned short type, int n, unsigned int col)
{
  if(n == -1) { /* reset first_sel */
    xctx->first_sel.type = 0;
    xctx->first_sel.n = -1;
    xctx->first_sel.col = 0;
  }else if(xctx->first_sel.n == -1) {
    xctx->first_sel.type = type;
    xctx->first_sel.n = n;
    xctx->first_sel.col = col;
  }
}

void unselect_all(int dr)
{
 int i,c;
 char str[PATH_MAX];
 #if HAS_CAIRO==1
 int customfont;
 #endif
  set_first_sel(0, -1, 0);
  if((xctx->ui_state & SELECTION) || xctx->lastsel) {
    dbg(1, "unselect_all(1): start\n");
    xctx->ui_state = 0;
    xctx->lastsel = 0;
    for(i=0;i<xctx->wires; ++i)
    {
     if(xctx->wire[i].sel)
     {
      xctx->wire[i].sel = 0;
      {
        if(dr) {
          if(xctx->wire[i].bus)
            drawtempline(xctx->gctiled, THICK, xctx->wire[i].x1, xctx->wire[i].y1,
                                               xctx->wire[i].x2, xctx->wire[i].y2);
          else
            drawtempline(xctx->gctiled, ADD, xctx->wire[i].x1, xctx->wire[i].y1,
                                             xctx->wire[i].x2, xctx->wire[i].y2);
        }
      }
     }
    }
    for(i=0;i<xctx->instances; ++i)
    {
     if(xctx->inst[i].sel == SELECTED)
     {
      xctx->inst[i].sel = 0;
      if(dr) for(c=0;c<cadlayers; ++c)
        draw_temp_symbol(ADD, xctx->gctiled, i, c,0,0,0.0,0.0);
     }
    }
    for(i=0;i<xctx->texts; ++i)
    {
     if(xctx->text[i].sel == SELECTED)
     {
      xctx->text[i].sel = 0;
      if(dr) {
        #if HAS_CAIRO==1
        customfont = set_text_custom_font(& xctx->text[i]); /* needed for bbox calculation */
        #endif
        draw_temp_string(xctx->gctiled,ADD, get_text_floater(i),
         xctx->text[i].rot, xctx->text[i].flip, xctx->text[i].hcenter, xctx->text[i].vcenter,
         xctx->text[i].x0, xctx->text[i].y0,
         xctx->text[i].xscale, xctx->text[i].yscale);
        #if HAS_CAIRO==1
        if(customfont) {
          cairo_restore(xctx->cairo_ctx);
        }
        #endif
      }
     }
    }
    for(c=0;c<cadlayers; ++c)
    {
     for(i=0;i<xctx->arcs[c]; ++i)
     {
      if(xctx->arc[c][i].sel)
      {
       xctx->arc[c][i].sel = 0;
       if(dr) drawtemparc(xctx->gctiled, ADD, xctx->arc[c][i].x, xctx->arc[c][i].y,
                                 xctx->arc[c][i].r, xctx->arc[c][i].a, xctx->arc[c][i].b);
      }
     }
     for(i=0;i<xctx->rects[c]; ++i)
     {
      if(xctx->rect[c][i].sel)
      {
       xctx->rect[c][i].sel = 0;
       if(dr) drawtemprect(xctx->gctiled, ADD, xctx->rect[c][i].x1, xctx->rect[c][i].y1,
                                  xctx->rect[c][i].x2, xctx->rect[c][i].y2);
      }
     }
     for(i=0;i<xctx->lines[c]; ++i)
     {
      if(xctx->line[c][i].sel)
      {
       xctx->line[c][i].sel = 0;
       if(dr) {
         if(xctx->line[c][i].bus)
           drawtempline(xctx->gctiled, THICK, xctx->line[c][i].x1, xctx->line[c][i].y1,
                                        xctx->line[c][i].x2, xctx->line[c][i].y2);
         else
           drawtempline(xctx->gctiled, ADD, xctx->line[c][i].x1, xctx->line[c][i].y1,
                                      xctx->line[c][i].x2, xctx->line[c][i].y2);
       }
      }
     }
     for(i=0;i<xctx->polygons[c]; ++i)
     {
      if(xctx->poly[c][i].sel)
      {
       int k;
       for(k=0;k<xctx->poly[c][i].points; ++k) xctx->poly[c][i].selected_point[k] = 0;
       xctx->poly[c][i].sel = 0;
       if(dr) drawtemppolygon(xctx->gctiled, NOW, xctx->poly[c][i].x, xctx->poly[c][i].y, xctx->poly[c][i].points);
      }
     }
    }
    if(dr) {
      drawtemparc(xctx->gctiled, END, 0.0, 0.0, 0.0, 0.0, 0.0);
      drawtemprect(xctx->gctiled, END, 0.0, 0.0, 0.0, 0.0);
      drawtempline(xctx->gctiled,END, 0.0, 0.0, 0.0, 0.0);
    }
    xctx->ui_state &= ~SELECTION;
    my_snprintf(str, S(str), "%s/%s", user_conf_dir, ".selection.sch"); /* 20161115  PWD->HOME */
    xunlink(str);
    dbg(2, "unselect_all(1): done\n");
  }
}

void select_wire(int i,unsigned short select_mode, int fast)
{
  char str[1024];       /* overflow safe */
  /*my_strncpy(s,xctx->wire[i].prop_ptr!=NULL?xctx->wire[i].prop_ptr:"<NULL>",256); */
  if( !fast )
  {
    my_snprintf(str, S(str), "Info: selected wire: n=%d end1=%d end2=%d\nnode=%s",i,
           xctx->wire[i].end1, xctx->wire[i].end2,
           xctx->wire[i].prop_ptr? xctx->wire[i].prop_ptr: "(null)");
    statusmsg(str,3);
    tcleval("infowindow");

    my_snprintf(str, S(str), "n=%4d x = %.16g  y = %.16g  w = %.16g h = %.16g",
      i, xctx->wire[i].x1, xctx->wire[i].y1,
      xctx->wire[i].x2-xctx->wire[i].x1, xctx->wire[i].y2-xctx->wire[i].y1
    );
    statusmsg(str,1);
  }
  if( ((xctx->wire[i].sel|select_mode) == (SELECTED1|SELECTED2)) ||
      ((xctx->wire[i].sel == SELECTED) && select_mode) ) {
    xctx->wire[i].sel = SELECTED;
  } else {
    xctx->wire[i].sel = select_mode;
  }
  if( xctx->wire[i].sel == SELECTED) set_first_sel(WIRE, i, 0);
  if(select_mode) {
   dbg(1, "select(): wire[%d].end1=%d, ,end2=%d\n", i, xctx->wire[i].end1, xctx->wire[i].end2);
   if(xctx->wire[i].bus)
     drawtempline(xctx->gc[SELLAYER], THICK, xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2);
   else
     drawtempline(xctx->gc[SELLAYER], ADD, xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2);
  }
  else {
   if(xctx->wire[i].bus)
     drawtempline(xctx->gctiled, THICK, xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2);
   else
     drawtempline(xctx->gctiled, NOW, xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2);
  }
  xctx->need_reb_sel_arr=1;
}

void select_element(int i,unsigned short select_mode, int fast, int override_lock)
{
  int c, j;
  char str[1024];       /* overflow safe */
  char s[256];          /* overflow safe */

  if(!strboolcmp(get_tok_value(xctx->inst[i].prop_ptr, "lock", 0), "true") &&
      select_mode == SELECTED && !override_lock) return;
  my_strncpy(s,xctx->inst[i].prop_ptr!=NULL?xctx->inst[i].prop_ptr:"<NULL>",S(s));
  if( !fast )
  {
    my_snprintf(str, S(str), "Info: selected element %d: %s\nproperties:\n%s", i,
      xctx->inst[i].name ? xctx->inst[i].name : "<NULL>" , s);
    statusmsg(str,3);
    /* 20190526 */ /*Why this? 20191125 only on small schematics. slow down on big schematics */
    if(xctx->instances < 150) {
      prepare_netlist_structs(0);
      if(xctx->inst[i].ptr != -1) for(j=0;j< (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER] ; ++j)
      {
        if(j == 0) statusmsg("pinlist:", 2);
        if(xctx->inst[i].node && (xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][j].prop_ptr)
        {
          my_snprintf(str, S(str), "pin:%s -> %s",
            get_tok_value(
               (xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][j].prop_ptr,"name",0) ,
            xctx->inst[i].node[j] ? xctx->inst[i].node[j] : "__UNCONNECTED_PIN__");
          statusmsg(str,2);
        }
      }
    }
    tcleval("infowindow");
    my_snprintf(str, S(str), "n=%4d x = %.16g  y = %.16g  w = %.16g h = %.16g",i, 
       xctx->inst[i].xx1, xctx->inst[i].yy1,
       xctx->inst[i].xx2-xctx->inst[i].xx1, xctx->inst[i].yy2-xctx->inst[i].yy1
    );
    statusmsg(str,1);
  }
  xctx->inst[i].sel = select_mode;
  if(select_mode == SELECTED) set_first_sel(ELEMENT, i, 0);
  if(select_mode) {
    for(c=0;c<cadlayers; ++c) {
      draw_temp_symbol(ADD, xctx->gc[SELLAYER], i,c,0,0,0.0,0.0);
    }
  } else {
    symbol_bbox(i, &xctx->inst[i].x1, &xctx->inst[i].y1, &xctx->inst[i].x2, &xctx->inst[i].y2 );
    for(c=0;c<cadlayers; ++c) {
      draw_temp_symbol(NOW, xctx->gctiled, i,c,0,0,0.0,0.0);
    }
  }
  xctx->need_reb_sel_arr=1;
}

void select_text(int i,unsigned short select_mode, int fast)
{
  char str[1024];       /* overflow safe */
  char s[256];          /* overflow safe */
  #if HAS_CAIRO==1
  int customfont;
  #endif
  if(!fast) {
    my_strncpy(s,xctx->text[i].prop_ptr!=NULL?xctx->text[i].prop_ptr:"<NULL>",S(s));
    my_snprintf(str, S(str), "Info: selected text %d: properties: %s", i,s);
    statusmsg(str,3);
    tcleval("infowindow");
    my_snprintf(str, S(str), "n=%4d x = %.16g  y = %.16g", i, xctx->text[i].x0, xctx->text[i].y0);
    statusmsg(str,1);
  }
  xctx->text[i].sel = select_mode;
  #if HAS_CAIRO==1
  customfont = set_text_custom_font(&xctx->text[i]);
  #endif
  if(select_mode) {
    set_first_sel(xTEXT, i, 0);
    draw_temp_string(xctx->gc[SELLAYER],ADD, get_text_floater(i),
      xctx->text[i].rot, xctx->text[i].flip, xctx->text[i].hcenter, xctx->text[i].vcenter,
      xctx->text[i].x0, xctx->text[i].y0,
      xctx->text[i].xscale, xctx->text[i].yscale);
  } else {
    draw_temp_string(xctx->gctiled,NOW, get_text_floater(i),
      xctx->text[i].rot, xctx->text[i].flip, xctx->text[i].hcenter, xctx->text[i].vcenter,
      xctx->text[i].x0, xctx->text[i].y0,
      xctx->text[i].xscale, xctx->text[i].yscale);
  }
  #if HAS_CAIRO==1
  if(customfont) {
    cairo_restore(xctx->cairo_ctx);
  }
  #endif
  xctx->need_reb_sel_arr=1;
}

void select_box(int c, int i, unsigned short select_mode, int fast, int override_lock)
{
  char str[1024];       /* overflow safe */
  char s[256];          /* overflow safe */

  if(!strboolcmp(get_tok_value(xctx->rect[c][i].prop_ptr, "lock", 0), "true") &&
      select_mode == SELECTED && !override_lock) return;
  if(!fast)
  {
   my_strncpy(s,xctx->rect[c][i].prop_ptr!=NULL?xctx->rect[c][i].prop_ptr:"<NULL>",S(s));
   my_snprintf(str, S(str), "Info: selected box : layer=%d, n=%d properties: %s",c-4,i,s);
   statusmsg(str,3);
   tcleval("infowindow");

   my_snprintf(str, S(str), "n=%4d x = %.16g  y = %.16g  w = %.16g h = %.16g",
      i, xctx->rect[c][i].x1, xctx->rect[c][i].y1,
      xctx->rect[c][i].x2-xctx->rect[c][i].x1, xctx->rect[c][i].y2-xctx->rect[c][i].y1
   );
   statusmsg(str,1);
  }
  if(select_mode) {
    if(select_mode==SELECTED) {
      xctx->rect[c][i].sel = select_mode;
      /* remember last selected graph */
      if(c == GRIDLAYER && (xctx->rect[c][i].flags & 1)) xctx->graph_lastsel = i;
    } else {
      xctx->rect[c][i].sel |= select_mode;
    }
    drawtemprect(xctx->gc[SELLAYER], ADD, xctx->rect[c][i].x1, xctx->rect[c][i].y1,
                                    xctx->rect[c][i].x2, xctx->rect[c][i].y2);
  } else {
    xctx->rect[c][i].sel = 0;
    drawtemprect(xctx->gctiled, NOW, xctx->rect[c][i].x1, xctx->rect[c][i].y1,   
                               xctx->rect[c][i].x2, xctx->rect[c][i].y2);
  }
  if( xctx->rect[c][i].sel == (SELECTED1|SELECTED2|SELECTED3|SELECTED4)) xctx->rect[c][i].sel = SELECTED;
  if(xctx->rect[c][i].sel == SELECTED) set_first_sel(xRECT, i, c);
  xctx->need_reb_sel_arr=1;
}



void select_arc(int c, int i, unsigned short select_mode, int fast)
{
  char str[1024];   /* overflow safe */
  char s[256];    /* overflow safe */
  if(!fast)
  {
   my_strncpy(s,xctx->rect[c][i].prop_ptr!=NULL?xctx->rect[c][i].prop_ptr:"<NULL>",S(s));
   my_snprintf(str, S(str), "Info: selected arc : layer=%d, n=%d properties: %s",c-4,i,s);
   statusmsg(str,3);
   tcleval("infowindow");
 
   my_snprintf(str, S(str), "n=%4d x = %.16g  y = %.16g  r = %.16g a = %.16g b = %.16g",
      i, xctx->arc[c][i].x, xctx->arc[c][i].y, xctx->arc[c][i].r, xctx->arc[c][i].a, xctx->arc[c][i].b);
   statusmsg(str,1);
  }
  if(select_mode) {
    xctx->arc[c][i].sel = select_mode;
    drawtemparc(xctx->gc[SELLAYER], ADD, xctx->arc[c][i].x, xctx->arc[c][i].y,
                              xctx->arc[c][i].r, xctx->arc[c][i].a, xctx->arc[c][i].b);
  } else {
    xctx->arc[c][i].sel = 0;
    drawtemparc(xctx->gctiled, NOW, xctx->arc[c][i].x, xctx->arc[c][i].y,
                              xctx->arc[c][i].r, xctx->arc[c][i].a, xctx->arc[c][i].b);
  }
  if(xctx->arc[c][i].sel == SELECTED) set_first_sel(ARC, i, c);

  /*if( xctx->arc[c][i].sel == (SELECTED1|SELECTED2|SELECTED3|SELECTED4)) xctx->arc[c][i].sel = SELECTED; */

  xctx->need_reb_sel_arr=1;
}

void select_polygon(int c, int i, unsigned short select_mode, int fast )
{
  char str[1024];       /* overflow safe */
  char s[256];          /* overflow safe */
  if(!fast)
  {
   my_strncpy(s,xctx->poly[c][i].prop_ptr!=NULL?xctx->poly[c][i].prop_ptr:"<NULL>",S(s));
   my_snprintf(str, S(str), "Info: selected polygon: layer=%d, n=%d properties: %s",c-4,i,s);
   statusmsg(str,3);
   tcleval("infowindow");

   my_snprintf(str, S(str), "n=%4d x0 = %.16g  y0 = %.16g ...", i, xctx->poly[c][i].x[0], xctx->poly[c][i].y[0]);
   statusmsg(str,1);
  }
  xctx->poly[c][i].sel = select_mode;
  if(select_mode) {
    drawtemppolygon(xctx->gc[SELLAYER], NOW, xctx->poly[c][i].x, xctx->poly[c][i].y, xctx->poly[c][i].points);
  }
  else {
    drawtemppolygon(xctx->gctiled, NOW, xctx->poly[c][i].x, xctx->poly[c][i].y, xctx->poly[c][i].points);
  }
  if(xctx->poly[c][i].sel == SELECTED) set_first_sel(POLYGON, i, c);
  xctx->need_reb_sel_arr=1;
}

void select_line(int c, int i, unsigned short select_mode, int fast )
{
  char str[1024];       /* overflow safe */
  char s[256];          /* overflow safe */
  if(!fast)
  {
   my_strncpy(s,xctx->line[c][i].prop_ptr!=NULL?xctx->line[c][i].prop_ptr:"<NULL>",S(s));
   my_snprintf(str, S(str), "Info: selected line: layer=%d, n=%d properties: %s",c-4,i,s);
   statusmsg(str,3);
   tcleval("infowindow");

   my_snprintf(str, S(str), "n=%4d x = %.16g  y = %.16g  w = %.16g h = %.16g",
      i, xctx->line[c][i].x1, xctx->line[c][i].y1,
      xctx->line[c][i].x2-xctx->line[c][i].x1, xctx->line[c][i].y2-xctx->line[c][i].y1
   );
   statusmsg(str,1);
  }
  if( ((xctx->line[c][i].sel|select_mode) == (SELECTED1|SELECTED2)) ||
      ((xctx->line[c][i].sel == SELECTED) && select_mode) ) {
    xctx->line[c][i].sel = SELECTED;
  } else {
    xctx->line[c][i].sel = select_mode;
  }
  if(xctx->line[c][i].sel == SELECTED) set_first_sel(LINE, i, c);
  if(select_mode) {
   if(xctx->line[c][i].bus)
     drawtempline(xctx->gc[SELLAYER], THICK, xctx->line[c][i].x1, xctx->line[c][i].y1,
                                       xctx->line[c][i].x2, xctx->line[c][i].y2);
   else
     drawtempline(xctx->gc[SELLAYER], ADD, xctx->line[c][i].x1, xctx->line[c][i].y1,
                                     xctx->line[c][i].x2, xctx->line[c][i].y2);
  }
  else {
    if(xctx->line[c][i].bus)
      drawtempline(xctx->gctiled, THICK, xctx->line[c][i].x1, xctx->line[c][i].y1,
                                 xctx->line[c][i].x2, xctx->line[c][i].y2);
    else
      drawtempline(xctx->gctiled, NOW, xctx->line[c][i].x1, xctx->line[c][i].y1,
                                 xctx->line[c][i].x2, xctx->line[c][i].y2);
  }
  xctx->need_reb_sel_arr=1;
}

/* 20160503 return type field */
Selected select_object(double mx,double my, unsigned short select_mode, int override_lock)
{
   Selected sel;
   xctx->already_selected = 0;
   sel = find_closest_obj(mx, my, override_lock);
   dbg(1, "select_object(): sel.n=%d, sel.col=%d, sel.type=%d\n", sel.n, sel.col, sel.type);

   switch(sel.type)
   {
    case WIRE:
     if(xctx->wire[sel.n].sel) xctx->already_selected = 1;
     select_wire(sel.n, select_mode, 0);
     break;
    case xTEXT:
     if(xctx->text[sel.n].sel) xctx->already_selected = 1;
     select_text(sel.n, select_mode, 0);
     break;
    case LINE:
     if(xctx->line[sel.col][sel.n].sel) xctx->already_selected = 1;
     select_line(sel.col, sel.n, select_mode,0);
     break;
    case POLYGON:
     if(xctx->poly[sel.col][sel.n].sel) xctx->already_selected = 1;
     select_polygon(sel.col, sel.n, select_mode,0);
     break;
    case xRECT:
     if(xctx->rect[sel.col][sel.n].sel) xctx->already_selected = 1;
     select_box(sel.col,sel.n, select_mode,0, override_lock);
     break;
    case ARC:
     if(xctx->arc[sel.col][sel.n].sel) xctx->already_selected = 1;
     select_arc(sel.col,sel.n, select_mode,0);
     break;
    case ELEMENT:
     if(xctx->inst[sel.n].sel) xctx->already_selected = 1;
     select_element(sel.n,select_mode,0, override_lock);
     break;
    default:
     break;
   } /*end switch */

   drawtemparc(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
   drawtemprect(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
   drawtempline(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);

   if(sel.type)  xctx->ui_state |= SELECTION;


   if(!select_mode) {
     rebuild_selected_array();
     draw_selection(xctx->gc[SELLAYER], 0);
   }

   return sel;
}

/* Partial-select wire ends that land on instance pins  and selected nets */
void select_attached_nets(void)
{
  int wire, inst, j, i, rects, r, sqx, sqy;
  double x0, y0;
  Wireentry *wptr;

  hash_wires();
  rebuild_selected_array();

  for(j=0;j<xctx->lastsel; ++j) {
    if(xctx->sel_array[j].type==ELEMENT) {
      inst = xctx->sel_array[j].n;
      if((xctx->inst[inst].ptr >= 0)) {
        rects = (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER];
        for(r = 0; r < rects; r++)
        {
          get_inst_pin_coord(inst, r, &x0, &y0);
          get_square(x0, y0, &sqx, &sqy);
          for(wptr=xctx->wire_spatial_table[sqx][sqy]; wptr; wptr=wptr->next) {
            i = wptr->n;
            if(xctx->wire[i].x1 == x0 &&  xctx->wire[i].y1 == y0) {
               select_wire(i,SELECTED1, 1);
            }
            if(xctx->wire[i].x2 == x0 &&  xctx->wire[i].y2 == y0) {
               select_wire(i,SELECTED2, 1);
            }
          }
        }
      }
    }
    if(xctx->sel_array[j].type==WIRE) {
      wire = xctx->sel_array[j].n;
      if(xctx->wire[wire].sel != SELECTED) continue;
      for(r = 0; r < 2; r++)
      {
        if(r == 0) {
          x0 = xctx->wire[wire].x1;
          y0 = xctx->wire[wire].y1;
        } else {
          x0 = xctx->wire[wire].x2;
          y0 = xctx->wire[wire].y2;
        }
        get_square(x0, y0, &sqx, &sqy);
        for(wptr=xctx->wire_spatial_table[sqx][sqy]; wptr; wptr=wptr->next) {
          i = wptr->n;
          if(i == wire) continue;
          if(xctx->wire[i].x1 == x0 &&  xctx->wire[i].y1 == y0) {
             select_wire(i,SELECTED1, 1);
          }
          if(xctx->wire[i].x2 == x0 &&  xctx->wire[i].y2 == y0) {
             select_wire(i,SELECTED2, 1);
          }
        }
      }
    }
  } /*  for(j=0;j<xctx->lastsel; ++j) */
  rebuild_selected_array();
}

void select_inside(double x1,double y1, double x2, double y2, int sel) /*added unselect (sel param) */
{
 int c,i, tmpint;
 double x, y, r, a, b, xa, ya, xb, yb; /* arc */
 double xx1,yy1,xx2,yy2, dtmp;
 xRect tmp;
 int en_s;
 int select_rot = 0, select_flip = 0;
 #if HAS_CAIRO==1
 int customfont;
 #endif

 en_s = tclgetboolvar("enable_stretch");
 for(i=0;i<xctx->wires; ++i)
 {
  if(RECT_INSIDE(xctx->wire[i].x1,xctx->wire[i].y1,xctx->wire[i].x2,xctx->wire[i].y2, x1,y1,x2,y2))
  {
   xctx->ui_state |= SELECTION; /* set xctx->ui_state to SELECTION also if unselecting by area ???? */
   sel ? select_wire(i,SELECTED, 1): select_wire(i,0, 1);
  }
  else if( sel && en_s && POINTINSIDE(xctx->wire[i].x1,xctx->wire[i].y1, x1,y1,x2,y2) )
  {
   xctx->ui_state |= SELECTION;
   select_wire(i,SELECTED1, 1);
  }
  else if( sel && en_s && POINTINSIDE(xctx->wire[i].x2,xctx->wire[i].y2, x1,y1,x2,y2) )
  {
   xctx->ui_state |= SELECTION;
   select_wire(i,SELECTED2, 1);
  }
 }
 for(i=0;i<xctx->texts; ++i)
 {
  select_rot = xctx->text[i].rot;
  select_flip = xctx->text[i].flip;
  #if HAS_CAIRO==1
  customfont = set_text_custom_font(&xctx->text[i]);
  #endif

  text_bbox(get_text_floater(i),
             xctx->text[i].xscale, xctx->text[i].yscale, (short)select_rot, (short)select_flip, 
             xctx->text[i].hcenter, xctx->text[i].vcenter,
             xctx->text[i].x0, xctx->text[i].y0,
             &xx1,&yy1, &xx2,&yy2, &tmpint, &dtmp);
  #if HAS_CAIRO==1
  if(customfont) {
    cairo_restore(xctx->cairo_ctx);
  }
  #endif
  if(RECT_INSIDE(xx1,yy1, xx2, yy2,x1,y1,x2,y2))
  {
   xctx->ui_state |= SELECTION; /* set xctx->ui_state to SELECTION also if unselecting by area ???? */
   sel ? select_text(i, SELECTED, 1): select_text(i, 0, 1);
  }
 }
 for(i=0;i<xctx->instances; ++i)
 {
  if(RECT_INSIDE(xctx->inst[i].xx1, xctx->inst[i].yy1, xctx->inst[i].xx2, xctx->inst[i].yy2, x1,y1,x2,y2))
  {
   xctx->ui_state |= SELECTION; /* set xctx->ui_state to SELECTION also if unselecting by area ???? */
   if(sel) {
     if(strboolcmp(get_tok_value(xctx->inst[i].prop_ptr, "lock", 0), "true")) {
       select_element(i, SELECTED, 1, 1);
     }
   } else {
     select_element(i, 0, 1, 0);
   }
  }
 }
 for(c=0;c<cadlayers; ++c)
 {
   if(!xctx->enable_layer[c]) continue;
   for(i=0;i<xctx->polygons[c]; ++i) {
     int k, selected_points, flag;
 
     polygon_bbox(xctx->poly[c][i].x, xctx->poly[c][i].y, xctx->poly[c][i].points, &xa, &ya, &xb, &yb);
     if(RECT_OUTSIDE(xa, ya, xb, yb, x1, y1, x2, y2)) continue;
     selected_points = 0;
     flag=0;
     for(k=0; k<xctx->poly[c][i].points; ++k) {
       if(xctx->poly[c][i].sel==SELECTED) xctx->poly[c][i].selected_point[k] = 1;
       if( POINTINSIDE(xctx->poly[c][i].x[k],xctx->poly[c][i].y[k], x1,y1,x2,y2)) {
         flag=1;
         xctx->poly[c][i].selected_point[k] = (short)sel;
       }
       if(xctx->poly[c][i].selected_point[k]) selected_points++;
     }
     if(flag) {
       if(selected_points==0) {
         select_polygon(c, i, 0, 1);
       }
       if(selected_points==xctx->poly[c][i].points) {
         xctx->ui_state |= SELECTION;
         select_polygon(c, i, SELECTED, 1);
       } else if(selected_points) {
         /* for polygon, SELECTED1 means partial sel */
         if(sel && en_s) select_polygon(c, i, SELECTED1,1);
       }
     }
 
   }
   for(i=0;i<xctx->lines[c]; ++i)
   {
    if(RECT_INSIDE(xctx->line[c][i].x1,xctx->line[c][i].y1,xctx->line[c][i].x2,xctx->line[c][i].y2, x1,y1,x2,y2))
    {
     xctx->ui_state |= SELECTION;
     sel? select_line(c,i,SELECTED,1): select_line(c,i,0,1);
    }
    else if( sel && en_s && POINTINSIDE(xctx->line[c][i].x1,xctx->line[c][i].y1, x1,y1,x2,y2) )
    {
     xctx->ui_state |= SELECTION; /* set xctx->ui_state to SELECTION also if unselecting by area ???? */
     select_line(c, i,SELECTED1,1);
    }
    else if( sel && en_s && POINTINSIDE(xctx->line[c][i].x2,xctx->line[c][i].y2, x1,y1,x2,y2) )
    {
     xctx->ui_state |= SELECTION;
     select_line(c, i,SELECTED2,1);
    }
   }
   for(i=0;i<xctx->arcs[c]; ++i) {
     x = xctx->arc[c][i].x;
     y = xctx->arc[c][i].y;
     a = xctx->arc[c][i].a;
     b = xctx->arc[c][i].b;
     r = xctx->arc[c][i].r;
     xa = x + r * cos(a * XSCH_PI/180.);
     ya = y - r * sin(a * XSCH_PI/180.);
     xb = x + r * cos((a+b) * XSCH_PI/180.);
     yb = y - r * sin((a+b) * XSCH_PI/180.);
     arc_bbox(x, y, r, a, b, &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
     if(RECT_INSIDE(tmp.x1, tmp.y1, tmp.x2, tmp.y2, x1,y1,x2,y2)) {
       xctx->ui_state |= SELECTION; /* set xctx->ui_state to SELECTION also if unselecting by area ???? */
       sel? select_arc(c, i, SELECTED,1): select_arc(c, i, 0,1);
     }
     else if( sel && en_s && POINTINSIDE(x, y, x1, y1, x2, y2) )
     {
       xctx->ui_state |= SELECTION; /* set xctx->ui_state to SELECTION also if unselecting by area ???? */
       select_arc(c, i,SELECTED1,1);
     }
     else if( sel && en_s && POINTINSIDE(xb, yb, x1, y1, x2, y2) )
     {
       xctx->ui_state |= SELECTION; /* set xctx->ui_state to SELECTION also if unselecting by area ???? */
       select_arc(c, i,SELECTED3,1);
     }
     else if( sel && en_s && POINTINSIDE(xa, ya, x1, y1, x2, y2) )
     {
       xctx->ui_state |= SELECTION; /* set xctx->ui_state to SELECTION also if unselecting by area ???? */
       select_arc(c, i,SELECTED2,1);
     }
   }
   for(i=0;i<xctx->rects[c]; ++i)
   {
    if(RECT_INSIDE(xctx->rect[c][i].x1,xctx->rect[c][i].y1,xctx->rect[c][i].x2,xctx->rect[c][i].y2, x1,y1,x2,y2))
    {
      xctx->ui_state |= SELECTION; /* set xctx->ui_state to SELECTION also if unselecting by area ???? */


      if(sel) {
        if(strboolcmp(get_tok_value(xctx->rect[c][i].prop_ptr, "lock", 0), "true")) {
          select_box(c,i, SELECTED, 1, 1);
        }
      } else {
        select_box(c,i, 0, 1, 0);
      }
    }
    else if(c != GRIDLAYER || !(xctx->rect[c][i].flags & 2048)){ /* no stretch on unscaled images */
      if( sel && en_s && POINTINSIDE(xctx->rect[c][i].x1,xctx->rect[c][i].y1, x1,y1,x2,y2) )
      {                                  /*20070302 added stretch select */
        xctx->ui_state |= SELECTION;
        select_box(c, i,SELECTED1,1, 0);
      }
      if( sel && en_s && POINTINSIDE(xctx->rect[c][i].x2,xctx->rect[c][i].y1, x1,y1,x2,y2) )
      {
        xctx->ui_state |= SELECTION;
        select_box(c, i,SELECTED2,1, 0);
      }
      if( sel && en_s && POINTINSIDE(xctx->rect[c][i].x1,xctx->rect[c][i].y2, x1,y1,x2,y2) )
      {
        xctx->ui_state |= SELECTION;
        select_box(c, i,SELECTED3,1, 0);
      }
      if( sel && en_s && POINTINSIDE(xctx->rect[c][i].x2,xctx->rect[c][i].y2, x1,y1,x2,y2) )
      {
        xctx->ui_state |= SELECTION;
        select_box(c, i,SELECTED4,1, 0);
      }
    }
 
   } /* end for i */
 } /* end for c */
 drawtemparc(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
 drawtemprect(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
 drawtempline(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);

 if(!sel) {
   rebuild_selected_array();
   draw_selection(xctx->gc[SELLAYER], 0);
 }

}

void select_touch(double x1,double y1, double x2, double y2, int sel) /*added unselect (sel param) */
{
 int c,i, tmpint;
 double x, y, r, a, b, xa, ya, xb, yb; /* arc */
 double xx1,yy1,xx2,yy2, dtmp;
 xRect tmp;
 int select_rot = 0, select_flip = 0;
 #if HAS_CAIRO==1
 int customfont;
 #endif

 for(i=0;i<xctx->wires; ++i)
 {
  double lx1, ly1, lx2, ly2;
  lx1 = xctx->wire[i].x1;
  ly1 = xctx->wire[i].y1;
  lx2 = xctx->wire[i].x2;
  ly2 = xctx->wire[i].y2;
  if(lineclip(&lx1, &ly1, &lx2, &ly2, x1,y1,x2,y2)) {
   xctx->ui_state |= SELECTION; /* set xctx->ui_state to SELECTION also if unselecting by area ???? */
   sel ? select_wire(i,SELECTED, 1): select_wire(i,0, 1);
  }
 }
 for(i=0;i<xctx->texts; ++i)
 {
  select_rot = xctx->text[i].rot;
  select_flip = xctx->text[i].flip;
  #if HAS_CAIRO==1
  customfont = set_text_custom_font(&xctx->text[i]);
  #endif

  text_bbox(get_text_floater(i),
             xctx->text[i].xscale, xctx->text[i].yscale, (short)select_rot, (short)select_flip, 
             xctx->text[i].hcenter, xctx->text[i].vcenter,
             xctx->text[i].x0, xctx->text[i].y0,
             &xx1,&yy1, &xx2,&yy2, &tmpint, &dtmp);
  #if HAS_CAIRO==1
  if(customfont) {
    cairo_restore(xctx->cairo_ctx);
  }
  #endif
  if(RECT_TOUCH(xx1, yy1, xx2, yy2,x1,y1,x2,y2))
  {
   xctx->ui_state |= SELECTION; /* set xctx->ui_state to SELECTION also if unselecting by area ???? */
   sel ? select_text(i, SELECTED, 1): select_text(i, 0, 1);
  }
 }
 for(i=0;i<xctx->instances; ++i)
 {
  if(RECT_TOUCH(xctx->inst[i].xx1, xctx->inst[i].yy1, xctx->inst[i].xx2, xctx->inst[i].yy2, x1,y1,x2,y2))
  {
   xctx->ui_state |= SELECTION; /* set xctx->ui_state to SELECTION also if unselecting by area ???? */
   if(sel) {
     if(strboolcmp(get_tok_value(xctx->inst[i].prop_ptr, "lock", 0), "true")) {
       select_element(i, SELECTED, 1, 1);
     }
   } else {
     select_element(i, 0, 1, 0);
   }
  }
 }
 for(c=0;c<cadlayers; ++c)
 {
   if(!xctx->enable_layer[c]) continue;
   for(i=0;i<xctx->polygons[c]; ++i) {
     int k, flag;
 
     polygon_bbox(xctx->poly[c][i].x, xctx->poly[c][i].y, xctx->poly[c][i].points, &xa, &ya, &xb, &yb);
     if(RECT_OUTSIDE(xa, ya, xb, yb, x1, y1, x2, y2)) continue;
     flag=0;
     for(k=0; k<xctx->poly[c][i].points; ++k) {
       if(xctx->poly[c][i].sel==SELECTED) xctx->poly[c][i].selected_point[k] = 1;
       if( POINTINSIDE(xctx->poly[c][i].x[k],xctx->poly[c][i].y[k], x1,y1,x2,y2)) {
         xctx->ui_state |= SELECTION;
         flag=1;
         break;
       }
     }
     if(flag) {
       sel ? select_polygon(c, i, SELECTED, 1):  select_polygon(c, i, 0, 1);
     }
   }
   for(i=0;i<xctx->lines[c]; ++i)
   {
    double lx1, ly1, lx2, ly2;
    lx1 = xctx->line[c][i].x1;
    ly1 = xctx->line[c][i].y1;
    lx2 = xctx->line[c][i].x2;
    ly2 = xctx->line[c][i].y2;
    if(lineclip(&lx1, &ly1, &lx2, &ly2, x1,y1,x2,y2)) {
     xctx->ui_state |= SELECTION;
     sel? select_line(c,i,SELECTED,1): select_line(c,i,0,1);
    }
   }
   for(i=0;i<xctx->arcs[c]; ++i) {
     x = xctx->arc[c][i].x;
     y = xctx->arc[c][i].y;
     a = xctx->arc[c][i].a;
     b = xctx->arc[c][i].b;
     r = xctx->arc[c][i].r;
     xa = x + r * cos(a * XSCH_PI/180.);
     ya = y - r * sin(a * XSCH_PI/180.);
     xb = x + r * cos((a+b) * XSCH_PI/180.);
     yb = y - r * sin((a+b) * XSCH_PI/180.);
     arc_bbox(x, y, r, a, b, &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
     if(RECT_TOUCH(tmp.x1, tmp.y1, tmp.x2, tmp.y2, x1,y1,x2,y2)) {
       xctx->ui_state |= SELECTION; /* set xctx->ui_state to SELECTION also if unselecting by area ???? */
       sel? select_arc(c, i, SELECTED,1): select_arc(c, i, 0,1);
     }
   }
   for(i=0;i<xctx->rects[c]; ++i)
   {
    if(RECT_TOUCH(xctx->rect[c][i].x1,xctx->rect[c][i].y1,xctx->rect[c][i].x2,xctx->rect[c][i].y2, x1,y1,x2,y2))
    {
      xctx->ui_state |= SELECTION; /* set xctx->ui_state to SELECTION also if unselecting by area ???? */
      if(sel) {
        if(strboolcmp(get_tok_value(xctx->rect[c][i].prop_ptr, "lock", 0), "true")) {
          select_box(c,i, SELECTED, 1, 1);
        }
      } else {
        select_box(c,i, 0, 1, 0);
      }
    }
   } /* end for i */
 } /* end for c */
 drawtemparc(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
 drawtemprect(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
 drawtempline(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);

 if(!sel) {
   rebuild_selected_array();
   draw_selection(xctx->gc[SELLAYER], 0);
 }

}


int floaters_from_selected_inst()
{
  int res = 0, first = 1;
  int i, n, t;
  int instrot, instflip;
  double instx0, insty0;
  xSymbol *sym;
  for(n = 0; n < xctx->lastsel; ++n) {
    i = xctx->sel_array[n].n;
    if(xctx->sel_array[n].type == ELEMENT) {
      if(xctx->inst[i].ptr < 0) continue;
      if(xctx->inst[i].flags & HIDE_SYMBOL_TEXTS) continue;
      sym = xctx->sym + xctx->inst[i].ptr;
      if( sym->type && (IS_LABEL_SH_OR_PIN(sym->type) || !strcmp(sym->type, "probe") )) continue;
      instx0 = xctx->inst[i].x0;
      insty0 = xctx->inst[i].y0;
      instrot = xctx->inst[i].rot;
      instflip = xctx->inst[i].flip;
      if(first) {
        xctx->push_undo();
        set_modify(1);
        first = 0;
      }
      my_strdup2(_ALLOC_ID_, &xctx->inst[i].prop_ptr,
           subst_token(xctx->inst[i].prop_ptr, "hide_texts", "true"));
      set_inst_flags(&xctx->inst[i]);
      for(t = 0; t < sym->texts; t++) {
        double txtx0, txty0;
        int txtrot, txtflip;
        int rot, flip;
        double x0, y0;
        xText *symtxt;
        symtxt = &sym->text[t];
        if(strstr(symtxt->txt_ptr, ":net_name")) continue;
        txtx0 = symtxt->x0;
        txty0 = symtxt->y0;
        txtrot = symtxt->rot;
        txtflip = symtxt->flip;
       
        rot = (txtrot + ( (instflip && (txtrot & 1) ) ? instrot+2 : instrot) ) & 0x3;
        flip = txtflip ^ instflip;
  
        ROTATION(instrot, instflip, 0.0, 0.0, txtx0, txty0, x0, y0);
        x0 += instx0;
        y0 += insty0;
  
        create_text(0, x0, y0, rot, flip, symtxt->txt_ptr, 
              subst_token(symtxt->prop_ptr, "name", xctx->inst[i].instname),
              symtxt->xscale, symtxt->yscale);
        
        set_text_flags(symtxt);
        dbg(1, "instance %d: symtext %d: %s\n", i, t, symtxt->txt_ptr);
      }
    }
  }
  return res;
}

void select_all(void)
{
 int c,i;

 for(i=0;i<xctx->wires; ++i)
 {
   select_wire(i,SELECTED, 1);
 }
 for(i=0;i<xctx->texts; ++i)
 {
   select_text(i, SELECTED, 1);
 }
 for(i=0;i<xctx->instances; ++i)
 {
   select_element(i,SELECTED,1, 0);
 }
 for(c=0;c<cadlayers; ++c)
 {
  for(i=0;i<xctx->polygons[c]; ++i)
  {
    select_polygon(c,i,SELECTED,1);
  }
  for(i=0;i<xctx->lines[c]; ++i)
  {
    select_line(c,i,SELECTED,1);
  }
  for(i=0;i<xctx->arcs[c]; ++i)
  {
    select_arc(c,i, SELECTED, 1);
  }
  for(i=0;i<xctx->rects[c]; ++i)
  {
    select_box(c,i, SELECTED, 1, 0);
  }
 } /* end for c */
 drawtemparc(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
 drawtemprect(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
 drawtempline(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
 rebuild_selected_array(); /* sets or clears xctx->ui_state SELECTION flag */
}


