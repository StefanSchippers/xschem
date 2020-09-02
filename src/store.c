/* File: store.c
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


void check_wire_storage(void)
{
 if(lastwire >= max_wires)
 {
  max_wires=(1+lastwire / CADMAXWIRES)*CADMAXWIRES;
  my_realloc(392, &wire, sizeof(Wire)*max_wires);
 }
}

void check_selected_storage(void)
{
 if(lastselected >= max_selected)
 {
  max_selected=(1+lastselected / MAXGROUP) * MAXGROUP;
  my_realloc(393, &selectedgroup, sizeof(Selected)*max_selected);
 }
}

void check_text_storage(void)
{
 if(lasttext >= max_texts)
 {
  max_texts=(1 + lasttext / CADMAXTEXT) * CADMAXTEXT;
  my_realloc(394, &textelement, sizeof(Text)*max_texts);
 }
}

void check_symbol_storage(void)
{
 int i;
 if(lastinstdef >= max_symbols)
 {
  dbg(1, "check_symbol_storage(): more than max_symbols, %s\n",
        schematic[currentsch] );
  max_symbols=(1 + lastinstdef / ELEMDEF) * ELEMDEF;
  my_realloc(395, &instdef, sizeof(Instdef)*max_symbols);
  for(i=lastinstdef;i<max_symbols;i++) {
    instdef[i].polygonptr=my_calloc(68, cadlayers, sizeof(xPolygon *));
    if(instdef[i].polygonptr==NULL){
       fprintf(errfp, "check_symbol_storage(): calloc error\n");tcleval( "exit");
    }
  
    instdef[i].arcptr=my_calloc(396, cadlayers, sizeof(xArc *));
    if(instdef[i].arcptr==NULL){
       fprintf(errfp, "check_symbol_storage(): calloc error\n");tcleval( "exit");
    }
  
    instdef[i].lineptr=my_calloc(397, cadlayers, sizeof(Line *));
    if(instdef[i].lineptr==NULL){
       fprintf(errfp, "check_symbol_storage(): calloc error\n");tcleval( "exit");
    }
  
    instdef[i].boxptr=my_calloc(398, cadlayers, sizeof(Box *));
    if(instdef[i].boxptr==NULL){
      fprintf(errfp, "check_symbol_storage(): calloc error\n");tcleval( "exit");
    }
   
    instdef[i].lines=my_calloc(399, cadlayers, sizeof(int));
    if(instdef[i].lines==NULL){
      fprintf(errfp, "check_symbol_storage(): calloc error\n");tcleval( "exit");
    }
   
    instdef[i].rects=my_calloc(400, cadlayers, sizeof(int));
    if(instdef[i].rects==NULL){
      fprintf(errfp, "check_symbol_storage(): calloc error\n");tcleval( "exit");
    }
    instdef[i].polygons=my_calloc(401, cadlayers, sizeof(int)); /* 20171115 */
    if(instdef[i].polygons==NULL){
      fprintf(errfp, "check_symbol_storage(): calloc error\n");tcleval( "exit");
    }
    instdef[i].arcs=my_calloc(66, cadlayers, sizeof(int)); /* 20171115 */
    if(instdef[i].arcs==NULL){
      fprintf(errfp, "check_symbol_storage(): calloc error\n");tcleval( "exit");
    }
  }
 }

}

void check_inst_storage(void)
{
 if(lastinst >= max_instances)
 {
  max_instances=(1 + lastinst / ELEMINST) * ELEMINST;
  my_realloc(402, &inst_ptr, sizeof(Instance)*max_instances);
 }
}

void check_arc_storage(int c)
{
 if(lastarc[c] >= max_arcs[c])
 {
  max_arcs[c]=(1 + lastarc[c] / CADMAXOBJECTS) * CADMAXOBJECTS;
  my_realloc(403, &arc[c], sizeof(xArc)*max_arcs[c]);
 }
}

void check_box_storage(int c)
{
 if(lastrect[c] >= max_rects[c])
 {
  max_rects[c]=(1 + lastrect[c] / CADMAXOBJECTS) * CADMAXOBJECTS;
  my_realloc(404, &rect[c], sizeof(Box)*max_rects[c]);
 }
}

void check_line_storage(int c)
{
 if(lastline[c] >= max_lines[c])
 {
  max_lines[c]=(1 + lastline[c] / CADMAXOBJECTS) * CADMAXOBJECTS;
  my_realloc(405, &line[c], sizeof(Line)*max_lines[c]);
 }
}

void check_polygon_storage(int c) /*20171115 */
{
 if(lastpolygon[c] >= max_polygons[c])
 {
  max_polygons[c]=(1 + lastpolygon[c] / CADMAXOBJECTS) * CADMAXOBJECTS;
  my_realloc(406, &polygon[c], sizeof(xPolygon)*max_polygons[c]);
 }
}

void store_arc(int pos, double x, double y, double r, double a, double b, 
               unsigned int rectcolor, unsigned short sel, char *prop_ptr)
{
  int n, j;
  const char *dash;
  check_arc_storage(rectcolor);
  if(pos==-1) n=lastarc[rectcolor];
  else
  {
   for(j=lastarc[rectcolor];j>pos;j--)
   {
    arc[rectcolor][j]=arc[rectcolor][j-1];
   }
   n=pos;
  }
  arc[rectcolor][n].x = x;
  arc[rectcolor][n].y = y;
  arc[rectcolor][n].r = r;
  arc[rectcolor][n].a = a;
  arc[rectcolor][n].b = b;
  arc[rectcolor][n].prop_ptr = NULL;
  my_strdup(407, &arc[rectcolor][n].prop_ptr, prop_ptr);
  arc[rectcolor][n].sel = sel;
  if( !strcmp(get_tok_value(arc[rectcolor][n].prop_ptr,"fill",0),"true") )
    arc[rectcolor][n].fill =1;
  else
    arc[rectcolor][n].fill =0;
  dash = get_tok_value(arc[rectcolor][n].prop_ptr,"dash",0);
  if( strcmp(dash, "") )
    arc[rectcolor][n].dash = atoi(dash);
  else
    arc[rectcolor][n].dash = 0;

  lastarc[rectcolor]++;
  set_modify(1);
}

void store_polygon(int pos, double *x, double *y, int points, unsigned int rectcolor, unsigned short sel, char *prop_ptr)
{
  int n, j;
  const char *dash;
  check_polygon_storage(rectcolor);
  if(pos==-1) n=lastpolygon[rectcolor];
  else
  {
   for(j=lastpolygon[rectcolor];j>pos;j--)
   {
    polygon[rectcolor][j]=polygon[rectcolor][j-1];
   }
   n=pos;
  }
  dbg(2, "store_polygon(): storing POLYGON %d\n",n);
  
  polygon[rectcolor][n].x=NULL;
  polygon[rectcolor][n].y=NULL;
  polygon[rectcolor][n].selected_point=NULL;
  polygon[rectcolor][n].prop_ptr=NULL;
  polygon[rectcolor][n].x= my_calloc(408, points, sizeof(double));
  polygon[rectcolor][n].y= my_calloc(409, points, sizeof(double));
  polygon[rectcolor][n].selected_point= my_calloc(410, points, sizeof(unsigned short));
  my_strdup(411, &polygon[rectcolor][n].prop_ptr, prop_ptr);
  for(j=0;j<points; j++) {
    polygon[rectcolor][n].x[j] = x[j];
    polygon[rectcolor][n].y[j] = y[j];
  }
  polygon[rectcolor][n].points = points;
  polygon[rectcolor][n].sel = sel;

  /* 20181002 */
  if( !strcmp(get_tok_value(polygon[rectcolor][n].prop_ptr,"fill",0),"true") )
    polygon[rectcolor][n].fill =1;
  else
    polygon[rectcolor][n].fill =0;
  dash = get_tok_value(polygon[rectcolor][n].prop_ptr,"dash",0);
  if( strcmp(dash, "") )
    polygon[rectcolor][n].dash = atoi(dash);
  else
    polygon[rectcolor][n].dash = 0;


  lastpolygon[rectcolor]++;
  set_modify(1);
}

void storeobject(int pos, double x1,double y1,double x2,double y2,
                 unsigned short type, unsigned int rectcolor,
                 unsigned short sel, const char *prop_ptr)
{
 int n, j;
 const char * dash;
    if(type == LINE)
    {
     check_line_storage(rectcolor);

     if(pos==-1) n=lastline[rectcolor];
     else
     {
      for(j=lastline[rectcolor];j>pos;j--)
      {
       line[rectcolor][j]=line[rectcolor][j-1];
      }
      n=pos;
     }
     dbg(2, "storeobject(): storing LINE %d\n",n);
     line[rectcolor][n].x1=x1;
     line[rectcolor][n].x2=x2;
     line[rectcolor][n].y1=y1;
     line[rectcolor][n].y2=y2;
     line[rectcolor][n].prop_ptr=NULL;
     my_strdup(412, &line[rectcolor][n].prop_ptr, prop_ptr);
     line[rectcolor][n].sel=sel;
     dash = get_tok_value(line[rectcolor][n].prop_ptr,"dash",0);
     if( strcmp(dash, "") )
       line[rectcolor][n].dash = atoi(dash);
     else
       line[rectcolor][n].dash = 0;
     lastline[rectcolor]++;
     set_modify(1);
    }
    if(type == xRECT)
    {
     check_box_storage(rectcolor);
     if(pos==-1) n=lastrect[rectcolor];
     else
     {
      for(j=lastrect[rectcolor];j>pos;j--)
      {
       rect[rectcolor][j]=rect[rectcolor][j-1];
      }
      n=pos;
     }
     dbg(2, "storeobject(): storing RECT %d\n",n);
     rect[rectcolor][n].x1=x1;
     rect[rectcolor][n].x2=x2;
     rect[rectcolor][n].y1=y1;
     rect[rectcolor][n].y2=y2;
     rect[rectcolor][n].prop_ptr=NULL;
     my_strdup(413, &rect[rectcolor][n].prop_ptr, prop_ptr);
     rect[rectcolor][n].sel=sel;
     dash = get_tok_value(rect[rectcolor][n].prop_ptr,"dash",0);
     if( strcmp(dash, "") )
       rect[rectcolor][n].dash = atoi(dash);
     else
       rect[rectcolor][n].dash = 0;
     lastrect[rectcolor]++;
     set_modify(1);
    }
    if(type == WIRE)
    {
     check_wire_storage();
     if(pos==-1) n=lastwire;
     else
     {
      for(j=lastwire;j>pos;j--)
      {
       wire[j]=wire[j-1];
      }
      n=pos;
     }
     dbg(2, "storeobject(): storing WIRE %d\n",n);
     wire[n].x1=x1;
     wire[n].y1=y1;
     wire[n].x2=x2;
     wire[n].y2=y2;
     wire[n].prop_ptr=NULL;
     wire[n].node=NULL;
     wire[n].end1=0;
     wire[n].end2=0;
     my_strdup(414, &wire[n].prop_ptr, prop_ptr);
     if(!strcmp(get_tok_value(wire[n].prop_ptr,"bus",0), "true")) wire[n].bus=1; /* 20171201 */
     else wire[n].bus=0;

     wire[n].sel=sel;
     lastwire++;
     set_modify(1);
     prepared_hash_wires=0;
     prepared_netlist_structs=0;
     prepared_hilight_structs=0;
    }
}

void freenet_nocheck(int i)
{
 int j;
  my_free(959, &wire[i].prop_ptr);
  my_free(960, &wire[i].node);
  for(j=i+1;j<lastwire;j++)
  {
    wire[j-1] = wire[j];
    wire[j].prop_ptr=NULL;
    wire[j].node=NULL;
  } /*end for j */
  lastwire--;
}

