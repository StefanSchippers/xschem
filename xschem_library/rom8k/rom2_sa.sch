v {xschem version=2.9.5 file_version=1.1}
G {}
V {}
S {}
E {}
T {Was:
60/24} 1420 -310 0 0 0.4 0.4 {}
T {was: LDCP_B} 510 -410 0 0 0.4 0.4 {}
N 190 -580 270 -580 {lab=LDSALI_B}
N 190 -630 190 -580 {lab=LDSALI_B}
N 190 -390 210 -390 {lab=LDCP_B}
N 1640 -370 1660 -370 {lab=LDQ}
N 1490 -370 1560 -370 {lab=LDQIII}
N 1180 -370 1180 -300 {lab=LDQIB}
N 1220 -530 1220 -510 {lab=vcc}
N 1220 -370 1220 -330 {lab=LDQII}
N 1220 -210 1220 -190 {lab=vss}
N 1320 -490 1400 -490 {lab=LDQII}
N 1480 -600 1480 -490 {lab=#net1}
N 1440 -540 1440 -520 {lab=LDSALI_B}
N 1440 -460 1440 -440 {lab=LDSALI}
N 1320 -490 1320 -370 {lab=LDQII}
N 1320 -600 1320 -490 {lab=LDQII}
N 1090 -370 1180 -370 {lab=LDQIB}
N 1180 -420 1180 -370 {lab=LDQIB}
N 1220 -370 1320 -370 {lab=LDQII}
N 1220 -390 1220 -370 {lab=LDQII}
N 1320 -370 1410 -370 {lab=LDQII}
N 1560 -370 1560 -330 {lab=LDQIII}
C {opin.sym} 200 -190 0 0 {name=p4 lab=LDQ}
C {title.sym} 170 0 0 0 {name=l3 author="Stefan Schippers"}
C {ipin.sym} 100 -290 0 0 {name=p1 lab=LDCP}
C {ipin.sym} 100 -250 0 0 {name=p2 lab=LDYMS}
C {ipin.sym} 100 -190 0 0 { name=p42 lab=LDOE }
C {ipin.sym} 100 -150 0 0 { name=p10 lab=LDPRECH }
C {ipin.sym} 100 -130 0 0 { name=p8 lab=LDSAL }
C {ipin.sym} 100 -110 0 0 {name=p17 lab=vcc}
C {ipin.sym} 100 -90 0 0 {name=p18 lab=vss}
C {lvnot.sym} 230 -630 0 0 {name=x242 m=1 
+ wn=8.4u lln=2.4u wp=12u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lab_pin.sym} 270 -630 0 1 {name=p25 lab=LDSALI}
C {lab_pin.sym} 270 -580 0 1 {name=p27 lab=LDSALI_B}
C {lab_pin.sym} 110 -630 0 0 {name=p741 lab=LDSAL}
C {lab_pin.sym} 210 -390 0 1 {name=p627 lab=LDCP_B}
C {lab_pin.sym} 110 -390 0 0 {name=p647 lab=LDCP}
C {lvnot.sym} 150 -390 0 0 {name=x249 m=1 
+ wn=8.4u lln=2.4u wp=20u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lab_pin.sym} 1660 -370 0 1 {name=p15 lab=LDQ}
C {lab_pin.sym} 1560 -400 0 0 {name=p16 lab=LDOE}
C {bts.sym} 1600 -370 0 0 {name=x6 m=1 VCCPIN=vcc VSSPIN=vss}
C {passhs.sym} 1440 -490 2 1 {name=x7 WN=12u WP=12u VCCPIN=vcc VSSPIN=vss}
C {lab_pin.sym} 1220 -530 0 0 {name=p19 lab=vcc}
C {nlv.sym} 1200 -300 0 0 {name=m6 model=cmosn w=24u l=2.4u m=1}
C {plv.sym} 1200 -420 0 0 {name=m7 model=cmosp w=24u l=2.4u m=1
}
C {nlv.sym} 1200 -240 0 0 {name=m4 model=cmosn w=24u l=2.4u m=1}
C {plv.sym} 1200 -480 0 0 {name=m1 model=cmosp w=24u l=2.4u m=1
}
C {lab_pin.sym} 1220 -190 0 0 {name=p20 lab=vss}
C {lvnot.sym} 1360 -600 0 0 {name=x8 m=1 
+ wn=8.4u lln=2.4u wp=12u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 1440 -600 0 0 {name=x9 m=1 
+ wn=8.4u lln=2.4u wp=12u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lab_pin.sym} 1180 -240 0 0 {name=p21 lab=LDSALI}
C {lab_pin.sym} 1180 -480 0 0 {name=p22 lab=LDSALI_B}
C {lab_pin.sym} 1440 -440 0 0 {name=p23 lab=LDSALI}
C {lab_pin.sym} 1440 -540 0 0 {name=p28 lab=LDSALI_B}
C {capa.sym} 1360 -340 0 0 {name=c0 m=1 value=3f}
C {lab_pin.sym} 1360 -310 0 0 {name=p29 lab=vss}
C {capa.sym} 1120 -340 0 0 {name=c1 m=1 value=3f}
C {lab_pin.sym} 1120 -310 0 0 {name=p31 lab=vss}
C {lab_pin.sym} 1320 -400 0 0 {name=p9 lab=LDQII}
C {lvnot.sym} 1450 -370 0 0 {name=x228 m=1 
+ wn=24u lln=2.4u wp=60u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 150 -630 0 0 {name=x1 m=1 
+ wn=8.4u lln=2.4u wp=12u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lab_pin.sym} 1180 -400 0 0 {name=p5 lab=LDQIB}
C {lab_pin.sym} 1560 -330 0 0 {name=p14 lab=LDQIII}
C {rom2_sacell.sym} 790 -350 0 0 {name=xsacell}
C {lab_pin.sym} 640 -360 0 0 {name=p24 lab=LDPRECH}
C {lab_pin.sym} 940 -360 0 1 {name=p32 lab=LDQIB}
C {lab_pin.sym} 940 -340 0 1 {name=p33 lab=LDYMS}
C {lab_pin.sym} 640 -340 0 0 {name=p34 lab=vcc}
C {lab_pin.sym} 640 -320 0 0 {name=p35 lab=vss}
C {lab_pin.sym} 940 -380 0 1 {name=p39 lab=LDQI}
C {spice_probe.sym} 1090 -370 0 1 {name=p95 analysis=tran}
C {spice_probe.sym} 1320 -600 3 1 {name=p3 analysis=tran}
C {spice_probe.sym} 200 -390 0 0 {name=p6 analysis=tran}
C {spice_probe.sym} 270 -630 0 0 {name=p7 analysis=tran}
C {spice_probe.sym} 940 -380 0 0 {name=p11 analysis=tran}
C {spice_probe.sym} 1560 -330 2 1 {name=p30 analysis=tran}
C {lab_pin.sym} 640 -380 0 0 {name=p12 lab=LDCP_B}
