v {xschem version=3.1.0 file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
L 2 125 -885 125 -655 {}
L 2 165 -885 165 -655 {}
L 2 205 -885 205 -655 {}
L 2 245 -885 245 -655 {}
L 2 285 -885 285 -655 {}
L 2 325 -885 325 -655 {}
L 2 365 -885 365 -655 {}
L 2 405 -885 405 -655 {}
L 2 85 -855 415 -855 {}
L 2 85 -815 415 -815 {}
L 2 85 -775 415 -775 {}
L 2 85 -735 415 -735 {}
L 2 85 -695 415 -695 {}
L 4 85 -895 85 -635 {}
L 4 75 -655 425 -655 {}
L 4 125 -655 125 -645 {}
L 4 165 -655 165 -645 {}
L 4 205 -655 205 -645 {}
L 4 245 -655 245 -645 {}
L 4 285 -655 285 -645 {}
L 4 325 -655 325 -645 {}
L 4 365 -655 365 -645 {}
L 4 405 -655 405 -645 {}
L 4 75 -695 85 -695 {}
L 4 75 -735 85 -735 {}
L 4 75 -775 85 -775 {}
L 4 75 -815 85 -815 {}
L 4 75 -855 85 -855 {}
L 4 610 -1100 630 -1140 {}
L 4 630 -1140 650 -1100 {}
L 4 650 -1100 670 -1140 {}
L 4 670 -1140 690 -1100 {}
L 4 690 -1100 710 -1140 {}
L 4 710 -1140 730 -1100 {}
L 4 1040 -1020 1050 -1020 {}
L 4 1050 -1060 1050 -1020 {}
L 4 1050 -1060 1070 -1060 {}
L 4 1070 -1060 1070 -1020 {}
L 4 1070 -1020 1090 -1020 {}
L 4 1090 -1060 1090 -1020 {}
L 4 1090 -1060 1110 -1060 {}
L 4 1110 -1060 1110 -1020 {}
L 4 1110 -1020 1130 -1020 {}
L 4 1130 -1060 1130 -1020 {}
L 4 1130 -1060 1150 -1060 {}
L 4 1150 -1060 1150 -1020 {}
L 4 1150 -1020 1160 -1020 {}
B 2 1260 -560 1680 -390 {flags=graph 
y1 = -0.42
y2 = 22
divy = 6
subdivy=1
x1=5e-10
x2=0.001
divx=8
node="panel
led" unitx=m
color="7 4"}
B 2 1260 -390 1680 -220 {flags=graph 
y1 = 0
y2 = 6.7
divy = 4
subdivy=1
x1=5e-10
x2=0.001
divx=8
  unitx=m
color="7 4 6"
node="i(vpanel)
i(vled)
\\"R.Avg. I(vpanel); i(vpanel) 20u ravg()\\""}
B 2 1260 -750 1680 -560 {flags=graph 
y1 = -8.9e-05
y2 = 100
divy = 5
subdivy=1
x1=5e-10
x2=0.001
divx=9

 unitx=m subdivx=4
color="7 4 6 10"
node="\\"Panel power; i(vpanel) v(panel) *\\"
\\"Led power; i(vled) v(led) *\\"
\\"Avg.Pan. Pwr; i(vpanel) v(panel) * 20u ravg()\\"
\\"SUN %; sun 100 *\\""
hilight_wave=-1}
B 2 1260 -940 1680 -750 {flags=graph 
y1 = 0
y2 = 1
divy = 5
subdivy=1
x1=5e-10
x2=0.001
divx=9

 unitx=u subdivx=4
color="14 7"
node="triang
level"
hilight_wave=-1
digital=0
ypos1=0.00261891
ypos2=0.51596}
B 2 1260 -1140 1680 -950 {flags=graph 
y1 = 0
y2 = 1
divy = 5
subdivy=1
x1=5e-10
x2=0.001
divx=9

 unitx=u subdivx=4


hilight_wave=-1
digital=0
ypos1=0.00261891
ypos2=0.51596
color=8
node="\\"CTRL1 Duty cycle; ctrl1 20u ravg()\\""}
B 18 85 -840 340 -655 {}
A 5 340 -840 5.590169943749475 243.434948822922 360 {fill=true}
P 7 6 415 -655 360 -811.25 355 -825 342.5 -840 330 -845 85 -855 {}
T {2x10 1W white LED} 1240 -200 0 0 0.4 0.4 {layer=8}
T {2xseries 1W white LEDs} 1220 -140 0 0 0.4 0.4 {}
T {2.5} 115 -635 0 0 0.2 0.2 {}
T {5.0} 155 -635 0 0 0.2 0.2 {}
T {7.5} 195 -635 0 0 0.2 0.2 {}
T {10.0} 235 -635 0 0 0.2 0.2 {}
T {12.5} 275 -635 0 0 0.2 0.2 {}
T {15.0} 315 -635 0 0 0.2 0.2 {}
T {17.5} 355 -635 0 0 0.2 0.2 {}
T {20.0} 395 -635 0 0 0.2 0.2 {}
T {2.5} 55 -860 0 0 0.2 0.2 {}
T {2.0} 55 -820 0 0 0.2 0.2 {}
T {1.5} 55 -775 0 0 0.2 0.2 {}
T {1.0} 55 -740 0 0 0.2 0.2 {}
T {0.5} 55 -700 0 0 0.2 0.2 {}
T {25C, 1000W/m2} 210 -910 0 0 0.2 0.2 {}
T {V} 430 -645 0 0 0.4 0.4 {}
T {I} 60 -900 0 0 0.4 0.4 {}
T {SOLAR PANEL} 165 -935 0 0 0.4 0.4 {}
T {Maximum Power} 327.5 -860 0 0 0.2 0.2 {layer=8}
T {set between 0 and 1
to simulate
sun radiation
level} 20 -350 0 0 0.2 0.2 {}
N 1010 -160 1100 -160 {lab=0}
N 1100 -250 1100 -160 {lab=0}
N 640 -560 730 -560 {lab=#net1}
N 1010 -390 1040 -390 {lab=VO}
N 1010 -390 1010 -260 {lab=VO}
N 1010 -200 1010 -160 {lab=0}
N 530 -560 580 -560 {lab=PANEL}
N 1010 -560 1010 -390 {lab=VO}
N 820 -560 860 -560 {lab=SW}
N 820 -560 820 -440 {lab=SW}
N 790 -560 820 -560 {lab=SW}
N 820 -160 1010 -160 {lab=0}
N 630 -160 820 -160 {lab=0}
N 1000 -560 1010 -560 {lab=VO}
N 1100 -390 1140 -390 {lab=LED}
N 1100 -390 1100 -310 {lab=LED}
N 820 -380 820 -340 { lab=#net2}
N 920 -560 940 -560 { lab=#net3}
N 370 -440 370 -390 {
lab=#net4}
N 370 -330 370 -280 {lab=0}
N 370 -440 410 -440 {
lab=#net4}
N 760 -670 760 -600 {
lab=CTRL1}
N 470 -440 540 -440 {
lab=#net5}
N 600 -440 660 -440 {
lab=PANEL}
N 510 -300 510 -280 {
lab=0}
N 510 -380 510 -360 {
lab=#net6}
N 170 -360 330 -360 {
lab=SUN}
N 170 -270 170 -250 {
lab=0}
N 610 -1010 610 -990 {
lab=0}
N 610 -1080 610 -1070 {
lab=TRIANG}
N 610 -1080 770 -1080 {
lab=TRIANG}
N 720 -870 830 -870 {
lab=LEVEL}
N 550 -840 550 -800 {
lab=REF}
N 550 -840 600 -840 {
lab=REF}
N 550 -900 600 -900 {
lab=LED}
N 1010 -1000 1060 -1000 {
lab=CTRL1}
N 170 -360 170 -330 {
lab=SUN}
N 830 -970 830 -870 {
lab=LEVEL}
N 830 -970 890 -970 {
lab=LEVEL}
N 770 -1080 770 -1030 {
lab=TRIANG}
N 770 -1030 890 -1030 {
lab=TRIANG}
N 820 -280 820 -160 {
lab=0}
C {title.sym} 160 -40 0 0 {name=l1 author="Stefan Schippers" net_name=true}
C {code_shown.sym} 190 -200 0 0 {name=CONTROL value=".control
save all
tran .05u 1m uic
write solar_panel.raw
.endc

" net_name=true}
C {code.sym} 20 -190 0 0 {name=MODELS value=".MODEL DIODE D(IS=1.139e-08 RS=0.99 CJO=9.3e-12 VJ=1.6 M=0.411 BV=30 EG=0.7 ) 
.MODEL swmod SW(VT=0.5 VH=0.01 RON=0.01 ROFF=10000000)
" net_name=true}
C {lab_pin.sym} 660 -440 0 1 {name=l4  lab=PANEL }
C {lab_pin.sym} 630 -160 0 0 {name=l6  lab=0 }
C {ammeter.sym} 1070 -390 3 0 {name=Vled net_name=true}
C {ind.sym} 890 -560 3 1 {name=L2
m=1
value=40u
footprint=1206
device=inductor net_name=true}
C {lab_pin.sym} 1140 -390 0 1 {name=l7  lab=LED }
C {lab_pin.sym} 820 -500 0 1 {name=l9  lab=SW }
C {capa.sym} 1010 -230 0 0 {name=C1
m=1
value=500n
footprint=1206
device="ceramic capacitor" net_name=true}
C {lab_pin.sym} 1010 -350 0 1 {name=l10  lab=VO }
C {lab_pin.sym} 530 -560 0 0 {name=l3  lab=PANEL }
C {ammeter.sym} 970 -560 3 0 {name=Vind net_name=true}
C {isource_table.sym} 1100 -280 0 0 {name=G2[9..0] CTRL="V(LED)" TABLE="
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
C {ammeter.sym} 610 -560 3 0 {name=Vsw net_name=true}
C {ammeter.sym} 820 -310 2 0 {name=Vdiode net_name=true}
C {spice_probe.sym} 1120 -390 0 0 {name=p1 analysis=tran}
C {spice_probe.sym} 660 -440 0 0 {name=p2 analysis=tran}
C {spice_probe.sym} 820 -460 0 1 {name=p3 analysis=tran}
C {launcher.sym} 1080 -1140 0 0 {name=h3 
descr="Reload Waves" 
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw tran
"
}
C {pv_ngspice.sym} 370 -360 0 0 {name=X1  m=1}
C {lab_pin.sym} 370 -280 0 0 {name=l12  lab=0 }
C {capa.sym} 510 -410 0 0 {name=C11
m=1
value=10u
footprint=1206
device="ceramic capacitor" net_name=true}
C {lab_pin.sym} 510 -280 0 0 {name=l90  lab=0 }
C {ammeter.sym} 440 -440 3 1 {name=Vpanel net_name=true}
C {diode_ngspice.sym} 820 -410 2 0 {name=X2  m=1 Roff=1e9 Ron=0.1}
C {switch_ngspice.sym} 760 -560 1 0 {name=S1 model=swmod}
C {lab_pin.sym} 740 -600 0 0 {name=l5  lab=0 }
C {ammeter.sym} 570 -440 3 1 {name=Vpanel1 net_name=true}
C {ammeter.sym} 510 -330 0 0 {name=Vcap net_name=true}
C {vsource.sym} 170 -300 0 0 {name=Vfade value="pwl 0 1 1m 0"}
C {lab_pin.sym} 170 -250 0 0 {name=l8  lab=0 }
C {ngspice_get_expr.sym} 1120 -320 0 0 {name=r29 
node="[format %.4g [expr \{[ngspice::get_voltage led] * [ngspice::get_current vled]\}]] W"
descr = power
}
C {vsource.sym} 610 -1040 0 0 {name=Vtriang value="pulse 0 1 0 2u 2u 1f 4u"}
C {lab_pin.sym} 610 -990 0 0 {name=l11  lab=0 }
C {lab_pin.sym} 770 -1080 0 1 {name=l14
lab=TRIANG }
C {vsource.sym} 550 -770 0 0 {name=Vref value=7}
C {lab_pin.sym} 550 -740 0 0 {name=l17  lab=0 }
C {lab_pin.sym} 760 -670 0 0 {name=l13  lab=CTRL1 }
C {comp_ngspice.sym} 660 -870 0 0 {name=x3 OFFSET=0.5 AMPLITUDE=1 ROUT=7k COUT=1n}
C {lab_pin.sym} 550 -840 0 0 {name=l16
lab=REF}
C {lab_pin.sym} 550 -900 0 0 {name=l15  lab=LED }
C {lab_pin.sym} 830 -870 0 1 {name=l18  lab=LEVEL}
C {comp_ngspice.sym} 950 -1000 0 0 {name=x4 OFFSET=0.5 AMPLITUDE=1 ROUT=1 COUT=1p}
C {lab_pin.sym} 1060 -1000 0 1 {name=l19  lab=CTRL1 }
C {lab_pin.sym} 170 -360 0 0 {name=l20  lab=SUN }
