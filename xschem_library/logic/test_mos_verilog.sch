v {xschem version=3.4.4 file_version=1.2
*
* This file is part of XSCHEM,
* a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
* simulation.
* Copyright (C) 1998-2024 Stefan Frederik Schippers
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
}
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
menu.} 1050 -240 0 0 0.4 0.4 {}
T {Trivial Depletion NMOS inverter simulation in verilog} 140 -670 0 0 0.7 0.7 {}
T {NMOS
depletion
load} 420 -520 0 0 0.4 0.4 {}
T {CMOS} 710 -520 0 0 0.4 0.4 {}
T {NMOS
Enhancement
load } 980 -520 0 0 0.4 0.4 {}
N 380 -480 380 -420 { lab=VDD}
N 380 -390 440 -390 { lab=GND}
N 380 -230 440 -230 { lab=GND}
N 380 -360 380 -260 { lab=OUT}
N 380 -200 380 -160 { lab=GND}
N 280 -230 340 -230 { lab=IN}
N 380 -310 450 -310 { lab=OUT}
N 340 -390 340 -350 { lab=OUT}
N 340 -350 380 -350 { lab=OUT}
N 650 -480 650 -420 { lab=VDD}
N 650 -230 710 -230 { lab=GND}
N 650 -360 650 -260 { lab=OUT2}
N 650 -200 650 -160 { lab=GND}
N 550 -230 610 -230 { lab=IN}
N 650 -310 720 -310 { lab=OUT2}
N 650 -390 710 -390 { lab=VDD}
N 610 -390 610 -230 { lab=IN}
N 920 -480 920 -420 { lab=VDD}
N 920 -230 980 -230 { lab=GND}
N 920 -360 920 -260 { lab=OUT3}
N 920 -200 920 -160 { lab=GND}
N 820 -230 880 -230 { lab=IN}
N 920 -310 990 -310 { lab=OUT3}
N 920 -390 980 -390 { lab=GND}
N 880 -440 880 -390 { lab=VDD}
N 880 -440 920 -440 { lab=VDD}
C {nmos4.sym} 360 -230 0 0 {name=M1 model=nmos w=5u l=0.18u m=1 del=10}
C {nmos4_depl.sym} 360 -390 0 0 {name=M3 model=nmos w=5u l=0.18u m=1 del=10}
C {gnd.sym} 440 -390 0 0 {name=l1 lab=GND}
C {gnd.sym} 440 -230 0 0 {name=l2 lab=GND}
C {gnd.sym} 380 -160 0 0 {name=l3 lab=GND value=0}
C {vdd.sym} 380 -480 0 0 {name=l4 lab=VDD value=1}
C {lab_pin.sym} 280 -230 0 0 {name=l5 sig_type=std_logic lab=IN verilog_type=reg}
C {lab_pin.sym} 450 -310 0 1 {name=l6 sig_type=std_logic lab=OUT}
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
C {nmos4.sym} 630 -230 0 0 {name=M2 model=nmos w=5u l=0.18u m=1 del=10}
C {gnd.sym} 710 -230 0 0 {name=l8 lab=GND}
C {gnd.sym} 650 -160 0 0 {name=l9 lab=GND }
C {vdd.sym} 650 -480 0 0 {name=l10 lab=VDD }
C {lab_pin.sym} 550 -230 0 0 {name=l11 sig_type=std_logic lab=IN verilog_type=reg}
C {lab_pin.sym} 720 -310 0 1 {name=l12 sig_type=std_logic lab=OUT2}
C {pmos4.sym} 630 -390 0 0 {name=M4 model=pmos w=5u l=0.18u m=1}
C {vdd.sym} 710 -390 0 0 {name=l13 lab=VDD value=1}
C {noconn.sym} 300 -230 1 0 {name=l14}
C {nmos4.sym} 900 -230 0 0 {name=M5 model=nmos w=5u l=0.18u m=1 del=10}
C {gnd.sym} 980 -230 0 0 {name=l15 lab=GND}
C {gnd.sym} 920 -160 0 0 {name=l16 lab=GND }
C {vdd.sym} 920 -480 0 0 {name=l17 lab=VDD }
C {lab_pin.sym} 820 -230 0 0 {name=l18 sig_type=std_logic lab=IN verilog_type=reg}
C {lab_pin.sym} 990 -310 0 1 {name=l19 sig_type=std_logic lab=OUT3}
C {gnd.sym} 980 -390 0 0 {name=l20 lab=GND}
C {rnmos4.sym} 900 -390 0 0 {name=M6 model=nmos w=5u l=0.18u m=1 del=10}
