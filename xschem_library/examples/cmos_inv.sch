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
K {type=subcircuit
function0="1 ~"
format="@name @pinlist @symname WN=@WN WP=@WP LLN=@LLN LLP=@LLP m=@m"
template="name=X1 WN=15u WP=45u LLN=3u LLP=3u m=1"
}
V {}
S {}
E {}
A 15 90 -410 14.14213562373095 135 360 {dash=2}
P 2 5 60 -450 370 -450 370 -90 60 -90 60 -450 {dash=5}
T {@name} 60 -465 0 0 0.2 0.2 {}
T {@symname} 63.75 -85 0 0 0.2 0.2 {}
N 80 -230 80 -170 {lab=A}
N 60 -230 80 -230 {lab=A}
N 140 -400 140 -380 {lab=VDD}
N 140 -140 140 -120 {lab=0}
N 140 -230 180 -230 {
lab=D}
N 140 -320 140 -290 {
lab=#net1}
N 140 -230 140 -200 {
lab=D}
N 80 -350 100 -350 {
lab=A}
N 80 -170 100 -170 {
lab=A}
N 330 -230 370 -230 {
lab=Z}
N 240 -230 270 -230 {
lab=#net2}
N 140 -350 200 -350 {
lab=VDD}
N 140 -170 200 -170 {
lab=0}
N 80 -350 80 -230 {lab=A}
C {opin.sym} 370 -230 0 0 {name=p2 lab=Z}
C {ipin.sym} 60 -230 0 0 {name=p1 lab=A  goto=0}
C {vdd.sym} 140 -400 0 0 {name=l1 lab=VDD}
C {lab_pin.sym} 140 -120 0 0 {name=l2 sig_type=std_logic lab=0}
C {pmos4.sym} 120 -350 0 0 {name=M2 model=p w=WP l=LLP m=1}
C {nmos4.sym} 120 -170 0 0 {name=M1 model=n w=WN l=LLN m=1}
C {lab_pin.sym} 200 -170 0 1 {name=l3 sig_type=std_logic lab=0}
C {lab_pin.sym} 200 -350 0 1 {name=l4 sig_type=std_logic lab=VDD}
C {title.sym} 160 -30 0 0 {name=l5 author="Stefan Schippers"}
C {res.sym} 210 -230 1 0 {name=R1
value=10
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 140 -220 0 1 {name=l6 sig_type=std_logic lab=D}
C {lab_pin.sym} 80 -320 0 1 {name=l7 sig_type=std_logic lab=A}
C {ammeter.sym} 140 -260 0 0 {name=V1}
C {ammeter.sym} 300 -230 3 0 {name=Vmeas savecurrent=true}
