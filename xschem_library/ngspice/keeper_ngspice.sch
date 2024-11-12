v {xschem version=3.4.5 file_version=1.2
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
N 260 -300 290 -300 { lab=A}
N 260 -300 260 -200 { lab=A}
N 260 -200 290 -200 { lab=A}
N 370 -300 400 -300 { lab=Y}
N 400 -300 400 -200 { lab=Y}
N 370 -200 400 -200 { lab=Y}
N 220 -250 260 -250 { lab=A}
C {ipin.sym} 220 -250 0 0 {name=p1 lab=A}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {inv_ngspice.sym} 330 -200 0 0 {name=x1}
C {inv_ngspice.sym} 330 -300 0 1 {name=x2 RUP=300k RDOWN=200k}
C {lab_pin.sym} 400 -250 0 1 {name=l2 sig_type=std_logic lab=Y}
