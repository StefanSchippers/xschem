v {xschem version=2.9.8 file_version=1.2}
G {process
begin
  if now = 0 ns then
    A <= "00000000";
  elsif A = "00000000" and now > 100 ns then
    wait;
  end if;
  wait for 100 ns;
  A <= A + 1;
end process;}
K {}
V {initial begin
  $dumpfile("dumpfile.vcd");
  $dumpvars(0, greycnt);
  A=0;
end

always begin
  #100000;
  $display("%08b %08b", A, B);
  A=A + 1;
  if(A==0) $finish;
end}
S {
.tran 1n 2000n

* to generate following file copy .../share/doc/xschem/examples/stimuli.greycnt
* to the simulation directory and run simulation -> Utile Stimuli Editor (GUI), 
* and press 'Translate'
.include stimuli_greycnt.cir

.model nmos NMOS
+ LEVEL=1
+ LMIN=0.5e-6 LMAX=50e-6 WMIN=0.9e-6 WMAX=1
+ VTO=0.7 GAMMA=0.45 PHI=0.9
+ NSUB=9e14 LD=0.08e-6 UO=350 LAMBDA=0.1
+ TOX=9e-9 PB=0.9 CJ=0.56e-3 CJSW=0.35e-11
+ MJ=0.45 MJSW=0.2 CGDO=0.4e-9 JS=1.0e-8
.model pmos PMOS
+ LEVEL=1
+ LMIN=0.5e-6 LMAX=50e-6 WMIN=0.9e-6 WMAX=1
+ VTO=-0.8 GAMMA=0.4 PHI=0.8
+ NSUB=5e14 LD=0.09e-6 UO=100 LAMBDA=0.2
+ TOX=9e-9 PB=0.9 CJ=0.94e-3 CJSW=0.32e-11
+ MJ=0.5 MJSW=0.3 CGDO=0.3e-9 JS=0.5e-8}
E {}
T {BINARY} 500 -780 0 0 0.4 0.4 {}
T {GRAY} 830 -780 0 0 0.4 0.4 {}
T {BINARY} 1210 -780 0 0 0.4 0.4 {}
T {This example can be simulated in SPICE, VHDL, VERILOG} 50 -860 0 0 0.6 0.6 {layer=7}
N 720 -630 830 -630 {lab=B[6]}
N 570 -610 620 -610 {lab=A[6]}
N 570 -530 620 -530 {lab=A[5]}
N 570 -450 620 -450 {lab=A[4]}
N 720 -550 830 -550 {lab=B[5]}
N 720 -470 830 -470 {lab=B[4]}
N 570 -690 630 -690 {lab=A[7]}
N 720 -390 830 -390 {lab=B[3]}
N 570 -370 620 -370 {lab=A[3]}
N 570 -290 620 -290 {lab=A[2]}
N 570 -210 620 -210 {lab=A[1]}
N 720 -310 830 -310 {lab=B[2]}
N 720 -230 830 -230 {lab=B[1]}
N 570 -130 620 -130 {lab=A[0]}
N 720 -150 830 -150 {lab=B[0]}
N 830 -130 1020 -130 {lab=B[0]}
N 830 -150 830 -130 {lab=B[0]}
N 830 -210 1020 -210 {lab=B[1]}
N 830 -230 830 -210 {lab=B[1]}
N 830 -290 1020 -290 {lab=B[2]}
N 830 -310 830 -290 {lab=B[2]}
N 830 -370 1020 -370 {lab=B[3]}
N 830 -390 830 -370 {lab=B[3]}
N 830 -450 1020 -450 {lab=B[4]}
N 830 -470 830 -450 {lab=B[4]}
N 830 -530 1020 -530 {lab=B[5]}
N 830 -550 830 -530 {lab=B[5]}
N 830 -610 1020 -610 {lab=B[6]}
N 830 -630 830 -610 {lab=B[6]}
N 690 -690 830 -690 {lab=B[7]}
N 830 -690 1030 -690 {lab=B[7]}
N 830 -710 830 -690 {lab=B[7]}
N 1120 -630 1230 -630 {lab=C[6]}
N 1120 -550 1230 -550 {lab=C[5]}
N 1120 -470 1230 -470 {lab=C[4]}
N 1120 -390 1230 -390 {lab=C[3]}
N 1120 -310 1230 -310 {lab=C[2]}
N 1120 -230 1230 -230 {lab=C[1]}
N 1120 -150 1230 -150 {lab=C[0]}
N 1090 -690 1130 -690 {lab=C[7]}
N 1130 -710 1130 -690 {lab=C[7]}
N 1130 -710 1230 -710 {lab=C[7]}
N 1120 -630 1120 -590 {lab=C[6]}
N 1020 -590 1120 -590 {lab=C[6]}
N 1020 -590 1020 -570 {lab=C[6]}
N 1120 -550 1120 -510 {lab=C[5]}
N 1020 -510 1120 -510 {lab=C[5]}
N 1020 -510 1020 -490 {lab=C[5]}
N 1120 -470 1120 -430 {lab=C[4]}
N 1020 -430 1120 -430 {lab=C[4]}
N 1020 -430 1020 -410 {lab=C[4]}
N 1120 -390 1120 -350 {lab=C[3]}
N 1020 -350 1120 -350 {lab=C[3]}
N 1020 -350 1020 -330 {lab=C[3]}
N 1120 -310 1120 -270 {lab=C[2]}
N 1020 -270 1120 -270 {lab=C[2]}
N 1020 -270 1020 -250 {lab=C[2]}
N 1120 -230 1120 -190 {lab=C[1]}
N 1020 -190 1120 -190 {lab=C[1]}
N 1020 -190 1020 -170 {lab=C[1]}
N 1130 -690 1130 -670 {lab=C[7]}
N 1020 -670 1130 -670 {lab=C[7]}
N 1020 -670 1020 -650 {lab=C[7]}
N 620 -690 620 -650 {lab=A[7]}
N 620 -610 620 -570 {lab=A[6]}
N 620 -530 620 -490 {lab=A[5]}
N 620 -450 620 -410 {lab=A[4]}
N 620 -370 620 -330 {lab=A[3]}
N 620 -290 620 -250 {lab=A[2]}
N 620 -210 620 -170 {lab=A[1]}
C {title.sym} 160 -30 0 0 {name=l3 author="Stefan Schippers"}
C {verilog_timescale.sym} 30 -110 0 0 {name=s1 timestep="1ps" precision="1ps" }
C {xnor.sym} 660 -470 0 0 {name=x2}
C {xnor.sym} 660 -550 0 0 {name=x3}
C {xnor.sym} 660 -630 0 0 {name=x14}
C {lab_pin.sym} 570 -690 0 0 {name=p9 lab=A[7] }
C {lab_pin.sym} 570 -610 0 0 {name=p1 lab=A[6] }
C {lab_pin.sym} 570 -530 0 0 {name=p2 lab=A[5] }
C {lab_pin.sym} 570 -450 0 0 {name=p3 lab=A[4] }
C {lab_pin.sym} 830 -710 0 1 {name=p4 lab=B[7]}
C {lab_pin.sym} 830 -630 0 1 {name=p0 lab=B[6]}
C {lab_pin.sym} 830 -550 0 1 {name=p5 lab=B[5]}
C {lab_pin.sym} 830 -470 0 1 {name=p6 lab=B[4]}
C {assign.sym} 660 -690 0 0 {name=v1 delay=1}
C {xnor.sym} 660 -230 0 0 {name=x1}
C {xnor.sym} 660 -310 0 0 {name=x4}
C {xnor.sym} 660 -390 0 0 {name=x5}
C {lab_pin.sym} 570 -370 0 0 {name=p7 lab=A[3] }
C {lab_pin.sym} 570 -290 0 0 {name=p8 lab=A[2] }
C {lab_pin.sym} 570 -210 0 0 {name=p10 lab=A[1]}
C {lab_pin.sym} 830 -390 0 1 {name=p11 lab=B[3]}
C {lab_pin.sym} 830 -310 0 1 {name=p12 lab=B[2]}
C {lab_pin.sym} 830 -230 0 1 {name=p13 lab=B[1]}
C {xnor.sym} 660 -150 0 0 {name=x6}
C {lab_pin.sym} 570 -130 0 0 {name=p14 lab=A[0]}
C {lab_pin.sym} 830 -150 0 1 {name=p15 lab=B[0]}
C {lab_pin.sym} 150 -220 0 1 { name=l16 lab=B[7:0] }
C {lab_pin.sym} 90 -200 0 0 { name=l17 lab=A[7:0]  verilog_type=reg}
C {xnor.sym} 1060 -470 0 0 {name=x7}
C {xnor.sym} 1060 -550 0 0 {name=x8}
C {xnor.sym} 1060 -630 0 0 {name=x9}
C {assign.sym} 1060 -690 0 0 {name=v0 delay=1}
C {xnor.sym} 1060 -230 0 0 {name=x10}
C {xnor.sym} 1060 -310 0 0 {name=x11}
C {xnor.sym} 1060 -390 0 0 {name=x12}
C {xnor.sym} 1060 -150 0 0 {name=x13}
C {lab_pin.sym} 1230 -710 0 1 {name=p18 lab=C[7]}
C {lab_pin.sym} 1230 -630 0 1 {name=p19 lab=C[6]}
C {lab_pin.sym} 1230 -550 0 1 {name=p20 lab=C[5]}
C {lab_pin.sym} 1230 -470 0 1 {name=p21 lab=C[4]}
C {lab_pin.sym} 1230 -390 0 1 {name=p22 lab=C[3]}
C {lab_pin.sym} 1230 -310 0 1 {name=p23 lab=C[2]}
C {lab_pin.sym} 1230 -230 0 1 {name=p24 lab=C[1]}
C {lab_pin.sym} 1230 -150 0 1 {name=p25 lab=C[0]}
C {lab_pin.sym} 150 -190 0 1 { name=l26 lab=C[7:0] }
C {use.sym} 60 -650 0 0 {library ieee;
use std.TEXTIO.all;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;


}
