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
B 2 520 -415 630 -315 {name=l4
flags=graph,unlocked
lock=1
color=8
node="tcleval([xschem get_fqdevice [xschem translate l4 @device]])"
y1=-0.00014
y2=0.00013
x1=0.00044464632
x2=0.00046163671}
T {( @#0:resolved_net )} 440 -265 0 1 0.2 0.2 {name=l2 layer=15}
T {( @#0:resolved_net )} 100 -285 0 1 0.2 0.2 {name=p1 layer=15}
T {( @#0:resolved_net )} 100 -235 0 1 0.2 0.2 {name=p2 layer=15}
T {( @#0:resolved_net )} 690 -265 0 1 0.2 0.2 {name=p3 layer=15}
T {( @#0:resolved_net )} 250 -285 0 1 0.2 0.2 {name=l6 layer=15}
T {( @#0:resolved_net )} 250 -235 0 1 0.2 0.2 {name=l1 layer=15}
N 450 -240 450 -180 { lab=IOUT}
N 450 -240 540 -240 { lab=IOUT}
N 600 -240 680 -240 { lab=OUT}
C {ipin.sym} 120 -260 0 0 {name=p1 lab=PLUS}
C {ipin.sym} 120 -210 0 0 {name=p2 lab=MINUS}
C {opin.sym} 680 -240 0 0 {name=p3 lab=OUT}
C {lab_pin.sym} 180 -260 0 1 {name=l6 sig_type=std_logic lab=IPLUS}
C {lab_pin.sym} 180 -210 0 1 {name=l1 sig_type=std_logic lab=IMINUS}
C {lab_pin.sym} 450 -240 0 0 {name=l2 sig_type=std_logic lab=IOUT}
C {lab_pin.sym} 450 -120 0 0 {name=p35 lab=0}
C {bsource.sym} 450 -150 0 1 {name=B1 VAR=V FUNC="\{OFFSET + AMPLITUDE/2*(tanh(V(IPLUS,IMINUS)*GAIN*2/AMPLITUDE))\}"
}
C {title.sym} 160 -30 0 0 {name=l3 author="Stefan Schippers"}
C {res.sym} 570 -240 1 0 {name=R1
value=\{ROUT\}
m=1}
C {parax_cap.sym} 630 -230 0 0 {name=C3 gnd=0 value=\{COUT\} m=1}
C {vsource.sym} 150 -260 1 0 {name=V1 value=0}
C {vsource.sym} 150 -210 1 0 {name=V2 value=0}
C {scope_ammeter.sym} 500 -290 0 0 {name=l4
attach=l4
device=R1}
