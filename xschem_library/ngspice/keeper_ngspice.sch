v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
N 260 -300 290 -300 { lab=A}
N 260 -300 260 -200 { lab=A}
N 260 -200 290 -200 { lab=A}
N 370 -300 400 -300 { lab=Y}
N 400 -300 400 -200 { lab=Y}
N 370 -200 400 -200 { lab=Y}
N 220 -250 260 -250 { lab=A}
C {ipin.sym} 220 -250 0 0 {name=p1 lab=A}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {inv_ngspice.sym} 330 -200 0 0 {name=x1 ROUT=1000}
C {inv_ngspice.sym} 330 -300 0 1 {name=x2 ROUT=300k}
C {lab_pin.sym} 400 -250 0 1 {name=l2 sig_type=std_logic lab=Y}
