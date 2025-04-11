v {xschem version=3.4.7RC file_version=1.2}
G {}
K {}
V {}
S {}
E {}
N 340 -240 340 -160 {lab=TAIL}
N 340 -320 340 -300 {lab=IN}
N 300 -160 370 -160 {lab=TAIL}
N 80 -320 340 -320 {lab=IN}
N 80 -160 180 -160 {lab=CTL}
N 80 -230 240 -230 {lab=VCC}
C {ipin.sym} 80 -320 0 0 {name=p1 lab=IN}
C {ipin.sym} 80 -230 0 0 {name=p2 lab=VCC}
C {ipin.sym} 80 -160 0 0 {name=p3 lab=CTL}
C {ainv.sym} 240 -160 0 0 {name=XINV}
C {lab_pin.sym} 370 -160 0 1 {name=p5 lab=TAIL}
C {capa.sym} 340 -270 0 0 {name=CB
m=1
value=c='C'
footprint=1206
device="ceramic capacitor"}
