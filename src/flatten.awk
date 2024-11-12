#!/usr/bin/awk -f
#
#  File: flatten.awk
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
#
# CDL netlist flattener,
# 19-09-1998  Stefan Schippers
#
# Rel. history:
#
# 0.1: 19-09-1998:      parameters, global node declarations recognized.
# 0.2: 27-09-1998:	nodes inside expression are also recognized
#			{value= V(IN)*3+V(A) ....}
# 0.3: 16-11-1998	Bug fixes
# 0.4: 28-03-2003	added ccvs vcvs cccs vccs
#			adapted for xschem netlist
#      30-03-2003       do not expand subcircuit if no .subckt present in netlist


BEGIN{
 # topcell=toupper(ARGV[2])
 # ARGC=2
 first_subckt = 1
 global["0"] = 1 # 0 is always a global reference node
 pathsep="_"
 nodes["M"]=4; nodes["R"]=2; nodes["D"]=2; nodes["V"]=2
 nodes["I"]=2; nodes["C"]=2; nodes["L"]=2; nodes["Q"]=3
 nodes["E"]=4; nodes["G"]=4; nodes["H"]=2; nodes["F"]=2
 nodes["B"]=2; nodes["S"]=4
 controlblock = 0
}
{
 if(toupper($0) ~ /^[ \t]*\.CONTROL/) controlblock = 1
 if(controlblock == 0 && toupper($0) !~/^(\.INCLUDE|\.LIB|\.WRITE| *WRITE)/) $0=toupper($0)
 if(toupper($0) ~ /^[ \t]*\.ENDC/) controlblock = 0
 # allow to specify *.nodes[W]=2 or *.nodes["W"] = 2 metadata in the netlist for additional
 # custom devices nodes specification.
 if($0 ~/^[ \t]*\*\.[ \t]*NODES\["?[^]["]"?\][ \t]*=[ \t]*.*/) {
   n_nodes = $0
   sub(/^.*=[ \t]*/, "", n_nodes)
   n_initial = $0
   sub(/"?\].*/, "", n_initial)
   sub(/^.*\["?/, "", n_initial)
   nodes[n_initial] = n_nodes
 }
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

 gsub(/[\t ]*=[\t ]*/, "=")
 a[lines++]=$0
} 

END{

 for(j=0;j<lines;j++) a[j] = trim_quoted_spaces(a[j])
 devpattern = "^["
 for(j in nodes) {
   devpattern = devpattern j
 }
 devpattern = devpattern "]"
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
    for(;k<=NF;k++)			# subckt["name", "param", "parname"]=
     if($k ~ /=/)			# default value
     {
      split($k,tmp,"=")
      subckt[subname,"param",tmp[1]]=tmp[2]
     }
  } 
  if($1 ~ /^\.ENDS/) {
    subckt[subname,"last"]=j
  }
 }
 print "** " topcell " flat netlist"
 expand(topcell,"","","")
 print ".end"
 # parameters: 
 #	- subckt name to expand
 #	- current path (will be prefixed to inst & node names)
 #	- string of params: par1=val1 par2=val2 par3=val3 ...
 #	- port list to connect the subckt to.

} 


# recursive routine!!! private variables must be declared local !!

function expand(name, path, param,ports,   		# func. params
         portlist,portarray,paramlist,j,k,subname,	# local vars
         paramarray,paramarray2,pathname,pathnode, 	# local vars
         instname,parameter,num,line,subname_pos)	# local vars
{
 if(path != "")
 {
  pathname=pathsep path ; pathnode=path pathsep
 }
 split(param,paramarray)
 for(k in paramarray) 
 {
  split(paramarray[k],parameter,"=")
  paramarray2[parameter[1]]=parameter[2]
 }
 split(ports,portarray)
 controlblock = 0
 for(j=subckt[name , "first"]+1;j<subckt[name , "last"];j++)
 {
  if(a[j] ~ /^[ \t]*\.control/) controlblock = 1
  num=split(a[j],line)
  if(controlblock) {
    print a[j]
  } else if(line[1] ~ /^X/ )
  {
   paramlist = ""; portlist = ""; subname=""
   for(k=num;k>=2;k--)
   {
    if(line[k] ~ /=/) 
    {
     split(line[k],parameter,"=")
     if(parameter[2] in paramarray2)           
      paramlist= parameter[1] "=" paramarray2[parameter[2]] " " paramlist
     else paramlist= line[k] " " paramlist
    }
    else if(subname=="") {subname=line[k]; subname_pos=k }
    else if( (subname,ports) in subckt && k<=subckt[subname,"ports"]+1) 
     portlist = getnode(name,pathnode,portarray,line[k]) " " portlist
    else if(k<subname_pos){ 
     # 30032003 undefined subcircuit
     portlist = getnode(name,pathnode,portarray,line[k]) " " portlist
    }
   }
   print "*--------BEGIN_" pathnode line[1]_"->" subname

   if( (subname,"first") in subckt) # 30032003 do not expand subcircuit call if undefined subckt
     expand(subname,pathnode line[1],paramlist,portlist)
   else
   {
    printf "%s %s %s %s\n",line[1] pathname , portlist, subname, paramlist
   }

   print "*--------END___" pathnode line[1] "->" subname 
  }
  else
  {
   if(line[1] ~ devpattern)
   {
    nn = nodes[substr(line[1],1,1)]
    if(a[j] ~ /^[GE].*(VALUE|CUR|VOL)=/) nn = 2 # behavioral VCVS/VCCS have 2 nodes only
    printf "%s ",line[1] pathname
    for(k = 2; k <= nn + 1; k++)
     printf "%s ", getnode(name,pathnode,portarray,line[k])
    for(; k<=num;k++)
    {
     if(line[1] ~ /^[FH]/ && k==4)  printf "%s ", line[k] pathname
     else if(line[k] ~ /^(VALUE|VOL|CUR|R|C|L|V|I)=/) {
       # expressions contain spaces, but usually end the line. Concatenate all fields into line[k]
       for(m = k + 1; m <= num; m++) line[k] = line[k] " " line[m]
       printf "%s ",general_sub(subst_param(line[k], paramarray2),name,pathnode,portarray)
       break
     }
     else if(line[k] ~/=/) {
      split(line[k],parameter,"=")
      if(parameter[2] in paramarray2)
       printf "%s ", parameter[1] "=" paramarray2[parameter[2]]
      else if(name SUBSEP "param" SUBSEP parameter[2] in subckt)
       printf "%s ",parameter[1] "=" subckt[name,"param",parameter[2]]
      else printf "%s ",line[k]
     }
     else { # if parameter get actual value
       if(line[k] in paramarray2) line[k] = paramarray2[line[k]]
       else { # try to see if parameter inside quotes or braces 
         m = line[k]
         gsub(/[{}']/, "", m)
         if(m in paramarray2) line[k] = paramarray2[m]
       }
       printf "%s ", line[k]
     }
    }
   } 
   else if(line[1] ~/^\.(SAVE|PRINT|PROBE)/) {
     printf "%s ", general_sub(a[j],name,pathnode,portarray)
   }
   else {
     printf "%s ", a[j]
   }
   printf "\n"
  }
  if(a[j] ~ /^[ \t]*\.endc/) controlblock = 0
 }
}

function subst_param(s, pa,     p, i, ss)
{
  for(p in pa) {
    while(1) {
      i = match(s, "[^a-zA-Z0-9_]" p "[^=a-zA-Z0-9_]") 
      ss = ""
      if(i) {
        ss = ss substr(s, 1, RSTART)
        ss = ss pa[p]
        ss = ss substr(s, RSTART + RLENGTH - 1)
        s = ss
      }
      if(!i) {
        break
      }
    }
  }
  return s
}




function getnode(name, path, portarray, node)
# return the full path-name of <node> in subckt <name> 
# in path <path>, called with ports <portarray>
{
 sub(/ *$/, "", node)
 sub(/^ */, "", node)
 if(name!=topcell)  		# if we are in top cell, nothing to do
 {
  if(name SUBSEP "port" SUBSEP node in subckt)
    return portarray[subckt[name,"port",node]] 	# <node> is a port, 
						#return port mapping
  if(!(node in global)) return path node	# local node
 }
 return node	# if <node> is a top level or global (not a port) just return
}


# expand expressions like: VALUE=3*V(IN)+VA) or I={V(nn,mm) *1e-6}
# substituting node names
function general_sub(string,name,pathnode,portarray,       nod, sss, state, lastc)
{
 state = 0
 while(1) {
   # print "****** " string
   if(state == 0 && match(string, /V\([^(),]*[,)]/)) { # match V(XXX) or V(XXX,
     # print "***** here0"
     lastc = substr(string, RSTART+RLENGTH-1, 1)
     if(lastc == ",") state =1
     nod = substr(string,RSTART+2,RLENGTH-3)
     sss=sss substr(string,1,RSTART-1) "V(" getnode(name,pathnode,portarray,nod) lastc
     string=substr(string,RSTART+RLENGTH)
   } else if(state == 1 && match(string, /[^(),]*[)]/)) { # match YYY) only if state==1
     # print "***** here1"
     nod = substr(string,RSTART,RLENGTH-1)
     sss=sss substr(string,1,RSTART-1) getnode(name,pathnode,portarray,nod) ")"
     string=substr(string,RSTART+RLENGTH)
     state = 0
   } else break
 }
 sss=sss string
 return sss
}

function trim_quoted_spaces(s,                p, m)
{
  p = ""
  while(match(s, /['{][^}']*['}]/)) {
    m = substr(s, RSTART, RLENGTH)
    gsub(/ /, "", m)
    p = p substr(s, 1, RSTART -1) m
    s = substr(s, RSTART + RLENGTH)
  }
  p = p s
  return p
}

