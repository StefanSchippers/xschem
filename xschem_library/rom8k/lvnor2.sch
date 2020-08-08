v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {assign #200 y =~(a|b);}
S {}
E {}
N 60 -30 100 -30 {lab=y}
N 40 -230 100 -230 {lab=VCCPIN}
N -130 30 -110 30 {lab=VSSBPIN}
N -20 -230 0 -230 {lab=a}
N 100 -260 100 -230 {lab=VCCPIN}
N -130 -30 -130 0 {lab=y}
N 60 30 80 30 {lab=VSSBPIN}
N 60 -30 60 0 {lab=y}
N 40 -110 100 -110 {lab=VCCPIN}
N 100 -140 100 -110 {lab=VCCPIN}
N 40 -80 40 -30 {lab=y}
N 40 -30 60 -30 {lab=y}
N -160 -260 100 -260 {lab=VCCPIN}
N -220 60 -130 60 {lab=VSSPIN}
N -130 -30 40 -30 {lab=y}
N -170 -110 -0 -110 {lab=b}
N -170 -110 -170 30 {lab=b}
N -20 30 20 30 {lab=a}
N -20 -230 -20 30 {lab=a}
N -200 -230 -20 -230 {lab=a}
N -200 -110 -170 -110 {lab=b}
N 40 -200 40 -140 {lab=#net1}
N 100 -230 100 -140 {lab=VCCPIN}
N -130 60 60 60 {lab=VSSPIN}
C {opin.sym} 100 -30 0 0 {name=p1 lab=y verilog_type=wire}
C {ipin.sym} -200 -230 0 0 {name=p2 lab=a}
C {plv4t.sym} 20 -230 0 0 {name=m2 model=cmosp w=wpa l=lpa  m=1 
}
C {plv4t.sym} 20 -110 0 0 {name=m1 model=cmosp w=wpb l=lpb  m=1 
}
C {ipin.sym} -200 -110 0 0 {name=p3 lab=b}
C {lab_pin.sym} -160 -260 0 0 {name=l1 sig_type=std_logic lab=VCCPIN}
C {lab_pin.sym} -220 60 0 0 {name=l2 sig_type=std_logic lab=VSSPIN}
C {lab_pin.sym} 80 30 0 1 {name=l3 sig_type=std_logic lab=VSSPIN}
C {lab_pin.sym} -110 30 0 1 {name=l4 sig_type=std_logic lab=VSSPIN}
C {nlv4t.sym} 40 30 0 0 {name=m3 model=cmosn w=wna l=lna m=1}
C {nlv4t.sym} -150 30 0 0 {name=m4 model=cmosn w=wnb l=lnb m=1}
C {verilog_timescale.sym} 400 -37.5 0 0 {name=s1 timestep="1ps" precision="1ps" }
