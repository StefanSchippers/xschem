/* File: check.c
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
#ifdef __unix__
#include <sys/select.h> /* select() */
#endif
static int check_includes(double x1a, double y1a, double x2a, double y2a,
                   double x1b, double y1b, double x2b, double y2b)
{ 
  if( x1b >= x1a && x2b <= x2a && y1b >= y1a && y2b <= y2a &&
      ( (x2a-x1a)*(y2b-y1b) == (x2b-x1b)*(y2a-y1a) ) /* parallel */
  ) {
    return 1;
  }
  return 0;
}

static int check_breaks(double x1, double y1, double x2, double y2, double x, double y)
{
  if( ( (x > x1 && x < x2) || (y > y1 && y < y2) ) && 
      ( (x2-x1)*(y-y1) == (x-x1)*(y2-y1) ) /* parallel */
  ) {
    return 1;
  }
  return 0;
}

void update_conn_cues(int layer, int draw_cues, int dr_win)
{
  int k, i, l, sqx, sqy, save_draw;
  Wireentry *wptr;
  double x0, y0;
  double x1, y1, x2, y2;
  Wireentry *wireptr;
  xWire * const wire = xctx->wire;
  Iterator_ctx ctx;

  hash_wires(); /* must be done also if wires==0 to clear wire_spatial_table */
  if(!xctx->wires) return;
  if(!xctx->draw_dots) return;
  if(xctx->cadhalfdotsize*xctx->mooz<0.7) return;
  x1 = X_TO_XSCHEM(xctx->areax1);
  y1 = Y_TO_XSCHEM(xctx->areay1);
  x2 = X_TO_XSCHEM(xctx->areax2);
  y2 = Y_TO_XSCHEM(xctx->areay2);
  for(init_wire_iterator(&ctx, x1, y1, x2, y2); ( wireptr = wire_iterator_next(&ctx) ) ;) {
    k=wireptr->n;
    /* optimization when editing small areas (detailed zoom)  of a huge schematic */
    if(LINE_OUTSIDE(wire[k].x1, wire[k].y1, wire[k].x2, wire[k].y2, x1, y1, x2, y2)) continue;
    for(l = 0;l < 2; ++l) {
      if(l==0 ) {
        if(wire[k].end1 !=-1) continue;
        wire[k].end1=0;
        x0 = wire[k].x1;
        y0 = wire[k].y1;
      } else {
        if(wire[k].end2 !=-1) continue;
        wire[k].end2=0;
        x0 = wire[k].x2;
        y0 = wire[k].y2;
      }
      get_square(x0, y0, &sqx, &sqy);
      for(wptr = xctx->wire_spatial_table[sqx][sqy] ; wptr ; wptr = wptr->next) {
        i = wptr->n;
        if(i == k) {
          continue; /* no check wire against itself */
        }
        if( touch(wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2, x0,y0) ) {
          if( (x0 != wire[i].x1 && x0 != wire[i].x2) ||
              (y0 != wire[i].y1 && y0 != wire[i].y2) ) {
            if(l == 0) wire[k].end1 += 2;
            else     wire[k].end2 += 2;
          } else {
            if(l == 0) wire[k].end1 += 1;
            else     wire[k].end2 += 1;
          }
        }
      }
    }
  }
  dbg(3, "update_conn_cues(): check3\n");
  if(draw_cues) {
    save_draw = xctx->draw_window; xctx->draw_window = dr_win;
    for(init_wire_iterator(&ctx, x1, y1, x2, y2); ( wireptr = wire_iterator_next(&ctx) ) ;) {
      i = wireptr->n;
      /* optimization when editing small areas (detailed zoom)  of a huge schematic */
      if(LINE_OUTSIDE(wire[i].x1, wire[i].y1,
                      wire[i].x2, wire[i].y2, x1, y1, x2, y2)) continue;
      if( wire[i].end1 >1 ) {
        filledarc(layer, ADD, wire[i].x1, wire[i].y1, xctx->cadhalfdotsize, 0, 360);
      }
      if( wire[i].end2 >1 ) {
        filledarc(layer, ADD, wire[i].x2, wire[i].y2, xctx->cadhalfdotsize, 0, 360);
      }
    }
    filledarc(layer, END, 0.0, 0.0, 0.0, 0.0, 0.0);
    xctx->draw_window = save_draw;
  }
}

void sleep_ms(int milliseconds)
{
  #ifdef __unix__
  struct timeval tv;
  tv.tv_sec = milliseconds / 1000;
  tv.tv_usec = milliseconds % 1000 * 1000;
  select(0, NULL, NULL, NULL, &tv);
  #else
  Sleep(milliseconds);
  #endif
}

/* start = 0: initialize timer
 * start = 1: return elapsed time since previous call
 * start = 2: return total time from initialize */
double timer(int start)
{
  /* used only for test mode. No problem with switching schematic context */
  static double st, cur, lap; /* safe to keep even with multiple schematics */
  if(start == 0) return lap = st = (double) clock() / CLOCKS_PER_SEC;
  else if(start == 1) {
    double prevlap = lap;
    lap = cur = (double) clock() / CLOCKS_PER_SEC;
    return cur - prevlap;
  } else {
    cur = (double) clock() / CLOCKS_PER_SEC;
    return cur - st;
  }
}

void trim_wires(void)
{
  int k, sqx, sqy, doloops;
  double x0, y0;
  int j, i, changed;
  int includes, breaks;
  Wireentry *wptr;
  unsigned short *wireflag=NULL;

  doloops = 0;
  xctx->prep_hash_wires = 0;
  /* timer(0); */
  do {
    /* dbg(1, "trim_wires(): start: %g\n", timer(1)); */
    changed = 0;
    ++doloops;
    hash_wires(); /* end1 and end2 reset to -1 */
    /* dbg(1, "trim_wires(): hash_wires_1: %g\n", timer(1)); */

    /* break all wires */
    for(i=0;i<xctx->wires; ++i) {
      int hashloopcnt = 0;
      x0 = xctx->wire[i].x1;
      y0 = xctx->wire[i].y1;
      get_square(x0, y0, &sqx, &sqy);
      k=1;
      for(wptr = xctx->wire_spatial_table[sqx][sqy] ; ; wptr = wptr->next) {
        if(!wptr) {
          if(k == 1) {
            x0 = xctx->wire[i].x2;
            y0 = xctx->wire[i].y2;
            get_square(x0, y0, &sqx, &sqy);
            wptr = xctx->wire_spatial_table[sqx][sqy];
            k = 2;
            if(!wptr) break;
          } else break;
        }
        j = wptr->n;
        if(i == j) continue;
        ++hashloopcnt;
        breaks = check_breaks(xctx->wire[j].x1, xctx->wire[j].y1, xctx->wire[j].x2, xctx->wire[j].y2, x0, y0);
        if(breaks) { /* wire[i] breaks wire[j] */
          dbg(2, "trim_wires(): %d (%g %g %g %g) breaks %d (%g %g %g %g) in (%g, %g)\n", i,
            xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2,
            j,
            xctx->wire[j].x1, xctx->wire[j].y1, xctx->wire[j].x2, xctx->wire[j].y2,
            x0, y0
          );
          check_wire_storage();
          xctx->wire[xctx->wires].x1=xctx->wire[j].x1;
          xctx->wire[xctx->wires].y1=xctx->wire[j].y1;
          xctx->wire[xctx->wires].x2=x0;
          xctx->wire[xctx->wires].y2=y0;


          if(xctx->wire[j].sel == SELECTED1) {
            xctx->wire[xctx->wires].sel = SELECTED1;
            xctx->wire[j].sel = 0;
          } else if(xctx->wire[j].sel == SELECTED2) {
            xctx->wire[xctx->wires].sel = 0;
            xctx->wire[j].sel = SELECTED2;
          } else if(xctx->wire[j].sel == SELECTED) {
            xctx->wire[xctx->wires].sel = SELECTED;
            xctx->wire[j].sel = SELECTED;
          } else {
            xctx->wire[xctx->wires].sel = 0;
            xctx->wire[j].sel = 0;
          }
          xctx->wire[xctx->wires].prop_ptr=NULL;
          my_strdup(_ALLOC_ID_, &xctx->wire[xctx->wires].prop_ptr, xctx->wire[j].prop_ptr);
           xctx->wire[xctx->wires].bus =
             get_attr_val(get_tok_value(xctx->wire[xctx->wires].prop_ptr,"bus",0));
          xctx->wire[xctx->wires].node=NULL;

          my_strdup(_ALLOC_ID_, &xctx->wire[xctx->wires].node, xctx->wire[j].node);
          xctx->wire[j].x1 = x0;
          xctx->wire[j].y1 = y0;
          hash_wire(XINSERT, xctx->wires, 0);
          i--; /* redo current i iteration, since we break the 'j' loop due to changed wire hash table */
          hash_wire(XDELETE, j, 0); /* rehash since endpoint x1, y1 changed */
          hash_wire(XINSERT, j, 0);
          xctx->wires++;
          changed = 1;
          break;
        }
      }
      dbg(2, "trim_wires(): hashloopcnt = %d, wires = %d\n", hashloopcnt, xctx->wires);
    }
    /* dbg(1, "trim_wires(): break: %g\n", timer(1)); */
    /* reduce included wires */
    my_realloc(_ALLOC_ID_, &wireflag, xctx->wires*sizeof(unsigned short));
    memset(wireflag, 0, xctx->wires*sizeof(unsigned short));
    for(i=0;i<xctx->wires; ++i) {
      if(wireflag[i]) continue;
      x0 = xctx->wire[i].x1;
      y0 = xctx->wire[i].y1;
      get_square(x0, y0, &sqx, &sqy);
      k=1;
      for(wptr = xctx->wire_spatial_table[sqx][sqy] ; ; wptr = wptr->next) {
        if(!wptr) {
          if(k == 1) {
            x0 = xctx->wire[i].x2;
            y0 = xctx->wire[i].y2;
            get_square(x0, y0, &sqx, &sqy);
            wptr = xctx->wire_spatial_table[sqx][sqy];
            k = 2;
            if(!wptr) break;
          } else break;
        }
        j = wptr->n;
        if(i == j || wireflag[j]) continue;
  
        includes = check_includes(xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2,
                                  xctx->wire[j].x1, xctx->wire[j].y1, xctx->wire[j].x2, xctx->wire[j].y2);
        if(includes) {
          dbg(2, "trim_wires(): %d (%g %g %g %g) include %d (%g %g %g %g)\n", i,
            xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2,
            j,
            xctx->wire[j].x1, xctx->wire[j].y1, xctx->wire[j].x2, xctx->wire[j].y2
          );
          wireflag[j] = 1;
        }
      }
    }
    /* dbg(1, "trim_wires(): included: %g\n", timer(1)); */
  
    /* delete wires */
    j = 0;
    for(i=0;i<xctx->wires; ++i)
    {
      if(wireflag[i]) {
        ++j;
        /* hash_wire(XDELETE, i, 0);*/ /* can not be done since wire deletions change wire idexes in array */
        my_free(_ALLOC_ID_, &xctx->wire[i].prop_ptr);
        my_free(_ALLOC_ID_, &xctx->wire[i].node);
        continue;
      }
      if(j) {
        xctx->wire[i-j] = xctx->wire[i];
      }
    }
    xctx->wires -= j;
    if(j) {
      xctx->prep_hash_wires=0;
      changed = 1;
    }
    /* dbg(1, "trim_wires(): delete_1: %g\n", timer(1)); */
  
    /* after wire deletions full rehash is needed */
    hash_wires();

    my_realloc(_ALLOC_ID_, &wireflag, xctx->wires*sizeof(unsigned short));
    memset(wireflag, 0, xctx->wires*sizeof(unsigned short));
    /* dbg(1, "trim_wires(): hash_wires_2: %g\n", timer(1)); */

    /* update endpoint (end1, end2) connection counters */
    for(i=0;i<xctx->wires; ++i) {
      x0 = xctx->wire[i].x1;
      y0 = xctx->wire[i].y1;
      xctx->wire[i].end1 = xctx->wire[i].end2 = 0;
      get_square(x0, y0, &sqx, &sqy);
      k=1;
      for(wptr = xctx->wire_spatial_table[sqx][sqy] ; ; wptr = wptr->next) {
        if(!wptr) {
          if(k == 1) {
            x0 = xctx->wire[i].x2;
            y0 = xctx->wire[i].y2;
            get_square(x0, y0, &sqx, &sqy);
            wptr = xctx->wire_spatial_table[sqx][sqy];
            k = 2;
            if(!wptr) break;
          } else break;
        }
        j = wptr->n;
        if(i == j) continue;
        if( touch(xctx->wire[j].x1, xctx->wire[j].y1, xctx->wire[j].x2, xctx->wire[j].y2, x0,y0) ) {
          /* not parallel */
          if( (xctx->wire[i].x2 -  xctx->wire[i].x1) * (xctx->wire[j].y2 -  xctx->wire[j].y1) !=
              (xctx->wire[j].x2 -  xctx->wire[j].x1) * (xctx->wire[i].y2 -  xctx->wire[i].y1)) {
            /* wire[i] touches wire[j] in an inner point, not at edge */
            if( (x0 != xctx->wire[j].x1 && x0 != xctx->wire[j].x2) ||
                (y0 != xctx->wire[j].y1 && y0 != xctx->wire[j].y2) ) {
              if(k == 1) xctx->wire[i].end1 += 2;
              else       xctx->wire[i].end2 += 2;
            } else {
              if(k == 1) xctx->wire[i].end1 += 1;
              else       xctx->wire[i].end2 += 1;
            }
          } 
        }
      }
    }
    /* dbg(1, "trim_wires(): endpoints: %g\n", timer(1)); */
  
    /* merge parallel touching (in wire[i].x2, wire[i].y2) wires */
    for(i=0;i<xctx->wires; ++i) {
      if(wireflag[i]) continue;
      x0 = xctx->wire[i].x2;
      y0 = xctx->wire[i].y2;
      get_square(x0, y0, &sqx, &sqy);
      for(wptr = xctx->wire_spatial_table[sqx][sqy] ; wptr ; wptr = wptr->next) {
        j = wptr->n;
        if(i == j || wireflag[j]) continue;
        if( touch(xctx->wire[j].x1, xctx->wire[j].y1, xctx->wire[j].x2, xctx->wire[j].y2, x0,y0) &&
            /* parallel */
            (xctx->wire[i].x2 -  xctx->wire[i].x1) * (xctx->wire[j].y2 -  xctx->wire[j].y1) ==
            (xctx->wire[j].x2 -  xctx->wire[j].x1) * (xctx->wire[i].y2 -  xctx->wire[i].y1) && 
            /* touch in wire[j].x1, wire[j].y1 */
            xctx->wire[j].x1 == x0 && xctx->wire[j].y1 == y0 &&
            /* no other connecting wires */
            xctx->wire[i].end2 == 0 && xctx->wire[j].end1 == 0 ) {
          dbg(2, "trim_wires(): i=%d merged with j=%d\n", i, j);
          xctx->wire[i].x2 = xctx->wire[j].x2;
          xctx->wire[i].y2 = xctx->wire[j].y2;
          if(xctx->wire[j].sel) xctx->wire[i].sel = xctx->wire[j].sel;
          wireflag[j] = 1;
          break;
        }
      }
    }
    /* dbg(1, "trim_wires(): merge: %g\n", timer(1)); */
  
    /* delete wires */
    j = 0;
    for(i=0;i<xctx->wires; ++i)
    {
      xctx->wire[i].end1 = xctx->wire[i].end2 = -1; /* reset all endpoints we recalculate all at end */
      if(wireflag[i]) {
        ++j;
        /* hash_wire(XDELETE, i, 0);*/ /* can not be done since wire deletions change wire idexes in array */
        my_free(_ALLOC_ID_, &xctx->wire[i].prop_ptr);
        my_free(_ALLOC_ID_, &xctx->wire[i].node);
        continue;
      }
      if(j) {
        xctx->wire[i-j] = xctx->wire[i];
      }
    }
    xctx->wires -= j;
    if(j) {
      xctx->prep_hash_wires=0; /* after wire deletions full rehash is needed */
      changed = 1;
    }
    /* dbg(1, "trim_wires(): delete_2: %g\n", timer(1)); */

    if(changed) {
      xctx->need_reb_sel_arr = 1;
      xctx->prep_net_structs=0;
      xctx->prep_hi_structs=0;
      set_modify(1);
    }
  } while(changed);
  dbg(1, "trim_wires(): doloops=%d changed=%d\n", doloops, changed);
  my_free(_ALLOC_ID_, &wireflag);
  update_conn_cues(WIRELAYER, 0, 0);
}

static int touches_inst_pin(double x, double y, int inst)
{
  int rects, r;
  double x0, y0;
  int touches = 0;
  if((xctx->inst[inst].ptr >= 0)) {
    rects = (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER];
    for(r=0;r<rects;r++)
    {
      get_inst_pin_coord(inst, r, &x0, &y0);
      if(x == x0 && y == y0) {
        touches = 1;
        break;
      }
    }
  }
  dbg(1, "touches_inst_pin(): %g %g : touches =%d on inst %d\n", x, y, touches, inst);
  return touches;
}

/* return 2 if x0, y0 is on the segment
 * return 1 if x0, y0 is less than cadsnap (10) from the segment
 * return 0 if nothing will be cut (mouse too far away or degenerated segment) 
 * In this case x0, y0 are reset to the closest point on the segment */
static int closest_point_calculation(double x1, double y1, double x2, double y2,
                                     double *x0, double *y0, int align)
{
  double projection, sq_distance, x3, y3;
  double cs = tclgetdoublevar("cadsnap"), sq_cs;
  int ret = 0;

  sq_cs = cs * cs; /* get squared value to compare with squared distance */

  if(x1 == x2 && y1 == y2) {
    ret = 0;
  } else {
    projection = (x2 - x1) * (*x0 - x1) + (y2 - y1) * (*y0 - y1);
    projection /= (x2 - x1) * ( x2 - x1) + (y2 - y1) * (y2 - y1);
    x3 = x1 + projection * (x2 - x1);
    y3 = y1 + projection * (y2 - y1);
    sq_distance = (*x0 - x3) * (*x0 - x3) + (*y0 - y3) * (*y0 - y3);
    if(projection <= 1 && projection >= 0) { /* point is within x1,y1 - x2,y2 */
      if(sq_distance == 0) ret = 2;
      else if(sq_distance <  sq_cs) ret = 1;
    }
    dbg(1, "x3 = %g y3=%g dist=%g ret=%d\n", x3, y3, sqrt(sq_distance), ret);
  }

  if(ret == 1) {
    if(align) {
      *x0 = my_round(x3 / cs) * cs;
      *y0 = my_round(y3 / cs) * cs;
    } else {
      *x0 = x3;
      *y0 = y3;
    }
    /* if ret == 2 leave x0 and y0 as they are since x0,y0 already touches wire */
  }
  return ret;
}

void break_wires_at_point(double x0, double y0, int align)
{
  int r, i, sqx, sqy;
  Wireentry *wptr;
  int changed=0;
  double x1, y1, x2, y2;

  dbg(1, "break_wires_at_pins(): processing pin %g %g\n", x0, y0);
  get_square(x0, y0, &sqx, &sqy);
  for(wptr=xctx->wire_spatial_table[sqx][sqy]; wptr; wptr=wptr->next) {
    i = wptr->n;
    x1 = xctx->wire[i].x1;
    y1 = xctx->wire[i].y1;
    x2 = xctx->wire[i].x2;
    y2 = xctx->wire[i].y2;
    r = closest_point_calculation(x1, y1, x2, y2, &x0, &y0, align);
    if( r != 0 && (r == 1 || touch(x1, y1, x2, y2, x0,y0) )) {
      if( (x0 != x1 && x0 != x2) ||
          (y0 != y1 && y0 != y2) ) {
        dbg(1, "break_wires_at_point(): processing wire %d: %g %g %g %g\n",
            i, xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2);
        if(!changed) { xctx->push_undo(); changed=1;}
        check_wire_storage();
        xctx->wire[xctx->wires].x1=xctx->wire[i].x1;
        xctx->wire[xctx->wires].y1=xctx->wire[i].y1;
        xctx->wire[xctx->wires].end1 = xctx->wire[i].end1;
        xctx->wire[xctx->wires].end2 = 1;
        xctx->wire[xctx->wires].x2=x0;
        xctx->wire[xctx->wires].y2=y0;
        xctx->wire[xctx->wires].sel=0;
        xctx->wire[xctx->wires].prop_ptr=NULL;
        my_strdup(_ALLOC_ID_, &xctx->wire[xctx->wires].prop_ptr, xctx->wire[i].prop_ptr);
        xctx->wire[xctx->wires].bus =
          get_attr_val(get_tok_value(xctx->wire[xctx->wires].prop_ptr,"bus",0));
        xctx->wire[xctx->wires].node=NULL;
        hash_wire(XINSERT, xctx->wires, 0);  /* insertion happens at beginning of list */
        dbg(1, "break_wires_at_pins(): hashing new wire %d: %g %g %g %g\n", 
            xctx->wires, xctx->wire[xctx->wires].x1, xctx->wire[xctx->wires].y1,
                         xctx->wire[xctx->wires].x2, xctx->wire[xctx->wires].y2);
        my_strdup(_ALLOC_ID_, &xctx->wire[xctx->wires].node, xctx->wire[i].node);
        xctx->need_reb_sel_arr=1;
        xctx->wires++;
        xctx->wire[i].x1 = x0;
        xctx->wire[i].y1 = y0;
        xctx->wire[i].sel = 0;
        xctx->wire[i].end1 = 1;
      } /* if( (x0!=xctx->wire[i].x1 && x0!=xctx->wire[i].x2) || ... ) */
    } /* if( touch(xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2, x0,y0) ) */
  } /* for(wptr=xctx->wire_spatial_table[sqx][sqy]; wptr; wptr=wptr->next) */
  if(changed) {
    int w = xctx->draw_window;
    int p = xctx->draw_pixmap;
    xctx->need_reb_sel_arr = 1;
    rebuild_selected_array();
    draw();
    xctx->draw_window = 1;
    xctx->draw_pixmap = 0;
    filledarc(PINLAYER, NOW, x0, y0, xctx->cadhalfdotsize, 0, 360);
    xctx->draw_window = w;
    xctx->draw_pixmap = p;

  }
}

/* if remove=1 is given wires that are all inside instance bboxes are deleted */
void break_wires_at_pins(int remove)
{
  int k, i, j, r, rects, sqx, sqy;
  Wireentry *wptr;
  double x0, y0;
  int changed=0;
  int deleted_wire = 0;
  hash_wires();
  rebuild_selected_array();

  /* break wires that touch selected instance pins */
  for(j=0;j<xctx->lastsel; ++j) if(xctx->sel_array[j].type==ELEMENT) {
    k = xctx->sel_array[j].n;
    if( (rects = (xctx->inst[k].ptr+ xctx->sym)->rects[PINLAYER]) > 0 )
    {
      for(r=0;r<rects;r++) {
        get_inst_pin_coord(k, r, &x0, &y0);
        dbg(1, "break_wires_at_pins(): processing pin %g %g\n", x0, y0);
        get_square(x0, y0, &sqx, &sqy);
        for(wptr=xctx->wire_spatial_table[sqx][sqy]; wptr; wptr=wptr->next) {
          i = wptr->n;
          if( touch(xctx->wire[i].x1, xctx->wire[i].y1,
                    xctx->wire[i].x2, xctx->wire[i].y2, x0,y0) ) {
            if( (x0!=xctx->wire[i].x1 && x0!=xctx->wire[i].x2) ||
                (y0!=xctx->wire[i].y1 && y0!=xctx->wire[i].y2) ) {
              dbg(1, "break_wires_at_pins(): processing wire %d: %g %g %g %g\n",
                  i, xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2);
              if(!changed) { xctx->push_undo(); changed=1;}
              check_wire_storage();
              if(!remove || !RECT_INSIDE(xctx->wire[i].x1, xctx->wire[i].y1, x0, y0, 
                             xctx->inst[k].xx1, xctx->inst[k].yy1, xctx->inst[k].xx2, xctx->inst[k].yy2)
                             || (!touches_inst_pin(xctx->wire[i].x1, xctx->wire[i].y1, k) && xctx->wire[i].end1 > 0) 
                ) {
                xctx->wire[xctx->wires].x1=xctx->wire[i].x1;
                xctx->wire[xctx->wires].y1=xctx->wire[i].y1;
                xctx->wire[xctx->wires].end1 = xctx->wire[i].end1;
                xctx->wire[xctx->wires].end2 = 1;
                xctx->wire[xctx->wires].x2=x0;
                xctx->wire[xctx->wires].y2=y0;
                xctx->wire[xctx->wires].sel=xctx->wire[i].sel;
                xctx->wire[xctx->wires].prop_ptr=NULL;
                my_strdup(_ALLOC_ID_, &xctx->wire[xctx->wires].prop_ptr, xctx->wire[i].prop_ptr);
                xctx->wire[xctx->wires].bus =
                   get_attr_val(get_tok_value(xctx->wire[xctx->wires].prop_ptr,"bus", 0));
                xctx->wire[xctx->wires].node=NULL;
                hash_wire(XINSERT, xctx->wires, 0);  /* insertion happens at beginning of list */
                dbg(1, "break_wires_at_pins(): hashing new wire %d: %g %g %g %g\n", 
                    xctx->wires, xctx->wire[xctx->wires].x1, xctx->wire[xctx->wires].y1,
                                 xctx->wire[xctx->wires].x2, xctx->wire[xctx->wires].y2);
                my_strdup(_ALLOC_ID_, &xctx->wire[xctx->wires].node, xctx->wire[i].node);
                xctx->need_reb_sel_arr=1;
                xctx->wires++;
              } else {
                deleted_wire = 1;
              }
              xctx->wire[i].x1 = x0;
              xctx->wire[i].y1 = y0;
              xctx->wire[i].end1 = 1;
              if(remove && RECT_INSIDE(xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2,
                  xctx->inst[k].xx1, xctx->inst[k].yy1, xctx->inst[k].xx2, xctx->inst[k].yy2)) {

                if(touches_inst_pin(xctx->wire[i].x2, xctx->wire[i].y2, k) || xctx->wire[i].end2 == 0) {
                  dbg(1, "break_wires_at_pins(): wire %d needs to be deleted: %g %g %g %g\n", 
                          i, xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2);
                  /* mark for deletion only if no other nets attached */
                  xctx->wire[i].sel = SELECTED4; /* use a special flag to later delete these wires
                                                  * only and not other seleted wires */
                  dbg(1, "break_wires_at_pins(): mark wire %d for deletion: end2=%d\n", i, xctx->wire[i].end2);
                }
              }
            } /* if( (x0!=xctx->wire[i].x1 && x0!=xctx->wire[i].x2) || ... ) */
            else if(remove) {
              int t1 = touches_inst_pin(xctx->wire[i].x1, xctx->wire[i].y1, k);
              int t2 = touches_inst_pin(xctx->wire[i].x2, xctx->wire[i].y2, k);
              int e1 = xctx->wire[i].end1;
              int e2 = xctx->wire[i].end2;
              int inside = RECT_INSIDE(xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2,
                           xctx->inst[k].xx1, xctx->inst[k].yy1, xctx->inst[k].xx2, xctx->inst[k].yy2);
              dbg(1, "i=%d, t1=%d, t2=%d, e1=%d, e2=%d\n", i, t1, t2, e1, e2);
              if(inside && ( (t1 && t2) || (t1 && e2 == 0) || (t2 && e1 == 0) )) {
                xctx->wire[i].sel = SELECTED4; 
                if(!changed) { xctx->push_undo(); changed=1;}
              }
            }
          } /* if( touch(xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2, x0,y0) ) */
        } /* for(wptr=xctx->wire_spatial_table[sqx][sqy]; wptr; wptr=wptr->next) */
      } /* for(r=0;r<rects;r++) */
    } /* if( (rects = (xctx->inst[k].ptr+ xctx->sym)->rects[PINLAYER]) > 0 ) */
  } /* for(j=0;j<xctx->lastsel; ++j) if(xctx->sel_array[j].type==ELEMENT) */

  if(remove) {
    if(delete_wires(SELECTED4)) {
      deleted_wire = 1;
    }
  }
  /* break wires that touch selected wires */
  rebuild_selected_array();
  for(j=0;j<xctx->lastsel; ++j) if(xctx->sel_array[j].type==WIRE) {
  /* for(k=0; k < xctx->wires; ++k) { */
    int l;

    k = xctx->sel_array[j].n;
    for(l=0;l<2; ++l) {
      if(l==0 ) {
        x0 = xctx->wire[k].x1;
        y0 = xctx->wire[k].y1;
      } else {
        x0 = xctx->wire[k].x2;
        y0 = xctx->wire[k].y2;
      }
      get_square(x0, y0, &sqx, &sqy);
      /* printf("  k=%d, x0=%g, y0=%g\n", k, x0, y0); */
      for(wptr=xctx->wire_spatial_table[sqx][sqy] ; wptr ; wptr = wptr->next) {
        i = wptr->n;
        /* printf("check wire %d to wire %d\n", k, i); */
        if(i==k) {
          continue; /* no check wire against itself */
        }
        if( touch(xctx->wire[i].x1, xctx->wire[i].y1,
                  xctx->wire[i].x2, xctx->wire[i].y2, x0,y0) )
        {
          if( (x0!=xctx->wire[i].x1 && x0!=xctx->wire[i].x2) ||
              (y0!=xctx->wire[i].y1 && y0!=xctx->wire[i].y2) ) {
            /* printf("touch in mid point: %d\n", l+1); */
            if(!changed) { xctx->push_undo(); changed=1;}
            check_wire_storage();
            xctx->wire[xctx->wires].x1=xctx->wire[i].x1;
            xctx->wire[xctx->wires].y1=xctx->wire[i].y1;
            xctx->wire[xctx->wires].x2=x0;
            xctx->wire[xctx->wires].y2=y0;
            xctx->wire[xctx->wires].end1 = xctx->wire[i].end1;
            xctx->wire[xctx->wires].end2 = 1;
            xctx->wire[xctx->wires].sel=SELECTED;
            set_first_sel(WIRE, xctx->wires, 0);
            xctx->wire[xctx->wires].prop_ptr=NULL;
            my_strdup(_ALLOC_ID_, &xctx->wire[xctx->wires].prop_ptr, xctx->wire[i].prop_ptr);
            xctx->wire[xctx->wires].bus =
              get_attr_val(get_tok_value(xctx->wire[xctx->wires].prop_ptr,"bus",0));
            xctx->wire[xctx->wires].node=NULL;
            hash_wire(XINSERT, xctx->wires, 0);  /* insertion happens at beginning of list */
            xctx->need_reb_sel_arr=1;
            xctx->wires++;
            xctx->wire[i].x1 = x0;
            xctx->wire[i].y1 = y0;
            xctx->wire[i].end1 = 1;
          }
        }
      }
    }
  }
  if(changed) {
    set_modify(1);
    xctx->prep_net_structs=0;
    xctx->prep_hi_structs=0;
    xctx->prep_hash_wires=0;
    prepare_netlist_structs(0);
    if(deleted_wire) {
      if(tclgetboolvar("autotrim_wires")) trim_wires();
      update_conn_cues(WIRELAYER, 0, 0);
      draw();
    }
  }

}
