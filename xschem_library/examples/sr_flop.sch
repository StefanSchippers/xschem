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
N 780 -320 780 -280 {lab=#net1}
N 750 -280 780 -280 {lab=#net1}
N 780 -390 780 -360 {lab=#net2}
N 750 -390 780 -390 {lab=#net2}
N 300 -260 300 -220 {lab=#net3}
N 300 -410 300 -370 {lab=#net4}
N 300 -410 380 -410 {lab=#net4}
N 380 -410 560 -410 {lab=#net4}
N 560 -410 560 -380 {lab=#net4}
N 560 -410 650 -410 {lab=#net4}
N 560 -380 560 -300 {lab=#net4}
N 560 -300 650 -300 {lab=#net4}
N 620 -370 620 -260 {lab=#net5}
N 620 -370 650 -370 {lab=#net5}
N 880 -340 930 -340 {lab=Q}
N 390 -370 390 -320 {lab=#net6}
N 120 -320 390 -320 {lab=#net6}
N 120 -320 120 -260 {lab=#net6}
N 380 -370 390 -370 {lab=#net6}
N 310 -300 310 -260 {lab=#net3}
N 50 -300 310 -300 {lab=#net3}
N 50 -410 50 -300 {lab=#net3}
N 50 -410 120 -410 {lab=#net3}
N 300 -260 310 -260 {lab=#net3}
N 380 -220 580 -220 {lab=#net5}
N 580 -260 580 -220 {lab=#net5}
N 580 -260 650 -260 {lab=#net5}
C {ipin.sym} 280 -130 0 0 {name=p0 lab=S}
C {opin.sym} 580 -130 0 0 {name=p2 lab=Q}
C {ipin.sym} 280 -80 0 0 {name=p4 lab=VSS}
C {ipin.sym} 280 -110 0 0 {name=p1 lab=R}
C {flop.sym} 210 -400 0 0 {name=x1 VTH=VTH VHI=VHI}
C {flop.sym} 210 -250 0 0 {name=x2 VTH=VTH VHI=VHI}
C {lab_pin.sym} 340 -190 0 0 {name=p13 lab=VSS}
C {lab_pin.sym} 120 -220 0 0 {name=p3 lab=VSS}
C {lab_pin.sym} 120 -370 0 0 {name=p5 lab=VSS}
C {an2.sym} 690 -390 0 0 {name=E2 TABLE="'VTH-0.1' 0.0 'VTH+0.1' VHI"}
C {nr2-1.sym} 690 -280 0 0 {name=E3 TABLE="'VTH-0.1' VHI 'VTH+0.1' 0.0"}
C {or2.sym} 820 -340 0 0 {name=E4 TABLE="'VTH-0.1' 0.0 'VTH+0.1' VHI"}
C {lab_pin.sym} 690 -240 0 0 {name=p6 lab=VSS}
C {lab_pin.sym} 690 -350 0 0 {name=p7 lab=VSS}
C {lab_pin.sym} 820 -300 0 0 {name=p8 lab=VSS}
C {lab_pin.sym} 340 -340 0 0 {name=p9 lab=VSS}
C {lab_pin.sym} 930 -340 0 1 {name=l2 lab=Q}
C {lab_pin.sym} 120 -390 0 0 {name=l3 lab=R}
C {lab_pin.sym} 120 -240 0 0 {name=l4 lab=S}
C {inv_bsource.sym} 340 -370 0 0 {name=B5 TABLE="'VTH-0.1' VHI 'VTH+0.1' 0"}
C {inv_bsource.sym} 340 -220 0 0 {name=B1 TABLE="'VTH-0.1' VHI 'VTH+0.1' 0"}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
