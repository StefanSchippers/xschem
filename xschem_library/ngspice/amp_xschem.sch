v {xschem version=2.9.5_RC8 file_version=1.1}
G {}
V {}
S {}
E {}
N 600 -450 650 -450 {lab=B}
N 690 -690 690 -670 {lab=D}
N 510 -690 690 -690 {lab=D}
N 190 -690 310 -690 {lab=VCC}
N 190 -690 190 -670 {lab=VCC}
N 690 -500 690 -480 {lab=C}
N 690 -500 750 -500 {lab=C}
N 810 -500 840 -500 {lab=OUT}
N 840 -500 840 -440 {lab=OUT}
N 690 -410 690 -400 {lab=E}
N 780 -410 780 -400 {lab=E}
N 690 -410 780 -410 {lab=E}
N 510 -450 510 -400 {lab=#net1}
N 510 -570 510 -530 {lab=#net2}
N 420 -450 510 -450 {lab=#net1}
N 370 -690 510 -690 {lab=D}
N 690 -420 690 -410 {lab=E}
N 510 -470 510 -450 {lab=#net1}
N 190 -450 360 -450 {lab=IN}
N 190 -450 190 -420 {lab=IN}
N 840 -500 930 -500 {lab=OUT}
N 510 -450 540 -450 {lab=#net1}
N 510 -690 510 -630 {lab=D}
N 690 -550 690 -500 {lab=C}
C {title.sym} 160 -30 0 0 {name=l7 author="Stefan Schippers"}
C {code_shown.sym} 30 -310 0 0 {name=CONTROL place=end value=".ac oct 1000 100 10G
.control
   listing e
   run
   write amp_xschem.raw
   let response = db(v(out)/v(in))
   settype decibel response
   gnuplot amp_xschem response xlog
   save all
   op
   write amp_xschem.raw
.endc

"}
C {code.sym} 840 -250 0 0 {name=MODELS value=".model Q2N2219A  NPN(Is=14.34f Xti=3 Eg=1.11 Vaf=74.03 Bf=255.9 Ne=1.307 Ise=14.34f Ikf=.2847 Xtb=1.5 Br=6.092 Nc=2 Isc=0 Ikr=0 Rc=1 Cjc=7.306p
+ Mjc=.3416 Vjc=.75 Fc=.5 Cje=22.01p Mje=.377 Vje=.75 Tr=46.91n Tf=411.1p Itf=.6 Vtf=1.7 Xtf=3 Rb=10 Vceo=40 Icrating=800m mfg=Philips)
"}
C {ind.sym} 340 -690 1 0 {name=l1 value=10uH}
C {vsource.sym} 190 -640 0 0 {name=v1 value=12}
C {gnd.sym} 190 -610 0 0 {name=l11 lab=0}
C {capa.sym} 430 -660 0 1 {name=C4 m=1 value=100nF footprint=1206 device="ceramic capacitor"}
C {res.sym} 510 -500 0 0 {name=R1 value=6.8k footprint=1206 device=resistor m=1}
C {npn.sym} 670 -450 0 0 {name=Q2 model=Q2N2219A device=Q2N5179 footprint=SOT23 area=1}
C {res.sym} 510 -370 0 0 {name=R2 value=1.8k footprint=1206 device=resistor m=1}
C {capa.sym} 780 -500 1 1 {name=C5 m=1 value=1nF footprint=1206 device="ceramic capacitor"}
C {res.sym} 840 -410 0 0 {name=R3 value=120k footprint=1206 device=resistor m=1}
C {gnd.sym} 840 -380 0 0 {name=l12 lab=0}
C {res.sym} 690 -370 0 0 {name=R4 value=100 footprint=1206 device=resistor m=1}
C {gnd.sym} 690 -340 0 0 {name=l13 lab=0}
C {capa.sym} 390 -450 1 1 {name=C6 m=1 value=1nF footprint=1206 device="ceramic capacitor"}
C {vsource.sym} 190 -390 0 0 {name=v2 value="dc 0 ac 1"}
C {gnd.sym} 190 -360 0 0 {name=l14 lab=0}
C {res.sym} 690 -640 0 0 {name=R5 value=330 footprint=1206 device=resistor m=1}
C {capa.sym} 780 -370 0 1 {name=C7 m=1 value=100nF footprint=1206 device="ceramic capacitor"}
C {gnd.sym} 780 -340 0 0 {name=l15 lab=0}
C {gnd.sym} 510 -340 0 0 {name=l16 lab=0}
C {gnd.sym} 430 -630 0 0 {name=l17 lab=0}
C {lab_wire.sym} 270 -690 0 0 {name=l18 sig_type=std_logic lab=VCC}
C {lab_wire.sym} 590 -690 0 0 {name=l19 sig_type=std_logic lab=D}
C {lab_wire.sym} 630 -450 0 0 {name=l20 sig_type=std_logic lab=B}
C {lab_wire.sym} 730 -410 0 0 {name=l21 sig_type=std_logic lab=E}
C {opin.sym} 930 -500 0 0 {name=p2 lab=OUT}
C {lab_wire.sym} 690 -510 0 0 {name=l22 sig_type=std_logic lab=C}
C {lab_pin.sym} 190 -450 0 0 {name=l23 sig_type=std_logic lab=IN}
C {ammeter.sym} 570 -450 3 0 {name=v3 current=8.9002e-05}
C {ammeter.sym} 510 -600 0 0 {name=v4 current=0.001414}
C {ammeter.sym} 690 -580 0 0 {name=v5 current=0.01657}
C {spice_probe.sym} 860 -500 0 0 {name=p1 analysis=tran voltage=0.0000e+00}
C {spice_probe.sym} 710 -500 0 0 {name=p3 analysis=tran voltage=6.533}
C {spice_probe.sym} 190 -690 0 0 {name=p4 analysis=tran voltage=12}
C {spice_probe.sym} 210 -450 0 0 {name=p5 analysis=tran voltage=0.0000e+00}
C {spice_probe.sym} 630 -450 0 0 {name=p6 analysis=tran voltage=2.385}
C {spice_probe.sym} 750 -410 0 0 {name=p7 analysis=tran voltage=1.666}
C {spice_probe.sym} 470 -690 0 0 {name=p8 analysis=tran voltage=12}
