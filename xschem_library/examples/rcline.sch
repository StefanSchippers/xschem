v {xschem version=2.9.8 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
N 370 -180 420 -180 { lab=OUT}
N 270 -180 310 -180 { lab=#net1}
N 170 -180 210 -180 { lab=IN}
N 290 -180 290 -130 { lab=#net1}
N 290 -70 290 -40 { lab=0}
C {ipin.sym} 170 -180 0 0 {name=p1 lab=IN}
C {opin.sym} 420 -180 0 0 {name=p2 lab=OUT}
C {capa.sym} 290 -100 0 0 {name=C1
m=1
value='Cap/2'
footprint=1206
device="ceramic capacitor"}
C {res.sym} 340 -180 3 0 {name=R1
value='Res/2'
footprint=1206
device=resistor
m=1}
C {res.sym} 240 -180 3 0 {name=R2
value='Res/2'
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 290 -40 0 0 {name=l1 sig_type=std_logic lab=0}
