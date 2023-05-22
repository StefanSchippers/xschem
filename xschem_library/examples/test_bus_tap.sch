v {xschem version=3.4.0 file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
N 140 -300 880 -300 {bus=true
lab=DATA[15:0]}
N 360 -490 360 -310 {
lab=DATA[3]}
N 250 -490 250 -310 {
lab=DATA[13]}
N 420 -290 420 -100 {
lab=DATA[7:4]}
N 300 -290 300 -100 {
lab=DATA[11:8]}
N 190 -290 190 -100 {
lab=DATA[3:0]}
N 570 -290 570 -100 {
lab=DATA[15:12]}
N 470 -490 470 -310 {
lab=DATA[10]}
N 590 -490 590 -310 {
lab=DATA[0]}
N 590 -560 590 -550 {
lab=VCC}
N 470 -560 470 -550 {
lab=VCC}
N 360 -560 360 -550 {
lab=VCC}
N 250 -560 250 -550 {
lab=VCC}
N 190 -40 190 -30 {
lab=VSS}
N 300 -40 300 -30 {
lab=VSS}
N 420 -40 420 -30 {
lab=VSS}
N 570 -40 570 -30 {
lab=VSS}
N 160 -410 160 -300 {
lab=DATA[15:0]}
N 160 -480 160 -470 {
lab=VCC}
N 810 -150 810 -140 {
lab=VSS}
N 810 -300 810 -210 {
lab=DATA[15:0]}
C {bus_tap.sym} 370 -300 3 0 {name=l1 lab=[3]}
C {bus_tap.sym} 260 -300 3 0 {name=l2 lab=[13]}
C {bus_tap.sym} 410 -300 1 0 {name=l3 lab=[7:4]}
C {bus_tap.sym} 290 -300 1 0 {name=l4 lab=[11:8]}
C {bus_tap.sym} 180 -300 1 0 {name=l5 lab=[3:0]}
C {bus_tap.sym} 480 -300 3 0 {name=l7 lab=[10]}
C {bus_tap.sym} 600 -300 3 0 {name=l8 lab=[0]}
C {res.sym} 590 -520 0 0 {name=R1
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 470 -520 0 0 {name=R2
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 360 -520 0 0 {name=R3
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 250 -520 0 0 {name=R4
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 190 -70 0 0 {name=R5[3:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 300 -70 0 0 {name=R6[3:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 420 -70 0 0 {name=R7[3:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 570 -70 0 0 {name=R8[3:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {lab_pin.sym} 160 -480 0 0 {name=p10 sig_type=std_logic lab=VCC
}
C {bus_tap.sym} 560 -300 1 0 {name=l6 lab=[15:12]}
C {lab_pin.sym} 250 -560 0 0 {name=p2 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 360 -560 0 0 {name=p3 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 470 -560 0 0 {name=p4 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 590 -560 0 0 {name=p5 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 190 -30 0 0 {name=p6 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 300 -30 0 0 {name=p7 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 420 -30 0 0 {name=p8 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 570 -30 0 0 {name=p9 sig_type=std_logic lab=VSS
}
C {res.sym} 160 -440 0 0 {name=R9[15:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {lab_pin.sym} 140 -300 0 0 {name=p1 sig_type=std_logic lab=DATA[15:0]
}
C {lab_pin.sym} 810 -140 0 0 {name=p11 sig_type=std_logic lab=VSS
}
C {res.sym} 810 -180 0 0 {name=R10[15:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
