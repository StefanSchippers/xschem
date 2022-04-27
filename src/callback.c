/* File: callback.c
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

static int waves_selected(int event, int key, int state, int button)
{
  int i;
  int is_inside = 0, skip = 0;
  static unsigned int excl = STARTZOOM | STARTRECT | STARTLINE | STARTWIRE |
                             STARTPAN | STARTSELECT | STARTMOVE | STARTCOPY;
  if(xctx->ui_state & excl) skip = 1;
  else if(state & Mod1Mask) skip = 1;
  else if(event == MotionNotify && (state & Button2Mask)) skip = 1;
  else if(event == MotionNotify && (state & Button1Mask) && (state & ShiftMask)) skip = 1;
  else if(event == ButtonPress && button == Button2) skip = 1;
  else if(event == ButtonPress && button == Button1 && (state & ShiftMask) ) skip = 1;
  else if(event == ButtonRelease && button == Button2) skip = 1;
  else if(event == KeyPress && (state & ShiftMask)) skip = 1;
  else if(!skip) for(i=0; i< xctx->rects[GRIDLAYER]; i++) {
    xRect *r;
    r = &xctx->rect[GRIDLAYER][i];
    if(!(r->flags & 1) ) continue;
    if( (xctx->ui_state & GRAPHPAN) ||
       POINTINSIDE(xctx->mousex, xctx->mousey, r->x1,  r->y1,  r->x2 - 40,  r->y1 + 20) ||
       POINTINSIDE(xctx->mousex, xctx->mousey, r->x1 + 20,  r->y1,  r->x2 - 30,  r->y2 - 10) ) {
       is_inside = 1;
       tclvareval(xctx->top_path, ".drw configure -cursor tcross" , NULL);
    }
  }
  if(!is_inside) {
    tclvareval(xctx->top_path, ".drw configure -cursor arrow" , NULL);
    if(xctx->graph_flags & 64) {
      tcleval("graph_show_measure stop");
    }
  }
  return is_inside;
}

void redraw_w_a_l_r_p_rubbers(void)
{
  if(xctx->ui_state & STARTWIRE) {
    if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
    if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
    new_wire(RUBBER, xctx->mousex_snap, xctx->mousey_snap);
  }
  if(xctx->ui_state & STARTARC) {
    if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
    if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
    new_arc(RUBBER, 0);
  }
  if(xctx->ui_state & STARTLINE) {
    if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
    if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
    new_line(RUBBER);
  }
  if(xctx->ui_state & STARTRECT) new_rect(RUBBER);
  if(xctx->ui_state & STARTPOLYGON) {
    if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
    if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
    new_polygon(RUBBER);
  }
}

static void abort_operation(void)
{
  xctx->no_draw = 0;
  tcleval("set constrained_move 0" );
  constrained_move=0;
  xctx->last_command=0;
  xctx->manhattan_lines = 0;
  dbg(1, "abort_operation(): Escape: ui_state=%ld\n", xctx->ui_state);
  if(xctx->ui_state & STARTMOVE)
  {
   move_objects(ABORT,0,0,0);
   if(xctx->ui_state & (START_SYMPIN | PLACE_SYMBOL | PLACE_TEXT)) {
     delete(1/* to_push_undo */);
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
  unselect_all();
  draw();
}

static void start_place_symbol(double mx, double my)
{
    xctx->last_command = 0;
    rebuild_selected_array();
    if(xctx->lastsel && xctx->sel_array[0].type==ELEMENT) {
      tclvareval("set INITIALINSTDIR [file dirname {",
           abs_sym_path(xctx->inst[xctx->sel_array[0].n].name, ""), "}]", NULL);
    } 
    unselect_all();
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
      if(constrained_move != 2) {
        xctx->mx_double_save=xctx->mousex_snap;
      }
      if(constrained_move != 1) {
        xctx->my_double_save=xctx->mousey_snap;
      }
      if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
      if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
    } else {
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
    }
    new_line(PLACE);
}

static void start_wire(double mx, double my)
{
     xctx->last_command = STARTWIRE;
     if(xctx->ui_state & STARTWIRE) {
       if(constrained_move != 2) {
         xctx->mx_double_save=xctx->mousex_snap;
       }
       if(constrained_move != 1) {
         xctx->my_double_save=xctx->mousey_snap;
       }
       if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
       if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
     } else {
       xctx->mx_double_save=xctx->mousex_snap;
       xctx->my_double_save=xctx->mousey_snap;
     }
     new_wire(PLACE,xctx->mousex_snap, xctx->mousey_snap);

}

/* process user input (arrow keys for now) when only graphs are selected */

/* xctx->graph_flags:
 *  1: dnu, reserved, used in draw_graphs()
 *  2: draw x-cursor1
 *  4: draw x-cursor2
 *  8: dnu, reserved, used in draw_graphs()
 * 16: move cursor1
 * 32: move cursor2
 * 64: show measurement tooltip
 */
static int waves_callback(int event, int mx, int my, KeySym key, int button, int aux, int state)
{
  Graph_ctx *gr;
  const char *val;
  int i, need_all_redraw = 0, need_redraw = 0, dataset = 0;
  double xx1, xx2, yy1, yy2;
  double delta_threshold = 0.25;
  double zoom_m = 0.5;
  int save_mouse_at_end = 0, clear_graphpan_at_end = 0;
  #if HAS_CAIRO==1
  cairo_save(xctx->cairo_ctx);
  cairo_save(xctx->cairo_save_ctx);
  cairo_select_font_face(xctx->cairo_ctx, "Sans-Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_select_font_face(xctx->cairo_save_ctx, "Sans-Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  #endif
  gr = &xctx->graph_struct;
  for(i=0; i < xctx->rects[GRIDLAYER]; i++) {
    xRect *r;
    if( (xctx->ui_state & GRAPHPAN) && i != xctx->graph_master) continue;
    r = &xctx->rect[GRIDLAYER][i];
    /* process only graph boxes */
    if(!(r->flags & 1) ) continue;
    /* check if this is the master graph (the one containing the mouse pointer) */
    /* determine if mouse pointer is below xaxis or left of yaxis in some graph */
    if( POINTINSIDE(xctx->mousex, xctx->mousey, r->x1, r->y1, r->x2, r->y2)) {
      setup_graph_data(i, xctx->graph_flags, 0, gr);

      /* move cursor1 */
      /* set cursor position from master graph x-axis */
      if(event == MotionNotify && (state & Button1Mask) && (xctx->graph_flags & 16 )) {
        xctx->graph_cursor1_x = G_X(xctx->mousex);
      }
      /* move cursor2 */
      /* set cursor position from master graph x-axis */
      else if(event == MotionNotify && (state & Button1Mask) && (xctx->graph_flags & 32 )) {
        xctx->graph_cursor2_x = G_X(xctx->mousex);
      }
      if(xctx->ui_state & GRAPHPAN) break; /* After GRAPHPAN only need to check Motion events for cursors */
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
      xctx->graph_master = i;
      zoom_m = (xctx->mousex  - gr->x1) / gr->w;
      if(event == ButtonPress && button == Button1) {
        /* dragging cursors when mouse is very close */
        if( (xctx->graph_flags & 2) && fabs(xctx->mousex - W_X(xctx->graph_cursor1_x)) < 10) {
          xctx->graph_flags |= 16; /* Start move cursor1 */
        }
        if( (xctx->graph_flags & 4) && fabs(xctx->mousex - W_X(xctx->graph_cursor2_x)) < 10) {
          xctx->graph_flags |= 32; /* Start move cursor2 */
        }
      }
      else if(event == -3 && button == Button1) {
        if(!edit_wave_attributes(1, i, gr)) {
          tclvareval("graph_edit_properties ", my_itoa(i), NULL);
        }
      }
      /* x cursor1 toggle */
      else if((key == 'a') ) {
        xctx->graph_flags ^= 2;
        need_all_redraw = 1;
        if(xctx->graph_flags & 2) xctx->graph_cursor1_x = G_X(xctx->mousex);
      }
      /* x cursor2 toggle */
      else if((key == 'b') ) {
        xctx->graph_flags ^= 4;
        need_all_redraw = 1;
        if(xctx->graph_flags & 4) xctx->graph_cursor2_x = G_X(xctx->mousex);
      }
      /* measurement tooltip */
      else if((key == 'm') ) {
        xctx->graph_flags ^= 64;
        if(!(xctx->graph_flags & 64)) {
          tcleval("graph_show_measure stop");
        }
      }
      break;
    } /* if( POINTINSIDE(...) */
  } /* for(i=0; i <  xctx->rects[GRIDLAYER]; i++) */

  /* check if user clicked on a wave label -> draw wave in bold */
  if(event == ButtonPress && button == Button3 &&
           edit_wave_attributes(2, i, gr)) {
    draw_graph(i, 1 + 8 + (xctx->graph_flags & 6), gr); /* draw data in graph box */
    return 0;
  }
  /* save mouse position when doing pan operations */
  else if(
      (
        (event == ButtonPress && (button == Button1 || button == Button3)) ||
        (event == MotionNotify && (state & (Button1Mask | Button3Mask)))
      ) &&
      !(xctx->ui_state & GRAPHPAN) && 
      !xctx->graph_top /* && !xctx->graph_bottom */
    ) {
    xctx->ui_state |= GRAPHPAN;
    if(!xctx->graph_left) xctx->mx_double_save = xctx->mousex_snap;
    if(xctx->graph_left) xctx->my_double_save = xctx->mousey_snap;
  }
  gr->master_gx1 = 0;
  gr->master_gx2 = 1e-6;
  val = get_tok_value(xctx->rect[GRIDLAYER][xctx->graph_master].prop_ptr,"x1",0);
  if(val[0]) gr->master_gx1 = atof(val);
  val = get_tok_value(xctx->rect[GRIDLAYER][xctx->graph_master].prop_ptr,"x2",0);
  if(val[0]) gr->master_gx2 = atof(val);
  if(gr->master_gx1 == gr->master_gx2) gr->master_gx2 += 1e-6;
  gr->master_gw = gr->master_gx2 - gr->master_gx1;

  /* second loop: after having determined the master graph do the others */
  for(i=0; i< xctx->rects[GRIDLAYER]; i++) {
    xRect *r;
    r = &xctx->rect[GRIDLAYER][i];
    need_redraw = 0;
    if( !(r->flags & 1) ) continue;
    gr->gx1 = gr->master_gx1;
    gr->gx2 = gr->master_gx2;
    gr->gw = gr->master_gw;
    setup_graph_data(i, xctx->graph_flags, 1, gr); /* skip flag set, no reload x1 and x2 fields */
    if(gr->dataset >= 0 && gr->dataset < xctx->graph_datasets) dataset =gr->dataset;
    else dataset = -1;
    /* destroy / show measurement widget */
    if(i == xctx->graph_master) {
      if(xctx->graph_flags & 64) {
        if( POINTINSIDE(xctx->mousex, xctx->mousey, gr->x1, gr->y1, gr->x2, gr->y2)) {
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
          if(xctx->graph_sim_type == 3) xval = pow(10, xval);
          if(gr->unitx != 0) 
            my_snprintf(sx, S(sx), "%.4g%c", gr->unitx * xval, gr->unitx_suffix);
          else
            my_snprintf(sx, S(sx), "%.4g", xval);

          if(gr->unitx != 0)
            my_snprintf(sy, S(sy), "%.4g%c", gr->unity * yval, gr->unity_suffix);
          else
            my_snprintf(sy, S(sy), "%.4g", yval);
  
          tclvareval("set measure_text \"y=", sy, "\nx=", sx, "\"", NULL);
          tcleval("graph_show_measure");
        } else {
          tcleval("graph_show_measure stop");
        }
      }
    }
    dbg(1, "%g %g %g %g - %d %d\n", gr->gx1, gr->gy1, gr->gx2, gr->gy2, gr->divx, gr->divy);
    if( event == KeyPress || event == ButtonPress || event == MotionNotify ) {
      /* move cursor1 */
      if(event == MotionNotify && (state & Button1Mask) && (xctx->graph_flags & 16 )) {
        need_redraw = 1;
      }
      /* move cursor2 */
      else if(event == MotionNotify && (state & Button1Mask) && (xctx->graph_flags & 32 )) {
        need_redraw = 1;
      }
      else /* drag waves with mouse */
      if(event == MotionNotify && (state & Button1Mask) && !xctx->graph_bottom) {
        double delta;
        if(xctx->graph_left) {
          if(i == xctx->graph_master) {
            if(gr->digital) {
              delta = gr->posh;
              delta_threshold = 0.01;
              if(fabs(xctx->my_double_save - xctx->mousey_snap) > fabs(gr->dcy * delta) * delta_threshold) {
                yy1 = gr->ypos1 + (xctx->my_double_save - xctx->mousey_snap) / gr->dcy;
                yy2 = gr->ypos2 + (xctx->my_double_save - xctx->mousey_snap) / gr->dcy;
                my_strdup(1424, &r->prop_ptr, subst_token(r->prop_ptr, "ypos1", dtoa(yy1)));
                my_strdup(1425, &r->prop_ptr, subst_token(r->prop_ptr, "ypos2", dtoa(yy2)));
                xctx->my_double_save = xctx->mousey_snap;
                need_redraw = 1;
              }
            } else {
              delta = gr->gh / gr->divy;
              delta_threshold = 0.01;
              if(fabs(xctx->my_double_save - xctx->mousey_snap) > fabs(gr->cy * delta) * delta_threshold) {
                yy1 = gr->gy1 + (xctx->my_double_save - xctx->mousey_snap) / gr->cy;
                yy2 = gr->gy2 + (xctx->my_double_save - xctx->mousey_snap) / gr->cy;
                my_strdup(1435, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
                my_strdup(1437, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
                xctx->my_double_save = xctx->mousey_snap;
                need_redraw = 1;
              }
            }
          }
        } else {
          save_mouse_at_end = 1;
          delta = gr->gw;
          delta_threshold = 0.01;
          if( r->sel || !(r->flags & 2) || i == xctx->graph_master) {
            if(fabs(xctx->mx_double_save - xctx->mousex_snap) > fabs(gr->cx * delta) * delta_threshold) {
              xx1 = gr->gx1 + (xctx->mx_double_save - xctx->mousex_snap) / gr->cx;
              xx2 = gr->gx2 + (xctx->mx_double_save - xctx->mousex_snap) / gr->cx;
              my_strdup(1410, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
              my_strdup(1411, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
              need_redraw = 1;
            }
          }
        }
      }
      else if((button == Button5 && !(state & ShiftMask))) {
        double delta;
        if(xctx->graph_left) {
          if(i == xctx->graph_master) {
            if(gr->digital) {
              delta = gr->posh * 0.05;
              yy1 = gr->ypos1 + delta;
              yy2 = gr->ypos2 + delta;
              my_strdup(1456, &r->prop_ptr, subst_token(r->prop_ptr, "ypos1", dtoa(yy1)));
              my_strdup(1457, &r->prop_ptr, subst_token(r->prop_ptr, "ypos2", dtoa(yy2)));
              need_redraw = 1;
            } else {
              delta = gr->gh/ gr->divy;
              delta_threshold = 1.0;
              yy1 = gr->gy1 + delta * delta_threshold;
              yy2 = gr->gy2 + delta * delta_threshold;
              my_strdup(1420, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
              my_strdup(1421, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
              need_redraw = 1;
            }
          }
        } else {
          if( r->sel || !(r->flags & 2) || i == xctx->graph_master) {
            delta = gr->gw;
            delta_threshold = 0.05;
            xx1 = gr->gx1 - delta * delta_threshold;
            xx2 =gr->gx2 - delta * delta_threshold;
            my_strdup(1428, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
            my_strdup(1429, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
            need_redraw = 1;
          }
        }
      }
      else if(key == XK_Left) {
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
            my_strdup(1451, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
            my_strdup(1448, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
            need_redraw = 1;
          }
        } else {
          delta = gr->gw;
          delta_threshold = 0.05;
          xx1 = gr->gx1 - delta * delta_threshold;
          xx2 = gr->gx2 - delta * delta_threshold;
          my_strdup(1395, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
          my_strdup(1396, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
          need_redraw = 1;
        }
      }
      else if(button == Button4 && !(state & ShiftMask))  {
        double delta;
        if(xctx->graph_left) {
          if(i == xctx->graph_master) {
            if(gr->digital) {
              delta = gr->posh * 0.05;
              yy1 = gr->ypos1 - delta;
              yy2 = gr->ypos2 - delta;
              my_strdup(1416, &r->prop_ptr, subst_token(r->prop_ptr, "ypos1", dtoa(yy1)));
              my_strdup(1417, &r->prop_ptr, subst_token(r->prop_ptr, "ypos2", dtoa(yy2)));
              need_redraw = 1;
            } else {
              delta = gr->gh / gr->divy;
              delta_threshold = 1.0;
              yy1 = gr->gy1 - delta * delta_threshold;
              yy2 = gr->gy2 - delta * delta_threshold;
              my_strdup(1458, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
              my_strdup(1434, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
              need_redraw = 1;
            }
          }
        } else {
          if(r->sel || !(r->flags & 2) || i == xctx->graph_master) {
            delta = gr->gw;
            delta_threshold = 0.05;
            xx1 = gr->gx1 + delta * delta_threshold;
            xx2 = gr->gx2 + delta * delta_threshold;
            my_strdup(1397, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
            my_strdup(1398, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
            need_redraw = 1;
          }
        }
      }
      else if(key == XK_Right) {
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
            my_strdup(1433, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
            my_strdup(1439, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
            need_redraw = 1;
          }
        } else {
          delta = gr->gw;
          delta_threshold = 0.05;
          xx1 = gr->gx1 + delta * delta_threshold;
          xx2 = gr->gx2 + delta * delta_threshold;
          my_strdup(1430, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
          my_strdup(1431, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
          need_redraw = 1;
        }
      }
      else if(button == Button5 && (state & ShiftMask)) {
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
              my_strdup(1461, &r->prop_ptr, subst_token(r->prop_ptr, "ypos1", dtoa(yy1)));
              my_strdup(1459, &r->prop_ptr, subst_token(r->prop_ptr, "ypos2", dtoa(yy2)));
              need_redraw = 1;

            } else {
              double m = G_Y(xctx->mousey);
              double a = m - gr->gy1;
              double b = gr->gy2 -m;
              double delta = (gr->gy2 - gr->gy1);
              double var = delta * 0.2;
              yy2 = gr->gy2 + var * b / delta;
              yy1 = gr->gy1 - var * a / delta;
              my_strdup(1460, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
              my_strdup(1438, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
              need_redraw = 1;
            }
          }
        } else {
          if(r->sel || !(r->flags & 2) || i == xctx->graph_master) {
            double var = 0.2 * gr->gw;
            xx2 = gr->gx2 + var * (1 - zoom_m);
            xx1 = gr->gx1 - var * zoom_m;
            my_strdup(1399, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
            my_strdup(1400, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
            need_redraw = 1;
          }
        }
      }
      else if(key == XK_Down) {
        if(!xctx->graph_left) {
          if(r->sel || !(r->flags & 2) || i == xctx->graph_master) {
            double var = 0.2 * gr->gw;
            xx2 = gr->gx2 + var * (1 - zoom_m);
            xx1 = gr->gx1 - var * zoom_m;
            my_strdup(1418, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
            my_strdup(1432, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
            need_redraw = 1;
          }
        }
      }
      else if(button == Button4 && (state & ShiftMask)) {
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
              my_strdup(1447, &r->prop_ptr, subst_token(r->prop_ptr, "ypos1", dtoa(yy1)));
              my_strdup(1436, &r->prop_ptr, subst_token(r->prop_ptr, "ypos2", dtoa(yy2)));
              need_redraw = 1;
            } else {
              double m = G_Y(xctx->mousey);
              double a = m - gr->gy1;
              double b = gr->gy2 -m;
              double delta = (gr->gy2 - gr->gy1);
              double var = delta * 0.2;
              yy2 = gr->gy2 - var * b / delta;
              yy1 = gr->gy1 + var * a / delta;
              my_strdup(1419, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(yy1)));
              my_strdup(1444, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(yy2)));
              need_redraw = 1;
            }
          }
        } else {
          if(r->sel || !(r->flags & 2) || i == xctx->graph_master) {
            double var = 0.2 * gr->gw;
            xx2 = gr->gx2 - var * (1 - zoom_m);
            xx1 = gr->gx1 + var * zoom_m;
            my_strdup(1449, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
            my_strdup(1450, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
            need_redraw = 1;
          }
        }
      }
      else if(key == XK_Up) {
        if(!xctx->graph_left) {
          if(r->sel || !(r->flags & 2) || i == xctx->graph_master) {
            double var = 0.2 * gr->gw;
            xx2 = gr->gx2 - var * (1 - zoom_m);
            xx1 = gr->gx1 + var * zoom_m;
            my_strdup(1445, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
            my_strdup(1446, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
            need_redraw = 1;
          }
        }
      }
      else if(key == 'f') {
        if(xctx->graph_values) {
          if(xctx->graph_left) {
            if(i == xctx->graph_master) {
              if(!gr->digital) {
                int dset;
                int i, j;
                const char *bus_msb = NULL;
                int sweep_idx = 0;
                double v, start, end;
                double min=0.0, max=0.0;
                int first = 1;
                char *saves, *sptr, *stok, *sweep = NULL, *saven, *nptr, *ntok, *node = NULL;
                my_strdup2(1426, &node, get_tok_value(r->prop_ptr,"node",0));
                my_strdup2(542, &sweep, get_tok_value(r->prop_ptr,"sweep",0));
                nptr = node;
                sptr = sweep;
                start = (gr->gx1 <= gr->gx2) ? gr->gx1 : gr->gx2;
                end = (gr->gx1 <= gr->gx2) ? gr->gx2 : gr->gx1;

                while( (ntok = my_strtok_r(nptr, "\n\t ", "\"", &saven)) ) {
                  stok = my_strtok_r(sptr, "\n\t ", "\"", &saves);
                  nptr = sptr = NULL;
                  if(stok && stok[0]) {
                    sweep_idx = get_raw_index(stok);
                    if( sweep_idx == -1) sweep_idx = 0;
                  }
                  bus_msb = strstr(ntok, ",");
                  j = -1;
                  if(!bus_msb && xctx->graph_values) {
                    char *express = NULL;
                    if(strstr(ntok, ";")) {
                      my_strdup2(1505, &express, find_nth(ntok, ";", 2));
                    } else {
                      my_strdup2(1506, &express, ntok);
                    }
                    if(strstr(express, " ")) {
                      /* just probe a single point to get the index. custom data column already calculated */
                      j = calc_custom_data_yrange(sweep_idx, express, gr);
                    } else {
                      j = get_raw_index(express);
                    }
                    my_free(1507, &express);
                  }
                  if(j >= 0) {
                    int ofs = 0;
                    for(dset = 0 ; dset < xctx->graph_datasets; dset++) {
                      for(i = ofs; i < ofs + xctx->graph_npoints[dset]; i++) {
                        if(dataset >= 0 && dataset != dset) continue;
                        if( xctx->graph_values[sweep_idx][i] < start ||
                            xctx->graph_values[sweep_idx][i] > end)  continue;
                        v = xctx->graph_values[j][i];
                        if(first || v < min) min = v;
                        if(first || v > max) max = v;
                        first = 0;
                      } 
                      ofs += xctx->graph_npoints[dset];
                    }
                  }
                }
                if(max == min) max += 0.01;
                min = floor_to_n_digits(min, 2);
                max = ceil_to_n_digits(max, 2);
                my_free(1427, &node);
                my_free(573, &sweep);
                my_strdup(1422, &r->prop_ptr, subst_token(r->prop_ptr, "y1", dtoa(min)));
                my_strdup(1423, &r->prop_ptr, subst_token(r->prop_ptr, "y2", dtoa(max)));
                need_redraw = 1;
              } else { /* digital plot */
                my_strdup(1497, &r->prop_ptr, subst_token(r->prop_ptr, "ypos1",
                   get_tok_value(r->prop_ptr, "y1", 0) ));
                my_strdup(1498, &r->prop_ptr, subst_token(r->prop_ptr, "ypos2",
                   get_tok_value(r->prop_ptr, "y2", 0) ));
                need_redraw = 1;
              }
            } /* graph_master */
          } else { /* not graph_left */
            int dset = dataset == -1 ? 0 : dataset;
            if(r->sel || !(r->flags & 2) || i == xctx->graph_master) {
              xx1 = get_raw_value(dset, 0, 0);
              xx2 = get_raw_value(dset, 0, xctx->graph_npoints[dset] -1);
              my_strdup(1409, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
              my_strdup(1412, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
              need_redraw = 1;
            }
          }
        } /* graph_values */
      } /* key == 'f' */
      else if( event == MotionNotify && (state & Button1Mask) && xctx->graph_bottom ) {
        double wwx1, wwx2, p, delta, ccx, ddx;

        if(xctx->graph_values) {
          if(r->sel || !(r->flags & 2) || i == xctx->graph_master) {
            int dset = dataset == -1 ? 0 : dataset;
            delta = gr->gw;
            wwx1 =  get_raw_value(dset, 0, 0);
            wwx2 = get_raw_value(dset, 0, xctx->graph_npoints[dset] - 1);
            ccx = (gr->x2 - gr->x1) / (wwx2 - wwx1);
            ddx = gr->x1 - wwx1 * ccx;
            p = (xctx->mousex_snap - ddx) / ccx;
            xx1 = p - delta / 2.0;
            xx2 = p + delta / 2.0;
            my_strdup(1442, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
            my_strdup(1443, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
            need_redraw = 1;
          }
        }
      }
    } /* if( event == KeyPress || event == ButtonPress || event == MotionNotify ) */
    else if( event == ButtonRelease) {
      if(button != Button3) {
        xctx->ui_state &= ~GRAPHPAN;
        xctx->graph_flags &= ~(16 | 32); /* clear move cursor flags */
      }
      /* zoom area by mouse drag */
      else if(button == Button3 && (xctx->ui_state & GRAPHPAN) && 
              !xctx->graph_left && !xctx->graph_top) {
        if(r->sel || !(r->flags & 2) || i == xctx->graph_master) {
          if(xctx->mx_double_save != xctx->mousex_snap) {
            double tmp;
            clear_graphpan_at_end = 1;
            xx1 = G_X(xctx->mx_double_save);
            xx2 = G_X(xctx->mousex_snap);
            if(xx2 < xx1) { tmp = xx1; xx1 = xx2; xx2 = tmp; }
            if(xx1 == xx2) xx2 += 1e-6;
            my_strdup(1440, &r->prop_ptr, subst_token(r->prop_ptr, "x1", dtoa(xx1)));
            my_strdup(1441, &r->prop_ptr, subst_token(r->prop_ptr, "x2", dtoa(xx2)));
            need_redraw = 1;
          }
        }
      }
    } /* else if( event == ButtonRelease) */
    if(need_redraw || need_all_redraw) {
      setup_graph_data(i, xctx->graph_flags, 0, gr);
      draw_graph(i, 1 + 8 + (xctx->graph_flags & 6), gr); /* draw data in each graph box */
    }
  } /* for(i=0; i< xctx->rects[GRIDLAYER]; i++ */


  if(clear_graphpan_at_end) xctx->ui_state &= ~GRAPHPAN;
  /* update saved mouse position after processing all graphs */
  if(save_mouse_at_end && 
    fabs(xctx->mx_double_save - xctx->mousex_snap) > fabs(gr->cx * gr->gw) * delta_threshold) {
    xctx->mx_double_save = xctx->mousex_snap;
    xctx->my_double_save = xctx->mousey_snap;
  }


  draw_selection(xctx->gc[SELLAYER], 0);
  #if HAS_CAIRO==1
  cairo_restore(xctx->cairo_ctx);
  cairo_restore(xctx->cairo_save_ctx);
  #endif
  return 0;
}

/* main window callback */
/* mx and my are set to the mouse coord. relative to window  */
int callback(const char *winpath, int event, int mx, int my, KeySym key,
                 int button, int aux, int state)
{
 static char old_winpath[PATH_MAX] = ""; /* previous focused window, used to do context switch */
 char str[PATH_MAX + 100];
 struct stat buf;
 int redraw_only;
 unsigned short sel;
 double c_snap;
#ifndef __unix__
 short cstate = GetKeyState(VK_CAPITAL);
 short nstate = GetKeyState(VK_NUMLOCK);
#else
 XKeyboardState kbdstate;
#endif


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
 if(event!=6 /* && event!=12 */) dbg(0, "callback(): event=%d, winpath=%s, old_winpath=%s, semaphore=%d\n",
                                      event, winpath, old_winpath, xctx->semaphore+1);
 #endif
 
 /* Schematic window context switch */
 redraw_only =0;
 if(strcmp(old_winpath, winpath) ) {
   if( xctx->semaphore >= 1  || event == Expose) {
     dbg(1, "callback(): semaphore >=2 (or Expose) switching window context: %s --> %s\n", old_winpath, winpath);
     redraw_only = 1;
   } else {
     dbg(1, "callback(): switching window context: %s --> %s, semaphore=%d\n", old_winpath, winpath, xctx->semaphore);
     if(old_winpath[0]) tclvareval("save_ctx ", old_winpath, NULL);
     tclvareval("restore_ctx ", winpath, NULL);
     tclvareval("housekeeping_ctx", NULL);
   }
   new_schematic("switch_win", winpath, "");
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
 state &=~Mod2Mask; /* 20170511 filter out NumLock status */
 if(xctx->semaphore >= 2)
 {
   if(debug_var>=2)
     if(event != MotionNotify) 
       fprintf(errfp, "callback(): reentrant call of callback(), semaphore=%d, ev=%d, ui_state=%ld\n",
               xctx->semaphore, event, xctx->ui_state);
 }
 xctx->mousex=X_TO_XSCHEM(mx);
 xctx->mousey=Y_TO_XSCHEM(my);
 xctx->mousex_snap=my_round(xctx->mousex / c_snap) * c_snap;
 xctx->mousey_snap=my_round(xctx->mousey / c_snap) * c_snap;
 my_snprintf(str, S(str), "mouse = %.16g %.16g - selected: %d path: %s",
   xctx->mousex_snap, xctx->mousey_snap, xctx->lastsel, xctx->sch_path[xctx->currsch] );
 statusmsg(str,1);

 switch(event)
 {
  case EnterNotify:
    if(!xctx->sel_or_clip[0]) my_snprintf(xctx->sel_or_clip, S(xctx->sel_or_clip), "%s/%s",
        user_conf_dir, ".selection.sch");

    /* xschem window *sending* selected objects
       when the pointer comes back in abort copy operation since it has been done
       in another xschem xctx->window; STARTCOPY set and selection file does not exist any more */
    if( stat(xctx->sel_or_clip, &buf)  && (xctx->ui_state & STARTCOPY) )
    {
      copy_objects(ABORT); /* also unlinks sel_or_flip file */
      unselect_all();
    }
    /* xschem window *receiving* selected objects */
    /* no selected objects and selection file exists */
    if(xctx->lastsel == 0  && !stat(xctx->sel_or_clip, &buf)) {
      dbg(2, "callback(): Enter event\n");
      xctx->mousex_snap = 490;
      xctx->mousey_snap = -340;
      merge_file(1, ".sch");
      xunlink(xctx->sel_or_clip);
    }
    break;

  case Expose:
    dbg(1, "callback: Expose, winpath=%s, %dx%d+%d+%d\n", winpath, button, aux, mx, my);
    MyXCopyArea(display, xctx->save_pixmap, xctx->window, xctx->gctiled, mx,my,button,aux,mx,my);
    {
      XRectangle xr[1];
      xr[0].x=mx;
      xr[0].y=my;
      xr[0].width=button;
      xr[0].height=aux;
      /* redraw selection on expose, needed if no backing store available on the server 20171112 */
      XSetClipRectangles(display, xctx->gc[SELLAYER], 0,0, xr, 1, Unsorted);
      rebuild_selected_array();
      draw_selection(xctx->gc[SELLAYER],0);
      XSetClipMask(display, xctx->gc[SELLAYER], None);
    }
    dbg(1, "callback(): Expose\n");
    break;
  case ConfigureNotify:
    resetwin(1, 1, 0, 0, 0);
    draw();
    break;

  case MotionNotify:
    if( waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      break;
    }
    if(xctx->ui_state & STARTPAN)   pan(RUBBER, mx, my);
    #ifndef __unix__
    if ((xctx->ui_state & STARTWIRE) || (xctx->ui_state & STARTARC) ||
        (xctx->ui_state & STARTLINE) || (xctx->ui_state & STARTMOVE) ||
        (xctx->ui_state & STARTCOPY) || (xctx->ui_state & STARTRECT) ||
        (xctx->ui_state & STARTPOLYGON) || (xctx->ui_state & STARTPAN) ||
        (xctx->ui_state & STARTSELECT)) {
      MyXCopyArea(display, xctx->save_pixmap, xctx->window, xctx->gctiled, xctx->xrect[0].x, xctx->xrect[0].y,
        xctx->xrect[0].width, xctx->xrect[0].height, xctx->xrect[0].x, xctx->xrect[0].y);
    }
    #endif
    if(xctx->semaphore >= 2) break;
    if(xctx->ui_state) {
      my_snprintf(str, S(str), "mouse = %.16g %.16g - selected: %d w=%.16g h=%.16g",
        xctx->mousex_snap, xctx->mousey_snap,
        xctx->lastsel ,
        xctx->mousex_snap-xctx->mx_double_save, xctx->mousey_snap-xctx->my_double_save
      );
      statusmsg(str,1);
    }
    if(xctx->ui_state & STARTZOOM)   zoom_rectangle(RUBBER);
    if(xctx->ui_state & STARTSELECT && !(xctx->ui_state & (PLACE_SYMBOL | STARTPAN | PLACE_TEXT)) ) {
      if( (state & Button1Mask)  && (state & Mod1Mask)) { /* 20171026 added unselect by area  */
          select_rect(RUBBER,0);
      } else if(state & Button1Mask) {
        select_rect(RUBBER,1);
      }
    }
    if(xctx->ui_state & STARTMOVE) {
      if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
      if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
      move_objects(RUBBER,0,0,0);
    }
    if(xctx->ui_state & STARTCOPY) {
      if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
      if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
      copy_objects(RUBBER);
    }

    redraw_w_a_l_r_p_rubbers();
    /* start of a mouse area select */
    if(!(xctx->ui_state & STARTPOLYGON) && (state&Button1Mask) && !(xctx->ui_state & STARTWIRE) && 
       !(xctx->ui_state & STARTPAN) && !(state & Mod1Mask) &&
       !(state & ShiftMask) && !(xctx->ui_state & (PLACE_SYMBOL | PLACE_TEXT)))
    {
      if(mx != xctx->mx_save || my != xctx->my_save) {
        if( !(xctx->ui_state & STARTSELECT)) {
          select_rect(START,1);
          xctx->onetime=1;
        }
        if(abs(mx-xctx->mx_save) > 8 ||
           abs(my-xctx->my_save) > 8 ) { /* set reasonable threshold before unsel */
          if(xctx->onetime) {
            unselect_all(); /* 20171026 avoid multiple calls of unselect_all() */
            xctx->onetime=0;
          }
          xctx->ui_state|=STARTSELECT; /* set it again cause unselect_all() clears it... */
        }
      }
    }
    if((state & Button1Mask)  && (state & Mod1Mask) && !(state & ShiftMask) &&
       !(xctx->ui_state & STARTPAN) && 
       !(xctx->ui_state & (PLACE_SYMBOL | PLACE_TEXT))) { /* unselect area */
      if( !(xctx->ui_state & STARTSELECT)) {
        select_rect(START,0);
      }
    }
    else if((state&Button1Mask) && (state & ShiftMask) &&
             !(xctx->ui_state & (PLACE_SYMBOL | PLACE_TEXT)) &&
             !(xctx->ui_state & STARTPAN) ) {
      if(mx != xctx->mx_save || my != xctx->my_save) {
        if( !(xctx->ui_state & STARTSELECT)) {
          select_rect(START,1);
        }
        if(abs(mx-xctx->mx_save) > 8 || 
           abs(my-xctx->my_save) > 8 ) {  /* set reasonable threshold before unsel */
          select_object(X_TO_XSCHEM(xctx->mx_save),
                        Y_TO_XSCHEM(xctx->my_save), 0, 0); /* remove near obj if dragging */
          rebuild_selected_array();
        }
      }
    }
    break;
  case KeyRelease:
    break;
  case KeyPress:
   if(key==' ') {
     if(xctx->ui_state & STARTWIRE) { /*  & instead of == 20190409 */
       new_wire(RUBBER|CLEAR, xctx->mousex_snap, xctx->mousey_snap);
       xctx->manhattan_lines++;
       xctx->manhattan_lines %=3;
       new_wire(RUBBER, xctx->mousex_snap, xctx->mousey_snap);

     } else if(xctx->ui_state==STARTLINE) {
       new_line(RUBBER|CLEAR);
       xctx->manhattan_lines++;
       xctx->manhattan_lines %=3;
       new_line(RUBBER);
     } else {
       if(xctx->semaphore<2) {
         rebuild_selected_array(); /* sets or clears xctx->ui_state SELECTION flag */
       }
       pan(START, mx, my);
       xctx->ui_state |= STARTPAN;
     }
     break;
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
    break;
   }
   if(key == 'b' && state==ControlMask)         /* toggle show text in symbol */
   {
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
    break;
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
    break;
   }
   if(key == 'j'  && state==0 )                 /* print list of highlight nets */
   {
     if(xctx->semaphore >= 2) break;
     print_hilight_net(1);
     break;
   }
   if(key == 'j'  && state==ControlMask)        /* create ipins from highlight nets */
   {
     if(xctx->semaphore >= 2) break;
     print_hilight_net(0);
     break;
   }
   if(key == 'j'  && state==Mod1Mask)   /* create labels without i prefix from hilight nets */
   {
     if(xctx->semaphore >= 2) break;
     print_hilight_net(4);
     break;
   }
   if(key == 'J'  && state==(Mod1Mask | ShiftMask) ) /* create labels with i prefix from hilight nets */
   {
     if(xctx->semaphore >= 2) break;
     print_hilight_net(2);
     break;
   }
   if(key == 'h'  && state==ControlMask )       /* go to http link */
   {
     launcher();
     break;
   }
   if(key == 'h'  && state==Mod1Mask)   /* create symbol pins from schematic pins 20171208 */
   {
     tcleval("schpins_to_sympins");
     break;
   }
   if(key == 'h' && state == 0) {
     /* horizontally constrained drag 20171023 */
     if ( constrained_move == 1 ) {
       tcleval("set constrained_move 0" );
       constrained_move = 0;
     } else {
       tcleval("set constrained_move 1" );
       constrained_move = 1;
     }
     if(xctx->ui_state & STARTWIRE) {
       if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
       if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
       new_wire(RUBBER, xctx->mousex_snap, xctx->mousey_snap);
     }
     if(xctx->ui_state & STARTLINE) {
       if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
       if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
       new_line(RUBBER);
     }
     break;
   }
   if(key=='H' && state==ShiftMask) {           /* attach labels to selected instances */
     attach_labels_to_inst(1);
     break;
   }
   if (key == 'H' && state == (ControlMask | ShiftMask)) { /* create schematic and symbol from selected components */
     make_schematic_symbol_from_sel();
     break;
   }
   if(key == 'v' && state==0) {
     /* vertically constrained drag 20171023 */
     if ( constrained_move == 2 ) {
       tcleval("set constrained_move 0" );
       constrained_move = 0;
     } else {
       tcleval("set constrained_move 2" );
       constrained_move = 2;
     }
     if(xctx->ui_state & STARTWIRE) {
       if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
       if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
       new_wire(RUBBER, xctx->mousex_snap, xctx->mousey_snap);
     }
     if(xctx->ui_state & STARTLINE) {
       if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
       if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
       new_line(RUBBER);
     }
     break;
   }
   if(key == 'j'  && state == (ControlMask | Mod1Mask) )  /* print list of highlight net with label expansion */
   {
     print_hilight_net(3);
     break;
   }
   if(key == 'J' && state==ShiftMask) {
    create_plot_cmd();
    break;
   }
   if(key == '$'  && ( state == ShiftMask) )            /* toggle pixmap  saving */
   {
    xctx->draw_pixmap =!xctx->draw_pixmap;
    if(xctx->draw_pixmap) tcleval("alert_ { enabling draw pixmap} {}");
    else tcleval("alert_ { disabling draw pixmap} {}");
    break;
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
    break;
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
    break;
   }
   if(key == '+'  && state&ControlMask)         /* change line width */
   {
    xctx->lw+=0.1;
    change_linewidth(xctx->lw);
    draw();
    break;
   }

   if(key == '-'  && state&ControlMask)         /* change line width */
   {
    xctx->lw-=0.1;if(xctx->lw<0.0) xctx->lw=0.0;
    change_linewidth(xctx->lw);
    draw();
    break;
   }
   if(key == 'X' && state == ShiftMask) /* highlight discrepanciens between selected instance pin and net names */
   {
     hilight_net_pin_mismatches();
     break;
   }
   if(key== 'W' && state == ShiftMask) {  /* create wire snapping to closest instance pin */
     double x, y;
     if(xctx->semaphore >= 2) break;
     if(!(xctx->ui_state & STARTWIRE)){
       find_closest_net_or_symbol_pin(xctx->mousex, xctx->mousey, &x, &y);
       xctx->mx_double_save = my_round(x / c_snap) * c_snap;
       xctx->my_double_save = my_round(y / c_snap) * c_snap;
       new_wire(PLACE, x, y);
     }
     else {
       find_closest_net_or_symbol_pin(xctx->mousex, xctx->mousey, &x, &y);
       new_wire(RUBBER, x, y);
       new_wire(PLACE|END, x, y);
       constrained_move=0;
       tcleval("set constrained_move 0" );
     }
     break;
   }
   if(key == 'w' && state==0)    /* place wire. */
   {
     int prev_state = xctx->ui_state;
     if(xctx->semaphore >= 2) break;
     start_wire(mx, my);
     if(prev_state == STARTWIRE) {
       tcleval("set constrained_move 0" );
       constrained_move=0;
     }
     break;
   }
   if(key == XK_Return && (state == 0 ) && xctx->ui_state & STARTPOLYGON) { /* close polygon */
    new_polygon(ADD|END);
    break;
   }
   if(key == XK_Escape) /* abort & redraw */
   {
    if(xctx->semaphore < 2) {
      abort_operation();
    }
    /* stuff that can be done reentrantly ... */
    tclsetvar("tclstop", "1"); /* stop simulation if any running */
    break;
   }
   if(key=='z' && state == 0)                   /* zoom box */
   {
    dbg(1, "callback(): zoom_rectangle call\n");
    zoom_rectangle(START);break;
   }
   if(key=='Z' && state == ShiftMask)                   /* zoom in */
   {
    view_zoom(0.0); break;
   }
   if(key=='p' && state == Mod1Mask)                           /* add symbol pin */
   {
    unselect_all();
    storeobject(-1, xctx->mousex_snap-2.5, xctx->mousey_snap-2.5, xctx->mousex_snap+2.5, xctx->mousey_snap+2.5,
                xRECT, PINLAYER, SELECTED, "name=XXX\ndir=inout");
    xctx->need_reb_sel_arr=1;
    rebuild_selected_array();
    move_objects(START,0,0,0);
    xctx->ui_state |= START_SYMPIN;
    break;
   }
   if(key=='w' && !xctx->ui_state && state==ControlMask)              /* start polygon, 20171115 */
   {
     if(xctx->semaphore >= 2) break;
     dbg(1, "callback(): start polygon\n");
     xctx->mx_double_save=xctx->mousex_snap;
     xctx->my_double_save=xctx->mousey_snap;
     xctx->last_command = 0;
     new_polygon(PLACE);
     break;
   }
   if(key=='P' && state == ShiftMask)                   /* pan, other way to. */
   {
    xctx->xorigin=-xctx->mousex_snap+xctx->areaw*xctx->zoom/2.0;
    xctx->yorigin=-xctx->mousey_snap+xctx->areah*xctx->zoom/2.0;
    draw();
    redraw_w_a_l_r_p_rubbers();
    break;
   }
   if(key=='5' && state == 0) { /* 20110112 display only probes */
    xctx->only_probes = !xctx->only_probes;
    tclsetboolvar("only_probes", xctx->only_probes);
    toggle_only_probes();
    break;
   }  /* /20110112 */
   if(key<='9' && key >='0' && state==ControlMask)              /* choose layer */
   {
    char n[30];
    xctx->rectcolor = key - '0'+4;
    my_snprintf(n, S(n), "%d", xctx->rectcolor);
    tclvareval("xschem set rectcolor ", n, NULL);

    if(!strcmp(winpath, ".drw")) {
      tclvareval("reconfigure_layers_button {}", NULL);
    } else {
      tclvareval("reconfigure_layers_button [winfo parent ", winpath, "]", NULL);
    }
    dbg(1, "callback(): new color: %d\n",xctx->color_index[xctx->rectcolor]);
    break;
   }
   if(key==XK_Delete && (xctx->ui_state & SELECTION) ) /* delete selection */
   {
     if(xctx->semaphore >= 2) break;
     delete(1/* to_push_undo */);break;
   }
   if(key==XK_Left && (state & ControlMask)) /* previous tab */
   {
     tcleval("prev_tab");
   }
   if(key==XK_Right && (state & ControlMask)) /* next tab */
   {
     tcleval("next_tab");
   }
   if(key==XK_Right && !(state & ControlMask))   /* left */
   {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      break;
    }
    xctx->xorigin+=-CADMOVESTEP*xctx->zoom;
    draw();
    redraw_w_a_l_r_p_rubbers();
    break;
   }
   if(key==XK_Left && !(state & ControlMask))   /* right */
   {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      break;
    }
    xctx->xorigin-=-CADMOVESTEP*xctx->zoom;
    draw();
    redraw_w_a_l_r_p_rubbers();
    break;
   }
   if(key==XK_Down)                     /* down */
   {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      break;
    }
    xctx->yorigin+=-CADMOVESTEP*xctx->zoom;
    draw();
    redraw_w_a_l_r_p_rubbers();
    break;
   }
   if(key==XK_Up)                       /* up */
   {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      break;
    }
    xctx->yorigin-=-CADMOVESTEP*xctx->zoom;
    draw();
    redraw_w_a_l_r_p_rubbers();
    break;
   }
   if(key=='q' && state == ControlMask) /* exit */
   {
     if(xctx->semaphore >= 2) break;
     if(!strcmp(xctx->current_win_path, ".drw")) {
       int remaining;
       /* tcleval("new_window destroy_all"); */ /* close child schematics */
       remaining = new_schematic("destroy_all", NULL, NULL);
       /* if(tclresult()[0] == '1') { */
       if(!remaining) {
         if(xctx->modified) {
           tcleval("tk_messageBox -type okcancel  -parent [xschem get topwindow] -message \"" 
                   "[get_cell [xschem get schname] 0]"
                   ": UNSAVED data: want to exit?\"");
         }
         if(!xctx->modified || !strcmp(tclresult(),"ok")) tcleval("exit");
       }
     } else {
       /* xschem new_schematic destroy asks user confirmation if schematic changed */
       new_schematic("destroy", xctx->current_win_path, NULL);
     }
     break;
   }
   if(key=='t' && state == 0)                        /* place text */
   {
     if(xctx->semaphore >= 2) break;
     xctx->last_command = 0;
     xctx->mx_double_save = xctx->mousex_snap;
     xctx->my_double_save = xctx->mousey_snap;
     if(place_text(0, xctx->mousex_snap, xctx->mousey_snap)) { /* 1 = draw text 24122002 */
       xctx->mousey_snap = xctx->my_double_save;
       xctx->mousex_snap = xctx->mx_double_save;
       move_objects(START,0,0,0);
       xctx->ui_state |= PLACE_TEXT;
     }
     break;
   }
   if(key=='r' && !xctx->ui_state && state==0)              /* start rect */
   {
    dbg(1, "callback(): start rect\n");
    xctx->mx_double_save=xctx->mousex_snap;
    xctx->my_double_save=xctx->mousey_snap;
    xctx->last_command = 0;
    new_rect(PLACE);
    break;
   }
   if(key=='V' && state == ShiftMask)                           /* toggle spice/vhdl netlist  */
   {
    xctx->netlist_type++; if(xctx->netlist_type==6) xctx->netlist_type=1;
    set_tcl_netlist_type();
    break;
   }
   if(key=='s' && (state == 0) )      /* simulate */
   {
     tcleval("[xschem get top_path].menubar.simulate invoke");
     break;
   }
   if(key=='s' && (state == ControlMask) )      /* save 20121201 */
   {
     if(xctx->semaphore >= 2) break;
     /* check if unnamed schematic, use saveas in this case */
     if(!strcmp(xctx->sch[xctx->currsch],"") || strstr(xctx->sch[xctx->currsch], "untitled")) {
       saveas(NULL, SCHEMATIC);
     } else {
       save(1);
     }
     break;
   }
   if(key=='s' && state == (ControlMask | Mod1Mask) )           /* save as symbol */
   {
     if(xctx->semaphore >= 2) break;
     saveas(NULL, SYMBOL);
     break;
   }
   if(key=='S' && state == (ShiftMask | ControlMask)) /* save as schematic */
   {
     if(xctx->semaphore >= 2) break;
     saveas(NULL, SCHEMATIC);
     break;
   }
   if(key=='e' && state == 0)           /* descend to schematic */
   {
    if(xctx->semaphore >= 2) break;
    descend_schematic(0);break;
   }
   if(key=='e' && state == Mod1Mask)            /* edit schematic in new window */
   {
    int save = xctx->semaphore;
    xctx->semaphore--; /* so semaphore for current context wll be saved correctly */
    schematic_in_new_window();
    xctx->semaphore = save;
    break;
   }
   if(key=='i' && state == Mod1Mask)            /* edit symbol in new window */
   {
    int save =  xctx->semaphore;
    xctx->semaphore--; /* so semaphore for current context wll be saved correctly */
    symbol_in_new_window();
    xctx->semaphore = save;
    break;
   }
   if( (key=='e' && state == ControlMask) || (key==XK_BackSpace))  /* back */
   {
    if(xctx->semaphore >= 2) break;
    go_back(1);break;
   }

   if(key=='a' && state == 0)   /* make symbol */
   {
    if(xctx->semaphore >= 2) break;
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      break;
    }
    tcleval("tk_messageBox -type okcancel -parent [xschem get topwindow] "
            "-message {do you want to make symbol view ?}");
    if(strcmp(tclresult(),"ok")==0)
    {
     save_schematic(xctx->sch[xctx->currsch]);
     make_symbol();
    }
    break;
   }
   if(key=='a' && state == ControlMask)         /* select all */
   {
    select_all();
    break;
   }
   if(key=='y' && state == 0)                           /* toggle stretching */
   {
    int en_s;
    en_s = tclgetboolvar("enable_stretch");
    en_s = !en_s;

    if(en_s) {
        tcleval("alert_ { enabling stretch mode } {}");
        tclsetvar("enable_stretch","1");
    }
    else {
        tcleval("alert_ { disabling stretch mode } {}");
        tclsetvar("enable_stretch","0");
    }
    break;
   }
   if(key=='x' && state == ControlMask) /* cut selection into clipboard */
   {
    if(xctx->semaphore >= 2) break;
    rebuild_selected_array();
    if(xctx->lastsel) {  /* 20071203 check if something selected */
      save_selection(2);
      delete(1/* to_push_undo */);
    }
    break;
   }
   if(key=='c' && state == ControlMask) /* copy selection into clipboard */
   {
     if(xctx->semaphore >= 2) break;
     rebuild_selected_array();
     if(xctx->lastsel) {  /* 20071203 check if something selected */
       save_selection(2);
     }
    break;
   }
   if(key=='C' && state == ShiftMask) /* place arc */
   {
     if(xctx->semaphore >= 2) break;
     xctx->mx_double_save=xctx->mousex_snap;
     xctx->my_double_save=xctx->mousey_snap;
     xctx->last_command = 0;
     new_arc(PLACE, 180.);
     break;
   }
   if(key=='C' && state == (ControlMask|ShiftMask)) /* place circle */
   {
     if(xctx->semaphore >= 2) break;
     xctx->mx_double_save=xctx->mousex_snap;
     xctx->my_double_save=xctx->mousey_snap;
     xctx->last_command = 0;
     new_arc(PLACE, 360.);
     break;
   }
   if(key=='O' && (state == (ControlMask|ShiftMask)) )   /* load most recent tile */
   {
     tclvareval("xschem load [lindex $recentfile 0]", NULL);
     break;
   }
   if(key=='O' && state == ShiftMask)   /* toggle light/dark colorscheme 20171113 */
   {
     int d_c;
     d_c = tclgetboolvar("dark_colorscheme");
     d_c = !d_c;
     tclsetboolvar("dark_colorscheme", d_c);
     tclsetdoublevar("dim_value", 0.0);
     tclsetdoublevar("dim_bg", 0.0);
     build_colors(0.0, 0.0);
     draw();
     break;
   }
   if(key=='v' && state == ControlMask)   /* paste from clipboard */
   {
    if(xctx->semaphore >= 2) break;
    merge_file(2,".sch");
    break;
   }
   if(key=='Q' && state == (ControlMask | ShiftMask) ) /* view attributes */
   {
    edit_property(2);break;
   }
   if(key=='q' && state==0) /* edit attributes */
   {
    if(xctx->semaphore >= 2) break;
    edit_property(0);
    break;
   }
   if(key=='q' && state==Mod1Mask)                      /* edit .sch file (DANGER!!) */
   {
    if(xctx->semaphore >= 2) break;
    rebuild_selected_array();
    if(xctx->lastsel==0 ) {
      my_snprintf(str, S(str), "edit_file {%s}", abs_sym_path(xctx->sch[xctx->currsch], ""));
      tcleval(str);
    }
    else if(xctx->sel_array[0].type==ELEMENT) {
      my_snprintf(str, S(str), "edit_file {%s}",
         abs_sym_path(xctx->inst[xctx->sel_array[0].n].name, ""));
      tcleval(str);

    }
    break;
   }
   if(key=='Q' && state == ShiftMask) /* edit attributes in editor */
   {
    if(xctx->semaphore >= 2) break;
    edit_property(1);break;
   }
   if(key=='i' && state==0)                     /* descend to  symbol */
   {
    if(xctx->semaphore >= 2) break;
    descend_symbol();break;
   }
   if(key==XK_Insert || (key == 'I' && state == ShiftMask) ) /* insert sym */
   {
    if(xctx->semaphore >= 2) break;
    start_place_symbol(mx, my);

    break;
   }
   if(key=='s' && state & Mod1Mask)                     /* reload */
   {
    if(xctx->semaphore >= 2) break;
     tcleval("tk_messageBox -type okcancel -parent [xschem get topwindow] "
             "-message {Are you sure you want to reload from disk?}");
     if(strcmp(tclresult(),"ok")==0) {
        char filename[PATH_MAX];
        unselect_all();
        remove_symbols();
        my_strncpy(filename, abs_sym_path(xctx->sch[xctx->currsch], ""), S(filename));
        load_schematic(1, filename, 1);
        draw();
     }
     break;
   }
   if(key=='o' && state == ControlMask)   /* load */
   {
    if(xctx->semaphore >= 2) break;
    ask_new_file();
    break;
   }
   if(key=='S' && state == ShiftMask)   /* change element order */
   {
    if(xctx->semaphore >= 2) break;
    change_elem_order();
    break;
   }
   if(key=='k' && state==Mod1Mask)        /* select whole net (all attached wires/labels/pins) */
   {
     select_hilight_net();
     break;
   }
   if(key=='k' && state==ControlMask)                           /* unhilight net */
   {
    if(xctx->semaphore >= 2) break;
    unhilight_net();
    break;
   }
   if(key=='K' && state==(ControlMask|ShiftMask))       /* hilight net drilling thru elements  */
                                                        /* with 'propag=' prop set on pins */
   {
    if(xctx->semaphore >= 2) break;
    xctx->enable_drill=1;
    hilight_net(0);
    redraw_hilights(0);
    /* draw_hilight_net(1); */
    break;
   }
   if(key=='k' && state==0)                             /* hilight net */
   {
    if(xctx->semaphore >= 2) break;
    xctx->enable_drill=0;
    hilight_net(0);
    redraw_hilights(0);
    /* draw_hilight_net(1); */
    break;
   }
   if(key=='K' && state == ShiftMask)                           /* delete hilighted nets */
   {
    xRect boundbox;
    int big =  xctx->wires> 2000 || xctx->instances > 2000 ;
    if(xctx->semaphore >= 2) break;
    xctx->enable_drill=0;
    if(!big) calc_drawing_bbox(&boundbox, 2);
    clear_all_hilights();
    /* undraw_hilight_net(1); */
    if(!big) {
      bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
      bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
      bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
    }
    draw();
    if(!big) bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
    break;
   }
   if(key=='g' && state==Mod1Mask) { /* highlight net and send to viewer */
     int tool = 0;
     int exists = 0;
     char *tool_name = NULL;
     char str[200];

     if(xctx->semaphore >= 2) break;
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
         tool = atol(tclgetvar("sim(spicewave,default)"));
         my_snprintf(str, S(str), "sim(spicewave,%d,name)", tool);
         my_strdup(1271, &tool_name, tclgetvar(str));
         dbg(1,"callback(): tool_name=%s\n", tool_name);
         if(strstr(tool_name, "Gaw")) tool=GAW;
         else if(strstr(tool_name, "Bespice")) tool=BESPICE;
         my_free(1272, &tool_name);
       }
     }
     if(tool) {
       hilight_net(tool);
       redraw_hilights(0);
     }
     Tcl_ResetResult(interp);
     break;
   }
   if(key=='g' && state==0)                         /* half snap factor */
   {
    set_snap(c_snap / 2.0);
    break;
   }
   if(key=='g' && state==ControlMask)              /* set snap factor 20161212 */
   {
    my_snprintf(str, S(str),
     "input_line {Enter snap value (default: %.16g current: %.16g)}  {xschem set cadsnap} {%g} 10",
     CADSNAP, c_snap, c_snap);
    tcleval(str);
    break;
   }
   if(key=='G' && state==ShiftMask)                                    /* double snap factor */
   {
    set_snap(c_snap * 2.0);
    break;
   }
   if(key=='*' && state==(Mod1Mask|ShiftMask) )         /* svg print , 20121108 */
   {
    if(xctx->semaphore >= 2) break;
    svg_draw();
    break;
   }
   if(key=='*' && state==ShiftMask )                    /* postscript print */
   {
    if(xctx->semaphore >= 2) break;
    ps_draw(7);
    break;
   }
   if(key=='*' && state==(ControlMask|ShiftMask) )      /* xpm print */
   {
    if(xctx->semaphore >= 2) break;
    print_image();
    break;
   }
   if(key=='u' && state==Mod1Mask)                      /* align to grid */
   {
    if(xctx->semaphore >= 2) break;
    xctx->push_undo();
    round_schematic_to_grid(c_snap);
    set_modify(1);
    if(tclgetboolvar("autotrim_wires")) trim_wires();
    xctx->prep_hash_inst=0;
    xctx->prep_hash_wires=0;
    xctx->prep_net_structs=0;
    xctx->prep_hi_structs=0;

    draw();
    break;
   }
   if(0 && (key=='u') && (state==ControlMask))                   /* testmode */
   {
    int mult;
    remove_symbol(2);
    link_symbols_to_instances(-1);
    expandlabel("/RST", &mult);
    expandlabel("/CCC[3:0]", &mult);
    expandlabel("CCC[AA:BB:DD]", &mult);
    expandlabel("CCC[9:1:2]", &mult);
    expandlabel("CCC[10:BB:DD]", &mult);
    expandlabel("CCC[10..BB..DD]", &mult);
    expandlabel("CCC[10..0..2]", &mult);
    expandlabel("123", &mult);
    expandlabel("123AA", &mult);
    break;
   }
   if(key=='u' && state==0)                             /* undo */
   {
    if(xctx->semaphore >= 2) break;
    xctx->pop_undo(0, 1);  /* 2nd parameter: set_modify_status */
    draw();
    break;
   }
   if(key=='U' && state==ShiftMask)                     /* redo */
   {
    if(xctx->semaphore >= 2) break;
    xctx->pop_undo(1, 1); /* 2nd parameter: set_modify_status */
    draw();
    break;
   }
   if(key=='&')                         /* check wire connectivity */
   {
    if(xctx->semaphore >= 2) break;
    xctx->push_undo();
    trim_wires();
    draw();
    break;
   }
   if(key=='l' && state == ControlMask) { /* create schematic from selected symbol 20171004 */
     if(xctx->semaphore >= 2) break;
     create_sch_from_sym();
     break;
   }
   if(key=='l' && state == 0) /* start line */
   {
     int prev_state = xctx->ui_state;
     start_line(mx, my);
     if(prev_state == STARTLINE) {
       tcleval("set constrained_move 0" );
       constrained_move=0;
     }
     break;
   }
   if(key=='l' && state == Mod1Mask) {                         /* add pin label*/
     place_net_label(1);
     break;
   }
   if(key >= '0' && key <= '4' && state == 0) {  /* toggle pin logic level */
     if(xctx->semaphore >= 2) break;
     if(key == '4') logic_set(-1, 1);
     else logic_set(key - '0', 1);
     break;
   }
   if(key=='L' && state == (Mod1Mask | ShiftMask)) {                         /* add pin label*/
    place_net_label(0);
    break;
   }
   if(key=='F' && state==ShiftMask)                     /* flip */
   {
    if(xctx->ui_state & STARTMOVE) move_objects(FLIP,0,0,0);
    else if(xctx->ui_state & STARTCOPY) copy_objects(FLIP);
    else {
      rebuild_selected_array();
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      move_objects(START,0,0,0);
      if(xctx->lastsel>1) move_objects(FLIP,0,0,0);
      else               move_objects(FLIP|ROTATELOCAL,0,0,0);
      move_objects(END,0,0,0);
    }
    break;
   }
   if(key=='\\' && state==0)          /* fullscreen */
   {
    
    dbg(1, "callback(): toggle fullscreen, winpath=%s\n", winpath);
    toggle_fullscreen(winpath);
    break;
   }
   if(key=='f' && state==Mod1Mask)              /* flip objects around their anchor points 20171208 */
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
    break;
   }
   if(key=='R' && state==ShiftMask)             /* rotate */
   {
    if(xctx->ui_state & STARTMOVE) move_objects(ROTATE,0,0,0);
    else if(xctx->ui_state & STARTCOPY) copy_objects(ROTATE);
    else {
      rebuild_selected_array();
      xctx->mx_double_save=xctx->mousex_snap;
      xctx->my_double_save=xctx->mousey_snap;
      move_objects(START,0,0,0);
      if(xctx->lastsel>1) move_objects(ROTATE,0,0,0);
      else               move_objects(ROTATE|ROTATELOCAL,0,0,0);
      move_objects(END,0,0,0);
    }

    break;
   }
   if(key=='r' && state==Mod1Mask)              /* rotate objects around their anchor points 20171208 */
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
    break;
   }
   if(key=='m' && state==0 && !(xctx->ui_state & (STARTMOVE | STARTCOPY))) /* move selection */
   {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      break;
    }
    xctx->mx_double_save=xctx->mousex_snap;
    xctx->my_double_save=xctx->mousey_snap;
    move_objects(START,0,0,0);
    break;
   }

   if(key=='c' && state==0 &&           /* duplicate selection */
     !(xctx->ui_state & (STARTMOVE | STARTCOPY)))
   {
    if(xctx->semaphore >= 2) break;
    xctx->mx_double_save=xctx->mousex_snap;
    xctx->my_double_save=xctx->mousey_snap;
    copy_objects(START);
    break;
   }
   if(key=='n' && state==Mod1Mask)              /* empty schematic in new window */
   {
     if(xctx->semaphore >= 2) break;
     tcleval("xschem new_window");
     break;
   }
   if(key=='N' && state==(ShiftMask|Mod1Mask) )    /* empty symbol in new window */
   {
     if(xctx->semaphore >= 2) break;
     tcleval("xschem new_symbol_window");
     break;
   }
   if(key=='n' && state==ControlMask)              /* new schematic */
   {
     if(xctx->semaphore >= 2) break;
     tcleval("xschem clear SCHEMATIC");
     break;
   }
   if(key=='N' && state==(ShiftMask|ControlMask) )    /* new symbol */
   {
     if(xctx->semaphore >= 2) break;
     tcleval("xschem clear SYMBOL");
     break;
   }
   if(key=='n' && state==0)              /* hierarchical netlist */
   {
    yyparse_error = 0;
    if(xctx->semaphore >= 2) break;
    unselect_all();
    if(set_netlist_dir(0, NULL)) {
      dbg(1, "callback(): -------------\n");
      if(xctx->netlist_type == CAD_SPICE_NETLIST)
        global_spice_netlist(1);
      else if(xctx->netlist_type == CAD_VHDL_NETLIST)
        global_vhdl_netlist(1);
      else if(xctx->netlist_type == CAD_VERILOG_NETLIST)
        global_verilog_netlist(1);
      else if(xctx->netlist_type == CAD_TEDAX_NETLIST)
        global_tedax_netlist(1);
      else
        if(has_x) tcleval("tk_messageBox -type ok -parent [xschem get topwindow] "
                          "-message {Please Set netlisting mode (Options menu)}");

      dbg(1, "callback(): -------------\n");
    }
    break;
   }
   if(key=='N' && state==ShiftMask)              /* current level only netlist */
   {
    yyparse_error = 0;
    if(xctx->semaphore >= 2) break;
    unselect_all();
    if( set_netlist_dir(0, NULL) ) {
      dbg(1, "callback(): -------------\n");
      if(xctx->netlist_type == CAD_SPICE_NETLIST)
        global_spice_netlist(0);
      else if(xctx->netlist_type == CAD_VHDL_NETLIST)
        global_vhdl_netlist(0);
      else if(xctx->netlist_type == CAD_VERILOG_NETLIST)
        global_verilog_netlist(0);
      else if(xctx->netlist_type == CAD_TEDAX_NETLIST)
        global_tedax_netlist(0);
      else
        if(has_x) tcleval("tk_messageBox -type ok -parent [xschem get topwindow] "
                          "-message {Please Set netlisting mode (Options menu)}");
      dbg(1, "callback(): -------------\n");
    }
    break;
   }
   if(key=='A' && state==ShiftMask)                             /* toggle show netlist */
   {
    int net_s;
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
    break;
   }
   if(key=='>') {
     if(xctx->semaphore >= 2) break;
     if(xctx->draw_single_layer< cadlayers-1) xctx->draw_single_layer++;
     draw();
     break;
   }
   if(key=='<') {
     if(xctx->semaphore >= 2) break;
     if(xctx->draw_single_layer>=0 ) xctx->draw_single_layer--;
     draw();
     break;
   }
   if(key==':')                         /* toggle flat netlist (only spice)  */
   {
    xctx->flat_netlist = !xctx->flat_netlist;
    if(xctx->flat_netlist) {
        tcleval("alert_ { enabling flat netlist} {}");
        tclsetvar("flat_netlist","1");
    }
    else {
        tcleval("alert_ { set hierarchical netlist } {}");
        tclsetvar("flat_netlist","0");
    }
    break;
   }
   if(key=='b' && state==0)                     /* merge schematic */
   {
    if(xctx->semaphore >= 2) break;
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      break;
    }
    merge_file(0, ""); /* 2nd parameter not used any more for merge 25122002 */
    break;
   }
   if(key=='b' && state==Mod1Mask)                     /* hide/show instance details */
   {
    if(xctx->semaphore >= 2) break;
    xctx->hide_symbols++;
    if(xctx->hide_symbols >= 3) xctx->hide_symbols = 0;
    tclsetintvar("hide_symbols", xctx->hide_symbols);
    draw();
    break;
   }
   if(key=='D' && state==ShiftMask)                     /* delete files */
   {
    if(xctx->semaphore >= 2) break;
    delete_files();
    break;
   }
   if(key=='x' && state == 0 )                  /* new cad session */
   {
    new_xschem_process(NULL ,0);
    break;
   }
   if((key=='#') && !(state & ControlMask) )
   {
    check_unique_names(0);
    break;
   }
   if((key=='#') && (state & ControlMask) )
   {
    check_unique_names(1);
    break;
   }
   if( 0 && (key==';') && (state & ControlMask) )    /* testmode:  for performance testing */
   {
    break;
   }
   if(0 && key=='~' && (state & ControlMask)) {  /* testmode */
    break;
   }
   if(0 && key=='|' && !(state & ControlMask)) {            /* testmode */
    break;
   }
   if(0 && key=='|' && (state & ControlMask))      /* testmode */
   {
    break;
   }

   if(key=='f' && state == ControlMask)         /* search */
   {
    if(xctx->semaphore >= 2) break;
    tcleval("property_search");
    break;
   }
   if(key=='F' && state == (ShiftMask | ControlMask) )   /* full zoom selection */
   {
    if(xctx->ui_state == SELECTION) {
      zoom_full(1, 1, 3, 0.97);
    }
    break;
   }
   if(key=='f' && state == 0 )                  /* full zoom */
   {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      break;
    }
    zoom_full(1, 0, 1, 0.97);
    break;
   }
   if((key=='z' && state==ControlMask))                         /* zoom out */
   {
     view_unzoom(0.0);
     break;
   }
   if(key=='!')
   {
     if(xctx->semaphore >= 2) break;
     break_wires_at_pins();
     break;
   }
   break;
  case ButtonPress:                     /* end operation */
   dbg(1, "callback(): ButtonPress  ui_state=%ld state=%d\n",xctx->ui_state,state);
   if(waves_selected(event, key, state, button)) {
     waves_callback(event, mx, my, key, button, aux, state);
     break;
   }
   if(xctx->ui_state & STARTPAN) {
     xctx->ui_state &=~STARTPAN;
     xctx->mx_save = mx; xctx->my_save = my;
     xctx->mx_double_save=xctx->mousex_snap;
     xctx->my_double_save=xctx->mousey_snap;
     break;
   }

  /*
   * if(button == Button3 && tclgetvar("graph_selected")[0] && xctx->semaphore >=2 )
   * {
   *    sel = select_object(xctx->mousex, xctx->mousey, SELECTED, 0);
   *    if(sel) send_net_to_graph(1);
   *   
   * }
   * else
   */
   if(button == Button3 &&  state == ControlMask && xctx->semaphore <2)
   {
     sel = select_object(xctx->mousex, xctx->mousey, SELECTED, 0);
     if(sel) select_connected_wires(1);
   }
   else if(button == Button3 &&  state == ShiftMask && xctx->semaphore <2)
   {
     sel = select_object(xctx->mousex, xctx->mousey, SELECTED, 0);
     if(sel) select_connected_wires(0);
   }
   else if(button == Button3 &&  state == 0 && xctx->semaphore <2) {
     int ret;
     const char *status;
     int prev_state;
     xctx->semaphore++;
     status = tcleval("context_menu");
     xctx->semaphore--;
     if(!status) break;
     ret = atoi(status);
     switch(ret) {
       case 1:
         start_place_symbol(mx, my);
         break;
       case 2:
         prev_state = xctx->ui_state;
         start_wire(mx, my);
         if(prev_state == STARTWIRE) {
           tcleval("set constrained_move 0" );
           constrained_move=0;
         }
         break;
       case 3:
         prev_state = xctx->ui_state;
         start_line(mx, my);
         if(prev_state == STARTLINE) {
           tcleval("set constrained_move 0" );
           constrained_move=0;
         }
         break;
       case 4:
         xctx->mx_double_save=xctx->mousex_snap;
         xctx->my_double_save=xctx->mousey_snap;
         xctx->last_command = 0;
         new_rect(PLACE);
         break;
       case 5:
         xctx->mx_double_save=xctx->mousex_snap;
         xctx->my_double_save=xctx->mousey_snap;
         xctx->last_command = 0;
         new_polygon(PLACE);
         break;
       case 6: /* place text */
         xctx->last_command = 0;
         xctx->mx_double_save=xctx->mousex_snap;
         xctx->my_double_save=xctx->mousey_snap;
         if(place_text(0, xctx->mousex_snap, xctx->mousey_snap)) { /* 1 = draw text */
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
         tclvareval("xschem load [lindex $recentfile 0]", NULL);
         break;
       case 10: /* edit attributes */
         edit_property(0);
         break;
       case 11: /* edit attributes in editor */
         edit_property(1);
         break;
       case 12:
         descend_schematic(0);
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
         new_arc(PLACE, 180.);
         break;
       case 20: /* place circle */
         xctx->mx_double_save=xctx->mousex_snap;
         xctx->my_double_save=xctx->mousey_snap;
         xctx->last_command = 0;
         new_arc(PLACE, 360.);
         break;
       case 21: /* abort & redraw */
         abort_operation();
         break;
       default:
         break;
     }
   }
   else if(button==Button5 && state == 0 ) {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      break;
    }
     view_unzoom(CADZOOMSTEP);
   }
   else if(button==Button4 && state == 0 ) {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      break;
    }
     view_zoom(CADZOOMSTEP);
   }
   else if(button==Button4 && (state & ShiftMask) && !(state & Button2Mask)) {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      break;
    }
    xctx->xorigin+=-CADMOVESTEP*xctx->zoom/2.;
    draw();
    redraw_w_a_l_r_p_rubbers();
   }
   else if(button==Button5 && (state & ShiftMask) && !(state & Button2Mask)) {
    if(waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      break;
    }
    xctx->xorigin-=-CADMOVESTEP*xctx->zoom/2.;
    draw();
    redraw_w_a_l_r_p_rubbers();
   }
   else if(button==Button4 && (state & ControlMask) && !(state & Button2Mask)) {
    xctx->yorigin+=-CADMOVESTEP*xctx->zoom/2.;
    draw();
    redraw_w_a_l_r_p_rubbers();
   }
   else if(button==Button5 && (state & ControlMask) && !(state & Button2Mask)) {
    xctx->yorigin-=-CADMOVESTEP*xctx->zoom/2.;
    draw();
   }
   else if(button==Button1 && (state & Mod1Mask) ) {
     xctx->last_command = 0;
     xctx->mx_save = mx; xctx->my_save = my;
     xctx->mx_double_save=xctx->mousex_snap;
     xctx->my_double_save=xctx->mousey_snap;

     /* useless code ? 20200905 */
     /* if(xctx->semaphore<2) {
       rebuild_selected_array();
     } */

     select_object(xctx->mousex, xctx->mousey, 0, 0);
     rebuild_selected_array(); /* sets or clears xctx->ui_state SELECTION flag */
   }
   else if(button==Button2 && (state == 0)) {
     pan(START, mx, my);
     xctx->ui_state |= STARTPAN;
   }
   else if(xctx->semaphore >= 2) { /* button1 click to select another instance while edit prop dialog open */
     if(button==Button1 && state==0 && tclgetvar("edit_symbol_prop_new_sel")[0]) {
       tcleval("set edit_symbol_prop_new_sel 1; .dialog.f1.b1 invoke"); /* invoke 'OK' of edit prop dialog */
     } else if(button==Button1 && (state & ShiftMask) && tclgetvar("edit_symbol_prop_new_sel")[0]) {
       select_object(xctx->mousex, xctx->mousey, SELECTED, 0);
       rebuild_selected_array();
     }
   }
   else if(button==Button1)
   {
     if(tclgetboolvar("persistent_command") && xctx->last_command) {
       if(xctx->last_command == STARTLINE)  start_line(mx, my);
       if(xctx->last_command == STARTWIRE)  start_wire(mx, my);
       break;
     }
     if(xctx->ui_state & MENUSTARTWIRE) {
       xctx->mx_double_save=xctx->mousex_snap;
       xctx->my_double_save=xctx->mousey_snap;
       new_wire(PLACE, xctx->mousex_snap, xctx->mousey_snap);
       xctx->ui_state &=~MENUSTARTWIRE;
       break;
     }
     if(xctx->ui_state & MENUSTARTSNAPWIRE) {
       double x, y;

       find_closest_net_or_symbol_pin(xctx->mousex, xctx->mousey, &x, &y);
       xctx->mx_double_save = my_round(x / c_snap) * c_snap;
       xctx->my_double_save = my_round(y / c_snap) * c_snap;
       new_wire(PLACE, x, y);
       xctx->ui_state &=~MENUSTARTSNAPWIRE;
       break;
     }
     if(xctx->ui_state & MENUSTARTLINE) {
       xctx->mx_double_save=xctx->mousex_snap;
       xctx->my_double_save=xctx->mousey_snap;
       new_line(PLACE);
       xctx->ui_state &=~MENUSTARTLINE;
       break;
     }
     if(xctx->ui_state & MENUSTARTRECT) {
       xctx->mx_double_save=xctx->mousex_snap;
       xctx->my_double_save=xctx->mousey_snap;
       new_rect(PLACE);
       xctx->ui_state &=~MENUSTARTRECT;
       break;
     }
     if(xctx->ui_state & MENUSTARTPOLYGON) {
       xctx->mx_double_save=xctx->mousex_snap;
       xctx->my_double_save=xctx->mousey_snap;
       new_polygon(PLACE);
       xctx->ui_state &=~MENUSTARTPOLYGON;
       break;
     }
     if(xctx->ui_state & MENUSTARTARC) {
       xctx->mx_double_save=xctx->mousex_snap;
       xctx->my_double_save=xctx->mousey_snap;
       new_arc(PLACE, 180.);
       xctx->ui_state &=~MENUSTARTARC;
       break;
     }
     if(xctx->ui_state & MENUSTARTCIRCLE) {
       xctx->mx_double_save=xctx->mousex_snap;
       xctx->my_double_save=xctx->mousey_snap;
       new_arc(PLACE, 360.);
       xctx->ui_state &=~MENUSTARTCIRCLE;
       break;
     }
     if(xctx->ui_state & MENUSTARTZOOM) {
       zoom_rectangle(START);
       xctx->ui_state &=~MENUSTARTZOOM;
       break;
     }
     if(xctx->ui_state & STARTZOOM) {
       zoom_rectangle(END);
       break;
     }
     if(xctx->ui_state & STARTWIRE) {
       if(tclgetboolvar("persistent_command")) {
         if(constrained_move != 2) {
           xctx->mx_double_save=xctx->mousex_snap;
         }
         if(constrained_move != 1) {
           xctx->my_double_save=xctx->mousey_snap;
         }
         if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
         if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
         new_wire(PLACE, xctx->mousex_snap, xctx->mousey_snap);

       } else {
         new_wire(PLACE|END, xctx->mousex_snap, xctx->mousey_snap);
       }
       constrained_move=0;
       tcleval("set constrained_move 0" );
       break;
     }
     if(xctx->ui_state & STARTARC) {
       new_arc(SET, 0);
       break;
     }
     if(xctx->ui_state & STARTLINE) {
       if(tclgetboolvar("persistent_command")) {
         if(constrained_move != 2) {
           xctx->mx_double_save=xctx->mousex_snap;
         }
         if(constrained_move == 1) {
           xctx->my_double_save=xctx->mousey_snap;
         }
         if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
         if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
         new_line(PLACE);
       } else {
         new_line(PLACE|END);
       }
       constrained_move=0;
       tcleval("set constrained_move 0" );
       break;
     }
     if(xctx->ui_state & STARTRECT) {
       new_rect(PLACE|END);
       break;
     }
     if(xctx->ui_state & STARTPOLYGON) {
       if(constrained_move == 1) xctx->mousey_snap = xctx->my_double_save;
       if(constrained_move == 2) xctx->mousex_snap = xctx->mx_double_save;
       new_polygon(ADD);
       xctx->mx_double_save=xctx->mousex_snap;
       xctx->my_double_save=xctx->mousey_snap;
       constrained_move=0;
       tcleval("set constrained_move 0" );
       break;
     }
     if(xctx->ui_state & STARTMOVE) {
       move_objects(END,0,0,0);
       xctx->ui_state &=~START_SYMPIN;
       constrained_move=0;
       tcleval("set constrained_move 0" );
       break;
     }
     if(xctx->ui_state & STARTCOPY) {
       copy_objects(END);
       constrained_move=0;
       tcleval("set constrained_move 0" );
       break;
     }
     if( !(xctx->ui_state & STARTSELECT) && !(xctx->ui_state & STARTWIRE) && !(xctx->ui_state & STARTLINE)  ) {
       int prev_last_sel = xctx->lastsel;
       xctx->mx_save = mx; xctx->my_save = my;
       xctx->mx_double_save=xctx->mousex_snap;
       xctx->my_double_save=xctx->mousey_snap;
       if( !(state & ShiftMask) && !(state & Mod1Mask) ) {
         unselect_all();
#ifndef __unix__
         MyXCopyArea(display, xctx->save_pixmap, xctx->window, xctx->gctiled, xctx->xrect[0].x, xctx->xrect[0].y,
           xctx->xrect[0].width, xctx->xrect[0].height, xctx->xrect[0].x, xctx->xrect[0].y);
#endif
       }
       sel = select_object(xctx->mousex, xctx->mousey, SELECTED, 0);
       rebuild_selected_array();
#ifndef __unix__
       draw_selection(xctx->gc[SELLAYER], 0);
#endif
       if(sel && state == ControlMask) {
         launcher();
       }
       if( !(state & ShiftMask) )  {
         if(tclgetboolvar("auto_hilight") && xctx->hilight_nets && sel == 0 ) {
           if(!prev_last_sel) {
             redraw_hilights(1); /* 1: clear all hilights, then draw */
           }
         }
       }
       if(tclgetboolvar("auto_hilight")) {
         hilight_net(0);
         if(xctx->lastsel) {
           redraw_hilights(0);
           /* draw_hilight_net(1); */
         }
       }
       break;
     }
   } /* button==Button1 */
   break;
  case ButtonRelease:
   if(waves_selected(event, key, state, button)) {
     waves_callback(event, mx, my, key, button, aux, state);
     break;
   }
   if(xctx->ui_state & STARTPAN) {
     xctx->ui_state &=~STARTPAN;
     xctx->mx_save = mx; xctx->my_save = my;
     xctx->mx_double_save=xctx->mousex_snap;
     xctx->my_double_save=xctx->mousey_snap;
     redraw_w_a_l_r_p_rubbers();
     break;
   }
   dbg(1, "callback(): ButtonRelease  ui_state=%ld state=%d\n",xctx->ui_state,state);
   if(xctx->semaphore >= 2) break;
   if(xctx->ui_state & STARTSELECT) {
     if(state & ControlMask) {
       tclsetvar("enable_stretch", "1");
       select_rect(END,-1);
       tclsetvar("enable_stretch", "0");
       break;
     } else {
       /* 20150927 filter out button4 and button5 events */
       if(!(state&(Button4Mask|Button5Mask) ) ) select_rect(END,-1);
     }
     rebuild_selected_array();
     my_snprintf(str, S(str), "mouse = %.16g %.16g - selected: %d path: %s",
       xctx->mousex_snap, xctx->mousey_snap, xctx->lastsel, xctx->sch_path[xctx->currsch] );
     statusmsg(str,1);

   }
   break;
  case -3:  /* double click  : edit prop */
    if( waves_selected(event, key, state, button)) {
      waves_callback(event, mx, my, key, button, aux, state);
      break;
    }
   if(xctx->semaphore >= 2) break;
   dbg(1, "callback(): DoubleClick  ui_state=%ld state=%d\n",xctx->ui_state,state);
   if(button==Button1) {
     if(xctx->ui_state == STARTWIRE) {
       xctx->ui_state &= ~STARTWIRE;
     }
     if(xctx->ui_state == STARTLINE) {
       xctx->ui_state &= ~STARTLINE;
     }
     if( (xctx->ui_state & STARTPOLYGON) && (state ==0 ) ) {
       new_polygon(SET);
     }
   }
#ifndef __unix__
  case MOUSE_WHEEL_UP:  /* windows do not use button4 and button5 like X */
  {
    xctx->xorigin += -CADMOVESTEP * xctx->zoom / 2.;
    draw();
  }
#endif
   break;

  default:
   dbg(1, "callback(): Event:%d\n",event);
   break;
 } /* switch(event) */

 xctx->semaphore--;
 if(redraw_only) {
   xctx->semaphore--; /* decrement articially incremented semaphore (see above) */
   dbg(1, "callback(): semaphore >=2 restoring window context: %s <-- %s\n", old_winpath, winpath);
   if(old_winpath[0]) new_schematic("switch_win", old_winpath, "");
 }
 else
 if(strcmp(old_winpath, winpath)) {
   if(old_winpath[0]) dbg(1, "callback(): reset old_winpath: %s <- %s\n", old_winpath, winpath);
   my_strncpy(old_winpath, winpath, S(old_winpath));
 }
 return 0;
}

