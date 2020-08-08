v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {assign #200 y = ~(a & b);
}
S {}
E {}
N 370 -280 410 -280 {lab=y}
N 240 -510 300 -510 {lab=VCCPIN}
N 240 -480 300 -480 {lab=VCCPIN}
N 240 -450 240 -280 {lab=y}
N 370 -140 390 -140 {lab=VSSPIN}
N 180 -480 200 -480 {lab=a}
N 290 -140 330 -140 {lab=b}
N 290 -360 310 -360 {lab=b}
N 300 -510 300 -480 {lab=VCCPIN}
N 370 -190 370 -170 {lab=#net1}
N 370 -220 390 -220 {lab=VSSPIN}
N 370 -280 370 -250 {lab=y}
N 350 -390 410 -390 {lab=VCCPIN}
N 350 -360 410 -360 {lab=VCCPIN}
N 410 -390 410 -360 {lab=VCCPIN}
N 350 -330 350 -280 {lab=y}
N 240 -280 350 -280 {lab=y}
N 350 -280 370 -280 {lab=y}
N 180 -220 330 -220 {lab=a}
N 180 -480 180 -220 {lab=a}
N 290 -360 290 -140 {lab=b}
N 110 -480 180 -480 {lab=a}
N 110 -360 290 -360 {lab=b}
N 350 -510 350 -390 {lab=VCCPIN}
N 240 -510 350 -510 {lab=VCCPIN}
N 150 -510 240 -510 {lab=VCCPIN}
N 280 -110 370 -110 {lab=VSSPIN}
C {opin.sym} 410 -280 0 0 {name=p1 lab=y verilog_type=wire}
C {ipin.sym} 110 -480 0 0 {name=p2 lab=a}
C {p.sym} 220 -480 0 0 {name=m2 model=cmosp w=wpa l=lpa  m=1 
}
C {p.sym} 330 -360 0 0 {name=m1 model=cmosp w=wpb l=lpb  m=1 
}
C {ipin.sym} 110 -360 0 0 {name=p3 lab=b}
C {lab_pin.sym} 150 -510 0 0 {name=l1 sig_type=std_logic lab=VCCPIN}
C {lab_pin.sym} 280 -110 0 0 {name=l2 sig_type=std_logic lab=VSSPIN}
C {lab_pin.sym} 390 -220 0 1 {name=l3 sig_type=std_logic lab=VSSPIN}
C {lab_pin.sym} 390 -140 0 1 {name=l4 sig_type=std_logic lab=VSSPIN}
C {n.sym} 350 -220 0 0 {name=m3 model=cmosn w=wna l=lna m=1}
C {n.sym} 350 -140 0 0 {name=m4 model=cmosn w=wnb l=lnb m=1}
C {title.sym} 170 0 0 0 {name=l5 author="Stefan Schippers"}
C {verilog_timescale.sym} 660 -257.5 0 0 {name=s1 timestep="1ps" precision="1ps" }
