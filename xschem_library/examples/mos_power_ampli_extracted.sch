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
S {xm2 net1 GB SB irf540 m=1
R7 GB SB 190 m=1
xm1 VPP GA SA irf540 m=1
R0 GA SA 190 m=1
R2 VPP E2 50 m=1
R3 VBOOST E1 50 m=1
Q5 C5 PLUS E9 q2n2222 area=1 m=1
Q4 net8 net2 E4 q2n2907p area=1 m=1
R9 VBOOST E4 50 m=1
Q6 C6 C2 net5 q2n2907p area=1 m=1
R4 VPP E6 50 m=1
vd OUTI net1 0
.save  i(vd)
vu SA OUTI 0
.save  i(vu)
Q1 net2 net2 E1 q2n2907p area=1 m=1
Q2 C2 C2 net4 q2n2907p area=1 m=1
Q9 C9 MINUS E9 q2n2222 area=1 m=1
R11 GB net6 1300 m=1
Q8 C8 VSS E8 q2n2907p area=1 m=1
C12 VBOOST net3 40u m=1
D0 VPP VBOOST d1n4148 area=1
R18 net3 OUTI 200 m=1
D1 VPP VBOOST d1n758
R14 B1 net7 4k m=1
Q11 GA B1 E11 q2n2907p area=1 m=1
R15 VBOOST B1 4k m=1
R5 E9 net9 120 m=1
R6 net9 net10 120 m=1
D2 VSS B3 d1n755
Q3 C3 B3 E3 q2n2222 area=1 m=1
R1 VPP B3 10k m=1
R10 E3 VSS 170 m=1
C3 B3 VSS 100n m=1
R12 C6 E8 1300 m=1
R13 C2 C9 300 m=1
Q7 C7 C9 C5 q2n2222 area=1 m=1
v0 SB VNN 0
.save  i(v0)
v1 E6 net5 0
.save  i(v1)
v2 E2 net4 0
.save  i(v2)
v3 net10 C3 0
.save  i(v3)
v4 C8 net6 0
.save  i(v4)
v5 net7 OUTI 0
.save  i(v5)
v6 net2 C7 0
.save  i(v6)
v7 net8 E11 0
.save  i(v7)
.save  v(gb)
.save  v(sb)
.save  v(ga)
.save  v(sa)
v8 OUTI OUT 0
.save  i(v8)
.save  v(ga)
.save  v(gb)
.save  v(c2)
.save  v(net2)
.save  v(c7)
.save  v(vboost)
.save  v(outi)}
E {}
T {The netlist is saved in the global
schematic attribute.
To see the netlist press 'q' (with no
selected items) or menu Properties->Edit} 130 -460 0 0 0.4 0.4 {}
C {ipin.sym} 140 -190 0 0 {name=p0 lab=PLUS}
C {ipin.sym} 140 -150 0 0 {name=p2 lab=VPP}
C {ipin.sym} 140 -130 0 0 {name=p3 lab=VNN}
C {title.sym} 160 -30 0 0 {name=l2 author="Stefan Schippers"}
C {opin.sym} 210 -160 0 0 {name=p5 lab=OUT}
C {ipin.sym} 140 -210 0 0 {name=p1 lab=MINUS}
C {ipin.sym} 140 -170 0 0 {name=p4 lab=VSS}
C {noconn.sym} 140 -210 2 0 {name=l1}
C {noconn.sym} 140 -190 2 0 {name=l3}
C {noconn.sym} 140 -170 2 0 {name=l4}
C {noconn.sym} 140 -150 2 0 {name=l5}
C {noconn.sym} 140 -130 2 0 {name=l6}
C {noconn.sym} 210 -160 2 1 {name=l7}
