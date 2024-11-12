v {xschem version=3.4.5 file_version=1.2
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
K {type=subcircuit
format="@name @pinlist @symname"
template="name=x1"
}
V {}
S {}
E {}
P 4 5 280 -580 1070 -580 1070 -130 280 -130 280 -580 {dash=3}
T {( @#0:resolved_net )} 280 -445 0 1 0.2 0.2 {name=p161 layer=15}
T {( @#0:resolved_net )} 280 -305 0 1 0.2 0.2 {name=p1 layer=15}
T {( @#0:resolved_net )} 1130 -355 0 1 0.2 0.2 {name=p20 layer=15}
T {@name} 320 -640 0 0 0.8 0.8 {}
T {@symname} 1000 -640 0 1 0.8 0.8 {}
N 660 -190 660 -170 { lab=0}
N 440 -220 620 -220 { lab=GN1}
N 400 -270 400 -250 { lab=GN1}
N 400 -350 400 -330 { lab=VCC}
N 400 -190 400 -170 { lab=0}
N 580 -280 740 -280 { lab=S}
N 660 -280 660 -250 { lab=S}
N 580 -380 580 -340 { lab=#net1}
N 740 -380 740 -340 { lab=G}
N 620 -450 620 -420 { lab=#net1}
N 580 -420 620 -420 { lab=#net1}
N 580 -420 580 -380 { lab=#net1}
N 580 -500 580 -480 { lab=VCC}
N 940 -330 1070 -330 { lab=OUT}
N 940 -500 940 -440 { lab=VCC}
N 740 -410 900 -410 { lab=G}
N 400 -250 440 -250 { lab=GN1}
N 440 -250 440 -220 { lab=GN1}
N 740 -500 740 -480 { lab=VCC}
N 620 -450 710 -450 { lab=#net1}
N 940 -190 940 -170 { lab=0}
N 940 -380 940 -250 { lab=OUT}
N 620 -220 620 -210 { lab=GN1}
N 620 -210 730 -210 { lab=GN1}
N 730 -220 730 -210 { lab=GN1}
N 730 -220 900 -220 { lab=GN1}
N 740 -420 740 -380 { lab=G}
N 790 -410 790 -340 { lab=G}
N 910 -340 940 -340 { lab=OUT}
C {nmos4.sym} 640 -220 0 0 {name=M1 model=nmos w=4u l=0.4u m=1}
C {lab_pin.sym} 660 -220 0 1 {name=p2 lab=0}
C {lab_pin.sym} 660 -170 0 0 {name=p6 lab=0}
C {nmos4.sym} 420 -220 0 1 {name=M2 model=nmos w=4u l=0.4u m=1}
C {lab_pin.sym} 400 -220 0 0 {name=p7 lab=0}
C {isource.sym} 400 -300 0 0 {name=I0 value=30u}
C {lab_pin.sym} 400 -350 0 0 {name=p9 lab=VCC}
C {lab_pin.sym} 400 -170 0 0 {name=p16 lab=0}
C {nmos4.sym} 560 -310 0 0 {name=M3 model=nmos w=1.5u l=0.2u m=1}
C {lab_pin.sym} 580 -310 0 1 {name=p17 lab=0}
C {nmos4.sym} 760 -310 0 1 {name=M4 model=nmos w=1.5u l=0.2u m=1}
C {lab_pin.sym} 740 -310 0 0 {name=p18 lab=0}
C {pmos4.sym} 600 -450 0 1 {name=M5 model=pmos w=6u l=0.3u m=1}
C {lab_pin.sym} 580 -500 0 0 {name=p19 lab=VCC}
C {pmos4.sym} 720 -450 0 0 {name=M6 model=pmos w=6u l=0.3u m=1}
C {lab_pin.sym} 740 -500 0 0 {name=p21 lab=VCC}
C {lab_pin.sym} 580 -450 0 0 {name=p23 lab=VCC}
C {lab_pin.sym} 740 -450 0 1 {name=p33 lab=VCC}
C {lab_wire.sym} 570 -220 0 0 {name=l2 lab=GN1}
C {ipin.sym} 280 -420 0 0 {name=p161 lab=PLUS}
C {ipin.sym} 280 -280 0 0 {name=p1 lab=MINUS}
C {opin.sym} 1070 -330 0 0 {name=p20 lab=OUT}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {lab_pin.sym} 780 -310 0 1 {name=p3 lab=PLUS}
C {lab_pin.sym} 540 -310 0 0 {name=p4 lab=MINUS}
C {pmos4.sym} 920 -410 0 0 {name=M8 model=pmos w=6u l=0.4u m=1}
C {lab_pin.sym} 940 -410 0 1 {name=p15 lab=VCC}
C {lab_pin.sym} 940 -500 0 0 {name=p22 lab=VCC}
C {spice_probe.sym} 490 -220 0 0 {name=p27 attrs=""}
C {spice_probe.sym} 640 -450 0 0 {name=p28 attrs=""}
C {nmos4.sym} 920 -220 0 0 {name=M7 model=nmos w=4u l=0.4u m=1}
C {lab_pin.sym} 940 -220 0 1 {name=p5 lab=0}
C {lab_pin.sym} 940 -170 0 1 {name=p8 lab=0}
C {res.sym} 880 -340 1 0 {name=R1
value=5k
footprint=1206
device=resistor
m=1}
C {capa.sym} 820 -340 3 0 {name=C1
m=1
value=150f
footprint=1206
device="ceramic capacitor"}
C {parax_cap.sym} 460 -210 0 0 {name=C2 gnd=0 value=200f m=1}
C {spice_probe.sym} 630 -280 0 0 {name=p10 attrs=""}
C {lab_pin.sym} 660 -260 0 0 {name=p11 lab=S}
C {lab_pin.sym} 740 -380 0 0 {name=p12 lab=G}
