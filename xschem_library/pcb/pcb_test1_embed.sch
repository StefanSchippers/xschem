v {xschem version=2.9.5_RC5 file_version=1.1}
G {}
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
C {title.sym} 160 -30 0 0 {name=l2 author="Stefan" embed=true}
[
v {xschem version=2.9.5_RC5 file_version=1.1}
G {type=logo
template="name=l1 author=\\"Stefan Schippers\\""
verilog_ignore=true
vhdl_ignore=true
spice_ignore=true
tedax_ignore=true}
V {}
S {}
E {}
L 6 225 0 1020 0 {}
L 6 -160 0 -95 0 {}
T {@schname} 235 5 0 0 0.4 0.4 {}
T {@author} 235 -25 0 0 0.4 0.4 {}
T {@time_last_modified} 1020 -20 0 1 0.4 0.3 {}
T {SCHEM} 5 -25 0 0 1 1 {}
P 5 13 5 -30 -25 0 5 30 -15 30 -35 10 -55 30 -75 30 -45 0 -75 -30 -55 -30 -35 -10 -15 -30 5 -30 {fill=true}
]
C {74ls00.sym} 420 -350 0 0 {name=U1:2  risedel=100 falldel=200 embed=true}
[
v {xschem version=2.9.5_RC5 file_version=1.1}
G {type=nand
verilog_format="nand #(@risedel , @falldel ) @name ( @#2 , @#0 , @#1 );"
tedax_format="footprint @name @footprint
device @name @symname"
template="name=U1 footprint=\\"dip(14)\\" risedel=100 falldel=200 numslots=4 power=VCC ground=GND"
extra="power ground"
extra_pinnumber="14 7"}
V {}
S {}
E {}
L 4 -40 -20 -25 -20 {}
L 4 -25 -30 -25 30 {}
L 4 -25 -30 5 -30 {}
L 4 -25 30 5 30 {}
L 4 -40 20 -25 20 {}
L 4 45 0 60 0 {}
B 5 -42.5 -22.5 -37.5 -17.5 {name=A dir=in pinnumber=1:4:9:12}
B 5 -42.5 17.5 -37.5 22.5 {name=B dir=in pinnumber=2:5:10:13}
B 5 57.5 -2.5 62.5 2.5 {name=Z dir=out verilog_type=wire pinnumber=3:6:8:11}
A 4 5 0 30 270 180 {}
A 4 40 0 5 0 360 {}
T {@name} -20 -15 0 0 0.2 0.2 {}
T {@symname} -22.5 0 0 0 0.2 0.2 {}
T {@#0:pinnumber} -22.5 -22.5 0 0 0.12 0.12 {}
T {@#1:pinnumber} -22.5 17.5 0 0 0.12 0.12 {}
T {@#2:pinnumber} 32.5 -2.5 0 1 0.12 0.12 {}
]
C {74ls00.sym} 870 -440 0 0 {name=U1:1  risedel=100 falldel=200}
C {lab_pin.sym} 970 -440 0 1 {name=p0 lab=OUTPUT_Y}
C {capa.sym} 590 -160 0 0 {name=C0 m=1 value=100u device="electrolitic capacitor" embed=true}
[
v {xschem version=2.9.5_RC5 file_version=1.1}
G {type=capacitor
format="@name @pinlist @value m=@m"
tedax_format="footprint @name @footprint 
value @name @value
device @name @device"
verilog_ignore=true
template="name=C0 m=1 value=1p footprint=1206 device=\\"ceramic capacitor\\""}
V {}
S {}
E {}
L 4 0 5 0 30 {}
L 4 0 -30 0 -5 {}
L 4 -10 -5 10 -5 {}
L 4 -10 5 10 5 {}
L 4 2.5 -22.5 7.5 -22.5 {}
L 4 5 -25 5 -20 {}
B 5 -2.5 -32.5 2.5 -27.5 {name=p dir=in pinnumber=1}
B 5 -2.5 27.5 2.5 32.5 {name=m dir=in pinnumber=2}
T {@value} 15 -5 0 0 0.25 0.2 {}
T {@name} 15 -18.75 0 0 0.2 0.2 {}
T {m=@m} 15 10 0 0 0.25 0.2 {}
T {@#0:pinnumber} -2.5 -25 0 1 0.12 0.12 {}
T {@#1:pinnumber} -2.5 20 0 1 0.12 0.12 {}
]
C {74ls00.sym} 420 -460 0 0 {name=U1:4 risedel=100 falldel=200 power=VCC5
url="http://www.engrcs.com/components/74LS00.pdf"}
C {7805.sym} 730 -190 0 0 {name=U0 url="https://www.sparkfun.com/datasheets/Components/LM7805.pdf" embed=true}
[
v {xschem version=2.9.5_RC5 file_version=1.1}
G {type=regulator
format="x@name @pinlist r@symname"
verilog_format="assign @#2 = @#0 ;"
tedax_format="footprint @name @footprint
device @name @device"
template="name=U1 device=7805 footprint=TO220"}
V {}
S {}
E {}
L 4 -60 0 -50 0 {}
L 4 50 0 60 0 {}
L 4 -50 -20 50 -20 {}
L 4 50 -20 50 20 {}
L 4 -50 20 50 20 {}
L 4 -50 -20 -50 20 {}
L 4 0 20 0 30 {}
B 5 -62.5 -2.5 -57.5 2.5 {name=IN dir=in pinnumber=1}
B 5 -2.5 27.5 2.5 32.5 {name=GND dir=inout pinnumber=2}
B 5 57.5 -2.5 62.5 2.5 {name=OUT dir=out pinnumber=3}
T {@name} -17.5 -15 0 0 0.2 0.2 {}
T {@device} -17.5 0 0 0 0.2 0.2 {}
T {@#0:pinnumber} -47.5 -2.5 0 0 0.12 0.12 {}
T {@#1:pinnumber} -2.5 12.5 0 0 0.12 0.12 {}
T {@#2:pinnumber} 47.5 -2.5 0 1 0.12 0.12 {}
]
C {lab_pin.sym} 490 -190 0 0 {name=p20 lab=VCC12}
C {lab_pin.sym} 940 -190 0 1 {name=p22 lab=VCC5}
C {lab_pin.sym} 590 -110 0 0 {name=p23 lab=ANALOG_GND}
C {capa.sym} 890 -160 0 0 {name=C4 m=1 value=10u device="tantalium capacitor"}
C {res.sym} 520 -190 1 0 {name=R0 m=1 value=4.7 device="carbon resistor" embed=true}
[
v {xschem version=2.9.5_RC5 file_version=1.1}
G {type=resistor
format="@name @pinlist @value m=@m"
verilog_format="tran @name ( @#0 , @#1 ) ;"
tedax_format="footprint @name @footprint
value @name @value
device @name @device"
template="name=R0 m=1 value=1k footprint=1206 device=resistor"}
V {}
S {}
E {}
L 4 0 20 0 30 {}
L 4 0 20 7.5 17.5 {}
L 4 -7.5 12.5 7.5 17.5 {}
L 4 -7.5 12.5 7.5 7.5 {}
L 4 -7.5 2.5 7.5 7.5 {}
L 4 -7.5 2.5 7.5 -2.5 {}
L 4 -7.5 -7.5 7.5 -2.5 {}
L 4 -7.5 -7.5 7.5 -12.5 {}
L 4 -7.5 -17.5 7.5 -12.5 {}
L 4 -7.5 -17.5 0 -20 {}
L 4 0 -30 0 -20 {}
L 4 2.5 -22.5 7.5 -22.5 {}
L 4 5 -25 5 -20 {}
B 5 -2.5 -32.5 2.5 -27.5 {name=p dir=inout goto=1 pinnumber=1}
B 5 -2.5 27.5 2.5 32.5 {name=m dir=inout goto=0 pinnumber=2}
T {@name} 15 -18.75 0 0 0.2 0.2 {}
T {@value} 15 -3.75 0 0 0.2 0.2 {}
T {m=@m} 15 11.25 0 0 0.2 0.2 {}
T {@#0:pinnumber} -5 -25 0 1 0.12 0.12 {}
T {@#1:pinnumber} -5 20 0 1 0.12 0.12 {}
]
C {lab_wire.sym} 700 -460 0 0 {name=l3 lab=A}
C {lab_wire.sym} 700 -420 0 0 {name=l0 lab=B}
C {lab_wire.sym} 650 -190 0 0 {name=l1 lab=VCCFILT embed=true}
[
v {xschem version=2.9.5_RC5 file_version=1.1}
G {type=label
format="*.alias @lab"
template="name=l1 sig_type=std_logic lab=xxx"}
V {}
S {}
E {}
B 5 -1.25 -1.25 1.25 1.25 {name=p dir=in}
T {@lab} -3.75 -18.75 0 1 0.33 0.27 {}
]
C {lab_pin.sym} 310 -370 0 0 {name=lONN1 lab=INPUT_A verilog_type=reg}
C {lab_pin.sym} 310 -330 0 0 {name=lONN2 lab=INPUT_B verilog_type=reg}
C {lab_pin.sym} 40 -330 0 1 { name=lONN3 lab=OUTPUT_Y }
C {lab_pin.sym} 310 -480 0 0 {name=lONN6 lab=INPUT_E verilog_type=reg}
C {lab_pin.sym} 310 -440 0 0 {name=lONN8 lab=INPUT_F verilog_type=reg}
C {lab_pin.sym} 40 -310 0 1 { name=lONN9 lab=VCC12 }
C {lab_pin.sym} 40 -290 0 1 { name=lONN14 lab=ANALOG_GND  verilog_type=reg}
C {lab_pin.sym} 40 -270 0 1 { name=lONN15 lab=GND  verilog_type=reg embed=true}
[
v {xschem version=2.9.5_RC5 file_version=1.1}
G {type=label
format="*.alias @lab"
template="name=l1 sig_type=std_logic lab=xxx"}
V {}
S {}
E {}
B 5 -1.25 -1.25 1.25 1.25 {name=p dir=in}
T {@lab} -7.5 -7.5 0 1 0.36 0.33 {}
]
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

" embed=true}
[
v {xschem version=2.9.5_RC5 file_version=1.1}
G {type=netlist_commands
template="name=s1 only_toplevel=false value=blabla"
tedax_ignore=true
format="
@value
"}
V {}
S {}
E {}
L 3 20 30 60 30 {}
L 3 20 40 40 40 {}
L 3 20 50 60 50 {}
L 3 20 60 50 60 {}
L 3 20 70 50 70 {}
L 3 20 80 90 80 {}
L 3 20 90 40 90 {}
L 3 20 20 70 20 {}
L 3 20 10 40 10 {}
L 5 100 10 110 10 {}
L 5 110 10 110 110 {}
L 5 20 110 110 110 {}
L 5 20 100 20 110 {}
L 5 100 0 100 100 {}
L 5 10 100 100 100 {}
L 5 10 0 10 100 {}
L 5 10 0 100 0 {}
T {@name} 15 -25 0 0 0.3 0.3 {}
]
C {verilog_timescale.sym} 1050 -100 0 0 {name=s1 timestep="1ns" precision="1ns"  embed=true}
[
v {xschem version=2.9.5_RC5 file_version=1.1}
G {type=timescale
spice_ignore=true
template="name=s1 timestep=\\"100ps\\" precision=\\"100ps\\" "
verilog_format="`timescale @timestep/@precision"}
V {}
S {}
E {}
L 4 0 -10 70 -10 {}
L 4 0 -10 0 10 {}
T {TIMESCALE} 5 -25 0 0 0.3 0.3 {}
T {`timescale @timestep/@precision} 15 -5 0 0 0.3 0.3 {}
]
C {conn_8x1.sym} 20 -390 0 0 {name=C1 footprint=connector(8,1) embed=true}
[
v {xschem version=2.9.5_RC5 file_version=1.1}
G {type=connector
verilog_ignore=true
format="*connector(8,1) @pinlist"
tedax_format="footprint @name @footprint"
template="name=C1 footprint=connector(8,1)"
}
V {}
S {}
E {}
B 5 18.75 -21.25 21.25 -18.75 {name=conn_1 dir=inout pinnumber=1}
B 5 18.75 -1.25 21.25 1.25 {name=conn_2 dir=inout pinnumber=2}
B 5 18.75 18.75 21.25 21.25 {name=conn_3 dir=inout pinnumber=3}
B 5 18.75 38.75 21.25 41.25 {name=conn_4 dir=inout pinnumber=4}
B 5 18.75 58.75 21.25 61.25 {name=conn_5 dir=inout pinnumber=5}
B 5 18.75 78.75 21.25 81.25 {name=conn_6 dir=inout pinnumber=6}
B 5 18.75 98.75 21.25 101.25 {name=conn_7 dir=inout pinnumber=7}
B 5 18.75 118.75 21.25 121.25 {name=conn_8 dir=inout pinnumber=8}
A 4 15 -20 5 270 360 {}
A 4 15 0 5 270 360 {}
A 4 15 20 5 270 360 {}
A 4 15 40 5 270 360 {}
A 4 15 60 5 270 360 {}
A 4 15 80 5 270 360 {}
A 4 15 100 5 270 360 {}
A 4 15 120 5 270 360 {}
T {@#0:pinnumber} 6.25 -21.25 0 1 0.1 0.1 {}
T {@#1:pinnumber} 6.25 -1.25 0 1 0.1 0.1 {}
T {@#2:pinnumber} 6.25 18.75 0 1 0.1 0.1 {}
T {@name} -18.75 -43.75 0 0 0.2 0.2 {}
T {@#3:pinnumber} 6.25 38.75 0 1 0.1 0.1 {}
T {@#4:pinnumber} 6.25 58.75 0 1 0.1 0.1 {}
T {@#5:pinnumber} 6.25 78.75 0 1 0.1 0.1 {}
T {@#6:pinnumber} 6.25 98.75 0 1 0.1 0.1 {}
T {@#7:pinnumber} 6.25 118.75 0 1 0.1 0.1 {}
P 4 5 10 130 -10 130 -10 -30 10 -30 10 130 {}
]
C {lab_pin.sym} 40 -370 0 1 {name=l4 lab=INPUT_A verilog_type=reg}
C {lab_pin.sym} 40 -350 0 1 {name=l5 lab=INPUT_B verilog_type=reg}
C {lab_pin.sym} 40 -410 0 1 {name=l6 lab=INPUT_E verilog_type=reg}
C {lab_pin.sym} 40 -390 0 1 {name=l7 lab=INPUT_F verilog_type=reg}
