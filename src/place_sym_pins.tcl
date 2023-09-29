# from a 'pinlist' file like the one below:
# vss              1
# vccsa            2
# vccdec           3
# LDYMS[15:0]      4
# LDSAL            5
# LDPRECH          6
#
# place pin objects (squares on layer 5)  and labels in the current symbol window.
# pins are placed at growing y coordinates (going down in xschem coordinate system) 
# parameters:
# filename: name for the file holding the list of pins.
# 2 columns are assumed: pin name and pin number 
# dir: pin direction (in, out or inout)
# x, y: coordinate for first pin
# spacing: vertical spacing between one pin and the following.
#
proc place_sym_pins {filename dir {x 0}  {y 0} {spacing 20}} {
  set fd [open $filename r]
  set pinlist [read -nonewline $fd]
  if {$dir == {in} } {
    set flip 0
    set offset 25
    set line_offset 20
  } else {
    set flip 1
    set offset -25
    set line_offset -20
  }
  close $fd
  foreach {name num} $pinlist {
    puts "$name num"
    set x1 [expr {$x - 2.5}]
    set x2 [expr {$x + 2.5}]
    set y1 [expr {$y - 2.5}]
    set y2 [expr {$y + 2.5}]
    xschem set rectcolor 5 ;# symbol pin layer
    xschem rect $x1 $y1 $x2 $y2 -1 "name=$name dir=$dir" 0
    xschem set rectcolor 4 ;# symbol line color
    xschem line $x $y [expr {$x + $line_offset}] $y {} 0
    xschem text [expr {$x + $offset}]  [expr {$y - 4}]  0 $flip $name {} 0.2 0
    incr y $spacing
  }
  xschem set schsymbolprop "type=subcircuit\nformat=\"@name @pinlist @symname\"\ntemplate=\"name=X1\""
  xschem redraw
}


