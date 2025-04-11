v {xschem version=3.4.7RC file_version=1.2}
G {}
K {}
V {}
S {}
E {}
N 290 -240 290 -220 {lab=A}
N 290 -160 290 -140 {lab=B}
C {switch_ngspice.sym} 290 -190 0 0 {name=SWITCH model=TG
device_model=".MODEL TG SW( VT=1.5 RON=2k )"}
C {lab_pin.sym} 250 -190 0 0 {name=p1 lab=CTL}
C {lab_pin.sym} 290 -240 0 1 {name=p2 lab=A}
C {lab_pin.sym} 290 -140 0 1 {name=p3 lab=B}
C {lab_pin.sym} 250 -170 0 0 {name=p4 lab=0}
C {ipin.sym} 110 -170 0 0 { name=p5 lab=CTL }
C {iopin.sym} 110 -190 0 1 { name=p6 lab=B }
C {iopin.sym} 110 -210 0 1 { name=p7 lab=A }
C {ipin.sym} 110 -150 0 0 { name=p8 lab=VDD }
C {noconn.sym} 110 -150 0 1 {name=l1}
