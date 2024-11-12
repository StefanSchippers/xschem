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
C {ipin.sym} 110 -220 0 0 {name=p1 lab=A}
C {ipin.sym} 110 -190 0 0 {name=p2 lab=B}
C {opin.sym} 210 -220 0 0 {name=p4 lab=S}
C {opin.sym} 210 -190 0 0 {name=p5 lab=COUT}
C {title.sym} 160 -30 0 0 {name=l2}
C {lab_pin.sym} 470 -270 0 0 {name=l6 lab=A}
C {lab_pin.sym} 470 -230 0 0 {name=l7 lab=B}
C {lab_pin.sym} 570 -160 0 1 {name=p7 lab=COUT}
C {lab_pin.sym} 470 -180 0 0 {name=p3 lab=A}
C {lab_pin.sym} 470 -140 0 0 {name=p6 lab=B}
C {lab_pin.sym} 570 -250 0 1 {name=l3 lab=S}
C {xor_ngspice.sym} 510 -250 0 0 {name=x1 ROUT=1000}
C {and_ngspice.sym} 510 -160 0 0 {name=x2 ROUT=1000}
