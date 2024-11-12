v {xschem version=3.4.5 file_version=1.2
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
P 4 7 630 -290 630 -320 620 -320 630 -347.5 640 -320 630 -320 630 -290 {fill=true}
T {Specifying @lab
will result in net
@#0:net_name} 640 -310 0 0 0.4 0.4 {name=l6 layer=4}
T {Title symbol has embedded TCL command
to enable show_pin_net_names } 180 -110 0 0 0.4 0.4 { layer=7}
T {@#1:net_name} 1120 -1030 0 0 0.4 0.4 {name=l19 layer=4}
N 170 -390 910 -390 {bus=true
lab=DATA[15:0]}
N 390 -530 390 -400 {
lab=DATA[3]}
N 280 -530 280 -400 {
lab=DATA[13]}
N 450 -380 450 -230 {
lab=DATA[7:4]}
N 330 -380 330 -230 {
lab=DATA[11:8]}
N 220 -380 220 -230 {
lab=DATA[3:0]}
N 600 -380 600 -230 {
lab=DATA[15:12]}
N 500 -530 500 -400 {
lab=DATA[10]}
N 620 -530 620 -400 {
lab=DATA[0]}
N 840 -500 840 -490 {
lab=VCC}
N 500 -600 500 -590 {
lab=VCC}
N 390 -600 390 -590 {
lab=VCC}
N 280 -600 280 -590 {
lab=VCC}
N 220 -170 220 -160 {
lab=VSS}
N 330 -170 330 -160 {
lab=VSS}
N 450 -170 450 -160 {
lab=VSS}
N 600 -170 600 -160 {
lab=VSS}
N 190 -450 190 -390 {
lab=DATA[15:0]}
N 190 -520 190 -510 {
lab=VCC}
N 840 -430 840 -390 {
lab=DATA[15:0]}
N 170 -790 720 -790 {bus=true
lab=DIN[15..0]}
N 280 -780 280 -720 {
lab=DIN0}
N 500 -780 500 -720 {
lab=DIN[4..1]}
N 280 -660 280 -640 {
lab=VSS}
N 500 -660 500 -640 {
lab=VSS}
N 700 -780 700 -720 {
lab=DIN5}
N 700 -660 700 -640 {
lab=VSS}
N 230 -980 720 -980 {bus=true
lab="CK , S1, ADD[3:0],ENAB"}
N 280 -970 280 -910 {
lab=ADD[3:0]}
N 500 -970 500 -910 {
lab=ENAB}
N 280 -850 280 -830 {
lab=VSS}
N 500 -850 500 -830 {
lab=VSS}
N 700 -970 700 -910 {
lab=CK}
N 700 -850 700 -830 {
lab=VSS}
N 980 -790 1640 -790 {bus=true
lab=DOUT[15:0]}
N 1140 -780 1140 -720 {
lab=DOUT[0]}
N 1310 -780 1310 -720 {
lab=DOUT[7:1]}
N 1140 -660 1140 -640 {
lab=VSS}
N 1310 -660 1310 -640 {
lab=VSS}
N 1510 -780 1510 -720 {
lab=DOUT[15:8]}
N 1510 -660 1510 -640 {
lab=VSS}
N 980 -1170 1090 -1170 {
lab=DOUT[15:0]}
N 1090 -1170 1110 -1170 {
lab=DOUT[15:0]}
N 1110 -1170 1110 -800 {
lab=DOUT[15:0]}
N 620 -600 620 -590 {
lab=VCC}
N 390 -970 390 -910 {
lab=ADD[1]}
N 390 -850 390 -830 {
lab=VSS}
C {bus_tap.sym} 400 -390 3 0 {name=l1 lab=[3]
}
C {bus_tap.sym} 290 -390 3 0 {name=l2 lab=[13]
}
C {bus_tap.sym} 440 -390 1 0 {name=l3 lab=[7:4]
}
C {bus_tap.sym} 320 -390 1 0 {name=l4 lab=[11:8]
}
C {bus_tap.sym} 210 -390 1 0 {name=l5 lab=[3:0]
}
C {bus_tap.sym} 510 -390 3 0 {name=l7 lab=[10]
}
C {bus_tap.sym} 630 -390 3 0 {name=l8 lab=[0]
}
C {res.sym} 620 -560 0 0 {name=R1
value=1k
footprint=1206
device=resistor
m=1
}
C {res.sym} 500 -560 0 0 {name=R2
value=1k
footprint=1206
device=resistor
m=1
}
C {res.sym} 390 -560 0 0 {name=R3
value=1k
footprint=1206
device=resistor
m=1
}
C {res.sym} 280 -560 0 0 {name=R4
value=1k
footprint=1206
device=resistor
m=1
}
C {res.sym} 220 -200 0 0 {name=R5[3:0]
value=1k
footprint=1206
device=resistor
m=1
}
C {res.sym} 330 -200 0 0 {name=R6[3:0]
value=1k
footprint=1206
device=resistor
m=1
}
C {res.sym} 450 -200 0 0 {name=R7[3:0]
value=1k
footprint=1206
device=resistor
m=1
}
C {res.sym} 600 -200 0 0 {name=R8[3:0]
value=1k
footprint=1206
device=resistor
m=1
}
C {lab_pin.sym} 190 -520 0 0 {name=p10 sig_type=std_logic lab=VCC
}
C {bus_tap.sym} 590 -390 1 0 {name=l6 lab=[15:12]
}
C {lab_pin.sym} 280 -600 0 0 {name=p2 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 390 -600 0 0 {name=p3 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 500 -600 0 0 {name=p4 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 840 -500 0 0 {name=p5 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 220 -160 0 0 {name=p6 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 330 -160 0 0 {name=p7 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 450 -160 0 0 {name=p8 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 600 -160 0 0 {name=p9 sig_type=std_logic lab=VSS
}
C {res.sym} 190 -480 0 0 {name=R9[15:0]
value=1k
footprint=1206
device=resistor
m=1
}
C {lab_pin.sym} 170 -390 0 0 {name=p1 sig_type=std_logic lab=DATA[15:0]
}
C {res.sym} 840 -460 0 0 {name=R10[15:0]
value=1k
footprint=1206
device=resistor
m=1
}
C {title.sym} 160 -30 0 0 {name=l9 

author="Stefan Schippers"}
C {lab_pin.sym} 170 -790 0 0 {name=p11 sig_type=std_logic lab=DIN[15..0]
}
C {bus_tap.sym} 270 -790 1 0 {name=l10 lab=0
}
C {bus_tap.sym} 490 -790 1 0 {name=l11 lab=[4..1]
}
C {res.sym} 500 -690 0 0 {name=R11[3:0]
value=1k
footprint=1206
device=resistor
m=1
}
C {res.sym} 280 -690 0 0 {name=R12
value=1k
footprint=1206
device=resistor
m=1
}
C {lab_pin.sym} 280 -640 0 0 {name=p12 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 500 -640 0 0 {name=p13 sig_type=std_logic lab=VSS
}
C {bus_tap.sym} 690 -790 1 0 {name=l12 lab=5
}
C {res.sym} 700 -690 0 0 {name=R13
value=1k
footprint=1206
device=resistor
m=1
}
C {lab_pin.sym} 700 -640 0 0 {name=p14 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 230 -980 0 0 {name=p15 sig_type=std_logic lab="CK , S1, ADD[3:0],ENAB"
}
C {bus_tap.sym} 270 -980 1 0 {name=l13 lab=[3:0]
}
C {bus_tap.sym} 490 -980 1 0 {name=l14 lab=ENAB
}
C {res.sym} 280 -880 0 0 {name=R15[3:0]
value=1k
footprint=1206
device=resistor
m=1
}
C {res.sym} 500 -880 0 0 {name=R14
value=1k
footprint=1206
device=resistor
m=1
}
C {lab_pin.sym} 280 -830 0 0 {name=p16 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 500 -830 0 0 {name=p17 sig_type=std_logic lab=VSS
}
C {bus_tap.sym} 690 -980 1 0 {name=l15 lab=CK
}
C {res.sym} 700 -880 0 0 {name=R16
value=1k
footprint=1206
device=resistor
m=1
}
C {lab_pin.sym} 700 -830 0 0 {name=p18 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 980 -790 0 0 {name=p19 sig_type=std_logic lab=DOUT[15:0]
}
C {bus_tap.sym} 1130 -790 1 0 {name=l16 lab=[0]
}
C {bus_tap.sym} 1300 -790 1 0 {name=l17 lab=[7:1]
}
C {res.sym} 1310 -690 0 0 {name=R18[6:0]
value=1k
footprint=1206
device=resistor
m=1
}
C {res.sym} 1140 -690 0 0 {name=R17
value=1k
footprint=1206
device=resistor
m=1
}
C {lab_pin.sym} 1140 -640 0 0 {name=p20 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 1310 -640 0 0 {name=p21 sig_type=std_logic lab=VSS
}
C {bus_tap.sym} 1500 -790 1 0 {name=l18 lab=[15:8]
}
C {res.sym} 1510 -690 0 0 {name=R19[7:0]
value=1k
footprint=1206
device=resistor
m=1
}
C {lab_pin.sym} 1510 -640 0 0 {name=p22 sig_type=std_logic lab=VSS
}
C {rom2_sa.sym} 830 -1110 0 0 {name=xsa[15:0]}
C {lab_pin.sym} 680 -1170 0 0 {name=p24 lab=LDCP}
C {lab_pin.sym} 680 -1150 0 0 {name=p25 lab=LDYMS}
C {lab_pin.sym} 680 -1130 0 0 {name=p26 lab=LDOE}
C {lab_pin.sym} 680 -1110 0 0 {name=p27 lab=LDPRECH}
C {lab_pin.sym} 680 -1090 0 0 {name=p28 lab=LDSAL}
C {lab_pin.sym} 680 -1070 0 0 {name=p29 lab=vcc}
C {lab_pin.sym} 680 -1050 0 0 {name=p30 lab=vss}
C {bus_tap.sym} 1120 -790 3 0 {name=l19 lab=[15:0]
}
C {lab_pin.sym} 620 -600 0 0 {name=p23 sig_type=std_logic lab=VCC
}
C {bus_tap.sym} 380 -980 1 0 {name=l20 lab=[1]
}
C {res.sym} 390 -880 0 0 {name=R20
value=1k
footprint=1206
device=resistor
m=1
}
C {lab_pin.sym} 390 -830 0 0 {name=p31 sig_type=std_logic lab=VSS
}
