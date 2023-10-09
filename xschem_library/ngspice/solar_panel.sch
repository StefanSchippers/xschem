v {xschem version=3.4.4 file_version=1.2
*
* This file is part of XSCHEM,
* a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
* simulation.
* Copyright (C) 1998-2023 Stefan Frederik Schippers
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
Solar panel simulation. 
A Photovoltaic model has been created for this (pv_ngspice.sym)
\{Stefan\}.}
G {}
K {}
V {}
S {}
E {}
L 2 105 -1005 105 -775 {}
L 2 145 -1005 145 -775 {}
L 2 185 -1005 185 -775 {}
L 2 225 -1005 225 -775 {}
L 2 265 -1005 265 -775 {}
L 2 305 -1005 305 -775 {}
L 2 345 -1005 345 -775 {}
L 2 385 -1005 385 -775 {}
L 2 65 -975 395 -975 {}
L 2 65 -935 395 -935 {}
L 2 65 -895 395 -895 {}
L 2 65 -855 395 -855 {}
L 2 65 -815 395 -815 {}
L 4 65 -1015 65 -755 {}
L 4 55 -775 405 -775 {}
L 4 105 -775 105 -765 {}
L 4 145 -775 145 -765 {}
L 4 185 -775 185 -765 {}
L 4 225 -775 225 -765 {}
L 4 265 -775 265 -765 {}
L 4 305 -775 305 -765 {}
L 4 345 -775 345 -765 {}
L 4 385 -775 385 -765 {}
L 4 55 -815 65 -815 {}
L 4 55 -855 65 -855 {}
L 4 55 -895 65 -895 {}
L 4 55 -935 65 -935 {}
L 4 55 -975 65 -975 {}
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
L 4 880 -760 880 -720 {}
L 4 870 -720 880 -720 {}
L 4 870 -720 880 -710 {}
L 4 880 -710 890 -720 {}
L 4 880 -720 890 -720 {}
L 4 970 -380 970 -340 {}
L 4 960 -340 970 -340 {}
L 4 960 -340 970 -330 {}
L 4 970 -330 980 -340 {}
L 4 970 -340 980 -340 {}
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
color="7 4"
jpeg_quality=30
linewidth_mult=2.0
hilight_wave=-1}
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
\\"R.Avg. I(vpanel); i(vpanel) 20u ravg()\\""
jpeg_quality=30
linewidth_mult=2.0}
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
hilight_wave=-1
jpeg_quality=30
linewidth_mult=2.0}
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
ypos2=0.51596
jpeg_quality=30
linewidth_mult=2.0}
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
node="\\"CTRL1 Duty cycle; ctrl1 20u ravg()\\""
jpeg_quality=30
linewidth_mult=2.0}
B 18 65 -960 320 -775 {}
A 5 320 -960 5.590169943749475 243.434948822922 360 {fill=true}
P 7 6 395 -775 340 -931.25 335 -945 322.5 -960 310 -965 65 -975 {}
T {2x10 1W white LED} 1240 -200 0 0 0.4 0.4 {layer=8}
T {2xseries 1W white LEDs} 1220 -140 0 0 0.4 0.4 {}
T {2.5} 95 -755 0 0 0.2 0.2 {}
T {5.0} 135 -755 0 0 0.2 0.2 {}
T {7.5} 175 -755 0 0 0.2 0.2 {}
T {10.0} 215 -755 0 0 0.2 0.2 {}
T {12.5} 255 -755 0 0 0.2 0.2 {}
T {15.0} 295 -755 0 0 0.2 0.2 {}
T {17.5} 335 -755 0 0 0.2 0.2 {}
T {20.0} 375 -755 0 0 0.2 0.2 {}
T {2.5} 35 -980 0 0 0.2 0.2 {}
T {2.0} 35 -940 0 0 0.2 0.2 {}
T {1.5} 35 -895 0 0 0.2 0.2 {}
T {1.0} 35 -860 0 0 0.2 0.2 {}
T {0.5} 35 -820 0 0 0.2 0.2 {}
T {25C, 1000W/m2} 190 -1030 0 0 0.2 0.2 {}
T {V} 410 -765 0 0 0.4 0.4 {}
T {I} 40 -1020 0 0 0.4 0.4 {}
T {SOLAR PANEL} 145 -1055 0 0 0.4 0.4 {}
T {Maximum Power} 307.5 -980 0 0 0.2 0.2 {layer=8}
T {set between 0 and 1
to simulate
sun radiation
level} 10 -440 0 0 0.2 0.2 {}
T {@name} 875 -698.75 0 0 0.3 0.3 {layer=7 name=L2}
T {@value} 875 -673.75 0 0 0.3 0.3 {layer=7 name=L2}
T {m=@m} 875 -648.75 0 0 0.3 0.3 {layer=7 name=L2}
T {Floater text
example} 850 -820 0 0 0.4 0.4 {}
T {@name} 985 -308.75 0 1 0.3 0.3 {layer=7 name=C1}
T {@value} 985 -286.25 0 1 0.3 0.3 {layer=7 name=C1}
T {m=@m} 985 -263.75 0 1 0.3 0.3 {layer=7 name=C1}
T {Floater text
example} 870 -440 0 0 0.4 0.4 {}
N 1010 -210 1100 -210 {lab=0}
N 1100 -300 1100 -210 {lab=0}
N 640 -610 730 -610 {lab=#net1}
N 1010 -440 1040 -440 {lab=VO}
N 1010 -440 1010 -310 {lab=VO}
N 1010 -250 1010 -210 {lab=0}
N 530 -610 580 -610 {lab=PANEL}
N 1010 -610 1010 -440 {lab=VO}
N 820 -610 860 -610 {lab=SW}
N 820 -610 820 -490 {lab=SW}
N 790 -610 820 -610 {lab=SW}
N 820 -210 1010 -210 {lab=0}
N 800 -210 820 -210 {lab=0}
N 1000 -610 1010 -610 {lab=VO}
N 1100 -440 1140 -440 {lab=LED}
N 1100 -440 1100 -360 {lab=LED}
N 820 -430 820 -390 { lab=#net2}
N 920 -610 940 -610 { lab=#net3}
N 360 -530 360 -480 {
lab=#net4}
N 360 -420 360 -370 {lab=0}
N 360 -530 400 -530 {
lab=#net4}
N 760 -720 760 -650 {
lab=CTRL1}
N 460 -530 530 -530 {
lab=#net5}
N 590 -530 650 -530 {
lab=PANEL}
N 500 -390 500 -370 {
lab=0}
N 500 -470 500 -450 {
lab=#net6}
N 160 -450 320 -450 {
lab=SUN}
N 160 -360 160 -340 {
lab=0}
N 610 -1010 610 -990 {
lab=0}
N 610 -1080 610 -1070 {
lab=TRIANG}
N 610 -1080 770 -1080 {
lab=TRIANG}
N 720 -890 830 -890 {
lab=LEVEL}
N 550 -860 550 -820 {
lab=REF}
N 550 -860 600 -860 {
lab=REF}
N 550 -920 600 -920 {
lab=LED}
N 1010 -1000 1060 -1000 {
lab=CTRL1}
N 160 -450 160 -420 {
lab=SUN}
N 830 -970 830 -890 {
lab=LEVEL}
N 830 -970 890 -970 {
lab=LEVEL}
N 770 -1080 770 -1030 {
lab=TRIANG}
N 770 -1030 890 -1030 {
lab=TRIANG}
N 820 -330 820 -210 {
lab=0}
C {title.sym} 160 -40 0 0 {name=l1 author="Stefan Schippers" net_name=true}
C {code_shown.sym} 170 -310 0 0 {name=CONTROL
value="tcleval(
.control
  * example of tcl evaluation of code blocks:
  *   current path: $path 
  *   schematic: [xschem get current_name]
  tran .05u 1m uic
  write solar_panel.raw
  exit
.endc
)" 

net_name=true}
C {code.sym} 20 -240 0 0 {name=MODELS value=".MODEL DIODE D(IS=1.139e-08 RS=0.99 CJO=9.3e-12 VJ=1.6 M=0.411 BV=30 EG=0.7 ) 
.MODEL swmod SW(VT=0.5 VH=0.01 RON=0.01 ROFF=10000000)
" net_name=true}
C {lab_pin.sym} 650 -530 0 1 {name=l4  lab=PANEL }
C {lab_pin.sym} 800 -210 0 0 {name=l6  lab=0 }
C {ammeter.sym} 1070 -440 3 0 {name=Vled net_name=true}
C {ind.sym} 890 -610 3 1 {name=L2
m=1
value=40u
footprint=1206
device=inductor net_name=true
hide_texts=true}
C {lab_pin.sym} 1140 -440 0 1 {name=l7  lab=LED }
C {lab_pin.sym} 820 -550 0 1 {name=l9  lab=SW }
C {capa.sym} 1010 -280 0 0 {name=C1
m=1
value=500n
footprint=1206
device="ceramic capacitor" net_name=true
hide_texts=true}
C {lab_pin.sym} 1010 -400 0 1 {name=l10  lab=VO }
C {lab_pin.sym} 530 -610 0 0 {name=l3  lab=PANEL }
C {ammeter.sym} 970 -610 3 0 {name=Vind net_name=true}
C {isource_table.sym} 1100 -330 0 0 {name=G2[9..0] CTRL="V(LED)" TABLE="
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
C {ammeter.sym} 610 -610 3 0 {name=Vsw net_name=true}
C {ammeter.sym} 820 -360 2 0 {name=Vdiode net_name=true}
C {spice_probe.sym} 1120 -440 0 0 {name=p1 analysis=tran}
C {spice_probe.sym} 650 -530 0 0 {name=p2 analysis=tran}
C {spice_probe.sym} 820 -510 0 1 {name=p3 analysis=tran}
C {launcher.sym} 1080 -1140 0 0 {name=h3 
descr="Reload Waves" 
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw tran
"
}
C {pv_ngspice.sym} 360 -450 0 0 {name=X1  m=1 power=100 n=36}
C {lab_pin.sym} 360 -370 0 0 {name=l12  lab=0 }
C {capa.sym} 500 -500 0 0 {name=C11
m=1
value=10u
footprint=1206
device="ceramic capacitor" net_name=true}
C {lab_pin.sym} 500 -370 0 0 {name=l90  lab=0 }
C {ammeter.sym} 430 -530 3 1 {name=Vpanel net_name=true}
C {diode_ngspice.sym} 820 -460 2 0 {name=X2  m=1 Roff=1e9 Ron=0.1}
C {switch_ngspice.sym} 760 -610 1 0 {name=S1 model=swmod}
C {lab_pin.sym} 740 -650 0 0 {name=l5  lab=0 }
C {ammeter.sym} 560 -530 3 1 {name=Vpanel1 net_name=true}
C {ammeter.sym} 500 -420 0 0 {name=Vcap net_name=true}
C {vsource.sym} 160 -390 0 0 {name=Vfade value="pwl 0 1 1m 0"}
C {lab_pin.sym} 160 -340 0 0 {name=l8  lab=0 }
C {ngspice_get_expr.sym} 1120 -370 0 0 {name=r29 
node="[format %.4g [expr \{[ngspice::get_voltage led] * [ngspice::get_current vled]\}]] W"
descr = power
}
C {vsource.sym} 610 -1040 0 0 {name=Vtriang value="pulse 0 1 0 2u 2u 1f 4u"}
C {lab_pin.sym} 610 -990 0 0 {name=l11  lab=0 }
C {lab_pin.sym} 770 -1080 0 1 {name=l14
lab=TRIANG }
C {vsource.sym} 550 -790 0 0 {name=Vref value=7}
C {lab_pin.sym} 550 -760 0 0 {name=l17  lab=0 }
C {lab_pin.sym} 760 -720 0 0 {name=l13  lab=CTRL1 }
C {comp_ngspice.sym} 660 -890 0 0 {name=x3 OFFSET=0.5 AMPLITUDE=1 ROUT=7k COUT=1n}
C {lab_pin.sym} 550 -860 0 0 {name=l16
lab=REF}
C {lab_pin.sym} 550 -920 0 0 {name=l15  lab=LED}
C {lab_pin.sym} 830 -890 0 1 {name=l18  lab=LEVEL}
C {comp_ngspice.sym} 950 -1000 0 0 {name=x4 OFFSET=0.5 AMPLITUDE=1 ROUT=1 COUT=1p}
C {lab_pin.sym} 1060 -1000 0 1 {name=l19  lab=CTRL1 }
C {lab_pin.sym} 160 -450 0 0 {name=l20  lab=SUN }
C {spice_probe.sym} 850 -1030 0 1 {name=p4 analysis=tran}
C {spice_probe.sym} 810 -890 0 1 {name=p5 analysis=tran}
C {spice_probe.sym} 760 -670 0 0 {name=p6 analysis=tran}
C {spice_probe.sym} 160 -450 0 0 {name=p7 analysis=tran}
