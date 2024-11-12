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
N 540 -190 590 -190 {lab=#net1}
N 600 -150 730 -150 {lab=#net2}
N 600 -170 600 -150 {lab=#net2}
N 540 -170 600 -170 {lab=#net2}
N 730 -210 830 -210 {lab=S}
N 400 -230 550 -230 {lab=CIN}
N 550 -230 550 -210 {lab=CIN}
N 550 -210 590 -210 {lab=CIN}
C {ipin.sym} 110 -220 0 0 {name=p1 lab=A}
C {ipin.sym} 110 -190 0 0 {name=p2 lab=B}
C {ipin.sym} 110 -150 0 0 {name=p3 lab=CIN}
C {opin.sym} 210 -220 0 0 {name=p4 lab=S}
C {opin.sym} 210 -190 0 0 {name=p5 lab=COUT}
C {title.sym} 160 -30 0 0 {name=l2}
C {lab_pin.sym} 400 -230 0 0 {name=l8 lab=CIN}
C {lab_pin.sym} 830 -210 0 1 {name=l1 lab=S}
C {lab_pin.sym} 830 -170 0 1 {name=l3 lab=COUT}
C {lab_pin.sym} 400 -190 0 0 {name=p6 lab=A}
C {lab_pin.sym} 400 -170 0 0 {name=p7 lab=B}
C {half_adder_ngspice.sym} 470 -180 0 0 {name=x0}
C {half_adder_ngspice.sym} 660 -200 0 0 {name=x1}
C {or_ngspice.sym} 770 -170 0 0 {name=x2 ROUT=1000}
