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
N 110 -90 110 -20 {lab=m}
N 110 -240 110 -150 {lab=p}
N 70 -150 70 -120 {}
N 70 -150 110 -150 {}
N 70 -100 70 -90 {}
N 70 -90 110 -90 {}
C {iopin.sym} 110 -240 0 0 {name=p0 lab=p}
C {iopin.sym} 110 -20 0 0 {name=p1 lab=m}
C {switch.sym} 110 -120 0 0 {name=G2 TABLE="'VTH-0.1' ROFF 'VTH+0.1' RON"}
