#!/usr/bin/awk -f 
#
#  File: tedax.awk
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
## handle options
# if( ARGV[1] == "-hspice" ) {
#   hspice=1
#   for(i=2; i<= ARGC;i++) {
#     ARGV[i-1] = ARGV[i]
#   }
#   ARGC--
# }
}

##1       2     3        4    5           6
##        inst  net      pin  pinnumber   pinindex
# conn    U1:2  INPUT_A  A    1:4:9:12    1

/^[ \t]*#/{ print; next}
/^begin netlist/{ 
  print
  next
}

/^begin_inst .* numslots/{ 
  reparse()
  numslots=$4; 
  if(numslots ==0) numslots = 1
  next
}
$1=="device"||$1=="footprint"{
  reparse()
  arg1=$1; arg2=$2; $1=$2=""; $0=$0
  gsub(/^[ \t]*/,"")
  gsub(/[\\]? +/,"\\ ")
  $0=arg1 " " arg2 " " $0
  current_name=arg2
}
/^footprint/{
  if(NF>=3) {
    reparse()
    fp=""
    nn=split($2, inst_arr, ":")
    inst_name=inst_arr[1]
    for(i=3; i<=NF;i++) {
      fp = ((i==3) ? $i : fp " " $i)
    }
    footprint[inst_name] = fp
  }
  next
}

/^device/{
  if(NF>=3) {
    reparse()
    dev=""
    nn=split($2, inst_arr, ":")
    inst_name=inst_arr[1]
    for(i=3; i<=NF;i++) {
      dev = ((i==3) ? $i : dev " " $i)
    }
    device[inst_name] = dev
  }
  next
}
  
# conn U1 Tx2In Tx2\ In 10 10
/^conn/{
  reparse()
  nn=split($2, inst_arr, ":")
  inst_name=inst_arr[1]
  slot = (nn==2) ? inst_arr[2]+0 : 1
  nn = split($5, pinlist_arr, ":")
  pin_number= (nn>1) ? pinlist_arr[slot] : pinlist_arr[1]
  pin_index = $6
  pin_name = $4
  net_name=$3
  slotted=nn>1? 1:0
  # even for unused slots initialize the arr[] array.
  # conn lines need not to be printed but pinslot and pinidx lines do.
  for(i=1; i<=numslots;i++) {
    curr_pin = (nn>1) ? pinlist_arr[i]: pinlist_arr[1]
    if(!((inst_name, curr_pin) in arr) || arr[inst_name, curr_pin]=="" || arr[inst_name, curr_pin] ~/^--UNCONN--/) {
      if(curr_pin == pin_number) {
        arr[inst_name, curr_pin]=net_name SUBSEP pin_index SUBSEP pin_name SUBSEP i SUBSEP slotted
      } else {
        arr[inst_name, curr_pin]="--UNCONN--" SUBSEP pin_index SUBSEP pin_name SUBSEP i SUBSEP slotted
      }
    # hidden connections (VCC, VSS on slotted devices, usually) specified on instance have higher
    # precedence w.r.t. default specified in symbol.
    } else if($0 ~ /# instance_based/ && curr_pin==pin_number) {
      # overwrite with instance specified net name.
      arr[inst_name, curr_pin]=net_name SUBSEP pin_index SUBSEP pin_name SUBSEP i SUBSEP slotted
    }
  }
  next
}

/^(__subcircuit__|__map__|tEDAx v1|__GLOBAL__|__HIERSEP__)/ {
  print
  next
}

/^end_inst/{
  next
}

/^end netlist/{
  for(i in arr) { 
    if(arr[i]) {
      split(i, i_arr, SUBSEP)
      split(arr[i], n_arr, SUBSEP)
      slotted = n_arr[5]+0
      # print "slotted=" slotted ">> " $0 > "/dev/stderr"
      if(n_arr[1] !~/--UNCONN--/) print "conn", n_arr[1], i_arr[1], i_arr[2]
      if(slotted) {
        print "pinslot", i_arr[1], i_arr[2], n_arr[4]
        print "pinidx", i_arr[1], i_arr[2], n_arr[2]
      }
      print "pinname", i_arr[1], i_arr[2], n_arr[3]
    }
  }
  for(i in footprint) print "footprint", i, footprint[i]
  for(i in device) print "device", i, device[i]
  print $0
  delete arr
  delete footprint
  delete device
  next
}

$1 ~ /^(comptag|spicedev|spiceval|value)$/ {
  if($1 == "comptag") for(i = 3; i<=NF; i+=2) print "comptag", $2, $i, $(i+1)
  else if(NF > 2) print $0
  next
}

# avoid considering escaped spaces as field separators
function reparse(    i)
{
  gsub(/\\ /, SUBSEP "s")
  gsub(/\\\t/, SUBSEP "t")
  for(i = 1; i <= NF; i++) {
    gsub(SUBSEP "s", "\\ ", $i)
    gsub(SUBSEP "t", "\\\t", $i)
  }
}
