v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {}
S {}
E {}
N 650 -420 650 -390 {lab=LDWL_B[15:0]}
N 650 -590 650 -570 {lab=VCC}
N 650 -420 830 -420 {lab=LDWL_B[15:0]}
N 730 -360 730 -340 {lab=VSS}
N 650 -510 650 -420 {lab=LDWL_B[15:0]}
N 650 -270 650 -250 {lab=LDL1X_S}
N 120 -470 120 -440 {lab=LDL1X_S}
N 120 -320 120 -300 {lab=VSS}
N 120 -550 120 -530 {lab=VCC}
N 280 -550 280 -530 {lab=VCC}
N 120 -460 310 -460 {lab=LDL1X_S}
N 280 -470 280 -460 {lab=LDL1X_S}
C {title.sym} 170 0 0 0 {name=l3 author="Stefan Schippers"}
C {opin.sym} 290 -200 0 0 {name=p15 lab=LDWL[15:0]}
C {nlv.sym} 630 -300 0 0 {name=m0[15:0] model=cmosn w=14u l=2u m=1}
C {lab_pin.sym} 650 -590 0 0 {name=p16 lab=VCC}
C {lab_pin.sym} 730 -340 0 1 {name=p18 lab=VSS}
C {capa.sym} 730 -390 0 0 {name=c1[15:0] m=1 value=3f}
C {lvnot.sym} 870 -420 0 0 {name=x2[15:0] m=1 
+ wn=24u lln=2.4u wp=120u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {plv.sym} 630 -540 0 0 {name=m3[15:0] model=cmosp w=8.4u l=8.4u m=1
}
C {lab_pin.sym} 910 -420 0 1 {name=l0 lab=LDWL[15:0]}
C {lab_pin.sym} 610 -300 0 0 {name=p19 lab=LDL1X[15:0]}
C {lab_pin.sym} 650 -480 0 0 {name=l4 lab=LDWL_B[15:0]}
C {lab_pin.sym} 610 -540 0 0 {name=p23 lab=VSS}
C {nlv.sym} 630 -360 0 0 {name=m4[15:0] model=cmosn w=19u l=2.4u m=1}
C {lab_pin.sym} 610 -360 0 0 {name=p27 lab=LDCP}
C {lab_pin.sym} 650 -250 0 1 {name=p3 lab=LDL1X_S}
C {ipin.sym} 180 -210 0 0 {name=p1 lab=LDL1X[15:0]}
C {ipin.sym} 180 -190 0 0 {name=p2 lab=LDL2X}
C {ipin.sym} 180 -170 0 0 {name=p7 lab=LDL3X}
C {ipin.sym} 180 -150 0 0 {name=p8 lab=LDCP}
C {ipin.sym} 180 -130 0 0 {name=p9 lab=VSS}
C {ipin.sym} 180 -110 0 0 {name=p10 lab=VCC}
C {nlv.sym} 100 -350 0 0 {name=m1 model=cmosn w=40u l=2.4u m=1}
C {nlv.sym} 100 -410 0 0 {name=m2 model=cmosn w=40u l=2.4u m=1}
C {lab_pin.sym} 80 -410 0 0 {name=p11 lab=LDL2X}
C {lab_pin.sym} 240 -500 0 0 {name=p12 lab=LDL3X}
C {lab_pin.sym} 120 -300 0 0 {name=p13 lab=VSS}
C {lab_pin.sym} 120 -550 0 0 {name=p14 lab=VCC}
C {plv.sym} 100 -500 0 0 {name=m5 model=cmosp w=8.4u l=2.4u m=1
}
C {lab_pin.sym} 280 -550 0 0 {name=p17 lab=VCC}
C {plv.sym} 260 -500 0 0 {name=m6 model=cmosp w=8.4u l=2.4u m=1
}
C {lab_pin.sym} 310 -460 0 1 {name=p21 lab=LDL1X_S}
C {lab_pin.sym} 80 -500 0 0 {name=p20 lab=LDL2X}
C {lab_pin.sym} 80 -350 0 0 {name=p22 lab=LDL3X}
