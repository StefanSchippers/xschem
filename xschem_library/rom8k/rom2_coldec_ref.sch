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
N 650 -550 650 -520 {lab=LDBLREF}
N 650 -460 650 -400 {lab=LDYMSREF}
N 720 -390 720 -370 {lab=VSS}
N 650 -450 720 -450 {lab=LDYMSREF}
N 720 -450 830 -450 {lab=LDYMSREF}
N 830 -460 830 -450 {lab=LDYMSREF}
N 830 -550 830 -520 {lab=#net1}
C {title.sym} 160 0 0 0 {name=l3 author="Stefan Schippers"}
C {nlv.sym} 630 -490 0 0 {name=m1 model=cmosn w=50u l=2u m=1
}
C {lab_pin.sym} 610 -490 0 0 {name=p2 lab=VCC}
C {lab_pin.sym} 650 -550 0 1 {name=p3 lab=LDBLREF}
C {lab_pin.sym} 650 -400 0 0 {name=p6 lab=LDYMSREF}
C {opin.sym} 320 -150 0 0 {name=p7 lab=LDYMSREF}
C {ipin.sym} 120 -170 0 0 {name=p10 lab=LDBLREF}
C {ipin.sym} 120 -150 0 0 {name=p14 lab=VCC}
C {ipin.sym} 120 -130 0 0 {name=p1 lab=VSS}
C {lab_pin.sym} 720 -370 0 1 {name=p408 lab=VSS}
C {capa.sym} 720 -420 0 0 {name=c61 m=1 value=20f}
C {nlv.sym} 810 -490 0 0 {name=m2[15:1] model=cmosn w=50u l=2u m=1
}
C {lab_pin.sym} 790 -490 0 0 {name=p4 lab=VSS}
