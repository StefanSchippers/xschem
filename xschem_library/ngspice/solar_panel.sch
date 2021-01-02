v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
L 2 85 -895 85 -665 {}
L 2 125 -895 125 -665 {}
L 2 165 -895 165 -665 {}
L 2 205 -895 205 -665 {}
L 2 245 -895 245 -665 {}
L 2 285 -895 285 -665 {}
L 2 325 -895 325 -665 {}
L 2 365 -895 365 -665 {}
L 2 45 -865 375 -865 {}
L 2 45 -825 375 -825 {}
L 2 45 -785 375 -785 {}
L 2 45 -745 375 -745 {}
L 2 45 -705 375 -705 {}
L 4 45 -905 45 -645 {}
L 4 35 -665 385 -665 {}
L 4 85 -665 85 -655 {}
L 4 125 -665 125 -655 {}
L 4 165 -665 165 -655 {}
L 4 205 -665 205 -655 {}
L 4 245 -665 245 -655 {}
L 4 285 -665 285 -655 {}
L 4 325 -665 325 -655 {}
L 4 365 -665 365 -655 {}
L 4 35 -705 45 -705 {}
L 4 35 -745 45 -745 {}
L 4 35 -785 45 -785 {}
L 4 35 -825 45 -825 {}
L 4 35 -865 45 -865 {}
L 8 790 -500 790 -480 {}
L 8 770 -510 810 -510 {}
L 8 790 -530 790 -510 {}
L 8 770 -610 790 -610 {}
L 8 790 -610 807.5 -620 {}
L 8 810 -610 830 -610 {}
B 18 45 -850 300 -665 {}
A 5 300 -850 5.590169943749475 243.434948822922 360 {fill=true}
P 7 6 375 -665 320 -821.25 315 -835 302.5 -850 290 -855 45 -865 {}
P 8 5 790 -500 800 -500 790 -510 780 -500 790 -500 {fill=true}
T {2.5} 75 -645 0 0 0.2 0.2 {}
T {5.0} 115 -645 0 0 0.2 0.2 {}
T {7.5} 155 -645 0 0 0.2 0.2 {}
T {10.0} 195 -645 0 0 0.2 0.2 {}
T {12.5} 235 -645 0 0 0.2 0.2 {}
T {15.0} 275 -645 0 0 0.2 0.2 {}
T {17.5} 315 -645 0 0 0.2 0.2 {}
T {20.0} 355 -645 0 0 0.2 0.2 {}
T {2.5} 15 -870 0 0 0.2 0.2 {}
T {2.0} 15 -830 0 0 0.2 0.2 {}
T {1.5} 15 -785 0 0 0.2 0.2 {}
T {1.0} 15 -750 0 0 0.2 0.2 {}
T {0.5} 15 -710 0 0 0.2 0.2 {}
T {25C, 1000W/m2} 170 -920 0 0 0.2 0.2 {}
T {V} 390 -655 0 0 0.4 0.4 {}
T {I} 20 -910 0 0 0.4 0.4 {}
T {SOLAR PANEL} 125 -945 0 0 0.4 0.4 {}
T {Maximum Power} 287.5 -870 0 0 0.2 0.2 {layer=8}
T {2x10 1W white LED} 1230 -340 0 0 0.4 0.4 {layer=8}
T {IDEAL Diode} 690 -470 0 0 0.4 0.4 {layer=8}
T {2xseries 1W white LEDs} 1250 -230 0 0 0.4 0.4 {}
N 80 -450 80 -430 {lab=SRC}
N 1050 -250 1140 -250 {lab=0}
N 1140 -290 1140 -250 {lab=0}
N 680 -650 770 -650 {lab=#net1}
N 1050 -480 1080 -480 {lab=VO}
N 1050 -480 1050 -350 {lab=VO}
N 1050 -290 1050 -250 {lab=0}
N 570 -650 620 -650 {lab=PANEL}
N 1050 -650 1050 -480 {lab=VO}
N 610 -870 610 -810 {lab=CTRL1}
N 860 -650 900 -650 {lab=SW}
N 860 -650 860 -530 {lab=SW}
N 830 -650 860 -650 {lab=SW}
N 80 -450 170 -450 {lab=SRC}
N 860 -250 1050 -250 {lab=0}
N 570 -250 860 -250 {lab=0}
N 1040 -650 1050 -650 {lab=VO}
N 345 -450 345 -420 {lab=PANEL}
N 345 -360 345 -310 {lab=0}
N 1140 -480 1180 -480 {lab=LED}
N 1140 -480 1140 -350 {lab=LED}
N 230 -450 250 -450 {lab=#net2}
N 345 -450 400 -450 {lab=PANEL}
N 860 -370 860 -250 {lab=0}
N 860 -470 860 -430 { lab=#net3}
N 960 -650 980 -650 { lab=#net4}
N 310 -450 345 -450 {lab=PANEL}
C {title.sym} 160 -40 0 0 {name=l1 author="Stefan Schippers" net_name=true}
C {code_shown.sym} 245 -245 0 0 {name=CONTROL value="* .control
* save all
* tran 5n 500u uic
* write led_driver.raw
* .endc
.option savecurrents
*.save all
.tran 5n 200u uic
* .dc VP 0 21 0.01
" net_name=true}
C {code.sym} 15 -225 0 0 {name=MODELS value=".MODEL DIODE D(IS=1.139e-08 RS=0.99 CJO=9.3e-12 VJ=1.6 M=0.411 BV=30 EG=0.7 ) 
.MODEL swmod SW(VT=0.1 VH=0.01 RON=0.01 ROFF=10000000)
" net_name=true}
C {vsource.sym} 80 -340 0 0 {name=V1 value=21 net_name=true}
C {lab_pin.sym} 80 -310 0 0 {name=l3 sig_type=std_logic lab=0 net_name=true}
C {res.sym} 80 -400 0 0 {name=R1
value=1.4
footprint=1206
device=resistor
m=1 net_name=true}
C {lab_pin.sym} 400 -450 0 1 {name=l4 sig_type=std_logic lab=PANEL net_name=true}
C {lab_pin.sym} 80 -450 0 0 {name=l5 sig_type=std_logic lab=SRC net_name=true}
C {lab_pin.sym} 570 -250 0 0 {name=l6 sig_type=std_logic lab=0 net_name=true}
C {ammeter.sym} 1110 -480 3 0 {name=Vled net_name=true}
C {ind.sym} 930 -650 3 1 {name=L1
m=1
value=40u
footprint=1206
device=inductor net_name=true}
C {lab_pin.sym} 1180 -480 0 1 {name=l7 sig_type=std_logic lab=LED net_name=true}
C {lab_pin.sym} 610 -870 0 0 {name=l8 sig_type=std_logic lab=CTRL1 net_name=true}
C {lab_pin.sym} 860 -590 0 1 {name=l9 sig_type=std_logic lab=SW net_name=true}
C {capa.sym} 1050 -320 0 0 {name=C1
m=1
value=10u
footprint=1206
device="ceramic capacitor" net_name=true}
C {ammeter.sym} 280 -450 3 1 {name=Vpanel net_name=true}
C {lab_pin.sym} 1050 -440 0 1 {name=l10 sig_type=std_logic lab=VO net_name=true}
C {res.sym} 860 -500 0 0 {name=R2
value="r='V(SW) < 0 ? 0.01 : 1e9'"
footprint=1206
device=resistor
m=1
 net_name=true}
C {vsource.sym} 610 -780 0 0 {name=Vset1 value="pulse 0 1 0 1n 1n 1.9u 5u" net_name=true}
C {lab_pin.sym} 610 -750 0 0 {name=l13 sig_type=std_logic lab=0 net_name=true}
C {res.sym} 800 -650 3 0 {name=R3
value="r='V(CTRL1) > 0.5 ? 0.01 : 1e7'"
footprint=1206
device=resistor
m=1
 net_name=true}
C {lab_pin.sym} 570 -650 0 0 {name=l2 sig_type=std_logic lab=PANEL net_name=true}
C {res.sym} 200 -450 1 1 {name=R4
value="r='(15-V(PANEL) > 0) ?
+ (15-V(PANEL))/2.5 + 1.12 : 
+ (18-V(PANEL)> 0)? 
+ (18-V(PANEL))**1.1/3 : 
+ 0.001'"
footprint=1206
device=resistor
m=1
 net_name=true}
C {ammeter.sym} 1010 -650 3 0 {name=Vind net_name=true}
C {capa.sym} 345 -390 0 0 {name=C2
m=1
value=10u
footprint=1206
device="ceramic capacitor" net_name=true}
C {lab_pin.sym} 345 -310 0 0 {name=l11 sig_type=std_logic lab=0 net_name=true}
C {launcher.sym} 655 -105 0 0 {name=h2 
descr="Simulate" 
tclcommand="xschem netlist; xschem simulate" net_name=true}
C {isource_table.sym} 1140 -320 0 0 {name=G2[9..0] CTRL="V(LED)" TABLE="
+ (0, 0)
+ (4.8, 5m)
+ (5.2, 15m)
+ (5.6, 46m)
+ (5.8, 80m)
+ (6.0, 115m)
+ (6.2, 157m)
+ (6.4, 202m)
+ (6.6, 245m)
+ (6.8, 290m)
+ (7.0, 337m)
+ (7.2, 395m)
+ (7.4, 470m)
+ (8.0, 750m)"
 net_name=true}
C {ammeter.sym} 650 -650 3 0 {name=Vsw net_name=true}
C {ammeter.sym} 860 -400 2 0 {name=Vdiode net_name=true}
C {launcher.sym} 655 -165 0 0 {name=h1 
descr="Simulate + gaw reload" 
tclcommand="set sim(spice,default) 1; set sim(spice,1,fg) 1; set sim(spice,1,st) 0;xschem netlist; xschem simulate; gaw_cmd reload_all" net_name=true}
C {spice_probe.sym} 1160 -480 0 0 {name=p1 analysis=tran}
C {spice_probe.sym} 360 -450 0 0 {name=p2 analysis=tran}
C {spice_probe.sym} 860 -550 0 1 {name=p3 analysis=tran}
C {spice_probe.sym} 100 -450 0 1 {name=p4 analysis=tran}
