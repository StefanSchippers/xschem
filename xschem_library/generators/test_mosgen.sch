v {xschem version=3.4.4 file_version=1.2
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
N 500 -230 500 -170 {
lab=#net1}
N 390 -140 460 -140 {
lab=IN}
N 500 -110 500 -50 {
lab=GND}
N 790 -230 790 -170 {
lab=OUT}
N 680 -140 750 -140 {
lab=#net1}
N 790 -110 790 -50 {
lab=GND}
N 390 -260 460 -260 {
lab=IN}
N 680 -260 750 -260 {
lab=#net1}
N 500 -350 500 -290 {
lab=VDD}
N 790 -350 790 -290 {
lab=VDD}
N 790 -140 790 -110 {
lab=GND}
N 500 -140 500 -110 {
lab=GND}
N 500 -290 500 -260 {
lab=VDD}
N 790 -290 790 -260 {
lab=VDD}
N 680 -260 680 -140 {
lab=#net1}
N 500 -200 680 -200 {
lab=#net1}
N 390 -260 390 -140 {
lab=IN}
N 500 -50 790 -50 {
lab=GND}
N 500 -350 790 -350 {
lab=VDD}
N 340 -200 390 -200 {
lab=IN}
N 790 -200 900 -200 {
lab=OUT}
C {ipin.sym} 340 -200 0 0 {name=p1 lab=IN}
C {opin.sym} 900 -200 0 0 {name=p2 lab=OUT}
C {vdd.sym} 640 -350 0 0 {name=l1 lab=VDD}
C {gnd.sym} 640 -50 0 0 {name=l2 lab=GND}
C {mosgen.tcl(@model\\)} 480 -260 0 0 {name=M1
L=0.15
W=1
nf=1 
mult=1
ad="'int((nf+1)/2) * W/nf * 0.29'" 
pd="'2*int((nf+1)/2) * (W/nf + 0.29)'"
as="'int((nf+2)/2) * W/nf * 0.29'" 
ps="'2*int((nf+2)/2) * (W/nf + 0.29)'"
nrd="'0.29 / W'" nrs="'0.29 / W'"
sa=0 sb=0 sd=0
model=pfet_01v8
spiceprefix=X
}
C {mosgen.tcl(@model\\)} 480 -140 0 0 {name=M2
L=0.15
W=1
nf=1 
mult=1
ad="'int((nf+1)/2) * W/nf * 0.29'" 
pd="'2*int((nf+1)/2) * (W/nf + 0.29)'"
as="'int((nf+2)/2) * W/nf * 0.29'" 
ps="'2*int((nf+2)/2) * (W/nf + 0.29)'"
nrd="'0.29 / W'" nrs="'0.29 / W'"
sa=0 sb=0 sd=0
model=nfet_01v8
spiceprefix=X
}
C {mosgen.tcl(@model\\)} 770 -260 0 0 {name=M3
L=0.15
W=1
nf=1 
mult=1
ad="'int((nf+1)/2) * W/nf * 0.29'" 
pd="'2*int((nf+1)/2) * (W/nf + 0.29)'"
as="'int((nf+2)/2) * W/nf * 0.29'" 
ps="'2*int((nf+2)/2) * (W/nf + 0.29)'"
nrd="'0.29 / W'" nrs="'0.29 / W'"
sa=0 sb=0 sd=0
model=pfet_g5v0d10v5
spiceprefix=X
}
C {mosgen.tcl(@model\\)} 770 -140 0 0 {name=M4
L=0.15
W=1
nf=1 
mult=1
ad="'int((nf+1)/2) * W/nf * 0.29'" 
pd="'2*int((nf+1)/2) * (W/nf + 0.29)'"
as="'int((nf+2)/2) * W/nf * 0.29'" 
ps="'2*int((nf+2)/2) * (W/nf + 0.29)'"
nrd="'0.29 / W'" nrs="'0.29 / W'"
sa=0 sb=0 sd=0
model=nfet_g5v0d10v5
spiceprefix=X
}
