#!/usr/bin/awk -f
#
#  File: convert_to_verilog2001.awk
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



NF==3 && $0 ~/^module/ {
  module=1
  port_decl=0
  parameter = ""
  ports = ""
  comma=0
  sub(/[ \t]*\(/, "")
  print
  next
}

module==1 && $0 == ");" {
  port_decl=1
  module=0
  next
}

port_decl==1 && $1 ~ /^(output|input|inout)$/{
  dir=$1 #20161118
  getline 
  sub(/;[ \t]*$/,"")
  $1 = dir " " $1
  if(comma) ports = ports ",\n"
  ports = ports  "  " $0
  comma=1
  next
}
 
port_decl==1 && $0 ~ /^parameter/{
  sub(/[ \t]*;[ \t]*$/, "")
  if(parameter!="") parameter = parameter ",\n"
  parameter =  parameter "  " $0
  next
}
port_decl==1 && $0 ~/^[ \t]*$/{
  port_decl=0
  if(parameter) print "#(\n" parameter "\n)"
  print  "(\n" ports "\n);"
  
  next
}

module==1 || port_decl==1{
  next
}

{ 
  print
}
