v {xschem version=3.4.4 file_version=1.2
*
* This file is part of XSCHEM,
* a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
* simulation.
* Copyright (C) 1998-2024 Stefan Frederik Schippers
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
}
K {}
G {}
V {}
S {}
E {}
T {Two stage amplifier SPICE playpen} 3610 -4680 2 1 0.666667 0.666667 {}
T {Stuart Brorson -- sdb@cloud9.net} 4000 -4610 2 1 0.333333 0.333333 {}






C {transistor.sym} 3190 -4920 0 0 {name=Q1
model-name=2N3904
device=NPN_TRANSISTOR
}
C {resistor-1.sym} 2810 -4960 0 0 {name=R5
value=10
device=RESISTOR
}
C {resistor-1.sym} 3110 -4990 3 0 {name=R1
value=28K
device=RESISTOR
}
C {resistor-1.sym} 3160 -4820 3 0 {name=R2
value=2K
device=RESISTOR
}
C {resistor-1.sym} 3260 -4790 3 0 {name=RE1
value=100
device=RESISTOR
}
C {resistor-1.sym} 3800 -5460 3 0 {name=RC2
value=1K
device=RESISTOR
}
C {resistor-1.sym} 4080 -5220 3 0 {name=RL
value=100K
device=RESISTOR
}
C {capacitor-1.sym} 3930 -5390 0 0 {name=Cout
value=2.2uF
device=CAPACITOR
symversion=0.1
}
C {capacitor-1.sym} 3370 -4790 3 0 {name=CE1
value=1pF
device=CAPACITOR
symversion=0.1
}
C {gnd-1.sym} 3140 -4740 0 0 {}
C {gnd-1.sym} 3240 -4740 0 0 {}
C {gnd-1.sym} 3340 -4740 0 0 {}
C {gnd-1.sym} 4060 -5120 0 0 {}
C {vsin-1.sym} 2720 -4830 0 0 {name=Vinput
lab=Vinput
value=DC\\ 1.6V\\ AC\\ 10MV\\ SIN(0\\ 1MV\\ 1KHZ)
device=vsin
footprint=none
}
C {vdc-1.sym} 4070 -4870 0 0 {name=VCC
value=DC\\ 15V
device=VOLTAGE_SOURCE
footprint=none
}
C {vcc-1.sym} 4080 -5010 0 0 {}
C {vcc-1.sym} 3770 -5570 0 0 {}
C {gnd-1.sym} 4090 -4820 0 0 {}
C {gnd-1.sym} 2740 -4760 0 0 {}
C {title-B.sym} 2600 -4600 0 0 {}
C {spice-model-1.sym} 2650 -5550 0 0 {name=A1
model-name=2N3904
file=./models/2N3904.mod
device=model
}
C {spice-include-1.sym} 2650 -5440 0 0 {name=A2
file=./Simulation.cmd
device=include
}
C {spice-directive-1.sym} 2650 -5300 0 0 {name=A3
value=.options\\ TEMP=25
device=directive
}
C {capacitor-1.sym} 2940 -4950 0 0 {name=C1
value=2.2uF
device=CAPACITOR
symversion=0.1
}
C {transistor.sym} 3730 -5260 0 0 {name=Q2
model-name=2N3904
device=NPN_TRANSISTOR
}
C {gnd-1.sym} 3780 -4890 0 0 {}
C {vcc-1.sym} 3080 -5100 0 0 {}
C {resistor-1.sym} 3260 -5350 3 0 {name=RC1
value=3.3K
device=RESISTOR
}
C {resistor-1.sym} 3800 -4950 3 0 {name=RE2
value=100
device=RESISTOR
}
C {vcc-1.sym} 3230 -5480 0 0 {}
C {capacitor-1.sym} 3920 -4950 3 0 {name=CE2
value=1pF
device=CAPACITOR
symversion=0.1
}
C {gnd-1.sym} 3890 -4890 0 0 {}
C {resistor-1.sym} 3330 -5300 0 0 {name=R8
value=1
device=RESISTOR
}
C {resistor-1.sym} 3670 -5470 3 0 {name=R3
value=28K
device=RESISTOR
}
C {resistor-1.sym} 3640 -5160 3 0 {name=R4
value=2.8K
device=RESISTOR
}
C {gnd-1.sym} 3620 -5080 0 0 {}
C {vcc-1.sym} 3640 -5580 0 0 {}
C {capacitor-1.sym} 3500 -5290 0 0 {name=C2
value=2.2uF
device=CAPACITOR
symversion=0.1
}

N 3250 -4920 3250 -4880 {}
N 3350 -4910 3350 -4880 {}
N 3790 -5410 3930 -5410 {lab=VColl2 }
C {lab_wire.sym} 3850 -5410 0 1 {lab=VColl2 }
N 4070 -5410 4070 -5310 {}
N 3790 -5570 3790 -5550 {}
N 4100 -5010 4100 -4990 {}
N 2750 -4830 2750 -4790 {}
N 2750 -4970 2750 -4950 {}
N 3150 -4820 3150 -4770 {}
N 3250 -4790 3250 -4770 {}
N 3350 -4790 3350 -4770 {}
N 4070 -5220 4070 -5150 {}
N 4100 -4870 4100 -4850 {}
N 4020 -5410 4130 -5410 {lab=Vout }
C {lab_wire.sym} 4130 -5410 0 1 {lab=Vout }
N 2700 -4970 2810 -4970 {lab=Vin }
C {lab_wire.sym} 2700 -4970 0 1 {lab=Vin }
N 3100 -4990 3100 -4970 {}
N 3150 -4970 3150 -4910 {}
N 2900 -4970 2940 -4970 {}
N 3790 -5460 3790 -5360 {}
N 3100 -5100 3100 -5080 {}
N 3790 -4950 3790 -4920 {}
N 3790 -5260 3790 -5040 {}
N 3250 -5350 3250 -5020 {}
N 3250 -5480 3250 -5440 {}
N 3030 -4970 3190 -4970 {lab=Vbase1 }
C {lab_wire.sym} 3100 -4970 0 1 {lab=Vbase1 }
N 3900 -4950 3900 -4920 {}
N 3900 -5100 3900 -5040 {}
N 3790 -5100 3900 -5100 {lab=Vem2 }
C {lab_wire.sym} 3800 -5100 0 1 {lab=Vem2 }
N 3250 -4910 3350 -4910 {lab=Vem1 }
C {lab_wire.sym} 3270 -4910 0 1 {lab=Vem1 }
N 3590 -5310 3730 -5310 {lab=Vbase2 }
C {lab_wire.sym} 3690 -5310 0 1 {lab=Vbase2 }
N 3250 -5310 3330 -5310 {lab=Vcoll1 }
C {lab_wire.sym} 3300 -5310 0 1 {lab=Vcoll1 }
N 3630 -5160 3630 -5110 {}
N 3630 -5310 3630 -5250 {}
N 3660 -5580 3660 -5560 {}
N 3420 -5310 3500 -5310 {}
N 3660 -5470 3660 -5310 {}

