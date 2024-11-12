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
T {LM324 OPAMP EXAMPLE} 450 -510 0 0 0.4 0.4 {}
N 520 -340 560 -340 {lab=P}
N 640 -400 640 -370 {lab=VCC}
N 640 -230 640 -200 {lab=VSS}
N 770 -300 960 -300 {lab=Q}
N 900 -300 900 -280 {lab=Q}
N 820 -300 820 -280 {lab=Q}
N 770 -300 770 -150 {lab=Q}
N 560 -150 770 -150 {lab=Q}
N 560 -260 560 -150 {lab=Q}
C {code.sym} 320 -210 0 0 {name=STIMULI value="

.option RUNLVL=5 post
.param VCC=10
.param VSS=0
.temp  25
vvcc vcc 0 dc VCC
vvss vss 0 dc VSS
vp p 0 pwl 0 0 100n 2 200u 2 201u 3 400u 3 401u 1

** models are generally not free: you must download
** SPICE models for active devices and put them  into the below 
** referenced file in netlist/simulation directory.
.include model_lm324.txt
.tran 0.1u 500u
"}
C {lm324.sym} 640 -300 0 0 {name=x1}
C {lab_pin.sym} 640 -200 0 0 {name=p10 lab=VSS}
C {lab_pin.sym} 640 -400 0 0 {name=p1 lab=VCC}
C {lab_pin.sym} 960 -300 0 1 {name=p2 lab=Q}
C {lab_pin.sym} 520 -340 0 0 {name=p3 lab=P}
C {res.sym} 900 -250 0 0 {name=r1 m=1 value=10k}
C {capa.sym} 820 -250 0 0 {name=c1 m=1 value=1n}
C {lab_pin.sym} 820 -220 0 0 {name=p5 lab=VSS}
C {lab_pin.sym} 900 -220 0 0 {name=p6 lab=VSS}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {noconn.sym} 520 -340 1 0 {name=l2}
C {noconn.sym} 640 -370 2 0 {name=l3}
