#!/usr/bin/awk -f
#
#  File: flatten_tedax.awk
#  
#  This file is part of XSCHEM,
#  a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
#  simulation.
#  Copyright (C) 1998-2024 Stefan Frederik Schippers
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


BEGIN{
  first = 1
  hier = 0
  debug = 0
  hiersep="."
}

{
  reparse()
}

/^begin netlist/{
  cell = $4
  if(first) {
    topcell=cell
    first = 0
  }
  start[cell] = NR
}

/^end netlist/{
  end[cell] = NR
}
/^__GLOBAL__/{
  global[$2] = 1
}

/^__HIERSEP__/{
  hiersep = $2
}

{
  netlist[NR] = $0
}

END{
  out("tEDAx v1")
  out("begin netlist v1 " topcell)
  expand(topcell, "", "", "")
  out("end netlist")
}


function expand(cell, instname, path, maplist,       i, j, subpos, subcell, subinst, submaplist)
{
  hier++
  for(i = start[cell]+1; i <= end[cell]-1; i++) {
    $0 = netlist[i]
    reparse()
    # __subcircuit__ pcb_voltage_protection x0 
    # __map__ VOUT -> VOUTXX
    # __map__  ... -> ...
    if($1 == "__subcircuit__") {
      subcell=$2
      subinst=$3
      subpos = i
      sub(/__subcircuit__/, "subcircuit", $0)
      out("#" spaces(hier * 2 - 1) $0)
      submaplist = "" 
      for(i++; ;i++) {
        $0 = netlist[i]
        reparse()
        if($1 != "__map__") break
        dbg("expand: $4=" $4)
        $4 = resolve_node($4, path, maplist)
        submaplist = submaplist " " $2 " " $4
        sub(/__map__/, "map", $0)
        out("#" spaces(hier * 2 - 1) $0)
      }
      
      expand(subcell, subinst, path subinst hiersep, submaplist)
 
      i--; continue; # redo processing of current line
    }
    if($1 == "conn") {
      dbg("conn: $2=" $2)
      $2 = resolve_node($2, path, maplist) #net name
      $3 = path $3 #refdes
    }
    if($1 ~/^(device|footprint|value|spiceval|spicedev|comptag|pinname|pinslot|pinidx)$/) {
      $2 = path $2 #refdes
    }
    out(spaces(hier * 2) $0)
    if($0 ~ /^end netlist/) break
  }
  hier--
}

function spaces(n,     s)
{
  n = (n > 0) ? n - 1 : 0 
  s ="                        "
  while(length(s) < n) s = s s
  return substr(s,1,n)
}

function out(s)
{
  if(length(s) > 512) 
    print "flatten_tedax: WARNING: >> " s "\n  line length > 512 chars, this breaks tEDAx" > "/dev/stderr"
  print s
}

function dbg(s)
{
  if(debug) print s > "/dev/stderr"
}

function resolve_node(node, path, maplist,     arr, n, retnode, i) 
{
  dbg("resolve_node: node=" node " maplist=" maplist)
  gsub(/\\ /, SUBSEP "s", maplist)
  gsub(/\\\t/, SUBSEP "t", maplist)
  n = split(maplist, arr)
  for(i = 1; i <= n; i++) {
    gsub(SUBSEP "s", "\\ ", arr[i])
    gsub(SUBSEP "t", "\\\t", arr[i])
  }
  if(node in global) retnode = node
  else for(i = 1; i <= n; i += 2) {
    if(node == arr[i]) {
      retnode = arr[i+1]
      break
    }
  }
  if(retnode =="") retnode = path node
  return retnode
}

# avoid considering escaped white spaces as field separators
function reparse(    i)
{
  gsub(/\\ /, SUBSEP "s")
  gsub(/\\\t/, SUBSEP "t")
  for(i = 1; i <= NF; i++) {
    gsub(SUBSEP "s", "\\ ", $i)
    gsub(SUBSEP "t", "\\\t", $i)
  }
}

