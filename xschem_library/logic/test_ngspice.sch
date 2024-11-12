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
G {process
begin
A<='0';
B<='0';
wait for 1 ns;
A<='1';
wait for 1 ns;
B<='1';
wait for 1 ns;
A<='0';
wait for 1 ns;
B<='0';
wait for 1 ns;
B<= '1'; 
wait for 1 ns;
B<= '0'; 
A<= '0'; 
wait for 1 ns;
B<= '1'; 
wait for 1 ns;
A <='1';
wait for 20 ns;
A <= '0';
wait for 1 ns;
A<='Z';
B<='Z';
wait;
end process;

}
K {}
V {integer n = 0;

initial begin
  $dumpfile("dumpfile.vcd");
  $dumpvars;
  A=0;
  B=0;
  #1000;
  A=1;
  #1000;
  B=1;
  #1000;
  A=0;
  #1000;
  B=0;
  #1000;
  B=1;
  #1000;
  B=0;
  A=0;
  #1000;
  B=1;
  #1000;
  A=1;
  #20000;
  A=0;
end
}
S {}
E {}
B 2 130 -1260 1450 -900 {flags=graph
y1=0
y2=2
ypos1=0.0541696
ypos2=1.13488
divy=5
subdivy=1
unity=1
x1=0
x2=2.6e-05
divx=5
subdivx=1
node="a_a
b_a
y_nand_a
y_nor_a
y_nand_ax4x"
color="4 4 4 4 4"
dataset=-1
unitx=1
logx=0
logy=0
digital=1}
B 21 490 -760 1060 -360 {}
T {XSPICE DOMAIN} 600 -800 0 0 0.6 0.6 {}
T {A --> D} 510 -420 0 0 0.6 0.6 {}
T {D --> A} 930 -420 0 0 0.6 0.6 {}
N 570 -480 700 -480 {lab=A}
N 570 -440 700 -440 {lab=B}
N 800 -460 960 -460 {lab=Y_NAND}
N 1020 -460 1090 -460 {lab=Y_NAND_A}
N 670 -620 700 -620 {lab=A}
N 670 -580 700 -580 {lab=~B,~Y_NAND}
N 800 -600 960 -600 {lab=Y_NOR}
N 1020 -600 1090 -600 {lab=Y_NOR_A}
N 460 -480 510 -480 {lab=A_A}
N 460 -440 510 -440 {lab=B_A}
N 480 -730 610 -730 {lab=A}
N 480 -690 610 -690 {lab=B}
N 710 -710 960 -710 {lab=Y_NAND4}
N 1020 -710 1090 -710 {lab=Y_NAND_A[4]}
C {title.sym} 160 -30 0 0 {name=l2}
C {verilog_timescale.sym} 30 -440 0 0 {name=s1 timestep="1ps" precision="1ps" }
C {use.sym} 30 -540 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
C {lab_pin.sym} 460 -480 2 1 {name=p47 lab=A_A verilog_type=reg}
C {lab_pin.sym} 460 -440 2 1 {name=p48 lab=B_A verilog_type=reg}
C {adc_bridge.sym} 540 -480 0 0 {name=a1 delay=1}
C {adc_bridge.sym} 540 -440 0 0 {name=a2 delay=1
device_model=".model adc_buff adc_bridge(in_low = 0.3 in_high = 0.7)"}
C {code_shown.sym} 30 -330 0 0 {name=STIMULI
place=end
vhdl_ignore=true
verilog_ignore=true
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value="
* to generate following file copy .../share/doc/xschem/logic/stimuli.test_ngspice
* to the simulation directory and run simulation -> Utile Stimuli Editor (GUI), 
* and press 'Translate'
.include stimuli_test_ngspice.cir

.control
save all
tran 100n 26u
eprvcd A B Y_NAND  > zzzz.vcd
write test_ngspice.raw
.endc
"}
C {lab_wire.sym} 670 -480 0 1 {name=l3 lab=A}
C {lab_wire.sym} 670 -440 0 1 {name=l4 lab=B}
C {lab_wire.sym} 830 -460 0 1 {name=l5 lab=Y_NAND}
C {nd2.sym} 740 -460 0 0 {name=a3 delay="120 ps" del=120}
C {dac_bridge.sym} 990 -460 0 0 {name=a4 
device_model=".model dac_buff  dac_bridge(out_low = 0 out_high = 1.2 out_undef = 0.6
+ input_load = 5.0e-15 t_rise = 0.5e-9
+ t_fall = 0.2e-9)"}
C {lab_pin.sym} 1090 -460 2 0 {name=p1 lab=Y_NAND_A}
C {netlist_options.sym} 30 -610 0 0 {bus_replacement_char="xx"}
C {lab_pin.sym} 670 -620 2 1 {name=p13 lab=A}
C {lab_pin.sym} 670 -580 2 1 {name=p14 lab=~B,~Y_NAND}
C {lab_wire.sym} 870 -600 0 1 {name=l15 lab=Y_NOR}
C {dac_bridge.sym} 990 -600 0 0 {name=a6 }
C {lab_pin.sym} 1090 -600 2 0 {name=p16 lab=Y_NOR_A}
C {nr2.sym} 740 -600 0 0 {name=a5 delay="200 ps" del=200
device_model=".model nor d_nor(rise_delay = 0.7e-9 fall_delay = 0.2e-9
+ input_load = 5e-15)"
}
C {lab_wire.sym} 580 -730 0 1 {name=l1 lab=A}
C {lab_wire.sym} 580 -690 0 1 {name=l6 lab=B}
C {lab_wire.sym} 740 -710 0 1 {name=l7 lab=Y_NAND4}
C {nd2.sym} 650 -710 0 0 {name=a7 delay="120 ps" del=120

device_model=".model nand d_nand(rise_delay = 0.5e-9 fall_delay = 0.3e-9
+ input_load = 5e-15)"}
C {dac_bridge.sym} 990 -710 0 0 {name=a8 }
C {lab_pin.sym} 1090 -710 2 0 {name=p2 lab=Y_NAND_A[4]}
C {launcher.sym} 530 -870 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/test_ngspice.raw tran"
}
