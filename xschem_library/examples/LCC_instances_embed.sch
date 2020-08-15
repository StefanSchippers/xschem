v {xschem version=2.9.7 file_version=1.2}
G {}
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
L 4 350 -620 370 -600 {}
L 4 370 -640 370 -600 {}
L 4 350 -620 370 -640 {}
L 4 370 -620 530 -620 {}
L 4 1400 -290 1420 -310 {}
L 4 1400 -330 1400 -290 {}
L 4 1400 -330 1420 -310 {}
L 4 1240 -310 1400 -310 {}
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
C {vsource.sym} 50 -140 0 0 {name=V1 value="pwl 0 0 1u 0 5u 3" embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=vsource
format="@name @pinlist @value"
template="name=V1 value=3"}
V {}
S {}
E {}
L 4 2.5 -22.5 7.5 -22.5 {}
L 4 5 -25 5 -20 {}
L 4 -0 -30 -0 30 {}
B 5 -2.5 -32.5 2.5 -27.5 {name=p dir=inout}
B 5 -2.5 27.5 2.5 32.5 {name=m dir=inout}
A 4 0 0 15 270 360 {}
T {@value} 20 0 0 0 0.2 0.2 {}
T {@name} 20 -17.5 0 0 0.2 0.2 {}
]
C {lab_pin.sym} 50 -170 0 0 {name=p4 lab=AA embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=label
format="*.alias @lab"
template="name=l1 sig_type=std_logic lab=xxx"}
V {}
S {}
E {}
B 5 -1.25 -1.25 1.25 1.25 {name=p dir=in}
T {@lab} -7.5 -8.125 0 1 0.33 0.33 {}
]
C {lab_pin.sym} 50 -110 0 0 {name=p5 lab=0 embed=true}
C {code_shown.sym} 580 -150 0 0 {name=STIMULI
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value="

.tran 10n 10u uic
.save all
" embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=netlist_commands
template="name=s1 only_toplevel=false value=blabla"
format="
@value
"}
V {}
S {}
E {}
L 4 0 -10 70 -10 {}
L 4 0 -10 0 10 {}
T {@name} 5 -25 0 0 0.3 0.3 {}
T {@value} 15 -5 0 0 0.3 0.3 {}
]
C {code.sym} 760 -170 0 0 {name=MODEL
only_toplevel=true
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

" embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=netlist_commands
template="name=s1 only_toplevel=false value=blabla"
format="
@value
"}
V {}
S {}
E {}
L 4 20 30 60 30 {}
L 4 20 40 40 40 {}
L 4 20 50 60 50 {}
L 4 20 60 50 60 {}
L 4 20 70 50 70 {}
L 4 20 80 90 80 {}
L 4 20 90 40 90 {}
L 4 20 20 70 20 {}
L 4 20 10 40 10 {}
L 4 100 10 110 10 {}
L 4 110 10 110 110 {}
L 4 20 110 110 110 {}
L 4 20 100 20 110 {}
L 4 100 0 100 100 {}
L 4 10 100 100 100 {}
L 4 10 0 10 100 {}
L 4 10 0 100 0 {}
T {@name} 15 -25 0 0 0.3 0.3 {}
]
C {lab_pin.sym} 240 -230 0 0 {name=p6 lab=AA embed=true}
C {lab_pin.sym} 430 -230 0 1 {name=p7 lab=ZZ embed=true}
C {vsource.sym} 50 -240 0 0 {name=V2 value=3 embed=true}
C {lab_pin.sym} 50 -270 0 0 {name=p8 lab=VDD embed=true}
C {lab_pin.sym} 50 -210 0 0 {name=p9 lab=0 embed=true}
C {res.sym} 410 -170 0 0 {name=R1
value=20k
footprint=1206
device=resistor
m=1 embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=resistor
format="@name @pinlist @value m=@m"
verilog_format="tran @name (@@P\\\\, @@M\\\\);"
tedax_format="footprint @name @footprint
value @name @value
device @name @device
@comptag"
template="name=R1
value=1k
footprint=1206
device=resistor
m=1"
}
V {}
S {}
E {}
L 4 0 20 0 30 {}
L 4 0 20 7.5 17.5 {}
L 4 -7.5 12.5 7.5 17.5 {}
L 4 -7.5 12.5 7.5 7.5 {}
L 4 -7.5 2.5 7.5 7.5 {}
L 4 -7.5 2.5 7.5 -2.5 {}
L 4 -7.5 -7.5 7.5 -2.5 {}
L 4 -7.5 -7.5 7.5 -12.5 {}
L 4 -7.5 -17.5 7.5 -12.5 {}
L 4 -7.5 -17.5 0 -20 {}
L 4 0 -30 0 -20 {}
L 4 2.5 -22.5 7.5 -22.5 {}
L 4 5 -25 5 -20 {}
B 5 -2.5 -32.5 2.5 -27.5 {name=P dir=inout propagate_to=1 pinnumber=1}
B 5 -2.5 27.5 2.5 32.5 {name=M dir=inout propagate_to=0 pinnumber=2}
T {@name} 15 -13.75 0 0 0.2 0.2 {}
T {@value} 15 1.25 0 0 0.2 0.2 {}
T {@#0:pinnumber} -10 -26.25 0 1 0.2 0.2 {layer=13}
T {@#1:pinnumber} -10 16.25 0 1 0.2 0.2 {layer=13}
]
C {lab_pin.sym} 410 -120 0 0 {name=p10 lab=HALF embed=true}
C {vsource.sym} 50 -340 0 0 {name=V3 value=1.5 embed=true}
C {lab_pin.sym} 50 -370 0 0 {name=p11 lab=HALF embed=true}
C {lab_pin.sym} 50 -310 0 0 {name=p12 lab=0 embed=true}
C {lab_pin.sym} 200 -530 0 0 {name=p13 lab=AA embed=true}
C {res.sym} 420 -470 0 0 {name=R2
value=20k
footprint=1206
device=resistor
m=1 embed=true}
C {lab_pin.sym} 420 -420 0 0 {name=p15 lab=HALF embed=true}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers" embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=logo
template="name=l1 author=\\"Stefan Schippers\\""
verilog_ignore=true
vhdl_ignore=true
spice_ignore=true
tedax_ignore=true}
V {}
S {}
E {}
L 6 225 0 1020 0 {}
L 6 -160 0 -95 0 {}
T {@schname} 235 5 0 0 0.4 0.4 {}
T {@author} 235 -25 0 0 0.4 0.4 {}
T {@time_last_modified} 1020 -20 0 1 0.4 0.3 {}
T {SCHEM} 5 -25 0 0 1 1 {}
P 5 13 5 -30 -25 0 5 30 -15 30 -35 10 -55 30 -75 30 -45 0 -75 -30 -55 -30 -35 -10 -15 -30 5 -30 {fill=true}
]
C {cmos_inv.sch} 140 -300 0 0 {name=Xinv WN=15u WP=45u LLN=3u LLP=3u embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=subcircuit
format="@name @pinlist @symname WN=@WN WP=@WP LLN=@LLN LLP=@LLP m=@m"
template="name=X1 WN=15u WP=45u LLN=3u LLP=3u m=1"
}
V {}
S {}
E {}
L 1 140 -260 140 -200 {lab=Z}
L 1 100 -290 100 -170 {lab=A}
L 1 60 -230 100 -230 {lab=A}
L 1 140 -230 190 -230 {lab=Z}
L 1 140 -340 140 -320 {lab=VDD}
L 1 140 -140 140 -120 {lab=0}
L 4 125 -200 125 -140 {}
L 4 125 -190 140 -190 {}
L 4 140 -200 140 -190 {}
L 4 125 -150 140 -150 {}
L 4 140 -150 140 -140 {}
L 4 115 -185 115 -155 {}
L 4 115 -170 115 -165 {}
L 4 100 -170 107.5 -170 {}
L 4 100 -170 115 -170 {}
L 4 130 -170 140 -170 {}
L 4 125 -175 130 -170 {}
L 4 125 -165 130 -170 {}
L 4 125 -320 125 -260 {11}
L 4 125 -270 140 -270 {}
L 4 140 -270 140 -260 {}
L 4 125 -310 140 -310 {}
L 4 140 -320 140 -310 {}
L 4 115 -305 115 -275 {}
L 4 115 -295 115 -290 {}
L 4 112.5 -295 115 -292.5 {}
L 4 110 -295 112.5 -295 {}
L 4 107.5 -292.5 110 -295 {}
L 4 107.5 -292.5 107.5 -287.5 {}
L 4 107.5 -287.5 110 -285 {}
L 4 110 -285 112.5 -285 {}
L 4 112.5 -285 115 -287.5 {}
L 4 100 -290 107.5 -290 {}
L 4 130 -290 140 -290 {}
L 4 125 -295 130 -290 {}
L 4 125 -285 130 -290 {}
L 4 140 -360 140 -340 {}
L 4 130 -360 150 -360 {}
L 7 70 -232.5 72.5 -230 {}
L 7 70 -227.5 72.5 -230 {}
L 7 177.5 -232.5 180 -230 {}
L 7 177.5 -227.5 180 -230 {}
B 5 57.5 -232.5 62.5 -227.5 {name=A dir=in  }
B 5 187.5 -232.5 192.5 -227.5 {name=Z dir=out  }
B 7 137.5 -202.5 142.5 -197.5 {name=d dir=inout}
B 7 97.5 -172.5 102.5 -167.5 {name=g dir=in}
B 7 137.5 -142.5 142.5 -137.5 {name=s dir=inout}
B 7 137.5 -172.5 142.5 -167.5 {name=b dir=in}
B 7 137.5 -262.5 142.5 -257.5 {name=d dir=inout}
B 7 97.5 -292.5 102.5 -287.5 {name=g dir=in}
B 7 137.5 -322.5 142.5 -317.5 {name=s dir=inout}
B 7 137.5 -292.5 142.5 -287.5 {name=b dir=in}
B 7 137.5 -342.5 142.5 -337.5 {name=p dir=inout verilog_type=wire}
B 7 138.75 -121.25 141.25 -118.75 {name=p dir=in}
B 7 138.75 -171.25 141.25 -168.75 {name=p dir=in}
B 7 138.75 -291.25 141.25 -288.75 {name=p dir=in}
T {@name} 60 -405 0 0 0.2 0.2 {}
T {@symname} 63.75 -85 0 0 0.2 0.2 {}
T {$WN\\/$LLN\\/$1} 127.5 -188.75 0 0 0.2 0.2 {}
T {$M1} 127.5 -163.75 0 0 0.2 0.2 {}
T {D} 145 -197.5 0 0 0.15 0.15 {}
T {$WP\\/$LLP\\/$1} 127.5 -307.5 0 0 0.2 0.2 {}
T {$M2} 127.5 -283.75 0 0 0.2 0.2 {}
T {D} 145 -270 0 0 0.15 0.15 {}
T {$VDD} 127.5 -375 0 0 0.2 0.2 {}
T {$0} 132.5 -128.125 0 1 0.33 0.33 {}
T {$A} 63.75 -246.25 0 0 0.2 0.2 {}
T {$Z} 186.25 -246.25 0 1 0.2 0.2 {}
T {$0} 147.5 -178.125 0 0 0.33 0.33 {}
T {$VDD} 147.5 -298.125 0 0 0.33 0.33 {}
P 4 5 60 -390 60 -90 190 -90 190 -390 60 -390 {}
]
C {cmos_inv.sym} 280 -230 0 0 {name=Xinv2 WN=15u WP=45u LLN=3u LLP=3u embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=subcircuit
format="@name @pinlist @symname WN=@WN WP=@WP LLN=@LLN LLP=@LLP m=@m"
verilog_primitive=true
verilog_format="assign #80 @@Z = ~ @@A ;"
template="name=X1 WN=15u WP=45u LLN=3u LLP=3u m=1"
}
V {}
S {}
E {}
L 4 -40 0 -27.5 0 {}
L 4 -27.5 -20 -27.5 20 {}
L 4 -27.5 -20 16.25 0 {}
L 4 -27.5 20 16.25 0 {}
L 4 26.25 0 40 0 {}
B 5 -42.5 -2.5 -37.5 2.5 {name=A dir=in }
B 5 37.5 -2.5 42.5 2.5 {name=Z dir=out }
A 4 21.25 -0 5 180 360 {}
T {@name} -26.25 -5 0 0 0.2 0.2 {}
T {@symname} -26.25 35 0 0 0.2 0.2 {}
T {@WP\\/@LLP\\/@m} -16.25 -25 0 0 0.2 0.2 {}
T {@WN\\/@LLN\\/@m} -16.25 15 0 0 0.2 0.2 {}
]
C {bus_keeper.sch} 1200 60 0 0 {name=Xkeeper WN_FB=3u WP_FB=5u embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=subcircuit
format="@name @pinlist @symname WN_FB=@WN_FB WP_FB=@WP_FB"
template="name=X1 WN_FB=1u WP_FB=2u"}
V {}
S {}
E {}
L 1 250 -300 300 -300 {lab=A}
L 1 280 -680 300 -680 {lab=A}
L 1 280 -680 280 -300 {lab=A}
L 1 430 -300 450 -300 {lab=#net1}
L 1 450 -680 450 -300 {lab=#net1}
L 1 430 -680 450 -680 {lab=#net1}
L 1 380 -330 380 -270 {lab=Z}
L 1 340 -360 340 -240 {lab=A}
L 1 300 -300 340 -300 {lab=A}
L 1 380 -300 430 -300 {lab=Z}
L 1 380 -410 380 -390 {lab=VDD}
L 1 380 -210 380 -190 {lab=0}
L 1 350 -710 350 -650 {lab=Z}
L 1 390 -740 390 -620 {lab=A}
L 1 390 -680 430 -680 {lab=A}
L 1 300 -680 350 -680 {lab=Z}
L 1 350 -790 350 -770 {lab=VDD}
L 1 350 -590 350 -570 {lab=0}
L 4 365 -270 365 -210 {}
L 4 365 -260 380 -260 {}
L 4 380 -270 380 -260 {}
L 4 365 -220 380 -220 {}
L 4 380 -220 380 -210 {}
L 4 355 -255 355 -225 {}
L 4 355 -240 355 -235 {}
L 4 340 -240 347.5 -240 {}
L 4 340 -240 355 -240 {}
L 4 370 -240 380 -240 {}
L 4 365 -245 370 -240 {}
L 4 365 -235 370 -240 {}
L 4 365 -390 365 -330 {11}
L 4 365 -340 380 -340 {}
L 4 380 -340 380 -330 {}
L 4 365 -380 380 -380 {}
L 4 380 -390 380 -380 {}
L 4 355 -375 355 -345 {}
L 4 355 -365 355 -360 {}
L 4 352.5 -365 355 -362.5 {}
L 4 350 -365 352.5 -365 {}
L 4 347.5 -362.5 350 -365 {}
L 4 347.5 -362.5 347.5 -357.5 {}
L 4 347.5 -357.5 350 -355 {}
L 4 350 -355 352.5 -355 {}
L 4 352.5 -355 355 -357.5 {}
L 4 340 -360 347.5 -360 {}
L 4 370 -360 380 -360 {}
L 4 365 -365 370 -360 {}
L 4 365 -355 370 -360 {}
L 4 380 -430 380 -410 {}
L 4 370 -430 390 -430 {}
L 4 365 -650 365 -590 {}
L 4 350 -640 365 -640 {}
L 4 350 -650 350 -640 {}
L 4 350 -600 365 -600 {}
L 4 350 -600 350 -590 {}
L 4 375 -635 375 -605 {}
L 4 375 -620 375 -615 {}
L 4 382.5 -620 390 -620 {}
L 4 375 -620 390 -620 {}
L 4 350 -620 360 -620 {}
L 4 360 -620 365 -625 {}
L 4 360 -620 365 -615 {}
L 4 365 -770 365 -710 {11}
L 4 350 -720 365 -720 {}
L 4 350 -720 350 -710 {}
L 4 350 -760 365 -760 {}
L 4 350 -770 350 -760 {}
L 4 375 -755 375 -725 {}
L 4 375 -745 375 -740 {}
L 4 375 -742.5 377.5 -745 {}
L 4 377.5 -745 380 -745 {}
L 4 380 -745 382.5 -742.5 {}
L 4 382.5 -742.5 382.5 -737.5 {}
L 4 380 -735 382.5 -737.5 {}
L 4 377.5 -735 380 -735 {}
L 4 375 -737.5 377.5 -735 {}
L 4 382.5 -740 390 -740 {}
L 4 350 -740 360 -740 {}
L 4 360 -740 365 -745 {}
L 4 360 -740 365 -735 {}
L 4 350 -810 350 -790 {}
L 4 340 -810 360 -810 {}
L 7 310 -302.5 312.5 -300 {}
L 7 310 -297.5 312.5 -300 {}
L 7 417.5 -302.5 420 -300 {}
L 7 417.5 -297.5 420 -300 {}
L 7 417.5 -680 420 -682.5 {}
L 7 417.5 -680 420 -677.5 {}
L 7 310 -680 312.5 -682.5 {}
L 7 310 -680 312.5 -677.5 {}
L 7 257.5 -300 260 -302.5 {}
L 7 257.5 -300 260 -297.5 {}
L 7 260 -302.5 262.5 -300 {}
L 7 260 -297.5 262.5 -300 {}
B 5 247.5 -302.5 252.5 -297.5 {name=A dir=inout  }
B 7 377.5 -272.5 382.5 -267.5 {name=d dir=inout}
B 7 337.5 -242.5 342.5 -237.5 {name=g dir=in}
B 7 377.5 -212.5 382.5 -207.5 {name=s dir=inout}
B 7 377.5 -242.5 382.5 -237.5 {name=b dir=in}
B 7 377.5 -332.5 382.5 -327.5 {name=d dir=inout}
B 7 337.5 -362.5 342.5 -357.5 {name=g dir=in}
B 7 377.5 -392.5 382.5 -387.5 {name=s dir=inout}
B 7 377.5 -362.5 382.5 -357.5 {name=b dir=in}
B 7 377.5 -412.5 382.5 -407.5 {name=p dir=inout verilog_type=wire}
B 7 378.75 -191.25 381.25 -188.75 {name=p dir=in}
B 7 297.5 -302.5 302.5 -297.5 {}
B 7 427.5 -302.5 432.5 -297.5 {}
B 7 378.75 -241.25 381.25 -238.75 {name=p dir=in}
B 7 378.75 -361.25 381.25 -358.75 {name=p dir=in}
B 7 347.5 -652.5 352.5 -647.5 {name=d dir=inout}
B 7 387.5 -622.5 392.5 -617.5 {name=g dir=in}
B 7 347.5 -592.5 352.5 -587.5 {name=s dir=inout}
B 7 347.5 -622.5 352.5 -617.5 {name=b dir=in}
B 7 347.5 -712.5 352.5 -707.5 {name=d dir=inout}
B 7 387.5 -742.5 392.5 -737.5 {name=g dir=in}
B 7 347.5 -772.5 352.5 -767.5 {name=s dir=inout}
B 7 347.5 -742.5 352.5 -737.5 {name=b dir=in}
B 7 347.5 -792.5 352.5 -787.5 {name=p dir=inout verilog_type=wire}
B 7 348.75 -571.25 351.25 -568.75 {name=p dir=in}
B 7 427.5 -682.5 432.5 -677.5 {}
B 7 297.5 -682.5 302.5 -677.5 {}
B 7 348.75 -621.25 351.25 -618.75 {name=p dir=in}
B 7 348.75 -741.25 351.25 -738.75 {name=p dir=in}
T {@name} 250 -915 0 0 0.5 0.5 {}
T {@symname} 253.75 -115 0 0 0.5 0.5 {}
T {$X2} 300 -475 0 0 0.2 0.2 {}
T {cmos_inv} 303.75 -155 0 0 0.2 0.2 {}
T {$15u\\/$3u\\/$1} 367.5 -258.75 0 0 0.2 0.2 {}
T {$M1} 367.5 -233.75 0 0 0.2 0.2 {}
T {D} 385 -267.5 0 0 0.15 0.15 {}
T {$45u\\/$3u\\/$1} 367.5 -377.5 0 0 0.2 0.2 {}
T {$M2} 367.5 -353.75 0 0 0.2 0.2 {}
T {D} 385 -340 0 0 0.15 0.15 {}
T {$VDD} 367.5 -445 0 0 0.2 0.2 {}
T {$0} 372.5 -198.125 0 1 0.33 0.33 {}
T {$A} 303.75 -316.25 0 0 0.2 0.2 {}
T {$Z} 426.25 -316.25 0 1 0.2 0.2 {}
T {$0} 387.5 -248.125 0 0 0.33 0.33 {}
T {$VDD} 387.5 -368.125 0 0 0.33 0.33 {}
T {$X1} 430 -855 0 1 0.2 0.2 {}
T {cmos_inv} 426.25 -535 0 1 0.2 0.2 {}
T {$WN_FB\\/$3u\\/$1} 362.5 -638.75 0 1 0.2 0.2 {}
T {$M1} 362.5 -613.75 0 1 0.2 0.2 {}
T {D} 345 -647.5 0 1 0.15 0.15 {}
T {$WP_FB\\/$3u\\/$1} 362.5 -757.5 0 1 0.2 0.2 {}
T {$M2} 362.5 -733.75 0 1 0.2 0.2 {}
T {D} 345 -720 0 1 0.15 0.15 {}
T {$VDD} 362.5 -825 0 1 0.2 0.2 {}
T {$0} 357.5 -578.125 0 0 0.33 0.33 {}
T {$A} 426.25 -696.25 0 1 0.2 0.2 {}
T {$Z} 303.75 -696.25 0 0 0.2 0.2 {}
T {$0} 342.5 -628.125 0 1 0.33 0.33 {}
T {$VDD} 342.5 -748.125 0 1 0.33 0.33 {}
T {$A} 253.75 -316.25 0 0 0.2 0.2 {}
P 4 5 250 -880 250 -120 480 -120 480 -880 250 -880 {}
P 4 5 300 -460 300 -160 430 -160 430 -460 300 -460 {}
P 4 5 430 -840 430 -540 300 -540 300 -840 430 -840 {}
]
C {lab_pin.sym} 700 -530 0 1 {name=p1 lab=Z embed=true}
