/* File: in_memory_undo.c
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

#ifdef IN_MEMORY_UNDO
typedef struct
{
  char     *gptr;
  char     *vptr;
  char     *sptr;
  char     *kptr;
  char     *eptr;
  int *lines;
  int *boxes;
  int *polygons;
  int *arcs;
  int wires;
  int texts;
  int instances;
  Line     **lptr;
  Box      **bptr;
  xPolygon  **pptr;
  xArc      **aptr;
  Wire     *wptr;
  Text     *tptr;
  Instance *iptr;
} Undo_slot;

static Undo_slot uslot[MAX_UNDO];
static int initialized=0;

void init_undo()
{
  int slot;
  
  for(slot=0;slot<MAX_UNDO; slot++) {
    uslot[slot].lines=my_calloc(165, cadlayers, sizeof(int));
    uslot[slot].boxes=my_calloc(166, cadlayers, sizeof(int));
    uslot[slot].arcs=my_calloc(167, cadlayers, sizeof(int));
    uslot[slot].polygons=my_calloc(168, cadlayers, sizeof(int));
    uslot[slot].lptr=my_calloc(169, cadlayers, sizeof(Line *));
    uslot[slot].bptr=my_calloc(170, cadlayers, sizeof(Box *));
    uslot[slot].aptr=my_calloc(171, cadlayers, sizeof(xArc *));
    uslot[slot].pptr=my_calloc(172, cadlayers, sizeof(xPolygon *));
  }
}

void free_lines(int slot)
{
  int i, c;
  
  for(c=0;c<cadlayers; c++) {
    for(i=0;i<uslot[slot].lines[c]; i++) {
      my_free(783, &uslot[slot].lptr[c][i].prop_ptr);
    }
    my_free(784, &uslot[slot].lptr[c]);
    uslot[slot].lines[c] = 0;
  }
}

void free_boxes(int slot)
{
  int i, c;

  for(c=0;c<cadlayers; c++) {
    for(i=0;i<uslot[slot].boxes[c]; i++) {
      my_free(785, &uslot[slot].bptr[c][i].prop_ptr);
    }
    my_free(786, &uslot[slot].bptr[c]);
    uslot[slot].boxes[c] = 0;
  }
}

void free_polygons(int slot)
{
  int i, c;

  for(c=0;c<cadlayers; c++) {
    for(i=0;i<uslot[slot].polygons[c]; i++) {
      my_free(787, &uslot[slot].pptr[c][i].prop_ptr);
      my_free(788, &uslot[slot].pptr[c][i].x);
      my_free(789, &uslot[slot].pptr[c][i].y);
      my_free(790, &uslot[slot].pptr[c][i].selected_point);
    }
    my_free(791, &uslot[slot].pptr[c]);
    uslot[slot].polygons[c] = 0;
  }
}

void free_arcs(int slot)
{
  int i, c;

  for(c=0;c<cadlayers; c++) {
    for(i=0;i<uslot[slot].arcs[c]; i++) {
      my_free(792, &uslot[slot].aptr[c][i].prop_ptr);
    }
    my_free(793, &uslot[slot].aptr[c]);
    uslot[slot].arcs[c] = 0;
  }
}

void free_wires(int slot)
{
  int i;

  for(i=0;i<uslot[slot].wires; i++) {
    my_free(794, &uslot[slot].wptr[i].prop_ptr);
  }
  my_free(795, &uslot[slot].wptr);
  uslot[slot].wires = 0;
}

void free_texts(int slot)
{ 
  int i;
    
  for(i=0;i<uslot[slot].texts; i++) {
    my_free(796, &uslot[slot].tptr[i].prop_ptr);
    my_free(797, &uslot[slot].tptr[i].txt_ptr);
    my_free(798, &uslot[slot].tptr[i].font);
  }
  my_free(799, &uslot[slot].tptr);
  uslot[slot].texts = 0;
}

void free_instances(int slot)
{
  int i;

  for(i=0;i<uslot[slot].instances; i++) {
    my_free(800, &uslot[slot].iptr[i].name);
    my_free(801, &uslot[slot].iptr[i].prop_ptr);
    my_free(802, &uslot[slot].iptr[i].instname);
  }
  my_free(803, &uslot[slot].iptr);
  uslot[slot].instances = 0;
}

void clear_undo(void) /* 20150327 */
{
  int slot;
  cur_undo_ptr = 0;
  tail_undo_ptr = 0;
  head_undo_ptr = 0;
  if(!initialized) return;
  for(slot=0; slot<MAX_UNDO; slot++) {
    free_lines(slot);
    free_boxes(slot);
    free_polygons(slot);
    free_arcs(slot);
    free_wires(slot);
    free_texts(slot);
    free_instances(slot);
  }
}

void delete_undo(void)  /* 20150327 */
{
  int slot;

  clear_undo();
  for(slot=0;slot<MAX_UNDO; slot++) {
    my_free(804, &uslot[slot].lines);
    my_free(805, &uslot[slot].boxes);
    my_free(806, &uslot[slot].arcs);
    my_free(807, &uslot[slot].polygons);
    my_free(808, &uslot[slot].lptr);
    my_free(809, &uslot[slot].bptr);
    my_free(810, &uslot[slot].aptr);
    my_free(811, &uslot[slot].pptr);
  }

}

void push_undo(void)
{
  int slot, i, c;

  if(no_undo)return;
  if(!initialized) {
    initialized=1;
    init_undo();
  }
  slot = cur_undo_ptr%max_undo;

  my_strdup(173, &uslot[slot].gptr, schvhdlprop);
  my_strdup(174, &uslot[slot].vptr, schverilogprop);
  my_strdup(175, &uslot[slot].sptr, schprop);
  my_strdup(359, &uslot[slot].kptr, schsymbolprop);
  my_strdup(176, &uslot[slot].eptr, schtedaxprop);

  free_lines(slot);
  free_boxes(slot);
  free_polygons(slot);
  free_arcs(slot);
  free_wires(slot);
  free_texts(slot);
  free_instances(slot);

  for(c=0;c<cadlayers;c++) {
    uslot[slot].lines[c] = lastline[c];
    uslot[slot].boxes[c] = lastrect[c];
    uslot[slot].arcs[c] = lastarc[c];
    uslot[slot].polygons[c] = lastpolygon[c];
    uslot[slot].lptr[c] = my_calloc(177, lastline[c], sizeof(Line));
    uslot[slot].bptr[c] = my_calloc(178, lastrect[c], sizeof(Box));
    uslot[slot].pptr[c] = my_calloc(179, lastpolygon[c], sizeof(xPolygon));
    uslot[slot].aptr[c] = my_calloc(180, lastarc[c], sizeof(xArc));
  }
  uslot[slot].wptr = my_calloc(181, lastwire, sizeof(Wire));
  uslot[slot].tptr = my_calloc(182, lasttext, sizeof(Text));
  uslot[slot].iptr = my_calloc(183, lastinst, sizeof(Instance));
  uslot[slot].texts = lasttext;
  uslot[slot].instances = lastinst;
  uslot[slot].wires = lastwire;

  for(c=0;c<cadlayers;c++) {
    /* lines */
    for(i=0;i<lastline[c];i++) {
      uslot[slot].lptr[c][i] = line[c][i];
      uslot[slot].lptr[c][i].prop_ptr = NULL;
      my_strdup(184, &uslot[slot].lptr[c][i].prop_ptr, line[c][i].prop_ptr);
    }
    /* boxes */
    for(i=0;i<lastrect[c];i++) {
      uslot[slot].bptr[c][i] = rect[c][i];
      uslot[slot].bptr[c][i].prop_ptr = NULL;
      my_strdup(185, &uslot[slot].bptr[c][i].prop_ptr, rect[c][i].prop_ptr);
    }
    /* arcs */
    for(i=0;i<lastarc[c];i++) {
      uslot[slot].aptr[c][i] = arc[c][i];
      uslot[slot].aptr[c][i].prop_ptr = NULL;
      my_strdup(186, &uslot[slot].aptr[c][i].prop_ptr, arc[c][i].prop_ptr);
    }
    /*polygons */
    for(i=0;i<lastpolygon[c];i++) {
      int points = polygon[c][i].points;
      uslot[slot].pptr[c][i] = polygon[c][i];
      uslot[slot].pptr[c][i].prop_ptr = NULL;
      uslot[slot].pptr[c][i].x = NULL;
      uslot[slot].pptr[c][i].y = NULL;
      uslot[slot].pptr[c][i].selected_point = NULL;
      uslot[slot].pptr[c][i].x = my_malloc(187, points * sizeof(double));
      uslot[slot].pptr[c][i].y = my_malloc(188, points * sizeof(double));
      uslot[slot].pptr[c][i].selected_point = my_malloc(189, points * sizeof(unsigned short));
      memcpy(uslot[slot].pptr[c][i].x, polygon[c][i].x, points * sizeof(double));
      memcpy(uslot[slot].pptr[c][i].y, polygon[c][i].y, points * sizeof(double));
      memcpy(uslot[slot].pptr[c][i].selected_point, polygon[c][i].selected_point, points * sizeof(unsigned short));
      my_strdup(190, &uslot[slot].pptr[c][i].prop_ptr, polygon[c][i].prop_ptr);
    }
  }
  /* instances */
  for(i=0;i<lastinst;i++) {
    uslot[slot].iptr[i] = inst_ptr[i];
    uslot[slot].iptr[i].prop_ptr = NULL;
    uslot[slot].iptr[i].name = NULL;
    uslot[slot].iptr[i].instname = NULL;
    uslot[slot].iptr[i].node = NULL;
    my_strdup2(191, &uslot[slot].iptr[i].instname, inst_ptr[i].instname);
    my_strdup(192, &uslot[slot].iptr[i].prop_ptr, inst_ptr[i].prop_ptr);
    my_strdup(193, &uslot[slot].iptr[i].name, inst_ptr[i].name);
  }
  /* texts */
  for(i=0;i<lasttext;i++) {
    uslot[slot].tptr[i] = textelement[i];
    uslot[slot].tptr[i].prop_ptr = NULL;
    uslot[slot].tptr[i].txt_ptr = NULL;
    uslot[slot].tptr[i].font = NULL;
    my_strdup(194, &uslot[slot].tptr[i].prop_ptr, textelement[i].prop_ptr);
    my_strdup(195, &uslot[slot].tptr[i].txt_ptr, textelement[i].txt_ptr);
    my_strdup(196, &uslot[slot].tptr[i].font, textelement[i].font);
  }

  /* wires */
  for(i=0;i<lastwire;i++) {
    uslot[slot].wptr[i] = wire[i];
    uslot[slot].wptr[i].prop_ptr = NULL;
    uslot[slot].wptr[i].node = NULL;
    my_strdup(197, &uslot[slot].wptr[i].prop_ptr, wire[i].prop_ptr);
  }


  cur_undo_ptr++;
  head_undo_ptr = cur_undo_ptr;
  tail_undo_ptr = head_undo_ptr <= max_undo? 0: head_undo_ptr-max_undo;
}


void pop_undo(int redo)
{ 
  int slot, i, c;

  if(no_undo)return;
  if(redo) {
    if(cur_undo_ptr < head_undo_ptr) {
      cur_undo_ptr++;
    } else {
      return;
    }
  } else {  /*redo=0 (undo) */
    if(cur_undo_ptr == tail_undo_ptr) return;
    if(head_undo_ptr == cur_undo_ptr) {
      push_undo();
      head_undo_ptr--;
      cur_undo_ptr--;
    }
    if(cur_undo_ptr<=0) return; /* check undo tail */
    cur_undo_ptr--;
  }
  slot = cur_undo_ptr%max_undo;
  clear_drawing();
  unselect_all();
  my_strdup(198, &schvhdlprop, uslot[slot].gptr);
  my_strdup(199, &schverilogprop, uslot[slot].vptr);
  my_strdup(200, &schprop, uslot[slot].sptr);
  my_strdup(389, &schsymbolprop, uslot[slot].kptr);
  my_strdup(201, &schtedaxprop, uslot[slot].eptr);
  for(c=0;c<cadlayers;c++) {
    /* lines */
    max_lines[c] = lastline[c] = uslot[slot].lines[c];
    line[c] = my_calloc(202, lastline[c], sizeof(Line));
    for(i=0;i<lastline[c];i++) {
      line[c][i] = uslot[slot].lptr[c][i];
      line[c][i].prop_ptr=NULL;
      my_strdup(203, &line[c][i].prop_ptr, uslot[slot].lptr[c][i].prop_ptr);
    }
    /* boxes */
    max_rects[c] = lastrect[c] = uslot[slot].boxes[c];
    rect[c] = my_calloc(204, lastrect[c], sizeof(Box));
    for(i=0;i<lastrect[c];i++) {
      rect[c][i] = uslot[slot].bptr[c][i];
      rect[c][i].prop_ptr=NULL;
      my_strdup(205, &rect[c][i].prop_ptr, uslot[slot].bptr[c][i].prop_ptr);
    }
    /* arcs */
    max_arcs[c] = lastarc[c] = uslot[slot].arcs[c];
    arc[c] = my_calloc(206, lastarc[c], sizeof(xArc));
    for(i=0;i<lastarc[c];i++) {
      arc[c][i] = uslot[slot].aptr[c][i];
      arc[c][i].prop_ptr=NULL;
      my_strdup(207, &arc[c][i].prop_ptr, uslot[slot].aptr[c][i].prop_ptr);
    }
    /* polygons */
    max_polygons[c] = lastpolygon[c] = uslot[slot].polygons[c];
    polygon[c] = my_calloc(208, lastpolygon[c], sizeof(xPolygon));
    for(i=0;i<lastpolygon[c];i++) {
      int points = uslot[slot].pptr[c][i].points;
      polygon[c][i] = uslot[slot].pptr[c][i];
      polygon[c][i].prop_ptr=NULL;
      polygon[c][i].x=NULL;
      polygon[c][i].y=NULL;
      polygon[c][i].selected_point=NULL;
      my_strdup(209, &polygon[c][i].prop_ptr, uslot[slot].pptr[c][i].prop_ptr);
      my_realloc(210, &polygon[c][i].x, points * sizeof(double));
      my_realloc(211, &polygon[c][i].y, points * sizeof(double));
      my_realloc(212, &polygon[c][i].selected_point, points * sizeof(unsigned short));
      memcpy(polygon[c][i].x, uslot[slot].pptr[c][i].x, points * sizeof(double));
      memcpy(polygon[c][i].y, uslot[slot].pptr[c][i].y, points * sizeof(double));
      memcpy(polygon[c][i].selected_point, uslot[slot].pptr[c][i].selected_point, points * sizeof(unsigned short));
    }

  }

  /* instances */
  max_instances = lastinst = uslot[slot].instances;
  inst_ptr = my_calloc(213, lastinst, sizeof(Instance));
  for(i=0;i<lastinst;i++) {
    inst_ptr[i] = uslot[slot].iptr[i];
    inst_ptr[i].prop_ptr=NULL;
    inst_ptr[i].name=NULL;
    inst_ptr[i].instname=NULL;
    my_strdup(214, &inst_ptr[i].prop_ptr, uslot[slot].iptr[i].prop_ptr);
    my_strdup(215, &inst_ptr[i].name, uslot[slot].iptr[i].name);
    my_strdup2(216, &inst_ptr[i].instname, uslot[slot].iptr[i].instname);
  }

  /* texts */
  max_texts = lasttext = uslot[slot].texts;
  textelement = my_calloc(217, lasttext, sizeof(Text));
  for(i=0;i<lasttext;i++) {
    textelement[i] = uslot[slot].tptr[i];
    textelement[i].txt_ptr=NULL;
    textelement[i].font=NULL;
    textelement[i].prop_ptr=NULL;
    my_strdup(218, &textelement[i].prop_ptr, uslot[slot].tptr[i].prop_ptr);
    my_strdup(219, &textelement[i].txt_ptr, uslot[slot].tptr[i].txt_ptr);
    my_strdup(220, &textelement[i].font, uslot[slot].tptr[i].font);
  }

  /* wires */
  max_wires = lastwire = uslot[slot].wires;
  wire = my_calloc(221, lastwire, sizeof(Wire));
  for(i=0;i<lastwire;i++) {
    wire[i] = uslot[slot].wptr[i];
    wire[i].prop_ptr=NULL;
    wire[i].node=NULL;
    my_strdup(222, &wire[i].prop_ptr, uslot[slot].wptr[i].prop_ptr);
  }

  link_symbols_to_instances();
  set_modify(1);
  prepared_hash_instances=0; /* 20171224 */
  prepared_hash_wires=0; /* 20171224 */
  prepared_netlist_structs=0; /* 20171224 */
  prepared_hilight_structs=0; /* 20171224 */
  update_conn_cues(0, 0);
}

#endif
