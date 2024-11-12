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
N 370 -180 420 -180 { lab=OUT}
N 270 -180 310 -180 { lab=#net1}
N 170 -180 210 -180 { lab=IN}
N 290 -180 290 -130 { lab=#net1}
N 290 -70 290 -40 { lab=0}
C {ipin.sym} 170 -180 0 0 {name=p1 lab=IN}
C {opin.sym} 420 -180 0 0 {name=p2 lab=OUT}
C {capa.sym} 290 -100 0 0 {name=C1
m=1
value='Cap/2'
footprint=1206
device="ceramic capacitor"}
C {res.sym} 340 -180 3 0 {name=R1
value='Res/2'
footprint=1206
device=resistor
m=1}
C {res.sym} 240 -180 3 0 {name=R2
value='Res/2'
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 290 -40 0 0 {name=l1 sig_type=std_logic lab=0}
