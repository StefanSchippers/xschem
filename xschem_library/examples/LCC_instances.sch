v {xschem version=2.9.7 file_version=1.2}
G {}
K {}
V {}
S {
}
E {}
L 4 250 -340 270 -360 {}
L 4 250 -340 290 -340 {}
L 4 270 -360 290 -340 {}
L 4 270 -260 290 -280 {}
L 4 250 -280 290 -280 {}
L 4 250 -280 270 -260 {}
L 4 270 -340 270 -280 {}
L 4 350 -620 370 -600 {dash=3}
L 4 370 -640 370 -600 {dash=3}
L 4 350 -620 370 -640 {dash=3}
L 4 370 -620 530 -620 {dash=3}
L 4 1400 -290 1420 -310 {dash=3}
L 4 1400 -330 1400 -290 {dash=3}
L 4 1400 -330 1420 -310 {dash=3}
L 4 1240 -310 1400 -310 {dash=3}
P 4 7 530 -860 1130 -860 1130 -690 1390 -690 1390 -580 530 -580 530 -860 {dash=3}
T {These 2 instances are equivalent} 290 -320 0 0 0.4 0.4 {}
T {Example of using a schematic as a
component instance instead of the 
usual symbol.
LCC: Local Custom Cell.

LCC schematic instantiation show actual parameters
in the schematic instance.} 550 -840 0 0 0.6 0.6 {}
T {LCC schematics can be nested
If only .sch is used there is
no need for a .sym file at all} 910 -430 0 0 0.6 0.6 {}
N 410 -140 410 -120 {lab=HALF}
N 410 -230 430 -230 {lab=ZZ}
N 410 -230 410 -200 {lab=ZZ}
N 420 -440 420 -420 {lab=HALF}
N 420 -530 700 -530 {lab=Z}
N 420 -530 420 -500 {lab=Z}
N 700 -530 700 -240 {lab=Z}
N 700 -240 1450 -240 {lab=Z}
N 320 -230 410 -230 {lab=ZZ}
N 330 -530 420 -530 {lab=Z}
C {vsource.sym} 50 -140 0 0 {name=V1 value="pwl 0 0 1u 0 5u 3"}
C {lab_pin.sym} 50 -170 0 0 {name=p4 lab=AA}
C {lab_pin.sym} 50 -110 0 0 {name=p5 lab=0}
C {code_shown.sym} 580 -150 0 0 {name=STIMULI
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value="

.tran 10n 10u uic
.save all
"}
C {code.sym} 760 -170 0 0 {name=MODEL
only_toplevel="true"
tclcommand="xschem edit_vi_prop"
value="************************************************
*         NOMINAL N-Channel Transistor         *
*            UCB-3 Parameter Set               *
*         HIGH-SPEED CMOS Logic Family         *
*                10-Jan.-1995                  *
************************************************
.Model N NMOS (
+LEVEL = 3
+KP    = 45.3E-6
+VTO   = 0.72
+TOX   = 51.5E-9
+NSUB  = 2.8E15
+GAMMA = 0.94
+PHI   = 0.65
+VMAX  = 150E3
+RS    = 40
+RD    = 40
+XJ    = 0.11E-6
+LD    = 0.52E-6
+DELTA = 0.315
+THETA = 0.054
+ETA   = 0.025
+KAPPA = 0.0
+WD    = 0.0 )

***********************************************
*        NOMINAL P-Channel transistor         *
*           UCB-3 Parameter Set               *
*         HIGH-SPEED CMOS Logic Family        *
*               10-Jan.-1995                  *
***********************************************
.Model P PMOS (
+LEVEL = 3
+KP    = 22.1E-6
+VTO   = -0.71
+TOX   = 51.5E-9
+NSUB  = 3.3E16
+GAMMA = 0.92
+PHI   = 0.65
+VMAX  = 970E3
+RS    = 80
+RD    = 80
+XJ    = 0.63E-6
+LD    = 0.23E-6
+DELTA = 2.24
+THETA = 0.108
+ETA   = 0.322
+KAPPA = 0.0
+WD    = 0.0 )

"}
C {lab_pin.sym} 240 -230 0 0 {name=p6 lab=AA}
C {lab_pin.sym} 430 -230 0 1 {name=p7 lab=ZZ}
C {vsource.sym} 50 -240 0 0 {name=V2 value=3}
C {lab_pin.sym} 50 -270 0 0 {name=p8 lab=VDD}
C {lab_pin.sym} 50 -210 0 0 {name=p9 lab=0}
C {res.sym} 410 -170 0 0 {name=R1
value=20k
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 410 -120 0 0 {name=p10 lab=HALF}
C {vsource.sym} 50 -340 0 0 {name=V3 value=1.5}
C {lab_pin.sym} 50 -370 0 0 {name=p11 lab=HALF}
C {lab_pin.sym} 50 -310 0 0 {name=p12 lab=0}
C {lab_pin.sym} 200 -530 0 0 {name=p13 lab=AA}
C {res.sym} 420 -470 0 0 {name=R2
value=20k
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 420 -420 0 0 {name=p15 lab=HALF}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {cmos_inv.sch} 140 -300 0 0 {name=Xinv WN=15u WP=45u LLN=3u LLP=3u}
C {cmos_inv.sym} 280 -230 0 0 {name=Xinv2 WN=15u WP=45u LLN=3u LLP=3u}
C {bus_keeper.sch} 1200 60 0 0 {name=Xkeeper WN_FB=3u WP_FB=5u}
C {lab_pin.sym} 700 -530 0 1 {name=p1 lab=Z}
