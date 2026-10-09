v {xschem version=3.4.8RC file_version=1.2}
G {}
K {}
V {}
S {}
E {}
N 620 -430 640 -430 {lab=LDCP_B}
N 620 -320 640 -320 {lab=LDCP2_B}
N 620 -530 640 -530 {
lab=LDCP3_B}
N 200 -340 220 -340 {lab=LDCP4_B}
N 200 -100 220 -100 {
lab=LDCP5_B}
N 620 -210 640 -210 {lab=LDCP2_B}
C {lab_pin.sym} 640 -430 0 1 {name=p627 lab=LDCP_B}
C {lab_pin.sym} 540 -430 0 0 {name=p647 lab=LDCP}
C {test_parametric_ports/inv3.sym} 580 -430 0 0 {name=x2 m=1 
+ wn=8.4u lln=2.4u wp=20u lp=2.4u
+ VCCPIN=vccpin VSSPIN=vsspin
schematic=@symname\\_1.sch
modn=yyn modp=yyp}
C {spice_probe.sym} 630 -430 0 0 {name=p6 analysis=tran}
C {opin.sym} 110 -560 0 0 { name=p1 lab=LDCP_B }
C {ipin.sym} 70 -560 0 0 { name=p2 lab=LDCP }
C {lab_pin.sym} 640 -320 0 1 {name=p3 lab=LDCP2_B}
C {lab_pin.sym} 540 -320 0 0 {name=p4 lab=LDCP}
C {test_parametric_ports/inv3.sym} 580 -320 0 0 {name=x3 m=1 
+ wn=8.4u lln=2.4u wp=20u lp=2.4u
+ VCCPIN=vccpin VSSPIN=vsspin
schematic=@symname\\_2.sch
modn=zzn modp=zzp}
C {spice_probe.sym} 630 -320 0 0 {name=p5 analysis=tran}
C {test_parametric_ports/inv3.sym} 580 -530 0 0 {name=x1 m=1 
+ wn=8.4u lln=2.4u wp=20u lp=2.4u
+ VCCPIN=vccpin VSSPIN=vsspin

}
C {lab_pin.sym} 640 -530 0 1 {name=p7 lab=LDCP3_B}
C {lab_pin.sym} 540 -530 0 0 {name=p8 lab=LDCP}
C {spice_probe.sym} 630 -530 0 0 {name=p9 analysis=tran}
C {lab_pin.sym} 220 -340 0 1 {name=p10 lab=LDCP4_B}
C {lab_pin.sym} 120 -340 0 0 {name=p11 lab=LDCP}
C {test_parametric_ports/inv4.sym} 160 -340 0 0 {name=x4 m=1 
+ wn=8.4u lln=2.4u wp=20u lp=2.4u
+ VCCPIN=vccpin VSSPIN=vsspin
}
C {spice_probe.sym} 210 -340 0 0 {name=p12 analysis=tran}
C {test_parametric_ports/inv4.sym} 160 -100 0 0 {name=x5 m=1 
+ wn=8.4u lln=2.4u wp=20u lp=2.4u
+ VCCPIN=vccpin VSSPIN=vsspin
schematic=inv4_1.sch
modeltag=13}
C {lab_pin.sym} 120 -100 0 0 {name=p13 lab=LDCP}
C {lab_pin.sym} 220 -100 0 1 {name=p14 lab=LDCP5_B}
C {lab_pin.sym} 640 -210 0 1 {name=p15 lab=LDCP2_B}
C {lab_pin.sym} 540 -210 0 0 {name=p16 lab=LDCP}
C {test_parametric_ports/inv3.sym} 580 -210 0 0 {name=x6 m=1 
+ wn=7.4u lln=1.4u wp=10u lp=1.4u
+ VCCPIN=vccpin VSSPIN=vsspin
schematic=@symname\\_2.sch
modn=zzn modp=zzp}
C {spice_probe.sym} 630 -210 0 0 {name=p17 analysis=tran}
