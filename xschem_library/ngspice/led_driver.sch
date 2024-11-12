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
L 8 580 -600 580 -580 {}
L 8 560 -570 600 -570 {}
L 8 580 -570 580 -550 {}
P 8 5 580 -580 570 -580 580 -570 590 -580 580 -580 {fill=true}
T {1W white LED} 800 -360 0 0 0.4 0.4 {layer=8}
T {IDEAL Diode} 520 -540 0 0 0.4 0.4 {layer=8}
N 30 -240 60 -240 {lab=0}
N 60 -280 60 -240 {lab=0}
N 690 -240 780 -240 {lab=0}
N 780 -280 780 -240 {lab=0}
N 60 -640 60 -340 {lab=VCC}
N 780 -470 780 -340 {lab=VLED}
N 380 -640 410 -640 {lab=SW}
N 410 -640 410 -320 {lab=SW}
N 410 -260 410 -240 {lab=0}
N 690 -470 720 -470 {lab=VO}
N 410 -640 690 -640 {lab=SW}
N 170 -280 170 -240 {lab=0}
N 170 -400 370 -400 {lab=CTRL1}
N 170 -400 170 -340 {lab=CTRL1}
N 290 -640 320 -640 {lab=#net1}
N 690 -470 690 -340 {lab=VO}
N 690 -280 690 -240 {lab=0}
N 170 -240 410 -240 {lab=0}
N 410 -240 690 -240 {lab=0}
N 60 -240 170 -240 {lab=0}
N 210 -640 230 -640 {lab=#net2}
N 60 -640 150 -640 {lab=VCC}
N 370 -400 370 -290 {lab=CTRL1}
N 690 -550 690 -470 {lab=VO}
N 690 -640 690 -610 {lab=SW}
N 880 -730 880 -700 {lab=COMP}
C {title.sym} 160 -40 0 0 {name=l1 author="Stefan Schippers"}
C {isource_table.sym} 780 -310 0 0 {name=G1 CTRL="V(VLED)" TABLE="
+ (0, 0)
+ (2.4, 5m)
+ (2.6, 15m)
+ (2.8, 46m)
+ (2.9, 80m)
+ (3.0, 115m)
+ (3.1, 157m)
+ (3.2, 202m)
+ (3.3, 245m)
+ (3.4, 290m)
+ (3.5, 337m)
+ (3.6, 395m)
+ (3.7, 470m)
+ (4.0, 750m)"
}
C {vsource.sym} 60 -310 0 0 {name=V1 value="pwl 0 0 1u 2"}
C {lab_pin.sym} 30 -240 0 0 {name=l2 sig_type=std_logic lab=0}
C {lab_pin.sym} 60 -640 0 0 {name=l3 sig_type=std_logic lab=VCC}
C {code_shown.sym} 1050 -240 0 0 {name=s1 value="* .control
* save all
* write led_driver.raw
* tran 5n 1000u uic
* .endc
* .save all
.tran 5n 1000u uic
"}
C {ammeter.sym} 750 -470 3 0 {name=VVled}
C {code.sym} 1040 -420 0 0 {name=MODELS value=".MODEL DIODE D(IS=1.139e-08 RS=0.99 CJO=9.3e-12 VJ=1.6 M=0.411 BV=30 EG=0.7 ) 
.MODEL swmod SW(VT=0.1 VH=0.01 RON=0.01 ROFF=10000000)
"}
C {switch_ngspice.sym} 410 -290 0 0 {name=S2 model=swmod}
C {ind.sym} 350 -640 3 1 {name=L1
m=1
value=40u
footprint=1206
device=inductor}
C {lab_pin.sym} 780 -470 0 1 {name=l6 sig_type=std_logic lab=VLED}
C {vsource.sym} 170 -310 0 0 {name=Vset value="pulse 0 1 0 1n 1n 2.1u 5u"}
C {lab_pin.sym} 170 -400 0 0 {name=l7 sig_type=std_logic lab=CTRL1}
C {lab_pin.sym} 410 -370 0 1 {name=l5 sig_type=std_logic lab=SW}
C {lab_pin.sym} 370 -270 0 0 {name=l4 sig_type=std_logic lab=0}
C {res.sym} 260 -640 1 0 {name=R1
value=0.01
footprint=1206
device=resistor
m=1}
C {capa.sym} 690 -310 0 0 {name=C1
m=1
value=10u
footprint=1206
device="ceramic capacitor"}
C {ammeter.sym} 180 -640 3 0 {name=Vvcc}
C {lab_pin.sym} 690 -430 0 1 {name=l8 sig_type=std_logic lab=VO}
C {res.sym} 690 -580 0 0 {name=R2
value="r='V(SW,VO) > 0 ? 0.1 : 1e7'"
footprint=1206
device=resistor
m=1
}
C {bsource.sym} 880 -670 0 0 {name=B1 VAR=V FUNC="pwl(V(VLED,VCC), 
+ -0.006, 0,
+ -0.005, 0, 
+ -0.001, 0.5, 
+ 0.001, 4.5, 
+ 0.005, 5,
+ 0.006, 5)"
}
C {lab_pin.sym} 880 -640 0 0 {name=l9 sig_type=std_logic lab=0}
C {lab_pin.sym} 880 -730 0 0 {name=l10 sig_type=std_logic lab=COMP}
C {spice_probe.sym} 780 -470 0 0 {name=p1 analysis=tran}
C {spice_probe.sym} 90 -640 0 0 {name=p2 analysis=tran}
C {spice_probe.sym} 410 -460 0 0 {name=p3 analysis=tran}
C {spice_probe.sym} 290 -400 0 0 {name=p4 analysis=tran}
C {title-2.sym} -170 100 0 0 {name=l11 author="Stefan Schippers" rev=1.0 lock=false}
