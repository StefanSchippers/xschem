v {xschem version=3.4.7RC file_version=1.2}
G {}
K {}
V {}
S {}
E {}
B 2 880 -290 1430 -110 {flags=graph
y1=0.00013
y2=45
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0.0042932407
x2=0.0044053357
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
y1=0
y2=13
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0.0042932407
x2=0.0044053357
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
y1=1.3
y2=7.5
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0.0042932407
x2=0.0044053357
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
node="i(vm1)
i(vbatt)"
linewidth_mult=1.2}
B 2 880 -590 1430 -500 {flags=graph
y1=0
y2=78
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0.0042932407
x2=0.0044053357
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
y1=-0.014
y2=2.6
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0.0042932407
x2=0.0044053357
divx=5
subdivx=1
xlabmag=1.5
ylabmag=1.0
dataset=-1
unitx=1
logx=0
logy=0
hilight_wave=-1
color="4 10 8"
node="M3_PWR; diode i(vm3) * 24u ravg()
D2_PWR; i(vm2) diode batt - * 24u ravg()
L_PWR;L1 DIODE - i(vm1) * 24u ravg()"
linewidth_mult=1.2}
T {@name} 295 -318.75 0 0 0.2 0.2 {name=Rind1}
T {@value} 295 -306.25 0 0 0.2 0.2 {name=Rind1}
T {m=@m} 295 -293.75 0 0 0.2 0.2 {name=Rind1}
T {@name} 205 -318.75 0 0 0.2 0.2 {name=l8}
T {@value} 205 -306.25 0 0 0.2 0.2 {name=l8}
T {m=@m} 205 -293.75 0 0 0.2 0.2 {name=l8}
T {@name} 655 -308.75 0 1 0.2 0.2 {name=XD2}
T {@model} 655 -296.25 0 1 0.2 0.2 {name=XD2}
N 780 -270 800 -270 {lab=BATT}
N 780 -120 780 -100 {lab=0}
N 680 -270 680 -180 {lab=BATT}
N 680 -120 680 -100 {lab=0}
N 780 -270 780 -240 {lab=BATT}
N 680 -270 780 -270 {lab=BATT}
N 490 -270 590 -270 {lab=DIODE}
N 370 -270 430 -270 {lab=#net1}
N 160 -270 190 -270 {lab=#net2}
N 650 -270 680 -270 {lab=BATT}
N 80 -270 100 -270 {lab=SUPPLY}
N 370 -120 370 -100 {lab=0}
N 340 -270 370 -270 {lab=#net1}
N 290 -150 330 -150 {lab=G1}
N 170 -150 210 -150 {lab=PWM1}
N 250 -270 280 -270 {lab=L1}
N 370 -270 370 -240 {lab=#net1}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {code_shown.sym} 10 -740 0 0 {
name=CONTROL 
tclcommand="xschem edit_vi_prop"
place=end
value="
.ic v(diode)=12 v(batt)=42
.param VCC=12
.include stimuli_boost.cir
.option method=gear gmin=1e-12
+ itl1=1000 itl2=1000 itl4=1000 itl5=0
.control
  save all
  tran 0.02u 5000u uic
  remzerovec
  write boost.raw
  quit 0
.endc
"}
C {launcher.sym} 935 -85 0 0 {name=h2 
descr="Simulate" 
tclcommand="set_sim_defaults; set sim(spice,0,fg) 1; xschem netlist; xschem simulate;xschem raw_read $netlist_dir/boost.raw tran"}
C {res.sym} 780 -150 0 0 {name=RLOAD value=30 footprint=1206 device=resistor m=1}
C {lab_pin.sym} 800 -270 2 0 {name=l6 lab=BATT}
C {lab_pin.sym} 780 -100 0 0 {name=l2 lab=0}
C {capa.sym} 680 -150 0 0 {name=C1 m=1 value="30u ic=42" footprint=1206 device="ceramic capacitor"}
C {lab_pin.sym} 680 -100 0 0 {name=l7 lab=0}
C {lab_wire.sym} 530 -270 0 1 {name=l10 lab=DIODE}
C {lab_pin.sym} 80 -270 0 0 {name=l11 lab=SUPPLY}
C {ammeter.sym} 780 -210 0 1 {name=vbatt}
C {launcher.sym} 1160 -90 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/boost.raw tran"
}
C {ipin.sym} 170 -150 0 0 {name=p3 lab=PWM1}
C {ammeter.sym} 460 -270 3 1 {name=vm2}
C {ammeter.sym} 130 -270 3 1 {name=vm1}
C {diode.sym} 620 -270 3 1 {name=XD2 model=STPST15H100SB area=1
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
C {nmos3.sym} 350 -150 0 0 {name=xm3 model=irf540 m=1
program=evince
url="https://www.vishay.com/docs/91021/irf540.pdf"

device_model="
.SUBCKT irf540 1 2 3
M1 9 7 8 8 MM L=100u W=100u
* Default values used in MM:
* The voltage-dependent capacitances are
* not included. Other default values are:
* RS=0 RD=0 LD=0 CBD=0 CBS=0 CGBO=0
.MODEL MM NMOS LEVEL=1 IS=1e-32
+VTO=3.56362 LAMBDA=0.00291031 KP=25.0081
+CGSO=1.60584e-05 CGDO=4.25919e-07
RS 8 3 0.0317085
D1 3 1 MD
.MODEL MD D IS=1.02194e-10 RS=0.00968022 N=1.21527 BV=100
+IBV=0.00025 EG=1.2 XTI=3.03885 TT=1e-07
+CJO=1.81859e-09 VJ=1.1279 M=0.449161 FC=0.5
RDS 3 1 4e+06
RD 9 1 0.0135649
RG 2 7 5.11362
D2 4 5 MD1
* Default values used in MD1:
* RS=0 EG=1.11 XTI=3.0 TT=0
* BV=infinite IBV=1mA
.MODEL MD1 D IS=1e-32 N=50
+CJO=2.49697e-09 VJ=0.5 M=0.9 FC=1e-08
D3 0 5 MD2
* Default values used in MD2:
* EG=1.11 XTI=3.0 TT=0 CJO=0
* BV=infinite IBV=1mA
.MODEL MD2 D IS=1e-10 N=0.4 RS=3e-06
RL 5 10 1
FI2 7 9 VFI2 -1
VFI2 4 0 0
EV16 10 0 9 7 1
CAP 11 10 2.49697e-09
FI1 7 9 VFI1 -1
VFI1 11 6 0
RCAP 6 10 1
D4 0 6 MD3
* Default values used in MD3:
* EG=1.11 XTI=3.0 TT=0 CJO=0
* RS=0 BV=infinite IBV=1mA
.MODEL MD3 D IS=1e-10 N=0.4
.ENDS
"

xdevice_model="
.SUBCKT IRF540 2 1 3 ; rev 19 July 1999
CA 12 8 1.95e-9
CB 15 14 1.90e-9
CIN 6 8 1.12e-9
DBODY 7 5 DBODYMOD
DBREAK 5 11 DBREAKMOD
DPLCAP 10 5 DPLCAPMOD
EBREAK 11 7 17 18 112.8
EDS 14 8 5 8 1
EGS 13 8 6 8 1
ESG 6 10 6 8 1
EVTHRES 6 21 19 8 1
EVTEMP 20 6 18 22 1
IT 8 17 1
LDRAIN 2 5 1.0e-9
LGATE 1 9 6.19e-9
LSOURCE 3 7 2.18e-9
MMED 16 6 8 8 MMEDMOD
MSTRO 16 6 8 8 MSTROMOD
MWEAK 16 21 8 8 MWEAKMOD
RBREAK 17 18 RBREAKMOD 1
RDRAIN 50 16 RDRAINMOD 2.00e-2
RGATE 9 20 1.77
RLDRAIN 2 5 10
RLGATE 1 9 26
RLSOURCE 3 7 11
RSLC1 5 51 RSLCMOD 1e-6
RSLC2 5 50 1e3
RSOURCE 8 7 RSOURCEMOD 6.5e-3
RVTHRES 22 8 RVTHRESMOD 1
RVTEMP 18 19 RVTEMPMOD 1
S1A 6 12 13 8 S1AMOD
S1B 13 12 13 8 S1BMOD
S2A 6 15 14 13 S2AMOD
S2B 13 15 14 13 S2BMOD
VBAT 22 19 DC 1
ESLC 51 50 VALUE=\{(V(5,51)/ABS(V(5,51)))*(PWR(V(5,51)/(1e-6*71),3.5))\}
.MODEL DBODYMOD D (IS = 1.20e-12 RS = 4.2e-3 XTI = 5 TRS1 = 1.3e-3 TRS2 = 8.0e-6 CJO = 1.50e-9 TT = 7.47e-8 M = 0.63)
.MODEL DBREAKMOD D (RS = 4.2e-1 TRS1 = 8e-4 TRS2 = 3e-6)
.MODEL DPLCAPMOD D (CJO = 1.45e-9 IS = 1e-30 M = 0.82)
.MODEL MMEDMOD NMOS (VTO = 3.11 KP = 5 IS = 1e-30 N = 10 TOX = 1 L = 1u W = 1u RG = 1.77)
.MODEL MSTROMOD NMOS (VTO = 3.57 KP = 33.5 IS = 1e-30 N = 10 TOX = 1 L = 1u W = 1u)
.MODEL MWEAKMOD NMOS (VTO = 2.68 KP = 0.09 IS = 1e-30 N = 10 TOX = 1 L = 1u W = 1u RG = 17.7 )
.MODEL RBREAKMOD RES (TC1 =1.05e-3 TC2 = -5e-7)
.MODEL RDRAINMOD RES (TC1 = 9.40e-3 TC2 = 2.93e-5)
.MODEL RSLCMOD RES (TC1 = 3.5e-3 TC2 = 2.0e-6)
.MODEL RSOURCEMOD RES (TC1 = 1e-3 TC2 = 1e-6)
.MODEL RVTHRESMOD RES (TC1 = -1.8e-3 TC2 = -8.6e-6)
.MODEL RVTEMPMOD RES (TC1 = -3.0e-3 TC2 =1.5e-7)

* .MODEL S1AMOD VSWITCH (RON = 1e-5 ROFF = 0.1 VON = -6.2 VOFF= -3.1)
* .MODEL S1BMOD VSWITCH (RON = 1e-5 ROFF = 0.1 VON = -3.1 VOFF= -6.2)
* .MODEL S2AMOD VSWITCH (RON = 1e-5 ROFF = 0.1 VON = -1.0 VOFF= 0.5)
* .MODEL S2BMOD VSWITCH (RON = 1e-5 ROFF = 0.1 VON = 0.5 VOFF= -1.0)


.MODEL S1AMOD sw RON = 0.1 ROFF = 1e-5 VT = -3.1 VH= 3.1
.MODEL S1BMOD sw RON = 1e-5 ROFF = 0.1 VT = -3.1 VH= 3.1
.MODEL S2AMOD sw RON = 0.1 ROFF = 1e-5 VT = 0.5 VH= 1.5
.MODEL S2BMOD sw RON = 1e-5 ROFF = 0.1 VT = 0.5 VH= 1.5
.ENDS
"
}
C {lab_pin.sym} 370 -100 0 0 {name=l3 lab=0}
C {ind.sym} 220 -270 3 0 {name=l8 value="50u ic=5"
hide_texts=true
attach=l8}
C {res.sym} 310 -270 3 0 {name=Rind1 value=0.05 footprint=1206 device=resistor m=1
hide_texts=true
attach=Rind1}
C {ammeter.sym} 370 -210 0 1 {name=vm3}
C {lab_wire.sym} 300 -150 0 1 {name=l4 lab=G1}
C {buf_ngspice.sym} 250 -150 0 0 {name=x1 RUP=20 RDOWN=20}
C {lab_wire.sym} 260 -270 0 1 {name=l5 lab=L1}
