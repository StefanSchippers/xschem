v {xschem version=3.1.0 file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
B 2 560 -400 1170 -150 {flags=graph
y1=0
y2=3
ypos1=0.157569
ypos2=1.98753
divy=5
subdivy=1
unity=1
x1=1e-11
x2=3e-07
divx=5
subdivx=1
node="in
in_inv
in_buf
in_inv2
in_buf2"
color="7 4 8 9 10"
dataset=-1
unitx=1
logx=0
logy=0
digital=1}
T {The two below symbols are created by a 'symbolgen' script
that takes a 'buf' or 'inv' argument.} 200 -640 0 0 0.6 0.6 {}
T {Click on symbol with Control key
pressed to see the generator script} 130 -340 0 0 0.3 0.3 {}
N 70 -250 70 -180 {
lab=IN}
N 70 -250 150 -250 {
lab=IN}
N 70 -180 70 -120 {
lab=IN}
N 70 -120 150 -120 {
lab=IN}
N 230 -250 350 -250 {
lab=IN_BUF}
N 230 -120 350 -120 {
lab=IN_INV}
N 600 -470 630 -470 {
lab=IN}
N 710 -470 740 -470 {
lab=IN_INV2}
N 890 -470 920 -470 {
lab=IN}
N 1000 -470 1030 -470 {
lab=IN_BUF2}
N 500 -310 500 -280 {
lab=VCC}
C {symbolgen(inv)} 190 -120 0 0 {name=x1  
tclcommand="edit_file [abs_sym_path symbolgen]"
ROUT=1200}
C {lab_pin.sym} 70 -180 0 0 {name=p1 lab=IN}
C {symbolgen(buf)} 190 -250 0 0 {name=x3  
tclcommand="edit_file [abs_sym_path symbolgen]"
ROUT=1200}
C {lab_pin.sym} 350 -250 0 1 {name=p2 lab=IN_BUF}
C {lab_pin.sym} 350 -120 0 1 {name=p3 lab=IN_INV}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {code_shown.sym} 40 -540 0 0 {name=CONTROL 
tclcommand="xschem edit_vi_prop"
xxplace=end
value=".include models_rom8k.txt
.param vcc=3
vvcc vcc 0 dc 3
Vin in 0 pwl 0 0 100n 0 100.1n 3 200n 3 200.1n 0
.control
  save all
  tran 1n 300n uic
  write test_symbolgen.raw
.endc
"}
C {parax_cap.sym} 280 -240 0 0 {name=C1 gnd=0 value=100f m=1}
C {parax_cap.sym} 280 -110 0 0 {name=C2 gnd=0 value=100f m=1}
C {launcher.sym} 620 -130 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/test_symbolgen.raw tran"
}
C {my_inv.sym} 670 -470 0 0 {name=x2 ROUT=1000
schematic=schematicgen(inv)}
C {lab_pin.sym} 600 -470 0 0 {name=p4 lab=IN}
C {lab_pin.sym} 740 -470 0 1 {name=p5 lab=IN_INV2}
C {my_inv.sym} 960 -470 0 0 {name=x4 ROUT=1000
schematic=schematicgen(buf)}
C {lab_pin.sym} 890 -470 0 0 {name=p6 lab=IN}
C {lab_pin.sym} 1030 -470 0 1 {name=p7 lab=IN_BUF2}
C {vdd.sym} 500 -310 0 0 {name=l2 lab=VCC}
C {lab_pin.sym} 500 -280 0 1 {name=p8 lab=VCC}
