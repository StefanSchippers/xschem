/* File: in_memory_undo.c
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2026 Stefan Frederik Schippers
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

static void free_undo_lines(Undo_slot *uslot, int slot)
{
  int i, c;
  if(uslot[slot].lines) for(c = 0;c<cadlayers; ++c) {
    for(i = 0;i<uslot[slot].lines[c]; ++i) {
      my_free(_ALLOC_ID_, &uslot[slot].lptr[c][i].prop_ptr);
    }
    my_free(_ALLOC_ID_, &uslot[slot].lptr[c]);
    uslot[slot].lines[c] = 0;
  }
}

static void free_undo_rects(Undo_slot *uslot, int slot)
{
  int i, c;

  if(uslot[slot].rects) for(c = 0;c<cadlayers; ++c) {
    for(i = 0;i<uslot[slot].rects[c]; ++i) {
      my_free(_ALLOC_ID_, &uslot[slot].bptr[c][i].prop_ptr);
    }
    my_free(_ALLOC_ID_, &uslot[slot].bptr[c]);
    uslot[slot].rects[c] = 0;
  }
}

static void free_undo_polygons(Undo_slot *uslot, int slot)
{
  int i, c;

  if(uslot[slot].polygons) for(c = 0;c<cadlayers; ++c) {
    for(i = 0;i<uslot[slot].polygons[c]; ++i) {
      my_free(_ALLOC_ID_, &uslot[slot].pptr[c][i].prop_ptr);
      my_free(_ALLOC_ID_, &uslot[slot].pptr[c][i].x);
      my_free(_ALLOC_ID_, &uslot[slot].pptr[c][i].y);
      my_free(_ALLOC_ID_, &uslot[slot].pptr[c][i].selected_point);
    }
    my_free(_ALLOC_ID_, &uslot[slot].pptr[c]);
    uslot[slot].polygons[c] = 0;
  }
}

static void free_undo_arcs(Undo_slot *uslot, int slot)
{
  int i, c;

  if(uslot[slot].arcs) for(c = 0;c<cadlayers; ++c) {
    for(i = 0;i<uslot[slot].arcs[c]; ++i) {
      my_free(_ALLOC_ID_, &uslot[slot].aptr[c][i].prop_ptr);
    }
    my_free(_ALLOC_ID_, &uslot[slot].aptr[c]);
    uslot[slot].arcs[c] = 0;
  }
}

static void free_undo_wires(Undo_slot *uslot, int slot)
{
  int i;

  for(i = 0;i<uslot[slot].wires; ++i) {
    my_free(_ALLOC_ID_, &uslot[slot].wptr[i].prop_ptr);
  }
  my_free(_ALLOC_ID_, &uslot[slot].wptr);
  uslot[slot].wires = 0;
}

static void free_undo_texts(Undo_slot *uslot, int slot)
{
  int i;

  for(i = 0;i<uslot[slot].texts; ++i) {
    my_free(_ALLOC_ID_, &uslot[slot].tptr[i].prop_ptr);
    my_free(_ALLOC_ID_, &uslot[slot].tptr[i].txt_ptr);
    my_free(_ALLOC_ID_, &uslot[slot].tptr[i].font);
    my_free(_ALLOC_ID_, &uslot[slot].tptr[i].floater_instname);
    my_free(_ALLOC_ID_, &uslot[slot].tptr[i].floater_ptr);
  }
  my_free(_ALLOC_ID_, &uslot[slot].tptr);
  uslot[slot].texts = 0;
}

static void free_undo_instances(Undo_slot *uslot, int slot)
{
  int i;

  for(i = 0;i<uslot[slot].instances; ++i) {
    my_free(_ALLOC_ID_, &uslot[slot].iptr[i].name);
    my_free(_ALLOC_ID_, &uslot[slot].iptr[i].prop_ptr);
    my_free(_ALLOC_ID_, &uslot[slot].iptr[i].instname);
    my_free(_ALLOC_ID_, &uslot[slot].iptr[i].lab);
  }
  my_free(_ALLOC_ID_, &uslot[slot].iptr);
  uslot[slot].instances = 0;
}


static void free_undo_symbol(xSymbol *sym)
{
    int j, c;
    my_free(_ALLOC_ID_, &sym->name);
    my_free(_ALLOC_ID_, &sym->prop_ptr);
    my_free(_ALLOC_ID_, &sym->type);
    my_free(_ALLOC_ID_, &sym->templ);
    my_free(_ALLOC_ID_, &sym->parent_prop_ptr);

    for(c = 0;c < cadlayers; ++c) {
      for(j = 0;j < sym->polygons[c]; ++j) {
        if(sym->poly[c][j].prop_ptr != NULL) {
          my_free(_ALLOC_ID_, &sym->poly[c][j].prop_ptr);
        }
        my_free(_ALLOC_ID_, &sym->poly[c][j].x);
        my_free(_ALLOC_ID_, &sym->poly[c][j].y);
        my_free(_ALLOC_ID_, &sym->poly[c][j].selected_point);
      }
      my_free(_ALLOC_ID_, &sym->poly[c]);
      sym->polygons[c] = 0;

      for(j = 0;j < sym->lines[c]; ++j) {
        if(sym->line[c][j].prop_ptr != NULL) {
          my_free(_ALLOC_ID_, &sym->line[c][j].prop_ptr);
        }
      }
      my_free(_ALLOC_ID_, &sym->line[c]);
      sym->lines[c] = 0;

      for(j = 0;j < sym->arcs[c]; ++j) {
        if(sym->arc[c][j].prop_ptr != NULL) {
          my_free(_ALLOC_ID_, &sym->arc[c][j].prop_ptr);
        }
      }
      my_free(_ALLOC_ID_, &sym->arc[c]);
      sym->arcs[c] = 0;

      for(j = 0;j < sym->rects[c]; ++j) {
        if(sym->rect[c][j].prop_ptr != NULL) {
          my_free(_ALLOC_ID_, &sym->rect[c][j].prop_ptr);
        }
      }
      my_free(_ALLOC_ID_, &sym->rect[c]);
      sym->rects[c] = 0;
    }
    for(j = 0;j < sym->texts; ++j) {
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

static void free_undo_symbols(Undo_slot *uslot, int slot)
{
  int i;
  int symbols = uslot[slot].symbols;

  for(i = 0; i < symbols; ++i) {
    xSymbol *sym = &uslot[slot].symptr[i];
    free_undo_symbol(sym);
  }
  my_free(_ALLOC_ID_, &uslot[slot].symptr);
  uslot[slot].symbols = 0;
}

static void mem_init_undo(void)
{
  int slot;

  /* all the uslot[] items are set to 0 since xctx is 'calloc'ated.
   * no need to zero xctx->uslot[slot].gptr, ..., xctx->uslot[slot].version_string,
   * xctx->uslot[slot].symptr, ...
   */
  if(xctx->no_undo == 0 && !xctx->undo_initialized) {
    xctx->uslot = my_calloc(_ALLOC_ID_, MAX_UNDO, sizeof(Undo_slot));
    dbg(1, ("mem_init_undo(): xctx->uslot=%p, size=%ld, %s\n",
        xctx->uslot, MAX_UNDO * sizeof(Undo_slot), xctx->current_name));
    /* delete previous if any */
    if(tclgetboolvar("preserve_undo")) {
      save_undo_table(xctx->current_name, NULL, XDELETE);
    }
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
    xctx->cur_undo_ptr = 0; 
    xctx->head_undo_ptr = 0; 
    xctx->tail_undo_ptr = 0; 
    /* create entry in undo save table */
    if(tclgetboolvar("preserve_undo")) {
      save_undo_table(xctx->current_name, NULL, XINSERT);
    }
  }
}

/* called when program resets undo stack (for example when loading a new file */
void mem_clear_undo(void)
{
  dbg(1, ("mem_clear_undo(): undo_initialized = %d\n", xctx->undo_initialized));
  xctx->cur_undo_ptr = 0;
  xctx->tail_undo_ptr = 0;
  xctx->head_undo_ptr = 0;
}

void mem_free_uslots(Undo_slot *uslot)
{
  int slot;
  dbg(1, ("mem_free_uslots(): freeing uslot=%p\n", uslot));
  for(slot = 0;slot<MAX_UNDO; slot++) {
    free_undo_lines(uslot, slot);
    free_undo_rects(uslot, slot);
    free_undo_polygons(uslot, slot);
    free_undo_arcs(uslot, slot);
    free_undo_wires(uslot, slot);
    free_undo_texts(uslot, slot);
    free_undo_instances(uslot, slot);
    free_undo_symbols(uslot, slot);

    my_free(_ALLOC_ID_, &uslot[slot].lines);
    my_free(_ALLOC_ID_, &uslot[slot].rects);
    my_free(_ALLOC_ID_, &uslot[slot].arcs);
    my_free(_ALLOC_ID_, &uslot[slot].polygons);
    my_free(_ALLOC_ID_, &uslot[slot].lptr);
    my_free(_ALLOC_ID_, &uslot[slot].bptr);
    my_free(_ALLOC_ID_, &uslot[slot].aptr);
    my_free(_ALLOC_ID_, &uslot[slot].pptr);
    my_free(_ALLOC_ID_, &uslot[slot].sptr);
    my_free(_ALLOC_ID_, &uslot[slot].vptr);
    my_free(_ALLOC_ID_, &uslot[slot].gptr);
    my_free(_ALLOC_ID_, &uslot[slot].eptr);
    my_free(_ALLOC_ID_, &uslot[slot].fptr);
    my_free(_ALLOC_ID_, &uslot[slot].kptr);
    my_free(_ALLOC_ID_, &uslot[slot].header_text);
    my_free(_ALLOC_ID_, &uslot[slot].version_string);
  }
}

/* used to delete everything when program exits */
void mem_delete_undo(void)
{
  if(!xctx->undo_initialized) goto done;
  dbg(1, ("mem_delete_undo(): undo_initialized = %d, %s\n", xctx->undo_initialized, xctx->current_name));

  if(tclgetboolvar("preserve_undo")) {
    /* update undo pointers in save table */
    save_undo_table(xctx->current_name, NULL, XINSERT);
  }

  mem_clear_undo();
  xctx->undo_initialized = 0;

  if(!tclgetboolvar("preserve_undo")) {
    mem_free_uslots(xctx->uslot);
    my_free(_ALLOC_ID_, &xctx->uslot);
  }
  done:
  xctx->uslot = NULL;

}

/* save state, then increment cur_undo_ptr
 * what: 
 *   0: normal push, put current state into cur_undo_ptr slot, then
 *      increment cur_undo_ptr and head_undo_ptr.
 *   1: as above but do not increment cur_undo_ptr. This is equivalent to a 
 *      push_undo(0) followed by a pop_undo(4)
 *   2: push state into undo reserved slot 0
 *   6: push state into undo reserved slot 1
 */
void mem_push_undo(int what)
{
  int i, c;
  int undo_index;

  dbg(1, ("mem_push_undo(%d), %s\n", what, xctx->current_name));
  undo_index = xctx->cur_undo_ptr % (MAX_UNDO - 2) + 2;
  if(what == 2) undo_index = 0;
  else if(what == 6) undo_index = 1;

  if(xctx->no_undo)return;
  mem_init_undo();

  my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].gptr, xctx->schvhdlprop);
  my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].vptr, xctx->schverilogprop);
  my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].sptr, xctx->schprop);
  my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].fptr, xctx->schspectreprop);
  my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].kptr, xctx->schsymbolprop);
  my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].eptr, xctx->schtedaxprop);
  my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].header_text, xctx->header_text);
  my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].version_string, xctx->version_string);

  free_undo_lines(xctx->uslot, undo_index);
  free_undo_rects(xctx->uslot, undo_index);
  free_undo_polygons(xctx->uslot, undo_index);
  free_undo_arcs(xctx->uslot, undo_index);
  free_undo_wires(xctx->uslot, undo_index);
  free_undo_texts(xctx->uslot, undo_index);
  free_undo_instances(xctx->uslot, undo_index);
  free_undo_symbols(xctx->uslot, undo_index);

  memcpy(xctx->uslot[undo_index].lines, xctx->lines, sizeof(xctx->lines[0]) * cadlayers);
  memcpy(xctx->uslot[undo_index].rects, xctx->rects, sizeof(xctx->rects[0]) * cadlayers);
  memcpy(xctx->uslot[undo_index].arcs, xctx->arcs, sizeof(xctx->arcs[0]) * cadlayers);
  memcpy(xctx->uslot[undo_index].polygons, xctx->polygons, sizeof(xctx->polygons[0]) * cadlayers);
  for(c = 0;c<cadlayers; ++c) {
    xctx->uslot[undo_index].lptr[c] = my_calloc(_ALLOC_ID_, xctx->lines[c], sizeof(xLine));
    xctx->uslot[undo_index].bptr[c] = my_calloc(_ALLOC_ID_, xctx->rects[c], sizeof(xRect));
    xctx->uslot[undo_index].pptr[c] = my_calloc(_ALLOC_ID_, xctx->polygons[c], sizeof(xPoly));
    xctx->uslot[undo_index].aptr[c] = my_calloc(_ALLOC_ID_, xctx->arcs[c], sizeof(xArc));
  }
  xctx->uslot[undo_index].wptr = my_calloc(_ALLOC_ID_, xctx->wires, sizeof(xWire));
  xctx->uslot[undo_index].tptr = my_calloc(_ALLOC_ID_, xctx->texts, sizeof(xText));
  xctx->uslot[undo_index].iptr = my_calloc(_ALLOC_ID_, xctx->instances, sizeof(xInstance));
  xctx->uslot[undo_index].symptr = my_calloc(_ALLOC_ID_, xctx->symbols, sizeof(xSymbol));
  xctx->uslot[undo_index].texts = xctx->texts;
  xctx->uslot[undo_index].instances = xctx->instances;
  xctx->uslot[undo_index].symbols = xctx->symbols;
  xctx->uslot[undo_index].wires = xctx->wires;

  for(c = 0;c<cadlayers; ++c) {
    /* lines */
    for(i = 0;i<xctx->lines[c]; ++i) {
      xctx->uslot[undo_index].lptr[c][i] = xctx->line[c][i];
      xctx->uslot[undo_index].lptr[c][i].sel = 0; /* we don't want to store select status */
      xctx->uslot[undo_index].lptr[c][i].prop_ptr = NULL;
      my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].lptr[c][i].prop_ptr, xctx->line[c][i].prop_ptr);
    }
    /* rects */
    for(i = 0;i<xctx->rects[c]; ++i) {
      xctx->uslot[undo_index].bptr[c][i] = xctx->rect[c][i];
      xctx->uslot[undo_index].bptr[c][i].sel = 0; /* we don't want to store select status */
      xctx->uslot[undo_index].bptr[c][i].prop_ptr = NULL;
      xctx->uslot[undo_index].bptr[c][i].extraptr = NULL;
      my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].bptr[c][i].prop_ptr, xctx->rect[c][i].prop_ptr);
    }
    /* arcs */
    for(i = 0;i<xctx->arcs[c]; ++i) {
      xctx->uslot[undo_index].aptr[c][i] = xctx->arc[c][i];
      xctx->uslot[undo_index].aptr[c][i].sel = 0; /* we don't want to store select status */
      xctx->uslot[undo_index].aptr[c][i].prop_ptr = NULL;
      my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].aptr[c][i].prop_ptr, xctx->arc[c][i].prop_ptr);
    }
    /*polygons */
    for(i = 0;i<xctx->polygons[c]; ++i) {
      int points = xctx->poly[c][i].points;
      xctx->uslot[undo_index].pptr[c][i] = xctx->poly[c][i];
      xctx->uslot[undo_index].pptr[c][i].sel = 0; /* we don't want to store select status */
      xctx->uslot[undo_index].pptr[c][i].prop_ptr = NULL;
      xctx->uslot[undo_index].pptr[c][i].x = my_malloc(_ALLOC_ID_, points * sizeof(double));
      xctx->uslot[undo_index].pptr[c][i].y = my_malloc(_ALLOC_ID_, points * sizeof(double));
      xctx->uslot[undo_index].pptr[c][i].selected_point = my_malloc(_ALLOC_ID_, points * sizeof(unsigned short));
      memcpy(xctx->uslot[undo_index].pptr[c][i].x, xctx->poly[c][i].x, points * sizeof(double));
      memcpy(xctx->uslot[undo_index].pptr[c][i].y, xctx->poly[c][i].y, points * sizeof(double));
      memcpy(xctx->uslot[undo_index].pptr[c][i].selected_point, xctx->poly[c][i].selected_point,
        points * sizeof(unsigned short));
      my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].pptr[c][i].prop_ptr, xctx->poly[c][i].prop_ptr);
    }
  }
  /* instances */
  for(i = 0;i<xctx->instances; ++i) {
    xctx->uslot[undo_index].iptr[i] = xctx->inst[i];
    xctx->uslot[undo_index].iptr[i].sel = 0; /* we don't want to store select status */
    xctx->uslot[undo_index].iptr[i].prop_ptr = NULL;
    xctx->uslot[undo_index].iptr[i].name = NULL;
    xctx->uslot[undo_index].iptr[i].instname = NULL;
    xctx->uslot[undo_index].iptr[i].lab = NULL;
    xctx->uslot[undo_index].iptr[i].node = NULL;
    my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].iptr[i].lab, xctx->inst[i].lab);
    my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].iptr[i].instname, xctx->inst[i].instname);
    my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].iptr[i].prop_ptr, xctx->inst[i].prop_ptr);
    my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].iptr[i].name, xctx->inst[i].name);
  }

  /* symbols */
  for(i = 0;i<xctx->symbols; ++i) {
    copy_symbol(i, &xctx->uslot[undo_index].symptr[i], &xctx->sym[i]);
  }
  /* texts */
  for(i = 0;i<xctx->texts; ++i) {
    xctx->uslot[undo_index].tptr[i] = xctx->text[i];
    xctx->uslot[undo_index].tptr[i].sel = 0; /* we don't want to store select status */
    xctx->uslot[undo_index].tptr[i].prop_ptr = NULL;
    xctx->uslot[undo_index].tptr[i].txt_ptr = NULL;
    xctx->uslot[undo_index].tptr[i].font = NULL;
    xctx->uslot[undo_index].tptr[i].floater_instname = NULL;
    xctx->uslot[undo_index].tptr[i].floater_ptr = NULL;
    my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].tptr[i].prop_ptr, xctx->text[i].prop_ptr);
    my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].tptr[i].txt_ptr, xctx->text[i].txt_ptr);
    my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].tptr[i].font, xctx->text[i].font);
    my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].tptr[i].floater_instname, xctx->text[i].floater_instname);
    my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].tptr[i].floater_ptr, xctx->text[i].floater_ptr);
  }

  /* wires */
  for(i = 0;i<xctx->wires; ++i) {
    xctx->uslot[undo_index].wptr[i] = xctx->wire[i];
    xctx->uslot[undo_index].wptr[i].sel = 0; /* we don't want to store select status */
    xctx->uslot[undo_index].wptr[i].prop_ptr = NULL;
    xctx->uslot[undo_index].wptr[i].node = NULL;
    my_strdup2(_ALLOC_ID_, &xctx->uslot[undo_index].wptr[i].prop_ptr, xctx->wire[i].prop_ptr);
  }

  if(what == 1 || what == 0) xctx->head_undo_ptr = xctx->cur_undo_ptr + 1;
  if(what == 0) xctx->cur_undo_ptr++;
  xctx->tail_undo_ptr = xctx->head_undo_ptr <= (MAX_UNDO - 2) ? 0: xctx->head_undo_ptr - (MAX_UNDO - 2);
  if(tclgetboolvar("preserve_undo")) {
    /* update head, cur, tail pointers */
    save_undo_table(xctx->current_name, NULL, XINSERT);
  }
}

/* decrement cur_undo_ptr, then fetch state
 * redo:
 * 0: undo (with push current state for allowing following redo)
 *    -> decrement cur_undo_ptr, then fetch state
 * 4: undo, do not push state for redo
 * 1: redo, increment cur_undo_ptr, then fetch state
 * 5: read data from undo stack at cur_undo_ptr - 1 without changing undo stack
 * 3: read slot pointed by cur_undo_ptr, do not change any *_undo_ptr
 * 2: read reserved slot 0, do not change any *_undo_ptr (for preserve_undo)
 * 6: read reserved slot 1, do not change any *_undo_ptr (for netlisting)
 * 
 */
void mem_pop_undo(int redo, int set_modify_status)
{
  int i, c;
  int undo_index;

  if(xctx->no_undo) return;
  if(!xctx->undo_initialized) return;

  dbg(1, ("mem_pop_undo(%d, %d), %s\n", redo, set_modify_status, xctx->current_name));
  if(redo == 1) {
    if(xctx->cur_undo_ptr < xctx->head_undo_ptr - 1) {
      xctx->cur_undo_ptr++;
    } else {
      return;
    }

  } else if(redo == 0 || redo == 4) {  /* undo */
    if(xctx->cur_undo_ptr <= xctx->tail_undo_ptr) return; /* check undo tail */
    if(redo == 0 && xctx->head_undo_ptr == xctx->cur_undo_ptr) {
      xctx->push_undo(0);
      xctx->cur_undo_ptr--;
    }
    xctx->cur_undo_ptr--;

  } else if(redo == 5) { /* get data at cur_undo_ptr - 1 without changing undo stack */
    if(xctx->cur_undo_ptr<= xctx->tail_undo_ptr) return; /* check undo tail */
    xctx->cur_undo_ptr--; /* will be restored after building file name and fetching data*/

  } else if(redo == 2) { /* read reserved slot 0, no change any *undo_ptr */
    /* set undo_index later */

  } else if(redo == 6) { /* read reserved slot 1, no change any *undo_ptr */
    /* set undo_index later */

  } else if(redo == 3) { /* read slot pointed by cur_undo_ptr, do not change any *_undo_ptr */
    if(xctx->head_undo_ptr <= xctx->cur_undo_ptr) return; /* nothing present */
    /* do nothing: not moving *_undo_ptr pointers */

  } else {
    info("mem_pop_undo(): wrong argument: %d, doing nothing\n", redo);
    return;
  }

  clear_drawing();
  unselect_all(1);
  undo_index = xctx->cur_undo_ptr % (MAX_UNDO - 2) + 2;
  if(redo == 2) {
    undo_index = 0;
  }
  else if(redo == 6) {
    undo_index = 1;
  }

  my_free(_ALLOC_ID_, &xctx->wire);
  my_free(_ALLOC_ID_, &xctx->text);
  my_free(_ALLOC_ID_, &xctx->inst);

  for(i = 0;i<cadlayers; ++i) {
    my_free(_ALLOC_ID_, &xctx->rect[i]);
    my_free(_ALLOC_ID_, &xctx->line[i]);
    my_free(_ALLOC_ID_, &xctx->poly[i]);
    my_free(_ALLOC_ID_, &xctx->arc[i]);
  }


  /* if netlisting and preserve_undo is set we need to keep symbols so we netlist them all
   * when descending with descend_schematic() symbols are cleared there */
  #if 0
  if(!tclgetboolvar("keep_symbols")) {
    remove_symbols();
    my_free(_ALLOC_ID_, &xctx->sym);
    xctx->maxs = xctx->symbols = 0;
  }
  #endif

  my_strdup2(_ALLOC_ID_, &xctx->schvhdlprop, xctx->uslot[undo_index].gptr);
  my_strdup2(_ALLOC_ID_, &xctx->schverilogprop, xctx->uslot[undo_index].vptr);
  my_strdup2(_ALLOC_ID_, &xctx->schspectreprop, xctx->uslot[undo_index].fptr);
  my_strdup2(_ALLOC_ID_, &xctx->schprop, xctx->uslot[undo_index].sptr);
  my_strdup2(_ALLOC_ID_, &xctx->schsymbolprop, xctx->uslot[undo_index].kptr);
  my_strdup2(_ALLOC_ID_, &xctx->schtedaxprop, xctx->uslot[undo_index].eptr);
  my_strdup2(_ALLOC_ID_, &xctx->header_text, xctx->uslot[undo_index].header_text);
  my_strdup2(_ALLOC_ID_, &xctx->version_string, xctx->uslot[undo_index].version_string);

  for(c = 0;c<cadlayers; ++c) {
    /* lines */
    xctx->maxl[c] = xctx->lines[c] = xctx->uslot[undo_index].lines[c];
    xctx->line[c] = my_calloc(_ALLOC_ID_, xctx->lines[c], sizeof(xLine));
    for(i = 0;i<xctx->lines[c]; ++i) {
      xctx->line[c][i] = xctx->uslot[undo_index].lptr[c][i];
      xctx->line[c][i].prop_ptr = NULL;
      my_strdup2(_ALLOC_ID_, &xctx->line[c][i].prop_ptr, xctx->uslot[undo_index].lptr[c][i].prop_ptr);
    }
    /* rects */
    xctx->maxr[c] = xctx->rects[c] = xctx->uslot[undo_index].rects[c];
    xctx->rect[c] = my_calloc(_ALLOC_ID_, xctx->rects[c], sizeof(xRect));
    for(i = 0;i<xctx->rects[c]; ++i) {
      xctx->rect[c][i] = xctx->uslot[undo_index].bptr[c][i];
      xctx->rect[c][i].prop_ptr = NULL;
      xctx->rect[c][i].extraptr = NULL;
      my_strdup2(_ALLOC_ID_, &xctx->rect[c][i].prop_ptr, xctx->uslot[undo_index].bptr[c][i].prop_ptr);
    }
    /* arcs */
    xctx->maxa[c] = xctx->arcs[c] = xctx->uslot[undo_index].arcs[c];
    xctx->arc[c] = my_calloc(_ALLOC_ID_, xctx->arcs[c], sizeof(xArc));
    for(i = 0;i<xctx->arcs[c]; ++i) {
      xctx->arc[c][i] = xctx->uslot[undo_index].aptr[c][i];
      xctx->arc[c][i].prop_ptr = NULL;
      my_strdup2(_ALLOC_ID_, &xctx->arc[c][i].prop_ptr, xctx->uslot[undo_index].aptr[c][i].prop_ptr);
    }
    /* polygons */
    xctx->maxp[c] = xctx->polygons[c] = xctx->uslot[undo_index].polygons[c];
    xctx->poly[c] = my_calloc(_ALLOC_ID_, xctx->polygons[c], sizeof(xPoly));
    for(i = 0;i<xctx->polygons[c]; ++i) {
      int points = xctx->uslot[undo_index].pptr[c][i].points;
      xctx->poly[c][i] = xctx->uslot[undo_index].pptr[c][i];
      xctx->poly[c][i].prop_ptr = NULL;
      my_strdup2(_ALLOC_ID_, &xctx->poly[c][i].prop_ptr, xctx->uslot[undo_index].pptr[c][i].prop_ptr);
      xctx->poly[c][i].x = my_malloc(_ALLOC_ID_, points * sizeof(double));
      xctx->poly[c][i].y = my_malloc(_ALLOC_ID_, points * sizeof(double));
      xctx->poly[c][i].selected_point = my_malloc(_ALLOC_ID_, points * sizeof(unsigned short));
      memcpy(xctx->poly[c][i].x, xctx->uslot[undo_index].pptr[c][i].x, points * sizeof(double));
      memcpy(xctx->poly[c][i].y, xctx->uslot[undo_index].pptr[c][i].y, points * sizeof(double));
      memcpy(xctx->poly[c][i].selected_point, xctx->uslot[undo_index].pptr[c][i].selected_point,
        points * sizeof(unsigned short));
    }
  }

  /* symbols */
  dbg(1, ("mem_pop_undo(): uslot[%d].symbols=%d\n", undo_index, xctx->uslot[undo_index].symbols));
  dbg(1, ("mem_pop_undo(): xctx->symbols=%d\n", xctx->symbols));
  dbg(1, ("mem_pop_undo(): xctx->sym=%p\n", xctx->sym));
  dbg(1, ("mem_pop_undo(): xctx->instances=%d\n", xctx->instances));
  if(!xctx->sym) { /* all symbols were deleted... */
    xctx->maxs = xctx->symbols = xctx->uslot[undo_index].symbols;
    xctx->sym = my_calloc(_ALLOC_ID_, xctx->symbols, sizeof(xSymbol));
    for(i = 0; i < xctx->uslot[undo_index].symbols; ++i) {
        dbg(1, ("mem_pop_undo(): loading symbol: %d <-- %s\n",
               i, xctx->uslot[undo_index].symptr[i].name));
      copy_symbol(i, &xctx->sym[i], &xctx->uslot[undo_index].symptr[i]);
    }
  } else {
    Int_hashtable sym_table = {NULL, 0};
    int_hash_init(&sym_table, 367);

    for(i = 0; i < xctx->symbols; ++i) {
      dbg(1, ("mem_pop_undo(): hashing symbol: %s\n", xctx->sym[i].name));
      int_hash_lookup(&sym_table, xctx->sym[i].name, i, XINSERT);
    }

    for(i = 0; i < xctx->uslot[undo_index].symbols; ++i) {
      const char *sym = xctx->uslot[undo_index].symptr[i].name;
      Int_hashentry *entry;
      dbg(1, ("mem_pop_undo(): see if %s must be removed\n", sym));
      if((entry = int_hash_lookup(&sym_table, sym, 0, XLOOKUP))) {
        dbg(1, ("mem_pop_undo(): deleting symbol %s\n", entry->token));
        remove_symbol(entry->value, 0); /* shift param set to 0. Leave all other symbols
                                         * in place, do not shift back */
        copy_symbol(entry->value, &xctx->sym[entry->value], &xctx->uslot[undo_index].symptr[i]);
      }
    }
    int_hash_free(&sym_table);
  }

  /* instances */
  xctx->maxi = xctx->instances = xctx->uslot[undo_index].instances;
  xctx->inst = my_calloc(_ALLOC_ID_, xctx->instances, sizeof(xInstance));
  for(i = 0;i<xctx->instances; ++i) {
    xctx->inst[i] = xctx->uslot[undo_index].iptr[i];
    xctx->inst[i].prop_ptr = NULL;
    xctx->inst[i].name = NULL;
    xctx->inst[i].instname = NULL;
    xctx->inst[i].lab = NULL;
    xctx->inst[i].node = NULL;
    my_strdup2(_ALLOC_ID_, &xctx->inst[i].prop_ptr, xctx->uslot[undo_index].iptr[i].prop_ptr);
    my_strdup2(_ALLOC_ID_, &xctx->inst[i].name, xctx->uslot[undo_index].iptr[i].name);
    my_strdup2(_ALLOC_ID_, &xctx->inst[i].instname, xctx->uslot[undo_index].iptr[i].instname);
    my_strdup2(_ALLOC_ID_, &xctx->inst[i].lab, xctx->uslot[undo_index].iptr[i].lab);
    set_inst_flags(&xctx->inst[i]);
  }

  /* texts */
  xctx->maxt = xctx->texts = xctx->uslot[undo_index].texts;
  xctx->text = my_calloc(_ALLOC_ID_, xctx->texts, sizeof(xText));
  for(i = 0;i<xctx->texts; ++i) {
    xctx->text[i] = xctx->uslot[undo_index].tptr[i];
    xctx->text[i].txt_ptr = NULL;
    xctx->text[i].font = NULL;
    xctx->text[i].floater_instname = NULL;
    xctx->text[i].floater_ptr = NULL;
    xctx->text[i].prop_ptr = NULL;
    my_strdup2(_ALLOC_ID_, &xctx->text[i].prop_ptr, xctx->uslot[undo_index].tptr[i].prop_ptr);
    my_strdup2(_ALLOC_ID_, &xctx->text[i].txt_ptr, xctx->uslot[undo_index].tptr[i].txt_ptr);
    my_strdup2(_ALLOC_ID_, &xctx->text[i].font, xctx->uslot[undo_index].tptr[i].font);
    my_strdup2(_ALLOC_ID_, &xctx->text[i].floater_instname, xctx->uslot[undo_index].tptr[i].floater_instname);
    my_strdup2(_ALLOC_ID_, &xctx->text[i].floater_ptr, xctx->uslot[undo_index].tptr[i].floater_ptr);
  }

  /* wires */
  xctx->maxw = xctx->wires = xctx->uslot[undo_index].wires;
  xctx->wire = my_calloc(_ALLOC_ID_, xctx->wires, sizeof(xWire));
  for(i = 0;i<xctx->wires; ++i) {
    xctx->wire[i] = xctx->uslot[undo_index].wptr[i];
    xctx->wire[i].prop_ptr = NULL;
    xctx->wire[i].node = NULL;
    my_strdup2(_ALLOC_ID_, &xctx->wire[i].prop_ptr, xctx->uslot[undo_index].wptr[i].prop_ptr);
    set_wire_flags(&xctx->wire[i]);
  }
  /* necessary, because symbols, although loaded, may be in a different order */
  link_symbols_to_instances(-1);
  if(redo == 5) xctx->cur_undo_ptr++; /* restore undo stack pointer */

  if(set_modify_status) {
    if(xctx->cur_undo_ptr == xctx->saved_cur_undo_ptr) {
      set_modify(0);
    } else {
      set_modify(1);
    }
  }

  xctx->prep_hash_inst = 0;
  xctx->prep_hash_wires = 0;
  xctx->prep_net_structs = 0;
  xctx->prep_hi_structs = 0;
  update_conn_cues(WIRELAYER, 0, 0);
  if(xctx->hilight_nets) {
    propagate_hilights(1, 1, XINSERT_NOREPLACE);
  }
  int_hash_free(&xctx->floater_inst_table); /* disk undo does this in read_xschem_file() */
  if(tclgetboolvar("preserve_undo")) {
    if(redo != 5 && redo != 2 && redo != 6) {
      /* update head, cur, tail pointers */
      save_undo_table(xctx->current_name, NULL, XINSERT);
    }
  }

}

void mem_copy_undo(Xschem_ctx *dest, Xschem_ctx *source)
{
  int i, c, undo_index;



  dest->uslot = my_calloc(_ALLOC_ID_, MAX_UNDO, sizeof(Undo_slot));

  for(undo_index = 0; undo_index < MAX_UNDO; undo_index++) {
    my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].gptr,
                          source->uslot[undo_index].gptr);
    my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].vptr,
                          source->uslot[undo_index].vptr);
    my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].sptr,
                          source->uslot[undo_index].sptr);
    my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].fptr,
                          source->uslot[undo_index].fptr);
    my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].kptr,
                          source->uslot[undo_index].kptr);
    my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].eptr,
                          source->uslot[undo_index].eptr);
    my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].header_text,
                          source->uslot[undo_index].header_text);
    my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].version_string,
                          source->uslot[undo_index].version_string);
  
    #if 0
    free_undo_lines(dest->uslot, undo_index);
    free_undo_rects(dest->uslot, undo_index);
    free_undo_polygons(dest->uslot, undo_index);
    free_undo_arcs(dest->uslot, undo_index);
    free_undo_wires(dest->uslot, undo_index);
    free_undo_texts(dest->uslot, undo_index);
    free_undo_instances(dest->uslot, undo_index);
    free_undo_symbols(dest->uslot, undo_index);
    #endif
  
    dest->uslot[undo_index].lines = my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
    dest->uslot[undo_index].rects = my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
    dest->uslot[undo_index].arcs = my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
    dest->uslot[undo_index].polygons = my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
    dest->uslot[undo_index].lptr = my_calloc(_ALLOC_ID_, cadlayers, sizeof(xLine *));
    dest->uslot[undo_index].bptr = my_calloc(_ALLOC_ID_, cadlayers, sizeof(xRect *));
    dest->uslot[undo_index].aptr = my_calloc(_ALLOC_ID_, cadlayers, sizeof(xArc *));
    dest->uslot[undo_index].pptr = my_calloc(_ALLOC_ID_, cadlayers, sizeof(xPoly *));

    memcpy(dest->uslot[undo_index].lines,
         source->uslot[undo_index].lines, sizeof(dest->uslot[undo_index].lines[0]) * cadlayers);
    memcpy(dest->uslot[undo_index].rects,
         source->uslot[undo_index].rects, sizeof(dest->uslot[undo_index].rects[0]) * cadlayers);
    memcpy(dest->uslot[undo_index].arcs,
         source->uslot[undo_index].arcs, sizeof(dest->uslot[undo_index].arcs[0]) * cadlayers);
    memcpy(dest->uslot[undo_index].polygons,
         source->uslot[undo_index].polygons, sizeof(dest->uslot[undo_index].polygons[0]) * cadlayers);

    for(c = 0;c < cadlayers; ++c) {
      dest->uslot[undo_index].lptr[c] = my_calloc(_ALLOC_ID_,
           source->uslot[undo_index].lines[c], sizeof(xLine));
      dest->uslot[undo_index].bptr[c] = my_calloc(_ALLOC_ID_,
           source->uslot[undo_index].rects[c], sizeof(xRect));
      dest->uslot[undo_index].pptr[c] = my_calloc(_ALLOC_ID_,
           source->uslot[undo_index].polygons[c], sizeof(xPoly));
      dest->uslot[undo_index].aptr[c] = my_calloc(_ALLOC_ID_,
           source->uslot[undo_index].arcs[c], sizeof(xArc));
    }

    dest->uslot[undo_index].wptr = my_calloc(_ALLOC_ID_,
           source->uslot[undo_index].wires, sizeof(xWire));
    dest->uslot[undo_index].tptr = my_calloc(_ALLOC_ID_,
           source->uslot[undo_index].texts, sizeof(xText));
    dest->uslot[undo_index].iptr = my_calloc(_ALLOC_ID_,
           source->uslot[undo_index].instances, sizeof(xInstance));
    dest->uslot[undo_index].symptr = my_calloc(_ALLOC_ID_,
           source->uslot[undo_index].symbols, sizeof(xSymbol));

    dest->uslot[undo_index].texts = source->uslot[undo_index].texts;
    dest->uslot[undo_index].instances = source->uslot[undo_index].instances;
    dest->uslot[undo_index].symbols = source->uslot[undo_index].symbols;
    dest->uslot[undo_index].wires = source->uslot[undo_index].wires;
  
    for(c = 0;c<cadlayers; ++c) {
      /* lines */
      for(i = 0;i<source->uslot[undo_index].lines[c]; ++i) {
        dest->uslot[undo_index].lptr[c][i] = source->uslot[undo_index].lptr[c][i];
        dest->uslot[undo_index].lptr[c][i].prop_ptr = NULL;
        my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].lptr[c][i].prop_ptr,
                              source->uslot[undo_index].lptr[c][i].prop_ptr);
      }
      /* rects */
      for(i = 0;i<source->uslot[undo_index].rects[c]; ++i) {
        dest->uslot[undo_index].bptr[c][i] = source->uslot[undo_index].bptr[c][i];
        dest->uslot[undo_index].bptr[c][i].prop_ptr = NULL;
        dest->uslot[undo_index].bptr[c][i].extraptr = NULL;
        my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].bptr[c][i].prop_ptr,
                              source->uslot[undo_index].bptr[c][i].prop_ptr);
      }
      /* arcs */
      for(i = 0;i<source->uslot[undo_index].arcs[c]; ++i) {
        dest->uslot[undo_index].aptr[c][i] = source->uslot[undo_index].aptr[c][i];
        dest->uslot[undo_index].aptr[c][i].prop_ptr = NULL;
        my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].aptr[c][i].prop_ptr,
                              source->uslot[undo_index].aptr[c][i].prop_ptr);
      }
      /*polygons */
      for(i = 0;i<source->uslot[undo_index].polygons[c]; ++i) {
        int points = source->uslot[undo_index].pptr[c][i].points;
        dest->uslot[undo_index].pptr[c][i] = source->uslot[undo_index].pptr[c][i];
        dest->uslot[undo_index].pptr[c][i].prop_ptr = NULL;
        dest->uslot[undo_index].pptr[c][i].x = my_malloc(_ALLOC_ID_, points * sizeof(double));
        dest->uslot[undo_index].pptr[c][i].y = my_malloc(_ALLOC_ID_, points * sizeof(double));
        dest->uslot[undo_index].pptr[c][i].selected_point =
             my_malloc(_ALLOC_ID_, points * sizeof(unsigned short));
        memcpy(dest->uslot[undo_index].pptr[c][i].x,
             source->uslot[undo_index].pptr[c][i].x, points * sizeof(double));
        memcpy(dest->uslot[undo_index].pptr[c][i].y,
             source->uslot[undo_index].pptr[c][i].y, points * sizeof(double));
        memcpy(dest->uslot[undo_index].pptr[c][i].selected_point,
             source->uslot[undo_index].pptr[c][i].selected_point, points * sizeof(unsigned short));
        my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].pptr[c][i].prop_ptr,
                              source->uslot[undo_index].pptr[c][i].prop_ptr);
      }
    }
    /* instances */
    for(i = 0;i<source->uslot[undo_index].instances; ++i) {
      dest->uslot[undo_index].iptr[i] = source->uslot[undo_index].iptr[i];
      dest->uslot[undo_index].iptr[i].prop_ptr = NULL;
      dest->uslot[undo_index].iptr[i].name = NULL;
      dest->uslot[undo_index].iptr[i].instname = NULL;
      dest->uslot[undo_index].iptr[i].lab = NULL;
      dest->uslot[undo_index].iptr[i].node = NULL;
      my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].iptr[i].lab,
                            source->uslot[undo_index].iptr[i].lab);
      my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].iptr[i].instname,
                            source->uslot[undo_index].iptr[i].instname);
      my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].iptr[i].prop_ptr,
                            source->uslot[undo_index].iptr[i].prop_ptr);
      my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].iptr[i].name,
                            source->uslot[undo_index].iptr[i].name);
    }
  
    /* symbols */
    for(i = 0;i<source->uslot[undo_index].symbols; ++i) {
      copy_symbol(i, &dest->uslot[undo_index].symptr[i], &source->uslot[undo_index].symptr[i]);
    }
    /* texts */
    for(i = 0;i<source->uslot[undo_index].texts; ++i) {
      dest->uslot[undo_index].tptr[i] = source->uslot[undo_index].tptr[i];
      dest->uslot[undo_index].tptr[i].prop_ptr = NULL;
      dest->uslot[undo_index].tptr[i].txt_ptr = NULL;
      dest->uslot[undo_index].tptr[i].font = NULL;
      dest->uslot[undo_index].tptr[i].floater_instname = NULL;
      dest->uslot[undo_index].tptr[i].floater_ptr = NULL;
      my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].tptr[i].prop_ptr,
                            source->uslot[undo_index].tptr[i].prop_ptr);
      my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].tptr[i].txt_ptr,
                            source->uslot[undo_index].tptr[i].txt_ptr);
      my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].tptr[i].font,
                            source->uslot[undo_index].tptr[i].font);
      my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].tptr[i].floater_instname,
                            source->uslot[undo_index].tptr[i].floater_instname);
      my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].tptr[i].floater_ptr,
                            source->uslot[undo_index].tptr[i].floater_ptr);
    }
  
    /* wires */
    for(i = 0;i<source->uslot[undo_index].wires; ++i) {
      dest->uslot[undo_index].wptr[i] = source->uslot[undo_index].wptr[i];
      dest->uslot[undo_index].wptr[i].prop_ptr = NULL;
      dest->uslot[undo_index].wptr[i].node = NULL;
      my_strdup2(_ALLOC_ID_, &dest->uslot[undo_index].wptr[i].prop_ptr,
                            source->uslot[undo_index].wptr[i].prop_ptr);
    }
  }
}

