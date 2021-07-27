/* File: paste.c
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





void merge_text(FILE *fd)
{
   int i;
   const char *str;
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
     xctx->text[i].sel=0;
     load_ascii_string(&xctx->text[i].prop_ptr,fd);
     my_strdup(302, &xctx->text[i].font, get_tok_value(xctx->text[i].prop_ptr, "font", 0));


     str = get_tok_value(xctx->text[i].prop_ptr, "hcenter", 0);
     xctx->text[i].hcenter = strcmp(str, "true")  ? 0 : 1;
     str = get_tok_value(xctx->text[i].prop_ptr, "vcenter", 0);
     xctx->text[i].vcenter = strcmp(str, "true")  ? 0 : 1;

     str = get_tok_value(xctx->text[i].prop_ptr, "layer", 0);
     if(str[0]) xctx->text[i].layer = atoi(str);
     else xctx->text[i].layer = -1;

     xctx->text[i].flags = 0;
     str = get_tok_value(xctx->text[i].prop_ptr, "slant", 0);
     xctx->text[i].flags |= strcmp(str, "oblique")  ? 0 : TEXT_OBLIQUE;
     xctx->text[i].flags |= strcmp(str, "italic")  ? 0 : TEXT_ITALIC;
     str = get_tok_value(xctx->text[i].prop_ptr, "weight", 0);
     xctx->text[i].flags |= strcmp(str, "bold")  ? 0 : TEXT_BOLD;

     select_text(i,SELECTED, 1);
     set_modify(1);
     xctx->texts++;
}

void merge_wire(FILE *fd)
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
    my_free(870, &ptr);
    select_wire(i, SELECTED, 1);
}

void merge_box(FILE *fd)
{
    int i,c,n;
    xRect *ptr;

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
    RECTORDER(ptr[i].x1, ptr[i].y1, ptr[i].x2, ptr[i].y2);
    ptr[i].sel=0;
    load_ascii_string( &ptr[i].prop_ptr, fd);
    select_box(c,i, SELECTED, 1);
    xctx->rects[c]++;
    set_modify(1);
}

void merge_arc(FILE *fd)
{
    int i,c,n;
    xArc *ptr;

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
    if( !strcmp(get_tok_value(ptr[i].prop_ptr,"fill",0),"true") )
      ptr[i].fill =1;
    else
      ptr[i].fill =0;
    select_arc(c,i, SELECTED, 1);
    xctx->arcs[c]++;
    set_modify(1);
}


void merge_polygon(FILE *fd)
{
    int i,c, j, points;
    xPoly *ptr;

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
    ptr[i].x = my_calloc(303, points, sizeof(double));
    ptr[i].y = my_calloc(304, points, sizeof(double));
    ptr[i].selected_point= my_calloc(305, points, sizeof(unsigned short));
    ptr[i].points=points;
    ptr[i].sel=0;
    for(j=0;j<points;j++) {
      if(fscanf(fd, "%lf %lf ",&(ptr[i].x[j]), &(ptr[i].y[j]))<2) {
        fprintf(errfp,"merge_polygon(): WARNING: missing fields for POLYGON points, ignoring.\n");
        my_free(827, &ptr[i].x);
        my_free(1218, &ptr[i].y);
        my_free(1223, &ptr[i].selected_point);
        read_line(fd, 0);
        return;
      }
    }
    load_ascii_string( &ptr[i].prop_ptr, fd);
    if( !strcmp(get_tok_value(ptr[i].prop_ptr,"fill",0),"true") )
      ptr[i].fill =1;
    else
      ptr[i].fill =0;
    select_polygon(c,i, SELECTED, 1);
    xctx->polygons[c]++;
    set_modify(1);
}

void merge_line(FILE *fd)
{
    int i,c,n;
    xLine *ptr;

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
    select_line(c,i, SELECTED, 1);
    xctx->lines[c]++;
    set_modify(1);
}





void merge_inst(int k,FILE *fd)
{
    int i;
    char *prop_ptr=NULL;
    char *tmp = NULL;
    xInstance *ptr;
    i=xctx->instances;
    check_inst_storage();
    ptr=xctx->inst;
    ptr[i].name=NULL;
    load_ascii_string(&tmp, fd);
    /* avoid as much as possible calls to rel_sym_path (slow) */
    #ifdef __unix__
    if(tmp[0] == '/') my_strdup(763, &ptr[i].name, rel_sym_path(tmp));
    else my_strdup(755, &ptr[i].name,tmp);
    #else
    my_strdup(780, &ptr[i].name, rel_sym_path(tmp));
    #endif
    my_free(756, &tmp);
    if(fscanf(fd, "%lf %lf %hd %hd",&ptr[i].x0, &ptr[i].y0,&ptr[i].rot, &ptr[i].flip) < 4) {
      fprintf(errfp,"WARNING: missing fields for INSTANCE object, ignoring.\n");
      read_line(fd, 0);
      return;
    }
    ptr[i].sel=0;
    ptr[i].flags=0;
    ptr[i].color=-10000;
    ptr[i].ptr=-1;
    ptr[i].prop_ptr=NULL;
    ptr[i].instname=NULL;
    ptr[i].lab=NULL;  /* assigned in link_symbols_to_instances */
    ptr[i].node=NULL;
    load_ascii_string(&prop_ptr,fd);
    if(!k) hash_all_names(i);
    new_prop_string(i, prop_ptr, k, dis_uniq_names);
    /* the final tmp argument is zero for the 1st call and used in */
    /* new_prop_string() for cleaning some internal caches. */
    my_strdup2(306, &xctx->inst[i].instname, get_tok_value(xctx->inst[i].prop_ptr, "name", 0));
    if(!strcmp(get_tok_value(xctx->inst[i].prop_ptr,"highlight",0), "true")) xctx->inst[i].flags |= 4;

    my_free(871, &prop_ptr);
    xctx->instances++;
    set_modify(1);
}

/* merge selection if selection_load=1, otherwise ask for filename */
/* selection_load: */
/*                      0: ask filename to merge */
/*                         if ext=="" else use ext as name  ... 20071215 */
/*                      1: merge selection */
/*                      2: merge clipboard */
void merge_file(int selection_load, const char ext[])
{
    FILE *fd;
    int k=0, old;
    int endfile=0;
    char name[PATH_MAX];
    char tag[1]; /* overflow safe */
    char tmp[256]; /* 20161122 overflow safe */
    char *aux_ptr=NULL;
    int got_mouse;


    if(selection_load==0)
    {
     if(!strcmp(ext,"")) {
       my_snprintf(tmp, S(tmp), "load_file_dialog {Merge file} {.sch.sym} INITIALLOADDIR", ext);
       tcleval(tmp);
       if(!strcmp(tclresult(),"")) return;
       my_strncpy(name, (char *)tclresult(), S(name));
     }
     else {
       my_strncpy(name, ext, S(name));
     }
     dbg(1, "merge_file(): sch=%d name=%s\n",xctx->currsch,name);
    }
    else if(selection_load==1)
    {
      my_snprintf(name, S(name), "%s/.selection.sch", user_conf_dir);
    }
    else    /* clipboard load */
    {
      my_snprintf(name, S(name), "%s/.clipboard.sch", user_conf_dir);
    }
    if( (fd=fopen(name,"r"))!= NULL) {
     xctx->prep_hi_structs=0;
     xctx->prep_net_structs=0;
     xctx->prep_hash_inst=0;
     xctx->prep_hash_wires=0;
     got_mouse = 0;
     push_undo();
     unselect_all();
     old=xctx->instances;
     while(!endfile)
     {
      if(fscanf(fd," %c",tag)==EOF) break;
      switch(tag[0])
      {
       case 'v':
        load_ascii_string(&aux_ptr, fd);
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
     my_free(875, &aux_ptr);
     link_symbols_to_instances(old);
     fclose(fd);
     xctx->ui_state |= STARTMERGE;
     dbg(1, "merge_file(): loaded file:wire=%d inst=%d ui_state=%ld\n",
             xctx->wires , xctx->instances, xctx->ui_state);
     move_objects(START,0,0,0);
     xctx->mousex_snap = xctx->mx_double_save;
     xctx->mousey_snap = xctx->my_double_save;
     move_objects(RUBBER,0,0,0);
    } else {
      dbg(0, "merge_file(): can not open %s\n", name);
    }
}
