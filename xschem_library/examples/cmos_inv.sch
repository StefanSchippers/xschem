v {xschem version=2.9.7 file_version=1.2}
G {}
K {type=subcircuit
format="@name @pinlist @symname WN=@WN WP=@WP LLN=@LLN LLP=@LLP m=@m"
template="name=X1 WN=15u WP=45u LLN=3u LLP=3u m=1"
}
V {}
S {}
E {}
B 2 60 -390 190 -90 {dash=5}
T {@name} 60 -405 0 0 0.2 0.2 {}
T {@symname} 63.75 -85 0 0 0.2 0.2 {}
N 140 -260 140 -200 {lab=Z}
N 100 -290 100 -170 {lab=A}
N 60 -230 100 -230 {lab=A}
N 140 -230 190 -230 {lab=Z}
N 140 -340 140 -320 {lab=VDD}
N 140 -140 140 -120 {lab=0}
C {nmos4.sym} 120 -170 0 0 {name=M1 model=n w=WN l=LLN m=1}
C {pmos4.sym} 120 -290 0 0 {name=M2 model=p w=WP l=LLP m=1}
C {vdd.sym} 140 -340 0 0 {name=l1 lab=VDD}
C {lab_pin.sym} 140 -120 0 0 {name=l2 sig_type=std_logic lab=0}
C {ipin.sym} 60 -230 0 0 {name=p1 lab=A}
C {opin.sym} 190 -230 0 0 {name=p2 lab=Z}
C {lab_pin.sym} 140 -170 0 1 {name=l3 sig_type=std_logic lab=0}
C {lab_pin.sym} 140 -290 0 1 {name=l4 sig_type=std_logic lab=VDD}
C {title.sym} 160 -30 0 0 {name=l5 author="Stefan Schippers"}
