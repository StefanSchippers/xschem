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
T {Lightning detector} 5910 -5930 0 0 0.533333 0.533333 { vcenter=true}
T {techman@dingoblue.net.au} 6100 -5830 2 1 0.266667 0.266667 {}
T {1.00} 6100 -5860 2 1 0.266667 0.266667 {}
T {lightning.sch} 5710 -5860 2 1 0.266667 0.266667 {}






C {inductor-1.sym} 5340 -6060 3 0 {name=L1
value=10mH
device=INDUCTOR
symversion=0.1
}
C {inductor-1.sym} 5340 -6310 3 0 {name=L2
value=10mH
device=INDUCTOR
symversion=0.1
}
C {title-A4.sym} 5250 -5820 0 0 {}
C {capacitor-2.sym} 5380 -6140 0 0 {name=C2
value=0.01uF
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5360 -6110 1 0 {name=C1
value=680pf
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5590 -6280 0 0 {name=C3
value=.01uF
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5810 -6210 1 0 {name=C4
value=100uF
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5950 -6210 1 0 {name=C5
value=.005uF
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {resistor-1.sym} 5460 -6330 1 0 {name=R1
value=180k
device=RESISTOR
}
C {resistor-1.sym} 5540 -6480 1 0 {name=R2
value=3.9k
device=RESISTOR
}
C {resistor-1.sym} 6190 -6490 3 0 {name=R7
value=47R
device=RESISTOR
}
C {resistor-1.sym} 5690 -6160 3 0 {name=R3
value=22k
device=RESISTOR
}
C {resistor-1.sym} 5850 -6240 0 0 {name=R5
value=2.2K
device=RESISTOR
}
C {resistor-1.sym} 6060 -6000 3 0 {name=R6
value=2.7k
device=RESISTOR
}
C {resistor-variable-1.sym} 5690 -6360 3 0 {name=R4
value=20k
device=VARIABLE_RESISTOR
}
C {out-1.sym} 6220 -6290 0 0 {name=lamp(1)
lab=lamp(1)
device=OUTPUT
}
C {out-1.sym} 6220 -6190 0 0 {name=lamp(2)
lab=lamp(2)
device=OUTPUT
}
C {in-1.sym} 6290 -6610 2 0 {name=bat(+3v)
lab=bat(+3v)
device=INPUT
}
C {in-1.sym} 6290 -6010 2 0 {name=bat(0v)
lab=bat(0v)
device=INPUT
}
C {in-1.sym} 5320 -6530 1 0 {name=A1
lab=A1
device=INPUT
}
C {capacitor-2.sym} 6090 -6410 1 0 {name=C6
value=1uF
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {diode-1.sym} 5760 -6470 1 0 {name=D1
value=1N914
device=DIODE
}
C {2N4401.sym} 5480 -6110 0 0 {name=Q1
value=2N4401
}
C {2N4403.sym} 5710 -6350 2 1 {name=Q2
value=2N4403
}
C {2N4401.sym} 5980 -6200 0 0 {name=Q3
value=2N4401
}
C {2N4401.sym} 6130 -6050 0 0 {name=Q4
value=2N4401
}

N 5330 -6310 5330 -6150 {}
N 5330 -6060 5330 -6000 {}
N 5380 -6160 5380 -6110 {}
N 5380 -6020 5380 -6000 {}
N 5330 -6160 5380 -6160 {}
N 5470 -6160 5480 -6160 {}
N 5550 -6390 5550 -6210 {}
N 5330 -6470 5330 -6400 {}
N 5330 -6000 6230 -6000 {}
N 5550 -6110 5550 -6000 {}
N 5550 -6000 6050 -6000 {}
N 5470 -6350 5470 -6330 {}
N 5470 -6240 5470 -6160 {}
N 6200 -6050 6200 -6000 {}
N 5550 -6600 6230 -6600 {}
N 5550 -6600 5550 -6480 {}
N 6180 -6600 6180 -6580 {}
N 6180 -6490 6180 -6300 {}
N 6180 -6300 6220 -6300 {}
N 5470 -6350 5550 -6350 {}
N 5680 -6100 6130 -6100 {}
N 5680 -6160 5680 -6100 {}
N 6050 -6100 6050 -6090 {}
N 6050 -6200 6050 -6100 {}
N 6050 -6480 6050 -6300 {}
N 5550 -6300 5590 -6300 {}
N 5680 -6360 5680 -6250 {}
N 5630 -6480 5630 -6410 {}
N 5630 -6480 6180 -6480 {}
N 5780 -6250 5850 -6250 {}
N 6200 -6200 6220 -6200 {}
N 6200 -6200 6200 -6150 {}
N 5970 -6250 5970 -6210 {}
N 5970 -6120 5970 -6100 {}
N 5780 -6350 5830 -6350 {}
N 5830 -6350 5830 -6210 {}
N 5830 -6120 5830 -6100 {}
N 6110 -6600 6110 -6410 {}
N 6110 -6320 6110 -6000 {}
N 5940 -6250 5980 -6250 {}
N 5680 -6300 5710 -6300 {}
N 5680 -6480 5680 -6450 {}
N 5780 -6480 5780 -6470 {}
N 5780 -6380 5780 -6350 {}

