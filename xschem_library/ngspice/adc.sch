v {xschem version=3.0.0 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
P 4 5 670 -580 1650 -580 1650 -190 670 -190 670 -580 {dash=5}
P 5 5 10 -770 940 -770 940 -330 10 -330 10 -770 { dash=5}
T {Modulator} 500 -810 0 0 0.6 0.6 { layer=5}
T {Digital Decimator} 1000 -620 0 0 0.6 0.6 {layer=4}
T {D} 160 -630 0 0 1 1 { layer=5 font=Symbol}
T {S} 350 -750 0 0 1 1 { layer=5 font=Symbol}
T {D-S} 430 -805 0 0 0.6 0.6 { layer=5 font=Symbol}
N 890 -510 910 -510 {lab=Q}
N 910 -510 910 -350 {lab=Q}
N 1610 -530 1670 -530 { lab=CODE[5:0]}
N 1300 -530 1490 -530 { lab=COUNT[5:0]}
N 1150 -270 1150 -250 { lab=#net1}
N 1150 -330 1150 -310 { lab=#net2}
N 1420 -510 1490 -510 { lab=RSTI}
N 1350 -310 1390 -310 { lab=#net3}
N 1510 -370 1510 -330 { lab=RSTI}
N 1420 -370 1510 -370 { lab=RSTI}
N 1420 -510 1420 -370 { lab=RSTI}
N 610 -290 760 -290 { lab=CK}
N 610 -250 760 -250 { lab=RST}
N 640 -510 770 -510 { lab=COMP}
N 180 -350 910 -350 { lab=Q}
N 1050 -510 1140 -510 { lab=QN}
N 910 -510 970 -510 { lab=Q}
N 500 -540 520 -540 { lab=INTEG}
N 500 -540 500 -500 { lab=INTEG}
N 280 -470 290 -470 { lab=VREF}
N 180 -530 290 -530 {lab=FB}
N 430 -500 500 -500 { lab=INTEG}
N 400 -670 430 -670 { lab=INTEG}
N 430 -670 430 -500 { lab=INTEG}
N 290 -670 340 -670 { lab=FB}
N 90 -530 110 -530 { lab=SIG_IN}
N 290 -670 290 -530 { lab=FB}
N 180 -530 180 -480 { lab=FB}
N 180 -420 180 -350 { lab=Q}
N 410 -500 430 -500 { lab=INTEG}
N 170 -530 180 -530 {lab=FB}
N 1040 -370 1420 -370 { lab=RSTI}
N 1040 -400 1040 -370 { lab=RSTI}
N 1140 -490 1140 -420 { lab=#net4}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {lab_pin.sym} 770 -490 0 0 {name=p4 lab=CK}
C {lab_wire.sym} 740 -510 0 0 {name=l4 lab=COMP}
C {lab_pin.sym} 910 -430 0 1 {name=p8 lab=Q}
C {flip_flop_ngspice.sym} 830 -490 0 0 {name=x1}
C {lab_pin.sym} 770 -470 0 0 {name=p3 lab=0}
C {counter_6bit_ngspice.sym} 1220 -510 0 0 {name=x2}
C {lab_pin.sym} 1140 -530 0 0 {name=p5 lab=CK}
C {lab_pin.sym} 1040 -440 0 0 {name=p12 lab=RST}
C {flip_flop_ngspice.sym} 1550 -510 0 0 {name=x4[5:0]}
C {opin.sym} 1670 -530 0 0 {name=p20 lab=CODE[5:0]}
C {lab_wire.sym} 1320 -530 0 1 {name=l10 lab=COUNT[5:0]}
C {counter_6bit_ngspice.sym} 840 -270 0 0 {name=x4}
C {ipin.sym} 610 -290 0 0 {name=p10 lab=CK}
C {ipin.sym} 610 -250 0 0 {name=p11 lab=RST}
C {lab_pin.sym} 760 -270 0 0 {name=p14 lab=VCC}
C {lab_pin.sym} 920 -290 0 1 {name=p24 lab=C[5:0]}
C {and3_ngspice.sym} 1090 -330 0 0 {name=x5 ROUT=1000}
C {lab_pin.sym} 1050 -350 0 0 {name=p25 lab=C[5]}
C {lab_pin.sym} 1050 -330 0 0 {name=p26 lab=C[4]}
C {lab_pin.sym} 1050 -310 0 0 {name=p27 lab=C[3]}
C {lab_pin.sym} 1050 -250 0 0 {name=p28 lab=C[1]}
C {lab_pin.sym} 1050 -230 0 0 {name=p29 lab=C[0]}
C {and3_ngspice.sym} 1090 -250 0 0 {name=x6 ROUT=1000}
C {lab_pin.sym} 1050 -270 0 0 {name=p30 lab=C[2]}
C {and_ngspice.sym} 1190 -290 0 0 {name=x7 ROUT=1000}
C {or_ngspice.sym} 1290 -310 0 0 {name=x8 ROUT=1000}
C {lab_pin.sym} 1250 -330 0 0 {name=p31 lab=RST}
C {lab_pin.sym} 1510 -330 0 1 {name=p15 lab=RSTI}
C {flip_flop_ngspice.sym} 1450 -310 0 0 {name=x9}
C {lab_pin.sym} 1390 -290 0 0 {name=p32 lab=RST}
C {lab_pin.sym} 1390 -330 0 0 {name=p35 lab=VCC}
C {lab_pin.sym} 1490 -490 0 0 {name=p1 lab=RST}
C {spice_probe.sym} 810 -350 0 0 {name=p6 attrs=""}
C {spice_probe.sym} 680 -510 0 0 {name=p17 attrs=""}
C {spice_probe.sym} 1510 -370 0 0 {name=p18 attrs=""}
C {inv_ngspice.sym} 1010 -510 0 0 {name=x11 ROUT=1000}
C {lab_wire.sym} 1100 -510 0 0 {name=l2 lab=QN}
C {spice_probe.sym} 1060 -510 0 0 {name=p19 attrs=""}
C {ipin.sym} 280 -470 0 0 {name=p242 lab=VREF}
C {lab_pin.sym} 500 -520 0 0 {name=p244 lab=INTEG}
C {ipin.sym} 90 -530 0 0 {name=p260 lab=SIG_IN}
C {capa.sym} 370 -670 1 0 {name=c10 m=1 value="2e-12"}
C {res.sym} 140 -530 1 0 {name=R4
value=60k
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 520 -480 0 0 {name=p261 lab=VREF}
C {res.sym} 180 -450 0 0 {name=R5
value=60k
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 290 -560 0 0 {name=p262 lab=FB}
C {spice_probe.sym} 430 -600 0 0 {name=p264 attrs=""}
C {spice_probe.sym} 190 -530 0 0 {name=p265 attrs=""}
C {opamp_65nm.sym} 350 -500 2 1 {name=x41}
C {comp_65nm.sym} 580 -510 0 0 {name=x42}
C {spice_probe.sym} 1440 -530 0 0 {name=p2 attrs=""}
C {or_ngspice.sym} 1080 -420 0 0 {name=x3 ROUT=1000}
