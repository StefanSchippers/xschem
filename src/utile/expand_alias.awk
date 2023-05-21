#!/usr/bin/awk -f
# File: expand_alias.awk
# 
# This file is part of XSCHEM,
# a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
# simulation.
# Copyright (C) 1998-2023 Stefan Frederik Schippers
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

# expand alias calls
# expand nested aliases too
#		Stefan Schippers, 04-08-1999
# 02-09-1999: - bug fixes
# 25-09-1999: - ability to process include directives
# 10/01/2000  - Removed explicit pathnames. this is no more an awk script but a
#               sh script, which in turn runs awk.
# 02/02/2000  - better output precision using OFMT global var.
# 20110630    - if expressions present within aliases, recognize and substitute parameters
#
# sample input:
#
#  alias ciclo_we par1 par2
#   
#  set apad 0
#  set dqpad 0
#  s .1
#  set apad par1
#  set dqpad par2
#  set wenpad 0
#  s .1
#  set apad 0
#  set dqpad 0
#  set wenpad 1
#  s .1
#   
#  endalias
# 
#  ciclo_we aa 55
#  ciclo_we 55 aa




BEGIN{
 OFMT="%.9g" # better precision
 beginalias=0
}

{
  process_line()
}


function process_line()
{
 if($1 ~ /^alias$/)
 {
  beginalias=1
  name=$2
  line=0
  alias[name]=NF-2
  for(i=3;i<=NF;i++)
  {
   alias[name , "p" $i]=i-2
  }
 }
 if(beginalias)
 {
  alias[name , "l" line++]=$0
  if($1 ~ /^endalias *$/)
  {
   alias[name , "lines"]=line-1 
   beginalias=0;
  }
  return
 }
 if($1 in alias) {
  print "dump # <<<alias>>>   " $0
  expand_alias($0)
 }
 else print $0
} 


# 20150718 generic replacer of {param} patterns in macro lines
# example: set vcc '{param}? 1.2 : {vhigh}'
function replace(name, s,       pre, par, post)
{
  while(s ~ /{[^{]+}/) {
    pre=s
    par=s
    post=s
    sub(/{.*/,"",pre)
    sub(/^[^}]*}/,"",post)
    sub(/^[^{]*{/, "", par)
    sub(/}.*/,"",par)
    # 20170828  param[] indexed with name to avoid clashes in case of multiple alias levels
    if(name SUBSEP "p" par  in alias) par = param[name, alias[name , "p" par]]
    else par = SUBSEP "%"   par "%" SUBSEP
    s = pre par post
  }
  gsub(SUBSEP "%", "{", s)
  gsub("%" SUBSEP , "}", s)
  return s
}

# 20110630 
function analyze_params_in_expressions(name, s,       ss, s_arr, i, o, o_arr, n)
{
  s=replace(name, s)  #20150918 new {par} format to avoid ambiguities in complex patterns
    
  ss=""
  n = split(s, s_arr, /[=\-\/\+\*]/)   # 20150810 added '=' for substituting 'set sig 0 slope=slp' with slp=param
  o = split(s, o_arr, /[^=\-\+\*\/]+/) # 20150810 added '='
  for(i=1; i<=n;i++) {
    invert=0 # 20140527
    if(s_arr[i] ~ /~/) { # 20140527
      sub(/^~/,"",s_arr[i])
      invert=1
    }
    if(name SUBSEP "p" s_arr[i]  in alias) {
      # 20170828 all param[] calls indexed with alias name to avoid clashes
      # print ";    --> " s_arr[i]  " -->|" alias[name , "p" s_arr[i]] "|--> " param[name, alias[name , "p" s_arr[i]]] > "/dev/stderr"
      s_arr[i]=param[name, alias[name , "p" s_arr[i]]]
    }
    if(invert) s_arr[i] = "~" s_arr[i] # 20140527
    ss = ss o_arr[i] s_arr[i]
  }
  return ss
}


function expand_alias(line,     numb,name,i,mm,j,tmp,zz)
{
  numb=split(line,tmp)
  name=tmp[1]
  #print "; expand: " name >"/dev/stderr"
  for(i=2;i<=numb;i++) {
    # 20170828 all param[] calls indexed with alias name to avoid clashes
    param[name, i-1]=tmp[i]
    #print "; param[" i-1 "]= " param[name, i-1] > "/dev/stderr"
  }
  for(i=1;i<alias[name , "lines"];i++)
  {
   mm=split(alias[name , "l" i],tmp)
   zz=""
   for(j=1;j<=mm;j++)
   {
    # 20170828 all param[] calls indexed with alias name to avoid clashes
    if(name SUBSEP "p" tmp[j]  in alias) tmp[j]=param[name, alias[name , "p" tmp[j]]]
    tmp[j] = analyze_params_in_expressions(name, tmp[j])
    zz=zz  " " tmp[j]
   }
   zz=substr(zz,2)  # remove leading blank
   if(tmp[1] in alias)
   {
    print "dump # <<<alias>>>   " alias[name , "l" i]
    expand_alias(zz)
   }
   else
    print zz
  }
}
