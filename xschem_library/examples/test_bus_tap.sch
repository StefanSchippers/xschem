v {xschem version=3.4.0 file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
N 140 -300 880 -300 {bus=true
lab=DATA[15:0]}
N 360 -410 360 -310 {
lab=DATA[3]}
N 250 -410 250 -310 {
lab=DATA[4]}
N 420 -290 420 -210 {
lab=DATA[7:4]}
N 300 -290 300 -210 {
lab=DATA[11:8]}
N 190 -290 190 -210 {
lab=DATA[3:0]}
N 570 -290 570 -210 {
lab=DATA[15:12]}
N 470 -410 470 -310 {
lab=DATA[10]}
N 590 -410 590 -310 {
lab=DATA[0]}
N 590 -480 590 -470 {
lab=VCC}
N 470 -480 470 -470 {
lab=VCC}
N 360 -480 360 -470 {
lab=VCC}
N 250 -480 250 -470 {
lab=VCC}
N 190 -150 190 -140 {
lab=VSS}
N 300 -150 300 -140 {
lab=VSS}
N 420 -150 420 -140 {
lab=VSS}
N 570 -150 570 -140 {
lab=VSS}
C {bus_tap.sym} 370 -300 3 0 {name=l1 lab=[3]}
C {bus_tap.sym} 260 -300 3 0 {name=l2 lab=[4]}
C {bus_tap.sym} 410 -300 1 0 {name=l3 lab=[7:4]}
C {bus_tap.sym} 290 -300 1 0 {name=l4 lab=[11:8]}
C {bus_tap.sym} 180 -300 1 0 {name=l5 lab=[3:0]}
C {bus_tap.sym} 480 -300 3 0 {name=l7 lab=[10]}
C {bus_tap.sym} 600 -300 3 0 {name=l8 lab=[0]}
C {res.sym} 590 -440 0 0 {name=R1
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 470 -440 0 0 {name=R2
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 360 -440 0 0 {name=R3
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 250 -440 0 0 {name=R4
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 190 -180 0 0 {name=R5[3:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 300 -180 0 0 {name=R6[3:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 420 -180 0 0 {name=R7[3:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 570 -180 0 0 {name=R8[3:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {lab_pin.sym} 140 -300 0 0 {name=p1 sig_type=std_logic lab=DATA[15:0]
}
C {bus_tap.sym} 560 -300 1 0 {name=l6 lab=[15:12]}
C {lab_pin.sym} 250 -480 0 0 {name=p2 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 360 -480 0 0 {name=p3 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 470 -480 0 0 {name=p4 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 590 -480 0 0 {name=p5 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 190 -140 0 0 {name=p6 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 300 -140 0 0 {name=p7 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 420 -140 0 0 {name=p8 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 570 -140 0 0 {name=p9 sig_type=std_logic lab=VSS
}
