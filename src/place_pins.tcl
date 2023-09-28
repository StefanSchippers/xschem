
# from a 'pinlist' file like the one below:
# vss              1
# vccsa            2
# vccdec           3
# LDYMS[15:0]      4
# LDSAL            5
# LDPRECH          6
#
# place pin symbols (like ipin.sym, opin.sym, iopin.sym,
# or devices/ipin.sym depending on your search path setting) in current schematic.
# Pins are placed at growing y coordinates (going down in xschem coordinate system) 
# parameters:
# filename: name for the file holding the list of pins.
# 2 columns are assumed: pin name and pin number 
# symname: name of the pin to place (ipin.sym, opin.sym, devices/ipin.sym, ...).
# x, y: coordinate for first pin
# spacing: vertical spacing between one pin and the following.
#
proc place_pins {filename symname {x 0}  {y 0} {spacing 40}} {
  set i 0
  set fd [open $filename r]
  set pinlist [read -nonewline $fd]
  close $fd
  foreach {name num} $pinlist {
    puts "$name num"
    # xschem instance sym_name x y rot flip [prop] [n]
    xschem instance $symname $x $y 0 0 "name=p$num lab=$name" $i
    incr i
    incr y $spacing
  }
  xschem redraw
}


