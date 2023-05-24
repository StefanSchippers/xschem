v {xschem version=3.4.0 file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
B 2 590 -730 1390 -330 {flags=graph,unlocked
y1=1.5e-42
y2=0.0028
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=-0.308107
x2=19.6819
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
node=i(@j1[id])
color=4
dataset=-1
unitx=1
logx=0
logy=0
rainbow=0}
N 340 -300 340 -250 {
lab=#net1}
N 340 -190 340 -110 {
lab=GND}
N 260 -220 300 -220 {
lab=#net2}
N 150 -220 200 -220 {
lab=G}
N 340 -420 340 -360 {
lab=D}
C {title.sym} 160 -30 0 0 {name=l9 

author="tcleval(Stefan Schippers[
  if \{$show_pin_net_names == 0\} \{
    set show_pin_net_names 1
    xschem update_all_sym_bboxes
  \}]
)"}
C {njfet.sym} 320 -220 0 0 {name=J1 model=2N3459 area=1 m=1
}
C {lab_pin.sym} 150 -220 0 0 {name=p1 sig_type=std_logic lab=G}
C {lab_pin.sym} 340 -420 0 0 {name=p2 sig_type=std_logic lab=D}
C {lab_pin.sym} 340 -110 0 0 {name=p3 sig_type=std_logic lab=GND}
C {code_shown.sym} 580 -280 0 0 {name=s1 only_toplevel=false value="VG G 0 dc 0
VD D 0 dc 0

.options savecurrents
.control
  save all
  dc VD 0 20 0.01 VG 0 -1 -0.1
  write test_jfet.raw
.endc"}
C {code.sym} 70 -400 0 0 {name=MODEL only_toplevel=false value=".MODEL 2N3459 NJF(VTO=-1.4 BETA=1.265m BETATCE=-0.5 LAMBDA=4m RD=1 RS=1 CGS=2.916p CGD=2.8p PB=0.5 IS=114.5f XTI=3 AF=1 FC=0.5 N=1 NR=2)"}
C {noconn.sym} 340 -140 0 0 {name=l1}
C {noconn.sym} 340 -390 0 0 {name=l2}
C {noconn.sym} 180 -220 1 0 {name=l3}
C {launcher.sym} 980 -290 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/test_jfet.raw dc"
}
C {ammeter.sym} 230 -220 1 0 {name=Vgate}
C {ammeter.sym} 340 -330 0 0 {name=Vdrain}
