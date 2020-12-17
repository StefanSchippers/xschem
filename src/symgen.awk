#!/usr/bin/awk -f
# 
#  File: symgen.awk
#  
#  This file is part of XSCHEM,
#  a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
#  simulation.
#  Copyright (C) 1998-2020 Stefan Frederik Schippers
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

# create a symbol file from a symdef file, like the geda djboxsym utility
BEGIN{
  debug = 1
  symbolx1 = 0
  symbolx2 = 700 # recalculated when number of top/bottom pins is known
  symboly1 = 0
  symboly2 = 700 # recalculated when number of left/right pins is known
  firstpinyoffset = 40 # recalculated when top pin max label length is known
  lastpinyoffset = 40 # recalculated when bottom pin max label length is known
  pinspacing = 40
  pinbusspacing = 20
  pintextoffset = 11
  pintextpadding = 18
  pinboxhalfsize = 2.5
  pinlinelength = 40
  pintextsize = 0.4
  overbaroffset = 6 * pintextsize
  pintextcenteroffset = 24 * pintextsize
  pintextheight = 56.6 * pintextsize
  pincharspacing = 35 * pintextsize
  pinnumbertextsize = 0.25
  pinnumbercenteroffset = 16
  pinnumberoffset = 12
  labeltextsize = 0.6
  labelspacing = 40 # vertical spacing of label strings
  labelcharspacing = 30 * labeltextsize
  labeloffset = 20 # extra space to put around labels to avoid collision with pin text labels
  labelcenteroffset = 20 * labeltextsize
  negradius = 5
  triggerxsize = 8
  triggerysize = 8
  grid = 20
  start_pin=0
  nlines = 0
  bus = 0
  pos = 0 ## default if no [position] field specified
  horizontal = 1 # default orient. for top/bottom pins, can be changed with --vertical
}

/^--/{
  if(/^--auto[-_]pinnumber/) { enable_autopinnumber = 1 }
  if(/^--hide[-_]pinnumber/) { hide_pinnumber = 1 }
  if(/^--vmode/) { horizontal = 0 } # force horizontal top/bottom labels
  next
}

/^[ \t]*#/{
  next
}

/^[ \t]*\[labels\]/ {
  start_labels = 1
  next
}

start_labels == 1 {
  if(/^\[/) start_labels = 0
  else if(/^! /) {
    if(attributes) attributes = attributes "\n"
    attributes = attributes substr($0, 3)
    dbg("attributes=" attributes)
    next
  } else if(NF > 0){
    labn = label["n"]++
    label[labn] = $0
    if( length($0) > label["maxlength"] ) label["maxlength"] = length($0)
    dbg("label: " $0)
    next
  }
}

/^[ \t]*\[left\]/   { pos = 0 }
/^[ \t]*\[top\]/    { pos = 1 }
/^[ \t]*\[right\]/  { pos = 2 }
/^[ \t]*\[bottom\]/ { pos = 3 }
/^[ \t]*\[[^][]*\][ \t]*$/ {
  start_labels = 0
  posseq[seqidx++] = pos
  pin[pos, "n"] = 0;
  start_pin = 1;
  coord = 0;
  next
}

/^\./{ 
  if(/^\.bus/) {
    if( !horizontal || (pos == 0 || pos == 2) ) {
      bus = 1
      firstbus=1
    }
  }
  next
}

## typical pin line is as follows
## 26   i!>   CLK
start_pin {
  if(NF > 0 && enable_autopinnumber) $0 = ++autopinnumber " " $0
  n =  pin[pos, "n"]
  if(NF == 0) {
    if(bus == 1) coord += delta1
    else coord += pinspacing
    bus = 0
    dbg("spacer")
    pin[pos, n, "spacer"] = 1
    pin[pos, "n"]++
  } else if(NF == 2) { $0 = $1 " io " $2 } ## backward compatibility with 'djboxsym' format
  if(NF == 3) {
    trig = 0
    neg = 0
    dir = "inout" ## default if (mandatory) direction forgotten
    if($2 ~ />/) trig = 1
    if($2 ~ /!/) neg = 1
    if($2 ~ /io/) dir = "inout"
    else if($2 ~ /i/) dir = "in"
    else if($2 ~ /o/) dir = "out"
    else if($2 ~ /p/) dir = "inout" # xschem has no special attrs for power so set direction as inout
    pinnumber = $1
    pinname = $3
    if(pinname ~ /^\\_.*\\_$/) {
      pin[pos, n, "overbar"] = 1
      sub(/^\\/, "", pinname)
      sub(/\\_$/, "_", pinname)
    }
    pin[pos, n, "pinname"] = pinname
    pin[pos, n, "pinnumber"] = pinnumber
    pin[pos, n, "dir"] = dir
    pin[pos, n, "neg"] = neg
    pin[pos, n, "trigger"] = trig
    pin[pos, n, "bus"] = bus
    if((pos == 1 || pos == 3 ) && horizontal )  {
      delta1 = delta2 = round((length(pin[pos, n, "pinname"]) * pincharspacing  + pintextpadding)) / 2
    } else {
      if(bus) {
        if(!firstbus) delta1 = pinbusspacing / 2
        else delta1 = pinspacing / 2
        delta2 = pinbusspacing / 2
        firstbus = 0
      } else {
        delta1 = delta2 = pinspacing / 2
      }
    }
    
    coord += delta1
    pin[pos, n, "coord"] = round(coord)
    dbg("pin[" pos ", " n ", coord]: " pin[pos, n, "coord"] " pinname: " \
        pinname " bus: " bus " delta: " delta1 ":" delta2 " coord: " coord)
    # dbg("delta1: " delta1 " delta2: " delta2)
    coord += delta2
    pin[pos, "maxcoord"] = coord
    if( length(pinname) > pin[pos, "maxlength"] ) pin[pos, "maxlength"] = length(pinname)
    pin[pos, "n"]++
  }
}

## GENERATOR
END{
  header()
  attrs(attributes)
 
  if(horizontal !=1) {
    firstpinyoffset = round(pintextoffset + pincharspacing * pin[1, "maxlength"])
    lastpinyoffset = round(pintextoffset + pincharspacing * pin[3, "maxlength"])
  }
 
  dbg("pin[1, maxlength]: " pin[1, "maxlength"])
  dbg("pin[3, maxlength]: " pin[3, "maxlength"])
  dbg("firstpinyoffset: " firstpinyoffset)
  dbg("lastpinyoffset: " lastpinyoffset)
  ## round to double grid so half size is grid-aligned when centering
  symboly2 = round(max(pin[0,"maxcoord"], pin[2, "maxcoord"]) / 2) * 2 + firstpinyoffset + lastpinyoffset
  dbg("symboly2: " symbolx2)
  dbg("pin[3, maxcoord]: " pin[3, "maxcoord"])
  dbg("pin[1, maxcoord]: " pin[1, "maxcoord"])
  dbg("pin[0, maxcoord]: " pin[0, "maxcoord"])
  dbg("pin[2, maxcoord]: " pin[2, "maxcoord"])
  topbotpinsize = max(pin[1,"maxcoord"], pin[3, "maxcoord"]) + pintextpadding / 2
  labsize = ( (labelcharspacing * label["maxlength"] + labeloffset)/2 \
             + max(pincharspacing * pin[0, "maxlength"], pincharspacing * pin[2, "maxlength"])) * 2
  dbg("topbotpinsize: " topbotpinsize)
  dbg("labsize: " labsize)
  dbg("pincharspacing * pin[0, maxlength]: " pincharspacing * pin[0, "maxlength"])
  dbg("pincharspacing * pin[2, maxlength]: " pincharspacing * pin[2, "maxlength"])
  symbolx2 = round( max(topbotpinsize, labsize) / 2) * 2 ## round to double grid so half size is grid-aligned when centering
  dbg("symbolx2: " symbolx2)
  ## center symbol after size calculations are done
  symbolx1 -= round(symbolx2/2)
  symbolx2 += symbolx1
  symboly1 -= round(symboly2/2)
  symboly2 += symboly1
  pinseq = 0;
  for(i = 0; i < 4; i++) {
    p = posseq[i]
    for(n = 0; n < pin[p,"n"]; n++) {
      dir = pin[p, n, "dir"]
      plinelay = (dir =="in") ? 4 : (dir == "inout") ? 3 : 2
      if(p == 0 ) { 
        x = symbolx1 - pinlinelength
        y = symboly1 + pin[p, n, "coord"] + firstpinyoffset
      }
      else if(p == 1 ) { 
        x = symbolx1 + pin[p, n, "coord"]
        y = symboly1 - pinlinelength
      }
      else if(p == 2 ) { 
        x = symbolx2 + pinlinelength
        y = symboly1 + pin[p, n, "coord"] + firstpinyoffset
      }
      else if(p == 3 ) { 
        x = symbolx1 + pin[p, n, "coord"]
        y = symboly2 + pinlinelength
      }
      if(pin[p, n, "spacer"] != 1) {
        pinbox(p, n, x, y)
        neg = pin[p, n, "neg"]
        trig = pin[p, n, "trigger"]
        pinname = pin[p, n, "pinname"]
        if(pin[p, n, "overbar"] == 1) {
          sub(/^_/,"", pinname)
          sub(/_$/,"", pinname)
        }
        if(p == 0) {
          line(x, y, x + (neg ? pinlinelength - 2 * negradius : pinlinelength), y, plinelay, "")
          text(pinname, x + pinlinelength + pintextoffset, y - pintextcenteroffset, 0, 0, pintextsize, "")
          if(pin[p, n, "overbar"] == 1) {
            obx = x + pinlinelength + pintextoffset
            oby = y - pintextcenteroffset - overbaroffset
            obl = (length(pin[p, n, "pinname"]) - 2 ) * pincharspacing
            line(obx, oby, obx + obl, oby, 3, "")
          }
          if(!hide_pinnumber)text(pinnumber_id(pinseq), x + pinlinelength - pinnumberoffset, \
               y - pinnumbercenteroffset, 0, 1, pinnumbertextsize, "layer=13")
          if(neg) circle(x + pinlinelength - negradius, y, negradius, plinelay, "")
        } else if(p == 1) {
          line(x, y, x, y + (neg ? pinlinelength - 2 * negradius : pinlinelength) , plinelay, "")
          if(horizontal) {
            halflabwidth = length(pinname) * pincharspacing / 2
            text(pinname, x - halflabwidth, y + pinlinelength + pintextoffset, 0, 0, pintextsize, "")
            if(pin[p, n, "overbar"] == 1) {
              obx = x - halflabwidth
              oby = y + pinlinelength + pintextoffset - overbaroffset
              obl = (length(pin[p, n, "pinname"]) - 2 ) * pincharspacing
              line(obx, oby, obx + obl, oby, 3, "")
            }
          } else {
            text(pinname, x + pintextcenteroffset, y + pinlinelength + pintextoffset, 1, 0, pintextsize, "")
            if(pin[p, n, "overbar"] == 1) {
              obx = x + pintextcenteroffset - pintextheight - overbaroffset
              oby = y + pinlinelength + pintextoffset
              obl = (length(pin[p, n, "pinname"]) - 2 ) * pincharspacing
              line(obx, oby + obl, obx, oby, 3, "")
            }
          }
          if(!hide_pinnumber)text(pinnumber_id(pinseq), x - pinnumbercenteroffset, \
               y + pinlinelength - pinnumberoffset, 3, 0, pinnumbertextsize, "layer=13")
          if(neg) circle(x, y + pinlinelength - negradius, negradius, plinelay, "")
        } else if(p == 2) {
          line(x - (neg ? pinlinelength -2 * negradius :pinlinelength), y, x, y, plinelay, "")
          text(pinname, x - pinlinelength - pintextoffset, y - pintextcenteroffset, 0, 1, pintextsize, "")
          if(pin[p, n, "overbar"] == 1) {
            obx = x - pinlinelength - pintextoffset
            oby = y - pintextcenteroffset - overbaroffset
            obl = (length(pin[p, n, "pinname"]) - 2) * pincharspacing
            line(obx - obl, oby, obx, oby, 3, "")
          }
          if(!hide_pinnumber)text(pinnumber_id(pinseq), x - pinlinelength + pinnumberoffset, \
               y - pinnumbercenteroffset, 0, 0, pinnumbertextsize, "layer=13")
          if(neg) circle(x - pinlinelength + negradius, y, negradius, plinelay, "")
        } else if(p == 3) {
          line(x, y - (neg ? pinlinelength -2 * negradius :pinlinelength), x, y, plinelay, "")
          if(horizontal) {
            halflabwidth = length(pinname) * pincharspacing / 2
            text(pinname, x - halflabwidth, y - pinlinelength - pintextoffset, 2, 1, pintextsize, "")
            if(pin[p, n, "overbar"] == 1) {
              obx = x - halflabwidth
              oby = y - pinlinelength - pintextoffset - pintextheight - overbaroffset
              obl = (length(pin[p, n, "pinname"]) - 2 ) * pincharspacing
              line(obx, oby, obx + obl, oby, 3, "")
            }
          } else {
            text(pinname, x + pintextcenteroffset, y - pinlinelength - pintextoffset, 1, 1, pintextsize, "")
            if(pin[p, n, "overbar"] == 1) {
              obx = x + pintextcenteroffset - pintextheight - overbaroffset
              oby = y - pinlinelength - pintextoffset
              obl = (length(pin[p, n, "pinname"]) - 2 ) * pincharspacing
              line(obx, oby, obx, oby - obl, 3, "")
            }
          }
          if(!hide_pinnumber)text(pinnumber_id(pinseq), x - pinnumbercenteroffset, \
               y - pinlinelength + pinnumberoffset, 3, 1, pinnumbertextsize, "layer=13")
          if(neg) circle(x, y - pinlinelength + negradius, negradius, plinelay, "")
        }
        if(trig) trigger(x, y, plinelay, p)
        pinseq++
      }
    } # for(n)
  } # for(p)
  box(symbolx1, symboly1, symbolx2, symboly2, 4, "")

  for(l = 0; l < label["n"]; l++) {
    dbg("label: " l " : " label[l])
    labx = (symbolx1 + symbolx2) / 2 - length(label[l]) * labelcharspacing /2 
    laby = (symboly1 + symboly2) / 2 + ( l - label["n"] / 2 ) * labelspacing + labelcenteroffset
    text(label[l], labx, laby, 0, 0, labeltextsize, "")
  }
}  

function dbg(s)
{
  if(debug) print s > "/dev/stderr"
}

function pinbox(p, n, x, y, arr)
{
  print "B 5", x-pinboxhalfsize, y-pinboxhalfsize, x+pinboxhalfsize, y+pinboxhalfsize, \
    "{name=" esc(pin[p, n, "pinname"]), \
     "dir=" pin[p, n, "dir"], \
     "pinnumber=" esc(pin[p, n, "pinnumber"]) "}"
}

function trigger(x, y, layer, pos)
{
  if(pos == 0) {
    line(x + pinlinelength, y - triggerysize, x + triggerxsize + pinlinelength, y, layer, "")
    line(x + pinlinelength , y + triggerysize, x + pinlinelength + triggerxsize, y, layer, "")
  } else if(pos == 2) {
    line(x - pinlinelength - triggerxsize, y, x - pinlinelength, y - triggerysize, layer, "")
    line(x - pinlinelength - triggerxsize, y , x - pinlinelength , y + triggerysize, layer, "")
  } else if(pos == 1) {
    line(x - triggerysize, y + pinlinelength, x, y + pinlinelength + triggerxsize, layer, "")
    line(x , y + pinlinelength + triggerxsize, x + triggerysize, y + pinlinelength, layer, "")
  } else if(pos == 3) {
    line(x - triggerysize, y - pinlinelength, x, y - pinlinelength - triggerxsize, layer, "")
    line(x , y - pinlinelength - triggerxsize, x + triggerysize, y - pinlinelength, layer, "")
  }
}


function pinnumber_id(pinseq) 
{
  return("@#" pinseq ":pinnumber")
}

function circle(x, y, r, layer, props)
{
  print "A", layer, x, y, r, 0, 360, "{" esc(props) "}"
}

function box(x1, y1, x2, y2, layer, props)
{
  ## closed polygon
  print "P", layer, 5, x1, y1, x2, y1, x2, y2, x1, y2, x1, y1, "{" esc(props) "}"
}

function line(x1, y1, x2, y2, layer, props)
{
  print "L", layer, x1, y1, x2, y2, "{" esc(props) "}"
}

function text(t, x, y, rot, flip, size, props)
{
  print "T {" esc(t) "}", x, y, rot, flip, size, size, "{" esc(props) "}"
}

function attrs(a)
{
  print "G {" esc(a) "}"
}

function header()
{
  print "v {xschem version=2.9.9  file_version=1.2}"
}

function round(n)
{
  return n==0 ? 0 : (n > 0) ? (int( (n-0.001)/grid) +1) * grid : (int( (n+0.001)/grid) -1) * grid
}

function max(a, b) { return (a > b) ? a : b }

function esc(s)
{
  gsub(/\\/, "\\\\", s)
  gsub(/{/, "\\{", s)
  gsub(/}/, "\\}", s)
  return s
}
