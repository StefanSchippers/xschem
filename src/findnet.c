/* File: findnet.c
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
#include <float.h>
static double distance; /* safe to keep even with multiple schematics */
static Selected sel; /* safe to keep even with multiple schematics */

static void find_closest_net(double mx, double my)
/* returns the net that is closest to the mouse pointer */
/* if there are nets and distance < CADWIREMINDIST */
{
 double tmp;
 int i, w=-1;
 double threshold;
 threshold = CADWIREMINDIST * CADWIREMINDIST * xctx->zoom * xctx->zoom;

 for(i=0;i<xctx->wires; ++i)
 {
  if( (tmp = dist(xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2, mx, my)) < distance )
  {
   w = i; distance = tmp;
  }
 }
 if( distance <= threshold && w!=-1)
 {
  sel.n = w; sel.type = WIRE;
 }
}

static void find_closest_bezier(double mx, double my, int c, int i, int *l, int *col)
{
  const double bez_steps = 1.0/8.0; /* divide the t = [0,1] interval into 8 steps */
  int b;
  double t, t1, tmp;
  double xp, yp, xp1, yp1;
  double x0, x1, x2, y0, y1, y2;
  double *x = xctx->poly[c][i].x;
  double *y = xctx->poly[c][i].y;
  int points = xctx->poly[c][i].points;

  for(b = 0; b < points - 2; b++) {
    if(points == 3) { /* 3 points: only one bezier */
      x0 = x[0];
      y0 = y[0];
      x1 = x[1];
      y1 = y[1];
      x2 = x[2];
      y2 = y[2];
    } else if(b == points - 3) { /* last bezier */
      x0 = (x[points - 3] + x[points - 2]) / 2.0;
      y0 = (y[points - 3] + y[points - 2]) / 2.0;
      x1 =  x[points - 2];
      y1 =  y[points - 2];
      x2 =  x[points - 1];
      y2 =  y[points - 1];
    } else if(b == 0) { /* first bezier */
      x0 =  x[0];
      y0 =  y[0];
      x1 =  x[1];
      y1 =  y[1];
      x2 = (x[1] + x[2]) / 2.0;
      y2 = (y[1] + y[2]) / 2.0;
    } else { /* beziers in the middle */
      x0 = (x[b] + x[b + 1]) / 2.0;
      y0 = (y[b] + y[b + 1]) / 2.0;
      x1 =  x[b + 1];
      y1 =  y[b + 1];
      x2 = (x[b + 1] + x[b + 2]) / 2.0;
      y2 = (y[b + 1] + y[b + 2]) / 2.0;
    }
    for(t = 0; t < 1.0; t += bez_steps) {
      xp = (1 - t) * (1 - t) * x0 + 2 * (1 - t) * t * x1 + t * t * x2;
      yp = (1 - t) * (1 - t) * y0 + 2 * (1 - t) * t * y1 + t * t * y2;
      t1 = t + bez_steps;
      xp1 = (1 - t1) * (1 - t1) * x0 + 2 * (1 - t1) * t1 * x1 + t1 * t1 * x2;
      yp1 = (1 - t1) * (1 - t1) * y0 + 2 * (1 - t1) * t1 * y1 + t1 * t1 * y2;
      ORDER(xp, yp, xp1, yp1);
      if( (tmp = dist(xp, yp, xp1, yp1, mx, my)) < distance )
      {
       *l = i; distance = tmp; *col = c;
       dbg(1, "find_closest_bezier(): distance=%.16g  n=%d\n", distance, i);
      }
    }
  }
}

static void find_closest_polygon(double mx, double my)
/* returns the polygon that is closest to the mouse pointer */
/* if there are lines and distance < CADWIREMINDIST */
{
 double tmp;
 int i, c, j, l=-1, col = 0, bezier;
 double x1, y1, x2, y2;
 double threshold;
 threshold = CADWIREMINDIST * CADWIREMINDIST * xctx->zoom * xctx->zoom;
 for(c=0;c<cadlayers; ++c)
 {
  if(!xctx->enable_layer[c]) continue;
  for(i=0;i<xctx->polygons[c]; ++i)
  {
    xPoly *p = &xctx->poly[c][i];
    bezier = !strboolcmp(get_tok_value(p->prop_ptr, "bezier", 0), "true");
    bezier = bezier && (p->points > 2);
    
    if(bezier) {
      double ds = xctx->cadhalfdotsize;

      find_closest_bezier(mx, my, c, i, &l, &col);
      for(j = 0; j < p->points; j++) {
        if( POINTINSIDE(mx, my, p->x[j] - ds, p->y[j] - ds, p->x[j] + ds, p->y[j] + ds)) {
           l = i; distance = 0.0;col = c;
           break;
        }
      }
    } else {
      for(j=0; j<xctx->poly[c][i].points-1; ++j) {
        x1 = p->x[j];
        y1 = p->y[j];
        x2 = p->x[j+1];
        y2 = p->y[j+1];
        ORDER(x1, y1, x2, y2);
        if( (tmp = dist(x1, y1, x2, y2, mx, my)) < distance )
        {
         l = i; distance = tmp;col = c;
         dbg(1, "find_closest_polygon(): distance=%.16g  n=%d\n", distance, i);
        }
      }
    }
  } /* end for i */
 } /* end for c */
 if( distance <= threshold && l!=-1)
 {
  sel.n = l; sel.type = POLYGON; sel.col = col;
 }
}


static void find_closest_line(double mx, double my)
/* returns the line that is closest to the mouse pointer */
/* if there are lines and distance < CADWIREMINDIST */
{
 double tmp;
 int i, c, l = -1, col = 0;
 double threshold;
 threshold = CADWIREMINDIST * CADWIREMINDIST * xctx->zoom * xctx->zoom;
 for(c=0;c<cadlayers; ++c)
 {
  if(!xctx->enable_layer[c]) continue;
  for(i=0;i<xctx->lines[c]; ++i)
  {
   if( (tmp = dist(xctx->line[c][i].x1, xctx->line[c][i].y1, xctx->line[c][i].x2, xctx->line[c][i].y2, mx, my))
         < distance )
   {
    l = i; distance = tmp;col = c;
    dbg(1, "find_closest_line(): distance=%.16g  n=%d\n", distance, i);
   }
  } /* end for i */
 } /* end for c */
 if( distance <= threshold && l!=-1)
 {
  sel.n = l; sel.type = LINE; sel.col = col;
 }
}


/* sample code: find all inst and wire endpoints around (mx, my) */
#if 0
static void xfind_closest_net_or_symbol_pin(double mx, double my, double *x, double *y)
{
  int sqx, sqy;
  Instpinentry *iptr;
  Wireentry *wptr;
  double xx, yy;

  get_square(mx, my, &sqx, &sqy);
  for(iptr = xctx->instpin_spatial_table[sqx][sqy]; iptr; iptr = iptr->next) {
    int n = iptr->n;
    if( !((xctx->inst[n].ptr + xctx->sym)->type) ) continue;
    xx = iptr->x0;
    yy = iptr->y0;
  }
  for(wptr = xctx->wire_spatial_table[sqx][sqy]; wptr; wptr = wptr->next) {
    int n = wptr->n;
    xWire *w = &xctx->wire[n];
    xx = w->x1;
    yy = w->y1;

    xx = w->x2;
    yy = w->y2; 
  }
}
#endif

/* 20171022 snap wire to closest pin or net endpoint */
void find_closest_net_or_symbol_pin(double mx, double my, double *x, double *y)
{
  int i, j, no_of_pin_rects;
  double x0, x1, x2, y0, y1, y2, xx, yy, dist, min_dist_x = 0, min_dist_y = 0;
  xRect rect;
  short rot, flip;
  char *type = NULL;
  double curr_dist;

  curr_dist = DBL_MAX;
  for(i = 0;i < xctx->instances; ++i) {
    x0=xctx->inst[i].x0;
    y0=xctx->inst[i].y0;
    rot = xctx->inst[i].rot;
    flip = xctx->inst[i].flip;
    my_strdup(_ALLOC_ID_, &type, (xctx->inst[i].ptr+ xctx->sym)->type);
    if(!type) continue;

    no_of_pin_rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];
    if(IS_LABEL_OR_PIN(type)) no_of_pin_rects=1;
    for(j=0; j<no_of_pin_rects; ++j) {
      rect = ((xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER])[j];
      ROTATION(rot, flip, 0.0, 0.0, rect.x1, rect.y1, x1, y1);
      ROTATION(rot, flip, 0.0, 0.0, rect.x2, rect.y2, x2, y2);
      x1 += x0;
      y1 += y0;
      x2 += x0;
      y2 += y0;
      xx = (x1+x2)/2;
      yy = (y1+y2)/2;
      dist = (mx - xx) * (mx - xx) + (my - yy) * (my - yy);
      if(dist < curr_dist) {
        curr_dist = dist;
        min_dist_x = xx;
        min_dist_y = yy;
      }
    }
  }
  for(i = 0;i < xctx->wires; ++i) {
    xx = xctx->wire[i].x1;
    yy = xctx->wire[i].y1;
    dist = (mx - xx) * (mx - xx) + (my - yy) * (my - yy);
    if(dist < curr_dist) {
      curr_dist = dist;
      min_dist_x = xx;
      min_dist_y = yy;
    }
    xx = xctx->wire[i].x2;
    yy = xctx->wire[i].y2;
    dist = (mx - xx) * (mx - xx) + (my - yy) * (my - yy);
    if(dist < curr_dist) {
      curr_dist = dist;
      min_dist_x = xx;
      min_dist_y = yy;
    }
  }
  *x = min_dist_x;
  *y = min_dist_y;
  my_free(_ALLOC_ID_, &type);
}

static void find_closest_arc(double mx, double my)
{
 double dist, angle, angle1, angle2;
 int i, c, r=-1, col;
 int match;
 double threshold;
 threshold = CADWIREMINDIST * CADWIREMINDIST * xctx->zoom * xctx->zoom;

 for(c=0;c<cadlayers; ++c)
 {
  if(!xctx->enable_layer[c]) continue;
  for(i=0;i<xctx->arcs[c]; ++i)
  {
    dist = sqrt(pow(mx-xctx->arc[c][i].x, 2) + pow(my-xctx->arc[c][i].y, 2)) - xctx->arc[c][i].r;
    dist *= dist; /* square distance */
    angle = fmod(atan2(xctx->arc[c][i].y-my, mx-xctx->arc[c][i].x)*180./XSCH_PI, 360.);
    if(angle<0.) angle +=360.;
    angle1 = xctx->arc[c][i].a;
    angle2 = fmod(xctx->arc[c][i].a + xctx->arc[c][i].b, 360.);

    match=0;
    if(dist < distance) {
      if(xctx->arc[c][i].b==360.) match=1;
      if(angle2<angle1) {
        if(angle >= angle1 || angle<= angle2) {
          match=1;
        }
      } else {
        if(angle >= angle1 && angle <= angle2) {
          match =1;
        }
      }
    }
    dbg(1, "find_closest_arc(): dist = %g, angle = %g\n", dist, angle);
    dbg(1, "find_closest_arc(): center=%g, %g: mouse: %g:%g\n",
                             xctx->arc[c][i].x, xctx->arc[c][i].y, mx, my);
    if(match ) {
      dbg(1, "find_closest_arc(): i = %d\n", i);
      r = i;
      distance = dist;
      col = c;
    }
  } /* end for i */
 } /* end for c */
 if( r!=-1 && distance <= threshold ) /*  * pow(xctx->arc[col][r].r, 2)) */
 {
  sel.n = r; sel.type = ARC; sel.col = col;
 }
}


static void find_closest_box(double mx ,double my, int override_lock)
{
 double tmp;
 double ds = xctx->cadhalfdotsize;

 int i, c, r=-1, col = 0;
 for(c=0;c<cadlayers; ++c)
 {
  if(!xctx->enable_layer[c]) continue;
  for(i=0;i<xctx->rects[c]; ++i)
  {
   if( POINTINSIDE(mx, my, xctx->rect[c][i].x1 - ds, xctx->rect[c][i].y1 - ds,
                         xctx->rect[c][i].x2 + ds, xctx->rect[c][i].y2 + ds) )
   {
    tmp=dist_from_rect(mx, my, xctx->rect[c][i].x1, xctx->rect[c][i].y1,
                                  xctx->rect[c][i].x2, xctx->rect[c][i].y2);
    if(tmp < distance)
    {
     r = i; distance = tmp;col = c;
    }
   }
  } /* end for i */
 } /* end for c */
 dbg(1, "find_closest_box(): distance=%.16g\n", distance);
 if( r!=-1 &&  (override_lock || strboolcmp(get_tok_value(xctx->rect[col][r].prop_ptr, "lock", 0), "true"))) {
  sel.n = r; sel.type = xRECT; sel.col = col;
 }
}

static void find_closest_element(double mx, double my, int override_lock)
{
 double tmp;
 int i, r=-1;
 for(i=0;i<xctx->instances; ++i)
 {
  dbg(2, "find_closest_element(): %s: %g %g %g %g\n",
         xctx->inst[i].instname, xctx->inst[i].x1, xctx->inst[i].y1, xctx->inst[i].x2, xctx->inst[i].y2);
  if( POINTINSIDE(mx, my, xctx->inst[i].x1, xctx->inst[i].y1, xctx->inst[i].x2, xctx->inst[i].y2) )
  {
   tmp=pow(mx-(xctx->inst[i].xx1 + xctx->inst[i].xx2)/2, 2)+pow(my-(xctx->inst[i].yy1 + xctx->inst[i].yy2)/2, 2);
   if(tmp*0.1 < distance)
   {
    r = i; distance = tmp*0.1;
   }
    dbg(2, "find_closest_element(): finding closest element, instances=%d, dist=%.16g\n", i, tmp);
  }
 } /* end for i */
 if( r!=-1 &&  (override_lock || strboolcmp(get_tok_value(xctx->inst[r].prop_ptr, "lock", 0), "true")) ) {
   sel.n = r; sel.type = ELEMENT;
 }
}

static void find_closest_text(double mx, double my)
{
 short rot, flip;
 double xx1, xx2, yy1, yy2;
 int i, r=-1, tmp;
 double threshold, dtmp;
 #if HAS_CAIRO==1
 int customfont;
 #endif
 char *estr = NULL;
 threshold = CADWIREMINDIST * CADWIREMINDIST * xctx->zoom * xctx->zoom;
  for(i=0;i<xctx->texts; ++i)
  {
   rot = xctx->text[i].rot;
   flip = xctx->text[i].flip;
   #if HAS_CAIRO==1
   customfont = set_text_custom_font(&xctx->text[i]);
   #endif
   estr = my_expand(get_text_floater(i), tclgetintvar("tabstop"));
   text_bbox(estr,
             xctx->text[i].xscale, xctx->text[i].yscale, rot, flip,
              xctx->text[i].hcenter, xctx->text[i].vcenter,
             xctx->text[i].x0, xctx->text[i].y0,
             &xx1, &yy1, &xx2, &yy2, &tmp, &dtmp);
   my_free(_ALLOC_ID_, &estr);
   #if HAS_CAIRO==1
   if(customfont) {
     cairo_restore(xctx->cairo_ctx);
   }
   #endif
   if(POINTINSIDE(mx, my, xx1, yy1, xx2, yy2))
   {
    r = i; distance = 0;
     dbg(2, "find_closest_text(): finding closest text, texts=%d, dist=%.16g\n", i, distance);
   }
  } /* end for i */
 if( distance <= threshold && r!=-1)
 {
  sel.n = r; sel.type = xTEXT;
 }
}

Selected find_closest_obj(double mx, double my, int override_lock)
{
 sel.n = 0L; sel.col = 0; sel.type = 0;
 distance = DBL_MAX;
 find_closest_line(mx, my);
 find_closest_polygon(mx, my);
 /* dbg(1, "1 find_closest_obj(): sel.n=%d, sel.col=%d, sel.type=%d\n", sel.n, sel.col, sel.type); */
 find_closest_box(mx, my, override_lock);
 find_closest_arc(mx, my);
 /* dbg(1, "2 find_closest_obj(): sel.n=%d, sel.col=%d, sel.type=%d\n", sel.n, sel.col, sel.type); */
 find_closest_text(mx, my);
 find_closest_net(mx, my);
 find_closest_element(mx, my, override_lock);
 return sel;    /*sel.type = 0 if nothing found */
}


