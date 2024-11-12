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
B 2 970 -610 1960 -220 {flags=graph
y1=-93
y2=1.3
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=-1.00075
x2=7.45087
divx=10
subdivx=8
node="\\"out db20()\\""
color=4
dataset=-1
unitx=1
logx=1
logy=0
}
N 100 -550 100 -520 {lab=IN}
N 650 -510 670 -510 {lab=OUT}
N 270 -550 430 -550 {lab=PLUS}
N 190 -550 210 -550 {lab=#net1}
N 200 -600 200 -550 {lab=#net1}
N 200 -660 650 -660 {lab=OUT}
N 650 -660 650 -510 {lab=OUT}
N 630 -510 650 -510 {lab=OUT}
N 650 -510 650 -360 {lab=OUT}
N 410 -360 650 -360 {lab=OUT}
N 410 -470 410 -360 {lab=OUT}
N 410 -470 430 -470 {lab=OUT}
N 850 -540 850 -520 {lab=0}
N 800 -530 850 -530 {lab=0}
N 100 -550 130 -550 {lab=IN}
C {code_shown.sym} 20 -320 0 0 {name=CONTROL vhdl_ignore=true place=end value=".control
  listing e
  save all
  run
  write hpf.raw
  let response = db(v(out)/v(in))
  settype decibel response
  plot xlog response
  gnuplot hpf response xlog
.endc
"}
C {code_shown.sym} 390 -310 0 0 {name=DIRECTIVES vhdl_ignore=true place=end value=".ac oct 100 0.1 100meg
.param pi   = 3.1415926
.param Q    = 1
.param freq = 20 
.param w0   = 2.0*pi*freq
.param k    = 4*Q**2
.param R    = 10k
.param C    = 1/(sqrt(k)*R*w0)
"}
C {title.sym} 160 -30 0 0 {name=l7 author="Stefan Schippers"}
C {vsource.sym} 100 -490 0 0 {name=v2 value="ac 1"}
C {gnd.sym} 100 -460 0 0 {name=l14 lab=0}
C {capa.sym} 240 -550 1 1 {name=C1 m=1 value=C footprint=1206 device="ceramic capacitor"}
C {lab_wire.sym} 400 -550 0 0 {name=l8 sig_type=std_logic lab=PLUS}
C {res.sym} 340 -520 0 0 {name=R6 value='k*R' footprint=1206 device=resistor m=1}
C {gnd.sym} 340 -490 0 0 {name=l2 lab=0}
C {vsource.sym} 850 -570 0 0 {name=v6 value=12}
C {vsource.sym} 850 -490 0 0 {name=v1 value=12}
C {lm741.sym} 530 -510 0 0 {name=X1 model=LM741 device=LM741 footprint="SO(8)"
}
C {code.sym} 760 -290 0 0 {name=MODELS vhdl_ignore=true value="*//////////////////////////////////////////////////////////
*LM741 OPERATIONAL AMPLIFIER MACRO-MODEL
*//////////////////////////////////////////////////////////
*
* connections:      non-inverting input
*                   |   inverting input
*                   |   |   positive power supply
*                   |   |   |   negative power supply
*                   |   |   |   |   output
*                   |   |   |   |   |
*                   |   |   |   |   |
.SUBCKT LM741    1   2  99  50  28
*
*Features:
*Improved performance over industry standards
*Plug-in replacement for LM709,LM201,MC1439,748
*Input and output overload protection
*
****************INPUT STAGE**************
*
IOS 2 1 20N
*^Input offset current
R1 1 3 250K
R2 3 2 250K
I1 4 50 100U
R3 5 99 517
R4 6 99 517
Q1 5 2 4 QX
Q2 6 7 4 QX
*Fp2=2.55 MHz
C4 5 6 60.3614P
*
***********COMMON MODE EFFECT***********
*
I2 99 50 1.6MA
*^Quiescent supply current
EOS 7 1 POLY(1) 16 49 1E-3 1
*Input offset voltage.^
R8 99 49 40K
R9 49 50 40K
*
*********OUTPUT VOLTAGE LIMITING********
V2 99 8 1.63
D1 9 8 DX
D2 10 9 DX
V3 10 50 1.63
*
**************SECOND STAGE**************
*
EH 99 98 99 49 1
G1 98 9 5 6 2.1E-3
*Fp1=5 Hz
R5 98 9 95.493MEG
C3 98 9 333.33P
*
***************POLE STAGE***************
*
*Fp=30 MHz
G3 98 15 9 49 1E-6
R12 98 15 1MEG
C5 98 15 5.3052E-15
*
*********COMMON-MODE ZERO STAGE*********
*
*Fpcm=300 Hz
G4 98 16 3 49 3.1623E-8
L2 98 17 530.5M
R13 17 16 1K
*
**************OUTPUT STAGE**************
*
F6 50 99 POLY(1) V6 450U 1
E1 99 23 99 15 1
R16 24 23 25
D5 26 24 DX
V6 26 22 0.65V
R17 23 25 25
D6 25 27 DX
V7 22 27 0.65V
V5 22 21 0.18V
D4 21 15 DX
V4 20 22 0.18V
D3 15 20 DX
L3 22 28 100P
RL3 22 28 100K
*
***************MODELS USED**************
*
.MODEL DX D(IS=1E-15)
.MODEL QX NPN(BF=625)
*
.ENDS
"}
C {lab_pin.sym} 100 -550 0 0 {name=l23 sig_type=std_logic lab=IN}
C {res.sym} 200 -630 0 0 {name=R7 value=R footprint=1206 device=resistor m=1}
C {capa.sym} 160 -550 1 1 {name=C2 m=1 value=C footprint=1206 device="ceramic capacitor"}
C {gnd.sym} 530 -450 0 0 {name=l1 lab=VEE}
C {vdd.sym} 530 -570 0 0 {name=l5 lab=VCC}
C {gnd.sym} 850 -460 0 0 {name=l3 lab=VEE}
C {vdd.sym} 850 -600 0 0 {name=l4 lab=VCC}
C {gnd.sym} 800 -530 0 0 {name=l6 lab=0}
C {opin.sym} 670 -510 0 0 {name=p9 lab=OUT}
C {launcher.sym} 1115 -175 0 0 {name=h5 
descr="load ngspice waves" 
tclcommand="
xschem raw_read $netlist_dir/hpf.raw ac
"
}
