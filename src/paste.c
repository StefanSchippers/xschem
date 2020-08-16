/* File: paste.c
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





void merge_text(FILE *fd)
{
   int i;
   const char *strlayer;
    check_text_storage();
    i=lasttext;
     textelement[i].txt_ptr=NULL;
     load_ascii_string(&textelement[i].txt_ptr,fd);
     fscanf(fd, "%lf %lf %d %d %lf %lf ",
      &textelement[i].x0, &textelement[i].y0, &textelement[i].rot,
      &textelement[i].flip, &textelement[i].xscale,
      &textelement[i].yscale);
     textelement[i].prop_ptr=NULL;
     textelement[i].font=NULL;
     textelement[i].sel=0;
     load_ascii_string(&textelement[i].prop_ptr,fd);
     my_strdup(302, &textelement[i].font, get_tok_value(textelement[i].prop_ptr, "font", 0));/*20171206 */
     strlayer = get_tok_value(textelement[i].prop_ptr, "layer", 0); /*20171206 */
     if(strlayer[0]) textelement[i].layer = atoi(strlayer);
     else textelement[i].layer = -1;

     select_text(i,SELECTED, 1);
     set_modify(1);
     lasttext++;
}

void merge_wire(FILE *fd)
{
    int i;
    double x1,y1,x2,y2;
    char *ptr=NULL;
    i=lastwire;
    fscanf(fd, "%lf %lf %lf %lf",&x1, &y1, &x2, &y2 );
    load_ascii_string( &ptr, fd);
    storeobject(-1, x1,y1,x2,y2,WIRE,0,SELECTED,ptr);
    my_free(870, &ptr);
    select_wire(i, SELECTED, 1);
}

void merge_box(FILE *fd)
{
    int i,c;
    Box *ptr;

    fscanf(fd, "%d",&c);
    if(c>=cadlayers) {
      fprintf(errfp,"Rectangle layer > defined cadlayers, increase cadlayers\n");
      c=cadlayers-1;
    } /* 20150408 */
    check_box_storage(c);
    i=lastrect[c];
    ptr=rect[c];
    fscanf(fd, "%lf %lf %lf %lf ",&ptr[i].x1, &ptr[i].y1, 
       &ptr[i].x2, &ptr[i].y2);
    ptr[i].prop_ptr=NULL;
    RECTORDER(ptr[i].x1, ptr[i].y1, ptr[i].x2, ptr[i].y2); /* 20180108 */
    ptr[i].sel=0;
    load_ascii_string( &ptr[i].prop_ptr, fd);
    select_box(c,i, SELECTED, 1);
    lastrect[c]++;
    set_modify(1);
}

void merge_arc(FILE *fd)
{
    int i,c;
    xArc *ptr;

    fscanf(fd, "%d",&c);
    if(c>=cadlayers) {
      fprintf(errfp,"arc layer > defined cadlayers, increase cadlayers\n");
      c=cadlayers-1;
    } /* 20150408 */
    check_arc_storage(c);
    i=lastarc[c];
    ptr=arc[c];
    fscanf(fd, "%lf %lf %lf %lf %lf ",&ptr[i].x, &ptr[i].y,
           &ptr[i].r, &ptr[i].a, &ptr[i].b);
    ptr[i].prop_ptr=NULL;
    ptr[i].sel=0;
    load_ascii_string(&ptr[i].prop_ptr, fd);
    if( !strcmp(get_tok_value(ptr[i].prop_ptr,"fill",0),"true") ) /* 20181011 */
      ptr[i].fill =1;
    else
      ptr[i].fill =0;
    select_arc(c,i, SELECTED, 1);
    lastarc[c]++;
    set_modify(1);
}


void merge_polygon(FILE *fd)
{
    int i,c, j, points;
    xPolygon *ptr;

    fscanf(fd, "%d %d",&c, &points);
    if(c>=cadlayers) {
      fprintf(errfp,"Rectangle layer > defined cadlayers, increase cadlayers\n");
      c=cadlayers-1;
    } /* 20150408 */
    check_polygon_storage(c);
    i=lastpolygon[c];
    ptr=polygon[c];
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
      fscanf(fd, "%lf %lf ",&(ptr[i].x[j]), &(ptr[i].y[j]));
    }
    load_ascii_string( &ptr[i].prop_ptr, fd);
    if( !strcmp(get_tok_value(ptr[i].prop_ptr,"fill",0),"true") ) /* 20181011 */
      ptr[i].fill =1;
    else
      ptr[i].fill =0;
    select_polygon(c,i, SELECTED, 1);
    lastpolygon[c]++;
    set_modify(1);
}

void merge_line(FILE *fd)
{
    int i,c;
    Line *ptr;

    fscanf(fd, "%d",&c);
    if(c>=cadlayers) {
      fprintf(errfp,"Rectangle layer > defined cadlayers, increase cadlayers\n");
      c=cadlayers-1;
    } /* 20150408 */
    check_line_storage(c);
    i=lastline[c];
    ptr=line[c];
    fscanf(fd, "%lf %lf %lf %lf ",&ptr[i].x1, &ptr[i].y1, 
       &ptr[i].x2, &ptr[i].y2);
    ORDER(ptr[i].x1, ptr[i].y1, ptr[i].x2, ptr[i].y2); /* 20180108 */

    ptr[i].prop_ptr=NULL;
    ptr[i].sel=0;
    load_ascii_string( &ptr[i].prop_ptr, fd);
    select_line(c,i, SELECTED, 1);
    lastline[c]++;
    set_modify(1);
}





void merge_inst(int k,FILE *fd)
{
    int i;
    char *prop_ptr=NULL;

    Instance *ptr;
    i=lastinst;
    check_inst_storage();
    ptr=inst_ptr;
    ptr[i].name=NULL;
    load_ascii_string(&ptr[i].name,fd);
    if(fscanf(fd, "%lf %lf %d %d",&ptr[i].x0, &ptr[i].y0,&ptr[i].rot, &ptr[i].flip) < 4) {
      fprintf(errfp,"WARNING: missing fields for INSTANCE object, ignoring.\n");
      read_line(fd, 0);
      return;
    }
    ptr[i].sel=0;
    ptr[i].flags=0;
    ptr[i].ptr=-1; 
    ptr[i].prop_ptr=NULL;
    ptr[i].instname=NULL; /* 20150411 */
    ptr[i].node=NULL;
    load_ascii_string(&prop_ptr,fd);
    if(!k) hash_all_names(i);
    new_prop_string(i, prop_ptr, k, disable_unique_names);
    /* the final tmp argument is zero for the 1st call and used in */
    /* new_prop_string() for cleaning some internal caches. */
    my_strdup2(306, &inst_ptr[i].instname, get_tok_value(inst_ptr[i].prop_ptr, "name", 0)); /* 20150409 */
    my_free(871, &prop_ptr);
    lastinst++;
    set_modify(1);
    prepared_hash_instances=0;
    prepared_netlist_structs=0;
    prepared_hilight_structs=0;
}





void match_merged_inst(int old)
{
    int i,missing,symbol;
    int cond;
    char *type;
    missing = 0;
    for(i=old;i<lastinst;i++)
    {
     symbol = match_symbol(inst_ptr[i].name);
     if(symbol == -1)
     {
      dbg(1, "merge_inst(): missing symbol, skipping...\n");
      my_free(872, &inst_ptr[i].prop_ptr);  /* 06052001 remove properties */
      my_free(873, &inst_ptr[i].name);      /* 06052001 remove symname   */
      my_free(874, &inst_ptr[i].instname);
      missing++;
      continue;
     }
     inst_ptr[i].ptr = symbol;
     if(missing)
     {

      inst_ptr[i-missing] = inst_ptr[i];
      inst_ptr[i].prop_ptr=NULL;
      /* delete_inst_node(i); */  /* probably not needed */
      inst_ptr[i].ptr=-1;  /*04112003 was 0 */
      inst_ptr[i].flags=0;
      inst_ptr[i].name=NULL;
      inst_ptr[i].instname=NULL;
     }
    }
    lastinst -= missing;
    for(i=old;i<lastinst;i++)
    {
     if(inst_ptr[i].ptr<0) continue;
     select_element(i,SELECTED,1, 0);
     symbol_bbox(i, &inst_ptr[i].x1, &inst_ptr[i].y1,
                       &inst_ptr[i].x2, &inst_ptr[i].y2);
     /* type=get_tok_value(instdef[inst_ptr[i].ptr].prop_ptr,"type",0); */
     type=instdef[inst_ptr[i].ptr].type; /* 20150409 */
     cond= !type || (strcmp(type,"label") && strcmp(type,"ipin") &&
           strcmp(type,"opin") &&  strcmp(type,"iopin"));
     if(cond) inst_ptr[i].flags|=2;
     else inst_ptr[i].flags &=~2;
    }
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
     if(!strcmp(ext,"")) {      /* 20071215 */
       my_snprintf(tmp, S(tmp), "load_file_dialog {Merge file} {.sch.sym} INITIALLOADDIR", ext);
       tcleval(tmp);
       if(!strcmp(tclresult(),"")) return;
       my_strncpy(name, (char *)tclresult(), S(name)); /* 20180925 */
     } 
     else {                     /* 20071215 */
       my_strncpy(name, ext, S(name));
     }
     dbg(1, "merge_file(): sch=%d name=%s\n",currentsch,name);
    }
    else if(selection_load==1)
    {
      my_snprintf(name, S(name), "%s/.selection.sch", user_conf_dir);
    }
    else    /* clipboard load */
    {
      my_snprintf(name, S(name), "%s/.clipboard.sch", user_conf_dir);
    }
    if( (fd=fopen(name,"r"))!= NULL)
    {
     got_mouse = 0;
     push_undo(); /* 20150327 */
     unselect_all();
     old=lastinst;
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
       case 'E': /* 20180912 */
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
          mx_double_save = mousex_snap;
          my_double_save = mousey_snap;
          sscanf( aux_ptr, "%lf %lf", &mousex_snap, &mousey_snap);
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
        read_record(tag[0], fd);
        break;
      }
      read_line(fd, 0); /* discard any remaining characters till (but not including) newline */
     }
     if(!got_mouse) {
       mx_double_save = mousex_snap;
       my_double_save = mousey_snap;
       mousex_snap = 0.;
       mousey_snap = 0.;
     }
     my_free(875, &aux_ptr);
     match_merged_inst(old);
     fclose(fd);
     ui_state |= STARTMERGE;
     dbg(1, "merge_file(): loaded file:wire=%d inst=%d ui_state=%ld\n",
             lastwire , lastinst, ui_state);
     move_objects(BEGIN,0,0,0);
     mousex_snap = mx_double_save;
     mousey_snap = my_double_save;
     move_objects(RUBBER,0,0,0);
    }
}
