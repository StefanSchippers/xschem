/* File: psprint.c
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
#define X_TO_PS(x) ( (x+xctx->xorigin)* xctx->mooz )
#define Y_TO_PS(y) ( (y+xctx->yorigin)* xctx->mooz )

static FILE *fd;

typedef struct {
 int red;
 int green;
 int blue;
} Ps_color;

static Ps_color *ps_colors;

static void restore_lw(void)
{
 if(xctx->lw==0.0)
   fprintf(fd, "%.16g setlinewidth\n",0.5);
 else
   if(a3page) fprintf(fd, "%.16g setlinewidth\n",xctx->lw/1.2/sqrt(2));
   else fprintf(fd, "%.16g setlinewidth\n",xctx->lw/1.2);
}

static void set_ps_colors(unsigned int pixel)
{

   if(color_ps) fprintf(fd, "%.16g %.16g %.16g setrgbcolor\n",
    (double)ps_colors[pixel].red/256.0, (double)ps_colors[pixel].green/256.0,
    (double)ps_colors[pixel].blue/256.0);

}

static void ps_xdrawarc(int layer, int fillarc, double x, double y, double r, double a, double b)
{
 if(fill && fillarc)
   fprintf(fd, "%.16g %.16g %.16g %.16g %.16g AF\n", x, y, r, -a, -a-b);
 else
   fprintf(fd, "%.16g %.16g %.16g %.16g %.16g A\n", x, y, r, -a, -a-b);

}

static void ps_xdrawline(int layer, double x1, double y1, double x2,
                  double y2)
{
 fprintf(fd, "%.16g %.16g %.16g %.16g L\n", x2, y2, x1, y1);
}

static void ps_xdrawpoint(int layer, double x1, double y1)
{
 fprintf(fd, "%.16g %.16g %.16g %.16g L\n", x1, y1,x1,y1);
}

static void ps_xfillrectange(int layer, double x1, double y1, double x2,
                  double y2)
{
 /*fprintf(fd, "%.16g %.16g moveto %.16g %.16g lineto %.16g %.16g lineto %.16g %.16g lineto closepath\n", */
 /*       x1,y1,x2,y1,x2,y2,x1,y2); */
 fprintf(fd, "%.16g %.16g %.16g %.16g R\n", x1,y1,x2-x1,y2-y1);
 if( (layer==4 || layer==PINLAYER || layer==WIRELAYER) && fill) {
   fprintf(fd, "%.16g %.16g %.16g %.16g RF\n", x1,y1,x2-x1,y2-y1);
   /* fprintf(fd,"fill\n"); */
 }
 /*fprintf(fd,"stroke\n"); */
}

/* Convex Nonconvex Complex */
#define Polygontype Nonconvex
static void ps_drawpolygon(int c, int what, double *x, double *y, int points, int poly_fill, int dash)
{
  double x1,y1,x2,y2;
  double xx, yy;
  double psdash;
  int i;
  polygon_bbox(x, y, points, &x1,&y1,&x2,&y2);
  x1=X_TO_PS(x1);
  y1=Y_TO_PS(y1);
  x2=X_TO_PS(x2);
  y2=Y_TO_PS(y2);
  if( !rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) ) {
    return;
  }

  psdash = dash / xctx->zoom;
  if(dash) {
    fprintf(fd, "[%g %g] 0 setdash\n", psdash, psdash);
  }
  for(i=0;i<points; i++) {
    xx = X_TO_PS(x[i]);
    yy = Y_TO_PS(y[i]);
    if(i==0) fprintf(fd, "newpath\n%.16g %.16g MT\n", xx, yy);
    else fprintf(fd, "%.16g %.16g LT\n", xx, yy);
  }
  if(fill && fill_type[c] && poly_fill) {
    fprintf(fd, "closepath gsave stroke\n");
    fprintf(fd, "grestore\n");
    fprintf(fd, " fill\n");
  } else {
    fprintf(fd, "stroke\n");
  }


  if(dash) {
    fprintf(fd, "[] 0 setdash\n");
  }
}


static void ps_filledrect(int gc, double rectx1,double recty1,double rectx2,double recty2, int dash)
{
 double x1,y1,x2,y2;
 double psdash;

  x1=X_TO_PS(rectx1);
  y1=Y_TO_PS(recty1);
  x2=X_TO_PS(rectx2);
  y2=Y_TO_PS(recty2);
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
    psdash = dash / xctx->zoom;
    if(dash) {
      fprintf(fd, "[%g %g] 0 setdash\n", psdash, psdash);
    }
    ps_xfillrectange(gc, x1,y1,x2,y2);
    if(dash) {
      fprintf(fd, "[] 0 setdash\n");
    }
  }
}

static void ps_drawarc(int gc, int fillarc, double x,double y,double r,double a, double b, int dash)
{
 double xx,yy,rr;
 double x1, y1, x2, y2;
 double psdash;

  xx=X_TO_PS(x);
  yy=Y_TO_PS(y);
  rr=r*xctx->mooz;
  arc_bbox(x, y, r, a, b, &x1,&y1,&x2,&y2);
  x1=X_TO_PS(x1);
  y1=Y_TO_PS(y1);
  x2=X_TO_PS(x2);
  y2=Y_TO_PS(y2);

  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
    psdash = dash / xctx->zoom;
    if(dash) {
      fprintf(fd, "[%g %g] 0 setdash\n", psdash, psdash);
    }
    ps_xdrawarc(gc, fillarc, xx, yy, rr, a, b);
    if(dash) {
      fprintf(fd, "[] 0 setdash\n");
    }
  }
}


static void ps_drawline(int gc, double linex1,double liney1,double linex2,double liney2, int dash)
{
 double x1,y1,x2,y2;
 double psdash;

  x1=X_TO_PS(linex1);
  y1=Y_TO_PS(liney1);
  x2=X_TO_PS(linex2);
  y2=Y_TO_PS(liney2);
  if( clip(&x1,&y1,&x2,&y2) )
  {
    psdash = dash / xctx->zoom;
    if(dash) {
      fprintf(fd, "[%g %g] 0 setdash\n", psdash, psdash);
    }
    ps_xdrawline(gc, x1, y1, x2, y2);
    if(dash) {
      fprintf(fd, "[] 0 setdash\n");
    }
  }
}

static void ps_draw_string(int gctext,  const char *str,
                 short rot, short flip, int hcenter, int vcenter,
                 double x1,double y1,
                 double xscale, double yscale)

{
 double a,yy,curr_x1,curr_y1,curr_x2,curr_y2,rx1,rx2,ry1,ry2;
 int pos=0,cc,pos2=0;
 int i;

 if(str==NULL) return;
 #ifdef HAS_CAIRO
 text_bbox_nocairo(str, xscale, yscale, rot, flip, hcenter, vcenter, x1,y1, &rx1,&ry1,&rx2,&ry2);
 #else
 text_bbox(str, xscale, yscale, rot, flip, hcenter, vcenter, x1,y1, &rx1,&ry1,&rx2,&ry2);
 #endif
 xscale*=nocairo_font_xscale;
 yscale*=nocairo_font_yscale;

 if(!textclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,rx1,ry1,rx2,ry2)) return;
 set_ps_colors(gctext);
 x1=rx1;y1=ry1;
 if(rot&1) {y1=ry2;rot=3;}
 else rot=0;
 flip = 0; yy=y1;
 while(str[pos2])
 {
  cc = (unsigned char)str[pos2++];
  if(cc>127) cc= '?';
  if(cc=='\n')
  {
   yy+=(FONTHEIGHT+FONTDESCENT+FONTWHITESPACE)*
    yscale;
   pos=0;
   continue;
  }
  a = pos*(FONTWIDTH+FONTWHITESPACE);
  for(i=0;i<character[cc][0]*4;i+=4)
  {
   curr_x1 = ( character[cc][i+1]+ a ) * xscale + x1;
   curr_y1 = ( character[cc][i+2] ) * yscale+yy;
   curr_x2 = ( character[cc][i+3]+ a ) * xscale + x1;
   curr_y2 = ( character[cc][i+4] ) * yscale+yy;
   ROTATION(rot, flip, x1,y1,curr_x1,curr_y1,rx1,ry1);
   ROTATION(rot, flip, x1,y1,curr_x2,curr_y2,rx2,ry2);
   ORDER(rx1,ry1,rx2,ry2);
   ps_drawline(gctext,  rx1, ry1, rx2, ry2, 0);
  }
  pos++;
 }
}


static void ps_drawgrid()
{
 double x,y;
 double delta,tmp;
 if(!draw_grid) return;
 delta=cadgrid* xctx->mooz;
 while(delta<CADGRIDTHRESHOLD) delta*=CADGRIDMULTIPLY;  /* <-- to be improved,but works */
 x = xctx->xorigin* xctx->mooz;y = xctx->yorigin* xctx->mooz;
 set_ps_colors(GRIDLAYER);
 if(y>xctx->areay1 && y<xctx->areay2)
 {
  ps_xdrawline(GRIDLAYER,xctx->areax1+1,(int)y, xctx->areax2-1, (int)y);
 }
 if(x>xctx->areax1 && x<xctx->areax2)
 {
  ps_xdrawline(GRIDLAYER,(int)x,xctx->areay1+1, (int)x, xctx->areay2-1);
 }
 set_ps_colors(GRIDLAYER);
 tmp = floor((xctx->areay1+1)/delta)*delta-fmod(-xctx->yorigin* xctx->mooz,delta);
 for(x=floor((xctx->areax1+1)/delta)*delta-fmod(-xctx->xorigin* xctx->mooz,delta);x<xctx->areax2;x+=delta)
 {
  for(y=tmp;y<xctx->areay2;y+=delta)
  {
   ps_xdrawpoint(GRIDLAYER,(int)(x), (int)(y));
  }
 }
}



static void ps_draw_symbol(int n,int layer, short tmp_flip, short rot, double xoffset, double yoffset)
                            /* draws current layer only, should be called within  */
{                           /* a "for(i=0;i<cadlayers;i++)" loop */
 int j;
 double x0,y0,x1,y1,x2,y2;
 short flip; 
 int textlayer;
 xLine line;
 xRect box;
 xText text;
 xArc arc;
 xPoly polygon;

  if(xctx->inst[n].ptr == -1) return;
  if( (layer != PINLAYER && !enable_layer[layer]) ) return;
  if(layer==0)
  {
   x1=X_TO_PS(xctx->inst[n].x1);
   x2=X_TO_PS(xctx->inst[n].x2);
   y1=Y_TO_PS(xctx->inst[n].y1);
   y2=Y_TO_PS(xctx->inst[n].y2);
   if(OUTSIDE(x1,y1,x2,y2,xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2))
   {
    xctx->inst[n].flags|=1;
    return;
   }
   else xctx->inst[n].flags&=~1;

   /* following code handles different text color for labels/pins 06112002 */

  }
  else if(xctx->inst[n].flags&1)
  {
   dbg(1, "draw_symbol(): skippinginst %d\n", n);
   return;
  }

  flip = xctx->inst[n].flip;
  if(tmp_flip) flip = !flip;
  rot = (xctx->inst[n].rot + rot ) & 0x3;

  x0=xctx->inst[n].x0 + xoffset;
  y0=xctx->inst[n].y0 + yoffset;
   for(j=0;j< (xctx->inst[n].ptr+ xctx->sym)->lines[layer];j++)
   {
    line = ((xctx->inst[n].ptr+ xctx->sym)->line[layer])[j];
    ROTATION(rot, flip, 0.0,0.0,line.x1,line.y1,x1,y1);
    ROTATION(rot, flip, 0.0,0.0,line.x2,line.y2,x2,y2);
    ORDER(x1,y1,x2,y2);
    ps_drawline(layer, x0+x1, y0+y1, x0+x2, y0+y2, line.dash);
   }
   for(j=0;j< (xctx->inst[n].ptr+ xctx->sym)->polygons[layer];j++)
   {
     polygon = ((xctx->inst[n].ptr+ xctx->sym)->poly[layer])[j];
     {   /* scope block so we declare some auxiliary arrays for coord transforms. 20171115 */
       int k;
       double *x = my_malloc(309, sizeof(double) * polygon.points);
       double *y = my_malloc(310, sizeof(double) * polygon.points);
       for(k=0;k<polygon.points;k++) {
         ROTATION(rot, flip, 0.0,0.0,polygon.x[k],polygon.y[k],x[k],y[k]);
         x[k]+= x0;
         y[k] += y0;
       }
       ps_drawpolygon(layer, NOW, x, y, polygon.points, polygon.fill, polygon.dash);
       my_free(876, &x);
       my_free(877, &y);
     }

   }
   for(j=0;j< (xctx->inst[n].ptr+ xctx->sym)->arcs[layer];j++)
   {
     double angle;
     arc = ((xctx->inst[n].ptr+ xctx->sym)->arc[layer])[j];
     if(flip) {
       angle = 270.*rot+180.-arc.b-arc.a;
     } else {
       angle = arc.a+rot*270.;
     }
     angle = fmod(angle, 360.);
     if(angle<0.) angle+=360.;
     ROTATION(rot, flip, 0.0,0.0,arc.x,arc.y,x1,y1);
     ps_drawarc(layer, arc.fill, x0+x1, y0+y1, arc.r, angle, arc.b, arc.dash);
   }
   if( (layer != PINLAYER || enable_layer[layer]) ) for(j=0;j< (xctx->inst[n].ptr+ xctx->sym)->rects[layer];j++)
   {
    box = ((xctx->inst[n].ptr+ xctx->sym)->rect[layer])[j];
    ROTATION(rot, flip, 0.0,0.0,box.x1,box.y1,x1,y1);
    ROTATION(rot, flip, 0.0,0.0,box.x2,box.y2,x2,y2);
    RECTORDER(x1,y1,x2,y2);
    ps_filledrect(layer, x0+x1, y0+y1, x0+x2, y0+y2, box.dash);
   }
   if(  (layer==TEXTWIRELAYER  && !(xctx->inst[n].flags&2) ) ||
        (sym_txt && (layer==TEXTLAYER)   && (xctx->inst[n].flags&2) ) )
   {
    const char *txtptr;
    for(j=0;j< (xctx->inst[n].ptr+ xctx->sym)->texts;j++)
    {
     text = (xctx->inst[n].ptr+ xctx->sym)->text[j];
     /* if(text.xscale*FONTWIDTH* xctx->mooz<1) continue; */
     txtptr= translate(n, text.txt_ptr);
     ROTATION(rot, flip, 0.0,0.0,text.x0,text.y0,x1,y1);
     textlayer = layer;
     if( !(layer == PINLAYER && (xctx->inst[n].flags & 4))) {
       textlayer = (xctx->inst[n].ptr+ xctx->sym)->text[j].layer;
       if(textlayer < 0 || textlayer >= cadlayers) textlayer = layer;
     }
     if((layer == PINLAYER && xctx->inst[n].flags & 4) ||  enable_layer[textlayer]) {
       ps_draw_string(textlayer, txtptr,
         (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
         flip^text.flip, text.hcenter, text.vcenter,
         x0+x1, y0+y1, text.xscale, text.yscale);
     }
    }
    restore_lw();
   }
   Tcl_SetResult(interp,"",TCL_STATIC);

}


static void fill_ps_colors()
{
 char s[200]; /* overflow safe 20161122 */
 unsigned int i,c;
 if(debug_var>=1) {
   tcleval( "puts $ps_colors");
 }
 for(i=0;i<cadlayers;i++) {
   my_snprintf(s, S(s), "lindex $ps_colors %d", i);
   tcleval( s);
   sscanf(tclresult(),"%x", &c);
   ps_colors[i].red   = (c & 0xff0000) >> 16;
   ps_colors[i].green = (c & 0x00ff00) >> 8;
   ps_colors[i].blue  = (c & 0x0000ff);
 }

}


void ps_draw(void)
{
 double dx, dy, delta,scale;
 int c,i, textlayer;
 char *tmp=NULL;
 int old_grid;
 int modified_save;
 const char *r;

 if(!plotfile[0]) {
   my_strdup(59, &tmp, "tk_getSaveFile -title {Select destination file} -initialdir [pwd]");
   tcleval(tmp);
   my_free(878, &tmp);
   r = tclresult();
   if(r[0]) my_strncpy(plotfile, r, S(plotfile));
   else {
     return;
   }
 }
 modified_save=xctx->modified;
 push_undo();
 trim_wires();    /* 20161121 add connection boxes on wires but undo at end */
 ps_colors=my_calloc(311, cadlayers, sizeof(Ps_color));
 if(ps_colors==NULL){
   fprintf(errfp, "ps_draw(): calloc error\n");tcleval( "exit");
 }

 fill_ps_colors();
 old_grid=draw_grid;
 draw_grid=0;

 dx=xctx->areax2-xctx->areax1;
 dy=xctx->areay2-xctx->areay1;
 dbg(1, "ps_draw(): dx=%.16g  dy=%.16g\n", dx, dy);

 fd=fopen("plot.ps", "w");
 fprintf(fd, "%%!\n");


 fprintf(fd, "%%%%BoundingBox: (atend)\n");
 fprintf(fd, "%%%%Title: xschem plot\n");
 fprintf(fd, "%%%%Creator: xschem\n");
 fprintf(fd, "%%%%Pages: 1\n");
 fprintf(fd, "%%%%DocumentFonts: (atend)\n");
 fprintf(fd, "%%%%EndComments\n");
 fprintf(fd, "%%%%BeginProlog\n\n");
 fprintf(fd, "%%%%EndProlog\n");
 fprintf(fd, "%%%%BeginSetup\n");
 if(a3page) {
   fprintf(fd, "%%%%BeginFeature: *PageSize A3\n");
   fprintf(fd, "<< /PageSize [842 1191]  /ImagingBBox null >> setpagedevice\n");
 }
 fprintf(fd, "%%%%EndSetup\n");
 fprintf(fd, "%%%%Page: 1 1\n\n");

 fprintf(fd,"/cm {28.346457 mul} bind def\n");
 fprintf(fd,"/LT {lineto} bind def\n");
 fprintf(fd,"/MT {moveto} bind def\n");
 fprintf(fd,"/L {moveto lineto stroke} bind def\n");
 fprintf(fd,"/A {arcn stroke} bind def\n");
 fprintf(fd,"/AF {arcn fill stroke} bind def\n");
 fprintf(fd,"/R {rectstroke} bind def\n");
 fprintf(fd,"/RF {rectfill} bind def\n");
 if(dx/dy>27.7/19 || dy/dx>27.7/19)
 {
  delta=dx>dy? dx:dy;
  if(a3page) scale=28.7*28.346457/delta*sqrt(2);
  else  scale=27.7*28.346457/delta;
 }
 else
 {
  delta=dx>dy? dy:dx;
  if(a3page) scale=20*28.346457/delta*sqrt(2);
  else scale=19*28.346457/delta;
 }
 if(dx>dy)
 {
  if(a3page) fprintf(fd,"28.99137803 cm 41.29503602 cm translate\n");
  else fprintf(fd,"20 cm 28.7 cm translate\n");
  fprintf(fd,"-90 rotate\n");
  fprintf(fd,"%.16g %.16g scale\n",scale,-scale);
 }
 else
 {
  fprintf(fd,"1 cm 28.7 cm translate\n");
  fprintf(fd,"%.16g %.16g scale\n",scale,-scale);
 }

 fprintf(fd, "1 setlinejoin 1 setlinecap\n");
 restore_lw();
 ps_drawgrid();

 for(i=0;i<xctx->texts;i++)
 {
   textlayer = xctx->text[i].layer;
   if(textlayer < 0 ||  textlayer >= cadlayers) textlayer = TEXTLAYER;
   ps_draw_string(textlayer, xctx->text[i].txt_ptr,
     xctx->text[i].rot, xctx->text[i].flip, xctx->text[i].hcenter, xctx->text[i].vcenter,
     xctx->text[i].x0,xctx->text[i].y0,
     xctx->text[i].xscale, xctx->text[i].yscale);
 }
 restore_lw();

 for(c=0;c<cadlayers;c++)
 {
  set_ps_colors(c);
  for(i=0;i<xctx->lines[c];i++)
   ps_drawline(c, xctx->line[c][i].x1, xctx->line[c][i].y1,
                  xctx->line[c][i].x2, xctx->line[c][i].y2, xctx->line[c][i].dash);
  for(i=0;i<xctx->rects[c];i++)
  {
   ps_filledrect(c, xctx->rect[c][i].x1, xctx->rect[c][i].y1,
                    xctx->rect[c][i].x2, xctx->rect[c][i].y2, xctx->rect[c][i].dash);
  }
  for(i=0;i<xctx->arcs[c];i++)
  {
    ps_drawarc(c, xctx->arc[c][i].fill, xctx->arc[c][i].x, xctx->arc[c][i].y, 
                  xctx->arc[c][i].r, xctx->arc[c][i].a, xctx->arc[c][i].b, xctx->arc[c][i].dash);
  }
  for(i=0;i<xctx->polygons[c];i++) {
    ps_drawpolygon(c, NOW, xctx->poly[c][i].x, xctx->poly[c][i].y, xctx->poly[c][i].points,
       xctx->poly[c][i].fill, xctx->poly[c][i].dash);
  }


  for(i=0;i<xctx->instances;i++)
   ps_draw_symbol(i,c,0,0,0.0,0.0);

 }
 set_ps_colors(WIRELAYER);
 for(i=0;i<xctx->wires;i++)
 {
    ps_drawline(WIRELAYER, xctx->wire[i].x1,xctx->wire[i].y1,xctx->wire[i].x2,xctx->wire[i].y2, 0);
 }

 {
   double x1, y1, x2, y2;
   struct wireentry *wireptr;
   int i;
   update_conn_cues(0, 0);
   /* draw connecting dots */
   x1 = X_TO_XSCHEM(xctx->areax1);
   y1 = Y_TO_XSCHEM(xctx->areay1);
   x2 = X_TO_XSCHEM(xctx->areax2);
   y2 = Y_TO_XSCHEM(xctx->areay2);
   for(init_wire_iterator(x1, y1, x2, y2); ( wireptr = wire_iterator_next() ) ;) {
     i = wireptr->n;
     if( xctx->wire[i].end1 >1 ) { /* 20150331 draw_dots */
       ps_drawarc(WIRELAYER, 1, xctx->wire[i].x1, xctx->wire[i].y1, cadhalfdotsize, 0, 360, 0);
     }
     if( xctx->wire[i].end2 >1 ) { /* 20150331 draw_dots */
       ps_drawarc(WIRELAYER, 1, xctx->wire[i].x2, xctx->wire[i].y2, cadhalfdotsize, 0, 360, 0);
     }
   }
 }






 dbg(1, "ps_draw(): INT_WIDTH(lw)=%d plotfile=%s\n",INT_WIDTH(xctx->lw), plotfile);
 fprintf(fd, "showpage\n\n");
 fprintf(fd, "%%%%EOF\n");
 fclose(fd);
 draw_grid=old_grid;
 my_free(879, &ps_colors);
 if(plotfile[0]) {
   my_strdup(53, &tmp, "convert_to_pdf plot.ps ");
   my_strcat(54, &tmp, plotfile);
 } else {
   my_strdup(312, &tmp, "convert_to_pdf plot.ps plot.pdf");
 }
 my_strncpy(plotfile,"", S(plotfile));
 tcleval( tmp);
 my_free(880, &tmp);
 pop_undo(0);
 xctx->modified=modified_save;
}

