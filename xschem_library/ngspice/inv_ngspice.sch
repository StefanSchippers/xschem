v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
N 470 -470 470 -430 { lab=#net1}
N 470 -470 540 -470 { lab=#net1}
N 470 -370 470 -320 { lab=0}
N 600 -470 670 -470 { lab=Y1}
N 320 -470 360 -470 { lab=A1}
C {ipin.sym} 70 -240 0 0 {name=p1 lab=A}
C {opin.sym} 320 -240 0 0 {name=p4 lab=Y}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {bsource.sym} 470 -400 0 1 {name=B1 VAR=V FUNC="'VCC/2*(1-tanh((V(A1)-VCC/2)*100))'"
}
C {lab_pin.sym} 320 -470 0 0 {name=l2 sig_type=std_logic lab=A1}
C {lab_pin.sym} 470 -320 0 0 {name=l3 sig_type=std_logic lab=0}
C {res.sym} 570 -470 1 0 {name=R1
value=ROUT
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 670 -470 0 1 {name=l5 sig_type=std_logic lab=Y1}
C {lab_pin.sym} 130 -240 0 1 {name=l6 sig_type=std_logic lab=A1}
C {lab_pin.sym} 260 -240 0 0 {name=l7 sig_type=std_logic lab=Y1}
C {parax_cap.sym} 360 -460 0 0 {name=C1 gnd=0 value=8f m=1}
C {parax_cap.sym} 620 -460 0 0 {name=C2 gnd=0 value=8f m=1}
C {vsource.sym} 290 -240 1 0 {name=V1 value=0}
C {vsource.sym} 100 -240 1 0 {name=V2 value=0}
