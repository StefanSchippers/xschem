v {xschem version=3.1.0 file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
N 150 -80 190 -80 {
lab=A}
N 150 -20 190 -20 {
lab=B}
C {iopin.sym} 190 -80 0 0 {name=p1 lab=A}
C {iopin.sym} 190 -20 0 0 {name=p1 lab=B}
C {use.sym} 160 -240 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
C {res.sym} 150 -50 0 0 {name=R1
value=1k
footprint=1206
device=resistor
verilog_ignore=true
vhdl_ignore=true
m=1}
