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
B 2 730 -720 1530 -320 {flags=graph
y1=-0.052
y2=5
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0.000177095
x2=0.00025416
divx=5
subdivx=1
node="out
trig
\\"2/3 Vcc; ctrl\\"
\\"1/3 Vcc; x1.18\\"
dis"
color="4 15 7 12 9"
dataset=-1
unitx=1
logx=0
logy=0
hilight_wave=-1}
T {NE555 TIMER IC EXAMPLE} 360 -490 0 0 0.4 0.4 {}
N 130 -150 200 -150 {lab=TRIG}
N 570 -130 570 -100 {
lab=CTRL}
C {code.sym} 710 -190 0 0 {
name=STIMULI 
value=".option SCALE=1e-6 PARHIER=LOCAL RUNLVL=6 post MODMONTE=1
.option sampling_method = SRS
* .option method=gear
vvss vss 0 dc 0

** models are generally not free: you must download
** SPICE models for active devices and put them  into the below 
** referenced file in netlist/simulation directory.
.temp 30
* .include \\"model_test_ne555.txt\\"

.control
  save all
  tran 0.1u 1m uic
  write test_ne555.raw
.endc
"}
C {ne555.sym} 490 -220 0 0 {name=x1
tclcommand="textwindow $netlist_dir/model_test_ne555.txt"}
C {res.sym} 130 -180 0 0 {name=r3 m=1 value=2k}
C {capa.sym} 130 -120 0 0 {name=c6 m=1 value=2n}
C {vsource.sym} 130 -360 0 1 {name=v1 value=5}
C {lab_pin.sym} 130 -330 0 0 {name=p16 lab=VSS}
C {lab_pin.sym} 130 -390 0 1 {name=p6 lab=VSUPPLY}
C {lab_pin.sym} 570 -340 0 1 {name=p8 lab=VSUPPLY}
C {lab_pin.sym} 410 -340 0 0 {name=p9 lab=VSS}
C {lab_pin.sym} 410 -260 0 0 {name=p11 lab=TRIG}
C {lab_pin.sym} 410 -180 0 0 {name=p12 lab=OUT}
C {lab_pin.sym} 570 -180 0 1 {name=p15 lab=TRIG}
C {lab_pin.sym} 200 -150 0 1 {name=p19 lab=TRIG}
C {lab_pin.sym} 130 -270 0 0 {name=p20 lab=VSUPPLY}
C {lab_pin.sym} 130 -90 0 0 {name=p21 lab=VSS}
C {lab_pin.sym} 570 -100 0 1 {name=p14 lab=CTRL}
C {lab_pin.sym} 410 -100 0 0 {name=p13 lab=VSUPPLY}
C {lab_pin.sym} 570 -260 0 1 {name=p17 lab=DIS}
C {res.sym} 130 -240 0 0 {name=r5 m=1 value=2k}
C {lab_pin.sym} 130 -210 0 0 {name=p18 lab=DIS}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {launcher.sym} 790 -290 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/test_ne555.raw tran"
}
C {noconn.sym} 570 -130 2 0 {name=l2}
