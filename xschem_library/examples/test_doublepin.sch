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
T {OK!} 500 -1020 0 0 1 1 {layer=4}
T {OK!} 500 -1250 0 0 1 1 {layer=4}
T {OK!} 500 -1480 0 0 1 1 {layer=4}
T {Spice netlist allows duplicated pins on symbols} 50 -1570 0 0 1 1 {}
T {OK!} 500 -790 0 0 1 1 {layer=4}
T {OK!} 500 -580 0 0 1 1 {layer=4}
T {OK!} 1320 -1160 0 0 1 1 {layer=4}
T {OK!} 1320 -1430 0 0 1 1 {layer=4}
T {OK!} 500 -350 0 0 1 1 {layer=4}
N 480 -930 630 -930 {
lab=RRSSTT}
N 480 -910 630 -910 {
lab=CCKK}
N 480 -890 630 -890 {
lab=AA}
N 480 -870 630 -870 {
lab=BB}
N 480 -830 510 -830 {
lab=ZZ5}
N 930 -830 960 -830 {
lab=ZZ6}
N 480 -1060 510 -1060 {
lab=ZZ3}
N 930 -1060 960 -1060 {
lab=ZZ4}
N 480 -1290 510 -1290 {
lab=ZZ1}
N 930 -1290 960 -1290 {
lab=ZZ2}
N 480 -1390 630 -1390 {
lab=RRSSTT}
N 480 -1370 630 -1370 {
lab=CCKK}
N 480 -1350 630 -1350 {
lab=AA}
N 480 -1330 630 -1330 {
lab=BB}
N 480 -720 660 -720 {
lab=RRSSTT}
N 480 -700 660 -700 {
lab=CCKK}
N 480 -680 660 -680 {
lab=AA}
N 480 -660 660 -660 {
lab=BB}
N 480 -620 510 -620 {
lab=ZZ7}
N 630 -620 660 -620 {
lab=ZZ8}
N 960 -720 1080 -720 {
lab=RRSSTT}
N 960 -700 1080 -700 {
lab=CCKK}
N 960 -680 1080 -680 {
lab=AA}
N 960 -660 1080 -660 {
lab=BB}
N 1380 -620 1410 -620 {
lab=ZZ9}
N 480 -510 660 -510 {
lab=#net9}
N 480 -490 660 -490 {
lab=#net10}
N 480 -470 660 -470 {
lab=#net11}
N 480 -450 660 -450 {
lab=#net12}
N 480 -410 510 -410 {
lab=ZZ12}
N 630 -410 660 -410 {
lab=ZZ11}
N 960 -510 1080 -510 {
lab=#net13}
N 960 -490 1080 -490 {
lab=#net14}
N 960 -470 1080 -470 {
lab=#net15}
N 960 -450 1080 -450 {
lab=#net16}
N 1380 -410 1410 -410 {
lab=ZZ10}
N 1520 -990 1550 -990 {
lab=ZZ13}
N 1170 -1090 1220 -1090 {
lab=RRSSTT}
N 1170 -1070 1220 -1070 {
lab=CCKK}
N 1170 -1050 1220 -1050 {
lab=AA}
N 1170 -1030 1220 -1030 {
lab=BB}
N 1190 -1250 1220 -1250 {
lab=ZZ14}
N 1170 -1350 1220 -1350 {
lab=RRSSTT}
N 1170 -1330 1220 -1330 {
lab=CCKK}
N 1170 -1310 1220 -1310 {
lab=AA}
N 1170 -1290 1220 -1290 {
lab=BB}
N 480 -280 660 -280 {
lab=#net25}
N 480 -260 660 -260 {
lab=#net26}
N 480 -240 660 -240 {
lab=#net27}
N 480 -220 660 -220 {
lab=#net28}
N 480 -180 510 -180 {
lab=ZZ17}
N 630 -180 660 -180 {
lab=ZZ16}
N 960 -280 1080 -280 {
lab=#net29}
N 960 -260 1080 -260 {
lab=#net30}
N 960 -240 1080 -240 {
lab=#net31}
N 960 -220 1080 -220 {
lab=#net32}
N 1380 -180 1410 -180 {
lab=ZZ15}
N 10 -260 40 -260 {
lab=#net33}
N 120 -260 180 -260 {
lab=#net34}
C {doublepin.sym} 1230 -670 0 0 {name=x9}
C {doublepin.sym} 810 -670 0 1 {name=x8}
C {doublepin.sym} 780 -880 0 0 {name=x2}
C {lab_wire.sym} 550 -890 0 0 {name=l2 sig_type=std_logic lab=AA}
C {lab_pin.sym} 510 -830 0 1 {name=p3 lab=ZZ5}
C {lab_pin.sym} 960 -830 0 1 {name=p5 lab=ZZ6}
C {lab_wire.sym} 550 -930 0 0 {name=l3 sig_type=std_logic lab=RRSSTT}
C {lab_wire.sym} 550 -910 0 0 {name=l4 sig_type=std_logic lab=CCKK}
C {lab_wire.sym} 550 -870 0 0 {name=l5 sig_type=std_logic lab=BB}
C {doublepin.sym} 330 -1110 0 0 {name=x3}
C {doublepin.sym} 780 -1110 0 0 {name=x4}
C {lab_pin.sym} 180 -1120 0 0 {name=l6 sig_type=std_logic lab=AA}
C {lab_pin.sym} 510 -1060 0 1 {name=p1 lab=ZZ3}
C {lab_pin.sym} 960 -1060 0 1 {name=p2 lab=ZZ4}
C {lab_pin.sym} 180 -1160 0 0 {name=l7 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 180 -1140 0 0 {name=l8 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 180 -1100 0 0 {name=l9 sig_type=std_logic lab=BB}
C {lab_pin.sym} 930 -1120 0 1 {name=l10 sig_type=std_logic lab=AA}
C {lab_pin.sym} 930 -1160 0 1 {name=l11 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 930 -1140 0 1 {name=l12 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 930 -1100 0 1 {name=l13 sig_type=std_logic lab=BB}
C {doublepin.sym} 330 -1340 0 0 {name=x5}
C {doublepin.sym} 780 -1340 0 0 {name=x6}
C {lab_pin.sym} 180 -1350 0 0 {name=l14 sig_type=std_logic lab=AA}
C {lab_pin.sym} 510 -1290 0 1 {name=p4 lab=ZZ1}
C {lab_pin.sym} 960 -1290 0 1 {name=p6 lab=ZZ2}
C {lab_pin.sym} 180 -1390 0 0 {name=l15 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 180 -1370 0 0 {name=l16 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 180 -1330 0 0 {name=l17 sig_type=std_logic lab=BB}
C {lab_pin.sym} 930 -1350 0 1 {name=l18 sig_type=std_logic lab=AA}
C {lab_pin.sym} 930 -1390 0 1 {name=l19 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 930 -1370 0 1 {name=l20 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 930 -1330 0 1 {name=l21 sig_type=std_logic lab=BB}
C {lab_wire.sym} 550 -1350 0 0 {name=l22 sig_type=std_logic lab=AA}
C {lab_wire.sym} 550 -1390 0 0 {name=l23 sig_type=std_logic lab=RRSSTT}
C {lab_wire.sym} 550 -1370 0 0 {name=l24 sig_type=std_logic lab=CCKK}
C {lab_wire.sym} 550 -1330 0 0 {name=l25 sig_type=std_logic lab=BB}
C {lab_pin.sym} 180 -660 0 0 {name=l33 sig_type=std_logic lab=BB}
C {doublepin.sym} 330 -880 0 0 {name=x1}
C {lab_pin.sym} 510 -620 0 1 {name=p7 lab=ZZ7}
C {lab_pin.sym} 630 -620 0 0 {name=p8 lab=ZZ8}
C {lab_pin.sym} 180 -680 0 0 {name=l30 sig_type=std_logic lab=AA}
C {lab_pin.sym} 180 -720 0 0 {name=l31 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 180 -700 0 0 {name=l32 sig_type=std_logic lab=CCKK}
C {doublepin.sym} 330 -670 0 0 {name=x7}
C {ipin.sym} 100 -80 0 0 { name=p9 lab=RRSSTT }
C {ipin.sym} 100 -100 0 0 { name=p10 lab=CCKK }
C {ipin.sym} 100 -120 0 0 { name=p11 lab=BB }
C {ipin.sym} 100 -140 0 0 { name=p12 lab=AA }
C {opin.sym} 270 -120 0 0 { name=p13 lab=ZZ[17..1]}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {lab_pin.sym} 1410 -620 0 1 {name=p8 lab=ZZ9}
C {doublepin.sym} 1230 -460 0 0 {name=x10}
C {doublepin.sym} 810 -460 0 1 {name=x11}
C {lab_pin.sym} 510 -410 0 1 {name=p7 lab=ZZ12}
C {lab_pin.sym} 630 -410 0 0 {name=p8 lab=ZZ11}
C {doublepin.sym} 330 -460 0 0 {name=x12}
C {lab_pin.sym} 1410 -410 0 1 {name=p1 lab=ZZ10}
C {doublepin.sym} 1370 -1040 0 0 {name=x13}
C {lab_pin.sym} 1550 -990 0 1 {name=p2 lab=ZZ13}
C {lab_pin.sym} 1520 -1050 0 1 {name=l10 sig_type=std_logic lab=AA}
C {lab_pin.sym} 1520 -1090 0 1 {name=l11 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 1520 -1070 0 1 {name=l12 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 1520 -1030 0 1 {name=l13 sig_type=std_logic lab=BB}
C {doublepin.sym} 1370 -1300 0 1 {name=x14}
C {lab_pin.sym} 1190 -1250 0 0 {name=p2 lab=ZZ14}
C {lab_pin.sym} 1520 -1310 0 1 {name=l10 sig_type=std_logic lab=AA}
C {lab_pin.sym} 1520 -1350 0 1 {name=l11 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 1520 -1330 0 1 {name=l12 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 1520 -1290 0 1 {name=l13 sig_type=std_logic lab=BB}
C {doublepin.sym} 330 -230 0 0 {name=x17}
C {doublepin.sym} 810 -230 0 1 {name=x16}
C {lab_pin.sym} 510 -180 0 1 {name=p7 lab=ZZ17}
C {lab_pin.sym} 630 -180 0 0 {name=p8 lab=ZZ16}
C {doublepin.sym} 1230 -230 0 0 {name=x15}
C {lab_pin.sym} 1410 -180 0 1 {name=p1 lab=ZZ15}
C {inv_ngspice.sym} 80 -260 0 0 {name=x18 ROUT=1000}
