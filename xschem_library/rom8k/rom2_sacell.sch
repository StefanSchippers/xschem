v {xschem version=2.9.7 file_version=1.2}
G {}
V {}
S {}
E {}
T {WAS: p: 40/2.4  p
     n:  8/2.4  n} 1030 -550 0 0 0.4 0.4 {}
T {WAS:
16u/2u x10 // 16u/2u x2} 570 -200 0 0 0.4 0.4 {}
T {WAS: p: 40/6.8  plvt
     n:  8/2.4  n} 1030 -620 0 0 0.4 0.4 {}
N 820 -450 850 -450 {lab=LDRESET}
N 890 -500 890 -480 {lab=VCC}
N 890 -370 890 -320 {lab=LDQI}
N 510 -340 510 -320 {lab=VCC}
N 510 -210 510 -190 {lab=LDYMS}
N 510 -210 890 -210 {lab=LDYMS}
N 890 -260 890 -210 {lab=LDYMS}
N 890 -370 1080 -370 {lab=LDQI}
N 890 -210 890 -110 {lab=LDYMS}
N 440 -160 470 -160 {lab=LDRESET}
N 510 -130 510 -110 {lab=VSS}
N 890 -420 890 -370 {lab=LDQI}
N 1080 -430 1080 -310 {lab=LDQI}
N 1120 -490 1120 -460 {lab=VCC}
N 1120 -400 1120 -340 {lab=LDQ_B}
N 1120 -370 1160 -370 {lab=LDQ_B}
N 1120 -280 1120 -250 {lab=VSS}
N 510 -260 510 -210 {lab=LDYMS}
C {ipin.sym} 130 -160 0 0 {name=p0 lab=LDRESET}
C {ipin.sym} 130 -140 0 0 {name=p1 lab=LDPRECH}
C {ipin.sym} 130 -120 0 0 {name=p2 lab=VCC}
C {ipin.sym} 130 -100 0 0 {name=p3 lab=VSS}
C {n.sym} 490 -290 0 0 {name=m3 model=cmosn w=WPRECH l=2u m=10}
C {n.sym} 870 -290 0 0 {name=m1 model=cmosn w=WPRECH l=2u m=2}
C {plv.sym} 870 -450 0 0 {name=m5 model=cmosp w=6.6u l=8.8u m=1}
C {lab_pin.sym} 890 -500 0 0 {name=p11 lab=VCC}
C {lab_pin.sym} 510 -340 0 0 {name=p13 lab=VCC}
C {lab_pin.sym} 890 -110 0 0 {name=p36 lab=LDYMS}
C {lab_pin.sym} 440 -160 0 0 {name=p37 lab=LDRESET}
C {nlv.sym} 490 -160 0 0 {name=m11 model=cmosn w=16u l=2.4u m=1}
C {lab_pin.sym} 510 -110 0 0 {name=p38 lab=VSS}
C {lab_pin.sym} 890 -400 0 1 {name=p8 lab=LDQI}
C {capa.sym} 1000 -340 0 0 {name=c2 m=1 value=3f}
C {lab_pin.sym} 1000 -310 0 0 {name=p9 lab=VSS}
C {title.sym} 160 0 0 0 {name=l3 author="Stefan Schippers"}
C {lab_pin.sym} 1160 -370 0 1 {name=p10 lab=LDQ_B}
C {opin.sym} 220 -160 0 0 {name=p14 lab=LDQI}
C {opin.sym} 220 -140 0 0 {name=p15 lab=LDQ_B}
C {iopin.sym} 220 -120 0 0 {name=p16 lab=LDYMS}
C {lab_pin.sym} 820 -450 0 0 {name=p7 lab=LDRESET}
C {lab_pin.sym} 850 -290 0 0 {name=p4 lab=VCC}
C {nlv.sym} 1100 -310 0 0 {name=m2 model=cmosn w=8u l=2.4u m=1}
C {p.sym} 1100 -430 0 0 {name=m4 model=cmosp w=40u l=2.4u m=1}
C {lab_pin.sym} 1120 -490 0 0 {name=p5 lab=VCC}
C {lab_pin.sym} 1120 -430 0 1 {name=p12 lab=VCC}
C {lab_pin.sym} 1120 -250 0 0 {name=p17 lab=VSS}
C {lab_pin.sym} 510 -290 0 1 {name=p18 lab=VSS}
C {lab_pin.sym} 890 -290 0 1 {name=p19 lab=VSS}
C {lab_pin.sym} 470 -290 0 0 {name=p6 lab=LDPRECH}
