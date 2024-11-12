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
B 2 20 -380 660 -110 {flags=graph
y1=0
y2=3
ypos1=0.0732701
ypos2=1.90323
divy=5
subdivy=1
unity=1
x1=-5.76971e-09
x2=2.9422e-07
divx=5
subdivx=1
node="in
in_inv
in_buf
in_inv2
in_buf2"
color="7 4 8 9 10"
dataset=-1
unitx=1
logx=0
logy=0
digital=1}
T {The two below symbols are
created by a 'symbolgen' script
that takes a 'buf' or 'inv'
argument. Schematic also
created by a 'schematicgen'
script.} 30 -830 0 0 0.5 0.5 {}
T {Click on symbol with Control key
pressed to see the generator script} 90 -620 0 0 0.3 0.3 {}
T {The two below symbols have a
schematic created by a
'schematicgen' script
that takes a 'buf' or 'inv'
argument.} 830 -830 0 0 0.5 0.5 {}
T {Click on symbol with Control key
pressed to see the generator script} 900 -620 0 0 0.3 0.3 {}
N 30 -560 30 -520 {
lab=IN}
N 30 -560 110 -560 {
lab=IN}
N 30 -520 30 -460 {
lab=IN}
N 30 -460 110 -460 {
lab=IN}
N 190 -560 310 -560 {
lab=IN_BUF}
N 190 -460 310 -460 {
lab=IN_INV}
N 1000 -460 1120 -460 {
lab=IN_INV2}
N 1000 -560 1120 -560 {
lab=IN_BUF2}
N 620 -470 620 -440 {
lab=VCC}
N 840 -560 840 -520 {
lab=IN}
N 840 -560 920 -560 {
lab=IN}
N 840 -520 840 -460 {
lab=IN}
N 840 -460 920 -460 {
lab=IN}
N 470 -640 520 -640 {
lab=IN}
N 600 -640 660 -640 {
lab=IN_BUF3}
C {symbolgen.tcl(inv,@ROUT\\)} 150 -460 0 0 {name=x1  
tclcommand="edit_file [abs_sym_path symbolgen.tcl]"
ROUT=1200}
C {lab_pin.sym} 30 -520 0 0 {name=p1 lab=IN}
C {symbolgen.tcl(buf,@ROUT\\)} 150 -560 0 0 {name=x3  
tclcommand="edit_file [abs_sym_path symbolgen.tcl]"
ROUT=1200}
C {lab_pin.sym} 310 -560 0 1 {name=p2 lab=IN_BUF}
C {lab_pin.sym} 310 -460 0 1 {name=p3 lab=IN_INV}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {code_shown.sym} 730 -370 0 0 {name=CONTROL 
tclcommand="xschem edit_vi_prop"
xxplace=end
value=".include models_rom8k.txt
.param vcc=3
vvcc vcc 0 dc 3
Vin in 0 pwl 0 0 100n 0 100.1n 3 200n 3 200.1n 0
.control
  save all
  tran 1n 300n uic
  write test_symbolgen.raw
.endc
"}
C {parax_cap.sym} 240 -550 0 0 {name=C1 gnd=0 value=100f m=1}
C {parax_cap.sym} 240 -450 0 0 {name=C2 gnd=0 value=100f m=1}
C {launcher.sym} 80 -80 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/test_symbolgen.raw tran"
}
C {my_inv.sym} 960 -460 0 0 {name=x2 ROUT=1000
schematic=schematicgen.tcl(inv)
tclcommand="edit_file [abs_sym_path schematicgen.tcl]"}
C {lab_pin.sym} 1120 -460 0 1 {name=p5 lab=IN_INV2}
C {my_inv.sym} 960 -560 0 0 {name=x4 ROUT=1000
schematic="schematicgen.tcl(buf,4)"
tclcommand="edit_file [abs_sym_path schematicgen.tcl]"}
C {lab_pin.sym} 1120 -560 0 1 {name=p7 lab=IN_BUF2}
C {vdd.sym} 620 -470 0 0 {name=l2 lab=VCC}
C {lab_pin.sym} 620 -440 0 1 {name=p8 lab=VCC}
C {lab_pin.sym} 840 -520 0 0 {name=p9 lab=IN}
C {parax_cap.sym} 1080 -550 0 0 {name=C3 gnd=0 value=100f m=1}
C {parax_cap.sym} 1080 -450 0 0 {name=C4 gnd=0 value=100f m=1}
C {noconn.sym} 840 -560 0 0 {name=l3}
C {symbolgen.tcl()} 560 -640 0 0 {name=x5  
tclcommand="edit_file [abs_sym_path symbolgen.tcl]"
ROUT=1200}
C {lab_pin.sym} 470 -640 0 0 {name=p4 lab=IN}
C {lab_pin.sym} 660 -640 0 1 {name=p6 lab=IN_BUF3}
