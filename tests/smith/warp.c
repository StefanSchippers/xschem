/*
 *  File: warp.c
 *
 *  This file is part of XSCHEM,
 *  a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 *  simulation.
 *  Copyright (C) 1998-2026 Stefan Frederik Schippers
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 */

/* find the xschem toplevel (by WM_NAME), pick its largest child window
 * (the drw canvas) and warp the pointer to its center */
#include <X11/Xlib.h>
#include <stdlib.h>
#include <string.h>

static long area(Display *d, Window w)
{
  Window rootret;
  int x, y;
  unsigned int width, height, bw, depth;
  long a = -1;
  if(XGetGeometry(d, w, &rootret, &x, &y, &width, &height, &bw, &depth)) {
    a = (long)width * (long)height;
  }
  return a;
}

static void find_biggest_child(Display *d, Window w, Window *best, long *best_area)
{
  Window root_ret, parent;
  Window *kids = NULL;
  int nkids = 0, i;

  if(!XQueryTree(d, w, &root_ret, &parent, &kids, &nkids)) return;
  for(i = 0; i < nkids; i++) {
    long a = area(d, kids[i]);
    if(a > *best_area) {
      *best_area = a;
      *best = kids[i];
    }
  }
  XFree(kids);
}

static void walk(Display *d, Window w, int depth, Window *toplevel, long *tarea)
{
  char *name = NULL;
  Window root_ret, parent;
  Window *kids = NULL;
  int nkids = 0, i;

  XFetchName(d, w, &name);
  if(name && strstr(name, "xschem")) {
    long a = area(d, w);
    if(a > *tarea) {
      *tarea = a;
      *toplevel = w;
    }
    if(name) XFree(name);
    return;
  }
  if(name) XFree(name);
  if(depth >= 5) return;
  if(!XQueryTree(d, w, &root_ret, &parent, &kids, &nkids)) return;
  for(i = 0; i < nkids; i++) {
    walk(d, kids[i], depth + 1, toplevel, tarea);
  }
  XFree(kids);
}

int main(void)
{
  Display *d = XOpenDisplay(NULL);
  Window root, toplevel = None;
  Window canvas = None;
  long tarea = 0, carea = -1;
  unsigned int bw, depth, width, height;
  int x, y, ax, ay;
  Window child;

  if(!d) return 1;
  root = DefaultRootWindow(d);
  walk(d, root, 0, &toplevel, &tarea);
  if(toplevel == None) {
    /* fallback: use root's largest child */
    find_biggest_child(d, root, &toplevel, &tarea);
  }
  if(toplevel == None || tarea < 10000) {
    /* no reasonable window found, use screen center */
    XWarpPointer(d, None, root, 0, 0, 0, 0,
                 DisplayWidth(d, DefaultScreen(d)) / 2,
                 DisplayHeight(d, DefaultScreen(d)) / 2);
    XFlush(d);
    XCloseDisplay(d);
    return 2;
  }
  find_biggest_child(d, toplevel, &canvas, &carea);
  if(canvas == None) canvas = toplevel;
  if(XGetGeometry(d, canvas, &child, &x, &y, &width, &height, &bw, &depth)) {
    XTranslateCoordinates(d, canvas, root, 0, 0, &ax, &ay, &child);
    XWarpPointer(d, None, root, 0, 0, 0, 0, ax + (int)width / 2, ay + (int)height / 2);
    XFlush(d);
  }
  XCloseDisplay(d);
  return 0;
}
