v {xschem version=3.4.8RC file_version=1.3
*
* This file is part of XSCHEM,
* a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
* simulation.
* Copyright (C) 1998-2026 Stefan Frederik Schippers
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
F {}
E {}
L 8 750 -700 750 -680 {}
L 8 730 -670 770 -670 {}
L 8 750 -670 750 -650 {}
B 2 1230 -1070 2030 -670 {flags=graph
y1=-1.8e-06
y2=4.3
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=1e-11
x2=0.001
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
legendmag=1.0
node="vled
vcc"
color="4 7"
dataset=-1
unitx=1
logx=0
logy=0
}
B 2 1230 -1490 2030 -1090 {flags=graph
y1=-2.2e-10
y2=3.4
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=1e-11
x2=0.001
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
legendmag=1.0
node="vcc_power; vcc i(vvcc) * 5u ravg()
LED_power; vled i(vvled) * 5u ravg()"
color="4 7"
dataset=-1
unitx=1
logx=0
logy=0
}
P 8 5 750 -680 740 -680 750 -670 760 -680 750 -680 {fill=true}
T {1W white LED} 970 -460 0 0 0.4 0.4 {layer=8}
T {IDEAL Diode} 690 -640 0 0 0.4 0.4 {layer=8}
N 200 -340 230 -340 {lab=0}
N 230 -380 230 -340 {lab=0}
N 860 -340 950 -340 {lab=0}
N 950 -380 950 -340 {lab=0}
N 230 -740 230 -440 {lab=VCC}
N 950 -570 950 -440 {lab=VLED}
N 550 -740 580 -740 {lab=SW}
N 580 -740 580 -420 {lab=SW}
N 580 -360 580 -340 {lab=0}
N 860 -570 890 -570 {lab=VO}
N 580 -740 860 -740 {cap=5f
lab=SW}
N 340 -380 340 -340 {lab=0}
N 340 -500 540 -500 {lab=CTRL1}
N 340 -500 340 -440 {lab=CTRL1}
N 460 -740 490 -740 {lab=#net1}
N 860 -570 860 -440 {lab=VO}
N 860 -380 860 -340 {lab=0}
N 340 -340 580 -340 {lab=0}
N 580 -340 860 -340 {lab=0}
N 230 -340 340 -340 {lab=0}
N 380 -740 400 -740 {lab=#net2}
N 230 -740 320 -740 {lab=VCC}
N 540 -500 540 -390 {lab=CTRL1}
N 860 -650 860 -570 {lab=VO}
N 860 -740 860 -710 {lab=SW}
N 1050 -830 1050 -800 {lab=COMP}
C {title.sym} 330 -140 0 0 {name=l1 author="Stefan Schippers"}
C {isource_table.sym} 950 -410 0 0 {name=G1 CTRL="V(VLED)" TABLE="
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
C {vsource.sym} 230 -410 0 0 {name=V1 value="pwl 0 0 1u 2"}
C {lab_pin.sym} 200 -340 0 0 {name=l2 sig_type=std_logic lab=0}
C {lab_pin.sym} 230 -740 0 0 {name=l3 sig_type=std_logic lab=VCC}
C {code_shown.sym} 1220 -340 0 0 {name=COMMANDS value=".option savecurrents
.control
save all
tran 5n 1000u uic
remzerovec
write led_driver.raw
.endc
"}
C {ammeter.sym} 920 -570 3 0 {name=VVled}
C {code.sym} 1210 -520 0 0 {name=MODELS value=".MODEL DIODE D(IS=1.139e-08 RS=0.99 CJO=9.3e-12 VJ=1.6 M=0.411 BV=30 EG=0.7 ) 
.MODEL swmod SW(VT=0.1 VH=0.01 RON=0.01 ROFF=10000000)
"}
C {switch_ngspice.sym} 580 -390 0 0 {name=S2 model=swmod}
C {ind.sym} 520 -740 3 1 {name=L1
m=1
value=40u
footprint=1206
device=inductor}
C {lab_pin.sym} 950 -570 0 1 {name=l6 sig_type=std_logic lab=VLED}
C {vsource.sym} 340 -410 0 0 {name=Vset value="pulse 0 1 0 1n 1n 2.1u 5u"}
C {lab_pin.sym} 340 -500 0 0 {name=l7 sig_type=std_logic lab=CTRL1}
C {lab_pin.sym} 580 -470 0 1 {name=l5 sig_type=std_logic lab=SW}
C {lab_pin.sym} 540 -370 0 0 {name=l4 sig_type=std_logic lab=0}
C {res.sym} 430 -740 1 0 {name=R1
value=0.01
footprint=1206
device=resistor
m=1}
C {capa.sym} 860 -410 0 0 {name=C1
m=1
value=10u
footprint=1206
device="ceramic capacitor"}
C {ammeter.sym} 350 -740 3 0 {name=Vvcc}
C {lab_pin.sym} 860 -530 0 1 {name=l8 sig_type=std_logic lab=VO}
C {res.sym} 860 -680 0 0 {name=R2
value="r='V(SW,VO) > 0 ? 0.1 : 1e7'"
footprint=1206
device=resistor
m=1
}
C {bsource.sym} 1050 -770 0 0 {name=B1 VAR=V FUNC="pwl(V(VLED,VCC), 
+ -0.006, 0,
+ -0.005, 0, 
+ -0.001, 0.5, 
+ 0.001, 4.5, 
+ 0.005, 5,
+ 0.006, 5)"
}
C {lab_pin.sym} 1050 -740 0 0 {name=l9 sig_type=std_logic lab=0}
C {lab_pin.sym} 1050 -830 0 0 {name=l10 sig_type=std_logic lab=COMP}
C {spice_probe.sym} 950 -570 0 0 {name=p1 analysis=tran}
C {spice_probe.sym} 260 -740 0 0 {name=p2 analysis=tran}
C {spice_probe.sym} 580 -560 0 0 {name=p3 analysis=tran}
C {spice_probe.sym} 460 -500 0 0 {name=p4 analysis=tran}
C {title-2.sym} 0 0 0 0 {name=l11 author="Stefan Schippers" lock=1
page=1 pages=1 rev=1.1}
C {launcher.sym} 1340 -640 0 0 {name=h5
descr="load waves"
tclcommand="xschem raw_read $netlist_dir/@schname\\\\.raw tran"
}
