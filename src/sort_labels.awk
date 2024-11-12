#!/usr/bin/awk -f
#
#  File: sort_labels.awk
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
 for(i=0;i<=127;i++)
 {
  val[sprintf("%c",i)]=i  # value of char: val["0"]=45
  char[i]=sprintf("%c",i) # char[48]="0"
 }
 lines=0
}
# LDYMS[12]

{ 
  line[lines++] = $0 " " hash_string($1)
}


END{
  close(FILENAME) 

  for(i=0;i< lines;i++) {
    print line[i] > FILENAME
  }
  close(FILENAME)
  system("sort -k 2 " FILENAME " | awk '{$NF=\"\";  print $0}' > "  FILENAME ".xxxxx")
  system("mv " FILENAME ".xxxxx " FILENAME)
}


function hash_string(s,         n, i, h, c) 
{
  n = s
  sub(/^.*\[/,"",n)
  sub(/\].*/,"",n)
  sub(/\[.*/, "", s) 

  h = s "__" sprintf("%08d", n)

  return h
}
