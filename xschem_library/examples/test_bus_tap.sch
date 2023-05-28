v {xschem version=3.4.0 file_version=1.2
}
G {}
K {}
V {}
S {}
E {}
P 4 7 740 -280 740 -310 730 -310 740 -337.5 750 -310 740 -310 740 -280 {fill=true}
T {Specifying @lab
will result in net
@#1:net_name} 750 -300 0 0 0.4 0.4 {name=l6 layer=4}
T {Title symbol has embedded TCL command
to enable show_pin_net_names } 180 -110 0 0 0.4 0.4 { layer=7}
N 280 -380 1020 -380 {bus=true
lab=DATA[15:0]}
N 500 -520 500 -390 {
lab=DATA[3]}
N 390 -520 390 -390 {
lab=DATA[13]}
N 560 -370 560 -220 {
lab=DATA[7:4]}
N 440 -370 440 -220 {
lab=DATA[11:8]}
N 330 -370 330 -220 {
lab=DATA[3:0]}
N 710 -370 710 -220 {
lab=DATA[15:12]}
N 610 -520 610 -390 {
lab=DATA[10]}
N 730 -520 730 -390 {
lab=DATA[0]}
N 950 -490 950 -480 {
lab=VCC}
N 610 -590 610 -580 {
lab=VCC}
N 500 -590 500 -580 {
lab=VCC}
N 390 -590 390 -580 {
lab=VCC}
N 330 -160 330 -150 {
lab=VSS}
N 440 -160 440 -150 {
lab=VSS}
N 560 -160 560 -150 {
lab=VSS}
N 710 -160 710 -150 {
lab=VSS}
N 300 -440 300 -380 {
lab=DATA[15:0]}
N 300 -510 300 -500 {
lab=VCC}
N 950 -420 950 -380 {
lab=DATA[15:0]}
N 280 -810 940 -810 {bus=true
lab=DIN[15..0]}
N 390 -800 390 -740 {
lab=0}
N 610 -800 610 -740 {
lab=DIN[4..1]}
N 390 -680 390 -660 {
lab=VSS}
N 610 -680 610 -660 {
lab=VSS}
N 810 -800 810 -740 {
lab=5}
N 810 -680 810 -660 {
lab=VSS}
N 1110 -620 1770 -620 {bus=true
lab=ADD[3:0],ENAB,CK}
N 1220 -610 1220 -550 {
lab=ADD[3:0]}
N 1440 -610 1440 -550 {
lab=ENAB}
N 1220 -490 1220 -470 {
lab=VSS}
N 1440 -490 1440 -470 {
lab=VSS}
N 1640 -610 1640 -550 {
lab=CK}
N 1640 -490 1640 -470 {
lab=VSS}
C {bus_tap.sym} 510 -380 3 0 {name=l1 lab=[3]
net_name=true}
C {bus_tap.sym} 400 -380 3 0 {name=l2 lab=[13]
net_name=true}
C {bus_tap.sym} 550 -380 1 0 {name=l3 lab=[7:4]
net_name=true}
C {bus_tap.sym} 430 -380 1 0 {name=l4 lab=[11:8]
net_name=true}
C {bus_tap.sym} 320 -380 1 0 {name=l5 lab=[3:0]
net_name=true}
C {bus_tap.sym} 620 -380 3 0 {name=l7 lab=[10]
net_name=true}
C {bus_tap.sym} 740 -380 3 0 {name=l8 lab=[0]
net_name=true}
C {res.sym} 730 -550 0 0 {name=R1
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 610 -550 0 0 {name=R2
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 500 -550 0 0 {name=R3
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 390 -550 0 0 {name=R4
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
C {lab_pin.sym} 300 -510 0 0 {name=p10 sig_type=std_logic lab=VCC
}
C {bus_tap.sym} 700 -380 1 0 {name=l6 lab=[15:12]
net_name=true}
C {lab_pin.sym} 390 -590 0 0 {name=p2 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 500 -590 0 0 {name=p3 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 610 -590 0 0 {name=p4 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 950 -490 0 0 {name=p5 sig_type=std_logic lab=VCC
}
C {lab_pin.sym} 330 -150 0 0 {name=p6 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 440 -150 0 0 {name=p7 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 560 -150 0 0 {name=p8 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 710 -150 0 0 {name=p9 sig_type=std_logic lab=VSS
}
C {res.sym} 300 -470 0 0 {name=R9[15:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {lab_pin.sym} 280 -380 0 0 {name=p1 sig_type=std_logic lab=DATA[15:0]
}
C {res.sym} 950 -450 0 0 {name=R10[15:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {title.sym} 160 -30 0 0 {name=l9 

author="tcleval(Stefan Schippers[
  if \{$show_pin_net_names == 0\} \{
    set show_pin_net_names 1
    xschem update_all_sym_bboxes
  \}]
)"}
C {lab_pin.sym} 280 -810 0 0 {name=p11 sig_type=std_logic lab=DIN[15..0]
}
C {bus_tap.sym} 380 -810 1 0 {name=l10 lab=0
net_name=true}
C {bus_tap.sym} 600 -810 1 0 {name=l11 lab=[4..1]
net_name=true}
C {res.sym} 610 -710 0 0 {name=R11[3:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 390 -710 0 0 {name=R12
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {lab_pin.sym} 390 -660 0 0 {name=p12 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 610 -660 0 0 {name=p13 sig_type=std_logic lab=VSS
}
C {bus_tap.sym} 800 -810 1 0 {name=l12 lab=5
net_name=true}
C {res.sym} 810 -710 0 0 {name=R13
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {lab_pin.sym} 810 -660 0 0 {name=p14 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 1110 -620 0 0 {name=p15 sig_type=std_logic lab=ADD[3:0],ENAB,CK
}
C {bus_tap.sym} 1210 -620 1 0 {name=l13 lab=[3:0]
net_name=true}
C {bus_tap.sym} 1430 -620 1 0 {name=l14 lab=ENAB
net_name=true}
C {res.sym} 1440 -520 0 0 {name=R1[3:0]
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {res.sym} 1220 -520 0 0 {name=R5
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {lab_pin.sym} 1220 -470 0 0 {name=p16 sig_type=std_logic lab=VSS
}
C {lab_pin.sym} 1440 -470 0 0 {name=p17 sig_type=std_logic lab=VSS
}
C {bus_tap.sym} 1630 -620 1 0 {name=l15 lab=CK
net_name=true}
C {res.sym} 1640 -520 0 0 {name=R6
value=1k
footprint=1206
device=resistor
m=1
net_name=true}
C {lab_pin.sym} 1640 -470 0 0 {name=p18 sig_type=std_logic lab=VSS
}
