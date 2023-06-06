v {xschem version=3.4.0 file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
L 7 920 -190 1100 -190 {}
P 4 7 320 -630 320 -550 310 -550 320 -530 330 -550 320 -550 320 -630 {}
P 4 7 640 -540 590 -540 590 -550 570 -540 590 -530 590 -540 640 -540 {}
P 4 7 950 -470 950 -380 940 -380 950 -360 960 -380 950 -380 950 -470 {}
T {Title symbol has embedded TCL command
to enable show_pin_net_names.} 130 -130 0 0 0.4 0.4 { layer=7}
T {Set tcl variable IGNORE to 1 or 0 to
enable / disable some components} 50 -940 0 0 1 1 {}
T {tcleval(IGNORE=$IGNORE)} 920 -230 0 0 0.6 0.6 {name=l1}
T {The short component is a pass-through symbol. It can be used to short two nets.
Setting spice_ignore=true will disable the component and remove the short.} 80 -690 0 0 0.4 0.4 {}
T {This is the lab_show component
it is used only to display the net
name it is attached to. This works if
Options->Show net names on symbol pins
is enabled.} 650 -600 0 0 0.4 0.4 {}
N 120 -230 170 -230 {
lab=NET_A}
N 470 -230 520 -230 {
lab=#net1}
N 170 -330 170 -230 {
lab=NET_A}
N 470 -330 470 -230 {
lab=#net1}
N 170 -330 290 -330 {
lab=NET_A}
N 350 -330 470 -330 {
lab=#net1}
N 120 -420 170 -420 {
lab=NET_C}
N 470 -420 520 -420 {
lab=NET_C}
N 170 -520 170 -420 {
lab=NET_C}
N 470 -520 470 -420 {
lab=NET_C}
N 170 -520 290 -520 {
lab=NET_C}
N 350 -520 470 -520 {
lab=NET_C}
N 370 -420 470 -420 {
lab=NET_C}
N 170 -420 290 -420 {
lab=NET_C}
N 370 -230 470 -230 {
lab=#net1}
N 170 -230 290 -230 {
lab=NET_A}
N 600 -420 650 -420 {
lab=#net2}
N 650 -400 650 -340 {
lab=#net2}
N 650 -340 740 -340 {
lab=#net2}
N 650 -300 740 -300 {
lab=NET_B}
N 650 -300 650 -230 {
lab=NET_B}
N 600 -230 650 -230 {
lab=NET_B}
N 860 -320 900 -320 {
lab=#net2}
N 650 -400 750 -400 {
lab=#net2}
N 810 -400 860 -400 {
lab=#net2}
N 860 -400 860 -320 {
lab=#net2}
N 650 -420 650 -400 {
lab=#net2}
N 840 -320 860 -320 {
lab=#net2}
C {lab_pin.sym} 120 -230 0 0 {name=p3 sig_type=std_logic lab=NET_A}
C {iopin.sym} 80 -140 0 1 { name=p4 lab=NET_A }
C {title.sym} 160 -30 0 0 {name=l1
author="tcleval([
  if \{$show_pin_net_names == 0\} \{
    set IGNORE 1
    set show_pin_net_names 1
    xschem update_all_sym_bboxes
  \}
  return \{Stefan Schippers\}
])"
}
C {short.sym} 320 -330 0 0 {name=x2
spice_ignore="tcleval([if \{$IGNORE == 1\} \{return \{false\}\} else \{return \{true\}\}])"
}
C {lab_pin.sym} 650 -230 0 1 {name=p5 sig_type=std_logic lab=NET_B}
C {lab_show.sym} 470 -330 0 1 {name=l2 }
C {lab_pin.sym} 120 -420 0 0 {name=p1 sig_type=std_logic lab=NET_C}
C {short.sym} 320 -520 0 0 {name=x5
spice_ignore="tcleval([if \{$IGNORE == 1\} \{return \{true\}\} else \{return \{false\}\}])"
}
C {lab_show.sym} 470 -520 0 1 {name=l3 }
C {iopin.sym} 80 -110 0 1 { name=p7 lab=NET_C }
C {lab_show.sym} 650 -420 0 1 {name=l5 }
C {inv_ngspice.sym} 560 -420 0 0 {name=x3  
ROUT=1000}
C {inv_ngspice.sym} 560 -230 0 0 {name=x6  
ROUT=1000}
C {inv_ngspice.sym} 330 -420 0 0 {name=x7  
spice_ignore="tcleval([if \{$IGNORE == 1\} \{return \{false\}\} else \{return \{true\}\}])"
ROUT=1000}
C {inv_ngspice.sym} 330 -230 0 0 {name=x8  
spice_ignore="tcleval([if \{$IGNORE == 1\} \{return \{true\}\} else \{return \{false\}\}])"
ROUT=1000}
C {and_ngspice.sym} 780 -320 0 0 {name=x4 ROUT=1000 
spice_ignore="tcleval([if \{$IGNORE == 1\} \{return \{false\}\} else \{return \{true\}\}])"
}
C {short.sym} 780 -400 0 0 {name=x1
spice_ignore="tcleval([if \{$IGNORE == 1\} \{return \{true\}\} else \{return \{false\}\}])"
}
C {lab_show.sym} 900 -320 0 1 {name=l4 }
C {launcher.sym} 750 -90 0 0 {name=h1
descr="Toggle IGNORE variable and
rebuild connectivity"
tclcommand="
if \{![info exists IGNORE]\} \{
   set IGNORE 1
\} else \{
  set IGNORE [expr \{!$IGNORE\}]
\}
xschem rebuild_connectivity
xschem unhilight_all
"}
