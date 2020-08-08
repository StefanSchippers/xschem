v {xschem version=2.9.7 file_version=1.1}
G {}
V {}
S {}
E {}
T {Components inside this box are inside the device package} 3020 -5690 2 1 0.333333 0.333333 {}
T {SPICE model of Agilent MSA-2643} 3670 -4936.67 0 0 0.666667 0.666667 {}
T {SDB -- 3.31.2003} 4060 -4848.33 0 0 0.333333 0.333333 {}

L 4 3000 -5010 3950 -5010 {}
L 4 3950 -5720 3950 -5010 {}
L 4 3000 -5720 3950 -5720 {}
L 4 3000 -5720 3000 -5010 {}





C {resistor-1.sym} 3230 -5490 3 0 {name=R1
value=560
device=RESISTOR
}
C {resistor-1.sym} 3230 -5260 3 0 {name=R2
value=660
device=RESISTOR
}
C {resistor-1.sym} 3390 -5220 3 0 {name=R3
value=250
device=RESISTOR
}
C {resistor-1.sym} 3680 -5160 3 0 {name=R4
value=5
device=RESISTOR
}
C {Q_Model.sym} 3300 -5370 0 0 {name=X1
model-name=Q1_MSA26F
file=Q1.cir
device=NPN_TRANSISTOR_subcircuit
}
C {Q_Model.sym} 3590 -5290 0 0 {name=X2
model-name=Q2_MSA26F
file=Q2.cir
device=NPN_TRANSISTOR_subcircuit
}
C {gnd-1.sym} 3990 -5080 0 0 {}
C {capacitor-1.sym} 3770 -5160 3 0 {name=C1
value=4pF
device=CAPACITOR
symversion=0.1
}
C {vdc-1.sym} 4050 -5130 0 0 {name=Vpwr
value=DC\\ 5V
device=VOLTAGE_SOURCE
footprint=none
}
C {vac-1.sym} 2720 -5230 0 0 {name=Vin
value=dc\\ 0\\ ac\\ 1
device=vac
footprint=none
}
C {resistor-1.sym} 2770 -5400 0 0 {name=Rth_in
value=50
device=RESISTOR
}
C {resistor-1.sym} 4230 -5390 3 0 {name=RL
value=50
device=RESISTOR
}
C {gnd-1.sym} 2740 -5170 0 0 {}
C {gnd-1.sym} 4070 -5050 0 0 {}
C {inductor-1.sym} 4090 -5460 3 0 {name=Lc
value=22nH
device=INDUCTOR
symversion=0.1
}
C {gnd-1.sym} 4210 -5330 0 0 {}
C {resistor-1.sym} 4090 -5300 3 0 {name=Rc
value=50
device=RESISTOR
}
C {capacitor-1.sym} 4110 -5620 0 0 {name=Cout
value=47pF
device=CAPACITOR
symversion=0.1
}
C {capacitor-1.sym} 2900 -5390 0 0 {name=Cin
value=47pF
device=CAPACITOR
symversion=0.1
}
C {title-B.sym} 2650 -4820 0 0 {}
C {spice-include-1.sym} 2740 -5760 0 0 {name=A1
file=./Simulation.cmd
device=include
}
C {inductor-1.sym} 3090 -5130 0 0 {name=L4
value=0.386nH
device=INDUCTOR
symversion=0.1
}
C {inductor-1.sym} 3800 -5130 0 0 {name=L6
value=0.313nH
device=INDUCTOR
symversion=0.1
}
C {inductor-1.sym} 3090 -5400 0 0 {name=L1
value=0.833nH
device=INDUCTOR
symversion=0.1
}
C {inductor-1.sym} 3760 -5630 0 0 {name=L7
value=0.407nH
device=INDUCTOR
symversion=0.1
}
C {gnd-1.sym} 2940 -5080 0 0 {}
C {capacitor-1.sym} 3060 -5230 3 0 {name=Cpara1
value=0.158pF
device=CAPACITOR
symversion=0.1
}
C {capacitor-1.sym} 3930 -5240 3 0 {name=Cpara2
value=0.155pF
device=CAPACITOR
symversion=0.1
}
C {resistor-1.sym} 3180 -5080 2 0 {name=Rbogus1
value=1e-12
device=RESISTOR
}

N 3220 -5490 3220 -5350 {}
N 3310 -5440 3220 -5440 {}
N 3380 -5380 3380 -5310 {}
N 3600 -5360 3380 -5360 {}
N 3670 -5250 3670 -5300 {}
N 3220 -5580 3220 -5640 {}
N 3670 -5420 3670 -5640 {}
N 3380 -5500 3380 -5640 {}
N 3670 -5160 3670 -5140 {}
N 3220 -5140 3220 -5260 {}
N 3380 -5220 3380 -5140 {}
N 3750 -5250 3750 -5270 {}
N 3750 -5270 3670 -5270 {}
N 3750 -5160 3750 -5140 {}
N 2770 -5410 2750 -5410 {}
N 2750 -5410 2750 -5350 {lab=Vsource }
C {lab_wire.sym} 2750 -5380 0 0 {lab=Vsource }
N 2750 -5200 2750 -5230 {}
N 4080 -5130 4080 -5080 {}
N 4220 -5360 4220 -5390 {}
N 2860 -5410 2900 -5410 {lab=Vin }
C {lab_wire.sym} 2880 -5410 0 0 {lab=Vin }
N 4200 -5640 4250 -5640 {lab=Vout }
C {lab_wire.sym} 4225 -5640 0 0 {lab=Vout }
N 4220 -5480 4220 -5640 {}
N 3220 -5640 3760 -5640 {}
N 4080 -5550 4080 -5640 {}
N 4080 -5460 4080 -5390 {}
N 4080 -5300 4080 -5250 {}
N 3180 -5410 3220 -5410 {}
N 2990 -5410 3090 -5410 {}
N 3850 -5640 4110 -5640 {}
N 2950 -5110 2950 -5140 {}
N 2950 -5140 3090 -5140 {}
N 3210 -5140 3800 -5140 {}
N 3890 -5140 4000 -5140 {}
N 4000 -5140 4000 -5110 {}
N 3910 -5330 3910 -5640 {}
N 3910 -5240 3910 -5140 {}
N 3040 -5320 3040 -5410 {}
N 3040 -5230 3040 -5140 {}
N 3210 -5070 3210 -5140 {}
N 3190 -5090 3190 -5140 {}
N 3190 -5090 3070 -5090 {}
N 3070 -5090 3070 -5070 {}
N 3070 -5070 3090 -5070 {}
N 3180 -5070 3210 -5070 {}
N 3190 -5140 3180 -5140 {}

