#!/usr/bin/gawk -f
#
#  File: create_alloc_ids_windows.awk
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

# replaces _ALLOC_ID_ in all source files with unique ID for memory tracking

BEGIN{
  if(ARGC <= 1) {
     print "Usage: " ENVIRON["_"] " create|reset <filename>"
     exit
  }
  create_id = 0
  if(ARGV[1] == "create") create_id = 1
  else if (ARGV[1] != "reset") {
     print "Usage: " ENVIRON["_"] " create|reset <filename>"
     exit
  }
  cnt = 0;
  filename = ARGV[2]
  if(filename ~ /expandlabel\.c/) exit
  if(filename ~ /expandlabel\.h/) exit
  if(filename ~ /parselabel\.c/) exit
  f = ""
  start = 1
  changed = 0
  while(getline < filename) {
    if(create_id == 0) {
      if(!start) f = f "\n"
        str = gensub(/(my_(malloc|calloc|realloc|free|strcat|strncat|mstrcat|strdup|strdup2))\([0-9]+,/, "\\1(_ALLOC_ID_,", "G")
        if(str != $0) changed = 1
        f = f str
      } else {
        if(!start) f = f "\n"
        if($0 !~ /^#define *_ALLOC_ID_/) {
          while(sub(/_ALLOC_ID_/, cnt)) {
          changed = 1
          cnt++
        }
      }
      f = f $0
    }
    start = 0
  }
  close(filename)
  if(changed) {
    print f > filename
    close(filename)
  }
}
