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
B 2 680 -520 1290 -250 {flags=graph
y1=0
y2=5
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=5e-08
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
node="in
out"
color="4 7"
dataset=-1
unitx=1
logx=0
logy=0
}
T {To run this example a 'calc_rc' procedure must be defined. 
I have embedded below procedure in title instance so
it will be defined on load/drawing
Descend into the symbol ('i' key) and see how it is used in the 'format'
attribute to calculate "Res" and "Cap" subcircuit parameters from W and L
instance parameters.
Tcl scripts can be loaded in xcshemrc by appending a tcl file name to variable
'tcl_files'.} 20 -520 2 1 0.5 0.5 {layer=8 }
T {proc calc_rc \{ L W \} \{
 if \{[catch \{expr 1200*$L/$W\} res]\} \{set res -1\}
 if \{[catch \{expr 1e-3*$W*$L\} cap]\} \{set cap -1\}
 return "Res=$res Cap=$cap"
\} } 10 -500 0 0 0.4 0.4 { font=monospace}
T {Dynamic calculation of subcircuit parameters} 30 -840 0 0 0.8 0.8 {}
T {Title symbol has embedded TCL command
define calc_rc} 330 -110 0 0 0.4 0.4 { layer=6}
N 140 -130 140 -100 { lab=0}
N 140 -210 160 -210 { lab=IN}
N 140 -210 140 -190 { lab=IN}
C {lab_pin.sym} 680 -170 0 1 {name=p1 lab=OUT}
C {lab_pin.sym} 380 -170 0 0 {name=p2 lab=IN}
C {title.sym} 160 -30 0 0 {name=l1 author="tcleval([
if \{ [info commands calc_rc] eq \{\} \} \{
    puts \{defining calc_rc procedure...\}
    puts \{############\}
    proc calc_rc \{ L W \} \{
     if \{[catch \{expr 1200*$L/$W\} res]\} \{set res -1\}
     if \{[catch \{expr 1e-3*$W*$L\} cap]\} \{set cap -1\}
     return \\"Res=$res Cap=$cap\\"
    \}
    puts [info body calc_rc]
    puts \{############\}
\}
]Stefan Schippers)"
}
C {vsource.sym} 140 -160 0 0 {name=V1 value="pwl 0 0 10n 0 11n 5"}
C {lab_pin.sym} 140 -100 0 0 {name=l2 sig_type=std_logic lab=0}
C {lab_pin.sym} 160 -210 0 1 {name=p3 lab=IN}
C {rcline.sym} 530 -170 0 0 {name=x1 L=1e-4 W=0.5e-6
}
C {code_shown.sym} 830 -190 0 0 {name=STIMULI
only_toplevel=false 

value="
.control
  save all
  tran 0.1n 50n
  write test_backannotated_subckt.raw
.endc
"}
C {launcher.sym} 1050 -230 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/test_backannotated_subckt.raw tran"
}
