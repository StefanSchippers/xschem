/* File: xinit.c
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
#include <pwd.h> /* getpwuid */
#endif

static int init_done=0; /* 20150409 to avoid double call by Xwindows close and TclExitHandler */
static XSetWindowAttributes winattr;
static int screen_number;
static Tk_Window  tkwindow, mainwindow, tkpre_window;
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

void windowid()
{
  int i;
  Display *display;
  Tk_Window mainwindow;

  unsigned int ww;
  Window framewin, rootwindow;
  Window *framewin_child_ptr;
  unsigned int framewindow_nchildren;

  framewindow_nchildren =0;
  mainwindow=Tk_MainWindow(interp);
  display = Tk_Display(mainwindow);
  tcleval( "winfo id .");
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
    dbg(1, "Tcl_AppInit(): creating icon pixmap returned: %d\n",i);
    hints_ptr = XAllocWMHints();
    hints_ptr->icon_pixmap = cad_icon_pixmap ;
    hints_ptr->icon_mask = cad_icon_mask ;
    hints_ptr->flags = IconPixmapHint | IconMaskHint;
    XSetWMHints(display, parent_of_topwindow, hints_ptr);
    XFree(hints_ptr);
  }
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
 unsigned int index;
#ifdef __unix__
 if( XAllocNamedColor(display, colormap, colorname, &xcolor_exact, &xcolor) ==0 )
#else
 Tk_Window mainwindow = Tk_MainWindow(interp);
 XColor* xc = Tk_GetColor(interp, mainwindow, colorname);
 if (XAllocColor(display, colormap, xc) == 0)
#endif
 {
  for(i=0;i<=255;i++) {
    xcolor_array[i].pixel=i;
#ifdef __unix__
    XQueryColor(display, colormap, xcolor_array+i);
#else
    xcolor = *xc;
    XQueryColors(display, colormap, xc, i);
#endif
  }
  /* debug ... */
  dbg(2,
        "find_best_color(): Server failed to allocate requested color, finding substitute\n");
  XLookupColor(display, colormap, colorname, &xcolor_exact, &xcolor);
  red = xcolor.red; green = xcolor.green; blue = xcolor.blue;
  index=0;
  for(i = 0;i<=255; i++)
  {
   r = xcolor_array[i].red ; g = xcolor_array[i].green ; b = xcolor_array[i].blue;
   deltar = (r - red);deltag = (g - green);deltab = (b - blue);
   dist = deltar*deltar + deltag*deltag + deltab*deltab;
   if( dist <= distance )
   {
    index = i;
    distance = dist;
   }
  }
 }
 else
 {
  /*XLookupColor(display, colormap, colorname, &xcolor_exact, &xcolor); */
#ifdef __unix__
  index = xcolor.pixel;
#else
  index = xc->pixel;
#endif
 }

 return index;
}


void init_color_array(double dim)
{
 char s[256]; /* overflow safe 20161122 */
 int i;
 int dim_bg;
 unsigned int r, g, b;
 double rr, gg, bb;
 static int done=0;

 dim_bg = tclgetvar("dim_background")[0] == '1' ? 1: 0;
 for(i=0;i<cadlayers;i++) {
   my_snprintf(s, S(s), "lindex $colors %d",i);
   tcleval(s);
   dbg(2, "init_color_array(): color:%s\n",tclresult());

   sscanf(tclresult(), "#%02x%02x%02x", &r, &g, &b);
   rr=r; gg=g; bb=b;

   
   if(dim_bg || i!=BACKLAYER ) {
     if(dim>=0.) {
       rr +=(51.-rr/5.)*dim;
       gg +=(51.-gg/5.)*dim;
       bb +=(51.-bb/5.)*dim;
     } else {
       rr +=(rr/5.)*dim;
       gg +=(gg/5.)*dim;
       bb +=(bb/5.)*dim;
     }
     /* fprintf(errfp, "init_color_array: colors: %.16g %.16g %.16g dim=%.16g c=%d\n", rr, gg, bb, dim, i); */
     r=rr;g=gg;b=bb;
     if(r>0xff) r=0xff;
     if(g>0xff) g=0xff;
     if(b>0xff) b=0xff;
   }
   my_snprintf(s, S(s), "#%02x%02x%02x", r, g, b);
   if(!done) {
     my_strdup(643, &color_array[i], s);
     done = 1;
   } else if(dim_bg || i!=BACKLAYER ) {
     my_strdup(605, &color_array[i], s);
   }
 }

}

void init_pixdata()
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
   if(full) fill_type[i] = 1;
   else if(empty) fill_type[i] = 0;
   else fill_type[i]=2;
   if(rainbow_colors && i>5) fill_type[i]=1; /* 20171212 solid fill style */
   /*fprintf(errfp, "fill_type[%d]= %d\n", i, fill_type[i]); */
 }
}

void free_xschem_data()
{
  int i;
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
  for(i=0;i<CADMAXHIER;i++) my_free(1139, &xctx->sch_path[i]);
  
  my_free(269, &xctx);

}

void alloc_xschem_data()
{
  int i;
  xctx = my_calloc(153, 1, sizeof(Xschem_ctx));
  xctx->zoom=CADINITIALZOOM;
  xctx->mooz=1/CADINITIALZOOM;
  xctx->xorigin=CADINITIALX;
  xctx->yorigin=CADINITIALY;
  xctx->wires = 0;
  xctx->instances = 0;
  xctx->symbols = 0;
  xctx->texts = 0;
  xctx->schprop = NULL;     /* SPICE */
  xctx->schtedaxprop=NULL;  /* tEDAx */
  xctx->schvhdlprop=NULL;   /* vhdl property string */
  xctx->schsymbolprop=NULL; /* symbol property string */
  xctx->schverilogprop=NULL;/* verilog */
  xctx->version_string = NULL;
  xctx->currsch = 0;
  xctx->ui_state = 0;
  xctx->need_reb_sel_arr = 1;
  xctx->lastsel = 0;
  xctx->maxsel = 0;
  xctx->prep_net_structs = 0;
  xctx->prep_hi_structs = 0;
  xctx->prep_hash_inst = 0;
  xctx->prep_hash_wires = 0;
  xctx->modified = 0;
  xctx->netlist_name[0] = '\0';

  for(i=0;i<CADMAXHIER;i++) xctx->sch_path[i]=NULL;
  my_strdup(1187, &xctx->sch_path[0],".");
  xctx->sch_inst_number[0] = 1;

  xctx->maxt=CADMAXTEXT;
  xctx->maxw=CADMAXWIRES;
  xctx->maxi=ELEMINST;
  xctx->maxs=ELEMDEF;
  xctx->text=my_calloc(606, xctx->maxt,sizeof(xText));
  if(xctx->text==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }

  xctx->wire=my_calloc(607, xctx->maxw,sizeof(xWire));
  if(xctx->wire==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }

  xctx->inst=my_calloc(609, xctx->maxi , sizeof(xInstance) );
  if(xctx->inst==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }

  xctx->sym=my_calloc(610, xctx->maxs , sizeof(xSymbol) );
  if(xctx->sym==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }
  for(i=0;i<xctx->maxs;i++) {
    xctx->sym[i].line=my_calloc(611, cadlayers, sizeof(xLine *));
    if(xctx->sym[i].line==NULL){
      fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
    }

    xctx->sym[i].poly=my_calloc(612, cadlayers, sizeof(xPoly *));
    if(xctx->sym[i].poly==NULL){
      fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
    }

    xctx->sym[i].arc=my_calloc(613, cadlayers, sizeof(xArc *));
    if(xctx->sym[i].arc==NULL){
      fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
    }

    xctx->sym[i].rect=my_calloc(614, cadlayers, sizeof(xRect *));
    if(xctx->sym[i].rect==NULL){
      fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
    }

    xctx->sym[i].lines=my_calloc(615, cadlayers, sizeof(int));
    if(xctx->sym[i].lines==NULL){
      fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
    }

    xctx->sym[i].rects=my_calloc(616, cadlayers, sizeof(int));
    if(xctx->sym[i].rects==NULL){
      fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
    }
    xctx->sym[i].arcs=my_calloc(617, cadlayers, sizeof(int));
    if(xctx->sym[i].arcs==NULL){
      fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
    }
    xctx->sym[i].polygons=my_calloc(618, cadlayers, sizeof(int));
    if(xctx->sym[i].polygons==NULL){
      fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
    }
  }

  xctx->maxr=my_calloc(620, cadlayers, sizeof(int));
  if(xctx->maxr==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }

  xctx->maxa=my_calloc(621, cadlayers, sizeof(int));
  if(xctx->maxa==NULL){
    fprintf(errfp, "Tcl_AppInit(): max_arcscalloc error\n");tcleval( "exit");
  }

  xctx->maxp=my_calloc(622, cadlayers, sizeof(int));
  if(xctx->maxp==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }

  xctx->maxl=my_calloc(623, cadlayers, sizeof(int));
  if(xctx->maxl==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }

  for(i=0;i<cadlayers;i++)
  {
   xctx->maxr[i]=CADMAXOBJECTS;
   xctx->maxp[i]=CADMAXOBJECTS;
   xctx->maxl[i]=CADMAXOBJECTS;
   xctx->maxa[i]=CADMAXOBJECTS;
  }

  xctx->rect=my_calloc(624, cadlayers, sizeof(xRect *));
  if(xctx->rect==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }

  xctx->line=my_calloc(625, cadlayers, sizeof(xLine *));
  if(xctx->line==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }

  xctx->poly=my_calloc(626, cadlayers, sizeof(xPoly *));
  if(xctx->poly==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }

  xctx->arc=my_calloc(627, cadlayers, sizeof(xArc *));
  if(xctx->arc==NULL){
    fprintf(errfp, "Tcl_AppInit(): arc calloc error\n");tcleval( "exit");
  }

  for(i=0;i<cadlayers;i++)
  {
   xctx->rect[i]=my_calloc(628, xctx->maxr[i],sizeof(xRect));
   if(xctx->rect[i]==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
   }

   xctx->arc[i]=my_calloc(629, xctx->maxa[i],sizeof(xArc));
   if(xctx->arc[i]==NULL){
     fprintf(errfp, "Tcl_AppInit(): arc[] calloc error\n");tcleval( "exit");
   }

   xctx->poly[i]=my_calloc(630, xctx->maxp[i],sizeof(xPoly));
   if(xctx->poly[i]==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
   }

   xctx->line[i]=my_calloc(631, xctx->maxl[i],sizeof(xLine));
   if(xctx->line[i]==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
   }
  }

  xctx->rects=my_calloc(632, cadlayers, sizeof(int));
  if(xctx->rects==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }

  xctx->polygons=my_calloc(633, cadlayers, sizeof(int));
  if(xctx->polygons==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }

  xctx->arcs=my_calloc(634, cadlayers, sizeof(int));
  if(xctx->arcs==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }

  xctx->lines=my_calloc(635, cadlayers, sizeof(int));
  if(xctx->lines==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }
}

void alloc_data()
{
 int i;

 alloc_xschem_data();
 /* global context / graphic preferences/settings */

 xctx->maxsel=MAXGROUP;
 xctx->sel_array=my_calloc(619, xctx->maxsel, sizeof(Selected));
 if(xctx->sel_array==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 gridpoint=(XPoint*)my_calloc(608, CADMAXGRIDPOINTS,sizeof(XPoint));
 if(gridpoint==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 pixmap=my_calloc(636, cadlayers, sizeof(Pixmap));
 if(pixmap==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 color_array=my_calloc(637, cadlayers, sizeof(char*));
 if(color_array==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 gc=my_calloc(638, cadlayers, sizeof(GC));
 if(gc==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 gcstipple=my_calloc(639, cadlayers, sizeof(GC));
 if(gcstipple==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 fill_type=my_calloc(640, cadlayers, sizeof(int));
 if(fill_type==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 active_layer=my_calloc(563, cadlayers, sizeof(int));
 if(active_layer==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 pixdata=my_calloc(641, cadlayers, sizeof(char*));
 if(pixdata==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 for(i=0;i<cadlayers;i++)
 {
   pixdata[i]=my_calloc(642, 32, sizeof(char));
   if(pixdata[i]==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
   }
 }
 enable_layer=my_calloc(87, cadlayers, sizeof(int));
}

void xwin_exit(void)
{
 int i;

 if(!init_done) {
   dbg(1, "xwin_exit() double call, doing nothing...\n");
   return;
 }
 delete_netlist_structs();
 delete_hilight_net();
 get_unnamed_node(0, 0, 0);

 if(has_x) {
    #ifdef HAS_CAIRO
    cairo_destroy(cairo_ctx);
    cairo_destroy(cairo_save_ctx);
    cairo_surface_destroy(sfc);
    cairo_surface_destroy(save_sfc);
    #endif
#ifdef __unix__
    XFreePixmap(display,save_pixmap);
    for(i=0;i<cadlayers;i++)XFreePixmap(display,pixmap[i]);
#else
    Tk_FreePixmap(display, save_pixmap);
    for (i = 0; i < cadlayers; i++)Tk_FreePixmap(display, pixmap[i]);
#endif
    dbg(1, "xwin_exit(): Releasing pixmaps\n");
    for(i=0;i<cadlayers;i++)
    {
     XFreeGC(display,gc[i]);
     XFreeGC(display,gcstipple[i]);
    }
    XFreeGC(display,gctiled);
    dbg(1, "xwin_exit(): destroying tk windows and releasing X11 stuff\n");
    Tk_DestroyWindow(mainwindow);
#ifdef __unix__
    if(cad_icon_pixmap) {
      XFreePixmap(display, cad_icon_pixmap);
      XFreePixmap(display, cad_icon_mask);
    }
#else
    if (cad_icon_pixmap) Tk_FreePixmap(display, cad_icon_pixmap);
#endif
 }
 dbg(1, "xwin_exit(): clearing drawing data structures\n");
 clear_drawing();
 remove_symbols();

 free_xschem_data();

 /* global context - graphic preferences/settings */
 for(i=0;i<cadlayers;i++) {
   my_free(1101, &color_array[i]);
   my_free(1102, &pixdata[i]);
 }
 my_free(1108, &xctx->sel_array);
 my_free(1120, &fill_type);
 my_free(1121, &active_layer);
 my_free(1122, &pixdata);
 my_free(1123, &enable_layer);
 my_free(1099, &gridpoint);
 my_free(1134, &pixmap);
 my_free(1135, &gc);
 my_free(1136, &gcstipple);
 my_free(1137, &color_array);
 my_free(1138, &tcl_command);
 clear_expandlabel_data();
 get_sym_template(NULL, NULL); /* clear static data in function */
 get_tok_value(NULL, NULL, 0); /* clear static data in function */
 list_tokens(NULL, 0); /* clear static data in function */
 translate(0, NULL); /* clear static data in function */
 translate2(NULL, 0, NULL); /* clear static data in function */
 subst_token(NULL, NULL, NULL); /* clear static data in function */
 find_nth(NULL, '\0', 0); /* clear static data in function */

 dbg(1, "xwin_exit(): removing font\n");
 for(i=0;i<127;i++) my_free(1140, &character[i]);

 dbg(1, "xwin_exit(): closed display\n");
 my_free(1141, &filename);

 delete_undo();
 my_free(1142, &netlist_dir);
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
 *  - color_array[]: 
 *      array of color character  names ("#xxxxxx" hex) indexed by xschem layer number.
 *      these are set from tcl 'color' list variable in init_color_array()
 *  - color_index[]: 
 *      array of integers, pixel values, color lookup table, indexed by xschem layer num.
 *      this array is initialized in build_colors() by calling find_best_color()
 *      indexes are returned from XAllocNamedColor()
 *      these are used in XSetForeground and XSetBackground calls:
 *        XSetForeground(display, gc, color_index[i]) 
 *  - xcolor_array[]: 
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
 *      When all color_index[] are populated with pixel values xcolor_array[]
 *      is reset with the xschem layer colors in build_colors() using XLookupColor():
 *        for(i=0;i<cadlayers;i++) {
 *          XLookupColor(display, colormap, color_array[i], &xcolor_exact, &xcolor);
 *          xcolor_array[i] = xcolor;
 *        }
 *
 *
 *
 */
int build_colors(double dim)
{
    int i;
    if(dark_colorscheme) {
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
    init_color_array(dim);
    for(i=0;i<cadlayers;i++)
    {
     color_index[i] = find_best_color(color_array[i]);
    }
    for(i=0;i<cadlayers;i++)
    {
      XSetBackground(display, gc[i], color_index[0]); /* for dashed lines 'off' color */
      XSetForeground(display, gc[i], color_index[i]);
      XSetForeground(display, gcstipple[i], color_index[i]);
    }
    for(i=0;i<cadlayers;i++) {
      XLookupColor(display, colormap, color_array[i], &xcolor_exact, &xcolor);
      xcolor_array[i] = xcolor;
    }
    tcleval("reconfigure_layers_menu");
    return 0; /* success */
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
  if(!strcmp(what, "create")) {
    tkpre_window = Tk_NameToWindow(interp, tk_win_path, mainwindow);
    Tk_MakeWindowExist(tkpre_window);
    pre_window = Tk_WindowId(tkpre_window);
  }
  else if(!strcmp(what, "draw")) {
    Xschem_ctx *save_xctx = NULL; /* save pointer to current schematic context structure */
    char *saveptr = NULL;
    int save_mod, save_show_pin;
    Pixmap save_save_pixmap;
    Window save_window;


    save_xctx = xctx; /* save current schematic */
    xctx = NULL;      /* reset for preview */
    alloc_xschem_data(); /* alloc data into xctx */

    /* save some relevant global context */
    save_window = window;
    save_save_pixmap = save_pixmap;
    save_mod = xctx->modified;
    save_show_pin = show_pin_net_names;
    show_pin_net_names = 0;
    my_strdup(117, &saveptr, tclgetvar("current_dirname"));

    /* preview */
    check_version = 0; /* if set refuse to load and preview anything if not a rel 1.1+ xschem file */
                       /* if not set heuristics is done in xschem.tcl to ensure it is an xschem file */
    load_schematic(1,filename, 0);
    window = pre_window;
    resetwin(1, 0, 1);
    zoom_full(1, 0); /* draw */
    check_version = 0;

    /* restore context */
    tclsetvar("current_dirname", saveptr);
    my_free(1144, &saveptr);
    unselect_all();
    remove_symbols();
    clear_drawing();
    free_xschem_data();

    show_pin_net_names = save_show_pin;
    xctx = save_xctx; /* restore schematic */
    xctx->modified = save_mod;
    set_modify(xctx->modified);
    /* free the pixmap (if a different one) used for preview */
#ifdef __unix__
    if (save_pixmap != save_save_pixmap)
      XFreePixmap(display,save_pixmap);
#else
    if (save_pixmap != save_save_pixmap)
      Tk_FreePixmap(display, save_pixmap);
#endif 
    window = save_window;
    save_pixmap = save_save_pixmap;
    /* reset window (back to main window), but don't delete and create a pixmap since we
       have preserved the main window pixmap and already erased the preview pixmap
       the goal of this complicated pixmap saving is to avoid a draw() call in the main window
       to regenerate the save_pixmap every time user browses a new symbol */
    resetwin(0, 0, 0);
    change_linewidth(-1.);
    /* not needed: event loop takes care of this and don't need to regenerate save_pixmap. */
    /* draw(); */
  }
  else if(!strcmp(what, "destroy")) {
    Tk_DestroyWindow(tkpre_window);
  }
}

#ifndef __unix__
/* Source: https://www.tcl.tk/man/tcl8.7/TclCmd/glob.htm */
/* backslash character has a special meaning to glob command,
so glob patterns containing Windows style path separators need special care.*/
void change_to_unix_fn(char* fn)
{
  int len, i, ii;
  len = strlen(fn);
  ii = 0;
  for (i = 0; i < len; ++i) {
    if (fn[i]!='\\') fn[ii++] = fn[i];
    else { fn[ii++] = '/'; if (fn[i + 1] == '\\') ++i; }
  }

}
#endif

int Tcl_AppInit(Tcl_Interp *inter)
{
 char name[PATH_MAX]; /* overflow safe 20161122 */
 char tmp[2*PATH_MAX+100]; /* 20161122 overflow safe */
 int i;
 struct stat buf;
 const char *home_buff;
 int running_in_src_dir;
 /* XVisualInfo vinfo; */

 #if HAS_XCB==1
 xcb_render_query_pict_formats_reply_t *formats_reply;
 xcb_render_pictforminfo_t *formats;
 xcb_render_query_pict_formats_cookie_t formats_cookie;
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

 XSetErrorHandler(err);

 interp=inter;
 Tcl_Init(interp);
 if(has_x) Tk_Init(interp);
 if(!has_x)  tclsetvar("no_x","1");

 Tcl_CreateExitHandler(tclexit, 0);
#ifdef __unix__
 my_snprintf(tmp, S(tmp),"regsub -all {~/} {.:%s} {%s/}", XSCHEM_LIBRARY_PATH, home_dir);
 tcleval(tmp);
 tclsetvar("XSCHEM_LIBRARY_PATH", tclresult());

 running_in_src_dir = 0;
 /* test if running xschem in src/ dir (usually for testing) */
 if( !stat("./xschem.tcl", &buf) && !stat("./systemlib", &buf) && !stat("./xschem", &buf)) {
   running_in_src_dir = 1;
   tclsetvar("XSCHEM_SHAREDIR",pwd_dir); /* for testing xschem builds in src dir*/
/*
   my_snprintf(tmp, S(tmp), "subst .:[file normalize \"%s/../xschem_library/devices\"]", pwd_dir);
   tcleval(tmp);
   tclsetvar("XSCHEM_LIBRARY_PATH", tclresult());
*/
   my_snprintf(tmp, S(tmp), "subst .:[file dirname \"%s\"]/xschem_library/devices", pwd_dir);
   tcleval(tmp);
   tclsetvar("XSCHEM_LIBRARY_PATH", tclresult());
 } else if( !stat(XSCHEM_SHAREDIR, &buf) ) {
   tclsetvar("XSCHEM_SHAREDIR",XSCHEM_SHAREDIR);
   /* ... else give up searching, may set later after loading xschemrc */
 }
 /* create user conf dir , remove ~ if present */
 my_snprintf(tmp, S(tmp),"regsub {^~/} {%s} {%s/}", USER_CONF_DIR, home_dir);
 tcleval(tmp);
 my_snprintf(user_conf_dir, S(user_conf_dir), "%s", tclresult());
 tclsetvar("USER_CONF_DIR", user_conf_dir);
#else
   char install_dir[MAX_PATH]="";
   char *up_hier=NULL, *win_xschem_library_path=NULL;
   #define WIN_XSCHEM_LIBRARY_PATH_NUM 8
   const char *WIN_XSCHEM_LIBRARY_PATH[WIN_XSCHEM_LIBRARY_PATH_NUM] = {
     /*1*/ "xschem_library",
     /*2*/ "xschem_library/devices", 
     /*3*/ "xschem_library/examples", 
     /*4*/ "xschem_library/ngspice", 
     /*5*/ "xschem_library/logic", 
     /*6*/ "xschem_library/binto7seg", 
     /*7*/ "xschem_library/pcb", 
     /*8*/ "xschem_library/rom8k"};
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
 char* gxschem_library=NULL, *xschem_sharedir=NULL;
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
 /* END LOOKING FOR xschemrc */

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
 /*  END EXECUTE xschem.tcl */

 /* resolve absolute pathname of xschem (argv[0]) for future usage */
 my_strdup(44, &xschem_executable, get_file_path(xschem_executable));
 dbg(1, "Tcl_AppInit(): resolved xschem_executable=%s\n", xschem_executable);

 /* set global variables fetching data from tcl code 25122002 */
 if(tclgetvar("dark_colorscheme")[0] == '1') dark_colorscheme = 1;
 else dark_colorscheme = 0;
 if(netlist_type==-1) {
  if(!strcmp(tclgetvar("netlist_type"),"vhdl") ) netlist_type=CAD_VHDL_NETLIST;
  else if(!strcmp(tclgetvar("netlist_type"),"verilog") ) netlist_type=CAD_VERILOG_NETLIST;
  else if(!strcmp(tclgetvar("netlist_type"),"tedax") ) netlist_type=CAD_TEDAX_NETLIST;
  else if(!strcmp(tclgetvar("netlist_type"),"symbol") ) netlist_type=CAD_SYMBOL_ATTRS;
  else netlist_type=CAD_SPICE_NETLIST;
 } else {
  if(netlist_type==CAD_VHDL_NETLIST)  tclsetvar("netlist_type","vhdl");
  else if(netlist_type==CAD_VERILOG_NETLIST)  tclsetvar("netlist_type","verilog");
  else if(netlist_type==CAD_TEDAX_NETLIST)  tclsetvar("netlist_type","tedax");
  else if(netlist_type==CAD_SYMBOL_ATTRS)  tclsetvar("netlist_type","symbol");
  else tclsetvar("netlist_type","spice");
 }

 split_files=atoi(tclgetvar("split_files"));
 netlist_show=atoi(tclgetvar("netlist_show"));
 unzoom_nodrift=atoi(tclgetvar("unzoom_nodrift"));
 show_pin_net_names = atoi(tclgetvar("show_pin_net_names"));

 if(color_ps==-1)
   color_ps=atoi(tclgetvar("color_ps"));
 else  {
   my_snprintf(tmp, S(tmp), "%d",color_ps);
   tclsetvar("color_ps",tmp);
 }
 change_lw=atoi(tclgetvar("change_lw"));
 draw_window=atoi(tclgetvar("draw_window"));
 incr_hilight=atoi(tclgetvar("incr_hilight"));
 if(a3page==-1)
   a3page=atoi(tclgetvar("a3page"));
 else  {
   my_snprintf(tmp, S(tmp), "%d",a3page);
   tclsetvar("a3page",tmp);
 }
 enable_stretch=atoi(tclgetvar("enable_stretch"));
 draw_grid=atoi(tclgetvar("draw_grid"));
 cadlayers=atoi(tclgetvar("cadlayers"));
 if(debug_var==-10) debug_var=0;

 /*                             */
 /*  [m]allocate dynamic memory */
 /*                             */
 alloc_data();

 #ifndef IN_MEMORY_UNDO
 /* 20150327 create undo directory */
 /* 20180923 no more mkdtemp (portability issues) */
 if( !my_strdup(644, &undo_dirname, create_tmpdir("xschem_undo_") )) {
   fprintf(errfp, "xinit(): problems creating tmp undo dir\n");
   tcleval( "exit");
 }
 dbg(1, "undo_dirname=%s\n", undo_dirname);
 #endif

 init_pixdata();
 init_color_array(0.0);
 my_snprintf(tmp, S(tmp), "%d",debug_var);
 tclsetvar("tcl_debug",tmp );
 tclsetvar("menu_tcl_debug",debug_var ? "1" : "0" );
 if(flat_netlist) tclsetvar("flat_netlist","1");

 xctx->xschem_w = CADWIDTH;
 xctx->xschem_h = CADHEIGHT;
 xctx->areaw = CADWIDTH+4*INT_WIDTH(xctx->lw);  /* clip area extends 1 pixel beyond physical window area */
 xctx->areah = CADHEIGHT+4*INT_WIDTH(xctx->lw); /* to avoid drawing clipped rectangle borders at window edges */
 xctx->areax1 = -2*INT_WIDTH(xctx->lw);
 xctx->areay1 = -2*INT_WIDTH(xctx->lw);
 xctx->areax2 = xctx->areaw-2*INT_WIDTH(xctx->lw);
 xctx->areay2 = xctx->areah-2*INT_WIDTH(xctx->lw);
 xrect[0].x = 0;
 xrect[0].y = 0;
 xrect[0].width = CADWIDTH;
 xrect[0].height = CADHEIGHT;


 my_strncpy(xctx->file_version, XSCHEM_FILE_VERSION, S(xctx->file_version));
 /* compile_font() needs enabled layers, these are set after xschem.tcl loading completed
  * so we temporarily enable all them here */
 for(i = 0; i < cadlayers; i++) enable_layer[i] = 1;
 compile_font();
 /* restore current dir after loading font */
 if(tcleval("info exists env(PWD)")[0] == '1') {
   /* $env(PWD) better than pwd_dir as it does not dereference symlinks */
   tcleval("set current_dirname $env(PWD)");
 } else {
   Tcl_VarEval(interp, "set current_dirname ", pwd_dir, NULL);
 }
 /* 
 my_snprintf(tmp, S(tmp), "set current_dirname \"%s\"", pwd_dir);
 tcleval(tmp);
 */

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
    window = Tk_WindowId(tkwindow);
    topwindow = Tk_WindowId(mainwindow);

    dbg(1, "Tcl_AppInit(): drawing window ID=0x%lx\n",window);
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
    my_free(1145, &formats_reply);
    /*/---------------------------------------------------- */
    /* /20171125 */
    #endif /*HAS_XCB */

    screen_number = DefaultScreen(display);
    colormap = DefaultColormap(display, screen_number);
    depth = DisplayPlanes(display, screen_number);
    dbg(1, "Tcl_AppInit(): screen depth: %d\n",depth);

    visual = DefaultVisual(display, screen_number);

    /*
    if (!XMatchVisualInfo(
        display, XDefaultScreen(display), 24, TrueColor, &vinfo)
    ) return fprintf(errfp, "no 32 bit visual\n");
    visual = vinfo.visual;
    */
    dbg(1, "Tcl_AppInit(): done step b of xinit()\n");
    rectcolor= 4;  /* this is the current layer when xschem started. */
    for(i=0;i<cadlayers;i++)
    {
     pixmap[i] = XCreateBitmapFromData(display, window, (char*)(pixdata[i]),16,16);
     gc[i] = XCreateGC(display,window,0L,NULL);
     gcstipple[i] = XCreateGC(display,window,0L,NULL);
     XSetStipple(display,gcstipple[i],pixmap[i]);
     if(fill_type[i]==1)  XSetFillStyle(display,gcstipple[i],FillSolid);
     else XSetFillStyle(display,gcstipple[i],FillStippled);
    }
    gctiled = XCreateGC(display,window,0L, NULL);
    dbg(1, "Tcl_AppInit(): done step c of xinit()\n");
    if(build_colors(0.0)) exit(-1);
    dbg(1, "Tcl_AppInit(): done step e of xinit()\n");
    /* save_pixmap must be created as resetwin() frees it before recreating with new size. */
#ifdef __unix__
    save_pixmap = XCreatePixmap(display,window,CADWIDTH,CADHEIGHT,depth);
#else
    save_pixmap = Tk_GetPixmap(display, window, CADWIDTH, CADHEIGHT, depth);
#endif
    XSetTile(display, gctiled, save_pixmap);
    XSetFillStyle(display,gctiled,FillTiled);
    #ifdef HAS_CAIRO
    {
      XWindowAttributes wattr;
      XGetWindowAttributes(display, window, &wattr);
      #if HAS_XRENDER==1
      #if HAS_XCB==1
      sfc = cairo_xcb_surface_create_with_xrender_format(xcbconn,
            screen_xcb, window, &format_rgb, 1 , 1);
      save_sfc = cairo_xcb_surface_create_with_xrender_format(xcbconn,
                 screen_xcb, save_pixmap, &format_rgb, 1 , 1);
      #else
      format = XRenderFindStandardFormat(display, PictStandardRGB24);
      sfc = cairo_xlib_surface_create_with_xrender_format (display,
            window, DefaultScreenOfDisplay(display), format, 1, 1);
      save_sfc = cairo_xlib_surface_create_with_xrender_format(
                 display, save_pixmap, DefaultScreenOfDisplay(display), format, 1, 1);
      #endif
      #else
      sfc = cairo_xlib_surface_create(display, window, visual, wattr.width, wattr.height);
      save_sfc = cairo_xlib_surface_create(display, save_pixmap, visual, wattr.width, wattr.height);
      #endif
      if(cairo_surface_status(sfc)!=CAIRO_STATUS_SUCCESS) {
        fprintf(errfp, "ERROR: invalid cairo surface\n");
        return 1;
      }
      if(cairo_surface_status(save_sfc)!=CAIRO_STATUS_SUCCESS) {
        fprintf(errfp, "ERROR: invalid cairo surface\n");
        return 1;
      }
      cairo_ctx = cairo_create(sfc);
      cairo_save_ctx = cairo_create(save_sfc);

      #if 0
      {
        cairo_font_options_t *cfo;
        cfo = cairo_font_options_create ();
        cairo_font_options_set_antialias(cfo, CAIRO_ANTIALIAS_DEFAULT); /* CAIRO_ANTIALIAS_NONE */
        cairo_set_font_options (cairo_ctx, cfo);
        cairo_set_font_options (cairo_save_ctx, cfo);
      }
      #endif

      /* load font from tcl 20171112 */
      tcleval("xschem set svg_font_name $svg_font_name");
      tcleval("xschem set cairo_font_name $cairo_font_name");
      tclsetvar("has_cairo","1");
      cairo_select_font_face (cairo_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cairo_ctx, 20);
      cairo_select_font_face (cairo_save_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cairo_save_ctx, 20);

      cairo_save_ctx = cairo_create(save_sfc);
      cairo_set_line_width(cairo_ctx, 1);
      cairo_set_line_width(cairo_save_ctx, 1);
      cairo_set_line_join(cairo_ctx, CAIRO_LINE_JOIN_ROUND);
      cairo_set_line_cap(cairo_ctx, CAIRO_LINE_CAP_ROUND);
      cairo_set_line_join(cairo_save_ctx, CAIRO_LINE_JOIN_ROUND);
      cairo_set_line_cap(cairo_save_ctx, CAIRO_LINE_CAP_ROUND);

    }
    #endif /* HAS_CAIRO */

    change_linewidth(0.);
    dbg(1, "Tcl_AppInit(): done xinit()\n");
    winattr.backing_store = WhenMapped;
    /* winattr.backing_store = NotUseful;*/
    Tk_ChangeWindowAttributes(tkwindow, CWBackingStore, &winattr);

    dbg(1, "Tcl_AppInit(): sizeof xInstance=%lu , sizeof xSymbol=%lu\n",
             (unsigned long) sizeof(xInstance),(unsigned long) sizeof(xSymbol));


    tcleval("xschem line_width $line_width");
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
 /* Completing tk windows creation (see xschem.tcl, build_windows) and event binding */
 /* *AFTER* X initialization done                                                    */
 /*                                                                                  */
 if(has_x) tcleval("build_windows");

 fullscreen=atoi(tclgetvar("fullscreen"));
 if(fullscreen) {
   fullscreen = 0;
   tcleval("update");
   toggle_fullscreen();
 }

 /*                                */
 /*  START PROCESSING USER OPTIONS */
 /*                                */

 /* set tcl netlist_dir if netlist_dir given on cmdline */
 if(netlist_dir && netlist_dir[0]) tclsetvar("netlist_dir", netlist_dir);

 if(!set_netlist_dir(0, NULL)) {
   fprintf(errfp, "problems creating netlist directory %s\n", netlist_dir ? netlist_dir : "<NULL>");
 }

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
       to tcl is_xschem_file that could change netlist_type to symbol */
    load_schematic(1, f, !do_netlist);
    Tcl_VarEval(interp, "update_recent_file {", f, "}", NULL);
 } else {
   char * tmp;
   char filename[PATH_MAX];
   tmp = (char *) tclgetvar("XSCHEM_START_WINDOW");
   dbg(1, "Tcl_AppInit(): tmp=%s\n", tmp? tmp: "NULL");
   my_strncpy(filename, abs_sym_path(tmp, ""), S(filename));
    /* if do_netlist=1 call load_schematic with 'reset_undo=0' avoiding call 
       to tcl is_xschem_file that could change netlist_type to symbol */
   load_schematic(1, filename, !do_netlist);
 }



 zoom_full(0, 0);   /* Necessary to tell xschem the
                  * initial area to display
                  */
 pending_fullzoom=1;
 if(do_netlist) {
   if(debug_var>=1) {
     if(flat_netlist)
       fprintf(errfp, "xschem: flat netlist requested\n");
   }
   if(!filename) {
     fprintf(errfp, "xschem: cant do a netlist without a filename\n");
     tcleval( "exit");
   }
   if(netlist_dir && netlist_dir[0]) {
     if(netlist_type == CAD_SPICE_NETLIST)
       global_spice_netlist(1);                  /* 1 means global netlist */
     else if(netlist_type == CAD_VHDL_NETLIST)
       global_vhdl_netlist(1);                   /* 1 means global netlist */
     else if(netlist_type == CAD_VERILOG_NETLIST)
       global_verilog_netlist(1);                /* 1 means global netlist */
     else if(netlist_type == CAD_TEDAX_NETLIST)
       global_tedax_netlist(1);                  /* 1 means global netlist */
   } else {
    fprintf(errfp, "xschem: please set netlist_dir in xschemrc\n");
   }
 }
 if(do_print) {
   if(!filename) {
     dbg(0, "xschem: can't do a print without a filename\n");
     tcleval( "exit");
   }
   if(do_print==1) ps_draw();
   else if(do_print == 2) {
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
     tcleval( "exit");
   }
   tcleval( "simulate");
 }

 if(do_waves) {
   if(!filename) {
     fprintf(errfp, "xschem: can't show simulation waves without a filename\n");
     tcleval( "exit");
   }
   tcleval( "waves [file tail \"[xschem get schname]\"]");
 }

 if(tcl_script[0]) {
   Tcl_VarEval(interp, "source ", tcl_script, NULL);
 }

 if(quit) {
   tcleval( "exit");
 }

 /* */
 /*  END PROCESSING USER OPTIONS */
 /* */


 if(
#ifdef __unix__
    !batch_mode &&
#endif
    !no_readline) {
   tcleval( "if {![catch {package require tclreadline}]} "
      "{::tclreadline::readline customcompleter  completer; ::tclreadline::Loop }" ) ;
 }

 dbg(1, "Tcl_AppInit(): returning TCL_OK\n");
 return TCL_OK;
}



