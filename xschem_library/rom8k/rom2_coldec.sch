v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {}
S {}
E {}
N 430 -390 430 -370 {lab=VSS}
N 300 -480 320 -480 {lab=LDY1[15:0]}
N 360 -540 360 -510 {lab=LDBL[15:0]}
N 360 -450 430 -450 {lab=LDYMS}
N 360 -450 360 -390 {lab=LDYMS}
C {opin.sym} 490 -250 0 0 {name=p5 lab=LDYMS}
C {lab_pin.sym} 430 -370 0 1 {name=p408 lab=VSS}
C {lab_pin.sym} 300 -480 0 0 {name=p453 lab=LDY1[15:0]}
C {lab_pin.sym} 360 -540 0 1 {name=p664 lab=LDBL[15:0]}
C {nlv.sym} 340 -480 0 0 {name=m96[15:0] model=cmosn w=50u l=2u m=1
}
C {capa.sym} 430 -420 0 0 {name=c61 m=1 value=20f}
C {title.sym} 160 0 0 0 {name=l3 author="Stefan Schippers"}
C {lab_pin.sym} 360 -390 0 0 {name=p8 lab=LDYMS}
C {ipin.sym} 290 -260 0 0 {name=p11 lab=LDY1[15:0]}
C {ipin.sym} 290 -240 0 0 {name=p12 lab=LDBL[15:0]}
C {ipin.sym} 290 -220 0 0 {name=p13 lab=VSS}
