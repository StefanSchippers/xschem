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
B 2 590 -730 1390 -330 {flags=graph,unlocked
y1=1.5e-42
y2=0.0028
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=19.99
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
node=i(@j1[id])
color=4
dataset=-1
unitx=1
logx=0
logy=0
rainbow=0}
B 2 590 -1150 1390 -750 {flags=graph,unlocked
y1=9.3e-42
y2=0.0086
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=-19.99
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
node=i(@j2[id])
color=4
dataset=-1
unitx=1
logx=0
logy=0
rainbow=0
sweep=d1}
N 340 -300 340 -250 {
lab=#net1}
N 340 -190 340 -110 {
lab=GND}
N 260 -220 300 -220 {
lab=#net2}
N 150 -220 200 -220 {
lab=G}
N 340 -420 340 -360 {
lab=D}
N 260 -700 300 -700 {
lab=#net3}
N 150 -700 200 -700 {
lab=G1}
N 150 -700 150 -640 {
lab=G1}
N 340 -510 340 -490 {
lab=GND}
N 150 -580 150 -560 {
lab=GND}
N 340 -610 340 -570 {
lab=D1}
N 340 -830 340 -730 {
lab=GND}
C {title.sym} 160 -30 0 0 {name=l9 

author="tcleval(Stefan Schippers[
  if \{$show_pin_net_names == 0\} \{
    set show_pin_net_names 1
    xschem update_all_sym_bboxes
  \}]
)"}
C {njfet.sym} 320 -220 0 0 {name=J1 model=2N3459 area=1 m=1
}
C {lab_pin.sym} 150 -220 0 0 {name=p1 sig_type=std_logic lab=G}
C {lab_pin.sym} 340 -420 0 0 {name=p2 sig_type=std_logic lab=D}
C {lab_pin.sym} 340 -110 0 0 {name=p3 sig_type=std_logic lab=GND}
C {code_shown.sym} 580 -280 0 0 {name=COMMANDS only_toplevel=false value="VG G 0 dc 0
VD D 0 dc 0

.options savecurrents
.control
  save all
  dc VD 0 20 0.01 VG 0 -1 -0.1
  write test_jfet.raw
  quit 0
.endc
"}
C {noconn.sym} 340 -140 0 0 {name=l1}
C {noconn.sym} 340 -390 0 0 {name=l2}
C {noconn.sym} 180 -220 1 0 {name=l3}
C {launcher.sym} 980 -290 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/test_jfet.raw dc"
}
C {ammeter.sym} 230 -220 1 0 {name=Vgate}
C {ammeter.sym} 340 -330 0 0 {name=Vdrain}
C {pjfet.sym} 320 -700 0 0 {name=J2 model=2N2609 area=1 m=1
}
C {lab_pin.sym} 150 -700 0 0 {name=p4 sig_type=std_logic lab=G1}
C {lab_pin.sym} 340 -830 0 0 {name=p6 sig_type=std_logic lab=GND}
C {ammeter.sym} 230 -700 1 0 {name=Vgate1}
C {ammeter.sym} 340 -640 0 0 {name=Vdrain1}
C {vcvs.sym} 340 -540 0 0 {name=E1 value=-1}
C {lab_pin.sym} 300 -520 0 0 {name=p7 sig_type=std_logic lab=GND}
C {lab_pin.sym} 300 -560 0 0 {name=p8 sig_type=std_logic lab=D}
C {lab_pin.sym} 340 -580 0 0 {name=p5 sig_type=std_logic lab=D1}
C {vcvs.sym} 150 -610 0 0 {name=E2 value=-1}
C {lab_pin.sym} 110 -590 0 0 {name=p9 sig_type=std_logic lab=GND}
C {lab_pin.sym} 110 -630 0 0 {name=p10 sig_type=std_logic lab=G}
C {lab_pin.sym} 340 -490 0 0 {name=p11 sig_type=std_logic lab=GND}
C {lab_pin.sym} 150 -560 0 0 {name=p12 sig_type=std_logic lab=GND}
C {code.sym} 40 -440 0 0 {name=MODEL only_toplevel=false value=".MODEL 2N3459 NJF(VTO=-1.4 BETA=1.265m BETATCE=-0.5 LAMBDA=4m RD=1 RS=1 CGS=2.916p CGD=2.8p PB=0.5 IS=114.5f XTI=3 AF=1 FC=0.5 N=1 NR=2)
.model 2N2609 PJF (Beta=3.2m Betatce=-500m Rd=1 Rs=1 Lambda=18.5m Vto=-1.408 Vtotc=-2.5m Is=461.5f Isr=4.402p N=1 Nr=2 Xti=3 Alpha=32.54u Vk=393.2 Cgd=6.5p M=278.9m Pb=1 Fc=500m Cgs=9p Kf=0.2062f Af=1)"}
