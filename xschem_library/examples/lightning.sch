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
T {Ligthning
simulation} 480 -210 0 0 0.4 0.4 {}
N 120 -540 120 -430 {lab=IN}
N 120 -540 220 -540 {lab=IN}
N 220 -540 220 -430 {lab=IN}
N 120 -640 120 -540 {lab=IN}
N 120 -370 120 -280 {lab=BAT0V}
N 1100 -280 1160 -280 {lab=BAT0V}
N 220 -370 220 -280 {lab=BAT0V}
N 220 -540 250 -540 {lab=IN}
N 360 -540 400 -540 {lab=B1}
N 120 -740 120 -700 {lab=A1}
N 440 -600 440 -570 {lab=C1}
N 360 -620 440 -620 {lab=C1}
N 360 -560 360 -540 {lab=B1}
N 440 -820 440 -740 {lab=BAT3V}
N 440 -600 500 -600 {lab=C1}
N 580 -600 620 -600 {lab=B2}
N 580 -600 580 -540 {lab=B2}
N 660 -640 660 -630 {lab=E2}
N 580 -820 580 -740 {lab=LAMP1}
N 580 -480 580 -440 {lab=E3}
N 880 -440 960 -440 {lab=E3}
N 660 -640 720 -640 {lab=E2}
N 720 -640 720 -540 {lab=E2}
N 720 -480 720 -440 {lab=E3}
N 660 -570 660 -560 {lab=C2}
N 660 -560 760 -560 {lab=C2}
N 880 -560 920 -560 {lab=B3}
N 880 -560 880 -540 {lab=B3}
N 880 -480 880 -440 {lab=E3}
N 960 -820 960 -590 {lab=LAMP1}
N 660 -820 960 -820 {lab=LAMP1}
N 960 -530 960 -440 {lab=E3}
N 960 -440 1060 -440 {lab=E3}
N 1100 -520 1100 -470 {lab=LAMP2}
N 1100 -520 1160 -520 {lab=LAMP2}
N 1040 -920 1040 -740 {lab=BAT3V}
N 1040 -680 1040 -280 {lab=BAT0V}
N 120 -280 220 -280 {lab=BAT0V}
N 220 -280 440 -280 {lab=BAT0V}
N 310 -540 360 -540 {lab=B1}
N 440 -680 440 -620 {lab=C1}
N 440 -620 440 -600 {lab=C1}
N 560 -600 580 -600 {lab=B2}
N 660 -680 660 -640 {lab=E2}
N 580 -820 660 -820 {lab=LAMP1}
N 580 -440 720 -440 {lab=E3}
N 720 -440 880 -440 {lab=E3}
N 820 -560 880 -560 {lab=B3}
N 1040 -280 1100 -280 {lab=BAT0V}
N 580 -680 580 -600 {lab=B2}
N 440 -280 1040 -280 {lab=BAT0V}
N 440 -920 440 -820 {lab=BAT3V}
N 440 -920 1160 -920 {lab=BAT3V}
N 960 -820 1100 -820 {lab=LAMP1}
N 1100 -840 1100 -820 {lab=LAMP1}
N 1100 -920 1100 -900 {lab=BAT3V}
N 1100 -820 1100 -640 {lab=LAMP1}
N 1100 -640 1160 -640 {lab=LAMP1}
N 550 -820 550 -710 {lab=LAMP1}
N 550 -820 580 -820 {lab=LAMP1}
N 440 -510 440 -440 {lab=#net1}
N 440 -380 440 -280 {lab=BAT0V}
N 1100 -410 1100 -380 {lab=#net2}
N 1100 -320 1100 -280 {lab=BAT0V}
N 660 -750 660 -740 {lab=#net3}
N 660 -820 660 -810 {lab=LAMP1}
N 960 -350 960 -340 {lab=#net4}
N 960 -360 960 -350 {lab=#net4}
N 960 -440 960 -420 {lab=E3}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {ind.sym} 120 -670 0 0 {name=L2 
m=1 
value=10m
footprint=1206 
device="inductor"}
C {ind.sym} 120 -400 0 0 {name=L1
m=1 
value=10m
footprint=1206 
device="inductor"}
C {capa.sym} 280 -540 3 1 {name=C2 
m=1 
value=10n
footprint=1206 
device="ceramic capacitor"}
C {capa.sym} 220 -400 0 1 {name=C1 
m=1 
value=680p
footprint=1206 
device="ceramic capacitor"}
C {npn.sym} 420 -540 0 0 {name=Q1 
model=Q2N4401 
device=2N4401 
footprint=TO92 
area=1
pinnumber(C)=1
pinnumber(B)=2
pinnumber(E)=3}
C {res.sym} 440 -710 0 0 {name=R2 
value=3.9K 
footprint=1206 
device=resistor 
m=1}
C {ipin.sym} 120 -740 1 0 {name=p1 lab=A1}
C {res.sym} 360 -590 0 0 {name=R1 
value=180K 
footprint=1206 
device=resistor 
m=1}
C {capa.sym} 530 -600 3 1 {name=C3
m=1 
value=10n
footprint=1206 
device="ceramic capacitor"}
C {pnp.sym} 640 -600 0 0 {name=Q2
model=Q2N4403
device=2N4403
footprint=TO92
area=1
pinnumber(C)=1
pinnumber(B)=2
pinnumber(E)=3}
C {diode.sym} 660 -710 0 0 {name=D1 model=D1N914 area=1}
C {res.sym} 580 -510 0 0 {name=R3 
value=22K
footprint=1206 
device=resistor 
m=1}
C {capa.sym} 720 -510 0 1 {name=C4 
m=1 
value=100u
footprint=1206 
device="ceramic capacitor"}
C {res.sym} 790 -560 3 1 {name=R5 
value=2.2K
footprint=1206 
device=resistor 
m=1}
C {capa.sym} 880 -510 0 1 {name=C5 
m=1 
value=5n
footprint=1206 
device="ceramic capacitor"}
C {npn.sym} 940 -560 0 0 {name=Q3 
model=Q2N4401 
device=2N4401 
footprint=TO92 
area=1
pinnumber(C)=1
pinnumber(B)=2
pinnumber(E)=3}
C {res.sym} 960 -310 0 0 {name=R6 
value=2.7K
footprint=1206 
device=resistor 
m=1}
C {npn.sym} 1080 -440 0 0 {name=Q4 
model=Q2N4401 
device=2N4401 
footprint=TO92 
area=1
pinnumber(C)=1
pinnumber(B)=2
pinnumber(E)=3}
C {capa.sym} 1040 -710 0 1 {name=C6 
m=1 
value=1u
footprint=1206 
device="ceramic capacitor"}
C {res.sym} 1100 -870 0 0 {name=R7 
value=47
footprint=1206 
device=resistor 
m=1}
C {ipin.sym} 1160 -920 0 1 {name=p2 lab=BAT3V}
C {ipin.sym} 1160 -280 0 1 {name=p3 lab=BAT0V}
C {opin.sym} 1160 -640 0 0 {name=p4 lab=LAMP1}
C {opin.sym} 1160 -520 0 0 {name=p5 lab=LAMP2}
C {var_res.sym} 580 -710 0 0 {name=R8
value=9.5K
footprint=1206
m=1
}
C {code.sym} 40 -210 0 0 {name=MODELS value="
** you need to get the spice models for Q2N4401, Q2N4403 and D1N914
** and put them in file referenced below.
.include models_lightning.txt
"}
C {code_shown.sym} 190 -210 0 0 {name=COMMANDS value=".save all
.OP
*.tran 4n 1m
"}
C {spice_probe.sym} 900 -920 0 0 {name=p7 analysis=tran voltage=3}
C {spice_probe.sym} 170 -540 0 0 {name=p9 analysis=tran voltage=0.0000e+00}
C {spice_probe.sym} 340 -540 0 1 {name=p10 analysis=tran voltage=0.6363}
C {spice_probe.sym} 670 -440 0 1 {name=p11 analysis=tran voltage=0.5154}
C {spice_probe.sym} 850 -560 0 1 {name=p12 analysis=tran voltage=1.116}
C {spice_probe.sym} 580 -620 0 1 {name=p13 analysis=tran voltage=2.244}
C {spice_probe.sym} 620 -280 0 1 {name=p14 analysis=tran voltage=0.0000e+00}
C {vsource.sym} 1060 -150 0 0 {name=V1 value=0}
C {gnd.sym} 1060 -120 0 0 {name=l2 lab=GND}
C {lab_pin.sym} 1060 -180 0 0 {name=l3 sig_type=std_logic lab=BAT0V}
C {vsource.sym} 940 -150 0 0 {name=V2 value=3}
C {gnd.sym} 940 -120 0 0 {name=l4 lab=GND}
C {lab_pin.sym} 940 -180 0 0 {name=l5 sig_type=std_logic lab=BAT3V}
C {spice_probe.sym} 490 -600 0 1 {name=p6 analysis=tran voltage=1.317}
C {spice_probe.sym} 730 -560 0 0 {name=p15 analysis=tran voltage=1.12}
C {spice_probe.sym} 1100 -520 0 0 {name=p16 analysis=tran voltage=2.99}
C {spice_probe.sym} 1100 -640 2 1 {name=p17 analysis=tran voltage=2.991}
C {lab_pin.sym} 800 -120 0 0 {name=l6 sig_type=std_logic lab=LAMP2}
C {lab_pin.sym} 800 -180 0 0 {name=l7 sig_type=std_logic lab=LAMP1}
C {res.sym} 800 -150 0 0 {name=Rlamp 
value=300
footprint=1206 
device=resistor 
m=1}
C {spice_probe.sym} 680 -640 0 0 {name=p8 analysis=tran voltage=2.718}
C {isource.sym} 600 -190 0 0 {name=I0 value="dc 0
+ pwl 0 0 
+ 1u 0 
+ 1.01u 1m
+ 2u 1m
+ 2.01u 0" }
C {lab_pin.sym} 600 -220 0 1 {name=l8 sig_type=std_logic lab=A1}
C {gnd.sym} 600 -100 0 0 {name=l9 lab=GND}
C {res.sym} 600 -130 0 1 {name=RS
value=1
footprint=1206 
device=resistor 
m=1}
C {lab_wire.sym} 400 -540 0 0 {name=l10 sig_type=std_logic lab=B1}
C {lab_wire.sym} 440 -570 0 1 {name=l11 sig_type=std_logic lab=C1}
C {lab_wire.sym} 620 -600 0 0 {name=l12 sig_type=std_logic lab=B2}
C {lab_wire.sym} 700 -560 0 0 {name=l13 sig_type=std_logic lab=C2}
C {lab_wire.sym} 660 -650 0 0 {name=l14 sig_type=std_logic lab=E2}
C {lab_wire.sym} 160 -540 0 0 {name=l15 sig_type=std_logic lab=IN}
C {lab_wire.sym} 910 -560 0 0 {name=l16 sig_type=std_logic lab=B3}
C {lab_wire.sym} 960 -490 0 0 {name=l18 sig_type=std_logic lab=E3}
C {ammeter.sym} 440 -410 0 0 {name=VE1 current=4.3151e-04}
C {ammeter.sym} 1100 -350 0 0 {name=VE4 current=4.3372e-06}
C {ammeter.sym} 660 -780 0 0 {name=VE2 current=1.5559e-06}
C {ammeter.sym} 960 -390 0 0 {name=VE3 current=1.9089e-04}
