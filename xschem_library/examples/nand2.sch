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
N 160 -310 250 -310 {lab=A}
N 160 -190 210 -190 {lab=B}
N 440 -380 500 -380 {lab=Z}
N 440 -500 440 -470 {lab=VCC}
N 290 -500 440 -500 {lab=VCC}
N 290 -500 290 -470 {lab=VCC}
N 290 -160 290 -130 {lab=VSS}
N 180 -110 290 -110 {lab=VSS}
N 290 -130 290 -110 {lab=VSS}
N 180 -500 290 -500 {lab=VCC}
N 290 -280 290 -220 {lab=#net1}
N 290 -410 290 -340 {lab=Z}
N 290 -380 440 -380 {lab=Z}
N 440 -410 440 -380 {lab=Z}
N 210 -190 250 -190 {lab=B}
N 400 -440 400 -400 {lab=A}
N 240 -400 400 -400 {lab=A}
N 240 -400 240 -310 {lab=A}
N 210 -440 210 -190 {lab=B}
N 210 -440 250 -440 {lab=B}
C {ipin.sym} 160 -310 0 0 {name=p1 lab=A}
C {opin.sym} 500 -380 0 0 {name=p2 lab=Z}
C {ipin.sym} 160 -190 0 0 {name=p3 lab=B}
C {title.sym} 160 -30 0 0 {name=l7 author="Stefan Schippers"}
C {gnd.sym} 180 -110 0 0 {name=l2 lab=VSS}
C {vdd.sym} 180 -500 0 0 {name=l1 lab=VCC}
C {lab_pin.sym} 290 -310 0 1 {name=l3 sig_type=std_logic lab=VSS}
C {lab_pin.sym} 290 -190 0 1 {name=l4 sig_type=std_logic lab=VSS}
C {lab_pin.sym} 290 -440 0 1 {name=l5 sig_type=std_logic lab=VCC}
C {lab_pin.sym} 440 -440 0 1 {name=l6 sig_type=std_logic lab=VCC}
C {nmos4.sym} 270 -310 0 0 {name=m1 model=CMOSN w=WN l=LLN m=1}
C {nmos4.sym} 270 -190 0 0 {name=m1 model=CMOSN w=WN l=LLN m=1}
C {pmos4.sym} 270 -440 0 0 {name=m2 model=CMOSP w=WP l=LP m=1}
C {pmos4.sym} 420 -440 0 0 {name=m2 model=CMOSP w=WP l=LP m=1}
