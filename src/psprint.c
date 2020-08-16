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
#define X_TO_PS(x) ( (x+xorigin)* mooz )
#define Y_TO_PS(y) ( (y+yorigin)* mooz )

static FILE *fd; 

typedef struct {
 int red;
 int green;
 int blue;
} Ps_color;

static Ps_color *ps_colors;

static void restore_lw(void) 
{
 if(lw_double==0.0)
   fprintf(fd, "%.16g setlinewidth\n",0.5);
 else
   if(a3page) fprintf(fd, "%.16g setlinewidth\n",lw_double/1.2/sqrt(2));
   else fprintf(fd, "%.16g setlinewidth\n",lw_double/1.2);
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
static void ps_drawpolygon(int c, int what, double *x, double *y, int points, int poly_fill)
{
  double x1,y1,x2,y2;
  double xx, yy;
  int i;
  polygon_bbox(x, y, points, &x1,&y1,&x2,&y2);
  x1=X_TO_PS(x1);
  y1=Y_TO_PS(y1);
  x2=X_TO_PS(x2);
  y2=Y_TO_PS(y2);
  if( !rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) ) {
    return;
  }

  for(i=0;i<points; i++) {
    xx = X_TO_PS(x[i]);
    yy = Y_TO_PS(y[i]);
    if(i==0) fprintf(fd, "%.16g %.16g MT\n", xx, yy);
    else fprintf(fd, "%.16g %.16g LT\n", xx, yy);
  }
  fprintf(fd, "closepath gsave stroke\n");
  fprintf(fd, "grestore\n");
  if(fill && fill_type[c] && poly_fill) {
    fprintf(fd, " fill\n");
  }
}


static void ps_filledrect(int gc, double rectx1,double recty1,double rectx2,double recty2)
{
 double x1,y1,x2,y2;

  x1=X_TO_PS(rectx1);
  y1=Y_TO_PS(recty1);
  x2=X_TO_PS(rectx2);
  y2=Y_TO_PS(recty2);
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   ps_xfillrectange(gc, x1,y1,x2,y2);
  }
}

static void ps_drawarc(int gc, int fillarc, double x,double y,double r,double a, double b)
{
 double xx,yy,rr;
 double x1, y1, x2, y2;

  xx=X_TO_PS(x);
  yy=Y_TO_PS(y);
  rr=r*mooz;
  arc_bbox(x, y, r, a, b, &x1,&y1,&x2,&y2);
  x1=X_TO_PS(x1);
  y1=Y_TO_PS(y1);
  x2=X_TO_PS(x2);
  y2=Y_TO_PS(y2);

  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   ps_xdrawarc(gc, fillarc, xx, yy, rr, a, b);
  }
}


static void ps_drawline(int gc, double linex1,double liney1,double linex2,double liney2)
{
 double x1,y1,x2,y2;

  x1=X_TO_PS(linex1);
  y1=Y_TO_PS(liney1);
  x2=X_TO_PS(linex2);
  y2=Y_TO_PS(liney2);
  if( clip(&x1,&y1,&x2,&y2) )
  {
   ps_xdrawline(gc, x1, y1, x2, y2);
  }
}

static void ps_draw_string(int gctext,  char *str, 
                 int rot, int flip, 
                 double x1,double y1,
                 double xscale, double yscale)  

{
 double a,yy,curr_x1,curr_y1,curr_x2,curr_y2,rx1,rx2,ry1,ry2;
 int pos=0,cc,pos2=0;
 int i; 

 if(str==NULL) return;
 #ifdef HAS_CAIRO
 text_bbox_nocairo(str, xscale, yscale, rot, flip, x1,y1, &rx1,&ry1,&rx2,&ry2);
 #else
 text_bbox(str, xscale, yscale, rot, flip, x1,y1, &rx1,&ry1,&rx2,&ry2);
 #endif
 xscale*=nocairo_font_xscale;
 yscale*=nocairo_font_yscale;

 if(!textclip(areax1,areay1,areax2,areay2,rx1,ry1,rx2,ry2)) return;
 x1=rx1;y1=ry1;
 if(rot&1) {y1=ry2;rot=3;}
 else rot=0;
 flip = 0; yy=y1;
 while(str[pos2])
 {
  cc = (int)str[pos2++];
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
   ROTATION(x1,y1,curr_x1,curr_y1,rx1,ry1);
   ROTATION(x1,y1,curr_x2,curr_y2,rx2,ry2);
   ORDER(rx1,ry1,rx2,ry2);
   ps_drawline(gctext,  rx1, ry1, rx2, ry2);
  }
  pos++;
 }
}


static void ps_drawgrid()
{
 double x,y;
 double delta,tmp;
 if(!draw_grid) return;
 delta=cadgrid* mooz;
 while(delta<CADGRIDTHRESHOLD) delta*=CADGRIDMULTIPLY;  /* <-- to be improved,but works */
 x = xorigin* mooz;y = yorigin* mooz;
 set_ps_colors(GRIDLAYER);
 if(y>areay1 && y<areay2)
 {
  ps_xdrawline(GRIDLAYER,areax1+1,(int)y, areax2-1, (int)y);
 }
 if(x>areax1 && x<areax2)
 {
  ps_xdrawline(GRIDLAYER,(int)x,areay1+1, (int)x, areay2-1);
 }
 set_ps_colors(GRIDLAYER);
 tmp = floor((areay1+1)/delta)*delta-fmod(-yorigin* mooz,delta);
 for(x=floor((areax1+1)/delta)*delta-fmod(-xorigin* mooz,delta);x<areax2;x+=delta)
 {
  for(y=tmp;y<areay2;y+=delta)
  {
   ps_xdrawpoint(GRIDLAYER,(int)(x), (int)(y));
  }
 }
}



static void ps_draw_symbol(int n,int layer,int tmp_flip, int rot, 
        double xoffset, double yoffset) 
                            /* draws current layer only, should be called within  */
{                           /* a "for(i=0;i<cadlayers;i++)" loop */
 int j;
 double x0,y0,x1,y1,x2,y2;
 int flip;
 Line line;
 Box box;
 Text text;
 xArc arc;
 xPolygon polygon;


  if(layer==0)
  {
   x1=X_TO_PS(inst_ptr[n].x1);
   x2=X_TO_PS(inst_ptr[n].x2);
   y1=Y_TO_PS(inst_ptr[n].y1);
   y2=Y_TO_PS(inst_ptr[n].y2);
   if(OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2))
   {
    inst_ptr[n].flags|=1;
    return;
   }
   else inst_ptr[n].flags&=~1;

   /* following code handles different text color for labels/pins 06112002 */

  }
  else if(inst_ptr[n].flags&1)
  {
   dbg(1, "draw_symbol(): skippinginst %d\n", n);
   return;
  }

  flip = inst_ptr[n].flip;
  if(tmp_flip) flip = !flip;
  rot = (inst_ptr[n].rot + rot ) & 0x3;
 
  x0=inst_ptr[n].x0 + xoffset;
  y0=inst_ptr[n].y0 + yoffset;
   for(j=0;j< (inst_ptr[n].ptr+instdef)->lines[layer];j++)
   {
    line = ((inst_ptr[n].ptr+instdef)->lineptr[layer])[j];
    ROTATION(0.0,0.0,line.x1,line.y1,x1,y1);
    ROTATION(0.0,0.0,line.x2,line.y2,x2,y2);
    ORDER(x1,y1,x2,y2);
    ps_drawline(layer, x0+x1, y0+y1, x0+x2, y0+y2);
   }
   for(j=0;j< (inst_ptr[n].ptr+instdef)->polygons[layer];j++) /* 20171115 */
   {
     polygon = ((inst_ptr[n].ptr+instdef)->polygonptr[layer])[j];
     {   /* scope block so we declare some auxiliary arrays for coord transforms. 20171115 */
       int k;
       double *x = my_malloc(309, sizeof(double) * polygon.points);
       double *y = my_malloc(310, sizeof(double) * polygon.points);
       for(k=0;k<polygon.points;k++) {
         ROTATION(0.0,0.0,polygon.x[k],polygon.y[k],x[k],y[k]);
         x[k]+= x0;
         y[k] += y0;
       }
       ps_drawpolygon(layer, NOW, x, y, polygon.points, polygon.fill);
       my_free(876, &x);
       my_free(877, &y);
     }
 
   }
   for(j=0;j< (inst_ptr[n].ptr+instdef)->arcs[layer];j++)
   {
     double angle;
     arc = ((inst_ptr[n].ptr+instdef)->arcptr[layer])[j];
     if(flip) {
       angle = 270.*rot+180.-arc.b-arc.a;
     } else {
       angle = arc.a+rot*270.;
     }
     angle = fmod(angle, 360.);
     if(angle<0.) angle+=360.;
     ROTATION(0.0,0.0,arc.x,arc.y,x1,y1);
     ps_drawarc(layer, arc.fill, x0+x1, y0+y1, arc.r, angle, arc.b);
   }
   for(j=0;j< (inst_ptr[n].ptr+instdef)->rects[layer];j++)
   {
    box = ((inst_ptr[n].ptr+instdef)->boxptr[layer])[j];
    ROTATION(0.0,0.0,box.x1,box.y1,x1,y1);
    ROTATION(0.0,0.0,box.x2,box.y2,x2,y2);
    RECTORDER(x1,y1,x2,y2); 
    ps_filledrect(layer, x0+x1, y0+y1, x0+x2, y0+y2);
   }
   if(  (layer==TEXTWIRELAYER  && !(inst_ptr[n].flags&2) ) || 
        (sym_txt && (layer==TEXTLAYER)   && (inst_ptr[n].flags&2) ) )
   {
    for(j=0;j< (inst_ptr[n].ptr+instdef)->texts;j++)
    {
     text = (inst_ptr[n].ptr+instdef)->txtptr[j];
     /* if(text.xscale*FONTWIDTH* mooz<1) continue; */
     text.txt_ptr= 
       translate(n, text.txt_ptr);
     ROTATION(0.0,0.0,text.x0,text.y0,x1,y1);
     ps_draw_string(layer, text.txt_ptr,
       (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
       flip^text.flip,
       x0+x1, y0+y1, text.xscale, text.yscale);                    
    }
    restore_lw();
   }
   Tcl_SetResult(interp,"",TCL_STATIC);  /* 26102003 */

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
 int c,i; 
 char *tmp=NULL; /* 20161121 */
 int old_grid;
 int modified_save;
 const char *r;

 if(!plotfile[0]) {
   my_strdup(59, &tmp, "tk_getSaveFile -title {Select destination file} -initialdir $env(PWD)");
   tcleval(tmp);
   my_free(878, &tmp);
   r = tclresult();
   if(r[0]) my_strncpy(plotfile, r, S(plotfile));
   else {
     return;
   }
 }
 modified_save=modified;
 push_undo(); /* 20161121 */
 trim_wires();    /* 20161121 add connection boxes on wires but undo at end */
 ps_colors=my_calloc(311, cadlayers, sizeof(Ps_color));
 if(ps_colors==NULL){
   fprintf(errfp, "ps_draw(): calloc error\n");tcleval( "exit");
 } 
 
 fill_ps_colors();
 old_grid=draw_grid;
 draw_grid=0;

 dx=areax2-areax1;
 dy=areay2-areay1;
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

 set_ps_colors(TEXTLAYER);
 for(i=0;i<lasttext;i++) 
 {
   ps_draw_string(TEXTLAYER, textelement[i].txt_ptr,
     textelement[i].rot, textelement[i].flip,
     textelement[i].x0,textelement[i].y0,
     textelement[i].xscale, textelement[i].yscale); 
 }
 restore_lw();

 for(c=0;c<cadlayers;c++)
 {
  set_ps_colors(c);
  for(i=0;i<lastline[c];i++) 
   ps_drawline(c, line[c][i].x1, line[c][i].y1, line[c][i].x2, line[c][i].y2);
  for(i=0;i<lastrect[c];i++) 
  {
   ps_filledrect(c, rect[c][i].x1, rect[c][i].y1, rect[c][i].x2, rect[c][i].y2);
  }
  for(i=0;i<lastarc[c];i++)
  {
    ps_drawarc(c, arc[c][i].fill, arc[c][i].x, arc[c][i].y, arc[c][i].r, arc[c][i].a, arc[c][i].b);
  }
  for(i=0;i<lastpolygon[c];i++) {
    ps_drawpolygon(c, NOW, polygon[c][i].x, polygon[c][i].y, polygon[c][i].points, polygon[c][i].fill);
  }


  for(i=0;i<lastinst;i++)
   ps_draw_symbol(i,c,0,0,0.0,0.0);

 }
 set_ps_colors(WIRELAYER);
 for(i=0;i<lastwire;i++)
 {
    ps_drawline(WIRELAYER, wire[i].x1,wire[i].y1,wire[i].x2,wire[i].y2);
 }

 {
   double x1, y1, x2, y2;
   struct wireentry *wireptr;
   int i;
   update_conn_cues(0, 0);
   /* draw connecting dots */
   x1 = X_TO_XSCHEM(areax1);
   y1 = Y_TO_XSCHEM(areay1);
   x2 = X_TO_XSCHEM(areax2);
   y2 = Y_TO_XSCHEM(areay2);
   for(init_wire_iterator(x1, y1, x2, y2); ( wireptr = wire_iterator_next() ) ;) {
     i = wireptr->n;
     if( wire[i].end1 >1 ) { /* 20150331 draw_dots */
       ps_drawarc(WIRELAYER, 1, wire[i].x1, wire[i].y1, cadhalfdotsize, 0, 360);
     }
     if( wire[i].end2 >1 ) { /* 20150331 draw_dots */
       ps_drawarc(WIRELAYER, 1, wire[i].x2, wire[i].y2, cadhalfdotsize, 0, 360);
     }
   }
 }

  




 dbg(1, "ps_draw(): lw=%d plotfile=%s\n",lw, plotfile);
 fprintf(fd, "showpage\n\n");
 fprintf(fd, "%%%%EOF\n");
 fclose(fd);
 draw_grid=old_grid;
 my_free(879, &ps_colors);
 if(plotfile[0]) {
   my_strdup(53, &tmp, "convert_to_pdf plot.ps "); /* 20161121 */
   my_strcat(54, &tmp, plotfile);
 } else {
   my_strdup(312, &tmp, "convert_to_pdf plot.ps plot.pdf"); /* 20161121 */
 }
 my_strncpy(plotfile,"", S(plotfile));
 tcleval( tmp);
 my_free(880, &tmp);
 pop_undo(0); /* 20161121 */
 modified=modified_save;  /* 20161121 */
}

