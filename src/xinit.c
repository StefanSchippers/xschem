/* File: xinit.c
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
#ifdef __unix__
#include <pwd.h> /* getpwuid */
#endif
/* max number of windows (including main) a single xschem process can handle */
#define MAX_NEW_WINDOWS 20

static int init_done=0; /* 20150409 to avoid double call by Xwindows close and TclExitHandler */
static XSetWindowAttributes winattr;
static Tk_Window  tkwindow, mainwindow;
static XWMHints *hints_ptr;
static Window topwindow;
static XColor xcolor_exact,xcolor;
typedef int myproc(
             ClientData clientData,
             Tcl_Interp *interp,
             int argc,
             const char *argv[]);

/* ----------------------------------------------------------------------- */
/* EWMH message handling routines 20071027... borrowed from wmctrl code */
/* ----------------------------------------------------------------------- */
#define _NET_WM_STATE_REMOVE        0    /* remove/unset property */
#define _NET_WM_STATE_ADD           1    /* add/set property */
#define _NET_WM_STATE_TOGGLE        2    /* toggle property  */

static int client_msg(Display *disp, Window win, char *msg, /* {{{ */
        unsigned long data0, unsigned long data1,
        unsigned long data2, unsigned long data3,
        unsigned long data4) {
    XEvent event;
    long mask = SubstructureRedirectMask | SubstructureNotifyMask;

    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = XInternAtom(disp, msg, False);
    event.xclient.window = win;
    event.xclient.format = 32;
    event.xclient.data.l[0] = data0;
    event.xclient.data.l[1] = data1;
    event.xclient.data.l[2] = data2;
    event.xclient.data.l[3] = data3;
    event.xclient.data.l[4] = data4;

    if (XSendEvent(disp, DefaultRootWindow(disp), False, mask, &event)) {
        return EXIT_SUCCESS;
    }
    else {
        fprintf(errfp, "Cannot send %s event.\n", msg);
        return EXIT_FAILURE;
    }
}/*}}}*/


int window_state (Display *disp, Window win, char *arg) {/*{{{*/
    static char *arg_copy=NULL;
    unsigned long action;
    int i;
    Atom prop1 = 0;
    Atom prop2 = 0;
    char *p1, *p2;
    const char *argerr = "expects a list of comma separated parameters: "
                         "\"(remove|add|toggle),<PROP1>[,<PROP2>]\"\n";


    my_strdup(604, &arg_copy, arg);

    dbg(1,"window_state() , win=0x%x arg_copy=%s\n",
          (int)win,arg_copy);
    if (!arg_copy || strlen(arg_copy) == 0) {
        fputs(argerr, errfp);
        return EXIT_FAILURE;
    }

    if ((p1 = strchr(arg_copy, ','))) {
        static char tmp_prop1[1024], tmp1[1024]; /* overflow safe 20161122 */

        *p1 = '\0';

        /* action */
        if (strcmp(arg_copy, "remove") == 0) {
            action = _NET_WM_STATE_REMOVE;
        }
        else if (strcmp(arg_copy, "add") == 0) {
            action = _NET_WM_STATE_ADD;
        }
        else if (strcmp(arg_copy, "toggle") == 0) {
            action = _NET_WM_STATE_TOGGLE;
        }
        else {
            fputs("Invalid action. Use either remove, add or toggle.\n", errfp);
            return EXIT_FAILURE;
        }
        p1++;

        /* the second property */
        if ((p2 = strchr(p1, ','))) {
            static char tmp_prop2[1024], tmp2[1024]; /* overflow safe */
            *p2 = '\0';
            p2++;
            if (strlen(p2) == 0) {
                fputs("Invalid zero length property.\n", errfp);
                return EXIT_FAILURE;
            }
            for( i = 0; p2[i]; i++) tmp2[i] = toupper( p2[i] );
            my_snprintf(tmp_prop2, S(tmp_prop2), "_NET_WM_STATE_%s", tmp2);
            prop2 = XInternAtom(disp, tmp_prop2, False);
        }

        /* the first property */
        if (strlen(p1) == 0) {
            fputs("Invalid zero length property.\n", errfp);
            return EXIT_FAILURE;
        }
        for( i = 0; p1[i]; i++) tmp1[i] = toupper( p1[i] );
        my_snprintf(tmp_prop1, S(tmp_prop1), "_NET_WM_STATE_%s", tmp1);
        prop1 = XInternAtom(disp, tmp_prop1, False);


        return client_msg(disp, win, "_NET_WM_STATE",
            action, (unsigned long)prop1, (unsigned long)prop2, 0, 0);
    }
    else {
        fputs(argerr, errfp);
        return EXIT_FAILURE;
    }
}/*}}}*/

/* ----------------------------------------------------------------------- */

/* used to set icon */
void windowid(const char *winpath)
{
  int i;
  Display *display;
  Tk_Window mainwindow;

  unsigned int ww;
  Window framewin, rootwindow, parent_of_topwindow;
  Window *framewin_child_ptr;
  unsigned int framewindow_nchildren;

  dbg(1, "windowid(): winpath=%s\n", winpath);
  framewindow_nchildren =0;
  mainwindow=Tk_MainWindow(interp);
  display = Tk_Display(mainwindow);
  Tcl_VarEval(interp, "winfo id ", winpath, NULL);
  sscanf(tclresult(), "0x%x", (unsigned int *) &ww);
  framewin = ww;
  XQueryTree(display, framewin, &rootwindow, &parent_of_topwindow, &framewin_child_ptr, &framewindow_nchildren);
  dbg(1,"framewinID=%x\n", (unsigned int) framewin);
  dbg(1,"framewin nchilds=%d\n", (unsigned int)framewindow_nchildren);
  dbg(1,"framewin parentID=%x\n", (unsigned int) parent_of_topwindow);
  if (debug_var>=1) {
    if (framewindow_nchildren==0) fprintf(errfp, "no framewin child\n");
    else fprintf(errfp, "framewin child 0=%x\n", (unsigned int)framewin_child_ptr[0]);
  }

  /* here I create the icon pixmap,to be used when iconified,  */
#ifdef __unix__
  if(!cad_icon_pixmap) {
    i=XpmCreatePixmapFromData(display,framewin, cad_icon,&cad_icon_pixmap, &cad_icon_mask, NULL);
    dbg(1, "windowid(): creating icon pixmap returned: %d\n",i);
  }
  hints_ptr = XAllocWMHints();
  hints_ptr->icon_pixmap = cad_icon_pixmap ;
  hints_ptr->icon_mask = cad_icon_mask ;
  hints_ptr->flags = IconPixmapHint | IconMaskHint;
  XSetWMHints(display, parent_of_topwindow, hints_ptr);
  XFree(hints_ptr);
#endif
  Tcl_SetResult(interp,"",TCL_STATIC);
}

int err(Display *display, XErrorEvent *xev)
{
 char s[1024];  /* overflow safe 20161122 */
 int l=250;
#ifdef __unix__
 XGetErrorText(display, xev->error_code, s,l);
 dbg(1, "err(): Err %d :%s maj=%d min=%d\n", xev->error_code, s, xev->request_code,
          xev->minor_code);
#endif
 return 0;
}

unsigned int  find_best_color(char colorname[])
{
 int i;
 double distance=10000000000.0, dist, r, g, b, red, green, blue;
 double deltar,deltag,deltab;
 unsigned int idx;

 dbg(1, "find_best_color() start: %g\n", timer(1));
#ifdef __unix__
 if( XAllocNamedColor(display, colormap, colorname, &xcolor_exact, &xcolor) ==0 )
#else
 Tk_Window mainwindow = Tk_MainWindow(interp);
 XColor* xc = Tk_GetColor(interp, mainwindow, colorname);
 if (XAllocColor(display, colormap, xc) == 0)
#endif
 {
  for(i=0;i<=255;i++) {
    xctx->xcolor_array[i].pixel=i;
#ifdef __unix__
    XQueryColor(display, colormap, xctx->xcolor_array+i);
#else
    xcolor = *xc;
    XQueryColors(display, colormap, xc, i);
#endif
  }
  /* debug ... */
  dbg(2, "find_best_color(): Server failed to allocate requested color, finding substitute\n");
  XLookupColor(display, colormap, colorname, &xcolor_exact, &xcolor);
  red = xcolor.red; green = xcolor.green; blue = xcolor.blue;
  idx=0;
  for(i = 0;i<=255; i++)
  {
   r = xctx->xcolor_array[i].red ; g = xctx->xcolor_array[i].green ; b = xctx->xcolor_array[i].blue;
   deltar = (r - red);deltag = (g - green);deltab = (b - blue);
   dist = deltar*deltar + deltag*deltag + deltab*deltab;
   if( dist <= distance )
   {
    idx = i;
    distance = dist;
   }
  }
 }
 else
 {
  /*XLookupColor(display, colormap, colorname, &xcolor_exact, &xcolor); */
#ifdef __unix__
  idx = xcolor.pixel;
#else
  idx = xc->pixel;
#endif
 }
 dbg(1, "find_best_color() return: %g\n", timer(1));
 return idx;
}


static void init_color_array(double dim, double dim_bg)
{
 char s[256]; /* overflow safe 20161122 */
 int i;
 unsigned int r, g, b;
 double tmp_dim;

 for(i=0;i<cadlayers;i++) {
   my_snprintf(s, S(s), "lindex $colors %d",i);
   tcleval(s);
   dbg(2, "init_color_array(): color:%s\n",tclresult());

   sscanf(tclresult(), "#%02x%02x%02x", &r, &g, &b);
   if(i == BACKLAYER) tmp_dim = dim_bg;
   else tmp_dim = dim;
   if(tmp_dim>=0.) {
     r +=(51.-r/5.)*tmp_dim;
     g +=(51.-g/5.)*tmp_dim;
     b +=(51.-b/5.)*tmp_dim;
   } else {
     r +=(r/5.)*tmp_dim;
     g +=(g/5.)*tmp_dim;
     b +=(b/5.)*tmp_dim;
   }
   /* fprintf(errfp, "init_color_array: colors: %.16g %.16g %.16g dim=%.16g c=%d\n", r, g, b, dim, i); */
   if(r>0xff) r=0xff;
   if(g>0xff) g=0xff;
   if(b>0xff) b=0xff;
   my_snprintf(s, S(s), "#%02x%02x%02x", r, g, b);
   my_strdup(643, &xctx->color_array[i], s);
 }
}

void init_pixdata()/* populate xctx->fill_type array that is used in create_gc() to set fill styles */
{
 int i,j, full, empty;
 for(i=0;i<cadlayers;i++) {
   full=1; empty=1;
   for(j=0;j<32;j++) {
     if(i<sizeof(pixdata_init)/sizeof(pixdata_init[0]))
       pixdata[i][j] = pixdata_init[i][j];
     else
       pixdata[i][j] = 0x00;

     if(pixdata[i][j]!=0xff) full=0;
     if(pixdata[i][j]!=0x00) empty=0;
   }
   if(full) xctx->fill_type[i] = 1;
   else if(empty) xctx->fill_type[i] = 0;
   else xctx->fill_type[i]=2;
   if(rainbow_colors && i>5) xctx->fill_type[i]=1; /* 20171212 solid fill style */
   /*fprintf(errfp, "fill_type[%d]= %d\n", i, xctx->fill_type[i]); */
 }
}

void free_xschem_data()
{
  int i;
  delete_undo();
  free_simdata();
  my_free(1098, &xctx->wire);
  my_free(1100, &xctx->text);
  my_free(1107, &xctx->inst);
  for(i=0;i<cadlayers;i++) {
    my_free(1103, &xctx->rect[i]);
    my_free(1104, &xctx->line[i]);
    my_free(1105, &xctx->poly[i]);
    my_free(1106, &xctx->arc[i]);
  }
  for(i=0;i<xctx->maxs;i++) {
    my_free(1109, &xctx->sym[i].line);
    my_free(1110, &xctx->sym[i].rect);
    my_free(1111, &xctx->sym[i].arc);
    my_free(1112, &xctx->sym[i].poly);
    my_free(1113, &xctx->sym[i].lines);
    my_free(1114, &xctx->sym[i].polygons);
    my_free(1115, &xctx->sym[i].arcs);
    my_free(1116, &xctx->sym[i].rects);
  }
  my_free(1117, &xctx->sym);
  my_free(1118, &xctx->rect);
  my_free(1119, &xctx->line);
  my_free(1125, &xctx->poly);
  my_free(1126, &xctx->arc);
  my_free(1124, &xctx->rects);
  my_free(1127, &xctx->polygons);
  my_free(1128, &xctx->arcs);
  my_free(1129, &xctx->lines);
  my_free(1130, &xctx->maxr);
  my_free(1131, &xctx->maxp);
  my_free(1132, &xctx->maxa);
  my_free(1133, &xctx->maxl);
  my_free(1108, &xctx->sel_array);
  for(i=0;i<CADMAXHIER;i++) my_free(1139, &xctx->sch_path[i]);
  my_free(1099, &xctx->gridpoint);
  my_free(1214, &xctx->biggridpoint);
  my_free(1135, &xctx->gc);
  my_free(1136, &xctx->gcstipple);
  for(i=0;i<cadlayers;i++) my_free(1101, &xctx->color_array[i]);
  my_free(605, &xctx->color_array);
  my_free(1123, &xctx->enable_layer);
  my_free(1121, &xctx->active_layer);
  my_free(1295, &xctx->top_path);
  my_free(1120, &xctx->fill_type);
  my_free(269, &xctx);
}

void create_gc(void)
{
  int i;
  for(i=0;i<cadlayers;i++)
  {
    pixmap[i] = XCreateBitmapFromData(display, xctx->window, (char*)(pixdata[i]),16,16);
    xctx->gc[i] = XCreateGC(display,xctx->window,0L,NULL);
    xctx->gcstipple[i] = XCreateGC(display,xctx->window,0L,NULL);
    XSetStipple(display,xctx->gcstipple[i],pixmap[i]);
    if(xctx->fill_type[i]==1)  XSetFillStyle(display,xctx->gcstipple[i],FillSolid);
    else XSetFillStyle(display,xctx->gcstipple[i],FillStippled);
  }
}

void free_gc()
{
  int i;
  for(i=0;i<cadlayers;i++) {
    XFreeGC(display,xctx->gc[i]);
    XFreeGC(display,xctx->gcstipple[i]);
  }
}

void alloc_xschem_data(const char *top_path)
{
  int i, j;

  xctx = my_calloc(153, 1, sizeof(Xschem_ctx));
  xctx->cur_undo_ptr = 0;
  xctx->head_undo_ptr = 0;
  xctx->tail_undo_ptr = 0;
  xctx->undo_dirname = NULL;
  #ifndef IN_MEMORY_UNDO
  /* 20150327 create undo directory */
  /* 20180923 no more mkdtemp (portability issues) */
  if( !my_strdup(644, &xctx->undo_dirname, create_tmpdir("xschem_undo_") )) {
    fprintf(errfp, "xinit(): problems creating tmp undo dir\n");
    tcleval("exit");
  }
  dbg(1, "undo_dirname=%s\n", xctx->undo_dirname);
  #endif
  xctx->zoom=CADINITIALZOOM;
  xctx->mooz=1/CADINITIALZOOM;
  xctx->xorigin=CADINITIALX;
  xctx->yorigin=CADINITIALY;
  xctx->wires = 0;
  xctx->instances = 0;
  xctx->symbols = 0;
  xctx->sym_txt = 1;
  xctx->texts = 0;
  xctx->schprop = NULL;     /* SPICE */
  xctx->schtedaxprop=NULL;  /* tEDAx */
  xctx->schvhdlprop=NULL;   /* vhdl property string */
  xctx->schsymbolprop=NULL; /* symbol property string */
  xctx->schverilogprop=NULL;/* verilog */
  xctx->version_string = NULL;
  xctx->rectcolor= 4;  /* this is the current layer when xschem started. */
  xctx->currsch = 0;
  xctx->ui_state = 0;
  xctx->need_reb_sel_arr = 1;
  xctx->lastsel = 0;
  xctx->maxsel = 0;
  xctx->prep_net_structs = 0;
  xctx->prep_hi_structs = 0;
  xctx->simdata = NULL;
  xctx->simdata_ninst = 0;
  xctx->prep_hash_inst = 0;
  xctx->prep_hash_wires = 0;
  xctx->modified = 0;
  xctx->semaphore = 0;
  xctx->get_tok_size = 0;
  xctx->netlist_name[0] = '\0';
  xctx->flat_netlist = 0;
  xctx->plotfile[0] = '\0';
  xctx->netlist_unconn_cnt = 0; /* unique count of unconnected pins while netlisting */
  xctx->current_dirname[0] = '\0';
  for(i = 0; i < NBOXES; i++) {
    for(j = 0; j < NBOXES; j++) {
      xctx->instpin_spatial_table[i][j] = NULL;
      xctx->wire_spatial_table[i][j] = NULL;
      xctx->inst_spatial_table[i][j] = NULL;
    }
  }
  for(i = 0 ; i < HASHSIZE; i++) {
    xctx->inst_table[i] = NULL;
    xctx->node_table[i] = NULL;
    xctx->hilight_table[i] = NULL;
    xctx->node_redraw_table[i] = NULL; /* move.c */
  }
  xctx->window = xctx->save_pixmap = 0;
  xctx->xrect[0].width = xctx->xrect[0].height = xctx->xrect[0].x = xctx->xrect[0].y = 0;
  xctx->xschem_w = xctx->xschem_h = 0;
#if HAS_CAIRO==1
  xctx->cairo_ctx = xctx->cairo_save_ctx = NULL;
  xctx->cairo_sfc = xctx->cairo_save_sfc = NULL;
#endif
  xctx->gctiled = 0;
  /* get_unnamed_node() */
  xctx->new_node = 0;
  xctx->node_mult = NULL;
  xctx->node_mult_size = 0;
  /* move.c */
  xctx->rx1 = xctx->rx2 = xctx->ry1 = xctx->ry2 = 0.0;
  xctx->move_rot = 0;
  xctx->move_flip = 0;
  xctx->manhattan_lines = 0;
  xctx->x1 = xctx->y_1 = xctx->x2 = xctx->y_2 = xctx->deltax = xctx->deltay = 0.0;
  xctx->movelastsel = 0;
  xctx->rotatelocal=0;
  /* new_wire */
  xctx->nl_x1 = xctx->nl_y1 = xctx->nl_x2 = xctx->nl_y2 = 0.0;
  xctx->nl_xx1 = xctx->nl_yy1 = xctx->nl_xx2 = xctx->nl_yy2 = 0.0;
  /* new_arc */
  xctx->nl_x = xctx->nl_y = xctx->nl_r = xctx->nl_a = xctx->nl_b = xctx->nl_x3 = xctx->nl_y3 = 0.0;
  xctx->nl_state = 0;
  xctx->nl_sweep_angle = 0.0;
  /* new_polygon */
  xctx->nl_polyx = xctx->nl_polyy = NULL;
  xctx->nl_points = xctx->nl_maxpoints = 0;
  /* select_rect */
  xctx->nl_xr = xctx->nl_yr = xctx->nl_xr2 = xctx->nl_yr2 = 0.0;
  xctx->nl_sel = xctx->nl_sem = 0;

  xctx->hilight_time = 0; /* timestamp for sims */
  xctx->hilight_nets = 0;
  xctx->hilight_color = 0;
  for(i=0;i<CADMAXHIER;i++) {
    xctx->sch_path[i]=NULL;
    xctx->sch_path_hash[i]=0;
  }
  my_strdup(1187, &xctx->sch_path[0],".");
  xctx->sch_inst_number[0] = 1;
  xctx->maxt=CADMAXTEXT;
  xctx->maxw=CADMAXWIRES;
  xctx->maxi=ELEMINST;
  xctx->maxs=ELEMDEF;
  xctx->text=my_calloc(606, xctx->maxt,sizeof(xText));
  xctx->wire=my_calloc(607, xctx->maxw,sizeof(xWire));
  xctx->inst=my_calloc(609, xctx->maxi , sizeof(xInstance) );
  xctx->sym=my_calloc(610, xctx->maxs , sizeof(xSymbol) );
  for(i=0;i<xctx->maxs;i++) {
    xctx->sym[i].line=my_calloc(611, cadlayers, sizeof(xLine *));
    xctx->sym[i].poly=my_calloc(612, cadlayers, sizeof(xPoly *));
    xctx->sym[i].arc=my_calloc(613, cadlayers, sizeof(xArc *));
    xctx->sym[i].rect=my_calloc(614, cadlayers, sizeof(xRect *));
    xctx->sym[i].lines=my_calloc(615, cadlayers, sizeof(int));
    xctx->sym[i].rects=my_calloc(616, cadlayers, sizeof(int));
    xctx->sym[i].arcs=my_calloc(617, cadlayers, sizeof(int));
    xctx->sym[i].polygons=my_calloc(618, cadlayers, sizeof(int));
  }
  xctx->maxr=my_calloc(620, cadlayers, sizeof(int));
  xctx->maxa=my_calloc(621, cadlayers, sizeof(int));
  xctx->maxp=my_calloc(622, cadlayers, sizeof(int));
  xctx->maxl=my_calloc(623, cadlayers, sizeof(int));
  for(i=0;i<cadlayers;i++)
  {
    xctx->maxr[i]=CADMAXOBJECTS;
    xctx->maxp[i]=CADMAXOBJECTS;
    xctx->maxl[i]=CADMAXOBJECTS;
    xctx->maxa[i]=CADMAXOBJECTS;
  }
  xctx->rect=my_calloc(624, cadlayers, sizeof(xRect *));
  xctx->line=my_calloc(625, cadlayers, sizeof(xLine *));
  xctx->poly=my_calloc(626, cadlayers, sizeof(xPoly *));
  xctx->arc=my_calloc(627, cadlayers, sizeof(xArc *));
  for(i=0;i<cadlayers;i++)
  {
    xctx->rect[i]=my_calloc(628, xctx->maxr[i],sizeof(xRect));
    xctx->arc[i]=my_calloc(629, xctx->maxa[i],sizeof(xArc));
    xctx->poly[i]=my_calloc(630, xctx->maxp[i],sizeof(xPoly));
    xctx->line[i]=my_calloc(631, xctx->maxl[i],sizeof(xLine));
  }
  xctx->rects=my_calloc(632, cadlayers, sizeof(int));
  xctx->polygons=my_calloc(633, cadlayers, sizeof(int));
  xctx->arcs=my_calloc(634, cadlayers, sizeof(int));
  xctx->lines=my_calloc(635, cadlayers, sizeof(int));
  xctx->maxsel=MAXGROUP;
  xctx->sel_array=my_calloc(619, xctx->maxsel, sizeof(Selected));
  xctx->biggridpoint=(XSegment*)my_calloc(1213, CADMAXGRIDPOINTS,sizeof(XSegment));
  xctx->gridpoint=(XPoint*)my_calloc(608, CADMAXGRIDPOINTS,sizeof(XPoint));
  xctx->enable_drill = 0;
  xctx->prev_set_modify = -1;
  xctx->pending_fullzoom = 0;
  my_strncpy(xctx->hiersep, ".", S(xctx->hiersep));
  xctx->no_undo = 0;
  xctx->draw_single_layer = -1;
  xctx->draw_dots = 1;
  xctx->only_probes = 0;
  xctx->menu_removed = 0; /* fullscreen pervious setting */
  xctx->save_lw = 0.0;  /* used to save linewidth when selecting 'only_probes' view */
  xctx->onetime = 0; /* callback() static var */
  xctx->save_netlist_type = 0;
  xctx->loaded_symbol = 0;
  xctx->no_draw = 0;
  xctx->sem = 0; /* bbox */
  xctx->old_prop = NULL;
  xctx->edit_sym_i = -1;
  xctx->netlist_commands = 0;
  xctx->draw_pixmap = 1;
  xctx->gc=my_calloc(638, cadlayers, sizeof(GC));
  xctx->gcstipple=my_calloc(639, cadlayers, sizeof(GC));
  xctx->color_array=my_calloc(637, cadlayers, sizeof(char*));
  xctx->enable_layer=my_calloc(87, cadlayers, sizeof(int));
  xctx->n_active_layers = 0;
  xctx->active_layer=my_calloc(563, cadlayers, sizeof(int));
  xctx->hide_symbols = 0;
  xctx->netlist_type = CAD_SPICE_NETLIST;
  xctx->top_path = NULL;
  my_strdup2(1296, &xctx->top_path, top_path);
  xctx->fill_type=my_calloc(640, cadlayers, sizeof(int));
  xctx->fill_pattern = 1;
  xctx->draw_window = 0;
  #ifdef IN_MEMORY_UNDO
  xctx->undo_initialized = 0; /* in_memory_undo */
  #endif
  xctx->time_last_modify = 0;
  xctx->push_undo_ptr = &push_undo;
  xctx->pop_undo_ptr = &pop_undo;
  xctx->delete_undo_ptr = &delete_undo;
  xctx->clear_undo_ptr = &clear_undo;

}

void delete_schematic_data(void)
{
  dbg(1, "delete_schematic_data()\n");
  unselect_all();
  /* clear static data in get_tok_value() must be done after unselect_all() 
   * as this functions re-uses get_tok_value() */
  get_tok_value(NULL, NULL, 0); /* clear static data in function */
  delete_netlist_structs();  /* netlist - specific data and hash tables */
  clear_all_hilights();      /* data structs for hilighting nets/instances */
  get_unnamed_node(0, 0, 0); /* net### enumerator used for netlisting */
  if(has_x) {
    resetwin(0, 1, 1, 0, 0);  /* delete preview pixmap, delete cairo surfaces */
    free_gc();
  }
  clear_drawing();    /* delete instances, wires, lines, rects, ... */
  remove_symbols();
  free_xschem_data(); /* delete the xctx struct */
}

void xwin_exit(void)
{
 int i;

 if(!init_done) {
   dbg(0, "xwin_exit() double call, doing nothing...\n");
   return;
 }
 delete_schematic_data();
 if(has_x) {
   Tk_DestroyWindow(mainwindow);
   #ifdef __unix__
   if(cad_icon_pixmap) {
     XFreePixmap(display, cad_icon_pixmap);
     XFreePixmap(display, cad_icon_mask);
   }
   #else
   if (cad_icon_pixmap) Tk_FreePixmap(display, cad_icon_pixmap);
   #endif
   #ifdef __unix__
   for(i = 0; i < cadlayers; i++) XFreePixmap(display,pixmap[i]);
   #else
   for(i = 0; i < cadlayers; i++) Tk_FreePixmap(display, pixmap[i]);
   #endif
   my_free(1134, &pixmap);
 }
 dbg(1, "xwin_exit(): clearing drawing data structures\n");

 /* global context - graphic preferences/settings */
 for(i=0;i<cadlayers;i++) {
   my_free(1102, &pixdata[i]);
 }
 my_free(1122, &pixdata);
 my_free(1138, &tcl_command);
 clear_expandlabel_data();
 get_sym_template(NULL, NULL); /* clear static data in function */
 list_tokens(NULL, 0); /* clear static data in function */
 translate(0, NULL); /* clear static data in function */
 translate2(NULL, 0, NULL); /* clear static data in function */
 subst_token(NULL, NULL, NULL); /* clear static data in function */
 find_nth(NULL, '\0', 0); /* clear static data in function */
 tcl_hook2(NULL); /* clear static data in function */
 save_ascii_string(NULL, NULL, 0); /* clear static data in function */
 dbg(1, "xwin_exit(): removing font\n");
 for(i=0;i<127;i++) my_free(1140, &character[i]);
 dbg(1, "xwin_exit(): closed display\n");
 my_free(1141, &filename);
 my_free(1143, &xschem_executable);
 record_global_node(2, NULL, NULL); /* delete global node array */
 dbg(1, "xwin_exit(): deleted undo buffer\n");
 if(errfp!=stderr) fclose(errfp);
 errfp=stderr;
 printf("\n");
 init_done=0; /* 20150409 to avoid multiple calls */
}


/* 
 *  color structures in xschem:
 *  - char *xctx->color_array[]: 
 *      array of color character  names ("#xxxxxx" hex) indexed by xschem layer number.
 *      these are set from tcl 'color' list variable in init_color_array()
 *  - unsigned int xctx->color_index[]: 
 *      array of integers, pixel values, color lookup table, indexed by xschem layer num.
 *      this array is initialized in build_colors() by calling find_best_color()
 *      indexes are returned from XAllocNamedColor()
 *      these are used in XSetForeground and XSetBackground calls:
 *        XSetForeground(display, gc, xctx->color_index[i]) 
 *  - XColor xctx->xcolor_array[]: 
 *      array of 256 XColor structures:
 *      typedef struct {
 *        unsigned long pixel; //  pixel value 
 *        unsigned short red, green, blue; // rgb values
 *        char flags; // DoRed, DoGreen, DoBlue 	
 *        char pad;
 *      } XColor;
 *      This array is used temporarily in find_best_color() to store all
 *      allocated colors in case of pseudocolor visuals to find best substitute when
 *      XAllocNamedColor fails to find the requested "color_array[i]".
 *      When all xctx->color_index[] are populated with pixel values xctx->xcolor_array[]
 *      is reset with the xschem layer colors in build_colors() using XLookupColor():
 *        for(i=0;i<cadlayers;i++) {
 *          XLookupColor(display, colormap, xctx->color_array[i], &xcolor_exact, &xcolor);
 *          xctx->xcolor_array[i] = xcolor;
 *        }
 *
 *
 *
 */
int build_colors(double dim, double dim_bg)
{
    int i;
    dbg(1, "build_colors(): dim=%g, dim_bg=%g\n", dim, dim_bg);
    if(tclgetboolvar("dark_colorscheme")) {
      tcleval("llength $dark_colors");
      if(atoi(tclresult())>=cadlayers){
        tcleval("set colors $dark_colors");
      }
    } else {
      tcleval("llength $light_colors");
      if(atoi(tclresult()) >=cadlayers){
        tcleval("set colors $light_colors");
      }
    }
    tcleval("llength $colors");
    if(atoi(tclresult())<cadlayers){
      fprintf(errfp,"Tcl var colors not set correctly\n");
      return -1; /* fail */
    } else {
      tcleval("regsub -all {\"} $colors {} svg_colors");
      tcleval("regsub -all {#} $svg_colors {0x} svg_colors");
    }
    init_color_array(dim, dim_bg);
    for(i=0;i<cadlayers;i++)
    {
     xctx->color_index[i] = find_best_color(xctx->color_array[i]);
    }
    for(i=0;i<cadlayers;i++)
    {
      XSetBackground(display, xctx->gc[i], xctx->color_index[0]); /* for dashed lines 'off' color */
      XSetForeground(display, xctx->gc[i], xctx->color_index[i]);
      XSetForeground(display, xctx->gcstipple[i], xctx->color_index[i]);
    }
    for(i=0;i<cadlayers;i++) {
      XLookupColor(display, colormap, xctx->color_array[i], &xcolor_exact, &xcolor);
      xctx->xcolor_array[i] = xcolor;
    }
    tcleval("reconfigure_layers_menu");
    return 0; /* success */
}

void set_clip_mask(int what)
{
  int i;
  if(what == SET) {
    for(i=0;i<cadlayers;i++)
    {
      XSetClipRectangles(display, xctx->gc[i], 0,0, xctx->xrect, 1, Unsorted);
      XSetClipRectangles(display, xctx->gcstipple[i], 0,0, xctx->xrect, 1, Unsorted);
    }
    XSetClipRectangles(display, xctx->gctiled, 0,0, xctx->xrect, 1, Unsorted);
    #if HAS_CAIRO==1
    cairo_rectangle(xctx->cairo_ctx, xctx->xrect[0].x, xctx->xrect[0].y,
                    xctx->xrect[0].width, xctx->xrect[0].height);
    cairo_clip(xctx->cairo_ctx);
    cairo_rectangle(xctx->cairo_save_ctx, xctx->xrect[0].x, xctx->xrect[0].y,
                    xctx->xrect[0].width, xctx->xrect[0].height);
    cairo_clip(xctx->cairo_save_ctx);
    #endif
  } else if(what == END) {
    for(i=0;i<cadlayers;i++)
    {
     XSetClipMask(display, xctx->gc[i], None);
     XSetClipMask(display, xctx->gcstipple[i], None);
    }
    XSetClipMask(display, xctx->gctiled, None);
    #if HAS_CAIRO==1
    cairo_reset_clip(xctx->cairo_ctx);
    cairo_reset_clip(xctx->cairo_save_ctx);
    #endif
  }
}
#ifdef __unix__
/* moved here to avoid Xorg-specific calls in move.c */
int pending_events(void)
{
  return XPending(display);
}
#endif

void toggle_fullscreen(const char *topwin)
{
  char fullscr[]="add,fullscreen";
  char normal[]="remove,fullscreen";
  unsigned int topwin_id;
  Window rootwindow, parent_id;
  Window *framewin_child_ptr;
  unsigned int framewindow_nchildren;
  int fs;

  if(!strcmp(topwin, ".drw")) {
    tcleval( "winfo id .");
    sscanf(tclresult(), "0x%x", (unsigned int *) &topwin_id);
  } else {
    Tcl_VarEval(interp, "winfo id ", xctx->top_path, NULL);
    sscanf(tclresult(), "0x%x", (unsigned int *) &topwin_id);
  }
  XQueryTree(display, topwin_id, &rootwindow, &parent_id, &framewin_child_ptr, &framewindow_nchildren);
  fs = tclgetintvar("fullscreen");
  fs = (fs+1)%2;
  if(fs==1) tclsetvar("fullscreen","1");
  else if(fs==2) tclsetvar("fullscreen","2");
  else tclsetvar("fullscreen","0");

  dbg(1, "toggle_fullscreen(): fullscreen=%d\n", fs);
  if(fs==2) {
    Tcl_VarEval(interp, "pack forget ", xctx->top_path, ".menubar ", xctx->top_path, ".statusbar; update", NULL);
    xctx->menu_removed = 1;
  }
  if(fs !=2 && xctx->menu_removed) {
    Tcl_VarEval(interp, "pack ", xctx->top_path,
       ".menubar -anchor n -side top -fill x  -before ", xctx->top_path, ".drw; pack ",
       xctx->top_path, ".statusbar -after ", xctx->top_path, ".drw -anchor sw  -fill x; update", NULL);
    xctx->menu_removed=0;
  }
  if(fs == 1) {
    window_state(display , parent_id,fullscr);
  } else if(fs == 2) {
    window_state(display , parent_id,normal);
    window_state(display , parent_id,fullscr);
  } else {
    window_state(display , parent_id,normal);
  }
  xctx->pending_fullzoom=1;
}


void tclexit(ClientData s)
{
  dbg(1, "tclexit() INVOKED\n");
  if(init_done) xwin_exit();
}

#if HAS_XCB==1
/* from xcb.freedesktop.org -- don't ask me what it does... 20171125 */
static xcb_visualtype_t *find_visual(xcb_connection_t *xcbconn, xcb_visualid_t visual)
{
    xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(xcb_get_setup(xcbconn));

    for (; screen_iter.rem; xcb_screen_next(&screen_iter)) {
        xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(screen_iter.data);
        for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
            xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
            for (; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
                if (visual == visual_iter.data->visual_id) {
                    return visual_iter.data;
                }
            }
        }
    }

    return NULL;
}
#endif /*HAS_XCB */

int source_tcl_file(char *s)
{
  char tmp[1024];
  if(Tcl_EvalFile(interp, s)==TCL_ERROR) {
    fprintf(errfp, "Tcl_AppInit() error: can not execute %s, please fix:\n", s);
    fprintf(errfp, "%s", tclresult());
    fprintf(errfp, "\n");
    my_snprintf(tmp, S(tmp), "tk_messageBox -icon error -type ok -message \
       {Tcl_AppInit() err 1: can not execute %s, please fix:\n %s}",
       s, tclresult());
    if(has_x) {
      tcleval( "wm withdraw .");
      tcleval( tmp);
      Tcl_Exit(EXIT_FAILURE);
    }
    return TCL_ERROR;
  }
  return TCL_OK;
}

void preview_window(const char *what, const char *tk_win_path, const char *filename)
{
  static char *current_file = NULL;
  static Xschem_ctx *save_xctx = NULL; /* save pointer to current schematic context structure */
  static Xschem_ctx *preview_xctx = NULL; /* save pointer to current schematic context structure */
  static Window pre_window;
  static Tk_Window tkpre_window;

  dbg(1, "------\n");
  if(!strcmp(what, "create")) {
    dbg(1, "preview_window() create, save ctx, tk_win_path=%s\n", tk_win_path);
    tkpre_window = Tk_NameToWindow(interp, tk_win_path, mainwindow);
    Tk_MakeWindowExist(tkpre_window);
    pre_window = Tk_WindowId(tkpre_window);
    save_xctx = xctx; /* save current schematic */
  }
  else if(!strcmp(what, "draw")) {
    dbg(1, "preview_window() draw\n");
    xctx = preview_xctx;
    if(!current_file || strcmp(filename, current_file) ) { 
      if(current_file) {
        delete_schematic_data();
      }
      my_strdup(117, &current_file, filename);
      xctx = NULL;      /* reset for preview */
      alloc_xschem_data(".dialog"); /* alloc data into xctx */
      init_pixdata(); /* populate xctx->fill_type array that is used in create_gc() to set fill styles */
      preview_xctx = xctx;
      preview_xctx->window = pre_window;
      create_gc();
      enable_layers();
      build_colors(0.0, 0.0);
      resetwin(1, 0, 1, 0, 0);  /* create preview pixmap.  resetwin(create_pixmap, clear_pixmap, force) */
      dbg(1, "preview_window() draw, load schematic\n");
      load_schematic(1,filename, 0);
    }
    zoom_full(1, 0, 1, 0.97); /* draw */
    xctx = save_xctx;
  }
  else if(!strcmp(what, "destroy")) {
    dbg(1, "preview_window() destroy\n");
    xctx = preview_xctx;
    if(current_file) {
      delete_schematic_data();
      preview_xctx = NULL;
    }
    
    Tk_DestroyWindow(tkpre_window);
    my_free(1144, &current_file);
    xctx = save_xctx; /* restore schematic */
    save_xctx = NULL;
    set_modify(xctx->modified);
  }
}


/* top_path is the path prefix of tk_win_path:
 *
 *  tk_win_path    top_path
 *    ".drw"       ""
 *    ".xx.drw"    ".xx"
 */
void new_schematic(const char *what, const char *top_path, const char *tk_win_path, const char *filename)
{
  static int cnt = 0;
  static Xschem_ctx *save_xctx[MAX_NEW_WINDOWS]; /* save pointer to current schematic context structure */
  static Tk_Window tknew_window[MAX_NEW_WINDOWS];
  int i, n;
  Window new_window;
  Tk_Window tkwin;

  if(!strcmp(what, "create")) {
    dbg(1, "new_schematic() create, save ctx tk_win_path=%s\n", tk_win_path);
    if(cnt == 0) {
      for(i = 0; i < MAX_NEW_WINDOWS; i++) {
        save_xctx[i] = NULL;
        tknew_window[i] = NULL;
      }
      save_xctx[0] = xctx; /* save current schematic */
      tknew_window[0] = Tk_NameToWindow(interp, ".drw", mainwindow);
    }
    if(cnt + 1 >= MAX_NEW_WINDOWS) {
      dbg(0, "new_schematic(\"create\"...): no more free slots\n");
      return; /* no more free slots */
    }
    cnt++;
    n = -1;
    for(i = 1; i < MAX_NEW_WINDOWS; i++) { /* search 1st free slot */
      if(save_xctx[i] == NULL) {
        n = i;
        break;
      }
    }
    if(n == -1) {
      dbg(0, "new_schematic(\"create\"...): no more free slots\n");
      return;
    }
    tknew_window[n] = Tk_NameToWindow(interp, tk_win_path, mainwindow);
    Tk_MakeWindowExist(tknew_window[n]);
    new_window = Tk_WindowId(tknew_window[n]);
    Tk_ChangeWindowAttributes(tknew_window[n], CWBackingStore, &winattr);
    dbg(1, "new_schematic() draw\n");
    xctx = NULL;      /* reset for preview */
    alloc_xschem_data(top_path); /* alloc data into xctx */
    xctx->netlist_type = CAD_SPICE_NETLIST; /* for new windows start with spice netlist mode */
    tclsetvar("netlist_type","spice");
    init_pixdata();/* populate xctx->fill_type array that is used in create_gc() to set fill styles */
    save_xctx[n] = xctx;
    dbg(1, "new_schematic() draw, load schematic\n");
    xctx->window = new_window;
    set_snap(0); /* set default value specified in xschemrc as 'snap' else CADSNAP */
    set_grid(0); /* set default value specified in xschemrc as 'grid' else CADGRID */
    create_gc();
    enable_layers();
    build_colors(0.0, 0.0);
    resetwin(1, 0, 1, 0, 0);  /* create preview pixmap.  resetwin(create_pixmap, clear_pixmap, force, w, h) */
    /* draw empty window so if following load fails due to missing file window appears correctly drawn */
    zoom_full(1, 0, 1, 0.97);
    load_schematic(1,filename, 1);
    zoom_full(1, 0, 1, 0.97); /* draw */
  } else if(!strcmp(what, "destroy")) {
    if(cnt) {
      int close = 0;
      dbg(1, "new_schematic() destroy\n");
      /* reset old focused window so callback() will force repaint on expose events */
      if(xctx->modified && has_x) {
        tcleval("tk_messageBox -type okcancel -message \""
                "[get_cell [xschem get schname] 0]"
                ": UNSAVED data: want to exit?\"");
        if(strcmp(tclresult(),"ok")==0) close = 1;
      }
      else close = 1;
      Tcl_ResetResult(interp);
      if(close) {
        tkwin =  Tk_NameToWindow(interp, tk_win_path, mainwindow); /* NULL if tk_win_path not existing */
        if(!tkwin) dbg(0, "new_schematic(\"destroy\", ...): Warning: %s has been destroyed\n", tk_win_path);
        n = -1;
        if(tkwin) for(i = 1; i < MAX_NEW_WINDOWS; i++) {
          if(tkwin == tknew_window[i]) {
            n = i;
            break;
          }
        }
        if(n == -1) {
          dbg(0, "new_schematic(\"destroy\"...): no window to destroy found: %s\n", tk_win_path);
          return;
        }
        if(tkwin && n >= 1 && n < MAX_NEW_WINDOWS) {
          xctx = save_xctx[n];
          delete_schematic_data();
          save_xctx[n] = NULL;
          Tcl_VarEval(interp, "winfo toplevel ", tk_win_path, NULL);
          Tk_DestroyWindow(tknew_window[n]);
          Tcl_VarEval(interp, "destroy ", tclresult(), NULL);
          tknew_window[n] = NULL;
          /* delete Tcl context of deleted schematic window */
          Tcl_VarEval(interp, "delete_ctx ", tk_win_path, "; incr tctx::cnt -1", NULL);
          cnt--;
        }
      }
      xctx = save_xctx[0]; /* restore main (.drw) schematic */
      tcleval("restore_ctx .drw; housekeeping_ctx");
      set_modify(xctx->modified); /* sets window title */
    }
  } else if(!strcmp(what, "switch")) {
    if(cnt) {
      dbg(1, "new_schematic() switch: %s\n", tk_win_path);
      tkwin =  Tk_NameToWindow(interp, tk_win_path, mainwindow); /* NULL if tk_win_path not existing */
      if(!tkwin) dbg(0, "new_schematic(\"switch\",...): Warning: %s has been destroyed\n", tk_win_path);
      n = -1;
      if(tkwin) for(i = 0; i < MAX_NEW_WINDOWS; i++) {
        if(tkwin == tknew_window[i]) {
          n = i;
          break;
        }
      }
      if(n == -1) {
        dbg(0, "new_schematic(\"switch\"...): no window to switch to found: %s\n", tk_win_path);
        return;
      }
      /* if window was closed then tkwin == 0 --> do nothing */
      if(tkwin && n >= 0 && n < MAX_NEW_WINDOWS) {
        xctx = save_xctx[n];
        set_modify(xctx->modified); /* sets window title */
      }
    }
  }
}

void change_linewidth(double w)
{
  int i, changed;

  changed=0;
  /* choose line width automatically based on zoom */
  if(w<0.) {
    int cs;
    cs = tclgetdoublevar("cadsnap");
    if(tclgetboolvar("change_lw"))  {
      xctx->lw=xctx->mooz * 0.09 * cs;
      cadhalfdotsize = CADHALFDOTSIZE +  0.04 * (cs-10);
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
        XSetLineAttributes (display, xctx->gc[i], INT_WIDTH(xctx->lw), LineSolid, CapRound , JoinRound);
    }
    XSetLineAttributes (display, xctx->gctiled, INT_WIDTH(xctx->lw), LineSolid, CapRound , JoinRound);
  }
  xctx->areax1 = -2*INT_WIDTH(xctx->lw);
  xctx->areay1 = -2*INT_WIDTH(xctx->lw);
  xctx->areax2 = xctx->xrect[0].width+2*INT_WIDTH(xctx->lw);
  xctx->areay2 = xctx->xrect[0].height+2*INT_WIDTH(xctx->lw);
  xctx->areaw = xctx->areax2-xctx->areax1;
  xctx->areah = xctx->areay2 - xctx->areay1;
}

/* clears and creates cairo_sfc, cairo_ctx, cairo_save_sfc, cairo_save_ctx
 * and sets some graphical attributes */
void resetcairo(int create, int clear, int force_or_resize)
{ 
  #if HAS_CAIRO==1
  dbg(1, "resetcairo() %d, %d, %d\n", create, clear, force_or_resize);
  if(clear && force_or_resize) {
    /* xctx->cairo_save_sfc is based on pixmap and pixmaps are not resizeable, so on resize 
     * we must destroy & recreate everything. xctx->cairo_sfc can be resized using cairo_*_surface_set_size
     * being based on window */
    cairo_destroy(xctx->cairo_save_ctx);
    cairo_surface_destroy(xctx->cairo_save_sfc);
    cairo_destroy(xctx->cairo_ctx);
    cairo_surface_destroy(xctx->cairo_sfc);
  }
  if(create && force_or_resize) {
    /***** Create Cairo save buffer drawing area *****/
    #if HAS_XRENDER==1
    #if HAS_XCB==1
    xctx->cairo_save_sfc = cairo_xcb_surface_create_with_xrender_format(xcbconn, screen_xcb, xctx->save_pixmap,
         &format_rgb, xctx->xschem_w, xctx->xschem_h);
    #else
    xctx->cairo_save_sfc = cairo_xlib_surface_create_with_xrender_format(display, xctx->save_pixmap,
         DefaultScreenOfDisplay(display), render_format, xctx->xschem_w, xctx->xschem_h);
    #endif /* HAS_XCB */
    #else
    xctx->cairo_save_sfc = 
       cairo_xlib_surface_create(display, xctx->save_pixmap, visual, xctx->xschem_w, xctx->xschem_h);
    #endif /* HAS_XRENDER */
    if(cairo_surface_status(xctx->cairo_save_sfc)!=CAIRO_STATUS_SUCCESS) {
      fprintf(errfp, "ERROR: invalid cairo xcb surface\n");
    }
    xctx->cairo_save_ctx = cairo_create(xctx->cairo_save_sfc);
    cairo_select_font_face(xctx->cairo_save_ctx, tclgetvar("cairo_font_name"), 
      CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(xctx->cairo_save_ctx, 20);
    cairo_set_line_width(xctx->cairo_save_ctx, 1);
    cairo_set_line_join(xctx->cairo_save_ctx, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_cap(xctx->cairo_save_ctx, CAIRO_LINE_CAP_ROUND);
    /***** Create Cairo main drawing window structures *****/
    #if HAS_XRENDER==1
    #if HAS_XCB==1
    dbg(1, "create_cairo_surface: w=%d, h=%d\n", xctx->xschem_w, xctx->xschem_h);
    xctx->cairo_sfc = cairo_xcb_surface_create_with_xrender_format(xcbconn,
          screen_xcb, xctx->window, &format_rgb, xctx->xschem_w, xctx->xschem_h);
    #else
    xctx->cairo_sfc = cairo_xlib_surface_create_with_xrender_format (display,
          xctx->window, DefaultScreenOfDisplay(display), render_format, xctx->xschem_w, xctx->xschem_h);
    #endif /* HAS_XCB */
    #else
    xctx->cairo_sfc = cairo_xlib_surface_create(display, xctx->window, visual, xctx->xschem_w, xctx->xschem_h);
    #endif /* HAS_XRENDER */
    if(cairo_surface_status(xctx->cairo_sfc)!=CAIRO_STATUS_SUCCESS) {
      fprintf(errfp, "ERROR: invalid cairo surface\n");
    }
    xctx->cairo_ctx = cairo_create(xctx->cairo_sfc);
    cairo_select_font_face(xctx->cairo_ctx, tclgetvar("cairo_font_name"),
      CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(xctx->cairo_ctx, 20);
    cairo_set_line_width(xctx->cairo_ctx, 1);
    cairo_set_line_join(xctx->cairo_ctx, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_cap(xctx->cairo_ctx, CAIRO_LINE_CAP_ROUND);
    #if 0
    *  #if HAS_XCB==1 && HAS_XRENDER==1
    *  cairo_xcb_surface_set_size(xctx->cairo_sfc, xctx->xschem_w, xctx->xschem_h);
    *  #else
    *  cairo_xlib_surface_set_size(xctx->cairo_sfc, xctx->xschem_w, xctx->xschem_h);
    *  #endif /* HAS_XCB  && HAS_XRENDER */
    #endif
  }
  #endif /* HAS_CAIRO */
}

/* w and h (if > 0 ) parameters force reset pixmap to w x h, regardless of window size */
void resetwin(int create_pixmap, int clear_pixmap, int force, int w, int h)
{
  unsigned int width, height;
  XWindowAttributes wattr;
  int status;
  #ifndef __unix__
  HWND hwnd = Tk_GetHWND(xctx->window);
  RECT rct;
  #endif

  if(has_x) {
    if(w && h) {
      width = w;
      height = h;
      status = 1;
    } else {
      #ifdef __unix__
      status = XGetWindowAttributes(display, xctx->window, &wattr);
      if(status) {
        width = wattr.width;
        height = wattr.height;
      }
      #else
      status = GetWindowRect(hwnd, &rct);
      if(status) {
        width = rct.right - rct.left;
        height = rct.bottom - rct.top;
      }
      #endif
    }
    if(status) {
      /* if(wattr.map_state==IsUnmapped) return; */
      xctx->xschem_w=width;
      xctx->xschem_h=height;
      xctx->areax2 = xctx->xschem_w+2*INT_WIDTH(xctx->lw);
      xctx->areay2 = xctx->xschem_h+2*INT_WIDTH(xctx->lw);
      xctx->areax1 = -2*INT_WIDTH(xctx->lw);
      xctx->areay1 = -2*INT_WIDTH(xctx->lw);
      xctx->areaw = xctx->areax2-xctx->areax1;
      xctx->areah = xctx->areay2-xctx->areay1;
      /* if no force avoid unnecessary work if no resize */
      if( force || xctx->xschem_w !=xctx->xrect[0].width || xctx->xschem_h !=xctx->xrect[0].height) {
        dbg(1, "resetwin(): create: %d, clear: %d, force: %d, xschem_w=%d xschem_h=%d\n",
                create_pixmap, clear_pixmap, force, xctx->xschem_w,xctx->xschem_h);
        dbg(1, "resetwin(): changing size\n\n");
        xctx->xrect[0].x = 0;
        xctx->xrect[0].y = 0;
        xctx->xrect[0].width = xctx->xschem_w;
        xctx->xrect[0].height = xctx->xschem_h;
        if(clear_pixmap) {
          resetcairo(0, 1, 1); /* create, clear, force */
          #ifdef __unix__
          XFreePixmap(display,xctx->save_pixmap);
          #else
          Tk_FreePixmap(display, xctx->save_pixmap);
          #endif
          XFreeGC(display,xctx->gctiled);
        }
        if(create_pixmap) {
          #ifdef __unix__
          xctx->save_pixmap = XCreatePixmap(display, xctx->window, xctx->xschem_w, xctx->xschem_h, screendepth);
          #else
          xctx->save_pixmap = Tk_GetPixmap(display, xctx->window, xctx->xschem_w, xctx->xschem_h, screendepth);
          #endif
          xctx->gctiled = XCreateGC(display,xctx->window,0L, NULL);
          XSetTile(display,xctx->gctiled, xctx->save_pixmap);
          XSetFillStyle(display,xctx->gctiled,FillTiled);
          /* whenever a pixmap is recreated all GC attributes must be reissued */
          change_linewidth(-1.0);
          resetcairo(1, 0, 1); /* create, clear, force */
        }
      }
    }
    if(xctx->pending_fullzoom) {
      zoom_full(0, 0, 1, 0.97);
      xctx->pending_fullzoom=0;
    }
    dbg(1, "resetwin(): Window reset\n");
  } /* end if(has_x) */
}

int Tcl_AppInit(Tcl_Interp *inter)
{
 char name[PATH_MAX]; /* overflow safe 20161122 */
 char tmp[2*PATH_MAX+100]; /* 20161122 overflow safe */
 int i;
 double l_width;
 struct stat buf;
 const char *home_buff;
 int running_in_src_dir;
 int fs;

 /* XVisualInfo vinfo; */
 #if HAS_XCB==1
 #if HAS_XRENDER==1
 xcb_render_query_pict_formats_reply_t *formats_reply;
 xcb_render_pictforminfo_t *formats;
 xcb_render_query_pict_formats_cookie_t formats_cookie;
 #endif
 #endif

 /* get PWD and HOME */
 if(!getcwd(pwd_dir, PATH_MAX)) {
   fprintf(errfp, "Tcl_AppInit(): getcwd() failed\n");
 }
#ifdef __unix__
 if ((home_buff = getenv("HOME")) == NULL) {
   home_buff = getpwuid(getuid())->pw_dir;
 }
#else
  change_to_unix_fn(pwd_dir);
  home_buff = getenv("USERPROFILE");
  change_to_unix_fn(home_buff);
#endif
 my_strncpy(home_dir, home_buff, S(home_dir));

 /* set error and exit handlers */
 XSetErrorHandler(err);
 interp=inter;
 Tcl_Init(interp);
 if(has_x) {
   Tk_Init(interp);
   tclsetvar("has_x","1");
 }
 Tcl_CreateExitHandler(tclexit, 0);
#ifdef __unix__
 my_snprintf(tmp, S(tmp),"regsub -all {~/} {%s} {%s/}", XSCHEM_LIBRARY_PATH, home_dir);
 tcleval(tmp);
 tclsetvar("XSCHEM_LIBRARY_PATH", tclresult());


 /* create user conf dir , remove ~ if present */
 my_snprintf(tmp, S(tmp),"regsub {^~/} {%s} {%s/}", USER_CONF_DIR, home_dir);
 tcleval(tmp);
 my_snprintf(user_conf_dir, S(user_conf_dir), "%s", tclresult());
 tclsetvar("USER_CONF_DIR", user_conf_dir);

 /* test if running xschem in src/ dir (usually for testing) */
 running_in_src_dir = 0;
 if( !stat("./xschem.tcl", &buf) && !stat("./systemlib", &buf) && !stat("./xschem", &buf)) {
   running_in_src_dir = 1;
   tclsetvar("XSCHEM_SHAREDIR",pwd_dir); /* for testing xschem builds in src dir*/

   /* set builtin library path if running in src/ */
   my_snprintf(tmp, S(tmp), 
      "set XSCHEM_LIBRARY_PATH %s/xschem_library", user_conf_dir);
   tcleval(tmp);
   my_snprintf(tmp, S(tmp), 
      "append XSCHEM_LIBRARY_PATH : [file dirname \"%s\"]/xschem_library/devices", pwd_dir);
   tcleval(tmp);
   my_snprintf(tmp, S(tmp), 
      "append XSCHEM_LIBRARY_PATH : [file dirname \"%s\"]/xschem_library/examples", pwd_dir);
   tcleval(tmp);
   my_snprintf(tmp, S(tmp), 
      "append XSCHEM_LIBRARY_PATH : [file dirname \"%s\"]/xschem_library/ngspice", pwd_dir);
   tcleval(tmp);
   my_snprintf(tmp, S(tmp), 
      "append XSCHEM_LIBRARY_PATH : [file dirname \"%s\"]/xschem_library/logic", pwd_dir);
   tcleval(tmp);
   my_snprintf(tmp, S(tmp), 
      "append XSCHEM_LIBRARY_PATH : [file dirname \"%s\"]/xschem_library/xschem_simulator", pwd_dir);
   tcleval(tmp);
   my_snprintf(tmp, S(tmp), 
      "append XSCHEM_LIBRARY_PATH : [file dirname \"%s\"]/xschem_library/binto7seg", pwd_dir);
   tcleval(tmp);
   my_snprintf(tmp, S(tmp), 
      "append XSCHEM_LIBRARY_PATH : [file dirname \"%s\"]/xschem_library/pcb", pwd_dir);
   tcleval(tmp);
   my_snprintf(tmp, S(tmp), 
      "append XSCHEM_LIBRARY_PATH : [file dirname \"%s\"]/xschem_library/rom8k", pwd_dir);
   tcleval(tmp);
 } else if( !stat(XSCHEM_SHAREDIR, &buf) ) {
   tclsetvar("XSCHEM_SHAREDIR",XSCHEM_SHAREDIR);
   /* ... else give up searching, may set later after loading xschemrc */
 }
#else
 char install_dir[MAX_PATH]="";
 char *up_hier=NULL, *win_xschem_library_path=NULL;
 #define WIN_XSCHEM_LIBRARY_PATH_NUM 9
 const char *WIN_XSCHEM_LIBRARY_PATH[WIN_XSCHEM_LIBRARY_PATH_NUM] = {
   /*1*/ "xschem_library",
   /*2*/ "xschem_library/devices", 
   /*3*/ "xschem_library/examples", 
   /*4*/ "xschem_library/ngspice", 
   /*5*/ "xschem_library/logic", 
   /*6*/ "xschem_library/xschem_simulator",
   /*7*/ "xschem_library/binto7seg", 
   /*8*/ "xschem_library/pcb", 
   /*9*/ "xschem_library/rom8k" };
 GetModuleFileNameA(NULL, install_dir, MAX_PATH);
 change_to_unix_fn(install_dir);
 int dir_len=strlen(install_dir);
 if (dir_len>11)
   install_dir[dir_len-11] = '\0'; /* 11 = remove /xschem.exe */
 /* debugging in Visual Studio will not have bin */
 my_snprintf(tmp, S(tmp), "regexp {bin$} \"%s\"", install_dir);
 tcleval(tmp);
 running_in_src_dir = 0;
 if (atoi(tclresult()) == 0)
 {
   running_in_src_dir = 1; /* no bin, so it's running in Visual studio source directory*/
   my_strdup(62, &up_hier, "../../..");
 }
 else my_strdup(63, &up_hier, "..");
 my_strcat(415, &win_xschem_library_path, ".");
 for (i = 0; i < WIN_XSCHEM_LIBRARY_PATH_NUM; ++i) {
   my_snprintf(tmp, S(tmp),"%s/%s/%s", install_dir, up_hier, WIN_XSCHEM_LIBRARY_PATH[i]);
   my_strcat(416, &win_xschem_library_path, "\;"); 
   my_strcat(431, &win_xschem_library_path, tmp);
 }
 my_snprintf(tmp, S(tmp), "set tmp2 \"%s\"; "
   "while {[regsub {([^/]*\\.*[^./]+[^/]*)/\\.\\./?} $tmp2 {} tmp2]} {}; ", win_xschem_library_path); 
 const char *result2 = tcleval(tmp);
 const char *win_xschem_library_path_clean = tclgetvar("tmp2");
 tclsetvar("XSCHEM_LIBRARY_PATH", win_xschem_library_path_clean);
 my_free(432, &win_xschem_library_path);
 my_free(441, &up_hier);
 char *xschem_sharedir=NULL;
 if ((xschem_sharedir=getenv("XSCHEM_SHAREDIR")) != NULL) {
   if (!stat(xschem_sharedir, &buf)) {
     tclsetvar("XSCHEM_SHAREDIR", xschem_sharedir);
   }
 }
 else {
   if (running_in_src_dir ==1) {
     /* pwd_dir can be working directory specified as "Working Directory" in settings */
     my_snprintf(tmp, S(tmp), "%s/../../../src", install_dir);
   }
   else {
     my_snprintf(tmp, S(tmp), "%s/../share", install_dir);
   }
   tclsetvar("XSCHEM_SHAREDIR", tmp);
 }
 /* create user conf dir */
 my_snprintf(user_conf_dir, S(user_conf_dir), "%s/xschem", home_dir); /* create user_conf root directory first */
 if (stat(user_conf_dir, &buf)) {
   if (!mkdir(user_conf_dir, 0700)) {
     dbg(1, "Tcl_AppInit(): created root directory to setup and create for user conf dir: %s\n", user_conf_dir);
   }
   else {
     fprintf(errfp, "Tcl_AppInit(): failure creating %s\n", user_conf_dir);
     Tcl_Exit(EXIT_FAILURE);
   }
 }
 my_snprintf(user_conf_dir, S(user_conf_dir), "%s/xschem/%s", home_dir, USER_CONF_DIR);
 tclsetvar("USER_CONF_DIR", user_conf_dir);
#endif

 /* create USER_CONF_DIR if it was not installed */
 if(stat(user_conf_dir, &buf)) {
   if(!mkdir(user_conf_dir, 0700)) {
     dbg(1, "Tcl_AppInit(): created %s dir\n", user_conf_dir);
   } else {
    fprintf(errfp, "Tcl_AppInit(): failure creating %s\n", user_conf_dir);
    Tcl_Exit(EXIT_FAILURE);
   }
 }

 /*                         */
 /*    SOURCE xschemrc file */
 /*                         */
 if(load_initfile) {
   /* get xschemrc given om cmdline, in this case do *not* source any other xschemrc*/
   if(rcfile[0]) {
     my_snprintf(name, S(name), rcfile);
     if(stat(name, &buf) ) {
       /* rcfile given on cmdline is not existing */
       fprintf(errfp, "Tcl_AppInit() err 2: cannot find %s\n", name);
       Tcl_ResetResult(interp);
       Tcl_Exit(EXIT_FAILURE);
       return TCL_ERROR;
     }
     else {
       dbg(1, "Tcl_AppInit(): sourcing %s\n", name);
       source_tcl_file(name);
     }
   }
   else {
     /* get systemwide xschemrc ... */
     if(tclgetvar("XSCHEM_SHAREDIR")) {
       my_snprintf(name, S(name), "%s/xschemrc",tclgetvar("XSCHEM_SHAREDIR"));
       if(!stat(name, &buf)) {
         dbg(1, "Tcl_AppInit(): sourcing %s\n", name);
         source_tcl_file(name);
       }
     }
     /* ... then source xschemrc in present directory if existing ... */
     if(!running_in_src_dir) {
       my_snprintf(name, S(name), "%s/xschemrc",pwd_dir);
       if(!stat(name, &buf)) {
         dbg(1, "Tcl_AppInit(): sourcing %s\n", name);
         source_tcl_file(name);
       } else {
         /* ... or look for (user_conf_dir)/xschemrc */
         my_snprintf(name, S(name), "%s/xschemrc", user_conf_dir);
         if(!stat(name, &buf)) {
           dbg(1, "Tcl_AppInit(): sourcing %s\n", name);
           source_tcl_file(name);
         }
       }
     }
   }
 }
 /* END SOURCING xschemrc */

 if(rainbow_colors) tclsetvar("rainbow_colors","1");

 /*                               */
 /*  START LOOKING FOR xschem.tcl */
 /*                               */
 if(!tclgetvar("XSCHEM_SHAREDIR")) {
   fprintf(errfp, "Tcl_AppInit() err 3: cannot find xschem.tcl\n");
   if(has_x) {
     tcleval( "wm withdraw .");
     tcleval(
       "tk_messageBox -icon error -type ok -message \"Tcl_AppInit() err 3: xschem.tcl not found, "
       "you are probably missing XSCHEM_SHAREDIR\"");
   }
   Tcl_ResetResult(interp);
   Tcl_AppendResult(interp, "Tcl_AppInit() err 3: xschem.tcl not found, "
                            "you are probably missing XSCHEM_SHAREDIR",NULL);
   Tcl_Exit(EXIT_FAILURE);
   return TCL_ERROR;
 }
 /*  END LOOKING FOR xschem.tcl   */

 dbg(1, "Tcl_AppInit(): XSCHEM_SHAREDIR=%s  XSCHEM_LIBRARY_PATH=%s\n",
       tclgetvar("XSCHEM_SHAREDIR"),
       tclgetvar("XSCHEM_LIBRARY_PATH") ? tclgetvar("XSCHEM_LIBRARY_PATH") : "NULL"
 );
 dbg(1, "Tcl_AppInit(): done step a of xinit()\n");

 /*                                */
 /* CREATE XSCHEM 'xschem' COMMAND */
 /*                                */
 Tcl_CreateCommand(interp, "xschem",   (myproc *) xschem, NULL, NULL);

 dbg(1, "Tcl_AppInit(): done step a1 of xinit()\n");


 /* Execute tcl script given on command line with --tcl */
 if(tcl_command) {
   tcleval(tcl_command);
 }


 /* set tcp port given in cmdline if any */
 if(tcp_port > 0) {
   if(tcp_port < 1024) fprintf(errfp, "please use port numbers >=1024 on command line\n");
   else {
     my_snprintf(name, S(name), "set xschem_listen_port %d", tcp_port);
     tcleval(name);
   }
 }
 /*                                */
 /*  EXECUTE xschem.tcl            */
 /*                                */
 my_snprintf(name, S(name), "%s/%s", tclgetvar("XSCHEM_SHAREDIR"), "xschem.tcl");
 if(stat(name, &buf) ) {
   fprintf(errfp, "Tcl_AppInit() err 4: cannot find %s\n", name);
   if(has_x) {
     tcleval( "wm withdraw .");
     tcleval(
       "tk_messageBox -icon error -type ok -message \"Tcl_AppInit() err 4: xschem.tcl not found, "
         "installation problem or undefined  XSCHEM_SHAREDIR\"");
   }
   Tcl_ResetResult(interp);
   Tcl_AppendResult(interp, "Tcl_AppInit() err 4: xschem.tcl not found, "
                            "you are probably missing XSCHEM_SHAREDIR\n",NULL);
   Tcl_Exit(EXIT_FAILURE);
   return TCL_ERROR;
 }
 dbg(1, "Tcl_AppInit(): sourcing %s\n", name);
 source_tcl_file(name);
 dbg(1, "Tcl_AppInit(): done executing xschem.tcl\n");
 /*                                */
 /*  END EXECUTE xschem.tcl        */
 /*                                */

 /* resolve absolute pathname of xschem (argv[0]) for future usage */
 my_strdup(44, &xschem_executable, get_file_path(xschem_executable));
 dbg(1, "Tcl_AppInit(): resolved xschem_executable=%s\n", xschem_executable);


 /* get xschem globals from tcl variables set in xschemrc/xschem.tcl */
 cairo_font_line_spacing = tclgetdoublevar("cairo_font_line_spacing");
 if(color_ps==-1)
   color_ps=atoi(tclgetvar("color_ps"));
 else  {
   my_snprintf(tmp, S(tmp), "%d",color_ps);
   tclsetvar("color_ps",tmp);
 }
 l_width=atoi(tclgetvar("line_width"));
 if(tclgetboolvar("change_lw")) l_width = 0.0;
 cadlayers=atoi(tclgetvar("cadlayers"));
 if(debug_var==-10) debug_var=0;
 my_snprintf(tmp, S(tmp), "%.16g",CADGRID);
 Tcl_VarEval(interp, "set_ne cadgrid ", tmp, NULL);
 my_snprintf(tmp, S(tmp), "%.16g",CADSNAP);
 Tcl_VarEval(interp, "set_ne cadsnap ", tmp, NULL);
 cairo_vert_correct = tclgetdoublevar("cairo_vert_correct");
 nocairo_vert_correct = tclgetdoublevar("nocairo_vert_correct");
 cairo_font_scale = tclgetdoublevar("cairo_font_scale");

 /*                             */
 /*  [m]allocate dynamic memory */
 /*                             */
 alloc_xschem_data("");
 /* global context / graphic preferences/settings */
 pixdata=my_calloc(641, cadlayers, sizeof(char*));
 for(i=0;i<cadlayers;i++)
 {
   pixdata[i]=my_calloc(642, 32, sizeof(char));
 }
 pixmap=my_calloc(636, cadlayers, sizeof(Pixmap));

 my_strncpy(xctx->plotfile, cli_opt_plotfile, S(xctx->plotfile));
 xctx->draw_window = tclgetintvar("draw_window");
 xctx->only_probes = tclgetintvar("only_probes");

 /* set global variables fetching data from tcl code */
 if(cli_opt_netlist_type) {
   xctx->netlist_type = cli_opt_netlist_type;
   set_tcl_netlist_type();
 } else {
   const char *n;
   n = tclgetvar("netlist_type");
   if(!strcmp(n, "spice")) xctx->netlist_type = CAD_SPICE_NETLIST;
   else if(!strcmp(n, "vhdl")) xctx->netlist_type = CAD_VHDL_NETLIST;
   else if(!strcmp(n, "verilog")) xctx->netlist_type = CAD_VERILOG_NETLIST;
   else if(!strcmp(n, "tedax")) xctx->netlist_type = CAD_TEDAX_NETLIST;
   else if(!strcmp(n, "symbol")) xctx->netlist_type = CAD_SYMBOL_ATTRS;
 }
 init_pixdata();/* populate xctx->fill_type array that is used in create_gc() to set fill styles */
 init_color_array(0.0, 0.0);
 my_snprintf(tmp, S(tmp), "%d",debug_var);
 tclsetvar("debug_var",tmp );
 tclsetvar("menu_debug_var",debug_var ? "1" : "0" );
 if(cli_opt_flat_netlist) {
   tclsetvar("flat_netlist","1");
   xctx->flat_netlist = 1;
 }
 xctx->xschem_w = CADWIDTH;
 xctx->xschem_h = CADHEIGHT;
 xctx->areaw = CADWIDTH+4*INT_WIDTH(xctx->lw);  /* clip area extends 1 pixel beyond physical xctx->window area */
 xctx->areah = CADHEIGHT+4*INT_WIDTH(xctx->lw); /* to avoid drawing clipped rectangle borders at xctx->window edges */
 xctx->areax1 = -2*INT_WIDTH(xctx->lw);
 xctx->areay1 = -2*INT_WIDTH(xctx->lw);
 xctx->areax2 = xctx->areaw-2*INT_WIDTH(xctx->lw);
 xctx->areay2 = xctx->areah-2*INT_WIDTH(xctx->lw);
 xctx->xrect[0].x = 0;
 xctx->xrect[0].y = 0;
 xctx->xrect[0].width = CADWIDTH;
 xctx->xrect[0].height = CADHEIGHT;


 my_strncpy(xctx->file_version, XSCHEM_FILE_VERSION, S(xctx->file_version));
 /* compile_font() needs enabled layers, these are set after xschem.tcl loading completed
  * so we temporarily enable all them here */
 for(i = 0; i < cadlayers; i++) xctx->enable_layer[i] = 1;
 compile_font();
 /* restore current dir after loading font */
 if(getenv("PWD")) {
   /* $env(PWD) better than pwd_dir as it does not dereference symlinks */
   my_strncpy(xctx->current_dirname, getenv("PWD"), S(xctx->current_dirname));
 } else {
   my_strncpy(xctx->current_dirname, pwd_dir, S(xctx->current_dirname));
 }

 /*                      */
 /*  X INITIALIZATION    */
 /*                      */
 if( has_x ) {
    mainwindow=Tk_MainWindow(interp);
    if(!mainwindow) {
       fprintf(errfp, "Tcl_AppInit() err 6: Tk_MainWindow returned NULL...\n");
       return TCL_ERROR;
    }
    display = Tk_Display(mainwindow);
    tkwindow = Tk_NameToWindow(interp, ".drw", mainwindow);
    Tk_MakeWindowExist(tkwindow);
    xctx->window = Tk_WindowId(tkwindow);
    topwindow = Tk_WindowId(mainwindow);

    dbg(1, "Tcl_AppInit(): drawing window ID=0x%lx\n",xctx->window);
    dbg(1, "Tcl_AppInit(): top window ID=0x%lx\n",topwindow);
    dbg(1, "Tcl_AppInit(): done tkinit()\n");

    #if HAS_XCB==1
    /* grab an existing xlib connection  20171125 */
    xcbconn = XGetXCBConnection(display);
    if(xcb_connection_has_error(xcbconn)) {
      fprintf(errfp, "Could not connect to X11 server");
      return 1;
    }
    screen_xcb = xcb_setup_roots_iterator(xcb_get_setup(xcbconn)).data;
    visual_xcb = find_visual(xcbconn, screen_xcb->root_visual);
    if(!visual_xcb) {
      fprintf(errfp, "got NULL (xcb_visualtype_t)visual");
      return 1;
    }
    #if HAS_XRENDER==1
    /*/--------------------------Xrender xcb  stuff------- */
    formats_cookie = xcb_render_query_pict_formats(xcbconn);
    formats_reply = xcb_render_query_pict_formats_reply(xcbconn, formats_cookie, 0);

    formats = xcb_render_query_pict_formats_formats(formats_reply);
    for (i = 0; i < formats_reply->num_formats; i++) {
             /* fprintf(errfp, "i=%d depth=%d  type=%d red_shift=%d\n", i,
                  formats[i].depth, formats[i].type, formats[i].direct.red_shift); */
            if (formats[i].direct.red_mask != 0xff &&
                formats[i].direct.red_shift != 16)
                    continue;
            if (formats[i].type == XCB_RENDER_PICT_TYPE_DIRECT &&
                formats[i].depth == 24 && formats[i].direct.red_shift == 16)
                    format_rgb = formats[i];
            if (formats[i].type == XCB_RENDER_PICT_TYPE_DIRECT &&
                formats[i].depth == 32 &&
                formats[i].direct.alpha_mask == 0xff &&
                formats[i].direct.alpha_shift == 24)
                    format_rgba = formats[i];
    }
    free(formats_reply); /* can not use my_free() since not allocated by my_* allocator wrappers */
    formats_reply = NULL;
    #endif /* HAS_XRENDER */
    /*/---------------------------------------------------- */
    /* /20171125 */
    #endif /*HAS_XCB */

    screen_number = Tk_ScreenNumber(mainwindow);
    colormap = Tk_Colormap(mainwindow);
    screendepth = Tk_Depth(mainwindow);
    dbg(1, "Tcl_AppInit(): screen depth: %d\n",screendepth);
    visual = Tk_Visual(mainwindow);

    /*
    if (!XMatchVisualInfo(
        display, XDefaultScreen(display), 24, TrueColor, &vinfo)
    ) return fprintf(errfp, "no 32 bit visual\n");
    visual = vinfo.visual;
    */
    dbg(1, "Tcl_AppInit(): done step b of xinit()\n");

    create_gc();

    dbg(1, "Tcl_AppInit(): done step c of xinit()\n");
    if(build_colors(0.0, 0.0)) exit(-1);
    dbg(1, "Tcl_AppInit(): done step e of xinit()\n");
    /* xctx->save_pixmap must be created as resetwin() frees it before recreating with new size. */

    #if HAS_XRENDER==1
    render_format = XRenderFindStandardFormat(display, PictStandardRGB24);
    #endif
    resetwin(1, 0, 1, 0, 0);
    #if HAS_CAIRO==1
    /* load font from tcl 20171112 */
    tclsetvar("has_cairo","1");
    #endif

    change_linewidth(l_width);
    dbg(1, "Tcl_AppInit(): done xinit()\n");
    /* Set backing store window attribute */
    winattr.backing_store = WhenMapped;
    /* winattr.backing_store = NotUseful; */
    Tk_ChangeWindowAttributes(tkwindow, CWBackingStore, &winattr);

    dbg(1, "Tcl_AppInit(): sizeof xInstance=%lu , sizeof xSymbol=%lu\n",
             (unsigned long) sizeof(xInstance),(unsigned long) sizeof(xSymbol));

#ifdef __unix__
    dbg(1, "Tcl_AppInit(): xserver max request size: %d\n",
                             (int)XMaxRequestSize(display));
#else
    dbg(1, "Tcl_AppInit(): xserver max request size:\n");
#endif

    set_snap(0); /* set default value specified in xschemrc as 'snap' else CADSNAP */
    set_grid(0); /* set default value specified in xschemrc as 'grid' else CADGRID */
 } /* if(has_x) */
 dbg(1, "Tcl_AppInit(): done X init\n");

/* pass to tcl values of Alt, Shift, COntrol key masks so bind Alt-KeyPress events will work for windows */
#ifndef __unix__
 my_snprintf(tmp, S(tmp), "%d", Mod1Mask);
 tclsetvar("Mod1Mask", tmp);
 my_snprintf(tmp, S(tmp), "%d", ShiftMask);
 tclsetvar("ShiftMask", tmp);
 my_snprintf(tmp, S(tmp), "%d", ControlMask);
 tclsetvar("ControlMask", tmp);
#endif


 /*  END X INITIALIZATION */


 init_done=1;  /* 20171008 moved before option processing, otherwise xwin_exit will not be invoked */
               /* leaving undo buffer and other garbage around. */

 /*                                                                                  */
 /* Completing tk windows creation (see xschem.tcl, pack_widgets) and event binding */
 /* *AFTER* X initialization done                                                    */
 /*                                                                                  */
 if(has_x) {
   tcleval("pack_widgets; set_bindings .drw");
 }

 fs=tclgetintvar("fullscreen");
 if(fs) {
   tclsetvar("fullscreen", "0");
   tcleval("update");
   toggle_fullscreen(".");
 }

 /*                                */
 /*  START PROCESSING USER OPTIONS */
 /*                                */

 /* set tcl netlist_dir if netlist_dir given on cmdline */
 if(cli_opt_netlist_dir[0]) tclsetvar("netlist_dir", cli_opt_netlist_dir);

 if(!set_netlist_dir(0, NULL)) {
   const char *n;
   n = tclgetvar("netlist_dir");
   fprintf(errfp, "problems creating netlist directory %s\n", n ? n : "<NULL>");
 }
 if(initial_netlist_name[0]) my_strncpy(xctx->netlist_name, initial_netlist_name, S(initial_netlist_name));

 enable_layers();

 /* prefer using env(PWD) if existing since it does not dereference symlinks */
 if(tcleval("info exists env(PWD)")[0] == '1') {
   my_snprintf(pwd_dir, S(pwd_dir), "%s", tclgetvar("env(PWD)"));
 }

 if(filename) {
    char f[PATH_MAX];

#ifdef __unix__
   if(filename[0] == '~' && filename[1] == '/') {
     my_snprintf(f, S(f), "%s%s", home_dir, filename + 1);
   } else if(filename[0] == '.' && filename[1] == '/') {
     my_snprintf(f, S(f), "%s%s", pwd_dir, filename + 1);
   } else if(filename[0] !='/') {
     my_snprintf(f, S(f), "%s/%s", pwd_dir, filename);
   } else {
     my_snprintf(f, S(f), "%s", filename);
   }
#else
   my_strncpy(f, abs_sym_path(filename, ""), S(f));
#endif
   dbg(1, "Tcl_AppInit(): filename %s given, removing symbols\n", filename);
   remove_symbols();
   /* if do_netlist=1 call load_schematic with 'reset_undo=0' avoiding call 
      to tcl is_xschem_file that could change xctx->netlist_type to symbol */
   load_schematic(1, f, !do_netlist);
   Tcl_VarEval(interp, "update_recent_file {", f, "}", NULL);
 } else if(!tcl_script[0]) {
   char * tmp;
   char filename[PATH_MAX];
   tmp = (char *) tclgetvar("XSCHEM_START_WINDOW");
   dbg(1, "Tcl_AppInit(): tmp=%s\n", tmp? tmp: "NULL");
   my_strncpy(filename, abs_sym_path(tmp, ""), S(filename));
    /* if do_netlist=1 call load_schematic with 'reset_undo=0' avoiding call 
       to tcl is_xschem_file that could change xctx->netlist_type to symbol */
   load_schematic(1, filename, !do_netlist);
 }

 tclsetintvar("max_new_windows", MAX_NEW_WINDOWS);

 zoom_full(0, 0, 1, 0.97);   /* Necessary to tell xschem the initial area to display */
 xctx->pending_fullzoom=1;
 if(do_netlist) {
   if(debug_var>=1) {
     if(xctx->flat_netlist)
       fprintf(errfp, "xschem: flat netlist requested\n");
   }
   if(!filename) {
     fprintf(errfp, "xschem: cant do a netlist without a filename\n");
     tcleval("exit");
   }
   if(tclgetvar("netlist_dir")[0]) {
     if(xctx->netlist_type == CAD_SPICE_NETLIST)
       global_spice_netlist(1);                  /* 1 means global netlist */
     else if(xctx->netlist_type == CAD_VHDL_NETLIST)
       global_vhdl_netlist(1);                   /* 1 means global netlist */
     else if(xctx->netlist_type == CAD_VERILOG_NETLIST)
       global_verilog_netlist(1);                /* 1 means global netlist */
     else if(xctx->netlist_type == CAD_TEDAX_NETLIST)
       global_tedax_netlist(1);                  /* 1 means global netlist */
   } else {
    fprintf(errfp, "xschem: please set netlist_dir in xschemrc\n");
   }
 }
 if(do_print) {
   if(!filename) {
     dbg(0, "xschem: can't do a print without a filename\n");
     tcleval("exit");
   }
   if(do_print==1) {

     xctx->xrect[0].x = 0;
     xctx->xrect[0].y = 0;
     xctx->xschem_w = xctx->xrect[0].width = 842;
     xctx->xschem_h = xctx->xrect[0].height = 595;
     xctx->areax2 = 842+2;
     xctx->areay2 = 595+2;
     xctx->areax1 = -2;
     xctx->areay1 = -2;
     xctx->areaw = xctx->areax2-xctx->areax1;
     xctx->areah = xctx->areay2-xctx->areay1;
     zoom_full(0, 0, 2, 0.97);
     ps_draw(7);
   } else if(do_print == 2) {
     if(!has_x) {
       dbg(0, "xschem: can not do a png export if no X11 present / Xserver running (check if DISPLAY set).\n");
     } else {
       tcleval("tkwait visibility .drw");
       print_image();
     }
   }
   else svg_draw();
 }

 if(do_simulation) {
   if(!filename) {
     fprintf(errfp, "xschem: can't do a simulation without a filename\n");
     tcleval("exit");
   }
   tcleval( "simulate");
 }

 if(do_waves) {
   if(!filename) {
     fprintf(errfp, "xschem: can't show simulation waves without a filename\n");
     tcleval("exit");
   }
   tcleval( "waves [file tail \"[xschem get schname]\"]");
 }

 /* source tcl file given on command line with --script */
 if(tcl_script[0]) {
   Tcl_VarEval(interp, "update; source ", tcl_script, NULL);
 }

 if(quit) {
   tcleval("exit");
 }

 /* */
 /*  END PROCESSING USER OPTIONS */
 /* */


 if(
#ifdef __unix__
    !detach &&
#endif
    !no_readline) {
   tcleval( "if {![catch {package require tclreadline}]} "
     "{::tclreadline::readline builtincompleter 0;"
     /*  "::tclreadline::readline customcompleter completer;" */
      "::tclreadline::Loop }" 
   );
 }

 dbg(1, "Tcl_AppInit(): returning TCL_OK\n");
 return TCL_OK;
}



