v {xschem version=2.9.5_RC5 file_version=1.1}
G {
Y <=  A xor B after delay ;}
V {
assign #del Y=A^B;}
S {}
E {}
C {ipin.sym} 150 -320 0 0 {name=p2 lab=A}
C {ipin.sym} 150 -350 0 0 {name=p3 lab=B}
C {opin.sym} 920 -350 0 0 {name=p0 lab=Y verilog_type=wire}
C {use.sym} 150 -650 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
--         use ieee.std_logic_arith.all;
--         use ieee.std_logic_unsigned.all;

-- library SYNOPSYS;
--         use SYNOPSYS.ATTRIBUTES.ALL;
}
C {title.sym} 160 -30 0 0 {name=l2}
