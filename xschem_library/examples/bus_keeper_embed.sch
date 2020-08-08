v {xschem version=2.9.7 file_version=1.2}
G {}
K {type=subcircuit
format="@name @pinlist @symname WN_FB=@WN_FB WP_FB=@WP_FB"
template="name=X1 WN_FB=3u WP_FB=4u"}
V {}
S {}
E {}
P 4 5 250 -880 250 -120 480 -120 480 -880 250 -880 {}
T {@name} 250 -915 0 0 0.5 0.5 {}
T {@symname} 253.75 -115 0 0 0.5 0.5 {}
N 250 -300 300 -300 {lab=A}
N 280 -680 300 -680 {lab=A}
N 280 -680 280 -300 {lab=A}
N 430 -300 450 -300 {lab=#net1}
N 450 -680 450 -300 {lab=#net1}
N 430 -680 450 -680 {lab=#net1}
C {cmos_inv.sch} 240 -70 0 0 {name=X2 WN=15u WP=45u LLN=3u LLP=3u embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=subcircuit
format="@name @pinlist @symname WN=@WN WP=@WP LLN=@LLN LLP=@LLP"
template="name=X1 WN=15u WP=45u LLN=3u LLP=3u"
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
B 5 57.5 -232.5 62.5 -227.5 {name=A dir=in}
B 5 187.5 -232.5 192.5 -227.5 {name=Z dir=out}
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
T {WN\\/LLN\\/1} 127.5 -188.75 0 0 0.2 0.2 {}
T {M1} 127.5 -163.75 0 0 0.2 0.2 {}
T {D} 145 -197.5 0 0 0.15 0.15 {}
T {WP\\/LLP\\/1} 127.5 -307.5 0 0 0.2 0.2 {}
T {M2} 127.5 -283.75 0 0 0.2 0.2 {}
T {D} 145 -270 0 0 0.15 0.15 {}
T {VDD} 127.5 -375 0 0 0.2 0.2 {}
T {0} 132.5 -128.125 0 1 0.33 0.33 {}
T {A} 63.75 -246.25 0 0 0.2 0.2 {}
T {Z} 186.25 -246.25 0 1 0.2 0.2 {}
T {0} 147.5 -178.125 0 0 0.33 0.33 {}
T {VDD} 147.5 -298.125 0 0 0.33 0.33 {}
P 4 5 60 -390 60 -90 190 -90 190 -390 60 -390 {}
]
C {cmos_inv.sch} 490 -450 0 1 {name=X1 WN=WN_FB WP=WP_FB LLN=3u LLP=3u embed=true}
C {iopin.sym} 250 -300 0 1 {name=p1 lab=A embed=true}
[
v {xschem version=2.9.7 file_version=1.2}
G {type=iopin
format="*.iopin @lab"
template="name=p1 lab=xxx"
}
V {}
S {}
E {}
B 5 -0.0098 -0.009765619999999999 0.0098 0.009765619999999999 {name=p dir=inout}
T {@lab} 19.8438 -8.75 0 0 0.4 0.33 {}
P 5 7 0 0 5.625 -4.84375 10.7812 -4.84375 16.4062 -0 10.7812 4.84375 5.625 4.84375 -0 0 {fill=true}
]
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
