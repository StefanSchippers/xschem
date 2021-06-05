#!/usr/bin/awk -f
#
#  File: make_sym_lcc.awk
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

BEGIN{
  width=150
}

FNR == 1 {
    if (_filename_ != "") endfile(_filename_)
    _filename_ = FILENAME
    beginfile(FILENAME)
}
 
END  { endfile(_filename_) }
 
 
function beginfile(f)
{
 sym=name=f
 sub(/^.*\//,"",name)
 name_ext=name
 sub(/\.sch.*$/,"",name)
 sub(/\.sch.*$/,".sym",sym)
 print "**** symbol-izing: " sym "  ****"
  template="" ; start=0
  while((getline symline <sym) >0) {
   if(symline ~ /^[GK] \{/ ) start=1
   if(start) template=template symline "\n"
   if(symline ~ /\} *$/) start=0
  }
  close(sym)
  
   
 size=2.5
 space=20
 lwidth=20
 textdist=5
 labsize=0.2
 titlesize=0.3
 text_voffset=10
 lab_voffset=4
 ip=op=n_pin=n_p=n_l=0
 print "v {xschem version=2.9.8 file_version=1.2}" > sym
 if(template=="") {
  printf "%s", "K {type=subcircuit\nformat=\"@name @pinlist @symname\"\n"  >sym
  printf "%s\n", "template=\"name=x1\""  >sym
  printf "%s", "}\n"  >sym
 } 
 else print template >sym
}


/^C \{.*generic_pin(\.sym)?\}/{
  get_end_line()
  process_line()
  type_pin[n_pin]=generic_type
  dir_pin[n_pin]="generic"
  x_pin[n_pin] = $3+0 # y coordinate of pin 20140519
  y_pin[n_pin] = $4+0 # y coordinate of pin 20140519
  rotation_pin[n_pin] = $5+0 # y coordinate of pin 20140519
  flip_pin[n_pin] = $6+0 # y coordinate of pin 20140519
  index_pin[n_pin] = n_pin # one level indirection for sorting pins 20140519
  value_pin[n_pin]=value
  label_pin[n_pin] = pin_label
  props_pin[n_pin] = rest_of_props()
  n_pin++
  ip++
}

/^[LP] /{
  get_end_line()
  process_box_line()
  box[n_p]=$0
  n_p++
}

/^C \{.*ipin(\.sym)?\}/{
  get_end_line()
  process_line()
  type_pin[n_pin]=sig_type
  verilog_pin[n_pin]=verilog_type
  dir_pin[n_pin]="ipin"
  x_pin[n_pin] = $3+0 # y coordinate of pin 20140519
  y_pin[n_pin] = $4+0 # y coordinate of pin 20140519
  rotation_pin[n_pin] = $5+0 # y coordinate of pin 20140519
  flip_pin[n_pin] = $6+0 # y coordinate of pin 20140519
  index_pin[n_pin] = n_pin # one level indirection 20140519
  value_pin[n_pin]=value
  label_pin[n_pin] = pin_label
  props_pin[n_pin] = rest_of_props()
  n_pin++
  ip++
}

$0 ~ /^C \{.*opin(\.sym)?\}/ && $0 !~ /^C \{.*iopin(\.sym)?\}/ {
  get_end_line()
  process_line()
  type_pin[n_pin]=sig_type
  verilog_pin[n_pin]=verilog_type
  dir_pin[n_pin]="opin"
  x_pin[n_pin] = $3+0 # y coordinate of pin 20140519
  y_pin[n_pin] = $4+0 # y coordinate of pin 20140519
  rotation_pin[n_pin] = $5+0 # y coordinate of pin 20140519
  flip_pin[n_pin] = $6+0 # y coordinate of pin 20140519
  index_pin[n_pin] = n_pin # one level indirection 20140519
  value_pin[n_pin]=value
  label_pin[n_pin] = pin_label
  props_pin[n_pin] = rest_of_props()
  n_pin++
  op++
}

/^C \{.*iopin(\.sym)?\}/{
  print "iopin"
  get_end_line()
  process_line()
  type_pin[n_pin]=sig_type
  verilog_pin[n_pin]=verilog_type
  dir_pin[n_pin]="iopin"
  x_pin[n_pin] = $3+0 # y coordinate of pin 20140519
  y_pin[n_pin] = $4+0 # y coordinate of pin 20140519
  rotation_pin[n_pin] = $5+0 # y coordinate of pin 20140519
  flip_pin[n_pin] = $6+0 # y coordinate of pin 20140519
  index_pin[n_pin] = n_pin # one level indirection 20140519
  value_pin[n_pin]=value
  label_pin[n_pin] = pin_label
  props_pin[n_pin] = rest_of_props()
  n_pin++
  op++
}

function rest_of_props()
{
  sub(/^C \{[^}]+\}.*\{/,"")
  sub(/\}[ \t]*$/, "")
  sub(/verilog_type[ \t]*=[ \t]*[^ \t]+[ \t]?/, "")
  sub(/sig_type[ \t]*=[ \t]*[^ \t]+[ \t]?/, "")
  sub(/lab[ \t]*=[ \t]*[^ \t]+[ \t]?/, "")
  sub(/value[ \t]*=[ \t]*[^ \t]+[ \t]?/, "")
  sub(/^[ \t]*$/, "")
  return $0
}

function process_line()
{
 print "process_line"
 sig_type=""  #20070726    # "std_logic"
 verilog_type= ""  # 20070726   "wire" #09112003
 pin_label=""
 value=""
 generic_type=""
 
 if($0 ~ /^.*lab=/)
 {
  pin_label=$0
  sub(/^.*lab=/,"",pin_label)
  sub(/[ }].*$/,"",pin_label)
 }

 if($0 ~ /^.*verilog_type=/)  #09112003
 {
  verilog_type=$0
  sub(/^.*verilog_type=/,"",verilog_type)
  sub(/[}].*$/,"",verilog_type)
  sub(/ $/,"",verilog_type)
 }
 
 if($0 ~ /^.*sig_type=/)
 {
  sig_type=$0
  sub(/^.*sig_type=/,"",sig_type)
  sub(/[ }].*$/,"",sig_type)
 }
 
 if($0 ~ /^.*generic_type=/)
 {
  generic_type=$0
  sub(/^.*generic_type=/,"",generic_type)
  sub(/[}].*$/,"",generic_type)
  sub(/[a-zA-Z0-9]+=.*$/,"",generic_type)	#03062002, allow spaces
  print "------------------------" $0 "-->" generic_type
 }

 if($0 ~ /^.*value=/)
 {
  value=$0
  if(value ~ /value="/) 
  {
   sub(/^.*value="/,"",value)
   value= "\"" substr(value,1, match(value, /[^\\]"/)  ) "\""
  }
  else
  {
   sub(/^.*value=/,"",value)
   sub(/[ }].*$/,"",value)
  }
 }
 #print "process_line: returning:" $0
 # print "process_line: pin_label=" pin_label " verilog_type=" verilog_type
}

function process_box_line(    value) # value is also a global. Avoid clashes by declaring it local
{
 print "process_box_line"
 # If sch has dash, sym will not have dash
 if($0 ~ /^.*dash=/) {
  sub(/dash=[1-9]/,"dash=0")
 }
 else {
   value = $0
   if (value ~ /\{.*\}/)
   {
     sub(/^.*\{/,"",value)
     sub(/\}/, "", value) # Get rid of }
     if (length(value)) sub(/\{.*\}/, "{" value " dash=5}") # Put dash=5 within {} along with its original content
     else sub(/\{.*\}/, "{dash=5}")
   }
 }
 print "process_box_line: returning:" $0
 # print "process_line: pin_label=" pin_label " verilog_type=" verilog_type
}

## join lines like this:
## C {ipin.sym} ........ {lab=xxx
## verilog_type=reg}
function get_end_line()
{
  print "get_end_line"
  while($0 !~ /\}[ \t]*$/) {
    a=$0
    getline
    $0 = a " " $0
  }
}

function endfile(f,     a) {

 n=ip;if(op>n) n=op
 if(n==0) n=1
 m=(n-1)/2
 y=-m*space
 x=-width
 
 box_minx=box_maxx=box_miny=box_maxy=0
 for(ii=0;ii<n_p;ii++)
 {
  print box[ii] >sym
  awk split(box[ii],a," ");
  box_type=a[1]
  if (box_type=="P")
  {
    box_num_vertices = a[3]
    for (j=0;j<box_num_vertices*2;j+=2)
    {
      if (box_minx==0 || box_minx > a[4+j]) box_minx=a[4+j]
      if (box_maxx==0 || box_maxx < a[4+j]) box_maxx=a[4+j]
      if (box_miny==0 || box_miny > a[4+j+1]) box_miny=a[4+j+1]
      if (box_maxy==0 || box_maxy < a[4+j+1]) box_maxy=a[4+j+1]
    }
    print "(" box_minx "," box_miny ") and (" box_maxx "," box_maxy ")"
  }
  if (box_type=="L")
  {
    if (box_minx==0 || box_minx > a[3]) box_minx=a[3]
    if (box_maxx==0 || box_maxx < a[3]) box_maxx=a[3]
    if (box_minx==0 || box_minx > a[5]) box_minx=a[5]
    if (box_maxx==0 || box_maxx < a[5]) box_maxx=a[5]
    if (box_miny==0 || box_miny > a[4]) box_miny=a[4]
    if (box_maxy==0 || box_maxy < a[4]) box_maxy=a[4]
    if (box_miny==0 || box_miny > a[6]) box_miny=a[6]
    if (box_maxy==0 || box_maxy < a[6]) box_maxy=a[6]
    #print "(" box_minx "," box_miny ") and (" box_maxx "," box_maxy ")"
  }
 }

 #print "Final: (" box_minx "," box_miny ") and (" box_maxx "," box_maxy ")"

 #print "T {@symname}" ,(box_maxx+box_minx)/2, (box_maxy+box_miny)/2,0,0,
 #      titlesize, titlesize, "{}" >sym
 print "T {@symname}",box_minx-lwidth+5, box_miny-text_voffset,0,0,labsize, labsize,"{}" >sym
 print "T {@name}",box_maxx-lwidth+5, box_miny-text_voffset,0,0,labsize, labsize,"{}" >sym

 for(ii=0;ii<n_pin;ii++)
 {
  i = index_pin[ii] # 20140519
  
  dir=dir_pin[i]
  value=value_pin[i]
  
  sig_type=type_pin[i]
  verilog_type=verilog_pin[i]
  vert = verilog_type ? (" verilog_type=" verilog_type) : ""
  vhdt = sig_type ? (" sig_type=" sig_type) : ""

  if(dir=="generic")
  {
   printf "B 3 " (x_pin[i]-size) " " (y_pin[i]-size) " " (x_pin[i]+size) " " (y_pin[i]+size) \
         " {name=" label_pin[i] " generic_type=" sig_type " " >sym
   if(value !="") printf "value=" value " " >sym
   printf props_pin[i] > sym
   printf "}\n" >sym
   x = get_text_x(label_pin[i], x_pin[i], y_pin[i], box_minx, box_maxx, box_miny, box_maxy)
   y = get_text_y(label_pin[i], x_pin[i], y_pin[i], box_minx, box_maxx, box_miny, box_maxy)
   print "T {" label_pin[i] "}",x,y,0,0,labsize, labsize, "{}" >sym
  }
  if(dir=="ipin")
  {
   printf "B 5 " (x_pin[i]-size) " " (y_pin[i]-size) " " (x_pin[i]+size) " " (y_pin[i]+size) \
         " {name=" label_pin[i] vhdt vert " dir=in " >sym
   if(value !="") printf "value=" value " " >sym
   printf props_pin[i] > sym
   printf "}\n" >sym
   x = get_text_x(label_pin[i], x_pin[i], y_pin[i], box_minx, box_maxx, box_miny, box_maxy)
   y = get_text_y(label_pin[i], x_pin[i], y_pin[i], box_minx, box_maxx, box_miny, box_maxy)
   print "T {" label_pin[i] "}",x,y,0,0,labsize, labsize, "{}" >sym
  }
  if(dir=="opin")
  {
   printf "B 5 " (x_pin[i]-size) " " (y_pin[i]-size) " " (x_pin[i]+size) " " (y_pin[i]+size) \
         " {name=" label_pin[i] vhdt vert " dir=out " >sym
   if(value !="") printf "value=" value " " >sym
   printf props_pin[i] > sym
   printf "}\n" >sym
   x = get_text_x(label_pin[i], x_pin[i], y_pin[i], box_minx, box_maxx, box_miny, box_maxy)
   y = get_text_y(label_pin[i], x_pin[i], y_pin[i], box_minx, box_maxx, box_miny, box_maxy)
   print "T {" label_pin[i] "}",x,y,0,0,labsize, labsize, "{}" >sym
  }
  if(dir=="iopin")
  {
   printf "B 5 " (x_pin[i]-size) " " (y_pin[i]-size) " " (x_pin[i]+size) " " (y_pin[i]+size) \
         " {name=" label_pin[i] vhdt vert " dir=inout " >sym
   if(value !="") printf "value=" value " " >sym
   printf props_pin[i] > sym
   printf "}\n" >sym
   x = get_text_x(label_pin[i], x_pin[i], y_pin[i], box_minx, box_maxx, box_miny, box_maxy)
   y = get_text_y(label_pin[i], x_pin[i], y_pin[i], box_minx, box_maxx, box_miny, box_maxy)
   print "T {" label_pin[i] "}",x,y,0,0,labsize, labsize, "{}" >sym
  }
 }
 close(sym)
}
 
function get_text_x(str, x, y, box_minx, box_maxx, box_miny, box_maxy)
{
  len = length(str)
  if (x == box_minx) # On the left hand side
  {
    return(x+lwidth+textdist)
  }
  if (x == box_maxx) # On the right hand side
  {
    return(x-len-lwidth-textdist)
  }
  # In between left and right
  return(x)
}

function get_text_y(str, x, y, box_minx, box_maxx, box_miny, box_maxy)
{
  if (y == box_miny) # On the top line
  {
    return(y+lwidth+textdist)
  }
  if (y == box_maxy) # On the bottom line
  {
    return(y-lwidth-textdist)
  }
  return(y-lab_voffset)
}
