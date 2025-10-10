v {xschem version=3.4.8RC file_version=1.3}
G {}
K {}
V {}
S {}
F {}
E {}
B 2 540 -740 1200 -340 {flags=graph
y1=0
y2=3.1
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=5e-08
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
node="a
z1
z2"
color="7 1 8"
dataset=-1
unitx=1
logx=0
logy=0
}
T {Test of *resolved* parameter passing
down to subcircuits.
Xschem does all parameter expression
evaluation. Subcircuits contains only
numeric literals. See the 'DEL' parameter.
every instance needs a unique
schematic=... attribute.} 10 -740 0 0 0.4 0.4 {layer=1}
T {Value of voltage
source is set with
the TCL 'VCC' variable} 480 -140 0 1 0.3 0.3 {}
T {Note also the VCC parameter is defined
as a spice .param and also as a TCL variable.
This is another method to pass parameters
to various parts of the circuit hierarchy.} 10 -500 0 0 0.4 0.4 {layer=8}
N 520 -150 600 -150 {lab=A}
N 520 -150 520 -140 {lab=A}
N 900 -150 950 -150 {lab=Z2}
N 900 -250 950 -250 {lab=Z1}
N 520 -250 600 -250 {lab=A}
N 520 -250 520 -150 {lab=A}
C {code_shown.sym} 0 -340 0 0 {name=COMMANDS only_toplevel=false value="tcleval(
.options savecurrents
** following .param is set with a TCL command
.param VCC=[set VCC 3]
.control
  save all
  tran 10p 50n
  write tb_test_evaluated_param.raw
.endc
)"}
C {test_evaluated_param.sym} 750 -150 0 0 {name=x1 DEL=5
schematic=test_evaluated_param2.sch}
C {lab_pin.sym} 950 -150 0 1 {name=p1 lab=Z2}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {vsource.sym} 520 -110 0 0 {name=V1 value="tcleval(pwl 0 0 10n 0 11n $VCC)" savecurrent=false}
C {gnd.sym} 520 -80 0 0 {name=l2 lab=GND}
C {lab_pin.sym} 520 -150 0 0 {name=p2 lab=A}
C {test_evaluated_param.sym} 750 -250 0 0 {name=x2 DEL=2
schematic=test_evaluated_param1.sch}
C {lab_pin.sym} 950 -250 0 1 {name=p3 lab=Z1}
C {launcher.sym} 960 -310 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/tb_test_evaluated_param.raw tran"
}
