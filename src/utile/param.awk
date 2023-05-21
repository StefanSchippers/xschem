#!/usr/bin/awk -f
# File: param.awk
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

# 20200212 added simple expression parsing

# 20170830 
BEGIN{
 for(i=0;i<=127;i++)
 {
  val[sprintf("%c",i)]=i  # value of char: val["0"]=45
  char[i]=sprintf("%c",i) # char[48]="0"
 }
}

{
  gsub (/[ \t]*=[ \t]*/, "=")
}
  

/^\.*param/{
 param[$2]=arith($3)
 next
}

{
  for(nf=2;nf<=NF;nf++)
  {
    # do not process comments
    if($nf ~ /^;/) break
    # do not expand double quoted expressions
    if($nf ~/^".*"$/) continue

    $nf = replace($nf)  #20150918

    if($nf ~/=/) {
     x1=$nf; sub(/=.*/,"=", x1)
     x2=$nf; sub(/^.*=/,"",x2)
     x2 = arith(x2)
     $nf = x1 x2
    }
    else {
     $nf = arith($nf)
    }  
  }
}   


 #20171117 ascii string --> hex
{
 if($3 ~/^%/){
   hexstring=""
   asciistring=$0
   #print "before: " asciistring > "/dev/stderr"
   sub(/^[^%]+%/,"",asciistring)
   sub(/%.*/,"",asciistring)
   for(i=1; i<=length(asciistring); i++) {
     hexstring = hexstring sprintf("%02x", val[substr(asciistring, i, 1)])
   }
   sub(/%[^%]+%/, hexstring)
   #print $0 , "|" asciistring "|" > "/dev/stderr"
 }
}


{ print }



# 20150718 generic replacer of {param} patterns 
# example: set vcc '{param}? 1.2 : {vhigh}'
function replace(s,       pre, par, post)
{
  while(s ~ /{[^{]+}/) {
    pre=s
    par=s
    post=s
    sub(/{.*/,"",pre)
    sub(/^[^}]*}/,"",post)
    sub(/^[^{]*{/, "", par)
    sub(/}.*/,"",par)
    if(par  in param) par = param[par]
    else par = SUBSEP "%"   par "%" SUBSEP
    s = pre par post
  }
  gsub(SUBSEP "%", "{", s)
  gsub("%" SUBSEP , "}", s)
  return s
}

function arith(s,      op, n, ss, j, valid_expr, arith_operators)
{
 sss="" # 20140625
 valid_expr=1
 arith_operators=0
 n = split(s,ss,/[-+*\/]/)
 op=s
 gsub(/[^-+*\/]/,"",op)

 for(j=1;j<=n;j++) {

  if(ss[j] in param) {
    ss[j] = param[ss[j]]
  }
  if(isnumber(ss[j])==0) {valid_expr=0}
  if(j==1) {
    lab = ss[j]
    sss = ss[j]  # 20140625 reassemble string in case not valid expr
  }
  else {
    if(substr(op, j-1,1)=="+") {lab+=ss[j]; arith_operators=1}
    if(substr(op, j-1,1)=="-") {lab-=ss[j]; arith_operators=1}
    if(substr(op, j-1,1)=="*") {lab*=ss[j]; arith_operators=1}
    if(substr(op, j-1,1)=="/") {lab/=ss[j]; arith_operators=1}
    sss = sss substr(op, j-1,1) ss[j]  # 20140625 reassemble string in case not valid expr
  }
 }
 if(!valid_expr) return sss
 # if the result of an arithmetic evaluation is 1 return 1.0 
 # otherwise stimuli.awk will translate it to the high logic value
 else if(arith_operators && lab==1) return "1.0"
 else return lab
}

function isnumber(n)
{
  if(n ~ /^[+-]?(([0-9]+\.?)|([0-9]*\.[0-9]+))([eE][+-]?[0-9]+)?$/) return 1
  else return 0
}


