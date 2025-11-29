/* File: paste.c
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





static void merge_text(FILE *fd)
{
   int i;
    check_text_storage();
    i=xctx->texts;
     xctx->text[i].txt_ptr=NULL;
     load_ascii_string(&xctx->text[i].txt_ptr,fd);
     if(fscanf(fd, "%lf %lf %hd %hd %lf %lf ",
       &xctx->text[i].x0, &xctx->text[i].y0, &xctx->text[i].rot,
       &xctx->text[i].flip, &xctx->text[i].xscale,
       &xctx->text[i].yscale) <6) {
         fprintf(errfp,"merge_text(): WARNING:  missing fields for TEXT object, ignoring\n");
         read_line(fd, 0);
         return;
     }
     xctx->text[i].prop_ptr=NULL;
     xctx->text[i].font=NULL;
     xctx->text[i].floater_instname=NULL;
     xctx->text[i].floater_ptr=NULL;
     xctx->text[i].sel=0;
     load_ascii_string(&xctx->text[i].prop_ptr,fd);
     set_text_flags(&xctx->text[i]);
     select_text(i,SELECTED, 1, 1);
     xctx->texts++;
}

static void merge_wire(FILE *fd)
{
    int i;
    double x1,y1,x2,y2;
    char *ptr=NULL;
    i=xctx->wires;
    if(fscanf(fd, "%lf %lf %lf %lf",&x1, &y1, &x2, &y2 ) < 4) {
      fprintf(errfp,"merge_wire(): WARNING:  missing fields for WIRE object, ignoring\n");
      read_line(fd, 0);
      return;
    }
    load_ascii_string( &ptr, fd);
    storeobject(-1, x1,y1,x2,y2,WIRE,0,SELECTED,ptr);
    my_free(_ALLOC_ID_, &ptr);
    select_wire(i, SELECTED, 1, 1);
}

static void merge_box(FILE *fd)
{
    int i,c,n;
    xRect *ptr;
    const char *attr, *fill_ptr;

    n = fscanf(fd, "%d",&c);
    if(n != 1 || c < 0 || c >= cadlayers) {
      fprintf(errfp,"merge_arc(): WARNING: wrong or missing layer number for xRECT object, ignoring.\n");
      read_line(fd, 0);
      return;
    }
    check_box_storage(c);
    i=xctx->rects[c];
    ptr=xctx->rect[c];
    if(fscanf(fd, "%lf %lf %lf %lf ",&ptr[i].x1, &ptr[i].y1,
       &ptr[i].x2, &ptr[i].y2) < 4) {
      fprintf(errfp,"merge_arc(): WARNING:  missing fields for xRECT object, ignoring\n");
      read_line(fd, 0);
      return;
    }
    ptr[i].prop_ptr=NULL;
    ptr[i].extraptr=NULL;
    RECTORDER(ptr[i].x1, ptr[i].y1, ptr[i].x2, ptr[i].y2);
    ptr[i].sel=0;
    load_ascii_string( &ptr[i].prop_ptr, fd);
    ptr[i].bus = get_attr_val(get_tok_value(ptr[i].prop_ptr, "bus", 0));
    attr = get_tok_value(ptr[i].prop_ptr,"dash",0);
    if(strcmp(attr, "")) {
      int d = atoi(attr);
      ptr[i].dash = (short)(d >= 0 ? d : 0);
    } else {
      ptr[i].dash = 0;
    }

    attr = get_tok_value(ptr[i].prop_ptr,"ellipse",0);
    if(strcmp(attr, "")) {
      int a;
      int b;
      if(sscanf(attr, "%d%*[ ,]%d", &a, &b) != 2) {
        a = 0;
        b = 360;
      }
      ptr[i].ellipse_a = a;
      ptr[i].ellipse_b = b;
    } else {
      ptr[i].ellipse_a = -1;
      ptr[i].ellipse_b = -1;
    }

    fill_ptr = get_tok_value(ptr[i].prop_ptr,"fill",0);
    if( !strcmp(fill_ptr, "full") )
      ptr[i].fill = 2;
    else if( !strboolcmp(fill_ptr, "false") )
      ptr[i].fill = 0;
    else
      ptr[i].fill = 1;
    set_rect_flags(&xctx->rect[c][i]); /* set cached .flags bitmask from on attributes */
    select_box(c,i, SELECTED, 1, 1);
    xctx->rects[c]++;
}

static void merge_arc(FILE *fd)
{
    int i,c,n;
    xArc *ptr;
    const char *dash, *fill_ptr;

    n = fscanf(fd, "%d",&c);
    if(n != 1 || c < 0 || c >= cadlayers) {
      fprintf(errfp,"merge_arc(): WARNING: wrong or missing layer number for ARC object, ignoring.\n");
      read_line(fd, 0);
      return;
    }
    check_arc_storage(c);
    i=xctx->arcs[c];
    ptr=xctx->arc[c];
    if(fscanf(fd, "%lf %lf %lf %lf %lf ",&ptr[i].x, &ptr[i].y,
           &ptr[i].r, &ptr[i].a, &ptr[i].b) < 5) {
      fprintf(errfp,"merge_arc(): WARNING:  missing fields for ARC object, ignoring\n");
      read_line(fd, 0);
      return;
    }

    ptr[i].prop_ptr=NULL;
    ptr[i].sel=0;
    load_ascii_string(&ptr[i].prop_ptr, fd);
    ptr[i].bus = get_attr_val(get_tok_value(ptr[i].prop_ptr,"bus",0));
    fill_ptr = get_tok_value(ptr[i].prop_ptr,"fill",0);
    if( !strcmp(fill_ptr, "full") )
      ptr[i].fill = 2; /* bit 1: solid fill (not stippled) */
    else if( !strboolcmp(fill_ptr, "true") )
      ptr[i].fill = 1;
    else
      ptr[i].fill = 0;
    dash = get_tok_value(ptr[i].prop_ptr,"dash",0);
    if(strcmp(dash, "")) {
      int d = atoi(dash);
      ptr[i].dash = (short)(d >= 0 ? d : 0);
    } else {
      ptr[i].dash = 0;
    }
    select_arc(c,i, SELECTED, 1, 1);
    xctx->arcs[c]++;
}


static void merge_polygon(FILE *fd)
{
    const char *fill_ptr;
    int i,c, j, points;
    xPoly *ptr;
    const char *dash;

    if(fscanf(fd, "%d %d",&c, &points)<2) {
      fprintf(errfp,"merge_polygon(): WARNING: missing fields for POLYGON object, ignoring.\n");
      read_line(fd, 0);
      return;
    }
    if(c < 0 || c>=cadlayers) {
      fprintf(errfp,"merge_polygon(): Rectangle layer > defined cadlayers, increase cadlayers\n");
      read_line(fd, 0);
      return;
    }
    check_polygon_storage(c);
    i=xctx->polygons[c];
    ptr=xctx->poly[c];
    ptr[i].x=NULL;
    ptr[i].y=NULL;
    ptr[i].selected_point=NULL;
    ptr[i].prop_ptr=NULL;
    ptr[i].x = my_calloc(_ALLOC_ID_, points, sizeof(double));
    ptr[i].y = my_calloc(_ALLOC_ID_, points, sizeof(double));
    ptr[i].selected_point= my_calloc(_ALLOC_ID_, points, sizeof(unsigned short));
    ptr[i].points=points;
    ptr[i].sel=0;
    for(j=0;j<points; ++j) {
      if(fscanf(fd, "%lf %lf ",&(ptr[i].x[j]), &(ptr[i].y[j]))<2) {
        fprintf(errfp,"merge_polygon(): WARNING: missing fields for POLYGON points, ignoring.\n");
        my_free(_ALLOC_ID_, &ptr[i].x);
        my_free(_ALLOC_ID_, &ptr[i].y);
        my_free(_ALLOC_ID_, &ptr[i].selected_point);
        read_line(fd, 0);
        return;
      }
    }
    load_ascii_string( &ptr[i].prop_ptr, fd);
    ptr[i].bus = get_attr_val(get_tok_value(ptr[i].prop_ptr, "bus", 0));
    fill_ptr = get_tok_value(ptr[i].prop_ptr,"fill",0);
    if( !strcmp(fill_ptr, "full") )
      ptr[i].fill = 2; /* bit 1: solid fill (not stippled) */
    else if( !strboolcmp(fill_ptr, "true") )
      ptr[i].fill = 1;
    else
      ptr[i].fill = 0;
    dash = get_tok_value(ptr[i].prop_ptr,"dash",0);
    if(strcmp(dash, "")) {
      int d = atoi(dash);
      ptr[i].dash = (short)(d >= 0 ? d : 0);
    } else {
      ptr[i].dash = 0;
    }
    select_polygon(c,i, SELECTED, 1, 1);
    xctx->polygons[c]++;
}

static void merge_line(FILE *fd)
{
    int i,c,n;
    xLine *ptr;
    const char *dash;

    n = fscanf(fd, "%d",&c);
    if(n != 1 || c < 0 || c >= cadlayers) {
      fprintf(errfp,"merge_line(): WARNING: Wrong or missing layer number for LINE object, ignoring\n");
      read_line(fd, 0);
      return;
    }
    check_line_storage(c);
    i=xctx->lines[c];
    ptr=xctx->line[c];
    if(fscanf(fd, "%lf %lf %lf %lf ",&ptr[i].x1, &ptr[i].y1, &ptr[i].x2, &ptr[i].y2) < 4) {
      fprintf(errfp,"merge_line(): WARNING:  missing fields for LINE object, ignoring\n");
      read_line(fd, 0);
      return;
    }
    ORDER(ptr[i].x1, ptr[i].y1, ptr[i].x2, ptr[i].y2);
    ptr[i].prop_ptr=NULL;
    ptr[i].sel=0;
    load_ascii_string( &ptr[i].prop_ptr, fd);
    ptr[i].bus = get_attr_val(get_tok_value(ptr[i].prop_ptr, "bus", 0));
    dash = get_tok_value(ptr[i].prop_ptr,"dash",0);
    if(strcmp(dash, "")) {
      int d = atoi(dash);
      ptr[i].dash = (short)(d >= 0 ? d : 0);
    } else {
      ptr[i].dash = 0;
    }
    select_line(c,i, SELECTED, 1, 1);
    xctx->lines[c]++;
}

static void merge_inst(int k,FILE *fd)
{
    int i;
    char *prop_ptr=NULL;
    char *tmp = NULL;
    i=xctx->instances;
    check_inst_storage();
    xctx->inst[i].name=NULL;
    load_ascii_string(&tmp, fd);
    /* avoid as much as possible calls to rel_sym_path (slow) */
    #ifdef __unix__
    if(tmp[0] == '/') my_strdup(_ALLOC_ID_, &xctx->inst[i].name, rel_sym_path(tmp));
    else my_strdup(_ALLOC_ID_, &xctx->inst[i].name,tmp);
    #else
    my_strdup(_ALLOC_ID_, &xctx->inst[i].name, rel_sym_path(tmp));
    #endif
    my_free(_ALLOC_ID_, &tmp);
    if(fscanf(fd, "%lf %lf %hd %hd",&xctx->inst[i].x0, &xctx->inst[i].y0,&xctx->inst[i].rot, &xctx->inst[i].flip) < 4) {
      fprintf(errfp,"WARNING: missing fields for INSTANCE object, ignoring.\n");
      read_line(fd, 0);
      return;
    }
    xctx->inst[i].sel=0;
    xctx->inst[i].color=-10000;
    xctx->inst[i].ptr=-1;
    xctx->inst[i].instname=NULL;
    xctx->inst[i].prop_ptr=NULL;
    xctx->inst[i].lab=NULL;  /* assigned in link_symbols_to_instances */
    xctx->inst[i].node=NULL;
    load_ascii_string(&prop_ptr,fd);
    my_strdup(_ALLOC_ID_, &xctx->inst[i].prop_ptr, prop_ptr);
    set_inst_flags(&xctx->inst[i]);
    if(!k) hash_names(-1, XINSERT);
    new_prop_string(i, prop_ptr, tclgetboolvar("disable_unique_names")); /* will also assign .instname */
    /* the final tmp argument is zero for the 1st call and used in */
    /* new_prop_string() for cleaning some internal caches. */
    hash_names(i, XINSERT);
    my_free(_ALLOC_ID_, &prop_ptr);
    xctx->instances++;
}

/* merge selection if selection_load=1, otherwise ask for filename
 * selection_load:
 *                      0: ask filename to merge
 *                         if ext=="" else use ext as name 
 *                      1: merge selection
 *                      2: merge clipboard
 *                      if bit 3 is set do not start a  move_objects(RUBBER,0,0,0)
 *                      to avoid graphical artifacts if doing a xschem paste with x and y offsets
 *                      from script
 */
void merge_file(int selection_load, const char ext[])
{
    FILE *fd;
    int k=0, old;
    int endfile=0;
    char *name;
    char filename[PATH_MAX];
    char tag[1]; /* overflow safe */
    char tmp[256]; /* 20161122 overflow safe */
    char *aux_ptr=NULL;
    int got_mouse, generator = 0;
    int rubber = 1;

    rubber = !(selection_load & 8);
    selection_load &= 7;
    xctx->paste_from = 0;
    if(selection_load==0)
    {
     if(!strcmp(ext,"")) {
       my_snprintf(tmp, S(tmp), "load_file_dialog {Merge file} {*} INITIALLOADDIR");
       tcleval(tmp);
       if(!strcmp(tclresult(),"")) return;
       my_strncpy(filename, (char *)tclresult(), S(filename));
       name = filename;
       xctx->paste_from = 3;
     }
     else {
       my_strncpy(filename, ext, S(filename));
       name = filename;
     }
     dbg(1, "merge_file(): sch=%d name=%s\n",xctx->currsch,name);
    }
    else if(selection_load==1)
    {
      name = sel_file;
      xctx->paste_from = 1;
    }
    else    /* selection_load==2, clipboard load */
    {
      name = clip_file;
      xctx->paste_from = 2;
    }

    if(is_generator(name)) generator = 1;

    if(generator) {
      char *cmd;
      cmd = get_generator_command(name);
      if(cmd) {
        fd = popen(cmd, "r");
        my_free(_ALLOC_ID_, &cmd);
      } else fd = NULL;
    } else {
      fd=my_fopen(name, fopen_read_mode);
    }
    if(fd) {
     xctx->prep_hi_structs=0;
     xctx->prep_net_structs=0;
     xctx->prep_hash_inst=0;
     xctx->prep_hash_wires=0;
     got_mouse = 0;
     xctx->push_undo();
     unselect_all(1);
     old=xctx->instances;
     while(!endfile)
     {
      if(fscanf(fd," %c",tag)==EOF) break;
      switch(tag[0])
      {
       case 'v':
        load_ascii_string(&aux_ptr, fd);
        break;
       case '#':
        read_line(fd, 1);
        break;
       case 'F': /* extension for future symbol floater labels */
        read_line(fd, 1);
        break;
       case 'V':
        load_ascii_string(&aux_ptr, fd);
        break;
       case 'E':
        load_ascii_string(&aux_ptr, fd);
        break;
       case 'S':
        load_ascii_string(&aux_ptr, fd);
        break;
       case 'K':
        load_ascii_string(&aux_ptr, fd);
        break;
       case 'G':
        load_ascii_string(&aux_ptr, fd);
        if(selection_load)
        {
          xctx->mx_double_save = xctx->mousex_snap;
          xctx->my_double_save = xctx->mousey_snap;
          sscanf( aux_ptr, "%lf %lf", &xctx->mousex_snap, &xctx->mousey_snap);
          got_mouse = 1;
        }
        break;
       case 'L':
        merge_line(fd);
        break;
       case 'B':
        merge_box(fd);
        break;
       case 'A':
        merge_arc(fd);
        break;
       case 'P':
        merge_polygon(fd);
        break;
       case 'T':
        merge_text(fd);
        break;
       case 'N':
        merge_wire(fd);
        break;
       case 'C':
        merge_inst(k++,fd);
        break;
       default:
        if( tag[0] == '{' ) ungetc(tag[0], fd);
        read_record(tag[0], fd, 0);
        break;
      }
      read_line(fd, 0); /* discard any remaining characters till (but not including) newline */
     }
     if(!got_mouse) {
       xctx->mx_double_save = xctx->mousex_snap;
       xctx->my_double_save = xctx->mousey_snap;
       xctx->mousex_snap = 0.;
       xctx->mousey_snap = 0.;
     }
     my_free(_ALLOC_ID_, &aux_ptr);
     link_symbols_to_instances(old); /* in case of paste/merge will set instances .sel to SELECTED */
     if(generator) pclose(fd);
     else fclose(fd);

     xctx->ui_state |= STARTMERGE;
     dbg(1, "End merge_file(): loaded file %s: wire=%d inst=%d ui_state=%ld\n",
             name, xctx->wires , xctx->instances, xctx->ui_state);
     move_objects(START,0,0,0);
     xctx->mousex_snap = xctx->mx_double_save;
     xctx->mousey_snap = xctx->my_double_save;
     if(rubber) move_objects(RUBBER,0,0,0);
    } else {
      dbg(0, "merge_file(): can not open %s\n", name);
      xctx->paste_from = 0;
    }
    set_modify(1);
}
