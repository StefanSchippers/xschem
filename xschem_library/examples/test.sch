v {xschem version=2.9.5_RC5 file_version=1.1}
G {}
V {}
S {}
E {}
N 220 -470 260 -470 {lab=SET_BAR}
N 220 -290 260 -290 {lab=CLEAR_BAR}
N 360 -450 390 -450 {lab=Q}
N 360 -310 390 -310 {lab=QBAR}
N 260 -430 260 -410 {lab=QBAR}
N 260 -410 360 -360 {lab=QBAR}
N 360 -360 360 -310 {lab=QBAR}
N 260 -350 260 -330 {lab=Q}
N 260 -350 360 -400 {lab=Q}
N 360 -450 360 -400 {lab=Q}
C {nand2.sym} 310 -450 0 0 {name=x1}
C {lab_pin.sym} 390 -450 0 1 {name=p1 lab=Q}
C {lab_pin.sym} 220 -470 0 0 {name=p2 lab=SET_BAR}
C {lab_pin.sym} 220 -290 0 0 {name=p3 lab=CLEAR_BAR}
C {nand2.sym} 310 -310 2 1 {name=x2}
C {lab_pin.sym} 390 -310 0 1 {name=p4 lab=QBAR}
