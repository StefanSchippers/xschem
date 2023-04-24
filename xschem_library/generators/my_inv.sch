v {xschem version=3.1.0 file_version=1.2
}
G {
y <= not a after 0.1 ns ;}
K {}
V {assign #150 y=~a ;}
S {}
E {}
N 190 -250 190 -210 {lab=y}
N 190 -230 230 -230 {lab=y}
N 150 -280 150 -180 {lab=a}
N 110 -230 150 -230 {lab=a}
N 190 -280 270 -280 {lab=VCC}
N 270 -310 270 -280 {lab=VCC}
N 190 -310 270 -310 {lab=VCC}
N 190 -180 290 -180 {lab=0}
N 290 -180 290 -150 {lab=0}
N 190 -150 290 -150 {lab=0}
N 190 -350 190 -310 {lab=VCC}
N 190 -150 190 -130 {lab=0}
C {opin.sym} 230 -230 0 0 {name=p1 lab=y verilog_type=wire}
C {ipin.sym} 110 -230 0 0 {name=p2 lab=a}
C {p.sym} 170 -280 0 0 {name=m2 model=cmosp w=wp l=lp  m=1 }
C {lab_pin.sym} 190 -350 0 0 {name=p149 lab=VCC}
C {lab_pin.sym} 190 -130 0 0 {name=p3 lab=0}
C {n.sym} 170 -180 0 0 {name=m1 model=cmosn w=wn l=lln m=1}
C {title.sym} 160 0 0 0 {name=l3 author="Stefan Schippers"}
