v {xschem version=2.9.5_RC6 file_version=1.1}
G {
y <= not a after 0.1 ns ;}
V {assign #150 y=~a ;}
S {}
E {}
N 420 -280 420 -240 {lab=y}
N 420 -260 460 -260 {lab=y}
N 380 -310 380 -210 {lab=a}
N 340 -260 380 -260 {lab=a}
N 420 -310 500 -310 {lab=VCCPIN}
N 500 -340 500 -310 {lab=VCCPIN}
N 420 -340 500 -340 {lab=VCCPIN}
N 420 -210 520 -210 {lab=VSSPIN}
N 520 -210 520 -180 {lab=VSSPIN}
N 420 -180 520 -180 {lab=VSSPIN}
N 420 -380 420 -340 {lab=VCCPIN}
N 420 -180 420 -160 {lab=VSSPIN}
C {opin.sym} 460 -260 0 0 {name=p1 lab=y verilog_type=wire}
C {ipin.sym} 340 -260 0 0 {name=p2 lab=a}
C {use.sym} 350 -550 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
--         use ieee.std_logic_arith.all;
--         use ieee.std_logic_unsigned.all;

-- library SYNOPSYS;
--         use SYNOPSYS.ATTRIBUTES.ALL;
}
C {p.sym} 400 -310 0 0 {name=m2 model=cmosp w=wp l=lp  m=1 }
C {lab_pin.sym} 420 -380 0 0 {name=p149 lab=VCCPIN}
C {lab_pin.sym} 420 -160 0 0 {name=p3 lab=VSSPIN}
C {n.sym} 400 -210 0 0 {name=m1 model=cmosn w=wn l=lln m=1}
C {title.sym} 160 0 0 0 {name=l3 author="Stefan Schippers"}
C {verilog_timescale.sym} 660 -217.5 0 0 {name=s1 timestep="1ps" precision="1ps" }
