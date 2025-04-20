v {xschem version=3.4.7RC file_version=1.2
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
B 2 810 -420 1460 -120 {flags=graph
y1=3.2
y2=5
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0.00040726616
x2=0.00050167268
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
node="nout
nin
ntriangle"
color="4 7 6"
dataset=-1
unitx=1
logx=0
logy=0
}
B 2 810 -710 1460 -420 {flags=graph
y1=0.027
y2=0.48
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0.00040726616
x2=0.00050167268
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
dataset=-1
unitx=1
logx=0
logy=0
color=6
node=i(l1)}
B 2 810 -1000 1460 -710 {flags=graph
y1=0
y2=0.89
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0.00040726616
x2=0.00050167268
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
dataset=-1
unitx=1
logx=0
logy=0
color="15 17"
node="power_in; i(vin) nin *  -1 * 6.67u 2 * ravg()
power_out; i(vload) nout * 6.67u 2 * ravg()"}
T {Buck Regulator
This circuit is a simplified buck regulator.
Instead of a digital logic block controlling the regulator,
a simple triangle wave and comparator generates the switch pulses. } 30 -740 0 0 0.4 0.4 {}
T {Copyright (C) 2011 DJ Delorie (dj delorie com)
Distributed under the terms of the GNU General Public License,
either verion 2 or (at your choice) any later version.} 20 -150 0 0 0.4 0.4 {}
N 60 -470 180 -470 {lab=nin}
N 60 -470 60 -340 {lab=nin}
N 210 -230 210 -220 {lab=0}
N 60 -220 210 -220 {lab=0}
N 60 -280 60 -220 {lab=0}
N 210 -430 210 -350 {lab=ntriangle}
N 360 -340 360 -220 {lab=0}
N 210 -220 360 -220 {lab=0}
N 360 -470 360 -400 {lab=ndiode}
N 240 -470 360 -470 {lab=ndiode}
N 360 -470 400 -470 {lab=ndiode}
N 360 -220 570 -220 {lab=0}
N 570 -280 570 -220 {lab=0}
N 520 -470 570 -470 {lab=nout}
N 570 -470 570 -400 {lab=nout}
N 690 -470 690 -400 {lab=nout}
N 570 -470 690 -470 {lab=nout}
N 690 -280 690 -220 {lab=0}
N 570 -220 690 -220 {lab=0}
N 230 -430 570 -430 {lab=nout}
C {vsource.sym} 60 -310 0 0 {name=Vin value="DC pwl 0 0 50u 5V"}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {gnd.sym} 60 -220 0 0 {name=l2 lab=0}
C {diode.sym} 360 -370 2 0 {name=D1 model=BAR42 area=1}
C {vsource.sym} 210 -320 0 0 {name=Vpulse value="pulse -0.06 0.14 0 
+ 3.32u 3.32u 1f 6.67u"}
C {vsource.sym} 210 -260 0 0 {name=Vset value=3.3}
C {ammeter.sym} 490 -470 3 0 {name=Vmeas}
C {capa.sym} 570 -310 0 0 {name=C1
m=1
value=47u
footprint=1206
device="ceramic capacitor"}
C {ammeter.sym} 570 -370 0 0 {name=Vcap}
C {ammeter.sym} 690 -370 0 0 {name=Vload}
C {res.sym} 690 -310 0 0 {name=Rload
value=13.2
footprint=1206
device=resistor
m=1}
C {ind.sym} 430 -470 3 1 {name=L1
m=1
value=18u
footprint=1206
device=inductor}
C {code_shown.sym} 300 -890 0 0 {name=MODELS value=".MODEL BAR42 D(IS=1.139e-08 RS=0.99 CJO=9.3e-12
+ VJ=1.6 M=0.411 BV=30 EG=0.7 ) 
"}
C {code_shown.sym} 50 -910 0 0 {name=COMMANDS value="
.option savecurrents
.control
  save all
  tran 0.001us 0.5ms
  write buck.raw
.endc
"}
C {lab_wire.sym} 650 -470 0 0 {name=l3 sig_type=std_logic lab=nout}
C {lab_wire.sym} 350 -470 0 0 {name=l4 sig_type=std_logic lab=ndiode}
C {lab_wire.sym} 140 -470 0 0 {name=l5 sig_type=std_logic lab=nin}
C {lab_wire.sym} 210 -360 0 0 {name=l6 sig_type=std_logic lab=ntriangle}
C {launcher.sym} 90 -580 0 0 {name=h1
descr="Ctrl-click to go to Delorie's 
project page for info"
url="http://www.delorie.com/electronics/spice-stuff"}
C {launcher.sym} 850 -90 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/buck.raw tran"
}
C {switch_ngspice.sym} 210 -470 3 0 {name=S1 model=SW1
device_model=".MODEL SW1 SW 
+ VT=0.0 VH=0.01
+ RON=0.001 ROFF=10G"}
