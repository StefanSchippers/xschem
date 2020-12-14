#!/usr/bin/awk -f
#
#  File: flatten.awk
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
# flatten .SAVE netlist lines

BEGIN{
 # topcell=toupper(ARGV[2])
 # ARGC=2
 first_subckt = 1
 pathsep="."
}

{
 up = toupper($0)
 if(up ~/^[ \t]*\.END([ \t]+|$)/ ) do_end = $0
 else if(up ~/^[ \t]*\.SAVE[ \t]+/){
   if(toupper($2) !="ALL") do_save = 1
   gsub(/V\([ \t]*/, "V( ", up)
   gsub(/[ \t]*\)/, " )", up)
 } else print
 $0=up
 if($0 ~ /^\**\.SUBCKT/ && first_subckt) {
   topcell=$2
   sub(/^\*\*/,"",$0)
 }
 if($0 ~ /^\**\.ENDS/ && first_subckt) {
   first_subckt = 0 
   sub(/^\*\*/,"",$0)
 }
 if($0 ~/^\+/) 				# join folded lines
 {
  a[lines-1]=a[lines-1] " " substr($0,2); next
 }
 a[lines++]=$0
} 

END{
 for(j=0;j<lines;j++) 
 {
  $0=a[j]
  if($1 ~ /\.GLOBAL/)			# get global nodes
   for(i=2;i<=NF;i++) global[$i]=i;
  if($1 ~ /^\.SUBCKT/)			# parse subckts
  {					# subckt["name","first"]= first line 
    subname=$2				# subckt["name","last"]=  last line
    subckt[subname,"first"]=j		# subckt["name", "ports"] = # of ports
    for(k=3;k<=NF;k++)			# subckt["name", "port","node"]=
					# port number (1,2,...)
    {
     if($k ~ /=/) break
     subckt[subname,"port",$k]=k-2
    }
    subckt[subname,"ports"]=k-3         
  } 
  if($1 ~ /^\.ENDS/) {
    subckt[subname,"last"]=j
  }
 }
 print "** flattened .save nodes"
 expand(topcell,"","")
 # parameters: 
 #	- subckt name to expand
 #	- current path (will be prefixed to inst & node names)
 #	- port list to connect the subckt to.

 if(do_end !="") print do_end
} 

# recursive routine!!! private variables must be declared local !!
function expand(name, path,ports,    			# func. params
         portlist,portarray,j,k,subname,		# local vars
         pathname,pathnode, 				# local vars
         instname,num,line,subname_pos)			# local vars
{
  if(path != "")
  {
    pathname=pathsep path ; pathnode=path pathsep
  }
  split(ports,portarray)
  for(j=subckt[name , "first"]+1;j<subckt[name , "last"];j++) {
    num=split(a[j],line)
    if(line[1] ~ /^X/ ) {
      portlist = ""; subname=""
      for(k=num;k>=2;k--) {
        if(line[k] !~ /=/) {
          if(subname=="") {subname=line[k]; subname_pos=k }
          else if( (subname,ports) in subckt && k<subckt[subname,"ports"]+0) 
            portlist = getnode(name,pathnode,portarray,line[k]) " " portlist
          else if(k<subname_pos){ 
            # undefined subcircuit
            portlist = getnode(name,pathnode,portarray,line[k]) " " portlist
          }
        }
      }
      # print "*--------BEGIN_" pathnode line[1]_"->" subname
      if( (subname,"first") in subckt) # 30032003 do not expand subcircuit call if undefined subckt
        expand(subname,pathnode line[1],portlist)
      # print "*--------END___" pathnode line[1] "->" subname 
    }
    else {
      if(line[1] ~ /^\.SAVE$/) {
        printf ".SAVE "
        for(k = 2; k <= num; k++) {
          if(k > 2) printf " " 
          if(line[k] ~ /^V\($/) {
            printf "V(%s)",getnode(name,pathnode,portarray,line[k+1])
            k += 2
          } else {
            printf "%s", line[k]
          }
        }
        printf "\n"
      }
      if(line[1] ~ /^\V/ && do_save == 1) {
        if(name !=topcell) 
          printf ".SAVE I(V.%s)\n", pathnode line[1]
        else
          printf ".SAVE I(%s)\n", line[1]
      } 
    }
  }
}

function getnode(name, path, portarray, node)
# return the full path-name of <node> in subckt <name> 
# in path <path>, called with ports <portarray>
{
 if(name!=topcell) {		# if we are in top cell, nothing to do
   if(name SUBSEP "port" SUBSEP node in subckt)
     return portarray[subckt[name,"port",node]] # <node> is a port, return port mapping
   if(!(node in global)) return path node	# local node
 }
 return node	# if <node> is a top level or global (not a port) just return
}

