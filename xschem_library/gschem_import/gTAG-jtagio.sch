v {xschem version=2.9.7 file_version=1.1}
G {}
V {}
S {}
E {}
T {N.C} 5650 -5050 2 1 0.333333 0.333333 {}
T {gTAG - JTAG IO } 5800 -4380 2 1 0.666667 0.666667 {}
T {gTAG-jtagio.sch} 5660 -4330 2 1 0.333333 0.333333 {}
T {5} 5660 -4300 2 1 0.333333 0.333333 {}
T {5} 5800 -4300 2 1 0.333333 0.333333 {}
T {$Revision$} 6040 -4330 2 1 0.333333 0.333333 {}
T {Stefan Petersen (spe@stacken.kth.se)} 6040 -4300 2 1 0.333333 0.333333 {}
T {JTAG connector} 5890 -5200 2 1 0.833333 0.833333 {}
T {Place near the VCC pin for U20} 5720 -5520 2 1 0.5 0.5 {}






C {title-A2.sym} 4020 -4290 0 0 {}
C {in-1.sym} 4540 -5360 0 0 {name=tdo_ttl
lab=tdo_ttl
device=INPUT
}
C {in-1.sym} 4540 -5180 0 0 {name=tms_ttl
lab=tms_ttl
device=INPUT
}
C {in-1.sym} 4540 -5010 0 0 {name=tck_ttl
lab=tck_ttl
device=INPUT
}
C {in-1.sym} 4540 -4840 0 0 {name=trst_ttl
lab=trst_ttl
device=INPUT
}
C {in-1.sym} 4540 -5600 0 0 {name=jtag_power
lab=jtag_power
device=INPUT
}
C {7414-1.sym} 5420 -5480 0 1 {name=U20:1
net:14=jtag_power
net:7=GND
footprint=SO14
device=7414
}
C {7414-1.sym} 5300 -5320 0 0 {name=U20:2
net:14=jtag_power
net:7=GND
device=7414
}
C {7414-1.sym} 5300 -5140 0 0 {name=U20:3
net:14=jtag_power
net:7=GND
device=7414
}
C {7414-1.sym} 5300 -4970 0 0 {name=U20:4
net:14=jtag_power
net:7=GND
device=7414
}
C {7414-1.sym} 5300 -4800 0 0 {name=U20:5
net:14=jtag_power
net:7=GND
device=7414
}
C {out-1.sym} 4600 -5520 0 1 {name=tdi_ttl
lab=tdi_ttl
device=OUTPUT
}
C {connector8-1.sym} 5860 -5050 0 1 {name=CONN4
footprint=conn_jtag
device=CONNECTOR_8
}
C {gnd-1.sym} 5610 -5020 0 0 {}
C {copyleft.sym} 4050 -4310 0 0 {name=CONN4
device=none
}
C {capacitor-1.sym} 5920 -5630 3 0 {name=C20
value=100n
footprint=sm1206
device=CAPACITOR
symversion=0.1
}
C {gnd-1.sym} 5890 -5560 0 0 {}
C {5V-plus-1.sym} 5880 -5760 0 0 {name=C20
net:1=jtag_power
}
C {resistor-2.sym} 4750 -5670 3 0 {name=R20
value=10k
footprint=sm0805
device=RESISTOR
}
C {resistor-2.sym} 4880 -5670 3 0 {name=R21
value=10k
footprint=sm0805
device=RESISTOR
}
C {resistor-2.sym} 5010 -5670 3 0 {name=R22
value=10k
footprint=sm0805
device=RESISTOR
}
C {resistor-2.sym} 5150 -5670 3 0 {name=R23
value=10k
footprint=sm0805
device=RESISTOR
}
C {5V-plus-1.sym} 4720 -5780 0 0 {name=R23
net:1=jtag_power
}
C {5V-plus-1.sym} 4850 -5780 0 0 {name=R23
net:1=jtag_power
}
C {5V-plus-1.sym} 4980 -5780 0 0 {name=R23
net:1=jtag_power
}
C {5V-plus-1.sym} 5120 -5780 0 0 {name=R23
net:1=jtag_power
}

N 5420 -5190 5690 -5190 {}
N 5420 -5370 5500 -5370 {}
N 5500 -5370 5500 -5220 {}
N 5500 -5220 5690 -5220 {}
N 5420 -5530 5560 -5530 {}
N 5560 -5530 5560 -5250 {}
N 5560 -5250 5690 -5250 {}
N 5690 -5160 5500 -5160 {}
N 5500 -5160 5500 -5020 {}
N 5500 -5020 5420 -5020 {}
N 5690 -5130 5560 -5130 {}
N 5560 -5130 5560 -4850 {}
N 5560 -4850 5420 -4850 {}
N 5690 -5100 5620 -5100 {}
N 5620 -5100 5620 -5050 {}
N 5690 -5280 5620 -5280 {}
N 5620 -5280 5620 -5610 {}
N 5620 -5610 4600 -5610 {}
N 4600 -5530 5300 -5530 {}
N 4600 -5370 5300 -5370 {}
N 4600 -5190 5300 -5190 {}
N 4600 -5020 5300 -5020 {}
N 4600 -4850 5300 -4850 {}
N 5900 -5720 5900 -5760 {}
N 5900 -5590 5900 -5630 {}
N 4740 -5670 4740 -4850 {}
N 4870 -5670 4870 -5020 {}
N 5000 -5670 5000 -5190 {}
N 5140 -5670 5140 -5370 {}
N 4740 -5780 4740 -5760 {}
N 4870 -5780 4870 -5760 {}
N 5000 -5780 5000 -5760 {}
N 5140 -5780 5140 -5760 {}

