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
 pathsep="_"
 nodes["M"]=4; nodes["R"]=2; nodes["D"]=2; nodes["V"]=2
 nodes["I"]=2; nodes["C"]=2; nodes["L"]=2; nodes["Q"]=3
 nodes["E"]=4; nodes["G"]=4; nodes["H"]=2; nodes["F"]=2
}

{
 if( ($0 !~/^\.include/) && ($0 !~/^\.INCLUDE/) ) $0=toupper($0)
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
 for(j=subckt[name , "first"]+1;j<subckt[name , "last"];j++)
 {
  num=split(a[j],line)
  if(line[1] ~ /^X/ )
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
   if(line[1] ~ /^[EGHFCMDQRGIV]/)
   {
    printf "%s ",line[1] pathname
    for(k=2;k<=nodes[substr(line[1],1,1)]+1;k++)
     printf "%s ", getnode(name,pathnode,portarray,line[k])
    for(; k<=num;k++)
    {
     if(line[1] ~ /[FH]/ && k==4)  printf "%s ", line[k] pathname
     else if(line[k] ~/.*VALUE=.*/)
       printf "%s ",general_sub(line[k],name,pathnode,portarray)
     else if(line[k] ~/=/)
     {
      split(line[k],parameter,"=")
      if(parameter[2] in paramarray2)
       printf "%s ", parameter[1] "=" paramarray2[parameter[2]]
      else if(name SUBSEP "param" SUBSEP parameter[2] in subckt)
       printf "%s ",parameter[1] "=" subckt[name,"param",parameter[2]]
      else printf "%s ",line[k]
     }
     else  printf "%s ", line[k]
    }
   } 
   else
    printf "%s ", a[j]
   printf "\n"
  }
 }
}






function getnode(name, path, portarray, node)
# return the full path-name of <node> in subckt <name> 
# in path <path>, called with ports <portarray>
{
 if(name!=topcell)  		# if we are in top cell, nothing to do
 {
  if(name SUBSEP "port" SUBSEP node in subckt)
    return portarray[subckt[name,"port",node]] 	# <node> is a port, 
						#return port mapping
  if(!(node in global)) return path node	# local node
 }
 return node	# if <node> is a top level or global (not a port) just return
}


# expand expressions like: VALUE=3*V(IN)+VA)
# substituting node names
function general_sub(string,name,pathnode,portarray,       nod,sss)
{
 while(match(string, /V\([^\(\)]*\)/ ))
 {
  nod = substr(string,RSTART+2,RLENGTH-3)
  sss=sss substr(string,1,RSTART-1) "V(" \
   getnode(name,pathnode,portarray,nod) ")"
  string=substr(string,RSTART+RLENGTH)
 }
 sss=sss string
 return sss
}                                      
