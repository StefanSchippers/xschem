v {xschem version=3.4.8RC file_version=1.3}
G {}
K {}
V {}
S {}
F {}
E {}
B 2 40 -890 1080 -550 {flags=graph
y1=0
y2=2
ypos1=0.12703394
ypos2=1.7910429
divy=5
subdivy=1
unity=1
x1=7.3755098e-15
x2=4e-05
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
B 2 40 -1220 1080 -890 {flags=graph
y1=0
y2=0.43
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=7.3755098e-15
x2=4e-05
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
dataset=-1
unitx=1
logx=0
logy=0
color=4
node=i(vamm)
hilight_wave=-1}
T {These capacitors are used to force
auto-creation of dac bridges
(digital count_out --> analog count_out).
Analog nodes can be plotted and saved
in raw file.} 880 -400 0 0 0.3 0.3 {layer=7}
N 70 -230 70 -210 {lab=CLK}
N 410 -430 460 -430 {lab=CLK}
N 600 -430 860 -430 {bus=1 lab=count_out[3..0]}
N 820 -430 820 -390 {lab=count_out[3..0]}
N 1210 -150 1210 -130 {lab=SUM}
N 790 -150 1210 -150 {lab=SUM}
N 790 -240 790 -210 {lab=count_out3}
N 910 -240 910 -210 {lab=count_out2}
N 1030 -240 1030 -210 {lab=count_out1}
N 1150 -240 1150 -210 {lab=count_out0}
C {vsource.sym} 70 -180 0 0 {name=VCLOCK value="pulse 0 'VCC' 500n 10n 10n 490n 1u"}
C {lab_pin.sym} 70 -150 0 0 {name=p6 lab=0}
C {lab_pin.sym} 70 -230 0 0 {name=p13 lab=CLK}
C {code_shown.sym} 40 -490 0 0 {name=COMMANDS only_toplevel=false value="
.param VCC=1.8

.control
  save all
  tran 10n 50u
  remzerovec
  write tb_counter_wrapper.raw
.endc
"}
C {launcher.sym} 480 -510 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw tran"
}
C {lab_pin.sym} 410 -430 0 0 {name=p3 lab=CLK}
C {lab_pin.sym} 860 -430 0 1 {name=p4 lab=count_out[3..0]}
C {counter.sym} 530 -430 0 0 {name=a1 model=counter

**** put an asteric or any other character before (and no spaces in between)
**** the model you DON'T want to use:

***Verilator***
*device_model=".model counter d_cosim simulation=\\"./counter.so\\""

***Icarus_verilog***
device_model=".model counter d_cosim simulation=\\"ivlng\\" sim_args=[\\"counter\\"]"

tclcommand="edit_file counter.v"}
C {parax_cap.sym} 820 -380 0 0 {name=C2[3..0] gnd=0 value=1f m=1}
C {launcher.sym} 890 -490 0 0 {name=h3
descr="Verilate Design" 
tclcommand="execute 1 sh -c \\"cd $netlist_dir; ngspice vlnggen [abs_sym_path counter.v]\\""}
C {launcher.sym} 890 -530 0 0 {name=h1
descr="Icarusate Design" 
tclcommand="execute 1 sh -c \\"cd $netlist_dir; iverilog -o counter [abs_sym_path counter.v]\\""
}
C {ammeter.sym} 1210 -100 0 0 {name=VAMM savecurrent=0 spice_ignore=0}
C {lab_pin.sym} 1210 -70 0 0 {name=p35 lab=0}
C {res.sym} 790 -180 0 0 {name=R4
value=8
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 790 -240 0 0 {name=p44 lab=count_out3}
C {res.sym} 910 -180 0 0 {name=R5
value=16
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 910 -240 0 0 {name=p45 lab=count_out2}
C {res.sym} 1030 -180 0 0 {name=R6
value=32
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 1030 -240 0 0 {name=p46 lab=count_out1}
C {res.sym} 1150 -180 0 0 {name=R7
value=64
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 1150 -240 0 0 {name=p47 lab=count_out0}
C {lab_pin.sym} 1210 -150 0 1 {name=p48 lab=SUM}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
