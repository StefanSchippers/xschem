/* File: draw.c
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


/* Window doesn't work with LineDoubleDash, but will with LineOnOffDash */
#ifdef __unix__
#define xDashType LineDoubleDash
#else
#define xDashType LineOnOffDash
#endif

static double textx1,textx2,texty1,texty2;

int textclip(int x1,int y1,int x2,int y2,
          double xa,double ya,double xb,double yb)
/* check if some of (xa,ya-xb,yb) is inside (x1,y1-x2,y2) */
/* coordinates should be ordered, x1<x2,ya<yb and so on... */
{
 dbg(2, "textclip(): %.16g %.16g %.16g %.16g - %d %d %d %d\n",
 X_TO_SCREEN(xa),Y_TO_SCREEN(ya), X_TO_SCREEN(xb),Y_TO_SCREEN(yb),x1,y1,x2,y2);
 /* drawtemprect(gc[WIRELAYER],xa,ya,xb,yb); */
 if          (X_TO_SCREEN(xa)>x2) return 0;
 else if     (Y_TO_SCREEN(ya)>y2) return 0;
 else if     (X_TO_SCREEN(xb)<x1) return 0;
 else if     (Y_TO_SCREEN(yb)<y1) return 0;
 return 1;
}

void print_image()
{
  int w, h, tmp, ww, hh, save_draw_grid, changed_size;
  int modified_save;
  char cmd[PATH_MAX+100];
  const char *r;
  char *tmpstring=NULL;
  double saveorx, saveory, savezoom;

  if(!has_x) return ;

  changed_size = 0;
  w = ww = xschem_w;
  h = hh = xschem_h;
  if(!plotfile[0]) {
    my_snprintf(cmd, S(cmd), "input_line {Enter image size} {} {%dx%d}", xschem_w, xschem_h);
    tcleval(cmd);
    if(sscanf(tclresult(), "%dx%d", &w, &h) != 2) {
      w = xschem_w; h = xschem_h;
    } else {
     if(w != xschem_w || h != xschem_h) changed_size = 1;
    }
    my_strdup(60, &tmpstring, "tk_getSaveFile -title {Select destination file} -initialdir [pwd]");
    tcleval(tmpstring);
    r = tclresult();
    my_free(717, &tmpstring);
    if(r[0]) my_strncpy(plotfile, r, S(plotfile));
    else return;
  }

  modified_save=modified; /* 20161121 save state */
  push_undo();
  trim_wires();    /* 20161121 add connection boxes on wires but undo at end */

  XUnmapWindow(display, window);

  xrect[0].x = 0;
  xrect[0].y = 0;
  xschem_w = xrect[0].width = w;
  xschem_h = xrect[0].height = h;
  areax2 = w+2*lw;
  areay2 = h+2*lw;
  areax1 = -2*lw;
  areay1 = -2*lw;
  areaw = areax2-areax1;
  areah = areay2-areay1;
  saveorx = xctx.xorigin;
  saveory = xctx.yorigin;
  savezoom = xctx.zoom;
#ifdef __unix__
  XFreePixmap(display,save_pixmap);
  /* save_pixmap = XCreatePixmap(display,window,areaw,areah,depth); */
  save_pixmap = XCreatePixmap(display,window,w,h,depth); /* 20161119 pixmap should be exact size of  */
                                                         /* cliprectangle to avoid random borders */
#else
  Tk_FreePixmap(display, save_pixmap);
  save_pixmap = Tk_GetPixmap(display, window, w, h, depth);
#endif
  XSetTile(display, gctiled, save_pixmap);

  #ifdef HAS_CAIRO
  cairo_destroy(cairo_save_ctx);
  cairo_surface_destroy(save_sfc);

  #if HAS_XRENDER==1
  #if HAS_XCB==1
  save_sfc = cairo_xcb_surface_create_with_xrender_format(xcbconn, screen_xcb, save_pixmap, &format_rgb, w, h);
  #else
  save_sfc = cairo_xlib_surface_create_with_xrender_format(display,
             save_pixmap, DefaultScreenOfDisplay(display), format, w, h);
  #endif /*HAS_XCB */
  #else
  save_sfc = cairo_xlib_surface_create(display, save_pixmap, visual, w, h);
  #endif /*HAS_XRENDER */
  if(cairo_surface_status(save_sfc)!=CAIRO_STATUS_SUCCESS) {
    fprintf(errfp, "ERROR: invalid cairo xcb surface\n");
     exit(-1);
  }
  cairo_save_ctx = cairo_create(save_sfc);
  cairo_set_line_width(cairo_save_ctx, 1);
  cairo_set_line_join(cairo_save_ctx, CAIRO_LINE_JOIN_ROUND);
  cairo_set_line_cap(cairo_save_ctx, CAIRO_LINE_CAP_ROUND);
  cairo_select_font_face (cairo_save_ctx, cairo_font_name,
       CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cairo_save_ctx, 20);
  #endif /*HAS_CAIRO */
  for(tmp=0;tmp<cadlayers;tmp++)
  {
    XSetClipRectangles(display, gc[tmp], 0,0, xrect, 1, Unsorted);
    XSetClipRectangles(display, gcstipple[tmp], 0,0, xrect, 1, Unsorted);
  }
  XSetClipRectangles(display, gctiled, 0,0, xrect, 1, Unsorted);
  save_draw_grid = draw_grid;
  draw_grid=0;
  draw_pixmap=1;
  if(changed_size) zoom_full(0, 0);

  draw();
#ifdef __unix__
  XpmWriteFileFromPixmap(display, "plot.xpm", save_pixmap,0, NULL ); /* .gz ???? */
#endif
  dbg(1, "print_image(): Window image saved\n");

  if(plotfile[0]) {
    my_snprintf(cmd, S(cmd), "convert_to_png plot.xpm %s", plotfile);
    tcleval(cmd);
  } else tcleval( "convert_to_png plot.xpm plot.png");
  my_strncpy(plotfile,"", S(plotfile));
  pop_undo(0); /* 20161121 restore state */
  modified=modified_save;

  w=ww;h=hh;
  xrect[0].x = 0;
  xrect[0].y = 0;
  xschem_w = xrect[0].width = w;
  xschem_h = xrect[0].height = h;
  areax2 = w+2*lw;
  areay2 = h+2*lw;
  areax1 = -2*lw;
  areay1 = -2*lw;
  areaw = areax2-areax1;
  areah = areay2-areay1;
  xctx.zoom = savezoom;
  xctx.mooz = 1/xctx.zoom;
  xctx.xorigin = saveorx;
  xctx.yorigin = saveory;
#ifdef __unix__
  XFreePixmap(display,save_pixmap);
  save_pixmap = XCreatePixmap(display,window,areaw,areah,depth);
#else
  Tk_FreePixmap(display, save_pixmap);
  save_pixmap = Tk_GetPixmap(display, window, areaw, areah, depth);
#endif
  XSetTile(display, gctiled, save_pixmap);


#ifdef HAS_CAIRO
  cairo_destroy(cairo_save_ctx);
  cairo_surface_destroy(save_sfc);

  #if HAS_XRENDER==1
  #if HAS_XCB==1
  save_sfc = cairo_xcb_surface_create_with_xrender_format(xcbconn, screen_xcb, save_pixmap, &format_rgb, w, h);
  #else
  save_sfc = cairo_xlib_surface_create_with_xrender_format (display,
             save_pixmap, DefaultScreenOfDisplay(display), format, w, h);
  #endif /*HAS_XCB */
  #else
  save_sfc = cairo_xlib_surface_create(display, save_pixmap, visual, w, h);
  #endif /*HAS_XRENDER */
  if(cairo_surface_status(save_sfc)!=CAIRO_STATUS_SUCCESS) {
    fprintf(errfp, "ERROR: invalid cairo xcb surface\n");
     exit(-1);
  }
  cairo_save_ctx = cairo_create(save_sfc);
  cairo_set_line_width(cairo_save_ctx, 1);
  cairo_set_line_join(cairo_save_ctx, CAIRO_LINE_JOIN_ROUND);
  cairo_set_line_cap(cairo_save_ctx, CAIRO_LINE_CAP_ROUND);
  cairo_select_font_face (cairo_save_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cairo_save_ctx, 20);
  #endif /*HAS_CAIRO */

  for(tmp=0;tmp<cadlayers;tmp++)
  {
    XSetClipMask(display, gc[tmp], None); /*20171110 no need to clip, already done in software */
    XSetClipMask(display, gcstipple[tmp], None);
  }
  XSetClipMask(display, gctiled, None);

  XMapWindow(display, window);
  draw_grid=save_draw_grid;
  draw_pixmap=1;
  draw();
}

#ifdef HAS_CAIRO
/* remember to call cairo_restore(cairo_ctx) when done !! */
int set_text_custom_font(xText *txt) /* 20171122 for correct text_bbox calculation */
{
  char *textfont;

  textfont = txt->font;
  if((textfont && textfont[0]) || txt->flags) {
    cairo_font_slant_t slant;
    cairo_font_weight_t weight;
    textfont = (txt->font && txt->font[0]) ? txt->font : cairo_font_name;
    weight = ( txt->flags & TEXT_BOLD) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL;
    slant = CAIRO_FONT_SLANT_NORMAL;
    if(txt->flags & TEXT_ITALIC) slant = CAIRO_FONT_SLANT_ITALIC;
    if(txt->flags & TEXT_OBLIQUE) slant = CAIRO_FONT_SLANT_OBLIQUE;
    cairo_save(cairo_ctx);
    cairo_select_font_face (cairo_ctx, textfont, slant, weight);
    return 1;
  }
  return 0;
}
#else
int set_text_custom_font(xText *txt)
{
  return 0;
}
#endif


#ifdef HAS_CAIRO
void cairo_draw_string_line(cairo_t *cairo_ctx, char *s,
    double x, double y, double size, int rot, int flip,
    int lineno, double fontheight, double fontascent, double fontdescent, int llength)
{
  double ix, iy;
  int rot1;
  cairo_text_extents_t ext;
  int line_delta;
  int line_offset;
  double xadvance;
  double lines;
  double vc; /* 20171121 vert correct */
  /*int rx1, ry1, rx2, ry2, save_rot, save_flip; */
  /* GC gcclear; */
  if(s==NULL) return;
  if(llength==0) return;
  cairo_text_extents(cairo_ctx, s, &ext);
  xadvance = ext.x_advance > ext.width? ext.x_advance : ext.width;

  line_delta = lineno*fontheight*cairo_font_line_spacing;
  lines = (cairo_lines-1)*fontheight*cairo_font_line_spacing;
  line_offset=cairo_longest_line-xadvance;

  /* 20171215 NO! will skip drawing of long strings */
  /* if(xadvance>14000) return; */ /* too big: close to short overflow */

  ix=X_TO_SCREEN(x);
  iy=Y_TO_SCREEN(y);
  if(rot&1) {
    rot1=3;
  } else rot1=0;

  vc = cairo_vert_correct*xctx.mooz; /* converted to device (pixel) space */

  if(     rot==0 && flip==0) {iy+=line_delta+fontascent-vc;}
  else if(rot==1 && flip==0) {iy+=xadvance+line_offset;ix=ix-fontheight+fontascent+vc-lines+line_delta;}
  else if(rot==2 && flip==0) {iy=iy-fontheight-lines+line_delta+fontascent+vc; ix=ix-xadvance-line_offset;}
  else if(rot==3 && flip==0) {ix+=line_delta+fontascent-vc;}
  else if(rot==0 && flip==1) {ix=ix-xadvance-line_offset;iy+=line_delta+fontascent-vc;}
  else if(rot==1 && flip==1) {ix=ix-fontheight+line_delta-lines+fontascent+vc;}
  else if(rot==2 && flip==1) {iy=iy-fontheight-lines+line_delta+fontascent+vc;}
  else if(rot==3 && flip==1) {iy=iy+xadvance+line_offset;ix+=line_delta+fontascent-vc;}

  cairo_save(cairo_ctx);
  cairo_translate(cairo_ctx, ix, iy);
  cairo_rotate(cairo_ctx, XSCH_PI/2*rot1);

  /* fprintf(errfp, "string_line: |%s|, y_bearing: %f descent: %f ascent: %f height: %f\n", */
  /*     s, ext.y_bearing, fontdescent, fontascent, fontheight); */
  cairo_move_to(cairo_ctx, 0,0);
  cairo_show_text(cairo_ctx, s);
  cairo_restore(cairo_ctx);
}

/* CAIRO version */
void draw_string(int layer, int what, const char *s, int rot, int flip, int hcenter, int vcenter,
                 double x, double y, double xscale, double yscale)
{
  char *tt, *ss, *sss=NULL;
  char c;
  int lineno=0;
  double size;
  cairo_font_extents_t fext;
  int llength=0;

  (void)what; /* UNUSED in cairo version, avoid compiler warning */
  if(s==NULL || !has_x ) return;
  size = xscale*52.*cairo_font_scale;
  /*fprintf(errfp, "size=%.16g\n", size*xctx.mooz); */
  if(size*xctx.mooz<3.0) return; /* too small */
  if(size*xctx.mooz>1600) return; /* too big */

  text_bbox(s, xscale, yscale, rot, flip, hcenter, vcenter, x,y, &textx1,&texty1,&textx2,&texty2);
  if(!textclip(areax1,areay1,areax2,areay2,textx1,texty1,textx2,texty2)) {
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

  cairo_set_source_rgb(cairo_ctx,
    (double)xcolor_array[layer].red/65535.0,
    (double)xcolor_array[layer].green/65535.0,
    (double)xcolor_array[layer].blue/65535.0);
  cairo_set_source_rgb(cairo_save_ctx,
    (double)xcolor_array[layer].red/65535.0,
    (double)xcolor_array[layer].green/65535.0,
    (double)xcolor_array[layer].blue/65535.0);

  cairo_set_font_size (cairo_ctx, size*xctx.mooz);
  cairo_set_font_size (cairo_save_ctx, size*xctx.mooz);
  cairo_font_extents(cairo_ctx, &fext);
  llength=0;
  my_strdup2(73, &sss, s);
  tt=ss=sss;
  for(;;) {
    c=*ss;
    if(c=='\n' || c==0) {
      *ss='\0';
      /*fprintf(errfp, "cairo_draw_string(): tt=%s, longest line: %d\n", tt, cairo_longest_line); */
      if(draw_window) cairo_draw_string_line(cairo_ctx, tt, x, y, size, rot, flip,
         lineno, fext.height, fext.ascent, fext.descent, llength);
      if(draw_pixmap) cairo_draw_string_line(cairo_save_ctx, tt, x, y, size, rot, flip,
         lineno, fext.height, fext.ascent, fext.descent, llength);
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
  my_free(1157, &sss);
}

#else /* !HAS_CAIRO */

/* no CAIRO version */
void draw_string(int layer, int what, const char *str, int rot, int flip, int hcenter, int vcenter,
                 double x1,double y1, double xscale, double yscale)
{
 double a=0.0,yy;
 register double rx1=0,rx2=0,ry1=0,ry2=0;
 double curr_x1,curr_y1,curr_x2,curr_y2;
 register double zx1, invxscale;
 register int pos=0,pos2=0;
 register unsigned int cc;
 register double *char_ptr_x1,*char_ptr_y1,*char_ptr_x2,*char_ptr_y2;
 register int i,lines;

 if(str==NULL || !has_x ) return;
 dbg(2, "draw_string(): string=%s\n",str);
 if(xscale*FONTWIDTH*xctx.mooz<1) {
   dbg(1, "draw_string(): xscale=%.16g xctx.zoom=%.16g \n",xscale,xctx.zoom);
   return;
 }
 else {
  text_bbox(str, xscale, yscale, rot, flip, hcenter, vcenter, x1,y1, &textx1,&texty1,&textx2,&texty2);
  xscale*=nocairo_font_xscale;
  yscale*=nocairo_font_yscale;
  if(!textclip(areax1,areay1,areax2,areay2,textx1,texty1,textx2,texty2)) return;
  x1=textx1;y1=texty1;
  if(rot&1) {y1=texty2;rot=3;}
  else rot=0;
  flip = 0; yy=y1;
  invxscale=1/xscale;
  while(str[pos2]) {
     cc = (unsigned int)str[pos2++];
     if(cc>127) cc= '?';
     if(cc=='\n') {
        yy+=(FONTHEIGHT+FONTDESCENT+FONTWHITESPACE)* yscale;
        pos=0;
        a=0.0;
        continue;
     }
     lines=character[cc][0]*4;
     char_ptr_x1=character[cc]+1;
     char_ptr_y1=character[cc]+2;
     char_ptr_x2=character[cc]+3;
     char_ptr_y2=character[cc]+4;
     zx1=a+x1*invxscale;
     for(i=0;i<lines;i+=4) {
        curr_x1 = ( char_ptr_x1[i]+ zx1 ) * xscale ;
        curr_y1 = ( char_ptr_y1[i] ) * yscale+yy;
        curr_x2 = ( char_ptr_x2[i]+ zx1 ) * xscale ;
        curr_y2 = ( char_ptr_y2[i] ) * yscale+yy;
        ROTATION(x1,y1,curr_x1,curr_y1,rx1,ry1);
        ROTATION(x1,y1,curr_x2,curr_y2,rx2,ry2);
        ORDER(rx1,ry1,rx2,ry2);
        drawline(layer, what, rx1, ry1, rx2, ry2, 0);
     }
     pos++;
     a += FONTWIDTH+FONTWHITESPACE;
  }
 }
}

#endif /* HAS_CAIRO */

void draw_temp_string(GC gctext, int what, const char *str, int rot, int flip, int hcenter, int vcenter,
                 double x1,double y1, double xscale, double yscale)
{
 if(!has_x) return;
 dbg(2, "draw_string(): string=%s\n",str);
 if(!text_bbox(str, xscale, yscale, rot, flip, hcenter, vcenter, x1,y1, &textx1,&texty1,&textx2,&texty2)) return;
 drawtemprect(gctext,what, textx1,texty1,textx2,texty2);
}

void draw_symbol(int what,int c, int n,int layer,int tmp_flip, int rot,
        double xoffset, double yoffset)
                            /* draws current layer only, should be called within  */
{                           /* a "for(i=0;i<cadlayers;i++)" loop */
  register int j;
  register double x0,y0,x1,y1,x2,y2;
  int flip;
  int hide = 0;
  xLine line;
  xRect box;
  xArc arc;
  xPoly polygon;
  xText text;
  register xSymbol *symptr;
  int textlayer;
  double angle;
  #ifdef HAS_CAIRO
  char *textfont;
  #endif
  if(xctx.inst[n].ptr == -1) return;
  if( (layer != PINLAYER && !enable_layer[layer]) ) return;
  if(!has_x) return;
  if(
    (hide_symbols==1 && (xctx.inst[n].ptr+ xctx.sym)->prop_ptr &&
     !strcmp( (xctx.inst[n].ptr+ xctx.sym)->type, "subcircuit") ) || (hide_symbols == 2) ) {
    hide = 1;
  } else {
    hide = 0;
  }
  if(hide && layer == 0) {
    drawrect(4, what, xctx.inst[n].xx1, xctx.inst[n].yy1, xctx.inst[n].xx2, xctx.inst[n].yy2, 2);
  }
  if(layer==0) {
    x1=X_TO_SCREEN(xctx.inst[n].x1+xoffset);  /* 20150729 added xoffset, yoffset */
    x2=X_TO_SCREEN(xctx.inst[n].x2+xoffset);
    y1=Y_TO_SCREEN(xctx.inst[n].y1+yoffset);
    y2=Y_TO_SCREEN(xctx.inst[n].y2+yoffset);
    if(!only_probes && (x2-x1)< 0.3 && (y2-y1)< 0.3) {
      xctx.inst[n].flags|=1;
      return;
    }
    else if(OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2))
    {
     xctx.inst[n].flags|=1;
     return;
    }
    else xctx.inst[n].flags&=~1;

  } else if(xctx.inst[n].flags&1) {
    dbg(2, "draw_symbol(): skipping inst %d\n", n);
    return;
  }
  flip = xctx.inst[n].flip;
  if(tmp_flip) flip = !flip;
  rot = (xctx.inst[n].rot + rot ) & 0x3;

  x0=xctx.inst[n].x0 + xoffset;
  y0=xctx.inst[n].y0 + yoffset;
  symptr = (xctx.inst[n].ptr+ xctx.sym);
  if(!hide) {
    for(j=0;j< symptr->lines[layer];j++)
    {
      line = (symptr->line[layer])[j];
      ROTATION(0.0,0.0,line.x1,line.y1,x1,y1);
      ROTATION(0.0,0.0,line.x2,line.y2,x2,y2);
      ORDER(x1,y1,x2,y2);
      if(line.bus)
        drawline(c,THICK, x0+x1, y0+y1, x0+x2, y0+y2, line.dash);
      else
        drawline(c,what, x0+x1, y0+y1, x0+x2, y0+y2, line.dash);
    }
    for(j=0;j< symptr->polygons[layer];j++)
    {
      polygon = (symptr->poly[layer])[j];
      {   /* scope block so we declare some auxiliary arrays for coord transforms. 20171115 */
        int k;
        double *x = my_malloc(34, sizeof(double) * polygon.points);
        double *y = my_malloc(35, sizeof(double) * polygon.points);
        for(k=0;k<polygon.points;k++) {
          ROTATION(0.0,0.0,polygon.x[k],polygon.y[k],x[k],y[k]);
          x[k]+= x0;
          y[k] += y0;
        }
        drawpolygon(c, NOW, x, y, polygon.points, polygon.fill, polygon.dash); /* 20180914 added fill */
        my_free(718, &x);
        my_free(719, &y);
      }
    }
    for(j=0;j< symptr->arcs[layer];j++)
    {

      arc = (symptr->arc[layer])[j];
      if(flip) {
        angle = 270.*rot+180.-arc.b-arc.a;
      } else {
        angle = arc.a+rot*270.;
      }
      angle = fmod(angle, 360.);
      if(angle<0.) angle+=360.;
      ROTATION(0.0,0.0,arc.x,arc.y,x1,y1);
      drawarc(c,what, x0+x1, y0+y1, arc.r, angle, arc.b, arc.fill, arc.dash);
    }
  } /* if(!hide) */

  if( (!hide && (layer != PINLAYER || enable_layer[layer]))  ||
      (hide && layer == PINLAYER && enable_layer[layer]) ) {
    for(j=0;j< symptr->rects[layer];j++)
    {
      box = (symptr->rect[layer])[j];
      ROTATION(0.0,0.0,box.x1,box.y1,x1,y1);
      ROTATION(0.0,0.0,box.x2,box.y2,x2,y2);
      RECTORDER(x1,y1,x2,y2);
      drawrect(c,what, x0+x1, y0+y1, x0+x2, y0+y2, box.dash);
      filledrect(c,what, x0+x1, y0+y1, x0+x2, y0+y2);
    }
  }
  if( (layer==TEXTWIRELAYER && !(xctx.inst[n].flags&2) ) ||
      (sym_txt && (layer==TEXTLAYER) && (xctx.inst[n].flags&2) ) ) {
    const char *txtptr;
    for(j=0;j< symptr->texts;j++)
    {
      text = symptr->text[j];
      if(text.xscale*FONTWIDTH*xctx.mooz<1) continue;
      if( hide && text.txt_ptr && strcmp(text.txt_ptr, "@symname") && strcmp(text.txt_ptr, "@name") ) continue;
      txtptr= translate(n, text.txt_ptr);
      ROTATION(0.0,0.0,text.x0,text.y0,x1,y1);

      textlayer = c;
      if( !(c == PINLAYER && (xctx.inst[n].flags & 4))) {
        textlayer = symptr->text[j].layer;
        if(textlayer < 0 || textlayer >= cadlayers) textlayer = c;
      }
      if((c == PINLAYER && xctx.inst[n].flags & 4) ||  enable_layer[textlayer]) {
        #ifdef HAS_CAIRO
        textfont = symptr->text[j].font;
        if((textfont && textfont[0]) || symptr->text[j].flags) {
          cairo_font_slant_t slant;
          cairo_font_weight_t weight;
          textfont = (symptr->text[j].font && symptr->text[j].font[0]) ? symptr->text[j].font : cairo_font_name;
          weight = ( symptr->text[j].flags & TEXT_BOLD) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL;
          slant = CAIRO_FONT_SLANT_NORMAL;
          if(symptr->text[j].flags & TEXT_ITALIC) slant = CAIRO_FONT_SLANT_ITALIC;
          if(symptr->text[j].flags & TEXT_OBLIQUE) slant = CAIRO_FONT_SLANT_OBLIQUE;

          cairo_save(cairo_ctx);
          cairo_save(cairo_save_ctx);
          cairo_select_font_face (cairo_ctx, textfont, slant, weight);
          cairo_select_font_face (cairo_save_ctx, textfont, slant, weight);
        }
        #endif
        dbg(1, "drawing string: str=%s prop=%s\n", txtptr, text.prop_ptr);
        draw_string(textlayer, what, txtptr,
          (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
          flip^text.flip, text.hcenter, text.vcenter,
          x0+x1, y0+y1, text.xscale, text.yscale);
        #ifndef HAS_CAIRO
        drawrect(textlayer, END, 0.0, 0.0, 0.0, 0.0, 0);
        drawline(textlayer, END, 0.0, 0.0, 0.0, 0.0, 0);
        #endif
        #ifdef HAS_CAIRO
        if( (textfont && textfont[0]) || symptr->text[j].flags) {
          cairo_restore(cairo_ctx);
          cairo_restore(cairo_save_ctx);
        }
        #endif
      }
    }
  }
}

void draw_temp_symbol(int what, GC gc, int n,int layer,int tmp_flip, int rot,
        double xoffset, double yoffset)
                            /* draws current layer only, should be called within */
{                           /* a "for(i=0;i<cadlayers;i++)" loop */
 int j;
 double x0,y0,x1,y1,x2,y2;
 int flip;
 xLine line;
 xPoly polygon;
 xRect box;
 xArc arc;
 xText text;
 register xSymbol *symptr;
 double angle;

 #ifdef HAS_CAIRO
 int customfont;
 #endif

 if(xctx.inst[n].ptr == -1) return;
 if(!has_x) return;

 flip = xctx.inst[n].flip;
 if(tmp_flip) flip = !flip;
 rot = (xctx.inst[n].rot + rot ) & 0x3;

 if(layer==0) {
   x1=X_TO_SCREEN(xctx.inst[n].x1+xoffset); /* 20150729 added xoffset, yoffset */
   x2=X_TO_SCREEN(xctx.inst[n].x2+xoffset);
   y1=Y_TO_SCREEN(xctx.inst[n].y1+yoffset);
   y2=Y_TO_SCREEN (xctx.inst[n].y2+yoffset);
   if(OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2))
   {
    xctx.inst[n].flags|=1;
    return;
   }
   else xctx.inst[n].flags&=~1;

   /* following code handles different text color for labels/pins 06112002 */

 } else if(xctx.inst[n].flags&1) {
   dbg(2, "draw_symbol(): skipping inst %d\n", n);
   return;
 } /* /20150424 */

 x0=xctx.inst[n].x0 + xoffset;
 y0=xctx.inst[n].y0 + yoffset;
 symptr = (xctx.inst[n].ptr+ xctx.sym);
 for(j=0;j< symptr->lines[layer];j++)
 {
  line = (symptr->line[layer])[j];
  ROTATION(0.0,0.0,line.x1,line.y1,x1,y1);
  ROTATION(0.0,0.0,line.x2,line.y2,x2,y2);
  ORDER(x1,y1,x2,y2);
  if(line.bus)
    drawtempline(gc,THICK, x0+x1, y0+y1, x0+x2, y0+y2);
  else
    drawtempline(gc,what, x0+x1, y0+y1, x0+x2, y0+y2);
 }
 for(j=0;j< symptr->polygons[layer];j++)
 {
   /*fprintf(errfp, "draw_temp_symbol: polygon\n"); */
   polygon = (symptr->poly[layer])[j];

   {   /* scope block so we declare some auxiliary arrays for coord transforms. 20171115 */
     int k;
     double *x = my_malloc(36, sizeof(double) * polygon.points);
     double *y = my_malloc(37, sizeof(double) * polygon.points);
     for(k=0;k<polygon.points;k++) {
       ROTATION(0.0,0.0,polygon.x[k],polygon.y[k],x[k],y[k]);
       x[k] += x0;
       y[k] += y0;
     }
     drawtemppolygon(gc, NOW, x, y, polygon.points);
     my_free(720, &x);
     my_free(721, &y);
   }
 }

 for(j=0;j< symptr->rects[layer];j++)
 {
  box = (symptr->rect[layer])[j];
  ROTATION(0.0,0.0,box.x1,box.y1,x1,y1);
  ROTATION(0.0,0.0,box.x2,box.y2,x2,y2);
  RECTORDER(x1,y1,x2,y2);
  drawtemprect(gc,what, x0+x1, y0+y1, x0+x2, y0+y2);
 }
 for(j=0;j< symptr->arcs[layer];j++)
 {
   arc = (symptr->arc[layer])[j];
   if(flip) {
     angle = 270.*rot+180.-arc.b-arc.a;
   } else {
     angle = arc.a+rot*270.;
   }
   angle = fmod(angle, 360.);
   if(angle<0.) angle+=360.;
   ROTATION(0.0,0.0,arc.x,arc.y,x1,y1);
   drawtemparc(gc, what, x0+x1, y0+y1, arc.r, angle, arc.b);
 }

 if(layer==PROPERTYLAYER && sym_txt)
 {
  const char *txtptr;
  for(j=0;j< symptr->texts;j++)
  {
   text = symptr->text[j];
   if(text.xscale*FONTWIDTH*xctx.mooz<1) continue;
   txtptr= translate(n, text.txt_ptr);
   ROTATION(0.0,0.0,text.x0,text.y0,x1,y1);
   #ifdef HAS_CAIRO
   customfont = set_text_custom_font(&text);
   #endif
   if(txtptr[0]) draw_temp_string(gc, what, txtptr,
     (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
     flip^text.flip, text.hcenter, text.vcenter, x0+x1, y0+y1, text.xscale, text.yscale);
   #ifdef HAS_CAIRO
   if(customfont) cairo_restore(cairo_ctx);
   #endif

  }
 }
}

void drawgrid()
{
 double x,y;
 double delta,tmp;
 int i=0;
 if( !draw_grid || !has_x) return;
 delta=cadgrid*xctx.mooz;
 while(delta<CADGRIDTHRESHOLD) delta*=CADGRIDMULTIPLY;  /* <-- to be improved,but works */
 x = xctx.xorigin*xctx.mooz;y = xctx.yorigin*xctx.mooz;
 if(y>areay1 && y<areay2)
 {
  if(draw_window) XDrawLine(display, window, gc[GRIDLAYER],areax1+1,(int)y, areax2-1, (int)y);
  if(draw_pixmap)
    XDrawLine(display, save_pixmap, gc[GRIDLAYER],areax1+1,(int)y, areax2-1, (int)y);
 }
 if(x>areax1 && x<areax2)
 {
  if(draw_window) XDrawLine(display, window, gc[GRIDLAYER],(int)x,areay1+1, (int)x, areay2-1);
  if(draw_pixmap)
    XDrawLine(display, save_pixmap, gc[GRIDLAYER],(int)x,areay1+1, (int)x, areay2-1);
 }
 tmp = floor((areay1+1)/delta)*delta-fmod(-xctx.yorigin*xctx.mooz,delta);
 for(x=floor((areax1+1)/delta)*delta-fmod(-xctx.xorigin*xctx.mooz,delta);x<areax2;x+=delta)
 {
  for(y=tmp;y<areay2;y+=delta)
  {
   if(i>=CADMAXGRIDPOINTS)
   {
    if(draw_window) XDrawPoints(display,window,gc[GRIDLAYER],gridpoint,i,CoordModeOrigin);
    if(draw_pixmap)
      XDrawPoints(display,save_pixmap,gc[GRIDLAYER],gridpoint,i,CoordModeOrigin);
    i=0;
   }
   gridpoint[i].x=(int)(x);gridpoint[i++].y=(int)(y);
  }
 }

 if(draw_window) XDrawPoints(display,window,gc[GRIDLAYER],gridpoint,i,CoordModeOrigin);
 if(draw_pixmap)
   XDrawPoints(display,save_pixmap,gc[GRIDLAYER],gridpoint,i,CoordModeOrigin);
/* debug ... */
/* XFlush(display); */
}


void drawline(int c, int what, double linex1, double liney1, double linex2, double liney2, int dash)
{
  static int i = 0;
#ifndef __unix__
  int j = 0;
#endif
 static XSegment r[CADDRAWBUFFERSIZE];
 double x1,y1,x2,y2;
 register XSegment *rr;
 char dash_arr[2];

 if(dash && what !=THICK) what = NOW;

 if(!has_x) return;
 rr=r;
 if(what & ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
#ifdef __unix__
   if(draw_window) XDrawSegments(display, window, gc[c], rr,i);
   if(draw_pixmap)
     XDrawSegments(display, save_pixmap, gc[c], rr,i);
#else
    for (j = 0; j < i; ++j) {
      if (draw_window)
        XDrawLine(display, window, gc[c], rr[j].x1, rr[j].y1, rr[j].x2, rr[j].y2);
      if (draw_pixmap)
        XDrawLine(display, save_pixmap, gc[c], rr[j].x1, rr[j].y1, rr[j].x2, rr[j].y2);
    }
#endif
   i=0;
  }
  x1=X_TO_SCREEN(linex1);
  y1=Y_TO_SCREEN(liney1);
  x2=X_TO_SCREEN(linex2);
  y2=Y_TO_SCREEN(liney2);
  if(!only_probes && (x2-x1)< 0.3 && fabs(y2-y1) < 0.3) return;
  if( clip(&x1,&y1,&x2,&y2) )
  {
   rr[i].x1=(short)x1;
   rr[i].y1=(short)y1;
   rr[i].x2=(short)x2;
   rr[i].y2=(short)y2;
   i++;
  }
 }
 else if(what & NOW)
 {
  x1=X_TO_SCREEN(linex1);
  y1=Y_TO_SCREEN(liney1);
  x2=X_TO_SCREEN(linex2);
  y2=Y_TO_SCREEN(liney2);
  if(!only_probes && (x2-x1)< 0.3 && fabs(y2-y1)< 0.3) return;
  if( clip(&x1,&y1,&x2,&y2) )
  {
   if(dash) {
     dash_arr[0] = dash_arr[1] = dash;
     XSetDashes(display, gc[c], 0, dash_arr, 2);
     XSetLineAttributes (display, gc[c], lw, xDashType, CapButt, JoinBevel);
   }
   if(draw_window) XDrawLine(display, window, gc[c], x1, y1, x2, y2);
   if(draw_pixmap)
    XDrawLine(display, save_pixmap, gc[c], x1, y1, x2, y2);
   if(dash) {
     XSetLineAttributes (display, gc[c], lw, LineSolid, CapRound, JoinRound);
   }
  }
 }

 else if(what & THICK)
 {
  x1=X_TO_SCREEN(linex1);
  y1=Y_TO_SCREEN(liney1);
  x2=X_TO_SCREEN(linex2);
  y2=Y_TO_SCREEN(liney2);
  if(!only_probes && (x2-x1)< 0.3 && fabs(y2-y1)< 0.3) return;
  if( clip(&x1,&y1,&x2,&y2) )
  {
   if(dash) {
     dash_arr[0] = dash_arr[1] = dash;
     XSetDashes(display, gc[c], 0, dash_arr, 2);
     XSetLineAttributes (display, gc[c], bus_width, xDashType, CapButt, JoinBevel);
   } else {
     XSetLineAttributes (display, gc[c], bus_width, LineSolid, CapRound, JoinRound);
   }
   if(draw_window) XDrawLine(display, window, gc[c], x1, y1, x2, y2);
   if(draw_pixmap) XDrawLine(display, save_pixmap, gc[c], x1, y1, x2, y2);
   XSetLineAttributes (display, gc[c], lw, LineSolid, CapRound , JoinRound);
  }
 }
 else if(what & BEGIN) i=0;
 else if((what & END) && i)
 {
#ifdef __unix__
  if(draw_window) XDrawSegments(display, window, gc[c], rr,i);
  if(draw_pixmap) XDrawSegments(display, save_pixmap, gc[c], rr,i);
#else
   for (j = 0; j < i; ++j) {
     if (draw_window)
       XDrawLine(display, window, gc[c], rr[j].x1, rr[j].y1, rr[j].x2, rr[j].y2);
     if (draw_pixmap)
       XDrawLine(display, save_pixmap, gc[c], rr[j].x1, rr[j].y1, rr[j].x2, rr[j].y2);
   }
#endif
  i=0;
 }
}

void drawtempline(GC gc, int what, double linex1,double liney1,double linex2,double liney2)
{
  static int i = 0;
#ifndef __unix__
 int j = 0;
#endif
 static XSegment r[CADDRAWBUFFERSIZE];
 double x1,y1,x2,y2;

 if(!has_x) return;
 if(what & ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
#ifdef __unix__
   XDrawSegments(display, window, gc, r,i);
#else
    for (j = 0; j < i; ++j) {
        XDrawLine(display, window, gc, r[j].x1, r[j].y1, r[j].x2, r[j].y2);
    }
#endif
   i=0;
  }
  x1=X_TO_SCREEN(linex1);
  y1=Y_TO_SCREEN(liney1);
  x2=X_TO_SCREEN(linex2);
  y2=Y_TO_SCREEN(liney2);
  if( clip(&x1,&y1,&x2,&y2) )
  {
   r[i].x1=(short)x1;
   r[i].y1=(short)y1;
   r[i].x2=(short)x2;
   r[i].y2=(short)y2;
   i++;
  }
 }
 else if(what & NOW)
 {
  x1=X_TO_SCREEN(linex1);
  y1=Y_TO_SCREEN(liney1);
  x2=X_TO_SCREEN(linex2);
  y2=Y_TO_SCREEN(liney2);
  if( clip(&x1,&y1,&x2,&y2) )
  {
   XDrawLine(display, window, gc, x1, y1, x2, y2);
  }
 }
 else if(what & THICK)
 {
  x1=X_TO_SCREEN(linex1);
  y1=Y_TO_SCREEN(liney1);
  x2=X_TO_SCREEN(linex2);
  y2=Y_TO_SCREEN(liney2);
  if( clip(&x1,&y1,&x2,&y2) )
  {
   XSetLineAttributes (display, gc, bus_width, LineSolid, CapRound , JoinRound);

   XDrawLine(display, window, gc, x1, y1, x2, y2);
   XSetLineAttributes (display, gc, lw, LineSolid, CapRound , JoinRound);
  }
 }

 else if(what & BEGIN) i=0;
 else if((what & END) && i)
 {
#ifdef __unix__
  XDrawSegments(display, window, gc, r,i);
#else
   for (j = 0; j < i; ++j) {
     XDrawLine(display, window, gc, r[j].x1, r[j].y1, r[j].x2, r[j].y2);
   }
#endif
  i=0;
 }
}

void drawtemparc(GC gc, int what, double x, double y, double r, double a, double b)
{
 static int i=0;
 static XArc xarc[CADDRAWBUFFERSIZE];
 double x1, y1, x2, y2; /* arc bbox */
 double xx1, yy1, xx2, yy2; /* complete circle bbox in screen coords */

 if(!has_x) return;
 if(what & ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
   XDrawArcs(display, window, gc, xarc,i);
   i=0;
  }
  xx1=X_TO_SCREEN(x-r);
  yy1=Y_TO_SCREEN(y-r);
  xx2=X_TO_SCREEN(x+r);
  yy2=Y_TO_SCREEN(y+r);
  arc_bbox(x, y, r, a, b, &x1,&y1,&x2,&y2);
  x1=X_TO_SCREEN(x1);
  y1=Y_TO_SCREEN(y1);
  x2=X_TO_SCREEN(x2);
  y2=Y_TO_SCREEN(y2);
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   xarc[i].x=(short)xx1;
   xarc[i].y=(short)yy1;
   xarc[i].width= (unsigned short)(xx2 - xx1);
   xarc[i].height=(unsigned short)(yy2 - yy1);
   xarc[i].angle1 = a*64;
   xarc[i].angle2 = b*64;
   i++;
  }
 }
 else if(what & NOW)
 {
  xx1=X_TO_SCREEN(x-r);
  yy1=Y_TO_SCREEN(y-r);
  xx2=X_TO_SCREEN(x+r);
  yy2=Y_TO_SCREEN(y+r);
  arc_bbox(x, y, r, a, b, &x1,&y1,&x2,&y2);
  x1=X_TO_SCREEN(x1);
  y1=Y_TO_SCREEN(y1);
  x2=X_TO_SCREEN(x2);
  y2=Y_TO_SCREEN(y2);
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   XDrawArc(display, window, gc, xx1, yy1, xx2-xx1, yy2-yy1, a*64, b*64);
  }
 }
 else if(what & BEGIN) i=0;
 else if((what & END) && i)
 {
  XDrawArcs(display, window, gc, xarc,i);
  i=0;
 }
}

/* x1,y1: start; x2,y2: end; x3,y3: way point */
void arc_3_points(double x1, double y1, double x2, double y2, double x3, double y3,
         double *x, double *y, double *r, double *a, double *b)
{
  double A, B, C;
  double c, s;

  /* s = signed_area, if > 0 : clockwise in xorg coordinate space */
  s = x3*y2-x2*y3 + x2*y1 -x1*y2 + x1*y3-x3*y1;
  A = x1*(y2-y3) - y1*(x2-x3) + x2*y3 - x3*y2;
  B = (x1*x1+y1*y1)*(y3-y2)+(x2*x2+y2*y2)*(y1-y3) + (x3*x3+y3*y3)*(y2-y1);
  C = (x1*x1+y1*y1)*(x2-x3)+(x2*x2+y2*y2)*(x3-x1) + (x3*x3+y3*y3)*(x1-x2);
  /* printf("s=%g\n", s); */
  *x = -B/2./A;
  *y = -C/2./A;
  *r = sqrt( (*x-x1)*(*x-x1) + (*y-y1)*(*y-y1) );
  *a = fmod(atan2(*y-y1 ,x1-*x )*180./XSCH_PI, 360.);
  if(*a<0.) *a+=360.;
  *b = fmod(atan2(*y-y2 ,x2-*x )*180./XSCH_PI, 360.);
  if(*b<0.) *b+=360.;
  if(s<0.) {  /* counter clockwise, P1, P3, P2 */
    *b = fmod(*b-*a, 360.);
    if(*b<0) *b+=360.;
    if(*b==0) *b=360.;
  } else if(s>0.) { /* clockwise, P2, P3, P1 */
    c = fmod(*a-*b, 360.);
    if(c<0) c+=360.;
    if(*b==0) *b=360.;
    *a = *b;
    *b = c;
  } else {
    *r = -1.0; /* no circle thru aligned points */
  }
}

void filledarc(int c, int what, double x, double y, double r, double a, double b)
{
 static int i=0;
 static XArc xarc[CADDRAWBUFFERSIZE];
 double x1, y1, x2, y2; /* arc bbox */
 double xx1, yy1, xx2, yy2; /* complete circle bbox in screen coords */

 if(!has_x) return;
 if(what & ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
   if(draw_window) XFillArcs(display, window, gc[c], xarc,i);
   if(draw_pixmap) XFillArcs(display, save_pixmap, gc[c], xarc,i);
   i=0;
  }
  xx1=X_TO_SCREEN(x-r);
  yy1=Y_TO_SCREEN(y-r);
  xx2=X_TO_SCREEN(x+r);
  yy2=Y_TO_SCREEN(y+r);
  arc_bbox(x, y, r, a, b, &x1,&y1,&x2,&y2);
  x1=X_TO_SCREEN(x1);
  y1=Y_TO_SCREEN(y1);
  x2=X_TO_SCREEN(x2);
  y2=Y_TO_SCREEN(y2);
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   xarc[i].x=(short)xx1;
   xarc[i].y=(short)yy1;
   xarc[i].width =(unsigned short)(xx2 - xx1);
   xarc[i].height=(unsigned short)(yy2 - yy1);
   xarc[i].angle1 = a*64;
   xarc[i].angle2 = b*64;
   i++;
  }
 }
 else if(what & NOW)
 {
  xx1=X_TO_SCREEN(x-r);
  yy1=Y_TO_SCREEN(y-r);
  xx2=X_TO_SCREEN(x+r);
  yy2=Y_TO_SCREEN(y+r);
  arc_bbox(x, y, r, a, b, &x1,&y1,&x2,&y2);
  x1=X_TO_SCREEN(x1);
  y1=Y_TO_SCREEN(y1);
  x2=X_TO_SCREEN(x2);
  y2=Y_TO_SCREEN(y2);
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   if(draw_window) XFillArc(display, window, gc[c], xx1, yy1, xx2-xx1, yy2-yy1, a*64, b*64);
   if(draw_pixmap) XFillArc(display, save_pixmap, gc[c], xx1, yy1, xx2-xx1, yy2-yy1, a*64, b*64);
  }
 }
 else if(what & BEGIN) i=0;
 else if((what & END) && i)
 {
  if(draw_window) XFillArcs(display, window, gc[c], xarc,i);
  if(draw_pixmap) XFillArcs(display, save_pixmap, gc[c], xarc,i);
  i=0;
 }
}


void drawarc(int c, int what, double x, double y, double r, double a, double b, int arc_fill, int dash)
{
 static int i=0;
 static XArc xarc[CADDRAWBUFFERSIZE];
 double x1, y1, x2, y2; /* arc bbox */
 double xx1, yy1, xx2, yy2; /* complete circle bbox in screen coords */

 if(arc_fill || dash) what = NOW;

 if(!has_x) return;
 if(what & ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
   if(draw_window) XDrawArcs(display, window, gc[c], xarc,i);
   if(draw_pixmap) XDrawArcs(display, save_pixmap, gc[c], xarc,i);
   i=0;
  }
  xx1=X_TO_SCREEN(x-r);
  yy1=Y_TO_SCREEN(y-r);
  xx2=X_TO_SCREEN(x+r);
  yy2=Y_TO_SCREEN(y+r);
  arc_bbox(x, y, r, a, b, &x1,&y1,&x2,&y2);
  x1=X_TO_SCREEN(x1);
  y1=Y_TO_SCREEN(y1);
  x2=X_TO_SCREEN(x2);
  y2=Y_TO_SCREEN(y2);
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   xarc[i].x=(short)xx1;
   xarc[i].y=(short)yy1;
   xarc[i].width =(unsigned short)(xx2 - xx1);
   xarc[i].height=(unsigned short)(yy2 - yy1);
   xarc[i].angle1 = a*64;
   xarc[i].angle2 = b*64;
   i++;
  }
 }
 else if(what & NOW)
 {
  xx1=X_TO_SCREEN(x-r);
  yy1=Y_TO_SCREEN(y-r);
  xx2=X_TO_SCREEN(x+r);
  yy2=Y_TO_SCREEN(y+r);
  if(arc_fill)
    arc_bbox(x, y, r, 0, 360, &x1,&y1,&x2,&y2);
  else
    arc_bbox(x, y, r, a, b, &x1,&y1,&x2,&y2);
  x1=X_TO_SCREEN(x1);
  y1=Y_TO_SCREEN(y1);
  x2=X_TO_SCREEN(x2);
  y2=Y_TO_SCREEN(y2);
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   if(dash) {
     char dash_arr[2];
     dash_arr[0] = dash_arr[1] = dash;
     XSetDashes(display, gc[c], 0, dash_arr, 2);
     XSetLineAttributes (display, gc[c], lw, xDashType, CapButt, JoinBevel);
   }

   if(draw_window) {
     XDrawArc(display, window, gc[c], xx1, yy1, xx2-xx1, yy2-yy1, a*64, b*64);
   }
   if(draw_pixmap) {
     XDrawArc(display, save_pixmap, gc[c], xx1, yy1, xx2-xx1, yy2-yy1, a*64, b*64);
   }

   if(fill && fill_type[c]){
     if(arc_fill) {
       if(draw_window)
         XFillArc(display, window, gcstipple[c], xx1, yy1, xx2-xx1, yy2-yy1, a*64, b*64);
       if(draw_pixmap)
         XFillArc(display, save_pixmap, gcstipple[c], xx1, yy1, xx2-xx1, yy2-yy1, a*64, b*64);
     }
   }
   if(dash) {
     XSetLineAttributes (display, gc[c], lw ,LineSolid, CapRound , JoinRound);
   }
  }
 }
 else if(what & BEGIN) i=0;
 else if((what & END) && i)
 {
  if(draw_window) XDrawArcs(display, window, gc[c], xarc,i);
  if(draw_pixmap) XDrawArcs(display, save_pixmap, gc[c], xarc,i);
  i=0;
 }
}


void filledrect(int c, int what, double rectx1,double recty1,double rectx2,double recty2)
{
 static int i=0;
 static XRectangle r[CADDRAWBUFFERSIZE];
 double x1,y1,x2,y2;

 if(!has_x) return;
 if(!fill || !fill_type[c]) return;
 if(what & NOW)
 {
  x1=X_TO_SCREEN(rectx1);
  y1=Y_TO_SCREEN(recty1);
  x2=X_TO_SCREEN(rectx2);
  y2=Y_TO_SCREEN(recty2);
  if(!only_probes && (x2-x1)< 2 && (y2-y1)< 2) return;
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   if(draw_window) XFillRectangle(display, window, gcstipple[c], (int)x1, (int)y1,
    (unsigned int)x2 - (unsigned int)x1,
    (unsigned int)y2 - (unsigned int)y1);
   if(draw_pixmap)
     XFillRectangle(display, save_pixmap,gcstipple[c],  (int)x1, (int)y1,
      (unsigned int)x2 - (unsigned int)x1,
      (unsigned int)y2 - (unsigned int)y1);
  }
 }
 else if(what & BEGIN) i=0;
 else if(what & ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
   if(draw_window) XFillRectangles(display, window, gcstipple[c], r,i);
   if(draw_pixmap)
     XFillRectangles(display, save_pixmap, gcstipple[c], r,i);
   i=0;
  }
  x1=X_TO_SCREEN(rectx1);
  y1=Y_TO_SCREEN(recty1);
  x2=X_TO_SCREEN(rectx2);
  y2=Y_TO_SCREEN(recty2);
  if(!only_probes && (x2-x1)< 2 && (y2-y1)< 2) return;
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   r[i].x=(short)x1;
   r[i].y=(short)y1;
   r[i].width=(unsigned short)x2-r[i].x;
   r[i].height=(unsigned short)y2-r[i].y;
   i++;
  }
 }
 else if((what & END) && i)
 {
  if(draw_window) XFillRectangles(display, window, gcstipple[c], r,i);
  if(draw_pixmap) XFillRectangles(display, save_pixmap, gcstipple[c], r,i);
  i=0;
 }
}

void polygon_bbox(double *x, double *y, int points, double *bx1, double *by1, double *bx2, double *by2)
{
  int j;
  for(j=0; j<points; j++) {
    if(j==0 || x[j] < *bx1) *bx1 = x[j];
    if(j==0 || y[j] < *by1) *by1 = y[j];
    if(j==0 || x[j] > *bx2) *bx2 = x[j];
    if(j==0 || y[j] > *by2) *by2 = y[j];
  }
}


void arc_bbox(double x, double y, double r, double a, double b,
              double *bx1, double *by1, double *bx2, double *by2)
{
  double x2, y2, x3, y3;
  int aa, bb, i;

  if(b==360.) {
    *bx1 = x-r;
    *by1 = y-r;
    *bx2 = x+r;
    *by2 = y+r;
    return;
  }
  a = fmod(a, 360.);
  if(a<0) a+=360.;
  aa = (int)(ceil(a/90.))*90;
  bb = (int)(floor((a+b)/90.))*90;


  /* printf("arc_bbox(): aa=%d bb=%d\n", aa, bb); */
  x2 = x + r * cos(a * XSCH_PI/180.);
  y2 = y - r * sin(a * XSCH_PI/180.);
  x3 = x + r * cos((a+b) * XSCH_PI/180.);
  y3 = y - r * sin((a+b) * XSCH_PI/180.);

  /* *bx1  = (x2 < x  ) ? x2 : x; */
  *bx1 = x2;
  if(x3 < *bx1) *bx1 = x3;
  /* *bx2  = (x2 > x  ) ? x2 : x; */
  *bx2 = x2;
  if(x3 > *bx2) *bx2 = x3;
  /* *by1  = (y2 < y  ) ? y2 : y; */
  *by1  = y2;
  if(y3 < *by1) *by1 = y3;
  /* *by2  = (y2 > y  ) ? y2 : y; */
  *by2  = y2;
  if(y3 > *by2) *by2 = y3;

  for(i=aa; i<=bb; i++) {
    if(i%360==0) {
      *bx2 = x + r;
    }
    if(i%360==90) {
      *by1 = y - r;
    }
    if(i%360==180) {
      *bx1 = x - r;
    }
    if(i%360==270) {
      *by2 = y + r;
    }
  }
  /* printf("arc_bbox(): bx1=%g by1=%g bx2=%g by2=%g\n", *bx1, *by1, *bx2, *by2); */
}

/* Convex Nonconvex Complex */
#define Polygontype Nonconvex
/* 20180914 added fill param */
void drawpolygon(int c, int what, double *x, double *y, int points, int poly_fill, int dash)
{
  double x1,y1,x2,y2;
  XPoint *p;
  int i;
  if(!has_x) return;

  polygon_bbox(x, y, points, &x1,&y1,&x2,&y2);
  x1=X_TO_SCREEN(x1);
  y1=Y_TO_SCREEN(y1);
  x2=X_TO_SCREEN(x2);
  y2=Y_TO_SCREEN(y2);
  if( !rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) ) {
    return;
  }
  if( !only_probes && (x2-x1)<0.3 && (y2-y1)<0.3) return;

  p = my_malloc(38, sizeof(XPoint) * points);
  for(i=0;i<points; i++) {
    p[i].x = X_TO_SCREEN(x[i]);
    p[i].y = Y_TO_SCREEN(y[i]);
  }
  if(dash) {
    char dash_arr[2];
    dash_arr[0] = dash_arr[1] = dash;
    XSetDashes(display, gc[c], 0, dash_arr, 2);
    XSetLineAttributes (display, gc[c], lw, xDashType, CapButt, JoinBevel);
  }
  if(draw_window) XDrawLines(display, window, gc[c], p, points, CoordModeOrigin);
  if(draw_pixmap)
    XDrawLines(display, save_pixmap, gc[c], p, points, CoordModeOrigin);
  if(fill && fill_type[c]){
    if(poly_fill && (x[0] == x[points-1]) && (y[0] == y[points-1])) {
      if(draw_window) XFillPolygon(display, window, gcstipple[c], p, points, Polygontype, CoordModeOrigin);
      if(draw_pixmap)
         XFillPolygon(display, save_pixmap, gcstipple[c], p, points, Polygontype, CoordModeOrigin);
    }
  }
  if(dash) {
    XSetLineAttributes (display, gc[c], lw ,LineSolid, CapRound , JoinRound);
  }

  my_free(722, &p);
}

void drawtemppolygon(GC g, int what, double *x, double *y, int points)
{
  double x1,y1,x2,y2;
  XPoint *p;
  int i;
  if(!has_x) return;
  polygon_bbox(x, y, points, &x1,&y1,&x2,&y2);
  x1=X_TO_SCREEN(x1);
  y1=Y_TO_SCREEN(y1);
  x2=X_TO_SCREEN(x2);
  y2=Y_TO_SCREEN(y2);
  p = my_malloc(39, sizeof(XPoint) * points);
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) ) {
    for(i=0;i<points; i++) {
      p[i].x = X_TO_SCREEN(x[i]);
      p[i].y = Y_TO_SCREEN(y[i]);
    }
    XDrawLines(display, window, g, p, points, CoordModeOrigin);
  }
  my_free(723, &p);
}

void drawrect(int c, int what, double rectx1,double recty1,double rectx2,double recty2, int dash)
{
 static int i=0;
 static XRectangle r[CADDRAWBUFFERSIZE];
 double x1,y1,x2,y2;
 char dash_arr[2];

 if(!has_x) return;
 if(dash) what = NOW;
 if(what & NOW)
 {
  x1=X_TO_SCREEN(rectx1);
  y1=Y_TO_SCREEN(recty1);
  x2=X_TO_SCREEN(rectx2);
  y2=Y_TO_SCREEN(recty2);
  if(!only_probes && (x2-x1)< 0.3 && (y2-y1)< 0.3) return;
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   if(dash) {
     dash_arr[0] = dash_arr[1] = dash;
     XSetDashes(display, gc[c], 0, dash_arr, 2);
     XSetLineAttributes (display, gc[c], lw, xDashType, CapButt, JoinBevel);
   }
   if(draw_window) XDrawRectangle(display, window, gc[c], (int)x1, (int)y1,
    (unsigned int)x2 - (unsigned int)x1,
    (unsigned int)y2 - (unsigned int)y1);
   if(draw_pixmap)
   {
    XDrawRectangle(display, save_pixmap, gc[c], (int)x1, (int)y1,
    (unsigned int)x2 - (unsigned int)x1,
    (unsigned int)y2 - (unsigned int)y1);
   }
   if(dash) {
     XSetLineAttributes (display, gc[c], lw ,LineSolid, CapRound , JoinRound);
   }
  }
 }
 else if(what & BEGIN) i=0;
 else if(what & ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
   if(draw_window) XDrawRectangles(display, window, gc[c], r,i);
   if(draw_pixmap)
     XDrawRectangles(display, save_pixmap, gc[c], r,i);
   i=0;
  }
  x1=X_TO_SCREEN(rectx1);
  y1=Y_TO_SCREEN(recty1);
  x2=X_TO_SCREEN(rectx2);
  y2=Y_TO_SCREEN(recty2);
  if(!only_probes && (x2-x1)< 0.3 && (y2-y1)< 0.3) return;
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   r[i].x=(short)x1;
   r[i].y=(short)y1;
   r[i].width=(unsigned short)x2-r[i].x;
   r[i].height=(unsigned short)y2-r[i].y;
   i++;
  }
 }
 else if((what & END) && i)
 {
  if(draw_window) XDrawRectangles(display, window, gc[c], r,i);
  if(draw_pixmap) XDrawRectangles(display, save_pixmap, gc[c], r,i);
  i=0;
 }
}

void drawtemprect(GC gc, int what, double rectx1,double recty1,double rectx2,double recty2)
{
 static int i=0;
 static XRectangle r[CADDRAWBUFFERSIZE];
 double x1,y1,x2,y2;

 if(!has_x) return;
 if(what & NOW)
 {
  x1=X_TO_SCREEN(rectx1);
  y1=Y_TO_SCREEN(recty1);
  x2=X_TO_SCREEN(rectx2);
  y2=Y_TO_SCREEN(recty2);
  if( (x2-x1)< 0.3 && (y2-y1)< 0.3) return;
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   XDrawRectangle(display, window, gc, (int)x1, (int)y1,
    (unsigned int)x2 - (unsigned int)x1,
    (unsigned int)y2 - (unsigned int)y1);
  }
 }
 else if(what & BEGIN) i=0;
 else if(what & ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
   XDrawRectangles(display, window, gc, r,i);
   i=0;
  }
  x1=X_TO_SCREEN(rectx1);
  y1=Y_TO_SCREEN(recty1);
  x2=X_TO_SCREEN(rectx2);
  y2=Y_TO_SCREEN(recty2);
  if( (x2-x1)< 0.3 && (y2-y1)< 0.3) return;
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   r[i].x=(short)x1;
   r[i].y=(short)y1;
   r[i].width=(unsigned short)x2-r[i].x;
   r[i].height=(unsigned short)y2-r[i].y;
   i++;
  }
 }
 else if((what & END) && i)
 {
  XDrawRectangles(display, window, gc, r,i);
  i=0;
 }
}

void draw(void)
{
 /* inst_ptr  and wire hash iterator 20171224 */
 double x1, y1, x2, y2;
 struct instentry *instanceptr;
 struct wireentry *wireptr;
 int ii;
 char *type=NULL;
 int use_hash;
 register int c,i;
 register xSymbol *symptr;
 int textlayer;

  #ifdef HAS_CAIRO
  char *textfont;
  #endif

 if(no_draw) return;
 rebuild_selected_array();
 if(has_x) {
    if(draw_pixmap)
      XFillRectangle(display, save_pixmap, gc[BACKLAYER], areax1, areay1, areaw, areah);
    if(draw_window) XFillRectangle(display, window, gc[BACKLAYER], areax1, areay1, areaw, areah);
    dbg(2, "draw(): window: %d %d %d %d\n",areax1, areay1, areax2, areay2);
    drawgrid();
    x1 = X_TO_XSCHEM(areax1);
    y1 = Y_TO_XSCHEM(areay1);
    x2 = X_TO_XSCHEM(areax2);
    y2 = Y_TO_XSCHEM(areay2);
    use_hash =  (xctx.wires> 2000 || xctx.instances > 2000 ) &&  (x2 - x1  < ITERATOR_THRESHOLD);

    if(use_hash) {
      hash_instances();
      hash_wires();
    }
    if(!only_probes) {
        dbg(3, "draw(): check4\n");
        for(c=0;c<cadlayers;c++)
        {
          if(draw_single_layer!=-1 && c != draw_single_layer) continue;

          if(enable_layer[c]) for(i=0;i<xctx.lines[c];i++) {
            xLine *l = &xctx.line[c][i];
            if(l->bus)
              drawline(c, THICK, l->x1, l->y1, l->x2, l->y2, l->dash);
            else
              drawline(c, ADD, l->x1, l->y1, l->x2, l->y2, l->dash);
          }
          if(enable_layer[c]) for(i=0;i<xctx.rects[c];i++)
          {
            xRect *r = &xctx.rect[c][i];
            drawrect(c, ADD, r->x1, r->y1, r->x2, r->y2, r->dash);
            filledrect(c, ADD, r->x1, r->y1, r->x2, r->y2);
          }
          if(enable_layer[c]) for(i=0;i<xctx.arcs[c];i++)
          {
            xArc *a = &xctx.arc[c][i];
            drawarc(c, ADD, a->x, a->y, a->r, a->a, a->b, a->fill, a->dash);
          }
          if(enable_layer[c]) for(i=0;i<xctx.polygons[c];i++) {
            xPoly *p = &xctx.poly[c][i];
            drawpolygon(c, NOW, p->x, p->y, p->points, p->fill, p->dash);
          }
          if(use_hash) {

            /* --------------------------------- inst_ptr iterator 20171224 */
            /*loop thru all squares that intersect drawing area */
            for(init_inst_iterator(x1, y1, x2, y2); ( instanceptr = inst_iterator_next() ) ;) {
              int ptr;
              i = instanceptr->n;
              ptr = xctx.inst[i].ptr;
              if( ptr !=-1) {
                symptr = ptr+xctx.sym;
                if( c==0 || /*20150408 draw_symbol call is needed on layer 0 to avoid redundant work (outside check) */
                    symptr->lines[c] ||
                    symptr->arcs[c] ||
                    symptr->rects[c] ||
                    symptr->polygons[c] ||
                    ((c==TEXTWIRELAYER || c==TEXTLAYER) && symptr->texts)) {

                  type = symptr->type;
                  if(!(
                       hilight_nets &&
                       type  &&
                       (
                        (
                            IS_LABEL_SH_OR_PIN(type) && xctx.inst[i].node && xctx.inst[i].node[0] &&
                            bus_hilight_lookup(xctx.inst[i].node[0], 0, XLOOKUP )
                        ) ||
                        (
                           !IS_LABEL_SH_OR_PIN(type) && (xctx.inst[i].flags & 4)
                        )
                       )
                      )
                  ) {
                    draw_symbol(ADD, c, instanceptr->n,c,0,0,0.0,0.0);
                  }
                }
              } /*if( ptr !=-1)  */
            }
            /* --------------------------------- /20171224 */
          } else {

            for(i=0;i<xctx.instances;i++) {
              if(xctx.inst[i].ptr == -1) continue;
              symptr = (xctx.inst[i].ptr+ xctx.sym);
              if( c==0 || /*20150408 draw_symbol call is needed on layer 0 to avoid redundant work (outside check) */
                  symptr->lines[c] ||
                  symptr->arcs[c] ||
                  symptr->rects[c] ||
                  symptr->polygons[c] ||
                  ((c==TEXTWIRELAYER || c==TEXTLAYER) && symptr->texts)) {


                type = (xctx.inst[i].ptr+ xctx.sym)->type;
                if(!(
                     hilight_nets &&
                     type  &&
                     (
                      (
                        IS_LABEL_SH_OR_PIN(type) && xctx.inst[i].node && xctx.inst[i].node[0] &&
                        bus_hilight_lookup(xctx.inst[i].node[0], 0, XLOOKUP )
                      ) ||
                      (
                        !IS_LABEL_SH_OR_PIN(type) && (xctx.inst[i].flags & 4)
                      )
                     )
                    )
                ) {
                  draw_symbol(ADD, c, i,c,0,0,0.0,0.0);
                }
              }
            }
          }

          filledrect(c, END, 0.0, 0.0, 0.0, 0.0);
          drawarc(c, END, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0);
          drawrect(c, END, 0.0, 0.0, 0.0, 0.0, 0);
          drawline(c, END, 0.0, 0.0, 0.0, 0.0, 0);
        }
        if(draw_single_layer==-1 || draw_single_layer==WIRELAYER){

          if(use_hash) {
            dbg(3, "using spatial hash table iterator\n");
            /*loop thru all squares that intersect drawing area */

            for(init_wire_iterator(x1, y1, x2, y2); ( wireptr = wire_iterator_next() ) ;) {
              ii=wireptr->n;
              if(xctx.wire[ii].bus) {
                drawline(WIRELAYER, THICK, xctx.wire[ii].x1,xctx.wire[ii].y1,xctx.wire[ii].x2,xctx.wire[ii].y2, 0);
              }
              else
                drawline(WIRELAYER, ADD, xctx.wire[ii].x1,xctx.wire[ii].y1,xctx.wire[ii].x2,xctx.wire[ii].y2, 0);
            }
          } else {
            for(i=0;i<xctx.wires;i++)
            {
              if(xctx.wire[i].bus) {
                drawline(WIRELAYER, THICK, xctx.wire[i].x1,xctx.wire[i].y1,xctx.wire[i].x2,xctx.wire[i].y2, 0);
              }
              else
                drawline(WIRELAYER, ADD, xctx.wire[i].x1,xctx.wire[i].y1,xctx.wire[i].x2,xctx.wire[i].y2, 0);
            }
          }
          update_conn_cues(1, draw_window);
          filledrect(WIRELAYER, END, 0.0, 0.0, 0.0, 0.0);
          drawline(WIRELAYER, END, 0.0, 0.0, 0.0, 0.0, 0);
        }
        if(draw_single_layer ==-1 || draw_single_layer==TEXTLAYER) {
          for(i=0;i<xctx.texts;i++)
          {
            textlayer = xctx.text[i].layer;
            if(textlayer < 0 ||  textlayer >= cadlayers) textlayer = TEXTLAYER;
            dbg(1, "draw(): drawing string %d = %s\n",i, xctx.text[i].txt_ptr);
            #ifdef HAS_CAIRO
            if(!enable_layer[textlayer]) continue;
            textfont = xctx.text[i].font;
            if( (textfont && textfont[0]) || xctx.text[i].flags) {
              cairo_font_slant_t slant;
              cairo_font_weight_t weight;
              textfont = (xctx.text[i].font && xctx.text[i].font[0]) ? xctx.text[i].font : cairo_font_name;
              weight = ( xctx.text[i].flags & TEXT_BOLD) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL;
              slant = CAIRO_FONT_SLANT_NORMAL;
              if(xctx.text[i].flags & TEXT_ITALIC) slant = CAIRO_FONT_SLANT_ITALIC;
              if(xctx.text[i].flags & TEXT_OBLIQUE) slant = CAIRO_FONT_SLANT_OBLIQUE;

              cairo_save(cairo_ctx);
              cairo_save(cairo_save_ctx);
              cairo_select_font_face (cairo_ctx, textfont, slant, weight);
              cairo_select_font_face (cairo_save_ctx, textfont, slant, weight);
            }
            #endif

            draw_string(textlayer, ADD, xctx.text[i].txt_ptr,
              xctx.text[i].rot, xctx.text[i].flip, xctx.text[i].hcenter, xctx.text[i].vcenter,
              xctx.text[i].x0,xctx.text[i].y0,
              xctx.text[i].xscale, xctx.text[i].yscale);
            #ifdef HAS_CAIRO
            if((textfont && textfont[0]) || xctx.text[i].flags ) {
              cairo_restore(cairo_ctx);
              cairo_restore(cairo_save_ctx);
            }
            #endif
            #ifndef HAS_CAIRO
            drawrect(textlayer, END, 0.0, 0.0, 0.0, 0.0, 0);
            drawline(textlayer, END, 0.0, 0.0, 0.0, 0.0, 0);
            #endif
          }
        }
    } /* !only_probes, 20110112 */
    draw_hilight_net(draw_window);
    if(!draw_window) {
      XCopyArea(display, save_pixmap, window, gctiled, xrect[0].x, xrect[0].y,
         xrect[0].width, xrect[0].height, xrect[0].x, xrect[0].y);
    }
    draw_selection(gc[SELLAYER], 0); /* 20181009 moved outside of cadlayers loop */

    dbg(1, "draw(): lw=%d\n",lw);
 } /* if(has_x) */
}

#ifndef __unix__
/* place holder for Windows to show that these XLib functions are not supported in Windows. */
int XSetClipRectangles(register Display* dpy, GC gc, int clip_x_origin, int clip_y_origin,
                       XRectangle* rectangles, int n, int ordering)
{
  return 0;
}
int XSetTile(Display* display, GC gc, Pixmap save_pixmap)
{
  return 0;
}
#endif
