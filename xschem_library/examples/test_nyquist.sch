v {xschem version=3.4.6RC file_version=1.2
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
}
G {}
K {}
V {}
S {}
E {}
B 2 1340 -650 2310 -140 {flags=graph,unlocked
y1=-13
y2=86
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=10

subdivx=8
xlabmag=1.0
ylabmag=1.0
node="\\"dB(out) comp; out db20() %0\\"
\\"dB(out) uncomp; out db20() %1\\"
\\"0 dB; 0"
color="4 5 8 4 4"
dataset=-1
unitx=1
logx=1
logy=0
divx=10
rainbow=0
lock=0
autoload=0
sim_type=ac}
B 2 1340 -1200 2310 -670 {flags=graph,unlocked
y1=-38
y2=180
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=-0.12711811
x2=9.8728819

subdivx=8
xlabmag=1.0
ylabmag=1.0
node="\\"ph(out) comp.; ph(out)%0\\"
\\"ph(out) uncomp.; ph(out)%1\\""
color="4 5"

unitx=1
logx=1
logy=0
divx=10
rainbow=0
lock=0
dataset=-1
autoload=0
sim_type=ac}
B 2 2330 -725 3150 -10 {flags=graph,unlocked
y1=-5.4178481
y2=4.8134072
ypos1=0
ypos2=2

subdivy=1
unity=1
x1=-45.601981
x2=5.9488199

subdivx=4
xlabmag=1.4
ylabmag=1.0
node="\\"im_out; im_out%0\\"
\\"im_out*; im_out -1 *%0\\"
\\"im_out; im_out%1\\"
\\"im_out*; im_out -1 *%1\\""
color="4 4 5 5"
dataset=-1
unitx=1
logx=0
logy=0
divx=10
sweep=re_out
divy=10
lock=1
rainbow=0
autoload=0
sim_type=ac}
B 2 2330 -1460 3150 -745 {flags=graph,unlocked
y1=-9400
y2=9400
ypos1=0
ypos2=2

subdivy=1
unity=1
x1=-323.03362
x2=18222.875

subdivx=1
xlabmag=1.4
ylabmag=1.0
node="\\"im_out; im_out%0\\"
\\"im_out*; im_out -1 *%0\\"
\\"im_out; im_out%1\\"
\\"im_out*; im_out -1 *%1\\""
color="4 4 7 7"

unitx=1
logx=0
logy=0
divx=10
sweep=re_out
divy=10
lock=1
rainbow=0
dataset=1
autoload=0
sim_type=ac}
B 4 930 -440 1050 -260 {fill=0 dash=6}
A 6 2450 -1100 40 0 360 {}
A 7 3020 -382.5 3.952847075210474 18.43494882292201 360 {fill=full}
P 6 7 2465 -1060 2635 -670 2645 -675 2650 -640 2625 -665 2635 -670 2465 -1060 {fill=full}
T {Feedback loading} 670 -930 0 0 0.4 0.4 {}
T {DC-only feedback} 670 -670 0 0 0.4 0.4 {}
T {Freq.
Compens.} 930 -250 0 0 0.4 0.4 { layer=4}
T {Uncompensated
Unstable} 2380 -1540 0 0 0.6 0.6 { layer=5}
T {Compensated
Stable} 2890 -1540 0 0 0.6 0.6 { layer=4}
T {Uncompensated
Unstable} 1950 -1280 0 0 0.6 0.6 { layer=5}
T {Compensated
Stable} 1480 -1280 0 0 0.6 0.6 { layer=4}
T {Some graphs are locked (lock=1) 
Double click close to the inner border
and set lock=0 to edit} 1370 -1450 0 0 0.8 0.8 {}
T {AC signal
gen} 530 -770 0 0 0.4 0.4 {}
N 30 -310 30 -280 {
lab=VCC}
N 30 -310 60 -310 {
lab=VCC}
N 260 -240 260 -190 {
lab=GN}
N 260 -190 300 -190 {
lab=GN}
N 300 -190 300 -160 {
lab=GN}
N 300 -160 530 -160 {
lab=GN}
N 500 -290 520 -290 {
lab=0}
N 670 -290 690 -290 {
lab=0}
N 570 -260 690 -260 {
lab=S}
N 440 -290 460 -290 {
lab=MINUS}
N 730 -290 750 -290 {
lab=PLUS}
N 690 -490 740 -490 {
lab=VCC}
N 450 -490 500 -490 {
lab=VCC}
N 540 -490 650 -490 {
lab=G}
N 540 -490 540 -460 {
lab=G}
N 500 -460 540 -460 {
lab=G}
N 690 -560 690 -520 {
lab=VCC}
N 570 -560 690 -560 {
lab=VCC}
N 500 -560 500 -520 {
lab=VCC}
N 570 -580 570 -560 {
lab=VCC}
N 690 -420 840 -420 {
lab=DIFFOUT}
N 30 -590 30 -560 {
lab=PLUS}
N 30 -590 60 -590 {
lab=PLUS}
N 570 -130 570 -100 {
lab=0}
N 570 -130 590 -130 {
lab=0}
N 590 -160 590 -130 {
lab=0}
N 570 -160 590 -160 {
lab=0}
N 260 -130 260 -100 {
lab=0}
N 240 -130 260 -130 {
lab=0}
N 240 -160 240 -130 {
lab=0}
N 240 -160 260 -160 {
lab=0}
N 690 -420 690 -400 {
lab=DIFFOUT}
N 570 -260 570 -190 {
lab=S}
N 500 -340 500 -320 {
lab=#net1}
N 690 -340 690 -320 {
lab=#net2}
N 500 -460 500 -400 {
lab=G}
N 1160 -130 1160 -100 {
lab=0}
N 1160 -130 1180 -130 {
lab=0}
N 1180 -160 1180 -130 {
lab=0}
N 1160 -160 1180 -160 {
lab=0}
N 1060 -160 1120 -160 {
lab=GN}
N 1160 -490 1210 -490 {
lab=VCC}
N 1160 -560 1160 -520 {
lab=VCC}
N 690 -560 1160 -560 {
lab=VCC}
N 840 -490 840 -420 {
lab=DIFFOUT}
N 1160 -290 1160 -190 {
lab=OUT}
N 500 -560 570 -560 {
lab=VCC}
N 690 -460 690 -420 {
lab=DIFFOUT}
N 500 -260 570 -260 {
lab=S}
N 1160 -330 1260 -330 {
lab=OUT}
N 1160 -460 1160 -330 {
lab=OUT}
N 990 -290 1160 -290 {
lab=OUT}
N 760 -980 900 -980 {lab=#net3}
N 680 -980 700 -980 {lab=0}
N 760 -1120 900 -1120 {lab=#net4}
N 680 -1120 700 -1120 {lab=0}
N 900 -1120 900 -1080 { lab=#net4}
N 900 -1020 900 -980 { lab=#net3}
N 940 -1050 960 -1050 { lab=OUT}
N 960 -1050 960 -790 { lab=OUT}
N 620 -790 730 -790 { lab=#net5}
N 920 -790 960 -790 { lab=OUT}
N 750 -1040 750 -1020 {
lab=S}
N 710 -1040 710 -1020 {
lab=0}
N 750 -1180 750 -1160 {
lab=G}
N 710 -1180 710 -1160 {
lab=0}
N 960 -790 980 -790 {
lab=OUT}
N 840 -790 840 -760 {
lab=#net6}
N 840 -790 860 -790 {
lab=#net6}
N 770 -790 770 -770 {
lab=#net6}
N 770 -790 840 -790 {
lab=#net6}
N 730 -720 730 -700 {
lab=0}
N 770 -700 840 -700 {
lab=0}
N 770 -730 770 -700 {
lab=0}
N 730 -790 730 -780 {
lab=#net5}
N 1160 -330 1160 -290 {
lab=OUT}
N 840 -490 1120 -490 {
lab=DIFFOUT}
N 730 -700 770 -700 {
lab=0}
N 520 -790 560 -790 { lab=MINUS}
N 990 -430 990 -410 {
lab=DIFFOUT}
N 990 -490 990 -430 {
lab=DIFFOUT}
C {lab_pin.sym} 30 -220 0 0 {name=p17 lab=0}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {nmos4.sym} 550 -160 0 0 {name=m1 model=cmosn w=5u l=2u m=2
}
C {pmos4.sym} 670 -490 0 0 {name=m2 model=cmosp w=5u l=2u m=1
}
C {vsource.sym} 30 -250 0 0 {name=VVCC value='VCC'
}
C {lab_pin.sym} 570 -100 0 0 {name=p1 lab=0}
C {lab_pin.sym} 60 -310 0 1 {name=p2 lab=VCC}
C {nmos4.sym} 280 -160 0 1 {name=m3 model=cmosn w=5u l=2u m=1
}
C {lab_pin.sym} 260 -100 0 0 {name=p3 lab=0}
C {isource.sym} 260 -270 0 0 {name=IBIAS value='IB'
}
C {lab_pin.sym} 260 -300 0 0 {name=p4 lab=0}
C {nmos4.sym} 480 -290 0 0 {name=m4 model=cmosn w=10u l=1u m=1
}
C {lab_pin.sym} 520 -290 0 1 {name=p5 lab=0}
C {nmos4.sym} 710 -290 0 1 {name=m5 model=cmosn w=10u l=1u m=1
}
C {lab_pin.sym} 670 -290 0 0 {name=p0 lab=0}
C {lab_pin.sym} 740 -490 0 1 {name=p6 lab=VCC}
C {pmos4.sym} 520 -490 0 1 {name=m6 model=cmosp w=5u l=2u m=1
}
C {lab_pin.sym} 450 -490 0 0 {name=p7 lab=VCC}
C {lab_pin.sym} 570 -580 0 0 {name=p8 lab=VCC}
C {lab_pin.sym} 750 -290 0 1 {name=p9 lab=PLUS}
C {lab_pin.sym} 440 -290 0 0 {name=p10 lab=MINUS}
C {lab_pin.sym} 840 -420 0 1 {name=p11 lab=DIFFOUT}
C {lab_pin.sym} 260 -220 0 0 {name=p13 lab=GN}
C {lab_pin.sym} 30 -500 0 0 {name=p16 lab=0}
C {vsource.sym} 30 -530 0 0 {name=VPLUS value="dc 2.5 pwl 0 2.5 100n 2.5 100.1n 2.51"
}
C {lab_pin.sym} 60 -590 0 1 {name=p18 lab=PLUS}
C {lab_pin.sym} 500 -430 0 0 {name=p12 lab=G}
C {launcher.sym} 700 -60 0 0 {name=h1
descr=Backannotate
tclcommand="
xschem raw read $netlist_dir/test_nyquist.raw ac
xschem raw add re_out \{out ph(out) re() -1 *\}
xschem raw add im_out \{out ph(out) im() -1 *\}
xschem raw read $netlist_dir/test_nyquist.raw op
xschem update_op
xschem redraw
"
}
C {launcher.sym} 700 -110 0 0 {name=h2
descr="View raw file"
tclcommand="textwindow $netlist_dir/cmos_example_ngspice.raw"}
C {ammeter.sym} 690 -370 0 1 {name=Vmeasr}
C {simulator_commands.sym} 60 -830 0 0 {name=COMMANDS
simulator=ngspice
only_toplevel=true 
value=".temp 30
.param IB=2u GMIN=1e-16
.param VCC = 5
.option savecurrents
.control
   save all
   op
   remzerovec
   write test_nyquist.raw
   set appendwrite
   ac dec 100 1 10G
   * tran 0.1n 1u
   remzerovec
   write test_nyquist.raw
   
   alter R1=5000G
   alter C1=1a

   ac dec 100 1 10G
   * tran 0.1n 1u
   remzerovec
   write test_nyquist.raw



   quit 0
.endc

"}
C {ammeter.sym} 500 -370 0 1 {name=Vmeasl}
C {lab_pin.sym} 570 -230 0 0 {name=p19 lab=S}
C {nmos4.sym} 1140 -160 0 0 {name=m7 model=cmosn w=5u l=2u m=1
device_model="
*n-ch model
.MODEL CMOSN NMOS (
+LEVEL   = 49             acm     = 3
+VERSION = 3.1            TNOM    = 27             TOX     = 7.7E-9
+XJ      = 1E-7           NCH     = 2.3579E17      VTH0    = 0.5048265
+K1      = 0.5542796      K2      = 0.0155863      K3      = 2.3475646
+K3B     = -3.3142916     W0      = 4.145888E-5    NLX     = 1.430868E-7
+DVT0W   = 0              DVT1W   = 0              DVT2W   = 0
+DVT0    = -0.0150839     DVT1    = 1.51022E-3     DVT2    = 0.170688
+U0      = 415.8570638    UA      = 5.057324E-11   UB      = 1.496793E-18
+UC      = 2.986268E-11   VSAT    = 1.237033E5     A0      = 0.9098788
+AGS     = 0.2120181      B0      = 1.683612E-6    B1      = 5E-6
+KETA    = -4.011887E-4   A1      = 0              A2      = 1
+RDSW    = 1.156967E3     PRWG    = -8.468558E-3   PRWB    = -7.678669E-3
+WR      = 1              WINT    = 5.621821E-8    LINT    = 1.606205E-8
+XL      = -2E-8          XW      = 0              DWG     = -6.450939E-9
+DWB     = 6.530228E-9    VOFF    = -0.1259348     NFACTOR = 0.3344887
+CIT     = 0              CDSC    = 1.527511E-3    CDSCD   = 0
+CDSCB   = 0              ETA0    = 1.21138E-3     ETAB    = -1.520242E-4
+DSUB    = 0.1259886      PCLM    = 0.8254768      PDIBLC1 = 0.4211084
+PDIBLC2 = 6.081164E-3    PDIBLCB = -5.865856E-6   DROUT   = 0.7022263
+PSCBE1  = 7.238634E9     PSCBE2  = 5E-10          PVAG    = 0.6261655
+DELTA   = 0.01           MOBMOD  = 1              PRT     = 0
+UTE     = -1.5           KT1     = -0.11          KT1L    = 0
+KT2     = 0.022          UA1     = 4.31E-9        UB1     = -7.61E-18
+UC1     = -5.6E-11       AT      = 3.3E4          WL      = 0
+WLN     = 1              WW      = -1.22182E-15   WWN     = 1.137
+WWL     = 0              LL      = 0              LLN     = 1
+LW      = 0              LWN     = 1              LWL     = 0
+CAPMOD  = 2              XPART   = 0.4            CGDO    = 1.96E-10
+CGSO    = 1.96E-10       CGBO    = 0              CJ      = 8.829973E-4
+PB      = 0.7946332      MJ      = 0.3539285      CJSW    = 2.992362E-10
+PBSW    = 0.9890846      MJSW    = 0.1871372      PVTH0   = -0.0148617
+PRDSW   = -114.7860236   PK2     = -5.151187E-3   WKETA   = 5.687313E-3
+LKETA   = -0.018518       )
"}
C {lab_pin.sym} 1160 -100 0 0 {name=p21 lab=0}
C {lab_pin.sym} 1060 -160 0 0 {name=p22 lab=GN}
C {pmos4.sym} 1140 -490 0 0 {name=m8 model=cmosp w=5u l=2u m=1
device_model="
*p-ch model
.MODEL CMOSP PMOS (
+LEVEL   = 49             acm     = 3
+VERSION = 3.1            TNOM    = 27             TOX     = 7.7E-9
+XJ      = 1E-7           NCH     = 8.52E16        VTH0    = -0.6897992
+K1      = 0.4134289      K2      = -5.342989E-3   K3      = 24.8361788
+K3B     = -1.4390847     W0      = 2.467689E-6    NLX     = 3.096223E-7
+DVT0W   = 0              DVT1W   = 0              DVT2W   = 0
+DVT0    = 1.3209807      DVT1    = 0.4695965      DVT2    = -8.790762E-4
+U0      = 150.6275733    UA      = 2.016943E-10   UB      = 1.714919E-18
+UC      = -1.36948E-11   VSAT    = 9.559222E4     A0      = 0.9871247
+AGS     = 0.3541967      B0      = 3.188091E-6    B1      = 5E-6
+KETA    = -0.0169877     A1      = 0              A2      = 1
+RDSW    = 2.443009E3     PRWG    = 0.0260616      PRWB    = 0.141561
+WR      = 1              WINT    = 5.038936E-8    LINT    = 1.650588E-9
+XL      = -2E-8          XW      = 0              DWG     = -1.535456E-8
+DWB     = 1.256904E-8    VOFF    = -0.15          NFACTOR = 1.5460516
+CIT     = 0              CDSC    = 1.413317E-4    CDSCD   = 0
+CDSCB   = 0              ETA0    = 0.3751392      ETAB    = 2.343374E-3
+DSUB    = 0.8877574      PCLM    = 5.8638076      PDIBLC1 = 1.05224E-3
+PDIBLC2 = 3.481753E-5    PDIBLCB = 2.37525E-3     DROUT   = 0.0277454
+PSCBE1  = 3.013379E10    PSCBE2  = 3.608179E-8    PVAG    = 3.9564294
+DELTA   = 0.01           MOBMOD  = 1              PRT     = 0
+UTE     = -1.5           KT1     = -0.11          KT1L    = 0
+KT2     = 0.022          UA1     = 4.31E-9        UB1     = -7.61E-18
+UC1     = -5.6E-11       AT      = 3.3E4          WL      = 0
+WLN     = 1              WW      = -5.22182E-16   WWN     = 1.125
+WWL     = 0              LL      = 0              LLN     = 1
+LW      = 0              LWN     = 1              LWL     = 0
+CAPMOD  = 2              XPART   = 0.4            CGDO    = 2.307E-10
+CGSO    = 2.307E-10      CGBO    = 0              CJ      = 1.397645E-3
+PB      = 0.99           MJ      = 0.5574537      CJSW    = 3.665392E-10
+PBSW    = 0.99           MJSW    = 0.3399328      PVTH0   = 0.0114364
+PRDSW   = 52.7951169     PK2     = 9.714153E-4    WKETA   = 0.0109418
+LKETA   = 7.702974E-3     )
"}
C {lab_pin.sym} 1210 -490 0 1 {name=p23 lab=VCC}
C {lab_pin.sym} 1260 -330 0 1 {name=p24 lab=OUT
lock=1}
C {launcher.sym} 2000 -100 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/test_nyquist.raw ac
xschem raw add re_out \{out ph(out) re() -1 *\}
xschem raw add im_out \{out ph(out) im() -1 *\}
xschem redraw
"
}
C {launcher.sym} 1750 -100 0 0 {name=h3
descr="Simulate" 
tclcommand="simulate"
}
C {res.sym} 990 -380 0 0 {name=R1
value=70k
footprint=1206
device=resistor
m=1}
C {capa.sym} 990 -320 0 0 {name=C1
m=1
value=200f
footprint=1206
device="ceramic capacitor"}
C {lab_pin.sym} 520 -790 0 0 {name=p20 lab=MINUS}
C {lab_pin.sym} 750 -1040 2 0 {name=l18 lab=S}
C {lab_pin.sym} 710 -1040 2 0 {name=l19 lab=0}
C {lab_pin.sym} 680 -980 1 0 {name=l20 lab=0}
C {vcvs.sym} 730 -980 1 0 {name=e1 value=1}
C {lab_pin.sym} 750 -1180 2 0 {name=l2 lab=G}
C {lab_pin.sym} 710 -1180 2 0 {name=l3 lab=0}
C {lab_pin.sym} 680 -1120 1 0 {name=l4 lab=0}
C {vcvs.sym} 730 -1120 1 0 {name=e2 value=1}
C {lab_pin.sym} 900 -1050 0 0 {name=p25 lab=0}
C {lab_pin.sym} 980 -790 0 1 {name=p27 lab=OUT}
C {nmos4.sym} 920 -1050 0 1 {name=m10 model=cmosn w=10u l=1u m=1
}
C {res.sym} 890 -790 1 0 {name=R3
value=10G
m=1}
C {capa.sym} 840 -730 0 0 {name=C3
m=1
value=1
footprint=1206
device="ceramic capacitor"}
C {vcvs.sym} 730 -750 0 1 {name=e3 value=1}
C {lab_pin.sym} 730 -700 0 0 {name=l8 lab=0}
C {vsource.sym} 590 -790 3 0 {name=VLOOP value="dc 0 ac 1 0"
}
C {launcher.sym} 1480 -100 0 0 {name=h4
descr="netlist" 
tclcommand="xschem netlist"
}
