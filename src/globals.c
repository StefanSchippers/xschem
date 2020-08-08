/* File: globals.c
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

int help=0; /* help option set to global scope, printing help is deferred */
            /* when configuration ~/.schem has been read 20140406 */
int fullscreen=0;
int semaphore=0; /* needed at global scope as it is set by tcl */
int unzoom_nodrift=1;
int a3page=-1;
int has_x=1;
int no_draw=0;
int sym_txt=1;
int event_reporting=0;
int rainbow_colors=0;
int manhattan_lines=0;
FILE *errfp; 
char *filename=NULL; /* filename given on cmdline */
char user_conf_dir[PATH_MAX];
char home_dir[PATH_MAX]; /* home dir obtained via getpwuid */
char pwd_dir[PATH_MAX];  /* obtained via getcwd() */
#ifndef __unix__
char win_temp_dir[PATH_MAX]="";
#endif
int load_initfile=1;
char plotfile[PATH_MAX] = {'\0'};
char rcfile[PATH_MAX] = {'\0'};
char *tcl_command = NULL;
char tcl_script[PATH_MAX] = {'\0'};
int persistent_command=0; /* remember last command 20181022 */
int disable_unique_names=0; /* if set allow instances with duplicate names */
int quit=0;  /* set from process_options (ex netlist from cmdline and quit) */
int debug_var=-10;  /* will be set to 0 in xinit.c */
int tcp_port = 0;
int do_print=0;
int no_readline=0;
Colormap colormap;
int lw=0; /* line width */
int bus_width = BUS_WIDTH; 
double lw_double=0.0; /* line width */
int fill=1; /* filled rectangles */
int draw_pixmap=1; /* use pixmap for double buffer */
int draw_window=0; /* 20181009 */
int draw_grid=1;
double cadgrid = CADGRID;
double cadhalfdotsize = CADHALFDOTSIZE;
int current_type=SCHEMATIC;
char current_name[PATH_MAX]; /* 20190519 */
int change_lw=0; /* allow change linewidth */
int incr_hilight=1;
int auto_hilight=0;
unsigned int color_index[256]; /* layer color lookup table */
unsigned int rectcolor ; /* this is the currently used layer */
unsigned long ui_state = 0; /* this signals that we are doing a net place,panning etc. */
                          /* used to prevent nesting of some commands */

char *undo_dirname = NULL; /* 20150327 */
int cur_undo_ptr=0;
int tail_undo_ptr=0;
int head_undo_ptr=0;
int max_undo=MAX_UNDO;
int draw_dots=1; /*20150331 */
int draw_single_layer=-1; /* 20151117 */
int check_version = 0; /* if set ensures 'v' version header line is present before loading file */
int yyparse_error = 0;
unsigned short enable_stretch=0;
int cadlayers=0;
int *enable_layer;
int n_active_layers=0;
int *active_layer;
int need_rebuild_selected_array=1;
Window window; /* window is the drawing area, topwindow is the root win */
Window pre_window; /* preview when opening files */
Window parent_of_topwindow;
int depth;
int *fill_type; /*20171117 for every layer: 0: no fill, 1, solid fill, 2: stipple fill */
unsigned char **pixdata;
unsigned char pixdata_init[22][32]={    /* fill patterns... indexed by laynumb. */
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,/*0 */
 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,/*1 */
 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},/*2 */
{/*3 */
   0x55, 0x55, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00,
   0x55, 0x55, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00
},
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,/*4 */
 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,/*5 */
 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
{/*6 */
   0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
},
{/*7 */
   0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02
},
{/*8 */
   0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
},
{/*9 */
   0x04, 0x41, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00, 0x41, 0x10, 0x00, 0x00,
   0x04, 0x41, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00, 0x41, 0x10, 0x00, 0x00,
   0x04, 0x41, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00
},
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,/*10 */
 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
{/*11 */
   0x11, 0x11, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00,
   0x44, 0x44, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00,
   0x11, 0x11, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00
},
{/*12 */
   0x04, 0x04, 0x02, 0x02, 0x01, 0x01, 0x80, 0x80, 0x40, 0x40, 0x20, 0x20,
   0x10, 0x10, 0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x01, 0x01, 0x80, 0x80,
   0x40, 0x40, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08
},
{/*13 */
   0x11, 0x11, 0x22, 0x22, 0x44, 0x44, 0x88, 0x88, 0x11, 0x11, 0x22, 0x22, 0x44, 0x44, 0x88, 0x88,
   0x11, 0x11, 0x22, 0x22, 0x44, 0x44, 0x88, 0x88, 0x11, 0x11, 0x22, 0x22, 0x44, 0x44, 0x88, 0x88
},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},/*14 */
{/*15 */
   0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
},
{  0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x00, 0x00,/*16 */
   0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},/*17 */
{0x55,0x55,0xaa,0xaa,0x55,0x55,0xaa,0xaa,0x55,0x55,0xaa,0xaa,0x55,0x55,0xaa,0xaa,/*18 */
 0x55,0x55,0xaa,0xaa,0x55,0x55,0xaa,0xaa,0x55,0x55,0xaa,0xaa,0x55,0x55,0xaa,0xaa},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},/*19 */
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},/*20 */
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}/*21 */
};

char **color_array;

char *xschem_executable=NULL;
Pixmap cad_icon_pixmap=0, *pixmap,save_pixmap;  /* save_pixmap used to restore window */
int areax1,areay1,areax2,areay2,areaw,areah; /* window corners / size */
GC *gcstipple,*gc, gctiled;
Display *display;
XRectangle xrect[1] = {{0,0,0,0}};
int xschem_h, xschem_w; /* 20171130 window size */
double mousex,mousey; /* mouse coord. */
double mousex_snap,mousey_snap; /* mouse coord. snapped to grid */
double mx_double_save, my_double_save; /* 20070322 */

/*double xorigin=-CADWIDTH/2.0,yorigin=-CADHEIGHT/2.0; */
double cadsnap = CADSNAP;

double zoom=CADINITIALZOOM;
double mooz=1/CADINITIALZOOM;
double xorigin=CADINITIALX;
double yorigin=CADINITIALY;
double *character[256];
 int lastselected = 0;
Selected *selectedgroup;     /*  array of selected objects to be */
                             /*  drawn while moving if < MAXGROUP selected */
XPoint *gridpoint;           /* pointer to array of gridpoints, used in draw() */
Tcl_Interp *interp;
 int max_texts;
 int max_wires;
 int max_instances;
 int max_symbols;
 int max_selected;
 int *max_rects;
 int *max_polygons; /* 20171115 */
 int *max_arcs; /* 20181012 */
 int *max_lines;     

int do_netlist=0;  /* set by process_options if user wants netllist from cmdline */
int do_simulation=0;  /* 20171007 */
int do_waves=0;  /* 20171007 */
int netlist_count=0; /* netlist counter incremented at any cell being netlisted */
int top_subckt = 0;
int spiceprefix = 1;
int netlist_show=0;
int flat_netlist=0;
int netlist_type=-1;
char bus_replacement_char[3] = {0, 0, 0};
int prepared_netlist_structs=0;
int prepared_hilight_structs=0;
int prepared_hash_instances=0;
int prepared_hash_wires=0;
/* */
/* following data is relative to the current schematic */
/* */
Wire *wire;
int lastwire = 0;
Instance *inst_ptr;          /* Pointer to element INSTANCE */
int lastinst = 0;
Instdef *instdef;            /* Pointer to element definition */
int lastinstdef = 0;
Box  **rect;
int *lastrect;
xPolygon **polygon; /* 20171115 */
int *lastpolygon; /* 20171115 */
xArc **arc;
int *lastarc;
Line **line;
int *lastline;
Text *textelement;
int lasttext=0;
char schematic[CADMAXHIER][PATH_MAX];
int currentsch = 0;
char *schprop=NULL;  /* spice */
char *schtedaxprop=NULL;  /* tEDAx */
char *schvhdlprop=NULL; /* vhdl and symbol property string */
char *schsymbolprop=NULL; /* vhdl and symbol property string */
char *xschem_version_string=NULL; /* vhdl and symbol property string */
char file_version[100];
char *schverilogprop=NULL;/* verilog */
int show_erc=1;
int hilight_nets=0;
char *sch_path[CADMAXHIER];
int sch_inst_number[CADMAXHIER];
int previous_instance[CADMAXHIER]; /* to remember the instance we came from when going up the hier. */
int modified = 0;
int color_ps=-1;
int only_probes=0; /* 20110112 */
int hilight_color=0;
Zoom zoom_array[CADMAXHIER];
int pending_fullzoom=0;
int split_files=0; /* split netlist files 20081202 */
char *netlist_dir=NULL; /* user set netlist directory via cmd-option or menu or xschemrc */
char user_top_netl_name[PATH_MAX] = ""; /* user set netlist name via cmd option -N <name> */
int horizontal_move=0; /* 20171023 */
int vertical_move=0; /* 20171023 */
XColor xcolor_array[256];/* 20171109 */
Visual *visual; /*20171111 */
int dark_colorscheme=1; /* 20171113 */
double color_dim=0.0; /* 20171123 */
int no_undo=0; /* 20171204 */
int enable_drill=0; /* 20171211 pass net hilights through components with 'propagate_to' property set on pins */
struct instpinentry *instpintable[NBOXES][NBOXES];
struct wireentry *wiretable[NBOXES][NBOXES];
struct instentry *insttable[NBOXES][NBOXES];
size_t get_tok_value_size;
size_t get_tok_size;


#ifdef HAS_CAIRO
cairo_surface_t *sfc, *save_sfc;
cairo_t *ctx, *save_ctx;
XRenderPictFormat *format;

#if HAS_XCB==1
xcb_connection_t *xcbconn; /* 20171125 */
xcb_render_pictforminfo_t format_rgb, format_rgba;
xcb_screen_t *screen_xcb;
xcb_visualtype_t *visual_xcb;
#endif /*HAS_XCB */

#endif /*HAS_CAIRO */
char cairo_font_name[1024]="Monospace";
int cairo_longest_line;
int cairo_lines;
double cairo_font_scale=1.0; /* default: 1.0, allows to adjust font size */
double nocairo_font_xscale=0.85; /* match with cairo sizing */
double nocairo_font_yscale=0.88; /* match with cairo sizing */
double cairo_font_line_spacing=1.0; /* allows to change line spacing: default: 1.0 */

/* lift up the text by 'n' pixels (zoom corrected) within the bbox.  */
/* This correction is used to better align existing schematics */
/* compared to the nocairo xschem version. */
/* allowed values should be in the range [-4, 4] */
double cairo_vert_correct=0.0;
double nocairo_vert_correct=0.0;
                               
