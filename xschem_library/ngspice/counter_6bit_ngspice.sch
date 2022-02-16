v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
N 420 -320 640 -320 { lab=S[5:0]}
N 760 -320 810 -320 { lab=COUNT[5:0]}
N 800 -350 800 -320 { lab=COUNT[5:0]}
N 280 -350 800 -350 { lab=COUNT[5:0]}
N 280 -350 280 -320 { lab=COUNT[5:0]}
N 420 -300 460 -300 { lab=COUT[5:0]}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {lab_wire.sym} 520 -320 0 1 {name=l42 lab=S[5:0]}
C {lab_pin.sym} 460 -300 0 1 {name=p43 lab=COUT[5:0]}
C {flip_flop_ngspice.sym} 700 -300 0 0 {name=x2[5:0]}
C {lab_pin.sym} 810 -320 0 1 {name=p11 lab=COUNT[5:0]}
C {lab_pin.sym} 640 -300 0 0 {name=p13 lab=CK}
C {lab_pin.sym} 640 -280 0 0 {name=p14 lab=RST}
C {half_adder_ngspice.sym} 350 -310 0 0 {name=x1[5:0]}
C {lab_pin.sym} 280 -300 0 0 {name=p5 lab=COUT[4:0],D}
C {ipin.sym} 120 -120 0 0 { name=p1 lab=RST }
C {ipin.sym} 120 -140 0 0 { name=p2 lab=D}
C {opin.sym} 220 -160 0 0 { name=p4 lab=COUNT[5:0] }
C {ipin.sym} 120 -160 0 0 { name=p6 lab=CK }
