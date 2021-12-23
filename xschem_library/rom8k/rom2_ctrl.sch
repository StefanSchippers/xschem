v {xschem version=3.0.0 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
B 2 580 -140 1200 -30 {flags=1
y1 = 0
y2 = 2
divy = 2
x1=1.4e-07
x2=1.8e-07
divx=8
node="v(ldcp) v(xctrl.ldcp_ref) v(xctrl.ldprechref)"
color=12\\ 5\\ 13}
B 2 580 -230 1200 -140 {flags=1
y1 = 0
y2 = 2
divy = 2
x1=1.4e-07
x2=1.8e-07
divx=8
node="v(ldcp) v(xctrl.ldcpb)"
color=12\\ 5\\ 8}
B 2 580 -320 1200 -230 {flags=1
y1 = 0
y2 = 2
divy = 2
x1=1.4e-07
x2=1.8e-07
divx=8
node="v(ldcp) v(xctrl.ldouti) v(xctrl.ldoutib)"
color=12\\ 5\\ 8}
N 370 -560 510 -560 {lab=LDCPB}
N 250 -650 260 -650 {lab=LDCPB}
N 250 -650 250 -560 {lab=LDCPB}
N 210 -560 250 -560 {lab=LDCPB}
N 250 -760 250 -650 {lab=LDCPB}
N 250 -760 260 -760 {lab=LDCPB}
N 370 -400 430 -400 {lab=LDCPB}
N 910 -850 910 -790 {lab=#net1}
N 810 -760 910 -790 {lab=#net1}
N 810 -760 810 -710 {lab=#net1}
N 910 -750 910 -690 {lab=#net2}
N 810 -780 910 -750 {lab=#net2}
N 810 -830 810 -780 {lab=#net2}
N 1360 -130 1360 -80 {lab=VSS}
N 990 -850 1010 -850 {lab=LDOUTIB}
N 1080 -400 1120 -400 {lab=LDPRECHREF}
N 370 -560 370 -400 {lab=LDCPB}
N 420 -760 440 -760 {lab=#net3}
N 250 -560 370 -560 {lab=LDCPB}
N 1360 -210 1360 -190 {lab=#net4}
N 1320 -240 1320 -160 {lab=LDOUTI}
N 1360 -390 1360 -370 {lab=LDYMSREF}
N 1360 -290 1360 -270 {lab=#net5}
N 1520 -210 1520 -190 {lab=#net6}
N 1480 -240 1480 -160 {lab=LDOUTI}
N 1360 -130 1520 -130 {lab=VSS}
N 1520 -290 1520 -270 {lab=#net7}
C {title.sym} 170 0 0 0 {name=l3 author="Stefan Schippers"}
C {opin.sym} 260 -230 0 0 { name=p1 lab=LDPRECH }
C {opin.sym} 260 -210 0 0 { name=p2 lab=LDSAL}
C {opin.sym} 260 -190 0 0 { name=p3 lab=LDCP_ROWDEC }
C {opin.sym} 260 -170 0 0 { name=p4 lab=LDCP_SA}
C {opin.sym} 260 -150 0 0 { name=p5 lab=LDCP_ADDLAT_B }
C {opin.sym} 260 -130 0 0 { name=p6 lab=LDCP_COL_B }
C {ipin.sym} 210 -170 0 0 { name=p49 lab=LDEN_LAT }
C {ipin.sym} 210 -150 0 0 { name=p50 lab=LDCP }
C {ipin.sym} 210 -130 0 0 { name=p51 lab=VCC }
C {ipin.sym} 210 -110 0 0 { name=p52 lab=VSS }
C {lab_wire.sym} 270 -560 0 0 {name=l19 lab=LDCPB}
C {capa.sym} 280 -530 0 0 {name=c84 m=1 value=5f}
C {lab_pin.sym} 280 -500 0 0 {name=p1109 lab=VSS}
C {lab_pin.sym} 340 -650 0 1 {name=p1111 lab=LDCP_SA}
C {lvnand2.sym} 150 -560 0 0 {name=x392 m=1 
+ wna=90u lna=2.4u wpa=60u lpa=2.4u
+ wnb=90u lnb=2.4u wpb=60u lpb=2.4u
+ VCCPIN=vcc VSSPIN=vss }
C {lab_pin.sym} 110 -580 0 0 {name=p1113 lab=LDCP}
C {lab_pin.sym} 110 -540 0 0 {name=p1114 lab=LDEN_LAT}
C {lab_pin.sym} 420 -890 0 1 {name=p1115 lab=LDCP_ADDLAT_B}
C {lvnot.sym} 300 -650 0 0 {name=x394 m=1 
+ wn=8.4u lln=2.8u wp=40u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss
}
C {lvnot.sym} 480 -760 0 0 {name=x395 m=10
+ wn=15u lln=2.4u wp=40u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 300 -760 0 0 {name=x396 m=1 
+ wn=15u lln=2.4u wp=40u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 380 -760 0 0 {name=x397 m=4 
+ wn=15u lln=2.4u wp=40u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lab_pin.sym} 520 -760 0 1 {name=p1116 lab=LDCP_ROWDEC}
C {lab_pin.sym} 590 -400 0 1 {name=p1117 lab=LDCP_COL_B}
C {lvnot.sym} 380 -890 0 0 {name=x405 m=1 
+ wn=30u lln=2.4u wp=80u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lab_pin.sym} 340 -890 0 0 {name=p1118 lab=LDCP}
C {lab_pin.sym} 590 -560 0 1 {name=p7 lab=LDCP_REF}
C {lab_pin.sym} 990 -690 0 1 {name=p22 lab=LDOUTI}
C {lvnot.sym} 900 -520 0 0 {name=x7 m=1 
+ wn=24u lln=2.4u wp=40u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lab_pin.sym} 860 -520 0 0 {name=p10 lab=VSS}
C {lvnor2.sym} 1020 -400 0 0 {name=x8 m=1 
+ wna=8.4u lna=2.4u wpa=70u lpa=2.4u
+ wnb=8.4u lnb=2.4u wpb=70u lpb=2.4u
+ VCCPIN=VCC VSSPIN=VSS}
C {lab_pin.sym} 980 -380 0 0 {name=p29 lab=LDOUTI}
C {lab_pin.sym} 940 -520 0 1 {name=p30 lab=LDCP_REF_B}
C {lvnand2.sym} 850 -850 0 0 {name=x18 m=1 
+ wna=20u lna=2.4u wpa=36u lpa=2.4u
+ wnb=20u lnb=2.4u wpb=36u lpb=2.4u
+ VCCPIN=VCC VSSPIN=VSS}
C {lvnand2.sym} 850 -690 2 1 {name=x3 m=1 
+ wna=30u lna=2.4u wpa=30u lpa=2.4u
+ wnb=30u lnb=2.4u wpb=30u lpb=2.4u
+ VCCPIN=VCC VSSPIN=VSS}
C {lab_pin.sym} 810 -670 0 0 {name=p11 lab=LDCP_REF}
C {lab_pin.sym} 810 -870 0 0 {name=p43 lab=LDQ_B}
C {lab_pin.sym} 1360 -80 0 0 {name=p63 lab=VSS}
C {nlv.sym} 1340 -160 0 0 {name=m15 model=cmosn w=4u l=2.4u m=1}
C {lvnand3.sym} 1280 -680 0 0 {name=x25 m=1 
+ wn=80u lln=2.4u wp=40u lp=2.4u
+ VCCPIN=VCC VSSPIN=VSS}
C {lab_pin.sym} 1240 -700 0 0 {name=p24 lab=LDCP_REF}
C {lab_pin.sym} 1240 -680 0 0 {name=p27 lab=LDQ_B}
C {lab_pin.sym} 1240 -660 0 0 {name=p66 lab=LDOUTI}
C {lab_pin.sym} 1420 -680 0 1 {name=p67 lab=LDSAL}
C {lvnot.sym} 1380 -680 0 0 {name=x26 m=4 
+ wn=13u lln=2.4u wp=40u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lab_pin.sym} 1420 -860 0 1 {name=p68 lab=LDPRECH}
C {lvnot.sym} 1380 -860 0 0 {name=x28 m=8 
+ wn=13u lln=2.4u wp=40u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lab_pin.sym} 1010 -850 0 1 {name=p15 lab=LDOUTIB}
C {lvnot.sym} 550 -560 0 0 {name=x4 m=2 
+ wn=13u lln=2.4u wp=40u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 550 -400 0 0 {name=x1 m=2 
+ wn=20u lln=2.4u wp=44u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 470 -400 0 0 {name=x6 m=1 
+ wn=8.4u lln=2.4u wp=16u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {iopin.sym} 260 -110 0 0 { name=p20 lab=LDYMSREF }
C {lvnot.sym} 950 -850 0 0 {name=x5 m=1 
+ wn=30u lln=2.4u wp=40u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lvnot.sym} 950 -690 0 0 {name=x9 m=2 
+ wn=12u lln=2.4u wp=30u lp=2.4u
+ VCCPIN=vcc VSSPIN=vss}
C {lab_pin.sym} 1600 -500 0 1 {name=p17 lab=LDYMSREF}
C {lab_pin.sym} 1600 -520 0 1 {name=p18 lab=LDQ_B}
C {lab_pin.sym} 1300 -520 0 0 {name=p21 lab=LDPRECHREF}
C {lab_pin.sym} 1300 -500 0 0 {name=p23 lab=VCC}
C {lab_pin.sym} 1300 -480 0 0 {name=p26 lab=VSS}
C {lab_pin.sym} 1600 -540 0 1 {name=p32 lab=LDQI}
C {lab_pin.sym} 1120 -400 0 1 {name=p9 lab=LDPRECHREF}
C {noconn.sym} 1580 -640 0 0 {name=l1}
C {lab_pin.sym} 1580 -640 0 1 {name=p33 lab=LDQI}
C {lab_pin.sym} 1360 -390 0 1 {name=p12 lab=LDYMSREF}
C {rom2_sacell.sym} 1450 -510 0 0 {name=xsacell}
C {lab_pin.sym} 1320 -160 0 0 {name=p13 lab=LDOUTI}
C {spice_probe.sym} 590 -560 0 0 {name=p95 analysis=tran}
C {spice_probe.sym} 1080 -400 0 0 {name=p19 analysis=tran}
C {spice_probe.sym} 990 -690 0 0 {name=p28 analysis=tran}
C {spice_probe.sym} 990 -850 0 0 {name=p31 analysis=tran}
C {spice_probe.sym} 1600 -540 0 0 {name=p34 analysis=tran}
C {spice_probe.sym} 810 -870 0 1 {name=p35 analysis=tran}
C {nlv.sym} 1340 -240 0 0 {name=m1 model=cmosn w=4u l=2.4u m=1}
C {lvnand3.sym} 1280 -860 0 0 {name=x2 m=1 
+ wn=80u lln=2.4u wp=60u lp=2.4u
+ VCCPIN=VCC VSSPIN=VSS}
C {lab_pin.sym} 1240 -840 0 0 {name=p36 lab=LDCP}
C {lab_pin.sym} 1240 -860 0 0 {name=p37 lab=LDEN_LAT}
C {lab_pin.sym} 1240 -880 0 0 {name=p38 lab=LDOUTIB}
C {lab_pin.sym} 980 -420 0 0 {name=p8 lab=LDCPB}
C {nlv.sym} 1500 -160 0 0 {name=m2 model=cmosn w=4u l=2.4u m=1}
C {lab_pin.sym} 1480 -160 0 0 {name=p0 lab=LDOUTI}
C {nlv.sym} 1500 -240 0 0 {name=m0 model=cmosn w=4u l=2.4u m=1}
C {spice_probe.sym} 410 -560 0 0 {name=p25 analysis=tran}
C {lab_pin.sym} 1300 -540 0 0 {name=p14 lab=LDCPB}
