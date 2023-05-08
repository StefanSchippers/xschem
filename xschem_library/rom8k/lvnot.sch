v {xschem version=3.1.0 file_version=1.2
}
G {
y <= not a after 0.1 ns ;}
K {type=subcircuit
function0="1 ~"
vhdl_stop=true
verilog_stop=true
format="@name @pinlist @VCCPIN @VSSPIN @symname wn=@wn lln=@lln wp=@wp lp=@lp m=@m"
template="name=x1 m=1 
+ wn=10u lln=1.2u wp=10u lp=1.2u 
+ VCCPIN=VCC VSSPIN=VSS"
extra="VCCPIN VSSPIN"
generic_type="m=integer wn=real lln=real wp=real lp=real VCCPIN=string VSSPIN=string"
verilog_stop=true}
V {assign #150 y=~a ;}
S {}
E {}
P 4 5 350 -380 350 -140 480 -140 480 -380 350 -380 {dash=4}
T {@name x @m} 353.75 -395 0 0 0.2 0.2 {}
T {@symname} 486.25 -395 0 1 0.2 0.2 {}
N 420 -280 420 -240 {lab=y}
N 420 -260 480 -260 {lab=y}
N 380 -310 380 -210 {lab=a}
N 350 -260 380 -260 {lab=a}
N 420 -310 470 -310 {lab=VCCPIN}
N 470 -340 470 -310 {lab=VCCPIN}
N 420 -340 470 -340 {lab=VCCPIN}
N 420 -210 470 -210 {lab=VSSPIN}
N 470 -210 470 -180 {lab=VSSPIN}
N 420 -180 470 -180 {lab=VSSPIN}
N 420 -360 420 -340 {lab=VCCPIN}
N 420 -180 420 -160 {lab=VSSPIN}
C {opin.sym} 480 -260 0 0 {name=p1 lab=y verilog_type=wire}
C {ipin.sym} 350 -260 0 0 {name=p2 lab=a}
C {use.sym} 350 -550 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
--         use ieee.std_logic_arith.all;
--         use ieee.std_logic_unsigned.all;

-- library SYNOPSYS;
--         use SYNOPSYS.ATTRIBUTES.ALL;
}
C {p.sym} 400 -310 0 0 {name=m2 model=cmosp w=wp l=lp  m=1 }
C {lab_pin.sym} 420 -360 0 0 {name=p149 lab=VCCPIN}
C {lab_pin.sym} 420 -160 0 0 {name=p3 lab=VSSPIN}
C {n.sym} 400 -210 0 0 {name=m1 model=cmosn w=wn l=lln m=1}
C {title.sym} 160 0 0 0 {name=l3 author="Stefan Schippers"}
C {verilog_timescale.sym} 660 -217.5 0 0 {name=s1 timestep="1ps" precision="1ps" }
