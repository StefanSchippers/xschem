/* File: xinit.c
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2023 Stefan Frederik Schippers
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

/* variables for handling multiple windows/tabs */
static Xschem_ctx *save_xctx[MAX_NEW_WINDOWS]; /* save pointer to current schematic context structure */
static char window_path[MAX_NEW_WINDOWS][WINDOW_PATH_SIZE];
/* ==0 if no additional windows/tabs, ==1 if one additional window/tab, ... */
static int window_count = 0;
static int last_created_window = -1;

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

Xschem_ctx **get_save_xctx(void)
{
  return save_xctx;
}

/* array of tab names to be matched with xctx->current_win_path
 * window_path[0] = .drw
 * window_path[1] = .x3.drw
 * window_path[2] = .x4.drw
 * ...
 */ 
char *get_window_path(int i)
{
  return window_path[i];
}

char *get_last_created_window(void)
{
  if(last_created_window >= 0) {
    return window_path[last_created_window];
  } else {
    return "";
  }
}

int get_window_count(void)
{
  return window_count;
}

static int window_state (Display *disp, Window win, char *arg) {/*{{{*/
    char arg_copy[256]; /* overflow safe */
    unsigned long action;
    int i;
    Atom prop1 = 0;
    Atom prop2 = 0;
    char *p1, *p2;
    const char *argerr = "expects a list of comma separated parameters: "
                         "\"(remove|add|toggle),<PROP1>[,<PROP2>]\"\n";

    char tmp_prop1[256], tmp1[256]; /* overflow safe */
    char tmp_prop2[256], tmp2[256]; /* overflow safe */


    if (!arg || strlen(arg) == 0) {
        fputs(argerr, errfp);
        return EXIT_FAILURE;
    }
    dbg(1,"window_state() , win=0x%x arg=%s\n", (int)win, arg);
    my_strncpy(arg_copy, arg, S(arg_copy));
    if ((p1 = strchr(arg_copy, ','))) {

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
            *p2 = '\0';
            p2++;
            if (strlen(p2) == 0) {
                fputs("Invalid zero length property.\n", errfp);
                return EXIT_FAILURE;
            }
            for( i = 0; p2[i]; ++i) tmp2[i] = (char)toupper( p2[i] );
            tmp2[i] = '\0';
            my_snprintf(tmp_prop2, S(tmp_prop2), "_NET_WM_STATE_%s", tmp2);
            prop2 = XInternAtom(disp, tmp_prop2, False);
        }

        /* the first property */
        if (strlen(p1) == 0) {
            fputs("Invalid zero length property.\n", errfp);
            return EXIT_FAILURE;
        }
        for( i = 0; p1[i]; ++i) tmp1[i] = (char)toupper( p1[i] );
        tmp1[i] = '\0';
        my_snprintf(tmp_prop1, S(tmp_prop1), "_NET_WM_STATE_%s", tmp1);
        prop1 = XInternAtom(disp, tmp_prop1, False);
        dbg(1, "window_state(): issuing client_msg((disp, win, \"_NET_WM_STATE\", %d, %d, %d\n",
            action, prop1, prop2);

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
#ifdef __unix__
  int i;
#endif
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
  tclvareval("winfo id ", winpath, NULL);
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

static int err(Display *display, XErrorEvent *xev)
{
 int l=250;
#ifdef __unix__
 char s[1024];  /* overflow safe 20161122 */
 XGetErrorText(display, xev->error_code, s,l);
 dbg(1, "err(): Err %d :%s maj=%d min=%d\n", xev->error_code, s, xev->request_code,
          xev->minor_code);
#endif
 return 0;
}

static unsigned int  find_best_color(char colorname[])
{
#ifdef __unix__
 int i;
 double distance=10000000000.0, dist, r, g, b, red, green, blue;
 double deltar,deltag,deltab;
 unsigned int idx;
/*  dbg(1, "find_best_color() start: %g\n", timer(1)); */

 if( XAllocNamedColor(display, colormap, colorname, &xcolor_exact, &xcolor) ==0 )
 {
  for(i=0;i<=255; ++i) {
    xctx->xcolor_array[i].pixel=i;
    XQueryColor(display, colormap, xctx->xcolor_array+i);
  }
  /* debug ... */
  dbg(2, "find_best_color(): Server failed to allocate requested color, finding substitute\n");
  XLookupColor(display, colormap, colorname, &xcolor_exact, &xcolor);
  red = xcolor.red; green = xcolor.green; blue = xcolor.blue;
  idx=0;
  for(i = 0;i<=255; ++i)
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
  idx = (int)xcolor.pixel;
 }
 /* dbg(1, "find_best_color() return: %g\n", timer(1)); */
 return idx;
#else
 Tk_Window mainwindow = Tk_MainWindow(interp);
 XColor *xc = Tk_GetColor(interp, mainwindow, colorname);
 /* if (XAllocColor(display, colormap, xc)) return(xc->pixel); */
 return xc->pixel;
#endif
}


static void init_color_array(double dim, double dim_bg)
{
 char s[256]; /* overflow safe 20161122 */
 int i;
 unsigned int r, g, b;
 double tmp_dim;

 for(i=0;i<cadlayers; ++i) {
   my_snprintf(s, S(s), "lindex $tctx::colors %d",i);
   tcleval(s);
   dbg(2, "init_color_array(): color:%s\n",tclresult());

   sscanf(tclresult(), "#%02x%02x%02x", &r, &g, &b);
   if(i == BACKLAYER) tmp_dim = dim_bg;
   else tmp_dim = dim;
   if(tmp_dim>=0.) {
     r +=(int)((51.-r/5.)*tmp_dim);
     g +=(int)((51.-g/5.)*tmp_dim);
     b +=(int)((51.-b/5.)*tmp_dim);
   } else {
     r +=(int)((r/5.)*tmp_dim);
     g +=(int)((g/5.)*tmp_dim);
     b +=(int)((b/5.)*tmp_dim);
   }
   /* fprintf(errfp, "init_color_array: colors: %.16g %.16g %.16g dim=%.16g c=%d\n", r, g, b, dim, i); */
   if(r>0xff) r=0xff;
   if(g>0xff) g=0xff;
   if(b>0xff) b=0xff;
   my_snprintf(s, S(s), "#%02x%02x%02x", r, g, b);
   my_strdup(_ALLOC_ID_, &xctx->color_array[i], s);
 }
}

void init_pixdata()/* populate xctx->fill_type array that is used in create_gc() to set fill styles */
{
 int i,j, full, empty;
 const char *tclpixdata;
 const char *tclword;
 int found_data;
  
 for(i=0;i<cadlayers; ++i) {
   tclpixdata = Tcl_GetVar2(interp, "pixdata", my_itoa(i), TCL_GLOBAL_ONLY);
   dbg(1, "pixdata(%d)=%s\n", i, tclpixdata);
   full=1; empty=1;
   for(j=0;j<32; ++j) {
     found_data = 0;
     if(tclpixdata) {
       tclvareval("lindex {", tclpixdata, "} ", my_itoa(j >> 1), NULL);
       tclword = tclresult();
       if(tclword[0]) {
         unsigned int word;
         unsigned char byte;
         sscanf(tclword, "%x", &word);
         if(j%2) byte = word & 0xff;
         else    byte = (word >> 8) & 0xff;
         dbg(1, "byte=%02x\n", byte);
         found_data = 1;
         pixdata[i][j] = byte;
       }
     }
     if(!found_data) {
       if(i<sizeof(pixdata_init)/sizeof(pixdata_init[0]))
         pixdata[i][j] = pixdata_init[i][j];
       else
         pixdata[i][j] = 0x00;
     }
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

static void free_xschem_data()
{
  int i;
  xctx->delete_undo();
  free_simdata();

  my_free(_ALLOC_ID_, &xctx->node_table);
  my_free(_ALLOC_ID_, &xctx->hilight_table);

  my_free(_ALLOC_ID_, &xctx->wire);
  my_free(_ALLOC_ID_, &xctx->text);
  my_free(_ALLOC_ID_, &xctx->inst);
  for(i=0;i<cadlayers; ++i) {
    my_free(_ALLOC_ID_, &xctx->rect[i]);
    my_free(_ALLOC_ID_, &xctx->line[i]);
    my_free(_ALLOC_ID_, &xctx->poly[i]);
    my_free(_ALLOC_ID_, &xctx->arc[i]);
  }
  my_free(_ALLOC_ID_, &xctx->sym);
  my_free(_ALLOC_ID_, &xctx->rect);
  my_free(_ALLOC_ID_, &xctx->line);
  my_free(_ALLOC_ID_, &xctx->poly);
  my_free(_ALLOC_ID_, &xctx->arc);
  my_free(_ALLOC_ID_, &xctx->rects);
  my_free(_ALLOC_ID_, &xctx->polygons);
  my_free(_ALLOC_ID_, &xctx->arcs);
  my_free(_ALLOC_ID_, &xctx->lines);
  my_free(_ALLOC_ID_, &xctx->maxr);
  my_free(_ALLOC_ID_, &xctx->maxp);
  my_free(_ALLOC_ID_, &xctx->maxa);
  my_free(_ALLOC_ID_, &xctx->maxl);
  my_free(_ALLOC_ID_, &xctx->sel_array);
  for(i=0;i<CADMAXHIER; ++i) {
    if(xctx->portmap[i].table) str_hash_free(&xctx->portmap[i]);
    if(xctx->sch[i]) my_free(_ALLOC_ID_, &xctx->sch[i]);
    if(xctx->sch_path[i]) my_free(_ALLOC_ID_, &xctx->sch_path[i]);
    if(xctx->hier_attr[i].templ) my_free(_ALLOC_ID_, &xctx->hier_attr[i].templ);
    if(xctx->hier_attr[i].prop_ptr) my_free(_ALLOC_ID_, &xctx->hier_attr[i].prop_ptr);
    if(xctx->hier_attr[i].symname) my_free(_ALLOC_ID_, &xctx->hier_attr[i].symname);
  }
    
  my_free(_ALLOC_ID_, &xctx->gridpoint);
  my_free(_ALLOC_ID_, &xctx->biggridpoint);
  my_free(_ALLOC_ID_, &xctx->gc);
  my_free(_ALLOC_ID_, &xctx->gcstipple);
  for(i=0;i<cadlayers; ++i) my_free(_ALLOC_ID_, &xctx->color_array[i]);
  my_free(_ALLOC_ID_, &xctx->color_array);
  my_free(_ALLOC_ID_, &xctx->enable_layer);
  my_free(_ALLOC_ID_, &xctx->active_layer);
  my_free(_ALLOC_ID_, &xctx->top_path);
  my_free(_ALLOC_ID_, &xctx->current_win_path);
  my_free(_ALLOC_ID_, &xctx->fill_type);
  my_free(_ALLOC_ID_, &xctx->format);
  my_free(_ALLOC_ID_, &xctx);
}

void create_gc(void)
{
  int i;
  for(i=0;i<cadlayers; ++i)
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
  for(i=0;i<cadlayers; ++i) {
    XFreeGC(display,xctx->gc[i]);
    XFreeGC(display,xctx->gcstipple[i]);
  }
}

static void alloc_xschem_data(const char *top_path, const char *win_path)
{
  int i, j;

  xctx = my_calloc(_ALLOC_ID_, 1, sizeof(Xschem_ctx));
  xctx->cur_undo_ptr = 0;
  xctx->head_undo_ptr = 0;
  xctx->tail_undo_ptr = 0;
  xctx->undo_dirname = NULL;
  xctx->infowindow_text = NULL;
  xctx->intuitive_interface = 0;

  if(!strcmp(tclgetvar("undo_type"), "disk")) {
    xctx->undo_type = 0;
    xctx->push_undo = push_undo;
    xctx->pop_undo = pop_undo;
    xctx->delete_undo = delete_undo;
    xctx->clear_undo = clear_undo;

  } else {
    xctx->undo_type = 1; /* "memory" */
    xctx->push_undo = mem_push_undo;
    xctx->pop_undo = mem_pop_undo;
    xctx->delete_undo = mem_delete_undo;
    xctx->clear_undo = mem_clear_undo;
  }
  xctx->undo_initialized = 0;
  xctx->zoom=CADINITIALZOOM;
  xctx->mooz=1/CADINITIALZOOM;
  xctx->xorigin=CADINITIALX;
  xctx->yorigin=CADINITIALY;
  xctx->raw = NULL;
  xctx->extra_idx = 0;
  xctx->extra_prev_idx = 0;
  xctx->extra_raw_n = 0;
  xctx->graph_master = 0;
  xctx->graph_cursor1_x = 0;
  xctx->graph_cursor2_x = 0;
  xctx->graph_flags = 0;
  xctx->graph_top = 0;
  xctx->graph_bottom = 0;
  xctx->graph_left = 0;
  xctx->graph_lastsel = -1;
  xctx->graph_struct.hilight_wave = -1; /* index of wave */
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
  xctx->header_text = NULL;
  xctx->rectcolor= 4;  /* this is the current layer when xschem started. */
  xctx->currsch = 0;
  xctx->constr_mv = 0; /* constrained move (vertical (2) / horizontal (1) ) */
  xctx->ui_state = 0;
  xctx->ui_state2 = 0;
  xctx->lw = 0.0;
  xctx->need_reb_sel_arr = 1;
  xctx->lastsel = 0;
  xctx->maxsel = 0;
  xctx->prep_net_structs = 0;
  xctx->prep_hi_structs = 0;
  xctx->simdata = NULL;
  xctx->simdata_ninst = 0;
  xctx->prep_hash_inst = 0;
  xctx->prep_hash_object = 0;
  xctx->prep_hash_wires = 0;
  xctx->modified = 0;
  xctx->semaphore = 0;
  xctx->paste_from = 0;
  xctx->current_name[0] = '\0';
  xctx->sch_to_compare[0] = '\0';
  xctx->tok_size = 0;
  xctx->netlist_name[0] = '\0';
  xctx->plotfile[0] = '\0';
  xctx->netlist_unconn_cnt = 0; /* unique count of unconnected pins while netlisting */
  xctx->current_dirname[0] = '\0';
  for(i = 0; i < NBOXES; ++i) {
    for(j = 0; j < NBOXES; ++j) {
      xctx->instpin_spatial_table[i][j] = NULL;
      xctx->wire_spatial_table[i][j] = NULL;
      xctx->inst_spatial_table[i][j] = NULL;
      xctx->object_spatial_table[i][j] = NULL;
    }
  }
  xctx->n_hash_objects = 0;
  xctx->node_table = my_calloc(_ALLOC_ID_,  HASHSIZE, sizeof(Node_hashentry *));
  xctx->inst_name_table.table = NULL;
  xctx->inst_name_table.size = 0;
  xctx->floater_inst_table.table = NULL;
  xctx->floater_inst_table.size = 0;
  xctx->hilight_table = my_calloc(_ALLOC_ID_,  HASHSIZE, sizeof(Hilight_hashentry *));

  xctx->window = xctx->save_pixmap = 0;
  xctx->xrect[0].width = xctx->xrect[0].height = xctx->xrect[0].x = xctx->xrect[0].y = 0;
#if HAS_CAIRO==1
  xctx->cairo_ctx = xctx->cairo_save_ctx = NULL;
  xctx->cairo_sfc = xctx->cairo_save_sfc = NULL;
  xctx->cairo_font = NULL;
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
  xctx->kissing = 0;
  xctx->connect_by_kissing = 0;
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
  xctx->nl_sel = xctx->nl_sem = xctx->nl_dir = 0;

  xctx->hilight_time = 0; /* timestamp for sims */
  xctx->shape_point_selected = 0;
  xctx->drag_elements = 0;
  xctx->hilight_nets = 0;
  xctx->hilight_color = 0;
  for(i=0;i<CADMAXHIER; ++i) {
    xctx->sch[i] = NULL;
    xctx->sch_path[i]=NULL;
    xctx->sch_path_hash[i]=0;
    xctx->hier_attr[i].prop_ptr = NULL;
    xctx->hier_attr[i].templ = NULL;
    xctx->hier_attr[i].symname = NULL;
    xctx->hier_attr[i].fd = NULL;
    xctx->portmap[i].table = NULL;
    xctx->portmap[i].size = 0;
  }
  my_strdup(_ALLOC_ID_, &xctx->sch_path[0],".");

  xctx->sch_inst_number[0] = 1;
  xctx->maxt=CADMAXTEXT;
  xctx->maxw=CADMAXWIRES;
  xctx->maxi=ELEMINST;
  xctx->maxs=ELEMDEF;
  xctx->text=my_calloc(_ALLOC_ID_, xctx->maxt,sizeof(xText));
  xctx->wire=my_calloc(_ALLOC_ID_, xctx->maxw,sizeof(xWire));
  xctx->inst=my_calloc(_ALLOC_ID_, xctx->maxi , sizeof(xInstance) );
  xctx->sym=my_calloc(_ALLOC_ID_, xctx->maxs , sizeof(xSymbol) );
  xctx->maxr=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  xctx->maxa=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  xctx->maxp=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  xctx->maxl=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  for(i=0;i<cadlayers; ++i)
  {
    xctx->maxr[i]=CADMAXOBJECTS;
    xctx->maxp[i]=CADMAXOBJECTS;
    xctx->maxl[i]=CADMAXOBJECTS;
    xctx->maxa[i]=CADMAXOBJECTS;
  }
  xctx->rect=my_calloc(_ALLOC_ID_, cadlayers, sizeof(xRect *));
  xctx->line=my_calloc(_ALLOC_ID_, cadlayers, sizeof(xLine *));
  xctx->poly=my_calloc(_ALLOC_ID_, cadlayers, sizeof(xPoly *));
  xctx->arc=my_calloc(_ALLOC_ID_, cadlayers, sizeof(xArc *));
  for(i=0;i<cadlayers; ++i)
  {
    xctx->rect[i]=my_calloc(_ALLOC_ID_, xctx->maxr[i],sizeof(xRect));
    xctx->arc[i]=my_calloc(_ALLOC_ID_, xctx->maxa[i],sizeof(xArc));
    xctx->poly[i]=my_calloc(_ALLOC_ID_, xctx->maxp[i],sizeof(xPoly));
    xctx->line[i]=my_calloc(_ALLOC_ID_, xctx->maxl[i],sizeof(xLine));
  }
  xctx->rects=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  xctx->polygons=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  xctx->arcs=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  xctx->lines=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  xctx->maxsel=MAXGROUP;
  xctx->sel_array=my_calloc(_ALLOC_ID_, xctx->maxsel, sizeof(Selected));
  xctx->first_sel.n = -1;
  xctx->first_sel.type = 0;
  xctx->first_sel.col = 0;
  xctx->biggridpoint=(XSegment*)my_calloc(_ALLOC_ID_, CADMAXGRIDPOINTS,sizeof(XSegment));
  xctx->gridpoint=(XPoint*)my_calloc(_ALLOC_ID_, CADMAXGRIDPOINTS,sizeof(XPoint));
  xctx->enable_drill = 0;
  xctx->prev_set_modify = -1;
  xctx->prev_crossx = xctx->prev_crossy = 0.0;
  xctx->mouse_inside = 0;
  xctx->pending_fullzoom = 0;
  my_strncpy(xctx->hiersep, ".", S(xctx->hiersep));
  xctx->no_undo = 0;
  xctx->draw_single_layer = -1;
  xctx->draw_dots = 1;
  xctx->only_probes = 0;
  xctx->menu_removed = 0; /* fullscreen previous setting */
  xctx->save_lw = 0.0;  /* used to save linewidth when selecting 'only_probes' view */
  xctx->already_selected = 0;
  xctx->onetime = 0; /* callback() static var */
  xctx->mouse_moved = 0; /* set to 0 on button1 press, set o 1 on mouse move */
  xctx->max_globals = 0;
  xctx->size_globals = 0;
  xctx->globals = NULL;
  xctx->save_netlist_type = 0;
  xctx->some_nets_added = 0;
  xctx->loaded_symbol = 0;
  xctx->no_draw = 0;
  xctx->bbox_set = 0; /* bbox */
  xctx->old_prop = NULL;
  xctx->edit_sym_i = -1;
  xctx->netlist_commands = 0;
  xctx->draw_pixmap = 1;
  xctx->gc=my_calloc(_ALLOC_ID_, cadlayers, sizeof(GC));
  xctx->gcstipple=my_calloc(_ALLOC_ID_, cadlayers, sizeof(GC));
  xctx->color_array=my_calloc(_ALLOC_ID_, cadlayers, sizeof(char*));
  xctx->enable_layer=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  xctx->crosshair_layer = TEXTLAYER;
  xctx->n_active_layers = 0;
  xctx->active_layer=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  xctx->hide_symbols = 0;
  xctx->netlist_type = CAD_SPICE_NETLIST;
  xctx->format = NULL; /* custom format string for netlist, otherwise use
                        * "format", "verilog_format", "vhdl_format", "tedax_format" */
  xctx->top_path = NULL;
  xctx->current_win_path = NULL;
  my_strdup2(_ALLOC_ID_, &xctx->top_path, top_path);
  my_strdup2(_ALLOC_ID_, &xctx->current_win_path, win_path);
  xctx->fill_type=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  xctx->case_insensitive = 0;
  xctx->show_hidden_texts = 0;
  xctx->x_strcmp = strcmp;
  xctx->fill_pattern = 1;
  xctx->draw_window = 0;
  xctx->do_copy_area = 1;
  xctx->time_last_modify = 0;
}

static void delete_schematic_data(int delete_pixmap)
{
  dbg(1, "delete_schematic_data()\n");
  unselect_all(1);
  /* clear static data in get_tok_value() must be done after unselect_all(1) 
   * as this functions re-uses get_tok_value() */
  parse_cmd_string(NULL, NULL); /* clear static data in function */
  get_tok_value(NULL, NULL, 0); /* clear static data in function */
  /* delete inst and wire node fields, delete inst_pin spatial hash, and node hash table */
  delete_netlist_structs();
  clear_all_hilights();      /* data structs for hilighting nets/instances */
  get_unnamed_node(0, 0, 0); /* net### enumerator used for netlisting */
  clear_drawing();
  if(has_x && delete_pixmap) {
    resetwin(0, 1, 1, 0, 0);  /* delete preview pixmap, delete cairo surfaces */
    free_gc();
  }
  /* delete instances, wires, lines, rects, arcs, polys, texts, hash_inst, hash_wire, 
   * inst & wire .node fields, instance name hash */
  remove_symbols();
  str_replace(NULL, NULL, NULL, 0);
  escape_chars(NULL, "");
  sanitize(NULL);
  is_generator(NULL);
  extra_rawfile(3, NULL, NULL, -1.0, -1.0);
  /* free_rawfile(&xctx->raw, 0); */
  statusmsg("", 1); /* clear allocated string */
  record_global_node(2, NULL, NULL); /* delete global node array */
  free_xschem_data(); /* delete the xctx struct */
}

int compare_schematics(const char *f)
{
  Int_hashtable table1 = {NULL, 0},  table2 = {NULL, 0};
  Int_hashentry *found;
  char *s = NULL;
  int i;
  size_t l;
  int ret=0; /* ret==0 means no differences found */
  Xschem_ctx *save_xctx;

  int_hash_init(&table1, HASHSIZE);
  int_hash_init(&table2, HASHSIZE);

  dbg(1, "compare_schematics(): xctx->sch_to_compare=%s\n", xctx->sch_to_compare);
  /* set filename of schematic to compare */
  if(f == NULL) {
    tcleval("load_file_dialog {Schematic to compare with} *.\\{sch,sym,tcl\\} INITIALLOADDIR");
    if(tclresult()[0]) my_strncpy(xctx->sch_to_compare, tclresult(), S(xctx->sch_to_compare));
    else my_strncpy(xctx->sch_to_compare, "", S(xctx->sch_to_compare));
  } else if(f[0] != '\0') {
    my_strncpy(xctx->sch_to_compare, f, S(xctx->sch_to_compare));
  }
  if(!xctx->sch_to_compare[0]) {
     my_strncpy(xctx->sch_to_compare, xctx->sch[xctx->currsch], S(xctx->sch_to_compare));
     dbg(0, "Compare current schematic with saved version of itself!\n");
  }

  /* HASH SCHEMATIC 1 */
  for(i = 0; i < xctx->instances; ++i) {
    l =  1024 + strlen(xctx->inst[i].prop_ptr ? xctx->inst[i].prop_ptr : "");
    my_realloc(_ALLOC_ID_, &s, l);
    my_snprintf(s, l, "C %s %g %g %d %d %s",  tcl_hook2(xctx->inst[i].name),
        xctx->inst[i].x0, xctx->inst[i].y0, xctx->inst[i].rot, xctx->inst[i].flip, 
        xctx->inst[i].prop_ptr ?  xctx->inst[i].prop_ptr : "");
    int_hash_lookup(&table1, s, i, XINSERT_NOREPLACE);
  }
  for(i=0;i<xctx->wires; ++i)
  {
    l =1024 + strlen(xctx->wire[i].prop_ptr ? xctx->wire[i].prop_ptr : "");
    my_realloc(_ALLOC_ID_, &s, l);
    my_snprintf(s, l, "N %g %g %g %g", xctx->wire[i].x1,  xctx->wire[i].y1,
        xctx->wire[i].x2, xctx->wire[i].y2);
    int_hash_lookup(&table1, s, i, XINSERT_NOREPLACE);
  }


  /* save old xctx and create new xctx for compare schematic */
  save_xctx = xctx;
  xctx = NULL; 
  alloc_xschem_data("", ".drw");
  xctx->netlist_type = CAD_SPICE_NETLIST;
  tclsetvar("netlist_type","spice"); 
  init_pixdata(); /* populate xctx->fill_type array that is used in create_gc() to set fill styles */
  /* draw in same window */
  xctx->window = save_xctx->window;
  /* copy filename from saved schematic ctx */
  my_strncpy(xctx->sch_to_compare, save_xctx->sch_to_compare, S(xctx->sch_to_compare));
  set_snap(0); /* set default value specified in xschemrc as 'snap' else CADSNAP */
  set_grid(0); /* set default value specified in xschemrc as 'grid' else CADGRID */
  create_gc();
  enable_layers();
  build_colors(0.0, 0.0);

  /* copy graphic context data from schematic 1 */
  xctx->areax1 = save_xctx->areax1;
  xctx->areax2 = save_xctx->areax2;
  xctx->areay1 = save_xctx->areay1;
  xctx->areay2 = save_xctx->areay2;
  xctx->areaw = save_xctx->areaw;
  xctx->areah = save_xctx->areah;
  xctx->xrect[0].x = save_xctx->xrect[0].x;
  xctx->xrect[0].y = save_xctx->xrect[0].y;
  xctx->xrect[0].width = save_xctx->xrect[0].width;
  xctx->xrect[0].height = save_xctx->xrect[0].height;
  xctx->save_pixmap = save_xctx->save_pixmap;
  xctx->gctiled = save_xctx->gctiled;
#if HAS_CAIRO==1
  xctx->cairo_ctx = save_xctx->cairo_ctx;
  xctx->cairo_save_ctx = save_xctx->cairo_save_ctx;
#endif

  /* set identical viewport */
  xctx->zoom = save_xctx->zoom;
  xctx->mooz = save_xctx->mooz;
  xctx->xorigin = save_xctx->xorigin;
  xctx->yorigin = save_xctx->yorigin;
  /* Load schematic 2 for comparing */
  load_schematic(1, xctx->sch_to_compare, 0, 1); /* last param to 0, do not alter window title */

  /* HASH SCHEMATIC 2 , CHECK SCHEMATIC 2 WITH SCHEMATIC 1 */
  for(i = 0; i < xctx->instances; ++i) {
    l =  1024 + strlen(xctx->inst[i].prop_ptr ? xctx->inst[i].prop_ptr : "");
    my_realloc(_ALLOC_ID_, &s, l);
    my_snprintf(s, l, "C %s %g %g %d %d %s",  tcl_hook2(xctx->inst[i].name),
        xctx->inst[i].x0, xctx->inst[i].y0, xctx->inst[i].rot, xctx->inst[i].flip, 
        xctx->inst[i].prop_ptr ?  xctx->inst[i].prop_ptr : "");
    int_hash_lookup(&table2, s, i, XINSERT_NOREPLACE);
    found = int_hash_lookup(&table1, s, i, XLOOKUP);
    if(!found) {
      dbg(1, "schematic 2 instance %d: %s mismatch or not found in schematic 1\n", i,
         xctx->inst[i].instname ? xctx->inst[i].instname : "");
      xctx->inst[i].sel = SELECTED;
      set_first_sel(ELEMENT, i, 0);
      xctx->need_reb_sel_arr=1;
      ret = 1;
    }
  }
  for(i=0;i<xctx->wires; ++i)
  {
    l =1024 + strlen(xctx->wire[i].prop_ptr ? xctx->wire[i].prop_ptr : "");
    my_realloc(_ALLOC_ID_, &s, l);
    my_snprintf(s, l, "N %g %g %g %g", xctx->wire[i].x1,  xctx->wire[i].y1,
        xctx->wire[i].x2, xctx->wire[i].y2);
    int_hash_lookup(&table2, s, i, XINSERT_NOREPLACE);
    found = int_hash_lookup(&table1, s, i, XLOOKUP);
    if(!found) {
      dbg(1, "schematic 2 net %d: %s mismatch or not found in schematic 1\n", i,
         xctx->wire[i].prop_ptr ? xctx->wire[i].prop_ptr : "");
      xctx->wire[i].sel = SELECTED;
      set_first_sel(WIRE, i, 0);
      xctx->need_reb_sel_arr=1;
      ret = 1;
    }
  }
  /* draw mismatches in red */
  if(ret) {
    rebuild_selected_array();
    draw_selection(xctx->gc[PINLAYER], 0);
  }
  unselect_all(0);

  /* delete compare schematic data */
  delete_schematic_data(0); /* do not delete save_pixmap */

  /* restore schematic 1 */
  xctx = save_xctx;

  /* CHECK SCHEMATIC 1 WITH SCHEMATIC 2*/
  for(i = 0; i < xctx->instances; ++i) {
    l = 1024 + strlen(xctx->inst[i].prop_ptr ? xctx->inst[i].prop_ptr : "");
    my_realloc(_ALLOC_ID_,&s, l);
    my_snprintf(s, l, "C %s %g %g %d %d %s",  tcl_hook2(xctx->inst[i].name),
        xctx->inst[i].x0, xctx->inst[i].y0, xctx->inst[i].rot, xctx->inst[i].flip,
        xctx->inst[i].prop_ptr ?  xctx->inst[i].prop_ptr : "");
    found = int_hash_lookup(&table2, s, i, XLOOKUP);
    if(!found) {
      xctx->inst[i].sel = SELECTED;
      set_first_sel(ELEMENT, i, 0);
      xctx->need_reb_sel_arr=1;
      ret = 1;
    }
  }
  for(i=0;i<xctx->wires; ++i)
  {
    l = 1024 + strlen(xctx->wire[i].prop_ptr ? xctx->wire[i].prop_ptr : "");
    my_realloc(_ALLOC_ID_, &s, l);
    my_snprintf(s, l, "N %g %g %g %g", xctx->wire[i].x1,  xctx->wire[i].y1,
        xctx->wire[i].x2, xctx->wire[i].y2);
    found = int_hash_lookup(&table2, s, i, XLOOKUP);
    if(!found) {
      xctx->wire[i].sel = SELECTED;
      xctx->need_reb_sel_arr=1;
      ret = 1;
    }
  }
  int_hash_free(&table1);
  int_hash_free(&table2);
  rebuild_selected_array();
  draw_selection(xctx->gc[SELLAYER], 0);
  my_free(_ALLOC_ID_, &s);
  return ret;
}


static void xwin_exit(void)
{
 int i;

 if(!init_done) {
   dbg(0, "xwin_exit() double call, doing nothing...\n");
   return;
 }
 if(xctx->infowindow_text) my_free(_ALLOC_ID_, &xctx->infowindow_text);
 if(has_x) new_schematic("destroy_all", "1", NULL, 1);
 drawbezier(xctx->window, xctx->gc[0], 0, NULL, NULL, 0, 0);
 delete_schematic_data(1);
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
   for(i = 0; i < cadlayers; ++i) XFreePixmap(display,pixmap[i]);
   #else
   for(i = 0; i < cadlayers; ++i) Tk_FreePixmap(display, pixmap[i]);
   #endif
   my_free(_ALLOC_ID_, &pixmap);
 }
 dbg(1, "xwin_exit(): clearing drawing data structures\n");

 /* global context - graphic preferences/settings */
 for(i=0;i<cadlayers; ++i) {
   my_free(_ALLOC_ID_, &pixdata[i]);
 }
 my_free(_ALLOC_ID_, &pixdata);
 my_free(_ALLOC_ID_, &cli_opt_tcl_command);
 my_free(_ALLOC_ID_, &cli_opt_preinit_command);
 my_free(_ALLOC_ID_, &cli_opt_tcl_post_command);
 clear_expandlabel_data();
 get_sym_template(NULL, NULL); /* clear static data in function */
 list_tokens(NULL, 0); /* clear static data in function */
 translate(0, NULL); /* clear static data in function */
 translate2(NULL, 0, NULL); /* clear static data in function */
 translate3(NULL, 0, NULL, NULL, NULL); /* clear static data in function */
 subst_token(NULL, NULL, NULL); /* clear static data in function */
 find_nth(NULL, "", "", 0, 0); /* clear static data in function */
 trim_chars(NULL, ""); /* clear static data in function */
 tcl_hook2(NULL); /* clear static data in function */
 save_ascii_string(NULL, NULL, 0); /* clear static data in function */
 dbg(1, "xwin_exit(): removing font\n");
 for(i=0;i<127; ++i) my_free(_ALLOC_ID_, &character[i]);
 dbg(1, "xwin_exit(): closed display\n");
 my_strncpy(cli_opt_filename, "", S(cli_opt_filename));
 my_free(_ALLOC_ID_, &xschem_executable);
 dbg(1, "xwin_exit(): deleted undo buffer\n");
 /* delete cmdline stuff */
 for(i = 0 ; i < cli_opt_argc; ++i) {
   my_free(_ALLOC_ID_, &cli_opt_argv[i]);
 }
 my_free(_ALLOC_ID_, &cli_opt_argv);
 if(xschem_web_dirname[0]) tclvareval("file delete -force ", xschem_web_dirname, NULL);
 if(errfp!=stderr) fclose(errfp);
 errfp=stderr;
 dbg(1, "xwin_exit(): closing tcp servers\n");
 tcleval("if {[info exists xschem_server_getdata(server)]} { close $xschem_server_getdata(server) }");
 tcleval("if {[info exists bespice_server_getdata(server)]} { close $bespice_server_getdata(server) }");
 if(!cli_opt_detach) printf("\n");
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
 *        for(i=0;i<cadlayers; ++i) {
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
        tcleval("set tctx::colors $dark_colors");
      }
    } else {
      tcleval("llength $light_colors");
      if(atoi(tclresult()) >=cadlayers){
        tcleval("set tctx::colors $light_colors");
      }
    }
    tcleval("llength $tctx::colors");
    if(atoi(tclresult())<cadlayers){
      fprintf(errfp,"Tcl var tctx::colors not set correctly\n");
      return -1; /* fail */
    } else {
      tcleval("regsub -all {\"} $tctx::colors {} svg_colors");
      tcleval("regsub -all {#} $svg_colors {0x} svg_colors");
    }
    init_color_array(dim, dim_bg);
    if(has_x) for(i=0;i<cadlayers; ++i)
    {
     xctx->color_index[i] = find_best_color(xctx->color_array[i]);
    }
    if(has_x) for(i=0;i<cadlayers; ++i)
    {
      XSetBackground(display, xctx->gc[i], xctx->color_index[0]); /* for dashed lines 'off' color */
      XSetForeground(display, xctx->gc[i], xctx->color_index[i]);
      XSetForeground(display, xctx->gcstipple[i], xctx->color_index[i]);
    }
    if(has_x) for(i=0;i<cadlayers; ++i) {
#ifdef __unix__
      XLookupColor(display, colormap, xctx->color_array[i], &xcolor_exact, &xcolor);
      xctx->xcolor_array[i] = xcolor;
#else
      Tk_Window mainwindow = Tk_MainWindow(interp);
      XColor *xc = Tk_GetColor(interp, mainwindow, xctx->color_array[i]);
      xctx->xcolor_array[i] = *xc;
#endif
    }
    if(has_x) tcleval("reconfigure_layers_menu");
    return 0; /* success */
}

void set_clip_mask(int what)
{
  int i;
  if(what == SET) {
    for(i=0;i<cadlayers; ++i)
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
    for(i=0;i<cadlayers; ++i)
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

/* topwin: .drw (always in tabbed interface) or .x1.drw, .x2.drw ... for multiple windows */
void toggle_fullscreen(const char *topwin)
{
  char fullscr[]="add,fullscreen";
  char normal[]="remove,fullscreen";
  unsigned int topwin_id;
  Window rootwindow, parent_id;
  Window *framewin_child_ptr;
  unsigned int framewindow_nchildren;
  int fs;

  dbg(1, "toggle_fullscreen(): topwin=%s\n", topwin);
  if(!strcmp(topwin, ".drw")) {
    tcleval( "winfo id .");
    sscanf(tclresult(), "0x%x", (unsigned int *) &topwin_id);
  } else {
    /* xctx->top_path is empty string for main window or .x1, .x2, ... for additional windows */
    tclvareval("winfo id ", xctx->top_path, NULL);
    sscanf(tclresult(), "0x%x", (unsigned int *) &topwin_id);
  }
  XQueryTree(display, topwin_id, &rootwindow, &parent_id, &framewin_child_ptr, &framewindow_nchildren);
  fs = tclgetintvar("fullscreen");
  fs = (fs+1)%3;
  if(fs==1) tclsetvar("fullscreen","1");  /* full screen with menubar, toolbar, tabs, statusbar */
  else if(fs==2) tclsetvar("fullscreen","2"); /* full screen with only drawing area */
  else tclsetvar("fullscreen","0"); /* normal view */

  dbg(1, "toggle_fullscreen(): fullscreen=%d\n", fs);
  if(fs==2) {
    if(tclgetboolvar("toolbar_visible")) {
      xctx->menu_removed |= 2; /* menu_removed bit 1 == 1: toolbar was removed */
      tclvareval("toolbar_hide ", xctx->top_path, NULL);
    }
    tclvareval("pack forget ", xctx->top_path, ".tabs", NULL);
    tclvareval("pack forget ", xctx->top_path, ".menubar", NULL);
    tclvareval("pack forget ", xctx->top_path, ".statusbar", NULL);
    xctx->menu_removed |= 1; /* menu_removed bit 0 == 1: other bars were removed */
  } else if(xctx->menu_removed) { /* bars were removed so pack them back */
    tclvareval("pack forget ", xctx->top_path, ".drw", NULL);
    tclvareval("pack ", xctx->top_path, ".menubar -side top -fill x", NULL);
    tclvareval("pack ", xctx->top_path, ".statusbar  -side bottom -fill x", NULL);
    tclvareval("pack ", xctx->top_path, ".drw  -side right -fill both -expand true", NULL);
    if(tclgetboolvar("tabbed_interface")) {
      tclvareval("pack_tabs", NULL);
    }
    if(xctx->menu_removed & 2) tclvareval("toolbar_show ", xctx->top_path, NULL);
    xctx->menu_removed=0;
  }
  if(fs == 1) {
    xctx->pending_fullzoom=1;
    window_state(display , parent_id,fullscr); /* full screen with menus and toolbars */
  } else if(fs == 2) {
    xctx->pending_fullzoom=2;
    window_state(display , parent_id,normal); /* full screen, only drawing area */
    window_state(display , parent_id,fullscr);
  } else {
    xctx->pending_fullzoom=1;
    window_state(display , parent_id,normal); /* normal view */
    /* when switching back from fullscreen multiple ConfigureNotify events are generated. 
     * pending_fullzoom does not work on the last corect ConfigureNotify event,
     * so wee zoom_full() again */
  }
  zoom_full(1, 0, 1 + 2 * tclgetboolvar("zoom_full_center"), 0.97); /* draw */
}


static void tclexit(ClientData s)
{
  dbg(1, "tclexit() INVOKED\n");
  if(init_done) xwin_exit();
}

static int source_tcl_file(char *s)
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

int preview_window(const char *what, const char *win_path, const char *fname)
{
  int result = 0;
  static int last_preview = 0;
  static char *current_file[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  /* save pointer to current schematic context structure */
  Xschem_ctx *save_xctx = NULL;
  /* save pointer to current schematic context structure */
  static Xschem_ctx *preview_xctx[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  static Tk_Window tkpre_window[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  static int semaphore=0;

  /* avoid reentrant calls for example if an alert box is displayed while loading file to preview,
   * and an Expose event calls another preview draw */
  if(semaphore) return 0;
  ++semaphore;
  dbg(1, "preview_window(): what=%s, win_path=%s, fname=%s\n", 
     what, win_path ? win_path : "NULL", fname ? fname : "NULL");
  dbg(1, "------\n");
  tclvareval("save_ctx ", xctx->current_win_path, NULL);
  if(!strcmp(what, "create") && last_preview < 4) {
    int i;
    dbg(1, "preview_window() create, save ctx, win_path=%s\n", win_path);

    for(i = 0; i < 10; i++) {
      if(tkpre_window[i] == NULL) break;
    }
    dbg(1, "preview_window(): create slot %d\n", i);
    if(i < 10) {
      tkpre_window[i] = Tk_NameToWindow(interp, win_path, mainwindow);
      if(tkpre_window[i]) {
        Tk_MakeWindowExist(tkpre_window[i]);
        result = 1;
        last_preview++;
      }
    }
  }
  else if(!strcmp(what, "draw") ) {
    int i;
    for(i = 0; i < 10; i++) {
      if(Tk_NameToWindow(interp, win_path, mainwindow) == tkpre_window[i] && tkpre_window[i]) break;
    }
    dbg(1, "preview_window(): draw slot %d\n", i);
    if(i < 10) {
      save_xctx = xctx; /* save current schematic */
      xctx = preview_xctx[i];
      if(fname && fname[0] && (!current_file[i] || strcmp(fname, current_file[i])) ) { 
        if(current_file[i]) {
          delete_schematic_data(1);
        }
        my_strdup(_ALLOC_ID_, &current_file[i], fname);
        xctx = NULL;      /* reset for preview */
        alloc_xschem_data(save_xctx->top_path, save_xctx->current_win_path); /* alloc data into xctx */
        init_pixdata(); /* populate xctx->fill_type array that is used in create_gc() to set fill styles */
        preview_xctx[i] = xctx;
        preview_xctx[i]->window = Tk_WindowId(tkpre_window[i]);
        create_gc();
        enable_layers();
        build_colors(0.0, 0.0);
        resetwin(1, 0, 1, 0, 0);  /* create preview pixmap.  resetwin(create_pixmap, clear_pixmap, force) */
        dbg(1, "preview_window() draw, load schematic\n");
        load_schematic(1,fname, 0, 1);
      } else {
        resetwin(1, 1, 0, 0, 0);  /* resetwin(create_pixmap, clear_pixmap, force) */
      }
      zoom_full(1, 0, 1 + 2 * tclgetboolvar("zoom_full_center"), 0.97); /* draw */
      xctx = save_xctx;
      result = 1;
    }
  }
  else if(!strcmp(what, "destroy") || !strcmp(what, "close")) {
    int i;
    dbg(1, "preview_window(): %s\n", what);
    for(i = 0; i < 10; i++) {
      if(Tk_NameToWindow(interp, win_path, mainwindow) == tkpre_window[i] && tkpre_window[i]) break;
    }
    dbg(1, "preview_window(): destroy slot %d\n", i);
    if(i < 10) {
      if(preview_xctx[i]) {
        save_xctx = xctx; /* save current schematic */
        xctx = preview_xctx[i];
        if(current_file[i]) {
          delete_schematic_data(1);
          preview_xctx[i] = NULL;
        }
        my_free(_ALLOC_ID_, &current_file[i]);
        xctx = save_xctx; /* restore schematic */
        save_xctx = NULL;
        set_modify(-1);
        result = 1;
        if(!strcmp(what, "destroy")) {
          Tk_DestroyWindow(tkpre_window[i]);
        }
        tkpre_window[i] = NULL;
        last_preview--;
      }
    }
  }
  tclvareval("restore_ctx ", xctx->current_win_path, NULL);
  semaphore--;
  return result;
}


int get_tab_or_window_number(const char *win_path)
{
  int i, n = -1;
  Xschem_ctx *ctx, **save_xctx = get_save_xctx();
  for(i = 0; i < MAX_NEW_WINDOWS; ++i) {
    if(!strcmp(win_path, window_path[i])) {
      n = i;
      break;
    }
  }
  if(n == -1) {
    for(i = 0; i < MAX_NEW_WINDOWS; ++i) {
      /* if only one schematic it is not yet saved in save_xctx */
      if(get_window_count() == 0 && i == 0)  {
        ctx = xctx;
      } else {
        ctx = save_xctx[i];
      }
      if(ctx && !strcmp(win_path, get_cell_w_ext(ctx->current_name, 0))) {
        n = i;
        break;
      }
    }
  }
  return n;
}

/* swap primary view (.drw) with first valid tab (x1.drw, x2.drw, ...)
 * used for window close ('xschem exit' command) */
void swap_tabs(void)
{
  int wc = window_count;
  if(!tclgetboolvar("tabbed_interface")) return;
  if(wc) {
    Xschem_ctx *ctx;
    char *tmp;
    int i = 0;
    int j;
    for(j = 1; j < MAX_NEW_WINDOWS; j++) {
      if(save_xctx[j]) break;
    }
    if(j >= MAX_NEW_WINDOWS) {
      dbg(0, "swap_tabs(): no tab to swap to found\n");
      return;
    }
    if(!save_xctx[i]) {
      dbg(0, "swap_tabs(): no tab to swap from found\n");
      return;
    }

    tmp = save_xctx[i]->top_path;
    save_xctx[i]->top_path = save_xctx[j]->top_path;
    save_xctx[j]->top_path = tmp;

    tmp = save_xctx[i]->current_win_path;
    save_xctx[i]->current_win_path = save_xctx[j]->current_win_path;
    save_xctx[j]->current_win_path = tmp;

    ctx = save_xctx[i];
    save_xctx[i] = save_xctx[j];
    save_xctx[j] = ctx;
   
    /* update filenames on tab buttons */
    ctx = xctx;
    xctx = save_xctx[i];
    set_modify(-1);
    xctx = save_xctx[j];
    set_modify(-1);
    xctx = ctx;
    set_modify(-1);
    /* set context to tab that is about to be deleted */
    new_schematic("switch_tab", save_xctx[j]->current_win_path, NULL, 1);
  }
}

/* swap primary view (.drw) with first valid window (x1.drw, x2.drw, ...)
 * used for window close ('xschem exit' command) 
 * if dr == 1 do draw and don't move following window onto first. This is used if
 * primary windows is about to be deleted */
void swap_windows(int dr)
{
  int wc = window_count;
  if(tclgetboolvar("tabbed_interface")) return;
  if(wc) {
    Xschem_ctx *ctx;
    char *tmp;
    char wp_i[WINDOW_PATH_SIZE], wp_j[WINDOW_PATH_SIZE];
    Window window;
    int i = 0, j;
    Tk_Window tkwin, mainwindow;
    char geometry[80];

    for(j = 1; j < MAX_NEW_WINDOWS; j++) {
      if(save_xctx[j]) break;
    }
    if(j >= MAX_NEW_WINDOWS) {
      dbg(0, "swap_windows(): no tab to swap to found\n");
      return;
    }
    if(!save_xctx[i]) {
      dbg(0, "swap_windows(): no tab to swap from found\n");
      return;
    }
    dbg(1, "swap_windows(): i=%d, j=%d\n", i, j);

    my_snprintf(wp_i, S(wp_i), "%s", save_xctx[i]->current_win_path); /* primary (.drw) window */
    my_snprintf(wp_j, S(wp_j), "%s", save_xctx[j]->current_win_path); /* sub (.x?.drw) window */

    mainwindow = Tk_MainWindow(interp);
    /* get parent of wp_j window, since window manager reparents application windows to a
     * parent window with title bar and borders. This gives accurate geometry data */
    tkwin = Tk_Parent(Tk_NameToWindow(interp, wp_j, mainwindow));

    /* get sub-window geometry and position. primary window wp_i will be sized and moved there. */
    dbg(1, "swap_windows(): %s: %dx%d+%d+%d\n",
            wp_j, Tk_Width(tkwin), Tk_Height(tkwin), Tk_X(tkwin), Tk_Y(tkwin));
    my_snprintf(geometry, S(geometry), "%dx%d+%d+%d",
            Tk_Width(tkwin), Tk_Height(tkwin), Tk_X(tkwin), Tk_Y(tkwin));

    /* swap tcl contexts */
    tclvareval("save_ctx ", xctx->current_win_path, NULL);
    tclvareval("restore_ctx ", wp_i, NULL);
    tclvareval("save_ctx ", "_temp", NULL);
    tclvareval("restore_ctx ", wp_j, NULL);
    tclvareval("save_ctx ", wp_i, NULL);
    tclvareval("restore_ctx ", "_temp", NULL);
    tclvareval("save_ctx ", wp_j, NULL);
    tclvareval("delete_ctx _temp", NULL);

    /* swap xschem xctx structs */
    ctx = save_xctx[i];
    save_xctx[i] = save_xctx[j];
    save_xctx[j] = ctx;

    /* re-swap window paths */
    SWAP(save_xctx[i]->top_path, save_xctx[j]->top_path, tmp);
    SWAP(save_xctx[i]->current_win_path, save_xctx[j]->current_win_path, tmp);
    /* re-swap window IDs */
    SWAP(save_xctx[i]->window, save_xctx[j]->window, window);

    new_schematic("switch", wp_i, "", 0);
    /* move primary window to location of deleted window */
    if(!dr) tclvareval("wm geometry . ", geometry, "; update", NULL);
    resetwin(1, 1, 1, 0, 0);
    if(dr) draw();
    
    new_schematic("switch", wp_j, "", 0);
    resetwin(1, 1, 1, 0, 0);

    my_snprintf(old_winpath, S(old_winpath), "");
    if(dr) draw();
  }
}



/* check if filename is already loaded into a tab or window */
/* caller should supply a win_path string for storing matching window path */
/* window_path[0] == ".drw" */
/* window_path[1] == ".x1.drw" */
/* ....                        */
int check_loaded(const char *f, char *win_path)
{  
  int i;
  Xschem_ctx *ctx;
  int found = 0;
  for(i = 0; i < MAX_NEW_WINDOWS; ++i) {
    ctx = save_xctx[i];
    dbg(1, "window_count=%d i=%d\n", window_count, i);
    /* if only one schematic it is not yet saved in save_xctx */
    if(window_count == 0 && i == 0)  {
      ctx = xctx;
      my_snprintf(window_path[0],  S(window_path[0]), ".drw" );
    }
    if(ctx) {
      dbg(1, "%s <--> %s\n", ctx->sch[ctx->currsch], f);
      if(!strcmp(ctx->sch[ctx->currsch], f)) {
        dbg(1, "check_loaded(): f=%s, sch=%s\n", f, ctx->sch[ctx->currsch]);
        found = 1;
        my_strncpy(win_path, window_path[i], S(window_path[i]));
        break;
      }
    }
  }
  dbg(1, "check_loaded: return %d\n", found);
  return found;
}

/* win_path: .drw for main (first) window, .x1.drw, ... for additional windows */
static int switch_window(int *window_count, const char *win_path, int tcl_ctx)
{
  int n;
  char my_win_path[80];
  Tk_Window tkwin=NULL;
  dbg(1, "switch_window(): win_path=%s\n", win_path);
  if(xctx->semaphore) return 1; /* some editing operation ongoing. do nothing */
  if(!win_path) {
    dbg(0, "switch_window(): no filename or window path given\n");
    return 1;
  }
  if(!strcmp(win_path, xctx->current_win_path)) return 0; /* already there */
  n = get_tab_or_window_number(win_path);
  if(n == 0) my_snprintf(my_win_path, S(my_win_path), ".drw");
  else my_snprintf(my_win_path, S(my_win_path), ".x%d.drw", n);
  if(n == -1) {
    dbg(0, "new_schematic(\"switch\"...): no window to switch to found: %s\n", win_path);
    return 1;
  }
  if(*window_count) {
    /* build my_win_path since win_path can also be a filename */
    dbg(1, "new_schematic(\"switch\"...): %s\n", my_win_path);
    if(has_x) {
      tkwin =  Tk_NameToWindow(interp, my_win_path, mainwindow); /* NULL if win_path not existing */
      if(!tkwin) {
        dbg(0, "new_schematic(\"switch\",...): Warning: %s has been destroyed\n", win_path);
        return 1;
      }
    }
    /* if window was closed then tkwin == 0 --> do nothing */
    if((!has_x || tkwin) && n >= 0 && n < MAX_NEW_WINDOWS) {
      if(tcl_ctx) tclvareval("save_ctx ", xctx->current_win_path, NULL);
      xctx = save_xctx[n];
      if(tcl_ctx) tclvareval("restore_ctx ", win_path, NULL);
      tclvareval("housekeeping_ctx", NULL);
      if(tcl_ctx && has_x) tclvareval("reconfigure_layers_button {}", NULL);
      set_modify(-1); /* sets window title */
      return 0;
    } else return 1;
  }
  dbg(0, "No additional windows are present\n");
  return 0;
}

/* win_path: .drw for main (first) tab, .x1.drw, ... for additional tabs */
static int switch_tab(int *window_count, const char *win_path, int dr)
{        
  int n;

  dbg(1, "switch_tab(): win_path=%s\n", win_path);
  if(xctx->semaphore) return 1; /* some editing operation ongoing. do nothing */
  if(!win_path) {
    dbg(0, "switch_tab(): no filename or window path given\n");
    return 1;
  }
  if(!strcmp(win_path, xctx->current_win_path)) return 0; /* already there */
  n = get_tab_or_window_number(win_path);
  if(n == -1) {
    dbg(0, "new_schematic(\"switch_tab\"...): no tab to switch to found: %s\n", win_path);
    return 1;
  }
  if(*window_count) {
    dbg(1, "new_schematic() switch_tab: %s\n", win_path);
    /* if no matching tab found --> do nothing */
    if(n >= 0 && n < MAX_NEW_WINDOWS) {
      tclvareval("save_ctx ", xctx->current_win_path, NULL);
      xctx = save_xctx[n];
      tclvareval("restore_ctx ", win_path, NULL);
      tclvareval("housekeeping_ctx", NULL);
      if(has_x) tclvareval("reconfigure_layers_button {}", NULL);
      xctx->window = save_xctx[0]->window;
      if(dr) resetwin(1, 1, 1, 0, 0);
      set_modify(-1); /* sets window title */
      if(dr) draw();
      return 0;
    } else return 1;
  }
  dbg(0, "No tabs are present\n");
  return 0;
}

/* non NULL and not empty noconfirm is used to avoid warning for duplicated filenames */
static void create_new_window(int *window_count, const char *noconfirm, const char *fname, int dr)
{
  double save_lw = xctx->lw;
  Window win_id = 0LU;
  char toppath[WINDOW_PATH_SIZE];
  char prev_window[WINDOW_PATH_SIZE];
  int i, n, confirm = 1;

  dbg(1, "new_schematic() create: fname=%s *window_count = %d\n", fname, *window_count);
  
  if(noconfirm && noconfirm[0]) confirm = 0;
  my_strncpy(prev_window,  xctx->current_win_path, S(prev_window));
  if(confirm && fname && fname[0] && check_loaded(fname, toppath)) {
    char msg[PATH_MAX+100];
    my_snprintf(msg, S(msg),
       "tk_messageBox -type okcancel -icon warning -parent [xschem get topwindow] "
       "-message {Warning: %s already open.}", fname);
    if(has_x) {
      tcleval(msg);
      if(strcmp(tclresult(), "ok")) return;
    }
    else {
      dbg(0, "create_new_window: %s already open: %s\n", fname, toppath);
      return;
    }
  }
  if(*window_count == 0) {
    for(i = 0; i < MAX_NEW_WINDOWS; ++i) {
      save_xctx[i] = NULL;
      my_strncpy(window_path[i], "", S(window_path[i]));
    }
    save_xctx[0] = xctx; /* save current schematic */
    /* window_path[0] = Tk_NameToWindow(interp, ".drw", mainwindow); */
    my_strncpy(window_path[0], xctx->current_win_path, S(window_path[0]));
  }
  if(*window_count + 1 >= MAX_NEW_WINDOWS) {
    dbg(0, "new_schematic(\"create\"...): no more free slots\n");
    return; /* no more free slots */
  }
  tclvareval("save_ctx ", xctx->current_win_path, NULL);
  (*window_count)++;
  if(has_x) {
    tclvareval("[xschem get top_path].menubar.simulate configure -bg $simulate_bg", NULL);
    tcleval(".menubar.view.menu entryconfigure {Tabbed interface} -state disabled");
  }
  n = -1;
  for(i = 1; i < MAX_NEW_WINDOWS; ++i) { /* search 1st free slot */
    if(save_xctx[i] == NULL) {
      last_created_window = n = i;
      break;
    }
  }
  if(n == -1) {
    dbg(0, "new_schematic(\"create\"...): no more free slots\n");
    return;
  }
  my_snprintf(window_path[n], S(window_path[n]), ".x%d.drw", n);
  my_snprintf(toppath, S(toppath), ".x%d", n);
  if(has_x) {
    tclvareval("toplevel ", toppath, " -bg {} -width 400 -height 400", NULL);
    tclvareval("build_widgets ", toppath, NULL);
    tclvareval("pack_widgets ", toppath, " ; update", NULL);
    Tk_MakeWindowExist(Tk_NameToWindow(interp, window_path[n], mainwindow));
    win_id = Tk_WindowId(Tk_NameToWindow(interp, window_path[n], mainwindow));
    Tk_ChangeWindowAttributes(Tk_NameToWindow(interp, window_path[n], mainwindow), CWBackingStore, &winattr);
  }
  xctx = NULL;
  alloc_xschem_data(toppath, window_path[n]); /* alloc data into xctx */
  xctx->netlist_type = CAD_SPICE_NETLIST; /* for new windows start with spice netlist mode */
  tclsetvar("netlist_type","spice");
  init_pixdata();/* populate xctx->fill_type array that is used in create_gc() to set fill styles */
  save_xctx[n] = xctx;
  dbg(1, "new_schematic() draw, load schematic\n");
  if(has_x) xctx->window = win_id;
  xctx->lw = save_lw; /* preserve line width on new tabs*/
  set_snap(0); /* set default value specified in xschemrc as 'snap' else CADSNAP */
  set_grid(0); /* set default value specified in xschemrc as 'grid' else CADGRID */
  if(has_x) create_gc();
  enable_layers();
  build_colors(0.0, 0.0);
  resetwin(1, 0, 1, 0, 0);  /* resetwin(create_pixmap, clear_pixmap, force, w, h) */
  xctx->zoom=CADINITIALZOOM;
  xctx->mooz=1/CADINITIALZOOM;
  xctx->xorigin=CADINITIALX;
  xctx->yorigin=CADINITIALY;
  load_schematic(1, fname, 1, confirm);
  if(dr) zoom_full(1, 0, 1 + 2 * tclgetboolvar("zoom_full_center"), 0.97); /* draw */
  tclvareval("set_bindings ", window_path[n], NULL);
  tclvareval("save_ctx ", window_path[n], NULL);
  /* restore previous context,
   * because the Expose event after new window creation does a context switch prev win -> new win 
   * 
   * tclvareval("restore_ctx ", prev_window, NULL);
   * new_schematic("switch", prev_window, "", 1);
   * tclvareval("housekeeping_ctx", NULL);
   */

  if(has_x) windowid(toppath);
}

/* non NULL and not empty noconfirm is used to avoid warning for duplicated filenames */
static void create_new_tab(int *window_count, const char *noconfirm, const char *fname, int dr)
{
  int i, confirm = 1;
  char open_path[WINDOW_PATH_SIZE];
  char nn[WINDOW_PATH_SIZE];
  char win_path[WINDOW_PATH_SIZE];
  double save_lw = xctx->lw;

  dbg(1, "new_schematic() new_tab, creating...\n");
  if(noconfirm && noconfirm[0]) confirm = 0;
  if(confirm && fname && fname[0] && check_loaded(fname, open_path)) {
    char msg[PATH_MAX+100];
    my_snprintf(msg, S(msg),
       "tk_messageBox -type okcancel -icon warning -parent [xschem get topwindow] "
       "-message {Warning: %s already open.}", fname);
    if(has_x) {
      tcleval(msg);
      if(strcmp(tclresult(), "ok")) {
        switch_tab(window_count, open_path, 1);
        return;
      }
    }
    else {
      dbg(0, "create_new_tab: %s already open: %s\n", fname, open_path);
      switch_tab(window_count, open_path, 1);
      return;
    }
  }
  if(*window_count == 0) {
    for(i = 0; i < MAX_NEW_WINDOWS; ++i) {
      save_xctx[i] = NULL;
      my_strncpy(window_path[i], "", S(window_path[i]));
    }
    save_xctx[0] = xctx; /* save current schematic */
    my_strncpy(window_path[0], xctx->current_win_path, S(window_path[0]));
  }
  if(*window_count + 1 >= MAX_NEW_WINDOWS) {
    dbg(0, "new_schematic(\"new_tab\"...): no more free slots\n");
    return; /* no more free slots */
  }
  tclvareval("save_ctx ", xctx->current_win_path, NULL);
  (*window_count)++;
  if(has_x) {
    tclvareval("[xschem get top_path].menubar.simulate configure -bg $simulate_bg", NULL);
    tcleval(".menubar.view.menu entryconfigure {Tabbed interface} -state disabled");
  }
  for(i = 1; i < MAX_NEW_WINDOWS; ++i) { /* search 1st free slot */
    if(save_xctx[i] == NULL) {
      break;
    }
  }
  if(i >= MAX_NEW_WINDOWS) {
    dbg(0, "new_schematic(\"newtab\"...): no more free slots\n");
    return;
  }
  last_created_window = i;
  /* tcl code to create the tab button */
  my_snprintf(nn, S(nn), "%d", i);
  if(has_x) {
    tclvareval(
      "button ", ".tabs.x", nn, " -padx 2 -pady 0 -takefocus 0 -anchor nw -text Tab2 "
      "-command \"xschem new_schematic switch .x", nn, ".drw\"", NULL);
    tclvareval("bind .tabs.x",nn," <ButtonPress-1> {swap_tabs %X %Y press}", NULL);
    tclvareval("bind .tabs.x",nn," <ButtonRelease-1> {swap_tabs %X %Y release}", NULL);
    tclvareval("bind .tabs.x",nn," <ButtonPress-3> {tab_context_menu %W}", NULL);
    tclvareval(
      "if {![info exists tctx::tab_bg] } {set tctx::tab_bg [",
      ".tabs.x", nn, " cget -bg]}", NULL);
    tclvareval("pack ", ".tabs.x", nn, 
      " -before ", ".tabs.add -side left", NULL);
  }
  /*                                   */

  my_snprintf(win_path, S(win_path), ".x%d.drw", i);
  my_strncpy(window_path[i], win_path, S(window_path[i]));
  xctx = NULL;
  alloc_xschem_data("", win_path); /* alloc data into xctx */
  xctx->netlist_type = CAD_SPICE_NETLIST; /* for new windows start with spice netlist mode */
  tclsetvar("netlist_type","spice");
  init_pixdata();/* populate xctx->fill_type array that is used in create_gc() to set fill styles */
  save_xctx[i] = xctx;
  dbg(1, "new_schematic() draw, load schematic\n");
  xctx->window = save_xctx[0]->window;
  xctx->lw = save_lw; /* preserve line width on new tabs*/
  set_snap(0); /* set default value specified in xschemrc as 'snap' else CADSNAP */
  set_grid(0); /* set default value specified in xschemrc as 'grid' else CADGRID */
  if(has_x) create_gc();
  enable_layers();
  build_colors(0.0, 0.0);
  resetwin(1, 0, 1, 0, 0);  /* resetwin(create_pixmap, clear_pixmap, force, w, h) */
  tclvareval("housekeeping_ctx", NULL);
  xctx->zoom=CADINITIALZOOM;
  xctx->mooz=1/CADINITIALZOOM;
  xctx->xorigin=CADINITIALX;
  xctx->yorigin=CADINITIALY;
  load_schematic(1,fname, 1, confirm);
  if(dr) zoom_full(1, 0, 1 + 2 * tclgetboolvar("zoom_full_center"), 0.97); /* draw */
  /* xctx->pending_fullzoom=1; */
}

static void destroy_window(int *window_count, const char *win_path)
{
  int i, n;
  Xschem_ctx *savectx;
  Tk_Window tkwin=NULL;
  savectx = xctx;
  if(*window_count) {
    int close = 0;
    dbg(1, "new_schematic() destroy {%s}\n", win_path);
    if(!win_path) {
      dbg(0, "destroy_window(): no window path given\n");
      return;
    }
    if(xctx->modified && has_x) {
      tcleval("tk_messageBox -type okcancel  -parent [xschem get topwindow] -message \""
              "[get_cell [xschem get schname] 0]"
              ": UNSAVED data: want to exit?\"");
      if(strcmp(tclresult(),"ok")==0) close = 1;
    }
    else close = 1;
    Tcl_ResetResult(interp);
    if(close) {
      if(has_x) {
        tkwin =  Tk_NameToWindow(interp, win_path, mainwindow); /* NULL if win_path not existing */
        if(!tkwin) dbg(0, "new_schematic(\"destroy\", ...): Warning: %s has been destroyed\n", win_path);
      }
      n = -1;
      if(!has_x || tkwin) for(i = 1; i < MAX_NEW_WINDOWS; ++i) {
        if(!strcmp(win_path, window_path[i])) {
          n = i;
          break;
        }
      }
      if(n == -1) {
        dbg(0, "new_schematic(\"destroy\"...): no window to destroy found: %s\n", win_path);
        return;
      }
      if(tkwin && n >= 1 && n < MAX_NEW_WINDOWS) {
        char *toplevel = NULL;
        /* delete Tcl context of deleted schematic window */
        tclvareval("delete_ctx ", win_path, NULL);
        xctx = save_xctx[n];
        /* set saved ctx to main window if current is to be destroyed */
        if(savectx == xctx) savectx = save_xctx[0];
        if(has_x) {
          tclvareval("winfo toplevel ", win_path, NULL);
          my_strdup2(_ALLOC_ID_, &toplevel, tclresult());
        }
        delete_schematic_data(1);
        save_xctx[n] = NULL;
        if(has_x) {
          Tk_DestroyWindow(Tk_NameToWindow(interp, window_path[n], mainwindow));
          tclvareval("destroy ", toplevel, NULL);
          my_free(_ALLOC_ID_, &toplevel);
        }
        my_strncpy(window_path[n], "", S(window_path[n]));
        (*window_count)--;
        if(has_x && *window_count == 0)
          tcleval(".menubar.view.menu entryconfigure {Tabbed interface} -state normal");
      }
    }
    /* following 3 lines must be done also if window not closed */
    xctx = savectx; /* restore previous schematic or main window if previous destroyed */
    tclvareval("restore_ctx ", xctx->current_win_path, " ; housekeeping_ctx", NULL);
    set_modify(-1); /* sets window title */
  } else {
    dbg(0, "new_schematic() destroy_window: there are no additional tabs\n");
  }
}

static void destroy_tab(int *window_count, const char *win_path)
{
  int i, n;
  if(*window_count) {
    int close = 0;
    dbg(1, "new_schematic() destroy_tab\n");

    if(!win_path) {
      dbg(0, "destroy_tab(): no window path given\n");
      return;
    }
    if(strcmp(win_path, xctx->current_win_path)) {
      dbg(0, "new_schematic(\"destroy_tab\", %s): must be in this tab to destroy\n", win_path);
      return;
    }
    if(xctx->modified && has_x) {
      tcleval("tk_messageBox -type okcancel  -parent [xschem get topwindow] -message \""
              "[get_cell [xschem get schname] 0]"
              ": UNSAVED data: want to exit?\"");
      if(strcmp(tclresult(),"ok")==0) close = 1;
    }
    else close = 1;
    Tcl_ResetResult(interp);
    if(close) {
      n = -1;
      for(i = 1; i < MAX_NEW_WINDOWS; ++i) {
        if(!strcmp(win_path, window_path[i])) {
          n = i;
          break;
        }
      }
      if(n == -1) {
        dbg(0, "new_schematic(\"destroy_tab\"...): no tab to destroy found: %s\n", win_path);
        return;
      }
      if(n >= 1 && n < MAX_NEW_WINDOWS) {
        tclvareval("delete_ctx ", win_path, NULL);
        tclvareval("delete_tab ", win_path, NULL);
        xctx = save_xctx[n];
        delete_schematic_data(1);
        save_xctx[n] = NULL;
        my_strncpy(window_path[n], "", S(window_path[n]));
        /* delete Tcl context of deleted schematic window */
        (*window_count)--;
        if(*window_count == 0) tcleval(".menubar.view.menu entryconfigure {Tabbed interface} -state normal");
      }
      xctx = save_xctx[0]; /* restore main (.drw) schematic */

      /* seems unnecessary; previous tab save_pixmap was not deleted */
      /* resetwin(1, 0, 0, 0, 0); */ /* create pixmap.  resetwin(create_pixmap, clear_pixmap, force, w, h) */
 
      tclvareval("restore_ctx ", xctx->current_win_path, " ; housekeeping_ctx", NULL);
      resetwin(1, 1, 1, 0, 0);
      set_modify(-1); /* sets window title */
      draw();
    }
  } else {
    dbg(0, "new_schematic() destroy_tab: there are no additional tabs\n");
  }
}

static void destroy_all_windows(int *window_count, int force)
{
  int i;
  Xschem_ctx *savectx;
  Tk_Window tkwin=NULL;
  savectx = xctx;
  if(*window_count) {
    int close;
    dbg(1, "new_schematic() destroy_all\n");
    for(i = 1; i < MAX_NEW_WINDOWS; ++i) {
      if(window_path[i][0]) {
        if(has_x) {
          tkwin = Tk_NameToWindow(interp, window_path[i], mainwindow); /* NULL if win_path not existing */
        }
        if(has_x && !tkwin) dbg(0, "new_schematic(\"switch\",...): Warning: %s has been destroyed\n", window_path[i]);
        else { 
          xctx = save_xctx[i];
          close = 0;
          /* reset old focused window so callback() will force repaint on expose events */
          if(!force && xctx->modified && has_x) {
            tcleval("tk_messageBox -type okcancel  -parent [xschem get topwindow] -message \""
                    "[get_cell [xschem get schname] 0]"
                    ": UNSAVED data: want to exit?\"");
            if(strcmp(tclresult(),"ok")==0) close = 1;
          }
          else close = 1;
          Tcl_ResetResult(interp);
          if(close) {
            char *toplevel = NULL;
            if(has_x) {
              tclvareval("winfo toplevel ", window_path[i], NULL);
              my_strdup2(_ALLOC_ID_, &toplevel, tclresult());
            }
            delete_schematic_data(1);
            /* set saved ctx to main window if previous is about to be destroyed */
            if(savectx == save_xctx[i]) savectx = save_xctx[0];
            save_xctx[i] = NULL;
            if(has_x) {
              Tk_DestroyWindow(Tk_NameToWindow(interp, window_path[i], mainwindow));
              tclvareval("destroy ", toplevel, NULL);
              my_free(_ALLOC_ID_, &toplevel);
            }
            /* delete Tcl context of deleted schematic window */
            tclvareval("delete_ctx ", window_path[i], NULL);
            my_strncpy(window_path[i], "", S(window_path[i]));
            (*window_count)--;
            if(has_x && *window_count == 0)
               tcleval(".menubar.view.menu entryconfigure {Tabbed interface} -state normal");
          }
        }
      }
    }
    /* following 3 lines must be done also if windows not closed */
    xctx = savectx; /* restore previous schematic or main if old is destroyed */
    tclvareval("restore_ctx ", xctx->current_win_path, " ; housekeeping_ctx", NULL);
    set_modify(-1); /* sets window title */
  }
}

static void destroy_all_tabs(int *window_count, int force)
{
  int i;
  Xschem_ctx *savectx;
  savectx = xctx;
  if(*window_count) {
    int close;
    dbg(1, "new_schematic() destroy_all_tabs\n");
    for(i = 1; i < MAX_NEW_WINDOWS; ++i) {
      if(window_path[i][0]) {
        xctx = save_xctx[i];
        close = 0;
        /* reset old focused window so callback() will force repaint on expose events */
        if(!force && xctx->modified && has_x) {
          tcleval("tk_messageBox -type okcancel  -parent [xschem get topwindow] -message \""
                  "[get_cell [xschem get schname] 0]"
                  ": UNSAVED data: want to exit?\"");
          if(strcmp(tclresult(),"ok")==0) close = 1;
        }
        else close = 1;
        Tcl_ResetResult(interp);
        if(close) {
          /* delete Tcl context of deleted schematic window */
          tclvareval("delete_ctx ", window_path[i], NULL);
          if(has_x) tclvareval("delete_tab ", window_path[i], NULL);
          delete_schematic_data(1);
          /* set saved ctx to main window if previous is about to be destroyed */
          if(savectx == save_xctx[i]) savectx = save_xctx[0];
          save_xctx[i] = NULL;
          my_strncpy(window_path[i], "", S(window_path[i]));
          (*window_count)--;
          if(has_x && *window_count == 0) tcleval(".menubar.view.menu entryconfigure {Tabbed interface} -state normal");
        }
      }
    }
    /* following 3 lines must be done also if windows not closed */
    xctx = savectx; /* restore previous schematic or main if old is destroyed */
    tclvareval("restore_ctx ", xctx->current_win_path, " ; housekeeping_ctx", NULL);
    set_modify(-1); /* sets window title */
  }
}

/* top_path is the path prefix of win_path:
 *
 *  win_path    top_path
 *    ".drw"       ""
 *    ".x1.drw"    ".x1"
 */
int new_schematic(const char *what, const char *win_path, const char *fname, int dr)
{
  int tabbed_interface;
  tabbed_interface = tclgetboolvar("tabbed_interface");
  dbg(1, "new_schematic(): current_win_path=%s, what=%s, win_path=%s\n", xctx->current_win_path, what, win_path);
  if(!strcmp(what, "ntabs")) {
    return window_count;
  } else if(!strcmp(what, "create")) {
    if(!tabbed_interface) {
      create_new_window(&window_count, win_path, fname, dr);
    } else {
      create_new_tab(&window_count, win_path, fname, dr);
    }
  } else if(!strcmp(what, "destroy")) {
    if(!tabbed_interface) {
      destroy_window(&window_count, win_path);
    } else {
      destroy_tab(&window_count, win_path);
    }
  } else if(!strcmp(what, "destroy_all")) {
    if(!tabbed_interface) {
      destroy_all_windows(&window_count, (win_path && win_path[0]) ? 1 : 0);
    } else {
      destroy_all_tabs(&window_count, (win_path && win_path[0]) ? 1 : 0);
    }
  } else if(!strcmp(what, "switch")) {
    if(tabbed_interface) return switch_tab(&window_count, win_path, dr);
    else return switch_window(&window_count, win_path, 1);
  } else if(!strcmp(what, "switch_no_tcl_ctx") && !tabbed_interface) {
    switch_window(&window_count, win_path, 0);
  }
  return window_count;
}

void change_linewidth(double w)
{
  int i, linew;

  /* choose line width automatically based on zoom */
  dbg(1, "change_linewidth(): w = %g, win_path=%s lw=%g\n", w, xctx->current_win_path, xctx->lw);
  if(w<0. || xctx->lw == -1.0) {
    double cs = tclgetdoublevar("cadsnap");
    if(tclgetboolvar("change_lw"))  {
      xctx->lw=xctx->mooz * 0.09 * cs;
      xctx->cadhalfdotsize = CADHALFDOTSIZE * (cs < 20. ? cs : 20.) / 10.;
    }
  /* explicitly set line width */
  } else {
    xctx->lw=w;
  }
  if(has_x) {
    linew = INT_WIDTH(xctx->lw);
    dbg(1, "Line width = %d\n", linew);
    for(i=0;i<cadlayers; ++i) {
        XSetLineAttributes (display, xctx->gc[i], linew, LineSolid, LINECAP , LINEJOIN);
    }
    XSetLineAttributes (display, xctx->gctiled, linew, LineSolid, LINECAP , LINEJOIN);
  }
  if(!xctx->only_probes) {
    xctx->areax1 = -2*INT_WIDTH(xctx->lw);
    xctx->areay1 = -2*INT_WIDTH(xctx->lw);
    xctx->areax2 = xctx->xrect[0].width+2*INT_WIDTH(xctx->lw);
    xctx->areay2 = xctx->xrect[0].height+2*INT_WIDTH(xctx->lw);
    xctx->areaw = xctx->areax2-xctx->areax1;
    xctx->areah = xctx->areay2 - xctx->areay1;
  }
  #if HAS_CAIRO==1
  if(has_x) {
    cairo_set_line_width(xctx->cairo_ctx, INT_WIDTH(xctx->lw));
    cairo_set_line_width(xctx->cairo_save_ctx, INT_WIDTH(xctx->lw));
  }
  #endif
}

/* try to create a cairo context so we get better font metric calculation (text bbox)
 * what = 1: create
 * what = 0 : clear */
void create_memory_cairo_ctx(int what)
{
#if HAS_CAIRO==1
  enum { w = 200, h = 150};

  if(what && !xctx->cairo_ctx) {
    xctx->cairo_sfc = cairo_image_surface_create(CAIRO_FORMAT_RGB24, w, h);
    xctx->cairo_ctx = cairo_create(xctx->cairo_sfc);
  }

  if(!what && xctx->cairo_ctx ) {
    cairo_destroy(xctx->cairo_ctx);
    cairo_surface_destroy(xctx->cairo_sfc);
    xctx->cairo_ctx = NULL;
    xctx->cairo_sfc = NULL;
  }
#endif
}

/* clears and creates cairo_sfc, cairo_ctx, cairo_save_sfc, cairo_save_ctx
 * and sets some graphical attributes */
static void resetcairo(int create, int clear, int force_or_resize)
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
    xctx->cairo_save_ctx = NULL;
    xctx->cairo_ctx = NULL;
    xctx->cairo_save_sfc = NULL;
    xctx->cairo_sfc = NULL;
  }
  if(create && force_or_resize) {
    cairo_font_options_t *options;
    options = cairo_font_options_create();
    cairo_font_options_set_antialias(options, CAIRO_ANTIALIAS_FAST);
    cairo_font_options_set_hint_style(options, CAIRO_HINT_STYLE_SLIGHT);
    /***** Create Cairo save buffer drawing area *****/
    #ifdef __unix__
    xctx->cairo_save_sfc = cairo_xlib_surface_create(display, xctx->save_pixmap,
         visual, xctx->xrect[0].width, xctx->xrect[0].height);
    dbg(1, "resetcairo: create cairo_save_sfc: %d %d\n", xctx->xrect[0].width, xctx->xrect[0].height);
    #else
    xctx->cairo_save_sfc = cairo_win32_surface_create_with_dib(CAIRO_FORMAT_RGB24,
         xctx->xrect[0].width, xctx->xrect[0].height);
    #endif
    if(cairo_surface_status(xctx->cairo_save_sfc)!=CAIRO_STATUS_SUCCESS) {
      fprintf(errfp, "ERROR: invalid cairo xcb surface\n");
    }
    xctx->cairo_font =
       cairo_toy_font_face_create(tclgetvar("cairo_font_name"), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    /* dbg(0, "1 refcount=%d\n", cairo_font_face_get_reference_count(xctx->cairo_font)); */
    xctx->cairo_save_ctx = cairo_create(xctx->cairo_save_sfc);
    cairo_set_antialias (xctx->cairo_save_ctx, CAIRO_ANTIALIAS_NONE);
    cairo_set_font_face(xctx->cairo_save_ctx, xctx->cairo_font);
    cairo_set_font_size(xctx->cairo_save_ctx, 20);

    cairo_set_font_options(xctx->cairo_save_ctx, options);

    cairo_set_line_join(xctx->cairo_save_ctx, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_cap(xctx->cairo_save_ctx, CAIRO_LINE_CAP_ROUND);
    /***** Create Cairo main drawing window structures *****/
    #ifdef __unix__
    xctx->cairo_sfc = cairo_xlib_surface_create(display, xctx->window, visual,
        xctx->xrect[0].width, xctx->xrect[0].height);
    #else
    xctx->cairo_sfc = cairo_win32_surface_create_with_dib(CAIRO_FORMAT_RGB24,
        xctx->xrect[0].width, xctx->xrect[0].height);
    #endif
    if(cairo_surface_status(xctx->cairo_sfc)!=CAIRO_STATUS_SUCCESS) {
      fprintf(errfp, "ERROR: invalid cairo surface\n");
    }
    xctx->cairo_ctx = cairo_create(xctx->cairo_sfc);
    cairo_set_antialias (xctx->cairo_ctx, CAIRO_ANTIALIAS_NONE);
    cairo_set_font_face(xctx->cairo_ctx, xctx->cairo_font);
    cairo_set_font_size(xctx->cairo_ctx, 20);
    cairo_set_font_options(xctx->cairo_ctx, options);
    cairo_set_line_join(xctx->cairo_ctx, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_cap(xctx->cairo_ctx, CAIRO_LINE_CAP_ROUND);
    cairo_font_face_destroy(xctx->cairo_font);
    cairo_font_options_destroy(options);
  }
  #endif /* HAS_CAIRO */
}

/* w and h (if > 0 ) parameters force reset pixmap to w x h, regardless of window size */
void resetwin(int create_pixmap, int clear_pixmap, int force, int w, int h)
{
  unsigned int width = 0, height = 0;
  int status;
  if(has_x) {
    #ifndef __unix__
    HWND hwnd = Tk_GetHWND(xctx->window);
    RECT rct;
    #else
    XWindowAttributes wattr;
    #endif
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
       dbg(1, "resetwin(): win_path=%s\n", xctx->current_win_path);
       dbg(1, "resetwin(): create_pixmap=%d, clear_pixmap=%d, force=%d, width=%d, height=%d, pending_fullzoom=%d\n",
           create_pixmap, clear_pixmap, force, width, height, xctx->pending_fullzoom);
      /* if(wattr.map_state==IsUnmapped) return; */
      xctx->areax2 = width + 2 * INT_WIDTH(xctx->lw);
      xctx->areay2 = height + 2 * INT_WIDTH(xctx->lw);
      xctx->areax1 = -2 * INT_WIDTH(xctx->lw);
      xctx->areay1 = -2 * INT_WIDTH(xctx->lw);
      xctx->areaw = xctx->areax2 - xctx->areax1;
      xctx->areah = xctx->areay2 - xctx->areay1;
      /* if no force avoid unnecessary work if no resize */
      if( force || width != xctx->xrect[0].width || height != xctx->xrect[0].height) {
        dbg(1, "resetwin(): create: %d, clear: %d, force: %d, w=%d h=%d\n",
                create_pixmap, clear_pixmap, force, width, height);
        dbg(1, "resetwin(): changing size\n\n");
        xctx->xrect[0].x = 0;
        xctx->xrect[0].y = 0;
        xctx->xrect[0].width = (unsigned short) width;
        xctx->xrect[0].height = (unsigned short) height;
        if(clear_pixmap) {
          resetcairo(0, 1, 1); /* create, clear, force */
          #ifdef __unix__
          XFreePixmap(display,xctx->save_pixmap);
          #else
          Tk_FreePixmap(display, xctx->save_pixmap);
          #endif
          xctx->save_pixmap = 0;
          XFreeGC(display,xctx->gctiled);
        }
        if(create_pixmap) {
          XGCValues gcv;
          unsigned long gcvm;

          #ifdef __unix__
          xctx->save_pixmap = XCreatePixmap(display, xctx->window,
             xctx->xrect[0].width, xctx->xrect[0].height, screendepth);
          #else
          xctx->save_pixmap = Tk_GetPixmap(display, xctx->window,
             xctx->xrect[0].width, xctx->xrect[0].height, screendepth);
          #endif

          if(fix_broken_tiled_fill || !_unix) {
            gcv.function = GXnoop; /* disable all graphic operations with gctiled */
            gcvm = GCFunction;
            xctx->gctiled = XCreateGC(display,xctx->window, gcvm, &gcv); /* noop for gctiled */
            XSetFillStyle(display,xctx->gctiled,FillSolid);
          } else {
            xctx->gctiled = XCreateGC(display,xctx->window,0L, NULL);
            XSetTile(display,xctx->gctiled, xctx->save_pixmap);
            XSetFillStyle(display,xctx->gctiled,FillTiled);
          }
         
          /* whenever a pixmap is recreated all GC attributes must be reissued */
          resetcairo(1, 0, 1); /* create, clear, force */
          change_linewidth(-1.0);
        }
      }
    }
    if(xctx->pending_fullzoom > 0 && create_pixmap) {
      tcleval("winfo ismapped .");
      /* if window unmapped or size has not been set by wm (width == 1 // height == 1) do not 
       * perform the pending full_zoom */
      if(tclresult()[0] == '1' && (width > 1 || height > 1) ) {
        dbg(1, "resetwin(): window mapped, pending_fulzoom: doing zoom_full()\n");
        zoom_full(1, 0, 1 + 2 * tclgetboolvar("zoom_full_center"), 0.97);
        xctx->pending_fullzoom--;
      }
    }
    dbg(1, "resetwin(): Window reset\n");
  } /* end if(has_x) */
  else {
    /* in memory cairo_ctx if not already created. For text_bbox() when no X */
    /* does something only if HAS_CAIRO is defined */
    if(clear_pixmap) create_memory_cairo_ctx(0);
    if(create_pixmap) create_memory_cairo_ctx(1);
  }
}

void tclmainloop(void)
{
  while(1) Tcl_DoOneEvent(TCL_ALL_EVENTS);
}

int Tcl_AppInit(Tcl_Interp *inter)
{
 const char *tmp_ptr;
 char name[PATH_MAX]; /* overflow safe 20161122 */
 char tmp[2*PATH_MAX+100]; /* 20161122 overflow safe */
#ifndef __unix__
 char install_dir[PATH_MAX] = "";
#endif
 int i;
 double l_width;
 struct stat buf;
 int running_in_src_dir;
 int fs;

#ifdef __unix__
 const char* home_buff;
#endif
 /* get PWD and HOME */
 if(!getcwd(pwd_dir, PATH_MAX)) {
   fprintf(errfp, "Tcl_AppInit(): getcwd() failed\n");
 }
#ifdef __unix__
 if ((home_buff = getenv("HOME")) == NULL) {
   home_buff = getpwuid(getuid())->pw_dir;
 }
 my_strncpy(home_dir, home_buff, S(home_dir));
#else
  change_to_unix_fn(pwd_dir);
  char *home_buff = getenv("USERPROFILE");
  change_to_unix_fn(home_buff);
  my_strncpy(home_dir, home_buff, S(home_dir));
#endif
 /* set error and exit handlers */
 if(!interp) interp=inter;
 Tcl_Init(interp);
 if(has_x) {
   XSetErrorHandler(err);
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
      "append XSCHEM_LIBRARY_PATH : [file dirname \"%s\"]/xschem_library/generators", pwd_dir);
   tcleval(tmp);
   my_snprintf(tmp, S(tmp), 
      "append XSCHEM_LIBRARY_PATH : [file dirname \"%s\"]/xschem_library/inst_sch_select", pwd_dir);
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
 char *up_hier=NULL, *win_xschem_library_path=NULL;
 #define WIN_XSCHEM_LIBRARY_PATH_NUM 11
 const char *WIN_XSCHEM_LIBRARY_PATH[WIN_XSCHEM_LIBRARY_PATH_NUM] = {
   /*1*/  "xschem_library",
   /*2*/  "xschem_library/devices", 
   /*3*/  "xschem_library/examples", 
   /*4*/  "xschem_library/ngspice", 
   /*5*/  "xschem_library/logic", 
   /*6*/  "xschem_library/xschem_simulator",
   /*7*/  "xschem_library/generators", 
   /*8*/  "xschem_library/inst_sch_select", 
   /*9*/  "xschem_library/binto7seg", 
   /*10*/ "xschem_library/pcb", 
   /*11*/ "xschem_library/rom8k" };
 GetModuleFileNameA(NULL, install_dir, MAX_PATH);
 change_to_unix_fn(install_dir);
 size_t dir_len=strlen(install_dir);
 if (dir_len>11)
   install_dir[dir_len-11] = '\0'; /* 11 = remove /xschem.exe */
 /* debugging in Visual Studio will not have bin */
 my_snprintf(tmp, S(tmp), "regexp {bin$} \"%s\"", install_dir);
 tcleval(tmp);
 running_in_src_dir = 0;
 if (atoi(tclresult()) == 0)
 {
   running_in_src_dir = 1; /* no bin, so it's running in Visual studio source directory*/
   my_strdup(_ALLOC_ID_, &up_hier, "../../..");
 }
 else my_strdup(_ALLOC_ID_, &up_hier, "..");
 /* my_strcat(_ALLOC_ID_, &win_xschem_library_path, "."); */
 for (i = 0; i < WIN_XSCHEM_LIBRARY_PATH_NUM; ++i) {
   my_snprintf(tmp, S(tmp),"%s/%s/%s", install_dir, up_hier, WIN_XSCHEM_LIBRARY_PATH[i]);
   if (i > 0) my_strcat(_ALLOC_ID_, &win_xschem_library_path, "\;"); 
   my_strcat(_ALLOC_ID_, &win_xschem_library_path, tmp);
 }
 my_snprintf(tmp, S(tmp), "set tmp2 {%s}; "
   "while {[regsub {([^/]*\\.*[^./]+[^/]*)/\\.\\./?} $tmp2 {} tmp2]} {}; ", win_xschem_library_path); 
 const char *result2 = tcleval(tmp);
 const char *win_xschem_library_path_clean = tclgetvar("tmp2");
 tclsetvar("XSCHEM_LIBRARY_PATH", win_xschem_library_path_clean);
 my_free(_ALLOC_ID_, &win_xschem_library_path);
 my_free(_ALLOC_ID_, &up_hier);
 char *xschem_sharedir=NULL;
 if ((xschem_sharedir=getenv("XSCHEM_SHAREDIR")) != NULL) {
   if (!stat(xschem_sharedir, &buf)) {
     tclsetvar("XSCHEM_SHAREDIR", xschem_sharedir);
   }
 }
 else {
   if (running_in_src_dir==1) {
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

 /* create USER_CONF_DIR if it was not installed, also populate it with template xschemrc */
 if(stat(user_conf_dir, &buf)) {
   if(!mkdir(user_conf_dir, 0700)) {
     char srcfile[PATH_MAX];
     char dstfile[PATH_MAX];
     my_snprintf(dstfile, S(dstfile), "%s/xschemrc", user_conf_dir);
     my_snprintf(srcfile, S(srcfile), "%s/xschemrc", tclgetvar("XSCHEM_SHAREDIR"));
     tclvareval("file copy {", srcfile, "} {", dstfile, "}", NULL);
     fprintf(stderr, "Created %s dir with template xschemrc\n", user_conf_dir);
   } else {
    fprintf(errfp, "Tcl_AppInit(): failure creating %s\n", user_conf_dir);
    Tcl_Exit(EXIT_FAILURE);
   }
 
 }

 /* Execute tcl script given on command line with --preinit, before sourcing xschemrc */
 if(cli_opt_preinit_command) {
   tcleval(cli_opt_preinit_command);
 }
 
 /*                         */
 /*    SOURCE xschemrc file */
 /*                         */
 if(cli_opt_load_initfile) {
   /* get systemwide xschemrc ... */
   if(tclgetvar("XSCHEM_SHAREDIR")) {
     #ifndef __unix__
     if (running_in_src_dir == 1) {
       my_snprintf(name, S(name), "%s/../../XSchemWix/xschemrc", install_dir);
       if (!stat(name, &buf)) {
         fprintf(errfp, "Sourcing %s init file\n", name);
         source_tcl_file(name);
       }
     }
     else {
       my_snprintf(name, S(name), "%s/xschemrc", tclgetvar("XSCHEM_SHAREDIR"));
       if (!stat(name, &buf)) {
         fprintf(errfp, "Tcl_AppInit(): sourcing %s\n", name);
         source_tcl_file(name);
       }
     }
     #else
     my_snprintf(name, S(name), "%s/xschemrc",tclgetvar("XSCHEM_SHAREDIR"));
     if(!stat(name, &buf)) {
       fprintf(errfp, "Sourcing %s init file\n", name);
       source_tcl_file(name);
     }
     #endif
   }
   /* get xschemrc given om cmdline */
   if(cli_opt_rcfile[0]) {
     my_snprintf(name, S(name), cli_opt_rcfile);
     if(stat(name, &buf) ) {
       /* cli_opt_rcfile given on cmdline is not existing */
       fprintf(errfp, "Tcl_AppInit() err 2: cannot find %s\n", name);
       Tcl_ResetResult(interp);
       Tcl_Exit(EXIT_FAILURE);
       return TCL_ERROR;
     }
     else {
       fprintf(errfp, "Sourcing %s init file\n", name);
       source_tcl_file(name);
     }
   }
   else {
     /* source xschemrc in present directory if existing ... */
     if(!running_in_src_dir) {
       my_snprintf(name, S(name), "%s/xschemrc",pwd_dir);
       if(!stat(name, &buf)) {
         fprintf(errfp, "Sourcing %s init file\n", name);
         source_tcl_file(name);
       } else {
         /* ... or look for (user_conf_dir)/xschemrc */
         my_snprintf(name, S(name), "%s/xschemrc", user_conf_dir);
         if(!stat(name, &buf)) {
           fprintf(errfp, "Sourcing %s init file\n", name);
           source_tcl_file(name);
         }
       }
     }
   }
 }
 /* END SOURCING xschemrc */

 if(!sel_file[0]) {
   my_snprintf(sel_file, S(sel_file), "%s/%s", user_conf_dir, ".selection.sch");
 }

 if(!clip_file[0]) {
   my_snprintf(clip_file, S(clip_file), "%s/%s", user_conf_dir, ".clipboard.sch");
 }

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
 if(cli_opt_tcl_command) {
   tcleval(cli_opt_tcl_command);
 }
 /* set tcp port given in cmdline if any */
 if(tcp_port > 0) {
   if(tcp_port < 1024) fprintf(errfp, "please use port numbers >=1024 on command line\n");
   else {
     my_snprintf(name, S(name), "set xschem_listen_port %d", tcp_port);
     tcleval(name);
   }
 }
 if(debug_var==-10) debug_var=0;
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
 my_strdup(_ALLOC_ID_, &xschem_executable, get_file_path(xschem_executable));
 dbg(1, "Tcl_AppInit(): resolved xschem_executable=%s\n", xschem_executable);

 /* get xschem globals from tcl variables set in xschemrc/xschem.tcl */
 cairo_font_line_spacing = tclgetdoublevar("cairo_font_line_spacing");

 if(color_ps==-1)
   color_ps=tclgetboolvar("color_ps");
 else  {
   tclsetboolvar("color_ps", color_ps);
 }
 l_width=tclgetdoublevar("line_width");
 if(tclgetboolvar("change_lw")) l_width = -1.0;
 cadlayers=tclgetintvar("cadlayers");
 fix_broken_tiled_fill = tclgetboolvar("fix_broken_tiled_fill");
 fix_mouse_coord = tclgetboolvar("fix_mouse_coord");
 my_snprintf(tmp, S(tmp), "%.16g",CADGRID);
 tclvareval("set_ne cadgrid ", tmp, NULL);
 my_snprintf(tmp, S(tmp), "%.16g",CADSNAP);
 tclvareval("set_ne cadsnap ", tmp, NULL);
 cairo_vert_correct = tclgetdoublevar("cairo_vert_correct");
 nocairo_vert_correct = tclgetdoublevar("nocairo_vert_correct");
 cairo_font_scale = tclgetdoublevar("cairo_font_scale");

 /*                             */
 /*  [m]allocate dynamic memory */
 /*                             */
 alloc_xschem_data("", ".drw");

 /* create /tmp/xschem_web_xxx directory for remote objects */
 tmp_ptr = create_tmpdir("xschem_web_");
 if(!tmp_ptr) {
   dbg(0, "xinit(): problems creating /tmp/xschem_web_xxxx dir\n");
 } else {
   my_strncpy(xschem_web_dirname, tmp_ptr, S(xschem_web_dirname));
 }

 /* initialize current schematic name to empty string to avoid gazillion checks in the code for NULL */
 my_strdup2(_ALLOC_ID_, &xctx->sch[xctx->currsch], "");

 xctx->crosshair_layer = tclgetintvar("crosshair_layer");
 if(xctx->crosshair_layer < 0 ) xctx->crosshair_layer = 2;
 if(xctx->crosshair_layer >= cadlayers ) xctx->crosshair_layer = 2;

 /* global context / graphic preferences/settings */
 pixdata=my_calloc(_ALLOC_ID_, cadlayers, sizeof(char*));
 for(i=0;i<cadlayers; ++i)
 {
   pixdata[i]=my_calloc(_ALLOC_ID_, 32, sizeof(char));
 }
 if(has_x) pixmap=my_calloc(_ALLOC_ID_, cadlayers, sizeof(Pixmap));

 my_strncpy(xctx->plotfile, cli_opt_plotfile, S(xctx->plotfile));
 xctx->draw_window = tclgetintvar("draw_window");
 xctx->only_probes = tclgetintvar("only_probes");
 xctx->intuitive_interface = tclgetboolvar("intuitive_interface");

 /* set global variables fetching data from tcl code */

 /* if lvs_netlist is set also use lvs_format for devices netlisting rule if existing */
 if(tclgetboolvar("lvs_netlist")) tcleval("xschem set format lvs_format");
 else tcleval("xschem set format {}");

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
 }
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
 for(i = 0; i < cadlayers; ++i) xctx->enable_layer[i] = 1;
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

    #if 0
    #ifdef HAS_XCB
    xcb_conn = XGetXCBConnection(display);
    #endif
    #endif

    tkwindow = Tk_NameToWindow(interp, ".drw", mainwindow);
    Tk_MakeWindowExist(tkwindow);
    xctx->window = Tk_WindowId(tkwindow);
    topwindow = Tk_WindowId(mainwindow);

    dbg(1, "Tcl_AppInit(): drawing window ID=0x%lx\n",xctx->window);
    dbg(1, "Tcl_AppInit(): top window ID=0x%lx\n",topwindow);
    dbg(1, "Tcl_AppInit(): done tkinit()\n");

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
    if(build_colors(0.0, 0.0)) exit(1);
    dbg(1, "Tcl_AppInit(): done step e of xinit()\n");
    /* xctx->save_pixmap must be created as resetwin() frees it before recreating with new size. */
  }
  resetwin(1, 0, 1, 0, 0);
  if(has_x) {
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
 my_snprintf(tmp, S(tmp), "%d", Mod4Mask);
 tclsetvar("Mod4Mask", tmp);
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
   tclsetintvar("tctx::max_new_windows", MAX_NEW_WINDOWS);
   tcleval("pack_widgets; set_bindings .drw");
 }

 fs=tclgetintvar("fullscreen");
 if(fs) {
   tclsetvar("fullscreen", "0");
   tcleval("update");
   toggle_fullscreen(".drw");
 }

 if(tclgetboolvar("case_insensitive")) {
   xctx->case_insensitive = 1;
   xctx->x_strcmp = my_strcasecmp;
 }

 if(tclgetboolvar("show_hidden_texts")) {
   xctx->show_hidden_texts = 1;
 }

 /*                                */
 /*  START PROCESSING USER OPTIONS */
 /*                                */

 /* set netlist filename. Set also netlist_dir if a path is given */
 if(cli_opt_initial_netlist_name[0]) {
   my_strncpy(xctx->netlist_name, cli_opt_initial_netlist_name, S(cli_opt_initial_netlist_name));
   #ifdef __unix__
   if(strchr(xctx->netlist_name, '/')) {
     tclvareval("file dirname ", xctx->netlist_name, NULL);
     tclsetvar("netlist_dir", tclresult());
   }
   #endif
 }
 /* set tcl netlist_dir if netlist_dir given on cmdline */
 if(cli_opt_netlist_dir[0]) tclsetvar("netlist_dir", cli_opt_netlist_dir);
 enable_layers();
 /* prefer using env(PWD) if existing since it does not dereference symlinks */
 if(tcleval("info exists env(PWD)")[0] == '1') {
   my_snprintf(pwd_dir, S(pwd_dir), "%s", tclgetvar("env(PWD)"));
 }

 if(cli_opt_diff[0]) {
   my_strncpy(xctx->sch_to_compare, abs_sym_path(cli_opt_diff, ""), S(xctx->sch_to_compare));
   tclsetvar("compare_sch", "1");
 } 
 if(cli_opt_filename[0]) {
    char f[PATH_MAX];
    int file_loaded = 1;
   /* check if local_netlist_dir is set and set netlist_dir accordingly
    * following call is needed since load_schematic() may be called with 
    * reset_undo=0 and will not call set_netlist_dir */
   set_netlist_dir(2, NULL);
   #ifdef __unix__
   if(is_from_web(cli_opt_filename)) {
     my_snprintf(f, S(f), "%s", cli_opt_filename);
   } else if(cli_opt_filename[0] == '~' && cli_opt_filename[1] == '/') {
     my_snprintf(f, S(f), "%s%s", home_dir, cli_opt_filename + 1);
   } else if(cli_opt_filename[0] == '.' && cli_opt_filename[1] == '/') {
     my_snprintf(f, S(f), "%s%s", pwd_dir, cli_opt_filename + 1);
   } else {
     my_snprintf(f, S(f), "%s", abs_sym_path(cli_opt_filename, ""));
   }
   #else
   my_strncpy(f, abs_sym_path(cli_opt_filename, ""), S(f));
   #endif
   dbg(1, "Tcl_AppInit(): cli_opt_filename %s given, removing symbols\n", cli_opt_filename);
   remove_symbols();
   /* if cli_opt_do_netlist=1 call load_schematic with 'reset_undo=0' avoiding call 
      to tcl is_xschem_file that could change xctx->netlist_type to symbol */
   file_loaded = load_schematic(1, f, !cli_opt_do_netlist, 1);
   if(cli_opt_do_netlist) if(!file_loaded) tcleval("exit 1");
   if(cli_opt_do_netlist) set_modify(-1); /* set tab/window title */
   tclvareval("update_recent_file {", f, "}", NULL);
 } else /* if(!cli_opt_filename[0]) */
 {
   char *tmp;
   char fname[PATH_MAX];
   int file_loaded = 1;
   tmp = (char *) tclgetvar("XSCHEM_START_WINDOW");
   #ifndef __unix__
   change_to_unix_fn(tmp);
   #endif
   dbg(1, "Tcl_AppInit(): tmp=%s\n", tmp? tmp: "NULL");
   if(0 && cli_argc > 1)  /* disabled */
     my_strncpy(fname, "", S(fname)); /* no load XSCHEM_START_WINDOW if cli args given */
   else
     my_strncpy(fname, abs_sym_path(tmp, ""), S(fname));
    /* if cli_opt_do_netlist=1 call load_schematic with 'reset_undo=0' avoiding call 
       to tcl is_xschem_file that could change xctx->netlist_type to symbol */
   file_loaded = load_schematic(1, fname, !cli_opt_do_netlist, 1);
   if(!file_loaded) tcleval("exit 1");
   if(cli_opt_do_netlist) set_modify(-1); /* set tab/window title */
 }
 /* Necessary to tell xschem the initial area to display */
 zoom_full(0, 0, 1 + 2 * tclgetboolvar("zoom_full_center"), 0.97);
 xctx->pending_fullzoom=1;
 if(cli_opt_do_netlist) {
   if(!cli_opt_filename[0]) {
     fprintf(errfp, "xschem: cant do a netlist without a filename\n");
     tcleval("exit 1");
   }
   if(set_netlist_dir(0, NULL)) { /* necessary to create netlist dir if not existing */
     if(debug_var>=1) {
       if(tclgetboolvar("flat_netlist"))
         fprintf(errfp, "xschem: flat netlist requested\n");
     }
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
 if(cli_opt_do_print) {
   if(!cli_opt_filename[0]) {
     dbg(0, "xschem: can't do a print without a filename\n");
     tcleval("exit 1");
   }
   if(cli_opt_do_print==1) {

     xctx->xrect[0].x = 0;
     xctx->xrect[0].y = 0;
     xctx->xrect[0].width = 842;
     xctx->xrect[0].height = 595;
     xctx->areax2 = 842+2;
     xctx->areay2 = 595+2;
     xctx->areax1 = -2;
     xctx->areay1 = -2;
     xctx->areaw = xctx->areax2-xctx->areax1;
     xctx->areah = xctx->areay2-xctx->areay1;
     zoom_full(0, 0, 2 * tclgetboolvar("zoom_full_center"), 0.97);
     ps_draw(7, 0, 0);
   } else if(cli_opt_do_print == 2) {
     if(!has_x) {
       dbg(0, "xschem: can not do a png export if no X11 present / Xserver running (check if DISPLAY set).\n");
     } else {
       tcleval("tkwait visibility .drw");
       print_image();
     }
   }
   else {
     svg_draw();
   }
 }

 if(cli_opt_do_simulation) {
   if(!cli_opt_filename[0]) {
     fprintf(errfp, "xschem: can't do a simulation without a filename\n");
     tcleval("exit 1");
   }
   tcleval( "simulate");
 }

 if(cli_opt_do_waves) {
   if(!cli_opt_filename[0]) {
     fprintf(errfp, "xschem: can't show simulation waves without a filename\n");
     tcleval("exit 1");
   }
   tcleval( "waves [file tail \"[xschem get schname]\"]");
 }

 /*                                    */
 /* SOURCE XSCHEMRC SUPPLIED TCL FILES */
 /*                                    */
 tcleval("update; source_user_tcl_files");

 /* source tcl file given on command line with --script */
 if(cli_opt_tcl_script[0]) {
   /* can not use tclvareval() here because if script contains 'exit'
    * program terminates before tclvareval() has a chance to cleanup
    * its dynamically allocated string
    */
   dbg(1, "executing --script file : %s\n",  cli_opt_tcl_script);
   tcleval("update");
   source_tcl_file(cli_opt_tcl_script);
 }

 /* load additional files */
 for(i = 2; i < cli_opt_argc; ++i) {
   tclvareval("xschem load_new_window ",  cli_opt_argv[i], NULL);
 }

 /* Execute tcl script given on command line with --command */
 if(cli_opt_tcl_post_command) {
   tcleval(cli_opt_tcl_post_command);
 }

 /* Execute tcl commands given in tcl variable postinit_commands if existing */
 tcleval("eval_postinit_commands");

 if(cli_opt_quit) {
   char s[40];
   my_snprintf(s, S(s), "exit %d", exit_code);
   tcleval(s);
 }


 /* */
 /*  END PROCESSING USER OPTIONS */
 /* */


 if(tclgetboolvar("use_tclreadline") && !cli_opt_detach && !cli_opt_no_readline) {
   tcleval( "if {![catch {package require tclreadline}] && [info exists tcl_interactive] && $tcl_interactive} "
    "{"
      "::tclreadline::readline builtincompleter 0; "
      "::tclreadline::readline customcompleter completer; "
      "::tclreadline::Loop"
    "}" 
   );
 }
 /* set up a tcl event handler to serve events (tcp connections) if no other
  * event queue is running (no tk event queue, no tclreadline event loop)
  * otherwise you have to manually call a vwait or update command to let
  * tcp callbacks respond */
 if(!has_x) Tcl_SetMainLoop(tclmainloop);
 dbg(1, "Tcl_AppInit(): returning TCL_OK\n");
 return TCL_OK;
}



