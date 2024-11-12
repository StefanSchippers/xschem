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
V {}
S {}
E {}
P 11 5 290 -100 290 -670 1000 -670 1000 -100 290 -100 {}
T {Components inside green box are in package} 340 -700 0 0 0.4 0.4 {}
T {MSA-2643} 300 -660 0 0 0.8 0.8 {layer=5}
T {Agilent MSA-2643
Cascadable Silicon Bipolar
Gain Block MMIC Amplifier} 390 -940 0 0 0.9 0.9 {}
N 500 -430 560 -430 {lab=B1}
N 500 -430 500 -390 {lab=B1}
N 460 -390 500 -390 {lab=B1}
N 380 -390 380 -290 {lab=VIN_IN}
N 380 -390 400 -390 {lab=VIN_IN}
N 150 -390 180 -390 {lab=#net1}
N 70 -390 70 -170 {lab=Vin}
N 70 -390 90 -390 {lab=Vin}
N 380 -170 400 -170 {lab=#net2}
N 380 -230 380 -170 {lab=#net2}
N 360 -170 380 -170 {lab=#net2}
N 240 -170 240 -110 {lab=0}
N 500 -490 500 -430 {lab=B1}
N 500 -390 500 -290 {lab=B1}
N 500 -230 500 -170 {lab=GND_IN}
N 460 -170 500 -170 {lab=GND_IN}
N 600 -370 600 -290 {lab=B2}
N 600 -230 600 -170 {lab=GND_IN}
N 500 -170 600 -170 {lab=GND_IN}
N 600 -370 660 -370 {lab=B2}
N 700 -590 700 -400 {lab=C}
N 500 -590 500 -550 {lab=C}
N 500 -590 700 -590 {lab=C}
N 700 -320 700 -290 {lab=E2}
N 700 -230 700 -170 {lab=GND_IN}
N 600 -170 700 -170 {lab=GND_IN}
N 700 -320 770 -320 {lab=E2}
N 770 -320 770 -290 {lab=E2}
N 770 -230 770 -170 {lab=GND_IN}
N 700 -170 770 -170 {lab=GND_IN}
N 770 -170 800 -170 {lab=GND_IN}
N 700 -590 800 -590 {lab=C}
N 890 -590 890 -290 {lab=VCC_IN}
N 860 -590 890 -590 {lab=VCC_IN}
N 890 -230 890 -170 {lab=#net3}
N 970 -170 1050 -170 {lab=0}
N 1050 -170 1050 -110 {lab=0}
N 240 -390 380 -390 {lab=VIN_IN}
N 600 -400 600 -370 {lab=B2}
N 700 -340 700 -320 {lab=E2}
N 860 -170 890 -170 {lab=#net3}
N 600 -590 600 -460 {lab=C}
N 1100 -230 1100 -170 {lab=#net4}
N 1100 -390 1100 -290 {lab=#net5}
N 1100 -480 1100 -450 {lab=#net6}
N 890 -590 1100 -590 {lab=VCC_IN}
N 1100 -590 1120 -590 {lab=VCC_IN}
N 1180 -590 1260 -590 {lab=Vout}
N 1220 -590 1220 -530 {lab=Vout}
N 50 -390 70 -390 {lab=Vin}
N 240 -170 300 -170 {lab=0}
N 890 -170 910 -170 {lab=#net3}
N 1100 -590 1100 -540 {lab=VCC_IN}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {Q1.sym} 580 -430 0 0 {name=x1}
C {Q2.sym} 680 -370 0 0 {name=x2}
C {vsource.sym} 70 -140 0 0 {name=Vin
value="dc 0 ac 1 0 SIN 0 0.1 1G 0 0"
}
C {res.sym} 120 -390 3 0 {name=Rth_in
value=50
footprint=1206 
device=resistor m=1}
C {capa.sym} 210 -390 3 0 {name=Cin 
m=1 
value=39p 
footprint=1206 
device="ceramic capacitor"}
C {gnd.sym} 70 -110 0 0 {name=l2 lab=0}
C {ind.sym} 430 -390 3 0 {name=L1 
m=1 
value=0.833n
footprint=1206 
device="inductor"}
C {capa.sym} 380 -260 0 0 {name=Cpara1
m=1 
value=0.158p 
footprint=1206 
device="ceramic capacitor"}
C {ind.sym} 430 -170 3 0 {name=L4
m=1 
value=0.386n
footprint=1206 
device="inductor"}
C {gnd.sym} 240 -110 0 0 {name=l3 lab=0}
C {res.sym} 500 -520 0 0 {name=R1
value=560
footprint=1206 
device=resistor m=1}
C {res.sym} 500 -260 0 0 {name=R2
value=660
footprint=1206 
device=resistor m=1}
C {res.sym} 600 -260 0 0 {name=R3
value=250
footprint=1206 
device=resistor m=1}
C {res.sym} 700 -260 0 0 {name=R4
value=5
footprint=1206 
device=resistor m=1}
C {capa.sym} 770 -260 0 0 {name=C1
m=1 
value=4p
footprint=1206 
device="ceramic capacitor"}
C {ind.sym} 830 -170 3 0 {name=L6
m=1 
value=0.313n
footprint=1206 
device="inductor"}
C {capa.sym} 890 -260 0 0 {name=Cpara2
m=1 
value=0.155p 
footprint=1206 
device="ceramic capacitor"}
C {ind.sym} 830 -590 3 0 {name=L7
m=1 
value=0.407n
footprint=1206 
device="inductor"}
C {gnd.sym} 1050 -110 0 1 {name=l4 lab=0}
C {vsource.sym} 1100 -140 0 0 {name=Vpwr
value="dc 5"}
C {gnd.sym} 1100 -110 0 0 {name=l5 lab=0}
C {res.sym} 1100 -260 0 0 {name=Rc
value=50
footprint=1206 
device=resistor m=1}
C {ind.sym} 1100 -420 0 0 {name=Lc
m=1 
value=47n
footprint=1206 
device="inductor"}
C {capa.sym} 1150 -590 3 0 {name=Cout
m=1 
value=39p
footprint=1206 
device="ceramic capacitor"}
C {res.sym} 1220 -500 0 0 {name=RL
value=50
footprint=1206 
device=resistor m=1}
C {gnd.sym} 1220 -470 0 0 {name=l6 lab=0}
C {opin.sym} 1260 -590 0 0 {name=p1 lab=Vout}
C {lab_wire.sym} 530 -430 0 1 {name=l7
lab=B1}
C {lab_wire.sym} 620 -370 0 1 {name=l8
lab=B2}
C {lab_wire.sym} 550 -590 0 1 {name=l9
lab=C}
C {lab_wire.sym} 730 -320 0 1 {name=l10
lab=E2}
C {lab_wire.sym} 510 -170 0 1 {name=l11
lab=GND_IN}
C {lab_wire.sym} 900 -590 0 1 {name=l12
lab=VCC_IN}
C {lab_wire.sym} 310 -390 0 1 {name=l13
lab=VIN_IN}
C {code_shown.sym} 0 -600 0 0 {name=CMD value="* .OP
.AC DEC 20 1MegHz 100GHz
* .DC I1 0 5mA .001mA
.SAVE ALL
* .TRAN 0.04ns 20ns
"}
C {ipin.sym} 50 -390 0 0 {name=p2 lab=Vin}
C {res.sym} 330 -170 1 0 {name=Rparax1
value=0.1
footprint=1206 
device=resistor m=1}
C {res.sym} 940 -170 1 0 {name=Rparax2
value=0.1
footprint=1206 
device=resistor m=1}
C {ammeter.sym} 1100 -510 2 0 {name=vmeasure current=0.02882}
C {spice_probe.sym} 1070 -590 0 0 {name=p3 analysis=tran voltage=3.559}
C {spice_probe.sym} 660 -590 0 0 {name=p4 analysis=tran voltage=3.559}
C {spice_probe.sym} 500 -360 0 0 {name=p5 analysis=tran voltage=1.848}
C {spice_probe.sym} 600 -310 0 0 {name=p6 analysis=tran voltage=0.9654}
C {spice_probe.sym} 770 -320 0 0 {name=p7 analysis=tran voltage=0.1123}
C {spice_probe.sym} 630 -170 0 0 {name=p8 analysis=tran voltage=0.001441}
