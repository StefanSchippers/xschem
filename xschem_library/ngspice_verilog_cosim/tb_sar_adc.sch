v {xschem version=3.4.7RC file_version=1.2}
G {}
K {}
V {}
S {}
E {}
B 2 80 -880 1140 -510 {flags=graph
y1=0
y2=3.3
ypos1=0.27131944
ypos2=4.2824898
divy=5
subdivy=1
unity=1
x1=2.7777218e-05
x2=9.3313218e-05
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
node="d[5..0];d5,d4,d3,d2,d1,d0
valid_a
clk
start
d0
d1
d2
d3
d4
d5
xtest.sample
xtest.comp_a"
color="8 4 4 8 10 10 10 10 10 10 4 20"
dataset=-1
unitx=1
logx=0
logy=0
digital=1
linewidth_mult=1}
B 2 80 -1640 1140 -920 {flags=graph
y1=-0.35
y2=4.9
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=2.7777218e-05
x2=9.3313218e-05
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
dataset=-1
unitx=1
logx=0
logy=0
digital=0
color="4 8 10 9"
node="INPUT
xtest.test_v
i(vamm)
xtest.sample 0.05 *"
linewidth_mult=1
hcursor1_y=1.0550322}
T { A simple DAC so that the result may be compared to the input.} 800 -250 0 0 0.4 0.4 {}
T {Analog conversion for plotting} 220 -140 0 0 0.4 0.4 {}
T {This is an example of a true mixed mode
(analog + Digital) simulation using ngspice 
for the analog part and Icarus Verilog 
(or Verilator) for the verilog part.

Instructions
- You need Verilator and / or Icarus verilog installed.
- Icarus verilog must be built with the --enable-libvvp option.
- Build the icarus / Verilator object
  (use the provided launchers) 
- Edit the sar_adc/sar_adc_vlog.sym attributes.
  There are two device_model attributes. Put an asterisc
  before the device_model attribute you want *NOT* to use.
  Example below enables Verilator object:
     ***Icarus_verilog***
     *device_model=".model dut d_cosim simulation=\\"ivlng\\" sim_args=[\\"adc\\"]"

     ***Verilator***
     device_model=".model dut d_cosim simulation=\\"./adc.so\\""
} 1150 -1470 0 0 0.7 0.7 {}
N 160 -290 160 -270 {lab=CLK}
N 1330 -120 1330 -100 {lab=SUM}
N 280 -290 280 -270 {lab=START}
N 540 -290 540 -270 {lab=INPUT}
N 50 -290 50 -270 {lab=VCC}
N 840 -120 1330 -120 {lab=SUM}
N 840 -210 840 -180 {lab=D5}
N 920 -210 920 -180 {lab=D4}
N 1000 -210 1000 -180 {lab=D3}
N 1080 -210 1080 -180 {lab=D2}
N 1160 -210 1160 -180 {lab=D1}
N 1240 -210 1240 -180 {lab=D0}
N 40 -140 40 -120 {lab=CLK}
C {vsource.sym} 160 -240 0 0 {name=ACLOCK value=""
format="@name @@m @@p clock"
device_model=".model clock d_osc cntl_array=[-1 1] freq_array=[1Meg 1Meg]"
spice_ignore=true}
C {lab_pin.sym} 160 -210 0 0 {name=p25 lab=0}
C {lab_pin.sym} 160 -290 0 0 {name=p34 lab=CLK}
C {ammeter.sym} 1330 -70 0 0 {name=VAMM savecurrent=0 spice_ignore=0}
C {lab_pin.sym} 1330 -40 0 0 {name=p35 lab=0}
C {vsource.sym} 280 -240 0 0 {name=VPULSE value="PULSE 0 \{vcc\} 0.2u 10n 10n 1.3u 10u" savecurrent=false}
C {lab_pin.sym} 280 -210 0 0 {name=p37 lab=0}
C {lab_pin.sym} 280 -290 0 0 {name=p38 lab=START}
C {vsource.sym} 540 -240 0 0 {name=VTEST value="PULSE 0 3 0 200u 200u 1u 401u" savecurrent=false}
C {lab_pin.sym} 540 -210 0 0 {name=p39 lab=0}
C {lab_pin.sym} 540 -290 0 0 {name=p40 lab=INPUT}
C {code_shown.sym} 700 -440 0 0 {name=COMMANDS only_toplevel=false value="
.param VCC=3.3
.control
  save all
  tran 10n 250u
  remzerovec
  write tb_sar_adc.raw
.endc
"}
C {vsource.sym} 50 -240 0 0 {name=VVCC value='VCC' savecurrent=false}
C {lab_pin.sym} 50 -210 0 0 {name=p1 lab=0}
C {lab_pin.sym} 50 -290 0 0 {name=p41 lab=VCC}
C {res.sym} 840 -150 0 0 {name=R2
value=2
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 840 -210 0 0 {name=p42 lab=D5}
C {res.sym} 920 -150 0 0 {name=R3
value=4
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 920 -210 0 0 {name=p43 lab=D4}
C {res.sym} 1000 -150 0 0 {name=R4
value=8
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 1000 -210 0 0 {name=p44 lab=D3}
C {res.sym} 1080 -150 0 0 {name=R5
value=16
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 1080 -210 0 0 {name=p45 lab=D2}
C {res.sym} 1160 -150 0 0 {name=R6
value=32
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 1160 -210 0 0 {name=p46 lab=D1}
C {res.sym} 1240 -150 0 0 {name=R7
value=64
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 1240 -210 0 0 {name=p47 lab=D0}
C {lab_pin.sym} 1330 -120 0 1 {name=p48 lab=SUM}
C {sar_adc.sym} 450 -380 0 0 {name=xtest}
C {lab_pin.sym} 300 -410 0 0 {name=p7 lab=INPUT}
C {lab_pin.sym} 300 -390 0 0 {name=p8 lab=VCC}
C {lab_pin.sym} 300 -370 0 0 {name=p9 lab=START}
C {lab_pin.sym} 600 -410 0 1 {name=p10 lab=VALID}
C {lab_pin.sym} 600 -390 0 1 {name=p11 lab=D[5..0]}
C {lab_pin.sym} 300 -350 0 0 {name=p12 lab=CLK}
C {launcher.sym} 150 -490 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/tb_sar_adc.raw tran"
}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {dac_bridge.sym} 330 -160 0 0 {name=A2 dac_bridge_model= dac_buff

device_model=".model dac_buff dac_bridge input_load=1e-15 t_rise=10n t_fall=10n
+ out_low=0 out_high=3.3"
}
C {lab_pin.sym} 300 -160 0 0 {name=p4 lab=VALID}
C {lab_pin.sym} 360 -160 0 1 {name=p5 lab=VALID_A}
C {vsource.sym} 40 -90 0 0 {name=VCLOCK value="pulse 0 'VCC' 500n 10n 10n 490n 1u"}
C {lab_pin.sym} 40 -60 0 0 {name=p6 lab=0}
C {lab_pin.sym} 40 -140 0 0 {name=p13 lab=CLK}
C {launcher.sym} 1410 -390 0 0 {name=h1
descr="Build Icarus Verilog object" 
tclcommand="execute 1 sh -c \\"cd $netlist_dir; iverilog -o adc [abs_sym_path adc.v]\\""
}
C {launcher.sym} 1410 -340 0 0 {name=h2
descr="Build Verilator object" 
tclcommand="execute 1 sh -c \\"cd $netlist_dir; ngspice vlnggen [abs_sym_path adc.v]\\""
}
