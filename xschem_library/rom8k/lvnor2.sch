v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {assign #200 y =~(a|b);}
S {}
E {}
N 400 -190 440 -190 {lab=y}
N 380 -390 440 -390 {lab=VCCPIN}
N 210 -130 230 -130 {lab=VSSBPIN}
N 320 -390 340 -390 {lab=a}
N 440 -420 440 -390 {lab=VCCPIN}
N 210 -190 210 -160 {lab=y}
N 400 -130 420 -130 {lab=VSSBPIN}
N 400 -190 400 -160 {lab=y}
N 380 -270 440 -270 {lab=VCCPIN}
N 440 -300 440 -270 {lab=VCCPIN}
N 380 -240 380 -190 {lab=y}
N 380 -190 400 -190 {lab=y}
N 180 -420 440 -420 {lab=VCCPIN}
N 120 -100 210 -100 {lab=VSSPIN}
N 210 -190 380 -190 {lab=y}
N 170 -270 340 -270 {lab=b}
N 170 -270 170 -130 {lab=b}
N 320 -130 360 -130 {lab=a}
N 320 -390 320 -130 {lab=a}
N 140 -390 320 -390 {lab=a}
N 140 -270 170 -270 {lab=b}
N 380 -360 380 -300 {lab=#net1}
N 440 -390 440 -300 {lab=VCCPIN}
N 210 -100 400 -100 {lab=VSSPIN}
C {opin.sym} 440 -190 0 0 {name=p1 lab=y verilog_type=wire}
C {ipin.sym} 140 -390 0 0 {name=p2 lab=a}
C {plv4t.sym} 360 -390 0 0 {name=m2 model=cmosp w=wpa l=lpa  m=1 
}
C {plv4t.sym} 360 -270 0 0 {name=m1 model=cmosp w=wpb l=lpb  m=1 
}
C {ipin.sym} 140 -270 0 0 {name=p3 lab=b}
C {lab_pin.sym} 180 -420 0 0 {name=l1 sig_type=std_logic lab=VCCPIN}
C {lab_pin.sym} 120 -100 0 0 {name=l2 sig_type=std_logic lab=VSSPIN}
C {lab_pin.sym} 420 -130 0 1 {name=l3 sig_type=std_logic lab=VSSPIN}
C {lab_pin.sym} 230 -130 0 1 {name=l4 sig_type=std_logic lab=VSSPIN}
C {nlv4t.sym} 380 -130 0 0 {name=m3 model=cmosn w=wna l=lna m=1}
C {nlv4t.sym} 190 -130 0 0 {name=m4 model=cmosn w=wnb l=lnb m=1}
C {verilog_timescale.sym} 740 -197.5 0 0 {name=s1 timestep="1ps" precision="1ps" }
C {title.sym} 160 0 0 0 {name=l5 author="Stefan Schippers"}
