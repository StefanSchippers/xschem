v {xschem version=3.4.5 file_version=1.2
*
* This file is part of XSCHEM,
* a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
* simulation.
* Copyright (C) 1998-2024 Stefan Frederik Schippers
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
}
G {}
K {}
V {}
S {}
E {}
L 7 930 -250 1110 -250 {}
P 4 7 330 -690 330 -610 320 -610 330 -590 340 -610 330 -610 330 -690 {}
P 4 7 650 -600 600 -600 600 -610 580 -600 600 -590 600 -600 650 -600 {}
T {Set tcl variable IGNORE to 1 or 0 to
enable / disable / short  some components} 50 -940 0 0 1 1 {}
T {tcleval(IGNORE=$IGNORE)} 930 -290 0 0 0.6 0.6 {name=l1}
T {The short component is a pass-through symbol. It can be used to short two nets.
Setting spice_ignore=true will disable the component and remove the short.} 90 -750 0 0 0.4 0.4 {}
T {This is the lab_show component
it is used only to display the net
name it is attached to. This works if
Options->Show net names on symbol pins
is enabled.} 660 -660 0 0 0.4 0.4 {}
T {This instance has a 'spice_ignore=short'
attribute when IGNORE=0} 440 -240 0 0 0.4 0.4 { layer=6}
T {This component behaves either as
an inverter or as a short
depending on IGNORE} 1310 -540 0 0 0.4 0.4 { layer=1}
N 130 -290 180 -290 {
lab=NET_A}
N 480 -290 530 -290 {
lab=NET_B}
N 180 -390 180 -290 {
lab=NET_A}
N 480 -390 480 -290 {
lab=NET_B}
N 180 -390 300 -390 {
lab=NET_A}
N 360 -390 480 -390 {
lab=NET_B}
N 160 -480 180 -480 {
lab=NET_C}
N 480 -480 530 -480 {
lab=NET_C}
N 180 -580 180 -480 {
lab=NET_C}
N 480 -580 480 -480 {
lab=NET_C}
N 180 -580 300 -580 {
lab=NET_C}
N 360 -580 480 -580 {
lab=NET_C}
N 380 -480 480 -480 {
lab=NET_C}
N 180 -480 300 -480 {
lab=NET_C}
N 380 -290 480 -290 {
lab=NET_B}
N 180 -290 300 -290 {
lab=NET_A}
N 610 -480 660 -480 {
lab=#net1}
N 660 -460 660 -400 {
lab=#net1}
N 660 -400 750 -400 {
lab=#net1}
N 660 -360 750 -360 {
lab=NET_B}
N 660 -360 660 -290 {
lab=NET_B}
N 610 -290 660 -290 {
lab=NET_B}
N 980 -380 1020 -380 {
lab=NET_E}
N 660 -460 760 -460 {
lab=#net1}
N 820 -460 980 -460 {
lab=NET_E}
N 980 -460 980 -380 {
lab=NET_E}
N 660 -480 660 -460 {
lab=#net1}
N 120 -420 160 -420 {
lab=NET_C}
N 160 -480 160 -420 {
lab=NET_C}
N 1560 -590 1680 -590 {
lab=#net2}
N 120 -480 160 -480 {
lab=NET_C}
N 1760 -590 1820 -590 {
lab=NET_F}
N 1210 -590 1480 -590 {
lab=NET_B}
N 850 -380 980 -380 {
lab=NET_E}
C {lab_pin.sym} 130 -290 0 0 {name=p3 sig_type=std_logic lab=NET_A}
C {ipin.sym} 100 -190 0 0 { name=p4 lab=NET_D }
C {title.sym} 160 -30 0 0 {name=l1
author="Stefan Schippers"
}
C {short.sym} 330 -390 1 0 {name=x2
spice_ignore="tcleval([if \{$IGNORE == 1\} \{return \{false\}\} else \{return \{true\}\}])"
}
C {lab_pin.sym} 660 -290 0 1 {name=p5 sig_type=std_logic lab=NET_B}
C {lab_show.sym} 480 -390 0 1 {name=l2 }
C {lab_pin.sym} 60 -480 0 0 {name=p1 sig_type=std_logic lab=NET_C}
C {short.sym} 330 -580 1 0 {name=x5
spice_ignore="tcleval([if \{$IGNORE == 1\} \{return \{true\}\} else \{return \{false\}\}])"
}
C {lab_show.sym} 480 -580 0 1 {name=l3 }
C {ipin.sym} 100 -170 0 0 { name=p7 lab=NET_C }
C {lab_show.sym} 660 -480 0 1 {name=l5 }
C {inv_ngspice.sym} 570 -480 0 0 {name=x3  
ROUT=1000}
C {inv_ngspice.sym} 570 -290 0 0 {name=x6  
ROUT=1000
spice_ignore="tcleval([if \{$IGNORE == 1\} \{return \{false\}\} else \{return \{short\}\}])"}
C {inv_ngspice.sym} 340 -480 0 0 {name=x7  
spice_ignore="tcleval([if \{$IGNORE == 1\} \{return \{false\}\} else \{return \{true\}\}])"
ROUT=1000}
C {inv_ngspice.sym} 340 -290 0 0 {name=x8  
spice_ignore="tcleval([if \{$IGNORE == 1\} \{return \{true\}\} else \{return \{false\}\}])"
ROUT=1000}
C {and_ngspice.sym} 790 -380 0 0 {name=x4 ROUT=1000 
spice_ignore="tcleval([if \{$IGNORE == 0\} \{return \{false\}\} else \{return \{true\}\}])"
}
C {short.sym} 790 -460 1 0 {name=x1
spice_ignore="tcleval([if \{$IGNORE == 0\} \{return \{true\}\} else \{return \{false\}\}])"
}
C {lab_show.sym} 1020 -380 0 1 {name=l4 }
C {launcher.sym} 830 -100 0 0 {name=h1
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
C {ammeter.sym} 90 -480 3 0 {name=Vopt2
spice_ignore="tcleval([if \{$IGNORE == 1\} \{return \{true\}\} else \{return \{short\}\}])"
}
C {lab_pin.sym} 60 -420 0 0 {name=p2 sig_type=std_logic lab=NET_D}
C {ammeter.sym} 90 -420 3 0 {name=Vopt1
spice_ignore="tcleval([if \{$IGNORE == 0\} \{return \{true\}\} else \{return \{short\}\}])"
}
C {lab_show.sym} 180 -580 0 0 {name=l6 }
C {lab_wire.sym} 920 -380 0 0 {name=p6 sig_type=std_logic lab=NET_E}
C {lab_pin.sym} 1820 -590 0 1 {name=p8 sig_type=std_logic lab=NET_F}
C {inv_ngspice.sym} 1520 -590 0 0 {name=x10  
ROUT=1000
spice_ignore="tcleval([if \{$IGNORE == 0\} \{return \{false\}\} else \{return \{short\}\}])"}
C {inv_ngspice.sym} 1720 -590 0 0 {name=x11  
ROUT=1000}
C {lab_pin.sym} 1210 -590 0 0 {name=p9 sig_type=std_logic lab=NET_B}
C {lab_show.sym} 1590 -590 0 1 {name=l7 }
C {lab_show.sym} 1380 -590 0 1 {name=l8 }
C {ipin.sym} 100 -210 0 0 { name=p10 lab=NET_A }
