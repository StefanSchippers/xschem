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
T {gTAG - Power Supply Unit} 6570 -4290 2 1 0.666667 0.666667 {}
T {gTAG-psu.sch} 6460 -4240 2 1 0.333333 0.333333 {}
T {4} 6460 -4210 2 1 0.333333 0.333333 {}
T {5} 6610 -4210 2 1 0.333333 0.333333 {}
T {$Revision$} 6860 -4240 2 1 0.333333 0.333333 {}
T {Stefan Petersen (spe@stacken.kth.se)} 6860 -4210 2 1 0.333333 0.333333 {}
T {"low-bat" voltage is calculated} 5840 -4940 2 1 0.333333 0.333333 {}
T {according to the following formula:} 5840 -4920 2 1 0.333333 0.333333 {}
T {Vlo = Vlbi*((R32+R33)/R33), where Vlbi is 1.2V} 5840 -4880 2 1 0.333333 0.333333 {}
T {Here: Vlo = 1.2*139/39 = 4.3V} 5840 -4840 2 1 0.333333 0.333333 {}






C {title-A2.sym} 4830 -4200 0 0 {}
C {in-1.sym} 5470 -5230 0 0 {name=usb_power
lab=usb_power
device=INPUT
}
C {in-1.sym} 5470 -5160 0 0 {name=jtag_power
lab=jtag_power
device=INPUT
}
C {max882.sym} 6100 -5100 0 0 {name=U30
device=MAX882
footprint=SO8
}
C {3.3V-plus-1.sym} 6410 -5190 0 0 {}
C {gnd-1.sym} 6180 -4920 0 0 {}
C {capacitor-2.sym} 6330 -5140 1 0 {name=C32
value=2.2u
footprint=sm1206
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5870 -5130 1 0 {name=C31
value=2.2u
footprint=sm1206
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {resistor-2.sym} 5580 -5230 0 0 {name=R30
value=0
footprint=sm0805
device=RESISTOR
}
C {resistor-2.sym} 5580 -5160 0 0 {name=R31
value=0
footprint=sm0805
device=RESISTOR
}
C {capacitor-1.sym} 5780 -5130 1 0 {name=C30
value=100n
footprint=sm1206
device=CAPACITOR
symversion=0.1
}
C {copyleft.sym} 4860 -4220 0 0 {name=C30
device=none
}
C {resistor-2.sym} 6010 -5030 3 0 {name=R33
value=39k
footprint=sm0805
device=RESISTOR
}
C {resistor-2.sym} 6010 -5140 3 0 {name=R32
value=100k
footprint=sm0805
device=RESISTOR
}
C {resistor-2.sym} 6420 -5260 3 0 {name=R34
value=10k
footprint=sm0805
device=RESISTOR
}
C {out-1.sym} 6500 -5230 0 0 {name=pon_reset#
lab=pon_reset#
device=OUTPUT
}
C {3.3V-plus-1.sym} 6390 -5370 0 0 {}

N 6310 -5180 6430 -5180 {}
N 6430 -5190 6430 -5180 {}
N 6350 -5180 6350 -5140 {}
N 5890 -5240 5890 -5130 {}
N 5800 -5000 6350 -5000 {}
N 6350 -5050 6350 -5000 {}
N 5890 -5040 5890 -5000 {}
N 6160 -5100 6160 -5000 {}
N 6210 -5100 6210 -5000 {}
N 6190 -5000 6190 -4950 {}
N 5670 -5240 6100 -5240 {}
N 6000 -5280 6100 -5280 {}
N 5530 -5170 5580 -5170 {}
N 5530 -5240 5580 -5240 {}
N 5800 -5040 5800 -5000 {}
N 5800 -5240 5800 -5130 {}
N 5700 -5240 5700 -5170 {}
N 5670 -5170 5700 -5170 {}
N 6230 -5100 6230 -5000 {}
N 6080 -5170 6100 -5170 {}
N 6080 -5170 6080 -5130 {}
N 6000 -5130 6080 -5130 {}
N 6000 -5140 6000 -5120 {}
N 6000 -5030 6000 -5000 {}
N 6000 -5240 6000 -5230 {}
N 6000 -5280 6000 -5240 {}
N 6310 -5240 6500 -5240 {}
N 6410 -5260 6410 -5240 {}
N 6410 -5370 6410 -5350 {}

