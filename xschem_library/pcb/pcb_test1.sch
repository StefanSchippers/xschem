v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
B 20 350 -550 940 -290 {}
T {3 of 4 NANDS of a 74ls00} 580 -580 0 0 0.4 0.4 {}
T {EXPERIMENTAL schematic for generating a tEDAx netlist
1) set netlist mode to 'tEDAx' (Options menu -> tEDAx netlist)
2) press 'Netlist' button on the right
3) resulting netlist is in pcb_test1.tdx } 40 -730 0 0 0.5 0.5 {font=monospace}
N 310 -330 380 -330 {lab=INPUT_B}
N 310 -370 380 -370 {lab=INPUT_A}
N 760 -420 830 -420 {lab=B}
N 760 -460 830 -460 {lab=A}
N 480 -350 520 -350 {lab=B}
N 930 -440 970 -440 {lab=OUTPUT_Y}
N 310 -440 380 -440 {lab=INPUT_F}
N 310 -480 380 -480 {lab=INPUT_E}
N 480 -460 520 -460 {lab=A}
N 550 -190 670 -190 {lab=VCCFILT}
N 590 -130 590 -110 {lab=ANALOG_GND}
N 790 -190 940 -190 {lab=VCC5}
N 890 -130 890 -110 {lab=ANALOG_GND}
N 730 -110 890 -110 {lab=ANALOG_GND}
N 730 -160 730 -110 {lab=ANALOG_GND}
N 590 -110 730 -110 {lab=ANALOG_GND}
N 520 -460 760 -460 {lab=A}
N 580 -420 760 -420 {lab=B}
N 580 -420 580 -350 {lab=B}
N 520 -350 580 -350 {lab=B}
C {title.sym} 160 -30 0 0 {name=l2 author="Stefan"}
C {74ls00.sym} 420 -350 0 0 {name=U1:2  risedel=100 falldel=200}
C {74ls00.sym} 870 -440 0 0 {name=U1:1  risedel=100 falldel=200}
C {lab_pin.sym} 970 -440 0 1 {name=p0 lab=OUTPUT_Y}
C {capa.sym} 590 -160 0 0 {name=C0 m=1 value=100u device="electrolitic capacitor"}
C {74ls00.sym} 420 -460 0 0 {name=U1:4 risedel=100 falldel=200
url="http://www.engrcs.com/components/74LS00.pdf" 
power=VCC5 

___net:14=VCC5
___pinnumber(B)=111:222:333:444}
C {7805.sym} 730 -190 0 0 {name=U0 
spiceprefix=X
url="https://www.sparkfun.com/datasheets/Components/LM7805.pdf"}
C {lab_pin.sym} 490 -190 0 0 {name=p20 lab=VCC12}
C {lab_pin.sym} 940 -190 0 1 {name=p22 lab=VCC5}
C {lab_pin.sym} 590 -110 0 0 {name=p23 lab=ANALOG_GND}
C {capa.sym} 890 -160 0 0 {name=C4 m=1 value=10u device="tantalium capacitor"}
C {res.sym} 520 -190 1 0 {name=R0 m=1 value=4.7 device="carbonresistor"}
C {lab_wire.sym} 700 -460 0 0 {name=l3 lab=A}
C {lab_wire.sym} 700 -420 0 0 {name=l0 lab=B}
C {lab_wire.sym} 650 -190 0 0 {name=l1 lab=VCCFILT}
C {lab_pin.sym} 310 -370 0 0 {name=lONN1 lab=INPUT_A verilog_type=reg}
C {lab_pin.sym} 310 -330 0 0 {name=lONN2 lab=INPUT_B verilog_type=reg}
C {lab_pin.sym} 40 -330 0 1 { name=lONN3 lab=OUTPUT_Y }
C {lab_pin.sym} 310 -480 0 0 {name=lONN6 lab=INPUT_E verilog_type=reg}
C {lab_pin.sym} 310 -440 0 0 {name=lONN8 lab=INPUT_F verilog_type=reg}
C {lab_pin.sym} 40 -310 0 1 { name=lONN9 lab=VCC12 }
C {lab_pin.sym} 40 -290 0 1 { name=lONN14 lab=ANALOG_GND  verilog_type=reg}
C {lab_pin.sym} 40 -270 0 1 { name=lONN15 lab=GND  verilog_type=reg}
C {code.sym} 1030 -280 0 0 {name=TESTBENCH_CODE only_toplevel=false value="initial begin
  $dumpfile(\\"dumpfile.vcd\\");
  $dumpvars;
  INPUT_E=0;
  INPUT_F=0;
  INPUT_A=0;
  INPUT_B=0;
  ANALOG_GND=0;
  #10000;
  INPUT_A=1;
  INPUT_B=1;
  #10000;
  INPUT_E=1;
  INPUT_F=1;
  #10000;
  INPUT_F=0;
  #10000;
  INPUT_B=0;
  #10000;
  $finish;
end

assign VCC12=1;

"}
C {verilog_timescale.sym} 1050 -100 0 0 {name=s1 timestep="1ns" precision="1ns" }
C {conn_8x1.sym} 20 -390 0 0 {name=C1 footprint=connector(8,1)}
C {lab_pin.sym} 40 -370 0 1 {name=l4 lab=INPUT_A verilog_type=reg}
C {lab_pin.sym} 40 -350 0 1 {name=l5 lab=INPUT_B verilog_type=reg}
C {lab_pin.sym} 40 -410 0 1 {name=l6 lab=INPUT_E verilog_type=reg}
C {lab_pin.sym} 40 -390 0 1 {name=l7 lab=INPUT_F verilog_type=reg}
