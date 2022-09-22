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
B 2 1270 -740 1690 -570 {flags=graph 
y1 = -0.00013
y2 = 21
divy = 6
subdivy=1
x1=1e-11
x2=0.0006
divx=8
node="panel
led" unitx=m
color="7 4"}
B 2 1270 -570 1690 -400 {flags=graph 
y1 = 0.0012
y2 = 6.3
divy = 4
subdivy=1
x1=1e-11
x2=0.0006
divx=8
  unitx=m
color="7 4"
node="i(vpanel)
i(vled)"}
B 2 1270 -930 1690 -740 {flags=graph 
y1 = -4.1e-08
y2 = 100
divy = 5
subdivy=1
x1=1e-11
x2=0.0006
divx=9

 unitx=m subdivx=4
color="7 4 6"
node="\\"Panel power; i(vpanel) v(panel) *\\"
\\"Led power; i(vled) v(led) *\\"
\\"R.Avg.Pan. Pwr; i(vpanel) v(panel) * 20u ravg()\\""
hilight_wave=-1}
B 18 85 -840 340 -655 {}
A 5 340 -840 5.590169943749475 243.434948822922 360 {fill=true}
P 7 6 415 -655 360 -811.25 355 -825 342.5 -840 330 -845 85 -855 {}
T {2x10 1W white LED} 1230 -340 0 0 0.4 0.4 {layer=8}
T {2xseries 1W white LEDs} 1250 -230 0 0 0.4 0.4 {}
T {Select one or more graphs (and no other objects)
and use arrow keys to zoom / pan waveforms} 1190 -1010 0 0 0.3 0.3 {}
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
N 860 -250 1050 -250 {lab=0}
N 670 -250 860 -250 {lab=0}
N 1040 -650 1050 -650 {lab=VO}
N 1140 -480 1180 -480 {lab=LED}
N 1140 -480 1140 -350 {lab=LED}
N 860 -370 860 -250 {lab=0}
N 860 -470 860 -430 { lab=#net2}
N 960 -650 980 -650 { lab=#net3}
N 280 -530 280 -480 {
lab=#net4}
N 280 -420 280 -370 {lab=0}
N 280 -530 320 -530 {
lab=#net4}
N 610 -870 800 -870 {
lab=CTRL1}
N 800 -870 800 -690 {
lab=CTRL1}
N 380 -530 450 -530 {
lab=#net5}
N 510 -530 570 -530 {
lab=PANEL}
N 420 -390 420 -370 {
lab=0}
N 420 -470 420 -450 {
lab=#net6}
N 170 -450 230 -450 {
lab=#net7}
N 170 -390 170 -370 {
lab=0}
C {title.sym} 160 -40 0 0 {name=l1 author="Stefan Schippers" net_name=true}
C {code_shown.sym} 190 -240 0 0 {name=CONTROL value=".control
save all
tran 1u 600u uic
write solar_panel.raw
.endc

" net_name=true}
C {code.sym} 20 -230 0 0 {name=MODELS value=".MODEL DIODE D(IS=1.139e-08 RS=0.99 CJO=9.3e-12 VJ=1.6 M=0.411 BV=30 EG=0.7 ) 
.MODEL swmod SW(VT=0.5 VH=0.01 RON=0.01 ROFF=10000000)
" net_name=true}
C {lab_pin.sym} 570 -530 0 1 {name=l4 sig_type=std_logic lab=PANEL net_name=true}
C {lab_pin.sym} 670 -250 0 0 {name=l6 sig_type=std_logic lab=0 net_name=true}
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
C {lab_pin.sym} 1050 -440 0 1 {name=l10 sig_type=std_logic lab=VO net_name=true}
C {vsource.sym} 610 -780 0 0 {name=Vset1 
value="dc 0 pulse 0 1 0 1n 1n 1.7u 5u"
net_name=true}
C {lab_pin.sym} 610 -750 0 0 {name=l13 sig_type=std_logic lab=0 net_name=true}
C {lab_pin.sym} 570 -650 0 0 {name=l2 sig_type=std_logic lab=PANEL net_name=true}
C {ammeter.sym} 1010 -650 3 0 {name=Vind net_name=true}
C {launcher.sym} 660 -110 0 0 {name=h2 
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
C {launcher.sym} 660 -170 0 0 {name=h1 
descr="Simulate + gaw reload" 
tclcommand="set sim(spice,default) 1; set sim(spice,1,fg) 1; set sim(spice,1,st) 0;xschem netlist; xschem simulate; gaw_cmd reload_all" net_name=true}
C {spice_probe.sym} 1160 -480 0 0 {name=p1 analysis=tran}
C {spice_probe.sym} 570 -530 0 0 {name=p2 analysis=tran}
C {spice_probe.sym} 860 -550 0 1 {name=p3 analysis=tran}
C {launcher.sym} 1200 -1050 0 0 {name=h3 
descr="Select arrow and 
Ctrl-Left-Click to load/unload waveforms" 
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw tran
"
}
C {pv_ngspice.sym} 280 -450 0 0 {name=X1  m=1 roff=1e9}
C {lab_pin.sym} 280 -370 0 0 {name=l12 sig_type=std_logic lab=0 net_name=true}
C {capa.sym} 420 -500 0 0 {name=C11
m=1
value=10u
footprint=1206
device="ceramic capacitor" net_name=true}
C {lab_pin.sym} 420 -370 0 0 {name=l90 sig_type=std_logic lab=0 net_name=true}
C {ammeter.sym} 350 -530 3 1 {name=Vpanel net_name=true}
C {diode_ngspice.sym} 860 -500 2 0 {name=X2  m=1 Roff=1e9 Ron=0.1}
C {switch_ngspice.sym} 800 -650 1 0 {name=S1 model=swmod}
C {lab_pin.sym} 780 -690 0 0 {name=l3 sig_type=std_logic lab=0 net_name=true}
C {ammeter.sym} 480 -530 3 1 {name=Vpanel1 net_name=true}
C {ammeter.sym} 420 -420 0 0 {name=Vcap net_name=true}
C {vsource.sym} 170 -420 0 0 {name=Vfade value=1}
C {lab_pin.sym} 170 -370 0 0 {name=l12 sig_type=std_logic lab=0 net_name=true}
