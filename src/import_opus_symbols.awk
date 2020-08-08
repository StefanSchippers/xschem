#!/usr/bin/awk -f
# import a opus generated list of symbols (cell_pinouts) 
# (generated with the cell_binding.il skill) 
# and outputs a tcl source file.
# this source file can be sourced by the xschem tcl interpreter 
# to generate schematics and symbols
# IMPORTANT: set the target library in library variable !!!!
#
# stefan 20090305

# ### input file format
# cell fullchip port CSNPAD : input
# cell fullchip port CKCPAD : input

BEGIN{
 library = "lpddr2_opus"
 print "set create_sym_interactive 0"
}

/^cell/ {
  cell=$2

  if( (NR>1 && oldcell!=cell)) {
    print_cell()
    delete ipins
    delete opins
    delete iopins
  }
  
  if($6=="input") ipins[$4]=1
  if($6=="output") opins[$4]=1
  if($6=="inputOutput") iopins[$4]=1



  oldcell=cell
}

END{
  print_cell()
}

function print_cell()
{
 print "set cellname " library "/" oldcell
 printf "set gensch_i_pin {" 
 for(i in ipins) { 
  printf "%s ", i
 }
 printf "}\n"

 printf "set gensch_o_pin {"
 for(i in opins) { 
  printf "%s ", i
 }
 printf "}\n"

 printf "set gensch_io_pin {"
 for(i in iopins) { 
  printf "%s ", i
 }
 printf "}\n"
 print "create_sym"
 print "make_symbol " library "/" oldcell

}





