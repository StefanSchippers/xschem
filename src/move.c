/* File: move.c
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2021 Stefan Frederik Schippers
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

void rebuild_selected_array() /* can be used only if new selected set is lower */
                              /* that is, xctx->sel_array[] size can not increase */
{
 int i,c;

 dbg(2, "rebuild selected array\n");
 if(!xctx->need_reb_sel_arr) return;
 xctx->lastsel=0;
 for(i=0;i<xctx->texts;i++)
  if(xctx->text[i].sel)
  {
   check_selected_storage();
   xctx->sel_array[xctx->lastsel].type = xTEXT;
   xctx->sel_array[xctx->lastsel].n = i;
   xctx->sel_array[xctx->lastsel++].col = TEXTLAYER;
  }
 for(i=0;i<xctx->instances;i++)
  if(xctx->inst[i].sel)
  {
   check_selected_storage();
   xctx->sel_array[xctx->lastsel].type = ELEMENT;
   xctx->sel_array[xctx->lastsel].n = i;
   xctx->sel_array[xctx->lastsel++].col = WIRELAYER;
  }
 for(i=0;i<xctx->wires;i++)
  if(xctx->wire[i].sel)
  {
   check_selected_storage();
   xctx->sel_array[xctx->lastsel].type = WIRE;
   xctx->sel_array[xctx->lastsel].n = i;
   xctx->sel_array[xctx->lastsel++].col = WIRELAYER;
  }
 for(c=0;c<cadlayers;c++)
 {
  for(i=0;i<xctx->arcs[c];i++)
   if(xctx->arc[c][i].sel)
   {
    check_selected_storage();
    xctx->sel_array[xctx->lastsel].type = ARC;
    xctx->sel_array[xctx->lastsel].n = i;
    xctx->sel_array[xctx->lastsel++].col = c;
   }
  for(i=0;i<xctx->rects[c];i++)
   if(xctx->rect[c][i].sel)
   {
    check_selected_storage();
    xctx->sel_array[xctx->lastsel].type = xRECT;
    xctx->sel_array[xctx->lastsel].n = i;
    xctx->sel_array[xctx->lastsel++].col = c;
   }
  for(i=0;i<xctx->lines[c];i++)
   if(xctx->line[c][i].sel)
   {
    check_selected_storage();
    xctx->sel_array[xctx->lastsel].type = LINE;
    xctx->sel_array[xctx->lastsel].n = i;
    xctx->sel_array[xctx->lastsel++].col = c;
   }
  for(i=0;i<xctx->polygons[c];i++)
   if(xctx->poly[c][i].sel)
   {
    check_selected_storage();
    xctx->sel_array[xctx->lastsel].type = POLYGON;
    xctx->sel_array[xctx->lastsel].n = i;
    xctx->sel_array[xctx->lastsel++].col = c;
   }
 }
 if(xctx->lastsel==0) xctx->ui_state &= ~SELECTION;
 else xctx->ui_state |= SELECTION;
 xctx->need_reb_sel_arr=0;
}

void check_collapsing_objects()
{
  int  j,i, c;
  int found=0;

  j=0;
  for(i=0;i<xctx->wires;i++)
  {
   if(xctx->wire[i].x1==xctx->wire[i].x2 && xctx->wire[i].y1 == xctx->wire[i].y2)
   {
    my_free(812, &xctx->wire[i].prop_ptr);
    my_free(813, &xctx->wire[i].node);
    found=1;
    j++;
    continue;
   }
   if(j)
   {
    xctx->wire[i-j] = xctx->wire[i];
   }
  }
  xctx->wires -= j;

 /* option: remove degenerated lines  */
   for(c=0;c<cadlayers;c++)
   {
    j = 0;
    for(i=0;i<xctx->lines[c];i++)
    {
     if(xctx->line[c][i].x1==xctx->line[c][i].x2 && xctx->line[c][i].y1 == xctx->line[c][i].y2)
     {
      my_free(814, &xctx->line[c][i].prop_ptr);
      found=1;
      j++;
      continue;
     }
     if(j)
     {
      xctx->line[c][i-j] = xctx->line[c][i];
     }
    }
    xctx->lines[c] -= j;
   }
   for(c=0;c<cadlayers;c++)
   {
    j = 0;
    for(i=0;i<xctx->rects[c];i++)
    {
     if(xctx->rect[c][i].x1==xctx->rect[c][i].x2 || xctx->rect[c][i].y1 == xctx->rect[c][i].y2)
     {
      my_free(815, &xctx->rect[c][i].prop_ptr);
      found=1;
      j++;
      continue;
     }
     if(j)
     {
      xctx->rect[c][i-j] = xctx->rect[c][i];
     }
    }
    xctx->rects[c] -= j;
   }

  if(found) {
    xctx->need_reb_sel_arr=1;
    rebuild_selected_array();
  }
}

void update_symbol_bboxes(short rot, short flip)
{
  int i, n;
  short save_flip, save_rot;

  for(i=0;i<xctx->movelastsel;i++)
  {
    n = xctx->sel_array[i].n;
    dbg(1, "update_symbol_bboxes(): i=%d, movelastsel=%d, n=%d\n", i, xctx->movelastsel, n);
    if(xctx->sel_array[i].type == ELEMENT) {
      dbg(1, "update_symbol_bboxes(): symbol flip=%d, rot=%d\n",  xctx->inst[n].flip, xctx->inst[n].rot);
      save_flip = xctx->inst[n].flip;
      save_rot = xctx->inst[n].rot;
      xctx->inst[n].flip = flip ^ xctx->inst[n].flip;
      xctx->inst[n].rot = (xctx->inst[n].rot + rot) & 0x3;
      symbol_bbox(n, &xctx->inst[n].x1, &xctx->inst[n].y1, &xctx->inst[n].x2, &xctx->inst[n].y2 );
      xctx->inst[n].rot = save_rot;
      xctx->inst[n].flip = save_flip;
    }
  }
}
void draw_selection(GC g, int interruptable)
{
  int i, c, k, n;
  double  angle; /* arc */
  #if HAS_CAIRO==1
  int customfont;
  #endif

  if(g == xctx->gc[SELLAYER]) xctx->movelastsel = xctx->lastsel;
  for(i=0;i<xctx->movelastsel;i++)
  {
   c = xctx->sel_array[i].col;n = xctx->sel_array[i].n;
   switch(xctx->sel_array[i].type)
   {
    case xTEXT:
     if(xctx->rotatelocal) {
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->text[n].x0, xctx->text[n].y0, 
         xctx->text[n].x0, xctx->text[n].y0, xctx->rx1,xctx->ry1);
     } else {
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
                xctx->text[n].x0, xctx->text[n].y0, xctx->rx1,xctx->ry1);
     }
     #if HAS_CAIRO==1
     customfont =  set_text_custom_font(&xctx->text[n]);
     #endif
     draw_temp_string(g,ADD, xctx->text[n].txt_ptr,
      (xctx->text[n].rot +
      ( (xctx->move_flip && (xctx->text[n].rot & 1) ) ? xctx->move_rot+2 : xctx->move_rot) ) & 0x3,
       xctx->text[n].flip^xctx->move_flip, xctx->text[n].hcenter, xctx->text[n].vcenter,
       xctx->rx1+xctx->deltax, xctx->ry1+xctx->deltay,
       xctx->text[n].xscale, xctx->text[n].yscale);
     #if HAS_CAIRO==1
     if(customfont) cairo_restore(xctx->cairo_ctx);
     #endif

     break;
    case xRECT:
     if(xctx->rotatelocal) {
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->rect[c][n].x1, xctx->rect[c][n].y1,
         xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rx1,xctx->ry1);
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->rect[c][n].x1, xctx->rect[c][n].y1,
         xctx->rect[c][n].x2, xctx->rect[c][n].y2, xctx->rx2,xctx->ry2);
     } else {
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
                xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rx1,xctx->ry1);
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
                xctx->rect[c][n].x2, xctx->rect[c][n].y2, xctx->rx2,xctx->ry2);
     }
     if(xctx->rect[c][n].sel==SELECTED)
     {
       RECTORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
       drawtemprect(g, ADD, xctx->rx1+xctx->deltax, xctx->ry1+xctx->deltay,
                xctx->rx2+xctx->deltax, xctx->ry2+xctx->deltay);
     }
     else if(xctx->rect[c][n].sel==SELECTED1)
     {
      xctx->rx1+=xctx->deltax;
      xctx->ry1+=xctx->deltay;
      RECTORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
      drawtemprect(g, ADD, xctx->rx1, xctx->ry1, xctx->rx2, xctx->ry2);
     }
     else if(xctx->rect[c][n].sel==SELECTED2)
     {
      xctx->rx2+=xctx->deltax;
      xctx->ry1+=xctx->deltay;
      RECTORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
      drawtemprect(g, ADD, xctx->rx1, xctx->ry1, xctx->rx2, xctx->ry2);
     }
     else if(xctx->rect[c][n].sel==SELECTED3)
     {
      xctx->rx1+=xctx->deltax;
      xctx->ry2+=xctx->deltay;
      RECTORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
      drawtemprect(g, ADD, xctx->rx1, xctx->ry1, xctx->rx2, xctx->ry2);
     }
     else if(xctx->rect[c][n].sel==SELECTED4)
     {
      xctx->rx2+=xctx->deltax;
      xctx->ry2+=xctx->deltay;
      RECTORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
      drawtemprect(g, ADD, xctx->rx1, xctx->ry1, xctx->rx2, xctx->ry2);
     }
     else if(xctx->rect[c][n].sel==(SELECTED1|SELECTED2))
     {
      xctx->ry1+=xctx->deltay;
      RECTORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
      drawtemprect(g, ADD, xctx->rx1, xctx->ry1, xctx->rx2, xctx->ry2);
     }
     else if(xctx->rect[c][n].sel==(SELECTED3|SELECTED4))
     {
      xctx->ry2+=xctx->deltay;
      RECTORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
      drawtemprect(g, ADD, xctx->rx1, xctx->ry1, xctx->rx2, xctx->ry2);
     }
     else if(xctx->rect[c][n].sel==(SELECTED1|SELECTED3))
     {
      xctx->rx1+=xctx->deltax;
      RECTORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
      drawtemprect(g, ADD, xctx->rx1, xctx->ry1, xctx->rx2, xctx->ry2);
     }
     else if(xctx->rect[c][n].sel==(SELECTED2|SELECTED4))
     {
      xctx->rx2+=xctx->deltax;
      RECTORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
      drawtemprect(g, ADD, xctx->rx1, xctx->ry1, xctx->rx2, xctx->ry2);
     }
     break;
    case POLYGON:
     {
      double *x = my_malloc(223, sizeof(double) *xctx->poly[c][n].points);
      double *y = my_malloc(224, sizeof(double) *xctx->poly[c][n].points);
      if(xctx->poly[c][n].sel==SELECTED || xctx->poly[c][n].sel==SELECTED1) {
        for(k=0;k<xctx->poly[c][n].points; k++) {
          if( xctx->poly[c][n].sel==SELECTED || xctx->poly[c][n].selected_point[k]) {
            if(xctx->rotatelocal) {
              ROTATION(xctx->move_rot, xctx->move_flip, xctx->poly[c][n].x[0], xctx->poly[c][n].y[0],
                       xctx->poly[c][n].x[k], xctx->poly[c][n].y[k], xctx->rx1,xctx->ry1);
            } else {
              ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
                xctx->poly[c][n].x[k], xctx->poly[c][n].y[k], xctx->rx1,xctx->ry1);
            }
            x[k] = xctx->rx1 + xctx->deltax;
            y[k] = xctx->ry1 + xctx->deltay;
          } else {
            x[k] = xctx->poly[c][n].x[k];
            y[k] = xctx->poly[c][n].y[k];
          }
        }
        drawtemppolygon(g, NOW, x, y, xctx->poly[c][n].points);
      }
      my_free(816, &x);
      my_free(817, &y);
     }
     break;

    case WIRE:
     if(xctx->rotatelocal) {
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->wire[n].x1, xctx->wire[n].y1,
         xctx->wire[n].x1, xctx->wire[n].y1, xctx->rx1,xctx->ry1);
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->wire[n].x1, xctx->wire[n].y1,
         xctx->wire[n].x2, xctx->wire[n].y2, xctx->rx2,xctx->ry2);
     } else {
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
                xctx->wire[n].x1, xctx->wire[n].y1, xctx->rx1,xctx->ry1);
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
                xctx->wire[n].x2, xctx->wire[n].y2, xctx->rx2,xctx->ry2);
     }

     ORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
     if(xctx->wire[n].sel==SELECTED)
     {
      if(xctx->wire[n].bus)
        drawtempline(g, THICK, xctx->rx1+xctx->deltax, xctx->ry1+xctx->deltay,
                xctx->rx2+xctx->deltax, xctx->ry2+xctx->deltay);
      else
        drawtempline(g, ADD, xctx->rx1+xctx->deltax, xctx->ry1+xctx->deltay,
                xctx->rx2+xctx->deltax, xctx->ry2+xctx->deltay);
     }
     else if(xctx->wire[n].sel==SELECTED1)
     {
      if(xctx->wire[n].bus)
        drawtempline(g, THICK, xctx->rx1+xctx->deltax, xctx->ry1+xctx->deltay, xctx->rx2, xctx->ry2);
      else
        drawtempline(g, ADD, xctx->rx1+xctx->deltax, xctx->ry1+xctx->deltay, xctx->rx2, xctx->ry2);
     }
     else if(xctx->wire[n].sel==SELECTED2)
     {
      if(xctx->wire[n].bus)
        drawtempline(g, THICK, xctx->rx1, xctx->ry1, xctx->rx2+xctx->deltax, xctx->ry2+xctx->deltay);
      else
        drawtempline(g, ADD, xctx->rx1, xctx->ry1, xctx->rx2+xctx->deltax, xctx->ry2+xctx->deltay);
     }
     break;
    case LINE:
     if(xctx->rotatelocal) {
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->line[c][n].x1, xctx->line[c][n].y1,
         xctx->line[c][n].x1, xctx->line[c][n].y1, xctx->rx1,xctx->ry1);
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->line[c][n].x1, xctx->line[c][n].y1,
         xctx->line[c][n].x2, xctx->line[c][n].y2, xctx->rx2,xctx->ry2);
     } else {
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
                xctx->line[c][n].x1, xctx->line[c][n].y1, xctx->rx1,xctx->ry1);
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
                xctx->line[c][n].x2, xctx->line[c][n].y2, xctx->rx2,xctx->ry2);
     }
     ORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
     if(xctx->line[c][n].sel==SELECTED)
     {
       if(xctx->line[c][n].bus)
         drawtempline(g, THICK, xctx->rx1+xctx->deltax, xctx->ry1+xctx->deltay,
                xctx->rx2+xctx->deltax, xctx->ry2+xctx->deltay);
       else
         drawtempline(g, ADD, xctx->rx1+xctx->deltax, xctx->ry1+xctx->deltay,
                xctx->rx2+xctx->deltax, xctx->ry2+xctx->deltay);
     }
     else if(xctx->line[c][n].sel==SELECTED1)
     {
       if(xctx->line[c][n].bus)
         drawtempline(g, THICK, xctx->rx1+xctx->deltax, xctx->ry1+xctx->deltay, xctx->rx2, xctx->ry2);
       else
         drawtempline(g, ADD, xctx->rx1+xctx->deltax, xctx->ry1+xctx->deltay, xctx->rx2, xctx->ry2);
     }
     else if(xctx->line[c][n].sel==SELECTED2)
     {
       if(xctx->line[c][n].bus)
         drawtempline(g, THICK, xctx->rx1, xctx->ry1, xctx->rx2+xctx->deltax, xctx->ry2+xctx->deltay);
       else
         drawtempline(g, ADD, xctx->rx1, xctx->ry1, xctx->rx2+xctx->deltax, xctx->ry2+xctx->deltay);
     }
     break;
    case ARC:
     if(xctx->rotatelocal) {
       /* rotate center wrt itself: do nothing */
       xctx->rx1 = xctx->arc[c][n].x;
       xctx->ry1 = xctx->arc[c][n].y;
     } else {
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
                xctx->arc[c][n].x, xctx->arc[c][n].y, xctx->rx1,xctx->ry1);
     }
     angle = xctx->arc[c][n].a;
     if(xctx->move_flip) {
       angle = 270.*xctx->move_rot+180.-xctx->arc[c][n].b-xctx->arc[c][n].a;
     } else {
       angle = xctx->arc[c][n].a+xctx->move_rot*270.;
     }
     angle = fmod(angle, 360.);
     if(angle<0.) angle+=360.;
     if(xctx->arc[c][n].sel==SELECTED) {
       drawtemparc(g, ADD, xctx->rx1+xctx->deltax, xctx->ry1+xctx->deltay,
                xctx->arc[c][n].r, angle, xctx->arc[c][n].b);
     } else if(xctx->arc[c][n].sel==SELECTED1) {
       drawtemparc(g, ADD, xctx->rx1, xctx->ry1,
                fabs(xctx->arc[c][n].r+xctx->deltax), angle, xctx->arc[c][n].b);
     } else if(xctx->arc[c][n].sel==SELECTED3) {
       angle = my_round(fmod(atan2(-xctx->deltay, xctx->deltax)*180./XSCH_PI+xctx->arc[c][n].b, 360.));
       if(angle<0.) angle +=360.;
       if(angle==0) angle=360.;
       drawtemparc(g, ADD, xctx->rx1, xctx->ry1, xctx->arc[c][n].r, xctx->arc[c][n].a, angle);
     } else if(xctx->arc[c][n].sel==SELECTED2) {
       angle = my_round(fmod(atan2(-xctx->deltay, xctx->deltax)*180./XSCH_PI+angle, 360.));
       if(angle<0.) angle +=360.;
       drawtemparc(g, ADD, xctx->rx1, xctx->ry1, xctx->arc[c][n].r, angle, xctx->arc[c][n].b);
     }
     break;
    case ELEMENT:
     if(xctx->rotatelocal) {
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->inst[n].x0, xctx->inst[n].y0,
         xctx->inst[n].x0, xctx->inst[n].y0, xctx->rx1,xctx->ry1);
     } else {
       ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
                xctx->inst[n].x0, xctx->inst[n].y0, xctx->rx1,xctx->ry1);
     }
     for(k=0;k<cadlayers;k++)
      draw_temp_symbol(ADD, g, n, k, xctx->move_flip,
       ( xctx->move_flip && (xctx->inst[n].rot & 1) ) ? xctx->move_rot+2 : xctx->move_rot,
       xctx->rx1-xctx->inst[n].x0+xctx->deltax,xctx->ry1-xctx->inst[n].y0+xctx->deltay);
     break;
   }
#ifdef __unix__
   if(pending_events() && interruptable)
   {
    drawtemparc(g, END, 0.0, 0.0, 0.0, 0.0, 0.0);
    drawtemprect(g, END, 0.0, 0.0, 0.0, 0.0);
    drawtempline(g, END, 0.0, 0.0, 0.0, 0.0);
    xctx->movelastsel = i+1;
    return;
   }
#else
   if (interruptable)
   {
     drawtemparc(g, END, 0.0, 0.0, 0.0, 0.0, 0.0);
     drawtemprect(g, END, 0.0, 0.0, 0.0, 0.0);
     drawtempline(g, END, 0.0, 0.0, 0.0, 0.0);
     xctx->movelastsel = i + 1;
     return;
   }
#endif
  } /* for(i=0;i<xctx->movelastsel;i++) */
  drawtemparc(g, END, 0.0, 0.0, 0.0, 0.0, 0.0);
  drawtemprect(g, END, 0.0, 0.0, 0.0, 0.0);
  drawtempline(g, END, 0.0, 0.0, 0.0, 0.0);
  xctx->movelastsel = i;
}

/* 
 * build list of nodes attached to objects (wires, pins, wire labels) about to be moved/copied/deleted,
 * first time call with find_inst_to_be_redrawn(1), before doing the move/copy/delete,
 * then call with find_inst_to_be_redrawn(6) (2 | 4) after move/copy/delete is done.
 * what (bits can be ORed together):
 * 1: collect list of instances to be redrawn, add bboxes
 * 2: use previously collected list, add bboxes with updated node names, do this call after moving/deleting 
 *    objects and after a prepare_netlist_structs() 
 * 4: call symbol_bbox before bbox(ADD, ...) 
 * 8: do NOT build xctx->node_redraw_table using selected instances/wires
 * 16: clear hash and arrays
 * 32: call prepare_netlist_structs(0) if hilights or show net name on pins
 */
void find_inst_to_be_redrawn(int what)
{
  Int_hashentry *nentry;
  int i, n, p, rects;
  xSymbol * sym;
  xInstance * const inst = xctx->inst;

  int s_pnetname = tclgetboolvar("show_pin_net_names");

  dbg(1,"find_inst_to_be_redrawn(): what=%d\n", what);
  if(what & 16) {
    my_free(1202, &xctx->inst_redraw_table);
    xctx->inst_redraw_table_size = 0;
    if((s_pnetname || xctx->hilight_nets)) int_hash_free(xctx->node_redraw_table);
    return;
  }
  if((s_pnetname || xctx->hilight_nets)) {
    if(what & 32) prepare_netlist_structs(0);
    if(!(what & 8)) {
      for(i=0;i<xctx->lastsel;i++)
      {
        n = xctx->sel_array[i].n;
        if( xctx->sel_array[i].type == ELEMENT) {
          int p;
          char *type=xctx->sym[xctx->inst[n].ptr].type;
          /* collect all nodes connected to instances that set node names */
          if(type && IS_LABEL_OR_PIN(type)) {
            for(p = 0;  p < (inst[n].ptr + xctx->sym)->rects[PINLAYER]; p++) {
              if( inst[n].node && inst[n].node[p]) {
                dbg(1,"find_inst_to_be_redrawn(): hashing inst %s, node %s\n", inst[n].instname, inst[n].node[p]);
                int_hash_lookup(xctx->node_redraw_table, xctx->inst[n].node[p], 0, XINSERT_NOREPLACE);
              }
            }
          }
        }
        /* collect all nodes connected to selected wires (node names will change if wire deleted/moved) */
        if(xctx->sel_array[i].type == WIRE) {
          int_hash_lookup(xctx->node_redraw_table,  xctx->wire[n].node, 0, XINSERT_NOREPLACE);
        }
      }
    } /* if(!(what & 8)) */
  
    if(!xctx->inst_redraw_table || xctx->instances > xctx->inst_redraw_table_size) {
      my_realloc(1203, &xctx->inst_redraw_table, xctx->instances * sizeof(unsigned char));
      xctx->inst_redraw_table_size = xctx->instances;
    }
    for(i=0; i < xctx->instances; i++) {
      sym = xctx->inst[i].ptr + xctx->sym;
      rects = sym->rects[PINLAYER];
      if(what & 2 && xctx->inst_redraw_table[i]) {
        dbg(1, "find_inst_to_be_redrawn(): 1 bboxing inst %s\n", xctx->inst[i].instname);
        if(what & 1) bbox(ADD, xctx->inst[i].x1, xctx->inst[i].y1, xctx->inst[i].x2, xctx->inst[i].y2 );
        if(what & 4) {
          symbol_bbox(i, &inst[i].x1, &inst[i].y1, &inst[i].x2, &inst[i].y2 );
          bbox(ADD, xctx->inst[i].x1, xctx->inst[i].y1, xctx->inst[i].x2, xctx->inst[i].y2 );
        }
        continue;
      }
      for(p = 0; p < rects; p++) {
        if(xctx->inst[i].node && xctx->inst[i].node[p]) {
          nentry = int_hash_lookup(xctx->node_redraw_table, xctx->inst[i].node[p], 0, XLOOKUP);
          if(nentry) {
            dbg(1, "find_inst_to_be_redrawn(): 2 bboxing inst %s\n", xctx->inst[i].instname);
            if(what & 1) bbox(ADD, xctx->inst[i].x1, xctx->inst[i].y1, xctx->inst[i].x2, xctx->inst[i].y2);
            if(what & 4) {
              symbol_bbox(i, &inst[i].x1, &inst[i].y1, &inst[i].x2, &inst[i].y2);
              bbox(ADD, xctx->inst[i].x1, xctx->inst[i].y1, xctx->inst[i].x2, xctx->inst[i].y2);
            }
            xctx->inst_redraw_table[i] = 1;
            break;
          }
        }
      }
    }
  
    if(what & 5) for(i=0;i < xctx->wires; i++) {
      if(xctx->wire[i].node) {
        nentry = int_hash_lookup(xctx->node_redraw_table, xctx->wire[i].node, 0, XLOOKUP);
        if(nentry) {
          if(xctx->wire[i].bus){
            int ov, y1, y2;
            ov = INT_BUS_WIDTH(xctx->lw)> cadhalfdotsize ? INT_BUS_WIDTH(xctx->lw) : CADHALFDOTSIZE;
            if(xctx->wire[i].y1 < xctx->wire[i].y2) { y1 = xctx->wire[i].y1-ov; y2 = xctx->wire[i].y2+ov; }
            else                        { y1 = xctx->wire[i].y1+ov; y2 = xctx->wire[i].y2-ov; }
            dbg(1, "find_inst_to_be_redrawn(): 3 bboxing wire %d\n", i);
            bbox(ADD, xctx->wire[i].x1-ov, y1 , xctx->wire[i].x2+ov , y2 );
          } else {
            int ov, y1, y2;
            ov = cadhalfdotsize;
            if(xctx->wire[i].y1 < xctx->wire[i].y2) { y1 = xctx->wire[i].y1-ov; y2 = xctx->wire[i].y2+ov; }
            else                        { y1 = xctx->wire[i].y1+ov; y2 = xctx->wire[i].y2-ov; }
            dbg(1, "find_inst_to_be_redrawn(): 4 bboxing wire %d\n", i);
            bbox(ADD, xctx->wire[i].x1-ov, y1 , xctx->wire[i].x2+ov , y2 );
          }
        }
      }
    }
  } /* if((s_pnetname || xctx->hilight_nets)) */
  if(!(what & 8) ) {
    if(what & 5) for(i=0;i<xctx->lastsel;i++) { /* add bboxes of selected objects */
      n = xctx->sel_array[i].n;
      if( xctx->sel_array[i].type == ELEMENT) {
        dbg(1, "find_inst_to_be_redrawn(): 5 bboxing inst %s\n", xctx->inst[n].instname);
        if(what & 1) bbox(ADD, inst[n].x1, inst[n].y1, inst[n].x2, inst[n].y2 );
        if(what & 4) {
          symbol_bbox(n, &inst[n].x1, &inst[n].y1, &inst[n].x2, &inst[n].y2);
          bbox(ADD, inst[n].x1, inst[n].y1, inst[n].x2, inst[n].y2);
        }
      }
    }
  }
}

void copy_objects(int what)
{
  int tmpi, c, i, n, k /*, tmp */ ;
  xRect tmp;
  double angle;
  int newpropcnt;
  double tmpx, tmpy;
  const char *str;
 
  #if HAS_CAIRO==1
  int customfont;
  #endif
 
  if(what & START)
  {
   xctx->rotatelocal=0;
   dbg(1, "copy_objects(): START copy\n");
   rebuild_selected_array();
   save_selection(1);
   xctx->deltax = xctx->deltay = 0.0;
   xctx->movelastsel = xctx->lastsel;
   xctx->x1=xctx->mousex_snap;xctx->y_1=xctx->mousey_snap;
   xctx->move_flip = 0;xctx->move_rot = 0;
   xctx->ui_state|=STARTCOPY;
  }
  if(what & ABORT)                               /* draw objects while moving */
  {
   char *str = NULL; /* 20161122 overflow safe */
   draw_selection(xctx->gctiled,0);
   xctx->move_rot=xctx->move_flip=xctx->deltax=xctx->deltay=0;
   xctx->ui_state&=~STARTCOPY;
   my_strdup(225, &str, user_conf_dir);
   my_strcat(226, &str, "/.selection.sch");
   xunlink(str);
   update_symbol_bboxes(0, 0);
   my_free(818, &str);
  }
  if(what & RUBBER)                              /* draw objects while moving */
  {
   xctx->x2=xctx->mousex_snap;xctx->y_2=xctx->mousey_snap;
   draw_selection(xctx->gctiled,0);
   xctx->deltax = xctx->x2-xctx->x1; xctx->deltay = xctx->y_2 - xctx->y_1;
   draw_selection(xctx->gc[SELLAYER],1);
  }
  if(what & ROTATELOCAL ) {
   xctx->rotatelocal=1;
  }
  if(what & ROTATE) {
   draw_selection(xctx->gctiled,0);
   xctx->move_rot= (xctx->move_rot+1) & 0x3;
   update_symbol_bboxes(xctx->move_rot, xctx->move_flip);
  }
  if(what & FLIP)
  {
   draw_selection(xctx->gctiled,0);
   xctx->move_flip = !xctx->move_flip;
   update_symbol_bboxes(xctx->move_rot, xctx->move_flip);
  }
  if(what & END)                                 /* copy selected objects */
  {
    int l, firstw, firsti;
    bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
    newpropcnt=0;
    set_modify(1); xctx->push_undo(); /* 20150327 push_undo */
    
    firstw = firsti = 1;

    draw_selection(xctx->gctiled,0);
    update_symbol_bboxes(0, 0);
    find_inst_to_be_redrawn(0); /* build list before copying and recalculating prepare_netlist_structs() */

    for(i=0;i<xctx->lastsel;i++)
    {
      n = xctx->sel_array[i].n;
      if(xctx->sel_array[i].type == WIRE)
      {
        xctx->prep_hash_wires=0;
        firstw = 0;
        check_wire_storage();
        if(xctx->rotatelocal) {
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->wire[n].x1, xctx->wire[n].y1,
            xctx->wire[n].x1, xctx->wire[n].y1, xctx->rx1,xctx->ry1);
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->wire[n].x1, xctx->wire[n].y1,
            xctx->wire[n].x2, xctx->wire[n].y2, xctx->rx2,xctx->ry2);
        } else {
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
             xctx->wire[n].x1, xctx->wire[n].y1, xctx->rx1,xctx->ry1);
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
             xctx->wire[n].x2, xctx->wire[n].y2, xctx->rx2,xctx->ry2);
        }
        if( xctx->wire[n].sel & (SELECTED|SELECTED1) )
        {
         xctx->rx1+=xctx->deltax;
         xctx->ry1+=xctx->deltay;
        }
        if( xctx->wire[n].sel & (SELECTED|SELECTED2) )
        {
         xctx->rx2+=xctx->deltax;
         xctx->ry2+=xctx->deltay;
        }
        tmpx=xctx->rx1; /* used as temporary storage */
        tmpy=xctx->ry1;
        ORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
        if( tmpx == xctx->rx2 &&  tmpy == xctx->ry2)
        {
         if(xctx->wire[n].sel == SELECTED1) xctx->wire[n].sel = SELECTED2;
         else if(xctx->wire[n].sel == SELECTED2) xctx->wire[n].sel = SELECTED1;
        }
        xctx->sel_array[i].n=xctx->wires;
        storeobject(-1, xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2,WIRE,0,xctx->wire[n].sel,xctx->wire[n].prop_ptr);
        xctx->wire[n].sel=0;
  
        l = xctx->wires -1;
        if(xctx->wire[n].bus){
          int ov, y1, y2;
          ov = INT_BUS_WIDTH(xctx->lw)> cadhalfdotsize ? INT_BUS_WIDTH(xctx->lw) : CADHALFDOTSIZE;
          if(xctx->wire[l].y1 < xctx->wire[l].y2) { y1 = xctx->wire[l].y1-ov; y2 = xctx->wire[l].y2+ov; }
          else                        { y1 = xctx->wire[l].y1+ov; y2 = xctx->wire[l].y2-ov; }
          bbox(ADD, xctx->wire[l].x1-ov, y1 , xctx->wire[l].x2+ov , y2 );
        } else {
          int ov, y1, y2;
          ov = cadhalfdotsize;
          if(xctx->wire[l].y1 < xctx->wire[l].y2) { y1 = xctx->wire[l].y1-ov; y2 = xctx->wire[l].y2+ov; }
          else                        { y1 = xctx->wire[l].y1+ov; y2 = xctx->wire[l].y2-ov; }
          bbox(ADD, xctx->wire[l].x1-ov, y1 , xctx->wire[l].x2+ov , y2 );
        }
      }
    }
  
    for(k=0;k<cadlayers;k++)
    {
     for(i=0;i<xctx->lastsel;i++)
     {
      c = xctx->sel_array[i].col;n = xctx->sel_array[i].n;
      switch(xctx->sel_array[i].type)
      {
       case LINE:
        if(c!=k) break;
        if(xctx->rotatelocal) {
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->line[c][n].x1, xctx->line[c][n].y1,
            xctx->line[c][n].x1, xctx->line[c][n].y1, xctx->rx1,xctx->ry1);
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->line[c][n].x1, xctx->line[c][n].y1,
            xctx->line[c][n].x2, xctx->line[c][n].y2, xctx->rx2,xctx->ry2);
        } else {
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
             xctx->line[c][n].x1, xctx->line[c][n].y1, xctx->rx1,xctx->ry1);
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
             xctx->line[c][n].x2, xctx->line[c][n].y2, xctx->rx2,xctx->ry2);
        }
        if( xctx->line[c][n].sel & (SELECTED|SELECTED1) )
        {
         xctx->rx1+=xctx->deltax;
         xctx->ry1+=xctx->deltay;
        }
        if( xctx->line[c][n].sel & (SELECTED|SELECTED2) )
        {
         xctx->rx2+=xctx->deltax;
         xctx->ry2+=xctx->deltay;
        }
        tmpx=xctx->rx1;
        tmpy=xctx->ry1;
        ORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
        if( tmpx == xctx->rx2 &&  tmpy == xctx->ry2)
        {
         if(xctx->line[c][n].sel == SELECTED1) xctx->line[c][n].sel = SELECTED2;
         else if(xctx->line[c][n].sel == SELECTED2) xctx->line[c][n].sel = SELECTED1;
        }
        xctx->sel_array[i].n=xctx->lines[c];
        storeobject(-1, xctx->rx1, xctx->ry1, xctx->rx2, xctx->ry2, LINE, c,
           xctx->line[c][n].sel, xctx->line[c][n].prop_ptr);
        xctx->line[c][n].sel=0;
        
        l = xctx->lines[c] - 1;
        if(xctx->line[c][l].bus){
          int ov, y1, y2;
          ov = INT_BUS_WIDTH(xctx->lw)> cadhalfdotsize ? INT_BUS_WIDTH(xctx->lw) : CADHALFDOTSIZE;
          if(xctx->line[c][l].y1 < xctx->line[c][l].y2) 
               { y1 = xctx->line[c][l].y1-ov; y2 = xctx->line[c][l].y2+ov; }
          else { y1 = xctx->line[c][l].y1+ov; y2 = xctx->line[c][l].y2-ov; }
          bbox(ADD, xctx->line[c][l].x1-ov, y1 , xctx->line[c][l].x2+ov , y2 );
        } else {
          int ov, y1, y2;
          ov = cadhalfdotsize;
          if(xctx->line[c][l].y1 < xctx->line[c][l].y2) 
                { y1 = xctx->line[c][l].y1-ov; y2 = xctx->line[c][l].y2+ov; }
          else  { y1 = xctx->line[c][l].y1+ov; y2 = xctx->line[c][l].y2-ov; }
          bbox(ADD, xctx->line[c][l].x1-ov, y1 , xctx->line[c][l].x2+ov , y2 );
        }
        break;
  
       case POLYGON:
        if(c!=k) break;
        {
          xPoly *p = &xctx->poly[c][n];
          double bx1 = 0.0, by1 = 0.0, bx2 = 0.0, by2 = 0.0;
          double *x = my_malloc(227, sizeof(double) *p->points);
          double *y = my_malloc(228, sizeof(double) *p->points);
          int j;
          for(j=0; j<p->points; j++) {
            if( p->sel==SELECTED || p->selected_point[j]) {
              if(xctx->rotatelocal) {
                ROTATION(xctx->move_rot, xctx->move_flip, p->x[0], p->y[0], p->x[j], p->y[j], xctx->rx1,xctx->ry1);
              } else {
                ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1, p->x[j], p->y[j], xctx->rx1,xctx->ry1);
              }
              x[j] = xctx->rx1+xctx->deltax;
              y[j] = xctx->ry1+xctx->deltay;
            } else {
              x[j] = p->x[j];
              y[j] = p->y[j];
            }
            if(j==0 || x[j] < bx1) bx1 = x[j];
            if(j==0 || y[j] < by1) by1 = y[j];
            if(j==0 || x[j] > bx2) bx2 = x[j];
            if(j==0 || y[j] > by2) by2 = y[j];
          }
          bbox(ADD, bx1, by1, bx2, by2);
          xctx->sel_array[i].n=xctx->polygons[c];
          store_poly(-1, x, y, p->points, c, p->sel, p->prop_ptr);
          p->sel=0;
          my_free(819, &x);
          my_free(820, &y);
        }
        break;
       case ARC:
        if(c!=k) break;
        if(xctx->rotatelocal) {
          /* rotate center wrt itself: do nothing */
          xctx->rx1 = xctx->arc[c][n].x;
          xctx->ry1 = xctx->arc[c][n].y;
        } else {
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
             xctx->arc[c][n].x, xctx->arc[c][n].y, xctx->rx1,xctx->ry1);
        }
        angle = xctx->arc[c][n].a;
        if(xctx->move_flip) {
          angle = 270.*xctx->move_rot+180.-xctx->arc[c][n].b-xctx->arc[c][n].a;
        } else {
          angle = xctx->arc[c][n].a+xctx->move_rot*270.;
        }
        angle = fmod(angle, 360.);
        if(angle<0.) angle+=360.;
        xctx->arc[c][n].sel=0;
        xctx->sel_array[i].n=xctx->arcs[c];
  
        store_arc(-1, xctx->rx1+xctx->deltax, xctx->ry1+xctx->deltay,
                   xctx->arc[c][n].r, angle, xctx->arc[c][n].b, c, SELECTED, xctx->arc[c][n].prop_ptr);
     
        l = xctx->arcs[c] - 1;
        if(xctx->arc[c][l].fill)
          arc_bbox(xctx->arc[c][l].x, xctx->arc[c][l].y, xctx->arc[c][l].r, 0, 360,
                   &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
        else
          arc_bbox(xctx->arc[c][l].x, xctx->arc[c][l].y, xctx->arc[c][l].r,
                   xctx->arc[c][l].a, xctx->arc[c][n].b,
                   &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
        bbox(ADD, tmp.x1, tmp.y1, tmp.x2, tmp.y2);
        break;
  
       case xRECT:
        if(c!=k) break;
        if(xctx->rotatelocal) {
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->rect[c][n].x1, xctx->rect[c][n].y1,
            xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rx1,xctx->ry1);
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->rect[c][n].x1, xctx->rect[c][n].y1,
            xctx->rect[c][n].x2, xctx->rect[c][n].y2, xctx->rx2,xctx->ry2);
        } else {
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
             xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rx1,xctx->ry1);
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
             xctx->rect[c][n].x2, xctx->rect[c][n].y2, xctx->rx2,xctx->ry2);
        }
        RECTORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
        xctx->rect[c][n].sel=0;
        xctx->sel_array[i].n=xctx->rects[c];
        storeobject(-1, xctx->rx1+xctx->deltax, xctx->ry1+xctx->deltay,
                   xctx->rx2+xctx->deltax, xctx->ry2+xctx->deltay,xRECT, c, SELECTED, xctx->rect[c][n].prop_ptr);
        l = xctx->rects[c] - 1;
        bbox(ADD, xctx->rect[c][l].x1, xctx->rect[c][l].y1, xctx->rect[c][l].x2, xctx->rect[c][l].y2);
        break;
  
       case xTEXT:
        if(k!=TEXTLAYER) break;
        check_text_storage();
        if(xctx->rotatelocal) {
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->text[n].x0, xctx->text[n].y0,
           xctx->text[n].x0, xctx->text[n].y0, xctx->rx1,xctx->ry1);
        } else {
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
             xctx->text[n].x0, xctx->text[n].y0, xctx->rx1,xctx->ry1);
        }
        xctx->text[xctx->texts].txt_ptr=NULL;
        my_strdup(229, &xctx->text[xctx->texts].txt_ptr,xctx->text[n].txt_ptr);
        xctx->text[n].sel=0;
         dbg(2, "copy_objects(): current str=%s\n",
          xctx->text[xctx->texts].txt_ptr);
        xctx->text[xctx->texts].x0=xctx->rx1+xctx->deltax;
        xctx->text[xctx->texts].y0=xctx->ry1+xctx->deltay;
        xctx->text[xctx->texts].rot=(xctx->text[n].rot +
         ( (xctx->move_flip && (xctx->text[n].rot & 1) ) ? xctx->move_rot+2 : xctx->move_rot) ) & 0x3;
        xctx->text[xctx->texts].flip=xctx->move_flip^xctx->text[n].flip;
        xctx->text[xctx->texts].sel=SELECTED;
        xctx->text[xctx->texts].prop_ptr=NULL;
        xctx->text[xctx->texts].font=NULL;
        my_strdup(230, &xctx->text[xctx->texts].prop_ptr, xctx->text[n].prop_ptr);
        my_strdup(231, &xctx->text[xctx->texts].font, get_tok_value(xctx->text[xctx->texts].prop_ptr, "font", 0));
  
        str = get_tok_value(xctx->text[xctx->texts].prop_ptr, "hcenter", 0);
        xctx->text[xctx->texts].hcenter = strcmp(str, "true")  ? 0 : 1;
        str = get_tok_value(xctx->text[xctx->texts].prop_ptr, "vcenter", 0);
        xctx->text[xctx->texts].vcenter = strcmp(str, "true")  ? 0 : 1;
  
        str = get_tok_value(xctx->text[xctx->texts].prop_ptr, "layer", 0);
        if(str[0]) xctx->text[xctx->texts].layer = atoi(str);
        else xctx->text[xctx->texts].layer = -1;
  
        xctx->text[xctx->texts].flags = 0;
        str = get_tok_value(xctx->text[xctx->texts].prop_ptr, "slant", 0);
        xctx->text[xctx->texts].flags |= strcmp(str, "oblique")  ? 0 : TEXT_OBLIQUE;
        xctx->text[xctx->texts].flags |= strcmp(str, "italic")  ? 0 : TEXT_ITALIC;
        str = get_tok_value(xctx->text[xctx->texts].prop_ptr, "weight", 0);
        xctx->text[xctx->texts].flags |= strcmp(str, "bold")  ? 0 : TEXT_BOLD;
        str = get_tok_value(xctx->text[xctx->texts].prop_ptr, "hide", 0);
        xctx->text[xctx->texts].flags |= strcmp(str, "true")  ? 0 : SYM_HIDE_TEXT;
  
        xctx->text[xctx->texts].xscale=xctx->text[n].xscale;
        xctx->text[xctx->texts].yscale=xctx->text[n].yscale;
  
        l = xctx->texts;
        
        #if HAS_CAIRO==1 /* bbox after copy */
        customfont = set_text_custom_font(&xctx->text[l]);
        #endif
        text_bbox(xctx->text[l].txt_ptr, xctx->text[l].xscale,
          xctx->text[l].yscale, xctx->text[l].rot,xctx->text[l].flip, 
          xctx->text[l].hcenter, xctx->text[l].vcenter,
          xctx->text[l].x0, xctx->text[l].y0,
          &xctx->rx1,&xctx->ry1, &xctx->rx2,&xctx->ry2, &tmpi, &tmpi);
        #if HAS_CAIRO==1
        if(customfont) cairo_restore(xctx->cairo_ctx);
        #endif
        bbox(ADD, xctx->rx1, xctx->ry1, xctx->rx2, xctx->ry2 );
  
        xctx->sel_array[i].n=xctx->texts;
        xctx->texts++;
         dbg(2, "copy_objects(): done copy string\n");
        break;
       default:
        break;
      } /* end switch(xctx->sel_array[i].type) */
     } /* end for(i=0;i<xctx->lastsel;i++) */
  
  
    } /* end for(k=0;k<cadlayers;k++) */
  
  
  
  
    for(i = 0; i < xctx->lastsel; i++) {
      n = xctx->sel_array[i].n;
      if(xctx->sel_array[i].type == ELEMENT) {
        xctx->prep_hash_inst = 0;
        firsti = 0;
        check_inst_storage();
        if(xctx->rotatelocal) {
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->inst[n].x0, xctx->inst[n].y0,
             xctx->inst[n].x0, xctx->inst[n].y0, xctx->rx1,xctx->ry1);
        } else {
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
             xctx->inst[n].x0, xctx->inst[n].y0, xctx->rx1,xctx->ry1);
        }
        xctx->inst[xctx->instances] = xctx->inst[n];
        xctx->inst[xctx->instances].prop_ptr=NULL;
        xctx->inst[xctx->instances].instname=NULL;
        xctx->inst[xctx->instances].lab=NULL;
        xctx->inst[xctx->instances].node=NULL;
        xctx->inst[xctx->instances].name=NULL;
        my_strdup2(232, &xctx->inst[xctx->instances].name, xctx->inst[n].name);
        my_strdup(233, &xctx->inst[xctx->instances].prop_ptr, xctx->inst[n].prop_ptr);
        /* 
         * my_strdup2(234, &xctx->inst[xctx->instances].instname, get_tok_value(xctx->inst[n].prop_ptr, "name",0));
         */
        xctx->inst[xctx->instances].instname = NULL; /* will be set in new_prop_string() */
        my_strdup(312, &xctx->inst[xctx->instances].lab, xctx->inst[n].lab);
        xctx->inst[n].sel=0;
        xctx->inst[xctx->instances].flags = xctx->inst[n].flags;
        xctx->inst[xctx->instances].color = -10000;
        xctx->inst[xctx->instances].x0 = xctx->rx1+xctx->deltax;
        xctx->inst[xctx->instances].y0 = xctx->ry1+xctx->deltay;
        xctx->inst[xctx->instances].sel = SELECTED;
        xctx->inst[xctx->instances].rot = (xctx->inst[xctx->instances].rot + ( (xctx->move_flip && 
           (xctx->inst[xctx->instances].rot & 1) ) ? xctx->move_rot+2 : xctx->move_rot) ) & 0x3;
        xctx->inst[xctx->instances].flip = (xctx->move_flip? !xctx->inst[n].flip:xctx->inst[n].flip);
        /* the newpropcnt argument is zero for the 1st call and used in  */
        /* new_prop_string() for cleaning some internal caches. */
        if(!newpropcnt) hash_all_names(xctx->instances);
        new_prop_string(xctx->instances, xctx->inst[n].prop_ptr,newpropcnt++, 
          tclgetboolvar("disable_unique_names"));
        xctx->instances++;
      } /* if(xctx->sel_array[i].type == ELEMENT) */
    }  /* for(i = 0; i < xctx->lastsel; i++) */
    xctx->need_reb_sel_arr=1;
    rebuild_selected_array();
    if(!firsti || !firstw) {
      xctx->prep_net_structs=0;
      xctx->prep_hi_structs=0;
    }
    /* build after copying and after recalculating prepare_netlist_structs() */
    find_inst_to_be_redrawn(1 + 2 + 4 + 32); /* 32: call prepare_netlist_structs(0) */
    find_inst_to_be_redrawn(16); /* clear data */
    check_collapsing_objects();
    if(tclgetboolvar("autotrim_wires")) trim_wires();
    /* update_conn_cues(1, 1); */
    if(xctx->hilight_nets) {
      propagate_hilights(1, 1, XINSERT_NOREPLACE);
    }
    xctx->ui_state &= ~STARTCOPY;
    xctx->x1=xctx->y_1=xctx->x2=xctx->y_2=xctx->move_rot=xctx->move_flip=xctx->deltax=xctx->deltay=0;
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
    draw();
    bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
    xctx->rotatelocal=0;
  } /* if(what & END) */
  draw_selection(xctx->gc[SELLAYER], 0);
}


/* merge param unused, RFU */
void move_objects(int what, int merge, double dx, double dy)
{
 int c, i, n, k, tmpint;
 xRect tmp;
 double angle;
 double tx1,ty1; /* temporaries for swapping coordinates 20070302 */
 #if HAS_CAIRO==1
 int customfont;
 #endif
 xLine ** const line = xctx->line;
 xWire * const wire = xctx->wire;

 if(what & START)
 {
  xctx->rotatelocal=0;
  xctx->deltax = xctx->deltay = 0.0;
  rebuild_selected_array();
  if(tclgetboolvar("connect_by_kissing")) xctx->kissing = connect_by_kissing();
  else xctx->kissing = 0;
  xctx->movelastsel = xctx->lastsel;
  if(xctx->lastsel==1 && xctx->sel_array[0].type==ARC &&
          xctx->arc[c=xctx->sel_array[0].col][n=xctx->sel_array[0].n].sel!=SELECTED) {
    xctx->x1 = xctx->arc[c][n].x;
    xctx->y_1 = xctx->arc[c][n].y;
  } else {xctx->x1=xctx->mousex_snap;xctx->y_1=xctx->mousey_snap;}
  xctx->move_flip = 0;xctx->move_rot = 0;
  xctx->ui_state|=STARTMOVE;
 }
 if(what & ABORT)                               /* draw objects while moving */
 {
  if(xctx->kissing) pop_undo(0, 0);
  draw_selection(xctx->gctiled,0);
  xctx->move_rot=xctx->move_flip=xctx->deltax=xctx->deltay=0;
  xctx->ui_state &= ~STARTMOVE;
  update_symbol_bboxes(0, 0);

 }
 if(what & RUBBER)                              /* abort operation */
 {
  xctx->x2=xctx->mousex_snap;xctx->y_2=xctx->mousey_snap;
  draw_selection(xctx->gctiled,0);
  xctx->deltax = xctx->x2-xctx->x1; xctx->deltay = xctx->y_2 - xctx->y_1;
  draw_selection(xctx->gc[SELLAYER],1);
 }
 if(what & ROTATELOCAL) {
  xctx->rotatelocal=1;
 }
 if(what & ROTATE) {
  draw_selection(xctx->gctiled,0);
  xctx->move_rot= (xctx->move_rot+1) & 0x3;
  update_symbol_bboxes(xctx->move_rot, xctx->move_flip);
 }
 if(what & FLIP)
 {
  draw_selection(xctx->gctiled,0);
  xctx->move_flip = !xctx->move_flip;
  update_symbol_bboxes(xctx->move_rot, xctx->move_flip);
 }
 if(what & END)                                 /* move selected objects */
 {
  int firsti, firstw;

  bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
  set_modify(1);
  /* no undo push for MERGE ad PLACE, already done before */
  if( !xctx->kissing && !(xctx->ui_state & (STARTMERGE | PLACE_SYMBOL | PLACE_TEXT)) ) {
    dbg(0, "move_objects(): push undo state\n");
    xctx->push_undo();
  }
  xctx->ui_state &= ~PLACE_SYMBOL;
  xctx->ui_state &= ~PLACE_TEXT;
  if(dx!=0.0 || dy!=0.0) {
    xctx->deltax = dx;
    xctx->deltay = dy;
  }

  /* calculate moving symbols bboxes before actually doing the move */
  firsti = firstw = 1;
  draw_selection(xctx->gctiled,0);
  update_symbol_bboxes(0, 0);
  find_inst_to_be_redrawn(0); /* build list before moving and recalculating prepare_netlist_structs() */
  for(k=0;k<cadlayers;k++)
  {
   for(i=0;i<xctx->lastsel;i++)
   {
    c = xctx->sel_array[i].col;n = xctx->sel_array[i].n;
    switch(xctx->sel_array[i].type)
    {
     case WIRE:
      xctx->prep_hash_wires=0;
      firstw = 0;
      if(k == 0) {

        if(wire[n].bus){ /* bbox before move */
          int ov, y1, y2;
          ov = INT_BUS_WIDTH(xctx->lw)> cadhalfdotsize ? INT_BUS_WIDTH(xctx->lw) : CADHALFDOTSIZE;
          if(wire[n].y1 < wire[n].y2) { y1 = wire[n].y1-ov; y2 = wire[n].y2+ov; }
          else                        { y1 = wire[n].y1+ov; y2 = wire[n].y2-ov; }
          bbox(ADD, wire[n].x1-ov, y1 , wire[n].x2+ov , y2 );
        } else {
          int ov, y1, y2;
          ov = cadhalfdotsize;
          if(wire[n].y1 < wire[n].y2) { y1 = wire[n].y1-ov; y2 = wire[n].y2+ov; }
          else                        { y1 = wire[n].y1+ov; y2 = wire[n].y2-ov; }
          bbox(ADD, wire[n].x1-ov, y1 , wire[n].x2+ov , y2 );
        }

        if(xctx->rotatelocal) {
          ROTATION(xctx->move_rot, xctx->move_flip, wire[n].x1, wire[n].y1,
             wire[n].x1, wire[n].y1, xctx->rx1,xctx->ry1);
          ROTATION(xctx->move_rot, xctx->move_flip, wire[n].x1, wire[n].y1,
             wire[n].x2, wire[n].y2, xctx->rx2,xctx->ry2);
        } else {
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
             wire[n].x1, wire[n].y1, xctx->rx1,xctx->ry1);
          ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
             wire[n].x2, wire[n].y2, xctx->rx2,xctx->ry2);
        }
        if( wire[n].sel & (SELECTED|SELECTED1) )
        {
         xctx->rx1+=xctx->deltax;
         xctx->ry1+=xctx->deltay;
        }
        if( wire[n].sel & (SELECTED|SELECTED2) )
        {
         xctx->rx2+=xctx->deltax;
         xctx->ry2+=xctx->deltay;
        }
        wire[n].x1=xctx->rx1;
        wire[n].y1=xctx->ry1;
        ORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
        if( wire[n].x1 == xctx->rx2 &&  wire[n].y1 == xctx->ry2)
        {
         if(wire[n].sel == SELECTED1) wire[n].sel = SELECTED2;
         else if(wire[n].sel == SELECTED2) wire[n].sel = SELECTED1;
        }
        wire[n].x1=xctx->rx1;
        wire[n].y1=xctx->ry1;
        wire[n].x2=xctx->rx2;
        wire[n].y2=xctx->ry2;

        if(wire[n].bus){ /* bbox after move */
          int ov, y1, y2;
          ov = INT_BUS_WIDTH(xctx->lw)> cadhalfdotsize ? INT_BUS_WIDTH(xctx->lw) : CADHALFDOTSIZE;
          if(wire[n].y1 < wire[n].y2) { y1 = wire[n].y1-ov; y2 = wire[n].y2+ov; }
          else                        { y1 = wire[n].y1+ov; y2 = wire[n].y2-ov; }
          bbox(ADD, wire[n].x1-ov, y1 , wire[n].x2+ov , y2 );
        } else {
          int ov, y1, y2;
          ov = cadhalfdotsize;
          if(wire[n].y1 < wire[n].y2) { y1 = wire[n].y1-ov; y2 = wire[n].y2+ov; }
          else                        { y1 = wire[n].y1+ov; y2 = wire[n].y2-ov; }
          bbox(ADD, wire[n].x1-ov, y1 , wire[n].x2+ov , y2 );
        }
      }
      break;

     case LINE:
      if(c!=k) break;
      if(xctx->line[c][n].bus){ /* bbox before move */
        int ov, y1, y2;
        ov = INT_BUS_WIDTH(xctx->lw)> cadhalfdotsize ? INT_BUS_WIDTH(xctx->lw) : CADHALFDOTSIZE;
        if(xctx->line[c][n].y1 < xctx->line[c][n].y2)
             { y1 = xctx->line[c][n].y1-ov; y2 = xctx->line[c][n].y2+ov; }
        else { y1 = xctx->line[c][n].y1+ov; y2 = xctx->line[c][n].y2-ov; }
        bbox(ADD, xctx->line[c][n].x1-ov, y1 , xctx->line[c][n].x2+ov , y2 );
      } else {
        int ov, y1, y2;
        ov = cadhalfdotsize;
        if(xctx->line[c][n].y1 < xctx->line[c][n].y2)
              { y1 = xctx->line[c][n].y1-ov; y2 = xctx->line[c][n].y2+ov; }
        else  { y1 = xctx->line[c][n].y1+ov; y2 = xctx->line[c][n].y2-ov; }
        bbox(ADD, xctx->line[c][n].x1-ov, y1 , xctx->line[c][n].x2+ov , y2 );
      }

      if(xctx->rotatelocal) {
        ROTATION(xctx->move_rot, xctx->move_flip, line[c][n].x1, line[c][n].y1,
           line[c][n].x1, line[c][n].y1, xctx->rx1,xctx->ry1);
        ROTATION(xctx->move_rot, xctx->move_flip, line[c][n].x1, line[c][n].y1,
           line[c][n].x2, line[c][n].y2, xctx->rx2,xctx->ry2);
      } else {
        ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
           line[c][n].x1, line[c][n].y1, xctx->rx1,xctx->ry1);
        ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
           line[c][n].x2, line[c][n].y2, xctx->rx2,xctx->ry2);
      }

      if( line[c][n].sel & (SELECTED|SELECTED1) )
      {
       xctx->rx1+=xctx->deltax;
       xctx->ry1+=xctx->deltay;
      }
      if( line[c][n].sel & (SELECTED|SELECTED2) )
      {
       xctx->rx2+=xctx->deltax;
       xctx->ry2+=xctx->deltay;
      }
      line[c][n].x1=xctx->rx1;
      line[c][n].y1=xctx->ry1;
      ORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);
      if( line[c][n].x1 == xctx->rx2 &&  line[c][n].y1 == xctx->ry2)
      {
       if(line[c][n].sel == SELECTED1) line[c][n].sel = SELECTED2;
       else if(line[c][n].sel == SELECTED2) line[c][n].sel = SELECTED1;
      }
      line[c][n].x1=xctx->rx1;
      line[c][n].y1=xctx->ry1;
      line[c][n].x2=xctx->rx2;
      line[c][n].y2=xctx->ry2;

      if(xctx->line[c][n].bus){ /* bbox after move */
        int ov, y1, y2;
        ov = INT_BUS_WIDTH(xctx->lw)> cadhalfdotsize ? INT_BUS_WIDTH(xctx->lw) : CADHALFDOTSIZE;
        if(xctx->line[c][n].y1 < xctx->line[c][n].y2)
             { y1 = xctx->line[c][n].y1-ov; y2 = xctx->line[c][n].y2+ov; }
        else { y1 = xctx->line[c][n].y1+ov; y2 = xctx->line[c][n].y2-ov; }
        bbox(ADD, xctx->line[c][n].x1-ov, y1 , xctx->line[c][n].x2+ov , y2 );
      } else {
        int ov, y1, y2;
        ov = cadhalfdotsize;
        if(xctx->line[c][n].y1 < xctx->line[c][n].y2)
              { y1 = xctx->line[c][n].y1-ov; y2 = xctx->line[c][n].y2+ov; }
        else  { y1 = xctx->line[c][n].y1+ov; y2 = xctx->line[c][n].y2-ov; }
        bbox(ADD, xctx->line[c][n].x1-ov, y1 , xctx->line[c][n].x2+ov , y2 );
      }
      break;

     case POLYGON:
      if(c!=k) break;
      {
        xPoly *p = &xctx->poly[c][n];
        double bx1=0., by1=0., bx2=0., by2=0.;
        int j;
        double savex0, savey0;
        savex0 = p->x[0];
        savey0 = p->y[0];
        for(j=0; j<p->points; j++) {
          if(j==0 || p->x[j] < bx1) bx1 = p->x[j];
          if(j==0 || p->y[j] < by1) by1 = p->y[j];
          if(j==0 || p->x[j] > bx2) bx2 = p->x[j];
          if(j==0 || p->y[j] > by2) by2 = p->y[j];

          if( p->sel==SELECTED || p->selected_point[j]) {
            if(xctx->rotatelocal) {
              ROTATION(xctx->move_rot, xctx->move_flip, savex0, savey0, p->x[j], p->y[j],
                       xctx->rx1,xctx->ry1);
            } else {
              ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1, p->x[j], p->y[j],
                       xctx->rx1,xctx->ry1);
            }

            p->x[j] =  xctx->rx1+xctx->deltax;
            p->y[j] =  xctx->ry1+xctx->deltay;
          }

        }
        bbox(ADD, bx1, by1, bx2, by2); /* bbox before move */

        for(j=0; j<p->points; j++) {
          if(j==0 || p->x[j] < bx1) bx1 = p->x[j];
          if(j==0 || p->y[j] < by1) by1 = p->y[j];
          if(j==0 || p->x[j] > bx2) bx2 = p->x[j];
          if(j==0 || p->y[j] > by2) by2 = p->y[j];
        }
        bbox(ADD, bx1, by1, bx2, by2); /* bbox after move */
      }
      break;

     case ARC:
      if(c!=k) break;

      if(xctx->arc[c][n].fill) /* bbox before move */
        arc_bbox(xctx->arc[c][n].x, xctx->arc[c][n].y, xctx->arc[c][n].r, 0, 360,
                 &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
      else
        arc_bbox(xctx->arc[c][n].x, xctx->arc[c][n].y, xctx->arc[c][n].r,
                 xctx->arc[c][n].a, xctx->arc[c][n].b,
                 &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
      bbox(ADD, tmp.x1, tmp.y1, tmp.x2, tmp.y2);

      if(xctx->rotatelocal) {
        /* rotate center wrt itself: do nothing */
        xctx->rx1 = xctx->arc[c][n].x;
        xctx->ry1 = xctx->arc[c][n].y;
      } else {
        ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
           xctx->arc[c][n].x, xctx->arc[c][n].y, xctx->rx1,xctx->ry1);
      }
      angle = xctx->arc[c][n].a;
      if(xctx->move_flip) {
        angle = 270.*xctx->move_rot+180.-xctx->arc[c][n].b-xctx->arc[c][n].a;
      } else {
        angle = xctx->arc[c][n].a+xctx->move_rot*270.;
      }
      angle = fmod(angle, 360.);
      if(angle<0.) angle+=360.;

      if(xctx->arc[c][n].sel == SELECTED) {
        xctx->arc[c][n].x = xctx->rx1+xctx->deltax;
        xctx->arc[c][n].y = xctx->ry1+xctx->deltay;
        xctx->arc[c][n].a = angle;
      } else if(xctx->arc[c][n].sel == SELECTED1) {
        xctx->arc[c][n].x = xctx->rx1;
        xctx->arc[c][n].y = xctx->ry1;
        if(xctx->arc[c][n].r+xctx->deltax) xctx->arc[c][n].r = fabs(xctx->arc[c][n].r+xctx->deltax);
        xctx->arc[c][n].a = angle;
      } else if(xctx->arc[c][n].sel == SELECTED2) {
        angle = my_round(fmod(atan2(-xctx->deltay, xctx->deltax)*180./XSCH_PI+angle, 360.));
        if(angle<0.) angle +=360.;
        xctx->arc[c][n].x = xctx->rx1;
        xctx->arc[c][n].y = xctx->ry1;
        xctx->arc[c][n].a = angle;
      } else if(xctx->arc[c][n].sel==SELECTED3) {
        angle = my_round(fmod(atan2(-xctx->deltay, xctx->deltax)*180./XSCH_PI+xctx->arc[c][n].b, 360.));
        if(angle<0.) angle +=360.;
        if(angle==0) angle=360.;
        xctx->arc[c][n].x = xctx->rx1;
        xctx->arc[c][n].y = xctx->ry1;
        xctx->arc[c][n].b = angle;
      }

      if(xctx->arc[c][n].fill) /* bbox after move */
        arc_bbox(xctx->arc[c][n].x, xctx->arc[c][n].y, xctx->arc[c][n].r, 0, 360,
                 &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
      else
        arc_bbox(xctx->arc[c][n].x, xctx->arc[c][n].y, xctx->arc[c][n].r,
                 xctx->arc[c][n].a, xctx->arc[c][n].b,
                 &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
      bbox(ADD, tmp.x1, tmp.y1, tmp.x2, tmp.y2);

      break;

     case xRECT:
      if(c!=k) break;
      /* bbox before move */
      bbox(ADD, xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rect[c][n].x2, xctx->rect[c][n].y2);
      if(xctx->rotatelocal) {
        ROTATION(xctx->move_rot, xctx->move_flip, xctx->rect[c][n].x1, xctx->rect[c][n].y1,
          xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rx1,xctx->ry1);
        ROTATION(xctx->move_rot, xctx->move_flip, xctx->rect[c][n].x1, xctx->rect[c][n].y1,
          xctx->rect[c][n].x2, xctx->rect[c][n].y2, xctx->rx2,xctx->ry2);
      } else {
        ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
           xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rx1,xctx->ry1);
        ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
           xctx->rect[c][n].x2, xctx->rect[c][n].y2, xctx->rx2,xctx->ry2);
      }

      if( xctx->rect[c][n].sel == SELECTED) {
        xctx->rx1+=xctx->deltax;
        xctx->ry1+=xctx->deltay;
        xctx->rx2+=xctx->deltax;
        xctx->ry2+=xctx->deltay;
      }
      else if( xctx->rect[c][n].sel == SELECTED1) {   /* 20070302 stretching on rectangles */
        xctx->rx1+=xctx->deltax;
        xctx->ry1+=xctx->deltay;
      }
      else if( xctx->rect[c][n].sel == SELECTED2) {
        xctx->rx2+=xctx->deltax;
        xctx->ry1+=xctx->deltay;
      }
      else if( xctx->rect[c][n].sel == SELECTED3) {
        xctx->rx1+=xctx->deltax;
        xctx->ry2+=xctx->deltay;
      }
      else if( xctx->rect[c][n].sel == SELECTED4) {
        xctx->rx2+=xctx->deltax;
        xctx->ry2+=xctx->deltay;
      }
      else if(xctx->rect[c][n].sel==(SELECTED1|SELECTED2))
      {
        xctx->ry1+=xctx->deltay;
      }
      else if(xctx->rect[c][n].sel==(SELECTED3|SELECTED4))
      {
        xctx->ry2+=xctx->deltay;
      }
      else if(xctx->rect[c][n].sel==(SELECTED1|SELECTED3))
      {
        xctx->rx1+=xctx->deltax;
      }
      else if(xctx->rect[c][n].sel==(SELECTED2|SELECTED4))
      {
        xctx->rx2+=xctx->deltax;
      }

      tx1 = xctx->rx1;
      ty1 = xctx->ry1;
      RECTORDER(xctx->rx1,xctx->ry1,xctx->rx2,xctx->ry2);

      if( xctx->rx2 == tx1) {
        if(xctx->rect[c][n].sel==SELECTED1) xctx->rect[c][n].sel = SELECTED2;
        else if(xctx->rect[c][n].sel==SELECTED2) xctx->rect[c][n].sel = SELECTED1;
        else if(xctx->rect[c][n].sel==SELECTED3) xctx->rect[c][n].sel = SELECTED4;
        else if(xctx->rect[c][n].sel==SELECTED4) xctx->rect[c][n].sel = SELECTED3;
      }
      if( xctx->ry2 == ty1) {
        if(xctx->rect[c][n].sel==SELECTED1) xctx->rect[c][n].sel = SELECTED3;
        else if(xctx->rect[c][n].sel==SELECTED3) xctx->rect[c][n].sel = SELECTED1;
        else if(xctx->rect[c][n].sel==SELECTED2) xctx->rect[c][n].sel = SELECTED4;
        else if(xctx->rect[c][n].sel==SELECTED4) xctx->rect[c][n].sel = SELECTED2;
      }

      xctx->rect[c][n].x1 = xctx->rx1;
      xctx->rect[c][n].y1 = xctx->ry1;
      xctx->rect[c][n].x2 = xctx->rx2;
      xctx->rect[c][n].y2 = xctx->ry2;

      /* bbox after move */
      bbox(ADD, xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rect[c][n].x2, xctx->rect[c][n].y2);
      break;

     case xTEXT:
      if(k!=TEXTLAYER) break;
      #if HAS_CAIRO==1  /* bbox before move */
      customfont = set_text_custom_font(&xctx->text[n]);
      #endif
      text_bbox(xctx->text[n].txt_ptr, xctx->text[n].xscale,
         xctx->text[n].yscale, xctx->text[n].rot,xctx->text[n].flip, xctx->text[n].hcenter,
         xctx->text[n].vcenter, xctx->text[n].x0, xctx->text[n].y0,
         &xctx->rx1,&xctx->ry1, &xctx->rx2,&xctx->ry2, &tmpint, &tmpint);
      #if HAS_CAIRO==1
      if(customfont) cairo_restore(xctx->cairo_ctx);
      #endif
      bbox(ADD, xctx->rx1, xctx->ry1, xctx->rx2, xctx->ry2 );
      if(xctx->rotatelocal) {
        ROTATION(xctx->move_rot, xctx->move_flip, xctx->text[n].x0, xctx->text[n].y0,
          xctx->text[n].x0, xctx->text[n].y0, xctx->rx1,xctx->ry1);
      } else {
        ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
           xctx->text[n].x0, xctx->text[n].y0, xctx->rx1,xctx->ry1);
      }
      xctx->text[n].x0=xctx->rx1+xctx->deltax;
      xctx->text[n].y0=xctx->ry1+xctx->deltay;
      xctx->text[n].rot=(xctx->text[n].rot +
       ( (xctx->move_flip && (xctx->text[n].rot & 1) ) ? xctx->move_rot+2 : xctx->move_rot) ) & 0x3;
      xctx->text[n].flip=xctx->move_flip^xctx->text[n].flip;

      #if HAS_CAIRO==1  /* bbox after move */
      customfont = set_text_custom_font(&xctx->text[n]);
      #endif
      text_bbox(xctx->text[n].txt_ptr, xctx->text[n].xscale,
         xctx->text[n].yscale, xctx->text[n].rot,xctx->text[n].flip, xctx->text[n].hcenter,
         xctx->text[n].vcenter, xctx->text[n].x0, xctx->text[n].y0,
         &xctx->rx1,&xctx->ry1, &xctx->rx2,&xctx->ry2, &tmpint, &tmpint);
      #if HAS_CAIRO==1
      if(customfont) cairo_restore(xctx->cairo_ctx);
      #endif
      bbox(ADD, xctx->rx1, xctx->ry1, xctx->rx2, xctx->ry2 );

      break;

     default:
      break;
    } /* end switch(xctx->sel_array[i].type) */
   } /* end for(i=0;i<xctx->lastsel;i++) */
  } /*end for(k=0;k<cadlayers;k++) */

  for(i = 0; i < xctx->lastsel; i++) {
    n = xctx->sel_array[i].n;
    if(xctx->sel_array[i].type == ELEMENT) {
      xctx->prep_hash_inst=0;
      firsti = 0;
      if(xctx->rotatelocal) {
        ROTATION(xctx->move_rot, xctx->move_flip, xctx->inst[n].x0, xctx->inst[n].y0,
           xctx->inst[n].x0, xctx->inst[n].y0, xctx->rx1,xctx->ry1);
      } else {
        ROTATION(xctx->move_rot, xctx->move_flip, xctx->x1, xctx->y_1,
           xctx->inst[n].x0, xctx->inst[n].y0, xctx->rx1,xctx->ry1);
      }
      xctx->inst[n].x0 = xctx->rx1+xctx->deltax;
      xctx->inst[n].y0 = xctx->ry1+xctx->deltay;
      xctx->inst[n].rot = (xctx->inst[n].rot +
       ( (xctx->move_flip && (xctx->inst[n].rot & 1) ) ? xctx->move_rot+2 : xctx->move_rot) ) & 0x3;
      xctx->inst[n].flip = xctx->move_flip ^ xctx->inst[n].flip;
    } 
  }
  if(!firsti || !firstw) {
    xctx->prep_net_structs=0;
    xctx->prep_hi_structs=0;
  }
  /* build after copying and after recalculating prepare_netlist_structs() */
  find_inst_to_be_redrawn(1 + 2 + 4 + 32); /* 32: call prepare_netlist_structs(0) */
  find_inst_to_be_redrawn(16); /* clear data */
  check_collapsing_objects();
  if(tclgetboolvar("autotrim_wires")) trim_wires();
  /* update_conn_cues(1, 1); */

  if(xctx->hilight_nets) {
    propagate_hilights(1, 1, XINSERT_NOREPLACE);
  }

  xctx->ui_state &= ~STARTMOVE;
  if(xctx->ui_state & STARTMERGE) xctx->ui_state |= SELECTION; /* leave selection state so objects can be deleted */
  xctx->ui_state &= ~STARTMERGE;
  xctx->x1=xctx->y_1=xctx->x2=xctx->y_2=xctx->move_rot=xctx->move_flip=xctx->deltax=xctx->deltay=0;
  bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
  draw();
  bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  xctx->rotatelocal=0;
 }
 draw_selection(xctx->gc[SELLAYER], 0);
}
