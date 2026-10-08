v {xschem version=3.4.8RC file_version=1.3}
G {}
K {}
V {}
S {}
F {}
E {}
B 2 600 -440 1000 -40 {flags=graph
y1=0.071
y2=1
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=50000
x2=6e+08
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
legendmag=1.0
node="s_1_1
s_2_1"
color="4 12"
dataset=-1
unitx=1
logx=0
logy=0
autoload=1
sim_type=sp
mode=Smith
smz0=50
rawfile=$netlist_dir/SC_Test.raw}
B 2 1010 -440 1410 -40 {flags=graph
y1=0.071
y2=1
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=50000
x2=6e+08
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
legendmag=1.0
node="Zin; net1 i(v2) / -1 * imp()"
color=9
dataset=-1
unitx=1
logx=0
logy=0
autoload=1
sim_type=sp
mode=Smith
smz0=50
rawfile=$netlist_dir/SC_Test_AC.raw}
N 320 -130 320 -120 {lab=0}
N 420 -130 420 -120 {lab=0}
N 320 -220 350 -220 {lab=#net1}
N 420 -220 430 -220 {lab=#net2}
N 490 -220 500 -220 {lab=0}
N 500 -220 500 -200 {lab=0}
N 420 -220 420 -190 {lab=#net2}
N 410 -220 420 -220 {lab=#net2}
N 420 -270 430 -270 {lab=#net2}
N 420 -270 420 -220 {lab=#net2}
N 490 -270 500 -270 {lab=0}
N 500 -270 500 -220 {lab=0}
N 320 -220 320 -190 {lab=#net1}
C {simulator_commands.sym} 150 -300 0 0 {name=COMMANDS
simulator=ngspice
only_toplevel=true 
value=".temp 30
.option savecurrents
.save all
.control
  save all

  sp lin 1000 50k 600meg

  remzerovec

  write SC_Test.raw

  *** small-signal AC
  save all
  AC lin 1000 50k 600Meg
  remzerovec
  write $inputdir/SC_Test_AC.raw all
  quit 0
.endc

"}
C {vsource.sym} 320 -160 0 1 {name=V2 value="dc 0 ac 1 0 portnum=1 z0=50" savecurrent=false}
C {gnd.sym} 320 -120 0 0 {name=l4 lab=0}
C {vsource.sym} 420 -160 0 0 {name=V1 value="dc 0 ac 0 0 portnum=2 z0=50" savecurrent=false}
C {gnd.sym} 420 -120 0 1 {name=l2 lab=0}
C {capa.sym} 380 -220 3 0 {name=C2
m=1
value=100p
footprint=1206
device="ceramic capacitor"}
C {ind.sym} 460 -220 1 0 {name=L5
m=1
value=68n
footprint=1206
device=inductor}
C {gnd.sym} 500 -200 0 0 {name=l6 lab=0}
C {res.sym} 460 -270 3 0 {name=R2
value=100k
footprint=1206
device=resistor
m=1}
