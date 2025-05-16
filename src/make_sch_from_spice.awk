#!/usr/bin/awk -f
#
#  File: make_sch_from_spice.awk
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
#
# create xschem symbols  and schematics from a spice netlist.
# Spice netlist should contain *.PININFO lines that specify the direction
# of ports otherwise inout is assumed for all symbol ports.
# Usage:
#   make_sch_from_spice.awk netlist.spice
# Generated symbols  and schematics should be in current directory

BEGIN{
  avoid_brakets=0 ## set to 1 if you want to avoid [] in inst names: x12[3] --> x12_3_
                  ## this is needed for VHDl since [] are not allowed in inst names

  error_missing=0 ## flag to terminate or continue in case of missing subckt definitions in netlist
                  ## 20161118

 while ( "ls $HOME/.xschem/xschem_library/xschem_sky130/sky130_stdcells/*.sym"|getline sym )
 {
   insert_symbol(sym, "sky130_stdcells")
 } 
 

 # while ( "ls $HOME/share/xschem/xschem_library/devices/*.sym"|getline sym )
 # {
 #   insert_symbol(sym, "devices")
 # }


  devices_prefix = "devices/" #for sky130
  # devices_prefix = ""

  inherited_pin["VGND"]=1
  inherited_pin["VPWR"]=1
  inherited_pin["VNB"]=1
  inherited_pin["VPB"]=1
 
  skip_symbol_prefix= "sky130_fd_sc_hd__"
  # sym_type = "subcircuit"
  sym_type = "primitive" # do not use schematics although they will be generated

  all_signals=""

##########################   JOIN   ##########################
  netlist_lines=0
  first=1
  while(err= (getline l) >0) {
   gsub(/</,"[",l)
    gsub(/>/,"]",l)
    if(first) {
      $0=l
      first=0
    }
    else if(l !~/^\+/) {
      netlist[netlist_lines++]=$0
      $0=l
    }
    else $0 = $0 " " substr(l,2)
  }
  netlist[netlist_lines++]=$0
########################## END JOIN ##########################

  skip=0
  for(i=0;i<netlist_lines; i++) { 
    $0=netlist[i]
    process_subckts()
  }

  skip=0
  for(i=0;i<netlist_lines; i++) { 
    $0=netlist[i]
    process()
  }
}

function translate(cellname) 
{
 if     (cellname=="nmos") return "enbsim3"
 else if(cellname=="pmos") return "epbsim3"
 else return cellname
}


function insert_symbol(sym, lib,          n,cellname, name, dir, tmp)
{
  n=0
  tmp=sym
  sub(/.*\//,"", tmp)
  sub(/\..*/,"", tmp)
  cellname=translate(tmp)
  cell_filename[cellname] = tmp
  cell_library[cellname]=lib
  cell[cellname]=1
  print " inserting |" cellname "|"
  while( getline <sym)
  {

   if($0 ~ /^B 5 /)
   {
    name=$7
    sub(/.*=/,"",name)
    dir=$0
    sub(/.*dir[ \t]*=/,"",dir)
    sub(/[ \t]+.*\}.*/,"",dir)
    dir= dir=="in" ? "I" : (dir=="out" ? "O" : "B")
    pin_ar[cellname, ++n] = name
    pin_x[cellname,name]=($3+$5)/2
    pin_y[cellname,name]=($4+$6)/2
    pin_ar[cellname,"dir",name] = dir
   }
  }
  pin_ar[cellname,"n"] = n
}


function process_subckts(         j, i,name)
{
 if(skip==1 && toupper($1) ==".ENDS") { skip=0; return } 
 if(skip==1) return
 if(toupper($1) ==".SUBCKT") {
   curr_subckt=$2
   sub(skip_symbol_prefix, "", curr_subckt)
   pin_ar[curr_subckt, "name"] = $2

   print " process_subckt(): curr_subckt=|" curr_subckt "|"
   if(curr_subckt in cell) {print " process_subckt(); skipping " curr_subckt ; skip=1; return }
   subckt[curr_subckt]=1
   template=0
   j = 1
   for(i=3;i<=NF;i++) {
     if($i~ /=/) { template=i; break}
     if(!($i in inherited_pin)) {
       pin_ar[curr_subckt,j]=$i
       pin_ar[curr_subckt,"format"]=pin_ar[curr_subckt,"format"] " @@" $i
       j++
     } else {
       if(pin_ar[curr_subckt,"extra"]) 
         pin_ar[curr_subckt,"extra"] = pin_ar[curr_subckt,"extra"] " " $i
       else
         pin_ar[curr_subckt,"extra"] = $i
       pin_ar[curr_subckt,"template"] = pin_ar[curr_subckt,"template"] " " $i "=" $i
       pin_ar[curr_subckt,"format"]=pin_ar[curr_subckt,"format"] " @" $i
     }
   }

   pin_ar[curr_subckt,"n"]=j-1
   if(skip_symbol_prefix)
     pin_ar[curr_subckt,"template"] = pin_ar[curr_subckt,"template"] " prefix=" skip_symbol_prefix
   get_template(template) 
   if(skip_symbol_prefix)
     pin_ar[curr_subckt,"extra"] = pin_ar[curr_subckt,"extra"] " prefix"
   print "\n\n\n process_subckt() : " curr_subckt "--> " 
   for(i=1; i<= pin_ar[curr_subckt,"n"]; i++) printf "%s ", pin_ar[curr_subckt,i]; printf "\n"
 }
 else if($1 ~ /^\*\.PININFO/) {
   for(i=2;i<=NF;i++) {
     name=$i; sub(/:.*/,"",name)
     if($i ~ /:I$/ ) pin_ar[curr_subckt, "dir", name] = "I"
     else if($i ~ /:O$/ ) pin_ar[curr_subckt, "dir", name] = "O"
     else if($i ~ /:B$/ ) pin_ar[curr_subckt, "dir", name] = "B"
     else { print "ERROR in .PININFO: ", $i " --> " $0 ; exit} 
   }
 }
 else if(toupper($1) ~ /^\*\.(I|O|IO)PIN/) {
  if($1 ~ /^\*\.ipin/) { pin_ar[curr_subckt, "dir", $2] = "I" } 
  else if($1 ~ /^\*\.opin/) { pin_ar[curr_subckt, "dir", $2] = "O" } 
  else if($1 ~ /^\*\.iopin/) { pin_ar[curr_subckt, "dir", $2] = "B" } 
 }
}

function get_template(t,         templ, i) 
{
 templ=""
 if(t) for(i=t;i<=NF;i++) {
  templ = templ $i " " 
 }
 pin_ar[curr_subckt,"template"] = pin_ar[curr_subckt,"template"] " " templ
}


   
function process(         i,name,param)
{
 print "process(): skip = "  skip " --> " $0
 if(skip==1 && toupper($1) ==".ENDS") { skip=0; return }
 if(skip==1) return
 if(toupper($1) ==".SUBCKT") {
   curr_subckt=$2
   sub(skip_symbol_prefix, "", curr_subckt)
   if(curr_subckt in cell) {print "process(): skipping " curr_subckt ; skip=1; return }
   space=20
   width=150
   lwidth=20
   textdist=5
   labsize=0.2
   titlesize=0.3
   text_voffset=20
   xoffset=1300
   yoffset=0
   prev_size=0
   lab_voffset=4
   sp=ip=op=n_pin=0
   all_signals=""
   delete net_ar
   subckt_netlist = ""  # 20111009 all netlist lines except component instances (X) 

 }
 else if(toupper($1) ~ /^.ENDS/) {


   compact_pinlist( "" , curr_subckt)
   print "----------------------------------------------------------"
     
   for(i=1;i<= dir_ret["n"] ; i++) {
     print dir_ret[i] "   " pin_ret[i]
   }   
   print "\n\n"
         
   print_sch(curr_subckt, dir_ret, pin_ret)
   print_sym(curr_subckt, pin_ar[curr_subckt,"template"], \
     pin_ar[curr_subckt,"format"], pin_ar[curr_subckt,"name"], \
     sym_type, pin_ar[curr_subckt,"extra"], dir_ret, pin_ret)
   print "----------------------------------------------------------"


   if(all_signals !="") {
     print all_signals > (curr_subckt ".sch")
   }
   close((curr_subckt ".sch"))
 }
 else if(toupper($1) ~ /^X/) {

   ## 20111009 no need to remove 1st char, this was done on older netlist having double 'X'
   # inst=substr($1,2)
   inst = $1
   ## /20111009

   ## 20161225
   if(avoid_brakets) {
     sub(/\[/,"_",inst)
     sub(/\]/,"_",inst)
   }

   param=0
   for(i=2;i<=NF;i++) {
     sub(/!$/,"",$i)   # remove ! on global nodes
     if(i<NF && $(i+1) ~ /=/) {
       if(!param) param = i+1
       inst_sub=$i
       if(error_missing && !(inst_sub in cell ) && !(inst_sub in subckt)) {
         print "ERROR: " inst_sub " NOT DECLARED, curr_subckt=", curr_subckt
         exit
       }
       break
     }
     else if($i =="/" ) {
       if(i==NF) {print "ERROR: garbled netlist line : " $0; exit}
       inst_sub=$(i+1)
       if(!param) param = i+2
       if(error_missing && !(inst_sub in cell ) && !(inst_sub in subckt)) {
         print "ERROR: " inst_sub " NOT DECLARED, curr_subckt=", curr_subckt
         exit
       }
       break
     }
     else if(i==NF) {
       inst_sub=$i
       if(error_missing && !(inst_sub in cell ) && !(inst_sub in subckt)) {
         print "ERROR: " inst_sub " NOT DECLARED, curr_subckt=", curr_subckt
         exit
       }
       break
     }
     net_ar[inst,i-1] = $i
     #print "i-1=" i-1 " net_ar[inst,i-1]=" net_ar[inst,i-1]
   }
   net_ar[inst,"n"] = i-2
   #print "  net_ar[inst,n]= "  net_ar[inst,"n"]
   compact_pinlist(inst,inst_sub)
   #print inst " - " inst_sub  " --> "
   for(i=1;i<= dir_ret["n"] ; i++) {
     #print "  dir_ret " i " ------> " dir_ret[i] "   " pin_ret[i] " <-- " net_ret[i]
   }
   #print "\n\n"
   param = get_param(param) 
   print_signals( inst, inst_sub, param, pin_ret, dir_ret, net_ret )
 }
 else { # other components, M, R, C, D, .... 20111009
  if($1 !~ /(\.PININFO)|(^\.)/ ) {
    subckt_netlist = subckt_netlist $0 "\n"
  }
 }
}

function get_param(i,               param,j)
{
  param=""
  if(i) for(j=i;j<=NF;j++) {
    param = param $j " "
  }
  return param
}

function compact_pinlist(inst,inst_sub                  , prevgroup, group,i,ii,base,curr,curr_n,np)
{
 delete pin_ret
 delete net_ret
 delete dir_ret
 
 np=pin_ar[inst_sub,"n"]
 # print " compact_pinlist: inst=" inst " np= " np " inst_sub=" inst_sub
 if(np) {
   ii=1
   for(i=1;i<=np;i++) {
     group=0
     # print "compact_pinlist: i=" i " inst_sub=" inst_sub " pin_ar[inst_sub,i]=" pin_ar[inst_sub,i]
     # print "compact_pinlist: net_ar[inst,i]=" net_ar[inst,i]
     if(net_ar[inst,i] ~/,/) group=1

       base =lab_name( pin_ar[inst_sub,i] )
       if(i==1) {prevgroup=group; curr=base; curr_n=i}
       else { 
         if(prevgroup || base != curr) {
           if(prevgroup) pin_ret[ii] = pin_ar[inst_sub,i-1]
           else pin_ret[ii] = compact_label(inst_sub,pin_ar,curr_n,i-1)
           if(inst) {
             if(prevgroup) net_ret[ii] = compact_label_str(net_ar[inst,i-1])
             else net_ret[ii] = compact_label(inst,net_ar,curr_n,i-1)
           }
           dir_ret[ii] = pin_ar[inst_sub,"dir",pin_ar[inst_sub,i-1] ]
           if(dir_ret[ii] == "") dir_ret[ii] = "B"
           ii++
           curr=base;curr_n=i
           prevgroup=group
         }
       }
     }
   }
   pin_ret[ii] = compact_label(inst_sub, pin_ar,curr_n,np)
   if(inst) {
     if(group) net_ret[ii] = compact_label_str(net_ar[inst, np])
     else      net_ret[ii] = compact_label(inst, net_ar,curr_n,np)
   }

   dir_ret[ii] = pin_ar[inst_sub,"dir",pin_ar[inst_sub,np] ]
   if(dir_ret[ii] == "") dir_ret[ii] = "B"
   pin_ret["n"] =  dir_ret["n"] = ii
   if(inst) net_ret["n"] = ii
}
# 1  2    3    4    5 6 7     8    9    10   11   12
# PP A[3] A[2] A[1] B C K[10] K[9] K[5] K[4] K[3] K[1]
function compact_label_str(str,        a, b, ar, ret,start,i)
{
  # print "compact_label_str(): str=" str
  a=1
  b=split(str, ar,",")
  ret=""
  for(i=a;i<=b;i++) {
    if(i==a) {start=a}
    else {
      if(ar[i-1] !~ /\[/)  {
        if(ar[i-1] != ar[i]) {
          if(start < i-1) { ret = ret (i-start) "*" ar[i-1] ","; start=i }
          else {ret = ret ar[i-1] ","; start=i }
        }
      }
      else if(lab_name(ar[i])!=lab_name(ar[i-1])) {                     # lab basename changed
        if(start<i-1 && lab_index(ar[start]) == lab_index(ar[i-1]) )
          ret = ret (i-start) "*" ar[i-1] ",";
        else if(start<i-1)
          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) ":" lab_index(ar[i-1]) "],"
        else
          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) "],"
        start=i
      }

      else if( lab_index(ar[i]) != lab_index(ar[i-1])-1 &&                # index not equal, +1,-1
               lab_index(ar[i]) != lab_index(ar[i-1])+1 ) {               # to previous
        if(start<i-1 && lab_index(ar[start]) == lab_index(ar[i-1]) )
          ret = ret (i-start) "*" ar[i-1] ",";
        else if(start<i-1)
          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) ":" lab_index(ar[i-1]) "],"
        else
          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) "],"
        start=i
      }

      else if( lab_index(ar[start])!=lab_index(ar[i]) &&           # range count != element count
               abs(start-i)!=abs(lab_index(ar[start])-lab_index(ar[i]))) { 
        if(start<i-1 && lab_index(ar[start]) == lab_index(ar[i-1]) )
          ret = ret (i-start) "*" ar[i-1] ",";
        else if(start<i-1)
          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) ":" lab_index(ar[i-1]) "],"
        else
          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) "],"
        start=i
      }

#      else if( lab_index(ar[i]) != lab_index(ar[start])  ) {
#        if(start<i-1 && lab_index(ar[start]) == lab_index(ar[i-1]) )
#          ret = ret (i-start) "*" ar[i-1] ",";
#        else if(start<i-1)
#          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) ":" lab_index(ar[i-1]) "],"
#        else
#          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) "],"
#        start=i
#      }


    }
  }
  if(ar[b] !~ /\[/)  {
    if(start < b)  ret = ret (b-start+1) "*" ar[b]
    else ret = ret ar[b]
  }
  else if(start<b && lab_index(ar[start]) == lab_index(ar[b]))
    ret = ret (b-start+1) "*" ar[b]
  else if(start<b)
    ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) ":" lab_index(ar[b]) "]"
  else
    ret = ret lab_name(ar[b]) "[" lab_index(ar[b]) "]"

  # print "compact_label_str(): ret=" ret
  return ret
}

# 1  2    3    4    5 6 7     8    9    10   11   12
# PP A[3] A[2] A[1] B C K[10] K[9] K[5] K[4] K[3] K[1]
function compact_label(name, ar,a,b,        ret,start,i)
{
  ret=""
  for(i=a;i<=b;i++) {
    # print "compact_label(): ar[" name ", " i "]=" ar[name,i]
    if(i==a) {start=a}
    else {
      if(ar[name,i-1] !~ /\[/)  {
        if(ar[name,i-1] != ar[name,i]) {
          if(start < i-1) { ret = ret (i-start) "*" ar[name,i-1] ","; start=i }
          else {ret = ret ar[name,i-1] ","; start=i }
        }
      }
      else if(lab_name(ar[name,i])!=lab_name(ar[name,i-1])) {
        if(start<i-1 && lab_index(ar[name,start]) == lab_index(ar[name,i-1]) )
          ret = ret (i-start) "*" ar[name,i-1] ",";
        else if(start<i-1) 
          ret = ret lab_name(ar[name,start]) "[" lab_index(ar[name,start]) ":" lab_index(ar[name,i-1]) "],"
        else
          ret = ret lab_name(ar[name,start]) "[" lab_index(ar[name,start]) "],"
        start=i
      }
      else if(lab_index(ar[name,i]) != lab_index(ar[name,i-1])-1 &&
              lab_index(ar[name,i]) != lab_index(ar[name,i-1])+1  ) {             
        if(start<i-1 && lab_index(ar[name,start]) == lab_index(ar[name,i-1]) )
          ret = ret (i-start) "*" ar[name,i-1] ",";
        else if(start<i-1)
          ret = ret lab_name(ar[name,start]) "[" lab_index(ar[name,start]) ":" lab_index(ar[name,i-1]) "],"
        else
          ret = ret lab_name(ar[name,start]) "[" lab_index(ar[name,start]) "],"
        start=i
      }
      else if( lab_index(ar[name,start])!=lab_index(ar[name,i]) &&
               abs(start-i)!=abs(lab_index(ar[name,start])-lab_index(ar[name,i])) ) {
        if(start<i-1 && lab_index(ar[name,start]) == lab_index(ar[name,i-1]) )
          ret = ret (i-start) "*" ar[name,i-1] ",";
        else if(start<i-1)
          ret = ret lab_name(ar[name,start]) "[" lab_index(ar[name,start]) ":" lab_index(ar[name,i-1]) "],"
        else
          ret = ret lab_name(ar[name,start]) "[" lab_index(ar[name,start]) "],"
        start=i
      }
    }
  }
  if(ar[name,b] !~ /\[/)  {
    if(start < b)  ret = ret (b-start+1) "*" ar[name,b]
    else ret = ret ar[name,b]
  }
  else if(start<b && lab_index(ar[name,start]) == lab_index(ar[name,b]))   
    ret = ret (b-start+1) "*" ar[name,b] 
  else if(start<b)   
    ret = ret lab_name(ar[name,start]) "[" lab_index(ar[name,start]) ":" lab_index(ar[name,b]) "]"
  else
    ret = ret lab_name(ar[name,b]) "[" lab_index(ar[name,b]) "]"

  # print "compact_label(): ret=" ret
  return ret
}

function lab_name(lab)
{
 sub(/\[.*/,"",lab)
 return lab
}


function lab_index(lab)
{
 sub(/.*\[/,"",lab)
 sub(/\].*/,"",lab)
 if( (lab ~/:/) || (lab ~/,/) ) return lab # 20161225
 else return lab+0
}

function print_sch(schname, dir, pin,
           n_pin, ip, op,        iii,ooo,y,x,i, pin_dir,sch_x_offset)
{
 schname = schname ".sch"
 iii=0
 ooo=0
 print " --> print_sch called for: " schname
 print "V {}\nG {}\nS {" escape_brackets(subckt_netlist) "}" > schname # 20111009 added subckt_components
 n_pin = dir["n"]

 ip=op=0
 for(i=1;i<=n_pin; i++) {
   if(dir[i] == "I" ) ip++
   else if(dir[i]=="O" || dir[i]=="B") op++
   else {print "ERROR: print_sch(): undefined dir[i], i=",i, "schname=", schname, "pin[i]=", pin[i]; exit}
 }
  
 y=0
 x=-40
 sch_x_offset=230
 
 for(i=1;i<=n_pin;i++)
 {
  pin_dir=dir[i]

  if(pin_dir=="I")
  {
   iii++
   printf "C {" devices_prefix "ipin.sym} " (x+sch_x_offset) " " (y+iii*space) " 0 0 " \
         " {name=p" p_pin++ " lab=" pin[i] " " >schname
   printf "}\n" >schname
  }
  if(pin_dir=="O")
  {
   ooo++
   printf "C {" devices_prefix "opin.sym} " (-x+sch_x_offset) " " (y+ooo*space) " 0 0 " \
         " {name=p" p_pin++ " lab=" pin[i] " " >schname
   printf "}\n" >schname
  }
  if(pin_dir=="B")
  {
   ooo++
   printf "C {" devices_prefix "iopin.sym} " (-x+sch_x_offset) " " (y+ooo*space) " 0 0 " \
         " {name=p" p_pin++ " lab=" pin[i] " " >schname
   printf "}\n" >schname
  }
 }
}



function print_signals( inst_name, component_name, param, pin,dir,net,
         n_pin,n_dir,n_net,
	 ip, op, n,m,y,x,i, inum, onum,curr_dir)  #local vars
{

 n_pin=pin["n"]
 n_net=net["n"]
 n_dir=dir["n"]
 
 print " print_signals() : component_name = ", component_name
 if(n_dir != n_pin) { print " n_dir vs n_pin mismatch: inst / comp = " inst_name " / " component_name ; exit }
 if(n_net != n_pin) { print " n_net vs n_pin mismatch: inst / comp = " inst_name " / " component_name ; exit }

 ip=op=0 
 for(i=1; i<=n_net; i++) {
   if(dir[i] ~ /[OB]/) {
     op++
   }
   else if(dir[i] ~ /I/) ip++
   else {print "ERROR: print_sch(): undefined dir[]   i=" i " inst=" inst " sub=" component_name ; exit}
 }

 n=ip;if(op>n) n=op
 if(n==0) n=4
 m=(n-1)/2
 y=-m*space
 x=-width

 if(yoffset >= 1400)
 {
   yoffset=prev_size=0
   xoffset+=920
 } 
 yoffset += ((prev_size+n)/2+1)*space

 inum =onum=0
 for(i=1;i<=n_net;i++)
 {
   curr_dir=dir[i]
   idx=(component_name SUBSEP pin[i])
   if( idx in pin_x)
   {
     #print "print_signals() : " idx " found in library" 
     xpin=xoffset+pin_x[idx]
     ypin=yoffset+pin_y[idx]
   }
   else
   {
     #print "print_signals() : " idx " NOT found in library" 
     if(curr_dir=="O" || curr_dir=="B") {
       xpin=-x+xoffset
       ypin=y+onum*space+yoffset
       onum++
     }
     else {
       xpin=x+xoffset 
       ypin=y+inum*space+yoffset
       inum++
     }
   }
  
   if(curr_dir=="I")
   {
     all_signals = all_signals   "C {" devices_prefix "lab_pin.sym} " xpin " " ypin " 0 0 " \
           " {name=p" p_pin++ " lab=" net[i] " " 
     all_signals = all_signals   "}\n" 
   }
  
   if(curr_dir=="O")
   {
     all_signals = all_signals   "C {" devices_prefix "lab_pin.sym} " xpin " " ypin " 0 1 " \
           " {name=p" p_pin++ " lab=" net[i] " " 
     all_signals = all_signals   "}\n" 
   }
  
   if(curr_dir=="B")
   {
     all_signals = all_signals   "C {" devices_prefix "lab_pin.sym} " xpin " " ypin " 0 1 " \
           " {name=p" p_pin++ " lab=" net[i] " " 
     all_signals = all_signals   "}\n" 
   }
 }

 #  C {micro2/pump_logic} 4700 0 0 0 {name=x1}
 cur_path=ENVIRON["PWD"]
 sub(/^.*\//,"",cur_path)
 if(idx in pin_x)
  all_signals=all_signals "C {" cell_library[component_name]  "/" cell_filename[component_name] \
     "} " xoffset  " " yoffset " 0 0 {name=" inst_name " " param "}\n"
 else
  all_signals=all_signals "C {" cur_path "/" component_name \
     "} " xoffset  " " yoffset " 0 0 {name=" inst_name " " param "}\n"

 prev_size=n
 if( idx in pin_x) prev_size+=8
}  



#------------------------------

function print_sym(sym, template, format, subckt_name, sym_type, extra, dir, pin,
		size,space,width,lwidth,textdist,labsize,titlesize,
		i,name,text_voffset,lab_voffset,ip,op,n_pin ,m,x,y,n,
		iii,ooo) 
{
 size=2.5
 space=20
 width=150
 lwidth=20
 textdist=5
 labsize=0.2
 titlesize=0.3
 text_voffset=20
 lab_voffset=4
 ip=op=n_pin=0

 name=sym
 sym = sym ".sym"

#  G {type=subcircuit
#  vhdl_stop=true
#  verilog_stop=true;
#  format="@name @pinlist @symname"
#  template="name=x1"}


 print "start print symbol: " sym
 print "v {xschem version=3.4.8RC file_version=1.2}" > sym
 print "K {type=" sym_type > sym
 # print "format=\"@name @pinlist @symname " format_translate(template)  "\"" > sym
 iii = format_translate(template, extra)
 if(iii) iii = " " iii

 # since awk strings use backslash escapes and sub also uses backslash escapes (example for \& substitution) 
 # there are 2 levels of escape substitutions, we need \\\\ to generate one \.
 # in the xschem file \\\\ is reduced to \\ in the format string and finally format contains one \ 
 if(skip_symbol_prefix) sub(skip_symbol_prefix, "@prefix\\\\\\\\\\\\\\\\", subckt_name)
 print "format=\"@name" format " " subckt_name iii  "\"" > sym
 print "template=\"name=x1" template "\"" > sym
 print "extra=\"" extra "\"}" > sym
 # print "T {@symname}" ,-length(name)/2*titlesize*30, -text_voffset*titlesize,0,0,
 #       titlesize, titlesize, "{}" >sym
 print "T {@symname}" ,0, -text_voffset*titlesize,0,0,
       titlesize, titlesize, "{hcenter=true}" >sym
 
 n_pin=pin["n"]
 ip=op=0 
 for(i=1; i<=n_pin; i++) {
   if(dir[i] ~ /[OB]/) {
     op++  
   } 
   else if(dir[i] ~ /I/) ip++
   else {print "ERROR: print_sym(): undefined dir[]   i=" i " inst=" inst " sub=" component_name ; exit}
 } 

 


 n=ip;if(op>n) n=op
 if(n==0) n=1
 m=(n-1)/2
 y=-m*space
 x=-width
 print "T {@name}",-x-lwidth+5, y-space/2-8-lab_voffset,0,0,labsize, labsize,"{}" >sym
 print "L 4 " (x+lwidth) ,y-space/2,(-x-lwidth) , y-space/2,"{}" >sym
 print "L 4 " (x+lwidth) ,y+n*space-space/2,(-x-lwidth) , y+n*space-space/2,"{}" >sym
 print "L 4 " (x+lwidth) ,y-space/2,(x+lwidth) , y+n*space-space/2,"{}" >sym
 print "L 4 " (-x-lwidth) ,y-space/2,(-x-lwidth) , y+n*space-space/2,"{}" >sym
 iii=ooo=0
 for(i=1;i<=n_pin;i++)
 {
  pin_dir=dir[i]

  if(pin_dir=="I")
  {
   printf "B 5 " (x-size) " " (y+iii*space-size) " " (x+size) " " (y+iii*space+size) \
         " {name=" pin[i] " dir=in " >sym
   printf "}\n" >sym
   print "L 4 " x,y+iii*space,x+lwidth, y+iii*space,"{}" >sym
   print "T {" pin[i] "}",x+lwidth+textdist,y+iii*space-lab_voffset,0,0,labsize, labsize, "{}" >sym
   iii++
  }
  if(pin_dir=="O")
  {
   printf "B 5 " (-x-size) " " (y+ooo*space-size) " " (-x+size) " " (y+ooo*space+size) \
         " {name=" pin[i] " dir=out " >sym
   printf "}\n" >sym
   print "L 4 " (-x-lwidth),(y+ooo*space),(-x), (y+ooo*space),"{}" >sym
   print "T {" pin[i] "}",-x-lwidth-textdist,y+ooo*space-lab_voffset,0,1,labsize, labsize, "{}" >sym
   ooo++
  }
  if(pin_dir=="B")
  {
   printf "B 5 " (-x-size) " " (y+ooo*space-size) " " (-x+size) " " (y+ooo*space+size) \
         " {name=" pin[i] " dir=inout " >sym
   printf "}\n" >sym
   print "L 7 " (-x-lwidth),(y+ooo*space),(-x), (y+ooo*space),"{}" >sym
   print "T {" pin[i] "}",-x-lwidth-textdist,y+ooo*space-lab_voffset,0,1,labsize, labsize, "{}" >sym
   ooo++
  }
 }
 close(sym)
}
 

#------------------------------

function escape_brackets(s       , ss)
{
  ss = s
  gsub(/[{}]/, "\\\\&", ss)
  return ss
}

function abs(a) 
{
 return a>0 ? a: -a
}

function format_translate(s, extra,            n_extra, extra_arr, extra_hash, c,quote,str,n,i,ss,sss) 
{

 # 20140321
 str=""
 quote=0
 for(i=1; i< length(s); i++) {
   c = substr(s,i,1)
   if(c == "\"") quote=!quote
   if(quote && c==" ") str = str SUBSEP
   else str = str c
 }
 s = str
 # /20140321

 str=""
 n_extra = split(extra, extra_arr)
 for(i = 1; i <= n_extra; i++) extra_hash[ extra_arr[i] ] = 1
 

 n=split(s,ss)
 for(i=1;i<=n;i++) {
   gsub(SUBSEP," ", ss[i])
   print "subckt params: " ss[i]
   if(ss[i] ~ /[^=]+=[^=]+/) {
     split(ss[i],sss,"=") 
     if(!(sss[1] in extra_hash)) ss[i] = sss[1] "=@" sss[1]
     else ss[i] = ""
   }
   str = str ss[i]
   if(i<n && ss[i] ) str=str " " 
 }
 delete extra_hash
 return str
}

