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
G {
y <= not a after 0.1 ns ;}
K {}
V {assign #150 y=~a ;}
S {}
E {}
N 190 -250 190 -210 {lab=y}
N 190 -230 230 -230 {lab=y}
N 150 -280 150 -180 {lab=a}
N 110 -230 150 -230 {lab=a}
N 190 -280 270 -280 {lab=VCC}
N 270 -310 270 -280 {lab=VCC}
N 190 -310 270 -310 {lab=VCC}
N 190 -180 290 -180 {lab=0}
N 290 -180 290 -150 {lab=0}
N 190 -150 290 -150 {lab=0}
N 190 -350 190 -310 {lab=VCC}
N 190 -150 190 -130 {lab=0}
C {opin.sym} 230 -230 0 0 {name=p1 lab=y verilog_type=wire}
C {ipin.sym} 110 -230 0 0 {name=p2 lab=a}
C {p.sym} 170 -280 0 0 {name=m2 model=cmosp w=wp l=lp  m=1 }
C {lab_pin.sym} 190 -350 0 0 {name=p149 lab=VCC}
C {lab_pin.sym} 190 -130 0 0 {name=p3 lab=0}
C {n.sym} 170 -180 0 0 {name=m1 model=cmosn w=wn l=lln m=1}
C {title.sym} 160 0 0 0 {name=l3 author="Stefan Schippers"}
