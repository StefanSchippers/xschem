v {xschem version=3.0.0 file_version=1.2 }
G {}
K {}
V {}
S {
}
E {}
L 4 250 -300 270 -320 {}
L 4 250 -300 290 -300 {}
L 4 270 -320 290 -300 {}
L 4 270 -220 290 -240 {}
L 4 250 -240 290 -240 {}
L 4 250 -240 270 -220 {}
L 4 270 -300 270 -240 {}
L 4 350 -580 370 -560 {dash=3}
L 4 370 -600 370 -560 {dash=3}
L 4 350 -580 370 -600 {dash=3}
L 4 370 -580 560 -580 {dash=3}
L 4 1420 -730 1440 -750 {dash=3}
L 4 1420 -770 1420 -730 {dash=3}
L 4 1420 -770 1440 -750 {dash=3}
L 4 1300 -750 1420 -750 {dash=3}
B 2 830 -490 1300 -280 {flags=1 
y1 = -0.0578106
y2 = 3.04806
divy = 6
x1=-0.0713879
x2=3.06704
divx=6
node="v(a) v(zz) v(zzz)"
color="4 6 8"
sweep="v(a)"}
B 2 30 -930 500 -720 {flags=1 
y1 = -0.0578106
y2 = 3.04806
divy = 6
x1=-0.0713879
x2=3.06704
divx=6
node="v(a) v(z)"
color="4 6 8"
sweep="v(z) v(a)"}
P 4 5 560 -700 560 -510 1350 -510 1350 -700 560 -700 {dash=3}
P 4 5 770 -920 770 -730 1300 -730 1300 -920 770 -920 {dash=3}
T {These 2 instances are equivalent} 290 -280 0 0 0.4 0.4 {}
T {Example of using a schematic as a component instance
instead of the usual symbol. LCC: Local Custom Cell.

LCC schematic instantiation show actual parameters
in the schematic instance.} 570 -680 0 0 0.5 0.5 {}
T {LCC schematics can be nested
If only .sch is used there is
no need for a .sym file at all} 790 -880 0 0 0.6 0.6 {}
T {Select one or more graphs (and no other objects)
and use arrow keys to zoom / pan waveforms} 20 -980 0 0 0.3 0.3 {}
T {Butterfly diagram
of a cmos latch} 510 -940 0 0 0.4 0.4 {layer=8}
N 410 -100 410 -80 {lab=HALF}
N 410 -190 430 -190 {lab=ZZZ}
N 410 -190 410 -160 {lab=ZZZ}
N 420 -400 420 -380 {lab=HALF}
N 420 -490 700 -490 {lab=ZZ}
N 420 -490 420 -460 {lab=ZZ}
N 700 -490 700 -240 {lab=ZZ}
N 700 -240 1450 -240 {lab=ZZ}
N 320 -190 410 -190 {lab=ZZZ}
N 330 -490 420 -490 {lab=ZZ}
N 620 -850 620 -760 { lab=Z}
N 540 -850 540 -760 { lab=A}
C {vsource.sym} 50 -140 0 0 {name=V1 value="pwl 0 0 1u 0 5u 3"}
C {lab_pin.sym} 50 -170 0 0 {name=p4 lab=A}
C {lab_pin.sym} 50 -110 0 0 {name=p5 lab=0}
C {code_shown.sym} 520 -190 0 0 {name=STIMULI
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value=".control
dc v1 0 3 0.001
* .tran 10n 10u uic
save all
write LCC_instances.raw

set appendwrite 
dc v1 3 0 -0.001
write LCC_instances.raw
.endc
"}
C {code.sym} 840 -190 0 0 {name=MODEL
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
C {lab_pin.sym} 240 -190 0 0 {name=p6 lab=A}
C {lab_pin.sym} 430 -190 0 1 {name=p7 lab=ZZZ}
C {vsource.sym} 50 -240 0 0 {name=V2 value=3}
C {lab_pin.sym} 50 -270 0 0 {name=p8 lab=VDD}
C {lab_pin.sym} 50 -210 0 0 {name=p9 lab=0}
C {res.sym} 410 -130 0 0 {name=R1
value=20k
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 410 -80 0 0 {name=p10 lab=HALF}
C {vsource.sym} 50 -340 0 0 {name=V3 value=1.5}
C {lab_pin.sym} 50 -370 0 0 {name=p11 lab=HALF}
C {lab_pin.sym} 50 -310 0 0 {name=p12 lab=0}
C {lab_pin.sym} 200 -490 0 0 {name=p13 lab=A}
C {res.sym} 420 -430 0 0 {name=R2
value=20k
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 420 -380 0 0 {name=p15 lab=HALF}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {cmos_inv.sch} 140 -260 0 0 {name=Xinv WN=15u WP=45u LLN=3u LLP=3u}
C {cmos_inv.sym} 280 -190 0 0 {name=Xinv2 WN=15u WP=45u LLN=3u LLP=3u}
C {bus_keeper.sch} 1200 60 0 0 {name=Xkeeper WN_FB=3u WP_FB=5u}
C {lab_pin.sym} 700 -490 0 1 {name=p1 lab=ZZ}
C {lab_pin.sym} 540 -760 0 0 {name=p14 lab=A}
C {cmos_inv.sym} 580 -850 0 1 {name=Xinv3 WN=3u WP=5u LLN=3u LLP=3u}
C {lab_pin.sym} 620 -760 0 1 {name=p2 lab=Z}
C {cmos_inv.sym} 580 -760 0 0 {name=Xinv1 WN=3u WP=5u LLN=3u LLP=3u}
C {launcher.sym} 85 -1015 0 0 {name=h1 
descr="Select arrow and 
Ctrl-Left-Click to load/unload waveforms" 
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw
"
}
