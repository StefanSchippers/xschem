v {xschem version=3.0.0 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
T {Set netlist mode to 'verilog netlist'
(Options menu), then press 'Netlist'
and 'Simulate' button.
You need to have Icarus verilog installed
Configure the verilog simulator in 
Simulation-> Configure simulators and tools'
menu.} 700 -240 0 0 0.4 0.4 {}
T {Trivial Depletion NMOS inverter simulation in verilog} 140 -670 0 0 0.7 0.7 {}
N 490 -470 490 -410 { lab=VDD}
N 490 -380 550 -380 { lab=GND}
N 490 -220 550 -220 { lab=GND}
N 490 -350 490 -250 { lab=OUT}
N 490 -190 490 -150 { lab=GND}
N 390 -220 450 -220 { lab=IN}
N 490 -300 610 -300 { lab=OUT}
N 450 -380 450 -340 { lab=OUT}
N 450 -340 490 -340 { lab=OUT}
C {nmos4.sym} 470 -220 0 0 {name=M1 model=nmos w=5u l=0.18u m=1}
C {nmos4_depl.sym} 470 -380 0 0 {name=M3 model=nmos w=5u l=0.18u m=1}
C {gnd.sym} 550 -380 0 0 {name=l1 lab=GND}
C {gnd.sym} 550 -220 0 0 {name=l2 lab=GND}
C {gnd.sym} 490 -150 0 0 {name=l3 lab=GND value=0}
C {vdd.sym} 490 -470 0 0 {name=l4 lab=VDD value=1}
C {lab_pin.sym} 390 -220 0 0 {name=l5 sig_type=std_logic lab=IN verilog_type=reg}
C {lab_pin.sym} 610 -300 0 1 {name=l6 sig_type=std_logic lab=OUT}
C {verilog_timescale.sym} 0 -520 0 0 {name=s1 timestep="1ps" precision="1ps" }
C {code_shown.sym} 0 -390 0 0 {name=testbench only_toplevel=false value="initial begin
  $dumpfile(\\"dumpfile.vcd\\");
  $dumpvars(0, test_mos_verilog);

  IN = 1;
  #100000;
  IN = 0;
  #100000;
  IN = 1;
  #100000;
  IN = 0;
  #100000;
  $finish;
end
"}
C {title.sym} 160 -30 0 0 {name=l7 author="Stefan Schippers"}
