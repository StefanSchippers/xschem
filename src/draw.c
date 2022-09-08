/* File: draw.c
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2022 Stefan Frederik Schippers
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
#if defined(HAS_CAIRO)
static void clear_cairo_surface(cairo_t *cr, double x, double y, double width, double height)
{
  cairo_save(cr);
  cairo_set_source_rgba(cr, 0, 0, 0, 0);
  cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_rectangle(cr, x, y, width, height);
  cairo_fill(cr);
  /*cairo_paint(cr);
  cairo_set_operator(cr, CAIRO_OPERATOR_OVER);*/
  cairo_restore(cr);
}

static void my_cairo_fill(cairo_surface_t *src_surface, int x, int y, unsigned int width, unsigned int height)
{
  HWND hwnd = Tk_GetHWND(xctx->window);
  HDC dc = GetDC(hwnd);
  cairo_surface_t *dest_surface = cairo_win32_surface_create(dc);
  if (cairo_surface_status(dest_surface) != CAIRO_STATUS_SUCCESS) {
    fprintf(errfp, "ERROR: invalid cairo surface to copy over\n");
  }
  cairo_t *ct = cairo_create(dest_surface);
  cairo_surface_flush(src_surface);
  cairo_set_source_surface(ct, src_surface, 0, 0);
  cairo_rectangle(ct, x, y, width, height);
  cairo_set_operator(ct, CAIRO_OPERATOR_ADD);
  cairo_fill(ct);
  cairo_destroy(ct); ct = NULL;
  cairo_surface_destroy(dest_surface); dest_surface = NULL;
}
#endif
#endif

int textclip(int x1,int y1,int x2,int y2,
          double xa,double ya,double xb,double yb)
/* check if some of (xa,ya-xb,yb) is inside (x1,y1-x2,y2) */
/* coordinates should be ordered, x1<x2,ya<yb and so on... */
{
 dbg(2, "textclip(): %.16g %.16g %.16g %.16g - %d %d %d %d\n",
 X_TO_SCREEN(xa),Y_TO_SCREEN(ya), X_TO_SCREEN(xb),Y_TO_SCREEN(yb),x1,y1,x2,y2);
 /* drawtemprect(xctx->gc[WIRELAYER],xa,ya,xb,yb); */
 if          (X_TO_SCREEN(xa)>x2) return 0;
 else if     (Y_TO_SCREEN(ya)>y2) return 0;
 else if     (X_TO_SCREEN(xb)<x1) return 0;
 else if     (Y_TO_SCREEN(yb)<y1) return 0;
 return 1;
}

void print_image()
{
  #if HAS_CAIRO == 0
  char cmd[PATH_MAX+100];
  #endif
  int save_draw_grid;
  static char lastdir[PATH_MAX] = "";
  const char *r;

  if(!has_x) return ;
  if(!lastdir[0]) my_strncpy(lastdir, pwd_dir, S(lastdir));
  if(!xctx->plotfile[0]) {
    tclvareval("tk_getSaveFile -title {Select destination file} -initialfile {",
      get_cell(xctx->sch[xctx->currsch], 0) , ".png} -initialdir {", lastdir, "}", NULL);
    r = tclresult();
    if(r[0]) {
      my_strncpy(xctx->plotfile, r, S(xctx->plotfile));
      tclvareval("file dirname {", xctx->plotfile, "}", NULL);
      my_strncpy(lastdir, tclresult(), S(lastdir));
    }
    else return;
  }
  #if 0
  *  for(tmp=0;tmp<cadlayers;tmp++) {
  *    XSetClipRectangles(display, xctx->gc[tmp], 0,0, xctx->xrect, 1, Unsorted);
  *    XSetClipRectangles(display, xctx->gcstipple[tmp], 0,0, xctx->xrect, 1, Unsorted);
  *  }
  *  XSetClipRectangles(display, xctx->gctiled, 0,0, xctx->xrect, 1, Unsorted);
  #endif
  save_draw_grid = tclgetboolvar("draw_grid");
  tclsetvar("draw_grid", "0");
  xctx->draw_pixmap=1;
  draw();
  

  #if HAS_CAIRO == 1 /* use cairo native support for png writing, no need to convert
                      * XPM and handles Xrender extensions for transparent embedded images */
  {
    cairo_surface_t *png_sfc;
#ifdef __unix__
    png_sfc = cairo_xlib_surface_create(display, xctx->save_pixmap, visual,
               xctx->xrect[0].width, xctx->xrect[0].height);
#else
    HWND hwnd = Tk_GetHWND(xctx->window);
    HDC dc = GetDC(hwnd);
    png_sfc = cairo_win32_surface_create(dc);
#endif

    if(xctx->plotfile[0])
      cairo_surface_write_to_png(png_sfc, xctx->plotfile);
    else
      cairo_surface_write_to_png(png_sfc, "plot.png");
  }
  #else /* no cairo */
#ifdef __unix__
  XpmWriteFileFromPixmap(display, "plot.xpm", xctx->save_pixmap,0, NULL ); /* .gz ???? */
  dbg(1, "print_image(): Window image saved\n");
  if(xctx->plotfile[0]) {
    my_snprintf(cmd, S(cmd), "convert_to_png plot.xpm {%s}", xctx->plotfile);
    tcleval(cmd);
  } else tcleval( "convert_to_png plot.xpm plot.png");
#else
  char *psfile = NULL;
  create_ps(&psfile, 7);
  if (xctx->plotfile[0]) {
    my_snprintf(cmd, S(cmd), "convert_to_png {%s} {%s}", psfile, xctx->plotfile);
    tcleval(cmd);
  }
  else tcleval("convert_to_png {%s} plot.png", psfile);
#endif
  #endif
  my_strncpy(xctx->plotfile,"", S(xctx->plotfile));
  tclsetboolvar("draw_grid", save_draw_grid);
  xctx->draw_pixmap=1;
}

#if HAS_CAIRO==1
static void set_cairo_color(int layer)
{
  cairo_set_source_rgb(xctx->cairo_ctx,
    (double)xctx->xcolor_array[layer].red/65535.0,
    (double)xctx->xcolor_array[layer].green/65535.0,
    (double)xctx->xcolor_array[layer].blue/65535.0);
  cairo_set_source_rgb(xctx->cairo_save_ctx,
    (double)xctx->xcolor_array[layer].red/65535.0,
    (double)xctx->xcolor_array[layer].green/65535.0,
    (double)xctx->xcolor_array[layer].blue/65535.0);
}

/* remember to call cairo_restore(xctx->cairo_ctx) when done !! */
int set_text_custom_font(xText *txt) /* 20171122 for correct text_bbox calculation */
{
  const char *textfont;
  if (xctx->cairo_ctx==NULL) return 0;
  textfont = txt->font;
  if((textfont && textfont[0]) || (txt->flags & (TEXT_BOLD | TEXT_OBLIQUE | TEXT_ITALIC))) {
    cairo_font_slant_t slant;
    cairo_font_weight_t weight;
    textfont = (txt->font && txt->font[0]) ? txt->font : tclgetvar("cairo_font_name");
    weight = ( txt->flags & TEXT_BOLD) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL;
    slant = CAIRO_FONT_SLANT_NORMAL;
    if(txt->flags & TEXT_ITALIC) slant = CAIRO_FONT_SLANT_ITALIC;
    if(txt->flags & TEXT_OBLIQUE) slant = CAIRO_FONT_SLANT_OBLIQUE;
    cairo_save(xctx->cairo_ctx);
    cairo_select_font_face (xctx->cairo_ctx, textfont, slant, weight);
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


#if HAS_CAIRO==1
static void cairo_draw_string_line(cairo_t *c_ctx, char *s,
    double x, double y, short rot, short flip,
    int lineno, double fontheight, double fontascent, double fontdescent,
    int llength, int no_of_lines, double longest_line)
{
  double ix, iy;
  short rot1;
  double line_delta;
  double lines;
  double vc; /* 20171121 vert correct */
  
  if(s==NULL) return;
  if(llength==0) return;

  line_delta = (lineno*fontheight*cairo_font_line_spacing);
  lines = (no_of_lines-1)*fontheight*cairo_font_line_spacing;

  ix=X_TO_SCREEN(x);
  iy=Y_TO_SCREEN(y);
  if(rot&1) {
    rot1=3;
  } else rot1=0;

  vc = cairo_vert_correct*xctx->mooz; /* converted to device (pixel) space */

  if(     rot==0 && flip==0) {iy+=line_delta+fontascent-vc;}
  else if(rot==1 && flip==0) {iy+=longest_line;ix=ix-fontheight+fontascent+vc-lines+line_delta;}
  else if(rot==2 && flip==0) {iy=iy-fontheight-lines+line_delta+fontascent+vc; ix=ix-longest_line;}
  else if(rot==3 && flip==0) {ix+=line_delta+fontascent-vc;}
  else if(rot==0 && flip==1) {ix=ix-longest_line;iy+=line_delta+fontascent-vc;}
  else if(rot==1 && flip==1) {ix=ix-fontheight+line_delta-lines+fontascent+vc;}
  else if(rot==2 && flip==1) {iy=iy-fontheight-lines+line_delta+fontascent+vc;}
  else if(rot==3 && flip==1) {iy=iy+longest_line;ix+=line_delta+fontascent-vc;}

  cairo_save(c_ctx);
  cairo_translate(c_ctx, ix, iy);
  cairo_rotate(c_ctx, XSCH_PI/2*rot1);

  cairo_move_to(c_ctx, 0,0);
  cairo_show_text(c_ctx, s);
  cairo_restore(c_ctx);
}

/* CAIRO version */
void draw_string(int layer, int what, const char *str, short rot, short flip, int hcenter, int vcenter,
                 double x, double y, double xscale, double yscale)
{
  double textx1,textx2,texty1,texty2;
  char *tt, *ss, *sss=NULL;
  char c;
  int lineno=0;
  double size;
  cairo_font_extents_t fext;
  int llength=0, no_of_lines;
  double longest_line;

  (void)what; /* UNUSED in cairo version, avoid compiler warning */
  if(str==NULL || !has_x ) return;
  size = xscale*52.*cairo_font_scale;
  /*fprintf(errfp, "size=%.16g\n", size*xctx->mooz); */
  if(size*xctx->mooz<3.0) return; /* too small */
  if(size*xctx->mooz>1600) return; /* too big */

  text_bbox(str, xscale, yscale, rot, flip, hcenter, vcenter, x,y, 
            &textx1,&texty1,&textx2,&texty2, &no_of_lines, &longest_line);
  if(!textclip(xctx->areax1,xctx->areay1,xctx->areax2,
               xctx->areay2,textx1,texty1,textx2,texty2)) {
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

  set_cairo_color(layer);
  cairo_set_font_size (xctx->cairo_ctx, size*xctx->mooz);
  cairo_set_font_size (xctx->cairo_save_ctx, size*xctx->mooz);
  cairo_font_extents(xctx->cairo_ctx, &fext);
  dbg(1, "draw_string(): size * mooz=%g height=%g ascent=%g descent=%g\n",
       size * xctx->mooz, fext.height, fext.ascent, fext.descent);
  llength=0;
  my_strdup2(73, &sss, str);
  tt=ss=sss;
  for(;;) {
    c=*ss;
    if(c=='\n' || c==0) {
      *ss='\0';
      /*fprintf(errfp, "cairo_draw_string(): tt=%s, longest line: %d\n", tt, longest_line); */
      if(xctx->draw_window) cairo_draw_string_line(xctx->cairo_ctx, tt, x, y, rot, flip,
         lineno, fext.height, fext.ascent, fext.descent, llength, no_of_lines, longest_line);
      if(xctx->draw_pixmap) cairo_draw_string_line(xctx->cairo_save_ctx, tt, x, y, rot, flip,
         lineno, fext.height, fext.ascent, fext.descent, llength, no_of_lines, longest_line);
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
void draw_string(int layer, int what, const char *str, short rot, short flip, int hcenter, int vcenter,
                 double x1,double y1, double xscale, double yscale)
{
 double textx1,textx2,texty1,texty2;
 double a=0.0,yy;
 register double rx1=0,rx2=0,ry1=0,ry2=0;
 double curr_x1,curr_y1,curr_x2,curr_y2;
 double zx1, invxscale;
 int pos=0,pos2=0;
 unsigned int cc;
 double *char_ptr_x1,*char_ptr_y1,*char_ptr_x2,*char_ptr_y2;
 int i,lines, no_of_lines;
 double longest_line;

 if(str==NULL || !has_x ) return;
 dbg(2, "draw_string(): string=%s\n",str);
 if(xscale*FONTWIDTH*xctx->mooz<1) {
   dbg(1, "draw_string(): xscale=%.16g zoom=%.16g \n",xscale,xctx->zoom);
   return;
 }
 else {
  text_bbox(str, xscale, yscale, rot, flip, hcenter, vcenter, x1,y1,
            &textx1,&texty1,&textx2,&texty2, &no_of_lines, &longest_line);
  xscale*=tclgetdoublevar("nocairo_font_xscale");
  yscale*=tclgetdoublevar("nocairo_font_yscale");
  if(!textclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,
               textx1,texty1,textx2,texty2)) return;
  x1=textx1;y1=texty1;
  if(rot&1) {y1=texty2;rot=3;}
  else rot=0;
  flip = 0; yy=y1;
  invxscale=1/xscale;
  while(str[pos2]) {
     cc = (unsigned char)str[pos2++];
     if(cc>127) cc= '?';
     if(cc=='\n') {
        yy+=(FONTHEIGHT+FONTDESCENT+FONTWHITESPACE)* yscale;
        pos=0;
        a=0.0;
        continue;
     }
     lines=(int)character[cc][0]*4;
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
        ROTATION(rot, flip, x1,y1,curr_x1,curr_y1,rx1,ry1);
        ROTATION(rot, flip, x1,y1,curr_x2,curr_y2,rx2,ry2);
        ORDER(rx1,ry1,rx2,ry2);
        drawline(layer, what, rx1, ry1, rx2, ry2, 0);
     }
     pos++;
     a += FONTWIDTH+FONTWHITESPACE;
  }
 }
}

#endif /* HAS_CAIRO */

void draw_temp_string(GC gctext, int what, const char *str, short rot, short flip, int hcenter, int vcenter,
                 double x1,double y1, double xscale, double yscale)
{
 double textx1,textx2,texty1,texty2;
 int tmp;
 double dtmp;
 if(!has_x) return;
 dbg(2, "draw_string(): string=%s\n",str);
 if(!text_bbox(str, xscale, yscale, rot, flip, hcenter, vcenter, x1,y1,
     &textx1,&texty1,&textx2,&texty2, &tmp, &dtmp)) return;
 drawtemprect(gctext,what, textx1,texty1,textx2,texty2);
}


/* 
 * layer: the set of symbol objects on xschem layer 'layer' to draw
 * c    : the layer 'c' to draw those objects on (if != layer it is the hilight color)
 */
void draw_symbol(int what,int c, int n,int layer,short tmp_flip, short rot,
        double xoffset, double yoffset)
                            /* draws current layer only, should be called within  */
{                           /* a "for(i=0;i<cadlayers;i++)" loop */
  int k, j, textlayer, hide = 0;
  double x0,y0,x1,y1,x2,y2;
  double *x, *y; /* polygon point arrays */
  short flip;
  xLine *line;
  xRect *rect;
  xArc *arc;
  xPoly *polygon;
  xText text;
  register xSymbol *symptr;
  double angle;
  char *type;
  #if HAS_CAIRO==1
  const char *textfont;
  #endif
  int show_hidden_texts = tclgetboolvar("show_hidden_texts");

  if(xctx->inst[n].ptr == -1) return;
  if( (layer != PINLAYER && !xctx->enable_layer[layer]) ) return;
  if(!has_x) return;
  if( (xctx->hide_symbols==1 && (xctx->inst[n].ptr+ xctx->sym)->prop_ptr &&
      !strcmp( (xctx->inst[n].ptr+ xctx->sym)->type, "subcircuit") ) || (xctx->hide_symbols == 2) ) {
    hide = 1;
  } else {
    hide = 0;
  }
  type = (xctx->inst[n].ptr+ xctx->sym)->type;
  if(layer==0) {
    x1=X_TO_SCREEN(xctx->inst[n].x1+xoffset);  /* 20150729 added xoffset, yoffset */
    x2=X_TO_SCREEN(xctx->inst[n].x2+xoffset);
    y1=Y_TO_SCREEN(xctx->inst[n].y1+yoffset);
    y2=Y_TO_SCREEN(xctx->inst[n].y2+yoffset);
    if(OUTSIDE(x1,y1,x2,y2,xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2))
    {
     xctx->inst[n].flags|=1;
     return;
    }
    else if(
         xctx->hilight_nets &&                  /* if highlights...                       */
         c == 0 &&                              /* we are not drawing highlighted inst    */
                                                /* otherwise c > layer...                 */
         type  &&                               /* ... and type...                        */
         (
          (                                     /* ... and inst is hilighted ...          */
            IS_LABEL_SH_OR_PIN(type) && xctx->inst[n].node && xctx->inst[n].node[0] &&
            bus_hilight_hash_lookup(xctx->inst[n].node[0], 0, XLOOKUP )
          ) || ( !IS_LABEL_SH_OR_PIN(type) && (xctx->inst[n].color != -10000)) )) {
      xctx->inst[n].flags|=1;                    /* ... then SKIP instance now and for following layers */
      return;
    }
    else if(!xctx->only_probes && (xctx->inst[n].x2 - xctx->inst[n].x1) * xctx->mooz < 3 &&
                       (xctx->inst[n].y2 - xctx->inst[n].y1) * xctx->mooz < 3) {
      drawrect(SYMLAYER, NOW, xctx->inst[n].xx1, xctx->inst[n].yy1, xctx->inst[n].xx2, xctx->inst[n].yy2, 0);
      xctx->inst[n].flags|=1;
      return;
    }
    else {
      xctx->inst[n].flags&=~1;
    }
    if(hide) drawrect(SYMLAYER, NOW, xctx->inst[n].xx1, xctx->inst[n].yy1, xctx->inst[n].xx2, xctx->inst[n].yy2, 2);
  } else if(xctx->inst[n].flags&1) {
    dbg(2, "draw_symbol(): skipping inst %d\n", n);
    return;
  }

  flip = xctx->inst[n].flip;
  if(tmp_flip) flip = !flip;
  rot = (xctx->inst[n].rot + rot ) & 0x3;

  x0=xctx->inst[n].x0 + xoffset;
  y0=xctx->inst[n].y0 + yoffset;
  symptr = (xctx->inst[n].ptr+ xctx->sym);
  if(!hide) {
    for(j=0;j< symptr->lines[layer];j++)
    {
      line = &(symptr->line[layer])[j];
      ROTATION(rot, flip, 0.0,0.0,line->x1,line->y1,x1,y1);
      ROTATION(rot, flip, 0.0,0.0,line->x2,line->y2,x2,y2);
      ORDER(x1,y1,x2,y2);
      if(line->bus)
        drawline(c,THICK, x0+x1, y0+y1, x0+x2, y0+y2, line->dash);
      else
        drawline(c,what, x0+x1, y0+y1, x0+x2, y0+y2, line->dash);
    }
    for(j=0;j< symptr->polygons[layer];j++)
    {
      polygon = &(symptr->poly[layer])[j];
      x = my_malloc(34, sizeof(double) * polygon->points);
      y = my_malloc(35, sizeof(double) * polygon->points);
      for(k=0;k<polygon->points;k++) {
        ROTATION(rot, flip, 0.0,0.0,polygon->x[k],polygon->y[k],x[k],y[k]);
        x[k]+= x0;
        y[k] += y0;
      }
      drawpolygon(c, NOW, x, y, polygon->points, polygon->fill, polygon->dash); /* added fill */
      my_free(718, &x);
      my_free(719, &y);
    }
    for(j=0;j< symptr->arcs[layer];j++)
    {
  
      arc = &(symptr->arc[layer])[j];
      if(flip) {
        angle = 270.*rot+180.-arc->b-arc->a;
      } else {
        angle = arc->a+rot*270.;
      }
      angle = fmod(angle, 360.);
      if(angle<0.) angle+=360.;
      ROTATION(rot, flip, 0.0,0.0,arc->x,arc->y,x1,y1);
      drawarc(c,what, x0+x1, y0+y1, arc->r, angle, arc->b, arc->fill, arc->dash);
    }
  } /* if(!hide) */

  if( (!hide && xctx->enable_layer[layer])  ||
      (hide && layer == PINLAYER && xctx->enable_layer[layer]) ) {
    for(j=0;j< symptr->rects[layer];j++)
    {
      rect = &(symptr->rect[layer])[j];
      ROTATION(rot, flip, 0.0,0.0,rect->x1,rect->y1,x1,y1);
      ROTATION(rot, flip, 0.0,0.0,rect->x2,rect->y2,x2,y2);
      #if HAS_CAIRO == 1
      if(layer == GRIDLAYER && rect->flags & 1024) {
        double xx1 = x0 + x1;
        double yy1 = y0 + y1;
        double xx2 = x0 + x2;
        double yy2 = y0 + y2;
        draw_image(1, rect, &xx1, &yy1, &xx2, &yy2, rot, flip);
      } else 
      #endif
      {
        RECTORDER(x1,y1,x2,y2);
        drawrect(c,what, x0+x1, y0+y1, x0+x2, y0+y2, rect->dash);
        filledrect(c,what, x0+x1, y0+y1, x0+x2, y0+y2);
      }
    }
  }
  if( (layer==TEXTWIRELAYER && !(xctx->inst[n].flags&2) ) ||
      (xctx->sym_txt && (layer==TEXTLAYER) && (xctx->inst[n].flags&2) ) ) {
    const char *txtptr;
    for(j=0;j< symptr->texts;j++)
    {
      text = symptr->text[j];
      if(text.xscale*FONTWIDTH*xctx->mooz<1) continue;
      if(!show_hidden_texts && (symptr->text[j].flags & HIDE_TEXT)) continue;
      if( hide && text.txt_ptr && strcmp(text.txt_ptr, "@symname") && strcmp(text.txt_ptr, "@name") ) continue;
      txtptr= translate(n, text.txt_ptr);
      ROTATION(rot, flip, 0.0,0.0,text.x0,text.y0,x1,y1);
      textlayer = c;
      /* do not allow custom text color on PINLAYER hilighted instances */
      if( !(xctx->inst[n].color == -PINLAYER)) {
        textlayer = symptr->text[j].layer;
        if(textlayer < 0 || textlayer >= cadlayers) textlayer = c;
      }
      /* display PINLAYER colored instance texts even if PINLAYER disabled */
      if(xctx->inst[n].color == -PINLAYER || xctx->enable_layer[textlayer]) {
        #if HAS_CAIRO==1
        textfont = symptr->text[j].font;
        if((textfont && textfont[0]) || (symptr->text[j].flags & (TEXT_BOLD | TEXT_OBLIQUE | TEXT_ITALIC))) {
          cairo_font_slant_t slant;
          cairo_font_weight_t weight;
          textfont = (symptr->text[j].font && symptr->text[j].font[0]) ?
            symptr->text[j].font : tclgetvar("cairo_font_name");
          weight = ( symptr->text[j].flags & TEXT_BOLD) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL;
          slant = CAIRO_FONT_SLANT_NORMAL;
          if(symptr->text[j].flags & TEXT_ITALIC) slant = CAIRO_FONT_SLANT_ITALIC;
          if(symptr->text[j].flags & TEXT_OBLIQUE) slant = CAIRO_FONT_SLANT_OBLIQUE;
          cairo_save(xctx->cairo_ctx);
          cairo_save(xctx->cairo_save_ctx);
          cairo_select_font_face (xctx->cairo_ctx, textfont, slant, weight);
          cairo_select_font_face (xctx->cairo_save_ctx, textfont, slant, weight);
        }
        #endif
        dbg(1, "drawing string: str=%s prop=%s\n", txtptr, text.prop_ptr);
        draw_string(textlayer, what, txtptr,
          (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
          flip^text.flip, text.hcenter, text.vcenter,
          x0+x1, y0+y1, text.xscale, text.yscale);
        #if HAS_CAIRO!=1
        drawrect(textlayer, END, 0.0, 0.0, 0.0, 0.0, 0);
        drawline(textlayer, END, 0.0, 0.0, 0.0, 0.0, 0);
        #endif
        #if HAS_CAIRO==1
        if( (textfont && textfont[0]) || (symptr->text[j].flags & (TEXT_BOLD | TEXT_OBLIQUE | TEXT_ITALIC))) {
          cairo_restore(xctx->cairo_ctx);
          cairo_restore(xctx->cairo_save_ctx);
        }
        #endif
      }
    }
  }
}

void draw_temp_symbol(int what, GC gc, int n,int layer,short tmp_flip, short rot,
        double xoffset, double yoffset)
                            /* draws current layer only, should be called within */
{                           /* a "for(i=0;i<cadlayers;i++)" loop */
 int j;
 double x0,y0,x1,y1,x2,y2;
 short flip;
 xLine *line;
 xPoly *polygon;
 xRect *rect;
 xArc *arc;
 xText text;
 register xSymbol *symptr;
 double angle;

 #if HAS_CAIRO==1
 int customfont;
 #endif

 if(xctx->inst[n].ptr == -1) return;
 if(!has_x) return;

 flip = xctx->inst[n].flip;
 if(tmp_flip) flip = !flip;
 rot = (xctx->inst[n].rot + rot ) & 0x3;

 if(layer==0) {
   x1=X_TO_SCREEN(xctx->inst[n].x1+xoffset); /* 20150729 added xoffset, yoffset */
   x2=X_TO_SCREEN(xctx->inst[n].x2+xoffset);
   y1=Y_TO_SCREEN(xctx->inst[n].y1+yoffset);
   y2=Y_TO_SCREEN (xctx->inst[n].y2+yoffset);
   if(OUTSIDE(x1,y1,x2,y2,xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2))
   {
    xctx->inst[n].flags|=1;
    return;
   }
   else if(!xctx->only_probes && (xctx->inst[n].x2 - xctx->inst[n].x1) * xctx->mooz < 3 &&
                      (xctx->inst[n].y2 - xctx->inst[n].y1) * xctx->mooz < 3) {
     drawtemprect(gc, what, xctx->inst[n].xx1 + xoffset, xctx->inst[n].yy1 + yoffset, 
                            xctx->inst[n].xx2 + xoffset, xctx->inst[n].yy2 + yoffset);
     xctx->inst[n].flags|=1;
     return;
   }
   else xctx->inst[n].flags&=~1;
 } else if(xctx->inst[n].flags&1) {
   dbg(2, "draw_symbol(): skipping inst %d\n", n);
   return;
 } /* /20150424 */

 x0=xctx->inst[n].x0 + xoffset;
 y0=xctx->inst[n].y0 + yoffset;
 symptr = (xctx->inst[n].ptr+ xctx->sym);
 for(j=0;j< symptr->lines[layer];j++)
 {
  line = &(symptr->line[layer])[j];
  ROTATION(rot, flip, 0.0,0.0,line->x1,line->y1,x1,y1);
  ROTATION(rot, flip, 0.0,0.0,line->x2,line->y2,x2,y2);
  ORDER(x1,y1,x2,y2);
  if(line->bus)
    drawtempline(gc,THICK, x0+x1, y0+y1, x0+x2, y0+y2);
  else
    drawtempline(gc,what, x0+x1, y0+y1, x0+x2, y0+y2);
 }
 for(j=0;j< symptr->polygons[layer];j++)
 {
   polygon = &(symptr->poly[layer])[j];

   {   /* scope block so we declare some auxiliary arrays for coord transforms. 20171115 */
     int k;
     double *x = my_malloc(36, sizeof(double) * polygon->points);
     double *y = my_malloc(37, sizeof(double) * polygon->points);
     for(k=0;k<polygon->points;k++) {
       ROTATION(rot, flip, 0.0,0.0,polygon->x[k],polygon->y[k],x[k],y[k]);
       x[k] += x0;
       y[k] += y0;
     }
     drawtemppolygon(gc, NOW, x, y, polygon->points);
     my_free(720, &x);
     my_free(721, &y);
   }
 }

 for(j=0;j< symptr->rects[layer];j++)
 {
  rect = &(symptr->rect[layer])[j];
  ROTATION(rot, flip, 0.0,0.0,rect->x1,rect->y1,x1,y1);
  ROTATION(rot, flip, 0.0,0.0,rect->x2,rect->y2,x2,y2);
  RECTORDER(x1,y1,x2,y2);
  drawtemprect(gc,what, x0+x1, y0+y1, x0+x2, y0+y2);
 }
 for(j=0;j< symptr->arcs[layer];j++)
 {
   arc = &(symptr->arc[layer])[j];
   if(flip) {
     angle = 270.*rot+180.-arc->b-arc->a;
   } else {
     angle = arc->a+rot*270.;
   }
   angle = fmod(angle, 360.);
   if(angle<0.) angle+=360.;
   ROTATION(rot, flip, 0.0,0.0,arc->x,arc->y,x1,y1);
   drawtemparc(gc, what, x0+x1, y0+y1, arc->r, angle, arc->b);
 }

 if(layer==PROPERTYLAYER && xctx->sym_txt)
 {
  const char *txtptr;
  for(j=0;j< symptr->texts;j++)
  {
   text = symptr->text[j];
   if(text.xscale*FONTWIDTH*xctx->mooz<1) continue;
   txtptr= translate(n, text.txt_ptr);
   ROTATION(rot, flip, 0.0,0.0,text.x0,text.y0,x1,y1);
   #if HAS_CAIRO==1
   customfont = set_text_custom_font(&text);
   #endif
   if(txtptr[0]) draw_temp_string(gc, what, txtptr,
     (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
     flip^text.flip, text.hcenter, text.vcenter, x0+x1, y0+y1, text.xscale, text.yscale);
   #if HAS_CAIRO==1
   if(customfont) cairo_restore(xctx->cairo_ctx);
   #endif

  }
 }
}

static void drawgrid()
{
  double x,y;
  double delta,tmp;
  int i=0;
  int big_gr;
  
  dbg(1, "drawgrid(): draw grid\n");
  big_gr = tclgetboolvar("big_grid_points");
  if( !tclgetboolvar("draw_grid") || !has_x) return;
  delta=tclgetdoublevar("cadgrid")*xctx->mooz;
  while(delta < CADGRIDTHRESHOLD) delta*=CADGRIDMULTIPLY;  /* <-- to be improved,but works */
  x = xctx->xorigin*xctx->mooz; y = xctx->yorigin*xctx->mooz;
  if(y > xctx->areay1 && y < xctx->areay2) {
    if(xctx->draw_window)
      XDrawLine(display, xctx->window, xctx->gc[GRIDLAYER],xctx->areax1+1,(int)y, xctx->areax2-1, (int)y);
    if(xctx->draw_pixmap)
      XDrawLine(display, xctx->save_pixmap, xctx->gc[GRIDLAYER],xctx->areax1+1,(int)y, xctx->areax2-1, (int)y);
  }
  if(x > xctx->areax1 && x < xctx->areax2) {
    if(xctx->draw_window)
      XDrawLine(display, xctx->window, xctx->gc[GRIDLAYER],(int)x,xctx->areay1+1, (int)x, xctx->areay2-1);
    if(xctx->draw_pixmap)
      XDrawLine(display, xctx->save_pixmap, xctx->gc[GRIDLAYER],(int)x,xctx->areay1+1, (int)x, xctx->areay2-1);
  }
  tmp = floor((xctx->areay1+1)/delta)*delta-fmod(-xctx->yorigin*xctx->mooz,delta);
  for(x=floor((xctx->areax1+1)/delta)*delta-fmod(-xctx->xorigin*xctx->mooz,delta); x < xctx->areax2; x += delta) {
    for(y=tmp; y < xctx->areay2; y += delta) {
      if(i>=CADMAXGRIDPOINTS) {
        if(xctx->draw_window) {
          if(big_gr) {
            XDrawSegments(display,xctx->window,xctx->gc[GRIDLAYER],xctx->biggridpoint,i);
          } else {
            XDrawPoints(display,xctx->window,xctx->gc[GRIDLAYER],xctx->gridpoint,i,CoordModeOrigin);
          }
        }
        if(xctx->draw_pixmap) {
          if(big_gr) {
            XDrawSegments(display,xctx->save_pixmap,xctx->gc[GRIDLAYER],xctx->biggridpoint,i);
          } else {
            XDrawPoints(display,xctx->save_pixmap,xctx->gc[GRIDLAYER],xctx->gridpoint,i,CoordModeOrigin);
          }
        }
        i=0;
      }
      if(big_gr) {
        xctx->biggridpoint[i].x1 = xctx->biggridpoint[i].x2 = (short)(x);
        xctx->biggridpoint[i].y1 =  xctx->biggridpoint[i].y2 = (short)(y);
        i++;
      } else {
        xctx->gridpoint[i].x=(short)(x);
        xctx->gridpoint[i].y=(short)(y);
        i++;
      }
    }
  }
  if(xctx->draw_window) {
    if(big_gr) {
      XDrawSegments(display,xctx->window,xctx->gc[GRIDLAYER],xctx->biggridpoint,i);
    } else {
      XDrawPoints(display,xctx->window,xctx->gc[GRIDLAYER],xctx->gridpoint,i,CoordModeOrigin);
    }
  }
  if(xctx->draw_pixmap) {
    if(big_gr) {
      XDrawSegments(display,xctx->save_pixmap,xctx->gc[GRIDLAYER],xctx->biggridpoint,i);
    } else {
      XDrawPoints(display,xctx->save_pixmap,xctx->gc[GRIDLAYER],xctx->gridpoint,i,CoordModeOrigin);
    }
  }
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
   if(xctx->draw_window) XDrawSegments(display, xctx->window, xctx->gc[c], rr,i);
   if(xctx->draw_pixmap)
     XDrawSegments(display, xctx->save_pixmap, xctx->gc[c], rr,i);
#else
    for (j = 0; j < i; ++j) {
      if (xctx->draw_window)
        XDrawLine(display, xctx->window, xctx->gc[c], rr[j].x1, rr[j].y1, rr[j].x2, rr[j].y2);
      if (xctx->draw_pixmap)
        XDrawLine(display, xctx->save_pixmap, xctx->gc[c], rr[j].x1, rr[j].y1, rr[j].x2, rr[j].y2);
    }
#endif
   i=0;
  }
  x1=X_TO_SCREEN(linex1);
  y1=Y_TO_SCREEN(liney1);
  x2=X_TO_SCREEN(linex2);
  y2=Y_TO_SCREEN(liney2);
  /* if(!xctx->only_probes && (x2-x1)< 3.0 && fabs(y2-y1) < 3.0) return; */
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
  /* if(!xctx->only_probes && (x2-x1)< 3.0 && fabs(y2-y1)< 3.0) return; */
  if( clip(&x1,&y1,&x2,&y2) )
  {
   if(dash) {
     dash_arr[0] = dash_arr[1] = (char) dash;
     XSetDashes(display, xctx->gc[c], 0, dash_arr, 2);
     XSetLineAttributes (display, xctx->gc[c], INT_WIDTH(xctx->lw), xDashType, CapButt, JoinBevel);
   }
   if(xctx->draw_window) XDrawLine(display, xctx->window, xctx->gc[c], (int)x1, (int)y1, (int)x2, (int)y2);
   if(xctx->draw_pixmap)
    XDrawLine(display, xctx->save_pixmap, xctx->gc[c], (int)x1, (int)y1, (int)x2, (int)y2);
   if(dash) {
     XSetLineAttributes (display, xctx->gc[c], INT_WIDTH(xctx->lw), LineSolid, CapRound, JoinRound);
   }
  }
 }

 else if(what & THICK)
 {
  x1=X_TO_SCREEN(linex1);
  y1=Y_TO_SCREEN(liney1);
  x2=X_TO_SCREEN(linex2);
  y2=Y_TO_SCREEN(liney2);
  /* if(!xctx->only_probes && (x2-x1)< 3.0 && fabs(y2-y1)< 3.0) return; */
  if( clip(&x1,&y1,&x2,&y2) )
  {
   if(dash) {
     dash_arr[0] = dash_arr[1] = (char) dash;
     XSetDashes(display, xctx->gc[c], 0, dash_arr, 2);
     XSetLineAttributes (display, xctx->gc[c], INT_BUS_WIDTH(xctx->lw), xDashType, CapButt, JoinBevel);
   } else {
     XSetLineAttributes (display, xctx->gc[c], INT_BUS_WIDTH(xctx->lw), LineSolid, CapRound, JoinRound);
   }
   if(xctx->draw_window) XDrawLine(display, xctx->window, xctx->gc[c], (int)x1, (int)y1, (int)x2, (int)y2);
   if(xctx->draw_pixmap) XDrawLine(display, xctx->save_pixmap, xctx->gc[c], (int)x1, (int)y1, (int)x2, (int)y2);
   XSetLineAttributes (display, xctx->gc[c], INT_WIDTH(xctx->lw), LineSolid, CapRound , JoinRound);
  }
 }
 else if((what & END) && i)
 {
#ifdef __unix__
  if(xctx->draw_window) XDrawSegments(display, xctx->window, xctx->gc[c], rr,i);
  if(xctx->draw_pixmap) XDrawSegments(display, xctx->save_pixmap, xctx->gc[c], rr,i);
#else
   for (j = 0; j < i; ++j) {
     if (xctx->draw_window)
       XDrawLine(display, xctx->window, xctx->gc[c], rr[j].x1, rr[j].y1, rr[j].x2, rr[j].y2);
     if (xctx->draw_pixmap)
       XDrawLine(display, xctx->save_pixmap, xctx->gc[c], rr[j].x1, rr[j].y1, rr[j].x2, rr[j].y2);
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
   XDrawSegments(display, xctx->window, gc, r,i);
#else
    for (j = 0; j < i; ++j) {
        XDrawLine(display, xctx->window, gc, r[j].x1, r[j].y1, r[j].x2, r[j].y2);
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
   XDrawLine(display, xctx->window, gc, (int)x1, (int)y1, (int)x2, (int)y2);
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
   XSetLineAttributes (display, gc, INT_BUS_WIDTH(xctx->lw), LineSolid, CapRound , JoinRound);

   XDrawLine(display, xctx->window, gc, (int)x1, (int)y1, (int)x2, (int)y2);
   XSetLineAttributes (display, gc, INT_WIDTH(xctx->lw), LineSolid, CapRound , JoinRound);
  }
 }

 else if((what & END) && i)
 {
#ifdef __unix__
  XDrawSegments(display, xctx->window, gc, r,i);
#else
   for (j = 0; j < i; ++j) {
     XDrawLine(display, xctx->window, gc, r[j].x1, r[j].y1, r[j].x2, r[j].y2);
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
   XDrawArcs(display, xctx->window, gc, xarc,i);
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
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   xarc[i].x=(short)xx1;
   xarc[i].y=(short)yy1;
   xarc[i].width= (unsigned short)(xx2 - xx1);
   xarc[i].height=(unsigned short)(yy2 - yy1);
   xarc[i].angle1 = (short)(a*64);
   xarc[i].angle2 = (short)(b*64);
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
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   XDrawArc(display, xctx->window, gc, (int)xx1, (int)yy1, (int)(xx2-xx1), (int)(yy2-yy1),
            (int)(a*64), (int)(b*64));
  }
 }
 else if((what & END) && i)
 {
  XDrawArcs(display, xctx->window, gc, xarc,i);
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
   if(xctx->draw_window) XFillArcs(display, xctx->window, xctx->gc[c], xarc,i);
   if(xctx->draw_pixmap) XFillArcs(display, xctx->save_pixmap, xctx->gc[c], xarc,i);
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
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   xarc[i].x=(short)xx1;
   xarc[i].y=(short)yy1;
   xarc[i].width =(unsigned short)(xx2 - xx1);
   xarc[i].height=(unsigned short)(yy2 - yy1);
   xarc[i].angle1 = (short)(a*64);
   xarc[i].angle2 = (short)(b*64);
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
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   if(xctx->draw_window) XFillArc(display, xctx->window, xctx->gc[c], (int)xx1, (int)yy1,
                           (int)(xx2-xx1), (int)(yy2-yy1), (int)(a*64), (int)(b*64));
   if(xctx->draw_pixmap) XFillArc(display, xctx->save_pixmap, xctx->gc[c], (int)xx1, (int)yy1,
                            (int)(xx2-xx1), (int)(yy2-yy1), (int)(a*64), (int)(b*64));
  }
 }
 else if((what & END) && i)
 {
  if(xctx->draw_window) XFillArcs(display, xctx->window, xctx->gc[c], xarc,i);
  if(xctx->draw_pixmap) XFillArcs(display, xctx->save_pixmap, xctx->gc[c], xarc,i);
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
   if(xctx->draw_window) XDrawArcs(display, xctx->window, xctx->gc[c], xarc,i);
   if(xctx->draw_pixmap) XDrawArcs(display, xctx->save_pixmap, xctx->gc[c], xarc,i);
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
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   xarc[i].x=(short)xx1;
   xarc[i].y=(short)yy1;
   xarc[i].width =(unsigned short)(xx2 - xx1);
   xarc[i].height=(unsigned short)(yy2 - yy1);
   xarc[i].angle1 = (short)(a*64);
   xarc[i].angle2 = (short)(b*64);
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
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   if(dash) {
     char dash_arr[2];
     dash_arr[0] = dash_arr[1] = (char)dash;
     XSetDashes(display, xctx->gc[c], 0, dash_arr, 2);
     XSetLineAttributes (display, xctx->gc[c], INT_WIDTH(xctx->lw), xDashType, CapButt, JoinBevel);
   }

   if(xctx->draw_window) {
     XDrawArc(display, xctx->window, xctx->gc[c], (int)xx1, (int)yy1,
              (int)(xx2-xx1), (int)(yy2-yy1), (int)(a*64), (int)(b*64));
   }
   if(xctx->draw_pixmap) {
     XDrawArc(display, xctx->save_pixmap, xctx->gc[c], (int)xx1, (int)yy1, 
              (int)(xx2-xx1), (int)(yy2-yy1), (int)(a*64), (int)(b*64));
   }

   if(xctx->fill_pattern && xctx->fill_type[c]){
     if(arc_fill) {
       if(xctx->draw_window)
         XFillArc(display, xctx->window, xctx->gcstipple[c], (int)xx1, (int)yy1, 
              (int)(xx2-xx1), (int)(yy2-yy1), (int)(a*64), (int)(b*64));
       if(xctx->draw_pixmap)
         XFillArc(display, xctx->save_pixmap, xctx->gcstipple[c], (int)xx1, (int)yy1, 
              (int)(xx2-xx1), (int)(yy2-yy1), (int)(a*64), (int)(b*64));
     }
   }
   if(dash) {
     XSetLineAttributes (display, xctx->gc[c], INT_WIDTH(xctx->lw) ,LineSolid, CapRound , JoinRound);
   }
  }
 }
 else if((what & END) && i)
 {
  if(xctx->draw_window) XDrawArcs(display, xctx->window, xctx->gc[c], xarc,i);
  if(xctx->draw_pixmap) XDrawArcs(display, xctx->save_pixmap, xctx->gc[c], xarc,i);
  i=0;
 }
}


void filledrect(int c, int what, double rectx1,double recty1,double rectx2,double recty2)
{
 static int i=0;
 static XRectangle r[CADDRAWBUFFERSIZE];
 double x1,y1,x2,y2;

 if(!has_x) return;
 if(!xctx->fill_pattern || !xctx->fill_type[c]) return;
 if(what & NOW)
 {
  x1=X_TO_SCREEN(rectx1);
  y1=Y_TO_SCREEN(recty1);
  x2=X_TO_SCREEN(rectx2);
  y2=Y_TO_SCREEN(recty2);
  if(!xctx->only_probes && (x2-x1)< 3.0 && (y2-y1)< 3.0) return;
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   if(xctx->draw_window) XFillRectangle(display, xctx->window, xctx->gcstipple[c], (int)x1, (int)y1,
    (unsigned int)x2 - (unsigned int)x1,
    (unsigned int)y2 - (unsigned int)y1);
   if(xctx->draw_pixmap)
     XFillRectangle(display, xctx->save_pixmap,xctx->gcstipple[c],  (int)x1, (int)y1,
      (unsigned int)x2 - (unsigned int)x1,
      (unsigned int)y2 - (unsigned int)y1);
  }
 }
 else if(what & ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
   if(xctx->draw_window) XFillRectangles(display, xctx->window, xctx->gcstipple[c], r,i);
   if(xctx->draw_pixmap)
     XFillRectangles(display, xctx->save_pixmap, xctx->gcstipple[c], r,i);
   i=0;
  }
  x1=X_TO_SCREEN(rectx1);
  y1=Y_TO_SCREEN(recty1);
  x2=X_TO_SCREEN(rectx2);
  y2=Y_TO_SCREEN(recty2);
  if(!xctx->only_probes && (x2-x1)< 3.0 && (y2-y1)< 3.0) return;
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   r[i].x=(short)x1;
   r[i].y=(short)y1;
   r[i].width=(unsigned short)(x2-r[i].x);
   r[i].height=(unsigned short)(y2-r[i].y);
   i++;
  }
 }
 else if((what & END) && i)
 {
  if(xctx->draw_window) XFillRectangles(display, xctx->window, xctx->gcstipple[c], r,i);
  if(xctx->draw_pixmap) XFillRectangles(display, xctx->save_pixmap, xctx->gcstipple[c], r,i);
  i=0;
 }
}

void polygon_bbox(double *x, double *y, int points, double *bx1, double *by1, double *bx2, double *by2)
{
  int j;
  for(j=0; j<points; j++) {
    if(j==0 || x[j] < *bx1) *bx1 = x[j];
    if(j==0 || x[j] > *bx2) *bx2 = x[j];
    if(j==0 || y[j] < *by1) *by1 = y[j];
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
/* Unused 'what' parameter used in spice data draw_graph() 
 * to avoid unnecessary clipping (what = 0) */
void drawpolygon(int c, int what, double *x, double *y, int points, int poly_fill, int dash)
{
  double x1,y1,x2,y2;
  XPoint *p;
  int i;
  short sx, sy;
  if(!has_x) return;

  polygon_bbox(x, y, points, &x1,&y1,&x2,&y2);
  x1=X_TO_SCREEN(x1);
  x2=X_TO_SCREEN(x2);
  y1=Y_TO_SCREEN(y1);
  y2=Y_TO_SCREEN(y2);
  if( !rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) ) {
    return;
  }
  if( !xctx->only_probes && (x2-x1)<1.0 && (y2-y1)<1.0) return;

  p = my_malloc(38, sizeof(XPoint) * points);
  if(what) {
    for(i=0;i<points; i++) {
      clip_xy_to_short(X_TO_SCREEN(x[i]), Y_TO_SCREEN(y[i]), &sx, &sy);
      p[i].x = sx;
      p[i].y = sy;
    }
  } else {
      /* preserve cache locality working on contiguous data */
      for(i=0;i<points; i++) p[i].x = (short)X_TO_SCREEN(x[i]);
      for(i=0;i<points; i++) p[i].y = (short)Y_TO_SCREEN(y[i]);
  }
  if(dash) {
    char dash_arr[2];
    dash_arr[0] = dash_arr[1] = (char)dash;
    XSetDashes(display, xctx->gc[c], 0, dash_arr, 2);
    XSetLineAttributes (display, xctx->gc[c], INT_WIDTH(xctx->lw), xDashType, CapButt, JoinBevel);
  }
  if(xctx->draw_window) XDrawLines(display, xctx->window, xctx->gc[c], p, points, CoordModeOrigin);
  if(xctx->draw_pixmap)
    XDrawLines(display, xctx->save_pixmap, xctx->gc[c], p, points, CoordModeOrigin);
  if(xctx->fill_pattern && xctx->fill_type[c]){
    if(poly_fill && (x[0] == x[points-1]) && (y[0] == y[points-1])) {
      if(xctx->draw_window)
         XFillPolygon(display, xctx->window, xctx->gcstipple[c], p, points, Polygontype, CoordModeOrigin);
      if(xctx->draw_pixmap)
         XFillPolygon(display, xctx->save_pixmap, xctx->gcstipple[c], p, points, Polygontype, CoordModeOrigin);
    }
  }
  if(dash) {
    XSetLineAttributes (display, xctx->gc[c], INT_WIDTH(xctx->lw) ,LineSolid, CapRound , JoinRound);
  }
  my_free(722, &p);
}

void drawtemppolygon(GC g, int what, double *x, double *y, int points)
{
  double x1,y1,x2,y2;
  XPoint *p;
  int i;
  short sx, sy;
  if(!has_x) return;
  polygon_bbox(x, y, points, &x1,&y1,&x2,&y2);
  x1=X_TO_SCREEN(x1);
  y1=Y_TO_SCREEN(y1);
  x2=X_TO_SCREEN(x2);
  y2=Y_TO_SCREEN(y2);
  p = my_malloc(39, sizeof(XPoint) * points);
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) ) {
    for(i=0;i<points; i++) {
      clip_xy_to_short(X_TO_SCREEN(x[i]), Y_TO_SCREEN(y[i]), &sx, &sy);
      p[i].x = sx;
      p[i].y = sy;
    }
    XDrawLines(display, xctx->window, g, p, points, CoordModeOrigin);
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
  /* if(!xctx->only_probes && (x2-x1)< 3.0 && (y2-y1)< 3.0) return; */
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   if(dash) {
     dash_arr[0] = dash_arr[1] = (char)dash;
     XSetDashes(display, xctx->gc[c], 0, dash_arr, 2);
     XSetLineAttributes (display, xctx->gc[c], INT_WIDTH(xctx->lw), xDashType, CapButt, JoinBevel);
   }
   if(xctx->draw_window) XDrawRectangle(display, xctx->window, xctx->gc[c], (int)x1, (int)y1,
    (unsigned int)x2 - (unsigned int)x1,
    (unsigned int)y2 - (unsigned int)y1);
   if(xctx->draw_pixmap)
   {
    XDrawRectangle(display, xctx->save_pixmap, xctx->gc[c], (int)x1, (int)y1,
    (unsigned int)x2 - (unsigned int)x1,
    (unsigned int)y2 - (unsigned int)y1);
   }
   if(dash) {
     XSetLineAttributes (display, xctx->gc[c], INT_WIDTH(xctx->lw) ,LineSolid, CapRound , JoinRound);
   }
  }
 }
 else if(what & ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
   if(xctx->draw_window) XDrawRectangles(display, xctx->window, xctx->gc[c], r,i);
   if(xctx->draw_pixmap)
     XDrawRectangles(display, xctx->save_pixmap, xctx->gc[c], r,i);
   i=0;
  }
  x1=X_TO_SCREEN(rectx1);
  y1=Y_TO_SCREEN(recty1);
  x2=X_TO_SCREEN(rectx2);
  y2=Y_TO_SCREEN(recty2);
  /* if(!xctx->only_probes && (x2-x1)< 3.0 && (y2-y1)< 3.0) return; */
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   r[i].x=(short)x1;
   r[i].y=(short)y1;
   r[i].width=(unsigned short)(x2-r[i].x);
   r[i].height=(unsigned short)(y2-r[i].y);
   i++;
  }
 }
 else if((what & END) && i)
 {
  if(xctx->draw_window) XDrawRectangles(display, xctx->window, xctx->gc[c], r,i);
  if(xctx->draw_pixmap) XDrawRectangles(display, xctx->save_pixmap, xctx->gc[c], r,i);
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
  /* if( (x2-x1)< 3.0 && (y2-y1)< 3.0) return; */
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   XDrawRectangle(display, xctx->window, gc, (int)x1, (int)y1,
    (unsigned int)x2 - (unsigned int)x1,
    (unsigned int)y2 - (unsigned int)y1);
  }
 }
 else if(what & ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
   XDrawRectangles(display, xctx->window, gc, r,i);
   i=0;
  }
  x1=X_TO_SCREEN(rectx1);
  y1=Y_TO_SCREEN(recty1);
  x2=X_TO_SCREEN(rectx2);
  y2=Y_TO_SCREEN(recty2);
  /* if( (x2-x1)< 3.0 && (y2-y1)< 3.0) return; */
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   r[i].x=(short)x1;
   r[i].y=(short)y1;
   r[i].width=(unsigned short)(x2-r[i].x);
   r[i].height=(unsigned short)(y2-r[i].y);
   i++;
  }
 }
 else if((what & END) && i)
 {
  XDrawRectangles(display, xctx->window, gc, r,i);
  i=0;
 }
}

/* round to closest 1e-ee. 2e-ee, 5e-ee
 * example: delta = 0.234 --> 0.2
 *                  3.23  --> 5
 *                  66    --> 100
 *                  4.3   --> 5
 *                  13    --> 20
 *                  112   --> 100
 *                  6300  --> 10000
 */
static double axis_increment(double a, double b, int div, int freq)
{
  double scale;
  double sign;
  double scaled_delta;
  double delta = b - a;
  if(div == 1) {
    return delta;
  }
  if(div < 1) div = 1;
  if(delta == 0.0) return delta;
  /* if user wants only one division, just do what user asks */
  if(div == 1) return delta;
  delta /= div;
  sign = (delta < 0.0) ? -1.0 : 1.0;
  delta = fabs(delta);
  scale = pow(10.0, floor(mylog10(delta)));
  scaled_delta =  delta / scale; /* 1 <= scaled_delta < 10 */
  dbg(1, "a=%g, b=%g, scale=%g, scaled_delta=%g --> ", a, b, scale, scaled_delta);
  if(freq && scaled_delta > 2.5) scaled_delta = 10.0;
  else if(freq && scaled_delta > 1.9) scaled_delta = 5.0;
  else if(freq && scaled_delta > 1.4) scaled_delta = 2.0;
  else if(freq) scaled_delta = 1.0;
  else if(scaled_delta > 5.5) scaled_delta = 10.0;
  else if(scaled_delta > 2.2) scaled_delta = 5.0;
  else if(scaled_delta > 1.1) scaled_delta = 2.0;
  else scaled_delta = 1.0;
  dbg(1, "scaled_delta = %g, scaled_delta * scale * sign=%g\n", scaled_delta, scaled_delta * scale * sign);
  return scaled_delta * scale * sign;
}

static double axis_start(double n, double delta, int div)
{
  if(div < 1) div = 1;
  if(delta == 0.0) return n;
  /* if user wants only one division, just do what user asks */
  if(div == 1) return n;
  if(delta < 0.0) return floor(n / delta) * delta;
  return ceil(n / delta) * delta;
}

static int axis_end(double x, double delta, double b)
{
  if(delta == 0.0) return 1;  /* guard against infinite loops */
  if(delta > 0) return x > b + delta / 100000.0;
  return x < b + delta / 100000.0;
}

static int axis_within_range(double x, double a, double b)
{
  if(a < b) return x >= a;
  return x <= a;
}

static double get_unit(const char *val)
{
  if(!val)               return 1.0;
  else if(val[0] == 'p') return 1e12;
  else if(val[0] == 'n') return 1e9;
  else if(val[0] == 'u') return 1e6;
  else if(val[0] == 'm') return 1e3;
  else if(val[0] == 'k') return 1e-3;
  else if(val[0] == 'M') return 1e-6;
  else if(val[0] == 'G') return 1e-9;
  return 1.0;
}

int schematic_waves_loaded(void)
{
  int i;
  if(xctx->graph_values && xctx->graph_raw_schname) {
    for(i = xctx->currsch; i >= 0; i--) {
      if( !strcmp(xctx->graph_raw_schname, xctx->sch[i]) ) return 1;
    } 
  }
  return 0;
}

static void get_bus_value(int n_bits, int hex_digits, SPICE_DATA **idx_arr, int p, char *busval,
                   double vthl, double vthh)
{
  double val;
  int i;
  int hexdigit = 0;
  int bin = 0;
  int hex = 0;
  char hexstr[] = "084C2A6E195D3B7F"; /* mirrored (Left/right) hex */
  int x = 0;
  for(i = n_bits - 1; i >= 0; i--) {
    if(idx_arr[i]) val = idx_arr[i][p];
    else val = 0.0; /* undefined bus element */
    if(val >= vthl && val <= vthh) { /* signal transitioning --> 'X' */
       x = 1; /* flag for 'X' value */
       i += bin - 3;
       bin = 3; /* skip remaining bits of hex digit */
       if(i < 0) break; /* MSB nibble is less than 4 bits --> break */
    } else hexdigit |= (val >= vthh ? 1 : 0);
    if(bin < 3) {
      bin++;
      hexdigit <<= 1;
    } else {
      hex++;
      if(x) 
        busval[hex_digits - hex] = 'X';
      else
        busval[hex_digits - hex] = hexstr[hexdigit]; 
      hexdigit = 0; /* prepare for next hex digit */
      bin = 0;
      x = 0;
    }
  }
  if(bin) { /* process (incomplete) MSB nibble */
    hex++;
    if(x) 
      busval[hex_digits - hex] = 'X';
    else {
      hexdigit <<= (3 - bin);
      busval[hex_digits - hex] = hexstr[hexdigit]; 
    }
  }
  busval[hex_digits] = '\0';
} 


/* idx_arr malloc-ated and returned, caller must free! */
static SPICE_DATA **get_bus_idx_array(const char *ntok, int *n_bits)
{
  SPICE_DATA **idx_arr =NULL;
  int p;
  char *saven, *nptr, *ntok_copy = NULL;
  const char *bit_name;
  *n_bits = count_items(ntok, ";,", "") - 1;
  idx_arr = my_malloc(1454, (*n_bits) * sizeof(SPICE_DATA *));
  p = 0;
  my_strdup2(1402, &ntok_copy, ntok);
  nptr = ntok_copy;
  my_strtok_r(nptr, ";,", "", &saven); /*strip off bus name (1st field) */
  while( (bit_name = my_strtok_r(NULL, ",", "", &saven)) ) {
    int idx;
    if( (idx = get_raw_index(bit_name)) != -1) {
      idx_arr[p] = xctx->graph_values[idx];
    } else {
      idx_arr[p] = NULL;
    }
    p++;
  }
  my_free(1404, &ntok_copy);
  return idx_arr;
}

/* what == 1: set thick lines,
 * what == 0: restore default
 */
static void set_thick_waves(int what, int wcnt, int wave_col, Graph_ctx *gr)
{
  if(what) {
      if(gr->hilight_wave == wcnt)
         XSetLineAttributes (display, xctx->gc[wave_col],
            3 * INT_WIDTH(xctx->lw) ,LineSolid, CapRound , JoinRound);
  } else {
      if(gr->hilight_wave == wcnt)
         XSetLineAttributes (display, xctx->gc[wave_col],
            INT_WIDTH(xctx->lw) ,LineSolid, CapRound , JoinRound);
  }
}

/* draw bussed signals: ntok is a comma separated list of items, first item is bus name, 
 * following are bits that are bundled together:
   LDA,LDA[3],LDA[2],LDA1],LDA[0]
 */
static void draw_graph_bus_points(const char *ntok, int n_bits, SPICE_DATA **idx_arr, 
         int first, int last, int wave_col, int sweep_idx, int wcnt, int n_nodes, Graph_ctx *gr)
{
  int p;
  double s1 = DIG_NWAVES; /* 1/DIG_NWAVES  waveforms fit in graph if unscaled vertically */
  double s2 = DIG_SPACE; /* (DIG_NWAVES - DIG_SPACE) spacing between traces */
  double c = (n_nodes - wcnt) * s1 * gr->gh - gr->gy1 * s2; /* trace baseline */
  double c1 = c + gr->gh * 0.5 * s2; /* trace y-center, used for clipping */
  double lx1;
  double lx2;
  double ylow  = DW_Y(c + gr->gy2 * s2); /* swapped as xschem Y coordinates are top-bottom */
  double yhigh = DW_Y(c + gr->gy1 * s2);
  char busval[1024], old_busval[1024];
  double xval=0.0, xval_old=0.0;
  double ydelta = fabs(yhigh - ylow);
  double labsize = 0.015 * ydelta;
  double charwidth = labsize * 38.0;
  double x_size = 1.5 * xctx->zoom;
  double vthh = gr->gy1 * 0.2 + gr->gy2 * 0.8;
  double vthl = gr->gy1 * 0.8 + gr->gy2 * 0.2;
  int hex_digits = ((n_bits - 1) >> 2) + 1;

  if(gr->logx) {
    lx1 = W_X(mylog10(xctx->graph_values[sweep_idx][first]));
    lx2 = W_X(mylog10(xctx->graph_values[sweep_idx][last]));
  } else {
    lx1 = W_X(xctx->graph_values[sweep_idx][first]);
    lx2 = W_X(xctx->graph_values[sweep_idx][last]);
  }
  if(c1 >= gr->ypos1 && c1 <=gr->ypos2) {
    set_thick_waves(1, wcnt, wave_col, gr);
    drawline(wave_col, NOW, lx1, ylow, lx2, ylow, 0);
    drawline(wave_col, NOW, lx1, yhigh, lx2, yhigh, 0);
    for(p = first ; p <= last; p++) {
      /* calculate value of bus by adding all binary bits */
      /* hex_digits = */
      get_bus_value(n_bits, hex_digits, idx_arr, p, busval, vthl, vthh);
      if(gr->logx) {
        xval =  W_X(mylog10(xctx->graph_values[sweep_idx][p]));
      } else {
        xval =  W_X(xctx->graph_values[sweep_idx][p]);
      }
      /* used to draw bus value before 1st transition */
      if(p == first) {
        my_strncpy(old_busval, busval, hex_digits+1);
        xval_old = xval;
      }
      if(p > first &&  strcmp(busval, old_busval)) {
        /* draw transition ('X') */
        drawline(BACKLAYER, NOW, xval-x_size, yhigh, xval+x_size, yhigh, 0);
        drawline(BACKLAYER, NOW, xval-x_size, ylow,  xval+x_size, ylow, 0);
        drawline(wave_col, NOW, xval-x_size, ylow,  xval+x_size, yhigh, 0);
        drawline(wave_col, NOW, xval-x_size, yhigh, xval+x_size, ylow, 0);
        /* draw hex bus value if there is enough room */
        if(  fabs(xval - xval_old) > hex_digits * charwidth) {
          draw_string(wave_col, NOW, old_busval, 2, 0, 1, 0, (xval + xval_old) * 0.5,
                      yhigh, labsize, labsize);
        }
        my_strncpy(old_busval, busval, hex_digits + 1);
        xval_old = xval;
      } /* if(p > first &&  busval != old_busval) */
    } /* for(p = first ; p < last; p++) */
    /* draw hex bus value after last transition */
    if(  fabs(xval - xval_old) > hex_digits * charwidth) {
      draw_string(wave_col, NOW, old_busval, 2, 0, 1, 0, (xval + xval_old) * 0.5,
                  yhigh, labsize, labsize);
    }
    set_thick_waves(0, wcnt, wave_col, gr);
  }
}

/* wcnt is the nth wave in graph, idx is the index in spice raw file */
static void draw_graph_points(int idx, int first, int last,
         XPoint *point, int wave_col, int wcnt, int n_nodes, Graph_ctx *gr)
{
  int p;
  register double yy;
  register int digital;
  int poly_npoints = 0;
  double s1;
  double s2;
  double c, c1;
  register SPICE_DATA *gv = xctx->graph_values[idx];

  dbg(1, "draw_graph_points: idx=%d, first=%d, last=%d, wcnt=%d\n", idx, first, last, wcnt);
  if(idx == -1) return;
  digital = gr->digital;
  if(digital) {
    s1 = DIG_NWAVES; /* 1/DIG_NWAVES  waveforms fit in graph if unscaled vertically */
    s2 = DIG_SPACE; /* (DIG_NWAVES - DIG_SPACE) spacing between traces */
    c = (n_nodes - wcnt) * s1 * gr->gh - gr->gy1 * s2; /* trace baseline */
    c1 = c + gr->gh * 0.5 * s2; /* trace y-center, used for clipping */
  }
  if( !digital || (c1 >= gr->ypos1 && c1 <= gr->ypos2) ) {
    for(p = first ; p <= last; p++) {
      yy = gv[p];
      if(digital) {
        yy = c + yy *s2;
        /* Build poly y array. Translate from graph coordinates to screen coordinates  */
        point[poly_npoints].y = (short)CLIP(DS_Y(yy), -30000, 30000);
      } else {
        /* Build poly y array. Translate from graph coordinates to screen coordinates  */
        if(gr->logy) yy = mylog10(yy);
        point[poly_npoints].y = (short)CLIP(S_Y(yy), -30000, 30000);
      }
      poly_npoints++;
    }
    set_thick_waves(1, wcnt, wave_col, gr);
    if(xctx->draw_window) {
      XDrawLines(display, xctx->window, xctx->gc[wave_col], point, poly_npoints, CoordModeOrigin);
    }
    if(xctx->draw_pixmap) {
      XDrawLines(display, xctx->save_pixmap, xctx->gc[wave_col], point, poly_npoints, CoordModeOrigin);
    }
    set_thick_waves(0, wcnt, wave_col, gr);
  } else dbg(1, "skipping wave: %s\n", xctx->graph_names[idx]);
}

static void draw_graph_grid(Graph_ctx *gr)
{
  double deltax, startx, deltay, starty, wx,wy,  dash_sizex, dash_sizey;
  int j, k;
  double mark_size = gr->marginy/10.0;

  /* calculate dash length for grid lines */
  dash_sizex = 1.5 * xctx->mooz;
  dash_sizex = dash_sizex > 2.0 ? 2.0 : dash_sizex;
  if(dash_sizex <= 1.0) dash_sizex = 1.0;
  dash_sizey = 1.5 * xctx->mooz;
  dash_sizey = dash_sizey > 2.0 ? 2.0 : dash_sizey;
  if(dash_sizey <= 1.0) dash_sizey = 1.0;

  /* clipping everything outside container area */
  /* background */
  filledrect(0, NOW, gr->rx1, gr->ry1, gr->rx2, gr->ry2);
  /* graph bounding box */
  drawrect(GRIDLAYER, NOW, gr->rx1, gr->ry1, gr->rx2, gr->ry2, 2);

  bbox(START, 0.0, 0.0, 0.0, 0.0);
  bbox(ADD, gr->rx1, gr->ry1, gr->rx2, gr->ry2);
  bbox(SET_INSIDE, 0.0, 0.0, 0.0, 0.0);
  /* vertical grid lines */
  deltax = axis_increment(gr->gx1, gr->gx2, gr->divx, (gr->logx));
  startx = axis_start(gr->gx1, deltax, gr->divx);
  for(j = -1;; j++) { /* start one interval before to allow sub grids at beginning */
    wx = startx + j * deltax;
    if(gr->subdivx > 0) for(k = 1; k <=gr->subdivx; k++) {
      double subwx;
      if(gr->logx)
        subwx = wx + deltax * mylog10(1.0 + (double)k * 9.0 / ((double)gr->subdivx + 1.0)); 
      else
        subwx = wx + deltax * (double)k / ((double)gr->subdivx + 1.0);
      if(!axis_within_range(subwx, gr->gx1, gr->gx2)) continue;
      if(axis_end(subwx, deltax, gr->gx2)) break;
      drawline(GRIDLAYER, ADD, W_X(subwx),   W_Y(gr->gy2), W_X(subwx),   W_Y(gr->gy1), (int)dash_sizey);
    }
    if(!axis_within_range(wx, gr->gx1, gr->gx2)) continue;
    if(axis_end(wx, deltax, gr->gx2)) break;
    /* swap order of gy1 and gy2 since grap y orientation is opposite to xorg orientation */
    drawline(GRIDLAYER, ADD, W_X(wx),   W_Y(gr->gy2), W_X(wx),   W_Y(gr->gy1), (int)dash_sizey);
    drawline(GRIDLAYER, ADD, W_X(wx),   W_Y(gr->gy1), W_X(wx),   W_Y(gr->gy1) + mark_size, 0); /* axis marks */
    /* X-axis labels */
    if(gr->logx) 
      draw_string(3, NOW, dtoa(pow(10, wx) * gr->unitx), 0, 0, 1, 0, W_X(wx), gr->y2 + mark_size + 5 * gr->txtsizex,
                gr->txtsizex, gr->txtsizex);
    else
      draw_string(3, NOW, dtoa(wx * gr->unitx), 0, 0, 1, 0, W_X(wx), gr->y2 + mark_size + 5 * gr->txtsizex,
                gr->txtsizex, gr->txtsizex);
  }
  /* first and last vertical box delimiters */
  drawline(GRIDLAYER, ADD, W_X(gr->gx1),   W_Y(gr->gy2), W_X(gr->gx1),   W_Y(gr->gy1), 0);
  drawline(GRIDLAYER, ADD, W_X(gr->gx2),   W_Y(gr->gy2), W_X(gr->gx2),   W_Y(gr->gy1), 0);
  /* horizontal grid lines */
  if(!gr->digital) {
    deltay = axis_increment(gr->gy1, gr->gy2, gr->divy, gr->logy);
    starty = axis_start(gr->gy1, deltay, gr->divy);
    for(j = -1;; j++) { /* start one interval before to allow sub grids at beginning */
      wy = starty + j * deltay;
      if(gr->subdivy > 0) for(k = 1; k <=gr->subdivy; k++) {
        double subwy;
        if(gr->logy)
          subwy = wy + deltay * mylog10(1.0 + (double)k * 9.0 / ((double)gr->subdivy + 1.0));
        else
          subwy = wy + deltay * (double)k / ((double)gr->subdivy + 1.0);
        if(!axis_within_range(subwy, gr->gy1, gr->gy2)) continue;
        if(axis_end(subwy, deltay, gr->gy2)) break;
        drawline(GRIDLAYER, ADD, W_X(gr->gx1), W_Y(subwy),   W_X(gr->gx2), W_Y(subwy), (int)dash_sizex);
      }
      if(!axis_within_range(wy, gr->gy1, gr->gy2)) continue;
      if(axis_end(wy, deltay, gr->gy2)) break;
      drawline(GRIDLAYER, ADD, W_X(gr->gx1), W_Y(wy),   W_X(gr->gx2), W_Y(wy), (int)dash_sizex);
      drawline(GRIDLAYER, ADD, W_X(gr->gx1) - mark_size, W_Y(wy),   W_X(gr->gx1), W_Y(wy), 0); /* axis marks */
      /* Y-axis labels */
      if(gr->logy)
        draw_string(3, NOW, dtoa(pow(10, wy) * gr->unity), 0, 1, 0, 1, gr->x1 - mark_size - 5 * gr->txtsizey, W_Y(wy),
                  gr->txtsizey, gr->txtsizey);
      else 
        draw_string(3, NOW, dtoa(wy * gr->unity), 0, 1, 0, 1, gr->x1 - mark_size - 5 * gr->txtsizey, W_Y(wy),
                  gr->txtsizey, gr->txtsizey);
    }
  }
  /* first and last horizontal box delimiters */
  drawline(GRIDLAYER, ADD, W_X(gr->gx1),   W_Y(gr->gy1), W_X(gr->gx2),   W_Y(gr->gy1), 0);
  drawline(GRIDLAYER, ADD, W_X(gr->gx1),   W_Y(gr->gy2), W_X(gr->gx2),   W_Y(gr->gy2), 0);
  /* Horizontal axis (if in viewport) */
  if(!gr->digital && gr->gy1 <= 0 && gr->gy2 >= 0)
    drawline(GRIDLAYER, ADD, W_X(gr->gx1), W_Y(0), W_X(gr->gx2), W_Y(0), 0);
  /* Vertical axis (if in viewport) 
   * swap order of gy1 and gy2 since grap y orientation is opposite to xorg orientation */
  if(gr->gx1 <= 0 && gr->gx2 >= 0)
    drawline(GRIDLAYER, ADD, W_X(0),   W_Y(gr->gy2), W_X(0),   W_Y(gr->gy1), 0);
  drawline(GRIDLAYER, END, 0.0, 0.0, 0.0, 0.0, 0);
  bbox(END, 0.0, 0.0, 0.0, 0.0);
}

void setup_graph_data(int i, const int flags, int skip, Graph_ctx *gr)
{
  double tmp;
  const char *val;
  xRect *r = &xctx->rect[GRIDLAYER][i];

  gr->i = i;
  /* default values */
  gr->divx = gr->divy = 5;
  gr->subdivx = gr->subdivy = 0;
  gr->logx = gr->logy = 0;
  gr->digital = 0;

  if(!skip) {
    gr->gx1 = 0;
    gr->gx2 = 1e-6;
    val = get_tok_value(r->prop_ptr,"x1",0);
    if(val[0]) gr->gx1 = atof(val);
    val = get_tok_value(r->prop_ptr,"x2",0);
    if(val[0]) gr->gx2 = atof(val);
    if(gr->gx1 == gr->gx2) gr->gx2 += 1e-6;
    gr->gw = gr->gx2 - gr->gx1;
  }
  gr->gy1 = 0;
  gr->gy2 = 5;
  gr->dataset = -1; /* -1 means 'plot all datasets' */
  gr->ypos1 = 0;
  gr->ypos2 = 2;
  gr->digtxtsizelab = 0.3;
  gr->txtsizelab = 0.3;
  gr->txtsizex = 0.3;
  gr->txtsizey = 0.3;

  /* container (embedding rectangle) coordinates */
  gr->rx1 = r->x1;
  gr->ry1 = r->y1;
  gr->rx2 = r->x2;
  gr->ry2 = r->y2;

  /* screen position */
  gr->sx1=X_TO_SCREEN(gr->rx1);
  gr->sy1=Y_TO_SCREEN(gr->ry1);
  gr->sx2=X_TO_SCREEN(gr->rx2);
  gr->sy2=Y_TO_SCREEN(gr->ry2);

  if(RECT_OUTSIDE(gr->sx1, gr->sy1, gr->sx2, gr->sy2,
                  xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2)) return;

  gr->rw = gr->rx2 - gr->rx1;
  gr->rh = gr->ry2 - gr->ry1;

  /* wave to display in bold, -1=none */
  val=get_tok_value(r->prop_ptr,"hilight_wave",0);
  if(val[0]) gr->hilight_wave = atoi(val);
  else gr->hilight_wave = -1;

  /* get x/y range, grid info etc */
  val = get_tok_value(r->prop_ptr,"unitx",0);
  gr->unitx_suffix = val[0];
  gr->unitx = get_unit(val);
  val = get_tok_value(r->prop_ptr,"unity",0);
  if(gr->logx) { /* AC */
    gr->unity_suffix = '1';
    gr->unity = 1.0;
  } else {
    gr->unity_suffix = val[0];
    gr->unity = get_unit(val);
  }
  val = get_tok_value(r->prop_ptr,"subdivx",0);
  if(val[0]) gr->subdivx = atoi(val);
  val = get_tok_value(r->prop_ptr,"subdivy",0);
  if(val[0]) gr->subdivy = atoi(val);
  val = get_tok_value(r->prop_ptr,"divx",0);
  if(val[0]) gr->divx = atoi(val);
  if(gr->divx < 1) gr->divx = 1;
  val = get_tok_value(r->prop_ptr,"divy",0);
  if(val[0]) gr->divy = atoi(val);
  if(gr->divy < 1) gr->divy = 1;
  val = get_tok_value(r->prop_ptr,"logx",0);
  if(val[0] == '1') gr->logx = 1;
  val = get_tok_value(r->prop_ptr,"logy",0);
  if(val[0] == '1') gr->logy = 1;
  val = get_tok_value(r->prop_ptr,"y1",0);
  if(val[0]) gr->gy1 = atof(val);
  val = get_tok_value(r->prop_ptr,"y2",0);
  if(val[0]) gr->gy2 = atof(val);
  if(gr->gy1 == gr->gy2) gr->gy2 += 1.0;
  val = get_tok_value(r->prop_ptr,"digital",0);
  if(val[0]) gr->digital = atoi(val);
  if(gr->digital) {
    val = get_tok_value(r->prop_ptr,"ypos1",0);
    if(val[0]) gr->ypos1 = atof(val);
    val = get_tok_value(r->prop_ptr,"ypos2",0);
    if(val[0]) gr->ypos2 = atof(val);
    if(gr->ypos2 == gr->ypos1) gr->ypos2 += 1.0;
  }
  gr->posh = gr->ypos2 - gr->ypos1;

  /* plot single dataset */
  val = get_tok_value(r->prop_ptr,"dataset",0);
  if(val[0]) gr->dataset = atoi(val);
  gr->gh = gr->gy2 - gr->gy1;
  /* set margins */
  tmp = gr->rw * 0.14;
  gr->marginx = tmp < 30 ? 30 : tmp;
  tmp = gr->rh * 0.14;
  gr->marginy = tmp < 35 ? 35 : tmp;

  /* calculate graph bounding box (container - margin) 
   * This is the box where plot is done */
  gr->x1 =  gr->rx1 + gr->marginx;
  gr->x2 =  gr->rx2 - gr->marginx * 0.35; /* less space for right margin */
  if(gr->digital) gr->y1 = gr->ry1 + gr->marginy * 0.4; /* less top space for digital graphs */
  else gr->y1 =  gr->ry1 + gr->marginy;
  gr->y2 =  gr->ry2 - gr->marginy;
  gr->w = gr->x2 - gr->x1;
  gr->h = gr->y2 - gr->y1;

  /* label text size calculations */
  gr->txtsizelab = gr->marginy * 0.007;
  /* 
   * tmp =  gr->w * 0.00044;
   * if(tmp < gr->txtsizelab) gr->txtsizelab = tmp;
   */
  if(flags & 2)
    gr->digtxtsizelab = 0.000900 * fabs( gr->h / gr->posh * gr->gh ); 
  else
    gr->digtxtsizelab = 0.001200 * fabs( gr->h / gr->posh * gr->gh );

  /* x axis, y axis text sizes */
  gr->txtsizey = gr->h / gr->divy * 0.009;
  tmp = gr->marginx * 0.005;
  if(tmp < gr->txtsizey) gr->txtsizey = tmp;
  tmp = gr->marginy * 0.02;
  if(tmp < gr->txtsizey) gr->txtsizey = tmp;

  gr->txtsizex = gr->w / gr->divx * 0.0033;
  tmp = gr->marginy * 0.0063;
  if(tmp < gr->txtsizex) gr->txtsizex = tmp;

  /* cache coefficients for faster graph --> xschem coord transformations */
  gr->cx = gr->w / gr->gw;
  gr->dx = gr->x1 - gr->gx1 * gr->cx;
  gr->cy = -gr->h / gr->gh;
  gr->dy = gr->y2 - gr->gy1 * gr->cy;
  /* graph --> xschem transform for digital waves y axis */
  gr->dcy = -gr->h / gr->posh;
  gr->ddy = gr->y2 - gr->ypos1 * gr->dcy;

  /* direct graph --> screen transform */
  gr->scx = gr->cx * xctx->mooz;
  gr->sdx = (gr->dx + xctx->xorigin) * xctx->mooz;
  gr->scy = gr->cy * xctx->mooz;
  gr->sdy = (gr->dy + xctx->yorigin) * xctx->mooz;
  /* direct graph --> screen for digital waves y axis */
  gr->dscy = gr->dcy * xctx->mooz;
  gr->dsdy = (gr->ddy + xctx->yorigin) * xctx->mooz;
}

static void draw_cursor(double active_cursorx, double other_cursorx, int cursor_color, Graph_ctx *gr)
{

  double xx = W_X(active_cursorx);
  double tx1, ty1, tx2, ty2, dtmp;
  int tmp;
  char tmpstr[1024];
  double txtsize = gr->txtsizex;
  short flip = (other_cursorx > active_cursorx) ? 0 : 1;
  int xoffs = flip ? 3 : -3;

  if(xx >= gr->x1 && xx <= gr->x2) {
    drawline(cursor_color, NOW, xx, gr->ry1, xx, gr->ry2, 1);
    if(gr->logx) active_cursorx = pow(10, active_cursorx);
    if(gr->unitx != 1.0)
       my_snprintf(tmpstr, S(tmpstr), "%.5g%c", gr->unitx * active_cursorx , gr->unitx_suffix);
    else
       my_snprintf(tmpstr, S(tmpstr), "%.5g",  active_cursorx);
    text_bbox(tmpstr, txtsize, txtsize, 2, flip, 0, 0, xx + xoffs, gr->ry2-1, &tx1, &ty1, &tx2, &ty2, &tmp, &dtmp);
    filledrect(0, NOW,  tx1, ty1, tx2, ty2);
    draw_string(cursor_color, NOW, tmpstr, 2, flip, 0, 0, xx + xoffs, gr->ry2-1, txtsize, txtsize);
  }
}

static void draw_cursor_difference(Graph_ctx *gr)
{
  int tmp; 
  char tmpstr[1024];
  double txtsize = gr->txtsizex;
  double tx1, ty1, tx2, ty2;
  double aa = W_X(xctx->graph_cursor1_x);
  double a = CLIP(aa, gr->x1, gr->x2);
  double bb = W_X(xctx->graph_cursor2_x);
  double b = CLIP(bb, gr->x1, gr->x2);
  double diff = fabs(b - a);
  double diffw = fabs(xctx->graph_cursor2_x - xctx->graph_cursor1_x);
  double xx = ( a + b ) * 0.5;
  double yy = gr->ry2 - 1;
  double dtmp;
  double yline;
  if(gr->logx) return;
  if(gr->unitx != 1.0)
     my_snprintf(tmpstr, S(tmpstr), "%.4g%c", gr->unitx * diffw , gr->unitx_suffix);
  else
     my_snprintf(tmpstr, S(tmpstr), "%.4g",  diffw);
  text_bbox(tmpstr, txtsize, txtsize, 2, 0, 1, 0, xx, yy, &tx1, &ty1, &tx2, &ty2, &tmp, &dtmp);
  if( tx2 - tx1 < diff ) {
    draw_string(3, NOW, tmpstr, 2, 0, 1, 0, xx, yy, txtsize, txtsize);
    if( a > b) {
      dtmp = a; a = b; b = dtmp;
    }
    yline = (ty1 + ty2) * 0.5;
    if( tx1 - a > 4.0) drawline(3, NOW, a + 2, yline, tx1 - 2, yline, 1);
    if( b - tx2 > 4.0) drawline(3, NOW, tx2 + 2, yline, b - 2, yline, 1);
  }
}

/* sweep variables on x-axis, node labels */
static void draw_graph_variables(int wcnt, int wave_color, int n_nodes, int sweep_idx,
        int flags, const char *ntok, const char *stok, const char *bus_msb, Graph_ctx *gr)
{
  char tmpstr[1024];

  /* clipping everything outside container area */
  bbox(START, 0.0, 0.0, 0.0, 0.0);
  bbox(ADD, gr->rx1, gr->ry1, gr->rx2, gr->ry2);
  bbox(SET_INSIDE, 0.0, 0.0, 0.0, 0.0);
  /* draw sweep variable(s) on x-axis */
  if(wcnt == 0 || (stok && stok[0])) {
    if(xctx->graph_values) stok = xctx->graph_names[sweep_idx];
    if(gr->unitx != 1.0) my_snprintf(tmpstr, S(tmpstr), "%s[%c]", stok ? stok : "" , gr->unitx_suffix);
    else  my_snprintf(tmpstr, S(tmpstr), "%s", stok ? stok : "");
    draw_string(wave_color, NOW, tmpstr, 2, 1, 0, 0,
       gr->rx1 + 2 + gr->rw / n_nodes * wcnt, gr->ry2-1, gr->txtsizelab, gr->txtsizelab);
  }
  /* draw node labels in graph */
  if(bus_msb) {
    if(gr->unity != 1.0) my_snprintf(tmpstr, S(tmpstr), "%s[%c]", find_nth(ntok, ";,", 1), gr->unity_suffix);
    else  my_snprintf(tmpstr, S(tmpstr), "%s",find_nth(ntok, ";,", 1));
  } else {
    char *ntok_ptr = NULL;
    char *alias_ptr = NULL;
    if(strstr(ntok, ";")) {
       my_strdup2(646, &alias_ptr, find_nth(ntok, ";", 1));
       my_strdup2(665, &ntok_ptr, find_nth(ntok, ";", 2));
    }
    else {
       my_strdup2(925, &alias_ptr, ntok);
       my_strdup2(1155, &ntok_ptr, ntok);
    }
      
    if(gr->unity != 1.0) my_snprintf(tmpstr, S(tmpstr), "%s[%c]", alias_ptr, gr->unity_suffix);
    else  my_snprintf(tmpstr, S(tmpstr), "%s", alias_ptr);
    my_free(1188, &alias_ptr);
    my_free(1189, &ntok_ptr);
  }
  if(gr->digital) {
    double xt = gr->x1 - 15 * gr->txtsizelab;
    double s1 = DIG_NWAVES; /* 1/DIG_NWAVES  waveforms fit in graph if unscaled vertically */
    double s2 = DIG_SPACE; /* (DIG_NWAVES - DIG_SPACE) spacing between traces */
    double yt;
    if(flags & 2)  /* cursor1 with measures */
      yt = s1 * (double)(n_nodes - wcnt) * gr->gh + gr->gh * 0.4 * s2;
    else
      yt = s1 * (double)(n_nodes - wcnt) * gr->gh + gr->gh * 0.1 * s2;

    if(yt <= gr->ypos2 && yt >= gr->ypos1) {
      #if HAS_CAIRO == 1
      if(gr->hilight_wave == wcnt) {
        cairo_select_font_face(xctx->cairo_ctx, "Sans-Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_select_font_face(xctx->cairo_save_ctx, "Sans-Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
      }
      #endif
      draw_string(wave_color, NOW, tmpstr, 2, 0, 0, 0,
        xt, DW_Y(yt), gr->digtxtsizelab, gr->digtxtsizelab);
      #if HAS_CAIRO == 1
      if(gr->hilight_wave == wcnt) {
        cairo_select_font_face(xctx->cairo_ctx, "Sans-Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_select_font_face(xctx->cairo_save_ctx, "Sans-Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      }
      #endif
    }
  } else {
    #if HAS_CAIRO == 1
    if(gr->hilight_wave == wcnt) {
      cairo_select_font_face(xctx->cairo_ctx, "Sans-Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
      cairo_select_font_face(xctx->cairo_save_ctx, "Sans-Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    }
    #endif
    draw_string(wave_color, NOW, tmpstr, 0, 0, 0, 0,
        gr->rx1 + 2 + gr->rw / n_nodes * wcnt, gr->ry1, gr->txtsizelab, gr->txtsizelab);
    #if HAS_CAIRO == 1
    if(gr->hilight_wave == wcnt) {
      cairo_select_font_face(xctx->cairo_ctx, "Sans-Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_select_font_face(xctx->cairo_save_ctx, "Sans-Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    }
    #endif
  }
  bbox(END, 0.0, 0.0, 0.0, 0.0);
}

static void show_node_measures(int measure_p, double measure_x, double measure_prev_x,
       const char *bus_msb, int wave_color, int idx, SPICE_DATA **idx_arr,
       int n_bits, int n_nodes, const char *ntok, int wcnt, Graph_ctx *gr)
{
  char tmpstr[1024];
  double yy;
  /* show values of signals if cursor1 active */
  if(idx == -1) return;
  if(measure_p >= 0) {
   
    /* draw node values in graph */
    bbox(START, 0.0, 0.0, 0.0, 0.0);
    bbox(ADD, gr->rx1, gr->ry1, gr->rx2, gr->ry2);
    bbox(SET_INSIDE, 0.0, 0.0, 0.0, 0.0);
    if(!bus_msb) {
      double diffy;
      double diffx;
      char *fmt1, *fmt2;
      double yy1;
      yy1 = xctx->graph_values[idx][measure_p-1];
      diffy = xctx->graph_values[idx][measure_p] - yy1;
      diffx = measure_x - measure_prev_x;
      yy = yy1 + diffy / diffx * (xctx->graph_cursor1_x - measure_prev_x);
      if(XSIGN0(gr->gy1) != XSIGN0(gr->gy2) && fabs(yy) < 1e-4 * fabs(gr->gh)) yy = 0.0;
      if(yy != 0.0  && fabs(yy * gr->unity) < 1.0e-3) {
        fmt1="%.2e";
        fmt2="%.2e%c";
      } else {
        fmt1="%.4g";
        fmt2="%.4g%c";
      }
      if(gr->unity != 1.0) my_snprintf(tmpstr, S(tmpstr), fmt2, yy * gr->unity, gr->unity_suffix);
      else  my_snprintf(tmpstr, S(tmpstr), fmt1, yy);
    } else {
      double vthl, vthh;
      int hex_digits = ((n_bits - 1) >> 2) + 1;
      vthh = gr->gy1 * 0.2 + gr->gy2 * 0.8;
      vthl = gr->gy1 * 0.8 + gr->gy2 * 0.2;
      get_bus_value(n_bits, hex_digits, idx_arr, measure_p - 1, tmpstr, vthl, vthh);
    }
    if(!bus_msb && !gr->digital) {
      draw_string(wave_color, NOW, tmpstr, 0, 0, 0, 0, 
         gr->rx1 + 2 + gr->rw / n_nodes * wcnt, gr->ry1 + gr->txtsizelab * 60,
          gr->txtsizelab * 0.8, gr->txtsizelab * 0.8);
      dbg(1, "node: %s, x=%g, value=%g\n", ntok, measure_x, yy);
    }
    else if(gr->digital) {
      double xt = gr->x1 - 15 * gr->txtsizelab;
      double s1 = DIG_NWAVES; /* 1/DIG_NWAVES  waveforms fit in graph if unscaled vertically */
      double s2 = DIG_SPACE; /* (DIG_NWAVES - DIG_SPACE) spacing between traces */
      double yt = s1 * (double)(n_nodes - wcnt) * gr->gh + gr->gh * 0.4 * s2;
      if(yt <= gr->ypos2 && yt >= gr->ypos1) {
        draw_string(wave_color, NOW, tmpstr, 2, 0, 0, 0,
           xt, DW_Y(yt) + gr->digtxtsizelab * 50, gr->digtxtsizelab * 0.8, gr->digtxtsizelab * 0.8);
      }
    }
    bbox(END, 0.0, 0.0, 0.0, 0.0);
  } /* if(measure_p >= 0) */
}

int embed_rawfile(const char *rawfile)
{
  int res = 0;
  size_t len;
  char *ptr;
  
  if(xctx->lastsel==1 && xctx->sel_array[0].type==ELEMENT) {
    xInstance *i = &xctx->inst[xctx->sel_array[0].n];
    xctx->push_undo();
    set_modify(1);
    ptr = base64_from_file(rawfile, &len);
    my_strdup2(1466, &i->prop_ptr, subst_token(i->prop_ptr, "spice_data", ptr));
    my_free(1481, &ptr);
  }
  return res;
}

int read_embedded_rawfile(void)
{
  int res = 0;

  if(xctx->lastsel==1 && xctx->sel_array[0].type==ELEMENT) {
    xInstance *i = &xctx->inst[xctx->sel_array[0].n];
    const char *b64_spice_data;
    if(i->prop_ptr && (b64_spice_data = get_tok_value(i->prop_ptr, "spice_data", 0))[0]) {
      res = read_rawfile_from_attr(b64_spice_data, strlen(b64_spice_data));
    }
  }
  return res;
}

/* when double clicking in a graph if this happens on a wave label
 * what == 1: 
 *   look up the wave and call tcl "graph_edit_wave <graph> <wave>"
 *   with graph index and wave index
 * what == 2: 
 *   look up the wave and draw in bold
 * return 1 if a wave was found
 */
int edit_wave_attributes(int what, int i, Graph_ctx *gr)
{
  char *node = NULL, *color = NULL, *sweep = NULL;
  int sweep_idx = 0;
  int n_nodes; /* number of variables to display in a single graph */
  char *saven, *savec, *saves, *nptr, *cptr, *sptr;
  const char *ntok, *ctok, *stok;
  int wcnt = 0, ret = 0;
  xRect *r = &xctx->rect[GRIDLAYER][i];

  /* get plot data */
  my_strdup2(1491, &node, get_tok_value(r->prop_ptr,"node",0));
  my_strdup2(1492, &color, get_tok_value(r->prop_ptr,"color",0)); 
  my_strdup2(1493, &sweep, get_tok_value(r->prop_ptr,"sweep",0)); 
  nptr = node;
  cptr = color;
  sptr = sweep;
  n_nodes = count_items(node, " \t\n", "\"");
  /* process each node given in "node" attribute, get also associated color/sweep var if any */
  while( (ntok = my_strtok_r(nptr, "\n\t ", "\"", &saven)) ) {
    ctok = my_strtok_r(cptr, " ", "", &savec);
    stok = my_strtok_r(sptr, "\t\n ", "\"", &saves);
    nptr = cptr = sptr = NULL;
    dbg(1, "ntok=%s ctok=%s\n", ntok, ctok? ctok: "NULL");
    if(stok && stok[0]) {
      sweep_idx = get_raw_index(stok);
      if( sweep_idx == -1) sweep_idx = 0;
    }
    if(gr->digital) {
      double xt1 = gr->rx1; /* <-- waves_selected() is more restrictive than this */
      double xt2 = gr->x1 - 20 * gr->txtsizelab;
      double s1 = DIG_NWAVES; /* 1/DIG_NWAVES  waveforms fit in graph if unscaled vertically */
      double s2 = DIG_SPACE; /* (DIG_NWAVES - DIG_SPACE) spacing between traces */
      double yt1 = s1 * (double)(n_nodes - wcnt) * gr->gh - gr->gy1 * s2;
      double yt2 = yt1 + s1 * gr->gh;
      if(yt1 <= gr->ypos2 && yt1 >= gr->ypos1) {
        double tmp = DW_Y(yt1);
        yt1 = DW_Y(yt2);
        yt2 = tmp;
        if(POINTINSIDE(xctx->mousex_snap, xctx->mousey_snap, xt1, yt1, xt2, yt2)) {
          char s[30];
          ret = 1;
          if(what == 1) {
            int save = gr->hilight_wave;
            my_snprintf(s, S(s), "%d %d", i, wcnt);
            gr->hilight_wave = wcnt;
            tclvareval("graph_edit_wave ", s, NULL);
            gr->hilight_wave = save;
          } else {
             if(gr->hilight_wave == wcnt) {
               gr->hilight_wave = -1;
               my_strdup2(1522, &r->prop_ptr, subst_token(r->prop_ptr, "hilight_wave", my_itoa(gr->hilight_wave)));
             } else {
               gr->hilight_wave = wcnt;
               my_strdup2(1525, &r->prop_ptr, subst_token(r->prop_ptr, "hilight_wave", my_itoa(gr->hilight_wave)));
             }
          }
        }
      }
    } else {
      double xt1 = gr->rx1 + 2 + gr->rw / n_nodes * wcnt;
      double yt1 = gr->ry1;
      double xt2 = xt1 + gr->rw / n_nodes;
      double yt2 = gr->y1;
      if(POINTINSIDE(xctx->mousex_snap, xctx->mousey_snap, xt1, yt1, xt2, yt2)) {
        char s[50];
        ret = 1;
        if(what == 1) {
          int save = gr->hilight_wave;
          my_snprintf(s, S(s), "%d %d", i, wcnt);
          gr->hilight_wave = wcnt;
          tclvareval("graph_edit_wave ", s, NULL);
          gr->hilight_wave = save;
        } else {
          if(gr->hilight_wave == wcnt) {
            gr->hilight_wave = -1;
            my_strdup2(1538, &r->prop_ptr, subst_token(r->prop_ptr, "hilight_wave", my_itoa(gr->hilight_wave)));
          } else {
            gr->hilight_wave = wcnt;
            my_strdup2(1539, &r->prop_ptr, subst_token(r->prop_ptr, "hilight_wave", my_itoa(gr->hilight_wave)));
          }
        }
      }
    }
    wcnt++;
  } /* while( (ntok = my_strtok_r(nptr, "\n\t ", "", &saven)) ) */
  my_free(1494, &node);
  my_free(1495, &color);
  my_free(1496, &sweep);
  return ret;
}

/* derived from draw_graph(), calculate y range of custom equation graph data,
 * handling multiple datasets ad wraps (as in multi-sweep DC sims).
 */
int calc_custom_data_yrange(int sweep_idx, const char *express, Graph_ctx *gr)
{
  int idx = -1;
  int p, dset, ofs;
  int first, last;
  double xx; /* the p-th sweep variable value:  xctx->graph_values[sweep_idx][p] */
  double start;
  double end;
  int sweepvar_wrap = 0; /* incremented on new dataset or sweep variable wrap */
  int dataset = gr->dataset;
  ofs = 0;
  start = (gr->gx1 <= gr->gx2) ? gr->gx1 : gr->gx2;
  end = (gr->gx1 <= gr->gx2) ? gr->gx2 : gr->gx1;
  for(dset = 0 ; dset < xctx->graph_datasets; dset++) {
    double prev_x, prev_prev_x;
    int cnt=0, wrap;
    register SPICE_DATA *gv = xctx->graph_values[sweep_idx];
    first = -1;
    prev_prev_x = prev_x = 0;
    last = ofs; 
    for(p = ofs ; p < ofs + xctx->graph_npoints[dset]; p++) {
      if(gr->logx) 
        xx = mylog10(gv[p]);
      else
        xx = gv[p];
      wrap = (sweep_idx == 0 && cnt > 1 && XSIGN(xx - prev_x) != XSIGN(prev_x - prev_prev_x));
      if(first != -1) {                      /* there is something to plot ... */
        if(xx > end || xx < start ||         /* ... and we ran out of graph area ... */
          wrap) {                          /* ... or sweep variable changed direction */
          if(dataset == -1 || dataset == sweepvar_wrap) {
            idx = plot_raw_custom_data(sweep_idx, first, last, express);
          }
          first = -1;
        }
      }
      if(wrap) {
         sweepvar_wrap++;
         cnt = 0;
      }
      if(xx >= start && xx <= end) {
        if(first == -1) first = p;
        /* Build poly x array. Translate from graph coordinates to screen coords */
        if(dataset == -1 || dataset == sweepvar_wrap) {
        } /* if(dataset == -1 || dataset == sweepvar_wrap) */
        last = p;
        cnt++;
      } /* if(xx >= start && xx <= end) */
      prev_prev_x = prev_x;
      prev_x = xx;
    } /* for(p = ofs ; p < ofs + xctx->graph_npoints[dset]; p++) */
    if(first != -1) {
      if(dataset == -1 || dataset == sweepvar_wrap) {
        idx = plot_raw_custom_data(sweep_idx, first, last, express);
      }
    }
    /* offset pointing to next dataset */
    ofs += xctx->graph_npoints[dset];
    sweepvar_wrap++;
  } /* for(dset...) */
  return idx;
}

int find_closest_wave(int i, Graph_ctx *gr)
{
  double xval, yval;
  char *node = NULL, *sweep = NULL;
  int sweep_idx = 0;
  char *saven, *saves, *nptr, *sptr;
  const char *ntok, *stok;
  int wcnt = 0, idx, expression;
  char *express = NULL;
  xRect *r = &xctx->rect[GRIDLAYER][i];
  int closest_dataset = -1;
  double min=-1.0;
  
  if(gr->digital) return -1;

  yval = G_Y(xctx->mousey);
  xval = G_X(xctx->mousex);
  if(gr->logx) xval = pow(10, xval);
  if(gr->logy) yval = pow(10, yval);
  dbg(0, "x=%g y=%g\n", xval, yval);
  /* get data to plot */
  my_strdup2(474, &node, get_tok_value(r->prop_ptr,"node",0));
  my_strdup2(1012, &sweep, get_tok_value(r->prop_ptr,"sweep",0)); 
  nptr = node;
  sptr = sweep;
  /* process each node given in "node" attribute, get also associated sweep var if any*/
  while( (ntok = my_strtok_r(nptr, "\n\t ", "\"", &saven)) ) {
    if(strstr(ntok, ",")) {
      if(find_nth(ntok, ";,", 2)[0]) continue; /* bus signal: skip */
    }
    stok = my_strtok_r(sptr, "\t\n ", "\"", &saves);
    nptr = sptr = NULL;
    dbg(1, "ntok=%s\n", ntok);
    if(stok && stok[0]) {
      sweep_idx = get_raw_index(stok);
      if( sweep_idx == -1) {
        sweep_idx = 0;
      }
    }
    /* if ntok following possible 'alias;' definition contains spaces --> custom data plot */
    idx = -1;
    expression = 0;
    if(xctx->graph_values) {
      if(strstr(ntok, ";")) {
        my_strdup2(1191, &express, find_nth(ntok, ";", 2));
      } else {
        my_strdup2(1192, &express, ntok);
      }
      if(strstr(express, " ")) {
        expression = 1;
      }
    }
    if(expression) idx = plot_raw_custom_data(sweep_idx, 0, xctx->graph_allpoints-1, express);
    else idx = get_raw_index(express);
    if( idx != -1 ) {
      int p, dset, ofs;
      int first, last;
      double xx, yy ; /* the p-th point */
      double start;
      double end;
      int sweepvar_wrap = 0; /* incremented on new dataset or sweep variable wrap */
      ofs = 0;
      start = (gr->gx1 <= gr->gx2) ? gr->gx1 : gr->gx2;
      end = (gr->gx1 <= gr->gx2) ? gr->gx2 : gr->gx1;
      /* loop through all datasets found in raw file */
      for(dset = 0 ; dset < xctx->graph_datasets; dset++) {
        double prev_x, prev_prev_x;
        int cnt=0, wrap;
        register SPICE_DATA *gvx = xctx->graph_values[sweep_idx];
        register SPICE_DATA *gvy = xctx->graph_values[idx];
        first = -1;
        /* Process "npoints" simulation items 
         * p loop split repeated 2 timed (for x and y points) to preserve cache locality */
        prev_prev_x = prev_x = 0;
        last = ofs; 
        for(p = ofs ; p < ofs + xctx->graph_npoints[dset]; p++) {
          if(gr->logx) xx = mylog10(gvx[p]);
          else  xx = gvx[p];
          if(gr->logy) yy = mylog10(gvy[p]);
          else  yy = gvy[p];
          wrap = (sweep_idx == 0 && cnt > 1 && XSIGN(xx - prev_x) != XSIGN(prev_x - prev_prev_x));
          if(first != -1) {
            if(xx > end || xx < start || wrap) {
              dbg(1, "find_closest_wave(): last=%d\n", last);
              first = -1;
            }
          }
          if(wrap) {
             cnt = 0;
             sweepvar_wrap++;
          }
          if(xx >= start && xx <= end) {
            if(first == -1) first = p;
            if( XSIGN(xval - xx) != XSIGN(xval - prev_x)) {

               if(min < 0.0) {
                  min = fabs(yval - yy);
                  closest_dataset = sweepvar_wrap;
               } else {
                 double tmp = fabs(yval - yy);
                 if(tmp < min) {
                   min = tmp;
                   closest_dataset = sweepvar_wrap;
                 }
               }
               dbg(1, "find_closest_wave(): xval=%g yval=%g xx=%g yy=%g sweepvar_wrap=%d ntok=%s stok=%s\n",
                   xval, yval, xx, yy, sweepvar_wrap, ntok, stok? stok : "<NULL>");
            }
            last = p;
            cnt++;
          } /* if(xx >= start && xx <= end) */
          prev_prev_x = prev_x;
          prev_x = xx;
        } /* for(p = ofs ; p < ofs + xctx->graph_npoints[dset]; p++) */
        /* offset pointing to next dataset */
        ofs += xctx->graph_npoints[dset];
        sweepvar_wrap++;
      } /* for(dset...) */

    } /*  if( (idx = get_raw_index(ntok)) != -1 ) */
    wcnt++;
  } /* while( (ntok = my_strtok_r(nptr, "\n\t ", "", &saven)) ) */
  dbg(0, "closest dataset=%d\n", closest_dataset);
  if(express) my_free(1487, &express);
  my_free(478, &node);
  my_free(1262, &sweep);
  return closest_dataset;
}


/* flags:
 * 1: do final XCopyArea (copy 2nd buffer areas to screen) 
 *    If draw_graph_all() is called from draw() no need to do XCopyArea, as draw() does it already.
 *    This makes drawing faster and removes a 'tearing' effect when moving around.
 * 2: draw x-cursor1
 * 4: draw x-cursor2
 * 8: all drawing, if not set do only XCopyArea / x-cursor if specified
 */
void draw_graph(int i, const int flags, Graph_ctx *gr)
{
  int wave_color = 4;
  char *node = NULL, *color = NULL, *sweep = NULL;
  int sweep_idx = 0;
  int n_nodes; /* number of variables to display in a single graph */
  char *saven, *savec, *saves, *nptr, *cptr, *sptr;
  const char *ntok, *ctok, *stok;
  char *bus_msb = NULL;
  int wcnt = 0, idx, expression;
  int measure_p = -1;
  double measure_x;
  double measure_prev_x;
  char *express = NULL;
  xRect *r = &xctx->rect[GRIDLAYER][i];
  
  if(RECT_OUTSIDE( gr->sx1, gr->sy1, gr->sx2, gr->sy2,
      xctx->areax1, xctx->areay1, xctx->areax2, xctx->areay2)) return;
  /*
   * dbg(0, "draw_graph(): window: %d %d %d %d\n", xctx->areax1, xctx->areay1, xctx->areax2, xctx->areay2);
   * dbg(0, "draw_graph(): graph: %g %g %g %g\n", gr->sx1, gr->sy1, gr->sx2, gr->sy2);
   * dbg(0, "draw_graph(): i = %d, flags = %d\n", i, flags);
   */
  /* draw stuff */
  if(flags & 8) {
#if !defined(__unix__) && defined(HAS_CAIRO)
    double sw = (gr->sx2 - gr->sx1);
    double sh = (gr->sy2 - gr->sy1);
    clear_cairo_surface(xctx->cairo_save_ctx, gr->sx1, gr->sy1, sw, sh);
    clear_cairo_surface(xctx->cairo_ctx, gr->sx1, gr->sy1, sw, sh);
#endif
    /* graph box, gridlines and axes */
    draw_graph_grid(gr);
    /* get data to plot */
    my_strdup2(1389, &node, get_tok_value(r->prop_ptr,"node",0));
    my_strdup2(1390, &color, get_tok_value(r->prop_ptr,"color",0)); 
    my_strdup2(1407, &sweep, get_tok_value(r->prop_ptr,"sweep",0)); 
    nptr = node;
    cptr = color;
    sptr = sweep;
    n_nodes = count_items(node, " \t\n", "\"");
    /* process each node given in "node" attribute, get also associated color/sweep var if any*/
    while( (ntok = my_strtok_r(nptr, "\n\t ", "\"", &saven)) ) {
      if(strstr(ntok, ",")) {
        my_strdup2(1452, &bus_msb, find_nth(ntok, ";,", 2));
      }
      ctok = my_strtok_r(cptr, " ", "", &savec);
      stok = my_strtok_r(sptr, "\t\n ", "\"", &saves);
      nptr = cptr = sptr = NULL;
      dbg(1, "ntok=%s ctok=%s\n", ntok, ctok? ctok: "NULL");
      if(ctok && ctok[0]) wave_color = atoi(ctok);
      if(wave_color < 0) wave_color = 4;
      if(wave_color >= cadlayers) wave_color = cadlayers - 1;
      if(stok && stok[0]) {
        sweep_idx = get_raw_index(stok);
        if( sweep_idx == -1) {
          sweep_idx = 0;
        }
      }
      draw_graph_variables(wcnt, wave_color, n_nodes, sweep_idx, flags, ntok, stok, bus_msb, gr);
      /* if ntok following possible 'alias;' definition contains spaces --> custom data plot */
      idx = -1;
      expression = 0;
      if(xctx->graph_values && !bus_msb) {
        if(strstr(ntok, ";")) {
          my_strdup2(460, &express, find_nth(ntok, ";", 2));
        } else {
          my_strdup2(473, &express, ntok);
        }
        if(strstr(express, " ")) {
          expression = 1;
        }
      }
      /* quickly find index number of ntok variable to be plotted */
      if( expression || (idx = get_raw_index(bus_msb ? bus_msb : express)) != -1 ) {
        int p, dset, ofs;
        int poly_npoints;
        int first, last;
        double xx; /* the p-th sweep variable value:  xctx->graph_values[sweep_idx][p] */
        double start;
        double end;
        int n_bits = 1; 
        SPICE_DATA **idx_arr = NULL;
        int sweepvar_wrap = 0; /* incremented on new dataset or sweep variable wrap */
        XPoint *point = NULL;
        int dataset = gr->dataset;
        int digital = gr->digital;
        ofs = 0;
        start = (gr->gx1 <= gr->gx2) ? gr->gx1 : gr->gx2;
        end = (gr->gx1 <= gr->gx2) ? gr->gx2 : gr->gx1;
        if(bus_msb) {
          idx_arr = get_bus_idx_array(ntok, &n_bits); /* idx_arr allocated by function, must free! */
        }
        bbox(START, 0.0, 0.0, 0.0, 0.0);
        bbox(ADD,gr->x1, gr->y1, gr->x2, gr->y2);
        bbox(SET, 0.0, 0.0, 0.0, 0.0);
        /* loop through all datasets found in raw file */
        for(dset = 0 ; dset < xctx->graph_datasets; dset++) {
          double prev_x, prev_prev_x;
          int cnt=0, wrap;
          register SPICE_DATA *gv = xctx->graph_values[sweep_idx];
  
          first = -1;
          poly_npoints = 0;
          my_realloc(1401, &point, xctx->graph_npoints[dset] * sizeof(XPoint));
          /* Process "npoints" simulation items 
           * p loop split repeated 2 timed (for x and y points) to preserve cache locality */
          prev_prev_x = prev_x = 0;
          last = ofs; 
          for(p = ofs ; p < ofs + xctx->graph_npoints[dset]; p++) {
            if(gr->logx) xx = mylog10(gv[p]);
            else  xx = gv[p];
            wrap = (sweep_idx == 0 && cnt > 1 && XSIGN(xx - prev_x) != XSIGN(prev_x - prev_prev_x));
            if(first != -1) {                      /* there is something to plot ... */
              if(xx > end || xx < start ||         /* ... and we ran out of graph area ... */
                wrap) {                          /* ... or sweep variable changed direction */
                if(dataset == -1 || dataset == sweepvar_wrap) {
                  /* plot graph */
                  if(bus_msb) {
                    if(digital) {
                      draw_graph_bus_points(ntok, n_bits, idx_arr, first, last, wave_color,
                                   sweep_idx, wcnt, n_nodes, gr);
                    }
                  } else {
                    if(expression) idx = plot_raw_custom_data(sweep_idx, first, last, express);
                    draw_graph_points(idx, first, last, point, wave_color, wcnt, n_nodes, gr);
                  }
                }
                poly_npoints = 0;
                first = -1;
              }
            }
            if(wrap) {
               sweepvar_wrap++;
               cnt = 0;
            }
            if(xx >= start && xx <= end) {
              if(first == -1) first = p;
              /* Build poly x array. Translate from graph coordinates to screen coords */
              point[poly_npoints].x = (short)S_X(xx);
              if(dataset == -1 || dataset == sweepvar_wrap) {
                if(measure_p == -1 && flags & 2 && cnt) { /* cursor1: show measurements on nodes in graph */
                  if(XSIGN(xx - xctx->graph_cursor1_x) != XSIGN(prev_x - xctx->graph_cursor1_x)) {
                    measure_p = p;
                    measure_x = xx;
                    measure_prev_x = prev_x;
                  }
                } /* if(measure_p == -1 && flags & 2 && p > ofs) */
              } /* if(dataset == -1 || dataset == sweepvar_wrap) */
              last = p;
              poly_npoints++;
              cnt++;
            } /* if(xx >= start && xx <= end) */
            prev_prev_x = prev_x;
            prev_x = xx;
          } /* for(p = ofs ; p < ofs + xctx->graph_npoints[dset]; p++) */
          if(first != -1) {
            if(dataset == -1 || dataset == sweepvar_wrap) {
              /* plot graph. Bus bundles are not plotted if graph is not digital.*/
              if(bus_msb) {
                if(digital) {
                  draw_graph_bus_points(ntok, n_bits, idx_arr, first, last, wave_color,
                               sweep_idx, wcnt, n_nodes, gr);
                }
              } else {
                if(expression) idx = plot_raw_custom_data(sweep_idx, first, last, express);
                draw_graph_points(idx, first, last, point, wave_color, wcnt, n_nodes, gr);
              }
            }
          }
          /* offset pointing to next dataset */
          ofs += xctx->graph_npoints[dset];
          sweepvar_wrap++;
        } /* for(dset...) */
        bbox(END, 0.0, 0.0, 0.0, 0.0);
        if(measure_p != -1) show_node_measures(measure_p, measure_x, measure_prev_x, bus_msb, wave_color, 
            idx, idx_arr, n_bits, n_nodes, ntok, wcnt, gr);

        my_free(1403, &point);
        if(idx_arr) my_free(1455, &idx_arr);
      } /* if( expression || (idx = get_raw_index(bus_msb ? bus_msb : express)) != -1 ) */
      wcnt++;
      if(bus_msb) my_free(1453, &bus_msb);
    } /* while( (ntok = my_strtok_r(nptr, "\n\t ", "", &saven)) ) */
    if(express) my_free(1520, &express);
    my_free(1391, &node);
    my_free(1392, &color);
    my_free(1408, &sweep);
  } /* if(flags & 8) */
  bbox(START, 0.0, 0.0, 0.0, 0.0);
  bbox(ADD, gr->rx1, gr->ry1, gr->rx2, gr->ry2);
  bbox(SET_INSIDE, 0.0, 0.0, 0.0, 0.0);
  if(flags & 8) {
    /* cursor1 */
    if((flags & 2)) draw_cursor(xctx->graph_cursor1_x, xctx->graph_cursor2_x, 1, gr);
    /* cursor2 */
    if((flags & 4)) draw_cursor(xctx->graph_cursor2_x, xctx->graph_cursor1_x, 3, gr);
    /* difference between cursors */
    if((flags & 2) && (flags & 4)) draw_cursor_difference(gr);
  }
  if(flags & 1) { /* copy save buffer to screen */
    if(!xctx->draw_window) {
      MyXCopyArea(display, xctx->save_pixmap, xctx->window, xctx->gctiled, xctx->xrect[0].x, xctx->xrect[0].y,
         xctx->xrect[0].width, xctx->xrect[0].height, xctx->xrect[0].x, xctx->xrect[0].y);
    }
  }
  bbox(END, 0.0, 0.0, 0.0, 0.0);
}

/* flags:
 * see draw_graph()
 */
static void draw_graph_all(int flags)
{
  int  i, sch_loaded, hide_graphs;
  int bbox_set = 0;
  const char *tmp;
  int save_bbx1, save_bby1, save_bbx2, save_bby2;
  dbg(1, "draw_graph_all(): flags=%d\n", flags);
  /* save bbox data, since draw_graph_all() is called from draw() which may be called after a bbox(SET) */
  sch_loaded = schematic_waves_loaded();
  tmp =  tclgetvar("hide_empty_graphs");
  hide_graphs = (tmp && tmp[0] == '1') ? 1 : 0;
  if(sch_loaded || !hide_graphs) {
    if(xctx->sem) {
      bbox_set = 1;
      save_bbx1 = xctx->bbx1;
      save_bby1 = xctx->bby1;
      save_bbx2 = xctx->bbx2;
      save_bby2 = xctx->bby2;
      bbox(END, 0.0, 0.0, 0.0, 0.0);
    }
    #if HAS_CAIRO==1
    cairo_save(xctx->cairo_ctx);
    cairo_save(xctx->cairo_save_ctx);
    cairo_select_font_face(xctx->cairo_ctx, "Sans-Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_select_font_face(xctx->cairo_save_ctx, "Sans-Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    #endif
    if(xctx->draw_single_layer==-1 || GRIDLAYER == xctx->draw_single_layer) {
      if(xctx->enable_layer[GRIDLAYER]) for(i = 0; i < xctx->rects[GRIDLAYER]; i++) {
        xRect *r = &xctx->rect[GRIDLAYER][i];
        if(r->flags & 1) {
          setup_graph_data(i, flags, 0, &xctx->graph_struct);
          draw_graph(i, flags, &xctx->graph_struct); /* draw data in each graph box */
        }
      }
    }
    #if HAS_CAIRO==1
    cairo_restore(xctx->cairo_ctx);
    cairo_restore(xctx->cairo_save_ctx);
    #endif
    /* restore previous bbox */
    if(bbox_set) {
      xctx->bbx1 = save_bbx1;
      xctx->bby1 = save_bby1;
      xctx->bbx2 = save_bbx2;
      xctx->bby2 = save_bby2;
      xctx->sem = 1;
      bbox(SET, 0.0, 0.0, 0.0, 0.0);
    }
  }
}

#if HAS_CAIRO==1
typedef struct
{
  unsigned char *buffer;
  size_t pos;
  size_t size;
} png_to_byte_closure_t;

static cairo_status_t png_reader(void *in_closure, unsigned char *out_data, unsigned int length)
{
  png_to_byte_closure_t *closure = (png_to_byte_closure_t *) in_closure;
  if(!closure->buffer) return CAIRO_STATUS_READ_ERROR;
  memcpy(out_data, closure->buffer + closure->pos, length);
  closure->pos += length;
  return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t png_writer(void *in_closure, const unsigned char *in_data, unsigned int length)
{
  png_to_byte_closure_t *closure = (png_to_byte_closure_t *) in_closure;
  if(!in_data) return CAIRO_STATUS_WRITE_ERROR;
  if(closure->pos + length > closure->size) {
    my_realloc(1472, &closure->buffer, closure->pos + length + 65536);
    closure->size =  closure->pos + length + 65536;
  }
  memcpy(closure->buffer + closure->pos, in_data, length);
  closure->pos += length;
  return CAIRO_STATUS_SUCCESS;
}
#endif


/* rot and flip for rotated / flipped symbols
 * draw: 1 draw image
 *       0 only load image and build base64 
 */
void draw_image(int draw, xRect *r, double *x1, double *y1, double *x2, double *y2, int rot, int flip)
{
  #if HAS_CAIRO == 1
  const char *ptr;
  int w,h;
  double x, y, rw, rh;
  double sx1, sy1, sx2, sy2, alpha;
  char filename[PATH_MAX];
  struct stat buf;
  const char *attr ;
  char *filter = NULL;
  double xx1, yy1, scalex, scaley;
  png_to_byte_closure_t closure;
  xEmb_image *emb_ptr;

  xx1 = *x1; yy1 = *y1; /* image anchor point */
  RECTORDER(*x1, *y1, *x2, *y2);

  /* screen position */
  sx1=X_TO_SCREEN(*x1);
  sy1=Y_TO_SCREEN(*y1);
  sx2=X_TO_SCREEN(*x2);
  sy2=Y_TO_SCREEN(*y2);
  if(RECT_OUTSIDE(sx1, sy1, sx2, sy2,
                  xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2)) return;
  set_rect_extraptr(1, r); /* create r->extraptr pointing to a xEmb_image struct */
  emb_ptr = r->extraptr;
  if(draw) {
    cairo_save(xctx->cairo_ctx);
    cairo_save(xctx->cairo_save_ctx);
  }
  my_strncpy(filename, get_tok_value(r->prop_ptr, "image", 0), S(filename));
  my_strdup(1484, &filter, get_tok_value(r->prop_ptr, "filter", 0));

  /* read PNG from in-memory buffer ... */
  if(emb_ptr && emb_ptr->image) {
    ; /* nothing to do, image is already created */
  /* ... or read PNG from image_data attribute */
  } else if( (attr = get_tok_value(r->prop_ptr, "image_data", 0))[0] ) {
    size_t data_size;
    if(filter) {
      size_t filtersize = 0;
      char *filterdata = NULL;
      closure.buffer = NULL;
      filterdata = (char *)base64_decode(attr, strlen(attr), &filtersize);
      filter_data(filterdata, filtersize, (char **)&closure.buffer, &data_size, filter); 
      my_free(1488, &filterdata);
    } else {
      closure.buffer = base64_decode(attr, strlen(attr), &data_size);
    }
    closure.pos = 0;
    closure.size = data_size; /* should not be necessary */
    emb_ptr->image = cairo_image_surface_create_from_png_stream(png_reader, &closure);
    if(closure.buffer == NULL) dbg(0, "draw_image(): image creation failed\n");
    my_free(1467, &closure.buffer);
    dbg(1, "draw_image(): length2 = %d\n", closure.pos);
  /* ... or read PNG from file (image attribute) */
  } else if(filename[0] && !stat(filename, &buf)) {
    char *image_data = NULL;
    size_t olength;
    if(filter) {
      size_t filtersize = 0;
      char *filterdata = NULL;
      size_t pngsize = 0;
      char *pngdata = NULL;
      FILE *fd;
      filtersize = (size_t)buf.st_size;
      if(filtersize) {
        fd = fopen(filename, "r");
        if(fd) {
          filterdata = my_malloc(1490, filtersize);
          fread(filterdata, filtersize, 1, fd);
          fclose(fd);
        }
      }
      filter_data(filterdata, filtersize, &pngdata, &pngsize, filter);
      closure.buffer = (unsigned char *)pngdata;
      closure.size = pngsize;
      closure.pos = 0;
      emb_ptr->image = cairo_image_surface_create_from_png_stream(png_reader, &closure);
      image_data = base64_encode((unsigned char *)filterdata, filtersize, &olength, 0);
      my_free(1489, &filterdata);
    } else {
      closure.buffer = NULL;
      closure.size = 0;
      closure.pos = 0;
      emb_ptr->image = cairo_image_surface_create_from_png(filename);
      /* write PNG to in-memory buffer */
      cairo_surface_write_to_png_stream(emb_ptr->image, png_writer, &closure);
      image_data = base64_encode(closure.buffer, closure.pos, &olength, 0);
      my_free(1468, &closure.buffer);
    }
    /* put base64 encoded data to rect image_data attrinute */
    my_strdup2(1473, &r->prop_ptr, subst_token(r->prop_ptr, "image_data", image_data));
    my_free(1474, &image_data);
    if(cairo_surface_status(emb_ptr->image) != CAIRO_STATUS_SUCCESS) {
      my_free(442, &filter);
      return;
    }
    dbg(1, "draw_image(): length3 = %d\n", closure.pos);
  } else {
    my_free(453, &filter);
    return;
  }
  if(cairo_surface_status(emb_ptr->image) != CAIRO_STATUS_SUCCESS) {
    my_free(434, &filter);
    return;
  }
  my_free(1485, &filter);
  ptr = get_tok_value(r->prop_ptr, "alpha", 0);
  alpha = 1.0;
  if(ptr[0]) alpha = atof(ptr);
  w = cairo_image_surface_get_width (emb_ptr->image);
  h = cairo_image_surface_get_height (emb_ptr->image);
  dbg(1, "draw_image() w=%d, h=%d\n", w, h);
  x = X_TO_SCREEN(xx1);
  y = Y_TO_SCREEN(yy1);
  if(r->flags & 2048) { /* resize container rectangle to fit image */
    *x2 = *x1 + w;
    *y2 = *y1 + h;
    scalex = xctx->mooz;
    scaley = xctx->mooz;
  } else { /* resize image to fit in rectangle */
    rw = abs((int)(*x2 - *x1));
    rh = abs((int)(*y2 - *y1));
    scalex = rw/w * xctx->mooz;
    scaley = rh/h * xctx->mooz;
  }
  if(draw && xctx->draw_pixmap) {
    cairo_translate(xctx->cairo_save_ctx, x, y);
    if(flip && (rot == 0 || rot == 2)) cairo_scale(xctx->cairo_save_ctx, -scalex, scaley);
    else if(flip && (rot == 1 || rot == 3)) cairo_scale(xctx->cairo_save_ctx, scalex, -scaley);
    else cairo_scale(xctx->cairo_save_ctx, scalex, scaley);
    cairo_rotate(xctx->cairo_save_ctx, rot * XSCH_PI * 0.5);
    cairo_set_source_surface(xctx->cairo_save_ctx, emb_ptr->image, 0. , 0.);
    cairo_rectangle(xctx->cairo_save_ctx, 0, 0, w , h );
    /* cairo_fill(xctx->cairo_save_ctx);
     * cairo_stroke(xctx->cairo_save_ctx); */
    cairo_clip(xctx->cairo_save_ctx);
    cairo_paint_with_alpha(xctx->cairo_save_ctx, alpha);
  }
  if(draw && xctx->draw_window) {
    cairo_translate(xctx->cairo_ctx, x, y);
    if(flip && (rot == 0 || rot == 2)) cairo_scale(xctx->cairo_ctx, -scalex, scaley);
    else if(flip && (rot == 1 || rot == 3)) cairo_scale(xctx->cairo_ctx, scalex, -scaley);
    else cairo_scale(xctx->cairo_ctx, scalex, scaley);
    cairo_rotate(xctx->cairo_ctx, rot * XSCH_PI * 0.5);
    cairo_set_source_surface(xctx->cairo_ctx, emb_ptr->image, 0. , 0.);
    cairo_rectangle(xctx->cairo_ctx, 0, 0, w , h );
    /* cairo_fill(xctx->cairo_ctx);
     * cairo_stroke(xctx->cairo_ctx); */
    cairo_clip(xctx->cairo_ctx);
    cairo_paint_with_alpha(xctx->cairo_ctx, alpha);
  }
  if(draw) {
    cairo_restore(xctx->cairo_ctx);
    cairo_restore(xctx->cairo_save_ctx);
  }
  #endif
}

static void draw_images_all(void)
{
  #if HAS_CAIRO==1
  int i;
  if(xctx->draw_single_layer==-1 || GRIDLAYER == xctx->draw_single_layer) {
    if(xctx->enable_layer[GRIDLAYER]) for(i = 0; i < xctx->rects[GRIDLAYER]; i++) {
      xRect *r = &xctx->rect[GRIDLAYER][i];
      if(r->flags & 1024) {
        draw_image(1, r, &r->x1, &r->y1, &r->x2, &r->y2, 0, 0);
      }
    }
  }
  #endif
}

void draw(void)
{
 /* inst_ptr  and wire hash iterator 20171224 */
 double x1, y1, x2, y2;
 Instentry *instanceptr;
 Wireentry *wireptr;
 int use_hash;
 int c, i = 0;
 xSymbol *symptr;
 int textlayer;
 int show_hidden_texts = tclgetboolvar("show_hidden_texts");

 #if HAS_CAIRO==1
 const char *textfont;
#ifndef __unix__
 clear_cairo_surface(xctx->cairo_save_ctx,
   xctx->xrect[0].x, xctx->xrect[0].y, xctx->xrect[0].width, xctx->xrect[0].height);
 clear_cairo_surface(xctx->cairo_ctx,
   xctx->xrect[0].x, xctx->xrect[0].y, xctx->xrect[0].width, xctx->xrect[0].height);
#endif
 #endif
 if(xctx->no_draw) return;
 rebuild_selected_array();
 if(has_x) {
    if(xctx->draw_pixmap)
      XFillRectangle(display, xctx->save_pixmap, xctx->gc[BACKLAYER], xctx->areax1, xctx->areay1,
                     xctx->areaw, xctx->areah);
    if(xctx->draw_window) 
      XFillRectangle(display, xctx->window, xctx->gc[BACKLAYER], xctx->areax1, xctx->areay1,
                     xctx->areaw, xctx->areah);
    dbg(1, "draw(): window: %d %d %d %d\n",xctx->areax1, xctx->areay1, xctx->areax2, xctx->areay2);
    drawgrid();
    draw_graph_all((xctx->graph_flags & 6) + 8); /* xctx->graph_flags for cursors */
    draw_images_all();
    x1 = X_TO_XSCHEM(xctx->areax1);
    y1 = Y_TO_XSCHEM(xctx->areay1);
    x2 = X_TO_XSCHEM(xctx->areax2);
    y2 = Y_TO_XSCHEM(xctx->areay2);
    use_hash =  (xctx->wires> 2000 || xctx->instances > 2000 ) &&  (x2 - x1  < ITERATOR_THRESHOLD);
    if(use_hash) {
      hash_instances();
      hash_wires();
    }
    if(!xctx->only_probes) {
        Iterator_ctx ctx;
        dbg(3, "draw(): check4\n");
        for(c=0;c<cadlayers;c++) {
          if(xctx->draw_single_layer!=-1 && c != xctx->draw_single_layer) continue;

          if(xctx->enable_layer[c]) for(i=0;i<xctx->lines[c];i++) {
            xLine *l = &xctx->line[c][i];
            if(l->bus) drawline(c, THICK, l->x1, l->y1, l->x2, l->y2, l->dash);
            else       drawline(c, ADD, l->x1, l->y1, l->x2, l->y2, l->dash);
          }
          if(xctx->enable_layer[c]) for(i=0;i<xctx->rects[c];i++) {
            xRect *r = &xctx->rect[c][i]; 
            #if HAS_CAIRO==1
            if(c != GRIDLAYER || !(r->flags & (1 + 1024)) )  {
            #else
            if(c != GRIDLAYER || !(r->flags & 1) )  {
            #endif
              drawrect(c, ADD, r->x1, r->y1, r->x2, r->y2, r->dash);
              filledrect(c, ADD, r->x1, r->y1, r->x2, r->y2);
            }
          }
          if(xctx->enable_layer[c]) for(i=0;i<xctx->arcs[c];i++) {
            xArc *a = &xctx->arc[c][i];
            drawarc(c, ADD, a->x, a->y, a->r, a->a, a->b, a->fill, a->dash);
          }
          if(xctx->enable_layer[c]) for(i=0;i<xctx->polygons[c];i++) {
            xPoly *p = &xctx->poly[c][i];
            drawpolygon(c, NOW, p->x, p->y, p->points, p->fill, p->dash);
          }
          if(use_hash) init_inst_iterator(&ctx, x1, y1, x2, y2);
          else i = -1;
          while(1) {
            if(use_hash) {
              if( !(instanceptr = inst_iterator_next(&ctx))) break;
              i = instanceptr->n;
            }
            else {
              i++;
              if(i >= xctx->instances) break;
            }
            if(xctx->inst[i].ptr == -1 || (c > 0 && (xctx->inst[i].flags & 1)) ) continue;
            symptr = (xctx->inst[i].ptr+ xctx->sym);
            if(
                c==0 || /*draw_symbol call is needed on layer 0 to avoid redundant work (outside check) */
                symptr->lines[c] ||
                symptr->arcs[c] ||
                symptr->rects[c] ||
                symptr->polygons[c] ||
                ((c==TEXTWIRELAYER || c==TEXTLAYER) && symptr->texts) )
            {
                draw_symbol(ADD, c, i,c,0,0,0.0,0.0);     /* ... then draw current layer      */
            }
          }
          filledrect(c, END, 0.0, 0.0, 0.0, 0.0);
          drawarc(c, END, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0);
          drawrect(c, END, 0.0, 0.0, 0.0, 0.0, 0);
          drawline(c, END, 0.0, 0.0, 0.0, 0.0, 0);
        }
        if(xctx->draw_single_layer==-1 || xctx->draw_single_layer==WIRELAYER) {
          if(use_hash) init_wire_iterator(&ctx, x1, y1, x2, y2);
          else i = -1;
          while(1) {
            if(use_hash) {
              if( !(wireptr = wire_iterator_next(&ctx))) break;
              i = wireptr->n;
            }
            else {
              i++;
              if(i >= xctx->wires) break;
            }
            if(xctx->wire[i].bus) {
              drawline(WIRELAYER, THICK, xctx->wire[i].x1,xctx->wire[i].y1,
                xctx->wire[i].x2,xctx->wire[i].y2, 0);
            }
            else
              drawline(WIRELAYER, ADD, xctx->wire[i].x1,xctx->wire[i].y1,
                xctx->wire[i].x2,xctx->wire[i].y2, 0);
          }
          update_conn_cues(1, xctx->draw_window);
          filledrect(WIRELAYER, END, 0.0, 0.0, 0.0, 0.0);
          drawline(WIRELAYER, END, 0.0, 0.0, 0.0, 0.0, 0);
        }
        if(xctx->draw_single_layer ==-1 || xctx->draw_single_layer==TEXTLAYER) {
          for(i=0;i<xctx->texts;i++)
          {
            textlayer = xctx->text[i].layer;
            if(!show_hidden_texts && (xctx->text[i].flags & HIDE_TEXT)) continue;
            if(textlayer < 0 ||  textlayer >= cadlayers) textlayer = TEXTLAYER;
            dbg(1, "draw(): drawing string %d = %s\n",i, xctx->text[i].txt_ptr);
            #if HAS_CAIRO==1
            if(!xctx->enable_layer[textlayer]) continue;
            textfont = xctx->text[i].font;
            if( (textfont && textfont[0]) || 
                (xctx->text[i].flags & (TEXT_BOLD | TEXT_OBLIQUE | TEXT_ITALIC))) {
              cairo_font_slant_t slant;
              cairo_font_weight_t weight;
              textfont = (xctx->text[i].font && xctx->text[i].font[0]) ? 
                xctx->text[i].font : tclgetvar("cairo_font_name");
              weight = ( xctx->text[i].flags & TEXT_BOLD) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL;
              slant = CAIRO_FONT_SLANT_NORMAL;
              if(xctx->text[i].flags & TEXT_ITALIC) slant = CAIRO_FONT_SLANT_ITALIC;
              if(xctx->text[i].flags & TEXT_OBLIQUE) slant = CAIRO_FONT_SLANT_OBLIQUE;

              cairo_save(xctx->cairo_ctx);
              cairo_save(xctx->cairo_save_ctx);
              cairo_select_font_face (xctx->cairo_ctx, textfont, slant, weight);
              cairo_select_font_face (xctx->cairo_save_ctx, textfont, slant, weight);
            }
            #endif
            draw_string(textlayer, ADD, xctx->text[i].txt_ptr,
              xctx->text[i].rot, xctx->text[i].flip, xctx->text[i].hcenter, xctx->text[i].vcenter,
              xctx->text[i].x0,xctx->text[i].y0,
              xctx->text[i].xscale, xctx->text[i].yscale);
            #if HAS_CAIRO==1
            if( (textfont && textfont[0]) || 
                (xctx->text[i].flags & (TEXT_BOLD | TEXT_OBLIQUE | TEXT_ITALIC))) {
              cairo_restore(xctx->cairo_ctx);
              cairo_restore(xctx->cairo_save_ctx);
            }
            #endif
            #if HAS_CAIRO!=1
            drawrect(textlayer, END, 0.0, 0.0, 0.0, 0.0, 0);
            drawline(textlayer, END, 0.0, 0.0, 0.0, 0.0, 0);
            #endif
          }
        }
    } /* !xctx->only_probes, 20110112 */
    draw_hilight_net(xctx->draw_window);
    if(!xctx->draw_window) {
      MyXCopyArea(display, xctx->save_pixmap, xctx->window, xctx->gctiled, xctx->xrect[0].x, xctx->xrect[0].y,
         xctx->xrect[0].width, xctx->xrect[0].height, xctx->xrect[0].x, xctx->xrect[0].y);
    }
#if !defined(__unix__) && defined(HAS_CAIRO)
    else 
      my_cairo_fill(xctx->cairo_sfc, xctx->xrect[0].x, xctx->xrect[0].y, xctx->xrect[0].width, xctx->xrect[0].height);
#endif
    draw_selection(xctx->gc[SELLAYER], 0); /* 20181009 moved outside of cadlayers loop */
 } /* if(has_x) */
}

#ifndef __unix__
/* place holder for Windows to show that these XLib functions are not supported in Windows. */
int XSetClipRectangles(register Display* dpy, GC gc, int clip_x_origin, int clip_y_origin,
                       XRectangle* rectangles, int n, int ordering)
{
  return 0;
}
int XSetTile(Display* display, GC gc, Pixmap s_pixmap)
{
  return 0;
}
#endif

void MyXCopyArea(Display* display, Drawable src, Drawable dest, GC gc, int src_x, int src_y, unsigned int width, unsigned int height, int dest_x, int dest_y)
{
  XCopyArea(display, src, dest, gc, src_x, src_y, width, height, dest_x, dest_y);
#if !defined(__unix__)  && defined(HAS_CAIRO)
  my_cairo_fill(xctx->cairo_save_sfc, dest_x, dest_y, width, height);
#endif
}

