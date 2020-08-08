v {xschem version=2.9.5_RC5 file_version=1.1}
G {
Y <= not A after delay ;}
V {assign #del Y=~A;}
S {}
E {}
C {opin.sym} 550 -260 0 0 {name=p1 lab=Y verilog_type=wire}
C {ipin.sym} 270 -260 0 0 {name=p2 lab=A}
C {use.sym} 350 -560 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
--         use ieee.std_logic_arith.all;
--         use ieee.std_logic_unsigned.all;

-- library SYNOPSYS;
--         use SYNOPSYS.ATTRIBUTES.ALL;
}
C {title.sym} 160 -30 0 0 {name=l2}
