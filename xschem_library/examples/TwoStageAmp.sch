v {xschem version=2.9.8 file_version=1.2}
G {}
K {}
V {}
S {}
E {}
N 430 -330 430 -320 {lab=#net1}
N 430 -330 510 -330 {lab=#net1}
N 510 -330 510 -320 {lab=#net1}
N 430 -580 430 -490 {lab=Vcoll1}
N 330 -460 390 -460 {lab=Vbase1}
N 180 -460 220 -460 {lab=#net2}
N 90 -460 120 -460 {lab=Vin}
N 90 -460 90 -320 {lab=Vin}
N 310 -520 310 -460 {lab=Vbase1}
N 330 -460 330 -320 {lab=Vbase1}
N 70 -460 90 -460 {lab=Vin}
N 530 -580 570 -580 {lab=#net3}
N 430 -580 470 -580 {lab=Vcoll1}
N 790 -580 800 -580 {lab=#net4}
N 690 -670 690 -580 {lab=Vbase2}
N 710 -580 710 -520 {lab=Vbase2}
N 430 -340 430 -330 {lab=#net1}
N 430 -670 430 -580 {lab=Vcoll1}
N 280 -460 310 -460 {lab=Vbase1}
N 310 -460 330 -460 {lab=Vbase1}
N 630 -580 690 -580 {lab=Vbase2}
N 690 -580 710 -580 {lab=Vbase2}
N 840 -630 840 -610 {lab=Vcoll2}
N 840 -330 840 -320 {lab=Vem2}
N 840 -330 920 -330 {lab=Vem2}
N 920 -330 920 -320 {lab=Vem2}
N 840 -420 840 -330 {lab=Vem2}
N 840 -630 920 -630 {lab=Vcoll2}
N 1000 -630 1000 -520 {lab=Vout}
N 980 -630 1000 -630 {lab=Vout}
N 840 -670 840 -630 {lab=Vcoll2}
N 1000 -630 1050 -630 {lab=Vout}
N 840 -550 840 -480 {lab=#net5}
N 430 -430 430 -400 {lab=Vem1}
N 760 -190 760 -170 {lab=VCC}
N 710 -580 730 -580 {lab=Vbase2}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {vsource.sym} 90 -290 0 0 {name=Vinput 
value="DC 1.6V AC 1 0 SIN(0 1MV 1KHZ)" net_name=true}
C {gnd.sym} 90 -260 0 0 {name=l2 lab=0 net_name=true}
C {res.sym} 150 -460 3 1 {name=R5
value=10
footprint=1206 
device=resistor 
m=1 net_name=true}
C {capa.sym} 250 -460 3 1 {name=C1 
m=1 
value=2.2u
footprint=1206 
device="ceramic capacitor" net_name=true}
C {res.sym} 310 -550 0 0 {name=R1
value=28K
footprint=1206 
device=resistor 
m=1 net_name=true}
C {res.sym} 330 -290 0 0 {name=R2
value=2K
footprint=1206 
device=resistor 
m=1 net_name=true}
C {gnd.sym} 330 -260 0 0 {name=l3 lab=0 net_name=true}
C {npn.sym} 410 -460 0 0 {name=Q1 
model=Q2N3904 
device=2N3904 
footprint=TO92 
area=1
pinnumber(1) = 2 ; "alternatively use pinnumber(B)"
pinnumber(2) = 1 ; "alternatively use pinnumber(E)" net_name=true}
C {res.sym} 430 -290 0 0 {name=RE1
value=100
footprint=1206 
device=resistor 
m=1 net_name=true}
C {gnd.sym} 430 -260 0 0 {name=l4 lab=0 net_name=true}
C {capa.sym} 510 -290 0 0 {name=CE1 
m=1 
value=1p 
footprint=1206 
device="ceramic capacitor" net_name=true}
C {gnd.sym} 510 -260 0 0 {name=l5 lab=0 net_name=true}
C {res.sym} 430 -700 0 0 {name=RC1
value=3.3K
footprint=1206 
device=resistor 
m=1 net_name=true}
C {vdd.sym} 430 -730 0 0 {name=l6 lab=VCC net_name=true}
C {vdd.sym} 310 -580 0 0 {name=l7 lab=VCC net_name=true}
C {ipin.sym} 70 -460 0 0 {name=p1 lab=Vin net_name=true}
C {res.sym} 500 -580 3 1 {name=R8
value=1
footprint=1206 
device=resistor 
m=1 net_name=true}
C {capa.sym} 600 -580 3 1 {name=C2
m=1 
value=2.2u 
footprint=1206 
device="ceramic capacitor" net_name=true}
C {res.sym} 690 -700 0 0 {name=R3
value=28K
footprint=1206 
device=resistor 
m=1 net_name=true}
C {res.sym} 710 -490 0 0 {name=R4
value=2.8K
footprint=1206 
device=resistor 
m=1 net_name=true}
C {gnd.sym} 710 -460 0 0 {name=l8 lab=0 net_name=true}
C {vdd.sym} 690 -730 0 0 {name=l9 lab=VCC net_name=true}
C {npn.sym} 820 -580 0 0 {name=Q2 
model=Q2N3904 
device=2N3904 
footprint=TO92 
area=1
pinnumber(1) = 2
pinnumber(2) = 1 net_name=true}
C {res.sym} 840 -700 0 0 {name=RC2
value=1K
footprint=1206 
device=resistor 
m=1 net_name=true}
C {vdd.sym} 840 -730 0 0 {name=l10 lab=VCC net_name=true}
C {res.sym} 840 -290 0 0 {name=RE2
value=100
footprint=1206 
device=resistor 
m=1 net_name=true}
C {gnd.sym} 840 -260 0 0 {name=l11 lab=0 net_name=true}
C {capa.sym} 920 -290 0 0 {name=CE2
m=1 
value=1p 
footprint=1206 
device="ceramic capacitor" net_name=true}
C {gnd.sym} 920 -260 0 0 {name=l12 lab=0 net_name=true}
C {capa.sym} 950 -630 3 1 {name=Cout
m=1 
value=2.2u 
footprint=1206 
device="ceramic capacitor" net_name=true}
C {res.sym} 1000 -490 0 0 {name=RL
value=100K
footprint=1206 
device=resistor 
m=1 net_name=true}
C {gnd.sym} 1000 -460 0 0 {name=l13 lab=0 net_name=true}
C {opin.sym} 1050 -630 0 0 {name=p2 lab=Vout net_name=true}
C {vdd.sym} 760 -190 0 0 {name=l14 lab=VCC net_name=true}
C {vsource.sym} 760 -140 0 0 {name=VCC value=15 net_name=true}
C {gnd.sym} 760 -110 0 0 {name=l15 lab=0 net_name=true}
C {code.sym} 160 -190 0 0 {name=MODELS value=".model Q2N3904   NPN(Is=6.734f Xti=3 Eg=1.11 Vaf=74.03 Bf=416.4 Ne=1.259
+               Ise=6.734f Ikf=66.78m Xtb=1.5 Br=.7371 Nc=2 Isc=0 Ikr=0 Rc=1
+               Cjc=3.638p Mjc=.3085 Vjc=.75 Fc=.5 Cje=4.493p Mje=.2593 Vje=.75
+               Tr=239.5n Tf=301.2p Itf=.4 Vtf=4 Xtf=2 Rb=10)
*               Fairchild        pid=23          case=TO92
*               88-09-08 bam    creation

"}
C {code_shown.sym} 320 -180 0 0 {name=COMMANDS
value=".SAVE ALL
* .OP
.AC DEC 20 1Hz 100MegHz
* .DC Vinput 0 5 .01
* .DC Vinput 1 2 .0
"}
C {spice_probe.sym} 1020 -630 0 0 {name=p3 analysis=tran voltage=0.0000e+00 net_name=true}
C {spice_probe.sym} 650 -580 0 0 {name=p4 analysis=tran voltage=1.28 net_name=true}
C {spice_probe.sym} 840 -500 0 0 {name=p5 analysis=tran voltage=0.5705 net_name=true}
C {spice_probe.sym} 430 -540 0 1 {name=p6 analysis=tran voltage=5.932 net_name=true}
C {spice_probe.sym} 360 -460 0 1 {name=p7 analysis=tran voltage=0.9675 net_name=true}
C {spice_probe.sym} 110 -460 0 1 {name=p8 analysis=tran voltage=1.6 net_name=true}
C {spice_probe.sym} 880 -630 0 0 {name=p9 analysis=tran voltage=9.328 net_name=true}
C {lab_pin.sym} 430 -650 0 0 {name=l16 sig_type=std_logic lab=Vcoll1 net_name=true}
C {lab_pin.sym} 840 -650 0 0 {name=l17 sig_type=std_logic lab=Vcoll2 net_name=true}
C {lab_pin.sym} 710 -550 0 0 {name=l18 sig_type=std_logic lab=Vbase2 net_name=true}
C {lab_pin.sym} 310 -510 0 0 {name=l19 sig_type=std_logic lab=Vbase1 net_name=true}
C {lab_pin.sym} 840 -420 0 0 {name=l20 sig_type=std_logic lab=Vem2 net_name=true}
C {lab_pin.sym} 430 -420 0 0 {name=l21 sig_type=std_logic lab=Vem1 net_name=true}
C {ammeter.sym} 840 -450 0 0 {name=vm2 current=0.005705 net_name=true}
C {ammeter.sym} 430 -370 0 0 {name=vm1 current=0.002765 net_name=true}
C {spice_probe.sym} 510 -330 0 0 {name=p10 analysis=tran voltage=0.2765 net_name=true}
C {spice_probe.sym} 760 -170 0 1 {name=p12 analysis=tran voltage=15 net_name=true}
C {ammeter.sym} 760 -580 3 0 {name=v1 current=3.2742e-05 net_name=true}
