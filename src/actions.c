/* File: actions.c
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
#ifdef __unix__
#include <sys/wait.h>  /* waitpid */
#endif

void here(void)
{
  fprintf(stderr, "here\n");
}

void set_modify(int mod)
{
  static int prev = -1;
  modified = mod;
  if(mod != prev) {
    prev = mod;
    if(has_x && strcmp(get_cell(xctx->sch[xctx->currsch],1), "systemlib/font")) {
      if(mod == 1) {
        tcleval( "wm title . \"xschem - [file tail [xschem get schname]]*\"");
        tcleval( "wm iconname . \"xschem - [file tail [xschem get schname]]*\"");
      } else {
        tcleval( "wm title . \"xschem - [file tail [xschem get schname]]\"");
        tcleval( "wm iconname . \"xschem - [file tail [xschem get schname]]\"");
      }
    }
  }
}

void print_version()
{
  printf("XSCHEM V%s\n", XSCHEM_VERSION);
  printf("Copyright 1998-2020 Stefan Schippers\n");
  printf("\n");
  printf("This is free software; see the source for copying conditions.  There is NO\n");
  printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
  exit(EXIT_SUCCESS);
}


char *escape_chars(char *dest, const char *source, int size)
{
  int s=0;
  int d=0;
  size--; /* reserve space for \0 */
  while(source && source[s]) {
    switch(source[s]) {
      case '\n':
        if(d < size-1) {
          dest[d++] = '\\';
          dest[d++] = 'n';
        }
        break;
      case '\t':
        if(d < size-1) {
          dest[d++] = '\\';
          dest[d++] = 't';
        }
        break;
      case '\\':
      case '\'':
      case ' ':
      case ';':
      case '$':
      case '!':
      case '#':
      case '{':
      case '}':
      case '[':
      case ']':
      case '"':
        if(d < size-1) {
           dest[d++] = '\\';
           dest[d++] = source[s];
        }
        break;
      default:
        if(d < size) dest[d++] = source[s];
    }
    s++;
  }
  dest[d] = '\0';
  return dest;
}

void set_snap(double newsnap) /*  20161212 set new snap factor and just notify new value */
{
    char str[256];
    static double default_snap = -1.0;

    if(default_snap == -1.0) {
      default_snap = atof(tclgetvar("snap"));
      if(default_snap==0.0) default_snap = CADSNAP;
    }
    cadsnap = newsnap ? newsnap : default_snap;
    sprintf(str, "%.16g", cadsnap);
    if(cadsnap == default_snap) {
      tcleval(".statusbar.3 configure -background PaleGreen");
    } else {
      tcleval(".statusbar.3 configure -background OrangeRed");
    }
    tclsetvar("snap", str);
}

void set_grid(double newgrid)
{
    char str[256];
    static double default_grid = -1.0;

    if(default_grid == -1.0) {
      default_grid = atof(tclgetvar("grid"));
      if(default_grid==0.0) default_grid = CADGRID;
    }
    cadgrid = newgrid ? newgrid : default_grid;
    sprintf(str, "%.16g", cadgrid);
    dbg(1, "set_grid(): default_grid = %.16g, cadgrid=%.16g\n", default_grid, cadgrid);
    if(cadgrid == default_grid) {
      tcleval(".statusbar.5 configure -background PaleGreen");
    } else {
      tcleval(".statusbar.5 configure -background OrangeRed");
    }
    tclsetvar("grid", str);
}

int set_netlist_dir(int force, char *dir)
{
  char cmd[PATH_MAX+200];
  if(dir) my_snprintf(cmd, S(cmd), "select_netlist_dir %d %s", force, dir);
  else    my_snprintf(cmd, S(cmd), "select_netlist_dir %d", force);
  tcleval(cmd);
  if(!strcmp("", tclresult()) ) {
    return 0;
  }
  return 1;
}

/* wrapper to TCL function */
const char *abs_sym_path(const char *s, const char *ext)
{
  char c[PATH_MAX+1000];
  my_snprintf(c, S(c), "abs_sym_path {%s} {%s}", s, ext);
  tcleval(c);
  return tclresult();
}

/* Wrapper to Tcl function */
const char *rel_sym_path(const char *s)
{
  char c[PATH_MAX+1000];
  my_snprintf(c, S(c), "rel_sym_path {%s}", s);
  tcleval(c);
  return tclresult();
}

const char *add_ext(const char *f, const char *ext)
{
  static char ff[PATH_MAX];
  char *p;
  int i;

  dbg(1, "add_ext(): f=%s ext=%s\n", f, ext);
  if((p=strrchr(f,'.'))) {
    my_strncpy(ff, f, (p-f) + 1);
    p = ff + (p-f);
    dbg(1, "add_ext() 1: ff=%s\n", ff);
  } else {
    i = my_strncpy(ff, f, S(ff));
    p = ff+i;
    dbg(1, "add_ext() 2: ff=%s\n", ff);
  }
  my_strncpy(p, ext, S(ff)-(p-ff));
  dbg(1, "add_ext() 3: ff=%s\n", ff);
  return ff;
}

void resetwin(int create_pixmap, int clear_pixmap)
{
  int i;
  XWindowAttributes wattr;
  if(has_x) {
#ifdef __unix__
    i = XGetWindowAttributes(display, window, &wattr); /*  should call only when resized */
                                              /*  to avoid server roundtrip replies */
    if(!i) {
      return;
    }
    /* if(wattr.map_state==IsUnmapped) return; */

    xschem_w=wattr.width;
    xschem_h=wattr.height;
    areax2 = xschem_w+2*INT_WIDTH(xctx->lw);
    areay2 = xschem_h+2*INT_WIDTH(xctx->lw);
    areax1 = -2*INT_WIDTH(xctx->lw);
    areay1 = -2*INT_WIDTH(xctx->lw);
    areaw = areax2-areax1;
    areah = areay2-areay1;

    if( xschem_w !=xrect[0].width || xschem_h !=xrect[0].height) { /* 20171123 avoid unnecessary work if no resize */
      dbg(1, "resetwin(): x=%d y=%d   xschem_w=%d xschem_h=%d\n",
                       wattr.x, wattr.y, xschem_w,xschem_h);
      dbg(1, "resetwin(): changing size\n\n");
      xrect[0].x = 0;
      xrect[0].y = 0;
      xrect[0].width = xschem_w;
      xrect[0].height = xschem_h;

      if(clear_pixmap) XFreePixmap(display,save_pixmap);

      /*
      {
        unsigned int w, h;
        XQueryBestSize(display, TileShape, window,  xschem_w, xschem_h, &w, &h);
        dbg(1, "XQueryBestSize: req: w=%d, h=%d, opt: w=%d h=%d\n",
                         xschem_w, xschem_h, w, h);
      }
      */

      if(create_pixmap) {
        save_pixmap = XCreatePixmap(display, window, xschem_w, xschem_h, depth);
      }
      XSetTile(display,gctiled, save_pixmap);
#else
    Tk_Window mainwindow = Tk_MainWindow(interp);
    HWND hwnd = Tk_GetHWND(Tk_WindowId(mainwindow));
    RECT rct;
    if (GetWindowRect(hwnd, &rct))
    {
      unsigned int width = rct.right - rct.left;
      unsigned int height = rct.bottom - rct.top;
      xschem_w = width;
      xschem_h = height;
      areax2 = xschem_w + 2 * INT_WIDTH(xctx->lw);
      areay2 = xschem_h + 2 * INT_WIDTH(xctx->lw);
      areax1 = -2 * INT_WIDTH(xctx->lw);
      areay1 = -2 * INT_WIDTH(xctx->lw);
      areaw = areax2 - areax1;
      areah = areay2 - areay1;
      if (xschem_w != xrect[0].width || xschem_h != xrect[0].height) { /* avoid unnecessary work if no resize */
        dbg(1, "resetwin(): x=%d y=%d   xschem_w=%d xschem_h=%d\n",
          rct.right, rct.bottom, xschem_w, xschem_h);
        dbg(1, "resetwin(): changing size\n\n");
        xrect[0].x = 0;
        xrect[0].y = 0;
        xrect[0].width = xschem_w;
        xrect[0].height = xschem_h;
        if(clear_pixmap) Tk_FreePixmap(display, save_pixmap);
        if(create_pixmap) {
          save_pixmap = Tk_GetPixmap(display, window, xschem_w, xschem_h, depth);
        }
        XSetTile(display, gctiled, save_pixmap);
      }
#endif
      #ifdef HAS_CAIRO
      cairo_destroy(cairo_save_ctx);
      cairo_surface_destroy(save_sfc);

      #if HAS_XRENDER==1
      #if HAS_XCB==1
      save_sfc = cairo_xcb_surface_create_with_xrender_format(xcbconn, screen_xcb, save_pixmap,
           &format_rgb, xschem_w, xschem_h);
      #else
      save_sfc = cairo_xlib_surface_create_with_xrender_format(display, save_pixmap,
           DefaultScreenOfDisplay(display), format, xschem_w, xschem_h);
      #endif /* HAS_XCB */
      #else
      save_sfc = cairo_xlib_surface_create(display, save_pixmap, visual, xschem_w, xschem_h);
      #endif /* HAS_XRENDER */
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
      /* 20171125 select xlib or xcb :-) */
      #if HAS_XCB==1 && HAS_XRENDER==1
      cairo_xcb_surface_set_size(sfc, xschem_w, xschem_h);
      #else
      cairo_xlib_surface_set_size(sfc, xschem_w, xschem_h);
      #endif /* HAS_XCB */
      #endif /* HAS_CAIRO */

    }

    if(pending_fullzoom) {
      zoom_full(0, 0);
      pending_fullzoom=0;
    }
    /* debug ... */
    dbg(1, "resetwin(): Window reset\n");
  }
}

void toggle_only_probes()
{
   static double save_lw;
   if(!only_probes) {
     save_lw = xctx->lw;
     xctx->lw=3.0;
   } else {
     xctx->lw= save_lw;
   }
   only_probes =!only_probes;
   if(only_probes) {
       tclsetvar("only_probes","1");
   }
   else {
       tclsetvar("only_probes","0");
   }
   change_linewidth(xctx->lw);
   draw();
}

void toggle_fullscreen()
{
    char fullscr[]="add,fullscreen";
    char normal[]="remove,fullscreen";
    static int menu_removed = 0;
    fullscreen = (fullscreen+1)%2;
    if(fullscreen==1) tclsetvar("fullscreen","1");
    else if(fullscreen==2) tclsetvar("fullscreen","2");
    else tclsetvar("fullscreen","0");

    dbg(1, "toggle_fullscreen(): fullscreen=%d\n", fullscreen);
    if(fullscreen==2) {
      tcleval("pack forget .menubar .statusbar; update");
      menu_removed = 1;
    }
    if(fullscreen !=2 && menu_removed) {
      tcleval("pack .menubar -anchor n -side top -fill x  -before .drw\n\
               pack .statusbar -after .drw -anchor sw  -fill x; update");
      menu_removed=0;
    }


    if(fullscreen == 1) {
      window_state(display , parent_of_topwindow,fullscr);
    } else if(fullscreen == 2) {
      window_state(display , parent_of_topwindow,normal);
      window_state(display , parent_of_topwindow,fullscr);
    } else {
      window_state(display , parent_of_topwindow,normal);
    }
    pending_fullzoom=1;
}

#ifdef __unix__
void new_window(const char *cell, int symbol)
{

     char f[PATH_MAX]; /*  overflow safe 20161122 */
     struct stat buf;
     pid_t pid1;
     pid_t pid2;
     int status;

     dbg(1, "new_window(): executable: %s, cell=%s, symbol=%d\n", xschem_executable, cell, symbol);
     if(stat(xschem_executable,&buf)) {
       fprintf(errfp, "new_window(): executable not found\n");
       return;
     }

     /* double fork method to avoid zombies 20180925*/
     if ( (pid1 = fork()) ) {
       /* parent process */
       waitpid(pid1, &status, 0);
     } else if (!pid1) {
       /* child process  */
       if ( (pid2 = fork()) ) {
         exit(0); /* --> child of child will be reparented to init */
       } else if (!pid2) {
         /* child of child */
         if(!(freopen("/dev/null","w",stdout) && freopen("/dev/null","r",stdin)&& freopen("/dev/null","w",stderr))){
           fprintf(errfp, "new_window(): freopen error\n");
           tcleval("exit");
         }
         if(!cell || !cell[0]) {
           execl(xschem_executable,xschem_executable,"-r", NULL);
         }
         else if(!symbol) {


           my_strncpy(f, cell, S(f));
           execl(xschem_executable,xschem_executable,"-r",f, NULL);
         }
         else {
           my_strncpy(f, cell, S(f));
           execl(xschem_executable,xschem_executable,"-r",f, NULL);
         }
       } else {
         /* error */
         fprintf(errfp, "new_window(): fork error 1\n");
         tcleval( "exit");
       }
     } else {
       /* error */
       fprintf(errfp, "new_window(): fork error 2\n");
       tcleval( "exit");
     }
}
#else

void new_window(const char* cell, int symbol)
{
  struct stat buf;
  dbg(1, "new_window(): executable: %s, cell=%s, symbol=%d\n", xschem_executable, cell, symbol);
  if (stat(xschem_executable, &buf)) {
    fprintf(errfp, "new_window(): executable not found\n");
    return;
  }
  fprintf(errfp, "new_window(): fork error 1\n");
  tcleval("exit");
}
#endif
const char *get_file_path(char *f)
{
  char tmp[2*PATH_MAX+100];
  my_snprintf(tmp, S(tmp),"get_file_path \"%s\"", f);
  tcleval(tmp);
  return tclresult();
}


int samefile(const char *fa, const char *fb)
{

   struct stat a, b;
   int statusa, statusb;

   statusa = stat(fa, &a);
   statusb = stat(fb, &b);
   if(statusa == 0 && statusb == 0 && a.st_ino == b.st_ino) {
     return 1;
   }
   return 0; /* not same of one of the two not existing */
}

int save(int confirm) /* 20171006 add confirm */
{
     int cancel;
     int save_ok;

     save_ok=0;
     cancel=0;
     if(modified)
     {
       if(confirm) {
         tcleval("ask_save");
         if(!strcmp(tclresult(), "") ) cancel=1;
         if(!strcmp(tclresult(), "yes") ) save_ok = save_schematic(xctx->sch[xctx->currsch]);
       } else {
         save_ok = save_schematic(xctx->sch[xctx->currsch]);
       }
     }
     if(save_ok==-1) return 1;
     return cancel;
}

void saveas(const char *f, int type) /*  changed name from ask_save_file to saveas 20121201 */
{
    char name[PATH_MAX+1000];
    char filename[PATH_MAX];
    char res[PATH_MAX];
    char *p;
    if(!f && has_x) {
      my_strncpy(filename , xctx->sch[xctx->currsch], S(filename));
      if(type == SYMBOL) {
        if( (p = strrchr(filename, '.')) && !strcmp(p, ".sch") ) {
          my_strncpy(filename, add_ext(filename, ".sym"), S(filename));
        }
        my_snprintf(name, S(name), "save_file_dialog {Save file} .sym.sch INITIALLOADDIR {%s}", filename);
      } else {
        my_snprintf(name, S(name), "save_file_dialog {Save file} .sch.sym INITIALLOADDIR {%s}", filename);
      }

      tcleval(name);
      my_strncpy(res, tclresult(), S(res));
    }
    else if(f) {
      my_strncpy(res, f, S(res));
    }
    else res[0]='\0';

    if(!res[0]) return;
    dbg(1, "saveas(): res = %s\n", res);
    save_schematic(res);
    Tcl_VarEval(interp, "update_recent_file {", res,"}",  NULL);

    my_strncpy(xctx->current_name, rel_sym_path(res), S(xctx->current_name));
    return;
}

void ask_new_file(void)
{
    char fullname[PATH_MAX]; /*  overflow safe 20161125 */

    if(!has_x) return;

    if(modified) {
      if(save(1)) return; /*  user cancels save, so do nothing. */
    }
    tcleval("load_file_dialog {Load Schematic} .sch.sym INITIALLOADDIR");
    my_snprintf(fullname, S(fullname),"%s", tclresult());


    if( fullname[0] ) {
     dbg(1, "ask_new_file(): load file: %s\n", fullname);
     delete_hilight_net();
     xctx->currsch = 0;
     unselect_all();
     remove_symbols();
     load_schematic(1, fullname,1); /* 20180925.1 */
     Tcl_VarEval(interp, "update_recent_file {", fullname, "}", NULL);
     my_strdup(1, &xctx->sch_path[xctx->currsch],".");
     xctx->sch_inst_number[xctx->currsch] = 1;
     zoom_full(1, 0);
    }
}

/* remove symbol and decrement xctx->symbols */
/* Warning: removing a symbol with a loaded schematic will make all symbol references corrupt */
/* you should clear_drawing() first or load_schematic() or link_symbols_to_instances()
   immediately afterwards */
void remove_symbol(int j)
{
  int i,c;
  xSymbol save;
  dbg(1, "remove_symbol(): removing symbol %d\n", j);

  my_free(666, &xctx->sym[j].prop_ptr);
  my_free(667, &xctx->sym[j].templ);
  my_free(668, &xctx->sym[j].type);
  my_free(684, &xctx->sym[j].name);
  /*  /20150409 */
  for(c=0;c<cadlayers;c++)
  {
   for(i=0;i<xctx->sym[j].polygons[c];i++)
   {
     if(xctx->sym[j].poly[c][i].prop_ptr != NULL) {
       my_free(669, &xctx->sym[j].poly[c][i].prop_ptr);
     }
     my_free(670, &xctx->sym[j].poly[c][i].x);
     my_free(671, &xctx->sym[j].poly[c][i].y);
     my_free(672, &xctx->sym[j].poly[c][i].selected_point);
   }
   my_free(673, &xctx->sym[j].poly[c]);

   for(i=0;i<xctx->sym[j].lines[c];i++)
   {
    if(xctx->sym[j].line[c][i].prop_ptr != NULL)
    {
     my_free(674, &xctx->sym[j].line[c][i].prop_ptr);
    }
   }
   my_free(675, &xctx->sym[j].line[c]);

   for(i=0;i<xctx->sym[j].arcs[c];i++)
   {
    if(xctx->sym[j].arc[c][i].prop_ptr != NULL)
    {
     my_free(676, &xctx->sym[j].arc[c][i].prop_ptr);
    }
   }
   my_free(677, &xctx->sym[j].arc[c]);

   for(i=0;i<xctx->sym[j].rects[c];i++)
   {
    if(xctx->sym[j].rect[c][i].prop_ptr != NULL)
    {
     my_free(678, &xctx->sym[j].rect[c][i].prop_ptr);
    }
   }
   my_free(679, &xctx->sym[j].rect[c]);
  }
  for(i=0;i<xctx->sym[j].texts;i++)
  {
   if(xctx->sym[j].text[i].prop_ptr != NULL)
   {
    my_free(680, &xctx->sym[j].text[i].prop_ptr);
   }
   if(xctx->sym[j].text[i].txt_ptr != NULL)
   {
    my_free(681, &xctx->sym[j].text[i].txt_ptr);
   }
   if(xctx->sym[j].text[i].font != NULL)
   {
    my_free(682, &xctx->sym[j].text[i].font);
   }
  }
  my_free(683, &xctx->sym[j].text);

  save = xctx->sym[j];
  for(i = j + 1; i < xctx->symbols; i++) {
    xctx->sym[i-1] = xctx->sym[i];
  }
  xctx->sym[xctx->symbols-1] = save;
  xctx->symbols--;
}

void remove_symbols(void)
{
 int j;

 for(j=xctx->symbols-1;j>=0;j--) {
   dbg(2, "remove_symbols(): removing symbol %d\n",j);
   remove_symbol(j);
 }
  dbg(1, "remove_symbols(): done\n");
  if(event_reporting) {
    printf("xschem remove_symbols\n");
    fflush(stdout);
  }

}

void clear_drawing(void)
{
 int i,j;
 del_inst_table();
 del_wire_table();
 my_free(685, &xctx->schtedaxprop);
 my_free(686, &xctx->schsymbolprop);
 my_free(687, &xctx->schprop);
 my_free(688, &xctx->schvhdlprop);
 my_free(689, &xctx->version_string);
 my_free(690, &xctx->schverilogprop);
 for(i=0;i<xctx->wires;i++)
 {
  my_free(691, &xctx->wire[i].prop_ptr);
  my_free(692, &xctx->wire[i].node);
 }
 xctx->wires = 0;
 for(i=0;i<xctx->instances;i++)
 {
  my_free(693, &xctx->inst[i].prop_ptr);
  my_free(694, &xctx->inst[i].name);
  my_free(695, &xctx->inst[i].instname);
  delete_inst_node(i);
 }
 xctx->instances = 0;
 for(i=0;i<xctx->texts;i++)
 {
  my_free(696, &xctx->text[i].font);
  my_free(697, &xctx->text[i].prop_ptr);
  my_free(698, &xctx->text[i].txt_ptr);
 }
 xctx->texts = 0;
 for(i=0;i<cadlayers;i++)
 {
  for(j=0;j<xctx->lines[i];j++)
  {
   my_free(699, &xctx->line[i][j].prop_ptr);
  }
  for(j=0;j<xctx->rects[i];j++)
  {
   my_free(700, &xctx->rect[i][j].prop_ptr);
  }
  for(j=0;j<xctx->arcs[i];j++)
  {
   my_free(701, &xctx->arc[i][j].prop_ptr);
  }
  for(j=0;j<xctx->polygons[i]; j++) {
    my_free(702, &xctx->poly[i][j].x);
    my_free(703, &xctx->poly[i][j].y);
    my_free(704, &xctx->poly[i][j].prop_ptr);
    my_free(705, &xctx->poly[i][j].selected_point);
  }
  xctx->lines[i] = 0;
  xctx->arcs[i] = 0;
  xctx->rects[i] = 0;
  xctx->polygons[i] = 0;
 }
 dbg(1, "clear drawing(): deleted data structures, now deleting hash\n");
 clear_instance_hash();
}

void enable_layers(void)
{
  int i;
  char tmp[50];
  n_active_layers = 0;
  for(i = 0; i< cadlayers; i++) {
    my_snprintf(tmp, S(tmp), "enable_layer(%d)",i);
    if(tclgetvar(tmp)[0] == '0') enable_layer[i] = 0;
    else {
      enable_layer[i] = 1;
      if(i>=7) {
        active_layer[n_active_layers] = i;
        n_active_layers++;
      }
    }
  }
}

void attach_labels_to_inst() /*  offloaded from callback.c 20171005 */
{
  xSymbol *symbol;
  int npin, i, j;
  double x0,y0, pinx0, piny0;
  int flip, rot, rot1 ;
  xRect *rct;
  char *labname=NULL;
  char *prop=NULL; /*  20161122 overflow safe */
  const char symname_pin[] = "devices/lab_pin.sym";
  const char symname_wire[] = "devices/lab_wire.sym";
  const char symname_pin2[] = "lab_pin.sym";
  const char symname_wire2[] = "lab_wire.sym";
  char *type=NULL;
  int dir;
  int k,ii, skip;
  int do_all_inst=0;
  const char *rot_txt;
  int rotated_text=-1;

  struct wireentry *wptr;
  struct instpinentry *iptr;
  int sqx, sqy;
  int first_call;
  struct stat buf;
  int indirect;
  int use_label_prefix;
  int found=0;

  if(!stat(abs_sym_path(symname_pin, ""), &buf)) {
    indirect=1;
  } else {
    indirect=0;
  }
  /* printf("indirect=%d\n", indirect); */

  rebuild_selected_array();
  k = lastselected;
  first_call=1; /*  20171214 for place_symbol--> new_prop_string */
  prepare_netlist_structs(0);
  for(j=0;j<k;j++) if(selectedgroup[j].type==ELEMENT) {

    found=1;
    my_strdup(5, &prop, xctx->inst[selectedgroup[j].n].instname);
    my_strcat(6, &prop, "_");
    tclsetvar("custom_label_prefix",prop);

    if(!do_all_inst) {
      dbg(1,"attach_labels_to_inst(): invoking tcl attach_labels_to_inst\n");
      tcleval("attach_labels_to_inst");
      if(!strcmp(tclgetvar("rcode"),"") ) {
        bbox(END, 0., 0., 0., 0.);
        my_free(706, &prop);
        return;
      }
    }
    use_label_prefix = atoi(tclgetvar("use_label_prefix"));

    rot_txt = tclgetvar("rotated_text");
    if(strcmp(rot_txt,"")) rotated_text=atoi(rot_txt);

    my_strdup(7, &type,(xctx->inst[selectedgroup[j].n].ptr+ xctx->sym)->type);
    if( type && IS_LABEL_OR_PIN(type) ) {
      continue;
    }

    if(!do_all_inst && !strcmp(tclgetvar("do_all_inst"),"1")) do_all_inst=1;

    dbg(1, " 200711 1--> %s %.16g %.16g   %s\n",
        xctx->inst[selectedgroup[j].n].name,
        xctx->inst[selectedgroup[j].n].x0,
        xctx->inst[selectedgroup[j].n].y0,
        xctx->sym[xctx->inst[selectedgroup[j].n].ptr].name);

    x0 = xctx->inst[selectedgroup[j].n].x0;
    y0 = xctx->inst[selectedgroup[j].n].y0;
    rot = xctx->inst[selectedgroup[j].n].rot;
    flip = xctx->inst[selectedgroup[j].n].flip;
    symbol = xctx->sym + xctx->inst[selectedgroup[j].n].ptr;
    npin = symbol->rects[PINLAYER];
    rct=symbol->rect[PINLAYER];

    for(i=0;i<npin;i++) {
       my_strdup(8, &labname,get_tok_value(rct[i].prop_ptr,"name",1));
       dbg(1,"200711 2 --> labname=%s\n", labname);

       pinx0 = (rct[i].x1+rct[i].x2)/2;
       piny0 = (rct[i].y1+rct[i].y2)/2;

       if(strcmp(get_tok_value(rct[i].prop_ptr,"dir",0),"in")) dir=1; /*  out or inout pin */
       else dir=0; /*  input pin */

       /*  opin or iopin on left of symbol--> reverse orientation 20171205 */
       if(rotated_text ==-1 && dir==1 && pinx0<0) dir=0;

       ROTATION(0.0, 0.0, pinx0, piny0, pinx0, piny0);

       pinx0 += x0;
       piny0 += y0;

       get_square(pinx0, piny0, &sqx, &sqy);
       iptr=instpintable[sqx][sqy];
       wptr=wiretable[sqx][sqy];

       skip=0;
       while(iptr) {
         ii = iptr->n;
         if(ii == selectedgroup[j].n) {
           iptr = iptr->next;
           continue;
         }

         if( iptr->x0 == pinx0 && iptr->y0 == piny0 ) {
           skip=1;
           break;
         }
         iptr = iptr->next;
       }
       while(wptr) {
         if( touch(xctx->wire[wptr->n].x1, xctx->wire[wptr->n].y1,
             xctx->wire[wptr->n].x2, xctx->wire[wptr->n].y2, pinx0, piny0) ) {
           skip=1;
           break;
         }
         wptr = wptr->next;
       }
       if(!skip) {
         my_strdup(9, &prop, "name=p1 lab=");


         if(use_label_prefix) {
           my_strcat(10, &prop, (char *)tclgetvar("custom_label_prefix"));
         }
         /*  /20171005 */

         my_strcat(11, &prop, labname);
         dir ^= flip; /*  20101129  20111030 */
         if(rotated_text ==-1) {
           rot1=rot;
           if(rot1==1 || rot1==2) { dir=!dir;rot1 = (rot1+2) %4;}
         } else {
           rot1=(rot+rotated_text)%4; /*  20111103 20171208 text_rotation */
         }
         if(!strcmp(tclgetvar("use_lab_wire"),"0")) {
           if(indirect)
             place_symbol(-1,symname_pin, pinx0, piny0, rot1, dir, prop, 2, first_call);
           else
             place_symbol(-1,symname_pin2, pinx0, piny0, rot1, dir, prop, 2, first_call);
           first_call=0;
         } else {
           if(indirect)
             place_symbol(-1,symname_wire, pinx0, piny0, rot1, dir, prop, 2, first_call);
           else
             place_symbol(-1,symname_wire2, pinx0, piny0, rot1, dir, prop, 2, first_call);
           first_call=0;
         }
       }
       dbg(1, "%d   %.16g %.16g %s\n", i, pinx0, piny0,labname);
    }
  }
  my_free(707, &prop);
  my_free(708, &labname);
  my_free(709, &type);
  if(!found) return;
  /*  draw things  */
  bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
  draw();
  bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
}


/*  draw_sym==4 select element after placing */
/*  draw_sym==2 begin bbox if(first_call), add bbox */
/*  draw_sym==1 begin bbox if(first_call), add bbox, end bbox, draw placed symbols  */
/*  */
/*  first_call: set to 1 on first invocation for a given set of symbols (same prefix) */
/*  set to 0 on next calls, this speeds up searching for unique names in prop string */
/*  returns 1 if symbol successfully placed, 0 otherwise */
int place_symbol(int pos, const char *symbol_name, double x, double y, int rot, int flip,
                   const char *inst_props, int draw_sym, int first_call)
/*  if symbol_name is a valid string load specified cell and */
/*  use the given params, otherwise query user */
{
 int i,j,n;
 static char name[PATH_MAX];
 char *type;
 int cond;
 if(symbol_name==NULL) {
   tcleval("load_file_dialog {Choose symbol} .sym INITIALINSTDIR");
   my_strncpy(name, tclresult(), S(name));
 } else {
   my_strncpy(name, symbol_name, S(name));
 }
 dbg(1, "place_symbol(): load_file_dialog returns:  name=%s\n",name);
 my_strncpy(name, rel_sym_path(name), S(name));
 if(name[0]) {
   if(first_call) push_undo();
 } else  return 0;
 i=match_symbol(name);

 if(i!=-1)
 {
  check_inst_storage();
  if(pos==-1 || pos > xctx->instances) n=xctx->instances;
  else
  {
   for(j=xctx->instances;j>pos;j--)
   {
    xctx->inst[j]=xctx->inst[j-1];
   }
   n=pos;
  }
  /*  03-02-2000 */
  dbg(1, "place_symbol(): checked inst_ptr storage, sym number i=%d\n", i);
  xctx->inst[n].ptr = i;
  xctx->inst[n].name=NULL;
  xctx->inst[n].instname=NULL;
  dbg(1, "place_symbol(): entering my_strdup: name=%s\n",name);  /*  03-02-2000 */
  my_strdup(12, &xctx->inst[n].name ,name);
  dbg(1, "place_symbol(): done my_strdup: name=%s\n",name);  /*  03-02-2000 */
  /*  xctx->inst[n].x0=symbol_name ? x : mousex_snap; */
  /*  xctx->inst[n].y0=symbol_name ? y : mousey_snap; */
  xctx->inst[n].x0= x ; /*  20070228 x and y given in callback */
  xctx->inst[n].y0= y ;
  xctx->inst[n].rot=symbol_name ? rot : 0;
  xctx->inst[n].flip=symbol_name ? flip : 0;

  xctx->inst[n].flags=0;
  xctx->inst[n].sel=0;
  xctx->inst[n].node=NULL;
  xctx->inst[n].prop_ptr=NULL;
  dbg(1, "place_symbol() :all inst_ptr members set\n");  /*  03-02-2000 */
  if(first_call) hash_all_names(n);
  if(inst_props) {
    new_prop_string(n, inst_props,!first_call, dis_uniq_names); /*  20171214 first_call */
  }
  else {
    set_inst_prop(n); /* no props, get from sym template, also calls new_prop_string() */
  }
  dbg(1, "place_symbol(): done set_inst_prop()\n");  /*  03-02-2000 */

  my_strdup2(13, &xctx->inst[n].instname, get_tok_value(xctx->inst[n].prop_ptr,"name",0) );

  type = xctx->sym[xctx->inst[n].ptr].type;
  cond= !type || !IS_LABEL_SH_OR_PIN(type);
  if(cond) xctx->inst[n].flags|=2;
  else xctx->inst[n].flags &=~2;

  if(first_call && (draw_sym & 3) ) bbox(BEGIN, 0.0 , 0.0 , 0.0 , 0.0);

  xctx->instances++; /* must be updated before calling symbol_bbox() */
  prepared_hash_instances=0;
  /* force these vars to 0 to trigger a prepare_netlist_structs(0) needed by symbol_bbox->translate
   * to translate @#n:net_name texts */
  prepared_netlist_structs=0;
  prepared_hilight_structs=0;
  symbol_bbox(n, &xctx->inst[n].x1, &xctx->inst[n].y1,
                    &xctx->inst[n].x2, &xctx->inst[n].y2);
  if(draw_sym & 3) bbox(ADD, xctx->inst[n].x1, xctx->inst[n].y1, xctx->inst[n].x2, xctx->inst[n].y2);
  set_modify(1);
  if(draw_sym&1) {
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
    draw();
    bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  }
  /*   hilight new element 24122002 */

  if(draw_sym & 4 ) {
    select_element(n, SELECTED,0, 0);
    ui_state |= SELECTION;
    drawtemparc(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
    drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
    drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
    need_rebuild_selected_array = 1;
    rebuild_selected_array();
  }

 }
 return 1;
}

void symbol_in_new_window(void)
{
 rebuild_selected_array();
 if(lastselected !=1 || selectedgroup[0].type!=ELEMENT)
 {
  new_window(xctx->sch[xctx->currsch],1);
 }
 else
 {
  new_window(abs_sym_path(xctx->inst[selectedgroup[0].n].name, ""),1);
 }

}


void schematic_in_new_window(void)
{
 char filename[PATH_MAX];
 rebuild_selected_array();
 if(lastselected !=1 || selectedgroup[0].type!=ELEMENT)
 {
  /*  new_window("", 0); */
  new_window(xctx->sch[xctx->currsch], 0); /*  20111007 duplicate current schematic if no inst selected */
  return;
 }
 else
 {
  if(                   /*  do not descend if not subcircuit */
     (xctx->inst[selectedgroup[0].n].ptr+ xctx->sym)->type &&
     strcmp(
        (xctx->inst[selectedgroup[0].n].ptr+ xctx->sym)->type,
         "subcircuit"
     ) &&
     strcmp(
        (xctx->inst[selectedgroup[0].n].ptr+ xctx->sym)->type,
         "primitive"
     )
  ) return;

  my_strncpy(filename, abs_sym_path(get_tok_value(
    (xctx->inst[selectedgroup[0].n].ptr+ xctx->sym)->prop_ptr, "schematic",0 ), "")
    , S(filename));
  if(!filename[0]) {
    my_strncpy(filename, add_ext(abs_sym_path(xctx->inst[selectedgroup[0].n].name, ""), ".sch"), S(filename));
  }

  new_window(filename, 0);
 }
}

void launcher(void)
{
  const char *str;
  char program[PATH_MAX];
  int n;
  rebuild_selected_array();
  if(lastselected ==1 && selectedgroup[0].type==ELEMENT)
  {
    double mx=mousex, my=mousey;
    select_object(mx,my,SELECTED, 0);
    tcleval("update; after 300");
    select_object(mx,my,0, 0);
    n=selectedgroup[0].n;
    my_strncpy(program, get_tok_value(xctx->inst[n].prop_ptr,"program",2), S(program)); /* handle backslashes */
    str = get_tok_value(xctx->inst[n].prop_ptr,"url",2); /* handle backslashes */
    dbg(1, "launcher(): str=%s\n", str);
    if(str[0] || (program[0])) {
      tclsetvar("launcher_var",str);
      if(program[0]) { /*  20170413 leave launcher_program empty if unspecified */
        tclsetvar("launcher_program",program);
      } else {
        tclsetvar("launcher_program","");
      }
      tcleval( "launcher");
    } else {
      my_strncpy(program, get_tok_value(xctx->inst[n].prop_ptr,"tclcommand",2), S(program));
      if(program[0]) { /*  20170415 execute tcl command */
        tcleval(program);
      }
    }
  }
}

void descend_schematic(int instnumber)
{
 const char *str;
 char filename[PATH_MAX];
 int inst_mult, inst_number;
 int save_ok = 0;

 rebuild_selected_array();
 if(lastselected !=1 || selectedgroup[0].type!=ELEMENT)
 {
  dbg(1, "descend_schematic(): wrong selection\n");
  return;
 }
 else
 {
  dbg(1, "descend_schematic(): selected:%s\n", xctx->inst[selectedgroup[0].n].name);
  /* no name set for current schematic: save it before descending*/
  if(!strcmp(xctx->sch[xctx->currsch],""))
  {
    char cmd[PATH_MAX+1000];
    char filename[PATH_MAX];
    char res[PATH_MAX];

    my_strncpy(filename, xctx->sch[xctx->currsch], S(filename));
    my_snprintf(cmd, S(cmd), "save_file_dialog {Save file} .sch.sym INITIALLOADDIR {%s}", filename);
    tcleval(cmd);
    my_strncpy(res, tclresult(), S(res));
    if(!res[0]) return;
    dbg(1, "descend_schematic(): saving: %s\n",res);
    save_ok = save_schematic(res);
    if(save_ok==-1) return;
  }

  dbg(1, "type of instance: %s\n", (xctx->inst[selectedgroup[0].n].ptr+ xctx->sym)->type);

  if(                   /*  do not descend if not subcircuit */
     (xctx->inst[selectedgroup[0].n].ptr+ xctx->sym)->type &&
     strcmp(
        (xctx->inst[selectedgroup[0].n].ptr+ xctx->sym)->type,
         "subcircuit"
     ) &&
     strcmp(
        (xctx->inst[selectedgroup[0].n].ptr+ xctx->sym)->type,
         "primitive"
     )
  ) return;

  if(modified)
  {
    if(save(1)) return;
  }

  /*  build up current hierarchy path */
  dbg(1, "descend_schematic(): selected instname=%s\n", xctx->inst[selectedgroup[0].n].instname);


  if(xctx->inst[selectedgroup[0].n].instname && xctx->inst[selectedgroup[0].n].instname[0]) {
    str=expandlabel(xctx->inst[selectedgroup[0].n].instname, &inst_mult);
  } else {
    str = "";
    inst_mult = 1;
  }
  my_strdup(14, &xctx->sch_path[xctx->currsch+1], xctx->sch_path[xctx->currsch]);

  inst_number = 1;
  if(inst_mult > 1) { /* on multiple instances ask where to descend, to correctly evaluate
                         the hierarchy path you descend to */

    if(instnumber <= 0 ) {
      const char *inum;
      Tcl_VarEval(interp, "input_line ", "{input instance number (leftmost = 1) to descend into:\n"
        "negative numbers select instance starting\nfrom the right (rightmost = -1)}"
        " {} 1 6", NULL);
      inum = tclresult();
      dbg(1, "descend_schematic(): inum=%s\n", inum);
      if(!inum[0]) {
        my_free(710, &xctx->sch_path[xctx->currsch+1]);
        return;
      }
      inst_number=atoi(inum);
    } else {
      inst_number = instnumber;
    }
    if(inst_number < 0 ) inst_number += inst_mult+1;
    if(inst_number <1 || inst_number > inst_mult) inst_number = 1; /* any invalid number->descend to leftmost inst */
  }
  dbg(1," inst_number=%d\n", inst_number);
  my_strcat(15, &xctx->sch_path[xctx->currsch+1], find_nth(str, ',', inst_number));
  dbg(1," inst_number=%d\n", inst_number);
  my_strcat(16, &xctx->sch_path[xctx->currsch+1], ".");
  xctx->sch_inst_number[xctx->currsch+1] = inst_number;
  dbg(1, "descend_schematic(): current path: %s\n", xctx->sch_path[xctx->currsch+1]);
  dbg(1, "descend_schematic(): inst_number=%d\n", inst_number);

  xctx->previous_instance[xctx->currsch]=selectedgroup[0].n;
  xctx->zoom_array[xctx->currsch].x=xctx->xorigin;
  xctx->zoom_array[xctx->currsch].y=xctx->yorigin;
  xctx->zoom_array[xctx->currsch].zoom=xctx->zoom;
  xctx->currsch++;
  hilight_child_pins();

  my_strncpy(filename, abs_sym_path(get_tok_value(
     (xctx->inst[selectedgroup[0].n].ptr+ xctx->sym)->prop_ptr, "schematic",0 ), "")
     , S(filename));

  unselect_all();
  remove_symbols();
  if(filename[0]) {
    load_schematic(1,filename, 1);
  } else {
    my_strncpy(filename, add_ext(abs_sym_path(xctx->inst[selectedgroup[0].n].name, ""), ".sch"), S(filename));
    load_schematic(1, filename, 1);
  }
  if(hilight_nets)
  {
    prepare_netlist_structs(0);
    if(enable_drill) drill_hilight();
  }
  dbg(1, "descend_schematic(): before zoom(): prepared_hash_instances=%d\n", prepared_hash_instances);
  zoom_full(1, 0);
 }
}

void go_back(int confirm) /*  20171006 add confirm */
{
 int save_ok;
 int from_embedded_sym;
 int save_modified;
 char filename[PATH_MAX];

 save_ok=0;
 if(xctx->currsch>0)
 {
  /* if current sym/schematic is changed ask save before going up */
  if(modified)
  {
    if(confirm) {
      tcleval("ask_save");
      if(!strcmp(tclresult(), "yes") ) save_ok = save_schematic(xctx->sch[xctx->currsch]);
      else if(!strcmp(tclresult(), "") ) return;
    } else {
      save_ok = save_schematic(xctx->sch[xctx->currsch]);
    }
  }
  if(save_ok==-1) return;
  unselect_all();
  remove_symbols();
  from_embedded_sym=0;
  if(strstr(xctx->sch[xctx->currsch], ".xschem_embedded_")) {
    /* when returning after editing an embedded symbol
     * load immediately symbol definition before going back (.xschem_embedded... file will be lost)
     */
    load_sym_def(xctx->sch[xctx->currsch], NULL);
    from_embedded_sym=1;
  }
  my_strncpy(xctx->sch[xctx->currsch] , "", S(xctx->sch[xctx->currsch]));
  xctx->currsch--;
  save_modified = modified; /* we propagate modified flag (cleared by load_schematic */
                            /* by default) to parent schematic if going back from embedded symbol */

  my_strncpy(filename, xctx->sch[xctx->currsch], S(filename));
  load_schematic(1, filename, 1);
  if(from_embedded_sym) modified=save_modified; /* to force ask save embedded sym in parent schematic */

  hilight_parent_pins();
  if(enable_drill) drill_hilight();
  xctx->xorigin=xctx->zoom_array[xctx->currsch].x;
  xctx->yorigin=xctx->zoom_array[xctx->currsch].y;
  xctx->zoom=xctx->zoom_array[xctx->currsch].zoom;
  xctx->mooz=1/xctx->zoom;

  change_linewidth(-1.);
  draw();

  dbg(1, "go_back(): current path: %s\n", xctx->sch_path[xctx->currsch]);
 }
}

void change_linewidth(double w)
{
  int i, changed;

  changed=0;
  /* choose line width automatically based on zoom */
  if(w<0.) {
    if(change_lw)  {
      xctx->lw=xctx->mooz * 0.09 * cadsnap;
      cadhalfdotsize = CADHALFDOTSIZE +  0.04 * (cadsnap-10);
      changed=1;
    }
  /* explicitly set line width */
  } else {
    xctx->lw=w;
    changed=1;
  }
  if(!changed) return;
  if(has_x) {
    for(i=0;i<cadlayers;i++) {
        XSetLineAttributes (display, gc[i], INT_WIDTH(xctx->lw), LineSolid, CapRound , JoinRound);
    }
    XSetLineAttributes (display, gctiled, INT_WIDTH(xctx->lw), LineSolid, CapRound , JoinRound);
  }
  areax1 = -2*INT_WIDTH(xctx->lw);
  areay1 = -2*INT_WIDTH(xctx->lw);
  areax2 = xrect[0].width+2*INT_WIDTH(xctx->lw);
  areay2 = xrect[0].height+2*INT_WIDTH(xctx->lw);
  areaw = areax2-areax1;
  areah = areay2 - areay1;
}

void calc_drawing_bbox(xRect *boundbox, int selected)
{

 xRect tmp;
 int c, i;
 int count=0;
 #ifdef HAS_CAIRO
 int customfont;
 #endif

 boundbox->x1=-100;
 boundbox->x2=100;
 boundbox->y1=-100;
 boundbox->y2=100;
 for(c=0;c<cadlayers;c++)
 {
  for(i=0;i<xctx->lines[c];i++)
  {
   if(selected == 1 && !xctx->line[c][i].sel) continue;
   if(selected == 2) continue;
   tmp.x1=xctx->line[c][i].x1;
   tmp.x2=xctx->line[c][i].x2;
   tmp.y1=xctx->line[c][i].y1;
   tmp.y2=xctx->line[c][i].y2;
   count++;
   updatebbox(count,boundbox,&tmp);
  }

  for(i=0;i<xctx->polygons[c];i++)
  {
    double x1=0., y1=0., x2=0., y2=0.;
    int k;
    if(selected == 1 && !xctx->poly[c][i].sel) continue;
    if(selected == 2) continue;
    count++;
    for(k=0; k<xctx->poly[c][i].points; k++) {
      /* fprintf(errfp, "  poly: point %d: %.16g %.16g\n", k, pp[c][i].x[k], pp[c][i].y[k]); */
      if(k==0 || xctx->poly[c][i].x[k] < x1) x1 = xctx->poly[c][i].x[k];
      if(k==0 || xctx->poly[c][i].y[k] < y1) y1 = xctx->poly[c][i].y[k];
      if(k==0 || xctx->poly[c][i].x[k] > x2) x2 = xctx->poly[c][i].x[k];
      if(k==0 || xctx->poly[c][i].y[k] > y2) y2 = xctx->poly[c][i].y[k];
    }
    tmp.x1=x1;tmp.y1=y1;tmp.x2=x2;tmp.y2=y2;
    updatebbox(count,boundbox,&tmp);
  }

  for(i=0;i<xctx->arcs[c];i++)
  {
    if(selected == 1 && !xctx->arc[c][i].sel) continue;
    if(selected == 2) continue;
    arc_bbox(xctx->arc[c][i].x, xctx->arc[c][i].y, xctx->arc[c][i].r, xctx->arc[c][i].a, xctx->arc[c][i].b,
             &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
    count++;
    updatebbox(count,boundbox,&tmp);
  }

  for(i=0;i<xctx->rects[c];i++)
  {
   if(selected == 1 && !xctx->rect[c][i].sel) continue;
   if(selected == 2) continue;
   tmp.x1=xctx->rect[c][i].x1;
   tmp.x2=xctx->rect[c][i].x2;
   tmp.y1=xctx->rect[c][i].y1;
   tmp.y2=xctx->rect[c][i].y2;
   count++;
   updatebbox(count,boundbox,&tmp);
  }
 }
 for(i=0;i<xctx->wires;i++)
 {
   int ov, y1, y2;
   if(selected == 1 && !xctx->wire[i].sel) continue;
   if(selected == 2) {
     const char *str;
     str = get_tok_value(xctx->wire[i].prop_ptr, "lab",0);
     if(!str[0] || !bus_hilight_lookup(str, 0,XLOOKUP)) continue;
   }
   if(xctx->wire[i].bus){
     ov = INT_BUS_WIDTH(xctx->lw)> cadhalfdotsize ? INT_BUS_WIDTH(xctx->lw) : CADHALFDOTSIZE;
     if(xctx->wire[i].y1 < xctx->wire[i].y2) { y1 = xctx->wire[i].y1-ov; y2 = xctx->wire[i].y2+ov; }
     else                        { y1 = xctx->wire[i].y1+ov; y2 = xctx->wire[i].y2-ov; }
   } else {
     ov = cadhalfdotsize;
     if(xctx->wire[i].y1 < xctx->wire[i].y2) { y1 = xctx->wire[i].y1-ov; y2 = xctx->wire[i].y2+ov; }
     else                        { y1 = xctx->wire[i].y1+ov; y2 = xctx->wire[i].y2-ov; }
   }
   tmp.x1 = xctx->wire[i].x1-ov;
   tmp.x2 = xctx->wire[i].x2+ov;
   tmp.y1 = y1;
   tmp.y2 = y2;
   count++;
   updatebbox(count,boundbox,&tmp);
 }
 if(has_x) for(i=0;i<xctx->texts;i++)
 {
   if(selected == 1 && !xctx->text[i].sel) continue;
   if(selected == 2) continue;
   #ifdef HAS_CAIRO
   customfont = set_text_custom_font(&xctx->text[i]);
   #endif
   if(text_bbox(xctx->text[i].txt_ptr, xctx->text[i].xscale,
         xctx->text[i].yscale,xctx->text[i].rot, xctx->text[i].flip,
         xctx->text[i].hcenter, xctx->text[i].vcenter,
         xctx->text[i].x0, xctx->text[i].y0,
         &tmp.x1,&tmp.y1, &tmp.x2,&tmp.y2) ) {
     count++;
     updatebbox(count,boundbox,&tmp);
   }
   #ifdef HAS_CAIRO
   if(customfont) cairo_restore(cairo_ctx);
   #endif
 }
 for(i=0;i<xctx->instances;i++)
 {
  char *type;
  struct hilight_hashentry *entry;

  if(selected == 1 && !xctx->inst[i].sel) continue;

  if(selected == 2) {
    int j, rects, found, hilight_conn_inst;
    type = (xctx->inst[i].ptr+ xctx->sym)->type;
    found = 0;
    hilight_conn_inst = !strcmp(get_tok_value((xctx->inst[i].ptr+ xctx->sym)->prop_ptr, "highlight", 0), "true") ||
                             !strcmp(get_tok_value(xctx->inst[i].prop_ptr, "highlight", 0), "true");
    if( hilight_conn_inst && (rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER]) > 0 ) {
      prepare_netlist_structs(0);
      for(j=0;j<rects;j++) {
        if( xctx->inst[i].node && xctx->inst[i].node[j]) {
          entry=bus_hilight_lookup(xctx->inst[i].node[j], 0, XLOOKUP);
          if(entry) {
            found = 1;
            break;
          }
        }
      }
    }
    else if( type && IS_LABEL_OR_PIN(type)) {
      entry=bus_hilight_lookup( get_tok_value(xctx->inst[i].prop_ptr,"lab",0) , 0, XLOOKUP );
      if(entry) found = 1;
    }
    else if( (xctx->inst[i].flags & 4) ) {
      found = 1;
    }
    if(!found) continue;
  }




  /* cpu hog 20171206 */
  /*  symbol_bbox(i, &xctx->inst[i].x1, &xctx->inst[i].y1, &xctx->inst[i].x2, &xctx->inst[i].y2); */
  tmp.x1=xctx->inst[i].x1;
  tmp.y1=xctx->inst[i].y1;
  tmp.x2=xctx->inst[i].x2;
  tmp.y2=xctx->inst[i].y2;
  count++;
  updatebbox(count,boundbox,&tmp);
 }

}

void zoom_full(int dr, int sel)
{
  xRect boundbox;
  double yy1;

  if(change_lw) {
    xctx->lw = 1.;
  }
  areax1 = -2*INT_WIDTH(xctx->lw);
  areay1 = -2*INT_WIDTH(xctx->lw);
  areax2 = xrect[0].width+2*INT_WIDTH(xctx->lw);
  areay2 = xrect[0].height+2*INT_WIDTH(xctx->lw);
  areaw = areax2-areax1;
  areah = areay2 - areay1;

  calc_drawing_bbox(&boundbox, sel);
  xctx->zoom=(boundbox.x2-boundbox.x1)/(areaw-4*INT_WIDTH(xctx->lw));
  yy1=(boundbox.y2-boundbox.y1)/(areah-4*INT_WIDTH(xctx->lw));
  if(yy1>xctx->zoom) xctx->zoom=yy1;
  xctx->zoom*=1.05;
  xctx->mooz=1/xctx->zoom;
  xctx->xorigin=-boundbox.x1+(areaw-4*INT_WIDTH(xctx->lw))/40*xctx->zoom;
  xctx->yorigin=(areah-4*INT_WIDTH(xctx->lw))*xctx->zoom-boundbox.y2 - (areah-4*INT_WIDTH(xctx->lw))/40*xctx->zoom;
  dbg(1, "zoom_full(): areaw=%d, areah=%d\n", areaw, areah);

  change_linewidth(-1.);
  if(dr)
  {
   if(!has_x) return;
   draw();
  }
}


void view_zoom(double z)
{
    double factor;
    /*  int i; */
    factor = z!=0.0 ? z : CADZOOMSTEP;
    if(xctx->zoom<CADMINZOOM) return;
    xctx->zoom/= factor;
    xctx->mooz=1/xctx->zoom;
    xctx->xorigin=-mousex_snap+(mousex_snap+xctx->xorigin)/factor;
    xctx->yorigin=-mousey_snap+(mousey_snap+xctx->yorigin)/factor;
    change_linewidth(-1.);
    draw();
}

void view_unzoom(double z)
{
  double factor;
  /*  int i; */
  factor = z!=0.0 ? z : CADZOOMSTEP;
  if(xctx->zoom>CADMAXZOOM) return;
  xctx->zoom*= factor;
  xctx->mooz=1/xctx->zoom;
  /* 20181022 make unzoom and zoom symmetric  */
  /* keeping the mouse pointer as the origin */
  if(unzoom_nodrift) {
    xctx->xorigin=-mousex_snap+(mousex_snap+xctx->xorigin)*factor;
    xctx->yorigin=-mousey_snap+(mousey_snap+xctx->yorigin)*factor;
  } else {
    xctx->xorigin=xctx->xorigin+areaw*xctx->zoom*(1-1/factor)/2;
    xctx->yorigin=xctx->yorigin+areah*xctx->zoom*(1-1/factor)/2;
  }
  change_linewidth(-1.);
  draw();
}

void zoom_box(int what)
{
  static double x1,y1,x2,y2;
  static double xx1,yy1,xx2,yy2;

  if( (what & BEGIN) )
  {
    x1=x2=mousex_snap;y1=y2=mousey_snap;
    ui_state |= STARTZOOM;
  }
  if( what & END)
  {
    ui_state &= ~STARTZOOM;
    RECTORDER(x1,y1,x2,y2);
    drawtemprect(gctiled, NOW, xx1,yy1,xx2,yy2);
    xctx->xorigin=-x1;xctx->yorigin=-y1;
    xctx->zoom=(x2-x1)/(areaw-4*INT_WIDTH(xctx->lw));
    yy1=(y2-y1)/(areah-4*INT_WIDTH(xctx->lw));
    if(yy1>xctx->zoom) xctx->zoom=yy1;
    xctx->mooz=1/xctx->zoom;
    change_linewidth(-1.);
    draw();
    dbg(1, "zoom_box(): coord: %.16g %.16g %.16g %.16g zoom=%.16g\n",x1,y1,mousex_snap, mousey_snap,xctx->zoom);
  }
  if(what & RUBBER)
  {
    xx1=x1;yy1=y1;xx2=x2;yy2=y2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gctiled,NOW, xx1,yy1,xx2,yy2);
    x2=mousex_snap;y2=mousey_snap;


    /*  20171211 update selected objects while dragging */
    rebuild_selected_array();
    bbox(BEGIN,0.0, 0.0, 0.0, 0.0);
    bbox(ADD, xx1, yy1, xx2, yy2);
    bbox(SET,0.0, 0.0, 0.0, 0.0);
    draw_selection(gc[SELLAYER], 0);
    bbox(END,0.0, 0.0, 0.0, 0.0);

    xx1=x1;yy1=y1;xx2=x2;yy2=y2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gc[SELLAYER], NOW, xx1,yy1,xx2,yy2);
  }
}

void draw_stuff(void)
{
   double x1,y1,w,h, x2, y2;
   int i;
   clear_drawing();
   view_unzoom(40);
   for(i=0;i<=210000;i++)
    {
     w=(float)(areaw*xctx->zoom/800) * rand() / (RAND_MAX+1.0);
     h=(float)(areah*xctx->zoom/80) * rand() / (RAND_MAX+1.0);
     x1=(float)(areaw*xctx->zoom) * rand() / (RAND_MAX+1.0)-xctx->xorigin;
     y1=(float)(areah*xctx->zoom) * rand() / (RAND_MAX+1.0)-xctx->yorigin;
     x2=x1+w;
     y2=y1+h;
     ORDER(x1,y1,x2,y2);
     rectcolor = (int) (16.0*rand()/(RAND_MAX+1.0))+4;
     storeobject(-1, x1,y1,x2,y2, xRECT,rectcolor, 0, NULL);
   }

   for(i=0;i<=210000;i++)
    {
     w=(float)(areaw*xctx->zoom/80) * rand() / (RAND_MAX+1.0);
     h=(float)(areah*xctx->zoom/800) * rand() / (RAND_MAX+1.0);
     x1=(float)(areaw*xctx->zoom) * rand() / (RAND_MAX+1.0)-xctx->xorigin;
     y1=(float)(areah*xctx->zoom) * rand() / (RAND_MAX+1.0)-xctx->yorigin;
     x2=x1+w;
     y2=y1+h;
     ORDER(x1,y1,x2,y2);
     rectcolor = (int) (16.0*rand()/(RAND_MAX+1.0))+4;
     storeobject(-1, x1,y1,x2,y2,xRECT,rectcolor, 0, NULL);
   }

   for(i=0;i<=210000;i++)
   {
     w=(float)xctx->zoom * rand() / (RAND_MAX+1.0);
     h=w;
     x1=(float)(areaw*xctx->zoom) * rand() / (RAND_MAX+1.0)-xctx->xorigin;
     y1=(float)(areah*xctx->zoom) * rand() / (RAND_MAX+1.0)-xctx->yorigin;
     x2=x1+w;
     y2=y1+h;
     rectcolor = (int) (10.0*rand()/(RAND_MAX+1.0))+4;
     RECTORDER(x1,y1,x2,y2);
     storeobject(-1, x1,y1,x2,y2,xRECT,rectcolor, 0, NULL);
   }
}

void restore_selection(double x1, double y1, double x2, double y2)
{
  double xx1,yy1,xx2,yy2;
  xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
  RECTORDER(xx1,yy1,xx2,yy2);
  rebuild_selected_array();
  if(!lastselected) return;
  bbox(BEGIN,0.0, 0.0, 0.0, 0.0);
  bbox(ADD, xx1, yy1, xx2, yy2);
  bbox(SET,0.0, 0.0, 0.0, 0.0);
  draw_selection(gc[SELLAYER], 0);
  bbox(END,0.0, 0.0, 0.0, 0.0);
}

void new_wire(int what, double mx_snap, double my_snap)
{
 static double x1,y1,x2,y2;
 static double xx1,yy1,xx2,yy2;

   if( (what & PLACE) ) {
     if( (ui_state & STARTWIRE) && (x1!=x2 || y1!=y2) ) {
       push_undo();
       if(manhattan_lines==1) {
         if(xx2!=xx1) {
           xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
           ORDER(xx1,yy1,xx2,yy1);
           storeobject(-1, xx1,yy1,xx2,yy1,WIRE,0,0,NULL);
           drawline(WIRELAYER,NOW, xx1,yy1,xx2,yy1, 0);
         }
         if(yy2!=yy1) {
           xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
           ORDER(xx2,yy1,xx2,yy2);
           storeobject(-1, xx2,yy1,xx2,yy2,WIRE,0,0,NULL);
           drawline(WIRELAYER,NOW, xx2,yy1,xx2,yy2, 0);
         }
       } else if(manhattan_lines==2) {
         if(yy2!=yy1) {
           xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
           ORDER(xx1,yy1,xx1,yy2);
           storeobject(-1, xx1,yy1,xx1,yy2,WIRE,0,0,NULL);
           drawline(WIRELAYER,NOW, xx1,yy1,xx1,yy2, 0);
         }
         if(xx2!=xx1) {
           xx1=x1;yy1=y1;xx2=x2;yy2=y2;
           ORDER(xx1,yy2,xx2,yy2);
           storeobject(-1, xx1,yy2,xx2,yy2,WIRE,0,0,NULL);
           drawline(WIRELAYER,NOW, xx1,yy2,xx2,yy2, 0);
         }
       } else {
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy1,xx2,yy2);
         storeobject(-1, xx1,yy1,xx2,yy2,WIRE,0,0,NULL);
         drawline(WIRELAYER,NOW, xx1,yy1,xx2,yy2, 0);
       }
       if(event_reporting) {
         printf("xschem wire %g %g %g %g %d\n", xx1, yy1, xx2, yy2, -1);
         fflush(stdout);
       }
       hash_wire(XINSERT, xctx->wires-1, 1);
       /* prepared_hash_wires = 0; */
       prepared_hilight_structs = 0;
       update_conn_cues(1,1);
       if(show_pin_net_names) {
         prepare_netlist_structs(0);
         bbox(BEGIN , 0.0 , 0.0 , 0.0 , 0.0);
         find_inst_to_be_redrawn(xctx->wire[xctx->wires-1].node);
         find_inst_hash_clear();
         bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
         draw();
         bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
       }
       draw_hilight_net(1);/* for updating connection bubbles on hilight nets */
     }
     if(! (what &END)) {
       x1=mx_snap;
       y1=my_snap;
       x2=mousex_snap;
       y2=mousey_snap;
       xx1=x1;
       yy1=y1;
       xx2=mousex_snap;
       yy2=mousey_snap;
       if(manhattan_lines==1) {
         x2 = mx_snap; y2 = my_snap;
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy1,xx2,yy1);
         drawtempline(gc[WIRELAYER], NOW, xx1,yy1,xx2,yy1);
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx2,yy1,xx2,yy2);
         drawtempline(gc[WIRELAYER], NOW, xx2,yy1,xx2,yy2);
       } else if(manhattan_lines==2) {
         x2 = mx_snap; y2 = my_snap;
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy1,xx1,yy2);
         drawtempline(gc[WIRELAYER], NOW, xx1,yy1,xx1,yy2);
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy2,xx2,yy2);
         drawtempline(gc[WIRELAYER], NOW, xx1,yy2,xx2,yy2);
       } else {
         x2 = mx_snap; y2 = my_snap;
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy1,xx2,yy2);
         drawtempline(gc[WIRELAYER], NOW, xx1,yy1,xx2,yy2);
       }
     }
     ui_state |= STARTWIRE;
   }
   if( what & END) {
     ui_state &= ~STARTWIRE;
   }
   if( (what & RUBBER)  ) {
     if(manhattan_lines==1) {
       xx1=x1;yy1=y1;xx2=x2;yy2=y2;
       ORDER(xx1,yy1,xx2,yy1);
       drawtempline(gctiled, NOW, xx1,yy1,xx2,yy1);
       xx1=x1;yy1=y1;xx2=x2;yy2=y2;
       ORDER(xx2,yy1,xx2,yy2);
       drawtempline(gctiled, NOW, xx2,yy1,xx2,yy2);
       restore_selection(x1, y1, x2, y2);
       x2 = mx_snap; y2 = my_snap;
       if(!(what & CLEAR)) {
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy1,xx2,yy1);
         drawtempline(gc[WIRELAYER], NOW, xx1,yy1,xx2,yy1);
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx2,yy1,xx2,yy2);
         drawtempline(gc[WIRELAYER], NOW, xx2,yy1,xx2,yy2);
       }
     } else if(manhattan_lines==2) {
       xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
       ORDER(xx1,yy1,xx1,yy2);
       drawtempline(gctiled, NOW, xx1,yy1,xx1,yy2);
       xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
       ORDER(xx1,yy2,xx2,yy2);
       drawtempline(gctiled, NOW, xx1,yy2,xx2,yy2);
       restore_selection(x1, y1, x2, y2);
       x2 = mx_snap; y2 = my_snap;
       if(!(what & CLEAR)) {
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy1,xx1,yy2);
         drawtempline(gc[WIRELAYER], NOW, xx1,yy1,xx1,yy2);
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy2,xx2,yy2);
         drawtempline(gc[WIRELAYER], NOW, xx1,yy2,xx2,yy2);
       }
     } else {
       xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
       ORDER(xx1,yy1,xx2,yy2);
       drawtempline(gctiled, NOW, xx1,yy1,xx2,yy2);
       restore_selection(x1, y1, x2, y2);
       x2 = mx_snap; y2 = my_snap;
       if(!(what & CLEAR)) {
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy1,xx2,yy2);
         drawtempline(gc[WIRELAYER], NOW, xx1,yy1,xx2,yy2);
       }
     }
   }
}

void change_layer()
{
  int k, n, type, c;
  double x1,y1,x2,y2, a, b, r;


   if(lastselected) push_undo();
   for(k=0;k<lastselected;k++)
   {
     n=selectedgroup[k].n;
     type=selectedgroup[k].type;
     c=selectedgroup[k].col;
     if(type==LINE && xctx->line[c][n].sel==SELECTED) {
       x1 = xctx->line[c][n].x1;
       y1 = xctx->line[c][n].y1;
       x2 = xctx->line[c][n].x2;
       y2 = xctx->line[c][n].y2;
       storeobject(-1, x1,y1,x2,y2,LINE,rectcolor, 0, xctx->line[c][n].prop_ptr);
     }
     if(type==ARC && xctx->arc[c][n].sel==SELECTED) {
       x1 = xctx->arc[c][n].x;
       y1 = xctx->arc[c][n].y;
       r = xctx->arc[c][n].r;
       a = xctx->arc[c][n].a;
       b = xctx->arc[c][n].b;
       store_arc(-1, x1, y1, r, a, b, rectcolor, 0, xctx->arc[c][n].prop_ptr);
     }
     if(type==POLYGON && xctx->poly[c][n].sel==SELECTED) {
        store_poly(-1, xctx->poly[c][n].x, xctx->poly[c][n].y, 
                       xctx->poly[c][n].points, rectcolor, 0, xctx->poly[c][n].prop_ptr);
     }
     else if(type==xRECT && xctx->rect[c][n].sel==SELECTED) {
       x1 = xctx->rect[c][n].x1;
       y1 = xctx->rect[c][n].y1;
       x2 = xctx->rect[c][n].x2;
       y2 = xctx->rect[c][n].y2;
       storeobject(-1, x1,y1,x2,y2,xRECT,rectcolor, 0, xctx->rect[c][n].prop_ptr);
     }
   }
   if(lastselected) delete_only_rect_line_arc_poly();
   unselect_all();
}

void new_arc(int what, double sweep)
{
  static double x, y, r, a, b;
  static double x1, y1, x2, y2, x3, y3;
  static double xx1, yy1, xx2, yy2;
  static int state;
  static double sweep_angle;
  if(what & PLACE) {
    state=0;
    r = -1.;
    sweep_angle=sweep;
    xx1 = xx2 = x1 = x2 = x3 = mousex_snap;
    yy1 = yy2 = y1 = y2 = y3 = mousey_snap;
    ui_state |= STARTARC;
  }
  if(what & SET) {
    if(state==0) {
      x2 = mousex_snap;
      y2 = mousey_snap;
      drawtempline(gctiled, NOW, xx1,yy1,xx2,yy2);
      state=1;
    } else if(state==1) {
      x3 = mousex_snap;
      y3 = mousey_snap;
      arc_3_points(x1, y1, x2, y2, x3, y3, &x, &y, &r, &a, &b);
      if(sweep_angle==360.) b=360.;
      if(r>0.) {
        push_undo();
        drawarc(rectcolor, NOW, x, y, r, a, b, 0, 0);
        store_arc(-1, x, y, r, a, b, rectcolor, 0, NULL);
      }
      ui_state &= ~STARTARC;
      state=0;
    }
  }
  if(what & RUBBER) {
    if(state==0) {
      drawtempline(gctiled, NOW, xx1,yy1,xx2,yy2);
      xx2 = mousex_snap;
      yy2 = mousey_snap;
      xx1 = x1;yy1 = y1;
      ORDER(xx1,yy1,xx2,yy2);
      drawtempline(gc[SELLAYER], NOW, xx1,yy1,xx2,yy2);
    }
    else if(state==1) {
      x3 = mousex_snap;
      y3 = mousey_snap;
      if(r>0.) drawtemparc(gctiled, NOW, x, y, r, a, b);
      arc_3_points(x1, y1, x2, y2, x3, y3, &x, &y, &r, &a, &b);
      if(sweep_angle==360.) b=360.;
      if(r>0.) drawtemparc(gc[rectcolor], NOW, x, y, r, a, b);
    }
  }
}
void new_line(int what)
{
 static double x1,y1,x2,y2;
 static double xx1,yy1,xx2,yy2;

   if( (what & PLACE) )
   {
     if( (x1!=x2 || y1!=y2) && (ui_state & STARTLINE) )
     {
       push_undo();
       if(manhattan_lines==1) {
         if(xx2!=xx1) {
           xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
           ORDER(xx1,yy1,xx2,yy1);
           storeobject(-1, xx1,yy1,xx2,yy1,LINE,rectcolor,0,NULL);
           drawline(rectcolor,NOW, xx1,yy1,xx2,yy1, 0);
         }
         if(yy2!=yy1) {
           xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
           ORDER(xx2,yy1,xx2,yy2);
           storeobject(-1, xx2,yy1,xx2,yy2,LINE,rectcolor,0,NULL);
           drawline(rectcolor,NOW, xx2,yy1,xx2,yy2, 0);
         }
       } else if(manhattan_lines==2) {
         if(yy2!=yy1) {
           xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
           ORDER(xx1,yy1,xx1,yy2);
           storeobject(-1, xx1,yy1,xx1,yy2,LINE,rectcolor,0,NULL);
           drawline(rectcolor,NOW, xx1,yy1,xx1,yy2, 0);
         }
         if(xx2!=xx1) {
           xx1=x1;yy1=y1;xx2=x2;yy2=y2;
           ORDER(xx1,yy2,xx2,yy2);
           storeobject(-1, xx1,yy2,xx2,yy2,LINE,rectcolor,0,NULL);
           drawline(rectcolor,NOW, xx1,yy2,xx2,yy2, 0);
         }
       } else {
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy1,xx2,yy2);
         storeobject(-1, xx1,yy1,xx2,yy2,LINE,rectcolor,0,NULL);
         drawline(rectcolor,NOW, xx1,yy1,xx2,yy2, 0);
       }
     }
     x1=x2=mousex_snap;y1=y2=mousey_snap;
     ui_state |= STARTLINE;
   }
   if( what & END)
   {
     ui_state &= ~STARTLINE;
   }

   if(what & RUBBER)
   {
     if(manhattan_lines==1) {
       xx1=x1;yy1=y1;xx2=x2;yy2=y2;
       ORDER(xx1,yy1,xx2,yy1);
       drawtempline(gctiled, NOW, xx1,yy1,xx2,yy1);
       xx1=x1;yy1=y1;xx2=x2;yy2=y2;
       ORDER(xx2,yy1,xx2,yy2);
       drawtempline(gctiled, NOW, xx2,yy1,xx2,yy2);
       restore_selection(x1, y1, x2, y2);
       x2 = mousex_snap; y2 = mousey_snap;
       if(!(what & CLEAR)) {
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy1,xx2,yy1);
         drawtempline(gc[rectcolor], NOW, xx1,yy1,xx2,yy1);
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx2,yy1,xx2,yy2);
         drawtempline(gc[rectcolor], NOW, xx2,yy1,xx2,yy2);
       }
     } else if(manhattan_lines==2) {
       xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
       ORDER(xx1,yy1,xx1,yy2);
       drawtempline(gctiled, NOW, xx1,yy1,xx1,yy2);
       xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
       ORDER(xx1,yy2,xx2,yy2);
       drawtempline(gctiled, NOW, xx1,yy2,xx2,yy2);
       restore_selection(x1, y1, x2, y2);
       x2 = mousex_snap; y2 = mousey_snap;
       if(!(what & CLEAR)) {
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy1,xx1,yy2);
         drawtempline(gc[rectcolor], NOW, xx1,yy1,xx1,yy2);
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy2,xx2,yy2);
         drawtempline(gc[rectcolor], NOW, xx1,yy2,xx2,yy2);
       }
     } else {
       xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
       ORDER(xx1,yy1,xx2,yy2);
       drawtempline(gctiled, NOW, xx1,yy1,xx2,yy2);
       restore_selection(x1, y1, x2, y2);
       x2 = mousex_snap; y2 = mousey_snap;
       if(!(what & CLEAR)) {
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy1,xx2,yy2);
         drawtempline(gc[rectcolor], NOW, xx1,yy1,xx2,yy2);
       }
     }
   }
}

void new_rect(int what)
{
 static double x1,y1,x2,y2;
 static double xx1,yy1,xx2,yy2;

   if( (what & PLACE) )
   {
    if( (x1!=x2 || y1!=y2) && (ui_state & STARTRECT) )
    {
     int save_draw;
     RECTORDER(x1,y1,x2,y2);
     push_undo();
     drawrect(rectcolor, NOW, x1,y1,x2,y2, 0);
     save_draw = draw_window;
     draw_window = 1;
     filledrect(rectcolor, NOW, x1,y1,x2,y2); /* draw fill pattern even in XCopyArea mode */
     draw_window = save_draw;
     storeobject(-1, x1,y1,x2,y2,xRECT,rectcolor, 0, NULL);
    }
    x1=x2=mousex_snap;y1=y2=mousey_snap;
    ui_state |= STARTRECT;
   }
   if( what & END)
   {
    ui_state &= ~STARTRECT;
   }
   if(what & RUBBER)
   {
    xx1=x1;yy1=y1;xx2=x2;yy2=y2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gctiled,NOW, xx1,yy1,xx2,yy2);
    x2=mousex_snap;y2=mousey_snap;
    xx1=x1;yy1=y1;xx2=x2;yy2=y2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gc[rectcolor], NOW, xx1,yy1,xx2,yy2);
   }
}


void new_polygon(int what)
{
 static double *x=NULL, *y=NULL;
 static int points=0;
 static int maxpoints=0;

   if( what & PLACE ) points=0; /*  start new polygon placement */

   if(points >= maxpoints-1) {  /*  check storage for 2 points */
     maxpoints = (1+points / CADCHUNKALLOC) * CADCHUNKALLOC;
     my_realloc(17, &x, sizeof(double)*maxpoints);
     my_realloc(18, &y, sizeof(double)*maxpoints);
   }
   if( what & PLACE )
   {
     /* fprintf(errfp, "new_poly: PLACE, points=%d\n", points); */
     y[points]=mousey_snap;
     x[points]=mousex_snap;
     points++;
     x[points]=x[points-1]; /* prepare next point for rubber */
     y[points] = y[points-1];
     /* fprintf(errfp, "added point: %.16g %.16g\n", x[points-1], y[points-1]); */
     ui_state |= STARTPOLYGON;
   }
   if( what & ADD)
   {
     /* closed poly */
     if(what & END) {
       /* delete last rubber */
       drawtemppolygon(gctiled, NOW, x, y, points+1);
       x[points] = x[0];
       y[points] = y[0];
     /* add point */
     } else if(x[points] != x[points-1] || y[points] != y[points-1]) {
       x[points] = mousex_snap;
       y[points] = mousey_snap;
     } else {
       return;
     }
     points++;
     /* prepare next point for rubber */
     x[points]=x[points-1];y[points]=y[points-1];
   }
   /* end open or closed poly  by user request */
   if((what & SET || (what & END)) ||
        /* closed poly end by clicking on first point */
        ((what & ADD) && x[points-1] == x[0] && y[points-1] == y[0]) ) {
     push_undo();
     drawtemppolygon(gctiled, NOW, x, y, points+1);
     store_poly(-1, x, y, points, rectcolor, 0, NULL);
     /* fprintf(errfp, "new_poly: finish: points=%d\n", points); */
     drawtemppolygon(gc[rectcolor], NOW, x, y, points);
     ui_state &= ~STARTPOLYGON;
     drawpolygon(rectcolor, NOW, x, y, points, 0, 0);
     my_free(711, &x);
     my_free(712, &y);
     maxpoints = points = 0;
   }
   if(what & RUBBER)
   {
     /* fprintf(errfp, "new_poly: RUBBER\n"); */
     drawtemppolygon(gctiled, NOW, x, y, points+1);
     y[points] = mousey_snap;
     x[points] = mousex_snap;
     drawtemppolygon(gc[rectcolor], NOW, x, y, points+1);
   }
}

#ifdef HAS_CAIRO
int text_bbox(const char *str, double xscale, double yscale,
    int rot, int flip, int hcenter, int vcenter, double x1,double y1, double *rx1, double *ry1,
    double *rx2, double *ry2)
{
  int c=0;
  char *str_ptr, *s = NULL;
  double size;
  cairo_text_extents_t ext;
  cairo_font_extents_t fext;
  double ww, hh, maxw;

  if(!has_x) return 0;
  size = xscale*52.*cairo_font_scale;

  /*  if(size*xctx->mooz>800.) { */
  /*    return 0; */
  /*  } */
  cairo_set_font_size (cairo_ctx, size*xctx->mooz);
  cairo_font_extents(cairo_ctx, &fext);

  ww=0.; hh=1.;
  c=0;
  cairo_lines=1;
  my_strdup2(1158, &s, str);
  str_ptr = s;
  while( s && s[c] ) {
    if(s[c] == '\n') {
      s[c]='\0';
      hh++;
      cairo_lines++;
      if(str_ptr[0]!='\0') {
        cairo_text_extents(cairo_ctx, str_ptr, &ext);
        maxw = ext.x_advance > ext.width ? ext.x_advance : ext.width;
        if(maxw > ww) ww= maxw;
      }
      s[c]='\n';
      str_ptr = s+c+1;
    } else {
    }
    c++;
  }
  if(str_ptr && str_ptr[0]!='\0') {
    cairo_text_extents(cairo_ctx, str_ptr, &ext);
    maxw = ext.x_advance > ext.width ? ext.x_advance : ext.width;
    if(maxw > ww) ww= maxw;
  }
  my_free(1159, &s);
  hh = hh*fext.height*cairo_font_line_spacing;
  cairo_longest_line = ww;

  *rx1=x1;*ry1=y1;
  if(hcenter) {
    if     (rot==0 && flip == 0) { *rx1-= ww*xctx->zoom/2;}
    else if(rot==1 && flip == 0) { *ry1-= ww*xctx->zoom/2;}
    else if(rot==2 && flip == 0) { *rx1+= ww*xctx->zoom/2;}
    else if(rot==3 && flip == 0) { *ry1+= ww*xctx->zoom/2;}
    else if(rot==0 && flip == 1) { *rx1+= ww*xctx->zoom/2;}
    else if(rot==1 && flip == 1) { *ry1+= ww*xctx->zoom/2;}
    else if(rot==2 && flip == 1) { *rx1-= ww*xctx->zoom/2;}
    else if(rot==3 && flip == 1) { *ry1-= ww*xctx->zoom/2;}
  }

  if(vcenter) {
    if     (rot==0 && flip == 0) { *ry1-= hh*xctx->zoom/2;}
    else if(rot==1 && flip == 0) { *rx1+= hh*xctx->zoom/2;}
    else if(rot==2 && flip == 0) { *ry1+= hh*xctx->zoom/2;}
    else if(rot==3 && flip == 0) { *rx1-= hh*xctx->zoom/2;}
    else if(rot==0 && flip == 1) { *ry1-= hh*xctx->zoom/2;}
    else if(rot==1 && flip == 1) { *rx1+= hh*xctx->zoom/2;}
    else if(rot==2 && flip == 1) { *ry1+= hh*xctx->zoom/2;}
    else if(rot==3 && flip == 1) { *rx1-= hh*xctx->zoom/2;}
  }


  ROTATION(0.0,0.0, ww*xctx->zoom,hh*xctx->zoom,(*rx2),(*ry2));
  *rx2+=*rx1;*ry2+=*ry1;
  if     (rot==0) {*ry1-=cairo_vert_correct; *ry2-=cairo_vert_correct;}
  else if(rot==1) {*rx1+=cairo_vert_correct; *rx2+=cairo_vert_correct;}
  else if(rot==2) {*ry1+=cairo_vert_correct; *ry2+=cairo_vert_correct;}
  else if(rot==3) {*rx1-=cairo_vert_correct; *rx2-=cairo_vert_correct;}
  RECTORDER((*rx1),(*ry1),(*rx2),(*ry2));
  return 1;
}
int text_bbox_nocairo(const char * str,double xscale, double yscale,
    int rot, int flip, int hcenter, int vcenter, double x1,double y1, double *rx1, double *ry1,
    double *rx2, double *ry2)
#else
int text_bbox(const char * str,double xscale, double yscale,
    int rot, int flip, int hcenter, int vcenter, double x1,double y1, double *rx1, double *ry1,
    double *rx2, double *ry2)
#endif
{
 register int c=0, length =0;
 double w, h;

  w=0;h=1;
  if(str!=NULL) while( str[c] )
  {
   if((str)[c++]=='\n') {h++;length=0;}
   else length++;
   if(length > w)
     w = length;
  }
  w *= (FONTWIDTH+FONTWHITESPACE)*xscale*nocairo_font_xscale;
  h *= (FONTHEIGHT+FONTDESCENT+FONTWHITESPACE)*yscale*nocairo_font_yscale;
  *rx1=x1;*ry1=y1;
  if(     rot==0) *ry1-=nocairo_vert_correct;
  else if(rot==1) *rx1+=nocairo_vert_correct;
  else if(rot==2) *ry1+=nocairo_vert_correct;
  else            *rx1-=nocairo_vert_correct;

  if(hcenter) {
    if     (rot==0 && flip == 0) { *rx1-= w/2;}
    else if(rot==1 && flip == 0) { *ry1-= w/2;}
    else if(rot==2 && flip == 0) { *rx1+= w/2;}
    else if(rot==3 && flip == 0) { *ry1+= w/2;}
    else if(rot==0 && flip == 1) { *rx1+= w/2;}
    else if(rot==1 && flip == 1) { *ry1+= w/2;}
    else if(rot==2 && flip == 1) { *rx1-= w/2;}
    else if(rot==3 && flip == 1) { *ry1-= w/2;}
  }

  if(vcenter) {
    if     (rot==0 && flip == 0) { *ry1-= h/2;}
    else if(rot==1 && flip == 0) { *rx1+= h/2;}
    else if(rot==2 && flip == 0) { *ry1+= h/2;}
    else if(rot==3 && flip == 0) { *rx1-= h/2;}
    else if(rot==0 && flip == 1) { *ry1-= h/2;}
    else if(rot==1 && flip == 1) { *rx1+= h/2;}
    else if(rot==2 && flip == 1) { *ry1+= h/2;}
    else if(rot==3 && flip == 1) { *rx1-= h/2;}
  }

  ROTATION(0.0,0.0,w,h,(*rx2),(*ry2));
  *rx2+=*rx1;*ry2+=*ry1;
  RECTORDER((*rx1),(*ry1),(*rx2),(*ry2));
  return 1;
}

void place_text(int draw_text, double mx, double my)
{
  char *txt;
  int textlayer;
  const char *str;
  int save_draw;
  xText *t = &xctx->text[xctx->texts];
  #ifdef HAS_CAIRO
  char  *textfont;
  #endif

  tclsetvar("props","");
  tclsetvar("retval","");

  if(tclgetvar("hsize")==NULL)
   tclsetvar("hsize","0.4");
  if(tclgetvar("vsize")==NULL)
   tclsetvar("vsize","0.4");
  tcleval("enter_text {text:} normal");

  dbg(1, "place_text(): hsize=%s vsize=%s\n",tclgetvar("hsize"), tclgetvar("vsize") );

  txt =  (char *)tclgetvar("retval");
  if(!strcmp(txt,"")) return;   /*  dont allocate text object if empty string given */
  push_undo();
  check_text_storage();
  t->txt_ptr=NULL;
  t->prop_ptr=NULL;  /*  20111006 added missing initialization of pointer */
  t->font=NULL;
  my_strdup(19, &t->txt_ptr, txt);
  t->x0=mx;
  t->y0=my;
  t->rot=0;
  t->flip=0;
  t->sel=0;
  t->xscale= atof(tclgetvar("hsize"));
  t->yscale= atof(tclgetvar("vsize"));
  my_strdup(20, &t->prop_ptr, (char *)tclgetvar("props"));
  /*  debug ... */
  /*  t->prop_ptr=NULL; */
  dbg(1, "place_text(): done text input\n");

  str = get_tok_value(t->prop_ptr, "hcenter", 0);
  t->hcenter = strcmp(str, "true")  ? 0 : 1;
  str = get_tok_value(t->prop_ptr, "vcenter", 0);
  t->vcenter = strcmp(str, "true")  ? 0 : 1;

  str = get_tok_value(t->prop_ptr, "layer", 0);
  if(str[0]) t->layer = atoi(str);
  else t->layer = -1;

  t->flags = 0;
  str = get_tok_value(t->prop_ptr, "slant", 0);
  t->flags |= strcmp(str, "oblique")  ? 0 : TEXT_OBLIQUE;
  t->flags |= strcmp(str, "italic")  ? 0 : TEXT_ITALIC;
  str = get_tok_value(t->prop_ptr, "weight", 0);
  t->flags |= strcmp(str, "bold")  ? 0 : TEXT_BOLD;

  my_strdup(21, &t->font, get_tok_value(t->prop_ptr, "font", 0));
  textlayer = t->layer;
  if(textlayer < 0 || textlayer >= cadlayers) textlayer = TEXTLAYER;
  #ifdef HAS_CAIRO
  textfont = t->font;
  if((textfont && textfont[0]) || t->flags) {
    cairo_font_slant_t slant;
    cairo_font_weight_t weight;
    textfont = (t->font && t->font[0]) ? t->font : cairo_font_name;
    weight = ( t->flags & TEXT_BOLD) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL;
    slant = CAIRO_FONT_SLANT_NORMAL;
    if(t->flags & TEXT_ITALIC) slant = CAIRO_FONT_SLANT_ITALIC;
    if(t->flags & TEXT_OBLIQUE) slant = CAIRO_FONT_SLANT_OBLIQUE;
    cairo_save(cairo_ctx);
    cairo_save(cairo_save_ctx);
    cairo_select_font_face (cairo_ctx, textfont, slant, weight);
    cairo_select_font_face (cairo_save_ctx, textfont, slant, weight);
  }
  #endif
  save_draw=draw_window;
  draw_window=1;
  if(draw_text) {
    draw_string(textlayer, NOW, t->txt_ptr, 0, 0, t->hcenter, t->vcenter, t->x0,t->y0, t->xscale, t->yscale);
  }
  draw_window = save_draw;
  #ifdef HAS_CAIRO
  if((textfont && textfont[0]) || t->flags) {
    cairo_restore(cairo_ctx);
    cairo_restore(cairo_save_ctx);
  }
  #endif
  select_text(xctx->texts, SELECTED, 0);
  drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
  drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
  xctx->texts++;
  set_modify(1);
}

void pan2(int what, int mx, int my)
{
  int dx, dy, ddx, ddy;
  static int mx_save, my_save;
  static int mmx_save, mmy_save;
  static double xorig_save, yorig_save;
  if(what & BEGIN) {
    mmx_save = mx_save = mx;
    mmy_save = my_save = my;
    xorig_save = xctx->xorigin;
    yorig_save = xctx->yorigin;
  }
  else if(what == RUBBER) {
    dx = mx - mx_save;
    dy = my - my_save;
    ddx = abs(mx -mmx_save);
    ddy = abs(my -mmy_save);
    if(ddx>5 || ddy>5) {
      xctx->xorigin = xorig_save + dx*xctx->zoom;
      xctx->yorigin = yorig_save + dy*xctx->zoom;
      draw();
      mmx_save = mx;
      mmy_save = my;
    }
  }
}

void pan(int what)
{
 static double xpan,ypan,xpan2,ypan2;
 static double xx1,xx2,yy1,yy2;
 if(what & RUBBER)
 {
    xx1=xpan;yy1=ypan;xx2=xpan2;yy2=ypan2;
    ORDER(xx1,yy1,xx2,yy2);
    drawtempline(gctiled, NOW, xx1,yy1,xx2,yy2);
    xpan2=mousex_snap;ypan2=mousey_snap;
    xx1=xpan;yy1=ypan;xx2=xpan2;yy2=ypan2;
    ORDER(xx1,yy1,xx2,yy2);
    drawtempline(gc[SELLAYER], NOW, xx1,yy1,xx2,yy2);
 }
 if(what & BEGIN)
 {
    ui_state |= STARTPAN;
    xpan=mousex_snap;ypan=mousey_snap;xpan2=xpan;ypan2=ypan;
 }
 if(what & END)
 {
    ui_state &= ~STARTPAN;
    xctx->xorigin+=-xpan+mousex_snap;xctx->yorigin+=-ypan+mousey_snap;
    draw();
 }
}

/*  20150927 select=1: select objects, select=0: unselect objects */
void select_rect(int what, int select)
{
 static double xr,yr,xr2,yr2;
 static double xx1,xx2,yy1,yy2;
 static int sel;
 static int sem=0;

 if(what & RUBBER)
 {
    if(sem==0) {
      fprintf(errfp, "ERROR: select_rect() RUBBER called before BEGIN\n");
      tcleval("alert_ {ERROR: select_rect() RUBBER called before BEGIN} {}");
    }
    xx1=xr;xx2=xr2;yy1=yr;yy2=yr2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gctiled,NOW, xx1,yy1,xx2,yy2);
    xr2=mousex_snap;yr2=mousey_snap;

    /*  20171026 update unselected objects while dragging */
    rebuild_selected_array();
    bbox(BEGIN,0.0, 0.0, 0.0, 0.0);
    bbox(ADD, xx1, yy1, xx2, yy2);
    bbox(SET,0.0, 0.0, 0.0, 0.0);
    draw_selection(gc[SELLAYER], 0);
    if(!sel) select_inside(xx1, yy1, xx2, yy2, sel);
    bbox(END,0.0, 0.0, 0.0, 0.0);
    xx1=xr;xx2=xr2;yy1=yr;yy2=yr2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gc[SELLAYER],NOW, xx1,yy1,xx2,yy2);
 }
 else if(what & BEGIN)
 {
    /*
     * if(sem==1) {
     *  fprintf(errfp, "ERROR: reentrant call of select_rect()\n");
     *  tcleval("alert_ {ERROR: reentrant call of select_rect()} {}");
     * }
     */
    sel = select;
    ui_state |= STARTSELECT;

    /*  use m[xy]_double_save instead of mouse[xy]_snap */
    /*  to avoid delays in setting the start point of a */
    /*  selection rectangle, this is noticeable and annoying on */
    /*  networked / slow X servers. 20171218 */
    /* xr=xr2=mousex_snap; */
    /* yr=yr2=mousey_snap; */
    xr=xr2=mx_double_save;
    yr=yr2=my_double_save;
    sem=1;
 }
 else if(what & END)
 {
    RECTORDER(xr,yr,xr2,yr2);
    drawtemprect(gctiled, NOW, xr,yr,xr2,yr2);
    /*  draw_selection(gc[SELLAYER], 0); */
    select_inside(xr,yr,xr2,yr2, sel);


    bbox(BEGIN,0.0, 0.0, 0.0, 0.0);
    bbox(ADD, xr, yr, xr2, yr2);
    bbox(SET,0.0, 0.0, 0.0, 0.0);
    draw_selection(gc[SELLAYER], 0);
    bbox(END,0.0, 0.0, 0.0, 0.0);
    /*  /20171219 */

    ui_state &= ~STARTSELECT;
    sem=0;
 }
}

