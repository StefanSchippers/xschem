v {xschem version=3.4.6 file_version=1.2
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
T {Control + left mouse button on
the symbol to see the 
symbol_include.cir file} 290 -130 0 0 0.3 0.3 {}
T {Example of symbol with associated spice .subckt file.
The pin order will be taken from the spice file.} 10 -380 0 0 0.4 0.4 {}
C {lab_pin.sym} 260 -230 0 0 {name=p1 lab=XA[9:7]}
C {lab_pin.sym} 260 -210 0 0 {name=p2 lab=XB}
C {lab_pin.sym} 260 -190 0 0 {name=p3 lab=XC}
C {lab_pin.sym} 560 -230 0 1 {name=p4 lab=XZ}
C {lab_pin.sym} 260 -170 0 0 {name=p5 lab=XVCC
}
C {lab_pin.sym} 260 -150 0 0 {name=p6 lab=xvss}
C {opin.sym} 120 -70 0 0 { name=p7 lab=XY[7:4]}
C {ipin.sym} 80 -70 0 0 { name=p8 lab=xvss }
C {ipin.sym} 80 -90 0 0 { name=p9 lab=XVCC }
C {ipin.sym} 80 -110 0 0 { name=p10 lab=XC }
C {ipin.sym} 80 -130 0 0 { name=p11 lab=XB }
C {ipin.sym} 80 -150 0 0 { name=p12 lab=XA[9:7]}
C {lab_pin.sym} 560 -210 0 1 {name=p13 lab=XY[7:4]}
C {opin.sym} 120 -90 0 0 { name=p14 lab=XZ }
C {SYMBOL_include.sym} 410 -190 0 0 {name=x2
tclcommand="textwindow [xschem get current_dirname]/symbol_include.cir"

comm="following instance attributes (now 'x' commented) specify an alternate
      symbol reference to use in netlist"
xspice_sym_def=".include symbol_include2.cir"
xschematic="symbol_include2.sch"}
