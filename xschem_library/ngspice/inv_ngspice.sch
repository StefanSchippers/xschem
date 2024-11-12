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
T {@name
@FUNC} 490 -520 0 0 0.2 0.2 {name=B2}
T {( @#0:resolved_net )} 40 -275 0 0 0.2 0.2 {name=p1 layer=15}
T {( @#0:resolved_net )} 350 -275 0 1 0.2 0.2 {name=p4 layer=15}
N 470 -470 470 -430 { lab=X1}
N 470 -470 540 -470 { lab=X1}
N 470 -370 470 -320 { lab=0}
N 600 -470 670 -470 { lab=Y1}
N 320 -470 360 -470 { lab=A1}
C {ipin.sym} 70 -240 0 0 {name=p1 lab=A}
C {opin.sym} 320 -240 0 0 {name=p4 lab=Y}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {bsource.sym} 470 -400 0 1 {name=B1 VAR=V FUNC="'VCC/2*(1-tanh((V(A1)-VCC/2)*100))'"
}
C {lab_pin.sym} 320 -470 0 0 {name=l2 sig_type=std_logic lab=A1}
C {lab_pin.sym} 470 -320 0 0 {name=l3 sig_type=std_logic lab=0}
C {lab_pin.sym} 670 -470 0 1 {name=l5 sig_type=std_logic lab=Y1}
C {lab_pin.sym} 130 -240 0 1 {name=l6 sig_type=std_logic lab=A1}
C {lab_pin.sym} 260 -240 0 0 {name=l7 sig_type=std_logic lab=Y1}
C {parax_cap.sym} 360 -460 0 0 {name=C1 gnd=0 value=8f m=1}
C {parax_cap.sym} 620 -460 0 0 {name=C2 gnd=0 value=8f m=1}
C {vsource.sym} 290 -240 1 0 {name=V1 value=0
savecurrent=1}
C {vsource.sym} 100 -240 1 0 {name=V2 value=0
savecurrent=1}
C {bsource.sym} 570 -470 3 1 {name=B2 VAR=I FUNC="'v(X1,Y1) > 0 ? v(X1,Y1) / RUP : v(X1,Y1) / RDOWN'"
hide_texts=1}
C {lab_pin.sym} 470 -470 0 0 {name=l4 sig_type=std_logic lab=X1}
