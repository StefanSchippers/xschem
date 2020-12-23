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
static char ps_font_name[80] = "Helvetica"; /* Courier Times Helvetica */
static char ps_font_family[80] = "Helvetica"; /* Courier Times Helvetica */


static void set_lw(void)
{
 if(xctx->lw==0.0)
   fprintf(fd, "%g setlinewidth\n",0.5);
 else
   fprintf(fd, "%g setlinewidth\n",xctx->lw/1.2);
}

static void set_ps_colors(unsigned int pixel)
{

   if(color_ps) fprintf(fd, "%g %g %g RGB\n",
    (double)ps_colors[pixel].red/256.0, (double)ps_colors[pixel].green/256.0,
    (double)ps_colors[pixel].blue/256.0);

}

static void ps_xdrawarc(int layer, int fillarc, double x, double y, double r, double a, double b)
{
 if(fill && fillarc)
   fprintf(fd, "%g %g %g %g %g A %g %g LT C F S\n", x, y, r, -a, -a-b, x, y);
 else
   fprintf(fd, "%g %g %g %g %g A S\n", x, y, r, -a, -a-b);

}

static void ps_xdrawline(int layer, double x1, double y1, double x2,
                  double y2)
{
 fprintf(fd, "%.6g %.6g %.6g %.6g L\n", x2, y2, x1, y1);
}

static void ps_xdrawpoint(int layer, double x1, double y1)
{
 fprintf(fd, "%g %g %g %g L\n", x1, y1,x1,y1);
}

static void ps_xfillrectange(int layer, double x1, double y1, double x2,
                  double y2)
{
 fprintf(fd, "%g %g %g %g R\n", x1,y1,x2-x1,y2-y1);
 if( (fill_type[layer] == 1) && fill) {
   fprintf(fd, "%g %g %g %g RF\n", x1,y1,x2-x1,y2-y1);
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
    if(i==0) fprintf(fd, "NP\n%g %g MT\n", xx, yy);
    else fprintf(fd, "%g %g LT\n", xx, yy);
  }
  if(fill && fill_type[c] && poly_fill) {
    fprintf(fd, "C F S\n");
  } else {
    fprintf(fd, "S\n");
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



static void ps_draw_string_line(int layer, char *s, double x, double y, double size, short rot, short flip,
    int lineno, double fontheight, double fontascent, double fontdescent, int llength)
{
  double ix, iy;
  short rot1;
  int line_delta;
  double lines;
  set_ps_colors(layer);
  if(s==NULL) return;
  if(llength==0) return;

  line_delta = lineno*fontheight;
  lines = (cairo_lines-1)*fontheight;

  ix=X_TO_PS(x);
  iy=Y_TO_PS(y);
  if(rot&1) {
    rot1=3;
  } else rot1=0;

  if(     rot==0 && flip==0) {iy+=line_delta+fontascent;}
  else if(rot==1 && flip==0) {ix=ix-fontheight+fontascent-lines+line_delta;}
  else if(rot==2 && flip==0) {iy=iy-fontheight-lines+line_delta+fontascent;}
  else if(rot==3 && flip==0) {ix+=line_delta+fontascent;}
  else if(rot==0 && flip==1) {iy+=line_delta+fontascent;}
  else if(rot==1 && flip==1) {ix=ix-fontheight+line_delta-lines+fontascent;}
  else if(rot==2 && flip==1) {iy=iy-fontheight-lines+line_delta+fontascent;}
  else if(rot==3 && flip==1) {ix+=line_delta+fontascent;}

  fprintf(fd, "GS\n");
  fprintf(fd, "/%s FF\n", ps_font_family);
  fprintf(fd, "%g SCF\n", size * xctx->mooz);
  fprintf(fd, "SF\n");
  fprintf(fd, "NP\n");
  fprintf(fd, "%g %g MT\n", ix, iy);
  if(rot1) fprintf(fd, "%d rotate\n", rot1*90);
  fprintf(fd, "1 -1 scale\n");
  fprintf(fd, "(");
  while(*s) {
    switch(*s) {
      case '(':
        fputs("\\(", fd);
        break;
      case ')':
        fputs("\\)", fd);
        break;
      default:
       fputc(*s, fd);
    }
    s++;
  }
  fprintf(fd, ")\n");
  if     (rot==1 && flip==0) {fprintf(fd, "dup SW pop neg 0 RMT\n");}
  else if(rot==2 && flip==0) {fprintf(fd, "dup SW pop neg 0 RMT\n");}
  else if(rot==0 && flip==1) {fprintf(fd, "dup SW pop neg 0 RMT\n");}
  else if(rot==3 && flip==1) {fprintf(fd, "dup SW pop neg 0 RMT\n");}

  fprintf(fd, "show\n");
  fprintf(fd, "GR\n");
}



static void ps_draw_string(int layer, const char *str, short rot, short flip, int hcenter, int vcenter,
                 double x,double y, double xscale, double yscale)
{
  char *tt, *ss, *sss=NULL;
  double textx1,textx2,texty1,texty2;
  char c;
  int lineno=0;
  double size, height, ascent, descent;
  int llength=0;
  if(str==NULL || !has_x ) return;
  size = xscale*53.;
  height =  size*xctx->mooz * 1.147; /* was 1.147 */
  ascent =  size*xctx->mooz * 0.808; /* was 0.908 */
  descent = size*xctx->mooz * 0.219; /* was 0.219 */

  text_bbox(str, xscale, yscale, rot, flip, hcenter, vcenter, x,y, &textx1,&texty1,&textx2,&texty2);
  /* fprintf(fd, "%% text bbox: %g %g\n", xctx->mooz * (textx2 - textx1), xctx->mooz * (texty2 - texty1)); */
  if(!textclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,textx1,texty1,textx2,texty2)) {
    return;
  }
  if(hcenter) {
    if(rot == 0 && flip == 0 ) { x=textx1;}
    if(rot == 1 && flip == 0 ) { y=texty1;}
    if(rot == 2 && flip == 0 ) { x=textx2;}
    if(rot == 3 && flip == 0 ) { y=texty2;}
    if(rot == 0 && flip == 1 ) { x=textx2;}
    if(rot == 1 && flip == 1 ) { y=texty2;}
    if(rot == 2 && flip == 1 ) { x=textx1;}
    if(rot == 3 && flip == 1 ) { y=texty1;}
  }
  if(vcenter) {
    if(rot == 0 && flip == 0 ) { y=texty1;}
    if(rot == 1 && flip == 0 ) { x=textx2;}
    if(rot == 2 && flip == 0 ) { y=texty2;}
    if(rot == 3 && flip == 0 ) { x=textx1;}
    if(rot == 0 && flip == 1 ) { y=texty1;}
    if(rot == 1 && flip == 1 ) { x=textx2;}
    if(rot == 2 && flip == 1 ) { y=texty2;}
    if(rot == 3 && flip == 1 ) { x=textx1;}
  }
  llength=0;
  my_strdup2(54, &sss, str);
  tt=ss=sss;
  for(;;) {
    c=*ss;
    if(c=='\n' || c==0) {
      *ss='\0';
      ps_draw_string_line(layer, tt, x, y, size, rot, flip, lineno, height, ascent, descent, llength);
      lineno++;
      if(c==0) break;
      *ss='\n';
      tt=ss+1;
      llength=0;
    } else {
      llength++;
    }
    ss++;
  }
  my_free(53, &sss);
}

static void old_ps_draw_string(int gctext,  const char *str,
                 short rot, short flip, int hcenter, int vcenter,
                 double x1,double y1,
                 double xscale, double yscale)

{
 double a,yy,curr_x1,curr_y1,curr_x2,curr_y2,rx1,rx2,ry1,ry2;
 int pos=0,cc,pos2=0;
 int i;

 if(str==NULL) return;
 #if HAS_CAIRO==1
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
 xSymbol *symptr;
 char *textfont;

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
  symptr = (xctx->inst[n].ptr+ xctx->sym);
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
   if( enable_layer[layer] ) for(j=0;j< (xctx->inst[n].ptr+ xctx->sym)->rects[layer];j++)
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
     /* do not allow custom text color on PINLAYER hilighted instances */
     if( !(xctx->inst[n].color == PINLAYER)) {
       textlayer = (xctx->inst[n].ptr+ xctx->sym)->text[j].layer;
       if(textlayer < 0 || textlayer >= cadlayers) textlayer = layer;
     }
      /* display PINLAYER colored instance texts even if PINLAYER disabled */
     if(xctx->inst[n].color == PINLAYER || enable_layer[textlayer]) {
       my_snprintf(ps_font_family, S(ps_font_name), "Helvetica");
       my_snprintf(ps_font_name, S(ps_font_name), "Helvetica");
       textfont = symptr->text[j].font;
       if( (textfont && textfont[0])) {
         my_snprintf(ps_font_family, S(ps_font_family), textfont);
         my_snprintf(ps_font_name, S(ps_font_name), textfont);
       }
       if( symptr->text[j].flags & TEXT_BOLD)
         my_snprintf(ps_font_family, S(ps_font_family), "%s-Bold", ps_font_name);
       if( symptr->text[j].flags & TEXT_ITALIC)
         my_snprintf(ps_font_family, S(ps_font_family), "%s-Oblique", ps_font_name);
       if( symptr->text[j].flags & TEXT_OBLIQUE)
         my_snprintf(ps_font_family, S(ps_font_family), "%s-Oblique", ps_font_name);
       if(text_ps) {
         ps_draw_string(textlayer, txtptr,
           (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
           flip^text.flip, text.hcenter, text.vcenter,
           x0+x1, y0+y1, text.xscale, text.yscale);
       } else {
         old_ps_draw_string(textlayer, txtptr,
           (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
           flip^text.flip, text.hcenter, text.vcenter,
           x0+x1, y0+y1, text.xscale, text.yscale);
       }
     }
    }
   }

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
 double dx, dy, scale, scaley;
 int landscape=1;
 double margin=0; /* in postscript points, (1/72)". No need to add margin as xschem zoom full already has margins.*/

 /* Legal: 612 792 */
 double pagex=842;/* a4, in postscript points, (1/72)" */
 double pagey=595;/* a4, in postscript points, (1/72)" */
 xRect boundbox;
 int c,i, textlayer;
 char tmp[2*PATH_MAX+40];
 int old_grid;
 int modified_save;
 const char *r, *textfont;
 char *psfile;

 if(!plotfile[0]) {
   my_snprintf(tmp, S(tmp), "tk_getSaveFile -title {Select destination file} -initialdir [pwd]");
   tcleval(tmp);
   r = tclresult();
   if(r[0]) my_strncpy(plotfile, r, S(plotfile));
   else {
     return;
   }
 }

 if(!(fd = open_tmpfile("psplot_", &psfile)) ) {
   fprintf(errfp, "ps_draw(): can not create tmpfile %s\n", psfile);
   return;
 }

 modified_save=xctx->modified;
 push_undo();
 trim_wires();    /* add connection bubbles on wires but undo at end */
 ps_colors=my_calloc(311, cadlayers, sizeof(Ps_color));
 if(ps_colors==NULL){
   fprintf(errfp, "ps_draw(): calloc error\n");tcleval( "exit");
 }

 fill_ps_colors();
 old_grid=draw_grid;
 draw_grid=0;

 /* calc_drawing_bbox(&boundbox, 0);   ---.
  *                                       |
  *                                      \|/          */
 boundbox.x1 = xctx->areax1; /* X_TO_PS(boundbox.x1); */
 boundbox.x2 = xctx->areax2; /* X_TO_PS(boundbox.x2); */
 boundbox.y1 = xctx->areay1; /* Y_TO_PS(boundbox.y1); */
 boundbox.y2 = xctx->areay2; /* Y_TO_PS(boundbox.y2); */
 dx=boundbox.x2-boundbox.x1;
 dy=boundbox.y2-boundbox.y1;

 /* xschem window aspect ratio decides if portrait or landscape */
 if(dy > dx) landscape = 0;
 else landscape = 1;
 if(!landscape) {
   double tmp;
   tmp = pagex;
   pagex = pagey;
   pagey = tmp;
 }

 dbg(1, "ps_draw(): bbox: x1=%g y1=%g x2=%g y2=%g\n", boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
 fprintf(fd, "%%!PS-Adobe-3.0\n");
 /* fprintf(fd, "%%%%DocumentMedia: %s %g %g 80 () ()\n", landscape ? "a4land" : "a4", pagex, pagey); */
 fprintf(fd, "%%%%DocumentMedia: %s %g %g 80 () ()\n", "a4", pagex, pagey);
 fprintf(fd, "%%%%PageOrientation: %s\n", landscape ? "Landscape" : "Portrait");
 fprintf(fd, "%%%%Title: xschem plot\n");
 fprintf(fd, "%%%%Creator: xschem\n");
 fprintf(fd, "%%%%Pages: 1\n");
 fprintf(fd, "%%%%EndComments\n");
 fprintf(fd, "%%%%BeginProlog\n\n");
 fprintf(fd,"/cm {28.346457 mul} bind def\n");
 fprintf(fd,"/LT {lineto} bind def\n");
 fprintf(fd,"/MT {moveto} bind def\n");
 fprintf(fd,"/RMT {rmoveto} bind def\n");
 fprintf(fd,"/L {moveto lineto stroke} bind def\n");
 fprintf(fd,"/RGB {setrgbcolor} bind def\n");
 fprintf(fd,"/FF {findfont} bind def\n");
 fprintf(fd,"/SF {setfont} bind def\n");
 fprintf(fd,"/SCF {scalefont} bind def\n");
 fprintf(fd,"/SW {stringwidth} bind def\n");
 fprintf(fd,"/GS {gsave} bind def\n");
 fprintf(fd,"/GR {grestore} bind def\n");
 fprintf(fd,"/NP {newpath} bind def\n");
 fprintf(fd,"/A {arcn} bind def\n");
 fprintf(fd,"/R {rectstroke} bind def\n");
 fprintf(fd,"/S {stroke} bind def\n");
 fprintf(fd,"/C {closepath} bind def\n");
 fprintf(fd,"/F {fill} bind def\n");
 fprintf(fd,"/RF {rectfill} bind def\n");
 fprintf(fd, "%%%%EndProlog\n");
 fprintf(fd, "%%%%BeginSetup\n");
 fprintf(fd, "<< /PageSize [%g %g] /Orientation 0 >> setpagedevice\n", pagex, pagey);
 fprintf(fd, "%%%%Page: 1 1\n\n");
 fprintf(fd, "%%%%BeginPageSetup\n");
 fprintf(fd, "%%%%EndPageSetup\n");

 scaley = scale = (pagey-2 * margin) / dy;
 dbg(1, "scale=%g pagex=%g pagey=%g dx=%g dy=%g\n", scale, pagex, pagey, dx, dy);
 if(dx * scale > (pagex - 2 * margin)) {
   scale = (pagex - 2 * margin) / dx;
   dbg(1, "scale=%g\n", scale);
 }
 fprintf(fd, "%g %g translate\n", 
    -scale * boundbox.x1 + margin, pagey - (scaley - scale) * dy - margin + scale * boundbox.y1);
 fprintf(fd, "%g %g scale\n", scale, -scale);
 fprintf(fd, "1 setlinejoin 1 setlinecap\n");
 set_lw();
 ps_drawgrid();

 for(i=0;i<xctx->texts;i++)
 {
   textlayer = xctx->text[i].layer;
   if(textlayer < 0 ||  textlayer >= cadlayers) textlayer = TEXTLAYER;

   my_snprintf(ps_font_family, S(ps_font_name), "Helvetica");
   my_snprintf(ps_font_name, S(ps_font_name), "Helvetica");
   textfont = xctx->text[i].font;
   if( (textfont && textfont[0])) {
     my_snprintf(ps_font_family, S(ps_font_family), textfont);
     my_snprintf(ps_font_name, S(ps_font_name), textfont);
   }
   if( xctx->text[i].flags & TEXT_BOLD)
     my_snprintf(ps_font_family, S(ps_font_family), "%s-Bold", ps_font_name);
   if( xctx->text[i].flags & TEXT_ITALIC)
     my_snprintf(ps_font_family, S(ps_font_family), "%s-Oblique", ps_font_name);
   if( xctx->text[i].flags & TEXT_OBLIQUE)
     my_snprintf(ps_font_family, S(ps_font_family), "%s-Oblique", ps_font_name);

   if(text_ps) {
     ps_draw_string(textlayer, xctx->text[i].txt_ptr,
       xctx->text[i].rot, xctx->text[i].flip, xctx->text[i].hcenter, xctx->text[i].vcenter,
       xctx->text[i].x0,xctx->text[i].y0,
       xctx->text[i].xscale, xctx->text[i].yscale);
   } else {
     old_ps_draw_string(textlayer, xctx->text[i].txt_ptr,
       xctx->text[i].rot, xctx->text[i].flip, xctx->text[i].hcenter, xctx->text[i].vcenter,
       xctx->text[i].x0,xctx->text[i].y0,
       xctx->text[i].xscale, xctx->text[i].yscale);
   }
 }
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
 fprintf(fd, "%%%%trailer\n");
 fprintf(fd, "%%%%EOF\n");
 fclose(fd);
 draw_grid=old_grid;
 my_free(879, &ps_colors);
 if(plotfile[0]) {
   my_snprintf(tmp, S(tmp), "convert_to_pdf %s %s", psfile, plotfile);
 } else {
   my_snprintf(tmp, S(tmp), "convert_to_pdf %s plot.pdf", psfile);
 }
 my_strncpy(plotfile,"", S(plotfile));
 tcleval( tmp);
 pop_undo(0);
 xctx->modified=modified_save;
 Tcl_SetResult(interp,"",TCL_STATIC);
}

