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
L 4 920 -500 920 -340 {dash=3}
L 4 1270 -500 1270 -340 {dash=3}
P 4 11 1110 -510 1060 -510 1060 -520 1020 -500 1060 -480 1060 -490 1110 -490 1110 -480 1150 -500 1110 -520 1110 -510 {}
T {3-way, 4-way switches} 1180 -900 0 0 0.6 0.6 {}
T {rectifying devices} 1220 -1540 0 0 0.6 0.6 {}
T {Transistor-level logic gates} 1930 -590 0 0 0.6 0.6 {}
T {Bidirectional lines} 960 -590 0 0 0.6 0.6 {}
T {Standard cell logic gates} 310 -890 0 0 0.6 0.6 {}
T {Bidirectional pass transistors} 240 -1540 0 0 0.6 0.6 {}
T {Sequential logic} 1970 -1540 0 0 0.6 0.6 {}
T {Cyan: Uninitialized or unknown (X)} 210 -200 0 0 0.4 0.4 {layer=1}
T {Signal colors:} 190 -230 0 0 0.4 0.4 {}
T {Blue: Logic level low (0)} 210 -170 0 0 0.4 0.4 {layer=12}
T {Red: Logic level high (1)} 210 -140 0 0 0.4 0.4 {layer=5}
T {Brown: High Impedance (Z)} 210 -110 0 0 0.4 0.4 {layer=13}
T {To stop simulation: press Escape key} 10 -300 0 0 0.4 0.4 {}
N 170 -460 260 -460 { lab=B}
N 170 -580 230 -580 { lab=#net1}
N 210 -540 230 -540 { lab=C}
N 890 -460 890 -400 { lab=#net2}
N 890 -460 1300 -460 { lab=#net2}
N 720 -460 720 -330 { lab=#net2}
N 700 -210 700 -150 { lab=#net3}
N 680 -380 680 -330 { lab=B}
N 610 -380 680 -380 { lab=B}
N 810 -360 810 -270 { lab=DN}
N 690 -460 720 -460 { lab=#net2}
N 1470 -460 1490 -460 { lab=#net2}
N 780 -400 810 -400 { lab=CK3}
N 1300 -460 1300 -400 { lab=#net2}
N 1470 -460 1470 -330 { lab=#net2}
N 1490 -210 1490 -150 { lab=#net4}
N 1510 -380 1510 -330 { lab=A}
N 1510 -380 1580 -380 { lab=A}
N 1380 -360 1380 -270 { lab=D}
N 1380 -270 1390 -270 { lab=D}
N 1380 -400 1410 -400 { lab=CK2}
N 720 -460 890 -460 { lab=#net2}
N 1300 -460 1470 -460 { lab=#net2}
N 380 -710 470 -710 { lab=#net5}
N 420 -690 470 -690 { lab=CK2}
N 460 -670 470 -670 { lab=RSTB}
N 650 -710 740 -710 { lab=#net6}
N 460 -670 460 -640 { lab=RSTB}
N 460 -640 680 -640 { lab=RSTB}
N 680 -670 680 -640 { lab=RSTB}
N 680 -670 740 -670 { lab=RSTB}
N 920 -710 970 -710 { lab=CK3}
N 420 -770 420 -690 { lab=CK2}
N 680 -690 740 -690 { lab=CK2}
N 420 -770 680 -770 { lab=CK2}
N 680 -770 680 -690 { lab=CK2}
N 270 -740 290 -740 { lab=CK2}
N 290 -820 290 -740 { lab=CK2}
N 210 -820 290 -820 { lab=CK2}
N 30 -820 130 -820 { lab=#net7}
N 30 -820 30 -720 { lab=#net7}
N 30 -720 90 -720 { lab=#net7}
N 70 -740 90 -740 { lab=A}
N 90 -670 460 -670 { lab=RSTB}
N 90 -700 90 -670 { lab=RSTB}
N 50 -670 90 -670 { lab=RSTB}
N 1840 -300 1840 -270 { lab=Z}
N 1840 -210 1840 -140 { lab=GND}
N 1970 -160 1970 -140 { lab=GND}
N 1970 -300 1970 -220 { lab=Z}
N 1740 -190 1930 -190 { lab=B}
N 1740 -440 1740 -190 { lab=B}
N 1780 -280 1780 -240 { lab=A}
N 1780 -240 1800 -240 { lab=A}
N 1690 -280 1780 -280 { lab=A}
N 1690 -190 1740 -190 { lab=B}
N 1840 -240 1890 -240 { lab=GND}
N 1970 -190 2030 -190 { lab=GND}
N 1840 -410 1840 -370 { lab=#net8}
N 1840 -500 1840 -470 { lab=VCC}
N 1970 -300 2020 -300 { lab=Z}
N 1740 -440 1800 -440 { lab=B}
N 1780 -340 1800 -340 { lab=A}
N 1840 -440 1900 -440 { lab=VCC}
N 1840 -340 1900 -340 { lab=VCC}
N 1900 -440 1900 -340 { lab=VCC}
N 1840 -140 1970 -140 { lab=GND}
N 2390 -230 2390 -200 { lab=#net9}
N 2390 -260 2440 -260 { lab=GND}
N 2390 -170 2440 -170 { lab=GND}
N 2350 -440 2410 -440 { lab=VCC}
N 2470 -380 2530 -380 { lab=VCC}
N 2470 -350 2470 -320 { lab=#net10}
N 2390 -320 2470 -320 { lab=#net10}
N 2350 -410 2350 -320 { lab=#net10}
N 2390 -320 2390 -290 { lab=#net10}
N 2350 -500 2350 -470 { lab=VCC}
N 2350 -500 2470 -500 { lab=VCC}
N 2470 -500 2470 -410 { lab=VCC}
N 2390 -140 2390 -120 { lab=GND}
N 2310 -380 2430 -380 { lab=B}
N 2310 -310 2310 -260 { lab=B}
N 2310 -260 2350 -260 { lab=B}
N 2270 -170 2350 -170 { lab=A}
N 2270 -350 2270 -170 { lab=A}
N 2270 -440 2310 -440 { lab=A}
N 2180 -310 2310 -310 { lab=B}
N 2180 -350 2270 -350 { lab=A}
N 2470 -320 2600 -320 { lab=#net10}
N 700 -820 970 -820 { lab=CK3}
N 970 -820 970 -710 { lab=CK3}
N 380 -820 620 -820 { lab=#net5}
N 380 -820 380 -710 { lab=#net5}
N 380 -690 420 -690 { lab=CK2}
N 1840 -300 1970 -300 { lab=Z}
N 1780 -340 1780 -280 { lab=A}
N 2350 -320 2390 -320 { lab=#net10}
N 2310 -380 2310 -310 { lab=B}
N 2270 -440 2270 -350 { lab=A}
N 1840 -310 1840 -300 { lab=Z}
N 270 -1090 270 -1030 { lab=GND}
N 330 -1090 330 -1030 { lab=#net11}
N 100 -1090 270 -1090 { lab=GND}
N 330 -1090 370 -1090 { lab=#net11}
N 300 -990 300 -960 { lab=E}
N 160 -1220 170 -1220 { lab=E}
N 270 -1350 270 -1290 { lab=VCC}
N 330 -1350 330 -1290 { lab=#net11}
N 100 -1350 270 -1350 { lab=VCC}
N 330 -1350 370 -1350 { lab=#net11}
N 300 -1220 300 -1190 { lab=#net12}
N 250 -1220 300 -1220 { lab=#net12}
N 370 -1240 370 -1090 { lab=#net11}
N 370 -1240 420 -1240 { lab=#net11}
N 300 -1470 300 -1450 { lab=E}
N 150 -1470 150 -1220 { lab=E}
N 150 -1470 300 -1470 { lab=E}
N 160 -1220 160 -960 { lab=E}
N 160 -960 300 -960 { lab=E}
N 620 -1090 620 -1030 { lab=#net13}
N 680 -1090 680 -1030 { lab=GND}
N 680 -1090 850 -1090 { lab=GND}
N 560 -1090 620 -1090 { lab=#net13}
N 650 -990 650 -960 { lab=E}
N 800 -1220 850 -1220 { lab=E}
N 620 -1350 620 -1290 { lab=#net13}
N 680 -1350 680 -1290 { lab=VCC}
N 680 -1350 850 -1350 { lab=VCC}
N 560 -1350 620 -1350 { lab=#net13}
N 650 -1220 650 -1190 { lab=#net14}
N 650 -1220 700 -1220 { lab=#net14}
N 560 -1240 560 -1090 { lab=#net13}
N 510 -1240 560 -1240 { lab=#net13}
N 650 -1470 800 -1470 { lab=E}
N 650 -1470 650 -1450 { lab=E}
N 800 -1470 800 -1220 { lab=E}
N 790 -1220 790 -960 { lab=E}
N 650 -960 790 -960 { lab=E}
N 270 -1150 270 -1090 { lab=GND}
N 330 -1150 330 -1090 { lab=#net11}
N 270 -1410 270 -1350 { lab=VCC}
N 330 -1410 330 -1350 { lab=#net11}
N 300 -1250 300 -1220 { lab=#net12}
N 370 -1350 370 -1240 { lab=#net11}
N 110 -1220 150 -1220 { lab=E}
N 150 -1220 160 -1220 { lab=E}
N 680 -1150 680 -1090 { lab=GND}
N 620 -1150 620 -1090 { lab=#net13}
N 680 -1410 680 -1350 { lab=VCC}
N 620 -1410 620 -1350 { lab=#net13}
N 650 -1250 650 -1220 { lab=#net14}
N 560 -1350 560 -1240 { lab=#net13}
N 790 -1220 800 -1220 { lab=E}
N 780 -1220 790 -1220 { lab=E}
N 290 -690 380 -690 { lab=CK2}
N 290 -740 290 -690 { lab=CK2}
N 1270 -1440 1270 -1370 { lab=A}
N 1270 -1310 1270 -1250 { lab=#net15}
N 1270 -1250 1330 -1250 { lab=#net15}
N 1410 -1250 1500 -1250 { lab=#net16}
N 1400 -1320 1460 -1320 { lab=#net17}
N 1320 -1320 1340 -1320 { lab=#net15}
N 1320 -1320 1320 -1250 { lab=#net15}
N 1370 -1400 1370 -1360 { lab=#net18}
N 1240 -670 1340 -670 { lab=#net19}
N 1240 -690 1340 -690 { lab=#net20}
N 1400 -670 1500 -670 { lab=#net21}
N 1400 -690 1500 -690 { lab=#net22}
N 1560 -680 1660 -680 { lab=#net23}
N 1080 -680 1180 -680 { lab=VCC}
N 1210 -820 1210 -720 { lab=A}
N 1370 -820 1370 -720 { lab=C}
N 1530 -820 1530 -720 { lab=D}
N 2240 -1280 2240 -1250 { lab=#net24}
N 2120 -1350 2240 -1280 { lab=#net24}
N 2120 -1380 2120 -1350 { lab=#net24}
N 2240 -1400 2240 -1370 { lab=#net25}
N 2120 -1300 2240 -1370 { lab=#net25}
N 2120 -1300 2120 -1290 { lab=#net25}
N 2290 -1250 2320 -1250 { lab=#net24}
N 2060 -1420 2120 -1420 { lab=#net26}
N 2240 -860 2240 -830 { lab=#net26}
N 2120 -930 2240 -860 { lab=#net26}
N 2120 -940 2120 -930 { lab=#net26}
N 2240 -980 2240 -950 { lab=#net27}
N 2120 -880 2240 -950 { lab=#net27}
N 2240 -980 2320 -980 { lab=#net27}
N 2010 -980 2120 -980 { lab=A}
N 2470 -1080 2470 -1050 { lab=#net28}
N 2350 -1150 2470 -1080 { lab=#net28}
N 2350 -1180 2350 -1150 { lab=#net28}
N 2470 -1200 2470 -1170 { lab=Q}
N 2350 -1100 2470 -1170 { lab=Q}
N 2350 -1100 2350 -1090 { lab=Q}
N 2470 -1200 2530 -1200 { lab=Q}
N 2010 -1250 2120 -1250 { lab=A}
N 2010 -1250 2010 -980 { lab=A}
N 2060 -1420 2060 -940 { lab=#net26}
N 2060 -940 2120 -940 { lab=#net26}
N 2100 -1020 2120 -1020 { lab=#net24}
N 2100 -1040 2100 -1020 { lab=#net24}
N 2100 -1040 2290 -1170 { lab=#net24}
N 2290 -1250 2290 -1170 { lab=#net24}
N 2320 -1220 2350 -1220 { lab=#net24}
N 2320 -1250 2320 -1220 { lab=#net24}
N 2320 -1050 2350 -1050 { lab=#net27}
N 2320 -1050 2320 -980 { lab=#net27}
N 2350 -1010 2350 -710 { lab=RSTB}
N 1940 -790 2120 -790 { lab=RSTB}
N 1940 -790 1940 -710 { lab=RSTB}
N 1940 -1210 1940 -790 { lab=RSTB}
N 1940 -1210 2120 -1210 { lab=RSTB}
N 2240 -1250 2290 -1250 { lab=#net24}
N 1880 -830 2120 -830 { lab=C}
N 1870 -710 1940 -710 { lab=RSTB}
N 1940 -710 2350 -710 { lab=RSTB}
N 2010 -980 2010 -670 { lab=A}
N 2120 -880 2120 -870 { lab=#net27}
N 350 -560 370 -560 { lab=E}
N 370 -560 370 -500 { lab=E}
N 370 -500 390 -500 { lab=E}
N 260 -460 390 -460 { lab=B}
N 280 -420 390 -420 { lab=D}
N 550 -460 590 -460 { lab=G}
N 340 -380 390 -380 { lab=D}
N 340 -420 340 -380 { lab=D}
C {ipin.sym} 70 -190 0 0 { name=p20 lab=A }
C {ipin.sym} 70 -150 0 0 { name=p5 lab=C }
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {lab_pin.sym} 50 -600 0 0 {name=p10 lab=A}
C {nand2_1.sym} 110 -580 0 0 {name=x4 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 290 -560 0 0 {name=x8 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 210 -540 0 0 {name=p9 lab=C}
C {einvp_1.sym} 850 -400 0 0 {name=x1 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 700 -270 1 0 {name=x10 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {einvp_1.sym} 1340 -400 0 1 {name=x3 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 1490 -270 3 1 {name=x11 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {dlrtp_1.sym} 830 -690 0 0 {name=x15 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {dlrtn_1.sym} 560 -690 0 0 {name=x16 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {dfrtp_1.sym} 180 -720 0 0 {name=x13 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {inv_2.sym} 170 -820 0 1 {name=x14 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {ntrans.sym} 1820 -240 0 0 {name=m7 model=cmosn w=wn l=lln m=1}
C {ntrans.sym} 1950 -190 0 0 {name=m8 model=cmosn w=wn l=lln m=1}
C {lab_pin.sym} 1690 -280 0 0 {name=p24 lab=A}
C {lab_pin.sym} 1690 -190 0 0 {name=p25 lab=B}
C {lab_pin.sym} 2030 -190 0 1 {name=p27 lab=GND}
C {lab_pin.sym} 1890 -240 0 1 {name=p30 lab=GND}
C {ptrans.sym} 1820 -340 0 0 {name=m4 model=cmosp w=wp l=lp  m=1 }
C {ptrans.sym} 1820 -440 0 0 {name=m9 model=cmosp w=wp l=lp  m=1 }
C {lab_pin.sym} 2020 -300 0 1 {name=p26 lab=Z}
C {lab_pin.sym} 1900 -440 0 1 {name=p31 lab=VCC}
C {gnd.sym} 1840 -140 0 0 {name=l3 lab=GND}
C {vdd.sym} 1840 -500 0 0 {name=l4 lab=VCC}
C {ptrans.sym} 2330 -440 0 0 {name=m2 model=cmosp w=wp l=lp  m=1 }
C {ptrans.sym} 2450 -380 0 0 {name=m3 model=cmosp w=wp l=lp  m=1 }
C {ntrans.sym} 2370 -260 0 0 {name=m5 model=cmosn w=wn l=lln m=1}
C {ntrans.sym} 2370 -170 0 0 {name=m6 model=cmosn w=wn l=lln m=1}
C {lab_pin.sym} 2440 -260 0 1 {name=p22 lab=GND}
C {lab_pin.sym} 2440 -170 0 1 {name=p28 lab=GND}
C {lab_pin.sym} 2410 -440 0 1 {name=p29 lab=VCC}
C {lab_pin.sym} 2530 -380 0 1 {name=p32 lab=VCC}
C {vdd.sym} 2400 -500 0 0 {name=l11 lab=VCC}
C {gnd.sym} 2390 -120 0 0 {name=l12 lab=GND}
C {lab_pin.sym} 2180 -310 0 0 {name=p21 lab=B}
C {lab_pin.sym} 2180 -350 0 0 {name=p23 lab=A}
C {inv_2.sym} 660 -820 0 1 {name=x17 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {ntrans.sym} 300 -1170 1 0 {name=m1 model=cmosn w=wn l=lln m=1}
C {lab_pin.sym} 300 -1150 1 1 {name=p6 lab=GND}
C {ptrans.sym} 300 -1010 3 0 {name=m10 model=cmosp w=wp l=lp  m=1 }
C {lab_pin.sym} 300 -1030 3 1 {name=p18 lab=VCC}
C {inv_2.sym} 210 -1220 0 0 {name=x18 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {ntrans.sym} 300 -1430 1 0 {name=m11 model=cmosn w=wn l=lln m=1}
C {lab_pin.sym} 300 -1410 1 1 {name=p33 lab=GND}
C {ptrans.sym} 300 -1270 3 0 {name=m12 model=cmosp w=wp l=lp  m=1 }
C {lab_pin.sym} 300 -1290 3 1 {name=p34 lab=VCC}
C {vdd.sym} 100 -1350 0 0 {name=l5 lab=VCC}
C {gnd.sym} 100 -1090 0 0 {name=l6 lab=GND}
C {ntrans.sym} 650 -1170 3 1 {name=m13 model=cmosn w=wn l=lln m=1}
C {lab_pin.sym} 650 -1150 1 1 {name=p35 lab=GND}
C {ptrans.sym} 650 -1010 1 1 {name=m14 model=cmosp w=wp l=lp  m=1 }
C {lab_pin.sym} 650 -1030 3 1 {name=p36 lab=VCC}
C {inv_2.sym} 740 -1220 0 1 {name=x19 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {ntrans.sym} 650 -1430 3 1 {name=m15 model=cmosn w=wn l=lln m=1}
C {lab_pin.sym} 650 -1410 1 1 {name=p37 lab=GND}
C {ptrans.sym} 650 -1270 1 1 {name=m16 model=cmosp w=wp l=lp  m=1 }
C {lab_pin.sym} 650 -1290 3 1 {name=p38 lab=VCC}
C {vdd.sym} 850 -1350 0 1 {name=l7 lab=VCC}
C {gnd.sym} 850 -1090 0 1 {name=l8 lab=GND}
C {lab_pin.sym} 110 -1220 0 0 {name=p39 lab=E}
C {switch-1.sym} 1370 -1320 0 0 {name=s4}
C {inv_2.sym} 1370 -1250 0 0 {name=x20 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {diode_3.sym} 1270 -1340 0 0 {name=x12 VTH=0.6 RON=10 ROFF=1G}
C {ipin.sym} 70 -170 0 0 { name=p19 lab=B }
C {dev-2.sym} 1530 -680 0 1 {name=s3}
C {invert-1.sym} 1370 -680 0 0 {name=s1}
C {dev-1.sym} 1210 -680 0 0 {name=s2}
C {vdd.sym} 1080 -680 0 0 {name=l9 lab=VCC}
C {lab_pin.sym} 850 -1220 0 1 {name=p40 lab=E}
C {ipin.sym} 70 -130 0 0 { name=p69 lab=D }
C {nand2_1.sym} 2180 -1400 0 0 {name=x45 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand2_1.sym} 2410 -1200 0 0 {name=x49 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 2180 -1250 0 0 {name=x51 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 2180 -980 0 0 {name=x46 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 2410 -1050 0 0 {name=x50 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {nand3_1.sym} 2180 -830 0 0 {name=x48 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 2530 -1200 0 1 {name=l29 sig_type=std_logic lab=Q}
C {o21ai_1.sym} 470 -460 0 0 {name=x5 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 280 -420 0 0 {name=p12 lab=D}
C {lab_pin.sym} 970 -710 0 1 {name=p13 lab=CK3}
C {lab_pin.sym} 290 -820 0 1 {name=p14 lab=CK2}
C {lab_pin.sym} 170 -460 0 0 {name=p15 lab=B}
C {lab_pin.sym} 370 -560 0 1 {name=p1 lab=E}
C {lab_pin.sym} 50 -560 0 0 {name=p2 lab=B}
C {lab_pin.sym} 590 -460 0 1 {name=p11 lab=G}
C {lab_pin.sym} 70 -740 0 0 {name=p4 lab=A}
C {launcher.sym} 70 -330 0 0 {name=h3
descr="START SIMULATION" 
comment="
  This launcher Starts a simple interactive simulation
"
tclcommand="
  set count 0
  set duration 400
  xschem select instance p20  ;# A
  xschem select instance p19  ;# B
  xschem select instance p5   ;# C
  xschem select instance p69  ;# D
  xschem select instance p72  ;# RSTB
  xschem logic_set 0
  update
  after $duration
  xschem select instance p20 clear  ;# A
  xschem select instance p19 clear  ;# B
  xschem select instance p5  clear  ;# C
  xschem select instance p69 clear  ;# D
  xschem select instance p72 clear  ;# RSTB
  while \{1\} \{
    update
    incr count
    if \{$count>100 || $tclstop == 1\} break
    xschem select instance p20 
    xschem logic_set [expr int(rand()*2.)]
    xschem select instance p20 clear
    after $duration

    xschem select instance p19 
    xschem logic_set [expr int(rand()*2.)]
    xschem select instance p19 clear
    after $duration

    xschem select instance p5 
    xschem logic_set [expr int(rand()*2.)]
    xschem select instance p5 clear
    after $duration

    xschem select instance p69 
    xschem logic_set [expr int(rand()*2.)]
    xschem select instance p69 clear
    after $duration

    xschem select instance p72 
    xschem logic_set [expr int(0.8 + rand()*1.2)]
    xschem select instance p72 clear
    after $duration
  \}

"
}
C {lab_pin.sym} 610 -380 0 0 {name=p16 lab=B}
C {lab_pin.sym} 1580 -380 0 1 {name=p17 lab=A}
C {lab_pin.sym} 1390 -270 0 1 {name=p70 lab=D}
C {lab_pin.sym} 1410 -400 0 1 {name=p71 lab=CK2}
C {ipin.sym} 70 -110 0 0 { name=p72 lab=RSTB }
C {lab_pin.sym} 50 -670 0 0 {name=p73 lab=RSTB}
C {lab_pin.sym} 1870 -710 0 0 {name=p3 lab=RSTB}
C {lab_pin.sym} 1880 -830 0 0 {name=p8 lab=C}
C {lab_pin.sym} 2010 -670 0 0 {name=p7 lab=A}
C {lab_pin.sym} 1270 -1440 0 0 {name=p74 lab=A}
C {lab_pin.sym} 1210 -820 0 0 {name=p41 lab=A}
C {lab_pin.sym} 1370 -820 0 0 {name=p42 lab=C}
C {lab_pin.sym} 1530 -820 0 0 {name=p43 lab=D}
C {lab_pin.sym} 780 -400 0 0 {name=p44 lab=CK3}
C {inv_2.sym} 430 -380 0 0 {name=x2 VGND=VGND VNB=VNB VPB=VPB VPWR=VPWR prefix=sky130_fd_sc_hd__ }
C {lab_pin.sym} 470 -380 0 1 {name=p45 lab=DN}
C {lab_pin.sym} 810 -270 0 1 {name=p46 lab=DN}
