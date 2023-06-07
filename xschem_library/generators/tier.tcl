#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

set arg1 [lindex $argv 0]

puts stderr "tier node=$arg1"
puts "v {xschem version=3.4.0 file_version=1.2
}
G {}
K {type=label
format=\"*.alias @lab\"
template=\"name=p1 sig_type=std_logic lab=xxx\"
net_name=true
}
V {}
S {}
E {}
B 5 -2.5 -2.5 2.5 2.5 {name=P dir=inout propag=1 pinnumber=1 goto=1}"

if {[regexp -nocase {^(VCC|VDD|VPP)$} $arg1]} {
puts "L 4 -20 -40 20 -40 {}
L 4 0 -40 0 0 {}



T {@#0:net_name} 5 -13.75 0 0 0.15 0.15 {layer=15}
T {@lab} -15 -53.75 0 0 0.2 0.2 {}"
} else {
puts "P 4 7 0 0 0 -20 -10 -20 0 -40 10 -20 0 -20 0 0 {fill=true}
T {@#0:net_name} 5 -13.75 0 0 0.15 0.15 {layer=15}
T {@lab} -15 -53.75 0 0 0.2 0.2 {}"
}

