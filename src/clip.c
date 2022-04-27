/* File: clip.c
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


/* Cohen-Sutherland Algorithm for line clipping */
#define UP 8
#define DOWN 4
#define RIGHT 2
#define LEFT 1
static int outcode(double x,double y, double sx1, double sy1, double sx2, double sy2)
{
 register int code=0;
 if(y > sy2) code = UP;
 else if(y < sy1) code = DOWN;
 if(x > sx2) code |= RIGHT;
 else if(x < sx1) code |= LEFT;
 return code;
}

int clip( double *xa,double *ya,double *xb,double *yb)
{
 int outa, outb,outpoint;
 double x,y;
 double sx1, sy1, sx2, sy2;

 sx1 = xctx->xrect[0].x;
 sy1 = xctx->xrect[0].y;
 sx2 = sx1 + xctx->xrect[0].width;
 sy2 = sy1 + xctx->xrect[0].height;
 outa=outcode(*xa, *ya, sx1, sy1, sx2, sy2);
 outb=outcode(*xb, *yb, sx1, sy1, sx2, sy2);
 while(1)
 {
  if(!(outa | outb)) return 1;  /* line is all inside! */
  else if(outa & outb) return 0; /* line is all outside! */
  else
  {
   outpoint=outa? outa:outb;
   if(UP & outpoint)
     {x= *xa + (*xb-*xa) * (sy2 - *ya) / (*yb - *ya); y = sy2;}
   else if(DOWN & outpoint)
     {x= *xa + (*xb-*xa) * (sy1 - *ya) / (*yb - *ya); y = sy1;}
   else if(RIGHT & outpoint)
     {y= *ya + (*yb-*ya) * (sx2 - *xa) / (*xb - *xa); x = sx2;}
   /* else if(LEFT & outpoint) */
   else
     {y= *ya + (*yb-*ya) * (sx1 - *xa) / (*xb - *xa); x = sx1;}
   if(outpoint == outa)
     {*xa=x; *ya=y; outa=outcode(*xa, *ya, sx1, sy1, sx2, sy2);}
   else
     {*xb=x; *yb=y; outb=outcode(*xb, *yb, sx1, sy1, sx2, sy2);}
  }
 }
}

void clip_xy_to_short(double x, double y, short *sx, short *sy)
{
  double ax, ay;
  double r;
  ax = fabs(x);
  ay = fabs(y);

  if(ax > ay) {
    if( x > SHRT_MAX) {
      r = SHRT_MAX / x;
    } else if(x < SHRT_MIN) {
      r = SHRT_MIN / x;
    } else {
      r = 1.0;
    }
  } else {
    if( y > SHRT_MAX) {
      r = SHRT_MAX / y;
    } else if(y < SHRT_MIN) {
      r = SHRT_MIN / y;
    } else {
      r = 1.0;
    }
  }
  *sx = (short) (x * r);
  *sy = (short) (y * r);
}

/* 
 *static short clip_to_short(double n)
 *{
 *  return n > SHRT_MAX ? SHRT_MAX : n < SHRT_MIN ? SHRT_MIN : n;
 *}
 */
int rectclip(int x1,int y1,int x2,int y2,
          double *xa,double *ya,double *xb,double *yb)
/* coordinates should be ordered, x1<x2,ya<yb and so on... */
{
 if(*xa>x2) return 0;
 if(*xb<x1) return 0;
 if(*ya>y2) return 0;
 if(*yb<y1) return 0;
 *xa=CLIP(*xa,x1,x2);
 *xb=CLIP(*xb,x1,x2);
 *ya=CLIP(*ya,y1,y2);
 *yb=CLIP(*yb,y1,y2);
 return 1;
}

double dist_from_rect(double mx, double my, double x1, double y1, double x2, double y2)
{/* return square of dist... */
 double dist, tmp;

 dist=mx-x1;
 tmp=x2-mx;
 if(tmp < dist) dist=tmp;
 tmp=my-y1;
 if(tmp < dist) dist=tmp;
 tmp=y2-my;
 if(tmp < dist) dist=tmp;

 dbg(1, "dist_from_rect(): x1,y1,x2,y2=%.16g,%.16g,%.16g,%.16g\n",x1,y1,x2,y2);
 dbg(1, "dist_from_rect(): mx,my==%.16g,%.16g\n",xctx->mousex,xctx->mousey);
 dbg(1, "dist_from_rect(): dist=%.16g\n",dist);
 return dist*dist;
}

double dist(double x1,double y1,double x2,double y2,double xa,double ya)
/* works if segments are given from left to right, i.e. x1<=x2 */
/* or y1<y2 for vert. lines . */
{/* return square of dist... */
 double distance1,distance2,distance3,denom,a,b,c,ab,xb,yb,tmp;
 double xa1,ya1,xa2,ya2;

 if( (x1==x2) && (y1==y2) )
  return (x1-xa)*(x1-xa)+(y1-ya)*(y1-ya);
 xa1 = xa - x1; xa1*=xa1;
 ya1 = ya - y1; ya1*=ya1;
 xa2 = xa - x2; xa2*=xa2;
 ya2 = ya - y2; ya2*=ya2;
 distance1 = xa1 + ya1;
 distance2 = xa2 + ya2;
 a=(y1-y2);b=(x2-x1);c=-a*x1-b*y1;
 ab = a*b;
 denom = a*a + b*b;
 xb = (b*b*xa - ab*ya - c*a) / denom;
 yb = (a*a*ya - ab*xa - c*b) / denom;
     /* debug ... */
     dbg(1, "dist(): dist1 = %.16g dist2 = %.16g\n",distance1,distance2);
 if(x1<x2)
 {
  if(xb >x1 && xb < x2)
  {
   tmp = a*xa + b*ya + c;
   distance3 = tmp*tmp / denom;
     /* debug ... */
     dbg(1, "dist(); dist3 =  %.16g\n",distance3);
   return distance3;
  }
  else
  {
   return MINOR(distance1,distance2);
  }
 }
 else /* vert. lines */
 {
  if(yb >y1 && yb < y2)
  {
  tmp = a*xa + b*ya + c;
  distance3 = tmp*tmp / denom;
    /* debug ... */
    dbg(1, "dist(): dist3 =  %.16g\n",distance3);
  return distance3;
  }
  else
  {
   return MINOR(distance1,distance2);
  }
 }
    /* debug ... */
    if(debug_var>=0) {fprintf(errfp, "dist(): Internal error, \n");exit(1);}
}

int touch(double x1,double y1,double x2,double y2,double xa,double ya)
/* improved touch check routine, */
/* works if segments are given from left to right, i.e. x1<=x2 */
/* or y1<y2 for vert. lines . */
{
 if( ((x2-x1)*(ya-y1) == (y2-y1)*(xa-x1)) )
 {
  if( x1<x2 && xa>=x1 &&xa<=x2) return 1;
  if(x1==x2 && ya >=y1 && ya<=y2) return 1;
 }
 return 0;
}
