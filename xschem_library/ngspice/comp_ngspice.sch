v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
N 450 -240 450 -180 { lab=IOUT}
N 450 -240 540 -240 { lab=IOUT}
N 600 -240 680 -240 { lab=OUT}
C {ipin.sym} 120 -260 0 0 {name=p1 lab=PLUS}
C {ipin.sym} 120 -210 0 0 {name=p2 lab=MINUS}
C {opin.sym} 680 -240 0 0 {name=p3 lab=OUT}
C {lab_pin.sym} 180 -260 0 1 {name=l6 sig_type=std_logic lab=IPLUS}
C {lab_pin.sym} 180 -210 0 1 {name=l1 sig_type=std_logic lab=IMINUS}
C {lab_pin.sym} 450 -240 0 0 {name=l2 sig_type=std_logic lab=IOUT}
C {lab_pin.sym} 450 -120 0 0 {name=p35 lab=0}
C {bsource.sym} 450 -150 0 1 {name=B1 VAR=V FUNC="'VCC/2*(1+tanh(V(IPLUS,IMINUS)*300))'"
}
C {title.sym} 160 -30 0 0 {name=l3 author="Stefan Schippers"}
C {res.sym} 570 -240 1 0 {name=R1
value=1k
footprint=1206
device=resistor
m=1}
C {parax_cap.sym} 630 -230 0 0 {name=C3 gnd=0 value=8f m=1}
C {vsource.sym} 150 -260 1 0 {name=V1 value=0}
C {vsource.sym} 150 -210 1 0 {name=V2 value=0}
