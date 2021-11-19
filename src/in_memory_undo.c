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

void init_undo()
{
  int slot;

  for(slot=0;slot<MAX_UNDO; slot++) {
    xctx->uslot[slot].lines=my_calloc(165, cadlayers, sizeof(int));
    xctx->uslot[slot].rects=my_calloc(166, cadlayers, sizeof(int));
    xctx->uslot[slot].arcs=my_calloc(167, cadlayers, sizeof(int));
    xctx->uslot[slot].polygons=my_calloc(168, cadlayers, sizeof(int));
    xctx->uslot[slot].lptr=my_calloc(169, cadlayers, sizeof(xLine *));
    xctx->uslot[slot].bptr=my_calloc(170, cadlayers, sizeof(xRect *));
    xctx->uslot[slot].aptr=my_calloc(171, cadlayers, sizeof(xArc *));
    xctx->uslot[slot].pptr=my_calloc(172, cadlayers, sizeof(xPoly *));
  }
}

void free_lines(int slot)
{
  int i, c;

  for(c=0;c<cadlayers; c++) {
    for(i=0;i<xctx->uslot[slot].lines[c]; i++) {
      my_free(783, &xctx->uslot[slot].lptr[c][i].prop_ptr);
    }
    my_free(784, &xctx->uslot[slot].lptr[c]);
    xctx->uslot[slot].lines[c] = 0;
  }
}

void free_rects(int slot)
{
  int i, c;

  for(c=0;c<cadlayers; c++) {
    for(i=0;i<xctx->uslot[slot].rects[c]; i++) {
      my_free(785, &xctx->uslot[slot].bptr[c][i].prop_ptr);
    }
    my_free(786, &xctx->uslot[slot].bptr[c]);
    xctx->uslot[slot].rects[c] = 0;
  }
}

void free_polygons(int slot)
{
  int i, c;

  for(c=0;c<cadlayers; c++) {
    for(i=0;i<xctx->uslot[slot].polygons[c]; i++) {
      my_free(787, &xctx->uslot[slot].pptr[c][i].prop_ptr);
      my_free(788, &xctx->uslot[slot].pptr[c][i].x);
      my_free(789, &xctx->uslot[slot].pptr[c][i].y);
      my_free(790, &xctx->uslot[slot].pptr[c][i].selected_point);
    }
    my_free(791, &xctx->uslot[slot].pptr[c]);
    xctx->uslot[slot].polygons[c] = 0;
  }
}

void free_arcs(int slot)
{
  int i, c;

  for(c=0;c<cadlayers; c++) {
    for(i=0;i<xctx->uslot[slot].arcs[c]; i++) {
      my_free(792, &xctx->uslot[slot].aptr[c][i].prop_ptr);
    }
    my_free(793, &xctx->uslot[slot].aptr[c]);
    xctx->uslot[slot].arcs[c] = 0;
  }
}

void free_wires(int slot)
{
  int i;

  for(i=0;i<xctx->uslot[slot].wires; i++) {
    my_free(794, &xctx->uslot[slot].wptr[i].prop_ptr);
  }
  my_free(795, &xctx->uslot[slot].wptr);
  xctx->uslot[slot].wires = 0;
}

void free_texts(int slot)
{
  int i;

  for(i=0;i<xctx->uslot[slot].texts; i++) {
    my_free(796, &xctx->uslot[slot].tptr[i].prop_ptr);
    my_free(797, &xctx->uslot[slot].tptr[i].txt_ptr);
    my_free(798, &xctx->uslot[slot].tptr[i].font);
  }
  my_free(799, &xctx->uslot[slot].tptr);
  xctx->uslot[slot].texts = 0;
}

void free_instances(int slot)
{
  int i;

  for(i=0;i<xctx->uslot[slot].instances; i++) {
    my_free(800, &xctx->uslot[slot].iptr[i].name);
    my_free(801, &xctx->uslot[slot].iptr[i].prop_ptr);
    my_free(802, &xctx->uslot[slot].iptr[i].instname);
    my_free(104, &xctx->uslot[slot].iptr[i].lab);
  }
  my_free(803, &xctx->uslot[slot].iptr);
  xctx->uslot[slot].instances = 0;
}

void clear_undo(void)
{
  int slot;
  xctx->cur_undo_ptr = 0;
  xctx->tail_undo_ptr = 0;
  xctx->head_undo_ptr = 0;
  if(!xctx->initialized) return;
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
    my_free(804, &xctx->uslot[slot].lines);
    my_free(805, &xctx->uslot[slot].rects);
    my_free(806, &xctx->uslot[slot].arcs);
    my_free(807, &xctx->uslot[slot].polygons);
    my_free(808, &xctx->uslot[slot].lptr);
    my_free(809, &xctx->uslot[slot].bptr);
    my_free(810, &xctx->uslot[slot].aptr);
    my_free(811, &xctx->uslot[slot].pptr);
  }

}

void push_undo(void)
{
  int slot, i, c;

  if(xctx->no_undo)return;
  if(!xctx->initialized) {
    xctx->initialized=1;
    init_undo();
  }
  slot = xctx->cur_undo_ptr%MAX_UNDO;

  my_strdup(173, &xctx->uslot[slot].gptr, xctx->schvhdlprop);
  my_strdup(174, &xctx->uslot[slot].vptr, xctx->schverilogprop);
  my_strdup(175, &xctx->uslot[slot].sptr, xctx->schprop);
  my_strdup(359, &xctx->uslot[slot].kptr, xctx->schsymbolprop);
  my_strdup(176, &xctx->uslot[slot].eptr, xctx->schtedaxprop);

  free_lines(slot);
  free_rects(slot);
  free_polygons(slot);
  free_arcs(slot);
  free_wires(slot);
  free_texts(slot);
  free_instances(slot);

  for(c=0;c<cadlayers;c++) {
    xctx->uslot[slot].lines[c] = xctx->lines[c];
    xctx->uslot[slot].rects[c] = xctx->rects[c];
    xctx->uslot[slot].arcs[c] = xctx->arcs[c];
    xctx->uslot[slot].polygons[c] = xctx->polygons[c];
    xctx->uslot[slot].lptr[c] = my_calloc(177, xctx->lines[c], sizeof(xLine));
    xctx->uslot[slot].bptr[c] = my_calloc(178, xctx->rects[c], sizeof(xRect));
    xctx->uslot[slot].pptr[c] = my_calloc(179, xctx->polygons[c], sizeof(xPoly));
    xctx->uslot[slot].aptr[c] = my_calloc(180, xctx->arcs[c], sizeof(xArc));
  }
  xctx->uslot[slot].wptr = my_calloc(181, xctx->wires, sizeof(xWire));
  xctx->uslot[slot].tptr = my_calloc(182, xctx->texts, sizeof(xText));
  xctx->uslot[slot].iptr = my_calloc(183, xctx->instances, sizeof(xInstance));
  xctx->uslot[slot].texts = xctx->texts;
  xctx->uslot[slot].instances = xctx->instances;
  xctx->uslot[slot].wires = xctx->wires;

  for(c=0;c<cadlayers;c++) {
    /* lines */
    for(i=0;i<xctx->lines[c];i++) {
      xctx->uslot[slot].lptr[c][i] = xctx->line[c][i];
      xctx->uslot[slot].lptr[c][i].prop_ptr = NULL;
      my_strdup(184, &xctx->uslot[slot].lptr[c][i].prop_ptr, xctx->line[c][i].prop_ptr);
    }
    /* rects */
    for(i=0;i<xctx->rects[c];i++) {
      xctx->uslot[slot].bptr[c][i] = xctx->rect[c][i];
      xctx->uslot[slot].bptr[c][i].prop_ptr = NULL;
      my_strdup(185, &xctx->uslot[slot].bptr[c][i].prop_ptr, xctx->rect[c][i].prop_ptr);
    }
    /* arcs */
    for(i=0;i<xctx->arcs[c];i++) {
      xctx->uslot[slot].aptr[c][i] = xctx->arc[c][i];
      xctx->uslot[slot].aptr[c][i].prop_ptr = NULL;
      my_strdup(186, &xctx->uslot[slot].aptr[c][i].prop_ptr, xctx->arc[c][i].prop_ptr);
    }
    /*polygons */
    for(i=0;i<xctx->polygons[c];i++) {
      int points = xctx->poly[c][i].points;
      xctx->uslot[slot].pptr[c][i] = xctx->poly[c][i];
      xctx->uslot[slot].pptr[c][i].prop_ptr = NULL;
      xctx->uslot[slot].pptr[c][i].x = NULL;
      xctx->uslot[slot].pptr[c][i].y = NULL;
      xctx->uslot[slot].pptr[c][i].selected_point = NULL;
      xctx->uslot[slot].pptr[c][i].x = my_malloc(187, points * sizeof(double));
      xctx->uslot[slot].pptr[c][i].y = my_malloc(188, points * sizeof(double));
      xctx->uslot[slot].pptr[c][i].selected_point = my_malloc(189, points * sizeof(unsigned short));
      memcpy(xctx->uslot[slot].pptr[c][i].x, xctx->poly[c][i].x, points * sizeof(double));
      memcpy(xctx->uslot[slot].pptr[c][i].y, xctx->poly[c][i].y, points * sizeof(double));
      memcpy(xctx->uslot[slot].pptr[c][i].selected_point, xctx->poly[c][i].selected_point, 
        points * sizeof(unsigned short));
      my_strdup(190, &xctx->uslot[slot].pptr[c][i].prop_ptr, xctx->poly[c][i].prop_ptr);
    }
  }
  /* instances */
  for(i=0;i<xctx->instances;i++) {
    xctx->uslot[slot].iptr[i] = xctx->inst[i];
    xctx->uslot[slot].iptr[i].prop_ptr = NULL;
    xctx->uslot[slot].iptr[i].name = NULL;
    xctx->uslot[slot].iptr[i].instname = NULL;
    xctx->uslot[slot].iptr[i].lab = NULL;
    xctx->uslot[slot].iptr[i].node = NULL;
    my_strdup(330, &xctx->uslot[slot].iptr[i].lab, xctx->inst[i].lab);
    my_strdup2(191, &xctx->uslot[slot].iptr[i].instname, xctx->inst[i].instname);
    my_strdup(192, &xctx->uslot[slot].iptr[i].prop_ptr, xctx->inst[i].prop_ptr);
    my_strdup(193, &xctx->uslot[slot].iptr[i].name, xctx->inst[i].name);
  }
  /* texts */
  for(i=0;i<xctx->texts;i++) {
    xctx->uslot[slot].tptr[i] = xctx->text[i];
    xctx->uslot[slot].tptr[i].prop_ptr = NULL;
    xctx->uslot[slot].tptr[i].txt_ptr = NULL;
    xctx->uslot[slot].tptr[i].font = NULL;
    my_strdup(194, &xctx->uslot[slot].tptr[i].prop_ptr, xctx->text[i].prop_ptr);
    my_strdup(195, &xctx->uslot[slot].tptr[i].txt_ptr, xctx->text[i].txt_ptr);
    my_strdup(196, &xctx->uslot[slot].tptr[i].font, xctx->text[i].font);
  }

  /* wires */
  for(i=0;i<xctx->wires;i++) {
    xctx->uslot[slot].wptr[i] = xctx->wire[i];
    xctx->uslot[slot].wptr[i].prop_ptr = NULL;
    xctx->uslot[slot].wptr[i].node = NULL;
    my_strdup(197, &xctx->uslot[slot].wptr[i].prop_ptr, xctx->wire[i].prop_ptr);
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
  my_strdup(198, &xctx->schvhdlprop, xctx->uslot[slot].gptr);
  my_strdup(199, &xctx->schverilogprop, xctx->uslot[slot].vptr);
  my_strdup(200, &xctx->schprop, xctx->uslot[slot].sptr);
  my_strdup(389, &xctx->schsymbolprop, xctx->uslot[slot].kptr);
  my_strdup(201, &xctx->schtedaxprop, xctx->uslot[slot].eptr);
  for(c=0;c<cadlayers;c++) {
    /* lines */
    xctx->maxl[c] = xctx->lines[c] = xctx->uslot[slot].lines[c];
    xctx->line[c] = my_calloc(202, xctx->lines[c], sizeof(xLine));
    for(i=0;i<xctx->lines[c];i++) {
      xctx->line[c][i] = xctx->uslot[slot].lptr[c][i];
      xctx->line[c][i].prop_ptr=NULL;
      my_strdup(203, &xctx->line[c][i].prop_ptr, xctx->uslot[slot].lptr[c][i].prop_ptr);
    }
    /* rects */
    xctx->maxr[c] = xctx->rects[c] = xctx->uslot[slot].rects[c];
    xctx->rect[c] = my_calloc(204, xctx->rects[c], sizeof(xRect));
    for(i=0;i<xctx->rects[c];i++) {
      xctx->rect[c][i] = xctx->uslot[slot].bptr[c][i];
      xctx->rect[c][i].prop_ptr=NULL;
      my_strdup(205, &xctx->rect[c][i].prop_ptr, xctx->uslot[slot].bptr[c][i].prop_ptr);
    }
    /* arcs */
    xctx->maxa[c] = xctx->arcs[c] = xctx->uslot[slot].arcs[c];
    xctx->arc[c] = my_calloc(206, xctx->arcs[c], sizeof(xArc));
    for(i=0;i<xctx->arcs[c];i++) {
      xctx->arc[c][i] = xctx->uslot[slot].aptr[c][i];
      xctx->arc[c][i].prop_ptr=NULL;
      my_strdup(207, &xctx->arc[c][i].prop_ptr, xctx->uslot[slot].aptr[c][i].prop_ptr);
    }
    /* polygons */
    xctx->maxp[c] = xctx->polygons[c] = xctx->uslot[slot].polygons[c];
    xctx->poly[c] = my_calloc(208, xctx->polygons[c], sizeof(xPoly));
    for(i=0;i<xctx->polygons[c];i++) {
      int points = xctx->uslot[slot].pptr[c][i].points;
      xctx->poly[c][i] = xctx->uslot[slot].pptr[c][i];
      xctx->poly[c][i].prop_ptr=NULL;
      xctx->poly[c][i].x=NULL;
      xctx->poly[c][i].y=NULL;
      xctx->poly[c][i].selected_point=NULL;
      my_strdup(209, &xctx->poly[c][i].prop_ptr, xctx->uslot[slot].pptr[c][i].prop_ptr);
      my_realloc(210, &xctx->poly[c][i].x, points * sizeof(double));
      my_realloc(211, &xctx->poly[c][i].y, points * sizeof(double));
      my_realloc(212, &xctx->poly[c][i].selected_point, points * sizeof(unsigned short));
      memcpy(xctx->poly[c][i].x, xctx->uslot[slot].pptr[c][i].x, points * sizeof(double));
      memcpy(xctx->poly[c][i].y, xctx->uslot[slot].pptr[c][i].y, points * sizeof(double));
      memcpy(xctx->poly[c][i].selected_point, xctx->uslot[slot].pptr[c][i].selected_point, 
        points * sizeof(unsigned short));
    }

  }

  /* instances */
  xctx->maxi = xctx->instances = xctx->uslot[slot].instances;
  xctx->inst = my_calloc(213, xctx->instances, sizeof(xInstance));
  for(i=0;i<xctx->instances;i++) {
    xctx->inst[i] = xctx->uslot[slot].iptr[i];
    xctx->inst[i].prop_ptr=NULL;
    xctx->inst[i].name=NULL;
    xctx->inst[i].instname=NULL;
    xctx->inst[i].lab=NULL;
    my_strdup(214, &xctx->inst[i].prop_ptr, xctx->uslot[slot].iptr[i].prop_ptr);
    my_strdup(215, &xctx->inst[i].name, xctx->uslot[slot].iptr[i].name);
    my_strdup2(216, &xctx->inst[i].instname, xctx->uslot[slot].iptr[i].instname);
    my_strdup(766, &xctx->inst[i].lab, xctx->uslot[slot].iptr[i].lab);
  }

  /* texts */
  xctx->maxt = xctx->texts = xctx->uslot[slot].texts;
  xctx->text = my_calloc(217, xctx->texts, sizeof(xText));
  for(i=0;i<xctx->texts;i++) {
    xctx->text[i] = xctx->uslot[slot].tptr[i];
    xctx->text[i].txt_ptr=NULL;
    xctx->text[i].font=NULL;
    xctx->text[i].prop_ptr=NULL;
    my_strdup(218, &xctx->text[i].prop_ptr, xctx->uslot[slot].tptr[i].prop_ptr);
    my_strdup(219, &xctx->text[i].txt_ptr, xctx->uslot[slot].tptr[i].txt_ptr);
    my_strdup(220, &xctx->text[i].font, xctx->uslot[slot].tptr[i].font);
  }

  /* wires */
  xctx->maxw = xctx->wires = xctx->uslot[slot].wires;
  xctx->wire = my_calloc(221, xctx->wires, sizeof(xWire));
  for(i=0;i<xctx->wires;i++) {
    xctx->wire[i] = xctx->uslot[slot].wptr[i];
    xctx->wire[i].prop_ptr=NULL;
    xctx->wire[i].node=NULL;
    my_strdup(222, &xctx->wire[i].prop_ptr, xctx->uslot[slot].wptr[i].prop_ptr);
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
