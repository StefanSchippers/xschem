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
N 480 -400 480 -360 { lab=#net1}
N 480 -400 550 -400 { lab=#net1}
N 480 -300 480 -250 { lab=0}
N 610 -400 680 -400 { lab=Y1}
N 330 -400 370 -400 { lab=A1}
N 330 -460 370 -460 { lab=B1}
C {ipin.sym} 80 -170 0 0 {name=p1 lab=A}
C {opin.sym} 330 -170 0 0 {name=p4 lab=Y}
C {bsource.sym} 480 -330 0 1 {name=B1 VAR=V FUNC="'VCC/2*(1+tanh((max(V(A1),V(B1))-VCC/2)*100))'"
}
C {lab_pin.sym} 330 -400 0 0 {name=l2 sig_type=std_logic lab=A1}
C {lab_pin.sym} 480 -250 0 0 {name=l3 sig_type=std_logic lab=0}
C {res.sym} 580 -400 1 0 {name=R1
value='ROUT'
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 680 -400 0 1 {name=l5 sig_type=std_logic lab=Y1}
C {lab_pin.sym} 140 -170 0 1 {name=l6 sig_type=std_logic lab=A1}
C {lab_pin.sym} 270 -170 0 0 {name=l7 sig_type=std_logic lab=Y1}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {ipin.sym} 80 -110 0 0 {name=p2 lab=B}
C {lab_pin.sym} 140 -110 0 1 {name=l4 sig_type=std_logic lab=B1}
C {lab_pin.sym} 330 -460 0 0 {name=l8 sig_type=std_logic lab=B1}
C {parax_cap.sym} 370 -390 0 0 {name=C1 gnd=0 value=8f m=1}
C {parax_cap.sym} 370 -450 0 0 {name=C2 gnd=0 value=8f m=1}
C {parax_cap.sym} 630 -390 0 0 {name=C3 gnd=0 value=8f m=1}
C {vsource.sym} 110 -170 1 0 {name=V1 value=0}
C {vsource.sym} 110 -110 1 0 {name=V2 value=0}
C {vsource.sym} 300 -170 1 0 {name=V3 value=0}
