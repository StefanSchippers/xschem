#!/usr/bin/awk -f
#
#  File: verilog.awk
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


BEGIN{
 net_types["wire"]=1
 net_types["tri"]=1
 net_types["wor"]=1
 net_types["trior"]=1
 net_types["wand"]=1
 net_types["triand"]=1
 net_types["tri0"]=1
 net_types["tri1"]=1
 net_types["supply1"]=1
 net_types["supply0"]=1
 net_types["trireg"]=1
 net_types["reg"]=1
 net_types["integer"]=1
 net_types["time"]=1
 net_types["real"]=1
 net_types["logic"]=1
 net_types["bool"]=1
 direction["input"]=1
 direction["inout"]=1
 direction["output"]=1
}

/^---- start primitive/{primitive_line=""; primitive_mult=$4;primitive=1; next}
/^---- end primitive/{
  primitive=0
  $0 = primitive_line
  gsub(/----pin\(/, " ----pin(",$0)
  gsub(/----name\(/, " ----name(",$0)
  for(j=1;j<= primitive_mult; j++) {
    prefix=""
    # print $0 > "/dev/stderr"
    for(i=1;i<=NF;i++) {
      if($i ~/^#[a-zA-Z_]+#$/) {
         prefix=$i
         sub(/^#/,"", prefix)
         sub(/#$/,"", prefix)
         continue
      }
      prim_field=$i
      if($i ~ /^----pin\(.*\)/) {
        sub(/----pin\(/,"",prim_field)
        sub(/\)$/,"",prim_field)
        pport_mult = split(prim_field, prim_field_array,/,/)
    
        # 20060919 BEGIN
        # if bussed port connected to primitive and primitive mult==1 print only basename of bus
        if(primitive_mult==1 && pport_mult>1) {
          if(check2(prim_field_array, pport_mult)) 
           printf "%s[%s:%s] ", prefix s_b(prim_field_array[1]), 
              s_i(prim_field_array[1]), s_i(prim_field_array[pport_mult]) 
          else {
            printf " { "
            for(s=1;s<= pport_mult; s++) {
              printf "%s", prefix prim_field_array[s]
              if(s<pport_mult) printf ","
            }
            printf "} "
         }
        } 
        else 
        # 20060919 end

        printf "%s ", prim_field_array[1+(j-1) % pport_mult]   #  20140401 1+(j-1) % pport_mult instead of j
      }
      else if($i ~ /^----name\(.*\)/) {
        sub(/----name\(/,"",prim_field)
        sub(/\)$/,"",prim_field)
        sub(/:/, "_slot_", prim_field)
        split(prim_field, prim_field_array,/,/)
        sub(/\[/,"_", prim_field_array[j])
        sub(/\]/,"", prim_field_array[j])
        printf "%s ", prefix prim_field_array[j]
      }
      else  printf "%s ", prim_field
      prefix=""
    } # end for i
    printf "\n"
  } # end for j
  next
}

primitive==1{primitive_line=primitive_line " " $0; next  }

# print signals/regs/variables
/---- end signal list/{
 for(i in signal_basename) {
  n=signal_basename[i]
  split(signal_index[i],tmp)
  hsort(tmp,n)
  printf "%s ", signal_type[i]

  # 20161118
  ntypes = split(signal_type[i], sigtype_arr) 
  if(ntypes==2) {
    if( i in signal_delay) printf "%s ", signal_delay[i]
    printf "%s", i
    if(signal_index[i] !~ /no_index/)
    {
     if(tmp[1] ~ /:/) printf "[%s] ",tmp[1]
     else if(n==1) printf "[%s:%s] ",tmp[1], tmp[1]
     else printf "[%s:%s] ",tmp[1], tmp[n]
    }
    if(i in signal_value) printf " = %s ", signal_value[i]
    printf " ;\n"
  } else {
    if( i in signal_delay) printf "%s ", signal_delay[i]
    if(signal_index[i] !~ /no_index/) 
    {
     if(tmp[1] ~ /:/) printf "[%s] ",tmp[1]
     else if(n==1) printf "[%s:%s] ",tmp[1], tmp[1]
     else printf "[%s:%s] ",tmp[1], tmp[n]
    }
    printf "%s ", i
    if(i in signal_value) printf " = %s ", signal_value[i]
    printf " ;\n"
  }
 }
 # /20161118
 
 siglist=0;
 print ""
 netlist=1
 next
}

# store signals
siglist==1 && ($1 in net_types) {

# 20070525 recognize "reg real" types and similar 
 if($2 in net_types) {
   if($3 ~ /^#/) basename=s_b($4)
   else basename=s_b($3)
   signal_type[basename]=$1 " " $2
   if($3 ~ /^#/) {
    signal_delay[basename]=$3
    $3=""; $0=$0;
   }
   if($4=="="){
    val=$0
    sub(/.*=/,"",val)
    sub(/;.*/,"",val)
    signal_value[basename]=val
   }
   signal_basename[basename]++
   if($3 ~ /\[.*\]/) {
    signal_index[basename]=signal_index[basename] " " s_i($3)
   }
   else signal_index[basename]="no_index"

 }
# /20070525

 else {
   if($2 ~ /^#/) basename=s_b($3)
   else basename=s_b($2)
   signal_type[basename]=$1
   if($2 ~ /^#/) {
    signal_delay[basename]=$2
    $2=""; $0=$0;
   }
   if($3=="="){
    val=$0
    sub(/.*=/,"",val)
    sub(/;.*/,"",val) 
    signal_value[basename]=val
   }
   signal_basename[basename]++
   if($2 ~ /\[.*\]/) {
    signal_index[basename]=signal_index[basename] " " s_i($2)
   }
   else signal_index[basename]="no_index"
 }
 next
}
 
/---- begin signal list/{
 siglist=1;
 next
}

# print module ports
begin_module==1 {
 $1=s_b($1)
}

# new module declaration
NF==3 && $3=="(" && $1=="module" {
 begin_module=1
 delete signal_basename
 delete signal_index
 delete signal_value
 delete signal_type
 delete signal_delay
}

begin_module && $1 ~/^\);$/ {
 begin_module=0
}
 netlist==0 && $0 ~ /^ *parameter +[^ ]+ += +/{ $4=get_number($4) }

/^---- instance / {
  on_single_line()
## ---- instance xsl[0],xsl[1],xsl[2],xsl[3],xsl[4],xsl[5],xsl[6],xsl[7] ( ?32 I[0:31] DEL[0],DEL[1] ...
  num=split($3,name,",")
  for(j=7;j<=NF-1;j+=3)
  {
    arg_num[j]=split($j,tmp,",")
    for(k=1;k<=arg_num[j]; k++) {
     arg_name[j,k]=tmp[k]
    }
  }
  for(i=1;i<=num;i++)
  {
   gsub(/[\[]/,"_",name[i])
   gsub(/[\]]/,"",name[i])
   printf "%s", previous  # print module name (and params)
   printf "%s ( ", name[i]

   for(j=5;j<=NF-1;j++)
   {
    pin=""
    if(j>7) {printf ","}
    if($j !~ /^\?-?[0-9]+$/)
    {
      pin=pin $j # if not a node just print it
    }
    else
    {
     nmult=$(j++);j++

     sub(/\?/,"",nmult)
     nmult=nmult+0
     if(nmult==-1) nmult=arg_num[j]
     for(l=0;l<nmult;l++)
     {
      ii=(l+nmult*(i-1))%arg_num[j]+1
      if(pin!="") pin=pin ","
      pin=pin  arg_name[j,ii]
     }
    }
    if(nmult==1) printf "\n .%s( %s )" ,s_b($(j-1)),pin
    else {
     split(pin,pin_array,",")
     basename=s_b(pin_array[1])
     if(check2(pin_array,nmult) && net_ascending==0) {  ## 20140416 if ascending nets print single bits
      if(nmult==signal_basename[basename] )
        printf "\n .%s( %s )", s_b($(j-1)),basename
      else
        printf "\n .%s( %s[%s:%s] )",s_b($(j-1)),basename,s_i(pin_array[1]),s_i(pin_array[nmult])
     }
     else printf "\n .%s( %s )", s_b($(j-1)), (nmult>1? "{" pin "}" : pin)
    }
   }
   printf "\n);\n\n"
  }
 previous=""
 next 
}

# types of in/out/inout ports of module
netlist==0 && architecture==0 && ($1 in net_types) && ($2 in net_types) {
 if($3 ~ /\[.*\]/) {$3= s_b($3)  "[" s_i($3) "] "; $1="  " $1 }
}

netlist==0 && architecture==0 && ($1 in net_types) && !($2 in net_types) {
 if($2 ~ /\[.*\]/) {$2= "[" s_i($2) "] " s_b($2);  $1="  " $1 }
}

# module port directions
NF==3 && netlist==0 && architecture==0 && ($1 in direction)  {
 if($2 ~ /\[.*\]/) {$2= "[" s_i($2) "] " s_b($2);  $1="  " $1 } ## 20080412 print range also in port decl as req by standard
 #### $2= s_b($2); $1="  " $1
}

/^---- begin user architecture code/ {
 netlist=0
 architecture=1
 next
}

/^---- end user architecture code/ {
 architecture=0
 next
}

/^---- start parameters/{
 parameters=1
 next
}

/^---- end parameters/{ 
 parameters=0
 next
}


$NF=="," && parameters==1 {
  if($1 ~ /^[+-]?(([0-9]+\.?)|([0-9]*\.[0-9]+))(([aAfFpPnNuUgGtT])|([mM][eE]][gG]))/){
    $1="   " get_number($1)
  }
}

# parameters assignments on instance line
/ *\.[a-zA-Z_0-9]+ \( [^ ]+ \)/ && parameters==1{ $3=get_number($3) }
/ *\.[a-zA-Z_0-9]+\( [^ ]+ \)/ && parameters==1{ $2=get_number($2) }

netlist==1 {
 previous=previous   $0 "\n"
 next
}



{
 print
}


function get_number(n)   # follows SPICE conventions
{
 if(n !~ /^[-.0-9]/) return n   # not a number
 n=toupper(n)
 if(n ~ /A/) return n*1e-18
 if(n ~ /F/) return n*1e-15
 if(n ~ /P/) return n*1e-12
 if(n ~ /N/) return n*1e-9
 if(n ~ /U/) return n*1e-6
 if(n ~ /M[^E][^G]/) return n*1e-3
 if(n ~ /K/) return n*1e3
 if(n ~ /MEG/) return n*1e6
 if(n ~ /G/) return n*1e9
 if(n ~ /T/) return n*1e12
 return n+0.0 # force a numeric ret. value
}


# heap sort, this is a N*log2(N) routine
function hsort(ra,n,    l,j,ir,i,rra)
{
 if(n<=1) return    # bug fix: if only one element in array never returns
 l=int(n/2)+1
 ir=n
 for(;;) {
  if(l>1) {
   rra=ra[--l]
  }
  else {
   rra=ra[ir]
   ra[ir]=ra[1]
   if(--ir==1) {
    ra[1]=rra
    return
   }
  }
  i=l
  j=l*2
  while(j<=ir) {
   if(j<ir && ra[j] >ra[j+1]) ++j
   if(rra>ra[j]) {
    ra[i]=ra[j]
    j+=(i=j)
   }
   else j=ir+1
  }
  ra[i]=rra
 }
}
# check if an array of indexes (sig[3]) arr[1],arr[2]..... 
# is contigous and decreeasing
# 20140409 handle ascending order as well as descending
function check2(arr,n     ,decreasing,a,name,i,start,ok)
{
 decreasing=""
 net_ascending=0  # 20140416 must be global var !
 name=s_b(arr[1])
 start=s_i(arr[1])
 if(arr[1] !~ /[0-9]+/)
 {
  if(n>1) return 0
  else return 1
 }
 for(i=2;i<=n;i++)
 {
  if(s_b(arr[i]) != name) return 0
  if(arr[1] !~ /[0-9]+/) return 0
  a= s_i(arr[i])+0
  if(a+1 == start)
  {
   if(decreasing=="") {
     start=a
     decreasing=1
   } else if(decreasing==1) {
     start=a
   } else {
     return 0
   }
  }
  else if(a-1 == start)
  {
   if(decreasing=="") {
     start=a
     decreasing=0
   } else if(decreasing==0) {
     start=a
   } else {
     return 0
   }
  }
  else
  {
   return 0
  }
 }

 if(decreasing==0) net_ascending=1  # net_ascending is global 20140416
 else net_ascending=0               # net_ascending is global 20140416
 return 1
}

# check if an array of numbers arr[1],arr[2]..... is contigous and decreeasing
function check(arr,n      ,i,start,ok)
{
 start=arr[1]
 for(i=2;i<=n;i++)
 {
  if(arr[i]+1 == start)
  {
   start=arr[i]
  }
  else
  {
   return 0
  }
 }
 return 1
}

function s_b(n)
{
 sub(/\[.*/,"",n)
 return n;
}

function s_i(n)
{
 if(n ~ /\[/)  {
  sub(/.*\[/,"",n)
  sub(/\]/,"",n)
  return n
 } else return ""
}

function cleanup()
{
 gsub(/\);/," );")
}

function on_single_line(     i,a,count)
{
 cleanup()
 count=0
 i=1
 a=""
 while(1) {
  if(i>NF) { getline;cleanup();i=1 }
  if(count) a=a " "
  a=a $i
  count++
  if($i==");") break
  i++
 }
 $0= a
}

