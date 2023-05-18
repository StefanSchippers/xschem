#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

set arg1 [lindex $argv 0]
set rout [lindex $argv 1]
# puts stderr "arg1=|$arg1| $rout=|$rout|"
if { $arg1 eq {inv}} {
puts "v {xschem version=3.1.0 file_version=1.2}
K {type=subcircuit
xvhdl_primitive=true
xverilog_primitive=true
xvhdl_format=\"@@y <= not @@a after 90 ps;\"
xverilog_format=\"assign #90 @@y = ~@@a ;\"
format=\"@name @pinlist @symname wn=@wn lln=@lln wp=@wp lp=@lp\"
template=\"name=x1 wn=1u lln=2u wp=4u lp=2u\"
schematic=schematicgen.tcl(inv)}
L 4 -40 0 -20 0 {}
L 4 -20 -20 20 0 {}
L 4 -20 -20 -20 20 {}
L 4 -20 20 20 0 {}
L 4 30 -0 40 -0 {}
B 5 37.5 -2.5 42.5 2.5 {name=y dir=out }
B 5 -42.5 -2.5 -37.5 2.5 {name=a dir=in }
A 4 25 -0 5 180 360 {}
T {$arg1 $rout} -47.5 24 0 0 0.3 0.3 {}
T {@name} 25 -22 0 0 0.2 0.2 {}
T {y} 7.5 -6.5 0 1 0.2 0.2 {}
T {a} -17.5 -6.5 0 0 0.2 0.2 {}
"
} else {
puts "v {xschem version=3.1.0 file_version=1.2}
K {type=subcircuit
xvhdl_primitive=true
xverilog_primitive=true
xvhdl_format=\"@@y <= @@a after 90 ps;\"
xverilog_format=\"assign #90 @@y = @@a ;\"
format=\"@name @pinlist @symname wn=@wn lln=@lln wp=@wp lp=@lp\"
template=\"name=x1 wn=1u lln=2u wp=4u lp=2u\"
schematic=schematicgen.tcl(buf)}
L 4 20 0 40 0 {}
L 4 -40 0 -20 0 {}
L 4 -20 -20 20 0 {}
L 4 -20 -20 -20 20 {}
L 4 -20 20 20 0 {}
B 5 37.5 -2.5 42.5 2.5 {name=y dir=out }
B 5 -42.5 -2.5 -37.5 2.5 {name=a dir=in }
T {$arg1 $rout} -47.5 24 0 0 0.3 0.3 {}
T {@name} 25 -22 0 0 0.2 0.2 {}
T {y} 7.5 -6.5 0 1 0.2 0.2 {}
T {a} -17.5 -6.5 0 0 0.2 0.2 {}
"
}
