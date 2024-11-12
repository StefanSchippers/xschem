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
V {}
S {}
E {}
N 430 -390 430 -370 {lab=VSS}
N 300 -480 320 -480 {lab=LDY1[15:0]}
N 360 -540 360 -510 {lab=LDBL[15:0]}
N 360 -450 430 -450 {lab=LDYMS}
N 360 -450 360 -390 {lab=LDYMS}
C {opin.sym} 490 -250 0 0 {name=p5 lab=LDYMS}
C {lab_pin.sym} 430 -370 0 1 {name=p408 lab=VSS}
C {lab_pin.sym} 300 -480 0 0 {name=p453 lab=LDY1[15:0]}
C {lab_pin.sym} 360 -540 0 1 {name=p664 lab=LDBL[15:0]}
C {nlv.sym} 340 -480 0 0 {name=m96[15:0] model=cmosn w=50u l=2u m=1
}
C {capa.sym} 430 -420 0 0 {name=c61 m=1 value=20f}
C {title.sym} 160 0 0 0 {name=l3 author="Stefan Schippers"}
C {lab_pin.sym} 360 -390 0 0 {name=p8 lab=LDYMS}
C {ipin.sym} 290 -260 0 0 {name=p11 lab=LDY1[15:0]}
C {ipin.sym} 290 -240 0 0 {name=p12 lab=LDBL[15:0]}
C {ipin.sym} 290 -220 0 0 {name=p13 lab=VSS}
