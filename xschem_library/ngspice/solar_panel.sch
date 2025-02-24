v {xschem version=3.4.6 file_version=1.2
*
* This file is part of XSCHEM,
* a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
* simulation.
* Copyright (C) 1998-2024 Stefan Frederik Schippers
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
L 2 95 -1105 95 -875 {}
L 2 135 -1105 135 -875 {}
L 2 175 -1105 175 -875 {}
L 2 215 -1105 215 -875 {}
L 2 255 -1105 255 -875 {}
L 2 295 -1105 295 -875 {}
L 2 335 -1105 335 -875 {}
L 2 375 -1105 375 -875 {}
L 2 55 -1075 385 -1075 {}
L 2 55 -1035 385 -1035 {}
L 2 55 -995 385 -995 {}
L 2 55 -955 385 -955 {}
L 2 55 -915 385 -915 {}
L 4 55 -1115 55 -855 {}
L 4 45 -875 395 -875 {}
L 4 95 -875 95 -865 {}
L 4 135 -875 135 -865 {}
L 4 175 -875 175 -865 {}
L 4 215 -875 215 -865 {}
L 4 255 -875 255 -865 {}
L 4 295 -875 295 -865 {}
L 4 335 -875 335 -865 {}
L 4 375 -875 375 -865 {}
L 4 45 -915 55 -915 {}
L 4 45 -955 55 -955 {}
L 4 45 -995 55 -995 {}
L 4 45 -1035 55 -1035 {}
L 4 45 -1075 55 -1075 {}
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
B 2 1370 -555 1790 -385 {flags=graph 
y1 = -0.00068
y2 = 22
divy = 6
subdivy=1
x1=5e-10
x2=0.001
divx=8
node="PANEL
LED" unitx=m
color="7 4"
jpeg_quality=30
linewidth_mult=2.0
hilight_wave=-1



autoload=0

sim_type=tran
xrawfile=$netlist_dir/solar_panel.raw}
B 2 1370 -385 1790 -215 {flags=graph 
y1 = -0.0012
y2 = 6.8
divy = 4
subdivy=1
x1=5e-10
x2=0.001
divx=8
  unitx=m
color="7 4 6"
node="i(Vpanel)
i(Vled)
\\"R.Avg. I(VPANEL); i(VPANEL) 20u ravg()\\""
jpeg_quality=30
linewidth_mult=2.0
autoload=0

sim_type=tran
xrawfile=$netlist_dir/solar_panel.raw}
B 2 1370 -745 1790 -555 {flags=graph 
y1 = -2.7e-05
y2 = 100
divy = 5
subdivy=1
x1=5e-10
x2=0.001
divx=9

 unitx=m subdivx=4
color="7 4 6 8 4"
node="Panel power; i(Vpanel)\\\\
 v(PANEL) *
Led power; i(Vled) v(LED) *
Avg.Pan. Pwr; i(Vpanel) v(PANEL) * 20u ravg()
SUN \\\\%; SUN 100 *"
hilight_wave=-1
jpeg_quality=30
linewidth_mult=2.0
autoload=0

sim_type=tran
xrawfile=$netlist_dir/solar_panel.raw}
B 2 1370 -935 1790 -745 {flags=graph 
y1 = 0
y2 = 1
divy = 5
subdivy=1
x1=5e-10
x2=0.001
divx=9

 unitx=u subdivx=4
color="14 7"
node="TRIANG
LEVEL"
hilight_wave=-1
digital=0
ypos1=0.00261891
ypos2=0.51596
jpeg_quality=30
linewidth_mult=2.0
autoload=0

sim_type=tran
xrawfile=$netlist_dir/solar_panel.raw}
B 2 1370 -1135 1790 -945 {flags=graph 
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
node="\\"CTRL1 Duty cycle; CTRL1 20u ravg()\\""
jpeg_quality=30
linewidth_mult=2.0
autoload=0

sim_type=tran
xrawfile=$netlist_dir/solar_panel.raw}
B 2 1210 -605 1320 -505 {name=l21
flags=graph,unlocked
lock=1
color=8
node="tcleval([xschem translate l21 @#0:net_name] [xschem translate l21 @#1:net_name] -)"
x1=5e-10
x2=0.001
y1=-4.8
y2=15}
B 2 400 -735 510 -635 {name=l22
flags=graph,unlocked
lock=1
color=8
node="tcleval([xschem translate l22 @#0:net_name])"
y1=0.00033
y2=21
x1=5e-10
x2=0.001}
B 2 600 -485 710 -385 {name=l23
flags=graph,unlocked
lock=1
color=8
node="tcleval([xschem get_fqdevice [xschem translate l23 @device]])"
x1=5e-10
x2=0.001
y1=-2.9
y2=6.8}
B 2 640 -295 750 -195 {name=l26
flags=graph,unlocked
lock=1
color=8
node="tcleval([xschem get_fqdevice [xschem translate l26 @device]])"
x1=5e-10
x2=0.001
y1=-2.9
y2=6.8}
B 18 55 -1060 310 -875 {}
A 5 310 -1060 5.590169943749475 243.434948822922 360 {fill=true}
P 7 6 385 -875 330 -1031.25 325 -1045 312.5 -1060 300 -1065 55 -1075 {}
T {2x10 1W white LED} 1350 -195 0 0 0.4 0.4 {layer=8}
T {2xseries 1W white LEDs} 1330 -135 0 0 0.4 0.4 {}
T {2.5} 85 -855 0 0 0.2 0.2 {}
T {5.0} 125 -855 0 0 0.2 0.2 {}
T {7.5} 165 -855 0 0 0.2 0.2 {}
T {10.0} 205 -855 0 0 0.2 0.2 {}
T {12.5} 245 -855 0 0 0.2 0.2 {}
T {15.0} 285 -855 0 0 0.2 0.2 {}
T {17.5} 325 -855 0 0 0.2 0.2 {}
T {20.0} 365 -855 0 0 0.2 0.2 {}
T {2.5} 25 -1080 0 0 0.2 0.2 {}
T {2.0} 25 -1040 0 0 0.2 0.2 {}
T {1.5} 25 -995 0 0 0.2 0.2 {}
T {1.0} 25 -960 0 0 0.2 0.2 {}
T {0.5} 25 -920 0 0 0.2 0.2 {}
T {25C, 1000W/m2} 180 -1130 0 0 0.2 0.2 {}
T {V} 400 -865 0 0 0.4 0.4 {}
T {I} 30 -1120 0 0 0.4 0.4 {}
T {SOLAR PANEL} 135 -1155 0 0 0.4 0.4 {}
T {Maximum Power} 297.5 -1080 0 0 0.2 0.2 {layer=8}
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
T {@spice_get_current} 875 -598.75 0 0 0.3 0.3 {layer=7 name=L2}
T {@spice_get_current} 1015 -268.75 0 0 0.3 0.3 {layer=7 name=C1}
N 1010 -180 1100 -180 {lab=0}
N 1100 -300 1100 -180 {lab=0}
N 640 -610 730 -610 {lab=#net1}
N 1010 -440 1040 -440 {lab=VO}
N 1010 -440 1010 -310 {lab=VO}
N 1010 -250 1010 -180 {lab=0}
N 360 -610 580 -610 {lab=PANEL}
N 1010 -610 1010 -440 {lab=VO}
N 820 -610 860 -610 {lab=SW}
N 820 -610 820 -490 {lab=SW}
N 790 -610 820 -610 {lab=SW}
N 820 -180 1010 -180 {lab=0}
N 800 -180 820 -180 {lab=0}
N 1000 -610 1010 -610 {lab=VO}
N 1100 -440 1280 -440 {lab=LED}
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
N 820 -330 820 -180 {
lab=0}
N 1190 -480 1190 -440 {lab=LED}
N 1190 -670 1190 -640 {lab=PANEL}
C {title.sym} 160 -40 0 0 {name=l1 author="Stefan Schippers"}
C {code_shown.sym} 180 -310 0 0 {name=CONTROL
value="tcleval(
.option savecurrents
.control
  * example of tcl evaluation of code blocks:
  *   current path: $path 
  *   schematic: [xschem get current_name]
  save all
  tran .05u 1m uic
  write solar_panel.raw
  quit 0
.endc
)
" 

}
C {code.sym} 20 -240 0 0 {name=MODELS value=".MODEL DIODE D(IS=1.139e-08 RS=0.99 CJO=9.3e-12 VJ=1.6 M=0.411 BV=30 EG=0.7 ) 
.MODEL swmod SW(VT=0.5 VH=0.01 RON=0.01 ROFF=10000000)
"}
C {lab_pin.sym} 650 -530 0 1 {name=l4  lab=PANEL }
C {lab_pin.sym} 800 -180 0 0 {name=l6  lab=0 }
C {ammeter.sym} 1070 -440 3 0 {name=Vled}
C {ind.sym} 890 -610 3 1 {name=L2
m=1
value=40u
footprint=1206
device=inductor
hide_texts=true
attach=L2}
C {lab_pin.sym} 1280 -440 0 1 {name=l7  lab=LED }
C {lab_pin.sym} 820 -550 0 1 {name=l9  lab=SW }
C {capa.sym} 1010 -280 0 0 {name=C1
m=1
value=500n
footprint=1206
device="ceramic capacitor"
hide_texts=true
attach=C1}
C {lab_pin.sym} 1010 -400 0 1 {name=l10  lab=VO }
C {lab_pin.sym} 360 -610 0 0 {name=l3  lab=PANEL }
C {ammeter.sym} 970 -610 3 0 {name=Vind}
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
}
C {ammeter.sym} 610 -610 3 0 {name=Vsw}
C {ammeter.sym} 820 -360 2 0 {name=Vdiode}
C {spice_probe.sym} 1120 -440 0 0 {name=p1 analysis=tran}
C {spice_probe.sym} 650 -530 0 0 {name=p2 analysis=tran}
C {spice_probe.sym} 820 -510 0 1 {name=p3 analysis=tran}
C {launcher.sym} 1080 -1140 0 0 {name=h3 
descr="Reload Waves" 
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw tran
"
}
C {pv_ngspice.sym} 360 -450 0 0 {name=X1  m=1 isc=6.8 n=36}
C {lab_pin.sym} 360 -370 0 0 {name=l12  lab=0 }
C {capa.sym} 500 -500 0 0 {name=C11
m=1
value=10u
footprint=1206
device="ceramic capacitor"}
C {lab_pin.sym} 500 -370 0 0 {name=l90  lab=0 }
C {ammeter.sym} 430 -530 3 1 {name=Vpanel}
C {diode_ngspice.sym} 820 -460 2 0 {name=X2  m=1 Roff=1e9 Ron=0.1}
C {switch_ngspice.sym} 760 -610 1 0 {name=S1 model=swmod}
C {lab_pin.sym} 740 -650 0 0 {name=l5  lab=0 }
C {ammeter.sym} 560 -530 3 1 {name=Vpanel1}
C {ammeter.sym} 500 -420 0 0 {name=Vcap}
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
C {lab_show.sym} 440 -610 2 0 {name=l24}
C {lab_pin.sym} 1190 -670 0 0 {name=l25  lab=PANEL }
C {scope2.sym} 1190 -480 0 0 {name=l21
attach=l21}
C {scope.sym} 380 -610 0 0 {name=l22
attach=l22}
C {scope_ammeter.sym} 580 -360 0 0 {name=l23
attach=l23
device=Vcap}
C {scope_ammeter.sym} 620 -170 0 0 {name=l26
attach=l26
device=Vdiode}
