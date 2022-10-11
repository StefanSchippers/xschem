v {xschem version=3.1.0 file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {ipin.sym} 210 -220 0 0 {name=p1 lab=A[3:0]}
C {ipin.sym} 210 -190 0 0 {name=p3 lab=B}
C {opin.sym} 520 -240 0 0 {name=p4 lab=Z}
C {ipin.sym} 210 -260 0 0 {name=p5 lab=RST}
C {ipin.sym} 210 -300 0 0 {name=p7 lab=CK}
C {noconn.sym} 210 -300 2 0 {name=l2}
C {noconn.sym} 210 -260 2 0 {name=l3}
C {noconn.sym} 210 -220 2 0 {name=l4}
C {noconn.sym} 210 -190 2 0 {name=l5}
C {noconn.sym} 520 -240 2 1 {name=l6}
C {use.sym} 380 -480 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
