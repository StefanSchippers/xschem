v {xschem version=2.9.7 file_version=1.2}
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
S {.model adc_buff adc_bridge(in_low = 0.3 in_high = 0.7)


.model nand d_nand(rise_delay = 0.5e-9 fall_delay = 0.3e-9
+ input_load = 5e-15)

.model nor d_nor(rise_delay = 0.7e-9 fall_delay = 0.2e-9
+ input_load = 5e-15)

.model dac_buff  dac_bridge(out_low = 0 out_high = 1.2 out_undef = 0.6
+ input_load = 5.0e-15 t_rise = 0.5e-9
+ t_fall = 0.2e-9)
}
E {}
B 21 490 -550 1060 -260 {}
T {XSPICE DOMAIN} 600 -590 0 0 0.6 0.6 {}
T {A --> D} 510 -320 0 0 0.6 0.6 {}
T {D --> A} 950 -320 0 0 0.6 0.6 {}
N 570 -380 700 -380 {lab=A}
N 570 -340 700 -340 {lab=B}
N 800 -360 980 -360 {lab=Y_NAND}
N 1040 -360 1090 -360 {lab=Y_NAND_A}
N 670 -520 700 -520 {lab=A}
N 670 -480 700 -480 {lab=~B,~Y_NAND}
N 800 -500 980 -500 {lab=Y_NOR}
N 1040 -500 1090 -500 {lab=Y_NOR_A}
N 460 -380 510 -380 {lab=A_A}
N 460 -340 510 -340 {lab=B_A}
N 480 -720 610 -720 {lab=A}
N 480 -680 610 -680 {lab=B}
N 710 -700 890 -700 {lab=Y_NAND4}
N 950 -700 1000 -700 {lab=Y_NAND_A[4]}
C {title.sym} 160 -30 0 0 {name=l2}
C {verilog_timescale.sym} 30 -340 0 0 {name=s1 timestep="1ps" precision="1ps" }
C {use.sym} 30 -440 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
C {lab_pin.sym} 460 -380 2 1 {name=p47 lab=A_A verilog_type=reg}
C {lab_pin.sym} 460 -340 2 1 {name=p48 lab=B_A verilog_type=reg}
C {adc_bridge.sym} 540 -380 0 0 {name=a1 delay=1}
C {adc_bridge.sym} 540 -340 0 0 {name=a2 delay=1}
C {code_shown.sym} 30 -260 0 0 {name=STIMULI
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
tran 100n 26u
eprvcd A B Y_NAND  > zzzz.vcd
.endc
"}
C {lab_wire.sym} 670 -380 0 1 {name=l3 lab=A}
C {lab_wire.sym} 670 -340 0 1 {name=l4 lab=B}
C {lab_wire.sym} 830 -360 0 1 {name=l5 lab=Y_NAND}
C {nd2.sym} 740 -360 0 0 {name=a3 delay="120 ps" del=120}
C {dac_bridge.sym} 1010 -360 0 0 {name=a4 }
C {lab_pin.sym} 1090 -360 2 0 {name=p1 lab=Y_NAND_A}
C {netlist_options.sym} 30 -510 0 0 {bus_replacement_char="xx"}
C {lab_pin.sym} 670 -520 2 1 {name=p13 lab=A}
C {lab_pin.sym} 670 -480 2 1 {name=p14 lab=~B,~Y_NAND}
C {lab_wire.sym} 870 -500 0 1 {name=l15 lab=Y_NOR}
C {dac_bridge.sym} 1010 -500 0 0 {name=a6 }
C {lab_pin.sym} 1090 -500 2 0 {name=p16 lab=Y_NOR_A}
C {nr2.sym} 740 -500 0 0 {name=a5 delay="200 ps" del=200}
C {lab_wire.sym} 580 -720 0 1 {name=l1 lab=A}
C {lab_wire.sym} 580 -680 0 1 {name=l6 lab=B}
C {lab_wire.sym} 740 -700 0 1 {name=l7 lab=Y_NAND4}
C {nd2.sym} 650 -700 0 0 {name=a7 delay="120 ps" del=120}
C {dac_bridge.sym} 920 -700 0 0 {name=a8 }
C {lab_pin.sym} 1000 -700 2 0 {name=p2 lab=Y_NAND_A[4]}
C {assign.sym} 1280 -470 0 0 {name=v1 delay=1}
C {lab_pin.sym} 1250 -470 2 1 {name=p3 lab=Y_NOR_A}
C {lab_pin.sym} 1310 -470 2 0 {name=p4 lab=Y_NOR[3]}
