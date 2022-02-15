v {xschem version=3.0.0 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
L 3 290 -1250 630 -1250 {}
L 3 1160 -1250 1600 -1250 {}
L 4 525 -210 530 -250 {}
L 4 530 -250 535 -210 {}
L 4 340 -230 342.5 -242.5 {}
L 4 342.5 -242.5 347.5 -250 {}
L 4 347.5 -250 352.5 -250 {}
L 4 352.5 -250 357.5 -242.5 {}
L 4 357.5 -242.5 360 -230 {}
L 4 377.5 -217.5 380 -230 {}
L 4 372.5 -210 377.5 -217.5 {}
L 4 367.5 -210 372.5 -210 {}
L 4 362.5 -217.5 367.5 -210 {}
L 4 360 -230 362.5 -217.5 {}
L 4 380 -230 382.5 -242.5 {}
L 4 382.5 -242.5 387.5 -250 {}
L 4 387.5 -250 392.5 -250 {}
L 4 392.5 -250 397.5 -242.5 {}
L 4 397.5 -242.5 400 -230 {}
L 4 20 -860 20 -760 {dash=1}
L 4 10 -770 140 -770 {dash=1}
L 4 80 -860 80 -770 {dash=2}
L 4 20 -840 110 -840 {dash=2}
L 4 20 -1180 20 -1080 {dash=1}
L 4 10 -1090 140 -1090 {dash=1}
L 4 80 -1180 80 -1090 {dash=2}
L 4 20 -1160 110 -1160 {dash=2}
L 4 535 -210 540 -250 {}
L 4 540 -250 545 -210 {}
L 4 545 -210 550 -250 {}
L 4 550 -250 555 -210 {}
L 4 555 -210 560 -250 {}
L 4 560 -250 565 -210 {}
L 4 565 -210 570 -250 {}
L 4 570 -250 575 -210 {}
L 4 575 -210 580 -250 {}
L 4 580 -250 585 -210 {}
L 4 585 -210 590 -250 {}
L 4 590 -250 595 -210 {}
L 4 595 -210 600 -250 {}
L 4 600 -250 605 -210 {}
L 7 20 -1090 60 -1090 {}
L 7 60 -1090 70 -1095 {}
L 7 70 -1095 90 -1155 {}
L 7 100 -1160 140 -1160 {}
L 7 90 -1155 100 -1160 {}
L 7 20 -840 60 -840 {}
L 7 60 -840 70 -835 {}
L 7 70 -835 90 -775 {}
L 7 100 -770 140 -770 {}
L 7 90 -775 100 -770 {}
B 2 1100 -690 2050 -340 {flags=graph
y1=-41
y2=41
ypos1=2.19058
ypos2=36.4765
divy=5
subdivy=1
unity=1
x1=0.00124125
x2=0.00150969
divx=5
subdivx=1
node="out
out_lpf
\\"Expected;in 5 - 9 *\\""
color="7 8 6"
dataset=0
unitx=u
digital=0}
B 2 1100 -330 2050 -90 {flags=graph
y1=0
y2=12
ypos1=0.0476856
ypos2=6.80536
divy=5
subdivy=1
unity=1
x1=0.00124125
x2=0.00150969
divx=5
subdivx=1


dataset=0
unitx=u
digital=1
color="7 4 4 4"
node="\\"out;out 8 / 5 +\\"
\\"Gate Up;gu out -\\"
\\"Gate Down;gd vnn -\\"
drive"}
B 2 1100 -1040 2050 -710 {flags=graph
y1=0.00051
y2=10
ypos1=-3.90706
ypos2=14.7089
divy=5
subdivy=1
unity=1
x1=0.00124125
x2=0.00150969
divx=5
subdivx=1


dataset=0
unitx=u
digital=0

color="6 7"
node="ref
in"}
T {PWM Modulator} 470 -480 0 0 0.6 0.6 {}
T {Driver} 230 -930 0 0 0.6 0.6 {}
T {LP
Filter} 700 -1040 0 0 0.6 0.6 {}
T {Load} 860 -1010 0 0 0.6 0.6 {}
T {VREF} 65 -765 0 0 0.2 0.2 {layer=4}
T {10} 5 -845 0 0 0.2 0.2 {}
T {VREF} 65 -1085 0 0 0.2 0.2 {layer=4}
T {10} 5 -1165 0 0 0.2 0.2 {}
T {DRIVE} 125 -765 0 0 0.2 0.2 {}
T {DRIVE} 125 -1085 0 0 0.2 0.2 {}
T {Class D amplifier } 670 -1280 0 0 1 1 {}
N 460 -1020 460 -1000 {lab=#net1}
N 460 -920 460 -900 {lab=OUT}
N 460 -840 460 -800 {lab=#net2}
N 460 -1060 460 -1020 {lab=#net1}
N 460 -700 460 -640 {lab=VNN}
N 460 -940 460 -920 {lab=OUT}
N 460 -1160 460 -1120 { lab=VPP}
N 570 -920 680 -920 {
lab=OUT}
N 220 -1020 460 -1020 {
lab=#net1}
N 340 -1090 420 -1090 {
lab=GU_I}
N 220 -700 460 -700 {
lab=VNN}
N 340 -770 420 -770 {
lab=GD_I}
N 220 -770 220 -760 {
lab=GD}
N 610 -370 710 -370 {
lab=DRIVE}
N 220 -1090 220 -1080 {
lab=GU}
N 70 -510 70 -490 {
lab=VSS}
N 70 -590 70 -570 {
lab=VPP}
N 190 -510 190 -490 {
lab=VSS}
N 190 -590 190 -570 {
lab=VNN}
N 320 -510 320 -490 {
lab=0}
N 320 -590 320 -570 {
lab=VSS}
N 570 -920 570 -870 {
lab=OUT}
N 570 -810 570 -790 {
lab=VSS}
N 220 -1090 280 -1090 {
lab=GU}
N 220 -770 280 -770 {
lab=GD}
N 490 -150 490 -130 {
lab=VSS}
N 310 -150 310 -130 {
lab=VSS}
N 780 -920 780 -890 {
lab=OUT_LPF}
N 740 -920 780 -920 {
lab=OUT_LPF}
N 460 -740 460 -700 {lab=VNN}
N 460 -920 570 -920 {
lab=OUT}
N 780 -830 780 -810 {
lab=VSS}
N 900 -830 900 -810 {
lab=VSS}
N 900 -920 900 -890 {
lab=OUT_LPF}
N 900 -920 940 -920 {
lab=OUT_LPF}
N 780 -920 900 -920 {
lab=OUT_LPF}
N 490 -340 490 -210 {
lab=REF}
N 310 -400 490 -400 {
lab=IN}
N 310 -400 310 -210 {
lab=IN}
C {nmos3.sym} 440 -770 0 0 {name=xm2 model=irf540 m=1
program=evince
url="https://www.vishay.com/docs/91021/irf540.pdf"
net_name=true

device_model=".SUBCKT irf540 1 2 3
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
"}
C {nmos3.sym} 440 -1090 0 0 {name=xm1 model=irf540 m=1
program=evince
url="https://www.vishay.com/docs/91021/irf540.pdf"
net_name=true

device_model=".SUBCKT irf540 1 2 3
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
"}
C {lab_pin.sym} 460 -640 0 0 {name=p18 lab=VNN}
C {ammeter.sym} 460 -870 0 0 {name=vd  net_name=true current=0.2069}
C {ammeter.sym} 460 -970 0 0 {name=vu  net_name=true current=0.2005}
C {lab_pin.sym} 460 -1160 0 0 {name=p27 lab=VPP}
C {lab_pin.sym} 460 -910 0 0 {name=p3 lab=OUT}
C {comp_ngspice.sym} 550 -370 0 0 {name=x1 VCC=VCC}
C {bsource.sym} 220 -730 0 1 {name=B1 VAR=V FUNC="'10/2*(1+tanh(V(REF,DRIVE)*20))'"
}
C {lab_pin.sym} 710 -370 0 1 {name=p6 lab=DRIVE}
C {bsource.sym} 220 -1050 0 1 {name=B2 VAR=V FUNC="'10/2*(1+tanh(V(DRIVE,REF)*20))'"
}
C {code_shown.sym} 20 -420 0 0 {name=CONTROL value="
.param VCC=10
.control
save all
tran 100n 2000u uic
write classD_amp.raw
.endc

" net_name=true}
C {title.sym} 160 -40 0 0 {name=l1 author="Stefan Schippers" net_name=true}
C {vsource.sym} 70 -540 0 0 {name=V3 value=40}
C {lab_pin.sym} 70 -490 0 0 {name=p9 lab=VSS}
C {lab_pin.sym} 70 -590 0 0 {name=p10 lab=VPP}
C {vsource.sym} 190 -540 0 0 {name=V4 value=-40}
C {lab_pin.sym} 190 -490 0 0 {name=p11 lab=VSS}
C {lab_pin.sym} 190 -590 0 0 {name=p12 lab=VNN}
C {vsource.sym} 320 -540 0 0 {name=V5 value=0}
C {lab_pin.sym} 320 -490 0 0 {name=p13 lab=0}
C {lab_pin.sym} 320 -590 0 0 {name=p14 lab=VSS}
C {lab_pin.sym} 220 -1090 0 0 {name=p15 lab=GU}
C {lab_pin.sym} 220 -770 0 0 {name=p16 lab=GD}
C {res.sym} 570 -840 0 1 {name=R1 m=1 value=8}
C {lab_pin.sym} 570 -790 0 0 {name=p17 lab=VSS}
C {launcher.sym} 1160 -1080 0 0 {name=h5 
descr="Select arrow and 
Ctrl-Left-Click to load/unload waveforms" 
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw
"
}
C {res.sym} 310 -1090 1 1 {name=R2 m=1 value=2}
C {res.sym} 310 -770 1 1 {name=R3 m=1 value=2}
C {parax_cap.sym} 370 -1080 0 0 {name=C3 gnd=0 value=20f m=1}
C {parax_cap.sym} 370 -760 0 0 {name=C1 gnd=0 value=20f m=1}
C {parax_cap.sym} 610 -910 0 0 {name=C2 gnd=0 value=1u m=1}
C {lab_wire.sym} 410 -1090 0 0 {name=l19 lab=GU_I}
C {lab_wire.sym} 410 -770 0 0 {name=l2 lab=GD_I}
C {vsource.sym} 490 -180 0 0 {name=V6 value="pulse 0 VCC 0 5u 5u 5p 10u"}
C {lab_pin.sym} 490 -130 0 0 {name=p19 lab=VSS}
C {lab_pin.sym} 490 -230 0 0 {name=p20 lab=REF}
C {vsource.sym} 310 -180 0 0 {name=V7 value="SIN ( 5 4 2000 0 0 0 )"
}
C {lab_pin.sym} 310 -130 0 0 {name=p21 lab=VSS}
C {lab_pin.sym} 310 -230 0 0 {name=p22 lab=IN}
C {parax_cap.sym} 670 -360 0 0 {name=C4 gnd=0 value=200f m=1}
C {ind.sym} 710 -920 1 0 {name=L1
m=1
value=0.03m
footprint=1206
device=inductor}
C {capa.sym} 780 -860 0 0 {name=C5
m=1
value=20u
footprint=1206
device="ceramic capacitor"}
C {lab_pin.sym} 940 -920 0 1 {name=p4 lab=OUT_LPF}
C {lab_pin.sym} 780 -810 0 0 {name=p7 lab=VSS}
C {res.sym} 900 -860 0 1 {name=R4 m=1 value=8}
C {lab_pin.sym} 900 -810 0 0 {name=p8 lab=VSS}
C {launcher.sym} 1160 -1140 0 0 {name=h1 
descr="Netlist + Simulate
Ctrl-Left-Click" 
tclcommand="xschem netlist; xschem simulate"
}
