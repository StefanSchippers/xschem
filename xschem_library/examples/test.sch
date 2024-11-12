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
N 220 -470 260 -470 {lab=SET_BAR}
N 220 -290 260 -290 {lab=CLEAR_BAR}
N 360 -450 390 -450 {lab=Q}
N 360 -310 390 -310 {lab=QBAR}
N 260 -430 260 -410 {lab=QBAR}
N 260 -410 360 -360 {lab=QBAR}
N 360 -360 360 -310 {lab=QBAR}
N 260 -350 260 -330 {lab=Q}
N 260 -350 360 -400 {lab=Q}
N 360 -450 360 -400 {lab=Q}
C {nand2.sym} 310 -450 0 0 {name=x1}
C {lab_pin.sym} 390 -450 0 1 {name=p1 lab=Q}
C {lab_pin.sym} 220 -470 0 0 {name=p2 lab=SET_BAR}
C {lab_pin.sym} 220 -290 0 0 {name=p3 lab=CLEAR_BAR}
C {nand2.sym} 310 -310 2 1 {name=x2}
C {lab_pin.sym} 390 -310 0 1 {name=p4 lab=QBAR}
