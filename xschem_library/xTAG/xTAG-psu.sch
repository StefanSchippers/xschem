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
G {}
V {}
S {}
E {}
T {This schematic is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
This schematic is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
(C) 2001 Stefan Petersen (spe@stacken.kth.se)
} 20 -310 0 0 0.3 0.3 {}
N 330 -410 1210 -410 {lab=GND}
N 1030 -470 1030 -410 {lab=GND}
N 990 -470 990 -410 {lab=GND}
N 790 -470 790 -410 {lab=GND}
N 510 -430 510 -410 {lab=GND}
N 330 -430 330 -410 {lab=GND}
N 410 -430 410 -410 {lab=GND}
N 510 -540 510 -490 {lab=#net1}
N 510 -510 640 -510 {lab=#net1}
N 640 -570 640 -510 {lab=#net1}
N 640 -570 710 -570 {lab=#net1}
N 330 -610 710 -610 {lab=#net2}
N 330 -610 330 -490 {lab=#net2}
N 410 -610 410 -490 {lab=#net2}
N 510 -610 510 -590 {lab=#net2}
N 510 -650 710 -650 {lab=#net2}
N 510 -650 510 -610 {lab=#net2}
N 250 -610 330 -610 {lab=#net2}
N 250 -650 250 -570 {lab=#net2}
N 170 -570 190 -570 {lab=jtag_power}
N 170 -650 190 -650 {lab=usb_power}
N 1210 -430 1210 -410 {lab=GND}
N 1110 -570 1250 -570 {lab=V3_3}
N 1210 -570 1210 -490 {lab=V3_3}
N 1110 -650 1350 -650 {lab=pon_reset_}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {max882.sym} 910 -610 0 0 {name=U30
device=MAX882 
footprint=SO8}
C {res.sym} 510 -560 0 0 {name=R32
value=100k
footprint=sm0805
device=resistor
m=1}
C {res.sym} 510 -460 0 0 {name=R33
value=39k
footprint=sm0805
device=resistor
m=1}
C {capa.sym} 410 -460 0 0 {name=C31
m=1
value=2.2u
footprint=sm1206
device="POLARIZED_CAPACITOR"}
C {capa.sym} 330 -460 0 0 {name=C30
m=1
value=100n
footprint=sm1206
device="CAPACITOR"}
C {res.sym} 220 -650 3 1 {name=R30
value=0
footprint=sm0805
device=resistor
m=1}
C {res.sym} 220 -570 3 1 {name=R31
value=0
footprint=sm0805
device=resistor
m=1}
C {ipin.sym} 170 -650 0 0 {name=p1 lab=usb_power}
C {ipin.sym} 170 -570 0 0 {name=p1 lab=jtag_power}
C {capa.sym} 1210 -460 0 0 {name=C32
m=1
value=2.2u
footprint=sm1206
device="POLARIZED_CAPACITOR"}
C {vdd.sym} 1250 -570 0 0 {name=l1 lab=V3_3}
C {opin.sym} 1350 -650 0 0 {name=p1 lab=pon_reset_}
C {gnd.sym} 710 -410 0 0 {name=l1 lab=GND}
