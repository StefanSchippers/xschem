v {xschem version=3.1.0 file_version=1.2
}
G {}
K {}
V {

initial begin
  $dumpfile("dumpfile.vcd");
  $dumpvars;
  A=0;
end

integer i = 0;
always begin
  i = i + 1;
  #100000;
  $display("time= %t: A= %08b   Y= %08b", $time, A, Y);
  A=~A;
  if(i==20) $finish;
end}
S {va a 0 pwl 0 0 100n 0 101n 3
vvcc vcc 0 dc 3
vvss vss 0 dc 0

.tran 1n 200n}
E {}
T {OK!} 470 -750 0 0 1 1 {layer=4}
T {OK!} 470 -1010 0 0 1 1 {layer=4}
T {OK!} 470 -1260 0 0 1 1 {layer=4}
T {Spice netlist allows duplicated pins on symbols} 20 -1350 0 0 1 1 {}
T {OK!} 470 -490 0 0 1 1 {layer=4}
N 450 -660 600 -660 {
lab=RRSSTT}
N 450 -640 600 -640 {
lab=CCKK}
N 450 -620 600 -620 {
lab=AA}
N 450 -600 600 -600 {
lab=BB}
N 450 -560 480 -560 {
lab=ZZ5}
N 900 -560 930 -560 {
lab=ZZ6}
N 450 -820 480 -820 {
lab=ZZ3}
N 900 -820 930 -820 {
lab=ZZ4}
N 450 -1070 480 -1070 {
lab=ZZ1}
N 900 -1070 930 -1070 {
lab=ZZ2}
N 450 -1170 600 -1170 {
lab=RRSSTT}
N 450 -1150 600 -1150 {
lab=CCKK}
N 450 -1130 600 -1130 {
lab=AA}
N 450 -1110 600 -1110 {
lab=BB}
N 450 -410 600 -410 {
lab=RRSSTT}
N 450 -390 600 -390 {
lab=CCKK}
N 450 -370 600 -370 {
lab=AA}
N 450 -350 600 -350 {
lab=BB}
N 450 -310 480 -310 {
lab=ZZ7}
N 900 -310 930 -310 {
lab=ZZ8}
N 900 -410 1050 -410 {
lab=RRSSTT}
N 900 -390 1050 -390 {
lab=CCKK}
N 900 -370 1050 -370 {
lab=AA}
N 900 -350 1050 -350 {
lab=BB}
N 1350 -310 1380 -310 {
lab=ZZ9}
C {doublepin.sym} 1200 -360 0 0 {name=x9}
C {doublepin.sym} 750 -360 0 0 {name=x8}
C {doublepin.sym} 750 -610 0 0 {name=x2}
C {lab_wire.sym} 520 -620 0 0 {name=l2 sig_type=std_logic lab=AA}
C {lab_pin.sym} 480 -560 0 1 {name=p3 lab=ZZ5}
C {lab_pin.sym} 930 -560 0 1 {name=p5 lab=ZZ6}
C {lab_wire.sym} 520 -660 0 0 {name=l3 sig_type=std_logic lab=RRSSTT}
C {lab_wire.sym} 520 -640 0 0 {name=l4 sig_type=std_logic lab=CCKK}
C {lab_wire.sym} 520 -600 0 0 {name=l5 sig_type=std_logic lab=BB}
C {doublepin.sym} 300 -870 0 0 {name=x3}
C {doublepin.sym} 750 -870 0 0 {name=x4}
C {lab_pin.sym} 150 -880 0 0 {name=l6 sig_type=std_logic lab=AA}
C {lab_pin.sym} 480 -820 0 1 {name=p1 lab=ZZ3}
C {lab_pin.sym} 930 -820 0 1 {name=p2 lab=ZZ4}
C {lab_pin.sym} 150 -920 0 0 {name=l7 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 150 -900 0 0 {name=l8 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 150 -860 0 0 {name=l9 sig_type=std_logic lab=BB}
C {lab_pin.sym} 900 -880 0 1 {name=l10 sig_type=std_logic lab=AA}
C {lab_pin.sym} 900 -920 0 1 {name=l11 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 900 -900 0 1 {name=l12 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 900 -860 0 1 {name=l13 sig_type=std_logic lab=BB}
C {doublepin.sym} 300 -1120 0 0 {name=x5}
C {doublepin.sym} 750 -1120 0 0 {name=x6}
C {lab_pin.sym} 150 -1130 0 0 {name=l14 sig_type=std_logic lab=AA}
C {lab_pin.sym} 480 -1070 0 1 {name=p4 lab=ZZ1}
C {lab_pin.sym} 930 -1070 0 1 {name=p6 lab=ZZ2}
C {lab_pin.sym} 150 -1170 0 0 {name=l15 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 150 -1150 0 0 {name=l16 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 150 -1110 0 0 {name=l17 sig_type=std_logic lab=BB}
C {lab_pin.sym} 900 -1130 0 1 {name=l18 sig_type=std_logic lab=AA}
C {lab_pin.sym} 900 -1170 0 1 {name=l19 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 900 -1150 0 1 {name=l20 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 900 -1110 0 1 {name=l21 sig_type=std_logic lab=BB}
C {lab_wire.sym} 520 -1130 0 0 {name=l22 sig_type=std_logic lab=AA}
C {lab_wire.sym} 520 -1170 0 0 {name=l23 sig_type=std_logic lab=RRSSTT}
C {lab_wire.sym} 520 -1150 0 0 {name=l24 sig_type=std_logic lab=CCKK}
C {lab_wire.sym} 520 -1110 0 0 {name=l25 sig_type=std_logic lab=BB}
C {lab_pin.sym} 150 -350 0 0 {name=l33 sig_type=std_logic lab=BB}
C {doublepin.sym} 300 -610 0 0 {name=x1}
C {lab_pin.sym} 480 -310 0 1 {name=p7 lab=ZZ7}
C {lab_pin.sym} 930 -310 0 1 {name=p8 lab=ZZ8}
C {lab_pin.sym} 150 -370 0 0 {name=l30 sig_type=std_logic lab=AA}
C {lab_pin.sym} 150 -410 0 0 {name=l31 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 150 -390 0 0 {name=l32 sig_type=std_logic lab=CCKK}
C {doublepin.sym} 300 -360 0 0 {name=x7}
C {ipin.sym} 100 -80 0 0 { name=p9 lab=RRSSTT }
C {ipin.sym} 100 -100 0 0 { name=p10 lab=CCKK }
C {ipin.sym} 100 -120 0 0 { name=p11 lab=BB }
C {ipin.sym} 100 -140 0 0 { name=p12 lab=AA }
C {opin.sym} 270 -120 0 0 { name=p13 lab=ZZ[9..1]}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {lab_pin.sym} 1380 -310 0 1 {name=p8 lab=ZZ9}
