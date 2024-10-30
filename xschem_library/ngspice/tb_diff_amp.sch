v {xschem version=3.4.6RC file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
B 2 840 -900 1640 -500 {flags=graph
y1=0
y2=6
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=6
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
node="b
a"
color="6 4"
dataset=-1
unitx=1
logx=0
logy=0
}
B 2 840 -500 1640 -100 {flags=graph
y1=-30
y2=30
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=6
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
node=z
color=7
dataset=-1
unitx=1
logx=0
logy=0
}
P 4 5 140 -620 140 -870 710 -870 710 -620 140 -620 {}
P 4 7 410 -620 410 -560 420 -560 410 -540 400 -560 410 -560 410 -620 {}
T {// importing libs

`include "discipline.h"

module diff_amp(
  output electrical out,
  input electrical in1,
  input electrical in2);

parameter real gain = 10; // setting gain to 10 of the differential amplifier

analog begin

    V(out) <+ gain * (V(in1, in2));
    // V(out) <+ 2 * atan( gain / 2 * V(in1, in2) );

end
endmodule} 150 -860 0 0 0.2 0.2 {font=monospace}
T {create a diff_amp.va file with following code 
and compile it into a .osdi file with openvaf.} 190 -930 0 0 0.4 0.4 {}
N 180 -450 320 -450 {lab=B}
N 80 -530 320 -530 {lab=A}
N 520 -490 640 -490 {lab=Z}
N 60 -290 180 -290 {lab=0}
N 180 -330 180 -290 {lab=0}
N 80 -330 80 -290 {lab=0}
N 80 -530 80 -390 {lab=A}
N 180 -450 180 -390 {lab=B}
C {diff_amp.sym} 420 -490 0 0 {name=U1}
C {lab_pin.sym} 640 -490 0 1 {name=p1 sig_type=std_logic lab=Z}
C {lab_pin.sym} 80 -530 0 0 {name=p2 sig_type=std_logic lab=A}
C {lab_pin.sym} 180 -450 0 0 {name=p3 sig_type=std_logic lab=B}
C {vsource.sym} 80 -360 0 0 {name=V1 value=3.1 savecurrent=false}
C {vsource.sym} 180 -360 0 0 {name=V2 value=3 savecurrent=false}
C {lab_pin.sym} 60 -290 0 0 {name=p4 sig_type=std_logic lab=0}
C {code_shown.sym} 240 -320 0 0 {name=COMMANDS only_toplevel=false value="
.options savecurrents
.control
  save all
  op
  remzerovec
  write tb_diff_amp.raw
  dc V1 0 6 0.01
  set appendwrite
  remzerovec
  write tb_diff_amp.raw
  quit 0
.endc
"}
C {launcher.sym} 670 -120 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/tb_diff_amp.raw dc"
}
C {title.sym} 160 -30 0 0 {name=l1 author="Phillip Baade-Pedersen"}
C {launcher.sym} 670 -170 0 0 {name=h1
descr="OP annotate" 
tclcommand="xschem annotate_op"
}
