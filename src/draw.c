/* File: draw.c
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


/* Window doesn't work with LineDoubleDash, but will with LineOnOffDash */
#ifdef __unix__
#define xDashType LineDoubleDash
#else
#define xDashType LineOnOffDash
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
  int save_draw_grid;
  char cmd[PATH_MAX+100];
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
  #ifdef __unix__
  XpmWriteFileFromPixmap(display, "plot.xpm", xctx->save_pixmap,0, NULL ); /* .gz ???? */
  dbg(1, "print_image(): Window image saved\n");
  if(xctx->plotfile[0]) {
    my_snprintf(cmd, S(cmd), "convert_to_png plot.xpm {%s}", xctx->plotfile);
    tcleval(cmd);
  } else tcleval( "convert_to_png plot.xpm plot.png");
  #else
  char *psfile=NULL;
  create_ps(&psfile, 7);
  if(xctx->plotfile[0]) {
    my_snprintf(cmd, S(cmd), "convert_to_png {%s} {%s}", psfile, xctx->plotfile);
    tcleval(cmd);
  } else tcleval( "convert_to_png {%s} plot.png", psfile);
  #endif
  my_strncpy(xctx->plotfile,"", S(xctx->plotfile));
  tclsetboolvar("draw_grid", save_draw_grid);
  xctx->draw_pixmap=1;
}

#if HAS_CAIRO==1
void set_cairo_color(int layer) 
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

  textfont = txt->font;
  if((textfont && textfont[0]) || txt->flags) {
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
    int llength, int no_of_lines, int longest_line)
{
  double ix, iy;
  short rot1;
  int line_delta;
  double lines;
  double vc; /* 20171121 vert correct */
  
  if(s==NULL) return;
  if(llength==0) return;

  line_delta = lineno*fontheight*cairo_font_line_spacing;
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
  int llength=0, no_of_lines, longest_line;

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
 int i,lines, no_of_lines, longest_line;

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
 if(!has_x) return;
 dbg(2, "draw_string(): string=%s\n",str);
 if(!text_bbox(str, xscale, yscale, rot, flip, hcenter, vcenter, x1,y1,
     &textx1,&texty1,&textx2,&texty2, &tmp, &tmp)) return;
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
  int j, textlayer, hide = 0;
  double x0,y0,x1,y1,x2,y2;
  short flip;
  xLine line;
  xRect rect;
  xArc arc;
  xPoly polygon;
  xText text;
  register xSymbol *symptr;
  double angle;
  char *type;
  #if HAS_CAIRO==1
  const char *textfont;
  #endif

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
      line = (symptr->line[layer])[j];
      ROTATION(rot, flip, 0.0,0.0,line.x1,line.y1,x1,y1);
      ROTATION(rot, flip, 0.0,0.0,line.x2,line.y2,x2,y2);
      ORDER(x1,y1,x2,y2);
      if(line.bus)
        drawline(c,THICK, x0+x1, y0+y1, x0+x2, y0+y2, line.dash);
      else
        drawline(c,what, x0+x1, y0+y1, x0+x2, y0+y2, line.dash);
    }
    for(j=0;j< symptr->polygons[layer];j++)
    {
      polygon = (symptr->poly[layer])[j];
      {   /* scope block so we declare some auxiliary arrays for coord transforms. */
        int k;
        double *x = my_malloc(34, sizeof(double) * polygon.points);
        double *y = my_malloc(35, sizeof(double) * polygon.points);
        for(k=0;k<polygon.points;k++) {
          ROTATION(rot, flip, 0.0,0.0,polygon.x[k],polygon.y[k],x[k],y[k]);
          x[k]+= x0;
          y[k] += y0;
        }
        drawpolygon(c, NOW, x, y, polygon.points, polygon.fill, polygon.dash); /* added fill */
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
      ROTATION(rot, flip, 0.0,0.0,arc.x,arc.y,x1,y1);
      drawarc(c,what, x0+x1, y0+y1, arc.r, angle, arc.b, arc.fill, arc.dash);
    }
  } /* if(!hide) */

  if( (!hide && xctx->enable_layer[layer])  ||
      (hide && layer == PINLAYER && xctx->enable_layer[layer]) ) {
    for(j=0;j< symptr->rects[layer];j++)
    {
      rect = (symptr->rect[layer])[j];
      ROTATION(rot, flip, 0.0,0.0,rect.x1,rect.y1,x1,y1);
      ROTATION(rot, flip, 0.0,0.0,rect.x2,rect.y2,x2,y2);
      RECTORDER(x1,y1,x2,y2);
      drawrect(c,what, x0+x1, y0+y1, x0+x2, y0+y2, rect.dash);
      filledrect(c,what, x0+x1, y0+y1, x0+x2, y0+y2);
    }
  }
  if( (layer==TEXTWIRELAYER && !(xctx->inst[n].flags&2) ) ||
      (xctx->sym_txt && (layer==TEXTLAYER) && (xctx->inst[n].flags&2) ) ) {
    const char *txtptr;
    for(j=0;j< symptr->texts;j++)
    {
      text = symptr->text[j];
      if(text.xscale*FONTWIDTH*xctx->mooz<1) continue;
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
        if((textfont && textfont[0]) || symptr->text[j].flags) {
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
        if( (textfont && textfont[0]) || symptr->text[j].flags) {
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
 xLine line;
 xPoly polygon;
 xRect rect;
 xArc arc;
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
  line = (symptr->line[layer])[j];
  ROTATION(rot, flip, 0.0,0.0,line.x1,line.y1,x1,y1);
  ROTATION(rot, flip, 0.0,0.0,line.x2,line.y2,x2,y2);
  ORDER(x1,y1,x2,y2);
  if(line.bus)
    drawtempline(gc,THICK, x0+x1, y0+y1, x0+x2, y0+y2);
  else
    drawtempline(gc,what, x0+x1, y0+y1, x0+x2, y0+y2);
 }
 for(j=0;j< symptr->polygons[layer];j++)
 {
   polygon = (symptr->poly[layer])[j];

   {   /* scope block so we declare some auxiliary arrays for coord transforms. 20171115 */
     int k;
     double *x = my_malloc(36, sizeof(double) * polygon.points);
     double *y = my_malloc(37, sizeof(double) * polygon.points);
     for(k=0;k<polygon.points;k++) {
       ROTATION(rot, flip, 0.0,0.0,polygon.x[k],polygon.y[k],x[k],y[k]);
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
  rect = (symptr->rect[layer])[j];
  ROTATION(rot, flip, 0.0,0.0,rect.x1,rect.y1,x1,y1);
  ROTATION(rot, flip, 0.0,0.0,rect.x2,rect.y2,x2,y2);
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
   ROTATION(rot, flip, 0.0,0.0,arc.x,arc.y,x1,y1);
   drawtemparc(gc, what, x0+x1, y0+y1, arc.r, angle, arc.b);
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

void drawgrid()
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
        xctx->gridpoint[i].x=(int)(x);
        xctx->gridpoint[i].y=(int)(y);
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
     dash_arr[0] = dash_arr[1] = dash;
     XSetDashes(display, xctx->gc[c], 0, dash_arr, 2);
     XSetLineAttributes (display, xctx->gc[c], INT_WIDTH(xctx->lw), xDashType, CapButt, JoinBevel);
   }
   if(xctx->draw_window) XDrawLine(display, xctx->window, xctx->gc[c], x1, y1, x2, y2);
   if(xctx->draw_pixmap)
    XDrawLine(display, xctx->save_pixmap, xctx->gc[c], x1, y1, x2, y2);
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
     dash_arr[0] = dash_arr[1] = dash;
     XSetDashes(display, xctx->gc[c], 0, dash_arr, 2);
     XSetLineAttributes (display, xctx->gc[c], INT_BUS_WIDTH(xctx->lw), xDashType, CapButt, JoinBevel);
   } else {
     XSetLineAttributes (display, xctx->gc[c], INT_BUS_WIDTH(xctx->lw), LineSolid, CapRound, JoinRound);
   }
   if(xctx->draw_window) XDrawLine(display, xctx->window, xctx->gc[c], x1, y1, x2, y2);
   if(xctx->draw_pixmap) XDrawLine(display, xctx->save_pixmap, xctx->gc[c], x1, y1, x2, y2);
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
   XDrawLine(display, xctx->window, gc, x1, y1, x2, y2);
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

   XDrawLine(display, xctx->window, gc, x1, y1, x2, y2);
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
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   XDrawArc(display, xctx->window, gc, xx1, yy1, xx2-xx1, yy2-yy1, a*64, b*64);
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
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   if(xctx->draw_window) XFillArc(display, xctx->window, xctx->gc[c], xx1, yy1, xx2-xx1, yy2-yy1, a*64, b*64);
   if(xctx->draw_pixmap) XFillArc(display, xctx->save_pixmap, xctx->gc[c], xx1, yy1, xx2-xx1, yy2-yy1, a*64, b*64);
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
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
   if(dash) {
     char dash_arr[2];
     dash_arr[0] = dash_arr[1] = dash;
     XSetDashes(display, xctx->gc[c], 0, dash_arr, 2);
     XSetLineAttributes (display, xctx->gc[c], INT_WIDTH(xctx->lw), xDashType, CapButt, JoinBevel);
   }

   if(xctx->draw_window) {
     XDrawArc(display, xctx->window, xctx->gc[c], xx1, yy1, xx2-xx1, yy2-yy1, a*64, b*64);
   }
   if(xctx->draw_pixmap) {
     XDrawArc(display, xctx->save_pixmap, xctx->gc[c], xx1, yy1, xx2-xx1, yy2-yy1, a*64, b*64);
   }

   if(xctx->fill_pattern && xctx->fill_type[c]){
     if(arc_fill) {
       if(xctx->draw_window)
         XFillArc(display, xctx->window, xctx->gcstipple[c], xx1, yy1, xx2-xx1, yy2-yy1, a*64, b*64);
       if(xctx->draw_pixmap)
         XFillArc(display, xctx->save_pixmap, xctx->gcstipple[c], xx1, yy1, xx2-xx1, yy2-yy1, a*64, b*64);
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
   r[i].width=(unsigned short)x2-r[i].x;
   r[i].height=(unsigned short)y2-r[i].y;
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
  if( !xctx->only_probes && (x2-x1)<3.0 && (y2-y1)<3.0) return;

  p = my_malloc(38, sizeof(XPoint) * points);
  if(what) {
    for(i=0;i<points; i++) {
      clip_xy_to_short(X_TO_SCREEN(x[i]), Y_TO_SCREEN(y[i]), &sx, &sy);
      p[i].x = sx;
      p[i].y = sy;
    }
  } else {
      /* preserve cache locality working on contiguous data */
      for(i=0;i<points; i++) p[i].x = X_TO_SCREEN(x[i]);
      for(i=0;i<points; i++) p[i].y = Y_TO_SCREEN(y[i]);
  }
  if(dash) {
    char dash_arr[2];
    dash_arr[0] = dash_arr[1] = dash;
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
     dash_arr[0] = dash_arr[1] = dash;
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
   r[i].width=(unsigned short)x2-r[i].x;
   r[i].height=(unsigned short)y2-r[i].y;
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
   r[i].width=(unsigned short)x2-r[i].x;
   r[i].height=(unsigned short)y2-r[i].y;
   i++;
  }
 }
 else if((what & END) && i)
 {
  XDrawRectangles(display, xctx->window, gc, r,i);
  i=0;
 }
}

/* read the binary portion of a ngspice raw simulation file
 * data layout in memory arranged to maximize cache locality 
 * when looking up data 
 */
void read_binary_block(FILE *fd)
{
  int p, v;
  double *tmp;
  size_t size = 0;
  int offset = 0;


  for(p = 0 ; p < xctx->graph_datasets; p++) {
    size += xctx->graph_nvars * xctx->graph_npoints[p];
    offset += xctx->graph_npoints[p];
  }

  /* read buffer */
  tmp = my_calloc(1405, xctx->graph_nvars, sizeof(double *));
  /* allocate storage for binary block */
  if(!xctx->graph_values) xctx->graph_values = my_calloc(118, xctx->graph_nvars, sizeof(SPICE_DATA *));
  for(p = 0 ; p < xctx->graph_nvars; p++) {
    my_realloc(372, &xctx->graph_values[p], (size + xctx->graph_npoints[xctx->graph_datasets]) * sizeof(double));
  }
  /* read binary block */
  for(p = 0; p < xctx->graph_npoints[xctx->graph_datasets]; p++) {
    if(fread(tmp,  sizeof(double), xctx->graph_nvars, fd) != xctx->graph_nvars) {
       dbg(0, "Warning: binary block is not of correct size\n");
    }
    /* assign to xschem struct, memory aligned per variable, for cache locality */
    for(v = 0; v < xctx->graph_nvars; v++) {
      xctx->graph_values[v][offset + p] = tmp[v];
    }
  }
  my_free(1406, &tmp);
}

/* parse ascii raw header section:
 * returns: 1 if dataset and variables were read.
 *          0 if transient sim dataset not found
 *         -1 on EOF
 * Typical ascii header of raw file looks like:
 *
 * Title: **.subckt poweramp
 * Date: Thu Nov 21 18:36:25  2019
 * Plotname: Transient Analysis
 * Flags: real
 * No. Variables: 158
 * No. Points: 90267
 * Variables:
 *         0       time    time
 *         1       v(net1) voltage
 *         2       v(vss)  voltage
 *         ...
 *         ...
 *         155     i(v.x1.vd)      current
 *         156     i(v0)   current
 *         157     i(v1)   current
 * Binary:
 */
int read_dataset(FILE *fd)
{ 
  int variables = 0, i, done_points = 0;
  char line[PATH_MAX], varname[PATH_MAX];
  char *ptr;
  int sim_type = 0; /* 1: transient, 2: dc, 4: ... */
  int done_header = 0;
  int exit_status = 0;
  while((ptr = fgets(line, sizeof(line), fd)) ) {
    /* after this line comes the binary blob made of nvars * npoints * sizeof(double) bytes */
    if(!strcmp(line, "Binary:\n")) {
      int npoints = xctx->graph_npoints[xctx->graph_datasets];
      if(sim_type) {
        done_header = 1;
        read_binary_block(fd); 
        dbg(1, "read_dataset(): read binary block, nvars=%d npoints=%d\n", xctx->graph_nvars, npoints);
        xctx->graph_datasets++;
        exit_status = 1;
      } else { 
        dbg(1, "read_dataset(): skip binary block, nvars=%d npoints=%d\n", xctx->graph_nvars, npoints);
        fseek(fd, xctx->graph_nvars * npoints * sizeof(double), SEEK_CUR); /* skip binary block */
      }
      done_points = 0;
    }
    else if(!strncmp(line, "Plotname: Transient Analysis", 28)) {
      if(sim_type && sim_type != 1) sim_type = 0;
      else sim_type = 1;
    }
    else if(!strncmp(line, "Plotname: DC transfer characteristic", 36)) {
      if(sim_type && sim_type != 2) sim_type = 0;
      else sim_type = 2;
    }
    else if(!strncmp(line, "Plotname:", 9)) {
      sim_type = 0;
    }
    /* points and vars are needed for all sections (also ones we are not interested in)
     * to skip binary blobs */
    else if(!strncmp(line, "No. of Data Rows :", 18)) {
      /* array of number of points of datasets (they are of varialbe length) */
      my_realloc(1414, &xctx->graph_npoints, (xctx->graph_datasets+1) * sizeof(int));
      sscanf(line, "No. of Data Rows : %d", &xctx->graph_npoints[xctx->graph_datasets]);
      done_points = 1;
    }
    else if(!strncmp(line, "No. Variables:", 14)) {
      sscanf(line, "No. Variables: %d", &xctx->graph_nvars);
    }
    else if(!done_points && !strncmp(line, "No. Points:", 11)) {
      my_realloc(1415, &xctx->graph_npoints, (xctx->graph_datasets+1) * sizeof(int));
      sscanf(line, "No. Points: %d", &xctx->graph_npoints[xctx->graph_datasets]);
    }
    if(!done_header && variables) {
      /* get the list of lines with index and node name */
      if(!xctx->graph_names) xctx->graph_names = my_calloc(426, xctx->graph_nvars, sizeof(char *));
      sscanf(line, "%d %s", &i, varname); /* read index and name of saved waveform */
      xctx->graph_names[i] = my_malloc(415, strlen(varname) + 1);
      strcpy(xctx->graph_names[i], varname);
      /* use hash table to store index number of variables */
      int_hash_lookup(xctx->raw_table, xctx->graph_names[i], i, XINSERT_NOREPLACE);
      dbg(1, "read_dataset(): get node list -> names[%d] = %s\n", i, xctx->graph_names[i]);
    }
    /* after this line comes the list of indexes and associated nodes */
    if(sim_type && !strncmp(line, "Variables:", 10)) {
      variables = 1 ;
    }
  }
  dbg(1, "read_dataset(): datasets=%d, last npoints=%d, nvars=%d\n",
    xctx->graph_datasets,  xctx->graph_npoints[xctx->graph_datasets-1], xctx->graph_nvars);
  return exit_status;
}

void free_rawfile(int dr)
{
  int i;

  int deleted = 0;
  if(xctx->graph_names) {
    deleted = 1;
    for(i = 0 ; i < xctx->graph_nvars; i++) {
      my_free(510, &xctx->graph_names[i]);
    }
    my_free(968, &xctx->graph_names);
  }
  if(xctx->graph_values) {
    deleted = 1;
    for(i = 0 ; i < xctx->graph_nvars; i++) {
      my_free(512, &xctx->graph_values[i]);
    }
    my_free(528, &xctx->graph_values);
  }
  if(xctx->graph_npoints) my_free(1413, &xctx->graph_npoints);
  if(xctx->raw_schname) my_free(1393, &xctx->raw_schname);
  xctx->graph_datasets = 0;
  xctx->graph_nvars = 0;
  int_hash_free(xctx->raw_table);
  if(deleted && dr) draw();
}

/* read a ngspice raw file (with data portion in binary format) */
int read_rawfile(const char *f)
{
  int res = 0;
  FILE *fd;
  if(xctx->graph_values || xctx->graph_npoints || xctx->graph_nvars || xctx->graph_datasets) {
    dbg(0, "read_rawfile(): must clear current raw file before loading new\n");
    return 0;
  }
  fd = fopen(f, fopen_read_mode);
  if(fd) {
    if((res = read_dataset(fd)) == 1) {
      dbg(0, "Raw file data read\n");
      my_strdup2(1394, &xctx->raw_schname, xctx->sch[xctx->currsch]);
      draw();
    } else {
      dbg(0, "read_rawfile(): no useful data found\n");
    }
    fclose(fd);
    return res;
  }
  dbg(0, "read_rawfile(): failed to open file %s for reading\n", f);
  return 0;
}

int get_raw_index(const char *node)
{
  Int_hashentry *entry;
  if(xctx->graph_values) {
    entry = int_hash_lookup(xctx->raw_table, node, 0, XLOOKUP);
    if(entry) return entry->value;
  }
  return -1;
}

double get_raw_value(int dataset, int idx, int point)
{
  int i, ofs;
  ofs = 0;
  if(xctx->graph_values) {
    for(i = 0; i < dataset; i++) {
      ofs += xctx->graph_npoints[i];
    }
    return xctx->graph_values[idx][ofs + point];
  }
  return 0.0;
}
void calc_graph_area(int c, int i, int digital, double *x1, double *y1,double *x2,double *y2,
     double *marginx, double *marginy)
{
  double rx1, rx2, ry1, ry2, rw, rh, tmp;
  rx1 = xctx->rect[c][i].x1;
  ry1 = xctx->rect[c][i].y1;
  rx2 = xctx->rect[c][i].x2;
  ry2 = xctx->rect[c][i].y2;
  rw = (rx2 - rx1);
  rh = (ry2 - ry1);
  /* set margins */
  tmp = rw * 0.1;
  *marginx = tmp < 30 ? 30 : tmp;
  tmp = rh * 0.09;
  *marginy = tmp < 30 ? 30 : tmp;

  /* calculate graph bounding box (container - margin) 
   * This is the box where plot is done */
  *x1 =  rx1 + *marginx;
  *x2 =  rx2 - *marginx / 2.4; /* less space for right margin */
  if(digital) *y1 =  ry1 + *marginy * 0.4;
  else *y1 =  ry1 + *marginy;
  *y2 =  ry2 - *marginy;
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
static double axis_increment(double a, double b, int div)
{
  double scale;
  double sign;
  double scaled_delta;
  double delta = b - a;
  if(div == 1) {
    return delta;
  }
  if(delta == 0.0) return delta;
  /* if user wants only one division, just do what user asks */
  if(div == 1) return delta;
  delta /= div;
  sign = (delta < 0.0) ? -1.0 : 1.0;
  delta = fabs(delta);
  scale = pow(10.0, floor(log10(delta)));
  scaled_delta =  delta / scale; /* 1 <= scaled_delta < 10 */
  if(scaled_delta > 5.5) scaled_delta = 10.0;
  else if(scaled_delta > 2.2) scaled_delta = 5.0;
  else if(scaled_delta > 1.1) scaled_delta = 2.0;
  else scaled_delta = 1.0;
  return scaled_delta * scale * sign;
}

static double axis_start(double n, double delta, int div)
{
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
  if(xctx->graph_values && xctx->raw_schname) {
    for(i = xctx->currsch; i >= 0; i--) {
      if( !strcmp(xctx->raw_schname, xctx->sch[i]) ) return 1;
    } 
  }
  return 0;
}


/* coordinate transformations graph to xschem */
#define W_X(x) (cx * (x) + dx)
#define W_Y(y) (cy * (y) + dy)
/* coordinate transformations graph to screen */
#define S_X(x) (scx * (x) + sdx)
#define S_Y(y) (scy * (y) + sdy)

/* draw bussed signals: ntok is a comma separated list of items, first item is bus name, 
 * following are bits that are bundled together:
   LDA,LDA[3],LDA[2],LDA1],LDA[0]
 */
static void draw_graph_bus_points(const char *ntok, int first, int last,
         double cx, double dx, double cy, double dy,
         int wave_col, int sweep_idx,
         int digital, int dig_max_waves, int wcnt, int n_nodes, double wy1, double wy2)
{
  int p, i;
  int n = dig_max_waves;
  double s1 = 1.0 / n;
  double s2 = s1 * .66;
  double c = (wy2 - wy1) * wcnt * s1;
  double x1 = W_X(xctx->graph_values[sweep_idx][first]);
  double x2 = W_X(xctx->graph_values[sweep_idx][last-1]);
  double ylow  = W_Y(c + wy2 * s2); /* swapped as xschem Y coordinates are top-bottom */
  double yhigh = W_Y(c + wy1 * s2);
  const char *bit_name;
  int n_bits = count_items(ntok, ",") - 1;
  int *idx_arr = NULL;
  Int_hashentry *entry;
  unsigned long val, busval, old_busval;
  double vth = (wy1 + wy2) / 2.0; /* A to D threshold */
  double xval, xval_old;
  double ydelta = fabs(yhigh - ylow);
  double labsize = 0.015 * ydelta;
  double charwidth = labsize * 38.0;
  char str[100];
  int hex_digits = (n_bits - 1) / 4 + 1;
  double x_size = 1.5 * xctx->zoom;

  idx_arr = my_malloc(1454, (n_bits) * sizeof(int));
  p = 0;
  dbg(1, "n_bits = %d\n", n_bits);
  while( (bit_name = find_nth(ntok, ',', p + 2))[0] ) {
    dbg(1, "draw_graph_bus_points(): bit %d : %s: ", p, bit_name);
    entry = int_hash_lookup(xctx->raw_table, bit_name, 0, XLOOKUP);
    if(entry) {
      dbg(1, "%d\n", entry->value);
      idx_arr[p] = entry->value;
    } else {
      dbg(1, "\n");
    }
    p++;
  }
  drawline(wave_col, NOW, x1, ylow, x2, ylow, 0);
  drawline(wave_col, NOW, x1, yhigh, x2, yhigh, 0);
  for(p = first ; p <= last; p++) {
    /* calculate value of bus by adding all binary bits */
    busval = 0;
    for(i = 0; i < n_bits; i++) {
      val = xctx->graph_values[idx_arr[i]][p];
      busval = (busval << 1) + (val > vth);
    }

    xval =  W_X(xctx->graph_values[sweep_idx][p]);

    /* used to draw bus value before 1st transition */
    if(p == first) {
      old_busval = busval;
      xval_old = xval;
    }
    if(p > first &&  busval != old_busval) {
      /* draw transition ('X') */
      drawline(BACKLAYER, NOW, xval-x_size, yhigh, xval+x_size, yhigh, 0);
      drawline(BACKLAYER, NOW, xval-x_size, ylow,  xval+x_size, ylow, 0);
      drawline(wave_col, NOW, xval-x_size, ylow,  xval+x_size, yhigh, 0);
      drawline(wave_col, NOW, xval-x_size, yhigh, xval+x_size, ylow, 0);
      /* build hex string for bus value */
      sprintf(str, "%0*lX", hex_digits, old_busval);
      /* draw hex bus value if there is enough room */
      if(  fabs(xval - xval_old) > strlen(str) * charwidth) {
        draw_string(wave_col, NOW, str, 2, 0, 1, 0, (xval + xval_old) / 2.0,
                    yhigh, labsize, labsize);

      }
      old_busval = busval;
      xval_old = xval;
    } /* if(p > first &&  busval != old_busval) */
  } /* for(p = first ; p < last; p++) */
  /* draw hex bus value after last transition */
  sprintf(str, "%0*lX", hex_digits, busval);
  if(  fabs(xval - xval_old) > strlen(str) * charwidth) {
    draw_string(wave_col, NOW, str, 2, 0, 1, 0, (xval + xval_old) / 2.0,
                yhigh, labsize, labsize);
  }
  my_free(1455, &idx_arr);
}

static void draw_graph_points(int v, int first, int last, double scy, double sdy,
         XPoint *point, int wave_col,
         int digital, int dig_max_waves, int wcnt, int n_nodes, double wy1, double wy2)
{
  int p;
  double yy;
  double ydelta = wy2 - wy1;
  int poly_npoints = 0;
  int n = dig_max_waves;
  double s1 = 1.0 / n;
  double s2 = s1 * .66;
  double c = ydelta * wcnt * s1;

  if( !digital || (c >= wy1 && c <= wy2) ) {
    for(p = first ; p <= last; p++) {
      yy = xctx->graph_values[v][p];
      if(digital) {
        yy = c + yy *s2;
      }
      /* Build poly y array. Translate from graph coordinates to {x1,y1} - {x2, y2} world. */
      point[poly_npoints].y = CLIP(S_Y(yy), xctx->areay1, xctx->areay2);
      poly_npoints++;
    }
    /* plot data */
    if(xctx->draw_window)
      XDrawLines(display, xctx->window, xctx->gc[wave_col], point, poly_npoints, CoordModeOrigin);
    if(xctx->draw_pixmap)
      XDrawLines(display, xctx->save_pixmap, xctx->gc[wave_col], point, poly_npoints, CoordModeOrigin);
  } else dbg(1, "skipping wave: %s\n", xctx->graph_names[v]);
}

static void draw_graph_grid(
                  double rx1, double ry1, double rx2, double ry2, /* container box */
                  double x1, double y1, double x2, double y2,     /* graph area */
                  double marginx, double marginy,                 
                  double wx1, double wy1, double wx2, double wy2, /* graph coordinate system */
                  double cx, double cy, double dx, double dy,     /* graph to xschem matrix transform */
                  int divx, int divy, int subdivx, int subdivy,   /* axis major/minor grids */
                  double unitx, double unity,                     /* unit conversion (p, u, m, k, M, G) */
                  int digital,                                    /* set to 1 for digital plot */
                  double *txtsizex)                               /* txtsizex needed in caller too */
{
  double deltax, startx, deltay, starty, wx,wy,  dash_sizex, dash_sizey, w, h;
  int j, k;
  char lab[30];
  double tmp, txtsizey;
  double mark_size = marginy/10.0;

  w = (x2 - x1);
  h = (y2 - y1);
  /* x axis, y axis label text sizes */
  txtsizey = h / divy * 0.009;
  tmp = marginx * 0.005;
  if(tmp < txtsizey) txtsizey = tmp;
  tmp = marginy * 0.02;
  if(tmp < txtsizey) txtsizey = tmp;

  *txtsizex = w / divx * 0.0033;
  tmp = marginy * 0.0063;
  if(tmp < *txtsizex) *txtsizex = tmp;

  /* calculate dash length for grid lines */
  dash_sizex = 1.5 * xctx->mooz;
  dash_sizex = dash_sizex > 2.0 ? 2.0 : dash_sizex;
  if(dash_sizex <= 1.0) dash_sizex = 1.0;
  dash_sizey = 1.5 * xctx->mooz;
  dash_sizey = dash_sizey > 2.0 ? 2.0 : dash_sizey;
  if(dash_sizey <= 1.0) dash_sizey = 1.0;

  /* clipping everything outside container area */
  /* background */
  filledrect(0, NOW, rx1, ry1, rx2, ry2);
  /* graph bounding box */
  drawrect(GRIDLAYER, NOW, rx1, ry1, rx2, ry2, 2);

  bbox(START, 0.0, 0.0, 0.0, 0.0);
  bbox(ADD, rx1, ry1, rx2, ry2);
  bbox(SET_INSIDE, 0.0, 0.0, 0.0, 0.0);
  /* vertical grid lines */
  deltax = axis_increment(wx1, wx2, divx);
  startx = axis_start(wx1, deltax, divx);
  for(j = -1;; j++) { /* start one interval before to allow sub grids at beginning */
    wx = startx + j * deltax;
    if(subdivx > 0) for(k = 1; k <=subdivx; k++) {
      double subwx = wx + k * deltax / (subdivx + 1);
      if(!axis_within_range(subwx, wx1, wx2)) continue;
      if(axis_end(subwx, deltax, wx2)) break;
      drawline(GRIDLAYER, ADD, W_X(subwx),   W_Y(wy2), W_X(subwx),   W_Y(wy1), dash_sizey);
    }
    if(!axis_within_range(wx, wx1, wx2)) continue;
    if(axis_end(wx, deltax, wx2)) break;
    /* swap order of wy1 and wy2 since grap y orientation is opposite to xorg orientation */
    drawline(GRIDLAYER, ADD, W_X(wx),   W_Y(wy2), W_X(wx),   W_Y(wy1), dash_sizey);
    drawline(GRIDLAYER, ADD, W_X(wx),   W_Y(wy1), W_X(wx),   W_Y(wy1) + mark_size, 0); /* axis marks */
    /* X-axis labels */
    my_snprintf(lab, S(lab), "%g", wx * unitx);
    draw_string(3, NOW, lab, 0, 0, 1, 0, W_X(wx), y2 + mark_size + 5 * *txtsizex, *txtsizex, *txtsizex);
  }
  /* first and last vertical box delimiters */
  drawline(GRIDLAYER, ADD, W_X(wx1),   W_Y(wy2), W_X(wx1),   W_Y(wy1), 0);
  drawline(GRIDLAYER, ADD, W_X(wx2),   W_Y(wy2), W_X(wx2),   W_Y(wy1), 0);
  /* horizontal grid lines */
  if(!digital) {
    deltay = axis_increment(wy1, wy2, divy);
    starty = axis_start(wy1, deltay, divy);
    for(j = -1;; j++) { /* start one interval before to allow sub grids at beginning */
      wy = starty + j * deltay;
      if(subdivy > 0) for(k = 1; k <=subdivy; k++) {
        double subwy = wy + k * deltay / (subdivy + 1);
        if(!axis_within_range(subwy, wy1, wy2)) continue;
        if(axis_end(subwy, deltay, wy2)) break;
        drawline(GRIDLAYER, ADD, W_X(wx1), W_Y(subwy),   W_X(wx2), W_Y(subwy), dash_sizex);
      }
      if(!axis_within_range(wy, wy1, wy2)) continue;
      if(axis_end(wy, deltay, wy2)) break;
      drawline(GRIDLAYER, ADD, W_X(wx1), W_Y(wy),   W_X(wx2), W_Y(wy), dash_sizex);
      drawline(GRIDLAYER, ADD, W_X(wx1) - mark_size, W_Y(wy),   W_X(wx1), W_Y(wy), 0); /* axis marks */
      /* Y-axis labels */
      my_snprintf(lab, S(lab), "%g",  wy * unity);
      draw_string(3, NOW, lab, 0, 1, 0, 1, x1 - mark_size - 5 * txtsizey, W_Y(wy), txtsizey, txtsizey);
    }
  }
  /* first and last horizontal box delimiters */
  drawline(GRIDLAYER, ADD, W_X(wx1),   W_Y(wy1), W_X(wx2),   W_Y(wy1), 0);
  drawline(GRIDLAYER, ADD, W_X(wx1),   W_Y(wy2), W_X(wx2),   W_Y(wy2), 0);
  /* Horizontal axis (if in viewport) */
  if(!digital && wy1 <= 0 && wy2 >= 0) drawline(GRIDLAYER, ADD, W_X(wx1), W_Y(0), W_X(wx2), W_Y(0), 0);
  /* Vertical axis (if in viewport) 
   * swap order of wy1 and wy2 since grap y orientation is opposite to xorg orientation */
  if(wx1 <= 0 && wx2 >= 0) drawline(GRIDLAYER, ADD, W_X(0),   W_Y(wy2), W_X(0),   W_Y(wy1), 0);
  drawline(GRIDLAYER, END, 0.0, 0.0, 0.0, 0.0, 0);
  bbox(END, 0.0, 0.0, 0.0, 0.0);


}

/* flags:
 * 1: do final XCopyArea (copy 2nd buffer areas to screen) 
 *    If draw_graph_all() is called from draw() no need to do XCopyArea, as draw() does it already.
 *    This makes drawing faster and removes a 'tearing' effect when moving around.
 * 2: draw x-cursor1
 * 4: draw x-cursor2
 * 8: all drawing, if not set do only XCopyArea / x-cursor if specified
 */
void draw_graph(int c, int i, int flags)
{
  /* container box */
  double rx1, ry1, rx2, ry2, rw/*, rh */; 
  /* graph box (smaller due to margins) */
  double x1, y1, x2, y2; 
  /* graph coordinate, some defaults */
  int digital = 0;
  int dig_max_waves = 10; /* max waves that can be stacked, then vertical pan can be used to view more */
  double wx1 = 0;
  double wy1 = 0;
  double wx2 = 1e-6;
  double wy2 = 5;
  double marginx = 20; /* will be recalculated later */
  double marginy = 20; /* will be recalculated later */
  /* coefficients for graph to container coordinate transformations W_X() and W_Y()*/
  double cx, dx, cy, dy, scx, sdx, scy, sdy;
  int divx = 4;
  int divy = 4;
  int subdivx = 0;
  int subdivy = 0;
  double unitx = 1.0;
  double unity = 1.0;
  int unitx_suffix = ' '; /* 'n' or 'u' or 'M' or 'k' ... */
  int unity_suffix = ' ';
  int wave_color = 5;
  const char *val;
  char *node = NULL, *color = NULL, *sweep = NULL;
  double txtsizelab, digtxtsizelab, txtsizex, tmp;
  Int_hashentry *entry;
  int sweep_idx = 0;
  int n_nodes; /* number of variables to display in a single graph */
  xRect *r = &xctx->rect[c][i];
  char tmpstr[200];
  char *saven, *savec, *saves, *nptr, *cptr, *sptr;
  const char *ntok, *ctok, *stok;
  char *bus_msb = NULL;
  int wcnt = 0;
  int dataset = -1; 

  /* container (embedding rectangle) coordinates */
  rx1 = r->x1;
  ry1 = r->y1;
  rx2 = r->x2;
  ry2 = r->y2;
  rw = (rx2 - rx1);
  /* rh = (ry2 -ry1); */
  /* get variables to plot, x/y range, grid info etc */
  val = get_tok_value(r->prop_ptr,"unitx",0);
  unitx_suffix = val[0];
  unitx = get_unit(val);;
  val = get_tok_value(r->prop_ptr,"unity",0);
  unity_suffix = val[0];
  unity = get_unit(val);;
  val = get_tok_value(r->prop_ptr,"subdivx",0);
  if(val[0]) subdivx = atoi(val);
  val = get_tok_value(r->prop_ptr,"subdivy",0);
  if(val[0]) subdivy = atoi(val);
  val = get_tok_value(r->prop_ptr,"divx",0);
  if(val[0]) divx = atoi(val);
  val = get_tok_value(r->prop_ptr,"divy",0);
  if(val[0]) divy = atoi(val);
  val = get_tok_value(r->prop_ptr,"x1",0);
  if(val[0]) wx1 = atof(val);
  val = get_tok_value(r->prop_ptr,"y1",0);
  if(val[0]) wy1 = atof(val);
  val = get_tok_value(r->prop_ptr,"x2",0);
  if(val[0]) wx2 = atof(val);
  val = get_tok_value(r->prop_ptr,"y2",0);
  if(val[0]) wy2 = atof(val);
  val = get_tok_value(r->prop_ptr,"digital",0);
  if(val[0]) digital = atoi(val);
  if(digital) {
    val = get_tok_value(r->prop_ptr,"dig_max_waves",0);
    if(val[0]) dig_max_waves = atoi(val);
  }

  /* plot single dataset */
  val = get_tok_value(r->prop_ptr,"dataset",0);
  if(val[0]) dataset = atoi(val);
  if(dataset >= xctx->graph_datasets) dataset =  xctx->graph_datasets - 1;
  /* set margins */
  calc_graph_area(c, i, digital, &x1, &y1, &x2, &y2, &marginx, &marginy);

  txtsizelab = marginy * 0.014;
  tmp =  (x2 - x1) * 0.00036;
  if(tmp < txtsizelab) txtsizelab = tmp;
  digtxtsizelab = txtsizelab * 0.73;
  /* cache coefficients for faster graph --> xschem coord transformations */
  cx = (x2 - x1) / (wx2 - wx1);
  dx = x1 - wx1 * cx;
  cy = (y1 - y2) / (wy2 - wy1);
  dy = y2 - wy1 * cy;

  /* direct graph --> screen transform */
  scx = cx * xctx->mooz;
  sdx = (dx + xctx->xorigin) * xctx->mooz;
  scy = cy * xctx->mooz;
  sdy = (dy + xctx->yorigin) * xctx->mooz;


  /* draw stuff */
  if(flags & 8) {
    /* graph box, gridlines and axes */
    draw_graph_grid(rx1, ry1, rx2, ry2, x1, y1, x2, y2, marginx, marginy,
                    wx1, wy1, wx2, wy2, cx, cy, dx, dy,
                    divx, divy, subdivx, subdivy, unitx, unity, digital, &txtsizex);
                    
    /* get data to plot */
    my_strdup2(1389, &node, get_tok_value(r->prop_ptr,"node",0));
    my_strdup2(1390, &color, get_tok_value(r->prop_ptr,"color",0)); 
    my_strdup2(1407, &sweep, get_tok_value(r->prop_ptr,"sweep",0)); 
    nptr = node;
    cptr = color;
    sptr = sweep;
    n_nodes = count_items(node, " \t\n");
    /* process each node given in "node" attribute, get also associated color/sweep var if any*/
    while( (ntok = my_strtok_r(nptr, "\n\t ", &saven)) ) {
      if(strstr(ntok, ",")) {
        my_strdup2(1452, &bus_msb, find_nth(ntok, ',', 2));
      }
      ctok = my_strtok_r(cptr, " ", &savec);
      stok = my_strtok_r(sptr, " ", &saves);
      nptr = cptr = sptr = NULL;
      dbg(1, "ntok=%s ctok=%s\n", ntok, ctok? ctok: "NULL");
      if(ctok && ctok[0]) wave_color = atoi(ctok);
      if(stok && stok[0]) {
        sweep_idx = get_raw_index(stok);
        if( sweep_idx == -1) {
          sweep_idx = 0;
        }
      }
      /* clipping everything outside container area */
      bbox(START, 0.0, 0.0, 0.0, 0.0);
      bbox(ADD, rx1, ry1, rx2, ry2);
      bbox(SET_INSIDE, 0.0, 0.0, 0.0, 0.0);
      /* draw sweep variable(s) on x-axis */
      if(wcnt == 0 || (stok && stok[0])) {
        if(xctx->graph_values) stok = xctx->graph_names[sweep_idx];
        if(unitx != 1.0) my_snprintf(tmpstr, S(tmpstr), "%s[%c]", stok ? stok : "" , unitx_suffix);
        else  my_snprintf(tmpstr, S(tmpstr), "%s", stok ? stok : "");
        draw_string(wave_color, NOW, tmpstr, 2, 1, 0, 0,
           rx1 + 2 + rw / n_nodes * wcnt, ry2-1, txtsizelab, txtsizelab);
      }
      /* draw node labels in graph */
      if(bus_msb) {
        if(unity != 1.0) my_snprintf(tmpstr, S(tmpstr), "%s[%c]", find_nth(ntok, ',', 1), unity_suffix);
        else  my_snprintf(tmpstr, S(tmpstr), "%s",find_nth(ntok, ',', 1));
      } else {
        if(unity != 1.0) my_snprintf(tmpstr, S(tmpstr), "%s[%c]", ntok, unity_suffix);
        else  my_snprintf(tmpstr, S(tmpstr), "%s", ntok);
      }
      if(digital) {
        /* int n = n_nodes > dig_max_waves ? dig_max_waves : n_nodes; */
        int n = dig_max_waves;
        double xt = x1 - 10 * txtsizelab;
        double delta_div_n = (wy2 - wy1) / n;
        double yt = delta_div_n * (double)wcnt;
  
        if(yt <= wy2 && yt >= wy1) {
          draw_string(wave_color, NOW, tmpstr, 2, 0, 0, 0, xt, W_Y(yt), digtxtsizelab, digtxtsizelab);
        }
      } else {
        draw_string(wave_color, NOW, tmpstr, 0, 0, 0, 0, rx1 + rw / n_nodes * wcnt, ry1, txtsizelab, txtsizelab);
      }
      bbox(END, 0.0, 0.0, 0.0, 0.0);

      /* quickly find index number of ntok variable to be plotted */
      entry = int_hash_lookup(xctx->raw_table, bus_msb ? bus_msb : ntok, 0, XLOOKUP);
      if(xctx->graph_values && entry) {
        int p, dset, ofs;
        int poly_npoints;
        int v;
        int first, last;
        double xx;
        double start;
        double end;
        XPoint *point = NULL;
  
        /* clipping everything outside graph area */
        bbox(START, 0.0, 0.0, 0.0, 0.0);
        bbox(ADD,x1, y1, x2, y2);
        bbox(SET, 0.0, 0.0, 0.0, 0.0);
  
        ofs = 0;
        v = entry->value;
        start = (wx1 <= wx2) ? wx1 : wx2;
        end = (wx1 <= wx2) ? wx2 : wx1;
        /* loop through all datasets found in raw file */
        for(dset = 0 ; dset < xctx->graph_datasets; dset++) {
          if(dataset == -1 || dset == dataset) {
            double prev_x, prev_prev_x;
            int cnt=0;
            first = last = -1;
            poly_npoints = 0;
            my_realloc(1401, &point, xctx->graph_npoints[dset] * sizeof(XPoint));
            /* Process "npoints" simulation items 
             * p loop split repeated 2 timed (for x and y points) to preserve cache locality */
            prev_prev_x = prev_x = 0;
            last = ofs; 
            for(p = ofs ; p < ofs + xctx->graph_npoints[dset]; p++) {
              xx = xctx->graph_values[sweep_idx][p];
              if(xx > end || xx < start ||
                  ((sweep_idx == 0 && cnt > 1) && 
                  SIGN(xx - prev_x) != SIGN(prev_x - prev_prev_x) ) ) {
                if(first != -1) {
                  /* get y-axis points */
                  if(bus_msb) {
                    if(digital) {
                      draw_graph_bus_points(ntok, first, last, cx, dx, cy, dy, wave_color,
                                   sweep_idx, digital, dig_max_waves, wcnt, n_nodes, wy1, wy2);
                    }
                  } else {
                    draw_graph_points(v, first, last, scy, sdy, point, wave_color,
                                 digital, dig_max_waves, wcnt, n_nodes, wy1, wy2);
                  }
                  poly_npoints = 0;
                  first = -1;
                  cnt = 0;
                }
              }
              if(xx >= start && xx <= end) {
                if(first == -1) first = p;
                /* Build poly x array. Translate from graph coordinates to {x1,y1} - {x2, y2} world. */
                point[poly_npoints].x = S_X(xx); /* CLIP(S_X(xx), xctx->areax1, xctx->areax2); */
                last = p;
                poly_npoints++;
              }
              prev_prev_x = prev_x;
              prev_x = xx;
              cnt++;
            }
            if(first != -1) {
              /* get y-axis points */
              if(bus_msb) {
                if(digital) {
                  draw_graph_bus_points(ntok, first, last, cx, dx, cy, dy, wave_color,
                               sweep_idx, digital, dig_max_waves, wcnt, n_nodes, wy1, wy2);
                }
              } else {
                draw_graph_points(v, first, last, scy, sdy, point, wave_color,
                             digital, dig_max_waves, wcnt, n_nodes, wy1, wy2);
              }
            }
          } /* if(dataset == -1 || dset == dataset) */
  
          /* offset pointing to next dataset */
          ofs += xctx->graph_npoints[dset];
        } /* for(dset...) */
        my_free(1403, &point);
        bbox(END, 0.0, 0.0, 0.0, 0.0);
      }/* if(entry) */
      wcnt++;
      if(bus_msb) my_free(1453, &bus_msb);
    } /* while( (ntok = my_strtok_r(nptr, "\n\t ", &saven)) ) */
    my_free(1391, &node);
    my_free(1392, &color);
    my_free(1408, &sweep);
  }
  if(flags & 2) { /* cursor1 */
    double xx = W_X(xctx->graph_cursor1_x);
    double tx1, ty1, tx2, ty2;
    int tmp;
    double txtsize = txtsizex;

    int flip = (xctx->graph_cursor2_x > xctx->graph_cursor1_x) ? 0 : 1;
    int xoffs = flip ? 2 : -2;
    if(xx >= x1 && xx <= x2) {
      drawline(3, NOW, xx, ry1, xx, ry2, 1);
      if(unitx != 1.0)
         my_snprintf(tmpstr, S(tmpstr), "%.4g%c", unitx * xctx->graph_cursor1_x , unitx_suffix);
      else
         my_snprintf(tmpstr, S(tmpstr), "%.4g",  xctx->graph_cursor1_x);
      text_bbox(tmpstr, txtsize, txtsize, 2, flip, 0, 0, xx + xoffs, ry2-1, &tx1, &ty1, &tx2, &ty2, &tmp, &tmp);
      filledrect(0, NOW,  tx1, ty1, tx2, ty2);
      draw_string(3, NOW, tmpstr, 2, flip, 0, 0, xx + xoffs, ry2-1, txtsize, txtsize);
    }
  }
  if(flags & 4) { /* cursor2 */
    double xx = W_X(xctx->graph_cursor2_x);
    double tx1, ty1, tx2, ty2;
    int tmp;
    double txtsize = txtsizex;
    int flip = (xctx->graph_cursor2_x > xctx->graph_cursor1_x) ? 1 : 0;
    int xoffs = flip ? 2 : -2;

    if(xx >= x1 && xx <= x2) {
      drawline(3, NOW, xx, ry1, xx, ry2, 1);
      if(unitx != 1.0)
         my_snprintf(tmpstr, S(tmpstr), "%.4g%c", unitx * xctx->graph_cursor2_x , unitx_suffix);
      else
         my_snprintf(tmpstr, S(tmpstr), "%.4g",  xctx->graph_cursor2_x);
      text_bbox(tmpstr, txtsize, txtsize, 2, flip, 0, 0, xx + xoffs, ry2-1, &tx1, &ty1, &tx2, &ty2, &tmp, &tmp);
      filledrect(0, NOW,  tx1, ty1, tx2, ty2);
      draw_string(3, NOW, tmpstr, 2, flip, 0, 0, xx + xoffs, ry2-1, txtsize, txtsize);

    }
  }
  if((flags & 6) == 6) { /* difference between cursors */
    int tmp;
    double txtsize = txtsizex;
    double tx1, ty1, tx2, ty2;
    double aa = W_X(xctx->graph_cursor1_x);
    double a = CLIP(aa, x1, x2);
    double bb = W_X(xctx->graph_cursor2_x);
    double b = CLIP(bb, x1, x2);
    double diff = fabs(b - a);
    double diffw = fabs(xctx->graph_cursor2_x - xctx->graph_cursor1_x);
    double xx = ( a + b ) / 2.0;
    double yy = ry2 - 1;
    double tmpd;
    double yline;
    if(unitx != 1.0)
       my_snprintf(tmpstr, S(tmpstr), "%.4g%c", unitx * diffw , unitx_suffix);
    else
       my_snprintf(tmpstr, S(tmpstr), "%.4g",  diffw);
    text_bbox(tmpstr, txtsize, txtsize, 2, 0, 1, 0, xx, yy, &tx1, &ty1, &tx2, &ty2, &tmp, &tmp);
    if( tx2 - tx1 < diff ) {
      draw_string(3, NOW, tmpstr, 2, 0, 1, 0, xx, yy, txtsize, txtsize);
      if( a > b) {
        tmpd = a; a = b; b = tmpd;
      }
      yline = (ty1 + ty2) / 2.0;
      if( tx1 - a > 4.0) drawline(3, NOW, a + 2, yline, tx1 - 2, yline, 1);
      if( b - tx2 > 4.0) drawline(3, NOW, tx2 + 2, yline, b - 2, yline, 1);
    }
  }
  if(flags & 1) { /* copy save buffer to screen */
    bbox(START, 0.0, 0.0, 0.0, 0.0);
    bbox(ADD, rx1, ry1, rx2, ry2);
    bbox(SET, 0.0, 0.0, 0.0, 0.0);
    if(!xctx->draw_window) {
      XCopyArea(display, xctx->save_pixmap, xctx->window, xctx->gctiled, xctx->xrect[0].x, xctx->xrect[0].y,
         xctx->xrect[0].width, xctx->xrect[0].height, xctx->xrect[0].x, xctx->xrect[0].y);
    }
    bbox(END, 0.0, 0.0, 0.0, 0.0);
  }
}

/* flags:
 * see draw_graph()
 */
void draw_graph_all(int flags)
{
  int c, i, sch_loaded, hide_graphs;
  int bbox_set = 0;
  const char *tmp;
  int save_bbx1, save_bby1, save_bbx2, save_bby2;
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
    c = GRIDLAYER; /* only rectangles on layer 2 (GRID) can embed graphs */
    if(xctx->draw_single_layer==-1 || c == xctx->draw_single_layer) {
      if(xctx->enable_layer[c]) for(i = 0; i < xctx->rects[c]; i++) {
        xRect *r = &xctx->rect[c][i];
        if(r->flags == 1) {
          draw_graph(c, i, flags); /* draw data in each graph box */
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

 #if HAS_CAIRO==1
 const char *textfont;
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
            if(c != GRIDLAYER || !(r->flags & 1) )  {
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
            if(textlayer < 0 ||  textlayer >= cadlayers) textlayer = TEXTLAYER;
            dbg(1, "draw(): drawing string %d = %s\n",i, xctx->text[i].txt_ptr);
            #if HAS_CAIRO==1
            if(!xctx->enable_layer[textlayer]) continue;
            textfont = xctx->text[i].font;
            if( (textfont && textfont[0]) || xctx->text[i].flags) {
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
            if((textfont && textfont[0]) || xctx->text[i].flags ) {
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
      XCopyArea(display, xctx->save_pixmap, xctx->window, xctx->gctiled, xctx->xrect[0].x, xctx->xrect[0].y,
         xctx->xrect[0].width, xctx->xrect[0].height, xctx->xrect[0].x, xctx->xrect[0].y);
    }
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
