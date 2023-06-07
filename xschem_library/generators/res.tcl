#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

set arg1 [lindex $argv 0]
puts stderr "res value=$arg1"
if {$arg1 > 0.1} {
puts "v {xschem version=3.4.0 file_version=1.2
}
G {}
K {type=resistor
format=\"@name @pinlist @value m=@m\"
template=\"name=R1 value=1k m=1\"
}
V {}
S {}
E {}
L 4 0 20 0 30 {}
L 4 0 20 7.5 17.5 {}
L 4 -7.5 12.5 7.5 17.5 {}
L 4 -7.5 12.5 7.5 7.5 {}
L 4 -7.5 2.5 7.5 7.5 {}
L 4 -7.5 2.5 7.5 -2.5 {}
L 4 -7.5 -7.5 7.5 -2.5 {}
L 4 -7.5 -7.5 7.5 -12.5 {}
L 4 -7.5 -17.5 7.5 -12.5 {}
L 4 -7.5 -17.5 0 -20 {}
L 4 0 -30 0 -20 {}
L 4 2.5 -22.5 7.5 -22.5 {}
L 4 5 -25 5 -20 {}
B 5 -2.5 -32.5 2.5 -27.5 {name=P dir=inout propag=1 pinnumber=1 goto=1}
B 5 -2.5 27.5 2.5 32.5 {name=M dir=inout propag=0 pinnumber=2 goto=0}
T {@name} -15 -13.75 0 1 0.2 0.2 {}
T {@value} 15 -3.75 0 0 0.2 0.2 {}
T {@#0:net_name} 10 -28.75 0 0 0.15 0.15 {layer=15}
T {@#1:net_name} 10 20 0 0 0.15 0.15 {layer=15}
T {m=@m} -15 1.25 0 1 0.2 0.2 {}
T {@spice_get_current} 12.5 -16.25 0 0 0.2 0.2 {layer=15}"
} else {

puts "v {xschem version=3.4.0 file_version=1.2
}
G {}
K {type=show_label
format=\"* shorted: @name  @#0:net_name <--> @#1:net_name @value\"
template=\"name=R1 value=0.1\"
}
V {}
S {}
E {}
L 1 0 -30 0 30 {}
B 5 -2.5 -32.5 2.5 -27.5 {name=P dir=inout propag=1 pinnumber=1 goto=1}
B 5 -2.5 27.5 2.5 32.5 {name=P dir=inout propag=0 pinnumber=2 goto=0}
T {@value} 15 -3.75 0 0 0.2 0.2 {}
T {@#0:net_name} 10 -28.75 0 0 0.15 0.15 {layer=15}
T {@#1:net_name} 10 20 0 0 0.15 0.15 {layer=15}
T {@name} -15 -13.75 0 1 0.2 0.2 {}"
}
