v {xschem version=2.9.8 file_version=1.2}
G {}
K {}
V {}
S {}
E {}
L 4 120 -370 120 -360 {}
L 4 120 -180 120 -170 {}
L 4 580 -370 580 -360 {}
L 4 580 -180 580 -170 {}
B 7 115 -360 125 -350 {}
B 7 115 -170 125 -160 {}
B 7 575 -360 585 -350 {}
B 7 575 -170 585 -160 {}
P 4 5 120 -390 110 -380 120 -370 130 -380 120 -390 {}
P 4 5 120 -200 110 -190 120 -180 130 -190 120 -200 {}
P 4 5 580 -390 570 -380 580 -370 590 -380 580 -390 {}
P 4 5 580 -200 570 -190 580 -180 590 -190 580 -200 {}
P 4 5 70 -410 70 -150 630 -150 630 -410 70 -410 {}
T {MANUFACTURING NOTES
REFERENCE: CONN1 IS IN THE LOWER LEFT

LED8 (POWER) IS ALIGNED POS LEFT NEG RIGHT
LED2 AND LED1 ARE ALIGNED POS DOWN NEG UP
LED3/4/5/6 ARE ALIGNED POS LEFT NEG RIGHT
LED7 IS ALIGNED POS LEFT NEG RIGHT
I.E. BEVELED EDGE ON SILKSCREEN = PIN1 = ANODE

U1 PIN 1 IS UPPER RIGHT
U2 PIN 1 IS UPPER LEFT
X1 MAY BE INSTALLED EITHER ORIENTATION

ALL CAPACITORS, RESISTORS, AND L1 ARE NONPOLAR
AND MAY BE INSTALLED EITHER ORIENTATION} 1345 -485 0 0 0.5 0.5 {}
T {PCB MATERIAL FR4 OR EQUIV 0.031 OR
0.062 INCH COPPER ANY FROM 0.5 to
2.0 OZ SOLDERMASK BOTH SIDES DIFFERENT
SILKSCREEN BOTH SIDES DIFFERENT
SOLDERMASK AND SILKSCREEN ANY COLOR 
AS LONG AS THEY CONTRAST
DRILL TOLERANCE +- 0.002 INCH

PCB FINAL SIZE IS 1.0 INCH BY 2.0 INCH} 715 -485 0 0 0.5 0.5 {}
T {FIDUCIAL LOCATIONS ARE IN MILS
(0.001 INCH) RELATIVE TO THE LOWER
LEFT (CONN1 LOCATION) OF THE PCB

FIDUCIALS ARE EXPOSED 0.025 INCH
SQUARE COPPER WITH CENTERPOINTS
NOTED AS BELOW} 70 -670 0 0 0.5 0.5 {}
T {X = 1890
Y = 840} 440 -380 0 0 0.5 0.5 {}
T {X = 1950
Y = 50} 440 -220 0 0 0.5 0.5 {}
T {X = 60
Y = 910} 140 -390 0 0 0.5 0.5 {}
T {X = 50
Y = 50} 140 -230 0 0 0.5 0.5 {}
T {(CONN1)} 70 -140 0 0 0.5 0.5 {}
T {TP1} 105 -405 0 0 0.3 0.3 {layer=8}
T {TP2} 105 -215 0 0 0.3 0.3 {layer=8}
T {TP4} 565 -215 0 0 0.3 0.3 {layer=8}
T {TP3} 565 -405 0 0 0.3 0.3 {layer=8}
T {ALL RESISTORS ARE 5% OR BETTER
ALL CAPACITORS ARE 10V X5R OR BETTER
SUBSTITUTIONS TO SIMILAR OR HIGHER
SPEC PARTS IS ALLOWED.
DNP MEANS DO NOT POPULATE} 715 -185 0 0 0.5 0.5 {}
T {1 x 2 in R8C/33C Lab/Breadboard helper. P0 and P3 drive the breadboard, 
giving GPIO, ADC, DAC, SPI, I2C, and UART on the breadboard. 
P1 is used for serial mode 2 programming and UART communications via USB
(with HW flow control), as well as driving four discrete LEDs. 
P2's PWM outputs drive an RGB LED. } 385 -2185 0 0 0.7 0.7 {}
T {R8C/33c DevCon Lab Board} 690 -2325 0 0 1 1 {}
T {Copyright (C) 2010 DJ Delorie (dj delorie com)
Distributed under the terms of the GNU General
Public License, either version 2 or (at your choice)
any later version.} 2240 -370 0 0 0.7 0.7 {layer=8}
T {ctrl-left-click
for datasheet} 930 -1550 0 0 0.4 0.4 {layer=8}
T {ctrl-left-click
for datasheet} 2470 -1580 0 0 0.4 0.4 {layer=8}
N 290 -1440 640 -1440 {lab=USBDM}
N 420 -1400 640 -1400 {lab=USBDP}
N 420 -1410 420 -1400 {lab=USBDP}
N 290 -1410 420 -1410 {lab=USBDP}
N 290 -1850 290 -1470 {lab=RAW_5V}
N 290 -1850 410 -1850 {lab=RAW_5V}
N 470 -1850 470 -1790 {lab=USB_5V}
N 410 -1850 410 -1790 {lab=RAW_5V}
N 580 -1850 670 -1850 {lab=USB_5V}
N 700 -1800 700 -1790 {lab=#net1}
N 700 -1800 770 -1800 {lab=#net1}
N 1080 -1280 1150 -1280 {lab=PWREN}
N 1150 -1800 1150 -1280 {lab=PWREN}
N 700 -1810 700 -1800 {lab=#net1}
N 830 -1800 1150 -1800 {lab=PWREN}
N 730 -1850 970 -1850 {lab=+5V}
N 970 -1880 970 -1850 {lab=+5V}
N 1250 -1850 1370 -1850 {lab=+5V}
N 1250 -1760 1250 -1740 {lab=#net2}
N 1250 -1850 1250 -1820 {lab=+5V}
N 1080 -1440 1250 -1440 {lab=URX}
N 1250 -1680 1250 -1440 {lab=URX}
N 1370 -1760 1370 -1740 {lab=#net3}
N 1370 -1850 1370 -1820 {lab=+5V}
N 1370 -1680 1370 -1400 {lab=UTX}
N 1080 -1400 1370 -1400 {lab=UTX}
N 970 -1850 1250 -1850 {lab=+5V}
N 1490 -1880 1490 -1850 {lab=+5V}
N 1630 -1800 1700 -1800 {lab=+5V}
N 1630 -1880 1630 -1800 {lab=+5V}
N 1700 -1800 1700 -1770 {lab=+5V}
N 1700 -1770 1760 -1770 {lab=+5V}
N 1760 -1800 1760 -1770 {lab=+5V}
N 1760 -1800 1920 -1800 {lab=+5V}
N 1920 -1130 2050 -1130 {lab=+5V}
N 1920 -1370 1920 -1130 {lab=+5V}
N 1920 -1370 2050 -1370 {lab=+5V}
N 1920 -1800 1920 -1370 {lab=+5V}
N 1080 -1360 1500 -1360 {lab=MODE}
N 1500 -1330 2050 -1330 {lab=MODE}
N 1500 -1360 1500 -1330 {lab=MODE}
N 1080 -1200 1520 -1200 {lab=RESET}
N 1520 -1290 1520 -1200 {lab=RESET}
N 1520 -1290 2050 -1290 {lab=RESET}
N 2150 -2110 2150 -2090 {lab=+5V}
N 2060 -2090 2150 -2090 {lab=+5V}
N 2060 -2140 2060 -2090 {lab=+5V}
N 2170 -2110 2170 -2070 {lab=GND}
N 2190 -2110 2190 -1860 {lab=#net4}
N 2170 -1860 2190 -1860 {lab=#net4}
N 2170 -1860 2170 -1570 {lab=#net4}
N 2210 -2110 2210 -1570 {lab=#net5}
N 2250 -1860 2250 -1570 {lab=#net6}
N 2230 -1860 2250 -1860 {lab=#net6}
N 2230 -2110 2230 -1860 {lab=#net6}
N 2290 -1880 2290 -1570 {lab=#net7}
N 2250 -1880 2290 -1880 {lab=#net7}
N 2250 -2110 2250 -1880 {lab=#net7}
N 2330 -1900 2330 -1570 {lab=#net8}
N 2270 -1900 2330 -1900 {lab=#net8}
N 2270 -2110 2270 -1900 {lab=#net8}
N 2370 -1920 2370 -1570 {lab=#net9}
N 2290 -1920 2370 -1920 {lab=#net9}
N 2290 -2110 2290 -1920 {lab=#net9}
N 2410 -1940 2410 -1570 {lab=#net10}
N 2310 -1940 2410 -1940 {lab=#net10}
N 2310 -2110 2310 -1940 {lab=#net10}
N 2450 -1960 2450 -1570 {lab=#net11}
N 2330 -1960 2450 -1960 {lab=#net11}
N 2330 -2110 2330 -1960 {lab=#net11}
N 2170 -890 2170 -860 {lab=P3.5}
N 2090 -860 2170 -860 {lab=P3.5}
N 2210 -890 2210 -830 {lab=P3.4}
N 2090 -830 2210 -830 {lab=P3.4}
N 2250 -890 2250 -800 {lab=P3.3}
N 2090 -800 2250 -800 {lab=P3.3}
N 2020 -1090 2050 -1090 {lab=P3.7}
N 2350 -1980 2580 -1980 {lab=P3.3}
N 2350 -2110 2350 -1980 {lab=P3.3}
N 2370 -2010 2580 -2010 {lab=P3.4}
N 2370 -2110 2370 -2010 {lab=P3.4}
N 2490 -2040 2580 -2040 {lab=P3.5}
N 2390 -2110 2390 -2040 {lab=P3.5}
N 2550 -2070 2580 -2070 {lab=P3.7}
N 2410 -2110 2410 -2070 {lab=P3.7}
N 2490 -2140 2490 -2040 {lab=P3.5}
N 2550 -2140 2550 -2070 {lab=P3.7}
N 80 -900 80 -870 {lab=+5V}
N 270 -1240 640 -1240 {lab=GND}
N 270 -1240 270 -860 {lab=GND}
N 340 -1200 640 -1200 {lab=3V3OUT}
N 340 -1200 340 -1150 {lab=3V3OUT}
N 340 -1090 340 -860 {lab=GND}
N 410 -1160 640 -1160 {lab=GND}
N 410 -1160 410 -860 {lab=GND}
N 580 -1850 580 -1580 {lab=USB_5V}
N 720 -1580 800 -1580 {lab=USB_5V}
N 800 -1580 800 -1540 {lab=USB_5V}
N 720 -1580 720 -1540 {lab=USB_5V}
N 2570 -1170 2680 -1170 {lab=TXD}
N 2680 -1680 2680 -1170 {lab=TXD}
N 1670 -1680 2680 -1680 {lab=TXD}
N 1670 -1680 1670 -920 {lab=TXD}
N 1080 -920 1670 -920 {lab=TXD}
N 2570 -1130 2720 -1130 {lab=RTS}
N 2720 -1710 2720 -1130 {lab=RTS}
N 1610 -1710 2720 -1710 {lab=RTS}
N 1610 -1710 1610 -1000 {lab=RTS}
N 1080 -1000 1610 -1000 {lab=RTS}
N 2570 -1090 2750 -1090 {lab=CTS}
N 2750 -1740 2750 -1090 {lab=CTS}
N 1580 -1740 2750 -1740 {lab=CTS}
N 1580 -1740 1580 -1160 {lab=CTS}
N 1080 -1160 1580 -1160 {lab=CTS}
N 2570 -1210 2640 -1210 {lab=RXD}
N 2640 -1650 2640 -1210 {lab=RXD}
N 1720 -1650 2640 -1650 {lab=RXD}
N 1720 -1650 1720 -960 {lab=RXD}
N 1080 -960 1720 -960 {lab=RXD}
N 2570 -1370 2880 -1370 {lab=#net12}
N 3000 -1370 3030 -1370 {lab=GND}
N 3030 -1040 3030 -1010 {lab=GND}
N 3000 -1260 3030 -1260 {lab=GND}
N 3000 -1150 3030 -1150 {lab=GND}
N 3000 -1040 3030 -1040 {lab=GND}
N 2860 -1260 2880 -1260 {lab=#net13}
N 2860 -1330 2860 -1260 {lab=#net13}
N 2570 -1330 2860 -1330 {lab=#net13}
N 2840 -1150 2880 -1150 {lab=#net14}
N 2840 -1290 2840 -1150 {lab=#net14}
N 2570 -1290 2840 -1290 {lab=#net14}
N 2810 -1040 2880 -1040 {lab=#net15}
N 2810 -1250 2810 -1040 {lab=#net15}
N 2570 -1250 2810 -1250 {lab=#net15}
N 1960 -1210 2050 -1210 {lab=GND}
N 1770 -1250 2050 -1250 {lab=XOUT}
N 1770 -1030 1770 -890 {lab=XOUT}
N 1960 -1170 2050 -1170 {lab=XIN}
N 1960 -1030 1960 -890 {lab=XIN}
N 1890 -1030 1960 -1030 {lab=XIN}
N 1770 -1030 1830 -1030 {lab=XOUT}
N 470 -1850 580 -1850 {lab=USB_5V}
N 2390 -2040 2490 -2040 {lab=P3.5}
N 2410 -2070 2550 -2070 {lab=P3.7}
N 580 -1580 720 -1580 {lab=USB_5V}
N 3030 -1370 3030 -1260 {lab=GND}
N 1770 -1250 1770 -1030 {lab=XOUT}
N 1960 -1170 1960 -1030 {lab=XIN}
N 3030 -1260 3030 -1150 {lab=GND}
N 3030 -1150 3030 -1040 {lab=GND}
N 1890 -890 1960 -890 {lab=XIN}
N 1770 -890 1830 -890 {lab=XOUT}
N 2540 -760 2620 -760 {lab=#net16}
N 2540 -680 2570 -680 {lab=#net17}
N 2570 -730 2570 -680 {lab=#net17}
N 2570 -730 2620 -730 {lab=#net17}
N 2540 -830 2570 -830 {lab=#net18}
N 2570 -830 2570 -790 {lab=#net18}
N 2570 -790 2620 -790 {lab=#net18}
N 2370 -830 2480 -830 {lab=#net19}
N 2370 -890 2370 -830 {lab=#net19}
N 2330 -760 2480 -760 {lab=#net20}
N 2330 -890 2330 -760 {lab=#net20}
N 2290 -680 2480 -680 {lab=#net21}
N 2290 -890 2290 -680 {lab=#net21}
N 2680 -790 2750 -790 {lab=GND}
N 2750 -730 2750 -670 {lab=GND}
N 2680 -760 2750 -760 {lab=GND}
N 2680 -730 2750 -730 {lab=GND}
N 2750 -790 2750 -760 {lab=GND}
N 2750 -760 2750 -730 {lab=GND}
N 190 -1290 190 -1270 {lab=#net22}
N 230 -1290 230 -1270 {lab=#net23}
C {title-2.sym} 0 0 0 0 {name=l1 
author="Stefan Frederik Schippers"
page=1
pages=1
title="R8C/R33C RULZ LAB BOARD"
lock=truex rev=1.1}
C {usb-minib.sym} 210 -1410 0 0 {name=CONN1
model=usb-minib
device=UX60SC-MB-5ST(80)
footprint=hirose-UX60SC
comptag="manufacturer_part_number UX60SC-MB-5ST(80)
manifacturer Hirose
vendor_part_number H11671CT-ND
vendor digikey"
}
C {conn_14x1.sym} 2280 -2130 3 1 {name=CONN2
footprint=JUMPER_14
value=DNP
comptag="manufacturer_part_number DNP
manifacturer DNP
vendor_part_number DNP
vendor DNP" device=CONNECTOR_14}
C {jumper.sym} 1730 -1800 1 1 {name=JP1
footprint=JUMPER2
device=JUMPER}
C {r8c-33c.sym} 2310 -1230 0 0 {name=U2 device=r8c-33c
footprint=TQFP32_7
url="https://www.renesas.com/en-eu/doc/products/mpumcu/doc/r8c/r01ds0008ej_r8c33c.pdf"
comptag="manufacturer_part_number R8C/33C
manifacturer Renesas
vendor_part_number R5F21336CNFP
vendor Renesas"}
C {ft232rl.sym} 860 -1180 0 0 {name=U1
url="https://www.ftdichip.com/Support/Documents/DataSheets/ICs/DS_FT232R.pdf"
device="USB to Serial converter"
footprint=SSOP28
comptag="manufacturer_part_number FT232RL\\ R
manifacturer FTDI
vendor_part_number 768-1007-1-ND
vendor digikey"}
C {gnd.sym} 290 -1350 0 0 {name=l2 lab=GND}
C {noconn.sym} 290 -1380 2 0 {name=l3}
C {noconn.sym} 190 -1270 3 0 {name=l4}
C {vdd.sym} 160 -1660 0 0 {name=l5 lab=+5V}
C {led.sym} 160 -1630 0 0 {name=LED8 
model=LED 
device=LTST-C190KGKT
footprint=0603 
value=GREEN
comptag="manufacturer_part_number LTST-C190KGKT
manifacturer Lite-on
vendor_part_number 160-1435-1-ND
vendor digikey"}
C {res.sym} 160 -1570 0 0 {name=R5
value=3.3k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX3301
manifacturer Rohm
vendor_part_number RHM3.30KHCT-ND
vendor digikey"}
C {gnd.sym} 160 -1540 0 0 {name=l6 lab=GND}
C {ind.sym} 440 -1850 3 1 {name=L1
value="600@100"
footprint=0603
device=INDUCTOR
comptag="manufacturer_part_number BLM18AG601SN1D
manifacturer Murata
vendor_part_number 490-1014-1-ND
vendor digikey"}
C {capa-2.sym} 470 -1760 0 0 {name=C2
value=4.7uF
footprint=0603
device=polarized_capacitor
comptag="manufacturer_part_number C0603C475K9PACTU
manifacturer Kemet
vendor_part_number 399-3482-1-ND
vendor digikey"}
C {gnd.sym} 410 -1730 0 0 {name=l7 lab=GND}
C {gnd.sym} 470 -1730 0 0 {name=l8 lab=GND}
C {lab_pin.sym} 290 -1850 0 0 {name=l9 sig_type=std_logic lab=RAW_5V}
C {capa.sym} 410 -1760 0 0 {name=C1
value=10nF
footprint=0603
device="capacitor"
comptag="manufacturer_part_number C0603C103K5RACTU
manifacturer Kemet
vendor_part_number 399-1091-1-ND
vendor digikey"}
C {pmos.sym} 700 -1830 1 1 {name=Q1 
model=DMP2035U 
device=DMP2035U
footprint=SOT-523
comptag="manufacturer_part_number RZE002P02TL
manifacturer Rohm
vendor_part_number RZE002P02TLCT-ND
vendor digikey" 
pinnumber(G)=1 
pinnumber(S)=3 
pinnumber(D)=2}
C {capa.sym} 700 -1760 0 0 {name=C3
value=100nF
footprint=0603
device="capacitor"
comptag="manufacturer_part_number C0603C104K4RACTU
manifacturer Kemet
vendor_part_number 399-1096-1-ND
vendor digikey"}
C {gnd.sym} 700 -1730 0 0 {name=l10 lab=GND}
C {res.sym} 800 -1800 3 1 {name=R1
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {vdd.sym} 970 -1880 0 0 {name=l11 lab=+5V}
C {led.sym} 1250 -1790 0 0 {name=LED1
model=LED 
device=LTST-C190KGKT
footprint=0603 
value=GREEN
comptag="manufacturer_part_number LTST-C190KGKT
manifacturer Lite-on
vendor_part_number 160-1435-1-ND
vendor digikey"}
C {res.sym} 1250 -1710 0 0 {name=R2
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {lab_wire.sym} 1210 -1440 0 0 {name=l12 sig_type=std_logic lab=URX}
C {led.sym} 1370 -1790 0 0 {name=LED2
model=LED 
device=LTST-C190KGKT
footprint=0603 
value=GREEN
comptag="manufacturer_part_number LTST-C190KGKT
manifacturer Lite-on
vendor_part_number 160-1435-1-ND
vendor digikey"}
C {res.sym} 1370 -1710 0 0 {name=R3
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {lab_wire.sym} 1210 -1400 0 0 {name=l13 sig_type=std_logic lab=UTX}
C {vdd.sym} 1490 -1880 0 0 {name=l14 lab=+5V}
C {capa.sym} 1490 -1820 0 0 {name=C5
value=100nF
footprint=0603
device="capacitor"
comptag="manufacturer_part_number C0603C104K4RACTU
manifacturer Kemet
vendor_part_number 399-1096-1-ND
vendor digikey"}
C {gnd.sym} 1490 -1790 0 0 {name=l15 lab=GND}
C {vdd.sym} 1630 -1880 0 0 {name=l16 lab=+5V}
C {lab_wire.sym} 1210 -1360 0 0 {name=l17 sig_type=std_logic lab=MODE}
C {lab_wire.sym} 1150 -1560 0 0 {name=l18 sig_type=std_logic lab=PWREN}
C {noconn.sym} 1080 -1320 2 0 {name=l19}
C {lab_wire.sym} 1210 -1200 0 0 {name=l20 sig_type=std_logic lab=RESET}
C {vdd.sym} 2060 -2140 0 0 {name=l21 lab=+5V}
C {gnd.sym} 2170 -2070 0 1 {name=l22 lab=GND}
C {lab_pin.sym} 2090 -860 0 0 {name=p25 lab=P3.5}
C {lab_pin.sym} 2090 -830 0 0 {name=p26 lab=P3.4}
C {lab_pin.sym} 2090 -800 0 0 {name=p27 lab=P3.3}
C {lab_pin.sym} 2020 -1090 0 0 {name=p1 lab=P3.7}
C {lab_pin.sym} 2580 -2040 0 1 {name=p2 lab=P3.5}
C {lab_pin.sym} 2580 -2010 0 1 {name=p3 lab=P3.4}
C {lab_pin.sym} 2580 -1980 0 1 {name=p4 lab=P3.3}
C {lab_pin.sym} 2580 -2070 0 1 {name=p5 lab=P3.7}
C {res.sym} 2490 -2170 0 0 {name=R4
value=3.3k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX3301
manifacturer Rohm
vendor_part_number RHM3.30KHCT-ND
vendor digikey"}
C {vdd.sym} 2490 -2200 0 0 {name=l23 lab=+5V}
C {res.sym} 2550 -2170 0 0 {name=R6
value=3.3k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX3301
manifacturer Rohm
vendor_part_number RHM3.30KHCT-ND
vendor digikey"}
C {vdd.sym} 2550 -2200 0 0 {name=l24 lab=+5V}
C {vdd.sym} 80 -900 0 0 {name=l25 lab=+5V}
C {capa.sym} 80 -840 0 0 {name=C6
value=100nF
footprint=0603
device="capacitor"
comptag="manufacturer_part_number C0603C104K4RACTU
manifacturer Kemet
vendor_part_number 399-1096-1-ND
vendor digikey"}
C {gnd.sym} 80 -810 0 0 {name=l26 lab=GND}
C {gnd.sym} 270 -860 0 0 {name=l27 lab=GND}
C {lab_wire.sym} 590 -1200 0 0 {name=l28 sig_type=std_logic lab=3V3OUT}
C {capa.sym} 340 -1120 0 0 {name=C4
value=100nF
footprint=0603
device="capacitor"
comptag="manufacturer_part_number C0603C104K4RACTU
manifacturer Kemet
vendor_part_number 399-1096-1-ND
vendor digikey"}
C {gnd.sym} 340 -860 0 0 {name=l29 lab=GND}
C {gnd.sym} 410 -860 0 0 {name=l30 lab=GND}
C {lab_wire.sym} 560 -1440 0 0 {name=l35 lab=USBDM}
C {lab_wire.sym} 560 -1400 0 0 {name=l36 lab=USBDP}
C {noconn.sym} 640 -1280 0 0 {name=l31}
C {noconn.sym} 640 -1320 0 0 {name=l32}
C {noconn.sym} 640 -1360 0 0 {name=l33}
C {noconn.sym} 1080 -1120 0 1 {name=l34}
C {noconn.sym} 1080 -1080 0 1 {name=l37}
C {noconn.sym} 1080 -1040 0 1 {name=l38}
C {gnd.sym} 720 -820 0 0 {name=l39 lab=GND}
C {gnd.sym} 780 -820 0 0 {name=l40 lab=GND}
C {gnd.sym} 840 -820 0 0 {name=l41 lab=GND}
C {lab_wire.sym} 570 -1850 0 0 {name=l42 lab=USB_5V}
C {res.sym} 2910 -1370 3 1 {name=R7
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {led.sym} 2970 -1370 3 0 {name=LED3
model=LED 
device=LTST-C190KGKT
footprint=0603 
value=GREEN
comptag="manufacturer_part_number LTST-C190KGKT
manifacturer Lite-on
vendor_part_number 160-1435-1-ND
vendor digikey"}
C {res.sym} 2910 -1260 3 1 {name=R8
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {led.sym} 2970 -1260 3 0 {name=LED4
model=LED 
device=LTST-C190KGKT
footprint=0603 
value=GREEN
comptag="manufacturer_part_number LTST-C190KGKT
manifacturer Lite-on
vendor_part_number 160-1435-1-ND
vendor digikey"}
C {res.sym} 2910 -1150 3 1 {name=R9
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {led.sym} 2970 -1150 3 0 {name=LED5
model=LED 
device=LTST-C190KGKT
footprint=0603 
value=GREEN
comptag="manufacturer_part_number LTST-C190KGKT
manifacturer Lite-on
vendor_part_number 160-1435-1-ND
vendor digikey"}
C {res.sym} 2910 -1040 3 1 {name=R10
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {led.sym} 2970 -1040 3 0 {name=LED6
model=LED 
device=LTST-C190KGKT
footprint=0603 
value=GREEN
comptag="manufacturer_part_number LTST-C190KGKT
manifacturer Lite-on
vendor_part_number 160-1435-1-ND
vendor digikey"}
C {gnd.sym} 3030 -1010 0 0 {name=l43 lab=GND}
C {gnd.sym} 1960 -1210 0 0 {name=l44 lab=GND}
C {res.sym} 1860 -1030 3 1 {name=R15
value=1MEG
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1004
manifacturer Rohm
vendor_part_number RHM1.00MHCT-ND
vendor digikey"}
C {rgb_led.sym} 2650 -760 0 0 {name=LED7 model=XXX device=RGB_LED value=RGB footprint=LRTB-R98G comptag="manufacturer_part_number LRTB\\ R98G-R7T5-1+S7U-37+P7R-26
manifacturer OSRAM
vendor digikey"}
C {res.sym} 2510 -830 3 1 {name=R11
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {res.sym} 2510 -760 3 1 {name=R12
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {res.sym} 2510 -680 3 1 {name=R13
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {gnd.sym} 2750 -670 0 0 {name=l45 lab=GND}
C {noconn.sym} 2410 -890 1 1 {name=l46}
C {noconn.sym} 2450 -890 1 1 {name=l47}
C {lab_wire.sym} 1210 -1160 0 0 {name=l48 sig_type=std_logic lab=CTS}
C {lab_wire.sym} 1190 -1000 0 1 {name=l49 lab=RTS}
C {lab_wire.sym} 1190 -960 0 1 {name=l50 lab=RXD}
C {lab_wire.sym} 1190 -920 0 1 {name=l51 lab=TXD}
C {lab_wire.sym} 2000 -1250 0 1 {name=l52 lab=XOUT}
C {lab_wire.sym} 2010 -1170 0 1 {name=l53 lab=XIN}
C {crystal-2.sym} 1860 -890 3 0 {name=X1
value=18.43MHz
footprint=res3-e15mm 300
device=CRYSTAL_module comptag="manufacturer_part_number AWSCR-18.43CV-T
manifacturer Abracon"}
C {gnd.sym} 1860 -860 0 0 {name=l54 lab=GND}
C {launcher.sym} 1670 -2300 0 0 {name=h1
descr="Ctrl-click to go to Delorie's 
project page for info" 
url="http://www.delorie.com/electronics/rulz"}
C {noconn.sym} 230 -1270 1 1 {name=l55}
