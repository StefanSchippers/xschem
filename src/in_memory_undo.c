/* File: in_memory_undo.c
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

static void free_undo_lines(int slot)
{
  int i, c;

  for(c = 0;c<cadlayers; ++c) {
    for(i = 0;i<xctx->uslot[slot].lines[c]; ++i) {
      my_free(_ALLOC_ID_, &xctx->uslot[slot].lptr[c][i].prop_ptr);
    }
    my_free(_ALLOC_ID_, &xctx->uslot[slot].lptr[c]);
    xctx->uslot[slot].lines[c] = 0;
  }
}

static void free_undo_rects(int slot)
{
  int i, c;

  for(c = 0;c<cadlayers; ++c) {
    for(i = 0;i<xctx->uslot[slot].rects[c]; ++i) {
      my_free(_ALLOC_ID_, &xctx->uslot[slot].bptr[c][i].prop_ptr);
    }
    my_free(_ALLOC_ID_, &xctx->uslot[slot].bptr[c]);
    xctx->uslot[slot].rects[c] = 0;
  }
}

static void free_undo_polygons(int slot)
{
  int i, c;

  for(c = 0;c<cadlayers; ++c) {
    for(i = 0;i<xctx->uslot[slot].polygons[c]; ++i) {
      my_free(_ALLOC_ID_, &xctx->uslot[slot].pptr[c][i].prop_ptr);
      my_free(_ALLOC_ID_, &xctx->uslot[slot].pptr[c][i].x);
      my_free(_ALLOC_ID_, &xctx->uslot[slot].pptr[c][i].y);
      my_free(_ALLOC_ID_, &xctx->uslot[slot].pptr[c][i].selected_point);
    }
    my_free(_ALLOC_ID_, &xctx->uslot[slot].pptr[c]);
    xctx->uslot[slot].polygons[c] = 0;
  }
}

static void free_undo_arcs(int slot)
{
  int i, c;

  for(c = 0;c<cadlayers; ++c) {
    for(i = 0;i<xctx->uslot[slot].arcs[c]; ++i) {
      my_free(_ALLOC_ID_, &xctx->uslot[slot].aptr[c][i].prop_ptr);
    }
    my_free(_ALLOC_ID_, &xctx->uslot[slot].aptr[c]);
    xctx->uslot[slot].arcs[c] = 0;
  }
}

static void free_undo_wires(int slot)
{
  int i;

  for(i = 0;i<xctx->uslot[slot].wires; ++i) {
    my_free(_ALLOC_ID_, &xctx->uslot[slot].wptr[i].prop_ptr);
  }
  my_free(_ALLOC_ID_, &xctx->uslot[slot].wptr);
  xctx->uslot[slot].wires = 0;
}

static void free_undo_texts(int slot)
{
  int i;

  for(i = 0;i<xctx->uslot[slot].texts; ++i) {
    my_free(_ALLOC_ID_, &xctx->uslot[slot].tptr[i].prop_ptr);
    my_free(_ALLOC_ID_, &xctx->uslot[slot].tptr[i].txt_ptr);
    my_free(_ALLOC_ID_, &xctx->uslot[slot].tptr[i].font);
    my_free(_ALLOC_ID_, &xctx->uslot[slot].tptr[i].floater_instname);
    my_free(_ALLOC_ID_, &xctx->uslot[slot].tptr[i].floater_ptr);
  }
  my_free(_ALLOC_ID_, &xctx->uslot[slot].tptr);
  xctx->uslot[slot].texts = 0;
}

static void free_undo_instances(int slot)
{
  int i;

  for(i = 0;i<xctx->uslot[slot].instances; ++i) {
    my_free(_ALLOC_ID_, &xctx->uslot[slot].iptr[i].name);
    my_free(_ALLOC_ID_, &xctx->uslot[slot].iptr[i].prop_ptr);
    my_free(_ALLOC_ID_, &xctx->uslot[slot].iptr[i].instname);
    my_free(_ALLOC_ID_, &xctx->uslot[slot].iptr[i].lab);
  }
  my_free(_ALLOC_ID_, &xctx->uslot[slot].iptr);
  xctx->uslot[slot].instances = 0;
}

static void free_undo_symbols(int slot)
{
  int i, j, c, symbols;
  xSymbol *sym;

  symbols = xctx->uslot[slot].symbols;
  for(i = 0;i < symbols; ++i) {
    sym = &xctx->uslot[slot].symptr[i];
    my_free(_ALLOC_ID_, &sym->name);
    my_free(_ALLOC_ID_, &sym->prop_ptr);
    my_free(_ALLOC_ID_, &sym->type);
    my_free(_ALLOC_ID_, &sym->templ);
    my_free(_ALLOC_ID_, &sym->parent_prop_ptr);

    for(c = 0;c<cadlayers; ++c) {
      for(j = 0;j<sym->polygons[c]; ++j) {
        if(sym->poly[c][j].prop_ptr != NULL) {
          my_free(_ALLOC_ID_, &sym->poly[c][j].prop_ptr);
        }
        my_free(_ALLOC_ID_, &sym->poly[c][j].x);
        my_free(_ALLOC_ID_, &sym->poly[c][j].y);
        my_free(_ALLOC_ID_, &sym->poly[c][j].selected_point);
      }
      my_free(_ALLOC_ID_, &sym->poly[c]);
      sym->polygons[c] = 0;
  
      for(j = 0;j<sym->lines[c]; ++j) {
        if(sym->line[c][j].prop_ptr != NULL) {
          my_free(_ALLOC_ID_, &sym->line[c][j].prop_ptr);
        }
      }
      my_free(_ALLOC_ID_, &sym->line[c]);
      sym->lines[c] = 0;
  
      for(j = 0;j<sym->arcs[c]; ++j) {
        if(sym->arc[c][j].prop_ptr != NULL) {
          my_free(_ALLOC_ID_, &sym->arc[c][j].prop_ptr);
        }
      }
      my_free(_ALLOC_ID_, &sym->arc[c]);
      sym->arcs[c] = 0;
  
      for(j = 0;j<sym->rects[c]; ++j) {
        if(sym->rect[c][j].prop_ptr != NULL) {
          my_free(_ALLOC_ID_, &sym->rect[c][j].prop_ptr);
        }
      }
      my_free(_ALLOC_ID_, &sym->rect[c]);
      sym->rects[c] = 0;
    }
    for(j = 0;j<sym->texts; ++j) {
      if(sym->text[j].prop_ptr != NULL) {
        my_free(_ALLOC_ID_, &sym->text[j].prop_ptr);
      }
      if(sym->text[j].txt_ptr != NULL) {
        my_free(_ALLOC_ID_, &sym->text[j].txt_ptr);
      }
      if(sym->text[j].font != NULL) {
        my_free(_ALLOC_ID_, &sym->text[j].font);
      }
      if(sym->text[j].floater_instname != NULL) {
        my_free(_ALLOC_ID_, &sym->text[j].floater_instname);
      }
      if(sym->text[j].floater_ptr != NULL) {
        my_free(_ALLOC_ID_, &sym->text[j].floater_ptr);
      }
    }
    my_free(_ALLOC_ID_, &sym->text);
    sym->texts = 0;
    my_free(_ALLOC_ID_, &sym->line);
    my_free(_ALLOC_ID_, &sym->rect);
    my_free(_ALLOC_ID_, &sym->poly);
    my_free(_ALLOC_ID_, &sym->arc);
    my_free(_ALLOC_ID_, &sym->lines);
    my_free(_ALLOC_ID_, &sym->rects);
    my_free(_ALLOC_ID_, &sym->polygons);
    my_free(_ALLOC_ID_, &sym->arcs);
  }
  my_free(_ALLOC_ID_, &xctx->uslot[slot].symptr);
  xctx->uslot[slot].symbols = 0;
}

static void mem_init_undo(void)
{
  int slot;

  dbg(1, "mem_init_undo(): undo_initialized = %d\n", xctx->undo_initialized);
  if(!xctx->undo_initialized) {
    for(slot = 0;slot<MAX_UNDO; slot++) {
      xctx->uslot[slot].lines = my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
      xctx->uslot[slot].rects = my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
      xctx->uslot[slot].arcs = my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
      xctx->uslot[slot].polygons = my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
      xctx->uslot[slot].lptr = my_calloc(_ALLOC_ID_, cadlayers, sizeof(xLine *));
      xctx->uslot[slot].bptr = my_calloc(_ALLOC_ID_, cadlayers, sizeof(xRect *));
      xctx->uslot[slot].aptr = my_calloc(_ALLOC_ID_, cadlayers, sizeof(xArc *));
      xctx->uslot[slot].pptr = my_calloc(_ALLOC_ID_, cadlayers, sizeof(xPoly *));
    }
    xctx->undo_initialized = 1;
  }
}

/* called when program resets undo stack (for example when loading a new file */
void mem_clear_undo(void)
{
  int slot;
  dbg(1, "mem_clear_undo(): undo_initialized = %d\n", xctx->undo_initialized);
  xctx->cur_undo_ptr = 0;
  xctx->tail_undo_ptr = 0;
  xctx->head_undo_ptr = 0;
  if(!xctx->undo_initialized) return;
  for(slot = 0; slot<MAX_UNDO; slot++) {
    free_undo_lines(slot);
    free_undo_rects(slot);
    free_undo_polygons(slot);
    free_undo_arcs(slot);
    free_undo_wires(slot);
    free_undo_texts(slot);
    free_undo_instances(slot);
    free_undo_symbols(slot);
  }
}

/* used to delete everything when program exits */
void mem_delete_undo(void)
{
  int slot;
  dbg(1, "mem_delete_undo(): undo_initialized = %d\n", xctx->undo_initialized);
  if(!xctx->undo_initialized) return;
  mem_clear_undo();
  for(slot = 0;slot<MAX_UNDO; slot++) {
    my_free(_ALLOC_ID_, &xctx->uslot[slot].lines);
    my_free(_ALLOC_ID_, &xctx->uslot[slot].rects);
    my_free(_ALLOC_ID_, &xctx->uslot[slot].arcs);
    my_free(_ALLOC_ID_, &xctx->uslot[slot].polygons);
    my_free(_ALLOC_ID_, &xctx->uslot[slot].lptr);
    my_free(_ALLOC_ID_, &xctx->uslot[slot].bptr);
    my_free(_ALLOC_ID_, &xctx->uslot[slot].aptr);
    my_free(_ALLOC_ID_, &xctx->uslot[slot].pptr);
  }
  xctx->undo_initialized = 0;
}

void mem_push_undo(void)
{
  int slot, i, c;

  if(xctx->no_undo)return;
  mem_init_undo();
  slot = xctx->cur_undo_ptr%MAX_UNDO;

  my_strdup(_ALLOC_ID_, &xctx->uslot[slot].gptr, xctx->schvhdlprop);
  my_strdup(_ALLOC_ID_, &xctx->uslot[slot].vptr, xctx->schverilogprop);
  my_strdup(_ALLOC_ID_, &xctx->uslot[slot].sptr, xctx->schprop);
  my_strdup(_ALLOC_ID_, &xctx->uslot[slot].kptr, xctx->schsymbolprop);
  my_strdup(_ALLOC_ID_, &xctx->uslot[slot].eptr, xctx->schtedaxprop);

  free_undo_lines(slot);
  free_undo_rects(slot);
  free_undo_polygons(slot);
  free_undo_arcs(slot);
  free_undo_wires(slot);
  free_undo_texts(slot);
  free_undo_instances(slot);
  free_undo_symbols(slot);

  memcpy(xctx->uslot[slot].lines, xctx->lines, sizeof(xctx->lines[0]) * cadlayers);
  memcpy(xctx->uslot[slot].rects, xctx->rects, sizeof(xctx->rects[0]) * cadlayers);
  memcpy(xctx->uslot[slot].arcs, xctx->arcs, sizeof(xctx->arcs[0]) * cadlayers);
  memcpy(xctx->uslot[slot].polygons, xctx->polygons, sizeof(xctx->polygons[0]) * cadlayers);
  for(c = 0;c<cadlayers; ++c) {
    xctx->uslot[slot].lptr[c] = my_calloc(_ALLOC_ID_, xctx->lines[c], sizeof(xLine));
    xctx->uslot[slot].bptr[c] = my_calloc(_ALLOC_ID_, xctx->rects[c], sizeof(xRect));
    xctx->uslot[slot].pptr[c] = my_calloc(_ALLOC_ID_, xctx->polygons[c], sizeof(xPoly));
    xctx->uslot[slot].aptr[c] = my_calloc(_ALLOC_ID_, xctx->arcs[c], sizeof(xArc));
  }
  xctx->uslot[slot].wptr = my_calloc(_ALLOC_ID_, xctx->wires, sizeof(xWire));
  xctx->uslot[slot].tptr = my_calloc(_ALLOC_ID_, xctx->texts, sizeof(xText));
  xctx->uslot[slot].iptr = my_calloc(_ALLOC_ID_, xctx->instances, sizeof(xInstance));
  xctx->uslot[slot].symptr = my_calloc(_ALLOC_ID_, xctx->symbols, sizeof(xSymbol));
  xctx->uslot[slot].texts = xctx->texts;
  xctx->uslot[slot].instances = xctx->instances;
  xctx->uslot[slot].symbols = xctx->symbols;
  xctx->uslot[slot].wires = xctx->wires;

  for(c = 0;c<cadlayers; ++c) {
    /* lines */
    for(i = 0;i<xctx->lines[c]; ++i) {
      xctx->uslot[slot].lptr[c][i] = xctx->line[c][i];
      xctx->uslot[slot].lptr[c][i].prop_ptr = NULL;
      my_strdup(_ALLOC_ID_, &xctx->uslot[slot].lptr[c][i].prop_ptr, xctx->line[c][i].prop_ptr);
    }
    /* rects */
    for(i = 0;i<xctx->rects[c]; ++i) {
      xctx->uslot[slot].bptr[c][i] = xctx->rect[c][i];
      xctx->uslot[slot].bptr[c][i].prop_ptr = NULL;
      xctx->uslot[slot].bptr[c][i].extraptr = NULL;
      my_strdup(_ALLOC_ID_, &xctx->uslot[slot].bptr[c][i].prop_ptr, xctx->rect[c][i].prop_ptr);
    }
    /* arcs */
    for(i = 0;i<xctx->arcs[c]; ++i) {
      xctx->uslot[slot].aptr[c][i] = xctx->arc[c][i];
      xctx->uslot[slot].aptr[c][i].prop_ptr = NULL;
      my_strdup(_ALLOC_ID_, &xctx->uslot[slot].aptr[c][i].prop_ptr, xctx->arc[c][i].prop_ptr);
    }
    /*polygons */
    for(i = 0;i<xctx->polygons[c]; ++i) {
      int points = xctx->poly[c][i].points;
      xctx->uslot[slot].pptr[c][i] = xctx->poly[c][i];
      xctx->uslot[slot].pptr[c][i].prop_ptr = NULL;
      xctx->uslot[slot].pptr[c][i].x = my_malloc(_ALLOC_ID_, points * sizeof(double));
      xctx->uslot[slot].pptr[c][i].y = my_malloc(_ALLOC_ID_, points * sizeof(double));
      xctx->uslot[slot].pptr[c][i].selected_point = my_malloc(_ALLOC_ID_, points * sizeof(unsigned short));
      memcpy(xctx->uslot[slot].pptr[c][i].x, xctx->poly[c][i].x, points * sizeof(double));
      memcpy(xctx->uslot[slot].pptr[c][i].y, xctx->poly[c][i].y, points * sizeof(double));
      memcpy(xctx->uslot[slot].pptr[c][i].selected_point, xctx->poly[c][i].selected_point, 
        points * sizeof(unsigned short));
      my_strdup(_ALLOC_ID_, &xctx->uslot[slot].pptr[c][i].prop_ptr, xctx->poly[c][i].prop_ptr);
    }
  }
  /* instances */
  for(i = 0;i<xctx->instances; ++i) {
    xctx->uslot[slot].iptr[i] = xctx->inst[i];
    xctx->uslot[slot].iptr[i].prop_ptr = NULL;
    xctx->uslot[slot].iptr[i].name = NULL;
    xctx->uslot[slot].iptr[i].instname = NULL;
    xctx->uslot[slot].iptr[i].lab = NULL;
    xctx->uslot[slot].iptr[i].node = NULL;
    my_strdup2(_ALLOC_ID_, &xctx->uslot[slot].iptr[i].lab, xctx->inst[i].lab);
    my_strdup2(_ALLOC_ID_, &xctx->uslot[slot].iptr[i].instname, xctx->inst[i].instname);
    my_strdup2(_ALLOC_ID_, &xctx->uslot[slot].iptr[i].prop_ptr, xctx->inst[i].prop_ptr);
    my_strdup2(_ALLOC_ID_, &xctx->uslot[slot].iptr[i].name, xctx->inst[i].name);
  }

  /* symbols */
  for(i = 0;i<xctx->symbols; ++i) {
    copy_symbol(&xctx->uslot[slot].symptr[i], &xctx->sym[i]);
  }
  /* texts */
  for(i = 0;i<xctx->texts; ++i) {
    xctx->uslot[slot].tptr[i] = xctx->text[i];
    xctx->uslot[slot].tptr[i].prop_ptr = NULL;
    xctx->uslot[slot].tptr[i].txt_ptr = NULL;
    xctx->uslot[slot].tptr[i].font = NULL;
    xctx->uslot[slot].tptr[i].floater_instname = NULL;
    xctx->uslot[slot].tptr[i].floater_ptr = NULL;
    my_strdup2(_ALLOC_ID_, &xctx->uslot[slot].tptr[i].prop_ptr, xctx->text[i].prop_ptr);
    my_strdup2(_ALLOC_ID_, &xctx->uslot[slot].tptr[i].txt_ptr, xctx->text[i].txt_ptr);
    my_strdup2(_ALLOC_ID_, &xctx->uslot[slot].tptr[i].font, xctx->text[i].font);
    my_strdup2(_ALLOC_ID_, &xctx->uslot[slot].tptr[i].floater_instname, xctx->text[i].floater_instname);
    my_strdup2(_ALLOC_ID_, &xctx->uslot[slot].tptr[i].floater_ptr, xctx->text[i].floater_ptr);
  }

  /* wires */
  for(i = 0;i<xctx->wires; ++i) {
    xctx->uslot[slot].wptr[i] = xctx->wire[i];
    xctx->uslot[slot].wptr[i].prop_ptr = NULL;
    xctx->uslot[slot].wptr[i].node = NULL;
    my_strdup(_ALLOC_ID_, &xctx->uslot[slot].wptr[i].prop_ptr, xctx->wire[i].prop_ptr);
  }


  xctx->cur_undo_ptr++;
  xctx->head_undo_ptr = xctx->cur_undo_ptr;
  xctx->tail_undo_ptr = xctx->head_undo_ptr <= MAX_UNDO? 0: xctx->head_undo_ptr-MAX_UNDO;
}

/* redo:
 * 0: undo (with push current state for allowing following redo) 
 * 1: redo
 * 2: read top data from undo stack without changing undo stack
 */
void mem_pop_undo(int redo, int set_modify_status)
{
  int slot, i, c;

  if(xctx->no_undo)return;
  if(redo == 1) {
    if(xctx->cur_undo_ptr < xctx->head_undo_ptr) {
      xctx->cur_undo_ptr++;
    } else {
      return;
    }
  } else if(redo == 0) {  /* undo */
    if(xctx->cur_undo_ptr == xctx->tail_undo_ptr) return;
    if(xctx->head_undo_ptr == xctx->cur_undo_ptr) {
      xctx->push_undo();
      xctx->head_undo_ptr--;
      xctx->cur_undo_ptr--;
    }
    if(xctx->cur_undo_ptr<= 0) return; /* check undo tail */
    xctx->cur_undo_ptr--;
  } else { /* redo == 2, get data without changing undo stack */
    if(xctx->cur_undo_ptr<= 0) return; /* check undo tail */
    xctx->cur_undo_ptr--; /* will be restored at end */
  }
  slot = xctx->cur_undo_ptr%MAX_UNDO;
  clear_drawing();
  unselect_all(1);
  my_free(_ALLOC_ID_, &xctx->wire);
  my_free(_ALLOC_ID_, &xctx->text);
  my_free(_ALLOC_ID_, &xctx->inst);

  for(i = 0;i<cadlayers; ++i) {
    my_free(_ALLOC_ID_, &xctx->rect[i]);
    my_free(_ALLOC_ID_, &xctx->line[i]);
    my_free(_ALLOC_ID_, &xctx->poly[i]);
    my_free(_ALLOC_ID_, &xctx->arc[i]);
  }

  remove_symbols();


  for(i = 0;i<xctx->maxs; ++i) {
    my_free(_ALLOC_ID_, &xctx->sym[i].line);
    my_free(_ALLOC_ID_, &xctx->sym[i].rect);
    my_free(_ALLOC_ID_, &xctx->sym[i].arc);
    my_free(_ALLOC_ID_, &xctx->sym[i].poly);
    my_free(_ALLOC_ID_, &xctx->sym[i].lines);
    my_free(_ALLOC_ID_, &xctx->sym[i].polygons);
    my_free(_ALLOC_ID_, &xctx->sym[i].arcs);
    my_free(_ALLOC_ID_, &xctx->sym[i].rects);
  }


  my_free(_ALLOC_ID_, &xctx->sym);


  my_strdup(_ALLOC_ID_, &xctx->schvhdlprop, xctx->uslot[slot].gptr);
  my_strdup(_ALLOC_ID_, &xctx->schverilogprop, xctx->uslot[slot].vptr);
  my_strdup(_ALLOC_ID_, &xctx->schprop, xctx->uslot[slot].sptr);
  my_strdup(_ALLOC_ID_, &xctx->schsymbolprop, xctx->uslot[slot].kptr);
  my_strdup(_ALLOC_ID_, &xctx->schtedaxprop, xctx->uslot[slot].eptr);

  for(c = 0;c<cadlayers; ++c) {
    /* lines */
    xctx->maxl[c] = xctx->lines[c] = xctx->uslot[slot].lines[c];
    xctx->line[c] = my_calloc(_ALLOC_ID_, xctx->lines[c], sizeof(xLine));
    for(i = 0;i<xctx->lines[c]; ++i) {
      xctx->line[c][i] = xctx->uslot[slot].lptr[c][i];
      xctx->line[c][i].prop_ptr = NULL;
      my_strdup(_ALLOC_ID_, &xctx->line[c][i].prop_ptr, xctx->uslot[slot].lptr[c][i].prop_ptr);
    }
    /* rects */
    xctx->maxr[c] = xctx->rects[c] = xctx->uslot[slot].rects[c];
    xctx->rect[c] = my_calloc(_ALLOC_ID_, xctx->rects[c], sizeof(xRect));
    for(i = 0;i<xctx->rects[c]; ++i) {
      xctx->rect[c][i] = xctx->uslot[slot].bptr[c][i];
      xctx->rect[c][i].prop_ptr = NULL;
      xctx->rect[c][i].extraptr = NULL;
      my_strdup(_ALLOC_ID_, &xctx->rect[c][i].prop_ptr, xctx->uslot[slot].bptr[c][i].prop_ptr);
    }
    /* arcs */
    xctx->maxa[c] = xctx->arcs[c] = xctx->uslot[slot].arcs[c];
    xctx->arc[c] = my_calloc(_ALLOC_ID_, xctx->arcs[c], sizeof(xArc));
    for(i = 0;i<xctx->arcs[c]; ++i) {
      xctx->arc[c][i] = xctx->uslot[slot].aptr[c][i];
      xctx->arc[c][i].prop_ptr = NULL;
      my_strdup(_ALLOC_ID_, &xctx->arc[c][i].prop_ptr, xctx->uslot[slot].aptr[c][i].prop_ptr);
    }
    /* polygons */
    xctx->maxp[c] = xctx->polygons[c] = xctx->uslot[slot].polygons[c];
    xctx->poly[c] = my_calloc(_ALLOC_ID_, xctx->polygons[c], sizeof(xPoly));
    for(i = 0;i<xctx->polygons[c]; ++i) {
      int points = xctx->uslot[slot].pptr[c][i].points;
      xctx->poly[c][i] = xctx->uslot[slot].pptr[c][i];
      xctx->poly[c][i].prop_ptr = NULL;
      my_strdup(_ALLOC_ID_, &xctx->poly[c][i].prop_ptr, xctx->uslot[slot].pptr[c][i].prop_ptr);
      xctx->poly[c][i].x = my_malloc(_ALLOC_ID_, points * sizeof(double));
      xctx->poly[c][i].y = my_malloc(_ALLOC_ID_, points * sizeof(double));
      xctx->poly[c][i].selected_point = my_malloc(_ALLOC_ID_, points * sizeof(unsigned short));
      memcpy(xctx->poly[c][i].x, xctx->uslot[slot].pptr[c][i].x, points * sizeof(double));
      memcpy(xctx->poly[c][i].y, xctx->uslot[slot].pptr[c][i].y, points * sizeof(double));
      memcpy(xctx->poly[c][i].selected_point, xctx->uslot[slot].pptr[c][i].selected_point, 
        points * sizeof(unsigned short));
    }
  }

  /* instances */
  xctx->maxi = xctx->instances = xctx->uslot[slot].instances;
  xctx->inst = my_calloc(_ALLOC_ID_, xctx->instances, sizeof(xInstance));
  for(i = 0;i<xctx->instances; ++i) {
    xctx->inst[i] = xctx->uslot[slot].iptr[i];
    xctx->inst[i].prop_ptr = NULL;
    xctx->inst[i].name = NULL;
    xctx->inst[i].instname = NULL;
    xctx->inst[i].lab = NULL;
    my_strdup2(_ALLOC_ID_, &xctx->inst[i].prop_ptr, xctx->uslot[slot].iptr[i].prop_ptr);
    my_strdup2(_ALLOC_ID_, &xctx->inst[i].name, xctx->uslot[slot].iptr[i].name);
    my_strdup2(_ALLOC_ID_, &xctx->inst[i].instname, xctx->uslot[slot].iptr[i].instname);
    my_strdup2(_ALLOC_ID_, &xctx->inst[i].lab, xctx->uslot[slot].iptr[i].lab);
  }

  /* symbols */
  xctx->maxs = xctx->symbols = xctx->uslot[slot].symbols;
  xctx->sym = my_calloc(_ALLOC_ID_, xctx->symbols, sizeof(xSymbol));

  for(i = 0;i<xctx->symbols; ++i) {
    copy_symbol(&xctx->sym[i], &xctx->uslot[slot].symptr[i]);
  }

  /* texts */
  xctx->maxt = xctx->texts = xctx->uslot[slot].texts;
  xctx->text = my_calloc(_ALLOC_ID_, xctx->texts, sizeof(xText));
  for(i = 0;i<xctx->texts; ++i) {
    xctx->text[i] = xctx->uslot[slot].tptr[i];
    xctx->text[i].txt_ptr = NULL;
    xctx->text[i].font = NULL;
    xctx->text[i].floater_instname = NULL;
    xctx->text[i].floater_ptr = NULL;
    xctx->text[i].prop_ptr = NULL;
    my_strdup2(_ALLOC_ID_, &xctx->text[i].prop_ptr, xctx->uslot[slot].tptr[i].prop_ptr);
    my_strdup2(_ALLOC_ID_, &xctx->text[i].txt_ptr, xctx->uslot[slot].tptr[i].txt_ptr);
    my_strdup2(_ALLOC_ID_, &xctx->text[i].font, xctx->uslot[slot].tptr[i].font);
    my_strdup2(_ALLOC_ID_, &xctx->text[i].floater_instname, xctx->uslot[slot].tptr[i].floater_instname);
    my_strdup2(_ALLOC_ID_, &xctx->text[i].floater_ptr, xctx->uslot[slot].tptr[i].floater_ptr);
  }

  /* wires */
  xctx->maxw = xctx->wires = xctx->uslot[slot].wires;
  xctx->wire = my_calloc(_ALLOC_ID_, xctx->wires, sizeof(xWire));
  for(i = 0;i<xctx->wires; ++i) {
    xctx->wire[i] = xctx->uslot[slot].wptr[i];
    xctx->wire[i].prop_ptr = NULL;
    xctx->wire[i].node = NULL;
    my_strdup(_ALLOC_ID_, &xctx->wire[i].prop_ptr, xctx->uslot[slot].wptr[i].prop_ptr);
  }
  /* unnecessary since in_memory_undo saves all symbols */
  /* link_symbols_to_instances(-1); */
  if(redo == 2) xctx->cur_undo_ptr++; /* restore undo stack pointer */
  if(set_modify_status) set_modify(1);
  xctx->prep_hash_inst = 0;
  xctx->prep_hash_wires = 0;
  xctx->prep_net_structs = 0;
  xctx->prep_hi_structs = 0;
  update_conn_cues(WIRELAYER, 0, 0);
  int_hash_free(&xctx->floater_inst_table);
}
