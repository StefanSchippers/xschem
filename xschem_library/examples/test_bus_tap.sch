v {xschem version=3.4.0 file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
P 4 7 740 -320 740 -350 730 -350 740 -380 750 -350 740 -350 740 -320 {fill=true}
T {Specifying @lab
will result in net
@#1:net_name} 730 -320 0 0 0.4 0.4 {name=l6 layer=4}
T {tcleval(set show_pin_net_names=[set show_pin_net_names 1] [xschem update_all_sym_bboxes])} 150 -100 0 0 0.4 0.4 {name=l6 layer=4}
N 280 -420 1020 -420 {bus=true
lab=DATA[15:0]}
N 500 -610 500 -430 {
lab=DATA[3]}
N 390 -610 390 -430 {
lab=DATA[13]}
N 560 -410 560 -220 {
lab=DATA[7:4]}
N 440 -410 440 -220 {
lab=DATA[11:8]}
N 330 -410 330 -220 {
lab=DATA[3:0]}
N 710 -410 710 -220 {
lab=DATA[15:12]}
N 610 -610 610 -430 {
lab=DATA[10]}
N 730 -610 730 -430 {
lab=DATA[0]}
N 950 -580 950 -570 {
lab=VCC}
N 610 -680 610 -670 {
lab=VCC}
N 500 -680 500 -670 {
lab=VCC}
N 390 -680 390 -670 {
lab=VCC}
N 330 -160 330 -150 {
lab=VSS}
N 440 -160 440 -150 {
lab=VSS}
N 560 -160 560 -150 {
lab=VSS}
N 710 -160 710 -150 {
lab=VSS}
N 300 -530 300 -420 {
lab=DATA[15:0]}
N 300 -600 300 -590 {
lab=VCC}
N 950 -510 950 -420 {
lab=DATA[15:0]}
C {bus_tap.sym} 510 -420 3 0 {name=l1 lab=[3]
net_name=true}
C {bus_tap.sym} 400 -420 3 0 {name=l2 lab=[13]
net_name=true}
C {bus_tap.sym} 550 -420 1 0 {name=l3 lab=[7:4]
net_name=true}
C {bus_tap.sym} 430 -420 1 0 {name=l4 lab=[11:8]
net_name=true}
C {bus_tap.sym} 320 -420 1 0 {name=l5 lab=[3:0]
net_name=true}
C {bus_tap.sym} 620 -420 3 0 {name=l7 lab=[10]
net_name=true}
C {bus_tap.sym} 740 -420 3 0 {name=l8 lab=[0]
net_name=true}
C {res.sym} 730 -640 0 0 {name=R1
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 610 -640 0 0 {name=R2
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 500 -640 0 0 {name=R3
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 390 -640 0 0 {name=R4
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 330 -190 0 0 {name=R5[3:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 440 -190 0 0 {name=R6[3:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 560 -190 0 0 {name=R7[3:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 710 -190 0 0 {name=R8[3:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {lab_pin.sym} 300 -600 0 0 {name=p10 sig_type=std_logic lab=VCC
}
C {bus_tap.sym} 700 -420 1 0 {name=l6 lab=[15:12]
net_name=true}
C {lab_pin.sym} 390 -680 0 0 {name=p2 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 500 -680 0 0 {name=p3 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 610 -680 0 0 {name=p4 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 950 -580 0 0 {name=p5 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 330 -150 0 0 {name=p6 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 440 -150 0 0 {name=p7 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 560 -150 0 0 {name=p8 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 710 -150 0 0 {name=p9 sig_type=std_logic lab=VSS
}
C {res.sym} 300 -560 0 0 {name=R9[15:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {lab_pin.sym} 280 -420 0 0 {name=p1 sig_type=std_logic lab=DATA[15:0]
}
C {res.sym} 950 -540 0 0 {name=R10[15:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {title.sym} 160 -30 0 0 {name=l9 author="Stefan Schippers"}
