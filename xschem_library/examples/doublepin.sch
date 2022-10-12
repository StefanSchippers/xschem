v {xschem version=3.1.0 file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
N 210 -300 250 -300 {
lab=CK}
N 250 -300 250 -220 {
lab=CK}
N 250 -220 340 -220 {
lab=CK}
N 210 -260 210 -240 {
lab=RST}
N 210 -240 340 -240 {
lab=RST}
N 210 -190 280 -190 {
lab=B}
N 280 -260 280 -190 {
lab=B}
N 280 -260 340 -260 {
lab=B}
N 210 -220 230 -220 {
lab=A[3:0]}
N 230 -280 230 -220 {
lab=A[3:0]}
N 230 -280 340 -280 {
lab=A[3:0]}
N 540 -280 580 -280 {
lab=Z,NC1,NC2,NC3}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {ipin.sym} 210 -220 0 0 {name=p1 lab=A[3:0]}
C {ipin.sym} 210 -190 0 0 {name=p3 lab=B}
C {opin.sym} 690 -250 0 0 {name=p4 lab=Z}
C {ipin.sym} 210 -260 0 0 {name=p5 lab=RST}
C {ipin.sym} 210 -300 0 0 {name=p7 lab=CK}
C {noconn.sym} 690 -250 2 1 {name=l6}
C {use.sym} 380 -480 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
C {sync_reg.sym} 440 -250 0 0 {name=x1 width=4 del=400 delay="400 ps"}
C {lab_pin.sym} 580 -280 0 1 {name=l1 sig_type=std_logic lab=Z,NC1,NC2,NC3}
