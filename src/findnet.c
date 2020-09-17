/* File: findnet.c
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
#include <float.h>
static double distance;
static Selected sel;

void find_closest_net(double mx,double my)
/* returns the net that is closest to the mouse pointer */
/* if there are nets and distance < CADWIREMINDIST */
{
 double tmp;
 int i,w=-1;
 double threshold = CADWIREMINDIST * CADWIREMINDIST * cadgrid * cadgrid / 400;

 for(i=0;i<lastwire;i++)
 {
  if( (tmp = dist(wire[i].x1,wire[i].y1,wire[i].x2,wire[i].y2,mx,my)) < distance )
  {
   w = i; distance = tmp;
  }
 }
 if( distance <= threshold && w!=-1)
 {
  sel.n = w; sel.type = WIRE;
 }
}

void find_closest_polygon(double mx,double my)
/* returns the polygon that is closest to the mouse pointer */
/* if there are lines and distance < CADWIREMINDIST */
{
 double tmp;
 int i, c, j, l=-1, col = 0;
 double x1, y1, x2, y2;
 double threshold = CADWIREMINDIST * CADWIREMINDIST * cadgrid * cadgrid / 400;
 for(c=0;c<cadlayers;c++)
 {
  for(i=0;i<lastpolygon[c];i++)
  {
    /*fprintf(errfp, "points=%d\n", polygon[c][i].points); */
    for(j=0; j<polygon[c][i].points-1; j++) {
      x1 = polygon[c][i].x[j];
      y1 = polygon[c][i].y[j];
      x2 = polygon[c][i].x[j+1];
      y2 = polygon[c][i].y[j+1];
      ORDER(x1,y1,x2,y2);
      if( (tmp = dist(x1, y1, x2, y2, mx, my)) < distance )
      {
       l = i; distance = tmp;col = c;
       dbg(1, "find_closest_polygon(): distance=%.16g  n=%d\n", distance, i);
      }
    }
  } /* end for i */
 } /* end for c */
 if( distance <= threshold && l!=-1)
 {
  sel.n = l; sel.type = POLYGON; sel.col = col;
 }
}


void find_closest_line(double mx,double my)
/* returns the line that is closest to the mouse pointer */
/* if there are lines and distance < CADWIREMINDIST */
{
 double tmp;
 int i,c,l=-1, col = 0;
 double threshold = CADWIREMINDIST * CADWIREMINDIST * cadgrid * cadgrid / 400;
 for(c=0;c<cadlayers;c++)
 {
  for(i=0;i<lastline[c];i++)
  {
   if( (tmp = dist(line[c][i].x1,line[c][i].y1,line[c][i].x2,line[c][i].y2,mx,my)) 
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

/* 20171022 snap wire to closest pin or net endpoint */
void find_closest_net_or_symbol_pin(double mx,double my, double *x, double *y)
{
  int i, j, no_of_pin_rects;
  double x0, x1, x2, y0, y1, y2, xx, yy, dist, min_dist_x=0, min_dist_y=0;
  Box box;
  int rot, flip;
  char *type=NULL;

  distance = DBL_MAX;
  for(i=0;i<lastinst;i++) {
    x0=inst_ptr[i].x0;
    y0=inst_ptr[i].y0;
    rot = inst_ptr[i].rot;
    flip = inst_ptr[i].flip;
    my_strdup(133, &type,(inst_ptr[i].ptr+instdef)->type);
    if(!type) continue;

    no_of_pin_rects = (inst_ptr[i].ptr+instdef)->rects[PINLAYER];
    if( !strcmp(type, "label") ) no_of_pin_rects=1;
    if( !strcmp(type, "ipin") ) no_of_pin_rects=1;
    if( !strcmp(type, "opin") ) no_of_pin_rects=1;
    if( !strcmp(type, "iopin") ) no_of_pin_rects=1;
    for(j=0; j<no_of_pin_rects; j++) {
      box = ((inst_ptr[i].ptr+instdef)->boxptr[PINLAYER])[j];
      ROTATION(0.0,0.0,box.x1,box.y1,x1,y1);
      ROTATION(0.0,0.0,box.x2,box.y2,x2,y2);
      x1 += x0;
      y1 += y0;
      x2 += x0;
      y2 += y0;
      xx = (x1+x2)/2;
      yy = (y1+y2)/2;
      dist = (mx - xx) * (mx - xx) + (my - yy) * (my - yy);
      if(dist < distance) {
        distance = dist; 
        min_dist_x = xx;
        min_dist_y = yy;
      }
    }
  }
  for(i=0;i<lastwire; i++) {
    xx=wire[i].x1; 
    yy = wire[i].y1;
    dist = (mx - xx) * (mx - xx) + (my - yy) * (my - yy);
    if(dist < distance) {
      distance = dist;
      min_dist_x = xx;
      min_dist_y = yy;
    }
    xx=wire[i].x2; 
    yy = wire[i].y2;
    dist = (mx - xx) * (mx - xx) + (my - yy) * (my - yy);
    if(dist < distance) {
      distance = dist;
      min_dist_x = xx;
      min_dist_y = yy;
    }
  }
  *x = min_dist_x;
  *y = min_dist_y;
  my_free(752, &type);
}

void find_closest_arc(double mx,double my)
{
 double thres = 0.2*cadgrid*cadgrid/400;
 double dist, angle, angle1, angle2;
 int i,c,r=-1, col;
 int match;

 for(c=0;c<cadlayers;c++)
 {
  for(i=0;i<lastarc[c];i++)
  {
    dist = fabs(pow(mx-arc[c][i].x,2) + pow(my-arc[c][i].y,2) - pow(arc[c][i].r,2));
    angle = fmod(atan2(arc[c][i].y-my, mx-arc[c][i].x)*180./XSCH_PI, 360.);
    if(angle<0.) angle +=360.;
    angle1 = arc[c][i].a;
    angle2 = fmod(arc[c][i].a + arc[c][i].b, 360.);

    match=0;
    if(dist < distance) {
      if(arc[c][i].b==360.) match=1;
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
    dbg(1, "find_closest_arc(): center=%g,%g: mouse: %g:%g\n", 
                             arc[c][i].x, arc[c][i].y, mx, my);
    if(match ) {
      dbg(1, "find_closest_arc(): i = %d\n", i);
      r = i; 
      distance = dist;
      col = c;
    }
  } /* end for i */
 } /* end for c */
 if( r!=-1 && distance <= thres* pow(arc[col][r].r,2))
 {
  sel.n = r; sel.type = ARC; sel.col = col;
 }
}


void find_closest_box(double mx,double my)
{
 double tmp;
 int i,c,r=-1, col = 0;
 for(c=0;c<cadlayers;c++)
 {
  for(i=0;i<lastrect[c];i++)
  {
   if( POINTINSIDE(mx,my,rect[c][i].x1,rect[c][i].y1,rect[c][i].x2,rect[c][i].y2) )
   {
    tmp=dist_from_rect(mx,my,rect[c][i].x1,rect[c][i].y1,
                                  rect[c][i].x2,rect[c][i].y2);
    if(tmp < distance)
    {
     r = i; distance = tmp;col = c;
    }
   }
  } /* end for i */
 } /* end for c */
 dbg(1, "find_closest_box(): distance=%.16g\n", distance);
 if( r!=-1)
 {
  sel.n = r; sel.type = xRECT; sel.col = col;
 }
}

void find_closest_element(double mx,double my)
{
 double tmp;
 int i,r=-1;
 for(i=0;i<lastinst;i++)
 {
  dbg(2, "find_closest_element(): %s: %g %g %g %g\n", 
                           inst_ptr[i].instname, inst_ptr[i].x1,inst_ptr[i].y1,inst_ptr[i].x2,inst_ptr[i].y2);
  if( POINTINSIDE(mx,my,inst_ptr[i].x1,inst_ptr[i].y1,inst_ptr[i].x2,inst_ptr[i].y2) )
  {
   /* tmp=pow(mx-inst_ptr[i].x0, 2)+pow(my-inst_ptr[i].y0, 2); */
   tmp=pow(mx-(inst_ptr[i].xx1 + inst_ptr[i].xx2)/2, 2)+pow(my-(inst_ptr[i].yy1 + inst_ptr[i].yy2)/2, 2);
   dbg(0, "i=%d, xx1=%g, yy1=%g, xx2=%g, yy2=%g\n", i, inst_ptr[i].xx1, inst_ptr[i].yy1, inst_ptr[i].xx2, inst_ptr[i].yy2);
   if(tmp*0.1 < distance)
   {
    r = i; distance = tmp*0.1;
   }
    dbg(2, "find_closest_element(): finding closest element, lastinst=%d, dist=%.16g\n",i,tmp);
  }
 } /* end for i */
 if( r!=-1 )
 {
  sel.n = r; sel.type = ELEMENT;
 }
}

void find_closest_text(double mx,double my)
{
 int rot,flip;
 double xx1,xx2,yy1,yy2;
 int i,r=-1;
 double threshold = CADWIREMINDIST * CADWIREMINDIST * cadgrid * cadgrid / 400;
 #ifdef HAS_CAIRO
 int customfont;
 #endif
  for(i=0;i<lasttext;i++)
  {
   rot = textelement[i].rot;
   flip = textelement[i].flip;
   #ifdef HAS_CAIRO
   customfont = set_text_custom_font(&textelement[i]);
   #endif
   text_bbox(textelement[i].txt_ptr, 
             textelement[i].xscale, textelement[i].yscale, rot, flip, textelement[i].hcenter, textelement[i].vcenter,
             textelement[i].x0, textelement[i].y0,
             &xx1,&yy1, &xx2,&yy2);
   #ifdef HAS_CAIRO
   if(customfont) cairo_restore(ctx);
   #endif
   if(POINTINSIDE(mx,my,xx1,yy1, xx2, yy2))
   {
    r = i; distance = 0;
     dbg(2, "find_closest_text(): finding closest text, lasttext=%d, dist=%.16g\n",i,distance);
   }
  } /* end for i */
 if( distance <= threshold && r!=-1)
 {
  sel.n = r; sel.type = xTEXT;
 }
}               

Selected find_closest_obj(double mx,double my)
{
 sel.n = 0L; sel.col = 0; sel.type = 0;
 distance = DBL_MAX;
 find_closest_line(mx,my);
 find_closest_polygon(mx,my);
 /* dbg(1, "1 find_closest_obj(): sel.n=%d, sel.col=%d, sel.type=%d\n", sel.n, sel.col, sel.type); */
 find_closest_box(mx,my);
 find_closest_arc(mx,my);
 /* dbg(1, "2 find_closest_obj(): sel.n=%d, sel.col=%d, sel.type=%d\n", sel.n, sel.col, sel.type); */
 find_closest_text(mx,my);
 find_closest_net(mx,my);
 find_closest_element(mx,my);
 return sel;    /*sel.type = 0 if nothing found */
}


