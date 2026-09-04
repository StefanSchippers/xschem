v {xschem version=3.4.8RC file_version=1.3}
G {}
K {}
V {}
S {}
F {}
E {}
T {netlists ok in Spice and Verilog} 850 -1230 0 0 0.8 0.8 {}
N 1270 -890 1300 -890 {lab=OUTB[7:0]}
N 770 -890 800 -890 {lab=OUTA[2:0]}
N 600 -890 630 -890 {lab=INA[2:0]}
N 1100 -890 1130 -890 {lab=INB[7:0]}
C {lab_pin.sym} 800 -890 0 1 {name=p1 lab=OUTA[2:0]}
C {lab_pin.sym} 600 -890 0 0 {name=p2 lab=INA[2:0]}
C {test_parametric_ports/inv.sym} 1190 -890 0 0 {name=x2 width=8
schematic=inv2}
C {lab_pin.sym} 1300 -890 0 1 {name=p3 lab=OUTB[7:0]}
C {lab_pin.sym} 1100 -890 0 0 {name=p4 lab=INB[7:0]}
C {test_parametric_ports/inv.sym} 690 -890 0 0 {name=x4 width=3
schematic=inv1}
C {test_parametric_ports/buf.sym} 1640 -890 0 0 {name=x1 width=4 schematic=buf1
inv=inv3}
C {lab_pin.sym} 1720 -890 0 1 {name=p7 lab=OUTD[3:0]}
C {lab_pin.sym} 1580 -890 0 0 {name=p8 lab=IND[3:0]}
C {lab_pin.sym} 1270 -630 0 1 {name=p5 lab=OUTC[4:0]
}
C {lab_pin.sym} 1130 -630 0 0 {name=p6 lab=INC[4:0]
}
C {test_parametric_ports/double_buf.sym} 1190 -630 0 0 {name=x3 width=5
schematic=double_buf1.sch
buf=buf3
inv=inv4}
C {ipin.sym} 630 -700 0 0 { name=p9 lab=IND[3:0] }
C {ipin.sym} 630 -720 0 0 { name=p10 lab=INC[4:0] }
C {ipin.sym} 630 -740 0 0 { name=p11 lab=INB[7:0] }
C {ipin.sym} 630 -760 0 0 { name=p12 lab=INA[2:0] }
C {opin.sym} 1720 -700 0 0 { name=p13 lab=OUTD[3:0] }
C {opin.sym} 1720 -720 0 0 { name=p14 lab=OUTC[4:0] }
C {opin.sym} 1720 -740 0 0 { name=p15 lab=OUTB[7:0] }
C {opin.sym} 1720 -760 0 0 { name=p16 lab=OUTA[2:0] }
