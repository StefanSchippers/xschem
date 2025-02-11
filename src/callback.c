/* File: callback.c
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

/* allow to use the Windows keys as alternate for Alt */
#define SET_MODMASK ( (rstate & Mod1Mask) || (rstate & Mod4Mask) ) 
#define EQUAL_MODMASK ( (rstate == Mod1Mask) || (rstate == Mod4Mask) ) 

static int waves_selected(int event, KeySym key, int state, int button)
{
  int rstate; /* state without ShiftMask */
  int i, check;
  int graph_use_ctrl_key = tclgetboolvar("graph_use_ctrl_key");
  int is_inside = 0, skip = 0;
  static unsigned int excl = STARTZOOM | STARTRECT | STARTLINE | STARTWIRE |
                             STARTPAN | STARTSELECT | STARTMOVE | STARTCOPY;
  int draw_xhair = tclgetboolvar("draw_crosshair");
  rstate = state; /* rstate does not have ShiftMask bit, so easier to test for KeyPress events */
  rstate &= ~ShiftMask; /* don't use ShiftMask, identifying characters is sufficient */
  if(xctx->ui_state & excl) skip = 1;
  /* else if(event != -3 && sch_waves_loaded() < 0 ) skip = 1; */
  /* allow to work on graphs even if ctrl released while in GRAPHPAN mode
   * This is useful on touchpads with TappingDragLock enabled */
  else if(graph_use_ctrl_key && !(state & ControlMask) && !(xctx->ui_state & GRAPHPAN)) skip = 1;
  else if(SET_MODMASK) skip = 1;
  else if(event == MotionNotify && (state & Button2Mask)) skip = 1;
  else if(event == MotionNotify && (state & Button1Mask) && (state & ShiftMask)) skip = 1;
  else if(event == ButtonPress && button == Button2) skip = 1;
  else if(event == ButtonPress && button == Button1 && (state & ShiftMask) ) skip = 1;
  else if(event == ButtonRelease && button == Button2) skip = 1;
  /* else if(event == KeyPress && (state & ShiftMask)) skip = 1; */
  else if(!skip) for(i=0; i< xctx->rects[GRIDLAYER]; ++i) {
    xRect *r;
    r = &xctx->rect[GRIDLAYER][i];
    if(!(r->flags & 1) ) continue;
    if(!strboolcmp(get_tok_value(xctx->rect[GRIDLAYER][i].prop_ptr, "lock", 0), "true")) continue;

    check =
      (xctx->ui_state & GRAPHPAN) ||
      (event != -3 &&
         (
           POINTINSIDE(xctx->mousex, xctx->mousey, r->x1 + 20,  r->y1 + 8,  r->x2 - 20,  r->y2 - 8) ||
           POINTINSIDE(xctx->mousex, xctx->mousey, r->x1,  r->y1,  r->x1 + 20,  r->y1 + 8) ||
           POINTINSIDE(xctx->mousex, xctx->mousey, r->x2 - 20,  r->y2 - 8,  r->x2,  r->y2)
         )
      ) ||
      ( event == -3 &&
        (POINTINSIDE(xctx->mousex, xctx->mousey, r->x1,  r->y1,  r->x2 - 40,  r->y1 + 20) ||
         POINTINSIDE(xctx->mousex, xctx->mousey, r->x1 + 20,  r->y1,  r->x2 - 30,  r->y2 - 10))
      );

    if(check) {
       is_inside = 1;
       if(! (xctx->ui_state & GRAPHPAN) ) { 
         xctx->graph_master = i;
       }
       if(draw_xhair) draw_crosshair(1, 0); /* remove crosshair, re-enable mouse cursor */
       tclvareval(xctx->top_path, ".drw configure -cursor tcross" , NULL);
       break;
    }
  }
  if(!is_inside) {
    xctx->graph_master = -1;
    xctx->ui_state &= ~GRAPHPAN; /* terminate ongoing GRAPHPAN to avoid deadlocks */
    if(draw_xhair) {
      if(tclgetintvar("crosshair_size") == 0) {
        tclvareval(xctx->top_path, ".drw configure -cursor none" , NULL);
      } else {
        tclvareval(xctx->top_path, ".drw configure -cursor {}" , NULL);
      }
    } else
      tclvareval(xctx->top_path, ".drw configure -cursor {}" , NULL);
    if(xctx->graph_flags & 64) {
      tcleval("graph_show_measure stop");
    }
  }
  return is_inside;
}

/* do nothing if coordinates not changed unless force is given */
void redraw_w_a_l_r_p_z_rubbers(int force)
{
  double mx = xctx->mousex_snap;
  double my = xctx->mousey_snap;

  if(!force && xctx->mousex_snap == xctx->prev_rubberx && xctx->mousey_snap == xctx->prev_rubbery) return;

  if(xctx->ui_state & STARTZOOM) zoom_rectangle(RUBBER);
  if(xctx->ui_state & STARTWIRE) {
    if(xctx->constr_mv == 1) my = xctx->my_double_save;
    if(xctx->constr_mv == 2) mx = xctx->mx_double_save;
    new_wire(RUBBER, mx, my);
  }
  if(xctx->ui_state & STARTARC) {
    if(xctx->constr_mv == 1) my = xctx->my_double_save;
    if(xctx->constr_mv == 2) mx = xctx->mx_double_save;
    new_arc(RUBBER, 0, mx, my);
  }
  if(xctx->ui_state & STARTLINE) {
    if(xctx->constr_mv == 1) my = xctx->my_double_save;
    if(xctx->constr_mv == 2) mx = xctx->mx_double_save;
    new_line(RUBBER, mx, my);
  }
  if(xctx->ui_state & STARTRECT) new_rect(RUBBER,mx, my);
  if(xctx->ui_state & STARTPOLYGON) {
    if(xctx->constr_mv == 1) my = xctx->my_double_save;
    if(xctx->constr_mv == 2) mx = xctx->mx_double_save;
    new_polygon(RUBBER, mx, my);
  }
  xctx->prev_rubberx = xctx->mousex_snap;
  xctx->prev_rubbery = xctx->mousey_snap;
}

/* resets UI state, unselect all and abort any pending operation */
void abort_operation(void)
{
  xctx->no_draw = 0;
  tcleval("set constr_mv 0" );
  dbg(1, "abort_operation(): Escape: ui_state=%d, last_command=%d\n", xctx->ui_state, xctx->last_command);
  xctx->constr_mv=0;
  if(xctx->last_command && xctx->ui_state & (STARTWIRE | STARTLINE)) {
    if(xctx->ui_state & STARTWIRE) new_wire(RUBBER|CLEAR, xctx->mousex_snap, xctx->mousey_snap);
    if(xctx->ui_state & STARTLINE) new_line(RUBBER|CLEAR, xctx->mousex_snap, xctx->mousey_snap);
    if(tclgetboolvar("draw_crosshair")) draw_crosshair(2, 0);
    xctx->ui_state = 0;
    return;
  }
  xctx->last_command=0;
  xctx->manhattan_lines = 0;
  if(xctx->ui_state & STARTMOVE)
  {
   move_objects(ABORT,0,0,0);
   if(xctx->ui_state & (START_SYMPIN | PLACE_SYMBOL | PLACE_TEXT)) {
     int save;
     save =  xctx->modified;
     delete(1/* to_push_undo */);
     set_modify(save); /* aborted placement: no change, so reset modify flag set by delete() */
     xctx->ui_state &= ~START_SYMPIN;
     xctx->ui_state &= ~PLACE_SYMBOL;
     xctx->ui_state &= ~PLACE_TEXT;
   }
   return;
  }
  if(xctx->ui_state & STARTCOPY)
  {
   copy_objects(ABORT);
   return;
  }
  if(xctx->ui_state & STARTMERGE) {
    delete(1/* to_push_undo */);
    set_modify(0); /* aborted merge: no change, so reset modify flag set by delete() */
  }
  xctx->ui_state = 0;
  unselect_all(1);
  draw();
}

static void start_place_symbol(void)
{
    xctx->last_command = 0;
    rebuild_selected_array();
    if(xctx->lastsel && xctx->sel_array[0].type==ELEMENT) {
      tclvareval("set INITIALINSTDIR [file dirname {",
           abs_sym_path(tcl_hook2(xctx->inst[xctx->sel_array[0].n].name), ""), "}]", NULL);
    } 
    unselect_all(1);
    xctx->mx_double_save = xctx->mousex_snap;
    xctx->my_double_save = xctx->mousey_snap;
    if(place_symbol(-1,NULL,xctx->mousex_snap, xctx->mousey_snap, 0, 0, NULL, 4, 1, 1/* to_push_undo */) ) {
     xctx->mousey_snap = xctx->my_double_save;
      xctx->mousex_snap = xctx->mx_double_save;
      move_objects(START,0,0,0);
      xctx->ui_state |= PLACE_SYMBOL;
    }
}

static void start_line(double mx, double my)
{
    xctx->last_command = STARTLINE;
    if(xctx->ui_state & STARTLINE) {
      if(xctx->constr_mv != 2) {
        xctx->mx_double_save=mx;
      }
      if(xctx->constr_mv != 1) {
        xctx->my_double_save=my;
      }
      if(xctx->constr_mv == 1) my = xctx->my_double_save;
      if(xctx->constr_mv == 2) mx = xctx->mx_double_save;
    } else {
      xctx->mx_double_save=mx;
      xctx->my_double_save=my;
    }
    new_line(PLACE, mx, my);
}

static void start_wire(double mx, double my)
{
     dbg(1, "start_wire(): ui_state=%d, ui_state2=%d last_command=%d\n",
         xctx->ui_state, xctx->ui_state2, xctx->last_command);
     xctx->last_command = STARTWIRE;
     if(xctx->ui_state & STARTWIRE) {
       if(xctx->constr_mv != 2) {
         xctx->mx_double_save = mx;
       }
       if(xctx->constr_mv != 1) {
         xctx->my_double_save = my;
       }
       if(xctx->constr_mv == 1) my = xctx->my_double_save;
       if(xctx->constr_mv == 2) mx = xctx->mx_double_save;
     } else {
       xctx->mx_double_save=mx;
       xctx->my_double_save=my;
     }
     new_wire(PLACE,mx, my);
}

static double interpolate_yval(int idx, int p, double x, int sweep_idx, int point_not_last)
{          
  double val = xctx->raw->values[idx][p];
  /* not operating point, annotate from 'b' cursor */
  if(point_not_last && (xctx->raw->allpoints > 1) && sweep_idx >= 0) {
    Raw *raw = xctx->raw;
    SPICE_DATA *sweep_gv = raw->values[sweep_idx];
    SPICE_DATA *gv = raw->values[idx];
    double dx = sweep_gv[p + 1] - sweep_gv[p];
    double dy = gv[p + 1] - gv[p];
    double offset = x - sweep_gv[p];
    double interp = dx != 0.0 ? offset * dy / dx : 0.0;
    val += interp;
  }  
  return val;
}      

void backannotate_at_cursor_b_pos(xRect *r, Graph_ctx *gr)
{
  tcleval("catch {eval $cursor_2_hook}");
  if(sch_waves_loaded() >= 0) { 
    int dset, first = -1, last, dataset = gr->dataset, i, p, ofs = 0, ofs_end;
    double start, end;
    int sweepvar_wrap = 0, sweep_idx;
    double xx, cursor2; /* xx is the p-th sweep variable value, cursor2 is cursor 'b' x position */
    Raw *raw = xctx->raw;
    int  save_datasets = -1, save_npoints = -1;
    /* transform multiple OP points into a dc sweep */
    if(raw->sim_type && !strcmp(raw->sim_type, "op") && raw->datasets > 1 && raw->npoints[0] == 1) {
      save_datasets = raw->datasets;
      raw->datasets = 1;
      save_npoints = raw->npoints[0];
      raw->npoints[0] = raw->allpoints;
    }
    sweep_idx = get_raw_index(find_nth(get_tok_value(r->prop_ptr, "sweep", 0), ", ", "\"", 0, 1), NULL);
    if(sweep_idx < 0) sweep_idx = 0;
    if(r->flags & 4) { /* private_cursor */
      const char *s = get_tok_value(r->prop_ptr, "cursor2_x", 0);
      if(s[0]) {
        cursor2 = atof_spice(s);
      } else {
        cursor2 = xctx->graph_cursor2_x;
      } 
    } else {
      cursor2 = xctx->graph_cursor2_x; 
    } 
    start = (gr->gx1 <= gr->gx2) ? gr->gx1 : gr->gx2;
    end = (gr->gx1 <= gr->gx2) ? gr->gx2 : gr->gx1;
    dbg(1, "start=%g, end=%g\n", start, end);
    if(gr->logx) {
      start = pow(10, start);
      end = pow(10, end);
    }
    dbg(1, "cursor b pos: %g dataset=%d\n",  cursor2, gr->dataset);
    if(dataset < 0) dataset = 0; /* if all datasets are plotted use first for backannotation */
    dbg(1, "dataset=%d\n", dataset);
    ofs = 0;
    for(dset = 0 ; dset < raw->datasets; dset++) {
      double prev_x, prev_prev_x;
      int cnt=0, wrap;
      register SPICE_DATA *gv = raw->values[sweep_idx];
      int s=0;
      ofs_end = ofs + raw->npoints[dset];
      first = -1;
      prev_prev_x = prev_x = 0;
      last = ofs;

      /* optimization: skip unwanted datasets, if no dc no need to detect sweep variable wraps */
      if(dataset >= 0 && strcmp(xctx->raw->sim_type, "dc") && dataset != sweepvar_wrap) goto done;

      for(p = ofs ; p < ofs_end; p++) {
        xx = gv[p];
        wrap = ( cnt > 1 && XSIGN(xx - prev_x) != XSIGN(prev_x - prev_prev_x));
        if(wrap) {
           sweepvar_wrap++;
           cnt = 0;
        }
        if(xx >= start && xx <= end) {
          if((dataset == sweepvar_wrap)) {
            dbg(1, "xx=%g cursor2=%g first=%d last=%d start=%g end=%g p=%d wrap=%d sweepvar_wrap=%d ofs=%d\n",
              xx, cursor2, first, last, start, end, p, wrap, sweepvar_wrap, ofs);
            if(first == -1) first = p;
            if(p == first) {
              if(xx == cursor2) {goto found;}
              s = XSIGN0(xx - cursor2);
              dbg(1, "s=%d\n", s);
            } else {
              int ss =  XSIGN0(xx -  cursor2);
              dbg(1, "s=%d, ss=%d\n", s, ss);
              if(ss != s) {goto found;}
            }
            last = p;
          }
          ++cnt;
        } /* if(xx >= start && xx <= end) */
        prev_prev_x = prev_x;
        prev_x = xx;
      } /* for(p = ofs ; p < ofs + raw->npoints[dset]; p++) */
      /* offset pointing to next dataset */

      done:

      ofs = ofs_end;
      sweepvar_wrap++;
    } /* for(dset...) */
    found:
    if(first != -1) {
      if(p > last) {
        double sweep0, sweep1;
        p = last;
        sweep0 = raw->values[sweep_idx][first];
        sweep1 = raw->values[sweep_idx][p];
        if(fabs(sweep0 - cursor2) < fabs(sweep1 - cursor2)) {
          p = first;
        }
      }
      dbg(1, "xx=%g, p=%d\n", xx, p);
      Tcl_UnsetVar(interp, "ngspice::ngspice_data", TCL_GLOBAL_ONLY);
      raw->annot_p = p;
      raw->annot_x = cursor2;
      raw->annot_sweep_idx = sweep_idx;
      for(i = 0; i < raw->nvars; ++i) {
        char s[100];
        raw->cursor_b_val[i] = interpolate_yval(i, p, cursor2, sweep_idx, (p < ofs_end));
        my_snprintf(s, S(s), "%.5g", raw->cursor_b_val[i]);
        /* tclvareval("array set ngspice::ngspice_data [list {",  raw->names[i], "} ", s, "]", NULL); */
        Tcl_SetVar2(interp, "ngspice::ngspice_data", raw->names[i], s, TCL_GLOBAL_ONLY);
      }
      Tcl_SetVar2(interp, "ngspice::ngspice_data", "n\\ vars", my_itoa( raw->nvars), TCL_GLOBAL_ONLY);
      Tcl_SetVar2(interp, "ngspice::ngspice_data", "n\\ points", "1", TCL_GLOBAL_ONLY);
    }
    if(save_npoints != -1) { /* restore multiple OP points from artificial dc sweep */
      raw->datasets = save_datasets;
      raw->npoints[0] = save_npoints;
    }
  }
}

/* process user input (arrow keys for now) when only graphs are selected */

/* xctx->graph_flags:
 *  1: dnu, reserved, used in draw_graphs()
 *  2: draw x-cursor1
 *  4: draw x-cursor2
 *  8: dnu, reserved, used in draw_graphs()
 * 16: move x-cursor1
 * 32: move x-cursor2
 * 64: show measurement tooltip
 * 128: draw y-cursor1 (hcursor)
 * 256: draw y-cursor2 (hcursor)
 * 512: move y-cursor1
 * 1024: move y-cursor2
 */
static int waves_callback(int event, int mx, int my, KeySym key, int button, int aux, int state)
{
  Graph_ctx *gr;
  int rstate; /* reduced state wit ShiftMask bit filtered out */
  int graph_use_ctrl_key = tclgetboolvar("graph_use_ctrl_key");
  int i, dataset = 0;
  int need_fullredraw = 0, need_all_redraw = 0, need_redraw = 0, need_redraw_master = 0;
  double xx1 = 0.0, xx2 = 0.0, yy1, yy2;
  double delta_threshold = 0.25;
  double zoom_m = 0.5;
  int save_mouse_at_end = 0, clear_graphpan_at_end = 0;
  int track_dset = -2; /* used to find dataset of closest wave to mouse if 't' is pressed */
  xRect *r = NULL;
  int access_cond = !graph_use_ctrl_key || (state & ControlMask);

  dbg(1, "uistate=%d, graph_flags=%d\n", xctx->ui_state, xctx->graph_flags);
  if(event != -3 && !xctx->raw) return 0;
  rstate = state; /* rstate does not have ShiftMask bit, so easier to test for KeyPress events */
  rstate &= ~ShiftMask; /* don't use ShiftMask, identifying characters is sufficient */
  #if HAS_CAIRO==1
  cairo_save(xctx->cairo_ctx);
  cairo_save(xctx->cairo_save_ctx);
  xctx->cairo_font =
        cairo_toy_font_face_create("Sans-Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_face(xctx->cairo_ctx, xctx->cairo_font);
  cairo_set_font_face(xctx->cairo_save_ctx, xctx->cairo_font);
  cairo_font_face_destroy(xctx->cairo_font);
  #endif
  gr = &xctx->graph_struct;
  if((i = xctx->graph_master) >= 0 && ((r = &xctx->rect[GRIDLAYER][i])->flags & 1)) {
    /* check if this is the master graph (the one containing the mouse pointer) */
    /* determine if mouse pointer is below xaxis or left of yaxis in some graph */
    setup_graph_data(i, 0, gr);

   /* check if user clicked on a wave label -> draw wave in bold */
    if(event == ButtonPress && button == Button3 &&
             edit_wave_attributes(2, i, gr)) {
      draw_graph(i, 1 + 8 + (xctx->graph_flags & (2 | 4 | 128 | 256)), gr, NULL); /* draw data in graph box */
      return 0;
    }

    /* destroy / show measurement widget */
    if(xctx->graph_flags & 64) {
      char sx[100], sy[100];
      double xval, yval;
      if(gr->digital) { 
        double deltag = gr->gy2 - gr->gy1;
        double s1 = DIG_NWAVES; /* 1/DIG_NWAVES  waveforms fit in graph if unscaled vertically */
        double s2 = DIG_SPACE; /* (DIG_NWAVES - DIG_SPACE) spacing between traces */
        double c = s1 * deltag;
        deltag = deltag * s1 / s2;
        yval=(DG_Y(xctx->mousey) - c) / s2;
        yval=fmod(yval, deltag ) +  gr->gy1;
        if(yval > gr->gy2 + deltag * (s1 + s2) * 0.5) yval -= deltag;
      } else {
        yval = G_Y(xctx->mousey);
      }

      xval = G_X(xctx->mousex);
      if(gr->logx) xval = pow(10, xval);
      if(gr->logy) yval = pow(10, yval);
      if(gr->unitx != 1.0) 
        my_snprintf(sx, S(sx), "%.5g%c", gr->unitx * xval, gr->unitx_suffix);
      else
        my_strncpy(sx, dtoa_eng(xval), S(sx));

      if(gr->unitx != 1.0)
        my_snprintf(sy, S(sy), "%.4g%c", gr->unity * yval, gr->unity_suffix);
      else
        my_strncpy(sy, dtoa_eng(yval), S(sy));
  
      tclvareval("set measure_text \"y=", sy, "\nx=", sx, "\"", NULL);
      tcleval("graph_show_measure");
    } /* if(xctx->graph_flags & 64) */

    gr->master_gx1 = gr->gx1;
    gr->master_gx2 = gr->gx2;
    gr->master_gw = gr->gw;
    gr->master_cx = gr->cx;
    /* move hcursor1 */
    if(event == MotionNotify && (state & Button1Mask) && (xctx->graph_flags & 512 )) {
      double c;
      
      c = G_Y(xctx->mousey);
      if(gr->logy) c = pow(10, c);
      my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "hcursor1_y", dtoa(c)));
      need_redraw_master = 1;
    }   

    /* move hcursor2 */
    else if(event == MotionNotify && (state & Button1Mask) && (xctx->graph_flags & 1024 )) {
      double c;
    
      c = G_Y(xctx->mousey);
      if(gr->logy) c = pow(10, c);
      my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "hcursor2_y", dtoa(c)));
      need_redraw_master = 1;
    }   

    /* move cursor1 */
    /* set cursor position from master graph x-axis */
    else if(event == MotionNotify && (state & Button1Mask) && (xctx->graph_flags & 16 )) {
      double c;

      c = G_X(xctx->mousex);
      if(gr->logx) c = pow(10, c);
      if(r->flags & 4) { /* private_cursor */
        my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "cursor1_x", dtoa(c)));
      } else {
        xctx->graph_cursor1_x = c;
      }
      need_all_redraw = 1;
    }
    /* move cursor2 */
    /* set cursor position from master graph x-axis */
    else if(event == MotionNotify && (state & Button1Mask) && (xctx->graph_flags & 32 )) {
      double c;
      int floaters = there_are_floaters();

      c = G_X(xctx->mousex);
      if(gr->logx) c = pow(10, c);
      if(r->flags & 4) { /* private_cursor */
        my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "cursor2_x", dtoa(c)));
      } else {
        xctx->graph_cursor2_x = c; 
      }       
      if(tclgetboolvar("live_cursor2_backannotate")) {
        backannotate_at_cursor_b_pos(r, gr);
        if(floaters) set_modify(-2); /* update floater caches to reflect actual backannotation */
        need_fullredraw = 1;
      } else {
        need_all_redraw = 1;
      }
    }

    if(xctx->ui_state & GRAPHPAN) goto finish; /* After GRAPHPAN only need to check Motion events for cursors */
    if(xctx->mousey_snap < W_Y(gr->gy2)) {
      xctx->graph_top = 1;
    } else {
      xctx->graph_top = 0;
    }
    if(xctx->mousex_snap < W_X(gr->gx1)) {
      xctx->graph_left = 1;
    } else {
      xctx->graph_left = 0;
    }
    if(xctx->mousey_snap > W_Y(gr->gy1)) {
      xctx->graph_bottom = 1;
    } else {
      xctx->graph_bottom = 0;
    }
    zoom_m = (xctx->mousex  - gr->x1) / gr->w;
    if(event == ButtonPress && button == Button1) {
      /* dragging cursors when mouse is very close */
      if(xctx->graph_flags & 128) { /* hcursor1 */
        double cursor;
        cursor = gr->hcursor1_y;
        if(gr->logy ) { 
          cursor = mylog10(cursor);
        } 
        if(fabs(xctx->mousey - W_Y(cursor)) < 10) {
          xctx->graph_flags |= 512; /* Start move hcursor1 */
        }
      } 
      if(xctx->graph_flags & 256) { /* hcursor2 */
        double cursor;
        cursor = gr->hcursor2_y;
        if(gr->logy ) {
          cursor = mylog10(cursor);
        }
        if(fabs(xctx->mousey - W_Y(cursor)) < 10) {
          xctx->graph_flags |= 1024; /* Start move hcursor2 */
        }
      }
      if(xctx->graph_flags & 2) { /* cursor1 */
        double cursor1;
        if(r->flags & 4) { /* private_cursor */
          const char *s = get_tok_value(r->prop_ptr, "cursor1_x", 0);
          if(s[0]) {
            cursor1 = atof_eng(s);
          } else {
            cursor1 = xctx->graph_cursor1_x;
          }
        } else {
          cursor1 = xctx->graph_cursor1_x;
        }
        if(gr->logx ) {
          cursor1 = mylog10(cursor1);
        }
        if(fabs(xctx->mousex - W_X(cursor1)) < 10) {
          xctx->graph_flags |= 16; /* Start move cursor1 */
        }
      }
      if(xctx->graph_flags & 4) { /* cursor2 */
        double cursor2;
        if(r->flags & 4) { /* private_cursor */
          const char *s = get_tok_value(r->prop_ptr, "cursor2_x", 0);
          if(s[0]) {
            cursor2 = atof_eng(s);
          } else {
            cursor2 = xctx->graph_cursor2_x;
          }
        } else { 
          cursor2 = xctx->graph_cursor2_x;
        }
        if(gr->logx) {
          cursor2 = mylog10(cursor2);
        }
        if(fabs(xctx->mousex - W_X(cursor2)) < 10) {
          xctx->graph_flags |= 32; /* Start move cursor2 */
        }
      }
    }
    else if(event == ButtonPress && button == Button3) {
      /* Numerically set cursor position */
      if(xctx->graph_flags & 2) {
        double logcursor, cursor;
        if(r->flags & 4) { /* private_cursor */
          const char *s = get_tok_value(r->prop_ptr, "cursor1_x", 0);
          if(s[0]) {
            cursor = atof_spice(s);
          } else {
            cursor = xctx->graph_cursor1_x;
          }
        } else {
          cursor = xctx->graph_cursor1_x;
        }
        logcursor = cursor;
        if(gr->logx ) {
          logcursor = mylog10(cursor);
        }
        if(fabs(xctx->mousex - W_X(logcursor)) < 10) {
          tclvareval("input_line {Pos:} {} ", dtoa_eng(cursor), NULL);
          cursor = atof_eng(tclresult());
          if(r->flags & 4) {
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "cursor1_x", dtoa(cursor)));
          } else {
            xctx->graph_cursor1_x = cursor;
          }
          event = 0; button = 0; /* avoid further processing ButtonPress that might set GRAPHPAN */
        }
        need_all_redraw = 1;
      }
      /* Numerically set cursor position  *** DO NOT PUT AN `else if` BELOW *** */
      if(xctx->graph_flags & 4) {
        double logcursor, cursor;
        int floaters = there_are_floaters();
        if(r->flags & 4) { /* private_cursor */
          const char *s = get_tok_value(r->prop_ptr, "cursor2_x", 0);
          if(s[0]) {
            cursor = atof_spice(s);
          } else {
            cursor = xctx->graph_cursor2_x;
          }
        } else {
          cursor = xctx->graph_cursor2_x;
        }
        logcursor = cursor;
        if(gr->logx) {
          logcursor = mylog10(cursor);
        }
        if(fabs(xctx->mousex - W_X(logcursor)) < 10) {
          tclvareval("input_line {Pos:} {} ", dtoa_eng(cursor), NULL);
          cursor = atof_eng(tclresult());
          if(r->flags & 4) {
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "cursor2_x", dtoa(cursor)));
          } else {
            xctx->graph_cursor2_x = cursor;
          }
          event = 0; button = 0; /* avoid further processing ButtonPress that might set GRAPHPAN */
        }
        if(tclgetboolvar("live_cursor2_backannotate")) {
          backannotate_at_cursor_b_pos(r, gr);
          if(floaters) set_modify(-2); /* update floater caches to reflect actual backannotation */
          need_fullredraw = 1;
        } else {
          need_all_redraw = 1;
        }
      }
      /* Numerically set hcursor position  *** DO NOT PUT AN `else if` BELOW *** */
      if(xctx->graph_flags & 128) {
        double logcursor, cursor;
        logcursor = cursor = gr->hcursor1_y;
        if(gr->logy ) {
          logcursor = mylog10(cursor);
        }
        if(fabs(xctx->mousey - W_Y(logcursor)) < 10) {
          tclvareval("input_line {Pos:} {} ", dtoa_eng(cursor), NULL);
          cursor = atof_eng(tclresult());
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "hcursor1_y", dtoa(cursor)));
          event = 0; button = 0; /* avoid further processing ButtonPress that might set GRAPHPAN */
        }
        need_redraw_master = 1;
      }
      /* Numerically set hcursor position *** DO NOT PUT AN `else if` BELOW *** */
      if(xctx->graph_flags & 256) {
        double logcursor, cursor;
        logcursor = cursor = gr->hcursor2_y;
        if(gr->logy ) {
          logcursor = mylog10(cursor);
        }
        if(fabs(xctx->mousey - W_Y(logcursor)) < 10) {
          tclvareval("input_line {Pos:} {} ", dtoa_eng(cursor), NULL);
          cursor = atof_eng(tclresult());
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "hcursor2_y", dtoa(cursor)));
          event = 0; button = 0; /* avoid further processing ButtonPress that might set GRAPHPAN */
        }
        need_redraw_master = 1;
      }
    }
    else if(event == -3 && button == Button1) {
      if(!edit_wave_attributes(1, i, gr)) {
        tclvareval("graph_edit_properties ", my_itoa(i), NULL);
      }
    }
    /* x cursor1 toggle */
    else if(key == 'a' && access_cond) {
      xctx->graph_flags ^= 2;
      need_all_redraw = 1;
      if(xctx->graph_flags & 2) {
        double c = G_X(xctx->mousex);

        if(gr->logx) c = pow(10, c);
        if(r->flags & 4) {
          if(!get_tok_value(r->prop_ptr, "cursor1_x", 0)[0]) {
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "cursor1_x", dtoa(c)));
          }
        } else {
          xctx->graph_cursor1_x = c;
        }
      }
    }
    /* x cursor2 toggle */
    else if(key == 'b'  && access_cond) {
      int floaters = there_are_floaters();

      xctx->graph_flags ^= 4;
      if(xctx->graph_flags & 4) {
        double c = G_X(xctx->mousex);

        if(gr->logx) c = pow(10, c);
        if(r->flags & 4) {
          if(!get_tok_value(r->prop_ptr, "cursor2_x", 0)[0]) {
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "cursor2_x", dtoa(c)));
          }
        } else {
          xctx->graph_cursor2_x = c;
        }
        if(tclgetboolvar("live_cursor2_backannotate")) {
          backannotate_at_cursor_b_pos(r, gr);
          if(floaters) set_modify(-2); /* update floater caches to reflect actual backannotation */
          need_fullredraw = 1;
        } else {
          need_all_redraw = 1;
        }
      } else if(xctx->raw) {
        xctx->raw->annot_p = -1;
        xctx->raw->annot_sweep_idx = -1;
        /* need_all_redraw = 1; */
        need_fullredraw = 1;
      }
    }
    /* swap cursors */
    else if((key == 's' && access_cond) ) {
      double tmp, cursor1, cursor2;
      int floaters = there_are_floaters();

      if(r->flags & 4) { /* private_cursor */
        const char *s = get_tok_value(r->prop_ptr, "cursor1_x", 0);
        if(s[0]) {
          cursor1 = atof_spice(s);
        } else {
          cursor1 = xctx->graph_cursor1_x;
        }
      } else {
        cursor1 = xctx->graph_cursor1_x;
      }

      if(r->flags & 4) { /* private_cursor */
        const char *s = get_tok_value(r->prop_ptr, "cursor2_x", 0);
        if(s[0]) { 
          cursor2 = atof_spice(s);
        } else { 
          cursor2 = xctx->graph_cursor2_x;
        } 
      } else { 
        cursor2 = xctx->graph_cursor2_x;
      }

      tmp = cursor2;
      cursor2 = cursor1;
      cursor1 = tmp;

      if(r->flags & 4) {
        my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "cursor1_x", dtoa(cursor1)));
      } else {
        xctx->graph_cursor1_x = cursor1;
      }
      if(r->flags & 4) {
        my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "cursor2_x", dtoa(cursor2)));
      } else {
        xctx->graph_cursor2_x = cursor2;
      }
      if(tclgetboolvar("live_cursor2_backannotate")) {
        backannotate_at_cursor_b_pos(r, gr);
        if(floaters) set_modify(-2); /* update floater caches to reflect actual backannotation */
        need_fullredraw = 1;
      }
      else need_all_redraw = 1;
    }
    /* measurement tooltip */
    else if((key == 'm') && access_cond) {
      xctx->graph_flags ^= 64;
      if(!(xctx->graph_flags & 64)) {
        tcleval("graph_show_measure stop");
      }
    }
    else if(key == 't' && access_cond) {
      if(!gr->digital) {
          const char *d = get_tok_value(r->prop_ptr, "dataset", 0);
        if(d[0]) {
          track_dset = atoi(d);
        } else {
          track_dset = -1; /* all datasets */
        }
        if(track_dset < 0) {
          track_dset = find_closest_wave(i, gr);
        } else {
          track_dset = -1; /* all datasets */
        }
      }
    } /* key == 't' */
  } /* if((i = xctx->graph_master) >= 0 && ((r = &xctx->rect[GRIDLAYER][i])->flags & 1)) */

  /* save mouse position when doing pan operations */
  if(
      ( event == ButtonPress && (button == Button1 || button == Button3)) &&
      !(xctx->ui_state & GRAPHPAN) && 
      !xctx->graph_top /* && !xctx->graph_bottom */
    ) {
    xctx->ui_state |= GRAPHPAN;
    if(!xctx->graph_left) xctx->mx_double_save = xctx->mousex_snap;
    if(xctx->graph_left) xctx->my_double_save = xctx->mousey_snap;
  }
  dbg(1, "graph_master=%d\n", xctx->graph_master);

  finish:
  
  /* parameters for absolute positioning by mouse drag in bottom graph area */
  if( xctx->raw && event == MotionNotify && (state & Button1Mask) && xctx->graph_bottom ) {
    int idx;
    int dset;
    double wwx1, wwx2, pp, delta, ccx, ddx;

    char *rawfile = NULL;
    char *sim_type = NULL;
    int switched = 0;

    my_strdup2(_ALLOC_ID_, &rawfile, get_tok_value(r->prop_ptr, "rawfile", 0));
    my_strdup2(_ALLOC_ID_, &sim_type, get_tok_value(r->prop_ptr, "sim_type", 0));
    if(rawfile[0] && sim_type[0]) switched = extra_rawfile(2, rawfile, sim_type, -1.0, -1.0);
    my_free(_ALLOC_ID_, &rawfile);
    my_free(_ALLOC_ID_, &sim_type);

    idx = get_raw_index(find_nth(get_tok_value(r->prop_ptr, "sweep", 0), ", ", "\"", 0, 1), NULL);
    dset = dataset == -1 ? 0 : dataset;

    if(idx < 0 ) idx = 0;
    delta = gr->gw;
    wwx1 =  get_raw_value(dset, idx, 0);
    wwx2 = get_raw_value(dset, idx, xctx->raw->npoints[dset] - 1);
    if(wwx1 == wwx2) wwx2 += 1e-6;
    if(gr->logx) {
      wwx1 = mylog10(wwx1);
      wwx2 = mylog10(wwx2);
    }
    ccx = (gr->x2 - gr->x1) / (wwx2 - wwx1);
    ddx = gr->x1 - wwx1 * ccx;
    pp = (xctx->mousex_snap - ddx) / ccx;
    xx1 = pp - delta / 2.0;
    xx2 = pp + delta / 2.0;
    if(switched) extra_rawfile(5, NULL, NULL, -1.0, -1.0); /* switch back to previous raw file */
  }
  else if(button == Button3 && (xctx->ui_state & GRAPHPAN) && !xctx->graph_left && !xctx->graph_top) {
    /* parameters for zoom area by mouse drag */
    xx1 = G_X(xctx->mx_double_save);
    xx2 = G_X(xctx->mousex_snap);
    if(state & ShiftMask) {
      if(xx1 < xx2) { double tmp; tmp = xx1; xx1 = xx2; xx2 = tmp; }
    } else {
      if(xx2 < xx1) { double tmp; tmp = xx1; xx1 = xx2; xx2 = tmp; }
    }

    if(xx1 == xx2) xx2 += 1e-6;
  }
  /* loop: after having operated on the master graph do the others */
  for(i=0; i< xctx->rects[GRIDLAYER]; ++i) {
    int same_sim_type = 0;
    char *curr_sim_type = NULL;
    r = &xctx->rect[GRIDLAYER][i];
    my_strdup2(_ALLOC_ID_, &curr_sim_type, get_tok_value(r->prop_ptr, "sim_type", 0));
    need_redraw = 0;
    if( !(r->flags & 1) ) continue; /* 1: graph; 3: graph_unlocked */
    gr->gx1 = gr->master_gx1;
    gr->gx2 = gr->master_gx2;
    gr->gw = gr->master_gw;
    setup_graph_data(i, 1, gr); /* skip flag set, no reload x1 and x2 fields */
    if(gr->dataset >= 0 /* && gr->dataset < xctx->raw->datasets */) dataset =gr->dataset;
    else dataset = -1;

    if(!strcmp(curr_sim_type,
          get_tok_value(xctx->rect[GRIDLAYER][xctx->graph_master].prop_ptr, "sim_type", 0))) {
      same_sim_type = 1;
    }
    my_free(_ALLOC_ID_, &curr_sim_type);
    
    if(event == MotionNotify && (state & Button1Mask) && !xctx->graph_bottom && 
      !(xctx->graph_flags & (16 | 32 | 512 | 1024))) {
      double delta;
      /* vertical move of waveforms */
      if(xctx->graph_left) {
        if(i == xctx->graph_master) {
          if(gr->digital) {
            delta = gr->posh;
            delta_threshold = 0.01;
            if(fabs(xctx->my_double_save - xctx->mousey_snap) > fabs(gr->dcy * delta) * delta_threshold) {
              yy1 = gr->ypos1 + (xctx->my_double_save - xctx->mousey_snap) / gr->dcy;
              yy2 = gr->ypos2 + (xctx->my_double_save - xctx->mousey_snap) / gr->dcy;
              my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "ypos1", dtoa(yy1)));
              my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "ypos2", dtoa(yy2)));
              xctx->my_double_save = xctx->mousey_snap;
              need_redraw = 1;
            }
          } else {
            delta = gr->gh / gr->divy;
            delta_threshold = 0.01;
            if(fabs(xctx->my_double_save - xctx->mousey_snap) > fabs(gr->cy * delta) * delta_threshold) {
              yy1 = gr->gy1 + (xctx->my_double_save - xctx->mousey_snap) / gr->cy;
              yy2 = gr->gy2 + (xctx->my_double_save - xctx->mousey_snap) / gr->cy;
              my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
              my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
              xctx->my_double_save = xctx->mousey_snap;
              need_redraw = 1;
            }
          }
        }
      }
      /* horizontal move of waveforms */
      else {
        save_mouse_at_end = 1;
        delta = gr->gw;
        delta_threshold = 0.01;
        /* selected or locked or master */
        if( r->sel || (same_sim_type && !(r->flags & 2)) || i == xctx->graph_master) {
          dbg(1, "moving waves: %d\n", i);
          if(fabs(xctx->mx_double_save - xctx->mousex_snap) > fabs(gr->cx * delta) * delta_threshold) {
            xx1 = gr->gx1 + (xctx->mx_double_save - xctx->mousex_snap) / gr->cx;
            xx2 = gr->gx2 + (xctx->mx_double_save - xctx->mousex_snap) / gr->cx;
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
            need_redraw = 1;
          }
        }
      }
    }

    else if(event == ButtonPress && button == Button5 && !(state & ShiftMask)) {
      double delta;
      /* vertical move of waveforms with mouse wheel */
      if(xctx->graph_left) {
        if(i == xctx->graph_master) {
          if(gr->digital) {
            delta = gr->posh * 0.05;
            yy1 = gr->ypos1 + delta;
            yy2 = gr->ypos2 + delta;
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "ypos1", dtoa(yy1)));
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "ypos2", dtoa(yy2)));
            need_redraw = 1;
          } else {
            delta = gr->gh/ gr->divy;
            delta_threshold = 1.0;
            yy1 = gr->gy1 + delta * delta_threshold;
            yy2 = gr->gy2 + delta * delta_threshold;
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
            need_redraw = 1;
          }
        }
      }
      /* horizontal move of waveforms with mouse wheel */
      else {
        /* selected or locked or master */
        if( r->sel || (same_sim_type && !(r->flags & 2)) || i == xctx->graph_master) {
          delta = gr->gw;
          delta_threshold = 0.05;
          xx1 = gr->gx1 - delta * delta_threshold;
          xx2 =gr->gx2 - delta * delta_threshold;
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
          need_redraw = 1;
        }
      }
    }
    else if(event == ButtonPress && button == Button4 && !(state & ShiftMask))  {
      double delta;
      /* vertical move of waveforms with mouse wheel */
      if(xctx->graph_left) {
        if(i == xctx->graph_master) {
          if(gr->digital) {
            delta = gr->posh * 0.05;
            yy1 = gr->ypos1 - delta;
            yy2 = gr->ypos2 - delta;
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "ypos1", dtoa(yy1)));
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "ypos2", dtoa(yy2)));
            need_redraw = 1;
          } else {
            delta = gr->gh / gr->divy;
            delta_threshold = 1.0;
            yy1 = gr->gy1 - delta * delta_threshold;
            yy2 = gr->gy2 - delta * delta_threshold;
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
            need_redraw = 1;
          }
        }
      }
      /* horizontal move of waveforms with mouse wheel */
      else {
        /* selected or locked or master */
        if(r->sel || (same_sim_type && !(r->flags & 2)) || i == xctx->graph_master) {
          delta = gr->gw;
          delta_threshold = 0.05;
          xx1 = gr->gx1 + delta * delta_threshold;
          xx2 = gr->gx2 + delta * delta_threshold;
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
          need_redraw = 1;
        }
      }
    }
    else if(event == ButtonPress && button == Button5 && (state & ShiftMask)) {
      if(xctx->graph_left) {
        if(i == xctx->graph_master) {
          if(gr->digital) {
            double m = DG_Y(xctx->mousey);
            double a = m - gr->ypos1;
            double b = gr->ypos2 -m;
            double delta = gr->posh;
            double var = delta * 0.05;
            yy2 = gr->ypos2 + var * b / delta;
            yy1 = gr->ypos1 - var * a / delta;
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "ypos1", dtoa(yy1)));
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "ypos2", dtoa(yy2)));
            need_redraw = 1;

          } else {
            double m = G_Y(xctx->mousey);
            double a = m - gr->gy1;
            double b = gr->gy2 -m;
            double delta = (gr->gy2 - gr->gy1);
            double var = delta * 0.2;
            yy2 = gr->gy2 + var * b / delta;
            yy1 = gr->gy1 - var * a / delta;
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
            need_redraw = 1;
          }
        }
      } else {
        /* selected or locked or master */
        if(r->sel || (same_sim_type && !(r->flags & 2)) || i == xctx->graph_master) {
          double var = 0.2 * gr->gw;
          xx2 = gr->gx2 + var * (1 - zoom_m);
          xx1 = gr->gx1 - var * zoom_m;
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
          need_redraw = 1;
        }
      }
    }
    else if(event == ButtonPress && button == Button4 && (state & ShiftMask)) {
      if(xctx->graph_left) {
        if(i == xctx->graph_master) {
          if(gr->digital) {
            double m = DG_Y(xctx->mousey);
            double a = m - gr->ypos1;
            double b = gr->ypos2 -m;
            double delta = (gr->ypos2 - gr->ypos1);
            double var = delta * 0.05;
            yy2 = gr->ypos2 - var * b / delta;
            yy1 = gr->ypos1 + var * a / delta;
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "ypos1", dtoa(yy1)));
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "ypos2", dtoa(yy2)));
            need_redraw = 1;
          } else {
            double m = G_Y(xctx->mousey);
            double a = m - gr->gy1;
            double b = gr->gy2 -m;
            double delta = (gr->gy2 - gr->gy1);
            double var = delta * 0.2;
            yy2 = gr->gy2 - var * b / delta;
            yy1 = gr->gy1 + var * a / delta;
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
            need_redraw = 1;
          }
        }
      } else {
        /* selected or locked or master */
        if(r->sel || (same_sim_type && !(r->flags & 2)) || i == xctx->graph_master) {
          double var = 0.2 * gr->gw;
          xx2 = gr->gx2 - var * (1 - zoom_m);
          xx1 = gr->gx1 + var * zoom_m;
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
          need_redraw = 1;
        }
      }
    }
    /* y hcursor1 toggle */
    else if(event == KeyPress && key == 'A' && access_cond && i == xctx->graph_master) {
      xctx->graph_flags ^= 128;
      need_redraw = 1;
      if(xctx->graph_flags & 128) {
        double c = G_Y(xctx->mousey);
        if(gr->logy) c = pow(10, c);
        my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "hcursor1_y", dtoa(c)));
      } else {
        my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "hcursor1_y", NULL));
      }
    }
    /* y hcursor2 toggle */
    else if(event == KeyPress && key == 'B' && access_cond && i == xctx->graph_master) {
      xctx->graph_flags ^= 256;
      need_redraw = 1;
      if(xctx->graph_flags & 256) {
        double c = G_Y(xctx->mousey);
        if(gr->logy) c = pow(10, c);
        my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "hcursor2_y", dtoa(c)));
      } else {
        my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "hcursor2_y", NULL));
      }
    }
    else if(event == KeyPress && key == 't' && access_cond ) {
      if(track_dset != -2) { /* -2 means no dataset selection ('t' key) was started */
        /* 
        const char *unlocked = strstr(get_tok_value(r->prop_ptr, "flags", 0), "unlocked");
        */
        int unlocked = r->flags & 2;
        int floaters = there_are_floaters();
        if(i == xctx->graph_master || !unlocked) {
          gr->dataset = track_dset;
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "dataset", my_itoa(track_dset)));
          
        }
       /* do this here to update texts printing current dataset in graph
        *     tcleval([xschem getprop rect 2 n dataset]) */
        if(i == xctx->graph_master && floaters) {
          set_modify(-2); /* update floater caches to reflect actual backannotation */
          need_fullredraw = 1;
        }
        if((xctx->graph_flags & 4)  && tclgetboolvar("live_cursor2_backannotate")) {
          if(i == xctx->graph_master) {
            backannotate_at_cursor_b_pos(r, gr);
          }
          need_fullredraw = 1;
        } else {
          if(!need_fullredraw) need_redraw = 1;
        }

      }
    } /* key == 't' */
    else if(event == KeyPress && key == XK_Left) {
      double delta;
      if(xctx->graph_left) {
        if(!gr->digital && i == xctx->graph_master) {
          double m = G_Y(xctx->mousey);
          double a = m - gr->gy1;
          double b = gr->gy2 -m;
          double delta = gr->gh;
          double var = delta * 0.2;
          yy2 = gr->gy2 + var * b / delta;
          yy1 = gr->gy1 - var * a / delta;
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
          need_redraw = 1;
        }
      } else {
        if(r->sel || (same_sim_type && !(r->flags & 2)) || i == xctx->graph_master) {
          delta = gr->gw;
          delta_threshold = 0.05;
          xx1 = gr->gx1 - delta * delta_threshold;
          xx2 = gr->gx2 - delta * delta_threshold;
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
          need_redraw = 1;
        }
      }
    }
    else if(event == KeyPress && key == XK_Right) {
      double delta;
      if(xctx->graph_left) {
        if(!gr->digital && i == xctx->graph_master) {
          double m = G_Y(xctx->mousey);
          double a = m - gr->gy1;
          double b = gr->gy2 -m;
          double delta = gr->gh;
          double var = delta * 0.2;
          yy2 = gr->gy2 - var * b / delta;
          yy1 = gr->gy1 + var * a / delta;
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
          need_redraw = 1;
        }
      } else {
        if(r->sel || (same_sim_type && !(r->flags & 2)) || i == xctx->graph_master) {
          delta = gr->gw;
          delta_threshold = 0.05;
          xx1 = gr->gx1 + delta * delta_threshold;
          xx2 = gr->gx2 + delta * delta_threshold;
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
          need_redraw = 1;
        }
      }
    }
    else if(event == KeyPress && key == XK_Down) {
      if(!xctx->graph_left) {
        /* selected or locked or master */
        if(r->sel || !(r->flags & 2) || i == xctx->graph_master) {
          double var = 0.2 * gr->gw;
          xx2 = gr->gx2 + var * (1 - zoom_m);
          xx1 = gr->gx1 - var * zoom_m;
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
          need_redraw = 1;
        }
      }
    }
    else if(event == KeyPress && key == XK_Up) {
      if(!xctx->graph_left) {
        /* selected or locked or master */
        if(r->sel || !(r->flags & 2) || i == xctx->graph_master) {
          double var = 0.2 * gr->gw;
          xx2 = gr->gx2 - var * (1 - zoom_m);
          xx1 = gr->gx1 + var * zoom_m;
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
          need_redraw = 1;
        }
      }
    }
    else if(event == KeyPress && key == 'f' && access_cond) {
      if(xctx->raw && xctx->raw->values) {
        if(xctx->graph_left) { /* full Y zoom*/
          if(i == xctx->graph_master) {
            need_redraw = graph_fullyzoom(r, gr, dataset);
          } /* graph_master */
        } else { /* not graph_left, full X zoom*/ 
          /* selected or locked or master */
          if(r->sel || (same_sim_type && !(r->flags & 2)) || i == xctx->graph_master) {
            need_redraw = graph_fullxzoom(i, gr, dataset);
          }
        }
      } /* raw->values */
    } /* key == 'f' */
    /* absolute positioning by mouse drag in bottom graph area */
    else if(event == MotionNotify && (state & Button1Mask) && xctx->graph_bottom ) {
      if(xctx->raw && xctx->raw->values) {
        /* selected or locked or master */
        if(r->sel || (same_sim_type && !(r->flags & 2)) || i == xctx->graph_master) {

          /* xx1 and xx2 calculated for master graph above */
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
          my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
          need_redraw = 1;
        }
      }
    }


    else if(event == ButtonRelease) { 
      if(button != Button3) {
        xctx->ui_state &= ~GRAPHPAN;
        xctx->graph_flags &= ~(16 | 32 | 512 | 1024); /* clear move cursor flags */
      }
      /* zoom X area by mouse drag */
      else if(button == Button3 && (xctx->ui_state & GRAPHPAN) && 
              !xctx->graph_left && !xctx->graph_top) {
        /* selected or locked or master */
        if(r->sel || (same_sim_type && !(r->flags & 2)) || i == xctx->graph_master) {
          if(xctx->mx_double_save != xctx->mousex_snap) {
            clear_graphpan_at_end = 1;

            /* xx1 and xx2 calculated for master graph above */
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
            my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
            need_redraw = 1;
          } else if(i == xctx->graph_master) {
            clear_graphpan_at_end = 1;
          }
        }
      }
      /* zoom Y area by mouse drag */
      else if(button == Button3 && (xctx->ui_state & GRAPHPAN) &&
              xctx->graph_left && !xctx->graph_top) {
        /* Only on master */
        if(i == xctx->graph_master) {
          if(xctx->my_double_save != xctx->mousey_snap) {
            double yy1, yy2;
            clear_graphpan_at_end = 1;
            if(!gr->digital) {
              yy1 = G_Y(xctx->my_double_save);
              yy2 = G_Y(xctx->mousey_snap);
              if(state & ShiftMask) {
                if(yy1 < yy2) { double tmp; tmp = yy1; yy1 = yy2; yy2 = tmp; }
              } else {
                if(yy2 < yy1) { double tmp; tmp = yy1; yy1 = yy2; yy2 = tmp; }
              }
              my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
              my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
            } else {
              yy1 = DG_Y(xctx->my_double_save);
              yy2 = DG_Y(xctx->mousey_snap);
              if(state & ShiftMask) {
                if(yy1 < yy2) { double tmp; tmp = yy1; yy1 = yy2; yy2 = tmp; }
              } else {
                if(yy2 < yy1) { double tmp; tmp = yy1; yy1 = yy2; yy2 = tmp; }
              }
              my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "ypos1", dtoa(yy1)));
              my_strdup(_ALLOC_ID_, &r->prop_ptr, subst_token(r->prop_ptr, "ypos2", dtoa(yy2)));
            }
            need_redraw = 1;
          } else if(i == xctx->graph_master) {
            clear_graphpan_at_end = 1;
          }
        }
      }
    } /* else if( event == ButtonRelease) */
    if(need_redraw || need_all_redraw || ( i == xctx->graph_master && need_redraw_master) ) {
      setup_graph_data(i, 0, gr);
      draw_graph(i, 1 + 8 + (xctx->graph_flags & (4 | 2 | 128 | 256)), gr, NULL); /* draw data in each graph box */
    }
  } /* for(i=0; i< xctx->rects[GRIDLAYER]; i++ */

  if(need_fullredraw ==1) {
    draw();
    need_fullredraw = 0;
  }
  if(clear_graphpan_at_end) xctx->ui_state &= ~GRAPHPAN;
  /* update saved mouse position after processing all graphs */
  if(save_mouse_at_end) {
    if( fabs(xctx->mx_double_save - xctx->mousex_snap) > fabs(gr->master_cx * gr->master_gw) * delta_threshold) {
      dbg(1, "save mouse pos\n");
      xctx->mx_double_save = xctx->mousex_snap;
      xctx->my_double_save = xctx->mousey_snap;
    }
  }


  draw_selection(xctx->gc[SELLAYER], 0);
  #if HAS_CAIRO==1
  cairo_restore(xctx->cairo_ctx);
  cairo_restore(xctx->cairo_save_ctx);
  #endif
  return 0;
}

/* what == 3 (+4) : delete and draw (force)
 * what == 1 (+4) : delete (force)
 * what == 2 (+4) : draw (force)
 * what == 4 : force (re)clear and/or (re)draw even if on same point */
void draw_crosshair(int what, int state)
{
  int sdw, sdp;
  int xhair_size = tclgetintvar("crosshair_size");
  double mx, my;
  int changed = 0;
  dbg(1, "draw_crosshair(): what=%d\n", what);
  sdw = xctx->draw_window;
  sdp = xctx->draw_pixmap;

  if(!xctx->mouse_inside) return;
  mx = xctx->mousex_snap;
  my = xctx->mousey_snap;
  if( ( (xctx->ui_state & (MENUSTART | STARTWIRE) ) || xctx->ui_state == 0 ) &&
        (state == ShiftMask) ) {
    /* mouse not changed so closest net or symbol pin unchanged too */
    if(mx == xctx->prev_m_crossx && my == xctx->prev_m_crossy) {
      mx = xctx->prev_crossx; /* get previous one */
      my = xctx->prev_crossy;
    } else {
      /* mouse position changed, so find new closest net or pin */
      find_closest_net_or_symbol_pin(xctx->mousex_snap, xctx->mousey_snap, &mx, &my);
      changed = 1; /* we force a cursor redraw */
      dbg(1, "find\n");
    }
  }
  
  /* no changed closest pin/net, no force, mx,my is not changed. --> do nothing
         |             _____________|                |
         |            |         _____________________|____________________________ */
  if(!changed && !(what & 4) && mx == xctx->prev_crossx && my == xctx->prev_crossy) {
    return;
  }
  dbg(1, "draw %d\n", what);
  xctx->draw_pixmap = 0;
  xctx->draw_window = 1;
  if(what & 1) { /* delete previous */
    if(fix_broken_tiled_fill || !_unix) {
      if(xhair_size) {
        MyXCopyArea(display, xctx->save_pixmap, xctx->window, xctx->gc[0],
             (int)X_TO_SCREEN(xctx->prev_crossx) - 1 * INT_WIDTH(xctx->lw) - xhair_size,
             (int)Y_TO_SCREEN(xctx->prev_crossy) - 1 * INT_WIDTH(xctx->lw) - xhair_size,
             2 * INT_WIDTH(xctx->lw) + 2 * xhair_size,
             2 * INT_WIDTH(xctx->lw) + 2 * xhair_size,
             (int)X_TO_SCREEN(xctx->prev_crossx) - 1 * INT_WIDTH(xctx->lw) - xhair_size, 
             (int)Y_TO_SCREEN(xctx->prev_crossy) - 1 * INT_WIDTH(xctx->lw) - xhair_size);
        MyXCopyArea(display, xctx->save_pixmap, xctx->window, xctx->gc[0],
             (int)X_TO_SCREEN(xctx->prev_crossx) - 1 * INT_WIDTH(xctx->lw) - xhair_size,
             (int)Y_TO_SCREEN(xctx->prev_crossy) - 1 * INT_WIDTH(xctx->lw) - xhair_size,
             2 * INT_WIDTH(xctx->lw) + 2 * xhair_size,
             2 * INT_WIDTH(xctx->lw) + 2 * xhair_size,
             (int)X_TO_SCREEN(xctx->prev_crossx) - 1 * INT_WIDTH(xctx->lw) - xhair_size,
             (int)Y_TO_SCREEN(xctx->prev_crossy) - 1 * INT_WIDTH(xctx->lw) - xhair_size);
      } else { /* full screen span xhair */
        MyXCopyArea(display, xctx->save_pixmap, xctx->window, xctx->gc[0],
             0, (int)Y_TO_SCREEN(xctx->prev_crossy) - 1 * INT_WIDTH(xctx->lw),
             xctx->xrect[0].width, 2 * INT_WIDTH(xctx->lw),
             0, (int)Y_TO_SCREEN(xctx->prev_crossy) - 1 * INT_WIDTH(xctx->lw));
        MyXCopyArea(display, xctx->save_pixmap, xctx->window, xctx->gc[0],
             (int)X_TO_SCREEN(xctx->prev_crossx) - 1 * INT_WIDTH(xctx->lw), 0, 
             2 * INT_WIDTH(xctx->lw), xctx->xrect[0].height,
             (int)X_TO_SCREEN(xctx->prev_crossx) - 1 * INT_WIDTH(xctx->lw), 0);
      }
  
    } else {
      if(xhair_size) {
        draw_xhair_line(xctx->gctiled, xhair_size,
            X_TO_SCREEN(xctx->prev_crossx) - xhair_size,
            Y_TO_SCREEN(xctx->prev_crossy) - xhair_size,
            X_TO_SCREEN(xctx->prev_crossx) + xhair_size,
            Y_TO_SCREEN(xctx->prev_crossy) - xhair_size);
        draw_xhair_line(xctx->gctiled, xhair_size,
            X_TO_SCREEN(xctx->prev_crossx) - xhair_size,
            Y_TO_SCREEN(xctx->prev_crossy) + xhair_size,
            X_TO_SCREEN(xctx->prev_crossx) + xhair_size,
            Y_TO_SCREEN(xctx->prev_crossy) + xhair_size);
        draw_xhair_line(xctx->gctiled, xhair_size,
            X_TO_SCREEN(xctx->prev_crossx) - xhair_size,
            Y_TO_SCREEN(xctx->prev_crossy) - xhair_size,
            X_TO_SCREEN(xctx->prev_crossx) - xhair_size,
            Y_TO_SCREEN(xctx->prev_crossy) + xhair_size);
        draw_xhair_line(xctx->gctiled, xhair_size,
            X_TO_SCREEN(xctx->prev_crossx) + xhair_size,
            Y_TO_SCREEN(xctx->prev_crossy) - xhair_size,
            X_TO_SCREEN(xctx->prev_crossx) + xhair_size,
            Y_TO_SCREEN(xctx->prev_crossy) + xhair_size);
      } else { /* full screen span xhair */
        drawtempline(xctx->gctiled, NOW, X_TO_XSCHEM(xctx->areax1),
             xctx->prev_crossy, X_TO_XSCHEM(xctx->areax2), xctx->prev_crossy);
        drawtempline(xctx->gctiled, NOW, xctx->prev_crossx, Y_TO_XSCHEM(xctx->areay1),
             xctx->prev_crossx, Y_TO_XSCHEM(xctx->areay2));
      }
    }
  }
  if(what & 2) { /* draw new */
    if(xhair_size) {
      draw_xhair_line(xctx->gc[xctx->crosshair_layer], xhair_size,
          X_TO_SCREEN(mx) - xhair_size,
          Y_TO_SCREEN(my) - xhair_size,
          X_TO_SCREEN(mx) + xhair_size,
          Y_TO_SCREEN(my) - xhair_size);
      draw_xhair_line(xctx->gc[xctx->crosshair_layer], xhair_size,
          X_TO_SCREEN(mx) - xhair_size,
          Y_TO_SCREEN(my) + xhair_size,
          X_TO_SCREEN(mx) + xhair_size,
          Y_TO_SCREEN(my) + xhair_size);
      draw_xhair_line(xctx->gc[xctx->crosshair_layer], xhair_size,
          X_TO_SCREEN(mx) - xhair_size,
          Y_TO_SCREEN(my) - xhair_size,
          X_TO_SCREEN(mx) - xhair_size,
          Y_TO_SCREEN(my) + xhair_size);
      draw_xhair_line(xctx->gc[xctx->crosshair_layer], xhair_size,
          X_TO_SCREEN(mx) + xhair_size,
          Y_TO_SCREEN(my) - xhair_size,
          X_TO_SCREEN(mx) + xhair_size,
          Y_TO_SCREEN(my) + xhair_size);
    } else { /* full screen span xhair */
      draw_xhair_line(xctx->gc[xctx->crosshair_layer], xhair_size,
         xctx->areax1, Y_TO_SCREEN(my),
         xctx->areax2, Y_TO_SCREEN(my));
      draw_xhair_line(xctx->gc[xctx->crosshair_layer], xhair_size,
         X_TO_SCREEN(mx), xctx->areay1,
         X_TO_SCREEN(mx), xctx->areay2);
    }
  }
  if(what) draw_selection(xctx->gc[SELLAYER], 0);

  if(what & 2) {
    /* previous closest pin or net position (if snap wire or Shift pressed) */
    xctx->prev_crossx = mx;
    xctx->prev_crossy = my;
    /* previous mouse_snap position */
    xctx->prev_m_crossx = xctx->mousex_snap;
    xctx->prev_m_crossy = xctx->mousey_snap;
  }

  xctx->draw_window = sdw;
  xctx->draw_pixmap = sdp;
}

/* complete the STARTWIRE, STARTRECT, STARTZOOM, STARTCOPY ... operations */
static int end_place_move_copy_zoom()
{
  if(xctx->ui_state & STARTZOOM) {
    zoom_rectangle(END);
    if( xctx->nl_x1 == xctx->nl_x2 && xctx->nl_y1 == xctx->nl_y2) {
      return 0;
    }
    return 1;
  }
  else if(xctx->ui_state & STARTWIRE) {
    if(tclgetboolvar("persistent_command")) {
      if(xctx->constr_mv != 2) {
        xctx->mx_double_save=xctx->mousex_snap;
      }
      if(xctx->constr_mv != 1) {
        xctx->my_double_save=xctx->mousey_snap;
      }
      if(xctx->constr_mv == 1) xctx->mousey_snap = xctx->my_double_save;
      if(xctx->constr_mv == 2) xctx->mousex_snap = xctx->mx_double_save;
      new_wire(PLACE, xctx->mousex_snap, xctx->mousey_snap);

    } else {
      new_wire(PLACE|END, xctx->mousex_snap, xctx->mousey_snap);
    }
    xctx->constr_mv=0;
    tcleval("set constr_mv 0" );
    return 1;
  }
  else if(xctx->ui_state & STARTARC) {
    new_arc(SET, 0, xctx->mousex_snap, xctx->mousey_snap);
    return 1;
  }
  else if(xctx->ui_state & STARTLINE) {
    if(tclgetboolvar("persistent_command")) {
      if(xctx->constr_mv != 2) {
        xctx->mx_double_save=xctx->mousex_snap;
      }
      if(xctx->constr_mv == 1) {
        xctx->my_double_save=xctx->mousey_snap;
      }
      if(xctx->constr_mv == 1) xctx->mousey_snap = xctx->my_double_save;
      if(xctx->constr_mv == 2) xctx->mousex_snap = xctx->mx_double_save;
      new_line(PLACE, xctx->mousex_snap, xctx->mousey_snap);
    } else {
      new_line(PLACE|END, xctx->mousex_snap, xctx->mousey_snap);
    }
    xctx->constr_mv=0;
    tcleval("set constr_mv 0" );
    return 1;
  }
  else if(xctx->ui_state & STARTRECT) {
    new_rect(PLACE|END,xctx->mousex_snap, xctx->mousey_snap);
    return 1;
  }
  else if(xctx->ui_state & STARTPOLYGON) {
    if(xctx->constr_mv == 1) xctx->mousey_snap = xctx->my_double_save;
    if(xctx->constr_mv == 2) xctx->mousex_snap = xctx->mx_double_save;
    new_polygon(ADD, xctx->mousex_snap, xctx->mousey_snap);
    xctx->mx_double_save=xctx->mousex_snap;
    xctx->my_double_save=xctx->mousey_snap;
    xctx->constr_mv=0;
    tcleval("set constr_mv 0" );
    return 1;
  }
  else if(xctx->ui_state & STARTMOVE) {
    move_objects(END,0,0,0);
    xctx->ui_state &=~START_SYMPIN;
    xctx->constr_mv=0;
    tcleval("set constr_mv 0" );
    return 1;
  }
  else if(xctx->ui_state & STARTCOPY) {
    copy_objects(END);
    xctx->constr_mv=0;
    tcleval("set constr_mv 0" );
    return 1;
  }
  return 0;
}

static void unselect_at_mouse_pos(int mx, int my)
{
       xctx->last_command = 0;
       xctx->mx_save = mx; xctx->my_save = my;
       xctx->mx_double_save=xctx->mousex_snap;
       xctx->my_double_save=xctx->mousey_snap;
       select_object(xctx->mousex, xctx->mousey, 0, 0, NULL);
       rebuild_selected_array(); /* sets or clears xctx->ui_state SELECTION flag */
}

void snapped_wire(double c_snap)
{
  double x, y;
  if(!(xctx->ui_state & STARTWIRE)){
    find_closest_net_or_symbol_pin(xctx->mousex, xctx->mousey, &x, &y);
    xctx->mx_double_save = my_round(x / c_snap) * c_snap;
    xctx->my_double_save = my_round(y / c_snap) * c_snap;
    new_wire(PLACE, x, y);
    new_wire(RUBBER, xctx->mousex_snap,xctx->mousey_snap);
  }
  else {
    find_closest_net_or_symbol_pin(xctx->mousex, xctx->mousey, &x, &y);
    new_wire(RUBBER, x, y);
    new_wire(PLACE|END, x, y);
    xctx->constr_mv=0;
    tcleval("set constr_mv 0" );
  }
}

static int check_menu_start_commands(double c_snap, int mx, int my)
{
  dbg(1, "check_menu_start_commands(): ui_state=%x, ui_state2=%x last_command=%d\n", 
      xctx->ui_state, xctx->ui_state2, xctx->last_command);

  if((xctx->ui_state & MENUSTART) && (xctx->ui_state2 & MENUSTARTDESEL) ) {
    if(xctx->ui_state & DESEL_CLICK) {
      unselect_at_mouse_pos(mx, my);
    } else { /* unselect by area */
      xctx->mx_save = mx; xctx->my_save = my;
      xctx->mx_double_save=xctx->mousex;
      xctx->my_double_save=xctx->mousey;
      xctx->ui_state |= DESEL_AREA;
    }
    return 1;
  }
  if((xctx->ui_state & MENUSTART) && (xctx->ui_state2 & MENUSTARTWIRECUT)) {
    break_wires_at_point(xctx->mousex_snap, xctx->mousey_snap, 1);
    return 1;
  }
  else if((xctx->ui_state & MENUSTART) && (xctx->ui_state2 & MENUSTARTWIRECUT2)) {
    break_wires_at_point(xctx->mousex_snap, xctx->mousey_snap, 0);
    return 1;
  }
  else if((xctx->ui_state & MENUSTART) && (xctx->ui_state2 & MENUSTARTMOVE)) {
    xctx->mx_double_save=xctx->mousex_snap;
    xctx->my_double_save=xctx->mousey_snap;
    /* stretch nets that land on selected instance pins if connect_by_kissing == 2 */
    /* select_attached_nets(); */
    move_objects(START,0,0,0);
    return 1;
  }
  else if((xctx->ui_state & MENUSTART) && (xctx->ui_state2 & MENUSTARTCOPY)) {
    xctx->mx_double_save=xctx->mousex_snap;
    xctx->my_double_save=xctx->mousey_snap;
    copy_objects(START);
    return 1;
  }    
  else if((xctx->ui_state & MENUSTART) && (xctx->ui_state2 & MENUSTARTWIRE)) {
    int prev_state = xctx->ui_state;
    if(xctx->semaphore >= 2) return 0;
    start_wire(xctx->mousex_snap, xctx->mousey_snap);
    if(prev_state == STARTWIRE) {
      tcleval("set constr_mv 0" );
      xctx->constr_mv=0;
    }

    /* 
     * xctx->mx_double_save=xctx->mousex_snap;
     * xctx->my_double_save=xctx->mousey_snap;
     * new_wire(PLACE, xctx->mousex_snap, xctx->mousey_snap);
     * xctx->ui_state &=~MENUSTART;
     */
    return 1;
  }
  else if((xctx->ui_state & MENUSTART) && (xctx->ui_state2 & MENUSTARTSNAPWIRE)) {
    snapped_wire(c_snap);
    return 1;
  }
  else if((xctx->ui_state & MENUSTART) && (xctx->ui_state2 & MENUSTARTLINE)) {
    int prev_state = xctx->ui_state;
    if(xctx->semaphore >= 2) return 0;
    start_line(xctx->mousex_snap, xctx->mousey_snap);
    if(prev_state == STARTLINE) {
      tcleval("set constr_mv 0" );
      xctx->constr_mv=0;
    }

    /*
     * xctx->mx_double_save=xctx->mousex_snap;
     * xctx->my_double_save=xctx->mousey_snap;
     * new_line(PLACE, xctx->mousex_snap, xctx->mousey_snap);
     * xctx->ui_state &=~MENUSTART;
     */
    return 1;
  }
  else if((xctx->ui_state & MENUSTART) && (xctx->ui_state2 & MENUSTARTRECT)) {
    xctx->mx_double_save=xctx->mousex_snap;
    xctx->my_double_save=xctx->mousey_snap;
    new_rect(PLACE,xctx->mousex_snap, xctx->mousey_snap);
    return 1;
  }
  else if((xctx->ui_state & MENUSTART) && (xctx->ui_state2 & MENUSTARTPOLYGON)) {
    xctx->mx_double_save=xctx->mousex_snap;
    xctx->my_double_save=xctx->mousey_snap;
    new_polygon(PLACE, xctx->mousex_snap, xctx->mousey_snap);
    return 1;
  }
  else if((xctx->ui_state & MENUSTART) && (xctx->ui_state2 & MENUSTARTARC)) {
    xctx->mx_double_save=xctx->mousex_snap;
    xctx->my_double_save=xctx->mousey_snap;
    new_arc(PLACE, 180., xctx->mousex_snap, xctx->mousey_snap);
    return 1;
  }
  else if((xctx->ui_state & MENUSTART) && (xctx->ui_state2 & MENUSTARTCIRCLE)) {
    xctx->mx_double_save=xctx->mousex_snap;
    xctx->my_double_save=xctx->mousey_snap;
    new_arc(PLACE, 360., xctx->mousex_snap, xctx->mousey_snap);
    return 1;
  }
  else if((xctx->ui_state & MENUSTART) && (xctx->ui_state2 & MENUSTARTZOOM)) {
    zoom_rectangle(START);
    return 1;
  }
  return 0;
}

static int add_wire_from_inst(Selected *sel, double mx, double my)
{
  int res = 0;
  int prev_state = xctx->ui_state;
  int i, type = sel->type;
  double pinx0, piny0;
  if(type == ELEMENT) {
    int n = sel->n;
    xSymbol *symbol = xctx->sym + xctx->inst[n].ptr;
    int npin = symbol->rects[PINLAYER];
    for(i = 0; i < npin; ++i) {
      get_inst_pin_coord(n, i, &pinx0, &piny0);
      if(pinx0 == mx && piny0 == my) {
        break;
      }
    }
    if(i < npin) {
      dbg(1, "pin: %g %g\n", pinx0, piny0);
      unselect_all(1);
      start_wire(xctx->mousex_snap, xctx->mousey_snap); 
      if(prev_state == STARTWIRE) {
        tcleval("set constr_mv 0" );
        xctx->constr_mv=0;
      }
      res = 1;
    }
  }
  return res;
}

static int add_wire_from_wire(Selected *sel, double mx, double my)
{
  int res = 0;
  int prev_state = xctx->ui_state;
  int type = sel->type;
  if(type == WIRE) {
    int n = sel->n;
    double x1 = xctx->wire[n].x1;
    double y1 = xctx->wire[n].y1;
    double x2 = xctx->wire[n].x2;
    double y2 = xctx->wire[n].y2;
    if( (mx == x1 && my == y1) || (mx == x2 && my == y2) ) {
      unselect_all(1); 
      start_wire(xctx->mousex_snap, xctx->mousey_snap); 
      if(prev_state == STARTWIRE) {
        tcleval("set constr_mv 0" );
        xctx->constr_mv=0;
      }
      res = 1;
    }
  }
  return res;
}

/* sets xctx->shape_point_selected */
static int edit_line_point(int state)
{
   int line_n = -1, line_c = -1;
   dbg(1, "1 Line selected\n");
   line_n = xctx->sel_array[0].n;
   line_c = xctx->sel_array[0].col;
  /* lineangle point: Check is user is clicking a control point of a lineangle */
  if(line_n >= 0) {
    xLine *p = &xctx->line[line_c][line_n];

    xctx->need_reb_sel_arr=1;
    if(xctx->mousex_snap == p->x1 && xctx->mousey_snap == p->y1) {
      xctx->shape_point_selected = 1;
      p->sel = SELECTED1;
    }
    else if(xctx->mousex_snap == p->x2 && xctx->mousey_snap == p->y2) {
      xctx->shape_point_selected = 1;
      p->sel = SELECTED2;
    }
    if(xctx->shape_point_selected) {
      /* move one line selected point */
      if(!(state & (ControlMask | ShiftMask))){
        /* xctx->push_undo(); */
        move_objects(START,0,0,0);
        return 1;
      }
    } /* if(xctx->shape_point_selected) */
  } /* if(line_n >= 0) */
  return 0;
}

/* sets xctx->shape_point_selected */
static int edit_wire_point(int state)
{       
   int wire_n = -1;
   dbg(1, "edit_wire_point, ds = %g\n", xctx->cadhalfdotsize);
   wire_n = xctx->sel_array[0].n;
  /* wire point: Check is user is clicking a control point of a wire */
  if(wire_n >= 0) {
    xWire *p = &xctx->wire[wire_n];
        
    xctx->need_reb_sel_arr=1;
    if(xctx->mousex_snap == p->x1 && xctx->mousey_snap == p->y1) {
      xctx->shape_point_selected = 1;
      p->sel = SELECTED1;
    }
    else if(xctx->mousex_snap == p->x2 && xctx->mousey_snap == p->y2) {
      xctx->shape_point_selected = 1;
      p->sel = SELECTED2;
    }
    if(xctx->shape_point_selected) {
      /* move one wire selected point */
      if(!(state & (ControlMask | ShiftMask))){
        /* xctx->push_undo(); */
        move_objects(START,0,0,0);
        return 1;
      }
    } /* if(xctx->shape_point_selected) */
  } /* if(wire_n >= 0) */
  return 0;
}      

/* sets xctx->shape_point_selected */
static int edit_rect_point(int state)
{
   int rect_n = -1, rect_c = -1;
   dbg(1, "1 Rectangle selected\n");
   rect_n = xctx->sel_array[0].n;
   rect_c = xctx->sel_array[0].col;
  /* rectangle point: Check is user is clicking a control point of a rectangle */
  if(rect_n >= 0) {
    double ds = xctx->cadhalfdotsize * 2;
    xRect *p = &xctx->rect[rect_c][rect_n];

    xctx->need_reb_sel_arr=1;
    if(POINTINSIDE(xctx->mousex, xctx->mousey, p->x1, p->y1, p->x1 + ds, p->y1 + ds)) {
      xctx->shape_point_selected = 1;
      p->sel = SELECTED1;
    }
    else if(POINTINSIDE(xctx->mousex, xctx->mousey, p->x2 - ds, p->y1, p->x2, p->y1 + ds)) {
      xctx->shape_point_selected = 1;
      p->sel = SELECTED2;
    }
    else if(POINTINSIDE(xctx->mousex, xctx->mousey, p->x1, p->y2 - ds, p->x1 + ds, p->y2)) {
      xctx->shape_point_selected = 1;
      p->sel = SELECTED3;
    }
    else if(POINTINSIDE(xctx->mousex, xctx->mousey, p->x2 - ds, p->y2 - ds, p->x2, p->y2)) {
      xctx->shape_point_selected = 1;
      p->sel = SELECTED4;
    }
    if(xctx->shape_point_selected) { 
      /* move one rectangle selected point */
      if(!(state & (ControlMask | ShiftMask))){
        /* xctx->push_undo(); */
        move_objects(START,0,0,0);
        return 1;
      }
    } /* if(xctx->shape_point_selected) */
  } /* if(rect_n >= 0) */
  return 0;
}

/* sets xctx->shape_point_selected */
static int edit_polygon_point(int state)
{
   int poly_n = -1, poly_c = -1;
   dbg(1, "1 Polygon selected\n");
   poly_n = xctx->sel_array[0].n;
   poly_c = xctx->sel_array[0].col;
  /* polygon point: Check is user is clicking a control point of a polygon */
  if(poly_n >= 0) {
    int i;
    double ds = xctx->cadhalfdotsize;
    int c = poly_c;
    int n = poly_n;
    xPoly *p = &xctx->poly[c][n];

    xctx->need_reb_sel_arr=1;
    for(i = 0; i < p->points; i++) {
      if(
          POINTINSIDE(xctx->mousex, xctx->mousey, p->x[i] - ds, p->y[i] - ds, 
                        p->x[i] + ds, p->y[i] + ds)
        ) {
          dbg(1, "selecting point %d\n", i);
          p->selected_point[i] = 1;
          xctx->shape_point_selected = 1;
          break;
      }
    }
    if(xctx->shape_point_selected) { 
      int j;
      int points = p->points;

      /* add a new polygon/bezier point after selected one and start moving it*/
      if(state & ShiftMask) {
        xctx->push_undo();
        points++;
        my_realloc(_ALLOC_ID_, &p->x, sizeof(double) * points);
        my_realloc(_ALLOC_ID_, &p->y, sizeof(double) * points);
        my_realloc(_ALLOC_ID_, &p->selected_point, sizeof(unsigned short) * points);
        p->selected_point[i] = 0;
        for(j = points - 2; j > i; j--) {
          p->x[j + 1] = p->x[j];
          p->y[j + 1] = p->y[j];
          p->selected_point[j + 1] = p->selected_point[j];
        }
        p->selected_point[i + 1] = 1;
        p->x[i + 1] = p->x[i];
        p->y[i + 1] = p->y[i];
        p->points = points;
        p->sel = SELECTED1;
        move_objects(START,0,0,0);
        return 1;
      }
      /* delete polygon/bezier selected point */
      else if(points > 2 && state & ControlMask) {
        xctx->push_undo();
        points--;
        for(j = i ; j < points ; j++) {
           p->x[j] = p->x[j + 1];
           p->y[j] = p->y[j + 1];
           p->selected_point[j] = p->selected_point[j + 1];
        }
        my_realloc(_ALLOC_ID_, &p->x, sizeof(double) * points);
        my_realloc(_ALLOC_ID_, &p->y, sizeof(double) * points);
        my_realloc(_ALLOC_ID_, &p->selected_point, sizeof(unsigned short) * points);
        p->points = points;
        p->sel = SELECTED;
        return 1;
      /* move one polygon/bezier selected point */
      } else if(!(state & (ControlMask | ShiftMask))){
        /* xctx->push_undo(); */
        p->sel = SELECTED1;
        move_objects(START,0,0,0);
        return 1;
      }
    } /* if(xctx->shape_point_selected) */
  } /* if(poly_n >= 0) */
  return 0;
}

static void context_menu_action(double mx, double my)
{
  int ret;
  const char *status;
  int prev_state;
  xctx->semaphore++;
  status = tcleval("context_menu");
  xctx->semaphore--;
  if(!status) return;
  ret = atoi(status);
  switch(ret) {
    case 1:
      start_place_symbol();
      break;
    case 2:
      prev_state = xctx->ui_state;
      start_wire(mx, my);
      if(prev_state == STARTWIRE) {
        tcleval("set constr_mv 0" );
        xctx->constr_mv=0;
      }
      break;
    case 3:
      prev_state = xctx->ui_state;
      start_line(mx, my);
      if(prev_state == STARTLINE) {
        tcleval("set constr_mv 0" );
        xctx->constr_mv=0;
      }
      break;
    case 4:
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      xctx->last_command = 0;
      new_rect(PLACE,mx, my);
      break;
    case 5:
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      xctx->last_command = 0;
      new_polygon(PLACE, mx, my);
      break;
    case 6: /* place text */
      xctx->last_command = 0;
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      if(place_text(0, mx, my)) { /* 1 = draw text */
        xctx->mousey_snap = xctx->my_double_save;
        xctx->mousex_snap = xctx->mx_double_save;
        move_objects(START,0,0,0);
        xctx->ui_state |= PLACE_TEXT;
      }
      break;
    case 7: /* cut selection into clipboard */
      rebuild_selected_array();
      if(xctx->lastsel) {  /* 20071203 check if something selected */
        save_selection(2);
        delete(1/* to_push_undo */);
      }
      break;
    case 8: /* paste from clipboard */
      merge_file(2,".sch");
      break;
    case 9: /* load most recent file */
      tclvareval("xschem load -gui [lindex $recentfile 0]", NULL);
      break;
    case 10: /* edit attributes */
      edit_property(0);
      break;
    case 11: /* edit attributes in editor */
      edit_property(1);
      break;
    case 12:
      descend_schematic(0, 1, 1, 1);
      break;
    case 13:
      descend_symbol();
      break;
    case 14:
      go_back(1);
      break;
    case 15: /* copy selection into clipboard */
      rebuild_selected_array();
      if(xctx->lastsel) {
        save_selection(2);
      }
      break;
    case 16: /* move selection */
      if(!(xctx->ui_state & (STARTMOVE | STARTCOPY))) {
        xctx->mx_double_save=xctx->mousex_snap;
        xctx->my_double_save=xctx->mousey_snap;
        move_objects(START,0,0,0);
      }
      break;
    case 17: /* duplicate selection */
      if(!(xctx->ui_state & (STARTMOVE | STARTCOPY))) {
        xctx->mx_double_save=xctx->mousex_snap;
        xctx->my_double_save=xctx->mousey_snap;
        copy_objects(START);
      }
      break;
    case 18: /* delete selection */
      if(xctx->ui_state & SELECTION) delete(1/* to_push_undo */);
      break;
    case 19: /* place arc */
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      xctx->last_command = 0;
      new_arc(PLACE, 180., mx, my);
      break;
    case 20: /* place circle */
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      xctx->last_command = 0;
      new_arc(PLACE, 360., mx, my);
      break;
    case 21: /* abort & redraw */
      abort_operation();
      break;
    default:
      break;
  }
}

/* Mouse wheel events */
static int handle_mouse_wheel(int event, int mx, int my, KeySym key, int button, int aux, int state)
{
   int graph_use_ctrl_key = tclgetboolvar("graph_use_ctrl_key");
   if(button==Button5 && state == 0 ) {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return 1;
    }
     view_unzoom(CADZOOMSTEP);
     return 0;
   }
   else if(button==Button4 && state == 0 ) {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return 1;
    }
     view_zoom(CADZOOMSTEP);
     return 0;
   }
   if(!graph_use_ctrl_key) {
     if(button==Button4 && (state & ShiftMask) && !(state & Button2Mask)) {
      if(waves_selected(event, key, state, button)) {
        waves_callback(event, mx, my, key, button, aux, state);
        return 1;
      }
      xctx->xorigin+=-CADMOVESTEP*xctx->zoom/2.;
      draw();
      redraw_w_a_l_r_p_z_rubbers(1);
     }
     else if(button==Button5 && (state & ShiftMask) && !(state & Button2Mask)) {
      if(waves_selected(event, key, state, button)) {
        waves_callback(event, mx, my, key, button, aux, state);
        return 1;
      }
      xctx->xorigin-=-CADMOVESTEP*xctx->zoom/2.;
      draw();
      redraw_w_a_l_r_p_z_rubbers(1);
     }
     else if(button==Button4 && (state & ControlMask) && !(state & Button2Mask)) {
      xctx->yorigin+=-CADMOVESTEP*xctx->zoom/2.;
      draw();
      redraw_w_a_l_r_p_z_rubbers(1);
     }
     else if(button==Button5 && (state & ControlMask) && !(state & Button2Mask)) {
      xctx->yorigin-=-CADMOVESTEP*xctx->zoom/2.;
      draw();
      redraw_w_a_l_r_p_z_rubbers(1);
     }
   }
   return 0;
}

static void end_shape_point_edit(double c_snap)
{
     int save = xctx->modified;
     double sx, sy;
     dbg(1, "%g %g %g %g\n",
         xctx->mx_double_save, xctx->my_double_save, xctx->mousex_snap, xctx->mousey_snap);
     if(xctx->lastsel == 1 && xctx->sel_array[0].type==POLYGON) {
        int k;
        int n = xctx->sel_array[0].n;
        int c = xctx->sel_array[0].col;
        move_objects(END,0,0,0);
        xctx->constr_mv=0;
        tcleval("set constr_mv 0" );
        xctx->poly[c][n].sel = SELECTED;
        xctx->shape_point_selected = 0;
        for(k=0; k<xctx->poly[c][n].points; ++k) {
          xctx->poly[c][n].selected_point[k] = 0;
        }
        xctx->need_reb_sel_arr=1;
     }
     else if(xctx->lastsel == 1 && xctx->sel_array[0].type==xRECT) {
        int n = xctx->sel_array[0].n;
        int c = xctx->sel_array[0].col;
        move_objects(END,0,0,0);
        xctx->constr_mv=0;
        tcleval("set constr_mv 0" );
        xctx->rect[c][n].sel = SELECTED;
        xctx->shape_point_selected = 0;
        xctx->need_reb_sel_arr=1;
     }
     else if(xctx->lastsel == 1 && xctx->sel_array[0].type==LINE) {
        int n = xctx->sel_array[0].n;
        int c = xctx->sel_array[0].col;
        move_objects(END,0,0,0);
        xctx->constr_mv=0;
        tcleval("set constr_mv 0" );
        xctx->line[c][n].sel = SELECTED;
        xctx->shape_point_selected = 0;
        xctx->need_reb_sel_arr=1;
     }
     else if(xctx->lastsel == 1 && xctx->sel_array[0].type==WIRE) {
        int n = xctx->sel_array[0].n;
        move_objects(END,0,0,0);
        xctx->constr_mv=0;
        tcleval("set constr_mv 0" );
        xctx->wire[n].sel = SELECTED;
        xctx->shape_point_selected = 0;
        xctx->need_reb_sel_arr=1;
     }
     sx = my_round(xctx->mx_double_save / c_snap) * c_snap;
     sy = my_round(xctx->my_double_save / c_snap) * c_snap;

     if(sx == xctx->mousex_snap && sy == xctx->mousey_snap) {  
       set_modify(save);
     }
}

#if defined(__unix__) && HAS_CAIRO==1
static int grabscreen(const char *win_path, int event, int mx, int my, KeySym key,
                 int button, int aux, int state)
{
  static int grab_state = 0;
  static int x1, y1, x2, y2;
  int rmx, rmy, wmx, wmy;
  unsigned int msq;
  Window rw, cw;
  XSetWindowAttributes winattr;
  XGCValues gcv;
  static GC gc = NULL;
  static Window clientwin = 0;
  static int first_motion = 1;
  static int displayh = 0, displayw = 0;
  static unsigned long white = 0;

  if(grab_state == 0 && event == ButtonPress && button == Button1) {
    unsigned long gcvm = GCFunction | GCForeground;

    white = WhitePixel(display, screen_number);
    displayh = DisplayHeight(display, screen_number);
    displayw = DisplayWidth(display, screen_number);

    XQueryPointer(display, xctx->window, &rw, &cw , &rmx, &rmy, &wmx, &wmy, &msq);
    gcv.function = GXxor;
    gcv.foreground = white;
    gc = XCreateGC(display, rw, gcvm, &gcv);

    winattr.override_redirect = True;
    clientwin = XCreateWindow(display, rw, 0, 0, displayw, displayh, 0, screendepth,
                InputOutput, visual, CWOverrideRedirect, &winattr);
    XMapRaised(display,clientwin);

    x1 = rmx;
    y1 = rmy;
    dbg(1, "grabscreen(): got point1: %d %d\n", x1, y1);
    grab_state = 1;  
  }

  if(grab_state == 1 && event == MotionNotify) {
    static int xx1, xx2, yy1, yy2;
    xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
    INT_RECTORDER(xx1, yy1, xx2, yy2);
    dbg(1, "Motion: %d %d %d %d\n", xx1, yy1, xx2, yy2);
    if(!first_motion) {
      XDrawRectangle(display, clientwin, gc, xx1 - 1, yy1 - 1, xx2 - xx1 + 2, yy2 - yy1 + 2);
    }
    first_motion = 0;
    XQueryPointer(display, xctx->window, &rw, &cw , &rmx, &rmy, &wmx, &wmy, &msq);
    x2 = xx2 = rmx;
    y2 = yy2 = rmy;
    xx1 = x1; yy1 = y1;
    INT_RECTORDER(xx1, yy1, xx2, yy2);
    XDrawRectangle(display, clientwin, gc, xx1 - 1, yy1 - 1, xx2 - xx1 + 2, yy2 - yy1 + 2);
  }

  if(grab_state == 1 && event == ButtonRelease) {
    int grab_w = 0, grab_h = 0;
    cairo_surface_t *sfc = NULL, *subsfc = NULL;
    png_to_byte_closure_t closure;
    char *encoded_data = NULL;
    size_t olength;
    char *prop = NULL;


    grab_state = 0;
    first_motion = 1;
    xctx->ui_state &= ~GRABSCREEN;
    XQueryPointer(display, xctx->window, &rw, &cw , &rmx, &rmy, &wmx, &wmy, &msq);
    x2 = rmx;
    y2 = rmy;
    INT_RECTORDER(x1, y1, x2, y2);
    tclvareval("grab release ", xctx->top_path, ".drw", NULL);
    if(x2 - x1 > 10 && y2 -y1 > 10) {
      xctx->push_undo();
      grab_w = (x2 - x1 + 1);
      grab_h = (y2 - y1 + 1);
      dbg(1, "grabscreen(): grab area: %d %d - %d %d\n", x1, y1, x2, y2);
      dbg(1, "grabscreen(): root w=%d, h=%d\n", displayw, displayh);
      sfc =  cairo_xlib_surface_create(display, rw, visual, displayw, displayh);
      if(!sfc || cairo_surface_status(sfc) != CAIRO_STATUS_SUCCESS) {
        dbg(0, "grabscreen(): failure creating sfc\n");
        XFreeGC(display, gc);
        XDestroyWindow(display, clientwin);
        return 0;
      }
      dbg(1, "sfc: w=%d, h=%d\n", 
            cairo_xlib_surface_get_width(sfc),
            cairo_xlib_surface_get_height(sfc));
      subsfc = cairo_surface_create_for_rectangle(sfc, x1, y1, grab_w, grab_h);
      if(!subsfc || cairo_surface_status(subsfc) != CAIRO_STATUS_SUCCESS) {
        dbg(0, "grabscreen(): failure creating subsfc\n");
        cairo_surface_destroy(sfc);
        XFreeGC(display, gc);
        XDestroyWindow(display, clientwin);
        return 0;
      }
      closure.buffer = NULL;
      closure.size = 0;
      closure.pos = 0;
      cairo_surface_write_to_png_stream(subsfc, png_writer, &closure);
      cairo_surface_destroy(subsfc);
      cairo_surface_destroy(sfc);
      closure.size = closure.pos;
      dbg(1, "closure.size = %ld\n", closure.size);
      encoded_data = base64_encode((unsigned char *)closure.buffer, closure.size, &olength, 0);
      dbg(1, "olength = %ld\n", olength);
      my_free(_ALLOC_ID_, &closure.buffer);
      my_mstrcat(_ALLOC_ID_, &prop, "flags=image,unscaled\nalpha=0.8\nimage_data=", encoded_data, NULL);
      my_free(_ALLOC_ID_, &encoded_data);
      storeobject(-1, xctx->mousex_snap, xctx->mousey_snap, xctx->mousex_snap + grab_w, xctx->mousey_snap + grab_h,
                  xRECT, GRIDLAYER, SELECTED, prop);
      my_free(_ALLOC_ID_, &prop);
      xctx->need_reb_sel_arr=1;
      rebuild_selected_array();
      move_objects(START,0,0,0);
      xctx->ui_state |= START_SYMPIN;
    }
    XFreeGC(display, gc);
    XDestroyWindow(display, clientwin);
  }
  return 1;
}
#endif

static void handle_enter_notify(int draw_xhair, int crosshair_size)
{
    struct stat buf;
    dbg(2, "callback(): Enter event, ui_state=%d\n", xctx->ui_state);
    xctx->mouse_inside = 1;
    if(draw_xhair) {
      if(crosshair_size == 0) {
        tclvareval(xctx->top_path, ".drw configure -cursor none" , NULL);
      }
    } else 
      tclvareval(xctx->top_path, ".drw configure -cursor {}" , NULL);
    /* xschem window *sending* selected objects
       when the pointer comes back in abort copy operation since it has been done
       in another xschem xctx->window; STARTCOPY set and selection file does not exist any more */
    if(stat(sel_file, &buf) && (xctx->ui_state & STARTCOPY) )
    {
      copy_objects(ABORT);
      unselect_all(1);
    }
    /* xschem window *receiving* selected objects selection cleared --> abort */
    else if(xctx->paste_from == 1 && stat(sel_file, &buf) && (xctx->ui_state & STARTMERGE)) {
      abort_operation();
    }
    /*xschem window *receiving* selected objects 
     * no selected objects and selection file exists --> start merge */
    else if(xctx->lastsel == 0 && !stat(sel_file, &buf)) {
      xctx->mousex_snap = 490;
      xctx->mousey_snap = -340;
      merge_file(1, ".sch");
    }
}

static void handle_motion_notify(int event, KeySym key, int state, int rstate, int button, 
  int mx, int my, int aux, int draw_xhair, int enable_stretch)
{
    char str[PATH_MAX + 100];
    if( waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return;
    }
    if(draw_xhair) {
      draw_crosshair(1, state); /* when moving mouse: first action is delete crosshair, will be drawn later */
    }
    /* pan schematic */
    if(xctx->ui_state & STARTPAN) pan(RUBBER, mx, my);

    if(xctx->semaphore >= 2) {
      if(draw_xhair) {
        draw_crosshair(2, state); /* locked UI: draw new crosshair and break out */
      }
      return;
    }

    /* update status bar messages */
    if(xctx->ui_state) {
      if(abs(mx-xctx->mx_save) > 8 || abs(my-xctx->my_save) > 8 ) {
        my_snprintf(str, S(str), "mouse = %.16g %.16g - selected: %d w=%.6g h=%.6g",
          xctx->mousex_snap, xctx->mousey_snap,
          xctx->lastsel ,
          xctx->mousex_snap-xctx->mx_double_save, xctx->mousey_snap-xctx->my_double_save
        );
        statusmsg(str,1);
      }
    }
    
    /* determine direction of a rectangle selection  (or unselection with ALT key) */
    if(xctx->ui_state & STARTSELECT && !(xctx->ui_state & (PLACE_SYMBOL | STARTPAN | PLACE_TEXT)) ) {
      /* Unselect by area : determine direction */
      if( ((state & Button1Mask)  && SET_MODMASK) || (xctx->ui_state & DESEL_AREA)) { 
        if(mx >= xctx->mx_save) xctx->nl_dir = 0;
        else  xctx->nl_dir = 1;
        select_rect(enable_stretch, RUBBER,0);
      /* select by area : determine direction */
      } else if(state & Button1Mask) {
        if(mx >= xctx->mx_save) xctx->nl_dir = 0;
        else  xctx->nl_dir = 1;
        select_rect(enable_stretch, RUBBER,1);
      }
    }
    /* draw objects being moved */
    if(xctx->ui_state & STARTMOVE) {
      if(xctx->constr_mv == 1) xctx->mousey_snap = xctx->my_double_save;
      if(xctx->constr_mv == 2) xctx->mousex_snap = xctx->mx_double_save;
      move_objects(RUBBER,0,0,0);
    }
    
    /* draw objects being copied */
    if(xctx->ui_state & STARTCOPY) {
      if(xctx->constr_mv == 1) xctx->mousey_snap = xctx->my_double_save;
      if(xctx->constr_mv == 2) xctx->mousex_snap = xctx->mx_double_save;
      copy_objects(RUBBER);
    }
    
    /* draw moving objects being inserted, wires, arcs, lines, rectangles, polygons or zoom box */
    redraw_w_a_l_r_p_z_rubbers(0);

    /* start of a mouse area select. Button1 pressed. No shift pressed
     * Do not start an area select if user is dragging a polygon/bezier point */
    if(!(xctx->ui_state & STARTPOLYGON) && (state&Button1Mask) && !(xctx->ui_state & STARTWIRE) && 
       !(xctx->ui_state & STARTPAN) && !(SET_MODMASK) && !xctx->shape_point_selected &&
       !(state & ShiftMask) && !(xctx->ui_state & (PLACE_SYMBOL | PLACE_TEXT)))
    {
      if(mx != xctx->mx_save || my != xctx->my_save) {
        xctx->mouse_moved = 1;
        if(!xctx->drag_elements) {
          if( !(xctx->ui_state & STARTSELECT)) {
            select_rect(enable_stretch, START,1);
            xctx->onetime=1;
          }
          if(abs(mx-xctx->mx_save) > 8 ||
             abs(my-xctx->my_save) > 8 ) { /* set reasonable threshold before unsel */
            if(xctx->onetime) {
              unselect_all(1); /* 20171026 avoid multiple calls of unselect_all() */
              xctx->onetime=0;
            }
            xctx->ui_state|=STARTSELECT; /* set it again cause unselect_all(1) clears it... */
          }
        }
      }
    }
    /* Unselect by area */
    if( (((state & Button1Mask)  && SET_MODMASK) || (xctx->ui_state & DESEL_AREA)) &&
       !(state & ShiftMask) &&
       !(xctx->ui_state & STARTPAN) &&
       !xctx->shape_point_selected &&
       !(xctx->ui_state & STARTSELECT) &&
       !(xctx->ui_state & (PLACE_SYMBOL | PLACE_TEXT))) { /* unselect area */
      select_rect(enable_stretch, START,0);
    }
    /* Select by area. Shift pressed */
    else if((state&Button1Mask) && (state & ShiftMask) && !(xctx->ui_state & STARTWIRE) &&
             !(xctx->ui_state & (PLACE_SYMBOL | PLACE_TEXT)) && !xctx->shape_point_selected &&
             !xctx->drag_elements && !(xctx->ui_state & STARTPAN) ) {
      if(mx != xctx->mx_save || my != xctx->my_save) {
        if( !(xctx->ui_state & STARTSELECT)) {
          select_rect(enable_stretch, START,1);
        }
        if(abs(mx-xctx->mx_save) > 8 || 
           abs(my-xctx->my_save) > 8 ) {  /* set reasonable threshold before unsel */
          if(!xctx->already_selected) {
            select_object(X_TO_XSCHEM(xctx->mx_save),
                          Y_TO_XSCHEM(xctx->my_save), 0, 0, NULL); /* remove near obj if dragging */
          }
          rebuild_selected_array();
        }
      }
    }
    if(draw_xhair) {
      draw_crosshair(2, state); /* what = 2(draw) */
    }
}

static void handle_key_press(int event, KeySym key, int state, int rstate, int mx, int my, 
     int button, int aux, int infix_interface, int enable_stretch, const char *win_path, double c_snap)
{
   char str[PATH_MAX + 100];
   if(key==' ') {
     if(xctx->ui_state & STARTWIRE) { /*  & instead of == 20190409 */
       new_wire(RUBBER|CLEAR, xctx->mousex_snap, xctx->mousey_snap);
       xctx->manhattan_lines++;
       xctx->manhattan_lines %=3;
       new_wire(RUBBER, xctx->mousex_snap, xctx->mousey_snap);

     } else if(xctx->ui_state & STARTLINE) {
       new_line(RUBBER|CLEAR, xctx->mousex_snap, xctx->mousey_snap);
       xctx->manhattan_lines++;
       xctx->manhattan_lines %=3;
       new_line(RUBBER, xctx->mousex_snap, xctx->mousey_snap);
     } else {
       if(xctx->semaphore<2) {
         rebuild_selected_array(); /* sets or clears xctx->ui_state SELECTION flag */
       }
       pan(START, mx, my);
       xctx->ui_state |= STARTPAN;
     }
     return;
   }
   if(key == '_' )              /* toggle change line width */
   {
    if(!tclgetboolvar("change_lw")) {
        tcleval("alert_ { enabling change line width} {}");
        tclsetvar("change_lw","1");
    }
    else {
        tcleval("alert_ { disabling change line width} {}");
        tclsetvar("change_lw","0");
    }
    return;
   }
   if(key == 'b' && rstate==ControlMask)         /* toggle show text in symbol */
   {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return;
    }     
    xctx->sym_txt =!xctx->sym_txt;
    if(xctx->sym_txt) {
        /* tcleval("alert_ { enabling text in symbol} {}"); */
        tclsetvar("sym_txt","1");
        draw();
    }
    else {
        /* tcleval("alert_ { disabling text in symbol} {}"); */
        tclsetvar("sym_txt","0");
        draw();
    }
    return;
   }
   if(key == '%' )              /* toggle draw grid */
   {
    int dr_gr;
    dr_gr = tclgetboolvar("draw_grid");
    dr_gr =!dr_gr;
    if(dr_gr) {
        /* tcleval("alert_ { enabling draw grid} {}"); */
        tclsetvar("draw_grid","1");
        draw();
    }
    else {
        /* tcleval("alert_ { disabling draw grid} {}"); */
        tclsetvar("draw_grid","0");
        draw();
    }
    return;
   }
   if(key == 'j'  && rstate==0 )                 /* print list of highlight nets */
   {
     if(xctx->semaphore >= 2) return;
     print_hilight_net(1);
     return;
   }
   if(key == 'j'  && rstate==ControlMask)        /* create ipins from highlight nets */
   {
     if(xctx->semaphore >= 2) return;
     print_hilight_net(0);
     return;
   }
   if(key == 'j'  && EQUAL_MODMASK)   /* create labels without i prefix from hilight nets */
   {
     if(xctx->semaphore >= 2) return;
     print_hilight_net(4);
     return;
   }
   if(key == 'J'  && SET_MODMASK ) /* create labels with i prefix from hilight nets */
   {
     if(xctx->semaphore >= 2) return;
     print_hilight_net(2);
     return;
   }
   if(key == 'h'  && rstate==ControlMask )       /* go to http link */
   {
     int savesem = xctx->semaphore;
     xctx->semaphore = 0;
     launcher();
     xctx->semaphore = savesem;
     return;
   }
   if(key == 'h'  && EQUAL_MODMASK)   /* create symbol pins from schematic pins 20171208 */
   {
     tcleval("schpins_to_sympins");
     return;
   }
   if(key == 'h' && rstate == 0) {
     /* horizontally constrained drag 20171023 */
     if ( xctx->constr_mv == 1 ) {
       tcleval("set constr_mv 0" );
       xctx->constr_mv = 0;
     } else {
       tcleval("set constr_mv 1" );
       xctx->constr_mv = 1;
     }
     if(xctx->ui_state & STARTWIRE) {
       if(xctx->constr_mv == 1) xctx->mousey_snap = xctx->my_double_save;
       if(xctx->constr_mv == 2) xctx->mousex_snap = xctx->mx_double_save;
       new_wire(RUBBER, xctx->mousex_snap, xctx->mousey_snap);
     }
     if(xctx->ui_state & STARTLINE) {
       if(xctx->constr_mv == 1) xctx->mousey_snap = xctx->my_double_save;
       if(xctx->constr_mv == 2) xctx->mousex_snap = xctx->mx_double_save;
       new_line(RUBBER, xctx->mousex_snap, xctx->mousey_snap);
     }
     return;
   }
   if(key=='H' && rstate == 0) {           /* attach labels to selected instances */
     attach_labels_to_inst(1);
     return;
   }
   if (key == 'H' && rstate == ControlMask) { /* create schematic and symbol from selected components */
     make_schematic_symbol_from_sel();
     return;
   }
   if(key == 'v' && rstate==0) {
     /* vertically constrained drag 20171023 */
     if ( xctx->constr_mv == 2 ) {
       tcleval("set constr_mv 0" );
       xctx->constr_mv = 0;
     } else {
       tcleval("set constr_mv 2" );
       xctx->constr_mv = 2;
     }
     if(xctx->ui_state & STARTWIRE) {
       if(xctx->constr_mv == 1) xctx->mousey_snap = xctx->my_double_save;
       if(xctx->constr_mv == 2) xctx->mousex_snap = xctx->mx_double_save;
       new_wire(RUBBER, xctx->mousex_snap, xctx->mousey_snap);
     }
     if(xctx->ui_state & STARTLINE) {
       if(xctx->constr_mv == 1) xctx->mousey_snap = xctx->my_double_save;
       if(xctx->constr_mv == 2) xctx->mousex_snap = xctx->mx_double_save;
       new_line(RUBBER, xctx->mousex_snap, xctx->mousey_snap);
     }
     return;
   }
   if(key == 'j'  && SET_MODMASK && (state & ControlMask) )  /* print list of highlight net with label expansion */
   {
     print_hilight_net(3);
     return;
   }
   if(key == 'J' && rstate == 0) {
    create_plot_cmd();
    return;
   }
   if(key == '$' &&  rstate == 0 )            /* toggle pixmap  saving */
   {
    xctx->draw_pixmap =!xctx->draw_pixmap;
    if(xctx->draw_pixmap) tcleval("alert_ { enabling draw pixmap} {}");
    else tcleval("alert_ { disabling draw pixmap} {}");
    return;
   }
   if(key == '$' && (state &ControlMask) )              /* toggle window  drawing */
   {
    xctx->draw_window =!xctx->draw_window;
    if(xctx->draw_window) {
      tcleval("alert_ { enabling draw window} {}");
      tclsetvar("draw_window","1");
    } else {
      tcleval("alert_ { disabling draw window} {}");
      tclsetvar("draw_window","0");
    }
    return;
   }
   if(key == '='  && (state &ControlMask))              /* toggle fill rectangles */
   {
    int x;
    xctx->fill_pattern++;
    if(xctx->fill_pattern==3) xctx->fill_pattern=0;

    if(xctx->fill_pattern==1) {
     tcleval("alert_ { Stippled pattern fill} {}");
     for(x=0;x<cadlayers;x++) {
       if(xctx->fill_type[x]==1) XSetFillStyle(display,xctx->gcstipple[x],FillSolid);
       else XSetFillStyle(display,xctx->gcstipple[x],FillStippled);
     }
    }
    else if(xctx->fill_pattern==2) {
     tcleval("alert_ { solid pattern fill} {}");
     for(x=0;x<cadlayers;x++)
      XSetFillStyle(display,xctx->gcstipple[x],FillSolid);
    }
    else  {
     tcleval("alert_ { No pattern fill} {}");
     for(x=0;x<cadlayers;x++)
      XSetFillStyle(display,xctx->gcstipple[x],FillStippled);
    }

    draw();
    return;
   }
   if(key == '+'  && state & ControlMask)         /* change line width */
   {
    xctx->lw = round_to_n_digits(xctx->lw + 0.5, 2);
    change_linewidth(xctx->lw);
    tclsetboolvar("change_lw", 0);
    draw();
    return;
   }

   if(key == '-'  && state & ControlMask)         /* change line width */
   {
    xctx->lw = round_to_n_digits(xctx->lw - 0.5, 2);
    if(xctx->lw < 0.0) xctx->lw = 0.0;
    change_linewidth(xctx->lw);
    tclsetboolvar("change_lw", 0);
    draw();
    return;
   }
   if(key == 'X' && rstate == 0) /* highlight discrepanciens between selected instance pin and net names */
   {
     hilight_net_pin_mismatches();
     return;
   }
   if(key== 'W' /* && !xctx->ui_state */ && rstate == 0) {  /* create wire snapping to closest instance pin */
     if(xctx->semaphore >= 2) return;
     if(infix_interface) {
       snapped_wire(c_snap);
     } else {
       xctx->ui_state |= MENUSTART;
       xctx->ui_state2 = MENUSTARTSNAPWIRE;
     }
     return;
   }
   if(key == 'w' /* && !xctx->ui_state */ && rstate==0)    /* place wire. */
   {
     int prev_state = xctx->ui_state;
     if(xctx->semaphore >= 2) return;

     if(infix_interface) {
       start_wire(xctx->mousex_snap, xctx->mousey_snap);
       if(prev_state == STARTWIRE) {
         tcleval("set constr_mv 0" );
         xctx->constr_mv=0;
       }
     } else {
       xctx->last_command = 0;
       xctx->ui_state |= MENUSTART;
       xctx->ui_state2 = MENUSTARTWIRE;
     }
     return;
   }
   if(key == XK_Return && (state == 0 ) && xctx->ui_state & STARTPOLYGON) { /* close polygon */
    new_polygon(ADD|END, xctx->mousex_snap, xctx->mousey_snap);
    return;
   }
   if(key == XK_Escape) /* abort & redraw */
   {
    if(xctx->semaphore < 2) {
      abort_operation();
    }
    /* stuff that can be done reentrantly ... */
    tclsetvar("tclstop", "1"); /* stop simulation if any running */
    return;
   }
   if(key=='z' && rstate == 0 &&
     !(xctx->ui_state & (STARTRECT | STARTLINE | STARTWIRE | STARTPOLYGON | STARTARC))) /* zoom box */
   {
    dbg(1, "callback(): zoom_rectangle call\n");
    zoom_rectangle(START);return;
   }
   if(key=='Z' && rstate == 0)                   /* zoom in */
   {
    view_zoom(0.0); return;
   }
   if(key=='p' && EQUAL_MODMASK)                           /* add symbol pin */
   {
    xctx->push_undo();
    unselect_all(1);
    storeobject(-1, xctx->mousex_snap-2.5, xctx->mousey_snap-2.5, xctx->mousex_snap+2.5, xctx->mousey_snap+2.5,
                xRECT, PINLAYER, SELECTED, "name=XXX\ndir=inout");
    xctx->need_reb_sel_arr=1;
    rebuild_selected_array();
    move_objects(START,0,0,0);
    xctx->ui_state |= START_SYMPIN;
    return;
   }
   if(key=='p' && !(xctx->ui_state & STARTPOLYGON) && rstate==0)              /* start polygon */
   {
     if(xctx->semaphore >= 2) return;
     dbg(1, "callback(): start polygon\n");
     if(infix_interface) {
       xctx->mx_double_save=xctx->mousex_snap;
       xctx->my_double_save=xctx->mousey_snap;
       xctx->last_command = 0;
       new_polygon(PLACE, xctx->mousex_snap, xctx->mousey_snap);
     } else {
       xctx->ui_state |= MENUSTART;
       xctx->ui_state2 = MENUSTARTPOLYGON;
     }
     return;
   }
   if(key=='P' && rstate == 0)                   /* pan, other way to. */
   {
    xctx->xorigin=-xctx->mousex_snap+xctx->areaw*xctx->zoom/2.0;
    xctx->yorigin=-xctx->mousey_snap+xctx->areah*xctx->zoom/2.0;
    draw();
    redraw_w_a_l_r_p_z_rubbers(1);
    return;
   }
   if(key=='5' && rstate == 0) { /* 20110112 display only probes */
    xctx->only_probes = !xctx->only_probes;
    tclsetboolvar("only_probes", xctx->only_probes);
    toggle_only_probes();
    return;
   }  /* /20110112 */
   if(key<='9' && key >='0' && state==ControlMask)              /* choose layer */
   {
    char n[30];
    xctx->rectcolor = (int)key - '0'+4;
    my_snprintf(n, S(n), "%d", xctx->rectcolor);
    tclvareval("xschem set rectcolor ", n, NULL);

    if(has_x) {
      if(!strcmp(win_path, ".drw")) {
        tclvareval("reconfigure_layers_button {}", NULL);
      } else {
        tclvareval("reconfigure_layers_button [winfo parent ", win_path, "]", NULL);
      }
    }
    dbg(1, "callback(): new color: %d\n",xctx->color_index[xctx->rectcolor]);
    return;
   }
   if(key==XK_Delete && (xctx->ui_state & SELECTION) ) /* delete selection */
   {
     if(xctx->semaphore >= 2) return;
     delete(1/* to_push_undo */);return;
   }
   if(key==XK_Right && state == ControlMask) {
     int save = xctx->semaphore;
     if(waves_selected(event, key, state, button)) {
       waves_callback(event, mx, my, key, button, aux, state);
       return;
     }
     if(xctx->semaphore >= 2) return;
     xctx->semaphore = 0;
     tcleval("next_tab");
     xctx->semaphore = save;
     return;
   }
   if(key==XK_Left && state == ControlMask) {
     int save = xctx->semaphore;
     if(waves_selected(event, key, state, button)) {
       waves_callback(event, mx, my, key, button, aux, state);
       return;
     }
     if(xctx->semaphore >= 2) return;
     xctx->semaphore = 0;
     tcleval("prev_tab");
     xctx->semaphore = save;
     return;
   }
   if(key==XK_Right && !(state & ControlMask))   /* left */
   {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return;
    }
    xctx->xorigin+=-CADMOVESTEP*xctx->zoom;
    draw();
    redraw_w_a_l_r_p_z_rubbers(1);
    return;
   }
   if(key==XK_Left && !(state & ControlMask))   /* right */
   {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return;
    }
    xctx->xorigin-=-CADMOVESTEP*xctx->zoom;
    draw();
    redraw_w_a_l_r_p_z_rubbers(1);
    return;
   }
   if(key==XK_Down)                     /* down */
   {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return;
    }
    xctx->yorigin+=-CADMOVESTEP*xctx->zoom;
    draw();
    redraw_w_a_l_r_p_z_rubbers(1);
    return;
   }
   if(key==XK_Up)                       /* up */
   {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return;
    }
    xctx->yorigin-=-CADMOVESTEP*xctx->zoom;
    draw();
    redraw_w_a_l_r_p_z_rubbers(1);
    return;
   }
   if(key=='w' && rstate == ControlMask) /* close current schematic */
   {
     int save_sem;
     if(xctx->semaphore >= 2) return;
     save_sem = xctx->semaphore;
     tcleval("xschem exit");
     xctx->semaphore = save_sem;
     return;
   }
   /* toggle spice_ignore, verilog_ignore, ... flag on selected instances. */
   if(key == 'T' && rstate == 0) {
     toggle_ignore();
   }
   if(key=='t' && rstate == 0)                        /* place text */
   {
     if(waves_selected(event, key, state, button)) {
       waves_callback(event, mx, my, key, button, aux, state);
       return;
     }
     if(xctx->semaphore >= 2) return;
     xctx->last_command = 0;
     xctx->mx_double_save = xctx->mousex_snap;
     xctx->my_double_save = xctx->mousey_snap;
     if(place_text(0, xctx->mousex_snap, xctx->mousey_snap)) { /* 1 = draw text 24122002 */
       xctx->mousey_snap = xctx->my_double_save;
       xctx->mousex_snap = xctx->mx_double_save;
       move_objects(START,0,0,0);
       xctx->ui_state |= PLACE_TEXT;
     }
     return;
   }
   if(key=='t' && (rstate & ControlMask)) 
   {
     if(waves_selected(event, key, state, button)) {
       waves_callback(event, mx, my, key, button, aux, state);
       return; 
     }
     return;
   }  
   if(key=='r' /* && !xctx->ui_state */ && rstate==0)              /* start rect */
   {
    dbg(1, "callback(): start rect\n");
    if(xctx->semaphore >= 2) return;
    if(infix_interface) {
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      xctx->last_command = 0;
      new_rect(PLACE,xctx->mousex_snap, xctx->mousey_snap);
    } else {
      xctx->ui_state |= MENUSTART;
      xctx->ui_state2 = MENUSTARTRECT;
    }
    return;
   }
   if(key=='V' && rstate == ControlMask)                     /* toggle spice/vhdl netlist  */
   {
    xctx->netlist_type++; if(xctx->netlist_type==6) xctx->netlist_type=1;
    set_tcl_netlist_type();
    draw(); /* needed to ungrey or grey out  components due to *_ignore attribute */
    return;
   }
   if(key=='s' && rstate == 0 )      /* simulate */
   {
     if(xctx->semaphore >= 2) return;
     if(waves_selected(event, key, state, button)) {
       waves_callback(event, mx, my, key, button, aux, state);
       return;
     }
     tcleval("tk_messageBox -type okcancel -parent [xschem get topwindow] "
             "-message {Run circuit simulation?}");
     if(strcmp(tclresult(),"ok")==0) {
       tcleval("[xschem get top_path].menubar invoke Simulate");
     }
     return;
   }
   if(key=='s' && rstate == ControlMask )      /* save 20121201 */
   {
     if(xctx->semaphore >= 2) return;
     if(waves_selected(event, key, state, button)) {
       waves_callback(event, mx, my, key, button, aux, state);
       return;
     }     
     /* check if unnamed schematic, use saveas in this case */
     if(!strcmp(xctx->sch[xctx->currsch],"") || strstr(xctx->sch[xctx->currsch], "untitled")) {
       saveas(NULL, SCHEMATIC);
     } else {
       save(1, 0);
     }
     return;
   }
   if(key=='s' && SET_MODMASK && (state & ControlMask) )           /* save as symbol */
   {
     if(xctx->semaphore >= 2) return;
     saveas(NULL, SYMBOL);
     return;
   }
   if(key=='S' && rstate == ControlMask) /* save as schematic */
   {
     if(xctx->semaphore >= 2) return;
     saveas(NULL, SCHEMATIC);
     return;
   }
   if(key=='e' && rstate == 0)           /* descend to schematic */
   {
    if(xctx->semaphore >= 2) return;
    descend_schematic(0, 1, 1, 1);return;
   }
   if(key=='e' && EQUAL_MODMASK)            /* edit schematic in new window */
   {
    int save = xctx->semaphore;
    xctx->semaphore--; /* so semaphore for current context wll be saved correctly */
    /*  schematic_in_new_window(0, 1, 0); */
    tcleval("open_sub_schematic");
    xctx->semaphore = save;
    return;
   }

   if(key=='E' && EQUAL_MODMASK)          /* edit schematic in new window - new xschem process */
   { 
    int save = xctx->semaphore;
    xctx->semaphore--; /* so semaphore for current context wll be saved correctly */
    schematic_in_new_window(1, 1, 0);
    xctx->semaphore = save;
    return;
   }

   if(key=='i' && EQUAL_MODMASK)            /* edit symbol in new window */
   {
    int save =  xctx->semaphore;
    xctx->semaphore--; /* so semaphore for current context wll be saved correctly */
    symbol_in_new_window(0);
    xctx->semaphore = save;
    return;
   }

   if(key=='I' && EQUAL_MODMASK)            /* edit symbol in new window - new xschem process */
   {
    int save =  xctx->semaphore;
    xctx->semaphore--; /* so semaphore for current context wll be saved correctly */
    symbol_in_new_window(1);
    xctx->semaphore = save;
    return;
   }


   if( (key=='e' && rstate == ControlMask) || (key==XK_BackSpace))  /* back */
   {
    if(xctx->semaphore >= 2) return;
    go_back(1);return;
   }

   if(key=='a' && rstate == 0)   /* make symbol */
   {
    if(xctx->semaphore >= 2) return;
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return;
    }
    tcleval("tk_messageBox -type okcancel -parent [xschem get topwindow] "
            "-message {do you want to make symbol view ?}");
    if(strcmp(tclresult(),"ok")==0)
    {
     save_schematic(xctx->sch[xctx->currsch], 0);
     make_symbol();
    }
    return;
   }
   if(key=='a' && rstate == ControlMask)         /* select all */
   {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return;
    }     
    select_all();
    return;
   }
   if(key=='y' && rstate == 0)                           /* toggle stretching */
   {
    enable_stretch = !enable_stretch;
    tclsetboolvar("enable_stretch", enable_stretch);
    return;
   }
   if(key=='x' && EQUAL_MODMASK) /* toggle draw crosshair at mouse pos */
   {
     if(tclgetboolvar("draw_crosshair")) {
       tclsetvar("draw_crosshair", "0");
     } else {
       tclsetvar("draw_crosshair", "1");
     }
     draw();
   }
   if(key=='x' && rstate == ControlMask) /* cut selection into clipboard */
   {
    if(xctx->semaphore >= 2) return;
    rebuild_selected_array();
    if(xctx->lastsel) {  /* 20071203 check if something selected */
      save_selection(2);
      delete(1/* to_push_undo */);
    }
    return;
   }
   if(key=='c' && rstate == ControlMask) /* copy selection into clipboard */
   {
     if(xctx->semaphore >= 2) return;
     rebuild_selected_array();
     if(xctx->lastsel) {  /* 20071203 check if something selected */
       save_selection(2);
     }
    return;
   }
   if(key=='C' /* && !xctx->ui_state */ && rstate == 0) /* place arc */
   {
     if(xctx->semaphore >= 2) return;
     if(infix_interface) {
       xctx->mx_double_save=xctx->mousex_snap;
       xctx->my_double_save=xctx->mousey_snap;
       xctx->last_command = 0;
       new_arc(PLACE, 180., xctx->mousex_snap, xctx->mousey_snap);
     } else {
       xctx->ui_state |= MENUSTART;
       xctx->ui_state2 = MENUSTARTARC;
     }
     return;
   }
   if(key=='C' /* && !xctx->ui_state */ && rstate == ControlMask) /* place circle */
   {
     if(xctx->semaphore >= 2) return;
     if(infix_interface) {
       xctx->mx_double_save=xctx->mousex_snap;
       xctx->my_double_save=xctx->mousey_snap;
       xctx->last_command = 0;
       new_arc(PLACE, 360., xctx->mousex_snap, xctx->mousey_snap);
     } else {
       xctx->ui_state |= MENUSTART;
       xctx->ui_state2 = MENUSTARTCIRCLE;
     }
     return;
   }
   if(key=='O' && rstate == ControlMask )   /* load most recent tile */
   {
     tclvareval("xschem load -gui [lindex $recentfile 0]", NULL);
     return;
   }
   if(key=='O' && rstate == 0)   /* toggle light/dark colorscheme 20171113 */
   {
     int d_c;
     d_c = tclgetboolvar("dark_colorscheme");
     d_c = !d_c;
     tclsetboolvar("dark_colorscheme", d_c);
     tclsetdoublevar("dim_value", 0.0);
     tclsetdoublevar("dim_bg", 0.0);
     build_colors(0.0, 0.0);
     draw();
     return;
   }
   if(key=='v' && rstate == ControlMask)   /* paste from clipboard */
   {
    if(xctx->semaphore >= 2) return;
    merge_file(2,".sch");
    return;
   }
   if(key=='Q' && rstate == ControlMask ) /* view attributes */
   {
    edit_property(2);return;
   }
   if(key=='q' && rstate==ControlMask) /* quit xschem */
   {
    if(xctx->semaphore >= 2) return;
    /* must be set to zero, otherwise switch_tab/switch_win does not proceed
     * and these are necessary when closing tabs/windows */
    xctx->semaphore = 0;
    tcleval("quit_xschem");
    return;
   }
   if(key=='q' && rstate==0) /* edit attributes */
   {
    if(xctx->semaphore >= 2) return;
    edit_property(0);
    return;
   }
   if(key=='q' && EQUAL_MODMASK)                      /* edit .sch file (DANGER!!) */
   {
    if(xctx->semaphore >= 2) return;
    rebuild_selected_array();
    if(xctx->lastsel==0 ) {
      my_snprintf(str, S(str), "edit_file {%s}", abs_sym_path(xctx->sch[xctx->currsch], ""));
      tcleval(str);
    }
    else if(xctx->sel_array[0].type==ELEMENT) {
      my_snprintf(str, S(str), "edit_file {%s}",
         abs_sym_path(tcl_hook2(xctx->inst[xctx->sel_array[0].n].name), ""));
      tcleval(str);

    }
    return;
   }
   #if defined(__unix__) && HAS_CAIRO==1
   if(key == XK_Print) {
     xctx->ui_state |= GRABSCREEN;
     tclvareval(xctx->top_path, ".drw configure -cursor {}" , NULL);
     tclvareval("grab set -global ", xctx->top_path, ".drw", NULL);
     return;
   }
   #endif
   if(key=='Q' && rstate == 0) /* edit attributes in editor */
   {
    if(xctx->semaphore >= 2) return;
    edit_property(1);return;
   }
   if(key=='i' && rstate==0) /* descend to  symbol */
   {
    if(xctx->semaphore >= 2) return;
    descend_symbol();return;
   }
   if((key==XK_Insert && state == ShiftMask) ||  (key == 'i' && rstate == ControlMask)) /* insert sym */
   {
     tcleval("load_file_dialog {Insert symbol} *.\\{sym,tcl\\} INITIALINSTDIR 2");
     return;
   }
   if((key==XK_Insert) || (key == 'I' && rstate == 0) ) /* insert sym */
   {
    if(xctx->semaphore >= 2) return;
    start_place_symbol();

    return;
   }
   if(key=='s' && SET_MODMASK)                     /* reload */
   {
    if(xctx->semaphore >= 2) return;
     tcleval("tk_messageBox -type okcancel -parent [xschem get topwindow] "
             "-message {Are you sure you want to reload from disk?}");
     if(strcmp(tclresult(),"ok")==0) {
        char filename[PATH_MAX];
        unselect_all(1);
        remove_symbols();
        my_strncpy(filename, abs_sym_path(xctx->sch[xctx->currsch], ""), S(filename));
        load_schematic(1, filename, 1, 1);
        draw();
     }
     return;
   }
   if(key=='o' && rstate == ControlMask)   /* load */
   {
    if(xctx->semaphore >= 2) return;
    ask_new_file();
    return;
   }
   if(key=='S' && rstate == 0)   /* change element order */
   {
    if(xctx->semaphore >= 2) return;
    change_elem_order(-1);
    return;
   }
   if(key=='k' && EQUAL_MODMASK)        /* select whole net (all attached wires/labels/pins) */
   {
     select_hilight_net();
     return;
   }
   if(key=='k' && rstate==ControlMask)                           /* unhilight net */
   {
    if(xctx->semaphore >= 2) return;
    unhilight_net();
    return;
   }
   if(key=='K' && rstate == ControlMask)       /* hilight net drilling thru elements  */
                                                        /* with 'propag=' prop set on pins */
   {
    if(xctx->semaphore >= 2) return;
    xctx->enable_drill=1;
    hilight_net(0);
    redraw_hilights(0);
    /* draw_hilight_net(1); */
    return;
   }
   if(key=='k' && rstate==0)                             /* hilight net */
   {
    if(xctx->semaphore >= 2) return;
    xctx->enable_drill=0;
    hilight_net(0);
    redraw_hilights(0);
    /* draw_hilight_net(1); */
    return;
   }
   if(key=='K' && rstate == 0)                           /* delete hilighted nets */
   {
    if(xctx->semaphore >= 2) return;
    xctx->enable_drill=0;
    clear_all_hilights();
    /* undraw_hilight_net(1); */
    draw();
    return;
   }
   if(key=='g' && EQUAL_MODMASK) { /* highlight net and send to viewer */
     int tool = 0;
     int exists = 0;
     char *tool_name = NULL;

     if(xctx->semaphore >= 2) return;
     tcleval("winfo exists .graphdialog");
     if(tclresult()[0] == '1') tool = XSCHEM_GRAPH;
     else if(xctx->graph_lastsel >=0 &&
         xctx->rects[GRIDLAYER] > xctx->graph_lastsel &&
         xctx->rect[GRIDLAYER][xctx->graph_lastsel].flags & 1) {
       tool = XSCHEM_GRAPH;
     }
     tcleval("info exists sim");
     if(tclresult()[0] == '1') exists = 1;
     xctx->enable_drill = 0;
     if(exists) {
       if(!tool) {
         tool = tclgetintvar("sim(spicewave,default)");
         my_snprintf(str, PATH_MAX + 100, "sim(spicewave,%d,name)", tool);
         my_strdup(_ALLOC_ID_, &tool_name, tclgetvar(str));
         dbg(1,"callback(): tool_name=%s\n", tool_name);
         if(strstr(tool_name, "Gaw")) tool=GAW;
         else if(strstr(tool_name, "Bespice")) tool=BESPICE;
         my_free(_ALLOC_ID_, &tool_name);
       }
     }
     if(tool) {
       hilight_net(tool);
       redraw_hilights(0);
     }
     Tcl_ResetResult(interp);
     return;
   }
   if(key=='g' && rstate==0)                         /* half snap factor */
   {
    set_snap(c_snap / 2.0);
    change_linewidth(-1.);
    draw();
    return;
   }
   if(key=='g' && rstate==ControlMask)              /* set snap factor 20161212 */
   {
    my_snprintf(str,  S(str),
     "input_line {Enter snap value (default: %.16g current: %.16g)}  {xschem set cadsnap} {%g} 10",
     CADSNAP, c_snap, c_snap);
    tcleval(str);
    return;
   }
   if(key=='G' && rstate == 0)                                    /* double snap factor */
   {
    set_snap(c_snap * 2.0);
    change_linewidth(-1.);
    draw();
    return;
   }
   if(key=='*' && EQUAL_MODMASK)         /* svg print , 20121108 */
   {
    if(xctx->semaphore >= 2) return;
    svg_draw();
    return;
   }
   if(key=='*' && rstate == 0 )                    /* postscript print */
   {
    if(xctx->semaphore >= 2) return;
    ps_draw(7, 0, 0);
    return;
   }
   if(key=='*' && rstate == ControlMask)      /* xpm print */
   {
    if(xctx->semaphore >= 2) return;
    print_image();
    return;
   }
   if(key=='u' && EQUAL_MODMASK)                      /* align to grid */
   {
    if(xctx->semaphore >= 2) return;
    xctx->push_undo();
    round_schematic_to_grid(c_snap);
    set_modify(1);
    if(tclgetboolvar("autotrim_wires")) trim_wires();
    xctx->prep_hash_inst=0;
    xctx->prep_hash_wires=0;
    xctx->prep_net_structs=0;
    xctx->prep_hi_structs=0;

    draw();
    return;
   }
   if(0 && (key=='u') && rstate==ControlMask)                   /* testmode */
   {
    static int x = 0;

    if(x == 0) {
      int i;
      XFillRectangle(display, xctx->window, xctx->gc[BACKLAYER], xctx->areax1, xctx->areay1,
            xctx->areaw, xctx->areah);
      XFlush(display);
      sleep_ms(400);
      for(i = xctx->xrect[0].x; i < xctx->xrect[0].width; i++) {
        XDrawLine(display, xctx->window,  xctx->gctiled,
           i, xctx->xrect[0].y, i, xctx->xrect[0].height);
        XFlush(display);
        sleep_ms(4);
      }
    } else if(x == 1) {
      int i;
      XFillRectangle(display, xctx->window, xctx->gc[BACKLAYER], xctx->areax1, xctx->areay1,
            xctx->areaw, xctx->areah);
      XFlush(display);
      sleep_ms(400);
      for(i = xctx->xrect[0].x; i < xctx->xrect[0].width; i++) {
        XDrawLine(display, xctx->window,  xctx->gctiled,
           i, xctx->xrect[0].y, i+1, xctx->xrect[0].height);
        XFlush(display);
        sleep_ms(4);
      }
    }
    x++;
    x %= 2;
    return;
   }
   if(key=='u' && rstate==0)                             /* undo */
   {
    if(xctx->semaphore >= 2) return;
    xctx->pop_undo(0, 1);  /* 2nd parameter: set_modify_status */
    draw();
    return;
   }
   if(key=='U' && rstate == 0)                     /* redo */
   {
    if(xctx->semaphore >= 2) return;
    xctx->pop_undo(1, 1); /* 2nd parameter: set_modify_status */
    draw();
    return;
   }
   if(key=='&')                         /* check wire connectivity */
   {
    if(xctx->semaphore >= 2) return;
    xctx->push_undo();
    trim_wires();
    draw();
    return;
   }
   if(key=='l' && rstate == ControlMask) { /* create schematic from selected symbol 20171004 */
     if(xctx->semaphore >= 2) return;
     create_sch_from_sym();
     return;
   }
   if(key=='l' /* && !xctx->ui_state */ && rstate == 0) /* start line */
   {
     int prev_state = xctx->ui_state;
     if(xctx->semaphore >= 2) return;
     if(infix_interface) {
       start_line(xctx->mousex_snap, xctx->mousey_snap);
       if(prev_state == STARTLINE) {
         tcleval("set constr_mv 0" );
         xctx->constr_mv=0;
       }
     } else {
       xctx->last_command = 0;
       xctx->ui_state |= MENUSTART;
       xctx->ui_state2 = MENUSTARTLINE;
     }
     return;
   }
   if(key=='l' && EQUAL_MODMASK) {                         /* add pin label*/
     place_net_label(1);
     return;
   }
   if(key >= '0' && key <= '4' && state == 0) {  /* toggle pin logic level */
     if(xctx->semaphore >= 2) return;
     if(key == '4') logic_set(-1, 1, NULL);
     else logic_set((int)key - '0', 1, NULL);
     return;
   }
   if(key=='L' && EQUAL_MODMASK ) {                         /* add pin label*/
    place_net_label(0);
    return;
   }
   if(key=='F' && rstate == 0)                     /* flip */
   {
    if(xctx->ui_state & STARTMOVE) move_objects(FLIP,0,0,0);
    else if(xctx->ui_state & STARTCOPY) copy_objects(FLIP);
    else {
      rebuild_selected_array();
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      move_objects(START,0,0,0);
      move_objects(FLIP,0,0,0);
      move_objects(END,0,0,0);
    }
    return;
   }
   if(key=='V' && rstate == 0)                     /* vertical flip */
   {
    if(xctx->ui_state & STARTMOVE) {
      move_objects(ROTATE,0,0,0);
      move_objects(ROTATE,0,0,0);
      move_objects(FLIP,0,0,0);
    }
    else if(xctx->ui_state & STARTCOPY) {
      copy_objects(ROTATE);
      copy_objects(ROTATE);
      copy_objects(FLIP);
    }
    else {
      rebuild_selected_array();
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;    
      move_objects(START,0,0,0);
      move_objects(ROTATE,0,0,0);
      move_objects(ROTATE,0,0,0);
      move_objects(FLIP,0,0,0);
      move_objects(END,0,0,0);
    } 
    return;
   }
   if(key=='v' && EQUAL_MODMASK)  /* vertical flip objects around their anchor points */
   {
    if(xctx->ui_state & STARTMOVE) {
      move_objects(ROTATE|ROTATELOCAL,0,0,0);
      move_objects(ROTATE|ROTATELOCAL,0,0,0);
      move_objects(FLIP|ROTATELOCAL,0,0,0);
    }
    else if(xctx->ui_state & STARTCOPY) {
      copy_objects(ROTATE|ROTATELOCAL);
      copy_objects(ROTATE|ROTATELOCAL);
      copy_objects(FLIP|ROTATELOCAL);
    }
    else {
      rebuild_selected_array();
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      move_objects(START,0,0,0);
      move_objects(ROTATE|ROTATELOCAL,0,0,0);
      move_objects(ROTATE|ROTATELOCAL,0,0,0);
      move_objects(FLIP|ROTATELOCAL,0,0,0);
      move_objects(END,0,0,0);
    }
    return;
   }

   if(key=='\\' && state==0)          /* fullscreen */
   {
    
    dbg(1, "callback(): toggle fullscreen, win_path=%s\n", win_path);
    toggle_fullscreen(win_path);
    return;
   }
   if(key=='f' && EQUAL_MODMASK)              /* flip objects around their anchor points 20171208 */
   {
    if(xctx->ui_state & STARTMOVE) move_objects(FLIP|ROTATELOCAL,0,0,0);
    else if(xctx->ui_state & STARTCOPY) copy_objects(FLIP|ROTATELOCAL);
    else {
      rebuild_selected_array();
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      move_objects(START,0,0,0);
      move_objects(FLIP|ROTATELOCAL,0,0,0);
      move_objects(END,0,0,0);
    }
    return;
   }
   if(key=='R' && rstate == 0)             /* rotate */
   {
    if(xctx->ui_state & STARTMOVE) move_objects(ROTATE,0,0,0);
    else if(xctx->ui_state & STARTCOPY) copy_objects(ROTATE);
    else {
      rebuild_selected_array();
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      move_objects(START,0,0,0);
      move_objects(ROTATE,0,0,0);
      move_objects(END,0,0,0);
    }

    return;
   }
   if(key=='r' && EQUAL_MODMASK)              /* rotate objects around their anchor points 20171208 */
   {
    if(xctx->ui_state & STARTMOVE) move_objects(ROTATE|ROTATELOCAL,0,0,0);
    else if(xctx->ui_state & STARTCOPY) copy_objects(ROTATE|ROTATELOCAL);
    else {
      rebuild_selected_array();
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      move_objects(START,0,0,0);
      move_objects(ROTATE|ROTATELOCAL,0,0,0);
      move_objects(END,0,0,0);
    }
    return;
   }
   /* Move selection */
   if(key=='m' && rstate==0 && !(xctx->ui_state & (STARTMOVE | STARTCOPY)))
   {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return;
    }
    if(enable_stretch) select_attached_nets(); /* stretch nets that land on selected instance pins */
    if(infix_interface) {
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      move_objects(START,0,0,0);
    } else {
      xctx->ui_state |= MENUSTART;
      xctx->ui_state2 = MENUSTARTMOVE;
    }
    return;
   }
   /* Move selection adding wires to moved pins */
   if(((key == 'M' && rstate == 0) || (key == 'm' && EQUAL_MODMASK)) &&
     !(xctx->ui_state & (STARTMOVE | STARTCOPY)))
   {
    xctx->connect_by_kissing = 2; /* 2 will be used to reset var to 0 at end of move */
    if(infix_interface) {
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      /* select_attached_nets(); */ /* stretch nets that land on selected instance pins */
      move_objects(START,0,0,0);
    } else {
      xctx->ui_state |= MENUSTART;
      xctx->ui_state2 = MENUSTARTMOVE;
    }
    return;
   }
   /* move selection stretching attached nets */
   if(key=='m' && rstate == ControlMask &&
       !(xctx->ui_state & (STARTMOVE | STARTCOPY)))
   {        
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return;
    }     
    if(!enable_stretch) select_attached_nets(); /* stretch nets that land on selected instance pins */
    if(infix_interface) {
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      move_objects(START,0,0,0);
    } else {
      xctx->ui_state |= MENUSTART;
      xctx->ui_state2 = MENUSTARTMOVE;
    }
    return;
   }
   
   /* move selection, stretch attached nets, create new wires on pin-to-moved-pin connections */
   if(key=='M' && rstate == ControlMask &&  
       !(xctx->ui_state & (STARTMOVE | STARTCOPY)))
   {
    if(!enable_stretch) select_attached_nets(); /* stretch nets that land on selected instance pins */
    xctx->connect_by_kissing = 2; /* 2 will be used to reset var to 0 at end of move */
    if(infix_interface) {
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      move_objects(START,0,0,0);
    } else {
      xctx->ui_state |= MENUSTART;
      xctx->ui_state2 = MENUSTARTMOVE;
    }
    return;
   }

   if(key=='c' && EQUAL_MODMASK &&           /* duplicate selection */
     !(xctx->ui_state & (STARTMOVE | STARTCOPY)))
   {
    if(xctx->semaphore >= 2) return;
    xctx->connect_by_kissing = 2; /* 2 will be used to reset var to 0 at end of move */
    if(infix_interface) {
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      copy_objects(START);
    } else {
      xctx->ui_state |= MENUSTART;
      xctx->ui_state2 = MENUSTARTCOPY;
    }
    return;
   }

   if(key=='c' && rstate==0 &&           /* duplicate selection */
     !(xctx->ui_state & (STARTMOVE | STARTCOPY)))
   {
    if(xctx->semaphore >= 2) return;
    if(infix_interface) {
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      copy_objects(START);
    } else {
      xctx->ui_state |= MENUSTART;
      xctx->ui_state2 = MENUSTARTCOPY;
    }
    return;
   }
   if(key=='n' && rstate == ControlMask)              /* clear schematic */
   {
     if(xctx->semaphore >= 2) return;
     tcleval("xschem clear SCHEMATIC");
     return;
   }
   if(key=='N' && rstate == ControlMask )    /* clear symbol */
   {
     if(xctx->semaphore >= 2) return;
     tcleval("xschem clear SYMBOL");
     return;
   }
   if(key=='n' && rstate==0)              /* hierarchical netlist */
   {
    if(xctx->semaphore >= 2) return;
    tcleval("xschem netlist -erc");
    return;
   }
   if(key=='N' && rstate == 0)              /* current level only netlist */
   {
    int err = 0;
    yyparse_error = 0;
    if(xctx->semaphore >= 2) return;
    unselect_all(1);
    if( set_netlist_dir(0, NULL) ) {
      dbg(1, "callback(): -------------\n");
      if(xctx->netlist_type == CAD_SPICE_NETLIST)
        err = global_spice_netlist(0, 1);
      else if(xctx->netlist_type == CAD_VHDL_NETLIST)
        err = global_vhdl_netlist(0, 1);
      else if(xctx->netlist_type == CAD_VERILOG_NETLIST)
        err = global_verilog_netlist(0, 1);
      else if(xctx->netlist_type == CAD_TEDAX_NETLIST)
        err = global_tedax_netlist(0, 1);
      else
        tcleval("tk_messageBox -type ok -parent [xschem get topwindow] "
                "-message {Please Set netlisting mode (Options menu)}");
      dbg(1, "callback(): -------------\n");
    }
    else {
       if(has_x) tcleval("alert_ {Can not write into the netlist directory. Please check} {}");
       else dbg(0, "Can not write into the netlist directory. Please check");
       err = 1;
    }
    if(err) {
      if(has_x) {
        tclvareval(xctx->top_path, ".menubar entryconfigure Netlist -background red", NULL);
        tclvareval("set tctx::", xctx->current_win_path, "_netlist red", NULL);
      }
    } else {
      if(has_x) {
        tclvareval(xctx->top_path, ".menubar entryconfigure Netlist -background Green", NULL);
        tclvareval("set tctx::", xctx->current_win_path, "_netlist Green", NULL);
      }
    }

    return;
   }
   if(key=='A' && rstate == ControlMask) /* only for graph (toggle hcursor1 if graph_use_ctrl_key set) */
   {
     if(waves_selected(event, key, state, button)) {
       waves_callback(event, mx, my, key, button, aux, state);
       return;
     }
     return;
   }
   if(key=='A' && rstate == 0)                             /* toggle show netlist */
   {
     int net_s;
     if(waves_selected(event, key, state, button)) {
       waves_callback(event, mx, my, key, button, aux, state);
       return;
     }
     net_s = tclgetboolvar("netlist_show");
     net_s = !net_s;
     if(net_s) {
       tcleval("alert_ { enabling show netlist window} {}");
       tclsetvar("netlist_show","1");
     }
     else {
       tcleval("alert_ { disabling show netlist window } {}");
       tclsetvar("netlist_show","0");
     }
     return;
   }
   if(key=='>') {
     if(xctx->semaphore >= 2) return;
     if(xctx->draw_single_layer< cadlayers-1) xctx->draw_single_layer++;
     draw();
     return;
   }
   if(key=='<') {
     if(xctx->semaphore >= 2) return;
     if(xctx->draw_single_layer>=0 ) xctx->draw_single_layer--;
     draw();
     return;
   }
   if(key==':')                         /* toggle flat netlist (only spice)  */
   {
    if(!tclgetboolvar("flat_netlist")) {
        tcleval("alert_ { enabling flat netlist} {}");
        tclsetvar("flat_netlist","1");
    }
    else {
        tcleval("alert_ { set hierarchical netlist } {}");
        tclsetvar("flat_netlist","0");
    }
    return;
   }
   if(key=='B' && rstate == ControlMask) /* only for graph (toggle hcursor2 if graph_use_ctrl_key set) */
   { 
     if(waves_selected(event, key, state, button)) {
       waves_callback(event, mx, my, key, button, aux, state);
       return;
     } 
     return;
   }
   if(key=='B' && rstate == 0)    /* edit schematic header/license */
   {
     if(waves_selected(event, key, state, button)) {
       waves_callback(event, mx, my, key, button, aux, state);
       return;
     }
     tcleval("update_schematic_header");
   }
   if(key=='b' && rstate==0)                     /* merge schematic */
   {
    if(xctx->semaphore >= 2) return;
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return;
    }
    merge_file(0, ""); /* 2nd parameter not used any more for merge 25122002 */
    return;
   }
   if(key=='b' && EQUAL_MODMASK)                     /* hide/show instance details */
   {
    if(xctx->semaphore >= 2) return;
    xctx->hide_symbols++;
    if(xctx->hide_symbols >= 3) xctx->hide_symbols = 0;
    tclsetintvar("hide_symbols", xctx->hide_symbols);
    draw();
    return;
   }
   if(key=='d' && rstate == 0) /* unselect object under the mouse */
   {
     if(infix_interface) {
       unselect_at_mouse_pos(mx, my);
     } else {
       xctx->ui_state |= (MENUSTART | DESEL_CLICK);
       xctx->ui_state2 = MENUSTARTDESEL;
     }
     return;
   }
   if(key=='D' && rstate == 0)          /* unselect by area */
   {
       if( !(xctx->ui_state & STARTPAN) && !xctx->shape_point_selected &&
         !(xctx->ui_state & (PLACE_SYMBOL | PLACE_TEXT)) && !(xctx->ui_state & STARTSELECT)) {
         if(infix_interface) {
           xctx->mx_save = mx; xctx->my_save = my;
           xctx->mx_double_save=xctx->mousex;
           xctx->my_double_save=xctx->mousey;
           xctx->ui_state |= DESEL_AREA;
         } else {
           xctx->ui_state |= MENUSTART;
           xctx->ui_state2 = MENUSTARTDESEL;
         }
       }
     return;
   }
   if(key=='d' && rstate == ControlMask)          /* delete files */
   {
    if(xctx->semaphore >= 2) return;
    delete_files();
    return;
   }
   if(key=='x' && rstate == 0 )                  /* new cad session */
   {
    new_xschem_process(NULL ,0);
    return;
   }
   if((key=='#') && !(state & ControlMask) )
   {
    check_unique_names(0);
    return;
   }
   if((key=='#') && (state & ControlMask) )
   {
    check_unique_names(1);
    return;
   }
   if( 0 && (key==';') && (state & ControlMask) )    /* testmode */
   {
    return;
   }
   if(0 && key=='~' && (state & ControlMask)) {  /* testmode */
    return;
   }
   if(0 && key=='|' && !(state & ControlMask)) {            /* testmode */
    return;
   }
   if(0 && key=='|' && (state & ControlMask))      /* testmode */
   {
    return;
   }

   if(key=='f' && rstate == ControlMask)         /* search */
   {
    if(xctx->semaphore >= 2) return;
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return;
    }     
    tcleval("property_search");
    return;
   }
   if(key=='F' && rstate == ControlMask )   /* full zoom selection */
   {
    if(xctx->ui_state == SELECTION) {
      zoom_full(1, 1, 3, 0.97);
    }
    return;
   }
   if(key=='f' && rstate == 0 )                  /* full zoom */
   { 
    int flags = 1;
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return;
    }
    if(tclgetboolvar("zoom_full_center")) flags |= 2;
    zoom_full(1, 0, flags, 0.97);
    return;
   }
   if((key=='z' && rstate==ControlMask))                         /* zoom out */
   {
     view_unzoom(0.0);
     return;
   }
   if(key=='!' && !(state & ControlMask))
   {
     if(xctx->semaphore >= 2) return;
     break_wires_at_pins(0);
     return;
   }
   if(key=='!' && (state & ControlMask))
   {
     if(xctx->semaphore >= 2) return;
     break_wires_at_pins(1);
     return;
   }
}

static void handle_button_press(int event, int state, int rstate, KeySym key, int button, int mx, int my,
		 double c_snap, int draw_xhair, int crosshair_size, int enable_stretch, int aux  )
{
   int use_cursor_for_sel = tclgetintvar("use_cursor_for_selection");
   dbg(1, "callback(): ButtonPress  ui_state=%d state=%d\n",xctx->ui_state,state);
   if(waves_selected(event, key, state, button)) {
     waves_callback(event, mx, my, key, button, aux, state);
     return;
   }
   /* terminate a schematic pan action */
   if(xctx->ui_state & STARTPAN) {
     xctx->ui_state &=~STARTPAN;
     return;
   }

   /* select instance and connected nets stopping at wire junctions */
   if(button == Button3 &&  state == ControlMask && xctx->semaphore <2)
   {
     Selected sel;
     sel = select_object(xctx->mousex, xctx->mousey, SELECTED, 0, NULL);
     if(sel.type) select_connected_nets(1);
   }
   
   /* break wire at mouse coordinates, move break point to nearest grid point */
   else if(button == Button3 &&  EQUAL_MODMASK && !(state & ShiftMask) && xctx->semaphore <2)
   {
     break_wires_at_point(xctx->mousex_snap, xctx->mousey_snap, 1);
   }
   /* break wire at mouse coordinates */
   else if(button == Button3 && EQUAL_MODMASK && (state & ShiftMask) && xctx->semaphore <2)
   {
     break_wires_at_point(xctx->mousex_snap, xctx->mousey_snap, 0);
   }
   /* select instance and connected nets NOT stopping at wire junctions */
   else if(button == Button3 &&  state == ShiftMask && xctx->semaphore <2)
   {
     Selected sel;
     sel = select_object(xctx->mousex, xctx->mousey, SELECTED, 0, NULL);
     if(sel.type) select_connected_nets(0);
   }
   /* moved to Button3 release */
   /* 
    * else if(button == Button3 &&  state == 0 && xctx->semaphore <2) {
    *   context_menu_action(xctx->mousex_snap, xctx->mousey_snap);
    * }
    */

   /* zoom rectangle by right clicking and drag */
   else if(button == Button3 && state == 0 && xctx->semaphore < 2) {
     zoom_rectangle(START);return;
   }
     
   /* Mouse wheel events */
   else if(handle_mouse_wheel(event, mx, my, key, button, aux, state)) return;

   /* terminate wire placement in snap mode */
   else if(button==Button1 && (state & ShiftMask) && (xctx->ui_state & STARTWIRE) ) {
     snapped_wire(c_snap);
   }
   /* Alt - Button1 click to unselect */
   else if(button==Button1 && (SET_MODMASK) ) {
     unselect_at_mouse_pos(mx, my);
   }
   
   /* Middle button press (Button2) will pan the schematic. */
   else if(button==Button2 && (state == 0)) {
     pan(START, mx, my);
     xctx->ui_state |= STARTPAN;
   }

   /* button1 click to select another instance while edit prop dialog open */
   else if(button==Button1 && xctx->semaphore >= 2) {
     if(tcleval("winfo exists .dialog.textinput")[0] == '1') { /* proc text_line */
       tcleval(".dialog.f1.b1 invoke");
       return;
     } else if(tcleval("winfo exists .dialog.txt")[0] == '1') { /* proc enter_text */
       tcleval(".dialog.buttons.ok invoke");
       return;
     } else if(state==0 && tclgetvar("edit_symbol_prop_new_sel")[0]) {
       tcleval("set edit_symbol_prop_new_sel 1; .dialog.f1.b1 invoke"); /* invoke 'OK' of edit prop dialog */
     } else if((state & ShiftMask) && tclgetvar("edit_symbol_prop_new_sel")[0]) {
       select_object(xctx->mousex, xctx->mousey, SELECTED, 0, NULL);
       tclsetvar("preserve_unchanged_attrs", "1");
       rebuild_selected_array();
     }
   }

   /* Handle the remaining Button1Press events */
   else if(button==Button1) /* MOD button is not pressed here. Processed above */
   {
     xctx->onetime = 0;
     xctx->mouse_moved = 0;
     xctx->drag_elements = 0;

     /* start another wire or line in persistent mode */
     if(tclgetboolvar("persistent_command") && xctx->last_command) {
       if(xctx->last_command == STARTLINE)  start_line(xctx->mousex_snap, xctx->mousey_snap);
       if(xctx->last_command == STARTWIRE)  start_wire(xctx->mousex_snap, xctx->mousey_snap);
       return;
     }
     /* handle all object insertions started from Tools/Edit menu */
     if(check_menu_start_commands(c_snap, mx, my)) return;

     /* complete the pending STARTWIRE, STARTRECT, STARTZOOM, STARTCOPY ... operations */
     if(end_place_move_copy_zoom()) return;

     /* Button1Press to select objects */
     if( !(xctx->ui_state & STARTSELECT) && !(xctx->ui_state & STARTWIRE) && !(xctx->ui_state & STARTLINE) ) {
       Selected sel;
       int already_selected = 0;
       int prev_last_sel = xctx->lastsel;
       int no_shift_no_ctrl = !(state & (ShiftMask | ControlMask));

       xctx->shape_point_selected = 0;
       xctx->mx_save = mx; xctx->my_save = my;
       xctx->mx_double_save=xctx->mousex;
       xctx->my_double_save=xctx->mousey;

       #if 0 /* disabled */
       /* Clicking and dragging from a **selected** instance pin will start a new wire
        * if no other elements are selected */
       if(xctx->lastsel == 1 && xctx->sel_array[0].type==ELEMENT) {
         if(add_wire_from_wire(&xctx->sel_array[0], xctx->mousex_snap, xctx->mousey_snap)) return;
         if(add_wire_from_inst(&xctx->sel_array[0], xctx->mousex_snap, xctx->mousey_snap)) return;
       }
       #endif

       /* In *NON* intuitive interface a button1 press with no modifiers will
        * first unselect everything... 
        * For intuitive interface unselection see below... */
       if(!xctx->intuitive_interface && no_shift_no_ctrl ) unselect_all(1);

       /* find closest object. Use snap coordinates if full crosshair is enabled
        * since the mouse pointer is obscured and crosshair is snapped to grid points */
       if(draw_xhair && (use_cursor_for_sel || crosshair_size == 0)) {
         sel = find_closest_obj(xctx->mousex_snap, xctx->mousey_snap, 0);
       } else {
         sel = find_closest_obj(xctx->mousex, xctx->mousey, 0);
       }
       dbg(1, "sel.type=%d\n", sel.type);
       /* determine if closest object was already selected when button1 was pressed */
       switch(sel.type) {
         case WIRE:    if(xctx->wire[sel.n].sel)          already_selected = 1; break;
         case xTEXT:   if(xctx->text[sel.n].sel)          already_selected = 1; break;
         case LINE:    if(xctx->line[sel.col][sel.n].sel) already_selected = 1; break;
         case POLYGON: if(xctx->poly[sel.col][sel.n].sel) already_selected = 1; break;
         case xRECT:   if(xctx->rect[sel.col][sel.n].sel) already_selected = 1; break;
         case ARC:     if(xctx->arc[sel.col][sel.n].sel)  already_selected = 1; break;
         case ELEMENT: if(xctx->inst[sel.n].sel)          already_selected = 1; break;
         default: break;
       } /*end switch */

       /* Clicking and drag on an instance pin -> drag a new wire */
       if(xctx->intuitive_interface && !already_selected) {
         if(add_wire_from_inst(&sel, xctx->mousex_snap, xctx->mousey_snap)) return;  
       }
  
       /* Clicking and drag on a wire end -> drag a new wire */
       if(xctx->intuitive_interface && !already_selected) {
         if(add_wire_from_wire(&sel, xctx->mousex_snap, xctx->mousey_snap)) return;  
       }

       /* In intuitive interface a button1 press with no modifiers will
        *  unselect everything... we do it here */
       if(xctx->intuitive_interface && !already_selected && no_shift_no_ctrl )  unselect_all(1);

       /* select the object under the mouse and rebuild the selected array */
       if(!already_selected) select_object(xctx->mousex, xctx->mousey, SELECTED, 0, &sel);
       rebuild_selected_array();
       dbg(1, "Button1Press to select objects, lastsel = %d\n", xctx->lastsel);

       /* if clicking on some object endpoints or vertices set shape_point_selected
        * this information will be used in Motion events to draw the stretched vertices */
       if(xctx->lastsel == 1 && xctx->sel_array[0].type==POLYGON) {
         if(edit_polygon_point(state)) return; /* sets xctx->shape_point_selected */
       }
       if(xctx->lastsel == 1 && xctx->intuitive_interface) {
         int cond = already_selected;

         if(cond && xctx->sel_array[0].type==xRECT) {
           if(edit_rect_point(state)) return; /* sets xctx->shape_point_selected */
         }

         if(cond && xctx->sel_array[0].type==LINE) {
           if(edit_line_point(state)) return; /* sets xctx->shape_point_selected */
         }

         if(cond && xctx->sel_array[0].type==WIRE) {
          if(edit_wire_point(state)) return; /* sets xctx->shape_point_selected */
         }
       }
       dbg(1, "shape_point_selected=%d, lastsel=%d\n", xctx->shape_point_selected, xctx->lastsel);

       /* intuitive interface: directly drag elements */
       if(sel.type && xctx->intuitive_interface && xctx->lastsel >= 1 &&
          !xctx->shape_point_selected) {
         /* enable_stretch (from TCL variable) reverses command if enabled:
          * - move --> stretch move
          * - stretch move (with ctrl key) --> move
          */
         int stretch = (state & ControlMask ? 1 : 0) ^ enable_stretch;
         xctx->drag_elements = 1;
         /* select attached nets depending on ControlMask and enable_stretch */
         if(stretch) {
           select_attached_nets(); /* stretch nets that land on selected instance pins */
         }
         /* if dragging instances with stretch enabled and Shift down add wires to pins
          * attached to something */
         if((state & ShiftMask) && stretch) {
           xctx->connect_by_kissing = 2; /* 2 will be used to reset var to 0 at end of move */
           move_objects(START,0,0,0);
         }
         /* dragging away an object with Shift pressed is a copy (duplicate object) */
         else if(state & ShiftMask) copy_objects(START);
         /* else it is a normal move */
         else move_objects(START,0,0,0);
       }

       if(tclgetboolvar("auto_hilight") && !xctx->shape_point_selected) {
         if(!(state & ShiftMask) && xctx->hilight_nets && sel.type == 0 ) {
           if(!prev_last_sel) {
             redraw_hilights(1); /* 1: clear all hilights, then draw */
           }
         }
         hilight_net(0);
         if(xctx->lastsel) {
           redraw_hilights(0);
         }
       }
       return;
     }
   } /* button==Button1 */
}

static void handle_button_release(int event, KeySym key, int state, int button, int mx, int my,
              int aux, double c_snap, int enable_stretch, int draw_xhair)
{
   char str[PATH_MAX + 100];
   if(waves_selected(event, key, state, button)) {
     waves_callback(event, mx, my, key, button, aux, state);
     return;
   }
   xctx->ui_state &= ~DESEL_CLICK;
   dbg(1, "release: shape_point_selected=%d\n", xctx->shape_point_selected);
   /* bring up context menu if no pending operation */
   if(state == Button3Mask && xctx->semaphore <2) {
     if(!end_place_move_copy_zoom()) {
       context_menu_action(xctx->mousex_snap, xctx->mousey_snap);
     }
   }

   /* launcher, no intuitive interface */
   if(!xctx->intuitive_interface && state == (Button1Mask | ControlMask) && 
      !xctx->shape_point_selected && xctx->mouse_moved == 0) {
     int savesem = xctx->semaphore;
     xctx->semaphore = 0;
     launcher(); /* works only if lastsel == 1 */
     xctx->semaphore = savesem;
   }

   /* launcher, intuitive_interface, only if no movement has been done */
   else if(xctx->intuitive_interface && state == (Button1Mask | ControlMask) &&
      !xctx->shape_point_selected && (xctx->ui_state & STARTMOVE) && xctx->mouse_moved == 0) {
     int savesem = xctx->semaphore;
     move_objects(ABORT, 0, 0.0, 0.0);
     unselect_all(1);
     select_object(xctx->mousex, xctx->mousey, SELECTED, 0, NULL);
     rebuild_selected_array();
     xctx->semaphore = 0;
     launcher(); /* works only if lastsel == 1 */
     xctx->semaphore = savesem;
   }

   /* end wire creation when dragging in intuitive interface from an inst pin or wire endpoint */
   else if(state == Button1Mask && xctx->intuitive_interface &&
           (xctx->ui_state & STARTWIRE) && !(xctx->ui_state & MENUSTART)) {
     if(end_place_move_copy_zoom()) return;
   }
 
   /* end intuitive_interface copy or move */
   if(xctx->ui_state & STARTCOPY && xctx->drag_elements) {
      copy_objects(END);
      xctx->constr_mv=0;
      tcleval("set constr_mv 0" );
      xctx->drag_elements = 0;
   }
   else if(xctx->ui_state & STARTMOVE && xctx->drag_elements) {
      move_objects(END,0,0,0);
      xctx->constr_mv=0;
      tcleval("set constr_mv 0" );
      xctx->drag_elements = 0;
   }

   /* if a polygon/bezier/rectangle control point was clicked, end point move operation
    * and set polygon state back to SELECTED from SELECTED1 */
   else if((xctx->ui_state & (STARTMOVE | SELECTION)) && xctx->shape_point_selected) {
     end_shape_point_edit(c_snap);
   }

   if(xctx->ui_state & STARTPAN) {
     xctx->ui_state &=~STARTPAN;
     /* xctx->mx_save = mx; xctx->my_save = my; */
     /* xctx->mx_double_save=xctx->mousex_snap; */
     /* xctx->my_double_save=xctx->mousey_snap; */
     redraw_w_a_l_r_p_z_rubbers(1);
     return;
   }
   dbg(1, "callback(): ButtonRelease  ui_state=%d state=%d\n",xctx->ui_state,state);
   if(xctx->semaphore >= 2) return;
   if(xctx->ui_state & STARTSELECT) {
     if(state & ControlMask) {
       select_rect(!enable_stretch, END,-1);
     } else {
       /* Button1 release: end of rectangle select */
       if(!(state & (Button4Mask|Button5Mask) ) ) {
         select_rect(enable_stretch, END,-1);
       }
     }
     xctx->ui_state &= ~DESEL_AREA;
     rebuild_selected_array();
     my_snprintf(str, S(str), "mouse = %.16g %.16g - selected: %d path: %s",
       xctx->mousex_snap, xctx->mousey_snap, xctx->lastsel, xctx->sch_path[xctx->currsch] );
     statusmsg(str,1);
   }

   /* clear start from menu flag or infix_interface=0 start commands */
   if(xctx->ui_state & MENUSTART) {
     xctx->ui_state &= ~MENUSTART;
     return;
   }
   if(draw_xhair) draw_crosshair(3, state); /* restore crosshair when selecting / unselecting */
}

static void handle_double_click(int event, int state, KeySym key, int button, int mx, int my, int aux )
{
    if( waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      return;
    } else {
     if(xctx->semaphore >= 2) return;
     dbg(1, "callback(): DoubleClick  ui_state=%d state=%d\n",xctx->ui_state,state);
     if(button==Button1) {
       Selected sel;
       if(!xctx->lastsel && xctx->ui_state ==  0) {
         /* Following 5 lines do again a selection overriding lock,
          * so locked instance attrs can be edited */
         sel = select_object(xctx->mousex, xctx->mousey, SELECTED, 1, NULL);
         if(sel.type) {
           xctx->ui_state = SELECTION;
           rebuild_selected_array();
         }
       }
       if(xctx->ui_state ==  0 || xctx->ui_state == SELECTION) {
         edit_property(0);
       } else {
         if(xctx->ui_state & STARTWIRE) {
           xctx->ui_state &= ~STARTWIRE;
         }
         if(xctx->ui_state & STARTLINE) {
           xctx->ui_state &= ~STARTLINE;
         }
         if( (xctx->ui_state & STARTPOLYGON) && (state ==0 ) ) {
           new_polygon(SET, xctx->mousex_snap, xctx->mousey_snap);
         }
       }
     }
    }
}


/* main window callback */
/* mx and my are set to the mouse coord. relative to window  */
/* win_path: set to .drw or sub windows .x1.drw, .x2.drw, ...  */
int callback(const char *win_path, int event, int mx, int my, KeySym key,
                 int button, int aux, int state)
{
 char str[PATH_MAX + 100];
 int redraw_only;
 double c_snap;
#ifndef __unix__
 short cstate = GetKeyState(VK_CAPITAL);
 short nstate = GetKeyState(VK_NUMLOCK);
#else
 XKeyboardState kbdstate;
#endif
int enable_stretch = tclgetboolvar("enable_stretch");
int draw_xhair = tclgetboolvar("draw_crosshair");
int crosshair_size = tclgetintvar("crosshair_size");
int infix_interface = tclgetboolvar("infix_interface");
int rstate; /* (reduced state, without ShiftMask) */

 /* this fix uses an alternative method for getting mouse coordinates on KeyPress/KeyRelease
  * events. Some remote connection softwares do not generate the correct coordinates
  * on such events */
 if(fix_mouse_coord) {
   if(event == KeyPress || event == KeyRelease) {
     tclvareval("getmousex ", win_path, NULL);
     mx = atoi(tclresult());
     tclvareval("getmousey ", win_path, NULL);
     my = atoi(tclresult());
     dbg(1, "mx = %d  my=%d\n", mx, my);
   }
 }

#ifndef __unix__
 if(cstate & 0x0001) { /* caps lock */
   tclvareval(xctx->top_path, ".statusbar.8 configure -state active -text {CAPS LOCK SET! }", NULL);
 } else if (nstate & 0x0001) { /* num lock */
   tclvareval(xctx->top_path, ".statusbar.8 configure -state active -text {NUM LOCK SET! }", NULL);
 } else { /* normal state */
   tclvareval(xctx->top_path, ".statusbar.8 configure -state  normal -text {}", NULL);
 }
#else
 XGetKeyboardControl(display, &kbdstate);
 if(kbdstate.led_mask & 1) { /* caps lock */
   tclvareval(xctx->top_path, ".statusbar.8 configure -state active -text {CAPS LOCK SET! }", NULL);
 } else if(kbdstate.led_mask & 2) { /* num lock */
   tclvareval(xctx->top_path, ".statusbar.8 configure -state active -text {NUM LOCK SET! }", NULL);
 } else { /* normal state */
   tclvareval(xctx->top_path, ".statusbar.8 configure -state  normal -text {}", NULL);
 }
#endif

 tclvareval(xctx->top_path, ".statusbar.7 configure -text $netlist_type", NULL);
 tclvareval(xctx->top_path, ".statusbar.3 delete 0 end;",
                     xctx->top_path, ".statusbar.3 insert 0 $cadsnap",
                     NULL);
 tclvareval(xctx->top_path, ".statusbar.5 delete 0 end;",
                     xctx->top_path, ".statusbar.5 insert 0 $cadgrid",
                     NULL);

 #if 0
 /* exclude Motion and Expose events */
 if(event!=6 /* && event!=12 */) {
   dbg(0, "callback(): state=%d event=%d, win_path=%s, old_win_path=%s, semaphore=%d\n",
           state, event, win_path, old_win_path, xctx->semaphore+1);
 }
 #endif
 
 /* Schematic window context switch */
 redraw_only =0;
 if(strcmp(old_win_path, win_path) ) {
   if( xctx->semaphore >= 1  || event == Expose) {
     dbg(1, "callback(): semaphore >=2 (or Expose) switching window context: %s --> %s\n", old_win_path, win_path);
     redraw_only = 1;
     new_schematic("switch_no_tcl_ctx", win_path, "", 1);
   } else {
     dbg(1, "callback(): switching window context: %s --> %s, semaphore=%d\n", old_win_path, win_path, xctx->semaphore);
     new_schematic("switch", win_path, "", 1);
   }
   tclvareval("housekeeping_ctx", NULL);
 }
 /* artificially set semaphore to allow only redraw operations in switched schematic,
  * so we don't need  to switch tcl context which is costly performance-wise
  */
 if(redraw_only) {
   dbg(1, "callback(): incrementing semaphore for redraw_only\n");
   xctx->semaphore++;
 }

 xctx->semaphore++; /* to recognize recursive callback() calls */

 c_snap = tclgetdoublevar("cadsnap");
 #ifdef __unix__
 state &= (1 <<13) -1; /* filter out anything above bit 12 (4096) */
 #endif
 state &= ~Mod2Mask; /* 20170511 filter out NumLock status */
 state &= ~LockMask; /* filter out Caps Lock */
 rstate = state; /* rstate does not have ShiftMask bit, so easier to test for KeyPress events */
 rstate &= ~ShiftMask; /* don't use ShiftMask, identifying characters is sufficient */
 rstate &= ~Button1Mask; /* ignore button-1 */
 if(xctx->semaphore >= 2)
 {
   if(debug_var>=2)
     if(event != MotionNotify) 
       fprintf(errfp, "callback(): reentrant call of callback(), semaphore=%d, ev=%d, ui_state=%d\n",
               xctx->semaphore, event, xctx->ui_state);
 }
 xctx->mousex=X_TO_XSCHEM(mx);
 xctx->mousey=Y_TO_XSCHEM(my);
 xctx->mousex_snap=my_round(xctx->mousex / c_snap) * c_snap;
 xctx->mousey_snap=my_round(xctx->mousey / c_snap) * c_snap;

 if(abs(mx-xctx->mx_save) > 8 || abs(my-xctx->my_save) > 8 ) {
   my_snprintf(str, S(str), "mouse = %.16g %.16g - selected: %d path: %s",
     xctx->mousex_snap, xctx->mousey_snap, xctx->lastsel, xctx->sch_path[xctx->currsch] );
   statusmsg(str,1);
 }

 dbg(1, "key=%d EQUAL_MODMASK=%d, SET_MODMASK=%d\n", key, SET_MODMASK, EQUAL_MODMASK);

 #if defined(__unix__) && HAS_CAIRO==1
 if(xctx->ui_state & GRABSCREEN) {
   grabscreen(win_path, event, mx, my, key, button, aux, state);
 } else 
 #endif
 switch(event)
 {

  case LeaveNotify:
    if(draw_xhair) draw_crosshair(1, state); /* clear crosshair when exiting window */
    tclvareval(xctx->top_path, ".drw configure -cursor {}" , NULL);
    xctx->mouse_inside = 0;
    break;

  case EnterNotify:
    handle_enter_notify(draw_xhair, crosshair_size);
    break;

  case Expose:
    dbg(1, "callback: Expose, win_path=%s, %dx%d+%d+%d\n", win_path, button, aux, mx, my);
    MyXCopyArea(display, xctx->save_pixmap, xctx->window, xctx->gc[0], mx,my,button,aux,mx,my);
    {
      XRectangle xr[1];
      xr[0].x=(short)mx;
      xr[0].y=(short)my;
      xr[0].width=(unsigned short)button;
      xr[0].height=(unsigned short)aux;
      /* redraw selection on expose, needed if no backing store available on the server 20171112 */
      XSetClipRectangles(display, xctx->gc[SELLAYER], 0,0, xr, 1, Unsorted);
      rebuild_selected_array();
      if(tclgetboolvar("compare_sch") /* && xctx->sch_to_compare[0] */){
        compare_schematics("");
      } else {
        draw_selection(xctx->gc[SELLAYER],0);
      }
      XSetClipMask(display, xctx->gc[SELLAYER], None);
    }
    dbg(1, "callback(): Expose\n");
    break;

  case ConfigureNotify:
    dbg(1,"callback(): ConfigureNotify event\n");
    resetwin(1, 1, 0, 0, 0);
    draw();
    break;

  case MotionNotify:
    handle_motion_notify(event, key, state, rstate, button, mx, my, aux, draw_xhair, enable_stretch);
    break;

  case KeyRelease:
    break;

  case KeyPress:
   handle_key_press(event, key, state, rstate, mx, my, button, aux, 
                      infix_interface, enable_stretch, win_path, c_snap);
   break;

  case ButtonPress:
    handle_button_press(event, state, rstate, key, button, mx, my,
           c_snap, draw_xhair, crosshair_size, enable_stretch, aux);
    break;

  case ButtonRelease:
    handle_button_release(event, key, state, button, mx, my, aux, c_snap, enable_stretch, draw_xhair);
    break;

  case -3:  /* double click  : edit prop */
   handle_double_click(event, state, key, button, mx, my, aux);
   break;
   
  default:
   dbg(1, "callback(): Event:%d\n",event);
   break;
 } /* switch(event) */

 if(xctx->semaphore > 0) xctx->semaphore--;
 if(redraw_only) {
   xctx->semaphore--; /* decrement articially incremented semaphore (see above) */
   dbg(1, "callback(): semaphore >=2 restoring window context: %s <-- %s\n", old_win_path, win_path);
   if(old_win_path[0]) new_schematic("switch_no_tcl_ctx", old_win_path, "", 1);
 }
 else
 if(strcmp(old_win_path, win_path)) {
   if(old_win_path[0]) dbg(1, "callback(): reset old_win_path: %s <- %s\n", old_win_path, win_path);
   my_strncpy(old_win_path, win_path, S(old_win_path));
 }
 return 0;
}

