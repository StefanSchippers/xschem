#!/bin/sh
# creates an edif port declaration netlist from a vhdl file
# usage: make_edif.awk file.vhd
# resulting netlist will be written in file.edif
#
# Warning:
#	can be used on vhdl entities that have std_logic/std_logic_vector
#       pins, any other types not recognized
#	Also, types and ranges must NOT be referenced in packages or 
#	as constants/variables, if this is the case use the design_analyzer
#	"EDIF save as"  facility
#
# stefan, 23052002

awk '
BEGIN{

# get cell name
  cell=ARGV[1]
  sub(/\..*/,"",cell)
  sub(/.*\//,"",cell)
  outfile = cell ".edif"

# write header
  print "(edif Synopsys_edif (edifVersion 2 0 0) (edifLevel 0)" >outfile
  print " (library DESIGNS (edifLevel 0)" >outfile
  print "  (cell " cell " (cellType GENERIC)" >outfile
  print "   (view Netlist_representation (viewType NETLIST)" >outfile
  print "    (interface" >outfile
}

# do some clean up
{
  $0=toupper($0)
  sub(":"," : ",$0)
  sub(/;.*/,"",$0)
  sub("\("," ( ",$0)
  sub("\)"," ) ",$0)
}

# parse only entity declaration
($1=="ENTITY" && $2==cell && $3== "IS") { entity=1 }
($1=="END" && $2==cell) { entity=0 }

# print port list
($0 ~ /^[ \t]*[A-Z0-9_]+[ \t]*:[ \t]* (IN|OUT|INOUT)[ \t]+STD_LOGIC/ && entity){
  if($0 ~ /STD_LOGIC_VECTOR/) {
    if($6 < $8) print "Warning, ascending bus: " $0
    for( i=$6; i>=$8 ; i-- ) {
     print "     (port " $1 "_E2V_" i " (direction "  dir($3)  "))" > outfile
    }
  }
  else {
    print "     (port " $1 " (direction "  dir($3)  "))" > outfile
  }

}

# write trailer
END{
  print "    )" >outfile
  print "   )" >outfile
  print "  )" >outfile
  print " )" >outfile
  print ")" >outfile
}

function dir(d) {
  if(d=="IN") return "INPUT"
  else if(d=="OUT") return "OUTPUT"
  else return "INOUT"
}

' $@
