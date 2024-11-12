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
L 8 360 -240 360 -220 {}
L 8 340 -250 380 -250 {}
L 8 360 -270 360 -250 {}
P 8 5 360 -240 370 -240 360 -250 350 -240 360 -240 {fill=true}
T {IDEAL Diode} 260 -210 0 0 0.4 0.4 {layer=8}
N 430 -310 430 -270 {
lab=mn}
C {iopin.sym} 430 -150 0 0 {name=p1 lab=plus}
C {iopin.sym} 430 -370 0 0 {name=p2 lab=minus}
C {title.sym} 160 -40 0 0 {name=l1 author="Stefan Schippers"}
C {ammeter.sym} 430 -340 2 0 {name=Vm}
C {ammeter.sym} 430 -180 2 0 {name=Vp}
C {lab_pin.sym} 430 -210 0 1 {name=l2 sig_type=std_logic lab=pn}
C {lab_pin.sym} 430 -290 0 1 {name=l3 sig_type=std_logic lab=mn}
C {bsource.sym} 430 -240 2 1 {name=B1 VAR=I FUNC="'v(pn,mn) > 0 ? v(pn,mn) / Ron : v(pn,mn) / Roff'"}
