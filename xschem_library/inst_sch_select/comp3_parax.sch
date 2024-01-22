v {xschem version=3.4.4 file_version=1.2
*
* This file is part of XSCHEM,
* a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
* simulation.
* Copyright (C) 1998-2023 Stefan Frederik Schippers
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
N 100 -310 120 -310 {
lab=PLUS}
N 100 -260 120 -260 {
lab=MINUS}
N 230 -290 230 -260 {
lab=OUT}
C {opin.sym} 230 -290 0 0 {name=p20 lab=OUT}
C {ipin.sym} 100 -260 0 0 {name=p1 lab=MINUS}
C {ipin.sym} 100 -310 0 0 {name=p161 lab=PLUS}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {parax_cap.sym} 120 -300 0 0 {name=C1 gnd=0 value=50f m=1}
C {parax_cap.sym} 120 -250 0 0 {name=C2 gnd=0 value=50f m=1}
C {ammeter.sym} 230 -230 0 0 {name=Vmeas}
C {lab_pin.sym} 230 -200 0 0 {name=p2 sig_type=std_logic lab=0}
