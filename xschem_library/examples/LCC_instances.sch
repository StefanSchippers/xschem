v {xschem version=3.4.6 file_version=1.2
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
K {type=subcircuit
format="@name @pinlist @symname "
template="name=X1"}
V {}
S {
}
E {}
L 4 220 -310 240 -330 {}
L 4 220 -310 260 -310 {}
L 4 240 -330 260 -310 {}
L 4 240 -230 260 -250 {}
L 4 220 -250 260 -250 {}
L 4 220 -250 240 -230 {}
L 4 240 -310 240 -250 {}
L 4 350 -580 370 -560 {dash=3}
L 4 370 -600 370 -560 {dash=3}
L 4 350 -580 370 -600 {dash=3}
L 4 370 -580 560 -580 {dash=3}
L 4 1420 -730 1440 -750 {dash=3}
L 4 1420 -770 1420 -730 {dash=3}
L 4 1420 -770 1440 -750 {dash=3}
L 4 1350 -750 1420 -750 {dash=3}
B 2 10 -1080 340 -730 {flags=graph,unlocked 
y1 = 0
y2 = 3
divy = 6
x1=2.7755576e-17
x2=3
divx=6
node="a
z"
color="7 4"
sweep="z a"

sim_type=dc
dataset=0}
B 2 850 -270 1150 -60 {flags=graph
y1=0
y2=3
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=-3.9128648e-09
x2=3
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0


dataset=0
unitx=1
logx=0
logy=0

rainbow=0
sweep=a
color="7 8 6"
node="zz%0
zz%1
a%0"

sim_type=tran
hilight_wave=-1}
B 2 850 -480 1150 -270 {flags=graph
y1=0
y2=3
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=-3.9128648e-09
x2=3
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0


dataset=0
unitx=1
logx=0
logy=0

rainbow=0
sweep=a
color="7 8 6"
node="zz%0
zz%1
a%0"

sim_type=dc
hilight_wave=-1}
B 8 255 -871.25 295 -831.25 {}
B 8 113.75 -1002.5 153.75 -962.5 {}
P 4 5 560 -700 560 -510 1350 -510 1350 -700 560 -700 {dash=3}
P 4 5 820 -920 820 -730 1350 -730 1350 -920 820 -920 {dash=3}
P 4 5 0 -1160 1960 -1160 1960 0 -0 0 0 -1160 {dash=4}
T {These 2 instances
are equivalent} 270 -320 0 0 0.4 0.4 {}
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
@name} 1960 -1250 0 1 0.7 0.7 {}
T {DC} 1160 -300 0 0 0.4 0.4 {}
T {TRAN} 1160 -90 0 0 0.4 0.4 {}
N 340 -90 340 -70 {lab=HALF}
N 380 -190 400 -190 {lab=ZZZ}
N 1240 -380 1240 -360 {lab=HALF}
N 420 -490 1350 -490 {lab=ZZ}
N 1350 -240 1450 -240 {lab=#net1}
N 290 -190 380 -190 {lab=ZZZ}
N 1350 -320 1350 -240 {
lab=#net1}
N 1350 -490 1350 -380 {
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
N 570 -910 570 -860 {
lab=Z}
N 630 -820 680 -820 {
lab=VDD}
N 680 -860 680 -820 {
lab=VDD}
N 60 -490 110 -490 {
lab=A}
N 150 -190 210 -190 {
lab=A}
N 1240 -460 1240 -440 {
lab=#net2}
N 340 -170 340 -150 {
lab=#net3}
N 410 -860 470 -860 {
lab=A}
C {vsource.sym} 50 -110 0 0 {name=V1 value="pwl 0 0 10u 3"
savecurrent=1
}
C {lab_pin.sym} 50 -180 0 0 {name=p4 lab=A}
C {lab_pin.sym} 50 -80 0 0 {name=p5 lab=0}
C {code_shown.sym} 510 -450 0 0 {name=STIMULI
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value="* .options SRCSTEPS=0
.option savecurrents
.control
save all
dc v1 0 3 0.001
write LCC_instances.raw
set appendwrite 
dc v1 3 0 -0.001
write LCC_instances.raw
op
write LCC_instances.raw
tran 10n 10u uic
write LCC_instances.raw
alter V1 pwl = [ 0 3 10u 0 ]
tran 10n 10u uic
write LCC_instances.raw
* quit 0
.endc
"}
C {code.sym} 1260 -170 0 0 {name=MODEL
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
C {lab_pin.sym} 150 -190 0 0 {name=p6 lab=A}
C {lab_pin.sym} 400 -190 0 1 {name=p7 lab=ZZZ}
C {vsource.sym} 50 -240 0 0 {name=V2 value=3
savecurrent=1}
C {lab_pin.sym} 50 -210 0 0 {name=p9 lab=0}
C {res.sym} 340 -120 0 0 {name=R1
value=80k
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 340 -70 0 0 {name=p10 lab=HALF}
C {vsource.sym} 50 -370 0 0 {name=V3 value=1.5
savecurrent=1}
C {lab_pin.sym} 50 -420 0 0 {name=p11 lab=HALF}
C {lab_pin.sym} 50 -340 0 0 {name=p12 lab=0}
C {lab_pin.sym} 60 -490 0 0 {name=p13 lab=A}
C {res.sym} 1240 -410 0 0 {name=R2
value=80k
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 1240 -360 0 0 {name=p15 lab=HALF}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {cmos_inv.sch} 50 -260 0 0 {name=Xinv WN=15u WP=45u LLN=3u LLP=3u}
C {cmos_inv.sym} 250 -190 0 0 {name=Xinv2 WN=15u WP=45u LLN=3u LLP=3u}
C {bus_keeper.sch} 1290 60 0 0 {name=Xkeeper WN_FB=6u WP_FB=12u}
C {lab_pin.sym} 1350 -490 0 1 {name=p1 lab=ZZ}
C {lab_pin.sym} 410 -860 0 0 {name=p14 lab=A}
C {lab_pin.sym} 570 -910 0 1 {name=p2 lab=Z}
C {cmos_inv.sym} 510 -860 0 0 {name=Xinv1 WN=1u WP=1u LLN=2u LLP=2u}
C {launcher.sym} 655 -1115 0 0 {name=h1 
descr="load DC sim"
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw dc
"
}
C {ammeter.sym} 1350 -350 0 1 {name=Vmeas}
C {vdd.sym} 50 -280 0 0 {name=l2 lab=VDD}
C {nmos4.sym} 630 -840 3 0 {name=M1 model=n w=1u l=2u m=1}
C {lab_pin.sym} 630 -860 3 1 {name=l4 sig_type=std_logic lab=0}
C {vdd.sym} 680 -860 0 0 {name=l3 lab=VDD}
C {launcher.sym} 655 -1045 0 0 {name=h2 
descr="Annotate OP" 
tclcommand="
xschem annotate_op
"
}
C {launcher.sym} 885 -1115 0 0 {name=h3 
descr="load tran sim"
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw tran
"
}
