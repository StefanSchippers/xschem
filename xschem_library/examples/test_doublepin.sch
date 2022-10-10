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
T {Netlister allows duplicated pins on symbols
Electrical nodes are propagated through duplicated symbol pins} 50 -1570 0 0 1 1 {}
N 480 -930 630 -930 {
lab=RRSSTT}
N 480 -910 630 -910 {
lab=CCKK}
N 480 -890 630 -890 {
lab=AA[3:0]}
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
lab=AA[3:0]}
N 480 -1330 630 -1330 {
lab=BB}
N 480 -720 660 -720 {
lab=RRSSTT}
N 480 -700 660 -700 {
lab=CCKK}
N 480 -680 660 -680 {
lab=AA[3:0]}
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
lab=AA[3:0]}
N 960 -660 1080 -660 {
lab=BB}
N 1380 -620 1410 -620 {
lab=ZZ9}
N 480 -510 660 -510 {
lab=#net1}
N 480 -490 660 -490 {
lab=#net2}
N 480 -470 660 -470 {
lab=#net3}
N 480 -450 660 -450 {
lab=#net4}
N 480 -410 510 -410 {
lab=ZZ12}
N 630 -410 660 -410 {
lab=ZZ11}
N 1260 -410 1290 -410 {
lab=ZZ10}
N 1520 -990 1550 -990 {
lab=ZZ13}
N 1170 -1090 1220 -1090 {
lab=RRSSTT}
N 1170 -1070 1220 -1070 {
lab=CCKK}
N 1170 -1050 1220 -1050 {
lab=AA[3:0]}
N 1170 -1030 1220 -1030 {
lab=BB}
N 1190 -1250 1220 -1250 {
lab=ZZ14}
N 1170 -1350 1220 -1350 {
lab=RRSSTT}
N 1170 -1330 1220 -1330 {
lab=CCKK}
N 1170 -1310 1220 -1310 {
lab=AA[3:0]}
N 1170 -1290 1220 -1290 {
lab=BB}
N 620 -280 660 -280 {
lab=#net5}
N 480 -260 660 -260 {
lab=#net6}
N 480 -240 660 -240 {
lab=#net7}
N 480 -220 660 -220 {
lab=#net8}
N 480 -180 510 -180 {
lab=ZZ17}
N 630 -180 660 -180 {
lab=ZZ16}
N 960 -280 1080 -280 {
lab=#net5}
N 960 -260 1080 -260 {
lab=#net6}
N 960 -240 1080 -240 {
lab=#net7}
N 960 -220 1080 -220 {
lab=#net8}
N 1380 -180 1410 -180 {
lab=ZZ15}
N 10 -260 40 -260 {
lab=#net9}
N 120 -260 180 -260 {
lab=#net6}
N 70 -450 100 -450 {
lab=#net10}
N 1260 -510 1440 -510 {
lab=#net1}
N 1260 -490 1440 -490 {
lab=#net2}
N 1260 -470 1440 -470 {
lab=#net3}
N 1260 -450 1440 -450 {
lab=#net4}
N 1740 -310 1770 -310 {
lab=ZZ18}
N 1740 -410 1740 -310 {
lab=ZZ18}
N 2040 -310 2070 -310 {
lab=ZZ19}
N 2040 -410 2040 -310 {
lab=ZZ19}
N 620 -350 620 -280 {
lab=#net5}
N 620 -350 680 -350 {
lab=#net5}
N 760 -350 810 -350 {
lab=#net11}
N 1110 -920 1260 -920 {
lab=RRSSTT}
N 1110 -900 1260 -900 {
lab=CCKK}
N 1110 -880 1260 -880 {
lab=AA[3:0]}
N 1110 -860 1260 -860 {
lab=BB}
N 1560 -820 1590 -820 {
lab=ZZ20}
N 1690 -920 1760 -920 {
lab=RRSSTT}
N 1560 -900 1760 -900 {
lab=CCKK}
N 1560 -880 1760 -880 {
lab=AA[3:0]}
N 1730 -860 1760 -860 {
lab=BB}
N 1730 -820 1760 -820 {
lab=ZZ21}
N 2060 -880 2080 -880 {
lab=AA[3:0]}
N 2060 -860 2080 -860 {
lab=BB}
N 2060 -900 2080 -900 {
lab=CCKK}
N 2060 -920 2080 -920 {
lab=RRSSTT}
N 1690 -1070 1690 -920 {
lab=RRSSTT}
N 1690 -1370 1820 -1370 {
lab=RRSSTT}
N 1730 -1110 1730 -860 {
lab=BB}
N 1730 -1410 1820 -1410 {
lab=BB}
N 1980 -1330 2060 -1330 {
lab=BB}
N 2220 -1330 2280 -1330 {
lab=RRSSTT}
N 2220 -1250 2280 -1250 {
lab=BB}
N 2260 -1030 2360 -1030 {
lab=BB}
N 2260 -950 2300 -950 {
lab=RRSSTT}
N 1730 -1110 1780 -1110 {
lab=BB}
N 1690 -1070 1780 -1070 {
lab=RRSSTT}
N 2330 -930 2360 -930 {
lab=ZZ22}
N 2660 -990 2690 -990 {
lab=#net12}
N 2660 -970 2690 -970 {
lab=RRSSTT}
N 2660 -1010 2690 -1010 {
lab=#net13}
N 2660 -1030 2690 -1030 {
lab=BB}
N 2300 -970 2300 -950 {
lab=RRSSTT}
N 2300 -970 2360 -970 {
lab=RRSSTT}
N 480 -280 620 -280 {
lab=#net5}
N 1560 -920 1690 -920 {
lab=RRSSTT}
N 1560 -860 1730 -860 {
lab=BB}
N 1730 -1410 1730 -1110 {
lab=BB}
N 2300 -1010 2360 -1010 {
lab=#net13}
N 2300 -990 2360 -990 {
lab=#net12}
N 1690 -1370 1690 -1070 {
lab=RRSSTT}
N 1980 -1290 2060 -1290 {
lab=RRSSTT}
N 2220 -1290 2280 -1290 {
lab=BB}
N 2220 -1210 2280 -1210 {
lab=RRSSTT}
N 2140 -1410 2200 -1410 {
lab=BB}
N 2140 -1370 2200 -1370 {
lab=RRSSTT}
N 1920 -1490 1980 -1490 {
lab=BB}
N 1920 -1450 1980 -1450 {
lab=RRSSTT}
C {doublepin.sym} 1230 -670 0 0 {name=x9
net_name=true}
C {doublepin.sym} 810 -670 0 1 {name=x8
net_name=true}
C {doublepin.sym} 780 -880 0 0 {name=x2
net_name=true}
C {lab_wire.sym} 550 -890 0 0 {name=l2 sig_type=std_logic lab=AA[3:0]}
C {lab_pin.sym} 510 -830 0 1 {name=p3 lab=ZZ5}
C {lab_pin.sym} 960 -830 0 1 {name=p5 lab=ZZ6}
C {lab_wire.sym} 550 -930 0 0 {name=l3 sig_type=std_logic lab=RRSSTT}
C {lab_wire.sym} 550 -910 0 0 {name=l4 sig_type=std_logic lab=CCKK}
C {lab_wire.sym} 550 -870 0 0 {name=l5 sig_type=std_logic lab=BB}
C {doublepin.sym} 330 -1110 0 0 {name=x3
net_name=true}
C {doublepin.sym} 780 -1110 0 0 {name=x4
net_name=true}
C {lab_pin.sym} 180 -1120 0 0 {name=l6 sig_type=std_logic lab=AA[3:0]}
C {lab_pin.sym} 510 -1060 0 1 {name=p1 lab=ZZ3}
C {lab_pin.sym} 960 -1060 0 1 {name=p2 lab=ZZ4}
C {lab_pin.sym} 180 -1160 0 0 {name=l7 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 180 -1140 0 0 {name=l8 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 180 -1100 0 0 {name=l9 sig_type=std_logic lab=BB}
C {lab_pin.sym} 930 -1120 0 1 {name=l10 sig_type=std_logic lab=AA[3:0]}
C {lab_pin.sym} 930 -1160 0 1 {name=l11 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 930 -1140 0 1 {name=l12 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 930 -1100 0 1 {name=l13 sig_type=std_logic lab=BB}
C {doublepin.sym} 330 -1340 0 0 {name=x5
net_name=true}
C {doublepin.sym} 780 -1340 0 0 {name=x6
net_name=true}
C {lab_pin.sym} 180 -1350 0 0 {name=l14 sig_type=std_logic lab=AA[3:0]}
C {lab_pin.sym} 510 -1290 0 1 {name=p4 lab=ZZ1}
C {lab_pin.sym} 960 -1290 0 1 {name=p6 lab=ZZ2}
C {lab_pin.sym} 180 -1390 0 0 {name=l15 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 180 -1370 0 0 {name=l16 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 180 -1330 0 0 {name=l17 sig_type=std_logic lab=BB}
C {lab_pin.sym} 930 -1350 0 1 {name=l18 sig_type=std_logic lab=AA[3:0]}
C {lab_pin.sym} 930 -1390 0 1 {name=l19 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 930 -1370 0 1 {name=l20 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 930 -1330 0 1 {name=l21 sig_type=std_logic lab=BB}
C {lab_wire.sym} 550 -1350 0 0 {name=l22 sig_type=std_logic lab=AA[3:0]}
C {lab_wire.sym} 550 -1390 0 0 {name=l23 sig_type=std_logic lab=RRSSTT}
C {lab_wire.sym} 550 -1370 0 0 {name=l24 sig_type=std_logic lab=CCKK}
C {lab_wire.sym} 550 -1330 0 0 {name=l25 sig_type=std_logic lab=BB}
C {lab_pin.sym} 180 -660 0 0 {name=l33 sig_type=std_logic lab=BB}
C {doublepin.sym} 330 -880 0 0 {name=x1
net_name=true}
C {lab_pin.sym} 510 -620 0 1 {name=p7 lab=ZZ7}
C {lab_pin.sym} 630 -620 0 0 {name=p8 lab=ZZ8}
C {lab_pin.sym} 180 -680 0 0 {name=l30 sig_type=std_logic lab=AA[3:0]}
C {lab_pin.sym} 180 -720 0 0 {name=l31 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 180 -700 0 0 {name=l32 sig_type=std_logic lab=CCKK}
C {doublepin.sym} 330 -670 0 0 {name=x7
net_name=true}
C {ipin.sym} 100 -80 0 0 { name=p9 lab=RRSSTT }
C {ipin.sym} 100 -100 0 0 { name=p10 lab=CCKK }
C {ipin.sym} 100 -120 0 0 { name=p11 lab=BB }
C {ipin.sym} 100 -140 0 0 { name=p12 lab=AA[3:0] }
C {opin.sym} 270 -120 0 0 { name=p13 lab=ZZ[22..1]}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {lab_pin.sym} 1410 -620 0 1 {name=p8 lab=ZZ9}
C {doublepin.sym} 1110 -460 0 0 {name=x10
net_name=true}
C {doublepin.sym} 810 -460 0 1 {name=x11
net_name=true}
C {lab_pin.sym} 510 -410 0 1 {name=p7 lab=ZZ12}
C {lab_pin.sym} 630 -410 0 0 {name=p8 lab=ZZ11}
C {doublepin.sym} 330 -460 0 0 {name=x12
net_name=true}
C {lab_pin.sym} 1290 -410 0 1 {name=p1 lab=ZZ10}
C {doublepin.sym} 1370 -1040 0 0 {name=x13
net_name=true}
C {lab_pin.sym} 1550 -990 0 1 {name=p2 lab=ZZ13}
C {lab_pin.sym} 1520 -1050 0 1 {name=l1 sig_type=std_logic lab=AA[3:0]}
C {lab_pin.sym} 1520 -1090 0 1 {name=l11 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 1520 -1070 0 1 {name=l12 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 1520 -1030 0 1 {name=l13 sig_type=std_logic lab=BB}
C {doublepin.sym} 1370 -1300 0 1 {name=x14
net_name=true}
C {lab_pin.sym} 1190 -1250 0 0 {name=p2 lab=ZZ14}
C {lab_pin.sym} 1520 -1310 0 1 {name=l3 sig_type=std_logic lab=AA[3:0]}
C {lab_pin.sym} 1520 -1350 0 1 {name=l11 sig_type=std_logic lab=RRSSTT}
C {lab_pin.sym} 1520 -1330 0 1 {name=l12 sig_type=std_logic lab=CCKK}
C {lab_pin.sym} 1520 -1290 0 1 {name=l13 sig_type=std_logic lab=BB}
C {doublepin.sym} 330 -230 0 0 {name=x17[1:0]
net_name=true}
C {doublepin.sym} 810 -230 0 1 {name=x16[1:0]
net_name=true}
C {lab_pin.sym} 510 -180 0 1 {name=p7 lab=ZZ17}
C {lab_pin.sym} 630 -180 0 0 {name=p8 lab=ZZ16}
C {doublepin.sym} 1230 -230 0 0 {name=x22[1:0]
net_name=true}
C {lab_pin.sym} 1410 -180 0 1 {name=p1 lab=ZZ15}
C {inv_ngspice.sym} 80 -260 0 0 {name=x18 ROUT=1000 net_name=true}
C {inv_ngspice.sym} 140 -450 0 0 {name=x19 ROUT=1000 net_name=true}
C {doublepin.sym} 1590 -460 0 0 {name=x20
net_name=true}
C {lab_pin.sym} 1770 -310 0 1 {name=p8 lab=ZZ18}
C {doublepin.sym} 1890 -460 0 0 {name=x21
net_name=true}
C {lab_pin.sym} 2070 -310 0 1 {name=p8 lab=ZZ19}
C {inv_ngspice.sym} 720 -350 0 0 {name=x15 ROUT=1000 net_name=true}
C {doublepin.sym} 1410 -870 0 0 {name=x23
net_name=true}
C {lab_wire.sym} 1180 -880 0 0 {name=l2 sig_type=std_logic lab=AA[3:0]}
C {lab_pin.sym} 1590 -820 0 1 {name=p5 lab=ZZ20}
C {lab_wire.sym} 1180 -920 0 0 {name=l3 sig_type=std_logic lab=RRSSTT}
C {lab_wire.sym} 1180 -900 0 0 {name=l4 sig_type=std_logic lab=CCKK}
C {lab_wire.sym} 1180 -860 0 0 {name=l5 sig_type=std_logic lab=BB}
C {doublepin.sym} 1910 -870 0 1 {name=x24
net_name=true}
C {lab_pin.sym} 1730 -820 0 0 {name=p5 lab=ZZ21}
C {cross.sym} 1900 -1370 0 0 {name=x25}
C {cross.sym} 2140 -1290 0 0 {name=x26}
C {cross.sym} 1860 -1070 0 0 {name=x27}
C {cross.sym} 2020 -1070 0 0 {name=x28}
C {cross.sym} 2180 -990 0 0 {name=x29}
C {doublepin.sym} 2510 -980 0 1 {name=x30
net_name=true}
C {lab_pin.sym} 2330 -930 0 0 {name=p5 lab=ZZ22}
C {cross.sym} 2060 -1410 2 0 {name=x31}
