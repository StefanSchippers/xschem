v {xschem version=3.4.7RC file_version=1.2}
G {}
K {}
V {}
S {}
E {}
B 2 880 -290 1430 -110 {flags=graph
y1=-42
y2=12
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0.0035110393
x2=0.0037319804
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
node="batt
supply
diode
g1
g2"
color="4 7 17 12 21"
dataset=-1
unitx=1
logx=0
logy=0
hilight_wave=-1
linewidth_mult=1.2}
B 2 880 -400 1430 -290 {flags=graph
y1=-0.00023
y2=12
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0.0035110393
x2=0.0037319804
divx=5
subdivx=1
xlabmag=1.5
ylabmag=1.0
dataset=-1
unitx=1
logx=0
logy=0
hilight_wave=2
color="4 7"
node="pwm1
g1"
linewidth_mult=1.2}
B 2 880 -500 1430 -400 {flags=graph
y1=-1.5
y2=36
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0.0035110393
x2=0.0037319804
divx=5
subdivx=1
xlabmag=1.5
ylabmag=1.0
dataset=-1
unitx=1
logx=0
logy=0
hilight_wave=-1
color="4 7 17"
node="i(vm1)
i(vbatt)
i(vind)"
linewidth_mult=1.2}
B 2 880 -590 1430 -500 {flags=graph
y1=0
y2=61
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0.0035110393
x2=0.0037319804
divx=5
subdivx=1
xlabmag=1.5
ylabmag=1.0
dataset=-1
unitx=1
logx=0
logy=0
hilight_wave=-1
color="4 7"
node="i(vbatt) batt * 24u ravg()
i(vm1) supply * 24u ravg()"
linewidth_mult=1.2}
B 2 880 -700 1430 -590 {flags=graph
y1=0
y2=2.8
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0.0035110393
x2=0.0037319804
divx=5
subdivx=1
xlabmag=1.5
ylabmag=1.0
dataset=-1
unitx=1
logx=0
logy=0
hilight_wave=-1
linewidth_mult=1.2
color=4
node="M1_PWR;supply diode - i(vm1) * 24u ravg()"}
T {@name} 235 -168.75 0 0 0.2 0.2 {name=Rind1}
T {@value} 235 -156.25 0 0 0.2 0.2 {name=Rind1}
T {m=@m} 235 -143.75 0 0 0.2 0.2 {name=Rind1}
T {@name} 315 -228.75 0 0 0.2 0.2 {name=l8}
T {@value} 235 -216.25 0 0 0.2 0.2 {name=l8}
T {m=@m} 235 -203.75 0 0 0.2 0.2 {name=l8}
T {@name} 505 -368.75 0 0 0.2 0.2 {name=XD2}
T {@model} 505 -356.25 0 0 0.2 0.2 {name=XD2}
N 780 -330 800 -330 {lab=BATT}
N 780 -120 780 -100 {lab=0}
N 680 -330 680 -180 {lab=BATT}
N 680 -120 680 -100 {lab=0}
N 780 -330 780 -240 {lab=BATT}
N 680 -330 780 -330 {lab=BATT}
N 400 -330 500 -330 {lab=DIODE}
N 110 -430 150 -430 {lab=G1}
N -50 -430 30 -430 {lab=PWM1}
N 80 -170 80 -100 {lab=0}
N 80 -330 80 -230 {lab=SUPPLY}
N 300 -250 300 -240 {lab=#net1}
N 300 -120 300 -100 {lab=0}
N 560 -330 680 -330 {lab=BATT}
N 80 -330 120 -330 {lab=SUPPLY}
N 150 -430 150 -370 {lab=G1}
N 300 -330 340 -330 {lab=#net2}
N 280 -330 300 -330 {lab=#net2}
N 180 -330 220 -330 {lab=#net3}
N 300 -330 300 -310 {lab=#net2}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {code_shown.sym} 10 -740 0 0 {
name=CONTROL 
tclcommand="xschem edit_vi_prop"
place=end
value="
.ic v(diode)=12 v(batt)=42
.param VCC=12
.include stimuli_buck_boost.cir
.option method=gear gmin=1e-10
+ itl1=1000 itl2=100 itl4=1000 itl5=0
.control
  save all
  tran 0.02u 5000u uic
  remzerovec
  write buck_boost.raw
  quit 0
.endc
"}
C {launcher.sym} 935 -85 0 0 {name=h2 
descr="Simulate" 
tclcommand="set_sim_defaults; set sim(spice,0,fg) 1; xschem netlist; xschem simulate;xschem raw_read $netlist_dir/buck_boost.raw tran"
}
C {res.sym} 780 -150 0 0 {name=RLOAD value=30 footprint=1206 device=resistor m=1}
C {lab_pin.sym} 800 -330 2 0 {name=l6 lab=BATT}
C {lab_pin.sym} 780 -100 0 0 {name=l2 lab=0}
C {capa.sym} 680 -150 0 0 {name=C1 m=1 value="20u ic=42" footprint=1206 device="ceramic capacitor"}
C {lab_pin.sym} 680 -100 0 0 {name=l7 lab=0}
C {lab_wire.sym} 420 -330 0 1 {name=l10 lab=DIODE}
C {lab_pin.sym} 80 -330 0 0 {name=l11 lab=SUPPLY}
C {ammeter.sym} 780 -210 0 1 {name=vbatt}
C {launcher.sym} 1160 -90 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/buck_boost.raw tran"
}
C {ipin.sym} -50 -430 0 0 {name=p3 lab=PWM1}
C {ammeter.sym} 370 -330 1 0 {name=vdiode}
C {diode.sym} 530 -330 1 0 {name=XD2 model=STPST15H100SB area=1
format="@name @pinlist @model"
device_model="
*******************************************************************
* Model name       : STPST15H100SB
* Description      : 100 V - 15 A power Schottky trench diode
* Package type     : DPAK
*******************************************************************
*model STPST15H100SB  anode
*                     | cathode
*                     | |
*                     | |      
.subckt STPST15H100SB 1 2
r1 1 13 0.1E-3
r2 1 13 0.1E-3
d1 13 2 dx
.model dx       D(IS=383.61E-9 N=1.0502 RS=10.043E-3 IKF=3.9178 CJO=2.0881E-9 M=1.0083
+ VJ=5.1229 ISR=262.18E-9 TT=0 EG=.69 XTI=2 FC=0.5)
.ends
"
hide_texts=true
attach=XD2}
C {lab_pin.sym} 80 -100 0 0 {name=l3 lab=0}
C {ind.sym} 300 -210 0 0 {name=l8 value="50u ic=5"
hide_texts=true
attach=l8}
C {res.sym} 300 -150 0 0 {name=Rind1 value=0.05 footprint=1206 device=resistor m=1
hide_texts=true
attach=Rind1}
C {lab_wire.sym} 150 -430 0 1 {name=l4 lab=G1}
C {buf_ngspice.sym} 70 -430 0 0 {name=x1 RUP=10 RDOWN=10}
C {vsource.sym} 80 -200 0 0 {name=VSUPPLY value=12 savecurrent=false}
C {lab_pin.sym} 300 -100 0 0 {name=l12 lab=0}
C {pmos3.sym} 150 -350 3 1 {name=XM1 model=irf5305 m=1
device_model="
.SUBCKT irfr5410 1 2 3
**************************************
* Model Generated by MODPEX          *
*Copyright(c) Symmetry Design Systems*
* All Rights Reserved                *
* UNPUBLISHED LICENSED SOFTWARE      *
* Contains Proprietary Information   *
* Which is The Property of           *
* SYMMETRY OR ITS LICENSORS          *
*Commercial Use or Resale Restricted *
* by Symmetry License Agreement      *
**************************************
* Model generated on Mar 8, 01
* MODEL FORMAT: SPICE3
* Symmetry POWER MOS Model (Version 1.0)
* External Node Designations
* Node 1 -> Drain
* Node 2 -> Gate
* Node 3 -> Source
M1 9 7 8 8 MM L=100u W=100u
.MODEL MM PMOS LEVEL=1 IS=1e-32
+VTO=-4.10332 LAMBDA=0.044529 KP=3.57307
+CGSO=5.98432e-06 CGDO=1e-11
RS 8 3 0.110361
D1 1 3 MD
.MODEL MD D IS=2.32858e-09 RS=0.0349622 N=1.5 BV=100
+IBV=0.00025 EG=1 XTI=1 TT=0.0001
+CJO=5.85483e-10 VJ=0.500001 M=0.463449 FC=0.5
RDS 3 1 1e+06
RD 9 1 0.0177033
RG 2 7 9.86001
D2 5 4 MD1
* Default values used in MD1:
* RS=0 EG=1.11 XTI=3.0 TT=0
* BV=infinite IBV=1mA
.MODEL MD1 D IS=1e-32 N=50
+CJO=1.41944e-09 VJ=1.36483 M=0.723945 FC=1e-08
D3 5 0 MD2
* Default values used in MD2:
* EG=1.11 XTI=3.0 TT=0 CJO=0
* BV=infinite IBV=1mA
.MODEL MD2 D IS=1e-10 N=1 RS=3.0002e-06
RL 5 10 1
FI2 7 9 VFI2 -1
VFI2 4 0 0
EV16 10 0 9 7 1
CAP 11 10 2.27857e-09
FI1 7 9 VFI1 -1
VFI1 11 6 0
RCAP 6 10 1
D4 6 0 MD3
* Default values used in MD3:
* EG=1.11 XTI=3.0 TT=0 CJO=0
* RS=0 BV=infinite IBV=1mA
.MODEL MD3 D IS=1e-10 N=1
.ENDS irfr5410





.SUBCKT irf5305 1 2 3
**************************************
*      Model Generated by MODPEX     *
*Copyright(c) Symmetry Design Systems*
*         All Rights Reserved        *
*    UNPUBLISHED LICENSED SOFTWARE   *
*   Contains Proprietary Information *
*      Which is The Property of      *
*     SYMMETRY OR ITS LICENSORS      *
*Commercial Use or Resale Restricted *
*   by Symmetry License Agreement    *
**************************************
* Model generated on Apr 23, 96
* Model format: SPICE3
* Symmetry POWER MOS Model (Version 1.0)
* External Node Designations
* Node 1 -> Drain
* Node 2 -> Gate
* Node 3 -> Source
M1 9 7 8 8 MM L=100u W=100u
* Default values used in MM:
* The voltage-dependent capacitances are
* not included. Other default values are:
*   RS=0 RD=0 LD=0 CBD=0 CBS=0 CGBO=0
.MODEL MM PMOS LEVEL=1 IS=1e-32
+VTO=-3.45761 LAMBDA=0 KP=10.066
+CGSO=1.03141e-05 CGDO=1e-11
RS 8 3 0.0262305
D1 1 3 MD
.MODEL MD D IS=8.90854e-09 RS=0.00921723 N=1.5 BV=55
+IBV=0.00025 EG=1.2 XTI=3.13635 TT=1e-07
+CJO=1.30546e-09 VJ=0.663932 M=0.419045 FC=0.5
RDS 3 1 2.2e+06
RD 9 1 0.0001
RG 2 7 11.0979
D2 5 4 MD1
* Default values used in MD1:
*   RS=0 EG=1.11 XTI=3.0 TT=0
*   BV=infinite IBV=1mA
.MODEL MD1 D IS=1e-32 N=50
+CJO=1.63729e-09 VJ=0.791199 M=0.552278 FC=1e-08
D3 5 0 MD2
* Default values used in MD2:
*   EG=1.11 XTI=3.0 TT=0 CJO=0
*   BV=infinite IBV=1mA
.MODEL MD2 D IS=1e-10 N=0.400245 RS=3e-06
RL 5 10 1
FI2 7 9 VFI2 -1
VFI2 4 0 0
EV16 10 0 9 7 1
CAP 11 10 1.89722e-09
FI1 7 9 VFI1 -1
VFI1 11 6 0
RCAP 6 10 1
D4 6 0 MD3
* Default values used in MD3:
*   EG=1.11 XTI=3.0 TT=0 CJO=0
*   RS=0 BV=infinite IBV=1mA
.MODEL MD3 D IS=1e-10 N=0.400245
.ENDS


"}
C {ammeter.sym} 250 -330 3 1 {name=vm1}
C {ammeter.sym} 300 -280 0 1 {name=vind}
