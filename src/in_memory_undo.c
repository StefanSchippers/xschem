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

static void free_undo_lines(int slot)
{
  int i, c;

  for(c = 0;c<cadlayers; c++) {
    for(i = 0;i<xctx->uslot[slot].lines[c]; i++) {
      my_free(783, &xctx->uslot[slot].lptr[c][i].prop_ptr);
    }
    my_free(784, &xctx->uslot[slot].lptr[c]);
    xctx->uslot[slot].lines[c] = 0;
  }
}

static void free_undo_rects(int slot)
{
  int i, c;

  for(c = 0;c<cadlayers; c++) {
    for(i = 0;i<xctx->uslot[slot].rects[c]; i++) {
      my_free(785, &xctx->uslot[slot].bptr[c][i].prop_ptr);
    }
    my_free(786, &xctx->uslot[slot].bptr[c]);
    xctx->uslot[slot].rects[c] = 0;
  }
}

static void free_undo_polygons(int slot)
{
  int i, c;

  for(c = 0;c<cadlayers; c++) {
    for(i = 0;i<xctx->uslot[slot].polygons[c]; i++) {
      my_free(787, &xctx->uslot[slot].pptr[c][i].prop_ptr);
      my_free(788, &xctx->uslot[slot].pptr[c][i].x);
      my_free(789, &xctx->uslot[slot].pptr[c][i].y);
      my_free(790, &xctx->uslot[slot].pptr[c][i].selected_point);
    }
    my_free(791, &xctx->uslot[slot].pptr[c]);
    xctx->uslot[slot].polygons[c] = 0;
  }
}

static void free_undo_arcs(int slot)
{
  int i, c;

  for(c = 0;c<cadlayers; c++) {
    for(i = 0;i<xctx->uslot[slot].arcs[c]; i++) {
      my_free(792, &xctx->uslot[slot].aptr[c][i].prop_ptr);
    }
    my_free(793, &xctx->uslot[slot].aptr[c]);
    xctx->uslot[slot].arcs[c] = 0;
  }
}

static void free_undo_wires(int slot)
{
  int i;

  for(i = 0;i<xctx->uslot[slot].wires; i++) {
    my_free(794, &xctx->uslot[slot].wptr[i].prop_ptr);
  }
  my_free(795, &xctx->uslot[slot].wptr);
  xctx->uslot[slot].wires = 0;
}

static void free_undo_texts(int slot)
{
  int i;

  for(i = 0;i<xctx->uslot[slot].texts; i++) {
    my_free(796, &xctx->uslot[slot].tptr[i].prop_ptr);
    my_free(797, &xctx->uslot[slot].tptr[i].txt_ptr);
    my_free(798, &xctx->uslot[slot].tptr[i].font);
  }
  my_free(799, &xctx->uslot[slot].tptr);
  xctx->uslot[slot].texts = 0;
}

static void free_undo_instances(int slot)
{
  int i;

  for(i = 0;i<xctx->uslot[slot].instances; i++) {
    my_free(800, &xctx->uslot[slot].iptr[i].name);
    my_free(801, &xctx->uslot[slot].iptr[i].prop_ptr);
    my_free(802, &xctx->uslot[slot].iptr[i].instname);
    my_free(104, &xctx->uslot[slot].iptr[i].lab);
  }
  my_free(803, &xctx->uslot[slot].iptr);
  xctx->uslot[slot].instances = 0;
}

static void free_undo_symbols(int slot)
{
  int i, j, c, symbols;
  xSymbol *sym;

  symbols = xctx->uslot[slot].symbols;
  for(i = 0;i < symbols; i++) {
    sym = &xctx->uslot[slot].symptr[i];
    my_free(354, &sym->name);
    my_free(355, &sym->prop_ptr);
    my_free(373, &sym->type);
    my_free(660, &sym->templ);

    for(c = 0;c<cadlayers;c++) {
      for(j = 0;j<sym->polygons[c];j++) {
        if(sym->poly[c][j].prop_ptr != NULL) {
          my_free(892, &sym->poly[c][j].prop_ptr);
        }
        my_free(914, &sym->poly[c][j].x);
        my_free(915, &sym->poly[c][j].y);
        my_free(918, &sym->poly[c][j].selected_point);
      }
      my_free(1137, &sym->poly[c]);
      sym->polygons[c] = 0;
  
      for(j = 0;j<sym->lines[c];j++) {
        if(sym->line[c][j].prop_ptr != NULL) {
          my_free(1142, &sym->line[c][j].prop_ptr);
        }
      }
      my_free(1290, &sym->line[c]);
      sym->lines[c] = 0;
  
      for(j = 0;j<sym->arcs[c];j++) {
        if(sym->arc[c][j].prop_ptr != NULL) {
          my_free(1291, &sym->arc[c][j].prop_ptr);
        }
      }
      my_free(1292, &sym->arc[c]);
      sym->arcs[c] = 0;
  
      for(j = 0;j<sym->rects[c];j++) {
        if(sym->rect[c][j].prop_ptr != NULL) {
          my_free(1293, &sym->rect[c][j].prop_ptr);
        }
      }
      my_free(1294, &sym->rect[c]);
      sym->rects[c] = 0;
    }
    for(j = 0;j<sym->texts;j++) {
      if(sym->text[j].prop_ptr != NULL) {
        my_free(1297, &sym->text[j].prop_ptr);
      }
      if(sym->text[j].txt_ptr != NULL) {
        my_free(1298, &sym->text[j].txt_ptr);
      }
      if(sym->text[j].font != NULL) {
        my_free(1299, &sym->text[j].font);
      }
    }
    my_free(1300, &sym->text);
    sym->texts = 0;
    my_free(1301, &sym->line);
    my_free(1302, &sym->rect);
    my_free(1303, &sym->poly);
    my_free(1304, &sym->arc);
    my_free(1305, &sym->lines);
    my_free(1306, &sym->rects);
    my_free(1307, &sym->polygons);
    my_free(1308, &sym->arcs);
  }
  my_free(48, &xctx->uslot[slot].symptr);
  xctx->uslot[slot].symbols = 0;
}

static void mem_init_undo(void)
{
  int slot;

  dbg(1, "mem_init_undo(): undo_initialized = %d\n", xctx->undo_initialized);
  if(!xctx->undo_initialized) {
    for(slot = 0;slot<MAX_UNDO; slot++) {
      xctx->uslot[slot].lines = my_calloc(165, cadlayers, sizeof(int));
      xctx->uslot[slot].rects = my_calloc(166, cadlayers, sizeof(int));
      xctx->uslot[slot].arcs = my_calloc(167, cadlayers, sizeof(int));
      xctx->uslot[slot].polygons = my_calloc(168, cadlayers, sizeof(int));
      xctx->uslot[slot].lptr = my_calloc(169, cadlayers, sizeof(xLine *));
      xctx->uslot[slot].bptr = my_calloc(170, cadlayers, sizeof(xRect *));
      xctx->uslot[slot].aptr = my_calloc(171, cadlayers, sizeof(xArc *));
      xctx->uslot[slot].pptr = my_calloc(172, cadlayers, sizeof(xPoly *));
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
    my_free(804, &xctx->uslot[slot].lines);
    my_free(805, &xctx->uslot[slot].rects);
    my_free(806, &xctx->uslot[slot].arcs);
    my_free(807, &xctx->uslot[slot].polygons);
    my_free(808, &xctx->uslot[slot].lptr);
    my_free(809, &xctx->uslot[slot].bptr);
    my_free(810, &xctx->uslot[slot].aptr);
    my_free(811, &xctx->uslot[slot].pptr);
  }
  xctx->undo_initialized = 0;
}

void mem_push_undo(void)
{
  int slot, i, c;
  xSymbol *sym;
  int j;

  if(xctx->no_undo)return;
  mem_init_undo();
  slot = xctx->cur_undo_ptr%MAX_UNDO;

  my_strdup(173, &xctx->uslot[slot].gptr, xctx->schvhdlprop);
  my_strdup(174, &xctx->uslot[slot].vptr, xctx->schverilogprop);
  my_strdup(175, &xctx->uslot[slot].sptr, xctx->schprop);
  my_strdup(359, &xctx->uslot[slot].kptr, xctx->schsymbolprop);
  my_strdup(176, &xctx->uslot[slot].eptr, xctx->schtedaxprop);

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
  for(c = 0;c<cadlayers;c++) {
    xctx->uslot[slot].lptr[c] = my_calloc(177, xctx->lines[c], sizeof(xLine));
    xctx->uslot[slot].bptr[c] = my_calloc(178, xctx->rects[c], sizeof(xRect));
    xctx->uslot[slot].pptr[c] = my_calloc(179, xctx->polygons[c], sizeof(xPoly));
    xctx->uslot[slot].aptr[c] = my_calloc(180, xctx->arcs[c], sizeof(xArc));
  }
  xctx->uslot[slot].wptr = my_calloc(181, xctx->wires, sizeof(xWire));
  xctx->uslot[slot].tptr = my_calloc(182, xctx->texts, sizeof(xText));
  xctx->uslot[slot].iptr = my_calloc(183, xctx->instances, sizeof(xInstance));
  xctx->uslot[slot].symptr = my_calloc(353, xctx->symbols, sizeof(xSymbol));
  xctx->uslot[slot].texts = xctx->texts;
  xctx->uslot[slot].instances = xctx->instances;
  xctx->uslot[slot].symbols = xctx->symbols;
  xctx->uslot[slot].wires = xctx->wires;

  for(c = 0;c<cadlayers;c++) {
    /* lines */
    for(i = 0;i<xctx->lines[c];i++) {
      xctx->uslot[slot].lptr[c][i] = xctx->line[c][i];
      xctx->uslot[slot].lptr[c][i].prop_ptr = NULL;
      my_strdup(184, &xctx->uslot[slot].lptr[c][i].prop_ptr, xctx->line[c][i].prop_ptr);
    }
    /* rects */
    for(i = 0;i<xctx->rects[c];i++) {
      xctx->uslot[slot].bptr[c][i] = xctx->rect[c][i];
      xctx->uslot[slot].bptr[c][i].prop_ptr = NULL;
      my_strdup(185, &xctx->uslot[slot].bptr[c][i].prop_ptr, xctx->rect[c][i].prop_ptr);
    }
    /* arcs */
    for(i = 0;i<xctx->arcs[c];i++) {
      xctx->uslot[slot].aptr[c][i] = xctx->arc[c][i];
      xctx->uslot[slot].aptr[c][i].prop_ptr = NULL;
      my_strdup(186, &xctx->uslot[slot].aptr[c][i].prop_ptr, xctx->arc[c][i].prop_ptr);
    }
    /*polygons */
    for(i = 0;i<xctx->polygons[c];i++) {
      int points = xctx->poly[c][i].points;
      xctx->uslot[slot].pptr[c][i] = xctx->poly[c][i];
      xctx->uslot[slot].pptr[c][i].prop_ptr = NULL;
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
  for(i = 0;i<xctx->instances;i++) {
    xctx->uslot[slot].iptr[i] = xctx->inst[i];
    xctx->uslot[slot].iptr[i].prop_ptr = NULL;
    xctx->uslot[slot].iptr[i].name = NULL;
    xctx->uslot[slot].iptr[i].instname = NULL;
    xctx->uslot[slot].iptr[i].lab = NULL;
    xctx->uslot[slot].iptr[i].node = NULL;
    my_strdup(330, &xctx->uslot[slot].iptr[i].lab, xctx->inst[i].lab);
    my_strdup2(191, &xctx->uslot[slot].iptr[i].instname, xctx->inst[i].instname);
    my_strdup(192, &xctx->uslot[slot].iptr[i].prop_ptr, xctx->inst[i].prop_ptr);
    my_strdup2(193, &xctx->uslot[slot].iptr[i].name, xctx->inst[i].name);
  }

  /* symbols */
  for(i = 0;i<xctx->symbols;i++) {
    sym = &xctx->uslot[slot].symptr[i];
    xctx->uslot[slot].symptr[i] = xctx->sym[i];
    sym->name = NULL;
    sym->prop_ptr = NULL;
    sym->type = NULL;
    sym->templ = NULL;
    my_strdup2(1316, &sym->name,  xctx->sym[i].name);
    my_strdup2(1317, &sym->type,  xctx->sym[i].type);
    my_strdup2(1318, &sym->templ,  xctx->sym[i].templ);
    my_strdup2(1319, &sym->prop_ptr,  xctx->sym[i].prop_ptr);
    sym->line = my_calloc(1309, cadlayers, sizeof(xLine *));
    sym->poly = my_calloc(1324, cadlayers, sizeof(xPoly *));
    sym->arc = my_calloc(1310, cadlayers, sizeof(xArc *));
    sym->rect = my_calloc(1311, cadlayers, sizeof(xRect *));
    sym->lines = my_calloc(1312, cadlayers, sizeof(int));
    sym->rects = my_calloc(1313, cadlayers, sizeof(int));
    sym->arcs = my_calloc(1314, cadlayers, sizeof(int));
    sym->polygons = my_calloc(1315, cadlayers, sizeof(int));
    sym->text = my_calloc(1320, xctx->sym[i].texts, sizeof(xText));

    memcpy(sym->lines, xctx->sym[i].lines, sizeof(sym->lines[0]) * cadlayers);
    memcpy(sym->rects, xctx->sym[i].rects, sizeof(sym->rects[0]) * cadlayers);
    memcpy(sym->arcs, xctx->sym[i].arcs, sizeof(sym->arcs[0]) * cadlayers);
    memcpy(sym->polygons, xctx->sym[i].polygons, sizeof(sym->polygons[0]) * cadlayers);
    for(c = 0;c<cadlayers;c++) {
      /* symbol lines */
      sym->line[c] = my_calloc(1359, xctx->sym[i].lines[c], sizeof(xLine));
      for(j = 0; j < xctx->sym[i].lines[c]; j++) { 
        sym->line[c][j] = xctx->sym[i].line[c][j];
        sym->line[c][j].prop_ptr = NULL;
        my_strdup(1358, &sym->line[c][j].prop_ptr, xctx->sym[i].line[c][j].prop_ptr);
      }
      /* symbol rects */
      sym->rect[c] = my_calloc(1360, xctx->sym[i].rects[c], sizeof(xRect));
      for(j = 0; j < xctx->sym[i].rects[c]; j++) { 
        sym->rect[c][j] = xctx->sym[i].rect[c][j];
        sym->rect[c][j].prop_ptr = NULL;
        my_strdup(1362, &sym->rect[c][j].prop_ptr, xctx->sym[i].rect[c][j].prop_ptr);
      }
      /* symbol arcs */
      sym->arc[c] = my_calloc(1361, xctx->sym[i].arcs[c], sizeof(xArc));
      for(j = 0; j < xctx->sym[i].arcs[c]; j++) { 
        sym->arc[c][j] = xctx->sym[i].arc[c][j];
        sym->arc[c][j].prop_ptr = NULL;
        my_strdup(1375, &sym->arc[c][j].prop_ptr, xctx->sym[i].arc[c][j].prop_ptr);
      }
      /* symbol polygons */
      sym->poly[c] = my_calloc(1363, xctx->sym[i].polygons[c], sizeof(xPoly));
      for(j = 0; j < xctx->sym[i].polygons[c]; j++) { 
        int points = xctx->sym[i].poly[c][j].points;
        sym->poly[c][j] = xctx->sym[i].poly[c][j];
        sym->poly[c][j].prop_ptr = NULL;
        sym->poly[c][j].x = my_malloc(1365, points * sizeof(double));
        sym->poly[c][j].y = my_malloc(1366, points * sizeof(double));
        sym->poly[c][j].selected_point = my_malloc(1367, points * sizeof(unsigned short));
        my_strdup(1364, &sym->poly[c][j].prop_ptr, xctx->sym[i].poly[c][j].prop_ptr);
        memcpy(sym->poly[c][j].x, xctx->sym[i].poly[c][j].x, points * sizeof(double));
        memcpy(sym->poly[c][j].y, xctx->sym[i].poly[c][j].y, points * sizeof(double));
        memcpy(sym->poly[c][j].selected_point, xctx->sym[i].poly[c][j].selected_point,
             points * sizeof(unsigned short));
      }
    }
    /* symbol texts */
    for(j = 0; j < xctx->sym[i].texts; j++) {
      sym->text[j] = xctx->sym[i].text[j];
      sym->text[j].prop_ptr = NULL;
      sym->text[j].txt_ptr = NULL;
      sym->text[j].font = NULL;
      my_strdup(1321, &sym->text[j].prop_ptr, xctx->sym[i].text[j].prop_ptr);
      my_strdup(1322, &sym->text[j].txt_ptr, xctx->sym[i].text[j].txt_ptr);
      my_strdup(1323, &sym->text[j].font, xctx->sym[i].text[j].font);
    }
   
  }
  /* texts */
  for(i = 0;i<xctx->texts;i++) {
    xctx->uslot[slot].tptr[i] = xctx->text[i];
    xctx->uslot[slot].tptr[i].prop_ptr = NULL;
    xctx->uslot[slot].tptr[i].txt_ptr = NULL;
    xctx->uslot[slot].tptr[i].font = NULL;
    my_strdup(194, &xctx->uslot[slot].tptr[i].prop_ptr, xctx->text[i].prop_ptr);
    my_strdup(195, &xctx->uslot[slot].tptr[i].txt_ptr, xctx->text[i].txt_ptr);
    my_strdup(196, &xctx->uslot[slot].tptr[i].font, xctx->text[i].font);
  }

  /* wires */
  for(i = 0;i<xctx->wires;i++) {
    xctx->uslot[slot].wptr[i] = xctx->wire[i];
    xctx->uslot[slot].wptr[i].prop_ptr = NULL;
    xctx->uslot[slot].wptr[i].node = NULL;
    my_strdup(197, &xctx->uslot[slot].wptr[i].prop_ptr, xctx->wire[i].prop_ptr);
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
  xSymbol *sym;
  int j;

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
  unselect_all();
  my_free(1345, &xctx->wire);
  my_free(1346, &xctx->text);
  my_free(1347, &xctx->inst);

  for(i = 0;i<cadlayers;i++) {
    my_free(1348, &xctx->rect[i]);
    my_free(1342, &xctx->line[i]);
    my_free(1343, &xctx->poly[i]);
    my_free(1344, &xctx->arc[i]);
  }

  remove_symbols();


  for(i = 0;i<xctx->maxs;i++) {
    my_free(1349, &xctx->sym[i].line);
    my_free(1350, &xctx->sym[i].rect);
    my_free(1351, &xctx->sym[i].arc);
    my_free(1352, &xctx->sym[i].poly);
    my_free(1353, &xctx->sym[i].lines);
    my_free(1354, &xctx->sym[i].polygons);
    my_free(1355, &xctx->sym[i].arcs);
    my_free(1356, &xctx->sym[i].rects);
  }


  my_free(0, &xctx->sym);


  my_strdup(198, &xctx->schvhdlprop, xctx->uslot[slot].gptr);
  my_strdup(199, &xctx->schverilogprop, xctx->uslot[slot].vptr);
  my_strdup(200, &xctx->schprop, xctx->uslot[slot].sptr);
  my_strdup(389, &xctx->schsymbolprop, xctx->uslot[slot].kptr);
  my_strdup(201, &xctx->schtedaxprop, xctx->uslot[slot].eptr);

  for(c = 0;c<cadlayers;c++) {
    /* lines */
    xctx->maxl[c] = xctx->lines[c] = xctx->uslot[slot].lines[c];
    xctx->line[c] = my_calloc(202, xctx->lines[c], sizeof(xLine));
    for(i = 0;i<xctx->lines[c];i++) {
      xctx->line[c][i] = xctx->uslot[slot].lptr[c][i];
      xctx->line[c][i].prop_ptr = NULL;
      my_strdup(203, &xctx->line[c][i].prop_ptr, xctx->uslot[slot].lptr[c][i].prop_ptr);
    }
    /* rects */
    xctx->maxr[c] = xctx->rects[c] = xctx->uslot[slot].rects[c];
    xctx->rect[c] = my_calloc(204, xctx->rects[c], sizeof(xRect));
    for(i = 0;i<xctx->rects[c];i++) {
      xctx->rect[c][i] = xctx->uslot[slot].bptr[c][i];
      xctx->rect[c][i].prop_ptr = NULL;
      my_strdup(205, &xctx->rect[c][i].prop_ptr, xctx->uslot[slot].bptr[c][i].prop_ptr);
    }
    /* arcs */
    xctx->maxa[c] = xctx->arcs[c] = xctx->uslot[slot].arcs[c];
    xctx->arc[c] = my_calloc(206, xctx->arcs[c], sizeof(xArc));
    for(i = 0;i<xctx->arcs[c];i++) {
      xctx->arc[c][i] = xctx->uslot[slot].aptr[c][i];
      xctx->arc[c][i].prop_ptr = NULL;
      my_strdup(207, &xctx->arc[c][i].prop_ptr, xctx->uslot[slot].aptr[c][i].prop_ptr);
    }
    /* polygons */
    xctx->maxp[c] = xctx->polygons[c] = xctx->uslot[slot].polygons[c];
    xctx->poly[c] = my_calloc(208, xctx->polygons[c], sizeof(xPoly));
    for(i = 0;i<xctx->polygons[c];i++) {
      int points = xctx->uslot[slot].pptr[c][i].points;
      xctx->poly[c][i] = xctx->uslot[slot].pptr[c][i];
      xctx->poly[c][i].prop_ptr = NULL;
      my_strdup(209, &xctx->poly[c][i].prop_ptr, xctx->uslot[slot].pptr[c][i].prop_ptr);
      xctx->poly[c][i].x = my_malloc(210, points * sizeof(double));
      xctx->poly[c][i].y = my_malloc(211, points * sizeof(double));
      xctx->poly[c][i].selected_point = my_malloc(212, points * sizeof(unsigned short));
      memcpy(xctx->poly[c][i].x, xctx->uslot[slot].pptr[c][i].x, points * sizeof(double));
      memcpy(xctx->poly[c][i].y, xctx->uslot[slot].pptr[c][i].y, points * sizeof(double));
      memcpy(xctx->poly[c][i].selected_point, xctx->uslot[slot].pptr[c][i].selected_point, 
        points * sizeof(unsigned short));
    }
  }

  /* instances */
  xctx->maxi = xctx->instances = xctx->uslot[slot].instances;
  xctx->inst = my_calloc(213, xctx->instances, sizeof(xInstance));
  for(i = 0;i<xctx->instances;i++) {
    xctx->inst[i] = xctx->uslot[slot].iptr[i];
    xctx->inst[i].prop_ptr = NULL;
    xctx->inst[i].name = NULL;
    xctx->inst[i].instname = NULL;
    xctx->inst[i].lab = NULL;
    my_strdup(214, &xctx->inst[i].prop_ptr, xctx->uslot[slot].iptr[i].prop_ptr);
    my_strdup2(215, &xctx->inst[i].name, xctx->uslot[slot].iptr[i].name);
    my_strdup2(216, &xctx->inst[i].instname, xctx->uslot[slot].iptr[i].instname);
    my_strdup(766, &xctx->inst[i].lab, xctx->uslot[slot].iptr[i].lab);
  }

  /* symbols */
  xctx->maxs = xctx->symbols = xctx->uslot[slot].symbols;
  xctx->sym = my_calloc(1325, xctx->symbols, sizeof(xSymbol));

  for(i = 0;i<xctx->symbols;i++) {
    sym = &xctx->uslot[slot].symptr[i];
    xctx->sym[i] =  *sym;
    xctx->sym[i].name = NULL;
    xctx->sym[i].prop_ptr = NULL;
    xctx->sym[i].type = NULL;
    xctx->sym[i].templ = NULL;
    my_strdup2(1326, &xctx->sym[i].name, sym->name);
    my_strdup2(1327, &xctx->sym[i].type, sym->type);
    my_strdup2(1328, &xctx->sym[i].templ, sym->templ);
    my_strdup2(1329, &xctx->sym[i].prop_ptr, sym->prop_ptr);

    xctx->sym[i].line = my_calloc(1330, cadlayers, sizeof(xLine *));
    xctx->sym[i].poly = my_calloc(1331, cadlayers, sizeof(xPoly *));
    xctx->sym[i].arc = my_calloc(1332, cadlayers, sizeof(xArc *));
    xctx->sym[i].rect = my_calloc(1333, cadlayers, sizeof(xRect *));
    xctx->sym[i].lines = my_calloc(1334, cadlayers, sizeof(int));
    xctx->sym[i].rects = my_calloc(1335, cadlayers, sizeof(int));
    xctx->sym[i].arcs = my_calloc(1336, cadlayers, sizeof(int));
    xctx->sym[i].polygons = my_calloc(1337, cadlayers, sizeof(int));
    xctx->sym[i].text = my_calloc(1338, xctx->sym[i].texts, sizeof(xText));

    memcpy(xctx->sym[i].lines, sym->lines, sizeof(sym->lines[0]) * cadlayers);
    memcpy(xctx->sym[i].rects, sym->rects, sizeof(sym->rects[0]) * cadlayers);
    memcpy(xctx->sym[i].arcs, sym->arcs, sizeof(sym->arcs[0]) * cadlayers);
    memcpy(xctx->sym[i].polygons, sym->polygons, sizeof(sym->polygons[0]) * cadlayers);

    for(c = 0;c<cadlayers;c++) {
      /* symbol lines */
      xctx->sym[i].line[c] = my_calloc(1371, sym->lines[c], sizeof(xLine));
      for(j = 0; j < xctx->sym[i].lines[c]; j++) {
        xctx->sym[i].line[c][j] = sym->line[c][j];
        xctx->sym[i].line[c][j].prop_ptr = NULL;
        my_strdup(1368, & xctx->sym[i].line[c][j].prop_ptr, sym->line[c][j].prop_ptr);
      }
      /* symbol rects */
      xctx->sym[i].rect[c] = my_calloc(1372, sym->rects[c], sizeof(xRect));
      for(j = 0; j < xctx->sym[i].rects[c]; j++) {
        xctx->sym[i].rect[c][j] = sym->rect[c][j];
        xctx->sym[i].rect[c][j].prop_ptr = NULL;
        my_strdup(1369, & xctx->sym[i].rect[c][j].prop_ptr, sym->rect[c][j].prop_ptr);
      }
      /* symbol arcs */
      xctx->sym[i].arc[c] = my_calloc(1373, sym->arcs[c], sizeof(xArc));
      for(j = 0; j < xctx->sym[i].arcs[c]; j++) {
        xctx->sym[i].arc[c][j] = sym->arc[c][j];
        xctx->sym[i].arc[c][j].prop_ptr = NULL;
        my_strdup(1370, & xctx->sym[i].arc[c][j].prop_ptr, sym->arc[c][j].prop_ptr);
      }
      /* symbol polygons */
      xctx->sym[i].poly[c] = my_calloc(1374, sym->polygons[c], sizeof(xPoly));
      for(j = 0; j < xctx->sym[i].polygons[c]; j++) { 
        int points = sym->poly[c][j].points;
        xctx->sym[i].poly[c][j] = sym->poly[c][j];
        xctx->sym[i].poly[c][j].prop_ptr = NULL;
        xctx->sym[i].poly[c][j].x = my_malloc(1376, points * sizeof(double));
        xctx->sym[i].poly[c][j].y = my_malloc(1377, points * sizeof(double));
        xctx->sym[i].poly[c][j].selected_point = my_malloc(1378, points * sizeof(unsigned short));
        memcpy(xctx->sym[i].poly[c][j].x, sym->poly[c][j].x, points * sizeof(double));
        memcpy(xctx->sym[i].poly[c][j].y, sym->poly[c][j].y, points * sizeof(double));
        memcpy(xctx->sym[i].poly[c][j].selected_point, sym->poly[c][j].selected_point,
             points * sizeof(unsigned short));
        my_strdup(1357, & xctx->sym[i].poly[c][j].prop_ptr, sym->poly[c][j].prop_ptr);
      }
    }
    /* symbol texts */
    for(j = 0; j < xctx->sym[i].texts; j++) {
      xctx->sym[i].text[j] = sym->text[j];
      xctx->sym[i].text[j].prop_ptr = NULL;
      xctx->sym[i].text[j].txt_ptr = NULL;
      xctx->sym[i].text[j].font = NULL;
      my_strdup(1339, &xctx->sym[i].text[j].prop_ptr, sym->text[j].prop_ptr);
      my_strdup(1340, &xctx->sym[i].text[j].txt_ptr, sym->text[j].txt_ptr);
      my_strdup(1341, &xctx->sym[i].text[j].font, sym->text[j].font);
    }
  }

  /* texts */
  xctx->maxt = xctx->texts = xctx->uslot[slot].texts;
  xctx->text = my_calloc(217, xctx->texts, sizeof(xText));
  for(i = 0;i<xctx->texts;i++) {
    xctx->text[i] = xctx->uslot[slot].tptr[i];
    xctx->text[i].txt_ptr = NULL;
    xctx->text[i].font = NULL;
    xctx->text[i].prop_ptr = NULL;
    my_strdup(218, &xctx->text[i].prop_ptr, xctx->uslot[slot].tptr[i].prop_ptr);
    my_strdup(219, &xctx->text[i].txt_ptr, xctx->uslot[slot].tptr[i].txt_ptr);
    my_strdup(220, &xctx->text[i].font, xctx->uslot[slot].tptr[i].font);
  }

  /* wires */
  xctx->maxw = xctx->wires = xctx->uslot[slot].wires;
  xctx->wire = my_calloc(221, xctx->wires, sizeof(xWire));
  for(i = 0;i<xctx->wires;i++) {
    xctx->wire[i] = xctx->uslot[slot].wptr[i];
    xctx->wire[i].prop_ptr = NULL;
    xctx->wire[i].node = NULL;
    my_strdup(222, &xctx->wire[i].prop_ptr, xctx->uslot[slot].wptr[i].prop_ptr);
  }
  /* unnecessary since in_memory_undo saves all symbols */
  /* link_symbols_to_instances(-1); */
  if(redo == 2) xctx->cur_undo_ptr++; /* restore undo stack pointer */
  if(set_modify_status) set_modify(1);
  xctx->prep_hash_inst = 0;
  xctx->prep_hash_wires = 0;
  xctx->prep_net_structs = 0;
  xctx->prep_hi_structs = 0;
  update_conn_cues(0, 0);
}
