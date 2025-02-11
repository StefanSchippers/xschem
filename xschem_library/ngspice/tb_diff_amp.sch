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
B 2 840 -580 1640 -170 {flags=graph
y1=6.3e-14
y2=5
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=1.1
x2=1.9
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
node="b
a
z"
color="6 4 7"
dataset=-1
unitx=1
logx=0
logy=0
}
B 2 840 -990 1640 -580 {flags=graph
y1=0
y2=100
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=1.1
x2=1.9
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
node="Opamp gain; z deriv()"
color=7
dataset=-1
unitx=1
logx=0
logy=0
}
P 4 5 30 -600 30 -880 740 -880 740 -600 30 -600 {}
P 4 7 410 -600 410 -560 420 -560 410 -540 400 -560 410 -560 410 -600 {}
T {// importing libs
`include "discipline.h"

module diff_amp(
  output electrical out,
  input electrical in1,
  input electrical in2);

(* desc="gain", units="", type="instance" *)   parameter real gain = 40 from [-inf: inf];
(* desc="amplitude", units="", type="instance" *)    parameter real amplitude = 3 from [-inf: inf];
(* desc="offset", units="", type="instance" *) parameter real offset = 1.5 from [-inf: inf];

analog begin

    V(out) <+ offset + amplitude / 2 * tanh( gain / amplitude * 2 * V(in1, in2));

end
endmodule
} 40 -870 0 0 0.2 0.2 {font=monospace}
T {create a diff_amp.va file with following code 
and compile it into a .osdi file with openvaf.} 190 -940 0 0 0.4 0.4 {}
N 180 -450 320 -450 {lab=B}
N 80 -530 320 -530 {lab=A}
N 520 -490 640 -490 {lab=Z}
N 60 -290 180 -290 {lab=0}
N 180 -330 180 -290 {lab=0}
N 80 -330 80 -290 {lab=0}
N 80 -530 80 -390 {lab=A}
N 180 -450 180 -390 {lab=B}
C {lab_pin.sym} 640 -490 0 1 {name=p1 sig_type=std_logic lab=Z}
C {lab_pin.sym} 80 -530 0 0 {name=p2 sig_type=std_logic lab=A}
C {lab_pin.sym} 180 -450 0 0 {name=p3 sig_type=std_logic lab=B}
C {vsource.sym} 80 -360 0 0 {name=V1 value=1.51 savecurrent=false}
C {vsource.sym} 180 -360 0 0 {name=V2 value=1.5 savecurrent=false}
C {lab_pin.sym} 60 -290 0 0 {name=p4 sig_type=std_logic lab=0}
C {code_shown.sym} 240 -320 0 0 {name=COMMANDS only_toplevel=false value="
.options savecurrents
.control
  save all
  op
  remzerovec
  write tb_diff_amp.raw
  dc V1 1.1 1.9 0.001
  set appendwrite
  remzerovec
  write tb_diff_amp.raw
  * quit 0
.endc
"}
C {launcher.sym} 670 -120 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/tb_diff_amp.raw dc"
}
C {title.sym} 160 -30 0 0 {name=l1 author="Phillip Baade-Pedersen"}
C {launcher.sym} 670 -170 0 0 {name=h1
descr="OP annotate" 
tclcommand="xschem annotate_op"
}
C {diff_amp.sym} 420 -490 0 0 {name=X1 model=diff_amp_cell gain=100 amplitude=5 offset=2.5}
