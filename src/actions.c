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
    if(has_x && strcmp(get_cell(schematic[currentsch],1), "systemlib/font")) {
      if(mod == 1) {
        tcleval( "wm title . \"xschem - [file tail [xschem get schname]]*\"");    /* 20161207 */
        tcleval( "wm iconname . \"xschem - [file tail [xschem get schname]]*\""); /* 20161207 */
      } else {
        tcleval( "wm title . \"xschem - [file tail [xschem get schname]]\"");    /* 20161207 */
        tcleval( "wm iconname . \"xschem - [file tail [xschem get schname]]\""); /* 20161207 */
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
  if(!strcmp("", Tcl_GetStringResult(interp)) ) {
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
  return Tcl_GetStringResult(interp);
}

/* Wrapper to Tcl function */
const char *rel_sym_path(const char *s)
{
  char c[PATH_MAX+1000];
  my_snprintf(c, S(c), "rel_sym_path {%s}", s);
  tcleval(c);
  return Tcl_GetStringResult(interp);
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

void resetwin(void)
{
  int i;
  XWindowAttributes wattr;
  if(has_x) {
#ifdef __unix__
    i = XGetWindowAttributes(display, window, &wattr); /*  should call only when resized */
                                              /*  to avoid server roundtrip replies */
    if(!i) { /*  20171105 */
      return;
    }
    /* if(wattr.map_state==IsUnmapped) return; */

    xschem_w=wattr.width;
    xschem_h=wattr.height;
    areax2 = xschem_w+2*lw;
    areay2 = xschem_h+2*lw;
    areax1 = -2*lw;
    areay1 = -2*lw;
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
  
      XFreePixmap(display,save_pixmap);
 
      /* 
      {
        unsigned int w, h;
        XQueryBestSize(display, TileShape, window,  xschem_w, xschem_h, &w, &h);
        dbg(1, "XQueryBestSize: req: w=%d, h=%d, opt: w=%d h=%d\n", 
                         xschem_w, xschem_h, w, h);
      }
      */

      save_pixmap = XCreatePixmap(display, window, xschem_w, xschem_h, depth); /*  20171111 */
      XSetTile(display,gctiled, save_pixmap);
#else
    Tk_Window mainwindow = Tk_MainWindow(interp);
    HWND hwnd = Tk_GetHWND(Tk_WindowId(mainwindow));
    RECT rect;
    if (GetWindowRect(hwnd, &rect))
    {
      unsigned int width = rect.right - rect.left;
      unsigned int height = rect.bottom - rect.top;
      xschem_w = width;
      xschem_h = height;
      areax2 = xschem_w + 2 * lw;
      areay2 = xschem_h + 2 * lw;
      areax1 = -2 * lw;
      areay1 = -2 * lw;
      areaw = areax2 - areax1;
      areah = areay2 - areay1;
      if (xschem_w != xrect[0].width || xschem_h != xrect[0].height) { /* 20171123 avoid unnecessary work if no resize */
        dbg(1, "resetwin(): x=%d y=%d   xschem_w=%d xschem_h=%d\n",
          rect.right, rect.bottom, xschem_w, xschem_h);
        dbg(1, "resetwin(): changing size\n\n");
        xrect[0].x = 0;
        xrect[0].y = 0;
        xrect[0].width = xschem_w;
        xrect[0].height = xschem_h;
        Tk_FreePixmap(display, save_pixmap);
        save_pixmap = Tk_GetPixmap(display, window, xschem_w, xschem_h, depth); /*  20171111 */
        XSetTile(display, gctiled, save_pixmap);
      }
#endif
      #ifdef HAS_CAIRO
      cairo_destroy(save_ctx);
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
      save_ctx = cairo_create(save_sfc);
      cairo_set_line_width(save_ctx, 1);
      cairo_set_line_join(save_ctx, CAIRO_LINE_JOIN_ROUND);
      cairo_set_line_cap(save_ctx, CAIRO_LINE_CAP_ROUND);
      cairo_select_font_face (save_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (save_ctx, 20);
      /* 20171125 select xlib or xcb :-) */
      #if HAS_XCB==1 && HAS_XRENDER==1
      cairo_xcb_surface_set_size(sfc, xschem_w, xschem_h); /*  20171123 */
      #else
      cairo_xlib_surface_set_size(sfc, xschem_w, xschem_h); /*  20171123 */
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
     save_lw = lw_double;
     lw_double=3.0;
   } else {
     lw_double= save_lw;
   }
   only_probes =!only_probes;
   if(only_probes) {
       tclsetvar("only_probes","1");
   }
   else {
       tclsetvar("only_probes","0");
   }
   change_linewidth(lw_double);
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
     if(stat(xschem_executable,&buf)) { /*  20121110 */
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
  if (stat(xschem_executable, &buf)) { /*  20121110 */
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
  return Tcl_GetStringResult(interp);
}

int save(int confirm) /* 20171006 add confirm */
{
     int cancel;
     int save_ok; /*  20171020 */

     save_ok=0;
     cancel=0;
     if(modified)
     {
       if(confirm) {
         tcleval("ask_save");
         if(!strcmp(Tcl_GetStringResult(interp), "") ) cancel=1;
         if(!strcmp(Tcl_GetStringResult(interp), "yes") ) save_ok = save_schematic(schematic[currentsch]);
       } else {
         save_ok = save_schematic(schematic[currentsch]);
       }
     }
     if(save_ok==-1) return 1;
     return cancel;
}

void saveas(const char *f) /*  changed name from ask_save_file to saveas 20121201 */
{
    char name[PATH_MAX+1000];
    char filename[PATH_MAX];
    char res[PATH_MAX];
    char *p;
    if(!f && has_x) {
      my_strncpy(filename , abs_sym_path(schematic[currentsch], ""), S(filename));
      if(current_type == SCHEMATIC) {
        my_snprintf(name, S(name), "save_file_dialog {Save file} .sch.sym INITIALLOADDIR {%s}", filename);
      } else {
        if( (p = strrchr(filename, '.')) && !strcmp(p, ".sch") ) {
          my_strncpy(filename, add_ext(filename, ".sym"), S(filename));
        }
        my_snprintf(name, S(name), "save_file_dialog {Save file} .sym.sch INITIALLOADDIR {%s}", filename);
      }

      tcleval(name);
      my_strncpy(res, Tcl_GetStringResult(interp), S(res));
    }
    else if(f) {
      my_strncpy(res, f, S(res));
    }
    else res[0]='\0';

    if(!res[0]) return; /* 20071104 */
    dbg(1, "saveas(): res = %s\n", res);
    save_schematic(res);
    Tcl_VarEval(interp, "update_recent_file {", res,"}",  NULL);

    my_strncpy(current_name, rel_sym_path(res), S(current_name)); /* 20190519 */
    return;
}
void ask_new_file(void)
{
    char fullname[PATH_MAX]; /*  overflow safe 20161125 */
    
    if(!has_x) return;

    if(modified) { /*  20161209 */
      if(save(1)) return; /*  user cancels save, so do nothing. */
    }
    tcleval("load_file_dialog {Load Schematic} .sch.sym INITIALLOADDIR");
    my_snprintf(fullname, S(fullname),"%s", Tcl_GetStringResult(interp));


    if( fullname[0] ) {
     dbg(1, "ask_new_file(): load file: %s\n", fullname);
     delete_hilight_net();
     currentsch = 0;
     unselect_all(); /* 20180929 */
     remove_symbols();
     load_schematic(1, fullname,1); /* 20180925.1 */
     Tcl_VarEval(interp, "update_recent_file {", fullname, "}", NULL);
     my_strdup(1, &sch_path[currentsch],".");
     sch_inst_number[currentsch] = 1;
     zoom_full(1, 0);
    }
}

/* remove symbol and decrement lastinstdef */
/* Warning: removing a symbol with a loaded schematic will make all symbol references corrupt */
/* you should clear_drawing() first or load_schematic() or link_symbols_to_instances()
   immediately afterwards */
void remove_symbol(int j)
{
  int i,c;
  dbg(1, "remove_symbol(): removing symbol %d\n", j);
  if(instdef[j].prop_ptr != NULL) {
    my_free(666, &instdef[j].prop_ptr);
  }
  /*  20150409 */
  if(instdef[j].templ != NULL) {
    my_free(667, &instdef[j].templ);
  }
  if(instdef[j].type != NULL) {
    my_free(668, &instdef[j].type);
  }
  /*  /20150409 */
  for(c=0;c<cadlayers;c++) /*  20171115 */
  {
   for(i=0;i<instdef[j].polygons[c];i++)
   {
     if(instdef[j].polygonptr[c][i].prop_ptr != NULL) {
       my_free(669, &instdef[j].polygonptr[c][i].prop_ptr);
     }
     my_free(670, &instdef[j].polygonptr[c][i].x);
     my_free(671, &instdef[j].polygonptr[c][i].y);
     my_free(672, &instdef[j].polygonptr[c][i].selected_point);
   }
   my_free(673, &instdef[j].polygonptr[c]);

   for(i=0;i<instdef[j].lines[c];i++)
   {
    if(instdef[j].lineptr[c][i].prop_ptr != NULL)
    {
     my_free(674, &instdef[j].lineptr[c][i].prop_ptr);
    }
   }
   my_free(675, &instdef[j].lineptr[c]);

   for(i=0;i<instdef[j].arcs[c];i++)
   {
    if(instdef[j].arcptr[c][i].prop_ptr != NULL)
    {
     my_free(676, &instdef[j].arcptr[c][i].prop_ptr);
    }
   }
   my_free(677, &instdef[j].arcptr[c]);

   for(i=0;i<instdef[j].rects[c];i++)
   {
    if(instdef[j].boxptr[c][i].prop_ptr != NULL)
    {
     my_free(678, &instdef[j].boxptr[c][i].prop_ptr);
    }
   }
   my_free(679, &instdef[j].boxptr[c]);
  }
  for(i=0;i<instdef[j].texts;i++)
  {
   if(instdef[j].txtptr[i].prop_ptr != NULL)
   {
    my_free(680, &instdef[j].txtptr[i].prop_ptr);
   }
   if(instdef[j].txtptr[i].txt_ptr != NULL)
   {
    my_free(681, &instdef[j].txtptr[i].txt_ptr);
   }
   if(instdef[j].txtptr[i].font != NULL)
   {
    my_free(682, &instdef[j].txtptr[i].font);
   }
  }
  my_free(683, &instdef[j].txtptr);
  my_free(684, &instdef[j].name);

  for(i = j + 1; i < lastinstdef; i++) {
    instdef[i-1] = instdef[i];
  }
  lastinstdef--;
}

void remove_symbols(void)
{
 int j;
 
 for(j=lastinstdef-1;j>=0;j--) {
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
 del_inst_table(); /*  20180917 */
 del_wire_table();   /*  20180917 */
 my_free(685, &schtedaxprop);
 my_free(686, &schsymbolprop);
 my_free(687, &schprop);
 my_free(688, &schvhdlprop);
 my_free(689, &xschem_version_string);
 my_free(690, &schverilogprop); /* 09112003 */
 for(i=0;i<lastwire;i++)
 {
  my_free(691, &wire[i].prop_ptr);
  my_free(692, &wire[i].node);
 } 
 lastwire = 0;
 for(i=0;i<lastinst;i++)
 {
  my_free(693, &inst_ptr[i].prop_ptr);
  my_free(694, &inst_ptr[i].name);
  my_free(695, &inst_ptr[i].instname);
  delete_inst_node(i);
 }
 lastinst = 0;
 for(i=0;i<lasttext;i++)
 {
  my_free(696, &textelement[i].font);
  my_free(697, &textelement[i].prop_ptr);
  my_free(698, &textelement[i].txt_ptr);
 }
 lasttext = 0;
 for(i=0;i<cadlayers;i++)
 {
  for(j=0;j<lastline[i];j++)
  {
   my_free(699, &line[i][j].prop_ptr);
  }
  for(j=0;j<lastrect[i];j++)
  {
   my_free(700, &rect[i][j].prop_ptr);
  }
  for(j=0;j<lastarc[i];j++)
  {
   my_free(701, &arc[i][j].prop_ptr);
  }
  for(j=0;j<lastpolygon[i]; j++) {/*  20171115 */
    my_free(702, &polygon[i][j].x);
    my_free(703, &polygon[i][j].y);
    my_free(704, &polygon[i][j].prop_ptr);
    my_free(705, &polygon[i][j].selected_point); 
  }
  lastline[i] = 0;
  lastarc[i] = 0;
  lastrect[i] = 0;
  lastpolygon[i] = 0;/*  20171115 */
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
  Instdef *symbol;
  int npin, i, j;
  double x0,y0, pinx0, piny0; 
  int flip, rot, rot1 ; /*  20101129  */
  Box *rect;
  char *labname=NULL;
  char *prop=NULL; /*  20161122 overflow safe */
  const char symname_pin[] = "devices/lab_pin.sym";
  const char symname_wire[] = "devices/lab_wire.sym"; /*  20171005 */
  const char symname_pin2[] = "lab_pin.sym";
  const char symname_wire2[] = "lab_wire.sym"; /*  20171005 */
  char *type=NULL;
  int dir;
  int k,ii, skip;
  int do_all_inst=0; /*  20171206 */
  const char *rot_txt;
  int rotated_text=-1; /*  20171208 */

  struct wireentry *wptr; /*  20171214 */
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
    my_strdup(5, &prop, inst_ptr[selectedgroup[j].n].instname);
    my_strcat(6, &prop, "_");
    tclsetvar("custom_label_prefix",prop);
    /*  20171005 */
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
    if(strcmp(rot_txt,"")) rotated_text=atoi(rot_txt); /*  20171208 */

    my_strdup(7, &type,(inst_ptr[selectedgroup[j].n].ptr+instdef)->type); /*  20150409 */
    if( type && (strcmp(type,"label") && strcmp(type,"ipin")&&strcmp(type,"opin")&&strcmp(type,"iopin") )==0) {
      continue;
    }

    if(!do_all_inst && !strcmp(tclgetvar("do_all_inst"),"1")) do_all_inst=1; /*  20171206 */

    dbg(1, " 200711 1--> %s %.16g %.16g   %s\n", 
        inst_ptr[selectedgroup[j].n].name, 
        inst_ptr[selectedgroup[j].n].x0, 
        inst_ptr[selectedgroup[j].n].y0, 
        instdef[inst_ptr[selectedgroup[j].n].ptr].name);

    x0 = inst_ptr[selectedgroup[j].n].x0;
    y0 = inst_ptr[selectedgroup[j].n].y0;
    rot = inst_ptr[selectedgroup[j].n].rot;
    flip = inst_ptr[selectedgroup[j].n].flip;
    symbol = instdef + inst_ptr[selectedgroup[j].n].ptr;
    npin = symbol->rects[PINLAYER];
    rect=symbol->boxptr[PINLAYER];
    
    for(i=0;i<npin;i++) {
       my_strdup(8, &labname,get_tok_value(rect[i].prop_ptr,"name",1));
       dbg(1,"200711 2 --> labname=%s\n", labname);
       
       pinx0 = (rect[i].x1+rect[i].x2)/2;
       piny0 = (rect[i].y1+rect[i].y2)/2;

       if(strcmp(get_tok_value(rect[i].prop_ptr,"dir",0),"in")) dir=1; /*  out or inout pin */
       else dir=0; /*  input pin */

       /*  opin or iopin on left of symbol--> reverse orientation 20171205 */
       if(rotated_text ==-1 && dir==1 && pinx0<0) dir=0;
      
       ROTATION(0.0, 0.0, pinx0, piny0, pinx0, piny0); /*  20101129 */
 
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
         if( touch(wire[wptr->n].x1, wire[wptr->n].y1,
             wire[wptr->n].x2, wire[wptr->n].y2, pinx0, piny0) ) {
           skip=1;
           break;
         }
         wptr = wptr->next;
       }
       if(!skip) {
         my_strdup(9, &prop, "name=p1 lab=");

         /*  20171005 */
         if(use_label_prefix) {
           my_strcat(10, &prop, (char *)tclgetvar("custom_label_prefix"));
         }
         /*  /20171005 */

         my_strcat(11, &prop, labname);
         dir ^= flip; /*  20101129  20111030 */
         if(rotated_text ==-1) {
           rot1=rot;
           if(rot1==1 || rot1==2) { dir=!dir;rot1 = (rot1+2) %4;}  /*  20111103 */
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
 if(current_type==SYMBOL) return 0; /*  20161210 dont allow components placed inside symbols */
 if(symbol_name==NULL) {
   tcleval("load_file_dialog {Choose symbol} .sym INITIALINSTDIR");
   my_strncpy(name, Tcl_GetStringResult(interp), S(name));
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
  if(pos==-1 || pos > lastinst) n=lastinst;
  else
  {
   for(j=lastinst;j>pos;j--)
   {
    inst_ptr[j]=inst_ptr[j-1];
   }
   n=pos;
  }
  /*  03-02-2000 */
  dbg(1, "place_symbol(): checked inst_ptr storage, instdef number i=%d\n", i);
  inst_ptr[n].ptr = i;
  inst_ptr[n].name=NULL;
  inst_ptr[n].instname=NULL; /*  20150409 */
  dbg(1, "place_symbol(): entering my_strdup: name=%s\n",name);  /*  03-02-2000 */
  my_strdup(12, &inst_ptr[n].name ,name);
  dbg(1, "place_symbol(): done my_strdup: name=%s\n",name);  /*  03-02-2000 */
  /*  inst_ptr[n].x0=symbol_name ? x : mousex_snap; */
  /*  inst_ptr[n].y0=symbol_name ? y : mousey_snap; */
  inst_ptr[n].x0= x ; /*  20070228 x and y given in callback */
  inst_ptr[n].y0= y ; /*  20070228 */
  inst_ptr[n].rot=symbol_name ? rot : 0;
  inst_ptr[n].flip=symbol_name ? flip : 0;

  inst_ptr[n].flags=0;
  inst_ptr[n].sel=0;
  inst_ptr[n].node=NULL;
  inst_ptr[n].prop_ptr=NULL;
  dbg(1, "place_symbol() :all inst_ptr members set\n");  /*  03-02-2000 */
  if(first_call) hash_all_names(n);
  if(inst_props) {
    new_prop_string(n, inst_props,!first_call, disable_unique_names); /*  20171214 first_call */
  }
  else {
    set_inst_prop(n); /* no props, get from sym template, also calls new_prop_string() */
  }
  dbg(1, "place_symbol(): done set_inst_prop()\n");  /*  03-02-2000 */

  my_strdup2(13, &inst_ptr[n].instname, get_tok_value(inst_ptr[n].prop_ptr,"name",0) ); /*  20150409 */

  type = instdef[inst_ptr[n].ptr].type; /* 20150409 */
  cond= !type || (strcmp(type,"label") && strcmp(type,"ipin") &&
        strcmp(type,"opin") &&  strcmp(type,"iopin"));
  if(cond) inst_ptr[n].flags|=2;
  else inst_ptr[n].flags &=~2;

  if(first_call && (draw_sym & 3) ) bbox(BEGIN, 0.0 , 0.0 , 0.0 , 0.0);

  symbol_bbox(n, &inst_ptr[n].x1, &inst_ptr[n].y1,
                    &inst_ptr[n].x2, &inst_ptr[n].y2);
  if(draw_sym & 3) bbox(ADD, inst_ptr[n].x1, inst_ptr[n].y1, inst_ptr[n].x2, inst_ptr[n].y2);
  lastinst++;
  set_modify(1);
  prepared_hash_instances=0; /*  20171224 */
  prepared_netlist_structs=0;
  prepared_hilight_structs=0;
  if(draw_sym&1) {
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
    draw();
    bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  }
  /*   hilight new element 24122002 */

  if(draw_sym & 4 ) {
    drawtempline(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
    drawtemprect(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
    drawtemparc(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
    select_element(n, SELECTED,0, 0);
    ui_state |= SELECTION;
    drawtemparc(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
    drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
    drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
    need_rebuild_selected_array = 1; /* 20191127 */
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
  new_window(schematic[currentsch],1);
 }
 else
 {
  new_window(abs_sym_path(inst_ptr[selectedgroup[0].n].name, ""),1);
 }

}


void schematic_in_new_window(void)
{
 char filename[PATH_MAX];
 rebuild_selected_array();
 if(lastselected !=1 || selectedgroup[0].type!=ELEMENT) 
 {
  /*  new_window("", 0); */
  new_window(schematic[currentsch], 0); /*  20111007 duplicate current schematic if no inst selected */
  return;
 }
 else
 {
  if(                   /*  do not descend if not subcircuit */
     (inst_ptr[selectedgroup[0].n].ptr+instdef)->type && 
     strcmp(
        (inst_ptr[selectedgroup[0].n].ptr+instdef)->type, /*  20150409 */
         "subcircuit"
     ) && 
     strcmp(
        (inst_ptr[selectedgroup[0].n].ptr+instdef)->type, /*  20150409 */
         "primitive"
     ) 
  ) return;

  my_strncpy(filename, abs_sym_path(get_tok_value(
    (inst_ptr[selectedgroup[0].n].ptr+instdef)->prop_ptr, "schematic",0 ), "")
    , S(filename));
  if(!filename[0]) {
    my_strncpy(filename, add_ext(abs_sym_path(inst_ptr[selectedgroup[0].n].name, ""), ".sch"), S(filename));
  }

  new_window(filename, 0);
 }
}

void launcher(void) /*  20161102 */
{
  const char *str;
  char program[PATH_MAX];
  int n;
  rebuild_selected_array();
  if(lastselected ==1 && selectedgroup[0].type==ELEMENT) 
  {
    n=selectedgroup[0].n;
    my_strncpy(program, get_tok_value(inst_ptr[n].prop_ptr,"program",2), S(program)); /*  20170414 handle backslashes */
    str = get_tok_value(inst_ptr[n].prop_ptr,"url",2); /*  20170414 handle backslashes */
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
      my_strncpy(program, get_tok_value(inst_ptr[n].prop_ptr,"tclcommand",2), S(program)); /*  20170415 */
      if(program[0]) { /*  20170415 execute tcl command */
        tcleval(program);
      }
    }
  } 
}

void descend_schematic(void)
{
 const char *str;
 char filename[PATH_MAX];
 int inst_mult, inst_number;
 int save_ok = 0;

 if(!(current_type==SCHEMATIC)) return; /*20180928 */

 rebuild_selected_array();
 if(lastselected !=1 || selectedgroup[0].type!=ELEMENT) 
 {
  dbg(1, "descend_schematic(): wrong selection\n");
  return;
 }
 else
 {
  dbg(1, "descend_schematic(): selected:%s\n", inst_ptr[selectedgroup[0].n].name);
  /* no name set for current schematic: save it before descending*/
  if(!strcmp(schematic[currentsch],""))
  {
    char cmd[PATH_MAX+1000];
    char filename[PATH_MAX];
    char res[PATH_MAX];

    my_strncpy(filename, schematic[currentsch], S(filename));
    my_snprintf(cmd, S(cmd), "save_file_dialog {Save file} .sch.sym INITIALLOADDIR {%s}", filename);
    tcleval(cmd);
    my_strncpy(res, Tcl_GetStringResult(interp), S(res));
    if(!res[0]) return; /* 20071104 */
    dbg(1, "descend_schematic(): saving: %s\n",res);
    save_ok = save_schematic(res);
    if(save_ok==-1) return; /*  20171020 */
  }

  dbg(1, "type of instance: %s\n", (inst_ptr[selectedgroup[0].n].ptr+instdef)->type);

  if(                   /*  do not descend if not subcircuit */
     (inst_ptr[selectedgroup[0].n].ptr+instdef)->type && 
     strcmp(
        (inst_ptr[selectedgroup[0].n].ptr+instdef)->type, /*  20150409 */
         "subcircuit"
     ) && 
     strcmp(
        (inst_ptr[selectedgroup[0].n].ptr+instdef)->type, /*  20150409 */
         "primitive"
     ) 
  ) return;

  if(modified)
  {
    if(save(1)) return; /*  20161209 */
  }

  /*  build up current hierarchy path */
  dbg(1, "descend_schematic(): selected instname=%s\n", inst_ptr[selectedgroup[0].n].instname);


  if(inst_ptr[selectedgroup[0].n].instname && inst_ptr[selectedgroup[0].n].instname[0]) {
    str=expandlabel(inst_ptr[selectedgroup[0].n].instname, &inst_mult); /*  20150409 */
  } else {
    str = "";
    inst_mult = 1;
  }
  my_strdup(14, &sch_path[currentsch+1], sch_path[currentsch]);

  inst_number = 1;
  if(inst_mult > 1) { /* on multiple instances ask where to descend, to correctly evaluate 
                         the hierarchy path you descend to */
    const char *inum;
    Tcl_VarEval(interp, "input_line ", "{input instance number (leftmost = 1) to descend into:\n" 
      "negative numbers select instance starting\nfrom the right (rightmost = -1)}"
      " {} 1 6", NULL);
    inum = Tcl_GetStringResult(interp);
    dbg(1, "descend_schematic(): inum=%s\n", inum);
    if(!inum[0]) {
      my_free(710, &sch_path[currentsch+1]);
      return;
    }
    inst_number=atoi(inum);
    if(inst_number < 0 ) inst_number += inst_mult+1;
    if(inst_number <1 || inst_number > inst_mult) inst_number = 1; /* any invalid number->descend to leftmost inst */
  }
  dbg(1," inst_number=%d\n", inst_number);
  my_strcat(15, &sch_path[currentsch+1], find_nth(str, ',', inst_number));
  dbg(1," inst_number=%d\n", inst_number);
  my_strcat(16, &sch_path[currentsch+1], ".");
  sch_inst_number[currentsch+1] = inst_number;
  dbg(1, "descend_schematic(): current path: %s\n", sch_path[currentsch+1]);
  dbg(1, "descend_schematic(): inst_number=%d\n", inst_number);

  previous_instance[currentsch]=selectedgroup[0].n;
  zoom_array[currentsch].x=xorigin;
  zoom_array[currentsch].y=yorigin;
  zoom_array[currentsch].zoom=zoom;
  currentsch++;
  hilight_child_pins();

  my_strncpy(filename, abs_sym_path(get_tok_value(
     (inst_ptr[selectedgroup[0].n].ptr+instdef)->prop_ptr, "schematic",0 ), "")
     , S(filename));

  unselect_all();
  remove_symbols();
  if(filename[0]) {
    load_schematic(1,filename, 1);
  } else {
    my_strncpy(filename, add_ext(abs_sym_path(inst_ptr[selectedgroup[0].n].name, ""), ".sch"), S(filename));
    load_schematic(1, filename, 1);
  }
  if(hilight_nets) 
  {
    prepare_netlist_structs(0);
    if(enable_drill) drill_hilight(); /*  20171212 */
  }
  dbg(1, "descend_schematic(): before zoom(): prepared_hash_instances=%d\n", prepared_hash_instances);
  zoom_full(1, 0);
 }
}

void go_back(int confirm) /*  20171006 add confirm */
{
 int prev_curr_type=0;
 int save_ok;  /*  20171020 */
 int from_embedded_sym;
 int save_modified;
 char filename[PATH_MAX];

 save_ok=0;
 if(currentsch>0)
 {
  prev_curr_type=current_type; /* 20190521 */
  /* if current sym/schematic is changed ask save before going up */
  if(modified)
  {
    if(confirm) {
      tcleval("ask_save");
      if(!strcmp(Tcl_GetStringResult(interp), "yes") ) save_ok = save_schematic(schematic[currentsch]);
      else if(!strcmp(Tcl_GetStringResult(interp), "") ) return;
    } else {
      save_ok = save_schematic(schematic[currentsch]);
    }
  }
  if(save_ok==-1) return; /*  20171020 */
  unselect_all();
  remove_symbols();
  from_embedded_sym=0;
  if(strstr(schematic[currentsch], ".xschem_embedded_")) {
    /* when returning after editing an embedded symbol
     * load immediately symbol definition before going back (.xschem_embedded... file will be lost) 
     */
    load_sym_def(schematic[currentsch], NULL);
    from_embedded_sym=1;
  }
  my_strncpy(schematic[currentsch] , "", S(schematic[currentsch]));
  currentsch--;
  save_modified = modified; /* we propagate modified flag (cleared by load_schematic */
                            /* by default) to parent schematic if going back from embedded symbol */

  my_strncpy(filename, abs_sym_path(schematic[currentsch], ""), S(filename));
  load_schematic(1, filename, 1);
  if(from_embedded_sym) modified=save_modified; /* to force ask save embedded sym in parent schematic */

  if(prev_curr_type==SCHEMATIC) {
    hilight_parent_pins();
  }
  if(enable_drill) drill_hilight(); /*  20171212 */
  xorigin=zoom_array[currentsch].x;
  yorigin=zoom_array[currentsch].y;
  zoom=zoom_array[currentsch].zoom;
  mooz=1/zoom;

  change_linewidth(-1.);
  draw();

  current_type=SCHEMATIC;
  dbg(1, "go_back(): current path: %s\n", sch_path[currentsch]);
 }
}

void change_linewidth(double w)
{
  int i, changed;
 
  changed=0;
  /* choose line width automatically based on zoom */
  if(w<0.) {
    if(change_lw)  {
      lw_double=mooz * 0.09 * cadsnap;
      bus_width = BUS_WIDTH * mooz * 0.09 * cadsnap;
      cadhalfdotsize = CADHALFDOTSIZE * 0.1 * cadsnap;
      changed=1;
    }
  /* explicitly set line width */
  } else {
    lw_double=w;
    bus_width = BUS_WIDTH * w;
    changed=1;
  }
  if(!changed) return;
  lw=lw_double;
  if(lw==0) lw=1;     /*  on some servers zero width */
                      /*  draws fast but not good... */
  if(has_x) {
    for(i=0;i<cadlayers;i++) {
        XSetLineAttributes (display, gc[i], lw, LineSolid, CapRound , JoinRound);
    }
    XSetLineAttributes (display, gctiled, lw, LineSolid, CapRound , JoinRound);
  }
  areax1 = -2*lw;
  areay1 = -2*lw;
  areax2 = xrect[0].width+2*lw;
  areay2 = xrect[0].height+2*lw;
  areaw = areax2-areax1;
  areah = areay2 - areay1;
}

void calc_drawing_bbox(Box *boundbox, int selected)
{

 Box tmp;
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
  for(i=0;i<lastline[c];i++)
  {
   if(selected == 1 && !line[c][i].sel) continue;
   if(selected == 2) continue;
   tmp.x1=line[c][i].x1;
   tmp.x2=line[c][i].x2;
   tmp.y1=line[c][i].y1;
   tmp.y2=line[c][i].y2;
   count++;
   updatebbox(count,boundbox,&tmp);
  }

  for(i=0;i<lastpolygon[c];i++) /*  20171115 */
  {
    double x1=0., y1=0., x2=0., y2=0.;
    int k;
    if(selected == 1 && !polygon[c][i].sel) continue;
    if(selected == 2) continue;
    count++;
    for(k=0; k<polygon[c][i].points; k++) {
      /* fprintf(errfp, "  poly: point %d: %.16g %.16g\n", k, pp[c][i].x[k], pp[c][i].y[k]); */
      if(k==0 || polygon[c][i].x[k] < x1) x1 = polygon[c][i].x[k];
      if(k==0 || polygon[c][i].y[k] < y1) y1 = polygon[c][i].y[k];
      if(k==0 || polygon[c][i].x[k] > x2) x2 = polygon[c][i].x[k];
      if(k==0 || polygon[c][i].y[k] > y2) y2 = polygon[c][i].y[k];
    }
    tmp.x1=x1;tmp.y1=y1;tmp.x2=x2;tmp.y2=y2;
    updatebbox(count,boundbox,&tmp);
  }

  for(i=0;i<lastarc[c];i++)
  {
    if(selected == 1 && !arc[c][i].sel) continue;
    if(selected == 2) continue;
    arc_bbox(arc[c][i].x, arc[c][i].y, arc[c][i].r, arc[c][i].a, arc[c][i].b,
             &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
    count++;
    updatebbox(count,boundbox,&tmp);
  }

  for(i=0;i<lastrect[c];i++)
  {
   if(selected == 1 && !rect[c][i].sel) continue;
   if(selected == 2) continue;
   tmp.x1=rect[c][i].x1;
   tmp.x2=rect[c][i].x2;
   tmp.y1=rect[c][i].y1;
   tmp.y2=rect[c][i].y2;
   count++;
   updatebbox(count,boundbox,&tmp);
  }
 }
 for(i=0;i<lastwire;i++)
 { 
   if(selected == 1 && !wire[i].sel) continue;
   if(selected == 2) {
     const char *str;
     str = get_tok_value(wire[i].prop_ptr, "lab",0);
     if(!str[0] || !bus_hilight_lookup(str, 0,XLOOKUP)) continue;
   }
   tmp.x1=wire[i].x1;
   tmp.x2=wire[i].x2;
   tmp.y1=wire[i].y1;
   tmp.y2=wire[i].y2;
   count++;
   updatebbox(count,boundbox,&tmp);
 }
 if(has_x) for(i=0;i<lasttext;i++)
 {
   if(selected == 1 && !textelement[i].sel) continue;
   if(selected == 2) continue;
   #ifdef HAS_CAIRO
   customfont = set_text_custom_font(&textelement[i]);
   #endif
   if(text_bbox(textelement[i].txt_ptr, textelement[i].xscale,
         textelement[i].yscale,textelement[i].rot, textelement[i].flip,
         textelement[i].x0, textelement[i].y0,
         &tmp.x1,&tmp.y1, &tmp.x2,&tmp.y2) ) {
     count++;
     updatebbox(count,boundbox,&tmp);
   }
   #ifdef HAS_CAIRO
   if(customfont) cairo_restore(ctx);
   #endif
 }
 for(i=0;i<lastinst;i++)
 {
  char *type;
  struct hilight_hashentry *entry;

  if(selected == 1 && !inst_ptr[i].sel) continue;
  
  if(selected == 2) {
    int j, rects, found, hilight_connected_inst;
    type = (inst_ptr[i].ptr+instdef)->type; /* 20150409 */
    found = 0;
    hilight_connected_inst = !strcmp(get_tok_value((inst_ptr[i].ptr+instdef)->prop_ptr, "highlight", 0), "true");
    if( hilight_connected_inst && (rects = (inst_ptr[i].ptr+instdef)->rects[PINLAYER]) > 0 ) {
      prepare_netlist_structs(0);
      for(j=0;j<rects;j++) {
        if( inst_ptr[i].node && inst_ptr[i].node[j]) {
          entry=bus_hilight_lookup(inst_ptr[i].node[j], 0, XLOOKUP);
          if(entry) {
            found = 1;
            break;
          }
        }
      }
    }
    else if( type && !(strcmp(type,"label") && strcmp(type,"ipin") && strcmp(type,"iopin") && strcmp(type,"opin") )) {
      entry=bus_hilight_lookup( get_tok_value(inst_ptr[i].prop_ptr,"lab",0) , 0, XLOOKUP );
      if(entry) found = 1;
    }
    else if( (inst_ptr[i].flags & 4) ) {
      found = 1;
    }
    if(!found) continue;
  }




  /* cpu hog 20171206 */
  /*  symbol_bbox(i, &inst_ptr[i].x1, &inst_ptr[i].y1, &inst_ptr[i].x2, &inst_ptr[i].y2); */
  tmp.x1=inst_ptr[i].x1;
  tmp.y1=inst_ptr[i].y1;
  tmp.x2=inst_ptr[i].x2;
  tmp.y2=inst_ptr[i].y2;
  count++;
  updatebbox(count,boundbox,&tmp);
 }

}

void zoom_full(int dr, int sel)
{
  Box boundbox;
  double yy1;

  if(change_lw) {
    lw = lw_double=1.;
    bus_width = BUS_WIDTH * lw_double;
  }
  areax1 = -2*lw;
  areay1 = -2*lw;
  areax2 = xrect[0].width+2*lw;
  areay2 = xrect[0].height+2*lw;
  areaw = areax2-areax1;
  areah = areay2 - areay1;

  calc_drawing_bbox(&boundbox, sel);
  zoom=(boundbox.x2-boundbox.x1)/(areaw-4*lw);
  yy1=(boundbox.y2-boundbox.y1)/(areah-4*lw);
  if(yy1>zoom) zoom=yy1;
  zoom*=1.05;
  mooz=1/zoom;
  xorigin=-boundbox.x1+(areaw-4*lw)/40*zoom;
  yorigin=(areah-4*lw)*zoom-boundbox.y2 - (areah-4*lw)/40*zoom;
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
    if(zoom<CADMINZOOM) return;
    zoom/= factor;
    mooz=1/zoom;
    xorigin=-mousex_snap+(mousex_snap+xorigin)/factor;
    yorigin=-mousey_snap+(mousey_snap+yorigin)/factor;
    change_linewidth(-1.);
    draw();
}

void view_unzoom(double z)
{
  double factor;
  /*  int i; */
  factor = z!=0.0 ? z : CADZOOMSTEP;
  if(zoom>CADMAXZOOM) return;
  zoom*= factor;
  mooz=1/zoom;
  /* 20181022 make unzoom and zoom symmetric  */
  /* keeping the mouse pointer as the origin */
  if(unzoom_nodrift) {
    xorigin=-mousex_snap+(mousex_snap+xorigin)*factor;
    yorigin=-mousey_snap+(mousey_snap+yorigin)*factor;
  } else {
    xorigin=xorigin+areaw*zoom*(1-1/factor)/2;
    yorigin=yorigin+areah*zoom*(1-1/factor)/2;
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
    xorigin=-x1;yorigin=-y1;
    zoom=(x2-x1)/(areaw-4*lw);
    yy1=(y2-y1)/(areah-4*lw);
    if(yy1>zoom) zoom=yy1;
    mooz=1/zoom;
    change_linewidth(-1.);
    draw();
    dbg(1, "zoom_box(): coord: %.16g %.16g %.16g %.16g zoom=%.16g\n",x1,y1,mousex_snap, mousey_snap,zoom);
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
     w=(float)(areaw*zoom/800) * rand() / (RAND_MAX+1.0);
     h=(float)(areah*zoom/80) * rand() / (RAND_MAX+1.0);
     x1=(float)(areaw*zoom) * rand() / (RAND_MAX+1.0)-xorigin;
     y1=(float)(areah*zoom) * rand() / (RAND_MAX+1.0)-yorigin;
     x2=x1+w;
     y2=y1+h;
     ORDER(x1,y1,x2,y2);
     rectcolor = (int) (16.0*rand()/(RAND_MAX+1.0))+4;
     storeobject(-1, x1,y1,x2,y2, xRECT,rectcolor, 0, NULL);
   }

   for(i=0;i<=210000;i++)
    {
     w=(float)(areaw*zoom/80) * rand() / (RAND_MAX+1.0);
     h=(float)(areah*zoom/800) * rand() / (RAND_MAX+1.0);
     x1=(float)(areaw*zoom) * rand() / (RAND_MAX+1.0)-xorigin;
     y1=(float)(areah*zoom) * rand() / (RAND_MAX+1.0)-yorigin;
     x2=x1+w;
     y2=y1+h;
     ORDER(x1,y1,x2,y2);
     rectcolor = (int) (16.0*rand()/(RAND_MAX+1.0))+4;
     storeobject(-1, x1,y1,x2,y2,xRECT,rectcolor, 0, NULL);
   }

   for(i=0;i<=210000;i++)
   {
     w=(float)zoom * rand() / (RAND_MAX+1.0);
     h=w;
     x1=(float)(areaw*zoom) * rand() / (RAND_MAX+1.0)-xorigin;
     y1=(float)(areah*zoom) * rand() / (RAND_MAX+1.0)-yorigin;
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
           drawline(WIRELAYER,NOW, xx1,yy1,xx2,yy1);
         }
         if(yy2!=yy1) {
           xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
           ORDER(xx2,yy1,xx2,yy2);
           storeobject(-1, xx2,yy1,xx2,yy2,WIRE,0,0,NULL);
           drawline(WIRELAYER,NOW, xx2,yy1,xx2,yy2);
         }
       } else if(manhattan_lines==2) {
         if(yy2!=yy1) {
           xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
           ORDER(xx1,yy1,xx1,yy2);
           storeobject(-1, xx1,yy1,xx1,yy2,WIRE,0,0,NULL);
           drawline(WIRELAYER,NOW, xx1,yy1,xx1,yy2);
         }
         if(xx2!=xx1) {
           xx1=x1;yy1=y1;xx2=x2;yy2=y2;
           ORDER(xx1,yy2,xx2,yy2);
           storeobject(-1, xx1,yy2,xx2,yy2,WIRE,0,0,NULL);
           drawline(WIRELAYER,NOW, xx1,yy2,xx2,yy2);
         }
       } else {
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy1,xx2,yy2);
         storeobject(-1, xx1,yy1,xx2,yy2,WIRE,0,0,NULL);
         drawline(WIRELAYER,NOW, xx1,yy1,xx2,yy2);
       }
       if(event_reporting) {
         printf("xschem wire %g %g %g %g %d\n", xx1, yy1, xx2, yy2, -1);
         fflush(stdout);
       }
 
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
     update_conn_cues(1,1);
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


   if(lastselected) push_undo(); /*  20151204 */
   for(k=0;k<lastselected;k++)
   {
     n=selectedgroup[k].n;
     type=selectedgroup[k].type;
     c=selectedgroup[k].col;
     if(type==LINE && line[c][n].sel==SELECTED) {
       x1 = line[c][n].x1;
       y1 = line[c][n].y1;
       x2 = line[c][n].x2;
       y2 = line[c][n].y2;
       storeobject(-1, x1,y1,x2,y2,LINE,rectcolor, 0, line[c][n].prop_ptr);
     }
     if(type==ARC && arc[c][n].sel==SELECTED) {
       x1 = arc[c][n].x;
       y1 = arc[c][n].y;
       r = arc[c][n].r;
       a = arc[c][n].a;
       b = arc[c][n].b;
       store_arc(-1, x1, y1, r, a, b, rectcolor, 0, arc[c][n].prop_ptr);
     }
     if(type==POLYGON && polygon[c][n].sel==SELECTED) {
        store_polygon(-1, polygon[c][n].x, polygon[c][n].y, polygon[c][n].points, rectcolor, 0, polygon[c][n].prop_ptr);
     }
     else if(type==xRECT && rect[c][n].sel==SELECTED) {
       x1 = rect[c][n].x1;
       y1 = rect[c][n].y1;
       x2 = rect[c][n].x2;
       y2 = rect[c][n].y2;
       storeobject(-1, x1,y1,x2,y2,xRECT,rectcolor, 0, rect[c][n].prop_ptr);
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
        drawarc(rectcolor, NOW, x, y, r, a, b, 0);
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
           drawline(rectcolor,NOW, xx1,yy1,xx2,yy1);
         }
         if(yy2!=yy1) {
           xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
           ORDER(xx2,yy1,xx2,yy2);
           storeobject(-1, xx2,yy1,xx2,yy2,LINE,rectcolor,0,NULL);
           drawline(rectcolor,NOW, xx2,yy1,xx2,yy2);
         }
       } else if(manhattan_lines==2) {
         if(yy2!=yy1) {
           xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
           ORDER(xx1,yy1,xx1,yy2);
           storeobject(-1, xx1,yy1,xx1,yy2,LINE,rectcolor,0,NULL);
           drawline(rectcolor,NOW, xx1,yy1,xx1,yy2);
         }
         if(xx2!=xx1) {
           xx1=x1;yy1=y1;xx2=x2;yy2=y2;
           ORDER(xx1,yy2,xx2,yy2);
           storeobject(-1, xx1,yy2,xx2,yy2,LINE,rectcolor,0,NULL);
           drawline(rectcolor,NOW, xx1,yy2,xx2,yy2);
         }
       } else {
         xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
         ORDER(xx1,yy1,xx2,yy2);
         storeobject(-1, xx1,yy1,xx2,yy2,LINE,rectcolor,0,NULL);
         drawline(rectcolor,NOW, xx1,yy1,xx2,yy2);
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
     drawrect(rectcolor, NOW, x1,y1,x2,y2);
     save_draw = draw_window;
     draw_window = 1; /* 20181009 */
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


void new_polygon(int what) /*  20171115 */
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
     store_polygon(-1, x, y, points, rectcolor, 0, NULL);
     /* fprintf(errfp, "new_poly: finish: points=%d\n", points); */
     drawtemppolygon(gc[rectcolor], NOW, x, y, points);
     ui_state &= ~STARTPOLYGON;
     drawpolygon(rectcolor, NOW, x, y, points, 0); /*  20180914 added fill param */
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
int text_bbox(char *str, double xscale, double yscale,
    int rot, int flip, double x1,double y1, double *rx1, double *ry1,
    double *rx2, double *ry2)
{
  int c=0, length =0;
  char *str_ptr;
  double size;
  cairo_text_extents_t ext;
  cairo_font_extents_t fext;
  double ww, hh;

  if(!has_x) return 0;
  size = xscale*52.*cairo_font_scale;

  /*  if(size*mooz>800.) { */
  /*    return 0; */
  /*  } */
  cairo_set_font_size (ctx, size*mooz);
  cairo_font_extents(ctx, &fext);

  ww=0.; hh=1.;
  c=0;
  cairo_longest_line=0;
  cairo_lines=1;
  str_ptr = str;
  while( str && str[c] ) {
    if(str[c] == '\n') {
      str[c]='\0';
      hh++;
      cairo_lines++;
      length=0;
      if(str_ptr[0]!='\0') {
        cairo_text_extents(ctx, str_ptr, &ext);
        if(ext.x_advance > ww) ww= ext.x_advance;
      }
      str[c]='\n';
      str_ptr = str+c+1;
    } else {
      length++;
    }
    if(length > cairo_longest_line) {
      cairo_longest_line = length;
    }
    c++;
  }
  if(str_ptr && str_ptr[0]!='\0') {
    cairo_text_extents(ctx, str_ptr, &ext);
    if(ext.x_advance > ww) ww= ext.x_advance;
  }
  hh = hh*fext.height*cairo_font_line_spacing;
  cairo_longest_line = ww;

  *rx1=x1;*ry1=y1; 
  ROTATION(0.0,0.0, ww*zoom,hh*zoom,(*rx2),(*ry2));
  *rx2+=*rx1;*ry2+=*ry1;
  if     (rot==0) {*ry1-=cairo_vert_correct; *ry2-=cairo_vert_correct;}
  else if(rot==1) {*rx1+=cairo_vert_correct; *rx2+=cairo_vert_correct;}
  else if(rot==2) {*ry1+=cairo_vert_correct; *ry2+=cairo_vert_correct;}
  else if(rot==3) {*rx1-=cairo_vert_correct; *rx2-=cairo_vert_correct;}
  RECTORDER((*rx1),(*ry1),(*rx2),(*ry2));
  return 1;
}
int text_bbox_nocairo(char * str,double xscale, double yscale,
    int rot, int flip, double x1,double y1, double *rx1, double *ry1,
    double *rx2, double *ry2)
#else
int text_bbox(char * str,double xscale, double yscale,
    int rot, int flip, double x1,double y1, double *rx1, double *ry1,
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

  ROTATION(0.0,0.0,w,h,(*rx2),(*ry2));
  *rx2+=*rx1;*ry2+=*ry1;
  RECTORDER((*rx1),(*ry1),(*rx2),(*ry2)); 
  return 1;
}

void place_text(int draw_text, double mx, double my)
{
  char *txt;
  int textlayer;
  const char *strlayer;
  int save_draw;

  /*  20171112 */
  #ifdef HAS_CAIRO
  char  *textfont;
  #endif

  tclsetvar("props","");
  tclsetvar("txt","");

  if(tclgetvar("hsize")==NULL)
   tclsetvar("hsize","0.4");
  if(tclgetvar("vsize")==NULL)
   tclsetvar("vsize","0.4");
  tcleval("enter_text {text:}");

  dbg(1, "place_text(): hsize=%s vsize=%s\n",tclgetvar("hsize"),
           tclgetvar("vsize") );
  
  txt =  (char *)tclgetvar("txt");
  if(!strcmp(txt,"")) return;   /*  01112004 dont allocate text object if empty string given */
  push_undo(); /*  20150327 */
  check_text_storage();
  textelement[lasttext].txt_ptr=NULL;
  textelement[lasttext].prop_ptr=NULL;  /*  20111006 added missing initialization of pointer */
  textelement[lasttext].font=NULL;  /*  20171206 */
  my_strdup(19, &textelement[lasttext].txt_ptr, txt);
  textelement[lasttext].x0=mx;
  textelement[lasttext].y0=my;
  textelement[lasttext].rot=0;
  textelement[lasttext].flip=0;
  textelement[lasttext].sel=0;
  textelement[lasttext].xscale=
   atof(tclgetvar("hsize"));
  textelement[lasttext].yscale=
   atof(tclgetvar("vsize"));
   my_strdup(20, &textelement[lasttext].prop_ptr, (char *)tclgetvar("props"));
  /*  debug ... */
  /*  textelement[lasttext].prop_ptr=NULL; */
  dbg(1, "place_text(): done text input\n");
  strlayer = get_tok_value(textelement[lasttext].prop_ptr, "layer", 0);
  if(strlayer[0]) textelement[lasttext].layer = atoi(strlayer);
  else textelement[lasttext].layer = -1;
  my_strdup(21, &textelement[lasttext].font, get_tok_value(textelement[lasttext].prop_ptr, "font", 0));/* 20171206 */
  textlayer = TEXTLAYER;
  #ifdef HAS_CAIRO
  textlayer = textelement[lasttext].layer;
  if(textlayer < 0 || textlayer >= cadlayers) textlayer = TEXTLAYER;
  textfont = textelement[lasttext].font;
  if(textfont && textfont[0]) {
    cairo_save(ctx);
    cairo_save(save_ctx);
    cairo_select_font_face (ctx, textfont, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_select_font_face (save_ctx, textfont, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  }
  #endif
  save_draw=draw_window; /* 20181009 */
  draw_window=1;
  if(draw_text) draw_string(textlayer, NOW, textelement[lasttext].txt_ptr, 0, 0, 
              textelement[lasttext].x0,textelement[lasttext].y0,
              textelement[lasttext].xscale, textelement[lasttext].yscale);
  draw_window = save_draw;
  #ifdef HAS_CAIRO
  if(textfont && textfont[0]) {
    cairo_select_font_face (ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_select_font_face (save_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_restore(ctx);
    cairo_restore(save_ctx);
  }
  #endif
  drawtempline(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
  drawtemprect(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
  select_text(lasttext, SELECTED, 0);
  drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
  drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
  lasttext++;
  set_modify(1);
}

void pan2(int what, int mx, int my) /*  20121123 */
{
  int dx, dy, ddx, ddy;
  static int mx_save, my_save;
  static int mmx_save, mmy_save;
  static double xorig_save, yorig_save;
  if(what & BEGIN) {
    mmx_save = mx_save = mx; 
    mmy_save = my_save = my; 
    xorig_save = xorigin;
    yorig_save = yorigin;
  }
  else if(what == RUBBER) {
    dx = mx - mx_save;
    dy = my - my_save;
    ddx = abs(mx -mmx_save);
    ddy = abs(my -mmy_save);
    if(ddx>5 || ddy>5) {
      xorigin = xorig_save + dx*zoom;
      yorigin = yorig_save + dy*zoom;
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
    xorigin+=-xpan+mousex_snap;yorigin+=-ypan+mousey_snap;
    draw();
 }
}

/*  20150927 select=1: select objects, select=0: unselect objects */
void select_rect(int what, int select)
{
 static double xr,yr,xr2,yr2;
 static double xx1,xx2,yy1,yy2;
 static int sel;
 static int sem=0; /*  20171130 */

 if(what & RUBBER)
 {
    if(sem==0) {
      fprintf(errfp, "ERROR: select_rect() RUBBER called before BEGIN\n");
      tcleval("alert_ {ERROR: select_rect() RUBBER called before BEGIN} {}"); /*  20171020 */
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
    sel = select; /*  20150927 */
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

    /*  20171219 */
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

