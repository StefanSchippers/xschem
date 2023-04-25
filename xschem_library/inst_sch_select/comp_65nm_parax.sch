v {xschem version=3.1.0 file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
N 100 -310 120 -310 {
lab=PLUS}
N 100 -260 120 -260 {
lab=MINUS}
N 230 -290 230 -260 {
lab=OUT}
C {opin.sym} 230 -290 0 0 {name=p20 lab=OUT}
C {ipin.sym} 100 -260 0 0 {name=p1 lab=MINUS}
C {ipin.sym} 100 -310 0 0 {name=p161 lab=PLUS}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {parax_cap.sym} 120 -300 0 0 {name=C1 gnd=0 value=50f m=1}
C {parax_cap.sym} 120 -250 0 0 {name=C2 gnd=0 value=50f m=1}
C {ammeter.sym} 230 -230 0 0 {name=Vmeas}
C {lab_pin.sym} 230 -200 0 0 {name=p2 sig_type=std_logic lab=0}
