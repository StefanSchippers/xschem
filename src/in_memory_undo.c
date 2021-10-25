/* File: in_memory_undo.c
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

#ifdef IN_MEMORY_UNDO
typedef struct
{
  char     *gptr;
  char     *vptr;
  char     *sptr;
  char     *kptr;
  char     *eptr;
  int *lines;
  int *rects;
  int *polygons;
  int *arcs;
  int wires;
  int texts;
  int instances;
  xLine     **lptr;
  xRect      **bptr;
  xPoly  **pptr;
  xArc      **aptr;
  xWire     *wptr;
  xText     *tptr;
  xInstance *iptr;
} Undo_slot;

static Undo_slot uslot[MAX_UNDO];
static int initialized=0;

void init_undo()
{
  int slot;

  for(slot=0;slot<MAX_UNDO; slot++) {
    uslot[slot].lines=my_calloc(165, cadlayers, sizeof(int));
    uslot[slot].rects=my_calloc(166, cadlayers, sizeof(int));
    uslot[slot].arcs=my_calloc(167, cadlayers, sizeof(int));
    uslot[slot].polygons=my_calloc(168, cadlayers, sizeof(int));
    uslot[slot].lptr=my_calloc(169, cadlayers, sizeof(xLine *));
    uslot[slot].bptr=my_calloc(170, cadlayers, sizeof(xRect *));
    uslot[slot].aptr=my_calloc(171, cadlayers, sizeof(xArc *));
    uslot[slot].pptr=my_calloc(172, cadlayers, sizeof(xPoly *));
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

void free_rects(int slot)
{
  int i, c;

  for(c=0;c<cadlayers; c++) {
    for(i=0;i<uslot[slot].rects[c]; i++) {
      my_free(785, &uslot[slot].bptr[c][i].prop_ptr);
    }
    my_free(786, &uslot[slot].bptr[c]);
    uslot[slot].rects[c] = 0;
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
    my_free(104, &uslot[slot].iptr[i].lab);
  }
  my_free(803, &uslot[slot].iptr);
  uslot[slot].instances = 0;
}

void clear_undo(void)
{
  int slot;
  xctx->cur_undo_ptr = 0;
  xctx->tail_undo_ptr = 0;
  xctx->head_undo_ptr = 0;
  if(!initialized) return;
  for(slot=0; slot<MAX_UNDO; slot++) {
    free_lines(slot);
    free_rects(slot);
    free_polygons(slot);
    free_arcs(slot);
    free_wires(slot);
    free_texts(slot);
    free_instances(slot);
  }
}

void delete_undo(void)
{
  int slot;

  clear_undo();
  for(slot=0;slot<MAX_UNDO; slot++) {
    my_free(804, &uslot[slot].lines);
    my_free(805, &uslot[slot].rects);
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

  if(xctx->no_undo)return;
  if(!initialized) {
    initialized=1;
    init_undo();
  }
  slot = xctx->cur_undo_ptr%MAX_UNDO;

  my_strdup(173, &uslot[slot].gptr, xctx->schvhdlprop);
  my_strdup(174, &uslot[slot].vptr, xctx->schverilogprop);
  my_strdup(175, &uslot[slot].sptr, xctx->schprop);
  my_strdup(359, &uslot[slot].kptr, xctx->schsymbolprop);
  my_strdup(176, &uslot[slot].eptr, xctx->schtedaxprop);

  free_lines(slot);
  free_rects(slot);
  free_polygons(slot);
  free_arcs(slot);
  free_wires(slot);
  free_texts(slot);
  free_instances(slot);

  for(c=0;c<cadlayers;c++) {
    uslot[slot].lines[c] = xctx->lines[c];
    uslot[slot].rects[c] = xctx->rects[c];
    uslot[slot].arcs[c] = xctx->arcs[c];
    uslot[slot].polygons[c] = xctx->polygons[c];
    uslot[slot].lptr[c] = my_calloc(177, xctx->lines[c], sizeof(xLine));
    uslot[slot].bptr[c] = my_calloc(178, xctx->rects[c], sizeof(xRect));
    uslot[slot].pptr[c] = my_calloc(179, xctx->polygons[c], sizeof(xPoly));
    uslot[slot].aptr[c] = my_calloc(180, xctx->arcs[c], sizeof(xArc));
  }
  uslot[slot].wptr = my_calloc(181, xctx->wires, sizeof(xWire));
  uslot[slot].tptr = my_calloc(182, xctx->texts, sizeof(xText));
  uslot[slot].iptr = my_calloc(183, xctx->instances, sizeof(xInstance));
  uslot[slot].texts = xctx->texts;
  uslot[slot].instances = xctx->instances;
  uslot[slot].wires = xctx->wires;

  for(c=0;c<cadlayers;c++) {
    /* lines */
    for(i=0;i<xctx->lines[c];i++) {
      uslot[slot].lptr[c][i] = xctx->line[c][i];
      uslot[slot].lptr[c][i].prop_ptr = NULL;
      my_strdup(184, &uslot[slot].lptr[c][i].prop_ptr, xctx->line[c][i].prop_ptr);
    }
    /* rects */
    for(i=0;i<xctx->rects[c];i++) {
      uslot[slot].bptr[c][i] = xctx->rect[c][i];
      uslot[slot].bptr[c][i].prop_ptr = NULL;
      my_strdup(185, &uslot[slot].bptr[c][i].prop_ptr, xctx->rect[c][i].prop_ptr);
    }
    /* arcs */
    for(i=0;i<xctx->arcs[c];i++) {
      uslot[slot].aptr[c][i] = xctx->arc[c][i];
      uslot[slot].aptr[c][i].prop_ptr = NULL;
      my_strdup(186, &uslot[slot].aptr[c][i].prop_ptr, xctx->arc[c][i].prop_ptr);
    }
    /*polygons */
    for(i=0;i<xctx->polygons[c];i++) {
      int points = xctx->poly[c][i].points;
      uslot[slot].pptr[c][i] = xctx->poly[c][i];
      uslot[slot].pptr[c][i].prop_ptr = NULL;
      uslot[slot].pptr[c][i].x = NULL;
      uslot[slot].pptr[c][i].y = NULL;
      uslot[slot].pptr[c][i].selected_point = NULL;
      uslot[slot].pptr[c][i].x = my_malloc(187, points * sizeof(double));
      uslot[slot].pptr[c][i].y = my_malloc(188, points * sizeof(double));
      uslot[slot].pptr[c][i].selected_point = my_malloc(189, points * sizeof(unsigned short));
      memcpy(uslot[slot].pptr[c][i].x, xctx->poly[c][i].x, points * sizeof(double));
      memcpy(uslot[slot].pptr[c][i].y, xctx->poly[c][i].y, points * sizeof(double));
      memcpy(uslot[slot].pptr[c][i].selected_point, xctx->poly[c][i].selected_point, 
        points * sizeof(unsigned short));
      my_strdup(190, &uslot[slot].pptr[c][i].prop_ptr, xctx->poly[c][i].prop_ptr);
    }
  }
  /* instances */
  for(i=0;i<xctx->instances;i++) {
    uslot[slot].iptr[i] = xctx->inst[i];
    uslot[slot].iptr[i].prop_ptr = NULL;
    uslot[slot].iptr[i].name = NULL;
    uslot[slot].iptr[i].instname = NULL;
    uslot[slot].iptr[i].lab = NULL;
    uslot[slot].iptr[i].node = NULL;
    my_strdup(330, &uslot[slot].iptr[i].lab, xctx->inst[i].lab);
    my_strdup2(191, &uslot[slot].iptr[i].instname, xctx->inst[i].instname);
    my_strdup(192, &uslot[slot].iptr[i].prop_ptr, xctx->inst[i].prop_ptr);
    my_strdup(193, &uslot[slot].iptr[i].name, xctx->inst[i].name);
  }
  /* texts */
  for(i=0;i<xctx->texts;i++) {
    uslot[slot].tptr[i] = xctx->text[i];
    uslot[slot].tptr[i].prop_ptr = NULL;
    uslot[slot].tptr[i].txt_ptr = NULL;
    uslot[slot].tptr[i].font = NULL;
    my_strdup(194, &uslot[slot].tptr[i].prop_ptr, xctx->text[i].prop_ptr);
    my_strdup(195, &uslot[slot].tptr[i].txt_ptr, xctx->text[i].txt_ptr);
    my_strdup(196, &uslot[slot].tptr[i].font, xctx->text[i].font);
  }

  /* wires */
  for(i=0;i<xctx->wires;i++) {
    uslot[slot].wptr[i] = xctx->wire[i];
    uslot[slot].wptr[i].prop_ptr = NULL;
    uslot[slot].wptr[i].node = NULL;
    my_strdup(197, &uslot[slot].wptr[i].prop_ptr, xctx->wire[i].prop_ptr);
  }


  xctx->cur_undo_ptr++;
  xctx->head_undo_ptr = xctx->cur_undo_ptr;
  xctx->tail_undo_ptr = xctx->head_undo_ptr <= MAX_UNDO? 0: xctx->head_undo_ptr-MAX_UNDO;
}


void pop_undo(int redo)
{
  int slot, i, c;

  if(xctx->no_undo)return;
  if(redo) {
    if(xctx->cur_undo_ptr < xctx->head_undo_ptr) {
      xctx->cur_undo_ptr++;
    } else {
      return;
    }
  } else {  /*redo=0 (undo) */
    if(xctx->cur_undo_ptr == xctx->tail_undo_ptr) return;
    if(xctx->head_undo_ptr == xctx->cur_undo_ptr) {
      push_undo();
      xctx->head_undo_ptr--;
      xctx->cur_undo_ptr--;
    }
    if(xctx->cur_undo_ptr<=0) return; /* check undo tail */
    xctx->cur_undo_ptr--;
  }
  slot = xctx->cur_undo_ptr%MAX_UNDO;
  clear_drawing();
  unselect_all();
  my_strdup(198, &xctx->schvhdlprop, uslot[slot].gptr);
  my_strdup(199, &xctx->schverilogprop, uslot[slot].vptr);
  my_strdup(200, &xctx->schprop, uslot[slot].sptr);
  my_strdup(389, &xctx->schsymbolprop, uslot[slot].kptr);
  my_strdup(201, &xctx->schtedaxprop, uslot[slot].eptr);
  for(c=0;c<cadlayers;c++) {
    /* lines */
    xctx->maxl[c] = xctx->lines[c] = uslot[slot].lines[c];
    xctx->line[c] = my_calloc(202, xctx->lines[c], sizeof(xLine));
    for(i=0;i<xctx->lines[c];i++) {
      xctx->line[c][i] = uslot[slot].lptr[c][i];
      xctx->line[c][i].prop_ptr=NULL;
      my_strdup(203, &xctx->line[c][i].prop_ptr, uslot[slot].lptr[c][i].prop_ptr);
    }
    /* rects */
    xctx->maxr[c] = xctx->rects[c] = uslot[slot].rects[c];
    xctx->rect[c] = my_calloc(204, xctx->rects[c], sizeof(xRect));
    for(i=0;i<xctx->rects[c];i++) {
      xctx->rect[c][i] = uslot[slot].bptr[c][i];
      xctx->rect[c][i].prop_ptr=NULL;
      my_strdup(205, &xctx->rect[c][i].prop_ptr, uslot[slot].bptr[c][i].prop_ptr);
    }
    /* arcs */
    xctx->maxa[c] = xctx->arcs[c] = uslot[slot].arcs[c];
    xctx->arc[c] = my_calloc(206, xctx->arcs[c], sizeof(xArc));
    for(i=0;i<xctx->arcs[c];i++) {
      xctx->arc[c][i] = uslot[slot].aptr[c][i];
      xctx->arc[c][i].prop_ptr=NULL;
      my_strdup(207, &xctx->arc[c][i].prop_ptr, uslot[slot].aptr[c][i].prop_ptr);
    }
    /* polygons */
    xctx->maxp[c] = xctx->polygons[c] = uslot[slot].polygons[c];
    xctx->poly[c] = my_calloc(208, xctx->polygons[c], sizeof(xPoly));
    for(i=0;i<xctx->polygons[c];i++) {
      int points = uslot[slot].pptr[c][i].points;
      xctx->poly[c][i] = uslot[slot].pptr[c][i];
      xctx->poly[c][i].prop_ptr=NULL;
      xctx->poly[c][i].x=NULL;
      xctx->poly[c][i].y=NULL;
      xctx->poly[c][i].selected_point=NULL;
      my_strdup(209, &xctx->poly[c][i].prop_ptr, uslot[slot].pptr[c][i].prop_ptr);
      my_realloc(210, &xctx->poly[c][i].x, points * sizeof(double));
      my_realloc(211, &xctx->poly[c][i].y, points * sizeof(double));
      my_realloc(212, &xctx->poly[c][i].selected_point, points * sizeof(unsigned short));
      memcpy(xctx->poly[c][i].x, uslot[slot].pptr[c][i].x, points * sizeof(double));
      memcpy(xctx->poly[c][i].y, uslot[slot].pptr[c][i].y, points * sizeof(double));
      memcpy(xctx->poly[c][i].selected_point, uslot[slot].pptr[c][i].selected_point, 
        points * sizeof(unsigned short));
    }

  }

  /* instances */
  xctx->maxi = xctx->instances = uslot[slot].instances;
  xctx->inst = my_calloc(213, xctx->instances, sizeof(xInstance));
  for(i=0;i<xctx->instances;i++) {
    xctx->inst[i] = uslot[slot].iptr[i];
    xctx->inst[i].prop_ptr=NULL;
    xctx->inst[i].name=NULL;
    xctx->inst[i].instname=NULL;
    xctx->inst[i].lab=NULL;
    my_strdup(214, &xctx->inst[i].prop_ptr, uslot[slot].iptr[i].prop_ptr);
    my_strdup(215, &xctx->inst[i].name, uslot[slot].iptr[i].name);
    my_strdup2(216, &xctx->inst[i].instname, uslot[slot].iptr[i].instname);
    my_strdup(766, &xctx->inst[i].lab, uslot[slot].iptr[i].lab);
  }

  /* texts */
  xctx->maxt = xctx->texts = uslot[slot].texts;
  xctx->text = my_calloc(217, xctx->texts, sizeof(xText));
  for(i=0;i<xctx->texts;i++) {
    xctx->text[i] = uslot[slot].tptr[i];
    xctx->text[i].txt_ptr=NULL;
    xctx->text[i].font=NULL;
    xctx->text[i].prop_ptr=NULL;
    my_strdup(218, &xctx->text[i].prop_ptr, uslot[slot].tptr[i].prop_ptr);
    my_strdup(219, &xctx->text[i].txt_ptr, uslot[slot].tptr[i].txt_ptr);
    my_strdup(220, &xctx->text[i].font, uslot[slot].tptr[i].font);
  }

  /* wires */
  xctx->maxw = xctx->wires = uslot[slot].wires;
  xctx->wire = my_calloc(221, xctx->wires, sizeof(xWire));
  for(i=0;i<xctx->wires;i++) {
    xctx->wire[i] = uslot[slot].wptr[i];
    xctx->wire[i].prop_ptr=NULL;
    xctx->wire[i].node=NULL;
    my_strdup(222, &xctx->wire[i].prop_ptr, uslot[slot].wptr[i].prop_ptr);
  }

  link_symbols_to_instances(-1);
  set_modify(1);
  xctx->prep_hash_inst=0;
  xctx->prep_hash_wires=0;
  xctx->prep_net_structs=0;
  xctx->prep_hi_structs=0;
  update_conn_cues(0, 0);
}

#endif
