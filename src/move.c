/* File: move.c
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
static double rx1, rx2, ry1, ry2;
static int move_rot = 0;
static int  move_flip = 0;
static double x1=0.0, y_1=0.0, x2=0.0, y_2=0.0, deltax = 0.0, deltay = 0.0;
/* static int i,c,n,k; */
static int lastsel;
static int rotatelocal=0;


void rebuild_selected_array() /* can be used only if new selected set is lower */
                              /* that is, selectedgroup[] size can not increase */
{
 int i,c;

 dbg(1, "rebuild selected array\n");
 if(!need_rebuild_selected_array) return;
 lastselected=0;
 for(i=0;i<xctx->texts;i++)
  if(xctx->text[i].sel)
  {
   check_selected_storage();
   selectedgroup[lastselected].type = xTEXT;
   selectedgroup[lastselected].n = i;
   selectedgroup[lastselected++].col = TEXTLAYER;
  }
 for(i=0;i<xctx->instances;i++)
  if(xctx->inst[i].sel)
  {
   check_selected_storage();
   selectedgroup[lastselected].type = ELEMENT;
   selectedgroup[lastselected].n = i;
   selectedgroup[lastselected++].col = WIRELAYER;
  }
 for(i=0;i<xctx->wires;i++)
  if(xctx->wire[i].sel)
  {
   check_selected_storage();
   selectedgroup[lastselected].type = WIRE;
   selectedgroup[lastselected].n = i;
   selectedgroup[lastselected++].col = WIRELAYER;
  }
 for(c=0;c<cadlayers;c++)
 {
  for(i=0;i<xctx->arcs[c];i++)
   if(xctx->arc[c][i].sel)
   {
    check_selected_storage();
    selectedgroup[lastselected].type = ARC;
    selectedgroup[lastselected].n = i;
    selectedgroup[lastselected++].col = c;
   }
  for(i=0;i<xctx->rects[c];i++)
   if(xctx->rect[c][i].sel)
   {
    check_selected_storage();
    selectedgroup[lastselected].type = xRECT;
    selectedgroup[lastselected].n = i;
    selectedgroup[lastselected++].col = c;
   }
  for(i=0;i<xctx->lines[c];i++)
   if(xctx->line[c][i].sel)
   {
    check_selected_storage();
    selectedgroup[lastselected].type = LINE;
    selectedgroup[lastselected].n = i;
    selectedgroup[lastselected++].col = c;
   }
  for(i=0;i<xctx->polygons[c];i++)
   if(xctx->poly[c][i].sel)
   {
    check_selected_storage();
    selectedgroup[lastselected].type = POLYGON;
    selectedgroup[lastselected].n = i;
    selectedgroup[lastselected++].col = c;
   }
 }
 need_rebuild_selected_array=0;
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
    need_rebuild_selected_array=1;
    rebuild_selected_array();
  }
}

void update_symbol_bboxes(int rot, int flip)
{
  int i, n, save_flip, save_rot;

  for(i=0;i<lastsel;i++)
  {
    n = selectedgroup[i].n;
    if(selectedgroup[i].type == ELEMENT) {
      save_flip = xctx->inst[n].flip;
      save_rot = xctx->inst[n].rot;
      xctx->inst[n].flip = flip ^ xctx->inst[n].flip;
      xctx->inst[n].rot = (xctx->inst[n].rot + ( flip && (xctx->inst[n].rot & 1) ) ? rot+2 : rot) &0x3;
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
  #ifdef HAS_CAIRO
  int customfont;
  #endif

  if(g == gc[SELLAYER]) lastsel = lastselected;
  for(i=0;i<lastsel;i++)
  {
   c = selectedgroup[i].col;n = selectedgroup[i].n;
   switch(selectedgroup[i].type)
   {
    case xTEXT:
     if(rotatelocal) {
       ROTATION(move_rot, move_flip, xctx->text[n].x0, xctx->text[n].y0, xctx->text[n].x0, xctx->text[n].y0, rx1,ry1);
     } else {
       ROTATION(move_rot, move_flip, x1, y_1, xctx->text[n].x0, xctx->text[n].y0, rx1,ry1);
     }
     #ifdef HAS_CAIRO
     customfont =  set_text_custom_font(&xctx->text[n]);
     #endif
     draw_temp_string(g,ADD, xctx->text[n].txt_ptr,
      (xctx->text[n].rot +
      ( (move_flip && (xctx->text[n].rot & 1) ) ? move_rot+2 : move_rot) ) & 0x3,
       xctx->text[n].flip^move_flip, xctx->text[n].hcenter, xctx->text[n].vcenter,
       rx1+deltax, ry1+deltay,
       xctx->text[n].xscale, xctx->text[n].yscale);
     #ifdef HAS_CAIRO
     if(customfont) cairo_restore(cairo_ctx);
     #endif

     break;
    case xRECT:
     if(rotatelocal) {
       ROTATION(move_rot, move_flip, xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rect[c][n].x1, xctx->rect[c][n].y1, rx1,ry1);
       ROTATION(move_rot, move_flip, xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rect[c][n].x2, xctx->rect[c][n].y2, rx2,ry2);
     } else {
       ROTATION(move_rot, move_flip, x1, y_1, xctx->rect[c][n].x1, xctx->rect[c][n].y1, rx1,ry1);
       ROTATION(move_rot, move_flip, x1, y_1, xctx->rect[c][n].x2, xctx->rect[c][n].y2, rx2,ry2);
     }
     if(xctx->rect[c][n].sel==SELECTED)
     {
       RECTORDER(rx1,ry1,rx2,ry2);
       drawtemprect(g, ADD, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
     }
     else if(xctx->rect[c][n].sel==SELECTED1)
     {
      rx1+=deltax;
      ry1+=deltay;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     else if(xctx->rect[c][n].sel==SELECTED2)
     {
      rx2+=deltax;
      ry1+=deltay;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     else if(xctx->rect[c][n].sel==SELECTED3)
     {
      rx1+=deltax;
      ry2+=deltay;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     else if(xctx->rect[c][n].sel==SELECTED4)
     {
      rx2+=deltax;
      ry2+=deltay;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     else if(xctx->rect[c][n].sel==(SELECTED1|SELECTED2))
     {
      ry1+=deltay;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     else if(xctx->rect[c][n].sel==(SELECTED3|SELECTED4))
     {
      ry2+=deltay;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     else if(xctx->rect[c][n].sel==(SELECTED1|SELECTED3))
     {
      rx1+=deltax;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     else if(xctx->rect[c][n].sel==(SELECTED2|SELECTED4))
     {
      rx2+=deltax;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     break;
    case POLYGON:
     {
      double *x = my_malloc(223, sizeof(double) *xctx->poly[c][n].points);
      double *y = my_malloc(224, sizeof(double) *xctx->poly[c][n].points);
      if(xctx->poly[c][n].sel==SELECTED || xctx->poly[c][n].sel==SELECTED1) {
        for(k=0;k<xctx->poly[c][n].points; k++) {
          if( xctx->poly[c][n].sel==SELECTED || xctx->poly[c][n].selected_point[k]) {
            if(rotatelocal) {
              ROTATION(move_rot, move_flip, xctx->poly[c][n].x[0], xctx->poly[c][n].y[0],
                       xctx->poly[c][n].x[k], xctx->poly[c][n].y[k], rx1,ry1);
            } else {
              ROTATION(move_rot, move_flip, x1, y_1, xctx->poly[c][n].x[k], xctx->poly[c][n].y[k], rx1,ry1);
            }
            x[k] = rx1 + deltax;
            y[k] = ry1 + deltay;
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
     if(rotatelocal) {
       ROTATION(move_rot, move_flip, xctx->wire[n].x1, xctx->wire[n].y1, xctx->wire[n].x1, xctx->wire[n].y1, rx1,ry1);
       ROTATION(move_rot, move_flip, xctx->wire[n].x1, xctx->wire[n].y1, xctx->wire[n].x2, xctx->wire[n].y2, rx2,ry2);
     } else {
       ROTATION(move_rot, move_flip, x1, y_1, xctx->wire[n].x1, xctx->wire[n].y1, rx1,ry1);
       ROTATION(move_rot, move_flip, x1, y_1, xctx->wire[n].x2, xctx->wire[n].y2, rx2,ry2);
     }

     ORDER(rx1,ry1,rx2,ry2);
     if(xctx->wire[n].sel==SELECTED)
     {
      if(xctx->wire[n].bus)
        drawtempline(g, THICK, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
      else
        drawtempline(g, ADD, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
     }
     else if(xctx->wire[n].sel==SELECTED1)
     {
      if(xctx->wire[n].bus)
        drawtempline(g, THICK, rx1+deltax, ry1+deltay, rx2, ry2);
      else
        drawtempline(g, ADD, rx1+deltax, ry1+deltay, rx2, ry2);
     }
     else if(xctx->wire[n].sel==SELECTED2)
     {
      if(xctx->wire[n].bus)
        drawtempline(g, THICK, rx1, ry1, rx2+deltax, ry2+deltay);
      else
        drawtempline(g, ADD, rx1, ry1, rx2+deltax, ry2+deltay);
     }
     break;
    case LINE:
     if(rotatelocal) {
       ROTATION(move_rot, move_flip, xctx->line[c][n].x1, xctx->line[c][n].y1, xctx->line[c][n].x1, xctx->line[c][n].y1, rx1,ry1);
       ROTATION(move_rot, move_flip, xctx->line[c][n].x1, xctx->line[c][n].y1, xctx->line[c][n].x2, xctx->line[c][n].y2, rx2,ry2);
     } else {
       ROTATION(move_rot, move_flip, x1, y_1, xctx->line[c][n].x1, xctx->line[c][n].y1, rx1,ry1);
       ROTATION(move_rot, move_flip, x1, y_1, xctx->line[c][n].x2, xctx->line[c][n].y2, rx2,ry2);
     }
     ORDER(rx1,ry1,rx2,ry2);
     if(xctx->line[c][n].sel==SELECTED)
     {
       if(xctx->line[c][n].bus)
         drawtempline(g, THICK, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
       else
         drawtempline(g, ADD, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
     }
     else if(xctx->line[c][n].sel==SELECTED1)
     {
       if(xctx->line[c][n].bus)
         drawtempline(g, THICK, rx1+deltax, ry1+deltay, rx2, ry2);
       else
         drawtempline(g, ADD, rx1+deltax, ry1+deltay, rx2, ry2);
     }
     else if(xctx->line[c][n].sel==SELECTED2)
     {
       if(xctx->line[c][n].bus)
         drawtempline(g, THICK, rx1, ry1, rx2+deltax, ry2+deltay);
       else
         drawtempline(g, ADD, rx1, ry1, rx2+deltax, ry2+deltay);
     }
     break;
    case ARC:
     if(rotatelocal) {
       /* rotate center wrt itself: do nothing */
       rx1 = xctx->arc[c][n].x;
       ry1 = xctx->arc[c][n].y;
     } else {
       ROTATION(move_rot, move_flip, x1, y_1, xctx->arc[c][n].x, xctx->arc[c][n].y, rx1,ry1);
     }
     angle = xctx->arc[c][n].a;
     if(move_flip) {
       angle = 270.*move_rot+180.-xctx->arc[c][n].b-xctx->arc[c][n].a;
     } else {
       angle = xctx->arc[c][n].a+move_rot*270.;
     }
     angle = fmod(angle, 360.);
     if(angle<0.) angle+=360.;
     if(xctx->arc[c][n].sel==SELECTED) {
       drawtemparc(g, ADD, rx1+deltax, ry1+deltay, xctx->arc[c][n].r, angle, xctx->arc[c][n].b);
     } else if(xctx->arc[c][n].sel==SELECTED1) {
       drawtemparc(g, ADD, rx1, ry1, fabs(xctx->arc[c][n].r+deltax), angle, xctx->arc[c][n].b);
     } else if(xctx->arc[c][n].sel==SELECTED3) {
       angle = ROUND(fmod(atan2(-deltay, deltax)*180./XSCH_PI+xctx->arc[c][n].b, 360.));
       if(angle<0.) angle +=360.;
       if(angle==0) angle=360.;
       drawtemparc(g, ADD, rx1, ry1, xctx->arc[c][n].r, xctx->arc[c][n].a, angle);
     } else if(xctx->arc[c][n].sel==SELECTED2) {
       angle = ROUND(fmod(atan2(-deltay, deltax)*180./XSCH_PI+angle, 360.));
       if(angle<0.) angle +=360.;
       drawtemparc(g, ADD, rx1, ry1, xctx->arc[c][n].r, angle, xctx->arc[c][n].b);
     }
     break;
    case ELEMENT:
     if(rotatelocal) {
       ROTATION(move_rot, move_flip, xctx->inst[n].x0, xctx->inst[n].y0, xctx->inst[n].x0, xctx->inst[n].y0, rx1,ry1);
     } else {
       ROTATION(move_rot, move_flip, x1, y_1, xctx->inst[n].x0, xctx->inst[n].y0, rx1,ry1);
     }
     for(k=0;k<cadlayers;k++)
      draw_temp_symbol(ADD, g, n, k, move_flip,
       ( move_flip && (xctx->inst[n].rot & 1) ) ? move_rot+2 : move_rot,
       rx1-xctx->inst[n].x0+deltax,ry1-xctx->inst[n].y0+deltay);
     break;
   }
#ifdef __unix__
   if(XPending(display) && interruptable)
   {
    drawtemparc(g, END, 0.0, 0.0, 0.0, 0.0, 0.0);
    drawtemprect(g, END, 0.0, 0.0, 0.0, 0.0);
    drawtempline(g, END, 0.0, 0.0, 0.0, 0.0);
    lastsel = i+1;
    return;
   }
#else
   if (interruptable)
   {
     drawtemparc(g, END, 0.0, 0.0, 0.0, 0.0, 0.0);
     drawtemprect(g, END, 0.0, 0.0, 0.0, 0.0);
     drawtempline(g, END, 0.0, 0.0, 0.0, 0.0);
     lastsel = i + 1;
     return;
   }
#endif
  }
  drawtemparc(g, END, 0.0, 0.0, 0.0, 0.0, 0.0);
  drawtemprect(g, END, 0.0, 0.0, 0.0, 0.0);
  drawtempline(g, END, 0.0, 0.0, 0.0, 0.0);
  lastsel = i;
}

static struct int_hashentry *nodetable[HASHSIZE];

void find_inst_hash_clear(void)
{
  free_int_hash(nodetable);
}

void find_inst_to_be_redrawn(const char *node)
{
  int i, p, rects;
  xSymbol * sym;


  if(int_hash_lookup(nodetable, node, 0, XINSERT_NOREPLACE)) return;
  dbg(1, "find_inst_to_be_redrawn(): node=%s\n", node);
  for(i=0; i < xctx->instances; i++) {
    sym = xctx->inst[i].ptr + xctx->sym;
    rects = sym->rects[PINLAYER];
    for(p = 0; p < rects; p++) {
      if(node && xctx->inst[i].node[p] && !strcmp(xctx->inst[i].node[p], node )) {
        symbol_bbox(i, &xctx->inst[i].x1, &xctx->inst[i].y1, &xctx->inst[i].x2, &xctx->inst[i].y2 );
        bbox(ADD, xctx->inst[i].x1, xctx->inst[i].y1, xctx->inst[i].x2, xctx->inst[i].y2 );
      }
    }
  }
  for(i=0;i < xctx->wires; i++) {
    if(node && xctx->wire[i].node && !strcmp(xctx->wire[i].node, node )) {
      if(xctx->wire[i].bus){
        int ov, y1, y2;
        ov = INT_BUS_WIDTH(xctx->lw)> cadhalfdotsize ? INT_BUS_WIDTH(xctx->lw) : CADHALFDOTSIZE;
        if(xctx->wire[i].y1 < xctx->wire[i].y2) { y1 = xctx->wire[i].y1-ov; y2 = xctx->wire[i].y2+ov; }
        else                        { y1 = xctx->wire[i].y1+ov; y2 = xctx->wire[i].y2-ov; }
        bbox(ADD, xctx->wire[i].x1-ov, y1 , xctx->wire[i].x2+ov , y2 );
      } else {
        int ov, y1, y2;
        ov = cadhalfdotsize;
        if(xctx->wire[i].y1 < xctx->wire[i].y2) { y1 = xctx->wire[i].y1-ov; y2 = xctx->wire[i].y2+ov; }
        else                        { y1 = xctx->wire[i].y1+ov; y2 = xctx->wire[i].y2-ov; }
        bbox(ADD, xctx->wire[i].x1-ov, y1 , xctx->wire[i].x2+ov , y2 );
      }
    }
  }
}

void copy_objects(int what)
{
 int c, i, n, k;
 /* xRect tmp; */
 double angle;
 int newpropcnt;
 double tmpx, tmpy;
 int textlayer;
 const char *str;


 #ifdef HAS_CAIRO
 char *textfont;
 /* int customfont; */
 #endif

 if(what & START)
 {
  rotatelocal=0;
  dbg(1, "copy_objects(): START copy\n");
  rebuild_selected_array();
  save_selection(1);
  deltax = deltay = 0.0;
  lastsel = lastselected;
  x1=mousex_snap;y_1=mousey_snap;
   move_flip = 0;move_rot = 0;
  ui_state|=STARTCOPY;
 }
 if(what & ABORT)                               /* draw objects while moving */
 {
  char *str = NULL; /* 20161122 overflow safe */
  draw_selection(gctiled,0);
  move_rot=move_flip=deltax=deltay=0;
  ui_state&=~STARTCOPY;
  my_strdup(225, &str, user_conf_dir);
  my_strcat(226, &str, "/.selection.sch");
  xunlink(str);
  update_symbol_bboxes(move_rot, move_flip);
  my_free(818, &str);
 }
 if(what & RUBBER)                              /* draw objects while moving */
 {
  x2=mousex_snap;y_2=mousey_snap;
  draw_selection(gctiled,0);
  deltax = x2-x1; deltay = y_2 - y_1;
  draw_selection(gc[SELLAYER],1);
 }
 if(what & ROTATELOCAL ) {
  rotatelocal=1;
 }
 if(what & ROTATE) {
  draw_selection(gctiled,0);
  move_rot= (move_rot+1) & 0x3;
  update_symbol_bboxes(move_rot, move_flip);
 }
 if(what & FLIP)
 {
  draw_selection(gctiled,0);
  move_flip = !move_flip;
  update_symbol_bboxes(move_rot, move_flip);
 }
 if(what & END)                                 /* copy selected objects */
 {
  int firstw, firsti;
  int save_draw;
  /* if the copy operation involved move_flip or rotations the original element bboxes were changed. 
     restore them now */
  update_symbol_bboxes(0, 0);
  save_draw = draw_window;
  draw_window=1; /* temporarily re-enable draw to window together with pixmap */
  draw_selection(gctiled,0);
  bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
  newpropcnt=0;
  set_modify(1); push_undo(); /* 20150327 push_undo */
  firstw = firsti = 1;
  for(i=0;i<lastselected;i++)
  {
   n = selectedgroup[i].n;
   if(selectedgroup[i].type == WIRE)
   {
       if(firstw) {
         prepared_hash_wires=0;
         firstw = 0;
       }
       check_wire_storage();
       /*
       if(xctx->wire[n].bus){
         int ov, y1, y2;
         ov = INT_BUS_WIDTH(xctx->lw)> cadhalfdotsize ? INT_BUS_WIDTH(xctx->lw) : CADHALFDOTSIZE;
         if(xctx->wire[n].y1 < xctx->wire[n].y2) { y1 = xctx->wire[n].y1-ov; y2 = xctx->wire[n].y2+ov; }
         else                        { y1 = xctx->wire[n].y1+ov; y2 = xctx->wire[n].y2-ov; }
         bbox(ADD, xctx->wire[n].x1-ov, y1 , xctx->wire[n].x2+ov , y2 );
       } else {
         int ov, y1, y2;
         ov = cadhalfdotsize;
         if(xctx->wire[n].y1 < xctx->wire[n].y2) { y1 = xctx->wire[n].y1-ov; y2 = xctx->wire[n].y2+ov; }
         else                        { y1 = xctx->wire[n].y1+ov; y2 = xctx->wire[n].y2-ov; }
         bbox(ADD, xctx->wire[n].x1-ov, y1 , xctx->wire[n].x2+ov , y2 );
       }
       */
       if(rotatelocal) {
         ROTATION(move_rot, move_flip, xctx->wire[n].x1, xctx->wire[n].y1, xctx->wire[n].x1, xctx->wire[n].y1, rx1,ry1);
         ROTATION(move_rot, move_flip, xctx->wire[n].x1, xctx->wire[n].y1, xctx->wire[n].x2, xctx->wire[n].y2, rx2,ry2);
       } else {
         ROTATION(move_rot, move_flip, x1, y_1, xctx->wire[n].x1, xctx->wire[n].y1, rx1,ry1);
         ROTATION(move_rot, move_flip, x1, y_1, xctx->wire[n].x2, xctx->wire[n].y2, rx2,ry2);
       }
       if( xctx->wire[n].sel & (SELECTED|SELECTED1) )
       {
        rx1+=deltax;
        ry1+=deltay;
       }
       if( xctx->wire[n].sel & (SELECTED|SELECTED2) )
       {
        rx2+=deltax;
        ry2+=deltay;
       }
       tmpx=rx1; /* used as temporary storage */
       tmpy=ry1;
       ORDER(rx1,ry1,rx2,ry2);
       if( tmpx == rx2 &&  tmpy == ry2)
       {
        if(xctx->wire[n].sel == SELECTED1) xctx->wire[n].sel = SELECTED2;
        else if(xctx->wire[n].sel == SELECTED2) xctx->wire[n].sel = SELECTED1;
       }
       selectedgroup[i].n=xctx->wires;
       storeobject(-1, rx1,ry1,rx2,ry2,WIRE,0,xctx->wire[n].sel,xctx->wire[n].prop_ptr);
       xctx->wire[n].sel=0;
       if(xctx->wire[n].bus)
         drawline(WIRELAYER, THICK, rx1,ry1,rx2,ry2, 0);
       else
         drawline(WIRELAYER, ADD, rx1,ry1,rx2,ry2, 0);
   }
   drawline(WIRELAYER, END, 0.0, 0.0, 0.0, 0.0, 0);
  }


  for(k=0;k<cadlayers;k++)
  {
   for(i=0;i<lastselected;i++)
   {
    c = selectedgroup[i].col;n = selectedgroup[i].n;
    switch(selectedgroup[i].type)
    {
     case LINE:
      if(c!=k) break;
      /* bbox(ADD, xctx->line[c][n].x1, xctx->line[c][n].y1, xctx->line[c][n].x2, xctx->line[c][n].y2) */
      if(rotatelocal) {
        ROTATION(move_rot, move_flip, xctx->line[c][n].x1, xctx->line[c][n].y1, xctx->line[c][n].x1, xctx->line[c][n].y1, rx1,ry1);
        ROTATION(move_rot, move_flip, xctx->line[c][n].x1, xctx->line[c][n].y1, xctx->line[c][n].x2, xctx->line[c][n].y2, rx2,ry2);
      } else {
        ROTATION(move_rot, move_flip, x1, y_1, xctx->line[c][n].x1, xctx->line[c][n].y1, rx1,ry1);
        ROTATION(move_rot, move_flip, x1, y_1, xctx->line[c][n].x2, xctx->line[c][n].y2, rx2,ry2);
      }
      if( xctx->line[c][n].sel & (SELECTED|SELECTED1) )
      {
       rx1+=deltax;
       ry1+=deltay;
      }
      if( xctx->line[c][n].sel & (SELECTED|SELECTED2) )
      {
       rx2+=deltax;
       ry2+=deltay;
      }
      tmpx=rx1;
      tmpy=ry1;
      ORDER(rx1,ry1,rx2,ry2);
      if( tmpx == rx2 &&  tmpy == ry2)
      {
       if(xctx->line[c][n].sel == SELECTED1) xctx->line[c][n].sel = SELECTED2;
       else if(xctx->line[c][n].sel == SELECTED2) xctx->line[c][n].sel = SELECTED1;
      }
      if(xctx->line[c][n].bus)
        drawline(k, THICK, rx1,ry1,rx2,ry2, xctx->line[c][n].dash);
      else
        drawline(k, ADD, rx1,ry1,rx2,ry2, xctx->line[c][n].dash);
      selectedgroup[i].n=xctx->lines[c];
      storeobject(-1, rx1, ry1, rx2, ry2, LINE, c, xctx->line[c][n].sel, xctx->line[c][n].prop_ptr);
      xctx->line[c][n].sel=0;
      break;

     case POLYGON:
      if(c!=k) break;
      {
        xPoly *p = &xctx->poly[c][n];
        /* double bx1, by1, bx2, by2; */
        double *x = my_malloc(227, sizeof(double) *p->points);
        double *y = my_malloc(228, sizeof(double) *p->points);
        int j;
        for(j=0; j<p->points; j++) {
          /*
          if(j==0 || p->x[j] < bx1) bx1 = p->x[j];
          if(j==0 || p->y[j] < by1) by1 = p->y[j];
          if(j==0 || p->x[j] > bx2) bx2 = p->x[j];
          if(j==0 || p->y[j] > by2) by2 = p->y[j];
          */
          if( p->sel==SELECTED || p->selected_point[j]) {
            if(rotatelocal) {
              ROTATION(move_rot, move_flip, p->x[0], p->y[0], p->x[j], p->y[j], rx1,ry1);
            } else {
              ROTATION(move_rot, move_flip, x1, y_1, p->x[j], p->y[j], rx1,ry1);
            }
            x[j] = rx1+deltax;
            y[j] = ry1+deltay;
          } else {
            x[j] = p->x[j];
            y[j] = p->y[j];
          }
        }
        /* bbox(ADD, bx1, by1, bx2, by2); */
        drawpolygon(k,  NOW, x, y, p->points, p->fill, p->dash); /* 20180914 added fill */
        selectedgroup[i].n=xctx->polygons[c];
        store_poly(-1, x, y, p->points, c, p->sel, p->prop_ptr);
        p->sel=0;
        my_free(819, &x);
        my_free(820, &y);
      }
      break;
     case ARC:
      if(c!=k) break;
      /*
      arc_bbox(xctx->arc[c][n].x, xctx->arc[c][n].y, xctx->arc[c][n].r, xctx->arc[c][n].a, xctx->arc[c][n].b,
               &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
      bbox(ADD, tmp.x1, tmp.y1, tmp.x2, tmp.y2);
      */
      if(rotatelocal) {
        /* rotate center wrt itself: do nothing */
        rx1 = xctx->arc[c][n].x;
        ry1 = xctx->arc[c][n].y;
      } else {
        ROTATION(move_rot, move_flip, x1, y_1, xctx->arc[c][n].x, xctx->arc[c][n].y, rx1,ry1);
      }
      angle = xctx->arc[c][n].a;
      if(move_flip) {
        angle = 270.*move_rot+180.-xctx->arc[c][n].b-xctx->arc[c][n].a;
      } else {
        angle = xctx->arc[c][n].a+move_rot*270.;
      }
      angle = fmod(angle, 360.);
      if(angle<0.) angle+=360.;



      xctx->arc[c][n].sel=0;
      drawarc(k, ADD, rx1+deltax, ry1+deltay, 
                 xctx->arc[c][n].r, angle, xctx->arc[c][n].b, xctx->arc[c][n].fill, xctx->arc[c][n].dash);
      selectedgroup[i].n=xctx->arcs[c];
      store_arc(-1, rx1+deltax, ry1+deltay,
                 xctx->arc[c][n].r, angle, xctx->arc[c][n].b, c, SELECTED, xctx->arc[c][n].prop_ptr);
      break;

     case xRECT:
      if(c!=k) break;
      /* bbox(ADD, xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rect[c][n].x2, xctx->rect[c][n].y2); */
      if(rotatelocal) {
        ROTATION(move_rot, move_flip, xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rect[c][n].x1, xctx->rect[c][n].y1, rx1,ry1);
        ROTATION(move_rot, move_flip, xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rect[c][n].x2, xctx->rect[c][n].y2, rx2,ry2);
      } else {
        ROTATION(move_rot, move_flip, x1, y_1, xctx->rect[c][n].x1, xctx->rect[c][n].y1, rx1,ry1);
        ROTATION(move_rot, move_flip, x1, y_1, xctx->rect[c][n].x2, xctx->rect[c][n].y2, rx2,ry2);
      }
      RECTORDER(rx1,ry1,rx2,ry2);
      xctx->rect[c][n].sel=0;
      drawrect(k, ADD, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay, xctx->rect[c][n].dash);
      filledrect(k, ADD, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
      selectedgroup[i].n=xctx->rects[c];
      storeobject(-1, rx1+deltax, ry1+deltay,
                 rx2+deltax, ry2+deltay,xRECT, c, SELECTED, xctx->rect[c][n].prop_ptr);
      break;

     case xTEXT:
      if(k!=TEXTLAYER) break;
      check_text_storage();
      /*
      #ifdef HAS_CAIRO
      customfont = set_text_custom_font(&xctx->text[n]);
      #endif
      text_bbox(xctx->text[n].txt_ptr, xctx->text[n].xscale,
        xctx->text[n].yscale, xctx->text[n].rot,xctx->text[n].flip, xctx->text[n].hcenter, xctx->text[n].vcenter,
        xctx->text[n].x0, xctx->text[n].y0,
        &rx1,&ry1, &rx2,&ry2);
      #ifdef HAS_CAIRO
      if(customfont) cairo_restore(cairo_ctx);
      #endif
      bbox(ADD, rx1, ry1, rx2, ry2 );
      */
      if(rotatelocal) {
        ROTATION(move_rot, move_flip, xctx->text[n].x0, xctx->text[n].y0, xctx->text[n].x0, xctx->text[n].y0, rx1,ry1);
      } else {
        ROTATION(move_rot, move_flip, x1, y_1, xctx->text[n].x0, xctx->text[n].y0, rx1,ry1);
      }
      xctx->text[xctx->texts].txt_ptr=NULL;
      my_strdup(229, &xctx->text[xctx->texts].txt_ptr,xctx->text[n].txt_ptr);
      xctx->text[n].sel=0;
       dbg(2, "copy_objects(): current str=%s\n",
        xctx->text[xctx->texts].txt_ptr);
      xctx->text[xctx->texts].x0=rx1+deltax;
      xctx->text[xctx->texts].y0=ry1+deltay;
      xctx->text[xctx->texts].rot=(xctx->text[n].rot +
       ( (move_flip && (xctx->text[n].rot & 1) ) ? move_rot+2 : move_rot) ) & 0x3;
      xctx->text[xctx->texts].flip=move_flip^xctx->text[n].flip;
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

      xctx->text[xctx->texts].xscale=xctx->text[n].xscale;
      xctx->text[xctx->texts].yscale=xctx->text[n].yscale;

      textlayer = xctx->text[xctx->texts].layer;
      if(textlayer < 0 ||  textlayer >= cadlayers) textlayer = TEXTLAYER;
      #ifdef HAS_CAIRO
      textfont = xctx->text[xctx->texts].font;
      if((textfont && textfont[0]) || xctx->text[xctx->texts].flags) {
        char *font = xctx->text[xctx->texts].font;
        int flags = xctx->text[xctx->texts].flags;
        cairo_font_slant_t slant;
        cairo_font_weight_t weight;
        textfont = (font && font[0]) ? font : cairo_font_name;
        weight = ( flags & TEXT_BOLD) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL;
        slant = CAIRO_FONT_SLANT_NORMAL;
        if(flags & TEXT_ITALIC) slant = CAIRO_FONT_SLANT_ITALIC;
        if(flags & TEXT_OBLIQUE) slant = CAIRO_FONT_SLANT_OBLIQUE;
        cairo_save(cairo_ctx);
        cairo_save(cairo_save_ctx);
        cairo_select_font_face (cairo_ctx, textfont, slant, weight);
        cairo_select_font_face (cairo_save_ctx, textfont, slant, weight);
      }
      #endif
      draw_string(textlayer, ADD, xctx->text[xctx->texts].txt_ptr,    /* draw moved txt */
       xctx->text[xctx->texts].rot, xctx->text[xctx->texts].flip,
       xctx->text[xctx->texts].hcenter, xctx->text[xctx->texts].vcenter,
       rx1+deltax,ry1+deltay,
       xctx->text[xctx->texts].xscale, xctx->text[xctx->texts].yscale);
      #ifndef HAS_CAIRO
      drawrect(textlayer, END, 0.0, 0.0, 0.0, 0.0, 0);
      drawline(textlayer, END, 0.0, 0.0, 0.0, 0.0, 0);
      #endif
      #ifdef HAS_CAIRO
      if( (textfont && textfont[0]) || xctx->text[xctx->texts].flags) {
        cairo_restore(cairo_ctx);
        cairo_restore(cairo_save_ctx);
      }
      #endif
      selectedgroup[i].n=xctx->texts;
      xctx->texts++;
       dbg(2, "copy_objects(): done copy string\n");
      break;
     case ELEMENT:
      if(k==0) {
       if(firsti) {
         prepared_hash_instances = 0;
         firsti = 0;
       }
       check_inst_storage();
       if(rotatelocal) {
         ROTATION(move_rot, move_flip, xctx->inst[n].x0, xctx->inst[n].y0, xctx->inst[n].x0, xctx->inst[n].y0, rx1,ry1);
       } else {
         ROTATION(move_rot, move_flip, x1, y_1, xctx->inst[n].x0, xctx->inst[n].y0, rx1,ry1);
       }
       xctx->inst[xctx->instances] = xctx->inst[n];
       xctx->inst[xctx->instances].prop_ptr=NULL;
       xctx->inst[xctx->instances].instname=NULL;
       xctx->inst[xctx->instances].node=NULL;
       xctx->inst[xctx->instances].name=NULL;
       my_strdup(232, &xctx->inst[xctx->instances].name, xctx->inst[n].name);
       my_strdup(233, &xctx->inst[xctx->instances].prop_ptr, xctx->inst[n].prop_ptr);
       my_strdup2(234, &xctx->inst[xctx->instances].instname, get_tok_value(xctx->inst[n].prop_ptr, "name",0));
       xctx->inst[n].sel=0;
       xctx->inst[xctx->instances].flags = xctx->inst[n].flags;
       xctx->inst[xctx->instances].flags &= ~4; /* do not propagate hilight */
       xctx->inst[xctx->instances].x0 = rx1+deltax;
       xctx->inst[xctx->instances].y0 = ry1+deltay;
       xctx->inst[xctx->instances].sel = SELECTED;
       xctx->inst[xctx->instances].rot = (xctx->inst[xctx->instances].rot +
          ( (move_flip && (xctx->inst[xctx->instances].rot & 1) ) ? move_rot+2 : move_rot) ) & 0x3;
       xctx->inst[xctx->instances].flip = (move_flip? !xctx->inst[n].flip:xctx->inst[n].flip);
       /* the newpropcnt argument is zero for the 1st call and used in  */
       /* new_prop_string() for cleaning some internal caches. */
       if(!newpropcnt) hash_all_names(xctx->instances);
       new_prop_string(xctx->instances, xctx->inst[n].prop_ptr,newpropcnt++, dis_uniq_names);
       my_strdup2(235, &xctx->inst[xctx->instances].instname,
                   get_tok_value(xctx->inst[xctx->instances].prop_ptr, "name", 0));
       n=selectedgroup[i].n=xctx->instances;

       xctx->instances++;
      }
      break;
    } /* end switch(selectedgroup[i].type) */
   } /* end for(i=0;i<lastselected;i++) */



   if(k == 0 ) {
     /* force these vars to 0 to trigger a prepare_netlist_structs(0) needed by symbol_bbox->translate
      * to translate @#n:net_name texts */
     need_rebuild_selected_array=1;
     rebuild_selected_array();
     if(!firsti || !firstw) {
       prepared_netlist_structs=0;
       prepared_hilight_structs=0;
     }
     if(show_pin_net_names) {
       prepare_netlist_structs(0);
     }
   }

   for(i = 0; i < lastselected; i++) {
     n = selectedgroup[i].n;
     if(k == 0) {
       if(selectedgroup[i].type == ELEMENT) {
         int p;
         symbol_bbox(n, &xctx->inst[n].x1, &xctx->inst[n].y1, &xctx->inst[n].x2, &xctx->inst[n].y2 );
         bbox(ADD, xctx->inst[n].x1, xctx->inst[n].y1, xctx->inst[n].x2, xctx->inst[n].y2 );
         if(show_pin_net_names) for(p = 0;  p < (xctx->inst[n].ptr + xctx->sym)->rects[PINLAYER]; p++) {
           if( xctx->inst[n].node && xctx->inst[n].node[p]) {
              find_inst_to_be_redrawn(xctx->inst[n].node[p]);
           }
         }
       }
       if(show_pin_net_names && selectedgroup[i].type == WIRE) {
         find_inst_to_be_redrawn(xctx->wire[n].node);
       }
     }
   }
   if(show_pin_net_names) find_inst_hash_clear();

   filledrect(k, END, 0.0, 0.0, 0.0, 0.0);
   drawarc(k, END, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0);
   drawrect(k, END, 0.0, 0.0, 0.0, 0.0, 0);
   drawline(k, END, 0.0, 0.0, 0.0, 0.0, 0);
  } /* end for(k=0;k<cadlayers;k++) */
  check_collapsing_objects();
  update_conn_cues(1, 1);
  ui_state &= ~STARTCOPY;
  x1=y_1=x2=y_2=move_rot=move_flip=deltax=deltay=0;
  bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
  draw();
  bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  rotatelocal=0;
  draw_window = save_draw;
 }
 draw_selection(gc[SELLAYER], 0);
}


/* merge param unused, RFU */
void move_objects(int what, int merge, double dx, double dy)
{
 int c, i, n, k;
 xRect tmp;
 double angle;
 double tx1,ty1; /* temporaries for swapping coordinates 20070302 */
 int textlayer;
 #ifdef HAS_CAIRO
 int customfont;
 #endif

 #ifdef HAS_CAIRO
 char  *textfont;
 #endif
 xInstance * const inst = xctx->inst;
 xLine ** const line = xctx->line;
 xWire * const wire = xctx->wire;

 if(what & START)
 {
  rotatelocal=0;
  deltax = deltay = 0.0;
  rebuild_selected_array();
  lastsel = lastselected;
  if(lastselected==1 && selectedgroup[0].type==ARC &&
          xctx->arc[c=selectedgroup[0].col][n=selectedgroup[0].n].sel!=SELECTED) {
    x1 = xctx->arc[c][n].x;
    y_1 = xctx->arc[c][n].y;
  } else {x1=mousex_snap;y_1=mousey_snap;}
  move_flip = 0;move_rot = 0;
  ui_state|=STARTMOVE;
 }
 if(what & ABORT)                               /* draw objects while moving */
 {
  draw_selection(gctiled,0);
  move_rot=move_flip=deltax=deltay=0;
  ui_state &= ~STARTMOVE;
  ui_state &= ~PLACE_SYMBOL;
  update_symbol_bboxes(0, 0);
 }
 if(what & RUBBER)                              /* abort operation */
 {
  x2=mousex_snap;y_2=mousey_snap;
  draw_selection(gctiled,0);
  deltax = x2-x1; deltay = y_2 - y_1;
  draw_selection(gc[SELLAYER],1);
 }
 if(what & ROTATELOCAL) {
  rotatelocal=1;
 }
 if(what & ROTATE) {
  draw_selection(gctiled,0);
  move_rot= (move_rot+1) & 0x3;
  update_symbol_bboxes(move_rot, move_flip);
 }
 if(what & FLIP)
 {
  draw_selection(gctiled,0);
  move_flip = !move_flip;
  update_symbol_bboxes(move_rot, move_flip);
 }
 if(what & END)                                 /* move selected objects */
 {
  int firsti, firstw;
  int save_draw;
  save_draw = draw_window;
  draw_window=1; /* temporarily re-enable draw to window together with pixmap */
  draw_selection(gctiled,0);
  bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
  set_modify(1);
  if( !(ui_state & (STARTMERGE | PLACE_SYMBOL)) ) {
    dbg(1, "move_objects(): push undo state\n");
    push_undo(); /* 20150327 push_undo */
  }
  ui_state &= ~PLACE_SYMBOL;
  if(dx!=0.0 || dy!=0.0) {
    deltax = dx;
    deltay = dy;
  }

  if(event_reporting) {
    printf("xschem move_objects %g %g\n", deltax, deltay);
    fflush(stdout);
  }

  /* calculate moving symbols bboxes before actually doing the move */
  firsti = firstw = 1;
  for(i=0;i<lastselected;i++)
  {
    n = selectedgroup[i].n;
    if( selectedgroup[i].type == ELEMENT) {
       int p;
       symbol_bbox(n, &inst[n].x1, &inst[n].y1, &inst[n].x2, &inst[n].y2 );
       bbox(ADD, inst[n].x1, inst[n].y1, inst[n].x2, inst[n].y2 );
       if(show_pin_net_names) for(p = 0;  p < (inst[n].ptr + xctx->sym)->rects[PINLAYER]; p++) {
         if( inst[n].node && inst[n].node[p]) {
            find_inst_to_be_redrawn(inst[n].node[p]);
         }
       }
    }
    if(show_pin_net_names && selectedgroup[i].type == WIRE) {
      find_inst_to_be_redrawn(wire[n].node);
    }
  }
  if(show_pin_net_names) find_inst_hash_clear();
  for(k=0;k<cadlayers;k++)
  {
   for(i=0;i<lastselected;i++)
   {
    c = selectedgroup[i].col;n = selectedgroup[i].n;
    switch(selectedgroup[i].type)
    {
     case WIRE:
      if(firstw) {
        prepared_hash_wires=0;
        firstw = 0;
      }
      if(k == 0) {
        if(wire[n].bus){
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
        if(rotatelocal) {
          ROTATION(move_rot, move_flip, wire[n].x1, wire[n].y1, wire[n].x1, wire[n].y1, rx1,ry1);
          ROTATION(move_rot, move_flip, wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2, rx2,ry2);
        } else {
          ROTATION(move_rot, move_flip, x1, y_1, wire[n].x1, wire[n].y1, rx1,ry1);
          ROTATION(move_rot, move_flip, x1, y_1, wire[n].x2, wire[n].y2, rx2,ry2);
        }

        if( wire[n].sel & (SELECTED|SELECTED1) )
        {
         rx1+=deltax;
         ry1+=deltay;
        }
        if( wire[n].sel & (SELECTED|SELECTED2) )
        {
         rx2+=deltax;
         ry2+=deltay;
        }
        wire[n].x1=rx1;
        wire[n].y1=ry1;
        ORDER(rx1,ry1,rx2,ry2);
        if( wire[n].x1 == rx2 &&  wire[n].y1 == ry2)
        {
         if(wire[n].sel == SELECTED1) wire[n].sel = SELECTED2;
         else if(wire[n].sel == SELECTED2) wire[n].sel = SELECTED1;
        }
        wire[n].x1=rx1;
        wire[n].y1=ry1;
        wire[n].x2=rx2;
        wire[n].y2=ry2;
      } else if(k == WIRELAYER) {
        if(wire[n].bus)
          drawline(WIRELAYER, THICK, wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2, 0);
        else
          drawline(WIRELAYER, ADD, wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2, 0);
      }
      break;
     case LINE:
      if(c!=k) break;
      bbox(ADD, line[c][n].x1, line[c][n].y1, line[c][n].x2, line[c][n].y2);
      if(rotatelocal) {
        ROTATION(move_rot, move_flip, line[c][n].x1, line[c][n].y1, line[c][n].x1, line[c][n].y1, rx1,ry1);
        ROTATION(move_rot, move_flip, line[c][n].x1, line[c][n].y1, line[c][n].x2, line[c][n].y2, rx2,ry2);
      } else {
        ROTATION(move_rot, move_flip, x1, y_1, line[c][n].x1, line[c][n].y1, rx1,ry1);
        ROTATION(move_rot, move_flip, x1, y_1, line[c][n].x2, line[c][n].y2, rx2,ry2);
      }

      if( line[c][n].sel & (SELECTED|SELECTED1) )
      {
       rx1+=deltax;
       ry1+=deltay;
      }
      if( line[c][n].sel & (SELECTED|SELECTED2) )
      {
       rx2+=deltax;
       ry2+=deltay;
      }
      line[c][n].x1=rx1;
      line[c][n].y1=ry1;
      ORDER(rx1,ry1,rx2,ry2);
      if( line[c][n].x1 == rx2 &&  line[c][n].y1 == ry2)
      {
       if(line[c][n].sel == SELECTED1) line[c][n].sel = SELECTED2;
       else if(line[c][n].sel == SELECTED2) line[c][n].sel = SELECTED1;
      }
      line[c][n].x1=rx1;
      line[c][n].y1=ry1;
      line[c][n].x2=rx2;
      line[c][n].y2=ry2;
      if(line[c][n].bus)
        drawline(k, THICK, line[c][n].x1, line[c][n].y1, 
                           line[c][n].x2, line[c][n].y2, line[c][n].dash);
      else
        drawline(k, ADD, line[c][n].x1, line[c][n].y1,
                         line[c][n].x2, line[c][n].y2, line[c][n].dash);
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
            if(rotatelocal) {
              ROTATION(move_rot, move_flip, savex0, savey0, p->x[j], p->y[j], rx1,ry1);
            } else {
              ROTATION(move_rot, move_flip, x1, y_1, p->x[j], p->y[j], rx1,ry1);
            }

            p->x[j] =  rx1+deltax;
            p->y[j] =  ry1+deltay;
          }

        }
        bbox(ADD, bx1, by1, bx2, by2);
        drawpolygon(k,  NOW, p->x, p->y, p->points, p->fill, p->dash);
      }
      break;

     case ARC:
      if(c!=k) break;
      if(xctx->arc[c][n].fill)
        arc_bbox(xctx->arc[c][n].x, xctx->arc[c][n].y, xctx->arc[c][n].r, 0, 360,
                 &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
      else
        arc_bbox(xctx->arc[c][n].x, xctx->arc[c][n].y, xctx->arc[c][n].r, xctx->arc[c][n].a, xctx->arc[c][n].b,
                 &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
      dbg(1, "move_objects(): arc_bbox: %g %g %g %g\n", tmp.x1, tmp.y1, tmp.x2, tmp.y2);
      bbox(ADD, tmp.x1, tmp.y1, tmp.x2, tmp.y2);

      if(rotatelocal) {
        /* rotate center wrt itself: do nothing */
        rx1 = xctx->arc[c][n].x;
        ry1 = xctx->arc[c][n].y;
      } else {
        ROTATION(move_rot, move_flip, x1, y_1, xctx->arc[c][n].x, xctx->arc[c][n].y, rx1,ry1);
      }
      angle = xctx->arc[c][n].a;
      if(move_flip) {
        angle = 270.*move_rot+180.-xctx->arc[c][n].b-xctx->arc[c][n].a;
      } else {
        angle = xctx->arc[c][n].a+move_rot*270.;
      }
      angle = fmod(angle, 360.);
      if(angle<0.) angle+=360.;

      if(xctx->arc[c][n].sel == SELECTED) {
        xctx->arc[c][n].x = rx1+deltax;
        xctx->arc[c][n].y = ry1+deltay;
        xctx->arc[c][n].a = angle;
      } else if(xctx->arc[c][n].sel == SELECTED1) {
        xctx->arc[c][n].x = rx1;
        xctx->arc[c][n].y = ry1;
        if(xctx->arc[c][n].r+deltax) xctx->arc[c][n].r = fabs(xctx->arc[c][n].r+deltax);
        xctx->arc[c][n].a = angle;
      } else if(xctx->arc[c][n].sel == SELECTED2) {
        angle = ROUND(fmod(atan2(-deltay, deltax)*180./XSCH_PI+angle, 360.));
        if(angle<0.) angle +=360.;
        xctx->arc[c][n].x = rx1;
        xctx->arc[c][n].y = ry1;
        xctx->arc[c][n].a = angle;
      } else if(xctx->arc[c][n].sel==SELECTED3) {
        angle = ROUND(fmod(atan2(-deltay, deltax)*180./XSCH_PI+xctx->arc[c][n].b, 360.));
        if(angle<0.) angle +=360.;
        if(angle==0) angle=360.;
        xctx->arc[c][n].x = rx1;
        xctx->arc[c][n].y = ry1;
        xctx->arc[c][n].b = angle;
      }
      drawarc(k, ADD, xctx->arc[c][n].x, xctx->arc[c][n].y, xctx->arc[c][n].r,
                      xctx->arc[c][n].a, xctx->arc[c][n].b, xctx->arc[c][n].fill, xctx->arc[c][n].dash);
      break;

     case xRECT:
      if(c!=k) break;
      bbox(ADD, xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rect[c][n].x2, xctx->rect[c][n].y2);
      if(rotatelocal) {
        ROTATION(move_rot, move_flip, xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rect[c][n].x1, xctx->rect[c][n].y1, rx1,ry1);
        ROTATION(move_rot, move_flip, xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rect[c][n].x2, xctx->rect[c][n].y2, rx2,ry2);
      } else {
        ROTATION(move_rot, move_flip, x1, y_1, xctx->rect[c][n].x1, xctx->rect[c][n].y1, rx1,ry1);
        ROTATION(move_rot, move_flip, x1, y_1, xctx->rect[c][n].x2, xctx->rect[c][n].y2, rx2,ry2);
      }

      if( xctx->rect[c][n].sel == SELECTED) {
        rx1+=deltax;
        ry1+=deltay;
        rx2+=deltax;
        ry2+=deltay;
      }
      else if( xctx->rect[c][n].sel == SELECTED1) {   /* 20070302 stretching on rectangles */
        rx1+=deltax;
        ry1+=deltay;
      }
      else if( xctx->rect[c][n].sel == SELECTED2) {
        rx2+=deltax;
        ry1+=deltay;
      }
      else if( xctx->rect[c][n].sel == SELECTED3) {
        rx1+=deltax;
        ry2+=deltay;
      }
      else if( xctx->rect[c][n].sel == SELECTED4) {
        rx2+=deltax;
        ry2+=deltay;
      }
      else if(xctx->rect[c][n].sel==(SELECTED1|SELECTED2))
      {
        ry1+=deltay;
      }
      else if(xctx->rect[c][n].sel==(SELECTED3|SELECTED4))
      {
        ry2+=deltay;
      }
      else if(xctx->rect[c][n].sel==(SELECTED1|SELECTED3))
      {
        rx1+=deltax;
      }
      else if(xctx->rect[c][n].sel==(SELECTED2|SELECTED4))
      {
        rx2+=deltax;
      }

      tx1 = rx1;
      ty1 = ry1;
      RECTORDER(rx1,ry1,rx2,ry2);

      if( rx2 == tx1) {
        if(xctx->rect[c][n].sel==SELECTED1) xctx->rect[c][n].sel = SELECTED2;
        else if(xctx->rect[c][n].sel==SELECTED2) xctx->rect[c][n].sel = SELECTED1;
        else if(xctx->rect[c][n].sel==SELECTED3) xctx->rect[c][n].sel = SELECTED4;
        else if(xctx->rect[c][n].sel==SELECTED4) xctx->rect[c][n].sel = SELECTED3;
      }
      if( ry2 == ty1) {
        if(xctx->rect[c][n].sel==SELECTED1) xctx->rect[c][n].sel = SELECTED3;
        else if(xctx->rect[c][n].sel==SELECTED3) xctx->rect[c][n].sel = SELECTED1;
        else if(xctx->rect[c][n].sel==SELECTED2) xctx->rect[c][n].sel = SELECTED4;
        else if(xctx->rect[c][n].sel==SELECTED4) xctx->rect[c][n].sel = SELECTED2;
      }

      xctx->rect[c][n].x1 = rx1;
      xctx->rect[c][n].y1 = ry1;
      xctx->rect[c][n].x2 = rx2;
      xctx->rect[c][n].y2 = ry2;
      drawrect(k, ADD, xctx->rect[c][n].x1, xctx->rect[c][n].y1,
                       xctx->rect[c][n].x2, xctx->rect[c][n].y2, xctx->rect[c][n].dash);
      filledrect(c, ADD, xctx->rect[c][n].x1, xctx->rect[c][n].y1,
                 xctx->rect[c][n].x2, xctx->rect[c][n].y2);

      break;

     case xTEXT:
      if(k!=TEXTLAYER) break;
      #ifdef HAS_CAIRO
      customfont = set_text_custom_font(&xctx->text[n]);
      #endif
      text_bbox(xctx->text[n].txt_ptr, xctx->text[n].xscale,
         xctx->text[n].yscale, xctx->text[n].rot,xctx->text[n].flip, xctx->text[n].hcenter,
         xctx->text[n].vcenter, xctx->text[n].x0, xctx->text[n].y0, &rx1,&ry1, &rx2,&ry2);
      #ifdef HAS_CAIRO
      if(customfont) cairo_restore(cairo_ctx);
      #endif
      bbox(ADD, rx1, ry1, rx2, ry2 );

      if(rotatelocal) {
        ROTATION(move_rot, move_flip, xctx->text[n].x0, xctx->text[n].y0, xctx->text[n].x0, xctx->text[n].y0, rx1,ry1);
      } else {
        ROTATION(move_rot, move_flip, x1, y_1, xctx->text[n].x0, xctx->text[n].y0, rx1,ry1);
      }

      xctx->text[n].x0=rx1+deltax;
      xctx->text[n].y0=ry1+deltay;
      xctx->text[n].rot=(xctx->text[n].rot +
       ( (move_flip && (xctx->text[n].rot & 1) ) ? move_rot+2 : move_rot) ) & 0x3;
      xctx->text[n].flip=move_flip^xctx->text[n].flip;

      textlayer = xctx->text[n].layer;
      if(textlayer < 0 || textlayer >=  cadlayers) textlayer = TEXTLAYER;
      #ifdef HAS_CAIRO
      textfont = xctx->text[n].font;
      if((textfont && textfont[0]) || xctx->text[n].flags) {
        cairo_font_slant_t slant;
        cairo_font_weight_t weight;
        textfont = (xctx->text[n].font && xctx->text[n].font[0]) ? xctx->text[n].font : cairo_font_name;
        weight = ( xctx->text[n].flags & TEXT_BOLD) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL;
        slant = CAIRO_FONT_SLANT_NORMAL;
        if(xctx->text[n].flags & TEXT_ITALIC) slant = CAIRO_FONT_SLANT_ITALIC;
        if(xctx->text[n].flags & TEXT_OBLIQUE) slant = CAIRO_FONT_SLANT_OBLIQUE;
        cairo_save(cairo_ctx);
        cairo_save(cairo_save_ctx);
        cairo_select_font_face (cairo_ctx, textfont, slant, weight);
        cairo_select_font_face (cairo_save_ctx, textfont, slant, weight);
      }
      #endif
      draw_string(textlayer, ADD, xctx->text[n].txt_ptr,        /* draw moved txt */
       xctx->text[n].rot, xctx->text[n].flip, xctx->text[n].hcenter, xctx->text[n].vcenter,
       xctx->text[n].x0, xctx->text[n].y0,
       xctx->text[n].xscale, xctx->text[n].yscale);
      #ifndef HAS_CAIRO
      drawrect(textlayer, END, 0.0, 0.0, 0.0, 0.0, 0);
      drawline(textlayer, END, 0.0, 0.0, 0.0, 0.0, 0);
      #endif
      #ifdef HAS_CAIRO
      if( (textfont && textfont[0]) || xctx->text[n].flags) {
        cairo_restore(cairo_ctx);
        cairo_restore(cairo_save_ctx);
      }
      #endif
      break;

     case ELEMENT:
      if(k==0) {
       if(firsti) {
         prepared_hash_instances=0;
         firsti = 0;
       }
       if(rotatelocal) {
         ROTATION(move_rot, move_flip, inst[n].x0, inst[n].y0, inst[n].x0, inst[n].y0, rx1,ry1);
       } else {
         ROTATION(move_rot, move_flip, x1, y_1, inst[n].x0, inst[n].y0, rx1,ry1);
       }
       inst[n].x0 = rx1+deltax;
       inst[n].y0 = ry1+deltay;
       inst[n].rot = (inst[n].rot +
        ( (move_flip && (inst[n].rot & 1) ) ? move_rot+2 : move_rot) ) & 0x3;
       inst[n].flip = move_flip ^ inst[n].flip;

      }
      break;
    } /* end switch(selectedgroup[i].type) */
   } /* end for(i=0;i<lastselected;i++) */

   if(k == 0 ) {
     /* force these vars to 0 to trigger a prepare_netlist_structs(0) needed by symbol_bbox->translate
      * to translate @#n:net_name texts */
     if(!firsti || !firstw) {
       prepared_netlist_structs=0;
       prepared_hilight_structs=0;
     }
     if(show_pin_net_names) {
       prepare_netlist_structs(0);
     }
   }
   for(i = 0; i < lastselected; i++) {
     n = selectedgroup[i].n;
     if(k == 0) {
       if(selectedgroup[i].type == ELEMENT) {
         int p;
         symbol_bbox(n, &inst[n].x1, &inst[n].y1, &inst[n].x2, &inst[n].y2 );
         bbox(ADD, inst[n].x1, inst[n].y1, inst[n].x2, inst[n].y2 );
         if(show_pin_net_names) for(p = 0;  p < (inst[n].ptr + xctx->sym)->rects[PINLAYER]; p++) {
           if( inst[n].node && inst[n].node[p]) {
              find_inst_to_be_redrawn(inst[n].node[p]);
           }
         }
       }
       if(show_pin_net_names && selectedgroup[i].type == WIRE) {
         find_inst_to_be_redrawn(wire[n].node);
       }
     }
     /* draw_symbol(ADD,k, n,k, 0, 0, 0.0, 0.0); */
   }
   if(show_pin_net_names) find_inst_hash_clear();

   filledrect(k, END, 0.0, 0.0, 0.0, 0.0);
   drawarc(k, END, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0);
   drawrect(k, END, 0.0, 0.0, 0.0, 0.0, 0);
   drawline(k, END, 0.0, 0.0, 0.0, 0.0, 0);
  } /*end for(k=0;k<cadlayers;k++) */
  check_collapsing_objects();
  update_conn_cues(1, 1);
  ui_state &= ~STARTMOVE;
  ui_state &= ~STARTMERGE;
  x1=y_1=x2=y_2=move_rot=move_flip=deltax=deltay=0;
  bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
  dbg(2, "move_objects(): bbox= %d %d %d %d\n", areax1, areay1, areaw, areah);
  draw();
  bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  rotatelocal=0;
  draw_window =save_draw;
 }
 draw_selection(gc[SELLAYER], 0);
}
