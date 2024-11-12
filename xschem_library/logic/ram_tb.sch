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
V {
integer tck = 10000; // 10 ns


task cycle;
input [7:0] add;
input cen;
input oen;
input wen;
input [15:0] din;
begin
  CEN = cen;
  OEN = oen;
  WEN = wen;
  ADD = add;
  if(wen == 0) DIN = din;
  else DIN = 16'hxxxx;
  #(tck/4);
  CK=1;
  #(tck/2);
  CK=0;
  #(tck/4);
end endtask


initial begin
  $dumpfile("dumpfile.vcd");
  $dumpvars(0, ram_tb);

  CK = 0;
  WEN = 1;
  OEN = 1;
  CEN = 1;
  ADD = 8'h00;
  DIN = 16'h 0000;
  M = 16'h0000;

  //// read cycles
  //     add  cen oen wen    din
  cycle('h00,  0,  0,  1, 16'h0000);
  cycle('h01,  0,  0,  1, 16'h0000);
  cycle('h02,  0,  0,  1, 16'h0000);
  
  ///// write cycle
  //     add  cen oen wen    din
  cycle('h01,  0,  1,  0, 16'h4444);

  //// read cycles
  //     add  cen oen wen    din
  cycle('h00,  0,  0,  1, 16'h0000);
  cycle('h01,  0,  0,  1, 16'h0000);
  cycle('h02,  0,  0,  1, 16'h0000);


  $finish;
end
}
S {}
E {}
B 0 40 -940 1260 -270 {}
B 19 40 -940 1260 -270 {}
T {parameters:

dim:          number of address bits
width:        number of data bits
hex:         -1 -> do not preset memory with datafile
              0 -> preset memory with binary datafile 
              1 -> preset memory with hex datafile
datafile:     name of memory initialization file
access_delay: from clock rising edge to internal data ready
oe_delay:     output buffer delay (OEN low to DOUT ready)
modulename:   a string that is printed during simulation

total access time from clock positive edge is access_delay + oe_delay
Memory initialization file contains just one word per line (start from address 0)

HEX        BINARY
----------------------------
1234       0001001000110100
abcd       1010101111001101
ffaa       1111111110101010
....       ................
....       ................
} 70 -920 0 0 0.4 0.4 {font=monospace}
T {Verilog example of parametric memory module} 50 -1020 0 0 1 1 {}
C {ram.sym} 740 -160 0 0 {name=x1 
dim=8 
width=16 
hex=1 
datafile=ram.list 
modulename=ram
access_delay=3000 
oe_delay=300}
C {lab_pin.sym} 890 -220 0 1 {name=p1 lab=DOUT[15:0]}
C {verilog_timescale.sym} 40 -137.5 0 0 {name=s1 timestep="1ps" precision="1ps" }
C {lab_pin.sym} 590 -220 0 0 {name=p9 lab=ADD[7:0] verilog_type=reg}
C {lab_pin.sym} 590 -180 0 0 {name=p10 lab=DIN[15:0] verilog_type=reg}
C {lab_pin.sym} 590 -160 0 0 {name=p11 lab=WEN verilog_type=reg}
C {lab_pin.sym} 590 -120 0 0 {name=p12 lab=OEN verilog_type=reg}
C {lab_pin.sym} 590 -100 0 0 {name=p13 lab=CK verilog_type=reg}
C {lab_pin.sym} 590 -140 0 0 {name=p14 lab=CEN verilog_type=reg}
C {lab_pin.sym} 590 -200 0 0 {name=p15 lab=M[15:0] verilog_type=reg}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
