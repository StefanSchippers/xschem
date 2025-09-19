v {xschem version=3.4.8RC file_version=1.3}
G {}
K {}
V {}
S {}
F {}
E {}
B 2 20 -770 1060 -430 {flags=graph
y1=0
y2=2
ypos1=0.12703394
ypos2=1.7910429
divy=5
subdivy=1
unity=1
x1=-3.9199067e-07
x2=1.9268808e-05
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
node="clk
----
count_out; count_out3,count_out2,count_out1,count_out0
---
count_out3
count_out2
count_out1
count_out0"
color="8 9 11 9 11 11 11 11"
dataset=-1
unitx=1
logx=0
logy=0
digital=1}
T {These capacitors are used to force
auto-creation of dac bridges
(digital count_out --> analog count_out).
Analog nodes can be plotted and saved
in raw file.} 800 -230 0 0 0.3 0.3 {layer=7}
N 50 -110 50 -90 {lab=CLK}
N 440 -310 570 -310 {lab=CLK}
N 710 -310 840 -310 {bus=1 lab=count_out[3..0]}
N 800 -310 800 -270 {lab=count_out[3..0]}
C {vsource.sym} 50 -60 0 0 {name=VCLOCK value="pulse 0 'VCC' 500n 10n 10n 490n 1u"}
C {lab_pin.sym} 50 -30 0 0 {name=p6 lab=0}
C {lab_pin.sym} 50 -110 0 0 {name=p13 lab=CLK}
C {code_shown.sym} 20 -370 0 0 {name=COMMANDS only_toplevel=false value="
.param VCC=1.8

.control
  save all
  tran 10n 50u
  remzerovec
  write tb_counter_wrapper.raw
.endc
"}
C {launcher.sym} 460 -390 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw tran"
}
C {lab_pin.sym} 440 -310 0 0 {name=p3 lab=CLK}
C {lab_pin.sym} 840 -310 0 1 {name=p4 lab=count_out[3..0]}
C {counter.sym} 640 -310 0 0 {name=a1 model=counter

**** put an asteric or any other character before (and no spaces in between)
**** the model you DON'T want to use:

***Verilator***
*device_model=".model counter d_cosim simulation=\\"./counter.so\\""

***Icarus_verilog***
device_model=".model counter d_cosim simulation=\\"ivlng\\" sim_args=[\\"counter\\"]"

tclcommand="edit_file counter.v"}
C {parax_cap.sym} 800 -260 0 0 {name=C2[3..0] gnd=0 value=1f m=1}
C {launcher.sym} 870 -400 0 0 {name=h3
descr="Verilate Design" 
tclcommand="execute 1 sh -c \\"cd $netlist_dir; ngspice vlnggen [abs_sym_path counter.v]\\""}
C {launcher.sym} 870 -360 0 0 {name=h1
descr="Icarusate Design" 
tclcommand="execute 1 sh -c \\"cd $netlist_dir; iverilog -o counter [abs_sym_path counter.v]\\""
}
