/* File: psprint.c
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
#define X_TO_PS(x) ( (x+xctx->xorigin)* xctx->mooz )
#define Y_TO_PS(y) ( (y+xctx->yorigin)* xctx->mooz )

char *utf8_enc[]={
  "/recodedict 24 dict def\n",
  "/recode {recodedict begin /nco&na exch def\n",
  "/nfnam exch def /basefontname exch def /basefontdict basefontname findfont def\n",
  "/newfont basefontdict maxlength dict def basefontdict {exch dup /FID ne\n",
  "{dup /Encoding eq {exch dup length array copy newfont 3 1 roll put} {exch\n",
  "newfont 3 1 roll put} ifelse} {pop pop} ifelse } forall newfont\n",
  "/FontName nfnam put nco&na aload pop nco&na length 2 idiv {newfont\n",
  "/Encoding get 3 1 roll put} repeat nfnam newfont definefont pop end } def\n",
};

char *utf8[]={
"/chararr [\n",
/* 0xC2 2-byte characters */
" 161 /exclamdown\n", " 162 /cent\n", " 163 /sterling\n", " 164 /currency\n",
" 165 /yen\n", " 166 /bar\n", " 167 /section\n", " 168 /dieresis\n",
" 169 /copyright\n", " 170 /ordfeminine\n", " 171 /guillemotleft\n", " 172 /logicalnot\n",
" 173 /emdash\n", " 174 /registered\n", " 175 /macron\n", " 176 /degree\n",
" 177 /plusminus\n", " 178 /twosuperior\n", " 179 /threesuperior\n", " 180 /acute\n",
" 181 /mu\n", " 182 /paragraph\n", " 183 /periodcentered\n", " 184 /cedilla\n",
" 185 /onesuperior\n", " 186 /ordmasculine\n", " 187 /guillemotright\n", " 188 /onequarter\n",
" 189 /onehalf\n", " 190 /threequarters\n", " 191 /questiondown\n",
/* 0xC3 2-byte characters */
" 192 /Agrave\n", " 193 /Aacute\n", " 194 /Acircumflex\n", " 195 /Atilde\n",
" 196 /Adieresis\n", " 197 /Aring\n", " 198 /AE\n", " 199 /Ccedilla\n",
" 200 /Egrave\n", " 201 /Eacute\n", " 202 /Ecircumflex\n", " 203 /Edieresis\n",
" 204 /Igrave\n", " 205 /Iacute\n", " 206 /Icircumflex\n", " 207 /Idieresis\n",
" 208 /Eth\n", " 209 /Ntilde\n", " 210 /Ograve\n", " 211 /Oacute\n",
" 212 /Ocircumflex\n", " 213 /Otilde\n", " 214 /Odieresis\n", " 215 /multiply\n",
" 216 /Oslash\n", " 217 /Ugrave\n", " 218 /Uacute\n", " 219 /Ucircumflex\n",
" 220 /Udieresis\n", " 221 /Yacute\n", " 222 /Thorn\n", " 223 /germandbls\n",
" 224 /agrave\n", " 225 /aacute\n", " 226 /acircumflex\n", " 227 /atilde\n",
" 228 /adieresis\n", " 229 /aring\n", " 230 /ae\n", " 231 /ccedilla\n",
" 232 /egrave\n", " 233 /eacute\n", " 234 /ecircumflex\n", " 235 /edieresis\n",
" 236 /igrave\n", " 237 /iacute\n", " 238 /icircumflex\n", " 239 /idieresis\n",
" 240 /eth\n", " 241 /ntilde\n", " 242 /ograve\n", " 243 /oacute\n",
" 244 /ocircumflex\n", " 245 /otilde\n", " 246 /odieresis\n", " 247 /divide\n",
" 248 /oslash\n", " 249 /ugrave\n", " 250 /uacute\n", " 251 /ucircumflex\n",
" 252 /udieresis\n", " 253 /yacute\n", " 254 /thorn\n", " 255 /ydieresis\n",
" ] def\n"};

static FILE *fd;

typedef struct {
 unsigned int red;
 unsigned int green;
 unsigned int blue;
} Ps_color;

static Ps_color *ps_colors;
static char ps_font_name[80] = "Helvetica"; /* Courier Times Helvetica Symbol */
static char ps_font_family[80] = "Helvetica"; /* Courier Times Helvetica Symbol */

static int ps_embedded_image(xRect* r, double x1, double y1, double x2, double y2, int rot, int flip)
{
  #if defined(HAS_LIBJPEG) && HAS_CAIRO==1
  int i, jpg;
  int size_x, size_y;
  unsigned char *ptr = NULL;
  int invertImage;
  unsigned char* ascii85EncodedJpeg;
  char *attr = NULL;
  unsigned char *buffer = NULL;
  cairo_surface_t *surface = NULL, *orig_sfc = NULL;
  xEmb_image *emb_ptr;
  unsigned char* jpgData = NULL;
  size_t fileSize = 0;
  int quality=40;
  const char *quality_attr, *filter;
  size_t oLength, attr_len, buffer_size;
  cairo_t *ct;
  double sx1, sy1, sx2, sy2;

  /* screen position */
  sx1=X_TO_SCREEN(x1);
  sy1=Y_TO_SCREEN(y1); 
  sx2=X_TO_SCREEN(x2);
  sy2=Y_TO_SCREEN(y2);
  if(RECT_OUTSIDE(sx1, sy1, sx2, sy2,
                  xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2)) return 0;

  invertImage = !strboolcmp(get_tok_value(r->prop_ptr, "InvertOnExport", 0), "true");
  if(!invertImage)
    invertImage = !strboolcmp(get_tok_value(r->prop_ptr, "ps_invert", 0), "true");
  quality_attr = get_tok_value(r->prop_ptr, "jpeg_quality", 0);
  if(quality_attr[0]) quality = atoi(quality_attr);
  else {
    quality_attr = get_tok_value(r->prop_ptr, "jpg_quality", 0);
    if(quality_attr[0]) quality = atoi(quality_attr);
  }
  attr_len = my_strdup2(_ALLOC_ID_, &attr, get_tok_value(r->prop_ptr, "image_data", 0));
  filter = get_tok_value(r->prop_ptr, "filter", 0);
  buffer = base64_decode(attr, attr_len, &buffer_size);
  if(attr_len > 5) {
    if(!strncmp(attr, "/9j/", 4)) jpg = 1; /* jpg */
    else if(!strncmp(attr, "iVBOR", 5)) jpg = 0; /* png */
    else if(my_memmem(buffer, buffer_size, "<svg", 4) &&
            my_memmem(buffer, buffer_size, "xmlns", 5)) {
      if(filter) {
        jpg = 2; /* svg */
      }
    } 
    else jpg = -1; /* some invalid data */
  } else {
    jpg = -1;
  }
  emb_ptr = r->extraptr;
  if(jpg == -1 || !(emb_ptr && emb_ptr->image)) {
    my_free(_ALLOC_ID_, &buffer);
    my_free(_ALLOC_ID_, &attr);
    return 0;
  }
  orig_sfc = emb_ptr->image;
  cairo_surface_flush(orig_sfc);
  /* create a copy of image surface with no alpha */
  size_x = cairo_image_surface_get_width(orig_sfc);
  size_y = cairo_image_surface_get_height(orig_sfc);
  surface = cairo_surface_create_similar_image(orig_sfc, CAIRO_FORMAT_RGB24, size_x, size_y);
  ct = cairo_create(surface);
  cairo_set_source_surface(ct, orig_sfc, 0, 0);
  cairo_set_operator(ct, CAIRO_OPERATOR_SOURCE);
  cairo_paint(ct);
  cairo_destroy(ct);

  ptr = cairo_image_surface_get_data(surface);

  for (i = 0; i < (size_x * size_y * 4); i += 4)
  {
    unsigned char a = ptr[i + 3];
    unsigned char r = ptr[i + 2];
    unsigned char g = ptr[i + 1];
    unsigned char b = ptr[i + 0];
    /* invert colors */
    if(invertImage) {
      r = a - r;
      g = a - g;
      b = a - b;
    }
    /* blend with white, remove alpha */
    r += (unsigned char)(0xff - a);
    g += (unsigned char)(0xff - a);
    b += (unsigned char)(0xff - a);
    a  = (unsigned char) 0xff;
    /* write result back */
    ptr[i + 3] = a;
    ptr[i + 2] = r;
    ptr[i + 1] = g;
    ptr[i + 0] = b;
  }
  cairo_surface_mark_dirty(surface);
  if(invertImage || jpg != 1) {
    cairo_image_surface_write_to_jpeg_mem(surface, &jpgData, &fileSize, quality);
  } else {
    jpgData = base64_decode(attr, attr_len, &fileSize);
  }
  cairo_surface_destroy(surface);
  ascii85EncodedJpeg = ascii85_encode(jpgData, fileSize, &oLength);
  fprintf(fd, "gsave\n");
  fprintf(fd, "save\n");
  fprintf(fd, "/RawData currentfile /ASCII85Decode filter def\n");
  fprintf(fd, "/Data RawData << >> /DCTDecode filter def\n");
  fprintf(fd, "%g %g translate\n", X_TO_PS(x1), Y_TO_PS(y1));
  if(rot==1) fprintf(fd, "90 rotate\n");
  if(rot==2) fprintf(fd, "180 rotate\n");
  if(rot==3) fprintf(fd, "270 rotate\n");
  fprintf(fd, "%g %g scale\n", (X_TO_PS(x2) - X_TO_PS(x1))*1., (Y_TO_PS(y2) - Y_TO_PS(y1))*1.);
  fprintf(fd, "/DeviceRGB setcolorspace\n");
  fprintf(fd, "{ << /ImageType 1\n");
  fprintf(fd, "     /Width %g\n", (double)size_x);
  fprintf(fd, "     /Height %g\n", (double)size_y);
  
  if(!flip)
  {
    if(rot==1) fprintf(fd, "     /ImageMatrix [%g 0 0 %g 0 %g]\n",
           (double)size_y, (double)size_x, (double)size_y);
    else if(rot==2) fprintf(fd, "     /ImageMatrix [%g 0 0 %g %g %g]\n",
           (double)size_x, (double)size_y, (double)size_x, (double)size_y);
    else if(rot==3) fprintf(fd, "     /ImageMatrix [%g 0 0 %g %g 0]\n", 
           (double)size_y, (double)size_x, (double)size_x);
    else fprintf(fd, "     /ImageMatrix [%g 0 0 %g 0 0]\n", (double)size_x, (double)size_y); 
  }
  else
  {
    if(rot==1) fprintf(fd, "     /ImageMatrix [%g 0 0 %g %g %g]\n",
          -(double)size_y, (double)size_x, (double)size_x, (double)size_y);
    else if(rot==2) fprintf(fd, "     /ImageMatrix [%g 0 0 %g 0 %g]\n",
          -(double)size_x, (double)size_y, (double)size_y);
    else if(rot==3) fprintf(fd, "     /ImageMatrix [%g 0 0 %g 0 0]\n",
          -(double)size_y, (double)size_x);
    else fprintf(fd, "     /ImageMatrix [%g 0 0 %g %g 0]\n",
          -(double)size_x, (double)size_y, (double)size_x); 
  }
  fprintf(fd, "     /DataSource Data\n");
  fprintf(fd, "     /BitsPerComponent 8\n");
  fprintf(fd, "     /Decode [0 1 0 1 0 1]\n");
  fprintf(fd, "  >> image\n");
  fprintf(fd, "  Data closefile\n");
  fprintf(fd, "  RawData flushfile\n");
  fprintf(fd, "  restore\n");
  fprintf(fd, "} exec\n");

  #if 1  /* break lines */
  for (i = 0; i < oLength; ++i)
  {
    fputc(ascii85EncodedJpeg[i],fd);
    if(i > 0 && (i % 64) == 0)
    {
      fputc('\n',fd);
      /* if (ascii85Encode[i+1]=='%') idx=63; imageMagic does this for some reason?!
       *  Doesn't seem to be necesary. */
    }
  }
  #else
  fprintf(fd, "%s", ascii85EncodedJpeg);
  #endif
  fprintf(fd, "~>\n");
  
  fprintf(fd, "grestore\n");
  my_free(_ALLOC_ID_, &ascii85EncodedJpeg);
  free(jpgData);
  my_free(_ALLOC_ID_, &buffer);
  my_free(_ALLOC_ID_, &attr);
  #endif
  return 1;
}

static int ps_embedded_graph(int i, double rx1, double ry1, double rx2, double ry2)
{
  #if defined(HAS_LIBJPEG) && HAS_CAIRO==1
  xRect *r = &xctx->rect[GRIDLAYER][i];
  Zoom_info zi;
  double  rw, rh, scale;
  cairo_surface_t* png_sfc;
  int save, save_draw_window, save_draw_grid, rwi, rhi;
  const double max_size = 2500.0;
  int d_c;
  unsigned char* jpgData = NULL;
  size_t fileSize = 0;
  /* 
   * FILE* fp;
   * static char str[PATH_MAX];
   */
  unsigned char *ascii85EncodedJpeg;
  int quality=40;
  const char *quality_attr;
  size_t oLength;
  int j;
  double sx1, sy1, sx2, sy2;

  /* screen position */
  sx1=X_TO_SCREEN(rx1);
  sy1=Y_TO_SCREEN(ry1);
  sx2=X_TO_SCREEN(rx2);
  sy2=Y_TO_SCREEN(ry2);
  if(RECT_OUTSIDE(sx1, sy1, sx2, sy2,
                  xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2)) return 0;

  quality_attr = get_tok_value(r->prop_ptr, "jpeg_quality", 0);
  if(quality_attr[0]) quality = atoi(quality_attr);
  else {
    quality_attr = get_tok_value(r->prop_ptr, "jpg_quality", 0);
    if(quality_attr[0]) quality = atoi(quality_attr);
  }
  if(quality_attr[0]) quality = atoi(quality_attr);
  if (!has_x) return 0;

  rw = fabs(rx2 - rx1);
  rh = fabs(ry2 - ry1);
  scale = 3.0;
  if (rw > rh && rw * scale  > max_size) {
    scale = max_size / rw;
  }
  else if (rh * scale  > max_size) {
    scale = max_size / rh;
  }
  rwi = (int)(rw * scale + 1.0);
  rhi = (int)(rh * scale + 1.0);
  dbg(1, "graph size, saving zoom : %dx%d\n", rwi, rhi);
  save_restore_zoom(1, &zi);
  set_viewport_size(rwi, rhi, xctx->lw);

  /* zoom_box(rx1 - xctx->lw, ry1 - xctx->lw, rx2 + xctx->lw, ry2 + xctx->lw, 1.0); */

  xctx->xorigin = -rx1;
  xctx->yorigin = -ry1;
  xctx->zoom=(rx2-rx1)/(rwi - 1);
  xctx->mooz = 1 / xctx->zoom;

  resetwin(1, 1, 1, rwi, rhi);
  dbg(1, "lw=%g\n", xctx->lw);
  save_draw_grid = tclgetboolvar("draw_grid");
  tclsetvar("draw_grid", "0");
  save_draw_window = xctx->draw_window;
  xctx->draw_window = 0;
  xctx->draw_pixmap = 1;
  save = xctx->do_copy_area;
  xctx->do_copy_area = 0;
  d_c = tclgetboolvar("dark_colorscheme");
  tclsetboolvar("dark_colorscheme", 0);
  build_colors(0, 0);
  setup_graph_data(i, 0, &xctx->graph_struct);
  draw_graph(i, 8 + (xctx->graph_flags & (4 | 2 | 128 | 256)), &xctx->graph_struct, NULL);

  dbg(1, "width=%d, rwi=%d height=%d rhi=%d\n", xctx->xrect[0].width, rwi, xctx->xrect[0].height, rhi);
  #ifdef __unix__
  png_sfc = cairo_xlib_surface_create(display, xctx->save_pixmap, visual,
     xctx->xrect[0].width, xctx->xrect[0].height);
  #else
  /* pixmap doesn't work on windows
       Copy from cairo_save_sfc and use cairo
       to draw in the data points to embed the graph */
  png_sfc = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, xctx->xrect[0].width, xctx->xrect[0].height);
  cairo_t* ct = cairo_create(png_sfc);
  cairo_set_source_surface(ct, xctx->cairo_save_sfc, 0, 0);
  cairo_set_operator(ct, CAIRO_OPERATOR_SOURCE);
  cairo_paint(ct);
  setup_graph_data(i, 0, &xctx->graph_struct); 
  draw_graph(i, 8 + (xctx->graph_flags & (4 | 2 | 128 | 256)), &xctx->graph_struct, (void *)ct);
  #endif
  cairo_image_surface_write_to_jpeg_mem(png_sfc, &jpgData, &fileSize, quality);

  ascii85EncodedJpeg = ascii85_encode(jpgData, fileSize, &oLength);
  free(jpgData);

  cairo_surface_destroy(png_sfc);
  xctx->draw_pixmap = 1;
  tclsetboolvar("draw_grid", save_draw_grid);
  save_restore_zoom(0, &zi);
  resetwin(1, 1, 1, xctx->xrect[0].width, xctx->xrect[0].height);
  dbg(1, "restore zoom + resetwin: %dx%d\n", xctx->xrect[0].width, xctx->xrect[0].height);
  change_linewidth(xctx->lw);
  tclsetboolvar("dark_colorscheme", d_c);
  build_colors(0, 0);
  draw();
  xctx->do_copy_area = save;
  xctx->draw_window = save_draw_window;
  fprintf(fd, "gsave\n");
  fprintf(fd, "save\n");
  fprintf(fd, "/RawData currentfile /ASCII85Decode filter def\n");
  fprintf(fd, "/Data RawData << >> /DCTDecode filter def\n");
  fprintf(fd, "%f %f translate\n", X_TO_PS(rx1), Y_TO_PS(ry1));
  fprintf(fd, "%f %f scale\n", X_TO_PS(rx2) - X_TO_PS(rx1), Y_TO_PS(ry2) - Y_TO_PS(ry1));
  fprintf(fd, "/DeviceRGB setcolorspace\n");
  fprintf(fd, "{ << /ImageType 1\n");
  fprintf(fd, "     /Width %d\n", rwi);
  fprintf(fd, "     /Height %d\n", rhi);
  fprintf(fd, "     /ImageMatrix [%d 0 0 %d 0 0]\n", rwi, rhi);
  fprintf(fd, "     /DataSource Data\n");
  fprintf(fd, "     /BitsPerComponent 8\n");
  fprintf(fd, "     /Decode [0 1 0 1 0 1]\n");
  fprintf(fd, "  >> image\n");
  fprintf(fd, "  Data closefile\n");
  fprintf(fd, "  RawData flushfile\n");
  fprintf(fd, "  restore\n");
  fprintf(fd, "} exec\n");

  #if 1 /* break lines */
  for (j = 0; j < oLength; ++j)
  {
    fputc(ascii85EncodedJpeg[j],fd);
    if(j > 0 && (j % 64) == 0) 
    {
      fputc('\n',fd);
      /* if (ascii85Encode[i+1]=='%') idx=63; imageMagic does this for some reason?!
       *  Doesn't seem to be necesary. */
    }
  }
  #else
  fprintf(fd, "%s", ascii85EncodedJpeg);
  #endif
  fprintf(fd, "~>\n");
  
  fprintf(fd, "grestore\n");

  my_free(_ALLOC_ID_, &ascii85EncodedJpeg);
  
  #endif
  return 1;
}
static void set_lw(double lw)
{
 if(lw==0.0)
   fprintf(fd, "%g setlinewidth\n", 0.5);
 else
   fprintf(fd, "%g setlinewidth\n", lw / 1.2);
}

static void set_ps_colors(unsigned int pixel)
{

   dbg(1, "set_ps_colors(): setting color %u\n", pixel);
   if(color_ps) fprintf(fd, "%g %g %g RGB\n",
     (double)ps_colors[pixel].red/256.0, (double)ps_colors[pixel].green/256.0,
     (double)ps_colors[pixel].blue/256.0);

}

static void ps_xdrawarc(int layer, int fillarc, double x, double y, double r, double a, double b)
{
 if(xctx->fill_pattern && xctx->fill_type[layer]  && fillarc)
   fprintf(fd, "%g %g MT %g %g %g %g %g A %g %g LT C F S\n",
                 x, y,  x, y, r, -a, -a-b, x, y);
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

/* fill_pattern:
 * 0 : no fill
 * 1 : stippled fill
 * 2 : solid fill
 *
 * fill_type[i]: 
 * 0 : no fill
 * 1 : patterned (stippled) fill
 * 2 : solid fill
 *
 * fill:
 * 0 : no fill
 * 1 : stippled fill
 * 2 : solid fill
 */

static void ps_xfillrectangle(int layer, double x1, double y1, double x2,
                  double y2, int fill)
{
 fprintf(fd, "%g %g %g %g R\n", x1,y1,x2-x1,y2-y1);
 if(xctx->fill_pattern && xctx->fill_type[layer] && fill) {
   fprintf(fd, "%g %g %g %g RF\n", x1,y1,x2-x1,y2-y1);
   /* fprintf(fd,"fill\n"); */
 }
 /*fprintf(fd,"stroke\n"); */
}

static void ps_drawbezier(double *x, double *y, int points)
{
  const double bez_steps = 1.0/32.0; /* divide the t = [0,1] interval into 32 steps */
  int b, i;
  double t;
  double xp, yp;
  double x0, x1, x2, y0, y1, y2;

  i = 0;
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
    for(t = 0; t <= 1.0; t += bez_steps) {
      xp = (1 - t) * (1 - t) * x0 + 2 * (1 - t) * t * x1 + t * t * x2;
      yp = (1 - t) * (1 - t) * y0 + 2 * (1 - t) * t * y1 + t * t * y2;
      if(i==0) fprintf(fd, "NP\n%g %g MT\n",  X_TO_PS(xp), Y_TO_PS(yp));
      else fprintf(fd, "%g %g LT\n",  X_TO_PS(xp), Y_TO_PS(yp));
      i++;
    }
  }
}

/* Convex Nonconvex Complex */
#define Polygontype Nonconvex
static void ps_drawpolygon(int c, int what, double *x, double *y, int points,
                           int poly_fill, int dash, int flags, int bus)
{
  double x1,y1,x2,y2;
  double xx, yy;
  double psdash;
  int i, bezier;
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
  if(bus) set_lw(BUS_WIDTH * xctx->lw);
  bezier = flags && (points > 2);
  if(bezier) {
    ps_drawbezier(x, y, points);
  } else {
    for(i=0;i<points; ++i) {
      xx = X_TO_PS(x[i]);
      yy = Y_TO_PS(y[i]);
      if(i==0) fprintf(fd, "NP\n%g %g MT\n", xx, yy);
      else fprintf(fd, "%g %g LT\n", xx, yy);
    }
  }
  if(xctx->fill_pattern && xctx->fill_type[c] && poly_fill) {
    fprintf(fd, "GS C F GR S\n");
  } else {
    fprintf(fd, "S\n");
  }
  if(dash) {
    fprintf(fd, "[] 0 setdash\n");
  }
  if(bus) set_lw(xctx->lw);
}


static void ps_filledrect(int gc, double rectx1,double recty1,double rectx2,double recty2,
     int dash, int fill, int e_a, int e_b)
{
 double x1,y1,x2,y2;
 double psdash;

  x1=X_TO_PS(rectx1);
  y1=Y_TO_PS(recty1);
  x2=X_TO_PS(rectx2);
  y2=Y_TO_PS(recty2);
  if(rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2))
  {
    psdash = dash / xctx->zoom;
    if(dash) {
      fprintf(fd, "[%g %g] 0 setdash\n", psdash, psdash);
    }
    if(e_a != -1) {
      double rx = (x2 - x1) / 2.0;
      double ry = (y2 - y1) / 2.0;
      double cx = (x2 + x1) / 2.0;
      double cy = (y2 + y1) / 2.0;

      if(xctx->fill_pattern && xctx->fill_type[gc] && fill) {
        fprintf(fd, "%g %g MT %g %g %g %g %d %d E %g %g LT C F S\n",
                    cx, cy, cx, cy, rx, ry, -e_a, -e_a-e_b, cx, cy);
      } else {
        fprintf(fd, "%g %g %g %g %d %d E S\n", cx, cy, rx, ry, -e_a, -e_a-e_b);
      }
    } else {
      ps_xfillrectangle(gc, x1,y1,x2,y2, fill);
    }
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


static void ps_drawline(int gc, double linex1,double liney1,double linex2,double liney2, int dash, int bus)
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
    if(dash) fprintf(fd, "[%g %g] 0 setdash\n", psdash, psdash);
    if(bus) set_lw(xctx->lw * BUS_WIDTH);
    ps_xdrawline(gc, x1, y1, x2, y2);
    if(dash) fprintf(fd, "[] 0 setdash\n");
    if(bus) set_lw(xctx->lw);
  }
}



static void ps_draw_string_line(int layer, char *s, double x, double y, double size, 
           short rot, short flip, int lineno, double fontheight, double fontascent, 
           double fontdescent, int llength, int no_of_lines, double longest_line)
{
  double ix, iy;
  short rot1;
  unsigned char c, offset;
  double line_delta;
  double lines;
  dbg(1, "ps_draw_string_line(): drawing |%s| on layer %d\n", s, layer);
  if(s==NULL) return;
  if(llength==0) return;
  fprintf(fd, "GS\n");
  set_ps_colors(layer);

  line_delta = lineno*fontheight;
  lines = (no_of_lines-1)*fontheight;

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

  fprintf(fd, "/%s", ps_font_family);
  fprintf(fd, " FF\n");
  fprintf(fd, "%g SCF\n", size * xctx->mooz);
  fprintf(fd, "SF\n");
  fprintf(fd, "NP\n");
  fprintf(fd, "%g %g MT\n", ix, iy);
  if(rot1) fprintf(fd, "%d rotate\n", rot1*90);
  fprintf(fd, "1 -1 scale\n");
  fprintf(fd, "(");
  offset = 0;
  while( (c = (unsigned char) *s) ) {
    if(c > 127) {
      if(c == 195) {offset = 64;s++; continue;}
      if(c == 194) {s++; continue;}
      fprintf(fd, "\\%03o", c + offset);
      offset = 0;
    } else
    switch(c) {
      case '(':
        fputs("\\(", fd);
        break;
      case ')':
        fputs("\\)", fd);
        break;
      default:
       fputc(c, fd);
    }
    ++s;
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
  char *estr = NULL; /* expanded string: TABs replaced with spaces */
  int lineno=0;
  double size, height, ascent, descent;
  int llength=0, no_of_lines;
  double longest_line;

  if(str==NULL) return;
  size = xscale*53. * cairo_font_scale;
  height =  size*xctx->mooz * 1.147; /* was 1.147 */
  ascent =  size*xctx->mooz * 0.808; /* was 0.908 */
  descent = size*xctx->mooz * 0.219; /* was 0.219 */

  estr = my_expand(str, tclgetintvar("tabstop"));
  text_bbox(estr, xscale, yscale, rot, flip, hcenter, vcenter,
          x,y, &textx1,&texty1,&textx2,&texty2, &no_of_lines, &longest_line);
  
  if(!textclip(xctx->areax1,xctx->areay1,xctx->areax2,
               xctx->areay2,textx1,texty1,textx2,texty2)) {
    my_free(_ALLOC_ID_, &estr);
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
  my_strdup2(_ALLOC_ID_, &sss, estr);
  tt=ss=sss;
  for(;;) {
    c=*ss;
    if(c=='\n' || c==0) {
      *ss='\0';
      ps_draw_string_line(layer, tt, x, y, size, rot, flip, lineno, 
              height, ascent, descent, llength, no_of_lines, longest_line);
      ++lineno;
      if(c==0) break;
      *ss='\n';
      tt=ss+1;
      llength=0;
    } else {
      ++llength;
    }
    ++ss;
  }
  my_free(_ALLOC_ID_, &sss);
  my_free(_ALLOC_ID_, &estr);
}

static void old_ps_draw_string(int gctext,  const char *str,
                 short rot, short flip, int hcenter, int vcenter,
                 double x1,double y1,
                 double xscale, double yscale)

{
 double a,yy,curr_x1,curr_y1,curr_x2,curr_y2,rx1,rx2,ry1,ry2;
 int pos=0,cc,pos2=0;
 int i, no_of_lines;
 double longest_line;
 char *estr = NULL;

 if(str==NULL) return;
 estr = my_expand(str, tclgetintvar("tabstop"));
 #if HAS_CAIRO==1
 text_bbox_nocairo(estr, xscale, yscale, rot, flip, hcenter, vcenter,
                   x1,y1, &rx1,&ry1,&rx2,&ry2, &no_of_lines, &longest_line);
 #else
 text_bbox(estr, xscale, yscale, rot, flip, hcenter, vcenter,
           x1,y1, &rx1,&ry1,&rx2,&ry2, &no_of_lines, &longest_line);
 #endif

 if(!textclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,rx1,ry1,rx2,ry2)) {
   my_free(_ALLOC_ID_, &estr);
   return;
 }
 xscale*=tclgetdoublevar("nocairo_font_xscale") * cairo_font_scale;
 yscale*=tclgetdoublevar("nocairo_font_yscale") * cairo_font_scale;
 set_ps_colors(gctext);
 x1=rx1;y1=ry1;
 if(rot&1) {y1=ry2;rot=3;}
 else rot=0;
 flip = 0; yy=y1;
 while(estr[pos2])
 {
  cc = (unsigned char)estr[pos2++];
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
   ps_drawline(gctext,  rx1, ry1, rx2, ry2, 0, 0);
  }
  ++pos;
 }
 my_free(_ALLOC_ID_, &estr);
}

static void ps_drawgrid()
{
 double x,y;
 double delta,tmp;
 if( !tclgetboolvar("draw_grid")) return;
 delta=tclgetdoublevar("cadgrid")* xctx->mooz;
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
 tmp = floor((xctx->areay1+1)/delta)*delta-fmod(-xctx->yorigin* xctx->mooz,delta);
 for(x=floor((xctx->areax1+1)/delta)*delta-fmod(-xctx->xorigin* xctx->mooz,delta);x<xctx->areax2;x+=delta)
 {
  for(y=tmp;y<xctx->areay2;y+=delta)
  {
   ps_xdrawpoint(GRIDLAYER,(int)(x), (int)(y));
  }
 }
}



static void ps_draw_symbol(int c, int n,int layer, int what, short tmp_flip, short rot,
        double xoffset, double yoffset)
                            /* draws current layer only, should be called within  */
{                           /* a "for(i=0;i<cadlayers; ++i)" loop */
  int j, hide = 0, disabled = 0;
  double x0,y0,x1,y1,x2,y2;
  short flip; 
  int textlayer;
  xLine *line;
  xRect *rect;
  xText text;
  xArc *arc;
  xPoly *polygon;
  xSymbol *symptr;
  char *type;
  int lvs_ignore = 0;
  char *textfont;
  int c_for_text;

  type = xctx->sym[xctx->inst[n].ptr].type;
  lvs_ignore=tclgetboolvar("lvs_ignore");
  if(xctx->inst[n].ptr == -1) return;
  if(layer == 0) { 
    xctx->inst[n].flags &= ~IGNORE_INST; /* clear bit */
    if( type && strcmp(type, "launcher") && strcmp(type, "logo") &&
        strcmp(type, "probe") &&
        strcmp(type, "architecture") && strcmp(type, "noconn")) {
      if(skip_instance(n, 1, lvs_ignore)) {
        xctx->inst[n].flags |= IGNORE_INST;
      }
    }
  }

  if(shorted_instance(n, lvs_ignore)) {
    c = PINLAYER;
    disabled = 2;
  }
  else if(xctx->inst[n].flags & IGNORE_INST) {
    c = GRIDLAYER;
    disabled = 1;
  }
  if(xctx->inst[n].color != -10000) c = get_color(xctx->inst[n].color);

  if( (xctx->inst[n].flags & HIDE_INST) || ((xctx->inst[n].ptr + xctx->sym)->flags & HIDE_INST) ||
      ((xctx->hide_symbols==1 && (xctx->inst[n].ptr + xctx->sym)->type &&
      !strcmp( (xctx->inst[n].ptr+ xctx->sym)->type, "subcircuit") )) ||
      (xctx->hide_symbols == 2) ) {
    hide = 1;
  } else {
    hide = 0;
  }
  if(layer==0)
  {
    x1=X_TO_PS(xctx->inst[n].x1);
    x2=X_TO_PS(xctx->inst[n].x2);
    y1=Y_TO_PS(xctx->inst[n].y1);
    y2=Y_TO_PS(xctx->inst[n].y2);
    if(RECT_OUTSIDE(x1,y1,x2,y2,xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2))
    {
      xctx->inst[n].flags|=1;
      return;
    }
    #if 0
    else if(
         xctx->hilight_nets &&                  /* if highlights...                       */
         c == 0 &&                              /* we are not drawing highlighted inst    */
                                                /* otherwise c > layer...                 */
         type  &&                               /* ... and type...                        */
         (
          (                                     /* ... and inst is hilighted ...          */
            IS_LABEL_SH_OR_PIN(type) && xctx->inst[n].node && xctx->inst[n].node[0] &&
            bus_hilight_hash_lookup(xctx->inst[n].node[0], 0, XLOOKUP )
          ) || (/* !IS_LABEL_SH_OR_PIN(type) && */ (xctx->inst[n].color != -10000)) )) {
      xctx->inst[n].flags|=1;      /* ... then SKIP instance now and for following layers */
      return;
    }
    #endif
    else if((xctx->inst[n].x2 - xctx->inst[n].x1) * xctx->mooz < 3 &&
                       (xctx->inst[n].y2 - xctx->inst[n].y1) * xctx->mooz < 3) {
      set_ps_colors(SYMLAYER);
      ps_filledrect(SYMLAYER, xctx->inst[n].xx1, xctx->inst[n].yy1, xctx->inst[n].xx2, xctx->inst[n].yy2, 0, 0, -1, -1);
      xctx->inst[n].flags|=1;
      return;
    }
    else {
      xctx->inst[n].flags&=~1;
    }
    if(hide) {
      int color = (disabled==1) ? GRIDLAYER : (disabled == 2) ? PINLAYER : SYMLAYER;
      set_ps_colors(color);
      ps_filledrect(color, xctx->inst[n].xx1, xctx->inst[n].yy1, xctx->inst[n].xx2, xctx->inst[n].yy2, 2, 0, -1, -1);
    }
    /* pdfmarks, only if doing hierarchy print and if symbol has a subcircuit */ 
    if(what != 7) {
      char fname[PATH_MAX];
      if(!strcmp(xctx->sym[xctx->inst[n].ptr].type, "subcircuit")) {
        get_sch_from_sym(fname, xctx->inst[n].ptr+ xctx->sym, n, 0);
        fprintf(fd, 
          "[ "
          "/Rect [ %g %g %g %g ] "
          "/Border [0 0 0] "
          "/Dest /%s "
          "/Subtype /Link "
          "/ANN pdfmark\n",
          x1, y1, x2, y2,
          get_cell_w_ext(sanitize(fname), 0));
      }
    }
  }
  else if(xctx->inst[n].flags&1)
  {
   dbg(1, "ps_draw_symbol(): skipping inst %d\n", n);
   return;
  }
  flip = xctx->inst[n].flip;
  if(tmp_flip) flip = !flip;
  rot = (xctx->inst[n].rot + rot ) & 0x3;

  x0=xctx->inst[n].x0 + xoffset;
  y0=xctx->inst[n].y0 + yoffset;
  symptr = (xctx->inst[n].ptr+ xctx->sym);

  if(layer == cadlayers) goto draw_texts;
  if( (layer != PINLAYER && !xctx->enable_layer[layer]) ) return;

  if(!hide) {
    if(symptr->lines[layer] || symptr->polygons[layer] || symptr->arcs[layer]) {
      set_ps_colors(c);
    }
    for(j=0;j< symptr->lines[layer]; ++j)
    {
     int dash;
     line =  &(symptr->line[layer])[j];
     dash = (disabled == 1) ? 3 : line->dash;
     ROTATION(rot, flip, 0.0,0.0,line->x1,line->y1,x1,y1);
     ROTATION(rot, flip, 0.0,0.0,line->x2,line->y2,x2,y2);
     ORDER(x1,y1,x2,y2);
     ps_drawline(c, x0+x1, y0+y1, x0+x2, y0+y2, dash, line->bus);
    }
    for(j=0;j< symptr->polygons[layer]; ++j)
    {
      int dash;
      int bezier;
      int bus;
      polygon = &(symptr->poly[layer])[j];
      bezier = !strboolcmp(get_tok_value(polygon->prop_ptr, "bezier", 0), "true");
      bus = get_attr_val(get_tok_value(polygon->prop_ptr, "bus", 0));
      dash = (disabled == 1) ? 3 : polygon->dash;
      {   /* scope block so we declare some auxiliary arrays for coord transforms. 20171115 */
        int k;
        double *x = my_malloc(_ALLOC_ID_, sizeof(double) * polygon->points);
        double *y = my_malloc(_ALLOC_ID_, sizeof(double) * polygon->points);
        for(k=0;k<polygon->points; ++k) {
          ROTATION(rot, flip, 0.0,0.0,polygon->x[k],polygon->y[k],x[k],y[k]);
          x[k]+= x0;
          y[k] += y0;
        }
        ps_drawpolygon(c, NOW, x, y, polygon->points, polygon->fill, dash, bezier, bus);
        my_free(_ALLOC_ID_, &x);
        my_free(_ALLOC_ID_, &y);
      }
    }
    if(symptr->arcs[layer]) fprintf(fd, "NP\n"); /* newpath */
    for(j=0;j< symptr->arcs[layer]; ++j)
    {
      int dash;
      double angle;
      arc = &(symptr->arc[layer])[j];
      dash = (disabled == 1) ? 3 : arc->dash;
      if(flip) {
        angle = 270.*rot+180.-arc->b-arc->a;
      } else {
        angle = arc->a+rot*270.;
      }
      angle = fmod(angle, 360.);
      if(angle<0.) angle+=360.;
      ROTATION(rot, flip, 0.0,0.0,arc->x,arc->y,x1,y1);
      ps_drawarc(c, arc->fill, x0+x1, y0+y1, arc->r, angle, arc->b, dash);
    }
  } /* if(!hide) */

  if( (!hide && xctx->enable_layer[layer]) ||
      (hide && layer == PINLAYER && xctx->enable_layer[layer]) ) {
    if(symptr->rects[layer]) {
      fprintf(fd, "NP\n"); /* newpath */
      set_ps_colors(c);
    } 

    for(j=0;j< symptr->rects[layer]; ++j)
    {
       int dash;
       rect = &(symptr->rect[layer])[j];
       dash = (disabled == 1) ? 3 : rect->dash;
       ROTATION(rot, flip, 0.0,0.0,rect->x1,rect->y1,x1,y1);
       ROTATION(rot, flip, 0.0,0.0,rect->x2,rect->y2,x2,y2);
       RECTORDER(x1,y1,x2,y2);
       if (layer == GRIDLAYER && rect->flags & 1024) /* image */
       {
         ps_embedded_image(rect, x0 + x1, y0 + y1, x0 + x2, y0 + y2, rot, flip);
       } else {
         int ellipse_a = rect->ellipse_a;
         int ellipse_b = rect->ellipse_b;
      
         if(ellipse_a != -1 && ellipse_b != 360) {
           if(flip) {
             ellipse_a = 180 - ellipse_a - ellipse_b;
           }
           if(rot) {
             if(rot == 3) {
               ellipse_a += 90;
             } else if(rot == 2) {
               ellipse_a += 180;
             } else if(rot == 1) {
               ellipse_a += 270;
             }
             ellipse_a %= 360;
           }
         }              
         ps_filledrect(c, x0+x1, y0+y1, x0+x2, y0+y2, dash, rect->fill, ellipse_a, ellipse_b);
       }
    }
  } /* if( (!hide && xctx->enable_layer[layer]) || ... */
  
  draw_texts:
  if(xctx->sym_txt && !(xctx->inst[n].flags & HIDE_SYMBOL_TEXTS) && (layer == cadlayers)) {
    const char *txtptr;
    if(c != layer) c_for_text = c;
    else if(xctx->inst[n].flags & PIN_OR_LABEL) c_for_text = TEXTWIRELAYER;
    else c_for_text = TEXTLAYER;
    for(j=0;j< (xctx->inst[n].ptr+ xctx->sym)->texts; ++j)
    {
      double xscale, yscale;
        
      get_sym_text_size(n, j, &xscale, &yscale);
      text = symptr->text[j];
      /* if(xscale*FONTWIDTH* xctx->mooz<1) continue; */
      if(!xctx->show_hidden_texts && (text.flags & (HIDE_TEXT | HIDE_TEXT_INSTANTIATED))) continue;
      if( hide && text.txt_ptr && strcmp(text.txt_ptr, "@symname") && strcmp(text.txt_ptr, "@name") ) continue;
      txtptr= translate(n, text.txt_ptr);
      ROTATION(rot, flip, 0.0,0.0,text.x0,text.y0,x1,y1);
      textlayer = c_for_text;
      /* do not allow custom text color on hilighted instances */
      if(disabled == 1) textlayer = GRIDLAYER;
      else if(disabled == 2) textlayer = PINLAYER;
      else if( xctx->inst[n].color == -10000) {
        int lay;
        get_sym_text_layer(n, j, &lay);
        if(lay != -1) textlayer = lay;
        else textlayer = symptr->text[j].layer;
      }
      if(textlayer < 0 || textlayer >= cadlayers) textlayer = c_for_text;
      if(textlayer != c_for_text) set_ps_colors(textlayer);

       /* display PINLAYER colored instance texts even if PINLAYER disabled */
      if(xctx->inst[n].color == -PINLAYER || xctx->enable_layer[textlayer]) {
        my_snprintf(ps_font_family, S(ps_font_name), "Helvetica");
        my_snprintf(ps_font_name, S(ps_font_name), "Helvetica");
        textfont = symptr->text[j].font;
        if( (textfont && textfont[0])) {
          my_snprintf(ps_font_family, S(ps_font_family), textfont);
          my_snprintf(ps_font_name, S(ps_font_name), textfont);
        }
        if( symptr->text[j].flags & TEXT_BOLD) { 
          if( (symptr->text[j].flags & TEXT_ITALIC) || (symptr->text[j].flags & TEXT_OBLIQUE) ) {
            my_snprintf(ps_font_family, S(ps_font_family), "%s-BoldOblique", ps_font_name);
          } else {
            my_snprintf(ps_font_family, S(ps_font_family), "%s-Bold", ps_font_name);
          }
        }
        else if( symptr->text[j].flags & TEXT_ITALIC)
          my_snprintf(ps_font_family, S(ps_font_family), "%s-Oblique", ps_font_name);
        else if( symptr->text[j].flags & TEXT_OBLIQUE)
          my_snprintf(ps_font_family, S(ps_font_family), "%s-Oblique", ps_font_name);
        if(text_ps) {
          ps_draw_string(textlayer, txtptr,
            (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
            flip^text.flip, text.hcenter, text.vcenter,
            x0+x1, y0+y1, xscale, yscale);
        } else {
          old_ps_draw_string(textlayer, txtptr,
            (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
            flip^text.flip, text.hcenter, text.vcenter,
            x0+x1, y0+y1, xscale, yscale);
        }
      }
      if(textlayer != c) set_ps_colors(c);
    }
  }
}


static void fill_ps_colors()
{
 char s[200]; /* overflow safe 20161122 */
 unsigned int i,c;
 /* if(debug_var>=1) {
  *   tcleval( "puts $ps_colors");
  * }
  */
 for(i=0;i<cadlayers; ++i) {
   my_snprintf(s, S(s), "lindex $ps_colors %u", i);
   tcleval( s);
   sscanf(tclresult(),"%x", &c);
   ps_colors[i].red   = (c & 0xff0000) >> 16;
   ps_colors[i].green = (c & 0x00ff00) >> 8;
   ps_colors[i].blue  = (c & 0x0000ff);
 }

}
/* fullzoom:
 *   0: Print area displayed in window
 *   1: Do a full zoom before generating ps/pdf
 *   2: set paper size to bounding box instead of a4/letter
 */
void create_ps(char **psfile, int what, int fullzoom, int eps)
{
  double dx, dy, scale, scaley;
  int landscape=1;
  static int numpages = 0;
  double margin=10; /* in postscript points, (1/72)". No need to add margin as xschem zoom full already has margins.*/
  char papername[80] = "a4";
  double pagex = 842;
  double pagey = 595;
  xRect boundbox;
  int c,i, textlayer;
  int old_grid;
  const char *textfont;
  static Zoom_info zi;
  Hilight_hashentry *entry;

  dbg(1, "create_ps(): what = %d, fullzoom=%d\n", what, fullzoom);
  if(tcleval("info exists ps_paper_size")[0] == '1') {
    double tmp;
    my_strncpy(papername, tcleval("lindex $ps_paper_size 0"), S(papername));
    pagex = my_atod(tcleval("lindex $ps_paper_size 1"));
    pagey = my_atod(tcleval("lindex $ps_paper_size 2"));
    if(pagex < pagey) { /* start with landscape; later we decide paper orientation */
      tmp = pagex;
      pagex = pagey;
      pagey = tmp;
    }
  }
  if(what & 1) { /* prolog */
    numpages = 0;
    if(!(fd = open_tmpfile("psplot_", ".ps", psfile)) ) {
      fprintf(errfp, "ps_draw(): can not create tmpfile %s\n", *psfile);
      return;
    }
    /* setbuf(fd, NULL); */ /* To prevent buffer errors, still investigating cause. */
  }
  ps_colors=my_calloc(_ALLOC_ID_, cadlayers, sizeof(Ps_color));
  if(ps_colors==NULL){
    fprintf(errfp, "create_ps(): calloc error\n");
    return;
  }

  fill_ps_colors();
  old_grid=tclgetboolvar("draw_grid");
  tclsetvar("draw_grid", "0");

  /* xschem window aspect ratio decides if portrait or landscape */
  boundbox.x1 = xctx->areax1;
  boundbox.x2 = xctx->areax2;
  boundbox.y1 = xctx->areay1;
  boundbox.y2 = xctx->areay2;
  dx=boundbox.x2-boundbox.x1;
  dy=boundbox.y2-boundbox.y1;

  /* xschem drawing bbox decides if portrait or landscape */
  if(fullzoom == 1) {
    calc_drawing_bbox(&boundbox, 0);
    dx=boundbox.x2-boundbox.x1;
    dy=boundbox.y2-boundbox.y1;
  }
  if(dx >= dy) {
    landscape = 1;
  } else {
    landscape = 0;
  }
  dbg(1, "dx=%g, dy=%g\n", dx, dy);


  if(fullzoom == 1) {
    /* save size and zoom factor */
    save_restore_zoom(1, &zi);
    /* this zoom only done to reset lw */
    zoom_full(0, 0, 1 + 2 * tclgetboolvar("zoom_full_center"), 0.97);
    /* adjust aspect ratio to paper size */
    if(landscape) 
      xctx->xrect[0].height = (short unsigned int) (xctx->xrect[0].width * pagey / pagex);
    else
      xctx->xrect[0].width = (short unsigned int) (xctx->xrect[0].height * pagey / pagex);
    dbg(1, "create_ps(): save zoom, r.width=%d, r.height=%d\n", xctx->xrect[0].width, xctx->xrect[0].height);
    xctx->areax1 = -2*INT_LINE_W(xctx->lw);
    xctx->areay1 = -2*INT_LINE_W(xctx->lw);
    xctx->areax2 = xctx->xrect[0].width+2*INT_LINE_W(xctx->lw);
    xctx->areay2 = xctx->xrect[0].height+2*INT_LINE_W(xctx->lw);
    xctx->areaw = xctx->areax2-xctx->areax1;
    xctx->areah = xctx->areay2 - xctx->areay1;
    dbg(1, "create_ps(): areax1=%d areay1=%d areax2=%d areay2=%d\n",
       xctx->areax1, xctx->areay1, xctx->areax2, xctx->areay2);
    dbg(1, "create_ps(): dx=%g, dy=%g\n", dx, dy);
    /* fit schematic into adjusted size */
    zoom_full(0, 0, 0 + 2 * tclgetboolvar("zoom_full_center"), 0.97);
    boundbox.x1 = xctx->areax1;
    boundbox.x2 = xctx->areax2;
    boundbox.y1 = xctx->areay1;
    boundbox.y2 = xctx->areay2;
    dx=boundbox.x2-boundbox.x1;
    dy=boundbox.y2-boundbox.y1;
  }

  if(!landscape) { /* decide paper orientation for best schematic fit */
    double tmp;
    tmp = pagex;
    pagex = pagey;
    pagey = tmp;
  }
  if(fullzoom == 2) { /* set media size to bbox */
    double sc;
    my_strncpy(papername, "bbox", S(papername));
    pagex = xctx->xrect[0].width;
    pagey = xctx->xrect[0].height;
    if(pagex > pagey) {
      sc = 842. / pagex;
      pagex = my_round(pagex * sc);
      pagey = my_round(pagey * sc);
    } else {
      sc = 842. / pagey;
      pagex = my_round(pagex * sc);
      pagey = my_round(pagey * sc);
    }
    margin = 0.0;
  }
    
  if(what & 1) {/* prolog */
    dbg(1, "ps_draw(): bbox: x1=%g y1=%g x2=%g y2=%g\n", boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
    if(!eps) {
      fprintf(fd, "%%!PS-Adobe-3.0\n");
    } else {
      fprintf(fd, "%%!PS-Adobe-2.0 EPSF-2.0\n");
      fprintf(fd, "%%%%BoundingBox: 0 0 %g %g\n",  pagex, pagey);
    }
    /* fprintf(fd, "%%%%DocumentMedia: %s %g %g 80 () ()\n", landscape ? "a4land" : "a4", pagex, pagey); */
    fprintf(fd, "%%%%DocumentMedia: %s %g %g 80 () ()\n", papername, pagex, pagey);
    fprintf(fd, "%%%%PageOrientation: %s\n", landscape ? "Landscape" : "Portrait");
    fprintf(fd, "%%%%Title: xschem plot\n");
    fprintf(fd, "%%%%Creator: xschem\n");
    if(!eps) fprintf(fd, "%%%%Pages: (atend)\n");
    fprintf(fd, "%%%%EndComments\n");

    if(eps) {
      fprintf(fd, "%%%%BeginProlog\n");
      fprintf(fd, "save\n");
      fprintf(fd, "countdictstack\n");
      fprintf(fd, "mark\n");
      fprintf(fd, "newpath\n");
      fprintf(fd, "/showpage {} def\n");
      fprintf(fd, "/setpagedevice {pop} def\n");
      fprintf(fd, "%%%%EndProlog\n");
      fprintf(fd, "%%%%Page 1 1\n");
    }
    fprintf(fd, "%%%%BeginProlog\n\n");
  
    for(i = 0; i < sizeof(utf8_enc)/sizeof(char *); ++i) {
      fprintf(fd, "%s", utf8_enc[i]);
    }
    for(i = 0; i < sizeof(utf8)/sizeof(char *); ++i) {
      fprintf(fd, "%s", utf8[i]);
    }
  
    fprintf(fd, "/Times /Times chararr recode\n");
    fprintf(fd, "/Times-Bold /Times-Bold chararr recode\n");
    fprintf(fd, "/Times-Oblique /Times-Oblique chararr recode\n");
    fprintf(fd, "/Times-BoldOblique /Times-BoldOblique chararr recode\n");
    fprintf(fd, "/Helvetica /Helvetica chararr recode\n");
    fprintf(fd, "/Helvetica-Bold /Helvetica-Bold chararr recode\n");
    fprintf(fd, "/Helvetica-Oblique /Helvetica-Oblique chararr recode\n");
    fprintf(fd, "/Helvetica-BoldOblique /Helvetica-BoldOblique chararr recode\n");
    fprintf(fd, "/Courier /Courier chararr recode\n");
    fprintf(fd, "/Courier-Bold /Courier-Bold chararr recode\n");
    fprintf(fd, "/Courier-Oblique /Courier-Oblique chararr recode\n");
    fprintf(fd, "/Courier-BoldOblique /Courier-BoldOblique chararr recode\n");
  
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
    fprintf(fd,"/E {\n"); /* function for drawing ellipses */
    fprintf(fd,"/endangle exch def\n");
    fprintf(fd,"/startangle exch def\n");
    fprintf(fd,"/yrad exch def\n");
    fprintf(fd,"/xrad exch def\n");
    fprintf(fd,"/y exch def\n");
    fprintf(fd,"/x exch def\n");
    fprintf(fd,"/savematrix matrix currentmatrix def\n");
    fprintf(fd,"x y translate\n");
    fprintf(fd,"xrad yrad scale\n");
    fprintf(fd,"0 0 1 startangle endangle arcn\n");
    fprintf(fd,"savematrix setmatrix\n");
    fprintf(fd,"} def %% ellipse\n");
    fprintf(fd, "%%%%EndProlog\n");
  }


  if(what & 2) { /* page */
    ++numpages;

    if(!eps) {
      fprintf(fd, "%%%%BeginSetup\n");
      fprintf(fd, "<< /PageSize [%g %g] /Orientation 0 >> setpagedevice\n", pagex, pagey);
      fprintf(fd, "%%%%EndSetup\n");
      fprintf(fd, "%%%%Page: %d %d\n\n", numpages, numpages);
      fprintf(fd, "%%%%BeginPageSetup\n");
      fprintf(fd, "%%%%EndPageSetup\n");
    }
    /* add small page title */
    if(tclgetboolvar("ps_page_title") && fullzoom != 2)
       fprintf(fd, "/Helvetica FF 10 SCF SF NP 20 %g MT (%s) show\n", pagey - 20, xctx->current_name);

    /* Add anchor for pdfmarks */
    fprintf(fd,
      "[ "
      "/Dest /%s "
      "/DEST pdfmark\n", get_cell_w_ext(sanitize(xctx->current_name), 0));
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

    set_lw(xctx->lw);
    ps_drawgrid();

    for(c=0;c<cadlayers; ++c)
    {
      if(xctx->lines[c] || xctx->rects[c] || xctx->arcs[c] || xctx->polygons[c]) {
        set_ps_colors(c);
      }
      for(i=0;i<xctx->lines[c]; ++i)
        ps_drawline(c, xctx->line[c][i].x1, xctx->line[c][i].y1,
          xctx->line[c][i].x2, xctx->line[c][i].y2, xctx->line[c][i].dash, xctx->line[c][i].bus);
      if(xctx->rects[c]) fprintf(fd, "NP\n"); /* newpath */
      for(i=0;i<xctx->rects[c]; ++i)
      {
        
        if (c == GRIDLAYER && (xctx->rect[c][i].flags & 1024)) { /* image */
          xRect* r = &xctx->rect[c][i];
          /* PNG Code Here */
          ps_embedded_image(r, r->x1, r->y1, r->x2, r->y2,0 ,0);
          continue;
        }
        if (c == GRIDLAYER && (xctx->rect[c][i].flags & 1)) { /* graph */
          xRect *r = &xctx->rect[c][i];
          ps_embedded_graph(i, r->x1, r->y1, r->x2, r->y2);
        }
        if(c != GRIDLAYER || !(xctx->rect[c][i].flags & 1) )  {
          ps_filledrect(c, xctx->rect[c][i].x1, xctx->rect[c][i].y1,
            xctx->rect[c][i].x2, xctx->rect[c][i].y2,
            xctx->rect[c][i].dash, xctx->rect[c][i].fill,
            xctx->rect[c][i].ellipse_a, xctx->rect[c][i].ellipse_b);
        }
      }
      if(xctx->arcs[c]) fprintf(fd, "NP\n"); /* newpath */
      for(i=0;i<xctx->arcs[c]; ++i)
      {
        ps_drawarc(c, xctx->arc[c][i].fill, xctx->arc[c][i].x, xctx->arc[c][i].y, 
          xctx->arc[c][i].r, xctx->arc[c][i].a, xctx->arc[c][i].b, xctx->arc[c][i].dash);
      }
      for(i=0;i<xctx->polygons[c]; ++i) {
        int bezier = !strboolcmp(get_tok_value(xctx->poly[c][i].prop_ptr, "bezier", 0), "true");
        int bus = get_attr_val(get_tok_value(xctx->poly[c][i].prop_ptr, "bus", 0));
        ps_drawpolygon(c, NOW, xctx->poly[c][i].x, xctx->poly[c][i].y, xctx->poly[c][i].points,
          xctx->poly[c][i].fill, xctx->poly[c][i].dash, bezier, bus);
      }
      dbg(1, "create_ps(): starting drawing symbols on layer %d\n", c);
    } /* for(c=0;c<cadlayers; ++c) */

    /* bring outside previous for(c=0...) loop since ps_embedded_graph() calls ps_draw_symbol() */
    for(c=0;c<cadlayers; ++c) {
      for(i=0;i<xctx->instances; ++i) {
        ps_draw_symbol(c, i, c, what, 0,0, 0.0, 0.0);
        if(c == cadlayers - 1) {
          ps_draw_symbol(c + 1 , i, c + 1, what, 0, 0, 0.0, 0.0); /* ... draw texts */
        }
      }
    }
    prepare_netlist_structs(0); /* NEEDED: data was cleared by trim_wires() */
    for(i=0;i<xctx->wires; ++i)
    {
      int color = WIRELAYER;
      if(xctx->hilight_nets && (entry=bus_hilight_hash_lookup( xctx->wire[i].node, 0, XLOOKUP))) {
        color = get_color(entry->value);
      }
      set_ps_colors(color);
      ps_drawline(color, xctx->wire[i].x1,xctx->wire[i].y1,xctx->wire[i].x2,xctx->wire[i].y2,
                  0 ,xctx->wire[i].bus);
    }
  
    {
      double x1, y1, x2, y2;
      Wireentry *wireptr;
      int i;
      int first = 1;
      Iterator_ctx ctx;
      update_conn_cues(WIRELAYER, 0, 0);
      /* draw connecting dots */
      x1 = X_TO_XSCHEM(xctx->areax1);
      y1 = Y_TO_XSCHEM(xctx->areay1);
      x2 = X_TO_XSCHEM(xctx->areax2);
      y2 = Y_TO_XSCHEM(xctx->areay2);
      for(init_wire_iterator(&ctx, x1, y1, x2, y2); ( wireptr = wire_iterator_next(&ctx) ) ;) {
        if(first) {
          fprintf(fd, "NP\n"); /* newpath */
          first = 0;
        }
        i = wireptr->n;
        if( xctx->wire[i].end1 >1 ) {
          ps_drawarc(WIRELAYER, 1, xctx->wire[i].x1, xctx->wire[i].y1, xctx->cadhalfdotsize, 0, 360, 0);
        }
        if( xctx->wire[i].end2 >1 ) {
          ps_drawarc(WIRELAYER, 1, xctx->wire[i].x2, xctx->wire[i].y2, xctx->cadhalfdotsize, 0, 360, 0);
        }
      }
    }

    for(i=0;i<xctx->texts; ++i)
    {
      textlayer = xctx->text[i].layer;
      if(!xctx->show_hidden_texts && (xctx->text[i].flags & HIDE_TEXT)) continue;
      if(textlayer < 0 ||  textlayer >= cadlayers) textlayer = TEXTLAYER;
  
      my_snprintf(ps_font_family, S(ps_font_name), "Helvetica");
      my_snprintf(ps_font_name, S(ps_font_name), "Helvetica");
      textfont = xctx->text[i].font;
      if( (textfont && textfont[0])) {
        my_snprintf(ps_font_family, S(ps_font_family), textfont);
        my_snprintf(ps_font_name, S(ps_font_name), textfont);
      }
      if( xctx->text[i].flags & TEXT_BOLD) { 
        if( (xctx->text[i].flags & TEXT_ITALIC) || (xctx->text[i].flags & TEXT_OBLIQUE) ) {
          my_snprintf(ps_font_family, S(ps_font_family), "%s-BoldOblique", ps_font_name);
        } else {
          my_snprintf(ps_font_family, S(ps_font_family), "%s-Bold", ps_font_name);
        }
      }
      else if( xctx->text[i].flags & TEXT_ITALIC)
        my_snprintf(ps_font_family, S(ps_font_family), "%s-Oblique", ps_font_name);
      else if( xctx->text[i].flags & TEXT_OBLIQUE)
        my_snprintf(ps_font_family, S(ps_font_family), "%s-Oblique", ps_font_name);
  
      if(text_ps) {
        ps_draw_string(textlayer, get_text_floater(i),
          xctx->text[i].rot, xctx->text[i].flip, xctx->text[i].hcenter, xctx->text[i].vcenter,
          xctx->text[i].x0,xctx->text[i].y0,
          xctx->text[i].xscale, xctx->text[i].yscale);
      } else {
        old_ps_draw_string(textlayer, get_text_floater(i),
          xctx->text[i].rot, xctx->text[i].flip, xctx->text[i].hcenter, xctx->text[i].vcenter,
          xctx->text[i].x0,xctx->text[i].y0,
          xctx->text[i].xscale, xctx->text[i].yscale);
      }
    }

    dbg(1, "ps_draw(): INT_LINE_W(lw)=%d plotfile=%s\n",INT_LINE_W(xctx->lw), xctx->plotfile);
    fprintf(fd, "showpage\n\n");
  }
  if(what & 4) { /* trailer */
    fprintf(fd, "%%%%trailer\n");
    fprintf(fd, "%%%%Pages: %d\n", numpages);
    if(eps) {
      fprintf(fd, "cleartomark\n");
      fprintf(fd, "countdictstack\n");
      fprintf(fd, "exch sub { end } repeat\n");
      fprintf(fd, "restore\n");
    }
    fprintf(fd, "%%%%EOF\n");
    fclose(fd);
  }
  tclsetboolvar("draw_grid", old_grid);
  my_free(_ALLOC_ID_, &ps_colors);


  /* restore original size and zoom factor */
  if(fullzoom == 1) {
    save_restore_zoom(0, &zi);
    resetwin(1, 1, 1, 0, 0);
    change_linewidth(xctx->lw);
    zoom_full(1, 0, 0 + 2 * tclgetboolvar("zoom_full_center"), 0.97);
  }

}

int ps_draw(int what, int fullzoom, int eps)
{
 char tmp[2*PATH_MAX+40];
 static char lastdir[PATH_MAX] = "";
 const char *r;
 static char *psfile;

 if(what & 1) { /* prolog */
   if(!lastdir[0]) my_strncpy(lastdir, pwd_dir, S(lastdir));
   if(has_x && !xctx->plotfile[0]) {
     /* tclvareval("tk_getSaveFile -title {Select destination file} -initialfile {",
      *   get_cell(xctx->sch[xctx->currsch], 0) , ".pdf} -initialdir {", lastdir, "}", NULL); */
     tclvareval("save_file_dialog {Select destination file} *.{ps,pdf} INITIALLOADDIR {", pwd_dir, "/",
       get_cell(xctx->sch[xctx->currsch], 0), eps ? ".eps}": ".pdf}", NULL);
     r = tclresult();
     if(r[0]) {
       my_strncpy(xctx->plotfile, r, S(xctx->plotfile));
       tclvareval("file dirname {", xctx->plotfile, "}", NULL);
       my_strncpy(lastdir, tclresult(), S(lastdir));
     }
     else return 0;
   }
 }
 create_ps(&psfile, what, fullzoom, eps);
 if(what & 4) { /* trailer */
   if(xctx->plotfile[0]) {
     my_snprintf(tmp, S(tmp), "convert_to_pdf {%s} {%s}", psfile, xctx->plotfile);
   } else {
     my_snprintf(tmp, S(tmp), "convert_to_pdf {%s} plot.pdf", psfile);
   }
   my_strncpy(xctx->plotfile,"", S(xctx->plotfile));
   tcleval( tmp);
   Tcl_SetResult(interp,"",TCL_STATIC);
 }
 return 1;
}

