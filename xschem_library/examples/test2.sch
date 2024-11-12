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
N 100 -800 150 -800 {lab=#net1}
N 150 -800 150 -780 {lab=#net1}
N 150 -780 210 -780 {lab=#net1}
C {nand2.sym} 260 -760 0 0 {name=x1 WP=12u LP=0.4u WN=8u LN=0.6u}
C {nand2.sym} 50 -800 0 0 {name=x2 WP=5u LP=1u WN=3u LN=1.5u}
C {lab_pin.sym} 0 -820 0 0 {name=p1 lab=A}
C {lab_pin.sym} 0 -780 0 0 {name=p2 lab=B}
C {lab_pin.sym} 310 -760 0 1 {name=p3 lab=Z}
C {lab_pin.sym} 210 -740 0 0 {name=p4 lab=C}
