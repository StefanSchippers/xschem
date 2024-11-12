#!/usr/bin/awk  -f
#
#  File: break.awk
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

#break VERY long lines

BEGIN{ quote=0 }

/^\*\*\*\* begin user (architecture|header) code/ { user_code = 1 }

{
 pos=0
 sub(/[ \t]*$/, "") # chop trailing white space
 if(NF==0) { print ""; next }
 first = substr($0,1,1)

 # dont break .include lines as ngspice chokes on these.
 if(user_code == 1) nobreak = 1
 else if(tolower($1) ~ /\.inc(lude)?|\.lib|\.title|\.save|\.write/) nobreak = 1
 else if($0 ~/^\*\* ..._path:/) nobreak = 1
 else nobreak = 0
 # 20151203 faster executionif no {}' present
 if($0 ~/[{}']/ || quote) {
   l = length($0)
   for(i=1;i<=l;i++) {
     pos++
     c = substr($0,i,1)
     if(c ~/[{}']/) quote=!quote 
     if(!nobreak && pos> 130 && !quote && (c ~/[ \t]/)) {
       if(first=="*") 
         c = "\n*+" c
       else
         c = "\n+" c
       pos=0
     }
     printf "%s",c
   }
   printf "\n" 
 } else {  #20151203
   split($0, a, /[^ \t]+/)
   for(i=1;i<=NF;i++) {
     pos += length($i)+length(a[i])
     if(!nobreak && pos>130) {
       if(first=="*") {
         printf "%s", "\n*+"
       } else {
         printf "%s", "\n+"
       }
       pos=0
     } # else if(i>1) {
       # printf " "
     # }
     printf "%s%s", a[i], $i
   }
   printf "\n"
 }
}

/^\*\*\*\* end user (architecture|header) code/ { user_code = 0 }
