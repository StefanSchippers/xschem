v {xschem version=3.4.8RC file_version=1.3}
G {}
K {}
V {}
S {}
F {}
E {}
N 160 -200 160 -130 {lab=VSS}
N 200 -190 200 -130 {
lab=VSS}
N 400 -260 400 -220 {lab=vcm}
N 400 -360 400 -320 {lab=vp}
N 200 -310 200 -250 {lab=#net1}
N 320 -270 360 -270 {lab=VSS}
N 260 -130 360 -130 {lab=VSS}
N 260 -310 360 -310 {lab=#net1}
N 400 -220 400 -180 {lab=vcm}
N 400 -120 400 -80 {lab=vinn}
N 200 -130 200 -80 {lab=VSS}
N 80 -240 160 -240 {lab=vin}
N 400 -220 480 -220 {lab=vcm}
N 400 -360 480 -360 {lab=vp}
N 400 -80 480 -80 {lab=vinn}
N 160 -130 200 -130 {lab=VSS}
N 320 -170 360 -170 {lab=#net1}
N 260 -310 260 -300 {lab=#net1}
N 260 -140 260 -130 {lab=VSS}
N 260 -140 320 -270 {lab=VSS}
N 260 -300 320 -170 {lab=#net1}
N 200 -310 260 -310 {lab=#net1}
N 200 -130 260 -130 {lab=VSS}
C {devices/vcvs.sym} 400 -150 0 0 {name=E3 value=0.5}
C {devices/vcvs.sym} 400 -290 0 0 {name=E2 value=0.5}
C {devices/vcvs.sym} 200 -220 0 0 {name=E1 value=1}
C {devices/ipin.sym} 80 -240 0 0 {name=p8 lab=vin}
C {devices/opin.sym} 480 -80 0 0 {name=p7 lab=vn}
C {devices/iopin.sym} 480 -220 0 0 {name=p2 lab=vcm}
C {devices/opin.sym} 480 -360 0 0 {name=p1 lab=vp}
C {devices/iopin.sym} 200 -80 1 0 {name=p3 lab=VSS}
