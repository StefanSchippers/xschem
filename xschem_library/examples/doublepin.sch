v {xschem version=3.1.0 file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
N 360 -240 400 -240 {
lab=CK}
N 210 -190 400 -190 {
lab=B}
N 210 -280 400 -280 {
lab=A[3:0]}
N 540 -280 580 -280 {
lab=Z,NC1,NC2,NC3}
N 330 -150 470 -150 {
lab=RST}
N 470 -210 470 -150 {
lab=RST}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {ipin.sym} 210 -280 0 0 {name=p1 lab=A[3:0]}
C {ipin.sym} 210 -190 0 0 {name=p3 lab=B}
C {opin.sym} 810 -250 0 0 {name=p4 lab=Z}
C {ipin.sym} 330 -150 0 0 {name=p5 lab=RST}
C {ipin.sym} 360 -240 0 0 {name=p7 lab=CK}
C {noconn.sym} 810 -250 2 1 {name=l6}
C {use.sym} 380 -480 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
C {lab_pin.sym} 580 -280 0 1 {name=l2 sig_type=std_logic lab=Z,NC1,NC2,NC3}
C {latch.sym} 470 -260 0 0 {name=x2[3:0] delay="200 ps" del=200}
C {lab_pin.sym} 540 -240 0 1 {name=l3 sig_type=std_logic lab=NC4,NC5,NC6,NC7}
C {noconn.sym} 400 -190 2 0 {name=l4}
