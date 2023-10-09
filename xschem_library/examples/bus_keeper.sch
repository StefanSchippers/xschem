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
K {type=subcircuit
format="@name @pinlist @symname WN_FB=@WN_FB WP_FB=@WP_FB"
template="name=X1 WN_FB=1u WP_FB=2u"}
V {}
S {}
E {}
P 2 5 250 -930 250 -120 610 -120 610 -930 250 -930 {dash=5}
T {@name} 250 -965 0 0 0.5 0.5 {}
T {@symname} 253.75 -115 0 0 0.5 0.5 {}
N 250 -300 300 -300 {lab=A}
N 280 -690 300 -690 {lab=A}
N 280 -690 280 -300 {lab=A}
N 510 -300 550 -300 {lab=Y}
N 550 -690 550 -300 {lab=Y}
N 510 -690 550 -690 {lab=Y}
N 550 -240 550 -200 { lab=GND}
C {cmos_inv.sch} 240 -70 0 0 {name=X2 WN=15u WP=45u LLN=3u LLP=3u}
C {cmos_inv.sch} 570 -460 0 1 {name=X1 WN=WN_FB WP=WP_FB LLN=3u LLP=3u}
C {iopin.sym} 250 -300 0 1 {name=p1 lab=A}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {capa.sym} 550 -270 0 0 {name=C1
m=1
value=10f
footprint=1206
device="ceramic capacitor"}
C {gnd.sym} 550 -200 0 0 {name=l2 lab=0}
C {lab_pin.sym} 550 -550 0 0 {name=l3 sig_type=std_logic lab=Y}
