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
B 2 970 -630 1770 -230 {flags=graph
y1=-4.3e-13
y2=1.4
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=6
divx=5
subdivx=1
node=s_vec
color=4
dataset=0
unitx=1
logx=0
logy=0
}
T {This example shows how to create an ngspice custom plot
some op analyses are done and operating point data for node 'S'
of all op runs is collected and stored into a new vector,
result is saved in a raw file.} 10 -770 0 0 0.7 0.7 {layer=3}
N 30 -240 30 -210 {lab=VCC}
N 30 -240 60 -240 {lab=VCC}
N 240 -260 240 -210 {lab=GN}
N 240 -210 280 -210 {lab=GN}
N 280 -210 280 -180 {lab=GN}
N 280 -180 460 -180 {lab=GN}
N 500 -250 500 -210 {lab=S}
N 430 -280 450 -280 {lab=0}
N 550 -280 570 -280 {lab=0}
N 430 -250 570 -250 {lab=S}
N 370 -280 390 -280 {lab=PLUS}
N 610 -280 630 -280 {lab=MINUS}
N 570 -430 620 -430 {lab=VCC}
N 380 -430 430 -430 {lab=VCC}
N 470 -430 530 -430 {lab=G}
N 470 -430 470 -400 {lab=G}
N 430 -400 470 -400 {lab=G}
N 430 -400 430 -310 {lab=G}
N 570 -400 570 -310 {lab=DIFFOUT}
N 570 -500 570 -460 {lab=VCC}
N 430 -500 570 -500 {lab=VCC}
N 430 -500 430 -460 {lab=VCC}
N 500 -520 500 -500 {lab=VCC}
N 570 -360 830 -360 {lab=DIFFOUT}
N 30 -370 30 -340 {lab=PLUS}
N 30 -370 60 -370 {lab=PLUS}
N 30 -520 30 -490 {lab=MINUS}
N 30 -520 60 -520 {lab=MINUS}
N 500 -150 500 -120 {lab=0}
N 500 -150 520 -150 {lab=0}
N 520 -180 520 -150 {lab=0}
N 500 -180 520 -180 {lab=0}
N 240 -150 240 -120 {lab=0}
N 220 -150 240 -150 {lab=0}
N 220 -180 220 -150 {lab=0}
N 220 -180 240 -180 {lab=0}
C {lab_pin.sym} 750 -300 0 0 {name=p20 lab=0}
C {lab_pin.sym} 30 -150 0 0 {name=p17 lab=0}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {nmos4.sym} 480 -180 0 0 {name=m1 model=cmosn w=5u l=2u m=1}
C {pmos4.sym} 550 -430 0 0 {name=m2 model=cmosp w=5u l=2u m=1}
C {vsource.sym} 30 -180 0 0 {name=VVCC value=5}
C {lab_pin.sym} 500 -120 0 0 {name=p1 lab=0}
C {lab_pin.sym} 60 -240 0 1 {name=p2 lab=VCC}
C {nmos4.sym} 260 -180 0 1 {name=m3 model=cmosn w=5u l=2u m=1}
C {lab_pin.sym} 240 -120 0 0 {name=p3 lab=0}
C {isource.sym} 240 -290 0 0 {name=IBIAS value=100u}
C {lab_pin.sym} 240 -320 0 0 {name=p4 lab=0}
C {nmos4.sym} 410 -280 0 0 {name=m4 model=cmosn w=10u l=1u m=1}
C {lab_pin.sym} 450 -280 0 1 {name=p5 lab=0}
C {nmos4.sym} 590 -280 0 1 {name=m5 model=cmosn w=10u l=1u m=1}
C {lab_pin.sym} 550 -280 0 0 {name=p0 lab=0}
C {lab_pin.sym} 620 -430 0 1 {name=p6 lab=VCC}
C {pmos4.sym} 450 -430 0 1 {name=m6 model=cmosp w=5u l=2u m=1}
C {lab_pin.sym} 380 -430 0 0 {name=p7 lab=VCC}
C {lab_pin.sym} 500 -520 0 0 {name=p8 lab=VCC}
C {lab_pin.sym} 370 -280 0 0 {name=p9 lab=PLUS}
C {lab_pin.sym} 630 -280 0 1 {name=p10 lab=MINUS}
C {lab_pin.sym} 830 -360 0 1 {name=p11 lab=DIFFOUT}
C {lab_pin.sym} 240 -230 0 0 {name=p13 lab=GN}
C {lab_pin.sym} 30 -280 0 0 {name=p14 lab=0}
C {vsource.sym} 30 -310 0 0 {name=VPLUS value=2.5
}
C {lab_pin.sym} 60 -370 0 1 {name=p15 lab=PLUS}
C {lab_pin.sym} 30 -430 0 0 {name=p16 lab=0}
C {vsource.sym} 30 -460 0 0 {name=V1 value=2.5}
C {lab_pin.sym} 60 -520 0 1 {name=p18 lab=MINUS}
C {capa.sym} 750 -330 0 0 {name=CL
m=1
value=2p
footprint=1206
device="ceramic capacitor"}
C {code.sym} 870 -190 0 0 {name=STIMULI
only_toplevel=true
value="
** ngspice
.temp 30

** xyce
* .step temp list 30

** ngspice
* .save all

** models are generally not free: you must download
** SPICE models for active devices and put them  into the below 
** referenced file in netlist/simulation directory.
.option savecurrents
.include \\"models_cmos_example.txt\\"
.control
save all
let vol = 0
let i = 1
set curplot = new         $ create a new plot
set myplot = $curplot     $ store the plot name
set curplotname = myplot
let s_vec = vector(61)
settype voltage s_vec
while vol <= 6
alter VVCC vol  $ change value of vcc source
op
let vol = vol + 0.1
set run = $&i  $ create a var from the vector
let i = i + 1
setplot $myplot
let s_vec[\{$run\}] = op\{$run\}.s  $ get operating point v(s) and store into vector
end
* current plot here is last set one, $myplot
compose vcc start=0 stop=6 step=0.1  $ build x-axis voltage 
setscale vcc                         $ set as xaxis for myplot
settype voltage vcc
write plot_manipulation.raw
plot s_vec
quit 0
.endc

** ngspice
* .save all

** xyce, not needed if -r given om cmdline
* .print tran format=raw v(diffout) v(plus) v(minus)
"}
C {ngspice_probe.sym} 500 -210 0 0 {name=r9}
C {lab_pin.sym} 430 -370 0 0 {name=p12 lab=G}
C {lab_pin.sym} 500 -230 0 0 {name=p19 lab=S}
C {launcher.sym} 1100 -200 0 0 {name=h3 
descr="Select arrow and 
Ctrl-Left-Click to load/unload waveforms" 
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw myplot
"
}
