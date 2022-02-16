v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
N 480 -400 480 -360 { lab=#net1}
N 480 -400 550 -400 { lab=#net1}
N 480 -300 480 -250 { lab=0}
N 610 -400 680 -400 { lab=Y1}
N 330 -400 370 -400 { lab=A1}
N 330 -460 370 -460 { lab=B1}
C {ipin.sym} 80 -170 0 0 {name=p1 lab=A}
C {opin.sym} 330 -170 0 0 {name=p4 lab=Y}
C {bsource.sym} 480 -330 0 1 {name=B1 VAR=V FUNC="'VCC/2*(1-tanh((abs(V(A1)-V(B1))-VCC/2)*100))'"
}
C {lab_pin.sym} 330 -400 0 0 {name=l2 sig_type=std_logic lab=A1}
C {lab_pin.sym} 480 -250 0 0 {name=l3 sig_type=std_logic lab=0}
C {res.sym} 580 -400 1 0 {name=R1
value=ROUT
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 680 -400 0 1 {name=l5 sig_type=std_logic lab=Y1}
C {ammeter.sym} 110 -170 3 0 {name=Va}
C {lab_pin.sym} 140 -170 0 1 {name=l6 sig_type=std_logic lab=A1}
C {ammeter.sym} 300 -170 3 0 {name=Vy}
C {lab_pin.sym} 270 -170 0 0 {name=l7 sig_type=std_logic lab=Y1}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {ipin.sym} 80 -110 0 0 {name=p2 lab=B}
C {ammeter.sym} 110 -110 3 0 {name=Va1}
C {lab_pin.sym} 140 -110 0 1 {name=l4 sig_type=std_logic lab=B1}
C {lab_pin.sym} 330 -460 0 0 {name=l8 sig_type=std_logic lab=B1}
C {parax_cap.sym} 370 -390 0 0 {name=C1 gnd=0 value=8f m=1}
C {parax_cap.sym} 370 -450 0 0 {name=C2 gnd=0 value=8f m=1}
C {parax_cap.sym} 630 -390 0 0 {name=C3 gnd=0 value=8f m=1}
