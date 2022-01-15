#!/usr/bin/awk -f
#
#  File: viewdraw_import.awk
#  
#  This file is part of XSCHEM,
#  a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
#  simulation.
#  Copyright (C) 1998-2021 Stefan Frederik Schippers
# 
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#
# Stefan Schippers, 20220113
# Translate viewdraw schematic or symbol files to xschem
# Usage:
#   /path/to/viewdraw_import.awk <xschem_ref_lib> /path/to/viewdraw_schematic.1 > /path/to/xschem_schematic.sch
#   /path/to/viewdraw_import.awk <xschem_ref_lib> /path/to/viewdraw_symbol.1 > /path/to/xschem_symbol.sym
#
# <xschem_ref_lib> if given is the name of the xschem reference lib instances will use
# to reference symbols. If not given no reference lib will be used and xschem will
# expect to find symbols in current directory.
# NOTES: everything is translated to lower case since in test examples mixed case is used

BEGIN{
 debug = 1
 # define xschem lib instances will use to reference symbols
 if(ARGC > 2) {
   xschem_lib = ARGV[2]
   ARGC = 2
 } else {
   xschem_lib=""
 }
 # initialize xschem objects
 xs_version = "v {xschem version=3.0.0 file_version=1.2 }\n"
 xs_vhdl =  "G {}\n"
 xs_symbol = "K {}\n"
 xs_verilog = "V {}\n"
 xs_spice = "S {}\n"
 xs_tedax = "E {}\n"
 xs_texts = ""
 xs_lines = ""
 xs_circles = ""
 xs_boxes = ""
 xs_pins = ""
 xs_attrs = ""
 xs_pin_attr = ""
 xs_nets = ""
 xs_insts = ""
}

# read viewdraw file, remove DOS cr characters
{
  gsub(/\r/, "") 
  line[lines++] = $0
}

# parse viewdraw file
END{
  for(lineno = 0; lineno < lines; lineno++) {
    $0 = line[lineno]
    if($1 ~/^T/){
      get_text()
    }
    if($1 ~/^l/) {
      get_line()
    }
    if($1 ~/^b/){
      get_box()
    }
    if($1 ~/^c/){
      get_circle()
    }
    if($0 ~/^U/){
      get_attr()
    }
    if($1 ~/^P/) {
      get_pin()
    }
    if($1 ~/^N/) {
      get_net()
    }
    if($1 ~/^I/) {
      get_inst()
    }
  }
  # print xschem schematic/symbol
  if(xs_version) printf "%s", xs_version
  if(xs_vhdl) printf "%s", xs_vhdl
  if(xs_symbol) printf "%s", xs_symbol
  if(xs_verilog) printf "%s", xs_verilog
  if(xs_spice) printf "%s", xs_spice
  if(xs_tedax) printf "%s", xs_tedax
  if(xs_lines) printf "%s", xs_lines
  if(xs_texts) printf "%s", xs_texts
  if(xs_circles) printf "%s", xs_circles
  if(xs_boxes) printf "%s", xs_boxes
  if(xs_pins) printf "%s", xs_pins
  if(xs_attrs) printf "%s", xs_attrs
  if(xs_pin_attrs) printf "%s", xs_pin_attrs
  if(xs_nets) printf "%s", xs_nets
  if(xs_insts) printf "%s", xs_insts
}


##############################
## functions
##############################

# 1   2       3             4     5      6      7     8     9
#    id      ref            ?     x      y     rot    ?    end
# I  112  test:DTI_GEN_INV  1    1580   1370    3     1     '
function get_inst(       name, attr, x, y, rot)
{
  name = $3
  x = xscale($5)
  y = yscale($6)
  rot = $7
  sub(/.*:/,"", name)
  if(xschem_lib) name = xschem_lib "/" tolower(name) ".sym"
  else           name = tolower(name) ".sym"
  xs_insts = xs_insts "C {" name "} " x " " y " " inst_rot_flip(rot) " {" attr "}\n"
}

# 1  2
# N  54
function get_net(      id, lab, p1, p2, x1, y1, x2, y2, joints, jointx, jointy)
{
  id = $2
  while(1) {
    # 1  2   3  4
    # J 660 700 2
    if( next_line() ~ /^J/) {
      $0 = line[++lineno]
      ++joints
      jointx[joints] = xscale($2)
      jointy[joints] = yscale($3)
    # 1  2  3
    # S  6  5
    } else if(next_line() ~/^S/) {
      $0 = line[++lineno]
      p1 = $2
      p2 = $3
      x1 = jointx[p1]
      y1 = jointy[p1]
      x2 = jointx[p2]
      y2 = jointy[p2]
      xs_nets = xs_nets "N " ordered(x1, y1, x2, y2) " {}\n"
    # 1     2      3     4     5    6       7     8     9     10
    #       x      y    size  rot anchor  scope visib invert  txt
    # L    420    620    20    0    3       0     1     0     A
    } else if(next_line() ~/^L/) {
      $0 = line[++lineno]
      lab = $10
    } else {
      break
    }
  }
}


# 1  2  3   4     5    6      7
# T  x  y  size  rot anchor text
function get_text(     x, y, size, rot, anchor, text)
{
  x = xscale($2)
  y = yscale($3)
  size = lenscale($4)
  rot=$5
  anchor=$6
  text=get_fields_after(6)
  xs_texts = xs_texts  "T {" text "} " x " " y " " text_align(size, rot, anchor) "\n"
}

# poly lines are supported, 0-length segments discarded
# 1     2        3      4     5    6    7     8
# l line_color xstart ystart xend yend xmore ymore ...
function get_line(x1, y1, x2, y2, color, i)
{
  color=color_map($2)
  for(i = 3; i < NF - 2; i += 2) {
    x1=xscale($i)
    y1=yscale($(i + 1))
    x2=xscale($(i + 2))
    y2=yscale($(i + 3))
    if(x1 == x2 && y1 == y2) continue
    xs_lines = xs_lines "L " color " " ordered(x1, y1, x2, y2) " {}" "\n"
  }
}
# 1    2       3      4
# c xcenter ycenter radius
function get_circle(     x, y, r, color)
{
  x = xscale($2)
  y = yscale($3)
  r = lenscale($4)
  # 1     2          3       4
  # Q line_color line_style fill
  if(next_line() ~/^Q/) {
    $0 = line[++lineno]
  }
  color = color_map($2)
  xs_circles = xs_circles  "A " color " " x " " y " " r " " 0 " " 360 " {}" "\n"
}


# 1 2   3   4   5
# b x1  y1  x2  y2
function get_box(     x1, y1, x2, y2, color, attr)
{
  x1 = xscale($2)
  y1 = yscale($3)
  x2 = xscale($4)
  y2 = yscale($5)
  # 1     2          3       4
  # Q line_color line_style fill
  if(next_line() ~/^Q/) {
    $0 = line[++lineno]
  }
  color = color_map($2)
  if(color == 5) {
    xs_boxes = xs_boxes "B 5 " rectordered(x1, y1, x2, y2) " {}\n"
  } else {
    xs_boxes = xs_boxes "P " color " 5 " x1 " " y1 " " x2 " " y1 " " x2 " " y2 " " x1 " " y2 " " x1 " " y1 " {}\n"
  }
}


# 1   2     3       4          5        6       7       8
# U  xpos  ypos  text_size  rotation  anchor  scope  attribute
function get_attr(     show_all, x, y, size, rot, anchor, scope, attr)
{
  show_all = 0
  attr = get_fields_after(7)
  x = xscale($2)
  y = yscale($3)
  size = lenscale($4)
  rot = $5
  anchor = $6
  scope = $7
  if(scope == 1 || show_all) {
    xs_attrs = xs_attrs "T {" attr "} " x " " y " " text_align(size, rot, anchor) "\n"
  }
}

#1      2     3      4      5     6     7    8      9
#      ID   Xend   Yend    Xbeg  Xbeg  rot  side inv_bubble
#P     16    60     100     60    65    0    0      0
# side: 0:top, 1: bottom, 2:left, 3:right
# rot: 0:0, 1:90,2:180, 3:270
function get_pin(     l, b, t, x, y, show_all, rot, anchor, size, attr, visible, pin_name, pin_dir, pin_order)
{
  show_all = 0
  size = 0.1
  l =  "L 1 " ordered(xscale($5), yscale($6), xscale($3), yscale($4)) " {}"
  b =  "B 5 " rectordered(xscale($3 - size), yscale($4 - size), xscale($3 + size), yscale($4 + size))
  while(1) {
    $0 = line[++lineno]
    # 1   2     3        4         5         6          7          8
    # A  xpos  ypos  text_size  rotation   anchor   visibility  attribute
    # A   60    65      10         1         1          0          #=1
    if($0 ~/^A/){ # attributes
      x = xscale($2)
      y = yscale($3)
      size = lenscale($4)
      rot = $5
      anchor = $6
      visible = $7
      attr = get_fields_after(7)
      if(visible || show_all) {
        xs_pin_attrs = xs_pin_attrs "T {" attr "} " x " " y " " text_align(size, rot, anchor, 13) "\n"
      }
      if(attr ~/#=/) { # pin order
        pin_order = attr
        sub(/.*=/, "", pin_order)
      # 1   2     3        4         5         6          7          8
      # A  xpos  ypos  text_size  rotation   anchor   visibility  attribute
      # A   60    65     10          1          1         0       PINTYPE=B
      } else if(attr ~ /PINTYPE=/) { # pin direction
        pin_dir = get_fields_after(7)
        sub(/.*=/, "", pin_dir)
        if(pin_dir == "BI") pin_dir = "inout"
        else if(pin_dir == "IN") pin_dir = "in"
        else if(pin_dir == "OUT") pin_dir = "out"
        else if(pin_dir == "ANALOG") pin_dir = "inout"
        else pin_dir = "inout"
      }
    # 1   2    3     4         5        6       7       8          9         10
    # L xpos ypos text_size rotation anchor  scope visibility logic_sense text_label
    } else if($0 ~ /^L/) { # Label
      x = xscale($2)
      y = yscale($3)
      visible = $8
      rot = $5
      anchor=$6
      size = lenscale($4)
      pin_name = $10
      b = b " {name=" pin_name " dir=" pin_dir "}"
      t = "T {" pin_name "} " x " " y " " text_align(size, rot, anchor)
      break # assumption is "P " line is followed by "A ", "A ", and "L " lines.
    }
  }
  xs_pins = xs_pins l "\n" b "\n"
  if(visible) xs_pins = xs_pins t "\n"
}

# the various align/rotate combinations are obtained by reverse engineering
# more combinations need to be added as if(rot...&& anchor...)
function text_align(size, rot, anchor, color,      flip, halign, valign, tattrs)
{
  halign = ""
  valign = ""

  if(anchor == 2 || anchor == 5 || anchor == 8) valign = "vcenter=true"
  if(anchor == 4 || anchor == 5 || anchor == 6) valign = "hcenter=true"
  if(color) {
    color = "layer=" color
  } else {
    color = ""
  }
  size = size / 60
  tattrs = halign " " valign " " color


  if(anchor == 7 || anchor == 8 || anchor == 9) {
    if     (rot == 0) {rot = 2; flip = 0}
    else if(rot == 1) {rot = 1; flip = 0}
    else if(rot == 2) {rot = 0; flip = 0}
    else if(rot == 3) {rot = 3; flip = 0}
    else if(rot == 4) {rot = 2; flip = 1}
    else if(rot == 5) {rot = 1; flip = 1}
    else if(rot == 6) {rot = 0; flip = 1}
    else if(rot == 7) {rot = 3; flip = 1}
    else              {rot = 0; flip = 0}
  } else {
    if     (rot == 0) {rot = 2; flip = 1}
    else if(rot == 1) {rot = 1; flip = 1}
    else if(rot == 2) {rot = 0; flip = 1}
    else if(rot == 3) {rot = 3; flip = 1}
    else if(rot == 4) {rot = 2; flip = 0}
    else if(rot == 5) {rot = 1; flip = 0}
    else if(rot == 6) {rot = 0; flip = 0}
    else if(rot == 7) {rot = 3; flip = 0}
    else              {rot = 0; flip = 0}
  }
  return rot " " flip " "  size " " size " {" tattrs "}"
}


# the various align/rotate combinations are obtained by reverse engineering
function inst_rot_flip(rot)
{
  if     (rot == 0) return "0 0"
  else if(rot == 1) return "3 0"
  else if(rot == 2) return "2 0"
  else if(rot == 3) return "1 0"
  else if(rot == 4) return "0 1"
  else if(rot == 5) return "3 1"
  else if(rot == 6) return "2 1"
  else if(rot == 7) return "1 1"
  else return "0 0"
}
function color_map(c)
{
  if(c == 2) return 4  # green
  if(c == 12) return 5 # red
  if(c == 4) return 5  # red
  else return 4
}

# x, y, length  axis transforms. Xschem is downward oriented!
# any geometric coordinate gets processed by these functions
function xscale(x)
{
  return x
}

function yscale(y)
{
  return -y
}

# distance transformations (like circle radius) 
function lenscale(len)
{
  return len
}

# print debug info on stderr if debug global is != 0
function dbg(str)
{
  if(debug) {
     print str > "/dev/stderr"
  }
}

# get all of $0 following $n
function get_fields_after(n,     str, nf, i, a, arr)
{
  str = ""
  a = $0
  nf = split(a, arr)
  for(i = n + 1; i <= nf; i++) {
    if(i > n + 1) str = str " "
    str = str arr[i]
  }
  return str
}

#used to poke what's next wihout advancing read to next line
function next_line()
{
  return line[lineno + 1] 
}

# xschem rectangle coordinates are ordered for efficiency reasons
function rectordered(x1, y1, x2, y2,    tmp)
{
  if(x2 < x1) {
    tmp = x1; x1 = x2; x2 = tmp
  }
  if(y2 < y1) {
    tmp = y1; y1 = y2; y2 = tmp
  }
  return x1 " " y1 " " x2 " " y2
}

# xschem line coordinates are ordered for efficiency reasons
function ordered(x1, y1, x2, y2,    tmp)
{
  if(x2 < x1) {
    tmp = x1; x1 = x2; x2 = tmp
    tmp = y1; y1 = y2; y2 = tmp
  }
  if(x1 == x2 && y2 < y1) {
    tmp = x1; x1 = x2; x2 = tmp
    tmp = y1; y1 = y2; y2 = tmp
  }
  return x1 " " y1 " " x2 " " y2
}
