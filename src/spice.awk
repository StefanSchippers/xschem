#!/usr/bin/awk -f 
#
#  File: spice.awk
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
 lines = 0;
 first=1
 user_code=0 #20180129

 # used to handle strange primitives that have a type word before the instance name
 special_devs["ymemristor"] = 1
 special_devs["ylin"] = 1
 special_devs["ydelay"] = 1
 special_devs["ytransline"] = 1
 special_devs["ypgbr"] = 1
 special_devs["ypowergridbranch"] = 1
 special_devs["yacc"] = 1
 special_devs[".model"] = 1
 special_devs[".subckt"] = 1

 while( (ARGV[1] ~ /^[-]/) || (ARGV[1] ~ /^$/) ) {
   if(ARGV[1] == "-xyce") { xyce = 1} 
   for(i=2; i<= ARGC;i++) {
     ARGV[i-1] = ARGV[i]
   }
   ARGC--
 }
}

# join split lines
{

  if($0 ~ /^\*\*\*\* begin user (architecture|header) code/) {
     user_code = 1
  }
  if($0 ~ /^[+]/ && !user_code) {
    yy = yy " " substr($0,2)
    next
  }
  else {
    zz = yy
    yy = $0
    $0 = zz
    if(first) { 
      first=0
      next
    }
    line[lines++] = $0
  }
  if($0 ~ /^\*\*\*\* end user (architecture|header) code/) {
    user_code = 0
  }
}

END{
  user_code = 0
  $0=yy
  line[lines++] = $0

  ## resolve parametric instance name vector multiplicity
  substitute_instance_param()
  
  for(i=0; i<lines; i++) {
    $0 = line[i]

    ## /place to insert processing awk hooks
    if(xyce == 1) {
      ## transform ".save" lines into ".print tran" *only* for spice_probe elements, not user code
      if(tolower($0) ~/^[ \t]*\.save[ \t]+.*\?-?[0-9]+/) {   # .save file=test1.raw format=raw v( ?1 C2  )
        $1 = ""
        $0 = ".print " $0
      } 
      gsub(/ [mM] *= *1 *$/,"") # xyce does not like m=# fields (multiplicity) removing m=1 is no an issue anyway
    }
    process()
  }
}

## if .param m=10 n=5 is defined and R1[m:n] instance name is present in netlist --> R1[10],R1[9],...,R1[5]
## before further processing netlist.
function substitute_instance_param(    i, j, name, first, last)
{
  IGNORECASE=1
  for(i = 0;i < lines; i++) {
    $0 = line[i]
    if($0 ~/^\.param/) {
      gsub(/ *= */, "=")
      for(j = 2; j <= NF; j++) {
        param = value = $j
        sub(/=.*/, "", param)
        sub(/.*=/,"", value)
        par[param] = value
        # print "*** " param " = " value
      }
    }
  }
  for(i = 0;i < lines; i++) {
    $0 = line[i]
    if($1 ~ /^[a-zA-Z][^\[\]:]+\[[a-zA-Z_][a-zA-Z0-9_]*:[a-zA-Z_][a-zA-Z0-9_]*\]$/) {
      name = first = last = $1
      sub(/\[.*/, "", name)
      sub(/^.*\[/, "", first)
      sub(/:.*/,"", first)
      sub(/^.*:/,"", last)
      sub(/\]/,"", last)
      if(first in par) first = par[first]
      if(last in par) last = par[last]

      n=""
      for(j= first;;) {
        if(j!=first) n = n ","
        n = n  name "[" j "]"
        if(j == last) break
        j+= sign(last - first)
      }
      # print "*** " n
      $1 = n
      line[i] = $0
    }
  }
  IGNORECASE=0
}


function sign(x) 
{
  return x<0 ? -1 : x>0 ? 1 : 0
}

function process(        i,j, iprefix, saveinstr, savetype, saveanalysis)
{

  if($0 ~/\*\*\*\* end_element/){
    spiceprefix=""
    return
  }
  if($0 ~/\*\*\*\* spice_prefix/){
    spiceprefix=$3
    return
  }
  if($0 ~/\*\*\*\* begin user header code/){ #20180129
    user_code=1
    return
  }
  if($0 ~/\*\*\*\* begin user architecture code/){ #20180129
    user_code=1
    print
    return
  }
  if($0 ~/\*\*\*\* end user architecture code/){ #20180129
    user_code=0
    print
    return
  }
  if($0 ~/\*\*\*\* end user header code/){ #20180129
    user_code=0
    return
  }
  if(user_code) { #20180129
    print
    return
  }

 # 20181208 do not process commented lines
 if($1 ~/^\*/) {
   print
   return
 }

 #20150922 handle the case of pmos elements that netlist also a diode
 # format="@name @pinlist @model w=@w l=@l geomod=0 m=@m
 # #dx#name 0 @@b dnwell area=... pj=..."
 # replace #dx#name with dx prefix added on all @name elements 
 # example:
 # m6[3] GWL WLDECF[3] WL[3] HDD phv w=50u l=10u geomod=0 m=1 
 # m6[2] GWL WLDECF[2] WL[2] HDD phv w=50u l=10u geomod=0 m=1 
 # m6[1] GWL WLDECF[1] WL[1] HDD phv w=50u l=10u geomod=0 m=1 
 # m6[0] GWL WLDECF[0] WL[0] HDD phv w=50u l=10u geomod=0 m=1 
 # dxm6[3] 0 HDD dnwell area='(50u + 73u)*(10u + 32u)' pj='2*(50u +73u)+2*(10u +32u)' 
 # dxm6[2] 0 HDD dnwell area='(50u + 73u)*(10u + 32u)' pj='2*(50u +73u)+2*(10u +32u)' 
 # dxm6[1] 0 HDD dnwell area='(50u + 73u)*(10u + 32u)' pj='2*(50u +73u)+2*(10u +32u)' 
 # dxm6[0] 0 HDD dnwell area='(50u + 73u)*(10u + 32u)' pj='2*(50u +73u)+2*(10u +32u)' 
 #20151027 do this for all fields
 for(i=1; i<=NF;i++) {
   
   if($i ~/^##[a-zA-Z_]+/) {
     sub(/^##/, "", $i)
   } else 
   if($i ~/^#[a-zA-Z_0-9]+#[a-zA-Z_]+/) {
     iprefix=$i
     sub(/^#/,"",iprefix)
     sub(/#.*/,"",iprefix)
     sub("#" iprefix "#", iprefix,$i)
     gsub(/,/, "," iprefix,$i)
     ## 20160301 add '?1' if missing in format string
     if(i>1 && ( $(i-1) !~/^\?/) )  {
       $i = "?1 " $i
     } 
     $0 = $0  # reparse input line 
   }
 }
 ## 20140506 do not transform {} of variation groups
 ## nmos N {
 ## ...
 ## }
 gsub(/PARAM:/,"")     # stefan 20110627

 if($0 ~/^[gG]/) {
   IGNORECASE=1
   sub(/ value=/," cur=")
   IGNORECASE=0
 }
 if($0 ~/^[eE]/) {
   IGNORECASE=1
   sub(/ value=/," vol=")
   IGNORECASE=0
 }
 if($0 ~/^[rR]/) {
   IGNORECASE=1
   sub(/ value=/," r=")
   IGNORECASE=0
 }
 if($0 ~/^[cC]/) {
   IGNORECASE=1
   sub(/ value=/," c=")
   IGNORECASE=0
 }
 ### ?? too dangerous 
 # gsub(/ value=/," ")
 # gsub(/ VALUE=/," ")

 if($0 ~ /^D/ ) sub(/PERI[ \t]*=/,"PJ=")

 ## .save tran v(?1 GB ) v(?1 SB )
 ## ? may be followed by -1 in some cases
 if(tolower($1) ~ /^\.(save|print)$/ && $0 ~/\?-?[0-9]/) {
   $0 = tolower($0)
   saveinstr = $1
   if(!xyce) {
     $1=""
     $0 = $0 # reparse line for field splitting
  
     gsub(/ *\?-?[0-9]+ */, "")   # in some cases ?-1 is printed (unknow multiplicity) 
     gsub(/\( */, "(")
     gsub(/ *\)/, ")")
     for(i=1; i<=NF; i++) {
       savetype=$i; sub(/\(.*/,"", savetype)  # v(...)  --> v
       sub(/^.*\(/,"", $i)
       sub(/\).*/,"", $i)
       num = split($i, name, ",")
       for(j=1; j<= num; j++) {
         print saveinstr " " savetype "(" name[j] ")"
       }
     }
   }
   
 } else if( $1 ~ /^\*\.(ipin|opin|iopin)/ ) {
   num=split($2,name,",")
   for(i=1;i<=num;i++) print $1 " " name[i]
 } else if(  tolower($1) ~ /\.subckt/) {
  # remove m=.. from subcircuit definition since m= is a multiplier not a param
  sub(/ m=[0-9]+/," ",$0)
  gsub(","," ",$0)
  print $0
 } else {
  # handle uncommon primitives that have a prefix before the device name
  if(tolower($1) in special_devs) { 
    devprefix = $1
    num = split($3, name, ",")
    $1 = ""
    for(i = 0; i < num; i++) {
      if(i) $1 = $1 ","
      $1 = $1 devprefix
    }
    num = split($1, name, ",")
    $0 = $0
  } else {
    num = split($1, name, ",")
  }
  if(num==0) print ""

 
  for(j=2;j<=NF;j+=1)  		# start from 2 not from 3 20070221
  {
    #       ............ --> matches ?n and ?-n         
    if($j ~/^\?-?[0-9]+$/) continue	# handle the case that $2 not pinlist  20070221
    arg_num[j]=split($j,tmp,",")
    for(k=1;k<=arg_num[j]; k++) {
     arg_name[j,k]=tmp[k]
    }
  }
  for(i=1;i<=num;i++)
  {
   printf "%s ", spiceprefix name[i]

   for(j=2;j<=NF;j++)
   {
    #         ............ --> matches ?n and ?-n
    if($j !~ /^\?-?[0-9]+$/)
    {
      printf "%s ", $j # if not a node just print it
    }
    else
    {
     nmult=$(j++)
     sub(/\?/,"",nmult)
     if(nmult+0==-1) nmult=arg_num[j]
     for(l=0;l<nmult+0;l++)
     {
      ii=(l+nmult*(i-1))%arg_num[j]+1
      printf "%s ", arg_name[j,ii]
     }
    }
   }
   printf "\n"
  }
 }
}

