v {xschem version=3.4.5 file_version=1.2
*
* This file is part of XSCHEM,
* a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
* simulation.
* Copyright (C) 1998-2023 Stefan Frederik Schippers
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
K {type=subcircuit
format="@name @pinlist @symname "
template="name=X1"}
V {}
S {
}
E {}
L 4 210 -300 230 -320 {}
L 4 210 -300 250 -300 {}
L 4 230 -320 250 -300 {}
L 4 230 -220 250 -240 {}
L 4 210 -240 250 -240 {}
L 4 210 -240 230 -220 {}
L 4 230 -300 230 -240 {}
L 4 350 -580 370 -560 {dash=3}
L 4 370 -600 370 -560 {dash=3}
L 4 350 -580 370 -600 {dash=3}
L 4 370 -580 560 -580 {dash=3}
L 4 1420 -730 1440 -750 {dash=3}
L 4 1420 -770 1420 -730 {dash=3}
L 4 1420 -770 1440 -750 {dash=3}
L 4 1350 -750 1420 -750 {dash=3}
B 2 790 -490 1410 -260 {flags=graph,unlocked 
y1 = 0
y2 = 3
divy = 6
x1=0
x2=3
divx=6
node="a
zz%0
zz%1
zzz
xkeeper.y
xkeeper.y%1"
color="8 6 12 7 9 10"
sweep="v(a)"
dataset=0
hilight_wave=2}
B 2 10 -1080 340 -730 {flags=graph,unlocked 
y1 = 0
y2 = 3
divy = 6
x1=0
x2=3
divx=6
node="a z"
color="7 6"
sweep="z a"
}
B 8 255 -871.25 295 -831.25 {}
B 8 113.75 -1002.5 153.75 -962.5 {}
P 4 5 560 -700 560 -510 1350 -510 1350 -700 560 -700 {dash=3}
P 4 5 820 -920 820 -730 1350 -730 1350 -920 820 -920 {dash=3}
P 4 5 0 -1160 1840 -1160 1840 0 -0 0 0 -1160 {dash=4}
T {These 2 instances
are equivalent} 260 -310 0 0 0.4 0.4 {}
T {Example of using a schematic as a component instance
instead of the usual symbol. LCC: Local Custom Cell.

LCC schematic instantiation show actual parameters
in the schematic instance.} 570 -680 0 0 0.5 0.5 {}
T {LCC schematics can be nested
If only .sch is used there is
no need for a .sym file at all} 840 -880 0 0 0.6 0.6 {}
T {Select one or more graphs (and no other objects)
and use arrow keys to zoom / pan waveforms} 10 -1150 0 0 0.3 0.3 {}
T {Butterfly diagram
of a sram cell} 460 -980 0 0 0.4 0.4 {layer=8}
T {@symname
@name} 1840 -1250 0 1 0.7 0.7 {}
N 410 -100 410 -80 {lab=HALF}
N 410 -190 430 -190 {lab=ZZZ}
N 410 -190 410 -160 {lab=ZZZ}
N 420 -400 420 -380 {lab=HALF}
N 420 -490 740 -490 {lab=ZZ}
N 420 -490 420 -460 {lab=ZZ}
N 740 -240 1450 -240 {lab=#net1}
N 320 -190 410 -190 {lab=ZZZ}
N 330 -490 420 -490 {lab=ZZ}
N 740 -320 740 -240 {
lab=#net1}
N 740 -490 740 -380 {
lab=ZZ}
N 50 -280 50 -270 {
lab=VDD}
N 50 -180 50 -140 {
lab=A}
N 50 -420 50 -400 {
lab=HALF}
N 660 -860 680 -860 {lab=VDD}
N 550 -860 600 -860 {
lab=Z}
N 570 -890 570 -860 {
lab=Z}
N 630 -820 680 -820 {
lab=VDD}
N 680 -860 680 -820 {
lab=VDD}
C {vsource.sym} 50 -110 0 0 {name=V1 value="dc 0 pwl 0 0 1u 0 5u 3"
savecurrent=1}
C {lab_pin.sym} 50 -180 0 0 {name=p4 lab=A}
C {lab_pin.sym} 50 -80 0 0 {name=p5 lab=0}
C {code_shown.sym} 470 -340 0 0 {name=STIMULI
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value="* .ic v(L) = 3 v(R)=0
.control
save all
dc v1 0 3 0.001
* tran 10n 10u uic
write LCC_instances.raw
set appendwrite 
dc v1 3 0 -0.001
write LCC_instances.raw
op
write LCC_instances.raw
* quit 0
.endc
"}
C {code.sym} 840 -190 0 0 {name=MODEL
only_toplevel="true"
tclcommand="xschem edit_vi_prop"
value="************************************************
*         NOMINAL N-Channel Transistor         *
*            UCB-3 Parameter Set               *
*         HIGH-SPEED CMOS Logic Family         *
*                10-Jan.-1995                  *
************************************************
.Model N NMOS (
+LEVEL = 3
+KP    = 45.3E-6
+VTO   = 0.72
+TOX   = 51.5E-9
+NSUB  = 2.8E15
+GAMMA = 0.94
+PHI   = 0.65
+VMAX  = 150E3
+RS    = 40
+RD    = 40
+XJ    = 0.11E-6
+LD    = 0.52E-6
+DELTA = 0.315
+THETA = 0.054
+ETA   = 0.025
+KAPPA = 0.0
+WD    = 0.0 )

***********************************************
*        NOMINAL P-Channel transistor         *
*           UCB-3 Parameter Set               *
*         HIGH-SPEED CMOS Logic Family        *
*               10-Jan.-1995                  *
***********************************************
.Model P PMOS (
+LEVEL = 3
+KP    = 22.1E-6
+VTO   = -0.71
+TOX   = 51.5E-9
+NSUB  = 3.3E16
+GAMMA = 0.92
+PHI   = 0.65
+VMAX  = 970E3
+RS    = 80
+RD    = 80
+XJ    = 0.63E-6
+LD    = 0.23E-6
+DELTA = 2.24
+THETA = 0.108
+ETA   = 0.322
+KAPPA = 0.0
+WD    = 0.0 )

"}
C {lab_pin.sym} 240 -190 0 0 {name=p6 lab=A}
C {lab_pin.sym} 430 -190 0 1 {name=p7 lab=ZZZ}
C {vsource.sym} 50 -240 0 0 {name=V2 value=3
savecurrent=1}
C {lab_pin.sym} 50 -210 0 0 {name=p9 lab=0}
C {res.sym} 410 -130 0 0 {name=R1
value=80k
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 410 -80 0 0 {name=p10 lab=HALF}
C {vsource.sym} 50 -370 0 0 {name=V3 value=1.5
savecurrent=1}
C {lab_pin.sym} 50 -420 0 0 {name=p11 lab=HALF}
C {lab_pin.sym} 50 -340 0 0 {name=p12 lab=0}
C {lab_pin.sym} 120 -490 0 0 {name=p13 lab=A}
C {res.sym} 420 -430 0 0 {name=R2
value=80k
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 420 -380 0 0 {name=p15 lab=HALF}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {cmos_inv.sch} 60 -260 0 0 {name=Xinv WN=15u WP=45u LLN=3u LLP=3u}
C {cmos_inv.sym} 280 -190 0 0 {name=Xinv2 WN=15u WP=45u LLN=3u LLP=3u}
C {bus_keeper.sch} 1200 60 0 0 {name=Xkeeper WN_FB=6u WP_FB=12u}
C {lab_pin.sym} 740 -490 0 1 {name=p1 lab=ZZ}
C {lab_pin.sym} 470 -860 0 0 {name=p14 lab=A}
C {lab_pin.sym} 570 -890 0 1 {name=p2 lab=Z}
C {cmos_inv.sym} 510 -860 0 0 {name=Xinv1 WN=1u WP=1u LLN=2u LLP=2u}
C {launcher.sym} 655 -1115 0 0 {name=h1 
descr="Select arrow and 
Ctrl-Left-Click to load/unload waveforms" 
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw
"
}
C {ammeter.sym} 740 -350 0 1 {name=Vmeas}
C {vdd.sym} 50 -280 0 0 {name=l2 lab=VDD}
C {nmos4.sym} 630 -840 3 0 {name=M1 model=n w=1u l=2u m=1 net_name=true}
C {lab_pin.sym} 630 -860 3 1 {name=l4 sig_type=std_logic lab=0}
C {vdd.sym} 680 -860 0 0 {name=l3 lab=VDD}
